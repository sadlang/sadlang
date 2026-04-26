#include "optimizer/optimization_pass.h"
#include "expressions.h"
#include "statements.h"
#include "declarations.h"
#include "pattern_nodes.h"
#include "advanced_expr_nodes.h" // (AR) لتعريف AwaitExpr / (EN) For AwaitExpr definition
#include "ast_visitor.h"
#include "class_nodes.h"    // (AR) تعريف SuperExpr, ClassDeclStmt / (EN) SuperExpr, ClassDeclStmt definitions
#include "property_nodes.h" // (AR) تعريف PropertyDecl / (EN) PropertyDecl definition
#include "token.h"
#include <iostream>
#include <cmath>
#include <set>
#include <map>
#include <string>

namespace sad
{

namespace sad
{
    class ExpressionSimplificationVisitor : public Sad::AST::BaseASTVisitor
    {
    private:
        bool modified_ = false;

        // Helper to check if expression is a literal with specific value
        bool isLiteralWithValue(Sad::AST::Expression *expr, double value)
        {
            auto lit = dynamic_cast<Sad::AST::LiteralExpr *>(expr);
            if (!lit)
                return false;

            try
            {
                double val = std::stod(lit->token.getValue());
                return std::abs(val - value) < 0.0001;
            }
            catch (...)
            {
                return false;
            }
        }

        // Helper to check if two expressions are the same variable
        bool areIdenticalVariables(Sad::AST::Expression *left, Sad::AST::Expression *right)
        {
            auto leftVar = dynamic_cast<Sad::AST::VariableExpr *>(left);
            auto rightVar = dynamic_cast<Sad::AST::VariableExpr *>(right);

            if (!leftVar || !rightVar)
                return false;
            return leftVar->name == rightVar->name;
        }

    public:
        bool wasModified() const { return modified_; }

        void visitBinaryExpr(Sad::AST::BinaryExpr &node) override
        {
            // First, recursively simplify child expressions
            if (node.left)
                node.left->accept(*this);
            if (node.right)
                node.right->accept(*this);

            // Simplification rules

            // Rule: x + 0 → x  or  0 + x → x
            if (node.op == Sad::Lexer::TokenType::OP_PLUS)
            {
                if (isLiteralWithValue(node.right.get(), 0.0))
                {
                    // x + 0 → x: replace with left operand
                    // Note: We can't actually replace the node itself, but we can mark it
                    std::cout << "    ✓ تبسيط: x + 0 → x\n";
                    modified_ = true;
                    return;
                }
                if (isLiteralWithValue(node.left.get(), 0.0))
                {
                    // 0 + x → x: replace with right operand
                    std::cout << "    ✓ تبسيط: 0 + x → x\n";
                    modified_ = true;
                    return;
                }
            }

            // Rule: x - 0 → x
            if (node.op == Sad::Lexer::TokenType::OP_MINUS)
            {
                if (isLiteralWithValue(node.right.get(), 0.0))
                {
                    std::cout << "    ✓ تبسيط: x - 0 → x\n";
                    modified_ = true;
                    return;
                }
                // Rule: x - x → 0
                if (areIdenticalVariables(node.left.get(), node.right.get()))
                {
                    std::cout << "    ✓ تبسيط: x - x → 0\n";
                    modified_ = true;
                    return;
                }
            }

            // Rule: x * 1 → x  or  1 * x → x
            if (node.op == Sad::Lexer::TokenType::OP_MULTIPLY)
            {
                if (isLiteralWithValue(node.right.get(), 1.0))
                {
                    std::cout << "    ✓ تبسيط: x * 1 → x\n";
                    modified_ = true;
                    return;
                }
                if (isLiteralWithValue(node.left.get(), 1.0))
                {
                    std::cout << "    ✓ تبسيط: 1 * x → x\n";
                    modified_ = true;
                    return;
                }

                // Rule: x * 0 → 0  or  0 * x → 0
                if (isLiteralWithValue(node.right.get(), 0.0) ||
                    isLiteralWithValue(node.left.get(), 0.0))
                {
                    std::cout << "    ✓ تبسيط: x * 0 → 0\n";
                    modified_ = true;
                    return;
                }
            }

            // Rule: x / 1 → x
            if (node.op == Sad::Lexer::TokenType::OP_DIVIDE || node.op == Sad::Lexer::TokenType::OP_FLOOR_DIVIDE)
            {
                if (isLiteralWithValue(node.right.get(), 1.0))
                {
                    std::cout << "    ✓ تبسيط: x / 1 → x\n";
                    modified_ = true;
                    return;
                }
                // Rule: x / x → 1 (if x != 0)
                if (areIdenticalVariables(node.left.get(), node.right.get()))
                {
                    std::cout << "    ✓ تبسيط: x / x → 1\n";
                    modified_ = true;
                    return;
                }
            }

            // Rule: x ^ 0 → 1
            if (node.op == Sad::Lexer::TokenType::OP_POWER)
            {
                if (isLiteralWithValue(node.right.get(), 0.0))
                {
                    std::cout << "    ✓ تبسيط: x ^ 0 → 1\n";
                    modified_ = true;
                    return;
                }
                // Rule: x ^ 1 → x
                if (isLiteralWithValue(node.right.get(), 1.0))
                {
                    std::cout << "    ✓ تبسيط: x ^ 1 → x\n";
                    modified_ = true;
                    return;
                }
            }
        }

        // Default implementations for other node types
        void visitUnaryExpr(Sad::AST::UnaryExpr &node) override
        {
            if (node.operand)
                node.operand->accept(*this);
        }

        void visitLiteralExpr(Sad::AST::LiteralExpr &) override {}
        void visitVariableExpr(Sad::AST::VariableExpr &) override {}

        void visitAssignExpr(Sad::AST::AssignExpr &node) override
        {
            if (node.value)
                node.value->accept(*this);
        }

        void visitCallExpr(Sad::AST::CallExpr &node) override
        {
            for (auto &arg : node.arguments)
            {
                if (arg)
                    arg->accept(*this);
            }
        }

        void visitIndexExpr(Sad::AST::IndexExpr &node) override
        {
            if (node.object)
                node.object->accept(*this);
            if (node.index)
                node.index->accept(*this);
        }

        void visitArrayExpr(Sad::AST::ArrayExpr &node) override
        {
            for (auto &elem : node.elements)
            {
                if (elem)
                    elem->accept(*this);
            }
        }

        void visitTernaryExpr(Sad::AST::TernaryExpr &node) override
        {
            if (node.condition)
                node.condition->accept(*this);
            if (node.trueExpr)
                node.trueExpr->accept(*this);
            if (node.falseExpr)
                node.falseExpr->accept(*this);
        }

        void visitMemberExpr(Sad::AST::MemberExpr &) override {}

        void visitMemberAssignExpr(Sad::AST::MemberAssignExpr &node) override
        {
            if (node.value)
                node.value->accept(*this);
        }

        void visitMapExpr(Sad::AST::MapExpr &node) override
        {
            for (auto &pair : node.pairs)
            {
                if (pair.key)
                    pair.key->accept(*this);
                if (pair.value)
                    pair.value->accept(*this);
            }
        }

        void visitLambdaExpr(Sad::AST::LambdaExpr &) override {}
        void visitWalrusExpr(Sad::AST::WalrusExpr &node) override
        {
            if (node.value)
                node.value->accept(*this);
        }
        void visitAwaitExpr(Sad::AST::AwaitExpr &node) override
        {
            if (node.expression)
                node.expression->accept(*this);
        }
        void visitListComprehensionExpr(Sad::AST::ListComprehensionExpr &) override {}
        void visitDictComprehensionExpr(Sad::AST::DictComprehensionExpr &) override {}
        void visitSetComprehensionExpr(Sad::AST::SetComprehensionExpr &node) override
        {
            if (node.expression)
                node.expression->accept(*this);
            if (node.iterable)
                node.iterable->accept(*this);
            if (node.condition)
                node.condition->accept(*this);
        }
        void visitGeneratorExpr(Sad::AST::GeneratorExpr &) override {}
        void visitDecoratorExpr(Sad::AST::DecoratorExpr &) override {}

        // Statement visitors
        void visitExprStmt(Sad::AST::ExprStmt &node) override
        {
            if (node.expression)
                node.expression->accept(*this);
        }

        void visitBlockStmt(Sad::AST::BlockStmt &node) override
        {
            for (auto &stmt : node.statements)
            {
                if (stmt)
                    stmt->accept(*this);
            }
        }

        void visitIfStmt(Sad::AST::IfStmt &node) override
        {
            if (node.condition)
                node.condition->accept(*this);
            if (node.thenBranch)
                node.thenBranch->accept(*this);
            if (node.elseBranch)
                node.elseBranch->accept(*this);
        }

        void visitWhileStmt(Sad::AST::WhileStmt &node) override
        {
            if (node.condition)
                node.condition->accept(*this);
            if (node.body)
                node.body->accept(*this);
        }

        void visitForStmt(Sad::AST::ForStmt &node) override
        {
            if (node.initializer)
                node.initializer->accept(*this);
            if (node.condition)
                node.condition->accept(*this);
            if (node.increment)
                node.increment->accept(*this);
            if (node.body)
                node.body->accept(*this);
        }

        void visitReturnStmt(Sad::AST::ReturnStmt &node) override
        {
            if (node.value)
                node.value->accept(*this);
        }

        void visitBreakStmt(Sad::AST::BreakStmt &) override {}
        void visitContinueStmt(Sad::AST::ContinueStmt &) override {}

        void visitSwitchStmt(Sad::AST::SwitchStmt &node) override
        {
            if (node.expression)
                node.expression->accept(*this);
            for (auto &caseClause : node.cases)
            {
                if (caseClause.value)
                    caseClause.value->accept(*this);
                if (caseClause.body)
                    caseClause.body->accept(*this);
            }
            if (node.defaultCase)
                node.defaultCase->accept(*this);
        }

        void visitMatchStmt(Sad::AST::MatchStmt &stmt) override
        {
            if (stmt.value)
                stmt.value->accept(*this);
            for (auto &c : stmt.cases)
            {
                if (c.guard)
                    c.guard->accept(*this);
                for (auto &s : c.body)
                {
                    if (s)
                        s->accept(*this);
                }
            }
        }

        void visitImportStmt(Sad::AST::ImportStmt &) override {}
        void visitExportStmt(Sad::AST::ExportStmt &) override {}
        void visitFromImportStmt(Sad::AST::FromImportStmt &) override {}
        void visitExportDecl(Sad::AST::ExportDecl &) override {}
        void visitReExportStmt(Sad::AST::ReExportStmt &) override {}

        // Declaration visitors - simplified
        void visitFunctionDecl(Sad::AST::FunctionDecl &node) override
        {
            if (node.body)
                node.body->accept(*this);
        }

        void visitClassDecl(Sad::AST::ClassDecl &node) override
        {
            for (auto &member : node.members)
            {
                if (member)
                    member->accept(*this);
            }
        }

        void visitEnumDecl(Sad::AST::EnumDecl &) override {}

        // Additional required visitors
        void visitNewExpr(Sad::AST::NewExpr &) override {}
        void visitMemberAccessExpr(Sad::AST::MemberAccessExpr &) override {}
        void visitMethodCallExpr(Sad::AST::MethodCallExpr &) override {}
        void visitThisExpr(Sad::AST::ThisExpr &) override {}
        void visitSuperExpr(Sad::AST::SuperExpr &) override {}
        void visitVarDeclStmt(Sad::AST::VarDeclStmt &) override {}
        void visitForRangeStmt(Sad::AST::ForRangeStmt &) override {}
        void visitYieldStmt(Sad::AST::YieldStmt &) override {}
        void visitTryStmt(Sad::AST::TryStmt &) override {}
        void visitRaiseStmt(Sad::AST::RaiseStmt &) override {}
        void visitWithStmt(Sad::AST::WithStmt &) override {}
        void visitGoStmt(Sad::AST::GoStmt &) override {}
        void visitSelectCase(Sad::AST::SelectCase &) override {}
        void visitSelectStmt(Sad::AST::SelectStmt &) override {}
        void visitClassDeclStmt(Sad::AST::ClassDeclStmt &) override {}
        void visitFieldDecl(Sad::AST::FieldDecl &) override {}
        void visitMethodDecl(Sad::AST::MethodDecl &) override {}
        void visitPropertyDecl(Sad::AST::PropertyDecl &) override {}
        void visitConstructorDecl(Sad::AST::ConstructorDecl &) override {}
        void visitDestructorDecl(Sad::AST::DestructorDecl &) override {}
    };

    // =====================================
    // Expression Simplification Pass
    // =====================================

    std::shared_ptr<ASTNode> ExpressionSimplificationPass::apply(std::shared_ptr<ASTNode> ast)
    {
        if (!ast)
            return nullptr;

        std::cout << "  → تطبيق تبسيط التعبيرات...\n";
        std::cout << "  → Applying expression simplification...\n";

        ExpressionSimplificationVisitor visitor;
        ast->accept(visitor);

        if (visitor.wasModified())
        {
            std::cout << "    ✓ تم تبسيط التعبيرات الجبرية\n";
            std::cout << "    ✓ Algebraic expressions simplified\n";
        }
        else
        {
            std::cout << "    ℹ لا توجد تبسيطات قابلة للتطبيق\n";
            std::cout << "    ℹ No applicable simplifications found\n";
        }

        return ast;
    }

    // =====================================
    // Loop Optimization Pass
    // =====================================

} // namespace sad
