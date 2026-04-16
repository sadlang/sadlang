// ======================================================================
// parser_ui_declarations.cpp - تحليل الواجهات التصريحية / Declarative UI Parsing
// ======================================================================
// الوصف بالعربية:
//   تنفيذ دوال تحليل نظام الواجهات التصريحي الجديد المستوحى من Flutter/SwiftUI.
//   يُستدعى من parseDeclaration() عند رؤية الكلمة السياقية `واجهة`.
//
//   يشمل:
//   - parseUIDeclaration() — تحليل: واجهة اسم [يرث أب] ... نهاية
//   - parseWidgetExpression() — تحليل: نص()، عمود، زر() مع معدّلات وأبناء
//   - parseModifierChain() — تحليل: .حجم(32).لون(.أزرق).عند_النقر => ...
//   - parseUIStateDecl() — تحليل: @حالة/@ربط/@بيئة/@محسوب
//   - parseUIEventHandler() — تحليل: => تعبير أو كتلة ... نهاية
//   - parseWidgetChildren() — تحليل أبناء حاوية حتى نهاية
//   - isKnownWidget() — التحقق إذا كان الاسم عنصر واجهة مسجل
//
// English Description:
//   Implementation of the new declarative UI parsing system inspired by Flutter/SwiftUI.
//   Called from parseDeclaration() when the contextual keyword `واجهة` is seen.
//
// ملاحظات / Notes:
//   - `واجهة` كلمة سياقية — لا تُسجل في جدول الكلمات المحجوزة
//     `واجهة` is a contextual keyword — not registered in reserved keywords table
//   - تعبيرات العناصر تدعم التسلسل (Method Chaining) عبر المعدّلات
//     Widget expressions support method chaining through modifiers
//   - الحاويات (عمود، صف، رصة) تأخذ أبناء وتنتهي بـ `نهاية`
//     Containers (عمود، صف، رصة) take children and end with `نهاية`
//   - العناصر الورقية (نص, أيقونة) لا تأخذ أبناء ولا تنتهي بـ `نهاية`
//     Leaf widgets (نص, أيقونة) have no children and no `نهاية`
// ======================================================================

#include "parser_core.h"
#include "ui_nodes.h"
#include "declarations.h"
#include "expressions.h"
#include "statements.h"

#include <unordered_set>
#include <string>

namespace Sad
{
    namespace Parser
    {

        using namespace Lexer;
        using namespace AST;
        using TT = TokenType;

        // =====================================================================
        // (AR) مجموعة أسماء العناصر المسجلة — تُستخدم للتعرف على تعبيرات الواجهة
        // (EN) Registered widget names — used to recognize UI expressions
        //
        // (AR) ملاحظة: العناصر المسجلة هنا هي التي يتعرف عليها البارسر كتعبيرات UI
        //      داخل دالة بناء(). أي مُعرّف آخر يُعامل كاستدعاء دالة عادي.
        // (EN) Note: Widgets registered here are recognized as UI expressions
        //      inside بناء(). Any other identifier is treated as a regular function call.
        // =====================================================================

        // (AR) عناصر العرض الأساسية / (EN) Basic display widgets
        // (AR) عناصر الإدخال / (EN) Input widgets
        // (AR) عناصر التخطيط / (EN) Layout widgets
        // (AR) عناصر الحاويات / (EN) Container widgets
        // (AR) عناصر التنقل / (EN) Navigation widgets
        // (AR) عناصر الحوارات / (EN) Dialog widgets
        // (AR) عناصر البيانات / (EN) Data widgets
        static const std::unordered_set<std::string> knownWidgets = {
            // ── عرض أساسي ──
            "نص", "صورة", "أيقونة", "نص_منسق",

            // ── إدخال ──
            "زر", "زر_نصي", "زر_محدد", "زر_أيقونة", "زر_عائم",
            "حقل_نص", "منطقة_نص", "مفتاح", "منزلق",
            "خانة_اختيار", "زر_راديو", "منتقي",
            "منتقي_تاريخ", "منتقي_وقت", "منتقي_لون",

            // ── تخطيط ──
            "عمود", "صف", "رصة", "شبكة", "التفاف",
            "وسط", "حشوة", "محاذاة", "موسع", "مرن",
            "مقاس", "فاصل", "فاصل_خط", "نسبة_عرض",

            // ── حاويات ──
            "حاوية", "بطاقة", "سطح", "صندوق",
            "هيكل", "عرض_تمرير",
            "قائمة_عرض", "عمود_كسول",

            // ── تنقل ──
            "شريط_تطبيق", "درج", "تنقل_سفلي", "تبويبات", "ملاح",

            // ── حوارات ──
            "حوار", "ورقة_سفلية", "شريط_إشعار", "تلميح", "قائمة_خيارات",

            // ── بيانات ──
            "جدول_بيانات", "شريط_تقدم", "تقدم_دائري",
            "شارة", "رقاقة", "شريط_تقييم", "شريط_بحث"};

        // (AR) العناصر التي تقبل أبناء (حاويات) — يُتوقع `نهاية` بعد الأبناء
        // (EN) Widgets that accept children (containers) — expect `نهاية` after children
        static const std::unordered_set<std::string> containerWidgets = {
            "عمود", "صف", "رصة", "شبكة", "التفاف",
            "وسط", "حاوية", "بطاقة", "سطح", "صندوق",
            "هيكل", "عرض_تمرير", "قائمة_عرض", "عمود_كسول",
            "درج", "تبويبات", "ملاح"};

        // (AR) أسماء الأحداث المعروفة — تُستخدم لتمييز المعدّل-الحدث من المعدّل-القيمة
        // (EN) Known event names — used to distinguish event modifiers from value modifiers
        static const std::unordered_set<std::string> knownEvents = {
            "عند_النقر", "عند_الضغط_المطول", "عند_السحب",
            "عند_التحويم", "عند_التغيير", "عند_الظهور",
            "عند_الإرسال"};

        // =====================================================================
        // isKnownWidget — التحقق إذا كان الاسم عنصر واجهة مسجل
        // =====================================================================

        bool ParserCore::isKnownWidget(const std::string &name) const
        {
            return knownWidgets.count(name) > 0;
        }

        // =====================================================================
        // parseUIDeclaration — تحليل: واجهة اسم [يرث أب] ... نهاية
        // =====================================================================
        //
        // (AR) القاعدة النحوية (BNF):
        //   واجهة_decl → 'واجهة' IDENTIFIER ['يرث' IDENTIFIER]
        //                 واجهة_body
        //                 'نهاية'
        //
        //   واجهة_body → (state_decl | method_decl)*
        //
        //   state_decl → '@حالة' [type] IDENTIFIER ['=' expression]
        //              | '@ربط' [type] IDENTIFIER
        //              | '@بيئة' [type] IDENTIFIER
        //              | '@محسوب' IDENTIFIER '=' expression
        //
        //   method_decl → 'دالة' IDENTIFIER '(' [params] ')' block
        //
        // (EN) Grammar (BNF):
        //   ui_decl → 'واجهة' IDENTIFIER ['يرث' IDENTIFIER]
        //             body
        //             'نهاية'
        //
        // =====================================================================

        StmtPtr ParserCore::parseUIDeclaration()
        {
            auto pos = previous_.getPosition();

            // ── (AR) تحليل اسم المكون / (EN) Parse component name ──
            Token nameToken = consume(TT::IDENTIFIER,
                                      "(AR) خطأ نحوي: توقع اسم المكون بعد 'واجهة'.\n"
                                      "مثال: واجهة عداد ... نهاية\n"
                                      "(EN) Syntax error: expected component name after 'واجهة'.\n"
                                      "Example: واجهة counter ... end");

            auto uiDecl = std::make_unique<UIDeclarationNode>(nameToken.getValue(), pos);

            // ── (AR) تحليل الوراثة الاختيارية: يرث أب / (EN) Optional inheritance ──
            if (match(TT::KEYWORD_INHERITS))
            {
                Token parentToken = consume(TT::IDENTIFIER,
                                            "(AR) خطأ نحوي: توقع اسم المكون الأب بعد 'يرث'.\n"
                                            "(EN) Syntax error: expected parent name after 'يرث'.");
                uiDecl->parentName = parentToken.getValue();
            }

            // ── (AR) تحليل جسم الواجهة: تصريحات حالة + دوال / (EN) Parse body ──
            while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
            {

                if (check(TT::KEYWORD_END))
                    break;

                // ── (AR) تصريح حالة: @حالة, @ربط, @بيئة, @محسوب ──
                if (check(TT::AT_SIGN))
                {
                    advance(); // (AR) استهلاك '@' / (EN) consume '@'

                    // (AR) قراءة نوع الحالة
                    if (!check(TT::IDENTIFIER))
                    {
                        errorBilingual(
                            "خطأ نحوي: توقع نوع الحالة بعد '@' داخل واجهة.\n"
                            "الأنواع المسموحة: @حالة, @ربط, @بيئة, @محسوب",
                            "Syntax error: expected state kind after '@' inside واجهة.\n"
                            "Allowed: @حالة, @ربط, @بيئة, @محسوب");
                        break;
                    }

                    std::string stateKind = current_.getValue();
                    advance(); // (AR) استهلاك نوع الحالة

                    auto stateDecl = parseUIStateDecl(stateKind);
                    if (stateDecl)
                    {
                        uiDecl->stateDecls.push_back(std::move(stateDecl));
                    }
                    continue;
                }

                // ── (AR) تصريح دالة: دالة بناء(), دالة أخرى() ──
                if (match(TT::KEYWORD_FUNCTION))
                {
                    auto method = parseFunctionDecl();
                    if (method)
                    {
                        uiDecl->methods.push_back(std::move(method));
                    }
                    continue;
                }

                // ── (AR) أي شيء آخر هو خطأ ──
                errorBilingual(
                    "خطأ نحوي: داخل 'واجهة' يُسمح فقط بتصريحات @حالة/@ربط/@بيئة/@محسوب ودوال.\n"
                    "الرمز غير المتوقع: '" +
                        current_.getValue() + "'",
                    "Syntax error: inside 'واجهة' only @state declarations and functions are allowed.\n"
                    "Unexpected token: '" +
                        current_.getValue() + "'");
                advance(); // (AR) تخطي الرمز المشكل / (EN) skip problematic token
            }

            // ── (AR) استهلاك `نهاية` / (EN) Consume 'end' ──
            consume(TT::KEYWORD_END,
                    "(AR) خطأ نحوي: توقع 'نهاية' لإغلاق كتلة 'واجهة " + uiDecl->name + "'.\n"
                                                                                       "(EN) Syntax error: expected 'نهاية' to close 'واجهة " +
                        uiDecl->name + "' block.");

            return uiDecl;
        }

        // =====================================================================
        // parseUIStateDecl — تحليل: @حالة/@ربط/@بيئة/@محسوب
        // =====================================================================
        //
        // (AR) الصيغ المدعومة:
        //   @حالة اسم: نوع = قيمة     # حالة محلية بنوع وقيمة
        //   @حالة اسم = قيمة           # حالة محلية بقيمة فقط
        //   @حالة اسم: نوع             # حالة محلية بنوع فقط
        //   @ربط اسم: نوع              # مرجع بدون قيمة
        //   @بيئة اسم: نوع             # حالة عالمية
        //   @محسوب اسم = تعبير          # قيمة مشتقة
        //
        // (EN) Supported forms: see above
        // =====================================================================

        std::unique_ptr<UIStateDecl> ParserCore::parseUIStateDecl(const std::string &stateKind)
        {
            auto pos = previous_.getPosition();

            // ── (AR) تحديد نوع الحالة / (EN) Determine state kind ──
            UIStateKind kind;
            if (stateKind == "حالة")
            {
                kind = UIStateKind::STATE;
            }
            else if (stateKind == "ربط")
            {
                kind = UIStateKind::BINDING;
            }
            else if (stateKind == "بيئة")
            {
                kind = UIStateKind::ENVIRONMENT;
            }
            else if (stateKind == "محسوب")
            {
                kind = UIStateKind::COMPUTED;
            }
            else
            {
                errorBilingual(
                    "خطأ نحوي: نوع حالة غير معروف '@" + stateKind + "'.\n"
                                                                    "الأنواع المسموحة: @حالة, @ربط, @بيئة, @محسوب",
                    "Syntax error: unknown state kind '@" + stateKind + "'.\n"
                                                                        "Allowed: @حالة, @ربط, @بيئة, @محسوب");
                return nullptr;
            }

            // ── (AR) اسم المتغير / (EN) Variable name ──
            Token nameToken = consume(TT::IDENTIFIER,
                                      "(AR) خطأ نحوي: توقع اسم المتغير بعد '@" + stateKind + "'.\n"
                                                                                             "(EN) Syntax error: expected variable name after '@" +
                                          stateKind + "'.");

            std::string typeName;
            ExprPtr initializer;

            // ── (AR) نوع اختياري: اسم: نوع / (EN) Optional type ──
            if (match(TT::COLON))
            {
                if (check(TT::IDENTIFIER))
                {
                    typeName = current_.getValue();
                    advance();
                }
            }

            // ── (AR) قيمة ابتدائية اختيارية: = تعبير / (EN) Optional initializer ──
            if (match(TT::OP_ASSIGN))
            {
                initializer = parseExpression();
            }

            // (AR) @محسوب يتطلب تعبير ابتدائي
            // (EN) @computed requires an initializer expression
            if (kind == UIStateKind::COMPUTED && !initializer)
            {
                errorBilingual(
                    "خطأ نحوي: '@محسوب' يتطلب تعبيراً: @محسوب " + nameToken.getValue() + " = تعبير",
                    "Syntax error: '@محسوب' requires an expression: @محسوب " + nameToken.getValue() + " = expression");
            }

            return std::make_unique<UIStateDecl>(kind, nameToken.getValue(), typeName,
                                                 std::move(initializer), pos);
        }

        // =====================================================================
        // parseWidgetExpression — تحليل تعبير عنصر واجهة
        // =====================================================================
        //
        // (AR) القاعدة النحوية:
        //   widget_expr → widget_call modifier_chain? children_block?
        //   widget_call → WIDGET_NAME '(' [args] ')'
        //               | WIDGET_NAME                    # حاوية بدون وسائط
        //   children_block → NEWLINE widget_expr* 'نهاية'
        //
        // (EN) Grammar:
        //   widget_expr → widget_call modifier_chain? children_block?
        //   widget_call → WIDGET_NAME '(' [args] ')'
        //               | WIDGET_NAME                    # container without args
        //   children_block → NEWLINE widget_expr* 'نهاية'
        // =====================================================================

        ExprPtr ParserCore::parseWidgetExpression()
        {
            auto pos = current_.getPosition();
            std::string widgetName = current_.getValue();
            advance(); // (AR) استهلاك اسم العنصر / (EN) consume widget name

            auto widget = std::make_unique<UIWidgetExprNode>(widgetName, pos);

            // ── (AR) وسائط الاستدعاء (اختيارية) / (EN) Optional call arguments ──
            if (check(TT::PAREN_LEFT))
            {
                advance(); // (AR) استهلاك '(' / (EN) consume '('

                if (!check(TT::PAREN_RIGHT))
                {
                    // (AR) تحليل الوسائط — دعم المسماة وغير المسماة
                    // (EN) Parse arguments — support named and positional
                    do
                    {
                        // (AR) تحقق من وسيط مسمّى: اسم: قيمة
                        // (EN) Check for named argument: name: value
                        if (check(TT::IDENTIFIER) && peekNext().getType() == TT::COLON)
                        {
                            std::string argName = current_.getValue();
                            advance(); // (AR) استهلاك الاسم
                            advance(); // (AR) استهلاك ':'
                            auto argValue = parseExpression();
                            widget->namedArgs.push_back({argName, std::move(argValue)});
                        }
                        else
                        {
                            auto arg = parseExpression();
                            widget->arguments.push_back(std::move(arg));
                        }
                    } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));
                }

                consume(TT::PAREN_RIGHT,
                        "(AR) خطأ نحوي: توقع ')' بعد وسائط '" + widgetName + "'.\n"
                                                                             "(EN) Syntax error: expected ')' after '" +
                            widgetName + "' arguments.");
            }

            // ── (AR) سلسلة المعدّلات / (EN) Modifier chain ──
            widget->modifiers = parseModifierChain();

            // ── (AR) كتلة الأبناء (للحاويات فقط) / (EN) Children block (containers only) ──
            // (AR) الحاوية تأخذ أبناء إذا كانت في مجموعة containerWidgets
            //      وتلاها سطر جديد أو عنصر واجهة آخر (وليس `.` أو `)`)
            // (EN) A container takes children if it's in containerWidgets set
            //      and followed by newline or another widget (not `.` or `)`)
            bool isContainer = containerWidgets.count(widgetName) > 0;

            if (isContainer && !check(TT::PAREN_RIGHT) && !check(TT::DOT))
            {
                if (check(TT::KEYWORD_END))
                {
                    // (AR) حاوية فارغة: عمود نهاية / (EN) Empty container
                    widget->hasChildrenBlock = true;
                    advance(); // (AR) استهلاك 'نهاية'
                }
                else if (!check(TT::END_OF_FILE))
                {
                    // (AR) حاوية مع أبناء — نحلل حتى نهاية
                    // (EN) Container with children — parse until نهاية
                    widget->hasChildrenBlock = true;
                    widget->children = parseWidgetChildren();

                    consume(TT::KEYWORD_END,
                            "(AR) خطأ نحوي: توقع 'نهاية' لإغلاق كتلة '" + widgetName + "'.\n"
                                                                                       "(EN) Syntax error: expected 'نهاية' to close '" +
                                widgetName + "' block.");
                }
            }

            return widget;
        }

        // =====================================================================
        // parseModifierChain — تحليل سلسلة المعدّلات
        // =====================================================================
        //
        // (AR) القاعدة النحوية:
        //   modifier_chain → ('.' IDENTIFIER '(' args ')' )*
        //                  | ('.' IDENTIFIER '=>' expression)*
        //                  | ('.' IDENTIFIER block)*
        //
        // أمثلة:
        //   .حجم(32)
        //   .لون(.أزرق)
        //   .عند_النقر => عدد += 1
        //   .عند_النقر ... نهاية
        //
        // (EN) Grammar:
        //   modifier_chain → ('.' IDENTIFIER '(' args ')' )*
        //                  | ('.' IDENTIFIER '=>' expression)*
        //                  | ('.' IDENTIFIER block)*
        // =====================================================================

        std::vector<std::unique_ptr<UIModifierNode>> ParserCore::parseModifierChain()
        {
            std::vector<std::unique_ptr<UIModifierNode>> modifiers;

            while (check(TT::DOT))
            {
                auto dotPos = current_.getPosition();
                advance(); // (AR) استهلاك '.' / (EN) consume '.'

                if (!check(TT::IDENTIFIER))
                {
                    errorBilingual(
                        "خطأ نحوي: توقع اسم معدّل بعد '.' في سلسلة المعدّلات.",
                        "Syntax error: expected modifier name after '.' in modifier chain.");
                    break;
                }

                std::string modName = current_.getValue();
                auto modPos = current_.getPosition();
                advance(); // (AR) استهلاك اسم المعدّل

                // ── (AR) تحقق: هل هذا معالج حدث؟ / (EN) Check: is this an event handler? ──
                bool isEvent = knownEvents.count(modName) > 0;

                if (isEvent)
                {
                    auto handler = parseUIEventHandler(modName);
                    if (handler)
                    {
                        modifiers.push_back(std::make_unique<UIModifierNode>(modName, std::move(handler), modPos));
                    }
                }
                else if (check(TT::PAREN_LEFT))
                {
                    // ── (AR) معدّل بوسائط: .حجم(32) / (EN) Value modifier: .حجم(32) ──
                    advance(); // (AR) استهلاك '(' / (EN) consume '('

                    ExprList args;
                    if (!check(TT::PAREN_RIGHT))
                    {
                        do
                        {
                            args.push_back(parseExpression());
                        } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));
                    }

                    consume(TT::PAREN_RIGHT,
                            "(AR) خطأ نحوي: توقع ')' بعد وسائط المعدّل '." + modName + "'.\n"
                                                                                      "(EN) Syntax error: expected ')' after modifier '." +
                                modName + "' arguments.");

                    modifiers.push_back(std::make_unique<UIModifierNode>(modName, std::move(args), modPos));
                }
                else
                {
                    // ── (AR) معدّل بدون وسائط: .موسع أو .اختياري / (EN) Modifier without args ──
                    ExprList emptyArgs;
                    modifiers.push_back(std::make_unique<UIModifierNode>(modName, std::move(emptyArgs), modPos));
                }
            }

            return modifiers;
        }

        // =====================================================================
        // parseUIEventHandler — تحليل معالج حدث
        // =====================================================================
        //
        // (AR) الصيغ المدعومة:
        //   1. سهم:   .عند_النقر => تعبير
        //   2. كتلة:  .عند_النقر ... نهاية
        //   3. لامدا: .عند_التغيير(لامدا(ق) ... نهاية)
        //
        // (EN) Supported forms:
        //   1. Arrow: .عند_النقر => expression
        //   2. Block: .عند_النقر ... end
        //   3. Lambda: .عند_التغيير(lambda(v) ... end)
        // =====================================================================

        std::unique_ptr<UIEventHandlerNode> ParserCore::parseUIEventHandler(const std::string &eventName)
        {
            auto pos = previous_.getPosition();

            // ── الصيغة 3: لامدا صريحة .عند_التغيير(لامدا ...) ──
            if (check(TT::PAREN_LEFT))
            {
                advance(); // (AR) استهلاك '('

                auto lambdaExpr = parseExpression();

                consume(TT::PAREN_RIGHT,
                        "(AR) خطأ نحوي: توقع ')' بعد لامدا الحدث '" + eventName + "'.\n"
                                                                                  "(EN) Syntax error: expected ')' after event lambda '" +
                            eventName + "'.");

                return UIEventHandlerNode::createLambda(eventName, std::move(lambdaExpr), pos);
            }

            // ── الصيغة 1: سهم .عند_النقر => تعبير ──
            if (match(TT::FAT_ARROW))
            {
                auto expr = parseExpression();
                return UIEventHandlerNode::createArrow(eventName, std::move(expr), pos);
            }

            // ── الصيغة 2: كتلة .عند_النقر ... نهاية ──
            // (AR) كل ما لم يكن سهم أو لامدا يُعامل ككتلة حتى نهاية
            // (EN) Anything that's not arrow or lambda is treated as block until end
            {
                StmtList stmts;
                while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
                {
                    auto stmt = parseDeclaration();
                    if (stmt)
                    {
                        stmts.push_back(std::move(stmt));
                    }
                }

                consume(TT::KEYWORD_END,
                        "(AR) خطأ نحوي: توقع 'نهاية' لإغلاق كتلة الحدث '" + eventName + "'.\n"
                                                                                        "(EN) Syntax error: expected 'نهاية' to close event block '" +
                            eventName + "'.");

                auto block = std::make_unique<BlockStmt>(std::move(stmts), pos);
                return UIEventHandlerNode::createBlock(eventName, std::move(block), pos);
            }
        }

        // =====================================================================
        // parseWidgetChildren — تحليل أبناء حاوية
        // =====================================================================
        //
        // (AR) يُستدعى بعد اسم الحاوية ومعدّلاتها.
        //      يحلل تعبيرات عناصر واجهة حتى `نهاية`.
        //      يدعم: عناصر واجهة مسجلة + جمل عادية (إذا/وإلا, لكل, ...)
        //
        // (EN) Called after container name and modifiers.
        //      Parses widget expressions until `نهاية`.
        //      Supports: registered widgets + regular statements (if/else, for, ...)
        // =====================================================================

        std::vector<std::unique_ptr<UIWidgetExprNode>> ParserCore::parseWidgetChildren()
        {
            std::vector<std::unique_ptr<UIWidgetExprNode>> children;

            while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
            {
                // ── (AR) عنصر واجهة مسجل / (EN) Known widget ──
                if (check(TT::IDENTIFIER) && isKnownWidget(current_.getValue()))
                {
                    auto widgetExpr = parseWidgetExpression();
                    if (auto *w = dynamic_cast<UIWidgetExprNode *>(widgetExpr.get()))
                    {
                        widgetExpr.release();
                        children.push_back(std::unique_ptr<UIWidgetExprNode>(w));
                    }
                    continue;
                }

                // ── (AR) جملة عادية داخل حاوية — غير مدعومة حالياً ──
                // (AR) ملاحظة: في المستقبل قد ندعم إذا/وإلا ولكل داخل الحاويات
                //      لتمكين الرسم الشرطي. حالياً نوقف عند أي رمز غير عنصر واجهة.
                // (EN) Note: In the future we may support if/else and for inside containers
                //      for conditional rendering. Currently we stop at any non-widget token.
                break;
            }

            return children;
        }

    } // namespace Parser
} // namespace Sad
