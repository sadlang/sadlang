/**
 * @file ownership_annotations.cpp
 * @brief (AR) تنفيذ تعليقات الملكية لـ FFI
 *        (EN) FFI Ownership Annotations Implementation
 * 
 * @details
 * (AR) هذا الملف يحتوي على تنفيذ:
 *      ┌────────────────────────────────────────────────────────────┐
 *      │ 1. AnnotationParser - محلل التعليقات                       │
 *      │ 2. OwnershipProfileBuilder - بناء ملفات الملكية           │
 *      │ 3. OwnershipChecker - التحقق من الملكية                   │
 *      │ 4. OwnershipCodeGenerator - توليد كود التحقق              │
 *      │ 5. OwnershipReport - تقارير التحليل                       │
 *      └────────────────────────────────────────────────────────────┘
 * 
 * @author فريق تطوير لغة ص (Sad Language Development Team)
 * @date 2025-01-18
 * @version 1.0.0
 */

#include "ffi/ownership_annotations.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <iostream>

namespace sad {
namespace ffi {

// ============================================================================
//                    (AR) ثوابت داخلية
//                    (EN) Internal Constants
// ============================================================================

namespace {
    // (AR) جدول تحويل الأسماء العربية للأنواع
    // (EN) Arabic name to type conversion table
    const std::map<std::string, AnnotationType> arabicNameToTypeMap = {
        {AnnotationNames::AR_OWNS_RESULT, AnnotationType::OWNS_RESULT},
        {AnnotationNames::AR_FREED_BY, AnnotationType::FREED_BY},
        {AnnotationNames::AR_BORROWS, AnnotationType::BORROWS},
        {AnnotationNames::AR_TRANSFERS, AnnotationType::TRANSFERS},
        {AnnotationNames::AR_OPTIONAL, AnnotationType::OPTIONAL},
        {AnnotationNames::AR_OUTPUT, AnnotationType::OUTPUT},
        {AnnotationNames::AR_NOT_NULL, AnnotationType::NOT_NULL},
        {AnnotationNames::AR_LIFETIME, AnnotationType::LIFETIME},
        {AnnotationNames::AR_THREAD_SAFE, AnnotationType::THREAD_SAFE},
        {AnnotationNames::AR_NO_ESCAPE, AnnotationType::NO_ESCAPE},
        
        // (AR) الأسماء الإنجليزية أيضاً
        {AnnotationNames::EN_OWNS_RESULT, AnnotationType::OWNS_RESULT},
        {AnnotationNames::EN_FREED_BY, AnnotationType::FREED_BY},
        {AnnotationNames::EN_BORROWS, AnnotationType::BORROWS},
        {AnnotationNames::EN_TRANSFERS, AnnotationType::TRANSFERS},
        {AnnotationNames::EN_OPTIONAL, AnnotationType::OPTIONAL},
        {AnnotationNames::EN_OUTPUT, AnnotationType::OUTPUT},
        {AnnotationNames::EN_NOT_NULL, AnnotationType::NOT_NULL},
        {AnnotationNames::EN_LIFETIME, AnnotationType::LIFETIME},
        {AnnotationNames::EN_THREAD_SAFE, AnnotationType::THREAD_SAFE},
        {AnnotationNames::EN_NO_ESCAPE, AnnotationType::NO_ESCAPE},
    };
    
    // (AR) جدول تحويل النوع للاسم العربي
    // (EN) Type to Arabic name table
    const std::map<AnnotationType, std::string> typeToArabicNameMap = {
        {AnnotationType::OWNS_RESULT, AnnotationNames::AR_OWNS_RESULT},
        {AnnotationType::FREED_BY, AnnotationNames::AR_FREED_BY},
        {AnnotationType::BORROWS, AnnotationNames::AR_BORROWS},
        {AnnotationType::TRANSFERS, AnnotationNames::AR_TRANSFERS},
        {AnnotationType::OPTIONAL, AnnotationNames::AR_OPTIONAL},
        {AnnotationType::OUTPUT, AnnotationNames::AR_OUTPUT},
        {AnnotationType::NOT_NULL, AnnotationNames::AR_NOT_NULL},
        {AnnotationType::LIFETIME, AnnotationNames::AR_LIFETIME},
        {AnnotationType::THREAD_SAFE, AnnotationNames::AR_THREAD_SAFE},
        {AnnotationType::NO_ESCAPE, AnnotationNames::AR_NO_ESCAPE},
    };
}

// ============================================================================
//                    (AR) تنفيذ AnnotationParser
//                    (EN) AnnotationParser Implementation
// ============================================================================

std::optional<Annotation> AnnotationParser::parse(const std::string& text) {
    errors_.clear();
    
    // (AR) التحقق من الصيغة الأساسية: #[اسم] أو #[اسم: قيمة]
    // (EN) Check basic format: #[name] or #[name: value]
    if (text.empty() || text[0] != '#' || text.length() < 3) {
        errors_.push_back(u8"صيغة تعليق غير صالحة: " + text);
        return std::nullopt;
    }
    
    // (AR) البحث عن الأقواس
    // (EN) Find brackets
    size_t openBracket = text.find('[');
    size_t closeBracket = text.rfind(']');
    
    if (openBracket == std::string::npos || closeBracket == std::string::npos ||
        openBracket >= closeBracket) {
        errors_.push_back(u8"أقواس مفقودة في التعليق: " + text);
        return std::nullopt;
    }
    
    // (AR) استخراج المحتوى
    // (EN) Extract content
    std::string content = text.substr(openBracket + 1, closeBracket - openBracket - 1);
    
    // (AR) البحث عن الاسم والوسائط
    // (EN) Find name and arguments
    std::string name;
    std::string args;
    
    size_t colonPos = content.find(':');
    if (colonPos != std::string::npos) {
        name = content.substr(0, colonPos);
        args = content.substr(colonPos + 1);
        
        // (AR) إزالة المسافات
        // (EN) Trim whitespace
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        args.erase(0, args.find_first_not_of(" \t"));
        args.erase(args.find_last_not_of(" \t") + 1);
    } else {
        name = content;
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
    }
    
    // (AR) تحليل الاسم
    // (EN) Parse name
    auto typeOpt = parseAnnotationName(name);
    if (!typeOpt) {
        errors_.push_back(u8"تعليق غير معروف: " + name);
        return std::nullopt;
    }
    
    // (AR) إنشاء التعليق
    // (EN) Create annotation
    Annotation ann;
    ann.type = *typeOpt;
    ann.arabicName = name;
    ann.target = AnnotationTarget::FUNCTION;  // (AR) افتراضي
    
    // (AR) تحليل الوسائط
    // (EN) Parse arguments
    if (!args.empty()) {
        ann.arguments = parseArguments(args);
    }
    
    // (AR) تحديد الهدف بناءً على نوع التعليق
    // (EN) Determine target based on annotation type
    switch (ann.type) {
        case AnnotationType::OWNS_RESULT:
            ann.target = AnnotationTarget::RETURN_VALUE;
            break;
        case AnnotationType::BORROWS:
        case AnnotationType::TRANSFERS:
        case AnnotationType::OUTPUT:
            ann.target = AnnotationTarget::PARAMETER;
            // (AR) إذا كانت هناك وسائط، فهي اسم المعامل
            if (ann.arguments.count("param") || !args.empty()) {
                ann.parameterName = args.empty() ? 
                    ann.arguments["param"].asString() : args;
            }
            break;
        default:
            // (AR) نوع تعليق غير معروف — تحذير لتجنب التجاهل الصامت
            // (EN) Unknown annotation type — warn to avoid silent ignore
            ann.target = AnnotationTarget::UNKNOWN;
            std::cerr << "[sadc تحذير] نوع تعليق ملكية غير معالج: " << static_cast<int>(ann.type) << std::endl;
            break;
    }
    
    return ann;
}

std::vector<Annotation> AnnotationParser::parseAll(const std::string& text) {
    std::vector<Annotation> result;
    
    // (AR) البحث عن جميع التعليقات
    // (EN) Find all annotations
    std::regex annRegex(R"(#\[[^\]]+\])");
    std::sregex_iterator iter(text.begin(), text.end(), annRegex);
    std::sregex_iterator end;
    
    while (iter != end) {
        std::string match = iter->str();
        auto ann = parse(match);
        if (ann) {
            result.push_back(*ann);
        }
        ++iter;
    }
    
    return result;
}

bool AnnotationParser::validate(const Annotation& annotation, std::vector<std::string>& errors) {
    bool valid = true;
    
    // (AR) التحقق من الوسائط المطلوبة
    // (EN) Check required arguments
    switch (annotation.type) {
        case AnnotationType::FREED_BY:
            if (annotation.arguments.empty()) {
                errors.push_back(u8"تعليق يُحرر_بـ يحتاج اسم دالة التحرير");
                valid = false;
            }
            break;
            
        case AnnotationType::LIFETIME:
            if (annotation.arguments.empty()) {
                errors.push_back(u8"تعليق عمر يحتاج تحديد العمر");
                valid = false;
            }
            break;
            
        case AnnotationType::BORROWS:
        case AnnotationType::TRANSFERS:
            if (annotation.target == AnnotationTarget::PARAMETER && 
                !annotation.parameterName) {
                errors.push_back(u8"تعليق يستعير/ينقل يحتاج تحديد المعامل");
                valid = false;
            }
            break;
            
        default:
            // (AR) نوع تعليق غير معالج — تحذير مع قبول مشروط
            // (EN) Unknown annotation type — warn but accept conditionally
            errors.push_back(u8"تحذير: نوع تعليق غير معروف (" + std::to_string(static_cast<int>(annotation.type)) + u8") — قد لا يعمل كما هو متوقع");
            break;
    }
    
    return valid;
}

std::optional<AnnotationType> AnnotationParser::parseAnnotationName(const std::string& name) {
    auto it = arabicNameToTypeMap.find(name);
    if (it != arabicNameToTypeMap.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::map<std::string, AnnotationValue> AnnotationParser::parseArguments(const std::string& args) {
    std::map<std::string, AnnotationValue> result;
    
    // (AR) تحليل بسيط: إما قيمة واحدة أو مفتاح=قيمة
    // (EN) Simple parsing: either single value or key=value
    size_t eqPos = args.find('=');
    if (eqPos != std::string::npos) {
        std::string key = args.substr(0, eqPos);
        std::string value = args.substr(eqPos + 1);
        
        // (AR) إزالة المسافات
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        AnnotationValue av;
        av.value = value;
        result[key] = av;
    } else {
        // (AR) قيمة واحدة بدون مفتاح
        // (EN) Single value without key
        AnnotationValue av;
        av.value = args;
        result["value"] = av;
    }
    
    return result;
}

AnnotationType AnnotationParser::arabicNameToType(const std::string& name) {
    auto it = arabicNameToTypeMap.find(name);
    if (it != arabicNameToTypeMap.end()) {
        return it->second;
    }
    return AnnotationType::OWNS_RESULT;  // (AR) افتراضي
}

// ============================================================================
//                    (AR) تنفيذ OwnershipProfileBuilder
//                    (EN) OwnershipProfileBuilder Implementation
// ============================================================================

FunctionOwnershipProfile OwnershipProfileBuilder::build(
    const std::string& functionName,
    const std::vector<Annotation>& annotations,
    const std::vector<std::string>& parameterNames
) {
    FunctionOwnershipProfile profile;
    profile.functionName = functionName;
    profile.allAnnotations = annotations;
    
    // (AR) تهيئة معلومات ملكية المعاملات
    // (EN) Initialize parameter ownership info
    for (const auto& paramName : parameterNames) {
        profile.parameterOwnership[paramName] = OwnershipInfo{};
    }
    
    // (AR) تطبيق كل تعليق
    // (EN) Apply each annotation
    for (const auto& ann : annotations) {
        applyAnnotation(profile, ann);
    }
    
    // (AR) استنتاج الملكية غير المُحددة
    // (EN) Infer unspecified ownership
    inferOwnership(profile);
    
    // (AR) التحقق من الصحة
    // (EN) Validate
    std::vector<std::string> errors;
    profile.isValid = validateConsistency(profile, errors);
    profile.errors = errors;
    
    return profile;
}

void OwnershipProfileBuilder::applyAnnotation(
    FunctionOwnershipProfile& profile,
    const Annotation& annotation
) {
    switch (annotation.type) {
        case AnnotationType::OWNS_RESULT:
            profile.returnOwnership.kind = OwnershipKind::OWNED;
            break;
            
        case AnnotationType::FREED_BY:
            if (!annotation.arguments.empty()) {
                auto it = annotation.arguments.find("value");
                if (it != annotation.arguments.end() && it->second.isString()) {
                    profile.returnOwnership.deallocator = it->second.asString();
                }
            }
            break;
            
        case AnnotationType::BORROWS:
            if (annotation.parameterName) {
                const std::string& paramName = *annotation.parameterName;
                if (profile.parameterOwnership.count(paramName)) {
                    profile.parameterOwnership[paramName].kind = OwnershipKind::BORROWED;
                }
            }
            break;
            
        case AnnotationType::TRANSFERS:
            if (annotation.parameterName) {
                const std::string& paramName = *annotation.parameterName;
                if (profile.parameterOwnership.count(paramName)) {
                    profile.parameterOwnership[paramName].kind = OwnershipKind::TRANSFERRED;
                }
            }
            break;
            
        case AnnotationType::OPTIONAL:
            if (annotation.target == AnnotationTarget::RETURN_VALUE) {
                profile.returnOwnership.isOptional = true;
            } else if (annotation.parameterName) {
                const std::string& paramName = *annotation.parameterName;
                if (profile.parameterOwnership.count(paramName)) {
                    profile.parameterOwnership[paramName].isOptional = true;
                }
            }
            break;
            
        case AnnotationType::OUTPUT:
            if (annotation.parameterName) {
                const std::string& paramName = *annotation.parameterName;
                if (profile.parameterOwnership.count(paramName)) {
                    profile.parameterOwnership[paramName].isOutput = true;
                }
            }
            break;
            
        case AnnotationType::THREAD_SAFE:
            if (annotation.target == AnnotationTarget::RETURN_VALUE) {
                profile.returnOwnership.isThreadSafe = true;
            }
            break;
            
        case AnnotationType::NO_ESCAPE:
            if (annotation.parameterName) {
                const std::string& paramName = *annotation.parameterName;
                if (profile.parameterOwnership.count(paramName)) {
                    profile.parameterOwnership[paramName].noEscape = true;
                }
            }
            break;
            
        default:
            // (AR) نوع تعليق ملكية غير معالج — تحذير لتجنب فقدان إعدادات الملكية
            // (EN) Unhandled annotation type — warn to avoid silent ownership loss
            std::cerr << "[sadc تحذير] تعليق ملكية غير معالج عند تطبيق الملف الشخصي: "
                      << static_cast<int>(annotation.type) << std::endl;
            break;
    }
}

void OwnershipProfileBuilder::inferOwnership(FunctionOwnershipProfile& profile) {
    // (AR) إذا كانت النتيجة مملوكة ولم تُحدد دالة التحرير، تحذير
    // (EN) If result is owned but no deallocator specified, warn
    if (profile.returnOwnership.kind == OwnershipKind::OWNED &&
        !profile.returnOwnership.deallocator) {
        profile.warnings.push_back(
            u8"الدالة " + profile.functionName + 
            u8" تُرجع ملكية لكن لم تُحدد دالة التحرير"
        );
    }
    
    // (AR) المعاملات غير المُحددة تُعتبر مستعارة
    // (EN) Unspecified parameters are considered borrowed
    for (auto& [name, info] : profile.parameterOwnership) {
        if (info.kind == OwnershipKind::UNKNOWN) {
            info.kind = OwnershipKind::BORROWED;  // (AR) افتراضي آمن
        }
    }
}

bool OwnershipProfileBuilder::validateConsistency(
    const FunctionOwnershipProfile& profile,
    std::vector<std::string>& errors
) {
    bool valid = true;
    
    // (AR) لا يمكن أن يكون المعامل مستعار ومنقول في نفس الوقت
    // (EN) Parameter cannot be both borrowed and transferred
    for (const auto& [name, info] : profile.parameterOwnership) {
        // (AR) التحقق من التناقضات
        // ... يمكن إضافة المزيد من التحققات
    }
    
    // (AR) إذا كانت النتيجة مملوكة، يجب تحديد طريقة التحرير
    // (EN) If result is owned, must specify deallocation method
    if (profile.returnOwnership.kind == OwnershipKind::OWNED &&
        !profile.returnOwnership.deallocator) {
        // (AR) هذا تحذير وليس خطأ
        // (EN) This is a warning, not an error
    }
    
    return valid;
}

// ============================================================================
//                    (AR) تنفيذ OwnershipChecker
//                    (EN) OwnershipChecker Implementation
// ============================================================================

// (AR) جدول تطابق الحجز والتحرير
// (EN) Allocator/deallocator match table
const std::map<std::string, std::string> OwnershipChecker::allocDeallocPairs_ = {
    {"malloc", "free"},
    {"calloc", "free"},
    {"realloc", "free"},
    {"strdup", "free"},
    {"strndup", "free"},
    {"aligned_alloc", "free"},
    {"fopen", "fclose"},
    {"fdopen", "fclose"},
    {"popen", "pclose"},
    {"opendir", "closedir"},
    {"socket", "close"},
    {"accept", "close"},
};

OwnershipChecker::CheckResult OwnershipChecker::checkCall(
    const FunctionOwnershipProfile& calleeProfile,
    const std::vector<OwnershipInfo>& argumentOwnership
) {
    CheckResult result;
    
    // (AR) التحقق من تطابق عدد الوسائط
    // (EN) Check argument count matches
    if (argumentOwnership.size() != calleeProfile.parameterOwnership.size()) {
        result.isValid = false;
        result.errors.push_back(u8"عدد الوسائط لا يطابق عدد المعاملات");
        return result;
    }
    
    // (AR) التحقق من كل وسيطة
    // (EN) Check each argument
    size_t i = 0;
    for (const auto& [paramName, paramOwnership] : calleeProfile.parameterOwnership) {
        const OwnershipInfo& argOwnership = argumentOwnership[i];
        
        // (AR) إذا كان المعامل ينقل الملكية، يجب أن يكون الوسيط مملوك
        // (EN) If parameter transfers ownership, argument must be owned
        if (paramOwnership.kind == OwnershipKind::TRANSFERRED) {
            if (argOwnership.kind != OwnershipKind::OWNED) {
                result.errors.push_back(
                    u8"المعامل " + paramName + 
                    u8" يتطلب نقل ملكية لكن الوسيط ليس مملوكاً"
                );
                result.isValid = false;
            }
        }
        
        // (AR) إذا كان المعامل ليس اختياري، يجب ألا يكون null
        // (EN) If parameter is not optional, must not be null
        if (!paramOwnership.isOptional && argOwnership.isOptional) {
            result.warnings.push_back(
                u8"المعامل " + paramName + 
                u8" لا يقبل null لكن الوسيط قد يكون null"
            );
        }
        
        i++;
    }
    
    return result;
}

OwnershipChecker::CheckResult OwnershipChecker::checkResult(
    const OwnershipInfo& resultOwnership,
    bool isStored,
    bool hasCleanup
) {
    CheckResult result;
    
    // (AR) إذا كانت النتيجة مملوكة ولم تُخزن ولا يوجد تنظيف
    // (EN) If result is owned and not stored and no cleanup
    if (resultOwnership.kind == OwnershipKind::OWNED) {
        if (!isStored && !hasCleanup) {
            result.errors.push_back(u8"تسريب محتمل: النتيجة المملوكة لم تُخزن ولم تُحرر");
            result.isValid = false;
        }
    }
    
    return result;
}

bool OwnershipChecker::checkDeallocatorMatch(
    const std::string& allocator,
    const std::string& deallocator
) {
    auto it = allocDeallocPairs_.find(allocator);
    if (it != allocDeallocPairs_.end()) {
        return it->second == deallocator;
    }
    
    // (AR) غير معروف - نفترض صحيح
    // (EN) Unknown - assume correct
    return true;
}

// ============================================================================
//                    (AR) تنفيذ OwnershipCodeGenerator
//                    (EN) OwnershipCodeGenerator Implementation
// ============================================================================

std::string OwnershipCodeGenerator::generateParamCheck(
    const std::string& paramName,
    const OwnershipInfo& ownership
) {
    std::ostringstream code;
    
    if (!ownership.isOptional) {
        code << "    " << u8"إذا " << paramName << " == " << u8"فارغ {\n";
        code << "        " << u8"إرجاع خطأ(\"المعامل " << paramName << u8" فارغ\")\n";
        code << "    }\n";
    }
    
    return code.str();
}

std::string OwnershipCodeGenerator::generateResultCheck(const OwnershipInfo& ownership) {
    std::ostringstream code;
    
    if (!ownership.isOptional) {
        code << "    " << u8"إذا _result == فارغ {\n";
        code << "        " << u8"إرجاع خطأ(\"النتيجة فارغة\")\n";
        code << "    }\n";
    }
    
    return code.str();
}

std::string OwnershipCodeGenerator::generateCleanup(
    const std::string& varName,
    const OwnershipInfo& ownership
) {
    std::ostringstream code;
    
    if (ownership.kind == OwnershipKind::OWNED && ownership.deallocator) {
        code << "    " << *ownership.deallocator << "(" << varName << ")\n";
    }
    
    return code.str();
}

std::string OwnershipCodeGenerator::generateRaiiWrapper(
    const std::string& typeName,
    const OwnershipInfo& ownership
) {
    std::ostringstream code;
    
    if (ownership.kind == OwnershipKind::OWNED && ownership.deallocator) {
        code << u8"بنية غلاف_" << typeName << " {\n";
        code << u8"    _ptr: *" << typeName << "\n";
        code << "\n";
        code << u8"    دالة حرر(ذاتي)\n";
        code << "        " << u8"إذا ذاتي._ptr != فارغ {\n";
        code << "            " << *ownership.deallocator << "(ذاتي._ptr)\n";
        code << "            ذاتي._ptr = " << u8"فارغ\n";
        code << "        }\n";
        code << u8"    نهاية\n";
        code << "}\n";
    }
    
    return code.str();
}

// ============================================================================
//                    (AR) تنفيذ OwnershipReport
//                    (EN) OwnershipReport Implementation
// ============================================================================

void OwnershipReport::addFunction(const FunctionOwnershipProfile& profile) {
    functions_.push_back(profile);
}

void OwnershipReport::addWarning(const std::string& warning, const std::string& location) {
    warnings_.emplace_back(warning, location);
}

void OwnershipReport::addError(const std::string& error, const std::string& location) {
    errors_.emplace_back(error, location);
}

std::string OwnershipReport::generateTextReport() const {
    std::ostringstream report;
    
    report << u8"═══════════════════════════════════════════════════════════\n";
    report << u8"           تقرير تحليل الملكية - لغة ص                      \n";
    report << u8"═══════════════════════════════════════════════════════════\n\n";
    
    // (AR) الإحصائيات
    // (EN) Statistics
    auto stats = getStatistics();
    report << u8"## الإحصائيات\n\n";
    report << u8"- إجمالي الدوال: " << stats.totalFunctions << "\n";
    report << u8"- دوال مع ملكية: " << stats.functionsWithOwnership << "\n";
    report << u8"- أخطاء: " << stats.errors << "\n";
    report << u8"- تحذيرات: " << stats.warnings << "\n\n";
    
    // (AR) الأخطاء
    // (EN) Errors
    if (!errors_.empty()) {
        report << u8"## الأخطاء\n\n";
        for (const auto& [error, location] : errors_) {
            report << u8"❌ " << location << ": " << error << "\n";
        }
        report << "\n";
    }
    
    // (AR) التحذيرات
    // (EN) Warnings
    if (!warnings_.empty()) {
        report << u8"## التحذيرات\n\n";
        for (const auto& [warning, location] : warnings_) {
            report << u8"⚠️ " << location << ": " << warning << "\n";
        }
        report << "\n";
    }
    
    // (AR) تفاصيل الدوال
    // (EN) Function details
    report << u8"## تفاصيل الدوال\n\n";
    for (const auto& func : functions_) {
        report << u8"### " << func.functionName << "\n\n";
        
        // (AR) ملكية النتيجة
        // (EN) Return ownership
        report << u8"- النتيجة: ";
        switch (func.returnOwnership.kind) {
            case OwnershipKind::OWNED: 
                report << u8"مملوكة";
                if (func.returnOwnership.deallocator) {
                    report << u8" (تُحرر بـ " << *func.returnOwnership.deallocator << ")";
                }
                break;
            case OwnershipKind::BORROWED: report << u8"مستعارة"; break;
            default: report << u8"غير محددة"; break;
        }
        report << "\n";
        
        // (AR) المعاملات
        // (EN) Parameters
        for (const auto& [name, info] : func.parameterOwnership) {
            report << u8"- " << name << ": ";
            switch (info.kind) {
                case OwnershipKind::BORROWED: report << u8"مستعار"; break;
                case OwnershipKind::TRANSFERRED: report << u8"منقول"; break;
                case OwnershipKind::OWNED: report << u8"مملوك"; break;
                default: report << u8"غير محدد"; break;
            }
            if (info.isOptional) report << u8" (اختياري)";
            if (info.isOutput) report << u8" (خرج)";
            report << "\n";
        }
        report << "\n";
    }
    
    return report.str();
}

std::string OwnershipReport::generateJsonReport() const {
    std::ostringstream json;
    
    json << "{\n";
    json << "  \"statistics\": {\n";
    auto stats = getStatistics();
    json << "    \"totalFunctions\": " << stats.totalFunctions << ",\n";
    json << "    \"functionsWithOwnership\": " << stats.functionsWithOwnership << ",\n";
    json << "    \"errors\": " << stats.errors << ",\n";
    json << "    \"warnings\": " << stats.warnings << "\n";
    json << "  },\n";
    
    // (AR) الدوال
    json << "  \"functions\": [\n";
    bool first = true;
    for (const auto& func : functions_) {
        if (!first) json << ",\n";
        first = false;
        
        json << "    {\n";
        json << "      \"name\": \"" << func.functionName << "\",\n";
        json << "      \"valid\": " << (func.isValid ? "true" : "false") << "\n";
        json << "    }";
    }
    json << "\n  ]\n";
    
    json << "}\n";
    
    return json.str();
}

OwnershipReport::Statistics OwnershipReport::getStatistics() const {
    Statistics stats;
    stats.totalFunctions = functions_.size();
    stats.errors = errors_.size();
    stats.warnings = warnings_.size();
    
    for (const auto& func : functions_) {
        if (func.returnOwnership.kind != OwnershipKind::UNKNOWN ||
            !func.parameterOwnership.empty()) {
            stats.functionsWithOwnership++;
        }
        
        // (AR) عد أنواع الملكية
        // (EN) Count ownership kinds
        stats.ownershipKindCounts[func.returnOwnership.kind]++;
        for (const auto& [_, info] : func.parameterOwnership) {
            stats.ownershipKindCounts[info.kind]++;
        }
    }
    
    return stats;
}

} // namespace ffi
} // namespace sad
