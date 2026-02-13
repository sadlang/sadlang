/**
 * @file safe_wrapper.h
 * @brief (AR) مولد الأغلفة الآمنة لـ FFI - "جسر ص"
 *        (EN) Safe Wrapper Generator for FFI - "Sad Bridge"
 * 
 * @details
 * (AR) هذا الملف يحتوي على نظام توليد أغلفة آمنة تلقائياً لدوال C.
 *      الأغلفة توفر:
 *      ┌────────────────────────────────────────────────────────────┐
 *      │ 1. تحرير تلقائي للموارد (RAII)                              │
 *      │ 2. معالجة أخطاء آمنة بدل null/errno                         │
 *      │ 3. تتبع الملكية وكشف التسريبات                             │
 *      │ 4. تحويل أنواع ضمني                                        │
 *      │ 5. توثيق عربي تلقائي                                       │
 *      └────────────────────────────────────────────────────────────┘
 * 
 *      مثال الاستخدام:
 *      @code{.cpp}
 *      SafeWrapperGenerator gen;
 *      gen.addFunction("malloc", "void*", {{"size_t", "size"}});
 *      gen.setResourceType("malloc", ResourceType::MEMORY);
 *      gen.setDeallocator("malloc", "free");
 *      
 *      std::string sadCode = gen.generateSadBindings();
 *      @endcode
 * 
 * (EN) This file contains an automatic safe wrapper generation system for C functions.
 *      Wrappers provide RAII, safe error handling, ownership tracking, etc.
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#ifndef SAD_TOOLS_BINDGEN_SAFE_WRAPPER_H
#define SAD_TOOLS_BINDGEN_SAFE_WRAPPER_H

#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <functional>
#include <optional>
#include <variant>

namespace sad {
namespace tools {
namespace bindgen {

// ============================================================================
//                    (AR) التعدادات والثوابت
//                    (EN) Enumerations and Constants
// ============================================================================

/**
 * @enum ResourceType
 * @brief (AR) أنواع الموارد التي تحتاج إدارة
 *        (EN) Types of resources that need management
 * 
 * (AR) هذا التعداد يحدد نوع المورد المُرجع من دالة C
 *      لتوليد الغلاف المناسب.
 */
enum class ResourceType {
    NONE,           ///< (AR) لا مورد / (EN) No resource
    MEMORY,         ///< (AR) ذاكرة (malloc/free) / (EN) Memory
    FILE_HANDLE,    ///< (AR) مقبض ملف (fopen/fclose) / (EN) File handle
    SOCKET,         ///< (AR) مقبض شبكة / (EN) Socket handle
    MUTEX,          ///< (AR) قفل / (EN) Mutex
    THREAD,         ///< (AR) خيط / (EN) Thread
    CUSTOM          ///< (AR) مخصص / (EN) Custom
};

/**
 * @enum NullBehavior
 * @brief (AR) سلوك المؤشر الفارغ
 *        (EN) Null pointer behavior
 */
enum class NullBehavior {
    IGNORE,         ///< (AR) تجاهل (يُمرر null) / (EN) Ignore (pass null)
    CHECK_PARAM,    ///< (AR) فحص المعامل وإرجاع خطأ / (EN) Check param and return error
    CHECK_RESULT,   ///< (AR) فحص النتيجة وإرجاع خطأ / (EN) Check result and return error
    BOTH            ///< (AR) فحص المعامل والنتيجة / (EN) Check both
};

/**
 * @enum ErrorConversion
 * @brief (AR) نوع تحويل الأخطاء
 *        (EN) Error conversion type
 */
enum class ErrorConversion {
    NONE,           ///< (AR) لا تحويل / (EN) No conversion
    ERRNO_TO_RESULT,///< (AR) errno إلى نتيجة<T> / (EN) errno to Result<T>
    RETURN_TO_RESULT,///< (AR) القيمة المُرجعة إلى نتيجة / (EN) Return value to Result
    CUSTOM          ///< (AR) مخصص / (EN) Custom
};

// ============================================================================
//                    (AR) بنيات البيانات
//                    (EN) Data Structures
// ============================================================================

/**
 * @struct ParameterInfo
 * @brief (AR) معلومات معامل الدالة
 *        (EN) Function parameter information
 */
struct ParameterInfo {
    std::string cType;          ///< (AR) نوع C / (EN) C type
    std::string name;           ///< (AR) الاسم / (EN) Name
    std::string arabicName;     ///< (AR) الاسم بالعربية / (EN) Arabic name
    bool isPointer = false;     ///< (AR) هل هو مؤشر / (EN) Is pointer
    bool isConst = false;       ///< (AR) هل هو ثابت / (EN) Is const
    bool isOutput = false;      ///< (AR) هل هو معامل إخراج / (EN) Is output param
    bool isOptional = false;    ///< (AR) هل هو اختياري / (EN) Is optional
    std::string defaultValue;   ///< (AR) القيمة الافتراضية / (EN) Default value
};

/**
 * @struct FunctionInfo
 * @brief (AR) معلومات الدالة الكاملة
 *        (EN) Complete function information
 */
struct FunctionInfo {
    std::string name;                       ///< (AR) اسم الدالة بـ C / (EN) C function name
    std::string arabicName;                 ///< (AR) الاسم بالعربية / (EN) Arabic name
    std::string returnType;                 ///< (AR) نوع الإرجاع / (EN) Return type
    std::vector<ParameterInfo> parameters;  ///< (AR) المعاملات / (EN) Parameters
    
    ResourceType resourceType = ResourceType::NONE;
    std::string deallocator;                ///< (AR) دالة التحرير / (EN) Deallocator function
    NullBehavior nullBehavior = NullBehavior::CHECK_RESULT;
    ErrorConversion errorConversion = ErrorConversion::ERRNO_TO_RESULT;
    
    std::string description;                ///< (AR) الوصف / (EN) Description
    std::string arabicDescription;          ///< (AR) الوصف بالعربية / (EN) Arabic description
    
    bool isVariadic = false;                ///< (AR) دالة متعددة الوسائط / (EN) Variadic function
    bool canFail = true;                    ///< (AR) قد تفشل / (EN) Can fail
    std::vector<std::string> errorCodes;    ///< (AR) رموز الأخطاء المحتملة / (EN) Possible error codes
};

/**
 * @struct WrapperConfig
 * @brief (AR) إعدادات توليد الأغلفة
 *        (EN) Wrapper generation configuration
 */
struct WrapperConfig {
    bool generateRaii = true;           ///< (AR) توليد RAII / (EN) Generate RAII
    bool generateResultType = true;     ///< (AR) استخدام نتيجة<T> / (EN) Use Result<T>
    bool generateDocs = true;           ///< (AR) توليد التوثيق / (EN) Generate docs
    bool arabicDocs = true;             ///< (AR) توثيق عربي / (EN) Arabic docs
    bool generateTests = false;         ///< (AR) توليد اختبارات / (EN) Generate tests
    bool trackOwnership = true;         ///< (AR) تتبع الملكية / (EN) Track ownership
    bool detectLeaks = true;            ///< (AR) كشف التسريبات / (EN) Detect leaks
    
    std::string outputNamespace = "ffi";
    std::string modulePrefix;           ///< (AR) بادئة الوحدة / (EN) Module prefix
};

/**
 * @struct GeneratedWrapper
 * @brief (AR) الغلاف المُولّد
 *        (EN) Generated wrapper
 */
struct GeneratedWrapper {
    std::string sadCode;                ///< (AR) كود لغة ص / (EN) Sad language code
    std::string cppGlue;                ///< (AR) كود لصق C++ / (EN) C++ glue code
    std::string tests;                  ///< (AR) كود الاختبارات / (EN) Test code
    std::string documentation;          ///< (AR) التوثيق / (EN) Documentation
    
    std::vector<std::string> warnings;  ///< (AR) تحذيرات / (EN) Warnings
    std::vector<std::string> errors;    ///< (AR) أخطاء / (EN) Errors
};

// ============================================================================
//                    (AR) فئة مولد الأغلفة الآمنة
//                    (EN) Safe Wrapper Generator Class
// ============================================================================

/**
 * @class SafeWrapperGenerator
 * @brief (AR) مولد الأغلفة الآمنة لدوال C
 *        (EN) Safe wrapper generator for C functions
 * 
 * @details
 * (AR) هذه الفئة تأخذ تعريفات دوال C وتولّد أغلفة آمنة بلغة ص.
 *      الأغلفة تتضمن:
 *      - تحرير تلقائي للموارد
 *      - معالجة أخطاء صحيحة
 *      - تتبع الملكية
 *      - توثيق تلقائي
 * 
 * (EN) This class takes C function definitions and generates safe Sad wrappers.
 */
class SafeWrapperGenerator {
public:
    // -------------------- (AR) البناء / (EN) Construction --------------------
    
    /**
     * @brief (AR) المنشئ الافتراضي
     *        (EN) Default constructor
     */
    SafeWrapperGenerator() = default;
    
    /**
     * @brief (AR) المنشئ مع إعدادات
     *        (EN) Constructor with configuration
     */
    explicit SafeWrapperGenerator(const WrapperConfig& config);
    
    // -------------------- (AR) الإعدادات / (EN) Configuration --------------------
    
    /**
     * @brief (AR) تعيين إعدادات التوليد
     *        (EN) Set generation configuration
     */
    void setConfig(const WrapperConfig& config);
    
    /**
     * @brief (AR) الحصول على الإعدادات الحالية
     *        (EN) Get current configuration
     */
    const WrapperConfig& getConfig() const { return config_; }
    
    // -------------------- (AR) إضافة الدوال / (EN) Adding Functions --------------------
    
    /**
     * @brief (AR) إضافة دالة للتغليف
     *        (EN) Add function for wrapping
     * 
     * @param info (AR) معلومات الدالة / (EN) Function information
     */
    void addFunction(const FunctionInfo& info);
    
    /**
     * @brief (AR) إضافة دالة بشكل مبسط
     *        (EN) Add function in simple form
     */
    void addFunction(
        const std::string& name,
        const std::string& returnType,
        const std::vector<std::pair<std::string, std::string>>& params
    );
    
    /**
     * @brief (AR) تعيين نوع المورد للدالة
     *        (EN) Set resource type for function
     */
    void setResourceType(const std::string& funcName, ResourceType type);
    
    /**
     * @brief (AR) تعيين دالة التحرير
     *        (EN) Set deallocator function
     */
    void setDeallocator(const std::string& funcName, const std::string& deallocator);
    
    /**
     * @brief (AR) تعيين سلوك المؤشر الفارغ
     *        (EN) Set null behavior
     */
    void setNullBehavior(const std::string& funcName, NullBehavior behavior);
    
    /**
     * @brief (AR) تعيين الاسم العربي
     *        (EN) Set Arabic name
     */
    void setArabicName(const std::string& funcName, const std::string& arabicName);
    
    // -------------------- (AR) التوليد / (EN) Generation --------------------
    
    /**
     * @brief (AR) توليد جميع الأغلفة
     *        (EN) Generate all wrappers
     * 
     * @return (AR) الأغلفة المُولّدة / (EN) Generated wrappers
     */
    GeneratedWrapper generateAll();
    
    /**
     * @brief (AR) توليد كود ص فقط
     *        (EN) Generate Sad code only
     */
    std::string generateSadBindings();
    
    /**
     * @brief (AR) توليد كود الربط C++
     *        (EN) Generate C++ glue code
     */
    std::string generateCppGlue();
    
    /**
     * @brief (AR) توليد الاختبارات
     *        (EN) Generate tests
     */
    std::string generateTests();
    
    /**
     * @brief (AR) توليد التوثيق
     *        (EN) Generate documentation
     */
    std::string generateDocumentation();
    
    // -------------------- (AR) التحقق / (EN) Validation --------------------
    
    /**
     * @brief (AR) التحقق من صحة الإعدادات
     *        (EN) Validate configuration
     */
    std::vector<std::string> validate() const;
    
    /**
     * @brief (AR) الحصول على التحذيرات
     *        (EN) Get warnings
     */
    const std::vector<std::string>& getWarnings() const { return warnings_; }
    
private:
    // (AR) الدوال المسجلة / (EN) Registered functions
    std::map<std::string, FunctionInfo> functions_;
    
    // (AR) الإعدادات / (EN) Configuration
    WrapperConfig config_;
    
    // (AR) التحذيرات / (EN) Warnings
    std::vector<std::string> warnings_;
    
    // -------------------- (AR) دوال مساعدة / (EN) Helper Functions --------------------
    
    /**
     * @brief (AR) توليد غلاف دالة واحدة
     *        (EN) Generate wrapper for single function
     */
    std::string generateFunctionWrapper(const FunctionInfo& info);
    
    /**
     * @brief (AR) توليد غلاف RAII
     *        (EN) Generate RAII wrapper
     */
    std::string generateRaiiWrapper(const FunctionInfo& info);
    
    /**
     * @brief (AR) توليد معالجة الأخطاء
     *        (EN) Generate error handling
     */
    std::string generateErrorHandling(const FunctionInfo& info);
    
    /**
     * @brief (AR) توليد تتبع الملكية
     *        (EN) Generate ownership tracking
     */
    std::string generateOwnershipTracking(const FunctionInfo& info);
    
    /**
     * @brief (AR) توليد التوثيق للدالة
     *        (EN) Generate documentation for function
     */
    std::string generateFunctionDoc(const FunctionInfo& info);
    
    /**
     * @brief (AR) تحويل نوع C لنوع ص
     *        (EN) Convert C type to Sad type
     */
    std::string cTypeToSad(const std::string& cType);
    
    /**
     * @brief (AR) توليد اسم عربي تلقائي
     *        (EN) Generate automatic Arabic name
     */
    std::string autoArabicName(const std::string& englishName);
};

// ============================================================================
//                    (AR) فئة مُترجم الأسماء العربية
//                    (EN) Arabic Name Translator Class
// ============================================================================

/**
 * @class ArabicFunctionTranslator
 * @brief (AR) مترجم أسماء الدوال إلى العربية
 *        (EN) Function name translator to Arabic
 */
class ArabicFunctionTranslator {
public:
    /**
     * @brief (AR) ترجمة اسم دالة
     *        (EN) Translate function name
     */
    static std::string translate(const std::string& englishName);
    
    /**
     * @brief (AR) إضافة ترجمة مخصصة
     *        (EN) Add custom translation
     */
    static void addTranslation(const std::string& english, const std::string& arabic);
    
    /**
     * @brief (AR) تحميل ترجمات من ملف
     *        (EN) Load translations from file
     */
    static bool loadFromFile(const std::string& filePath);
    
private:
    static std::map<std::string, std::string> translations_;
    
    /**
     * @brief (AR) تهيئة القاموس الافتراضي
     *        (EN) Initialize default dictionary
     */
    static void initializeDefaults();
};

// ============================================================================
//                    (AR) فئة تحليل سلوك الدوال
//                    (EN) Function Behavior Analyzer Class
// ============================================================================

/**
 * @class FunctionBehaviorAnalyzer
 * @brief (AR) محلل سلوك دوال C لتوليد أغلفة مناسبة
 *        (EN) C function behavior analyzer for generating appropriate wrappers
 * 
 * @details
 * (AR) يحلل هذا الفئة:
 *      - هل الدالة تحجز ذاكرة؟
 *      - ما هي دالة التحرير المناسبة؟
 *      - هل يمكن أن تفشل؟
 *      - ما هي الأخطاء المحتملة؟
 */
class FunctionBehaviorAnalyzer {
public:
    /**
     * @struct AnalysisResult
     * @brief (AR) نتيجة التحليل
     *        (EN) Analysis result
     */
    struct AnalysisResult {
        ResourceType resourceType = ResourceType::NONE;
        std::string suggestedDeallocator;
        bool canFail = false;
        std::vector<std::string> possibleErrors;
        NullBehavior suggestedNullBehavior = NullBehavior::CHECK_RESULT;
        std::string confidence;  // "high", "medium", "low"
    };
    
    /**
     * @brief (AR) تحليل دالة
     *        (EN) Analyze function
     */
    static AnalysisResult analyze(const FunctionInfo& info);
    
private:
    /**
     * @brief (AR) اكتشاف دالة الحجز
     *        (EN) Detect allocation function
     */
    static bool isAllocator(const FunctionInfo& info);
    
    /**
     * @brief (AR) اكتشاف دالة فتح ملف
     *        (EN) Detect file open function
     */
    static bool isFileOpener(const FunctionInfo& info);
    
    /**
     * @brief (AR) اقتراح دالة التحرير
     *        (EN) Suggest deallocator
     */
    static std::string suggestDeallocator(const std::string& funcName);
};

// ============================================================================
//                    (AR) فئة مُصدر الأغلفة
//                    (EN) Wrapper Emitter Class
// ============================================================================

/**
 * @class WrapperEmitter
 * @brief (AR) مُصدر كود الأغلفة
 *        (EN) Wrapper code emitter
 */
class WrapperEmitter {
public:
    /**
     * @brief (AR) بدء ملف جديد
     *        (EN) Start new file
     */
    void startFile(const std::string& filename);
    
    /**
     * @brief (AR) إضافة ترويسة
     *        (EN) Add header
     */
    void addHeader(const std::string& title, const std::string& description);
    
    /**
     * @brief (AR) إضافة استيراد
     *        (EN) Add import
     */
    void addImport(const std::string& module);
    
    /**
     * @brief (AR) بدء بنية
     *        (EN) Start struct
     */
    void startStruct(const std::string& name, const std::string& arabicName = "");
    
    /**
     * @brief (AR) إنهاء بنية
     *        (EN) End struct
     */
    void endStruct();
    
    /**
     * @brief (AR) بدء دالة
     *        (EN) Start function
     */
    void startFunction(
        const std::string& name,
        const std::string& returnType,
        const std::vector<std::pair<std::string, std::string>>& params
    );
    
    /**
     * @brief (AR) إنهاء دالة
     *        (EN) End function
     */
    void endFunction();
    
    /**
     * @brief (AR) إضافة سطر كود
     *        (EN) Add code line
     */
    void addLine(const std::string& code);
    
    /**
     * @brief (AR) إضافة تعليق
     *        (EN) Add comment
     */
    void addComment(const std::string& comment, bool arabic = true);
    
    /**
     * @brief (AR) الحصول على الكود المُولّد
     *        (EN) Get generated code
     */
    std::string getCode() const;
    
    /**
     * @brief (AR) مسح الكود
     *        (EN) Clear code
     */
    void clear();
    
private:
    std::string code_;
    int indentLevel_ = 0;
    
    std::string indent() const;
};

} // namespace bindgen
} // namespace tools
} // namespace sad

#endif // SAD_TOOLS_BINDGEN_SAFE_WRAPPER_H
