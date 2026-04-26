// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_variable.cpp
// File: type_variable.cpp
//
// الوصف: تنفيذ متغيرات الأنواع (Type Variables)
// Description: Implementation of type variables
//
// المؤلف: Sad Language Type System - Type Inference Engine
// Author: Sad Language Type System - Type Inference Engine
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
// ════════════════════════════════════════════════════════════════════════════════

#include "type_variable.h"   // المصدر / Source (type_variable.h:1-end)
#include <sstream>           // لـ ostringstream / For ostringstream

namespace Sad {
namespace TypeSystem {

// ═══════════════════════════════════════════════════════════════════════════
// Static Member Initialization / تهيئة العضو الثابت
// المصدر / Source: type_variable.h:124
// ═══════════════════════════════════════════════════════════════════════════
std::atomic<int> TypeVariable::nextId_(0);  // بداية من 0 / Starting from 0

// ═══════════════════════════════════════════════════════════════════════════
// المُنشئات / Constructors
// ═══════════════════════════════════════════════════════════════════════════

// منشئ مع اسم محدد / Constructor with specific name
// التعريف / Definition: type_variable.h:48
TypeVariable::TypeVariable(const std::string& name)
    : Type(SadTypeKind::TypeParameter),  // استدعاء منشئ الأب / Call parent constructor (type.h:88)
      name_(name),                     // تعيين الاسم / Set name
      id_(generateUniqueId()),         // توليد معرف فريد / Generate unique ID
      isBound_(false)                  // غير مُرتبط في البداية / Not bound initially
{
}

// منشئ يُولد اسم فريد تلقائياً / Constructor generating unique name
// التعريف / Definition: type_variable.h:53
TypeVariable::TypeVariable()
    : Type(SadTypeKind::TypeParameter),  // استدعاء منشئ الأب / Call parent constructor (type.h:88)
      name_(generateUniqueName()),     // توليد اسم فريد / Generate unique name
      id_(generateUniqueId()),         // توليد معرف فريد / Generate unique ID
      isBound_(false)                  // غير مُرتبط في البداية / Not bound initially
{
}

// ═══════════════════════════════════════════════════════════════════════════
// تجاوز الدوال الافتراضية / Override Virtual Functions
// من الصنف الأساسي Type (type.h:93-107)
// ═══════════════════════════════════════════════════════════════════════════

// الحصول على اسم النوع بالعربية / Get type name in Arabic
// التعريف / Definition: type_variable.h:65
std::string TypeVariable::getArabicName() const {
    return name_;  // إرجاع الاسم / Return name (type_variable.h:114)
}

// الحصول على اسم النوع بالإنجليزية / Get type name in English
// التعريف / Definition: type_variable.h:70
std::string TypeVariable::getEnglishName() const {
    return name_;  // إرجاع الاسم / Return name (type_variable.h:114)
}

// تحويل النوع إلى نص تمثيلي / Convert type to string representation
// التعريف / Definition: type_variable.h:75
std::string TypeVariable::toString() const {
    // تنسيق: 'T0 أو TypeVar(T0)
    // Format: 'T0 or TypeVar(T0)
    std::ostringstream oss;
    oss << "'" << name_;  // name_ من (type_variable.h:114)
    return oss.str();
}

// التحقق من التساوي مع نوع آخر / Check equality with another type
// التعريف / Definition: type_variable.h:81
bool TypeVariable::equals(const Type* other) const {
    // التحقق من nullptr / Check for nullptr
    if (!other) {
        return false;
    }
    
    // التحقق من كون النوع الآخر متغير نوع / Check if other is type variable
    if (!other->isTypeParameter()) {  // isTypeParameter من (type.h:143)
        return false;
    }
    
    // تحويل إلى TypeVariable / Cast to TypeVariable
    const TypeVariable* otherVar = static_cast<const TypeVariable*>(other);
    
    // المقارنة بالمُعرف (أدق من الاسم) / Compare by ID (more precise than name)
    return id_ == otherVar->id_;  // id_ من (type_variable.h:118)
}

// استنساخ النوع / Clone the type
// التعريف / Definition: type_variable.h:85
std::shared_ptr<Type> TypeVariable::clone() const {
    // إنشاء نسخة جديدة بنفس الاسم / Create new copy with same name
    return std::make_shared<TypeVariable>(name_);  // name_ من (type_variable.h:114)
}

// ═══════════════════════════════════════════════════════════════════════════
// علاقات الأنواع / Type Relations
// ═══════════════════════════════════════════════════════════════════════════

// التحقق من إمكانية تحويل هذا النوع إلى نوع آخر / Check assignability
// التعريف / Definition: type_variable.h:123
bool TypeVariable::isAssignableTo(const Type* other) const {
    // متغير النوع يمكن تعيينه لأي نوع / Type variable can be assigned to any type
    (void)other;  // تجنب تحذير عدم الاستخدام / Avoid unused warning
    return true;
}

// التحقق من إمكانية استبدال هذا النوع بنوع آخر / Check subtyping
// التعريف / Definition: type_variable.h:129
bool TypeVariable::isSubtypeOf(const Type* other) const {
    // متغير النوع هو نوع فرعي فقط لنفسه أو لمتغير نوع آخر
    // Type variable is subtype only of itself or another type variable
    if (!other) {
        return false;
    }
    
    // إذا كان نفس متغير النوع / If same type variable
    return equals(other);
}

// ═══════════════════════════════════════════════════════════════════════════
// دوال مساعدة خاصة / Private Helper Functions
// ═══════════════════════════════════════════════════════════════════════════

// توليد اسم فريد تلقائياً / Generate unique name automatically
// التعريف / Definition: type_variable.h:138
std::string TypeVariable::generateUniqueName() {
    // توليد اسم مثل "T0", "T1", "T2", ...
    // Generate name like "T0", "T1", "T2", ...
    int id = nextId_.fetch_add(1);  // nextId_ من (type_variable.h:124)
    std::ostringstream oss;
    oss << "T" << id;
    return oss.str();
}

// توليد مُعرف فريد / Generate unique ID
// التعريف / Definition: type_variable.h:142
int TypeVariable::generateUniqueId() {
    // استخدام نفس العداد للمُعرف / Use same counter for ID
    return nextId_.fetch_add(1);  // nextId_ من (type_variable.h:124)
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مصنع / Factory Functions
// المصدر / Source: type_variable.h:148-164
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء متغير نوع باسم محدد / Create type variable with specific name
// التعريف / Definition: type_variable.h:151
TypePtr createTypeVariable(const std::string& name) {
    return std::make_shared<TypeVariable>(name);
}

// إنشاء متغير نوع باسم فريد تلقائي / Create type variable with unique auto-generated name
// التعريف / Definition: type_variable.h:155
TypePtr createFreshTypeVariable() {
    return std::make_shared<TypeVariable>();  // يستخدم المُنشئ الافتراضي / Uses default constructor
}

// إنشاء مجموعة من متغيرات الأنواع / Create a list of type variables
// التعريف / Definition: type_variable.h:160
std::vector<TypePtr> createTypeVariables(int count) {
    std::vector<TypePtr> vars;
    vars.reserve(count);  // حجز المساحة / Reserve space
    
    for (int i = 0; i < count; ++i) {
        vars.push_back(createFreshTypeVariable());  // إنشاء متغير جديد / Create new variable
    }
    
    return vars;
}

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// المصدر / Source: type_variable.h:170-178
// ════════════════════════════════════════════════════════════════════════════════

// التحقق من كون نوع هو متغير نوع / Check if type is a type variable
// التعريف / Definition: type_variable.h:173
bool isTypeVariable(const TypePtr& type) {
    return type && type->isTypeParameter();  // isTypeParameter من (type.h:143)
}

// الحصول على TypeVariable من TypePtr / Get TypeVariable from TypePtr
// التعريف / Definition: type_variable.h:178
TypeVariable* asTypeVariable(const TypePtr& type) {
    if (!isTypeVariable(type)) {
        return nullptr;
    }
    return static_cast<TypeVariable*>(type.get());  // تحويل آمن / Safe cast
}

// الحصول على TypeVariable من TypePtr (const version)
// التعريف / Definition: type_variable.h:182
const TypeVariable* asTypeVariable(const TypePtr& type, bool constVersion) {
    (void)constVersion;  // تجنب تحذير عدم الاستخدام / Avoid unused warning
    if (!isTypeVariable(type)) {
        return nullptr;
    }
    return static_cast<const TypeVariable*>(type.get());  // تحويل آمن / Safe cast
}

} // namespace TypeSystem
} // namespace Sad
