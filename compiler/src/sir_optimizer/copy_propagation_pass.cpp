/**
 * @file copy_propagation_pass.cpp
 * @brief تطبيق تمرير نشر النسخ
 * @brief Copy Propagation Pass Implementation
 * 
 * @details
 * (AR) ينفذ خوارزمية نشر النسخ للقضاء على عمليات النسخ غير الضرورية.
 * (EN) Implements copy propagation algorithm to eliminate unnecessary copies.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "sir_optimizer/copy_propagation_pass.h"
#include "frontend/sir_module.h"
#include "frontend/sir_instruction.h"
#include <iostream>
#include <algorithm>

namespace Sad {
namespace Compiler {
namespace Optimizer {

// ======================================================================
// Constructor
// ======================================================================

CopyPropagationPass::CopyPropagationPass()
    : OptimizationPass("CopyPropagation", PassType::TRANSFORMATION),
      propagationCount_(0),
      removalCount_(0) {
}

// ======================================================================
// Main Entry Point
// ======================================================================

bool CopyPropagationPass::runOnFunction(SIR::SIRFunction* function) {
    if (!function) {
        return false;
    }
    
    clearTables();
    bool modified = false;
    bool changed = true;
    int iteration = 0;
    const int MAX_ITERATIONS = 10; // لتجنب حلقات لا نهائية / Avoid infinite loops
    
    // (AR) كرر حتى لا يتم العثور على المزيد من النسخ
    // (EN) Iterate until no more copies can be found
    while (changed && iteration < MAX_ITERATIONS) {
        changed = false;
        iteration++;
        
        // (AR) معالجة كل كتلة أساسية
        // (EN) Process each basic block
        for (size_t i = 0; i < function->basicBlocks.size(); ++i) {
            auto& block = function->basicBlocks[i];
            if (processBlock(block.get(), static_cast<int>(i))) {
                changed = true;
                modified = true;
            }
        }
    }
    
    if (modified) {
        std::cout << "[CopyPropagation] Propagated " << propagationCount_ 
                  << " copies, removed " << removalCount_ << " instructions\n";
    }
    
    return modified;
}

// ======================================================================
// Block Processing
// ======================================================================

bool CopyPropagationPass::processBlock(SIR::SIRBasicBlock* block, int blockIndex) {
    if (!block) {
        return false;
    }
    
    bool modified = false;
    copies_.clear();
    copyMap_.clear();
    
    // (AR) المرحلة 1: اكتشاف عمليات النسخ
    // (EN) Phase 1: Detect copy operations
    for (auto& inst : block->instructions) {
        auto copyInfo = detectCopy(&inst, blockIndex);
        if (copyInfo) {
            copies_.push_back(*copyInfo);
            copyMap_[copyInfo->destination] = copyInfo->source;
        }
    }
    
    if (copies_.empty()) {
        return false;
    }
    
    // (AR) المرحلة 2: نشر النسخ
    // (EN) Phase 2: Propagate copies
    for (const auto& copy : copies_) {
        // (AR) حل سلسلة النسخ للحصول على السجل الأصلي
        // (EN) Resolve copy chain to get original register
        std::string originalSource = resolveCopyChain(copy.source);
        
        // (AR) استبدال جميع استخدامات الوجهة بالمصدر الأصلي
        // (EN) Replace all uses of destination with original source
        int replacements = replaceRegisterUses(
            block->parent,
            copy.destination,
            originalSource,
            blockIndex
        );
        
        if (replacements > 0) {
            propagationCount_ += replacements;
            modified = true;
            
            // (AR) إذا لم يعد السجل الوجهة مستخدماً، احذف التعليمة
            // (EN) If destination register is no longer used, remove instruction
            if (!isRegisterUsed(block->parent, copy.destination, blockIndex)) {
                if (removeCopyInstruction(block, copy.instruction)) {
                    removalCount_++;
                    recordModification();
                }
            }
        }
    }
    
    return modified;
}

// ======================================================================
// Copy Detection
// ======================================================================

std::unique_ptr<CopyInfo> CopyPropagationPass::detectCopy(
    SIR::SIRInstruction* inst, int blockIndex) {
    
    if (!inst || !isSimpleCopy(inst)) {
        return nullptr;
    }
    
    // (AR) تحقق من وجود نتيجة ومعامل واحد على الأقل
    // (EN) Check for result and at least one operand
    if (!inst->result.has_value() || inst->operands.empty()) {
        return nullptr;
    }
    
    // (AR) استخراج السجل الوجهة
    // (EN) Extract destination register
    std::string destination;
    if (inst->result->type == SIR::SIROperandType::REGISTER) {
        destination = inst->result->name;
    } else {
        return nullptr;
    }
    
    // (AR) استخراج السجل المصدر
    // (EN) Extract source register
    std::string source;
    const auto& firstOperand = inst->operands[0];
    
    if (firstOperand.type == SIR::SIROperandType::REGISTER) {
        source = firstOperand.name;
    } else {
        return nullptr;
    }
    
    // (AR) تجنب النسخ الذاتي (%a = %a)
    // (EN) Avoid self-copy (%a = %a)
    if (source == destination) {
        return nullptr;
    }
    
    return std::make_unique<CopyInfo>(source, destination, inst, blockIndex);
}

bool CopyPropagationPass::isSimpleCopy(const SIR::SIRInstruction* inst) const {
    if (!inst) {
        return false;
    }
    
    using SIR::SIROpcode;
    
    // (AR) عمليات النسخ المباشرة
    // (EN) Direct copy operations
    if (inst->opcode == SIROpcode::MOVE) {
        return true;
    }
    
    // (AR) عمليات حسابية تعادل النسخ
    // (EN) Arithmetic operations equivalent to copy
    if (inst->operands.size() >= 2) {
        const auto& secondOperand = inst->operands[1];
        
        // %dest = ADD %src, 0 → copy
        if ((inst->opcode == SIROpcode::ADD_I64 || inst->opcode == SIROpcode::ADD_F64) &&
            secondOperand.type == SIR::SIROperandType::CONSTANT) {
            if (secondOperand.name == "0" || secondOperand.name == "0.0") {
                return true;
            }
        }
        
        // %dest = MUL %src, 1 → copy
        if ((inst->opcode == SIROpcode::MUL_I64 || inst->opcode == SIROpcode::MUL_F64) &&
            secondOperand.type == SIR::SIROperandType::CONSTANT) {
            if (secondOperand.name == "1" || secondOperand.name == "1.0") {
                return true;
            }
        }
        
        // %dest = OR %src, 0 → copy
        if (inst->opcode == SIROpcode::OR &&
            secondOperand.type == SIR::SIROperandType::CONSTANT) {
            if (secondOperand.name == "0") {
                return true;
            }
        }
    }
    
    return false;
}

// ======================================================================
// Copy Chain Resolution
// ======================================================================

std::string CopyPropagationPass::resolveCopyChain(const std::string& reg) {
    std::unordered_set<std::string> visited; // للكشف عن الدورات / For cycle detection
    std::string current = reg;
    
    // (AR) تتبع السلسلة حتى نصل إلى سجل غير منسوخ
    // (EN) Follow chain until we reach non-copied register
    while (copyMap_.find(current) != copyMap_.end()) {
        // (AR) كشف الدورة
        // (EN) Detect cycle
        if (visited.find(current) != visited.end()) {
            std::cerr << "[CopyPropagation] Warning: Detected copy cycle starting at " 
                      << current << "\n";
            return reg; // إرجاع السجل الأصلي / Return original register
        }
        
        visited.insert(current);
        current = copyMap_[current];
    }
    
    return current;
}

// ======================================================================
// Register Use Replacement
// ======================================================================

int CopyPropagationPass::replaceRegisterUses(
    SIR::SIRFunction* function,
    const std::string& oldReg,
    const std::string& newReg,
    int startBlock) {
    
    if (!function || oldReg == newReg) {
        return 0;
    }
    
    int replacements = 0;
    
    // (AR) استبدال في الكتل اللاحقة
    // (EN) Replace in subsequent blocks
    for (size_t i = startBlock; i < function->basicBlocks.size(); ++i) {
        auto& block = function->basicBlocks[i];
        
        for (auto& inst : block->instructions) {
            // (AR) لا تستبدل في النتيجة، فقط في المعاملات
            // (EN) Don't replace in result, only in operands
            for (auto& operand : inst.operands) {
                if (operand.type == SIR::SIROperandType::REGISTER &&
                    operand.name == oldReg) {
                    operand.name = newReg;
                    replacements++;
                }
            }
        }
    }
    
    return replacements;
}

// ======================================================================
// Register Usage Check
// ======================================================================

bool CopyPropagationPass::isRegisterUsed(
    SIR::SIRFunction* function,
    const std::string& reg,
    int startBlock) const {
    
    if (!function) {
        return false;
    }
    
    // (AR) تحقق من الكتل اللاحقة
    // (EN) Check subsequent blocks
    for (size_t i = startBlock; i < function->basicBlocks.size(); ++i) {
        const auto& block = function->basicBlocks[i];
        
        for (const auto& inst : block->instructions) {
            // (AR) تحقق من الاستخدام في المعاملات
            // (EN) Check usage in operands
            for (const auto& operand : inst.operands) {
                if (operand.type == SIR::SIROperandType::REGISTER &&
                    operand.name == reg) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

// ======================================================================
// Copy Instruction Removal
// ======================================================================

bool CopyPropagationPass::removeCopyInstruction(
    SIR::SIRBasicBlock* block,
    SIR::SIRInstruction* inst) {
    
    if (!block || !inst) {
        return false;
    }
    
    // (AR) تجنب حذف التعليمة مرتين
    // (EN) Avoid removing instruction twice
    if (removedInstructions_.find(inst) != removedInstructions_.end()) {
        return false;
    }
    
    // (AR) ابحث عن التعليمة واحذفها
    // (EN) Find and remove instruction
    auto& instructions = block->instructions;
    auto it = std::find_if(instructions.begin(), instructions.end(),
        [inst](const SIR::SIRInstruction& i) {
            return &i == inst;
        });
    
    if (it != instructions.end()) {
        removedInstructions_.insert(inst);
        instructions.erase(it);
        return true;
    }
    
    return false;
}

// ======================================================================
// Cleanup
// ======================================================================

void CopyPropagationPass::clearTables() {
    copyMap_.clear();
    copies_.clear();
    removedInstructions_.clear();
    propagationCount_ = 0;
    removalCount_ = 0;
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
