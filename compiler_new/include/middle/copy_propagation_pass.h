/**
 * @file copy_propagation_pass.h
 * @brief تمرير نشر النسخ
 * @brief Copy Propagation Pass
 * 
 * @details
 * (AR) يكتشف عمليات النسخ البسيطة (%a = %b) وينشر القيمة الأصلية
 *      لتقليل عدد السجلات المستخدمة وتحسين أداء الكود.
 * 
 * (EN) Detects simple copy operations (%a = %b) and propagates the original value
 *      to reduce register usage and improve code performance.
 * 
 * @example
 * Before:
 *   %a = load %x
 *   %b = %a          ← Copy operation
 *   %c = add %b, 10  ← Uses copy
 * 
 * After:
 *   %a = load %x
 *   %c = add %a, 10  ← Uses original
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#pragma once

#include "pass.h"
#include <unordered_map>
#include <unordered_set>
#include <string>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief معلومات النسخة
 * @brief Copy Information
 */
struct CopyInfo {
    std::string source;         ///< السجل المصدر / Source register
    std::string destination;    ///< السجل الوجهة / Destination register
    SIR::SIRInstruction* instruction; ///< التعليمة / Instruction
    int blockIndex;             ///< رقم الكتلة / Block index
    
    CopyInfo() : instruction(nullptr), blockIndex(-1) {}
    
    CopyInfo(const std::string& src, const std::string& dst, 
             SIR::SIRInstruction* inst, int block)
        : source(src), destination(dst), instruction(inst), blockIndex(block) {}
};

/**
 * @brief تمرير نشر النسخ
 * @brief Copy Propagation Pass
 * 
 * @details
 * (AR) يحلل التعليمات للعثور على عمليات النسخ البسيطة (%dest = %src)
 *      وينشر السجل المصدر في جميع الاستخدامات اللاحقة للسجل الوجهة.
 *      يزيل عمليات النسخ غير الضرورية بعد النشر.
 * 
 * (EN) Analyzes instructions to find simple copy operations (%dest = %src)
 *      and propagates source register to all subsequent uses of destination.
 *      Removes unnecessary copy operations after propagation.
 * 
 * @algorithm
 * 1. Scan all instructions for copy operations
 * 2. Build copy chain (if %a = %b and %b = %c, then %a = %c)
 * 3. Replace all uses of destination with source
 * 4. Remove copy instruction if destination is no longer used
 * 5. Repeat until no more copies can be eliminated
 * 
 * @complexity
 * Time: O(n) where n is number of instructions
 * Space: O(k) where k is number of registers
 * 
 * @example
 * Complex case:
 * ```
 * %a = load %x
 * %b = %a
 * %c = %b
 * %d = add %c, 10
 * ```
 * After propagation:
 * ```
 * %a = load %x
 * %d = add %a, 10
 * ```
 */
class CopyPropagationPass : public OptimizationPass {
public:
    /**
     * @brief Constructor / البناء
     */
    CopyPropagationPass();
    
    /**
     * @brief تشغيل التمرير على دالة
     * @brief Run pass on function
     * 
     * @param function الدالة المراد تحسينها / Function to optimize
     * @return true إذا تم تعديل الدالة / if function was modified
     * 
     * @details
     * (AR) يطبق نشر النسخ على جميع الكتل الأساسية في الدالة.
     *      يتكرر حتى لا يتم العثور على المزيد من النسخ.
     * 
     * (EN) Applies copy propagation to all basic blocks in function.
     *      Iterates until no more copies can be found.
     */
    bool runOnFunction(SIR::SIRFunction* function) override;

private:
    /**
     * @brief معالجة كتلة أساسية واحدة
     * @brief Process single basic block
     * 
     * @param block الكتلة المراد معالجتها / Block to process
     * @param blockIndex رقم الكتلة / Block index
     * @return true إذا تم تعديل الكتلة / if block was modified
     */
    bool processBlock(SIR::SIRBasicBlock* block, int blockIndex);
    
    /**
     * @brief اكتشاف عملية نسخ
     * @brief Detect copy operation
     * 
     * @param inst التعليمة المراد فحصها / Instruction to check
     * @return معلومات النسخة إذا كانت عملية نسخ / Copy info if it's a copy
     * 
     * @details
     * (AR) تكتشف الأنماط التالية:
     *      - %dest = MOVE %src
     *      - %dest = ADD %src, 0
     *      - %dest = MUL %src, 1
     *      - %dest = OR %src, 0
     * 
     * (EN) Detects following patterns:
     *      - %dest = MOVE %src
     *      - %dest = ADD %src, 0
     *      - %dest = MUL %src, 1
     *      - %dest = OR %src, 0
     */
    std::unique_ptr<CopyInfo> detectCopy(SIR::SIRInstruction* inst, int blockIndex);
    
    /**
     * @brief التحقق من أن التعليمة هي عملية نسخ بسيطة
     * @brief Check if instruction is simple copy
     * 
     * @param inst التعليمة / Instruction
     * @return true إذا كانت نسخة بسيطة / if simple copy
     */
    bool isSimpleCopy(const SIR::SIRInstruction* inst) const;
    
    /**
     * @brief بناء سلسلة النسخ
     * @brief Build copy chain
     * 
     * @param reg السجل / Register
     * @return السجل الأصلي في نهاية السلسلة / Original register at end of chain
     * 
     * @details
     * (AR) إذا كان %a = %b و %b = %c، فإن السلسلة هي %a → %b → %c
     *      ترجع %c كالسجل الأصلي.
     * 
     * (EN) If %a = %b and %b = %c, chain is %a → %b → %c
     *      Returns %c as original register.
     * 
     * @note يكتشف الدورات لتجنب حلقات لا نهائية / Detects cycles to avoid infinite loops
     */
    std::string resolveCopyChain(const std::string& reg);
    
    /**
     * @brief استبدال استخدامات السجل
     * @brief Replace register uses
     * 
     * @param function الدالة / Function
     * @param oldReg السجل القديم / Old register
     * @param newReg السجل الجديد / New register
     * @param startBlock رقم الكتلة للبدء منها / Block index to start from
     * @return عدد الاستبدالات / Number of replacements
     * 
     * @details
     * (AR) يستبدل جميع استخدامات oldReg بـ newReg في الكتل اللاحقة.
     *      لا يستبدل التعريفات، فقط الاستخدامات.
     * 
     * (EN) Replaces all uses of oldReg with newReg in subsequent blocks.
     *      Does not replace definitions, only uses.
     */
    int replaceRegisterUses(SIR::SIRFunction* function, 
                           const std::string& oldReg,
                           const std::string& newReg,
                           int startBlock);
    
    /**
     * @brief التحقق من استخدام السجل في الدالة
     * @brief Check if register is used in function
     * 
     * @param function الدالة / Function
     * @param reg السجل / Register
     * @param startBlock رقم الكتلة للبدء منها / Block index to start from
     * @return true إذا كان السجل مستخدماً / if register is used
     */
    bool isRegisterUsed(SIR::SIRFunction* function,
                       const std::string& reg,
                       int startBlock) const;
    
    /**
     * @brief إزالة تعليمة نسخ
     * @brief Remove copy instruction
     * 
     * @param block الكتلة / Block
     * @param inst التعليمة / Instruction
     * @return true إذا تم الحذف / if removed
     */
    bool removeCopyInstruction(SIR::SIRBasicBlock* block,
                               SIR::SIRInstruction* inst);
    
    /**
     * @brief مسح الجداول
     * @brief Clear tables
     */
    void clearTables();

private:
    /// (AR) خريطة من السجل الوجهة إلى السجل المصدر
    /// (EN) Map from destination register to source register
    std::unordered_map<std::string, std::string> copyMap_;
    
    /// (AR) مجموعة النسخ المكتشفة
    /// (EN) Set of detected copies
    std::vector<CopyInfo> copies_;
    
    /// (AR) مجموعة التعليمات المحذوفة
    /// (EN) Set of removed instructions
    std::unordered_set<SIR::SIRInstruction*> removedInstructions_;
    
    /// (AR) عدد النسخ المنشورة
    /// (EN) Number of propagated copies
    int propagationCount_;
    
    /// (AR) عدد التعليمات المحذوفة
    /// (EN) Number of removed instructions
    int removalCount_;
};

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
