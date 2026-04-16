// ============================================================================
// sir_builder_builtins_math.cpp
// ============================================================================
// (AR) دوال الرياضيات المضمنة — مستخرج من sir_builder_builtins_core.cpp (CW-05)
//      جذر/أس/لوغاريتم/باي/بتر/سقف/أرضية/sin/cos/tan/أكبر/أصغر/جمع
// (EN) Math builtin functions — extracted from sir_builder_builtins_core.cpp
// ============================================================================

#include "sir_builder.h"
#include <stdexcept>
#include <iostream>
#include <optional>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            std::optional<BuildResult> SIRBuilder::buildBuiltinMathCall(
                const std::string &funcName,
                std::vector<BuildResult> &argResults,
                std::vector<SIROperand> &argOperands)
            {
                // ========================================================================
                // (AR) دوال رياضية - Math Functions
                // (EN) Math functions
                // ========================================================================

                // (AR) دالة جذر() - sqrt
                // (EN) sqrt() function - Square root
                // الأسماء المدعومة: جذر, sqrt, الجذر_التربيعي
                if (funcName == "جذر" || funcName == "sqrt" || funcName == "الجذر_التربيعي")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة جذر تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction sqrtInst(SIROpcode::BUILTIN_SQRT);
                    sqrtInst.result = resultOp;
                    sqrtInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(sqrtInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin جذر() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة لوغ() - natural logarithm
                // (EN) log() function - natural logarithm
                // الأسماء المدعومة: لوغ, log, ln, لوغاريتم
                if (funcName == "لوغ" || funcName == "log" || funcName == "ln" || funcName == "لوغاريتم")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة لوغ تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction logInst(SIROpcode::BUILTIN_LOG);
                    logInst.result = resultOp;
                    logInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(logInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin لوغ() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة إشارة() - signum
                // (EN) sign() function - returns -1, 0, or 1
                // الأسماء المدعومة: إشارة, اشارة, sign, signum
                if (funcName == "إشارة" || funcName == "اشارة" || funcName == "sign" || funcName == "signum")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة إشارة تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }

                    std::string resultPtrReg = newTempRegister();
                    SIRInstruction allocInst(SIROpcode::ALLOC);
                    allocInst.result = SIROperand::Register(resultPtrReg, SadTypeKind::Integer);
                    allocInst.comment = "alloca sign result";
                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(allocInst);
                    }

                    SIRInstruction initInst(SIROpcode::STORE);
                    initInst.operands.push_back(SIROperand::ConstantI64(0));
                    initInst.operands.push_back(SIROperand::Register(resultPtrReg, SadTypeKind::Integer));
                    initInst.comment = "default sign result = 0";
                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(initInst);
                    }

                    std::string isPositiveReg = newTempRegister();
                    SIRInstruction gtZeroInst(SIROpcode::GT);
                    gtZeroInst.result = SIROperand::Register(isPositiveReg, SadTypeKind::Boolean);
                    gtZeroInst.operands.push_back(argOperands[0]);
                    gtZeroInst.operands.push_back(SIROperand::ConstantI64(0));
                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(gtZeroInst);
                    }

                    std::string positiveLabel = newLabel("sign_positive");
                    std::string checkNegativeLabel = newLabel("sign_check_negative");
                    std::string negativeLabel = newLabel("sign_negative");
                    std::string mergeLabel = newLabel("sign_merge");
                    auto positiveBlock = createBasicBlock(positiveLabel);
                    auto checkNegativeBlock = createBasicBlock(checkNegativeLabel);
                    auto negativeBlock = createBasicBlock(negativeLabel);
                    auto mergeBlock = createBasicBlock(mergeLabel);
                    if (currentFunction_)
                    {
                        currentFunction_->addBasicBlock(positiveBlock);
                        currentFunction_->addBasicBlock(checkNegativeBlock);
                        currentFunction_->addBasicBlock(negativeBlock);
                        currentFunction_->addBasicBlock(mergeBlock);
                    }

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(isPositiveReg, SadTypeKind::Boolean),
                            SIROperand::Label(positiveLabel),
                            SIROperand::Label(checkNegativeLabel)));
                    }

                    currentBlock_ = positiveBlock;
                    {
                        SIRInstruction storePosInst(SIROpcode::STORE);
                        storePosInst.operands.push_back(SIROperand::ConstantI64(1));
                        storePosInst.operands.push_back(SIROperand::Register(resultPtrReg, SadTypeKind::Integer));
                        storePosInst.comment = "sign result = 1";
                        currentBlock_->instructions.push_back(storePosInst);
                        currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                    }

                    currentBlock_ = checkNegativeBlock;
                    std::string isNegativeReg = newTempRegister();
                    {
                        SIRInstruction ltZeroInst(SIROpcode::LT);
                        ltZeroInst.result = SIROperand::Register(isNegativeReg, SadTypeKind::Boolean);
                        ltZeroInst.operands.push_back(argOperands[0]);
                        ltZeroInst.operands.push_back(SIROperand::ConstantI64(0));
                        currentBlock_->instructions.push_back(ltZeroInst);
                        currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(isNegativeReg, SadTypeKind::Boolean),
                            SIROperand::Label(negativeLabel),
                            SIROperand::Label(mergeLabel)));
                    }

                    currentBlock_ = negativeBlock;
                    {
                        SIRInstruction storeNegInst(SIROpcode::STORE);
                        storeNegInst.operands.push_back(SIROperand::ConstantI64(-1));
                        storeNegInst.operands.push_back(SIROperand::Register(resultPtrReg, SadTypeKind::Integer));
                        storeNegInst.comment = "sign result = -1";
                        currentBlock_->instructions.push_back(storeNegInst);
                        currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                    }

                    currentBlock_ = mergeBlock;
                    std::string resultReg = newTempRegister();
                    SIRInstruction loadInst(SIROpcode::LOAD);
                    loadInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    loadInst.operands.push_back(SIROperand::Register(resultPtrReg, SadTypeKind::Integer));
                    loadInst.comment = "load sign result";
                    currentBlock_->instructions.push_back(loadInst);

                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) دالة أس() - power
                // (EN) power() function - Base^Exponent
                // الأسماء المدعومة: أس, power, pow
                if (funcName == "أس" || funcName == "power" || funcName == "pow")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة أس تتطلب معاملين (الأساس والأس)" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction powInst(SIROpcode::BUILTIN_POW);
                    powInst.result = resultOp;
                    powInst.operands.push_back(argOperands[0]); // base
                    powInst.operands.push_back(argOperands[1]); // exponent

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(powInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin أس() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة مطلق() - abs
                // (EN) abs() function - Absolute value
                // الأسماء المدعومة: مطلق, abs, القيمة_المطلقة, absolute
                if (funcName == "مطلق" || funcName == "abs" || funcName == "القيمة_المطلقة" || funcName == "absolute")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة مطلق تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = newTempRegister();
                    SadTypeKind resultType = argResults[0].type; // preserve type (I64 or F64)
                    SIROperand resultOp = SIROperand::Register(resultReg, resultType);

                    SIRInstruction absInst(SIROpcode::BUILTIN_ABS);
                    absInst.result = resultOp;
                    absInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(absInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin مطلق() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, resultType);
                }

                // (AR) دالة تقريب() - round
                // (EN) round() function
                // الأسماء المدعومة: تقريب, round
                if (funcName == "تقريب" || funcName == "round")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة تقريب تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);

                    SIRInstruction roundInst(SIROpcode::BUILTIN_ROUND);
                    roundInst.result = resultOp;
                    roundInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(roundInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin تقريب() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) دالة أرضية() - floor
                // (EN) floor() function
                // الأسماء المدعومة: أرضية, floor
                if (funcName == "أرضية" || funcName == "floor")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة أرضية تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);

                    SIRInstruction floorInst(SIROpcode::BUILTIN_FLOOR);
                    floorInst.result = resultOp;
                    floorInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(floorInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin أرضية() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) دالة سقف() - ceil
                // (EN) ceil() function
                // الأسماء المدعومة: سقف, ceil, ceiling
                if (funcName == "سقف" || funcName == "ceil" || funcName == "ceiling")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة سقف تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);

                    SIRInstruction ceilInst(SIROpcode::BUILTIN_CEIL);
                    ceilInst.result = resultOp;
                    ceilInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(ceilInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin سقف() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) دالة مربع() - square
                // (EN) square() function - x^2
                // الأسماء المدعومة: مربع, square
                if (funcName == "مربع" || funcName == "square")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة مربع تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction sqInst(SIROpcode::BUILTIN_POW);
                    sqInst.result = resultOp;
                    sqInst.operands.push_back(argOperands[0]);
                    sqInst.operands.push_back(SIROperand::ConstantF64(2.0));

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(sqInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin مربع() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة جيب() - sin
                // (EN) sin() function - sine
                // الأسماء المدعومة: جيب, sin, sine
                if (funcName == "جيب" || funcName == "sin" || funcName == "sine")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة جيب تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction sinInst(SIROpcode::BUILTIN_SIN);
                    sinInst.result = resultOp;
                    sinInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(sinInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin جيب() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة جيب_تمام() - cos
                // (EN) cos() function - cosine
                // الأسماء المدعومة: جيب_تمام, cos, cosine
                if (funcName == "جيب_تمام" || funcName == "cos" || funcName == "cosine")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة جيب_تمام تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction cosInst(SIROpcode::BUILTIN_COS);
                    cosInst.result = resultOp;
                    cosInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(cosInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin جيب_تمام() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة ظل() - tan
                // (EN) tan() function - tangent
                // الأسماء المدعومة: ظل, tan, tangent
                if (funcName == "ظل" || funcName == "tan" || funcName == "tangent")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة ظل تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction tanInst(SIROpcode::BUILTIN_TAN);
                    tanInst.result = resultOp;
                    tanInst.operands.push_back(argOperands[0]);

                    if (currentBlock_)
                    {
                        currentBlock_->instructions.push_back(tanInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin ظل() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }


                // ========================================================================
                // Math Functions - أكبر/أصغر/جمع (3 functions)
                // ========================================================================

                // أكبر / max - الأكبر من قيمتين
                if (funcName == "أكبر" || funcName == "max" || funcName == "maximum")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة أكبر تتطلب معاملين" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_MAX);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // أصغر / min - الأصغر من قيمتين
                if (funcName == "أصغر" || funcName == "min" || funcName == "minimum")
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة أصغر تتطلب معاملين" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_MIN);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // جمع / sum - مجموع عناصر مصفوفة
                if (funcName == "جمع" || funcName == "sum")
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة جمع تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_SUM);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (currentBlock_)
                        currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }


                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad