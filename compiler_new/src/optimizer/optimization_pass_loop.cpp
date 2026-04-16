#include "optimizer/optimization_pass.h"
#include "expressions.h"
#include "statements.h"
#include "declarations.h"
#include "pattern_nodes.h"
#include "advanced_expr_nodes.h"
#include "ast_visitor.h"
#include "class_nodes.h"
#include "property_nodes.h"
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
    class LoopOptimizationVisitor : public Sad::AST::BaseASTVisitor
    {
    private:
        bool modified_ = false;
        int optimizationCount_ = 0;

        // Check if a loop has constant bounds and small iteration count (for unrolling)
        bool isSmallConstantLoop(Sad::AST::ForStmt *forStmt, int &iterationCount)
        {
            // Simplified check - in real implementation, we'd need more sophisticated analysis
            // For now, just detect patterns like: for (i = 0; i < N; i++)

            // Check if condition is a comparison
            auto *condBinary = dynamic_cast<Sad::AST::BinaryExpr *>(forStmt->condition.get());
            if (!condBinary)
                return false;

            // Check if we have a literal on one side
            auto *rightLit = dynamic_cast<Sad::AST::LiteralExpr *>(condBinary->right.get());
            if (!rightLit)
                return false;

            try
            {
                int limit = std::stoi(rightLit->token.getValue());
                if (limit > 0 && limit <= 4)
                { // Only unroll very small loops
                    iterationCount = limit;
                    return true;
                }
            }
            catch (...)
            {
                return false;
            }

            return false;
        }

    public:
        bool wasModified() const { return modified_; }
        int getOptimizationCount() const { return optimizationCount_; }

        // Visit ForStmt to optimize loops
        void visitForStmt(Sad::AST::ForStmt &stmt) override
        {
            int iterCount = 0;

            // Check for loop unrolling opportunity
            if (isSmallConstantLoop(&stmt, iterCount))
            {
                std::cout << "    ✓ فرصة لفك الحلقة: " << iterCount << " تكرارات\n";
                std::cout << "    ✓ Loop unrolling opportunity: " << iterCount << " iterations\n";
                modified_ = true;
                optimizationCount_++;
            }

            // Visit loop body
            if (stmt.body)
                stmt.body->accept(*this);
        }

        // Visit WhileStmt
        void visitWhileStmt(Sad::AST::WhileStmt &stmt) override
        {
            // Check for invariant condition
            auto *condLit = dynamic_cast<Sad::AST::LiteralExpr *>(stmt.condition.get());
            if (condLit)
            {
                std::cout << "    ✓ حلقة بشرط ثابت يمكن تحسينها\n";
                std::cout << "    ✓ Loop with constant condition can be optimized\n";
                modified_ = true;
                optimizationCount_++;
            }

            if (stmt.body)
                stmt.body->accept(*this);
        }

        // Visit ForRangeStmt
        void visitForRangeStmt(Sad::AST::ForRangeStmt &stmt) override
        {
            if (stmt.iterable)
                stmt.iterable->accept(*this);
            if (stmt.body)
                stmt.body->accept(*this);
        }

        // Other visitors - minimal implementation
        void visitBinaryExpr(Sad::AST::BinaryExpr &node) override
        {
            if (node.left)
                node.left->accept(*this);
            if (node.right)
                node.right->accept(*this);
        }

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

    std::shared_ptr<ASTNode> LoopOptimizationPass::apply(std::shared_ptr<ASTNode> ast)
    {
        if (!ast)
            return nullptr;

        std::cout << "  → تطبيق تحسين الحلقات...\n";
        std::cout << "  → Applying loop optimization...\n";

        LoopOptimizationVisitor visitor;
        ast->accept(visitor);

        if (visitor.wasModified())
        {
            std::cout << "    ✅ تم اكتشاف " << visitor.getOptimizationCount()
                      << " فرصة لتحسين الحلقات\n";
        }
        else
        {
            std::cout << "    ℹ لا توجد حلقات قابلة للتحسين\n";
        }

        return ast;
    }

    // =====================================
    // Function Inlining Pass
    // =====================================


} // namespace sad

} // namespace sad
} // namespace sad

