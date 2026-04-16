/**
 * @file constant_folding_pass.cpp
 * @brief تنفيذ تمرير طي الثوابت
 * @brief Constant Folding Pass Implementation
 *
 * @details
 * (AR) يقوم هذا التمرير بطي العمليات الحسابية على الثوابت في وقت الترجمة.
 * (EN) This pass folds arithmetic operations on constants at compile time.
 *
 * @author SadLanguage Compiler Team
 * @date December 2025
 */

#include "middle/constant_folding_pass.h"
#include <cmath>
#include <iostream>

namespace Sad
{
    namespace Compiler
    {
        namespace Optimizer
        {

            // ============================================================================
            // Constructor / البناء
            // ============================================================================

            ConstantFoldingPass::ConstantFoldingPass()
                : OptimizationPass("Constant Folding", PassType::TRANSFORMATION), foldCount_(0)
            {
            }

            ConstantFoldingPass::~ConstantFoldingPass() = default;

            // ============================================================================
            // Main Entry Point
            // ============================================================================

            bool ConstantFoldingPass::runOnFunction(SIR::SIRFunction *function)
            {
                if (!function)
                {
                    return false;
                }

                clearConstantTable();
                bool modified = false;

                // Process all basic blocks
                for (auto &blockPtr : function->basicBlocks)
                {
                    if (processBlock(blockPtr.get()))
                    {
                        modified = true;
                    }
                }

                if (foldCount_ > 0)
                {
                    std::cerr << "[ConstantFolding] Folded " << foldCount_ << " operations\n";
                }

                return modified;
            }

            // ============================================================================
            // Block Processing
            // ============================================================================

            bool ConstantFoldingPass::processBlock(SIR::SIRBasicBlock *block)
            {
                if (!block)
                {
                    return false;
                }

                bool modified = false;

                for (size_t i = 0; i < block->instructions.size(); i++)
                {
                    auto &inst = block->instructions[i];

                    // Try to fold based on opcode
                    using SIR::SIROpcode;

                    switch (inst.opcode)
                    {
                    // Integer arithmetic
                    case SIROpcode::ADD_I64:
                    case SIROpcode::SUB_I64:
                    case SIROpcode::MUL_I64:
                    case SIROpcode::DIV_I64:
                    case SIROpcode::FLOOR_DIV_I64:
                    case SIROpcode::MOD_I64:
                        if (tryFoldIntegerArithmetic(inst))
                        {
                            modified = true;
                            foldCount_++;
                            recordModification();
                        }
                        break;

                    // Float arithmetic
                    case SIROpcode::ADD_F64:
                    case SIROpcode::SUB_F64:
                    case SIROpcode::MUL_F64:
                    case SIROpcode::DIV_F64:
                        if (tryFoldFloatArithmetic(inst))
                        {
                            modified = true;
                            foldCount_++;
                            recordModification();
                        }
                        break;

                    // Comparisons
                    case SIROpcode::EQ:
                    case SIROpcode::NE:
                    case SIROpcode::LT:
                    case SIROpcode::LE:
                    case SIROpcode::GT:
                    case SIROpcode::GE:
                        if (tryFoldComparison(inst))
                        {
                            modified = true;
                            foldCount_++;
                            recordModification();
                        }
                        break;

                    // Bitwise operations
                    case SIROpcode::AND:
                    case SIROpcode::OR:
                    case SIROpcode::XOR:
                    case SIROpcode::NOT:
                    case SIROpcode::SHL:
                    case SIROpcode::SHR:
                        if (tryFoldBitwise(inst))
                        {
                            modified = true;
                            foldCount_++;
                            recordModification();
                        }
                        break;

                    // Negation
                    case SIROpcode::NEG:
                        if (tryFoldNegation(inst))
                        {
                            modified = true;
                            foldCount_++;
                            recordModification();
                        }
                        break;

                    default:
                        // Track constants for propagation
                        trackConstant(inst);
                        break;
                    }
                }

                return modified;
            }

            // ============================================================================
            // Integer Arithmetic Folding
            // ============================================================================

            bool ConstantFoldingPass::tryFoldIntegerArithmetic(SIR::SIRInstruction &inst)
            {
                if (inst.operands.size() < 2)
                {
                    return false;
                }

                const auto &leftOp = inst.operands[0];
                const auto &rightOp = inst.operands[1];

                // Check if both operands are constants
                std::optional<int64_t> leftVal = getIntegerConstant(leftOp);
                std::optional<int64_t> rightVal = getIntegerConstant(rightOp);

                if (!leftVal || !rightVal)
                {
                    return false;
                }

                int64_t result = 0;
                using SIR::SIROpcode;

                switch (inst.opcode)
                {
                case SIROpcode::ADD_I64:
                    result = *leftVal + *rightVal;
                    break;
                case SIROpcode::SUB_I64:
                    result = *leftVal - *rightVal;
                    break;
                case SIROpcode::MUL_I64:
                    result = *leftVal * *rightVal;
                    break;
                case SIROpcode::DIV_I64:
                    if (*rightVal == 0)
                        return false; // Avoid division by zero
                    result = *leftVal / *rightVal;
                    break;
                case SIROpcode::FLOOR_DIV_I64:
                {
                    if (*rightVal == 0)
                        return false;
                    int64_t q = *leftVal / *rightVal;
                    if ((*leftVal ^ *rightVal) < 0 && *leftVal % *rightVal != 0)
                        q -= 1;
                    result = q;
                    break;
                }
                case SIROpcode::MOD_I64:
                    if (*rightVal == 0)
                        return false; // Avoid division by zero
                    result = *leftVal % *rightVal;
                    break;
                default:
                    return false;
                }

                // Replace instruction with constant
                replaceWithConstant(inst, result);
                return true;
            }

            // ============================================================================
            // Float Arithmetic Folding
            // ============================================================================

            bool ConstantFoldingPass::tryFoldFloatArithmetic(SIR::SIRInstruction &inst)
            {
                if (inst.operands.size() < 2)
                {
                    return false;
                }

                const auto &leftOp = inst.operands[0];
                const auto &rightOp = inst.operands[1];

                // Check if both operands are constants
                std::optional<double> leftVal = getFloatConstant(leftOp);
                std::optional<double> rightVal = getFloatConstant(rightOp);

                if (!leftVal || !rightVal)
                {
                    return false;
                }

                double result = 0.0;
                using SIR::SIROpcode;

                switch (inst.opcode)
                {
                case SIROpcode::ADD_F64:
                    result = *leftVal + *rightVal;
                    break;
                case SIROpcode::SUB_F64:
                    result = *leftVal - *rightVal;
                    break;
                case SIROpcode::MUL_F64:
                    result = *leftVal * *rightVal;
                    break;
                case SIROpcode::DIV_F64:
                    if (*rightVal == 0.0)
                        return false; // Avoid division by zero
                    result = *leftVal / *rightVal;
                    break;
                default:
                    return false;
                }

                // Replace instruction with constant
                replaceWithConstant(inst, result);
                return true;
            }

            // ============================================================================
            // Comparison Folding
            // ============================================================================

            bool ConstantFoldingPass::tryFoldComparison(SIR::SIRInstruction &inst)
            {
                if (inst.operands.size() < 2)
                {
                    return false;
                }

                const auto &leftOp = inst.operands[0];
                const auto &rightOp = inst.operands[1];

                // Try integer comparison first
                std::optional<int64_t> leftInt = getIntegerConstant(leftOp);
                std::optional<int64_t> rightInt = getIntegerConstant(rightOp);

                if (leftInt && rightInt)
                {
                    bool result = false;
                    using SIR::SIROpcode;

                    switch (inst.opcode)
                    {
                    case SIROpcode::EQ:
                        result = (*leftInt == *rightInt);
                        break;
                    case SIROpcode::NE:
                        result = (*leftInt != *rightInt);
                        break;
                    case SIROpcode::LT:
                        result = (*leftInt < *rightInt);
                        break;
                    case SIROpcode::LE:
                        result = (*leftInt <= *rightInt);
                        break;
                    case SIROpcode::GT:
                        result = (*leftInt > *rightInt);
                        break;
                    case SIROpcode::GE:
                        result = (*leftInt >= *rightInt);
                        break;
                    default:
                        return false;
                    }

                    replaceWithConstant(inst, result);
                    return true;
                }

                // Try float comparison
                std::optional<double> leftFloat = getFloatConstant(leftOp);
                std::optional<double> rightFloat = getFloatConstant(rightOp);

                if (leftFloat && rightFloat)
                {
                    bool result = false;
                    using SIR::SIROpcode;

                    switch (inst.opcode)
                    {
                    case SIROpcode::EQ:
                        result = (*leftFloat == *rightFloat);
                        break;
                    case SIROpcode::NE:
                        result = (*leftFloat != *rightFloat);
                        break;
                    case SIROpcode::LT:
                        result = (*leftFloat < *rightFloat);
                        break;
                    case SIROpcode::LE:
                        result = (*leftFloat <= *rightFloat);
                        break;
                    case SIROpcode::GT:
                        result = (*leftFloat > *rightFloat);
                        break;
                    case SIROpcode::GE:
                        result = (*leftFloat >= *rightFloat);
                        break;
                    default:
                        return false;
                    }

                    replaceWithConstant(inst, result);
                    return true;
                }

                return false;
            }

            // ============================================================================
            // Bitwise Folding
            // ============================================================================

            bool ConstantFoldingPass::tryFoldBitwise(SIR::SIRInstruction &inst)
            {
                using SIR::SIROpcode;

                // NOT هو أحادي — إذا كان المعامل منطقي نعكسه كمنطقي
                if (inst.opcode == SIROpcode::NOT)
                {
                    if (inst.operands.empty())
                        return false;

                    bool isBooleanNot = (inst.operands[0].dataType == SIR::SadTypeKind::Boolean);
                    std::optional<int64_t> val = getIntegerConstant(inst.operands[0]);
                    if (!val)
                        return false;

                    if (isBooleanNot)
                    {
                        // NOT على منطقي: نعكس القيمة المنطقية
                        replaceWithConstant(inst, (*val == 0));
                    }
                    else
                    {
                        replaceWithConstant(inst, ~(*val));
                    }
                    return true;
                }

                // العمليات الثنائية
                if (inst.operands.size() < 2)
                {
                    return false;
                }

                // فحص إذا كان كلا المعاملين منطقيين (Boolean)
                bool isBooleanOp = (inst.operands[0].dataType == SIR::SadTypeKind::Boolean &&
                                    inst.operands[1].dataType == SIR::SadTypeKind::Boolean);

                std::optional<int64_t> leftVal = getIntegerConstant(inst.operands[0]);
                std::optional<int64_t> rightVal = getIntegerConstant(inst.operands[1]);

                if (!leftVal || !rightVal)
                {
                    return false;
                }

                int64_t result = 0;

                switch (inst.opcode)
                {
                case SIROpcode::AND:
                    result = *leftVal & *rightVal;
                    break;
                case SIROpcode::OR:
                    result = *leftVal | *rightVal;
                    break;
                case SIROpcode::XOR:
                    result = *leftVal ^ *rightVal;
                    break;
                case SIROpcode::SHL:
                    result = *leftVal << *rightVal;
                    break;
                case SIROpcode::SHR:
                    result = static_cast<uint64_t>(*leftVal) >> *rightVal;
                    break;
                default:
                    return false;
                }

                // إذا كانت العملية AND/OR على معاملات منطقية، نحافظ على النوع المنطقي
                if (isBooleanOp && (inst.opcode == SIROpcode::AND || inst.opcode == SIROpcode::OR))
                {
                    replaceWithConstant(inst, result != 0);
                }
                else
                {
                    replaceWithConstant(inst, result);
                }
                return true;
            }

            // ============================================================================
            // Negation Folding
            // ============================================================================

            bool ConstantFoldingPass::tryFoldNegation(SIR::SIRInstruction &inst)
            {
                if (inst.operands.empty())
                {
                    return false;
                }

                // Try integer negation
                std::optional<int64_t> intVal = getIntegerConstant(inst.operands[0]);
                if (intVal)
                {
                    replaceWithConstant(inst, -(*intVal));
                    return true;
                }

                // Try float negation
                std::optional<double> floatVal = getFloatConstant(inst.operands[0]);
                if (floatVal)
                {
                    replaceWithConstant(inst, -(*floatVal));
                    return true;
                }

                return false;
            }

            // ============================================================================
            // Helper Methods
            // ============================================================================

            std::optional<int64_t> ConstantFoldingPass::getIntegerConstant(const SIR::SIROperand &operand)
            {
                if (operand.type == SIR::SIROperandType::CONSTANT)
                {
                    // Try to parse as integer
                    try
                    {
                        return std::stoll(operand.name);
                    }
                    catch (...)
                    {
                        return std::nullopt;
                    }
                }

                // Check constant table
                auto it = intConstants_.find(operand.name);
                if (it != intConstants_.end())
                {
                    return it->second;
                }

                return std::nullopt;
            }

            std::optional<double> ConstantFoldingPass::getFloatConstant(const SIR::SIROperand &operand)
            {
                if (operand.type == SIR::SIROperandType::CONSTANT)
                {
                    // Try to parse as float
                    try
                    {
                        return std::stod(operand.name);
                    }
                    catch (...)
                    {
                        return std::nullopt;
                    }
                }

                // Check constant table
                auto it = floatConstants_.find(operand.name);
                if (it != floatConstants_.end())
                {
                    return it->second;
                }

                return std::nullopt;
            }

            void ConstantFoldingPass::replaceWithConstant(SIR::SIRInstruction &inst, int64_t value)
            {
                // Keep the result, clear operands, set constant
                inst.operands.clear();

                SIR::SIROperand constOp;
                constOp.type = SIR::SIROperandType::CONSTANT;
                constOp.name = std::to_string(value);
                constOp.dataType = SIR::SadTypeKind::Integer;
                inst.operands.push_back(constOp);

                // Change opcode to MOVE (simple copy)
                inst.opcode = SIR::SIROpcode::MOVE;

                // Track the result as a constant
                if (inst.result.has_value())
                {
                    intConstants_[inst.result->name] = value;
                }
            }

            void ConstantFoldingPass::replaceWithConstant(SIR::SIRInstruction &inst, double value)
            {
                inst.operands.clear();

                SIR::SIROperand constOp;
                constOp.type = SIR::SIROperandType::CONSTANT;
                constOp.name = std::to_string(value);
                constOp.dataType = SIR::SadTypeKind::Float;
                inst.operands.push_back(constOp);

                inst.opcode = SIR::SIROpcode::MOVE;

                if (inst.result.has_value())
                {
                    floatConstants_[inst.result->name] = value;
                }
            }

            void ConstantFoldingPass::replaceWithConstant(SIR::SIRInstruction &inst, bool value)
            {
                inst.operands.clear();

                // (AR) استخدام ConstantBool لضمان ضبط boolValue في union بشكل صحيح
                //      بدلاً من ضبط الحقول يدوياً (الذي كان يترك boolValue = 0)
                // (EN) Use ConstantBool to properly set boolValue in the union
                //      instead of manually setting fields (which left boolValue = 0)
                SIR::SIROperand constOp = SIR::SIROperand::ConstantBool(value);
                inst.operands.push_back(constOp);

                inst.opcode = SIR::SIROpcode::MOVE;

                if (inst.result.has_value())
                {
                    intConstants_[inst.result->name] = value ? 1 : 0;
                }
            }

            void ConstantFoldingPass::trackConstant(const SIR::SIRInstruction &inst)
            {
                // Track MOVE instructions with constant operands
                if (inst.opcode == SIR::SIROpcode::MOVE &&
                    inst.result.has_value() &&
                    !inst.operands.empty())
                {

                    const auto &operand = inst.operands[0];
                    if (operand.type == SIR::SIROperandType::CONSTANT)
                    {
                        try
                        {
                            // Try integer first
                            int64_t intVal = std::stoll(operand.name);
                            intConstants_[inst.result->name] = intVal;
                        }
                        catch (...)
                        {
                            try
                            {
                                // Try float
                                double floatVal = std::stod(operand.name);
                                floatConstants_[inst.result->name] = floatVal;
                            }
                            catch (...)
                            {
                                // Not a numeric constant
                            }
                        }
                    }
                }
            }

            void ConstantFoldingPass::clearConstantTable()
            {
                intConstants_.clear();
                floatConstants_.clear();
            }

        } // namespace Optimizer
    } // namespace Compiler
} // namespace Sad
