/**
 * @file register_coalescing_pass.h
 * @brief تمرير دمج السجلات
 * @brief Register Coalescing Pass
 * 
 * @details
 * (AR) يدمج السجلات التي لا تتداخل في حياتها لتقليل عدد السجلات المستخدمة
 *      وتحسين كفاءة تخصيص السجلات. يُستخدم في مستوى التحسين O3.
 * 
 * (EN) Merges registers with non-overlapping lifetimes to reduce register count
 *      and improve register allocation efficiency. Used in O3 optimization.
 * 
 * @example
 * Before:
 *   %a = load %x
 *   %b = add %a, 10   // %a dies here
 *   %c = mul %b, 2    // %b dies here
 * 
 * After:
 *   %a = load %x
 *   %a = add %a, 10   // Reuse %a (was %b)
 *   %a = mul %a, 2    // Reuse %a (was %c)
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#pragma once

#include "pass.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief نطاق حياة سجل
 * @brief Register Live Range
 */
struct LiveRange {
    std::string registerName;     ///< اسم السجل / Register name
    int startBlock;               ///< كتلة البداية / Start block
    int endBlock;                 ///< كتلة النهاية / End block
    int startInstruction;         ///< تعليمة البداية / Start instruction
    int endInstruction;           ///< تعليمة النهاية / End instruction
    
    LiveRange() 
        : startBlock(-1), endBlock(-1), 
          startInstruction(-1), endInstruction(-1) {}
    
    LiveRange(const std::string& name, int sb, int si, int eb, int ei)
        : registerName(name), startBlock(sb), startInstruction(si),
          endBlock(eb), endInstruction(ei) {}
    
    /**
     * @brief التحقق من التداخل مع نطاق آخر
     * @brief Check if overlaps with another range
     */
    bool overlapsWith(const LiveRange& other) const {
        // (AR) إذا كانا في كتل مختلفة تماماً
        // (EN) If in completely different blocks
        if (endBlock < other.startBlock || startBlock > other.endBlock) {
            return false;
        }
        
        // (AR) إذا كانا في نفس الكتلة
        // (EN) If in same block
        if (startBlock == other.startBlock && endBlock == other.endBlock) {
            return !(endInstruction < other.startInstruction || 
                    startInstruction > other.endInstruction);
        }
        
        return true; // تداخل محتمل / Potential overlap
    }
};

/**
 * @brief رسم بياني للتداخل
 * @brief Interference Graph
 * 
 * @details
 * (AR) يمثل العلاقات بين السجلات. إذا تداخلت حياة سجلين،
 *      يتم وضع حافة بينهما في الرسم البياني.
 * 
 * (EN) Represents relationships between registers. If two registers
 *      have overlapping lifetimes, an edge is placed between them.
 */
class InterferenceGraph {
public:
    /**
     * @brief إضافة سجل
     * @brief Add register
     */
    void addRegister(const std::string& reg) {
        if (adjacencyList_.find(reg) == adjacencyList_.end()) {
            adjacencyList_[reg] = std::unordered_set<std::string>();
        }
    }
    
    /**
     * @brief إضافة حافة تداخل
     * @brief Add interference edge
     */
    void addInterference(const std::string& reg1, const std::string& reg2) {
        addRegister(reg1);
        addRegister(reg2);
        adjacencyList_[reg1].insert(reg2);
        adjacencyList_[reg2].insert(reg1);
    }
    
    /**
     * @brief التحقق من وجود تداخل
     * @brief Check if interference exists
     */
    bool interferes(const std::string& reg1, const std::string& reg2) const {
        auto it = adjacencyList_.find(reg1);
        if (it == adjacencyList_.end()) {
            return false;
        }
        return it->second.find(reg2) != it->second.end();
    }
    
    /**
     * @brief الحصول على الجيران
     * @brief Get neighbors
     */
    const std::unordered_set<std::string>& getNeighbors(const std::string& reg) const {
        static const std::unordered_set<std::string> empty;
        auto it = adjacencyList_.find(reg);
        return (it != adjacencyList_.end()) ? it->second : empty;
    }
    
    /**
     * @brief الحصول على جميع السجلات
     * @brief Get all registers
     */
    std::vector<std::string> getAllRegisters() const {
        std::vector<std::string> result;
        for (const auto& pair : adjacencyList_) {
            result.push_back(pair.first);
        }
        return result;
    }
    
    /**
     * @brief دمج سجلين
     * @brief Merge two registers
     */
    void mergeRegisters(const std::string& dest, const std::string& src) {
        if (dest == src) return;
        
        // (AR) نقل جميع حواف src إلى dest
        // (EN) Move all edges from src to dest
        auto srcIt = adjacencyList_.find(src);
        if (srcIt != adjacencyList_.end()) {
            for (const auto& neighbor : srcIt->second) {
                if (neighbor != dest) {
                    addInterference(dest, neighbor);
                }
                // إزالة src من جيران neighbor
                auto neighborIt = adjacencyList_.find(neighbor);
                if (neighborIt != adjacencyList_.end()) {
                    neighborIt->second.erase(src);
                }
            }
            adjacencyList_.erase(srcIt);
        }
    }
    
    void clear() {
        adjacencyList_.clear();
    }

private:
    std::unordered_map<std::string, std::unordered_set<std::string>> adjacencyList_;
};

/**
 * @brief تمرير دمج السجلات
 * @brief Register Coalescing Pass
 * 
 * @details
 * (AR) يحلل نطاقات حياة السجلات ويبني رسماً بيانياً للتداخل،
 *      ثم يدمج السجلات التي لا تتداخل لتقليل الضغط على السجلات.
 * 
 * (EN) Analyzes register live ranges and builds interference graph,
 *      then merges non-interfering registers to reduce register pressure.
 * 
 * @algorithm
 * 1. Compute live ranges for all registers
 * 2. Build interference graph
 * 3. Find non-interfering register pairs
 * 4. Merge registers (rename all uses)
 * 5. Update interference graph
 * 6. Repeat until no more merges possible
 * 
 * @complexity
 * Time: O(n²) where n is number of registers
 * Space: O(n² + m) where m is number of instructions
 */
class RegisterCoalescingPass : public OptimizationPass {
public:
    /**
     * @brief Constructor / البناء
     */
    RegisterCoalescingPass();
    
    /**
     * @brief تشغيل التمرير على دالة
     * @brief Run pass on function
     */
    bool runOnFunction(SIR::SIRFunction* function) override;

private:
    /**
     * @brief حساب نطاقات الحياة
     * @brief Compute live ranges
     */
    void computeLiveRanges(SIR::SIRFunction* function);
    
    /**
     * @brief بناء رسم التداخل
     * @brief Build interference graph
     */
    void buildInterferenceGraph();
    
    /**
     * @brief العثور على أفضل زوج للدمج
     * @brief Find best pair to coalesce
     * 
     * @return زوج من السجلات، أو فارغ إذا لم يوجد / Pair of registers, or empty
     */
    std::pair<std::string, std::string> findCoalescePair();
    
    /**
     * @brief دمج سجلين
     * @brief Coalesce two registers
     * 
     * @param dest السجل الوجهة / Destination register
     * @param src السجل المصدر / Source register
     * @return true إذا تم الدمج / if coalesced
     */
    bool coalesceRegisters(SIR::SIRFunction* function,
                          const std::string& dest,
                          const std::string& src);
    
    /**
     * @brief استبدال جميع استخدامات سجل
     * @brief Replace all uses of register
     */
    int replaceAllUses(SIR::SIRFunction* function,
                      const std::string& oldReg,
                      const std::string& newReg);
    
    /**
     * @brief الحصول على نطاق حياة سجل
     * @brief Get live range of register
     */
    const LiveRange* getLiveRange(const std::string& reg) const;
    
    /**
     * @brief التحقق من أن الدمج آمن
     * @brief Check if coalescing is safe
     */
    bool isSafeToCoalesce(const std::string& reg1, const std::string& reg2) const;
    
    /**
     * @brief مسح البيانات
     * @brief Clear data
     */
    void clearData();

private:
    /// نطاقات الحياة / Live ranges
    std::unordered_map<std::string, LiveRange> liveRanges_;
    
    /// رسم التداخل / Interference graph
    InterferenceGraph interferenceGraph_;
    
    /// السجلات المدمجة / Coalesced registers
    std::unordered_map<std::string, std::string> coalescedMap_;
    
    /// عداد الدمج / Coalesce counter
    int coalesceCount_;
};

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
