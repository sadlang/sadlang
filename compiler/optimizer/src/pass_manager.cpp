/**
 * @file pass_manager.cpp
 * @brief تطبيق مدير تمريرات التحسين
 * @brief Optimization Pass Manager Implementation
 * 
 * @details
 * (AR) يدير تنفيذ تمريرات التحسين المتعددة بالترتيب الصحيح.
 * (EN) Manages execution of multiple optimization passes in correct order.
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#include "../include/pass_manager.h"
#include <iostream>
#include <chrono>
#include <iomanip>

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief Constructor / البناء
 */
PassManager::PassManager()
    : maxIterations_(10)
    , iterative_(true)
{
}

/**
 * @brief Destructor / الهدم
 */
PassManager::~PassManager() {
    passes_.clear();
}

/**
 * @brief إضافة تمرير
 * @brief Add pass
 */
void PassManager::addPass(std::unique_ptr<OptimizationPass> pass) {
    if (pass) {
        passes_.push_back(std::move(pass));
    }
}

/**
 * @brief تشغيل جميع التمريرات على دالة
 * @brief Run all passes on function
 * 
 * @details
 * (AR) يطبق التمريرات بشكل متكرر حتى عدم حدوث تغييرات.
 * (EN) Applies passes iteratively until no changes occur.
 */
bool PassManager::runOnFunction(SIR::SIRFunction* function) {
    if (!function) {
        return false;
    }
    
    bool totalModified = false;
    int iteration = 0;
    
    // تكرار التمريرات حتى التقارب / Iterate until convergence
    do {
        bool iterationModified = false;
        
        // تطبيق كل تمرير / Apply each pass
        for (auto& pass : passes_) {
            bool passModified = applyPass(pass.get(), function);
            
            if (passModified) {
                iterationModified = true;
                totalModified = true;
            }
        }
        
        iteration++;
        
        // إذا لم يتم التعديل أو وصلنا للحد الأقصى، نتوقف
        // If no modifications or max iterations reached, stop
        if (!iterationModified || !iterative_) {
            break;
        }
        
        if (iteration >= maxIterations_) {
            #ifdef SIR_OPTIMIZER_DEBUG
            std::cerr << "Warning: Max iterations (" << maxIterations_ 
                      << ") reached for function " << function->getName() 
                      << std::endl;
            #endif
            break;
        }
        
    } while (iterative_);
    
    return totalModified;
}

/**
 * @brief تطبيق تمرير واحد
 * @brief Apply single pass
 * 
 * @details
 * (AR) يقيس وقت التنفيذ ويجمع الإحصائيات.
 * (EN) Measures execution time and collects statistics.
 */
bool PassManager::applyPass(OptimizationPass* pass, SIR::SIRFunction* function) {
    if (!pass || !function) {
        return false;
    }
    
    // إعادة تعيين إحصائيات التمرير / Reset pass statistics
    pass->resetStats();
    
    // قياس وقت التنفيذ / Measure execution time
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // تشغيل التمرير / Run pass
    bool modified = pass->runOnFunction(function);
    
    // حساب الوقت المستغرق / Calculate elapsed time
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        endTime - startTime
    );
    double timeMs = duration.count() / 1000.0;
    
    // تسجيل الإحصائيات / Record statistics
    PassStats stats;
    stats.passName = pass->getName();
    stats.modificationsCount = pass->getModificationCount();
    stats.executionTimeMs = timeMs;
    passStats_.push_back(stats);
    
    #ifdef SIR_OPTIMIZER_DEBUG
    if (modified) {
        std::cout << "[PassManager] " << pass->getName() 
                  << " modified function " << function->getName()
                  << " (" << stats.modificationsCount << " changes, "
                  << std::fixed << std::setprecision(2) << timeMs << " ms)"
                  << std::endl;
    }
    #endif
    
    return modified;
}

/**
 * @brief إعادة تعيين الإحصائيات
 * @brief Reset statistics
 */
void PassManager::resetStats() {
    passStats_.clear();
    
    for (auto& pass : passes_) {
        pass->resetStats();
    }
}

/**
 * @brief طباعة معلومات التمريرات
 * @brief Print pass information
 */
void PassManager::printPassInfo() const {
    std::cout << "\n╔════════════════════════════════════════════════════╗\n";
    std::cout << "║         Optimization Passes / تمريرات التحسين     ║\n";
    std::cout << "╠════════════════════════════════════════════════════╣\n";
    
    for (size_t i = 0; i < passes_.size(); i++) {
        const auto& pass = passes_[i];
        std::cout << "║ " << std::setw(2) << (i + 1) << ". "
                  << std::left << std::setw(44) << pass->getName()
                  << " ║\n";
    }
    
    std::cout << "╠════════════════════════════════════════════════════╣\n";
    std::cout << "║  Total Passes: " << std::setw(32) << passes_.size()
              << " ║\n";
    std::cout << "║  Max Iterations: " << std::setw(30) << maxIterations_
              << " ║\n";
    std::cout << "║  Iterative Mode: "
              << std::setw(30) << (iterative_ ? "Enabled" : "Disabled")
              << " ║\n";
    std::cout << "╚════════════════════════════════════════════════════╝\n\n";
}

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
