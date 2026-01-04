// ════════════════════════════════════════════════════════════════════════════════
// ملف: generic_type.h
// File: generic_type.h
//
// الوصف: تعريف الأنواع العامة (Generic Types) لنظام الأنواع
// Description: Generic Types definition for the type system
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_GENERIC_TYPE_H
#define SAD_COMPILER_TYPE_SYSTEM_GENERIC_TYPE_H

#include "type.h"
#include "type_parameter.h"
#include <unordered_map>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: GenericType
// Class: GenericType
//
// الوصف: يمثل نوعاً عاماً (Generic Type) مع معاملات أنواع
// Description: Represents a generic type with type parameters
//
// أمثلة / Examples:
//   - مصفوفة<T>  →  Array<T>
//   - قاموس<K, V>  →  Dictionary<K, V>
//   - صندوق<T>  →  Box<T>
// ════════════════════════════════════════════════════════════════════════════════

class GenericType : public Type {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * منشئ للنوع العام / Constructor for generic type
     * 
     * @param baseName الاسم الأساسي للنوع (مثل "Array", "Dictionary")
     *                 Base name of the type (e.g., "Array", "Dictionary")
     * @param typeParameters معاملات الأنواع (مثل [T], [K, V])
     *                       Type parameters (e.g., [T], [K, V])
     */
    GenericType(const std::string& baseName, const std::vector<TypeParameterPtr>& typeParameters);
    
    /**
     * منشئ للنوع العام المُخصص / Constructor for specialized generic type
     * 
     * @param baseName الاسم الأساسي
     * @param typeArguments الأنواع الفعلية (مثل [Integer], [String, Integer])
     *                      Actual types (e.g., [Integer], [String, Integer])
     */
    GenericType(const std::string& baseName, const TypeList& typeArguments);
    
    // المُنشئ الافتراضي محذوف / Default constructor deleted
    GenericType() = delete;
    
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
    // الدوال الخاصة بالأنواع العامة / Generic-Specific Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على الاسم الأساسي / Get base name
     * 
     * @return الاسم الأساسي للنوع (مثل "Array", "Box")
     */
    const std::string& getBaseName() const { return baseName_; }
    
    /**
     * الحصول على معاملات الأنواع / Get type parameters
     * 
     * @return قائمة معاملات الأنواع (مثل [T], [K, V])
     */
    const std::vector<TypeParameterPtr>& getTypeParameters() const { 
        return typeParameters_; 
    }
    
    /**
     * الحصول على الأنواع الفعلية / Get type arguments
     * 
     * @return قائمة الأنواع الفعلية (إذا كان مُخصصاً)
     *         List of actual types (if specialized)
     */
    const TypeList& getTypeArguments() const { 
        return typeArguments_; 
    }
    
    /**
     * التحقق من كون النوع مُخصصاً / Check if type is specialized
     * 
     * @return true إذا كان النوع مُخصصاً بأنواع فعلية
     *         true if type is specialized with actual types
     */
    bool isSpecialized() const { 
        return !typeArguments_.empty(); 
    }
    
    /**
     * تخصيص النوع العام بأنواع فعلية / Specialize generic type with actual types
     * 
     * @param typeArgs الأنواع الفعلية للتخصيص
     *                 Actual types for specialization
     * @return نوع عام جديد مُخصص / New specialized generic type
     * 
     * مثال / Example:
     *   Array<T> + [Integer] → Array<Integer>
     */
    std::shared_ptr<GenericType> specialize(const TypeList& typeArgs) const;
    
    /**
     * إنشاء خريطة استبدال من معاملات الأنواع إلى الأنواع الفعلية
     * Create substitution map from type parameters to actual types
     * 
     * @return خريطة استبدال / Substitution map
     */
    std::unordered_map<std::string, TypePtr> getSubstitutionMap() const;
    
    /**
     * استبدال معاملات الأنواع بالأنواع الفعلية في نوع
     * Substitute type parameters with actual types in a type
     * 
     * @param type النوع المراد استبدال معاملاته
     * @return النوع بعد الاستبدال
     */
    TypePtr substituteTypeParameters(TypePtr type) const;
    
    /**
     * التحقق من عدد معاملات الأنواع / Check type parameter count
     * 
     * @param count العدد المتوقع
     * @return true إذا كان عدد المعاملات مطابقاً
     */
    bool hasTypeParameterCount(size_t count) const {
        return typeParameters_.size() == count;
    }
    
    /**
     * الحصول على عدد معاملات الأنواع / Get type parameter count
     * 
     * @return عدد معاملات الأنواع
     */
    size_t getTypeParameterCount() const {
        return typeParameters_.size();
    }
    
    /**
     * الحصول على معامل نوع بالفهرس / Get type parameter by index
     * 
     * @param index فهرس المعامل (0-based)
     * @return معامل النوع أو nullptr إذا كان الفهرس خارج الحدود
     */
    TypeParameterPtr getTypeParameter(size_t index) const;
    
    /**
     * الحصول على معامل نوع بالاسم / Get type parameter by name
     * 
     * @param name اسم المعامل
     * @return معامل النوع أو nullptr إذا لم يُعثر عليه
     */
    TypeParameterPtr getTypeParameter(const std::string& name) const;
    
    /**
     * الحصول على النوع الفعلي بالفهرس / Get type argument by index
     * 
     * @param index فهرس النوع (0-based)
     * @return النوع الفعلي أو nullptr إذا كان الفهرس خارج الحدود
     */
    TypePtr getTypeArgument(size_t index) const;
    
    /**
     * الحصول على النوع الفعلي بمعامل النوع / Get type argument by type parameter
     * 
     * @param param معامل النوع
     * @return النوع الفعلي المقابل أو nullptr
     */
    TypePtr getTypeArgument(const TypeParameterPtr& param) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // علاقات الأنواع / Type Relations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التحقق من إمكانية تحويل هذا النوع إلى نوع آخر
     * Check if this type can be converted to another
     * 
     * للأنواع العامة:
     * - Array<T> يمكن تحويله إلى Array<U> إذا كان T يمكن تحويله إلى U
     * - Covariance للأنواع القراءة فقط
     * - Contravariance للأنواع الكتابة فقط
     * - Invariance للأنواع القراءة والكتابة
     */
    bool isAssignableTo(const Type* other) const override;
    
    /**
     * التحقق من كون هذا النوع نوعاً فرعياً من نوع آخر
     * Check if this type is a subtype of another
     */
    bool isSubtypeOf(const Type* other) const override;
    
    /**
     * التحقق من variance للنوع العام / Check variance of generic type
     * 
     * @return نوع الـ variance (Covariant, Contravariant, Invariant)
     */
    enum class Variance {
        Covariant,      // +T: Array<Dog> <: Array<Animal> إذا Dog <: Animal
        Contravariant,  // -T: Comparator<Animal> <: Comparator<Dog>
        Invariant       // T: Box<T> لا يتوافق إلا مع Box<T> فقط
    };
    
    Variance getVariance() const { return variance_; }
    void setVariance(Variance v) { variance_ = v; }
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string baseName_;                           // الاسم الأساسي / Base name
    std::vector<TypeParameterPtr> typeParameters_;   // معاملات الأنواع / Type parameters
    TypeList typeArguments_;                         // الأنواع الفعلية / Type arguments
    Variance variance_;                              // نوع الـ variance / Variance type
    
    // خريطة للبحث السريع عن معاملات الأنواع بالاسم
    // Map for quick lookup of type parameters by name
    std::unordered_map<std::string, size_t> parameterNameToIndex_;
    
    /**
     * بناء خريطة معاملات الأنواع / Build type parameter map
     */
    void buildParameterMap();
    
    /**
     * التحقق من تطابق الأنواع الفعلية / Validate type arguments
     * 
     * @return true إذا كانت الأنواع الفعلية صحيحة
     */
    bool validateTypeArguments() const;
};

// ════════════════════════════════════════════════════════════════════════════════
// Pointer Type للاستخدام المريح / Pointer Type for Convenience
// ════════════════════════════════════════════════════════════════════════════════

using GenericTypePtr = std::shared_ptr<GenericType>;

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

/**
 * إنشاء نوع عام جديد / Create a new generic type
 * 
 * @param baseName الاسم الأساسي
 * @param typeParameters معاملات الأنواع
 * @return النوع العام الجديد
 */
GenericTypePtr makeGenericType(const std::string& baseName, 
                               const std::vector<TypeParameterPtr>& typeParameters);

/**
 * إنشاء نوع عام مُخصص / Create a specialized generic type
 * 
 * @param baseName الاسم الأساسي
 * @param typeArguments الأنواع الفعلية
 * @return النوع العام المُخصص
 */
GenericTypePtr makeSpecializedGenericType(const std::string& baseName, 
                                          const TypeList& typeArguments);

/**
 * التحقق من كون نوع عاماً / Check if type is generic
 * 
 * @param type النوع المراد فحصه
 * @return true إذا كان النوع عاماً
 */
bool isGenericType(const TypePtr& type);

/**
 * تحويل نوع إلى نوع عام (cast آمن) / Cast type to generic type (safe cast)
 * 
 * @param type النوع المراد تحويله
 * @return النوع العام أو nullptr إذا لم يكن عاماً
 */
GenericTypePtr asGenericType(const TypePtr& type);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_GENERIC_TYPE_H
