// ============================================================================
// sir_builder_expr_nullsafety.cpp
// ============================================================================
// Null safety, slice, and error propagation expression builders
// ============================================================================
#include "sir_builder.h"
#include "builders/expression_builder.h"

#include <iostream>
#include <limits>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // buildExprOptionalChain
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprOptionalChain(AST::OptionalChainExpr *optChainExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found OptionalChainExpr for member '"
                          << optChainExpr->member << "'" << std::endl;
#endif

                // (AR) بناء تعبير الكائن
                // (EN) Build object expression
                auto objResult = buildExpression(optChainExpr->object.get());

                // (AR) [ISSUE-064] الكائن معروف «لاشيء» وقت الترجمة (نوعه Null): قصر
                //      الدائرة مباشرةً إلى حارس العدم دون توليد فرع الوصول. فرعُ الوصول
                //      يُصدر LOAD عضوٍ يتطلّب تخطيط صنف، وهو غير موجود لقيمةٍ فارغة ⇒
                //      كان يُنتج «No class mapping» رغم صحّة المخرج وقت التشغيل.
                // (EN) [ISSUE-064] The object is statically «لاشيء» (type Null): short-circuit
                //      straight to the null sentinel without emitting the access branch. The
                //      access branch emits a member LOAD that needs a class layout, which a
                //      null value lacks ⇒ it produced «No class mapping» despite a correct
                //      runtime result.
                if (objResult.type == SadTypeKind::Null)
                {
                    std::string nullReg = b_.newTempRegister();
                    if (b_.currentBlock_)
                    {
                        SIRInstruction moveInst(SIROpcode::MOVE);
                        moveInst.result = SIROperand::Register(nullReg, SadTypeKind::Integer);
                        moveInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                        b_.currentBlock_->addInstruction(moveInst);
                    }
                    return BuildResult(nullReg, SadTypeKind::Integer);
                }

                // (AR) إنشاء الكتل: فحص null → وصول العضو / null
                // (EN) Create blocks: null check → member access / null
                std::string accessLabel = b_.newLabel("optchain_access");
                std::string nullLabel = b_.newLabel("optchain_null");
                std::string mergeLabel = b_.newLabel("optchain_merge");

                auto accessBlock = b_.createBasicBlock(accessLabel);
                auto nullBlock = b_.createBasicBlock(nullLabel);
                auto mergeBlock = b_.createBasicBlock(mergeLabel);

                // (AR) [ISSUE-064] فحص: هل الكائن ليس «لاشيء»؟ «لاشيء» يُمثَّل بـ
                //      kSadNullSentinel (≠ 0) لا بصفر، فمقارنته بـ0 كانت تجعل
                //      الكائن الفارغ «موجودًا» خطأً ⇒ يُحمَّل العضو من الحارس كمؤشّر.
                //      الصحيح: قصر الدائرة حين يساوي الكائن الحارس.
                // (EN) [ISSUE-064] Check: is object non-null? «لاشيء» is the sentinel
                //      kSadNullSentinel (≠ 0), not zero — comparing to 0 made a null
                //      object wrongly «present» ⇒ the member was loaded from the
                //      sentinel as a pointer. Correct: short-circuit when object == sentinel.
                std::string cmpReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction cmpInst(SIROpcode::NE);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                    b_.currentBlock_->addInstruction(cmpInst);

                    b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(accessLabel),
                        SIROperand::Label(nullLabel)));
                }

                // (AR) فرع الوصول: الكائن موجود → LOAD العضو
                // (EN) Access branch: object exists → LOAD member
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(accessBlock);
                b_.currentBlock_ = accessBlock;
                std::string memberReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction loadInst;
                    loadInst.opcode = SIROpcode::LOAD;
                    loadInst.result = SIROperand::Register(memberReg, SadTypeKind::Integer);
                    loadInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    loadInst.operands.push_back(SIROperand::ConstantString(optChainExpr->member));
                    loadInst.comment = "optional chain member: " + optChainExpr->member;
                    b_.currentBlock_->addInstruction(loadInst);
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                // (AR) [ISSUE-064] فرع null: إرجاع حارس العدم (kSadNullSentinel)
                //      لا الصفر، كي يطبعه مسار الطباعة «لاشيء» موافقًا للمفسّر.
                // (EN) [ISSUE-064] Null branch: return the null sentinel
                //      (kSadNullSentinel), not 0, so the print path renders «لاشيء»
                //      matching the interpreter.
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(nullBlock);
                b_.currentBlock_ = nullBlock;
                std::string nullReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(nullReg, SadTypeKind::Integer);
                    moveInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                    b_.currentBlock_->addInstruction(moveInst);
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                // (AR) كتلة الدمج مع PHI
                // (EN) Merge block with PHI
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(mergeBlock);
                b_.currentBlock_ = mergeBlock;
                std::string phiReg = b_.newTempRegister();
                SIRInstruction phiInst = SIRInstruction::Phi(
                    SIROperand::Register(phiReg, SadTypeKind::Integer),
                    {{SIROperand::Register(memberReg, SadTypeKind::Integer), SIROperand::Label(accessLabel)},
                     {SIROperand::Register(nullReg, SadTypeKind::Integer), SIROperand::Label(nullLabel)}});
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(phiInst);

                return BuildResult(phiReg, SadTypeKind::Integer);
            }

            // ============================================================================
            // buildExprNullCoalesce
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprNullCoalesce(AST::NullCoalesceExpr *nullCoalExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found NullCoalesceExpr" << std::endl;
#endif

                // (AR) بناء التعبير الأيسر
                // (EN) Build left expression
                auto leftResult = buildExpression(nullCoalExpr->left.get());

                // (AR) [ISSUE-046] القيمة المنطقيّة لا تكون «لاشيء» أبدًا. ومقارنة سجلّ
                //      منطقيّ (i1) بحارس العدم (i64 = 0x8000…0001) تبتر الحارس إلى بِتّه
                //      الأدنى (=1) فيُحسَب «صحيح» مساويًا للحارس خطأً ⇒ يُعاد الأيمن.
                //      الحلّ: نُرجِع الأيسر مباشرةً لأيّ معامل منطقيّ (غير قابل للعدم).
                // (EN) A boolean is never null. Comparing an i1 register to the i64 null
                //      sentinel (0x8000…0001) truncates the sentinel to its low bit (=1),
                //      so `true` is wrongly seen as the sentinel and the right operand is
                //      returned. Short-circuit to the left value for any boolean operand.
                if (leftResult.type == SadTypeKind::Boolean)
                {
                    return leftResult;
                }

                // (AR) إنشاء الكتل: فحص null → يسار / يمين
                // (EN) Create blocks: null check → left / right
                std::string leftLabel = b_.newLabel("nc_left");
                std::string rightLabel = b_.newLabel("nc_right");
                std::string mergeLabel = b_.newLabel("nc_merge");

                auto leftBlock = b_.createBasicBlock(leftLabel);
                auto rightBlock = b_.createBasicBlock(rightLabel);
                auto mergeBlock = b_.createBasicBlock(mergeLabel);

                // (AR) فحص: هل اليسار != لاشيء (null-sentinel)
                //      اللغة تمثل لاشيء بقيمة sentinel خاصة وليست 0، لذلك
                //      المقارنة مع 0 تكسر ?? عندما يكون اليسار هو لاشيء فعلاً.
                // (EN) Check: is left != null sentinel
                //      The language represents null with a dedicated sentinel, not 0,
                //      so comparing against 0 breaks ?? for real null values.
                std::string cmpReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction cmpInst(SIROpcode::NE);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(leftResult.registerName, leftResult.type));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                    b_.currentBlock_->addInstruction(cmpInst);

                    b_.currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(leftLabel),
                        SIROperand::Label(rightLabel)));
                }

                // (AR) فرع اليسار: القيمة موجودة
                // (EN) Left branch: value exists
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(leftBlock);
                b_.currentBlock_ = leftBlock;
                // (AR) دائماً MOVE محلي في nc_left لضمان صلاحية leftReg في PHI
                // (EN) Always emit local MOVE in nc_left so PHI has a locally-defined value
                std::string leftReg = b_.newTempRegister();
                if (b_.currentBlock_)
                {
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(leftReg, leftResult.type);
                    if (leftResult.isConstant)
                    {
                        switch (leftResult.type)
                        {
                        case SadTypeKind::String:
                            moveInst.operands.push_back(SIROperand::ConstantString(leftResult.constantValue));
                            break;
                        case SadTypeKind::Float:
                            moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(leftResult.constantValue)));
                            break;
                        default:
                            try
                            {
                                moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(leftResult.constantValue)));
                            }
                            catch (const std::exception &)
                            {
#ifndef NDEBUG
                                std::cerr << "[SIR] تحذير: فشل تحويل قيمة إلى I64، القيمة الافتراضية 0\n";
#endif
                                moveInst.operands.push_back(SIROperand::ConstantI64(0));
                            }
                            break;
                        }
                    }
                    else
                    {
                        // (AR) سجل: أُضيف كمعامل مصدر
                        // (EN) Register: add as source operand
                        moveInst.operands.push_back(SIROperand::Register(leftResult.registerName, leftResult.type));
                    }
                    b_.currentBlock_->addInstruction(moveInst);
                }

                // (AR) فرع اليمين: القيمة البديلة
                // (EN) Right branch: fallback value
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(rightBlock);
                b_.currentBlock_ = rightBlock;
                auto rightResult = buildExpression(nullCoalExpr->right.get());
                SadTypeKind resultType = leftResult.type;
                // (AR) [ISSUE-046] حين يكون الأيسر «لاشيء» حرفيًّا (Null) فالنتيجة تأتي
                //      من الأيمن دائمًا؛ نعتمد نوع الأيمن كي لا يُعامَل بديلٌ منطقيّ/عشريّ
                //      كـi64 فيُفسَد عند MOVE (مثلاً «true» تفشل stoll ⇒ 0).
                // (EN) When the left is the literal null, the result always comes from the
                //      right, so adopt the right's type — otherwise a boolean/float fallback
                //      is mis-typed as i64 and corrupted at MOVE (e.g. "true" fails stoll→0).
                if (resultType == SadTypeKind::Void || resultType == SadTypeKind::Unknown ||
                    resultType == SadTypeKind::Null)
                {
                    resultType = rightResult.type;
                }
                // (AR) عند مزج nullable عددي مع بديل نصي، نعتمد النص كنوع موحَّد
                //      للحفاظ على سلوك التنفيذ الديناميكي المتوقع في اللغة.
                // (EN) For numeric-nullable with string fallback, prefer string as unified type
                //      to preserve expected dynamic-language behavior.
                if (rightResult.type == SadTypeKind::String && resultType != SadTypeKind::String)
                {
                    resultType = SadTypeKind::String;
                }

                // (AR) توحيد فرع اليسار إلى resultType عند الحاجة قبل القفز إلى الدمج.
                // (EN) Normalize left branch to resultType when needed before jumping to merge.
                if (leftResult.type != resultType && b_.currentBlock_)
                {
                    b_.currentBlock_ = leftBlock;

                    if (resultType == SadTypeKind::String)
                    {
                        std::string leftCastReg = b_.newTempRegister();
                        SIROpcode castOpcode = (leftResult.type == SadTypeKind::Float)
                                                   ? SIROpcode::F64_TO_STRING
                                                   : SIROpcode::I64_TO_STRING;
                        SIRInstruction castInst(castOpcode);
                        castInst.result = SIROperand::Register(leftCastReg, SadTypeKind::String);
                        castInst.operands.push_back(SIROperand::Register(leftReg, leftResult.type));
                        b_.currentBlock_->addInstruction(castInst);
                        leftReg = leftCastReg;
                    }
                }
                if (leftBlock)
                {
                    b_.currentBlock_ = leftBlock;
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                b_.currentBlock_ = rightBlock;
                std::string rightReg = rightResult.registerName;
                if (rightResult.isConstant && b_.currentBlock_)
                {
                    rightReg = b_.newTempRegister();
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(rightReg, resultType);
                    switch (resultType)
                    {
                    case SadTypeKind::String:
                        moveInst.operands.push_back(SIROperand::ConstantString(rightResult.constantValue));
                        break;
                    case SadTypeKind::Float:
                        moveInst.operands.push_back(SIROperand::ConstantF64(std::stod(rightResult.constantValue)));
                        break;
                    case SadTypeKind::Boolean:
                        // (AR) [ISSUE-046] بديل منطقيّ: نُصدر ConstantBool لا I64 (stoll تفشل على «true»)
                        // (EN) [ISSUE-046] boolean fallback: emit ConstantBool, not I64 (stoll fails on "true")
                        moveInst.operands.push_back(SIROperand::ConstantBool(
                            rightResult.constantValue == "true" || rightResult.constantValue == "1"));
                        break;
                    default:
                        try
                        {
                            moveInst.operands.push_back(SIROperand::ConstantI64(std::stoll(rightResult.constantValue)));
                        }
                        catch (const std::exception &)
                        {
#ifndef NDEBUG
                            std::cerr << "[SIR] تحذير: فشل تحويل قيمة إلى I64، القيمة الافتراضية 0\n";
#endif
                            moveInst.operands.push_back(SIROperand::ConstantI64(0));
                        }
                        break;
                    }
                    b_.currentBlock_->addInstruction(moveInst);
                }
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));

                // (AR) كتلة الدمج مع PHI
                // (EN) Merge block with PHI
                if (b_.currentFunction_)
                    b_.currentFunction_->addBasicBlock(mergeBlock);
                b_.currentBlock_ = mergeBlock;
                std::string phiReg = b_.newTempRegister();
                SIRInstruction phiInst = SIRInstruction::Phi(
                    SIROperand::Register(phiReg, resultType),
                    {{SIROperand::Register(leftReg, resultType), SIROperand::Label(leftLabel)},
                     {SIROperand::Register(rightReg, resultType), SIROperand::Label(rightLabel)}});
                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(phiInst);

                return BuildResult(phiReg, resultType);
            }

            // ============================================================================
            // buildExprSlice — بناء تعبير الشريحة [:] على المصفوفات
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprSlice(AST::SliceExpr *sliceExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found SliceExpr" << std::endl;
#endif

                // (AR) بناء تعبير المصفوفة الأصلية
                // (EN) Build source array expression
                auto objResult = buildExpression(sliceExpr->object.get());

                // (AR) بناء معامل البداية (أو 0 افتراضياً)
                // (EN) Build start operand (or 0 as default)
                SIROperand startOp = SIROperand::ConstantI64(0);
                if (sliceExpr->start)
                {
                    auto startResult = buildExpression(sliceExpr->start.get());
                    if (startResult.isConstant)
                    {
                        try
                        {
                            startOp = SIROperand::ConstantI64(std::stoll(startResult.constantValue));
                        }
                        catch (...)
                        {
                            startOp = SIROperand::ConstantI64(0);
                        }
                    }
                    else if (!startResult.registerName.empty())
                    {
                        startOp = SIROperand::Register(startResult.registerName, startResult.type);
                    }
                }

                // (AR) [ISSUE-063] بناء معامل النهاية. الافتراض حين لا نهاية = حارس
                //      INT64_MIN لا -1: الخلفية (emitBuiltinArraySlice) تفسّر INT64_MIN
                //      «حتى آخر المصفوفة» بينما -1 فهرسٌ سالب صريح ⇒ srcLen-1 فيقتطع
                //      الذيل لعنصرٍ واحد (`[1..]` كان يُرجع [20] بدل [20, 30]).
                // (EN) [ISSUE-063] Build end operand. Default when no end = the sentinel
                //      INT64_MIN, not -1: the backend (emitBuiltinArraySlice) reads
                //      INT64_MIN as «to end of array» whereas -1 is an explicit negative
                //      index ⇒ srcLen-1, truncating the tail to one element (`[1..]`
                //      wrongly returned [20] instead of [20, 30]).
                SIROperand endOp = SIROperand::ConstantI64(std::numeric_limits<int64_t>::min());
                if (sliceExpr->end)
                {
                    auto endResult = buildExpression(sliceExpr->end.get());
                    if (endResult.isConstant)
                    {
                        try
                        {
                            endOp = SIROperand::ConstantI64(std::stoll(endResult.constantValue));
                        }
                        catch (...)
                        {
                            endOp = SIROperand::ConstantI64(std::numeric_limits<int64_t>::min());
                        }
                    }
                    else if (!endResult.registerName.empty())
                    {
                        endOp = SIROperand::Register(endResult.registerName, endResult.type);
                    }
                }

                // (AR) إنشاء تعليمة BUILTIN_ARRAY_SLICE: (array, start, end)
                // (EN) Create BUILTIN_ARRAY_SLICE instruction: (array, start, end)
                std::string resultReg = b_.newTempRegister();
                SIRInstruction sliceInst(SIROpcode::BUILTIN_ARRAY_SLICE);
                sliceInst.result = SIROperand::Register(resultReg, SadTypeKind::Array);
                // (AR) المعامل الأول: المصفوفة المصدر
                if (objResult.isConstant)
                    sliceInst.operands.push_back(SIROperand::ConstantString(objResult.constantValue));
                else
                    sliceInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                // (AR) المعامل الثاني: بداية الشريحة
                sliceInst.operands.push_back(startOp);
                // (AR) المعامل الثالث: نهاية الشريحة
                sliceInst.operands.push_back(endOp);

                // (AR) المعامل الرابع: الخطوة (اختياري)
                if (sliceExpr->step)
                {
                    auto stepResult = buildExpression(sliceExpr->step.get());
                    if (stepResult.isConstant)
                    {
                        try
                        {
                            sliceInst.operands.push_back(SIROperand::ConstantI64(std::stoll(stepResult.constantValue)));
                        }
                        catch (...)
                        {
                            sliceInst.operands.push_back(SIROperand::ConstantI64(1));
                        }
                    }
                    else if (!stepResult.registerName.empty())
                    {
                        sliceInst.operands.push_back(SIROperand::Register(stepResult.registerName, stepResult.type));
                    }
                }

                if (b_.currentBlock_)
                    b_.currentBlock_->addInstruction(sliceInst);

#ifndef NDEBUG
                std::cout << "[DEBUG] buildExprSlice -> " << resultReg << std::endl;
#endif
                return BuildResult(resultReg, SadTypeKind::Array);
            }

            // ============================================================================
            // buildExprErrorPropagate
            // ============================================================================
            BuildResult ExpressionBuilder::buildExprErrorPropagate(AST::ErrorPropagateExpr *errorPropExpr)
            {
                // (AR) تنفيذ أولي آمن: نبني التعبير الداخلي كما هو ونُرجع نتيجته.
                //      هذا يحافظ على تماسك الـ SIR ويمنع فشل الربط عند وجود "انشر".
                // (EN) Safe baseline implementation: build and return the inner expression.
                //      This keeps SIR generation consistent and fixes linker failures.
                if (!errorPropExpr || !errorPropExpr->inner)
                {
                    return BuildResult("", SadTypeKind::Unknown);
                }

                return buildExpression(errorPropExpr->inner.get());
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
