/**
 * @file strength_reduction_pass.cpp
 * @brief تنفيذ تمرير تقليل القوة
 * @brief Strength Reduction Pass Implementation
 * 
 * @details
 * (AR) يستبدل العمليات الحسابية المكلفة بعمليات أخف.
 * (EN) Replaces expensive arithmetic operations with cheaper alternatives.
 * 
 * @author SadLanguage Compiler Team
 * @date January 2026
 */

#include "middle/strength_reduction_pass.h"
#include <iostream>

namespace Sad {
namespace Compiler {
namespace Optimizer {

// ============================================================================
// Constructor / البناء
// ============================================================================

StrengthReductionPass::StrengthReductionPass()
    : OptimizationPass("Strength Reduction", PassType::TRANSFORMATION)
    , reductionCount_(0)
{
}

StrengthReductionPass::~StrengthReductionPass() = default;

// ============================================================================
// Main Entry Point
// ============================================================================

bool StrengthReductionPass::runOnFunction(SIR::SIRFunction* function) {
    if (!function) return false;
    
    reductionCount_ = 0;
    bool modified = false;
    
    for (auto& blockPtr : function->basicBlocks) {
        if (processBlock(blockPtr.get())) {
            modified = true;
        }
    }
    
    return modified;
}

// ============================================================================
// Block Processing / معالجة الكتل
// ============================================================================

bool StrengthReductionPass::processBlock(SIR::SIRBasicBlock* block) {
    if (!block) return false;
    
    bool modified = false;
    
    for (auto& inst : block->instructions) {
        // (AR) محاولة التبسيط الجبري أولاً
        // (EN) Try algebraic simplification first
        if (tryAlgebraicSimplification(inst)) {
            modified = true;
            continue;
        }
        
        // (AR) محاولة تقليل القوة حسب نوع العملية
        // (EN) Try strength reduction by operation type
        switch (inst.opcode) {
            case SIR::SIROpcode::MUL_I64:
                if (tryReduceMultiply(inst)) modified = true;
                break;
                
            case SIR::SIROpcode::DIV_I64:
                if (tryReduceDivision(inst)) modified = true;
                break;
                
            case SIR::SIROpcode::FLOOR_DIV_I64:
                if (tryReduceDivision(inst)) modified = true;
                break;
                
            case SIR::SIROpcode::MOD_I64:
                if (tryReduceModulo(inst)) modified = true;
                break;
                
            default:
                break;
        }
    }
    
    return modified;
}

// ============================================================================
// Strength Reduction Transforms / تحويلات تقليل القوة
// ============================================================================

bool StrengthReductionPass::tryReduceMultiply(SIR::SIRInstruction& inst) {
    if (inst.operands.size() < 2) return false;
    
    // (AR) محاولة إيجاد المعامل الثابت (قوة 2)
    // (EN) Try to find the constant operand (power of 2)
    for (size_t i = 0; i < 2; ++i) {
        auto constVal = getIntConstant(inst.operands[i]);
        if (constVal && isPowerOfTwo(*constVal) && *constVal > 1) {
            int shiftAmount = log2(*constVal);
            
            // (AR) تحويل: x * (2^n) → x << n
            // (EN) Transform: x * (2^n) → x << n
            size_t otherIdx = 1 - i;
            inst.opcode = SIR::SIROpcode::SHL;
            inst.operands[0] = inst.operands[otherIdx];
            inst.operands[1] = SIR::SIROperand::ConstantI64(shiftAmount);
            
            reductionCount_++;
            recordModification();
            return true;
        }
    }
    
    return false;
}

bool StrengthReductionPass::tryReduceDivision(SIR::SIRInstruction& inst) {
    if (inst.operands.size() < 2) return false;
    
    // (AR) التحقق من أن القاسم ثابت وقوة 2
    // (EN) Check if divisor is constant and power of 2
    auto constVal = getIntConstant(inst.operands[1]);
    if (constVal && isPowerOfTwo(*constVal) && *constVal > 1) {
        int shiftAmount = log2(*constVal);
        
        // (AR) تحويل: x / (2^n) → x >> n (for unsigned/positive)
        // (EN) Transform: x / (2^n) → x >> n
        inst.opcode = SIR::SIROpcode::SHR;
        inst.operands[1] = SIR::SIROperand::ConstantI64(shiftAmount);
        
        reductionCount_++;
        recordModification();
        return true;
    }
    
    return false;
}

bool StrengthReductionPass::tryReduceModulo(SIR::SIRInstruction& inst) {
    if (inst.operands.size() < 2) return false;
    
    // (AR) التحقق من أن القاسم ثابت وقوة 2
    // (EN) Check if divisor is constant and power of 2
    auto constVal = getIntConstant(inst.operands[1]);
    if (constVal && isPowerOfTwo(*constVal) && *constVal > 1) {
        // (AR) تحويل: x % (2^n) → x & (2^n - 1)
        // (EN) Transform: x % (2^n) → x & (2^n - 1)
        inst.opcode = SIR::SIROpcode::AND;
        inst.operands[1] = SIR::SIROperand::ConstantI64(*constVal - 1);
        
        reductionCount_++;
        recordModification();
        return true;
    }
    
    return false;
}

// ============================================================================
// Algebraic Simplification / التبسيط الجبري
// ============================================================================

bool StrengthReductionPass::tryAlgebraicSimplification(SIR::SIRInstruction& inst) {
    if (inst.operands.size() < 2) return false;
    if (!inst.hasResult()) return false;
    
    auto c0 = getIntConstant(inst.operands[0]);
    auto c1 = getIntConstant(inst.operands[1]);
    
    switch (inst.opcode) {
        // (AR) x + 0 → x, 0 + x → x
        case SIR::SIROpcode::ADD_I64: {
            if (c1 && *c1 == 0) {
                // (AR) تحويل إلى نسخ / Convert to move
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            if (c0 && *c0 == 0) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[1]};
                recordModification();
                return true;
            }
            break;
        }
        
        // (AR) x - 0 → x
        case SIR::SIROpcode::SUB_I64: {
            if (c1 && *c1 == 0) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            // (AR) x - x → 0
            if (inst.operands[0].type == SIR::SIROperandType::REGISTER &&
                inst.operands[1].type == SIR::SIROperandType::REGISTER &&
                inst.operands[0].name == inst.operands[1].name) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {SIR::SIROperand::ConstantI64(0)};
                recordModification();
                return true;
            }
            break;
        }
        
        // (AR) x * 0 → 0, x * 1 → x, 0 * x → 0, 1 * x → x
        case SIR::SIROpcode::MUL_I64: {
            for (size_t i = 0; i < 2; ++i) {
                auto cv = (i == 0) ? c0 : c1;
                if (cv) {
                    if (*cv == 0) {
                        inst.opcode = SIR::SIROpcode::MOVE;
                        inst.operands = {SIR::SIROperand::ConstantI64(0)};
                        recordModification();
                        return true;
                    }
                    if (*cv == 1) {
                        size_t otherIdx = 1 - i;
                        inst.opcode = SIR::SIROpcode::MOVE;
                        inst.operands = {inst.operands[otherIdx]};
                        recordModification();
                        return true;
                    }
                }
            }
            break;
        }
        
        // (AR) x / 1 → x
        case SIR::SIROpcode::DIV_I64:
        case SIR::SIROpcode::FLOOR_DIV_I64: {
            if (c1 && *c1 == 1) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            break;
        }
        
        // (AR) x & 0 → 0, x & -1 → x, x | 0 → x
        case SIR::SIROpcode::AND: {
            if (c1 && *c1 == 0) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {SIR::SIROperand::ConstantI64(0)};
                recordModification();
                return true;
            }
            if (c1 && *c1 == -1) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            break;
        }
        
        case SIR::SIROpcode::OR: {
            if (c1 && *c1 == 0) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            break;
        }
        
        // (AR) x ^ 0 → x
        case SIR::SIROpcode::XOR: {
            if (c1 && *c1 == 0) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {inst.operands[0]};
                recordModification();
                return true;
            }
            // (AR) x ^ x → 0
            if (inst.operands[0].type == SIR::SIROperandType::REGISTER &&
                inst.operands[1].type == SIR::SIROperandType::REGISTER &&
                inst.operands[0].name == inst.operands[1].name) {
                inst.opcode = SIR::SIROpcode::MOVE;
                inst.operands = {SIR::SIROperand::ConstantI64(0)};
                recordModification();
                return true;
            }
            break;
        }
        
        default:
            break;
    }
    
    return false;
}

// ============================================================================
// Helper Methods / دوال مساعدة
// ============================================================================

bool StrengthReductionPass::isPowerOfTwo(int64_t value) const {
    if (value <= 0) return false;
    return (value & (value - 1)) == 0;
}

int StrengthReductionPass::log2(int64_t value) const {
    int result = 0;
    while (value > 1) {
        value >>= 1;
        result++;
    }
    return result;
}

std::optional<int64_t> StrengthReductionPass::getIntConstant(
    const SIR::SIROperand& operand) const
{
    if (operand.type == SIR::SIROperandType::CONSTANT) {
        return operand.intValue;
    }
    return std::nullopt;
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
