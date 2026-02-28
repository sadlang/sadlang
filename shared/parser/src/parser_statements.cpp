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
#include "class_manager.h"
#include <iostream>
#include <sstream>

namespace Sad {
namespace Parser {

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
StmtPtr ParserCore::parseIfStmt() {
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
    if (!thenClosedByEnd && match(TT::KEYWORD_ELSE_IF)) {
        // else-if as single keyword (وإلا_إذا)
        // (AR) والا_اذا ككلمة واحدة
        elseBranch = parseIfStmt(); // Recursive for else-if
    } else if (!thenClosedByEnd && match(TT::KEYWORD_ELSE)) {
        // Check for else-if as two separate keywords (وإلا إذا) ON THE SAME LINE
        // (AR) التحقق من والا اذا ككلمتين منفصلتين على نفس السطر فقط
        // If إذا is on a DIFFERENT line than وإلا, treat as nested if inside else block
        // (AR) إذا كانت إذا على سطر مختلف عن وإلا، تعاملها كـ إذا متداخلة داخل كتلة وإلا
        if (check(TT::KEYWORD_IF) && 
            peek().getPosition().line == previous().getPosition().line) {
            // Same line: "وإلا إذا" → else-if chain (single نهاية for the chain)
            advance(); // consume 'if'
            elseBranch = parseIfStmt(); // Recursive for else-if
        } else {
            elseBranch = parseBlockStmt(); // Regular else block (may contain nested if)
        }
    }
    
    // Create if statement node
    // (AR) إنشاء عقدة جملة If
    return std::make_unique<IfStmt>(
        std::move(condition),
        std::move(thenBranch),
        std::move(elseBranch),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل حلقة while: بينما (شرط) { جسم }.
 *        (EN) Parses while loop: while (condition) { body }.
 */
StmtPtr ParserCore::parseWhileStmt() {
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
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل حلقة for: لكل عنصر في مجموعة { جسم }.
 *        (EN) Parses for loop: for element in collection { body }.
 */
StmtPtr ParserCore::parseForStmt() {
    // (AR) الأقواس اختيارية: يُقبل كلا الشكلين:
    //   لكل عنصر في مجموعة   <- بدون أقواس (المفضّل)
    //   لكل (عنصر في مجموعة) <- بأقواس (للتوافق مع القديم)
    // (EN) Parentheses optional: both forms accepted
    bool hasParen = match(TT::PAREN_LEFT);

    // Parse loop variable
    // (AR) تحليل متغير الحلقة — يسمح بالمعرّفات وكلمات الأنواع (رقم، عشري، نص...)
    // (EN) Loop variable: accept identifiers and type keywords (رقم، عشري، نص...)
    bool isTypeKeyword = check(TT::TYPE_INTEGER) || check(TT::TYPE_DOUBLE) || 
                         check(TT::TYPE_STRING)  || check(TT::TYPE_BOOLEAN) ||
                         check(TT::TYPE_ARRAY)   || check(TT::TYPE_MAP)     ||
                         check(TT::TYPE_ANY);
    if (isTypeKeyword) {
        // (AR) قبول كلمة النوع كاسم متغير حلقة
        // نحرّك المؤشر ونحفظ الرمز
        Token saved = current_;
        advance();
        // نعيد تعيين var بعد الاستهلاك
        Token var = saved;
        // (AR) توقع كلمة 'في'
        if (!match(TT::KEYWORD_IN)) {
            errorExpectedToken("كلمة 'في'", "keyword 'في' (in)", "في حلقة for", "in for loop");
        }
        auto collection = parseExpression();
        if (hasParen) {
            consume(TT::PAREN_RIGHT, "(AR) توقع ')' بعد مجموعة for. (EN) Expected ')' after for collection.");
        }
        auto body = parseBlockStmt();
        return std::make_unique<ForRangeStmt>(
            var.getValue(), std::move(collection), std::move(body), "", var.getPosition());
    }
    Token var = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم متغير الحلقة. (EN) Expected loop variable name.");
    
    // Expect 'in' keyword (Arabic: في)
    // (AR) توقع كلمة 'في'
    if (!match(TT::KEYWORD_IN)) {
        errorExpectedToken("كلمة 'في'", "keyword 'في' (in)", "في حلقة for", "in for loop");
    }
    
    // Parse collection expression
    // (AR) تحليل تعبير المجموعة
    auto collection = parseExpression();
    
    // (AR) إغلاق القوس إذا فُتح
    if (hasParen) {
        consume(TT::PAREN_RIGHT, 
            "(AR) توقع ')' بعد مجموعة for. (EN) Expected ')' after for collection.");
    }
    
    // Parse body - directly as block (spec 04_syntax.md)
    // (AR) تحليل الجسم - مباشرة ككتلة
    auto body = parseBlockStmt();
    
    // Create for-range statement node
    // (AR) إنشاء عقدة جملة For-Range
    return std::make_unique<ForRangeStmt>(
        var.getValue(),
        std::move(collection),
        std::move(body),
        "",
        var.getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة return: أرجع قيمة;
 *        (EN) Parses return statement: return value;
 */
StmtPtr ParserCore::parseReturnStmt() {
    Token keyword = previous();
    
    // Optional return value
    // (AR) قيمة الإرجاع الاختيارية
    ExprPtr value = nullptr;
    
    // Parse return value if present (not semicolon, end keyword, or newline)
    // (AR) تحليل قيمة الإرجاع إذا كانت موجودة (ليست فاصلة منقوطة أو نهاية أو سطر جديد)
    // (AR) إصلاح المشكلة 10: السماح بـ "ارجع" بدون قيمة قبل "نهاية" أو "وإلا"
    if (!check(TT::SEMICOLON) && !check(TT::ARABIC_SEMICOLON) &&
        !check(TT::KEYWORD_END) && !check(TT::KEYWORD_ELSE) &&
        !check(TT::KEYWORD_ELSE_IF) && !check(TT::KEYWORD_CATCH) &&
        !check(TT::KEYWORD_FINALLY) && !isAtEnd()) {
        value = parseExpression();
    }
    
    // Semicolon is optional after return statement
    // (AR) الفاصلة المنقوطة اختيارية بعد جملة return
    if (check(TT::SEMICOLON) || check(TT::ARABIC_SEMICOLON)) {
        advance(); // consume optional semicolon
    }
    
    // Create return statement node
    // (AR) إنشاء عقدة جملة Return
    return std::make_unique<ReturnStmt>(
        std::move(value),
        keyword.getPosition()
    );
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
StmtPtr ParserCore::parseYieldStmt() {
    // (AR) توقع yield قد استُهلكت بالفعل
    // (EN) Expect yield already consumed
    Token yieldToken = previous();
    
    // (AR) التحقق من 'yield from' (حيث 'from' معرّف وليس كلمة مفتاحية)
    // (EN) Check for 'yield from' (where 'from' is identifier, not keyword)
    bool isYieldFrom = false;
    if (check(TT::IDENTIFIER) && peek().getValue() == "from") {
        advance(); // consume 'from' identifier
        isYieldFrom = true;
    }
    
    // (AR) تحليل القيمة الاختيارية
    // (EN) Parse optional value
    ExprPtr value = nullptr;
    if (!check(TT::SEMICOLON) && !check(TT::ARABIC_SEMICOLON) && !check(TT::BRACE_RIGHT) && !check(TT::NEWLINE) && !isAtEnd()) {
        value = parseExpression();
    }
    
    // (AR) الفاصلة المنقوطة اختيارية (للتوافق مع النمط العربي)
    // (EN) Semicolon is optional (for Arabic style compatibility)
    if (check(TT::SEMICOLON) || check(TT::ARABIC_SEMICOLON)) {
        advance();
    }
    
    // (AR) إنشاء عقدة جملة Yield
    // (EN) Create yield statement node
    return std::make_unique<YieldStmt>(
        std::move(value), 
        isYieldFrom, 
        yieldToken.getPosition()
    );
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
StmtPtr ParserCore::parseWithStmt() {
    // (AR) توقع 'باستخدام' قد استُهلكت بالفعل
    // (EN) Expect 'with' already consumed
    Token withToken = previous();
    
    // (AR) تحليل تعبير المورد
    // (EN) Parse resource expression
    ExprPtr resource = parseExpression();
    if (!resource) {
        error("(AR) توقع تعبير بعد 'باستخدام'. (EN) Expected expression after 'with'.");
        return nullptr;
    }
    
    // (AR) تحليل الاسم المستعار (اختياري) بعد 'كـ'
    // (EN) Parse optional alias after 'as'
    std::string alias;
    if (match(TT::KEYWORD_AS)) {
        Token aliasToken = consume(TT::IDENTIFIER,
            "(AR) توقع اسم متغير بعد 'كـ'. (EN) Expected variable name after 'as'.");
        alias = aliasToken.getValue();
    }
    
    // (AR) تحليل جسم كتلة الاستخدام
    // (EN) Parse body of with block
    std::vector<StmtPtr> bodyStatements;
    
    while (!check(TT::KEYWORD_END_WITH) && !check(TT::KEYWORD_END) && !isAtEnd()) {
        StmtPtr stmt = parseStatement();
        if (stmt) {
            bodyStatements.push_back(std::move(stmt));
        }
    }
    
    // (AR) استهلاك 'نهاية_استخدام' أو 'نهاية'
    // (EN) Consume 'end_with' or 'end'
    if (!match(TT::KEYWORD_END_WITH)) {
        if (!match(TT::KEYWORD_END)) {
            error("(AR) توقع 'نهاية_استخدام' أو 'نهاية' لإنهاء كتلة الاستخدام. "
                  "(EN) Expected 'end_with' or 'end' to close with block.");
            return nullptr;
        }
    }
    
    // (AR) إنشاء كتلة الجسم
    // (EN) Create body block
    StmtPtr body = std::make_unique<BlockStmt>(
        std::move(bodyStatements),
        withToken.getPosition()
    );
    
    // (AR) إنشاء عقدة جملة With
    // (EN) Create with statement node
    return std::make_unique<WithStmt>(
        std::move(resource),
        alias,
        std::move(body),
        withToken.getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة break: اخرج;
 *        (EN) Parses break statement: break;
 */
StmtPtr ParserCore::parseBreakStmt() {
    Token keyword = previous();
    
    // Semicolon is optional after break statement (newline-terminated language)
    // (AR) الفاصلة المنقوطة اختيارية بعد جملة break
    if (check(TT::SEMICOLON) || check(TT::ARABIC_SEMICOLON)) {
        advance(); // consume optional semicolon
    }
    
    // Create break statement node
    // (AR) إنشاء عقدة جملة Break
    return std::make_unique<BreakStmt>(keyword.getPosition());
}

/**
 * @brief (AR) يحلل جملة continue: استمر;
 *        (EN) Parses continue statement: continue;
 */
StmtPtr ParserCore::parseContinueStmt() {
    Token keyword = previous();
    
    // Semicolon is optional after continue statement (newline-terminated language)
    // (AR) الفاصلة المنقوطة اختيارية بعد جملة continue
    if (check(TT::SEMICOLON) || check(TT::ARABIC_SEMICOLON)) {
        advance(); // consume optional semicolon
    }
    
    // Create continue statement node
    // (AR) إنشاء عقدة جملة Continue
    return std::make_unique<ContinueStmt>(keyword.getPosition());
}

/**
 * @brief (AR) يحلل كتلة من الجمل: { جملة1; جملة2; }.
 *        (EN) Parses block of statements: { stmt1; stmt2; }.
 */
StmtPtr ParserCore::parseBlockStmt(bool* closedByEnd) {
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
           !isAtEnd()) {
        auto stmt = parseDeclaration();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }
    
    // Don't require 'نهاية' if we stopped at else/else-if
    // (AR) لا نطلب 'نهاية' إذا توقفنا عند والا/والا_اذا
    if (check(TT::KEYWORD_ELSE) || check(TT::KEYWORD_ELSE_IF)) {
        // This is the then-branch of an if statement, don't consume 'نهاية'
        // (AR) هذا هو فرع then لجملة if، لا نستهلك 'نهاية'
        if (closedByEnd) *closedByEnd = false;
        return std::make_unique<BlockStmt>(
            std::move(statements),
            current_.getPosition()
        );
    }
    
    // Don't require 'نهاية' if we stopped at catch/finally (try-catch blocks)
    // (AR) لا نطلب 'نهاية' إذا توقفنا عند امسك/أخيراً (كتل حاول-امسك)
    if (check(TT::KEYWORD_CATCH) || check(TT::KEYWORD_FINALLY)) {
        if (closedByEnd) *closedByEnd = false;
        return std::make_unique<BlockStmt>(
            std::move(statements),
            current_.getPosition()
        );
    }
    
    // Block ended with 'نهاية' — set closedByEnd flag
    if (closedByEnd) *closedByEnd = true;

    if (isAtEnd() && !check(TT::KEYWORD_END)) {
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
            "  نهاية  # <-- Don't forget this!\n"
        );
    }
    
    consume(TT::KEYWORD_END, 
        "(AR) خطأ نحوي: توقع 'نهاية' لإغلاق الكتلة.\n"
        "(EN) Syntax error: Expected 'نهاية' (end) to close block.");
    
    // Create block statement node
    // (AR) إنشاء عقدة كتلة الجمل
    return std::make_unique<BlockStmt>(
        std::move(statements),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة try-catch: حاول { } اصطد { }.
 *        (EN) Parses try-catch statement: try { } catch { }.
 */
StmtPtr ParserCore::parseTryStmt() {
    // Parse try block using Arabic syntax
    // (AR) تحليل كتلة try باستخدام الصيغة العربية
    auto tryBlock = parseBlockStmt();
    
    // Parse catch clauses
    // (AR) تحليل بنود catch
    std::vector<CatchClause> catchClauses;
    while (match(TT::KEYWORD_CATCH)) {
        consume(TT::PAREN_LEFT, 
            "(AR) توقع '(' بعد 'امسك'. (EN) Expected '(' after 'catch'.");
        
        
        // (AR) قراءة نوع الاستثناء (اختياري) ثم اسم المتغير
        // (EN) Read optional exception type then variable name
        // (AR) الصيغة: امسك (متغير) أو امسك (نوع_الاستثناء متغير)
        // (EN) Syntax: catch (var) or catch (ExceptionType var)
        Data::DataType exceptionType = Data::DataType::UNKNOWN;
        std::string exceptionTypeName = "";
        
        Token firstToken = consume(TT::IDENTIFIER, 
            "(AR) توقع اسم متغير الاستثناء. (EN) Expected exception variable name.");
        
        // (AR) إذا كان هناك معرّف آخر قبل ')' فالأول هو النوع والثاني هو المتغير
        // (EN) If there's another identifier before ')' then first is type, second is variable
        Token exceptionVar = firstToken;
        if (check(TT::IDENTIFIER)) {
            // (AR) الرمز الأول هو اسم نوع الاستثناء المخصص
            // (EN) First token is custom exception type name
            exceptionTypeName = firstToken.getValue();
            exceptionType = Data::DataType::OBJECT;
            exceptionVar = consume(TT::IDENTIFIER, 
                "(AR) توقع اسم متغير الاستثناء. (EN) Expected exception variable name.");
        }
        
        consume(TT::PAREN_RIGHT, 
            "(AR) توقع ')' بعد متغير الاستثناء. (EN) Expected ')' after exception variable.");
        
        // Parse catch body using Arabic syntax
        // (AR) تحليل جسم catch باستخدام الصيغة العربية
        auto catchBody = parseBlockStmt();
        
        catchClauses.push_back(CatchClause(
            exceptionVar.getValue(), 
            exceptionType,  // (AR) استخدام النوع المُحلل / (EN) Use parsed type
            std::move(catchBody),
            exceptionTypeName
        ));
    }
    
    // Parse optional finally block using Arabic syntax
    // (AR) تحليل كتلة finally الاختيارية باستخدام الصيغة العربية
    StmtPtr finallyBlock = nullptr;
    if (match(TT::KEYWORD_FINALLY)) {
        finallyBlock = parseBlockStmt();
    }
    
    // Create try statement node
    // (AR) إنشاء عقدة جملة Try
    return std::make_unique<TryStmt>(
        std::move(tryBlock),
        std::move(catchClauses),
        std::move(finallyBlock),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة raise: ارمِ تعبير;
 *        (EN) Parses raise statement: throw expression;
 */
StmtPtr ParserCore::parseRaiseStmt() {
    // Parse exception expression
    // (AR) تحليل تعبير الاستثناء
    auto exception = parseExpression();
    
    // (AR) الفاصلة المنقوطة اختيارية — لغة ص تعتمد على أسطر جديدة
    // (EN) Semicolons are optional — Sad language uses newlines
    if (check(TT::SEMICOLON) || check(TT::ARABIC_SEMICOLON)) {
        advance();
    }
    
    // Create raise statement node
    // (AR) إنشاء عقدة جملة Raise
    return std::make_unique<RaiseStmt>(
        std::move(exception),
        previous().getPosition()
    );
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
StmtPtr ParserCore::parseSwitchStmt() {
    // Save position for error reporting
    // (AR) حفظ الموقع للإبلاغ عن الأخطاء
    Token startToken = previous();
    
    // Expect opening parenthesis
    // (AR) توقع قوس الفتح
    consume(TT::PAREN_LEFT,
        "(AR) توقع '(' بعد 'حالة'. (EN) Expected '(' after 'case'.");
    
    // Parse switch expression
    // (AR) تحليل تعبير switch
    auto expr = parseExpression();
    
    // Expect closing parenthesis
    // (AR) توقع قوس الإغلاق
    consume(TT::PAREN_RIGHT,
        "(AR) توقع ')' بعد تعبير الحالة. (EN) Expected ')' after case expression.");
    
    // Parse case branches
    // (AR) تحليل فروع الحالات
    std::vector<CaseBranch> cases;
    StmtPtr defaultCase = nullptr;
    
    // Continue parsing cases until we hit default or end
    // (AR) استمر في تحليل الحالات حتى نصل إلى افتراضي أو نهاية
    while (!check(TT::KEYWORD_END) && !isAtEnd()) {
        if (match(TT::KEYWORD_WHEN)) {
            // Parse case value
            // (AR) تحليل قيمة الحالة
            auto caseValue = parseExpression();
            
            consume(TT::COLON,
                "(AR) توقع ':' بعد قيمة الحالة. (EN) Expected ':' after case value.");
            
            // Parse case body (single statement or block)
            // (AR) تحليل جسم الحالة (جملة واحدة أو كتلة)
            StmtPtr caseBody;
            if (check(TT::BRACE_LEFT)) {
                caseBody = parseBlockStmt();
            } else {
                caseBody = parseStatement();
            }
            
            // Add case branch
            // (AR) إضافة فرع الحالة
            cases.push_back({std::move(caseValue), std::move(caseBody)});
            
        } else if (match(TT::KEYWORD_DEFAULT)) {
            // Parse default case
            // (AR) تحليل الحالة الافتراضية
            consume(TT::COLON,
                "(AR) توقع ':' بعد افتراضي. (EN) Expected ':' after default.");
            
            // Parse default body
            // (AR) تحليل جسم الحالة الافتراضية
            if (check(TT::BRACE_LEFT)) {
                defaultCase = parseBlockStmt();
            } else {
                defaultCase = parseStatement();
            }
            
            // Default must be last, so break
            // (AR) الحالة الافتراضية يجب أن تكون الأخيرة، لذا اخرج
            break;
        } else {
            // Error: expected case or default
            // (AR) خطأ: توقع عندما أو افتراضي
            errorBilingual(
                "توقع 'عندما' أو 'افتراضي' في جملة حالة.",
                "Expected 'when' or 'default' in switch statement."
            );
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
        startToken.getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة تعبير: تعبير;
 *        (EN) Parses expression statement: expression;
 */
StmtPtr ParserCore::parseExpressionStmt() {
    auto expr = parseExpression();
    
    // Check if expression parsing failed
    // (AR) تحقق من فشل تحليل التعبير
    if (!expr) {
        errorBilingual(
            "خطأ نحوي: لا يمكن تحليل جملة التعبير. تأكد من صحة التعبير أو التصريح.",
            "Syntax error: cannot parse expression statement. Make sure the expression or declaration is valid."
        );
        return nullptr;
    }
    
    // Semicolon is optional for expression statements
    // (AR) الفاصلة المنقوطة اختيارية لجمل التعبير
    if (check(TT::SEMICOLON) || check(TT::ARABIC_SEMICOLON)) {
        if (check(TT::SEMICOLON)) {
            consume(TT::SEMICOLON, "");
        } else {
            consume(TT::ARABIC_SEMICOLON, "");
        }
    }
    
    // Create expression statement node
    // (AR) إنشاء عقدة جملة التعبير
    return std::make_unique<ExprStmt>(std::move(expr));
}

// ======================================================================
// (AR) تحليل التعبيرات / (EN) Expression Parsing
// ======================================================================


} // namespace Parser
} // namespace Sad
