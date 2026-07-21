/**
 * @file statements.h
 * @brief Statement AST node definitions / تعريفات عُقد AST للجمل
 * @author Sad Language Team
 * @date 11 November 2025
 */

#ifndef SAD_AST_STATEMENTS_H
#define SAD_AST_STATEMENTS_H

#include "ast_node.h"
#include "ast_visitor.h"
#include "expressions.h"
#include <vector>
#include <string>

namespace Sad
{
    namespace AST
    {

        // Forward declarations
        class VarDeclStmt;

        // =========================================================================
        // Expression Statement / جملة التعبير
        // =========================================================================

        /**
         * @brief Expression statement node / عقدة جملة التعبير
         *
         * Represents an expression used as a statement.
         * يمثل تعبير مستخدم كجملة.
         *
         * @example Examples / أمثلة:
         * - print("hello")
         * - x + 5
         * - obj.method()
         */
        class ExprStmt : public Statement
        {
        public:
            ExprPtr expression; ///< The expression / التعبير

            /**
             * @brief Constructor / البناء
             * @param expr Expression / التعبير
             */
            explicit ExprStmt(ExprPtr expr)
                : Statement(expr->position), expression(std::move(expr)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitExprStmt(*this);
            }

            std::string toString() const override
            {
                return expression->toString() + ";";
            }
        };

        // =========================================================================
        // Variable Declaration Statement / جملة تصريح المتغير
        // =========================================================================

        /**
         * @brief Variable declaration statement node / عقدة جملة تصريح المتغير
         *
         * Represents a variable declaration with optional initialization.
         * يمثل تصريح متغير مع إعداد اختياري.
         *
         * @example Examples / أمثلة:
         * - var x = 10
         * - let name: string = "أحمد"
         * - const PI = 3.14
         */
        class VarDeclStmt : public Statement
        {
        public:
            std::string name;          ///< Variable name / اسم المتغير
            Types::SadTypeKind type;       ///< Variable type (legacy) / نوع المتغير (قديم)
            ExprPtr initializer;       ///< Initial value (optional) / القيمة الأولية
            bool isConst;              ///< Is constant? / ثابت؟
            Types::SadTypePtr sadType; ///< (AR) النوع الموحد الجديد / (EN) Unified type (new system)
            // (AR) سمات تخزين ساكن (اللبنة 3.14): يملؤها المحلّل من توجيهات @رمز/@متطاير
            std::string linkSymbol;    ///< (AR) رمز رابط مُصدَّر ثابت (@رمز("اسم")) — فارغ = اسم داخليّ
            bool isVolatile = false;   ///< (AR) متطاير (@متطاير) — يوسم قراءات/كتابات المخزن volatile
            // (AR) سمات مصفوفة تخزين ساكن مصفَّرة (اللبنة 3.16): «متغير ساكن اسم مصفوفة[N]»
            //      ⇒ ‏[N x i8] zeroinitializer في .bss (بلا مُهيّئ). N ثابت موجب.
            bool isStaticArray = false;      ///< (AR) مصفوفة .bss مصفَّرة مسمّاة (ساكن مصفوفة[N])
            uint64_t staticArrayCount = 0;   ///< (AR) عدد بايتات المصفوفة الساكنة N (i8 لكلّ عنصر)

            /**
             * @brief Constructor / البناء
             * @param n Variable name / اسم المتغير
             * @param t Variable type / نوع المتغير
             * @param init Initializer expression / تعبير الإعداد
             * @param isConst Is constant / ثابت
             * @param pos Source position / الموقع في الكود
             */
            VarDeclStmt(const std::string &n, Types::SadTypeKind t, ExprPtr init,
                        bool isConst = false, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(n), type(t),
                  initializer(std::move(init)), isConst(isConst),
                  sadType(Types::SadType::fromValueType(t)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitVarDeclStmt(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // Multi-Variable Declaration / تصريح متغيرات متعددة
        // =========================================================================

        /**
         * @brief Multiple variable declaration statement node / عقدة تصريح متغيرات متعددة
         *
         * (AR) يمثل تصريح عدة متغيرات في سطر واحد بفاصلة.
         *      لا يُنشئ نطاقاً جديداً — يُفكك نفسه إلى تصريحات فردية.
         * (EN) Represents multiple variable declarations in one line with commas.
         *      Does NOT create a new scope — unpacks itself into individual declarations.
         *
         * @example Examples / أمثلة:
         * - متغير اسم = "أحمد" ، عمر = 20 ، وظيفة = "مهندس"
         * - ثابت س = 1 ، ص = 2 ، ع = 3
         * - var x = 1 , y = 2 , z = 3
         */
        class MultiVarDeclStmt : public Statement
        {
        public:
            StmtList declarations; ///< Individual var declarations / التصريحات الفردية

            /**
             * @brief Constructor / البناء
             * @param decls List of VarDeclStmt / قائمة تصريحات المتغيرات
             * @param pos Source position / الموقع في الكود
             */
            MultiVarDeclStmt(StmtList decls,
                             const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), declarations(std::move(decls)) {}

            /**
             * @brief (AR) يُفكك التصريح المتعدد إلى تصريحات فردية بدون نطاق جديد.
             *        (EN) Unpacks multi-declaration into individual declarations without new scope.
             *
             * (AR) هذه الطريقة تستدعي accept لكل تصريح فردي مباشرة،
             *      فلا حاجة لإضافة visitMultiVarDeclStmt لأي زائر.
             * (EN) This method calls accept on each individual declaration directly,
             *      so no need to add visitMultiVarDeclStmt to any visitor.
             */
            void accept(ASTVisitor &visitor) override
            {
                for (auto &decl : declarations)
                {
                    decl->accept(visitor);
                }
            }

            std::string toString() const override
            {
                std::string result = "MultiVarDecl(";
                for (size_t i = 0; i < declarations.size(); ++i)
                {
                    if (i > 0)
                        result += ", ";
                    result += declarations[i]->toString();
                }
                result += ")";
                return result;
            }
        };

        // =========================================================================
        // If Statement / جملة إذا
        // =========================================================================

        /**
         * @brief If statement node / عقدة جملة إذا
         *
         * Represents an if-else conditional statement.
         * يمثل جملة شرطية إذا-وإلا.
         *
         * @example Examples / أمثلة:
         * - if (x > 0) { ... }
         * - if (condition) { ... } else { ... }
         * - إذا (الشرط) { ... } وإلا { ... }
         */
        class IfStmt : public Statement
        {
        public:
            ExprPtr condition;  ///< Condition expression / تعبير الشرط
            StmtPtr thenBranch; ///< Then branch / فرع إذا صح
            StmtPtr elseBranch; ///< Else branch (optional) / فرع وإلا

            /**
             * @brief Constructor / البناء
             * @param cond Condition / الشرط
             * @param thenBr Then branch / فرع إذا صح
             * @param elseBr Else branch / فرع وإلا
             * @param pos Source position / الموقع في الكود
             */
            IfStmt(ExprPtr cond, StmtPtr thenBr, StmtPtr elseBr = nullptr,
                   const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), condition(std::move(cond)),
                  thenBranch(std::move(thenBr)), elseBranch(std::move(elseBr)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitIfStmt(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // While Statement / جملة بينما
        // =========================================================================

        /**
         * @brief While loop statement node / عقدة جملة حلقة بينما
         *
         * Represents a while loop.
         * يمثل حلقة بينما.
         *
         * @example Examples / أمثلة:
         * - while (x < 10) { ... }
         * - بينما (العداد < 100) { ... }
         */
        class WhileStmt : public Statement
        {
        public:
            ExprPtr condition; ///< Loop condition / شرط الحلقة
            StmtPtr body;      ///< Loop body / جسم الحلقة

            /**
             * @brief Constructor / البناء
             * @param cond Condition / الشرط
             * @param body Body statement / جملة الجسم
             * @param pos Source position / الموقع في الكود
             */
            WhileStmt(ExprPtr cond, StmtPtr body,
                      const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), condition(std::move(cond)), body(std::move(body)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitWhileStmt(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // For Statement / جملة لكل
        // =========================================================================

        /**
         * @brief For loop statement node (C-style) / عقدة جملة حلقة لكل
         *
         * Represents a C-style for loop.
         * يمثل حلقة لكل بأسلوب C.
         *
         * @example Examples / أمثلة:
         * - for (var i = 0; i < 10; i++) { ... }
         * - for (initialization; condition; increment) { ... }
         */
        class ForStmt : public Statement
        {
        public:
            StmtPtr initializer; ///< Initializer / التهيئة
            ExprPtr condition;   ///< Condition / الشرط
            ExprPtr increment;   ///< Increment / الزيادة
            StmtPtr body;        ///< Loop body / جسم الحلقة

            /**
             * @brief Constructor / البناء
             */
            ForStmt(StmtPtr init, ExprPtr cond, ExprPtr incr, StmtPtr body,
                    const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), initializer(std::move(init)),
                  condition(std::move(cond)), increment(std::move(incr)),
                  body(std::move(body)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitForStmt(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // For-Range Statement / جملة لكل في
        // =========================================================================

        /**
         * @brief For-range loop statement node (Python-style) / عقدة جملة حلقة لكل في
         *
         * Represents a for-each/range-based loop.
         * يمثل حلقة لكل عنصر في مجموعة.
         *
         * @example Examples / أمثلة:
         * - for (x in array) { ... }
         * - لكل (عنصر في القائمة) { ... }
         * - for (key, value in dict) { ... }
         */
        class ForRangeStmt : public Statement
        {
        public:
            std::string variable; ///< Loop variable / متغير الحلقة
            std::string valueVar; ///< Value variable (for dict, optional) / متغير القيمة
            ExprPtr iterable;     ///< Iterable expression / التعبير القابل للتكرار
            StmtPtr body;         ///< Loop body / جسم الحلقة

            /**
             * @brief Constructor / البناء
             */
            ForRangeStmt(const std::string &var, ExprPtr iter, StmtPtr body,
                         const std::string &valVar = "",
                         const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), variable(var), valueVar(valVar),
                  iterable(std::move(iter)), body(std::move(body)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitForRangeStmt(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // Return Statement / جملة إرجاع
        // =========================================================================

        /**
         * @brief Return statement node / عقدة جملة إرجاع
         *
         * Represents a return statement in a function.
         * يمثل جملة إرجاع في دالة.
         *
         * @example Examples / أمثلة:
         * - return x
         * - return 42
         * - أرجع القيمة
         */
        class ReturnStmt : public Statement
        {
        public:
            ExprPtr value; ///< Return value (optional) / قيمة الإرجاع

            /**
             * @brief Constructor / البناء
             * @param val Return value / قيمة الإرجاع
             * @param pos Source position / الموقع في الكود
             */
            ReturnStmt(ExprPtr val = nullptr, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), value(std::move(val)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitReturnStmt(*this);
            }

            std::string toString() const override
            {
                return value ? "return " + value->toString() + ";" : "return;";
            }
        };

        // =========================================================================
        // Yield Statement / جملة الإعطاء (المولّدات)
        // =========================================================================

        /**
         * @brief Yield statement node / عقدة جملة yield
         *
         * Represents a yield statement in generator functions.
         * يمثل جملة yield في الدوال المولّدة.
         *
         * Supports two forms:
         * - yield expr         : yields a single value
         * - yield from iterable: delegates to another generator
         *
         * يدعم صيغتين:
         * - yield expr         : تُعطي قيمة واحدة
         * - yield from iterable: تفوّض إلى مولّد آخر
         *
         * @example Examples / أمثلة:
         * - yield 42
         * - yield x * 2
         * - yield from range(10)
         * - اعطِ 100
         * - اعطِ from قائمة
         */
        class YieldStmt : public Statement
        {
        public:
            ExprPtr value;    ///< Yielded value / القيمة المُعطاة
            bool isYieldFrom; ///< Is 'yield from'? / هل 'yield from'؟

            /**
             * @brief Constructor for simple yield / البناء لـ yield بسيط
             * @param val Yielded value / القيمة المُعطاة
             * @param pos Source position / الموقع في الكود
             */
            YieldStmt(ExprPtr val = nullptr, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), value(std::move(val)), isYieldFrom(false) {}

            /**
             * @brief Constructor for yield from / البناء لـ yield from
             * @param val Iterable expression / تعبير قابل للتكرار
             * @param yieldFrom Flag indicating 'yield from' / علامة تدل على 'yield from'
             * @param pos Source position / الموقع في الكود
             */
            YieldStmt(ExprPtr val, bool yieldFrom, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), value(std::move(val)), isYieldFrom(yieldFrom) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitYieldStmt(*this);
            }

            std::string toString() const override
            {
                if (isYieldFrom)
                {
                    return value ? "yield from " + value->toString() + ";" : "yield from;";
                }
                return value ? "yield " + value->toString() + ";" : "yield;";
            }
        };

        // =========================================================================
        // With Statement (Context Manager) / جملة باستخدام (مدير السياق)
        // =========================================================================

        /**
         * @brief With statement node for context managers / عقدة جملة باستخدام لمديري السياق
         *
         * Represents a context manager statement that ensures proper resource management.
         * يمثل جملة مدير السياق التي تضمن إدارة الموارد بشكل صحيح.
         *
         * The context manager calls __دخول__() when entering the block and __خروج__() when leaving.
         * يستدعي مدير السياق __دخول__() عند الدخول و__خروج__() عند الخروج.
         *
         * Syntax / الصيغة:
         * - باستخدام expr كـ var: body نهاية_استخدام
         * - باستخدام resource = factory() كـ r: ... نهاية_استخدام
         *
         * @example Examples / أمثلة:
         * @code{.s}
         * باستخدام ملف = افتح("data.txt") كـ م
         *     محتوى = م.اقرأ()
         * نهاية_استخدام
         *
         * // With multiple resources / مع موارد متعددة
         * باستخدام افتح("input.txt") كـ دخل
         *     باستخدام افتح("output.txt", "w") كـ خرج
         *         خرج.اكتب(دخل.اقرأ())
         *     نهاية_استخدام
         * نهاية_استخدام
         * @endcode
         */
        class WithStmt : public Statement
        {
        public:
            ExprPtr resource;  ///< Resource expression / تعبير المورد
            std::string alias; ///< Alias name (after كـ) / الاسم المستعار (بعد كـ)
            StmtPtr body;      ///< Body of the with block / جسم كتلة الاستخدام

            /**
             * @brief Constructor / البناء
             * @param res Resource expression / تعبير المورد
             * @param aliasName Alias for the resource / اسم مستعار للمورد
             * @param bodyStmt Body statements / جمل الجسم
             * @param pos Source position / الموقع في الكود
             */
            WithStmt(ExprPtr res, const std::string &aliasName, StmtPtr bodyStmt,
                     const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), resource(std::move(res)), alias(aliasName),
                  body(std::move(bodyStmt)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitWithStmt(*this);
            }

            std::string toString() const override
            {
                std::string result = "باستخدام " + resource->toString();
                if (!alias.empty())
                {
                    result += " كـ " + alias;
                }
                result += "\n" + body->toString() + "\nنهاية_استخدام";
                return result;
            }
        };

        // =========================================================================
        // Defer Statement / جملة التأجيل (تنظيف مضمون)
        // =========================================================================

        /**
         * @brief (AR) عقدة جملة التأجيل — تنفيذ مضمون عند خروج الدالة
         * @brief (EN) Defer statement node — guaranteed execution on function exit
         *
         * (AR) جملة أجّل تؤجل تنفيذ جملة أو كتلة حتى خروج الدالة الحالية.
         *      تُنفَّذ الجمل المؤجلة بترتيب LIFO (الأخير أولاً) عند:
         *      - وصول جملة ارجع (return)
         *      - انتهاء جسم الدالة طبيعياً
         *      - حدوث استثناء (throw)
         *
         * (EN) Defer statement postpones execution until the enclosing function exits.
         *      Deferred statements execute in LIFO order (last-in, first-out) when:
         *      - A return statement is reached
         *      - Function body completes normally
         *      - An exception is thrown
         *
         * @example أمثلة / Examples:
         * @code{.s}
         * # جملة واحدة مؤجلة
         * أجّل اطبع("تنظيف!")
         *
         * # كتلة مؤجلة
         * أجّل
         *     اطبع("أولاً")
         *     اطبع("ثانياً")
         * نهاية
         *
         * # ترتيب LIFO
         * أجّل اطبع("3")  # يُنفَّذ أخيراً
         * أجّل اطبع("2")
         * أجّل اطبع("1")  # يُنفَّذ أولاً
         * # الخرج: 1 2 3
         * @endcode
         */
        class DeferStmt : public Statement
        {
        public:
            StmtPtr body; ///< (AR) الجملة أو الكتلة المؤجلة / (EN) Deferred statement or block

            /**
             * @brief (AR) بانٍ / (EN) Constructor
             * @param stmt (AR) الجملة المؤجلة / (EN) Deferred statement
             * @param pos (AR) الموقع في الكود / (EN) Source position
             */
            DeferStmt(StmtPtr stmt, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), body(std::move(stmt)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitDeferStmt(*this);
            }

            std::string toString() const override
            {
                return "أجّل " + (body ? body->toString() : "");
            }
        };

        // =========================================================================
        // Go Statement / جملة أطلق (Goroutine)
        // =========================================================================

        /**
         * @brief (AR) جملة أطلق — إطلاق goroutine (مهمة متزامنة خفيفة)
         * @brief (EN) Go statement — launch a goroutine (lightweight concurrent task)
         *
         * (AR) تُطلق تعبيراً (عادة استدعاء دالة) في goroutine منفصلة تعمل بالتوازي.
         *      يمكن أن يكون التعبير: استدعاء دالة، لامدا، أو كتلة كود.
         * (EN) Launches an expression (usually a function call) in a separate goroutine
         *      running concurrently. Can be a function call, lambda, or code block.
         *
         * @code
         * أطلق اطبع_سطر("مرحبا")
         * أطلق دالة_طويلة(وسيط1، وسيط2)
         * أطلق لامدا() => عملية_ما()
         * أطلق
         *     # كتلة كود
         * نهاية
         * @endcode
         */
        class GoStmt : public Statement
        {
        public:
            ExprPtr expression; ///< (AR) التعبير المُطلَق (استدعاء دالة أو لامدا) / (EN) Expression to run (function call or lambda)
            StmtPtr blockBody;  ///< (AR) كتلة الكود (إذا كان أطلق ... نهاية) / (EN) Block body (if go ... end)

            /**
             * @brief (AR) بانٍ لأطلق تعبير / (EN) Constructor for go expression
             */
            GoStmt(ExprPtr expr, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), expression(std::move(expr)), blockBody(nullptr) {}

            /**
             * @brief (AR) بانٍ لأطلق كتلة / (EN) Constructor for go block
             */
            GoStmt(StmtPtr block, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), expression(nullptr), blockBody(std::move(block)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitGoStmt(*this);
            }

            std::string toString() const override
            {
                if (expression)
                    return "أطلق " + expression->toString();
                return "أطلق ... نهاية";
            }
        };

        // =========================================================================
        // Select Case / حالة اختيار من قناة
        // =========================================================================

        /**
         * @brief (AR) حالة واحدة من جملة اختر — ربط قيمة من قناة بجسم تنفيذي
         * @brief (EN) A single case in a select statement — binds a channel value to a body
         *
         * @code
         * عندما ق1.استقبل():
         *     اطبع("استلمت من ق1")
         * @endcode
         */
        class SelectCase : public Statement
        {
        public:
            ExprPtr channelExpr;       ///< (AR) تعبير القناة (.استقبل() أو .أرسل()) / (EN) Channel expression
            std::vector<StmtPtr> body; ///< (AR) جسم الحالة / (EN) Case body
            std::string bindVar;       ///< (AR) اسم المتغير المربوط (اختياري) / (EN) Bound variable name (optional)

            SelectCase(ExprPtr chanExpr, std::vector<StmtPtr> bodyStmts,
                       const std::string &var = "",
                       const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), channelExpr(std::move(chanExpr)),
                  body(std::move(bodyStmts)), bindVar(var) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitSelectCase(*this);
            }

            std::string toString() const override
            {
                return "عندما " + (channelExpr ? channelExpr->toString() : "?") + ":";
            }
        };

        // =========================================================================
        // Select Statement / جملة اختر (Select)
        // =========================================================================

        /**
         * @brief (AR) جملة اختر — اختيار من قنوات متعددة (مثل select في Go)
         * @brief (EN) Select statement — multiplex on multiple channels (like Go's select)
         *
         * (AR) تنتظر على عدة قنوات وتنفذ أول حالة جاهزة.
         *      إذا وُجدت حالة افتراضية، تُنفذ عندما لا تكون أي قناة جاهزة.
         * (EN) Waits on multiple channels and executes the first ready case.
         *      If a default case exists, it executes when no channel is ready.
         *
         * @code
         * اختر
         *     عندما ق1.استقبل():
         *         اطبع("من ق1")
         *     عندما ق2.استقبل():
         *         اطبع("من ق2")
         *     افتراضي:
         *         اطبع("لا رسائل")
         * نهاية
         * @endcode
         */
        class SelectStmt : public Statement
        {
        public:
            std::vector<std::unique_ptr<SelectCase>> cases; ///< (AR) حالات الاختيار / (EN) Select cases
            std::vector<StmtPtr> defaultBody;               ///< (AR) الجسم الافتراضي (اختياري) / (EN) Default body (optional)

            SelectStmt(std::vector<std::unique_ptr<SelectCase>> c,
                       std::vector<StmtPtr> def = {},
                       const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), cases(std::move(c)), defaultBody(std::move(def)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitSelectStmt(*this);
            }

            std::string toString() const override
            {
                return "اختر ... نهاية";
            }
        };

        // =========================================================================
        // Break Statement / جملة خروج
        // =========================================================================

        /**
         * @brief Break statement node / عقدة جملة خروج
         *
         * Represents a break statement in a loop.
         * يمثل جملة خروج من حلقة.
         *
         * @example Examples / أمثلة:
         * - break
         * - اخرج
         */
        class BreakStmt : public Statement
        {
        public:
            /**
             * @brief Constructor / البناء
             * @param pos Source position / الموقع في الكود
             */
            explicit BreakStmt(const Lexer::Position &pos = Lexer::Position())
                : Statement(pos) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitBreakStmt(*this);
            }

            std::string toString() const override
            {
                return "break;";
            }
        };

        // =========================================================================
        // Continue Statement / جملة تابع
        // =========================================================================

        /**
         * @brief Continue statement node / عقدة جملة تابع
         *
         * Represents a continue statement in a loop.
         * يمثل جملة متابعة للتكرار التالي في حلقة.
         *
         * @example Examples / أمثلة:
         * - continue
         * - تابع
         */
        class ContinueStmt : public Statement
        {
        public:
            /**
             * @brief Constructor / البناء
             * @param pos Source position / الموقع في الكود
             */
            explicit ContinueStmt(const Lexer::Position &pos = Lexer::Position())
                : Statement(pos) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitContinueStmt(*this);
            }

            std::string toString() const override
            {
                return "continue;";
            }
        };

        // =========================================================================
        // Block Statement / جملة الكتلة
        // =========================================================================

        /**
         * @brief Block statement node / عقدة جملة الكتلة
         *
         * Represents a block of statements enclosed in braces.
         * يمثل كتلة من الجمل محاطة بأقواس معقوفة.
         *
         * @example Examples / أمثلة:
         * - { stmt1; stmt2; stmt3; }
         */
        class BlockStmt : public Statement
        {
        public:
            StmtList statements; ///< Statements in the block / الجمل في الكتلة

            /**
             * @brief Constructor / البناء
             * @param stmts Statement list / قائمة الجمل
             * @param pos Source position / الموقع في الكود
             */
            BlockStmt(StmtList stmts, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), statements(std::move(stmts)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitBlockStmt(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // Try-Catch Statement / جملة محاولة-التقاط
        // =========================================================================

        /**
         * @brief Catch clause / بند الالتقاط
         */
        struct CatchClause
        {
            std::string exceptionVar;      ///< Exception variable name / اسم متغير الاستثناء
            Types::SadTypeKind exceptionType;  ///< Exception type / نوع الاستثناء
            std::string exceptionTypeName; ///< Custom exception class name / اسم صنف الاستثناء المخصص
            StmtPtr body;                  ///< Catch body / جسم الالتقاط

            CatchClause(const std::string &var, Types::SadTypeKind type, StmtPtr body,
                        const std::string &typeName = "")
                : exceptionVar(var), exceptionType(type), exceptionTypeName(typeName),
                  body(std::move(body)) {}

            // Copy constructor deleted (contains unique_ptr)
            CatchClause(const CatchClause &) = delete;
            CatchClause &operator=(const CatchClause &) = delete;

            // Move constructor and assignment
            CatchClause(CatchClause &&) = default;
            CatchClause &operator=(CatchClause &&) = default;
        };

        /**
         * @brief Try-catch statement node / عقدة جملة محاولة-التقاط
         *
         * Represents a try-catch exception handling block.
         * يمثل كتلة معالجة الاستثناءات محاولة-التقاط.
         *
         * @example Examples / أمثلة:
         * - try { ... } catch (e) { ... }
         * - حاول { ... } التقط (خطأ) { ... }
         * - try { ... } catch (e) { ... } finally { ... }
         */
        class TryStmt : public Statement
        {
        public:
            StmtPtr tryBlock;                      ///< Try block / كتلة المحاولة
            std::vector<CatchClause> catchClauses; ///< Catch clauses / بنود الالتقاط
            StmtPtr finallyBlock;                  ///< Finally block (optional) / كتلة أخيراً

            /**
             * @brief Constructor / البناء
             */
            TryStmt(StmtPtr tryBlk, std::vector<CatchClause> catches,
                    StmtPtr finallyBlk = nullptr,
                    const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), tryBlock(std::move(tryBlk)),
                  catchClauses(std::move(catches)), finallyBlock(std::move(finallyBlk)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitTryStmt(*this);
            }

            std::string toString() const override;
        };

        // =========================================================================
        // Raise/Throw Statement / جملة رفع الاستثناء
        // =========================================================================

        /**
         * @brief Raise/throw statement node / عقدة جملة رفع الاستثناء
         *
         * Represents a raise/throw exception statement.
         * يمثل جملة رفع استثناء.
         *
         * @example Examples / أمثلة:
         * - raise Exception("error")
         * - throw new Error("خطأ")
         * - ارفع خطأ("رسالة")
         */
        class RaiseStmt : public Statement
        {
        public:
            ExprPtr exception; ///< Exception expression / تعبير الاستثناء

            /**
             * @brief Constructor / البناء
             * @param exc Exception expression / تعبير الاستثناء
             * @param pos Source position / الموقع في الكود
             */
            RaiseStmt(ExprPtr exc, const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), exception(std::move(exc)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitRaiseStmt(*this);
            }

            std::string toString() const override
            {
                return "raise " + exception->toString() + ";";
            }
        };

        // =========================================================================
        // Switch Statement / جملة حالة (Switch/Case)
        // =========================================================================

        /**
         * @brief Case branch for switch statement / فرع حالة لجملة حالة
         *
         * Represents a single case branch في switch statement.
         * يمثل فرع حالة واحد في جملة حالة.
         *
         * Spec: docs/language_spec/rules/04_syntax.md - switch_stmt
         */
        struct CaseBranch
        {
            ExprPtr value; ///< Case value / قيمة الحالة
            StmtPtr body;  ///< Case body / جسم الحالة

            CaseBranch(ExprPtr val, StmtPtr bod)
                : value(std::move(val)), body(std::move(bod)) {}
        };

        /**
         * @brief Switch statement node / عقدة جملة حالة
         *
         * Represents a switch-case statement with parenthesized condition.
         * يمثل جملة حالة-عندما مع شرط بين أقواس.
         *
         * Spec: docs/language_spec/rules/04_syntax.md
         * Syntax: حالة (expr) عندما val1: body1 عندما val2: body2 افتراضي: default_body نهاية
         *
         * Features / الميزات:
         * - No fall-through: executes only first matching case / لا يوجد fall-through
         * - Default case is optional / الحالة الافتراضية اختيارية
         * - Values can be any expression / القيم يمكن أن تكون أي تعبير
         *
         * @example Examples / أمثلة:
         * @code{.s}
         * حالة (يوم)
         *     عندما 1:
         *         اطبع("الإثنين")
         *     عندما 2:
         *         اطبع("الثلاثاء")
         *     افتراضي:
         *         اطبع("يوم آخر")
         * نهاية
         *
         * حالة (اللون)
         *     عندما "أحمر": اطبع("Red")
         *     عندما "أزرق": اطبع("Blue")
         * نهاية
         * @endcode
         */
        class SwitchStmt : public Statement
        {
        public:
            ExprPtr expression;            ///< Switch expression / تعبير الحالة
            std::vector<CaseBranch> cases; ///< Case branches / فروع الحالات
            StmtPtr defaultCase;           ///< Default case (optional) / الحالة الافتراضية

            /**
             * @brief Constructor / البناء
             * @param expr Switch expression / تعبير الحالة
             * @param caseList Case branches / فروع الحالات
             * @param defCase Default case / الحالة الافتراضية
             * @param pos Source position / الموقع في الكود
             */
            SwitchStmt(ExprPtr expr, std::vector<CaseBranch> caseList,
                       StmtPtr defCase = nullptr,
                       const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), expression(std::move(expr)),
                  cases(std::move(caseList)), defaultCase(std::move(defCase)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitSwitchStmt(*this);
            }

            std::string toString() const override;
        };

    } // namespace AST
} // namespace Sad

#endif // SAD_AST_STATEMENTS_H
