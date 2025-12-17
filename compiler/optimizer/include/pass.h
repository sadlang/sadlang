/**
 * @file pass.h
 * @brief صنف التمرير الأساسي للتحسين
 * @brief Base Optimization Pass Class
 * 
 * @details
 * (AR) يوفر هذا الملف الصنف الأساسي لجميع تمريرات التحسين.
 *      كل تمرير يرث من OptimizationPass ويطبق دالة runOnFunction.
 * 
 * (EN) This file provides the base class for all optimization passes.
 *      Each pass inherits from OptimizationPass and implements runOnFunction.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../../frontend/include/sir_builder.h"

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief نوع التمرير
 * @brief Pass Type
 */
enum class PassType {
    ANALYSIS,        // تحليل فقط / Analysis only
    TRANSFORMATION   // تحويل / Transformation
};

/**
 * @brief صنف التمرير الأساسي
 * @brief Base Optimization Pass Class
 * 
 * @details
 * (AR) صنف أساسي مجرد لجميع تمريرات التحسين.
 *      يجب على كل تمرير تطبيق دالة runOnFunction.
 * 
 * (EN) Abstract base class for all optimization passes.
 *      Each pass must implement the runOnFunction method.
 */
class OptimizationPass {
public:
    /**
     * @brief Constructor / البناء
     * @param name اسم التمرير / Pass name
     * @param type نوع التمرير / Pass type
     */
    OptimizationPass(const std::string& name, PassType type);
    
    /**
     * @brief Destructor / الهدم
     */
    virtual ~OptimizationPass() = default;
    
    /**
     * @brief تشغيل التمرير على دالة
     * @brief Run pass on function
     * 
     * @param function الدالة المراد تحسينها / Function to optimize
     * @return true إذا تم تعديل الدالة / if function was modified
     */
    virtual bool runOnFunction(SIR::SIRFunction* function) = 0;
    
    /**
     * @brief الحصول على اسم التمرير
     * @brief Get pass name
     */
    const std::string& getName() const { return name_; }
    
    /**
     * @brief الحصول على نوع التمرير
     * @brief Get pass type
     */
    PassType getType() const { return type_; }
    
    /**
     * @brief الحصول على عدد التعديلات
     * @brief Get modification count
     */
    int getModificationCount() const { return modificationCount_; }
    
    /**
     * @brief إعادة تعيين الإحصائيات
     * @brief Reset statistics
     */
    void resetStats();

protected:
    /**
     * @brief تسجيل تعديل
     * @brief Record modification
     */
    void recordModification();
    
    /**
     * @brief طباعة رسالة تصحيح
     * @brief Print debug message
     */
    void debug(const std::string& message) const;

private:
    std::string name_;              // اسم التمرير / Pass name
    PassType type_;                 // نوع التمرير / Pass type
    int modificationCount_;         // عدد التعديلات / Modification count
};

/**
 * @brief إحصائيات التمرير
 * @brief Pass Statistics
 */
struct PassStats {
    std::string passName;           // اسم التمرير / Pass name
    int modificationsCount;         // عدد التعديلات / Modifications count
    double executionTimeMs;         // وقت التنفيذ (مللي ثانية) / Execution time (ms)
    
    PassStats() : modificationsCount(0), executionTimeMs(0.0) {}
};

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
