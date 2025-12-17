/**
 * @file register_coalescing_pass.cpp
 * @brief تطبيق تمرير دمج السجلات
 * @brief Register Coalescing Pass Implementation
 * 
 * @details
 * (AR) ينفذ خوارزمية دمج السجلات لتقليل ضغط السجلات.
 * (EN) Implements register coalescing algorithm to reduce register pressure.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/register_coalescing_pass.h"
#include "../../frontend/include/sir_module.h"
#include "../../frontend/include/sir_instruction.h"
#include <iostream>
#include <algorithm>

namespace Sad {
namespace Compiler {
namespace Optimizer {

// ======================================================================
// Constructor
// ======================================================================

RegisterCoalescingPass::RegisterCoalescingPass()
    : OptimizationPass("RegisterCoalescing", PassType::TRANSFORMATION),
      coalesceCount_(0) {
}

// ======================================================================
// Main Entry Point
// ======================================================================

bool RegisterCoalescingPass::runOnFunction(SIR::SIRFunction* function) {
    if (!function) {
        return false;
    }
    
    clearData();
    bool modified = false;
    int iteration = 0;
    const int MAX_ITERATIONS = 5; // للأمان / For safety
    
    // (AR) كرر حتى لا يمكن دمج المزيد من السجلات
    // (EN) Iterate until no more registers can be coalesced
    while (iteration < MAX_ITERATIONS) {
        iteration++;
        
        // (AR) المرحلة 1: حساب نطاقات الحياة
        // (EN) Phase 1: Compute live ranges
        computeLiveRanges(function);
        
        if (liveRanges_.empty()) {
            break;
        }
        
        // (AR) المرحلة 2: بناء رسم التداخل
        // (EN) Phase 2: Build interference graph
        buildInterferenceGraph();
        
        // (AR) المرحلة 3: العثور على زوج للدمج
        // (EN) Phase 3: Find pair to coalesce
        auto pair = findCoalescePair();
        if (pair.first.empty()) {
            break; // لا توجد المزيد من الأزواج / No more pairs
        }
        
        // (AR) المرحلة 4: دمج السجلات
        // (EN) Phase 4: Coalesce registers
        if (coalesceRegisters(function, pair.first, pair.second)) {
            modified = true;
            coalesceCount_++;
            recordModification();
        } else {
            break;
        }
    }
    
    if (modified) {
        std::cout << "[RegisterCoalescing] Coalesced " << coalesceCount_ 
                  << " register pairs\n";
    }
    
    return modified;
}

// ======================================================================
// Live Range Computation
// ======================================================================

void RegisterCoalescingPass::computeLiveRanges(SIR::SIRFunction* function) {
    liveRanges_.clear();
    
    // (AR) مرور على جميع الكتل والتعليمات
    // (EN) Pass over all blocks and instructions
    for (size_t blockIdx = 0; blockIdx < function->basicBlocks.size(); ++blockIdx) {
        const auto& block = function->basicBlocks[blockIdx];
        
        for (size_t instIdx = 0; instIdx < block->instructions.size(); ++instIdx) {
            const auto& inst = block->instructions[instIdx];
            
            // (AR) تسجيل تعريف السجل (result)
            // (EN) Record register definition (result)
            if (inst->result.has_value() && 
                inst->result->kind == SIR::OperandKind::REGISTER) {
                
                const std::string& regName = inst->result->value;
                
                if (liveRanges_.find(regName) == liveRanges_.end()) {
                    // تعريف جديد / New definition
                    liveRanges_[regName] = LiveRange(
                        regName,
                        static_cast<int>(blockIdx),
                        static_cast<int>(instIdx),
                        static_cast<int>(blockIdx),
                        static_cast<int>(instIdx)
                    );
                } else {
                    // تحديث البداية / Update start
                    auto& range = liveRanges_[regName];
                    if (static_cast<int>(blockIdx) < range.startBlock ||
                        (static_cast<int>(blockIdx) == range.startBlock && 
                         static_cast<int>(instIdx) < range.startInstruction)) {
                        range.startBlock = static_cast<int>(blockIdx);
                        range.startInstruction = static_cast<int>(instIdx);
                    }
                }
            }
            
            // (AR) تسجيل استخدام السجل (operands)
            // (EN) Record register use (operands)
            for (const auto& operand : inst->operands) {
                if (operand.kind == SIR::OperandKind::REGISTER) {
                    const std::string& regName = operand.value;
                    
                    if (liveRanges_.find(regName) != liveRanges_.end()) {
                        // توسيع نطاق الحياة / Extend live range
                        auto& range = liveRanges_[regName];
                        if (static_cast<int>(blockIdx) > range.endBlock ||
                            (static_cast<int>(blockIdx) == range.endBlock && 
                             static_cast<int>(instIdx) > range.endInstruction)) {
                            range.endBlock = static_cast<int>(blockIdx);
                            range.endInstruction = static_cast<int>(instIdx);
                        }
                    }
                }
            }
        }
    }
}

// ======================================================================
// Interference Graph Construction
// ======================================================================

void RegisterCoalescingPass::buildInterferenceGraph() {
    interferenceGraph_.clear();
    
    // (AR) إضافة جميع السجلات
    // (EN) Add all registers
    for (const auto& pair : liveRanges_) {
        interferenceGraph_.addRegister(pair.first);
    }
    
    // (AR) إضافة حواف التداخل
    // (EN) Add interference edges
    std::vector<std::string> registers;
    for (const auto& pair : liveRanges_) {
        registers.push_back(pair.first);
    }
    
    for (size_t i = 0; i < registers.size(); ++i) {
        for (size_t j = i + 1; j < registers.size(); ++j) {
            const auto& range1 = liveRanges_[registers[i]];
            const auto& range2 = liveRanges_[registers[j]];
            
            if (range1.overlapsWith(range2)) {
                interferenceGraph_.addInterference(registers[i], registers[j]);
            }
        }
    }
}

// ======================================================================
// Find Coalesce Pair
// ======================================================================

std::pair<std::string, std::string> RegisterCoalescingPass::findCoalescePair() {
    auto allRegisters = interferenceGraph_.getAllRegisters();
    
    // (AR) البحث عن أفضل زوج (أقل تداخل)
    // (EN) Find best pair (least interference)
    std::string bestReg1, bestReg2;
    int minNeighbors = INT_MAX;
    
    for (size_t i = 0; i < allRegisters.size(); ++i) {
        for (size_t j = i + 1; j < allRegisters.size(); ++j) {
            const std::string& reg1 = allRegisters[i];
            const std::string& reg2 = allRegisters[j];
            
            // (AR) تحقق من عدم التداخل وإمكانية الدمج
            // (EN) Check for non-interference and coalescability
            if (!interferenceGraph_.interferes(reg1, reg2) &&
                isSafeToCoalesce(reg1, reg2)) {
                
                // (AR) حساب عدد الجيران المجمع
                // (EN) Calculate combined neighbor count
                int neighbors = static_cast<int>(
                    interferenceGraph_.getNeighbors(reg1).size() +
                    interferenceGraph_.getNeighbors(reg2).size()
                );
                
                if (neighbors < minNeighbors) {
                    minNeighbors = neighbors;
                    bestReg1 = reg1;
                    bestReg2 = reg2;
                }
            }
        }
    }
    
    return {bestReg1, bestReg2};
}

// ======================================================================
// Register Coalescing
// ======================================================================

bool RegisterCoalescingPass::coalesceRegisters(
    SIR::SIRFunction* function,
    const std::string& dest,
    const std::string& src) {
    
    if (dest.empty() || src.empty() || dest == src) {
        return false;
    }
    
    // (AR) استبدال جميع استخدامات src بـ dest
    // (EN) Replace all uses of src with dest
    int replacements = replaceAllUses(function, src, dest);
    
    if (replacements > 0) {
        // (AR) تحديث رسم التداخل
        // (EN) Update interference graph
        interferenceGraph_.mergeRegisters(dest, src);
        
        // (AR) دمج نطاقات الحياة
        // (EN) Merge live ranges
        auto srcRange = liveRanges_.find(src);
        auto destRange = liveRanges_.find(dest);
        
        if (srcRange != liveRanges_.end() && destRange != liveRanges_.end()) {
            auto& dRange = destRange->second;
            const auto& sRange = srcRange->second;
            
            // توسيع نطاق dest / Extend dest range
            dRange.startBlock = std::min(dRange.startBlock, sRange.startBlock);
            dRange.endBlock = std::max(dRange.endBlock, sRange.endBlock);
            
            if (dRange.startBlock == sRange.startBlock) {
                dRange.startInstruction = std::min(dRange.startInstruction, 
                                                   sRange.startInstruction);
            }
            if (dRange.endBlock == sRange.endBlock) {
                dRange.endInstruction = std::max(dRange.endInstruction, 
                                                 sRange.endInstruction);
            }
            
            liveRanges_.erase(srcRange);
        }
        
        coalescedMap_[src] = dest;
        return true;
    }
    
    return false;
}

// ======================================================================
// Replace All Uses
// ======================================================================

int RegisterCoalescingPass::replaceAllUses(
    SIR::SIRFunction* function,
    const std::string& oldReg,
    const std::string& newReg) {
    
    if (!function || oldReg == newReg) {
        return 0;
    }
    
    int replacements = 0;
    
    // (AR) استبدال في جميع الكتل
    // (EN) Replace in all blocks
    for (auto& block : function->basicBlocks) {
        for (auto& inst : block->instructions) {
            // (AR) استبدال في النتيجة
            // (EN) Replace in result
            if (inst->result.has_value() &&
                inst->result->kind == SIR::OperandKind::REGISTER &&
                inst->result->value == oldReg) {
                inst->result->value = newReg;
                replacements++;
            }
            
            // (AR) استبدال في المعاملات
            // (EN) Replace in operands
            for (auto& operand : inst->operands) {
                if (operand.kind == SIR::OperandKind::REGISTER &&
                    operand.value == oldReg) {
                    operand.value = newReg;
                    replacements++;
                }
            }
        }
    }
    
    return replacements;
}

// ======================================================================
// Helper Functions
// ======================================================================

const LiveRange* RegisterCoalescingPass::getLiveRange(const std::string& reg) const {
    auto it = liveRanges_.find(reg);
    return (it != liveRanges_.end()) ? &it->second : nullptr;
}

bool RegisterCoalescingPass::isSafeToCoalesce(
    const std::string& reg1,
    const std::string& reg2) const {
    
    // (AR) الحصول على نطاقات الحياة
    // (EN) Get live ranges
    const auto* range1 = getLiveRange(reg1);
    const auto* range2 = getLiveRange(reg2);
    
    if (!range1 || !range2) {
        return false;
    }
    
    // (AR) تحقق من عدم التداخل
    // (EN) Check for non-overlap
    if (range1->overlapsWith(*range2)) {
        return false;
    }
    
    // (AR) تحقق من عدم التداخل في رسم التداخل
    // (EN) Check for non-interference in graph
    if (interferenceGraph_.interferes(reg1, reg2)) {
        return false;
    }
    
    return true;
}

void RegisterCoalescingPass::clearData() {
    liveRanges_.clear();
    interferenceGraph_.clear();
    coalescedMap_.clear();
    coalesceCount_ = 0;
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
