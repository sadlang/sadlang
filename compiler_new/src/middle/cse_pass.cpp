/**
 * @file cse_pass.cpp
 * @brief Common Subexpression Elimination Pass Implementation
 * @brief تطبيق تمرير حذف التعابير الفرعية المشتركة
 * 
 * @author SadLanguage Compiler Team
 * @date January 2026 - Fixed API
 */

#include "middle/cse_pass.h"
#include <algorithm>
#include <sstream>

namespace Sad {
namespace Compiler {
namespace Optimizer {

// ============================================================================
// ExpressionKey Implementation
// ============================================================================

size_t ExpressionKey::hash() const {
    size_t h = std::hash<int>{}(static_cast<int>(opcode));
    
    for (const auto& operand : operands) {
        h ^= std::hash<std::string>{}(operand) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    
    return h;
}

// ============================================================================
// CSEPass Implementation
// ============================================================================

CSEPass::CSEPass()
    : OptimizationPass("CSE", PassType::TRANSFORMATION)
    , replacementCount_(0)
{
}

CSEPass::~CSEPass() = default;

bool CSEPass::runOnFunction(SIR::SIRFunction* function) {
    if (!function) {
        return false;
    }
    
    bool modified = false;
    replacementCount_ = 0;
    
    // Iterate over all basic blocks
    auto& blocks = function->getBasicBlocks();
    
    for (size_t blockIdx = 0; blockIdx < blocks.size(); ++blockIdx) {
        auto& block = blocks[blockIdx];
        if (!block) continue;
        
        // Clear expression table for each block - LOCAL CSE only.
        // Cross-block CSE requires dominance analysis which we don't have,
        // and causes "Instruction does not dominate all uses!" LLVM errors.
        clearTables();
        
        // instructions is vector<SIRInstruction> (not unique_ptr)
        auto& instructions = block->instructions;
        
        // Track indices of instructions to remove
        std::vector<size_t> indicesToRemove;
        
        for (size_t instIdx = 0; instIdx < instructions.size(); ++instIdx) {
            auto& inst = instructions[instIdx];
            
            // Check if already marked for removal
            bool alreadyRemoved = std::find(indicesToRemove.begin(), 
                                            indicesToRemove.end(), 
                                            instIdx) != indicesToRemove.end();
            if (alreadyRemoved) {
                continue;
            }
            
            // Check if instruction is eliminable
            if (!isEliminable(&inst)) {
                continue;
            }
            
            // Analyze expression
            auto exprKey = analyzeExpression(&inst);
            if (!exprKey) {
                continue;
            }
            
            // Look for similar expression
            auto it = expressionTable_.find(*exprKey);
            
            if (it != expressionTable_.end()) {
                // Found duplicate expression!
                const auto& savedExpr = it->second;
                
                // Get result register name from optional<SIROperand>
                if (inst.result.has_value()) {
                    std::string currentResult = inst.result->name;
                    if (!currentResult.empty()) {
                        replaceRegisterUses(
                            function,
                            currentResult,
                            savedExpr.resultRegister,
                            blockIdx
                        );
                        
                        // Mark instruction index for removal
                        indicesToRemove.push_back(instIdx);
                        modified = true;
                        replacementCount_++;
                        
                        recordModification();
                    }
                }
            } else {
                // Save expression for future use
                if (inst.result.has_value()) {
                    std::string result = inst.result->name;
                    if (!result.empty()) {
                        expressionTable_[*exprKey] = ExpressionInfo(
                            result,
                            &inst,
                            blockIdx
                        );
                    }
                }
            }
        }
        
        // Remove marked instructions (in reverse order to maintain indices)
        std::sort(indicesToRemove.rbegin(), indicesToRemove.rend());
        for (size_t idx : indicesToRemove) {
            instructions.erase(instructions.begin() + idx);
        }
    }
    
    if (modified) {
        debug("CSE: Eliminated " + std::to_string(replacementCount_) + 
              " redundant expressions");
    }
    
    return modified;
}

std::unique_ptr<ExpressionKey> CSEPass::analyzeExpression(
    const SIR::SIRInstruction* inst
) {
    if (!inst) {
        return nullptr;
    }
    
    SIR::SIROpcode opcode = inst->opcode;
    
    // Only arithmetic and logical operations are eliminable
    switch (opcode) {
        // Arithmetic operations
        case SIR::SIROpcode::ADD_I64:
        case SIR::SIROpcode::ADD_F64:
        case SIR::SIROpcode::SUB_I64:
        case SIR::SIROpcode::SUB_F64:
        case SIR::SIROpcode::MUL_I64:
        case SIR::SIROpcode::MUL_F64:
        case SIR::SIROpcode::DIV_I64:
        case SIR::SIROpcode::DIV_F64:
        case SIR::SIROpcode::MOD_I64:
        
        // Bitwise operations
        case SIR::SIROpcode::AND:
        case SIR::SIROpcode::OR:
        case SIR::SIROpcode::XOR:
        case SIR::SIROpcode::SHL:
        case SIR::SIROpcode::SHR:
        case SIR::SIROpcode::SAR:
        
        // Comparisons
        case SIR::SIROpcode::EQ:
        case SIR::SIROpcode::NE:
        case SIR::SIROpcode::LT:
        case SIR::SIROpcode::LE:
        case SIR::SIROpcode::GT:
        case SIR::SIROpcode::GE:
            break;
            
        default:
            return nullptr;
    }
    
    // Collect operand names from inst->operands (vector<SIROperand>)
    // FIX: For constant operands, include the actual value in the key,
    // not just the name (which is empty for ConstantI64/F64/Bool).
    // Without this, all comparisons like GE %x, 3 and GE %x, 2
    // would be treated as identical expressions.
    std::vector<std::string> operandNames;
    for (const auto& operand : inst->operands) {
        if (operand.type == SIR::SIROperandType::CONSTANT) {
            switch (operand.dataType) {
                case SIR::SIRType::I64:
                    operandNames.push_back("$ci64_" + std::to_string(operand.intValue));
                    break;
                case SIR::SIRType::F64:
                    operandNames.push_back("$cf64_" + std::to_string(operand.floatValue));
                    break;
                case SIR::SIRType::BOOL:
                    operandNames.push_back(std::string("$cbool_") + (operand.boolValue ? "1" : "0"));
                    break;
                case SIR::SIRType::STRING:
                    operandNames.push_back("$cstr_" + operand.name);
                    break;
                default:
                    operandNames.push_back("$c_" + operand.name);
                    break;
            }
        } else {
            operandNames.push_back(operand.name);
        }
    }
    
    if (operandNames.empty()) {
        return nullptr;
    }
    
    return std::make_unique<ExpressionKey>(opcode, operandNames);
}

bool CSEPass::isEliminable(const SIR::SIRInstruction* inst) {
    if (!inst) {
        return false;
    }
    
    // Check for result (inst->result is std::optional<SIROperand>)
    if (!inst->result.has_value()) {
        return false;
    }
    
    // Check for no side effects
    if (hasSideEffects(inst->opcode)) {
        return false;
    }
    
    return true;
}

bool CSEPass::hasSideEffects(SIR::SIROpcode opcode) {
    switch (opcode) {
        // Memory operations
        case SIR::SIROpcode::STORE:
        case SIR::SIROpcode::ALLOC:
        case SIR::SIROpcode::ALLOC_HEAP:
        case SIR::SIROpcode::FREE:
        case SIR::SIROpcode::MEMCPY:
        case SIR::SIROpcode::MEMSET:
        
        // Function calls
        case SIR::SIROpcode::CALL:
        case SIR::SIROpcode::CALL_INDIRECT:
        
        // Control flow
        case SIR::SIROpcode::BR:
        case SIR::SIROpcode::BR_COND:
        case SIR::SIROpcode::RET:
        case SIR::SIROpcode::RET_VOID:
        
        // Built-in functions
        case SIR::SIROpcode::BUILTIN_PRINT:
        case SIR::SIROpcode::BUILTIN_READ:
            return true;
            
        default:
            return false;
    }
}

void CSEPass::replaceRegisterUses(
    SIR::SIRFunction* function,
    const std::string& oldReg,
    const std::string& newReg,
    int startBlock
) {
    if (!function || oldReg == newReg) {
        return;
    }
    
    auto& blocks = function->getBasicBlocks();
    
    // Only replace within the SAME basic block to avoid dominance issues.
    // Cross-block replacement can cause "Instruction does not dominate all uses!"
    if (startBlock >= 0 && startBlock < static_cast<int>(blocks.size())) {
        auto& block = blocks[startBlock];
        if (!block) return;
        
        for (auto& inst : block->instructions) {
            for (auto& operand : inst.operands) {
                if (operand.name == oldReg) {
                    operand.name = newReg;
                }
            }
        }
    }
}

void CSEPass::clearTables() {
    expressionTable_.clear();
    replacementCount_ = 0;
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
