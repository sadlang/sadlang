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
#include "generated/ui_parser_nodes_generated.h"

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
        // (AR) العناصر الأولية الـ 15 — المجموعة الأساسية فقط (ADR-UI-02)
        // (EN) 15 Primitive Widgets — core set only (ADR-UI-02)
        //
        // (AR) القرار المعماري ADR-UI-02:
        //      تقليص العناصر من 70+ إلى 15 عنصر أولي فقط.
        //      باقي العناصر (55+) تُبنى كـ `واجهة` مُركّبة من الأولية.
        //      لا توافقَ خلفيًّا: لا خريطةَ عناصرَ مُهمَلةٍ ولا تحذيرَ إهمال.
        //
        // (EN) Architecture Decision ADR-UI-02:
        //      Reduce widgets from 70+ to 15 primitives only.
        //      Remaining 55+ widgets are composed as user `واجهة` components.
        //      No back-compat: no deprecated-widget map, no deprecation warning.
        // =====================================================================

        // (AR) لا اسمَ عربيًّا حرفيًّا هنا: المجموعاتُ الثلاثُ تُبنى من قوائمِ
        //      X-macro المولَّدةِ من language-truth/ui_nodes.yaml وui_events.yaml.
        //      فكان الجدولُ اليدويُّ منحرفًا: «نص» بدل «نص_عنصر» القانونيّ،
        //      و«قائمة_كسولة» التي لا وجودَ لها في أيِّ محرّك بدل «عمود_كسول» —
        //      أي إنّ المسارَ التصريحيَّ كان يرفضُ ما يعرفه المحرّكان ويقبلُ ما
        //      لا يعرفانه. الآن يستحيل الانحرافُ: المصدرُ واحد.
        // (EN) No literal Arabic here — the three sets are built from generated
        //      X-macro lists, so parser and engines cannot drift apart.
#define SAD_UI_PARSER_NAME_ENTRY(Id, Name) Name,

        static const std::unordered_set<std::string> knownWidgets = {
            SAD_UI_PARSER_PRIMITIVE_LIST(SAD_UI_PARSER_NAME_ENTRY)};
        // (AR) قائمةٌ فارغةٌ تُترجَم بلا خطأ فتفشلُ صامتةً: بلا أوّليّاتٍ يُرفَض كلُّ
        //      عنصر، وبلا حاوياتٍ تصيرُ كلُّ حاويةٍ عنصرًا ورقيًّا فتُرفَض `نهاية`،
        //      وبلا أحداثٍ يصيرُ كلُّ `.عند_…` معدِّلَ قيمةٍ لا حدثًا. نُفشِل الترجمة.
        // (EN) An empty generated list compiles fine and fails silently — assert.
        static_assert(SAD_UI_PARSER_PRIMITIVE_COUNT > 0,
                      "قائمةُ العناصرِ الأوّليّةِ المولَّدةُ فارغة");

        // (AR) العناصر التي تقبل أبناء (حاويات) — مولَّدة من parser_container
        // (EN) Container widgets — generated from the parser_container flag
        static const std::unordered_set<std::string> containerWidgets = {
            SAD_UI_PARSER_CONTAINER_LIST(SAD_UI_PARSER_NAME_ENTRY)};
        static_assert(SAD_UI_PARSER_CONTAINER_COUNT > 0,
                      "قائمةُ الحاوياتِ المولَّدةُ فارغة");
        static_assert(SAD_UI_PARSER_CONTAINER_COUNT <= SAD_UI_PARSER_PRIMITIVE_COUNT,
                      "الحاوياتُ يجب أن تكون مجموعةً جزئيّةً من الأوّليّات");

        // (AR) أسماء الأحداث المعروفة — تُميّز المعدّل-الحدث من المعدّل-القيمة.
        //      مولَّدة كاملةً من language-truth/ui_events.yaml (لا مجموعةً جزئيّة).
        // (EN) Known event names — generated in full from ui_events.yaml.
        static const std::unordered_set<std::string> knownEvents = {
            SAD_UI_PARSER_EVENT_LIST(SAD_UI_PARSER_NAME_ENTRY)};
        static_assert(SAD_UI_PARSER_EVENT_COUNT > 0,
                      "قائمةُ الأحداثِ المولَّدةُ فارغة");

#undef SAD_UI_PARSER_NAME_ENTRY

        // =====================================================================
        // isKnownWidget — التحقق إذا كان الاسم عنصر واجهة مسجل
        // =====================================================================
        //
        // (AR) يتحقّق من العناصر الأوّليّة المولَّدة وحدَها (ADR-UI-02)
        // (EN) Checks the generated primitives only (ADR-UI-02)
        // =====================================================================

        bool ParserCore::isKnownWidget(const std::string &name) const
        {
            // (AR) لا توافقَ خلفيًّا: الاسمُ الأوّليُّ القانونيُّ وحدَه يُقبَل، ولا
            //      خريطةَ مُهمَلٍ ولا تحذيرَ إهمال (قرار مالك).
            // (EN) No back-compat: canonical primitives only.
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
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "نوع الحالة"}, {"what_en", "state kind"}, {"ctx_ar", "بعد '@' داخل '" + kw(TT::KEYWORD_UI_DECL) + "' — الأنواع المسموحة: @حالة, @ربط, @بيئة, @محسوب"}, {"ctx_en", "after '@' inside '" + kw(TT::KEYWORD_UI_DECL) + "' — allowed: @حالة, @ربط, @بيئة, @محسوب"}});
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
                    // (AR) RFC 0034: التصريح الخارجيّ بلا جسم لا يصلح طريقة واجهة — رفض صريح.
                    // (EN) RFC 0034: a body-less extern is not a UI method — reject.
                    auto *fnDecl = dynamic_cast<AST::FunctionDecl *>(method.get());
                    if (fnDecl && fnDecl->isExtern)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_DECL_NOT_ALLOWED_HERE, {{"decl_ar", "التصريح الخارجيّ '" + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + "' (بلا جسم)"}, {"decl_en", "the body-less extern declaration '" + kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + "'"}, {"where_ar", "واجهة"}, {"where_en", "a UI declaration"}, {"example", kw(TT::KEYWORD_FUNCTION) + " " + kwAlias(TT::KEYWORD_EXTERN) + " printf(" + kw(TT::TYPE_STRING) + ")"}});
                    }
                    else if (method)
                    {
                        uiDecl->methods.push_back(std::move(method));
                    }
                    continue;
                }

                // ── (AR) أي شيء آخر هو خطأ ──
                errorCatalog(Errors::ErrorCode::SYN_UNKNOWN_ELEMENT, {{"what_ar", "العنصر داخل '" + kw(TT::KEYWORD_UI_DECL) + "'"}, {"what_en", "element inside '" + kw(TT::KEYWORD_UI_DECL) + "'"}, {"found", current_.getValue()}, {"allowed", "@حالة, @ربط, @بيئة, @محسوب, " + kw(TT::KEYWORD_FUNCTION)}});
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
                errorCatalog(Errors::ErrorCode::SYN_UNKNOWN_ELEMENT, {{"what_ar", "نوع الحالة"}, {"what_en", "state kind"}, {"found", "@" + stateKind}, {"allowed", "@حالة, @ربط, @بيئة, @محسوب"}});
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
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "في '@محسوب " + nameToken.getValue() + "' — الصيغة: @محسوب " + nameToken.getValue() + " = تعبير"}, {"ctx_en", "in '@محسوب " + nameToken.getValue() + "' — form: @محسوب " + nameToken.getValue() + " = expression"}});
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
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "المعدِّل"}, {"what_en", "modifier"}, {"ctx_ar", "بعد '.' في سلسلة المعدِّلات"}, {"ctx_en", "after '.' in a modifier chain"}});
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
                // (AR) ISSUE-120 — جسمُ معالِجِ الحدث يُبنى باليد ولا يمرّ بـ
                //      `parseBlockStmt`، فيُحرَس كي يعدَّه عدّادُ الكتل: «متغير ساكن»
                //      داخله محلّيٌّ لا عامّ، فيجب أن يبلغ SEM039 كنظائره.
                // (EN) Hand-rolled event-handler body — guard it for the block counter.
                BlockDepthGuard eventBodyGuard(blockDepth_);
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
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "متغير"}, {"what_en", "variable"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_FOR) + "' في حلقة الرسم"}, {"ctx_en", "after '" + kw(TT::KEYWORD_FOR) + "' in a rendering loop"}});
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
