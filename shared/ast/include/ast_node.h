/**
 * ═══════════════════════════════════════════════════════════════════════════
 * ملف: ast_node.h
 * الوصف: فئات عُقد شجرة التركيب المجرد (AST) الأساسية للغة ص
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * الغرض من هذا الملف:
 * ──────────────────
 * يُعرّف هذا الملف الفئات الأساسية التي تشكّل شجرة التركيب المجرد (AST).
 * شجرة AST هي التمثيل الداخلي لبرنامج لغة ص بعد التحليل النحوي.
 * كل عقدة تمثّل وحدة بنائية في البرنامج (تعبير، جملة، تصريح...).
 *
 * المكونات الرئيسية:
 * ─────────────────
 * - ASTNode: الفئة الجذرية لجميع العُقد، تحمل الموقع في الكود المصدري
 *   وتوفر نمط الزائر (Visitor Pattern) لاجتياز الشجرة
 * - Expression: فئة أساسية للتعابير (كل ما يُقيَّم إلى قيمة):
 *   أمثلة: الثوابت الحرفية، العمليات الحسابية، استدعاء الدوال
 * - Statement: فئة أساسية للعبارات (كل ما ينفّذ إجراءً):
 *   أمثلة: إذا/وإلا، بينما، لكل، تصريحات المتغيرات
 * - ExprPtr / StmtPtr: مؤشرات ذكية (unique_ptr) لإدارة الذاكرة
 * - ExprList / StmtList: قوائم من المؤشرات الذكية
 *
 * الموقع في البنية العامة:
 * ───────────────────────
 *   مصدر .ص → المحلل المعجمي (Lexer) → المحلل النحوي (Parser)
 *            → [شجرة AST — هذا الملف] → المفسر أو المترجم
 *
 * المحلل النحوي (ParserCore) يبني عُقد AST من تدفق الرموز.
 * المفسر (InterpreterCore) أو المترجم (SIRBuilder) يجتاز الشجرة
 * باستخدام نمط الزائر (ASTVisitor) لتنفيذ أو ترجمة البرنامج.
 *
 * أمثلة الاستخدام:
 * ──────────────
 * @code{.cpp}
 * // إنشاء تعبير حرفي للعدد 42
 * auto lit = std::make_unique<LiteralExpr>(Token(TokenType::NUMBER_INTEGER, "42"));
 *
 * // إنشاء عملية جمع: 5 + 3
 * ExprPtr left = std::make_unique<LiteralExpr>(...);
 * ExprPtr right = std::make_unique<LiteralExpr>(...);
 * auto add = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_PLUS, std::move(right));
 *
 * // اجتياز الشجرة بالزائر
 * MyVisitor visitor;
 * add->accept(visitor);
 * @endcode
 *
 * الاعتماديات:
 * ──────────
 * - token.h: بنية Position لتحديد موقع العقدة في المصدر
 * - sad_type_system.h: تعداد SadTypeKind لتحديد نوع التعبير
 *
 * ═══════════════════════════════════════════════════════════════════════════
 * @file ast_node.h
 * @brief Base AST node classes for Sad language / فئات عُقد AST الأساسية للغة ص
 * @author Sad Language Team
 * @date 11 November 2025
 */

#ifndef SAD_AST_NODE_H
#define SAD_AST_NODE_H

#include <string>
#include <memory>
#include <vector>
#include "token.h"
#include "data_types.h"
#include "sad_type_system.h"

namespace Sad
{
    namespace AST
    {

        // Forward declarations
        class ASTVisitor;

        /**
         * @brief Base class for all AST nodes / الفئة الأساسية لجميع عُقد AST
         *
         * All AST nodes inherit from this class. Each node has a position
         * (for error reporting) and implements the Visitor pattern.
         *
         * جميع عُقد AST ترث من هذه الفئة. كل عقدة لها موقع
         * (للإبلاغ عن الأخطاء) وتنفذ نمط الزائر.
         */
        class ASTNode
        {
        public:
            Lexer::Position position; ///< Source code position / موقع الكود المصدري
            std::string docComment;   ///< (AR) تعليق توثيقي مرفق (## أو #** **#) — فارغ إذا لم يوجد
                                      ///< (EN) Attached doc comment (## or #** **#) — empty if none

            /**
             * @brief Default constructor / البناء الافتراضي
             */
            ASTNode() : position() {}

            /**
             * @brief Constructor with position / البناء مع الموقع
             * @param pos Source position / الموقع في الكود المصدري
             */
            explicit ASTNode(const Lexer::Position &pos)
                : position(pos) {}

            /**
             * @brief Virtual destructor / المُدمِّر الافتراضي
             */
            virtual ~ASTNode() = default;

            /**
             * @brief Convert node to string representation / تحويل العقدة لنص
             * @return String representation / التمثيل النصي
             *
             * @example
             * @code{.cpp}
             * auto node = std::make_unique<LiteralExpr>(...);
             * std::cout << node->toString(); // "42"
             * @endcode
             */
            virtual std::string toString() const = 0;

            /**
             * @brief Accept visitor (Visitor pattern) / قبول الزائر (نمط الزائر)
             * @param visitor The visitor to accept / الزائر المطلوب قبوله
             *
             * Implements the Visitor pattern for traversing the AST.
             * Each node type implements this to call the appropriate visit method.
             *
             * ينفذ نمط الزائر لاجتياز شجرة AST.
             * كل نوع عقدة ينفذ هذا لاستدعاء دالة الزيارة المناسبة.
             *
             * @example
             * @code{.cpp}
             * ASTPrinter printer;
             * node->accept(printer);
             * std::cout << printer.getOutput();
             * @endcode
             */
            virtual void accept(ASTVisitor &visitor) = 0;
        };

        /**
         * @brief Base class for all expression nodes / الفئة الأساسية لجميع عُقد التعابير
         *
         * Expressions are AST nodes that evaluate to a value.
         * Examples: literals, binary operations, function calls, etc.
         *
         * التعابير هي عُقد AST التي تُقيَّم إلى قيمة.
         * أمثلة: الثوابت، العمليات الثنائية، استدعاء الدوال، إلخ.
         */
        class Expression : public ASTNode
        {
        public:
            using ASTNode::ASTNode; // Inherit constructors / وراثة البناءات

            /**
             * @brief (AR) استنساخ عميق للتعبير
             * @brief (EN) Deep clone of the expression
             * @return (ExprPtr) — (AR) نسخة مستقلة أو nullptr إذا لم يُنفَّذ
             *                     (EN) Independent copy or nullptr if not implemented
             *
             * (AR) يُستخدم لنسخ AST (مثلاً: قوالب، ماكرو، دوال سمات افتراضية)
             * (EN) Used for AST copying (e.g.: templates, macros, trait default methods)
             * (AR) متوافق مع وضع freestanding — لا يرمي استثناءات
             * (EN) Freestanding compatible — does not throw exceptions
             */
            virtual std::unique_ptr<Expression> clone() const { return nullptr; }

            /**
             * @brief (AR) الحصول على النوع الموحد لهذا التعبير / (EN) Get the unified type of this expression
             * @return (AR) مؤشر النوع الموحد / (EN) The unified type pointer
             *
             * (AR) يُرجع النوع الذي يُقيَّم إليه هذا التعبير باستخدام نظام الأنواع الموحد (SadType).
             *      هذا هو الواجهة الأساسية — ADR-01 Phase 2.
             * (EN) Returns the SadType that this expression evaluates to.
             *      This is the primary interface — ADR-01 Phase 2.
             */
            virtual Types::SadTypePtr getType() const = 0;

            /**
             * @brief (AR) نوع العقدة كـ SadTypeKind — المحور الموحَّد (S-TS-P2.5a)
             * @brief (EN) Node kind as SadTypeKind — the unified pivot (S-TS-P2.5a)
             *
             * (AR) يحلّ محلّ getTypeKind() القديم؛ يُشتقّ مباشرة من getType()->getKind()
             *      المحور الموحَّد للنوع (S-TS-P2.5a).
             */
            Types::SadTypeKind getTypeKind() const
            {
                auto t = getType();
                return t ? t->getKind() : Types::SadTypeKind::Unknown;
            }
        };

        /**
         * @brief Base class for all statement nodes / الفئة الأساسية لجميع عُقد العبارات
         *
         * Statements are AST nodes that perform actions but don't evaluate to values.
         * Examples: if statements, while loops, variable declarations, etc.
         *
         * العبارات هي عُقد AST التي تؤدي إجراءات لكنها لا تُقيَّم إلى قيم.
         * أمثلة: عبارات if، حلقات while، تصريحات المتغيرات، إلخ.
         */
        class Statement : public ASTNode
        {
        public:
            using ASTNode::ASTNode; // Inherit constructors / وراثة البناءات

            /**
             * @brief (AR) استنساخ عميق للعبارة
             * @brief (EN) Deep clone of the statement
             * @return (StmtPtr) — (AR) نسخة مستقلة أو nullptr إذا لم يُنفَّذ
             *                     (EN) Independent copy or nullptr if not implemented
             */
            virtual std::unique_ptr<Statement> clone() const { return nullptr; }
        };

        // Smart pointer type aliases for convenience / أسماء مستعارة للمؤشرات الذكية للراحة

        /**
         * @brief Smart pointer to Expression / مؤشر ذكي للتعبير
         *
         * Use this instead of raw pointers to ensure proper memory management.
         * استخدم هذا بدلاً من المؤشرات الخام لضمان إدارة الذاكرة الصحيحة.
         *
         * @example
         * @code{.cpp}
         * ExprPtr left = std::make_unique<LiteralExpr>(...);
         * ExprPtr right = std::make_unique<LiteralExpr>(...);
         * ExprPtr binary = std::make_unique<BinaryExpr>(std::move(left), TokenType::OP_PLUS, std::move(right));
         * @endcode
         */
        using ExprPtr = std::unique_ptr<Expression>;

        /**
         * @brief Smart pointer to Statement / مؤشر ذكي للعبارة
         *
         * Use this instead of raw pointers to ensure proper memory management.
         * استخدم هذا بدلاً من المؤشرات الخام لضمان إدارة الذاكرة الصحيحة.
         *
         * @example
         * @code{.cpp}
         * StmtPtr ifStmt = std::make_unique<IfStmt>(...);
         * StmtPtr whileStmt = std::make_unique<WhileStmt>(...);
         * @endcode
         */
        using StmtPtr = std::unique_ptr<Statement>;

        /**
         * @brief List of expression pointers / قائمة من مؤشرات التعابير
         */
        using ExprList = std::vector<ExprPtr>;

        /**
         * @brief List of statement pointers / قائمة من مؤشرات العبارات
         */
        using StmtList = std::vector<StmtPtr>;

    } // namespace AST
} // namespace Sad

#endif // SAD_AST_NODE_H
