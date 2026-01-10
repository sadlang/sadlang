/**
 * @file dead_code_elimination_pass.h
 * @brief تمرير إزالة الكود الميت
 * @brief Dead Code Elimination Pass
 * 
 * @details
 * (AR) يحذف التعليمات والكتل الأساسية غير المستخدمة أو غير القابلة للوصول.
 *      يشمل: المتغيرات غير المستخدمة، الكتل الميتة، التخزينات الزائدة.
 * 
 * (EN) Removes unused instructions and unreachable basic blocks.
 *      Includes: unused variables, dead blocks, redundant stores.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#pragma once

#include "pass.h"
#include <unordered_set>
#include <unordered_map>
#include <queue>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief تمرير إزالة الكود الميت
 * @brief Dead Code Elimination Pass
 * 
 * @details
 * (AR) يستخدم تحليل الحيوية (liveness analysis) لتحديد التعليمات
 *      والكتل غير المستخدمة ثم يحذفها.
 * 
 * (EN) Uses liveness analysis to identify unused instructions
 *      and blocks, then removes them.
 * 
 * @example
 * Before:
 *   %0 = ADD %a, %b    // %0 never used
 *   %1 = MUL %c, %d
 *   RET %1
 * 
 * After:
 *   %1 = MUL %c, %d
 *   RET %1
 */
class DeadCodeEliminationPass : public OptimizationPass {
public:
    /**
     * @brief Constructor / البناء
     */
    DeadCodeEliminationPass();
    
    /**
     * @brief تشغيل التمرير على دالة
     * @brief Run pass on function
     * 
     * @param function الدالة المراد تحسينها / Function to optimize
     * @return true إذا تم تعديل الدالة / if function was modified
     */
    bool runOnFunction(SIR::SIRFunction* function) override;

private:
    /**
     * @brief حساب التعليمات الحية
     * @brief Compute live instructions
     * 
     * @details
     * (AR) يستخدم خوارزمية البحث العكسي لتحديد التعليمات الحية.
     *      يبدأ من تعليمات الإرجاع والتخزين ويتتبع الاستخدامات.
     * 
     * (EN) Uses backward search algorithm to identify live instructions.
     *      Starts from return and store instructions and traces uses.
     * 
     * @param function الدالة / Function
     * @return مجموعة التعليمات الحية / Set of live instructions
     */
    std::unordered_set<SIR::SIRInstruction*> computeLiveInstructions(
        SIR::SIRFunction* function
    );
    
    /**
     * @brief إضافة تعليمة إلى مجموعة الحية
     * @brief Add instruction to live set
     * 
     * @details
     * (AR) يضيف التعليمة ومعاملاتها بشكل متكرر.
     * (EN) Adds instruction and its operands recursively.
     */
    void markAsLive(
        SIR::SIRInstruction* inst,
        std::unordered_set<SIR::SIRInstruction*>& liveSet,
        std::queue<SIR::SIRInstruction*>& workList,
        const std::unordered_map<std::string, SIR::SIRInstruction*>& defMap
    );
    
    /**
     * @brief بناء خريطة التعريفات
     * @brief Build definitions map
     * 
     * @details
     * (AR) تربط كل سجل بالتعليمة التي عرّفته.
     * (EN) Maps each register to the instruction that defined it.
     * 
     * @param function الدالة / Function
     * @return خريطة السجلات → التعليمات / Register → Instruction map
     */
    std::unordered_map<std::string, SIR::SIRInstruction*> buildDefMap(
        SIR::SIRFunction* function
    );
    
    /**
     * @brief إزالة التعليمات الميتة من كتلة
     * @brief Remove dead instructions from block
     * 
     * @param block الكتلة / Block
     * @param liveSet مجموعة التعليمات الحية / Live instruction set
     * @return عدد التعليمات المحذوفة / Number of removed instructions
     */
    int removeDeadInstructions(
        SIR::SIRBasicBlock* block,
        const std::unordered_set<SIR::SIRInstruction*>& liveSet
    );
    
    /**
     * @brief حذف الكتل غير القابلة للوصول
     * @brief Remove unreachable blocks
     * 
     * @details
     * (AR) يستخدم BFS من كتلة الدخول لإيجاد الكتل القابلة للوصول.
     * (EN) Uses BFS from entry block to find reachable blocks.
     * 
     * @param function الدالة / Function
     * @return عدد الكتل المحذوفة / Number of removed blocks
     */
    int removeUnreachableBlocks(SIR::SIRFunction* function);
    
    /**
     * @brief حساب الكتل القابلة للوصول
     * @brief Compute reachable blocks
     * 
     * @param function الدالة / Function
     * @return مجموعة الكتل القابلة للوصول / Set of reachable blocks
     */
    std::unordered_set<SIR::SIRBasicBlock*> computeReachableBlocks(
        SIR::SIRFunction* function
    );
    
    /**
     * @brief التحقق من كون التعليمة لها تأثير جانبي
     * @brief Check if instruction has side effect
     * 
     * @details
     * (AR) التعليمات ذات التأثير الجانبي تعتبر حية دائماً
     *      (مثل: CALL, STORE, RET).
     * 
     * (EN) Instructions with side effects are always considered live
     *      (e.g., CALL, STORE, RET).
     */
    bool hasSideEffect(SIR::SIRInstruction* inst) const;
    
    /**
     * @brief الحصول على السجل المعرّف بواسطة التعليمة
     * @brief Get register defined by instruction
     */
    std::optional<std::string> getDefinedRegister(SIR::SIRInstruction* inst) const;
    
    /**
     * @brief الحصول على السجلات المستخدمة في التعليمة
     * @brief Get registers used in instruction
     */
    std::vector<std::string> getUsedRegisters(SIR::SIRInstruction* inst) const;
};

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
