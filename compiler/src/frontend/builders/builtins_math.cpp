// ============================================================================
// sir_builder_builtins_math.cpp
// ============================================================================
// (AR) دوال الرياضيات المضمنة — مستخرج من sir_builder_builtins_core.cpp (CW-05)
//      جذر/أس/لوغاريتم/باي/بتر/سقف/أرضية/sin/cos/tan/أكبر/أصغر/جمع
// (EN) Math builtin functions — extracted from sir_builder_builtins_core.cpp
// ============================================================================

#include "sir_builder.h"
#include "builders/builtin_builder.h"
#include "sir_builder.h"
#include "builtin_registry.h"
#include <stdexcept>
#include <iostream>
#include <optional>

// (AR) اختصار لأسماء الدوال المركزية — مصدر حقيقة واحد
namespace Bm = Sad::Builtins::Names::Math;

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
            std::optional<BuildResult> BuiltinBuilder::buildBuiltinMathCall(
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
                if (funcName == Bm::SQRT)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة جذر تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction sqrtInst(SIROpcode::BUILTIN_SQRT);
                    sqrtInst.result = resultOp;
                    sqrtInst.operands.push_back(argOperands[0]);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(sqrtInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin جذر() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة لوغ() - natural logarithm
                // (EN) log() function - natural logarithm
                // الأسماء المدعومة: لوغ, log, ln, لوغاريتم
                if (funcName == Bm::LOG)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة لوغ تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction logInst(SIROpcode::BUILTIN_LOG);
                    logInst.result = resultOp;
                    logInst.operands.push_back(argOperands[0]);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(logInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin لوغ() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة إشارة() - signum
                // (EN) sign() function - returns -1, 0, or 1
                // الأسماء المدعومة: إشارة, اشارة, sign, signum
                if (funcName == Bm::SIGN)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة إشارة تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }

                    std::string resultPtrReg = b_.newTempRegister();
                    SIRInstruction allocInst(SIROpcode::ALLOC);
                    allocInst.result = SIROperand::Register(resultPtrReg, SadTypeKind::Integer);
                    allocInst.comment = "alloca sign result";
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(allocInst);
                    }

                    SIRInstruction initInst(SIROpcode::STORE);
                    initInst.operands.push_back(SIROperand::ConstantI64(0));
                    initInst.operands.push_back(SIROperand::Register(resultPtrReg, SadTypeKind::Integer));
                    initInst.comment = "default sign result = 0";
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(initInst);
                    }

                    std::string isPositiveReg = b_.newTempRegister();
                    SIRInstruction gtZeroInst(SIROpcode::GT);
                    gtZeroInst.result = SIROperand::Register(isPositiveReg, SadTypeKind::Boolean);
                    gtZeroInst.operands.push_back(argOperands[0]);
                    gtZeroInst.operands.push_back(SIROperand::ConstantI64(0));
                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(gtZeroInst);
                    }

                    std::string positiveLabel = b_.newLabel("sign_positive");
                    std::string checkNegativeLabel = b_.newLabel("sign_check_negative");
                    std::string negativeLabel = b_.newLabel("sign_negative");
                    std::string mergeLabel = b_.newLabel("sign_merge");
                    auto positiveBlock = b_.createBasicBlock(positiveLabel);
                    auto checkNegativeBlock = b_.createBasicBlock(checkNegativeLabel);
                    auto negativeBlock = b_.createBasicBlock(negativeLabel);
                    auto mergeBlock = b_.createBasicBlock(mergeLabel);
                    if (b_.currentFunction_)
                    {
                        b_.currentFunction_->addBasicBlock(positiveBlock);
                        b_.currentFunction_->addBasicBlock(checkNegativeBlock);
                        b_.currentFunction_->addBasicBlock(negativeBlock);
                        b_.currentFunction_->addBasicBlock(mergeBlock);
                    }

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(isPositiveReg, SadTypeKind::Boolean),
                            SIROperand::Label(positiveLabel),
                            SIROperand::Label(checkNegativeLabel)));
                    }

                    b_.currentBlock_ = positiveBlock;
                    {
                        SIRInstruction storePosInst(SIROpcode::STORE);
                        storePosInst.operands.push_back(SIROperand::ConstantI64(1));
                        storePosInst.operands.push_back(SIROperand::Register(resultPtrReg, SadTypeKind::Integer));
                        storePosInst.comment = "sign result = 1";
                        b_.currentBlock_->instructions.push_back(storePosInst);
                        b_.currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                    }

                    b_.currentBlock_ = checkNegativeBlock;
                    std::string isNegativeReg = b_.newTempRegister();
                    {
                        SIRInstruction ltZeroInst(SIROpcode::LT);
                        ltZeroInst.result = SIROperand::Register(isNegativeReg, SadTypeKind::Boolean);
                        ltZeroInst.operands.push_back(argOperands[0]);
                        ltZeroInst.operands.push_back(SIROperand::ConstantI64(0));
                        b_.currentBlock_->instructions.push_back(ltZeroInst);
                        b_.currentBlock_->instructions.push_back(SIRInstruction::BranchCond(
                            SIROperand::Register(isNegativeReg, SadTypeKind::Boolean),
                            SIROperand::Label(negativeLabel),
                            SIROperand::Label(mergeLabel)));
                    }

                    b_.currentBlock_ = negativeBlock;
                    {
                        SIRInstruction storeNegInst(SIROpcode::STORE);
                        storeNegInst.operands.push_back(SIROperand::ConstantI64(-1));
                        storeNegInst.operands.push_back(SIROperand::Register(resultPtrReg, SadTypeKind::Integer));
                        storeNegInst.comment = "sign result = -1";
                        b_.currentBlock_->instructions.push_back(storeNegInst);
                        b_.currentBlock_->instructions.push_back(SIRInstruction::Branch(SIROperand::Label(mergeLabel)));
                    }

                    b_.currentBlock_ = mergeBlock;
                    std::string resultReg = b_.newTempRegister();
                    SIRInstruction loadInst(SIROpcode::LOAD);
                    loadInst.result = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    loadInst.operands.push_back(SIROperand::Register(resultPtrReg, SadTypeKind::Integer));
                    loadInst.comment = "load sign result";
                    b_.currentBlock_->instructions.push_back(loadInst);

                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) دالة أس() - power
                // (EN) power() function - Base^Exponent
                // الأسماء المدعومة: أس, power, pow
                if (funcName == Bm::POWER)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة أس تتطلب معاملين (الأساس والأس)" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction powInst(SIROpcode::BUILTIN_POW);
                    powInst.result = resultOp;
                    powInst.operands.push_back(argOperands[0]); // base
                    powInst.operands.push_back(argOperands[1]); // exponent

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(powInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin أس() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة مطلق() - abs
                // (EN) abs() function - Absolute value
                // الأسماء المدعومة: مطلق, abs, القيمة_المطلقة, absolute
                if (funcName == Bm::ABS)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة مطلق تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SadTypeKind resultType = argResults[0].type; // preserve type (I64 or F64)
                    SIROperand resultOp = SIROperand::Register(resultReg, resultType);

                    SIRInstruction absInst(SIROpcode::BUILTIN_ABS);
                    absInst.result = resultOp;
                    absInst.operands.push_back(argOperands[0]);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(absInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin مطلق() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, resultType);
                }

                // (AR) دالة تقريب() - round
                // (EN) round() function
                // الأسماء المدعومة: تقريب, round
                if (funcName == Bm::ROUND)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة تقريب تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);

                    SIRInstruction roundInst(SIROpcode::BUILTIN_ROUND);
                    roundInst.result = resultOp;
                    roundInst.operands.push_back(argOperands[0]);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(roundInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin تقريب() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) دالة أرضية() - floor
                // (EN) floor() function
                // الأسماء المدعومة: أرضية, floor
                if (funcName == Bm::FLOOR)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة أرضية تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);

                    SIRInstruction floorInst(SIROpcode::BUILTIN_FLOOR);
                    floorInst.result = resultOp;
                    floorInst.operands.push_back(argOperands[0]);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(floorInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin أرضية() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) دالة سقف() - ceil
                // (EN) ceil() function
                // الأسماء المدعومة: سقف, ceil, ceiling
                if (funcName == Bm::CEIL)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة سقف تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);

                    SIRInstruction ceilInst(SIROpcode::BUILTIN_CEIL);
                    ceilInst.result = resultOp;
                    ceilInst.operands.push_back(argOperands[0]);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(ceilInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin سقف() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) دالة مربع() - square
                // (EN) square() function - x^2
                // الأسماء المدعومة: مربع, square
                if (funcName == Bm::SQUARE)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة مربع تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = b_.newTempRegister();

                    // (AR) مطابقة المفسّر (MathFunctions::square): مدخلٌ صحيح ⇒ مربّعٌ صحيح عبر
                    //      الضرب الصحيح (x*x)؛ مدخلٌ عشري ⇒ مربّعٌ عشري عبر BUILTIN_POW.
                    //      السلوك السابق كان يُصلّب Float دائمًا فيُخرج «25.0» بدل «25» للمدخل
                    //      الصحيح — تباعُدٌ عن المفسّر (الذي يُرجع صحيحًا للمدخل الصحيح).
                    //      تباعُد متبقٍّ موثَّق: المفسّر يرتدّ إلى عشريّ حين يفيض الناتج عن int32
                    //      (مربع(50000) ⇒ «2500000000.0»)، بينما هنا يبقى i64 (⇒ «2500000000») —
                    //      لا يُحسم سكونيًّا لمدخل متغيّر.
                    // (EN) Match interpreter (MathFunctions::square): integer input ⇒ integer
                    //      square via integer multiply (x*x); float input ⇒ float square via
                    //      BUILTIN_POW. The previous code always pinned Float, emitting "25.0"
                    //      instead of "25" for integer input — a divergence from the interpreter.
                    //      Documented residual divergence: the interpreter falls back to double
                    //      when the result overflows int32 (مربع(50000) ⇒ "2500000000.0") while
                    //      this stays i64 (⇒ "2500000000") — not statically decidable for a
                    //      variable input.
                    if (argResults[0].type == SadTypeKind::Integer)
                    {
                        SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                        SIRInstruction mulInst(SIROpcode::MUL_I64);
                        mulInst.result = resultOp;
                        mulInst.operands.push_back(argOperands[0]);
                        mulInst.operands.push_back(argOperands[0]);
                        if (b_.currentBlock_)
                        {
                            b_.currentBlock_->instructions.push_back(mulInst);
                        }
#ifndef NDEBUG
                        std::cout << "[DEBUG] buildFunctionCall: builtin مربع() [i64] -> " << resultReg << std::endl;
#endif
                        return BuildResult(resultReg, SadTypeKind::Integer);
                    }

                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction sqInst(SIROpcode::BUILTIN_POW);
                    sqInst.result = resultOp;
                    sqInst.operands.push_back(argOperands[0]);
                    sqInst.operands.push_back(SIROperand::ConstantF64(2.0));

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(sqInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin مربع() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة جيب() - sin
                // (EN) sin() function - sine
                // الأسماء المدعومة: جيب, sin, sine
                if (funcName == Bm::SIN)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة جيب تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction sinInst(SIROpcode::BUILTIN_SIN);
                    sinInst.result = resultOp;
                    sinInst.operands.push_back(argOperands[0]);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(sinInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin جيب() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة جيب_تمام() - cos
                // (EN) cos() function - cosine
                // الأسماء المدعومة: جيب_تمام, cos, cosine
                if (funcName == Bm::COS)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة جيب_تمام تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction cosInst(SIROpcode::BUILTIN_COS);
                    cosInst.result = resultOp;
                    cosInst.operands.push_back(argOperands[0]);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(cosInst);
                    }

#ifndef NDEBUG
                    std::cout << "[DEBUG] buildFunctionCall: builtin جيب_تمام() -> " << resultReg << std::endl;
#endif
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة ظل() - tan
                // (EN) tan() function - tangent
                // الأسماء المدعومة: ظل, tan, tangent
                if (funcName == Bm::TAN)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة ظل تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }

                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);

                    SIRInstruction tanInst(SIROpcode::BUILTIN_TAN);
                    tanInst.result = resultOp;
                    tanInst.operands.push_back(argOperands[0]);

                    if (b_.currentBlock_)
                    {
                        b_.currentBlock_->instructions.push_back(tanInst);
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
                if (funcName == Bm::MAX)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة أكبر تتطلب معاملين" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    // (AR) نوعُ النتيجة = عشريّ إن كان أحدُ المعامِلَين عشريًّا، وإلّا صحيح (كنمطِ ABS:244).
                    //      المفسّرُ يعيدُ نوعَ الفائزِ (يتجاهلُ نوعَ SIR)، وLLVM يشتقُّ من نوعِ القيمة؛ فهذا
                    //      التغييرُ يخدمُ الخلفيّةَ الأصليّةَ (تعتمدُ result->dataType) بلا أثرٍ عليهما.
                    SadTypeKind maxType = (argResults[0].type == SadTypeKind::Float ||
                                           argResults[1].type == SadTypeKind::Float)
                                              ? SadTypeKind::Float : SadTypeKind::Integer;
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, maxType);
                    SIRInstruction inst(SIROpcode::BUILTIN_MAX);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, maxType);
                }

                // أصغر / min - الأصغر من قيمتين
                if (funcName == Bm::MIN)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة أصغر تتطلب معاملين" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    SadTypeKind minType = (argResults[0].type == SadTypeKind::Float ||
                                           argResults[1].type == SadTypeKind::Float)
                                              ? SadTypeKind::Float : SadTypeKind::Integer;
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, minType);
                    SIRInstruction inst(SIROpcode::BUILTIN_MIN);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, minType);
                }

                // جمع / sum - مجموع عناصر مصفوفة
                if (funcName == Bm::SUM)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة جمع تتطلب معامل واحد (مصفوفة)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::BUILTIN_SUM);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // ========================================================================
                // (AR) دوال رياضية إضافية — وحدة رياضيات الموسّعة
                // (EN) Extended math functions — stdlib رياضيات module
                // ========================================================================

                // (AR) دالة لوغ10() - لوغاريتم عشري
                // (EN) log10() function - Base-10 logarithm
                if (funcName == Bm::LOG10)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة لوغ10 تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::BUILTIN_LOG10);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة لوغ2() - لوغاريتم ثنائي
                // (EN) log2() function - Base-2 logarithm
                if (funcName == Bm::LOG2)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة لوغ2 تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::BUILTIN_LOG2);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة قوس_جيب() - asin
                // (EN) asin() function - Arc sine
                if (funcName == Bm::ASIN)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة قوس_جيب تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::BUILTIN_ASIN);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة قوس_جيب_تمام() - acos
                // (EN) acos() function - Arc cosine
                if (funcName == Bm::ACOS)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة قوس_جيب_تمام تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::BUILTIN_ACOS);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة قوس_ظل() - atan
                // (EN) atan() function - Arc tangent
                if (funcName == Bm::ATAN)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة قوس_ظل تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::BUILTIN_ATAN);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة اقتطاع() - truncate (قطع نحو الصفر)
                // (EN) trunc() function - Truncate toward zero
                if (funcName == Bm::TRUNCATE)
                {
                    if (argResults.empty())
                    {
                        std::cerr << "[Error] دالة اقتطاع تتطلب معامل واحد" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }
                    // (AR) مطابقة المفسّر (AdvancedMath::trunc = std::trunc): النتيجة عددٌ عشريّ
                    //      (قطعٌ نحو الصفر يُبقي النوع عشريًّا)، فـاقتطاع(3.7) ⇒ «3.0» لا «3».
                    //      السلوك السابق كان يُصلّب Integer (FPToSI في الخلف) فيُخرج «3».
                    // (EN) Match interpreter (AdvancedMath::trunc = std::trunc): result is a
                    //      double (truncation toward zero keeps the float type), so trunc(3.7)
                    //      ⇒ "3.0" not "3". The previous code pinned Integer (FPToSI in the
                    //      backend), emitting "3".
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::BUILTIN_TRUNC);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) دالة باقي() - باقي القسمة الصحيحة
                // (EN) mod() function - integer remainder
                if (funcName == Bm::FMOD)
                {
                    if (argResults.size() < 2)
                    {
                        std::cerr << "[Error] دالة باقي تتطلب معاملين (البسط والمقام)" << std::endl;
                        return BuildResult("", SadTypeKind::Integer);
                    }
                    // (AR) مطابقة المفسّر (AdvancedMath::mod = a % b على عددين صحيحين): «باقي»
                    //      باقٍ صحيح لا عشريّ. لذا نُصدر MOD_I64 (نظير عامل «%») الذي يطبّع
                    //      المعاملات إلى i64 (اقتطاعٌ نحو الصفر مطابقٌ لـtoInt) ثم SRem، فتُطبع
                    //      «1» لا «1.0». السلوك السابق (BUILTIN_FMOD = fmod عشريّ) كان يُخرج «1.0».
                    //      تنبيه: هذا لا يعني تطابق «باقي» مع عامل «%» دلاليًّا — «%» في المفسّر
                    //      يعمل fmod على العشريّين (7.5%2=1.5) بينما «باقي» يقتطع دومًا (باقي(7.5،2)=1).
                    // (EN) Match interpreter (AdvancedMath::mod = a % b on two ints): "باقي" is an
                    //      integer remainder, not a float one. Emit MOD_I64 (same opcode as the "%"
                    //      operator) which normalizes operands to i64 (truncation toward zero, as
                    //      toInt does) then SRem, printing "1" not "1.0". The previous behavior
                    //      (BUILTIN_FMOD = float fmod) emitted "1.0". Note: this does NOT make
                    //      "باقي" semantically identical to "%" — the interpreter's "%" does fmod
                    //      on floats (7.5%2=1.5) while "باقي" always truncates (باقي(7.5،2)=1).
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Integer);
                    SIRInstruction inst(SIROpcode::MOD_I64);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]);
                    inst.operands.push_back(argOperands[1]);
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Integer);
                }

                // (AR) دالة حصر() - تقييد القيمة ضمن نطاق [حد_أدنى، حد_أعلى]
                // (EN) clamp() function - Clamp value between min and max
                if (funcName == Bm::CLAMP)
                {
                    if (argResults.size() < 3)
                    {
                        std::cerr << "[Error] دالة حصر تتطلب 3 معاملات (القيمة، الحد_الأدنى، الحد_الأعلى)" << std::endl;
                        return BuildResult("", SadTypeKind::Float);
                    }
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::BUILTIN_CLAMP);
                    inst.result = resultOp;
                    inst.operands.push_back(argOperands[0]); // value
                    inst.operands.push_back(argOperands[1]); // min
                    inst.operands.push_back(argOperands[2]); // max
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) ثابت باي/ط — النسبة التقريبية π
                // (EN) Pi constant — returns 3.141592653589793
                if (funcName == Bm::PI || funcName == Bm::PI_ALT)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::MOVE);
                    inst.result = resultOp;
                    inst.operands.push_back(SIROperand::ConstantF64(3.141592653589793));
                    inst.comment = "π constant";
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                // (AR) ثابت إ/أولر — العدد e (أساس اللوغاريتم الطبيعي)
                // (EN) Euler's number constant — returns 2.718281828459045
                if (funcName == Bm::E)
                {
                    std::string resultReg = b_.newTempRegister();
                    SIROperand resultOp = SIROperand::Register(resultReg, SadTypeKind::Float);
                    SIRInstruction inst(SIROpcode::MOVE);
                    inst.result = resultOp;
                    inst.operands.push_back(SIROperand::ConstantF64(2.718281828459045));
                    inst.comment = "e constant (Euler)";
                    if (b_.currentBlock_)
                        b_.currentBlock_->instructions.push_back(inst);
                    return BuildResult(resultReg, SadTypeKind::Float);
                }

                return std::nullopt;
            }

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad