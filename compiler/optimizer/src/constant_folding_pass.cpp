/**
 * @file constant_folding_pass.cpp
 * @brief تطبيق تمرير طي الثوابت
 * @brief Constant Folding Pass Implementation
 * 
 * @details
 * (AR) يقيّم العمليات على الثوابت في وقت الترجمة.
 * (EN) Evaluates operations on constants at compile-time.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/constant_folding_pass.h"
#include <cmath>
#include <limits>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief Constructor / البناء
 */
ConstantFoldingPass::ConstantFoldingPass()
    : OptimizationPass("Constant Folding", PassType::TRANSFORMATION)
{
}

/**
 * @brief تشغيل التمرير على دالة
 * @brief Run pass on function
 */
bool ConstantFoldingPass::runOnFunction(SIR::SIRFunction* function) {
    if (!function) {
        return false;
    }
    
    bool modified = false;
    resetConstants();
    
    // معالجة كل كتلة أساسية / Process each basic block
    const auto& blocks = function->getBasicBlocks();
    for (auto& block : blocks) {
        if (block) {
            bool blockModified = processBlock(block.get());
            if (blockModified) {
                modified = true;
            }
        }
    }
    
    return modified;
}

/**
 * @brief معالجة كتلة أساسية
 * @brief Process basic block
 */
bool ConstantFoldingPass::processBlock(SIR::SIRBasicBlock* block) {
    if (!block) {
        return false;
    }
    
    bool modified = false;
    
    for (size_t i = 0; i < block->instructions.size(); i++) {
        auto& inst = block->instructions[i];
        auto opcode = inst.opcode;
        
        // معالجة LOAD_CONST / Handle LOAD_CONST
        if (opcode == SIR::SIROpcode::LOAD_CONST) {
            // تسجيل الثابت / Record constant
            if (inst.hasResult() && !inst.operands.empty()) {
                const auto& resultReg = inst.result->name;
                const auto& valueOperand = inst.operands[0];
                
                // تسجيل الثابت حسب نوعه / Record constant by type
                if (valueOperand.type == SIR::SIROperandType::CONSTANT) {
                    switch (valueOperand.dataType) {
                        case SIR::SIRType::I64:
                            recordConstant(resultReg, valueOperand.intValue);
                            break;
                        case SIR::SIRType::F64:
                            recordConstant(resultReg, valueOperand.floatValue);
                            break;
                        case SIR::SIRType::BOOL:
                            recordConstant(resultReg, valueOperand.boolValue);
                            break;
                        case SIR::SIRType::STRING:
                            recordConstant(resultReg, valueOperand.name);
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        
        // معالجة العمليات الثنائية / Handle binary operations
        else if (opcode == SIR::SIROpcode::ADD ||
                 opcode == SIR::SIROpcode::SUB ||
                 opcode == SIR::SIROpcode::MUL ||
                 opcode == SIR::SIROpcode::DIV ||
                 opcode == SIR::SIROpcode::MOD) {
            
            // محاولة طي العملية / Try to fold operation
            if (inst.operands.size() >= 2) {
                const auto& leftOp = inst.operands[0];
                const auto& rightOp = inst.operands[1];
                
                // فحص إذا كان كلا المعاملين ثوابت / Check if both operands are constants
                bool leftIsConst = (leftOp.type == SIR::SIROperandType::CONSTANT) ||
                                  (leftOp.type == SIR::SIROperandType::REGISTER && isConstant(leftOp.name));
                bool rightIsConst = (rightOp.type == SIR::SIROperandType::CONSTANT) ||
                                   (rightOp.type == SIR::SIROperandType::REGISTER && isConstant(rightOp.name));
                
                if (leftIsConst && rightIsConst) {
                    // الحصول على القيم / Get values
                    int64_t leftVal = 0, rightVal = 0;
                    
                    if (leftOp.type == SIR::SIROperandType::CONSTANT) {
                        leftVal = leftOp.intValue;
                    } else {
                        auto constVal = getConstantValue(leftOp.name);
                        if (constVal && std::holds_alternative<int64_t>(*constVal)) {
                            leftVal = std::get<int64_t>(*constVal);
                        } else {
                            continue;
                        }
                    }
                    
                    if (rightOp.type == SIR::SIROperandType::CONSTANT) {
                        rightVal = rightOp.intValue;
                    } else {
                        auto constVal = getConstantValue(rightOp.name);
                        if (constVal && std::holds_alternative<int64_t>(*constVal)) {
                            rightVal = std::get<int64_t>(*constVal);
                        } else {
                            continue;
                        }
                    }
                    
                    // طي العملية / Fold operation
                    auto result = foldIntBinary(opcode, leftVal, rightVal);
                    if (result) {
                        // استبدال التعليمة بـ LOAD_CONST / Replace with LOAD_CONST
                        inst.opcode = SIR::SIROpcode::LOAD_CONST;
                        inst.operands.clear();
                        inst.operands.push_back(SIR::SIROperand::ConstantI64(*result));
                        
                        // تسجيل الثابت الجديد / Record new constant
                        if (inst.hasResult()) {
                            recordConstant(inst.result->name, *result);
                        }
                        
                        modified = true;
                        recordModification();
                        debug("Folded binary operation: " + std::to_string(leftVal) + " op " + 
                              std::to_string(rightVal) + " = " + std::to_string(*result));
                    }
                }
            }
        }
        
        // معالجة عمليات المقارنة / Handle comparison operations
        else if (opcode == SIR::SIROpcode::ICMP_EQ ||
                 opcode == SIR::SIROpcode::ICMP_NE ||
                 opcode == SIR::SIROpcode::ICMP_LT ||
                 opcode == SIR::SIROpcode::ICMP_LE ||
                 opcode == SIR::SIROpcode::ICMP_GT ||
                 opcode == SIR::SIROpcode::ICMP_GE) {
            
            // محاولة طي المقارنة / Try to fold comparison
            if (inst.operands.size() >= 2) {
                const auto& leftOp = inst.operands[0];
                const auto& rightOp = inst.operands[1];
                
                // فحص إذا كان كلا المعاملين ثوابت / Check if both operands are constants
                bool leftIsConst = (leftOp.type == SIR::SIROperandType::CONSTANT) ||
                                  (leftOp.type == SIR::SIROperandType::REGISTER && isConstant(leftOp.name));
                bool rightIsConst = (rightOp.type == SIR::SIROperandType::CONSTANT) ||
                                   (rightOp.type == SIR::SIROperandType::REGISTER && isConstant(rightOp.name));
                
                if (leftIsConst && rightIsConst) {
                    // الحصول على القيم / Get values
                    int64_t leftVal = 0, rightVal = 0;
                    
                    if (leftOp.type == SIR::SIROperandType::CONSTANT) {
                        leftVal = leftOp.intValue;
                    } else {
                        auto constVal = getConstantValue(leftOp.name);
                        if (constVal && std::holds_alternative<int64_t>(*constVal)) {
                            leftVal = std::get<int64_t>(*constVal);
                        } else {
                            continue;
                        }
                    }
                    
                    if (rightOp.type == SIR::SIROperandType::CONSTANT) {
                        rightVal = rightOp.intValue;
                    } else {
                        auto constVal = getConstantValue(rightOp.name);
                        if (constVal && std::holds_alternative<int64_t>(*constVal)) {
                            rightVal = std::get<int64_t>(*constVal);
                        } else {
                            continue;
                        }
                    }
                    
                    // طي المقارنة / Fold comparison
                    auto result = foldIntComparison(opcode, leftVal, rightVal);
                    if (result) {
                        // استبدال التعليمة بـ LOAD_CONST / Replace with LOAD_CONST
                        inst.opcode = SIR::SIROpcode::LOAD_CONST;
                        inst.operands.clear();
                        inst.operands.push_back(SIR::SIROperand::ConstantBool(*result));
                        
                        // تسجيل الثابت الجديد / Record new constant
                        if (inst.hasResult()) {
                            recordConstant(inst.result->name, *result);
                        }
                        
                        modified = true;
                        recordModification();
                        debug("Folded comparison: " + std::to_string(leftVal) + " cmp " + 
                              std::to_string(rightVal) + " = " + (*result ? "true" : "false"));
                    }
                }
            }
        }
    }
    
    return modified;
}

/**
 * @brief طي عملية ثنائية على الأعداد الصحيحة
 * @brief Fold binary operation on integers
 */
std::optional<int64_t> ConstantFoldingPass::foldIntBinary(
    SIR::SIROpcode op,
    int64_t left,
    int64_t right
) {
    switch (op) {
        case SIR::SIROpcode::ADD:
            // فحص الفيض / Check overflow
            if ((right > 0 && left > std::numeric_limits<int64_t>::max() - right) ||
                (right < 0 && left < std::numeric_limits<int64_t>::min() - right)) {
                return std::nullopt;  // فيض / Overflow
            }
            return left + right;
            
        case SIR::SIROpcode::SUB:
            // فحص الفيض / Check overflow
            if ((right < 0 && left > std::numeric_limits<int64_t>::max() + right) ||
                (right > 0 && left < std::numeric_limits<int64_t>::min() + right)) {
                return std::nullopt;  // فيض / Overflow
            }
            return left - right;
            
        case SIR::SIROpcode::MUL:
            // فحص الفيض / Check overflow
            if (right != 0 && std::abs(left) > std::numeric_limits<int64_t>::max() / std::abs(right)) {
                return std::nullopt;  // فيض / Overflow
            }
            return left * right;
            
        case SIR::SIROpcode::DIV:
            // فحص القسمة على صفر / Check division by zero
            if (right == 0) {
                return std::nullopt;
            }
            // فحص أسوأ حالة / Check worst case
            if (left == std::numeric_limits<int64_t>::min() && right == -1) {
                return std::nullopt;
            }
            return left / right;
            
        case SIR::SIROpcode::MOD:
            // فحص القسمة على صفر / Check division by zero
            if (right == 0) {
                return std::nullopt;
            }
            return left % right;
            
        case SIR::SIROpcode::AND:
            return left & right;
            
        case SIR::SIROpcode::OR:
            return left | right;
            
        case SIR::SIROpcode::XOR:
            return left ^ right;
            
        case SIR::SIROpcode::SHL:
            if (right < 0 || right >= 64) {
                return std::nullopt;  // إزاحة غير صالحة / Invalid shift
            }
            return left << right;
            
        case SIR::SIROpcode::SHR:
            if (right < 0 || right >= 64) {
                return std::nullopt;  // إزاحة غير صالحة / Invalid shift
            }
            return left >> right;
            
        default:
            return std::nullopt;
    }
}

/**
 * @brief طي عملية ثنائية على الأعداد العشرية
 * @brief Fold binary operation on floats
 */
std::optional<double> ConstantFoldingPass::foldFloatBinary(
    SIR::SIROpcode op,
    double left,
    double right
) {
    switch (op) {
        case SIR::SIROpcode::FADD:
            return left + right;
            
        case SIR::SIROpcode::FSUB:
            return left - right;
            
        case SIR::SIROpcode::FMUL:
            return left * right;
            
        case SIR::SIROpcode::FDIV:
            // فحص القسمة على صفر / Check division by zero
            if (right == 0.0) {
                return std::nullopt;
            }
            return left / right;
            
        default:
            return std::nullopt;
    }
}

/**
 * @brief طي عملية مقارنة على الأعداد الصحيحة
 * @brief Fold comparison operation on integers
 */
std::optional<bool> ConstantFoldingPass::foldIntComparison(
    SIR::SIROpcode op,
    int64_t left,
    int64_t right
) {
    switch (op) {
        case SIR::SIROpcode::ICMP_EQ:
            return left == right;
            
        case SIR::SIROpcode::ICMP_NE:
            return left != right;
            
        case SIR::SIROpcode::ICMP_LT:
            return left < right;
            
        case SIR::SIROpcode::ICMP_LE:
            return left <= right;
            
        case SIR::SIROpcode::ICMP_GT:
            return left > right;
            
        case SIR::SIROpcode::ICMP_GE:
            return left >= right;
            
        default:
            return std::nullopt;
    }
}

/**
 * @brief طي عملية أحادية
 * @brief Fold unary operation
 */
std::optional<int64_t> ConstantFoldingPass::foldIntUnary(
    SIR::SIROpcode op,
    int64_t operand
) {
    switch (op) {
        case SIR::SIROpcode::NEG:
            // فحص الفيض / Check overflow
            if (operand == std::numeric_limits<int64_t>::min()) {
                return std::nullopt;
            }
            return -operand;
            
        case SIR::SIROpcode::NOT:
            return ~operand;
            
        default:
            return std::nullopt;
    }
}

/**
 * @brief التحقق من كون السجل ثابتاً
 * @brief Check if register is constant
 */
bool ConstantFoldingPass::isConstant(const std::string& reg) const {
    return constants_.find(reg) != constants_.end();
}

/**
 * @brief الحصول على قيمة الثابت
 * @brief Get constant value
 */
std::optional<ConstantValue> ConstantFoldingPass::getConstantValue(
    const std::string& reg
) const {
    auto it = constants_.find(reg);
    if (it != constants_.end()) {
        return it->second;
    }
    return std::nullopt;
}

/**
 * @brief تسجيل ثابت
 * @brief Record constant
 */
void ConstantFoldingPass::recordConstant(const std::string& reg, ConstantValue value) {
    constants_[reg] = value;
    debug("Recorded constant: " + reg);
}

/**
 * @brief حذف ثابت
 * @brief Remove constant
 */
void ConstantFoldingPass::removeConstant(const std::string& reg) {
    constants_.erase(reg);
}

/**
 * @brief إعادة تعيين جدول الثوابت
 * @brief Reset constants table
 */
void ConstantFoldingPass::resetConstants() {
    constants_.clear();
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
