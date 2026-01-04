// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_variable.h
// File: type_variable.h
//
// الوصف: تعريف متغيرات الأنواع (Type Variables) لاستنتاج الأنواع
// Description: Type variable definition for type inference
//
// المؤلف: Sad Language Type System - Type Inference Engine
// Author: Sad Language Type System - Type Inference Engine
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// الغرض: متغيرات الأنواع تُستخدم في Hindley-Milner type inference
// Purpose: Type variables are used in Hindley-Milner type inference
//
// مثال / Example:
//   دالة هوية(س) { إرجاع س }
//   Function identity(x) { return x }
//   => identity :: forall T. T -> T
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_TYPE_VARIABLE_H
#define SAD_COMPILER_TYPE_SYSTEM_TYPE_VARIABLE_H

#include "type.h"              // استيراد النوع الأساسي / Import base Type (type.h:1-218)
#include <string>              // لـ اسم المتغير / For variable name
#include <atomic>              // لـ مُولد الأسماء الفريدة / For unique name generator

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: TypeVariable
// Class: TypeVariable
//
// الوصف: متغير نوع يُمثل نوعاً غير محدد بعد
// Description: Type variable representing an undetermined type
//
// ملاحظة: TypeVariable يرث من Type (type.h:75)
// Note: TypeVariable inherits from Type (type.h:75)
//
// الاستخدام / Usage:
//   1. استنتاج الأنواع التلقائي / Automatic type inference
//   2. أنواع Generic / Generic types
//   3. Type Constraints / قيود الأنواع
// ════════════════════════════════════════════════════════════════════════════════

class TypeVariable : public Type {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ مع اسم محدد / Constructor with specific name
    // المعامل / Parameter: name - اسم متغير النوع (مثل "T", "T1", "a")
    explicit TypeVariable(const std::string& name);
    
    // منشئ يُولد اسم فريد تلقائياً / Constructor generating unique name
    // يُنشئ اسم مثل "T0", "T1", "T2", ...
    // Generates name like "T0", "T1", "T2", ...
    TypeVariable();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تجاوز الدوال الافتراضية / Override Virtual Functions
    // من الصنف الأساسي Type (type.h:75-218)
    // From base class Type (type.h:75-218)
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على اسم النوع بالعربية / Get type name in Arabic
    // الإرجاع / Return: اسم المتغير (مثل "T1")
    std::string getArabicName() const override;
    
    // الحصول على اسم النوع بالإنجليزية / Get type name in English
    // الإرجاع / Return: اسم المتغير (مثل "T1")
    std::string getEnglishName() const override;
    
    // تحويل النوع إلى نص تمثيلي / Convert type to string representation
    // الإرجاع / Return: تمثيل نصي (مثل "'T1" أو "TypeVar(T1)")
    std::string toString() const override;
    
    // التحقق من التساوي مع نوع آخر / Check equality with another type
    // المعامل / Parameter: other - النوع الآخر / Other type
    // الإرجاع / Return: true إذا كان نفس متغير النوع / true if same type variable
    bool equals(const Type* other) const override;
    
    // استنساخ النوع / Clone the type
    // الإرجاع / Return: نسخة جديدة من متغير النوع / New copy of type variable
    std::shared_ptr<Type> clone() const override;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال الخاصة / Specific Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على اسم المتغير / Get variable name
    // الإرجاع / Return: اسم المتغير (name_)
    const std::string& getName() const { return name_; }
    
    // تعيين اسم جديد / Set new name
    // المعامل / Parameter: newName - الاسم الجديد / New name
    void setName(const std::string& newName) { name_ = newName; }
    
    // التحقق من كون المتغير مُرتبط (bound) / Check if variable is bound
    // متغير مُرتبط = له استبدال في Substitution map
    // Bound variable = has substitution in Substitution map
    // الإرجاع / Return: true إذا كان مُرتبط / true if bound
    bool isBound() const { return isBound_; }
    
    // تعيين حالة الارتباط / Set bound state
    // المعامل / Parameter: bound - الحالة الجديدة / New state
    void setBound(bool bound) { isBound_ = bound; }
    
    // الحصول على مُعرف فريد / Get unique ID
    // الإرجاع / Return: مُعرف رقمي فريد / Unique numeric ID
    int getId() const { return id_; }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // علاقات الأنواع / Type Relations
    // ═══════════════════════════════════════════════════════════════════════════
    
    // التحقق من إمكانية تحويل هذا النوع إلى نوع آخر / Check assignability
    // متغير النوع يمكن تعيينه لأي نوع / Type variable can be assigned to any type
    // المعامل / Parameter: other - النوع الآخر / Other type
    // الإرجاع / Return: true دائماً (متغير النوع مرن) / true always (type variable is flexible)
    bool isAssignableTo(const Type* other) const override;
    
    // التحقق من إمكانية استبدال هذا النوع بنوع آخر / Check subtyping
    // المعامل / Parameter: other - النوع الآخر / Other type
    // الإرجاع / Return: true إذا كان نفس متغير النوع / true if same type variable
    bool isSubtypeOf(const Type* other) const override;
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // اسم متغير النوع / Type variable name
    // مثال / Example: "T", "T1", "a", "b", ...
    std::string name_;
    
    // مُعرف فريد لهذا المتغير / Unique ID for this variable
    // يُستخدم للتمييز بين متغيرات بنفس الاسم / Used to distinguish variables with same name
    int id_;
    
    // هل المتغير مُرتبط بنوع ملموس؟ / Is variable bound to concrete type?
    bool isBound_;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // مُولد الأسماء الفريدة / Unique Name Generator
    // ═══════════════════════════════════════════════════════════════════════════
    
    // عداد للأسماء الفريدة / Counter for unique names
    // static لضمان التفرد عبر جميع المتغيرات / static to ensure uniqueness across all variables
    // atomic لـ thread safety / atomic for thread safety
    static std::atomic<int> nextId_;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال مساعدة خاصة / Private Helper Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // توليد اسم فريد تلقائياً / Generate unique name automatically
    // الإرجاع / Return: اسم مثل "T0", "T1", "T2", ...
    static std::string generateUniqueName();
    
    // توليد مُعرف فريد / Generate unique ID
    // الإرجاع / Return: مُعرف رقمي فريد / Unique numeric ID
    static int generateUniqueId();
};

// ════════════════════════════════════════════════════════════════════════════════
// دوال مصنع / Factory Functions
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء متغير نوع باسم محدد / Create type variable with specific name
// المعامل / Parameter: name - اسم المتغير / Variable name
// الإرجاع / Return: TypePtr يشير إلى TypeVariable / TypePtr pointing to TypeVariable (type.h:214)
TypePtr createTypeVariable(const std::string& name);

// إنشاء متغير نوع باسم فريد تلقائي / Create type variable with unique auto-generated name
// الإرجاع / Return: TypePtr يشير إلى TypeVariable / TypePtr pointing to TypeVariable
TypePtr createFreshTypeVariable();

// إنشاء مجموعة من متغيرات الأنواع / Create a list of type variables
// المعامل / Parameter: count - عدد المتغيرات / Number of variables
// الإرجاع / Return: قائمة من متغيرات الأنواع / List of type variables
std::vector<TypePtr> createTypeVariables(int count);

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

// التحقق من كون نوع هو متغير نوع / Check if type is a type variable
// المعامل / Parameter: type - النوع المُراد فحصه / Type to check
// الإرجاع / Return: true إذا كان متغير نوع / true if type variable
bool isTypeVariable(const TypePtr& type);

// الحصول على TypeVariable من TypePtr / Get TypeVariable from TypePtr
// المعامل / Parameter: type - النوع / Type
// الإرجاع / Return: مؤشر إلى TypeVariable أو nullptr / Pointer to TypeVariable or nullptr
TypeVariable* asTypeVariable(const TypePtr& type);

// الحصول على TypeVariable من TypePtr (const version)
const TypeVariable* asTypeVariable(const TypePtr& type, bool constVersion);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_TYPE_VARIABLE_H
