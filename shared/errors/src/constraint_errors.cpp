// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file constraint_errors.cpp
 * @brief تنفيذ رسائل أخطاء القيود بالعربية
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "constraint_errors.hpp"
#include <optional>
#include <sstream>
#include <iomanip>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════════
//                    تنفيذ ConstraintErrorMessages
// ═══════════════════════════════════════════════════════════════════════════

std::string ConstraintErrorMessages::getMessage(const ConstraintErrorInfo& info, bool arabic) {
    if (arabic) {
        return getArabicMessage(info);
    }
    return getEnglishMessage(info);
}

std::string ConstraintErrorMessages::getErrorCode(ConstraintErrorCode code) {
    std::ostringstream oss;
    oss << "\xD8\xB5" << std::setfill('0') << std::setw(4) << static_cast<int>(code);
    // ص + 4 أرقام
    return oss.str();
}

std::string ConstraintErrorMessages::getSuggestion(const ConstraintErrorInfo& info, bool arabic) {
    if (arabic) {
        return getArabicSuggestion(info);
    }
    return getEnglishSuggestion(info);
}

std::string ConstraintErrorMessages::formatError(const ConstraintErrorInfo& info, bool arabic) {
    std::ostringstream oss;
    
    // رمز الخطأ
    oss << (arabic ? "خطأ[" : "error[") << getErrorCode(info.code) << "]: ";
    
    // رسالة الخطأ
    oss << getMessage(info, arabic);
    
    // الموقع
    if (!info.location.empty()) {
        oss << "\n" << (arabic ? "   --> " : "   --> ") << info.location;
    }
    
    // الملاحظات
    for (const auto& note : info.notes) {
        oss << "\n" << (arabic ? "   = ملاحظة: " : "   = note: ") << note;
    }
    
    // الاقتراح
    std::string suggestion = getSuggestion(info, arabic);
    if (!suggestion.empty()) {
        oss << "\n" << (arabic ? "   = اقتراح: " : "   = help: ") << suggestion;
    }
    
    return oss.str();
}

std::optional<std::string> ConstraintErrorMessages::getExample(ConstraintErrorCode code, bool arabic) {
    switch (code) {
        case ConstraintErrorCode::TRAIT_NOT_IMPLEMENTED:
            return arabic ?
                "// لتنفيذ السمة:\n"
                "نفذ قابل_للعرض لـ نوعي {\n"
                "    دالة عرض(ذاتي) -> نص {\n"
                "        أرجع \"...\"\n"
                "    }\n"
                "}" :
                "// To implement the trait:\n"
                "impl Display for MyType {\n"
                "    fn display(&self) -> String {\n"
                "        return \"...\"\n"
                "    }\n"
                "}";
        
        case ConstraintErrorCode::TRAIT_BOUND_NOT_SATISFIED:
            return arabic ?
                "// لتحقيق قيد السمة:\n"
                "دالة ف<ت: قابل_للعرض>(س: ت) { ... }\n"
                "// أو أضف السمة للنوع:\n"
                "نفذ قابل_للعرض لـ نوعي { ... }" :
                "// To satisfy the trait bound:\n"
                "fn f<T: Display>(x: T) { ... }\n"
                "// Or implement the trait:\n"
                "impl Display for MyType { ... }";
        
        case ConstraintErrorCode::GENERIC_CANNOT_INFER:
            return arabic ?
                "// حدد النوع صراحةً:\n"
                "دع س: قائمة<رقم> = قائمة::جديد()\n"
                "// أو:\n"
                "دع س = قائمة::<رقم>::جديد()" :
                "// Specify the type explicitly:\n"
                "let x: List<i32> = List::new()\n"
                "// Or:\n"
                "let x = List::<i32>::new()";
        
        default:
            return std::nullopt;
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//                    الرسائل العربية / Arabic Messages
// ═══════════════════════════════════════════════════════════════════════════

std::string ConstraintErrorMessages::getArabicMessage(const ConstraintErrorInfo& info) {
    switch (info.code) {
        // أخطاء السمات
        case ConstraintErrorCode::TRAIT_NOT_IMPLEMENTED:
            return "النوع `" + info.typeName + "` لا ينفذ السمة `" + info.traitName + "`";
        
        case ConstraintErrorCode::TRAIT_NOT_FOUND:
            return "السمة `" + info.traitName + "` غير معرّفة";
        
        case ConstraintErrorCode::TRAIT_ALREADY_IMPLEMENTED:
            return "السمة `" + info.traitName + "` منفذة مسبقاً للنوع `" + info.typeName + "`";
        
        case ConstraintErrorCode::TRAIT_METHOD_MISSING:
            return "الدالة `" + info.parameterName + "` مفقودة في تنفيذ السمة `" + 
                   info.traitName + "` للنوع `" + info.typeName + "`";
        
        case ConstraintErrorCode::TRAIT_METHOD_WRONG_SIGNATURE:
            return "توقيع الدالة `" + info.parameterName + "` لا يطابق تعريف السمة `" + 
                   info.traitName + "`";
        
        case ConstraintErrorCode::TRAIT_BOUND_NOT_SATISFIED:
            return "النوع `" + info.typeName + "` لا يحقق قيد السمة `" + info.traitName + 
                   "` المطلوب للمعامل `" + info.parameterName + "`";
        
        case ConstraintErrorCode::TRAIT_CIRCULAR_DEPENDENCY:
            return "تبعية دائرية في تعريف السمة `" + info.traitName + "`";
        
        case ConstraintErrorCode::TRAIT_CONFLICTING_IMPL:
            return "تنفيذ متعارض للسمة `" + info.traitName + "` للنوع `" + info.typeName + "`";
        
        case ConstraintErrorCode::TRAIT_ORPHAN_IMPL:
            return "تنفيذ يتيم للسمة `" + info.traitName + "`: يجب تنفيذها في نطاق السمة أو النوع";
        
        case ConstraintErrorCode::TRAIT_OBJECT_UNSAFE:
            return "السمة `" + info.traitName + "` غير آمنة للاستخدام ككائن سمة";
        
        // أخطاء المعممات
        case ConstraintErrorCode::GENERIC_PARAM_NOT_FOUND:
            return "معامل النوع `" + info.parameterName + "` غير معرّف";
        
        case ConstraintErrorCode::GENERIC_ARG_COUNT_MISMATCH:
            return "عدد وسائط النوع غير متطابق للنوع `" + info.typeName + "`";
        
        case ConstraintErrorCode::GENERIC_BOUND_NOT_MET:
            return "قيد المعامل `" + info.parameterName + "` غير محقق";
        
        case ConstraintErrorCode::GENERIC_INFINITE_TYPE:
            return "تم اكتشاف نوع لانهائي (تكراري)";
        
        case ConstraintErrorCode::GENERIC_CONFLICTING_BOUNDS:
            return "قيود متعارضة على المعامل `" + info.parameterName + "`";
        
        case ConstraintErrorCode::GENERIC_CANNOT_INFER:
            return "لا يمكن استنتاج نوع المعامل `" + info.parameterName + "`";
        
        case ConstraintErrorCode::GENERIC_TOO_COMPLEX:
            return "نظام القيود معقد جداً، فشل حل القيود";
        
        // أخطاء التحليل
        case ConstraintErrorCode::PARSE_EXPECTED_WHERE:
            return "متوقع كلمة 'حيث' في بداية جملة القيود";
        
        case ConstraintErrorCode::PARSE_EXPECTED_COLON:
            return "متوقع ':' بعد اسم المعامل";
        
        case ConstraintErrorCode::PARSE_EXPECTED_TRAIT_NAME:
            return "متوقع اسم سمة بعد ':'";
        
        case ConstraintErrorCode::PARSE_EXPECTED_PARAM_NAME:
            return "متوقع اسم معامل في جملة 'حيث'";
        
        case ConstraintErrorCode::PARSE_INVALID_CONSTRAINT:
            return "صيغة قيد غير صحيحة";
        
        case ConstraintErrorCode::PARSE_UNEXPECTED_TOKEN:
            return "رمز غير متوقع في جملة القيود";
        
        // أخطاء الحل
        case ConstraintErrorCode::SOLVE_UNIFICATION_FAILED:
            return "فشل توحيد الأنواع: `" + info.typeName + "` مع `" + info.traitName + "`";
        
        case ConstraintErrorCode::SOLVE_NO_SOLUTION:
            return "لا يوجد حل يحقق كل القيود";
        
        case ConstraintErrorCode::SOLVE_TIMEOUT:
            return "انتهى الوقت أثناء حل القيود";
        
        case ConstraintErrorCode::SOLVE_AMBIGUOUS:
            return "حل غامض: يوجد أكثر من حل ممكن";
        
        default:
            return "خطأ غير معروف في القيود";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//                    الرسائل الإنجليزية / English Messages
// ═══════════════════════════════════════════════════════════════════════════

std::string ConstraintErrorMessages::getEnglishMessage(const ConstraintErrorInfo& info) {
    switch (info.code) {
        // Trait errors
        case ConstraintErrorCode::TRAIT_NOT_IMPLEMENTED:
            return "the type `" + info.typeName + "` does not implement the trait `" + info.traitName + "`";
        
        case ConstraintErrorCode::TRAIT_NOT_FOUND:
            return "the trait `" + info.traitName + "` is not defined";
        
        case ConstraintErrorCode::TRAIT_ALREADY_IMPLEMENTED:
            return "the trait `" + info.traitName + "` is already implemented for type `" + info.typeName + "`";
        
        case ConstraintErrorCode::TRAIT_METHOD_MISSING:
            return "method `" + info.parameterName + "` is missing in implementation of trait `" + 
                   info.traitName + "` for type `" + info.typeName + "`";
        
        case ConstraintErrorCode::TRAIT_METHOD_WRONG_SIGNATURE:
            return "signature of method `" + info.parameterName + "` does not match trait `" + 
                   info.traitName + "` definition";
        
        case ConstraintErrorCode::TRAIT_BOUND_NOT_SATISFIED:
            return "the type `" + info.typeName + "` does not satisfy the trait bound `" + info.traitName + 
                   "` required by parameter `" + info.parameterName + "`";
        
        case ConstraintErrorCode::TRAIT_CIRCULAR_DEPENDENCY:
            return "circular dependency in trait definition `" + info.traitName + "`";
        
        case ConstraintErrorCode::TRAIT_CONFLICTING_IMPL:
            return "conflicting implementation of trait `" + info.traitName + "` for type `" + info.typeName + "`";
        
        case ConstraintErrorCode::TRAIT_ORPHAN_IMPL:
            return "orphan implementation of trait `" + info.traitName + "`: must be in trait or type scope";
        
        case ConstraintErrorCode::TRAIT_OBJECT_UNSAFE:
            return "the trait `" + info.traitName + "` is not object-safe";
        
        // Generic errors
        case ConstraintErrorCode::GENERIC_PARAM_NOT_FOUND:
            return "type parameter `" + info.parameterName + "` is not defined";
        
        case ConstraintErrorCode::GENERIC_ARG_COUNT_MISMATCH:
            return "wrong number of type arguments for `" + info.typeName + "`";
        
        case ConstraintErrorCode::GENERIC_BOUND_NOT_MET:
            return "bound on type parameter `" + info.parameterName + "` is not satisfied";
        
        case ConstraintErrorCode::GENERIC_INFINITE_TYPE:
            return "infinite type detected (recursive type)";
        
        case ConstraintErrorCode::GENERIC_CONFLICTING_BOUNDS:
            return "conflicting bounds on type parameter `" + info.parameterName + "`";
        
        case ConstraintErrorCode::GENERIC_CANNOT_INFER:
            return "cannot infer type for parameter `" + info.parameterName + "`";
        
        case ConstraintErrorCode::GENERIC_TOO_COMPLEX:
            return "constraint system too complex, solver failed";
        
        // Parse errors
        case ConstraintErrorCode::PARSE_EXPECTED_WHERE:
            return "expected 'where' keyword at start of constraint clause";
        
        case ConstraintErrorCode::PARSE_EXPECTED_COLON:
            return "expected ':' after parameter name";
        
        case ConstraintErrorCode::PARSE_EXPECTED_TRAIT_NAME:
            return "expected trait name after ':'";
        
        case ConstraintErrorCode::PARSE_EXPECTED_PARAM_NAME:
            return "expected parameter name in where clause";
        
        case ConstraintErrorCode::PARSE_INVALID_CONSTRAINT:
            return "invalid constraint syntax";
        
        case ConstraintErrorCode::PARSE_UNEXPECTED_TOKEN:
            return "unexpected token in constraint clause";
        
        // Solver errors
        case ConstraintErrorCode::SOLVE_UNIFICATION_FAILED:
            return "type unification failed: `" + info.typeName + "` with `" + info.traitName + "`";
        
        case ConstraintErrorCode::SOLVE_NO_SOLUTION:
            return "no solution satisfies all constraints";
        
        case ConstraintErrorCode::SOLVE_TIMEOUT:
            return "constraint solving timed out";
        
        case ConstraintErrorCode::SOLVE_AMBIGUOUS:
            return "ambiguous solution: multiple possible solutions exist";
        
        default:
            return "unknown constraint error";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//                    الاقتراحات العربية / Arabic Suggestions
// ═══════════════════════════════════════════════════════════════════════════

std::string ConstraintErrorMessages::getArabicSuggestion(const ConstraintErrorInfo& info) {
    switch (info.code) {
        case ConstraintErrorCode::TRAIT_NOT_IMPLEMENTED:
            return "أضف تنفيذاً للسمة: نفذ " + info.traitName + " لـ " + info.typeName + " { ... }";
        
        case ConstraintErrorCode::TRAIT_NOT_FOUND:
            return "تأكد من كتابة اسم السمة بشكل صحيح، أو عرّفها إذا كانت سمة جديدة";
        
        case ConstraintErrorCode::TRAIT_METHOD_MISSING:
            return "أضف الدالة المفقودة: دالة " + info.parameterName + "(...) { ... }";
        
        case ConstraintErrorCode::TRAIT_BOUND_NOT_SATISFIED:
            return "نفذ السمة " + info.traitName + " للنوع " + info.typeName + 
                   "، أو غيّر النوع المُمرر";
        
        case ConstraintErrorCode::GENERIC_CANNOT_INFER:
            return "حدد النوع صراحةً باستخدام: <" + info.parameterName + " = نوع_محدد>";
        
        case ConstraintErrorCode::GENERIC_ARG_COUNT_MISMATCH:
            return "تحقق من عدد وسائط النوع المطلوبة";
        
        case ConstraintErrorCode::SOLVE_UNIFICATION_FAILED:
            return "تأكد من تطابق الأنواع، أو أضف تحويلاً صريحاً";
        
        default:
            return "";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//                    الاقتراحات الإنجليزية / English Suggestions
// ═══════════════════════════════════════════════════════════════════════════

std::string ConstraintErrorMessages::getEnglishSuggestion(const ConstraintErrorInfo& info) {
    switch (info.code) {
        case ConstraintErrorCode::TRAIT_NOT_IMPLEMENTED:
            return "add an implementation: impl " + info.traitName + " for " + info.typeName + " { ... }";
        
        case ConstraintErrorCode::TRAIT_NOT_FOUND:
            return "check the trait name spelling, or define it if it's a new trait";
        
        case ConstraintErrorCode::TRAIT_METHOD_MISSING:
            return "add the missing method: fn " + info.parameterName + "(...) { ... }";
        
        case ConstraintErrorCode::TRAIT_BOUND_NOT_SATISFIED:
            return "implement " + info.traitName + " for " + info.typeName + 
                   ", or change the passed type";
        
        case ConstraintErrorCode::GENERIC_CANNOT_INFER:
            return "specify the type explicitly using: <" + info.parameterName + " = specific_type>";
        
        case ConstraintErrorCode::GENERIC_ARG_COUNT_MISMATCH:
            return "check the required number of type arguments";
        
        case ConstraintErrorCode::SOLVE_UNIFICATION_FAILED:
            return "ensure type compatibility, or add an explicit conversion";
        
        default:
            return "";
    }
}

} // namespace Errors
} // namespace Sad
