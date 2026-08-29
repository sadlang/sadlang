// ======================================================================
// ui_prop_lowering.h — خفضُ قيمةِ خاصّيّةِ عنصرِ واجهة (مسارٌ واحدٌ لا نسختان)
// ======================================================================
// (AR) لخاصّيّاتِ الواجهةِ **مساران** يكتبان القيمةَ نفسَها على العقدةِ نفسِها:
//      مسارُ الخاصّيّةِ الأولى (`نص_عنصر("م")` — builtins_ui.cpp) ومسارُ المعدّلاتِ
//      الانسيابيّة (`.نص("م")` — call_method_dispatch.cpp). وقد ثبتَ مرّتَين أنّ
//      ترقيةَ أحدِهما دونَ الآخرِ تُنتِجُ تباعُدًا صامتًا: رقّى #396 مسارَ الخاصّيّةِ
//      الأولى فبقيَ مسارُ المعدّلاتِ يُخزّنُ الأحجامَ المحدَّدةَ نصًّا فارغًا. فبدل
//      رقعةٍ ثالثةٍ في موضعٍ ثالثٍ، القرارُ هنا **مرّةً واحدةً** ويستدعيه المساران.
//
//      والقاعدةُ الحاكمةُ في كلِّ ما يلي: المفسّرُ يوزّعُ على **بنيةِ القيمةِ زمنَ
//      التشغيل** (setIRPropertyFromValue في widget_builder.cpp) والمصرّفُ يوزّعُ على
//      **النوعِ الساكن**. فحيثُ لا يحسمُ النوعُ الساكنُ شيئًا، لا نُخمّن: نمرّرُ
//      الوسمَ والحمولةَ (`SET_PROP_DYN`) ويحسمُ وقتُ التشغيلِ كالمفسّر.
//
// (EN) UI properties are written by TWO paths (primary-property factories and
//      fluent modifiers). Upgrading one and not the other has silently diverged
//      twice already, so the decision lives here once and both paths call it.
//      Guiding rule: the interpreter dispatches on runtime VALUE STRUCTURE while
//      the compiler dispatches on the STATIC TYPE; where the static type decides
//      nothing we do not guess — we pass tag+payload and let the runtime decide.
// ======================================================================

#ifndef SAD_COMPILER_FRONTEND_UI_PROP_LOWERING_H
#define SAD_COMPILER_FRONTEND_UI_PROP_LOWERING_H

#include "sir_types.h"

namespace Sad
{
    namespace Compiler
    {
        namespace Frontend
        {
            namespace UIProps
            {

                /// (AR) نائبُ الخانةِ النصّيّةِ حين لا يكونُ الوسيطُ نصًّا: ليس قيمةً
                ///      منطقيّةً من مصدرِ الحقيقةِ بل **حشوُ موضعٍ** يُبقي الوسائطَ
                ///      الباقيةَ في مواضعِها، والقيمةُ الحقيقيّةُ تُكتَبُ خاصّيّةً بعدَه.
                /// (EN) Placeholder for the text slot — a positional filler, not a value.
                inline constexpr const char *kEmptyTextSlotPlaceholder = "";

                using ::Sad::Compiler::SIR::SadTypeKind;
                using ::Sad::Compiler::SIR::SIRInstruction;
                using ::Sad::Compiler::SIR::SIROpcode;
                using ::Sad::Compiler::SIR::SIROperand;

                /// (AR) نوعٌ لا يحسمُه وقتُ الترجمة (نتيجةُ `/` مثلًا: صحيح/صحيح قد
                ///      يكونُ صحيحًا أو عشريًّا زمنَ التشغيل).
                /// (EN) A type the compiler cannot resolve statically.
                inline bool isUndecidedAtCompileTime(SadTypeKind kind)
                {
                    return kind == SadTypeKind::Any || kind == SadTypeKind::Unknown ||
                           kind == SadTypeKind::Void;
                }

                // ══════════════════════════════════════════════════════════════
                // (AR) **قائمةُ سماحٍ لا قائمةُ منع**: النصُّ وحدَه هو ما يجوزُ أن يبلغَ
                //   خانةَ `const char*`، وكلُّ ما عداه مقبضٌ مُبهَمٌ يُوسَم. الصيغةُ
                //   المعكوسةُ (تعديدُ الخطرِ) خاطئةٌ بنيويًّا: `toDyn` يُسقِطُ **كلَّ**
                //   قيمةٍ مؤشّريّةٍ غيرِ معدودةٍ على وسمِ النصّ، فكلُّ نوعٍ يُنسى من
                //   القائمة — صفٌّ، اختياريّ، نتيجة، تعداد، دالّة، إغلاق، لون، نافذة،
                //   حدث… — يصيرُ قراءةً خارجَ الحدّ يومَ يصيرُ تمثيلُه مؤشّرًا. ومقيسٌ
                //   اليومَ: `صندوق().لون(لامدا…)` يطبعُ عنوانًا خامًّا بينما المفسّرُ
                //   يطبعُ «<لامبدا(1)>». الحصرُ في الآمنِ يجعلُ النوعَ الجديدَ يسقطُ
                //   في المسارِ الآمنِ افتراضًا لا في الخطِر.
                // (EN) ALLOWLIST, not blocklist: only String may reach a `const char*`
                //   slot; everything else is an opaque handle and gets tagged. The
                //   inverse (enumerating the dangerous kinds) is structurally wrong —
                //   toDyn maps every unlisted pointer to the Str tag, so any kind
                //   forgotten from the list becomes an out-of-bounds read the day its
                //   representation becomes a pointer. New kinds must default to SAFE.
                // ══════════════════════════════════════════════════════════════
                inline bool isTextSafeKind(SadTypeKind kind)
                {
                    return kind == SadTypeKind::String;
                }

                inline bool isOpaqueHandleKind(SadTypeKind kind)
                {
                    // (AR) القياسيّاتُ تُوسَمُ بقيمتِها لا بمؤشّرِها، والعدمُ له وسمُه،
                    //   فليست مقابضَ مُبهَمة؛ وما بقيَ فهو مقبضٌ حتّى يُثبَتَ عكسُه.
                    switch (kind)
                    {
                    case SadTypeKind::String:
                    case SadTypeKind::Null:
                    case SadTypeKind::Boolean:
                    case SadTypeKind::Integer:
                    case SadTypeKind::Char:
                    case SadTypeKind::Int8:
                    case SadTypeKind::Int16:
                    case SadTypeKind::Int32:
                    case SadTypeKind::UInt8:
                    case SadTypeKind::UInt16:
                    case SadTypeKind::UInt32:
                    case SadTypeKind::UInt64:
                    case SadTypeKind::Float:
                    case SadTypeKind::Float32:
                    case SadTypeKind::Any:
                    case SadTypeKind::Unknown:
                        return false;
                    default:
                        break;
                    }
                    return true;
                }

                /// (AR) اختيارُ أوپكودِ الكتابةِ من نوعِ القيمةِ الساكن.
                ///      • كلُّ الأحجامِ الصحيحةِ ⇒ INT (المفسّرُ لا يعرفُ عرضًا أصلًا:
                ///        `isInteger()` صادقةٌ لكلِّ صحيحٍ مهما كان حجمُه).
                ///      • العدمُ ⇒ DYN لا STR: مسارُ STR يمرّرُ مؤشّرًا صفريًّا فتُخزَّنُ ""
                ///        بينما المفسّرُ يُصيِّرُ العدمَ نصَّ عرضِه.
                ///      • المُبهَمُ والمقبضُ ⇒ DYN: الوسمُ يحفظُ وقتَ التشغيلِ من تفسيرِ
                ///        المؤشّرِ نصًّا.
                ///      • النصُّ وحدَه ⇒ STR.
                /// (EN) Pick the SET_PROP opcode from the static value type.
                inline SIROpcode propOpcodeForValueType(SadTypeKind kind)
                {
                    switch (kind)
                    {
                    case SadTypeKind::Integer:
                    case SadTypeKind::Char:
                    case SadTypeKind::Int8:
                    case SadTypeKind::Int16:
                    case SadTypeKind::Int32:
                    case SadTypeKind::UInt8:
                    case SadTypeKind::UInt16:
                    case SadTypeKind::UInt32:
                    case SadTypeKind::UInt64:
                        return SIROpcode::BUILTIN_UI_SET_PROP_INT;
                    case SadTypeKind::Float:
                    case SadTypeKind::Float32:
                        return SIROpcode::BUILTIN_UI_SET_PROP_NUM;
                    case SadTypeKind::Boolean:
                        return SIROpcode::BUILTIN_UI_SET_PROP_BOOL;
                    case SadTypeKind::String:
                        return SIROpcode::BUILTIN_UI_SET_PROP_STR;
                    default:
                        break;
                    }
                    return SIROpcode::BUILTIN_UI_SET_PROP_DYN;
                }

                // ══════════════════════════════════════════════════════════════
                // (AR) مصفوفةٌ قيمةَ خاصّيّة ⇒ تُصيَّرُ نصًّا **بالمساعِدِ نفسِه** الذي
                //      يستعملُه `نص(مصفوفة)` (ARRAY_TO_STRING، يوزّعُ في الخلفيّةِ على
                //      نوعِ العنصر). المفسّرُ يفعلُ هذا بعينِه: `setIRPropertyFromValue`
                //      تُصيِّرُ ما ليس عددًا ولا منطقيًّا بـ`toString`، فـ
                //      `صندوق().لون(["أ"])` تُخزَّنُ عندَه «[أ]».
                //
                //      ⚠ **ومدى المطابقةِ محدودٌ ويُقال**: المساعِدُ يوزّعُ على نوعِ
                //      العنصرِ الساكن، فالنصُّ والعشريُّ والمختلطُ الموسومُ تطابقُ
                //      المفسّرَ حرفًا؛ أمّا عنصرٌ **مؤشّريّ** (مصفوفةُ عناصرِ واجهةٍ أو
                //      مصفوفةُ مصفوفات) فيسقطُ على مسارِ `%lld` فيُطبَعُ **عنوانُه**:
                //      `[زر("س")]` ⇒ «[2743989172880]» بينما المفسّرُ «[<عنصر…>]».
                //      تباعُدٌ باقٍ **يُعلَنُ ولا يُدَّعى إغلاقُه**، وهو أهونُ ممّا كان
                //      (قراءةٌ خارجَ الحدّ) لكنّه غيرُ حتميٍّ فلا يُقفَلُ باختبار.
                // (EN) Match is exact for String/Float/tagged-heterogeneous elements
                //      only; pointer-valued elements fall to the %lld path and print
                //      their ADDRESS. A declared, non-deterministic residual divergence.
                //      وبلا هذا الخفضِ يبلغُ **مؤشّرُ SadArray** خانةَ `const char*`
                //      فيُقرَأُ `std::string(SadArray*)` — قراءةٌ خارجَ الحدّ (مقيسة).
                //      وأبناءُ الحاويةِ لا يمرّون من هنا: `ابن/أبناء` محسومةٌ قبلَه
                //      بـADD_CHILD، فما يصلُ إلى هنا قيمةُ خاصّيّةٍ لا طفل.
                // (EN) An array as a property VALUE is stringified with the very same
                //      helper `نص(array)` uses, mirroring the interpreter's toString.
                //      Without it a SadArray* reaches a `const char*` slot ⇒ OOB read.
                //      Children never pass here (ابن/أبناء is handled earlier).
                //
                //      قالبٌ على نوعِ البانِي (لا مرجعٌ لـSIRBuilder) فلا دورةَ تضمين.
                //      Templated on the builder to avoid an include cycle.
                // ══════════════════════════════════════════════════════════════
                template <typename Builder>
                inline bool lowerArrayValueToString(Builder &builder, SIROperand &valueOp,
                                                    SadTypeKind valueType, SadTypeKind elementType)
                {
                    if (valueType != SadTypeKind::Array || !builder.currentBlock_)
                        return false;

                    // (AR) نوعُ العنصرِ يوزّعُ في الخلفيّة (نصّ ⇒ %s، عشريّ ⇒ تنسيقُ
                    //      double، مختلطٌ موسومٌ ⇒ dynToString)، فنُثبِتُه على المعامل.
                    SIROperand arrayOp = valueOp;
                    arrayOp.dataType = SadTypeKind::Array;
                    arrayOp.elementType = elementType;

                    const std::string stringReg = builder.newTempRegister();
                    SIRInstruction toStringInst(SIROpcode::ARRAY_TO_STRING);
                    toStringInst.result = SIROperand::Register(stringReg, SadTypeKind::String);
                    toStringInst.operands.push_back(arrayOp);
                    builder.currentBlock_->instructions.push_back(toStringInst);

                    valueOp = SIROperand::Register(stringReg, SadTypeKind::String);
                    return true;
                }

                // ══════════════════════════════════════════════════════════════
                // (AR) تحريرُ المخزنِ الذي خصّصَه `lowerArrayValueToString`. كلُّ فروعِ
                //   `ARRAY_TO_STRING` تُخصّصُ على الكومة، و`sad_set_prop_str` **ينسخُ**
                //   إلى `std::string` فيصيرُ المخزنُ لقًى فورَ الكتابة. بدونِ هذا يتسرّبُ
                //   عندَ كلِّ كتابةِ خاصّيّةٍ مصفوفيّة — وردُّ نداءٍ يُعيدُ بناءَ قائمةٍ
                //   كلَّ إطارٍ ينمو بلا حدّ. ومسارُ الطباعةِ يُحرّرُ صراحةً منذ البداية
                //   (io_builtins_ops.cpp) فهذا اتّباعٌ لسابقةٍ قائمةٍ لا اختراع.
                // (EN) Free the buffer ARRAY_TO_STRING allocated: sad_set_prop_str COPIES
                //   into std::string, so the buffer is garbage immediately afterwards.
                //   Without this every array-valued property write leaks. The print path
                //   already frees explicitly — this follows the established precedent.
                //   يُستدعى **بعدَ** دفعِ تعليمةِ SET_PROP لا قبلَها.
                // ══════════════════════════════════════════════════════════════
                template <typename Builder>
                inline void releaseLoweredArrayString(Builder &builder, const SIROperand &stringOp)
                {
                    if (!builder.currentBlock_)
                        return;
                    SIRInstruction freeInst(SIROpcode::FREE);
                    freeInst.operands.push_back(stringOp);
                    builder.currentBlock_->instructions.push_back(freeInst);
                }

            } // namespace UIProps
        } // namespace Frontend
    } // namespace Compiler
} // namespace Sad

#endif // SAD_COMPILER_FRONTEND_UI_PROP_LOWERING_H
