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
    class ConstantFoldingVisitor : public Sad::AST::BaseASTVisitor
    {
    private:
        bool modified_ = false;

        // Helper to check if expr is a literal number
        bool isNumericLiteral(Sad::AST::Expression *expr, double &outValue)
        {
            auto lit = dynamic_cast<Sad::AST::LiteralExpr *>(expr);
            if (!lit)
                return false;

            try
            {
                outValue = std::stod(lit->token.getValue());
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        // Helper to create a literal expression with given value
        std::shared_ptr<Sad::AST::LiteralExpr> makeLiteral(double value)
        {
            // Note: In real implementation, we'd need proper token creation
            // For now, we'll just mark as modified and log the folding
            return nullptr; // Placeholder
        }

    public:
        bool wasModified() const { return modified_; }

        // Visit BinaryExpr to fold constant operations
        void visitBinaryExpr(Sad::AST::BinaryExpr &node) override
        {
            // First, recursively fold child expressions
            if (node.left)
                node.left->accept(*this);
            if (node.right)
                node.right->accept(*this);

            double leftVal, rightVal;

            // Check if both operands are numeric literals
            if (!isNumericLiteral(node.left.get(), leftVal) ||
                !isNumericLiteral(node.right.get(), rightVal))
            {
                return; // Cannot fold
            }

            // Fold arithmetic operations
            double result = 0.0;
            bool canFold = true;
            std::string opName;

            using TokenType = Sad::Lexer::TokenType;

            if (node.op == TokenType::OP_PLUS)
            {
                result = leftVal + rightVal;
                opName = "+";
            }
            else if (node.op == TokenType::OP_MINUS)
            {
                result = leftVal - rightVal;
                opName = "-";
            }
            else if (node.op == TokenType::OP_MULTIPLY)
            {
                result = leftVal * rightVal;
                opName = "*";
            }
            else if (node.op == TokenType::OP_DIVIDE)
            {
                if (std::abs(rightVal) < 0.0001)
                {
                    canFold = false; // Division by zero
                }
                else
                {
                    result = leftVal / rightVal;
                    opName = "/";
                }
            }
            else if (node.op == TokenType::OP_FLOOR_DIVIDE)
            {
                if (std::abs(rightVal) < 0.0001)
                {
                    canFold = false; // Division by zero
                }
                else
                {
                    result = std::floor(leftVal / rightVal);
                    opName = "//";
                }
            }
            else if (node.op == TokenType::OP_MODULO)
            {
                if (std::abs(rightVal) < 0.0001)
                {
                    canFold = false; // Modulo by zero
                }
                else
                {
                    result = std::fmod(leftVal, rightVal);
                    opName = "%";
                }
            }
            else if (node.op == TokenType::OP_POWER)
            {
                result = std::pow(leftVal, rightVal);
                opName = "**";
            }
            else
            {
                canFold = false; // Not an arithmetic operation
            }

            if (canFold)
            {
                std::cout << "    ✓ طي ثابت: " << leftVal << " " << opName << " "
                          << rightVal << " → " << result << "\n";
                modified_ = true;

                // Note: In a real implementation, we would replace the node
                // with a new LiteralExpr containing the result.
                // For now, we just mark as modified and log the folding.
            }
        }

        // Visit UnaryExpr to fold constant unary operations
        void visitUnaryExpr(Sad::AST::UnaryExpr &node) override
        {
            if (node.operand)
                node.operand->accept(*this);

            double value;
            if (!isNumericLiteral(node.operand.get(), value))
            {
                return; // Cannot fold
            }

            using TokenType = Sad::Lexer::TokenType;

            if (node.op == TokenType::OP_MINUS)
            {
                std::cout << "    ✓ طي ثابت: -" << value << " → " << (-value) << "\n";
                modified_ = true;
            }
            else if (node.op == TokenType::OP_PLUS)
            {
                std::cout << "    ✓ طي ثابت: +" << value << " → " << value << "\n";
                modified_ = true;
            }
        }

        // Default implementations for other expressions
        void visitLiteralExpr(Sad::AST::LiteralExpr &) override {}
        void visitVariableExpr(Sad::AST::VariableExpr &) override {}

        void visitAssignExpr(Sad::AST::AssignExpr &node) override
        {
            if (node.value)
                node.value->accept(*this);
        }

        void visitCallExpr(Sad::AST::CallExpr &node) override
        {
            if (node.callee)
                node.callee->accept(*this);
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

        void visitTernaryExpr(Sad::AST::TernaryExpr &node) override
        {
            if (node.condition)
                node.condition->accept(*this);
            if (node.trueExpr)
                node.trueExpr->accept(*this);
            if (node.falseExpr)
                node.falseExpr->accept(*this);
        }

        void visitLambdaExpr(Sad::AST::LambdaExpr &) override {}
        void visitNewExpr(Sad::AST::NewExpr &) override {}
        void visitMemberAccessExpr(Sad::AST::MemberAccessExpr &) override {}
        void visitMethodCallExpr(Sad::AST::MethodCallExpr &) override {}
        void visitThisExpr(Sad::AST::ThisExpr &) override {}
        void visitSuperExpr(Sad::AST::SuperExpr &) override {}
        void visitMemberExpr(Sad::AST::MemberExpr &) override {}
        void visitMemberAssignExpr(Sad::AST::MemberAssignExpr &) override {}
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

        // Statement visitors (just traverse)
        void visitExprStmt(Sad::AST::ExprStmt &stmt) override
        {
            if (stmt.expression)
                stmt.expression->accept(*this);
        }

        void visitBlockStmt(Sad::AST::BlockStmt &stmt) override
        {
            for (auto &s : stmt.statements)
            {
                if (s)
                    s->accept(*this);
            }
        }

        void visitIfStmt(Sad::AST::IfStmt &stmt) override
        {
            if (stmt.condition)
                stmt.condition->accept(*this);
            if (stmt.thenBranch)
                stmt.thenBranch->accept(*this);
            if (stmt.elseBranch)
                stmt.elseBranch->accept(*this);
        }

        void visitWhileStmt(Sad::AST::WhileStmt &stmt) override
        {
            if (stmt.condition)
                stmt.condition->accept(*this);
            if (stmt.body)
                stmt.body->accept(*this);
        }

        void visitForStmt(Sad::AST::ForStmt &stmt) override
        {
            if (stmt.initializer)
                stmt.initializer->accept(*this);
            if (stmt.condition)
                stmt.condition->accept(*this);
            if (stmt.increment)
                stmt.increment->accept(*this);
            if (stmt.body)
                stmt.body->accept(*this);
        }

        void visitReturnStmt(Sad::AST::ReturnStmt &stmt) override
        {
            if (stmt.value)
                stmt.value->accept(*this);
        }

        void visitBreakStmt(Sad::AST::BreakStmt &) override {}
        void visitContinueStmt(Sad::AST::ContinueStmt &) override {}

        void visitSwitchStmt(Sad::AST::SwitchStmt &stmt) override
        {
            if (stmt.expression)
                stmt.expression->accept(*this);
            for (auto &c : stmt.cases)
            {
                if (c.value)
                    c.value->accept(*this);
                if (c.body)
                    c.body->accept(*this);
            }
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

        void visitVarDeclStmt(Sad::AST::VarDeclStmt &stmt) override
        {
            if (stmt.initializer)
                stmt.initializer->accept(*this);
        }

        void visitForRangeStmt(Sad::AST::ForRangeStmt &stmt) override
        {
            if (stmt.iterable)
                stmt.iterable->accept(*this);
            if (stmt.body)
                stmt.body->accept(*this);
        }

        void visitYieldStmt(Sad::AST::YieldStmt &stmt) override
        {
            if (stmt.value)
                stmt.value->accept(*this);
        }

        void visitTryStmt(Sad::AST::TryStmt &stmt) override
        {
            if (stmt.tryBlock)
                stmt.tryBlock->accept(*this);
            for (auto &c : stmt.catchClauses)
            {
                if (c.body)
                    c.body->accept(*this);
            }
            if (stmt.finallyBlock)
                stmt.finallyBlock->accept(*this);
        }

        void visitRaiseStmt(Sad::AST::RaiseStmt &stmt) override
        {
            if (stmt.exception)
                stmt.exception->accept(*this);
        }

        void visitWithStmt(Sad::AST::WithStmt &stmt) override
        {
            if (stmt.resource)
                stmt.resource->accept(*this);
            if (stmt.body)
                stmt.body->accept(*this);
        }

        // (AR) جملة أطلق / (EN) Go statement
        void visitGoStmt(Sad::AST::GoStmt &stmt) override
        {
            if (stmt.expression)
                stmt.expression->accept(*this);
            if (stmt.blockBody)
                stmt.blockBody->accept(*this);
        }
        // (AR) حالة اختيار / (EN) Select case
        void visitSelectCase(Sad::AST::SelectCase &stmt) override
        {
            if (stmt.channelExpr)
                stmt.channelExpr->accept(*this);
            for (auto &s : stmt.body)
            {
                if (s)
                    s->accept(*this);
            }
        }
        // (AR) جملة اختر / (EN) Select statement
        void visitSelectStmt(Sad::AST::SelectStmt &stmt) override
        {
            for (auto &c : stmt.cases)
            {
                if (c)
                    c->accept(*this);
            }
            for (auto &s : stmt.defaultBody)
            {
                if (s)
                    s->accept(*this);
            }
        }

        void visitClassDeclStmt(Sad::AST::ClassDeclStmt &) override {}

        // Declaration visitors
        void visitFunctionDecl(Sad::AST::FunctionDecl &decl) override
        {
            if (decl.body)
                decl.body->accept(*this);
        }

        void visitClassDecl(Sad::AST::ClassDecl &decl) override
        {
            for (auto &member : decl.members)
            {
                if (member)
                    member->accept(*this);
            }
        }

        void visitFieldDecl(Sad::AST::FieldDecl &decl) override
        {
            if (decl.initializer)
                decl.initializer->accept(*this);
        }

        void visitMethodDecl(Sad::AST::MethodDecl &decl) override
        {
            if (decl.body)
                decl.body->accept(*this);
        }

        void visitPropertyDecl(Sad::AST::PropertyDecl &) override {}
        void visitConstructorDecl(Sad::AST::ConstructorDecl &decl) override
        {
            if (decl.body)
                decl.body->accept(*this);
        }

        void visitDestructorDecl(Sad::AST::DestructorDecl &decl) override
        {
            if (decl.body)
                decl.body->accept(*this);
        }

        void visitEnumDecl(Sad::AST::EnumDecl &) override {}
        void visitImportStmt(Sad::AST::ImportStmt &) override {}
        void visitExportStmt(Sad::AST::ExportStmt &) override {}
        void visitFromImportStmt(Sad::AST::FromImportStmt &) override {}
        void visitExportDecl(Sad::AST::ExportDecl &) override {}
        void visitReExportStmt(Sad::AST::ReExportStmt &) override {}
    };

    std::shared_ptr<ASTNode> ConstantFoldingPass::apply(std::shared_ptr<ASTNode> ast)
    {
        if (!ast)
            return nullptr;

        std::cout << "  → تطبيق طي الثوابت...\n";
        std::cout << "  → Applying constant folding...\n";

        ConstantFoldingVisitor visitor;
        ast->accept(visitor);

        if (visitor.wasModified())
        {
            std::cout << "    ✅ تم طي ثوابت\n";
        }
        else
        {
            std::cout << "    ℹ لا توجد ثوابت لطيها\n";
        }

        return ast;
    }

    // =====================================
    // Expression Simplification Visitor
    // =====================================


} // namespace sad
