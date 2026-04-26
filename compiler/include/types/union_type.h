// ════════════════════════════════════════════════════════════════════════════════
// ملف: union_type.h
// File: union_type.h
//
// الوصف: تعريف أنواع الاتحاد (Union Types)
// Description: Union Types definition
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// Phase: 1.3.5.0 - Union Type Core
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_UNION_TYPE_H
#define SAD_COMPILER_TYPE_SYSTEM_UNION_TYPE_H

#include "type.h"
#include <vector>
#include <memory>
#include <unordered_set>
#include <algorithm>

namespace Sad {
namespace TypeSystem {

// إعلان مسبق / Forward declaration
class UnionType;
using UnionTypePtr = std::shared_ptr<UnionType>;

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: UnionType
// Class: UnionType
//
// الوصف: يمثل نوع اتحاد (union type) - نوع يمكن أن يكون أحد عدة أنواع
// Description: Represents a union type - a type that can be one of several types
//
// أمثلة / Examples:
//   - Integer | String (رقم | نص)
//   - Success<T> | Failure<E>
//   - Integer | String | Boolean | null
//
// Union Types في لغات أخرى:
//   - TypeScript: number | string
//   - Rust: enum Result<T, E> { Ok(T), Err(E) }
//   - Haskell: Either a b
// ════════════════════════════════════════════════════════════════════════════════

class UnionType : public Type {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * منشئ من قائمة أنواع / Constructor from type list
     * 
     * @param alternatives الأنواع البديلة المحتملة
     *                     The alternative possible types
     * 
     * مثال / Example:
     *   auto unionType = UnionType({intType, stringType, boolType});
     *   // يمثل: Integer | String | Boolean
     */
    explicit UnionType(const TypeList& alternatives);
    
    /**
     * منشئ من نوعين / Constructor from two types
     * 
     * @param type1 النوع الأول
     * @param type2 النوع الثاني
     * 
     * مثال / Example:
     *   auto unionType = UnionType(intType, stringType);
     *   // يمثل: Integer | String
     */
    UnionType(TypePtr type1, TypePtr type2);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مجردة مُطبقة / Implemented Abstract Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string getArabicName() const override;
    std::string getEnglishName() const override;
    std::string toString() const override;
    bool equals(const Type* other) const override;
    std::shared_ptr<Type> clone() const override;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال الأنواع الاتحادية / Union Type-Specific Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على الأنواع البديلة / Get alternative types
     * 
     * @return قائمة الأنواع البديلة
     */
    const TypeList& getAlternatives() const { return alternatives_; }
    
    /**
     * عدد الأنواع البديلة / Number of alternatives
     * 
     * @return عدد الأنواع
     */
    size_t size() const { return alternatives_.size(); }
    
    /**
     * هل القائمة فارغة / Is empty
     * 
     * @return true إذا لم يكن هناك أنواع بديلة
     */
    bool isEmpty() const { return alternatives_.empty(); }
    
    /**
     * التحقق من احتواء نوع / Check if contains type
     * 
     * @param type النوع المراد التحقق منه
     * @return true إذا كان النوع من ضمن البدائل
     * 
     * مثال / Example:
     *   UnionType u = Integer | String | Boolean
     *   u.contains(Integer) → true
     *   u.contains(Float) → false
     */
    bool contains(const TypePtr& type) const;
    
    /**
     * التحقق من احتواء نوع (بالاسم) / Check if contains type by name
     * 
     * @param typeName اسم النوع
     * @return true إذا كان النوع موجوداً
     */
    bool containsTypeName(const std::string& typeName) const;
    
    /**
     * تبسيط النوع الاتحادي / Simplify union type
     * 
     * يزيل التكرار ويُرتب الأنواع
     * Removes duplicates and sorts types
     * 
     * @return نوع اتحادي مُبسط
     * 
     * مثال / Example:
     *   (Integer | String | Integer).simplify() → Integer | String
     *   (Integer).simplify() → Integer (not a union)
     */
    UnionTypePtr simplify() const;
    
    /**
     * تقاطع نوعين اتحاديين / Intersection of two union types
     * 
     * @param other النوع الاتحادي الآخر
     * @return النوع الاتحادي الناتج من التقاطع
     * 
     * مثال / Example:
     *   (Integer | String | Boolean).intersection(String | Boolean | Float)
     *   → String | Boolean
     */
    UnionTypePtr intersection(const UnionTypePtr& other) const;
    
    /**
     * اتحاد نوعين اتحاديين / Union of two union types
     * 
     * @param other النوع الاتحادي الآخر
     * @return النوع الاتحادي الناتج من الاتحاد
     * 
     * مثال / Example:
     *   (Integer | String).union(Boolean | Float)
     *   → Integer | String | Boolean | Float
     */
    UnionTypePtr unionWith(const UnionTypePtr& other) const;
    
    /**
     * طرح نوع من الاتحاد / Subtract type from union
     * 
     * @param type النوع المراد طرحه
     * @return نوع اتحادي بدون النوع المُحدد
     * 
     * مثال / Example:
     *   (Integer | String | Boolean).difference(Integer)
     *   → String | Boolean
     * 
     * الاستخدام / Usage:
     *   Type narrowing بعد type guard
     */
    UnionTypePtr difference(const TypePtr& type) const;
    
    /**
     * طرح نوع اتحادي من نوع اتحادي / Subtract union from union
     * 
     * @param other النوع الاتحادي المراد طرحه
     * @return نوع اتحادي بدون الأنواع المُحددة
     * 
     * مثال / Example:
     *   (Integer | String | Boolean).difference(Integer | String)
     *   → Boolean
     */
    UnionTypePtr difference(const UnionTypePtr& other) const;
    
    /**
     * الحصول على النوع البديل بالفهرس / Get alternative by index
     * 
     * @param index الفهرس (0-based)
     * @return النوع البديل أو nullptr إذا كان الفهرس خاطئ
     */
    TypePtr getAlternative(size_t index) const;
    
    /**
     * هل النوع الاتحادي يحتوي على null / Contains null type
     * 
     * @return true إذا كان أحد البدائل null
     * 
     * مثال / Example:
     *   (Integer | null).containsNull() → true
     *   (Integer | String).containsNull() → false
     */
    bool containsNull() const;
    
    /**
     * إزالة null من الاتحاد / Remove null from union
     * 
     * @return نوع اتحادي بدون null
     * 
     * مثال / Example:
     *   (Integer | String | null).removeNull() → Integer | String
     */
    UnionTypePtr removeNull() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // علاقات الأنواع / Type Relations
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * التحقق من إمكانية التحويل / Check assignability
     * 
     * UnionType يمكن تحويله إلى:
     * - نفس Union type
     * - Union type يحتوي على جميع بدائله
     * 
     * مثال / Example:
     *   Integer → (Integer | String) ✅
     *   (Integer | String) → Integer ❌
     *   (Integer | String) → (Integer | String | Boolean) ✅
     */
    bool isAssignableTo(const Type* other) const override;
    
    /**
     * التحقق من كون هذا النوع فرعياً من نوع آخر
     * Check if this is a subtype of another
     * 
     * UnionType هو subtype من other إذا:
     * - كل بديل في Union هو subtype من other
     * أو
     * - other هو Union يحتوي على جميع بدائل هذا Union
     */
    bool isSubtypeOf(const Type* other) const override;
    
    /**
     * التحقق من كون نوع subtype من هذا Union
     * Check if type is subtype of this union
     * 
     * @param type النوع المراد التحقق منه
     * @return true إذا كان النوع subtype من أحد البدائل
     */
    bool isSuperTypeOf(const TypePtr& type) const;
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    TypeList alternatives_;  // الأنواع البديلة / Alternative types
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال الداخلية / Internal Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تسطيح الأنواع الاتحادية المتداخلة / Flatten nested unions
     * 
     * مثال / Example:
     *   (Integer | (String | Boolean)) → Integer | String | Boolean
     */
    void flatten();
    
    /**
     * إزالة التكرار / Remove duplicates
     */
    void removeDuplicates();
    
    /**
     * ترتيب الأنواع / Sort types
     * 
     * للحصول على تمثيل متسق / For consistent representation
     */
    void sortAlternatives();
};

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

/**
 * إنشاء نوع اتحادي من قائمة أنواع / Create union type from type list
 * 
 * @param alternatives قائمة الأنواع البديلة
 * @return نوع اتحادي
 */
UnionTypePtr makeUnionType(const TypeList& alternatives);

/**
 * إنشاء نوع اتحادي من نوعين / Create union type from two types
 * 
 * @param type1 النوع الأول
 * @param type2 النوع الثاني
 * @return نوع اتحادي
 */
UnionTypePtr makeUnionType(TypePtr type1, TypePtr type2);

/**
 * إنشاء نوع اتحادي من ثلاثة أنواع / Create union type from three types
 */
UnionTypePtr makeUnionType(TypePtr type1, TypePtr type2, TypePtr type3);

/**
 * التحقق من كون نوع union type / Check if type is union type
 * 
 * @param type النوع المراد التحقق منه
 * @return true إذا كان النوع union type
 */
bool isUnionType(const TypePtr& type);

/**
 * تحويل نوع إلى union type (cast آمن) / Cast type to union type (safe cast)
 * 
 * @param type النوع المراد تحويله
 * @return union type أو nullptr إذا لم يكن union type
 */
UnionTypePtr asUnionType(const TypePtr& type);

/**
 * اتحاد نوعين (helper) / Union of two types (helper)
 * 
 * @param type1 النوع الأول
 * @param type2 النوع الثاني
 * @return union type أو أحد الأنواع إذا كان أحدهما subtype من الآخر
 * 
 * مثال / Example:
 *   unionOf(Integer, String) → Integer | String
 *   unionOf(Integer, Integer) → Integer
 */
TypePtr unionOf(TypePtr type1, TypePtr type2);

/**
 * اتحاد قائمة أنواع / Union of type list
 * 
 * @param types قائمة الأنواع
 * @return union type مُبسط
 */
TypePtr unionOf(const TypeList& types);

/**
 * تقاطع نوعين / Intersection of two types
 * 
 * @param type1 النوع الأول
 * @param type2 النوع الثاني
 * @return تقاطع الأنواع أو nullptr إذا لم يكن هناك تقاطع
 * 
 * مثال / Example:
 *   intersectionOf(Integer | String, String | Boolean) → String
 *   intersectionOf(Integer, String) → nullptr
 */
TypePtr intersectionOf(TypePtr type1, TypePtr type2);

/**
 * طرح نوع من نوع آخر / Subtract type from type
 * 
 * @param from النوع الأصلي
 * @param toRemove النوع المراد إزالته
 * @return النوع بعد الطرح أو nullptr إذا لم يبقَ شيء
 * 
 * مثال / Example:
 *   differenceOf(Integer | String, Integer) → String
 *   differenceOf(Integer, Integer) → nullptr
 */
TypePtr differenceOf(TypePtr from, TypePtr toRemove);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_UNION_TYPE_H
