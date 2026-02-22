/**
 * @file safe_wrapper.cpp
 * @brief (AR) تنفيذ مولد الأغلفة الآمنة لـ FFI
 *        (EN) Safe Wrapper Generator Implementation for FFI
 * 
 * @details
 * (AR) هذا الملف يحتوي على تنفيذ نظام توليد الأغلفة الآمنة.
 *      يشمل:
 *      ┌────────────────────────────────────────────────────────────┐
 *      │ 1. SafeWrapperGenerator - المولد الرئيسي                   │
 *      │ 2. ArabicFunctionTranslator - مترجم الأسماء               │
 *      │ 3. FunctionBehaviorAnalyzer - محلل السلوك                 │
 *      │ 4. WrapperEmitter - مُصدر الكود                           │
 *      └────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#include "safe_wrapper.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <fstream>
#include <iostream>

namespace sad {
namespace tools {
namespace bindgen {

// ============================================================================
//                    (AR) ثوابت وكلمات مفتاحية عربية
//                    (EN) Constants and Arabic Keywords
// ============================================================================

namespace ArabicKeywords {
    // (AR) كلمات الدالة / (EN) Function keywords
    constexpr const char* FUNCTION = "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9";      // دالة
    constexpr const char* END = "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9";    // نهاية
    constexpr const char* RETURN = "\xD8\xA5\xD8\xB1\xD8\xAC\xD8\xA7\xD8\xB9"; // إرجاع
    constexpr const char* IF = "\xD8\xA5\xD8\xB0\xD8\xA7";                      // إذا
    constexpr const char* ELSE = "\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7";            // وإلا
    constexpr const char* CONST = "\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA";          // ثابت
    constexpr const char* LET = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1";    // متغير
    
    // (AR) أنواع النتيجة / (EN) Result types
    constexpr const char* RESULT = "\xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9"; // نتيجة
    constexpr const char* SUCCESS = "\xD9\x86\xD8\xAC\xD8\xA7\xD8\xAD";        // نجاح
    constexpr const char* ERROR_TYPE = "\xD8\xAE\xD8\xB7\xD8\xA3";             // خطأ
    
    // (AR) أنواع الملكية / (EN) Ownership types
    constexpr const char* SAFE_WRAPPER = "\xD8\xBA\xD9\x84\xD8\xA7\xD9\x81_\xD8\xA2\xD9\x85\xD9\x86"; // غلاف_آمن
    constexpr const char* OWNER = "\xD9\x85\xD8\xA7\xD9\x84\xD9\x83";          // مالك
    constexpr const char* BORROWED = "\xD9\x85\xD8\xB3\xD8\xAA\xD8\xB9\xD8\xA7\xD8\xB1"; // مستعار
    
    // (AR) كلمات خاصة / (EN) Special keywords
    constexpr const char* NULL_PTR = "\xD9\x81\xD8\xA7\xD8\xB1\xD8\xBA";       // فارغ
    constexpr const char* EXTERNAL = "\xD8\xAE\xD8\xA7\xD8\xB1\xD8\xAC\xD9\x8A"; // خارجي
    constexpr const char* STRUCT = "\xD8\xA8\xD9\x86\xD9\x8A\xD8\xA9";         // بنية
    constexpr const char* TYPE = "\xD9\x86\xD9\x88\xD8\xB9";                    // نوع
    
    // (AR) كلمات التوثيق / (EN) Documentation keywords
    constexpr const char* PARAM_DOC = "\xD9\x85\xD8\xB9\xD8\xA7\xD9\x85\xD9\x84"; // معامل
    constexpr const char* RETURN_DOC = "\xD9\x8A\xD8\xB1\xD8\xAC\xD8\xB9";     // يرجع
    constexpr const char* THROWS_DOC = "\xD9\x8A\xD8\xAE\xD8\xB7\xD8\xA6";     // يخطئ
}

// ============================================================================
//                    (AR) تنفيذ SafeWrapperGenerator
//                    (EN) SafeWrapperGenerator Implementation
// ============================================================================

SafeWrapperGenerator::SafeWrapperGenerator(const WrapperConfig& config)
    : config_(config) {}

void SafeWrapperGenerator::setConfig(const WrapperConfig& config) {
    config_ = config;
}

void SafeWrapperGenerator::addFunction(const FunctionInfo& info) {
    functions_[info.name] = info;
    
    // (AR) تحليل سلوك الدالة تلقائياً إذا لم يُحدد
    // (EN) Auto-analyze function behavior if not specified
    if (info.resourceType == ResourceType::NONE) {
        auto analysis = FunctionBehaviorAnalyzer::analyze(info);
        if (analysis.resourceType != ResourceType::NONE) {
            functions_[info.name].resourceType = analysis.resourceType;
            functions_[info.name].deallocator = analysis.suggestedDeallocator;
            
            warnings_.push_back(
                std::string(u8"تم اكتشاف أن ") + info.name + 
                u8" تحجز موارد. تم اقتراح " + analysis.suggestedDeallocator + 
                u8" للتحرير. (ثقة: " + analysis.confidence + ")"
            );
        }
    }
}

void SafeWrapperGenerator::addFunction(
    const std::string& name,
    const std::string& returnType,
    const std::vector<std::pair<std::string, std::string>>& params
) {
    FunctionInfo info;
    info.name = name;
    info.returnType = returnType;
    info.arabicName = autoArabicName(name);
    
    for (const auto& [type, paramName] : params) {
        ParameterInfo param;
        param.cType = type;
        param.name = paramName;
        param.arabicName = autoArabicName(paramName);
        param.isPointer = (type.find('*') != std::string::npos);
        param.isConst = (type.find("const") != std::string::npos);
        info.parameters.push_back(param);
    }
    
    addFunction(info);
}

void SafeWrapperGenerator::setResourceType(const std::string& funcName, ResourceType type) {
    if (functions_.count(funcName)) {
        functions_[funcName].resourceType = type;
    }
}

void SafeWrapperGenerator::setDeallocator(const std::string& funcName, const std::string& deallocator) {
    if (functions_.count(funcName)) {
        functions_[funcName].deallocator = deallocator;
    }
}

void SafeWrapperGenerator::setNullBehavior(const std::string& funcName, NullBehavior behavior) {
    if (functions_.count(funcName)) {
        functions_[funcName].nullBehavior = behavior;
    }
}

void SafeWrapperGenerator::setArabicName(const std::string& funcName, const std::string& arabicName) {
    if (functions_.count(funcName)) {
        functions_[funcName].arabicName = arabicName;
    }
}

GeneratedWrapper SafeWrapperGenerator::generateAll() {
    GeneratedWrapper result;
    
    result.sadCode = generateSadBindings();
    result.cppGlue = generateCppGlue();
    
    if (config_.generateTests) {
        result.tests = generateTests();
    }
    
    if (config_.generateDocs) {
        result.documentation = generateDocumentation();
    }
    
    result.warnings = warnings_;
    
    return result;
}

std::string SafeWrapperGenerator::generateSadBindings() {
    WrapperEmitter emitter;
    
    // (AR) ترويسة الملف
    // (EN) File header
    emitter.addHeader(
        u8"روابط C الآمنة",
        u8"تم توليد هذا الملف تلقائياً بواسطة مولد الأغلفة الآمنة"
    );
    
    // (AR) الاستيرادات
    // (EN) Imports
    emitter.addImport(u8"stdlib/core/result");
    emitter.addImport(u8"stdlib/core/optional");
    emitter.addImport(u8"stdlib/ffi/c_abi");
    emitter.addLine("");
    
    // (AR) توليد كل الأغلفة
    // (EN) Generate all wrappers
    for (const auto& [name, info] : functions_) {
        emitter.addComment(u8"════════════════════════════════════════", false);
        emitter.addComment(info.arabicName.empty() ? name : info.arabicName);
        emitter.addComment(u8"════════════════════════════════════════", false);
        
        std::string wrapper = generateFunctionWrapper(info);
        emitter.addLine(wrapper);
        emitter.addLine("");
    }
    
    return emitter.getCode();
}

std::string SafeWrapperGenerator::generateFunctionWrapper(const FunctionInfo& info) {
    std::ostringstream code;
    
    // (AR) توليد التوثيق إذا مُفعّل
    // (EN) Generate documentation if enabled
    if (config_.generateDocs) {
        code << generateFunctionDoc(info);
    }
    
    // (AR) تحديد نوع الإرجاع
    // (EN) Determine return type
    std::string returnType;
    if (config_.generateResultType && info.canFail) {
        // (AR) استخدام نتيجة<T>
        std::string sadRetType = cTypeToSad(info.returnType);
        if (config_.generateRaii && info.resourceType != ResourceType::NONE) {
            returnType = std::string(ArabicKeywords::RESULT) + "<" + 
                         ArabicKeywords::SAFE_WRAPPER + "<" + sadRetType + ">>";
        } else {
            returnType = std::string(ArabicKeywords::RESULT) + "<" + sadRetType + ">";
        }
    } else {
        returnType = cTypeToSad(info.returnType);
    }
    
    // (AR) توليد توقيع الدالة
    // (EN) Generate function signature
    std::string funcName = info.arabicName.empty() ? info.name : info.arabicName;
    code << ArabicKeywords::FUNCTION << " " << funcName << "(";
    
    // (AR) المعاملات
    // (EN) Parameters
    bool first = true;
    for (const auto& param : info.parameters) {
        if (!first) code << ", ";
        first = false;
        
        std::string paramName = param.arabicName.empty() ? param.name : param.arabicName;
        std::string paramType = cTypeToSad(param.cType);
        
        code << paramName << ": " << paramType;
    }
    
    code << ") -> " << returnType << "\n";
    
    // (AR) جسم الدالة
    // (EN) Function body
    code << generateErrorHandling(info);
    
    if (config_.trackOwnership && info.resourceType != ResourceType::NONE) {
        code << generateOwnershipTracking(info);
    }
    
    if (config_.generateRaii && info.resourceType != ResourceType::NONE) {
        code << generateRaiiWrapper(info);
    } else {
        // (AR) استدعاء مباشر
        // (EN) Direct call
        code << "    " << ArabicKeywords::CONST << " _result = " << info.name << "(";
        first = true;
        for (const auto& param : info.parameters) {
            if (!first) code << ", ";
            first = false;
            code << (param.arabicName.empty() ? param.name : param.arabicName);
        }
        code << ")\n";
        
        if (config_.generateResultType && info.canFail) {
            code << "    " << ArabicKeywords::IF << " _result == " << ArabicKeywords::NULL_PTR << " {\n";
            code << "        " << ArabicKeywords::RETURN << " " << ArabicKeywords::ERROR_TYPE 
                 << "(\"" << u8"فشل استدعاء " << info.name << "\")\n";
            code << "    }\n";
            code << "    " << ArabicKeywords::RETURN << " " << ArabicKeywords::SUCCESS << "(_result)\n";
        } else {
            code << "    " << ArabicKeywords::RETURN << " _result\n";
        }
    }
    
    code << ArabicKeywords::END << "\n";
    
    return code.str();
}

std::string SafeWrapperGenerator::generateRaiiWrapper(const FunctionInfo& info) {
    std::ostringstream code;
    
    // (AR) استدعاء الدالة الأصلية
    // (EN) Call original function
    code << "    " << ArabicKeywords::CONST << " _raw = " << info.name << "(";
    bool first = true;
    for (const auto& param : info.parameters) {
        if (!first) code << ", ";
        first = false;
        code << (param.arabicName.empty() ? param.name : param.arabicName);
    }
    code << ")\n";
    
    // (AR) التحقق من الفشل
    // (EN) Check for failure
    code << "    " << ArabicKeywords::IF << " _raw == " << ArabicKeywords::NULL_PTR << " {\n";
    code << "        " << ArabicKeywords::RETURN << " " << ArabicKeywords::ERROR_TYPE 
         << "(\"" << u8"فشل في " << info.arabicName << "\")\n";
    code << "    }\n";
    
    // (AR) إنشاء الغلاف الآمن
    // (EN) Create safe wrapper
    code << "    " << ArabicKeywords::CONST << " _wrapper = " << ArabicKeywords::SAFE_WRAPPER 
         << "(_raw, " << info.deallocator << ")\n";
    
    code << "    " << ArabicKeywords::RETURN << " " << ArabicKeywords::SUCCESS << "(_wrapper)\n";
    
    return code.str();
}

std::string SafeWrapperGenerator::generateErrorHandling(const FunctionInfo& info) {
    std::ostringstream code;
    
    // (AR) التحقق من المعاملات الفارغة إذا مطلوب
    // (EN) Check null parameters if required
    if (info.nullBehavior == NullBehavior::CHECK_PARAM || 
        info.nullBehavior == NullBehavior::BOTH) {
        
        for (const auto& param : info.parameters) {
            if (param.isPointer && !param.isOptional) {
                std::string paramName = param.arabicName.empty() ? param.name : param.arabicName;
                code << "    " << ArabicKeywords::IF << " " << paramName << " == " 
                     << ArabicKeywords::NULL_PTR << " {\n";
                code << "        " << ArabicKeywords::RETURN << " " << ArabicKeywords::ERROR_TYPE 
                     << "(\"" << u8"المعامل " << paramName << u8" فارغ\")\n";
                code << "    }\n";
            }
        }
    }
    
    return code.str();
}

std::string SafeWrapperGenerator::generateOwnershipTracking(const FunctionInfo& info) {
    std::ostringstream code;
    
    // (AR) تسجيل التخصيص
    // (EN) Register allocation
    code << "    // " << u8"تتبع الملكية\n";
    
    return code.str();
}

std::string SafeWrapperGenerator::generateFunctionDoc(const FunctionInfo& info) {
    std::ostringstream doc;
    
    doc << "/**\n";
    
    // (AR) الوصف العربي
    // (EN) Arabic description
    if (config_.arabicDocs) {
        doc << " * @brief ";
        if (!info.arabicDescription.empty()) {
            doc << info.arabicDescription;
        } else {
            doc << u8"غلاف آمن لـ " << info.name;
        }
        doc << "\n";
    }
    
    // (AR) الوصف الإنجليزي
    // (EN) English description
    doc << " *        (EN) Safe wrapper for " << info.name << "\n";
    doc << " *\n";
    
    // (AR) المعاملات
    // (EN) Parameters
    for (const auto& param : info.parameters) {
        doc << " * @param " << (param.arabicName.empty() ? param.name : param.arabicName);
        doc << " (" << param.cType << ")\n";
    }
    
    // (AR) القيمة المُرجعة
    // (EN) Return value
    doc << " * @return " << ArabicKeywords::RESULT << "<...> " << u8"نجاح أو خطأ\n";
    doc << " */\n";
    
    return doc.str();
}

std::string SafeWrapperGenerator::cTypeToSad(const std::string& cType) {
    // (AR) جدول التحويل الأساسي
    // (EN) Basic conversion table
    static const std::map<std::string, std::string> typeMap = {
        {"void", u8"فراغ"},
        {"int", u8"ص32"},
        {"int8_t", u8"ص8"},
        {"int16_t", u8"ص16"},
        {"int32_t", u8"ص32"},
        {"int64_t", u8"ص64"},
        {"uint8_t", u8"ص_غ8"},
        {"uint16_t", u8"ص_غ16"},
        {"uint32_t", u8"ص_غ32"},
        {"uint64_t", u8"ص_غ64"},
        {"float", u8"عش32"},
        {"double", u8"عش64"},
        {"char", u8"حرف"},
        {"_Bool", u8"منطقي"},
        {"bool", u8"منطقي"},
        {"size_t", u8"حجم"},
    };
    
    // (AR) تنظيف النوع
    // (EN) Clean type
    std::string cleanType = cType;
    
    // (AR) إزالة const
    size_t constPos = cleanType.find("const");
    if (constPos != std::string::npos) {
        cleanType.erase(constPos, 5);
    }
    
    // (AR) إزالة المسافات الزائدة
    cleanType.erase(std::remove(cleanType.begin(), cleanType.end(), ' '), cleanType.end());
    
    // (AR) معالجة المؤشرات
    // (EN) Handle pointers
    int ptrCount = 0;
    while (!cleanType.empty() && cleanType.back() == '*') {
        ptrCount++;
        cleanType.pop_back();
    }
    
    // (AR) البحث في الجدول
    // (EN) Look up in table
    std::string result;
    auto it = typeMap.find(cleanType);
    if (it != typeMap.end()) {
        result = it->second;
    } else {
        result = cleanType;  // (AR) استخدام كما هو
    }
    
    // (AR) إضافة المؤشرات
    // (EN) Add pointers
    for (int i = 0; i < ptrCount; i++) {
        result = "*" + result;
    }
    
    return result;
}

std::string SafeWrapperGenerator::autoArabicName(const std::string& englishName) {
    return ArabicFunctionTranslator::translate(englishName);
}

std::string SafeWrapperGenerator::generateCppGlue() {
    std::ostringstream code;
    
    code << "// " << u8"كود الربط C++ - تم توليده تلقائياً\n";
    code << "// C++ Glue Code - Auto-generated\n\n";
    
    code << "#include <cstdint>\n";
    code << "#include <cstddef>\n\n";
    
    code << "extern \"C\" {\n\n";
    
    for (const auto& [name, info] : functions_) {
        code << "// " << info.arabicName << "\n";
        code << info.returnType << " " << name << "(";
        
        bool first = true;
        for (const auto& param : info.parameters) {
            if (!first) code << ", ";
            first = false;
            code << param.cType << " " << param.name;
        }
        
        code << ");\n\n";
    }
    
    code << "} // extern \"C\"\n";
    
    return code.str();
}

std::string SafeWrapperGenerator::generateTests() {
    std::ostringstream code;
    
    code << "// " << u8"اختبارات الأغلفة الآمنة - تم توليدها تلقائياً\n\n";
    
    code << "#include <gtest/gtest.h>\n\n";
    
    for (const auto& [name, info] : functions_) {
        code << "TEST(SafeWrapper, " << name << "_Basic) {\n";
        code << "    // TODO: " << u8"تنفيذ الاختبار\n";
        code << "}\n\n";
    }
    
    return code.str();
}

std::string SafeWrapperGenerator::generateDocumentation() {
    std::ostringstream doc;
    
    doc << u8"# توثيق الأغلفة الآمنة\n\n";
    doc << u8"## الدوال المُغلّفة\n\n";
    
    for (const auto& [name, info] : functions_) {
        doc << "### " << (info.arabicName.empty() ? name : info.arabicName) << "\n\n";
        doc << u8"**الدالة الأصلية**: `" << name << "`\n\n";
        
        if (info.resourceType != ResourceType::NONE) {
            doc << u8"**نوع المورد**: ";
            switch (info.resourceType) {
                case ResourceType::MEMORY: doc << u8"ذاكرة"; break;
                case ResourceType::FILE_HANDLE: doc << u8"ملف"; break;
                case ResourceType::SOCKET: doc << u8"شبكة"; break;
                default: doc << u8"آخر"; break;
            }
            doc << "\n\n";
            
            if (!info.deallocator.empty()) {
                doc << u8"**دالة التحرير**: `" << info.deallocator << "`\n\n";
            }
        }
        
        doc << "---\n\n";
    }
    
    return doc.str();
}

std::vector<std::string> SafeWrapperGenerator::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& [name, info] : functions_) {
        if (info.resourceType != ResourceType::NONE && info.deallocator.empty()) {
            errors.push_back(
                u8"الدالة " + name + u8" تحجز موارد لكن لم تُحدد دالة التحرير"
            );
        }
    }
    
    return errors;
}

// ============================================================================
//                    (AR) تنفيذ ArabicFunctionTranslator
//                    (EN) ArabicFunctionTranslator Implementation
// ============================================================================

std::map<std::string, std::string> ArabicFunctionTranslator::translations_;

std::string ArabicFunctionTranslator::translate(const std::string& englishName) {
    // (AR) تهيئة القاموس الافتراضي مرة واحدة
    // (EN) Initialize default dictionary once
    if (translations_.empty()) {
        initializeDefaults();
    }
    
    // (AR) البحث عن الترجمة الكاملة
    // (EN) Look for full translation
    auto it = translations_.find(englishName);
    if (it != translations_.end()) {
        return it->second;
    }
    
    // (AR) محاولة ترجمة الأجزاء
    // (EN) Try translating parts
    std::string result;
    std::string current;
    
    for (char c : englishName) {
        if (c == '_' || isupper(c)) {
            if (!current.empty()) {
                auto partIt = translations_.find(current);
                if (partIt != translations_.end()) {
                    if (!result.empty()) result += "_";
                    result += partIt->second;
                } else {
                    if (!result.empty()) result += "_";
                    result += current;
                }
                current.clear();
            }
            if (isupper(c)) {
                current += tolower(c);
            }
        } else {
            current += c;
        }
    }
    
    // (AR) الجزء الأخير
    // (EN) Last part
    if (!current.empty()) {
        auto partIt = translations_.find(current);
        if (partIt != translations_.end()) {
            if (!result.empty()) result += "_";
            result += partIt->second;
        } else {
            if (!result.empty()) result += "_";
            result += current;
        }
    }
    
    return result.empty() ? englishName : result;
}

void ArabicFunctionTranslator::addTranslation(const std::string& english, const std::string& arabic) {
    translations_[english] = arabic;
}

bool ArabicFunctionTranslator::loadFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // (AR) تخطي التعليقات والأسطر الفارغة
        // (EN) Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;
        
        auto pos = line.find('=');
        if (pos != std::string::npos) {
            std::string english = line.substr(0, pos);
            std::string arabic = line.substr(pos + 1);
            translations_[english] = arabic;
        }
    }
    
    return true;
}

void ArabicFunctionTranslator::initializeDefaults() {
    // (AR) أفعال شائعة
    // (EN) Common verbs
    translations_["alloc"] = u8"احجز";
    translations_["allocate"] = u8"احجز";
    translations_["malloc"] = u8"احجز_ذاكرة";
    translations_["calloc"] = u8"احجز_صفري";
    translations_["realloc"] = u8"أعد_الحجز";
    translations_["free"] = u8"حرر";
    translations_["release"] = u8"حرر";
    translations_["delete"] = u8"احذف";
    
    translations_["open"] = u8"افتح";
    translations_["close"] = u8"أغلق";
    translations_["read"] = u8"اقرأ";
    translations_["write"] = u8"اكتب";
    translations_["seek"] = u8"انتقل";
    translations_["tell"] = u8"أخبر";
    translations_["flush"] = u8"امسح";
    
    translations_["create"] = u8"أنشئ";
    translations_["destroy"] = u8"دمر";
    translations_["init"] = u8"هيئ";
    translations_["cleanup"] = u8"نظف";
    
    translations_["get"] = u8"أحضر";
    translations_["set"] = u8"عيّن";
    translations_["add"] = u8"أضف";
    translations_["remove"] = u8"أزل";
    translations_["find"] = u8"ابحث";
    translations_["search"] = u8"ابحث";
    
    translations_["copy"] = u8"انسخ";
    translations_["move"] = u8"انقل";
    translations_["compare"] = u8"قارن";
    translations_["sort"] = u8"رتب";
    
    translations_["print"] = u8"اطبع";
    translations_["printf"] = u8"اطبع_منسق";
    translations_["puts"] = u8"اطبع_سطر";
    translations_["scanf"] = u8"اقرأ_منسق";
    translations_["gets"] = u8"اقرأ_سطر";
    
    translations_["strlen"] = u8"طول_نص";
    translations_["strcpy"] = u8"انسخ_نص";
    translations_["strcat"] = u8"ادمج_نص";
    translations_["strcmp"] = u8"قارن_نص";
    translations_["strdup"] = u8"استنسخ_نص";
    
    translations_["memset"] = u8"املأ_ذاكرة";
    translations_["memcpy"] = u8"انسخ_ذاكرة";
    translations_["memmove"] = u8"انقل_ذاكرة";
    translations_["memcmp"] = u8"قارن_ذاكرة";
    
    // (AR) أسماء
    // (EN) Nouns
    translations_["file"] = u8"ملف";
    translations_["string"] = u8"نص";
    translations_["buffer"] = u8"ذاكرة_مؤقتة";
    translations_["size"] = u8"حجم";
    translations_["length"] = u8"طول";
    translations_["count"] = u8"عدد";
    translations_["pointer"] = u8"مؤشر";
    translations_["ptr"] = u8"مؤشر";
    translations_["data"] = u8"بيانات";
    translations_["result"] = u8"نتيجة";
    translations_["error"] = u8"خطأ";
    translations_["value"] = u8"قيمة";
    translations_["array"] = u8"مصفوفة";
    translations_["list"] = u8"قائمة";
    translations_["stream"] = u8"تدفق";
}

// ============================================================================
//                    (AR) تنفيذ FunctionBehaviorAnalyzer
//                    (EN) FunctionBehaviorAnalyzer Implementation
// ============================================================================

FunctionBehaviorAnalyzer::AnalysisResult FunctionBehaviorAnalyzer::analyze(const FunctionInfo& info) {
    AnalysisResult result;
    
    // (AR) التحقق من دوال الحجز المعروفة
    // (EN) Check for known allocation functions
    if (isAllocator(info)) {
        result.resourceType = ResourceType::MEMORY;
        result.suggestedDeallocator = suggestDeallocator(info.name);
        result.canFail = true;
        result.possibleErrors = {"NULL", "ENOMEM"};
        result.confidence = "high";
    }
    // (AR) التحقق من دوال الملفات
    // (EN) Check for file functions
    else if (isFileOpener(info)) {
        result.resourceType = ResourceType::FILE_HANDLE;
        result.suggestedDeallocator = "fclose";
        result.canFail = true;
        result.possibleErrors = {"NULL", "ENOENT", "EACCES"};
        result.confidence = "high";
    }
    // (AR) تحليل النوع المُرجع
    // (EN) Analyze return type
    else if (info.returnType.find('*') != std::string::npos) {
        result.suggestedNullBehavior = NullBehavior::CHECK_RESULT;
        result.canFail = true;
        result.confidence = "medium";
    }
    
    return result;
}

bool FunctionBehaviorAnalyzer::isAllocator(const FunctionInfo& info) {
    static const std::set<std::string> allocators = {
        "malloc", "calloc", "realloc", "aligned_alloc",
        "strdup", "strndup", "wcsdup",
        "reallocarray"
    };
    
    if (allocators.count(info.name)) {
        return true;
    }
    
    // (AR) البحث عن أنماط الحجز
    // (EN) Look for allocation patterns
    if (info.name.find("alloc") != std::string::npos ||
        info.name.find("create") != std::string::npos ||
        info.name.find("new") != std::string::npos) {
        
        if (info.returnType.find('*') != std::string::npos) {
            return true;
        }
    }
    
    return false;
}

bool FunctionBehaviorAnalyzer::isFileOpener(const FunctionInfo& info) {
    static const std::set<std::string> fileOpeners = {
        "fopen", "freopen", "fdopen", "popen",
        "tmpfile", "fmemopen", "open_memstream"
    };
    
    return fileOpeners.count(info.name) > 0;
}

std::string FunctionBehaviorAnalyzer::suggestDeallocator(const std::string& funcName) {
    static const std::map<std::string, std::string> deallocators = {
        {"malloc", "free"},
        {"calloc", "free"},
        {"realloc", "free"},
        {"aligned_alloc", "free"},
        {"strdup", "free"},
        {"strndup", "free"},
        {"wcsdup", "free"},
        {"fopen", "fclose"},
        {"popen", "pclose"},
    };
    
    auto it = deallocators.find(funcName);
    if (it != deallocators.end()) {
        return it->second;
    }
    
    // (AR) تخمين بناءً على الاسم
    // (EN) Guess based on name
    if (funcName.find("alloc") != std::string::npos ||
        funcName.find("create") != std::string::npos ||
        funcName.find("new") != std::string::npos) {
        return "free";
    }
    
    if (funcName.find("open") != std::string::npos) {
        return "close";
    }
    
    return "";
}

// ============================================================================
//                    (AR) تنفيذ WrapperEmitter
//                    (EN) WrapperEmitter Implementation
// ============================================================================

void WrapperEmitter::startFile(const std::string& filename) {
    clear();
    addComment(u8"ملف: " + filename, true);
    addComment(u8"تم توليده تلقائياً - لا تعدل يدوياً", true);
    addLine("");
}

void WrapperEmitter::addHeader(const std::string& title, const std::string& description) {
    code_ += "/**\n";
    code_ += " * @file " + title + "\n";
    code_ += " * @brief " + description + "\n";
    code_ += " */\n\n";
}

void WrapperEmitter::addImport(const std::string& module) {
    code_ += std::string(u8"استورد \"") + module + "\"\n";
}

void WrapperEmitter::startStruct(const std::string& name, const std::string& arabicName) {
    code_ += indent() + ArabicKeywords::STRUCT + " " + (arabicName.empty() ? name : arabicName) + " {\n";
    indentLevel_++;
}

void WrapperEmitter::endStruct() {
    indentLevel_--;
    code_ += indent() + "}\n";
}

void WrapperEmitter::startFunction(
    const std::string& name,
    const std::string& returnType,
    const std::vector<std::pair<std::string, std::string>>& params
) {
    code_ += indent() + ArabicKeywords::FUNCTION + " " + name + "(";
    
    bool first = true;
    for (const auto& [paramName, paramType] : params) {
        if (!first) code_ += ", ";
        first = false;
        code_ += paramName + ": " + paramType;
    }
    
    code_ += ") -> " + returnType + "\n";
    indentLevel_++;
}

void WrapperEmitter::endFunction() {
    indentLevel_--;
    code_ += indent() + ArabicKeywords::END + "\n";
}

void WrapperEmitter::addLine(const std::string& codeLine) {
    if (!codeLine.empty()) {
        code_ += indent() + codeLine + "\n";
    } else {
        code_ += "\n";
    }
}

void WrapperEmitter::addComment(const std::string& comment, bool arabic) {
    code_ += indent() + "// " + comment + "\n";
}

std::string WrapperEmitter::getCode() const {
    return code_;
}

void WrapperEmitter::clear() {
    code_.clear();
    indentLevel_ = 0;
}

std::string WrapperEmitter::indent() const {
    return std::string(indentLevel_ * 4, ' ');
}

} // namespace bindgen
} // namespace tools
} // namespace sad
