// ════════════════════════════════════════════════════════════════════════════════
// ملف: type.cpp
// File: type.cpp
//
// الوصف: تنفيذ النوع الأساسي المجرد لنظام الأنواع
// Description: Implementation of the base abstract type for the type system
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 2 يناير 2026
// Date: January 2, 2026
// ════════════════════════════════════════════════════════════════════════════════

#include "types/type.h"  // استيراد تعريف الصنف / Import class definition
#include <algorithm>          // لـ std::equal / For std::equal

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ المُنشئ / Constructor Implementation
// ════════════════════════════════════════════════════════════════════════════════

// المنشئ مع نوع محدد / Constructor with specific kind
Type::Type(SadTypeKind kind) : kind_(kind) {
    // تهيئة نوع النوع / Initialize type kind
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ علاقات الأنواع / Type Relations Implementation
// ════════════════════════════════════════════════════════════════════════════════

// التحقق من إمكانية تحويل هذا النوع إلى نوع آخر / Check if assignable to another type
bool Type::isAssignableTo(const Type* other) const {
    // التحقق من null pointer / Check for null pointer
    if (other == nullptr) {
        return false; // لا يمكن التحويل إلى nullptr / Cannot assign to nullptr
    }
    
    // التحقق من التساوي المباشر / Check direct equality
    if (equals(other)) {
        return true; // نفس النوع / Same type
    }
    
    // Any يقبل أي نوع / Any accepts any type
    if (other->isAny()) {
        return true; // يمكن تحويل أي شيء إلى Any / Can assign anything to Any
    }
    
    // لا يمكن تحويل شيء إلى Never / Nothing can be assigned to Never
    if (other->isNever()) {
        return false; // Never لا يقبل أي قيمة / Never accepts no values
    }
    
    // Never يمكن تحويله إلى أي شيء / Never can be assigned to anything
    if (isNever()) {
        return true; // Never subtype لكل الأنواع / Never is subtype of all types
    }
    
    // Unknown يمكن تحويله فقط إلى Any / Unknown can only be assigned to Any
    if (isUnknown()) {
        return other->isAny(); // Unknown -> Any فقط / Unknown -> Any only
    }
    
    // التحقق من الأرقام: Integer يمكن تحويله إلى Float / Numeric: Integer -> Float
    if (isInteger() && other->isFloat()) {
        return true; // رقم صحيح يمكن تحويله إلى عشري / Integer can be assigned to float
    }
    
    // في الحالات الأخرى، استخدم subtyping / For other cases, use subtyping
    return isSubtypeOf(other);
}

// التحقق من إمكانية استبدال هذا النوع بنوع آخر / Check if this is a subtype of another
bool Type::isSubtypeOf(const Type* other) const {
    // التحقق من null pointer / Check for null pointer
    if (other == nullptr) {
        return false; // لا يمكن أن يكون subtype من nullptr / Cannot be subtype of nullptr
    }
    
    // التحقق من التساوي / Check equality
    if (equals(other)) {
        return true; // نوع هو subtype من نفسه / Type is subtype of itself
    }
    
    // Never subtype لكل الأنواع / Never is subtype of all types
    if (isNever()) {
        return true; // Never في أسفل hierarchy / Never at bottom of hierarchy
    }
    
    // كل الأنواع subtype من Any / All types are subtypes of Any
    if (other->isAny()) {
        return true; // Any في أعلى hierarchy / Any at top of hierarchy
    }
    
    // القيمة الافتراضية: false / Default: false
    // الأنواع المحددة تجاوز هذه الدالة للقواعد الخاصة / Specific types override for custom rules
    return false;
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ الدوال المساعدة / Helper Functions Implementation
// ════════════════════════════════════════════════════════════════════════════════

// تحويل SadTypeKind إلى نص عربي / Convert SadTypeKind to Arabic text
std::string typeKindToArabic(SadTypeKind kind) {
    // استخدام switch للتحويل / Use switch for conversion
    switch (kind) {
        // الأنواع البدائية / Primitive Types
        case SadTypeKind::Void:
            return "فراغ"; // Void
        case SadTypeKind::Integer:
            return "رقم"; // Integer
        case SadTypeKind::Float:
            return "عشري"; // Float
        case SadTypeKind::Boolean:
            return "منطقي"; // Boolean
        case SadTypeKind::String:
            return "نص"; // String
        
        // الأنواع المركبة / Composite Types
        case SadTypeKind::Array:
            return "مصفوفة"; // Array
        case SadTypeKind::Map:
            return "قاموس"; // Dictionary
        case SadTypeKind::Tuple:
            return "صف"; // Tuple
        
        // الأنواع الوظيفية / Function Types
        case SadTypeKind::Function:
            return "دالة"; // Function
        
        // الأنواع الكائنية / Object-Oriented Types
        case SadTypeKind::Class:
            return "صنف"; // Class
        case SadTypeKind::Trait:
            return "واجهة"; // Interface
        
        // الأنواع المتقدمة / Advanced Types
        case SadTypeKind::Generic:
            return "نوع_عام"; // Generic
        case SadTypeKind::TypeParameter:
            return "معامل_نوع"; // Type Parameter
        case SadTypeKind::Union:
            return "اتحاد"; // Union
        case SadTypeKind::Intersection:
            return "تقاطع"; // Intersection
        case SadTypeKind::Optional:
            return "اختياري"; // Optional
        
        // أنواع خاصة / Special Types
        case SadTypeKind::Any:
            return "أي"; // Any
        case SadTypeKind::Never:
            return "أبداً"; // Never
        case SadTypeKind::Unknown:
            return "مجهول"; // Unknown
        case SadTypeKind::Error:
            return "خطأ"; // Error
        
        // قيمة افتراضية (لا ينبغي الوصول) / Default (should not reach)
        default:
            return "نوع_غير_معروف"; // Unknown type
    }
}

// تحويل SadTypeKind إلى نص إنجليزي / Convert SadTypeKind to English text
std::string typeKindToEnglish(SadTypeKind kind) {
    // استخدام switch للتحويل / Use switch for conversion
    switch (kind) {
        // الأنواع البدائية / Primitive Types
        case SadTypeKind::Void:
            return "Void";
        case SadTypeKind::Integer:
            return "Integer";
        case SadTypeKind::Float:
            return "Float";
        case SadTypeKind::Boolean:
            return "Boolean";
        case SadTypeKind::String:
            return "String";
        
        // الأنواع المركبة / Composite Types
        case SadTypeKind::Array:
            return "Array";
        case SadTypeKind::Map:
            return "Dictionary";
        case SadTypeKind::Tuple:
            return "Tuple";
        
        // الأنواع الوظيفية / Function Types
        case SadTypeKind::Function:
            return "Function";
        
        // الأنواع الكائنية / Object-Oriented Types
        case SadTypeKind::Class:
            return "Class";
        case SadTypeKind::Trait:
            return "Interface";
        
        // الأنواع المتقدمة / Advanced Types
        case SadTypeKind::Generic:
            return "Generic";
        case SadTypeKind::TypeParameter:
            return "TypeParameter";
        case SadTypeKind::Union:
            return "Union";
        case SadTypeKind::Intersection:
            return "Intersection";
        case SadTypeKind::Optional:
            return "Optional";
        
        // أنواع خاصة / Special Types
        case SadTypeKind::Any:
            return "Any";
        case SadTypeKind::Never:
            return "Never";
        case SadTypeKind::Unknown:
            return "Unknown";
        case SadTypeKind::Error:
            return "Error";
        
        // قيمة افتراضية / Default
        default:
            return "UnknownType";
    }
}

// التحقق من تساوي نوعين / Check if two types are equal
bool typesEqual(const TypePtr& a, const TypePtr& b) {
    // التحقق من null pointers / Check for null pointers
    if (!a && !b) {
        return true; // كلاهما null / Both null
    }
    
    if (!a || !b) {
        return false; // واحد null والآخر ليس كذلك / One is null, other is not
    }
    
    // استخدام دالة equals / Use equals method
    return a->equals(b.get());
}

// التحقق من تساوي قوائم الأنواع / Check if two type lists are equal
bool typeListsEqual(const TypeList& a, const TypeList& b) {
    // التحقق من الأحجام أولاً / Check sizes first
    if (a.size() != b.size()) {
        return false; // أحجام مختلفة / Different sizes
    }
    
    // المقارنة عنصر بعنصر / Compare element by element
    for (size_t i = 0; i < a.size(); ++i) {
        // استخدام typesEqual للمقارنة / Use typesEqual for comparison
        if (!typesEqual(a[i], b[i])) {
            return false; // عنصر مختلف / Different element
        }
    }
    
    // جميع العناصر متساوية / All elements equal
    return true;
}

} // namespace TypeSystem
} // namespace Sad

