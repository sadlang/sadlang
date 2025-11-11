/**
 * @file statements.cpp
 * @brief Statement AST node implementations / تنفيذ عُقد AST للجمل
 * @author Sad Language Team
 * @date 11 November 2025
 */

#include "../../../include/parser/ast/statements.h"
#include <sstream>

namespace Sad {
namespace AST {

// =========================================================================
// VarDeclStmt Implementation
// =========================================================================

std::string VarDeclStmt::toString() const {
    std::ostringstream oss;
    
    oss << (isConst ? "const " : "var ") << name;
    
    if (type != Data::DataType::UNKNOWN) {
        oss << ": " << static_cast<int>(type);
    }
    
    if (initializer) {
        oss << " = " << initializer->toString();
    }
    
    oss << ";";
    return oss.str();
}

// =========================================================================
// IfStmt Implementation
// =========================================================================

std::string IfStmt::toString() const {
    std::ostringstream oss;
    
    oss << "if (" << condition->toString() << ") ";
    oss << thenBranch->toString();
    
    if (elseBranch) {
        oss << " else " << elseBranch->toString();
    }
    
    return oss.str();
}

// =========================================================================
// WhileStmt Implementation
// =========================================================================

std::string WhileStmt::toString() const {
    std::ostringstream oss;
    oss << "while (" << condition->toString() << ") ";
    oss << body->toString();
    return oss.str();
}

// =========================================================================
// ForStmt Implementation
// =========================================================================

std::string ForStmt::toString() const {
    std::ostringstream oss;
    oss << "for (";
    
    if (initializer) {
        oss << initializer->toString();
    }
    oss << " ";
    
    if (condition) {
        oss << condition->toString();
    }
    oss << "; ";
    
    if (increment) {
        oss << increment->toString();
    }
    
    oss << ") " << body->toString();
    return oss.str();
}

// =========================================================================
// ForRangeStmt Implementation
// =========================================================================

std::string ForRangeStmt::toString() const {
    std::ostringstream oss;
    oss << "for (";
    
    if (!valueVar.empty()) {
        oss << variable << ", " << valueVar;
    } else {
        oss << variable;
    }
    
    oss << " in " << iterable->toString() << ") ";
    oss << body->toString();
    return oss.str();
}

// =========================================================================
// BlockStmt Implementation
// =========================================================================

std::string BlockStmt::toString() const {
    std::ostringstream oss;
    oss << "{\n";
    
    for (const auto& stmt : statements) {
        oss << "  " << stmt->toString() << "\n";
    }
    
    oss << "}";
    return oss.str();
}

// =========================================================================
// TryStmt Implementation
// =========================================================================

std::string TryStmt::toString() const {
    std::ostringstream oss;
    oss << "try " << tryBlock->toString();
    
    for (const auto& catchClause : catchClauses) {
        oss << " catch (" << catchClause.exceptionVar << ") ";
        oss << catchClause.body->toString();
    }
    
    if (finallyBlock) {
        oss << " finally " << finallyBlock->toString();
    }
    
    return oss.str();
}

// =========================================================================
// WithStmt Implementation
// =========================================================================

std::string WithStmt::toString() const {
    std::ostringstream oss;
    oss << "with (" << variable << " = " << resource->toString() << ") ";
    oss << body->toString();
    return oss.str();
}

} // namespace AST
} // namespace Sad
