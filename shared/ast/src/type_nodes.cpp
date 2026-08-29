/**
 * @file type_nodes.cpp
 * @brief Implementation of advanced type annotation nodes / تنفيذ عُقد تعليمات الأنواع المتقدمة
 * @author Sad Language Team
 * @date January 3, 2026
 */

#include "type_nodes.h"
#include "ast_visitor.h"
#include <sstream>
#include <algorithm>

namespace Sad {
namespace AST {

// ============================================================================
// SimpleTypeNode Implementation / تنفيذ عُقدة النوع البسيط
// ============================================================================

void SimpleTypeNode::accept(ASTVisitor& visitor) {
    // سيتم تنفيذ visitSimpleType في ASTVisitor لاحقاً
    // visitSimpleType will be implemented in ASTVisitor later
    // visitor.visitSimpleType(*this);
}

bool SimpleTypeNode::isCompatibleWith(const TypeNode& other) const {
    // التحقق من نوع العُقدة الأخرى / Check other node kind
    if (other.kind != TypeAnnotationKind::SIMPLE) {
        // نوع بسيط متوافق مع Union إذا كان أحد عناصره
        // Simple type is compatible with Union if it's one of its elements
        if (other.kind == TypeAnnotationKind::UNION) {
            const auto& unionNode = static_cast<const UnionTypeNode&>(other);
            for (const auto& type : unionNode.types) {
                if (this->isCompatibleWith(*type)) {
                    return true;
                }
            }
            return false;
        }
        // نوع بسيط متوافق مع Optional إذا كان متوافقاً مع النوع الأساسي
        // Simple type is compatible with Optional if compatible with base type
        if (other.kind == TypeAnnotationKind::OPTIONAL) {
            const auto& optionalNode = static_cast<const OptionalTypeNode&>(other);
            return this->isCompatibleWith(*optionalNode.baseType);
        }
        return false;
    }
    
    // مقارنة الأنواع البسيطة / Compare simple types
    const auto& otherSimple = static_cast<const SimpleTypeNode&>(other);
    
    // تطابق تام / Exact match — (S-TS-P2.5a) المحور SadTypeKind
    if (kind == otherSimple.kind) {
        return true;
    }

    // قواعد التحويل الضمني / Implicit conversion rules
    // رقم صحيح يمكن تحويله إلى رقم عشري / Integer can be converted to float
    if (kind == Types::SadTypeKind::Integer &&
        otherSimple.kind == Types::SadTypeKind::Float) {
        return true;
    }
    
    return false;
}

std::string SimpleTypeNode::kindToString(Types::SadTypeKind type) {
    // تحويل Types::SadTypeKind إلى نص / Convert Types::SadTypeKind to string
    switch (type) {
        case Types::SadTypeKind::Void:      return "فراغ";     // void/none
        case Types::SadTypeKind::Integer:   return "رقم";      // number (int)
        case Types::SadTypeKind::Float:     return "عشري";     // decimal/float
        case Types::SadTypeKind::Boolean:   return "منطقي";    // boolean
        case Types::SadTypeKind::String:    return "نص";       // string
        case Types::SadTypeKind::Array:     return "مصفوفة";   // array
        case Types::SadTypeKind::Map:       return "قاموس";    // dictionary
        case Types::SadTypeKind::Function:  return "دالة";     // function
        case Types::SadTypeKind::Class:    return "كائن";     // object (class instance)
        case Types::SadTypeKind::Tuple:     return "ثنائي";    // tuple
        case Types::SadTypeKind::Enum:      return "تعداد";    // enum
        case Types::SadTypeKind::UInt8:      return "بايت";     // byte
        case Types::SadTypeKind::Error:     return "خطأ";      // error
        case Types::SadTypeKind::Unknown:   return "مجهول";    // unknown
        default:                        return "غير_معروف"; // undefined
    }
}

Types::SadTypeKind SimpleTypeNode::stringToKind(const std::string& name) {
    // تحويل نص إلى Types::SadTypeKind / Convert string to Types::SadTypeKind
    // دعم العربية والإنجليزية / Support Arabic and English
    
    // العربية / Arabic
    if (name == "فراغ" || name == "لاشيء" || name == "void" || name == "none")
        return Types::SadTypeKind::Void;
    if (name == "رقم" || name == "صحيح" || name == "integer" || name == "int") 
        return Types::SadTypeKind::Integer;
    if (name == "عشري" || name == "مضاعف" || name == "float" || name == "double") 
        return Types::SadTypeKind::Float;
    if (name == "منطقي" || name == "boolean" || name == "bool") 
        return Types::SadTypeKind::Boolean;
    if (name == "نص" || name == "string")           
        return Types::SadTypeKind::String;
    if (name == "مصفوفة" || name == "array")        
        return Types::SadTypeKind::Array;
    if (name == "قاموس" || name == "map" || name == "dictionary" || name == "dict") 
        return Types::SadTypeKind::Map;
    if (name == "دالة" || name == "function")       
        return Types::SadTypeKind::Function;
    if (name == "كائن" || name == "صنف" || name == "object" || name == "class")
        return Types::SadTypeKind::Class;
    if (name == "ثنائي" || name == "ثلاثي" || name == "tuple")
        return Types::SadTypeKind::Tuple;
    if (name == "تعداد" || name == "enum")
        return Types::SadTypeKind::Enum;
    if (name == "بايت" || name == "byte")
        return Types::SadTypeKind::UInt8;
    if (name == "خطأ" || name == "error")
        return Types::SadTypeKind::Error;
    
    // افتراضي: مجهول / Default: unknown
    return Types::SadTypeKind::Unknown;
}

// ============================================================================
// GenericTypeNode Implementation / تنفيذ عُقدة النوع العام
// ============================================================================

std::string GenericTypeNode::toString() const {
    // بناء النص: اسم<نوع1، نوع2، ...>
    // Build string: name<type1, type2, ...>
    std::ostringstream oss;
    oss << baseName << "<";
    
    // إضافة معاملات النوع / Add type parameters
    for (size_t i = 0; i < typeParams.size(); ++i) {
        if (i > 0) oss << "، "; // فاصلة عربية / Arabic comma
        oss << typeParams[i]->toString();
    }
    
    oss << ">";
    return oss.str();
}

void GenericTypeNode::accept(ASTVisitor& visitor) {
    // سيتم تنفيذ visitGenericType في ASTVisitor لاحقاً
    // visitGenericType will be implemented in ASTVisitor later
    // visitor.visitGenericType(*this);
}

bool GenericTypeNode::isCompatibleWith(const TypeNode& other) const {
    // التحقق من نوع العُقدة الأخرى / Check other node kind
    if (other.kind != TypeAnnotationKind::GENERIC) {
        return false;
    }
    
    const auto& otherGeneric = static_cast<const GenericTypeNode&>(other);
    
    // يجب أن يكون الاسم الأساسي متطابقاً / Base name must match
    if (baseName != otherGeneric.baseName) {
        return false;
    }
    
    // يجب أن يكون عدد معاملات النوع متطابقاً / Number of type params must match
    if (typeParams.size() != otherGeneric.typeParams.size()) {
        return false;
    }
    
    // جميع معاملات النوع يجب أن تكون متوافقة / All type params must be compatible
    for (size_t i = 0; i < typeParams.size(); ++i) {
        if (!typeParams[i]->isCompatibleWith(*otherGeneric.typeParams[i])) {
            return false;
        }
    }
    
    return true;
}

// ============================================================================
// UnionTypeNode Implementation / تنفيذ عُقدة نوع الاتحاد
// ============================================================================

std::string UnionTypeNode::toString() const {
    // بناء النص: نوع1 | نوع2 | ...
    // Build string: type1 | type2 | ...
    std::ostringstream oss;
    
    for (size_t i = 0; i < types.size(); ++i) {
        if (i > 0) oss << " | ";
        oss << types[i]->toString();
    }
    
    return oss.str();
}

void UnionTypeNode::accept(ASTVisitor& visitor) {
    // سيتم تنفيذ visitUnionType في ASTVisitor لاحقاً
    // visitUnionType will be implemented in ASTVisitor later
    // visitor.visitUnionType(*this);
}

bool UnionTypeNode::isCompatibleWith(const TypeNode& other) const {
    // نوع الاتحاد متوافق إذا كان أي من عناصره متوافقاً
    // Union type is compatible if any of its elements is compatible
    for (const auto& type : types) {
        if (type->isCompatibleWith(other)) {
            return true;
        }
    }
    
    return false;
}

// ============================================================================
// OptionalTypeNode Implementation / تنفيذ عُقدة النوع الاختياري
// ============================================================================

std::string OptionalTypeNode::toString() const {
    // بناء النص: نوع?
    // Build string: type?
    return baseType->toString() + "?";
}

void OptionalTypeNode::accept(ASTVisitor& visitor) {
    // سيتم تنفيذ visitOptionalType في ASTVisitor لاحقاً
    // visitOptionalType will be implemented in ASTVisitor later
    // visitor.visitOptionalType(*this);
}

bool OptionalTypeNode::isCompatibleWith(const TypeNode& other) const {
    // نوع اختياري متوافق مع نوع آخر إذا كان النوع الأساسي متوافقاً
    // Optional type is compatible if base type is compatible
    if (other.kind == TypeAnnotationKind::OPTIONAL) {
        const auto& otherOptional = static_cast<const OptionalTypeNode&>(other);
        return baseType->isCompatibleWith(*otherOptional.baseType);
    }
    
    // نوع اختياري يمكن أن يتلقى null دائماً
    // Optional type can always receive null
    // (سيتم التحقق من null في Type Checker)
    // (null check will be done in Type Checker)
    
    return baseType->isCompatibleWith(other);
}

// ============================================================================
// FunctionTypeNode Implementation / تنفيذ عُقدة نوع الدالة
// ============================================================================

std::string FunctionTypeNode::toString() const {
    // بناء النص: (نوع1، نوع2) -> نوع_الإرجاع
    // Build string: (type1, type2) -> returnType
    std::ostringstream oss;
    oss << "(";
    
    // إضافة أنواع المعاملات / Add parameter types
    for (size_t i = 0; i < paramTypes.size(); ++i) {
        if (i > 0) oss << "، "; // فاصلة عربية / Arabic comma
        oss << paramTypes[i]->toString();
    }
    
    oss << ") -> " << returnType->toString();
    return oss.str();
}

void FunctionTypeNode::accept(ASTVisitor& visitor) {
    // سيتم تنفيذ visitFunctionType في ASTVisitor لاحقاً
    // visitFunctionType will be implemented in ASTVisitor later
    // visitor.visitFunctionType(*this);
}

bool FunctionTypeNode::isCompatibleWith(const TypeNode& other) const {
    // التحقق من نوع العُقدة الأخرى / Check other node kind
    if (other.kind != TypeAnnotationKind::FUNCTION) {
        return false;
    }
    
    const auto& otherFunc = static_cast<const FunctionTypeNode&>(other);
    
    // يجب أن يكون عدد المعاملات متطابقاً / Number of params must match
    if (paramTypes.size() != otherFunc.paramTypes.size()) {
        return false;
    }
    
    // جميع أنواع المعاملات يجب أن تكون متوافقة / All param types must be compatible
    // (ملاحظة: Contravariance للمعاملات، لكن نبسّط هنا)
    // (Note: Contravariance for params, but we simplify here)
    for (size_t i = 0; i < paramTypes.size(); ++i) {
        if (!paramTypes[i]->isCompatibleWith(*otherFunc.paramTypes[i])) {
            return false;
        }
    }
    
    // نوع الإرجاع يجب أن يكون متوافقاً / Return type must be compatible
    // (Covariance لنوع الإرجاع)
    // (Covariance for return type)
    if (!returnType->isCompatibleWith(*otherFunc.returnType)) {
        return false;
    }
    
    return true;
}

// ============================================================================
// TupleTypeNode Implementation / تنفيذ عُقدة نوع المجموعة
// ============================================================================

std::string TupleTypeNode::toString() const {
    // بناء النص: (نوع1، نوع2، نوع3)
    // Build string: (type1, type2, type3)
    std::ostringstream oss;
    oss << "(";
    
    // إضافة أنواع العناصر / Add element types
    for (size_t i = 0; i < elementTypes.size(); ++i) {
        if (i > 0) oss << "، "; // فاصلة عربية / Arabic comma
        oss << elementTypes[i]->toString();
    }
    
    oss << ")";
    return oss.str();
}

void TupleTypeNode::accept(ASTVisitor& visitor) {
    // سيتم تنفيذ visitTupleType في ASTVisitor لاحقاً
    // visitTupleType will be implemented in ASTVisitor later
    // visitor.visitTupleType(*this);
}

bool TupleTypeNode::isCompatibleWith(const TypeNode& other) const {
    // التحقق من نوع العُقدة الأخرى / Check other node kind
    if (other.kind != TypeAnnotationKind::TUPLE) {
        return false;
    }
    
    const auto& otherTuple = static_cast<const TupleTypeNode&>(other);
    
    // يجب أن يكون عدد العناصر متطابقاً / Number of elements must match
    if (elementTypes.size() != otherTuple.elementTypes.size()) {
        return false;
    }
    
    // جميع أنواع العناصر يجب أن تكون متوافقة / All element types must be compatible
    for (size_t i = 0; i < elementTypes.size(); ++i) {
        if (!elementTypes[i]->isCompatibleWith(*otherTuple.elementTypes[i])) {
            return false;
        }
    }
    
    return true;
}

// ============================================================================
// ArrayTypeNode Implementation / تنفيذ عُقدة نوع المصفوفة
// ============================================================================

std::string ArrayTypeNode::toString() const {
    // بناء النص: نوع[]
    // Build string: type[]
    return elementType->toString() + "[]";
}

void ArrayTypeNode::accept(ASTVisitor& visitor) {
    // سيتم تنفيذ visitArrayType في ASTVisitor لاحقاً
    // visitArrayType will be implemented in ASTVisitor later
    // visitor.visitArrayType(*this);
}

bool ArrayTypeNode::isCompatibleWith(const TypeNode& other) const {
    // التحقق من نوع العُقدة الأخرى / Check other node kind
    if (other.kind != TypeAnnotationKind::ARRAY) {
        // مصفوفة متوافقة مع Generic<مصفوفة، T> إذا كان T متوافقاً
        // Array is compatible with Generic<Array, T> if T is compatible
        if (other.kind == TypeAnnotationKind::GENERIC) {
            const auto& otherGeneric = static_cast<const GenericTypeNode&>(other);
            if ((otherGeneric.baseName == "مصفوفة" || otherGeneric.baseName == "Array") &&
                otherGeneric.typeParams.size() == 1) {
                return elementType->isCompatibleWith(*otherGeneric.typeParams[0]);
            }
        }
        return false;
    }
    
    const auto& otherArray = static_cast<const ArrayTypeNode&>(other);
    
    // نوع العنصر يجب أن يكون متوافقاً / Element type must be compatible
    return elementType->isCompatibleWith(*otherArray.elementType);
}

// ============================================================================
// DictionaryTypeNode Implementation / تنفيذ عُقدة نوع القاموس
// ============================================================================

std::string DictionaryTypeNode::toString() const {
    // بناء النص: {نوع_المفتاح: نوع_القيمة}
    // Build string: {keyType: valueType}
    std::ostringstream oss;
    oss << "{" << keyType->toString() << ": " << valueType->toString() << "}";
    return oss.str();
}

void DictionaryTypeNode::accept(ASTVisitor& visitor) {
    // سيتم تنفيذ visitDictionaryType في ASTVisitor لاحقاً
    // visitDictionaryType will be implemented in ASTVisitor later
    // visitor.visitDictionaryType(*this);
}

bool DictionaryTypeNode::isCompatibleWith(const TypeNode& other) const {
    // التحقق من نوع العُقدة الأخرى / Check other node kind
    if (other.kind != TypeAnnotationKind::DICTIONARY) {
        // قاموس متوافق مع Generic<قاموس، K، V>
        // Dictionary is compatible with Generic<Dictionary, K, V>
        if (other.kind == TypeAnnotationKind::GENERIC) {
            const auto& otherGeneric = static_cast<const GenericTypeNode&>(other);
            if ((otherGeneric.baseName == "قاموس" || otherGeneric.baseName == "Dictionary" ||
                 otherGeneric.baseName == "Map") &&
                otherGeneric.typeParams.size() == 2) {
                return keyType->isCompatibleWith(*otherGeneric.typeParams[0]) &&
                       valueType->isCompatibleWith(*otherGeneric.typeParams[1]);
            }
        }
        return false;
    }
    
    const auto& otherDict = static_cast<const DictionaryTypeNode&>(other);
    
    // نوع المفتاح ونوع القيمة يجب أن يكونا متوافقين
    // Key type and value type must be compatible
    return keyType->isCompatibleWith(*otherDict.keyType) &&
           valueType->isCompatibleWith(*otherDict.valueType);
}

// ============================================================================
// Helper Functions Implementation / تنفيذ الدوال المساعدة
// ============================================================================

TypeNodePtr makeSimpleType(Types::SadTypeKind type, const Lexer::Position& pos) {
    // إنشاء عُقدة نوع بسيط / Create simple type node
    return std::make_shared<SimpleTypeNode>(type, pos);
}

TypeNodePtr makeSimpleType(const std::string& typeName, const Lexer::Position& pos) {
    // إنشاء عُقدة نوع بسيط من اسم / Create simple type node from name
    return std::make_shared<SimpleTypeNode>(typeName, pos);
}

TypeNodePtr makeGenericType(const std::string& baseName, 
                           std::vector<TypeNodePtr> params,
                           const Lexer::Position& pos) {
    // إنشاء عُقدة نوع عام / Create generic type node
    return std::make_shared<GenericTypeNode>(baseName, std::move(params), pos);
}

TypeNodePtr makeUnionType(std::vector<TypeNodePtr> types,
                         const Lexer::Position& pos) {
    // إنشاء عُقدة نوع اتحاد / Create union type node
    return std::make_shared<UnionTypeNode>(std::move(types), pos);
}

TypeNodePtr makeOptionalType(TypeNodePtr baseType,
                            const Lexer::Position& pos) {
    // إنشاء عُقدة نوع اختياري / Create optional type node
    return std::make_shared<OptionalTypeNode>(std::move(baseType), pos);
}

TypeNodePtr makeFunctionType(std::vector<TypeNodePtr> paramTypes,
                            TypeNodePtr returnType,
                            const Lexer::Position& pos) {
    // إنشاء عُقدة نوع دالة / Create function type node
    return std::make_shared<FunctionTypeNode>(std::move(paramTypes), 
                                             std::move(returnType), pos);
}

TypeNodePtr makeArrayType(TypeNodePtr elementType,
                         const Lexer::Position& pos) {
    // إنشاء عُقدة نوع مصفوفة / Create array type node
    return std::make_shared<ArrayTypeNode>(std::move(elementType), pos);
}

TypeNodePtr makeDictionaryType(TypeNodePtr keyType, TypeNodePtr valueType,
                              const Lexer::Position& pos) {
    // إنشاء عُقدة نوع قاموس / Create dictionary type node
    return std::make_shared<DictionaryTypeNode>(std::move(keyType), 
                                               std::move(valueType), pos);
}

} // namespace AST
} // namespace Sad
