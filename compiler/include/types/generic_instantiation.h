// ════════════════════════════════════════════════════════════════════════════════
// ملف: generic_instantiation.h
// File: generic_instantiation.h
//
// الوصف: تنفيذ instantiation للأنواع العامة (Monomorphization)
// Description: Generic instantiation implementation (Monomorphization)
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_GENERIC_INSTANTIATION_H
#define SAD_COMPILER_TYPE_SYSTEM_GENERIC_INSTANTIATION_H

#include "generic_type.h"
#include "type_parameter.h"
#include <unordered_map>
#include <vector>
#include <string>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: GenericInstantiator
// Class: GenericInstantiator
//
// الوصف: مسؤول عن instantiation (monomorphization) للأنواع العامة
// Description: Responsible for instantiation (monomorphization) of generic types
//
// Monomorphization:
//   تحويل كود عام إلى نسخ مُخصصة لكل نوع فعلي
//   Converting generic code to specialized versions for each actual type
//
// أمثلة / Examples:
//   دالة عكس<T>(قائمة: مصفوفة<T>) → عكس_Integer, عكس_String, etc.
//   reverse<T>(list: Array<T>) → reverse_Integer, reverse_String, etc.
// ════════════════════════════════════════════════════════════════════════════════

class GenericInstantiator {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * منشئ افتراضي / Default constructor
     */
    GenericInstantiator();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال Instantiation الرئيسية / Main Instantiation Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إنشاء نسخة مُخصصة من نوع عام / Instantiate a generic type
     * 
     * @param genericType النوع العام (مثل Array<T>)
     * @param typeArgs الأنواع الفعلية (مثل [Integer])
     * @return النوع المُخصص (مثل Array<Integer>)
     * 
     * مثال / Example:
     *   Array<T> + [Integer] → Array<Integer>
     *   Dictionary<K, V> + [String, Integer] → Dictionary<String, Integer>
     */
    GenericTypePtr instantiate(const GenericTypePtr& genericType, 
                                const TypeList& typeArgs);
    
    /**
     * إنشاء نسخة مُخصصة مع التحقق من القيود
     * Instantiate with constraint checking
     * 
     * @param genericType النوع العام
     * @param typeArgs الأنواع الفعلية
     * @param errors [out] قائمة الأخطاء إذا فشل التحقق
     * @return النوع المُخصص أو nullptr إذا فشل
     */
    GenericTypePtr instantiateWithValidation(const GenericTypePtr& genericType, 
                                              const TypeList& typeArgs,
                                              std::vector<std::string>* errors = nullptr);
    
    /**
     * الحصول على أو إنشاء instantiation (مع caching)
     * Get or create instantiation (with caching)
     * 
     * @param genericType النوع العام
     * @param typeArgs الأنواع الفعلية
     * @return النوع المُخصص (من cache أو مُنشأ جديد)
     */
    GenericTypePtr getOrInstantiate(const GenericTypePtr& genericType, 
                                     const TypeList& typeArgs);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // Type Substitution
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * استبدال معاملات الأنواع في نوع / Substitute type parameters in a type
     * 
     * @param type النوع المراد استبدال معاملاته
     * @param substitutionMap خريطة الاستبدال (T → Integer, K → String, etc.)
     * @return النوع بعد الاستبدال
     * 
     * مثال / Example:
     *   substituteTypeParameters(Array<T>, {T → Integer}) → Array<Integer>
     *   substituteTypeParameters(T, {T → String}) → String
     */
    TypePtr substituteTypeParameters(const TypePtr& type, 
                                      const std::unordered_map<std::string, TypePtr>& substitutionMap);
    
    /**
     * استبدال معاملات الأنواع في قائمة أنواع / Substitute in type list
     * 
     * @param types قائمة الأنواع
     * @param substitutionMap خريطة الاستبدال
     * @return قائمة الأنواع بعد الاستبدال
     */
    TypeList substituteTypeList(const TypeList& types, 
                                 const std::unordered_map<std::string, TypePtr>& substitutionMap);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // Validation
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التحقق من صحة instantiation / Validate instantiation
     * 
     * @param genericType النوع العام
     * @param typeArgs الأنواع الفعلية
     * @param errors [out] قائمة الأخطاء إذا فشل التحقق
     * @return true إذا كان صحيحاً
     */
    bool validate(const GenericTypePtr& genericType, 
                  const TypeList& typeArgs,
                  std::vector<std::string>* errors = nullptr);
    
    /**
     * التحقق من عدد معاملات الأنواع / Validate type parameter count
     * 
     * @param genericType النوع العام
     * @param typeArgs الأنواع الفعلية
     * @return true إذا كان العدد صحيحاً
     */
    bool validateParameterCount(const GenericTypePtr& genericType, 
                                 const TypeList& typeArgs);
    
    /**
     * التحقق من القيود / Validate constraints
     * 
     * @param genericType النوع العام
     * @param typeArgs الأنواع الفعلية
     * @param errors [out] قائمة الأخطاء
     * @return true إذا كانت جميع القيود مُطابقة
     */
    bool validateConstraints(const GenericTypePtr& genericType, 
                              const TypeList& typeArgs,
                              std::vector<std::string>* errors = nullptr);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // Cache Management
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * مسح cache / Clear cache
     */
    void clearCache();
    
    /**
     * الحصول على عدد instantiations في cache / Get cache size
     * 
     * @return عدد الـ instantiations المخزنة
     */
    size_t getCacheSize() const;
    
    /**
     * التحقق من وجود instantiation في cache / Check if instantiation is cached
     * 
     * @param key مفتاح الـ instantiation
     * @return true إذا كان موجوداً
     */
    bool isCached(const std::string& key) const;
    
    /**
     * الحصول على مفتاح cache / Get cache key
     * 
     * @param genericType النوع العام
     * @param typeArgs الأنواع الفعلية
     * @return مفتاح الـ cache
     */
    std::string getCacheKey(const GenericTypePtr& genericType, 
                            const TypeList& typeArgs) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // Statistics
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على إحصائيات الاستخدام / Get usage statistics
     */
    struct Statistics {
        size_t totalInstantiations;      // إجمالي الـ instantiations
        size_t cacheHits;                // عدد المرات التي وُجدت في cache
        size_t cacheMisses;              // عدد المرات التي لم تُوجد في cache
        size_t validationFailures;       // عدد فشل التحقق
        
        Statistics() 
            : totalInstantiations(0)
            , cacheHits(0)
            , cacheMisses(0)
            , validationFailures(0)
        {}
    };
    
    /**
     * الحصول على الإحصائيات / Get statistics
     */
    const Statistics& getStatistics() const { return stats_; }
    
    /**
     * إعادة تعيين الإحصائيات / Reset statistics
     */
    void resetStatistics();
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // Cache: مفتاح → النوع المُخصص
    // Cache: key → specialized type
    std::unordered_map<std::string, GenericTypePtr> instantiationCache_;
    
    // إحصائيات الاستخدام / Usage statistics
    Statistics stats_;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال الداخلية / Internal Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * استبدال في نوع واحد / Substitute in single type
     */
    TypePtr substituteSingleType(const TypePtr& type, 
                                  const std::unordered_map<std::string, TypePtr>& substitutionMap);
    
    /**
     * استبدال في نوع عام / Substitute in generic type
     */
    TypePtr substituteInGenericType(const GenericTypePtr& genericType, 
                                     const std::unordered_map<std::string, TypePtr>& substitutionMap);
    
    /**
     * التحقق من قيد واحد / Validate single constraint
     */
    bool validateSingleConstraint(const TypeConstraintPtr& constraint, 
                                   const TypePtr& type,
                                   std::string* error = nullptr);
};

// ════════════════════════════════════════════════════════════════════════════════
// Global Instance للاستخدام المريح / Global Instance for Convenience
// ════════════════════════════════════════════════════════════════════════════════

/**
 * الحصول على instantiator عام / Get global instantiator
 * 
 * @return مرجع إلى الـ instantiator العام
 */
GenericInstantiator& getGlobalInstantiator();

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_GENERIC_INSTANTIATION_H
