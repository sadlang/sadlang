// تعطيل تحذير Unicode للنصوص العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file ffi_wrapper.h
 * @brief (AR) مولد أغلفة FFI الآمنة - إدارة الذاكرة والموارد تلقائياً
 *        (EN) Safe FFI Wrapper Generator - Automatic Memory and Resource Management
 * 
 * @details
 * (AR) هذا الملف يحتوي على نظام توليد الأغلفة الآمنة لدوال FFI.
 *      الهدف هو تحويل دوال C غير الآمنة إلى دوال "ص" آمنة.
 * 
 *      المميزات الرئيسية:
 *      ┌────────────────────────────────────────────────────────────┐
 *      │ 1. غلاف_آمن (SafeWrapper): RAII للمؤشرات                    │
 *      │ 2. نتيجة<T>: إرجاع نتيجة أو خطأ بدلاً من null              │
 *      │ 3. مدقق_null: فحص المؤشرات قبل الاستخدام                   │
 *      │ 4. محرر_تلقائي: تحرير الذاكرة عند الخروج من النطاق         │
 *      │ 5. تتبع_الملكية: تتبع من يملك المؤشر                       │
 *      │ 6. تحذيرات_التسرب: كشف تسرب الذاكرة                        │
 *      └────────────────────────────────────────────────────────────┘
 * 
 *      مثال الاستخدام:
 *      @code{.ص}
 *      // بدلاً من استخدام malloc/free مباشرة
 *      دالة مثال() {
 *          // غلاف آمن يحرر الذاكرة تلقائياً
 *          ثابت ذاكرة = غلاف_آمن<*ص8>(malloc(100), free)
 *          
 *          // استخدام الذاكرة...
 *          
 *          // لا حاجة لاستدعاء free - يُحرر تلقائياً
 *      }
 *      @endcode
 * 
 * (EN) This file contains the safe wrapper generation system for FFI functions.
 *      The goal is to convert unsafe C functions to safe Sad functions.
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#ifndef SAD_FFI_WRAPPER_H
#define SAD_FFI_WRAPPER_H

#include "ffi/c_abi.h"
#include "types/c_types.h"

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <optional>
#include <unordered_map>

// (AR) DEF-001: متتبع الملكية الموحَّد + سياسات الملكية + AllocationInfo —
//      جميعها في shared/ownership_runtime/. كل التعريفات هنا أصبحت aliases.
// (EN) DEF-001: unified ownership tracker, policy enum, and AllocationInfo
//      now live in shared/ownership_runtime/. The declarations below are aliases.
#include "ownership/runtime/ownership_tracker.h"

namespace Sad {
namespace FFI {

// ============================================================================
//                    (AR) إعادة توجيه الأنواع
//                    (EN) Type Forward Declarations
// ============================================================================

class SafeWrapper;
class WrapperGenerator;
class LeakDetector;

using SafeWrapperPtr = std::shared_ptr<SafeWrapper>;
using DestructorFn = std::function<void(void*)>;

// ============================================================================
//                    (AR) سياسات الملكية — DEF-001 alias
//                    (EN) Ownership Policies — DEF-001 alias
// ============================================================================

// (AR) كان `enum class OwnershipPolicy` معرَّفاً هنا. نُقل إلى
//      shared/ownership_runtime/. نُبقي alias للحفاظ على التوافق الخلفي.
// (EN) Was a local enum; moved by DEF-001. Alias preserves backward compat.
using OwnershipPolicy = ::Sad::Ownership::Runtime::OwnershipPolicy;

// (AR) الدالة المساعدة inline — مرر مباشرة للتطبيق الموحَّد.
// (EN) Inline helper — forwards to the unified implementation.
inline const char* ownershipPolicyToArabic(OwnershipPolicy policy) {
    return ::Sad::Ownership::Runtime::ownershipPolicyToArabic(policy);
}

// ============================================================================
//                    (AR) معلومات الغلاف
//                    (EN) Wrapper Information
// ============================================================================

/**
 * @struct WrapperInfo
 * @brief (AR) معلومات عن غلاف آمن
 *        (EN) Information about safe wrapper
 */
struct WrapperInfo {
    std::string cFunctionName;              // اسم دالة C
    std::string sadWrapperName;             // اسم الغلاف في "ص"
    std::string arabicName;                 // الاسم العربي
    OwnershipPolicy returnPolicy;           // سياسة الإرجاع
    std::string destructorName;             // اسم دالة التحرير
    bool checksNull = true;                 // يفحص null؟
    bool propagatesErrors = true;           // ينقل الأخطاء؟
    std::vector<std::pair<size_t, OwnershipPolicy>> paramPolicies;  // سياسات المعاملات
};

// ============================================================================
//                    (AR) نتيجة<T> - نوع النتيجة الآمن
//                    (EN) Result<T> - Safe Result Type
// ============================================================================

/**
 * @struct FFIError
 * @brief (AR) خطأ FFI
 *        (EN) FFI Error
 */
struct FFIError {
    int code;                   // رمز الخطأ
    std::string message;        // رسالة الخطأ
    std::string arabicMessage;  // الرسالة العربية
    std::string function;       // الدالة التي أنتجت الخطأ
};

/**
 * @class FFIResult
 * @brief (AR) نتيجة عملية FFI - إما قيمة أو خطأ
 *        (EN) FFI operation result - either value or error
 * 
 * @details
 * (AR) بديل آمن لإرجاع مؤشرات null أو رموز خطأ.
 *      يُجبر المبرمج على معالجة حالة الخطأ.
 * 
 * @tparam T (AR) نوع القيمة (EN) Value type
 */
template<typename T>
class FFIResult {
public:
    // (AR) إنشاء نتيجة ناجحة / (EN) Create success result
    static FFIResult success(T value) {
        FFIResult result;
        result.value_ = std::move(value);
        result.hasValue_ = true;
        return result;
    }
    
    // (AR) إنشاء نتيجة خطأ / (EN) Create error result
    static FFIResult error(const FFIError& err) {
        FFIResult result;
        result.error_ = err;
        result.hasValue_ = false;
        return result;
    }
    
    // (AR) إنشاء نتيجة خطأ بسيط / (EN) Create simple error result
    static FFIResult error(const std::string& message) {
        FFIError err;
        err.code = -1;
        err.message = message;
        err.arabicMessage = message;
        return error(err);
    }
    
    // (AR) هل النتيجة ناجحة؟ / (EN) Is result successful?
    bool isSuccess() const { return hasValue_; }
    bool isError() const { return !hasValue_; }
    
    // (AR) الحصول على القيمة / (EN) Get value
    T& getValue() { return value_; }
    const T& getValue() const { return value_; }
    
    // (AR) الحصول على الخطأ / (EN) Get error
    const FFIError& getError() const { return error_; }
    
    // (AR) محول ضمني للـ bool / (EN) Implicit bool conversion
    explicit operator bool() const { return hasValue_; }
    
    // (AR) الوصول للقيمة بـ * / (EN) Access value with *
    T& operator*() { return value_; }
    const T& operator*() const { return value_; }

private:
    T value_;
    FFIError error_;
    bool hasValue_ = false;
};

// ============================================================================
//                    (AR) الغلاف الآمن
//                    (EN) Safe Wrapper
// ============================================================================

/**
 * @class SafeWrapper
 * @brief (AR) غلاف آمن لمؤشر C - يُحرر تلقائياً
 *        (EN) Safe wrapper for C pointer - automatically freed
 * 
 * @details
 * (AR) يستخدم نمط RAII لضمان تحرير الموارد:
 *      - عند الإنشاء: يحفظ المؤشر ودالة التحرير
 *      - عند التدمير: يستدعي دالة التحرير تلقائياً
 *      - يمنع النسخ (لتجنب التحرير المزدوج)
 *      - يدعم النقل (move semantics)
 * 
 * (EN) Uses RAII pattern to ensure resource cleanup:
 *      - On construction: stores pointer and destructor
 *      - On destruction: automatically calls destructor
 *      - Prevents copying (to avoid double-free)
 *      - Supports move semantics
 */
template<typename T>
class SafePointerWrapper {
public:
    /**
     * @brief (AR) بناء غلاف آمن
     *        (EN) Construct safe wrapper
     * 
     * @param ptr (AR) المؤشر (EN) The pointer
     * @param destructor (AR) دالة التحرير (EN) Destructor function
     * @param policy (AR) سياسة الملكية (EN) Ownership policy
     */
    SafePointerWrapper(T* ptr, 
                       std::function<void(T*)> destructor,
                       OwnershipPolicy policy = OwnershipPolicy::OWNED)
        : ptr_(ptr), destructor_(destructor), policy_(policy) {
        /*
         * (AR) تهيئة الغلاف:
         *      - حفظ المؤشر
         *      - حفظ دالة التحرير
         *      - تعيين سياسة الملكية
         * 
         * (EN) Initialize wrapper:
         *      - Store pointer
         *      - Store destructor
         *      - Set ownership policy
         */
    }
    
    /**
     * @brief (AR) بناء غلاف بدالة free الافتراضية
     *        (EN) Construct wrapper with default free function
     */
    explicit SafePointerWrapper(T* ptr)
        : SafePointerWrapper(ptr, [](T* p) { std::free(p); }) {
    }
    
    /**
     * @brief (AR) المدمر - يحرر الذاكرة إذا كانت مملوكة
     *        (EN) Destructor - frees memory if owned
     */
    ~SafePointerWrapper() {
        if (policy_ == OwnershipPolicy::OWNED && ptr_ != nullptr) {
            destructor_(ptr_);
            ptr_ = nullptr;
        }
    }
    
    // (AR) منع النسخ / (EN) Prevent copying
    SafePointerWrapper(const SafePointerWrapper&) = delete;
    SafePointerWrapper& operator=(const SafePointerWrapper&) = delete;
    
    // (AR) السماح بالنقل / (EN) Allow moving
    SafePointerWrapper(SafePointerWrapper&& other) noexcept
        : ptr_(other.ptr_), destructor_(std::move(other.destructor_)), 
          policy_(other.policy_) {
        other.ptr_ = nullptr;
        other.policy_ = OwnershipPolicy::TRANSFERRED;
    }
    
    SafePointerWrapper& operator=(SafePointerWrapper&& other) noexcept {
        if (this != &other) {
            // (AR) تحرير الموجود أولاً
            if (policy_ == OwnershipPolicy::OWNED && ptr_ != nullptr) {
                destructor_(ptr_);
            }
            
            ptr_ = other.ptr_;
            destructor_ = std::move(other.destructor_);
            policy_ = other.policy_;
            
            other.ptr_ = nullptr;
            other.policy_ = OwnershipPolicy::TRANSFERRED;
        }
        return *this;
    }
    
    // --- (AR) الوصول للمؤشر / (EN) Pointer access ---
    
    T* get() const { return ptr_; }
    T* operator->() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    
    explicit operator bool() const { return ptr_ != nullptr; }
    
    // --- (AR) إدارة الملكية / (EN) Ownership management ---
    
    /**
     * @brief (AR) تحرير الملكية (لا يُحرر الذاكرة)
     *        (EN) Release ownership (doesn't free memory)
     */
    T* release() {
        T* p = ptr_;
        ptr_ = nullptr;
        policy_ = OwnershipPolicy::TRANSFERRED;
        return p;
    }
    
    /**
     * @brief (AR) إعادة تعيين المؤشر
     *        (EN) Reset pointer
     */
    void reset(T* newPtr = nullptr) {
        if (policy_ == OwnershipPolicy::OWNED && ptr_ != nullptr) {
            destructor_(ptr_);
        }
        ptr_ = newPtr;
        policy_ = OwnershipPolicy::OWNED;
    }
    
    /**
     * @brief (AR) نقل الملكية لغلاف آخر
     *        (EN) Transfer ownership to another wrapper
     */
    void transferTo(SafePointerWrapper& other) {
        other.reset(release());
    }
    
    OwnershipPolicy getPolicy() const { return policy_; }

private:
    T* ptr_;
    std::function<void(T*)> destructor_;
    OwnershipPolicy policy_;
};

// ============================================================================
//                    (AR) مولد الأغلفة
//                    (EN) Wrapper Generator
// ============================================================================

/**
 * @class WrapperGenerator
 * @brief (AR) مولد كود الأغلفة الآمنة
 *        (EN) Safe wrapper code generator
 * 
 * @details
 * (AR) يقوم بتوليد كود "ص" للأغلفة الآمنة من تصريحات FFI:
 * 
 *      مدخل:
 *      @code{.c}
 *      void* malloc(size_t size);
 *      void free(void* ptr);
 *      @endcode
 * 
 *      مخرج:
 *      @code{.ص}
 *      // غلاف آمن لـ malloc
 *      دالة احجز_ذاكرة_آمن(الحجم: حجم) -> نتيجة<غلاف_آمن<*فراغ>>
 *          ثابت مؤشر = malloc(الحجم)
 *          إذا مؤشر == فارغ {
 *              إرجاع خطأ("فشل حجز الذاكرة")
 *          }
 *          إرجاع نجاح(غلاف_آمن(مؤشر, free))
 *      نهاية
 *      @endcode
 * 
 * (EN) Generates Sad code for safe wrappers from FFI declarations
 */
class WrapperGenerator {
public:
    /**
     * @struct GeneratorConfig
     * @brief (AR) إعدادات المولد
     *        (EN) Generator configuration
     */
    struct Config {
        bool generateNullChecks = true;         // فحص null
        bool generateErrorPropagation = true;   // نقل الأخطاء
        bool generateArabicNames = true;        // أسماء عربية
        bool generateDocumentation = true;      // توليد التوثيق
        bool useResultType = true;              // استخدام نتيجة<T>
        bool generateLeakWarnings = true;       // تحذيرات التسرب
    };
    
    WrapperGenerator();
    explicit WrapperGenerator(const Config& config);
    
    // --- (AR) توليد الأغلفة / (EN) Generate wrappers ---
    
    /**
     * @brief (AR) توليد غلاف لدالة تحجز ذاكرة
     *        (EN) Generate wrapper for memory-allocating function
     * 
     * @param funcDecl (AR) تصريح الدالة (EN) Function declaration
     * @param destructor (AR) اسم دالة التحرير (EN) Destructor function name
     * @return (AR) كود الغلاف (EN) Wrapper code
     */
    std::string generateAllocatorWrapper(const ExternFunctionDecl& funcDecl,
                                         const std::string& destructor = "free");
    
    /**
     * @brief (AR) توليد غلاف لدالة تفتح مورد
     *        (EN) Generate wrapper for resource-opening function
     * 
     * @param funcDecl (AR) تصريح الدالة (EN) Function declaration
     * @param closer (AR) اسم دالة الإغلاق (EN) Closer function name
     */
    std::string generateResourceWrapper(const ExternFunctionDecl& funcDecl,
                                        const std::string& closer);
    
    /**
     * @brief (AR) توليد غلاف عام لدالة
     *        (EN) Generate generic wrapper for function
     */
    std::string generateWrapper(const ExternFunctionDecl& funcDecl,
                               const WrapperInfo& info);
    
    /**
     * @brief (AR) توليد أغلفة لملف كامل
     *        (EN) Generate wrappers for complete file
     */
    std::string generateFileWrappers(const std::vector<ExternFunctionDecl>& functions,
                                     const std::unordered_map<std::string, std::string>& destructorMap);
    
    // --- (AR) الإعدادات / (EN) Configuration ---
    
    void setConfig(const Config& config) { config_ = config; }
    const Config& getConfig() const { return config_; }

private:
    Config config_;
    
    // (AR) دوال مساعدة / (EN) Helper functions
    std::string generateNullCheck(const std::string& varName);
    std::string generateErrorReturn(const std::string& message);
    std::string generateSuccessReturn(const std::string& value);
    std::string generateRAIIWrapper(const std::string& ptr, const std::string& destructor);
    std::string generateDocumentation(const ExternFunctionDecl& funcDecl);
    std::string convertToArabicName(const std::string& cName);
};

// ============================================================================
//                    (AR) متتبع الملكية — موحَّد عبر DEF-001
//                    (EN) Ownership Tracker — unified via DEF-001
// ============================================================================
//
// (AR) كان `OwnershipTracker` مُعرَّفاً هنا. نُقل في DEF-001 إلى
//      shared/ownership_runtime/ مع طبقة C-ABI لفك قفل تفعيل --prod على
//      مخرجات sadc. هنا نُبقي الاسم القديم كـ `using` alias للحفاظ على
//      التوافق الخلفي 100% مع جميع المستهلكين الحاليين (BF-15).
//
// (EN) `OwnershipTracker` previously lived here. DEF-001 moved it to
//      shared/ownership_runtime/ with a C-ABI surface so sadc output can
//      enable --prod. The using-alias keeps full backward compatibility.
// ============================================================================

using OwnershipTracker = ::Sad::Ownership::Runtime::OwnershipTracker;
// (AR) AllocationInfo كان عضواً مُتداخِلاً (OwnershipTracker::AllocationInfo)؛
//      صار في النسخة الموحَّدة struct مستقل في النطاق نفسه.
//      using داخل الصنف غير ممكن من خارجه، لذا نوفّره عبر typedef في الصنف
//      الموحَّد نفسه (بالفعل صنف معاد تعريفه باسم alias). الوصول
//      `OwnershipTracker::AllocationInfo` لا يعمل تلقائياً → نضيف alias في
//      الـ namespace كذلك للحفاظ على القراءة المختصرة.
using AllocationInfo = ::Sad::Ownership::Runtime::AllocationInfo;

// ============================================================================
//                    (AR) كاشف التسرب
//                    (EN) Leak Detector
// ============================================================================

/**
 * @class LeakDetector
 * @brief (AR) كاشف تسرب الذاكرة في وقت الترجمة
 *        (EN) Compile-time memory leak detector
 * 
 * @details
 * (AR) يحلل كود "ص" للكشف عن:
 *      - مؤشرات لا تُحرر
 *      - مسارات كود لا تصل لـ free
 *      - استخدام بعد التحرير المحتمل
 * 
 * (EN) Analyzes Sad code to detect:
 *      - Pointers that are never freed
 *      - Code paths that don't reach free
 *      - Potential use-after-free
 */
class LeakDetector {
public:
    /**
     * @struct LeakWarning
     * @brief (AR) تحذير تسرب
     *        (EN) Leak warning
     */
    struct LeakWarning {
        enum Type {
            POSSIBLE_LEAK,          // تسرب محتمل
            DEFINITE_LEAK,          // تسرب مؤكد
            USE_AFTER_FREE,         // استخدام بعد التحرير
            DOUBLE_FREE,            // تحرير مزدوج
            MISSING_CLEANUP         // تنظيف مفقود في مسار
        };
        
        Type type;
        std::string variableName;
        std::string file;
        int line;
        std::string message;
        std::string arabicMessage;
    };
    
    LeakDetector() = default;
    
    /**
     * @brief (AR) تحليل دالة للكشف عن التسربات
     *        (EN) Analyze function for leaks
     */
    std::vector<LeakWarning> analyzeFunction(const std::string& code);
    
    /**
     * @brief (AR) تحليل ملف كامل
     *        (EN) Analyze complete file
     */
    std::vector<LeakWarning> analyzeFile(const std::string& filePath);
    
    /**
     * @brief (AR) الحصول على تحذيرات بالعربية
     *        (EN) Get warnings in Arabic
     */
    std::string formatWarningsArabic(const std::vector<LeakWarning>& warnings) const;
};

// ============================================================================
//                    (AR) دوال المساعدة العامة
//                    (EN) Public Helper Functions
// ============================================================================

/**
 * @brief (AR) إنشاء غلاف آمن بسيط
 *        (EN) Create simple safe wrapper
 */
template<typename T>
SafePointerWrapper<T> makeSafeWrapper(T* ptr, std::function<void(T*)> destructor) {
    return SafePointerWrapper<T>(ptr, destructor);
}

/**
 * @brief (AR) إنشاء غلاف آمن مع free
 *        (EN) Create safe wrapper with free
 */
template<typename T>
SafePointerWrapper<T> makeSafeWrapper(T* ptr) {
    return SafePointerWrapper<T>(ptr, [](T* p) { std::free(p); });
}

/**
 * @brief (AR) إنشاء نتيجة ناجحة
 *        (EN) Create success result
 */
template<typename T>
FFIResult<T> makeSuccess(T value) {
    return FFIResult<T>::success(std::move(value));
}

/**
 * @brief (AR) إنشاء نتيجة خطأ
 *        (EN) Create error result
 */
template<typename T>
FFIResult<T> makeError(const std::string& message) {
    return FFIResult<T>::error(message);
}

} // namespace FFI
} // namespace Sad

#endif // SAD_FFI_WRAPPER_H
