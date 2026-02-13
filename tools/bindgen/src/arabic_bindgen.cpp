/**
 * @file arabic_bindgen.cpp
 * @brief (AR) أداة ص_ولّد_ربط - مولّد ربط عربي من ملفات C الرأسية
 *        (EN) Arabic Bindgen Tool - Generates Arabic bindings from C headers
 * 
 * @details
 * (AR) ═══════════════════════════════════════════════════════════════════════════
 *      ███████╗   ████████╗██████╗ ██████╗        ██████╗ ██████╗ ██╗████████╗
 *      ██╔════╝   ╚══██╔══╝██╔══██╗██╔══██╗       ██╔══██╗██╔══██╗██║╚══██╔══╝
 *      ███████╗      ██║   ██████╔╝██║  ██║       ██████╔╝██████╔╝██║   ██║   
 *      ╚════██║      ██║   ██╔══██╗██║  ██║       ██╔══██╗██╔══██╗██║   ██║   
 *      ███████║      ██║   ██║  ██║██████╔╝       ██████╔╝██║  ██║██║   ██║   
 *      ╚══════╝      ╚═╝   ╚═╝  ╚═╝╚═════╝        ╚═════╝ ╚═╝  ╚═╝╚═╝   ╚═╝   
 *                               ص_ولّد_ربط
 *                  مولّد الربط العربي من ملفات C الرأسية
 *      ═══════════════════════════════════════════════════════════════════════════
 * 
 *      هذا الملف يحتوي على أداة سطر الأوامر التي تحوّل ملفات C الرأسية
 *      (.h) إلى ربطات لغة ص مع أسماء عربية.
 * 
 *      الاستخدام:
 *      ┌────────────────────────────────────────────────────────────────────┐
 *      │ ص_ولّد_ربط header.h -o bindings.sad                                │
 *      │ ص_ولّد_ربط header.h --آمن --ترجم-أسماء                              │
 *      │ ص_ولّد_ربط header.h -I/include/path --بادئة مكتبتي_                 │
 *      └────────────────────────────────────────────────────────────────────┘
 * 
 *      الخيارات:
 *      --آمن, --safe          : توليد أغلفة آمنة RAII
 *      --ترجم-أسماء, --translate : ترجمة أسماء الدوال للعربية
 *      --بادئة, --prefix      : بادئة للأسماء المولّدة
 *      -I, --include          : مسار البحث عن ملفات الرأس
 *      -o, --output           : ملف الخرج (.sad)
 *      --json                 : إخراج JSON للمعالجة البرمجية
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <regex>
#include <filesystem>
#include <memory>
#include <optional>
#include <algorithm>

namespace fs = std::filesystem;

namespace sad {
namespace bindgen {

// ============================================================================
//                    (AR) ثوابت الكلمات العربية
//                    (EN) Arabic Keyword Constants
// ============================================================================

namespace ArabicKeywords {
    // (AR) الكلمات المفتاحية للربط
    // (EN) Binding keywords
    const std::string EXTERN_C = u8"خارجي \"C\"";
    const std::string FUNCTION = u8"دالة";
    const std::string END = u8"نهاية";
    const std::string STRUCT = u8"بنية";
    const std::string ENUM = u8"تعداد";
    const std::string TYPE = u8"نوع";
    const std::string CONST = u8"ثابت";
    const std::string MUT = u8"متغير";
    const std::string UNSAFE = u8"غير_آمن";
    const std::string SAFE_WRAPPER = u8"غلاف_آمن";
    const std::string RETURN = u8"إرجاع";
    const std::string IF = u8"إذا";
    const std::string ELSE = u8"وإلا";
    const std::string NULL_CHECK = u8"فارغ؟";
    const std::string NOT_NULL = u8"غير_فارغ!";
    
    // (AR) أنواع البيانات
    // (EN) Data types
    const std::string VOID = u8"فراغ";
    const std::string INT8 = u8"ص8";
    const std::string INT16 = u8"ص16";
    const std::string INT32 = u8"ص32";
    const std::string INT64 = u8"ص64";
    const std::string UINT8 = u8"ص_غ8";
    const std::string UINT16 = u8"ص_غ16";
    const std::string UINT32 = u8"ص_غ32";
    const std::string UINT64 = u8"ص_غ64";
    const std::string FLOAT32 = u8"عش32";
    const std::string FLOAT64 = u8"عش64";
    const std::string CHAR = u8"حرف";
    const std::string BOOL = u8"منطقي";
    const std::string SIZE = u8"حجم";
    const std::string PTR = u8"مؤشر";
    
    // (AR) الملكية
    // (EN) Ownership
    const std::string OWNS_RESULT = u8"ملك_النتيجة";
    const std::string FREED_BY = u8"يُحرر_بـ";
    const std::string BORROWS = u8"يستعير";
    const std::string TRANSFERS = u8"ينقل";
    const std::string OPTIONAL = u8"اختياري";
}

// ============================================================================
//                    (AR) ترجمة أسماء الدوال
//                    (EN) Function Name Translation
// ============================================================================

/**
 * @class NameTranslator
 * @brief (AR) مترجم الأسماء من الإنجليزية للعربية
 *        (EN) English to Arabic name translator
 */
class NameTranslator {
public:
    /**
     * (AR) ترجمة اسم دالة للعربية
     * (EN) Translate function name to Arabic
     */
    std::string translateFunctionName(const std::string& englishName) {
        // (AR) البحث في الجدول أولاً
        // (EN) Check lookup table first
        auto it = functionTranslations_.find(englishName);
        if (it != functionTranslations_.end()) {
            return it->second;
        }
        
        // (AR) تحليل الاسم وترجمة الأجزاء
        // (EN) Parse name and translate parts
        return decomposeAndTranslate(englishName);
    }
    
    /**
     * (AR) ترجمة اسم نوع للعربية
     * (EN) Translate type name to Arabic
     */
    std::string translateTypeName(const std::string& englishName) {
        auto it = typeTranslations_.find(englishName);
        if (it != typeTranslations_.end()) {
            return it->second;
        }
        
        return englishName;  // (AR) الإبقاء على الاسم الأصلي
    }
    
private:
    /**
     * (AR) تحليل الاسم وترجمة كل جزء
     * (EN) Decompose name and translate each part
     */
    std::string decomposeAndTranslate(const std::string& name) {
        std::vector<std::string> parts;
        std::string current;
        
        // (AR) تقسيم بناءً على _ أو camelCase
        // (EN) Split by _ or camelCase
        for (size_t i = 0; i < name.size(); i++) {
            char c = name[i];
            if (c == '_') {
                if (!current.empty()) {
                    parts.push_back(current);
                    current.clear();
                }
            } else if (std::isupper(c) && !current.empty()) {
                parts.push_back(current);
                current = std::tolower(c);
            } else {
                current += std::tolower(c);
            }
        }
        if (!current.empty()) {
            parts.push_back(current);
        }
        
        // (AR) ترجمة كل جزء
        // (EN) Translate each part
        std::string result;
        for (const auto& part : parts) {
            auto it = wordTranslations_.find(part);
            if (it != wordTranslations_.end()) {
                result += it->second;
            } else {
                result += part;
            }
            result += "_";
        }
        
        if (!result.empty() && result.back() == '_') {
            result.pop_back();
        }
        
        return result;
    }
    
    // (AR) جدول ترجمة الدوال الشائعة
    // (EN) Common function translation table
    std::map<std::string, std::string> functionTranslations_ = {
        // (AR) دوال الذاكرة
        {"malloc", u8"احجز_ذاكرة"},
        {"calloc", u8"احجز_صفري"},
        {"realloc", u8"أعد_الحجز"},
        {"free", u8"حرر"},
        {"memcpy", u8"انسخ_ذاكرة"},
        {"memmove", u8"انقل_ذاكرة"},
        {"memset", u8"عيّن_ذاكرة"},
        {"memcmp", u8"قارن_ذاكرة"},
        
        // (AR) دوال النصوص
        {"strlen", u8"طول_نص"},
        {"strcpy", u8"انسخ_نص"},
        {"strncpy", u8"انسخ_نص_ن"},
        {"strcat", u8"الحق_نص"},
        {"strncat", u8"الحق_نص_ن"},
        {"strcmp", u8"قارن_نص"},
        {"strncmp", u8"قارن_نص_ن"},
        {"strstr", u8"ابحث_نص"},
        {"strchr", u8"ابحث_حرف"},
        {"strrchr", u8"ابحث_حرف_أخير"},
        {"strdup", u8"انسخ_نص_جديد"},
        {"strtok", u8"قسّم_نص"},
        
        // (AR) دوال الملفات
        {"fopen", u8"افتح_ملف"},
        {"fclose", u8"أغلق_ملف"},
        {"fread", u8"اقرأ_ملف"},
        {"fwrite", u8"اكتب_ملف"},
        {"fseek", u8"انتقل_ملف"},
        {"ftell", u8"موضع_ملف"},
        {"rewind", u8"أعد_للبداية"},
        {"fgets", u8"اقرأ_سطر"},
        {"fputs", u8"اكتب_سطر"},
        {"fprintf", u8"اطبع_منسق_ملف"},
        {"fscanf", u8"اقرأ_منسق_ملف"},
        {"fflush", u8"افرغ_ملف"},
        {"feof", u8"نهاية_ملف؟"},
        {"ferror", u8"خطأ_ملف؟"},
        
        // (AR) دوال الطباعة
        {"printf", u8"اطبع"},
        {"sprintf", u8"اطبع_لنص"},
        {"snprintf", u8"اطبع_لنص_ن"},
        {"puts", u8"اطبع_سطر"},
        {"putchar", u8"اطبع_حرف"},
        {"getchar", u8"اقرأ_حرف"},
        {"gets", u8"اقرأ_نص"},
        {"scanf", u8"اقرأ_منسق"},
        {"sscanf", u8"اقرأ_منسق_نص"},
        
        // (AR) دوال الرياضيات
        {"sin", u8"جا"},
        {"cos", u8"جتا"},
        {"tan", u8"ظا"},
        {"asin", u8"قوس_جا"},
        {"acos", u8"قوس_جتا"},
        {"atan", u8"قوس_ظا"},
        {"atan2", u8"قوس_ظا2"},
        {"sinh", u8"جا_قطعي"},
        {"cosh", u8"جتا_قطعي"},
        {"tanh", u8"ظا_قطعي"},
        {"exp", u8"أس"},
        {"log", u8"لو"},
        {"log10", u8"لو10"},
        {"pow", u8"قوة"},
        {"sqrt", u8"جذر"},
        {"cbrt", u8"جذر3"},
        {"ceil", u8"سقف"},
        {"floor", u8"أرضية"},
        {"round", u8"تقريب"},
        {"abs", u8"قيمة_مطلقة"},
        {"fabs", u8"قيمة_مطلقة_عش"},
        
        // (AR) دوال التحويل
        {"atoi", u8"نص_لعدد"},
        {"atol", u8"نص_لعدد_طويل"},
        {"atof", u8"نص_لعشري"},
        {"strtol", u8"نص_لعدد_طويل_ص"},
        {"strtod", u8"نص_لعشري_ص"},
        {"itoa", u8"عدد_لنص"},
        
        // (AR) دوال عشوائية
        {"rand", u8"عشوائي"},
        {"srand", u8"بذرة_عشوائي"},
        
        // (AR) دوال الوقت
        {"time", u8"الوقت"},
        {"clock", u8"الساعة"},
        {"localtime", u8"وقت_محلي"},
        {"gmtime", u8"وقت_عالمي"},
        {"strftime", u8"نسّق_وقت"},
        {"difftime", u8"فرق_وقت"},
        {"mktime", u8"اصنع_وقت"},
        {"sleep", u8"نم"},
        {"usleep", u8"نم_ميكرو"},
        
        // (AR) دوال النظام
        {"exit", u8"اخرج"},
        {"abort", u8"أوقف"},
        {"system", u8"نفّذ_أمر"},
        {"getenv", u8"متغير_بيئة"},
        {"setenv", u8"عيّن_بيئة"},
    };
    
    // (AR) جدول ترجمة الأنواع
    // (EN) Type translation table
    std::map<std::string, std::string> typeTranslations_ = {
        {"FILE", u8"ملف"},
        {"size_t", u8"حجم"},
        {"ptrdiff_t", u8"فرق_مؤشر"},
        {"time_t", u8"وقت_ن"},
        {"clock_t", u8"ساعة_ن"},
        {"tm", u8"وقت_بنية"},
    };
    
    // (AR) جدول ترجمة الكلمات
    // (EN) Word translation table
    std::map<std::string, std::string> wordTranslations_ = {
        {"get", u8"أحضر"},
        {"set", u8"عيّن"},
        {"read", u8"اقرأ"},
        {"write", u8"اكتب"},
        {"open", u8"افتح"},
        {"close", u8"أغلق"},
        {"create", u8"أنشئ"},
        {"delete", u8"احذف"},
        {"remove", u8"أزل"},
        {"add", u8"أضف"},
        {"append", u8"ألحق"},
        {"insert", u8"أدخل"},
        {"find", u8"ابحث"},
        {"search", u8"بحث"},
        {"copy", u8"انسخ"},
        {"move", u8"انقل"},
        {"clear", u8"امسح"},
        {"reset", u8"أعد"},
        {"init", u8"هيئ"},
        {"free", u8"حرر"},
        {"alloc", u8"احجز"},
        {"new", u8"جديد"},
        {"destroy", u8"دمّر"},
        {"print", u8"اطبع"},
        {"format", u8"نسّق"},
        {"parse", u8"حلل"},
        {"convert", u8"حوّل"},
        {"compare", u8"قارن"},
        {"sort", u8"رتّب"},
        {"reverse", u8"اعكس"},
        {"size", u8"حجم"},
        {"length", u8"طول"},
        {"count", u8"عدد"},
        {"is", u8"هل"},
        {"has", u8"لديه"},
        {"can", u8"يمكن"},
        {"to", u8"إلى"},
        {"from", u8"من"},
        {"str", u8"نص"},
        {"string", u8"نص"},
        {"char", u8"حرف"},
        {"int", u8"عدد"},
        {"float", u8"عشري"},
        {"double", u8"عشري_مزدوج"},
        {"bool", u8"منطقي"},
        {"file", u8"ملف"},
        {"buffer", u8"مخزن"},
        {"array", u8"مصفوفة"},
        {"list", u8"قائمة"},
        {"map", u8"خريطة"},
        {"hash", u8"تجزئة"},
        {"key", u8"مفتاح"},
        {"value", u8"قيمة"},
        {"error", u8"خطأ"},
        {"result", u8"نتيجة"},
        {"status", u8"حالة"},
        {"ptr", u8"مؤشر"},
        {"pointer", u8"مؤشر"},
        {"ref", u8"مرجع"},
        {"data", u8"بيانات"},
        {"info", u8"معلومات"},
        {"node", u8"عقدة"},
        {"head", u8"رأس"},
        {"tail", u8"ذيل"},
        {"next", u8"التالي"},
        {"prev", u8"السابق"},
        {"first", u8"أول"},
        {"last", u8"آخر"},
        {"begin", u8"بداية"},
        {"end", u8"نهاية"},
        {"start", u8"ابدأ"},
        {"stop", u8"أوقف"},
        {"pause", u8"أوقف_مؤقت"},
        {"resume", u8"استأنف"},
        {"wait", u8"انتظر"},
        {"signal", u8"إشارة"},
        {"event", u8"حدث"},
        {"callback", u8"استدعاء_راجع"},
        {"handler", u8"معالج"},
        {"listener", u8"مستمع"},
        {"observer", u8"مراقب"},
        {"lock", u8"قفل"},
        {"unlock", u8"فك_قفل"},
        {"acquire", u8"احصل"},
        {"release", u8"أطلق"},
        {"send", u8"أرسل"},
        {"receive", u8"استقبل"},
        {"connect", u8"اتصل"},
        {"disconnect", u8"افصل"},
        {"bind", u8"اربط"},
        {"unbind", u8"فك_ربط"},
    };
};

// ============================================================================
//                    (AR) محلل ملفات C الرأسية
//                    (EN) C Header File Parser
// ============================================================================

/**
 * @struct CParameter
 * @brief (AR) معلومات معامل دالة C
 *        (EN) C function parameter info
 */
struct CParameter {
    std::string type;           // (AR) نوع المعامل (EN) Parameter type
    std::string name;           // (AR) اسم المعامل (EN) Parameter name
    bool isPointer = false;     // (AR) هل مؤشر؟ (EN) Is pointer?
    bool isConst = false;       // (AR) هل ثابت؟ (EN) Is const?
    bool isArray = false;       // (AR) هل مصفوفة؟ (EN) Is array?
    size_t arraySize = 0;       // (AR) حجم المصفوفة (EN) Array size
};

/**
 * @struct CFunction
 * @brief (AR) معلومات دالة C
 *        (EN) C function info
 */
struct CFunction {
    std::string name;               // (AR) اسم الدالة (EN) Function name
    std::string returnType;         // (AR) نوع الإرجاع (EN) Return type
    std::vector<CParameter> params; // (AR) المعاملات (EN) Parameters
    bool returnsPointer = false;    // (AR) هل ترجع مؤشر؟
    bool isVariadic = false;        // (AR) هل متغيرة المعاملات؟
    std::string comment;            // (AR) التعليق (EN) Comment
    
    // (AR) خصائص الملكية المستنتجة
    // (EN) Inferred ownership properties
    bool ownsResult = false;        // (AR) تملك النتيجة
    std::string freedBy;            // (AR) تُحرر بواسطة
};

/**
 * @struct CStruct
 * @brief (AR) معلومات بنية C
 *        (EN) C struct info
 */
struct CStruct {
    std::string name;                       // (AR) اسم البنية
    std::vector<CParameter> fields;         // (AR) الحقول
    std::string comment;                    // (AR) التعليق
};

/**
 * @struct CEnum
 * @brief (AR) معلومات تعداد C
 *        (EN) C enum info
 */
struct CEnum {
    std::string name;                       // (AR) اسم التعداد
    std::vector<std::pair<std::string, std::optional<int>>> values;  // (AR) القيم
    std::string comment;                    // (AR) التعليق
};

/**
 * @struct CTypedef
 * @brief (AR) معلومات اسم مستعار C
 *        (EN) C typedef info
 */
struct CTypedef {
    std::string alias;          // (AR) الاسم المستعار
    std::string original;       // (AR) النوع الأصلي
};

/**
 * @struct CConstant
 * @brief (AR) معلومات ثابت C
 *        (EN) C constant info
 */
struct CConstant {
    std::string name;           // (AR) اسم الثابت
    std::string value;          // (AR) القيمة
    std::string type;           // (AR) النوع
};

/**
 * @class CHeaderParser
 * @brief (AR) محلل ملفات C الرأسية
 *        (EN) C header file parser
 */
class CHeaderParser {
public:
    /**
     * (AR) تحليل ملف رأسي
     * (EN) Parse header file
     */
    bool parseFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << u8"خطأ: لا يمكن فتح الملف: " << filepath << std::endl;
            return false;
        }
        
        currentFile_ = filepath;
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // (AR) إزالة التعليقات متعددة الأسطر
        // (EN) Remove multi-line comments
        content = std::regex_replace(content, std::regex("/\\*[^*]*\\*+(?:[^/*][^*]*\\*+)*/"), "");
        
        // (AR) استخراج التعليقات أحادية السطر للاستخدام لاحقاً
        // (EN) Extract single-line comments for later use
        
        // (AR) تحليل الدوال
        // (EN) Parse functions
        parseFunctions(content);
        
        // (AR) تحليل البنى
        // (EN) Parse structs
        parseStructs(content);
        
        // (AR) تحليل التعدادات
        // (EN) Parse enums
        parseEnums(content);
        
        // (AR) تحليل typedef
        // (EN) Parse typedefs
        parseTypedefs(content);
        
        // (AR) تحليل الثوابت (#define)
        // (EN) Parse constants (#define)
        parseConstants(content);
        
        return true;
    }
    
    // (AR) الوصول للنتائج
    // (EN) Access results
    const std::vector<CFunction>& getFunctions() const { return functions_; }
    const std::vector<CStruct>& getStructs() const { return structs_; }
    const std::vector<CEnum>& getEnums() const { return enums_; }
    const std::vector<CTypedef>& getTypedefs() const { return typedefs_; }
    const std::vector<CConstant>& getConstants() const { return constants_; }
    
private:
    void parseFunctions(const std::string& content) {
        // (AR) نمط بسيط لاكتشاف الدوال
        // (EN) Simple pattern for function detection
        // نمط: return_type function_name(params);
        std::regex funcRegex(
            R"(^\s*((?:const\s+)?[\w_]+(?:\s*\*)?)\s+([\w_]+)\s*\(([^)]*)\)\s*;)",
            std::regex::multiline
        );
        
        std::sregex_iterator it(content.begin(), content.end(), funcRegex);
        std::sregex_iterator end;
        
        while (it != end) {
            std::smatch match = *it;
            CFunction func;
            
            std::string returnType = match[1].str();
            func.name = match[2].str();
            std::string params = match[3].str();
            
            // (AR) تنظيف نوع الإرجاع
            func.returnType = trim(returnType);
            func.returnsPointer = returnType.find('*') != std::string::npos;
            
            // (AR) تحليل المعاملات
            parseParameters(params, func.params);
            
            // (AR) هل متغيرة المعاملات؟
            func.isVariadic = params.find("...") != std::string::npos;
            
            // (AR) استنتاج الملكية
            inferOwnership(func);
            
            functions_.push_back(func);
            ++it;
        }
    }
    
    void parseParameters(const std::string& paramsStr, std::vector<CParameter>& params) {
        if (paramsStr.empty() || paramsStr == "void") {
            return;
        }
        
        // (AR) تقسيم بالفاصلة
        std::vector<std::string> paramList;
        std::string current;
        int depth = 0;
        
        for (char c : paramsStr) {
            if (c == '(' || c == '[') depth++;
            else if (c == ')' || c == ']') depth--;
            else if (c == ',' && depth == 0) {
                paramList.push_back(trim(current));
                current.clear();
                continue;
            }
            current += c;
        }
        if (!current.empty()) {
            paramList.push_back(trim(current));
        }
        
        // (AR) تحليل كل معامل
        for (const auto& param : paramList) {
            if (param == "..." || param.empty()) continue;
            
            CParameter p;
            
            // (AR) فحص const
            p.isConst = param.find("const") != std::string::npos;
            
            // (AR) فحص المؤشر
            p.isPointer = param.find('*') != std::string::npos;
            
            // (AR) فحص المصفوفة
            auto bracketPos = param.find('[');
            if (bracketPos != std::string::npos) {
                p.isArray = true;
                auto closePos = param.find(']');
                if (closePos > bracketPos + 1) {
                    std::string sizeStr = param.substr(bracketPos + 1, closePos - bracketPos - 1);
                    try {
                        p.arraySize = std::stoul(sizeStr);
                    } catch (...) {
                        p.arraySize = 0;
                    }
                }
            }
            
            // (AR) استخراج الاسم والنوع
            std::string cleanParam = param;
            cleanParam = std::regex_replace(cleanParam, std::regex("const\\s+"), "");
            cleanParam = std::regex_replace(cleanParam, std::regex("\\[.*\\]"), "");
            
            // (AR) آخر كلمة هي الاسم
            auto lastSpace = cleanParam.rfind(' ');
            if (lastSpace != std::string::npos) {
                p.name = trim(cleanParam.substr(lastSpace + 1));
                p.name.erase(std::remove(p.name.begin(), p.name.end(), '*'), p.name.end());
                p.type = trim(cleanParam.substr(0, lastSpace));
            } else {
                p.type = cleanParam;
                p.name = "arg" + std::to_string(params.size());
            }
            
            params.push_back(p);
        }
    }
    
    void parseStructs(const std::string& content) {
        std::regex structRegex(
            R"((?:typedef\s+)?struct\s+(\w+)\s*\{([^}]*)\}\s*(?:(\w+)\s*)?;)",
            std::regex::multiline
        );
        
        std::sregex_iterator it(content.begin(), content.end(), structRegex);
        std::sregex_iterator end;
        
        while (it != end) {
            std::smatch match = *it;
            CStruct s;
            
            s.name = match[1].str().empty() ? match[3].str() : match[1].str();
            std::string fields = match[2].str();
            
            // (AR) تحليل الحقول
            std::regex fieldRegex(R"(([\w_]+(?:\s*\*)?)\s+([\w_]+)\s*(?:\[(\d+)\])?\s*;)");
            std::sregex_iterator fieldIt(fields.begin(), fields.end(), fieldRegex);
            
            while (fieldIt != end) {
                std::smatch fieldMatch = *fieldIt;
                CParameter field;
                field.type = fieldMatch[1].str();
                field.name = fieldMatch[2].str();
                field.isPointer = field.type.find('*') != std::string::npos;
                if (fieldMatch[3].matched) {
                    field.isArray = true;
                    field.arraySize = std::stoul(fieldMatch[3].str());
                }
                s.fields.push_back(field);
                ++fieldIt;
            }
            
            structs_.push_back(s);
            ++it;
        }
    }
    
    void parseEnums(const std::string& content) {
        std::regex enumRegex(
            R"((?:typedef\s+)?enum\s*(\w*)\s*\{([^}]*)\}\s*(?:(\w+)\s*)?;)",
            std::regex::multiline
        );
        
        std::sregex_iterator it(content.begin(), content.end(), enumRegex);
        std::sregex_iterator end;
        
        while (it != end) {
            std::smatch match = *it;
            CEnum e;
            
            e.name = match[1].str().empty() ? match[3].str() : match[1].str();
            std::string values = match[2].str();
            
            // (AR) تحليل القيم
            std::regex valueRegex(R"((\w+)\s*(?:=\s*(\d+))?\s*,?)");
            std::sregex_iterator valueIt(values.begin(), values.end(), valueRegex);
            
            while (valueIt != end) {
                std::smatch valueMatch = *valueIt;
                std::string valueName = valueMatch[1].str();
                std::optional<int> valueNum;
                if (valueMatch[2].matched) {
                    valueNum = std::stoi(valueMatch[2].str());
                }
                e.values.push_back({valueName, valueNum});
                ++valueIt;
            }
            
            enums_.push_back(e);
            ++it;
        }
    }
    
    void parseTypedefs(const std::string& content) {
        std::regex typedefRegex(
            R"(typedef\s+([\w_\s\*]+)\s+(\w+)\s*;)",
            std::regex::multiline
        );
        
        std::sregex_iterator it(content.begin(), content.end(), typedefRegex);
        std::sregex_iterator end;
        
        while (it != end) {
            std::smatch match = *it;
            CTypedef td;
            td.original = trim(match[1].str());
            td.alias = match[2].str();
            
            // (AR) تخطي typedef struct/enum
            if (td.original.find("struct") == std::string::npos &&
                td.original.find("enum") == std::string::npos) {
                typedefs_.push_back(td);
            }
            ++it;
        }
    }
    
    void parseConstants(const std::string& content) {
        std::regex defineRegex(
            R"(#define\s+(\w+)\s+(.+)$)",
            std::regex::multiline
        );
        
        std::sregex_iterator it(content.begin(), content.end(), defineRegex);
        std::sregex_iterator end;
        
        while (it != end) {
            std::smatch match = *it;
            CConstant c;
            c.name = match[1].str();
            c.value = trim(match[2].str());
            
            // (AR) تحديد النوع
            if (c.value.find('.') != std::string::npos) {
                c.type = "double";
            } else if (c.value.find('"') != std::string::npos) {
                c.type = "const char*";
            } else {
                c.type = "int";
            }
            
            constants_.push_back(c);
            ++it;
        }
    }
    
    void inferOwnership(CFunction& func) {
        // (AR) الدوال التي تُرجع ذاكرة مملوكة
        static const std::set<std::string> allocators = {
            "malloc", "calloc", "realloc", "strdup", "strndup",
            "fopen", "popen", "opendir", "fdopen", "tmpfile"
        };
        
        if (allocators.count(func.name)) {
            func.ownsResult = true;
            
            // (AR) تحديد دالة التحرير
            static const std::map<std::string, std::string> deallocators = {
                {"malloc", "free"}, {"calloc", "free"}, {"realloc", "free"},
                {"strdup", "free"}, {"strndup", "free"},
                {"fopen", "fclose"}, {"popen", "pclose"},
                {"opendir", "closedir"}, {"fdopen", "fclose"},
                {"tmpfile", "fclose"}
            };
            
            auto it = deallocators.find(func.name);
            if (it != deallocators.end()) {
                func.freedBy = it->second;
            }
        }
    }
    
    std::string trim(const std::string& s) {
        auto start = s.find_first_not_of(" \t\n\r");
        if (start == std::string::npos) return "";
        auto end = s.find_last_not_of(" \t\n\r");
        return s.substr(start, end - start + 1);
    }
    
    std::string currentFile_;
    std::vector<CFunction> functions_;
    std::vector<CStruct> structs_;
    std::vector<CEnum> enums_;
    std::vector<CTypedef> typedefs_;
    std::vector<CConstant> constants_;
};

// ============================================================================
//                    (AR) مولّد ربطات لغة ص
//                    (EN) Sad Language Bindings Generator
// ============================================================================

/**
 * @struct BindgenConfig
 * @brief (AR) إعدادات مولّد الربط
 *        (EN) Bindgen configuration
 */
struct BindgenConfig {
    bool generateSafeWrappers = false;      // (AR) توليد أغلفة آمنة
    bool translateNames = false;             // (AR) ترجمة الأسماء للعربية
    std::string prefix;                      // (AR) بادئة للأسماء
    std::string outputPath;                  // (AR) مسار الخرج
    std::vector<std::string> includePaths;   // (AR) مسارات البحث
    bool jsonOutput = false;                 // (AR) إخراج JSON
    bool verbose = false;                    // (AR) وضع مفصّل
    std::string libraryName;                 // (AR) اسم المكتبة
};

/**
 * @class SadBindingsGenerator
 * @brief (AR) مولّد ربطات لغة ص من C
 *        (EN) Sad bindings generator from C
 */
class SadBindingsGenerator {
public:
    SadBindingsGenerator(const BindgenConfig& config) : config_(config) {}
    
    /**
     * (AR) توليد الربطات
     * (EN) Generate bindings
     */
    std::string generate(const CHeaderParser& parser) {
        std::ostringstream out;
        
        // (AR) رأس الملف
        out << generateHeader();
        
        // (AR) الثوابت
        if (!parser.getConstants().empty()) {
            out << "\n// ═══════════════════════════════════════════════════════════════\n";
            out << u8"// الثوابت\n";
            out << "// ═══════════════════════════════════════════════════════════════\n\n";
            for (const auto& c : parser.getConstants()) {
                out << generateConstant(c);
            }
        }
        
        // (AR) الأنواع المستعارة
        if (!parser.getTypedefs().empty()) {
            out << "\n// ═══════════════════════════════════════════════════════════════\n";
            out << u8"// الأنواع المستعارة\n";
            out << "// ═══════════════════════════════════════════════════════════════\n\n";
            for (const auto& td : parser.getTypedefs()) {
                out << generateTypedef(td);
            }
        }
        
        // (AR) التعدادات
        if (!parser.getEnums().empty()) {
            out << "\n// ═══════════════════════════════════════════════════════════════\n";
            out << u8"// التعدادات\n";
            out << "// ═══════════════════════════════════════════════════════════════\n\n";
            for (const auto& e : parser.getEnums()) {
                out << generateEnum(e);
            }
        }
        
        // (AR) البنى
        if (!parser.getStructs().empty()) {
            out << "\n// ═══════════════════════════════════════════════════════════════\n";
            out << u8"// البنى\n";
            out << "// ═══════════════════════════════════════════════════════════════\n\n";
            for (const auto& s : parser.getStructs()) {
                out << generateStruct(s);
            }
        }
        
        // (AR) كتلة extern "C"
        out << "\n// ═══════════════════════════════════════════════════════════════\n";
        out << u8"// الدوال الخارجية\n";
        out << "// ═══════════════════════════════════════════════════════════════\n\n";
        out << ArabicKeywords::EXTERN_C << " {\n\n";
        
        for (const auto& func : parser.getFunctions()) {
            out << generateFunction(func);
        }
        
        out << "}\n";
        
        // (AR) الأغلفة الآمنة
        if (config_.generateSafeWrappers) {
            out << "\n// ═══════════════════════════════════════════════════════════════\n";
            out << u8"// الأغلفة الآمنة\n";
            out << "// ═══════════════════════════════════════════════════════════════\n\n";
            
            for (const auto& func : parser.getFunctions()) {
                if (func.ownsResult || func.returnsPointer) {
                    out << generateSafeWrapper(func);
                }
            }
        }
        
        return out.str();
    }
    
private:
    std::string generateHeader() {
        std::ostringstream out;
        
        out << "/**\n";
        out << u8" * @file ربط_" << config_.libraryName << ".sad\n";
        out << u8" * @brief ربطات لغة ص لمكتبة " << config_.libraryName << "\n";
        out << " * \n";
        out << u8" * @details تم توليد هذا الملف تلقائياً بواسطة ص_ولّد_ربط\n";
        out << u8" *          لا تعدّل يدوياً - أي تغييرات ستُفقد عند إعادة التوليد\n";
        out << " * \n";
        out << u8" * @generated " << getCurrentDate() << "\n";
        out << " */\n\n";
        
        return out.str();
    }
    
    std::string generateConstant(const CConstant& c) {
        std::ostringstream out;
        
        std::string name = c.name;
        if (config_.translateNames) {
            name = translator_.translateFunctionName(c.name);
        }
        if (!config_.prefix.empty()) {
            name = config_.prefix + name;
        }
        
        out << ArabicKeywords::CONST << " " << name << ": ";
        out << cTypeToSadType(c.type) << " = " << c.value << ";\n";
        
        return out.str();
    }
    
    std::string generateTypedef(const CTypedef& td) {
        std::ostringstream out;
        
        std::string alias = td.alias;
        if (config_.translateNames) {
            alias = translator_.translateTypeName(td.alias);
        }
        if (!config_.prefix.empty()) {
            alias = config_.prefix + alias;
        }
        
        out << ArabicKeywords::TYPE << " " << alias << " = ";
        out << cTypeToSadType(td.original) << ";\n";
        
        return out.str();
    }
    
    std::string generateEnum(const CEnum& e) {
        std::ostringstream out;
        
        std::string name = e.name;
        if (config_.translateNames) {
            name = translator_.translateTypeName(e.name);
        }
        if (!config_.prefix.empty()) {
            name = config_.prefix + name;
        }
        
        out << ArabicKeywords::ENUM << " " << name << " {\n";
        
        for (const auto& [valueName, valueNum] : e.values) {
            out << "    " << valueName;
            if (valueNum) {
                out << " = " << *valueNum;
            }
            out << ",\n";
        }
        
        out << "}\n\n";
        
        return out.str();
    }
    
    std::string generateStruct(const CStruct& s) {
        std::ostringstream out;
        
        std::string name = s.name;
        if (config_.translateNames) {
            name = translator_.translateTypeName(s.name);
        }
        if (!config_.prefix.empty()) {
            name = config_.prefix + name;
        }
        
        out << "#[repr(C)]\n";
        out << ArabicKeywords::STRUCT << " " << name << " {\n";
        
        for (const auto& field : s.fields) {
            std::string fieldName = field.name;
            if (config_.translateNames) {
                fieldName = translator_.translateFunctionName(field.name);
            }
            
            out << "    " << fieldName << ": ";
            out << cTypeToSadType(field.type);
            if (field.isArray && field.arraySize > 0) {
                out << "[" << field.arraySize << "]";
            }
            out << ",\n";
        }
        
        out << "}\n\n";
        
        return out.str();
    }
    
    std::string generateFunction(const CFunction& func) {
        std::ostringstream out;
        
        std::string name = func.name;
        if (config_.translateNames) {
            name = translator_.translateFunctionName(func.name);
        }
        if (!config_.prefix.empty()) {
            name = config_.prefix + name;
        }
        
        // (AR) تعليقات الملكية
        if (func.ownsResult) {
            out << "    #[" << ArabicKeywords::OWNS_RESULT << "]\n";
            if (!func.freedBy.empty()) {
                out << "    #[" << ArabicKeywords::FREED_BY << ": " << func.freedBy << "]\n";
            }
        }
        
        // (AR) توقيع الدالة
        out << "    " << ArabicKeywords::FUNCTION << " " << name << "(";
        
        for (size_t i = 0; i < func.params.size(); i++) {
            if (i > 0) out << ", ";
            
            const auto& param = func.params[i];
            std::string paramName = param.name;
            if (config_.translateNames) {
                paramName = translator_.translateFunctionName(param.name);
            }
            
            out << paramName << ": ";
            if (param.isPointer) {
                if (param.isConst) {
                    out << "*" << ArabicKeywords::CONST << " ";
                } else {
                    out << "*" << ArabicKeywords::MUT << " ";
                }
            }
            out << cTypeToSadType(param.type);
        }
        
        if (func.isVariadic) {
            if (!func.params.empty()) out << ", ";
            out << "...";
        }
        
        out << ")";
        
        // (AR) نوع الإرجاع
        if (func.returnType != "void") {
            out << " -> ";
            if (func.returnsPointer) {
                out << "*" << ArabicKeywords::MUT << " ";
            }
            out << cTypeToSadType(func.returnType);
        }
        
        out << ";\n\n";
        
        return out.str();
    }
    
    std::string generateSafeWrapper(const CFunction& func) {
        std::ostringstream out;
        
        std::string rawName = func.name;
        std::string safeName = rawName;
        
        if (config_.translateNames) {
            safeName = translator_.translateFunctionName(func.name) + u8"_آمن";
        } else {
            safeName = rawName + "_safe";
        }
        
        if (!config_.prefix.empty()) {
            safeName = config_.prefix + safeName;
        }
        
        // (AR) توليد دالة آمنة
        out << u8"/// غلاف آمن لـ " << rawName << "\n";
        out << ArabicKeywords::FUNCTION << " " << safeName << "(";
        
        // (AR) المعاملات
        for (size_t i = 0; i < func.params.size(); i++) {
            if (i > 0) out << ", ";
            const auto& param = func.params[i];
            out << param.name << ": " << cTypeToSadType(param.type);
        }
        
        out << ")";
        
        // (AR) نوع الإرجاع - نتيجة
        if (func.returnType != "void") {
            out << u8" -> نتيجة<";
            out << cTypeToSadType(func.returnType);
            out << u8", خطأ_ffi>";
        }
        
        out << " {\n";
        
        // (AR) جسم الدالة
        if (func.ownsResult) {
            out << u8"    متغير نتيجة = " << ArabicKeywords::UNSAFE << " { " << rawName << "(";
            for (size_t i = 0; i < func.params.size(); i++) {
                if (i > 0) out << ", ";
                out << func.params[i].name;
            }
            out << ") };\n\n";
            
            out << u8"    إذا نتيجة.فارغ؟() {\n";
            out << u8"        إرجاع خطأ(خطأ_ffi::فشل_تخصيص);\n";
            out << "    }\n\n";
            
            out << u8"    إرجاع نجاح(نتيجة);\n";
        } else {
            out << "    " << ArabicKeywords::UNSAFE << " { " << rawName << "(";
            for (size_t i = 0; i < func.params.size(); i++) {
                if (i > 0) out << ", ";
                out << func.params[i].name;
            }
            out << ") }\n";
        }
        
        out << "}\n\n";
        
        return out.str();
    }
    
    std::string cTypeToSadType(const std::string& cType) {
        // (AR) إزالة const و *
        std::string clean = cType;
        clean = std::regex_replace(clean, std::regex("const\\s+"), "");
        clean = std::regex_replace(clean, std::regex("\\*"), "");
        clean = std::regex_replace(clean, std::regex("\\s+"), "");
        
        // (AR) جدول التحويل
        static const std::map<std::string, std::string> typeMap = {
            {"void", ArabicKeywords::VOID},
            {"char", ArabicKeywords::CHAR},
            {"signedchar", ArabicKeywords::INT8},
            {"unsignedchar", ArabicKeywords::UINT8},
            {"short", ArabicKeywords::INT16},
            {"unsignedshort", ArabicKeywords::UINT16},
            {"int", ArabicKeywords::INT32},
            {"unsignedint", ArabicKeywords::UINT32},
            {"long", ArabicKeywords::INT64},
            {"unsignedlong", ArabicKeywords::UINT64},
            {"longlong", ArabicKeywords::INT64},
            {"unsignedlonglong", ArabicKeywords::UINT64},
            {"float", ArabicKeywords::FLOAT32},
            {"double", ArabicKeywords::FLOAT64},
            {"size_t", ArabicKeywords::SIZE},
            {"_Bool", ArabicKeywords::BOOL},
            {"bool", ArabicKeywords::BOOL},
            {"int8_t", ArabicKeywords::INT8},
            {"int16_t", ArabicKeywords::INT16},
            {"int32_t", ArabicKeywords::INT32},
            {"int64_t", ArabicKeywords::INT64},
            {"uint8_t", ArabicKeywords::UINT8},
            {"uint16_t", ArabicKeywords::UINT16},
            {"uint32_t", ArabicKeywords::UINT32},
            {"uint64_t", ArabicKeywords::UINT64},
        };
        
        auto it = typeMap.find(clean);
        if (it != typeMap.end()) {
            return it->second;
        }
        
        // (AR) ترجمة النوع إذا كان معروفاً
        if (config_.translateNames) {
            return translator_.translateTypeName(clean);
        }
        
        return clean;
    }
    
    std::string getCurrentDate() {
        auto now = std::time(nullptr);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d", std::localtime(&now));
        return buf;
    }
    
    BindgenConfig config_;
    NameTranslator translator_;
};

// ============================================================================
//                    (AR) واجهة سطر الأوامر
//                    (EN) Command Line Interface
// ============================================================================

/**
 * @brief (AR) طباعة رسالة الاستخدام
 *        (EN) Print usage message
 */
void printUsage(const std::string& programName) {
    std::cout << u8"\n";
    std::cout << u8"╔═══════════════════════════════════════════════════════════════════╗\n";
    std::cout << u8"║                        ص_ولّد_ربط                                   ║\n";
    std::cout << u8"║              مولّد ربطات لغة ص من ملفات C الرأسية                   ║\n";
    std::cout << u8"╚═══════════════════════════════════════════════════════════════════╝\n\n";
    
    std::cout << u8"الاستخدام:\n";
    std::cout << "  " << programName << u8" <ملف.h> [خيارات]\n\n";
    
    std::cout << u8"الخيارات:\n";
    std::cout << u8"  -o, --output <ملف>      ملف الخرج (.sad)\n";
    std::cout << u8"  --آمن, --safe           توليد أغلفة آمنة مع RAII\n";
    std::cout << u8"  --ترجم-أسماء, --translate ترجمة أسماء الدوال للعربية\n";
    std::cout << u8"  --بادئة, --prefix <نص>  بادئة للأسماء المولّدة\n";
    std::cout << u8"  -I, --include <مسار>    مسار البحث عن ملفات الرأس\n";
    std::cout << u8"  --مكتبة, --library <اسم> اسم المكتبة\n";
    std::cout << u8"  --json                  إخراج JSON للمعالجة البرمجية\n";
    std::cout << u8"  --مفصّل, --verbose      وضع التشغيل المفصّل\n";
    std::cout << u8"  -h, --مساعدة, --help    عرض هذه الرسالة\n";
    std::cout << u8"  -v, --نسخة, --version   عرض رقم النسخة\n\n";
    
    std::cout << u8"أمثلة:\n";
    std::cout << u8"  # توليد ربطات بسيطة\n";
    std::cout << "  " << programName << u8" stdio.h -o ربط_stdio.sad\n\n";
    
    std::cout << u8"  # توليد مع ترجمة وأغلفة آمنة\n";
    std::cout << "  " << programName << u8" mylib.h --آمن --ترجم-أسماء -o ربط_مكتبتي.sad\n\n";
    
    std::cout << u8"  # مع بادئة ومسار بحث\n";
    std::cout << "  " << programName << u8" header.h -I/usr/include --بادئة مك_ -o ربط.sad\n\n";
}

/**
 * @brief (AR) تحليل سطر الأوامر
 *        (EN) Parse command line arguments
 */
BindgenConfig parseArguments(int argc, char* argv[]) {
    BindgenConfig config;
    std::string inputFile;
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help" || arg == u8"--مساعدة") {
            printUsage(argv[0]);
            exit(0);
        }
        else if (arg == "-v" || arg == "--version" || arg == u8"--نسخة") {
            std::cout << u8"ص_ولّد_ربط الإصدار 1.0.0\n";
            exit(0);
        }
        else if (arg == "-o" || arg == "--output") {
            if (i + 1 < argc) {
                config.outputPath = argv[++i];
            }
        }
        else if (arg == "--safe" || arg == u8"--آمن") {
            config.generateSafeWrappers = true;
        }
        else if (arg == "--translate" || arg == u8"--ترجم-أسماء") {
            config.translateNames = true;
        }
        else if (arg == "--prefix" || arg == u8"--بادئة") {
            if (i + 1 < argc) {
                config.prefix = argv[++i];
            }
        }
        else if (arg == "-I" || arg == "--include") {
            if (i + 1 < argc) {
                config.includePaths.push_back(argv[++i]);
            }
        }
        else if (arg == "--library" || arg == u8"--مكتبة") {
            if (i + 1 < argc) {
                config.libraryName = argv[++i];
            }
        }
        else if (arg == "--json") {
            config.jsonOutput = true;
        }
        else if (arg == "--verbose" || arg == u8"--مفصّل") {
            config.verbose = true;
        }
        else if (arg[0] != '-') {
            inputFile = arg;
        }
    }
    
    if (inputFile.empty()) {
        std::cerr << u8"خطأ: لم يتم تحديد ملف الإدخال\n";
        printUsage(argv[0]);
        exit(1);
    }
    
    // (AR) تعيين القيم الافتراضية
    if (config.outputPath.empty()) {
        fs::path input(inputFile);
        config.outputPath = u8"ربط_" + input.stem().string() + ".sad";
    }
    
    if (config.libraryName.empty()) {
        fs::path input(inputFile);
        config.libraryName = input.stem().string();
    }
    
    return config;
}

} // namespace bindgen
} // namespace sad

// ============================================================================
//                    (AR) نقطة الدخول الرئيسية
//                    (EN) Main Entry Point
// ============================================================================

int main(int argc, char* argv[]) {
    using namespace sad::bindgen;
    
    // (AR) تعيين ترميز UTF-8 للإخراج
    #ifdef _WIN32
    SetConsoleOutputCP(65001);
    #endif
    
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }
    
    // (AR) تحليل الوسائط
    BindgenConfig config = parseArguments(argc, argv);
    
    // (AR) البحث عن ملف الإدخال
    std::string inputFile = argv[1];
    
    if (config.verbose) {
        std::cout << u8"جاري تحليل: " << inputFile << "\n";
    }
    
    // (AR) تحليل الملف
    CHeaderParser parser;
    if (!parser.parseFile(inputFile)) {
        return 1;
    }
    
    if (config.verbose) {
        std::cout << u8"تم العثور على:\n";
        std::cout << u8"  - " << parser.getFunctions().size() << u8" دالة\n";
        std::cout << u8"  - " << parser.getStructs().size() << u8" بنية\n";
        std::cout << u8"  - " << parser.getEnums().size() << u8" تعداد\n";
        std::cout << u8"  - " << parser.getTypedefs().size() << u8" نوع مستعار\n";
        std::cout << u8"  - " << parser.getConstants().size() << u8" ثابت\n";
    }
    
    // (AR) توليد الربطات
    SadBindingsGenerator generator(config);
    std::string output = generator.generate(parser);
    
    // (AR) كتابة الملف
    std::ofstream outFile(config.outputPath);
    if (!outFile.is_open()) {
        std::cerr << u8"خطأ: لا يمكن فتح ملف الخرج: " << config.outputPath << "\n";
        return 1;
    }
    
    outFile << output;
    outFile.close();
    
    std::cout << u8"✓ تم توليد الربطات بنجاح: " << config.outputPath << "\n";
    
    if (config.generateSafeWrappers) {
        std::cout << u8"✓ تم توليد الأغلفة الآمنة\n";
    }
    
    if (config.translateNames) {
        std::cout << u8"✓ تمت ترجمة الأسماء للعربية\n";
    }
    
    return 0;
}
