/*
 * بسم الله الرحمن الرحيم
 * ═══════════════════════════════════════════════════════════════════════════
 * ملف: ffi.h
 * الوصف: نظام استدعاء الدوال الخارجية (FFI) للغة ص
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * الغرض من هذا الملف:
 * ──────────────────
 * يوفر نظام FFI (Foreign Function Interface) الذي يسمح لبرامج لغة ص
 * باستدعاء دوال من مكتبات C/C++ خارجية ديناميكياً في وقت التشغيل.
 * يتعامل مع تحميل المكتبات، البحث عن الرموز، تحويل الأنواع،
 * ودعم الـ callbacks (استدعاء دوال ص من كود C).
 *
 * المكونات الرئيسية:
 * ─────────────────
 * - Library: تمثيل مكتبة ديناميكية محملة (.dll/.so/.dylib)
 *   • تحميل وإغلاق المكتبة (dlopen/LoadLibrary)
 *   • البحث عن الرموز (dlsym/GetProcAddress)
 *   • البحث عن الدوال مع إنشاء كائن Function مع التوقيع
 *   • تخزين مؤقت للدوال المحملة لتجنب البحث المتكرر
 *   • ترث enable_shared_from_this لضمان بقاء المكتبة حية
 * - Function: غلاف لدالة خارجية قابلة للاستدعاء
 *   • استدعاء الدالة مع تحويل المعاملات (Sad → C → Sad)
 *   • دعم الدوال متغيرة المعاملات (variadic)
 *   • يحتفظ بمرجع للمكتبة المالكة
 * - Callback: غلاف يسمح لكود C باستدعاء دوال ص
 *   • يُنشئ مؤشر C قابل للاستدعاء من دالة ص
 *   • يدير ذاكرة الغلاف تلقائياً
 * - FFIManager: المدير المركزي (Singleton) لنظام FFI
 *   • تحميل المكتبات وإدارتها
 *   • توفير واجهة مبسطة لاستدعاء الدوال الخارجية
 *   • إدارة الـ callbacks النشطة
 *
 * الموقع في البنية العامة:
 * ───────────────────────
 *   runtime_new/
 *   ├── abi/
 *   │   ├── c_abi.h ← واجهة ABI الأساسية (أنماط الاستدعاء، الأنواع)
 *   │   └── type_marshal.h ← تحويل الأنواع
 *   └── ffi/
 *       └── [ffi.h — هذا الملف] ← استدعاء الدوال الخارجية
 *
 * أمثلة الاستخدام (من كود ص):
 * ──────────────────────────
 *   # تحميل مكتبة رياضيات C واستدعاء cos
 *   متغير مكتبة = حمّل_مكتبة("libm.so")
 *   متغير cos = مكتبة.دالة("cos", عشري, [عشري])
 *   اطبع(cos(3.14))  # -0.9999...
 *
 * الاعتماديات:
 * ──────────
 * - abi/c_abi.h: لأنماط الاستدعاء والأنواع
 * - abi/type_marshal.h: لتحويل القيم بين Sad و C
 * - <dlfcn.h> (Linux/macOS) أو Windows API: لتحميل المكتبات
 *
 * @file runtime/ffi/ffi.h
 * @author SadLang Compiler Team
 * @date December 2025
 * ═══════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_RUNTIME_FFI_H
#define SAD_RUNTIME_FFI_H

#include "abi/c_abi.h"
#include "abi/type_marshal.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace Sad {
namespace FFI {

// ============================================================================
// Forward Declarations / التصريحات المسبقة
// ============================================================================

class Library;
class Function;
class Callback;

// ============================================================================
// Library Handle / مقبض المكتبة
// ============================================================================

/**
 * مكتبة ديناميكية محملة / Loaded dynamic library
 * 
 * (AR) يرث من enable_shared_from_this لأن get_function() يستدعي shared_from_this()
 *      لتمرير مرجع مُدار إلى كائنات Function لضمان بقاء المكتبة حية.
 * (EN) Inherits from enable_shared_from_this because get_function() calls shared_from_this()
 *      to pass a managed reference to Function objects to keep the library alive.
 */
class Library : public std::enable_shared_from_this<Library> {
public:
    Library(const std::string& path);
    ~Library();
    
    /**
     * تحميل المكتبة / Load the library
     */
    bool load();
    
    /**
     * إغلاق المكتبة / Close the library
     */
    void close();
    
    /**
     * البحث عن رمز / Lookup symbol
     */
    void* get_symbol(const std::string& name) const;
    
    /**
     * البحث عن دالة / Lookup function
     */
    std::shared_ptr<Function> get_function(
        const std::string& name,
        const ABI::FunctionSignature& signature
    );
    
    /**
     * الحصول على المسار / Get path
     */
    std::string get_path() const { return path_; }
    
    /**
     * هل المكتبة محملة؟ / Is library loaded?
     */
    bool is_loaded() const { return handle_ != nullptr; }
    
    /**
     * الحصول على المقبض الخام / Get raw handle
     */
    void* get_handle() const { return handle_; }
    
private:
    std::string path_;              // مسار المكتبة
    void* handle_;                  // مقبض النظام
    bool loaded_;                   // حالة التحميل
    
    // Cache of loaded functions / ذاكرة مؤقتة للدوال المحملة
    std::unordered_map<std::string, std::shared_ptr<Function>> functions_;
    
    // Allow FFIManager to access functions_ for cache lookup
    friend class FFIManager;
};

// ============================================================================
// Function Wrapper / غلاف الدالة
// ============================================================================

/**
 * دالة خارجية قابلة للاستدعاء / Callable external function
 */
class Function {
public:
    Function(
        void* ptr,
        const ABI::FunctionSignature& signature,
        std::shared_ptr<Library> lib = nullptr
    );
    
    /**
     * استدعاء الدالة / Call the function
     * 
     * @param args المعاملات بصيغة Sad
     * @return القيمة المرجعة بصيغة Sad
     */
    void* call(const std::vector<void*>& args);
    
    /**
     * استدعاء مع أنواع متغيرة / Call with variadic args
     */
    void* call_variadic(
        const std::vector<void*>& args,
        const std::vector<ABI::TypeInfo>& variadic_types
    );
    
    /**
     * الحصول على التوقيع / Get signature
     */
    const ABI::FunctionSignature& get_signature() const { return signature_; }
    
    /**
     * الحصول على المؤشر الخام / Get raw pointer
     */
    void* get_pointer() const { return ptr_; }
    
    /**
     * الحصول على المكتبة / Get library
     */
    std::shared_ptr<Library> get_library() const { return library_; }
    
private:
    void* ptr_;                             // مؤشر الدالة
    ABI::FunctionSignature signature_;      // التوقيع
    std::shared_ptr<Library> library_;      // المكتبة المالكة
    std::unique_ptr<ABI::CABI> abi_;       // نظام ABI
    std::unique_ptr<ABI::TypeMarshaller> marshaller_; // محول الأنواع
};

// ============================================================================
// Callback Support / دعم Callbacks
// ============================================================================

/**
 * callback من C إلى Sad / C to Sad callback
 */
class Callback {
public:
    using SadFunction = std::function<void*(const std::vector<void*>&)>;
    
    Callback(
        const ABI::FunctionSignature& signature,
        SadFunction sad_func
    );
    
    ~Callback();
    
    /**
     * الحصول على مؤشر C القابل للاستدعاء
     * Get callable C pointer
     */
    void* get_c_pointer() const { return c_wrapper_; }
    
    /**
     * الحصول على التوقيع / Get signature
     */
    const ABI::FunctionSignature& get_signature() const { return signature_; }
    
private:
    ABI::FunctionSignature signature_;  // التوقيع
    SadFunction sad_function_;          // الدالة الأصلية
    void* c_wrapper_;                   // غلاف C
    void* wrapper_memory_;              // ذاكرة الغلاف / Wrapper memory
    size_t wrapper_size_;               // حجم الذاكرة / Memory size
    
    /**
     * إنشاء غلاف C / Create C wrapper
     */
    void* create_c_wrapper();
    
    /**
     * تنظيف الغلاف / Cleanup wrapper
     */
    void destroy_c_wrapper();
    
    /**
     * دالة مساعدة لـ trampoline / Trampoline helper function
     */
    static void* trampoline_helper(void* callback_ptr, ...);
};

// ============================================================================
// FFI Manager / مدير FFI
// ============================================================================

/**
 * النظام الرئيسي لإدارة FFI / Main FFI management system
 */
class FFIManager {
public:
    FFIManager();
    ~FFIManager();
    
    // ========================================================================
    // Library Management / إدارة المكتبات
    // ========================================================================
    
    /**
     * تحميل مكتبة / Load library
     */
    std::shared_ptr<Library> load_library(const std::string& path);
    
    /**
     * إغلاق مكتبة / Close library
     */
    void close_library(const std::string& path);
    
    /**
     * الحصول على مكتبة محملة / Get loaded library
     */
    std::shared_ptr<Library> get_library(const std::string& path) const;
    
    /**
     * إغلاق جميع المكتبات / Close all libraries
     */
    void close_all_libraries();
    
    // ========================================================================
    // Function Loading / تحميل الدوال
    // ========================================================================
    
    /**
     * تحميل دالة من مكتبة / Load function from library
     */
    std::shared_ptr<Function> load_function(
        const std::string& library_path,
        const std::string& function_name,
        const ABI::FunctionSignature& signature
    );
    
    /**
     * البحث عن دالة محملة / Get loaded function
     */
    std::shared_ptr<Function> get_function(
        const std::string& library_path,
        const std::string& function_name
    ) const;
    
    // ========================================================================
    // Callback Management / إدارة Callbacks
    // ========================================================================
    
    /**
     * تسجيل callback / Register callback
     */
    std::shared_ptr<Callback> register_callback(
        const std::string& name,
        const ABI::FunctionSignature& signature,
        Callback::SadFunction sad_func
    );
    
    /**
     * إلغاء تسجيل callback / Unregister callback
     */
    void unregister_callback(const std::string& name);
    
    /**
     * الحصول على callback / Get callback
     */
    std::shared_ptr<Callback> get_callback(const std::string& name) const;
    
    // ========================================================================
    // Direct Calls / استدعاءات مباشرة
    // ========================================================================
    
    /**
     * استدعاء دالة خارجية مباشرة / Call external function directly
     */
    void* call_external(
        const std::string& library_path,
        const std::string& function_name,
        const ABI::FunctionSignature& signature,
        const std::vector<void*>& args
    );
    
    /**
     * استدعاء مع رمز خام / Call with raw symbol
     */
    void* call_symbol(
        void* symbol_ptr,
        const ABI::FunctionSignature& signature,
        const std::vector<void*>& args
    );
    
    // ========================================================================
    // Library Search Paths / مسارات البحث عن المكتبات
    // ========================================================================
    
    /**
     * إضافة مسار بحث / Add search path
     */
    void add_search_path(const std::string& path);
    
    /**
     * إزالة مسار بحث / Remove search path
     */
    void remove_search_path(const std::string& path);
    
    /**
     * الحصول على مسارات البحث / Get search paths
     */
    const std::vector<std::string>& get_search_paths() const { 
        return search_paths_; 
    }
    
    /**
     * البحث عن مكتبة في المسارات / Find library in paths
     */
    std::string find_library(const std::string& name) const;
    
    // ========================================================================
    // Error Handling / معالجة الأخطاء
    // ========================================================================
    
    enum class ErrorCode {
        Success,
        LibraryNotFound,
        LibraryLoadFailed,
        SymbolNotFound,
        CallFailed,
        InvalidSignature,
        CallbackFailed,
        UnknownError
    };
    
    struct Error {
        ErrorCode code;
        std::string message;
        std::string library_path;
        std::string symbol_name;
        
        Error() : code(ErrorCode::Success) {}
        Error(ErrorCode c, const std::string& msg)
            : code(c), message(msg) {}
        
        bool has_error() const { return code != ErrorCode::Success; }
    };
    
    Error get_last_error() const { return last_error_; }
    void clear_error() { last_error_ = Error(); }
    
    // ========================================================================
    // Statistics & Debugging / الإحصائيات والتنقيح
    // ========================================================================
    
    struct Stats {
        size_t libraries_loaded;        // المكتبات المحملة
        size_t functions_loaded;        // الدوال المحملة
        size_t callbacks_registered;    // Callbacks المسجلة
        size_t total_calls;             // مجموع الاستدعاءات
        size_t failed_calls;            // الاستدعاءات الفاشلة
    };
    
    Stats get_stats() const { return stats_; }
    void reset_stats();
    
    /**
     * تفعيل وضع التنقيح / Enable debug mode
     */
    void set_debug_mode(bool enabled) { debug_mode_ = enabled; }
    bool is_debug_mode() const { return debug_mode_; }
    
private:
    // ========================================================================
    // Private Members / الأعضاء الخاصة
    // ========================================================================
    
    // المكتبات المحملة / Loaded libraries
    std::unordered_map<std::string, std::shared_ptr<Library>> libraries_;
    
    // Callbacks المسجلة / Registered callbacks
    std::unordered_map<std::string, std::shared_ptr<Callback>> callbacks_;
    
    // مسارات البحث / Search paths
    std::vector<std::string> search_paths_;
    
    // ABI و Marshaller
    std::unique_ptr<ABI::CABI> abi_;
    std::unique_ptr<ABI::TypeMarshaller> marshaller_;
    
    // Error state / حالة الخطأ
    mutable Error last_error_;
    
    // Statistics / الإحصائيات
    Stats stats_;
    
    // Debug mode / وضع التنقيح
    bool debug_mode_;
    
    // ========================================================================
    // Private Helper Methods / طرق مساعدة خاصة
    // ========================================================================
    
    /**
     * تعيين خطأ / Set error
     */
    void set_error(
        ErrorCode code,
        const std::string& message,
        const std::string& library_path = "",
        const std::string& symbol_name = ""
    );
    
    /**
     * تسجيل معلومات تنقيح / Log debug info
     */
    void log_debug(const std::string& message);
    
    /**
     * تهيئة مسارات البحث الافتراضية / Initialize default search paths
     */
    void initialize_default_paths();
};

// ============================================================================
// Convenience Functions / دوال للراحة
// ============================================================================

/**
 * دالة مساعدة لتحميل واستدعاء دالة مرة واحدة
 * Helper to load and call a function once
 */
void* ffi_call_once(
    const std::string& library_path,
    const std::string& function_name,
    const ABI::FunctionSignature& signature,
    const std::vector<void*>& args
);

/**
 * تحميل مكتبة قياسية / Load standard library
 */
std::shared_ptr<Library> load_std_library(const std::string& name);

/**
 * إنشاء توقيع بسيط / Create simple signature
 */
ABI::FunctionSignature create_signature(
    const std::string& name,
    ABI::SadType return_type,
    const std::vector<ABI::SadType>& param_types,
    bool is_variadic = false
);

} // namespace FFI
} // namespace Sad

#endif // SAD_RUNTIME_FFI_H
