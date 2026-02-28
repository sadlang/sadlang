/**
 * @file strength_reduction_pass.h
 * @brief تمرير تقليل القوة (Strength Reduction)
 * @brief Strength Reduction Pass
 * 
 * @details
 * (AR) يستبدل العمليات المكلفة بعمليات أرخص محسابياً:
 *      - الضرب بقوة 2 → إزاحة لليسار (x * 8 → x << 3)
 *      - القسمة على قوة 2 → إزاحة لليمين (x / 4 → x >> 2)
 *      - باقي القسمة على قوة 2 → AND (x % 8 → x & 7)
 *      - الضرب بصفر → صفر (x * 0 → 0)
 *      - الضرب بواحد → الهوية (x * 1 → x)
 *      - الجمع مع صفر → الهوية (x + 0 → x)
 *      - القوة (x ** 2 → x * x)
 * 
 * (EN) Replaces expensive operations with cheaper equivalents:
 *      - Multiply by power of 2 → shift left (x * 8 → x << 3)
 *      - Divide by power of 2 → shift right (x / 4 → x >> 2)
 *      - Modulo by power of 2 → AND (x % 8 → x & 7)
 *      - Multiply by 0 → 0
 *      - Multiply by 1 → identity
 *      - Add 0 → identity
 *      - Power of 2 → multiply chain
 * 
 * @author SadLanguage Compiler Team
 * @date January 2026
 * @version 1.0
 */

#pragma once

#include "pass.h"
#include <optional>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief تمرير تقليل القوة
 * @brief Strength Reduction Pass
 */
class StrengthReductionPass : public OptimizationPass {
public:
    StrengthReductionPass();
    ~StrengthReductionPass();
    
    bool runOnFunction(SIR::SIRFunction* function) override;

private:
    /**
     * @brief معالجة كتلة أساسية
     * @brief Process basic block
     */
    bool processBlock(SIR::SIRBasicBlock* block);
    
    /**
     * @brief محاولة تقليل القوة لعملية ضرب
     * @brief Try strength reduction for multiplication
     */
    bool tryReduceMultiply(SIR::SIRInstruction& inst);
    
    /**
     * @brief محاولة تقليل القوة لعملية قسمة
     * @brief Try strength reduction for division
     */
    bool tryReduceDivision(SIR::SIRInstruction& inst);
    
    /**
     * @brief محاولة تقليل القوة لعملية باقي القسمة
     * @brief Try strength reduction for modulo
     */
    bool tryReduceModulo(SIR::SIRInstruction& inst);
    
    /**
     * @brief محاولة تبسيط العمليات الحسابية
     * @brief Try algebraic simplification
     * 
     * @details
     * (AR) x + 0 → x, x - 0 → x, x * 1 → x, x * 0 → 0, x / 1 → x
     * (EN) Algebraic identity simplifications
     */
    bool tryAlgebraicSimplification(SIR::SIRInstruction& inst);
    
    /**
     * @brief التحقق مما إذا كانت القيمة قوة 2
     * @brief Check if value is power of 2
     */
    bool isPowerOfTwo(int64_t value) const;
    
    /**
     * @brief حساب لوغاريتم أساس 2
     * @brief Calculate log base 2
     */
    int log2(int64_t value) const;
    
    /**
     * @brief الحصول على ثابت صحيح من معامل
     * @brief Get integer constant from operand
     */
    std::optional<int64_t> getIntConstant(const SIR::SIROperand& operand) const;
    
    int reductionCount_;  ///< عدد التقليلات / Reduction count
};

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
