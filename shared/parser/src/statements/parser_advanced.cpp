/**
 * @file parser_core.cpp
 * @brief (AR) تنفيذ المحلل النحوي الأساسي - تحويل الرموز إلى شجرة AST.
 *        (EN) Parser core implementation - converting tokens to AST.
 *
 * (AR) يحتوي هذا الملف على تنفيذات جميع دوال المحلل النحوي.
 *      يستخدم تقنية Recursive Descent Parsing مع معالجة الأخطاء والتعافي منها.
 *
 * (EN) This file contains implementations of all parser functions.
 *      Uses Recursive Descent Parsing with error handling and recovery.
 *
 * @author Sad Language Development Team
 * @date 2025-11-11
 * @license MIT License
 */

#include <string>
#include "parser_core.h"
#include "advanced_expr_nodes.h"
#include <iostream>
#include <sstream>

namespace Sad
{
    namespace Parser
    {

        using namespace AST;
        using namespace Lexer;
        using TT = TokenType;

        /**
         * @brief (AR) يحلل مصفوفة حرفية أو استيعاب قائمة: [1، 2، 3] أو [لكل س في مصدر أنتج س * 2].
         *        (EN) Parses array literal or list comprehension: [1, 2, 3] or [لكل var في iterable أنتج var * 2].
         */
        ExprPtr ParserCore::parseArrayLiteral()
        {
            // Check for empty array
            // (AR) التحقق من مصفوفة فارغة
            if (check(TT::BRACKET_RIGHT))
            {
                consume(TT::BRACKET_RIGHT, "");
                return std::make_unique<ArrayExpr>(ExprList{}, previous().getPosition());
            }

            // (AR) استيعاب قائمة بالترتيب العربيّ: [لكل س في مصدر [إذا شرط] أنتج تعبير]
            //      يُكتشف الاستيعاب مبكّرًا عبر 'لكل' في البداية — لا لبس مع مصفوفة عاديّة.
            // (EN) List comprehension in Arabic order: [for var in iterable [if cond] yield expr]
            //      Detected early via leading 'لكل' — no ambiguity with a regular array.
            if (check(TT::KEYWORD_FOR))
            {
                advance(); // consume 'لكل'

                // (AR) اسم متغيّر الحلقة / (EN) loop variable name
                if (!check(TT::IDENTIFIER))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "متغير"}, {"what_en", "variable"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_FOR) + "' في استيعاب القائمة"}, {"ctx_en", "after '" + kw(TT::KEYWORD_FOR) + "' in a list comprehension"}});
                    return nullptr;
                }
                Token var = peek();
                advance();

                // (AR) فكّ زوج اختياريّ: «لكل مفتاح، قيمة في خريطة» — الفاصلة تُدخِل متغيّر القيمة.
                //      يوازي حلقة «لكل» (parseForStmt) ويعمل على الخرائط فقط دلاليًّا.
                // (EN) Optional pair-unpack: "for key, value in map" — a comma introduces the value
                //      variable. Mirrors the «for» loop (parseForStmt); semantically map-only.
                std::string listValueVar;
                if (matchComma())
                {
                    if (!check(TT::IDENTIFIER))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "متغير القيمة"}, {"what_en", "value variable"}, {"ctx_ar", "بعد '،' في استيعاب القائمة"}, {"ctx_en", "after ',' in a list comprehension"}});
                        return nullptr;
                    }
                    listValueVar = peek().getValue();
                    advance();
                }

                // (AR) توقّع 'في' / (EN) expect 'في'
                if (!check(TT::KEYWORD_IN))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_IN)}, {"ctx_ar", "بعد اسم المتغير في استيعاب القائمة"}, {"ctx_en", "after the variable name in a list comprehension"}});
                    return nullptr;
                }
                advance();

                // (AR) المصدر القابل للتكرار / (EN) iterable source
                auto iterable = parseExpression();
                if (!iterable)
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "قابلاً للتكرار بعد '" + kw(TT::KEYWORD_IN) + "' في استيعاب القائمة"}, {"ctx_en", "(iterable) after '" + kw(TT::KEYWORD_IN) + "' in a list comprehension"}});
                    return nullptr;
                }

                // (AR) شرط اختياريّ قبل 'أنتج' / (EN) optional condition before 'أنتج'
                ExprPtr condition = nullptr;
                if (match(TT::KEYWORD_IF))
                {
                    condition = parseExpression();
                    if (!condition)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "شرطيًا صالحًا بعد '" + kw(TT::KEYWORD_IF) + "' في استيعاب القائمة"}, {"ctx_en", "(valid condition) after '" + kw(TT::KEYWORD_IF) + "' in a list comprehension"}});
                        return nullptr;
                    }
                }

                // (AR) 'أنتج' تفصل رأس الحلقة عن تعبير الناتج (KEYWORD_YIELD، سياقيّة)
                // (EN) 'أنتج' separates the loop head from the output expression
                if (!match(TT::KEYWORD_YIELD) && !matchContextual(TT::KEYWORD_YIELD))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_YIELD)}, {"ctx_ar", "قبل تعبير الناتج في استيعاب القائمة"}, {"ctx_en", "before the output expression in a list comprehension"}});
                    return nullptr;
                }

                // (AR) تعبير الناتج / (EN) output expression
                auto elemExpr = parseExpression();
                if (!elemExpr)
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "للناتج بعد '" + kw(TT::KEYWORD_YIELD) + "' في استيعاب القائمة"}, {"ctx_en", "(output) after '" + kw(TT::KEYWORD_YIELD) + "' in a list comprehension"}});
                    return nullptr;
                }

                if (!check(TT::BRACKET_RIGHT))
                {
                    // (AR) محاولة التعافي: إدراج ']' المفقودة / (EN) recover missing ']'
                    if (!tryRecoverFromError(TT::BRACKET_RIGHT, "list comprehension"))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "]"}, {"ctx_ar", "في نهاية استيعاب القائمة"}, {"ctx_en", "at the end of a list comprehension"}});
                        return nullptr;
                    }
                }
                else
                {
                    consume(TT::BRACKET_RIGHT, "");
                }

                // (AR) إنشاء عقدة استيعاب القائمة (نفس عقدة AST — الترتيب فقط تغيّر)
                // (EN) Build the list comprehension node (same AST node — only order changed)
                auto listComp = std::make_unique<ListComprehensionExpr>(
                    std::move(elemExpr),
                    var.getValue(),
                    std::move(iterable),
                    std::move(condition),
                    var.getPosition());
                listComp->valueVariable = listValueVar; // (AR) متغيّر القيمة (فارغ إن غاب) / (EN) value var (empty if absent)
                return listComp;
            }

            // Parse first element/expression (regular array)
            // (AR) تحليل العنصر/التعبير الأول (مصفوفة عاديّة)
            auto firstExpr = parseExpression();
            if (!firstExpr)
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "صالحًا في أول عنصر من المصفوفة"}, {"ctx_en", "(valid) as the first array element"}});
                return nullptr;
            }

            // Regular array literal
            // (AR) مصفوفة حرفية عادية
            ExprList elements;
            elements.push_back(std::move(firstExpr));

            // Parse remaining elements
            // (AR) تحليل العناصر المتبقية
            while (matchComma())
            {
                if (check(TT::BRACKET_RIGHT))
                {
                    break; // Trailing comma is allowed
                }

                auto elem = parseExpression();
                if (!elem)
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "صالحًا في عنصر المصفوفة"}, {"ctx_en", "(valid) in an array element"}});
                    return nullptr;
                }
                elements.push_back(std::move(elem));
            }

            if (!check(TT::BRACKET_RIGHT))
            {
                // (AR) محاولة التعافي: إدراج ']' المفقودة
                // (EN) Try recovery: insert missing ']'
                if (tryRecoverFromError(TT::BRACKET_RIGHT, "array literal"))
                {
                    // (AR) التعافي نجح — نكمل كأن ']' موجودة
                    // (EN) Recovery succeeded — continue as if ']' was present
                }
                else
                {
                    errorCatalog(Errors::ErrorCode::SYN_UNCLOSED_CONSTRUCT, {{"construct_ar", "المصفوفة"}, {"construct_en", "array"}, {"closer", "]"}});
                    return nullptr;
                }
            }
            else
            {
                consume(TT::BRACKET_RIGHT, "");
            }

            // Create array expression node
            // (AR) إنشاء عقدة تعبير المصفوفة
            return std::make_unique<ArrayExpr>(
                std::move(elements),
                previous().getPosition());
        }

        /**
         * @brief (AR) يحلل خريطة حرفية أو استيعاب قاموس/مجموعة: {م: ق} أو {لكل س في مصدر أنتج م: ق} أو {لكل س في مصدر أنتج تعبير}.
         *        (EN) Parses map literal or dict/set comprehension: {k: v} or {لكل var في iterable أنتج k: v} or {لكل var في iterable أنتج expr}.
         */
        ExprPtr ParserCore::parseMapLiteral()
        {
            // Check for empty map/set
            // (AR) التحقق من خريطة/مجموعة فارغة
            if (check(TT::BRACE_RIGHT))
            {
                consume(TT::BRACE_RIGHT, "");
                // Empty {} defaults to empty map (like Python)
                // (AR) {} فارغة تُعتبر خريطة فارغة (مثل Python)
                return std::make_unique<MapExpr>(std::vector<MapPair>{}, previous().getPosition());
            }

            // (AR) التحقق من spread كأول عنصر / (EN) Check for spread as first element
            if (check(TT::ELLIPSIS))
            {
                advance(); // consume '...'
                auto spreadExpr = parseTernary();
                if (!spreadExpr)
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "بعد '...' في الخريطة"}, {"ctx_en", "after '...' in a map"}});
                    return nullptr;
                }

                // (AR) خريطة تبدأ بـ spread — تُعامل كـ map literal مع spread
                // (EN) Map starting with spread — treat as map literal with spread
                std::vector<MapPair> pairs;
                pairs.emplace_back(std::move(spreadExpr)); // spread: key=nullptr

                // (AR) تحليل المزيد من الأزواج
                while (matchComma())
                {
                    if (check(TT::BRACE_RIGHT))
                        break;

                    if (check(TT::ELLIPSIS))
                    {
                        advance();
                        auto nextSpread = parseTernary();
                        if (nextSpread)
                            pairs.emplace_back(std::move(nextSpread));
                        continue;
                    }

                    auto key = parseTernary();
                    if (!key)
                        break;
                    if (!check(TT::COLON) && !check(TT::OP_ASSIGN))
                        break;
                    advance();
                    auto value = parseExpression();
                    if (!value)
                        break;
                    pairs.emplace_back(std::move(key), std::move(value));
                }

                consume(TT::BRACE_RIGHT, "");
                return std::make_unique<MapExpr>(std::move(pairs), previous().getPosition());
            }

            // (AR) استيعاب مجموعة/قاموس بالترتيب العربيّ — يُكتشف مبكّرًا عبر 'لكل' في البداية:
            //      مجموعة: {لكل س في مصدر [إذا شرط] أنتج تعبير}
            //      قاموس:  {لكل س في مصدر [إذا شرط] أنتج مفتاح: قيمة}
            // (EN) Set/dict comprehension in Arabic order — detected early via leading 'لكل'.
            if (check(TT::KEYWORD_FOR))
            {
                advance(); // consume 'لكل'

                // (AR) متغيّر الحلقة / (EN) loop variable
                if (!check(TT::IDENTIFIER))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "متغير"}, {"what_en", "variable"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_FOR) + "' في الاستيعاب"}, {"ctx_en", "after '" + kw(TT::KEYWORD_FOR) + "' in a comprehension"}});
                    return nullptr;
                }
                Token loopVar = peek();
                advance();

                // (AR) فكّ زوج اختياريّ: «لكل مفتاح، قيمة في خريطة» (مجموعة وقاموس معًا).
                // (EN) Optional pair-unpack: "for key, value in map" (both set and dict).
                std::string compValueVar;
                if (matchComma())
                {
                    if (!check(TT::IDENTIFIER))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "متغير القيمة"}, {"what_en", "value variable"}, {"ctx_ar", "بعد '،' في الاستيعاب"}, {"ctx_en", "after ',' in a comprehension"}});
                        return nullptr;
                    }
                    compValueVar = peek().getValue();
                    advance();
                }

                // (AR) توقّع 'في' / (EN) expect 'في'
                if (!check(TT::KEYWORD_IN))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_IN)}, {"ctx_ar", "بعد متغير الحلقة في الاستيعاب"}, {"ctx_en", "after the loop variable in a comprehension"}});
                    return nullptr;
                }
                advance();

                // (AR) المصدر القابل للتكرار / (EN) iterable source
                auto iterable = parseExpression();
                if (!iterable)
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "قابلاً للتكرار في الاستيعاب"}, {"ctx_en", "(iterable) in a comprehension"}});
                    return nullptr;
                }

                // (AR) شرط اختياريّ قبل 'أنتج' / (EN) optional condition before 'أنتج'
                ExprPtr condition = nullptr;
                if (match(TT::KEYWORD_IF))
                {
                    condition = parseExpression();
                    if (!condition)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "شرطيًا صالحًا بعد '" + kw(TT::KEYWORD_IF) + "' في الاستيعاب"}, {"ctx_en", "(valid condition) after '" + kw(TT::KEYWORD_IF) + "' in a comprehension"}});
                        return nullptr;
                    }
                }

                // (AR) 'أنتج' تفصل رأس الحلقة عن الناتج / (EN) 'أنتج' separates loop head from output
                if (!match(TT::KEYWORD_YIELD) && !matchContextual(TT::KEYWORD_YIELD))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_YIELD)}, {"ctx_ar", "قبل الناتج في الاستيعاب"}, {"ctx_en", "before the output in a comprehension"}});
                    return nullptr;
                }

                // (AR) الناتج الأوّل — عنصر مجموعة أو مفتاح قاموس (parseTernary لتجنّب التباس ':')
                // (EN) First output — set element or dict key (parseTernary to avoid ':' clash)
                auto firstOut = parseTernary();
                if (!firstOut)
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "للناتج بعد '" + kw(TT::KEYWORD_YIELD) + "' في الاستيعاب"}, {"ctx_en", "(output) after '" + kw(TT::KEYWORD_YIELD) + "' in a comprehension"}});
                    return nullptr;
                }

                // (AR) وجود ':' بعد الناتج ⇒ استيعاب قاموس، وإلّا استيعاب مجموعة
                // (EN) A ':' after the output ⇒ dict comprehension, else set comprehension
                if (check(TT::COLON) || check(TT::OP_ASSIGN))
                {
                    advance(); // consume ':' or '='
                    auto valueExpr = parseExpression();
                    if (!valueExpr)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "لقيمة القاموس بعد ':' في الاستيعاب"}, {"ctx_en", "(dict value) after ':' in a comprehension"}});
                        return nullptr;
                    }

                    if (!check(TT::BRACE_RIGHT))
                    {
                        if (!tryRecoverFromError(TT::BRACE_RIGHT, "dict comprehension"))
                        {
                            errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "}"}, {"ctx_ar", "في نهاية استيعاب القاموس"}, {"ctx_en", "at the end of a dict comprehension"}});
                            return nullptr;
                        }
                    }
                    else
                    {
                        consume(TT::BRACE_RIGHT, "");
                    }

                    // (AR) عقدة استيعاب القاموس (نفس عقدة AST — الترتيب فقط تغيّر)
                    auto dictComp = std::make_unique<DictComprehensionExpr>(
                        std::move(firstOut),
                        std::move(valueExpr),
                        loopVar.getValue(),
                        std::move(iterable),
                        std::move(condition),
                        loopVar.getPosition());
                    dictComp->valueVariable = compValueVar; // (AR) متغيّر القيمة (فارغ إن غاب) / (EN) value var (empty if absent)
                    return dictComp;
                }

                // (AR) استيعاب مجموعة / (EN) set comprehension
                if (!check(TT::BRACE_RIGHT))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "}"}, {"ctx_ar", "في نهاية استيعاب المجموعة"}, {"ctx_en", "at the end of a set comprehension"}});
                    return nullptr;
                }
                consume(TT::BRACE_RIGHT, "");

                auto setComp = std::make_unique<SetComprehensionExpr>(
                    std::move(firstOut),
                    loopVar.getValue(),
                    std::move(iterable),
                    std::move(condition));
                setComp->valueVariable = compValueVar; // (AR) متغيّر القيمة (فارغ إن غاب) / (EN) value var (empty if absent)
                return setComp;
            }

            // Parse first expression using parseTernary to avoid consuming 'for' keyword
            // (AR) تحليل التعبير الأول باستخدام parseTernary لتجنب استهلاك 'for'
            auto firstKey = parseTernary();
            if (!firstKey)
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "صالحًا في هذا الموضع"}, {"ctx_en", "(valid) at this position"}});
                return nullptr;
            }

            // (AR) الاستيعابات (قائمة/مجموعة/قاموس) تُكتشف مبكّرًا عبر 'لكل' في البداية،
            //      لذا هنا يكون العنصر الأوّل دائمًا مفتاح خريطة عاديّ — يجب أن يتبعه ':' أو '='.
            // (EN) Comprehensions are detected earlier via leading 'لكل', so the first element
            //      here is always a plain map key — it must be followed by ':' or '='.
            if (!check(TT::COLON) && !check(TT::OP_ASSIGN))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ":"}, {"ctx_ar", "بعد مفتاح الخريطة"}, {"ctx_en", "after the map key"}});
                return nullptr;
            }
            advance(); // consume ':' or '='

            // Parse first value expression using parseTernary to avoid consuming 'for'
            // (AR) تحليل تعبير القيمة باستخدام parseTernary لتجنب استهلاك 'for'
            auto firstValue = parseTernary();
            if (!firstValue)
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "صالحًا لقيمة الخريطة"}, {"ctx_en", "(valid map value)"}});
                return nullptr;
            }

            // (AR) استيعاب القاموس يُكتشف مبكّرًا عبر 'لكل' في البداية (أعلاه)؛ هنا خريطة عاديّة فقط.
            // (EN) Dict comprehension is detected earlier via leading 'لكل' (above); only a plain map here.

            // Regular map literal
            // (AR) خريطة حرفية عادية
            std::vector<MapPair> pairs;
            pairs.emplace_back(std::move(firstKey), std::move(firstValue));

            // Parse remaining key-value pairs
            // (AR) تحليل أزواج المفتاح-القيمة المتبقية
            while (matchComma())
            {
                if (check(TT::BRACE_RIGHT))
                {
                    break; // Trailing comma is allowed
                }

                // (AR) التحقق من spread operator (...) / (EN) Check for spread operator
                if (check(TT::ELLIPSIS))
                {
                    advance(); // consume '...'
                    auto spreadExpr = parseTernary();
                    if (!spreadExpr)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "بعد '...' في الخريطة"}, {"ctx_en", "after '...' in a map"}});
                        return nullptr;
                    }
                    pairs.emplace_back(std::move(spreadExpr)); // spread: key=nullptr
                    continue;
                }

                // (AR) استخدم parseTernary بدلاً من parseExpression لتجنب تفسير = كإسناد
                // (EN) Use parseTernary instead of parseExpression to avoid = being parsed as assignment
                auto key = parseTernary();
                if (!key)
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "لمفتاح إضافي في الخريطة"}, {"ctx_en", "(additional map key)"}});
                    return nullptr;
                }

                if (!check(TT::COLON) && !check(TT::OP_ASSIGN))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ":"}, {"ctx_ar", "بعد مفتاح الخريطة — هل نسيت الفاصلة قبل المفتاح التالي؟"}, {"ctx_en", "after the map key — did you forget the comma before the next key?"}});
                    return nullptr;
                }
                advance(); // consume ':' or '='

                auto value = parseExpression();
                if (!value)
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "لقيمة إضافية في الخريطة"}, {"ctx_en", "(additional map value)"}});
                    return nullptr;
                }

                pairs.emplace_back(std::move(key), std::move(value));
            }

            if (!check(TT::BRACE_RIGHT))
            {
                // (AR) محاولة التعافي: إدراج '}' المفقودة
                // (EN) Try recovery: insert missing '}'
                if (!tryRecoverFromError(TT::BRACE_RIGHT, "map literal"))
                {
                    errorCatalog(Errors::ErrorCode::SYN_UNCLOSED_CONSTRUCT, {{"construct_ar", "الخريطة"}, {"construct_en", "map"}, {"closer", "}"}});
                    return nullptr;
                }
            }
            else
            {
                consume(TT::BRACE_RIGHT, "");
            }

            // Create map expression node
            // (AR) إنشاء عقدة تعبير الخريطة
            return std::make_unique<MapExpr>(
                std::move(pairs),
                previous().getPosition());
        }

        // ============================================================================
        // (AR) تحليل Pattern Matching / (EN) Pattern Matching Parsing
        // ============================================================================

        /**
         * @brief (AR) يحلل جملة match لمطابقة الأنماط
         *        (EN) Parses match statement for pattern matching
         */
        StmtPtr ParserCore::parseMatchStmt()
        {
            // Already consumed 'match'
            // (AR) تم استهلاك 'match' بالفعل

            // Parse value to match against
            // (AR) تحليل القيمة المُختبرة
            auto value = parseExpression();

            if (!value)
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "بعد '" + kw(TT::KEYWORD_MATCH) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_MATCH) + "'"}});
                return nullptr;
            }

            // (AR) صيغة {} أُزيلت — استخدم 'نهاية' فقط
            // (EN) Brace syntax removed — use 'نهاية' only
            if (check(TT::BRACE_LEFT))
            {
                errorCatalog(Errors::ErrorCode::SYN_REMOVED_SYNTAX, {{"old", kw(TT::KEYWORD_MATCH) + " { ... }"}, {"new", kw(TT::KEYWORD_END)}, {"example", kw(TT::KEYWORD_MATCH) + " (قيمة)\n " + kw(TT::KEYWORD_WHEN) + " 1: ...\n" + kw(TT::KEYWORD_END)}});
            }

            // Parse case clauses
            // (AR) تحليل فروع case
            std::vector<AST::CaseClause> cases;

            auto isMatchEnd = [&]() -> bool
            {
                return check(TT::KEYWORD_END);
            };

            while (!isMatchEnd() && !isAtEnd())
            {
                // (AR) فروع طابق: تُقبل `عندما` فقط
                // (EN) Match branches: only `عندما` is accepted
                if (check(TT::KEYWORD_CASE) || checkContextual(TT::KEYWORD_CASE))
                {
                    // (AR) كانت رسالةً ثنائيّةً **مكتوبةً في الكود بلا رمزِ خطأ**، فلا يراها
                    //      sad-check --json ولا الـLSP ولا تخضع لمستوى الشرح. SYN019 يحمل
                    //      المعنى. وحشواتُ SYN031 كلُّها تهجئةُ معجمٍ من kw() — لا اسمَ فئةٍ
                    //      إنجليزيًّا يُكتب هنا، فالنصُّ كلُّه من مصدر الحقيقة.
                    // (EN) This was a bilingual message hard-written with NO error code, so
                    //      neither sad-check --json nor the LSP could see it. Every SYN031
                    //      placeholder is a lexicon spelling from kw() — no prose in code.
                    errorCatalog(Errors::ErrorCode::SYN_MATCH_WRONG_ARM_KEYWORD,
                                 {{"found", kw(TT::KEYWORD_CASE)},
                                  {"match_kw", kw(TT::KEYWORD_MATCH)},
                                  {"when_kw", kw(TT::KEYWORD_WHEN)},
                                  {"default_kw", kw(TT::KEYWORD_DEFAULT)}});
                    // recover: treat as عندما
                    advance(); // skip حالة/KEYWORD_CASE
                    cases.push_back(parseCaseClause());
                }
                else if (check(TT::KEYWORD_WHEN))
                {
                    cases.push_back(parseCaseClause());
                }
                else if (match(TT::KEYWORD_DEFAULT))
                {
                    // (AR) تحليل الحالة الافتراضية — تعامل كـ WildcardPattern
                    // (EN) Parse default case — treat as WildcardPattern
                    // (AR) نقطتان اختياريّتان بعد 'افتراضي' (gr.stmt.match: COLON optional)،
                    //      اتّساقًا مع اختياريّتها بعد نمط 'عندما'. نستهلكها إن وُجدت.
                    // (EN) Optional colon after 'default' (gr.stmt.match: COLON optional),
                    //      mirroring its optionality after a 'when' pattern. Consume if present.
                    match(TT::COLON);
                    // (AR) تحليل جسم الحالة الافتراضية
                    // (EN) Parse default case body
                    // (AR) يقف الجسم عند «عندما» أيضًا: ذراعٌ بعد «افتراضي» خطأٌ نحويّ
                    //      (SYN032)، وكان الجسمُ يبتلعها فيقع التشخيصُ SYN001 على النقطتين
                    //      في السطر التالي — بعيدًا عن السبب. والأذرعُ المتداخلةُ لا تبلغ
                    //      هذا الشرط، إذ تستهلكها `parseDeclaration` مع جملتها.
                    // (EN) The body also stops at «عندما»: an arm after «افتراضي» is a
                    //      syntax error (SYN032); the body used to swallow it so SYN001
                    //      landed on the next line's colon, far from the cause. Nested arms
                    //      never reach this check — parseDeclaration consumes them with
                    //      their own statement.
                    std::vector<StmtPtr> defaultBody;
                    while (!isMatchEnd() && !check(TT::KEYWORD_WHEN) && !isAtEnd())
                    {
                        auto stmt = parseDeclaration();
                        if (stmt)
                        {
                            defaultBody.push_back(std::move(stmt));
                        }
                        else
                        {
                            synchronize();
                        }
                    }
                    auto wildcardPattern = std::make_unique<AST::WildcardPattern>();
                    cases.push_back(AST::CaseClause(
                        std::move(wildcardPattern),
                        nullptr,
                        std::move(defaultBody)));

                    // (AR) «افتراضي» آخرُ الأذرع. فإن تلاه «عندما» رُفع SYN032 ثمّ تُوبع
                    //      التحليلُ تعافيًا كي تُقرأ الأذرعُ الباقيةُ في موضعها لا داخلَه.
                    // (EN) «افتراضي» comes last. If a «عندما» follows, raise SYN032 and keep
                    //      parsing for recovery so the remaining arms are read in their own
                    //      position rather than inside the default body.
                    if (check(TT::KEYWORD_WHEN))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_DEFAULT_CLAUSE_NOT_LAST,
                                     {{"construct_kw", kw(TT::KEYWORD_MATCH)},
                                      {"when_kw", kw(TT::KEYWORD_WHEN)},
                                      {"default_kw", kw(TT::KEYWORD_DEFAULT)}});
                        continue;
                    }

                    break; // (AR) الافتراضي يجب أن يكون الأخير / (EN) Default must be last
                }
                else
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_WHEN)}, {"ctx_ar", "أو '" + kw(TT::KEYWORD_CASE) + "' أو '" + kw(TT::KEYWORD_DEFAULT) + "' أو '" + kw(TT::KEYWORD_END) + "' داخل '" + kw(TT::KEYWORD_MATCH) + "'"}, {"ctx_en", "or '" + kw(TT::KEYWORD_CASE) + "' or '" + kw(TT::KEYWORD_DEFAULT) + "' or '" + kw(TT::KEYWORD_END) + "' inside '" + kw(TT::KEYWORD_MATCH) + "'"}});
                    return nullptr;
                }
            }

            if (cases.empty())
            {
                errorCatalog(Errors::ErrorCode::SYN_EMPTY_CONSTRUCT, {{"construct_ar", "جملة '" + kw(TT::KEYWORD_MATCH) + "'"}, {"construct_en", "'" + kw(TT::KEYWORD_MATCH) + "' statement"}, {"required_ar", "فرع '" + kw(TT::KEYWORD_WHEN) + "' واحد"}, {"required_en", "one '" + kw(TT::KEYWORD_WHEN) + "' clause"}});
                return nullptr;
            }

            // Expect closing: 'نهاية'
            // (AR) توقع إنهاء: 'نهاية'
            if (!match(TT::KEYWORD_END))
            {
                errorCatalog(Errors::ErrorCode::SYN_UNCLOSED_CONSTRUCT, {{"construct_ar", "جملة '" + kw(TT::KEYWORD_MATCH) + "'"}, {"construct_en", "'" + kw(TT::KEYWORD_MATCH) + "' statement"}, {"closer", kw(TT::KEYWORD_END)}});
                return nullptr;
            }

            return std::make_unique<AST::MatchStmt>(
                std::move(value),
                std::move(cases),
                previous().getPosition());
        }

        /**
         * @brief (AR) يحلل فرع case واحد
         *        (EN) Parses one case clause
         */
        AST::CaseClause ParserCore::parseCaseClause()
        {
            // Consume 'عندما' only
            // (AR) استهلاك 'عندما' فقط
            if (!match(TT::KEYWORD_WHEN))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_WHEN)}, {"ctx_ar", "في هذا الموضع"}, {"ctx_en", "at this position"}});
                return AST::CaseClause(nullptr, nullptr, {});
            }

            // Parse pattern
            // (AR) تحليل النمط
            auto pattern = parsePattern();

            if (!pattern)
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "(نمطًا) بعد '" + kw(TT::KEYWORD_CASE) + "'"}, {"ctx_en", "(pattern) after '" + kw(TT::KEYWORD_CASE) + "'"}});
                return AST::CaseClause(nullptr, nullptr, {});
            }

            // Parse optional guard (if condition)
            // (AR) تحليل guard اختياري (شرط if)
            ExprPtr guard = nullptr;
            if (match(TT::KEYWORD_IF))
            {
                guard = parseExpression();

                if (!guard)
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "بعد '" + kw(TT::KEYWORD_IF) + "' في الحارس"}, {"ctx_en", "after '" + kw(TT::KEYWORD_IF) + "' in a guard"}});
                }
            }

            // (AR) نقطتان اختياريّتان بعد النمط (gr.stmt.match: COLON optional). جسم
            //      الذراع يُغلَق ضمنيًّا بوصول عندما/حالة/افتراضي/نهاية التالية، فلا حاجة
            //      لإلزام ':'. نستهلكها إن وُجدت ونتجاوز غيابها بلا خطأ.
            // (EN) Optional colon after the pattern (gr.stmt.match: COLON is optional). The
            //      arm body is delimited implicitly by the next when/case/default/end, so ':'
            //      is not required; consume it if present, tolerate its absence.
            match(TT::COLON);

            // Parse body - multiple statements until next 'case' or 'end' or '}'
            // (AR) تحليل الجسم - جمل متعددة حتى 'حالة' أو 'نهاية' أو '}' التالية
            std::vector<StmtPtr> body;

            while (!check(TT::KEYWORD_CASE) && !check(TT::KEYWORD_WHEN) && !check(TT::KEYWORD_DEFAULT) && !check(TT::KEYWORD_END) &&
                   !checkContextual(TT::KEYWORD_CASE) &&
                   !check(TT::BRACE_RIGHT) && !isAtEnd())
            {
                auto stmt = parseDeclaration();
                if (stmt)
                {
                    body.push_back(std::move(stmt));
                }
                else
                {
                    synchronize();
                }
            }

            return AST::CaseClause(
                std::move(pattern),
                std::move(guard),
                std::move(body));
        }

        /**
         * @brief (AR) يحلل نمط
         *        (EN) Parses a pattern
         */
        std::unique_ptr<AST::Pattern> ParserCore::parsePattern()
        {
            // Wildcard: _ (using IDENTIFIER)
            // (AR) النمط الشامل: _ (باستخدام IDENTIFIER)
            // (AR) رمز ASCII خاص للأنماط الشاملة — ليس كلمة عربية
            if (check(TT::IDENTIFIER) && current_.getValue() == "_")
            {
                advance();
                return std::make_unique<AST::WildcardPattern>();
            }

            // Struct pattern: { field: pattern, ... }
            // (AR) نمط بنية: { حقل: نمط، ... }
            if (check(TT::BRACE_LEFT))
            {
                return parseStructPattern();
            }

            // List pattern: [...]
            // (AR) نمط قائمة: [...]
            if (check(TT::BRACKET_LEFT))
            {
                return parseListPattern();
            }

            // Parse primary pattern (literal or variable)
            // (AR) تحليل نمط أساسي (قيمة حرفية أو متغير)
            auto primary = parsePrimaryPattern();

            if (!primary)
            {
                return nullptr;
            }

            // Check for binding pattern: name @ pattern
            // (AR) التحقق من نمط الربط: اسم @ نمط
            if (check(TT::AT_SIGN))
            {
                // (AR) primary يجب أن يكون VariablePattern
                auto *varPat = dynamic_cast<AST::VariablePattern *>(primary.get());
                if (varPat)
                {
                    std::string bindName = varPat->name;
                    advance(); // consume @
                    auto innerPattern = parsePattern();
                    if (innerPattern)
                    {
                        return std::make_unique<AST::BindingPattern>(
                            bindName, std::move(innerPattern));
                    }
                }
            }

            // Check for OR pattern: a | b | c
            // (AR) التحقق من نمط البدائل: a | b | c
            // (AR) القاعدة gr.pattern.or في language-truth تستخدم '|' المفردة (OP_BITWISE_OR)؛
            //      نقبل أيضًا '||' (OP_OR) تساهلًا. إصلاح ISSUE-033 (انجراف SoT↔مُحلِّل):
            //      كان الفحص يقتصر على OP_OR ('||') فيرفض الصياغة المعيارية '|'.
            if (check(TT::OP_OR) || check(TT::OP_BITWISE_OR))
            {
                std::vector<std::unique_ptr<AST::Pattern>> alternatives;
                alternatives.push_back(std::move(primary));

                while (match(TT::OP_OR) || match(TT::OP_BITWISE_OR))
                {
                    auto alt = parsePrimaryPattern();
                    if (alt)
                    {
                        alternatives.push_back(std::move(alt));
                    }
                    else
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "(نمطًا) بعد '||'"}, {"ctx_en", "(pattern) after '||'"}});
                        break;
                    }
                }

                return std::make_unique<AST::OrPattern>(std::move(alternatives));
            }

            return primary;
        }

        /**
         * @brief (AR) يحلل نمط أساسي (literal, variable, wildcard, range)
         *        (EN) Parses primary pattern (literal, variable, wildcard, range)
         */
        std::unique_ptr<AST::Pattern> ParserCore::parsePrimaryPattern()
        {
            // (AR) دالة مساعدة: تحليل رقم مع دعم الإشارة السالبة
            //      تستهلك رمز الطرح الاختياري ثم الرقم وتُعيد القيمة
            // (EN) Helper lambda: parse a number with optional leading minus
            //      Consumes optional OP_MINUS then the number token, returns value
            auto parseSignedNumber = [&]() -> std::pair<bool, double>
            {
                bool negative = false;
                if (check(TT::OP_MINUS))
                {
                    negative = true;
                    advance(); // (AR) استهلاك '-' / (EN) consume '-'
                }
                if (!check(TT::NUMBER_INTEGER) && !check(TT::NUMBER_DOUBLE))
                {
                    return {false, 0.0}; // (AR) فشل / (EN) failure
                }
                double v = std::stod(current_.getValue());
                advance();
                return {true, negative ? -v : v};
            };

            // Number literal (INTEGER or DOUBLE, optionally negative) — may be start of range
            // (AR) قيمة رقمية حرفية (موجبة أو سالبة) — قد تكون بداية نطاق
            bool startsWithMinus = check(TT::OP_MINUS);
            bool startsWithNumber = check(TT::NUMBER_INTEGER) || check(TT::NUMBER_DOUBLE);

            if (startsWithMinus || startsWithNumber)
            {
                // (AR) محاولة تحليل الرقم (مع إشارة سالبة اختيارية)
                // (EN) Attempt to parse the number (with optional negative sign)
                auto [ok, value] = parseSignedNumber();
                if (!ok)
                {
                    // (AR) لم يكن رقماً بعد الناقص — خطأ نحوي
                    // (EN) Not a number after minus — syntax error
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "(رقمًا) بعد '-' في نمط '" + kw(TT::KEYWORD_MATCH) + "'"}, {"ctx_en", "(number) after '-' in a '" + kw(TT::KEYWORD_MATCH) + "' pattern"}});
                    return nullptr;
                }

                // Check for range pattern: 1..10 or 1..=10 (or -10..0 etc.)
                // (AR) التحقق من نمط نطاق: 1..10 أو 1..=10 أو -10..0
                if (check(TT::DOT_DOT))
                {
                    advance(); // consume ..
                    bool inclusive = false;

                    // Check for ..= (inclusive)
                    if (check(TT::OP_ASSIGN))
                    {
                        advance();
                        inclusive = true;
                    }

                    // (AR) تحليل قيمة النهاية مع دعم الإشارة السالبة
                    // (EN) Parse end value with optional negative sign support
                    auto [endOk, endValue] = parseSignedNumber();
                    if (!endOk)
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "(رقمًا) بعد '..' في نمط النطاق"}, {"ctx_en", "(number) after '..' in a range pattern"}});
                        return nullptr;
                    }

                    return std::make_unique<AST::RangePattern>(
                        Data::Value(value), Data::Value(endValue), inclusive);
                }

                return std::make_unique<AST::LiteralPattern>(Data::Value(value));
            }

            // String literal
            // (AR) قيمة نصية حرفية
            if (check(TT::STRING_LITERAL))
            {
                Token token = current_;
                advance();
                return std::make_unique<AST::LiteralPattern>(Data::Value(token.getValue()));
            }

            // Boolean literal: true/false
            // (AR) قيمة منطقية حرفية
            if (check(TT::LITERAL_TRUE))
            {
                advance();
                return std::make_unique<AST::LiteralPattern>(Data::Value(true));
            }

            if (check(TT::LITERAL_FALSE))
            {
                advance();
                return std::make_unique<AST::LiteralPattern>(Data::Value(false));
            }

            // null
            // (AR) قيمة null — يجب أن تُبنى كـNull (عدم) لا Void الافتراضي. بعد S-TS-P1
            //      صار «عدم» متمايزًا عن «فراغ»، فلو بقي النمط Void لما طابق المُسنَد
            //      «لاشيء» (Null) ⇒ انكسرت مطابقة `عندما لاشيء:`.
            // (EN) null pattern must be built as Null, not default Void. After S-TS-P1
            //      «عدم» is distinct from «فراغ»; a Void pattern no longer matches a Null
            //      scrutinee, breaking `when null:` matching.
            if (check(TT::LITERAL_NULL))
            {
                advance();
                return std::make_unique<AST::LiteralPattern>(Data::Value::makeNull());
            }

            // Variable pattern (identifier) — or qualified enum member: Enum.Value
            // (AR) نمط متغير (معرّف) — أو عضو تعداد مؤهل: تعداد.قيمة
            if (check(TT::IDENTIFIER))
            {
                Token token = current_;
                advance();

                // (AR) تحقق من وصول عضو: Enum.Value أو Enum.Value1.SubValue
                // (EN) Check for member access: Enum.Value or Enum.Value1.SubValue
                if (check(TT::DOT))
                {
                    // (AR) بناء اسم مؤهل: "تعداد.عضو"
                    // (EN) Build qualified name: "Enum.Variant"
                    std::string enumName = token.getValue();
                    advance(); // (AR) استهلاك '.' / (EN) consume '.'

                    std::string variantName;
                    if (check(TT::IDENTIFIER) || isTokenUsableAsName(current_.getType()))
                    {
                        variantName = current_.getValue();
                        advance();
                    }
                    else
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "عضو التعداد"}, {"what_en", "enum variant"}, {"ctx_ar", "بعد '.' في نمط التعداد"}, {"ctx_en", "after '.' in an enum pattern"}});
                        return nullptr;
                    }

                    // (AR) تحقق من وجود أقواس — نمط تعداد جبري (ADT) مع بيانات
                    //      مثال: شكل.دائرة(ق) أو شكل.مستطيل(ع، ا)
                    // (EN) Check for parentheses — ADT enum variant pattern with data
                    //      Example: Shape.Circle(r) or Shape.Rectangle(w, h)
                    if (check(TT::PAREN_LEFT))
                    {
                        advance(); // (AR) استهلاك '(' / (EN) consume '('

                        std::vector<std::unique_ptr<AST::Pattern>> fieldPatterns;

                        // (AR) تحليل أنماط الحقول مفصولة بفواصل
                        // (EN) Parse comma-separated field patterns
                        if (!check(TT::PAREN_RIGHT))
                        {
                            auto firstPattern = parsePattern();
                            if (firstPattern)
                            {
                                fieldPatterns.push_back(std::move(firstPattern));
                            }

                            while (checkComma())
                            {
                                advance(); // (AR) استهلاك الفاصلة / (EN) consume comma
                                auto nextPattern = parsePattern();
                                if (nextPattern)
                                {
                                    fieldPatterns.push_back(std::move(nextPattern));
                                }
                            }
                        }

                        if (!check(TT::PAREN_RIGHT))
                        {
                            errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ")"}, {"ctx_ar", "لإغلاق أنماط التعداد الجبري"}, {"ctx_en", "to close the ADT enum variant patterns"}});
                            return nullptr;
                        }
                        advance(); // (AR) استهلاك ')' / (EN) consume ')'

                        return std::make_unique<AST::EnumVariantPattern>(
                            enumName, variantName, std::move(fieldPatterns));
                    }

                    // (AR) نمط تعداد جبري بدون بيانات (Unit variant)
                    //      مثال: شكل.نقطة
                    //      نُنشئ EnumVariantPattern بدون أنماط حقول
                    // (EN) ADT enum variant without data (Unit variant)
                    //      Example: Shape.Point
                    //      Create EnumVariantPattern without field patterns
                    return std::make_unique<AST::EnumVariantPattern>(
                        enumName, variantName);
                }

                // (AR) نمط الباني غير المؤهَّل: اسم(نمط، ...) — عضو تعداد بحمولة.
                //      يُميَّز عن نمط المتغيّر بوجود '(' بعد الاسم مباشرةً (بلا نقطة).
                //      مثال المواصفة: عدد(ق) — جمع(ي، ن). هويّة التعداد تُحسم دلاليًّا (أ-م٢).
                // (EN) Unqualified constructor pattern: Name(pattern, ...) — tagged-enum variant.
                //      Distinguished from a variable pattern by '(' right after the name (no dot).
                if (check(TT::PAREN_LEFT))
                {
                    advance(); // (AR) استهلاك '(' / (EN) consume '('

                    std::vector<std::unique_ptr<AST::Pattern>> fieldPatterns;

                    // (AR) تحليل أنماط الحمولة مفصولة بفواصل / (EN) parse comma-separated payload patterns
                    if (!check(TT::PAREN_RIGHT))
                    {
                        auto firstPattern = parsePattern();
                        if (firstPattern)
                        {
                            fieldPatterns.push_back(std::move(firstPattern));
                        }

                        while (checkComma())
                        {
                            advance(); // (AR) استهلاك الفاصلة / (EN) consume comma
                            auto nextPattern = parsePattern();
                            if (nextPattern)
                            {
                                fieldPatterns.push_back(std::move(nextPattern));
                            }
                        }
                    }

                    if (!check(TT::PAREN_RIGHT))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ")"}, {"ctx_ar", "لإغلاق أنماط باني التعداد بحمولة"}, {"ctx_en", "to close the tagged-enum constructor patterns"}});
                        return nullptr;
                    }
                    advance(); // (AR) استهلاك ')' / (EN) consume ')'

                    return std::make_unique<AST::ConstructorPattern>(
                        token.getValue(), std::move(fieldPatterns));
                }

                return std::make_unique<AST::VariablePattern>(token.getValue());
            }

            errorCatalog(Errors::ErrorCode::SYN_EXPECTED_EXPRESSION, {{"ctx_ar", "(نمطًا: رقم، نص، متغير، أو '_')"}, {"ctx_en", "(pattern: number, string, variable, or '_')"}});
            return nullptr;
        }

        /**
         * @brief (AR) يحلل نمط قائمة [...]
         *        (EN) Parses list pattern [...]
         */
        std::unique_ptr<AST::Pattern> ParserCore::parseListPattern()
        {
            if (!match(TT::BRACKET_LEFT))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "["}, {"ctx_ar", "لبداية نمط القائمة"}, {"ctx_en", "to start a list pattern"}});
                return nullptr;
            }

            std::vector<std::unique_ptr<AST::Pattern>> elements;
            bool has_rest = false;
            std::string rest_name;

            // Empty list: []
            // (AR) قائمة فارغة: []
            if (check(TT::BRACKET_RIGHT))
            {
                advance();
                return std::make_unique<AST::ListPattern>(std::move(elements), false, "");
            }

            // Parse elements
            // (AR) تحليل العناصر
            do
            {
                // Rest pattern: *rest (using OP_MULTIPLY)
                // (AR) نمط الباقي: *rest
                if (check(TT::OP_MULTIPLY))
                {
                    advance();

                    if (!check(TT::IDENTIFIER))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "متغير"}, {"what_en", "variable"}, {"ctx_ar", "بعد '*' في نمط القائمة"}, {"ctx_en", "after '*' in a list pattern"}});
                        return nullptr;
                    }

                    Token idToken = current_;
                    advance();
                    rest_name = idToken.getValue();
                    has_rest = true;

                    // Can't have elements after *rest
                    // (AR) لا يمكن أن يكون هناك عناصر بعد *rest
                    break;
                }

                // Parse regular pattern
                // (AR) تحليل نمط عادي
                auto pattern = parsePattern();
                if (pattern)
                {
                    elements.push_back(std::move(pattern));
                }
                else
                {
                    // Error in pattern
                    // (AR) خطأ في النمط
                    return nullptr;
                }

            } while (matchComma());

            if (!match(TT::BRACKET_RIGHT))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "]"}, {"ctx_ar", "في نهاية نمط القائمة"}, {"ctx_en", "at the end of a list pattern"}});
                return nullptr;
            }

            return std::make_unique<AST::ListPattern>(
                std::move(elements),
                has_rest,
                rest_name);
        }

        /**
         * @brief (AR) يحلل نمط بنية/صنف { حقل: نمط، ... }
         *        (EN) Parses struct/class pattern { field: pattern, ... }
         */
        std::unique_ptr<AST::Pattern> ParserCore::parseStructPattern()
        {
            if (!match(TT::BRACE_LEFT))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "{"}, {"ctx_ar", "لبداية نمط البنية"}, {"ctx_en", "to start a struct pattern"}});
                return nullptr;
            }

            std::vector<std::pair<std::string, std::unique_ptr<AST::Pattern>>> fields;

            // Empty struct: {}
            // (AR) بنية فارغة: {}
            if (check(TT::BRACE_RIGHT))
            {
                advance();
                return std::make_unique<AST::StructPattern>("", std::move(fields));
            }

            // Parse fields
            // (AR) تحليل الحقول
            do
            {
                // Field name
                // (AR) اسم الحقل
                if (!check(TT::IDENTIFIER))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "الحقل"}, {"what_en", "field"}, {"ctx_ar", "في نمط البنية"}, {"ctx_en", "in a struct pattern"}});
                    return nullptr;
                }

                std::string fieldName = current_.getValue();
                advance();

                // Colon
                // (AR) نقطتان
                if (!match(TT::COLON))
                {
                    // (AR) إذا لم يوجد :، نفترض أن اسم الحقل هو أيضاً المتغير
                    // (EN) If no :, assume field name is also the variable
                    fields.push_back({fieldName,
                                      std::make_unique<AST::VariablePattern>(fieldName)});
                }
                else
                {
                    // Parse field pattern
                    // (AR) تحليل نمط الحقل
                    auto fieldPattern = parsePattern();
                    if (!fieldPattern)
                    {
                        return nullptr;
                    }
                    fields.push_back({fieldName, std::move(fieldPattern)});
                }

            } while (matchComma());

            if (!match(TT::BRACE_RIGHT))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "}"}, {"ctx_ar", "في نهاية نمط البنية"}, {"ctx_en", "at the end of a struct pattern"}});
                return nullptr;
            }

            return std::make_unique<AST::StructPattern>("", std::move(fields));
        }

        // ======================================================================
        // (AR) تحليل القوالب (Templates - Phase 7B)
        // (EN) Template Parsing (Phase 7B)
        // ======================================================================

        /**
         * @brief (AR) يحلل معاملات أنواع القالب.
         *        (EN) Parses template type parameters.
         *
         * @details
         * Syntax / النحو:
         *   <نوع ت> | <typename T>
         *   <نوع ت، نوع م> | <typename T, typename U>
         *   <نوع ت: قابل_للمقارنة> | <typename T: Comparable>
         */
        std::vector<AST::TypeParameter> ParserCore::parseTemplateParameters()
        {
            std::vector<AST::TypeParameter> params;

            // (AR) توقع '<' / (EN) Expect '<'
            if (!match(TT::OP_LESS))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "<"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_TEMPLATE) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_TEMPLATE) + "'"}});
                return params;
            }

            // (AR) تحليل معاملات الأنواع
            // (EN) Parse type parameters
            do
            {
                // ==================================================================
                // (AR) [Phase 4 — Monomorphization] دعم const-generic:
                //      صيغة: ثابت <نوع> <اسم>  مثل: ثابت رقم N
                //      تُمثَّل كـ TypeParameter::makeConst(name, typeName)
                // (EN) [Phase 4] const-generic syntax: const <type> <name>
                //      e.g. const int N → TypeParameter::makeConst("N", "رقم")
                // ==================================================================
                bool isConstParam = false;
                if (check(TT::KEYWORD_CONST))
                {
                    advance();
                    isConstParam = true;
                }

                if (isConstParam)
                {
                    // (AR) توقع اسم نوع الثابت — يأتي كـ IDENTIFIER (رقم/عشري/نص/منطقي)
                    if (!check(TT::IDENTIFIER))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "النوع"}, {"what_en", "type"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_CONST) + "' في معامل القالب"}, {"ctx_en", "after '" + kw(TT::KEYWORD_CONST) + "' in a template parameter"}});
                        break;
                    }
                    std::string constTypeName = current_.getValue();
                    advance();

                    if (!check(TT::IDENTIFIER))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "المعامل الثابت"}, {"what_en", "const parameter"}, {"ctx_ar", "في معاملات القالب"}, {"ctx_en", "in the template parameters"}});
                        break;
                    }
                    std::string paramName = current_.getValue();
                    advance();

                    // (AR) قيمة افتراضية اختيارية: ثابت رقم N = 4
                    AST::ExprPtr defaultExpr;
                    if (match(TT::OP_ASSIGN))
                    {
                        defaultExpr = parseExpression();
                    }

                    params.push_back(AST::TypeParameter::makeConst(paramName, constTypeName, std::move(defaultExpr)));
                }
                else
                {
                    // (AR) توقع 'نوع' أو 'typename' — كلمة سياقية: تحقق مزدوج
                    // (EN) Expect 'typename' keyword — contextual: double check
                    if (!match(TT::KEYWORD_TYPENAME) &&
                        !matchContextual(TT::KEYWORD_TYPENAME))
                    { // نوع
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_TYPENAME)}, {"ctx_ar", "في معامل القالب"}, {"ctx_en", "in a template parameter"}});
                        break;
                    }

                    // (AR) توقع اسم المعامل (مثل T أو ت)
                    // (EN) Expect parameter name (e.g., T)
                    if (!check(TT::IDENTIFIER))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "معامل النوع"}, {"what_en", "type parameter"}, {"ctx_ar", "في القالب"}, {"ctx_en", "in a template"}});
                        break;
                    }

                    std::string paramName = current_.getValue();
                    advance();

                    // (AR) تحقق من وجود قيد (constraint)
                    // (EN) Check for constraint — supports multiple: ت: قيد1 + قيد2
                    std::vector<std::string> constraintsList;
                    if (match(TT::COLON))
                    {
                        // (AR) تحليل قيود مفصولة بـ +
                        // (EN) Parse constraints separated by +
                        do
                        {
                            if (!check(TT::IDENTIFIER))
                            {
                                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "القيد"}, {"what_en", "constraint"}, {"ctx_ar", "بعد ':'"}, {"ctx_en", "after ':'"}});
                                break;
                            }
                            constraintsList.push_back(current_.getValue());
                            advance();
                        } while (match(TT::OP_PLUS));
                    }

                    params.emplace_back(paramName, constraintsList);
                }

            } while (matchComma());

            // (AR) توقع '>' / (EN) Expect '>'
            if (!match(TT::OP_GREATER))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ">"}, {"ctx_ar", "لإنهاء معاملات القالب"}, {"ctx_en", "to close the template parameters"}});
            }

            return params;
        }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) تحليل جملة حيث (Where Clause)
        // (EN) Where Clause Parsing
        // ═══════════════════════════════════════════════════════════════════════

        /**
         * @brief (AR) يحلل جملة حيث لقيود القوالب المتقدمة.
         *        (EN) Parses where clause for advanced template constraints.
         *
         * @details
         * Syntax / النحو:
         *   حيث ت: قابل_للمقارنة + قابل_للتجزئة، م: قابل_للطباعة
         *   حيث ت.عنصر: قابل_للمقارنة   (Associated type constraint)
         *   where T: Comparable + Hashable, U: Printable
         */
        AST::WhereClause ParserCore::parseWhereClause()
        {
            AST::WhereClause clause;

            // (AR) تحليل كل عنصر قيد / (EN) Parse each constraint item
            do
            {
                AST::WhereConstraintItem item;

                if (!check(TT::IDENTIFIER))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "معامل النوع"}, {"what_en", "type parameter"}, {"ctx_ar", "في جملة '" + kw(TT::KEYWORD_WHERE) + "'"}, {"ctx_en", "in a '" + kw(TT::KEYWORD_WHERE) + "' clause"}});
                    break;
                }

                item.typeName = current_.getValue();
                advance();

                // (AR) تحقق من مسار نوع مرتبط: ت.عنصر
                // (EN) Check for associated type path: T.Element
                if (match(TT::DOT))
                {
                    if (!check(TT::IDENTIFIER))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "النوع المرتبط"}, {"what_en", "associated type"}, {"ctx_ar", "بعد '.'"}, {"ctx_en", "after '.'"}});
                        break;
                    }
                    item.associatedPath = current_.getValue();
                    advance();
                }

                // (AR) توقع ':' / (EN) Expect ':'
                if (!match(TT::COLON))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ":"}, {"ctx_ar", "بعد اسم المعامل في جملة '" + kw(TT::KEYWORD_WHERE) + "'"}, {"ctx_en", "after the parameter name in a '" + kw(TT::KEYWORD_WHERE) + "' clause"}});
                    break;
                }

                // (AR) تحليل قيود مفصولة بـ +
                // (EN) Parse constraints separated by +
                do
                {
                    if (!check(TT::IDENTIFIER))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "القيد"}, {"what_en", "constraint"}, {"ctx_ar", "في جملة '" + kw(TT::KEYWORD_WHERE) + "'"}, {"ctx_en", "in a '" + kw(TT::KEYWORD_WHERE) + "' clause"}});
                        break;
                    }
                    item.constraints.push_back(current_.getValue());
                    advance();
                } while (match(TT::OP_PLUS));

                clause.items.push_back(std::move(item));

            } while (matchComma());

            return clause;
        }

        /**
         * @brief (AR) يحلل تصريح قالب (دالة أو صنف).
         *        (EN) Parses template declaration (function or class).
         *
         * @details
         * Syntax / النحو:
         *   قالب<نوع ت> دالة اسم(...) نوع_الإرجاع { ... }
         *   قالب<نوع ت> صنف اسم { ... نهاية }
         */
        StmtPtr ParserCore::parseTemplateDecl()
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

            auto startPos = previous_.getPosition();

            // (AR) تحليل معاملات الأنواع
            // (EN) Parse type parameters
            auto typeParams = parseTemplateParameters();

            if (typeParams.empty())
            {
                errorCatalog(Errors::ErrorCode::SYN_EMPTY_CONSTRUCT, {{"construct_ar", "القالب"}, {"construct_en", "template"}, {"required_ar", "معامل نوع واحد"}, {"required_en", "one type parameter"}});
                return nullptr;
            }

            // (AR) التحقق من نوع التصريح (دالة أو صنف)
            // (EN) Check declaration type (function or class)
            if (match(TT::KEYWORD_FUNCTION))
            {
                // (AR) تحليل دالة قالب
                // (EN) Parse template function

                // (AR) توقع اسم الدالة
                // (EN) Expect function name
                if (!check(TT::IDENTIFIER))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "الدالة"}, {"what_en", "function"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_FUNCTION) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_FUNCTION) + "'"}});
                    return nullptr;
                }

                std::string funcName = current_.getValue();
                advance();

                // (AR) [Phase 4] تسجيل اسم القالب ليُميَّز `اسم<...>` لاحقاً
                //      عن عمليات المقارنة `اسم < قيمة` — انظر parser_expressions.cpp.
                // (EN) [Phase 4] Register template name so later `name<...>` can be
                //      disambiguated from comparison `name < value` — see parser_expressions.cpp.
                knownTemplateNames_.insert(funcName);

                // (AR) تحليل المعاملات
                // (EN) Parse parameters
                if (!match(TT::PAREN_LEFT))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "("}, {"ctx_ar", "بعد اسم الدالة"}, {"ctx_en", "after the function name"}});
                    return nullptr;
                }

                std::vector<AST::Parameter> params;
                if (!check(TT::PAREN_RIGHT))
                {
                    do
                    {
                        // (AR) تحليل معامل واحد: اسم: نوع
                        // (EN) Parse single parameter: name: type
                        if (!check(TT::IDENTIFIER) && !isTypeToken(current_.getType()))
                        {
                            errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "المعامل أو نوعه"}, {"what_en", "parameter (or its type)"}, {"ctx_ar", "في قائمة المعاملات"}, {"ctx_en", "in the parameter list"}});
                            break;
                        }

                        std::string paramName;
                        Types::SadTypeKind paramType = Types::SadTypeKind::Void;
                        std::string templateTypeName; // (AR) لحفظ اسم نوع القالب / (EN) To store template type name

                        // (AR) معامل يبدأ بالنوع أو الاسم
                        // (EN) Parameter starts with type or name
                        if (isTypeToken(current_.getType()))
                        {
                            // (AR) نوع مدمج مثل: رقم س
                            // (EN) Built-in type like: int x
                            paramType = mapTokenTypeToKind(current_.getType());
                            advance();
                            if (check(TT::IDENTIFIER))
                            {
                                paramName = current_.getValue();
                                advance();
                            }
                        }
                        else if (check(TT::IDENTIFIER))
                        {
                            // (AR) قد يكون: ت س (نوع قالب + اسم) أو س: ت (اسم + نوع)
                            // (EN) Could be: T x (template type + name) or x: T (name + type)
                            std::string firstIdent = current_.getValue();
                            advance();

                            if (check(TT::IDENTIFIER))
                            {
                                // (AR) صيغة: ت س (نوع معرف + اسم)
                                // (EN) Format: T x (identifier type + name)
                                templateTypeName = firstIdent;
                                paramName = current_.getValue();
                                paramType = Types::SadTypeKind::Class; // Template type as OBJECT
                                advance();
                            }
                            else if (match(TT::COLON))
                            {
                                // (AR) صيغة: س: ت (اسم + نوع)
                                // (EN) Format: x: T (name + type)
                                paramName = firstIdent;
                                if (isTypeToken(current_.getType()))
                                {
                                    paramType = mapTokenTypeToKind(current_.getType());
                                    advance();
                                }
                                else if (check(TT::IDENTIFIER))
                                {
                                    // (AR) قد يكون معامل نوع من القالب
                                    // (EN) Might be a template type parameter
                                    templateTypeName = current_.getValue();
                                    paramType = Types::SadTypeKind::Class;
                                    advance();
                                }
                            }
                            else
                            {
                                // (AR) فقط اسم بدون نوع
                                // (EN) Just name without type
                                paramName = firstIdent;
                            }
                        }

                        params.emplace_back(paramName, paramType, nullptr, templateTypeName);

                    } while (matchComma());
                }

                if (!match(TT::PAREN_RIGHT))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ")"}, {"ctx_ar", "بعد معاملات الدالة"}, {"ctx_en", "after the function parameters"}});
                    return nullptr;
                }

                // (AR) تحليل نوع الإرجاع
                // (EN) Parse return type
                Types::SadTypeKind returnType = Types::SadTypeKind::Void;
                std::string returnTypeName;

                // (AR) البحث عن نوع الإرجاع بعد المعاملات مع كلمة "ترجع" الاختيارية
                // (EN) Look for return type after parameters with optional "ترجع" keyword

                // (AR) [إصلاح Phase 8] KEYWORD_RETURNS كلمة سياقية — المحلل المعجمي يُنتجها دائماً كـ IDENTIFIER
                //      لذا يجب الفحص عن كليهما: التوكن السياقي والنص العربي "ترجع"
                // (EN) [Fix Phase 8] KEYWORD_RETURNS is contextual — lexer always produces IDENTIFIER
                //      Must check both token type AND Arabic string "ترجع" (= \xD8\xAA\xD8\xB1\xD8\xAC\xD8\xB9)
                bool consumedReturnsKeyword = match(TT::KEYWORD_RETURNS) || match(TT::ARROW);
                if (!consumedReturnsKeyword &&
                    check(TT::IDENTIFIER) &&
                    checkContextual(TT::KEYWORD_RETURNS)) // ترجع
                {
                    advance();
                    consumedReturnsKeyword = true;
                }

                if (consumedReturnsKeyword)
                {
                    if (isTypeToken(current_.getType()))
                    {
                        returnType = mapTokenTypeToKind(current_.getType());
                        advance();
                    }
                    else if (check(TT::IDENTIFIER))
                    {
                        const std::string &rn = current_.getValue();
                        // (AR) [Phase 8] أسماء الأنواع البدائية كنوع إرجاع في قوالب الدوال
                        //      مثال: قالب<نوع T> دالة اسم(T أ) ترجع رقم
                        // (EN) [Phase 8] Primitive type names as return type in template functions
                        //      Example: template<type T> function name(T a) returns int
                        //
                        // (AR) 🔑 كان هنا جدولٌ سادسٌ مكتوبٌ بمحارفَ سُداسيّةٍ انجرف عن
                        //      إخوته: يقبل «مضاعف» صامتًا (`ترجع مضاعف` ⇒ 1.5 بخروجٍ ٠
                        //      بلا SYN014)، وينقصه «عدم/مصفوفة/خريطة/أي/طبيعي64/بايت».
                        //      والسُداسيّةُ تجعله غيرَ مرئيٍّ لحارسٍ يبحث عن ألفاظٍ عربيّة.
                        //      الآن يفوّض إلى النقطةِ الواحدة في `parser_helpers.cpp`.
                        // (EN) A sixth, hex-escaped copy lived here and had drifted;
                        //      it now delegates to the single resolver.
                        //      و`primitivesOnly` يمنع التوحيدَ من **توسيع** المقبول:
                        //      «عدم/مصفوفة/خريطة/أي/طبيعي64/بايت» تبقى أسماءَ أصناف
                        //      هنا كما كانت — قِيس أنّ توسيعَها كسر `صنف بايت` بانهيارٍ
                        //      صامت و`ترجع أي` بخطأٍ داخليّ.
                        Types::SadTypeKind resolvedReturn =
                            resolveTypeWordName(rn, /*primitivesOnly=*/true);
                        if (resolvedReturn != Types::SadTypeKind::Unknown)
                        {
                            returnType = resolvedReturn;
                        }
                        else
                        {
                            returnTypeName = rn;
                            returnType = Types::SadTypeKind::Class;
                        }
                        advance();
                    }
                }
                else if (isTypeToken(current_.getType()))
                {
                    // Built-in type without "ترجع"
                    returnType = mapTokenTypeToKind(current_.getType());
                    advance();
                }
                else if (check(TT::IDENTIFIER))
                {
                    // Template type parameter as return type without "ترجع"
                    // Check if it looks like a type parameter from template
                    std::string possibleType = current_.getValue();

                    // Check if this identifier is one of the template type parameters
                    bool isTemplateParam = false;
                    for (const auto &param : typeParams)
                    {
                        if (param.name == possibleType)
                        {
                            isTemplateParam = true;
                            break;
                        }
                    }

                    // Accept it as return type if it's a template parameter
                    if (isTemplateParam)
                    {
                        returnTypeName = possibleType;
                        returnType = Types::SadTypeKind::Class;
                        advance();
                    }
                }

                // (AR) تحليل جملة حيث (اختيارية)
                // (EN) Parse optional where clause
                AST::WhereClause whereClause;
                if ((check(TT::KEYWORD_WHERE)) || checkContextual(TT::KEYWORD_WHERE))
                {              // حيث
                    advance(); // تخطي 'حيث'/'where'
                    whereClause = parseWhereClause();
                }

                // (AR) تحليل جسم الدالة
                // (EN) Parse function body
                StmtPtr body = nullptr;
                if (check(TT::BRACE_LEFT))
                {
                    body = parseBlockStmt();
                }
                else if (check(TT::KEYWORD_END))
                {
                    // (AR) دالة فارغة / (EN) Empty function
                    advance();
                }
                else
                {
                    // (AR) حاول قراءة الجسم
                    body = parseBlockStmt();
                }

                auto templateFunc = std::make_unique<AST::TemplateFunctionDecl>(
                    std::move(typeParams),
                    funcName,
                    std::move(params),
                    returnType,
                    std::move(body),
                    false, // isExported
                    startPos);
                templateFunc->whereClause = std::move(whereClause);
                templateFunc->docComment = std::move(docComment);
                return templateFunc;
            }
            else if (match(TT::KEYWORD_CLASS))
            {
                // (AR) تحليل صنف قالب
                // (EN) Parse template class

                // (AR) توقع اسم الصنف
                // (EN) Expect class name
                if (!check(TT::IDENTIFIER))
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "الصنف"}, {"what_en", "class"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_CLASS) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_CLASS) + "'"}});
                    return nullptr;
                }

                std::string className = current_.getValue();
                advance();

                // (AR) [Phase 4] تسجيل اسم الصنف القالب للتمييز لاحقاً.
                // (EN) [Phase 4] Register template class name for later disambiguation.
                knownTemplateNames_.insert(className);

                // (AR) التحقق من الوراثة
                // (EN) Check for inheritance
                std::vector<std::string> baseClasses;
                if (match(TT::KEYWORD_INHERITS) || match(TT::KEYWORD_EXTENDS))
                {
                    do
                    {
                        if (!check(TT::IDENTIFIER))
                        {
                            errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "الصنف الأب"}, {"what_en", "base class"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_INHERITS) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_INHERITS) + "'"}});
                            break;
                        }
                        baseClasses.push_back(current_.getValue());
                        advance();
                    } while (matchComma());
                }

                // (AR) تحليل جملة حيث للصنف القالب (اختيارية)
                // (EN) Parse optional where clause for template class
                AST::WhereClause classWhereClause;
                if ((check(TT::KEYWORD_WHERE)) || checkContextual(TT::KEYWORD_WHERE))
                { // حيث
                    advance();
                    classWhereClause = parseWhereClause();
                }

                // (AR) تحليل أعضاء الصنف
                // (EN) Parse class members
                AST::StmtList members;

                // ══════════════════════════════════════════════════════════════════════════
                // (AR) دالة مساعدة لتحليل أعضاء الصنف القالبي
                // ══════════════════════════════════════════════════════════════════════════
                // تتعامل هذه الدالة مع جميع أنواع أعضاء الصنف القالبي:
                //   1. خاصية (KEYWORD_PROPERTY) — حقل بسيط أو خاصية كاملة مع getter/setter
                //   2. دالة/طريقة (KEYWORD_FUNCTION) — طريقة عادية
                //   3. بناء (KEYWORD_CONSTRUCTOR) — دالة البناء (باني/منشئ/بناء)
                //   4. هدم (KEYWORD_DESTRUCTOR) — دالة الهدم
                //   5. عامل (KEYWORD_OPERATOR) — تحميل العوامل الزائد
                //   6. متغير (KEYWORD_VAR) — حقل بنوع ديناميكي
                //   7. معامل نوع قالب كاسم نوع (مثل: ت اسم_الحقل)
                //   8. نوع مدمج عادي (نص، رقم، منطقي، إلخ) — حقل بنوع ثابت
                // ══════════════════════════════════════════════════════════════════════════
                auto parseTemplateClassMember = [this, &className, &typeParams]() -> StmtPtr
                {
                    // ═══════════════════════════════════════════════════════════════════
                    // (AR) قاعدة الصفة بعد الموصوف — المعدلات تأتي بعد الكلمة المفتاحية
                    //      الصيغة الجديدة: دالة عام ساكن طريقة()
                    //      بدلاً من القديمة: عام ساكن دالة طريقة()
                    // (EN) Arabic adjective-after-noun rule for template class members
                    // ═══════════════════════════════════════════════════════════════════
                    bool isStatic = false;
                    bool isVirtual = false;
                    bool isAbstract = false;
                    AccessModifier access = AccessModifier::PUBLIC;

                    // (AR) رسالة خطأ للصيغة القديمة: معدل قبل الكلمة المفتاحية
                    if (check(TT::KEYWORD_PUBLIC) || check(TT::KEYWORD_PRIVATE) ||
                        check(TT::KEYWORD_PROTECTED) || check(TT::KEYWORD_STATIC) || check(TT::KEYWORD_ABSTRACT))
                    {
                        std::string modName = current_.getValue();
                        auto nextTT = peekNext().getType();
                        if (nextTT == TT::KEYWORD_FUNCTION || nextTT == TT::KEYWORD_CONSTRUCTOR ||
                            nextTT == TT::KEYWORD_DESTRUCTOR || nextTT == TT::KEYWORD_PROPERTY ||
                            nextTT == TT::KEYWORD_OPERATOR || nextTT == TT::KEYWORD_VAR ||
                            nextTT == TT::KEYWORD_PUBLIC || nextTT == TT::KEYWORD_PRIVATE ||
                            nextTT == TT::KEYWORD_PROTECTED || nextTT == TT::KEYWORD_STATIC ||
                            nextTT == TT::KEYWORD_ABSTRACT ||
                            (nextTT == TT::IDENTIFIER && (peekNext().getValue() == "خاصية" ||
                                                          peekNext().getValue() == "عامل" || peekNext().getValue() == "هدم")))
                        {
                            errorCatalog(Errors::ErrorCode::SYN_ADJECTIVE_ORDER, {{"wrong", modName + " " + kw(TT::KEYWORD_FUNCTION) + " ..."}, {"right", kw(TT::KEYWORD_FUNCTION) + " " + modName + " ..."}});
                        }
                        // (AR) ابتلاع المعدلات القديمة للاسترداد
                        access = parseModifiers(isStatic, isVirtual, isAbstract);
                    }

                    // ─────────────────────────────────────────────────────────────
                    // (AR) [1] التحقق من كلمة 'خاصية' (KEYWORD_PROPERTY)
                    // ─────────────────────────────────────────────────────────────
                    if (check(TT::KEYWORD_PROPERTY) || checkContextual(TT::KEYWORD_PROPERTY))
                    {
                        advance(); // (AR) استهلاك 'خاصية'
                        // (AR) قراءة المعدلات بعد 'خاصية' (الصفة بعد الموصوف)
                        access = parseModifiers(isStatic, isVirtual, isAbstract);

                        // (AR) الحالة (أ): التالي هو معامل نوع قالب (مثل: خاصية ت محتوى)
                        if (check(TT::IDENTIFIER))
                        {
                            std::string nextIdent = current_.getValue();
                            bool isTemplateTypeParam = false;
                            for (const auto &tp : typeParams)
                            {
                                if (tp.name == nextIdent)
                                {
                                    isTemplateTypeParam = true;
                                    break;
                                }
                            }
                            if (isTemplateTypeParam)
                            {
                                // (AR) النوع هو معامل قالب — نعامله كـ OBJECT (نوع عام)
                                advance(); // (AR) استهلاك اسم معامل النوع
                                Token nameToken = consume(TT::IDENTIFIER, "");
                                ExprPtr initializer = nullptr;
                                if (match(TT::OP_ASSIGN))
                                {
                                    initializer = parseExpression();
                                }
                                if (matchSemicolon())
                                {
                                }
                                return std::make_unique<FieldDecl>(nameToken.getValue(), Types::SadTypeKind::Class,
                                                                   std::move(initializer), access, isStatic, nameToken.getPosition());
                            }
                        }

                        // (AR) الحالة (ب)/(د): التالي هو رمز نوع مدمج (نص، رقم، منطقي...)
                        if (isTypeToken(current_.getType()))
                        {
                            // (AR) تحقق مما إذا كان هناك 'احصل' بعد النوع والاسم
                            //      لتحديد إذا كانت خاصية كاملة أم حقل بسيط
                            // (EN) Check if there's a 'get' keyword after type+name to determine
                            //      full property vs simple field
                            return parsePropertyDeclaration(access, isStatic);
                        }

                        // (AR) الحالة (ج): خاصية متبوعة مباشرة باسم فقط (بدون نوع)
                        //      نعاملها كحقل بنوع OBJECT (ديناميكي)
                        if (check(TT::IDENTIFIER))
                        {
                            Token nameToken = current_;
                            advance(); // (AR) استهلاك اسم الحقل
                            ExprPtr initializer = nullptr;
                            if (match(TT::OP_ASSIGN))
                            {
                                initializer = parseExpression();
                            }
                            if (matchSemicolon())
                            {
                            }
                            return std::make_unique<FieldDecl>(nameToken.getValue(), Types::SadTypeKind::Class,
                                                               std::move(initializer), access, isStatic, nameToken.getPosition());
                        }

                        // (AR) لا شيء معروف بعد 'خاصية'
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "نوع أو اسم"}, {"what_en", "type or name"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_PROPERTY) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_PROPERTY) + "'"}});
                        advance();
                        return nullptr;
                    }

                    // ─────────────────────────────────────────────────────────────
                    // (AR) [2] التحقق من طريقة (KEYWORD_FUNCTION = دالة/طريقة)
                    // ─────────────────────────────────────────────────────────────
                    if (check(TT::KEYWORD_FUNCTION))
                    {
                        advance();
                        // (AR) قراءة المعدلات بعد 'دالة' (الصفة بعد الموصوف)
                        access = parseModifiers(isStatic, isVirtual, isAbstract);
                        return parseMethodDeclaration(access, isStatic, isVirtual, isAbstract);
                    }

                    // ─────────────────────────────────────────────────────────────
                    // (AR) [3] التحقق من دالة البناء (باني/منشئ/بناء)
                    // (EN) [3] Check for constructor (باني/منشئ/بناء keywords)
                    // ─────────────────────────────────────────────────────────────
                    if (check(TT::KEYWORD_CONSTRUCTOR) ||
                        (check(TT::IDENTIFIER) && current_.getValue() == className &&
                         peekNext().getType() == TT::PAREN_LEFT))
                    {
                        if (check(TT::KEYWORD_CONSTRUCTOR))
                        {
                            advance(); // (AR) استهلاك كلمة الباني
                        }
                        else
                        {
                            advance(); // (AR) استهلاك اسم الصنف
                        }
                        // (AR) قراءة المعدلات بعد 'باني' (الصفة بعد الموصوف)
                        access = parseModifiers(isStatic, isVirtual, isAbstract);
                        return parseConstructorDeclaration(className, access);
                    }

                    // ─────────────────────────────────────────────────────────────
                    // (AR) [4] التحقق من دالة الهدم (KEYWORD_DESTRUCTOR = هدم)
                    // ─────────────────────────────────────────────────────────────
                    if (check(TT::KEYWORD_DESTRUCTOR) || checkContextual(TT::KEYWORD_DESTRUCTOR))
                    {
                        advance(); // (AR) استهلاك كلمة الهدم
                        // (AR) قراءة المعدلات بعد 'هدم' (الصفة بعد الموصوف)
                        access = parseModifiers(isStatic, isVirtual, isAbstract);
                        return parseDestructorDeclaration(className, access);
                    }

                    // ─────────────────────────────────────────────────────────────
                    // (AR) [5] التحقق من تحميل العوامل الزائد (KEYWORD_OPERATOR = عامل)
                    // ─────────────────────────────────────────────────────────────
                    if (check(TT::KEYWORD_OPERATOR) || checkContextual(TT::KEYWORD_OPERATOR))
                    {
                        advance(); // (AR) استهلاك 'عامل'
                        // (AR) قراءة المعدلات بعد 'عامل' (الصفة بعد الموصوف)
                        access = parseModifiers(isStatic, isVirtual, isAbstract);
                        auto operatorDecl = parseOperatorDecl();
                        if (operatorDecl)
                        {
                            if (auto *opDecl = dynamic_cast<AST::OperatorDecl *>(operatorDecl.get()))
                            {
                                opDecl->access = access;
                            }
                            return operatorDecl;
                        }
                        return nullptr;
                    }

                    // ─────────────────────────────────────────────────────────────
                    // (AR) [6] دعم 'متغير' كنوع حقل في الأصناف القالبية (نوع ديناميكي)
                    // (EN) Support 'متغير' (var) as field type in template classes (dynamic type)
                    // ─────────────────────────────────────────────────────────────
                    if (check(TT::KEYWORD_VAR))
                    {
                        advance(); // (AR) استهلاك 'متغير'
                        // (AR) قراءة المعدلات بعد 'متغير' (الصفة بعد الموصوف)
                        access = parseModifiers(isStatic, isVirtual, isAbstract);
                        Token nameToken = consume(TT::IDENTIFIER, "");
                        ExprPtr initializer = nullptr;
                        if (match(TT::OP_ASSIGN))
                        {
                            initializer = parseExpression();
                        }
                        if (matchSemicolon())
                        {
                        }
                        return std::make_unique<FieldDecl>(nameToken.getValue(), Types::SadTypeKind::Class,
                                                           std::move(initializer), access, isStatic, nameToken.getPosition());
                    }

                    // ─────────────────────────────────────────────────────────────
                    // (AR) [7] التحقق إذا كان المعرّف الحالي هو اسم معامل نوع قالب
                    //      مثال: ت اسم_الحقل — حيث ت هو معامل النوع في القالب
                    // (EN) Check if current identifier is a template type parameter name
                    // ─────────────────────────────────────────────────────────────
                    if (check(TT::IDENTIFIER))
                    {
                        std::string identName = current_.getValue();
                        bool isTemplateTypeParam = false;
                        for (const auto &tp : typeParams)
                        {
                            if (tp.name == identName)
                            {
                                isTemplateTypeParam = true;
                                break;
                            }
                        }
                        if (isTemplateTypeParam)
                        {
                            // (AR) هذا معامل نوع قالب - نعامله كنوع حقل OBJECT (عام)
                            advance(); // (AR) استهلاك اسم معامل النوع
                            Token nameToken = consume(TT::IDENTIFIER, "");
                            ExprPtr initializer = nullptr;
                            if (match(TT::OP_ASSIGN))
                            {
                                initializer = parseExpression();
                            }
                            if (matchSemicolon())
                            {
                            }
                            return std::make_unique<FieldDecl>(nameToken.getValue(), Types::SadTypeKind::Class,
                                                               std::move(initializer), access, isStatic, nameToken.getPosition());
                        }
                    }

                    // ─────────────────────────────────────────────────────────────
                    // (AR) [8] حقل بنوع مدمج عادي (نص، رقم، منطقي، إلخ)
                    // (EN) Field with built-in type (string, number, boolean, etc.)
                    // ─────────────────────────────────────────────────────────────
                    if (isTypeToken(current_.getType()) ||
                        (check(TT::IDENTIFIER) && isClassName(current_.getValue())))
                    {
                        return parseFieldDeclaration(access, isStatic);
                    }

                    errorCatalog(Errors::ErrorCode::SYN_UNKNOWN_ELEMENT, {{"what_ar", "عضو الصنف"}, {"what_en", "class member"}, {"found", current_.getValue()}, {"allowed", kw(TT::KEYWORD_FUNCTION) + "، " + kw(TT::KEYWORD_VAR) + "، " + kw(TT::KEYWORD_CONST) + "، " + kw(TT::KEYWORD_PROPERTY) + "، " + kw(TT::KEYWORD_CONSTRUCTOR)}});
                    advance();
                    return nullptr;
                };

                if (match(TT::BRACE_LEFT))
                {
                    while (!check(TT::BRACE_RIGHT) && !isAtEnd())
                    {
                        auto member = parseTemplateClassMember();
                        if (member)
                        {
                            members.push_back(std::move(member));
                        }
                    }
                    if (!match(TT::BRACE_RIGHT))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "}"}, {"ctx_ar", "لإنهاء الصنف"}, {"ctx_en", "to close the class"}});
                    }
                }
                else
                {
                    // (AR) صيغة بدون أقواس: صنف اسم ... نهاية
                    // (EN) No-brace syntax: class name ... end
                    while (!check(TT::KEYWORD_END) && !isAtEnd())
                    {
                        auto member = parseTemplateClassMember();
                        if (member)
                        {
                            members.push_back(std::move(member));
                        }
                    }
                    if (!match(TT::KEYWORD_END))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_UNCLOSED_CONSTRUCT, {{"construct_ar", "كتلة الصنف"}, {"construct_en", "class block"}, {"closer", kw(TT::KEYWORD_END)}});
                    }
                }

                auto templateClass = std::make_unique<AST::TemplateClassDecl>(
                    std::move(typeParams),
                    className,
                    baseClasses,
                    std::move(members),
                    false, // isExported
                    startPos);
                templateClass->whereClause = std::move(classWhereClause);
                templateClass->docComment = std::move(docComment);
                return templateClass;
            }
            else
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_KEYWORD, {{"kw", kw(TT::KEYWORD_FUNCTION)}, {"ctx_ar", "أو '" + kw(TT::KEYWORD_CLASS) + "' بعد معاملات القالب"}, {"ctx_en", "or '" + kw(TT::KEYWORD_CLASS) + "' after the template parameters"}});
                return nullptr;
            }
        }

        /**
         * @brief (AR) يحلل تصريح فضاء الأسماء.
         *        (EN) Parses namespace declaration.
         *
         * @details
         * Syntax / النحو:
         *   فضاء اسم ... نهاية_فضاء
         *   namespace name { ... }
         */
        StmtPtr ParserCore::parseNamespaceDecl()
        {
            // (AR) التقاط التعليق التوثيقي المعلق
            // (EN) Capture pending doc comment
            std::string docComment = consumePendingDocComment();

            auto startPos = previous_.getPosition();

            // (AR) توقع اسم فضاء الأسماء
            // (EN) Expect namespace name
            if (!check(TT::IDENTIFIER))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "فضاء الأسماء"}, {"what_en", "namespace"}, {"ctx_ar", "بعد '" + kw(TT::KEYWORD_NAMESPACE) + "'"}, {"ctx_en", "after '" + kw(TT::KEYWORD_NAMESPACE) + "'"}});
                return nullptr;
            }

            std::string nsName = current_.getValue();
            advance();

            // (AR) تحليل أعضاء فضاء الأسماء
            // (EN) Parse namespace members
            AST::StmtList members;

            // (AR) صيغة {} أُزيلت — استخدم 'نهاية' فقط
            // (EN) Brace syntax removed — use 'نهاية' only
            if (check(TT::BRACE_LEFT))
            {
                errorCatalog(Errors::ErrorCode::SYN_REMOVED_SYNTAX, {{"old", kw(TT::KEYWORD_NAMESPACE) + " { ... }"}, {"new", kw(TT::KEYWORD_END)}, {"example", kw(TT::KEYWORD_NAMESPACE) + " اسم\n ...\n" + kw(TT::KEYWORD_END)}});
            }

            {
                // (AR) صيغة بدون أقواس: فضاء اسم ... نهاية
                // (EN) No-brace syntax
                while (!check(TT::KEYWORD_END_NAMESPACE) && !check(TT::KEYWORD_END) &&
                       !checkContextual(TT::KEYWORD_END_NAMESPACE) &&
                       !isAtEnd())
                {
                    auto decl = parseDeclaration();
                    if (decl)
                    {
                        members.push_back(std::move(decl));
                    }
                }
                if (!match(TT::KEYWORD_END_NAMESPACE) && !match(TT::KEYWORD_END) &&
                    !matchContextual(TT::KEYWORD_END_NAMESPACE))
                {
                    errorCatalog(Errors::ErrorCode::SYN_UNCLOSED_CONSTRUCT, {{"construct_ar", "فضاء الأسماء"}, {"construct_en", "namespace"}, {"closer", kw(TT::KEYWORD_END)}});
                }
            }

            auto nsDecl = std::make_unique<AST::NamespaceDecl>(
                nsName,
                std::move(members),
                startPos);
            nsDecl->docComment = std::move(docComment);
            return nsDecl;
        }

        /**
         * @brief (AR) يحلل تصريح تحميل عامل.
         *        (EN) Parses operator overload declaration.
         *
         * @details
         * Syntax / النحو:
         *   عامل +(آخر: نوع) نوع { ... }
         *   operator +(other: Type) -> Type { ... }
         */
        StmtPtr ParserCore::parseOperatorDecl()
        {
            auto startPos = previous_.getPosition();

            // (AR) توقع رمز العامل
            // (EN) Expect operator symbol
            std::string opSymbol;

            // (AR) التحقق من أنواع العوامل المختلفة
            // (EN) Check for different operator types
            switch (current_.getType())
            {
            case TT::OP_PLUS:
                opSymbol = "+";
                break;
            case TT::OP_MINUS:
                opSymbol = "-";
                break;
            case TT::OP_MULTIPLY:
                opSymbol = "*";
                break;
            case TT::OP_DIVIDE:
                opSymbol = "/";
                break;
            case TT::OP_FLOOR_DIVIDE:
                opSymbol = "//";
                break;
            case TT::OP_MODULO:
                opSymbol = "%";
                break;
            case TT::OP_POWER:
                opSymbol = "**";
                break;
            case TT::OP_EQUAL:
                opSymbol = "==";
                break;
            case TT::OP_NOT_EQUAL:
                opSymbol = "!=";
                break;
            case TT::OP_LESS:
                opSymbol = "<";
                break;
            case TT::OP_GREATER:
                opSymbol = ">";
                break;
            case TT::OP_LESS_EQUAL:
                opSymbol = "<=";
                break;
            case TT::OP_GREATER_EQUAL:
                opSymbol = ">=";
                break;
            // (AR) العوامل البتية / (EN) Bitwise operators
            case TT::OP_XOR:
                opSymbol = "^";
                break;
            case TT::OP_BITWISE_AND:
                opSymbol = "&";
                break;
            case TT::OP_BITWISE_OR:
                opSymbol = "|";
                break;
            case TT::OP_SHIFT_LEFT:
                opSymbol = "<<";
                break;
            case TT::OP_SHIFT_RIGHT:
                opSymbol = ">>";
                break;
            case TT::BRACKET_LEFT:
                advance();
                if (match(TT::BRACKET_RIGHT))
                {
                    // (AR) تحقق: هل يتبعه = ليكون عامل []=؟
                    // (EN) Check: is it followed by = to make []=?
                    if (match(TT::OP_ASSIGN))
                    {
                        opSymbol = "[]=";
                    }
                    else
                    {
                        opSymbol = "[]";
                    }
                }
                else
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "]"}, {"ctx_ar", "بعد '['"}, {"ctx_en", "after '['"}});
                    return nullptr;
                }
                break;
            case TT::PAREN_LEFT:
                advance();
                if (match(TT::PAREN_RIGHT))
                {
                    opSymbol = "()";
                }
                else
                {
                    errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ")"}, {"ctx_ar", "بعد '('"}, {"ctx_en", "after '('"}});
                    return nullptr;
                }
                break;
            case TT::IDENTIFIER:
            {
                // (AR) عوامل التحويل الضمني: عامل نص() / عامل رقم() / عامل منطقي()
                // (EN) Implicit conversion operators: عامل نص() / عامل رقم() / عامل منطقي()
                std::string name = current_.getValue();
                if (name == "نص" || name == "رقم" || name == "منطقي")
                {
                    opSymbol = name;
                    advance(); // (AR) استهلاك اسم التحويل
                    break;
                }
                // (AR) مُعرّف غير معروف كعامل
                errorCatalog(Errors::ErrorCode::SYN_UNKNOWN_ELEMENT, {{"what_ar", "عامل التحويل"}, {"what_en", "conversion operator"}, {"found", name}, {"allowed", kw(TT::TYPE_STRING) + "، " + kw(TT::TYPE_INTEGER) + "، " + kw(TT::TYPE_BOOLEAN)}});
                return nullptr;
            }
            default:
                errorCatalog(Errors::ErrorCode::SYN_UNKNOWN_ELEMENT, {{"what_ar", "رمز العامل"}, {"what_en", "operator symbol"}, {"found", current_.getValue()}, {"allowed", "+ - * / == != < > <= >= [] ()"}});
                return nullptr;
            }

            if (opSymbol != "[]" && opSymbol != "[]=" && opSymbol != "()" &&
                opSymbol != "نص" && opSymbol != "رقم" && opSymbol != "منطقي")
            {
                advance(); // (AR) استهلاك رمز العامل
            }

            // (AR) تحليل المعاملات
            // (EN) Parse parameters
            if (!match(TT::PAREN_LEFT))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", "("}, {"ctx_ar", "بعد رمز العامل"}, {"ctx_en", "after the operator symbol"}});
                return nullptr;
            }

            std::vector<AST::Parameter> params;
            if (!check(TT::PAREN_RIGHT))
            {
                do
                {
                    if (!check(TT::IDENTIFIER))
                    {
                        errorCatalog(Errors::ErrorCode::SYN_EXPECTED_NAME, {{"what_ar", "المعامل"}, {"what_en", "parameter"}, {"ctx_ar", "في قائمة معاملات العامل"}, {"ctx_en", "in the operator parameter list"}});
                        break;
                    }

                    std::string paramName = current_.getValue();
                    advance();

                    Types::SadTypeKind paramType = Types::SadTypeKind::Void;
                    if (match(TT::COLON))
                    {
                        if (isTypeToken(current_.getType()))
                        {
                            paramType = mapTokenTypeToKind(current_.getType());
                            advance();
                        }
                        else if (check(TT::IDENTIFIER))
                        {
                            paramType = Types::SadTypeKind::Class;
                            advance();
                        }
                    }

                    params.emplace_back(paramName, paramType);

                } while (matchComma());
            }

            if (!match(TT::PAREN_RIGHT))
            {
                errorCatalog(Errors::ErrorCode::SYN_EXPECTED_SYMBOL, {{"symbol", ")"}, {"ctx_ar", "بعد معاملات العامل"}, {"ctx_en", "after the operator parameters"}});
                return nullptr;
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) تحويل العوامل الأحادية: إذا كان + أو - أو ! بدون معاملات → u+ أو u- أو !
            // (EN) Convert unary operators: if +/- /! with no params → u+/u-/!
            // ═══════════════════════════════════════════════════════════════════
            if (params.empty())
            {
                if (opSymbol == "-")
                    opSymbol = "u-";
                else if (opSymbol == "+")
                    opSymbol = "u+";
            }

            // (AR) تحليل نوع الإرجاع — يأتي مباشرة بعد القوس (بدون سهم)
            // (EN) Parse return type — bare type after closing paren (no arrow)
            Types::SadTypeKind returnType = Types::SadTypeKind::Void;
            if (isTypeToken(current_.getType()))
            {
                returnType = mapTokenTypeToKind(current_.getType());
                advance();
            }
            else if (check(TT::IDENTIFIER) && !check(TT::KEYWORD_END) &&
                     nextToken_.getType() != TT::OP_ASSIGN &&
                     nextToken_.getType() != TT::PAREN_LEFT &&
                     nextToken_.getType() != TT::DOT)
            {
                // (AR) قد يكون نوع صنف مخصص كنوع إرجاع
                // (EN) Could be a custom class type as return type
                returnType = Types::SadTypeKind::Class;
                advance();
            }

            // (AR) إذا استخدم سهم -> نعطي رسالة خطأ
            // (EN) If arrow -> is used, give error message
            if (check(TT::ARROW))
            {
                errorCatalog(Errors::ErrorCode::SYN_REMOVED_SYNTAX, {{"old", "-> نوع_الإرجاع"}, {"new", "نوع الإرجاع بعد الأقواس مباشرة"}, {"example", kw(TT::KEYWORD_OPERATOR) + " +(آخر: نوع) نوع_الإرجاع"}});
            }

            // (AR) تحليل جسم العامل
            // (EN) Parse operator body - ends with 'نهاية'
            StmtPtr body = parseBlockStmt();

            return std::make_unique<AST::OperatorDecl>(
                opSymbol,
                std::move(params),
                returnType,
                std::move(body),
                AST::AccessModifier::PUBLIC,
                startPos);
        }

        // =========================================================================
        // (AR) تحليل تنفيذ القوالب / (EN) Template Instantiation Parsing
        // =========================================================================

        /**
         * @brief (AR) يتحقق إذا كان الرمز الحالي بداية type argument
         *        (EN) Checks if current token starts a type argument
         */
        bool ParserCore::isTypeArgumentStart()
        {
            // (AR) أنواع مدمجة / (EN) Built-in types
            if (isTypeToken(current_.getType()))
            {
                return true;
            }

            // (AR) معرّف يمكن أن يكون اسم صنف أو معامل قالب
            // (EN) Identifier could be class name or template parameter
            if (check(TT::IDENTIFIER))
            {
                return true;
            }

            return false;
        }

        /**
         * @brief (AR) يحلل تنفيذ قالب مثل: أكبر<رقم>(10, 20)
         *        (EN) Parses template instantiation like: max<int>(10, 20)
         */
        ExprPtr ParserCore::parseTemplateInstantiation(const std::string &templateName,
                                                       const Lexer::Position &pos)
        {
            // (AR) حفظ الموقع الحالي للرجوع إذا لم يكن قالب
            // (EN) Save current position to rollback if not a template
            // Note: We'll use peek/advance pattern instead of manual index tracking

            // (AR) محاولة قراءة <
            // (EN) Try to consume <
            if (!match(TT::OP_LESS))
            {
                return nullptr;
            }

            // (AR) تحليل وسائط الأنواع
            // (EN) Parse type arguments
            std::vector<Types::SadTypeKind> typeArgs;
            std::vector<std::string> typeArgNames;
            // ==========================================================================
            // (AR) [Phase 4] وسائط ثابتة (const-generic args) موازية لوسائط الأنواع
            //      مثال: f<عشري، 4> → typeArgs=[Float], constArgs=[4]
            //      argumentKindOrder يحفظ الترتيب الأصلي (0=type, 1=const)
            //      لإعادة بنائه عند instantiation
            // ==========================================================================
            std::vector<AST::ExprPtr> constArgs;
            std::vector<int> argKindOrder;

            // (AR) التحقق من وجود نوع/قيمة بعد <
            // (EN) Check for type or constant after <
            //      (نسمح بالحرفيات أيضاً لدعم const-generics)
            if (!isTypeArgumentStart() &&
                !check(TT::NUMBER_INTEGER) && !check(TT::NUMBER_DOUBLE) &&
                !check(TT::STRING_LITERAL) &&
                !check(TT::LITERAL_TRUE) && !check(TT::LITERAL_FALSE))
            {
                // (AR) ليس تنفيذ قالب
                // (EN) Not template instantiation
                return nullptr;
            }

            do
            {
                // ==================================================================
                // (AR) [Phase 4] التمييز بين وسيط نوع ووسيط ثابت:
                //      حرفيات (أرقام/نصوص/منطقي) → وسائط ثابتة
                //      أسماء أنواع/معرفات → وسائط أنواع
                // (EN) [Phase 4] Distinguish type-arg vs const-arg by token kind:
                //      literals → const-generic args; type tokens/identifiers → type args
                // ==================================================================
                if (check(TT::NUMBER_INTEGER) || check(TT::NUMBER_DOUBLE) ||
                    check(TT::STRING_LITERAL) ||
                    check(TT::LITERAL_TRUE) || check(TT::LITERAL_FALSE))
                {
                    // (AR) وسيط ثابت — نقرأ رمزاً حرفياً واحداً فقط
                    //      (لا نستخدم parseExpression لأنه يبتلع '>' كعامل مقارنة)
                    // (EN) Const-generic argument — consume a single literal token only
                    //      (avoid parseExpression which would swallow '>' as comparison op)
                    Lexer::Token litTok = current_;
                    advance();
                    auto litExpr = std::make_unique<AST::LiteralExpr>(litTok);
                    constArgs.push_back(std::move(litExpr));
                    argKindOrder.push_back(1);
                }
                else
                {
                    Types::SadTypeKind argType = Types::SadTypeKind::Unknown;
                    std::string typeName;

                    if (isTypeToken(current_.getType()))
                    {
                        // (AR) نوع مدمج
                        // (EN) Built-in type
                        argType = mapTokenTypeToKind(current_.getType());
                        typeName = current_.getValue();
                        advance();
                    }
                    else if (check(TT::IDENTIFIER))
                    {
                        // (AR) قد يكون اسم صنف
                        // (EN) Could be class name
                        typeName = current_.getValue();
                        argType = Types::SadTypeKind::Class;
                        advance();
                    }
                    else
                    {
                        // (AR) خطأ: توقعت نوع
                        // (EN) Error: expected type
                        return nullptr;
                    }

                    typeArgs.push_back(argType);
                    typeArgNames.push_back(typeName);
                    argKindOrder.push_back(0);
                }

            } while (matchComma());

            // (AR) توقع >
            // (EN) Expect >
            if (!match(TT::OP_GREATER))
            {
                // (AR) ليس تنفيذ قالب صالح
                // (EN) Not valid template instantiation
                return nullptr;
            }

            // (AR) إنشاء عقدة TemplateInstantiation مع وسائط النوع والثابت
            // (EN) Create TemplateInstantiation node with both type and const args
            auto node = std::make_unique<AST::TemplateInstantiation>(
                templateName,
                std::move(typeArgs),
                std::move(typeArgNames),
                pos);
            node->constArguments = std::move(constArgs);
            node->argumentKindOrder = std::move(argKindOrder);
            return node;
        }

    } // namespace Parser
} // namespace Sad
