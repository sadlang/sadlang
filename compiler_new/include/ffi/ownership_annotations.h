/**
 * @file ownership_annotations.h
 * @brief (AR) تعليقات الملكية لـ FFI - نظام تتبع ملكية المؤشرات
 *        (EN) FFI Ownership Annotations - Pointer Ownership Tracking System
 * 
 * @details
 * (AR) هذا الملف يحتوي على نظام تعليقات الملكية للـ FFI.
 *      التعليقات تُحدد كيفية إدارة ملكية المؤشرات المُمررة والمُرجعة.
 * 
 *      التعليقات المدعومة:
 *      ┌────────────────────────────────────────────────────────────┐
 *      │ #[ملك_النتيجة]     - الدالة تُرجع مؤشراً يملكه المُستدعي    │
 *      │ #[يُحرر_بـ: دالة]  - تُحدد دالة التحرير المناسبة             │
 *      │ #[يستعير]          - الدالة تستعير المؤشر (لا تملكه)         │
 *      │ #[ينقل]            - الدالة تأخذ ملكية المؤشر                │
 *      │ #[اختياري]         - المؤشر قد يكون null                    │
 *      │ #[خرج]             - معامل إخراج                             │
 *      └────────────────────────────────────────────────────────────┘
 * 
 *      مثال الاستخدام:
 *      @code{.sad}
 *      خارجي "C" {
 *          #[ملك_النتيجة]
 *          #[يُحرر_بـ: free]
 *          دالة malloc(size: حجم) -> *فراغ
 *          
 *          #[ينقل ptr]
 *          دالة free(ptr: *فراغ)
 *      }
 *      @endcode
 * 
 * (EN) This file contains the ownership annotation system for FFI.
 *      Annotations specify how to manage ownership of passed and returned pointers.
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#ifndef SAD_COMPILER_FFI_OWNERSHIP_ANNOTATIONS_H
#define SAD_COMPILER_FFI_OWNERSHIP_ANNOTATIONS_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <variant>
#include <memory>
#include <functional>

namespace sad {
namespace ffi {

// ============================================================================
//                    (AR) التعدادات
//                    (EN) Enumerations
// ============================================================================

/**
 * @enum OwnershipKind
 * @brief (AR) أنواع الملكية
 *        (EN) Ownership kinds
 * 
 * (AR) تحدد طبيعة ملكية المؤشر:
 *      - OWNED: المُستدعي يملك المؤشر ومسؤول عن تحريره
 *      - BORROWED: مستعار مؤقتاً، لا يُحرر
 *      - TRANSFERRED: الملكية تُنقل للدالة
 *      - SHARED: ملكية مشتركة (مثل Arc)
 */
enum class OwnershipKind {
    UNKNOWN,        ///< (AR) غير معروف / (EN) Unknown
    OWNED,          ///< (AR) مملوك / (EN) Owned
    BORROWED,       ///< (AR) مستعار / (EN) Borrowed
    TRANSFERRED,    ///< (AR) منقول / (EN) Transferred
    SHARED          ///< (AR) مشترك / (EN) Shared
};

/**
 * @enum AnnotationType
 * @brief (AR) أنواع التعليقات
 *        (EN) Annotation types
 */
enum class AnnotationType {
    OWNS_RESULT,        ///< (AR) ملك_النتيجة / (EN) owns_result
    FREED_BY,           ///< (AR) يُحرر_بـ / (EN) freed_by
    BORROWS,            ///< (AR) يستعير / (EN) borrows
    TRANSFERS,          ///< (AR) ينقل / (EN) transfers
    OPTIONAL,           ///< (AR) اختياري / (EN) optional
    OUTPUT,             ///< (AR) خرج / (EN) output
    NOT_NULL,           ///< (AR) ليس_فارغ / (EN) not_null
    LIFETIME,           ///< (AR) عمر / (EN) lifetime
    THREAD_SAFE,        ///< (AR) آمن_للخيوط / (EN) thread_safe
    NO_ESCAPE           ///< (AR) لا_يهرب / (EN) no_escape
};

/**
 * @enum AnnotationTarget
 * @brief (AR) هدف التعليق
 *        (EN) Annotation target
 */
enum class AnnotationTarget {
    FUNCTION,       ///< (AR) الدالة ككل / (EN) The function
    RETURN_VALUE,   ///< (AR) القيمة المُرجعة / (EN) Return value
    PARAMETER       ///< (AR) معامل محدد / (EN) Specific parameter
};

// ============================================================================
//                    (AR) بنيات البيانات
//                    (EN) Data Structures
// ============================================================================

/**
 * @struct AnnotationValue
 * @brief (AR) قيمة التعليق (قد تكون نص، رقم، أو قائمة)
 *        (EN) Annotation value (may be string, number, or list)
 */
struct AnnotationValue {
    std::variant<
        std::monostate,                     // (AR) فارغ / (EN) Empty
        std::string,                        // (AR) نص / (EN) String
        int64_t,                            // (AR) رقم / (EN) Number
        bool,                               // (AR) منطقي / (EN) Boolean
        std::vector<std::string>            // (AR) قائمة / (EN) List
    > value;
    
    bool isEmpty() const { return std::holds_alternative<std::monostate>(value); }
    bool isString() const { return std::holds_alternative<std::string>(value); }
    bool isNumber() const { return std::holds_alternative<int64_t>(value); }
    bool isBool() const { return std::holds_alternative<bool>(value); }
    bool isList() const { return std::holds_alternative<std::vector<std::string>>(value); }
    
    const std::string& asString() const { return std::get<std::string>(value); }
    int64_t asNumber() const { return std::get<int64_t>(value); }
    bool asBool() const { return std::get<bool>(value); }
    const std::vector<std::string>& asList() const { return std::get<std::vector<std::string>>(value); }
};

/**
 * @struct Annotation
 * @brief (AR) تعليق واحد
 *        (EN) Single annotation
 * 
 * (AR) مثال: #[ملك_النتيجة] أو #[يُحرر_بـ: free]
 */
struct Annotation {
    AnnotationType type;                ///< (AR) نوع التعليق / (EN) Annotation type
    std::string arabicName;             ///< (AR) الاسم بالعربية / (EN) Arabic name
    std::string englishName;            ///< (AR) الاسم بالإنجليزية / (EN) English name
    AnnotationTarget target;            ///< (AR) الهدف / (EN) Target
    std::optional<std::string> parameterName;  ///< (AR) اسم المعامل (إن وُجد)
    std::map<std::string, AnnotationValue> arguments;  ///< (AR) الوسائط
    
    size_t lineNumber = 0;              ///< (AR) رقم السطر / (EN) Line number
    size_t columnNumber = 0;            ///< (AR) رقم العمود / (EN) Column number
};

/**
 * @struct OwnershipInfo
 * @brief (AR) معلومات ملكية كاملة لمؤشر
 *        (EN) Complete ownership info for a pointer
 */
struct OwnershipInfo {
    OwnershipKind kind = OwnershipKind::UNKNOWN;
    std::optional<std::string> deallocator;     ///< (AR) دالة التحرير
    std::optional<std::string> lifetime;        ///< (AR) العمر المرتبط
    bool isOptional = false;                    ///< (AR) قد يكون null
    bool isOutput = false;                      ///< (AR) معامل إخراج
    bool isThreadSafe = false;                  ///< (AR) آمن للخيوط
    bool noEscape = false;                      ///< (AR) لا يُخزن
    
    std::vector<std::string> errors;            ///< (AR) أخطاء التحقق
    std::vector<std::string> warnings;          ///< (AR) تحذيرات
};

/**
 * @struct FunctionOwnershipProfile
 * @brief (AR) ملف الملكية الكامل للدالة
 *        (EN) Complete ownership profile for a function
 */
struct FunctionOwnershipProfile {
    std::string functionName;
    OwnershipInfo returnOwnership;
    std::map<std::string, OwnershipInfo> parameterOwnership;
    std::vector<Annotation> allAnnotations;
    
    bool isValid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
};

// ============================================================================
//                    (AR) فئة تحليل التعليقات
//                    (EN) Annotation Parser Class
// ============================================================================

/**
 * @class AnnotationParser
 * @brief (AR) محلل تعليقات الملكية
 *        (EN) Ownership annotation parser
 * 
 * @details
 * (AR) يحلل هذا الفئة تعليقات الملكية من كود لغة ص.
 *      مثال:
 *      @code
 *      AnnotationParser parser;
 *      auto ann = parser.parse("#[ملك_النتيجة]");
 *      @endcode
 */
class AnnotationParser {
public:
    /**
     * @brief (AR) تحليل تعليق واحد
     *        (EN) Parse single annotation
     * 
     * @param text (AR) نص التعليق (مثل "#[ملك_النتيجة]")
     * @return (AR) التعليق المُحلل أو nullopt
     */
    std::optional<Annotation> parse(const std::string& text);
    
    /**
     * @brief (AR) تحليل قائمة تعليقات
     *        (EN) Parse annotation list
     * 
     * @param text (AR) نص يحتوي على تعليقات متعددة
     * @return (AR) قائمة التعليقات
     */
    std::vector<Annotation> parseAll(const std::string& text);
    
    /**
     * @brief (AR) التحقق من صحة التعليق
     *        (EN) Validate annotation
     */
    bool validate(const Annotation& annotation, std::vector<std::string>& errors);
    
    /**
     * @brief (AR) الحصول على الأخطاء
     *        (EN) Get errors
     */
    const std::vector<std::string>& getErrors() const { return errors_; }
    
private:
    std::vector<std::string> errors_;
    
    /**
     * @brief (AR) تحليل اسم التعليق
     *        (EN) Parse annotation name
     */
    std::optional<AnnotationType> parseAnnotationName(const std::string& name);
    
    /**
     * @brief (AR) تحليل وسائط التعليق
     *        (EN) Parse annotation arguments
     */
    std::map<std::string, AnnotationValue> parseArguments(const std::string& args);
    
    /**
     * @brief (AR) تحويل الاسم العربي للنوع
     *        (EN) Convert Arabic name to type
     */
    static AnnotationType arabicNameToType(const std::string& name);
};

// ============================================================================
//                    (AR) فئة بناء ملف الملكية
//                    (EN) Ownership Profile Builder Class
// ============================================================================

/**
 * @class OwnershipProfileBuilder
 * @brief (AR) بناء ملف ملكية للدالة من التعليقات
 *        (EN) Build function ownership profile from annotations
 */
class OwnershipProfileBuilder {
public:
    /**
     * @brief (AR) بناء ملف ملكية من التعليقات
     *        (EN) Build ownership profile from annotations
     * 
     * @param functionName (AR) اسم الدالة / (EN) Function name
     * @param annotations (AR) قائمة التعليقات / (EN) Annotation list
     * @param parameterNames (AR) أسماء المعاملات / (EN) Parameter names
     * @return (AR) ملف الملكية / (EN) Ownership profile
     */
    FunctionOwnershipProfile build(
        const std::string& functionName,
        const std::vector<Annotation>& annotations,
        const std::vector<std::string>& parameterNames
    );
    
    /**
     * @brief (AR) التحقق من تناسق التعليقات
     *        (EN) Validate annotation consistency
     */
    bool validateConsistency(
        const FunctionOwnershipProfile& profile,
        std::vector<std::string>& errors
    );
    
private:
    /**
     * @brief (AR) تطبيق تعليق على ملف الملكية
     *        (EN) Apply annotation to ownership profile
     */
    void applyAnnotation(
        FunctionOwnershipProfile& profile,
        const Annotation& annotation
    );
    
    /**
     * @brief (AR) استنتاج الملكية من السياق
     *        (EN) Infer ownership from context
     */
    void inferOwnership(FunctionOwnershipProfile& profile);
};

// ============================================================================
//                    (AR) فئة التحقق من الملكية
//                    (EN) Ownership Checker Class
// ============================================================================

/**
 * @class OwnershipChecker
 * @brief (AR) التحقق من صحة استخدام الملكية
 *        (EN) Validate ownership usage correctness
 * 
 * @details
 * (AR) يتحقق هذا الفئة من:
 *      - عدم تسريب الذاكرة
 *      - عدم استخدام بعد التحرير
 *      - تطابق دوال الحجز والتحرير
 *      - تناسق العمر
 */
class OwnershipChecker {
public:
    /**
     * @struct CheckResult
     * @brief (AR) نتيجة التحقق
     */
    struct CheckResult {
        bool isValid = true;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::map<std::string, std::string> suggestions;
    };
    
    /**
     * @brief (AR) التحقق من استدعاء دالة
     *        (EN) Check function call
     */
    CheckResult checkCall(
        const FunctionOwnershipProfile& calleeProfile,
        const std::vector<OwnershipInfo>& argumentOwnership
    );
    
    /**
     * @brief (AR) التحقق من نتيجة الدالة
     *        (EN) Check function result
     */
    CheckResult checkResult(
        const OwnershipInfo& resultOwnership,
        bool isStored,
        bool hasCleanup
    );
    
    /**
     * @brief (AR) التحقق من تطابق الحجز والتحرير
     *        (EN) Check allocator/deallocator match
     */
    bool checkDeallocatorMatch(
        const std::string& allocator,
        const std::string& deallocator
    );
    
private:
    // (AR) جدول تطابق الحجز والتحرير
    // (EN) Allocator/deallocator match table
    static const std::map<std::string, std::string> allocDeallocPairs_;
};

// ============================================================================
//                    (AR) فئة توليد كود الملكية
//                    (EN) Ownership Code Generator Class
// ============================================================================

/**
 * @class OwnershipCodeGenerator
 * @brief (AR) توليد كود التحقق من الملكية
 *        (EN) Generate ownership checking code
 * 
 * @details
 * (AR) يولّد هذا الفئة كود للتحقق من الملكية في وقت التشغيل.
 */
class OwnershipCodeGenerator {
public:
    /**
     * @brief (AR) توليد فحص المعامل
     *        (EN) Generate parameter check
     */
    std::string generateParamCheck(
        const std::string& paramName,
        const OwnershipInfo& ownership
    );
    
    /**
     * @brief (AR) توليد فحص النتيجة
     *        (EN) Generate result check
     */
    std::string generateResultCheck(
        const OwnershipInfo& ownership
    );
    
    /**
     * @brief (AR) توليد كود التنظيف
     *        (EN) Generate cleanup code
     */
    std::string generateCleanup(
        const std::string& varName,
        const OwnershipInfo& ownership
    );
    
    /**
     * @brief (AR) توليد غلاف RAII
     *        (EN) Generate RAII wrapper
     */
    std::string generateRaiiWrapper(
        const std::string& typeName,
        const OwnershipInfo& ownership
    );
};

// ============================================================================
//                    (AR) فئة تقرير الملكية
//                    (EN) Ownership Report Class
// ============================================================================

/**
 * @class OwnershipReport
 * @brief (AR) تقرير تحليل الملكية
 *        (EN) Ownership analysis report
 */
class OwnershipReport {
public:
    /**
     * @brief (AR) إضافة دالة للتقرير
     *        (EN) Add function to report
     */
    void addFunction(const FunctionOwnershipProfile& profile);
    
    /**
     * @brief (AR) إضافة تحذير
     *        (EN) Add warning
     */
    void addWarning(const std::string& warning, const std::string& location);
    
    /**
     * @brief (AR) إضافة خطأ
     *        (EN) Add error
     */
    void addError(const std::string& error, const std::string& location);
    
    /**
     * @brief (AR) توليد تقرير نصي
     *        (EN) Generate text report
     */
    std::string generateTextReport() const;
    
    /**
     * @brief (AR) توليد تقرير JSON
     *        (EN) Generate JSON report
     */
    std::string generateJsonReport() const;
    
    /**
     * @brief (AR) الحصول على إحصائيات
     *        (EN) Get statistics
     */
    struct Statistics {
        size_t totalFunctions = 0;
        size_t functionsWithOwnership = 0;
        size_t errors = 0;
        size_t warnings = 0;
        std::map<OwnershipKind, size_t> ownershipKindCounts;
    };
    Statistics getStatistics() const;
    
private:
    std::vector<FunctionOwnershipProfile> functions_;
    std::vector<std::pair<std::string, std::string>> warnings_;
    std::vector<std::pair<std::string, std::string>> errors_;
};

// ============================================================================
//                    (AR) ثوابت أسماء التعليقات
//                    (EN) Annotation Name Constants
// ============================================================================

namespace AnnotationNames {
    // (AR) الأسماء العربية
    // (EN) Arabic names
    constexpr const char* AR_OWNS_RESULT = "\xD9\x85\xD9\x84\xD9\x83_\xD8\xA7\xD9\x84\xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9";  // ملك_النتيجة
    constexpr const char* AR_FREED_BY = "\xD9\x8A\xD9\x8F\xD8\xAD\xD8\xB1\xD8\xB1_\xD8\xA8\xD9\x80";  // يُحرر_بـ
    constexpr const char* AR_BORROWS = "\xD9\x8A\xD8\xB3\xD8\xAA\xD8\xB9\xD9\x8A\xD8\xB1";  // يستعير
    constexpr const char* AR_TRANSFERS = "\xD9\x8A\xD9\x86\xD9\x82\xD9\x84";  // ينقل
    constexpr const char* AR_OPTIONAL = "\xD8\xA7\xD8\xAE\xD8\xAA\xD9\x8A\xD8\xA7\xD8\xB1\xD9\x8A";  // اختياري
    constexpr const char* AR_OUTPUT = "\xD8\xAE\xD8\xB1\xD8\xAC";  // خرج
    constexpr const char* AR_NOT_NULL = "\xD9\x84\xD9\x8A\xD8\xB3_\xD9\x81\xD8\xA7\xD8\xB1\xD8\xBA";  // ليس_فارغ
    constexpr const char* AR_LIFETIME = "\xD8\xB9\xD9\x85\xD8\xB1";  // عمر
    constexpr const char* AR_THREAD_SAFE = "\xD8\xA2\xD9\x85\xD9\x86_\xD9\x84\xD9\x84\xD8\xAE\xD9\x8A\xD9\x88\xD8\xB7";  // آمن_للخيوط
    constexpr const char* AR_NO_ESCAPE = "\xD9\x84\xD8\xA7_\xD9\x8A\xD9\x87\xD8\xB1\xD8\xA8";  // لا_يهرب
    
    // (AR) الأسماء الإنجليزية
    // (EN) English names
    constexpr const char* EN_OWNS_RESULT = "owns_result";
    constexpr const char* EN_FREED_BY = "freed_by";
    constexpr const char* EN_BORROWS = "borrows";
    constexpr const char* EN_TRANSFERS = "transfers";
    constexpr const char* EN_OPTIONAL = "optional";
    constexpr const char* EN_OUTPUT = "output";
    constexpr const char* EN_NOT_NULL = "not_null";
    constexpr const char* EN_LIFETIME = "lifetime";
    constexpr const char* EN_THREAD_SAFE = "thread_safe";
    constexpr const char* EN_NO_ESCAPE = "no_escape";
}

} // namespace ffi
} // namespace sad

#endif // SAD_COMPILER_FFI_OWNERSHIP_ANNOTATIONS_H
