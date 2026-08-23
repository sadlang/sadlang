// ============================================================================
// sir_builder_assign_if.cpp — إسناد، متغيرات محلية، جملة إذا
// (AR) buildAssignment, buildLocalVariable, buildIfStatement
// (EN) Assignment, local variable declaration, if statement SIR generation
// تم استخراج هذا الملف من sir_builder_control_branch.cpp وفقاً لقاعدة CW-05
// ============================================================================

#include <string>
#include "sir_builder.h"
#include "builders/statement_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "utf8_utils.h"
#include "error_manager.h" // (AR) رسائل الكتالوج الثنائيّة (ISSUE-120) / (EN) catalog bilingual messages
#include <stdexcept>
#include <iostream>
#include <optional>
#include <filesystem>
#include "sir_constants.h" // (AR) kSadNullSentinel — حارسُ العدمِ لا صفرُ المؤشّر

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            void StatementBuilder::buildAssignment(AST::AssignExpr *assignment)
            {
                if (!assignment)
                {
                    return;
                }

                // (AR) البحث عن المتغير (AssignExpr::name: std::string, line 249)
                // (EN) Lookup variable
                VariableInfo *varInfo = b_.lookupVariable(assignment->name);
                if (!varInfo)
                {
                    // (AR) متغير غير معرف
                    // (EN) Undefined variable
                    b_.errors_.push_back("Undefined variable: " + assignment->name);
                    return;
                }

                // (AR) التحقق من إمكانية التعديل (VariableInfo::isMutable, sir_builder.h:145)
                // (EN) Check if mutable
                if (!varInfo->isMutable)
                {
                    // (AR) ISSUE-120 — عبر الكتالوج لا بنصٍّ خامّ: الرسالةُ كانت مكتوبةً
                    //      يدويًّا بالإنجليزيّة وحدَها، فلا رمزَ لها ولا مقابلَ عربيّ. وأثرُ
                    //      ذلك مقيس: اختبارٌ سالبٌ يطلب SEM007 في المترجم يُخفق رغم أنّ
                    //      المترجم **يرفض** الإسناد فعلًا — فالحارسُ لا يرى ما لا يُرمَّز.
                    //      المفسّر يبلّغ SEM007 لهذه الحالة نفسها، فالتوحيد يسدّ تباعدًا.
                    // (EN) ISSUE-120 — route through the catalog: the hand-written English
                    //      string carried no code, so a compiler-negative test asking for
                    //      SEM007 failed even though the compiler does reject the write.
                    Sad::Errors::RenderContext ectx;
                    ectx.placeholders = {{"name", assignment->name}};
                    b_.errors_.push_back(
                        Sad::Errors::ErrorManager::getInstance().buildBilingualMessage(
                            Sad::Errors::ErrorCode::SEM_CONST_ASSIGNMENT, ectx));
                    return;
                }

                // (AR) بناء قيمة التعبير (AssignExpr::value: ExprPtr, line 250)
                // (EN) Build value expression
                auto valueResult = b_.buildExpression(assignment->value.get());

                // (AR) اقتطاع البايت (u8) عند إعادة الإسناد أيضًا (الخطوة ٩) — نظيرٌ حرفيّ
                //      لاقتطاع buildLocalVariable عند التهيئة (`AND 0xFF`) ولاقتطاع المفسّر
                //      في visitAssignExpr، حفظًا لتكافؤ المسارَين. المصدرُ declaredSurfaceType
                //      المُصرَّح لا varInfo->type — إذ Fix #52 (أعلاه) قد يطمس type إلى Integer
                //      بإعادة إسنادٍ سابقة، بينما يبقى النوعُ السطحيّ ثابتًا. الثابتُ يُطوى؛
                //      السجلّ يُقنَّع بتعليمة AND.
                // (EN) Byte (u8) truncation on REASSIGNMENT too (Step 9) — mirrors the init
                //      truncation in buildLocalVariable (`AND 0xFF`) and the interpreter's
                //      visitAssignExpr, keeping both tracks in parity. Source is the declared
                //      surface type, NOT varInfo->type — Fix #52 (below) may clobber `type` to
                //      Integer on a prior reassignment while the surface type stays fixed.
                //      Constants are folded; registers are masked with an AND instruction.
                if (varInfo->declaredSurfaceType == SadTypeKind::Byte)
                {
                    // (AR) الأنواع العدديّة الصحيحة التي يشملها اقتطاعُ المفسّر: قيمةُ طبيعي64
                    //      تحمل getKind()==Integer زمن التشغيل (Option B)، فيقتطعها المفسّر؛
                    //      فلا بدّ أن يقتطعها المترجم أيضًا (وإلا `ب = <متغيّر طبيعي64>` ينفرج).
                    //      Float/String/Pointer مُستثناة — مطابقةً لحارس المفسّر getKind()==Integer.
                    // (EN) Integer-like numeric types the interpreter's truncation covers: a
                    //      UInt64 value carries getKind()==Integer at runtime (Option B), so the
                    //      interpreter truncates it; the compiler must too (else `ب = <uint64 var>`
                    //      diverges). Float/String/Pointer excluded — mirrors the interpreter's
                    //      getKind()==Integer guard.
                    const bool intLike = (valueResult.type == SadTypeKind::Integer ||
                                          valueResult.type == SadTypeKind::Byte ||
                                          valueResult.type == SadTypeKind::UInt64);
                    if (intLike && valueResult.isConstant)
                    {
                        // (AR) طيّ الثابت: stoull (لا stoll) كي يستوعب حرفيّات طبيعي64 فوق
                        //      INT64_MAX دون رمي؛ التقنيعُ `& 0xFF` لا يمسّ إلا البايت الأدنى
                        //      فيطابق اقتطاع المفسّر (toInt64() & 0xFF). stoull("-1")=2⁶⁴−1
                        //      ⇒ 255، مطابقًا لـ(-1 & 0xFF) في المفسّر.
                        // (EN) Constant fold with stoull (not stoll) to accept UInt64 literals
                        //      above INT64_MAX without throwing; `& 0xFF` touches only the low
                        //      byte, matching the interpreter's toInt64() & 0xFF. stoull("-1")=
                        //      2^64-1 ⇒ 255, matching (-1 & 0xFF).
                        try
                        {
                            unsigned long long uv = std::stoull(valueResult.constantValue);
                            valueResult.constantValue =
                                std::to_string(static_cast<long long>(uv & 0xFFULL));
                            valueResult.type = SadTypeKind::Integer;
                        }
                        catch (const std::exception &)
                        {
                            // (AR) سلسلةٌ غير رقميّة (نادر) — تُترَك للمسار الأصليّ.
                            // (EN) Non-numeric string (rare) — leave to the original path.
                        }
                    }
                    else if (intLike && !valueResult.registerName.empty() && b_.currentBlock_)
                    {
                        SIRInstruction andInst;
                        andInst.opcode = SIROpcode::AND;
                        std::string maskedReg = b_.newTempRegister();
                        andInst.result = SIROperand::Register(maskedReg, SadTypeKind::Integer);
                        andInst.operands.push_back(
                            SIROperand::Register(valueResult.registerName, valueResult.type));
                        andInst.operands.push_back(SIROperand::ConstantI64(0xFF));
                        b_.currentBlock_->addInstruction(andInst);
                        valueResult.registerName = maskedReg;
                        valueResult.type = SadTypeKind::Integer;
                    }
                    else if (valueResult.type == SadTypeKind::Any &&
                             !valueResult.registerName.empty() && b_.currentBlock_)
                    {
                        // (AR) قيمةٌ ديناميّةُ النوع (نتيجةُ قسمةٍ حقيقيّةٍ `/` أو أرضيّةٍ `//`:
                        //      صحيحةٌ إن انقسمت تمامًا وإلّا عشريّة، يُحسَم زمنَ التشغيل). لا يمكن
                        //      اقتطاعُها ساكنًا: قناعٌ غير مشروطٍ يُفسِد بتّاتِ العشريّ. فنُصدر
                        //      TRUNCATE_U8 الذي يقنّع البايت الأدنى **إن كان الوسم صحيحًا فقط**،
                        //      مطابقةً لحارس المفسّر getKind()==Integer. النوعُ يبقى Any (القيمةُ
                        //      إمّا صحيحةٌ مُقنَّعةٌ أو عشريّةٌ سليمة) والطباعةُ تفكّه لاحقًا.
                        // (EN) A runtime-typed value (a true `/` or floor `//` division result:
                        //      integer if exact, else float, decided at runtime). It cannot be
                        //      truncated statically — an unconditional mask corrupts float bits.
                        //      Emit TRUNCATE_U8, which masks the low byte ONLY when the tag is
                        //      integer, mirroring the interpreter's getKind()==Integer guard. The
                        //      type stays Any (masked-int or intact-float) and print decodes it.
                        SIRInstruction truncInst;
                        truncInst.opcode = SIROpcode::TRUNCATE_U8;
                        std::string truncReg = b_.newTempRegister();
                        truncInst.result = SIROperand::Register(truncReg, SadTypeKind::Any);
                        truncInst.operands.push_back(
                            SIROperand::Register(valueResult.registerName, SadTypeKind::Any));
                        b_.currentBlock_->addInstruction(truncInst);
                        valueResult.registerName = truncReg;
                    }
                }

                // (AR) توليد تعليمة STORE لإسناد القيمة
                // (EN) Generate STORE instruction to assign value
                if (b_.currentBlock_ && !valueResult.registerName.empty())
                {
                    SIRInstruction storeInst;
                    storeInst.opcode = SIROpcode::STORE;

                    // (AR) المعامل الأول: القيمة المراد تخزينها
                    // (EN) First operand: value to store
                    SIROperand valueOp;
                    if (valueResult.isConstant && valueResult.type == SadTypeKind::Function)
                    {
                        // (AR) إعادة تعيين لامدا لمتغير: استخدم SIROperand::Function
                        //      بدلاً من CONSTANT لأن resolveOperand يدعم FUNCTION مباشرة
                        // (EN) Lambda reassignment: use SIROperand::Function
                        //      instead of CONSTANT since resolveOperand handles FUNCTION directly
                        valueOp = SIROperand::Function(valueResult.constantValue);
                    }
                    else if (valueResult.isConstant)
                    {
                        valueOp.type = SIROperandType::CONSTANT;
                        valueOp.dataType = valueResult.type;
                        valueOp.name = valueResult.constantValue;
                        if (valueResult.type == SadTypeKind::Integer)
                        {
                            try
                            {
                                valueOp.intValue = std::stoll(valueResult.constantValue);
                            }
                            catch (const std::exception &)
                            {
                                valueOp.intValue = 0;
                            }
                        }
                        else if (valueResult.type == SadTypeKind::Float)
                        {
                            try
                            {
                                valueOp.floatValue = std::stod(valueResult.constantValue);
                            }
                            catch (const std::exception &)
                            {
                                valueOp.floatValue = 0.0;
                            }
                        }
                        // ================================================================
                        // (AR) [Fix #49] معالج Boolean — بدون هذا، صحيح/خطأ يُخزنان دائماً كـ false
                        //      لأن intValue يبقى 0 (الافتراضي) ولا يُعيّن من constantValue
                        //      constantValue هي "true" أو "false" (من b_.buildLiteral)
                        //      resolveOperand يستخدم intValue != 0 لتحديد i1 true/false
                        // (EN) [Fix #49] Boolean handler — without this, true/false always stored as false
                        //      because intValue stays 0 (default) and is not set from constantValue
                        //      resolveOperand uses intValue != 0 to determine i1 true/false
                        // ================================================================
                        else if (valueResult.type == SadTypeKind::Boolean)
                        {
                            valueOp.intValue = (valueResult.constantValue == "true") ? 1 : 0;
                        }
                        // (AR) [Fix #47] لاشيء/null — عدد صحيح بقيمة 0 (تمثيل موحد)
                        // (EN) [Fix #47] null — integer with value 0 (unified representation)
                        else if (valueResult.type == SadTypeKind::Pointer)
                        {
                            valueOp.intValue = 0;
                        }
                        // (AR) [NS-06] لاشيء (Null): خزّن الحارس kSadNullSentinel (لا 0)
                        //      ليتمايز عن الصفر العددي، فيعمل ?? وفحوص العدم بعد إعادة
                        //      الإسناد (س = لاشيء). يطابق تمثيل buildLiteral للحرفيّ.
                        // (EN) [NS-06] null: store kSadNullSentinel (not 0) so ?? and
                        //      null-checks work after reassignment; matches buildLiteral.
                        else if (valueResult.type == SadTypeKind::Null)
                        {
                            try
                            {
                                valueOp.intValue = std::stoll(valueResult.constantValue);
                            }
                            catch (const std::exception &)
                            {
                                valueOp.intValue = Sad::Compiler::kSadNullSentinel;
                            }
                        }
                    }
                    else if (valueResult.type == SadTypeKind::Void)
                    {
                        // (AR) SEM045 (دَين الخانة المجرَّدة): إسنادُ ناتجِ دالّةٍ بلا قيمةٍ —
                        //      سجلُّ الفراغِ لا يقابله شيءٌ في LLVM، فيُخزَّن ثابتُ الفراغِ
                        //      وتُوسَم الخانةُ الديناميّةُ Void (نظيرُ مسارِ التصريح).
                        // (EN) SEM045 (bare-slot debt): assigning a value-less call's result —
                        //      the Void register has no LLVM value; store the Void constant so
                        //      the dynamic slot is tagged Void (mirror of the declaration path).
                        valueOp = SIROperand::ConstantVoid();
                    }
                    else
                    {
                        valueOp.type = SIROperandType::REGISTER;
                        valueOp.name = valueResult.registerName;
                        valueOp.dataType = valueResult.type;
                    }
                    storeInst.operands.push_back(valueOp);

                    // (AR) المعامل الثاني: المؤشر (alloca) للمتغير
                    // (EN) Second operand: variable pointer (alloca)
                    SIROperand ptrOp;
                    ptrOp.type = SIROperandType::REGISTER;
                    ptrOp.name = varInfo->registerName;
                    ptrOp.dataType = varInfo->type;
                    storeInst.operands.push_back(ptrOp);

                    b_.currentBlock_->instructions.push_back(storeInst);

                    // ================================================================
                    // (AR) [Fix #51] إذا كان المتغير ملتقطاً في إغلاق — أصدر ENV_STORE
                    //      لكتابة القيمة الجديدة إلى مصفوفة env[captureIndex]
                    //      نستخدم نفس valueOp (القيمة المُسندة) مباشرة — لا حاجة لـ LOAD
                    //      لأن القيمة متاحة بالفعل في registerName أو كثابت
                    // (EN) [Fix #51] If variable is captured in closure — emit ENV_STORE
                    //      to write new value to env[captureIndex]
                    //      Use valueOp directly — no need for LOAD since value is already available
                    // ================================================================
                    if (varInfo->isCaptured && varInfo->captureIndex >= 0)
                    {
                        SIRInstruction envStoreInst;
                        envStoreInst.opcode = SIROpcode::ENV_STORE;
                        envStoreInst.operands.push_back(valueOp);
                        envStoreInst.operands.push_back(SIROperand::Register(varInfo->envRegister, SadTypeKind::Integer));
                        envStoreInst.operands.push_back(SIROperand::ConstantI64(static_cast<int64_t>(varInfo->captureIndex)));
                        envStoreInst.comment = "closure env_store: " + varInfo->name + " -> env[" + std::to_string(varInfo->captureIndex) + "]";
                        b_.currentBlock_->addInstruction(envStoreInst);
                    }

                    // ================================================================
                    // (AR) [Fix #52] تحديث نوع المتغير عند إعادة الإسناد بنوع مختلف:
                    //      لغة ص ديناميكية — يمكن إعادة إسناد متغير بنوع آخر:
                    //      متغير س = لاشيء  →  نوع: Integer/Pointer
                    //      س = 42           →  نوع: Integer
                    //      س = "نص"         →  نوع: **يجب أن يصبح String**
                    //      بدون هذا: varInfo->type يبقى Integer، والطباعة تطبع عنوان المؤشر
                    //      كرقم بدلاً من محتوى النص.
                    //      هذا التحديث يُبلّغ b_.buildExpression و b_.emitBuiltinPrint بالنوع الصحيح.
                    // (EN) [Fix #52] Update variable type on cross-type reassignment:
                    //      Sad is dynamically typed — variables can be reassigned to different types.
                    //      Without this: varInfo->type stays Integer, print outputs pointer address
                    //      as number instead of string content.
                    // ================================================================
                    if (valueResult.type != SadTypeKind::Void &&
                        valueResult.type != SadTypeKind::Unknown &&
                        valueResult.type != varInfo->type)
                    {
                        varInfo->type = valueResult.type;
                    }
                    // (AR) SEM045 (دَين الخانة المجرَّدة): إسنادُ فراغٍ يجعل الخانةَ ديناميّةً —
                    //      بلا هذا يبقى النوعُ السابقُ (رقمًا مثلًا) فتقرأ الطباعةُ حمولةَ
                    //      الفراغِ صفرًا كاذبًا بدل «لاشيء» (مقيس). الشرطُ أعلاه يستثني
                    //      Void عمدًا لعلّةِ elementType، فالتحويلُ إلى Any هنا لا هناك.
                    // (EN) SEM045 (bare-slot debt): a Void assignment makes the slot dynamic —
                    //      otherwise the stale type (e.g. Integer) makes print read the Void
                    //      payload as a lying 0 instead of «لاشيء» (measured). The condition
                    //      above excludes Void for elementType reasons; the Any switch lives here.
                    else if (valueResult.type == SadTypeKind::Void &&
                             varInfo->type != SadTypeKind::Any)
                    {
                        varInfo->type = SadTypeKind::Any;
                    }

                    // (AR) حدّث وسم «مرجع دالّة مولّدة» عند إعادة الإسناد (بلا شرط، ليُصفَّر
                    //      أيضًا حين يُعاد الإسناد لغير مولّد): يمنع اتّجاهَي الانهيار —
                    //      سلبيّ كاذب (`د = عد` بعد عاديّة ⇒ لا CONSUME) وإيجابيّ كاذب
                    //      (وسم مولّد بائت بعد إسناد عاديّ ⇒ CONSUME على غير مقبض).
                    // (EN) Update the generator-func-ref tag on reassignment (unconditional,
                    //      so it also RESETS when reassigned to a non-generator): prevents both
                    //      crash directions — false negative (`d = count` after a plain fn ⇒ no
                    //      CONSUME) and false positive (stale generator tag after a plain
                    //      reassignment ⇒ CONSUME on a non-handle).
                    varInfo->isGeneratorFuncRef = valueResult.isGeneratorFuncRef;

                    // ================================================================
                    // (AR) [Fix #52 تكملة — ISSUE-082] حدّث أيضًا نوعَ عنصر المصفوفة عند
                    //      إعادة الإسناد. Fix #52 أعلاه يُحدّث varInfo->type فقط؛ لكنّ إعادة
                    //      إسناد مصفوفةٍ بمصفوفةٍ أخرى تُبقي `type=Array` ثابتًا فلا يُحدَّث
                    //      elementType البائت. سيناريو الانهيار: `ز=["مرحبا"]` (elementType=
                    //      String) ثمّ `ز=[9.5]` — يبقى String فيقرأ ARRAY_GET بتّاتِ العشريّ
                    //      كمؤشّرٍ (فرع isNestedArray/String) ⇒ segfault في `اطبع_سطر(ز[0])`
                    //      وفي مطابقة `عندما [ن] إذا ن == "س"` (STRING_CMP يفكّ مؤشّرًا قمامة).
                    //      نُحدّث فقط عند نوعٍ معروف (≠Void) كي لا نطمس معلومةً صحيحة بمجهول —
                    //      نظيرٌ حرفيٌّ لمسار التصريح (buildLocalVariable). المفسّر ديناميّ
                    //      فيوافق دائمًا؛ هذا يُعيد التكافؤ الثنائيّ.
                    // (EN) [Fix #52 follow-up — ISSUE-082] Also update the array element type
                    //      on reassignment. Fix #52 above updates varInfo->type only; but
                    //      reassigning an array with another array keeps `type=Array` constant,
                    //      so the stale elementType is never refreshed. Crash scenario:
                    //      `ز=["مرحبا"]` (elementType=String) then `ز=[9.5]` — stays String, so
                    //      ARRAY_GET reads the float bits as a pointer (isNestedArray/String
                    //      branch) ⇒ segfault in `print(ز[0])` and in a `عندما [ن] if ن == "س"`
                    //      match (STRING_CMP dereferences a garbage pointer). Update only for a
                    //      known type (≠Void) so a valid element type is never clobbered by an
                    //      unknown — mirrors the declaration path (buildLocalVariable). The
                    //      interpreter is dynamic and always agrees; this restores binary parity.
                    // ================================================================
                    if (valueResult.elementType != SadTypeKind::Void &&
                        valueResult.elementType != varInfo->elementType)
                    {
                        varInfo->elementType = valueResult.elementType;
                    }
                    // (AR) [عناصر موسومة — option A] مسحُ وسم Any البائت عند إعادة الإسناد:
                    //      متغيّرٌ موسومٌ (elementType=Any، خاناتُه صناديقُ كومة) أُعيد إسنادُه
                    //      بقيمةٍ **نوعُ عنصرها ليس Any** (مصفوفةٌ مختلطةٌ غير-قياسيّة/متداخلة
                    //      ⇒ Void، أو غيرُ مصفوفةٍ أصلًا ⇒ خاناتٌ خام). الشرطُ أعلاه يتخطّى
                    //      Void فيُبقي الوسمَ البائت، فيرى القارئُ Any ويفكّ خانةً خامًا ⇒
                    //      **انهيار ذاكرة** (رصدته أميليا). نمسحه صراحةً إلى نوع القيمة الجديدة.
                    // (EN) [boxed elements — option A] Clear a STALE Any tag on reassignment: a
                    //      boxed variable (elementType=Any, heap-box slots) reassigned with a value
                    //      whose element type is NOT Any (a non-scalar-heterogeneous/nested array
                    //      ⇒ Void, or a non-array ⇒ raw slots). The guard above skips Void so it
                    //      keeps the stale tag ⇒ the reader sees Any and unboxes a raw slot ⇒ a
                    //      memory CRASH (found by Amelia). Clear it to the new value's type.
                    else if (varInfo->elementType == SadTypeKind::Any &&
                             valueResult.elementType != SadTypeKind::Any)
                    {
                        varInfo->elementType = valueResult.elementType; // may be Void — clears Any
                    }
                }

                // (AR) تتبع نوع الصنف عند إعادة التعيين بـ جديد()
                //      مثال: _نظام = جديد نظام_ملفات() ← يجب ربط _نظام بالصنف نظام_ملفات
                //      بدون هذا، استدعاءات _نظام.هيئ() تولّد ".هيئ" بدلاً من "نظام_ملفات.هيئ"
                // (EN) Track class type on reassignment with new()
                //      e.g., _نظام = جديد نظام_ملفات() → must associate _نظام with نظام_ملفات
                //      Without this, _نظام.هيئ() generates ".هيئ" instead of "نظام_ملفات.هيئ"
                if (auto *newExpr = dynamic_cast<Sad::AST::NewExpr *>(assignment->value.get()))
                {
                    b_.classInstanceTypes_[assignment->name] = newExpr->className;
                }

                // (AR) [نظام الإغلاقات الجديد] لم نعد نُسجّل b_.lambdaAliases_ عند إعادة التعيين
                //      جميع استدعاءات اللامدا تمر عبر CLOSURE_CALL
                // (EN) [New closure system] No longer update b_.lambdaAliases_ on reassignment
                //      All lambda calls go through CLOSURE_CALL
            }

            // ============================================================================
            // buildLocalVariable - بناء تصريح متغير محلي
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:433
            // التوقيع / Signature: void buildLocalVariable(AST::VarDeclStmt* varDecl);
            //
            // المعاملات / Parameters:
            // - varDecl: AST::VarDeclStmt* (statements.h:74)
            //
            // VarDeclStmt Members (statements.h:74-100):
            // - name: std::string (line 76)
            // - type: Types::SadTypeKind (line 77)
            // - initializer: ExprPtr (line 78)
            // - isConst: bool (line 79)
            // ============================================================================
            void StatementBuilder::buildLocalVariable(AST::VarDeclStmt *varDecl)
            {
                if (!varDecl)
                {
                    return;
                }

                // (AR) اللبنة 3.16: المصفوفة الساكنة (.bss) تُعرَّف في المستوى الأعلى حصرًا
                //      (تخزين ساكن عالميّ). داخل دالّة لا يُصدَر تخزين .bss، فبدل توليد خاطئ
                //      صامت (alloca) نرفضها صراحةً هنا (نقطة تجميع كلّ المسارات — رصد أميليا).
                if (varDecl->isStaticArray)
                {
                    b_.errors_.push_back(
                        "Error: SEM023 المصفوفة الساكنة «متغير ساكن اسم مصفوفة[N]» تُعرَّف "
                        "في المستوى الأعلى حصرًا (تخزين .bss عالميّ)، لا داخل دالّة.");
                    return;
                }

#ifdef SIR_BUILDER_DEBUG
                std::cerr << "[SIR-DBG] buildLocalVariable: name='" << varDecl->name
                          << "' type=" << static_cast<int>(varDecl->type) << std::endl;
#endif

                // ════════════════════════════════════════════════════════════════
                // (AR) 🔑 هل نحن داخلَ جسمِ ماكرو؟ العَلَمُ يُشتقُّ **مرّةً** هنا ويُقرأ
                //      في **خمسةِ** مواضعَ أسفل. لأنّ التصريحَ لا يبلغُ حالةَ المحيطِ
                //      من طريقٍ واحد: ① اسمُ السجلّ · ② إعادةُ استعمالِ خانةِ الخارج ·
                //      ③ تخطّي الثابتِ العامّ · ④ نوعُ المتغيّرِ العامِّ في الوحدة ·
                //      ⑤ نوعُ `VariableInfo` للمتغيّرِ الخارجيِّ في النطاق.
                //
                //      ⚠️ **والعددُ نفسُه كان كاذبًا**: كُتِب «أربعة» أوّلًا فبقيَ ⑤
                //      مفتوحًا — وهو جارُ ④ حرفيًّا في الملفّ. فأنتج العزلُ الجزئيُّ
                //      انهيارًا (`rc=139`) لم يكن في خطِّ الأساس. وعددٌ منثورٌ في
                //      تعليقٍ نسخةٌ ثانيةٌ لحقيقةٍ تُصدَّق بلا فحص.
                //
                //      🔑 وسادسٌ لا يمرُّ بهذا العَلَمِ أصلًا: `classInstanceTypes_`
                //      خريطةٌ مسطّحةٌ بالاسمِ المجرَّد، فتُحفَظ وتُستعاد في
                //      `call_macro.cpp` — لأنّ كتمانَها هنا يُعمي جسمَ الماكرو عن
                //      صنفِ كائنِه هو.
                // (EN) Are we inside a macro body? Derived ONCE and read in FIVE places:
                //      slot name, outer-slot reuse, const-global skip, module global type,
                //      and the enclosing VariableInfo's type. The count itself was WRONG at
                //      first ("four"), leaving ⑤ — literally the neighbour of ④ — open, and
                //      partial isolation produced an rc=139 crash absent from the baseline.
                //      A sixth route bypasses this flag entirely: `classInstanceTypes_` is a
                //      flat bare-name map, saved and restored in call_macro.cpp instead,
                //      because suppressing it here would blind the body to its own object.
                // ════════════════════════════════════════════════════════════════
                const bool inMacroBody = !b_.macroSlotNamespace_.empty();

                // ================================================================
                // (AR) تخطي الثوابت العامة التي لديها قيمة أولية حرفية:
                //      المتغير مسجّل مسبقاً كمتغير عام ثابت في المرحلة 1.5 مع قيمة أولية.
                //      b_.emitGlobalVariables ينشئ `@name = internal constant i64 42`.
                //      لا نحتاج ALLOC أو STORE — القيمة موجودة في المتغير العام.
                //      بدون هذا: STORE إلى constant = ACCESS_VIOLATION/crash.
                // (EN) Skip const globals that already have a literal initializer:
                //      The variable is pre-registered as a constant global in Phase 1.5 with initialValue.
                //      b_.emitGlobalVariables creates `@name = internal constant i64 42`.
                //      No ALLOC or STORE needed — value is already in the global.
                //      Without this: STORE to constant = ACCESS_VIOLATION/crash.
                // ================================================================
                // (AR) ⚠️ `!inMacroBody`: بلا هذا يقعُ `ثابت س = ...` داخلَ ماكرو على
                //      عامٍّ خارجيٍّ يحملُ الاسمَ نفسَه فيُتخطّى إصدارُه أصلًا.
                // (EN) Without !inMacroBody a const declaration inside a macro binds to a
                //      same-named outer global and its emission is skipped entirely.
                if (b_.module_ && varDecl->isConst && !inMacroBody)
                {
                    auto sirGlobal = b_.module_->getGlobalVariable(varDecl->name);
                    // (AR) الحكمُ بالعَلَمِ لا بفراغِ النصّ: `ثابت فارغ = ""` مُهيَّأٌ تهيئةً
                    //      كاملةً، وكان اشتقاقُ التهيئةِ من الفراغِ يُسقِطه إلى المسارِ المحلّيِّ
                    //      فيبقى العامُّ مصفَّرًا ⇒ يُطبع «void».
                    // (EN) Judge by the flag, not by emptiness: `ثابت فارغ = ""` is fully
                    //      initialised; deriving initialisation from emptiness dropped it to the
                    //      local path, leaving the global zeroed ⇒ printed as "void".
                    if (sirGlobal && sirGlobal->isConstant && sirGlobal->hasInitialValue)
                    {
                        // (AR) الثابت العام مُهيأ بالفعل — لا حاجة لكود إضافي
                        // (EN) Const global already initialized — no code needed
                        return;
                    }
                }

                // ════════════════════════════════════════════════════════════════
                // (AR) تحويل النوع (VarDeclStmt::type: Types::SadTypeKind, line 77)
                // (EN) Convert type
                SadTypeKind varType = b_.astTypeToSIRType(varDecl->type);
                bool needsTypeInference = (varDecl->type == Types::SadTypeKind::Unknown);

                // ================================================================
                // (AR) م-١٠: لقطةُ حالِ المتغيّرِ السابقِ بالاسم نفسِه **قبل** بناء المُهيِّئ.
                //      ولا تجوزُ اللقطةُ بعدَه: استنتاجُ النوع أدناه يكتبُ
                //      `existingVar->type = varType` على المدخلةِ الموجودةِ نفسِها، فتصيرُ
                //      مقارنةُ النوعِ بعدَها **صحيحةً دائمًا** — وهو ما أخفى انهيارًا حقيقيًّا.
                // (EN) م-١٠: snapshot the pre-existing same-named variable BEFORE building the
                //      initializer. Snapshotting after is wrong: the type-inference block below
                //      writes `existingVar->type = varType` onto that very entry, making a later
                //      type comparison vacuously true — which masked a real crash.
                // ================================================================
                const VariableInfo *priorSameName = b_.lookupVariable(varDecl->name);
                const bool priorIsReusableSlot =
                    priorSameName != nullptr &&
                    !priorSameName->isGlobal &&
                    // (AR) المعامل مستثنًى **تحفّظًا** لا لأنّ تخزينَه مختلف: موقعُه
                    //      ALLOC محلّيّةٌ باسم `%<اسم>` تمامًا كغيره (sir_builder_functions).
                    //      فالاستثناءُ قرارُ حَذَرٍ يُبقي السلوكَ السابقَ ولا يزعمُ فرقًا بنيويًّا.
                    // (EN) Parameters are excluded out of CAUTION, not because their storage
                    //      differs: it is a local ALLOC named `%<name>` like any other. The
                    //      exclusion preserves prior behaviour; it does not claim a structural
                    //      difference. (A pre-existing divergence remains in that case.)
                    !priorSameName->isParameter &&
                    // (AR) الملتقَطُ يحمل `%__cap_…` فيستبعدُه فحصُ الاسم أدناه وحدَه؛
                    //      يُذكَر صراحةً لأنّ الاعتمادَ على تسميةٍ داخليّةٍ حمايةٌ هشّة.
                    // (EN) Captured vars carry `%__cap_…`, so the name check alone excludes
                    //      them; stated explicitly because relying on an internal naming
                    //      convention is a brittle guarantee.
                    !priorSameName->isCaptured &&
                    // (AR) 🔴 والثابتُ مستثنًى **لزومًا لا تحفّظًا**: إعادةُ استعمالِ موقعِه
                    //      تكتبُ فوقَه فعلًا، فتفتحُ طريقًا للالتفافِ على SEM007 («لا يمكن
                    //      تعديل الثابت») بلا تشخيصٍ ولا خطأ. مُبرهَنٌ حيًّا: «ثابت ع = 1»
                    //      ثمّ «متغير ع = 9» في ذراعِ «طابق» ⇒ المفسّرُ 1 والمترجِمُ 9.
                    //      وقبلَ هذا الإصلاحِ كان المترجِمُ يطبعُ 1 مطابقًا — فحذفُ هذا
                    //      الشرطِ انحدارٌ لا تضييقُ نطاق.
                    // (EN) 🔴 Constants are excluded out of NECESSITY, not caution: reusing the
                    //      slot genuinely overwrites them, silently bypassing SEM007 ("cannot
                    //      modify constant"). Proven live: «ثابت ع = 1» then «متغير ع = 9» in a
                    //      match arm ⇒ interpreter 1, compiler 9. Before this fix the compiler
                    //      printed 1 (matching), so dropping this clause is a regression.
                    priorSameName->isMutable &&
                    // (AR) وعدّادُ حلقةِ المدى مستثنًى **لزومًا** كذلك: يجتازُ كلَّ ما
                    //      سبق (محلّيٌّ · متغيّرٌ · عددٌ · `%<اسم>`) لكنّ موقعَه يملكُه
                    //      شرطُ الحلقة. من دونِ هذا الاستثناءِ يكتبُ التصريحُ الداخليُّ
                    //      في موقعِ التحكّمِ فيهبطُ عدُّ الدوراتِ من ٣ إلى ١ (مقيس).
                    // (EN) Range-loop counters are excluded out of NECESSITY too: they pass
                    //      every clause above, but their slot is owned by the loop condition.
                    //      Without this the inner declaration writes into the control slot and
                    //      the iteration count drops from 3 to 1 (measured).
                    !priorSameName->isLoopControl &&
                    priorSameName->registerName == ("%" + varDecl->name);
                const SadTypeKind priorType =
                    priorSameName ? priorSameName->type : SadTypeKind::Unknown;

                // ================================================================
                // (AR) ⚠ إعادةُ الاستعمال مقصورةٌ على **الأنواعِ القياسيّة** عمدًا.
                //      تطابقُ `SadTypeKind` تطابقٌ اسميٌّ لا تمثيليّ: مصفوفتانِ كلتاهما
                //      `Array` تختلفان في نوعِ العنصر، وخريطتانِ في نوعِ القيمة، وكائنانِ
                //      في تخطيطِ الحقول — ونوعُ الصنفِ **لا يُسجَّل في VariableInfo أصلًا**
                //      (يُسجَّل في classInstanceTypes_)، فلا سبيلَ إلى مقارنتِه هنا.
                //      فأيُّ إعادةِ استعمالٍ في هذه الفئاتِ تقرأ بياناتٍ بتخطيطٍ خاطئ:
                //      مُبرهَنٌ حيًّا أنّ `["أ"،"ب"]` ثمّ `[10،20]` بالاسم نفسِه ⇒ **SIGSEGV**.
                //      القياسيُّ وحدَه ذو تمثيلٍ محدَّدٍ بالنوع، فهو وحدَه آمن.
                // (EN) ⚠ Slot reuse is deliberately limited to SCALAR types. A matching
                //      SadTypeKind is a nominal match, not a representational one: two Arrays
                //      may differ in element type, two Maps in value type, two objects in field
                //      layout — and the class name is never recorded in VariableInfo at all
                //      (it lives in classInstanceTypes_), so it cannot be compared here. Reuse
                //      in those families reads data through the wrong layout: proven live that
                //      `["أ","ب"]` then `[10,20]` under one name ⇒ SIGSEGV. Only scalars have a
                //      representation fully determined by their type.
                // ================================================================
                const bool priorTypeIsScalar =
                    priorType == SadTypeKind::Integer || priorType == SadTypeKind::Float ||
                    priorType == SadTypeKind::Boolean || priorType == SadTypeKind::String ||
                    priorType == SadTypeKind::UInt64 || priorType == SadTypeKind::Byte;
                // (AR) ⚠ لا يُبنى الحكمُ على `elementType`/`elementClassName`: الأوّلُ **بلا
                //      تهيئةٍ افتراضيّةٍ** في VariableInfo ولا يُكتَب إلّا للمصفوفات، فقراءتُه
                //      لمتغيّرٍ قياسيٍّ قراءةُ قيمةٍ غيرِ معرَّفة. (جرّبتُه فمنعَ إعادةَ
                //      الاستعمالِ في الحالةِ القياسيّةِ نفسِها التي يقصدُها الإصلاح.)
                //      والفصلُ الصحيحُ **بالنوعِ وحدَه**: Array/Map/Class ليست قياسيّةً فتُستبعَد.
                // (EN) ⚠ The decision must NOT rest on elementType/elementClassName: the former
                //      has NO default initialiser in VariableInfo and is written only for arrays,
                //      so reading it for a scalar reads an indeterminate value. (Tried it: it
                //      blocked reuse in the very scalar case this fix targets.) The type alone is
                //      the correct discriminator — Array/Map/Class are not scalars.
                const bool priorHasNoAggregateShape =
                    priorSameName == nullptr || priorSameName->className.empty();

                // (AR) [NS-06 موجة 2] نوع اختياريّ `T؟`: استعمل النوع الداخليّ (T) للتخزين/
                //      التحميل كي تحتفظ القيمة الحاضرة بنوعها الصحيح (نص لا i64)، فلا
                //      يُحوَّل مؤشّر النصّ عدديًّا في `؟؟`. تمثيل العدم يبقى الحارس i64
                //      (يُفحَص في `؟؟`/المقارنات) وهو متوافق مع كلّ الأنواع. النوع الداخليّ
                //      مُسبَّك الآن عبر sadType = Optional<T> من المحلّل النحويّ.
                // (EN) [NS-06 wave 2] Optional `T?`: use inner type T for storage/load so a
                //      present value keeps its real type (string not i64); null stays the i64
                //      sentinel. Inner type now plumbed via sadType = Optional<T> from parser.
                varType = b_.resolveDeclaredStorageKind(varDecl->type, varDecl->sadType.get(), varType);

                // ════════════════════════════════════════════════════════════════
                // (AR) 🔑 ISSUE-138: نائبٌ لا يُستبدَل يصير جوابًا نهائيًّا كاذبًا
                // ════════════════════════════════════════════════════════════════
                //
                // (AR) `astTypeToSIRType` تُرجِع `Integer` عن `Unknown` بتعليقٍ يقول
                //      إنّه **نائبٌ يستبدله استنتاجُ الأنواع لاحقًا**. والفرضُ صحيحٌ
                //      حيثما وُجِد مُهيِّئٌ يُستنتَج منه — ويسقط حين لا يوجد: فلا شيءَ
                //      يستبدل النائبَ، فيخرج «رقم» جوابًا نهائيًّا عن خانةٍ لا نوعَ
                //      لها. ومقيسُه (٢٠٢٦-٠٨-١٧): `متغير ك` ثمّ `اطبع_سطر(ك)` يطبع
                //      **`0`** والمرجعُ «لاشيء»، و`نوع(ك)` يقول **«رقم»** والمرجعُ
                //      «فراغ» — بـrc=0 في الطرفَين، أي كذبٌ صامتٌ لا انهيار.
                //
                //      ⇒ الخانةُ بلا نوعٍ وبلا مُهيِّئٍ **ديناميّةٌ** (`%SadDyn`)،
                //      تُهيَّأ فراغًا في جدولِ القيمِ الافتراضيّةِ أدناه.
                //
                //      وقِيس أنّ التبديلَ لا يُغيّر ما بعدَه: `أي ك` ثمّ `ك = 5` ثمّ
                //      `ك + 1` تُعطي `6` في المحرّكَين كما تُعطيها `متغير ك`، عالميًّا
                //      وداخلَ دالّة. فالتغييرُ يمسّ **الخانةَ قبل أوّلِ إسناد** وحدَها.
                // (EN) ISSUE-138: astTypeToSIRType maps Unknown to Integer as a placeholder
                //      "overwritten by inference later" — true only when an initializer
                //      exists. With none, the placeholder IS the final answer, so a typeless
                //      slot prints 0 and نوع() says «رقم» while the reference says «لاشيء»
                //      / «فراغ», both at exit code 0. A typeless, initializer-less slot is
                //      dynamic; it is defaulted to VOID below.
                // ════════════════════════════════════════════════════════════════
                varType = b_.resolveBareSlotStorageKind(
                    varDecl->type, varDecl->initializer != nullptr, varType);

                // (AR) إنشاء معلومات المتغير (sir_builder.h:139 - VariableInfo)
                // (EN) Create variable info
                VariableInfo varInfo;
                varInfo.name = varDecl->name; // line 76
                varInfo.type = varType;
                // (AR) [طبقة طبيعي64 — الخطوة ٥] النوع السطحيّ المُصرَّح صراحةً (طبيعي64/بايت…)
                //      من تعليق `متغير <نوع>`؛ Unknown إذا كان النوع مُستنتَجًا. لا نلوّثه
                //      باستنتاج القيمة الأوّليّة أدناه كي يظلّ قرار الإشارة مطابقًا للمفسّر.
                // (EN) [طبيعي64 layer — Step 5] Explicitly-declared surface type from the
                //      `var <type>` annotation; Unknown when the type is inferred. Deliberately
                //      NOT overwritten by initializer inference below so the signedness decision
                //      stays identical to the interpreter.
                varInfo.declaredSurfaceType = varDecl->type;
                // (AR) 🔑 الفضاءُ يُلحَقُ بـ**السجلِّ** وحدَه، و`varInfo.name` يبقى مجرَّدًا:
                //      البحثُ في النطاقاتِ بالاسمِ المصدريّ، فقراءةُ الجسمِ لمتغيّرِه تصلُ
                //      إليه عبرَ النطاقِ وتقرأُ السجلَّ المُفضَّى. ولو فُضِّيَ الاسمُ لَانقطعَ
                //      كذلك البحثُ عن المتغيّراتِ الخارجيّةِ التي يقرؤها الجسم.
                // (EN) The namespace goes on the REGISTER only; `name` stays bare so scope
                //      lookup keeps resolving both the body's own name and outer names.
                varInfo.registerName = inMacroBody
                                           ? ("%" + varDecl->name + kSlotNamespaceSeparator +
                                              b_.macroSlotNamespace_)
                                           : ("%" + varDecl->name);
                varInfo.isGlobal = false;
                varInfo.isMutable = !varDecl->isConst; // line 79
                varInfo.scopeLevel = b_.currentScopeLevel_;

                // (AR) معالجة القيمة الأولية (VarDeclStmt::initializer: ExprPtr, line 78)
                // (EN) Handle initializer - process first for type inference
                BuildResult initResult;
                bool hasInitializer = varDecl->initializer && b_.currentBlock_;

                if (hasInitializer)
                {
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG]   building initializer, expr type="
                              << typeid(*varDecl->initializer).name() << std::endl;
#endif
                    initResult = b_.buildExpression(varDecl->initializer.get());
#ifdef SIR_BUILDER_DEBUG
                    std::cerr << "[SIR-DBG]   initResult: reg='" << initResult.registerName
                              << "' type=" << static_cast<int>(initResult.type)
                              << " isConst=" << initResult.isConstant
                              << " constVal='" << initResult.constantValue << "'" << std::endl;
#endif

                    // (AR) استنتاج النوع من التعبير إذا كان النوع غير معروف
                    // (EN) Infer type from expression if type is unknown
                    if (needsTypeInference)
                    {
                        // (AR) SEM045 (دَين الخانة المجرَّدة): الاستنتاجُ يمرّ بسلطةِ الخانةِ
                        //      نفسِها — مُهيِّئٌ «فراغٌ» (نداءُ دالّةٍ بلا قيمة) ⇒ خانةٌ
                        //      ديناميّةٌ Any لا خانةُ Void (عقد ISSUE-138 المقيس).
                        // (EN) SEM045 (bare-slot debt): inference goes through the storage
                        //      authority — a Void initializer (value-less call) ⇒ a dynamic
                        //      Any slot, never a Void slot (measured ISSUE-138 contract).
                        varType = b_.resolveBareSlotStorageKind(
                            varDecl->type, /*hasInitializer=*/true, initResult.type);
                        varInfo.type = varType;

                        // ================================================================
                        // (AR) [Fix #44] تحديث المتغير العام في SIRModule عندما يتغير النوع:
                        //      Phase 1.5 تُسجّل المتغيرات العامة مبكراً قبل تنفيذ b_.buildExpression.
                        //      إذا كانت القيمة الأولية تعبيراً معقداً (BinaryExpr, CallExpr...)
                        //      قد يُخطئ Phase 1.5 في استنتاج النوع (مثلاً: Integer بدلاً من Float).
                        //      هنا في Phase 2/3، بعد b_.buildExpression الفعلي، لدينا النوع الدقيق.
                        //      نُحدّث SIRGlobalVariable والنطاق ليتطابقا مع النوع الحقيقي.
                        //      بدون هذا: متغير عشري يُخزن في alloca i64 → fptosi → قطع عشري!
                        //
                        //      مثال: متغير مج = ع1 + ع2 حيث ع1=3.14, ع2=2.71
                        //      Phase 1.5 أنشأ SIRGlobalVariable(مج, Integer) [قبل الإصلاح]
                        //      الآن بعد b_.inferExprType: SIRGlobalVariable(مج, Float)
                        //      مع هذا الأمان الإضافي: حتى لو أخطأ b_.inferExprType،
                        //      b_.buildExpression يعطينا النوع الدقيق ونُحدّث هنا.
                        // (EN) [Fix #44] Update SIRGlobalVariable when type changes:
                        //      Phase 1.5 pre-registers globals before b_.buildExpression runs.
                        //      For complex initializers (BinaryExpr, CallExpr...),
                        //      Phase 1.5 may infer wrong type (e.g. Integer instead of Float).
                        //      Here in Phase 2/3, after real b_.buildExpression, we have exact type.
                        //      Update SIRGlobalVariable and scope to match the real type.
                        //      Without this: float stored in i64 alloca → fptosi → truncation!
                        // ================================================================
                        // (AR) ⚠️ `!inMacroBody`: تصريحٌ داخلَ ماكرو لا يملكُ العامَّ
                        //      الخارجيَّ فلا يجوزُ أن يُبدِّلَ نوعَه.
                        // (EN) A macro-body declaration does not own the outer global.
                        if (b_.module_ && !inMacroBody)
                        {
                            auto sirGlobal = b_.module_->getGlobalVariable(varDecl->name);
                            if (sirGlobal && sirGlobal->type != varType)
                            {
                                sirGlobal->type = varType;
                            }
                        }
                        // ════════════════════════════════════════════════════
                        // (AR) 🔴 **الطريقُ الخامس** — وهو أخطرُ الخمسةِ ولم يكن في
                        //      العدِّ الأوّل. `addVariable` للمدخلةِ الداخليّةِ يقعُ
                        //      **بعدَ** هذا السطر، فـ`lookupVariable` هنا يُصيبُ
                        //      متغيّرَ **المحيطِ** حتمًا لا الداخليَّ.
                        //
                        //      وسدُّ جارِه أعلاه (نوعِ العامِّ) دونَ سدِّه **يُنتِجُ ما هو
                        //      أسوأُ من التسريبِ الأصليّ**: تبقى الخانةُ معزولةً
                        //      والعامُّ `i64`، بينما يظنُّ القارئُ أنّه نصٌّ — فيُنادى
                        //      `strlen` على العنوان `0x5`. مقيسٌ ٢٠٢٦-٠٨-١٨:
                        //      خارجُه `متغير ن = 5` وداخلُ الماكرو `متغير ن = "نص"` ⇒
                        //      **`rc=139`** بعدَ الرقعةِ الناقصة، مقابلَ `rc=0` بقيمةٍ
                        //      خاطئةٍ قبلَها. والعكسُ (نصٌّ خارجًا وعددٌ داخلًا) يطبعُ
                        //      عنوانًا رقمًا (`140702511144960`) بلا انهيار.
                        //
                        //      🔑 والدرسُ أنّ **العزلَ الجزئيَّ أخطرُ من لا عزل**:
                        //      حقيقتانِ متّفقتانِ على الخطأِ تُعطيان جوابًا خاطئًا،
                        //      وحقيقتانِ متناقضتانِ تُعطيان انهيارًا.
                        // (EN) 🔴 The FIFTH route, and the most dangerous — absent from the
                        //      first count. The inner `addVariable` happens AFTER this line,
                        //      so `lookupVariable` here necessarily hits the ENCLOSING
                        //      variable. Sealing its neighbour (the global's type) without
                        //      sealing this one is WORSE than the original leak: the slot is
                        //      isolated and the global stays i64 while the reader believes it
                        //      is a string, so `strlen` runs on address 0x5. Measured:
                        //      rc=139 with the partial patch vs rc=0 (wrong value) before it.
                        //      Partial isolation is more dangerous than none: two facts that
                        //      agree on a wrong answer print garbage; two that disagree crash.
                        // ════════════════════════════════════════════════════
                        if (!inMacroBody)
                        {
                            VariableInfo *existingVar = b_.lookupVariable(varDecl->name);
                            if (existingVar && existingVar->type != varType)
                            {
                                existingVar->type = varType;
                            }
                        }
                    }

                    // (AR) نقل نوع عنصر المصفوفة إلى VariableInfo لدعم foreach
                    // (EN) Propagate array element type to VariableInfo for foreach support
                    if (initResult.elementType != SadTypeKind::Void)
                    {
                        varInfo.elementType = initResult.elementType;
                    }
                    if (!initResult.elementClassName.empty())
                    {
                        varInfo.elementClassName = initResult.elementClassName;
                    }

                    // (AR) تتبع اسم دالة اللامدا المرتبطة (لتحديد نوع الإرجاع في CLOSURE_CALL)
                    // (EN) Track associated lambda function name (for CLOSURE_CALL return type)
                    if (!initResult.closureLambdaName.empty())
                    {
                        varInfo.closureLambdaName = initResult.closureLambdaName;
                    }
                    // (AR) تتبّع مرجع الدالّة المولّدة (لإصدار CONSUME عند الاستدعاء غير المباشر)
                    // (EN) Track generator func-ref (to emit CONSUME on indirect call)
                    varInfo.isGeneratorFuncRef = initResult.isGeneratorFuncRef;

                    // (AR) تتبع نوع الصنف إذا كان التعبير جديد ClassName()
                    // (EN) Track class type if expression is new ClassName()
                    if (auto *newExpr = dynamic_cast<Sad::AST::NewExpr *>(varDecl->initializer.get()))
                    {
                        b_.classInstanceTypes_[varDecl->name] = newExpr->className;
                    }
                    // ================================================================
                    // (AR) تتبع نوع الصنف من أي مصدر آخر (استدعاء دالة، عامل محمّل، الخ):
                    //      إذا كان التعبير المبدئي يحمل className (مثلاً من b_.buildFunctionCall
                    //      أو b_.buildBinaryOp مع operator overloading)، نُسجّل المتغير
                    //      في b_.classInstanceTypes_ حتى يتمكن المترجم من معرفة نوع الصنف
                    //      عند الوصول لحقول الكائن لاحقاً.
                    //      مثال: متغير ج = أ + ب → إذا عامل + يُرجع كائن نقطة → ج.س يعمل
                    //      مثال: متغير ن = اصنع_نقطة() → ن.س يعمل
                    //      بدون هذا الإصلاح: الوصول لحقول كائن مُرجع من دالة أو عامل يتعطل
                    // (EN) Track class type from any other source (function call, operator, etc):
                    //      If the initializer expression has className (e.g. from b_.buildFunctionCall
                    //      or b_.buildBinaryOp with operator overloading), register the variable
                    //      in b_.classInstanceTypes_ so the compiler knows the class type when
                    //      accessing fields later.
                    //      Without this: accessing fields of object from function/operator crashes
                    // ================================================================
                    else if (!initResult.className.empty())
                    {
                        b_.classInstanceTypes_[varDecl->name] = initResult.className;
                    }
                }

                // ================================================================
                // (AR) م-١٠ (ISSUE-100): «متغير» بالاسم نفسه داخل نطاقٍ مُتضمَّنٍ في
                //      الدالّة نفسِها **يكتب فوق** الخارجيَّ ولا يُظلّلُه — قرارٌ مقصودٌ
                //      في المفسّر (interpreter/src/managers/variable_manager.cpp:84)
                //      وموثَّقٌ في language-truth/grammar/20_declarations.yaml (م-٨).
                //      وكان المترجِمُ يخالفُه **صامتًا** حيثما فُتح نطاقٌ فعلًا: كتلُ
                //      «إذا» لا تفتح نطاقًا فتطابقُ مصادفةً، أمّا كلُّ موضعٍ ينادي
                //      `enterScope()` فيفتح — ومنها ذراعُ «طابق»، و«حاول»/«امسك»،
                //      و«أجّل»، واللامدا/الاستيعاب، وحلقاتُ «لكل» بصيغتَيها.
                //      (لا يُذكَرُ عددٌ هنا عمدًا: عدُّ المواضعِ يختلفُ باختلافِ ما
                //      يُستثنى من تعريفٍ وتعليقٍ ومُتشابِهِ اسمٍ في الخلفيّة، فهو رقمٌ
                //      هشٌّ لا يُضيفُ إلى الحجّةِ شيئًا.)
                //      **المُبرهَنُ حيًّا هنا ذراعُ «طابق»**
                //      وحدَها (055 و001/002 في grammar_gaps)؛ وبقيّةُ المواضع تُغطّيها
                //      **بالآليّة لا بالتعداد**: الإصلاحُ يقعُ في `buildLocalVariable`
                //      فيسري على أيِّ نطاقٍ مفتوحٍ كان. فتُصدَر ALLOC ثانيةٌ
                //      بالاسم نفسِه؛ يُعيد المولّدُ تسميتَها فيكتب STORE في الموقع
                //      الجديد ويقرأ LOAD الخارجيَّ القديم ⇒ نتيجتان مختلفتان بلا خطأ.
                //      الإصلاح نظيرُ قاعدة المفسّر: إن وُجد الاسمُ في نطاقٍ أعلى ضمن
                //      الدالّة (لا عامًّا) نُعيد استعمال موقعِه ولا نُخصّص جديدًا،
                //      فيقع STORE على الموقع الذي يقرؤه LOAD.
                // (EN) م-١٠ (ISSUE-100): a same-named «متغير» in an inner scope of the same
                //      function OVERWRITES the outer one rather than shadowing it — a
                //      deliberate interpreter rule (variable_manager.cpp:84), documented in
                //      20_declarations.yaml. The compiler diverged SILENTLY wherever a scope
                //      was actually pushed: an «إذا» body pushes none (so it matched by
                //      accident), but every enterScope() site does — match arms, try/catch,
                //      defer, lambdas/comprehensions, both for-loop forms. (No count is given:
                //      it shifts with what you exclude — the definition, comments, a same-named
                //      backend method — so it is a brittle number that adds nothing.)
                //      Only the match arm is proven live; the rest are covered by
                //      MECHANISM (the fix sits in buildLocalVariable, so it applies to any
                //      pushed scope) rather than by enumeration. Emitting a second ALLOC under the same
                //      name. The emitter renames it, so the STORE lands in the new slot while
                //      the later LOAD reads the original — two different results, no error.
                // ================================================================
                // (AR) ⚠ شرطُ إعادةِ الاستعمالِ **يجب** أن يشترطَ تطابقَ النوع. الموقعُ
                //      مُخصَّصٌ بنوعِ الأوّل، فإعادةُ استعمالِه لنوعٍ آخرَ تكتبُ عددًا في
                //      موقعِ مؤشّرِ نصٍّ ⇒ SIGSEGV عندَ أوّلِ قراءة (مُبرهَنٌ حيًّا:
                //      «متغير ق = "نص"» ثمّ «متغير ق = 5» كان ينهار). والمفسّرُ لا يقعُ
                //      في هذا لأنّ موقعَه مُعنوَنٌ ديناميكيًّا لا مُخصَّصٌ بنوعٍ ساكن.
                //      وعندَ اختلافِ النوعِ نُخصّصُ ALLOC جديدةً كما كان — أي نعودُ إلى
                //      السلوكِ السابقِ في تلك الحالةِ وحدَها، فلا انحدار.
                //      ويُستثنى المعاملُ والملتقَطُ **تحفّظًا مُعلَنًا لا فرقًا بنيويًّا**:
                //      المعاملُ يستعملُ `%<اسم>` نفسَه فلا يميّزُه شيءٌ في التمثيل، لكنّ
                //      امتلاكَه للموقعِ يعودُ إلى نداءِ الدالّةِ لا إلى هذا التصريح، فتُرك
                //      خارجَ نطاقِ الإصلاحِ حتّى يُقاسَ على حِدَة. و`!isCaptured` **زائدٌ
                //      منطقيًّا** (الملتقَطُ يحملُ `%__cap_…` فيسقطُ بفحصِ اسمِ السجلّ
                //      أصلًا) وأُبقيَ صريحًا ليقرأَه من يأتي بعدُ شرطًا مقصودًا.
                // (EN) ⚠ Slot reuse MUST require a matching type: the slot was allocated with
                //      the first declaration's type, so reusing it for another type writes an
                //      integer into a string-pointer slot ⇒ SIGSEGV on the first read (proven
                //      live). On a type mismatch we emit a fresh ALLOC exactly as before, so
                //      that case keeps its previous behaviour. Parameters and closure-captured
                //      variables are excluded as a STATED PRECAUTION, not a representational
                //      difference: a parameter uses the very same `%<name>` slot. Ownership
                //      comes from the call, not from this declaration, so it stays out of
                //      scope until measured separately. `!isCaptured` is logically redundant
                //      (captures carry `%__cap_…`) and is kept explicit for the next reader.
                // (AR) ولا يكفي أن يكونَ **السابقُ** قياسيًّا: لو صار الجديدُ مصفوفةً
                //      لكُتب مؤشّرُ SadArray في موقعٍ يقرؤه ما بعدُ عددًا. فالطرفانِ معًا.
                // (EN) The PRIOR being scalar is not enough: if the new one is an aggregate we
                //      would store a SadArray pointer into a slot later read as a scalar.
                const bool newTypeIsScalar =
                    varType == SadTypeKind::Integer || varType == SadTypeKind::Float ||
                    varType == SadTypeKind::Boolean || varType == SadTypeKind::String ||
                    varType == SadTypeKind::UInt64 || varType == SadTypeKind::Byte;
                const bool newHasNoAggregateShape =
                    varInfo.className.empty() &&
                    b_.classInstanceTypes_.find(varDecl->name) == b_.classInstanceTypes_.end();

                const bool reuseOuterSlot =
                    // (AR) 🔴 داخلَ جسمِ الماكرو لا إعادةَ استعمالٍ ألبتّة: هذا هو
                    //      الطريقُ الذي قِيس في الشكلِ (ج) — `alloca` واحدةٌ وتخزينان
                    //      فيها. وإعادةُ الاستعمالُ خارجَ الماكرو قرارٌ مقصودٌ يبقى
                    //      (ISSUE-028)، فالاستثناءُ للماكرو وحدَه لا نقضٌ له.
                    // (EN) No slot reuse inside a macro body — the route measured in
                    //      shape (ج): one alloca, two stores. Reuse outside a macro is a
                    //      deliberate decision (ISSUE-028) and stays untouched.
                    !inMacroBody &&
                    priorIsReusableSlot &&
                    priorTypeIsScalar && newTypeIsScalar &&
                    priorHasNoAggregateShape && newHasNoAggregateShape &&
                    priorType == varType;

                // (AR) توليد تعليمة ALLOC لتخصيص الذاكرة
                // (EN) Generate ALLOC instruction for memory allocation
                if (b_.currentBlock_ && !reuseOuterSlot)
                {
                    SIRInstruction allocInst;
                    allocInst.opcode = SIROpcode::ALLOC;
                    allocInst.result = SIROperand::Register(varInfo.registerName, varType);
                    b_.currentBlock_->addInstruction(allocInst);
                }

                // (AR) توليد تعليمة STORE لإسناد القيمة الأولية
                // (EN) Generate STORE instruction to assign initial value
                if (hasInitializer)
                {
                    // (AR) تحقق: هل القيمة ثابتة يمكن استخدامها مباشرة؟
                    // (EN) Check: is the value a usable constant?
                    bool useConstant = initResult.isConstant && (initResult.type == SadTypeKind::String ||
                                                                 !initResult.constantValue.empty());

                    // (AR) تخطي STORE إذا لم تكن القيمة ثابتة ولا في سجل صالح
                    //      هذا يحدث عند فشل b_.buildExpression (مثلاً: متغير غير معرّف)
                    // (EN) Skip STORE if value is neither a usable constant nor in a valid register.
                    //      This happens when b_.buildExpression fails (e.g., undefined variable).
                    if (!useConstant && initResult.registerName.empty())
                    {
                        // Just register the variable without initialization
                        b_.addVariable(varInfo);
                        return;
                    }

                    // (AR) اقتطاع البايت (u8): إسناد التهيئة يُقتطع 0–255 — نظيرٌ حرفيّ
                    //      لاقتطاع المفسّر (`& 0xFF`) حفظًا لتكافؤ المسارَين. الثابت يُطوى
                    //      هنا؛ السجلّ يُقنَّع بتعليمة AND. (الدلالة اللا-موقَّعة الكاملة
                    //      واقتطاع إعادة الإسناد طبقةٌ تاليةٌ مؤجَّلة.)
                    // (EN) Byte (u8) truncation: init assignment truncates to 0-255 — mirrors
                    //      the interpreter's `& 0xFF` to keep both tracks in parity. Constants
                    //      are folded here; registers are masked with an AND instruction.
                    bool truncateByte = (varType == SadTypeKind::Byte);
                    if (truncateByte && useConstant && initResult.type == SadTypeKind::Integer)
                    {
                        long long v = std::stoll(initResult.constantValue) & 0xFF;
                        initResult.constantValue = std::to_string(v);
                    }
                    else if (truncateByte && !useConstant && !initResult.registerName.empty())
                    {
                        SIRInstruction andInst;
                        andInst.opcode = SIROpcode::AND;
                        std::string maskedReg = b_.newTempRegister();
                        andInst.result = SIROperand::Register(maskedReg, SadTypeKind::Integer);
                        andInst.operands.push_back(SIROperand::Register(initResult.registerName, initResult.type));
                        andInst.operands.push_back(SIROperand::ConstantI64(0xFF));
                        b_.currentBlock_->addInstruction(andInst);
                        initResult.registerName = maskedReg;
                        initResult.type = SadTypeKind::Integer;
                    }

                    SIRInstruction storeInst;
                    storeInst.opcode = SIROpcode::STORE;

                    // (AR) المعامل الأول: القيمة المراد تخزينها
                    // (EN) First operand: value to store
                    if (useConstant)
                    {
                        // (AR) القيمة ثابتة - تحويلها لثابت SIR
                        // (EN) Value is constant - convert to SIR constant
                        switch (initResult.type)
                        {
                        case SadTypeKind::Integer:
                            storeInst.operands.push_back(SIROperand::ConstantI64(std::stoll(initResult.constantValue)));
                            break;
                        case SadTypeKind::Float:
                            storeInst.operands.push_back(SIROperand::ConstantF64(std::stod(initResult.constantValue)));
                            break;
                        case SadTypeKind::Boolean:
                            storeInst.operands.push_back(SIROperand::ConstantBool(initResult.constantValue == "true" || initResult.constantValue == "1"));
                            break;
                        case SadTypeKind::String:
                            storeInst.operands.push_back(SIROperand::ConstantString(initResult.constantValue));
                            break;
                        case SadTypeKind::Function:
                            // (AR) تخزين مؤشر دالة (لامدا أو دالة عادية)
                            //      نستخدم SIROperand::Function لتجنب خطأ "Undefined register"
                            //      لأن اللامدا لا تُنتج تعليمة SIR تُعرّف سجلاً — فقط SIRFunction
                            // (EN) Store function pointer (lambda or regular function)
                            //      Use SIROperand::Function to avoid "Undefined register" error
                            //      since lambda doesn't emit a SIR instruction that defines a register
                            storeInst.operands.push_back(SIROperand::Function(initResult.constantValue));
                            break;
                        default:
                            storeInst.operands.push_back(SIROperand::Register(initResult.registerName, initResult.type));
                            break;
                        }
                    }
                    else if (initResult.type == SadTypeKind::Void)
                    {
                        // (AR) SEM045 (دَين الخانة المجرَّدة): نداءٌ بلا قيمةٍ لا يملك سجلًّا
                        //      يُقرأ — سجلُّه لا يقابله شيءٌ في LLVM. النداءُ نفسُه صدر أعلاه
                        //      (أثرُه الجانبيُّ باقٍ)، والمخزونُ ثابتُ الفراغِ فتُوسَم الخانةُ
                        //      الديناميّةُ Void — فتطبع «لاشيء» و«نوع()» فراغًا كالمفسّر.
                        // (EN) SEM045 (bare-slot debt): a value-less call has no readable
                        //      result register in LLVM. The call itself was emitted above
                        //      (side effect preserved); store the Void constant so the
                        //      dynamic slot is tagged Void — printing «لاشيء» and نوع()
                        //      «فراغ», matching the interpreter.
                        storeInst.operands.push_back(SIROperand::ConstantVoid());
                    }
                    else
                    {
                        // (AR) القيمة في سجل
                        // (EN) Value is in register
                        storeInst.operands.push_back(SIROperand::Register(initResult.registerName, initResult.type));
                    }

                    // (AR) المعامل الثاني: عنوان المتغير
                    // (EN) Second operand: variable address
                    storeInst.operands.push_back(SIROperand::Register(varInfo.registerName, varType));

                    b_.currentBlock_->addInstruction(storeInst);
                }
                else if (b_.currentBlock_ && !reuseOuterSlot)
                {
                    // ════════════════════════════════════════════════════════════
                    // (AR) 🔑 تصريحٌ بلا تهيئة: كانت الخانةُ تُحجَز ولا تُكتَب، فيقرأ
                    //      المترجّمُ مكدّسًا غيرَ مهيّأ. مقيسٌ (٢٠٢٦-٠٨-١٤): «رقم س»
                    //      داخل دالّةٍ يطبع ‎140733877452800‎ وقيمةً **تختلف في كلِّ
                    //      تشغيل**، و«منطقي ج» يطبع «صحيح»، بينما المفسّرُ يطبع ‎0‎
                    //      و«خطأ». ورمزُ خروجِ البناءِ صفرٌ في الحالتين، فلا شيءَ يشي.
                    //
                    // (AR) والقيمةُ الافتراضيّةُ تتبعُ **قابليّةَ النوعِ للعدم**، وهذا
                    //      هو ما يجعلها متّسقةً مع أمان العدم لا مجرّدَ تصفيرٍ أعمى:
                    //        • نوعٌ يقبلُ العدم («رقم عدمي» · «نص؟») ⇒ «لاشيء».
                    //        • نوعٌ لا يقبله ⇒ قيمتُه الصفريّة (0 · 0.0 · خطأ · "").
                    //      فحشوُ «لاشيء» في «رقم» غيرِ العدميِّ يخرق العقدَ الذي
                    //      يقوم عليه أمانُ العدم: نوعٌ غيرُ اختياريٍّ لا يحمل عدمًا.
                    //
                    // (AR) والأنواعُ المركّبة (مصفوفة/خريطة/أي/كائن) تُترَك كما كانت:
                    //      قيمتُها الافتراضيّةُ ليست ثابتًا بسيطًا، وتصفيرُها هنا
                    //      ادّعاءٌ لا قياسَ له. حدٌّ مُعلَنٌ لا مسكوتٌ عنه.
                    // (EN) 🔑 Declaration with no initializer: the slot was allocated and never
                    //      written, so the compiler read uninitialized stack — measured as a
                    //      value that CHANGES on every run, with build exit code 0. The default
                    //      now follows the type's NULLABILITY: a nullable type defaults to null,
                    //      a non-nullable one to its zero value. Filling a non-nullable «رقم»
                    //      with null would break the very contract null-safety rests on.
                    //      Aggregates (array/map/any/object) are deliberately left alone.
                    // ════════════════════════════════════════════════════════════
                    const bool declaredNullable =
                        varDecl->sadType && varDecl->sadType->isNullable();

                    std::optional<SIROperand> defaultValue;
                    if (declaredNullable)
                    {
                        // ════════════════════════════════════════════════════════
                        // (AR) ⚠️ «عشري عدمي» بلا تمثيلٍ للعدم — عيبٌ مُعلَنٌ لا مسدود
                        // ════════════════════════════════════════════════════════
                        //
                        // (AR) الحارسُ `kSadNullSentinel` عددٌ صحيحٌ بعرضِ ٦٤ بتًّا،
                        //      ولا نظيرَ له للعائم. فـ«عشري عدمي ع» يُخزَّن حارسًا
                        //      يُعاد تفسيرُ بتّاتِه عائمةً، فيطبع المترجّمُ
                        //      `-9223372036854775808.0` بينما يطبع المفسّرُ «لاشيء».
                        //
                        //      🔑 وجرّبتُ تخطّيَ التخزينِ للعائمِ فصار المخرَجُ `0.0`
                        //      حتميًّا — **فتراجعتُ عنه**: «0.0» قيمةٌ **معقولةٌ**
                        //      كاذبةٌ في نوعٍ عدميّ، و«-9.2e18» كاذبةٌ **مستنكَرة**.
                        //      وحين لا يتاح الصوابُ فالخطأُ المستنكَرُ أسلمُ من
                        //      الخطأِ المعقول: هذا يُلاحَظ فيُبلَّغ، وذاك يُصدَّق
                        //      فيُبنى عليه. ولا أُبدِل كذبةً صاخبةً بكذبةٍ هادئةٍ
                        //      وأسمّي ذلك إصلاحًا.
                        //
                        //      والسدُّ الحقيقيُّ يقتضي تمثيلًا للعدمِ في خانةٍ عائمة
                        //      (تعليبٌ أو حمولةُ NaN) — قرارُ تصميمٍ لا رقعةُ سطر.
                        //      تحرسه البذرةُ `VE043` حمراءَ مُعلَنةً حتّى يُقرَّر.
                        //
                        // (AR) 🔑 والحارسُ يُكتَب **حيث يُفهَم فقط**. قِيس (2026-08-15)
                        //      أنّ كتابتَه في كلِّ نوعٍ عدميٍّ تسرّبه خامًا في أربعة:
                        //        رقم عدمي     ⇒ «لاشيء»  ✅ (مسارُ الطباعةِ يفحصه)
                        //        نص عدمية     ⇒ «لاشيء»  ✅ (مسارُ النصِّ يفحصه)
                        //        بايت/طبيعي64 ⇒ -9223372036854775807  🔴 مستثنيان من
                        //                       فحصِ الحارسِ عمدًا (تصادمُ قيمةٍ شرعيّة)
                        //        منطقي عدمي   ⇒ «صحيح»   🔴 يُبتَر إلى بتٍّ واحد
                        //        عشري عدمي    ⇒ -9.2e18  🔴 يُعاد تفسيرُه عائمًا
                        //      و«صحيح» أخبثُها: قيمةٌ **معقولةٌ حتميّة** — وهي عينُ
                        //      الكذبةِ الهادئةِ التي أعلنتُ رفضَها في الفقرةِ أعلاه
                        //      ثمّ أَحدثتُها في هذه الكتلةِ نفسِها لنوعٍ آخر.
                        //      فيُقصَر التخزينُ على النوعَين اللذَين يفهمهما الطابعُ،
                        //      ويبقى الباقي على ما كان **قبل** هذه الرقعةِ: بلا كتابة.
                        //      ⚠️ 🔑 **وتصويبُ ادّعاءٍ كتبتُه هنا ثمّ نقضه القياس:**
                        //      كتبتُ أنّ ما يبقى بلا كتابةٍ «عطبٌ صاخبٌ يفضح نفسَه
                        //      (قيمةٌ تختلف كلَّ تشغيل)». وقياسُ الثمانِ خاناتٍ
                        //      (أربعةُ أنواعٍ × نطاقَين، خمسُ تشغيلاتٍ لكلٍّ) يقول غيرَ
                        //      ذلك: **ستٌّ من ثمانٍ حتميّة**. داخلَ دالّةٍ يتغيّر
                        //      «بايت» و«طبيعي64» وحدَهما، أمّا «منطقي» فيعطي «صحيح»
                        //      حتميًّا و«عشري» يعطي `0.0` حتميًّا — و`0.0` هي بعينِها
                        //      القيمةُ التي أعلنتُ أعلاه أنّي تراجعتُ عن كتابتِها لأنّها
                        //      «كذبةٌ معقولة». وعلى مستوى الوحدةِ **الثمانِ كلُّها
                        //      حتميّة** (خطأ · 0.0 · 0 · 0).
                        //      فالتقييدُ **صحيحٌ في وجهِه الآخر** — يمنع كتابةَ حارسٍ
                        //      يُفَكُّ خطأً — ولا يصحّ تعليلُه بأنّه اختار الصخبَ:
                        //      لم يكن ثمّةَ صخبٌ ليُختار. والحدُّ الحقيقيُّ أنّ هذه
                        //      الأنواعَ **بلا تمثيلٍ للعدمِ أصلًا**، وأيُّ قيمةٍ تبقى
                        //      في خانتِها كذبةٌ — صاخبةً كانت أو هادئة.
                        //      ودرسُه: **تعليلٌ يُكتَب قبل القياسِ يصير ادّعاءً**،
                        //      ولو كان القرارُ الذي يعلّله صحيحًا.
                        // (EN) CORRECTION, measured: the claim that the un-stored slots
                        //      are "loud (a value that changes every run)" is false —
                        //      6 of 8 measured slots are deterministic; at module scope
                        //      all 8 are. Boolean yields «صحيح» and Float yields 0.0 —
                        //      the very value rejected above as a plausible lie. The
                        //      restriction is still right, but not for that reason:
                        //      these types have NO null representation at all.
                        // (EN) The sentinel is written ONLY where the print path
                        //      understands it (Integer, String). Byte/UInt64 are
                        //      deliberately excluded from the null check (legal-value
                        //      collision), Boolean truncates it to «true», and Float
                        //      reinterprets it. Those revert to the pre-patch state:
                        //      no store — loud, nondeterministic, and honest.
                        // ════════════════════════════════════════════════════════
                        // ════════════════════════════════════════════════════════
                        // (AR) ✅ م‑ب/م‑ج/م‑د: القيدُ أعلاه **بطَل** للأنواعِ المهاجَرة
                        // ════════════════════════════════════════════════════════
                        //
                        // (AR) كلُّ ما قيل أعلاه («لا تمثيلَ للعدمِ في هذه الأنواع»،
                        //      «أيُّ قيمةٍ تبقى في خانتِها كذبة») كان **صحيحًا حين
                        //      كُتِب**، وقد زال سببُه: النوعُ العدميُّ الذي يُرجِع له
                        //      `sirNullableNeedsOutOfBandTag` صوابًا صار يُخزَّن `Any`
                        //      أي `%SadDyn` = {وسمٌ i8، حمولةٌ i64} — ووسمُ `Null` فيه
                        //      **لا تنازعُه قيمةٌ مشروعة**. فلا كذبةَ صاخبةً ولا هادئة:
                        //      الخانةُ تحمل عدمًا يُقرَأ عدمًا.
                        //
                        // (AR) 🔑 والتقريرُ أعلاه **يُترَك ولا يُمحى**: هو سجلُّ لماذا
                        //      كان التقييدُ صوابًا، وفيه تصويبُ ادّعاءٍ نقضه القياس.
                        //      محوُه يجعل السطرَ الجديدَ يبدو بديهيًّا وقد كلّف قياسَ
                        //      ثمانِ خاناتٍ في خمسِ تشغيلات.
                        //
                        // (AR) ⚠️ ويبقى `Integer`/`String` على الحارسِ داخلَ النطاقِ
                        //      حتّى **م‑هـ**: فحصُهما مفهومٌ في مسارِ الطباعةِ اليوم،
                        //      وهجرتُهما مرحلةٌ قائمةٌ بذاتها لها بوّابتُها.
                        // (EN) م‑ب/م‑ج/م‑د: the restriction above is obsolete for migrated
                        //      kinds. A nullable kind for which sirNullableNeedsOutOfBandTag
                        //      is true is now stored as `Any` (%SadDyn), whose `Null` tag no
                        //      legitimate value can contend for — so there is no lie left to
                        //      choose between, loud or quiet. The rationale above is KEPT, not
                        //      deleted: it records why the restriction was right and carries a
                        //      measured correction of a claim written before measuring.
                        //      Integer/String stay on the in-band sentinel until م‑هـ.
                        // ════════════════════════════════════════════════════════
                        if (varType == SadTypeKind::Any)
                        {
                            // (AR) الوسمُ `Null` هو ما تُعلّبه `toDyn` وتقرؤه
                            //      `dynToString`/`dynCompare` — نظيرُ `makeOmittedArgPad`.
                            // (EN) The `Null` tag is what toDyn packs and dynToString /
                            //      dynCompare read — mirroring makeOmittedArgPad.
                            SIROperand dynNullDefault =
                                SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel);
                            dynNullDefault.dataType = SadTypeKind::Null;
                            defaultValue = dynNullDefault;
                        }
                        else if (varType == SadTypeKind::Integer || varType == SadTypeKind::String)
                        {
                            defaultValue = SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel);
                        }
                    }
                    else
                    {
                        // ════════════════════════════════════════════════════
                        // (AR) 🔑 القرارُ من مصدرِ الحقيقة، والتمثيلُ وحدَه هنا
                        // ════════════════════════════════════════════════════
                        //
                        // (AR) كان هذا الجدولُ يوزّع على أنواعِ اللغةِ كلِّها بيدِه،
                        //      وله خمسُ شقيقاتٍ في المفسّرِ ووقتِ التشغيل. صار يوزّع
                        //      على **سبعِ مفرداتٍ** يُقرّرها
                        //      language-truth/types.yaml في حقلِ default_init،
                        //      ويُولَّد منها SAD_TYPE_DEFAULT_INIT_TABLE. فالقرارُ
                        //      واحدٌ في مكانٍ واحد، والتمثيلُ (SIROperand) يبقى هنا.
                        //
                        //      🔑 وهذا بعينُه ما كان يمنع علّةَ «طبيعي64» المُدوَّنةَ
                        //      أدناه: لم تكن غيابَ فهمٍ بل غيابَ سطرٍ في نسخةٍ من ستّ.
                        // (EN) This table used to enumerate the language's kinds by hand,
                        //      with five siblings elsewhere. It now switches on SEVEN
                        //      words decided by language-truth/types.yaml (default_init)
                        //      and generated into SAD_TYPE_DEFAULT_INIT_TABLE: one
                        //      decision in one place, representation stays local.
                        // ════════════════════════════════════════════════════
                        // (AR) 🔑 «طبيعي64» كان غائبًا عن هذا الجدولِ وحدَه بينما
                        //      أُضيف إلى نظائرِه الثلاثةِ في المفسّرِ ومصدرِ الحقيقة.
                        //      قِيس أثرُ غيابِه (٤ تشغيلاتٍ للثنائيِّ نفسِه):
                        //      47139388169 · 355351788617 · 129422588889 · 18615432889
                        //      — أي أنّ العطبَ الذي يعلن التعليقُ أعلاه أنّه أُغلق
                        //      **بقي حيًّا لنوعٍ واحد**. ودرسُه: جدولٌ يُعدَّل يدويًّا
                        //      في أربعةِ مواضعَ ينجرف في الموضعِ الذي يُنسى.
                        // (EN) UInt64 was missing HERE only, while its three siblings
                        //      got it — so the "fixed" uninitialized-stack read stayed
                        //      alive for exactly one type, measured changing every run.
                        static_assert(Sad::Types::SAD_DEFAULT_INIT_COUNT == 8,
                                      "(AR) تغيّرت مفرداتُ default_init — قرّرْ تمثيلَ "
                                      "المفردةِ الجديدةِ هنا صراحةً. (EN) The default_init "
                                      "vocabulary changed — decide the new word's "
                                      "representation here explicitly.");

                        switch (Sad::Types::sadTypeKindDefaultInit(varType))
                        {
                        case Sad::Types::SadDefaultInit::IntZero:
                            defaultValue = SIROperand::ConstantI64(0);
                            break;
                        case Sad::Types::SadDefaultInit::FloatZero:
                            defaultValue = SIROperand::ConstantF64(0.0);
                            break;
                        case Sad::Types::SadDefaultInit::BoolFalse:
                            defaultValue = SIROperand::ConstantBool(false);
                            break;
                        case Sad::Types::SadDefaultInit::EmptyString:
                            defaultValue = SIROperand::ConstantString(std::string());
                            break;
                        // ════════════════════════════════════════════════════
                        // (AR) 🔑 خانةُ «أي» المجرَّدة: **فراغٌ** لا صفرٌ ولا عدم
                        // ════════════════════════════════════════════════════
                        //
                        // (AR) كانت تسقط في `default: break` فلا يُصدَر تخزينٌ
                        //      البتّةَ، فتُقرأ الخانةُ على ما تركه المكدّس. والمقيسُ
                        //      (٢٠٢٦-٠٨-١٧) أنّ `نوع()` عليها كان يُجيب **ثلاثةَ
                        //      أجوبةٍ مختلفة** بحسبِ الموضعِ لا بحسبِ المعنى:
                        //        `متغير ك` في المستوى الأعلى ⇒ «رقم»
                        //        `أي م`   في المستوى الأعلى ⇒ «عدم»
                        //        `أي م`   داخلَ دالّةٍ       ⇒ «مجهول»
                        //      والمرجعُ يقول «فراغ» في الثلاثة.
                        //
                        //      ⚠️ ولا يجوز أن يُملأ بالحارسِ `kSadNullSentinel`:
                        //      ذلك يجعلها «عدمًا» فيضيع الفرقُ بين «لم تُسنَد بعدُ»
                        //      و«أُسنِد إليها العدمُ عمدًا» — وهو فرقٌ يحمله المرجعُ
                        //      في نصِّه ويقوم عليه تشخيصُ الاستعمالِ قبل الإسناد.
                        // (EN) A bare `أي` slot is VOID — not zero, not null. It used to
                        //      fall through with no store at all, so نوع() answered three
                        //      different things depending on position, none of them «فراغ».
                        case Sad::Types::SadDefaultInit::Void:
                            defaultValue = SIROperand::ConstantVoid();
                            break;
                        // ════════════════════════════════════════════════════
                        // (AR) العدمُ لا يُقرَّر هنا — قرارُه في الفرعِ الذي فوق
                        // ════════════════════════════════════════════════════
                        //
                        // (AR) هذا الفرعُ هو `else` لـ`declaredNullable`، والخانةُ
                        //      العدميّةُ يقرّر تمثيلَها الفرعُ الأوّلُ (وسمُ `Null` في
                        //      `%SadDyn` أو الحارسُ داخلَ النطاق). فبلوغُ العدمِ هنا
                        //      يعني نوعًا اسمُه «عدم» بلا لاحقةِ `عدمي` — ولا تخزينَ
                        //      له اليومَ، كما كان قبلَ التوحيدِ حرفًا بحرف.
                        //      ⚠️ **دَينٌ مُعلَنٌ لا مسدود**: المفسّرُ يُعطيه «لاشيء»،
                        //      وسدُّه تغييرُ سلوكٍ يلزمه بذرةٌ تُحمِّر أوّلًا — فلا
                        //      يُدَسّ في رقعةٍ غايتُها **ألّا تُغيّر شيئًا**.
                        // (EN) This is the `else` of `declaredNullable`; the nullable slot's
                        //      representation is decided by the branch above. Reaching Null
                        //      here means a type literally named «عدم» without the `عدمي`
                        //      suffix — no store today, byte-for-byte as before this
                        //      unification. Declared debt: the interpreter yields null here.
                        //      Closing it is a behaviour change that needs its own reddening
                        //      seed, so it is NOT smuggled into a no-op refactor.
                        // ════════════════════════════════════════════════════
                        case Sad::Types::SadDefaultInit::Null:
                        // (AR) «غيرُ مُعلَن»: مصدرُ الحقيقةِ لا يُقرّر افتراضيًّا لهذا
                        //      النوع (مصفوفةٌ وخريطةٌ اليوم) ⇒ لا تخزين، كما كان.
                        // (EN) Undeclared: the SoT decides no default (array/map today).
                        case Sad::Types::SadDefaultInit::Unspecified:
                        // (AR) لا خانةَ تحمله — مرفوضٌ في المحلّلِ المشترك
                        //      (SEM040) قبلَ بلوغِ البناء. ذراعٌ دفاعيّةٌ مُعلَنة،
                        //      ولا تُدمَج مع Unspecified: تلك دَينٌ وهذه استحالة.
                        // (EN) No slot can hold it — rejected in the shared parser
                        //      before building. Declared defensive arm; kept apart
                        //      from Unspecified, which is a debt rather than an
                        //      impossibility.
                        case Sad::Types::SadDefaultInit::NotASlot:
                            break;
                        }
                    }

                    if (defaultValue.has_value())
                    {
                        SIRInstruction zeroInit;
                        zeroInit.opcode = SIROpcode::STORE;
                        zeroInit.operands.push_back(*defaultValue);
                        zeroInit.operands.push_back(
                            SIROperand::Register(varInfo.registerName, varType));
                        zeroInit.comment = "default init (no initializer)";
                        b_.currentBlock_->addInstruction(zeroInit);
                    }
                }

                // (AR) إضافة المتغير للنطاق (sir_builder.h:591 - b_.addVariable)
                // (EN) Add variable to scope
                b_.addVariable(varInfo);

                // (AR) [نظام الإغلاقات الجديد] لم نعد نُسجّل b_.lambdaAliases_ هنا
                //      جميع استدعاءات اللامدا تمر عبر CLOSURE_CALL (الخطوة 3.5)
                //      بدلاً من الاستدعاء المباشر عبر الاسم المستعار (الخطوة 2.7)
                // (EN) [New closure system] No longer register b_.lambdaAliases_ here
                //      All lambda calls go through CLOSURE_CALL (Step 3.5)
                //      instead of direct call via alias (Step 2.7)
            }

            // ============================================================================
            // buildIfStatement - بناء جملة if
            // ============================================================================
            // مصدر التعريف / Source: sir_builder.h:385
            // التوقيع / Signature: void buildIfStatement(AST::IfStmt* ifStmt);
            //
            // المعاملات / Parameters:
            // - ifStmt: AST::IfStmt* = Sad::AST::IfStmt* (sir_builder.h:385)
            //
            // IfStmt Members (statements.h:104-135):
            // - condition: ExprPtr (line 107)
            // - thenBranch: StmtPtr (line 108)
            // - elseBranch: StmtPtr (line 109) - optional, can be nullptr
            //
            // المتغيرات المستخدمة / Used variables:
            // - b_.currentBlock_: sir_builder.h:582 (shared_ptr<SIRBasicBlock>)
            //
            // الدوال المستدعاة / Called functions:
            // - b_.buildExpression: sir_builder.h:432
            // - buildStatement: sir_builder.h:372
            // - b_.createBasicBlock: sir_builder.h:501
            // - b_.newLabel: sir_builder.h:520
            // ============================================================================
            void StatementBuilder::buildIfStatement(AST::IfStmt *ifStmt)
            {
                if (!ifStmt)
                {
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildIfStatement: starting" << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 1: إنشاء الكتل الأساسية
                // (EN) Step 1: Create basic blocks
                // المصدر: sir_builder.h:501 - b_.createBasicBlock()
                // المصدر: sir_builder.h:520 - b_.newLabel()
                // ========================================================================
                std::string thenLabel = b_.newLabel("then");
                std::string mergeLabel = b_.newLabel("merge");

                // (AR) كتلة else فقط إذا كان هناك فرع else
                // (EN) Else block only if there's an else branch
                std::string elseLabel = ifStmt->elseBranch ? b_.newLabel("else") : mergeLabel;

                auto thenBlock = b_.createBasicBlock(thenLabel);
                auto elseBlock = ifStmt->elseBranch ? b_.createBasicBlock(elseLabel) : nullptr;
                auto mergeBlock = b_.createBasicBlock(mergeLabel);

                // (AR) إضافة الكتل إلى الدالة الحالية
                // (EN) Add blocks to current function
                if (b_.currentFunction_)
                {
                    b_.currentFunction_->addBasicBlock(thenBlock);
                    if (elseBlock)
                        b_.currentFunction_->addBasicBlock(elseBlock);
                    b_.currentFunction_->addBasicBlock(mergeBlock);
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildIfStatement: created blocks then=" << thenLabel
                          << ", else=" << elseLabel << ", merge=" << mergeLabel << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 2: بناء الشرط
                // (EN) Step 2: Build condition expression
                // المصدر: IfStmt::condition (statements.h:107)
                // ========================================================================
                auto condResult = b_.buildExpression(ifStmt->condition.get());

                if (condResult.registerName.empty())
                {
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildIfStatement: condition build failed!" << std::endl;
#endif
                    b_.errors_.push_back("Error: Failed to build if condition");
                    return;
                }

#ifndef NDEBUG
                std::cout << "[DEBUG] buildIfStatement: condition reg=" << condResult.registerName << std::endl;
#endif

                // ========================================================================
                // (AR) الخطوة 2.5: تحويل تلقائي لـ __op_tobool__ إذا كان الشرط كائناً
                //      مثال: إذا (كائن) → إذا (__op_tobool__(كائن))
                //      يتوافق مع: expression_evaluator_calls.cpp findOperator("منطقي")
                // (EN) Step 2.5: Auto-convert __op_tobool__ if condition is an object
                //      Example: if (object) → if (__op_tobool__(object))
                //      Matches: interpreter's findOperator("منطقي")
                // ========================================================================
                b_.coerceObjectToBool(condResult);

                // ========================================================================
                // (AR) الخطوة 3: توليد تعليمة القفز الشرطي
                // (EN) Step 3: Generate conditional branch instruction
                // المصدر: sir_instruction.h:190-197 - SIRInstruction::BranchCond()
                // المصدر: sir_types.h:366-372 - SIROperand::Label()
                // ========================================================================
                // (AR) إذا كان الشرط ثابتاً منطقياً (صحيح/خطأ)، نستخدم ConstantBool بدلاً من Register
                //      لتجنب توليد سجل غير معرّف في LLVM IR
                // (EN) If condition is a boolean constant (true/false), use ConstantBool instead of Register
                //      to avoid generating an undefined register in LLVM IR
                SIROperand condOp;
                if (condResult.isConstant && condResult.type == SadTypeKind::Boolean)
                {
                    condOp = SIROperand::ConstantBool(condResult.constantValue == "true" || condResult.constantValue == "1");
                }
                else if (condResult.isConstant && condResult.type == SadTypeKind::Integer)
                {
                    condOp = SIROperand::ConstantI64(std::stoll(condResult.constantValue));
                }
                else
                {
                    condOp = SIROperand::Register(condResult.registerName, condResult.type);
                }
                SIROperand thenLabelOp = SIROperand::Label(thenLabel);
                SIROperand elseLabelOp = SIROperand::Label(elseLabel);

                SIRInstruction brCondInst = SIRInstruction::BranchCond(condOp, thenLabelOp, elseLabelOp);

                if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brCondInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildIfStatement: added BR_COND to current block" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) الخطوة 4: بناء فرع then
                // (EN) Step 4: Build then branch
                // المصدر: IfStmt::thenBranch (statements.h:108)
                // ========================================================================
                b_.currentBlock_ = thenBlock;
                if (ifStmt->thenBranch)
                {
                    buildStatement(ifStmt->thenBranch.get());
                }

                // (AR) قفز غير شرطي إلى merge (sir_instruction.h:178-183)
                // (EN) Unconditional jump to merge
                // (AR) لا نضيف القفز إذا كان الفرع قد انتهى بـ RET أو BR أو BR_COND
                // (EN) Don't add branch if the block already ends with RET or BR or BR_COND
                SIROperand mergeLabelOp = SIROperand::Label(mergeLabel);
                SIRInstruction brMergeInst = SIRInstruction::Branch(mergeLabelOp);

                if (b_.currentBlock_ && !b_.currentBlock_->instructions.empty())
                {
                    const auto &lastInst = b_.currentBlock_->instructions.back();
                    bool hasTerminator = (lastInst.opcode == SIROpcode::RET ||
                                          lastInst.opcode == SIROpcode::RET_VOID ||
                                          lastInst.opcode == SIROpcode::BR ||
                                          lastInst.opcode == SIROpcode::BR_COND);
                    if (!hasTerminator)
                    {
                        b_.currentBlock_->instructions.push_back(brMergeInst);
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildIfStatement: added BR to merge from then" << std::endl;
#endif
                    }
                    else
                    {
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildIfStatement: then block already has terminator, skipping BR" << std::endl;
#endif
                    }
                }
                else if (b_.currentBlock_)
                {
                    b_.currentBlock_->instructions.push_back(brMergeInst);
#ifndef NDEBUG
                    std::cout << "[DEBUG] buildIfStatement: added BR to merge from then (empty block)" << std::endl;
#endif
                }

                // ========================================================================
                // (AR) الخطوة 5: بناء فرع else إذا وُجد
                // (EN) Step 5: Build else branch if exists
                // المصدر: IfStmt::elseBranch (statements.h:109)
                // ========================================================================
                if (ifStmt->elseBranch && elseBlock)
                {
                    b_.currentBlock_ = elseBlock;
                    buildStatement(ifStmt->elseBranch.get());

                    // (AR) قفز غير شرطي إلى merge
                    // (EN) Unconditional jump to merge
                    // (AR) لا نضيف القفز إذا كان الفرع قد انتهى بـ RET أو BR أو BR_COND
                    // (EN) Don't add branch if the block already ends with RET or BR or BR_COND
                    if (b_.currentBlock_ && !b_.currentBlock_->instructions.empty())
                    {
                        const auto &lastInst = b_.currentBlock_->instructions.back();
                        bool hasTerminator = (lastInst.opcode == SIROpcode::RET ||
                                              lastInst.opcode == SIROpcode::RET_VOID ||
                                              lastInst.opcode == SIROpcode::BR ||
                                              lastInst.opcode == SIROpcode::BR_COND);
                        if (!hasTerminator)
                        {
                            b_.currentBlock_->instructions.push_back(brMergeInst);
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildIfStatement: added BR to merge from else" << std::endl;
#endif
                        }
                        else
                        {
#ifndef NDEBUG
                            std::cout << "[DEBUG] buildIfStatement: else block already has terminator, skipping BR" << std::endl;
#endif
                        }
                    }
                    else if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(brMergeInst);
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildIfStatement: added BR to merge from else (empty block)" << std::endl;
#endif
                    }
                }

                // ========================================================================
                // (AR) الخطوة 6: الاستمرار بعد if
                // (EN) Step 6: Continue after if statement
                // ========================================================================
                b_.currentBlock_ = mergeBlock;
#ifndef NDEBUG
                std::cout << "[DEBUG] buildIfStatement: completed, now at merge block" << std::endl;
#endif
            }

            // ============================================================================
            // buildMatchStatement - بناء جملة match (Pattern Matching)
            // ============================================================================
            // (AR) تحويل جملة match إلى سلسلة من BR_COND/BR باستخدام SIR الموجود
            // (EN) Lower match statement to chain of BR_COND/BR using existing SIR
            //
            // (AR) الاستراتيجية:
            // لكل case:
            //   1. كتلة اختبار: مقارنة النمط مع القيمة
            //   2. كتلة guard (إن وجد): تقييم الشرط الإضافي
            //   3. كتلة الجسم: تنفيذ الكود
            //   4. قفز إلى كتلة النهاية
            //
            // (EN) Strategy:
            // For each case:
            //   1. Test block: compare pattern with value
            //   2. Guard block (if exists): evaluate guard condition
            //   3. Body block: execute code
            //   4. Jump to merge block
            // ============================================================================

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
