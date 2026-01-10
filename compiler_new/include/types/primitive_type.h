// ════════════════════════════════════════════════════════════════════════════════
// ملف: primitive_type.h
// File: primitive_type.h
//
// الوصف: تعريف الأنواع البدائية (رقم، نص، منطقي، ...)
// Description: Primitive type definitions (int, string, bool, ...)
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 2 يناير 2026
// Date: January 2, 2026
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_PRIMITIVE_TYPE_H
#define SAD_COMPILER_TYPE_SYSTEM_PRIMITIVE_TYPE_H

#include "type.h"  // استيراد النوع الأساسي / Import base Type

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: PrimitiveType
// Class: PrimitiveType
//
// الوصف: يمثل الأنواع البدائية الأساسية
// Description: Represents basic primitive types
// ════════════════════════════════════════════════════════════════════════════════

class PrimitiveType : public Type {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئ / Constructor
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ مع نوع محدد / Constructor with specific kind
    explicit PrimitiveType(TypeKind kind);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تجاوز الدوال الافتراضية / Override Virtual Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على اسم النوع بالعربية / Get type name in Arabic
    std::string getArabicName() const override;
    
    // الحصول على اسم النوع بالإنجليزية / Get type name in English
    std::string getEnglishName() const override;
    
    // تحويل النوع إلى نص تمثيلي / Convert type to string representation
    std::string toString() const override;
    
    // التحقق من التساوي / Check equality
    bool equals(const Type* other) const override;
    
    // استنساخ النوع / Clone the type
    std::shared_ptr<Type> clone() const override;
    
    // الحصول على حجم النوع بالبايتات / Get type size in bytes
    size_t getSizeInBytes() const override;
    
    // الحصول على محاذاة النوع / Get type alignment
    size_t getAlignment() const override;
};

// ════════════════════════════════════════════════════════════════════════════════
// دوال مصنع للأنواع البدائية / Factory Functions for Primitive Types
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء نوع فراغ / Create Void type
TypePtr createVoidType();

// إنشاء نوع رقم صحيح / Create Integer type
TypePtr createIntegerType();

// إنشاء نوع عشري / Create Float type
TypePtr createFloatType();

// إنشاء نوع منطقي / Create Boolean type
TypePtr createBooleanType();

// إنشاء نوع نص / Create String type
TypePtr createStringType();

// إنشاء نوع بدائي من TypeKind / Create primitive type from TypeKind
TypePtr createPrimitiveType(TypeKind kind);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_PRIMITIVE_TYPE_H
