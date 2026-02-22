// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file constraint_errors.hpp
 * @brief رسائل أخطاء القيود بالعربية
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *              رسائل أخطاء القيود في لغة ص (Constraint Error Messages)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على رسائل الأخطاء العربية المتعلقة بالقيود والسمات.
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_SHARED_ERRORS_CONSTRAINT_ERRORS_HPP
#define SAD_SHARED_ERRORS_CONSTRAINT_ERRORS_HPP

#include <string>
#include <vector>
#include <optional>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════════
//                    رموز أخطاء القيود / Constraint Error Codes
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @enum ConstraintErrorCode
 * @brief رموز أخطاء القيود
 */
enum class ConstraintErrorCode {
    // أخطاء السمات (ص2001-ص2020)
    TRAIT_NOT_IMPLEMENTED       = 2001,  // السمة غير منفذة
    TRAIT_NOT_FOUND            = 2002,  // السمة غير موجودة
    TRAIT_ALREADY_IMPLEMENTED  = 2003,  // السمة منفذة مسبقاً
    TRAIT_METHOD_MISSING       = 2004,  // دالة السمة مفقودة
    TRAIT_METHOD_WRONG_SIGNATURE = 2005, // توقيع دالة السمة خاطئ
    TRAIT_BOUND_NOT_SATISFIED  = 2006,  // قيد السمة غير محقق
    TRAIT_CIRCULAR_DEPENDENCY  = 2007,  // تبعية دائرية في السمات
    TRAIT_CONFLICTING_IMPL     = 2008,  // تنفيذ متعارض للسمة
    TRAIT_ORPHAN_IMPL          = 2009,  // تنفيذ يتيم (خارج النطاق)
    TRAIT_OBJECT_UNSAFE        = 2010,  // السمة غير آمنة للكائنات
    
    // أخطاء المعممات (ص2021-ص2040)
    GENERIC_PARAM_NOT_FOUND    = 2021,  // معامل معمم غير موجود
    GENERIC_ARG_COUNT_MISMATCH = 2022,  // عدد وسائط النوع غير متطابق
    GENERIC_BOUND_NOT_MET      = 2023,  // قيد المعامل غير محقق
    GENERIC_INFINITE_TYPE      = 2024,  // نوع لانهائي (تكراري)
    GENERIC_CONFLICTING_BOUNDS = 2025,  // قيود متعارضة
    GENERIC_CANNOT_INFER       = 2026,  // لا يمكن استنتاج النوع
    GENERIC_TOO_COMPLEX        = 2027,  // القيود معقدة جداً
    
    // أخطاء التحليل (ص2041-ص2060)
    PARSE_EXPECTED_WHERE       = 2041,  // متوقع 'حيث'
    PARSE_EXPECTED_COLON       = 2042,  // متوقع ':'
    PARSE_EXPECTED_TRAIT_NAME  = 2043,  // متوقع اسم سمة
    PARSE_EXPECTED_PARAM_NAME  = 2044,  // متوقع اسم معامل
    PARSE_INVALID_CONSTRAINT   = 2045,  // قيد غير صحيح
    PARSE_UNEXPECTED_TOKEN     = 2046,  // رمز غير متوقع
    
    // أخطاء الحل (ص2061-ص2080)
    SOLVE_UNIFICATION_FAILED   = 2061,  // فشل التوحيد
    SOLVE_NO_SOLUTION          = 2062,  // لا يوجد حل
    SOLVE_TIMEOUT              = 2063,  // انتهى الوقت
    SOLVE_AMBIGUOUS            = 2064   // حل غامض
};

// ═══════════════════════════════════════════════════════════════════════════
//                    معلومات خطأ القيود / Constraint Error Info
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct ConstraintErrorInfo
 * @brief معلومات كاملة عن خطأ القيود
 */
struct ConstraintErrorInfo {
    ConstraintErrorCode code;           // رمز الخطأ
    std::string typeName;               // اسم النوع المتأثر
    std::string traitName;              // اسم السمة (إن وجدت)
    std::string parameterName;          // اسم المعامل (إن وجد)
    std::string location;               // موقع الخطأ
    std::vector<std::string> notes;     // ملاحظات إضافية
    
    ConstraintErrorInfo() : code(ConstraintErrorCode::TRAIT_NOT_IMPLEMENTED) {}
    ConstraintErrorInfo(ConstraintErrorCode c) : code(c) {}
};

// ═══════════════════════════════════════════════════════════════════════════
//                    مُنشئ رسائل الأخطاء / Error Message Builder
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @class ConstraintErrorMessages
 * @brief منشئ رسائل أخطاء القيود بالعربية والإنجليزية
 */
class ConstraintErrorMessages {
public:
    /**
     * @brief الحصول على رسالة الخطأ
     * @param info معلومات الخطأ
     * @param arabic استخدام اللغة العربية
     */
    static std::string getMessage(const ConstraintErrorInfo& info, bool arabic = true);
    
    /**
     * @brief الحصول على رمز الخطأ بالتنسيق ص####
     */
    static std::string getErrorCode(ConstraintErrorCode code);
    
    /**
     * @brief الحصول على اقتراح للإصلاح
     */
    static std::string getSuggestion(const ConstraintErrorInfo& info, bool arabic = true);
    
    /**
     * @brief تنسيق رسالة الخطأ الكاملة
     */
    static std::string formatError(const ConstraintErrorInfo& info, bool arabic = true);
    
    /**
     * @brief الحصول على مثال للاستخدام الصحيح
     */
    static std::optional<std::string> getExample(ConstraintErrorCode code, bool arabic = true);

private:
    // رسائل الأخطاء العربية
    static std::string getArabicMessage(const ConstraintErrorInfo& info);
    
    // رسائل الأخطاء الإنجليزية
    static std::string getEnglishMessage(const ConstraintErrorInfo& info);
    
    // اقتراحات الإصلاح العربية
    static std::string getArabicSuggestion(const ConstraintErrorInfo& info);
    
    // اقتراحات الإصلاح الإنجليزية
    static std::string getEnglishSuggestion(const ConstraintErrorInfo& info);
};

// ═══════════════════════════════════════════════════════════════════════════
//                    أخطاء محددة / Specific Errors
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief إنشاء خطأ "السمة غير منفذة"
 */
inline ConstraintErrorInfo traitNotImplemented(
    const std::string& typeName,
    const std::string& traitName,
    const std::string& location = "") {
    ConstraintErrorInfo info(ConstraintErrorCode::TRAIT_NOT_IMPLEMENTED);
    info.typeName = typeName;
    info.traitName = traitName;
    info.location = location;
    return info;
}

/**
 * @brief إنشاء خطأ "السمة غير موجودة"
 */
inline ConstraintErrorInfo traitNotFound(
    const std::string& traitName,
    const std::string& location = "") {
    ConstraintErrorInfo info(ConstraintErrorCode::TRAIT_NOT_FOUND);
    info.traitName = traitName;
    info.location = location;
    return info;
}

/**
 * @brief إنشاء خطأ "قيد السمة غير محقق"
 */
inline ConstraintErrorInfo traitBoundNotSatisfied(
    const std::string& typeName,
    const std::string& traitName,
    const std::string& parameterName,
    const std::string& location = "") {
    ConstraintErrorInfo info(ConstraintErrorCode::TRAIT_BOUND_NOT_SATISFIED);
    info.typeName = typeName;
    info.traitName = traitName;
    info.parameterName = parameterName;
    info.location = location;
    return info;
}

/**
 * @brief إنشاء خطأ "دالة السمة مفقودة"
 */
inline ConstraintErrorInfo traitMethodMissing(
    const std::string& typeName,
    const std::string& traitName,
    const std::string& methodName,
    const std::string& location = "") {
    ConstraintErrorInfo info(ConstraintErrorCode::TRAIT_METHOD_MISSING);
    info.typeName = typeName;
    info.traitName = traitName;
    info.parameterName = methodName; // reuse for method name
    info.location = location;
    return info;
}

/**
 * @brief إنشاء خطأ "لا يمكن استنتاج النوع"
 */
inline ConstraintErrorInfo cannotInferType(
    const std::string& parameterName,
    const std::string& location = "") {
    ConstraintErrorInfo info(ConstraintErrorCode::GENERIC_CANNOT_INFER);
    info.parameterName = parameterName;
    info.location = location;
    return info;
}

/**
 * @brief إنشاء خطأ "عدد وسائط النوع غير متطابق"
 */
inline ConstraintErrorInfo genericArgCountMismatch(
    const std::string& typeName,
    size_t expected,
    size_t found,
    const std::string& location = "") {
    ConstraintErrorInfo info(ConstraintErrorCode::GENERIC_ARG_COUNT_MISMATCH);
    info.typeName = typeName;
    info.notes.push_back("متوقع: " + std::to_string(expected));
    info.notes.push_back("وُجد: " + std::to_string(found));
    info.location = location;
    return info;
}

/**
 * @brief إنشاء خطأ "فشل التوحيد"
 */
inline ConstraintErrorInfo unificationFailed(
    const std::string& type1,
    const std::string& type2,
    const std::string& location = "") {
    ConstraintErrorInfo info(ConstraintErrorCode::SOLVE_UNIFICATION_FAILED);
    info.typeName = type1;
    info.traitName = type2; // reuse for second type
    info.location = location;
    return info;
}

} // namespace Errors
} // namespace Sad

#endif // SAD_SHARED_ERRORS_CONSTRAINT_ERRORS_HPP
