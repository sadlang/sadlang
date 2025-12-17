/**
 * @file pass_manager.h
 * @brief مدير تمريرات التحسين
 * @brief Optimization Pass Manager
 * 
 * @details
 * (AR) يدير تنفيذ تمريرات التحسين بترتيب محدد.
 *      يتتبع التبعيات بين التمريرات ويجمع الإحصائيات.
 * 
 * (EN) Manages execution of optimization passes in specific order.
 *      Tracks dependencies between passes and collects statistics.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "pass.h"

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief مدير التمريرات
 * @brief Pass Manager
 * 
 * @details
 * (AR) يدير مجموعة من تمريرات التحسين ويطبقها بالترتيب الصحيح.
 *      يدعم التمريرات المتكررة والتحقق من التقارب.
 * 
 * (EN) Manages a set of optimization passes and applies them in correct order.
 *      Supports iterative passes and convergence checking.
 */
class PassManager {
public:
    /**
     * @brief Constructor / البناء
     */
    PassManager();
    
    /**
     * @brief Destructor / الهدم
     */
    ~PassManager();
    
    /**
     * @brief إضافة تمرير
     * @brief Add pass
     * 
     * @param pass التمرير المراد إضافته / Pass to add
     */
    void addPass(std::unique_ptr<OptimizationPass> pass);
    
    /**
     * @brief تشغيل جميع التمريرات على دالة
     * @brief Run all passes on function
     * 
     * @param function الدالة المراد تحسينها / Function to optimize
     * @return true إذا تم تعديل الدالة / if function was modified
     * 
     * @details
     * (AR) يطبق جميع التمريرات المسجلة بالترتيب.
     *      يكرر التطبيق حتى عدم حدوث تغييرات أو الوصول للحد الأقصى.
     * 
     * (EN) Applies all registered passes in order.
     *      Repeats until no changes or max iterations reached.
     */
    bool runOnFunction(SIR::SIRFunction* function);
    
    /**
     * @brief الحصول على إحصائيات التمريرات
     * @brief Get pass statistics
     */
    const std::vector<PassStats>& getPassStats() const { return passStats_; }
    
    /**
     * @brief إعادة تعيين الإحصائيات
     * @brief Reset statistics
     */
    void resetStats();
    
    /**
     * @brief تعيين الحد الأقصى للتكرارات
     * @brief Set maximum iterations
     * 
     * @param maxIter الحد الأقصى / Maximum iterations
     */
    void setMaxIterations(int maxIter) { maxIterations_ = maxIter; }
    
    /**
     * @brief تمكين/تعطيل التكرار
     * @brief Enable/disable iteration
     */
    void setIterative(bool iterative) { iterative_ = iterative; }
    
    /**
     * @brief طباعة معلومات التمريرات
     * @brief Print pass information
     */
    void printPassInfo() const;

private:
    /**
     * @brief تطبيق تمرير واحد
     * @brief Apply single pass
     */
    bool applyPass(OptimizationPass* pass, SIR::SIRFunction* function);

private:
    std::vector<std::unique_ptr<OptimizationPass>> passes_; // التمريرات / Passes
    std::vector<PassStats> passStats_;                      // الإحصائيات / Statistics
    int maxIterations_;                                     // الحد الأقصى للتكرارات / Max iterations
    bool iterative_;                                        // تمكين التكرار / Enable iteration
};

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
