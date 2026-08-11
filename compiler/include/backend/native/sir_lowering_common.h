// ============================================================================
// (AR) أدواتٌ مشتركةٌ لمخفّضات SIR الأصليّة (x86-64 وAArch64، ولاحقًا RISC-V/…) في
//      ثلاثِ طبقات: (١) مسنداتُ تحليلٍ عديمةُ الحالة محايدةُ المعماريّة (لا بايتاتٌ ولا
//      سجلّات) — تمييزُ المقارنات، إدماجُ المقارنة المُغذِّية لفرعٍ شرطيّ، قراءةُ الثابت
//      الصحيح، وعقدُ الشكل؛ و(٢) `LoweringDiagnostics`: قاعدةٌ **ذاتُ حالة** تملك رمزَ
//      الخطأ وبياناتِه؛ و(٣) `LoweringDriver<Target>`: المُصدِرُ المجرَّد بنمط CRTP —
//      تتابعاتُ الثنائيّ والأحاديّ والمقارنة تعيش مرّةً واحدةً وتستدعي خطّافاتِ الهدف
//      (أسفلَ الملفّ). كان كلُّ ذلك مكرَّرًا حرفيًّا في X86SirLowering
//      وArm64SirLowering؛ توحيدُه هنا يمنع الانجرافَ بينهما. النموذجُ
//      «كلٌّ في سجلّ» لكلّ الأهداف يجعل الإدماجَ (المقارنةُ آخرَ تعليمةٍ قبل المُنهي)
//      قاعدةً مشتركة، فلا حاجةَ لـsetcc/movzx (x86) ولا لسجلّ شرطٍ وسيط (ARM64).
// (EN) Shared architecture-neutral SIR analysis for the native lowerers (x86-64,
//      AArch64, and later RISC-V/…): classify comparisons, fuse a comparison that
//      feeds a conditional branch, read an integer constant, and check instruction
//      shape — plus a stateful `LoweringDiagnostics` base that owns the error code,
//      plus `LoweringDriver<Target>`, the CRTP abstract emitter that owns the
//      binary/unary/comparison sequences and calls back into each target's hooks.
//      All of it was duplicated verbatim in both lowerers; unifying it here prevents drift.
// ============================================================================
#ifndef SAD_NATIVE_SIR_LOWERING_COMMON_H
#define SAD_NATIVE_SIR_LOWERING_COMMON_H

#include "backend/native/generated/native_diagnostics_generated.h"
#include "frontend/sir_constants.h"
#include "frontend/sir_instruction.h"
#include "frontend/sir_types.h"
#include "error_codes.h"

#include <cstddef>
#include <string>

namespace sad
{
    namespace native
    {
        namespace common
        {
            namespace sir = ::Sad::Compiler::SIR;
            namespace types = ::Sad::Types;

            // ═══════════════════════════════════════════════════════════════
            // (AR) هل الأوپكودُ من نظامِ الواجهة؟ يُقاس بحدَّي الكتلةِ المُعلَنَين في
            //   التعدادِ نفسِه (_UiOpcodeFirst/_UiOpcodeLast) لا بقائمةٍ يدويّةٍ
            //   تتعفّن: أوپكودٌ واجهيٌّ جديدٌ يدخل المدى تلقائيًّا.
            //
            //   لماذا يعنينا هنا: الخلفيّةُ السياديّةُ (بلا LLVM) لا تخفّض الواجهةَ
            //   — قرارٌ معماريٌّ مُعلَنٌ لا نقصٌ عابر (ث١). ومعرفتُه تجعل الفشلَ
            //   **مُشخَّصًا** بسببِه لا فشلًا عامًّا مبهمًا.
            // ═══════════════════════════════════════════════════════════════
            inline bool isUiOpcode(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                const auto value = static_cast<int>(op);
                return value >= static_cast<int>(OP::_UiOpcodeFirst) &&
                       value <= static_cast<int>(OP::_UiOpcodeLast);
            }

            // (AR) هل الأوپكود مقارنةٌ عدديّةٌ موقَّعة؟ (تُدمَج في BR_COND التالي عبر المعماريّات.)
            inline bool isComparison(sir::SIROpcode op)
            {
                using OP = sir::SIROpcode;
                return op == OP::EQ || op == OP::NE || op == OP::LT ||
                       op == OP::LE || op == OP::GT || op == OP::GE;
            }

            // (AR) هل التعليمةُ مقارنةُ عوائم؟ (أحدُ المعامِلَين Float.) المقارنةُ موحَّدةٌ (EQ..GE)
            //      والنوعُ في المعامِل. مقارنةُ العوائم تحتاج ucomisd/fcmp + أعلامًا لا-موقَّعةً
            //      + معالجةَ NaN ⇒ **لا تُدمَج في الفرع**: تُخفَّض كقيمةٍ (٠/١) ويختبرها الفرعُ
            //      الاحتياطيّ (test/cbnz). ⇒ findFusedComparison يستثنيها ليخفّضها مسارُ القيمة.
            inline bool isFloatCompare(const sir::SIRInstruction &inst)
            {
                if (!isComparison(inst.opcode) || inst.operands.size() != 2)
                    return false;
                return inst.operands[0].dataType == types::SadTypeKind::Float ||
                       inst.operands[1].dataType == types::SadTypeKind::Float;
            }

            // ══════════════════════════════════════════════════════════════
            // (AR) **عقدُ الشكل** (م٤، اللبنةُ الأولى): نتيجةٌ موجودةٌ وعددُ معامِلاتٍ متوقَّع.
            //   هذا الشرطُ كان مكتوبًا خامًا ٨٢ مرّةً بصيغةِ
            //   (`!inst.result || inst.operands.size() != N`) وستَّ مرّاتٍ بصيغةِ `< N` —
            //   ٨٨ موضعًا في المخفّضَين (٤٧ في x86 و٤١ في ARM64)، وكانت كلُّ معماريّةٍ جديدةٍ
            //   ستكتبه من جديد. تسميتُه هنا تجعله **عقدًا واحدًا** يستهلكه كلُّ هدف، ويمنع
            //   أن ينجرف هدفٌ إلى `!=` وآخرُ إلى `<` صامتًا.
            //
            //   هذان الشرطان مجرَّدان بلا تشخيص. ولا يُناديهما المخفّضان مباشرةً: يرثان
            //   `LoweringDiagnostics` أدناه ويُناديان `requireArity`/`requireMinArity`
            //   فيأتيهما الشرطُ وتشخيصُه معًا. وفصلُ الطبقتين مقصود — هدفٌ لا يرث القاعدةَ
            //   (أداةُ تحليلٍ مثلًا) يبقى قادرًا على استهلاك العقد بلا حالةِ فشل.
            // ══════════════════════════════════════════════════════════════
            inline bool hasResultAndArity(const sir::SIRInstruction &inst, std::size_t arity)
            {
                return inst.result && inst.operands.size() == arity;
            }

            // (AR) نظيرُه ذو الحدّ الأدنى: تعليماتٌ متغيّرةُ العدد (نداءٌ، بناءُ مصفوفة…).
            //      مفصولٌ عمدًا لا بمعاملٍ منطقيّ: `==` و`>=` عقدان مختلفان، وخلطُهما في
            //      دالّةٍ واحدةٍ يجعل موضعَ الاستدعاء يقرأ الشرطَ الخطأ بسهولة.
            inline bool hasResultAndMinArity(const sir::SIRInstruction &inst, std::size_t least)
            {
                return inst.result && inst.operands.size() >= least;
            }

            // (AR) هل المعامِلُ **علبةُ Any** (مؤشّرُ خانةِ dyn) لا قيمةً خامّة؟ بوّابةٌ واحدةٌ
            //      لأنّ تفرّقَها فخٌّ صامت: موضعٌ يبوّب على `dataType == Any` وحدَه بينما
            //      المُحمِّلُ يشترط `REGISTER` أيضًا ⇒ معامِلُ Any غيرُ REGISTER يمرّ بلا فكٍّ
            //      ولا تحويلٍ فيُنتج جوابًا خاطئًا صامتًا. الشرطُ الآن في موضعٍ واحد لكلّ
            //      بوّاباتِ معامِلاتِ **الحساب والمقارنة والتحويل**.
            //      (١) وفحصُ `result->dataType == Any` شيءٌ آخر: سؤالُه «أيُعلَّب الناتج؟»
            //          لا «أهذا المعامِلُ علبة؟» — فلا يُدمَج بها.
            //      (٢) وبوّابةُ الطباعةِ المعلَّبة (`BUILTIN_PRINT` في الهدفَين) ما تزال
            //          يدويّةً تبوّب على `dataType == Any` بلا شرطِ REGISTER — مسارٌ قديمٌ
            //          خارجَ هذه الدفعة، ولم يُبلَغ فيه معامِلٌ ثابتٌ بنوعِ Any عمليًّا،
            //          لكنّه الفخُّ نفسُه فيُضمَّ إلى هنا حين يُمَسّ.
            //      (الثابتُ لا يكون علبةً: التعليبُ يُخصَّص زمنَ التشغيل.)
            inline bool isBoxedAny(const sir::SIROperand &op)
            {
                return op.type == sir::SIROperandType::REGISTER &&
                       op.dataType == types::SadTypeKind::Any;
            }

            // (AR) هل في التعليمةِ معامِلٌ **علبةُ Any**؟ سؤالٌ مختلفٌ عن سابقِه: ذاك عن
            //      معامِلٍ بعينه، وهذا عن **شكلِ التخفيض** — لأنّ وجودَ علبةٍ واحدةٍ يعني
            //      أنّ نوعَ العمليّةِ الحقيقيَّ لا يُعرَف إلّا زمنَ التشغيل: الأماميّةُ تُصدِر
            //      ADD_I64 لـ`Any + Any` (لا صيغةً عشريّة)، فإن كانت إحدى الحمولتَين
            //      بتّاتِ double فالجمعُ الصحيحُ عليها **جوابٌ خاطئٌ صامت** لا تقريب.
            //      فمن هنا يفترق المسار: لا بفكِّ المعامِلِ بل بالفرعِ على وسمِه.
            inline bool hasBoxedAnyOperand(const sir::SIRInstruction &inst)
            {
                for (const auto &op : inst.operands)
                    if (isBoxedAny(op))
                        return true;
                return false;
            }

            // (AR) هل هذا نداءٌ لمُساعِدِ زمنِ تشغيلٍ بالاسم؟ الأماميّةُ تُصدِر مُساعِداتِ
            //      الخريطة/المجموعة بمعامِلٍ أوّلَ ثابتٍ نصّيٍّ لا FUNCTION. والشكلُ وحدَه
            //      لا يكفي: حرفيّةٌ نصّيّةٌ وقعت في خانةِ المُنادى بفسادِ SIR تحمل الشكلَ
            //      عينَه، فتُصنَّف خطأً «غيرَ مدعومةٍ بعد» — وتلميحُ ذاك الرمزِ يقول
            //      «ليس عطلًا يُبلَّغ» فيُسكِت عطبًا حقيقيًّا. فالبادئةُ هي الفارق.
            // (EN) Is this a runtime-helper call? The frontend emits map/set helpers
            //      with a string-constant callee, but so would a corrupt SIR whose
            //      callee slot holds a user literal — and misclassifying that as
            //      "unsupported yet" silences a real defect. The prefix decides.
            inline bool isRuntimeHelperCallee(const sir::SIROperand &op)
            {
                if (op.type != sir::SIROperandType::CONSTANT ||
                    op.dataType != types::SadTypeKind::String)
                    return false;
                const std::string prefix = Sad::Compiler::kRuntimeSymbolPrefix;
                return op.name.rfind(prefix, 0) == 0;
            }

            // (AR) هل المعاملُ ثابتٌ صحيح؟ يُعيد قيمتَه (الشرطُ نفسُه في كلّ الأهداف).
            inline bool isConstInt(const sir::SIROperand &op, long long &out)
            {
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::Integer)
                {
                    out = op.intValue;
                    return true;
                }
                // (AR) [توسيع اللا-موقَّع] ثابتٌ طبيعي64: نمطُ بتّاته يعيش في intValue (نفسُ اتّحادِ
                //      Integer، لا حقلَ منفصل) ⇒ نُرجعه كـi64. آمنٌ في كلِّ مواضعِ الاستدعاء: العرضُ
                //      الكامل ٦٤-بت يُحمَّل عبر movImm64/movConst، والمقارناتُ اللا-موقَّعةُ محروسةٌ
                //      ببوّابتِها المستقلّة (bothUInt64/eitherUInt64). بدونه يفشلُ أيُّ معامِلٍ ثابتٍ
                //      طبيعي64 صراحةً (const-type=13 · operand-kind=CONSTANT) في القسمة/الإزاحة/غيرِها.
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::UInt64)
                {
                    out = op.intValue;
                    return true;
                }
                // (AR) الدفعة ٨: ثابتٌ منطقيّ ⇒ صحيحٌ ٠/١ (bool يعيش قيمةً ٠/١؛ boolValue لا intValue
                //      لأنّهما في اتّحادٍ). يوحّد معالجةَ الحرفيّاتِ المنطقيّة عبر المعماريّتين.
                if (op.type == sir::SIROperandType::CONSTANT &&
                    op.dataType == types::SadTypeKind::Boolean)
                {
                    out = op.boolValue ? 1 : 0;
                    return true;
                }
                return false;
            }

            // (AR) هل نوعُ المعامِلِ **صحيحٌ موقَّعٌ** يصلح لترقيةٍ عشريّةٍ بـcvtsi2sd/scvtf؟
            //      كان الفحصُ في `loadFloatOperandPromoting` يقارن بـInteger وحدَه، فمعامِلٌ
            //      مصرَّحٌ `عدد٣٢`/`عدد٦٤` يسقط من الترقيةِ ويمرّ نمطَ بتّاتِه فيُقرأ double —
            //      وهو **جوابٌ خاطئٌ صامتٌ** لا إخفاق: `عدد٣٢ س = ٣` في طرفٍ معلَّبٍ يعطي
            //      ١٫٥e‑٣٢٣ لا ٣٫٠، وهو بعينِه العطبُ الذي وُجدت الترقيةُ لسدّه في Integer.
            //      واللا-موقَّعُ يبقى خارجًا عمدًا: تحويلُه يلزمه مسارٌ لا-موقَّعٌ لا cvtsi2sd،
            //      فإدراجُه هنا يستبدل خطأً بخطأٍ بدل أن يُخفِق بوضوح.
            // (EN) Is this operand a SIGNED integer kind eligible for float promotion?
            //      The check previously compared against Integer alone, so a declared
            //      Int32/Int64 operand silently passed its bit pattern as a double.
            //      Unsigned kinds stay out deliberately (they need an unsigned convert).
            inline bool isSignedIntKind(types::SadTypeKind k)
            {
                using K = types::SadTypeKind;
                return k == K::Integer || k == K::Int8 || k == K::Int16 || k == K::Int32 ||
                       k == K::Int64;
            }

            // (AR) يجد مقارنةً في الكتلة نتيجتُها condName تُغذّي BR_COND المُنهيَ لها، شرطَ أن
            //      تكون آخرَ تعليمةٍ قبل المُنهي (نمطُ البانِي المعتاد) — وإلّا لا إدماج. محايدٌ
            //      للمعماريّة: يخفّضها كلُّ هدفٍ بتسلسلِ (مقارنة؛ فرعٌ شرطيّ then؛ فرعٌ else).
            inline const sir::SIRInstruction *findFusedComparison(const sir::SIRBasicBlock &block,
                                                                  const std::string &condName)
            {
                const auto &is = block.instructions;
                if (is.size() < 2)
                    return nullptr;
                const sir::SIRInstruction &prev = is[is.size() - 2];
                // (AR) مقارنةُ العوائم لا تُدمَج (ucomisd/fcmp + NaN) ⇒ تُترَك لمسارِ القيمة.
                // (AR) وكذلك **مقارنةُ معامِلٍ معلَّبٍ**: نوعُها لا يُعرَف إلّا زمنَ التشغيل،
                //      والإدماجُ يُخفّضها في كلِّ هدفٍ بتسلسلِه الخاصّ (مقارنةٌ فـb.cond)
                //      خارجَ `driveComparison` ⇒ لا يبلغها الإرسالُ بالوسم. وقِيس ذلك:
                //      `إذا (خليط[١] > ٣)` بقي «نعم» في الهدفَين بعد أن صارت الصيغةُ
                //      قيمةً «كاذبة» — العيبُ نفسُه بشكلٍ ثانٍ. فتُترَك لمسارِ القيمة:
                //      يُصدِر ٠/١ في سجلٍّ ثمّ يختبره BR_COND، وهو المسارُ الذي تسلكه
                //      مقارنةُ العوائمِ أصلًا ⇒ لا تسلسلَ جديدًا في أيِّ هدف.
                if (isComparison(prev.opcode) && !isFloatCompare(prev) &&
                    !hasBoxedAnyOperand(prev) && prev.result && prev.result->name == condName)
                    return &prev;
                return nullptr;
            }

            // (AR) طبقةُ تحليل الحياة/الانسكاب — محايدةٌ للمعماريّة (لا سجلّاتٌ ولا بايتات، فقط
            //      قراءةُ SIR). كانت مكرَّرةً حرفيًّا في مخفّضَي x86 وARM64 لتقرير أيَّ المؤقّتات
            //      تُنسَك حولَ نداءٍ/طباعةٍ يدهسان سجلّاتِ الحوض (caller-saved). النموذجُ ينظّف
            //      الحوضَ لكلّ كتلة ⇒ الحياةُ ذاتُ الصلة محصورةٌ داخل الكتلة. مُسنِدُ «هل متغيّرُ
            //      ذاكرة» (isMem) يُمرَّر لأنّ تمييزَ المؤقّت السجليّ من متغيّر الذاكرة يعتمد على
            //      خريطةِ الخانات الخاصّة بكلّ مخفّض (byte-disp لـx86، فهرسٌ لـARM64).

            // (AR) هل يُقرأ vreg في تعليمةٍ لاحقةٍ (فهرسٌ > from) في الكتلة نفسِها؟ الميتُ لا يُنسَك.
            inline bool usedAfterInBlock(const sir::SIRBasicBlock &block, std::size_t from,
                                         const std::string &vreg)
            {
                for (std::size_t i = from + 1; i < block.instructions.size(); ++i)
                    for (const auto &op : block.instructions[i].operands)
                        if (op.type == sir::SIROperandType::REGISTER && op.name == vreg)
                            return true;
                return false;
            }

            // (AR) هل vreg وسيطٌ سجليٌّ مؤقّتٌ (لا متغيّرَ ذاكرة، بحسب isMem) لهذا النداء؟ وسائطُ
            //      المؤقّتات تُحمَّل من خانات الانسكاب ⇒ يجب نسكُها حتّى لو ماتت بعد النداء.
            template <typename IsMem>
            inline bool isPoolArgOfCall(const sir::SIRInstruction &call, const std::string &vreg,
                                        IsMem isMem)
            {
                for (std::size_t i = 1; i < call.operands.size(); ++i)
                {
                    const auto &a = call.operands[i];
                    if (a.type == sir::SIROperandType::REGISTER && a.name == vreg && !isMem(a.name))
                        return true;
                }
                return false;
            }

            // (AR) هل vreg مؤقّتٌ سجليٌّ (لا متغيّرَ ذاكرة، بحسب isMem) بين معاملات التعليمة؟ (للطباعة.)
            template <typename IsMem>
            inline bool isPoolOperandOf(const sir::SIRInstruction &inst, const std::string &vreg,
                                        IsMem isMem)
            {
                for (const auto &a : inst.operands)
                    if (a.type == sir::SIROperandType::REGISTER && a.name == vreg && !isMem(a.name))
                        return true;
                return false;
            }
            // ═══════════════════════════════════════════════════════════════
            // (AR) **القاعدةُ التشخيصيّةُ المشتركة** (م٤، الطورُ الثالث).
            //
            //   حالةُ الفشل — `errorCode_` و`detail_` و`fail()` و`detailOpcode()`
            //   و`detailUiOpcode()` — كانت مكرَّرةً **حرفًا بحرف** في المخفّضَين، وكانت
            //   ستُكرَّر ثالثةً في RISC-V ورابعةً في ARMv7 وخامسةً في i686. رفعُها إلى
            //   قاعدةٍ واحدةٍ يجعل عقدَ التشخيص واحدًا لا خمسةَ نسخٍ تنجرف.
            //
            //   ولماذا قاعدةٌ عاديّةٌ لا CRTP: لا شيءَ هنا يستدعي المشتقَّ — الحالةُ
            //   والمنطقُ محايدان تمامًا. وموضعُ الوراثةِ القالبيّة `LoweringDriver`
            //   أسفلَ هذا الملفّ، حيث يقود التتابعُ المشتركُ خطّافاتِ الهدف فعلًا؛
            //   استعمالُها هنا كلفةٌ نحويّةٌ بلا مقابل.
            //
            //   `requireArity`/`requireMinArity`: عقدُ الشكل وتشخيصُه في نداءٍ واحد.
            //   قبلَها كان موضعُ الاستدعاء يختار رمزَ الخطأ بنفسه ٨٨ مرّة — ويكفي أن
            //   يختار هدفٌ جديدٌ رمزًا آخرَ ليفترق التشخيصُ بين المعماريّات صامتًا.
            // ═══════════════════════════════════════════════════════════════
            class LoweringDiagnostics
            {
            protected:
                // (AR) محميّةٌ لا عامّة: اختصارٌ لخدمةِ المشتقّات، لا جزءٌ من واجهةِ المخفّض.
                using EC = ::Sad::Errors::ErrorCode;

                // (AR) قاعدةُ تركيبٍ لا تعدّدِ أشكال: لا حذفَ عبر مؤشّرِ قاعدة، فمُدمِّرٌ
                //      محميٌّ غيرُ افتراضيّ يُقفل البابَ بلا كلفةِ جدولٍ افتراضيّة.
                ~LoweringDiagnostics() = default;

                // (AR) رمزُ الخطأ من كتالوج SoT وبياناتُه المرافقة؛ لا نصَّ رسالةٍ هنا.
                //      القيمةُ الابتدائيّة «لا مدخل»: وحدةٌ بلا دالّةٍ رئيسيّةٍ فشلٌ لا نجاح.
                EC errorCode_ = EC::INT_NATIVE_NO_ENTRY;
                std::string detail_;

                bool fail(EC code, const std::string &detail = "")
                {
                    errorCode_ = code;
                    detail_ = detail;
                    return false;
                }

                static std::string detailOpcode(const sir::SIRInstruction &i)
                {
                    return diag::kOpcode + std::to_string(static_cast<int>(i.opcode));
                }

                // (AR) تشخيصُ أوپكودِ واجهةٍ في المسار الأصليّ: الخلفيّةُ السياديّةُ لا تخفّض
                //      الواجهةَ — قرارٌ معماريٌّ مُعلَنٌ لا نقصٌ عابر (انظر isUiOpcode أعلاه).
                static std::string detailUiOpcode(const sir::SIRInstruction &i)
                {
                    return diag::kUiRequiresLlvm + std::to_string(static_cast<int>(i.opcode));
                }

                // (AR) شكلٌ مضبوط: نتيجةٌ موجودةٌ وعددُ معامِلاتٍ مساوٍ. الفشلُ يَسِمُ
                //      الأوپكودَ كي لا يكون التشخيصُ «معامِلاتٌ غيرُ صالحة» بلا دلالة.
                bool requireArity(const sir::SIRInstruction &inst, std::size_t arity)
                {
                    return hasResultAndArity(inst, arity) ||
                           fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                }

                // (AR) شكلٌ ذو حدٍّ أدنى: تعليماتٌ متغيّرةُ العدد (نداءٌ، بناءُ مصفوفة…).
                bool requireMinArity(const sir::SIRInstruction &inst, std::size_t least)
                {
                    return hasResultAndMinArity(inst, least) ||
                           fail(EC::INT_COMPILER_INVALID_OPERANDS, detailOpcode(inst));
                }
            };

            // ═══════════════════════════════════════════════════════════════
            // (AR) **المُصدِرُ المجرَّدُ لتخفيضِ SIR** (م٤، الطورُ الرابع، الشقُّ الثاني).
            //
            //   الملاحظةُ التي يقوم عليها: خفضُ عمليّةٍ ثنائيّةٍ في كلِّ معماريّةٍ رأيناها
            //   هو **التتابعُ نفسُه** بستِّ خطوات — تحقّقُ الشكل، البحثُ في جدولِ الهدف،
            //   تخصيصُ سجلِّ الوجهة، تهيئةُ المعامِلَين، إصدارُ التعليمة، ثمّ إعادةُ تعليبِ
            //   النتيجةِ إن كان نوعُها Any. ما يفترق بين الأهداف هو **مضمونُ** الخطوتين
            //   الرابعةِ والخامسة لا ترتيبُهما ولا وجودُهما.
            //
            //   والملاحظةُ ذاتُها تصدق على الأحاديِّ وعلى المقارنة، فيقودها المُصدِرُ نفسُه
            //   بتتابعَين أقصر (`driveUnary` و`driveComparison`) — ولذلك اسمُه «مُصدِرُ
            //   تخفيضٍ» لا «مُصدِرُ عمليّةٍ ثنائيّة».
            //
            //   لذلك رُفِع التتابعُ إلى هنا وبقي المضمونُ في الهدف عبر ثلاثةِ خطّافات
            //   (`prepareBinaryOperands` · `emitBinaryOp` · `boxBinaryResult`) وجدولٍ
            //   (`binaryShape`). المكسبُ ليس في عددِ الأسطر: المعماريّةُ السادسةُ تُصبح
            //   **جدولَ أسماءِ تعليمات** لا نسخةً سادسةً من التتابع — ولا تستطيع أن تُخطئ
            //   ترتيبَه ولا أن تُسقط تعليبَ Any، لأنّ التتابعَ ليس ملكَها.
            //
            //   ولماذا CRTP هنا وقاعدةٌ عاديّةٌ في `LoweringDiagnostics` أعلاه: هذه القاعدةُ
            //   **تنادي المشتقَّ** (الخطّافاتُ الثلاث)، وتلك لا تناديه. القاعدةُ الافتراضيّةُ
            //   كانت ستؤدّي الغرضَ بجدولٍ افتراضيٍّ في مسارٍ ساخنٍ يُنفَّذ لكلِّ تعليمة؛
            //   وCRTP يُلغي ذلك بلا كلفةِ تشغيلٍ ولا مؤشّرِ صنف.
            //
            //   ⚠️ ما لا يقوده هذا المُصدِر — وبقصد: العمليّاتُ التي تكسر **ترتيبَ**
            //      التتابع لا مضمونَه. مثالُها الحاسم: الإزاحةُ ذاتُ المقدارِ المتغيّر في
            //      x86 تُخصِّص الوجهةَ **بعد** الإصدار (لأنّ CL يجب أن يُحرَّر أوّلًا)،
            //      والقسمةُ تحجز RDX:RAX. إقحامُها هنا يقلب ترتيبَ تخصيصِ السجلّات ⇒
            //      شيفرةٌ أخرى لا تجريدٌ. تبقى في الهدف صراحةً، وجدولُه يُغفلها فيردُّ
            //      `nullptr` ⇒ لا تصل أصلًا.
            // ═══════════════════════════════════════════════════════════════

            // (AR) وصفُ العمليّةِ الثنائيّةِ في جدولِ الهدف. الحقلُ المشتركُ الوحيد: هل
            //      النتيجةُ خاضعةٌ لعقدِ Any (تُعلَّب إن كان نوعُ النتيجةِ Any). أسماءُ
            //      التعليمات نفسُها تبقى في جدولِ الهدف لأنّ ترميزَها ملكُ معماريّتِه.
            struct BinaryOpShape
            {
                bool boxesAny = false;
            };

            template <class Target>
            class LoweringDriver : public LoweringDiagnostics
            {
            protected:
                // (AR) قاعدةُ تركيبٍ كسابقتِها: مُدمِّرٌ محميٌّ غيرُ افتراضيّ. بدونه يكون
                //      مُدمِّرُ القالبِ الضمنيُّ عامًّا، فيُفتَح البابُ الذي أُقفل في القاعدة
                //      أدنى منه — وحذفٌ عبر مؤشّرِ قاعدةٍ يُسرِّب حالةَ الهدفِ صامتًا.
                ~LoweringDriver() = default;

                // (AR) التتابعُ المشترك. الخطّافاتُ الثلاثةُ خاصّةٌ في الهدف عادةً، فالهدفُ
                //      يُصادِق هذا القالبَ (`friend`) بدل أن يفتحَ واجهتَه للجميع.
                bool driveBinary(const sir::SIRInstruction &inst)
                {
                    Target *self = static_cast<Target *>(this);
                    if (!requireArity(inst, 2))
                        return false;
                    // (AR) خارجَ الجدول ⇒ «غيرُ مدعوم». مسارٌ ميّتٌ ما دام الموزِّعُ لا يوجّه
                    //      إلّا ما في الجدول؛ حارسٌ يمنع أن يُسقِطَ هدفٌ جديدٌ مدخلًا صامتًا.
                    const BinaryOpShape *shape = self->binaryShape(inst.opcode);
                    if (!shape)
                        return fail(EC::INT_NATIVE_UNSUPPORTED, detailOpcode(inst));
                    int dst;
                    if (!self->allocReg(inst.result->name, dst))
                        return false;
                    // (AR) **الشكلُ الثاني**: معامِلٌ معلَّبٌ ونتيجةٌ Any ⇒ نوعُ العمليّةِ
                    //      لا يُعرَف إلّا زمنَ التشغيل، فالتتابعُ الثلاثيُّ لا يصفُه: لا
                    //      «معامِلانِ ثمّ إصدارٌ ثمّ تعليب» بل مساران مُصدَران معًا وفرعٌ
                    //      بينهما على الوسم، ووسمُ المُخرَجِ يتبع المسارَ المسلوك لا
                    //      يُفترَض. ولذلك خطّافٌ رابعٌ يملك تتابعَه، لا حقلٌ في الشكل.
                    //      وشرطُ `boxesAny` قيدٌ لا تزيين: البتّيّاتُ والعشريّاتُ تُنتِج
                    //      بتّاتٍ خامّةً بلا وسمٍ تُتابِعُه.
                    //      ⚠️ وشرطُ `result->dataType == Any` قيدٌ **يُسرِّب**، وهو مقيسٌ لا
                    //         مُقدَّر: في `دالة ط(أي س، أي ع)` تُصدِر الأماميّةُ ADD_I64
                    //         بمعامِلَي Any ونتيجةِ **Integer** (رُصِد dt0=dt1=Any و
                    //         res=Integer) ⇒ يسقط الإرسالُ بالوسمِ ويعود المسارُ الأعمى:
                    //         «س + ع» = ٤٦١٢٨١١٩١٨٣٣٤٢٣٠٥٣٥ والمفسّرُ «٩٫٥».
                    //         ورفعُ القيدِ **لا يُصلحه**: خانةُ dyn محجوزةٌ لنتائجِ Any
                    //         وحدَها فيتجاوز التعليبُ الحجز، والمستهلِكُ (اطبع_سطر) يقرأ
                    //         سجلًّا نوعُه Integer فيطبع صحيحًا مهما صحّ الحساب. موضعُ
                    //         العلاجِ الأماميّةُ (نوعُ نتيجةِ «أي ⊕ أي» المكتوبةِ معامِلًا)،
                    //         وهو عيبٌ **سابقٌ لهذه الدفعة** (قِيس على HEAD بالنتيجةِ
                    //         عينِها) مُسجَّلٌ حالةً في prove_any_float.sh
                    //         (`any_param_add`) لا مسكوتٌ عنه.
                    if (shape->boxesAny && inst.result->dataType == types::SadTypeKind::Any &&
                        common::hasBoxedAnyOperand(inst))
                        return self->lowerTaggedBinary(inst, dst);
                    if (!self->prepareBinaryOperands(inst, dst))
                        return false;
                    if (!self->emitBinaryOp(inst, dst))
                        return false;
                    return shape->boxesAny ? self->boxBinaryResult(inst, dst) : true;
                }

                // (AR) **الأحاديّ** (NOT/NEG): التتابعُ عينُه بلا معامِلٍ ثانٍ. وقد كان بلا
                //      خطّافِ تعليبٍ أيضًا، وكان ذلك عيبًا لا تبسيطًا: النفيُ على معامِلِ Any
                //      كان مكسورًا في الهدفَين (سقوطٌ بإشارة ١١)، وإصلاحُه مسّ **هذا
                //      التتابعَ** لا الخطّافَ وحدَه لأنّ النتيجةَ تخضع لعقدِ Any كنظيرتِها
                //      الثنائيّة. ولا حقلَ `boxesAny` هنا: الأحاديُّ كلُّه خاضعٌ للعقد،
                //      والخطّافُ نفسُه يردُّ فورًا حين لا تكون النتيجةُ Any.
                bool driveUnary(const sir::SIRInstruction &inst)
                {
                    Target *self = static_cast<Target *>(this);
                    if (!requireArity(inst, 1))
                        return false;
                    int dst;
                    if (!self->allocReg(inst.result->name, dst))
                        return false;
                    // (AR) والتعليبُ خطوةٌ ثالثةٌ في التتابعِ لا خطّافٌ اختياريّ: كان
                    //      ناقصًا هنا وحدَه بين التتابعاتِ الثلاثة، فنتيجةُ النفيِ على
                    //      معامِلٍ Any تخرج **خامّةً** بلا خانةِ dyn ⇒ يقرؤها المستهلِكُ
                    //      مؤشّرًا فيسقط. عقدُ Any واحدٌ للثنائيّ والأحاديّ.
                    return self->prepareUnaryOperand(inst, dst) && self->emitUnaryOp(inst, dst) &&
                           self->boxUnaryResult(inst, dst);
                }

                // (AR) **المقارنةُ كقيمة**. تتابعٌ بخمسِ خطوات، وترتيبُه مُلزِمٌ لا اعتباطيّ:
                //      حلُّ الشرطِ يسبق تخصيصَ الوجهة لأنّه **قد يفشل** (مقارنةٌ لا-موقَّعةٌ
                //      مرفوضة، أو أوپكودُ مقارنةٍ لا شرطَ له) — والفشلُ بعد التخصيصِ يترك
                //      سجلًّا محجوزًا لنتيجةٍ لن تُكتَب. مسارُ العوائمِ يفترق داخليًّا بين
                //      الهدفَين (x86 يُخصِّص ثمّ يقارن، وAArch64 يحلُّ حقلَ cset أوّلًا)
                //      فبقي كتلةً واحدةً يملكها الهدف.
                //      لا تصل هنا مقارنةٌ مدموجةٌ في فرعٍ شرطيّ: `lowerBlock` يتخطّاها.
                bool driveComparison(const sir::SIRInstruction &inst)
                {
                    Target *self = static_cast<Target *>(this);
                    if (!requireArity(inst, 2))
                        return false;
                    if (isFloatCompare(inst))
                        return self->lowerFloatComparison(inst);
                    if (!self->resolveCompareCondition(inst))
                        return false;
                    int dst;
                    if (!self->allocReg(inst.result->name, dst))
                        return false;
                    // (AR) ونظيرُه في المقارنة: `خليط[١] > ٣` تُخفَّض مقارنةً **صحيحةً**
                    //      (نوعُ المعامِلِ الساكنُ Any لا Float) فتُقارَن بتّاتُ ٢٫٥ عددًا
                    //      صحيحًا ضخمًا ⇒ «صادقة» والصوابُ «كاذبة». والفرعُ هنا يقع بعد
                    //      `resolveCompareCondition` عمدًا: الشرطُ الصحيحُ محلولٌ سلفًا،
                    //      ويبقى الحقلُ العشريُّ على الخطّافِ لأنّ ترميزَه ملكُ الهدف.
                    if (common::hasBoxedAnyOperand(inst))
                        return self->lowerTaggedComparison(inst, dst);
                    return self->prepareCompareOperands(inst) && self->emitCompareResult(inst, dst);
                }
            };

        } // namespace common
    } // namespace native
} // namespace sad

#endif // SAD_NATIVE_SIR_LOWERING_COMMON_H
