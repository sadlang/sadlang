// ════════════════════════════════════════════════════════════════════════════════
// ملف: constraint.h
// File: constraint.h
//
// الوصف: تعريف قيود الأنواع (Type Constraints) لاستنتاج الأنواع
// Description: Type constraint definitions for type inference
//
// المؤلف: Sad Language Type System - Type Inference Engine
// Author: Sad Language Type System - Type Inference Engine
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// الغرض: قيود الأنواع تُستخدم في خوارزمية Hindley-Milner
// Purpose: Type constraints are used in Hindley-Milner algorithm
//
// مثال / Example:
//   دالة جمع(أ، ب) { إرجاع أ + ب }
//   Constraints:
//     T_a = T_b         (same type)
//     T_result = T_a    (return type equals parameter type)
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_CONSTRAINT_H
#define SAD_COMPILER_TYPE_SYSTEM_CONSTRAINT_H

#include "type.h"              // استيراد النوع الأساسي / Import base Type (type.h:1-218)
#include <string>              // لـ الرسائل / For messages
#include <vector>              // لـ القوائم / For lists
#include <memory>              // لـ shared_ptr / For shared_ptr

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تعداد أنواع القيود / Constraint Kind Enumeration
// ════════════════════════════════════════════════════════════════════════════════

enum class ConstraintKind {
    // قيد المساواة: T1 = T2
    // Equality constraint: T1 = T2
    Equality,
    
    // قيد النوع الفرعي: T1 <: T2 (T1 is subtype of T2)
    // Subtype constraint: T1 <: T2
    Subtype,
    
    // قيد العضو: T has member M
    // Member constraint: T has member M
    HasMember,
    
    // قيد الاستدعاء: T is callable with args A and returns R
    // Callable constraint: T(A) -> R
    Callable,
    
    // قيد التكرار: T is iterable with element type E
    // Iterable constraint: T is iterable<E>
    Iterable
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: Constraint
// Class: Constraint
//
// الوصف: قيد نوع يجب حله أثناء Type Inference
// Description: Type constraint to be solved during type inference
//
// الاستخدام / Usage:
//   1. توليد القيود من الكود / Generate constraints from code
//   2. حل القيود بواسطة ConstraintSolver / Solve constraints using ConstraintSolver
//   3. تطبيق الاستبدالات / Apply substitutions
// ════════════════════════════════════════════════════════════════════════════════

class Constraint {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ لقيد المساواة: type1 = type2
    // Constructor for equality constraint: type1 = type2
    // المعامل / Parameter: kind - نوع القيد / Constraint kind
    // المعامل / Parameter: type1 - النوع الأول / First type (from type.h:214)
    // المعامل / Parameter: type2 - النوع الثاني / Second type
    // المعامل / Parameter: location - موقع الكود / Code location (optional)
    Constraint(ConstraintKind kind, 
               TypePtr type1, 
               TypePtr type2,
               const std::string& location = "");
    
    // منشئ نسخ / Copy constructor
    Constraint(const Constraint& other);
    
    // منشئ نقل / Move constructor
    Constraint(Constraint&& other) noexcept;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // معاملات التعيين / Assignment Operators
    // ═══════════════════════════════════════════════════════════════════════════
    
    // معامل تعيين النسخ / Copy assignment
    Constraint& operator=(const Constraint& other);
    
    // معامل تعيين النقل / Move assignment
    Constraint& operator=(Constraint&& other) noexcept;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال العامة / Public Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على نوع القيد / Get constraint kind
    // الإرجاع / Return: نوع القيد (Equality, Subtype, ...)
    ConstraintKind getKind() const { return kind_; }
    
    // الحصول على النوع الأول / Get first type
    // الإرجاع / Return: النوع الأول (type1_)
    TypePtr getType1() const { return type1_; }
    
    // الحصول على النوع الثاني / Get second type
    // الإرجاع / Return: النوع الثاني (type2_)
    TypePtr getType2() const { return type2_; }
    
    // الحصول على الموقع / Get location
    // الإرجاع / Return: موقع الكود حيث نشأ القيد
    const std::string& getLocation() const { return location_; }
    
    // تعيين موقع / Set location
    // المعامل / Parameter: location - الموقع الجديد / New location
    void setLocation(const std::string& location) { location_ = location; }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // فحص النوع / Type Checking
    // ═══════════════════════════════════════════════════════════════════════════
    
    // هل هذا قيد مساواة؟ / Is this an equality constraint?
    bool isEquality() const { return kind_ == ConstraintKind::Equality; }
    
    // هل هذا قيد نوع فرعي؟ / Is this a subtype constraint?
    bool isSubtype() const { return kind_ == ConstraintKind::Subtype; }
    
    // هل هذا قيد عضو؟ / Is this a member constraint?
    bool isHasMember() const { return kind_ == ConstraintKind::HasMember; }
    
    // هل هذا قيد استدعاء؟ / Is this a callable constraint?
    bool isCallable() const { return kind_ == ConstraintKind::Callable; }
    
    // هل هذا قيد تكرار؟ / Is this an iterable constraint?
    bool isIterable() const { return kind_ == ConstraintKind::Iterable; }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التحويل إلى نص / String Conversion
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تحويل القيد إلى نص تمثيلي / Convert constraint to string
    // مثال / Example: "T1 = Integer" أو "T2 <: T3"
    std::string toString() const;
    
    // تحويل نوع القيد إلى نص / Convert constraint kind to string
    // المعامل / Parameter: kind - نوع القيد / Constraint kind
    // الإرجاع / Return: تمثيل نصي ("=", "<:", ...)
    static std::string kindToString(ConstraintKind kind);
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // نوع القيد / Constraint kind
    ConstraintKind kind_;
    
    // النوع الأول في القيد / First type in constraint
    TypePtr type1_;
    
    // النوع الثاني في القيد / Second type in constraint
    TypePtr type2_;
    
    // موقع الكود حيث نشأ القيد / Code location where constraint originated
    // مفيد لرسائل الأخطاء / Useful for error messages
    std::string location_;
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: ConstraintSet
// Class: ConstraintSet
//
// الوصف: مجموعة من القيود / Set of constraints
// يُستخدم لتجميع القيود وإدارتها / Used to collect and manage constraints
// ════════════════════════════════════════════════════════════════════════════════

class ConstraintSet {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ افتراضي - مجموعة فارغة / Default constructor - empty set
    ConstraintSet();
    
    // منشئ مع قائمة قيود / Constructor with constraint list
    explicit ConstraintSet(const std::vector<Constraint>& constraints);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إدارة القيود / Constraint Management
    // ═══════════════════════════════════════════════════════════════════════════
    
    // إضافة قيد / Add constraint
    // المعامل / Parameter: constraint - القيد المُراد إضافته / Constraint to add
    void add(const Constraint& constraint);
    
    // إضافة قيد مساواة / Add equality constraint
    // المعامل / Parameter: type1 - النوع الأول / First type
    // المعامل / Parameter: type2 - النوع الثاني / Second type
    // المعامل / Parameter: location - الموقع (اختياري) / Location (optional)
    void addEquality(TypePtr type1, TypePtr type2, const std::string& location = "");
    
    // إضافة قيد نوع فرعي / Add subtype constraint
    // المعامل / Parameter: subtype - النوع الفرعي / Subtype
    // المعامل / Parameter: supertype - النوع الأساسي / Supertype
    // المعامل / Parameter: location - الموقع (اختياري) / Location (optional)
    void addSubtype(TypePtr subtype, TypePtr supertype, const std::string& location = "");
    
    // دمج مجموعة قيود أخرى / Merge another constraint set
    // المعامل / Parameter: other - المجموعة الأخرى / Other set
    void merge(const ConstraintSet& other);
    
    // مسح جميع القيود / Clear all constraints
    void clear();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الاستعلام / Query
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على عدد القيود / Get number of constraints
    size_t size() const { return constraints_.size(); }
    
    // التحقق من كون المجموعة فارغة / Check if set is empty
    bool isEmpty() const { return constraints_.empty(); }
    
    // الحصول على جميع القيود / Get all constraints
    const std::vector<Constraint>& getConstraints() const { return constraints_; }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التحويل إلى نص / String Conversion
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تحويل المجموعة إلى نص تمثيلي / Convert set to string
    // مثال / Example: "{T1 = Integer, T2 <: T3, T4 = String}"
    std::string toString() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التكرار / Iteration
    // ═══════════════════════════════════════════════════════════════════════════
    
    // البداية / Begin iterator
    std::vector<Constraint>::iterator begin() { return constraints_.begin(); }
    std::vector<Constraint>::const_iterator begin() const { return constraints_.begin(); }
    
    // النهاية / End iterator
    std::vector<Constraint>::iterator end() { return constraints_.end(); }
    std::vector<Constraint>::const_iterator end() const { return constraints_.end(); }
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // قائمة القيود / List of constraints
    std::vector<Constraint> constraints_;
};

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء قيد مساواة / Create equality constraint
// المعامل / Parameter: type1 - النوع الأول / First type
// المعامل / Parameter: type2 - النوع الثاني / Second type
// المعامل / Parameter: location - الموقع / Location
// الإرجاع / Return: قيد مساواة / Equality constraint
Constraint createEqualityConstraint(TypePtr type1, TypePtr type2, 
                                   const std::string& location = "");

// إنشاء قيد نوع فرعي / Create subtype constraint
// المعامل / Parameter: subtype - النوع الفرعي / Subtype
// المعامل / Parameter: supertype - النوع الأساسي / Supertype
// المعامل / Parameter: location - الموقع / Location
// الإرجاع / Return: قيد نوع فرعي / Subtype constraint
Constraint createSubtypeConstraint(TypePtr subtype, TypePtr supertype,
                                  const std::string& location = "");

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_CONSTRAINT_H
