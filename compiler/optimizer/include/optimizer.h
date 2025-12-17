/**
 * @file optimizer.h
 * @brief محسّن التمثيل الوسيط SIR
 * @brief SIR Intermediate Representation Optimizer
 * 
 * @details
 * (AR) يوفر هذا الملف الواجهة الرئيسية للمحسّن الذي يطبق
 *      تمريرات متعددة من التحسينات على وحدة SIR.
 *      يدعم مستويات تحسين مختلفة (0-3).
 * 
 * (EN) This file provides the main optimizer interface that applies
 *      multiple optimization passes to a SIR module.
 *      Supports different optimization levels (0-3).
 * 
 * @author SadLanguage Compiler Team
 * @date December 2025
 * @version 1.0
 */

#pragma once

#include <memory>
#include <vector>
#include <string>
#include "pass.h"
#include "pass_manager.h"
#include "../../frontend/include/sir_builder.h"

namespace Sad {
namespace Compiler {
namespace Optimizer {

/**
 * @brief مستوى التحسين
 * @brief Optimization Level
 */
enum class OptimizationLevel {
    O0 = 0,  // لا توجد تحسينات / No optimization
    O1 = 1,  // تحسينات أساسية / Basic optimizations
    O2 = 2,  // تحسينات قياسية (افتراضي) / Standard (default)
    O3 = 3   // تحسينات عدوانية / Aggressive optimizations
};

/**
 * @brief إحصائيات التحسين
 * @brief Optimization Statistics
 */
struct OptimizationStats {
    int originalInstructionCount;   // عدد التعليمات الأصلية / Original instruction count
    int optimizedInstructionCount;  // عدد التعليمات المحسّنة / Optimized instruction count
    int instructionsRemoved;        // التعليمات المحذوفة / Instructions removed
    int registersReduced;           // السجلات المختصرة / Registers reduced
    double optimizationTimeMs;      // وقت التحسين (مللي ثانية) / Optimization time (ms)
    std::vector<PassStats> passStats; // إحصائيات التمريرات / Pass statistics
    
    /**
     * @brief حساب نسبة التحسين
     * @brief Calculate improvement percentage
     */
    double getImprovementPercentage() const {
        if (originalInstructionCount == 0) return 0.0;
        return 100.0 * instructionsRemoved / originalInstructionCount;
    }
};

/**
 * @brief محسّن SIR
 * @brief SIR Optimizer
 * 
 * @details
 * (AR) المحسّن الرئيسي الذي يطبق تمريرات التحسين على وحدة SIR.
 *      يدعم مستويات تحسين مختلفة ويجمع الإحصائيات.
 * 
 * (EN) Main optimizer that applies optimization passes to SIR module.
 *      Supports different optimization levels and collects statistics.
 * 
 * @example
 * ```cpp
 * Optimizer optimizer(OptimizationLevel::O2);
 * auto module = sirBuilder.buildModule(ast);
 * optimizer.optimize(module);
 * auto stats = optimizer.getStats();
 * std::cout << "Improvement: " << stats.getImprovementPercentage() << "%\n";
 * ```
 */
class Optimizer {
public:
    /**
     * @brief Constructor / البناء
     * @param level مستوى التحسين (افتراضياً O2) / Optimization level (default O2)
     */
    explicit Optimizer(OptimizationLevel level = OptimizationLevel::O2);
    
    /**
     * @brief Destructor / الهدم
     */
    ~Optimizer();
    
    /**
     * @brief تحسين وحدة SIR كاملة
     * @brief Optimize complete SIR module
     * 
     * @param module الوحدة المراد تحسينها / Module to optimize
     * @return true إذا نجح التحسين / if optimization succeeded
     * 
     * @details
     * (AR) يطبق جميع تمريرات التحسين المحددة حسب مستوى التحسين.
     *      يتم تطبيق التمريرات على كل دالة في الوحدة.
     * 
     * (EN) Applies all configured optimization passes based on level.
     *      Passes are applied to each function in the module.
     */
    bool optimize(std::shared_ptr<SIR::SIRModule> module);
    
    /**
     * @brief تحسين دالة واحدة
     * @brief Optimize single function
     * 
     * @param function الدالة المراد تحسينها / Function to optimize
     * @return true إذا تم تعديل الدالة / if function was modified
     */
    bool optimizeFunction(SIR::SIRFunction* function);
    
    /**
     * @brief إضافة تمرير تحسين مخصص
     * @brief Add custom optimization pass
     * 
     * @param pass التمرير المراد إضافته / Pass to add
     */
    void addPass(std::unique_ptr<OptimizationPass> pass);
    
    /**
     * @brief الحصول على إحصائيات التحسين
     * @brief Get optimization statistics
     * 
     * @return إحصائيات التحسين / Optimization statistics
     */
    const OptimizationStats& getStats() const { return stats_; }
    
    /**
     * @brief طباعة إحصائيات التحسين
     * @brief Print optimization statistics
     */
    void printStats() const;
    
    /**
     * @brief تمكين/تعطيل وضع التصحيح
     * @brief Enable/disable debug mode
     */
    void setDebugMode(bool enabled) { debugMode_ = enabled; }
    
    /**
     * @brief التحقق من صحة التحسين
     * @brief Verify optimization correctness
     * 
     * @details
     * (AR) يتحقق من أن التحسين لم يغير المعنى البرمجي.
     * (EN) Verifies that optimization preserved program semantics.
     */
    bool verify(std::shared_ptr<SIR::SIRModule> module) const;

private:
    /**
     * @brief تهيئة التمريرات حسب المستوى
     * @brief Initialize passes based on level
     */
    void initializePasses();
    
    /**
     * @brief إضافة تمريرات المستوى 1
     * @brief Add level 1 passes
     */
    void addLevel1Passes();
    
    /**
     * @brief إضافة تمريرات المستوى 2
     * @brief Add level 2 passes
     */
    void addLevel2Passes();
    
    /**
     * @brief إضافة تمريرات المستوى 3
     * @brief Add level 3 passes
     */
    void addLevel3Passes();
    
    /**
     * @brief جمع الإحصائيات من الوحدة
     * @brief Collect statistics from module
     */
    void collectStats(std::shared_ptr<SIR::SIRModule> module);

private:
    OptimizationLevel level_;           // مستوى التحسين / Optimization level
    std::unique_ptr<PassManager> passManager_; // مدير التمريرات / Pass manager
    OptimizationStats stats_;           // الإحصائيات / Statistics
    bool debugMode_;                    // وضع التصحيح / Debug mode
};

/**
 * @brief إنشاء محسّن بمستوى محدد
 * @brief Create optimizer with specified level
 * 
 * @param level مستوى التحسين / Optimization level
 * @return محسّن جديد / New optimizer
 */
std::unique_ptr<Optimizer> createOptimizer(OptimizationLevel level);

/**
 * @brief تحويل نص إلى مستوى تحسين
 * @brief Convert string to optimization level
 * 
 * @param str النص ("O0", "O1", "O2", "O3") / String ("O0", "O1", "O2", "O3")
 * @return مستوى التحسين / Optimization level
 */
OptimizationLevel parseOptimizationLevel(const std::string& str);

} // namespace Optimizer
} // namespace Compiler
} // namespace Sad
