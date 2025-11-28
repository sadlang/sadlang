// ======================================================================
// class_nodes.cpp - تنفيذ عُقد AST للأصناف / Class AST Nodes Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ دوال جميع عُقد AST الخاصة بالبرمجة الكائنية
//   تشمل accept() و toString() لجميع العُقد
//
// English Description:
//   Implementation of all OOP AST node functions
//   Includes accept() and toString() for all nodes
// ======================================================================

#include "parser/ast/class_nodes.h"
#include "parser/ast/ast_visitor.h"
#include <sstream>

namespace Sad {
namespace AST {

// ======================================================================
// ClassDeclStmt - تصريح صنف / Class Declaration Statement
// ======================================================================

void ClassDeclStmt::accept(ASTVisitor& visitor) {
    visitor.visitClassDeclStmt(*this);
}

std::string ClassDeclStmt::toString() const {
    std::ostringstream oss;
    oss << "صنف " << name;
    
    if (!baseClasses.empty()) {
        oss << " يرث ";
        for (size_t i = 0; i < baseClasses.size(); i++) {
            if (i > 0) oss << ", ";
            oss << baseClasses[i];
        }
    }
    
    oss << " {\n";
    
    // (AR) طباعة الحقول
    // (EN) Print fields
    for (const auto& field : fields) {
        oss << "  " << field->toString() << "\n";
    }
    
    // (AR) طباعة الباني
    // (EN) Print constructor
    if (constructor) {
        oss << "  " << constructor->toString() << "\n";
    }
    
    // (AR) طباعة الدوال
    // (EN) Print methods
    for (const auto& method : methods) {
        oss << "  " << method->toString() << "\n";
    }
    
    // (AR) طباعة الهادم
    // (EN) Print destructor
    if (destructor) {
        oss << "  " << destructor->toString() << "\n";
    }
    
    oss << "}";
    
    return oss.str();
}

// ======================================================================
// ملاحظة: تنفيذات FieldDecl, MethodDecl, ConstructorDecl, DestructorDecl
// موجودة في src/parser/ast/declarations.cpp
// Note: Implementations of FieldDecl, MethodDecl, ConstructorDecl, DestructorDecl
// are in src/parser/ast/declarations.cpp
// ======================================================================

// ======================================================================
// NewExpr - تعبير إنشاء كائن / Object Creation Expression
// ======================================================================

void NewExpr::accept(ASTVisitor& visitor) {
    visitor.visitNewExpr(*this);
}

std::string NewExpr::toString() const {
    std::ostringstream oss;
    
    oss << "جديد " << className << "(";
    
    // (AR) معاملات الباني
    // (EN) Constructor arguments
    for (size_t i = 0; i < arguments.size(); i++) {
        if (i > 0) oss << ", ";
        oss << arguments[i]->toString();
    }
    
    oss << ")";
    
    return oss.str();
}

// ======================================================================
// MemberAccessExpr - تعبير الوصول للعضو / Member Access Expression
// ======================================================================

void MemberAccessExpr::accept(ASTVisitor& visitor) {
    visitor.visitMemberAccessExpr(*this);
}

std::string MemberAccessExpr::toString() const {
    std::ostringstream oss;
    
    if (object) {
        oss << object->toString();
    }
    
    oss << "." << memberName;
    
    return oss.str();
}

// ======================================================================
// MethodCallExpr - تعبير استدعاء دالة / Method Call Expression
// ======================================================================

void MethodCallExpr::accept(ASTVisitor& visitor) {
    visitor.visitMethodCallExpr(*this);
}

std::string MethodCallExpr::toString() const {
    std::ostringstream oss;
    
    if (object) {
        oss << object->toString();
    }
    
    oss << "." << methodName << "(";
    
    // (AR) المعاملات
    // (EN) Arguments
    for (size_t i = 0; i < arguments.size(); i++) {
        if (i > 0) oss << ", ";
        oss << arguments[i]->toString();
    }
    
    oss << ")";
    
    return oss.str();
}

// ======================================================================
// ThisExpr - تعبير هذا / This Expression
// ======================================================================

void ThisExpr::accept(ASTVisitor& visitor) {
    visitor.visitThisExpr(*this);
}

std::string ThisExpr::toString() const {
    return "هذا";
}

// ======================================================================
// SuperExpr - تعبير الأساس / Super Expression
// ======================================================================

void SuperExpr::accept(ASTVisitor& visitor) {
    visitor.visitSuperExpr(*this);
}

std::string SuperExpr::toString() const {
    return "الأساس";
}

} // namespace AST
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
