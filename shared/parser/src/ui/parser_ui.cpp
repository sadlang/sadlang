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
#include <unordered_map>
#include <string>
#include <iostream>

namespace Sad
{
    namespace Parser
    {

        using namespace Lexer;
        using namespace AST;
        using TT = TokenType;

        // =====================================================================
        // (AR) العناصر الأولية الـ 15 — المجموعة الأساسية فقط (ADR-UI-02)
        // (EN) 15 Primitive Widgets — core set only (ADR-UI-02)
        //
        // (AR) القرار المعماري ADR-UI-02:
        //      تقليص العناصر من 70+ إلى 15 عنصر أولي فقط.
        //      باقي العناصر (55+) ستُبنى كـ `واجهة` مُركّبة من الأولية.
        //      العناصر القديمة تبقى مؤقتاً في deprecatedWidgets مع تحذير.
        //
        // (EN) Architecture Decision ADR-UI-02:
        //      Reduce widgets from 70+ to 15 primitives only.
        //      Remaining 55+ widgets will be composed as user `واجهة` components.
        //      Old widgets stay temporarily in deprecatedWidgets with warning.
        // =====================================================================

        static const std::unordered_set<std::string> knownWidgets = {
            // ── (AR) تخطيط — 4 حاويات أساسية / (EN) Layout — 4 basic containers ──
            "عمود", "صف", "رصة", "شبكة",

            // ── (AR) عرض — 3 عناصر عرض / (EN) Display — 3 display elements ──
            "نص", "صورة", "أيقونة",

            // ── (AR) تفاعل — 4 عناصر إدخال / (EN) Interaction — 4 input elements ──
            "زر", "حقل_نص", "مفتاح", "منزلق",

            // ── (AR) هيكل — 3 حاويات متقدمة / (EN) Structure — 3 advanced containers ──
            "حاوية", "عرض_تمرير", "قائمة_كسولة",

            // ── (AR) فراغ — عنصر واحد / (EN) Spacer — 1 element ──
            "فاصل"};

        // =====================================================================
        // (AR) خريطة العناصر المُهملة — المرحلة الأولى من خطة الهجرة (ADR-UI-02)
        // (EN) Deprecated widgets map — Phase 1 of migration plan (ADR-UI-02)
        //
        // (AR) كل عنصر قديم يُربط بالعنصر الأولي البديل.
        //      عند استخدام عنصر مُهمل، يُصدر البارسر تحذيراً ويُعامله كبديله.
        //      المرحلة 1 (v1.2): تحذير + عمل عادي
        //      المرحلة 2 (v1.3): ترجمة داخلية تلقائية
        //      المرحلة 3 (v2.0): إزالة مع رسالة خطأ
        //
        // (EN) Each deprecated widget maps to its primitive replacement.
        //      Parser emits a warning when a deprecated widget is used, and treats it as its replacement.
        //      Phase 1 (v1.2): warning + works normally
        //      Phase 2 (v1.3): automatic internal translation
        //      Phase 3 (v2.0): removal with error message
        // =====================================================================

        static const std::unordered_map<std::string, std::string> deprecatedWidgets = {
            // ── (AR) عرض مُهمل → بديل / (EN) Deprecated display → replacement ──
            {"نص_منسق", "نص"},

            // ── (AR) أزرار مُهملة → زر / (EN) Deprecated buttons → زر ──
            {"زر_نصي", "زر"},
            {"زر_محدد", "زر"},
            {"زر_أيقونة", "زر"},
            {"زر_عائم", "زر"},

            // ── (AR) إدخال مُهمل → حقل_نص أو معادل / (EN) Deprecated input → حقل_نص or equivalent ──
            {"منطقة_نص", "حقل_نص"},
            {"خانة_اختيار", "مفتاح"},
            {"زر_راديو", "مفتاح"},
            {"منتقي", "حقل_نص"},
            {"منتقي_تاريخ", "حقل_نص"},
            {"منتقي_وقت", "حقل_نص"},
            {"منتقي_لون", "حقل_نص"},

            // ── (AR) تخطيط مُهمل → حاوية أو معادل / (EN) Deprecated layout → container or equivalent ──
            {"التفاف", "صف"},
            {"وسط", "حاوية"},
            {"حشوة", "حاوية"},
            {"محاذاة", "حاوية"},
            {"موسع", "حاوية"},
            {"مرن", "حاوية"},
            {"مقاس", "حاوية"},
            {"فاصل_خط", "فاصل"},
            {"نسبة_عرض", "حاوية"},

            // ── (AR) حاويات مُهملة → حاوية / (EN) Deprecated containers → حاوية ──
            {"بطاقة", "حاوية"},
            {"سطح", "حاوية"},
            {"صندوق", "حاوية"},
            {"هيكل", "حاوية"},
            {"قائمة_عرض", "قائمة_كسولة"},
            {"عمود_كسول", "قائمة_كسولة"},

            // ── (AR) تنقل مُهمل → حاوية أو معادل / (EN) Deprecated nav → container or equivalent ──
            {"شريط_تطبيق", "حاوية"},
            {"درج", "حاوية"},
            {"تنقل_سفلي", "صف"},
            {"تبويبات", "صف"},
            {"ملاح", "رصة"},

            // ── (AR) حوارات مُهملة → رصة / (EN) Deprecated dialogs → رصة ──
            {"حوار", "رصة"},
            {"ورقة_سفلية", "حاوية"},
            {"شريط_إشعار", "حاوية"},
            {"تلميح", "حاوية"},
            {"قائمة_خيارات", "عمود"},

            // ── (AR) بيانات مُهملة → معادل / (EN) Deprecated data → equivalent ──
            {"جدول_بيانات", "شبكة"},
            {"شريط_تقدم", "حاوية"},
            {"تقدم_دائري", "حاوية"},
            {"شارة", "حاوية"},
            {"رقاقة", "حاوية"},
            {"شريط_تقييم", "صف"},
            {"شريط_بحث", "حقل_نص"},
        };

        // (AR) العناصر التي تقبل أبناء (حاويات) — 4 أساسية + 3 متقدمة (ADR-UI-02)
        // (EN) Container widgets — 4 basic + 3 advanced (ADR-UI-02)
        static const std::unordered_set<std::string> containerWidgets = {
            // (AR) حاويات تخطيط أساسية
            "عمود", "صف", "رصة", "شبكة",
            // (AR) حاويات متقدمة
            "حاوية", "عرض_تمرير", "قائمة_كسولة"};

        // (AR) أسماء الأحداث المعروفة — تُستخدم لتمييز المعدّل-الحدث من المعدّل-القيمة
        // (EN) Known event names — used to distinguish event modifiers from value modifiers
        static const std::unordered_set<std::string> knownEvents = {
            "عند_النقر", "عند_الضغط_المطول", "عند_السحب",
            "عند_التحويم", "عند_التغيير", "عند_الظهور",
            "عند_الإرسال"};

        // =====================================================================
        // isKnownWidget — التحقق إذا كان الاسم عنصر واجهة مسجل
        // =====================================================================
        //
        // (AR) يتحقق من العناصر الأولية (15) + العناصر المُهملة (55+)
        //      إذا كان العنصر مُهملاً، يُصدر تحذيراً ويعيد true (ADR-UI-02 مرحلة 1)
        // (EN) Checks primitives (15) + deprecated (55+)
        //      If deprecated, emits warning and returns true (ADR-UI-02 phase 1)
        // =====================================================================

        bool ParserCore::isKnownWidget(const std::string &name) const
        {
            // (AR) أولاً: هل هو عنصر أولي؟ / (EN) First: is it a primitive?
            if (knownWidgets.count(name) > 0)
                return true;

            // (AR) ثانياً: هل هو عنصر مُهمل؟ (ADR-UI-02 مرحلة 1: تحذير + عمل عادي)
            // (EN) Second: is it deprecated? (ADR-UI-02 phase 1: warning + works normally)
            auto it = deprecatedWidgets.find(name);
            if (it != deprecatedWidgets.end())
            {
                // (AR) تحذير إهمال: العنصر القديم لا يزال يعمل لكن يجب استبداله
                // (EN) Deprecation warning: old widget still works but should be replaced
                std::cerr << "[تحذير/Warning] " << current_.getPosition().line << ":"
                          << current_.getPosition().column << ": "
                          << "العنصر \"" << name << "\" مُهمل — استخدم \""
                          << it->second << "\" بدلاً منه / "
                          << "Widget \"" << name << "\" is deprecated — use \""
                          << it->second << "\" instead." << std::endl;
                return true;
            }

            return false;
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
        //      يدعم: عناصر واجهة مسجلة + الرسم الشرطي (إذا/وإلا) + الحلقات (لكل/بينما)
        //
        // (EN) Called after container name and modifiers.
        //      Parses widget expressions until `نهاية`.
        //      Supports: registered widgets + conditional rendering (if/else) + loops (for/while)
        // =====================================================================

        std::vector<std::unique_ptr<UIWidgetExprNode>> ParserCore::parseWidgetChildren()
        {
            std::vector<std::unique_ptr<UIWidgetExprNode>> children;

            while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
            {
                // ── (AR) عنصر واجهة مسجل / (EN) Known widget ──
                if (check(TT::IDENTIFIER) && isKnownWidget(current_.getValue()))
                {
                    children.push_back(parseWidgetExpressionTyped());
                    continue;
                }

                // ── (AR) رسم شرطي: إذا (شرط) ... وإلا ... نهاية (ADR-UI-01)
                // (EN) Conditional rendering: if (cond) ... else ... end
                if (check(TT::KEYWORD_IF))
                {
                    children.push_back(parseUIConditional());
                    continue;
                }

                // ── (AR) حلقة رسم: لكل عنصر في قائمة ... نهاية (ADR-UI-01)
                // (EN) Loop rendering: for each item in list ... end
                if (check(TT::KEYWORD_FOR))
                {
                    children.push_back(parseUILoop(/*isForEach=*/true));
                    continue;
                }

                // ── (AR) حلقة رسم: بينما (شرط) ... نهاية (ADR-UI-01)
                // (EN) Loop rendering: while (cond) ... end
                if (check(TT::KEYWORD_WHILE))
                {
                    children.push_back(parseUILoop(/*isForEach=*/false));
                    continue;
                }

                // ── (AR) رمز غير معروف داخل حاوية — توقف
                // (EN) Unknown token inside container — stop
                break;
            }

            return children;
        }

        // =====================================================================
        // parseWidgetExpressionTyped — تحليل عنصر واجهة مع النوع الصحيح (ADR-UI-07)
        // =====================================================================
        //
        // (AR) يُغلّف parseWidgetExpression() ويُعيد unique_ptr<UIWidgetExprNode> مباشرةً
        //      بدون الحاجة إلى dynamic_cast. يُستخدم داخل parseWidgetChildren().
        //      يحتفظ بـ parseWidgetExpression() لأسباب التوافق الخلفي (BF-15).
        //
        // (EN) Wraps parseWidgetExpression() and returns typed unique_ptr<UIWidgetExprNode>
        //      without dynamic_cast. Used inside parseWidgetChildren().
        //      Keeps parseWidgetExpression() for backward compatibility (BF-15).
        // =====================================================================

        std::unique_ptr<UIWidgetExprNode> ParserCore::parseWidgetExpressionTyped()
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
                    do
                    {
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

            // ── (AR) كتلة الأبناء (للحاويات فقط) / (EN) Children block ──
            bool isContainer = containerWidgets.count(widgetName) > 0;

            if (isContainer && !check(TT::PAREN_RIGHT) && !check(TT::DOT))
            {
                if (check(TT::KEYWORD_END))
                {
                    widget->hasChildrenBlock = true;
                    advance(); // (AR) استهلاك 'نهاية'
                }
                else if (!check(TT::END_OF_FILE))
                {
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
        // parseUIConditional — تحليل كتلة رسم شرطي (ADR-UI-01)
        // =====================================================================
        //
        // (AR) القاعدة النحوية:
        //   ui_conditional → 'إذا' '(' تعبير ')' widget_children ('وإلا' widget_children)? 'نهاية'
        //
        // أمثلة:
        //   إذا (مسجّل)
        //       نص("مرحباً!")
        //   وإلا
        //       نص("سجّل أولاً")
        //   نهاية
        //
        // (EN) Grammar:
        //   ui_conditional → 'if' '(' expr ')' widget_children ('else' widget_children)? 'end'
        // =====================================================================

        std::unique_ptr<UIConditionalNode> ParserCore::parseUIConditional()
        {
            auto pos = current_.getPosition();
            advance(); // (AR) استهلاك 'إذا' / (EN) consume 'إذا'

            // ── (AR) تحليل الشرط / (EN) Parse condition ──
            consume(TT::PAREN_LEFT,
                    "(AR) خطأ نحوي: توقع '(' بعد 'إذا' في كتلة الرسم الشرطي.\n"
                    "(EN) Syntax error: expected '(' after 'إذا' in conditional rendering block.");

            auto condition = parseExpression();

            consume(TT::PAREN_RIGHT,
                    "(AR) خطأ نحوي: توقع ')' بعد شرط الرسم الشرطي.\n"
                    "(EN) Syntax error: expected ')' after conditional rendering condition.");

            // ── (AR) تحليل فرع "صحيح" / (EN) Parse "then" branch ──
            std::vector<std::unique_ptr<UIWidgetExprNode>> thenChildren;
            while (!check(TT::KEYWORD_ELSE) && !check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
            {
                if (check(TT::IDENTIFIER) && isKnownWidget(current_.getValue()))
                {
                    thenChildren.push_back(parseWidgetExpressionTyped());
                }
                else if (check(TT::KEYWORD_IF))
                {
                    thenChildren.push_back(parseUIConditional());
                }
                else if (check(TT::KEYWORD_FOR))
                {
                    thenChildren.push_back(parseUILoop(/*isForEach=*/true));
                }
                else if (check(TT::KEYWORD_WHILE))
                {
                    thenChildren.push_back(parseUILoop(/*isForEach=*/false));
                }
                else
                {
                    break;
                }
            }

            // ── (AR) فرع "وإلا" (اختياري) / (EN) Optional "else" branch ──
            std::vector<std::unique_ptr<UIWidgetExprNode>> elseChildren;
            if (check(TT::KEYWORD_ELSE))
            {
                advance(); // (AR) استهلاك 'وإلا' / (EN) consume 'وإلا'

                while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
                {
                    if (check(TT::IDENTIFIER) && isKnownWidget(current_.getValue()))
                    {
                        elseChildren.push_back(parseWidgetExpressionTyped());
                    }
                    else if (check(TT::KEYWORD_IF))
                    {
                        elseChildren.push_back(parseUIConditional());
                    }
                    else if (check(TT::KEYWORD_FOR))
                    {
                        elseChildren.push_back(parseUILoop(/*isForEach=*/true));
                    }
                    else if (check(TT::KEYWORD_WHILE))
                    {
                        elseChildren.push_back(parseUILoop(/*isForEach=*/false));
                    }
                    else
                    {
                        break;
                    }
                }
            }

            // ── (AR) استهلاك 'نهاية' / (EN) consume 'end' ──
            consume(TT::KEYWORD_END,
                    "(AR) خطأ نحوي: توقع 'نهاية' لإغلاق كتلة 'إذا' في الرسم الشرطي.\n"
                    "(EN) Syntax error: expected 'نهاية' to close conditional rendering 'إذا' block.");

            auto node = std::make_unique<UIConditionalNode>(std::move(condition), pos);
            node->thenChildren = std::move(thenChildren);
            node->elseChildren = std::move(elseChildren);
            return node;
        }

        // =====================================================================
        // parseUILoop — تحليل كتلة حلقة رسم (ADR-UI-01)
        // =====================================================================
        //
        // (AR) القاعدة النحوية:
        //   ui_loop_foreach → 'لكل' IDENTIFIER 'في' تعبير widget_children 'نهاية'
        //   ui_loop_while   → 'بينما' '(' تعبير ')' widget_children 'نهاية'
        //
        // أمثلة:
        //   لكل عنصر في القائمة
        //       نص(عنصر)
        //   نهاية
        //
        //   بينما (العدد > 0)
        //       نص("...")
        //   نهاية
        //
        // (EN) Grammar:
        //   ui_loop_foreach → 'foreach' IDENTIFIER 'in' expr widget_children 'end'
        //   ui_loop_while   → 'while' '(' expr ')' widget_children 'end'
        // =====================================================================

        std::unique_ptr<UILoopNode> ParserCore::parseUILoop(bool isForEach)
        {
            auto pos = current_.getPosition();

            if (isForEach)
            {
                advance(); // (AR) استهلاك 'لكل' / (EN) consume 'لكل'

                // ── (AR) اسم المتغير التكراري / (EN) Iterator variable name ──
                if (!check(TT::IDENTIFIER))
                {
                    errorBilingual(
                        "خطأ نحوي: توقع اسم متغير بعد 'لكل' في حلقة الرسم.",
                        "Syntax error: expected variable name after 'لكل' in rendering loop.");
                }
                std::string iterName = current_.getValue();
                advance(); // (AR) استهلاك اسم المتغير

                // ── (AR) كلمة 'في' / (EN) keyword 'في' ──
                consume(TT::KEYWORD_IN,
                        "(AR) خطأ نحوي: توقع 'في' بعد '" + iterName + "' في حلقة 'لكل'.\n"
                                                                      "(EN) Syntax error: expected 'في' after '" +
                            iterName + "' in 'لكل' loop.");

                // ── (AR) التعبير القابل للتكرار / (EN) Iterable expression ──
                auto iterable = parseExpression();

                // ── (AR) تحليل أبناء الحلقة / (EN) Parse loop body children ──
                std::vector<std::unique_ptr<UIWidgetExprNode>> bodyChildren;
                while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
                {
                    if (check(TT::IDENTIFIER) && isKnownWidget(current_.getValue()))
                    {
                        bodyChildren.push_back(parseWidgetExpressionTyped());
                    }
                    else if (check(TT::KEYWORD_IF))
                    {
                        bodyChildren.push_back(parseUIConditional());
                    }
                    else if (check(TT::KEYWORD_FOR))
                    {
                        bodyChildren.push_back(parseUILoop(/*isForEach=*/true));
                    }
                    else if (check(TT::KEYWORD_WHILE))
                    {
                        bodyChildren.push_back(parseUILoop(/*isForEach=*/false));
                    }
                    else
                    {
                        break;
                    }
                }

                consume(TT::KEYWORD_END,
                        "(AR) خطأ نحوي: توقع 'نهاية' لإغلاق حلقة 'لكل' في الرسم.\n"
                        "(EN) Syntax error: expected 'نهاية' to close 'لكل' rendering loop.");

                auto loopNode = UILoopNode::createForEach(iterName, std::move(iterable), pos);
                loopNode->bodyChildren = std::move(bodyChildren);
                return loopNode;
            }
            else
            {
                advance(); // (AR) استهلاك 'بينما' / (EN) consume 'بينما'

                // ── (AR) شرط الحلقة / (EN) Loop condition ──
                consume(TT::PAREN_LEFT,
                        "(AR) خطأ نحوي: توقع '(' بعد 'بينما' في حلقة الرسم.\n"
                        "(EN) Syntax error: expected '(' after 'بينما' in rendering loop.");

                auto condition = parseExpression();

                consume(TT::PAREN_RIGHT,
                        "(AR) خطأ نحوي: توقع ')' بعد شرط 'بينما' في حلقة الرسم.\n"
                        "(EN) Syntax error: expected ')' after 'بينما' rendering loop condition.");

                // ── (AR) تحليل أبناء الحلقة / (EN) Parse loop body children ──
                std::vector<std::unique_ptr<UIWidgetExprNode>> bodyChildren;
                while (!check(TT::KEYWORD_END) && !check(TT::END_OF_FILE))
                {
                    if (check(TT::IDENTIFIER) && isKnownWidget(current_.getValue()))
                    {
                        bodyChildren.push_back(parseWidgetExpressionTyped());
                    }
                    else if (check(TT::KEYWORD_IF))
                    {
                        bodyChildren.push_back(parseUIConditional());
                    }
                    else if (check(TT::KEYWORD_FOR))
                    {
                        bodyChildren.push_back(parseUILoop(/*isForEach=*/true));
                    }
                    else if (check(TT::KEYWORD_WHILE))
                    {
                        bodyChildren.push_back(parseUILoop(/*isForEach=*/false));
                    }
                    else
                    {
                        break;
                    }
                }

                consume(TT::KEYWORD_END,
                        "(AR) خطأ نحوي: توقع 'نهاية' لإغلاق حلقة 'بينما' في الرسم.\n"
                        "(EN) Syntax error: expected 'نهاية' to close 'بينما' rendering loop.");

                auto loopNode = UILoopNode::createWhile(std::move(condition), pos);
                loopNode->bodyChildren = std::move(bodyChildren);
                return loopNode;
            }
        }

    } // namespace Parser
} // namespace Sad
