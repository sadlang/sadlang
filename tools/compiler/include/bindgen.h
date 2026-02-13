// تعطيل تحذير Unicode للنصوص العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file bindgen.h
 * @brief (AR) أداة توليد الربط التلقائي - Bindgen لـ FFI
 *        (EN) Automatic Binding Generator - Bindgen for FFI
 * 
 * @details
 * (AR) هذا الملف يحتوي على أداة bindgen التي تقوم بتوليد روابط "ص"
 *      تلقائياً من ملفات رأس C (.h).
 * 
 *      المميزات الرئيسية:
 *      ┌────────────────────────────────────────────────────────────┐
 *      │ 1. قراءة وتحليل ملفات .h                                    │
 *      │ 2. استخراج الدوال والأنواع والثوابت                          │
 *      │ 3. توليد ملفات .sad مع extern blocks                       │
 *      │ 4. توليد الأسماء العربية تلقائياً                           │
 *      │ 5. إنشاء توثيق عربي للدوال                                  │
 *      │ 6. التعامل مع الـ preprocessor directives                   │
 *      │ 7. دعم البنيات والتعدادات والاتحادات                         │
 *      │ 8. إنشاء أغلفة آمنة للمؤشرات                                │
 *      └────────────────────────────────────────────────────────────┘
 * 
 *      أمثلة الاستخدام:
 *      @code{.bash}
 *      # توليد ربط من stdio.h
 *      ص_ولّد_ربط --input stdio.h --output stdio.sad
 *      
 *      # مع ترجمة عربية
 *      ص_ولّد_ربط --input math.h --output حسابات.sad --arabic-names
 *      
 *      # مع تضمين مسارات
 *      ص_ولّد_ربط -I/usr/include --input raylib.h --output raylib.sad
 *      @endcode
 * 
 * (EN) This file contains the bindgen tool that automatically generates
 *      Sad bindings from C header files (.h).
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#ifndef SAD_TOOLS_BINDGEN_H
#define SAD_TOOLS_BINDGEN_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <filesystem>
#include <functional>

namespace Sad {
namespace Tools {

// ============================================================================
//                    (AR) إعادة توجيه الأنواع
//                    (EN) Type Forward Declarations
// ============================================================================

class BindgenConfig;
class CHeaderParser;
class SadBindingGenerator;
class ArabicNameTranslator;

// ============================================================================
//                    (AR) تمثيل عناصر C
//                    (EN) C Element Representations
// ============================================================================

/**
 * @enum CElementKind
 * @brief (AR) أنواع عناصر C المستخرجة
 *        (EN) Extracted C element kinds
 */
enum class CElementKind {
    FUNCTION,       // دالة
    STRUCT,         // بنية
    UNION,          // اتحاد
    ENUM,           // تعداد
    TYPEDEF,        // تعريف نوع
    CONSTANT,       // ثابت (#define)
    GLOBAL_VAR,     // متغير عام
    MACRO_FUNC      // ماكرو دالة
};

/**
 * @struct CParameter
 * @brief (AR) معامل دالة C
 *        (EN) C function parameter
 */
struct CParameter {
    std::string name;           // اسم المعامل
    std::string cType;          // نوع C
    std::string arabicName;     // الاسم العربي
    bool isPointer = false;     // هل مؤشر؟
    bool isConst = false;       // هل ثابت؟
    bool isArray = false;       // هل مصفوفة؟
    size_t arraySize = 0;       // حجم المصفوفة
};

/**
 * @struct CFunction
 * @brief (AR) دالة C مستخرجة
 *        (EN) Extracted C function
 */
struct CFunction {
    std::string name;                       // اسم الدالة
    std::string arabicName;                 // الاسم العربي
    std::string returnType;                 // نوع الإرجاع
    std::vector<CParameter> parameters;     // المعاملات
    bool isVariadic = false;                // دالة متغيرة المعاملات؟
    bool isStatic = false;                  // static؟
    bool isInline = false;                  // inline؟
    std::string callingConvention;          // اصطلاح الاستدعاء
    std::string documentation;              // التوثيق
    std::string headerFile;                 // ملف الرأس المصدر
    int lineNumber = 0;                     // رقم السطر
};

/**
 * @struct CStructField
 * @brief (AR) حقل في بنية C
 *        (EN) Field in C struct
 */
struct CStructField {
    std::string name;           // اسم الحقل
    std::string cType;          // نوع C
    std::string arabicName;     // الاسم العربي
    size_t offset = 0;          // الإزاحة
    size_t size = 0;            // الحجم
};

/**
 * @struct CStruct
 * @brief (AR) بنية C مستخرجة
 *        (EN) Extracted C struct
 */
struct CStruct {
    std::string name;                       // اسم البنية
    std::string arabicName;                 // الاسم العربي
    std::vector<CStructField> fields;       // الحقول
    bool isPacked = false;                  // بدون محاذاة؟
    size_t alignment = 0;                   // المحاذاة
    size_t size = 0;                        // الحجم الكلي
    std::string documentation;              // التوثيق
};

/**
 * @struct CEnumerator
 * @brief (AR) عنصر في تعداد C
 *        (EN) Enumerator in C enum
 */
struct CEnumerator {
    std::string name;           // اسم العنصر
    std::string arabicName;     // الاسم العربي
    int64_t value;              // القيمة
};

/**
 * @struct CEnum
 * @brief (AR) تعداد C مستخرج
 *        (EN) Extracted C enum
 */
struct CEnum {
    std::string name;                           // اسم التعداد
    std::string arabicName;                     // الاسم العربي
    std::vector<CEnumerator> enumerators;       // العناصر
    std::string documentation;                  // التوثيق
};

/**
 * @struct CTypedef
 * @brief (AR) تعريف نوع C
 *        (EN) C typedef
 */
struct CTypedef {
    std::string name;           // الاسم الجديد
    std::string arabicName;     // الاسم العربي
    std::string originalType;   // النوع الأصلي
};

/**
 * @struct CConstant
 * @brief (AR) ثابت C (#define)
 *        (EN) C constant (#define)
 */
struct CConstant {
    std::string name;           // اسم الثابت
    std::string arabicName;     // الاسم العربي
    std::string value;          // القيمة
    std::string inferredType;   // النوع المستنتج
};

/**
 * @struct CHeaderContent
 * @brief (AR) محتوى ملف رأس C كامل
 *        (EN) Complete C header file content
 */
struct CHeaderContent {
    std::string fileName;                       // اسم الملف
    std::vector<CFunction> functions;           // الدوال
    std::vector<CStruct> structs;               // البنيات
    std::vector<CStruct> unions;                // الاتحادات
    std::vector<CEnum> enums;                   // التعدادات
    std::vector<CTypedef> typedefs;             // تعريفات الأنواع
    std::vector<CConstant> constants;           // الثوابت
    std::vector<std::string> includes;          // ملفات مضمنة
};

// ============================================================================
//                    (AR) إعدادات Bindgen
//                    (EN) Bindgen Configuration
// ============================================================================

/**
 * @class BindgenConfig
 * @brief (AR) إعدادات أداة توليد الربط
 *        (EN) Binding generator configuration
 */
class BindgenConfig {
public:
    /**
     * @brief (AR) بناء الإعدادات الافتراضية
     *        (EN) Build default configuration
     */
    BindgenConfig();
    
    // --- (AR) مسارات الملفات / (EN) File paths ---
    
    void addInputFile(const std::string& path) { inputFiles_.push_back(path); }
    void setOutputFile(const std::string& path) { outputFile_ = path; }
    void setOutputDir(const std::string& path) { outputDir_ = path; }
    void addIncludePath(const std::string& path) { includePaths_.push_back(path); }
    
    const std::vector<std::string>& getInputFiles() const { return inputFiles_; }
    const std::string& getOutputFile() const { return outputFile_; }
    const std::string& getOutputDir() const { return outputDir_; }
    const std::vector<std::string>& getIncludePaths() const { return includePaths_; }
    
    // --- (AR) خيارات التوليد / (EN) Generation options ---
    
    void setGenerateArabicNames(bool value) { generateArabicNames_ = value; }
    void setGenerateDocumentation(bool value) { generateDocumentation_ = value; }
    void setGenerateSafeWrappers(bool value) { generateSafeWrappers_ = value; }
    void setGenerateOwnershipAnnotations(bool value) { generateOwnershipAnnotations_ = value; }
    void setPreserveComments(bool value) { preserveComments_ = value; }
    void setInlineConstants(bool value) { inlineConstants_ = value; }
    
    bool getGenerateArabicNames() const { return generateArabicNames_; }
    bool getGenerateDocumentation() const { return generateDocumentation_; }
    bool getGenerateSafeWrappers() const { return generateSafeWrappers_; }
    bool getGenerateOwnershipAnnotations() const { return generateOwnershipAnnotations_; }
    bool getPreserveComments() const { return preserveComments_; }
    bool getInlineConstants() const { return inlineConstants_; }
    
    // --- (AR) تصفية العناصر / (EN) Element filtering ---
    
    void addWhitelistPattern(const std::string& pattern) { whitelistPatterns_.push_back(pattern); }
    void addBlacklistPattern(const std::string& pattern) { blacklistPatterns_.push_back(pattern); }
    void addTypeOverride(const std::string& cType, const std::string& sadType);
    
    const std::vector<std::string>& getWhitelistPatterns() const { return whitelistPatterns_; }
    const std::vector<std::string>& getBlacklistPatterns() const { return blacklistPatterns_; }
    
    // --- (AR) خيارات المنصة / (EN) Platform options ---
    
    void setTargetPlatform(const std::string& platform) { targetPlatform_ = platform; }
    void setIs64Bit(bool value) { is64Bit_ = value; }
    
    const std::string& getTargetPlatform() const { return targetPlatform_; }
    bool is64Bit() const { return is64Bit_; }
    
    // --- (AR) التحقق / (EN) Validation ---
    
    bool validate(std::string& errorMessage) const;

private:
    std::vector<std::string> inputFiles_;
    std::string outputFile_;
    std::string outputDir_;
    std::vector<std::string> includePaths_;
    
    bool generateArabicNames_ = true;
    bool generateDocumentation_ = true;
    bool generateSafeWrappers_ = true;
    bool generateOwnershipAnnotations_ = true;
    bool preserveComments_ = true;
    bool inlineConstants_ = true;
    
    std::vector<std::string> whitelistPatterns_;
    std::vector<std::string> blacklistPatterns_;
    std::unordered_map<std::string, std::string> typeOverrides_;
    
    std::string targetPlatform_ = "windows";
    bool is64Bit_ = true;
};

// ============================================================================
//                    (AR) محلل ملفات رأس C
//                    (EN) C Header Parser
// ============================================================================

/**
 * @class CHeaderParser
 * @brief (AR) محلل لملفات رأس C
 *        (EN) Parser for C header files
 * 
 * @details
 * (AR) يقوم هذا المحلل بقراءة ملفات .h واستخراج:
 *      - تصريحات الدوال
 *      - تعريفات البنيات
 *      - تعريفات التعدادات
 *      - الثوابت (macros)
 *      - typedefs
 * 
 * (EN) This parser reads .h files and extracts:
 *      - Function declarations
 *      - Struct definitions
 *      - Enum definitions
 *      - Constants (macros)
 *      - typedefs
 */
class CHeaderParser {
public:
    /**
     * @brief (AR) بناء المحلل
     *        (EN) Construct parser
     */
    explicit CHeaderParser(const BindgenConfig& config);
    
    ~CHeaderParser() = default;
    
    // --- (AR) التحليل / (EN) Parsing ---
    
    /**
     * @brief (AR) تحليل ملف رأس واحد
     *        (EN) Parse single header file
     * 
     * @param filePath (AR) مسار الملف (EN) File path
     * @return (AR) محتوى الملف (EN) File content
     */
    CHeaderContent parseFile(const std::string& filePath);
    
    /**
     * @brief (AR) تحليل عدة ملفات
     *        (EN) Parse multiple files
     */
    std::vector<CHeaderContent> parseFiles(const std::vector<std::string>& filePaths);
    
    /**
     * @brief (AR) تحليل نص مباشر
     *        (EN) Parse text directly
     */
    CHeaderContent parseText(const std::string& text, const std::string& fileName = "<input>");
    
    // --- (AR) الأخطاء / (EN) Errors ---
    
    bool hasErrors() const { return !errors_.empty(); }
    const std::vector<std::string>& getErrors() const { return errors_; }
    void clearErrors() { errors_.clear(); }

private:
    const BindgenConfig& config_;
    std::vector<std::string> errors_;
    
    // (AR) دوال التحليل الداخلية / (EN) Internal parsing functions
    void parseFunction(const std::string& line, CHeaderContent& content);
    void parseStruct(std::istream& input, CHeaderContent& content);
    void parseEnum(std::istream& input, CHeaderContent& content);
    void parseTypedef(const std::string& line, CHeaderContent& content);
    void parseConstant(const std::string& line, CHeaderContent& content);
    void parseInclude(const std::string& line, CHeaderContent& content);
    
    std::string extractType(const std::string& declaration);
    std::vector<CParameter> extractParameters(const std::string& paramString);
    bool matchesFilter(const std::string& name) const;
};

// ============================================================================
//                    (AR) مترجم الأسماء العربية
//                    (EN) Arabic Name Translator
// ============================================================================

/**
 * @class ArabicNameTranslator
 * @brief (AR) مترجم لتحويل أسماء C إلى أسماء عربية
 *        (EN) Translator for converting C names to Arabic names
 * 
 * @details
 * (AR) يستخدم قاموساً مدمجاً وقواعد ترجمة لتحويل:
 *      - أسماء الدوال: printf → اطبع_منسق
 *      - أسماء الأنواع: FILE → ملف
 *      - أسماء المعاملات: buffer → ذاكرة_مؤقتة
 * 
 * (EN) Uses built-in dictionary and translation rules to convert:
 *      - Function names: printf → اطبع_منسق
 *      - Type names: FILE → ملف
 *      - Parameter names: buffer → ذاكرة_مؤقتة
 */
class ArabicNameTranslator {
public:
    ArabicNameTranslator();
    
    /**
     * @brief (AR) ترجمة اسم دالة
     *        (EN) Translate function name
     */
    std::string translateFunctionName(const std::string& cName) const;
    
    /**
     * @brief (AR) ترجمة اسم نوع
     *        (EN) Translate type name
     */
    std::string translateTypeName(const std::string& cName) const;
    
    /**
     * @brief (AR) ترجمة اسم معامل
     *        (EN) Translate parameter name
     */
    std::string translateParameterName(const std::string& cName) const;
    
    /**
     * @brief (AR) ترجمة اسم ثابت
     *        (EN) Translate constant name
     */
    std::string translateConstantName(const std::string& cName) const;
    
    /**
     * @brief (AR) إضافة ترجمة مخصصة
     *        (EN) Add custom translation
     */
    void addTranslation(const std::string& english, const std::string& arabic);
    
    /**
     * @brief (AR) تحميل قاموس من ملف
     *        (EN) Load dictionary from file
     */
    bool loadDictionary(const std::string& filePath);

private:
    std::unordered_map<std::string, std::string> dictionary_;
    
    void initializeDefaultDictionary();
    std::string camelCaseToArabic(const std::string& name) const;
    std::string snakeCaseToArabic(const std::string& name) const;
};

// ============================================================================
//                    (AR) مولد الربط
//                    (EN) Binding Generator
// ============================================================================

/**
 * @class SadBindingGenerator
 * @brief (AR) مولد ملفات ربط "ص" من محتوى C
 *        (EN) Generator for Sad binding files from C content
 * 
 * @details
 * (AR) يقوم بتوليد كود "ص" من العناصر المستخرجة:
 * 
 *      مثال المدخل (C):
 *      @code{.c}
 *      int printf(const char* format, ...);
 *      @endcode
 * 
 *      مثال المخرج (ص):
 *      @code{.sad}
 *      // دالة طباعة منسقة
 *      خارجي "C" {
 *          دالة printf(format: *ثابت[حرف], ...) -> ص32
 *      }
 *      @endcode
 * 
 * (EN) Generates Sad code from extracted elements
 */
class SadBindingGenerator {
public:
    /**
     * @brief (AR) بناء المولد
     *        (EN) Construct generator
     */
    explicit SadBindingGenerator(const BindgenConfig& config);
    
    // --- (AR) التوليد / (EN) Generation ---
    
    /**
     * @brief (AR) توليد ملف ربط من محتوى رأس واحد
     *        (EN) Generate binding file from single header content
     */
    std::string generate(const CHeaderContent& content);
    
    /**
     * @brief (AR) توليد ملف ربط من عدة محتويات
     *        (EN) Generate binding file from multiple contents
     */
    std::string generate(const std::vector<CHeaderContent>& contents);
    
    /**
     * @brief (AR) كتابة الملف المولد
     *        (EN) Write generated file
     */
    bool writeToFile(const std::string& content, const std::string& filePath);
    
    // --- (AR) توليد مخصص / (EN) Custom generation ---
    
    /**
     * @brief (AR) توليد extern block لدالة
     *        (EN) Generate extern block for function
     */
    std::string generateFunctionBinding(const CFunction& func);
    
    /**
     * @brief (AR) توليد تعريف بنية
     *        (EN) Generate struct definition
     */
    std::string generateStructBinding(const CStruct& structDef);
    
    /**
     * @brief (AR) توليد تعريف تعداد
     *        (EN) Generate enum definition
     */
    std::string generateEnumBinding(const CEnum& enumDef);
    
    /**
     * @brief (AR) توليد ثابت
     *        (EN) Generate constant
     */
    std::string generateConstantBinding(const CConstant& constant);

private:
    const BindgenConfig& config_;
    ArabicNameTranslator translator_;
    
    // (AR) دوال مساعدة / (EN) Helper functions
    std::string generateFileHeader(const std::string& fileName);
    std::string convertCTypeToSad(const std::string& cType);
    std::string generateDocComment(const std::string& doc);
    std::string generateOwnershipAnnotation(const CParameter& param);
    std::string generateSafeWrapper(const CFunction& func);
    std::string indent(const std::string& text, int level = 1);
};

// ============================================================================
//                    (AR) الصنف الرئيسي Bindgen
//                    (EN) Main Bindgen Class
// ============================================================================

/**
 * @class Bindgen
 * @brief (AR) الصنف الرئيسي لأداة توليد الربط
 *        (EN) Main class for binding generator tool
 * 
 * @example
 * @code{.cpp}
 * // استخدام Bindgen
 * Bindgen bindgen;
 * bindgen.config().addInputFile("raylib.h");
 * bindgen.config().setOutputFile("raylib.sad");
 * bindgen.config().setGenerateArabicNames(true);
 * 
 * if (bindgen.run()) {
 *     std::cout << "تم توليد الربط بنجاح!" << std::endl;
 * }
 * @endcode
 */
class Bindgen {
public:
    Bindgen();
    ~Bindgen() = default;
    
    // --- (AR) الإعدادات / (EN) Configuration ---
    
    BindgenConfig& config() { return config_; }
    const BindgenConfig& config() const { return config_; }
    
    // --- (AR) التنفيذ / (EN) Execution ---
    
    /**
     * @brief (AR) تشغيل أداة Bindgen
     *        (EN) Run Bindgen tool
     * 
     * @return (AR) نجاح أم فشل (EN) Success or failure
     */
    bool run();
    
    /**
     * @brief (AR) تشغيل مع إعدادات من سطر الأوامر
     *        (EN) Run with command line arguments
     */
    bool run(int argc, char* argv[]);
    
    /**
     * @brief (AR) الحصول على المحتوى المولد بدون كتابة ملف
     *        (EN) Get generated content without writing file
     */
    std::string generateContent();
    
    // --- (AR) الأخطاء / (EN) Errors ---
    
    bool hasErrors() const { return !errors_.empty(); }
    const std::vector<std::string>& getErrors() const { return errors_; }
    
    // --- (AR) الإحصائيات / (EN) Statistics ---
    
    struct Statistics {
        size_t functionsGenerated = 0;
        size_t structsGenerated = 0;
        size_t enumsGenerated = 0;
        size_t constantsGenerated = 0;
        size_t typedefsGenerated = 0;
        size_t errorsEncountered = 0;
    };
    
    const Statistics& getStatistics() const { return stats_; }

private:
    BindgenConfig config_;
    std::vector<std::string> errors_;
    Statistics stats_;
    
    bool parseCommandLine(int argc, char* argv[]);
    void printUsage();
    void printVersion();
};

// ============================================================================
//                    (AR) دوال المساعدة العامة
//                    (EN) Public Helper Functions
// ============================================================================

/**
 * @brief (AR) تشغيل Bindgen من سطر الأوامر
 *        (EN) Run Bindgen from command line
 */
int bindgenMain(int argc, char* argv[]);

} // namespace Tools
} // namespace Sad

#endif // SAD_TOOLS_BINDGEN_H
