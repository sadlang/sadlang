/**
 * @file pass.cpp
 * @brief تطبيق صنف التمرير الأساسي
 * @brief Base Optimization Pass Implementation
 * 
 * @details
 * (AR) يحتوي على التطبيق الأساسي لجميع تمريرات التحسين.
 * (EN) Contains base implementation for all optimization passes.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "middle/pass.h"
#include <iostream>
#include <iomanip>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief Constructor / البناء
 */
OptimizationPass::OptimizationPass(const std::string& name, PassType type)
    : name_(name)
    , type_(type)
    , modificationCount_(0)
{
}

/**
 * @brief إعادة تعيين الإحصائيات
 * @brief Reset statistics
 */
void OptimizationPass::resetStats() {
    modificationCount_ = 0;
}

/**
 * @brief تسجيل تعديل
 * @brief Record modification
 * 
 * @details
 * (AR) يزيد عداد التعديلات عند إجراء تحسين.
 * (EN) Increments modification counter when optimization is performed.
 */
void OptimizationPass::recordModification() {
    modificationCount_++;
}

/**
 * @brief طباعة رسالة تصحيح
 * @brief Print debug message
 * 
 * @param message الرسالة / Message
 */
void OptimizationPass::debug(const std::string& message) const {
    #ifdef SIR_OPTIMIZER_DEBUG
    std::cout << "[" << name_ << "] " << message << std::endl;
    #endif
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
