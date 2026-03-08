// ════════════════════════════════════════════════════════════════════════════════
// ملف: unification.cpp
// File: unification.cpp
//
// الوصف: تنفيذ محرك التوحيد (Unification Engine) - Robinson's Algorithm
// Description: Implementation of unification engine - Robinson's Algorithm
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// المرجع: Robinson's Unification Algorithm (1965)
// Reference: J.A. Robinson, "A Machine-Oriented Logic Based on the Resolution Principle"
// ════════════════════════════════════════════════════════════════════════════════

#include "unification.h"      // المصدر: unification.h:1-240 / Source: unification.h:1-240
#include "type_variable.h"    // المصدر: type_variable.h:1-186 / Source: type_variable.h:1-186
#include "primitive_type.h"   // المصدر: primitive_type.h:1-100 / Source: primitive_type.h:1-100
#include "types/composite_type_classes.h"  // ArrayType, TupleType, FunctionType
#include <sstream>            // لـ ostringstream / For ostringstream
#include <algorithm>          // لـ std::find / For std::find
#include <set>                // لـ std::set / For std::set

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ UnificationError
// UnificationError Implementation
// ════════════════════════════════════════════════════════════════════════════════

// المنشئ الكامل / Full constructor
// المصدر: unification.h:37-42 / Source: unification.h:37-42
UnificationError::UnificationError(UnificationErrorKind kind,
                                 TypePtr type1,
                                 TypePtr type2,
                                 const std::string& message)
    : kind_(kind),           // تخزين نوع الخطأ / Store error kind
      type1_(type1),         // تخزين النوع الأول / Store first type
      type2_(type2),         // تخزين النوع الثاني / Store second type
      message_(message) {}   // تخزين رسالة الخطأ / Store error message

// المنشئ الافتراضي / Default constructor
// المصدر: unification.h:44 / Source: unification.h:44
UnificationError::UnificationError()
    : kind_(UnificationErrorKind::None),  // بدون خطأ / No error
      type1_(nullptr),                     // لا يوجد نوع أول / No first type
      type2_(nullptr),                     // لا يوجد نوع ثاني / No second type
      message_("") {}                      // رسالة فارغة / Empty message

// تحويل الخطأ إلى نص / Convert error to string
// المصدر: unification.h:79 / Source: unification.h:79
std::string UnificationError::toString() const {
    std::ostringstream oss;  // منشئ النص / String builder
    
    // البادئة حسب نوع الخطأ / Prefix based on error kind
    switch (kind_) {
        case UnificationErrorKind::TypeMismatch:
            oss << "خطأ عدم تطابق الأنواع / Type Mismatch Error: ";
            break;
        case UnificationErrorKind::OccursCheckFailed:
            oss << "خطأ فحص التواجد / Occurs Check Error: ";
            break;
        case UnificationErrorKind::ArityMismatch:
            oss << "خطأ عدم تطابق عدد المعاملات / Arity Mismatch Error: ";
            break;
        case UnificationErrorKind::None:
            oss << "لا يوجد خطأ / No Error: ";
            break;
        default:
            oss << "خطأ غير معروف / Unknown Error: ";
            break;
    }
    
    oss << message_;  // إضافة رسالة الخطأ / Add error message
    
    // إضافة تفاصيل الأنواع إذا كانت موجودة / Add type details if available
    if (type1_ || type2_) {
        oss << " [";
        if (type1_) {
            oss << type1_->toString();  // المصدر: type.h:104 / Source: type.h:104
        } else {
            oss << "null";
        }
        oss << " != ";
        if (type2_) {
            oss << type2_->toString();  // المصدر: type.h:104 / Source: type.h:104
        } else {
            oss << "null";
        }
        oss << "]";
    }
    
    return oss.str();  // إرجاع النص المُشكَّل / Return formatted string
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ UnificationResult
// UnificationResult Implementation
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء نتيجة ناجحة / Create success result
// المصدر: unification.h:109 / Source: unification.h:109
UnificationResult UnificationResult::makeSuccess(const Substitution& subst) {
    UnificationResult result;    // إنشاء نتيجة جديدة / Create new result
    result.success = true;       // تعيين النجاح / Set success
    result.substitution = subst; // نسخ الاستبدال / Copy substitution
    return result;               // إرجاع النتيجة / Return result
}

// إنشاء نتيجة فاشلة / Create failure result
// المصدر: unification.h:112 / Source: unification.h:112
UnificationResult UnificationResult::makeFailure(const UnificationError& err) {
    UnificationResult result;  // إنشاء نتيجة جديدة / Create new result
    result.success = false;    // تعيين الفشل / Set failure
    result.error = err;        // نسخ الخطأ / Copy error
    return result;             // إرجاع النتيجة / Return result
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ Unifier - المحرك الرئيسي
// Unifier Implementation - Main Engine
// ════════════════════════════════════════════════════════════════════════════════

// المنشئ / Constructor
// المصدر: unification.h:135 / Source: unification.h:135
Unifier::Unifier() 
    : occursCheckEnabled_(true),  // تفعيل فحص التواجد / Enable occurs check
      unificationCount_(0) {}     // عداد التوحيدات = 0 / Unification counter = 0

// توحيد نوعين بسيط / Simple unify two types
// المصدر: unification.h:148 / Source: unification.h:148
UnificationResult Unifier::unify(TypePtr type1, TypePtr type2) {
    Substitution emptySubst;  // استبدال فارغ / Empty substitution
    return unify(type1, type2, emptySubst);  // استدعاء النسخة الكاملة / Call full version
}

// توحيد نوعين مع استبدال مبدئي / Unify two types with initial substitution
// المصدر: unification.h:153 / Source: unification.h:153
UnificationResult Unifier::unify(TypePtr type1, TypePtr type2, const Substitution& subst) {
    Substitution workingSubst = subst;  // نسخ الاستبدال للعمل / Copy substitution for work
    return unifyInternal(type1, type2, workingSubst);  // استدعاء المحرك الداخلي / Call internal engine
}

// توحيد مجموعة من أزواج الأنواع / Unify a list of type pairs
// المصدر: unification.h:158 / Source: unification.h:158
UnificationResult Unifier::unifyAll(const std::vector<std::pair<TypePtr, TypePtr>>& pairs) {
    Substitution currentSubst;  // الاستبدال الحالي / Current substitution
    
    // المرور على كل زوج / Iterate over each pair
    for (const auto& [type1, type2] : pairs) {
        // توحيد الزوج الحالي / Unify current pair
        UnificationResult result = unify(type1, type2, currentSubst);
        
        // إذا فشل التوحيد، أوقف وأرجع الفشل / If unification fails, stop and return failure
        if (!result.success) {
            return result;
        }
        
        // دمج الاستبدال الجديد / Merge new substitution
        currentSubst = result.substitution;
    }
    
    // نجحت جميع التوحيدات / All unifications succeeded
    return UnificationResult::makeSuccess(currentSubst);
}

// المحرك الداخلي للتوحيد - Robinson's Algorithm
// Internal unification engine - Robinson's Algorithm
// المصدر: unification.h:181 / Source: unification.h:181
UnificationResult Unifier::unifyInternal(TypePtr type1, TypePtr type2, Substitution& subst) {
    unificationCount_++;  // زيادة عداد التوحيدات / Increment unification counter
    
    // === الخطوة 1: التحقق من صحة المدخلات / Step 1: Validate inputs ===
    if (!type1 || !type2) {
        // أحد الأنواع null / One of the types is null
        return UnificationResult::makeFailure(
            createMismatchError(type1, type2)
        );
    }
    
    // === الخطوة 2: تطبيق الاستبدال الحالي / Step 2: Apply current substitution ===
    // المصدر: substitution.h:81 / Source: substitution.h:81
    type1 = subst.apply(type1);  // تطبيق على النوع الأول / Apply to first type
    type2 = subst.apply(type2);  // تطبيق على النوع الثاني / Apply to second type
    
    // === الخطوة 3: التحقق من التطابق المباشر / Step 3: Check direct equality ===
    // المصدر: type.h:107 / Source: type.h:107
    if (type1 == type2) {
        // نفس المؤشر / Same pointer
        return UnificationResult::makeSuccess(subst);
    }
    
    if (type1->equals(type2.get())) {
        // نفس القيمة / Same value
        return UnificationResult::makeSuccess(subst);
    }
    
    // === الخطوة 4: معالجة متغيرات الأنواع / Step 4: Handle type variables ===
    // المصدر: type_variable.h:156 / Source: type_variable.h:156
    if (isTypeVariable(type1)) {
        // type1 هو متغير نوع / type1 is a type variable
        TypeVariable* var = asTypeVariable(type1);  // المصدر: type_variable.h:174 / Source: type_variable.h:174
        if (!var) {
            return UnificationResult::makeFailure(
                UnificationError(UnificationErrorKind::TypeMismatch, type1, type2,
                               "فشل تحويل متغير النوع / Failed to cast type variable")
            );
        }
        return unifyVariable(var->getName(), type2, subst);  // المصدر: type_variable.h:70 / Source: type_variable.h:70
    }
    
    // المصدر: type_variable.h:156 / Source: type_variable.h:156
    if (isTypeVariable(type2)) {
        // type2 هو متغير نوع / type2 is a type variable
        TypeVariable* var = asTypeVariable(type2);  // المصدر: type_variable.h:174 / Source: type_variable.h:174
        if (!var) {
            return UnificationResult::makeFailure(
                UnificationError(UnificationErrorKind::TypeMismatch, type1, type2,
                               "فشل تحويل متغير النوع / Failed to cast type variable")
            );
        }
        return unifyVariable(var->getName(), type1, subst);  // المصدر: type_variable.h:70 / Source: type_variable.h:70
    }
    
    // === الخطوة 5: معالجة الأنواع البدائية / Step 5: Handle primitive types ===
    // المصدر: type.h:147 / Source: type.h:147
    if (type1->isPrimitive() && type2->isPrimitive()) {
        // كلاهما نوع بدائي / Both are primitive types
        // المصدر: type.h:115 / Source: type.h:115
        if (type1->getKind() == type2->getKind()) {
            // نفس النوع البدائي / Same primitive type
            return UnificationResult::makeSuccess(subst);
        } else {
            // أنواع بدائية مختلفة / Different primitive types
            return UnificationResult::makeFailure(
                createMismatchError(type1, type2)
            );
        }
    }
    
    // === الخطوة 6: معالجة الأنواع الخاصة / Step 6: Handle special types ===
    // المصدر: type.h:140-143 / Source: type.h:140-143
    if (type1->isAny() || type2->isAny()) {
        // Any يتوحد مع أي نوع / Any unifies with any type
        return UnificationResult::makeSuccess(subst);
    }
    
    // المصدر: type.h:144 / Source: type.h:144
    if (type1->isUnknown() || type2->isUnknown()) {
        // Unknown يتوحد مع أي نوع / Unknown unifies with any type
        return UnificationResult::makeSuccess(subst);
    }
    
    // === الخطوة 7: معالجة الأنواع المركبة / Step 7: Handle composite types ===
    // المصدر: type.h:151-156 / Source: type.h:151-156
    if (type1->isComposite() && type2->isComposite()) {
        // كلاهما نوع مركب / Both are composite types
        if (type1->getKind() != type2->getKind()) {
            // أنواع مركبة مختلفة / Different composite types
            return UnificationResult::makeFailure(
                createMismatchError(type1, type2)
            );
        }
        
        // (AR) تحليل عناصر الأنواع المركبة / (EN) Analyze composite type elements
        if (type1->getKind() == TypeKind::Array) {
            auto* arr1 = static_cast<ArrayType*>(type1.get());
            auto* arr2 = static_cast<ArrayType*>(type2.get());
            TypePtr elem1 = arr1->getElementType();
            TypePtr elem2 = arr2->getElementType();
            if (elem1 && elem2) {
                auto elemResult = unifyInternal(elem1, elem2, subst);
                if (!elemResult.success) {
                    return elemResult;
                }
            }
        } else if (type1->getKind() == TypeKind::Tuple) {
            auto* tup1 = static_cast<TupleType*>(type1.get());
            auto* tup2 = static_cast<TupleType*>(type2.get());
            if (tup1->getArity() != tup2->getArity()) {
                return UnificationResult::makeFailure(
                    createMismatchError(type1, type2)
                );
            }
            for (size_t i = 0; i < tup1->getArity(); ++i) {
                TypePtr e1 = tup1->getElementAt(i);
                TypePtr e2 = tup2->getElementAt(i);
                if (e1 && e2) {
                    auto elemResult = unifyInternal(e1, e2, subst);
                    if (!elemResult.success) {
                        return elemResult;
                    }
                }
            }
        }
        
        return UnificationResult::makeSuccess(subst);
    }
    
    // === الخطوة 8: معالجة أنواع الدوال / Step 8: Handle function types ===
    // المصدر: type.h:129 / Source: type.h:129
    if (type1->isFunction() && type2->isFunction()) {
        // كلاهما نوع دالة / Both are function types
        // (AR) تحليل معاملات ونوع الإرجاع / (EN) Analyze parameters and return type
        auto* fn1 = static_cast<FunctionType*>(type1.get());
        auto* fn2 = static_cast<FunctionType*>(type2.get());
        
        // (AR) تحقق من عدد المعاملات / (EN) Check parameter count
        if (fn1->getArity() != fn2->getArity()) {
            return UnificationResult::makeFailure(
                createMismatchError(type1, type2)
            );
        }
        
        // (AR) توحيد كل معامل / (EN) Unify each parameter
        for (size_t i = 0; i < fn1->getArity(); ++i) {
            TypePtr p1 = fn1->getParamAt(i);
            TypePtr p2 = fn2->getParamAt(i);
            if (p1 && p2) {
                auto paramResult = unifyInternal(p1, p2, subst);
                if (!paramResult.success) {
                    return paramResult;
                }
            }
        }
        
        // (AR) توحيد نوع الإرجاع / (EN) Unify return type
        TypePtr ret1 = fn1->getReturnType();
        TypePtr ret2 = fn2->getReturnType();
        if (ret1 && ret2) {
            auto retResult = unifyInternal(ret1, ret2, subst);
            if (!retResult.success) {
                return retResult;
            }
        }
        
        return UnificationResult::makeSuccess(subst);
    }
    
    // === الخطوة 9: فشل التوحيد / Step 9: Unification failed ===
    return UnificationResult::makeFailure(
        createMismatchError(type1, type2)
    );
}

// توحيد متغير نوع مع نوع / Unify type variable with type
// المصدر: unification.h:191 / Source: unification.h:191
UnificationResult Unifier::unifyVariable(const std::string& varName, 
                                        TypePtr type, 
                                        Substitution& subst) {
    // === الخطوة 1: البحث عن استبدال موجود / Step 1: Look for existing substitution ===
    // المصدر: substitution.h:66 / Source: substitution.h:66
    TypePtr existing = subst.lookup(varName);
    
    if (existing) {
        // يوجد استبدال لهذا المتغير / Substitution exists for this variable
        // نوحد الاستبدال الموجود مع النوع الجديد / Unify existing substitution with new type
        return unifyInternal(existing, type, subst);
    }
    
    // === الخطوة 2: فحص التواجد (Occurs Check) / Step 2: Occurs Check ===
    // منع الأنواع اللانهائية مثل T = List<T>
    // Prevent infinite types like T = List<T>
    if (occursCheckEnabled_ && occursCheck(varName, type)) {
        return UnificationResult::makeFailure(
            createOccursError(varName, type)
        );
    }
    
    // === الخطوة 3: إضافة الاستبدال / Step 3: Add substitution ===
    // المصدر: substitution.h:58 / Source: substitution.h:58
    subst.add(varName, type);
    
    return UnificationResult::makeSuccess(subst);
}

// فحص تواجد متغير في نوع (Occurs Check)
// Check if variable occurs in type (Occurs Check)
// المصدر: unification.h:201 / Source: unification.h:201
bool Unifier::occursCheck(const std::string& varName, TypePtr type) const {
    if (!type) {
        return false;  // نوع null / null type
    }
    
    // المصدر: type_variable.h:156 / Source: type_variable.h:156
    if (isTypeVariable(type)) {
        // النوع هو متغير / Type is a variable
        TypeVariable* var = asTypeVariable(type);  // المصدر: type_variable.h:174 / Source: type_variable.h:174
        if (!var) {
            return false;
        }
        
        // المصدر: type_variable.h:70 / Source: type_variable.h:70
        const std::string& currentVarName = var->getName();
        
        // إذا كان نفس المتغير / If same variable
        if (currentVarName == varName) {
            return true;  // تم العثور على تواجد / Occurs found
        }
        
        return false;  // متغير مختلف / Different variable
    }
    
    // (AR) فحص الأنواع المركبة والدوال / (EN) Check composite types and functions
    if (type->isComposite()) {
        if (type->getKind() == TypeKind::Array) {
            auto* arr = static_cast<ArrayType*>(type.get());
            TypePtr elem = arr->getElementType();
            if (elem && occursCheck(varName, elem)) {
                return true;
            }
        } else if (type->getKind() == TypeKind::Tuple) {
            auto* tup = static_cast<TupleType*>(type.get());
            for (size_t i = 0; i < tup->getArity(); ++i) {
                TypePtr e = tup->getElementAt(i);
                if (e && occursCheck(varName, e)) {
                    return true;
                }
            }
        }
    }
    
    if (type->isFunction()) {
        auto* fn = static_cast<FunctionType*>(type.get());
        // (AR) فحص المعاملات / (EN) Check parameters
        for (size_t i = 0; i < fn->getArity(); ++i) {
            TypePtr p = fn->getParamAt(i);
            if (p && occursCheck(varName, p)) {
                return true;
            }
        }
        // (AR) فحص نوع الإرجاع / (EN) Check return type
        TypePtr ret = fn->getReturnType();
        if (ret && occursCheck(varName, ret)) {
            return true;
        }
    }
    
    return false;  // لم يتم العثور على تواجد / No occurrence found
}

// إنشاء خطأ عدم تطابق / Create mismatch error
// المصدر: unification.h:206 / Source: unification.h:206
UnificationError Unifier::createMismatchError(TypePtr type1, TypePtr type2) const {
    std::ostringstream oss;  // منشئ النص / String builder
    
    oss << "لا يمكن توحيد الأنواع / Cannot unify types: ";
    
    if (type1) {
        // المصدر: type.h:104 / Source: type.h:104
        oss << "'" << type1->toString() << "'";
    } else {
        oss << "'null'";
    }
    
    oss << " مع / with ";
    
    if (type2) {
        // المصدر: type.h:104 / Source: type.h:104
        oss << "'" << type2->toString() << "'";
    } else {
        oss << "'null'";
    }
    
    return UnificationError(
        UnificationErrorKind::TypeMismatch,  // نوع الخطأ / Error kind
        type1,                               // النوع الأول / First type
        type2,                               // النوع الثاني / Second type
        oss.str()                            // الرسالة / Message
    );
}

// إنشاء خطأ فحص التواجد / Create occurs check error
// المصدر: unification.h:211 / Source: unification.h:211
UnificationError Unifier::createOccursError(const std::string& varName, TypePtr type) const {
    std::ostringstream oss;  // منشئ النص / String builder
    
    oss << "فشل فحص التواجد / Occurs check failed: ";
    oss << "المتغير / Variable '" << varName << "' ";
    oss << "يحدث في / occurs in ";
    
    if (type) {
        // المصدر: type.h:104 / Source: type.h:104
        oss << "'" << type->toString() << "'";
    } else {
        oss << "'null'";
    }
    
    oss << " (هذا سيُنشئ نوعاً لانهائياً / This would create an infinite type)";
    
    return UnificationError(
        UnificationErrorKind::OccursCheckFailed,  // نوع الخطأ / Error kind
        nullptr,                                   // لا يوجد نوع أول / No first type
        type,                                      // النوع الثاني / Second type
        oss.str()                                  // الرسالة / Message
    );
}



// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة عامة / Global Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

// توحيد نوعين بسيط / Simple unify two types
// المصدر: unification.h:220 / Source: unification.h:220
UnificationResult unify(TypePtr type1, TypePtr type2) {
    Unifier unifier;  // إنشاء محرك توحيد / Create unifier
    return unifier.unify(type1, type2);  // توحيد الأنواع / Unify types
}

// توحيد نوعين مع استبدال / Unify two types with substitution
// المصدر: unification.h:225 / Source: unification.h:225
UnificationResult unify(TypePtr type1, TypePtr type2, const Substitution& subst) {
    Unifier unifier;  // إنشاء محرك توحيد / Create unifier
    return unifier.unify(type1, type2, subst);  // توحيد مع استبدال / Unify with substitution
}

// توحيد قائمة من أزواج الأنواع / Unify list of type pairs
// المصدر: unification.h:230 / Source: unification.h:230
UnificationResult unifyAll(const std::vector<std::pair<TypePtr, TypePtr>>& pairs) {
    Unifier unifier;  // إنشاء محرك توحيد / Create unifier
    return unifier.unifyAll(pairs);  // توحيد جميع الأزواج / Unify all pairs
}

} // namespace TypeSystem
} // namespace Sad
