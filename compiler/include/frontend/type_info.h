// ======================================================================
// type_info.h - معلومات نوع البيانات / Type Information System
//              Type Info Classes for SIR Type System
// ======================================================================
// الوصف بالعربية:
//   نظام معلومات الأنواع الشامل لـ SIR:
//   - TypeInfo: فئة أساسية لمعلومات النوع
//   - PrimitiveType: الأنواع البسيطة (i64, f64, bool, etc.)
//   - PointerType: الأنواع المؤشرة
//   - ArrayType: أنواع المصفوفات
//   - StructType: أنواع البنى (الكائنات)
//   - FunctionType: أنواع الدوال
//
// English Description:
//   Comprehensive type information system for SIR:
//   - TypeInfo: Base class for type information
//   - PrimitiveType: Primitive types (i64, f64, bool, etc.)
//   - PointerType: Pointer types
//   - ArrayType: Array types
//   - StructType: Structure types (objects)
//   - FunctionType: Function types
//
// الإصدار / Version: 1.0
// التاريخ / Date: December 16, 2025
// المرحلة / Phase: 1 - Frontend
// ======================================================================

#pragma once

#include "sir_types.h"
#include <string>
#include <memory>
#include <vector>
#include <cstdint>
#include <map>

namespace Sad {
namespace Compiler {
namespace SIR {

// ======================================================================
// Forward Declarations
// ======================================================================

class TypeInfo;
using TypePtr = std::shared_ptr<TypeInfo>;

// ======================================================================
// TypeInfo - Base Class for Type Information
// ======================================================================

/**
 * @brief (AR) فئة أساسية لمعلومات النوع
 * @brief (EN) Base class for type information
 * 
 * توفر:
 * Provides:
 * - النوع الأساسي / Base type
 * - تحويل إلى نص / String conversion
 * - حجم ومحاذاة / Size and alignment
 * - مقارنة الأنواع / Type comparison
 */
class TypeInfo {
public:
    virtual ~TypeInfo() = default;
    
    // ==========================================
    // الواجهات الأساسية / Core Interfaces
    // ==========================================
    
    /**
     * @brief (AR) الحصول على النوع الأساسي
     * @brief (EN) Get base type
     */
    virtual SadTypeKind getBaseType() const = 0;
    
    /**
     * @brief (AR) تحويل النوع إلى نص
     * @brief (EN) Convert type to string
     */
    virtual std::string toString() const = 0;
    
    /**
     * @brief (AR) الحصول على حجم النوع بالبايت
     * @brief (EN) Get type size in bytes
     */
    virtual size_t getSize() const = 0;
    
    /**
     * @brief (AR) الحصول على محاذاة النوع
     * @brief (EN) Get type alignment
     */
    virtual size_t getAlignment() const = 0;
    
    /**
     * @brief (AR) التحقق من تساوي النوعين
     * @brief (EN) Check if types are equal
     */
    virtual bool equals(const TypePtr& other) const = 0;
    
    /**
     * @brief (AR) نسخ النوع
     * @brief (EN) Clone the type
     */
    virtual TypePtr clone() const = 0;
};

// ======================================================================
// PrimitiveType - Primitive Data Types
// ======================================================================

/**
 * @brief (AR) أنواع البيانات البسيطة
 * @brief (EN) Primitive data types
 * 
 * يشمل:
 * Includes:
 * - VOID: فارغ / void
 * - I64: رقم صحيح 64-bit / 64-bit integer
 * - F64: رقم عشري 64-bit / 64-bit floating point
 * - BOOL: منطقي / boolean
 * - STRING: نص / string
 */
class PrimitiveType : public TypeInfo {
private:
    SadTypeKind type;
    
public:
    explicit PrimitiveType(SadTypeKind t) : type(t) {}
    
    SadTypeKind getBaseType() const override { return type; }
    std::string toString() const override;
    size_t getSize() const override;
    size_t getAlignment() const override;
    bool equals(const TypePtr& other) const override;
    TypePtr clone() const override;
};

// ======================================================================
// PointerType - Pointer Types
// ======================================================================

/**
 * @brief (AR) نوع المؤشر
 * @brief (EN) Pointer type
 * 
 * يشير إلى نوع آخر
 * Points to another type
 */
class PointerType : public TypeInfo {
private:
    TypePtr pointeeType;
    
public:
    explicit PointerType(TypePtr pt) : pointeeType(pt) {}
    
    SadTypeKind getBaseType() const override { return SadTypeKind::Pointer; }
    std::string toString() const override;
    size_t getSize() const override;
    size_t getAlignment() const override;
    bool equals(const TypePtr& other) const override;
    TypePtr clone() const override;
    
    TypePtr getPointeeType() const { return pointeeType; }
};

// ======================================================================
// ArrayType - Array Types
// ======================================================================

/**
 * @brief (AR) نوع المصفوفة
 * @brief (EN) Array type
 * 
 * مصفوفة ذات حجم محدد من نوع معين
 * Fixed-size array of specific type
 */
class ArrayType : public TypeInfo {
private:
    TypePtr elementType;
    size_t arraySize;
    
public:
    ArrayType(TypePtr et, size_t sz) : elementType(et), arraySize(sz) {}
    
    SadTypeKind getBaseType() const override { return SadTypeKind::Array; }
    std::string toString() const override;
    size_t getSize() const override;
    size_t getAlignment() const override;
    bool equals(const TypePtr& other) const override;
    TypePtr clone() const override;
    
    TypePtr getElementType() const { return elementType; }
    size_t getArraySize() const { return arraySize; }
};

// ======================================================================
// StructType - Structure Types
// ======================================================================

/**
 * @brief (AR) نوع البنية/الكائن
 * @brief (EN) Structure/Object type
 * 
 * تجميع من الحقول بأنواع مختلفة
 * Collection of fields with different types
 */
class StructType : public TypeInfo {
private:
    std::string structName;
    std::vector<TypePtr> fieldTypes;
    std::map<std::string, size_t> fieldOffsets;
    size_t totalSize = 0;
    
    void calculateOffsets();
    
public:
    StructType(const std::string& name, const std::vector<TypePtr>& fields)
        : structName(name), fieldTypes(fields) {
        calculateOffsets();
    }
    
    SadTypeKind getBaseType() const override { return SadTypeKind::Struct; }
    std::string toString() const override;
    size_t getSize() const override { return totalSize; }
    size_t getAlignment() const override;
    bool equals(const TypePtr& other) const override;
    TypePtr clone() const override;
    
    const std::string& getStructName() const { return structName; }
    const std::vector<TypePtr>& getFieldTypes() const { return fieldTypes; }
    size_t getFieldOffset(size_t index) const;
};

// ======================================================================
// FunctionType - Function Types
// ======================================================================

/**
 * @brief (AR) نوع الدالة
 * @brief (EN) Function type
 * 
 * يحتوي على نوع الإرجاع والمعاملات
 * Contains return type and parameter types
 */
class FunctionType : public TypeInfo {
private:
    TypePtr returnType;
    std::vector<TypePtr> parameterTypes;
    
public:
    FunctionType(TypePtr rt, const std::vector<TypePtr>& pt)
        : returnType(rt), parameterTypes(pt) {}
    
    SadTypeKind getBaseType() const override { return SadTypeKind::Function; }
    std::string toString() const override;
    size_t getSize() const override;
    size_t getAlignment() const override;
    bool equals(const TypePtr& other) const override;
    TypePtr clone() const override;
    
    TypePtr getReturnType() const { return returnType; }
    const std::vector<TypePtr>& getParameterTypes() const { return parameterTypes; }
    size_t getParameterCount() const { return parameterTypes.size(); }
};

// ======================================================================
// Factory Functions - نوابع المصنع
// ======================================================================

/**
 * @brief (AR) إنشاء نوع i64
 * @brief (EN) Create i64 type
 */
inline TypePtr makeI64Type() {
    return std::make_shared<PrimitiveType>(SadTypeKind::Integer);
}

/**
 * @brief (AR) إنشاء نوع f64
 * @brief (EN) Create f64 type
 */
inline TypePtr makeF64Type() {
    return std::make_shared<PrimitiveType>(SadTypeKind::Float);
}

/**
 * @brief (AR) إنشاء نوع bool
 * @brief (EN) Create bool type
 */
inline TypePtr makeBoolType() {
    return std::make_shared<PrimitiveType>(SadTypeKind::Boolean);
}

/**
 * @brief (AR) إنشاء نوع void
 * @brief (EN) Create void type
 */
inline TypePtr makeVoidType() {
    return std::make_shared<PrimitiveType>(SadTypeKind::Void);
}

/**
 * @brief (AR) إنشاء نوع string
 * @brief (EN) Create string type
 */
inline TypePtr makeStringType() {
    return std::make_shared<PrimitiveType>(SadTypeKind::String);
}

/**
 * @brief (AR) إنشاء نوع مؤشر
 * @brief (EN) Create pointer type
 */
inline TypePtr makePtrType(TypePtr pointeeType) {
    return std::make_shared<PointerType>(pointeeType);
}

/**
 * @brief (AR) إنشاء نوع مصفوفة
 * @brief (EN) Create array type
 */
inline TypePtr makeArrayType(TypePtr elementType, size_t size) {
    return std::make_shared<ArrayType>(elementType, size);
}

/**
 * @brief (AR) إنشاء نوع بنية
 * @brief (EN) Create struct type
 */
inline TypePtr makeStructType(const std::string& name, const std::vector<TypePtr>& fields) {
    return std::make_shared<StructType>(name, fields);
}

/**
 * @brief (AR) إنشاء نوع دالة
 * @brief (EN) Create function type
 */
inline TypePtr makeFunctionType(TypePtr returnType, const std::vector<TypePtr>& paramTypes) {
    return std::make_shared<FunctionType>(returnType, paramTypes);
}

// ======================================================================
// Helper Functions
// ======================================================================

/**
 * @brief (AR) تحويل سلسلة نصية إلى نوع SIR
 * @brief (EN) Convert string to SadTypeKind
 */
SadTypeKind stringToSIRType(const std::string& str);

/**
 * @brief (AR) مقارنة نوعين
 * @brief (EN) Check if two types are compatible
 */
bool areTypesCompatible(const TypePtr& type1, const TypePtr& type2);

} // namespace SIR
} // namespace Compiler
} // namespace Sad
