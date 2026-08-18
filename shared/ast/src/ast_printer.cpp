/**
 * @file ast_printer.cpp
 * @brief (AR) تنفيذ طابعة الشجرة النحوية التجريدية.
 *        (EN) Implementation of AST printer.
 *
 * @details
 *   (AR) يحتوي على تنفيذ جميع دوال زيارة العُقد لطباعة AST بشكل منسّق ومقروء.
 *   (EN) Contains implementation of all node visitor functions for formatted AST printing.
 *
 * @author Sad Language Team
 * @date 11 November 2025
 * @copyright Copyright (c) 2025 Sad Language Project
 */

#include "ast_printer.h"
#include "property_nodes.h"
#include "class_nodes.h"
#include "expressions.h"
#include "declarations.h"        // (AR) لتعريف TemplateFunctionDecl, NamespaceDecl, OperatorDecl / (EN) For template/namespace/operator decls
#include "advanced_expr_nodes.h" // (AR) لتعريف AwaitExpr / (EN) For AwaitExpr definition
#include "pattern_nodes.h"       // (AR) لتعريف MatchStmt / (EN) For MatchStmt definition - Source: pattern_nodes.h:375
#include "directive_nodes.h"     // (AR) لتعريف UnsafeBlockStmt, ComptimeBlockStmt, etc. / (EN) For directive nodes
#include <sstream>

namespace Sad
{
    namespace AST
    {

        // =========================================================================
        // Constructor / البناء
        // =========================================================================

        /**
         * @brief (AR) البناء الافتراضي - يُهيئ الطابعة بحالة فارغة.
         *        (EN) Default constructor - initializes printer with empty state.
         */
        ASTPrinter::ASTPrinter()
            : result_(""), indentLevel_(0)
        {
        }

        // =========================================================================
        // Public Methods / الدوال العامة
        // =========================================================================

        /**
         * @brief (AR) يطبع عقدة AST ويُرجع النص الناتج.
         *        (EN) Prints an AST node and returns the resulting text.
         *
         * @param node (AR) مؤشر للعقدة المراد طباعتها. (EN) Pointer to node to print.
         * @return (AR) النص المُنسّق الممثل للعقدة. (EN) Formatted text representation.
         *
         * @note (AR) يُعيد تعيين النتيجة قبل الزيارة.
         *       (EN) Resets result before visiting.
         */
        std::string ASTPrinter::print(ASTNode *node)
        {
            result_.clear();
            indentLevel_ = 0;

            if (node)
            {
                node->accept(*this);
            }

            return result_;
        }

        /**
         * @brief (AR) يُضيف إزاحة (مسافات) حسب المستوى الحالي.
         *        (EN) Adds indentation (spaces) based on current level.
         *
         * @return (AR) نص الإزاحة (مسافتان لكل مستوى).
         *         (EN) Indentation string (2 spaces per level).
         */
        std::string ASTPrinter::indent() const
        {
            return std::string(indentLevel_ * 2, ' ');
        }

        // =========================================================================
        // Expression Visitors / زوار التعابير
        // =========================================================================

        /**
         * @brief (AR) يزور عقدة التعبير الثنائي - يطبع العامل والمعاملين.
         *        (EN) Visits binary expression node - prints operator and operands.
         *
         * @param expr (AR) مؤشر لعقدة التعبير الثنائي. (EN) Pointer to binary expression node.
         */
        void ASTPrinter::visitBinaryExpr(BinaryExpr &expr)
        {
            result_ += "(";
            expr.left->accept(*this);
            result_ += " " + Lexer::Token::typeToString(expr.op) + " ";
            expr.right->accept(*this);
            result_ += ")";
        }

        /**
         * @brief (AR) يزور عقدة التعبير الأحادي - يطبع العامل والمعامل.
         *        (EN) Visits unary expression node - prints operator and operand.
         *
         * @param expr (AR) مؤشر لعقدة التعبير الأحادي. (EN) Pointer to unary expression node.
         */
        void ASTPrinter::visitUnaryExpr(UnaryExpr &expr)
        {
            result_ += "(";
            result_ += Lexer::Token::typeToString(expr.op);
            expr.operand->accept(*this);
            result_ += ")";
        }

        /**
         * @brief (AR) يزور عقدة التعبير الثلاثي الشرطي - يطبع بتنسيق (شرط ? صحيح : خطأ).
         *        (EN) Visits ternary conditional expression node - prints in (cond ? true : false) format.
         *
         * @param expr (AR) مؤشر لعقدة التعبير الثلاثي. (EN) Pointer to ternary expression node.
         *
         * Format: (condition ? true_value : false_value)
         * مثال: (س > 0 ? "موجب" : "سالب")
         */
        void ASTPrinter::visitTernaryExpr(TernaryExpr &expr)
        {
            result_ += "(";
            expr.condition->accept(*this);
            result_ += " ? ";
            expr.trueExpr->accept(*this);
            result_ += " : ";
            expr.falseExpr->accept(*this);
            result_ += ")";
        }

        /**
         * @brief (AR) يزور عقدة التعبير الحرفي - يطبع القيمة الحرفية.
         *        (EN) Visits literal expression node - prints literal value.
         *
         * @param expr (AR) مؤشر لعقدة التعبير الحرفي. (EN) Pointer to literal expression node.
         */
        void ASTPrinter::visitLiteralExpr(LiteralExpr &expr)
        {
            result_ += expr.token.getValue();
        }

        /**
         * @brief (AR) يزور عقدة تعبير المتغير - يطبع اسم المتغير.
         *        (EN) Visits variable expression node - prints variable name.
         *
         * @param expr (AR) مؤشر لعقدة تعبير المتغير. (EN) Pointer to variable expression node.
         */
        void ASTPrinter::visitVariableExpr(VariableExpr &expr)
        {
            result_ += expr.name;
        }

        void ASTPrinter::visitThisExpr(ThisExpr &expr)
        {
            result_ += "this";
        }

        void ASTPrinter::visitSuperExpr(SuperExpr &expr)
        {
            result_ += "super";
            if (!expr.memberName.empty())
            {
                result_ += "." + expr.memberName;
            }
        }

        /**
         * @brief (AR) يزور عقدة تعبير الإسناد - يطبع المتغير والقيمة المُسندة.
         *        (EN) Visits assignment expression node - prints variable and assigned value.
         *
         * @param expr (AR) مؤشر لعقدة تعبير الإسناد. (EN) Pointer to assignment expression node.
         */
        void ASTPrinter::visitAssignExpr(AssignExpr &expr)
        {
            result_ += "(";
            result_ += expr.name + " = ";
            expr.value->accept(*this);
            result_ += ")";
        }

        /**
         * @brief (AR) يزور عقدة استدعاء الدالة - يطبع اسم الدالة والمعاملات.
         *        (EN) Visits function call node - prints function name and arguments.
         *
         * @param expr (AR) مؤشر لعقدة استدعاء الدالة. (EN) Pointer to function call node.
         */
        void ASTPrinter::visitCallExpr(CallExpr &expr)
        {
            expr.callee->accept(*this);
            result_ += "(";

            for (size_t i = 0; i < expr.arguments.size(); ++i)
            {
                if (i > 0)
                    result_ += ", ";
                expr.arguments[i]->accept(*this);
            }

            result_ += ")";
        }

        /**
         * @brief (AR) يزور عقدة تعبير الفهرسة - يطبع الكائن والفهرس.
         *        (EN) Visits index expression node - prints object and index.
         *
         * @param expr (AR) مؤشر لعقدة تعبير الفهرسة. (EN) Pointer to index expression node.
         */
        void ASTPrinter::visitIndexExpr(IndexExpr &expr)
        {
            expr.object->accept(*this);
            result_ += "[";
            expr.index->accept(*this);
            result_ += "]";
        }

        /**
         * @brief (AR) يزور عقدة الوصول للعضو - يطبع الكائن واسم العضو.
         *        (EN) Visits member access node - prints object and member name.
         *
         * @param expr (AR) مؤشر لعقدة الوصول للعضو. (EN) Pointer to member access node.
         */
        void ASTPrinter::visitMemberExpr(MemberExpr &expr)
        {
            expr.object->accept(*this);
            result_ += "." + expr.member;
        }

        /**
         * @brief (AR) يزور عقدة تعيين قيمة لعضو - يطبع التعيين.
         *        (EN) Visits member assignment node - prints assignment.
         */
        void ASTPrinter::visitMemberAssignExpr(MemberAssignExpr &expr)
        {
            expr.object->accept(*this);
            result_ += "." + expr.member + " = ";
            expr.value->accept(*this);
        }

        void ASTPrinter::visitIndexAssignExpr(IndexAssignExpr &expr)
        {
            expr.object->accept(*this);
            result_ += "[";
            expr.index->accept(*this);
            result_ += "] = ";
            expr.value->accept(*this);
        }

        /**
         * @brief (AR) يزور عقدة المصفوفة الحرفية - يطبع العناصر.
         *        (EN) Visits array literal node - prints elements.
         *
         * @param expr (AR) مؤشر لعقدة المصفوفة الحرفية. (EN) Pointer to array literal node.
         */
        void ASTPrinter::visitArrayExpr(ArrayExpr &expr)
        {
            result_ += "[";

            for (size_t i = 0; i < expr.elements.size(); ++i)
            {
                if (i > 0)
                    result_ += ", ";
                expr.elements[i]->accept(*this);
            }

            result_ += "]";
        }

        /**
         * @brief (AR) يزور عقدة القاموس الحرفي - يطبع أزواج المفتاح-القيمة.
         *        (EN) Visits map literal node - prints key-value pairs.
         *
         * @param expr (AR) مؤشر لعقدة القاموس الحرفي. (EN) Pointer to map literal node.
         */
        void ASTPrinter::visitMapExpr(MapExpr &expr)
        {
            result_ += "{";

            for (size_t i = 0; i < expr.pairs.size(); ++i)
            {
                if (i > 0)
                    result_ += ", ";
                expr.pairs[i].key->accept(*this);
                result_ += ": ";
                expr.pairs[i].value->accept(*this);
            }

            result_ += "}";
        }

        /**

         * @brief (AR) يزور عقدة تعبير Lambda - يطبع المعاملات والجسم.
         *        (EN) Visits lambda expression node - prints parameters and body.
         *
         * @param expr (AR) مؤشر لعقدة تعبير Lambda. (EN) Pointer to lambda expression node.
         */
        void ASTPrinter::visitLambdaExpr(LambdaExpr &expr)
        {
            result_ += "(";

            for (size_t i = 0; i < expr.parameters.size(); ++i)
            {
                if (i > 0)
                    result_ += ", ";
                result_ += expr.parameters[i].name;
            }

            result_ += ") => ";
            expr.body->accept(*this);
        }

        /**
         * @brief (AR) يزور عقدة تعبير Walrus - يطبع التعيين بالإرجاع.
         *        (EN) Visits Walrus expression node - prints assignment with return.
         *
         * @param expr (AR) مؤشر لعقدة تعبير Walrus. (EN) Pointer to Walrus expression node.
         */
        void ASTPrinter::visitWalrusExpr(WalrusExpr &expr)
        {
            result_ += "(";
            result_ += expr.variable;
            result_ += " := ";
            expr.value->accept(*this);
            result_ += ")";
        }

        /**
         * @brief (AR) يزور عقدة تعبير Await - يطبع انتظار تعبير async.
         *        (EN) Visits Await expression node - prints await for async expression.
         *
         * مصدر التعريف / Source Definition: advanced_expr_nodes.h:85-120
         *
         * @param expr (AR) مؤشر لعقدة تعبير Await. (EN) Pointer to Await expression node.
         *
         * أمثلة / Examples:
         *   await fetchData()       -> "(await fetchData())"
         *   انتظر جلب_البيانات()   -> "(انتظر جلب_البيانات())"
         */
        void ASTPrinter::visitAwaitExpr(AwaitExpr &expr)
        {
            // (AR) طباعة كلمة await / (EN) Print await keyword
            result_ += "(await ";

            // (AR) زيارة التعبير المُنتظَر / (EN) Visit awaited expression
            expr.expression->accept(*this);

            // (AR) إغلاق القوس / (EN) Close parenthesis
            result_ += ")";
        }

        /**
         * @brief (AR) يزور عقدة الاستيعاب القائمي - يطبع التعبير والحلقة والشرط.
         *        (EN) Visits list comprehension node - prints expression, loop, and condition.
         *
         * @param expr (AR) مؤشر لعقدة الاستيعاب القائمي. (EN) Pointer to list comprehension node.
         */
        void ASTPrinter::visitListComprehensionExpr(ListComprehensionExpr &expr)
        {
            result_ += "[";
            expr.element->accept(*this);
            result_ += " for " + expr.variable + " in ";
            expr.iterable->accept(*this);

            if (expr.condition)
            {
                result_ += " if ";
                expr.condition->accept(*this);
            }

            result_ += "]";
        }

        /**
         * @brief (AR) يزور عقدة الاستيعاب القاموسي - يطبع المفتاح والقيمة والحلقة.
         *        (EN) Visits dict comprehension node - prints key, value, and loop.
         *
         * @param expr (AR) مؤشر لعقدة الاستيعاب القاموسي. (EN) Pointer to dict comprehension node.
         */
        void ASTPrinter::visitDictComprehensionExpr(DictComprehensionExpr &expr)
        {
            result_ += "{";
            expr.key->accept(*this);
            result_ += ": ";
            expr.value->accept(*this);
            result_ += " for " + expr.variable + " in ";
            expr.iterable->accept(*this);

            if (expr.condition)
            {
                result_ += " if ";
                expr.condition->accept(*this);
            }

            result_ += "}";
        }

        /**
         * @brief (AR) يزور عقدة الاستيعاب المجموعة - يطبع التعبير والحلقة.
         *        (EN) Visits set comprehension node - prints expression and loop.
         *
         * @param expr (AR) مؤشر لعقدة الاستيعاب المجموعة. (EN) Pointer to set comprehension node.
         */
        void ASTPrinter::visitSetComprehensionExpr(SetComprehensionExpr &expr)
        {
            result_ += "{";
            expr.expression->accept(*this);
            result_ += " for " + expr.variable + " in ";
            expr.iterable->accept(*this);

            if (expr.condition)
            {
                result_ += " if ";
                expr.condition->accept(*this);
            }

            result_ += "}";
        }

        /**
         * @brief (AR) يزور عقدة تعبير المولد - يطبع التعبير والحلقة.
         *        (EN) Visits generator expression node - prints expression and loop.
         *
         * @param expr (AR) مؤشر لعقدة تعبير المولد. (EN) Pointer to generator expression node.
         */
        void ASTPrinter::visitGeneratorExpr(GeneratorExpr &expr)
        {
            result_ += "(";
            expr.element->accept(*this);
            result_ += " for " + expr.variable + " in ";
            expr.iterable->accept(*this);

            if (expr.condition)
            {
                result_ += " if ";
                expr.condition->accept(*this);
            }

            result_ += ")";
        }

        // =========================================================================
        // Statement Visitors / زوار الجمل
        // =========================================================================

        /**
         * @brief (AR) يزور جملة التعبير - يطبع التعبير مع فاصلة منقوطة.
         *        (EN) Visits expression statement - prints expression with semicolon.
         *
         * @param stmt (AR) مؤشر لجملة التعبير. (EN) Pointer to expression statement.
         */
        void ASTPrinter::visitExprStmt(ExprStmt &stmt)
        {
            result_ += indent();
            stmt.expression->accept(*this);
            result_ += ";\n";
        }

        /**
         * @brief (AR) يزور جملة تصريح المتغير - يطبع النوع والاسم والقيمة الأولية.
         *        (EN) Visits variable declaration - prints type, name, and initial value.
         *
         * @param stmt (AR) مؤشر لجملة تصريح المتغير. (EN) Pointer to variable declaration.
         */
        void ASTPrinter::visitVarDeclStmt(VarDeclStmt &stmt)
        {
            result_ += indent();
            result_ += (stmt.isConst ? "const " : "var ");
            result_ += stmt.name;

            if (stmt.initializer)
            {
                result_ += " = ";
                stmt.initializer->accept(*this);
            }

            result_ += ";\n";
        }

        /**
         * @brief (AR) يزور جملة إذا - يطبع الشرط وفروع إذا صح ووإلا.
         *        (EN) Visits if statement - prints condition, then and else branches.
         *
         * @param stmt (AR) مؤشر لجملة إذا. (EN) Pointer to if statement.
         */
        void ASTPrinter::visitIfStmt(IfStmt &stmt)
        {
            result_ += indent() + "if (";
            stmt.condition->accept(*this);
            result_ += ")\n";

            increaseIndent();
            stmt.thenBranch->accept(*this);
            decreaseIndent();

            if (stmt.elseBranch)
            {
                result_ += indent() + "else\n";
                increaseIndent();
                stmt.elseBranch->accept(*this);
                decreaseIndent();
            }
        }

        /**
         * @brief (AR) يزور جملة بينما - يطبع الشرط والجسم.
         *        (EN) Visits while statement - prints condition and body.
         *
         * @param stmt (AR) مؤشر لجملة بينما. (EN) Pointer to while statement.
         */
        void ASTPrinter::visitWhileStmt(WhileStmt &stmt)
        {
            result_ += indent() + "while (";
            stmt.condition->accept(*this);
            result_ += ")\n";

            increaseIndent();
            stmt.body->accept(*this);
            decreaseIndent();
        }

        /**
         * @brief (AR) يزور جملة لكل (C-style) - يطبع التهيئة والشرط والزيادة.
         *        (EN) Visits for statement (C-style) - prints init, condition, increment.
         *
         * @param stmt (AR) مؤشر لجملة لكل. (EN) Pointer to for statement.
         */
        void ASTPrinter::visitForStmt(ForStmt &stmt)
        {
            result_ += indent() + "for (";

            if (stmt.initializer)
            {
                stmt.initializer->accept(*this);
            }
            result_ += "; ";

            if (stmt.condition)
            {
                stmt.condition->accept(*this);
            }
            result_ += "; ";

            if (stmt.increment)
            {
                stmt.increment->accept(*this);
            }
            result_ += ")\n";

            increaseIndent();
            stmt.body->accept(*this);
            decreaseIndent();
        }

        /**
         * @brief (AR) يزور جملة لكل في - يطبع المتغير والعنصر القابل للتكرار.
         *        (EN) Visits for-range statement - prints variable and iterable.
         *
         * @param stmt (AR) مؤشر لجملة لكل في. (EN) Pointer to for-range statement.
         */
        void ASTPrinter::visitForRangeStmt(ForRangeStmt &stmt)
        {
            result_ += indent() + "for (" + stmt.variable + " in ";
            stmt.iterable->accept(*this);
            result_ += ")\n";

            increaseIndent();
            stmt.body->accept(*this);
            decreaseIndent();
        }

        /**
         * @brief (AR) يزور جملة حالة (switch-case) - يطبع التعبير والحالات.
         *        (EN) Visits switch-case statement - prints expression and cases.
         *
         * @param stmt (AR) مؤشر لجملة حالة. (EN) Pointer to switch statement.
         *
         * Format / التنسيق:
         *   حالة <expression>
         *       عندما <value>: <statement>
         *       افتراضي: <statement>
         *   نهاية
         */
        void ASTPrinter::visitSwitchStmt(SwitchStmt &stmt)
        {
            result_ += indent() + "switch (";
            stmt.expression->accept(*this);
            result_ += ")\n";

            result_ += indent() + "{\n";
            increaseIndent();

            // Print all case branches / طباعة جميع فروع الحالات
            for (const auto &caseItem : stmt.cases)
            {
                result_ += indent() + "case ";
                caseItem.value->accept(*this);
                result_ += ":\n";

                increaseIndent();
                caseItem.body->accept(*this);
                decreaseIndent();
            }

            // Print default case if exists / طباعة الحالة الافتراضية إن وُجدت
            if (stmt.defaultCase)
            {
                result_ += indent() + "default:\n";
                increaseIndent();
                stmt.defaultCase->accept(*this);
                decreaseIndent();
            }

            decreaseIndent();
            result_ += indent() + "}\n";
        }

        /**
         * @brief (AR) يزور جملة إرجاع - يطبع القيمة المُرجعة.
         *        (EN) Visits return statement - prints returned value.
         *
         * @param stmt (AR) مؤشر لجملة إرجاع. (EN) Pointer to return statement.
         */
        void ASTPrinter::visitReturnStmt(ReturnStmt &stmt)
        {
            result_ += indent() + "return";

            if (stmt.value)
            {
                result_ += " ";
                stmt.value->accept(*this);
            }

            result_ += ";\n";
        }

        /**
         * @brief (AR) يزور جملة خروج - يطبع كلمة break.
         *        (EN) Visits break statement - prints break keyword.
         *
         * @param stmt (AR) مؤشر لجملة خروج. (EN) Pointer to break statement.
         */
        void ASTPrinter::visitBreakStmt(BreakStmt &stmt)
        {
            result_ += indent() + "break;\n";
        }

        /**
         * @brief (AR) يزور جملة تابع - يطبع كلمة continue.
         *        (EN) Visits continue statement - prints continue keyword.
         *
         * @param stmt (AR) مؤشر لجملة تابع. (EN) Pointer to continue statement.
         */
        void ASTPrinter::visitContinueStmt(ContinueStmt &stmt)
        {
            result_ += indent() + "continue;\n";
        }

        /**
         * @brief (AR) يزور جملة الكتلة - يطبع الأقواس والجمل الداخلية.
         *        (EN) Visits block statement - prints braces and inner statements.
         *
         * @param stmt (AR) مؤشر لجملة الكتلة. (EN) Pointer to block statement.
         */
        void ASTPrinter::visitBlockStmt(BlockStmt &stmt)
        {
            result_ += indent() + "{\n";

            increaseIndent();
            for (const auto &statement : stmt.statements)
            {
                statement->accept(*this);
            }
            decreaseIndent();

            result_ += indent() + "}\n";
        }

        /**
         * @brief (AR) يزور جملة محاولة-التقاط - يطبع كتل المحاولة والالتقاط وأخيراً.
         *        (EN) Visits try-catch statement - prints try, catch, and finally blocks.
         *
         * @param stmt (AR) مؤشر لجملة محاولة-التقاط. (EN) Pointer to try-catch statement.
         */
        void ASTPrinter::visitTryStmt(TryStmt &stmt)
        {
            result_ += indent() + "try\n";
            stmt.tryBlock->accept(*this);

            for (const auto &catchClause : stmt.catchClauses)
            {
                result_ += indent() + "catch (" + catchClause.exceptionVar + ")\n";
                catchClause.body->accept(*this);
            }

            if (stmt.finallyBlock)
            {
                result_ += indent() + "finally\n";
                stmt.finallyBlock->accept(*this);
            }
        }

        /**
         * @brief (AR) يزور جملة رفع الاستثناء - يطبع تعبير الاستثناء.
         *        (EN) Visits raise statement - prints exception expression.
         *
         * @param stmt (AR) مؤشر لجملة رفع الاستثناء. (EN) Pointer to raise statement.
         */
        void ASTPrinter::visitRaiseStmt(RaiseStmt &stmt)
        {
            result_ += indent() + "raise ";
            stmt.exception->accept(*this);
            result_ += ";\n";
        }

        /**
         * @brief (AR) يزور جملة باستخدام - يطبع المورد والاسم المستعار والجسم.
         *        (EN) Visits with statement - prints resource, alias, and body.
         *
         * @param stmt (AR) مؤشر لجملة مع. (EN) Pointer to with statement.
         */
        void ASTPrinter::visitWithStmt(WithStmt &stmt)
        {
            result_ += indent() + "باستخدام ";
            stmt.resource->accept(*this);
            if (!stmt.alias.empty())
            {
                result_ += " كـ " + stmt.alias;
            }
            result_ += "\n";

            increaseIndent();
            stmt.body->accept(*this);
            decreaseIndent();
            result_ += indent() + "نهاية_استخدام\n";
        }

        // =========================================================================
        // Declaration Visitors / زوار التصريحات
        // =========================================================================

        /**
         * @brief (AR) يزور تصريح الدالة - يطبع الاسم والمعاملات ونوع الإرجاع والجسم.
         *        (EN) Visits function declaration - prints name, parameters, return type, body.
         *
         * @param decl (AR) مؤشر لتصريح الدالة. (EN) Pointer to function declaration.
         */
        void ASTPrinter::visitFunctionDecl(FunctionDecl &decl)
        {
            result_ += indent() + "func " + decl.name + "(";

            for (size_t i = 0; i < decl.parameters.size(); ++i)
            {
                if (i > 0)
                    result_ += ", ";
                result_ += decl.parameters[i].name;
            }

            result_ += ")\n";
            decl.body->accept(*this);
        }

        /**
         * @brief (AR) يزور تصريح الصنف - يطبع الاسم والصنف الأب والأعضاء.
         *        (EN) Visits class declaration - prints name, superclass, and members.
         *
         * @param decl (AR) مؤشر لتصريح الصنف. (EN) Pointer to class declaration.
         */
        void ASTPrinter::visitClassDecl(ClassDecl &decl)
        {
            result_ += indent() + "class " + decl.name;

            if (!decl.superclasses.empty())
            {
                result_ += " extends ";
                for (size_t i = 0; i < decl.superclasses.size(); ++i)
                {
                    if (i > 0)
                        result_ += ", ";
                    result_ += decl.superclasses[i];
                }
            }

            result_ += "\n" + indent() + "{\n";

            increaseIndent();
            for (const auto &member : decl.members)
            {
                member->accept(*this);
            }
            decreaseIndent();

            result_ += indent() + "}\n";
        }

        /**
         * @brief (AR) يزور تصريح الحقل - يطبع معدّل الوصول والاسم والنوع.
         *        (EN) Visits field declaration - prints access modifier, name, and type.
         *
         * @param decl (AR) مؤشر لتصريح الحقل. (EN) Pointer to field declaration.
         */
        void ASTPrinter::visitFieldDecl(FieldDecl &decl)
        {
            result_ += indent() + "var " + decl.name;

            if (decl.initializer)
            {
                result_ += " = ";
                decl.initializer->accept(*this);
            }

            result_ += ";\n";
        }

        /**
         * @brief (AR) يزور تصريح الطريقة - يطبع معدّل الوصول والاسم والمعاملات.
         *        (EN) Visits method declaration - prints access, name, and parameters.
         *
         * @param decl (AR) مؤشر لتصريح الطريقة. (EN) Pointer to method declaration.
         */
        void ASTPrinter::visitMethodDecl(MethodDecl &decl)
        {
            result_ += indent() + "func " + decl.name + "(";

            for (size_t i = 0; i < decl.parameters.size(); ++i)
            {
                if (i > 0)
                    result_ += ", ";
                result_ += decl.parameters[i].name;
            }

            result_ += ")\n";
            decl.body->accept(*this);
        }

        /**
         * @brief (AR) يزور تصريح الباني - يطبع المعاملات والجسم.
         *        (EN) Visits constructor declaration - prints parameters and body.
         *
         * @param decl (AR) مؤشر لتصريح الباني. (EN) Pointer to constructor declaration.
         */
        void ASTPrinter::visitConstructorDecl(ConstructorDecl &decl)
        {
            result_ += indent() + "constructor(";

            for (size_t i = 0; i < decl.parameters.size(); ++i)
            {
                if (i > 0)
                    result_ += ", ";
                result_ += decl.parameters[i].name;
            }

            result_ += ")\n";
            decl.body->accept(*this);
        }

        /**
         * @brief (AR) يزور تصريح الخاصية - يطبع الاسم والـ getter/setter.
         *        (EN) Visits property declaration - prints name and getter/setter.
         *
         * @param decl (AR) مؤشر لتصريح الخاصية. (EN) Pointer to property declaration.
         */
        void ASTPrinter::visitPropertyDecl(PropertyDecl &decl)
        {
            result_ += indent() + "property " + decl.name;

            if (decl.isStatic)
            {
                result_ += " (static)";
            }

            if (decl.isReadOnly())
            {
                result_ += " (read-only)";
            }
            else if (decl.isWriteOnly())
            {
                result_ += " (write-only)";
            }

            result_ += "\n";

            // Print getter
            if (decl.getter)
            {
                result_ += indent() + "  get:\n";
                indentLevel_ += 2;
                if (decl.getter->body)
                {
                    decl.getter->body->accept(*this);
                }
                indentLevel_ -= 2;
            }

            // Print setter if exists
            if (decl.setter)
            {
                result_ += indent() + "  set(" + decl.setter->parameterName + "):\n";
                indentLevel_ += 2;
                if (decl.setter->body)
                {
                    decl.setter->body->accept(*this);
                }
                indentLevel_ -= 2;
            }
        }

        /**
         * @brief (AR) يزور تصريح الهادم - يطبع الجسم.
         *        (EN) Visits destructor declaration - prints body.
         *
         * @param decl (AR) مؤشر لتصريح الهادم. (EN) Pointer to destructor declaration.
         */
        void ASTPrinter::visitDestructorDecl(DestructorDecl &decl)
        {
            result_ += indent() + "destructor\n";
            decl.body->accept(*this);
        }

        /**
         * @brief (AR) يزور تصريح التعداد - يطبع الاسم والأعضاء.
         *        (EN) Visits enum declaration - prints name and members.
         *
         * @param decl (AR) مؤشر لتصريح التعداد. (EN) Pointer to enum declaration.
         */
        void ASTPrinter::visitEnumDecl(EnumDecl &decl)
        {
            result_ += indent() + "enum " + decl.name + "\n";
            result_ += indent() + "{\n";

            increaseIndent();
            for (size_t i = 0; i < decl.members.size(); ++i)
            {
                const auto &member = decl.members[i];
                result_ += indent() + member.name;
                // (AR) حمولة موضعيّة (ADT): اسم(نوع اسم، ...) — النوع قبل الاسم
                // (EN) Positional payload (ADT): name(type field, ...) — type before name
                if (member.hasData())
                {
                    result_ += "(";
                    for (size_t f = 0; f < member.fields.size(); ++f)
                    {
                        if (f > 0)
                            result_ += ", ";
                        const std::string &ft = f < member.fieldTypes.size() ? member.fieldTypes[f] : "";
                        if (!ft.empty())
                            result_ += ft + " ";
                        result_ += member.fields[f];
                    }
                    result_ += ")";
                }
                if (i < decl.members.size() - 1)
                    result_ += ",";
                result_ += "\n";
            }
            decreaseIndent();

            result_ += indent() + "}\n";
        }

        /**
         * @brief (AR) يزور جملة الاستيراد - يطبع مسار الوحدة والاسم المستعار.
         *        (EN) Visits import statement - prints module path and alias.
         *
         * @param stmt (AR) مؤشر لجملة الاستيراد. (EN) Pointer to import statement.
         */
        void ASTPrinter::visitImportStmt(ImportStmt &stmt)
        {
            result_ += indent() + "استورد ";

            // طباعة مسار الوحدة / Print module path
            for (size_t i = 0; i < stmt.modulePath.size(); ++i)
            {
                if (i > 0)
                    result_ += ".";
                result_ += stmt.modulePath[i];
            }

            // طباعة الاسم المستعار / Print alias
            if (stmt.alias.has_value())
            {
                result_ += " كـ " + stmt.alias.value();
            }

            result_ += "\n";
        }

        /**
         * @brief (AR) يزور جملة الاستيراد الانتقائي - يطبع الوحدة والعناصر المستوردة.
         *        (EN) Visits from-import statement - prints module and imported items.
         *
         * @param stmt (AR) مؤشر لجملة الاستيراد الانتقائي. (EN) Pointer to from-import statement.
         */
        void ASTPrinter::visitFromImportStmt(FromImportStmt &stmt)
        {
            result_ += indent() + "من ";

            // طباعة مسار الوحدة / Print module path
            for (size_t i = 0; i < stmt.modulePath.size(); ++i)
            {
                if (i > 0)
                    result_ += ".";
                result_ += stmt.modulePath[i];
            }

            result_ += " استورد ";

            // طباعة العناصر أو * / Print items or *
            if (stmt.isWildcard)
            {
                result_ += "*";
            }
            else
            {
                for (size_t i = 0; i < stmt.items.size(); ++i)
                {
                    if (i > 0)
                        result_ += "، ";
                    result_ += stmt.items[i].name;
                    if (stmt.items[i].alias.has_value())
                    {
                        result_ += " كـ " + stmt.items[i].alias.value();
                    }
                }
            }

            result_ += "\n";
        }

        /**
         * @brief (AR) يزور جملة التصدير (قديم) - يطبع التصريح المصدَّر.
         *        (EN) Visits export statement (legacy) - prints exported declaration.
         *
         * @param stmt (AR) مؤشر لجملة التصدير. (EN) Pointer to export statement.
         */
        void ASTPrinter::visitExportStmt(ExportStmt &stmt)
        {
            result_ += indent() + "export ";
            if (stmt.declaration)
            {
                stmt.declaration->accept(*this);
            }
        }

        /**
         * @brief (AR) يزور تصريح التصدير - يطبع التصريح المصدَّر.
         *        (EN) Visits export declaration - prints exported declaration.
         *
         * @param decl (AR) مؤشر لتصريح التصدير. (EN) Pointer to export declaration.
         */
        void ASTPrinter::visitExportDecl(ExportDecl &decl)
        {
            result_ += indent() + "صدّر ";
            if (decl.declaration)
            {
                decl.declaration->accept(*this);
            }
        }

        void ASTPrinter::visitReExportStmt(ReExportStmt &stmt)
        {
            result_ += indent() + stmt.toString() + "\n";
        }

        // =========================================================================
        // (AR) دوال الـ Visitor المفقودة / (EN) Missing Visitor Methods
        // =========================================================================

        /**
         * @brief (AR) يزور تعبير decorator / (EN) Visits decorator expression
         */
        void ASTPrinter::visitDecoratorExpr(DecoratorExpr &expr)
        {
            result_ += indent() + "@decorator\n";
        }

        /**
         * @brief (AR) يزور تعبير new / (EN) Visits new expression
         */
        void ASTPrinter::visitNewExpr(NewExpr &expr)
        {
            result_ += indent() + "new " + expr.className + "()";
        }

        /**
         * @brief (AR) يزور تعبير الوصول للعضو / (EN) Visits member access expression
         */
        void ASTPrinter::visitMemberAccessExpr(MemberAccessExpr &expr)
        {
            if (expr.object)
            {
                expr.object->accept(*this);
            }
            result_ += "." + expr.memberName;
        }

        /**
         * @brief (AR) يزور تعبير استدعاء method / (EN) Visits method call expression
         */
        void ASTPrinter::visitMethodCallExpr(MethodCallExpr &expr)
        {
            if (expr.object)
            {
                expr.object->accept(*this);
            }
            // (AR) العلامةُ تُطبَع كما كُتِبت: طابعٌ يعرض «س؟.م()» بصيغةِ «س.م()»
            //      يجعل الشجرةَ المطبوعةَ **تكذب** على قارئِها — وهي الأداةُ التي
            //      يُشخَّص بها حين يُشكّ في المحلّل. وطابعٌ يكذب يُضلُّ التشخيصَ نفسَه.
            // (EN) The marker is printed as written: a printer that renders «x?.m()»
            //      as «x.m()» makes the dumped tree lie to the very reader who dumps
            //      it to diagnose the parser.
            result_ += (expr.isOptional ? "؟." : ".") + expr.methodName + "()";
        }

        /**
         * @brief (AR) يزور جملة yield / (EN) Visits yield statement
         */
        void ASTPrinter::visitYieldStmt(YieldStmt &stmt)
        {
            result_ += indent() + "yield";
            if (stmt.value)
            {
                result_ += " ";
                stmt.value->accept(*this);
            }
            result_ += "\n";
        }

        /**
         * @brief (AR) يزور جملة match (مطابقة الأنماط) / (EN) Visits match statement (pattern matching)
         *
         * Source: pattern_nodes.h:375-400 - MatchStmt class definition
         * Source: ast_visitor.h:528 - pure virtual visitMatchStmt declaration
         */
        void ASTPrinter::visitMatchStmt(MatchStmt &stmt)
        {
            result_ += indent() + "match ";

            // طباعة القيمة المُختبرة / Print value to test
            if (stmt.value)
            {
                stmt.value->accept(*this);
            }

            result_ += " {\n";
            indentLevel_++;

            // طباعة كل حالة / Print each case
            for (const auto &caseClause : stmt.cases)
            {
                result_ += indent() + "case ";

                // طباعة النمط باستخدام toString() (Pattern ليس لديه accept) / Print pattern using toString()
                // Source: pattern_nodes.h:56 - Pattern::toString() is pure virtual
                if (caseClause.pattern)
                {
                    result_ += caseClause.pattern->toString();
                }

                // طباعة الحارس (guard) إن وُجد / Print guard if present
                if (caseClause.guard)
                {
                    result_ += " if ";
                    caseClause.guard->accept(*this);
                }

                result_ += ":\n";
                indentLevel_++;

                // طباعة الجسم / Print body
                for (const auto &stmt_in_body : caseClause.body)
                {
                    if (stmt_in_body)
                    {
                        stmt_in_body->accept(*this);
                    }
                }

                indentLevel_--;
            }

            indentLevel_--;
            result_ += indent() + "}\n";
        }

        /**
         * @brief (AR) يزور جملة أجّل / (EN) Visits defer statement
         */
        void ASTPrinter::visitDeferStmt(DeferStmt &stmt)
        {
            result_ += indent() + "defer ";
            if (stmt.body)
            {
                stmt.body->accept(*this);
            }
            result_ += "\n";
        }

        /**
         * @brief (AR) يزور جملة أطلق (goroutine) / (EN) Visits go statement (goroutine)
         */
        void ASTPrinter::visitGoStmt(GoStmt &stmt)
        {
            result_ += indent() + "go ";
            if (stmt.expression)
            {
                stmt.expression->accept(*this);
            }
            else if (stmt.blockBody)
            {
                result_ += "{\n";
                indentLevel_++;
                stmt.blockBody->accept(*this);
                indentLevel_--;
                result_ += indent() + "}";
            }
            result_ += "\n";
        }

        /**
         * @brief (AR) يزور حالة اختيار من قناة / (EN) Visits select case
         */
        void ASTPrinter::visitSelectCase(SelectCase &stmt)
        {
            result_ += indent() + "case ";
            if (stmt.channelExpr)
            {
                stmt.channelExpr->accept(*this);
            }
            result_ += ":\n";
            indentLevel_++;
            for (auto &s : stmt.body)
            {
                if (s)
                    s->accept(*this);
            }
            indentLevel_--;
        }

        /**
         * @brief (AR) يزور جملة اختر (select) / (EN) Visits select statement
         */
        void ASTPrinter::visitSelectStmt(SelectStmt &stmt)
        {
            result_ += indent() + "select {\n";
            indentLevel_++;
            for (auto &c : stmt.cases)
            {
                if (c)
                    c->accept(*this);
            }
            if (!stmt.defaultBody.empty())
            {
                result_ += indent() + "default:\n";
                indentLevel_++;
                for (auto &s : stmt.defaultBody)
                {
                    if (s)
                        s->accept(*this);
                }
                indentLevel_--;
            }
            indentLevel_--;
            result_ += indent() + "}\n";
        }

        /**
         * @brief (AR) يزور جملة تصريح الصنف / (EN) Visits class declaration statement
         */
        void ASTPrinter::visitClassDeclStmt(ClassDeclStmt &stmt)
        {
            result_ += indent() + "class " + stmt.name;

            // طباعة الأصناف الأساسية / Print base classes
            if (!stmt.baseClasses.empty())
            {
                result_ += " : ";
                for (size_t i = 0; i < stmt.baseClasses.size(); ++i)
                {
                    if (i > 0)
                        result_ += ", ";
                    result_ += stmt.baseClasses[i];
                }
            }

            result_ += " {\n";
            indentLevel_++;

            // طباعة الخصائص / Print fields
            for (const auto &field : stmt.fields)
            {
                if (field)
                {
                    field->accept(*this);
                }
            }

            // طباعة الباني / Print constructor
            if (stmt.constructor)
            {
                stmt.constructor->accept(*this);
            }

            // طباعة الطرق / Print methods
            for (const auto &method : stmt.methods)
            {
                if (method)
                {
                    method->accept(*this);
                }
            }

            // طباعة الهدام / Print destructor
            if (stmt.destructor)
            {
                stmt.destructor->accept(*this);
            }

            indentLevel_--;
            result_ += indent() + "}\n";
        }

        void ASTPrinter::visitTraitDecl(TraitDecl &decl)
        {
            result_ += indent() + "trait " + decl.name + " {\n";
            indentLevel_++;
            for (auto &method : decl.methods)
            {
                result_ += indent() + "fn " + method.name + "()\n";
            }
            indentLevel_--;
            result_ += indent() + "}\n";
        }

        void ASTPrinter::visitImplDecl(ImplDecl &decl)
        {
            result_ += indent() + "impl " + decl.traitName + " for " + decl.targetType + " {\n";
            indentLevel_++;
            for (auto &method : decl.methods)
            {
                if (method)
                    method->accept(*this);
            }
            indentLevel_--;
            result_ += indent() + "}\n";
        }

        // =====================================================================
        // Missing Visitor Implementations / تنفيذات الزوار المفقودة
        // =====================================================================

        void ASTPrinter::visitBorrowExpr(BorrowExpr &expr)
        {
            result_ += "(borrow";
            if (expr.isMutable)
                result_ += " mut";
            result_ += " " + expr.variableName + ")";
        }

        void ASTPrinter::visitInlineAsmExpr(InlineAsmExpr &expr)
        {
            result_ += indent() + "(inline_asm \"" + expr.asmCode + "\")";
        }

        void ASTPrinter::visitRangeExpr(RangeExpr &expr)
        {
            result_ += indent() + "(range ";
            if (expr.start)
            {
                expr.start->accept(*this);
            }
            result_ += "..";
            if (expr.end)
            {
                expr.end->accept(*this);
            }
            result_ += ")";
        }

        void ASTPrinter::visitSliceExpr(SliceExpr &expr)
        {
            result_ += indent() + "(slice ";
            expr.object->accept(*this);
            result_ += "[";
            if (expr.start)
            {
                expr.start->accept(*this);
            }
            result_ += ":";
            if (expr.end)
            {
                expr.end->accept(*this);
            }
            if (expr.step)
            {
                result_ += ":";
                expr.step->accept(*this);
            }
            result_ += "])";
        }

        void ASTPrinter::visitTemplateFunctionDecl(TemplateFunctionDecl &decl)
        {
            result_ += indent() + "template_fn " + decl.name + "<";
            for (size_t i = 0; i < decl.typeParameters.size(); i++)
            {
                if (i > 0)
                    result_ += ", ";
                result_ += decl.typeParameters[i].name;
            }
            result_ += ">\n";
        }

        void ASTPrinter::visitTemplateClassDecl(TemplateClassDecl &decl)
        {
            result_ += indent() + "template_class " + decl.name + "<";
            for (size_t i = 0; i < decl.typeParameters.size(); i++)
            {
                if (i > 0)
                    result_ += ", ";
                result_ += decl.typeParameters[i].name;
            }
            result_ += ">\n";
        }

        void ASTPrinter::visitTemplateInstantiation(TemplateInstantiation &expr)
        {
            result_ += "(template_inst " + expr.templateName + ")";
        }

        void ASTPrinter::visitNamespaceDecl(NamespaceDecl &decl)
        {
            result_ += indent() + "namespace " + decl.name + " {\n";
            indentLevel_++;
            for (auto &stmt : decl.members)
            {
                if (stmt)
                    stmt->accept(*this);
            }
            indentLevel_--;
            result_ += indent() + "}\n";
        }

        void ASTPrinter::visitOperatorDecl(OperatorDecl &decl)
        {
            result_ += indent() + "operator " + decl.operatorSymbol + "(";
            for (size_t i = 0; i < decl.parameters.size(); i++)
            {
                if (i > 0)
                    result_ += ", ";
                result_ += decl.parameters[i].name;
            }
            result_ += ")\n";
        }

        void ASTPrinter::visitOptionalChainExpr(OptionalChainExpr &expr)
        {
            result_ += "(";
            if (expr.object)
                expr.object->accept(*this);
            result_ += "؟." + expr.member + ")"; // [ADR-NS-002] عربيّ حصرًا
        }

        void ASTPrinter::visitNullCoalesceExpr(NullCoalesceExpr &expr)
        {
            result_ += "(";
            if (expr.left)
                expr.left->accept(*this);
            result_ += " ?? ";
            if (expr.right)
                expr.right->accept(*this);
            result_ += ")";
        }

        void ASTPrinter::visitStructDecl(StructDecl &decl)
        {
            result_ += indent() + "struct " + decl.name + " {\n";
            indentLevel_++;
            for (auto &field : decl.fields)
            {
                result_ += indent() + field.name + "\n";
            }
            indentLevel_--;
            result_ += indent() + "}\n";
        }

        void ASTPrinter::visitTestDecl(TestDecl &decl)
        {
            result_ += indent() + "test \"" + decl.testName + "\" {\n";
            indentLevel_++;
            if (decl.body)
                decl.body->accept(*this);
            indentLevel_--;
            result_ += indent() + "}\n";
        }

        void ASTPrinter::visitUnsafeBlockStmt(UnsafeBlockStmt &stmt)
        {
            result_ += indent() + "@unsafe {\n";
            indentLevel_++;
            for (auto &s : stmt.body)
            {
                if (s)
                    s->accept(*this);
            }
            indentLevel_--;
            result_ += indent() + "}\n";
        }

        void ASTPrinter::visitComptimeBlockStmt(ComptimeBlockStmt &stmt)
        {
            result_ += indent() + "@comptime {\n";
            indentLevel_++;
            for (auto &s : stmt.body)
            {
                if (s)
                    s->accept(*this);
            }
            indentLevel_--;
            result_ += indent() + "}\n";
        }

        void ASTPrinter::visitSizeofExpr(SizeofExpr &expr)
        {
            result_ += "@sizeof(" + expr.typeName + ")";
        }

        void ASTPrinter::visitAtomicExpr(AtomicExpr &expr)
        {
            result_ += "@atomic(" + expr.operation;
            for (auto &op : expr.operands)
            {
                if (op)
                {
                    result_ += ", ";
                    op->accept(*this);
                }
            }
            result_ += ")";
        }

        void ASTPrinter::visitVolatileVarDeclStmt(VolatileVarDeclStmt &stmt)
        {
            result_ += indent() + "@volatile ";
            if (stmt.declaration)
                stmt.declaration->accept(*this);
        }

        // =========================================================================
        // (AR) امتدادات، ماكروز، أنواع مستعارة، تفكيك صفوف
        // (EN) Extensions, macros, type aliases, tuple destructure
        // =========================================================================

        void ASTPrinter::visitExtensionDecl(ExtensionDecl &decl)
        {
            result_ += indent() + "امتداد " + decl.targetType + " {\n";
            indentLevel_++;
            for (auto &m : decl.methods)
            {
                if (m)
                    m->accept(*this);
            }
            indentLevel_--;
            result_ += indent() + "}\n";
        }

        void ASTPrinter::visitMacroDecl(MacroDecl &decl)
        {
            result_ += indent() + "ماكرو " + decl.name + "(";
            for (size_t i = 0; i < decl.params.size(); ++i)
            {
                if (i > 0)
                    result_ += ", ";
                result_ += decl.params[i];
            }
            result_ += ") {\n";
            indentLevel_++;
            if (decl.body)
                decl.body->accept(*this);
            indentLevel_--;
            result_ += indent() + "}\n";
        }

        void ASTPrinter::visitTypeAliasDecl(TypeAliasDecl &decl)
        {
            result_ += indent() + "نوع " + decl.name + " = ";
            if (decl.target)
                decl.target->accept(*this);
            result_ += "\n";
        }

        void ASTPrinter::visitTupleDestructureStmt(TupleDestructureStmt &stmt)
        {
            result_ += indent() + "متغير (";
            for (size_t i = 0; i < stmt.names.size(); ++i)
            {
                if (i > 0)
                    result_ += ", ";
                result_ += stmt.names[i];
            }
            result_ += ") = ";
            if (stmt.initializer)
                stmt.initializer->accept(*this);
            result_ += "\n";
        }

        // =========================================================================
        // (AR) عقد الواجهات التصريحية / (EN) Declarative UI nodes
        // =========================================================================

        void ASTPrinter::visitUIDeclaration(UIDeclarationNode &node)
        {
            result_ += indent() + "واجهة " + node.name;
            if (!node.parentName.empty())
                result_ += " يرث " + node.parentName;
            result_ += " {\n";
            indentLevel_++;
            for (auto &s : node.stateDecls)
            {
                if (s)
                    s->accept(*this);
            }
            for (auto &m : node.methods)
            {
                if (m)
                    m->accept(*this);
            }
            indentLevel_--;
            result_ += indent() + "}\n";
        }

        void ASTPrinter::visitUIWidgetExpr(UIWidgetExprNode &node)
        {
            result_ += indent() + node.widgetName + "(";
            for (size_t i = 0; i < node.arguments.size(); ++i)
            {
                if (i > 0)
                    result_ += ", ";
                if (node.arguments[i])
                    node.arguments[i]->accept(*this);
            }
            for (size_t i = 0; i < node.namedArgs.size(); ++i)
            {
                if (i > 0 || !node.arguments.empty())
                    result_ += ", ";
                result_ += node.namedArgs[i].first + ": ";
                if (node.namedArgs[i].second)
                    node.namedArgs[i].second->accept(*this);
            }
            result_ += ")";
            for (auto &mod : node.modifiers)
            {
                if (mod)
                    mod->accept(*this);
            }
            if (node.hasChildrenBlock)
            {
                result_ += " {\n";
                indentLevel_++;
                for (auto &child : node.children)
                {
                    if (child)
                        child->accept(*this);
                }
                indentLevel_--;
                result_ += indent() + "}\n";
            }
            else
            {
                result_ += "\n";
            }
        }

        void ASTPrinter::visitUIModifier(UIModifierNode &node)
        {
            result_ += "." + node.name;
            if (node.isEvent && node.eventHandler)
            {
                result_ += " => ";
                node.eventHandler->accept(*this);
            }
            else if (!node.arguments.empty())
            {
                result_ += "(";
                for (size_t i = 0; i < node.arguments.size(); ++i)
                {
                    if (i > 0)
                        result_ += ", ";
                    if (node.arguments[i])
                        node.arguments[i]->accept(*this);
                }
                result_ += ")";
            }
        }

        void ASTPrinter::visitUIEventHandler(UIEventHandlerNode &node)
        {
            switch (node.kind)
            {
            case UIEventKind::ARROW:
                if (node.arrowExpr)
                    node.arrowExpr->accept(*this);
                break;
            case UIEventKind::BLOCK:
                result_ += "{\n";
                indentLevel_++;
                if (node.blockBody)
                    node.blockBody->accept(*this);
                indentLevel_--;
                result_ += indent() + "}";
                break;
            case UIEventKind::LAMBDA:
                if (node.lambdaExpr)
                    node.lambdaExpr->accept(*this);
                break;
            }
        }

        void ASTPrinter::visitUIStateDecl(UIStateDecl &node)
        {
            result_ += indent() + "@";
            switch (node.kind)
            {
            case UIStateKind::STATE:
                result_ += "حالة";
                break;
            case UIStateKind::BINDING:
                result_ += "ربط";
                break;
            case UIStateKind::ENVIRONMENT:
                result_ += "بيئة";
                break;
            case UIStateKind::COMPUTED:
                result_ += "محسوب";
                break;
            }
            result_ += " " + node.name;
            if (!node.typeName.empty())
                result_ += ": " + node.typeName;
            if (node.initializer)
            {
                result_ += " = ";
                node.initializer->accept(*this);
            }
            result_ += "\n";
        }

        // =====================================================================
        // (AR) طباعة عقدة الرسم الشرطي (ADR-UI-01)
        // (EN) Print conditional rendering node
        // =====================================================================

        void ASTPrinter::visitUIConditional(UIConditionalNode &node)
        {
            result_ += indent() + "إذا (";
            if (node.condition)
                node.condition->accept(*this);
            result_ += ")\n";

            indentLevel_++;
            for (auto &child : node.thenChildren)
            {
                if (child)
                    child->accept(*this);
            }
            indentLevel_--;

            if (node.hasElseBranch())
            {
                result_ += indent() + "وإلا\n";
                indentLevel_++;
                for (auto &child : node.elseChildren)
                {
                    if (child)
                        child->accept(*this);
                }
                indentLevel_--;
            }

            result_ += indent() + "نهاية\n";
        }

        // =====================================================================
        // (AR) طباعة عقدة حلقة الرسم (ADR-UI-01)
        // (EN) Print loop rendering node
        // =====================================================================

        void ASTPrinter::visitUILoop(UILoopNode &node)
        {
            if (node.loopKind == UILoopNode::LoopKind::FOR_EACH)
            {
                result_ += indent() + "لكل " + node.iteratorName + " في ";
                if (node.iterableExpr)
                    node.iterableExpr->accept(*this);
                result_ += "\n";
            }
            else
            {
                result_ += indent() + "بينما (";
                if (node.whileCondition)
                    node.whileCondition->accept(*this);
                result_ += ")\n";
            }

            indentLevel_++;
            for (auto &child : node.bodyChildren)
            {
                if (child)
                    child->accept(*this);
            }
            indentLevel_--;

            result_ += indent() + "نهاية\n";
        }

    } // namespace AST
} // namespace Sad
