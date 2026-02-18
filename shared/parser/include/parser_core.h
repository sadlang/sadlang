/**
 * @file parser_core.h
 * @brief (AR) المحلل النحوي الأساسي - يحول الرموز (Tokens) إلى شجرة التركيب المجرد (AST).
 *        (EN) Core Parser - Converts tokens into Abstract Syntax Tree (AST).
 * @note Updated: 2025-12-09 - Module system integration complete
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

#include "lexer_core.h"
#include "token.h"
#include "ast_node.h"
#include "expressions.h"
#include "statements.h"
#include "declarations.h"
#include "property_nodes.h"
#include "class_nodes.h"
#include "pattern_nodes.h"
#include "error_manager.h"

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
     * @brief (AR) يحلل تصريح دالة (function) مع مُزخرِفات اختيارية.
     *        (EN) Parses function declaration with optional decorators.
     * 
     * @param decorators (AR) قائمة المُزخرِفات المُطبقة على الدالة
     *                   (EN) List of decorators applied to the function
     * 
     * @return (AR) مؤشر لعقدة تصريح الدالة.
     *         (EN) Pointer to function declaration node.
     */
    AST::StmtPtr parseFunctionDecl(AST::ExprList decorators = AST::ExprList(), bool is_async = false, bool is_generator = false);

    /**
     * @brief (AR) يحلل تصريح صنف (class) مع الحقول والطرق.
     *        (EN) Parses class declaration with fields and methods.
     * 
     * @return (AR) مؤشر لعقدة تصريح الصنف.
     *         (EN) Pointer to class declaration node.
     */
    AST::StmtPtr parseClassDecl();
    
    /**
     * @brief (AR) يحلل تصريح واجهة/سمة
     *        (EN) Parses trait/interface declaration
     */
    AST::StmtPtr parseTraitDecl();
    
    /**
     * @brief (AR) يحلل كتلة تنفيذ سمة لصنف
     *        (EN) Parses impl block
     */
    AST::StmtPtr parseImplDecl();
    
    // =====================================================================
    // (AR) تحليل القوالب والميزات المتقدمة (Phase 7B)
    // (EN) Template and Advanced Features Parsing (Phase 7B)
    // =====================================================================
    
    /**
     * @brief (AR) يحلل تصريح قالب (دالة أو صنف).
     *        (EN) Parses template declaration (function or class).
     * 
     * @details
     * (AR) يدعم الأنماط التالية:
     *      - قالب<نوع ت> دالة اسم() { ... }
     *      - قالب<نوع ت، نوع م> صنف اسم { ... }
     *      - قالب<نوع ت: قابل_للمقارنة> ...
     * 
     * (EN) Supports the following patterns:
     *      - template<typename T> func name() { ... }
     *      - template<typename T, typename U> class name { ... }
     *      - template<typename T: Comparable> ...
     * 
     * @return (AR) مؤشر لعقدة تصريح القالب (TemplateFunctionDecl أو TemplateClassDecl).
     *         (EN) Pointer to template declaration node.
     */
    AST::StmtPtr parseTemplateDecl();
    
    /**
     * @brief (AR) يحلل معاملات أنواع القالب <نوع ت، نوع م>.
     *        (EN) Parses template type parameters <typename T, typename U>.
     * 
     * @return (AR) قائمة معاملات الأنواع.
     *         (EN) List of type parameters.
     */
    std::vector<AST::TypeParameter> parseTemplateParameters();
    
    /**
     * @brief (AR) يحلل تصريح فضاء الأسماء.
     *        (EN) Parses namespace declaration.
     * 
     * @details
     * (AR) يدعم الأنماط التالية:
     *      - فضاء رياضيات ... نهاية_فضاء
     *      - namespace math { ... }
     * 
     * @return (AR) مؤشر لعقدة تصريح فضاء الأسماء.
     *         (EN) Pointer to namespace declaration node.
     */
    AST::StmtPtr parseNamespaceDecl();
    
    /**
     * @brief (AR) يحلل تصريح تحميل عامل داخل صنف.
     *        (EN) Parses operator overload declaration inside class.
     * 
     * @details
     * (AR) يدعم الأنماط التالية:
     *      - عامل +(آخر: نوع) نوع { ... }
     *      - عامل [](فهرس: رقم) نوع { ... }
     * 
     * @return (AR) مؤشر لعقدة تصريح العامل.
     *         (EN) Pointer to operator declaration node.
     */
    AST::StmtPtr parseOperatorDecl();
    
    /**
     * @brief (AR) يحلل جملة استيراد - استورد [وحدة] [كـ اسم]
     *        (EN) Parses import statement - import [module] [as name]
     * 
     * @details
     * (AR) يدعم الأنماط التالية:
     *      - استورد رياضيات
     *      - استورد رياضيات كـ م
     *      - استورد مكتبتي.أدوات.رياضية
     * 
     * (EN) Supports the following patterns:
     *      - import math
     *      - import math as m
     *      - import mylib.utils.math
     * 
     * @return (AR) مؤشر لعقدة ImportStmt
     *         (EN) Pointer to ImportStmt node
     * 
     * @example
     * @code{.ص}
     * استورد رياضيات
     * استورد رياضيات كـ م
     * @endcode
     */
    AST::StmtPtr parseImportStmt();
    
    /**
     * @brief (AR) يحلل جملة استيراد انتقائي - من [وحدة] استورد [عناصر]
     *        (EN) Parses from-import statement - from [module] import [items]
     * 
     * @details
     * (AR) يدعم الأنماط التالية:
     *      - من رياضيات استورد جذر، قوة
     *      - من رياضيات استورد جذر كـ ج
     *      - من رياضيات استورد *
     * 
     * (EN) Supports the following patterns:
     *      - from math import sqrt, pow
     *      - from math import sqrt as s
     *      - from math import *
     * 
     * @return (AR) مؤشر لعقدة FromImportStmt
     *         (EN) Pointer to FromImportStmt node
     * 
     * @example
     * @code{.ص}
     * من رياضيات استورد جذر، قوة
     * من رياضيات استورد *
     * @endcode
     */
    AST::StmtPtr parseFromImportStmt();
    
    /**
     * @brief (AR) يحلل تصريح تصدير - صدّر [تصريح]
     *        (EN) Parses export declaration - export [declaration]
     * 
     * @details
     * (AR) يدعم تصدير:
     *      - الدوال: صدّر دالة حساب() { ... }
     *      - الأصناف: صدّر صنف شكل { ... }
     *      - المتغيرات: صدّر ثابت PI = 3.14
     * 
     * (EN) Supports exporting:
     *      - Functions: export function calc() { ... }
     *      - Classes: export class Shape { ... }
     *      - Variables: export const PI = 3.14
     * 
     * @return (AR) مؤشر لعقدة ExportDecl
     *         (EN) Pointer to ExportDecl node
     * 
     * @example
     * @code{.ص}
     * صدّر دالة مضاعفة(رقم س) { إرجاع س * 2 }
     * صدّر ثابت PI = 3.14
     * @endcode
     */
    AST::StmtPtr parseExportDecl();

    // ======================================================================
    // (AR) دوال تحليل البرمجة الكائنية / (EN) OOP Parsing Functions
    // ======================================================================

    /**
     * @brief (AR) يحلل تصريح خاصية في صنف
     *        (EN) Parses field declaration in class
     */
    std::unique_ptr<AST::FieldDecl> parseFieldDeclaration(AST::AccessModifier access, bool isStatic = false);

    /**
     * @brief (AR) يحلل تصريح خاصية (Property) مع getter/setter
     *        (EN) Parses property declaration with getter/setter
     */
    std::unique_ptr<AST::PropertyDecl> parsePropertyDeclaration(AST::AccessModifier access, bool isStatic = false);

    /**
     * @brief (AR) يحلل تصريح طريقة في صنف
     *        (EN) Parses method declaration in class
     */
    std::unique_ptr<AST::MethodDecl> parseMethodDeclaration(AST::AccessModifier access, bool isStatic, bool isVirtual, bool isAbstract);

    /**
     * @brief (AR) يحلل تصريح باني (constructor)
     *        (EN) Parses constructor declaration
     */
    std::unique_ptr<AST::ConstructorDecl> parseConstructorDeclaration(const std::string& className, AST::AccessModifier access);

    /**
     * @brief (AR) يحلل تصريح هدام (destructor)
     *        (EN) Parses destructor declaration
     */
    std::unique_ptr<AST::DestructorDecl> parseDestructorDeclaration(const std::string& className, AST::AccessModifier access);

    /**
     * @brief (AR) يحلل معدلات الرؤية (عام، خاص، محمي) والمعدلات (ثابت، افتراضي، مجرد)
     *        (EN) Parses visibility modifiers and modifiers (static, virtual, abstract)
     */
    AST::AccessModifier parseModifiers(bool& isStatic, bool& isVirtual, bool& isAbstract);

    /**
     * @brief (AR) يحلل تعبير new لإنشاء كائن
     *        (EN) Parses new expression for object instantiation
     */
    AST::ExprPtr parseNewExpression();

    /**
     * @brief (AR) يحلل تعبير this
     *        (EN) Parses this expression
     */
    AST::ExprPtr parseThisExpression();

    /**
     * @brief (AR) يحلل تعبير super
     *        (EN) Parses super expression
     */
    AST::ExprPtr parseSuperExpression();

    /**
     * @brief (AR) يحلل قائمة تهيئة الباني
     *        (EN) Parses constructor initializer list
     */
    std::vector<AST::FieldInitializer> parseInitializerList();

    /**
     * @brief (AR) يحلل معاملات دالة/طريقة
     *        (EN) Parses function/method parameters
     */
    std::vector<AST::Parameter> parseParameters();

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
     * @brief (AR) يحلل جملة yield (للدوال المولّدة).
     *        (EN) Parses yield statement (for generator functions).
     * 
     * Grammar / القواعد:
     *   yield_stmt → "yield" [ "from" ]? expression? ";"
     * 
     * Supports two forms:
     * - yield expr         : yields a single value
     * - yield from iterable: delegates to another generator
     * 
     * يدعم صيغتين:
     * - yield expr         : تُعطي قيمة واحدة
     * - yield from iterable: تفوّض إلى مولّد آخر
     * 
     * @example Examples / أمثلة:
     * - yield 42;
     * - yield x * 2;
     * - yield from range(10);
     * - اعطِ 100؛
     * 
     * @return (AR) مؤشر لعقدة جملة Yield.
     *         (EN) Pointer to yield statement node.
     */
    AST::StmtPtr parseYieldStmt();

    /**
     * @brief (AR) يحلل جملة باستخدام (مدير السياق).
     *        (EN) Parses with statement (context manager).
     * 
     * Context managers ensure proper resource management by calling
     * __دخول__() on entry and __خروج__() on exit.
     * 
     * Syntax:
     * - باستخدام expr كـ alias: body نهاية_استخدام
     * - with resource as name: ... end_with
     * 
     * @return (AR) مؤشر لعقدة جملة With.
     *         (EN) Pointer to with statement node.
     */
    AST::StmtPtr parseWithStmt();

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
    AST::StmtPtr parseBlockStmt(bool* closedByEnd = nullptr);

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
     * @brief (AR) يحلل جملة switch-case للتحكم متعدد الفروع.
     *        (EN) Parses switch-case statement for multi-way branching.
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
     *       [عندما <value>: <statement>]*
     *       [افتراضي: <statement>]
     *   نهاية
     * 
     * @example Examples / أمثلة:
     * حالة (يوم)
     *     عندما 1: اطبع("الإثنين")
     *     عندما 2: اطبع("الثلاثاء")
     *     افتراضي: اطبع("يوم آخر")
     * نهاية
     * 
     * @return (AR) مؤشر لعقدة جملة Switch.
     *         (EN) Pointer to switch statement node.
     * 
     * @note (AR) الشرط يجب أن يكون بين أقواس.
     *       (EN) Condition must be enclosed in parentheses.
     * @note (AR) لا يوجد fall-through - ينفذ أول حالة مطابقة فقط.
     *       (EN) No fall-through - executes only first matching case.
     * @note Spec reference: docs/language_spec/rules/04_syntax.md
     */
    AST::StmtPtr parseSwitchStmt();

    /**
     * @brief (AR) يحلل جملة match لمطابقة الأنماط.
     *        (EN) Parses match statement for pattern matching.
     * 
     * Grammar / القواعد:
     *   match_stmt → KEYWORD_MATCH expr ":"
     *                (case_clause)+
     * 
     * Syntax / النحو:
     *   match <expression>:
     *       case <pattern> [if <guard>]:
     *           <statements>
     * 
     * @example Examples / أمثلة:
     * match x:
     *     case 0:
     *         print("صفر")
     *     case n if n > 0:
     *         print("موجب")
     *     case _:
     *         print("آخر")
     * 
     * @return (AR) مؤشر لعقدة جملة Match.
     *         (EN) Pointer to match statement node.
     */
    AST::StmtPtr parseMatchStmt();

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
     * @brief (AR) يحلل عامل الأنبوب |> مع إزالة السكر النحوي.
     *        (EN) Parses pipeline operator |> with desugaring.
     * 
     * Grammar / القواعد:
     *   pipeline → assignment ("|>" assignment)*
     * 
     * @return (AR) مؤشر لعقدة تعبير الأنبوب (مُزال السكر إلى استدعاءات).
     *         (EN) Pointer to pipeline expression (desugared to calls).
     */
    AST::ExprPtr parsePipeline();

    /**
     * @brief (AR) يحلل التعبير الثلاثي الشرطي (ternary conditional).
     *        (EN) Parses ternary conditional expression.
     * 
     * Grammar / القواعد:
     *   ternary → logical_or ("?" expression ":" ternary)?
     * 
     * Syntax / النحو:
     *   condition ? true_expression : false_expression
     *   الشرط ؟ تعبير_صحيح : تعبير_خطأ
     * 
     * @example Examples / أمثلة:
     * x > 0 ? "positive" : "negative"
     * age >= 18 ? "adult" : "minor"
     * العمر >= 18 ؟ "بالغ" : "قاصر"
     * 
     * @return (AR) مؤشر لعقدة التعبير الثلاثي أو تعبير منطقي.
     *         (EN) Pointer to ternary expression or logical expression node.
     * 
     * @note Right-associative: a ? b : c ? d : e → a ? b : (c ? d : e)
     * @note Spec reference: docs/language_spec/rules/04_syntax.md
     */
    AST::ExprPtr parseTernary();

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
     * @brief (AR) يحلل عامل OR البتّي: |.
     *        (EN) Parses bitwise OR operator: |.
     */
    AST::ExprPtr parseBitwiseOr();

    /**
     * @brief (AR) يحلل عامل XOR البتّي: ^.
     *        (EN) Parses bitwise XOR operator: ^.
     */
    AST::ExprPtr parseBitwiseXor();

    /**
     * @brief (AR) يحلل عامل AND البتّي: &.
     *        (EN) Parses bitwise AND operator: &.
     */
    AST::ExprPtr parseBitwiseAnd();

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
     * @brief (AR) يحلل تنفيذ قالب مثل: أكبر<رقم>(10, 20) أو صندوق<نص>()
     *        (EN) Parses template instantiation like: max<int>(10, 20) or Box<string>()
     * 
     * @details
     * (AR) يُستدعى عندما يُكتشف معرّف متبوع بـ < وقد يكون تنفيذ قالب.
     *      يُميز بين تنفيذ القالب وعامل المقارنة < بالتحقق مما يلي الـ <
     * 
     * (EN) Called when identifier followed by < is detected and might be template.
     *      Distinguishes between template instantiation and < comparison operator
     *      by checking what follows the <
     * 
     * @param templateName (AR) اسم القالب / (EN) Template name
     * @param pos (AR) موقع في الكود / (EN) Position in code
     * @return (AR) مؤشر لعقدة TemplateInstantiation أو nullptr إذا لم يكن قالب
     *         (EN) Pointer to TemplateInstantiation node or nullptr if not template
     */
    AST::ExprPtr parseTemplateInstantiation(const std::string& templateName, 
                                            const Lexer::Position& pos);
    
    /**
     * @brief (AR) يتحقق إذا كان الرمز الحالي بداية type argument للقوالب
     *        (EN) Checks if current token starts a template type argument
     * 
     * @return (AR) صحيح إذا كان نوع أو معرف يمكن أن يكون نوع قالب
     *         (EN) True if type or identifier that could be template type
     */
    bool isTypeArgumentStart();

    /**
     * @brief (AR) يحلل دالة لامدا (lambda x: x + 1).
     *        (EN) Parses lambda function (lambda x: x + 1).
     * 
     * @return (AR) مؤشر لعقدة تعبير لامدا.
     *         (EN) Pointer to lambda expression node.
     */
    AST::ExprPtr parseLambda();
    
    /**
     * @brief (AR) يحلل f-string ويحوله إلى تسلسل نصوص.
     *        (EN) Parses f-string and converts to string concatenation.
     * 
     * F-strings like f"Hello {name}!" are converted to:
     * "Hello " + str(name) + "!"
     * 
     * النصوص المنسقة مثل f"مرحبا {الاسم}!" تُحول إلى:
     * "مرحبا " + str(الاسم) + "!"
     * 
     * @param fstring The f-string content / محتوى f-string
     * @param pos Source position / الموقع في الكود
     * @return (AR) مؤشر لعقدة تعبير التسلسل.
     *         (EN) Pointer to concatenation expression node.
     */
    AST::ExprPtr parseFStringExpr(const std::string& fstring, const Lexer::Position& pos);
    
    /**
     * @brief (AR) يحلل مُزخرِف (decorator): @decorator أو @decorator(args).
     *        (EN) Parses decorator: @decorator or @decorator(args).
     * 
     * Grammar / القواعد:
     *   decorator ::= '@' identifier ['(' [argument_list] ')']
     * 
     * Examples / أمثلة:
     *   - @staticmethod
     *   - @cache(maxsize=100)
     *   - @retry(times=3, delay=1.5)
     *   - @مُزخرِف
     *   - @تخزين_مؤقت(حجم=100)
     * 
     * @return (AR) مؤشر لعقدة تعبير المُزخرِف.
     *         (EN) Pointer to decorator expression node.
     * 
     * @note (AR) يجب أن يكون الرمز الحالي هو AT_SIGN (@)
     * @note (EN) Current token must be AT_SIGN (@)
     */
    AST::ExprPtr parseDecorator();

    /**
     * @brief (AR) يحلل arrow function: (x, y) => x + y.
     *        (EN) Parses arrow function: (x, y) => x + y.
     * 
     * Grammar:
     *   arrow_function ::= '(' [typed_param_list] ')' '=>' expression
     *                    | identifier '=>' expression
     * 
     * Examples:
     *   - (x, y) => x + y
     *   - x => x * 2
     *   - () => 42
     *   - (x: int) => x * 2
     * 
     * @return (AR) مؤشر لعقدة تعبير arrow function (LambdaExpr).
     *         (EN) Pointer to arrow function expression node (LambdaExpr).
     */
    AST::ExprPtr parseArrowFunction();

    /**
     * @brief (AR) يتحقق إذا كان التسلسل الحالي arrow function.
     *        (EN) Checks if current sequence is arrow function.
     * 
     * Lookahead patterns:
     *   - identifier '=>'  (e.g., x => x * 2)
     *   - '(' ... ')' '=>'  (e.g., (x, y) => x + y)
     * 
     * Note: This function uses lookahead and restores parser position.
     * 
     * @return (AR) true إذا كان arrow function، وإلا false.
     *         (EN) true if arrow function, false otherwise.
     */
    bool isArrowFunction();

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
    // (AR) تحليل الأنماط / (EN) Pattern Parsing
    // ======================================================================

    /**
     * @brief (AR) يحلل نمط في match statement
     *        (EN) Parses a pattern in match statement
     * 
     * @return (AR) مؤشر فريد للنمط
     *         (EN) Unique pointer to pattern
     */
    std::unique_ptr<AST::Pattern> parsePattern();

    /**
     * @brief (AR) يحلل نمط أساسي (literal, variable, wildcard)
     *        (EN) Parses primary pattern (literal, variable, wildcard)
     * 
     * @return (AR) مؤشر فريد للنمط الأساسي
     *         (EN) Unique pointer to primary pattern
     */
    std::unique_ptr<AST::Pattern> parsePrimaryPattern();

    /**
     * @brief (AR) يحلل نمط قائمة [...]
     *        (EN) Parses list pattern [...]
     * 
     * @return (AR) مؤشر فريد لنمط القائمة
     *         (EN) Unique pointer to list pattern
     */
    std::unique_ptr<AST::Pattern> parseListPattern();

    /**
     * @brief (AR) يحلل فرع case واحد
     *        (EN) Parses one case clause
     * 
     * @return (AR) CaseClause structure
     *         (EN) CaseClause structure
     */
    AST::CaseClause parseCaseClause();

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
     * @brief (AR) يعيد الرمز التالي.
     *        (EN) Returns next token.
     * 
     * @return (AR) الرمز التالي. (EN) Next token.
     */
    const Lexer::Token& peekNext() const;

    /**
     * @brief (AR) يعيد الرمز السابق.
     *        (EN) Returns previous token.
     * 
     * @return (AR) الرمز السابق. (EN) Previous token.
     */
    const Lexer::Token& previous() const;

    /**
     * @brief (AR) يبلغ عن خطأ برسالة واقتراح إصلاح.
     *        (EN) Reports error with message and fix-it hint.
     * 
     * @param message (AR) رسالة الخطأ. (EN) Error message.
     * @param fixText (AR) النص المقترح للإصلاح. (EN) Suggested fix text.
     * @param fixDesc_ar (AR) وصف الإصلاح بالعربية. (EN) Arabic fix description.
     * @param fixDesc_en (AR) وصف الإصلاح بالإنجليزية. (EN) English fix description.
     */
    void errorWithFixIt(const std::string& message, 
                        const std::string& fixText,
                        const std::string& fixDesc_ar,
                        const std::string& fixDesc_en);

    /**
     * @brief (AR) يسجل خطأ تحليل مع رسالة ومكان الخطأ.
     *        (EN) Records parsing error with message and location.
     * 
     * @param message (AR) رسالة الخطأ. (EN) Error message.
     */
    void error(const std::string& message);

    /**
     * @brief (AR) يسجل خطأ مع رسالة ثنائية اللغة وطباعة الكود المصدري.
     *        (EN) Records error with bilingual message and prints source code.
     * 
     * @param message_ar (AR) رسالة الخطأ بالعربية.
     * @param message_en (EN) Error message in English.
     * @param showCode (AR) عرض الكود المصدري (افتراضي: true).
     *                 (EN) Show source code (default: true).
     */
    void errorBilingual(const std::string& message_ar, 
                        const std::string& message_en);

    /**
     * @brief (AR) يُنشئ رسالة خطأ لرمز غير متوقع مع التوقع.
     *        (EN) Creates error message for unexpected token with expectation.
     * 
     * @param expected_ar (AR) الرمز المتوقع بالعربية.
     * @param expected_en (EN) Expected token in English.
     * @param context_ar (AR) السياق بالعربية (مثال: "في جملة if").
     * @param context_en (EN) Context in English (e.g., "in if statement").
     */
    void errorExpectedToken(const std::string& expected_ar,
                           const std::string& expected_en,
                           const std::string& context_ar = "",
                           const std::string& context_en = "");

    /**
     * @brief (AR) يُنشئ رسالة خطأ لجملة غير مكتملة.
     *        (EN) Creates error message for incomplete statement.
     * 
     * @param statement_ar (AR) نوع الجملة بالعربية.
     * @param statement_en (EN) Statement type in English.
     * @param missing_ar (AR) العنصر الناقص بالعربية.
     * @param missing_en (EN) Missing element in English.
     */
    void errorIncompleteStatement(const std::string& statement_ar,
                                  const std::string& statement_en,
                                  const std::string& missing_ar,
                                  const std::string& missing_en);

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
     * 
     * @deprecated (AR) استخدم parseTypedParameterList() للحصول على معاملات مكتوبة.
     *             (EN) Use parseTypedParameterList() for typed parameters.
     */
    std::vector<std::string> parseParameterList();

    /**
     * @brief (AR) يحلل قائمة معاملات مكتوبة: (x: int, y: float).
     *        (EN) Parses typed parameter list: (x: int, y: float).
     * 
     * @details (AR) يدعم التنسيقات التالية:
     *               - معاملات بسيطة: (x, y, z) → جميعها UNKNOWN
     *               - معاملات مكتوبة: (x: int, y: float) → بأنواع محددة
     *               - معاملات بقيم افتراضية: (x: int = 10) → قيمة ابتدائية
     *               - مختلط: (x, y: int, z: string = "hello")
     * 
     *          (EN) Supports the following formats:
     *               - Simple parameters: (x, y, z) → all UNKNOWN
     *               - Typed parameters: (x: int, y: float) → with specific types
     *               - Parameters with defaults: (x: int = 10) → initial value
     *               - Mixed: (x, y: int, z: string = "hello")
     * 
     * @return (AR) مصفوفة من كائنات Parameter مع الأسماء والأنواع والقيم الافتراضية.
     *         (EN) Vector of Parameter objects with names, types, and default values.
     * 
     * @example
     * @code{.cpp}
     * // Arabic example / مثال عربي
     * دالة جمع(أ: رقم، ب: رقم) { إرجاع أ + ب; }
     * 
     * // English example
     * function add(a: int, b: int) { return a + b; }
     * @endcode
     */
    std::vector<AST::Parameter> parseTypedParameterList();

    /**
     * @brief (AR) يحلل قائمة وسائط استدعاء دالة (f(1, 2, 3)).
     *        (EN) Parses function call argument list (f(1, 2, 3)).
     * 
     * @return (AR) مصفوفة من تعبيرات الوسائط. (EN) Vector of argument expressions.
     */
    AST::ExprList parseArgumentList();

    // ======================================================================
    // (AR) دوال نظام الأنواع / (EN) Type System Functions
    // ======================================================================

    /**
     * @brief (AR) يحلل نوع بيانات (int, float, string, etc).
     *        (EN) Parses data type (int, float, string, etc).
     * 
     * @details (AR) تدعم هذه الدالة تحليل:
     *               - الأنواع الأساسية: رقم، عشري، نص، منطقي، فراغ
     *               - الأنواع الإنجليزية: int, float, string, bool, void
     *               - الأنواع المركبة: مصفوفة، قاموس (array, dict)
     *               - الأنواع العامة: Array<int>, Map<string, int>
     * 
     *          (EN) This function supports parsing:
     *               - Basic types: رقم، عشري، نص، منطقي، فراغ
     *               - English types: int, float, string, bool, void
     *               - Composite types: مصفوفة، قاموس (array, dict)
     *               - Generic types: Array<int>, Map<string, int>
     * 
     * @return (AR) نوع البيانات المُحلل (UNKNOWN إذا فشل).
     *         (EN) Parsed data type (UNKNOWN if failed).
     * 
     * @throws (AR) لا تُلقي استثناءات، تُسجل الخطأ داخلياً.
     *         (EN) Does not throw, logs error internally.
     * 
     * @example
     * @code{.cpp}
     * // تحليل نوع بسيط / Parse simple type
     * auto type1 = parseType(); // "int" → INTEGER
     * 
     * // تحليل نوع عام / Parse generic type
     * auto type2 = parseType(); // "Array<int>" → ARRAY
     * @endcode
     */
    Data::DataType parseType();

    /**
     * @brief (AR) يحلل نوع عام (Generic Type) مثل Array<T>.
     *        (EN) Parses generic type like Array<T>.
     * 
     * @details (AR) تُستخدم لتحليل الأنواع ذات المعاملات العامة مثل:
     *               - Array<int>
     *               - Map<string, float>
     *               - مصفوفة<رقم>
     *               - قاموس<نص، عشري>
     * 
     *          (EN) Used to parse types with generic parameters like:
     *               - Array<int>
     *               - Map<string, float>
     *               - مصفوفة<رقم>
     *               - قاموس<نص، عشري>
     * 
     * @param baseType (AR) النوع الأساسي (ARRAY, MAP).
     *                 (EN) Base type (ARRAY, MAP).
     * 
     * @return (AR) نوع البيانات مع معلومات المعاملات العامة.
     *         (EN) Data type with generic parameter information.
     * 
     * @note (AR) التنفيذ الحالي يسجل المعاملات العامة لكن لا يستخدمها في runtime.
     *       (EN) Current implementation records generic parameters but doesn't use them at runtime.
     */
    Data::DataType parseGenericType(Data::DataType baseType);

    /**
     * @brief (AR) يفحص ما إذا كان نوع الرمز يمثل نوع بيانات.
     *        (EN) Checks if token type represents a data type.
     * 
     * @param tokenType (AR) نوع الرمز المراد فحصه (EN) Token type to check
     * @return (AR) true إذا كان نوع بيانات (EN) true if it's a data type
     */
    bool isTypeToken(Lexer::TokenType tokenType);
    
    /**
     * @brief (AR) التحقق من أن المعرّف هو اسم صنف مسجّل
     *        (EN) Checks if identifier is a registered class name
     * 
     * @param name (AR) اسم المعرّف (EN) Identifier name
     * @return (AR) true إذا كان اسم صنف (EN) true if it's a class name
     */
    bool isClassName(const std::string& name);
    
    /**
     * @brief (AR) تحليل تعبير إنشاء كائن (جديد صنف())
     *        (EN) Parse object creation expression (جديد Class())
     * 
     * @return (AR) عقدة تعبير (EN) Expression node
     */
    std::unique_ptr<AST::Expr> parseNewExpr();
    
    /**
     * @brief (AR) يحول نوع الرمز إلى نوع بيانات
     *        (EN) Converts token type to data type
     * 
     * @param tokenType (AR) نوع الرمز (EN) Token type
     * @return (AR) نوع البيانات المقابل (EN) Corresponding data type
     */
    Data::DataType mapTokenTypeToDataType(Lexer::TokenType tokenType);

private:
    // ======================================================================
    // (AR) الأعضاء الخاصة / (EN) Private Members
    // ======================================================================

    Lexer::LexerCore& lexer_;           ///< (AR) مرجع للمحلل المعجمي (EN) Reference to lexer
    Lexer::Token current_;               ///< (AR) الرمز الحالي (EN) Current token
    Lexer::Token previous_;              ///< (AR) الرمز السابق (EN) Previous token
    Lexer::Token nextToken_;             ///< (AR) الرمز التالي للنظر المسبق (EN) Next token for lookahead
    bool panicMode_;                     ///< (AR) وضع الذعر للتعافي من الأخطاء (EN) Panic mode for error recovery
    std::string filename_;               ///< (AR) اسم الملف المصدري (EN) Source filename
};

} // namespace Parser
} // namespace Sad
