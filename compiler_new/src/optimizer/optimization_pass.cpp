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

    // =====================================
    // Dead Code Elimination Pass
    // =====================================

    class DeadCodeEliminationVisitor : public Sad::AST::BaseASTVisitor
    {
    private:
        std::set<std::string> definedVars_; // Variables that are defined
        std::set<std::string> usedVars_;    // Variables that are used
        std::set<std::string> deadVars_;    // Variables that are unused
        bool modified_ = false;
        bool afterReturn_ = false; // Track if we're after a return statement

        // Helper to mark a variable as defined
        void defineVar(const std::string &name)
        {
            definedVars_.insert(name);
        }

        // Helper to mark a variable as used
        void useVar(const std::string &name)
        {
            usedVars_.insert(name);
        }

    public:
        bool wasModified() const { return modified_; }

        const std::set<std::string> &getDeadVars() const { return deadVars_; }

        // After first pass, compute dead variables
        void computeDeadVariables()
        {
            for (const auto &var : definedVars_)
            {
                if (usedVars_.find(var) == usedVars_.end())
                {
                    deadVars_.insert(var);
                }
            }
        }

        // Visit variable expression - mark as used
        void visitVariableExpr(Sad::AST::VariableExpr &node) override
        {
            useVar(node.name);
        }

        // Visit assignment - mark LHS as defined, visit RHS
        void visitAssignExpr(Sad::AST::AssignExpr &node) override
        {
            // Visit value first to mark used variables
            if (node.value)
                node.value->accept(*this);

            // Mark variable as defined
            defineVar(node.name);
        }

        // Visit block statement - check for unreachable code after return
        void visitBlockStmt(Sad::AST::BlockStmt &stmt) override
        {
            bool previousAfterReturn = afterReturn_;

            for (size_t i = 0; i < stmt.statements.size(); ++i)
            {
                auto &s = stmt.statements[i];
                if (!s)
                    continue;

                if (afterReturn_)
                {
                    std::cout << "    ✓ كود ميت: عبارة غير قابلة للوصول بعد return\n";
                    modified_ = true;
                    break; // Stop processing - all following statements are dead
                }

                s->accept(*this);
            }

            afterReturn_ = previousAfterReturn;
        }

        // Visit return statement - mark that we're after a return
        void visitReturnStmt(Sad::AST::ReturnStmt &stmt) override
        {
            if (stmt.value)
                stmt.value->accept(*this);
            afterReturn_ = true;
        }

        // Visit variable declaration statement
        void visitVarDeclStmt(Sad::AST::VarDeclStmt &stmt) override
        {
            defineVar(stmt.name);
            if (stmt.initializer)
                stmt.initializer->accept(*this);
        }

        // Expression visitors
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
            defineVar(node.variable);
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
        void visitExprStmt(Sad::AST::ExprStmt &stmt) override
        {
            if (stmt.expression)
                stmt.expression->accept(*this);
        }

        void visitIfStmt(Sad::AST::IfStmt &stmt) override
        {
            if (stmt.condition)
                stmt.condition->accept(*this);

            bool savedAfterReturn = afterReturn_;

            if (stmt.thenBranch)
            {
                afterReturn_ = savedAfterReturn;
                stmt.thenBranch->accept(*this);
                bool thenReturns = afterReturn_;

                if (stmt.elseBranch)
                {
                    afterReturn_ = savedAfterReturn;
                    stmt.elseBranch->accept(*this);
                    bool elseReturns = afterReturn_;

                    // Only set afterReturn if BOTH branches return
                    afterReturn_ = thenReturns && elseReturns;
                }
                else
                {
                    afterReturn_ = savedAfterReturn; // Reset if no else
                }
            }
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

        void visitForRangeStmt(Sad::AST::ForRangeStmt &stmt) override
        {
            defineVar(stmt.variable);
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
            // Mark parameters as defined
            for (const auto &param : decl.parameters)
            {
                defineVar(param.name);
            }
            if (decl.body)
            {
                afterReturn_ = false; // Reset for function body
                decl.body->accept(*this);
            }
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

    std::shared_ptr<ASTNode> DeadCodeEliminationPass::apply(std::shared_ptr<ASTNode> ast)
    {
        if (!ast)
            return nullptr;

        std::cout << "  → تطبيق حذف الكود الميت...\n";
        std::cout << "  → Applying dead code elimination...\n";

        DeadCodeEliminationVisitor visitor;
        ast->accept(visitor);

        // Compute which variables are dead
        visitor.computeDeadVariables();

        const auto &deadVars = visitor.getDeadVars();
        if (!deadVars.empty())
        {
            std::cout << "    ✓ متغيرات غير مستخدمة: ";
            for (const auto &var : deadVars)
            {
                std::cout << var << " ";
            }
            std::cout << "\n";
        }

        if (visitor.wasModified())
        {
            std::cout << "    ✅ تم حذف كود ميت\n";
        }
        else if (deadVars.empty())
        {
            std::cout << "    ℹ لا يوجد كود ميت\n";
        }

        return ast;
    }

    // =====================================
    // Constant Folding Pass
    // =====================================


} // namespace sad
