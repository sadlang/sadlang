// ════════════════════════════════════════════════════════════════════════════════
// ملف: substitution.h
// File: substitution.h
//
// الوصف: خريطة استبدال متغيرات الأنواع (Type Variable Substitution Map)
// Description: Type variable substitution map for type inference
//
// المؤلف: Sad Language Type System - Type Inference Engine
// Author: Sad Language Type System - Type Inference Engine
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// الغرض: تخزين وإدارة استبدالات متغيرات الأنواع أثناء Type Inference
// Purpose: Store and manage type variable substitutions during type inference
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_SUBSTITUTION_H
#define SAD_COMPILER_TYPE_SYSTEM_SUBSTITUTION_H

#include "type.h"              // استيراد النوع الأساسي / Import base Type (type.h:1-218)
#include <unordered_map>       // لـ خريطة الاستبدال / For substitution map
#include <string>              // لـ أسماء المتغيرات / For variable names
#include <memory>              // لـ shared_ptr / For shared_ptr

namespace Sad {
namespace TypeSystem {

// Forward declaration / تصريح مُسبق
class TypeVariable;  // سيتم تعريفه في type_variable.h / Will be defined in type_variable.h

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: Substitution
// Class: Substitution
//
// الوصف: خريطة استبدال تربط متغيرات الأنواع بأنواعها الملموسة
// Description: Substitution map linking type variables to their concrete types
//
// مثال / Example:
//   T1 -> Integer
//   T2 -> String
//   T3 -> Array<T1>  (يُحل تدريجياً إلى Array<Integer>)
// ════════════════════════════════════════════════════════════════════════════════

class Substitution {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ افتراضي - خريطة فارغة / Default constructor - empty map
    Substitution();
    
    // منشئ نسخ / Copy constructor
    Substitution(const Substitution& other);
    
    // منشئ نقل / Move constructor
    Substitution(Substitution&& other) noexcept;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // معاملات التعيين / Assignment Operators
    // ═══════════════════════════════════════════════════════════════════════════
    
    // معامل تعيين النسخ / Copy assignment operator
    Substitution& operator=(const Substitution& other);
    
    // معامل تعيين النقل / Move assignment operator
    Substitution& operator=(Substitution&& other) noexcept;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إدارة الاستبدالات / Substitution Management
    // ═══════════════════════════════════════════════════════════════════════════
    
    // إضافة استبدال: varName -> type
    // Add substitution: varName -> type
    // المعامل / Parameter: varName - اسم متغير النوع / Type variable name
    // المعامل / Parameter: type - النوع المُستبدل / Substituted type (from type.h:type.h:75)
    void add(const std::string& varName, TypePtr type);
    
    // البحث عن استبدال لمتغير / Look up substitution for a variable
    // الإرجاع / Return: النوع المُستبدل أو nullptr إذا لم يُوجد / Substituted type or nullptr if not found
    TypePtr lookup(const std::string& varName) const;
    
    // التحقق من وجود استبدال / Check if substitution exists
    // الإرجاع / Return: true إذا كان المتغير موجود في الخريطة / true if variable is in map
    bool contains(const std::string& varName) const;
    
    // إزالة استبدال / Remove a substitution
    void remove(const std::string& varName);
    
    // مسح جميع الاستبدالات / Clear all substitutions
    void clear();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التطبيق / Application
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تطبيق الاستبدالات على نوع / Apply substitutions to a type
    // يستبدل جميع متغيرات الأنواع الموجودة في النوع / Replaces all type variables in the type
    // المعامل / Parameter: type - النوع المُراد استبداله / Type to substitute
    // الإرجاع / Return: النوع بعد الاستبدال / Type after substitution
    TypePtr apply(TypePtr type) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدمج / Composition
    // ═══════════════════════════════════════════════════════════════════════════
    
    // دمج استبدال آخر مع هذا الاستبدال / Compose with another substitution
    // s1.compose(s2) يُنتج استبدال حيث s2 يُطبق أولاً ثم s1
    // s1.compose(s2) produces a substitution where s2 is applied first, then s1
    // المعامل / Parameter: other - الاستبدال الآخر / Other substitution
    // الإرجاع / Return: استبدال جديد مدموج / New composed substitution
    Substitution compose(const Substitution& other) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الاستعلام / Query
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على عدد الاستبدالات / Get number of substitutions
    size_t size() const;
    
    // التحقق من كون الخريطة فارغة / Check if map is empty
    bool isEmpty() const;
    
    // الحصول على جميع أسماء المتغيرات / Get all variable names
    std::vector<std::string> getVariableNames() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التحويل إلى نص / String Conversion
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تحويل الاستبدال إلى نص تمثيلي / Convert substitution to string
    // مثال / Example: "{T1 -> Integer, T2 -> String}"
    std::string toString() const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // التكرار / Iteration
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الوصول إلى الخريطة الداخلية (للتكرار) / Access internal map (for iteration)
    // يُستخدم بحذر - للقراءة فقط / Use with care - read-only
    const std::unordered_map<std::string, TypePtr>& getMap() const;
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // خريطة الاستبدالات: اسم متغير النوع -> النوع المُستبدل
    // Substitution map: type variable name -> substituted type
    // المفتاح / Key: std::string (اسم متغير النوع / Type variable name)
    // القيمة / Value: TypePtr (النوع المُستبدل / Substituted type from type.h:214)
    std::unordered_map<std::string, TypePtr> substitutions_;
};

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء استبدال فارغ / Create empty substitution
Substitution createEmptySubstitution();

// إنشاء استبدال من زوج واحد / Create substitution from single pair
// المعامل / Parameter: varName - اسم متغير النوع / Type variable name
// المعامل / Parameter: type - النوع المُستبدل / Substituted type
// الإرجاع / Return: استبدال يحتوي على زوج واحد / Substitution with single pair
Substitution createSingletonSubstitution(const std::string& varName, TypePtr type);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_SUBSTITUTION_H
