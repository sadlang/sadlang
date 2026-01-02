// ============================================================================
// llvm_context.cpp - تطبيق مدير سياق LLVM
// LLVM Context Manager Implementation
// ============================================================================
// المصدر: قراءة من llvm_context.h:1-277
// Source: Read from llvm_context.h:1-277
// ============================================================================

#include "llvm_context.h"

#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Error.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>

namespace Sad {
namespace JIT {

// ============================================================================
// المُنشئ والمُدمر / Constructor & Destructor
// ============================================================================
// المصدر: llvm_context.h:47-56
// Source: llvm_context.h:47-56

LLVMContextManager::LLVMContextManager()
    : ts_context_(nullptr)           // لا يوجد سياق بعد / No context yet
    , context_(nullptr)               // مؤشر فارغ / Null pointer
    , jit_(nullptr)                   // لا يوجد JIT بعد / No JIT yet
    , loaded_modules_()               // خريطة فارغة / Empty map
    , mutex_()                        // mutex افتراضي / Default mutex
    , last_error_()                   // رسالة خطأ فارغة / Empty error
    , initialized_(false)             // غير مُهيأ / Not initialized
    , target_triple_()                // سلسلة فارغة / Empty string
{
    // لا شيء إضافي / Nothing else
}

LLVMContextManager::~LLVMContextManager() {
    // التنظيف التلقائي عبر unique_ptr / Automatic cleanup via unique_ptr
    // سيتم حذف: ts_context_, jit_
    // Will delete: ts_context_, jit_
}

// ============================================================================
// التهيئة / Initialization
// ============================================================================
// المصدر: llvm_context.h:58-89
// Source: llvm_context.h:58-89

bool LLVMContextManager::initialize(const std::string& target_triple) {
    std::lock_guard<std::mutex> lock(mutex_); // حماية من التزامن / Thread safety
    
    if (initialized_) {
        // بالفعل مُهيأ / Already initialized
        return true;
    }
    
    // حفظ معمارية الهدف / Save target triple
    target_triple_ = target_triple;
    
    // الخطوة 1: تهيئة LLVM Targets / Step 1: Initialize LLVM Targets
    // المصدر: initializeTargets() أدناه / Source: initializeTargets() below
    if (!initializeTargets()) {
        logError("Failed to initialize LLVM targets");
        return false;
    }
    
    // الخطوة 2: إنشاء السياق الآمن للخيوط / Step 2: Create thread-safe context
    // المصدر: llvm::orc::ThreadSafeContext من LLVM ORC API
    // Source: llvm::orc::ThreadSafeContext from LLVM ORC API
    ts_context_ = std::make_unique<llvm::orc::ThreadSafeContext>(
        std::make_unique<llvm::LLVMContext>()
    );
    
    // الحصول على مؤشر للسياق الداخلي / Get pointer to internal context
    context_ = ts_context_->getContext();
    
    // الخطوة 3: إنشاء LLJIT / Step 3: Create LLJIT
    // المصدر: createJIT() أدناه / Source: createJIT() below
    if (!createJIT()) {
        logError("Failed to create LLJIT");
        return false;
    }
    
    initialized_ = true;
    return true;
}

bool LLVMContextManager::isInitialized() const {
    // المصدر: initialized_ من llvm_context.h:233
    // Source: initialized_ from llvm_context.h:233
    return initialized_;
}

void LLVMContextManager::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // إعادة تعيين كل شيء / Reset everything
    jit_.reset();
    ts_context_.reset();
    context_ = nullptr;
    loaded_modules_.clear();
    last_error_.clear();
    initialized_ = false;
}

// ============================================================================
// إدارة الوحدات / Module Management
// ============================================================================
// المصدر: llvm_context.h:91-138
// Source: llvm_context.h:91-138

std::unique_ptr<llvm::Module> LLVMContextManager::createModule(const std::string& module_name) {
    if (!initialized_) {
        logError("Context manager not initialized");
        return nullptr;
    }
    
    // إنشاء وحدة LLVM جديدة / Create new LLVM module
    // المصدر: llvm::Module(name, context) من LLVM API
    // Source: llvm::Module(name, context) from LLVM API
    auto module = std::make_unique<llvm::Module>(module_name, *context_);
    
    // تعيين الهدف إذا كان محدداً / Set target if specified
    if (!target_triple_.empty()) {
        module->setTargetTriple(target_triple_);
    }
    
    return module;
}

llvm::orc::ThreadSafeModule LLVMContextManager::createThreadSafeModule(const std::string& module_name) {
    if (!initialized_) {
        logError("Context manager not initialized");
        // إرجاع وحدة فارغة / Return empty module
        return llvm::orc::ThreadSafeModule();
    }
    
    // إنشاء وحدة عادية / Create normal module
    auto module = createModule(module_name);
    
    if (!module) {
        // فشل إنشاء الوحدة / Module creation failed
        return llvm::orc::ThreadSafeModule();
    }
    
    // تحويلها إلى وحدة آمنة للخيوط / Convert to thread-safe module
    // المصدر: llvm::orc::ThreadSafeModule(module, context) من LLVM ORC API
    // Source: llvm::orc::ThreadSafeModule(module, context) from LLVM ORC API
    return llvm::orc::ThreadSafeModule(std::move(module), *ts_context_);
}

bool LLVMContextManager::addModule(llvm::orc::ThreadSafeModule ts_module) {
    if (!initialized_ || !jit_) {
        logError("JIT not initialized");
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // إضافة الوحدة إلى JIT / Add module to JIT
    // المصدر: jit_->addIRModule() من llvm::orc::LLJIT API
    // Source: jit_->addIRModule() from llvm::orc::LLJIT API
    auto err = jit_->addIRModule(std::move(ts_module));
    
    if (err) {
        // حدث خطأ / Error occurred
        // المصدر: llvm::toString(Error) من LLVM API
        // Source: llvm::toString(Error) from LLVM API
        logError("Failed to add module: " + llvm::toString(std::move(err)));
        return false;
    }
    
    return true;
}

void LLVMContextManager::removeModule(const std::string& module_name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // إزالة من الخريطة / Remove from map
    // المصدر: loaded_modules_ من llvm_context.h:223
    // Source: loaded_modules_ from llvm_context.h:223
    loaded_modules_.erase(module_name);
    
    // ملاحظة: LLJIT لا يوفر removeModule مباشرة
    // Note: LLJIT doesn't provide removeModule directly
    // الوحدات تُحذف عند إعادة تهيئة JIT
    // Modules are deleted when JIT is reinitialized
}

// ============================================================================
// الوصول للسياق / Context Access
// ============================================================================
// المصدر: llvm_context.h:140-154
// Source: llvm_context.h:140-154

llvm::LLVMContext* LLVMContextManager::getContext() {
    // المصدر: context_ من llvm_context.h:217
    // Source: context_ from llvm_context.h:217
    return context_;
}

llvm::orc::ThreadSafeContext& LLVMContextManager::getThreadSafeContext() {
    // المصدر: ts_context_ من llvm_context.h:214-215
    // Source: ts_context_ from llvm_context.h:214-215
    return *ts_context_;
}

// ============================================================================
// JIT Execution Engine / محرك تنفيذ JIT
// ============================================================================
// المصدر: llvm_context.h:156-210
// Source: llvm_context.h:156-210

llvm::orc::LLJIT* LLVMContextManager::getJIT() {
    // المصدر: jit_ من llvm_context.h:219-220
    // Source: jit_ from llvm_context.h:219-220
    return jit_.get();
}

uint64_t LLVMContextManager::lookupSymbol(const std::string& symbol_name) {
    if (!initialized_ || !jit_) {
        logError("JIT not initialized");
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // البحث عن الرمز / Lookup symbol
    // المصدر: jit_->lookup() من llvm::orc::LLJIT API
    // Source: jit_->lookup() from llvm::orc::LLJIT API
    auto symbol = jit_->lookup(symbol_name);
    
    if (!symbol) {
        // فشل البحث / Lookup failed
        // المصدر: llvm::toString(Error) من LLVM API
        // Source: llvm::toString(Error) from LLVM API
        logError("Symbol not found: " + symbol_name + " - " + 
                 llvm::toString(symbol.takeError()));
        return 0;
    }
    
    // إرجاع العنوان / Return address
    // المصدر: getValue() من llvm::orc::ExecutorAddr في LLVM 18+
    // Source: getValue() from llvm::orc::ExecutorAddr in LLVM 18+
    // ملاحظة: في LLVM 18، تم تغيير getAddress() إلى getValue()
    // Note: In LLVM 18, getAddress() was changed to getValue()
    return symbol->getValue();
}

void* LLVMContextManager::lookupFunction(const std::string& symbol_name) {
    // البحث عن العنوان / Lookup address
    uint64_t addr = lookupSymbol(symbol_name);
    
    if (addr == 0) {
        return nullptr; // لم يُعثر عليه / Not found
    }
    
    // تحويل العنوان إلى مؤشر / Convert address to pointer
    return reinterpret_cast<void*>(addr);
}

// ============================================================================
// معلومات الأخطاء / Error Information
// ============================================================================
// المصدر: llvm_context.h:212-226
// Source: llvm_context.h:212-226

std::string LLVMContextManager::getLastError() const {
    std::lock_guard<std::mutex> lock(mutex_);
    // المصدر: last_error_ من llvm_context.h:229
    // Source: last_error_ from llvm_context.h:229
    return last_error_;
}

bool LLVMContextManager::hasError() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !last_error_.empty();
}

void LLVMContextManager::clearError() {
    std::lock_guard<std::mutex> lock(mutex_);
    last_error_.clear();
}

// ============================================================================
// دوال مساعدة خاصة / Private Helper Functions
// ============================================================================
// المصدر: llvm_context.h:228-277
// Source: llvm_context.h:228-277

bool LLVMContextManager::initializeTargets() {
    // تهيئة Native Target / Initialize Native Target
    // المصدر: llvm::InitializeNativeTarget() من LLVM Support API
    // Source: llvm::InitializeNativeTarget() from LLVM Support API
    if (llvm::InitializeNativeTarget()) {
        logError("Failed to initialize native target");
        return false;
    }
    
    // تهيئة Native Target AsmPrinter / Initialize Native Target AsmPrinter
    // المصدر: llvm::InitializeNativeTargetAsmPrinter() من LLVM Support API
    // Source: llvm::InitializeNativeTargetAsmPrinter() from LLVM Support API
    if (llvm::InitializeNativeTargetAsmPrinter()) {
        logError("Failed to initialize native target ASM printer");
        return false;
    }
    
    // تهيئة Native Target AsmParser / Initialize Native Target AsmParser
    // المصدر: llvm::InitializeNativeTargetAsmParser() من LLVM Support API
    // Source: llvm::InitializeNativeTargetAsmParser() from LLVM Support API
    if (llvm::InitializeNativeTargetAsmParser()) {
        logError("Failed to initialize native target ASM parser");
        return false;
    }
    
    return true;
}

bool LLVMContextManager::createJIT() {
    // إنشاء LLJIT باستخدام Builder / Create LLJIT using Builder
    // المصدر: llvm::orc::LLJITBuilder من LLVM ORC API
    // Source: llvm::orc::LLJITBuilder from LLVM ORC API
    auto jit_builder = llvm::orc::LLJITBuilder();
    
    // إنشاء JIT / Create JIT
    auto jit_or_err = jit_builder.create();
    
    if (!jit_or_err) {
        // فشل الإنشاء / Creation failed
        // المصدر: llvm::toString(Error) من LLVM API
        // Source: llvm::toString(Error) from LLVM API
        logError("Failed to create LLJIT: " + llvm::toString(jit_or_err.takeError()));
        return false;
    }
    
    // نقل الملكية / Transfer ownership
    jit_ = std::move(*jit_or_err);
    
    return true;
}

void LLVMContextManager::logError(const std::string& message) {
    // تسجيل رسالة الخطأ / Log error message
    // المصدر: last_error_ من llvm_context.h:229
    // Source: last_error_ from llvm_context.h:229
    last_error_ = message;
}

} // namespace JIT
} // namespace Sad

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
