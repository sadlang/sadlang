// ======================================================================
// ui_nodes.h - عقد AST لنظام الواجهات التصريحي / Declarative UI AST Nodes
// ======================================================================
// الوصف بالعربية:
//   تعريف جميع عقد شجرة التحليل النحوي (AST) المتعلقة بنظام الواجهات
//   التصريحي الجديد المستوحى من Flutter/SwiftUI.
//
//   يشمل:
//   - UIDeclarationNode: تعريف واجهة (واجهة اسم ... نهاية)
//   - UIWidgetExprNode: تعبير عنصر واجهة (نص(), عمود, زر() ...)
//   - UIModifierNode: معدّل سلسلي (.حجم(32), .لون(.أزرق) ...)
//   - UIEventHandlerNode: معالج حدث (.عند_النقر => ..., .عند_النقر ... نهاية)
//   - UIStateDecl: تصريح حالة (@حالة, @ربط, @بيئة, @محسوب)
//
// English Description:
//   Definition of all AST nodes for the new declarative UI system
//   inspired by Flutter/SwiftUI.
//
//   Includes:
//   - UIDeclarationNode: UI component declaration (واجهة name ... نهاية)
//   - UIWidgetExprNode: Widget expression (نص(), عمود, زر() ...)
//   - UIModifierNode: Chained modifier (.حجم(32), .لون(.أزرق) ...)
//   - UIEventHandlerNode: Event handler (.عند_النقر => ..., .عند_النقر ... نهاية)
//   - UIStateDecl: State declaration (@حالة, @ربط, @بيئة, @محسوب)
//
// ملاحظات / Notes:
//   - جميع العقد ترث من ASTNode عبر Expression أو Statement
//     All nodes inherit from ASTNode via Expression or Statement
//   - تدعم نمط الزائر (Visitor Pattern) عبر accept()
//     Supports Visitor Pattern through accept()
//   - `واجهة` كلمة سياقية — تعمل كمُعرّف خارج سياقها
//     `واجهة` is a contextual keyword — acts as identifier outside its context
//   - مسار البيانات: Parser → UIDeclarationNode → UIVisitor → UINode Tree → Backend
//     Data flow: Parser → UIDeclarationNode → UIVisitor → UINode Tree → Backend
// ======================================================================

#pragma once

#include "ast_node.h"
#include "ast_visitor.h"
#include "expressions.h"
#include "statements.h"
#include "data_types.h"
#include <vector>
#include <memory>
#include <string>
#include <optional>

namespace Sad
{
    namespace AST
    {

        // =====================================================================
        // تعدادات مساعدة / Helper Enums
        // =====================================================================

        /**
         * @enum UIStateKind
         * @brief (AR) نوع تصريح الحالة في الواجهة
         * @brief (EN) Kind of state declaration in UI component
         *
         * (AR) يحدد كيفية تعامل نظام الحالة التفاعلي مع هذا المتغير:
         *      - STATE: حالة محلية يملكها المكون — يُعيد بناء() عند التغيير
         *      - BINDING: مرجع لحالة في مكون أب — التعديل ينعكس على الأب
         *      - ENVIRONMENT: حالة عالمية مشتركة — قراءة فقط عادةً
         *      - COMPUTED: قيمة مشتقة حسابياً — تُحدَّث تلقائياً
         *
         * (EN) Determines how the reactive state system handles this variable:
         *      - STATE: local state owned by component — rebuild on change
         *      - BINDING: reference to parent state — changes propagate up
         *      - ENVIRONMENT: shared global state — typically read-only
         *      - COMPUTED: derived value — auto-updated when dependencies change
         */
        enum class UIStateKind
        {
            STATE,       ///< @حالة — حالة محلية / local state
            BINDING,     ///< @ربط — مرجع للأب / binding to parent
            ENVIRONMENT, ///< @بيئة — حالة عالمية / global environment
            COMPUTED     ///< @محسوب — قيمة مشتقة / computed value
        };

        /**
         * @enum UIEventKind
         * @brief (AR) نوع معالج الحدث — سطر واحد أو كتلة متعددة الأسطر
         * @brief (EN) Event handler kind — single-line or multi-line block
         *
         * (AR) يحدد شكل كتلة الحدث:
         *      - ARROW: سطر واحد باستخدام سهم ← `.عند_النقر => عدد += 1`
         *      - BLOCK: كتلة متعددة الأسطر ← `.عند_النقر ... نهاية`
         *      - LAMBDA: لامدا صريحة ← `.عند_التغيير(لامدا(ق) ... نهاية)`
         *
         * (EN) Determines event block shape:
         *      - ARROW: single-line with arrow ← `.عند_النقر => عدد += 1`
         *      - BLOCK: multi-line block ← `.عند_النقر ... نهاية`
         *      - LAMBDA: explicit lambda ← `.عند_التغيير(لامدا(ق) ... نهاية)`
         */
        enum class UIEventKind
        {
            ARROW, ///< .عند_النقر => تعبير / single-line arrow
            BLOCK, ///< .عند_النقر ... نهاية / multi-line block
            LAMBDA ///< .عند_التغيير(لامدا(ق) ...) / explicit lambda
        };

        // =====================================================================
        // UIStateDecl — تصريح حالة داخل واجهة
        // =====================================================================

        /**
         * @class UIStateDecl
         * @brief (AR) تصريح حالة داخل مكون واجهة
         * @brief (EN) State declaration inside a UI component
         *
         * (AR) يمثل تصريحاً واحداً من @حالة أو @ربط أو @بيئة أو @محسوب
         *      داخل كتلة `واجهة`. لكل نوع سلوك مختلف في نظام الحالة التفاعلي.
         *
         * (EN) Represents a single @حالة, @ربط, @بيئة, or @محسوب declaration
         *      inside a `واجهة` block. Each kind has different reactive behavior.
         *
         * @example أمثلة / Examples:
         * @code{.ص}
         * @حالة عدد: رقم = 0              # حالة محلية مع نوع وقيمة ابتدائية
         * @ربط قيمة: رقم                   # مرجع بدون قيمة ابتدائية
         * @بيئة سمة: نص                    # حالة عالمية
         * @محسوب المجموع = عناصر.طول()      # قيمة مشتقة
         * @endcode
         */
        class UIStateDecl : public Statement
        {
        public:
            UIStateKind kind;     ///< (AR) نوع الحالة / (EN) State kind
            std::string name;     ///< (AR) اسم المتغير / (EN) Variable name
            std::string typeName; ///< (AR) اسم النوع (اختياري) / (EN) Type name (optional)
            ExprPtr initializer;  ///< (AR) القيمة الابتدائية (اختياري) / (EN) Initial value (optional)

            /**
             * @brief (AR) باني تصريح الحالة
             * @brief (EN) State declaration constructor
             *
             * @param kind (UIStateKind) — نوع الحالة (STATE/BINDING/ENVIRONMENT/COMPUTED)
             * @param name (string) — اسم المتغير
             * @param typeName (string) — اسم النوع (فارغ إذا لم يُحدد)
             * @param init (ExprPtr) — القيمة الابتدائية (nullptr إذا لم تُحدد)
             * @param pos (Position) — الموقع في الكود المصدري
             */
            UIStateDecl(UIStateKind kind, const std::string &name,
                        const std::string &typeName = "",
                        ExprPtr init = nullptr,
                        const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), kind(kind), name(name),
                  typeName(typeName), initializer(std::move(init)) {}

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitUIStateDecl(*this);
            }

            std::string toString() const override
            {
                std::string kindStr;
                switch (kind)
                {
                case UIStateKind::STATE:
                    kindStr = "@حالة";
                    break;
                case UIStateKind::BINDING:
                    kindStr = "@ربط";
                    break;
                case UIStateKind::ENVIRONMENT:
                    kindStr = "@بيئة";
                    break;
                case UIStateKind::COMPUTED:
                    kindStr = "@محسوب";
                    break;
                }
                return kindStr + " " + name;
            }
        };

        // =====================================================================
        // UIEventHandlerNode — معالج حدث على عنصر واجهة
        // =====================================================================

        /**
         * @class UIEventHandlerNode
         * @brief (AR) معالج حدث مرتبط بعنصر واجهة
         * @brief (EN) Event handler attached to a UI widget
         *
         * (AR) يمثل معالج حدث يُطلق عند تفاعل المستخدم مع عنصر UI.
         *      يدعم ثلاث صيغ:
         *      1. سطر واحد: `.عند_النقر => عدد += 1`
         *      2. كتلة: `.عند_النقر ... نهاية`
         *      3. لامدا: `.عند_التغيير(لامدا(ق) ... نهاية)`
         *
         * (EN) Represents an event handler triggered on user interaction.
         *      Supports three forms:
         *      1. Arrow: `.عند_النقر => عدد += 1`
         *      2. Block: `.عند_النقر ... نهاية`
         *      3. Lambda: `.عند_التغيير(لامدا(ق) ... نهاية)`
         *
         * @example أمثلة / Examples:
         * @code{.ص}
         * .عند_النقر => عدد += 1                  # سطر واحد
         * .عند_النقر                               # كتلة
         *     عدد += 1
         *     اطبع("تم النقر")
         * نهاية
         * .عند_التغيير(لامدا(القيمة)               # لامدا
         *     نص_البحث = القيمة
         * نهاية)
         * @endcode
         */
        class UIEventHandlerNode : public Expression
        {
        public:
            std::string eventName; ///< (AR) اسم الحدث (عند_النقر, عند_التغيير, ...) / (EN) Event name
            UIEventKind kind;      ///< (AR) نوع المعالج (سهم/كتلة/لامدا) / (EN) Handler kind
            ExprPtr arrowExpr;     ///< (AR) تعبير السهم (ARROW فقط) / (EN) Arrow expression (ARROW only)
            StmtPtr blockBody;     ///< (AR) جسم الكتلة (BLOCK فقط) / (EN) Block body (BLOCK only)
            ExprPtr lambdaExpr;    ///< (AR) تعبير اللامدا (LAMBDA فقط) / (EN) Lambda expression (LAMBDA only)

            /**
             * @brief (AR) باني معالج حدث بسهم (سطر واحد)
             * @brief (EN) Arrow event handler constructor (single-line)
             */
            static std::unique_ptr<UIEventHandlerNode> createArrow(
                const std::string &eventName, ExprPtr expr,
                const Lexer::Position &pos = Lexer::Position())
            {
                auto node = std::make_unique<UIEventHandlerNode>(pos);
                node->eventName = eventName;
                node->kind = UIEventKind::ARROW;
                node->arrowExpr = std::move(expr);
                return node;
            }

            /**
             * @brief (AR) باني معالج حدث بكتلة (متعدد الأسطر)
             * @brief (EN) Block event handler constructor (multi-line)
             */
            static std::unique_ptr<UIEventHandlerNode> createBlock(
                const std::string &eventName, StmtPtr body,
                const Lexer::Position &pos = Lexer::Position())
            {
                auto node = std::make_unique<UIEventHandlerNode>(pos);
                node->eventName = eventName;
                node->kind = UIEventKind::BLOCK;
                node->blockBody = std::move(body);
                return node;
            }

            /**
             * @brief (AR) باني معالج حدث بلامدا صريحة
             * @brief (EN) Lambda event handler constructor
             */
            static std::unique_ptr<UIEventHandlerNode> createLambda(
                const std::string &eventName, ExprPtr lambda,
                const Lexer::Position &pos = Lexer::Position())
            {
                auto node = std::make_unique<UIEventHandlerNode>(pos);
                node->eventName = eventName;
                node->kind = UIEventKind::LAMBDA;
                node->lambdaExpr = std::move(lambda);
                return node;
            }

            Types::SadTypePtr getType() const override { return Types::SadTypeRegistry::instance().getAny(); }

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitUIEventHandler(*this);
            }

            std::string toString() const override
            {
                return "." + eventName + "(...)";
            }

            explicit UIEventHandlerNode(const Lexer::Position &pos = Lexer::Position())
                : Expression(pos) {}
        };

        // =====================================================================
        // UIModifierNode — معدّل سلسلي على عنصر واجهة
        // =====================================================================

        /**
         * @class UIModifierNode
         * @brief (AR) معدّل سلسلي (Modifier Chain) يُطبَّق على عنصر واجهة
         * @brief (EN) Chained modifier applied to a UI widget
         *
         * (AR) يمثل معدّلاً واحداً في سلسلة معدّلات. المعدّل يُطبَّق على العنصر
         *      السابق في السلسلة ويُرجع نفسه — يتيح التسلسل (Method Chaining).
         *
         *      أنواع المعدّلات:
         *      - قيمة: `.حجم(32)`, `.لون(.أزرق)`, `.عرض(200)`
         *      - حدث: `.عند_النقر => ...`, `.عند_التغيير(...)`
         *
         * (EN) Represents a single modifier in a chain. Applied to the preceding
         *      widget and returns itself — enables method chaining.
         *
         *      Modifier types:
         *      - Value: `.حجم(32)`, `.لون(.أزرق)`, `.عرض(200)`
         *      - Event: `.عند_النقر => ...`, `.عند_التغيير(...)`
         *
         * @example أمثلة / Examples:
         * @code{.ص}
         * نص("مرحبا")
         *     .حجم(32)                    # ← UIModifierNode: name="حجم", args=[32]
         *     .لون(.أزرق)                 # ← UIModifierNode: name="لون", args=[.أزرق]
         *     .وزن(.عريض)                 # ← UIModifierNode: name="وزن", args=[.عريض]
         * @endcode
         */
        class UIModifierNode : public Expression
        {
        public:
            std::string name;   ///< (AR) اسم المعدّل (حجم، لون، عرض، ...) / (EN) Modifier name
            ExprList arguments; ///< (AR) وسائط المعدّل / (EN) Modifier arguments
            bool isEvent;       ///< (AR) هل هو معالج حدث؟ / (EN) Is this an event handler?

            /// (AR) معالج الحدث المرتبط (فقط إذا isEvent == true)
            /// (EN) Associated event handler (only if isEvent == true)
            std::unique_ptr<UIEventHandlerNode> eventHandler;

            /**
             * @brief (AR) باني معدّل قيمة عادي
             * @brief (EN) Value modifier constructor
             *
             * @param name (string) — اسم المعدّل (حجم, لون, عرض, ...)
             * @param args (ExprList) — قائمة الوسائط
             * @param pos (Position) — الموقع في الكود
             */
            UIModifierNode(const std::string &name, ExprList args,
                           const Lexer::Position &pos = Lexer::Position())
                : Expression(pos), name(name), arguments(std::move(args)),
                  isEvent(false), eventHandler(nullptr) {}

            /**
             * @brief (AR) باني معدّل حدث
             * @brief (EN) Event modifier constructor
             *
             * @param name (string) — اسم الحدث (عند_النقر, عند_التغيير, ...)
             * @param handler (UIEventHandlerNode) — معالج الحدث
             * @param pos (Position) — الموقع في الكود
             */
            UIModifierNode(const std::string &name,
                           std::unique_ptr<UIEventHandlerNode> handler,
                           const Lexer::Position &pos = Lexer::Position())
                : Expression(pos), name(name), arguments(),
                  isEvent(true), eventHandler(std::move(handler)) {}

            Types::SadTypePtr getType() const override { return Types::SadTypeRegistry::instance().getAny(); }

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitUIModifier(*this);
            }

            std::string toString() const override
            {
                return "." + name + "(...)";
            }
        };

        // =====================================================================
        // UIWidgetExprNode — تعبير عنصر واجهة (الوحدة الأساسية)
        // =====================================================================

        /**
         * @class UIWidgetExprNode
         * @brief (AR) تعبير عنصر واجهة — القلب النابض لنظام الواجهات التصريحي
         * @brief (EN) Widget expression — the core building block of the declarative UI system
         *
         * (AR) يمثل تعبير عنصر واجهة واحد — يمكن أن يكون:
         *      - عنصر ورقي: `نص("مرحبا")` — بدون أبناء
         *      - حاوية بأبناء: `عمود ... نهاية` — مع قائمة أبناء
         *      - عنصر بمعدّلات: `نص("مرحبا").حجم(32).لون(.أزرق)`
         *
         *      البنية:
         *      widget_call + modifier_chain + children_block
         *        ↓              ↓                ↓
         *      widgetName    modifiers[]       children[]
         *      arguments[]
         *
         *      قاعدة `نهاية`:
         *      - حاوية بأبناء → تحتاج `نهاية`
         *      - عنصر ورقي → لا تحتاج `نهاية`
         *
         * (EN) Represents a single widget expression — can be:
         *      - Leaf widget: `نص("مرحبا")` — no children
         *      - Container with children: `عمود ... نهاية` — with children list
         *      - Widget with modifiers: `نص("مرحبا").حجم(32).لون(.أزرق)`
         *
         *      Structure:
         *      widget_call + modifier_chain + children_block
         *
         *      `نهاية` rule:
         *      - Container with children → needs `نهاية`
         *      - Leaf widget → no `نهاية`
         *
         * @example أمثلة / Examples:
         * @code{.ص}
         * # عنصر ورقي بمعدّلات
         * نص("مرحبا بالعالم")
         *     .حجم(32)
         *     .لون(.أزرق)
         *     .وزن(.عريض)
         *
         * # حاوية بأبناء ومعدّلات
         * عمود
         *     .حشوة(16)
         *     .تباعد(8)
         *
         *     نص("العنوان").حجم(24)
         *     زر("اضغط").عند_النقر => عدد += 1
         * نهاية
         *
         * # عنصر بوسائط مسماة
         * حقل_نص(تلميح: "أدخل اسمك", قيمة: الاسم)
         *     .عند_التغيير => الاسم = القيمة
         * @endcode
         */
        class UIWidgetExprNode : public Expression
        {
        public:
            std::string widgetName;                                  ///< (AR) اسم العنصر (نص، عمود، زر، ...) / (EN) Widget name
            ExprList arguments;                                      ///< (AR) وسائط الاستدعاء / (EN) Call arguments
            std::vector<std::pair<std::string, ExprPtr>> namedArgs;  ///< (AR) وسائط مسماة / (EN) Named arguments
            std::vector<std::unique_ptr<UIModifierNode>> modifiers;  ///< (AR) سلسلة المعدّلات / (EN) Modifier chain
            std::vector<std::unique_ptr<UIWidgetExprNode>> children; ///< (AR) العناصر الأبناء / (EN) Child widgets
            bool hasChildrenBlock;                                   ///< (AR) هل يحتوي على كتلة أبناء؟ / (EN) Has children block?

            /**
             * @brief (AR) باني عنصر واجهة
             * @brief (EN) Widget expression constructor
             *
             * @param widgetName (string) — اسم العنصر (نص, عمود, زر, صورة, ...)
             * @param pos (Position) — الموقع في الكود المصدري
             */
            explicit UIWidgetExprNode(const std::string &widgetName,
                                      const Lexer::Position &pos = Lexer::Position())
                : Expression(pos), widgetName(widgetName), hasChildrenBlock(false) {}

            /**
             * @brief (AR) هل هذا عنصر ورقي (بدون أبناء)؟
             * @brief (EN) Is this a leaf widget (no children)?
             */
            bool isLeaf() const { return children.empty() && !hasChildrenBlock; }

            /**
             * @brief (AR) هل يحتوي على معدّلات؟
             * @brief (EN) Has modifiers?
             */
            bool hasModifiers() const { return !modifiers.empty(); }

            /**
             * @brief (AR) عدد المعدّلات
             * @brief (EN) Number of modifiers
             */
            size_t modifierCount() const { return modifiers.size(); }

            /**
             * @brief (AR) عدد الأبناء
             * @brief (EN) Number of children
             */
            size_t childCount() const { return children.size(); }

            Types::SadTypePtr getType() const override { return Types::SadTypeRegistry::instance().getAny(); }

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitUIWidgetExpr(*this);
            }

            std::string toString() const override
            {
                std::string result = widgetName + "(";
                if (!arguments.empty())
                    result += "...";
                result += ")";
                if (!modifiers.empty())
                    result += "." + std::to_string(modifiers.size()) + " modifiers";
                if (!children.empty())
                    result += " [" + std::to_string(children.size()) + " children]";
                return result;
            }
        };

        // =====================================================================
        // UIDeclarationNode — تعريف مكون واجهة (واجهة ... نهاية)
        // =====================================================================

        /**
         * @class UIDeclarationNode
         * @brief (AR) تعريف مكون واجهة — المقابل لـ StatefulWidget/StatelessWidget في Flutter
         * @brief (EN) UI component declaration — equivalent to StatefulWidget/StatelessWidget in Flutter
         *
         * (AR) يمثل تعريف مكون واجهة كامل باستخدام الكلمة السياقية `واجهة`.
         *      يحتوي على:
         *      - اسم المكون
         *      - اسم المكون الأب (اختياري — وراثة بسيطة)
         *      - تصريحات الحالة (@حالة, @ربط, @بيئة, @محسوب)
         *      - دوال المكون (أهمها: `بناء()` التي تُرجع شجرة عناصر)
         *      - دوال مساعدة (اختياري)
         *
         *      يُعامَل كصنف خاص بالـ UI — لا وراثة معقدة، لا حقول عادية.
         *
         * (EN) Represents a complete UI component definition using the contextual keyword `واجهة`.
         *      Contains:
         *      - Component name
         *      - Parent component name (optional — simple inheritance)
         *      - State declarations (@حالة, @ربط, @بيئة, @محسوب)
         *      - Component methods (most important: `بناء()` returning widget tree)
         *      - Helper methods (optional)
         *
         *      Treated as a UI-specific class — no complex inheritance, no regular fields.
         *
         * @example مثال كامل / Complete example:
         * @code{.ص}
         * واجهة عداد
         *     @حالة عدد: رقم = 0
         *
         *     دالة بناء()
         *         ارجع عمود
         *             .محاذاة(.وسط)
         *             .تباعد(16)
         *
         *             نص("العدد: " + عدد)
         *                 .حجم(48)
         *                 .لون(.أزرق)
         *
         *             زر("زيادة")
         *                 .عند_النقر => عدد += 1
         *         نهاية
         *     نهاية
         * نهاية
         * @endcode
         */
        class UIDeclarationNode : public Statement
        {
        public:
            std::string name;                                     ///< (AR) اسم المكون / (EN) Component name
            std::string parentName;                               ///< (AR) اسم المكون الأب (فارغ = لا وراثة) / (EN) Parent name (empty = no inheritance)
            std::vector<std::unique_ptr<UIStateDecl>> stateDecls; ///< (AR) تصريحات الحالة / (EN) State declarations
            StmtList methods;                                     ///< (AR) دوال المكون (بما فيها بناء()) / (EN) Component methods (including بناء())
            bool isExported;                                      ///< (AR) مصدّر؟ / (EN) Exported?

            /**
             * @brief (AR) باني تعريف المكون
             * @brief (EN) Component declaration constructor
             *
             * @param name (string) — اسم المكون (عداد, قائمة_مهام, شاشة_رئيسية, ...)
             * @param pos (Position) — الموقع في الكود المصدري
             */
            explicit UIDeclarationNode(const std::string &name,
                                       const Lexer::Position &pos = Lexer::Position())
                : Statement(pos), name(name), isExported(false) {}

            /**
             * @brief (AR) هل يحتوي على حالة تفاعلية؟
             * @brief (EN) Has any reactive state?
             *
             * (AR) إذا كان يحتوي على @حالة واحدة على الأقل، فهو stateful.
             * (EN) If it has at least one @حالة, it's stateful.
             */
            bool isStateful() const
            {
                for (const auto &s : stateDecls)
                {
                    if (s->kind == UIStateKind::STATE)
                        return true;
                }
                return false;
            }

            /**
             * @brief (AR) هل يحتوي على ربط (binding)؟
             * @brief (EN) Has any bindings?
             */
            bool hasBindings() const
            {
                for (const auto &s : stateDecls)
                {
                    if (s->kind == UIStateKind::BINDING)
                        return true;
                }
                return false;
            }

            /**
             * @brief (AR) هل يرث من مكون آخر؟
             * @brief (EN) Has parent component?
             */
            bool hasParent() const { return !parentName.empty(); }

            /**
             * @brief (AR) عدد تصريحات الحالة
             * @brief (EN) Number of state declarations
             */
            size_t stateCount() const { return stateDecls.size(); }

            /**
             * @brief (AR) عدد الدوال
             * @brief (EN) Number of methods
             */
            size_t methodCount() const { return methods.size(); }

            void accept(ASTVisitor &visitor) override
            {
                visitor.visitUIDeclaration(*this);
            }

            std::string toString() const override
            {
                std::string result = "واجهة " + name;
                if (!parentName.empty())
                    result += " يرث " + parentName;
                return result;
            }
        };

    } // namespace AST
} // namespace Sad
