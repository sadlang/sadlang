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
    class FunctionInliningVisitor : public Sad::AST::BaseASTVisitor
    {
    private:
        std::map<std::string, int> functionSizes_;      // Function name -> line count
        std::map<std::string, int> functionCallCounts_; // Function name -> call count
        bool modified_ = false;
        int inliningOpportunities_ = 0;

        // Estimate function size (simplified)
        int estimateFunctionSize(Sad::AST::Statement *body)
        {
            // In a real implementation, we'd count statements recursively
            // For now, just assume small = inlinable
            return 1; // Placeholder
        }

    public:
        bool wasModified() const { return modified_; }
        int getInliningOpportunities() const { return inliningOpportunities_; }

        // First pass: collect function definitions
        void visitFunctionDecl(Sad::AST::FunctionDecl &decl) override
        {
            int size = estimateFunctionSize(decl.body.get());
            functionSizes_[decl.name] = size;

            if (decl.body)
                decl.body->accept(*this);
        }

        // Second pass: count function calls
        void visitCallExpr(Sad::AST::CallExpr &node) override
        {
            // Check if it's a simple function call (not a method)
            if (auto *varExpr = dynamic_cast<Sad::AST::VariableExpr *>(node.callee.get()))
            {
                std::string funcName = varExpr->name;
                functionCallCounts_[funcName]++;

                // Check if this function is a candidate for inlining
                if (functionSizes_.find(funcName) != functionSizes_.end())
                {
                    int size = functionSizes_[funcName];
                    int callCount = functionCallCounts_[funcName];

                    // Inline if: small function (<= 3 lines) OR called only once
                    if (size <= 3 || callCount == 1)
                    {
                        std::cout << "    ✓ فرصة لدمج الدالة: " << funcName
                                  << " (حجم: " << size << ", استدعاءات: " << callCount << ")\n";
                        modified_ = true;
                        inliningOpportunities_++;
                    }
                }
            }

            if (node.callee)
                node.callee->accept(*this);
            for (auto &arg : node.arguments)
            {
                if (arg)
                    arg->accept(*this);
            }
        }

        // Minimal visitor implementations
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

    std::shared_ptr<ASTNode> FunctionInliningPass::apply(std::shared_ptr<ASTNode> ast)
    {
        if (!ast)
            return nullptr;

        std::cout << "  → تطبيق دمج الدوال...\n";
        std::cout << "  → Applying function inlining...\n";

        FunctionInliningVisitor visitor;
        ast->accept(visitor);

        if (visitor.wasModified())
        {
            std::cout << "    ✅ تم اكتشاف " << visitor.getInliningOpportunities()
                      << " فرصة لدمج الدوال\n";
        }
        else
        {
            std::cout << "    ℹ لا توجد دوال قابلة للدمج\n";
        }

        return ast;
    }

    // =====================================
    // Redundant Assignment Elimination Pass
    // =====================================

    class RedundantAssignmentVisitor : public Sad::AST::BaseASTVisitor
    {
    private:
        std::map<std::string, int> lastAssignmentLine_; // Variable -> line of last assignment
        std::set<std::string> redundantAssignments_;    // Variables with redundant assignments
        bool modified_ = false;

    public:
        bool wasModified() const { return modified_; }
        const std::set<std::string> &getRedundantAssignments() const
        {
            return redundantAssignments_;
        }

        // Visit assignment expressions
        void visitAssignExpr(Sad::AST::AssignExpr &node) override
        {
            // Check if this variable was recently assigned without being used
            if (lastAssignmentLine_.find(node.name) != lastAssignmentLine_.end())
            {
                // Found a redundant assignment (previous one was never used)
                redundantAssignments_.insert(node.name);
                modified_ = true;
            }

            // Mark this as the last assignment
            lastAssignmentLine_[node.name] = 0; // Line number would be tracked in real impl

            // Visit the value expression
            if (node.value)
                node.value->accept(*this);
        }

        // Visit variable usage - clears the "last assignment" flag
        void visitVariableExpr(Sad::AST::VariableExpr &node) override
        {
            // Variable is being used, so last assignment was not redundant
            lastAssignmentLine_.erase(node.name);
        }

        // Visit block statements
        void visitBlockStmt(Sad::AST::BlockStmt &stmt) override
        {
            for (auto &s : stmt.statements)
            {
                if (s)
                    s->accept(*this);
            }
        }

        // Minimal visitor implementations
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
        void visitIfStmt(Sad::AST::IfStmt &stmt) override
        {
            if (stmt.condition)
                stmt.condition->accept(*this);

            // Save state before branches
            auto savedState = lastAssignmentLine_;

            if (stmt.thenBranch)
                stmt.thenBranch->accept(*this);

            if (stmt.elseBranch)
            {
                // Restore state for else branch
                lastAssignmentLine_ = savedState;
                stmt.elseBranch->accept(*this);
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
            // Mark as last assignment
            lastAssignmentLine_[stmt.name] = 0;
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

    std::shared_ptr<ASTNode> RedundantAssignmentEliminationPass::apply(std::shared_ptr<ASTNode> ast)
    {
        if (!ast)
            return nullptr;

        std::cout << "  → تطبيق إزالة الإسناد الزائد...\n";
        std::cout << "  → Applying redundant assignment elimination...\n";

        RedundantAssignmentVisitor visitor;
        ast->accept(visitor);

        const auto &redundant = visitor.getRedundantAssignments();
        if (!redundant.empty())
        {
            std::cout << "    ✓ متغيرات ذات تعيينات متكررة: ";
            for (const auto &var : redundant)
            {
                std::cout << var << " ";
            }
            std::cout << "\n";
            std::cout << "    ✅ تم اكتشاف " << redundant.size() << " تعيينات متكررة\n";
        }
        else
        {
            std::cout << "    ℹ لا توجد تعيينات متكررة\n";
        }

        return ast;
    }

} // namespace sad
