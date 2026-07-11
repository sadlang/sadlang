/**
 * @file constant_folding_pass.h
 * @brief تمرير طي الثوابت
 * @brief Constant Folding Pass
 * 
 * @details
 * (AR) يقيّم العمليات على الثوابت في وقت الترجمة بدلاً من وقت التشغيل.
 *      مثال: 10 + 20 -> 30, 5 * 6 -> 30
 * 
 * (EN) Evaluates operations on constants at compile-time instead of runtime.
 *      Example: 10 + 20 -> 30, 5 * 6 -> 30
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#pragma once

#include "pass.h"
#include <optional>
#include <unordered_map>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief تمرير طي الثوابت
 * @brief Constant Folding Pass
 */
class ConstantFoldingPass : public OptimizationPass {
public:
    ConstantFoldingPass();
    ~ConstantFoldingPass();
    
    bool runOnFunction(SIR::SIRFunction* function) override;

private:
    bool processBlock(SIR::SIRBasicBlock* block);
    
    // Folding methods
    bool tryFoldIntegerArithmetic(SIR::SIRInstruction& inst);
    bool tryFoldFloatArithmetic(SIR::SIRInstruction& inst);
    bool tryFoldComparison(SIR::SIRInstruction& inst);
    bool tryFoldBitwise(SIR::SIRInstruction& inst);
    bool tryFoldNegation(SIR::SIRInstruction& inst);
    
    // Helper methods
    std::optional<int64_t> getIntegerConstant(const SIR::SIROperand& operand);
    std::optional<double> getFloatConstant(const SIR::SIROperand& operand);
    // (AR) ISSUE-063: هل المعامل عشريّ (نوعًا، أو نصًّا عشريًّا، أو من جدول الثوابت العشريّة)؟
    // (EN) ISSUE-063: is the operand a float (by type, float-literal text, or the float table)?
    bool isFloatOperand(const SIR::SIROperand& operand);
    
    void replaceWithConstant(SIR::SIRInstruction& inst, int64_t value);
    void replaceWithConstant(SIR::SIRInstruction& inst, double value);
    void replaceWithConstant(SIR::SIRInstruction& inst, bool value);
    
    void trackConstant(const SIR::SIRInstruction& inst);
    void clearConstantTable();
    
    // Constant tables
    std::unordered_map<std::string, int64_t> intConstants_;
    std::unordered_map<std::string, double> floatConstants_;
    
    int foldCount_;
};

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
