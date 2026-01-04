// ════════════════════════════════════════════════════════════════════════════════
// ملف: optional_type.h
// File: optional_type.h
//
// الوصف: تعريف الأنواع الاختيارية (Optional Types) - syntactic sugar لـ Union
// Description: Optional Types definition - syntactic sugar for Union
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// Phase: 1.3.5.0 - Union Type Core
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_OPTIONAL_TYPE_H
#define SAD_COMPILER_TYPE_SYSTEM_OPTIONAL_TYPE_H

#include "union_type.h"

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: OptionalType
// Class: OptionalType
//
// الوصف: نوع اختياري - syntactic sugar لـ T | null
// Description: Optional type - syntactic sugar for T | null
//
// أمثلة / Examples:
//   - Integer? = Integer | null
//   - String? = String | null
//   - Array<T>? = Array<T> | null
//
// في لغات أخرى / In other languages:
//   - TypeScript: string | undefined
//   - Rust: Option<T>
//   - Swift: String?
//   - Kotlin: String?
// ════════════════════════════════════════════════════════════════════════════════

class OptionalType : public UnionType {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * منشئ من نوع / Constructor from type
     * 
     * @param innerType النوع الداخلي (T في T?)
     *                  Inner type (T in T?)
     * 
     * مثال / Example:
     *   OptionalType(intType) → Integer | null
     */
    explicit OptionalType(TypePtr innerType);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مُعاد تعريفها / Overridden Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    std::string getArabicName() const override;
    std::string getEnglishName() const override;
    std::string toString() const override;
    std::shared_ptr<Type> clone() const override;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال الأنواع الاختيارية / Optional Type-Specific Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على النوع الداخلي / Get inner type
     * 
     * @return النوع الداخلي (T في T?)
     * 
     * مثال / Example:
     *   Integer?.getInnerType() → Integer
     */
    TypePtr getInnerType() const;
    
    /**
     * هل القيمة موجودة (ليست null) / Is value present (not null)
     * 
     * @param value القيمة المراد التحقق منها
     * @return true إذا كانت القيمة موجودة
     * 
     * ملاحظة: هذه دالة مفاهيمية، التحقق الفعلي يتم في runtime
     * Note: Conceptual function, actual check happens at runtime
     */
    bool isPresent() const;
    
    /**
     * التحويل إلى non-optional type / Convert to non-optional type
     * 
     * @return النوع بدون optional
     * 
     * مثال / Example:
     *   Integer?.toNonOptional() → Integer
     */
    TypePtr toNonOptional() const;
    
    /**
     * هل النوع optional / Is type optional
     * 
     * @return true دائماً (هذا optional type)
     */
    bool isOptional() const { return true; }
    
private:
    TypePtr innerType_;  // النوع الداخلي / Inner type
};

// ════════════════════════════════════════════════════════════════════════════════
// Pointer Type للاستخدام المريح / Pointer Type for Convenience
// ════════════════════════════════════════════════════════════════════════════════

using OptionalTypePtr = std::shared_ptr<OptionalType>;

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

/**
 * إنشاء نوع اختياري / Create optional type
 * 
 * @param innerType النوع الداخلي
 * @return نوع اختياري
 * 
 * مثال / Example:
 *   makeOptionalType(intType) → Integer?
 */
OptionalTypePtr makeOptionalType(TypePtr innerType);

/**
 * التحقق من كون نوع optional / Check if type is optional
 * 
 * @param type النوع المراد التحقق منه
 * @return true إذا كان النوع optional
 * 
 * مثال / Example:
 *   isOptionalType(Integer?) → true
 *   isOptionalType(Integer) → false
 */
bool isOptionalType(const TypePtr& type);

/**
 * تحويل نوع إلى optional type (cast آمن) / Cast type to optional (safe cast)
 * 
 * @param type النوع المراد تحويله
 * @return optional type أو nullptr إذا لم يكن optional
 */
OptionalTypePtr asOptionalType(const TypePtr& type);

/**
 * استخراج النوع الداخلي من optional / Extract inner type from optional
 * 
 * @param type نوع اختياري
 * @return النوع الداخلي أو نفس النوع إذا لم يكن optional
 * 
 * مثال / Example:
 *   unwrapOptional(Integer?) → Integer
 *   unwrapOptional(Integer) → Integer
 */
TypePtr unwrapOptional(const TypePtr& type);

/**
 * تحويل نوع إلى optional / Convert type to optional
 * 
 * @param type النوع المراد تحويله
 * @return نوع اختياري
 * 
 * مثال / Example:
 *   toOptional(Integer) → Integer?
 *   toOptional(Integer?) → Integer? (no double wrapping)
 */
TypePtr toOptional(const TypePtr& type);

/**
 * التحقق من كون union type هو في الحقيقة optional / Check if union is optional
 * 
 * @param type نوع اتحادي
 * @return true إذا كان الـ union هو T | null فقط
 * 
 * مثال / Example:
 *   isUnionOptional(Integer | null) → true
 *   isUnionOptional(Integer | String) → false
 *   isUnionOptional(Integer | String | null) → false
 */
bool isUnionOptional(const UnionTypePtr& unionType);

/**
 * تحويل union إلى optional إذا كان ممكناً / Convert union to optional if possible
 * 
 * @param unionType نوع اتحادي
 * @return optional type إذا كان الـ union هو T | null، وإلا نفس الـ union
 * 
 * مثال / Example:
 *   unionToOptional(Integer | null) → Integer?
 *   unionToOptional(Integer | String) → Integer | String (unchanged)
 */
TypePtr unionToOptional(const UnionTypePtr& unionType);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_OPTIONAL_TYPE_H
