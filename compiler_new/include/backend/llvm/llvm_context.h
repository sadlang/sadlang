// ============================================================================
// llvm_context.h — إدارة سياق LLVM لمحرك التجميع الفوري
// LLVMContextManager — LLVM Context Manager for JIT Engine
// ============================================================================
//
// (AR) هذا الملف يدير سياق LLVM (LLVMContext) والبنية التحتية لـ ORC JIT.
//      يُغلّف إنشاء وإدارة LLVMContext والجلسة التنفيذية (ExecutionSession)
//      التي تعتمد عليها جميع عمليات التجميع الفوري.
//
//      المدير مسؤول عن:
//      1. إنشاء وامتلاك سياق LLVM
//      2. إدارة جلسة التنفيذ ORC JIT  
//      3. إضافة الوحدات المُجمّعة
//      4. البحث عن الدوال المُجمّعة بالاسم
//      5. ضمان سلامة الخيوط (thread-safety) للسياق
//
// (EN) This file manages the LLVM context and ORC JIT infrastructure.
//      Wraps creation and management of LLVMContext and ExecutionSession
//      on which all JIT compilation depends.
//
//      Manager responsibilities:
//      1. Create and own LLVM context
//      2. Manage ORC JIT execution session
//      3. Add compiled modules
//      4. Look up compiled functions by name
//      5. Ensure thread-safety for the context
// ============================================================================

#pragma once

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Error.h>
#include <memory>
#include <string>
#include <mutex>

namespace Sad {
namespace JIT {

// ============================================================================
// (AR) مدير سياق LLVM — يُغلّف LLVMContext وجلسة ORC JIT ويدير دورة حياتهما
//      هذه الفئة هي القلب الذي يربط بين لغة ص ومحرك التجميع LLVM.
//      يوفر واجهة بسيطة لإضافة وحدات LLVM والبحث عن الدوال المُجمّعة.
//
//      مراحل الاستخدام:
//      1. إنشاء الكائن
//      2. استدعاء initialize() مع معمارية الهدف
//      3. إضافة وحدات عبر addModule()
//      4. البحث عن الدوال عبر lookupFunction()
//
// (EN) LLVM Context Manager — Wraps LLVMContext and ORC JIT session
//      This class is the core connecting Sad Language to LLVM compilation.
//      Provides simple API for adding LLVM modules and looking up functions.
//
//      Usage phases:
//      1. Create object
//      2. Call initialize() with target architecture
//      3. Add modules via addModule()
//      4. Look up functions via lookupFunction()
// ============================================================================
class LLVMContextManager {
public:
    LLVMContextManager() = default;
    ~LLVMContextManager() = default;
    
    // ========================================================================
    // (AR) تهيئة المدير — يُنشئ سياق LLVM وجلسة ORC JIT
    //      يجب استدعاؤها مرة واحدة قبل أي عملية أخرى.
    //      تُهيئ أهداف LLVM الأصلية (native targets) وتُنشئ LLJIT.
    //
    //      @param target_triple معمارية الهدف (مثل "x86_64-pc-windows-msvc")
    //      @return true عند النجاح، false عند الفشل (راجع getLastError)
    //
    // (EN) Initialize manager — Creates LLVM context and ORC JIT session
    //      Must be called once before any other operation.
    //      Initializes native LLVM targets and creates LLJIT.
    //
    //      @param target_triple Target architecture (e.g., "x86_64-pc-windows-msvc")
    //      @return true on success, false on failure (check getLastError)
    // ========================================================================
    bool initialize(const std::string& target_triple) {
        try {
            // (AR) تهيئة أهداف LLVM الأصلية — مطلوب لتوليد كود الآلة
            // (EN) Initialize native LLVM targets — required for machine code generation
            llvm::InitializeNativeTarget();
            llvm::InitializeNativeTargetAsmPrinter();
            llvm::InitializeNativeTargetAsmParser();
            
            // (AR) إنشاء سياق LLVM — يملك جميع الأنواع والثوابت
            // (EN) Create LLVM context — owns all types and constants
            context_ = std::make_unique<llvm::LLVMContext>();
            
            // (AR) إنشاء سياق آمن للخيوط — مطلوب لـ ORC JIT
            // (EN) Create thread-safe context — required for ORC JIT
            ts_context_ = std::make_unique<llvm::orc::ThreadSafeContext>(
                std::make_unique<llvm::LLVMContext>()
            );
            
            // (AR) إنشاء LLJIT — محرك التجميع الفوري الأساسي
            //      يتعامل مع إنشاء ExecutionSession وطبقات الكائنات والتجميع
            // (EN) Create LLJIT — the core JIT compilation engine
            //      Handles ExecutionSession, object layers, and compilation layers
            auto jit_builder = llvm::orc::LLJITBuilder();
            auto jit_or_err = jit_builder.create();
            
            if (!jit_or_err) {
                lastError_ = "فشل إنشاء LLJIT / Failed to create LLJIT: " + 
                             llvm::toString(jit_or_err.takeError());
                return false;
            }
            
            jit_ = std::move(*jit_or_err);
            initialized_ = true;
            lastError_.clear();
            return true;
            
        } catch (const std::exception& e) {
            lastError_ = std::string("استثناء أثناء التهيئة / Exception during init: ") + e.what();
            return false;
        }
    }
    
    // ========================================================================
    // (AR) الحصول على مرجع للسياق — يُستخدم في جميع عمليات LLVM
    // (EN) Get context reference — used in all LLVM operations
    // ========================================================================
    llvm::LLVMContext& getContext() { return *context_; }
    const llvm::LLVMContext& getContext() const { return *context_; }
    
    // ========================================================================
    // (AR) الحصول على السياق الآمن للخيوط — مطلوب لإنشاء ThreadSafeModule
    //      يُستخدم عند تمرير وحدات LLVM إلى ORC JIT
    // (EN) Get thread-safe context — required for creating ThreadSafeModule
    //      Used when passing LLVM modules to ORC JIT
    // ========================================================================
    llvm::orc::ThreadSafeContext& getThreadSafeContext() { return *ts_context_; }
    
    // ========================================================================
    // (AR) إضافة وحدة مُجمّعة إلى JIT — تُضاف إلى الطبقة الرئيسية (MainJITDylib)
    //      بعد الإضافة، يمكن البحث عن دوال الوحدة عبر lookupFunction
    //
    //      @param module الوحدة الآمنة للخيوط (يتم نقل ملكيتها)
    //      @return true عند النجاح
    //
    // (EN) Add compiled module to JIT — added to MainJITDylib
    //      After adding, module functions can be found via lookupFunction
    //
    //      @param module Thread-safe module (ownership transferred)
    //      @return true on success
    // ========================================================================
    bool addModule(llvm::orc::ThreadSafeModule module) {
        if (!jit_) {
            lastError_ = "JIT غير مُهيأ / JIT not initialized";
            return false;
        }
        
        auto err = jit_->addIRModule(std::move(module));
        if (err) {
            lastError_ = "فشل إضافة الوحدة / Failed to add module: " + 
                         llvm::toString(std::move(err));
            return false;
        }
        return true;
    }
    
    // ========================================================================
    // (AR) البحث عن دالة مُجمّعة بالاسم — يُرجع مؤشر الدالة المُجمّعة
    //      يبحث في جميع الوحدات المُضافة سابقاً
    //
    //      @param name اسم الدالة
    //      @return مؤشر الدالة أو nullptr إذا لم تُوجد
    //
    // (EN) Look up compiled function by name — returns compiled function pointer
    //      Searches all previously added modules
    //
    //      @param name Function name
    //      @return Function pointer or nullptr if not found
    // ========================================================================
    void* lookupFunction(const std::string& name) {
        if (!jit_) {
            lastError_ = "JIT غير مُهيأ / JIT not initialized";
            return nullptr;
        }
        
        auto sym_or_err = jit_->lookup(name);
        if (!sym_or_err) {
            lastError_ = "فشل البحث عن الدالة / Function lookup failed: " + 
                         llvm::toString(sym_or_err.takeError());
            return nullptr;
        }
        
        // (AR) تحويل عنوان الرمز إلى مؤشر void*
        // (EN) Convert symbol address to void*
        return reinterpret_cast<void*>(sym_or_err->getValue());
    }
    
    // ========================================================================
    // (AR) إعادة تعيين السياق — يُنشئ سياق جديد ويُلغي القديم
    //      ⚠ تحذير: يُبطل جميع القيم المُنشأة بالسياق القديم
    // (EN) Reset context — Creates new context, discards old one
    //      ⚠ Warning: Invalidates ALL values created with old context
    // ========================================================================
    void reset() {
        context_ = std::make_unique<llvm::LLVMContext>();
        ts_context_ = std::make_unique<llvm::orc::ThreadSafeContext>(
            std::make_unique<llvm::LLVMContext>()
        );
    }
    
    bool isValid() const { return context_ != nullptr; }
    bool isInitialized() const { return initialized_; }
    const std::string& getLastError() const { return lastError_; }

private:
    // (AR) السياق الأساسي — لعمليات LLVM العامة
    // (EN) Primary context — for general LLVM operations
    std::unique_ptr<llvm::LLVMContext> context_;
    
    // (AR) السياق الآمن للخيوط — مطلوب لـ ORC JIT
    // (EN) Thread-safe context — required for ORC JIT
    std::unique_ptr<llvm::orc::ThreadSafeContext> ts_context_;
    
    // (AR) محرك LLJIT — يدير التجميع الفوري
    // (EN) LLJIT engine — manages JIT compilation
    std::unique_ptr<llvm::orc::LLJIT> jit_;
    
    std::string lastError_;
    bool initialized_ = false;
};

} // namespace JIT
} // namespace Sad
