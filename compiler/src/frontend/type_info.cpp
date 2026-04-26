// ======================================================================
// type_info.cpp - معلومات نوع البيانات / Type Information System Implementation
// ======================================================================

#include "type_info.h"
#include <sstream>
#include <stdexcept>
#include <algorithm>

namespace Sad {
namespace Compiler {
namespace SIR {

// ======================================================================
// PrimitiveType Implementation
// ======================================================================

std::string PrimitiveType::toString() const {
    switch (type) {
        case SadTypeKind::Void:     return "void";
        case SadTypeKind::Integer:      return "i64";
        case SadTypeKind::Float:      return "f64";
        case SadTypeKind::Boolean:     return "bool";
        case SadTypeKind::String:   return "string";
        default:                return "unknown";
    }
}

size_t PrimitiveType::getSize() const {
    switch (type) {
        case SadTypeKind::Void:     return 0;
        case SadTypeKind::Integer:      return 8;
        case SadTypeKind::Float:      return 8;
        case SadTypeKind::Boolean:     return 1;
        case SadTypeKind::String:   return 16;  // pointer + length
        default:                return 0;
    }
}

size_t PrimitiveType::getAlignment() const {
    switch (type) {
        case SadTypeKind::Void:     return 1;
        case SadTypeKind::Integer:      return 8;
        case SadTypeKind::Float:      return 8;
        case SadTypeKind::Boolean:     return 1;
        case SadTypeKind::String:   return 8;
        default:                return 1;
    }
}

bool PrimitiveType::equals(const TypePtr& other) const {
    if (!other) return false;
    
    if (other->getBaseType() != SadTypeKind::Integer &&
        other->getBaseType() != SadTypeKind::Float &&
        other->getBaseType() != SadTypeKind::Boolean &&
        other->getBaseType() != SadTypeKind::Void &&
        other->getBaseType() != SadTypeKind::String) {
        return false;
    }
    
    return type == other->getBaseType();
}

TypePtr PrimitiveType::clone() const {
    return std::make_shared<PrimitiveType>(type);
}

// ======================================================================
// PointerType Implementation
// ======================================================================

std::string PointerType::toString() const {
    if (!pointeeType) return "ptr";
    return pointeeType->toString() + "*";
}

size_t PointerType::getSize() const {
    return 8;  // 64-bit pointer
}

size_t PointerType::getAlignment() const {
    return 8;  // 64-bit alignment
}

bool PointerType::equals(const TypePtr& other) const {
    if (!other || other->getBaseType() != SadTypeKind::Pointer) return false;
    
    auto otherPtr = std::dynamic_pointer_cast<PointerType>(other);
    if (!otherPtr) return false;
    
    if (!pointeeType || !otherPtr->pointeeType) {
        return pointeeType == otherPtr->pointeeType;
    }
    
    return pointeeType->equals(otherPtr->pointeeType);
}

TypePtr PointerType::clone() const {
    return std::make_shared<PointerType>(pointeeType ? pointeeType->clone() : nullptr);
}

// ======================================================================
// ArrayType Implementation
// ======================================================================

std::string ArrayType::toString() const {
    std::ostringstream oss;
    oss << "[" << arraySize << " x " << (elementType ? elementType->toString() : "unknown") << "]";
    return oss.str();
}

size_t ArrayType::getSize() const {
    if (!elementType) return 0;
    return elementType->getSize() * arraySize;
}

size_t ArrayType::getAlignment() const {
    if (!elementType) return 1;
    return elementType->getAlignment();
}

bool ArrayType::equals(const TypePtr& other) const {
    if (!other || other->getBaseType() != SadTypeKind::Array) return false;
    
    auto otherArr = std::dynamic_pointer_cast<ArrayType>(other);
    if (!otherArr) return false;
    
    if (arraySize != otherArr->arraySize) return false;
    
    if (!elementType || !otherArr->elementType) {
        return elementType == otherArr->elementType;
    }
    
    return elementType->equals(otherArr->elementType);
}

TypePtr ArrayType::clone() const {
    return std::make_shared<ArrayType>(elementType ? elementType->clone() : nullptr, arraySize);
}

// ======================================================================
// StructType Implementation
// ======================================================================

void StructType::calculateOffsets() {
    fieldOffsets.clear();
    totalSize = 0;
    size_t currentOffset = 0;
    
    for (size_t i = 0; i < fieldTypes.size(); ++i) {
        if (!fieldTypes[i]) continue;
        
        size_t fieldAlignment = fieldTypes[i]->getAlignment();
        size_t alignmentPadding = (currentOffset % fieldAlignment == 0) ? 
                                   0 : (fieldAlignment - (currentOffset % fieldAlignment));
        
        currentOffset += alignmentPadding;
        fieldOffsets[std::to_string(i)] = currentOffset;
        currentOffset += fieldTypes[i]->getSize();
    }
    
    totalSize = currentOffset;
}

std::string StructType::toString() const {
    return "%" + structName;
}

size_t StructType::getAlignment() const {
    size_t maxAlignment = 1;
    for (const auto& field : fieldTypes) {
        if (field) {
            maxAlignment = std::max(maxAlignment, field->getAlignment());
        }
    }
    return maxAlignment;
}

bool StructType::equals(const TypePtr& other) const {
    if (!other || other->getBaseType() != SadTypeKind::Struct) return false;
    
    auto otherStruct = std::dynamic_pointer_cast<StructType>(other);
    if (!otherStruct) return false;
    
    if (structName != otherStruct->structName) return false;
    if (fieldTypes.size() != otherStruct->fieldTypes.size()) return false;
    
    for (size_t i = 0; i < fieldTypes.size(); ++i) {
        if (!fieldTypes[i] || !otherStruct->fieldTypes[i]) {
            if (fieldTypes[i] != otherStruct->fieldTypes[i]) return false;
        } else {
            if (!fieldTypes[i]->equals(otherStruct->fieldTypes[i])) return false;
        }
    }
    
    return true;
}

TypePtr StructType::clone() const {
    std::vector<TypePtr> clonedFields;
    for (const auto& field : fieldTypes) {
        clonedFields.push_back(field ? field->clone() : nullptr);
    }
    return std::make_shared<StructType>(structName, clonedFields);
}

size_t StructType::getFieldOffset(size_t index) const {
    auto it = fieldOffsets.find(std::to_string(index));
    return it != fieldOffsets.end() ? it->second : 0;
}

// ======================================================================
// FunctionType Implementation
// ======================================================================

std::string FunctionType::toString() const {
    std::ostringstream oss;
    
    if (returnType) {
        oss << returnType->toString();
    } else {
        oss << "unknown";
    }
    
    oss << "(";
    
    for (size_t i = 0; i < parameterTypes.size(); ++i) {
        if (i > 0) oss << ", ";
        if (parameterTypes[i]) {
            oss << parameterTypes[i]->toString();
        } else {
            oss << "unknown";
        }
    }
    
    oss << ")";
    
    return oss.str();
}

size_t FunctionType::getSize() const {
    return 8;  // Function pointers are 8 bytes on 64-bit systems
}

size_t FunctionType::getAlignment() const {
    return 8;  // Function pointers are 8-byte aligned
}

bool FunctionType::equals(const TypePtr& other) const {
    if (!other || other->getBaseType() != SadTypeKind::Function) return false;
    
    auto otherFunc = std::dynamic_pointer_cast<FunctionType>(other);
    if (!otherFunc) return false;
    
    if (parameterTypes.size() != otherFunc->parameterTypes.size()) return false;
    
    if (!returnType || !otherFunc->returnType) {
        if (returnType != otherFunc->returnType) return false;
    } else {
        if (!returnType->equals(otherFunc->returnType)) return false;
    }
    
    for (size_t i = 0; i < parameterTypes.size(); ++i) {
        if (!parameterTypes[i] || !otherFunc->parameterTypes[i]) {
            if (parameterTypes[i] != otherFunc->parameterTypes[i]) return false;
        } else {
            if (!parameterTypes[i]->equals(otherFunc->parameterTypes[i])) return false;
        }
    }
    
    return true;
}

TypePtr FunctionType::clone() const {
    std::vector<TypePtr> clonedParams;
    for (const auto& param : parameterTypes) {
        clonedParams.push_back(param ? param->clone() : nullptr);
    }
    return std::make_shared<FunctionType>(returnType ? returnType->clone() : nullptr, clonedParams);
}

// ======================================================================
// Helper Functions
// ======================================================================

SadTypeKind stringToSIRType(const std::string& str) {
    if (str == "void") return SadTypeKind::Void;
    if (str == "i64") return SadTypeKind::Integer;
    if (str == "f64") return SadTypeKind::Float;
    if (str == "bool") return SadTypeKind::Boolean;
    if (str == "string") return SadTypeKind::String;
    if (str == "ptr") return SadTypeKind::Pointer;
    if (str == "array") return SadTypeKind::Array;
    if (str == "struct") return SadTypeKind::Struct;
    if (str == "function") return SadTypeKind::Function;
    
    throw std::invalid_argument("Unknown type: " + str);
}

bool areTypesCompatible(const TypePtr& type1, const TypePtr& type2) {
    if (!type1 || !type2) return false;
    
    // Exact match
    if (type1->equals(type2)) return true;
    
    // Allow implicit conversions for numeric types
    SadTypeKind t1 = type1->getBaseType();
    SadTypeKind t2 = type2->getBaseType();
    
    // I64 and BOOL are compatible
    if ((t1 == SadTypeKind::Integer && t2 == SadTypeKind::Boolean) ||
        (t1 == SadTypeKind::Boolean && t2 == SadTypeKind::Integer)) {
        return true;
    }
    
    // I64 and F64 are compatible
    if ((t1 == SadTypeKind::Integer && t2 == SadTypeKind::Float) ||
        (t1 == SadTypeKind::Float && t2 == SadTypeKind::Integer)) {
        return true;
    }
    
    return false;
}

} // namespace SIR
} // namespace Compiler
} // namespace Sad
