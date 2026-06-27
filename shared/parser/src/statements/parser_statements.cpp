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

        // ======================================================================
        // (AR) تحليل الجمل / (EN) Statement Parsing
        // ======================================================================

        /**
         * @brief (AR) يحلل جملة if: إذا (شرط) { جسم } وإلا { جسم_آخر }.
         *        (EN) Parses if statement: if (condition) { body } else { elsebody }.
         */
        StmtPtr ParserCore::parseIfStmt()
        {
            // Parse condition
            // (AR) تحليل الشرط
            consume(TT::PAREN_LEFT,
                    "(AR) خطأ نحوي: بعد 'إذا' يجب أن يأتي قوس مفتوح '(' للشرط.\n"
                    "مثال: إذا (شرط) ... نهاية\n"
                    "(EN) Syntax error: After 'if' expected '(' for condition.\n"
                    "Example: if (condition) ... end");
            auto condition = parseExpression();
            consume(TT::PAREN_RIGHT,
                    "(AR) خطأ نحوي: بعد شرط 'إذا' يجب أن يأتي قوس مغلق ')'.\n"
                    "مثال: إذا (x > 5) ... نهاية\n"
                    "(EN) Syntax error: After 'if' condition expected ')'.\n"
                    "Example: if (x > 5) ... end");

            // Parse then branch - directly as block (spec 04_syntax.md)
            // (AR) تحليل فرع then - مباشرة ككتلة
            bool thenClosedByEnd = false;
            auto thenBranch = parseBlockStmt(&thenClosedByEnd);

            // (AR) تحقق: إذا أُغلقت الكتلة بـ 'نهاية'، فالجملة مكتملة — لا نبحث عن وإلا/وإلا_إذا
            //      لأن وإلا/وإلا_إذا التي تليها تنتمي لسلسلة if خارجية.
            // (EN) Check: if block was closed by 'نهاية' (KEYWORD_END), the if statement
            //      is complete — don't look for else/else_if because those belong to an outer chain.
            //      Only look for else/else_if if parseBlockStmt() stopped WITHOUT consuming 'نهاية'
            //      (i.e., it encountered else/else_if directly as the block terminator).

            // Parse optional else branch
            // (AR) تحليل فرع else الاختياري
            StmtPtr elseBranch = nullptr;
            if (!thenClosedByEnd && match(TT::KEYWORD_ELSE_IF))
            {
                // else-if as single keyword (وإلا_إذا)
                // (AR) والا_اذا ككلمة واحدة
                elseBranch = parseIfStmt(); // Recursive for else-if
            }
            else if (!thenClosedByEnd && match(TT::KEYWORD_ELSE))
            {
                // Check for else-if as two separate keywords (وإلا إذا) ON THE SAME LINE
                // (AR) التحقق من والا اذا ككلمتين منفصلتين على نفس السطر فقط
                // If إذا is on a DIFFERENT line than وإلا, treat as nested if inside else block
                // (AR) إذا كانت إذا على سطر مختلف عن وإلا، تعاملها كـ إذا متداخلة داخل كتلة وإلا
                if (check(TT::KEYWORD_IF) &&
                    peek().getPosition().line == previous().getPosition().line)
                {
                    // Same line: "وإلا إذا" → else-if chain (single نهاية for the chain)
                    advance();                  // consume 'if'
                    elseBranch = parseIfStmt(); // Recursive for else-if
                }
                else
                {
                    elseBranch = parseBlockStmt(); // Regular else block (may contain nested if)
                }
            }

            // Create if statement node
            // (AR) إنشاء عقدة جملة If
            return std::make_unique<IfStmt>(
                std::move(condition),
                std::move(thenBranch),
                std::move(elseBranch),
                previous().getPosition());
        }

        /**
         * @brief (AR) يحلل حلقة while: بينما (شرط) { جسم }.
         *        (EN) Parses while loop: while (condition) { body }.
         */
        StmtPtr ParserCore::parseWhileStmt()
        {
            // Parse condition
            // (AR) تحليل الشرط
            consume(TT::PAREN_LEFT,
                    "(AR) خطأ نحوي: بعد 'بينما' يجب أن يأتي قوس مفتوح '(' للشرط.\n"
                    "مثال: بينما (شرط) ... نهاية\n"
                    "(EN) Syntax error: After 'while' expected '(' for condition.\n"
                    "Example: while (condition) ... end");
            auto condition = parseExpression();
            consume(TT::PAREN_RIGHT,
                    "(AR) خطأ نحوي: بعد شرط 'بينما' يجب أن يأتي قوس مغلق ')'.\n"
                    "مثال: بينما (i < 10) ... نهاية\n"
                    "(EN) Syntax error: After 'while' condition expected ')'.\n"
                    "Example: while (i < 10) ... end");

            // Parse body - directly as block (spec 04_syntax.md)
            // (AR) تحليل الجسم - مباشرة ككتلة
            auto body = parseBlockStmt();

            // Create while statement node
            // (AR) إنشاء عقدة جملة While
            return std::make_unique<WhileStmt>(
                std::move(condition),
                std::move(body),
                previous().getPosition());
        }

        /**
         * @brief (AR) يحلل حلقة for: لكل عنصر في مجموعة { جسم }.
         *        (EN) Parses for loop: for element in collection { body }.
         */
        StmtPtr ParserCore::parseForStmt()
        {
            // (AR) صيغة الأقواس أُزيلت — لكل x في مجموعة (بدون أقواس)
            // (EN) Parenthesized form removed — for x in collection (no parens)
            if (check(TT::PAREN_LEFT))
            {
                errorBilingual(
                    "خطأ نحوي: صيغة الأقواس أُزيلت من 'لكل'. استخدم بدون أقواس.\n"
                    "💡 مثال: لكل عنصر في مجموعة",
                    "Syntax error: Parenthesized form removed from 'for'. Use without parens.\n"
                    "💡 Example: for item in collection");
            }

            // (AR) تحليل متغير الحلقة — يسمح بالمعرّفات وكلمات الأنواع والكلمات المفتاحية كأسماء
            // (EN) Loop variable: accept identifiers, type keywords, and keywords-as-names
            Token var(TT::IDENTIFIER, "", Lexer::Position());
            if (check(TT::IDENTIFIER))
            {
                var = current_;
                advance();
            }
            else if (isTypeToken(current_.getType()) || isTokenUsableAsName(current_.getType()))
            {
                // (AR) قبول كلمة النوع أو المفتاحية كاسم متغير حلقة (مثل: لكل خطأ في ...)
                // (EN) Accept type keyword or reserved word as loop variable (e.g., for error in ...)
                var = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                advance();
            }
            else
            {
                var = consume(TT::IDENTIFIER,
                              "(AR) توقع اسم متغير الحلقة. (EN) Expected loop variable name.");
            }

            // (AR) دعم تفكيك المتغيرات: لكل فهرس، عنصر في مجموعة
            // (EN) Support destructuring: for index, value in collection
            std::string valueVariable = "";
            if (matchComma())
            {
                Token valVar(TT::IDENTIFIER, "", Lexer::Position());
                if (check(TT::IDENTIFIER))
                {
                    valVar = current_;
                    advance();
                }
                else if (isTypeToken(current_.getType()) || isTokenUsableAsName(current_.getType()))
                {
                    valVar = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                    advance();
                }
                else
                {
                    valVar = consume(TT::IDENTIFIER,
                                     "(AR) توقع اسم المتغير الثاني. (EN) Expected second variable name.");
                }
                valueVariable = valVar.getValue();
            }

            // Expect 'in' keyword (Arabic: في)
            // (AR) توقع كلمة 'في'
            // (AR) دعم صيغتين: لكل x في مجموعة  |  لكل x من بداية الى نهاية
            // (EN) Two forms: for x in collection  |  for x from start to end
            ExprPtr collection;
            if (match(TT::KEYWORD_IN))
            {
                // (AR) الصيغة العادية: لكل x في مجموعة
                collection = parseExpression();
            }
            else if (match(TT::KEYWORD_FROM))
            {
                // (AR) صيغة النطاق: لكل x من بداية الى نهاية
                // (EN) Range form: for x from start to end
                auto startExpr = parseExpression();
                // (AR) توقع كلمة 'الى' أو 'إلى'
                if (checkContextual(TT::KEYWORD_TO))
                {
                    advance(); // consume الى
                }
                else
                {
                    errorExpectedToken("كلمة 'الى'", "keyword 'الى' (to)", "في حلقة من..الى", "in from..to loop");
                }
                auto endExpr = parseExpression();
                // (AR) إنشاء تعبير نطاق: بداية..نهاية
                collection = std::make_unique<RangeExpr>(
                    std::move(startExpr), std::move(endExpr), previous().getPosition());
            }
            else
            {
                errorExpectedToken("كلمة 'في' أو 'من'", "keyword 'في' or 'من'", "في حلقة for", "in for loop");
                collection = nullptr;
            }

            // (AR) لا أقواس — انتقل مباشرة للجسم

            // Parse body - directly as block (spec 04_syntax.md)
            // (AR) تحليل الجسم - مباشرة ككتلة
            auto body = parseBlockStmt();

            // Create for-range statement node
            // (AR) إنشاء عقدة جملة For-Range
            return std::make_unique<ForRangeStmt>(
                var.getValue(),
                std::move(collection),
                std::move(body),
                valueVariable,
                var.getPosition());
        }

        /**
         * @brief (AR) يحلل جملة return: أرجع قيمة;
         *        (EN) Parses return statement: return value;
         */
        StmtPtr ParserCore::parseReturnStmt()
        {
            Token keyword = previous();

            // Optional return value
            // (AR) قيمة الإرجاع الاختيارية
            ExprPtr value = nullptr;

            // Parse return value if present (not semicolon, end keyword, or newline)
            // (AR) تحليل قيمة الإرجاع إذا كانت موجودة (ليست فاصلة منقوطة أو نهاية أو سطر جديد)
            // (AR) إصلاح المشكلة 10: السماح بـ "ارجع" بدون قيمة قبل "نهاية" أو "وإلا"
            if (!checkSemicolon() &&
                !check(TT::KEYWORD_END) && !check(TT::KEYWORD_ELSE) &&
                !check(TT::KEYWORD_ELSE_IF) && !check(TT::KEYWORD_CATCH) &&
                !check(TT::KEYWORD_FINALLY) && !isAtEnd())
            {
                value = parseExpression();
            }

            // Semicolon is optional after return statement
            // (AR) الفاصلة المنقوطة اختيارية بعد جملة return
            matchSemicolon();

            // Create return statement node
            // (AR) إنشاء عقدة جملة Return
            return std::make_unique<ReturnStmt>(
                std::move(value),
                keyword.getPosition());
        }

        /**
         * @brief (AR) يحلل جملة yield (للدوال المولّدة).
         *        (EN) Parses yield statement (for generator functions).
         *
         * Grammar:
         *   yield_stmt → "yield" [ "from" ]? expression? ";"
         *
         * Supports:
         *   - yield expr         : yields a single value
         *   - yield from iterable: delegates to another generator
         *   - yield              : yields None
         *
         * Examples:
         *   yield 42;
         *   yield x * 2;
         *   yield from other_generator();
         *   اعطِ 100؛
         */
        StmtPtr ParserCore::parseYieldStmt()
        {
            // (AR) توقع yield قد استُهلكت بالفعل
            // (EN) Expect yield already consumed
            Token yieldToken = previous();

            // (AR) التحقق من 'yield from' (حيث 'from' معرّف وليس كلمة مفتاحية)
            // (EN) Check for 'yield from' (where 'from' is identifier, not keyword)
            bool isYieldFrom = false;
            if (check(TT::IDENTIFIER) && peek().getValue() == "from")
            {
                advance(); // consume 'from' identifier
                isYieldFrom = true;
            }

            // (AR) تحليل القيمة الاختيارية
            // (EN) Parse optional value
            ExprPtr value = nullptr;
            if (!checkSemicolon() && !check(TT::BRACE_RIGHT) && !check(TT::NEWLINE) && !isAtEnd())
            {
                value = parseExpression();
            }

            // (AR) الفاصلة المنقوطة اختيارية (للتوافق مع النمط العربي)
            // (EN) Semicolon is optional (for Arabic style compatibility)
            matchSemicolon();

            // (AR) إنشاء عقدة جملة Yield
            // (EN) Create yield statement node
            return std::make_unique<YieldStmt>(
                std::move(value),
                isYieldFrom,
                yieldToken.getPosition());
        }

        /**
         * @brief (AR) يحلل جملة باستخدام (مدير السياق).
         *        (EN) Parses with statement (context manager).
         *
         * Syntax:
         *   باستخدام expr كـ alias
         *       body
         *   نهاية_استخدام
         *
         * OR:
         *   with expr as alias
         *       body
         *   end_with
         */
        StmtPtr ParserCore::parseWithStmt()
        {
            // (AR) توقع 'باستخدام' قد استُهلكت بالفعل
            // (EN) Expect 'with' already consumed
            Token withToken = previous();

            // (AR) تحليل تعبير المورد
            // (EN) Parse resource expression
            ExprPtr resource = parseExpression();
            if (!resource)
            {
                error("(AR) توقع تعبير بعد 'باستخدام'. (EN) Expected expression after 'with'.");
                return nullptr;
            }

            // (AR) تحليل الاسم المستعار (اختياري) بعد 'كـ'
            // (EN) Parse optional alias after 'as'
            std::string alias;
            if (match(TT::KEYWORD_AS))
            {
                Token aliasToken = consume(TT::IDENTIFIER,
                                           "(AR) توقع اسم متغير بعد 'كـ'. (EN) Expected variable name after 'as'.");
                alias = aliasToken.getValue();
            }

            // (AR) تحليل جسم كتلة الاستخدام
            // (EN) Parse body of with block
            std::vector<StmtPtr> bodyStatements;

            while (!check(TT::KEYWORD_END) && !check(TT::KEYWORD_END_WITH) &&
                   !isAtEnd())
            {
                // (AR) رسالة خطأ إذا استُخدمت 'نهاية_استخدام'
                if (checkContextual(TT::KEYWORD_END_WITH))
                {
                    errorBilingual(
                        "خطأ نحوي: 'نهاية_استخدام' أُزيلت. استخدم 'نهاية' بدلاً منها.",
                        "Syntax error: 'نهاية_استخدام' removed. Use 'نهاية' instead.");
                    advance();
                    break;
                }
                StmtPtr stmt = parseStatement();
                if (stmt)
                {
                    bodyStatements.push_back(std::move(stmt));
                }
            }

            // (AR) استهلاك 'نهاية'
            // (EN) Consume 'end'
            if (!match(TT::KEYWORD_END) && !match(TT::KEYWORD_END_WITH))
            {
                error("(AR) توقع 'نهاية' لإنهاء كتلة الاستخدام. "
                      "(EN) Expected 'نهاية' to close with block.");
                return nullptr;
            }

            // (AR) إنشاء كتلة الجسم
            // (EN) Create body block
            StmtPtr body = std::make_unique<BlockStmt>(
                std::move(bodyStatements),
                withToken.getPosition());

            // (AR) إنشاء عقدة جملة With
            // (EN) Create with statement node
            return std::make_unique<WithStmt>(
                std::move(resource),
                alias,
                std::move(body),
                withToken.getPosition());
        }

        // ============================================================================
        // parseDeferStmt - تحليل جملة التأجيل (أجّل)
        // ============================================================================
        /**
         * @brief (AR) يحلل جملة أجّل — تنظيف مضمون عند خروج الدالة
         *        (EN) Parses defer statement — guaranteed cleanup on function exit
         *
         * (AR) الصيغ المدعومة:
         *   أجّل تعبير            # جملة واحدة مؤجلة
         *   أجّل                   # كتلة مؤجلة
         *       جملة1
         *       جملة2
         *   نهاية
         *
         * (EN) Supported forms:
         *   defer expression        # single deferred statement
         *   defer                   # deferred block
         *       statement1
         *       statement2
         *   end
         */
        StmtPtr ParserCore::parseDeferStmt()
        {
            Token keyword = previous();

            // (AR) تحقق: هل هي كتلة (نهاية) أم جملة واحدة؟
            //      إذا كان الرمز التالي هو نهاية سطر أو سطر جديد، فهي كتلة
            //      وإلا فهي جملة واحدة
            // (EN) Check: block (end) or single statement?
            //      If next token is on a new line or is a block start, treat as block

            // (AR) استراتيجية: إذا أتى بعد أجّل مباشرة نهاية أو إذا أو بينما... فهي كتلة
            //      وإلا نقرأ جملة واحدة
            // (EN) Strategy: if what follows defer looks like a block, parse block until نهاية

            // (AR) الحالة 1: كتلة — إذا كان السطر التالي يبدأ بشيء ليس تعبيراً على نفس السطر
            //      نستخدم خدعة: إذا الرمز الحالي على سطر مختلف عن أجّل، فهي كتلة
            // (EN) Case 1: block — if current token is on a different line, it's a block

            bool isBlock = false;

            // (AR) إذا كان الرمز الحالي هو نهاية مباشرة — خطأ (كتلة فارغة)
            if (check(TT::KEYWORD_END))
            {
                // (AR) أجّل نهاية — كتلة فارغة، نقبلها لكن ننبه
                advance(); // consume نهاية
                auto emptyBlock = std::make_unique<BlockStmt>(
                    std::vector<StmtPtr>{}, keyword.getPosition());
                return std::make_unique<DeferStmt>(std::move(emptyBlock), keyword.getPosition());
            }

            // (AR) إذا كانت بداية السطر مختلفة عن سطر أجّل، فهي كتلة
            // (EN) If current token starts on a different line than defer, it's a block
            if (current_.getPosition().line > keyword.getPosition().line)
            {
                isBlock = true;
            }

            if (isBlock)
            {
                // (AR) تحليل كتلة حتى نهاية
                // (EN) Parse block until end
                std::vector<StmtPtr> bodyStatements;

                while (!check(TT::KEYWORD_END) && !isAtEnd())
                {
                    StmtPtr stmt = parseStatement();
                    if (stmt)
                    {
                        bodyStatements.push_back(std::move(stmt));
                    }
                }

                if (!match(TT::KEYWORD_END))
                {
                    errorBilingual(
                        "خطأ نحوي: توقعت 'نهاية' لإنهاء كتلة أجّل.",
                        "Syntax error: Expected 'نهاية' to close defer block.");
                    return nullptr;
                }

                auto body = std::make_unique<BlockStmt>(
                    std::move(bodyStatements), keyword.getPosition());
                return std::make_unique<DeferStmt>(std::move(body), keyword.getPosition());
            }
            else
            {
                // (AR) جملة واحدة على نفس السطر
                // (EN) Single statement on the same line
                StmtPtr stmt = parseStatement();
                if (!stmt)
                {
                    errorBilingual(
                        "خطأ نحوي: توقعت جملة بعد 'أجّل'.",
                        "Syntax error: Expected statement after 'defer'.");
                    return nullptr;
                }
                return std::make_unique<DeferStmt>(std::move(stmt), keyword.getPosition());
            }
        }

        // =========================================================================
        // (AR) تحليل جملة أطلق (goroutine) / (EN) Parse Go Statement
        // =========================================================================

        /**
         * @brief (AR) يحلل جملة أطلق: أطلق تعبير  أو  أطلق ... نهاية
         *        (EN) Parses go statement: أطلق expr  or  أطلق ... نهاية
         *
         * (AR) الصيغ المدعومة:
         *   1. أطلق استدعاء_دالة()     — إطلاق دالة في goroutine
         *   2. أطلق لامدا() => تعبير   — إطلاق لامدا
         *   3. أطلق                    — كتلة كود حتى نهاية
         *       جملة1
         *       جملة2
         *   نهاية
         *
         * (EN) Supported forms:
         *   1. أطلق function_call()    — launch function in goroutine
         *   2. أطلق lambda() => expr   — launch lambda
         *   3. أطلق                    — code block until نهاية
         *       stmt1
         *       stmt2
         *   نهاية
         */
        StmtPtr ParserCore::parseGoStmt()
        {
            Token keyword = previous();

            // (AR) تحقق: هل هي كتلة (السطر التالي مختلف عن سطر أطلق)؟
            // (EN) Check: is it a block (next token on different line)?
            if (check(TT::KEYWORD_END))
            {
                // (AR) أطلق نهاية — كتلة فارغة
                advance(); // consume نهاية
                auto emptyBlock = std::make_unique<BlockStmt>(
                    std::vector<StmtPtr>{}, keyword.getPosition());
                return std::make_unique<GoStmt>(std::move(emptyBlock), keyword.getPosition());
            }

            // (AR) إذا الرمز التالي على سطر مختلف، فهي كتلة أطلق...نهاية
            // (EN) If next token is on a different line, it's a go...end block
            if (current_.getPosition().line > keyword.getPosition().line)
            {
                std::vector<StmtPtr> bodyStatements;
                while (!check(TT::KEYWORD_END) && !isAtEnd())
                {
                    // (AR) نستخدم parseDeclaration بدلاً من parseStatement
                    //      لتمكين تعريف المتغيرات (متغير/ثابت) داخل كتلة أطلق
                    // (EN) Use parseDeclaration instead of parseStatement
                    //      to enable variable declarations (var/const) inside go block
                    StmtPtr stmt = parseDeclaration();
                    if (stmt)
                    {
                        bodyStatements.push_back(std::move(stmt));
                    }
                }

                if (!match(TT::KEYWORD_END))
                {
                    errorBilingual(
                        "خطأ نحوي: توقعت 'نهاية' لإنهاء كتلة أطلق.",
                        "Syntax error: Expected 'نهاية' to close go block.");
                    return nullptr;
                }

                auto body = std::make_unique<BlockStmt>(
                    std::move(bodyStatements), keyword.getPosition());
                return std::make_unique<GoStmt>(std::move(body), keyword.getPosition());
            }
            else
            {
                // (AR) جملة واحدة على نفس السطر — نتوقع تعبير (استدعاء أو لامدا)
                // (EN) Single statement on same line — expect expression (call or lambda)
                ExprPtr expr = parseExpression();
                if (!expr)
                {
                    errorBilingual(
                        "خطأ نحوي: توقعت تعبيراً (استدعاء دالة أو لامدا) بعد 'أطلق'.",
                        "Syntax error: Expected expression (function call or lambda) after 'go'.");
                    return nullptr;
                }
                return std::make_unique<GoStmt>(std::move(expr), keyword.getPosition());
            }
        }

        // =========================================================================
        // (AR) تحليل جملة اختر (select) / (EN) Parse Select Statement
        // =========================================================================

        /**
         * @brief (AR) يحلل جملة اختر: اختر عندما... افتراضي:... نهاية
         *        (EN) Parses select statement: اختر عندما... افتراضي:... نهاية
         *
         * (AR) الصيغة:
         *   اختر
         *       عندما تعبير_قناة:
         *           جمل
         *       عندما تعبير_قناة:
         *           جمل
         *       افتراضي:
         *           جمل
         *   نهاية
         *
         * (EN) Syntax:
         *   اختر
         *       عندما channel_expr:
         *           stmts
         *       عندما channel_expr:
         *           stmts
         *       افتراضي:
         *           stmts
         *   نهاية
         */
        StmtPtr ParserCore::parseSelectStmt()
        {
            Token keyword = previous();

            std::vector<std::unique_ptr<SelectCase>> cases;
            std::vector<StmtPtr> defaultBody;

            // (AR) تحليل حالات عندما وافتراضي حتى نهاية
            // (EN) Parse when cases and default until end
            while (!check(TT::KEYWORD_END) && !isAtEnd())
            {
                // (AR) تحقق من حالة عندما
                // (EN) Check for عندما (when) case
                if (match(TT::KEYWORD_WHEN))
                {
                    // (AR) تحليل تعبير القناة
                    // (EN) Parse channel expression
                    ExprPtr channelExpr = parseExpression();

                    // (AR) تحقق من النقطتين (:) بعد التعبير
                    // (EN) Check for colon after expression
                    if (!match(TT::COLON))
                    {
                        errorBilingual(
                            "خطأ نحوي: توقعت ':' بعد تعبير القناة في اختر.",
                            "Syntax error: Expected ':' after channel expression in select.");
                        return nullptr;
                    }

                    // (AR) تحليل جسم الحالة حتى عندما/افتراضي/نهاية التالية
                    // (EN) Parse case body until next when/default/end
                    std::vector<StmtPtr> caseBody;
                    while (!check(TT::KEYWORD_WHEN) &&
                           !check(TT::KEYWORD_DEFAULT) &&
                           !check(TT::KEYWORD_END) &&
                           !isAtEnd())
                    {
                        StmtPtr stmt = parseStatement();
                        if (stmt)
                        {
                            caseBody.push_back(std::move(stmt));
                        }
                    }

                    cases.push_back(std::make_unique<SelectCase>(
                        std::move(channelExpr), std::move(caseBody),
                        "", keyword.getPosition()));
                }
                // (AR) تحقق من الحالة الافتراضية
                // (EN) Check for default case
                else if (match(TT::KEYWORD_DEFAULT))
                {
                    // (AR) تحقق من النقطتين (:)
                    if (!match(TT::COLON))
                    {
                        errorBilingual(
                            "خطأ نحوي: توقعت ':' بعد 'افتراضي' في اختر.",
                            "Syntax error: Expected ':' after 'default' in select.");
                        return nullptr;
                    }

                    // (AR) تحليل الجسم الافتراضي
                    while (!check(TT::KEYWORD_END) && !isAtEnd())
                    {
                        StmtPtr stmt = parseStatement();
                        if (stmt)
                        {
                            defaultBody.push_back(std::move(stmt));
                        }
                    }
                }
                else
                {
                    errorBilingual(
                        "خطأ نحوي: توقعت 'عندما' أو 'افتراضي' داخل جملة اختر.",
                        "Syntax error: Expected 'عندما' or 'افتراضي' inside select statement.");
                    advance(); // تخطي الرمز غير المتوقع
                }
            }

            if (!match(TT::KEYWORD_END))
            {
                errorBilingual(
                    "خطأ نحوي: توقعت 'نهاية' لإنهاء جملة اختر.",
                    "Syntax error: Expected 'نهاية' to close select statement.");
                return nullptr;
            }

            return std::make_unique<SelectStmt>(
                std::move(cases), std::move(defaultBody), keyword.getPosition());
        }

        /**
         * @brief (AR) يحلل جملة break: اخرج;
         *        (EN) Parses break statement: break;
         */
        StmtPtr ParserCore::parseBreakStmt()
        {
            Token keyword = previous();

            // Semicolon is optional after break statement (newline-terminated language)
            // (AR) الفاصلة المنقوطة اختيارية بعد جملة break
            matchSemicolon();

            // Create break statement node
            // (AR) إنشاء عقدة جملة Break
            return std::make_unique<BreakStmt>(keyword.getPosition());
        }

        /**
         * @brief (AR) يحلل جملة continue: استمر;
         *        (EN) Parses continue statement: continue;
         */
        StmtPtr ParserCore::parseContinueStmt()
        {
            Token keyword = previous();

            // Semicolon is optional after continue statement (newline-terminated language)
            // (AR) الفاصلة المنقوطة اختيارية بعد جملة continue
            matchSemicolon();

            // Create continue statement node
            // (AR) إنشاء عقدة جملة Continue
            return std::make_unique<ContinueStmt>(keyword.getPosition());
        }

        /**
         * @brief (AR) يحلل كتلة من الجمل: { جملة1; جملة2; }.
         *        (EN) Parses block of statements: { stmt1; stmt2; }.
         */
        StmtPtr ParserCore::parseBlockStmt(bool *closedByEnd)
        {
            StmtList statements;

            // Parse statements until 'نهاية' keyword (spec 04_syntax.md)
            // (AR) تحليل الجمل حتى كلمة 'نهاية'
            // Also stop at else/else-if keywords (for if statements)
            // (AR) أيضاً التوقف عند كلمات والا/والا_اذا (لجمل if)
            // Also stop at catch/finally keywords (for try statements)
            // (AR) أيضاً التوقف عند كلمات امسك/أخيراً (لجمل حاول)
            while (!check(TT::KEYWORD_END) &&
                   !check(TT::KEYWORD_ELSE) &&
                   !check(TT::KEYWORD_ELSE_IF) &&
                   !check(TT::KEYWORD_CATCH) &&
                   !check(TT::KEYWORD_FINALLY) &&
                   !isAtEnd())
            {
                auto stmt = parseDeclaration();
                if (stmt)
                {
                    statements.push_back(std::move(stmt));
                }
            }

            // Don't require 'نهاية' if we stopped at else/else-if
            // (AR) لا نطلب 'نهاية' إذا توقفنا عند والا/والا_اذا
            if (check(TT::KEYWORD_ELSE) || check(TT::KEYWORD_ELSE_IF))
            {
                // This is the then-branch of an if statement, don't consume 'نهاية'
                // (AR) هذا هو فرع then لجملة if، لا نستهلك 'نهاية'
                if (closedByEnd)
                    *closedByEnd = false;
                return std::make_unique<BlockStmt>(
                    std::move(statements),
                    current_.getPosition());
            }

            // Don't require 'نهاية' if we stopped at catch/finally (try-catch blocks)
            // (AR) لا نطلب 'نهاية' إذا توقفنا عند امسك/أخيراً (كتل حاول-امسك)
            if (check(TT::KEYWORD_CATCH) || check(TT::KEYWORD_FINALLY))
            {
                if (closedByEnd)
                    *closedByEnd = false;
                return std::make_unique<BlockStmt>(
                    std::move(statements),
                    current_.getPosition());
            }

            // Block ended with 'نهاية' — set closedByEnd flag
            if (closedByEnd)
                *closedByEnd = true;

            if (isAtEnd() && !check(TT::KEYWORD_END))
            {
                error(
                    "(AR) خطأ نحوي: الكتلة غير مغلقة!\n"
                    "لم يتم العثور على كلمة 'نهاية' لإغلاق الكتلة.\n"
                    "في لغة ص، كل كتلة (دالة، إذا، بينما، لكل، ...) يجب أن تنتهي بـ 'نهاية'.\n\n"
                    "مثال صحيح:\n"
                    "  دالة مثال() \n"
                    "    # هنا جسم الدالة\n"
                    "  نهاية  # <-- لا تنسى هذه!\n\n"
                    "(EN) Syntax error: Unclosed block!\n"
                    "Missing 'نهاية' (end) keyword to close the block.\n"
                    "In Sad language, every block (function, if, while, for, ...) must end with 'نهاية'.\n\n"
                    "Correct example:\n"
                    "  function example()\n"
                    "    # function body here\n"
                    "  نهاية  # <-- Don't forget this!\n");
            }

            consume(TT::KEYWORD_END,
                    "(AR) خطأ نحوي: توقع 'نهاية' لإغلاق الكتلة.\n"
                    "(EN) Syntax error: Expected 'نهاية' (end) to close block.");

            // Create block statement node
            // (AR) إنشاء عقدة كتلة الجمل
            return std::make_unique<BlockStmt>(
                std::move(statements),
                previous().getPosition());
        }

        /**
         * @brief (AR) يحلل جملة try-catch: حاول { } اصطد { }.
         *        (EN) Parses try-catch statement: try { } catch { }.
         */
        StmtPtr ParserCore::parseTryStmt()
        {
            // Parse try block using Arabic syntax
            // (AR) تحليل كتلة try باستخدام الصيغة العربية
            auto tryBlock = parseBlockStmt();

            // Parse catch clauses
            // (AR) تحليل بنود catch
            std::vector<CatchClause> catchClauses;
            bool lastBlockClosedByEnd = false; // (AR) تتبع إذا آخر كتلة أُغلقت بـ'نهاية'
            while (match(TT::KEYWORD_CATCH))
            {
                // ─────────────────────────────────────────────────────────────────
                // (AR) الأقواس مُزالة: استخدم `امسك خطأ` بدون أقواس
                // (EN) Parentheses removed: use `امسك error` without parens
                // ─────────────────────────────────────────────────────────────────
                bool hasCatchParen = false;
                if (check(TT::PAREN_LEFT))
                {
                    error(
                        "(AR) ❌ `امسك (خطأ)` بأقواس لم تعد مدعومة. استخدم `امسك خطأ` بدون أقواس.\n"
                        "(EN) `امسك (error)` with parentheses is no longer supported. Use `امسك error` without parens.");
                    hasCatchParen = true;
                    advance(); // skip '(' for recovery
                }

                // (AR) قراءة نوع الاستثناء (اختياري) ثم اسم المتغير
                // (EN) Read optional exception type then variable name
                // (AR) الصيغة: امسك (متغير) أو امسك (نوع_الاستثناء متغير)
                // (EN) Syntax: catch (var) or catch (ExceptionType var)
                Types::SadTypeKind exceptionType = Types::SadTypeKind::Unknown;
                std::string exceptionTypeName = "";

                // (AR) دعم الكلمات المفتاحية كأسماء استثناء (مثل: امسك خطأ)
                // (EN) Support keywords as exception variable names (e.g., catch error)
                Token firstToken(TT::IDENTIFIER, "", Lexer::Position());
                if (check(TT::IDENTIFIER))
                {
                    firstToken = current_;
                    advance();
                }
                else if (isTokenUsableAsName(current_.getType()) ||
                         current_.getType() == TT::LITERAL_FALSE || current_.getType() == TT::LITERAL_TRUE ||
                         current_.getType() == TT::LITERAL_NULL)
                {
                    // (AR) دعم خطأ/صحيح/لاشيء كأسماء استثناء: امسك (خطأ)
                    firstToken = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                    advance();
                }
                else
                {
                    firstToken = consume(TT::IDENTIFIER,
                                         "(AR) توقع اسم متغير الاستثناء. (EN) Expected exception variable name.");
                }

                // (AR) إذا كان هناك معرّف آخر قبل ')' فالأول هو النوع والثاني هو المتغير
                // (EN) If there's another identifier before ')' then first is type, second is variable
                // (AR) هذا يُفعّل فقط مع الأقواس: امسك (نوع_خطأ متغير) — بدون أقواس نأخذ رمزاً واحداً فقط
                // (EN) Only enabled with parens: catch (ExType var) — without parens take single token
                Token exceptionVar = firstToken;
                if (hasCatchParen && (check(TT::IDENTIFIER) || isTokenUsableAsName(current_.getType())))
                {
                    // (AR) الرمز الأول هو اسم نوع الاستثناء المخصص
                    // (EN) First token is custom exception type name
                    exceptionTypeName = firstToken.getValue();
                    exceptionType = Types::SadTypeKind::Class;
                    if (check(TT::IDENTIFIER))
                    {
                        exceptionVar = current_;
                        advance();
                    }
                    else
                    {
                        exceptionVar = Token(TT::IDENTIFIER, current_.getValue(), current_.getPosition());
                        advance();
                    }
                }

                if (hasCatchParen)
                {
                    consume(TT::PAREN_RIGHT,
                            "(AR) توقع ')' بعد متغير الاستثناء. (EN) Expected ')' after exception variable.");
                }

                // Parse catch body using Arabic syntax
                // (AR) تحليل جسم catch باستخدام الصيغة العربية
                // (AR) تتبع إذا أُغلقت الكتلة بـ'نهاية' — لمنع سرقة 'امسك' الخارجي في حالة التداخل
                // (EN) Track if block closed by 'end' — prevents stealing outer 'catch' in nested try
                bool catchClosedByEnd = false;
                auto catchBody = parseBlockStmt(&catchClosedByEnd);

                catchClauses.push_back(CatchClause(
                    exceptionVar.getValue(),
                    exceptionType, // (AR) استخدام النوع المُحلل / (EN) Use parsed type
                    std::move(catchBody),
                    exceptionTypeName));

                // (AR) إذا أُغلقت كتلة catch بـ'نهاية'، فهذا يعني انتهاء بنية try-catch بالكامل
                //      لا نبحث عن المزيد من بنود catch — لأن 'نهاية' تُنهي الكتلة
                //      هذا يمنع parseTryStmt الداخلي من سرقة 'امسك' الخارجي
                // (EN) If catch block was closed by 'end', the try-catch construct is done
                //      Don't look for more catch clauses — 'end' terminates the block
                //      This prevents inner parseTryStmt from stealing outer 'catch'
                if (catchClosedByEnd)
                {
                    lastBlockClosedByEnd = true;
                    break;
                }
            }

            // Parse optional finally block using Arabic syntax
            // (AR) تحليل كتلة finally الاختيارية باستخدام الصيغة العربية
            // (AR) فقط إذا لم تُغلق الكتلة السابقة بـ'نهاية' — لأن 'نهاية' تُنهي البنية بالكامل
            // (EN) Only if the last block wasn't closed by 'end' — 'end' terminates the entire construct
            StmtPtr finallyBlock = nullptr;
            if (!lastBlockClosedByEnd && match(TT::KEYWORD_FINALLY))
            {
                finallyBlock = parseBlockStmt();
            }

            // Create try statement node
            // (AR) إنشاء عقدة جملة Try
            return std::make_unique<TryStmt>(
                std::move(tryBlock),
                std::move(catchClauses),
                std::move(finallyBlock),
                previous().getPosition());
        }

        /**
         * @brief (AR) يحلل جملة raise: ارمِ تعبير;
         *        (EN) Parses raise statement: throw expression;
         */
        StmtPtr ParserCore::parseRaiseStmt()
        {
            // Parse exception expression
            // (AR) تحليل تعبير الاستثناء
            auto exception = parseExpression();

            // (AR) الفاصلة المنقوطة اختيارية — لغة ص تعتمد على أسطر جديدة
            // (EN) Semicolons are optional — Sad language uses newlines
            matchSemicolon();

            // Create raise statement node
            // (AR) إنشاء عقدة جملة Raise
            return std::make_unique<RaiseStmt>(
                std::move(exception),
                previous().getPosition());
        }

        /**
         * @brief (AR) يحلل جملة switch-case: حالة تعبير ... نهاية.
         *        (EN) Parses switch-case statement: case expression ... end.
         *
         * Grammar / القواعد:
         *   switch_stmt → KEYWORD_CASE "(" expr ")"
         *                 (KEYWORD_WHEN expr ":" stmt)*
         *                 [KEYWORD_DEFAULT ":" stmt]
         *                 KEYWORD_END
         *
         * Syntax / النحو:
         *   حالة (<expression>)
         *       عندما <value>: <statement>
         *       [افتراضي: <statement>]
         *   نهاية
         */
        StmtPtr ParserCore::parseSwitchStmt()
        {
            // Save position for error reporting
            // (AR) حفظ الموقع للإبلاغ عن الأخطاء
            Token startToken = previous();

            // ─────────────────────────────────────────────────────────────────────
            // (AR) الأقواس مُزالة: استخدم `حالة تعبير` بدون أقواس
            // (EN) Parentheses removed: use `حالة expression` without parens
            // ─────────────────────────────────────────────────────────────────────
            if (check(TT::PAREN_LEFT))
            {
                error(
                    "(AR) ❌ `حالة (تعبير)` بأقواس لم تعد مدعومة. استخدم `حالة تعبير` بدون أقواس.\n"
                    "(EN) `حالة (expr)` with parentheses is no longer supported. Use `حالة expr` without parens.");
                advance(); // skip '(' for recovery
            }

            // Parse switch expression
            // (AR) تحليل تعبير switch
            auto expr = parseExpression();

            // (AR) إذا وُجد ')' بعد التعبير — محاولة تعافي من الصيغة القديمة
            if (check(TT::PAREN_RIGHT))
            {
                advance(); // skip ')' for recovery
            }

            // Parse case branches
            // (AR) تحليل فروع الحالات
            std::vector<CaseBranch> cases;
            StmtPtr defaultCase = nullptr;

            // Continue parsing cases until we hit default or end
            // (AR) استمر في تحليل الحالات حتى نصل إلى افتراضي أو نهاية
            while (!check(TT::KEYWORD_END) && !isAtEnd())
            {
                if (match(TT::KEYWORD_WHEN))
                {
                    // Parse case value
                    // (AR) تحليل قيمة الحالة
                    auto caseValue = parseExpression();

                    // (AR) النقطتان ':' اختيارية بعد قيمة الحالة
                    // (EN) Colon ':' is optional after case value
                    match(TT::COLON);

                    // (AR) تحليل جسم الحالة — جمل متعددة حتى عندما/افتراضي/نهاية التالية
                    // (EN) Parse case body — multiple statements until next when/default/end
                    std::vector<StmtPtr> bodyStmts;
                    while (!check(TT::KEYWORD_WHEN) && !check(TT::KEYWORD_DEFAULT) &&
                           !check(TT::KEYWORD_END) && !isAtEnd())
                    {
                        auto stmt = parseStatement();
                        if (stmt)
                        {
                            bodyStmts.push_back(std::move(stmt));
                        }
                        else
                        {
                            synchronize();
                        }
                    }

                    // (AR) تغليف الجمل في كتلة واحدة
                    // (EN) Wrap statements in a single block
                    StmtPtr caseBody;
                    if (bodyStmts.size() == 1)
                    {
                        caseBody = std::move(bodyStmts[0]);
                    }
                    else
                    {
                        caseBody = std::make_unique<BlockStmt>(std::move(bodyStmts));
                    }

                    // Add case branch
                    // (AR) إضافة فرع الحالة
                    cases.push_back({std::move(caseValue), std::move(caseBody)});
                }
                else if (match(TT::KEYWORD_DEFAULT))
                {
                    // Parse default case
                    // (AR) تحليل الحالة الافتراضية
                    // (AR) النقطتان ':' اختيارية
                    // (EN) Colon ':' is optional
                    match(TT::COLON);

                    // (AR) تحليل جسم الحالة الافتراضية — جمل متعددة
                    // (EN) Parse default body — multiple statements
                    std::vector<StmtPtr> defaultStmts;
                    while (!check(TT::KEYWORD_END) && !isAtEnd())
                    {
                        auto stmt = parseStatement();
                        if (stmt)
                        {
                            defaultStmts.push_back(std::move(stmt));
                        }
                        else
                        {
                            synchronize();
                        }
                    }

                    if (defaultStmts.size() == 1)
                    {
                        defaultCase = std::move(defaultStmts[0]);
                    }
                    else
                    {
                        defaultCase = std::make_unique<BlockStmt>(std::move(defaultStmts));
                    }

                    // Default must be last, so break
                    // (AR) الحالة الافتراضية يجب أن تكون الأخيرة، لذا اخرج
                    break;
                }
                else
                {
                    // Error: expected case or default
                    // (AR) خطأ: توقع عندما أو افتراضي
                    errorBilingual(
                        "توقع 'عندما' أو 'افتراضي' في جملة حالة.",
                        "Expected 'when' or 'default' in switch statement.");
                    return nullptr;
                }
            }

            // Consume end keyword
            // (AR) استهلك كلمة نهاية
            consume(TT::KEYWORD_END,
                    "(AR) توقع 'نهاية' لإنهاء جملة حالة. (EN) Expected 'end' to close switch statement.");

            // Create switch statement node
            // (AR) إنشاء عقدة جملة Switch
            return std::make_unique<SwitchStmt>(
                std::move(expr),
                std::move(cases),
                std::move(defaultCase),
                startToken.getPosition());
        }

        /**
         * @brief (AR) يحلل جملة تعبير: تعبير;
         *        (EN) Parses expression statement: expression;
         */
        StmtPtr ParserCore::parseExpressionStmt()
        {
            auto expr = parseExpression();

            // Check if expression parsing failed
            // (AR) تحقق من فشل تحليل التعبير
            if (!expr)
            {
                errorBilingual(
                    "خطأ نحوي: لا يمكن تحليل جملة التعبير. تأكد من صحة التعبير أو التصريح.",
                    "Syntax error: cannot parse expression statement. Make sure the expression or declaration is valid.");
                return nullptr;
            }

            // Semicolon is optional for expression statements
            // (AR) الفاصلة المنقوطة اختيارية لجمل التعبير
            matchSemicolon();

            // Create expression statement node
            // (AR) إنشاء عقدة جملة التعبير
            return std::make_unique<ExprStmt>(std::move(expr));
        }

        // ======================================================================
        // (AR) تحليل التعبيرات / (EN) Expression Parsing
        // ======================================================================

    } // namespace Parser
} // namespace Sad
