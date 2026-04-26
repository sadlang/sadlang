// ════════════════════════════════════════════════════════════════════════════════
// ملف: substitution.cpp
// File: substitution.cpp
//
// الوصف: تنفيذ خريطة استبدال متغيرات الأنواع
// Description: Implementation of type variable substitution map
//
// المؤلف: Sad Language Type System - Type Inference Engine
// Author: Sad Language Type System - Type Inference Engine
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
// ════════════════════════════════════════════════════════════════════════════════

// ═════════════════════════════════════════════════════════════════════════════
// Header المصدر / Source Header
// مصدر التعريفات: substitution.h (substitution.h:1-end)
// Source of definitions: substitution.h
// ═════════════════════════════════════════════════════════════════════════════
#include "substitution.h"

// ═════════════════════════════════════════════════════════════════════════════
// Headers إضافية / Additional Headers
// ═════════════════════════════════════════════════════════════════════════════
#include "type_variable.h"   // لـ TypeVariable (type_variable.h:1-end)
#include <sstream>           // لـ ostringstream / For ostringstream
#include <algorithm>         // لـ std::find / For std::find

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ الصنف: Substitution
// Class Implementation: Substitution
// المصدر / Source: substitution.h:47-180
// ════════════════════════════════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════════════════════════
// المُنشئات / Constructors
// ═══════════════════════════════════════════════════════════════════════════

// المُنشئ الافتراضي - خريطة فارغة / Default constructor - empty map
// التعريف / Definition: substitution.h:54
Substitution::Substitution() {
    // لا يوجد عمل إضافي - substitutions_ يُهيأ تلقائياً فارغاً
    // No additional work - substitutions_ is initialized empty automatically
}

// منشئ النسخ / Copy constructor
// التعريف / Definition: substitution.h:57
Substitution::Substitution(const Substitution& other)
    : substitutions_(other.substitutions_) {
    // نسخ الخريطة بالكامل / Copy the entire map
}

// منشئ النقل / Move constructor
// التعريف / Definition: substitution.h:60
Substitution::Substitution(Substitution&& other) noexcept
    : substitutions_(std::move(other.substitutions_)) {
    // نقل الخريطة بكفاءة / Move the map efficiently
}

// ═══════════════════════════════════════════════════════════════════════════
// معاملات التعيين / Assignment Operators
// ═══════════════════════════════════════════════════════════════════════════

// معامل تعيين النسخ / Copy assignment operator
// التعريف / Definition: substitution.h:68
Substitution& Substitution::operator=(const Substitution& other) {
    // التحقق من التعيين الذاتي / Check for self-assignment
    if (this != &other) {
        // نسخ الخريطة / Copy the map
        substitutions_ = other.substitutions_;
    }
    return *this;  // إرجاع مرجع لهذا الكائن / Return reference to this object
}

// معامل تعيين النقل / Move assignment operator
// التعريف / Definition: substitution.h:71
Substitution& Substitution::operator=(Substitution&& other) noexcept {
    // التحقق من التعيين الذاتي / Check for self-assignment
    if (this != &other) {
        // نقل الخريطة / Move the map
        substitutions_ = std::move(other.substitutions_);
    }
    return *this;  // إرجاع مرجع لهذا الكائن / Return reference to this object
}

// ═══════════════════════════════════════════════════════════════════════════
// إدارة الاستبدالات / Substitution Management
// ═══════════════════════════════════════════════════════════════════════════

// إضافة استبدال: varName -> type
// Add substitution: varName -> type
// التعريف / Definition: substitution.h:82
// المتغيرات المستخدمة / Used variables: substitutions_ (substitution.h:176)
void Substitution::add(const std::string& varName, TypePtr type) {
    // إضافة أو تحديث الاستبدال في الخريطة / Add or update substitution in map
    substitutions_[varName] = type;
}

// البحث عن استبدال لمتغير / Look up substitution for a variable
// التعريف / Definition: substitution.h:86
// المتغيرات المستخدمة / Used variables: substitutions_ (substitution.h:176)
TypePtr Substitution::lookup(const std::string& varName) const {
    // البحث في الخريطة / Search in map
    auto it = substitutions_.find(varName);
    
    // إذا وُجد، إرجاع النوع / If found, return type
    if (it != substitutions_.end()) {
        return it->second;
    }
    
    // إذا لم يُوجد، إرجاع nullptr / If not found, return nullptr
    return nullptr;
}

// التحقق من وجود استبدال / Check if substitution exists
// التعريف / Definition: substitution.h:90
// المتغيرات المستخدمة / Used variables: substitutions_ (substitution.h:176)
bool Substitution::contains(const std::string& varName) const {
    // استخدام find للتحقق من الوجود / Use find to check existence
    return substitutions_.find(varName) != substitutions_.end();
}

// إزالة استبدال / Remove a substitution
// التعريف / Definition: substitution.h:94
// المتغيرات المستخدمة / Used variables: substitutions_ (substitution.h:176)
void Substitution::remove(const std::string& varName) {
    // إزالة من الخريطة / Remove from map
    substitutions_.erase(varName);
}

// مسح جميع الاستبدالات / Clear all substitutions
// التعريف / Definition: substitution.h:97
// المتغيرات المستخدمة / Used variables: substitutions_ (substitution.h:176)
void Substitution::clear() {
    // مسح الخريطة بالكامل / Clear the entire map
    substitutions_.clear();
}

// ═══════════════════════════════════════════════════════════════════════════
// التطبيق / Application
// ═══════════════════════════════════════════════════════════════════════════

// تطبيق الاستبدالات على نوع / Apply substitutions to a type
// التعريف / Definition: substitution.h:108
TypePtr Substitution::apply(TypePtr type) const {
    // التحقق من nullptr / Check for nullptr
    if (!type) {
        return nullptr;
    }
    
    // إذا كان النوع متغير نوع / If type is a type variable
    if (isTypeVariable(type)) {
        // الحصول على TypeVariable / Get TypeVariable
        TypeVariable* typeVar = asTypeVariable(type);
        if (typeVar) {
            // البحث عن استبدال لهذا المتغير / Look up substitution for this variable
            TypePtr substituted = lookup(typeVar->getName());
            if (substituted) {
                // تطبيق بشكل متكرر لحل جميع الاستبدالات المتسلسلة
                // Apply recursively to resolve all chained substitutions
                return apply(substituted);
            }
        }
        // إذا لم يُوجد استبدال، إرجاع النوع الأصلي / If no substitution, return original
        return type;
    }
    
    // للأنواع الأخرى (غير متغيرات الأنواع)، إرجاع النوع نفسه
    // For other types (non-type-variables), return the type itself
    // في التنفيذ الكامل، نحتاج لتطبيق على الأنواع المركبة أيضاً
    // In full implementation, we need to apply to composite types too
    return type;
}

// ═══════════════════════════════════════════════════════════════════════════
// الدمج / Composition
// ═══════════════════════════════════════════════════════════════════════════

// دمج استبدال آخر مع هذا الاستبدال / Compose with another substitution
// التعريف / Definition: substitution.h:120
// s1.compose(s2) = s2 ثم s1 / s1.compose(s2) = apply s2 then s1
Substitution Substitution::compose(const Substitution& other) const {
    // إنشاء استبدال جديد / Create new substitution
    Substitution result;
    
    // أولاً: تطبيق other على جميع استبدالات هذا الكائن
    // First: apply other to all substitutions of this object
    for (const auto& [varName, type] : substitutions_) {
        // تطبيق other.apply على النوع / Apply other.apply to type
        result.add(varName, other.apply(type));
    }
    
    // ثانياً: إضافة جميع استبدالات other التي لا توجد في result
    // Second: add all substitutions from other that are not in result
    for (const auto& [varName, type] : other.substitutions_) {
        if (!result.contains(varName)) {
            result.add(varName, type);
        }
    }
    
    return result;  // إرجاع الاستبدال المدموج / Return composed substitution
}

// ═══════════════════════════════════════════════════════════════════════════
// الاستعلام / Query
// ═══════════════════════════════════════════════════════════════════════════

// الحصول على عدد الاستبدالات / Get number of substitutions
// التعريف / Definition: substitution.h:129
// المتغيرات المستخدمة / Used variables: substitutions_ (substitution.h:176)
size_t Substitution::size() const {
    return substitutions_.size();
}

// التحقق من كون الخريطة فارغة / Check if map is empty
// التعريف / Definition: substitution.h:132
// المتغيرات المستخدمة / Used variables: substitutions_ (substitution.h:176)
bool Substitution::isEmpty() const {
    return substitutions_.empty();
}

// الحصول على جميع أسماء المتغيرات / Get all variable names
// التعريف / Definition: substitution.h:135
// المتغيرات المستخدمة / Used variables: substitutions_ (substitution.h:176)
std::vector<std::string> Substitution::getVariableNames() const {
    // إنشاء vector لحفظ الأسماء / Create vector to store names
    std::vector<std::string> names;
    names.reserve(substitutions_.size());  // حجز المساحة / Reserve space
    
    // تكرار على جميع العناصر / Iterate over all elements
    for (const auto& [varName, type] : substitutions_) {
        names.push_back(varName);  // إضافة الاسم / Add name
    }
    
    return names;  // إرجاع القائمة / Return list
}

// ═══════════════════════════════════════════════════════════════════════════
// التحويل إلى نص / String Conversion
// ═══════════════════════════════════════════════════════════════════════════

// تحويل الاستبدال إلى نص تمثيلي / Convert substitution to string
// التعريف / Definition: substitution.h:143
// المتغيرات المستخدمة / Used variables: substitutions_ (substitution.h:176)
std::string Substitution::toString() const {
    // إذا كانت الخريطة فارغة / If map is empty
    if (substitutions_.empty()) {
        return "{}";  // إرجاع خريطة فارغة / Return empty map
    }
    
    // بناء النص / Build string
    std::ostringstream oss;
    oss << "{";  // البداية / Start
    
    // التكرار على جميع الاستبدالات / Iterate over all substitutions
    bool first = true;
    for (const auto& [varName, type] : substitutions_) {
        if (!first) {
            oss << ", ";  // فاصلة بين العناصر / Comma between elements
        }
        first = false;
        
        // إضافة الاستبدال / Add substitution
        oss << varName << " -> ";
        if (type) {
            oss << type->toString();  // تحويل النوع إلى نص / Convert type to string (type.h:101)
        } else {
            oss << "null";
        }
    }
    
    oss << "}";  // النهاية / End
    return oss.str();  // إرجاع النص / Return string
}

// ═══════════════════════════════════════════════════════════════════════════
// التكرار / Iteration
// ═══════════════════════════════════════════════════════════════════════════

// الوصول إلى الخريطة الداخلية / Access internal map
// التعريف / Definition: substitution.h:151
// المتغيرات المستخدمة / Used variables: substitutions_ (substitution.h:176)
const std::unordered_map<std::string, TypePtr>& Substitution::getMap() const {
    return substitutions_;  // إرجاع مرجع إلى الخريطة / Return reference to map
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// المصدر / Source: substitution.h:158-177
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء استبدال فارغ / Create empty substitution
// التعريف / Definition: substitution.h:161
Substitution createEmptySubstitution() {
    // إنشاء وإرجاع استبدال فارغ / Create and return empty substitution
    return Substitution();
}

// إنشاء استبدال من زوج واحد / Create substitution from single pair
// التعريف / Definition: substitution.h:167
Substitution createSingletonSubstitution(const std::string& varName, TypePtr type) {
    // إنشاء استبدال فارغ / Create empty substitution
    Substitution subst;
    
    // إضافة الزوج الواحد / Add single pair
    subst.add(varName, type);
    
    return subst;  // إرجاع الاستبدال / Return substitution
}

} // namespace TypeSystem
} // namespace Sad
