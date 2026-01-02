// ============================================================================
// llvm_context.h - مدير سياق LLVM للـ JIT Engine
// LLVM Context Manager for JIT Engine
// ============================================================================
// الغرض: إدارة سياق LLVM والموارد المرتبطة به
// Purpose: Manage LLVM context and associated resources
// ============================================================================

#ifndef SAD_LLVM_CONTEXT_H
#define SAD_LLVM_CONTEXT_H

#include <memory>
#include <string>
#include <unordered_map>
#include <mutex>

// LLVM Headers / رؤوس LLVM
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>

namespace Sad {
namespace JIT {

// ============================================================================
// LLVMContext - مدير سياق LLVM
// LLVMContext - LLVM Context Manager
// ============================================================================

/**
 * مدير سياق LLVM يوفر:
 * LLVM context manager provides:
 * 
 * 1. إدارة السياق الآمن للخيوط / Thread-safe context management
 * 2. إنشاء وحدات LLVM / LLVM module creation
 * 3. التكامل مع LLVM ORC JIT / Integration with LLVM ORC JIT
 * 4. تخزين مؤقت للوحدات / Module caching
 * 
 * الاستخدام / Usage:
 *   ```cpp
 *   LLVMContextManager ctx_mgr;
 *   ctx_mgr.initialize();
 *   auto module = ctx_mgr.createModule("my_module");
 *   ```
 */
class LLVMContextManager {
public:
    // ========================================================================
    // المُنشئ والمُدمر / Constructor & Destructor
    // ========================================================================
    
    /**
     * المُنشئ / Constructor
     */
    LLVMContextManager();
    
    /**
     * المُدمر / Destructor
     */
    ~LLVMContextManager();
    
    // منع النسخ / Prevent copying
    LLVMContextManager(const LLVMContextManager&) = delete;
    LLVMContextManager& operator=(const LLVMContextManager&) = delete;
    
    // ========================================================================
    // التهيئة / Initialization
    // ========================================================================
    
    /**
     * تهيئة مدير السياق / Initialize context manager
     * 
     * @param target_triple معمارية الهدف (اختياري) / Target triple (optional)
     * @return true إذا نجحت التهيئة / true if initialization succeeded
     * 
     * الخطوات / Steps:
     *   1. إنشاء سياق LLVM / Create LLVM context
     *   2. تهيئة LLVM Native Target / Initialize LLVM Native Target
     *   3. إنشاء LLJIT / Create LLJIT
     */
    bool initialize(const std::string& target_triple = "");
    
    /**
     * هل المدير مُهيأ؟ / Is manager initialized?
     */
    bool isInitialized() const;
    
    /**
     * إعادة تهيئة المدير / Reinitialize manager
     */
    void reset();
    
    // ========================================================================
    // إدارة الوحدات / Module Management
    // ========================================================================
    
    /**
     * إنشاء وحدة LLVM جديدة / Create new LLVM module
     * 
     * @param module_name اسم الوحدة / Module name
     * @return وحدة LLVM جديدة / New LLVM module
     * 
     * المصدر: يستخدم context_ لإنشاء Module
     * Source: Uses context_ to create Module
     */
    std::unique_ptr<llvm::Module> createModule(const std::string& module_name);
    
    /**
     * إنشاء وحدة آمنة للخيوط / Create thread-safe module
     * 
     * @param module_name اسم الوحدة / Module name
     * @return وحدة آمنة للخيوط / Thread-safe module
     * 
     * المصدر: يستخدم ThreadSafeContext و createModule
     * Source: Uses ThreadSafeContext and createModule
     */
    llvm::orc::ThreadSafeModule createThreadSafeModule(const std::string& module_name);
    
    /**
     * إضافة وحدة إلى JIT / Add module to JIT
     * 
     * @param ts_module الوحدة الآمنة للخيوط / Thread-safe module
     * @return true إذا نجحت الإضافة / true if addition succeeded
     * 
     * المصدر: يستخدم jit_->addIRModule() من LLJIT
     * Source: Uses jit_->addIRModule() from LLJIT
     */
    bool addModule(llvm::orc::ThreadSafeModule ts_module);
    
    /**
     * حذف وحدة من الذاكرة / Remove module from memory
     * 
     * @param module_name اسم الوحدة / Module name
     */
    void removeModule(const std::string& module_name);
    
    // ========================================================================
    // الوصول للسياق / Context Access
    // ========================================================================
    
    /**
     * الحصول على سياق LLVM / Get LLVM context
     * 
     * @return مؤشر لسياق LLVM / Pointer to LLVM context
     */
    llvm::LLVMContext* getContext();
    
    /**
     * الحصول على السياق الآمن للخيوط / Get thread-safe context
     * 
     * @return السياق الآمن للخيوط / Thread-safe context
     */
    llvm::orc::ThreadSafeContext& getThreadSafeContext();
    
    // ========================================================================
    // JIT Execution Engine / محرك تنفيذ JIT
    // ========================================================================
    
    /**
     * الحصول على LLJIT / Get LLJIT
     * 
     * @return مؤشر لـ LLJIT / Pointer to LLJIT
     */
    llvm::orc::LLJIT* getJIT();
    
    /**
     * البحث عن رمز (دالة) / Lookup symbol (function)
     * 
     * @param symbol_name اسم الرمز / Symbol name
     * @return عنوان الرمز / Symbol address (أو 0 عند الفشل / or 0 on failure)
     * 
     * المصدر: يستخدم jit_->lookup() من LLJIT
     * Source: Uses jit_->lookup() from LLJIT
     * 
     * مثال / Example:
     *   ```cpp
     *   auto addr = ctx_mgr.lookupSymbol("my_function");
     *   if (addr) {
     *       auto func = reinterpret_cast<void(*)()>(addr);
     *       func(); // تنفيذ الدالة / Execute function
     *   }
     *   ```
     */
    uint64_t lookupSymbol(const std::string& symbol_name);
    
    /**
     * البحث عن رمز وإرجاع مؤشر دالة / Lookup symbol and return function pointer
     * 
     * @param symbol_name اسم الرمز / Symbol name
     * @return مؤشر دالة / Function pointer (أو nullptr عند الفشل / or nullptr on failure)
     * 
     * ملاحظة: يجب تحويل النوع (cast) حسب توقيع الدالة
     * Note: Must cast according to function signature
     */
    void* lookupFunction(const std::string& symbol_name);
    
    // ========================================================================
    // معلومات الأخطاء / Error Information
    // ========================================================================
    
    /**
     * الحصول على رسالة الخطأ الأخيرة / Get last error message
     */
    std::string getLastError() const;
    
    /**
     * هل حدث خطأ؟ / Did an error occur?
     */
    bool hasError() const;
    
    /**
     * مسح رسالة الخطأ / Clear error message
     */
    void clearError();

private:
    // ========================================================================
    // الحقول الخاصة / Private Fields
    // ========================================================================
    
    // السياق الآمن للخيوط / Thread-safe context
    // المصدر: llvm::orc::ThreadSafeContext من LLVM ORC
    // Source: llvm::orc::ThreadSafeContext from LLVM ORC
    std::unique_ptr<llvm::orc::ThreadSafeContext> ts_context_;
    
    // مؤشر للسياق الداخلي / Pointer to internal context
    llvm::LLVMContext* context_;
    
    // محرك JIT / JIT engine
    // المصدر: llvm::orc::LLJIT من LLVM ORC v2
    // Source: llvm::orc::LLJIT from LLVM ORC v2
    std::unique_ptr<llvm::orc::LLJIT> jit_;
    
    // الوحدات المُحملة / Loaded modules
    // key: اسم الوحدة / module name
    // value: معرف الوحدة / module identifier
    std::unordered_map<std::string, uint64_t> loaded_modules_;
    
    // mutex للحماية من التزامن / Mutex for thread safety
    mutable std::mutex mutex_;
    
    // رسالة الخطأ الأخيرة / Last error message
    std::string last_error_;
    
    // حالة التهيئة / Initialization state
    bool initialized_;
    
    // معمارية الهدف / Target triple
    std::string target_triple_;
    
    // ========================================================================
    // دوال مساعدة خاصة / Private Helper Functions
    // ========================================================================
    
    /**
     * تهيئة LLVM Targets / Initialize LLVM Targets
     * 
     * المصدر: يستدعي InitializeNativeTarget() و InitializeNativeTargetAsmPrinter()
     * Source: Calls InitializeNativeTarget() and InitializeNativeTargetAsmPrinter()
     */
    bool initializeTargets();
    
    /**
     * إنشاء LLJIT / Create LLJIT
     * 
     * المصدر: يستخدم LLJITBuilder من llvm::orc
     * Source: Uses LLJITBuilder from llvm::orc
     */
    bool createJIT();
    
    /**
     * تسجيل خطأ / Log error
     * 
     * @param message رسالة الخطأ / Error message
     */
    void logError(const std::string& message);
};

} // namespace JIT
} // namespace Sad

#endif // SAD_LLVM_CONTEXT_H

// ============================================================================
// نهاية الملف / End of File
// ============================================================================
