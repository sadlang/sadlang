/*
 * محسّن LLVM / LLVM Optimizer
 * ===========================
 * 
 * هذا الملف يوفر واجهة لتحسين كود LLVM IR باستخدام مجموعة من التمريرات.
 * This file provides an interface for optimizing LLVM IR code using a set of passes.
 * 
 * المستويات المدعومة / Supported Optimization Levels:
 * - O0: بدون تحسين / No optimization (debug builds)
 * - O1: تحسينات أساسية / Basic optimizations
 * - O2: تحسينات قياسية / Standard optimizations (recommended)
 * - O3: تحسينات عدوانية / Aggressive optimizations (maximum performance)
 * 
 * التمريرات المدعومة / Supported Passes:
 * - Dead Code Elimination (DCE): إزالة الكود الميت
 * - Constant Folding: طي الثوابت
 * - Common Subexpression Elimination (CSE): إزالة التعبيرات الفرعية المشتركة
 * - Loop Invariant Code Motion (LICM): نقل الكود الثابت خارج الحلقات
 * - Function Inlining: دمج الدوال
 * - Global Value Numbering (GVN): ترقيم القيم العامة
 * - Scalar Replacement of Aggregates (SROA): استبدال البنيات المجمعة
 * - Loop Unrolling: فك الحلقات
 * - Tail Call Optimization: تحسين استدعاء الذيل
 * 
 * مثال الاستخدام / Usage Example:
 * ```cpp
 * LLVMOptimizer optimizer;
 * optimizer.setOptimizationLevel(OptimizationLevel::O2);
 * optimizer.optimize(module.get());
 * ```
 */

#ifndef LLVM_OPTIMIZER_H
#define LLVM_OPTIMIZER_H

#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Analysis/TargetTransformInfo.h>
#include <llvm/Target/TargetMachine.h>
#include <memory>
#include <string>
#include <vector>

namespace sad {

/**
 * مستويات التحسين / Optimization Levels
 */
enum class OptimizationLevel {
    O0,  // بدون تحسين / No optimization
    O1,  // تحسينات أساسية / Basic optimizations
    O2,  // تحسينات قياسية / Standard optimizations
    O3,  // تحسينات عدوانية / Aggressive optimizations
    Os,  // تحسين لتقليل الحجم / Optimize for size
    Oz   // تحسين قوي لتقليل الحجم / Aggressively optimize for size
};

/**
 * إحصائيات التحسين / Optimization Statistics
 */
struct OptimizationStats {
    size_t instructions_before;      // عدد التعليمات قبل التحسين / Instructions before
    size_t instructions_after;       // عدد التعليمات بعد التحسين / Instructions after
    size_t functions_inlined;        // عدد الدوال المدمجة / Functions inlined
    size_t dead_code_eliminated;     // عدد التعليمات الميتة المحذوفة / Dead instructions removed
    size_t loops_unrolled;           // عدد الحلقات المفكوكة / Loops unrolled
    double optimization_time_ms;     // وقت التحسين بالميلي ثانية / Optimization time in ms
    
    OptimizationStats()
        : instructions_before(0), instructions_after(0),
          functions_inlined(0), dead_code_eliminated(0),
          loops_unrolled(0), optimization_time_ms(0.0) {}
};

/**
 * محسّن LLVM / LLVM Optimizer
 * 
 * هذه الفئة توفر واجهة عالية المستوى لتحسين وحدات LLVM IR.
 * This class provides a high-level interface for optimizing LLVM IR modules.
 */
class LLVMOptimizer {
public:
    /**
     * المُنشئ / Constructor
     */
    LLVMOptimizer();
    
    /**
     * المُدمر / Destructor
     */
    ~LLVMOptimizer();
    
    /**
     * تهيئة المُحسّن / Initialize the optimizer
     * @param targetMachine آلة الهدف المستخدمة للتحسينات المعتمدة على الهدف / Target machine for target-dependent optimizations
     * @return true إذا نجحت التهيئة / true if initialization succeeded
     */
    bool initialize(llvm::TargetMachine* targetMachine);
    
    /**
     * تعيين مستوى التحسين / Set optimization level
     * @param level مستوى التحسين المطلوب / Desired optimization level
     */
    void setOptimizationLevel(OptimizationLevel level);
    
    /**
     * الحصول على مستوى التحسين الحالي / Get current optimization level
     * @return مستوى التحسين / Optimization level
     */
    OptimizationLevel getOptimizationLevel() const;
    
    /**
     * تحسين وحدة LLVM / Optimize an LLVM module
     * @param module الوحدة المراد تحسينها / Module to optimize
     * @return true إذا نجح التحسين / true if optimization succeeded
     */
    bool optimize(llvm::Module* module);
    
    /**
     * الحصول على إحصائيات التحسين الأخيرة / Get statistics from last optimization
     * @return إحصائيات التحسين / Optimization statistics
     */
    const OptimizationStats& getStats() const;
    
    /**
     * تمكين/تعطيل تمريرة معينة / Enable/disable a specific pass
     * @param passName اسم التمريرة / Pass name
     * @param enabled true للتمكين، false للتعطيل / true to enable, false to disable
     */
    void setPassEnabled(const std::string& passName, bool enabled);
    
    /**
     * التحقق من تمكين تمريرة / Check if a pass is enabled
     * @param passName اسم التمريرة / Pass name
     * @return true إذا كانت التمريرة ممكّنة / true if pass is enabled
     */
    bool isPassEnabled(const std::string& passName) const;
    
    /**
     * الحصول على قائمة التمريرات المتاحة / Get list of available passes
     * @return قائمة بأسماء التمريرات / List of pass names
     */
    std::vector<std::string> getAvailablePasses() const;
    
    /**
     * تشغيل تمريرة مخصصة / Run a custom pass
     * @param passName اسم التمريرة / Pass name
     * @param module الوحدة المراد تحسينها / Module to optimize
     * @return true إذا نجح التشغيل / true if execution succeeded
     */
    bool runCustomPass(const std::string& passName, llvm::Module* module);
    
    /**
     * طباعة معلومات التحسين / Print optimization info
     */
    void printStats() const;
    
    /**
     * إعادة تعيين الإحصائيات / Reset statistics
     */
    void resetStats();
    
    /**
     * تمكين/تعطيل التحقق بعد كل تمريرة / Enable/disable verification after each pass
     * @param verify true للتمكين / true to enable
     */
    void setVerifyEachPass(bool verify);
    
    /**
     * تمكين/تعطيل طباعة الوقت لكل تمريرة / Enable/disable timing prints for each pass
     * @param timePasses true للتمكين / true to enable
     */
    void setTimePasses(bool timePasses);

private:
    /**
     * بناء خط أنابيب التحسين للمستوى المحدد / Build optimization pipeline for level
     * @param level مستوى التحسين / Optimization level
     */
    void buildOptimizationPipeline(OptimizationLevel level);
    
    /**
     * إضافة تمريرات التحسين الأساسية (O1) / Add basic optimization passes (O1)
     */
    void addBasicOptimizations();
    
    /**
     * إضافة تمريرات التحسين القياسية (O2) / Add standard optimization passes (O2)
     */
    void addStandardOptimizations();
    
    /**
     * إضافة تمريرات التحسين العدوانية (O3) / Add aggressive optimization passes (O3)
     */
    void addAggressiveOptimizations();
    
    /**
     * إضافة تمريرات تحسين الحجم (Os/Oz) / Add size optimization passes (Os/Oz)
     */
    void addSizeOptimizations(bool aggressive);
    
    /**
     * تشغيل تمريرات الوحدة / Run module passes
     * @param module الوحدة / Module
     */
    void runModulePasses(llvm::Module* module);
    
    /**
     * تشغيل تمريرات الدالة / Run function passes
     * @param module الوحدة / Module
     */
    void runFunctionPasses(llvm::Module* module);
    
    /**
     * جمع إحصائيات الوحدة / Collect module statistics
     * @param module الوحدة / Module
     * @return عدد التعليمات / Instruction count
     */
    size_t collectModuleStats(llvm::Module* module);
    
    /**
     * التحقق من الوحدة / Verify module
     * @param module الوحدة / Module
     * @return true إذا كانت الوحدة صحيحة / true if module is valid
     */
    bool verifyModule(llvm::Module* module);

private:
    OptimizationLevel optimization_level_;           // مستوى التحسين / Optimization level
    llvm::TargetMachine* target_machine_;            // آلة الهدف / Target machine
    
    std::unique_ptr<llvm::PassBuilder> pass_builder_;              // باني التمريرات / Pass builder
    std::unique_ptr<llvm::LoopAnalysisManager> loop_am_;           // مدير تحليل الحلقات / Loop analysis manager
    std::unique_ptr<llvm::FunctionAnalysisManager> function_am_;   // مدير تحليل الدوال / Function analysis manager
    std::unique_ptr<llvm::CGSCCAnalysisManager> cgscc_am_;         // مدير تحليل CGSCC / CGSCC analysis manager
    std::unique_ptr<llvm::ModuleAnalysisManager> module_am_;       // مدير تحليل الوحدات / Module analysis manager
    
    std::unique_ptr<llvm::ModulePassManager> module_pm_;           // مدير تمريرات الوحدة / Module pass manager
    std::unique_ptr<llvm::FunctionPassManager> function_pm_;       // مدير تمريرات الدالة / Function pass manager
    
    OptimizationStats stats_;                        // إحصائيات التحسين / Optimization statistics
    
    bool verify_each_pass_;                          // التحقق بعد كل تمريرة / Verify after each pass
    bool time_passes_;                               // طباعة وقت التمريرات / Print pass timing
    
    std::unordered_map<std::string, bool> enabled_passes_;  // التمريرات الممكنة / Enabled passes
};

} // namespace sad

#endif // LLVM_OPTIMIZER_H
