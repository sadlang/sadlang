/**
 * @file type_nodes.h
 * @brief Advanced type annotation nodes for Sad language / عُقد تعليمات الأنواع المتقدمة للغة ص
 * @author Sad Language Team
 * @date January 3, 2026
 * 
 * هذا الملف يوسّع نظام الأنواع الحالي (Types::SadTypeKind) ليدعم:
 * This file extends the current type system (Types::SadTypeKind) to support:
 * - Generic types: مصفوفة<T> / Array<T>
 * - Union types: رقم | نص / number | string
 * - Optional types: T? / T?
 * - Function types: (T1, T2) -> T3
 * - Tuple types: (T1, T2, ...)
 * - Type aliases: type NewName = OldType
 */

#ifndef SAD_AST_TYPE_NODES_H
#define SAD_AST_TYPE_NODES_H

#include "ast_node.h"
#include "data_types.h"
#include "sad_type_system.h" // (S-TS-P2.5a) SadTypeKind — المحور الموحَّد
#include <string>
#include <vector>
#include <memory>

namespace Sad {
namespace AST {

// Forward declarations / تصريحات مسبقة
class TypeNode;
using TypeNodePtr = std::shared_ptr<TypeNode>;

/**
 * @brief نوع التعليمة / Type annotation kind
 * 
 * يحدد أنواع التعليمات المختلفة المدعومة
 * Defines the different supported type annotation kinds
 */
enum class TypeAnnotationKind {
    SIMPLE,         ///< نوع بسيط: رقم، نص / Simple type: number, string
    GENERIC,        ///< نوع عام: مصفوفة<T> / Generic type: Array<T>
    UNION,          ///< نوع اتحاد: A | B / Union type: A | B
    OPTIONAL,       ///< نوع اختياري: T? / Optional type: T?
    FUNCTION,       ///< نوع دالة: (A, B) -> C / Function type: (A, B) -> C
    TUPLE,          ///< نوع مجموعة: (A, B, C) / Tuple type: (A, B, C)
    ARRAY,          ///< نوع مصفوفة: T[] / Array type: T[]
    DICTIONARY      ///< نوع قاموس: {K: V} / Dictionary type: {K: V}
};

/**
 * @brief عُقدة الأساس لتعليمات الأنواع / Base class for type annotation nodes
 * 
 * جميع عُقد تعليمات الأنواع ترث من هذه الفئة
 * All type annotation nodes inherit from this class
 */
class TypeNode : public ASTNode {
public:
    TypeAnnotationKind kind; ///< نوع التعليمة / Annotation kind
    
    explicit TypeNode(TypeAnnotationKind k, const Lexer::Position& pos = Lexer::Position())
        : ASTNode(pos), kind(k) {}
    
    virtual ~TypeNode() = default;
    
    /**
     * @brief تحويل عُقدة النوع إلى نص / Convert type node to string
     * @return التمثيل النصي / String representation
     */
    virtual std::string toString() const override = 0;
    
    /**
     * @brief قبول الزائر / Accept visitor
     * @param visitor الزائر / The visitor
     */
    virtual void accept(ASTVisitor& visitor) override = 0;
    
    /**
     * @brief التحقق من التوافق مع نوع آخر / Check compatibility with another type
     * @param other النوع الآخر / The other type
     * @return هل متوافق؟ / Is compatible?
     */
    virtual bool isCompatibleWith(const TypeNode& other) const = 0;
};

// ============================================================================
// Simple Type Node / عُقدة النوع البسيط
// ============================================================================

/**
 * @brief عُقدة نوع بسيط / Simple type node
 * 
 * يمثل نوعاً بسيطاً مثل: رقم، نص، منطقي
 * Represents a simple type like: number, string, boolean
 * 
 * @example أمثلة / Examples:
 * - متغير عمر: رقم
 * - متغير اسم: نص
 * - var age: number
 * - var name: string
 * 
 * @note يستخدم Types::SadTypeKind الحالي كأساس
 *       Uses existing Types::SadTypeKind as base
 *       الأنواع المدعومة: INTEGER, FLOAT, STRING, BOOLEAN, ARRAY, MAP, etc.
 *       Supported types: INTEGER, FLOAT, STRING, BOOLEAN, ARRAY, MAP, etc.
 */
class SimpleTypeNode : public TypeNode {
public:
    Types::SadTypeKind kind; ///< (S-TS-P2.5a) المحور الموحَّد — نوع العقدة

    std::string typeName;    ///< اسم النوع (عربي أو إنجليزي) / Type name (Arabic or English)

    /**
     * @brief البناء من Types::SadTypeKind / Construct from Types::SadTypeKind
     */
    explicit SimpleTypeNode(Types::SadTypeKind type,
                           const Lexer::Position& pos = Lexer::Position())
        : TypeNode(TypeAnnotationKind::SIMPLE, pos),
          kind(type),
          typeName(kindToString(type)) {}

    /**
     * @brief البناء من اسم النوع / Construct from type name
     */
    explicit SimpleTypeNode(const std::string& name,
                           const Lexer::Position& pos = Lexer::Position())
        : TypeNode(TypeAnnotationKind::SIMPLE, pos),
          kind(stringToKind(name)),
          typeName(name) {}
    
    std::string toString() const override {
        return typeName;
    }
    
    void accept(ASTVisitor& visitor) override;
    
    bool isCompatibleWith(const TypeNode& other) const override;
    
private:
    /**
     * @brief تحويل Types::SadTypeKind إلى نص / Convert Types::SadTypeKind to string
     */
    static std::string kindToString(Types::SadTypeKind type);

    /**
     * @brief تحويل نص إلى Types::SadTypeKind / Convert string to Types::SadTypeKind
     */
    static Types::SadTypeKind stringToKind(const std::string& name);
};

// ============================================================================
// Generic Type Node / عُقدة النوع العام
// ============================================================================

/**
 * @brief عُقدة نوع عام (Generic) / Generic type node
 * 
 * يمثل نوعاً عاماً مثل: مصفوفة<T>، قاموس<K, V>
 * Represents a generic type like: Array<T>, Dictionary<K, V>
 * 
 * @example أمثلة / Examples:
 * - متغير أرقام: مصفوفة<رقم>
 * - متغير بيانات: قاموس<نص، رقم>
 * - var numbers: Array<number>
 * - var data: Dictionary<string, number>
 */
class GenericTypeNode : public TypeNode {
public:
    std::string baseName;                  ///< الاسم الأساسي / Base name (e.g., "مصفوفة", "Array")
    std::vector<TypeNodePtr> typeParams;   ///< معاملات النوع / Type parameters
    
    /**
     * @brief البناء / Constructor
     * @param base الاسم الأساسي / Base name
     * @param params معاملات النوع / Type parameters
     * @param pos الموقع في الكود / Position in source
     */
    GenericTypeNode(const std::string& base, 
                   std::vector<TypeNodePtr> params,
                   const Lexer::Position& pos = Lexer::Position())
        : TypeNode(TypeAnnotationKind::GENERIC, pos),
          baseName(base),
          typeParams(std::move(params)) {}
    
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override;
    bool isCompatibleWith(const TypeNode& other) const override;
};

// ============================================================================
// Union Type Node / عُقدة نوع الاتحاد
// ============================================================================

/**
 * @brief عُقدة نوع اتحاد / Union type node
 * 
 * يمثل نوع اتحاد (OR): A | B | C
 * Represents a union type (OR): A | B | C
 * 
 * @example أمثلة / Examples:
 * - متغير قيمة: رقم | نص
 * - متغير نتيجة: نجاح | خطأ
 * - var value: number | string
 * - var result: Success | Error
 */
class UnionTypeNode : public TypeNode {
public:
    std::vector<TypeNodePtr> types; ///< قائمة الأنواع / List of types
    
    /**
     * @brief البناء / Constructor
     * @param typeList قائمة الأنواع / List of types
     * @param pos الموقع في الكود / Position in source
     */
    explicit UnionTypeNode(std::vector<TypeNodePtr> typeList,
                          const Lexer::Position& pos = Lexer::Position())
        : TypeNode(TypeAnnotationKind::UNION, pos),
          types(std::move(typeList)) {}
    
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override;
    bool isCompatibleWith(const TypeNode& other) const override;
};

// ============================================================================
// Optional Type Node / عُقدة النوع الاختياري
// ============================================================================

/**
 * @brief عُقدة نوع اختياري / Optional type node
 * 
 * يمثل نوعاً اختيارياً (يمكن أن يكون null): T?
 * Represents an optional type (can be null): T?
 * 
 * @example أمثلة / Examples:
 * - متغير عمر: رقم?
 * - دالة رقم? احصل(مفتاح: نص) { }
 * - var age: number?
 * - function get(key: string): number? { }
 */
class OptionalTypeNode : public TypeNode {
public:
    TypeNodePtr baseType; ///< النوع الأساسي / Base type
    
    /**
     * @brief البناء / Constructor
     * @param base النوع الأساسي / Base type
     * @param pos الموقع في الكود / Position in source
     */
    explicit OptionalTypeNode(TypeNodePtr base,
                             const Lexer::Position& pos = Lexer::Position())
        : TypeNode(TypeAnnotationKind::OPTIONAL, pos),
          baseType(std::move(base)) {}
    
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override;
    bool isCompatibleWith(const TypeNode& other) const override;
};

// ============================================================================
// Function Type Node / عُقدة نوع الدالة
// ============================================================================

/**
 * @brief عُقدة نوع دالة / Function type node
 * 
 * يمثل نوع دالة: (T1, T2, ...) -> TReturn
 * Represents a function type: (T1, T2, ...) -> TReturn
 * 
 * @example أمثلة / Examples:
 * - متغير معالج: (رقم، نص) -> منطقي
 * - متغير تحويل: (T) -> U
 * - var handler: (number, string) -> boolean
 * - var transform: (T) -> U
 */
class FunctionTypeNode : public TypeNode {
public:
    std::vector<TypeNodePtr> paramTypes; ///< أنواع المعاملات / Parameter types
    TypeNodePtr returnType;              ///< نوع الإرجاع / Return type
    
    /**
     * @brief البناء / Constructor
     * @param params أنواع المعاملات / Parameter types
     * @param ret نوع الإرجاع / Return type
     * @param pos الموقع في الكود / Position in source
     */
    FunctionTypeNode(std::vector<TypeNodePtr> params,
                    TypeNodePtr ret,
                    const Lexer::Position& pos = Lexer::Position())
        : TypeNode(TypeAnnotationKind::FUNCTION, pos),
          paramTypes(std::move(params)),
          returnType(std::move(ret)) {}
    
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override;
    bool isCompatibleWith(const TypeNode& other) const override;
};

// ============================================================================
// Tuple Type Node / عُقدة نوع المجموعة
// ============================================================================

/**
 * @brief عُقدة نوع مجموعة / Tuple type node
 * 
 * يمثل نوع مجموعة: (T1, T2, T3, ...)
 * Represents a tuple type: (T1, T2, T3, ...)
 * 
 * @example أمثلة / Examples:
 * - متغير موقع: (رقم، رقم)
 * - متغير بيانات: (نص، رقم، منطقي)
 * - var position: (number, number)
 * - var data: (string, number, boolean)
 */
class TupleTypeNode : public TypeNode {
public:
    std::vector<TypeNodePtr> elementTypes; ///< أنواع العناصر / Element types
    
    /**
     * @brief البناء / Constructor
     * @param elements أنواع العناصر / Element types
     * @param pos الموقع في الكود / Position in source
     */
    explicit TupleTypeNode(std::vector<TypeNodePtr> elements,
                          const Lexer::Position& pos = Lexer::Position())
        : TypeNode(TypeAnnotationKind::TUPLE, pos),
          elementTypes(std::move(elements)) {}
    
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override;
    bool isCompatibleWith(const TypeNode& other) const override;
};

// ============================================================================
// Array Type Node / عُقدة نوع المصفوفة
// ============================================================================

/**
 * @brief عُقدة نوع مصفوفة / Array type node
 * 
 * يمثل نوع مصفوفة: T[]
 * Represents an array type: T[]
 * 
 * @example أمثلة / Examples:
 * - متغير أرقام: رقم[]
 * - متغير أسماء: نص[]
 * - var numbers: number[]
 * - var names: string[]
 */
class ArrayTypeNode : public TypeNode {
public:
    TypeNodePtr elementType; ///< نوع العنصر / Element type
    
    /**
     * @brief البناء / Constructor
     * @param element نوع العنصر / Element type
     * @param pos الموقع في الكود / Position in source
     */
    explicit ArrayTypeNode(TypeNodePtr element,
                          const Lexer::Position& pos = Lexer::Position())
        : TypeNode(TypeAnnotationKind::ARRAY, pos),
          elementType(std::move(element)) {}
    
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override;
    bool isCompatibleWith(const TypeNode& other) const override;
};

// ============================================================================
// Dictionary Type Node / عُقدة نوع القاموس
// ============================================================================

/**
 * @brief عُقدة نوع قاموس / Dictionary type node
 * 
 * يمثل نوع قاموس: {K: V}
 * Represents a dictionary type: {K: V}
 * 
 * @example أمثلة / Examples:
 * - متغير أعمار: {نص: رقم}
 * - متغير إعدادات: {نص: أي}
 * - var ages: {string: number}
 * - var settings: {string: any}
 */
class DictionaryTypeNode : public TypeNode {
public:
    TypeNodePtr keyType;   ///< نوع المفتاح / Key type
    TypeNodePtr valueType; ///< نوع القيمة / Value type
    
    /**
     * @brief البناء / Constructor
     * @param key نوع المفتاح / Key type
     * @param value نوع القيمة / Value type
     * @param pos الموقع في الكود / Position in source
     */
    DictionaryTypeNode(TypeNodePtr key, TypeNodePtr value,
                      const Lexer::Position& pos = Lexer::Position())
        : TypeNode(TypeAnnotationKind::DICTIONARY, pos),
          keyType(std::move(key)),
          valueType(std::move(value)) {}
    
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override;
    bool isCompatibleWith(const TypeNode& other) const override;
};

// ============================================================================
// Helper Functions / دوال مساعدة
// ============================================================================

/**
 * @brief إنشاء عُقدة نوع بسيط / Create a simple type node
 */
TypeNodePtr makeSimpleType(Types::SadTypeKind type, const Lexer::Position& pos = Lexer::Position());

/**
 * @brief إنشاء عُقدة نوع بسيط من اسم / Create a simple type node from name
 */
TypeNodePtr makeSimpleType(const std::string& typeName, const Lexer::Position& pos = Lexer::Position());

/**
 * @brief إنشاء عُقدة نوع عام / Create a generic type node
 */
TypeNodePtr makeGenericType(const std::string& baseName, 
                           std::vector<TypeNodePtr> params,
                           const Lexer::Position& pos = Lexer::Position());

/**
 * @brief إنشاء عُقدة نوع اتحاد / Create a union type node
 */
TypeNodePtr makeUnionType(std::vector<TypeNodePtr> types,
                         const Lexer::Position& pos = Lexer::Position());

/**
 * @brief إنشاء عُقدة نوع اختياري / Create an optional type node
 */
TypeNodePtr makeOptionalType(TypeNodePtr baseType,
                            const Lexer::Position& pos = Lexer::Position());

/**
 * @brief إنشاء عُقدة نوع دالة / Create a function type node
 */
TypeNodePtr makeFunctionType(std::vector<TypeNodePtr> paramTypes,
                            TypeNodePtr returnType,
                            const Lexer::Position& pos = Lexer::Position());

/**
 * @brief إنشاء عُقدة نوع مصفوفة / Create an array type node
 */
TypeNodePtr makeArrayType(TypeNodePtr elementType,
                         const Lexer::Position& pos = Lexer::Position());

/**
 * @brief إنشاء عُقدة نوع قاموس / Create a dictionary type node
 */
TypeNodePtr makeDictionaryType(TypeNodePtr keyType, TypeNodePtr valueType,
                              const Lexer::Position& pos = Lexer::Position());

} // namespace AST
} // namespace Sad

#endif // SAD_AST_TYPE_NODES_H
