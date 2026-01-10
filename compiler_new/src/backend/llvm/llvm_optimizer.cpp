/*
 * تنفيذ محسّن LLVM / LLVM Optimizer Implementation
 * ===============================================
 */

#include "llvm_optimizer.h"
#include "arabic_optimizer.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/SROA.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>
#include <llvm/Transforms/Scalar/DCE.h>
#include <llvm/Transforms/Scalar/LICM.h>
#include <llvm/Transforms/Utils/LoopUtils.h>  // لـ createFunctionToLoopPassAdaptor / For createFunctionToLoopPassAdaptor
#include <llvm/Transforms/IPO/GlobalDCE.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/Timer.h>
#include <chrono>
#include <iostream>

namespace sad {

/**
 * المُنشئ / Constructor
 */
LLVMOptimizer::LLVMOptimizer()
    : optimization_level_(OptimizationLevel::O0),
      target_machine_(nullptr),
      verify_each_pass_(false),
      time_passes_(false) {
}

/**
 * المُدمر / Destructor
 */
LLVMOptimizer::~LLVMOptimizer() {
}

/**
 * تهيئة المُحسّن / Initialize the optimizer
 */
bool LLVMOptimizer::initialize(llvm::TargetMachine* targetMachine) {
    target_machine_ = targetMachine;
    
    // إنشاء باني التمريرات / Create pass builder
    pass_builder_ = std::make_unique<llvm::PassBuilder>(target_machine_);
    
    // إنشاء مديري التحليل / Create analysis managers
    loop_am_ = std::make_unique<llvm::LoopAnalysisManager>();
    function_am_ = std::make_unique<llvm::FunctionAnalysisManager>();
    cgscc_am_ = std::make_unique<llvm::CGSCCAnalysisManager>();
    module_am_ = std::make_unique<llvm::ModuleAnalysisManager>();
    
    // تسجيل تحليلات الحلقات / Register loop analyses
    pass_builder_->registerLoopAnalyses(*loop_am_);
    
    // تسجيل تحليلات الدوال / Register function analyses
    pass_builder_->registerFunctionAnalyses(*function_am_);
    
    // تسجيل تحليلات CGSCC / Register CGSCC analyses
    pass_builder_->registerCGSCCAnalyses(*cgscc_am_);
    
    // تسجيل تحليلات الوحدات / Register module analyses
    pass_builder_->registerModuleAnalyses(*module_am_);
    
    // ربط مديري التحليل / Cross-register analysis managers
    pass_builder_->crossRegisterProxies(*loop_am_, *function_am_, *cgscc_am_, *module_am_);
    
    // إنشاء محسّن اللغة العربية / Create Arabic language optimizer
    arabic_optimizer_ = std::make_unique<ArabicOptimizationPass>();
    
    return true;
}

/**
 * تعيين مستوى التحسين / Set optimization level
 */
void LLVMOptimizer::setOptimizationLevel(OptimizationLevel level) {
    optimization_level_ = level;
    buildOptimizationPipeline(level);
}

/**
 * الحصول على مستوى التحسين الحالي / Get current optimization level
 */
OptimizationLevel LLVMOptimizer::getOptimizationLevel() const {
    return optimization_level_;
}

/**
 * تحسين وحدة LLVM / Optimize an LLVM module
 */
bool LLVMOptimizer::optimize(llvm::Module* module) {
    if (!module) {
        std::cerr << "خطأ: وحدة فارغة / Error: null module" << std::endl;
        return false;
    }
    
    // إعادة تعيين الإحصائيات / Reset statistics
    resetStats();
    
    // بدء قياس الوقت / Start timing
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // جمع الإحصائيات قبل التحسين / Collect pre-optimization statistics
    stats_.instructions_before = collectModuleStats(module);
    
    // التحقق من الوحدة قبل التحسين / Verify module before optimization
    if (verify_each_pass_) {
        if (!verifyModule(module)) {
            std::cerr << "خطأ: الوحدة غير صحيحة قبل التحسين / Error: module invalid before optimization" << std::endl;
            return false;
        }
    }
    
    // تشغيل تمريرات التحسين / Run optimization passes
    if (optimization_level_ != OptimizationLevel::O0) {
        runModulePasses(module);
        runFunctionPasses(module);
    }
    
    // التحقق من الوحدة بعد التحسين / Verify module after optimization
    if (!verifyModule(module)) {
        std::cerr << "خطأ: الوحدة غير صحيحة بعد التحسين / Error: module invalid after optimization" << std::endl;
        return false;
    }
    
    // جمع الإحصائيات بعد التحسين / Collect post-optimization statistics
    stats_.instructions_after = collectModuleStats(module);
    
    // حساب وقت التحسين / Calculate optimization time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    stats_.optimization_time_ms = duration.count() / 1000.0;
    
    return true;
}

/**
 * الحصول على إحصائيات التحسين / Get optimization statistics
 */
const OptimizationStats& LLVMOptimizer::getStats() const {
    return stats_;
}

/**
 * تمكين/تعطيل تمريرة معينة / Enable/disable a specific pass
 */
void LLVMOptimizer::setPassEnabled(const std::string& passName, bool enabled) {
    enabled_passes_[passName] = enabled;
}

/**
 * التحقق من تمكين تمريرة / Check if a pass is enabled
 */
bool LLVMOptimizer::isPassEnabled(const std::string& passName) const {
    auto it = enabled_passes_.find(passName);
    if (it != enabled_passes_.end()) {
        return it->second;
    }
    return true; // افتراضياً جميع التمريرات ممكنة / By default, all passes enabled
}

/**
 * الحصول على قائمة التمريرات المتاحة / Get list of available passes
 */
std::vector<std::string> LLVMOptimizer::getAvailablePasses() const {
    return {
        "dce",              // Dead Code Elimination
        "sroa",             // Scalar Replacement of Aggregates
        "gvn",              // Global Value Numbering
        "licm",             // Loop Invariant Code Motion
        "inline",           // Function Inlining
        "simplifycfg",      // Simplify Control Flow Graph
        "instcombine",      // Instruction Combining
        "loop-unroll",      // Loop Unrolling
        "tailcallelim",     // Tail Call Elimination
        "globaldce",        // Global Dead Code Elimination
        "deadargelim",      // Dead Argument Elimination
        "mem2reg",          // Promote Memory to Register
        "constprop",        // Constant Propagation
        "ipsccp",           // Interprocedural Sparse Conditional Constant Propagation
        "vectorize"         // Vectorization
    };
}

/**
 * تشغيل تمريرة مخصصة / Run a custom pass
 */
bool LLVMOptimizer::runCustomPass(const std::string& passName, llvm::Module* module) {
    (void)passName; // Suppress unused parameter warning - function is extensible
    
    if (!module) {
        return false;
    }
    
    // تنفيذ التمريرات المخصصة حسب الاسم / Execute custom passes by name
    // هذه دالة قابلة للتوسيع / This function is extensible
    
    return true;
}

/**
 * طباعة معلومات التحسين / Print optimization info
 */
void LLVMOptimizer::printStats() const {
    std::cout << "\n========================================" << std::endl;
    std::cout << "إحصائيات التحسين / Optimization Statistics" << std::endl;
    std::cout << "========================================" << std::endl;
    
    std::cout << "عدد التعليمات قبل / Instructions Before: " 
              << stats_.instructions_before << std::endl;
    std::cout << "عدد التعليمات بعد / Instructions After: " 
              << stats_.instructions_after << std::endl;
    
    if (stats_.instructions_before > 0) {
        double reduction = 100.0 * (stats_.instructions_before - stats_.instructions_after) 
                          / stats_.instructions_before;
        std::cout << "نسبة التحسين / Reduction: " 
                  << reduction << "%" << std::endl;
    }
    
    std::cout << "الدوال المدمجة / Functions Inlined: " 
              << stats_.functions_inlined << std::endl;
    std::cout << "الكود الميت المحذوف / Dead Code Removed: " 
              << stats_.dead_code_eliminated << std::endl;
    std::cout << "الحلقات المفكوكة / Loops Unrolled: " 
              << stats_.loops_unrolled << std::endl;
    std::cout << "وقت التحسين / Optimization Time: " 
              << stats_.optimization_time_ms << " ms" << std::endl;
    
    std::cout << "========================================\n" << std::endl;
    
    // طباعة إحصائيات التحسين العربي / Print Arabic optimization statistics
    if (arabic_optimizer_ && optimization_level_ >= OptimizationLevel::O2) {
        std::cout << "\n";
        arabic_optimizer_->printStats();
    }
}

/**
 * إعادة تعيين الإحصائيات / Reset statistics
 */
void LLVMOptimizer::resetStats() {
    stats_ = OptimizationStats();
}

/**
 * تمكين/تعطيل التحقق بعد كل تمريرة / Enable/disable verification
 */
void LLVMOptimizer::setVerifyEachPass(bool verify) {
    verify_each_pass_ = verify;
}

/**
 * تمكين/تعطيل طباعة الوقت / Enable/disable timing
 */
void LLVMOptimizer::setTimePasses(bool timePasses) {
    time_passes_ = timePasses;
}

/**
 * بناء خط أنابيب التحسين / Build optimization pipeline
 */
void LLVMOptimizer::buildOptimizationPipeline(OptimizationLevel level) {
    // إنشاء مديري تمريرات جدد / Create new pass managers
    module_pm_ = std::make_unique<llvm::ModulePassManager>();
    function_pm_ = std::make_unique<llvm::FunctionPassManager>();
    
    switch (level) {
        case OptimizationLevel::O0:
            // بدون تحسينات / No optimizations
            break;
            
        case OptimizationLevel::O1:
            // تحسينات أساسية / Basic optimizations
            addBasicOptimizations();
            break;
            
        case OptimizationLevel::O2:
            // تحسينات قياسية / Standard optimizations
            addBasicOptimizations();
            addStandardOptimizations();
            break;
            
        case OptimizationLevel::O3:
            // تحسينات عدوانية / Aggressive optimizations
            addBasicOptimizations();
            addStandardOptimizations();
            addAggressiveOptimizations();
            break;
            
        case OptimizationLevel::Os:
            // تحسين للحجم / Size optimization
            addBasicOptimizations();
            addSizeOptimizations(false);
            break;
            
        case OptimizationLevel::Oz:
            // تحسين قوي للحجم / Aggressive size optimization
            addBasicOptimizations();
            addSizeOptimizations(true);
            break;
    }
}

/**
 * إضافة تمريرات التحسين الأساسية (O1) / Add basic optimizations
 */
void LLVMOptimizer::addBasicOptimizations() {
    if (!module_pm_ || !function_pm_) return;
    
    // تمريرات الدالة الأساسية / Basic function passes
    
    // SROA: استبدال البنيات المجمعة بالمتغيرات العددية / Replace aggregates with scalars
    if (isPassEnabled("sroa")) {
        function_pm_->addPass(llvm::SROAPass(llvm::SROAOptions::ModifyCFG));
    }
    
    // SimplifyCFG: تبسيط الرسم البياني للتحكم / Simplify control flow graph
    if (isPassEnabled("simplifycfg")) {
        function_pm_->addPass(llvm::SimplifyCFGPass());
    }
    
    // InstCombine: دمج التعليمات / Combine instructions
    if (isPassEnabled("instcombine")) {
        function_pm_->addPass(llvm::InstCombinePass());
    }
    
    // DCE: إزالة الكود الميت / Dead code elimination
    if (isPassEnabled("dce")) {
        function_pm_->addPass(llvm::DCEPass());
    }
    
    // Mem2Reg: ترقية الذاكرة إلى سجلات / Promote memory to registers
    // هذا Pass مهم جداً لتحسين الأداء / This pass is critical for performance
    if (isPassEnabled("mem2reg")) {
        // Mem2Reg مُضمّن في SROA بشكل أساسي / Mem2Reg is essentially included in SROA
        // لكن نضيف SimplifyCFG مرة أخرى للتنظيف / But we add SimplifyCFG again for cleanup
        function_pm_->addPass(llvm::SimplifyCFGPass());
    }
}

/**
 * إضافة تمريرات التحسين القياسية (O2) / Add standard optimizations
 */
void LLVMOptimizer::addStandardOptimizations() {
    if (!module_pm_ || !function_pm_) return;
    
    // تمريرات على مستوى الوحدة / Module-level passes
    
    // GlobalDCE: إزالة الدوال والمتغيرات العامة غير المستخدمة / Remove unused globals
    if (isPassEnabled("globaldce")) {
        module_pm_->addPass(llvm::GlobalDCEPass());
    }
    
    // تمريرات على مستوى الدالة / Function-level passes
    
    // GVN: ترقيم القيم العامة / Global value numbering
    if (isPassEnabled("gvn")) {
        function_pm_->addPass(llvm::GVNPass());
    }
    
    // LICM: نقل الكود الثابت خارج الحلقات / Loop invariant code motion
    if (isPassEnabled("licm")) {
        // LICM يحتاج loop analysis manager - نستخدم LoopPassManager / LICM needs loop analysis manager
        // يتم إضافته عبر pass builder الذي يربط التحليلات تلقائياً / Added via pass builder with automatic analysis linking
        
        // إنشاء LoopPassManager للحلقات / Create LoopPassManager for loops
        llvm::LoopPassManager loop_pm;
        
        // إضافة LICM للحلقات / Add LICM for loops
        loop_pm.addPass(llvm::LICMPass(llvm::LICMOptions()));
        
        // إضافة LoopPassManager إلى FunctionPassManager / Add LoopPassManager to FunctionPassManager
        function_pm_->addPass(llvm::createFunctionToLoopPassAdaptor(std::move(loop_pm)));
    }
    
    // إعادة تشغيل InstCombine بعد GVN / Re-run InstCombine after GVN
    if (isPassEnabled("instcombine")) {
        function_pm_->addPass(llvm::InstCombinePass());
    }
    
    // إعادة تشغيل SimplifyCFG / Re-run SimplifyCFG
    if (isPassEnabled("simplifycfg")) {
        function_pm_->addPass(llvm::SimplifyCFGPass());
    }
}

/**
 * إضافة تمريرات التحسين العدوانية (O3) / Add aggressive optimizations
 */
void LLVMOptimizer::addAggressiveOptimizations() {
    if (!module_pm_ || !function_pm_) return;
    
    // ============================================================================
    // REMOVED: DeadArgumentEliminationPass - not available in LLVM 15+
    // REMOVED: LoopUnrollPass - API changed in LLVM 15+
    // REMOVED: TailCallElimPass - not available in LLVM 15+
    // TODO: Update to use new LLVM 15+ pass pipeline when needed
    // ============================================================================
    
    // Vectorization: التجميع / Vectorize code
    if (isPassEnabled("vectorize")) {
        // Vectorization passes would be added here
    }
    
    // إعادة تشغيل GVN و InstCombine / Re-run GVN and InstCombine
    if (isPassEnabled("gvn")) {
        function_pm_->addPass(llvm::GVNPass());
    }
    if (isPassEnabled("instcombine")) {
        function_pm_->addPass(llvm::InstCombinePass());
    }
    
    // تحسينات مخصصة للغة العربية / Custom Arabic optimizations
    // يتم تشغيله في نهاية O3 للحصول على أفضل النتائج / Run at end of O3 for best results
    if (arabic_optimizer_) {
        // سيتم إضافته في runModulePasses / Will be added in runModulePasses
    }
}

/**
 * إضافة تمريرات تحسين الحجم / Add size optimizations
 */
void LLVMOptimizer::addSizeOptimizations(bool aggressive) {
    if (!module_pm_ || !function_pm_) return;
    
    // GlobalDCE: إزالة الدوال غير المستخدمة / Remove unused functions
    if (isPassEnabled("globaldce")) {
        module_pm_->addPass(llvm::GlobalDCEPass());
    }
    
    // SimplifyCFG مع تفضيل الحجم / SimplifyCFG with size preference
    if (isPassEnabled("simplifycfg")) {
        function_pm_->addPass(llvm::SimplifyCFGPass());
    }
    
    // InstCombine: دمج التعليمات / Combine instructions
    if (isPassEnabled("instcombine")) {
        function_pm_->addPass(llvm::InstCombinePass());
    }
    
    if (aggressive) {
        // تحسينات إضافية لتقليل الحجم / Additional size optimizations
        // REMOVED: DeadArgumentEliminationPass - not available in LLVM 15+
    }
}

/**
 * تشغيل تمريرات الوحدة / Run module passes
 */
void LLVMOptimizer::runModulePasses(llvm::Module* module) {
    if (!module_pm_ || !module) return;
    
    // تشغيل تمريرات الوحدة / Run module passes
    module_pm_->run(*module, *module_am_);
    
    // تشغيل تحسينات اللغة العربية / Run Arabic optimizations
    // يتم تشغيله بعد التحسينات الأساسية / Run after basic optimizations
    if (arabic_optimizer_ && optimization_level_ >= OptimizationLevel::O2) {
        arabic_optimizer_->run(*module, *module_am_);
    }
    
    if (verify_each_pass_) {
        verifyModule(module);
    }
}

/**
 * تشغيل تمريرات الدالة / Run function passes
 */
void LLVMOptimizer::runFunctionPasses(llvm::Module* module) {
    if (!function_pm_ || !module) return;
    
    // تشغيل تمريرات الدالة على كل دالة / Run function passes on each function
    for (auto& function : *module) {
        if (!function.isDeclaration()) {
            function_pm_->run(function, *function_am_);
            
            if (verify_each_pass_) {
                verifyModule(module);
            }
        }
    }
}

/**
 * جمع إحصائيات الوحدة / Collect module statistics
 */
size_t LLVMOptimizer::collectModuleStats(llvm::Module* module) {
    if (!module) return 0;
    
    size_t instruction_count = 0;
    
    for (auto& function : *module) {
        for (auto& basic_block : function) {
            instruction_count += basic_block.size();
        }
    }
    
    return instruction_count;
}

/**
 * التحقق من الوحدة / Verify module
 */
bool LLVMOptimizer::verifyModule(llvm::Module* module) {
    if (!module) return false;
    
    std::string error_msg;
    llvm::raw_string_ostream error_stream(error_msg);
    
    bool has_errors = llvm::verifyModule(*module, &error_stream);
    
    if (has_errors) {
        std::cerr << "خطأ في التحقق من الوحدة / Module verification failed:" << std::endl;
        std::cerr << error_msg << std::endl;
        return false;
    }
    
    return true;
}

} // namespace sad
