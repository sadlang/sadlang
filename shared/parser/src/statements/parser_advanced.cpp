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
#include "class_manager.h"
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
         * @brief (AR) يحلل مصفوفة حرفية أو list comprehension: [1, 2, 3] أو [x * 2 for x in list].
         *        (EN) Parses array literal or list comprehension: [1, 2, 3] or [x * 2 for x in list].
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

            // Parse first element/expression
            // (AR) تحليل العنصر/التعبير الأول
            auto firstExpr = parseExpression();
            if (!firstExpr)
            {
                errorBilingual(
                    "خطأ: لم يتم تحليل التعبير الأول في المصفوفة. تأكد من أن التعبير صحيح.",
                    "Error: failed to parse first expression in array. Make sure the expression is valid.");
                return nullptr;
            }

            // Check if this is a list comprehension
            // (AR) التحقق إذا كان list comprehension
            if (check(TT::KEYWORD_FOR))
            {
                // This is a list comprehension: [expr for var in iterable if cond]
                // (AR) هذا list comprehension

                advance(); // consume 'for'

                // Parse variable
                // (AR) تحليل المتغير
                if (!check(TT::IDENTIFIER))
                {
                    errorBilingual(
                        "خطأ: توقعت اسم متغير بعد 'for' في list comprehension. مثال: [x for x in list]",
                        "Error: expected variable name after 'for' in list comprehension. Example: [x for x in list]");
                    return nullptr;
                }
                Token var = peek();
                advance();

                // Expect 'in' keyword
                // (AR) توقع كلمة 'في'
                if (!check(TT::KEYWORD_IN))
                {
                    errorBilingual(
                        "خطأ: توقعت 'في' بعد اسم المتغير في list comprehension. الصيغة: [expr for var in iterable]",
                        "Error: expected 'in' after variable name in list comprehension. Format: [expr for var in iterable]");
                    return nullptr;
                }
                advance();

                // Parse iterable
                // (AR) تحليل المجموعة القابلة للتكرار
                auto iterable = parseExpression();
                if (!iterable)
                {
                    errorBilingual(
                        "خطأ: فشل تحليل المجموعة القابلة للتكرار بعد 'في'. تأكد من صيغة list comprehension.",
                        "Error: failed to parse iterable after 'in'. Make sure list comprehension syntax is correct.");
                    return nullptr;
                }

                // Optional condition
                // (AR) الشرط الاختياري
                ExprPtr condition = nullptr;
                if (match(TT::KEYWORD_IF))
                {
                    condition = parseExpression();
                    if (!condition)
                    {
                        errorBilingual(
                            "خطأ: تعبير شرط غير صحيح بعد 'إذا' في list comprehension.",
                            "Error: invalid condition expression after 'if' in list comprehension.");
                        return nullptr;
                    }
                }

                if (!check(TT::BRACKET_RIGHT))
                {
                    // (AR) محاولة التعافي: إدراج ']' المفقودة في list comprehension
                    // (EN) Try recovery: insert missing ']' in list comprehension
                    if (!tryRecoverFromError(TT::BRACKET_RIGHT, "list comprehension"))
                    {
                        errorBilingual(
                            "خطأ: توقعت ']' في نهاية list comprehension. الصيغة: [expr for var in iterable if cond]",
                            "Error: expected ']' at end of list comprehension. Format: [expr for var in iterable if cond]");
                        return nullptr;
                    }
                }
                else
                {
                    consume(TT::BRACKET_RIGHT, "");
                }

                // Create list comprehension node
                // (AR) إنشاء عقدة List Comprehension
                return std::make_unique<ListComprehensionExpr>(
                    std::move(firstExpr),
                    var.getValue(),
                    std::move(iterable),
                    std::move(condition),
                    var.getPosition());
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
                    errorBilingual(
                        "خطأ: تعبير عنصر غير صحيح في المصفوفة. تأكد من صيغة العنصر.",
                        "Error: invalid element expression in array. Make sure element syntax is correct.");
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
                    errorBilingual(
                        "خطأ: لم يتم إغلاق المصفوفة. توقعت ']'. هل نسيت إغلاق القوس المربع؟",
                        "Error: array not closed. Expected ']'. Did you forget to close the bracket?");
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
         * @brief (AR) يحلل خريطة حرفية أو dict comprehension: {k: v} أو {k: v for k, v in items}.
         *        (EN) Parses map literal or dict comprehension: {k: v} or {k: v for k, v in items}.
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
                    errorBilingual(
                        "خطأ: توقعت تعبيراً بعد '...' في الخريطة.",
                        "Error: expected expression after '...' in map.");
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

                consume(TT::BRACE_RIGHT, "(AR) توقعت '}' (EN) Expected '}'");
                return std::make_unique<MapExpr>(std::move(pairs), previous().getPosition());
            }

            // Parse first expression using parseTernary to avoid consuming 'for' keyword
            // (AR) تحليل التعبير الأول باستخدام parseTernary لتجنب استهلاك 'for'
            auto firstKey = parseTernary();
            if (!firstKey)
            {
                errorBilingual(
                    "خطأ: فشل تحليل التعبير - تعبير غير صحيح.",
                    "Error: failed to parse expression - invalid expression.");
                return nullptr;
            }

            // Check if this is a set comprehension: {expr for var in iterable}
            // (AR) التحقق إذا كان set comprehension
            if (check(TT::KEYWORD_FOR))
            {
                // This is a set comprehension: {expr for var in iterable if cond}
                // (AR) هذا set comprehension

                advance(); // consume 'for'

                // Parse loop variable
                // (AR) تحليل متغير الحلقة
                if (!check(TT::IDENTIFIER))
                {
                    errorBilingual(
                        "خطأ: توقعت اسم متغير حلقة بعد 'for' في set comprehension. مثال: {x for x in list}",
                        "Error: expected loop variable name after 'for' in set comprehension. Example: {x for x in list}");
                    return nullptr;
                }
                Token loopVar = peek();
                advance();

                // Expect 'in' keyword
                // (AR) توقع كلمة 'في'
                if (!check(TT::KEYWORD_IN))
                {
                    errorBilingual(
                        "خطأ: توقعت 'في' بعد متغير الحلقة. الصيغة: {expr for var in iterable}",
                        "Error: expected 'in' after loop variable. Format: {expr for var in iterable}");
                    return nullptr;
                }
                advance();

                // Parse iterable
                // (AR) تحليل المجموعة القابلة للتكرار
                auto iterable = parseExpression();
                if (!iterable)
                {
                    errorBilingual(
                        "خطأ: فشل تحليل المجموعة القابلة للتكرار في set comprehension.",
                        "Error: failed to parse iterable in set comprehension.");
                    return nullptr;
                }

                // Optional condition
                // (AR) الشرط الاختياري
                ExprPtr condition = nullptr;
                if (match(TT::KEYWORD_IF))
                {
                    condition = parseExpression();
                    if (!condition)
                    {
                        errorBilingual(
                            "خطأ: تعبير شرط غير صحيح بعد 'إذا' في set comprehension.",
                            "Error: invalid condition expression after 'if' in set comprehension.");
                        return nullptr;
                    }
                }

                if (!check(TT::BRACE_RIGHT))
                {
                    errorBilingual(
                        "خطأ: توقعت '}' في نهاية set comprehension.",
                        "Error: expected '}' at end of set comprehension.");
                    return nullptr;
                }
                consume(TT::BRACE_RIGHT, "");

                // Create set comprehension node
                // (AR) إنشاء عقدة Set Comprehension
                return std::make_unique<SetComprehensionExpr>(
                    std::move(firstKey),
                    loopVar.getValue(),
                    std::move(iterable),
                    std::move(condition));
            }

            // Check if this is a dict (has colon or =) or set (no colon)
            // (AR) التحقق إذا كان dict (له : أو =) أو set (بدون :)
            if (!check(TT::COLON) && !check(TT::OP_ASSIGN))
            {
                errorBilingual(
                    "خطأ: توقعت ':' بعد مفتاح الخريطة. الصيغة: {مفتاح: قيمة، ...}. لـ Set Comprehension استخدم: {expr for x in list}",
                    "Error: expected ':' after map key. Format: {key: value, ...}. For Set Comprehension use: {expr for x in list}");
                return nullptr;
            }
            advance(); // consume ':' or '='

            // Parse first value expression using parseTernary to avoid consuming 'for'
            // (AR) تحليل تعبير القيمة باستخدام parseTernary لتجنب استهلاك 'for'
            auto firstValue = parseTernary();
            if (!firstValue)
            {
                errorBilingual(
                    "خطأ: فشل تحليل قيمة الخريطة - تعبير غير صحيح. تأكد من أن القيمة صحيحة.",
                    "Error: failed to parse map value - invalid expression. Make sure the value is valid.");
                return nullptr;
            }

            // Check if this is a dict comprehension
            // (AR) التحقق إذا كان dict comprehension
            if (check(TT::KEYWORD_FOR))
            {
                // This is a dict comprehension: {k: v for var in iterable if cond}
                // Note: firstKey and firstValue are the TEMPLATE expressions (k, v)
                // The loop variable comes AFTER 'for'
                // (AR) هذا dict comprehension - firstKey و firstValue هي القوالب

                advance(); // consume 'for'

                // Parse loop variable (can be single: 'x' or tuple: 'k, v')
                // For now, we support single variable only
                // (AR) تحليل متغير الحلقة
                if (!check(TT::IDENTIFIER))
                {
                    errorBilingual(
                        "خطأ: توقعت اسم متغير حلقة بعد 'for' في dict comprehension. مثال: {k: v for item in list}",
                        "Error: expected loop variable name after 'for' in dict comprehension. Example: {k: v for item in list}");
                    return nullptr;
                }
                Token loopVar = peek();
                advance();

                // Expect 'in' keyword
                // (AR) توقع كلمة 'في'
                if (!check(TT::KEYWORD_IN))
                {
                    errorBilingual(
                        "خطأ: توقعت 'في' بعد متغير الحلقة. الصيغة: {k: v for var in iterable}",
                        "Error: expected 'in' after loop variable. Format: {k: v for var in iterable}");
                    return nullptr;
                }
                advance();

                // Parse iterable
                // (AR) تحليل المجموعة القابلة للتكرار
                auto iterable = parseExpression();
                if (!iterable)
                {
                    errorBilingual(
                        "خطأ: فشل تحليل المجموعة القابلة للتكرار في dict comprehension.",
                        "Error: failed to parse iterable in dict comprehension.");
                    return nullptr;
                }

                // Optional condition
                // (AR) الشرط الاختياري
                ExprPtr condition = nullptr;
                if (match(TT::KEYWORD_IF))
                {
                    condition = parseExpression();
                    if (!condition)
                    {
                        errorBilingual(
                            "خطأ: تعبير شرط غير صحيح بعد 'إذا' في dict comprehension.",
                            "Error: invalid condition expression after 'if' in dict comprehension.");
                        return nullptr;
                    }
                }

                if (!check(TT::BRACE_RIGHT))
                {
                    // (AR) محاولة التعافي: إدراج '}' المفقودة في dict comprehension
                    if (!tryRecoverFromError(TT::BRACE_RIGHT, "dict comprehension"))
                    {
                        errorBilingual(
                            "خطأ: توقعت '}' في نهاية dict comprehension.",
                            "Error: expected '}' at end of dict comprehension.");
                        return nullptr;
                    }
                }
                else
                {
                    consume(TT::BRACE_RIGHT, "");
                }

                // Create dict comprehension node
                // (AR) إنشاء عقدة Dict Comprehension
                return std::make_unique<DictComprehensionExpr>(
                    std::move(firstKey),
                    std::move(firstValue),
                    loopVar.getValue(),
                    std::move(iterable),
                    std::move(condition),
                    loopVar.getPosition());
            }

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
                        errorBilingual(
                            "خطأ: توقعت تعبيراً بعد '...' في الخريطة.",
                            "Error: expected expression after '...' in map.");
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
                    errorBilingual(
                        "خطأ: فشل تحليل مفتاح إضافي في الخريطة.",
                        "Error: failed to parse additional key in map.");
                    return nullptr;
                }

                if (!check(TT::COLON) && !check(TT::OP_ASSIGN))
                {
                    errorBilingual(
                        "خطأ: توقعت ':' بعد مفتاح الخريطة. هل نسيت الفاصلة قبل المفتاح التالي؟",
                        "Error: expected ':' after map key. Did you forget the colon before the value?");
                    return nullptr;
                }
                advance(); // consume ':' or '='

                auto value = parseExpression();
                if (!value)
                {
                    errorBilingual(
                        "خطأ: فشل تحليل قيمة إضافية في الخريطة.",
                        "Error: failed to parse additional value in map.");
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
                    errorBilingual(
                        "خطأ: لم يتم إغلاق الخريطة. توقعت '}'. هل نسيت إغلاق الأقواس المعقوفة؟",
                        "Error: map not closed. Expected '}'. Did you forget to close the braces?");
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
                errorBilingual(
                    "خطأ: توقعت تعبير بعد 'match'",
                    "Error: Expected expression after 'match'");
                return nullptr;
            }

            // (AR) صيغة {} أُزيلت — استخدم 'نهاية' فقط
            // (EN) Brace syntax removed — use 'نهاية' only
            if (check(TT::BRACE_LEFT))
            {
                errorBilingual(
                    "خطأ نحوي: صيغة {} أُزيلت من 'طابق'. استخدم 'نهاية' بدلاً منها.\n"
                    "💡 مثال:\n    طابق (قيمة)\n        عندما 1: ...\n    نهاية",
                    "Syntax error: {} syntax removed from 'match'. Use 'نهاية' instead.\n"
                    "💡 Example:\n    match (value)\n        when 1: ...\n    end");
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
                    error(
                        "(AR) ❌ `حالة` لا تُستخدم داخل `طابق`. استخدم `عندما` بدلاً منها.\n"
                        "(EN) `حالة` cannot be used inside `طابق`. Use `عندما` instead.");
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
                    if (!match(TT::COLON))
                    {
                        errorBilingual(
                            "خطأ: توقعت ':' بعد 'افتراضي'",
                            "Error: Expected ':' after 'default'");
                        return nullptr;
                    }
                    // (AR) تحليل جسم الحالة الافتراضية
                    // (EN) Parse default case body
                    std::vector<StmtPtr> defaultBody;
                    while (!isMatchEnd() && !isAtEnd())
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
                    break; // (AR) الافتراضي يجب أن يكون الأخير / (EN) Default must be last
                }
                else
                {
                    errorBilingual(
                        "خطأ: توقعت 'حالة' أو 'عندما' أو 'افتراضي' أو 'نهاية'",
                        "Error: Expected 'case', 'when', 'default', or 'end'");
                    return nullptr;
                }
            }

            if (cases.empty())
            {
                errorBilingual(
                    "خطأ: جملة match يجب أن تحتوي على فرع case واحد على الأقل",
                    "Error: Match statement must have at least one case clause");
                return nullptr;
            }

            // Expect closing: 'نهاية'
            // (AR) توقع إنهاء: 'نهاية'
            if (!match(TT::KEYWORD_END))
            {
                errorBilingual(
                    "خطأ: توقعت 'نهاية' في نهاية جملة طابق",
                    "Error: Expected 'end' at end of match statement");
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
                errorBilingual(
                    "خطأ: توقعت 'عندما'",
                    "Error: Expected 'عندما' (when)");
                return AST::CaseClause(nullptr, nullptr, {});
            }

            // Parse pattern
            // (AR) تحليل النمط
            auto pattern = parsePattern();

            if (!pattern)
            {
                errorBilingual(
                    "خطأ: توقعت نمط بعد 'case'",
                    "Error: Expected pattern after 'case'");
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
                    errorBilingual(
                        "خطأ: توقعت تعبير بعد 'if' في guard",
                        "Error: Expected expression after 'if' in guard");
                }
            }

            // Expect colon
            // (AR) توقع نقطتين رأسيتين
            if (!match(TT::COLON))
            {
                errorBilingual(
                    "خطأ: توقعت ':' بعد نمط case",
                    "Error: Expected ':' after case pattern");
                return AST::CaseClause(nullptr, nullptr, {});
            }

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

            // Check for OR pattern: a | b | c (using OP_OR)
            // (AR) التحقق من نمط OR: a | b | c
            if (check(TT::OP_OR))
            {
                std::vector<std::unique_ptr<AST::Pattern>> alternatives;
                alternatives.push_back(std::move(primary));

                while (match(TT::OP_OR))
                {
                    auto alt = parsePrimaryPattern();
                    if (alt)
                    {
                        alternatives.push_back(std::move(alt));
                    }
                    else
                    {
                        errorBilingual(
                            "خطأ: توقعت نمط بعد '||'",
                            "Error: Expected pattern after '||'");
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
                    errorBilingual(
                        "خطأ: توقعت رقماً بعد '-' في نمط طابق",
                        "Error: Expected number after '-' in match pattern");
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
                        errorBilingual(
                            "خطأ: توقعت رقم بعد '..' في نمط النطاق",
                            "Error: Expected number after '..' in range pattern");
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
                        errorBilingual(
                            "خطأ: توقعت اسم عضو بعد '.' في نمط التعداد",
                            "Error: Expected variant name after '.' in enum pattern");
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
                            errorBilingual(
                                "خطأ: توقعت ')' لإغلاق أنماط التعداد الجبري",
                                "Error: Expected ')' to close ADT enum variant patterns");
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

                return std::make_unique<AST::VariablePattern>(token.getValue());
            }

            errorBilingual(
                "خطأ: توقعت نمط (رقم، نص، متغير، أو '_')",
                "Error: Expected pattern (number, string, variable, or '_')");
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
                errorBilingual(
                    "خطأ: توقعت '[' لبداية نمط القائمة",
                    "Error: Expected '[' for list pattern");
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
                        errorBilingual(
                            "خطأ: توقعت اسم متغير بعد '*' في نمط القائمة",
                            "Error: Expected variable name after '*' in list pattern");
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
                errorBilingual(
                    "خطأ: توقعت ']' في نهاية نمط القائمة",
                    "Error: Expected ']' at end of list pattern");
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
                errorBilingual(
                    "خطأ: توقعت '{' لبداية نمط البنية",
                    "Error: Expected '{' for struct pattern");
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
                    errorBilingual(
                        "خطأ: توقعت اسم حقل في نمط البنية",
                        "Error: Expected field name in struct pattern");
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
                errorBilingual(
                    "خطأ: توقعت '}' في نهاية نمط البنية",
                    "Error: Expected '}' at end of struct pattern");
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
                errorBilingual(
                    "خطأ نحوي: توقعت '<' بعد 'قالب'",
                    "Syntax error: Expected '<' after 'template'");
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
                        errorBilingual(
                            "خطأ نحوي: توقعت اسم نوع بعد 'ثابت' في معامل القالب",
                            "Syntax error: Expected type name after 'const' in template parameter");
                        break;
                    }
                    std::string constTypeName = current_.getValue();
                    advance();

                    if (!check(TT::IDENTIFIER))
                    {
                        errorBilingual(
                            "خطأ نحوي: توقعت اسم المعامل الثابت",
                            "Syntax error: Expected const parameter name");
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
                        errorBilingual(
                            "خطأ نحوي: توقعت 'نوع' في معامل القالب",
                            "Syntax error: Expected 'typename' in template parameter");
                        break;
                    }

                    // (AR) توقع اسم المعامل (مثل T أو ت)
                    // (EN) Expect parameter name (e.g., T)
                    if (!check(TT::IDENTIFIER))
                    {
                        errorBilingual(
                            "خطأ نحوي: توقعت اسم معامل النوع",
                            "Syntax error: Expected type parameter name");
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
                                errorBilingual(
                                    "خطأ نحوي: توقعت اسم القيد بعد ':'",
                                    "Syntax error: Expected constraint name after ':'");
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
                errorBilingual(
                    "خطأ نحوي: توقعت '>' لإنهاء معاملات القالب",
                    "Syntax error: Expected '>' to close template parameters");
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
                    errorBilingual(
                        "خطأ نحوي: توقعت اسم معامل النوع في جملة 'حيث'",
                        "Syntax error: Expected type parameter name in 'where' clause");
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
                        errorBilingual(
                            "خطأ نحوي: توقعت اسم النوع المرتبط بعد '.'",
                            "Syntax error: Expected associated type name after '.'");
                        break;
                    }
                    item.associatedPath = current_.getValue();
                    advance();
                }

                // (AR) توقع ':' / (EN) Expect ':'
                if (!match(TT::COLON))
                {
                    errorBilingual(
                        "خطأ نحوي: توقعت ':' بعد اسم المعامل في جملة 'حيث'",
                        "Syntax error: Expected ':' after type parameter in 'where' clause");
                    break;
                }

                // (AR) تحليل قيود مفصولة بـ +
                // (EN) Parse constraints separated by +
                do
                {
                    if (!check(TT::IDENTIFIER))
                    {
                        errorBilingual(
                            "خطأ نحوي: توقعت اسم القيد في جملة 'حيث'",
                            "Syntax error: Expected constraint name in 'where' clause");
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
                errorBilingual(
                    "خطأ نحوي: القالب يحتاج إلى معامل نوع واحد على الأقل",
                    "Syntax error: Template requires at least one type parameter");
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
                    errorBilingual(
                        "خطأ نحوي: توقعت اسم الدالة بعد 'دالة'",
                        "Syntax error: Expected function name after 'function'");
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
                    errorBilingual(
                        "خطأ نحوي: توقعت '(' بعد اسم الدالة",
                        "Syntax error: Expected '(' after function name");
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
                            errorBilingual(
                                "خطأ نحوي: توقعت اسم أو نوع المعامل",
                                "Syntax error: Expected parameter name or type");
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
                    errorBilingual(
                        "خطأ نحوي: توقعت ')' بعد معاملات الدالة",
                        "Syntax error: Expected ')' after function parameters");
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
                        if (rn == "\xD8\xB1\xD9\x82\xD9\x85") // رقم
                            returnType = Types::SadTypeKind::Integer;
                        else if (rn == "\xD9\x86\xD8\xB5") // نص
                            returnType = Types::SadTypeKind::String;
                        else if (rn == "\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A" ||
                                 rn == "\xD9\x85\xD8\xB6\xD8\xA7\xD8\xB9\xD9\x81") // عشري/مضاعف
                            returnType = Types::SadTypeKind::Float;
                        else if (rn == "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A") // منطقي
                            returnType = Types::SadTypeKind::Boolean;
                        else if (rn == "\xD9\x81\xD8\xB1\xD8\xA7\xD8\xBA") // فراغ
                            returnType = Types::SadTypeKind::Void;
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
                    errorBilingual(
                        "خطأ نحوي: توقعت اسم الصنف بعد 'صنف'",
                        "Syntax error: Expected class name after 'class'");
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
                            errorBilingual(
                                "خطأ نحوي: توقعت اسم الصنف الأب",
                                "Syntax error: Expected base class name");
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
                            errorBilingual(
                                "خطأ نحوي: في لغة ص، الصفة تأتي بعد الموصوف.\n"
                                "❌ خطأ: " +
                                    modName + " دالة ...\n"
                                              "✅ صحيح: دالة " +
                                    modName + " ...",
                                "Syntax error: In Sad, adjectives come after nouns.\n"
                                "❌ Wrong: " +
                                    modName + " function ...\n"
                                              "✅ Correct: function " +
                                    modName + " ...");
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
                                Token nameToken = consume(TT::IDENTIFIER,
                                                          "(AR) توقع اسم الحقل بعد نوع القالب. (EN) Expected field name after template type.");
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
                        error("(AR) توقع نوع أو اسم بعد 'خاصية'. (EN) Expected type or name after 'خاصية'.");
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
                        Token nameToken = consume(TT::IDENTIFIER,
                                                  "(AR) توقع اسم الحقل. (EN) Expected field name.");
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
                            Token nameToken = consume(TT::IDENTIFIER,
                                                      "(AR) توقع اسم الحقل. (EN) Expected field name.");
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

                    error("(AR) عضو صنف غير معروف. (EN) Unknown class member.");
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
                        errorBilingual(
                            "خطأ نحوي: توقعت '}' لإنهاء الصنف",
                            "Syntax error: Expected '}' to close class");
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
                        errorBilingual(
                            "خطأ نحوي: توقعت 'نهاية' لإنهاء الصنف",
                            "Syntax error: Expected 'end' to close class");
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
                errorBilingual(
                    "خطأ نحوي: توقعت 'دالة' أو 'صنف' بعد معاملات القالب",
                    "Syntax error: Expected 'function' or 'class' after template parameters");
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
                errorBilingual(
                    "خطأ نحوي: توقعت اسم فضاء الأسماء",
                    "Syntax error: Expected namespace name");
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
                errorBilingual(
                    "خطأ نحوي: صيغة {} أُزيلت من 'فضاء'. استخدم 'نهاية' بدلاً منها.\n"
                    "💡 مثال:\n    فضاء اسم\n        ...\n    نهاية",
                    "Syntax error: {} syntax removed from 'namespace'. Use 'نهاية' instead.\n"
                    "💡 Example:\n    namespace name\n        ...\n    end");
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
                    errorBilingual(
                        "خطأ نحوي: توقعت 'نهاية' لإنهاء فضاء الأسماء",
                        "Syntax error: Expected 'end' to close namespace");
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
                    errorBilingual(
                        "خطأ نحوي: توقعت ']' بعد '['",
                        "Syntax error: Expected ']' after '['");
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
                    errorBilingual(
                        "خطأ نحوي: توقعت ')' بعد '('",
                        "Syntax error: Expected ')' after '('");
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
                errorBilingual(
                    "خطأ نحوي: اسم عامل تحويل غير معروف '" + name + "'. العوامل المتاحة: نص، رقم، منطقي",
                    "Syntax error: Unknown conversion operator '" + name + "'. Available: نص, رقم, منطقي");
                return nullptr;
            }
            default:
                errorBilingual(
                    "خطأ نحوي: رمز عامل غير معروف",
                    "Syntax error: Unknown operator symbol");
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
                errorBilingual(
                    "خطأ نحوي: توقعت '(' بعد رمز العامل",
                    "Syntax error: Expected '(' after operator symbol");
                return nullptr;
            }

            std::vector<AST::Parameter> params;
            if (!check(TT::PAREN_RIGHT))
            {
                do
                {
                    if (!check(TT::IDENTIFIER))
                    {
                        errorBilingual(
                            "خطأ نحوي: توقعت اسم المعامل",
                            "Syntax error: Expected parameter name");
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
                errorBilingual(
                    "خطأ نحوي: توقعت ')' بعد معاملات العامل",
                    "Syntax error: Expected ')' after operator parameters");
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
                errorBilingual(
                    "❌ صيغة '->' لنوع الإرجاع أُزيلت.\n"
                    "   ✅ الصحيح: عامل +(آخر: نوع) نوع_الإرجاع\n"
                    "   ❌ الخطأ:  عامل +(آخر: نوع) -> نوع_الإرجاع",
                    "❌ Arrow '->' return type syntax has been removed.\n"
                    "   ✅ Correct: عامل +(other: type) return_type\n"
                    "   ❌ Wrong:   عامل +(other: type) -> return_type");
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
