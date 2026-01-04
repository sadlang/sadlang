// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_guard.h
// File: type_guard.h
//
// الوصف: تعريف Type Guards (حراس الأنواع)
// Description: Type Guards definition
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// Phase: 1.3.5.1 - Type Guards
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_TYPE_GUARD_H
#define SAD_COMPILER_TYPE_SYSTEM_TYPE_GUARD_H

#include "type.h"
#include "union_type.h"
#include <string>
#include <memory>
#include <vector>
#include <functional>

namespace Sad {
namespace TypeSystem {

// إعلانات مسبقة / Forward declarations
class TypeGuard;
class TypeGuardExpression;
class TypeGuardResult;

using TypeGuardPtr = std::shared_ptr<TypeGuard>;
using TypeGuardExpressionPtr = std::shared_ptr<TypeGuardExpression>;
using TypeGuardResultPtr = std::shared_ptr<TypeGuardResult>;

// ════════════════════════════════════════════════════════════════════════════════
// تعداد: TypeGuardKind
// Enum: TypeGuardKind
//
// الوصف: أنواع حراس الأنواع
// Description: Types of type guards
// ════════════════════════════════════════════════════════════════════════════════

enum class TypeGuardKind {
    // حراس الأنواع البدائية / Built-in Type Guards
    IsType,           // القيمة نوع نص / value is Type
    TypeOf,           // نوع_من(القيمة) / typeof(value)
    InstanceOf,       // القيمة من صنف X / value instanceof Class
    
    // حراس الأنواع المعرّفة من المستخدم / User-Defined Type Guards
    UserDefined,      // دالة تعيد (القيمة نوع T) / Function returning (value is T)
    
    // حراس معقدة / Complex Guards
    And,              // حارس1 و حارس2 / guard1 && guard2
    Or,               // حارس1 أو حارس2 / guard1 || guard2
    Not,              // ليس حارس / !guard
    
    // حراس خاصة / Special Guards
    Null,             // القيمة == عدم / value == null
    NotNull,          // القيمة != عدم / value != null
    HasProperty,      // القيمة.خاصية موجودة / value.property exists
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: TypeGuardResult
// Class: TypeGuardResult
//
// الوصف: نتيجة تطبيق حارس النوع
// Description: Result of applying a type guard
// ════════════════════════════════════════════════════════════════════════════════

class TypeGuardResult {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئ / Constructor
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * منشئ نتيجة حارس النوع
     * Constructor for type guard result
     * 
     * @param success هل نجح الحارس؟
     *                Did the guard succeed?
     * @param narrowedType النوع بعد التضييق (null إذا فشل)
     *                     Narrowed type (null if failed)
     * @param reason سبب الفشل (إن وُجد)
     *               Failure reason (if any)
     */
    TypeGuardResult(bool success, TypePtr narrowedType, const std::string& reason = "");
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال / Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * هل نجح الحارس؟ / Did the guard succeed?
     */
    bool isSuccess() const { return success_; }
    
    /**
     * الحصول على النوع بعد التضييق / Get narrowed type
     */
    TypePtr getNarrowedType() const { return narrowedType_; }
    
    /**
     * الحصول على سبب الفشل / Get failure reason
     */
    std::string getReason() const { return reason_; }
    
    /**
     * تحويل إلى نص / Convert to string
     */
    std::string toString() const;
    
private:
    bool success_;              // هل نجح الحارس؟ / Did the guard succeed?
    TypePtr narrowedType_;      // النوع بعد التضييق / Narrowed type
    std::string reason_;        // سبب الفشل / Failure reason
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: TypeGuard
// Class: TypeGuard
//
// الوصف: يمثل حارس نوع (Type Guard) - تعبير يضيّق نطاق النوع
// Description: Represents a type guard - an expression that narrows a type
//
// أمثلة / Examples:
//   - إذا (القيمة نوع رقم) { ... }
//     if (value is Integer) { ... }
//   
//   - إذا (نوع_من(القيمة) == "نص") { ... }
//     if (typeof(value) == "string") { ... }
//   
//   - دالة هو_نص(القيمة: أي) -> القيمة نوع نص {
//       ارجع نوع_من(القيمة) == "نص"
//     }
//     function isString(value: any): value is string {
//       return typeof(value) == "string"
//     }
//
// Type Guards في لغات أخرى:
//   - TypeScript: value is Type, typeof value === "string"
//   - Rust: if let Some(x) = value
//   - Python: isinstance(value, Type)
// ════════════════════════════════════════════════════════════════════════════════

class TypeGuard {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * منشئ حارس نوع
     * Constructor for type guard
     * 
     * @param kind نوع الحارس
     *             Guard kind
     * @param variableName اسم المتغير المراد فحصه
     *                     Variable name to check
     * @param targetType النوع المستهدف (للـ IsType، InstanceOf)
     *                   Target type (for IsType, InstanceOf)
     */
    TypeGuard(TypeGuardKind kind, const std::string& variableName, TypePtr targetType = nullptr);
    
    /**
     * منشئ لحراس معقدة (And, Or, Not)
     * Constructor for complex guards (And, Or, Not)
     * 
     * @param kind نوع الحارس (And/Or/Not)
     * @param guards الحراس الفرعية
     *               Sub-guards
     */
    TypeGuard(TypeGuardKind kind, const std::vector<TypeGuardPtr>& guards);
    
    /**
     * منشئ لحراس معرّفة من المستخدم
     * Constructor for user-defined guards
     * 
     * @param functionName اسم الدالة الحارس
     *                     Guard function name
     * @param variableName المتغير المفحوص
     *                     Variable being checked
     * @param returnType نوع الإرجاع (value is T)
     *                   Return type (value is T)
     */
    TypeGuard(const std::string& functionName, const std::string& variableName, TypePtr returnType);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال الأساسية / Core Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تطبيق الحارس على نوع معين
     * Apply guard to a given type
     * 
     * @param currentType النوع الحالي للمتغير
     *                    Current type of the variable
     * @return نتيجة تطبيق الحارس (النوع بعد التضييق)
     *         Result of applying guard (narrowed type)
     */
    TypeGuardResultPtr apply(TypePtr currentType) const;
    
    /**
     * هل الحارس ينطبق على النوع المعطى؟
     * Does the guard apply to the given type?
     * 
     * @param type النوع المراد فحصه
     *             Type to check
     * @return true إذا كان الحارس ينطبق
     */
    bool matches(TypePtr type) const;
    
    /**
     * الحصول على النوع بعد التضييق (إذا نجح الحارس)
     * Get narrowed type (if guard succeeds)
     * 
     * @param originalType النوع الأصلي
     *                     Original type
     * @return النوع المضيّق
     *         Narrowed type
     */
    TypePtr narrow(TypePtr originalType) const;
    
    /**
     * الحصول على النوع بعد الفشل (إذا فشل الحارس)
     * Get type after failure (if guard fails)
     * 
     * @param originalType النوع الأصلي
     *                     Original type
     * @return النوع بعد استبعاد ما لا ينطبق عليه الحارس
     *         Type after excluding what guard doesn't match
     */
    TypePtr narrowElse(TypePtr originalType) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال المساعدة / Helper Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * الحصول على نوع الحارس / Get guard kind
     */
    TypeGuardKind getKind() const { return kind_; }
    
    /**
     * الحصول على اسم المتغير / Get variable name
     */
    std::string getVariableName() const { return variableName_; }
    
    /**
     * الحصول على النوع المستهدف / Get target type
     */
    TypePtr getTargetType() const { return targetType_; }
    
    /**
     * الحصول على الحراس الفرعية / Get sub-guards
     */
    const std::vector<TypeGuardPtr>& getSubGuards() const { return subGuards_; }
    
    /**
     * تحويل إلى نص / Convert to string
     */
    std::string toString() const;
    
    /**
     * تحويل إلى صيغة عربية / Convert to Arabic form
     */
    std::string toArabic() const;
    
    /**
     * تحويل إلى صيغة إنجليزية / Convert to English form
     */
    std::string toEnglish() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال إنشاء حراس / Factory Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * إنشاء حارس IsType: القيمة نوع T
     * Create IsType guard: value is T
     */
    static TypeGuardPtr makeIsTypeGuard(const std::string& variableName, TypePtr targetType);
    
    /**
     * إنشاء حارس TypeOf: نوع_من(القيمة) == "نوع"
     * Create TypeOf guard: typeof(value) == "type"
     */
    static TypeGuardPtr makeTypeOfGuard(const std::string& variableName, const std::string& typeName);
    
    /**
     * إنشاء حارس InstanceOf: القيمة من صنف X
     * Create InstanceOf guard: value instanceof Class
     */
    static TypeGuardPtr makeInstanceOfGuard(const std::string& variableName, TypePtr classType);
    
    /**
     * إنشاء حارس Null: القيمة == عدم
     * Create Null guard: value == null
     */
    static TypeGuardPtr makeNullGuard(const std::string& variableName);
    
    /**
     * إنشاء حارس NotNull: القيمة != عدم
     * Create NotNull guard: value != null
     */
    static TypeGuardPtr makeNotNullGuard(const std::string& variableName);
    
    /**
     * إنشاء حارس And: حارس1 و حارس2
     * Create And guard: guard1 && guard2
     */
    static TypeGuardPtr makeAndGuard(const std::vector<TypeGuardPtr>& guards);
    
    /**
     * إنشاء حارس Or: حارس1 أو حارس2
     * Create Or guard: guard1 || guard2
     */
    static TypeGuardPtr makeOrGuard(const std::vector<TypeGuardPtr>& guards);
    
    /**
     * إنشاء حارس Not: ليس حارس
     * Create Not guard: !guard
     */
    static TypeGuardPtr makeNotGuard(TypeGuardPtr guard);
    
    /**
     * إنشاء حارس معرّف من المستخدم
     * Create user-defined guard
     */
    static TypeGuardPtr makeUserDefinedGuard(
        const std::string& functionName,
        const std::string& variableName,
        TypePtr returnType
    );

private:
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مساعدة داخلية / Internal Helper Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    /**
     * تضييق نوع Union بإزالة الأنواع التي لا تطابق الحارس
     * Narrow union type by removing alternatives that don't match guard
     */
    TypePtr narrowUnionType(UnionTypePtr unionType) const;
    
    /**
     * فحص إذا كان النوع يطابق حارس IsType
     * Check if type matches IsType guard
     */
    bool matchesIsType(TypePtr type) const;
    
    /**
     * فحص إذا كان النوع يطابق حارس TypeOf
     * Check if type matches TypeOf guard
     */
    bool matchesTypeOf(TypePtr type) const;
    
    /**
     * فحص إذا كان النوع يطابق حارس InstanceOf
     * Check if type matches InstanceOf guard
     */
    bool matchesInstanceOf(TypePtr type) const;

private:
    TypeGuardKind kind_;                    // نوع الحارس / Guard kind
    std::string variableName_;              // اسم المتغير / Variable name
    TypePtr targetType_;                    // النوع المستهدف / Target type
    std::vector<TypeGuardPtr> subGuards_;   // الحراس الفرعية / Sub-guards
    std::string functionName_;              // اسم دالة (للحراس المعرّفة) / Function name (for user-defined)
};

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة عامة / Global Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

/**
 * فحص إذا كان نوع معين يمكن تضييقه بواسطة حارس
 * Check if a type can be narrowed by a guard
 * 
 * @param type النوع الأصلي
 * @param guard الحارس
 * @return true إذا كان يمكن التضييق
 */
bool canNarrow(TypePtr type, TypeGuardPtr guard);

/**
 * دمج حارسين في حارس And
 * Combine two guards into an And guard
 * 
 * @param guard1 الحارس الأول
 * @param guard2 الحارس الثاني
 * @return حارس مدمج
 */
TypeGuardPtr combineGuards(TypeGuardPtr guard1, TypeGuardPtr guard2);

/**
 * عكس حارس (تحويل إلى Not)
 * Invert a guard (convert to Not)
 * 
 * @param guard الحارس الأصلي
 * @return حارس معكوس
 */
TypeGuardPtr invertGuard(TypeGuardPtr guard);

/**
 * تطبيق عدة حراس على نوع بالتتابع
 * Apply multiple guards to a type sequentially
 * 
 * @param type النوع الأصلي
 * @param guards قائمة الحراس
 * @return النوع بعد تطبيق جميع الحراس
 */
TypePtr applyGuards(TypePtr type, const std::vector<TypeGuardPtr>& guards);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_TYPE_GUARD_H
