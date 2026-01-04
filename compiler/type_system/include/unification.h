// ════════════════════════════════════════════════════════════════════════════════
// ملف: unification.h
// File: unification.h
//
// الوصف: محرك التوحيد (Unification Engine) لحل معادلات الأنواع
// Description: Unification engine for solving type equations
//
// المؤلف: Sad Language Type System - Type Inference Engine
// Author: Sad Language Type System - Type Inference Engine
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// الخوارزمية: Robinson's Unification Algorithm + Occurs Check
// Algorithm: Robinson's Unification Algorithm + Occurs Check
//
// الغرض: حل معادلات مثل T1 = Integer, T2 = T1 -> T3, ...
// Purpose: Solve equations like T1 = Integer, T2 = T1 -> T3, ...
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_UNIFICATION_H
#define SAD_COMPILER_TYPE_SYSTEM_UNIFICATION_H

#include "type.h"              // استيراد النوع الأساسي / Import base Type (type.h:1-218)
#include "substitution.h"       // استيراد الاستبدال / Import Substitution
#include <string>              // لـ رسائل الأخطاء / For error messages
#include <optional>            // لـ النتائج الاختيارية / For optional results

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تعداد أخطاء التوحيد / Unification Error Enumeration
// ════════════════════════════════════════════════════════════════════════════════

enum class UnificationErrorKind {
    // لا يوجد خطأ / No error
    None,
    
    // عدم تطابق الأنواع / Type mismatch
    // مثال / Example: Integer != String
    TypeMismatch,
    
    // حلقة لا نهائية (Occurs Check failure)
    // Infinite loop (Occurs Check failure)
    // مثال / Example: T1 = List<T1>
    OccursCheckFailed,
    
    // عدد معاملات غير متطابق / Mismatched parameter count
    // مثال / Example: (T1, T2) != (T3, T4, T5)
    ArityMismatch,
    
    // نوع غير قابل للتوحيد / Type cannot be unified
    // أنواع مركبة غير متوافقة / Incompatible composite types
    CannotUnify
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: UnificationError
// Class: UnificationError
//
// الوصف: خطأ حدث أثناء التوحيد / Error that occurred during unification
// يحتوي على معلومات تفصيلية للتشخيص / Contains detailed information for diagnosis
// ════════════════════════════════════════════════════════════════════════════════

class UnificationError {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ كامل / Full constructor
    // المعامل / Parameter: kind - نوع الخطأ / Error kind
    // المعامل / Parameter: type1 - النوع الأول / First type
    // المعامل / Parameter: type2 - النوع الثاني / Second type
    // المعامل / Parameter: message - رسالة الخطأ / Error message
    UnificationError(UnificationErrorKind kind,
                    TypePtr type1,
                    TypePtr type2,
                    const std::string& message);
    
    // منشئ افتراضي - لا يوجد خطأ / Default constructor - no error
    UnificationError();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال العامة / Public Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على نوع الخطأ / Get error kind
    UnificationErrorKind getKind() const { return kind_; }
    
    // الحصول على النوع الأول / Get first type
    TypePtr getType1() const { return type1_; }
    
    // الحصول على النوع الثاني / Get second type
    TypePtr getType2() const { return type2_; }
    
    // الحصول على رسالة الخطأ / Get error message
    const std::string& getMessage() const { return message_; }
    
    // هل يوجد خطأ؟ / Is there an error?
    bool hasError() const { return kind_ != UnificationErrorKind::None; }
    
    // تحويل إلى نص / Convert to string
    std::string toString() const;
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    UnificationErrorKind kind_;  // نوع الخطأ / Error kind
    TypePtr type1_;              // النوع الأول / First type
    TypePtr type2_;              // النوع الثاني / Second type
    std::string message_;        // رسالة الخطأ / Error message
};

// ════════════════════════════════════════════════════════════════════════════════
// نوع النتيجة / Result Type
// ════════════════════════════════════════════════════════════════════════════════

// نتيجة التوحيد: إما Substitution أو UnificationError
// Unification result: either Substitution or UnificationError
struct UnificationResult {
    // هل نجح التوحيد؟ / Did unification succeed?
    bool success;
    
    // الاستبدال الناتج (إذا نجح) / Resulting substitution (if succeeded)
    Substitution substitution;
    
    // الخطأ (إذا فشل) / Error (if failed)
    UnificationError error;
    
    // منشئ لنتيجة ناجحة / Constructor for successful result
    static UnificationResult makeSuccess(const Substitution& subst);
    
    // منشئ لنتيجة فاشلة / Constructor for failed result
    static UnificationResult makeFailure(const UnificationError& err);
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: Unifier
// Class: Unifier
//
// الوصف: محرك التوحيد الرئيسي / Main unification engine
// يُحل معادلات الأنواع بواسطة خوارزمية Robinson
// Solves type equations using Robinson's algorithm
//
// الاستخدام / Usage:
//   Unifier unifier;
//   auto result = unifier.unify(type1, type2);
//   if (result.success) {
//       // استخدم result.substitution / Use result.substitution
//   }
// ════════════════════════════════════════════════════════════════════════════════

class Unifier {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ افتراضي / Default constructor
    Unifier();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التوحيد الرئيسي / Main Unification
    // ═══════════════════════════════════════════════════════════════════════════
    
    // توحيد نوعين / Unify two types
    // المعامل / Parameter: type1 - النوع الأول / First type
    // المعامل / Parameter: type2 - النوع الثاني / Second type
    // الإرجاع / Return: نتيجة التوحيد / Unification result
    UnificationResult unify(TypePtr type1, TypePtr type2);
    
    // توحيد نوعين مع استبدال موجود / Unify two types with existing substitution
    // المعامل / Parameter: type1 - النوع الأول / First type
    // المعامل / Parameter: type2 - النوع الثاني / Second type
    // المعامل / Parameter: subst - الاستبدال الموجود / Existing substitution
    // الإرجاع / Return: نتيجة التوحيد / Unification result
    UnificationResult unify(TypePtr type1, TypePtr type2, const Substitution& subst);
    
    // توحيد قائمة من الأزواج / Unify a list of type pairs
    // المعامل / Parameter: pairs - قائمة أزواج الأنواع / List of type pairs
    // الإرجاع / Return: نتيجة التوحيد / Unification result
    UnificationResult unifyAll(const std::vector<std::pair<TypePtr, TypePtr>>& pairs);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // Occurs Check (فحص الحلقة اللانهائية)
    // Occurs Check (Check for infinite loop)
    // ═══════════════════════════════════════════════════════════════════════════
    
    // التحقق من وجود متغير نوع داخل نوع آخر / Check if type variable occurs in another type
    // مثال / Example: T1 يحدث في List<T1>? -> نعم / T1 occurs in List<T1>? -> yes
    // المعامل / Parameter: var - اسم المتغير / Variable name
    // المعامل / Parameter: type - النوع المُراد فحصه / Type to check
    // الإرجاع / Return: true إذا حدث / true if occurs
    bool occursCheck(const std::string& varName, TypePtr type) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الإعدادات / Settings
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تفعيل/تعطيل Occurs Check / Enable/disable occurs check
    // المعامل / Parameter: enable - true للتفعيل / true to enable
    void setOccursCheckEnabled(bool enable) { occursCheckEnabled_ = enable; }
    
    // هل Occurs Check مُفعّل؟ / Is occurs check enabled?
    bool isOccursCheckEnabled() const { return occursCheckEnabled_; }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الإحصائيات / Statistics
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على عدد عمليات التوحيد / Get unification count
    int getUnificationCount() const { return unificationCount_; }
    
    // إعادة تعيين العداد / Reset counter
    void resetCount() { unificationCount_ = 0; }
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال التوحيد الداخلية / Internal Unification Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // توحيد داخلي مع استبدال قابل للتعديل / Internal unify with mutable substitution
    // المعامل / Parameter: type1 - النوع الأول / First type
    // المعامل / Parameter: type2 - النوع الثاني / Second type
    // المعامل / Parameter: subst - الاستبدال (يُعدّل) / Substitution (modified)
    // الإرجاع / Return: نتيجة التوحيد / Unification result
    UnificationResult unifyInternal(TypePtr type1, TypePtr type2, Substitution& subst);
    
    // توحيد متغير نوع مع نوع / Unify type variable with type
    // المعامل / Parameter: varName - اسم المتغير / Variable name
    // المعامل / Parameter: type - النوع / Type
    // المعامل / Parameter: subst - الاستبدال / Substitution
    // الإرجاع / Return: نتيجة التوحيد / Unification result
    UnificationResult unifyVariable(const std::string& varName, 
                                   TypePtr type, 
                                   Substitution& subst);
    
    // توحيد نوعين مركبين / Unify two composite types
    // المعامل / Parameter: type1 - النوع الأول / First type
    // المعامل / Parameter: type2 - النوع الثاني / Second type
    // المعامل / Parameter: subst - الاستبدال / Substitution
    // الإرجاع / Return: نتيجة التوحيد / Unification result
    UnificationResult unifyComposite(TypePtr type1, TypePtr type2, Substitution& subst);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مساعدة / Helper Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // التحقق من تطابق نوعي الأنواع / Check if type kinds match
    // المعامل / Parameter: type1 - النوع الأول / First type
    // المعامل / Parameter: type2 - النوع الثاني / Second type
    // الإرجاع / Return: true إذا تطابقا / true if match
    bool kindsMatch(TypePtr type1, TypePtr type2) const;
    
    // إنشاء خطأ عدم تطابق / Create type mismatch error
    // المعامل / Parameter: type1 - النوع الأول / First type
    // المعامل / Parameter: type2 - النوع الثاني / Second type
    // الإرجاع / Return: خطأ توحيد / Unification error
    UnificationError createMismatchError(TypePtr type1, TypePtr type2) const;
    
    // إنشاء خطأ Occurs Check / Create occurs check error
    // المعامل / Parameter: varName - اسم المتغير / Variable name
    // المعامل / Parameter: type - النوع / Type
    // الإرجاع / Return: خطأ توحيد / Unification error
    UnificationError createOccursError(const std::string& varName, TypePtr type) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // هل Occurs Check مُفعّل؟ / Is occurs check enabled?
    bool occursCheckEnabled_;
    
    // عداد عمليات التوحيد (للتشخيص) / Unification count (for diagnostics)
    mutable int unificationCount_;
};

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة عامة / Global Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

// توحيد سريع لنوعين / Quick unify two types
// المعامل / Parameter: type1 - النوع الأول / First type
// المعامل / Parameter: type2 - النوع الثاني / Second type
// الإرجاع / Return: نتيجة التوحيد / Unification result
UnificationResult unify(TypePtr type1, TypePtr type2);

// توحيد مع استبدال موجود / Unify with existing substitution
// المعامل / Parameter: type1 - النوع الأول / First type
// المعامل / Parameter: type2 - النوع الثاني / Second type
// المعامل / Parameter: subst - الاستبدال / Substitution
// الإرجاع / Return: نتيجة التوحيد / Unification result
UnificationResult unify(TypePtr type1, TypePtr type2, const Substitution& subst);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_UNIFICATION_H
