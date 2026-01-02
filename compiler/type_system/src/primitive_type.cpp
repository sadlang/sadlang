// ════════════════════════════════════════════════════════════════════════════════
// ملف: primitive_type.cpp
// File: primitive_type.cpp
//
// الوصف: تنفيذ الأنواع البدائية (رقم، نص، منطقي، ...)
// Description: Implementation of primitive types (int, string, bool, ...)
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 2 يناير 2026
// Date: January 2, 2026
// ════════════════════════════════════════════════════════════════════════════════

#include "../include/primitive_type.h"  // استيراد التعريف / Import definition
#include <stdexcept>                    // لـ std::invalid_argument / For invalid_argument

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ المُنشئ / Constructor Implementation
// ════════════════════════════════════════════════════════════════════════════════

// المنشئ مع نوع محدد / Constructor with specific kind
PrimitiveType::PrimitiveType(TypeKind kind) : Type(kind) {
    // التحقق من أن النوع مسموح به / Verify that kind is allowed
    // الأنواع المسموح بها: Void, Integer, Float, Boolean, String, Any, Never, Unknown
    // Allowed types: Void, Integer, Float, Boolean, String, Any, Never, Unknown
    bool isValidType = isPrimitive() || 
                      kind == TypeKind::Void || 
                      kind == TypeKind::Any || 
                      kind == TypeKind::Never || 
                      kind == TypeKind::Unknown;
    
    if (!isValidType) {
        // رمي خطأ إذا لم يكن نوعاً مسموحاً / Throw error if not allowed type
        throw std::invalid_argument("PrimitiveType: نوع غير بدائي / non-primitive kind");
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ الدوال الافتراضية / Virtual Functions Implementation
// ════════════════════════════════════════════════════════════════════════════════

// الحصول على اسم النوع بالعربية / Get type name in Arabic
std::string PrimitiveType::getArabicName() const {
    // استخدام الدالة المساعدة / Use helper function
    return typeKindToArabic(getKind());
}

// الحصول على اسم النوع بالإنجليزية / Get type name in English
std::string PrimitiveType::getEnglishName() const {
    // استخدام الدالة المساعدة / Use helper function
    return typeKindToEnglish(getKind());
}

// تحويل النوع إلى نص تمثيلي / Convert type to string representation
std::string PrimitiveType::toString() const {
    // إرجاع الاسم العربي / Return Arabic name
    return getArabicName();
}

// التحقق من التساوي / Check equality
bool PrimitiveType::equals(const Type* other) const {
    // التحقق من null / Check for null
    if (other == nullptr) {
        return false; // null لا يساوي شيئاً / null equals nothing
    }
    
    // التحقق من نفس النوع / Check same kind
    return getKind() == other->getKind();
}

// استنساخ النوع / Clone the type
std::shared_ptr<Type> PrimitiveType::clone() const {
    // إنشاء نسخة جديدة / Create new copy
    return std::make_shared<PrimitiveType>(getKind());
}

// الحصول على حجم النوع بالبايتات / Get type size in bytes
size_t PrimitiveType::getSizeInBytes() const {
    // حساب الحجم بناء على النوع / Calculate size based on kind
    switch (getKind()) {
        case TypeKind::Void:
            return 0;  // فراغ لا حجم له / Void has no size
        
        case TypeKind::Boolean:
            return 1;  // منطقي = 1 بايت / Boolean = 1 byte
        
        case TypeKind::Integer:
            return 8;  // رقم صحيح = 8 بايت (64-bit) / Integer = 8 bytes (64-bit)
        
        case TypeKind::Float:
            return 8;  // عشري = 8 بايت (double) / Float = 8 bytes (double)
        
        case TypeKind::String:
            return sizeof(void*);  // نص = حجم مؤشر / String = pointer size
        
        // الأنواع الخاصة / Special types
        case TypeKind::Any:
        case TypeKind::Never:
        case TypeKind::Unknown:
            return sizeof(void*);  // حجم مؤشر للأنواع الخاصة / Pointer size for special types
        
        default:
            return 0;  // قيمة افتراضية / Default value
    }
}

// الحصول على محاذاة النوع / Get type alignment
size_t PrimitiveType::getAlignment() const {
    // المحاذاة عادة تساوي الحجم / Alignment usually equals size
    // للأنواع الصغيرة / For small types
    
    switch (getKind()) {
        case TypeKind::Void:
            return 1;  // فراغ: محاذاة 1 / Void: alignment 1
        
        case TypeKind::Boolean:
            return 1;  // منطقي: محاذاة 1 / Boolean: alignment 1
        
        case TypeKind::Integer:
            return 8;  // رقم: محاذاة 8 / Integer: alignment 8
        
        case TypeKind::Float:
            return 8;  // عشري: محاذاة 8 / Float: alignment 8
        
        case TypeKind::String:
            return alignof(void*);  // نص: محاذاة المؤشر / String: pointer alignment
        
        // الأنواع الخاصة / Special types
        case TypeKind::Any:
        case TypeKind::Never:
        case TypeKind::Unknown:
            return alignof(void*);  // محاذاة المؤشر للأنواع الخاصة / Pointer alignment for special types
        
        default:
            return 1;  // قيمة افتراضية / Default value
    }
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ دوال المصنع / Factory Functions Implementation
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء نوع فراغ / Create Void type
TypePtr createVoidType() {
    // إنشاء كائن PrimitiveType مع نوع Void / Create PrimitiveType object with Void kind
    return std::make_shared<PrimitiveType>(TypeKind::Void);
}

// إنشاء نوع رقم صحيح / Create Integer type
TypePtr createIntegerType() {
    // إنشاء كائن PrimitiveType مع نوع Integer / Create PrimitiveType object with Integer kind
    return std::make_shared<PrimitiveType>(TypeKind::Integer);
}

// إنشاء نوع عشري / Create Float type
TypePtr createFloatType() {
    // إنشاء كائن PrimitiveType مع نوع Float / Create PrimitiveType object with Float kind
    return std::make_shared<PrimitiveType>(TypeKind::Float);
}

// إنشاء نوع منطقي / Create Boolean type
TypePtr createBooleanType() {
    // إنشاء كائن PrimitiveType مع نوع Boolean / Create PrimitiveType object with Boolean kind
    return std::make_shared<PrimitiveType>(TypeKind::Boolean);
}

// إنشاء نوع نص / Create String type
TypePtr createStringType() {
    // إنشاء كائن PrimitiveType مع نوع String / Create PrimitiveType object with String kind
    return std::make_shared<PrimitiveType>(TypeKind::String);
}

// إنشاء نوع بدائي من TypeKind / Create primitive type from TypeKind
TypePtr createPrimitiveType(TypeKind kind) {
    // استخدام switch لاستدعاء الدالة المناسبة / Use switch to call appropriate function
    switch (kind) {
        case TypeKind::Void:
            return createVoidType();  // فراغ / Void
        
        case TypeKind::Integer:
            return createIntegerType();  // رقم / Integer
        
        case TypeKind::Float:
            return createFloatType();  // عشري / Float
        
        case TypeKind::Boolean:
            return createBooleanType();  // منطقي / Boolean
        
        case TypeKind::String:
            return createStringType();  // نص / String
        
        default:
            // رمي خطأ للأنواع غير البدائية / Throw error for non-primitive types
            throw std::invalid_argument("createPrimitiveType: نوع غير بدائي / non-primitive kind");
    }
}

} // namespace TypeSystem
} // namespace Sad
