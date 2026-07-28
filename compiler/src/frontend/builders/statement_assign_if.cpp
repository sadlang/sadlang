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
#include <stdexcept>
#include <iostream>
#include <filesystem>

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
                    // (AR) متغير ثابت لا يمكن تعديله
                    // (EN) Constant variable cannot be modified
                    b_.errors_.push_back("Cannot assign to const variable: " + assignment->name);
                    return;
                }

                // (AR) بناء قيمة التعبير (AssignExpr::value: ExprPtr, line 250)
                // (EN) Build value expression
                auto valueResult = b_.buildExpression(assignment->value.get());

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
                if (b_.module_ && varDecl->isConst)
                {
                    auto sirGlobal = b_.module_->getGlobalVariable(varDecl->name);
                    if (sirGlobal && sirGlobal->isConstant && !sirGlobal->initialValue.empty())
                    {
                        // (AR) الثابت العام مُهيأ بالفعل — لا حاجة لكود إضافي
                        // (EN) Const global already initialized — no code needed
                        return;
                    }
                }

                // (AR) تحويل النوع (VarDeclStmt::type: Types::SadTypeKind, line 77)
                // (EN) Convert type
                SadTypeKind varType = b_.astTypeToSIRType(varDecl->type);
                bool needsTypeInference = (varDecl->type == Types::SadTypeKind::Unknown);

                // (AR) [NS-06 موجة 2] نوع اختياريّ `T؟`: استعمل النوع الداخليّ (T) للتخزين/
                //      التحميل كي تحتفظ القيمة الحاضرة بنوعها الصحيح (نص لا i64)، فلا
                //      يُحوَّل مؤشّر النصّ عدديًّا في `؟؟`. تمثيل العدم يبقى الحارس i64
                //      (يُفحَص في `؟؟`/المقارنات) وهو متوافق مع كلّ الأنواع. النوع الداخليّ
                //      مُسبَّك الآن عبر sadType = Optional<T> من المحلّل النحويّ.
                // (EN) [NS-06 wave 2] Optional `T?`: use inner type T for storage/load so a
                //      present value keeps its real type (string not i64); null stays the i64
                //      sentinel. Inner type now plumbed via sadType = Optional<T> from parser.
                if (varDecl->type == Types::SadTypeKind::Optional && varDecl->sadType)
                {
                    if (auto *opt = dynamic_cast<const Sad::Types::SadOptionalType *>(varDecl->sadType.get()))
                        if (opt->getInnerType())
                            varType = b_.astTypeToSIRType(opt->getInnerType()->getKind());
                }

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
                varInfo.registerName = "%" + varDecl->name;
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
                        varType = initResult.type;
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
                        if (b_.module_)
                        {
                            auto sirGlobal = b_.module_->getGlobalVariable(varDecl->name);
                            if (sirGlobal && sirGlobal->type != varType)
                            {
                                sirGlobal->type = varType;
                            }
                        }
                        // (AR) تحديث VariableInfo المُسجل مسبقاً في النطاق العام أيضاً
                        // (EN) Also update pre-registered VariableInfo in global scope
                        VariableInfo *existingVar = b_.lookupVariable(varDecl->name);
                        if (existingVar && existingVar->type != varType)
                        {
                            existingVar->type = varType;
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

                // (AR) توليد تعليمة ALLOC لتخصيص الذاكرة
                // (EN) Generate ALLOC instruction for memory allocation
                if (b_.currentBlock_)
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
                {
                    std::string condClassName = condResult.className;
                    if (condClassName.empty() && !condResult.registerName.empty())
                    {
                        auto it = b_.classInstanceTypes_.find(condResult.registerName);
                        if (it != b_.classInstanceTypes_.end())
                            condClassName = it->second;
                    }
                    if (!condClassName.empty())
                    {
                        // (AR) بحث في سلسلة الوراثة عن __op_tobool__
                        // (EN) Search inheritance chain for __op_tobool__
                        std::string searchClass = condClassName;
                        std::string toboolName;
                        bool foundToBool = false;
                        while (!searchClass.empty())
                        {
                            toboolName = searchClass + ".__op_tobool__";
                            if (b_.functionTable_.find(toboolName) != b_.functionTable_.end())
                            {
                                foundToBool = true;
                                break;
                            }
                            auto classInfo = b_.module_->getClass(searchClass);
                            if (classInfo && !classInfo->parentClass.empty())
                                searchClass = classInfo->parentClass;
                            else
                                break;
                        }
                        if (foundToBool)
                        {
                            std::string boolReg = b_.newTempRegister();
                            SIRInstruction callInst;
                            callInst.opcode = SIROpcode::OBJECT_CALL;
                            callInst.result = SIROperand::Register(boolReg, SadTypeKind::Boolean);
                            callInst.operands.push_back(SIROperand::Register(condResult.registerName, condResult.type));
                            callInst.operands.push_back(SIROperand::ConstantString("__op_tobool__"));
                            if (b_.currentBlock_)
                                b_.currentBlock_->addInstruction(callInst);
                            condResult = BuildResult(boolReg, SadTypeKind::Boolean);
                        }
                    }
                }

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
