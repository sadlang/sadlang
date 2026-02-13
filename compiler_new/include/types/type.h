// ════════════════════════════════════════════════════════════════════════════════
// ملف: type.h
// File: type.h
//
// الوصف: تعريف النوع الأساسي المجرد لنظام الأنواع
// Description: Base abstract type definition for the type system
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 2 يناير 2026
// Date: January 2, 2026
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_TYPE_H
#define SAD_COMPILER_TYPE_SYSTEM_TYPE_H

#include <memory>      // لـ shared_ptr / For shared_ptr
#include <string>      // لـ string / For string
#include <vector>      // لـ vector / For vector
#include <unordered_map> // لـ unordered_map / For unordered_map

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تعداد أنواع الأنواع / Type Kind Enumeration
// ════════════════════════════════════════════════════════════════════════════════

enum class TypeKind {
    // الأنواع البدائية / Primitive Types
    Void,          // فراغ / Void
    Integer,       // رقم صحيح / Integer
    Float,         // رقم عشري / Float
    Boolean,       // منطقي / Boolean
    String,        // نص / String
    
    // الأنواع المركبة / Composite Types
    Array,         // مصفوفة / Array
    Dictionary,    // قاموس / Dictionary
    Tuple,         // صف / Tuple
    
    // الأنواع الوظيفية / Function Types
    Function,      // دالة / Function
    
    // الأنواع الكائنية / Object-Oriented Types
    Class,         // صنف / Class
    Interface,     // واجهة / Interface
    
    // الأنواع المتقدمة / Advanced Types
    Generic,       // نوع عام / Generic Type
    TypeParameter, // معامل نوع / Type Parameter
    Union,         // اتحاد / Union Type
    Intersection,  // تقاطع / Intersection Type
    Optional,      // اختياري / Optional Type
    
    // أنواع خاصة / Special Types
    Any,           // أي / Any Type
    Never,         // أبداً / Never Type
    Unknown,       // مجهول / Unknown Type
    Error,         // خطأ / Error Type (للأخطاء الداخلية)
    
    // أنواع البرمجة غير المتزامنة / Async Programming Types
    Future,        // مستقبل / Future Type (async/await)
    Generator,     // مولّد / Generator Type (yield)
    Comprehension  // استيعاب / Comprehension Type (list/dict comprehensions)
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف الأساسي: Type
// Base Class: Type
//
// الوصف: النوع الأساسي المجرد الذي ترث منه جميع الأنواع الأخرى
// Description: Abstract base type from which all other types inherit
// ════════════════════════════════════════════════════════════════════════════════

class Type {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئ / Constructor
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ مع نوع محدد / Constructor with specific kind
    explicit Type(TypeKind kind);
    
    // المُنشئ الافتراضي محذوف / Default constructor deleted
    Type() = delete;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // المُدمّر الافتراضي / Virtual Destructor
    // ═══════════════════════════════════════════════════════════════════════════
    
    virtual ~Type() = default;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال النقية الافتراضية / Pure Virtual Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على اسم النوع بالعربية / Get type name in Arabic
    virtual std::string getArabicName() const = 0;
    
    // الحصول على اسم النوع بالإنجليزية / Get type name in English
    virtual std::string getEnglishName() const = 0;
    
    // تحويل النوع إلى نص تمثيلي / Convert type to string representation
    virtual std::string toString() const = 0;
    
    // التحقق من التساوي مع نوع آخر / Check equality with another type
    virtual bool equals(const Type* other) const = 0;
    
    // استنساخ النوع / Clone the type
    virtual std::shared_ptr<Type> clone() const = 0;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال العامة / Public Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على نوع النوع / Get the type kind
    TypeKind getKind() const { return kind_; }
    
    // التحقق من نوع معين / Check if specific kind
    bool isVoid() const { return kind_ == TypeKind::Void; }
    bool isInteger() const { return kind_ == TypeKind::Integer; }
    bool isFloat() const { return kind_ == TypeKind::Float; }
    bool isBoolean() const { return kind_ == TypeKind::Boolean; }
    bool isString() const { return kind_ == TypeKind::String; }
    bool isArray() const { return kind_ == TypeKind::Array; }
    bool isDictionary() const { return kind_ == TypeKind::Dictionary; }
    bool isTuple() const { return kind_ == TypeKind::Tuple; }
    bool isFunction() const { return kind_ == TypeKind::Function; }
    bool isClass() const { return kind_ == TypeKind::Class; }
    bool isInterface() const { return kind_ == TypeKind::Interface; }
    bool isGeneric() const { return kind_ == TypeKind::Generic; }
    bool isTypeParameter() const { return kind_ == TypeKind::TypeParameter; }
    bool isUnion() const { return kind_ == TypeKind::Union; }
    bool isIntersection() const { return kind_ == TypeKind::Intersection; }
    bool isOptional() const { return kind_ == TypeKind::Optional; }
    bool isAny() const { return kind_ == TypeKind::Any; }
    bool isNever() const { return kind_ == TypeKind::Never; }
    bool isUnknown() const { return kind_ == TypeKind::Unknown; }
    bool isError() const { return kind_ == TypeKind::Error; }
    
    // التحقق من كون النوع بدائياً / Check if primitive type
    bool isPrimitive() const {
        return kind_ == TypeKind::Integer ||
               kind_ == TypeKind::Float ||
               kind_ == TypeKind::Boolean ||
               kind_ == TypeKind::String;
    }
    
    // التحقق من كون النوع رقمياً / Check if numeric type
    bool isNumeric() const {
        return kind_ == TypeKind::Integer || kind_ == TypeKind::Float;
    }
    
    // التحقق من كون النوع مركباً / Check if composite type
    bool isComposite() const {
        return kind_ == TypeKind::Array ||
               kind_ == TypeKind::Dictionary ||
               kind_ == TypeKind::Tuple;
    }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // علاقات الأنواع / Type Relations
    // ═══════════════════════════════════════════════════════════════════════════
    
    // التحقق من إمكانية تحويل هذا النوع إلى نوع آخر / Check if this type can be converted to another
    // القيمة الافتراضية: تحقق من التساوي فقط / Default: check equality only
    virtual bool isAssignableTo(const Type* other) const;
    
    // التحقق من إمكانية استبدال هذا النوع بنوع آخر / Check if this type can substitute another
    // (Subtyping relation)
    virtual bool isSubtypeOf(const Type* other) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إدارة الحجم / Size Management
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على حجم النوع بالبايتات / Get type size in bytes
    // القيمة الافتراضية: 0 (يجب تجاوزها للأنواع المحددة) / Default: 0 (should be overridden)
    virtual size_t getSizeInBytes() const { return 0; }
    
    // الحصول على محاذاة النوع / Get type alignment
    // القيمة الافتراضية: 1 / Default: 1
    virtual size_t getAlignment() const { return 1; }
    
protected:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء المحمية / Protected Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    TypeKind kind_;  // نوع النوع / Type kind
};

// ════════════════════════════════════════════════════════════════════════════════
// Pointer Types للاستخدام المريح / Pointer Types for Convenience
// ════════════════════════════════════════════════════════════════════════════════

using TypePtr = std::shared_ptr<Type>;           // مؤشر ذكي لنوع / Smart pointer to Type
using TypeWeakPtr = std::weak_ptr<Type>;         // مؤشر ضعيف لنوع / Weak pointer to Type
using TypeList = std::vector<TypePtr>;           // قائمة من الأنواع / List of types

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة / Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

// تحويل TypeKind إلى نص عربي / Convert TypeKind to Arabic text
std::string typeKindToArabic(TypeKind kind);

// تحويل TypeKind إلى نص إنجليزي / Convert TypeKind to English text
std::string typeKindToEnglish(TypeKind kind);

// التحقق من تساوي نوعين / Check if two types are equal
bool typesEqual(const TypePtr& a, const TypePtr& b);

// التحقق من تساوي قوائم الأنواع / Check if two type lists are equal
bool typeListsEqual(const TypeList& a, const TypeList& b);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_TYPE_H
