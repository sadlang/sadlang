// تعطيل تحذير Unicode للنصوص العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file bindgen.cpp
 * @brief (AR) تنفيذ أداة توليد الربط التلقائي - Bindgen
 *        (EN) Implementation of Automatic Binding Generator - Bindgen
 * 
 * @details
 * (AR) هذا الملف يحتوي على التنفيذ الكامل لأداة bindgen.
 *      يتضمن:
 *      - قراءة وتحليل ملفات رأس C
 *      - استخراج الدوال والأنواع
 *      - توليد كود "ص" المناسب
 *      - ترجمة الأسماء إلى العربية
 * 
 * (EN) This file contains the complete implementation of the bindgen tool.
 *      Includes:
 *      - Reading and parsing C header files
 *      - Extracting functions and types
 *      - Generating appropriate Sad code
 *      - Translating names to Arabic
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#include "bindgen.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <regex>
#include <algorithm>
#include <cctype>

namespace Sad {
namespace Tools {

// ============================================================================
// (AR) الكلمات العربية المستخدمة في التوليد
// (EN) Arabic keywords used in generation
// ============================================================================

namespace ArabicBindgenKeywords {
    // (AR) كلمات extern block / (EN) Extern block keywords
    const char* EXTERN = "\xD8\xAE\xD8\xA7\xD8\xB1\xD8\xAC\xD9\x8A";                    // خارجي
    const char* FUNCTION = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9";                          // دالة
    const char* STRUCT = "\xD8\xA8\xD9\x86\xD9\x8A\xD8\xA9";                            // بنية
    const char* UNION = "\xD8\xA7\xD8\xAA\xD8\xAD\xD8\xA7\xD8\xAF";                     // اتحاد
    const char* ENUM = "\xD8\xAA\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF";                      // تعداد
    const char* CONSTANT = "\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA";                          // ثابت
    const char* TYPE = "\xD9\x86\xD9\x88\xD8\xB9";                                       // نوع
    const char* END = "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";                       // نهاية
    
    // (AR) أنواع البيانات / (EN) Data types
    const char* VOID = "\xD9\x81\xD8\xB1\xD8\xA7\xD8\xBA";                              // فراغ
    const char* CHAR = "\xD8\xAD\xD8\xB1\xD9\x81";                                      // حرف
    const char* INT = "\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD";                               // صحيح
    const char* FLOAT = "\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A";                             // عشري
    const char* DOUBLE = "\xD9\x85\xD8\xB2\xD8\xAF\xD9\x88\xD8\xAC";                    // مزدوج
    const char* BOOL = "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A";                      // منطقي
    const char* POINTER = "\xD9\x85\xD8\xA4\xD8\xB4\xD8\xB1";                           // مؤشر
    const char* CONST = "\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA";                             // ثابت
    
    // (AR) تعليقات / (EN) Comments
    const char* GENERATED_FILE = "\xD9\x85\xD9\x84\xD9\x81 "
        "\xD9\x85\xD9\x88\xD9\x84\xD9\x91\xD8\xAF "
        "\xD8\xAA\xD9\x84\xD9\x82\xD8\xA7\xD8\xA6\xD9\x8A\xD8\xA7\xD9\x8B";            // ملف مولّد تلقائياً
    const char* DO_NOT_EDIT = "\xD9\x84\xD8\xA7 "
        "\xD8\xAA\xD8\xB9\xD8\xAF\xD9\x91\xD9\x84 "
        "\xD9\x8A\xD8\xAF\xD9\x88\xD9\x8A\xD8\xA7\xD9\x8B";                             // لا تعدّل يدوياً
    const char* FROM_FILE = "\xD9\x85\xD9\x86 \xD9\x85\xD9\x84\xD9\x81";                 // من ملف
    const char* SAFE_WRAPPER = "\xD8\xBA\xD9\x84\xD8\xA7\xD9\x81 "
        "\xD8\xA2\xD9\x85\xD9\x86";                                                      // غلاف آمن
}

// ============================================================================
//                    (AR) تنفيذ BindgenConfig
//                    (EN) BindgenConfig Implementation
// ============================================================================

BindgenConfig::BindgenConfig() {
    /*
     * (AR) الإعدادات الافتراضية:
     *      - توليد أسماء عربية: نعم
     *      - توليد التوثيق: نعم
     *      - توليد أغلفة آمنة: نعم
     *      - المنصة: Windows 64-bit
     * 
     * (EN) Default settings:
     *      - Generate Arabic names: yes
     *      - Generate documentation: yes
     *      - Generate safe wrappers: yes
     *      - Platform: Windows 64-bit
     */
}

void BindgenConfig::addTypeOverride(const std::string& cType, const std::string& sadType) {
    typeOverrides_[cType] = sadType;
}

bool BindgenConfig::validate(std::string& errorMessage) const {
    /*
     * (AR) التحقق من صحة الإعدادات:
     *      1. وجود ملفات إدخال
     *      2. صحة مسار الإخراج
     *      3. وجود مسارات include
     * 
     * (EN) Validate configuration:
     *      1. Input files exist
     *      2. Output path is valid
     *      3. Include paths exist
     */
    
    if (inputFiles_.empty()) {
        errorMessage = "(AR) لا توجد ملفات إدخال محددة / (EN) No input files specified";
        return false;
    }
    
    if (outputFile_.empty() && outputDir_.empty()) {
        errorMessage = "(AR) لم يتم تحديد مسار الإخراج / (EN) No output path specified";
        return false;
    }
    
    // (AR) التحقق من وجود ملفات الإدخال
    for (const auto& file : inputFiles_) {
        if (!std::filesystem::exists(file)) {
            errorMessage = "(AR) الملف غير موجود: " + file + 
                          " / (EN) File not found: " + file;
            return false;
        }
    }
    
    return true;
}

// ============================================================================
//                    (AR) تنفيذ CHeaderParser
//                    (EN) CHeaderParser Implementation
// ============================================================================

CHeaderParser::CHeaderParser(const BindgenConfig& config)
    : config_(config) {
    /*
     * (AR) تهيئة المحلل
     * (EN) Initialize parser
     */
}

/**
 * @brief (AR) تحليل ملف رأس واحد
 *        (EN) Parse single header file
 */
CHeaderContent CHeaderParser::parseFile(const std::string& filePath) {
    CHeaderContent content;
    content.fileName = filePath;
    
    std::ifstream file(filePath);
    if (!file.is_open()) {
        errors_.push_back("(AR) \xD9\x81\xD8\xB4\xD9\x84 \xD9\x81\xD8\xAA\xD8\xAD "
                         "\xD8\xA7\xD9\x84\xD9\x85\xD9\x84\xD9\x81: " + filePath);
        return content;
    }
    
    std::string line;
    int lineNumber = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        /*
         * (AR) تحليل كل سطر:
         *      1. تجاهل التعليقات
         *      2. معالجة #include
         *      3. معالجة #define
         *      4. معالجة typedef
         *      5. معالجة struct/union/enum
         *      6. معالجة تصريحات الدوال
         * 
         * (EN) Parse each line:
         *      1. Skip comments
         *      2. Process #include
         *      3. Process #define
         *      4. Process typedef
         *      5. Process struct/union/enum
         *      6. Process function declarations
         */
        
        // (AR) تجاهل الأسطر الفارغة والتعليقات
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        if (trimmed.empty() || trimmed.substr(0, 2) == "//" || trimmed.substr(0, 2) == "/*") {
            continue;
        }
        
        // (AR) معالجة #include
        if (trimmed.substr(0, 8) == "#include") {
            parseInclude(trimmed, content);
            continue;
        }
        
        // (AR) معالجة #define (الثوابت)
        if (trimmed.substr(0, 7) == "#define") {
            parseConstant(trimmed, content);
            continue;
        }
        
        // (AR) معالجة typedef
        if (trimmed.substr(0, 7) == "typedef") {
            parseTypedef(trimmed, content);
            continue;
        }
        
        // (AR) معالجة struct
        if (trimmed.find("struct ") != std::string::npos) {
            parseStruct(file, content);
            continue;
        }
        
        // (AR) معالجة enum
        if (trimmed.find("enum ") != std::string::npos) {
            parseEnum(file, content);
            continue;
        }
        
        // (AR) معالجة تصريحات الدوال
        // (EN) Process function declarations
        if (trimmed.find("(") != std::string::npos && 
            trimmed.find(";") != std::string::npos) {
            parseFunction(trimmed, content);
        }
    }
    
    return content;
}

std::vector<CHeaderContent> CHeaderParser::parseFiles(const std::vector<std::string>& filePaths) {
    std::vector<CHeaderContent> results;
    results.reserve(filePaths.size());
    
    for (const auto& path : filePaths) {
        results.push_back(parseFile(path));
    }
    
    return results;
}

CHeaderContent CHeaderParser::parseText(const std::string& text, const std::string& fileName) {
    CHeaderContent content;
    content.fileName = fileName;
    
    std::istringstream stream(text);
    std::string line;
    
    while (std::getline(stream, line)) {
        // (AR) نفس المعالجة كما في parseFile
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        
        if (trimmed.empty()) continue;
        
        if (trimmed.substr(0, 7) == "#define") {
            parseConstant(trimmed, content);
        } else if (trimmed.find("(") != std::string::npos && 
                   trimmed.find(";") != std::string::npos) {
            parseFunction(trimmed, content);
        }
    }
    
    return content;
}

/**
 * @brief (AR) تحليل تصريح دالة
 *        (EN) Parse function declaration
 */
void CHeaderParser::parseFunction(const std::string& line, CHeaderContent& content) {
    /*
     * (AR) صيغ تصريح الدالة المدعومة:
     *      int func(int a, int b);
     *      static inline int func(void);
     *      extern int func(const char* str, ...);
     *      int __stdcall func(int x);
     * 
     * (EN) Supported function declaration formats shown above
     */
    
    // (AR) تعبير نمطي لاستخراج الدالة
    // (EN) Regex to extract function
    std::regex funcRegex(
        R"((?:extern\s+)?(?:static\s+)?(?:inline\s+)?)"
        R"((?:(__\w+)\s+)?)"                    // اصطلاح الاستدعاء
        R"((\w[\w\s\*]*?)\s+)"                  // نوع الإرجاع
        R"((\w+)\s*)"                           // اسم الدالة
        R"(\(([^)]*)\)\s*;)"                    // المعاملات
    );
    
    std::smatch match;
    if (std::regex_search(line, match, funcRegex)) {
        CFunction func;
        func.callingConvention = match[1].str();
        func.returnType = match[2].str();
        func.name = match[3].str();
        
        std::string paramString = match[4].str();
        func.parameters = extractParameters(paramString);
        func.isVariadic = (paramString.find("...") != std::string::npos);
        func.isStatic = (line.find("static") != std::string::npos);
        func.isInline = (line.find("inline") != std::string::npos);
        
        // (AR) التحقق من الفلتر
        if (matchesFilter(func.name)) {
            content.functions.push_back(func);
        }
    }
}

/**
 * @brief (AR) استخراج المعاملات من نص
 *        (EN) Extract parameters from string
 */
std::vector<CParameter> CHeaderParser::extractParameters(const std::string& paramString) {
    std::vector<CParameter> params;
    
    if (paramString.empty() || paramString == "void") {
        return params;
    }
    
    /*
     * (AR) تقسيم المعاملات بالفاصلة
     * (EN) Split parameters by comma
     */
    std::istringstream stream(paramString);
    std::string param;
    
    while (std::getline(stream, param, ',')) {
        // (AR) تنظيف المعامل
        param.erase(0, param.find_first_not_of(" \t"));
        param.erase(param.find_last_not_of(" \t") + 1);
        
        if (param.empty() || param == "...") {
            continue;
        }
        
        CParameter p;
        
        // (AR) استخراج النوع والاسم
        // (EN) Extract type and name
        size_t lastSpace = param.rfind(' ');
        if (lastSpace != std::string::npos) {
            // (AR) التعامل مع المؤشرات
            size_t asterisk = param.rfind('*');
            if (asterisk != std::string::npos && asterisk > lastSpace) {
                p.cType = param.substr(0, asterisk + 1);
                p.name = param.substr(asterisk + 1);
                p.isPointer = true;
            } else {
                p.cType = param.substr(0, lastSpace);
                p.name = param.substr(lastSpace + 1);
            }
        } else {
            p.cType = param;
            p.name = "arg" + std::to_string(params.size());
        }
        
        // (AR) تنظيف الاسم من الـ * إذا وجد
        if (!p.name.empty() && p.name[0] == '*') {
            p.name = p.name.substr(1);
            p.isPointer = true;
        }
        
        p.isConst = (param.find("const") != std::string::npos);
        
        params.push_back(p);
    }
    
    return params;
}

/**
 * @brief (AR) تحليل بنية
 *        (EN) Parse struct
 */
void CHeaderParser::parseStruct(std::istream& input, CHeaderContent& content) {
    CStruct structDef;
    std::string line;
    
    // (AR) البحث عن اسم البنية
    std::getline(input, line);
    std::regex nameRegex(R"(struct\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(line, match, nameRegex)) {
        structDef.name = match[1].str();
    }
    
    // (AR) قراءة الحقول حتى }
    while (std::getline(input, line)) {
        if (line.find("}") != std::string::npos) {
            break;
        }
        
        // (AR) استخراج الحقل
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == '/') continue;
        
        // (AR) إزالة الفاصلة المنقوطة
        if (line.back() == ';') {
            line.pop_back();
        }
        
        CStructField field;
        size_t lastSpace = line.rfind(' ');
        if (lastSpace != std::string::npos) {
            field.cType = line.substr(0, lastSpace);
            field.name = line.substr(lastSpace + 1);
            
            // (AR) التعامل مع المصفوفات
            size_t bracket = field.name.find('[');
            if (bracket != std::string::npos) {
                field.name = field.name.substr(0, bracket);
            }
            
            structDef.fields.push_back(field);
        }
    }
    
    if (matchesFilter(structDef.name)) {
        content.structs.push_back(structDef);
    }
}

/**
 * @brief (AR) تحليل تعداد
 *        (EN) Parse enum
 */
void CHeaderParser::parseEnum(std::istream& input, CHeaderContent& content) {
    CEnum enumDef;
    std::string line;
    int64_t nextValue = 0;
    
    // (AR) البحث عن اسم التعداد
    std::getline(input, line);
    std::regex nameRegex(R"(enum\s+(\w+))");
    std::smatch match;
    
    if (std::regex_search(line, match, nameRegex)) {
        enumDef.name = match[1].str();
    }
    
    // (AR) قراءة العناصر
    while (std::getline(input, line)) {
        if (line.find("}") != std::string::npos) {
            break;
        }
        
        line.erase(0, line.find_first_not_of(" \t"));
        if (line.empty() || line[0] == '/') continue;
        
        CEnumerator enumerator;
        
        // (AR) البحث عن القيمة
        size_t equals = line.find('=');
        if (equals != std::string::npos) {
            enumerator.name = line.substr(0, equals);
            enumerator.name.erase(enumerator.name.find_last_not_of(" \t") + 1);
            
            std::string valueStr = line.substr(equals + 1);
            valueStr.erase(0, valueStr.find_first_not_of(" \t"));
            size_t comma = valueStr.find(',');
            if (comma != std::string::npos) {
                valueStr = valueStr.substr(0, comma);
            }
            
            try {
                enumerator.value = std::stoll(valueStr, nullptr, 0);
                nextValue = enumerator.value + 1;
            } catch (...) {
                enumerator.value = nextValue++;
            }
        } else {
            size_t comma = line.find(',');
            enumerator.name = (comma != std::string::npos) ? 
                             line.substr(0, comma) : line;
            enumerator.name.erase(enumerator.name.find_last_not_of(" \t,") + 1);
            enumerator.value = nextValue++;
        }
        
        if (!enumerator.name.empty()) {
            enumDef.enumerators.push_back(enumerator);
        }
    }
    
    if (matchesFilter(enumDef.name)) {
        content.enums.push_back(enumDef);
    }
}

void CHeaderParser::parseTypedef(const std::string& line, CHeaderContent& content) {
    // (AR) typedef int MyInt;
    std::regex typedefRegex(R"(typedef\s+(.+?)\s+(\w+)\s*;)");
    std::smatch match;
    
    if (std::regex_search(line, match, typedefRegex)) {
        CTypedef td;
        td.originalType = match[1].str();
        td.name = match[2].str();
        
        if (matchesFilter(td.name)) {
            content.typedefs.push_back(td);
        }
    }
}

void CHeaderParser::parseConstant(const std::string& line, CHeaderContent& content) {
    /*
     * (AR) صيغ #define المدعومة:
     *      #define NAME value
     *      #define NAME (expression)
     *      // نتجاهل الـ function-like macros
     * 
     * (EN) Supported #define formats:
     *      #define NAME value
     *      #define NAME (expression)
     *      // We skip function-like macros
     */
    
    std::regex constRegex(R"(#define\s+(\w+)\s+(.+))");
    std::smatch match;
    
    if (std::regex_search(line, match, constRegex)) {
        std::string name = match[1].str();
        std::string value = match[2].str();
        
        // (AR) تجاهل الـ function-like macros
        if (name.find('(') != std::string::npos) {
            return;
        }
        
        CConstant constant;
        constant.name = name;
        constant.value = value;
        
        // (AR) استنتاج النوع
        if (value.find('.') != std::string::npos) {
            constant.inferredType = "double";
        } else if (value.find("0x") == 0 || value.find("0X") == 0) {
            constant.inferredType = "int";
        } else if (std::isdigit(value[0]) || value[0] == '-') {
            constant.inferredType = "int";
        } else if (value[0] == '"') {
            constant.inferredType = "const char*";
        } else {
            constant.inferredType = "int";
        }
        
        if (matchesFilter(constant.name)) {
            content.constants.push_back(constant);
        }
    }
}

void CHeaderParser::parseInclude(const std::string& line, CHeaderContent& content) {
    std::regex includeRegex(R"(#include\s*[<"]([^>"]+)[>"])");
    std::smatch match;
    
    if (std::regex_search(line, match, includeRegex)) {
        content.includes.push_back(match[1].str());
    }
}

bool CHeaderParser::matchesFilter(const std::string& name) const {
    /*
     * (AR) فحص الفلتر:
     *      1. إذا كان هناك whitelist، يجب المطابقة
     *      2. إذا كان هناك blacklist، يجب عدم المطابقة
     * 
     * (EN) Check filter:
     *      1. If whitelist exists, must match
     *      2. If blacklist exists, must not match
     */
    
    const auto& whitelist = config_.getWhitelistPatterns();
    const auto& blacklist = config_.getBlacklistPatterns();
    
    // (AR) فحص whitelist
    if (!whitelist.empty()) {
        bool matches = false;
        for (const auto& pattern : whitelist) {
            std::regex re(pattern);
            if (std::regex_search(name, re)) {
                matches = true;
                break;
            }
        }
        if (!matches) return false;
    }
    
    // (AR) فحص blacklist
    for (const auto& pattern : blacklist) {
        std::regex re(pattern);
        if (std::regex_search(name, re)) {
            return false;
        }
    }
    
    return true;
}

// ============================================================================
//                    (AR) تنفيذ ArabicNameTranslator
//                    (EN) ArabicNameTranslator Implementation
// ============================================================================

ArabicNameTranslator::ArabicNameTranslator() {
    initializeDefaultDictionary();
}

void ArabicNameTranslator::initializeDefaultDictionary() {
    /*
     * (AR) قاموس الترجمة الافتراضي
     *      يحتوي على الكلمات الشائعة في البرمجة
     * 
     * (EN) Default translation dictionary
     *      Contains common programming words
     */
    
    // --- (AR) أسماء الدوال الشائعة / (EN) Common function names ---
    dictionary_["printf"] = "\xD8\xA7\xD8\xB7\xD8\xA8\xD8\xB9_\xD9\x85\xD9\x86\xD8\xB3\xD9\x82";  // اطبع_منسق
    dictionary_["scanf"] = "\xD8\xA7\xD9\x82\xD8\xB1\xD8\xA3_\xD9\x85\xD9\x86\xD8\xB3\xD9\x82";   // اقرأ_منسق
    dictionary_["malloc"] = "\xD8\xA7\xD8\xAD\xD8\xAC\xD8\xB2_\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9"; // احجز_ذاكرة
    dictionary_["free"] = "\xD8\xAD\xD8\xB1\xD8\xB1_\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9";    // حرر_ذاكرة
    dictionary_["strlen"] = "\xD8\xB7\xD9\x88\xD9\x84_\xD9\x86\xD8\xB5";                          // طول_نص
    dictionary_["strcpy"] = "\xD8\xA7\xD9\x86\xD8\xB3\xD8\xAE_\xD9\x86\xD8\xB5";                  // انسخ_نص
    dictionary_["strcmp"] = "\xD9\x82\xD8\xA7\xD8\xB1\xD9\x86_\xD9\x86\xD8\xB5";                  // قارن_نص
    dictionary_["fopen"] = "\xD8\xA7\xD9\x81\xD8\xAA\xD8\xAD_\xD9\x85\xD9\x84\xD9\x81";           // افتح_ملف
    dictionary_["fclose"] = "\xD8\xA3\xD8\xBA\xD9\x84\xD9\x82_\xD9\x85\xD9\x84\xD9\x81";          // أغلق_ملف
    dictionary_["fread"] = "\xD8\xA7\xD9\x82\xD8\xB1\xD8\xA3_\xD9\x85\xD9\x84\xD9\x81";           // اقرأ_ملف
    dictionary_["fwrite"] = "\xD8\xA7\xD9\x83\xD8\xAA\xD8\xA8_\xD9\x85\xD9\x84\xD9\x81";          // اكتب_ملف
    dictionary_["exit"] = "\xD8\xA7\xD8\xAE\xD8\xB1\xD8\xAC";                                     // اخرج
    dictionary_["abort"] = "\xD8\xA3\xD9\x84\xD8\xBA\xD9\x90";                                    // ألغِ
    dictionary_["memcpy"] = "\xD8\xA7\xD9\x86\xD8\xB3\xD8\xAE_\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9"; // انسخ_ذاكرة
    dictionary_["memset"] = "\xD8\xB9\xD9\x8A\xD9\x91\xD9\x86_\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9"; // عيّن_ذاكرة
    
    // --- (AR) أسماء الأنواع / (EN) Type names ---
    dictionary_["FILE"] = "\xD9\x85\xD9\x84\xD9\x81";                                             // ملف
    dictionary_["size_t"] = "\xD8\xAD\xD8\xAC\xD9\x85";                                           // حجم
    dictionary_["time_t"] = "\xD9\x88\xD9\x82\xD8\xAA";                                           // وقت
    
    // --- (AR) كلمات شائعة / (EN) Common words ---
    dictionary_["buffer"] = "\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9_\xD9\x85\xD8\xA4\xD9\x82\xD8\xAA\xD8\xA9"; // ذاكرة_مؤقتة
    dictionary_["size"] = "\xD8\xAD\xD8\xAC\xD9\x85";                                             // حجم
    dictionary_["count"] = "\xD8\xB9\xD8\xAF\xD8\xAF";                                            // عدد
    dictionary_["length"] = "\xD8\xB7\xD9\x88\xD9\x84";                                           // طول
    dictionary_["data"] = "\xD8\xA8\xD9\x8A\xD8\xA7\xD9\x86\xD8\xA7\xD8\xAA";                     // بيانات
    dictionary_["value"] = "\xD9\x82\xD9\x8A\xD9\x85\xD8\xA9";                                    // قيمة
    dictionary_["result"] = "\xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9";                           // نتيجة
    dictionary_["error"] = "\xD8\xAE\xD8\xB7\xD8\xA3";                                            // خطأ
    dictionary_["success"] = "\xD9\x86\xD8\xAC\xD8\xA7\xD8\xAD";                                  // نجاح
    dictionary_["format"] = "\xD8\xAA\xD9\x86\xD8\xB3\xD9\x8A\xD9\x82";                           // تنسيق
    dictionary_["string"] = "\xD9\x86\xD8\xB5";                                                   // نص
    dictionary_["pointer"] = "\xD9\x85\xD8\xA4\xD8\xB4\xD8\xB1";                                  // مؤشر
    dictionary_["array"] = "\xD9\x85\xD8\xB5\xD9\x81\xD9\x88\xD9\x81\xD8\xA9";                    // مصفوفة
    dictionary_["file"] = "\xD9\x85\xD9\x84\xD9\x81";                                             // ملف
    dictionary_["path"] = "\xD9\x85\xD8\xB3\xD8\xA7\xD8\xB1";                                     // مسار
    dictionary_["name"] = "\xD8\xA7\xD8\xB3\xD9\x85";                                             // اسم
    dictionary_["type"] = "\xD9\x86\xD9\x88\xD8\xB9";                                             // نوع
    dictionary_["index"] = "\xD9\x81\xD9\x87\xD8\xB1\xD8\xB3";                                    // فهرس
    dictionary_["offset"] = "\xD8\xA5\xD8\xB2\xD8\xA7\xD8\xAD\xD8\xA9";                           // إزاحة
    dictionary_["ptr"] = "\xD9\x85\xD8\xA4\xD8\xB4\xD8\xB1";                                      // مؤشر
    dictionary_["src"] = "\xD9\x85\xD8\xB5\xD8\xAF\xD8\xB1";                                      // مصدر
    dictionary_["dest"] = "\xD9\x88\xD8\xAC\xD9\x87\xD8\xA9";                                     // وجهة
    dictionary_["dst"] = "\xD9\x88\xD8\xAC\xD9\x87\xD8\xA9";                                      // وجهة
    dictionary_["str"] = "\xD9\x86\xD8\xB5";                                                      // نص
    dictionary_["num"] = "\xD8\xB1\xD9\x82\xD9\x85";                                              // رقم
    dictionary_["max"] = "\xD8\xA3\xD9\x82\xD8\xB5\xD9\x89";                                      // أقصى
    dictionary_["min"] = "\xD8\xA3\xD8\xAF\xD9\x86\xD9\x89";                                      // أدنى
    dictionary_["input"] = "\xD9\x85\xD8\xAF\xD8\xAE\xD9\x84";                                    // مدخل
    dictionary_["output"] = "\xD9\x85\xD8\xAE\xD8\xB1\xD8\xAC";                                   // مخرج
    dictionary_["init"] = "\xD9\x87\xD9\x8A\xD8\xA6";                                             // هيئ
    dictionary_["create"] = "\xD8\xA3\xD9\x86\xD8\xB4\xD8\xA6";                                   // أنشئ
    dictionary_["destroy"] = "\xD8\xAF\xD9\x85\xD8\xB1";                                          // دمر
    dictionary_["delete"] = "\xD8\xA7\xD8\xAD\xD8\xB0\xD9\x81";                                   // احذف
    dictionary_["open"] = "\xD8\xA7\xD9\x81\xD8\xAA\xD8\xAD";                                     // افتح
    dictionary_["close"] = "\xD8\xA3\xD8\xBA\xD9\x84\xD9\x82";                                    // أغلق
    dictionary_["read"] = "\xD8\xA7\xD9\x82\xD8\xB1\xD8\xA3";                                     // اقرأ
    dictionary_["write"] = "\xD8\xA7\xD9\x83\xD8\xAA\xD8\xA8";                                    // اكتب
    dictionary_["get"] = "\xD8\xA7\xD8\xAD\xD8\xB5\xD9\x84";                                      // احصل
    dictionary_["set"] = "\xD8\xB9\xD9\x8A\xD9\x91\xD9\x86";                                      // عيّن
    dictionary_["add"] = "\xD8\xA3\xD8\xB6\xD9\x81";                                              // أضف
    dictionary_["remove"] = "\xD8\xA3\xD8\xB2\xD9\x84";                                           // أزل
    dictionary_["find"] = "\xD8\xA7\xD8\xA8\xD8\xAD\xD8\xAB";                                     // ابحث
    dictionary_["copy"] = "\xD8\xA7\xD9\x86\xD8\xB3\xD8\xAE";                                     // انسخ
    dictionary_["move"] = "\xD8\xA7\xD9\x86\xD9\x82\xD9\x84";                                     // انقل
    dictionary_["compare"] = "\xD9\x82\xD8\xA7\xD8\xB1\xD9\x86";                                  // قارن
}

std::string ArabicNameTranslator::translateFunctionName(const std::string& cName) const {
    /*
     * (AR) خوارزمية الترجمة:
     *      1. البحث في القاموس أولاً
     *      2. إذا لم يوجد، تحويل من snake_case أو camelCase
     * 
     * (EN) Translation algorithm:
     *      1. Search dictionary first
     *      2. If not found, convert from snake_case or camelCase
     */
    
    // (AR) البحث في القاموس
    auto it = dictionary_.find(cName);
    if (it != dictionary_.end()) {
        return it->second;
    }
    
    // (AR) تحويل من snake_case
    if (cName.find('_') != std::string::npos) {
        return snakeCaseToArabic(cName);
    }
    
    // (AR) تحويل من camelCase
    return camelCaseToArabic(cName);
}

std::string ArabicNameTranslator::translateTypeName(const std::string& cName) const {
    auto it = dictionary_.find(cName);
    if (it != dictionary_.end()) {
        return it->second;
    }
    
    // (AR) إرجاع الاسم الأصلي إذا لم يُترجم
    return cName;
}

std::string ArabicNameTranslator::translateParameterName(const std::string& cName) const {
    auto it = dictionary_.find(cName);
    if (it != dictionary_.end()) {
        return it->second;
    }
    
    // (AR) تحويل الاسم
    if (cName.find('_') != std::string::npos) {
        return snakeCaseToArabic(cName);
    }
    
    return camelCaseToArabic(cName);
}

std::string ArabicNameTranslator::translateConstantName(const std::string& cName) const {
    auto it = dictionary_.find(cName);
    if (it != dictionary_.end()) {
        return it->second;
    }
    
    return cName;  // (AR) الثوابت غالباً تبقى بالإنجليزية
}

void ArabicNameTranslator::addTranslation(const std::string& english, const std::string& arabic) {
    dictionary_[english] = arabic;
}

bool ArabicNameTranslator::loadDictionary(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // (AR) تنسيق الملف: english=arabic
        size_t equals = line.find('=');
        if (equals != std::string::npos) {
            std::string english = line.substr(0, equals);
            std::string arabic = line.substr(equals + 1);
            dictionary_[english] = arabic;
        }
    }
    
    return true;
}

std::string ArabicNameTranslator::camelCaseToArabic(const std::string& name) const {
    /*
     * (AR) تحويل camelCase إلى عربي:
     *      getUserName -> get_user_name -> احصل_اسم_المستخدم
     * 
     * (EN) Convert camelCase to Arabic
     */
    std::string result;
    std::string currentWord;
    
    for (char c : name) {
        if (std::isupper(c) && !currentWord.empty()) {
            // (AR) بداية كلمة جديدة
            auto it = dictionary_.find(currentWord);
            if (it != dictionary_.end()) {
                if (!result.empty()) result += "_";
                result += it->second;
            } else {
                if (!result.empty()) result += "_";
                result += currentWord;
            }
            currentWord.clear();
        }
        currentWord += std::tolower(c);
    }
    
    // (AR) الكلمة الأخيرة
    if (!currentWord.empty()) {
        auto it = dictionary_.find(currentWord);
        if (it != dictionary_.end()) {
            if (!result.empty()) result += "_";
            result += it->second;
        } else {
            if (!result.empty()) result += "_";
            result += currentWord;
        }
    }
    
    return result.empty() ? name : result;
}

std::string ArabicNameTranslator::snakeCaseToArabic(const std::string& name) const {
    /*
     * (AR) تحويل snake_case إلى عربي:
     *      get_user_name -> احصل_اسم_المستخدم
     * 
     * (EN) Convert snake_case to Arabic
     */
    std::string result;
    std::istringstream stream(name);
    std::string word;
    
    while (std::getline(stream, word, '_')) {
        if (word.empty()) continue;
        
        auto it = dictionary_.find(word);
        if (it != dictionary_.end()) {
            if (!result.empty()) result += "_";
            result += it->second;
        } else {
            if (!result.empty()) result += "_";
            result += word;
        }
    }
    
    return result.empty() ? name : result;
}

// ============================================================================
//                    (AR) تنفيذ SadBindingGenerator
//                    (EN) SadBindingGenerator Implementation
// ============================================================================

SadBindingGenerator::SadBindingGenerator(const BindgenConfig& config)
    : config_(config) {
}

/**
 * @brief (AR) توليد ملف ربط كامل
 *        (EN) Generate complete binding file
 */
std::string SadBindingGenerator::generate(const CHeaderContent& content) {
    std::ostringstream output;
    
    // (AR) إضافة رأس الملف
    output << generateFileHeader(content.fileName);
    
    // (AR) توليد الثوابت
    if (!content.constants.empty()) {
        output << "\n// " << ArabicBindgenKeywords::CONSTANT << " / Constants\n";
        for (const auto& constant : content.constants) {
            output << generateConstantBinding(constant);
        }
    }
    
    // (AR) توليد التعدادات
    if (!content.enums.empty()) {
        output << "\n// " << ArabicBindgenKeywords::ENUM << " / Enums\n";
        for (const auto& enumDef : content.enums) {
            output << generateEnumBinding(enumDef);
        }
    }
    
    // (AR) توليد البنيات
    if (!content.structs.empty()) {
        output << "\n// " << ArabicBindgenKeywords::STRUCT << " / Structs\n";
        for (const auto& structDef : content.structs) {
            output << generateStructBinding(structDef);
        }
    }
    
    // (AR) توليد الدوال في extern block
    if (!content.functions.empty()) {
        output << "\n// " << ArabicBindgenKeywords::FUNCTION << " / Functions\n";
        output << ArabicBindgenKeywords::EXTERN << " \"C\" {\n";
        
        for (const auto& func : content.functions) {
            output << indent(generateFunctionBinding(func));
        }
        
        output << "}\n";
        
        // (AR) توليد الأغلفة الآمنة إذا طُلب
        if (config_.getGenerateSafeWrappers()) {
            output << "\n// " << ArabicBindgenKeywords::SAFE_WRAPPER << " / Safe Wrappers\n";
            for (const auto& func : content.functions) {
                output << generateSafeWrapper(func);
            }
        }
    }
    
    return output.str();
}

std::string SadBindingGenerator::generate(const std::vector<CHeaderContent>& contents) {
    std::ostringstream output;
    
    // (AR) رأس الملف المجمع
    output << "// " << ArabicBindgenKeywords::GENERATED_FILE << "\n";
    output << "// " << ArabicBindgenKeywords::DO_NOT_EDIT << "\n";
    output << "// Generated by ص_ولّد_ربط (Sad Bindgen)\n";
    output << "// Date: " << __DATE__ << "\n\n";
    
    for (const auto& content : contents) {
        output << "// ============================================\n";
        output << "// " << ArabicBindgenKeywords::FROM_FILE << ": " << content.fileName << "\n";
        output << "// ============================================\n\n";
        output << generate(content);
    }
    
    return output.str();
}

bool SadBindingGenerator::writeToFile(const std::string& content, const std::string& filePath) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    return true;
}

/**
 * @brief (AR) توليد ربط لدالة واحدة
 *        (EN) Generate binding for single function
 */
std::string SadBindingGenerator::generateFunctionBinding(const CFunction& func) {
    std::ostringstream output;
    
    // (AR) التوثيق
    if (config_.getGenerateDocumentation() && !func.documentation.empty()) {
        output << generateDocComment(func.documentation);
    }
    
    // (AR) اسم الدالة
    output << ArabicBindgenKeywords::FUNCTION << " " << func.name << "(";
    
    // (AR) المعاملات
    for (size_t i = 0; i < func.parameters.size(); ++i) {
        const auto& param = func.parameters[i];
        
        if (i > 0) output << ", ";
        
        // (AR) إضافة ownership annotation إذا طُلب
        if (config_.getGenerateOwnershipAnnotations()) {
            std::string annotation = generateOwnershipAnnotation(param);
            if (!annotation.empty()) {
                output << annotation << " ";
            }
        }
        
        output << param.name << ": " << convertCTypeToSad(param.cType);
    }
    
    // (AR) الـ variadic
    if (func.isVariadic) {
        if (!func.parameters.empty()) output << ", ";
        output << "...";
    }
    
    output << ")";
    
    // (AR) نوع الإرجاع
    if (func.returnType != "void") {
        output << " -> " << convertCTypeToSad(func.returnType);
    }
    
    output << "\n";
    
    return output.str();
}

/**
 * @brief (AR) توليد ربط لبنية
 *        (EN) Generate binding for struct
 */
std::string SadBindingGenerator::generateStructBinding(const CStruct& structDef) {
    std::ostringstream output;
    
    // (AR) التوثيق
    if (config_.getGenerateDocumentation() && !structDef.documentation.empty()) {
        output << generateDocComment(structDef.documentation);
    }
    
    output << ArabicBindgenKeywords::STRUCT << " " << structDef.name;
    
    // (AR) الاسم العربي كتعليق
    if (config_.getGenerateArabicNames() && !structDef.arabicName.empty()) {
        output << " // " << structDef.arabicName;
    }
    
    output << " {\n";
    
    // (AR) الحقول
    for (const auto& field : structDef.fields) {
        output << "    " << field.name << ": " << convertCTypeToSad(field.cType);
        
        if (config_.getGenerateArabicNames() && !field.arabicName.empty()) {
            output << "  // " << field.arabicName;
        }
        
        output << "\n";
    }
    
    output << "}\n\n";
    
    return output.str();
}

/**
 * @brief (AR) توليد ربط لتعداد
 *        (EN) Generate binding for enum
 */
std::string SadBindingGenerator::generateEnumBinding(const CEnum& enumDef) {
    std::ostringstream output;
    
    output << ArabicBindgenKeywords::ENUM << " " << enumDef.name << " {\n";
    
    for (const auto& enumerator : enumDef.enumerators) {
        output << "    " << enumerator.name << " = " << enumerator.value;
        
        if (config_.getGenerateArabicNames() && !enumerator.arabicName.empty()) {
            output << "  // " << enumerator.arabicName;
        }
        
        output << "\n";
    }
    
    output << "}\n\n";
    
    return output.str();
}

/**
 * @brief (AR) توليد ربط لثابت
 *        (EN) Generate binding for constant
 */
std::string SadBindingGenerator::generateConstantBinding(const CConstant& constant) {
    std::ostringstream output;
    
    output << ArabicBindgenKeywords::CONSTANT << " " << constant.name 
           << ": " << convertCTypeToSad(constant.inferredType)
           << " = " << constant.value;
    
    if (config_.getGenerateArabicNames() && !constant.arabicName.empty()) {
        output << "  // " << constant.arabicName;
    }
    
    output << "\n";
    
    return output.str();
}

std::string SadBindingGenerator::generateFileHeader(const std::string& fileName) {
    std::ostringstream output;
    
    output << "// " << ArabicBindgenKeywords::GENERATED_FILE << "\n";
    output << "// " << ArabicBindgenKeywords::DO_NOT_EDIT << "\n";
    output << "// " << ArabicBindgenKeywords::FROM_FILE << ": " << fileName << "\n";
    output << "// Generated by sad_bindgen\n\n";
    
    return output.str();
}

/**
 * @brief (AR) تحويل نوع C إلى نوع "ص"
 *        (EN) Convert C type to Sad type
 */
std::string SadBindingGenerator::convertCTypeToSad(const std::string& cType) {
    /*
     * (AR) جدول التحويل:
     *      void     -> فراغ
     *      char     -> حرف
     *      int      -> ص32
     *      long     -> ص32/ص64
     *      float    -> عش32
     *      double   -> عش64
     *      char*    -> *حرف
     *      const char* -> *ثابت[حرف]
     * 
     * (EN) Conversion table shown above
     */
    
    std::string cleaned = cType;
    cleaned.erase(0, cleaned.find_first_not_of(" \t"));
    cleaned.erase(cleaned.find_last_not_of(" \t") + 1);
    
    // (AR) التعامل مع const
    bool isConst = cleaned.find("const") != std::string::npos;
    if (isConst) {
        size_t pos = cleaned.find("const");
        cleaned.erase(pos, 5);
        cleaned.erase(0, cleaned.find_first_not_of(" \t"));
    }
    
    // (AR) التعامل مع المؤشرات
    bool isPointer = cleaned.find('*') != std::string::npos;
    if (isPointer) {
        size_t asterisk = cleaned.find('*');
        cleaned = cleaned.substr(0, asterisk);
        cleaned.erase(cleaned.find_last_not_of(" \t") + 1);
    }
    
    // (AR) تحويل النوع الأساسي
    std::string sadType;
    
    if (cleaned == "void") {
        sadType = ArabicBindgenKeywords::VOID;
    } else if (cleaned == "char" || cleaned == "signed char") {
        sadType = ArabicBindgenKeywords::CHAR;
    } else if (cleaned == "unsigned char") {
        sadType = "\xD8\xB5_\xD8\xBA8";  // ص_غ8
    } else if (cleaned == "short") {
        sadType = "\xD8\xB516";  // ص16
    } else if (cleaned == "unsigned short") {
        sadType = "\xD8\xB5_\xD8\xBA16";  // ص_غ16
    } else if (cleaned == "int") {
        sadType = "\xD8\xB532";  // ص32
    } else if (cleaned == "unsigned int" || cleaned == "unsigned") {
        sadType = "\xD8\xB5_\xD8\xBA32";  // ص_غ32
    } else if (cleaned == "long") {
        sadType = config_.is64Bit() && config_.getTargetPlatform() != "windows" 
                 ? "\xD8\xB564" : "\xD8\xB532";  // ص64 أو ص32
    } else if (cleaned == "unsigned long") {
        sadType = config_.is64Bit() && config_.getTargetPlatform() != "windows"
                 ? "\xD8\xB5_\xD8\xBA64" : "\xD8\xB5_\xD8\xBA32";
    } else if (cleaned == "long long") {
        sadType = "\xD8\xB564";  // ص64
    } else if (cleaned == "unsigned long long") {
        sadType = "\xD8\xB5_\xD8\xBA64";  // ص_غ64
    } else if (cleaned == "float") {
        sadType = "\xD8\xB9\xD8\xB432";  // عش32
    } else if (cleaned == "double") {
        sadType = "\xD8\xB9\xD8\xB464";  // عش64
    } else if (cleaned == "bool" || cleaned == "_Bool") {
        sadType = ArabicBindgenKeywords::BOOL;
    } else if (cleaned == "size_t") {
        sadType = "\xD8\xAD\xD8\xAC\xD9\x85";  // حجم
    } else {
        sadType = cleaned;  // (AR) إبقاء الاسم كما هو
    }
    
    // (AR) إضافة المؤشر
    if (isPointer) {
        if (isConst) {
            sadType = "*" + std::string(ArabicBindgenKeywords::CONST) + "[" + sadType + "]";
        } else {
            sadType = "*" + sadType;
        }
    }
    
    return sadType;
}

std::string SadBindingGenerator::generateDocComment(const std::string& doc) {
    std::ostringstream output;
    output << "/// " << doc << "\n";
    return output.str();
}

std::string SadBindingGenerator::generateOwnershipAnnotation(const CParameter& param) {
    /*
     * (AR) إضافة annotations للملكية:
     *      - المؤشرات القابلة للتحرير: #[ملك_النتيجة]
     *      - المؤشرات للقراءة فقط: #[مستعار]
     * 
     * (EN) Add ownership annotations:
     *      - Freeable pointers: #[ملك_النتيجة]
     *      - Read-only pointers: #[مستعار]
     */
    
    if (!param.isPointer) {
        return "";
    }
    
    if (param.isConst) {
        return "#[\xD9\x85\xD8\xB3\xD8\xAA\xD8\xB9\xD8\xA7\xD8\xB1]";  // #[مستعار]
    }
    
    // (AR) المؤشرات غير الثابتة قد تُملك
    return "#[\xD9\x85\xD8\xA4\xD8\xB4\xD8\xB1_\xD9\x85\xD9\x85\xD9\x84\xD9\x88\xD9\x83]";  // #[مؤشر_مملوك]
}

std::string SadBindingGenerator::generateSafeWrapper(const CFunction& func) {
    /*
     * (AR) توليد غلاف آمن للدالة:
     *      - فحص المؤشرات null
     *      - تحرير الذاكرة تلقائياً
     *      - إرجاع نتيجة<T> بدلاً من مؤشر
     * 
     * (EN) Generate safe wrapper:
     *      - Check for null pointers
     *      - Automatic memory cleanup
     *      - Return نتيجة<T> instead of pointer
     */
    
    std::ostringstream output;
    
    // (AR) الدوال التي تُرجع مؤشرات
    bool returnsPointer = func.returnType.find('*') != std::string::npos;
    if (!returnsPointer) {
        return "";  // (AR) لا حاجة لغلاف آمن
    }
    
    std::string safeName = func.name + "_\xD8\xA2\xD9\x85\xD9\x86";  // _آمن
    std::string arabicName = translator_.translateFunctionName(func.name);
    
    output << "/// " << ArabicBindgenKeywords::SAFE_WRAPPER << " لـ " << func.name << "\n";
    output << ArabicBindgenKeywords::FUNCTION << " " << safeName << "(";
    
    for (size_t i = 0; i < func.parameters.size(); ++i) {
        if (i > 0) output << ", ";
        const auto& param = func.parameters[i];
        output << param.name << ": " << convertCTypeToSad(param.cType);
    }
    
    output << ") -> \xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9<"  // نتيجة<
           << convertCTypeToSad(func.returnType) << ">\n";
    
    // (AR) جسم الدالة
    output << "    " << ArabicBindgenKeywords::CONSTANT << " \xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9 = "
           << func.name << "(";
    
    for (size_t i = 0; i < func.parameters.size(); ++i) {
        if (i > 0) output << ", ";
        output << func.parameters[i].name;
    }
    
    output << ")\n";
    output << "    \xD8\xA5\xD8\xB0\xD8\xA7 \xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9 == \xD9\x81\xD8\xA7\xD8\xB1\xD8\xBA {\n";  // إذا نتيجة == فارغ
    output << "        \xD8\xA5\xD8\xB1\xD8\xAC\xD8\xA7\xD8\xB9 \xD8\xAE\xD8\xB7\xD8\xA3(\"\xD9\x81\xD8\xB4\xD9\x84 "
           << func.name << "\")\n";  // إرجاع خطأ
    output << "    }\n";
    output << "    \xD8\xA5\xD8\xB1\xD8\xAC\xD8\xA7\xD8\xB9 \xD9\x86\xD8\xAC\xD8\xA7\xD8\xAD(\xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9)\n";  // إرجاع نجاح
    output << ArabicBindgenKeywords::END << "\n\n";
    
    return output.str();
}

std::string SadBindingGenerator::indent(const std::string& text, int level) {
    std::string indentation(level * 4, ' ');
    std::ostringstream output;
    std::istringstream input(text);
    std::string line;
    
    while (std::getline(input, line)) {
        output << indentation << line << "\n";
    }
    
    return output.str();
}

// ============================================================================
//                    (AR) تنفيذ Bindgen
//                    (EN) Bindgen Implementation
// ============================================================================

Bindgen::Bindgen() {
}

bool Bindgen::run() {
    /*
     * (AR) خطوات التشغيل:
     *      1. التحقق من الإعدادات
     *      2. تحليل ملفات الإدخال
     *      3. توليد الربط
     *      4. كتابة الملف
     * 
     * (EN) Run steps:
     *      1. Validate configuration
     *      2. Parse input files
     *      3. Generate bindings
     *      4. Write file
     */
    
    // (AR) التحقق من الإعدادات
    std::string error;
    if (!config_.validate(error)) {
        errors_.push_back(error);
        return false;
    }
    
    // (AR) تحليل الملفات
    CHeaderParser parser(config_);
    auto contents = parser.parseFiles(config_.getInputFiles());
    
    if (parser.hasErrors()) {
        errors_ = parser.getErrors();
        stats_.errorsEncountered = errors_.size();
        return false;
    }
    
    // (AR) توليد الربط
    SadBindingGenerator generator(config_);
    std::string output = generator.generate(contents);
    
    // (AR) حساب الإحصائيات
    for (const auto& content : contents) {
        stats_.functionsGenerated += content.functions.size();
        stats_.structsGenerated += content.structs.size();
        stats_.enumsGenerated += content.enums.size();
        stats_.constantsGenerated += content.constants.size();
        stats_.typedefsGenerated += content.typedefs.size();
    }
    
    // (AR) كتابة الملف
    if (!generator.writeToFile(output, config_.getOutputFile())) {
        errors_.push_back("(AR) \xD9\x81\xD8\xB4\xD9\x84 \xD9\x83\xD8\xAA\xD8\xA7\xD8\xA8\xD8\xA9 "
                         "\xD8\xA7\xD9\x84\xD9\x85\xD9\x84\xD9\x81: " + config_.getOutputFile());
        return false;
    }
    
    return true;
}

bool Bindgen::run(int argc, char* argv[]) {
    if (!parseCommandLine(argc, argv)) {
        return false;
    }
    
    return run();
}

std::string Bindgen::generateContent() {
    std::string error;
    if (!config_.validate(error)) {
        errors_.push_back(error);
        return "";
    }
    
    CHeaderParser parser(config_);
    auto contents = parser.parseFiles(config_.getInputFiles());
    
    if (parser.hasErrors()) {
        errors_ = parser.getErrors();
        return "";
    }
    
    SadBindingGenerator generator(config_);
    return generator.generate(contents);
}

bool Bindgen::parseCommandLine(int argc, char* argv[]) {
    /*
     * (AR) تحليل سطر الأوامر:
     *      --input, -i    : ملفات الإدخال
     *      --output, -o   : ملف الإخراج
     *      --include, -I  : مسارات include
     *      --arabic       : توليد أسماء عربية
     *      --no-wrappers  : بدون أغلفة آمنة
     *      --help, -h     : المساعدة
     *      --version, -v  : الإصدار
     * 
     * (EN) Parse command line options shown above
     */
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage();
            return false;
        }
        
        if (arg == "--version" || arg == "-v") {
            printVersion();
            return false;
        }
        
        if ((arg == "--input" || arg == "-i") && i + 1 < argc) {
            config_.addInputFile(argv[++i]);
        } else if ((arg == "--output" || arg == "-o") && i + 1 < argc) {
            config_.setOutputFile(argv[++i]);
        } else if ((arg == "--include" || arg == "-I") && i + 1 < argc) {
            config_.addIncludePath(argv[++i]);
        } else if (arg == "--arabic") {
            config_.setGenerateArabicNames(true);
        } else if (arg == "--no-arabic") {
            config_.setGenerateArabicNames(false);
        } else if (arg == "--no-wrappers") {
            config_.setGenerateSafeWrappers(false);
        } else if (arg == "--wrappers") {
            config_.setGenerateSafeWrappers(true);
        } else if (arg[0] != '-') {
            // (AR) ملف إدخال بدون علامة
            config_.addInputFile(arg);
        } else {
            errors_.push_back("(AR) \xD8\xAE\xD9\x8A\xD8\xA7\xD8\xB1 \xD8\xBA\xD9\x8A\xD8\xB1 "
                             "\xD9\x85\xD8\xB9\xD8\xB1\xD9\x88\xD9\x81: " + arg);
            return false;
        }
    }
    
    return true;
}

void Bindgen::printUsage() {
    std::cout << R"(
ص_ولّد_ربط - أداة توليد روابط FFI
Sad Bindgen - FFI Binding Generator

الاستخدام / Usage:
  sad_bindgen [خيارات] <ملفات>
  sad_bindgen [options] <files>

الخيارات / Options:
  -i, --input <file>     ملف رأس C للإدخال / Input C header file
  -o, --output <file>    ملف الإخراج .sad / Output .sad file
  -I, --include <path>   مسار include / Include path
  
  --arabic               توليد أسماء عربية (افتراضي) / Generate Arabic names (default)
  --no-arabic            بدون أسماء عربية / No Arabic names
  --wrappers             توليد أغلفة آمنة (افتراضي) / Generate safe wrappers (default)
  --no-wrappers          بدون أغلفة آمنة / No safe wrappers
  
  -h, --help             عرض المساعدة / Show help
  -v, --version          عرض الإصدار / Show version

أمثلة / Examples:
  sad_bindgen -i stdio.h -o stdio.sad
  sad_bindgen -I/usr/include raylib.h -o raylib.sad --arabic

)" << std::endl;
}

void Bindgen::printVersion() {
    std::cout << "ص_ولّد_ربط الإصدار 1.0.0\n";
    std::cout << "Sad Bindgen Version 1.0.0\n";
}

// ============================================================================
//                    (AR) نقطة الدخول
//                    (EN) Entry Point
// ============================================================================

int bindgenMain(int argc, char* argv[]) {
    Bindgen bindgen;
    
    if (!bindgen.run(argc, argv)) {
        for (const auto& error : bindgen.getErrors()) {
            std::cerr << error << std::endl;
        }
        return 1;
    }
    
    // (AR) طباعة الإحصائيات
    const auto& stats = bindgen.getStatistics();
    std::cout << "\n=== إحصائيات التوليد / Generation Statistics ===\n";
    std::cout << "الدوال / Functions: " << stats.functionsGenerated << "\n";
    std::cout << "البنيات / Structs: " << stats.structsGenerated << "\n";
    std::cout << "التعدادات / Enums: " << stats.enumsGenerated << "\n";
    std::cout << "الثوابت / Constants: " << stats.constantsGenerated << "\n";
    std::cout << "تعريفات الأنواع / Typedefs: " << stats.typedefsGenerated << "\n";
    
    return 0;
}

} // namespace Tools
} // namespace Sad
