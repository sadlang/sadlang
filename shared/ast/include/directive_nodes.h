/**
 * @file directive_nodes.h
 * @brief (AR) عقد التوجيهات — AST nodes for @ directives
 * @brief (EN) Directive AST nodes for @-prefixed language directives
 *
 * (AR) هذا الملف يحتوي على عقد AST للتوجيهات الست:
 *   @تجميع — تجميع مضمّن (inline assembly)
 *   @متطاير — متغير متطاير (volatile variable)
 *   @غير_آمن — كتلة غير آمنة (unsafe block)
 *   @ذري — عملية ذرية (atomic operation)
 *   @وقت_الترجمة — تنفيذ وقت الترجمة (comptime block)
 *   @حجم — حجم النوع (sizeof)
 *
 * (EN) This file contains AST nodes for the 6 @ directives:
 *   @تجميع — inline assembly
 *   @متطاير — volatile variable
 *   @غير_آمن — unsafe block
 *   @ذري — atomic operation
 *   @وقت_الترجمة — compile-time execution
 *   @حجم — sizeof type
 *
 * @author Sad Language Team
 * @date 2025
 */

#pragma once

#include "ast_node.h"
#include "ast_visitor.h"
#include <memory>
#include <string>
#include <vector>

namespace Sad
{
    namespace AST
    {

        // ============================================================================
        // (AR) @غير_آمن ... نهاية — كتلة غير آمنة
        // (EN) @غير_آمن ... نهاية — Unsafe block statement
        // ============================================================================
        /**
         * @brief (AR) كتلة غير آمنة — تسمح بعمليات منخفضة المستوى داخلها
         * @brief (EN) Unsafe block — allows low-level operations inside
         *
         * @code
         * @غير_آمن
         *     # عمليات غير آمنة هنا
         * نهاية
         * @endcode
         */
        class UnsafeBlockStmt : public Statement
        {
        public:
            StmtList body; ///< (AR) جسم الكتلة / (EN) Block body

            explicit UnsafeBlockStmt(StmtList stmts,
                                     const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), body(std::move(stmts)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitUnsafeBlockStmt(*this);
            }

            std::string toString() const override
            {
                return "@غير_آمن { ... }";
            }
        };

        // ============================================================================
        // (AR) @وقت_الترجمة ... نهاية — تنفيذ وقت الترجمة
        // (EN) @وقت_الترجمة ... نهاية — Compile-time execution block
        // ============================================================================
        /**
         * @brief (AR) كتلة تنفيذ وقت الترجمة — تُنفَّذ أثناء الترجمة لا أثناء التشغيل
         * @brief (EN) Compile-time execution block — evaluated at compile time, not runtime
         *
         * @code
         * @وقت_الترجمة
         *     ثابت PI = 3.14159265
         *     ثابت TAU = PI * 2
         * نهاية
         * @endcode
         */
        class ComptimeBlockStmt : public Statement
        {
        public:
            StmtList body; ///< (AR) جسم الكتلة / (EN) Block body

            explicit ComptimeBlockStmt(StmtList stmts,
                                       const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), body(std::move(stmts)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitComptimeBlockStmt(*this);
            }

            std::string toString() const override
            {
                return "@وقت_الترجمة { ... }";
            }
        };

        // ============================================================================
        // (AR) @حجم(نوع) — حجم النوع بالبايتات
        // (EN) @حجم(type) — Size of type in bytes
        // ============================================================================
        /**
         * @brief (AR) تعبير حجم النوع — يُرجع حجم النوع بالبايتات
         * @brief (EN) Sizeof expression — returns the size of a type in bytes
         *
         * @code
         * متغير حجم_الرقم = @حجم(رقم)      # -> 8
         * متغير حجم_النص = @حجم(نص)       # -> حجم متغير
         * @endcode
         */
        class SizeofExpr : public Expression
        {
        public:
            std::string typeName; ///< (AR) اسم النوع / (EN) Type name

            explicit SizeofExpr(const std::string &type,
                                const Lexer::Position &pos = Lexer::Position())
                : Expression(pos), typeName(type) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitSizeofExpr(*this);
            }

            std::string toString() const override
            {
                return "@حجم(" + typeName + ")";
            }

            Types::SadTypePtr getType() const override
            {
                return Types::SadTypeRegistry::instance().getInteger();
            }
        };

        // ============================================================================
        // (AR) @ذري(عملية, وسائط...) — عملية ذرية
        // (EN) @ذري(operation, args...) — Atomic operation
        // ============================================================================
        /**
         * @brief (AR) عملية ذرية — تنفيذ عملية ذرية بدون قفل
         * @brief (EN) Atomic operation — lock-free atomic operation
         *
         * العمليات المدعومة / Supported operations:
         * - تحميل / load
         * - تخزين / store
         * - إضافة / add
         * - طرح / sub
         * - مقارنة_وتبديل / compare_and_swap
         *
         * @code
         * @ذري(تحميل, عداد)
         * @ذري(إضافة, عداد, 1)
         * @ذري(مقارنة_وتبديل, قيمة, متوقع, جديد)
         * @endcode
         */
        class AtomicExpr : public Expression
        {
        public:
            std::string operation; ///< (AR) اسم العملية / (EN) Operation name
            ExprList operands;     ///< (AR) معاملات العملية / (EN) Operation operands

            AtomicExpr(const std::string &op, ExprList args,
                       const Lexer::Position &pos = Lexer::Position())
                : Expression(pos), operation(op), operands(std::move(args)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitAtomicExpr(*this);
            }

            std::string toString() const override
            {
                return "@ذري(" + operation + ", ...)";
            }

            Types::SadTypePtr getType() const override
            {
                return Types::SadTypeRegistry::instance().getInteger();
            }
        };

        // ============================================================================
        // (AR) @متطاير متغير ... — إعلان متغير متطاير
        // (EN) @متطاير var ... — Volatile variable declaration
        // ============================================================================
        /**
         * @brief (AR) إعلان متغير متطاير — يمنع المترجم من تحسين الوصول إليه
         * @brief (EN) Volatile variable declaration — prevents compiler optimization on access
         *
         * @code
         * @متطاير متغير حالة_الجهاز = 0
         * @endcode
         */
        class VolatileVarDeclStmt : public Statement
        {
        public:
            StmtPtr declaration; ///< (AR) إعلان المتغير الأصلي / (EN) Original variable declaration

            explicit VolatileVarDeclStmt(StmtPtr decl,
                                         const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), declaration(std::move(decl)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitVolatileVarDeclStmt(*this);
            }

            std::string toString() const override
            {
                return "@متطاير " + (declaration ? declaration->toString() : "");
            }
        };

    } // namespace AST
} // namespace Sad
