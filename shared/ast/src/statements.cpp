/**
 * @file statements.cpp
 * @brief Statement AST node implementations / تنفيذ عُقد AST للجمل
 * @author Sad Language Team
 * @date 11 November 2025
 */

#include "statements.h"
#include "type_bridge.h" // (S-TS-P2.5a) Types::fromDataType — مقارنة الحقل تتمحور على SadTypeKind
#include <sstream>

namespace Sad {
namespace AST {

// =========================================================================
// VarDeclStmt Implementation
// =========================================================================

std::string VarDeclStmt::toString() const {
    std::ostringstream oss;
    
    oss << (isConst ? "const " : "var ") << name;
    
    if (Types::fromDataType(type) != Types::SadTypeKind::Unknown) {
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
// SwitchStmt Implementation
// =========================================================================

/**
 * @brief (AR) يحول جملة switch إلى نص.
 *        (EN) Converts switch statement to string.
 * 
 * Format:
 *   switch (expression) {
 *       case value1: body1
 *       case value2: body2
 *       default: defaultBody
 *   }
 */
std::string SwitchStmt::toString() const {
    std::ostringstream oss;
    oss << "switch (" << expression->toString() << ") {\n";
    
    // Print all case branches
    // طباعة جميع فروع الحالات
    for (const auto& caseItem : cases) {
        oss << "  case " << caseItem.value->toString() << ": ";
        oss << caseItem.body->toString() << "\n";
    }
    
    // Print default case if exists
    // طباعة الحالة الافتراضية إن وُجدت
    if (defaultCase) {
        oss << "  default: " << defaultCase->toString() << "\n";
    }
    
    oss << "}";
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

} // namespace AST
} // namespace Sad
