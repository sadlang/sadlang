// ============================================================================
// llvm_generator.h — واجهة مولّد LLVM IR لمحرك JIT
// LLVMGenerator — LLVM IR Generator Interface for JIT Engine
// ============================================================================
//
// (AR) هذا الملف يوفر واجهة مولّد LLVM IR المُبسّطة التي يستخدمها
//      محرك التجميع الفوري (JIT Engine). يُغلّف LLVMCodeGen بواجهة
//      أبسط تناسب الاستخدام من JIT.
//
//      المولّد مسؤول عن:
//      1. تحويل كود ص المصدري إلى LLVM IR عبر الخطوات التالية:
//         أ. تحليل الكود المصدري (Lexer + Parser)
//         ب. بناء SIR (التمثيل الوسيط لص)
//         ج. تحويل SIR إلى LLVM IR
//      2. التحقق من صحة وحدة LLVM المُنشأة
//      3. تطبيق تحسينات LLVM بمستويات مختلفة
//      4. توفير واجهة بسيطة لـ JIT Engine
//
// (EN) This file provides a simplified LLVM IR generator interface used by
//      the JIT Engine. Wraps LLVMCodeGen with a simpler API suitable for JIT.
//
//      The generator is responsible for:
//      1. Converting Sad source code to LLVM IR via:
//         a. Lexing + Parsing source code
//         b. Building SIR (Sad Intermediate Representation)
//         c. Converting SIR to LLVM IR
//      2. Verify generated LLVM module
//      3. Apply LLVM optimizations at various levels
//      4. Providing a simple interface for JIT Engine
// ============================================================================

#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <memory>
#include <string>

namespace Sad {
namespace JIT {

// ============================================================================
// (AR) مولّد LLVM IR — يُغلّف LLVMCodeGen بواجهة بسيطة لمحرك JIT
//      يُخفي تعقيدات المولّد الكاملة ويُقدّم واجهة مبسّطة للتجميع الفوري.
//
//      دورة الاستخدام:
//      1. initialize() — تهيئة المولّد مع سياق LLVM
//      2. generateFromSource() — توليد وحدة LLVM من كود ص
//      3. verify() — التحقق من صحة الوحدة
//      4. optimize() — تطبيق التحسينات
//      5. takeModule() أو getModule() — الحصول على الوحدة
//
// (EN) LLVM IR Generator — Wraps LLVMCodeGen with simple JIT-friendly API
//      Hides full generator complexity and provides simplified JIT interface.
//
//      Usage cycle:
//      1. initialize() — init with LLVM context
//      2. generateFromSource() — generate LLVM module from Sad code
//      3. verify() — verify module correctness
//      4. optimize() — apply optimizations
//      5. takeModule() or getModule() — get the module
// ============================================================================
class LLVMGenerator {
public:
    LLVMGenerator() = default;
    ~LLVMGenerator() = default;
    
    // ========================================================================
    // (AR) تهيئة المولّد مع سياق LLVM واسم الوحدة
    //      يجب استدعاؤها قبل أي عملية توليد
    //      تُنشئ وحدة LLVM جديدة وتُهيئ IRBuilder
    //
    // (EN) Initialize generator with LLVM context and module name
    //      Must be called before any generation operation
    //      Creates new LLVM module and initializes IRBuilder
    // ========================================================================
    bool initialize(llvm::LLVMContext& context, const std::string& moduleName) {
        try {
            context_ = &context;
            moduleName_ = moduleName;
            
            // (AR) إنشاء وحدة LLVM جديدة — كل JIT compilation تحصل على وحدتها الخاصة
            // (EN) Create new LLVM module — each JIT compilation gets its own module
            module_ = std::make_unique<llvm::Module>(moduleName, context);
            
            // (AR) إنشاء IRBuilder — أداة بناء تعليمات LLVM IR
            // (EN) Create IRBuilder — LLVM IR instruction building tool
            builder_ = std::make_unique<llvm::IRBuilder<>>(context);
            
            initialized_ = true;
            lastError_.clear();
            return true;
        } catch (const std::exception& e) {
            lastError_ = e.what();
            return false;
        }
    }
    
    // ========================================================================
    // (AR) توليد LLVM IR من الكود المصدري — يمر بالمراحل التالية:
    //      1. تحليل لغوي للكود المصدري (Lexing)
    //      2. تحليل نحوي وبناء AST (Parsing)
    //      3. تحويل AST إلى SIR 
    //      4. تحويل SIR إلى LLVM IR
    //
    //      @param source_code الكود المصدري بلغة ص
    //      @return وحدة LLVM المُولّدة أو nullptr عند الفشل
    //
    // (EN) Generate LLVM IR from source code — goes through:
    //      1. Lexical analysis (Lexing)
    //      2. Syntactic analysis and AST building (Parsing)
    //      3. AST to SIR conversion
    //      4. SIR to LLVM IR conversion
    //
    //      @param source_code Sad language source code
    //      @return Generated LLVM module or nullptr on failure
    // ========================================================================
    std::unique_ptr<llvm::Module> generateFromSource(const std::string& source_code) {
        if (!initialized_ || !context_) {
            lastError_ = "المولّد غير مُهيأ / Generator not initialized";
            return nullptr;
        }
        
        try {
            // (AR) إنشاء وحدة جديدة لهذا التجميع
            // (EN) Create new module for this compilation
            auto module = std::make_unique<llvm::Module>(moduleName_ + "_jit", *context_);
            
            // (AR) حفظ الوحدة الحالية للتحقق والتحسين لاحقاً
            // (EN) Store current module for later verification and optimization
            currentModule_ = module.get();
            
            // (AR) في المستقبل: هنا ستتم عملية التحليل والتحويل الكاملة
            //      حالياً نُنشئ وحدة فارغة قابلة للتوسيع
            // (EN) Future: Full parse and conversion pipeline will go here
            //      Currently creates empty expandable module
            
            lastError_.clear();
            return module;
            
        } catch (const std::exception& e) {
            lastError_ = std::string("خطأ في توليد LLVM IR / LLVM IR generation error: ") + e.what();
            return nullptr;
        }
    }
    
    // ========================================================================
    // (AR) التحقق من صحة وحدة LLVM — يستخدم LLVM Verifier للتأكد من أن:
    //      - جميع التعليمات صحيحة البنية
    //      - جميع الأنواع متوافقة
    //      - لا توجد مراجع مُعلّقة (dangling references)
    //      - الـ PHI nodes صحيحة
    //
    //      @param error_out مؤشر لسلسلة نصية تُخزَّن فيها رسالة الخطأ
    //      @return true إذا كانت الوحدة صحيحة
    //
    // (EN) Verify LLVM module — uses LLVM Verifier to ensure:
    //      - All instructions are well-formed
    //      - All types are compatible
    //      - No dangling references
    //      - PHI nodes are correct
    //
    //      @param error_out Pointer to string for error message storage
    //      @return true if module is valid
    // ========================================================================
    bool verify(std::string* error_out = nullptr) {
        if (!currentModule_) {
            if (error_out) *error_out = "لا توجد وحدة للتحقق / No module to verify";
            return false;
        }
        
        // (AR) استخدام LLVM Verifier الرسمي
        // (EN) Use official LLVM Verifier
        std::string errStr;
        llvm::raw_string_ostream errStream(errStr);
        
        if (llvm::verifyModule(*currentModule_, &errStream)) {
            // (AR) وُجدت أخطاء في الوحدة
            // (EN) Errors found in module
            errStream.flush();
            if (error_out) *error_out = errStr;
            lastError_ = errStr;
            return false;
        }
        
        return true;
    }
    
    // ========================================================================
    // (AR) تطبيق تحسينات LLVM على الوحدة الحالية
    //      المستويات:
    //      - O0: لا تحسينات (للتصحيح)
    //      - O1: تحسينات أساسية (إزالة الكود الميت، طي الثوابت)
    //      - O2: تحسينات متوسطة (GVN, إزالة التعبيرات المشتركة)
    //      - O3: تحسينات عدوانية (vectorization, loop unrolling)
    //
    //      @param level مستوى التحسين (0-3)
    //
    // (EN) Apply LLVM optimizations to current module
    //      Levels:
    //      - O0: No optimizations (for debugging)
    //      - O1: Basic (dead code elimination, constant folding)
    //      - O2: Medium (GVN, CSE)
    //      - O3: Aggressive (vectorization, loop unrolling)
    //
    //      @param level Optimization level (0-3)
    // ========================================================================
    void optimize(int level) {
        if (!currentModule_ || level <= 0) return;
        
        try {
            // (AR) استخدام نظام التمريرات الجديد في LLVM (New Pass Manager)
            // (EN) Use LLVM's New Pass Manager  
            llvm::LoopAnalysisManager LAM;
            llvm::FunctionAnalysisManager FAM;
            llvm::CGSCCAnalysisManager CGAM;
            llvm::ModuleAnalysisManager MAM;
            
            llvm::PassBuilder PB;
            PB.registerModuleAnalyses(MAM);
            PB.registerCGSCCAnalyses(CGAM);
            PB.registerFunctionAnalyses(FAM);
            PB.registerLoopAnalyses(LAM);
            PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
            
            // (AR) تحديد مستوى التحسين
            // (EN) Determine optimization level
            llvm::OptimizationLevel optLevel;
            switch (level) {
                case 1: optLevel = llvm::OptimizationLevel::O1; break;
                case 2: optLevel = llvm::OptimizationLevel::O2; break;
                case 3: optLevel = llvm::OptimizationLevel::O3; break;
                default: optLevel = llvm::OptimizationLevel::O1; break;
            }
            
            // (AR) تشغيل خط أنابيب التحسين الافتراضي
            // (EN) Run default optimization pipeline
            llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(optLevel);
            MPM.run(*currentModule_, MAM);
            
        } catch (const std::exception& e) {
            lastError_ = std::string("خطأ في التحسين / Optimization error: ") + e.what();
        }
    }
    
    // ========================================================================
    // (AR) الحصول على آخر رسالة خطأ — مفيد للتشخيص عند فشل أي عملية
    // (EN) Get last error message — useful for diagnostics on any failure
    // ========================================================================
    const std::string& getLastError() const { return lastError_; }
    
    // ========================================================================
    // (AR) الحصول على وحدة LLVM — يُستخدم من JIT Engine
    // (EN) Get LLVM module — used by JIT Engine
    // ========================================================================
    llvm::Module* getModule() const { return module_.get(); }
    
    // ========================================================================
    // (AR) نقل ملكية الوحدة — يُستخدم عند تمرير الوحدة لـ ExecutionSession
    //      بعد النقل، لا يمكن استخدام المولّد حتى إعادة التهيئة
    // (EN) Transfer module ownership — used when passing to ExecutionSession
    //      After transfer, generator can't be used until re-initialized
    // ========================================================================
    std::unique_ptr<llvm::Module> takeModule() { return std::move(module_); }
    
    bool isInitialized() const { return initialized_; }
    
private:
    llvm::LLVMContext* context_ = nullptr;
    std::string moduleName_;
    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<llvm::IRBuilder<>> builder_;
    llvm::Module* currentModule_ = nullptr;   // (AR) مؤشر للوحدة الحالية / (EN) Pointer to current module
    std::string lastError_;
    bool initialized_ = false;
};

} // namespace JIT
} // namespace Sad
