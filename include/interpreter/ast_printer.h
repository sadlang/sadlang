/**
 * @file ast_printer.h
 * @brief (AR) طابع AST - مثال على استخدام نمط الزائر
 * @brief (EN) AST Printer - example visitor implementation
 * 
 * يطبع الشجرة في شكل نصي للتصحيح والاختبار.
 * Prints the tree in text format for debugging and testing.
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#pragma once

#include "../parser/ast/ast_visitor.h"
#include "../parser/ast/expressions.h"
#include "../parser/ast/statements.h"
#include "../parser/ast/declarations.h"
#include <sstream>
#include <string>

namespace Sad {
namespace Interpreter {

/**
 * @brief (AR) طابع AST - يحول الشجرة لنص
 * @brief (EN) AST Printer - converts tree to text
 * 
 * مثال على استخدام نمط الزائر. يجتاز الشجرة ويطبعها
 * في شكل نصي قابل للقراءة.
 * 
 * Example of using the Visitor pattern. Traverses the tree
 * and prints it in a readable text format.
 */
class ASTPrinter : public AST::BaseASTVisitor {
public:
    ASTPrinter() : indentLevel_(0) {}
    
    /**
     * @brief (AR) الحصول على النص المطبوع
     * @brief (EN) Get printed output
     */
    std::string getOutput() const {
        return output_.str();
    }
    
    /**
     * @brief (AR) مسح النص المطبوع
     * @brief (EN) Clear output
     */
    void clear() {
        output_.str("");
        output_.clear();
        indentLevel_ = 0;
    }
    
    // Expression visitors
    void visitBinaryExpr(AST::BinaryExpr& node) override {
        printIndent();
        output_ << "BinaryExpr(" << tokenTypeToString(node.op) << ")\n";
        
        increaseIndent();
        node.left->accept(*this);
        node.right->accept(*this);
        decreaseIndent();
    }
    
    void visitUnaryExpr(AST::UnaryExpr& node) override {
        printIndent();
        output_ << "UnaryExpr(" << tokenTypeToString(node.op) << ")\n";
        
        increaseIndent();
        node.operand->accept(*this);
        decreaseIndent();
    }
    
    void visitLiteralExpr(AST::LiteralExpr& node) override {
        printIndent();
        output_ << "Literal(" << node.token.getValue() << ")\n";
    }
    
    void visitVariableExpr(AST::VariableExpr& node) override {
        printIndent();
        output_ << "Variable(" << node.name << ")\n";
    }
    
    void visitAssignExpr(AST::AssignExpr& node) override {
        printIndent();
        output_ << "Assign(" << node.name << ")\n";
        
        increaseIndent();
        node.value->accept(*this);
        decreaseIndent();
    }
    
    void visitCallExpr(AST::CallExpr& node) override {
        printIndent();
        output_ << "Call\n";
        
        increaseIndent();
        printIndent();
        output_ << "Callee:\n";
        increaseIndent();
        node.callee->accept(*this);
        decreaseIndent();
        
        if (!node.arguments.empty()) {
            printIndent();
            output_ << "Arguments:\n";
            increaseIndent();
            for (auto& arg : node.arguments) {
                arg->accept(*this);
            }
            decreaseIndent();
        }
        decreaseIndent();
    }
    
    void visitArrayExpr(AST::ArrayExpr& node) override {
        printIndent();
        output_ << "Array[" << node.elements.size() << " elements]\n";
        
        increaseIndent();
        for (auto& elem : node.elements) {
            elem->accept(*this);
        }
        decreaseIndent();
    }
    
    void visitMapExpr(AST::MapExpr& node) override {
        printIndent();
        output_ << "Map[" << node.pairs.size() << " pairs]\n";
        
        increaseIndent();
        for (auto& pair : node.pairs) {
            printIndent();
            output_ << "Pair:\n";
            increaseIndent();
            pair.key->accept(*this);
            pair.value->accept(*this);
            decreaseIndent();
        }
        decreaseIndent();
    }
    
    // Statement visitors
    void visitExprStmt(AST::ExprStmt& node) override {
        printIndent();
        output_ << "ExprStmt\n";
        
        increaseIndent();
        node.expression->accept(*this);
        decreaseIndent();
    }
    
    void visitVarDeclStmt(AST::VarDeclStmt& node) override {
        printIndent();
        output_ << "VarDecl(" << node.name << ")\n";
        
        if (node.initializer) {
            increaseIndent();
            node.initializer->accept(*this);
            decreaseIndent();
        }
    }
    
    void visitBlockStmt(AST::BlockStmt& node) override {
        printIndent();
        output_ << "Block\n";
        
        increaseIndent();
        for (auto& stmt : node.statements) {
            stmt->accept(*this);
        }
        decreaseIndent();
    }
    
    void visitIfStmt(AST::IfStmt& node) override {
        printIndent();
        output_ << "If\n";
        
        increaseIndent();
        printIndent();
        output_ << "Condition:\n";
        increaseIndent();
        node.condition->accept(*this);
        decreaseIndent();
        
        printIndent();
        output_ << "Then:\n";
        increaseIndent();
        node.thenBranch->accept(*this);
        decreaseIndent();
        
        if (node.elseBranch) {
            printIndent();
            output_ << "Else:\n";
            increaseIndent();
            node.elseBranch->accept(*this);
            decreaseIndent();
        }
        decreaseIndent();
    }
    
    void visitWhileStmt(AST::WhileStmt& node) override {
        printIndent();
        output_ << "While\n";
        
        increaseIndent();
        printIndent();
        output_ << "Condition:\n";
        increaseIndent();
        node.condition->accept(*this);
        decreaseIndent();
        
        printIndent();
        output_ << "Body:\n";
        increaseIndent();
        node.body->accept(*this);
        decreaseIndent();
        decreaseIndent();
    }
    
    void visitReturnStmt(AST::ReturnStmt& node) override {
        printIndent();
        output_ << "Return\n";
        
        if (node.value) {
            increaseIndent();
            node.value->accept(*this);
            decreaseIndent();
        }
    }
    
    void visitFunctionDecl(AST::FunctionDecl& node) override {
        printIndent();
        output_ << "FunctionDecl(" << node.name << ")\n";
        
        increaseIndent();
        if (!node.parameters.empty()) {
            printIndent();
            output_ << "Parameters: ";
            for (size_t i = 0; i < node.parameters.size(); ++i) {
                if (i > 0) output_ << ", ";
                output_ << node.parameters[i].name;
            }
            output_ << "\n";
        }
        
        if (node.body) {
            printIndent();
            output_ << "Body:\n";
            increaseIndent();
            node.body->accept(*this);
            decreaseIndent();
        }
        decreaseIndent();
    }
    
private:
    std::ostringstream output_;
    int indentLevel_;
    
    void increaseIndent() { indentLevel_++; }
    void decreaseIndent() { indentLevel_--; }
    
    void printIndent() {
        for (int i = 0; i < indentLevel_; ++i) {
            output_ << "  ";
        }
    }
    
    std::string tokenTypeToString(Lexer::TokenType type) {
        using namespace Lexer;
        switch (type) {
            case TokenType::OP_PLUS: return "+";
            case TokenType::OP_MINUS: return "-";
            case TokenType::OP_MULTIPLY: return "*";
            case TokenType::OP_DIVIDE: return "/";
            case TokenType::OP_MODULO: return "%";
            case TokenType::OP_EQUAL: return "==";
            case TokenType::OP_NOT_EQUAL: return "!=";
            case TokenType::OP_LESS: return "<";
            case TokenType::OP_GREATER: return ">";
            case TokenType::OP_LESS_EQUAL: return "<=";
            case TokenType::OP_GREATER_EQUAL: return ">=";
            case TokenType::OP_AND: return "&&";
            case TokenType::OP_OR: return "||";
            case TokenType::OP_NOT: return "!";
            default: return "?";
        }
    }
};

} // namespace Interpreter
} // namespace Sad
