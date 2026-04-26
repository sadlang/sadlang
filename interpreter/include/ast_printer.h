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

#include "ast_visitor.h"
#include "expressions.h"
#include "statements.h"
#include "declarations.h"
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
    
    void visitIndexExpr(AST::IndexExpr& node) override {
        printIndent();
        output_ << "IndexExpr\n";
        
        increaseIndent();
        printIndent();
        output_ << "Object:\n";
        increaseIndent();
        node.object->accept(*this);
        decreaseIndent();
        
        printIndent();
        output_ << "Index:\n";
        increaseIndent();
        node.index->accept(*this);
        decreaseIndent();
        decreaseIndent();
    }
    
    void visitMemberExpr(AST::MemberExpr& node) override {
        printIndent();
        output_ << "MemberExpr(" << node.member << ")\n";
        
        increaseIndent();
        node.object->accept(*this);
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
    
    void visitLambdaExpr(AST::LambdaExpr& node) override {
        printIndent();
        output_ << "LambdaExpr\n";
        
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
        
        printIndent();
        output_ << "Body:\n";
        increaseIndent();
        node.body->accept(*this);
        decreaseIndent();
        decreaseIndent();
    }
    
    void visitWalrusExpr(AST::WalrusExpr& node) override {
        printIndent();
        output_ << "WalrusExpr (:=) -> " << node.variable << "\n";
    }
    
    void visitListComprehensionExpr(AST::ListComprehensionExpr& node) override {
        printIndent();
        output_ << "ListComprehension\n";
    }
    
    void visitDictComprehensionExpr(AST::DictComprehensionExpr& node) override {
        printIndent();
        output_ << "DictComprehension\n";
    }
    
    void visitSetComprehensionExpr(AST::SetComprehensionExpr& node) override {
        printIndent();
        output_ << "SetComprehension\n";
    }
    
    void visitGeneratorExpr(AST::GeneratorExpr& node) override {
        printIndent();
        output_ << "GeneratorExpr\n";
    }
    
    void visitDecoratorExpr(AST::DecoratorExpr& node) override {
        printIndent();
        output_ << "DecoratorExpr\n";
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
    
    void visitForStmt(AST::ForStmt& node) override {
        printIndent();
        output_ << "ForStmt\n";
    }
    
    void visitForRangeStmt(AST::ForRangeStmt& node) override {
        printIndent();
        output_ << "ForRangeStmt\n";
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
    
    void visitYieldStmt(AST::YieldStmt& node) override {
        printIndent();
        output_ << "YieldStmt\n";
    }
    
    void visitBreakStmt(AST::BreakStmt& node) override {
        printIndent();
        output_ << "BreakStmt\n";
    }
    
    void visitContinueStmt(AST::ContinueStmt& node) override {
        printIndent();
        output_ << "ContinueStmt\n";
    }
    
    void visitTryStmt(AST::TryStmt& node) override {
        printIndent();
        output_ << "TryStmt\n";
    }
    
    void visitRaiseStmt(AST::RaiseStmt& node) override {
        printIndent();
        output_ << "RaiseStmt\n";
    }
    
    void visitWithStmt(AST::WithStmt& node) override {
        printIndent();
        output_ << "WithStmt\n";
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
    
    void visitClassDecl(AST::ClassDecl& node) override {
        printIndent();
        output_ << "ClassDecl(" << node.name << ")\n";
    }
    
    void visitFieldDecl(AST::FieldDecl& node) override {
        printIndent();
        output_ << "FieldDecl(" << node.name << ")\n";
    }
    
    void visitMethodDecl(AST::MethodDecl& node) override {
        printIndent();
        output_ << "MethodDecl(" << node.name << ")\n";
    }
    
    void visitConstructorDecl(AST::ConstructorDecl& node) override {
        printIndent();
        output_ << "ConstructorDecl\n";
    }
    
    void visitDestructorDecl(AST::DestructorDecl& node) override {
        printIndent();
        output_ << "DestructorDecl\n";
    }
    
    void visitEnumDecl(AST::EnumDecl& node) override {
        printIndent();
        output_ << "EnumDecl\n";
    }
    
    void visitImportStmt(AST::ImportStmt& node) override {
        printIndent();
        output_ << "ImportStmt\n";
    }
    
    void visitExportStmt(AST::ExportStmt& node) override {
        printIndent();
        output_ << "ExportStmt\n";
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
            case TokenType::OP_FLOOR_DIVIDE: return "//";
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
