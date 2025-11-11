/**
 * @file parser_core.h
 * @brief (AR) المحلل النحوي الأساسي - يحول الرموز (Tokens) إلى شجرة التركيب المجرد (AST).
 *        (EN) Core Parser - Converts tokens into Abstract Syntax Tree (AST).
 * 
 * (AR) هذا الملف يحتوي على المحلل النحوي الرئيسي للغة "ص" (Sad).
 *      يقوم المحلل بقراءة تدفق الرموز من المحلل المعجمي (Lexer)
 *      وبناء شجرة التركيب المجرد (AST) باستخدام تقنية Recursive Descent Parsing.
 *      يدعم المحلل جميع ميزات اللغة بما في ذلك:
 *      - التعبيرات (العمليات الحسابية، المنطقية، الاستدعاءات)
 *      - الجمل (if, while, for, return, إلخ)
 *      - التصريحات (الدوال، الأصناف، المتغيرات)
 *      - الميزات المتقدمة (List Comprehensions, Generators, Lambda)
 * 
 * (EN) This file contains the main parser for the Sad language.
 *      The parser reads token stream from the Lexer and builds
 *      an Abstract Syntax Tree (AST) using Recursive Descent Parsing technique.
 *      The parser supports all language features including:
 *      - Expressions (arithmetic, logical, function calls)
 *      - Statements (if, while, for, return, etc.)
 *      - Declarations (functions, classes, variables)
 *      - Advanced features (List Comprehensions, Generators, Lambda)
 * 
 * @author Sad Language Development Team
 * @date 2025-11-11
 * @license MIT License
 * 
 * @example
 * @code{.cpp}
 * // إنشاء محلل نحوي / Create a parser
 * Lexer::LexerCore lexer(source);
 * Parser::ParserCore parser(lexer);
 * 
 * // تحليل البرنامج / Parse the program
 * auto program = parser.parseProgram();
 * 
 * // التحقق من الأخطاء / Check for errors
 * if (parser.hasErrors()) {
 *     parser.printErrors();
 * }
 * @endcode
 */

#pragma once

#include "../lexer/lexer_core.h"
#include "../lexer/token.h"
#include "ast/ast_node.h"
#include "ast/expressions.h"
#include "ast/statements.h"
#include "ast/declarations.h"
#include "../errors/error_manager.h"

#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace Parser {

/**
 * @class ParserCore
 * @brief (AR) المحلل النحوي الرئيسي للغة "ص" - يبني شجرة AST من الرموز.
 *        (EN) Main parser for Sad language - builds AST from tokens.
 * 
 * (AR) يستخدم تقنية Recursive Descent Parsing لتحليل الكود المصدري.
 *      يحتفظ بـ current و peek tokens للمعاينة المسبقة (lookahead).
 *      يتبع أولويات العوامل المحددة في مواصفات اللغة.
 * 
 * (EN) Uses Recursive Descent Parsing technique to analyze source code.
 *      Maintains current and peek tokens for lookahead.
 *      Follows operator precedence defined in language specification.
 */
class ParserCore {
public:
    /**
     * @brief (AR) البناء - ينشئ محلل نحوي جديد مع مرجع للمحلل المعجمي.
     *        (EN) Constructor - Creates a new parser with reference to lexer.
     * 
     * @param lexer (AR) مرجع للمحلل المعجمي الذي يوفر الرموز.
     *              (EN) Reference to lexer that provides tokens.
     */
    explicit ParserCore(Lexer::LexerCore& lexer);

    /**
     * @brief (AR) الهدم - ينظف الموارد المستخدمة.
     *        (EN) Destructor - Cleans up resources.
     */
    ~ParserCore() = default;

    // ======================================================================
    // (AR) الدوال العامة / (EN) Public Interface
    // ======================================================================

    /**
     * @brief (AR) يحلل البرنامج الكامل - نقطة الدخول الرئيسية للمحلل.
     *        (EN) Parses complete program - main entry point for parser.
     * 
     * (AR) يقوم بتحليل جميع التصريحات على مستوى البرنامج وإرجاع قائمة من الجمل.
     *      يستمر في التحليل حتى نهاية الملف (EOF).
     * 
     * (EN) Parses all top-level declarations and returns list of statements.
     *      Continues parsing until end of file (EOF).
     * 
     * @return (AR) قائمة من جمل البرنامج (التصريحات، الجمل التنفيذية).
     *         (EN) List of program statements (declarations, executable statements).
     */
    AST::StmtList parseProgram();

    /**
     * @brief (AR) يتحقق إذا كانت هناك أخطاء تحليل.
     *        (EN) Checks if there are any parsing errors.
     * 
     * @return (AR) صحيح إذا وُجدت أخطاء، خطأ بخلاف ذلك.
     *         (EN) True if errors found, false otherwise.
     */
    bool hasErrors() const;

    /**
     * @brief (AR) يطبع جميع الأخطاء إلى الخرج القياسي.
     *        (EN) Prints all errors to standard output.
     */
    void printErrors() const;

    /**
     * @brief (AR) يرجع قائمة رسائل الأخطاء.
     *        (EN) Returns list of error messages.
     * 
     * @return (AR) مصفوفة من رسائل الأخطاء.
     *         (EN) Vector of error messages.
     */
    std::vector<std::string> getErrors() const;

private:
    // ======================================================================
    // (AR) دوال التحليل الأساسية / (EN) Core Parsing Functions
    // ======================================================================

    /**
     * @brief (AR) يحلل تصريح واحد (دالة، صنف، متغير، إلخ).
     *        (EN) Parses a single declaration (function, class, variable, etc.).
     * 
     * @return (AR) مؤشر لعقدة التصريح في AST.
     *         (EN) Pointer to declaration AST node.
     */
    AST::StmtPtr parseDeclaration();

    /**
     * @brief (AR) يحلل جملة واحدة (if, while, return, إلخ).
     *        (EN) Parses a single statement (if, while, return, etc.).
     * 
     * @return (AR) مؤشر لعقدة الجملة في AST.
     *         (EN) Pointer to statement AST node.
     */
    AST::StmtPtr parseStatement();

    // ======================================================================
    // (AR) تحليل التصريحات / (EN) Declaration Parsing
    // ======================================================================

    /**
     * @brief (AR) يحلل تصريح دالة (دالة عادية، طريقة، دالة مولّدة).
     *        (EN) Parses function declaration (regular function, method, generator).
     * 
     * @return (AR) مؤشر لعقدة تصريح الدالة.
     *         (EN) Pointer to function declaration node.
     */
    AST::StmtPtr parseFunctionDecl();

    /**
     * @brief (AR) يحلل تصريح صنف (class) مع الحقول والطرق.
     *        (EN) Parses class declaration with fields and methods.
     * 
     * @return (AR) مؤشر لعقدة تصريح الصنف.
     *         (EN) Pointer to class declaration node.
     */
    AST::StmtPtr parseClassDecl();

    /**
     * @brief (AR) يحلل تصريح متغير (var x = value).
     *        (EN) Parses variable declaration (var x = value).
     * 
     * @return (AR) مؤشر لعقدة تصريح المتغير.
     *         (EN) Pointer to variable declaration node.
     */
    AST::StmtPtr parseVarDecl();

    /**
     * @brief (AR) يحلل تصريح enum مع قائمة الأعضاء.
     *        (EN) Parses enum declaration with member list.
     * 
     * @return (AR) مؤشر لعقدة تصريح Enum.
     *         (EN) Pointer to enum declaration node.
     */
    AST::StmtPtr parseEnumDecl();

    /**
     * @brief (AR) يحلل جملة استيراد (استورد module).
     *        (EN) Parses import statement (import module).
     * 
     * @return (AR) مؤشر لعقدة جملة الاستيراد.
     *         (EN) Pointer to import statement node.
     */
    AST::StmtPtr parseImportStmt();

    /**
     * @brief (AR) يحلل جملة تصدير (صدّر function/class).
     *        (EN) Parses export statement (export function/class).
     * 
     * @return (AR) مؤشر لعقدة جملة التصدير.
     *         (EN) Pointer to export statement node.
     */
    AST::StmtPtr parseExportStmt();

    // ======================================================================
    // (AR) تحليل الجمل / (EN) Statement Parsing
    // ======================================================================

    /**
     * @brief (AR) يحلل جملة if مع else/else-if اختيارية.
     *        (EN) Parses if statement with optional else/else-if.
     * 
     * @return (AR) مؤشر لعقدة جملة If.
     *         (EN) Pointer to if statement node.
     */
    AST::StmtPtr parseIfStmt();

    /**
     * @brief (AR) يحلل حلقة while.
     *        (EN) Parses while loop.
     * 
     * @return (AR) مؤشر لعقدة جملة While.
     *         (EN) Pointer to while statement node.
     */
    AST::StmtPtr parseWhileStmt();

    /**
     * @brief (AR) يحلل حلقة for (لكل element في collection).
     *        (EN) Parses for loop (for element in collection).
     * 
     * @return (AR) مؤشر لعقدة جملة For.
     *         (EN) Pointer to for statement node.
     */
    AST::StmtPtr parseForStmt();

    /**
     * @brief (AR) يحلل جملة return مع قيمة اختيارية.
     *        (EN) Parses return statement with optional value.
     * 
     * @return (AR) مؤشر لعقدة جملة Return.
     *         (EN) Pointer to return statement node.
     */
    AST::StmtPtr parseReturnStmt();

    /**
     * @brief (AR) يحلل جملة break (للخروج من الحلقات).
     *        (EN) Parses break statement (exit loops).
     * 
     * @return (AR) مؤشر لعقدة جملة Break.
     *         (EN) Pointer to break statement node.
     */
    AST::StmtPtr parseBreakStmt();

    /**
     * @brief (AR) يحلل جملة continue (للمتابعة إلى التكرار التالي).
     *        (EN) Parses continue statement (skip to next iteration).
     * 
     * @return (AR) مؤشر لعقدة جملة Continue.
     *         (EN) Pointer to continue statement node.
     */
    AST::StmtPtr parseContinueStmt();

    /**
     * @brief (AR) يحلل كتلة من الجمل محاطة بأقواس معقوفة { }.
     *        (EN) Parses block of statements enclosed in braces { }.
     * 
     * @return (AR) مؤشر لعقدة كتلة الجمل.
     *         (EN) Pointer to block statement node.
     */
    AST::StmtPtr parseBlockStmt();

    /**
     * @brief (AR) يحلل جملة try-catch-finally للتعامل مع الأخطاء.
     *        (EN) Parses try-catch-finally statement for error handling.
     * 
     * @return (AR) مؤشر لعقدة جملة Try.
     *         (EN) Pointer to try statement node.
     */
    AST::StmtPtr parseTryStmt();

    /**
     * @brief (AR) يحلل جملة raise لرفع استثناء.
     *        (EN) Parses raise statement to throw exception.
     * 
     * @return (AR) مؤشر لعقدة جملة Raise.
     *         (EN) Pointer to raise statement node.
     */
    AST::StmtPtr parseRaiseStmt();

    /**
     * @brief (AR) يحلل جملة with لإدارة الموارد.
     *        (EN) Parses with statement for resource management.
     * 
     * @return (AR) مؤشر لعقدة جملة With.
     *         (EN) Pointer to with statement node.
     */
    AST::StmtPtr parseWithStmt();

    /**
     * @brief (AR) يحلل جملة تعبير (تعبير ينتهي بفاصلة منقوطة).
     *        (EN) Parses expression statement (expression followed by semicolon).
     * 
     * @return (AR) مؤشر لعقدة جملة التعبير.
     *         (EN) Pointer to expression statement node.
     */
    AST::StmtPtr parseExpressionStmt();

    // ======================================================================
    // (AR) تحليل التعبيرات / (EN) Expression Parsing
    // ======================================================================

    /**
     * @brief (AR) يحلل تعبير (نقطة دخول لتحليل التعبيرات).
     *        (EN) Parses expression (entry point for expression parsing).
     * 
     * @return (AR) مؤشر لعقدة التعبير.
     *         (EN) Pointer to expression node.
     */
    AST::ExprPtr parseExpression();

    /**
     * @brief (AR) يحلل عملية تعيين (assignment).
     *        (EN) Parses assignment operation.
     * 
     * @return (AR) مؤشر لعقدة تعبير التعيين.
     *         (EN) Pointer to assignment expression node.
     */
    AST::ExprPtr parseAssignment();

    /**
     * @brief (AR) يحلل العوامل المنطقية (أو - OR).
     *        (EN) Parses logical OR operators.
     * 
     * @return (AR) مؤشر لعقدة التعبير المنطقي.
     *         (EN) Pointer to logical expression node.
     */
    AST::ExprPtr parseLogicalOr();

    /**
     * @brief (AR) يحلل العوامل المنطقية (و - AND).
     *        (EN) Parses logical AND operators.
     * 
     * @return (AR) مؤشر لعقدة التعبير المنطقي.
     *         (EN) Pointer to logical expression node.
     */
    AST::ExprPtr parseLogicalAnd();

    /**
     * @brief (AR) يحلل عوامل المساواة (== !=).
     *        (EN) Parses equality operators (== !=).
     * 
     * @return (AR) مؤشر لعقدة تعبير المقارنة.
     *         (EN) Pointer to comparison expression node.
     */
    AST::ExprPtr parseEquality();

    /**
     * @brief (AR) يحلل عوامل المقارنة (< <= > >=).
     *        (EN) Parses comparison operators (< <= > >=).
     * 
     * @return (AR) مؤشر لعقدة تعبير المقارنة.
     *         (EN) Pointer to comparison expression node.
     */
    AST::ExprPtr parseComparison();

    /**
     * @brief (AR) يحلل عوامل الجمع والطرح (+ -).
     *        (EN) Parses addition and subtraction operators (+ -).
     * 
     * @return (AR) مؤشر لعقدة تعبير الجمع/الطرح.
     *         (EN) Pointer to addition/subtraction expression node.
     */
    AST::ExprPtr parseTerm();

    /**
     * @brief (AR) يحلل عوامل الضرب والقسمة (* / %).
     *        (EN) Parses multiplication and division operators (* / %).
     * 
     * @return (AR) مؤشر لعقدة تعبير الضرب/القسمة.
     *         (EN) Pointer to multiplication/division expression node.
     */
    AST::ExprPtr parseFactor();

    /**
     * @brief (AR) يحلل العوامل الأحادية (- ! ++ --).
     *        (EN) Parses unary operators (- ! ++ --).
     * 
     * @return (AR) مؤشر لعقدة تعبير أحادي.
     *         (EN) Pointer to unary expression node.
     */
    AST::ExprPtr parseUnary();

    /**
     * @brief (AR) يحلل عامل الأس (**).
     *        (EN) Parses power operator (**).
     * 
     * @return (AR) مؤشر لعقدة تعبير الأس.
     *         (EN) Pointer to power expression node.
     */
    AST::ExprPtr parsePower();

    /**
     * @brief (AR) يحلل استدعاءات الدوال والوصول للأعضاء (الفهرسة).
     *        (EN) Parses function calls and member access (indexing).
     * 
     * @return (AR) مؤشر لعقدة تعبير الاستدعاء/الوصول.
     *         (EN) Pointer to call/access expression node.
     */
    AST::ExprPtr parsePostfix();

    /**
     * @brief (AR) يحلل التعبيرات الأساسية (الأرقام، النصوص، المتغيرات، إلخ).
     *        (EN) Parses primary expressions (numbers, strings, variables, etc.).
     * 
     * @return (AR) مؤشر لعقدة تعبير أساسي.
     *         (EN) Pointer to primary expression node.
     */
    AST::ExprPtr parsePrimary();

    /**
     * @brief (AR) يحلل دالة لامدا (lambda x: x + 1).
     *        (EN) Parses lambda function (lambda x: x + 1).
     * 
     * @return (AR) مؤشر لعقدة تعبير لامدا.
     *         (EN) Pointer to lambda expression node.
     */
    AST::ExprPtr parseLambda();

    /**
     * @brief (AR) يحلل List Comprehension ([x*2 for x in list]).
     *        (EN) Parses list comprehension ([x*2 for x in list]).
     * 
     * @return (AR) مؤشر لعقدة List Comprehension.
     *         (EN) Pointer to list comprehension node.
     */
    AST::ExprPtr parseListComprehension();

    /**
     * @brief (AR) يحلل Dictionary Comprehension ({k: v for k, v in dict}).
     *        (EN) Parses dictionary comprehension ({k: v for k, v in dict}).
     * 
     * @return (AR) مؤشر لعقدة Dict Comprehension.
     *         (EN) Pointer to dict comprehension node.
     */
    AST::ExprPtr parseDictComprehension();

    /**
     * @brief (AR) يحلل مصفوفة حرفية [1, 2, 3].
     *        (EN) Parses array literal [1, 2, 3].
     * 
     * @return (AR) مؤشر لعقدة تعبير المصفوفة.
     *         (EN) Pointer to array expression node.
     */
    AST::ExprPtr parseArrayLiteral();

    /**
     * @brief (AR) يحلل خريطة حرفية {key: value}.
     *        (EN) Parses map literal {key: value}.
     * 
     * @return (AR) مؤشر لعقدة تعبير الخريطة.
     *         (EN) Pointer to map expression node.
     */
    AST::ExprPtr parseMapLiteral();

    // ======================================================================
    // (AR) دوال مساعدة / (EN) Helper Functions
    // ======================================================================

    /**
     * @brief (AR) يتقدم إلى الرمز التالي.
     *        (EN) Advances to next token.
     */
    void advance();

    /**
     * @brief (AR) يتحقق إذا كان الرمز الحالي من النوع المحدد.
     *        (EN) Checks if current token is of specified type.
     * 
     * @param type (AR) نوع الرمز المتوقع. (EN) Expected token type.
     * @return (AR) صحيح إذا تطابق النوع. (EN) True if type matches.
     */
    bool check(Lexer::TokenType type) const;

    /**
     * @brief (AR) يتحقق ويتقدم إذا كان الرمز الحالي من النوع المحدد.
     *        (EN) Checks and advances if current token is of specified type.
     * 
     * @param type (AR) نوع الرمز المتوقع. (EN) Expected token type.
     * @return (AR) صحيح إذا تم التطابق والتقدم. (EN) True if matched and advanced.
     */
    bool match(Lexer::TokenType type);

    /**
     * @brief (AR) يتحقق ويتقدم إذا كان الرمز الحالي من أحد الأنواع المحددة.
     *        (EN) Checks and advances if current token is one of specified types.
     * 
     * @param types (AR) قائمة أنواع الرموز المحتملة. (EN) List of possible token types.
     * @return (AR) صحيح إذا تم التطابق. (EN) True if matched.
     */
    bool matchAny(const std::vector<Lexer::TokenType>& types);

    /**
     * @brief (AR) يستهلك رمزاً من النوع المحدد أو يرفع خطأ.
     *        (EN) Consumes token of specified type or raises error.
     * 
     * @param type (AR) نوع الرمز المتوقع. (EN) Expected token type.
     * @param message (AR) رسالة الخطأ إذا لم يتطابق. (EN) Error message if not matched.
     * @return (AR) الرمز المستهلك. (EN) Consumed token.
     */
    Lexer::Token consume(Lexer::TokenType type, const std::string& message);

    /**
     * @brief (AR) يتحقق إذا وصلنا لنهاية الملف.
     *        (EN) Checks if we reached end of file.
     * 
     * @return (AR) صحيح إذا وصلنا للنهاية. (EN) True if at end.
     */
    bool isAtEnd() const;

    /**
     * @brief (AR) يعيد الرمز الحالي.
     *        (EN) Returns current token.
     * 
     * @return (AR) الرمز الحالي. (EN) Current token.
     */
    const Lexer::Token& peek() const;

    /**
     * @brief (AR) يعيد الرمز السابق.
     *        (EN) Returns previous token.
     * 
     * @return (AR) الرمز السابق. (EN) Previous token.
     */
    const Lexer::Token& previous() const;

    /**
     * @brief (AR) يسجل خطأ تحليل مع رسالة ومكان الخطأ.
     *        (EN) Records parsing error with message and location.
     * 
     * @param message (AR) رسالة الخطأ. (EN) Error message.
     */
    void error(const std::string& message);

    /**
     * @brief (AR) يحاول التعافي من خطأ التحليل بالانتقال للجملة التالية.
     *        (EN) Attempts to recover from parsing error by advancing to next statement.
     */
    void synchronize();

    /**
     * @brief (AR) يحلل قائمة معاملات الدالة (x, y, z).
     *        (EN) Parses function parameter list (x, y, z).
     * 
     * @return (AR) مصفوفة من أسماء المعاملات. (EN) Vector of parameter names.
     */
    std::vector<std::string> parseParameterList();

    /**
     * @brief (AR) يحلل قائمة وسائط استدعاء دالة (f(1, 2, 3)).
     *        (EN) Parses function call argument list (f(1, 2, 3)).
     * 
     * @return (AR) مصفوفة من تعبيرات الوسائط. (EN) Vector of argument expressions.
     */
    AST::ExprList parseArgumentList();

private:
    // ======================================================================
    // (AR) الأعضاء الخاصة / (EN) Private Members
    // ======================================================================

    Lexer::LexerCore& lexer_;           ///< (AR) مرجع للمحلل المعجمي (EN) Reference to lexer
    Lexer::Token current_;               ///< (AR) الرمز الحالي (EN) Current token
    Lexer::Token previous_;              ///< (AR) الرمز السابق (EN) Previous token
    std::vector<std::string> errors_;    ///< (AR) قائمة الأخطاء (EN) List of errors
    bool panicMode_;                     ///< (AR) وضع الذعر للتعافي من الأخطاء (EN) Panic mode for error recovery
};

} // namespace Parser
} // namespace Sad
