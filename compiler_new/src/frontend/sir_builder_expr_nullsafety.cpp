// ============================================================================
// sir_builder_expr_nullsafety.cpp
// ============================================================================
// Null safety, slice, and error propagation expression builders
// ============================================================================
#include "sir_builder.h"

#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

            // ============================================================================
            // buildExprOptionalChain
            // ============================================================================
            BuildResult SIRBuilder::buildExprOptionalChain(AST::OptionalChainExpr *optChainExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found OptionalChainExpr for member '"
                          << optChainExpr->member << "'" << std::endl;
#endif

                // (AR) بناء تعبير الكائن
                // (EN) Build object expression
                auto objResult = buildExpression(optChainExpr->object.get());

                // (AR) إنشاء الكتل: فحص null → وصول العضو / null
                // (EN) Create blocks: null check → member access / null
                std::string accessLabel = newLabel("optchain_access");
                std::string nullLabel = newLabel("optchain_null");
                std::string mergeLabel = newLabel("optchain_merge");

                auto accessBlock = createBasicBlock(accessLabel);
                auto nullBlock = createBasicBlock(nullLabel);
                auto mergeBlock = createBasicBlock(mergeLabel);

                // (AR) فحص: هل الكائن != null (!=0)
                // (EN) Check: is object != null (!=0)
                std::string cmpReg = newTempRegister();
                if (currentBlock_)
                {
                    SIRInstruction cmpInst(SIROpcode::NE);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(0));
                    currentBlock_->addInstruction(cmpInst);

                    currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(accessLabel),
                        SIROperand::Label(nullLabel)));
                }

                // (AR) فرع الوصول: الكائن موجود → LOAD العضو
                // (EN) Access branch: object exists → LOAD member
                if (currentFunction_)
                    currentFunction_->addBasicBlock(accessBlock);
                currentBlock_ = accessBlock;
                std::string memberReg = newTempRegister();
                if (currentBlock_)
                {
                    SIRInstruction loadInst;
                    loadInst.opcode = SIROpcode::LOAD;
                    loadInst.result = SIROperand::Register(memberReg, SadTypeKind::Integer);
                    loadInst.operands.push_back(SIROperand::Register(objResult.registerName, objResult.type));
                    loadInst.operands.push_back(SIROperand::ConstantString(optChainExpr->member));
                    loadInst.comment = "optional chain member: " + optChainExpr->member;
                    currentBlock_->addInstruction(loadInst);
                    currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                // (AR) فرع null: إرجاع 0 (null)
                // (EN) Null branch: return 0 (null)
                if (currentFunction_)
                    currentFunction_->addBasicBlock(nullBlock);
                currentBlock_ = nullBlock;
                std::string nullReg = newTempRegister();
                if (currentBlock_)
                {
                    SIRInstruction moveInst(SIROpcode::MOVE);
                    moveInst.result = SIROperand::Register(nullReg, SadTypeKind::Integer);
                    moveInst.operands.push_back(SIROperand::ConstantI64(0));
                    currentBlock_->addInstruction(moveInst);
                    currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                // (AR) كتلة الدمج مع PHI
                // (EN) Merge block with PHI
                if (currentFunction_)
                    currentFunction_->addBasicBlock(mergeBlock);
                currentBlock_ = mergeBlock;
                std::string phiReg = newTempRegister();
                SIRInstruction phiInst = SIRInstruction::Phi(
                    SIROperand::Register(phiReg, SadTypeKind::Integer),
                    {{SIROperand::Register(memberReg, SadTypeKind::Integer), SIROperand::Label(accessLabel)},
                     {SIROperand::Register(nullReg, SadTypeKind::Integer), SIROperand::Label(nullLabel)}});
                if (currentBlock_)
                    currentBlock_->addInstruction(phiInst);

                return BuildResult(phiReg, SadTypeKind::Integer);
            }

            // ============================================================================
            // buildExprNullCoalesce
            // ============================================================================
            BuildResult SIRBuilder::buildExprNullCoalesce(AST::NullCoalesceExpr *nullCoalExpr)
            {
#ifndef NDEBUG
                std::cout << "[DEBUG] buildExpression: found NullCoalesceExpr" << std::endl;
#endif

                // (AR) بناء التعبير الأيسر
                // (EN) Build left expression
                auto leftResult = buildExpression(nullCoalExpr->left.get());

                // (AR) إنشاء الكتل: فحص null → يسار / يمين
                // (EN) Create blocks: null check → left / right
                std::string leftLabel = newLabel("nc_left");
                std::string rightLabel = newLabel("nc_right");
                std::string mergeLabel = newLabel("nc_merge");

                auto leftBlock = createBasicBlock(leftLabel);
                auto rightBlock = createBasicBlock(rightLabel);
                auto mergeBlock = createBasicBlock(mergeLabel);

                // (AR) فحص: هل اليسار != لاشيء (null-sentinel)
                //      اللغة تمثل لاشيء بقيمة sentinel خاصة وليست 0، لذلك
                //      المقارنة مع 0 تكسر ?? عندما يكون اليسار هو لاشيء فعلاً.
                // (EN) Check: is left != null sentinel
                //      The language represents null with a dedicated sentinel, not 0,
                //      so comparing against 0 breaks ?? for real null values.
                std::string cmpReg = newTempRegister();
                if (currentBlock_)
                {
                    SIRInstruction cmpInst(SIROpcode::NE);
                    cmpInst.result = SIROperand::Register(cmpReg, SadTypeKind::Boolean);
                    cmpInst.operands.push_back(SIROperand::Register(leftResult.registerName, leftResult.type));
                    cmpInst.operands.push_back(SIROperand::ConstantI64(Sad::Compiler::kSadNullSentinel));
                    currentBlock_->addInstruction(cmpInst);

                    currentBlock_->addInstruction(SIRInstruction::BranchCond(
                        SIROperand::Register(cmpReg, SadTypeKind::Boolean),
                        SIROperand::Label(leftLabel),
                        SIROperand::Label(rightLabel)));
                }

                // (AR) فرع اليسار: القيمة موجودة

                // (AR) فرع اليسار: القيمة موجودة
                // (EN) Left branch: value exists
                if (currentFunction_)
                    currentFunction_->addBasicBlock(leftBlock);
                currentBlock_ = leftBlock;
                // (AR) دائماً MOVE محلي في nc_left لضمان صلاحية leftReg في PHI
                // (EN) Always emit local MOVE in nc_left so PHI has a locally-defined value
                std::string leftReg = newTempRegister();
                if (currentBlock_)
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
                    currentBlock_->addInstruction(moveInst);
                }

                // (AR) فرع اليمين: القيمة البديلة
                // (EN) Right branch: fallback value
                if (currentFunction_)
                    currentFunction_->addBasicBlock(rightBlock);
                currentBlock_ = rightBlock;
                auto rightResult = buildExpression(nullCoalExpr->right.get());
                SadTypeKind resultType = leftResult.type;
                if (resultType == SadTypeKind::Void || resultType == SadTypeKind::Unknown)
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
                if (leftResult.type != resultType && currentBlock_)
                {
                    currentBlock_ = leftBlock;

                    if (resultType == SadTypeKind::String)
                    {
                        std::string leftCastReg = newTempRegister();
                        SIROpcode castOpcode = (leftResult.type == SadTypeKind::Float)
                                                   ? SIROpcode::F64_TO_STRING
                                                   : SIROpcode::I64_TO_STRING;
                        SIRInstruction castInst(castOpcode);
                        castInst.result = SIROperand::Register(leftCastReg, SadTypeKind::String);
                        castInst.operands.push_back(SIROperand::Register(leftReg, leftResult.type));
                        currentBlock_->addInstruction(castInst);
                        leftReg = leftCastReg;
                    }
                }
                if (leftBlock)
                {
                    currentBlock_ = leftBlock;
                    currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                }

                currentBlock_ = rightBlock;
                std::string rightReg = rightResult.registerName;
                if (rightResult.isConstant && currentBlock_)
                {
                    rightReg = newTempRegister();
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
                    currentBlock_->addInstruction(moveInst);
                }
                if (currentBlock_)
                    currentBlock_->addInstruction(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));

                // (AR) كتلة الدمج مع PHI
                // (EN) Merge block with PHI
                if (currentFunction_)
                    currentFunction_->addBasicBlock(mergeBlock);
                currentBlock_ = mergeBlock;
                std::string phiReg = newTempRegister();
                SIRInstruction phiInst = SIRInstruction::Phi(
                    SIROperand::Register(phiReg, resultType),
                    {{SIROperand::Register(leftReg, resultType), SIROperand::Label(leftLabel)},
                     {SIROperand::Register(rightReg, resultType), SIROperand::Label(rightLabel)}});
                if (currentBlock_)
                    currentBlock_->addInstruction(phiInst);

                return BuildResult(phiReg, resultType);
            }

            // ============================================================================
            // buildExprSlice — بناء تعبير الشريحة [:] على المصفوفات
            // ============================================================================
            BuildResult SIRBuilder::buildExprSlice(AST::SliceExpr *sliceExpr)
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

                // (AR) بناء معامل النهاية (أو -1 = حتى النهاية افتراضياً)
                // (EN) Build end operand (or -1 = until end as default)
                SIROperand endOp = SIROperand::ConstantI64(-1);
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
                            endOp = SIROperand::ConstantI64(-1);
                        }
                    }
                    else if (!endResult.registerName.empty())
                    {
                        endOp = SIROperand::Register(endResult.registerName, endResult.type);
                    }
                }

                // (AR) إنشاء تعليمة BUILTIN_ARRAY_SLICE: (array, start, end)
                // (EN) Create BUILTIN_ARRAY_SLICE instruction: (array, start, end)
                std::string resultReg = newTempRegister();
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

                if (currentBlock_)
                    currentBlock_->addInstruction(sliceInst);

#ifndef NDEBUG
                std::cout << "[DEBUG] buildExprSlice -> " << resultReg << std::endl;
#endif
                return BuildResult(resultReg, SadTypeKind::Array);
            }

            // ============================================================================
            // buildExprErrorPropagate
            // ============================================================================
            BuildResult SIRBuilder::buildExprErrorPropagate(AST::ErrorPropagateExpr *errorPropExpr)
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
