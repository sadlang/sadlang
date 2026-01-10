// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_parameter.h
// File: type_parameter.h
//
// الوصف: تعريف معاملات الأنواع (Type Parameters) للأنواع العامة
// Description: Type Parameters definition for generic types
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_TYPE_PARAMETER_H
#define SAD_COMPILER_TYPE_SYSTEM_TYPE_PARAMETER_H

#include "type.h"
#include <vector>

namespace Sad {
namespace TypeSystem {

// إعلان مسبق / Forward declaration
class TypeConstraint;
using TypeConstraintPtr = std::shared_ptr<TypeConstraint>;

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: TypeParameter
// Class: TypeParameter
//
// الوصف: يمثل معامل نوع في نوع عام
// Description: Represents a type parameter in a generic type
//
// أمثلة / Examples:
//   - T في Array<T>
//   - K, V في Dictionary<K, V>
//   - T: قابل_للمقارنة في max<T: قابل_للمقارنة>(a, b)
// ════════════════════════════════════════════════════════════════════════════════

class TypeParameter : public Type {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * منشئ بسيط لمعامل نوع / Simple constructor for type parameter
     * 
     * @param name اسم المعامل (مثل "T", "K", "V")
     *             Parameter name (e.g., "T", "K", "V")
     */
    explicit TypeParameter(const std::string& name);
    
    /**
     * منشئ مع قيود / Constructor with constraints
     * 
     * @param name اسم المعامل
     * @param constraints قيود على النوع
     */
    TypeParameter(const std::string& name, const std::vector<TypeConstraintPtr>& constraints);
    
    /**
     * منشئ مع نوع علوي / Constructor with upper bound
     * 
     * @param name اسم المعامل
     * @param upperBound النوع العلوي (مثل T: Animal)
     */
    TypeParameter(const std::string& name, TypePtr upperBound);
    
    // المُنشئ الافتراضي محذوف / Default constructor deleted
    TypeParameter() = delete;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مجردة مُطبقة / Implemented Abstract Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على اسم النوع بالعربية / Get type name in Arabic
    std::string getArabicName() const override;
    
    // الحصول على اسم النوع بالإنجليزية / Get type name in English
    std::string getEnglishName() const override;
    
    // تحويل النوع إلى نص / Convert type to string
    std::string toString() const override;
    
    // التحقق من التساوي / Check equality
    bool equals(const Type* other) const override;
    
    // استنساخ النوع / Clone the type
    std::shared_ptr<Type> clone() const override;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال الخاصة بمعاملات الأنواع / Type Parameter-Specific Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على اسم المعامل / Get parameter name
     * 
     * @return اسم المعامل (مثل "T", "K")
     */
    const std::string& getName() const { return name_; }
    
    /**
     * الحصول على القيود / Get constraints
     * 
     * @return قائمة القيود على هذا المعامل
     */
    const std::vector<TypeConstraintPtr>& getConstraints() const { 
        return constraints_; 
    }
    
    /**
     * إضافة قيد / Add constraint
     * 
     * @param constraint القيد المراد إضافته
     */
    void addConstraint(const TypeConstraintPtr& constraint);
    
    /**
     * التحقق من وجود قيود / Check if has constraints
     * 
     * @return true إذا كان للمعامل قيود
     */
    bool hasConstraints() const { 
        return !constraints_.empty(); 
    }
    
    /**
     * الحصول على النوع العلوي / Get upper bound
     * 
     * @return النوع العلوي (مثل T: Animal يعني Animal هو upper bound)
     */
    TypePtr getUpperBound() const { return upperBound_; }
    
    /**
     * تعيين النوع العلوي / Set upper bound
     * 
     * @param bound النوع العلوي الجديد
     */
    void setUpperBound(TypePtr bound) { upperBound_ = bound; }
    
    /**
     * التحقق من وجود نوع علوي / Check if has upper bound
     * 
     * @return true إذا كان للمعامل نوع علوي
     */
    bool hasUpperBound() const { 
        return upperBound_ != nullptr; 
    }
    
    /**
     * الحصول على النوع السفلي / Get lower bound
     * 
     * @return النوع السفلي (نادر الاستخدام)
     */
    TypePtr getLowerBound() const { return lowerBound_; }
    
    /**
     * تعيين النوع السفلي / Set lower bound
     * 
     * @param bound النوع السفلي الجديد
     */
    void setLowerBound(TypePtr bound) { lowerBound_ = bound; }
    
    /**
     * التحقق من وجود نوع سفلي / Check if has lower bound
     * 
     * @return true إذا كان للمعامل نوع سفلي
     */
    bool hasLowerBound() const { 
        return lowerBound_ != nullptr; 
    }
    
    /**
     * التحقق من تطابق نوع مع قيود المعامل / Check if type satisfies constraints
     * 
     * @param type النوع المراد التحقق منه
     * @return true إذا كان النوع يُطابق جميع القيود
     * 
     * مثال / Example:
     *   T: قابل_للمقارنة
     *   satisfiesConstraints(Integer) → true (إذا كان Integer يُطبق Comparable)
     */
    bool satisfiesConstraints(const TypePtr& type) const;
    
    /**
     * التحقق من كون المعامل covariant / Check if parameter is covariant
     * 
     * @return true إذا كان المعامل covariant (+T)
     */
    bool isCovariant() const { return variance_ == Variance::Covariant; }
    
    /**
     * التحقق من كون المعامل contravariant / Check if parameter is contravariant
     * 
     * @return true إذا كان المعامل contravariant (-T)
     */
    bool isContravariant() const { return variance_ == Variance::Contravariant; }
    
    /**
     * التحقق من كون المعامل invariant / Check if parameter is invariant
     * 
     * @return true إذا كان المعامل invariant (T)
     */
    bool isInvariant() const { return variance_ == Variance::Invariant; }
    
    /**
     * نوع الـ variance
     */
    enum class Variance {
        Covariant,      // +T: out position only
        Contravariant,  // -T: in position only
        Invariant       // T: both in and out
    };
    
    /**
     * الحصول على variance / Get variance
     */
    Variance getVariance() const { return variance_; }
    
    /**
     * تعيين variance / Set variance
     */
    void setVariance(Variance v) { variance_ = v; }
    
    /**
     * التحقق من كون المعامل مُقيداً بشكل كافٍ / Check if parameter is sufficiently bounded
     * 
     * @return true إذا كان للمعامل حدود كافية للاستخدام
     */
    bool isSufficientlyBounded() const {
        return hasUpperBound() || hasConstraints();
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // علاقات الأنواع / Type Relations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التحقق من إمكانية تحويل هذا المعامل إلى نوع آخر
     * Check if this parameter can be converted to another type
     * 
     * معامل النوع يمكن تحويله إلى:
     * - معامل نوع آخر بنفس الاسم
     * - النوع العلوي الخاص به (upper bound)
     * - أي قيد آخر
     */
    bool isAssignableTo(const Type* other) const override;
    
    /**
     * التحقق من كون هذا المعامل نوعاً فرعياً من نوع آخر
     * Check if this parameter is a subtype of another
     */
    bool isSubtypeOf(const Type* other) const override;
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string name_;                               // اسم المعامل / Parameter name
    std::vector<TypeConstraintPtr> constraints_;     // القيود / Constraints
    TypePtr upperBound_;                             // النوع العلوي / Upper bound
    TypePtr lowerBound_;                             // النوع السفلي / Lower bound
    Variance variance_;                              // نوع الـ variance / Variance type
    
    /**
     * التحقق من قيد واحد / Check single constraint
     * 
     * @param constraint القيد المراد التحقق منه
     * @param type النوع المراد فحصه
     * @return true إذا كان النوع يُطابق القيد
     */
    bool checkConstraint(const TypeConstraintPtr& constraint, const TypePtr& type) const;
};

// ════════════════════════════════════════════════════════════════════════════════
// Pointer Type للاستخدام المريح / Pointer Type for Convenience
// ════════════════════════════════════════════════════════════════════════════════

using TypeParameterPtr = std::shared_ptr<TypeParameter>;

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

/**
 * إنشاء معامل نوع جديد / Create a new type parameter
 * 
 * @param name اسم المعامل
 * @return معامل النوع الجديد
 */
TypeParameterPtr makeTypeParameter(const std::string& name);

/**
 * إنشاء معامل نوع مع نوع علوي / Create type parameter with upper bound
 * 
 * @param name اسم المعامل
 * @param upperBound النوع العلوي
 * @return معامل النوع الجديد
 */
TypeParameterPtr makeTypeParameterWithBound(const std::string& name, TypePtr upperBound);

/**
 * إنشاء معامل نوع مع قيود / Create type parameter with constraints
 * 
 * @param name اسم المعامل
 * @param constraints القيود
 * @return معامل النوع الجديد
 */
TypeParameterPtr makeTypeParameterWithConstraints(const std::string& name, 
                                                   const std::vector<TypeConstraintPtr>& constraints);

/**
 * التحقق من كون نوع معامل نوع / Check if type is type parameter
 * 
 * @param type النوع المراد فحصه
 * @return true إذا كان النوع معامل نوع
 */
bool isTypeParameter(const TypePtr& type);

/**
 * تحويل نوع إلى معامل نوع (cast آمن) / Cast type to type parameter (safe cast)
 * 
 * @param type النوع المراد تحويله
 * @return معامل النوع أو nullptr إذا لم يكن معامل نوع
 */
TypeParameterPtr asTypeParameter(const TypePtr& type);

/**
 * إنشاء قائمة معاملات أنواع من أسماء / Create type parameter list from names
 * 
 * @param names قائمة الأسماء (مثل ["T", "K", "V"])
 * @return قائمة معاملات الأنواع
 */
std::vector<TypeParameterPtr> makeTypeParameterList(const std::vector<std::string>& names);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_TYPE_PARAMETER_H
