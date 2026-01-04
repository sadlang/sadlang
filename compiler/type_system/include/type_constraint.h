// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_constraint.h
// File: type_constraint.h
//
// الوصف: تعريف قيود الأنواع (Type Constraints) للأنواع العامة
// Description: Type Constraints definition for generic types
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_TYPE_CONSTRAINT_H
#define SAD_COMPILER_TYPE_SYSTEM_TYPE_CONSTRAINT_H

#include "type.h"
#include <vector>
#include <string>
#include <memory>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// التعدادات / Enumerations
// ════════════════════════════════════════════════════════════════════════════════

/**
 * أنواع القيود / Constraint Types
 * 
 * أمثلة / Examples:
 *   Subtype: T: Animal (T يجب أن يكون نوعاً فرعياً من Animal)
 *   Protocol: T: قابل_للمقارنة (T يجب أن يُطبق protocol معين)
 *   SameType: T == U (T و U يجب أن يكونا نفس النوع)
 */
enum class ConstraintKind {
    Subtype,        // T: Animal
    Protocol,       // T: Comparable
    SameType,       // T == U
    Constructor,    // T: has constructor with params
    Method,         // T: has method
    Property,       // T: has property
    Numeric,        // T: numeric type
    Iterable,       // T: iterable
    Custom          // Custom constraint
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: TypeConstraint
// Class: TypeConstraint
//
// الوصف: يمثل قيداً على معامل نوع
// Description: Represents a constraint on a type parameter
//
// أمثلة / Examples:
//   - T: قابل_للمقارنة (T must implement Comparable protocol)
//   - T: Animal (T must be subtype of Animal)
//   - T == U (T and U must be the same type)
// ════════════════════════════════════════════════════════════════════════════════

class TypeConstraint {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * منشئ لقيد من نوع Subtype / Constructor for Subtype constraint
     * 
     * @param kind نوع القيد
     * @param constraintType النوع المقيد (مثل Animal في T: Animal)
     */
    TypeConstraint(ConstraintKind kind, TypePtr constraintType);
    
    /**
     * منشئ لقيد SameType / Constructor for SameType constraint
     * 
     * @param leftParam المعامل الأيسر
     * @param rightParam المعامل الأيمن
     */
    TypeConstraint(const std::string& leftParam, const std::string& rightParam);
    
    /**
     * منشئ لقيد Protocol / Constructor for Protocol constraint
     * 
     * @param protocolName اسم الـ protocol
     */
    explicit TypeConstraint(const std::string& protocolName);
    
    /**
     * منشئ لقيد مخصص / Constructor for custom constraint
     * 
     * @param kind نوع القيد
     * @param description وصف القيد
     */
    TypeConstraint(ConstraintKind kind, const std::string& description);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال الوصول / Accessor Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على نوع القيد / Get constraint kind
     */
    ConstraintKind getKind() const { return kind_; }
    
    /**
     * الحصول على النوع المقيد / Get constraint type
     * 
     * يُستخدم في Subtype, Protocol constraints
     */
    TypePtr getConstraintType() const { return constraintType_; }
    
    /**
     * الحصول على اسم الـ protocol / Get protocol name
     */
    const std::string& getProtocolName() const { return protocolName_; }
    
    /**
     * الحصول على المعامل الأيسر (لـ SameType) / Get left parameter (for SameType)
     */
    const std::string& getLeftParameter() const { return leftParam_; }
    
    /**
     * الحصول على المعامل الأيمن (لـ SameType) / Get right parameter (for SameType)
     */
    const std::string& getRightParameter() const { return rightParam_; }
    
    /**
     * الحصول على الوصف / Get description
     */
    const std::string& getDescription() const { return description_; }
    
    /**
     * التحقق من كون القيد من نوع معين / Check if constraint is of specific kind
     */
    bool isKind(ConstraintKind k) const { return kind_ == k; }
    
    /**
     * التحقق من كون القيد Subtype / Check if is Subtype constraint
     */
    bool isSubtypeConstraint() const { return kind_ == ConstraintKind::Subtype; }
    
    /**
     * التحقق من كون القيد Protocol / Check if is Protocol constraint
     */
    bool isProtocolConstraint() const { return kind_ == ConstraintKind::Protocol; }
    
    /**
     * التحقق من كون القيد SameType / Check if is SameType constraint
     */
    bool isSameTypeConstraint() const { return kind_ == ConstraintKind::SameType; }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال التحقق / Validation Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التحقق من تطابق نوع مع القيد / Check if type satisfies constraint
     * 
     * @param type النوع المراد التحقق منه
     * @return true إذا كان النوع يُطابق القيد
     * 
     * أمثلة / Examples:
     *   Subtype: Integer.isSatisfied(Animal) → false
     *            Dog.isSatisfied(Animal) → true
     *   
     *   Protocol: Integer.isSatisfied(Comparable) → true (if implements)
     *   
     *   SameType: requires external type map for parameter resolution
     */
    bool isSatisfied(const TypePtr& type) const;
    
    /**
     * التحقق من تطابق نوع مع القيد (مع خريطة معاملات)
     * Check if type satisfies constraint (with parameter map)
     * 
     * @param type النوع المراد التحقق منه
     * @param parameterMap خريطة معاملات الأنواع (لـ SameType)
     * @return true إذا كان النوع يُطابق القيد
     */
    bool isSatisfied(const TypePtr& type, 
                     const std::unordered_map<std::string, TypePtr>& parameterMap) const;
    
    /**
     * تحويل القيد إلى نص / Convert constraint to string
     * 
     * @return تمثيل نصي للقيد
     * 
     * أمثلة / Examples:
     *   "T: Animal"
     *   "T: قابل_للمقارنة"
     *   "T == U"
     */
    std::string toString() const;
    
    /**
     * التحقق من تساوي قيدين / Check if two constraints are equal
     * 
     * @param other القيد الآخر
     * @return true إذا كانا متساويين
     */
    bool equals(const TypeConstraint& other) const;
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    ConstraintKind kind_;           // نوع القيد / Constraint kind
    TypePtr constraintType_;        // النوع المقيد / Constraint type (for Subtype, Protocol)
    std::string protocolName_;      // اسم الـ protocol / Protocol name
    std::string leftParam_;         // المعامل الأيسر (لـ SameType)
    std::string rightParam_;        // المعامل الأيمن (لـ SameType)
    std::string description_;       // وصف القيد / Constraint description
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال الداخلية / Internal Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التحقق من Subtype constraint
     */
    bool checkSubtypeConstraint(const TypePtr& type) const;
    
    /**
     * التحقق من Protocol constraint
     */
    bool checkProtocolConstraint(const TypePtr& type) const;
    
    /**
     * التحقق من SameType constraint
     */
    bool checkSameTypeConstraint(const TypePtr& type, 
                                  const std::unordered_map<std::string, TypePtr>& parameterMap) const;
    
    /**
     * التحقق من Constructor constraint
     */
    bool checkConstructorConstraint(const TypePtr& type) const;
    
    /**
     * التحقق من Method constraint
     */
    bool checkMethodConstraint(const TypePtr& type) const;
    
    /**
     * التحقق من Property constraint
     */
    bool checkPropertyConstraint(const TypePtr& type) const;
    
    /**
     * التحقق من Numeric constraint
     */
    bool checkNumericConstraint(const TypePtr& type) const;
    
    /**
     * التحقق من Iterable constraint
     */
    bool checkIterableConstraint(const TypePtr& type) const;
};

// ════════════════════════════════════════════════════════════════════════════════
// Pointer Type للاستخدام المريح / Pointer Type for Convenience
// ════════════════════════════════════════════════════════════════════════════════

using TypeConstraintPtr = std::shared_ptr<TypeConstraint>;

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

/**
 * إنشاء قيد Subtype / Create Subtype constraint
 * 
 * @param baseType النوع الأساسي (مثل Animal في T: Animal)
 * @return القيد الجديد
 */
TypeConstraintPtr makeSubtypeConstraint(TypePtr baseType);

/**
 * إنشاء قيد Protocol / Create Protocol constraint
 * 
 * @param protocolName اسم الـ protocol
 * @return القيد الجديد
 */
TypeConstraintPtr makeProtocolConstraint(const std::string& protocolName);

/**
 * إنشاء قيد SameType / Create SameType constraint
 * 
 * @param leftParam المعامل الأيسر
 * @param rightParam المعامل الأيمن
 * @return القيد الجديد
 */
TypeConstraintPtr makeSameTypeConstraint(const std::string& leftParam, 
                                         const std::string& rightParam);

/**
 * إنشاء قيد Numeric / Create Numeric constraint
 * 
 * @return القيد الجديد
 */
TypeConstraintPtr makeNumericConstraint();

/**
 * إنشاء قيد Iterable / Create Iterable constraint
 * 
 * @return القيد الجديد
 */
TypeConstraintPtr makeIterableConstraint();

/**
 * إنشاء قيد Constructor / Create Constructor constraint
 * 
 * @param description وصف المُنشئ المطلوب
 * @return القيد الجديد
 */
TypeConstraintPtr makeConstructorConstraint(const std::string& description = "");

/**
 * إنشاء قيد Method / Create Method constraint
 * 
 * @param methodName اسم الدالة المطلوبة
 * @return القيد الجديد
 */
TypeConstraintPtr makeMethodConstraint(const std::string& methodName);

/**
 * إنشاء قيد Property / Create Property constraint
 * 
 * @param propertyName اسم الخاصية المطلوبة
 * @return القيد الجديد
 */
TypeConstraintPtr makePropertyConstraint(const std::string& propertyName);

/**
 * دمج قيود متعددة / Combine multiple constraints
 * 
 * @param constraints قائمة القيود
 * @return قائمة القيود المدمجة (بدون تكرار)
 */
std::vector<TypeConstraintPtr> combineConstraints(const std::vector<TypeConstraintPtr>& constraints);

/**
 * التحقق من تطابق نوع مع قائمة قيود / Check if type satisfies all constraints
 * 
 * @param type النوع المراد التحقق منه
 * @param constraints قائمة القيود
 * @return true إذا كان النوع يُطابق جميع القيود
 */
bool satisfiesAllConstraints(const TypePtr& type, 
                              const std::vector<TypeConstraintPtr>& constraints);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_TYPE_CONSTRAINT_H
