/**
 * @file cse_pass.cpp
 * @brief تطبيق تمرير حذف التعابير الفرعية المشتركة
 * @brief Common Subexpression Elimination Pass Implementation
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 */

#include "cse_pass.h"
#include <algorithm>
#include <sstream>

namespace Sad {
namespace Compiler {
namespace Optimizer {

// ============================================================================
// ExpressionKey Implementation
// ============================================================================

size_t ExpressionKey::hash() const {
    // دمج hash للـ opcode مع hash للمعاملات
    // Combine hash of opcode with hash of operands
    size_t h = std::hash<int>{}(static_cast<int>(opcode));
    
    for (const auto& operand : operands) {
        // XOR مع hash كل معامل
        // XOR with hash of each operand
        h ^= std::hash<std::string>{}(operand) + 0x9e3779b9 + (h << 6) + (h >> 2);
    }
    
    return h;
}

// ============================================================================
// CSEPass Implementation
// ============================================================================

CSEPass::CSEPass()
    : OptimizationPass("CSE")
    , replacementCount_(0)
{
}

CSEPass::~CSEPass() = default;

bool CSEPass::runOnFunction(SIR::SIRFunction* function) {
    if (!function) {
        return false;
    }
    
    clearTables();
    bool modified = false;
    replacementCount_ = 0;
    
    // تكرار على كل الكتل الأساسية / Iterate over all basic blocks
    auto& blocks = function->getBasicBlocks();
    
    for (size_t blockIdx = 0; blockIdx < blocks.size(); ++blockIdx) {
        auto& block = blocks[blockIdx];
        if (!block) continue;
        
        // Source: SIRBasicBlock::instructions is PUBLIC member at sir_instruction.h:356
        auto& instructions = block->instructions;
        
        // تكرار على كل التعليمات في الكتلة / Iterate over all instructions in block
        for (size_t instIdx = 0; instIdx < instructions.size(); ++instIdx) {
            auto& inst = instructions[instIdx];
            if (!inst || removedInstructions_.count(inst.get()) > 0) {
                continue;
            }
            
            // فحص إذا كانت التعليمة قابلة للحذف / Check if instruction is eliminable
            if (!isEliminable(inst.get())) {
                continue;
            }
            
            // تحليل التعبير / Analyze expression
            auto exprKey = analyzeExpression(inst.get());
            if (!exprKey) {
                continue;
            }
            
            // البحث عن تعبير مشابه / Look for similar expression
            auto it = expressionTable_.find(*exprKey);
            
            if (it != expressionTable_.end()) {
                // وجدنا تعبير مكرر! / Found duplicate expression!
                const auto& savedExpr = it->second;
                
                // التأكد من أن التعبير المحفوظ لا يزال صالحاً
                // Ensure saved expression is still valid
                if (removedInstructions_.count(savedExpr.instruction) == 0) {
                    // استبدال استخدامات السجل الحالي بالسجل المحفوظ
                    // Replace uses of current register with saved register
                    std::string currentResult = inst->getResult();
                    if (!currentResult.empty()) {
                        replaceRegisterUses(
                            function,
                            currentResult,
                            savedExpr.resultRegister,
                            blockIdx
                        );
                        
                        // وضع علامة على التعليمة للحذف
                        // Mark instruction for removal
                        removedInstructions_.insert(inst.get());
                        modified = true;
                        replacementCount_++;
                        
                        recordModification();
                    }
                }
            } else {
                // حفظ التعبير للاستخدام المستقبلي
                // Save expression for future use
                std::string result = inst->getResult();
                if (!result.empty()) {
                    expressionTable_[*exprKey] = ExpressionInfo(
                        result,
                        inst.get(),
                        blockIdx
                    );
                }
            }
        }
    }
    
    // حذف التعليمات المحددة للحذف / Remove marked instructions
    if (!removedInstructions_.empty()) {
        for (auto& block : blocks) {
            if (!block) continue;
            
            // Source: SIRBasicBlock::instructions is PUBLIC member at sir_instruction.h:356
            auto& instructions = block->instructions;
            instructions.erase(
                std::remove_if(
                    instructions.begin(),
                    instructions.end(),
                    [this](const std::unique_ptr<SIR::SIRInstruction>& inst) {
                        return removedInstructions_.count(inst.get()) > 0;
                    }
                ),
                instructions.end()
            );
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
    
    // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
    SIR::SIROpcode opcode = inst->opcode;
    
    // فقط العمليات الحسابية والمنطقية قابلة للحذف
    // Only arithmetic and logical operations are eliminable
    switch (opcode) {
        // العمليات الحسابية / Arithmetic operations
        case SIR::SIROpcode::ADD_I64:
        case SIR::SIROpcode::ADD_F64:
        case SIR::SIROpcode::SUB_I64:
        case SIR::SIROpcode::SUB_F64:
        case SIR::SIROpcode::MUL_I64:
        case SIR::SIROpcode::MUL_F64:
        case SIR::SIROpcode::DIV_I64:
        case SIR::SIROpcode::DIV_F64:
        case SIR::SIROpcode::MOD_I64:
        
        // العمليات الثنائية / Bitwise operations
        case SIR::SIROpcode::AND:
        case SIR::SIROpcode::OR:
        case SIR::SIROpcode::XOR:
        case SIR::SIROpcode::SHL:
        case SIR::SIROpcode::SHR:
        case SIR::SIROpcode::SAR:
        
        // المقارنات / Comparisons
        case SIR::SIROpcode::EQ:
        case SIR::SIROpcode::NE:
        case SIR::SIROpcode::LT:
        case SIR::SIROpcode::LE:
        case SIR::SIROpcode::GT:
        case SIR::SIROpcode::GE:
            break;
            
        default:
            // العمليات الأخرى لا تُحذف / Other operations not eliminated
            return nullptr;
    }
    
    // جمع المعاملات / Collect operands
    std::vector<std::string> operands;
    for (const auto& operand : inst->getOperands()) {
        operands.push_back(operand.name);
    }
    
    if (operands.empty()) {
        return nullptr;
    }
    
    return std::make_unique<ExpressionKey>(opcode, operands);
}

bool CSEPass::isEliminable(const SIR::SIRInstruction* inst) {
    if (!inst) {
        return false;
    }
    
    // التحقق من وجود نتيجة / Check for result
    // Source: SIRInstruction::result is PUBLIC member at sir_instruction.h:61
    if (!inst->result.has_value()) {
        return false;
    }
    
    // التحقق من عدم وجود تأثيرات جانبية / Check for no side effects
    // Source: SIRInstruction::opcode is PUBLIC member at sir_instruction.h:60
    if (hasSideEffects(inst->opcode)) {
        return false;
    }
    
    return true;
}

bool CSEPass::hasSideEffects(SIR::SIROpcode opcode) {
    // العمليات التي لها تأثيرات جانبية / Operations with side effects
    switch (opcode) {
        // عمليات الذاكرة / Memory operations
        case SIR::SIROpcode::STORE:
        case SIR::SIROpcode::ALLOC:
        case SIR::SIROpcode::ALLOC_HEAP:
        case SIR::SIROpcode::FREE:
        case SIR::SIROpcode::MEMCPY:
        case SIR::SIROpcode::MEMSET:
        
        // استدعاء الدوال / Function calls
        case SIR::SIROpcode::CALL:
        case SIR::SIROpcode::CALL_INDIRECT:
        
        // التحكم في التدفق / Control flow
        case SIR::SIROpcode::BR:
        case SIR::SIROpcode::BR_COND:
        case SIR::SIROpcode::RET:
        case SIR::SIROpcode::RET_VOID:
        
        // الدوال المضمنة / Built-in functions
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
    
    // استبدال في الكتل اللاحقة / Replace in subsequent blocks
    for (size_t blockIdx = startBlock; blockIdx < blocks.size(); ++blockIdx) {
        auto& block = blocks[blockIdx];
        if (!block) continue;
        
        // Source: SIRBasicBlock::instructions is PUBLIC member at sir_instruction.h:356
        auto& instructions = block->instructions;
        for (auto& inst : instructions) {
            if (removedInstructions_.count(&inst) > 0) {
                continue;
            }
            
            // استبدال في المعاملات / Replace in operands
            for (auto& operand : inst->getOperands()) {
                if (operand.name == oldReg) {
                    operand.name = newReg;
                }
            }
        }
    }
}

void CSEPass::clearTables() {
    expressionTable_.clear();
    removedInstructions_.clear();
    replacementCount_ = 0;
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
