// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4819)
#endif

/**
 * @file unsafe_checker.h
 * @brief رأس فاحص الكود غير الآمن / Unsafe Code Checker Header
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety
 * 
 * =============================================================================
 *                         الكود غير الآمن في لغة ص
 *                         Unsafe Code in Sad Language
 * =============================================================================
 * 
 * (AR) نظام الكود غير الآمن في لغة ص:
 * =====================================
 * 
 *     لغة ص آمنة افتراضياً، لكنها تسمح بكتابة كود "غير آمن" ضمن
 *     كتل `غير_آمن` المحددة. هذا مشابه لـ Rust unsafe blocks.
 * 
 *     **متى نحتاج الكود غير الآمن؟**
 *     
 *     1. التعامل المباشر مع المؤشرات الخام
 *     2. استدعاء دوال نظام التشغيل (FFI)
 *     3. تحسينات الأداء المتقدمة
 *     4. التعامل مع الذاكرة الخام
 * 
 *     **مثال:**
 *     ```sad
 *     غير_آمن {
 *         دع مؤشر: *مغير عدد = &مغير قيمة
 *         *مؤشر = 42
 *     }
 *     ```
 * 
 * (EN) Unsafe Code System in Sad Language:
 * ========================================
 * 
 *     Sad is safe by default, but allows writing "unsafe" code within
 *     designated `unsafe` blocks. Similar to Rust's unsafe blocks.
 * 
 *     **When do we need unsafe code?**
 *     
 *     1. Direct raw pointer manipulation
 *     2. OS system calls (FFI)
 *     3. Advanced performance optimizations
 *     4. Raw memory handling
 * 
 *     **Example:**
 *     ```sad
 *     unsafe {
 *         let ptr: *mut i32 = &mut value
 *         *ptr = 42
 *     }
 *     ```
 * 
 * =============================================================================
 *                    العمليات غير الآمنة / Unsafe Operations
 * =============================================================================
 * 
 * (AR) العمليات التي تتطلب كتلة `غير_آمن`:
 * 
 *     ┌─────────────────────────────────────────────────────────────────────┐
 *     │  العملية              │ الوصف                                       │
 *     ├─────────────────────────────────────────────────────────────────────┤
 *     │  *مؤشر_خام            │ إلغاء مرجعية المؤشر الخام                   │
 *     │  مؤشر.إزاحة()         │ حساب عناوين المؤشرات                        │
 *     │  استدعاء_خارجي()      │ استدعاء دوال C/FFI                          │
 *     │  تحويل_خام()          │ تحويل بين أنواع غير متوافقة                  │
 *     │  الوصول_لـ_static     │ الوصول لمتغيرات ثابتة قابلة للتغيير          │
 *     │  union                │ الوصول لحقول الاتحادات                      │
 *     └─────────────────────────────────────────────────────────────────────┘
 * 
 * (EN) Operations requiring `unsafe` block:
 * 
 *     ┌─────────────────────────────────────────────────────────────────────┐
 *     │  Operation           │ Description                                  │
 *     ├─────────────────────────────────────────────────────────────────────┤
 *     │  *raw_ptr            │ Dereferencing raw pointer                    │
 *     │  ptr.offset()        │ Pointer arithmetic                           │
 *     │  extern_call()       │ Calling C/FFI functions                      │
 *     │  transmute()         │ Type transmutation                           │
 *     │  static_access       │ Accessing mutable statics                    │
 *     │  union               │ Accessing union fields                       │
 *     └─────────────────────────────────────────────────────────────────────┘
 */

#ifndef SAD_SEMANTIC_UNSAFE_CHECKER_H
#define SAD_SEMANTIC_UNSAFE_CHECKER_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <set>

namespace Sad {
namespace Semantic {

// ============================================================================
//                    موقع في الكود المصدري / Source Location
// ============================================================================

/**
 * @struct SourceLocation
 * @brief موقع في الكود المصدري / Location in source code
 */
struct SourceLocation {
    std::string filePath;   // مسار الملف
    size_t line = 0;        // رقم السطر
    size_t column = 0;      // رقم العمود
    size_t endLine = 0;     // نهاية السطر
    size_t endColumn = 0;   // نهاية العمود
    
    std::string toString() const {
        return filePath + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

// ============================================================================
//                    أنواع العمليات غير الآمنة / Unsafe Operation Types
// ============================================================================

/**
 * @enum UnsafeOperationKind
 * @brief أنواع العمليات غير الآمنة / Types of unsafe operations
 * 
 * (AR) تعداد يُحدد جميع العمليات التي تتطلب كتلة غير_آمن
 * (EN) Enumeration of all operations requiring unsafe block
 */
enum class UnsafeOperationKind {
    /**
     * (AR) إلغاء مرجعية مؤشر خام: *مؤشر
     * (EN) Raw pointer dereference: *ptr
     */
    RawPointerDeref,
    
    /**
     * (AR) حساب عنوان المؤشر: مؤشر.إزاحة(5)
     * (EN) Pointer arithmetic: ptr.offset(5)
     */
    PointerArithmetic,
    
    /**
     * (AR) استدعاء دالة خارجية (FFI)
     * (EN) Foreign function call (FFI)
     */
    FFICall,
    
    /**
     * (AR) تحويل الذاكرة الخام: تحويل_خام<من، إلى>()
     * (EN) Raw memory transmute: transmute<From, To>()
     */
    Transmute,
    
    /**
     * (AR) الوصول لمتغير ثابت قابل للتغيير
     * (EN) Mutable static variable access
     */
    MutableStaticAccess,
    
    /**
     * (AR) الوصول لحقول الاتحاد
     * (EN) Union field access
     */
    UnionFieldAccess,
    
    /**
     * (AR) تنفيذ سمة غير آمنة
     * (EN) Implementing unsafe trait
     */
    UnsafeTraitImpl,
    
    /**
     * (AR) استدعاء دالة غير آمنة
     * (EN) Calling unsafe function
     */
    UnsafeFnCall,
    
    /**
     * (AR) التعامل مع الذاكرة الخام
     * (EN) Raw memory manipulation
     */
    RawMemoryOp,
    
    /**
     * (AR) تحويل بين أنواع مؤشرات مختلفة
     * (EN) Pointer type casting
     */
    PointerCast
};

// ============================================================================
//                    معلومات العملية غير الآمنة / Unsafe Operation Info
// ============================================================================

/**
 * @struct UnsafeOperation
 * @brief معلومات عملية غير آمنة مكتشفة / Information about detected unsafe operation
 * 
 * (AR) يُخزن تفاصيل العملية غير الآمنة المكتشفة
 * (EN) Stores details of a detected unsafe operation
 */
struct UnsafeOperation {
    /// (AR) نوع العملية / (EN) Operation type
    UnsafeOperationKind kind;
    
    /// (AR) وصف العملية / (EN) Operation description
    std::string description;
    
    /// (AR) موقع العملية في الكود / (EN) Location in source code
    SourceLocation location;
    
    /// (AR) هل العملية داخل كتلة غير_آمن؟ / (EN) Is operation inside unsafe block?
    bool isInsideUnsafeBlock = false;
    
    /// (AR) اسم الدالة المحتوية / (EN) Containing function name
    std::string containingFunction;
    
    /**
     * @brief الحصول على اسم العملية بالعربية / Get Arabic operation name
     */
    std::string getArabicName() const {
        switch (kind) {
            case UnsafeOperationKind::RawPointerDeref:
                return "إلغاء مرجعية مؤشر خام";
            case UnsafeOperationKind::PointerArithmetic:
                return "حساب عناوين المؤشرات";
            case UnsafeOperationKind::FFICall:
                return "استدعاء دالة خارجية";
            case UnsafeOperationKind::Transmute:
                return "تحويل ذاكرة خام";
            case UnsafeOperationKind::MutableStaticAccess:
                return "الوصول لمتغير ثابت قابل للتغيير";
            case UnsafeOperationKind::UnionFieldAccess:
                return "الوصول لحقل اتحاد";
            case UnsafeOperationKind::UnsafeTraitImpl:
                return "تنفيذ سمة غير آمنة";
            case UnsafeOperationKind::UnsafeFnCall:
                return "استدعاء دالة غير آمنة";
            case UnsafeOperationKind::RawMemoryOp:
                return "عملية ذاكرة خام";
            case UnsafeOperationKind::PointerCast:
                return "تحويل نوع مؤشر";
            default:
                return "عملية غير آمنة";
        }
    }
    
    /**
     * @brief الحصول على اسم العملية بالإنجليزية / Get English operation name
     */
    std::string getEnglishName() const {
        switch (kind) {
            case UnsafeOperationKind::RawPointerDeref:
                return "raw pointer dereference";
            case UnsafeOperationKind::PointerArithmetic:
                return "pointer arithmetic";
            case UnsafeOperationKind::FFICall:
                return "foreign function call";
            case UnsafeOperationKind::Transmute:
                return "memory transmute";
            case UnsafeOperationKind::MutableStaticAccess:
                return "mutable static access";
            case UnsafeOperationKind::UnionFieldAccess:
                return "union field access";
            case UnsafeOperationKind::UnsafeTraitImpl:
                return "unsafe trait implementation";
            case UnsafeOperationKind::UnsafeFnCall:
                return "unsafe function call";
            case UnsafeOperationKind::RawMemoryOp:
                return "raw memory operation";
            case UnsafeOperationKind::PointerCast:
                return "pointer type cast";
            default:
                return "unsafe operation";
        }
    }
};

// ============================================================================
//                    خطأ الكود غير الآمن / Unsafe Code Error
// ============================================================================

/**
 * @struct UnsafeError
 * @brief خطأ كود غير آمن خارج كتلة غير_آمن / Unsafe code error outside unsafe block
 */
struct UnsafeError {
    /// (AR) العملية المخالفة / (EN) Violating operation
    UnsafeOperation operation;
    
    /// (AR) الرسالة بالعربية / (EN) Arabic message
    std::string arabicMessage;
    
    /// (AR) الرسالة بالإنجليزية / (EN) English message
    std::string englishMessage;
    
    /// (AR) اقتراح الإصلاح / (EN) Fix suggestion
    std::string suggestion;
    
    /**
     * @brief تحويل الخطأ لنص عربي / Convert error to Arabic string
     */
    std::string toArabicString() const;
    
    /**
     * @brief تحويل الخطأ لنص إنجليزي / Convert error to English string
     */
    std::string toEnglishString() const;
};

// ============================================================================
//                    فاحص الكود غير الآمن / Unsafe Code Checker
// ============================================================================

/**
 * @class UnsafeChecker
 * @brief فاحص الكود غير الآمن / Unsafe Code Checker
 * 
 * (AR) هذه الفئة تتحقق من:
 * 
 *      1. أن العمليات غير الآمنة داخل كتل `غير_آمن` فقط
 *      2. أن الدوال غير الآمنة موسومة صحيحاً
 *      3. أن المؤشرات الخام تُستخدم بشكل صحيح
 *      4. تتبع مستوى غير_آمن المتداخل
 * 
 * (EN) This class verifies:
 * 
 *      1. Unsafe operations are only inside `unsafe` blocks
 *      2. Unsafe functions are marked correctly
 *      3. Raw pointers are used properly
 *      4. Tracks nested unsafe levels
 * 
 * **مثال الاستخدام / Usage Example:**
 * 
 * ```cpp
 * UnsafeChecker checker;
 * 
 * // (AR) دخول كتلة غير آمنة / (EN) Enter unsafe block
 * checker.enterUnsafeBlock(location);
 * 
 * // (AR) التحقق من عملية / (EN) Check operation
 * if (!checker.checkOperation(UnsafeOperationKind::RawPointerDeref, location)) {
 *     // (AR) خطأ! العملية غير مسموحة / (EN) Error! Operation not allowed
 * }
 * 
 * // (AR) الخروج من الكتلة / (EN) Exit block
 * checker.exitUnsafeBlock();
 * ```
 */
class UnsafeChecker {
public:
    // ========================================================================
    //                    البناء / Construction
    // ========================================================================
    
    /**
     * @brief منشئ افتراضي / Default constructor
     */
    UnsafeChecker();
    
    /**
     * @brief هادم / Destructor
     */
    ~UnsafeChecker();
    
    // ========================================================================
    //                    إدارة كتل غير_آمن / Unsafe Block Management
    // ========================================================================
    
    /**
     * @brief الدخول لكتلة غير_آمن / Enter unsafe block
     * 
     * (AR) يُزيد مستوى غير_آمن. الكتل المتداخلة مدعومة.
     * (EN) Increases unsafe level. Nested blocks are supported.
     * 
     * @param location موقع كتلة غير_آمن / Location of unsafe block
     */
    void enterUnsafeBlock(const SourceLocation& location);
    
    /**
     * @brief الخروج من كتلة غير_آمن / Exit unsafe block
     * 
     * (AR) يُنقص مستوى غير_آمن. يجب أن يتطابق مع enterUnsafeBlock.
     * (EN) Decreases unsafe level. Must match enterUnsafeBlock.
     */
    void exitUnsafeBlock();
    
    /**
     * @brief هل نحن داخل كتلة غير_آمن؟ / Are we inside unsafe block?
     * 
     * @return true إذا كنا داخل كتلة غير_آمن واحدة على الأقل
     */
    bool isInsideUnsafeBlock() const { return unsafeDepth_ > 0; }
    
    /**
     * @brief الحصول على عمق غير_آمن / Get unsafe depth
     * 
     * @return عدد كتل غير_آمن المتداخلة
     */
    int getUnsafeDepth() const { return unsafeDepth_; }
    
    // ========================================================================
    //                    فحص العمليات / Operation Checking
    // ========================================================================
    
    /**
     * @brief فحص عملية غير آمنة / Check unsafe operation
     * 
     * (AR) يتحقق أن العملية غير الآمنة داخل كتلة غير_آمن
     * (EN) Verifies unsafe operation is inside unsafe block
     * 
     * @param kind نوع العملية / Operation kind
     * @param location موقع العملية / Operation location
     * @param description وصف إضافي / Additional description
     * @return true إذا كانت العملية مسموحة
     * 
     * **مثال / Example:**
     * ```cpp
     * // (AR) عند مصادفة *مؤشر_خام
     * checker.checkOperation(UnsafeOperationKind::RawPointerDeref, loc);
     * ```
     */
    bool checkOperation(UnsafeOperationKind kind,
                       const SourceLocation& location,
                       const std::string& description = "");
    
    /**
     * @brief فحص استدعاء دالة غير آمنة / Check unsafe function call
     * 
     * (AR) يتحقق أن استدعاء دالة غير آمنة داخل كتلة غير_آمن
     * (EN) Verifies unsafe function call is inside unsafe block
     * 
     * @param functionName اسم الدالة / Function name
     * @param location موقع الاستدعاء / Call location
     * @return true إذا كان الاستدعاء مسموح
     */
    bool checkUnsafeFunctionCall(const std::string& functionName,
                                const SourceLocation& location);
    
    /**
     * @brief فحص استدعاء FFI / Check FFI call
     * 
     * @param externName اسم الدالة الخارجية / External function name
     * @param location موقع الاستدعاء / Call location
     * @return true إذا كان الاستدعاء مسموح
     */
    bool checkFFICall(const std::string& externName,
                     const SourceLocation& location);
    
    // ========================================================================
    //                    تسجيل الدوال غير الآمنة / Unsafe Function Registration
    // ========================================================================
    
    /**
     * @brief تسجيل دالة كغير آمنة / Register function as unsafe
     * 
     * (AR) يُسجل أن الدالة موسومة بـ غير_آمن
     * (EN) Registers that function is marked unsafe
     * 
     * @param functionName اسم الدالة / Function name
     */
    void registerUnsafeFunction(const std::string& functionName);
    
    /**
     * @brief هل الدالة غير آمنة؟ / Is function unsafe?
     * 
     * @param functionName اسم الدالة / Function name
     * @return true إذا كانت الدالة موسومة غير_آمن
     */
    bool isUnsafeFunction(const std::string& functionName) const;
    
    /**
     * @brief تسجيل دالة FFI / Register FFI function
     * 
     * @param functionName اسم الدالة الخارجية / External function name
     */
    void registerFFIFunction(const std::string& functionName);
    
    /**
     * @brief هل الدالة FFI؟ / Is function FFI?
     * 
     * @param functionName اسم الدالة / Function name
     * @return true إذا كانت الدالة FFI
     */
    bool isFFIFunction(const std::string& functionName) const;
    
    // ========================================================================
    //                    الأخطاء / Errors
    // ========================================================================
    
    /**
     * @brief الحصول على الأخطاء / Get errors
     * 
     * @return قائمة أخطاء الكود غير الآمن
     */
    const std::vector<UnsafeError>& getErrors() const { return errors_; }
    
    /**
     * @brief هل هناك أخطاء؟ / Are there errors?
     */
    bool hasErrors() const { return !errors_.empty(); }
    
    /**
     * @brief مسح الأخطاء / Clear errors
     */
    void clearErrors() { errors_.clear(); }
    
    // ========================================================================
    //                    الإعدادات / Settings
    // ========================================================================
    
    /**
     * @brief تعيين وضع الرسائل العربية / Set Arabic messages mode
     */
    void setArabicMessages(bool arabic) { useArabicMessages_ = arabic; }
    
    /**
     * @brief تعيين وضع التنقيح / Set debug mode
     */
    void setDebugMode(bool debug) { debugMode_ = debug; }
    
    /**
     * @brief طباعة الحالة للتنقيح / Print state for debugging
     */
    void dump() const;

private:
    // ========================================================================
    //                    البيانات الداخلية / Internal Data
    // ========================================================================
    
    /// (AR) عمق كتل غير_آمن المتداخلة / (EN) Depth of nested unsafe blocks
    int unsafeDepth_ = 0;
    
    /// (AR) مواقع كتل غير_آمن / (EN) Unsafe block locations
    std::vector<SourceLocation> unsafeBlockLocations_;
    
    /// (AR) الدوال غير الآمنة المسجلة / (EN) Registered unsafe functions
    std::set<std::string> unsafeFunctions_;
    
    /// (AR) دوال FFI المسجلة / (EN) Registered FFI functions
    std::set<std::string> ffiFunctions_;
    
    /// (AR) العمليات المسجلة / (EN) Recorded operations
    std::vector<UnsafeOperation> operations_;
    
    /// (AR) الأخطاء / (EN) Errors
    std::vector<UnsafeError> errors_;
    
    /// (AR) استخدام الرسائل العربية / (EN) Use Arabic messages
    bool useArabicMessages_ = true;
    
    /// (AR) وضع التنقيح / (EN) Debug mode
    bool debugMode_ = false;
    
    // ========================================================================
    //                    دوال مساعدة / Helper Functions
    // ========================================================================
    
    /**
     * @brief تسجيل خطأ / Record error
     */
    void recordError(UnsafeOperationKind kind,
                    const SourceLocation& location,
                    const std::string& description);
    
    /**
     * @brief الحصول على رسالة الخطأ بالعربية / Get Arabic error message
     */
    std::string getArabicErrorMessage(UnsafeOperationKind kind) const;
    
    /**
     * @brief الحصول على رسالة الخطأ بالإنجليزية / Get English error message
     */
    std::string getEnglishErrorMessage(UnsafeOperationKind kind) const;
    
    /**
     * @brief الحصول على اقتراح الإصلاح / Get fix suggestion
     */
    std::string getSuggestion(UnsafeOperationKind kind) const;
};

// ============================================================================
//                    دوال مساعدة / Helper Functions
// ============================================================================

/**
 * @brief تحديد ما إذا كان النوع يتطلب كود غير آمن للوصول
 * @brief Determine if type requires unsafe code to access
 * 
 * @param typeName اسم النوع / Type name
 * @return true إذا كان النوع يتطلب كود غير آمن
 */
inline bool requiresUnsafeAccess(const std::string& typeName) {
    // (AR) المؤشرات الخام تتطلب كود غير آمن للوصول
    // (EN) Raw pointers require unsafe code to access
    if (typeName.find("*") != std::string::npos ||
        typeName.find("مؤشر_خام") != std::string::npos ||
        typeName.find("raw_ptr") != std::string::npos) {
        return true;
    }
    
    // (AR) الاتحادات تتطلب كود غير آمن
    // (EN) Unions require unsafe code
    if (typeName.find("union") != std::string::npos ||
        typeName.find("اتحاد") != std::string::npos) {
        return true;
    }
    
    return false;
}

/**
 * @brief تحديد ما إذا كانت الدالة FFI
 * @brief Determine if function is FFI
 * 
 * @param attributes سمات الدالة / Function attributes
 * @return true إذا كانت الدالة FFI
 */
inline bool isForeignFunction(const std::vector<std::string>& attributes) {
    for (const auto& attr : attributes) {
        if (attr == "extern" || attr == "خارجي" ||
            attr == "extern_c" || attr == "c_abi" ||
            attr == "ffi") {
            return true;
        }
    }
    return false;
}

} // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SAD_SEMANTIC_UNSAFE_CHECKER_H
