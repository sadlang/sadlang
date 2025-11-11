/**
 * @file expressions.cpp
 * @brief Expression AST node implementations / تنفيذ عُقد AST للتعابير
 * @author Sad Language Team
 * @date 11 November 2025
 */

#include "../../../include/parser/ast/expressions.h"
#include <sstream>

namespace Sad {
namespace AST {

// =========================================================================
// BinaryExpr Implementation
// =========================================================================

Data::DataType BinaryExpr::getType() const {
    // Type inference based on operator and operands
    // استنتاج النوع بناءً على العامل والمعاملات
    
    using TT = Lexer::TokenType;
    
    // Logical operators always return boolean
    // العوامل المنطقية تُرجع دائماً قيمة منطقية
    if (op == TT::OP_AND || op == TT::OP_OR || op == TT::OP_NOT ||
        op == TT::OP_EQUAL || op == TT::OP_NOT_EQUAL ||
        op == TT::OP_LESS || op == TT::OP_LESS_EQUAL ||
        op == TT::OP_GREATER || op == TT::OP_GREATER_EQUAL) {
        return Data::DataType::BOOLEAN;
    }
    
    // Arithmetic operators: if either operand is float, result is float
    // العوامل الحسابية: إذا كان أحد المعاملات float، النتيجة float
    auto leftType = left->getType();
    auto rightType = right->getType();
    
    if (leftType == Data::DataType::FLOAT || rightType == Data::DataType::FLOAT) {
        return Data::DataType::FLOAT;
    }
    
    if (leftType == Data::DataType::STRING || rightType == Data::DataType::STRING) {
        return Data::DataType::STRING;
    }
    
    // Default to integer for arithmetic
    return Data::DataType::INTEGER;
}

// =========================================================================
// UnaryExpr Implementation
// =========================================================================

Data::DataType UnaryExpr::getType() const {
    using TT = Lexer::TokenType;
    
    // Logical NOT returns boolean
    if (op == TT::OP_NOT) {
        return Data::DataType::BOOLEAN;
    }
    
    // Other unary operators preserve operand type
    return operand->getType();
}

// =========================================================================
// LiteralExpr Implementation
// =========================================================================

Data::DataType LiteralExpr::getType() const {
    using TT = Lexer::TokenType;
    
    switch (token.getType()) {
        case TT::NUMBER_INTEGER:
            return Data::DataType::INTEGER;
        
        case TT::NUMBER_DOUBLE:
            return Data::DataType::FLOAT;
        
        case TT::STRING_LITERAL:
            return Data::DataType::STRING;
        
        case TT::LITERAL_TRUE:
        case TT::LITERAL_FALSE:
            return Data::DataType::BOOLEAN;
        
        case TT::LITERAL_NULL:
            return Data::DataType::NONE;
        
        default:
            return Data::DataType::UNKNOWN;
    }
}

// =========================================================================
// VariableExpr Implementation
// =========================================================================

Data::DataType VariableExpr::getType() const {
    // Type will be resolved during semantic analysis
    // سيتم تحديد النوع أثناء التحليل الدلالي
    return Data::DataType::UNKNOWN;
}

// =========================================================================
// AssignExpr Implementation
// =========================================================================

Data::DataType AssignExpr::getType() const {
    // Assignment expression returns the type of the assigned value
    return value->getType();
}

// =========================================================================
// CallExpr Implementation
// =========================================================================

std::string CallExpr::toString() const {
    std::ostringstream oss;
    oss << callee->toString() << "(";
    
    for (size_t i = 0; i < arguments.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << arguments[i]->toString();
    }
    
    oss << ")";
    return oss.str();
}

Data::DataType CallExpr::getType() const {
    // Return type will be determined during semantic analysis
    // نوع الإرجاع سيتحدد أثناء التحليل الدلالي
    return Data::DataType::UNKNOWN;
}

// =========================================================================
// IndexExpr Implementation
// =========================================================================

Data::DataType IndexExpr::getType() const {
    auto objType = object->getType();
    
    // Arrays and strings return element type
    // المصفوفات والنصوص تُرجع نوع العنصر
    if (objType == Data::DataType::ARRAY) {
        // Element type will be determined during semantic analysis
        return Data::DataType::UNKNOWN;
    }
    
    if (objType == Data::DataType::STRING) {
        return Data::DataType::STRING;
    }
    
    if (objType == Data::DataType::MAP) {
        // Value type will be determined during semantic analysis
        return Data::DataType::UNKNOWN;
    }
    
    return Data::DataType::UNKNOWN;
}

// =========================================================================
// MemberExpr Implementation
// =========================================================================

Data::DataType MemberExpr::getType() const {
    // Member type will be determined during semantic analysis
    // نوع العضو سيتحدد أثناء التحليل الدلالي
    return Data::DataType::UNKNOWN;
}

// =========================================================================
// ArrayExpr Implementation
// =========================================================================

std::string ArrayExpr::toString() const {
    std::ostringstream oss;
    oss << "[";
    
    for (size_t i = 0; i < elements.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << elements[i]->toString();
    }
    
    oss << "]";
    return oss.str();
}

// =========================================================================
// MapExpr Implementation
// =========================================================================

std::string MapExpr::toString() const {
    std::ostringstream oss;
    oss << "{";
    
    for (size_t i = 0; i < pairs.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << pairs[i].key->toString() << ": " << pairs[i].value->toString();
    }
    
    oss << "}";
    return oss.str();
}

// =========================================================================
// LambdaExpr Implementation
// =========================================================================

std::string LambdaExpr::toString() const {
    std::ostringstream oss;
    oss << "(";
    
    for (size_t i = 0; i < parameters.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << parameters[i].name;
    }
    
    oss << ") => " << body->toString();
    return oss.str();
}

// =========================================================================
// ListComprehensionExpr Implementation
// =========================================================================

std::string ListComprehensionExpr::toString() const {
    std::ostringstream oss;
    oss << "[" << element->toString() << " for " << variable 
        << " in " << iterable->toString();
    
    if (condition) {
        oss << " if " << condition->toString();
    }
    
    oss << "]";
    return oss.str();
}

// =========================================================================
// DictComprehensionExpr Implementation
// =========================================================================

std::string DictComprehensionExpr::toString() const {
    std::ostringstream oss;
    oss << "{" << key->toString() << ": " << value->toString() 
        << " for " << variable << " in " << iterable->toString();
    
    if (condition) {
        oss << " if " << condition->toString();
    }
    
    oss << "}";
    return oss.str();
}

// =========================================================================
// GeneratorExpr Implementation
// =========================================================================

std::string GeneratorExpr::toString() const {
    std::ostringstream oss;
    oss << "(" << element->toString() << " for " << variable 
        << " in " << iterable->toString();
    
    if (condition) {
        oss << " if " << condition->toString();
    }
    
    oss << ")";
    return oss.str();
}

} // namespace AST
} // namespace Sad
