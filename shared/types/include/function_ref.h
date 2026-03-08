/**
 * @file function_ref.h
 * @brief (AR) بنية مرجع الدالة — تمثيل خفيف الوزن لمرجع دالة في وقت التشغيل
 * @brief (EN) Function reference struct — lightweight runtime function reference
 * 
 * (AR) تُستخدم هذه البنية لتمثيل الدوال كقيم من الدرجة الأولى في نظام Value.
 *      بدلاً من تخزين اسم الدالة كنص (STRING)، نخزنها في بنية مُصنَّفة تحمل
 *      معلومات إضافية عن نوع الدالة وعدد معاملاتها.
 *
 * (EN) This struct is used to represent functions as first-class values in the Value system.
 *      Instead of storing the function name as a string (STRING), we store it in a typed
 *      struct that carries additional information about the function type and arity.
 * 
 * @note (AR) هذه البنية مستقلة عن FunctionDefinition لتجنب التبعيات الدائرية
 * @note (EN) This struct is independent from FunctionDefinition to avoid circular dependencies
 */

#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>

namespace Sad {
namespace Data {

/**
 * @brief (AR) نوع مرجع الدالة — يحدد كيف تم تعريف الدالة
 * @brief (EN) Function reference kind — defines how the function was defined
 */
enum class FunctionRefKind {
    USER_DEFINED,    ///< (AR) دالة معرفة من المستخدم بكلمة "دالة" / (EN) User-defined function via "دالة" keyword
    BUILT_IN,        ///< (AR) دالة مدمجة (مكتوبة بـ C++) / (EN) Built-in function (written in C++)
    LAMBDA,          ///< (AR) دالة لامبدا مجهولة / (EN) Anonymous lambda function
    METHOD           ///< (AR) طريقة داخل صنف / (EN) Method inside a class
};

/**
 * @brief (AR) بنية مرجع الدالة — تمثيل القيمة في وقت التشغيل
 * @brief (EN) Function reference struct — runtime value representation
 * 
 * (AR) تحتوي على كل ما يلزم لتعريف مرجع دالة والبحث عنها لاستدعائها:
 *      - اسم العرض (اسم الدالة كما كتبها المستخدم)
 *      - اسم التسجيل (الاسم الفعلي في FunctionManager)
 *      - نوع الدالة (مستخدم/مدمجة/لامبدا/طريقة)
 *      - عدد المعاملات (arity) للتحقق السريع
 *      - أسماء المعاملات للاستبطان (reflection)
 *
 * (EN) Contains everything needed to identify a function reference and look it up for calling:
 *      - display name (function name as written by user)
 *      - registered name (actual name in FunctionManager)
 *      - function kind (user/builtin/lambda/method)
 *      - arity (parameter count) for quick checking
 *      - parameter names for reflection/introspection
 */
struct FunctionRef {
    /// (AR) اسم العرض — اسم الدالة كما كتبه المستخدم / (EN) Display name — function name as written by user
    std::string displayName;
    
    /// (AR) اسم التسجيل — الاسم الفعلي في FunctionManager (قد يختلف للامبدا) / (EN) Registered name — actual name in FunctionManager (may differ for lambdas)
    std::string registeredName;
    
    /// (AR) نوع مرجع الدالة / (EN) Function reference kind
    FunctionRefKind kind;
    
    /// (AR) عدد المعاملات (-1 = متغير العدد) / (EN) Parameter count (-1 = variadic)
    int arity;
    
    /// (AR) أسماء المعاملات (للاستبطان) / (EN) Parameter names (for introspection/reflection)
    std::vector<std::string> parameterNames;
    
    // ══════════════════════════════════════════════════════════════════
    // (AR) المنشئات / (EN) Constructors
    // ══════════════════════════════════════════════════════════════════
    
    /// (AR) منشئ افتراضي / (EN) Default constructor
    FunctionRef()
        : kind(FunctionRefKind::USER_DEFINED), arity(0) {}
    
    /// (AR) منشئ كامل / (EN) Full constructor
    FunctionRef(const std::string& dispName, 
                const std::string& regName,
                FunctionRefKind k,
                int ar = -1,
                const std::vector<std::string>& params = {})
        : displayName(dispName)
        , registeredName(regName)
        , kind(k)
        , arity(ar)
        , parameterNames(params) {}
    
    /// (AR) منشئ مختصر — عندما يتساوى اسم العرض واسم التسجيل / (EN) Short constructor — when display name equals registered name
    FunctionRef(const std::string& name, FunctionRefKind k, int ar = -1)
        : displayName(name)
        , registeredName(name)
        , kind(k)
        , arity(ar) {}
    
    // ══════════════════════════════════════════════════════════════════
    // (AR) دوال المساعدة / (EN) Helper functions
    // ══════════════════════════════════════════════════════════════════
    
    /// (AR) هل هذه لامبدا؟ / (EN) Is this a lambda?
    bool isLambda() const { return kind == FunctionRefKind::LAMBDA; }
    
    /// (AR) هل هذه دالة مدمجة؟ / (EN) Is this a built-in function?
    bool isBuiltIn() const { return kind == FunctionRefKind::BUILT_IN; }
    
    /// (AR) هل هذه دالة معرفة من المستخدم؟ / (EN) Is this a user-defined function?
    bool isUserDefined() const { return kind == FunctionRefKind::USER_DEFINED; }
    
    /// (AR) هل هذه طريقة (method)؟ / (EN) Is this a method?
    bool isMethod() const { return kind == FunctionRefKind::METHOD; }
    
    /// (AR) الحصول على اسم النوع بالعربية / (EN) Get kind name in Arabic
    std::string getKindNameAr() const {
        switch (kind) {
            case FunctionRefKind::USER_DEFINED: return "دالة_مستخدم";
            case FunctionRefKind::BUILT_IN:     return "دالة_مدمجة";
            case FunctionRefKind::LAMBDA:        return "لامبدا";
            case FunctionRefKind::METHOD:        return "طريقة";
        }
        return "مجهول";
    }
    
    /// (AR) الحصول على اسم النوع بالإنجليزية / (EN) Get kind name in English
    std::string getKindNameEn() const {
        switch (kind) {
            case FunctionRefKind::USER_DEFINED: return "user_function";
            case FunctionRefKind::BUILT_IN:     return "builtin_function";
            case FunctionRefKind::LAMBDA:        return "lambda";
            case FunctionRefKind::METHOD:        return "method";
        }
        return "unknown";
    }
    
    /// (AR) تحويل إلى نص للعرض / (EN) Convert to string for display
    std::string toString() const {
        std::string result = "<دالة:" + displayName;
        if (kind == FunctionRefKind::LAMBDA) {
            result = "<لامبدا";
            if (!displayName.empty() && displayName != registeredName) {
                result += ":" + displayName;
            }
        } else if (kind == FunctionRefKind::METHOD) {
            result = "<طريقة:" + displayName;
        } else if (kind == FunctionRefKind::BUILT_IN) {
            result = "<دالة_مدمجة:" + displayName;
        }
        
        if (arity >= 0) {
            result += "(" + std::to_string(arity) + ")";
        }
        result += ">";
        return result;
    }
    
    /// (AR) مقارنة المساواة / (EN) Equality comparison
    bool operator==(const FunctionRef& other) const {
        return registeredName == other.registeredName && kind == other.kind;
    }
    
    bool operator!=(const FunctionRef& other) const {
        return !(*this == other);
    }
};

} // namespace Data
} // namespace Sad
