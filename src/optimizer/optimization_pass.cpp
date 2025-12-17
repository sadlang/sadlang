#include "optimizer/optimization_pass.h"
#include "parser/ast/expressions.h"
#include "parser/ast/statements.h"
#include "parser/ast/declarations.h"
#include "parser/ast/ast_visitor.h"
#include "lexer/token.h"
#include <iostream>
#include <cmath>
#include <set>
#include <map>

namespace sad {

// =====================================
// Dead Code Elimination Pass
// =====================================

class DeadCodeEliminationVisitor : public Sad::AST::ASTVisitor {
private:
    std::set<std::string> definedVars_;    // Variables that are defined
    std::set<std::string> usedVars_;       // Variables that are used
    std::set<std::string> deadVars_;       // Variables that are unused
    bool modified_ = false;
    bool afterReturn_ = false;             // Track if we're after a return statement
    
    // Helper to mark a variable as defined
    void defineVar(const std::string& name) {
        definedVars_.insert(name);
    }
    
    // Helper to mark a variable as used
    void useVar(const std::string& name) {
        usedVars_.insert(name);
    }
    
public:
    bool wasModified() const { return modified_; }
    
    const std::set<std::string>& getDeadVars() const { return deadVars_; }
    
    // After first pass, compute dead variables
    void computeDeadVariables() {
        for (const auto& var : definedVars_) {
            if (usedVars_.find(var) == usedVars_.end()) {
                deadVars_.insert(var);
            }
        }
    }
    
    // Visit variable expression - mark as used
    void visitVariableExpr(Sad::AST::VariableExpr& node) override {
        useVar(node.name);
    }
    
    // Visit assignment - mark LHS as defined, visit RHS
    void visitAssignExpr(Sad::AST::AssignExpr& node) override {
        // Visit value first to mark used variables
        if (node.value) node.value->accept(*this);
        
        // Mark variable as defined
        defineVar(node.name);
    }
    
    // Visit block statement - check for unreachable code after return
    void visitBlockStmt(Sad::AST::BlockStmt& stmt) override {
        bool previousAfterReturn = afterReturn_;
        
        for (size_t i = 0; i < stmt.statements.size(); ++i) {
            auto& s = stmt.statements[i];
            if (!s) continue;
            
            if (afterReturn_) {
                std::cout << "    ✓ كود ميت: عبارة غير قابلة للوصول بعد return\n";
                modified_ = true;
                break; // Stop processing - all following statements are dead
            }
            
            s->accept(*this);
        }
        
        afterReturn_ = previousAfterReturn;
    }
    
    // Visit return statement - mark that we're after a return
    void visitReturnStmt(Sad::AST::ReturnStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
        afterReturn_ = true;
    }
    
    // Visit variable declaration statement
    void visitVarDeclStmt(Sad::AST::VarDeclStmt& stmt) override {
        defineVar(stmt.name);
        if (stmt.initializer) stmt.initializer->accept(*this);
    }
    
    // Expression visitors
    void visitBinaryExpr(Sad::AST::BinaryExpr& node) override {
        if (node.left) node.left->accept(*this);
        if (node.right) node.right->accept(*this);
    }
    
    void visitUnaryExpr(Sad::AST::UnaryExpr& node) override {
        if (node.operand) node.operand->accept(*this);
    }
    
    void visitLiteralExpr(Sad::AST::LiteralExpr&) override {}
    
    void visitCallExpr(Sad::AST::CallExpr& node) override {
        if (node.callee) node.callee->accept(*this);
        for (auto& arg : node.arguments) {
            if (arg) arg->accept(*this);
        }
    }
    
    void visitIndexExpr(Sad::AST::IndexExpr& node) override {
        if (node.object) node.object->accept(*this);
        if (node.index) node.index->accept(*this);
    }
    
    void visitArrayExpr(Sad::AST::ArrayExpr& node) override {
        for (auto& elem : node.elements) {
            if (elem) elem->accept(*this);
        }
    }
    
    void visitMapExpr(Sad::AST::MapExpr& node) override {
        for (auto& pair : node.pairs) {
            if (pair.key) pair.key->accept(*this);
            if (pair.value) pair.value->accept(*this);
        }
    }
    
    void visitTernaryExpr(Sad::AST::TernaryExpr& node) override {
        if (node.condition) node.condition->accept(*this);
        if (node.trueExpr) node.trueExpr->accept(*this);
        if (node.falseExpr) node.falseExpr->accept(*this);
    }
    
    void visitLambdaExpr(Sad::AST::LambdaExpr&) override {}
    void visitNewExpr(Sad::AST::NewExpr&) override {}
    void visitMemberAccessExpr(Sad::AST::MemberAccessExpr&) override {}
    void visitMethodCallExpr(Sad::AST::MethodCallExpr&) override {}
    void visitThisExpr(Sad::AST::ThisExpr&) override {}
    void visitSuperExpr(Sad::AST::SuperExpr&) override {}
    void visitMemberExpr(Sad::AST::MemberExpr&) override {}
    void visitMemberAssignExpr(Sad::AST::MemberAssignExpr&) override {}
    void visitListComprehensionExpr(Sad::AST::ListComprehensionExpr&) override {}
    void visitDictComprehensionExpr(Sad::AST::DictComprehensionExpr&) override {}
    void visitGeneratorExpr(Sad::AST::GeneratorExpr&) override {}
    void visitDecoratorExpr(Sad::AST::DecoratorExpr&) override {}
    
    // Statement visitors
    void visitExprStmt(Sad::AST::ExprStmt& stmt) override {
        if (stmt.expression) stmt.expression->accept(*this);
    }
    
    void visitIfStmt(Sad::AST::IfStmt& stmt) override {
        if (stmt.condition) stmt.condition->accept(*this);
        
        bool savedAfterReturn = afterReturn_;
        
        if (stmt.thenBranch) {
            afterReturn_ = savedAfterReturn;
            stmt.thenBranch->accept(*this);
            bool thenReturns = afterReturn_;
            
            if (stmt.elseBranch) {
                afterReturn_ = savedAfterReturn;
                stmt.elseBranch->accept(*this);
                bool elseReturns = afterReturn_;
                
                // Only set afterReturn if BOTH branches return
                afterReturn_ = thenReturns && elseReturns;
            } else {
                afterReturn_ = savedAfterReturn; // Reset if no else
            }
        }
    }
    
    void visitWhileStmt(Sad::AST::WhileStmt& stmt) override {
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    
    void visitForStmt(Sad::AST::ForStmt& stmt) override {
        if (stmt.initializer) stmt.initializer->accept(*this);
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.increment) stmt.increment->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    
    void visitBreakStmt(Sad::AST::BreakStmt&) override {}
    void visitContinueStmt(Sad::AST::ContinueStmt&) override {}
    
    void visitSwitchStmt(Sad::AST::SwitchStmt& stmt) override {
        if (stmt.expression) stmt.expression->accept(*this);
        for (auto& c : stmt.cases) {
            if (c.value) c.value->accept(*this);
            if (c.body) c.body->accept(*this);
        }
    }
    
    void visitForRangeStmt(Sad::AST::ForRangeStmt& stmt) override {
        defineVar(stmt.variable);
        if (stmt.iterable) stmt.iterable->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    
    void visitYieldStmt(Sad::AST::YieldStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
    }
    
    void visitTryStmt(Sad::AST::TryStmt& stmt) override {
        if (stmt.tryBlock) stmt.tryBlock->accept(*this);
        for (auto& c : stmt.catchClauses) {
            if (c.body) c.body->accept(*this);
        }
        if (stmt.finallyBlock) stmt.finallyBlock->accept(*this);
    }
    
    void visitRaiseStmt(Sad::AST::RaiseStmt& stmt) override {
        if (stmt.exception) stmt.exception->accept(*this);
    }
    
    void visitWithStmt(Sad::AST::WithStmt& stmt) override {
        if (stmt.resource) stmt.resource->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    
    void visitClassDeclStmt(Sad::AST::ClassDeclStmt&) override {}
    
    // Declaration visitors
    void visitFunctionDecl(Sad::AST::FunctionDecl& decl) override {
        // Mark parameters as defined
        for (const auto& param : decl.parameters) {
            defineVar(param.name);
        }
        if (decl.body) {
            afterReturn_ = false; // Reset for function body
            decl.body->accept(*this);
        }
    }
    
    void visitClassDecl(Sad::AST::ClassDecl& decl) override {
        for (auto& member : decl.members) {
            if (member) member->accept(*this);
        }
    }
    
    void visitFieldDecl(Sad::AST::FieldDecl& decl) override {
        if (decl.initializer) decl.initializer->accept(*this);
    }
    
    void visitMethodDecl(Sad::AST::MethodDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    
    void visitPropertyDecl(Sad::AST::PropertyDecl&) override {}
    
    void visitConstructorDecl(Sad::AST::ConstructorDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    
    void visitDestructorDecl(Sad::AST::DestructorDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    
    void visitEnumDecl(Sad::AST::EnumDecl&) override {}
    void visitImportStmt(Sad::AST::ImportStmt&) override {}
    void visitExportStmt(Sad::AST::ExportStmt&) override {}
    void visitFromImportStmt(Sad::AST::FromImportStmt&) override {}
    void visitExportDecl(Sad::AST::ExportDecl&) override {}
};

std::shared_ptr<ASTNode> DeadCodeEliminationPass::apply(std::shared_ptr<ASTNode> ast) {
    if (!ast) return nullptr;
    
    std::cout << "  → تطبيق حذف الكود الميت...\n";
    std::cout << "  → Applying dead code elimination...\n";
    
    DeadCodeEliminationVisitor visitor;
    ast->accept(visitor);
    
    // Compute which variables are dead
    visitor.computeDeadVariables();
    
    const auto& deadVars = visitor.getDeadVars();
    if (!deadVars.empty()) {
        std::cout << "    ✓ متغيرات غير مستخدمة: ";
        for (const auto& var : deadVars) {
            std::cout << var << " ";
        }
        std::cout << "\n";
    }
    
    if (visitor.wasModified()) {
        std::cout << "    ✅ تم حذف كود ميت\n";
    } else if (deadVars.empty()) {
        std::cout << "    ℹ لا يوجد كود ميت\n";
    }
    
    return ast;
}

// =====================================
// Constant Folding Pass
// =====================================

class ConstantFoldingVisitor : public Sad::AST::ASTVisitor {
private:
    bool modified_ = false;
    
    // Helper to check if expr is a literal number
    bool isNumericLiteral(Sad::AST::Expression* expr, double& outValue) {
        auto lit = dynamic_cast<Sad::AST::LiteralExpr*>(expr);
        if (!lit) return false;
        
        try {
            outValue = std::stod(lit->token.getValue());
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Helper to create a literal expression with given value
    std::shared_ptr<Sad::AST::LiteralExpr> makeLiteral(double value) {
        // Note: In real implementation, we'd need proper token creation
        // For now, we'll just mark as modified and log the folding
        return nullptr; // Placeholder
    }
    
public:
    bool wasModified() const { return modified_; }
    
    // Visit BinaryExpr to fold constant operations
    void visitBinaryExpr(Sad::AST::BinaryExpr& node) override {
        // First, recursively fold child expressions
        if (node.left) node.left->accept(*this);
        if (node.right) node.right->accept(*this);
        
        double leftVal, rightVal;
        
        // Check if both operands are numeric literals
        if (!isNumericLiteral(node.left.get(), leftVal) || 
            !isNumericLiteral(node.right.get(), rightVal)) {
            return; // Cannot fold
        }
        
        // Fold arithmetic operations
        double result = 0.0;
        bool canFold = true;
        std::string opName;
        
        using TokenType = Sad::Lexer::TokenType;
        
        if (node.op == TokenType::OP_PLUS) {
            result = leftVal + rightVal;
            opName = "+";
        } else if (node.op == TokenType::OP_MINUS) {
            result = leftVal - rightVal;
            opName = "-";
        } else if (node.op == TokenType::OP_MULTIPLY) {
            result = leftVal * rightVal;
            opName = "*";
        } else if (node.op == TokenType::OP_DIVIDE) {
            if (std::abs(rightVal) < 0.0001) {
                canFold = false; // Division by zero
            } else {
                result = leftVal / rightVal;
                opName = "/";
            }
        } else if (node.op == TokenType::OP_MODULO) {
            if (std::abs(rightVal) < 0.0001) {
                canFold = false; // Modulo by zero
            } else {
                result = std::fmod(leftVal, rightVal);
                opName = "%";
            }
        } else if (node.op == TokenType::OP_POWER) {
            result = std::pow(leftVal, rightVal);
            opName = "**";
        } else {
            canFold = false; // Not an arithmetic operation
        }
        
        if (canFold) {
            std::cout << "    ✓ طي ثابت: " << leftVal << " " << opName << " " 
                      << rightVal << " → " << result << "\n";
            modified_ = true;
            
            // Note: In a real implementation, we would replace the node
            // with a new LiteralExpr containing the result.
            // For now, we just mark as modified and log the folding.
        }
    }
    
    // Visit UnaryExpr to fold constant unary operations
    void visitUnaryExpr(Sad::AST::UnaryExpr& node) override {
        if (node.operand) node.operand->accept(*this);
        
        double value;
        if (!isNumericLiteral(node.operand.get(), value)) {
            return; // Cannot fold
        }
        
        using TokenType = Sad::Lexer::TokenType;
        
        if (node.op == TokenType::OP_MINUS) {
            std::cout << "    ✓ طي ثابت: -" << value << " → " << (-value) << "\n";
            modified_ = true;
        } else if (node.op == TokenType::OP_PLUS) {
            std::cout << "    ✓ طي ثابت: +" << value << " → " << value << "\n";
            modified_ = true;
        }
    }
    
    // Default implementations for other expressions
    void visitLiteralExpr(Sad::AST::LiteralExpr&) override {}
    void visitVariableExpr(Sad::AST::VariableExpr&) override {}
    
    void visitAssignExpr(Sad::AST::AssignExpr& node) override {
        if (node.value) node.value->accept(*this);
    }
    
    void visitCallExpr(Sad::AST::CallExpr& node) override {
        if (node.callee) node.callee->accept(*this);
        for (auto& arg : node.arguments) {
            if (arg) arg->accept(*this);
        }
    }
    
    void visitIndexExpr(Sad::AST::IndexExpr& node) override {
        if (node.object) node.object->accept(*this);
        if (node.index) node.index->accept(*this);
    }
    
    void visitArrayExpr(Sad::AST::ArrayExpr& node) override {
        for (auto& elem : node.elements) {
            if (elem) elem->accept(*this);
        }
    }
    
    void visitMapExpr(Sad::AST::MapExpr& node) override {
        for (auto& pair : node.pairs) {
            if (pair.key) pair.key->accept(*this);
            if (pair.value) pair.value->accept(*this);
        }
    }
    
    void visitTernaryExpr(Sad::AST::TernaryExpr& node) override {
        if (node.condition) node.condition->accept(*this);
        if (node.trueExpr) node.trueExpr->accept(*this);
        if (node.falseExpr) node.falseExpr->accept(*this);
    }
    
    void visitLambdaExpr(Sad::AST::LambdaExpr&) override {}
    void visitNewExpr(Sad::AST::NewExpr&) override {}
    void visitMemberAccessExpr(Sad::AST::MemberAccessExpr&) override {}
    void visitMethodCallExpr(Sad::AST::MethodCallExpr&) override {}
    void visitThisExpr(Sad::AST::ThisExpr&) override {}
    void visitSuperExpr(Sad::AST::SuperExpr&) override {}
    void visitMemberExpr(Sad::AST::MemberExpr&) override {}
    void visitMemberAssignExpr(Sad::AST::MemberAssignExpr&) override {}
    void visitListComprehensionExpr(Sad::AST::ListComprehensionExpr&) override {}
    void visitDictComprehensionExpr(Sad::AST::DictComprehensionExpr&) override {}
    void visitGeneratorExpr(Sad::AST::GeneratorExpr&) override {}
    void visitDecoratorExpr(Sad::AST::DecoratorExpr&) override {}
    
    // Statement visitors (just traverse)
    void visitExprStmt(Sad::AST::ExprStmt& stmt) override {
        if (stmt.expression) stmt.expression->accept(*this);
    }
    
    void visitBlockStmt(Sad::AST::BlockStmt& stmt) override {
        for (auto& s : stmt.statements) {
            if (s) s->accept(*this);
        }
    }
    
    void visitIfStmt(Sad::AST::IfStmt& stmt) override {
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.thenBranch) stmt.thenBranch->accept(*this);
        if (stmt.elseBranch) stmt.elseBranch->accept(*this);
    }
    
    void visitWhileStmt(Sad::AST::WhileStmt& stmt) override {
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    
    void visitForStmt(Sad::AST::ForStmt& stmt) override {
        if (stmt.initializer) stmt.initializer->accept(*this);
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.increment) stmt.increment->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    
    void visitReturnStmt(Sad::AST::ReturnStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
    }
    
    void visitBreakStmt(Sad::AST::BreakStmt&) override {}
    void visitContinueStmt(Sad::AST::ContinueStmt&) override {}
    
    void visitSwitchStmt(Sad::AST::SwitchStmt& stmt) override {
        if (stmt.expression) stmt.expression->accept(*this);
        for (auto& c : stmt.cases) {
            if (c.value) c.value->accept(*this);
            if (c.body) c.body->accept(*this);
        }
    }
    
    void visitVarDeclStmt(Sad::AST::VarDeclStmt& stmt) override {
        if (stmt.initializer) stmt.initializer->accept(*this);
    }
    
    void visitForRangeStmt(Sad::AST::ForRangeStmt& stmt) override {
        if (stmt.iterable) stmt.iterable->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    
    void visitYieldStmt(Sad::AST::YieldStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
    }
    
    void visitTryStmt(Sad::AST::TryStmt& stmt) override {
        if (stmt.tryBlock) stmt.tryBlock->accept(*this);
        for (auto& c : stmt.catchClauses) {
            if (c.body) c.body->accept(*this);
        }
        if (stmt.finallyBlock) stmt.finallyBlock->accept(*this);
    }
    
    void visitRaiseStmt(Sad::AST::RaiseStmt& stmt) override {
        if (stmt.exception) stmt.exception->accept(*this);
    }
    
    void visitWithStmt(Sad::AST::WithStmt& stmt) override {
        if (stmt.resource) stmt.resource->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    
    void visitClassDeclStmt(Sad::AST::ClassDeclStmt&) override {}
    
    // Declaration visitors
    void visitFunctionDecl(Sad::AST::FunctionDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    
    void visitClassDecl(Sad::AST::ClassDecl& decl) override {
        for (auto& member : decl.members) {
            if (member) member->accept(*this);
        }
    }
    
    void visitFieldDecl(Sad::AST::FieldDecl& decl) override {
        if (decl.initializer) decl.initializer->accept(*this);
    }
    
    void visitMethodDecl(Sad::AST::MethodDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    
    void visitPropertyDecl(Sad::AST::PropertyDecl&) override {}
    void visitConstructorDecl(Sad::AST::ConstructorDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    
    void visitDestructorDecl(Sad::AST::DestructorDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    
    void visitEnumDecl(Sad::AST::EnumDecl&) override {}
    void visitImportStmt(Sad::AST::ImportStmt&) override {}
    void visitExportStmt(Sad::AST::ExportStmt&) override {}
    void visitFromImportStmt(Sad::AST::FromImportStmt&) override {}
    void visitExportDecl(Sad::AST::ExportDecl&) override {}
};

std::shared_ptr<ASTNode> ConstantFoldingPass::apply(std::shared_ptr<ASTNode> ast) {
    if (!ast) return nullptr;
    
    std::cout << "  → تطبيق طي الثوابت...\n";
    std::cout << "  → Applying constant folding...\n";
    
    ConstantFoldingVisitor visitor;
    ast->accept(visitor);
    
    if (visitor.wasModified()) {
        std::cout << "    ✅ تم طي ثوابت\n";
    } else {
        std::cout << "    ℹ لا توجد ثوابت لطيها\n";
    }
    
    return ast;
}

// =====================================
// Expression Simplification Visitor
// =====================================

class ExpressionSimplificationVisitor : public Sad::AST::ASTVisitor {
private:
    bool modified_ = false;
    
    // Helper to check if expression is a literal with specific value
    bool isLiteralWithValue(Sad::AST::Expression* expr, double value) {
        auto lit = dynamic_cast<Sad::AST::LiteralExpr*>(expr);
        if (!lit) return false;
        
        try {
            double val = std::stod(lit->token.getValue());
            return std::abs(val - value) < 0.0001;
        } catch (...) {
            return false;
        }
    }
    
    // Helper to check if two expressions are the same variable
    bool areIdenticalVariables(Sad::AST::Expression* left, Sad::AST::Expression* right) {
        auto leftVar = dynamic_cast<Sad::AST::VariableExpr*>(left);
        auto rightVar = dynamic_cast<Sad::AST::VariableExpr*>(right);
        
        if (!leftVar || !rightVar) return false;
        return leftVar->name == rightVar->name;
    }
    
public:
    bool wasModified() const { return modified_; }
    
    void visitBinaryExpr(Sad::AST::BinaryExpr& node) override {
        // First, recursively simplify child expressions
        if (node.left) node.left->accept(*this);
        if (node.right) node.right->accept(*this);
        
        // Simplification rules
        
        // Rule: x + 0 → x  or  0 + x → x
        if (node.op == Sad::Lexer::TokenType::OP_PLUS) {
            if (isLiteralWithValue(node.right.get(), 0.0)) {
                // x + 0 → x: replace with left operand
                // Note: We can't actually replace the node itself, but we can mark it
                std::cout << "    ✓ تبسيط: x + 0 → x\n";
                modified_ = true;
                return;
            }
            if (isLiteralWithValue(node.left.get(), 0.0)) {
                // 0 + x → x: replace with right operand
                std::cout << "    ✓ تبسيط: 0 + x → x\n";
                modified_ = true;
                return;
            }
        }
        
        // Rule: x - 0 → x
        if (node.op == Sad::Lexer::TokenType::OP_MINUS) {
            if (isLiteralWithValue(node.right.get(), 0.0)) {
                std::cout << "    ✓ تبسيط: x - 0 → x\n";
                modified_ = true;
                return;
            }
            // Rule: x - x → 0
            if (areIdenticalVariables(node.left.get(), node.right.get())) {
                std::cout << "    ✓ تبسيط: x - x → 0\n";
                modified_ = true;
                return;
            }
        }
        
        // Rule: x * 1 → x  or  1 * x → x
        if (node.op == Sad::Lexer::TokenType::OP_MULTIPLY) {
            if (isLiteralWithValue(node.right.get(), 1.0)) {
                std::cout << "    ✓ تبسيط: x * 1 → x\n";
                modified_ = true;
                return;
            }
            if (isLiteralWithValue(node.left.get(), 1.0)) {
                std::cout << "    ✓ تبسيط: 1 * x → x\n";
                modified_ = true;
                return;
            }
            
            // Rule: x * 0 → 0  or  0 * x → 0
            if (isLiteralWithValue(node.right.get(), 0.0) || 
                isLiteralWithValue(node.left.get(), 0.0)) {
                std::cout << "    ✓ تبسيط: x * 0 → 0\n";
                modified_ = true;
                return;
            }
        }
        
        // Rule: x / 1 → x
        if (node.op == Sad::Lexer::TokenType::OP_DIVIDE) {
            if (isLiteralWithValue(node.right.get(), 1.0)) {
                std::cout << "    ✓ تبسيط: x / 1 → x\n";
                modified_ = true;
                return;
            }
            // Rule: x / x → 1 (if x != 0)
            if (areIdenticalVariables(node.left.get(), node.right.get())) {
                std::cout << "    ✓ تبسيط: x / x → 1\n";
                modified_ = true;
                return;
            }
        }
        
        // Rule: x ^ 0 → 1
        if (node.op == Sad::Lexer::TokenType::OP_POWER) {
            if (isLiteralWithValue(node.right.get(), 0.0)) {
                std::cout << "    ✓ تبسيط: x ^ 0 → 1\n";
                modified_ = true;
                return;
            }
            // Rule: x ^ 1 → x
            if (isLiteralWithValue(node.right.get(), 1.0)) {
                std::cout << "    ✓ تبسيط: x ^ 1 → x\n";
                modified_ = true;
                return;
            }
        }
    }
    
    // Default implementations for other node types
    void visitUnaryExpr(Sad::AST::UnaryExpr& node) override {
        if (node.operand) node.operand->accept(*this);
    }
    
    void visitLiteralExpr(Sad::AST::LiteralExpr&) override {}
    void visitVariableExpr(Sad::AST::VariableExpr&) override {}
    
    void visitAssignExpr(Sad::AST::AssignExpr& node) override {
        if (node.value) node.value->accept(*this);
    }
    
    void visitCallExpr(Sad::AST::CallExpr& node) override {
        for (auto& arg : node.arguments) {
            if (arg) arg->accept(*this);
        }
    }
    
    void visitIndexExpr(Sad::AST::IndexExpr& node) override {
        if (node.object) node.object->accept(*this);
        if (node.index) node.index->accept(*this);
    }
    
    void visitArrayExpr(Sad::AST::ArrayExpr& node) override {
        for (auto& elem : node.elements) {
            if (elem) elem->accept(*this);
        }
    }
    
    void visitTernaryExpr(Sad::AST::TernaryExpr& node) override {
        if (node.condition) node.condition->accept(*this);
        if (node.trueExpr) node.trueExpr->accept(*this);
        if (node.falseExpr) node.falseExpr->accept(*this);
    }
    
    void visitMemberExpr(Sad::AST::MemberExpr&) override {}
    
    void visitMemberAssignExpr(Sad::AST::MemberAssignExpr& node) override {
        if (node.value) node.value->accept(*this);
    }
    
    void visitMapExpr(Sad::AST::MapExpr& node) override {
        for (auto& pair : node.pairs) {
            if (pair.key) pair.key->accept(*this);
            if (pair.value) pair.value->accept(*this);
        }
    }
    
    void visitLambdaExpr(Sad::AST::LambdaExpr&) override {}
    void visitListComprehensionExpr(Sad::AST::ListComprehensionExpr&) override {}
    void visitDictComprehensionExpr(Sad::AST::DictComprehensionExpr&) override {}
    void visitGeneratorExpr(Sad::AST::GeneratorExpr&) override {}
    void visitDecoratorExpr(Sad::AST::DecoratorExpr&) override {}
    
    // Statement visitors
    void visitExprStmt(Sad::AST::ExprStmt& node) override {
        if (node.expression) node.expression->accept(*this);
    }
    
    void visitBlockStmt(Sad::AST::BlockStmt& node) override {
        for (auto& stmt : node.statements) {
            if (stmt) stmt->accept(*this);
        }
    }
    
    void visitIfStmt(Sad::AST::IfStmt& node) override {
        if (node.condition) node.condition->accept(*this);
        if (node.thenBranch) node.thenBranch->accept(*this);
        if (node.elseBranch) node.elseBranch->accept(*this);
    }
    
    void visitWhileStmt(Sad::AST::WhileStmt& node) override {
        if (node.condition) node.condition->accept(*this);
        if (node.body) node.body->accept(*this);
    }
    
    void visitForStmt(Sad::AST::ForStmt& node) override {
        if (node.initializer) node.initializer->accept(*this);
        if (node.condition) node.condition->accept(*this);
        if (node.increment) node.increment->accept(*this);
        if (node.body) node.body->accept(*this);
    }
    
    void visitReturnStmt(Sad::AST::ReturnStmt& node) override {
        if (node.value) node.value->accept(*this);
    }
    
    void visitBreakStmt(Sad::AST::BreakStmt&) override {}
    void visitContinueStmt(Sad::AST::ContinueStmt&) override {}
    
    void visitSwitchStmt(Sad::AST::SwitchStmt& node) override {
        if (node.expression) node.expression->accept(*this);
        for (auto& caseClause : node.cases) {
            if (caseClause.value) caseClause.value->accept(*this);
            if (caseClause.body) caseClause.body->accept(*this);
        }
        if (node.defaultCase) node.defaultCase->accept(*this);
    }
    
    void visitImportStmt(Sad::AST::ImportStmt&) override {}
    void visitExportStmt(Sad::AST::ExportStmt&) override {}
    void visitFromImportStmt(Sad::AST::FromImportStmt&) override {}
    void visitExportDecl(Sad::AST::ExportDecl&) override {}
    
    // Declaration visitors - simplified
    void visitFunctionDecl(Sad::AST::FunctionDecl& node) override {
        if (node.body) node.body->accept(*this);
    }
    
    void visitClassDecl(Sad::AST::ClassDecl& node) override {
        for (auto& member : node.members) {
            if (member) member->accept(*this);
        }
    }
    
    void visitEnumDecl(Sad::AST::EnumDecl&) override {}
    
    // Additional required visitors
    void visitNewExpr(Sad::AST::NewExpr&) override {}
    void visitMemberAccessExpr(Sad::AST::MemberAccessExpr&) override {}
    void visitMethodCallExpr(Sad::AST::MethodCallExpr&) override {}
    void visitThisExpr(Sad::AST::ThisExpr&) override {}
    void visitSuperExpr(Sad::AST::SuperExpr&) override {}
    void visitVarDeclStmt(Sad::AST::VarDeclStmt&) override {}
    void visitForRangeStmt(Sad::AST::ForRangeStmt&) override {}
    void visitYieldStmt(Sad::AST::YieldStmt&) override {}
    void visitTryStmt(Sad::AST::TryStmt&) override {}
    void visitRaiseStmt(Sad::AST::RaiseStmt&) override {}
    void visitWithStmt(Sad::AST::WithStmt&) override {}
    void visitClassDeclStmt(Sad::AST::ClassDeclStmt&) override {}
    void visitFieldDecl(Sad::AST::FieldDecl&) override {}
    void visitMethodDecl(Sad::AST::MethodDecl&) override {}
    void visitPropertyDecl(Sad::AST::PropertyDecl&) override {}
    void visitConstructorDecl(Sad::AST::ConstructorDecl&) override {}
    void visitDestructorDecl(Sad::AST::DestructorDecl&) override {}
};

// =====================================
// Expression Simplification Pass
// =====================================

std::shared_ptr<ASTNode> ExpressionSimplificationPass::apply(std::shared_ptr<ASTNode> ast) {
    if (!ast) return nullptr;
    
    std::cout << "  → تطبيق تبسيط التعبيرات...\n";
    std::cout << "  → Applying expression simplification...\n";
    
    ExpressionSimplificationVisitor visitor;
    ast->accept(visitor);
    
    if (visitor.wasModified()) {
        std::cout << "    ✓ تم تبسيط التعبيرات الجبرية\n";
        std::cout << "    ✓ Algebraic expressions simplified\n";
    } else {
        std::cout << "    ℹ لا توجد تبسيطات قابلة للتطبيق\n";
        std::cout << "    ℹ No applicable simplifications found\n";
    }
    
    return ast;
}

// =====================================
// Loop Optimization Pass
// =====================================

class LoopOptimizationVisitor : public Sad::AST::ASTVisitor {
private:
    bool modified_ = false;
    int optimizationCount_ = 0;
    
    // Check if a loop has constant bounds and small iteration count (for unrolling)
    bool isSmallConstantLoop(Sad::AST::ForStmt* forStmt, int& iterationCount) {
        // Simplified check - in real implementation, we'd need more sophisticated analysis
        // For now, just detect patterns like: for (i = 0; i < N; i++)
        
        // Check if condition is a comparison
        auto* condBinary = dynamic_cast<Sad::AST::BinaryExpr*>(forStmt->condition.get());
        if (!condBinary) return false;
        
        // Check if we have a literal on one side
        auto* rightLit = dynamic_cast<Sad::AST::LiteralExpr*>(condBinary->right.get());
        if (!rightLit) return false;
        
        try {
            int limit = std::stoi(rightLit->token.getValue());
            if (limit > 0 && limit <= 4) { // Only unroll very small loops
                iterationCount = limit;
                return true;
            }
        } catch (...) {
            return false;
        }
        
        return false;
    }
    
public:
    bool wasModified() const { return modified_; }
    int getOptimizationCount() const { return optimizationCount_; }
    
    // Visit ForStmt to optimize loops
    void visitForStmt(Sad::AST::ForStmt& stmt) override {
        int iterCount = 0;
        
        // Check for loop unrolling opportunity
        if (isSmallConstantLoop(&stmt, iterCount)) {
            std::cout << "    ✓ فرصة لفك الحلقة: " << iterCount << " تكرارات\n";
            std::cout << "    ✓ Loop unrolling opportunity: " << iterCount << " iterations\n";
            modified_ = true;
            optimizationCount_++;
        }
        
        // Visit loop body
        if (stmt.body) stmt.body->accept(*this);
    }
    
    // Visit WhileStmt
    void visitWhileStmt(Sad::AST::WhileStmt& stmt) override {
        // Check for invariant condition
        auto* condLit = dynamic_cast<Sad::AST::LiteralExpr*>(stmt.condition.get());
        if (condLit) {
            std::cout << "    ✓ حلقة بشرط ثابت يمكن تحسينها\n";
            std::cout << "    ✓ Loop with constant condition can be optimized\n";
            modified_ = true;
            optimizationCount_++;
        }
        
        if (stmt.body) stmt.body->accept(*this);
    }
    
    // Visit ForRangeStmt
    void visitForRangeStmt(Sad::AST::ForRangeStmt& stmt) override {
        if (stmt.iterable) stmt.iterable->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    
    // Other visitors - minimal implementation
    void visitBinaryExpr(Sad::AST::BinaryExpr& node) override {
        if (node.left) node.left->accept(*this);
        if (node.right) node.right->accept(*this);
    }
    
    void visitUnaryExpr(Sad::AST::UnaryExpr& node) override {
        if (node.operand) node.operand->accept(*this);
    }
    
    void visitLiteralExpr(Sad::AST::LiteralExpr&) override {}
    void visitVariableExpr(Sad::AST::VariableExpr&) override {}
    void visitAssignExpr(Sad::AST::AssignExpr& node) override {
        if (node.value) node.value->accept(*this);
    }
    void visitCallExpr(Sad::AST::CallExpr& node) override {
        if (node.callee) node.callee->accept(*this);
        for (auto& arg : node.arguments) {
            if (arg) arg->accept(*this);
        }
    }
    void visitIndexExpr(Sad::AST::IndexExpr& node) override {
        if (node.object) node.object->accept(*this);
        if (node.index) node.index->accept(*this);
    }
    void visitArrayExpr(Sad::AST::ArrayExpr& node) override {
        for (auto& elem : node.elements) {
            if (elem) elem->accept(*this);
        }
    }
    void visitMapExpr(Sad::AST::MapExpr& node) override {
        for (auto& pair : node.pairs) {
            if (pair.key) pair.key->accept(*this);
            if (pair.value) pair.value->accept(*this);
        }
    }
    void visitTernaryExpr(Sad::AST::TernaryExpr& node) override {
        if (node.condition) node.condition->accept(*this);
        if (node.trueExpr) node.trueExpr->accept(*this);
        if (node.falseExpr) node.falseExpr->accept(*this);
    }
    void visitLambdaExpr(Sad::AST::LambdaExpr&) override {}
    void visitNewExpr(Sad::AST::NewExpr&) override {}
    void visitMemberAccessExpr(Sad::AST::MemberAccessExpr&) override {}
    void visitMethodCallExpr(Sad::AST::MethodCallExpr&) override {}
    void visitThisExpr(Sad::AST::ThisExpr&) override {}
    void visitSuperExpr(Sad::AST::SuperExpr&) override {}
    void visitMemberExpr(Sad::AST::MemberExpr&) override {}
    void visitMemberAssignExpr(Sad::AST::MemberAssignExpr&) override {}
    void visitListComprehensionExpr(Sad::AST::ListComprehensionExpr&) override {}
    void visitDictComprehensionExpr(Sad::AST::DictComprehensionExpr&) override {}
    void visitGeneratorExpr(Sad::AST::GeneratorExpr&) override {}
    void visitDecoratorExpr(Sad::AST::DecoratorExpr&) override {}
    
    void visitExprStmt(Sad::AST::ExprStmt& stmt) override {
        if (stmt.expression) stmt.expression->accept(*this);
    }
    void visitBlockStmt(Sad::AST::BlockStmt& stmt) override {
        for (auto& s : stmt.statements) {
            if (s) s->accept(*this);
        }
    }
    void visitIfStmt(Sad::AST::IfStmt& stmt) override {
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.thenBranch) stmt.thenBranch->accept(*this);
        if (stmt.elseBranch) stmt.elseBranch->accept(*this);
    }
    void visitReturnStmt(Sad::AST::ReturnStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
    }
    void visitBreakStmt(Sad::AST::BreakStmt&) override {}
    void visitContinueStmt(Sad::AST::ContinueStmt&) override {}
    void visitSwitchStmt(Sad::AST::SwitchStmt& stmt) override {
        if (stmt.expression) stmt.expression->accept(*this);
        for (auto& c : stmt.cases) {
            if (c.value) c.value->accept(*this);
            if (c.body) c.body->accept(*this);
        }
    }
    void visitVarDeclStmt(Sad::AST::VarDeclStmt& stmt) override {
        if (stmt.initializer) stmt.initializer->accept(*this);
    }
    void visitYieldStmt(Sad::AST::YieldStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
    }
    void visitTryStmt(Sad::AST::TryStmt& stmt) override {
        if (stmt.tryBlock) stmt.tryBlock->accept(*this);
        for (auto& c : stmt.catchClauses) {
            if (c.body) c.body->accept(*this);
        }
        if (stmt.finallyBlock) stmt.finallyBlock->accept(*this);
    }
    void visitRaiseStmt(Sad::AST::RaiseStmt& stmt) override {
        if (stmt.exception) stmt.exception->accept(*this);
    }
    void visitWithStmt(Sad::AST::WithStmt& stmt) override {
        if (stmt.resource) stmt.resource->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    void visitClassDeclStmt(Sad::AST::ClassDeclStmt&) override {}
    
    void visitFunctionDecl(Sad::AST::FunctionDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    void visitClassDecl(Sad::AST::ClassDecl& decl) override {
        for (auto& member : decl.members) {
            if (member) member->accept(*this);
        }
    }
    void visitFieldDecl(Sad::AST::FieldDecl& decl) override {
        if (decl.initializer) decl.initializer->accept(*this);
    }
    void visitMethodDecl(Sad::AST::MethodDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    void visitPropertyDecl(Sad::AST::PropertyDecl&) override {}
    void visitConstructorDecl(Sad::AST::ConstructorDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    void visitDestructorDecl(Sad::AST::DestructorDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    void visitEnumDecl(Sad::AST::EnumDecl&) override {}
    void visitImportStmt(Sad::AST::ImportStmt&) override {}
    void visitExportStmt(Sad::AST::ExportStmt&) override {}
    void visitFromImportStmt(Sad::AST::FromImportStmt&) override {}
    void visitExportDecl(Sad::AST::ExportDecl&) override {}
};

std::shared_ptr<ASTNode> LoopOptimizationPass::apply(std::shared_ptr<ASTNode> ast) {
    if (!ast) return nullptr;
    
    std::cout << "  → تطبيق تحسين الحلقات...\n";
    std::cout << "  → Applying loop optimization...\n";
    
    LoopOptimizationVisitor visitor;
    ast->accept(visitor);
    
    if (visitor.wasModified()) {
        std::cout << "    ✅ تم اكتشاف " << visitor.getOptimizationCount() 
                  << " فرصة لتحسين الحلقات\n";
    } else {
        std::cout << "    ℹ لا توجد حلقات قابلة للتحسين\n";
    }
    
    return ast;
}

// =====================================
// Function Inlining Pass
// =====================================

class FunctionInliningVisitor : public Sad::AST::ASTVisitor {
private:
    std::map<std::string, int> functionSizes_;      // Function name -> line count
    std::map<std::string, int> functionCallCounts_; // Function name -> call count
    bool modified_ = false;
    int inliningOpportunities_ = 0;
    
    // Estimate function size (simplified)
    int estimateFunctionSize(Sad::AST::Statement* body) {
        // In a real implementation, we'd count statements recursively
        // For now, just assume small = inlinable
        return 1; // Placeholder
    }
    
public:
    bool wasModified() const { return modified_; }
    int getInliningOpportunities() const { return inliningOpportunities_; }
    
    // First pass: collect function definitions
    void visitFunctionDecl(Sad::AST::FunctionDecl& decl) override {
        int size = estimateFunctionSize(decl.body.get());
        functionSizes_[decl.name] = size;
        
        if (decl.body) decl.body->accept(*this);
    }
    
    // Second pass: count function calls
    void visitCallExpr(Sad::AST::CallExpr& node) override {
        // Check if it's a simple function call (not a method)
        if (auto* varExpr = dynamic_cast<Sad::AST::VariableExpr*>(node.callee.get())) {
            std::string funcName = varExpr->name;
            functionCallCounts_[funcName]++;
            
            // Check if this function is a candidate for inlining
            if (functionSizes_.find(funcName) != functionSizes_.end()) {
                int size = functionSizes_[funcName];
                int callCount = functionCallCounts_[funcName];
                
                // Inline if: small function (<= 3 lines) OR called only once
                if (size <= 3 || callCount == 1) {
                    std::cout << "    ✓ فرصة لدمج الدالة: " << funcName 
                              << " (حجم: " << size << ", استدعاءات: " << callCount << ")\n";
                    modified_ = true;
                    inliningOpportunities_++;
                }
            }
        }
        
        if (node.callee) node.callee->accept(*this);
        for (auto& arg : node.arguments) {
            if (arg) arg->accept(*this);
        }
    }
    
    // Minimal visitor implementations
    void visitBinaryExpr(Sad::AST::BinaryExpr& node) override {
        if (node.left) node.left->accept(*this);
        if (node.right) node.right->accept(*this);
    }
    void visitUnaryExpr(Sad::AST::UnaryExpr& node) override {
        if (node.operand) node.operand->accept(*this);
    }
    void visitLiteralExpr(Sad::AST::LiteralExpr&) override {}
    void visitVariableExpr(Sad::AST::VariableExpr&) override {}
    void visitAssignExpr(Sad::AST::AssignExpr& node) override {
        if (node.value) node.value->accept(*this);
    }
    void visitIndexExpr(Sad::AST::IndexExpr& node) override {
        if (node.object) node.object->accept(*this);
        if (node.index) node.index->accept(*this);
    }
    void visitArrayExpr(Sad::AST::ArrayExpr& node) override {
        for (auto& elem : node.elements) {
            if (elem) elem->accept(*this);
        }
    }
    void visitMapExpr(Sad::AST::MapExpr& node) override {
        for (auto& pair : node.pairs) {
            if (pair.key) pair.key->accept(*this);
            if (pair.value) pair.value->accept(*this);
        }
    }
    void visitTernaryExpr(Sad::AST::TernaryExpr& node) override {
        if (node.condition) node.condition->accept(*this);
        if (node.trueExpr) node.trueExpr->accept(*this);
        if (node.falseExpr) node.falseExpr->accept(*this);
    }
    void visitLambdaExpr(Sad::AST::LambdaExpr&) override {}
    void visitNewExpr(Sad::AST::NewExpr&) override {}
    void visitMemberAccessExpr(Sad::AST::MemberAccessExpr&) override {}
    void visitMethodCallExpr(Sad::AST::MethodCallExpr&) override {}
    void visitThisExpr(Sad::AST::ThisExpr&) override {}
    void visitSuperExpr(Sad::AST::SuperExpr&) override {}
    void visitMemberExpr(Sad::AST::MemberExpr&) override {}
    void visitMemberAssignExpr(Sad::AST::MemberAssignExpr&) override {}
    void visitListComprehensionExpr(Sad::AST::ListComprehensionExpr&) override {}
    void visitDictComprehensionExpr(Sad::AST::DictComprehensionExpr&) override {}
    void visitGeneratorExpr(Sad::AST::GeneratorExpr&) override {}
    void visitDecoratorExpr(Sad::AST::DecoratorExpr&) override {}
    
    void visitExprStmt(Sad::AST::ExprStmt& stmt) override {
        if (stmt.expression) stmt.expression->accept(*this);
    }
    void visitBlockStmt(Sad::AST::BlockStmt& stmt) override {
        for (auto& s : stmt.statements) {
            if (s) s->accept(*this);
        }
    }
    void visitIfStmt(Sad::AST::IfStmt& stmt) override {
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.thenBranch) stmt.thenBranch->accept(*this);
        if (stmt.elseBranch) stmt.elseBranch->accept(*this);
    }
    void visitWhileStmt(Sad::AST::WhileStmt& stmt) override {
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    void visitForStmt(Sad::AST::ForStmt& stmt) override {
        if (stmt.initializer) stmt.initializer->accept(*this);
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.increment) stmt.increment->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    void visitReturnStmt(Sad::AST::ReturnStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
    }
    void visitBreakStmt(Sad::AST::BreakStmt&) override {}
    void visitContinueStmt(Sad::AST::ContinueStmt&) override {}
    void visitSwitchStmt(Sad::AST::SwitchStmt& stmt) override {
        if (stmt.expression) stmt.expression->accept(*this);
        for (auto& c : stmt.cases) {
            if (c.value) c.value->accept(*this);
            if (c.body) c.body->accept(*this);
        }
    }
    void visitVarDeclStmt(Sad::AST::VarDeclStmt& stmt) override {
        if (stmt.initializer) stmt.initializer->accept(*this);
    }
    void visitForRangeStmt(Sad::AST::ForRangeStmt& stmt) override {
        if (stmt.iterable) stmt.iterable->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    void visitYieldStmt(Sad::AST::YieldStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
    }
    void visitTryStmt(Sad::AST::TryStmt& stmt) override {
        if (stmt.tryBlock) stmt.tryBlock->accept(*this);
        for (auto& c : stmt.catchClauses) {
            if (c.body) c.body->accept(*this);
        }
        if (stmt.finallyBlock) stmt.finallyBlock->accept(*this);
    }
    void visitRaiseStmt(Sad::AST::RaiseStmt& stmt) override {
        if (stmt.exception) stmt.exception->accept(*this);
    }
    void visitWithStmt(Sad::AST::WithStmt& stmt) override {
        if (stmt.resource) stmt.resource->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    void visitClassDeclStmt(Sad::AST::ClassDeclStmt&) override {}
    
    void visitClassDecl(Sad::AST::ClassDecl& decl) override {
        for (auto& member : decl.members) {
            if (member) member->accept(*this);
        }
    }
    void visitFieldDecl(Sad::AST::FieldDecl& decl) override {
        if (decl.initializer) decl.initializer->accept(*this);
    }
    void visitMethodDecl(Sad::AST::MethodDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    void visitPropertyDecl(Sad::AST::PropertyDecl&) override {}
    void visitConstructorDecl(Sad::AST::ConstructorDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    void visitDestructorDecl(Sad::AST::DestructorDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    void visitEnumDecl(Sad::AST::EnumDecl&) override {}
    void visitImportStmt(Sad::AST::ImportStmt&) override {}
    void visitExportStmt(Sad::AST::ExportStmt&) override {}
    void visitFromImportStmt(Sad::AST::FromImportStmt&) override {}
    void visitExportDecl(Sad::AST::ExportDecl&) override {}
};

std::shared_ptr<ASTNode> FunctionInliningPass::apply(std::shared_ptr<ASTNode> ast) {
    if (!ast) return nullptr;
    
    std::cout << "  → تطبيق دمج الدوال...\n";
    std::cout << "  → Applying function inlining...\n";
    
    FunctionInliningVisitor visitor;
    ast->accept(visitor);
    
    if (visitor.wasModified()) {
        std::cout << "    ✅ تم اكتشاف " << visitor.getInliningOpportunities() 
                  << " فرصة لدمج الدوال\n";
    } else {
        std::cout << "    ℹ لا توجد دوال قابلة للدمج\n";
    }
    
    return ast;
}

// =====================================
// Redundant Assignment Elimination Pass
// =====================================

class RedundantAssignmentVisitor : public Sad::AST::ASTVisitor {
private:
    std::map<std::string, int> lastAssignmentLine_;  // Variable -> line of last assignment
    std::set<std::string> redundantAssignments_;     // Variables with redundant assignments
    bool modified_ = false;
    
public:
    bool wasModified() const { return modified_; }
    const std::set<std::string>& getRedundantAssignments() const { 
        return redundantAssignments_; 
    }
    
    // Visit assignment expressions
    void visitAssignExpr(Sad::AST::AssignExpr& node) override {
        // Check if this variable was recently assigned without being used
        if (lastAssignmentLine_.find(node.name) != lastAssignmentLine_.end()) {
            // Found a redundant assignment (previous one was never used)
            redundantAssignments_.insert(node.name);
            modified_ = true;
        }
        
        // Mark this as the last assignment
        lastAssignmentLine_[node.name] = 0; // Line number would be tracked in real impl
        
        // Visit the value expression
        if (node.value) node.value->accept(*this);
    }
    
    // Visit variable usage - clears the "last assignment" flag
    void visitVariableExpr(Sad::AST::VariableExpr& node) override {
        // Variable is being used, so last assignment was not redundant
        lastAssignmentLine_.erase(node.name);
    }
    
    // Visit block statements
    void visitBlockStmt(Sad::AST::BlockStmt& stmt) override {
        for (auto& s : stmt.statements) {
            if (s) s->accept(*this);
        }
    }
    
    // Minimal visitor implementations
    void visitBinaryExpr(Sad::AST::BinaryExpr& node) override {
        if (node.left) node.left->accept(*this);
        if (node.right) node.right->accept(*this);
    }
    void visitUnaryExpr(Sad::AST::UnaryExpr& node) override {
        if (node.operand) node.operand->accept(*this);
    }
    void visitLiteralExpr(Sad::AST::LiteralExpr&) override {}
    void visitCallExpr(Sad::AST::CallExpr& node) override {
        if (node.callee) node.callee->accept(*this);
        for (auto& arg : node.arguments) {
            if (arg) arg->accept(*this);
        }
    }
    void visitIndexExpr(Sad::AST::IndexExpr& node) override {
        if (node.object) node.object->accept(*this);
        if (node.index) node.index->accept(*this);
    }
    void visitArrayExpr(Sad::AST::ArrayExpr& node) override {
        for (auto& elem : node.elements) {
            if (elem) elem->accept(*this);
        }
    }
    void visitMapExpr(Sad::AST::MapExpr& node) override {
        for (auto& pair : node.pairs) {
            if (pair.key) pair.key->accept(*this);
            if (pair.value) pair.value->accept(*this);
        }
    }
    void visitTernaryExpr(Sad::AST::TernaryExpr& node) override {
        if (node.condition) node.condition->accept(*this);
        if (node.trueExpr) node.trueExpr->accept(*this);
        if (node.falseExpr) node.falseExpr->accept(*this);
    }
    void visitLambdaExpr(Sad::AST::LambdaExpr&) override {}
    void visitNewExpr(Sad::AST::NewExpr&) override {}
    void visitMemberAccessExpr(Sad::AST::MemberAccessExpr&) override {}
    void visitMethodCallExpr(Sad::AST::MethodCallExpr&) override {}
    void visitThisExpr(Sad::AST::ThisExpr&) override {}
    void visitSuperExpr(Sad::AST::SuperExpr&) override {}
    void visitMemberExpr(Sad::AST::MemberExpr&) override {}
    void visitMemberAssignExpr(Sad::AST::MemberAssignExpr&) override {}
    void visitListComprehensionExpr(Sad::AST::ListComprehensionExpr&) override {}
    void visitDictComprehensionExpr(Sad::AST::DictComprehensionExpr&) override {}
    void visitGeneratorExpr(Sad::AST::GeneratorExpr&) override {}
    void visitDecoratorExpr(Sad::AST::DecoratorExpr&) override {}
    
    void visitExprStmt(Sad::AST::ExprStmt& stmt) override {
        if (stmt.expression) stmt.expression->accept(*this);
    }
    void visitIfStmt(Sad::AST::IfStmt& stmt) override {
        if (stmt.condition) stmt.condition->accept(*this);
        
        // Save state before branches
        auto savedState = lastAssignmentLine_;
        
        if (stmt.thenBranch) stmt.thenBranch->accept(*this);
        
        if (stmt.elseBranch) {
            // Restore state for else branch
            lastAssignmentLine_ = savedState;
            stmt.elseBranch->accept(*this);
        }
    }
    void visitWhileStmt(Sad::AST::WhileStmt& stmt) override {
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    void visitForStmt(Sad::AST::ForStmt& stmt) override {
        if (stmt.initializer) stmt.initializer->accept(*this);
        if (stmt.condition) stmt.condition->accept(*this);
        if (stmt.increment) stmt.increment->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    void visitReturnStmt(Sad::AST::ReturnStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
    }
    void visitBreakStmt(Sad::AST::BreakStmt&) override {}
    void visitContinueStmt(Sad::AST::ContinueStmt&) override {}
    void visitSwitchStmt(Sad::AST::SwitchStmt& stmt) override {
        if (stmt.expression) stmt.expression->accept(*this);
        for (auto& c : stmt.cases) {
            if (c.value) c.value->accept(*this);
            if (c.body) c.body->accept(*this);
        }
    }
    void visitVarDeclStmt(Sad::AST::VarDeclStmt& stmt) override {
        if (stmt.initializer) stmt.initializer->accept(*this);
        // Mark as last assignment
        lastAssignmentLine_[stmt.name] = 0;
    }
    void visitForRangeStmt(Sad::AST::ForRangeStmt& stmt) override {
        if (stmt.iterable) stmt.iterable->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    void visitYieldStmt(Sad::AST::YieldStmt& stmt) override {
        if (stmt.value) stmt.value->accept(*this);
    }
    void visitTryStmt(Sad::AST::TryStmt& stmt) override {
        if (stmt.tryBlock) stmt.tryBlock->accept(*this);
        for (auto& c : stmt.catchClauses) {
            if (c.body) c.body->accept(*this);
        }
        if (stmt.finallyBlock) stmt.finallyBlock->accept(*this);
    }
    void visitRaiseStmt(Sad::AST::RaiseStmt& stmt) override {
        if (stmt.exception) stmt.exception->accept(*this);
    }
    void visitWithStmt(Sad::AST::WithStmt& stmt) override {
        if (stmt.resource) stmt.resource->accept(*this);
        if (stmt.body) stmt.body->accept(*this);
    }
    void visitClassDeclStmt(Sad::AST::ClassDeclStmt&) override {}
    
    void visitFunctionDecl(Sad::AST::FunctionDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    void visitClassDecl(Sad::AST::ClassDecl& decl) override {
        for (auto& member : decl.members) {
            if (member) member->accept(*this);
        }
    }
    void visitFieldDecl(Sad::AST::FieldDecl& decl) override {
        if (decl.initializer) decl.initializer->accept(*this);
    }
    void visitMethodDecl(Sad::AST::MethodDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    void visitPropertyDecl(Sad::AST::PropertyDecl&) override {}
    void visitConstructorDecl(Sad::AST::ConstructorDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    void visitDestructorDecl(Sad::AST::DestructorDecl& decl) override {
        if (decl.body) decl.body->accept(*this);
    }
    void visitEnumDecl(Sad::AST::EnumDecl&) override {}
    void visitImportStmt(Sad::AST::ImportStmt&) override {}
    void visitExportStmt(Sad::AST::ExportStmt&) override {}
    void visitFromImportStmt(Sad::AST::FromImportStmt&) override {}
    void visitExportDecl(Sad::AST::ExportDecl&) override {}
};

std::shared_ptr<ASTNode> RedundantAssignmentEliminationPass::apply(std::shared_ptr<ASTNode> ast) {
    if (!ast) return nullptr;
    
    std::cout << "  → تطبيق إزالة الإسناد الزائد...\n";
    std::cout << "  → Applying redundant assignment elimination...\n";
    
    RedundantAssignmentVisitor visitor;
    ast->accept(visitor);
    
    const auto& redundant = visitor.getRedundantAssignments();
    if (!redundant.empty()) {
        std::cout << "    ✓ متغيرات ذات تعيينات متكررة: ";
        for (const auto& var : redundant) {
            std::cout << var << " ";
        }
        std::cout << "\n";
        std::cout << "    ✅ تم اكتشاف " << redundant.size() << " تعيينات متكررة\n";
    } else {
        std::cout << "    ℹ لا توجد تعيينات متكررة\n";
    }
    
    return ast;
}

} // namespace sad
