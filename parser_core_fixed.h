/**
 * @file parser_core.h
 * @brief (AR) ״§„…״­„„ ״§„†״­ˆ ״§„״£״³״§״³ - ״­ˆ„ ״§„״±…ˆ״² (Tokens) ״¥„‰ ״´״¬״±״© ״§„״×״±ƒ״¨ ״§„…״¬״±״¯ (AST).
 *        (EN) Core Parser - Converts tokens into Abstract Syntax Tree (AST).
 * 
 * (AR) ‡״°״§ ״§„…„ ״­״×ˆ ״¹„‰ ״§„…״­„„ ״§„†״­ˆ ״§„״±״¦״³ „„״÷״© "״µ" (Sad).
 *      ‚ˆ… ״§„…״­„„ ״¨‚״±״§״¡״© ״×״¯‚ ״§„״±…ˆ״² …† ״§„…״­„„ ״§„…״¹״¬… (Lexer)
 *      ˆ״¨†״§״¡ ״´״¬״±״© ״§„״×״±ƒ״¨ ״§„…״¬״±״¯ (AST) ״¨״§״³״×״®״¯״§… ״×‚†״© Recursive Descent Parsing.
 *      ״¯״¹… ״§„…״­„„ ״¬…״¹ …״²״§״× ״§„„״÷״© ״¨…״§  ״°„ƒ:
 *      - ״§„״×״¹״¨״±״§״× (״§„״¹…„״§״× ״§„״­״³״§״¨״©״ ״§„…†״·‚״©״ ״§„״§״³״×״¯״¹״§״¡״§״×)
 *      - ״§„״¬…„ (if, while, for, return, ״¥„״®)
 *      - ״§„״×״µ״±״­״§״× (״§„״¯ˆ״§„״ ״§„״£״µ†״§״ ״§„…״×״÷״±״§״×)
 *      - ״§„…״²״§״× ״§„…״×‚״¯…״© (List Comprehensions, Generators, Lambda)
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
 * // ״¥†״´״§״¡ …״­„„ †״­ˆ / Create a parser
 * Lexer::LexerCore lexer(source);
 * Parser::ParserCore parser(lexer);
 * 
 * // ״×״­„„ ״§„״¨״±†״§…״¬ / Parse the program
 * auto program = parser.parseProgram();
 * 
 * // ״§„״×״­‚‚ …† ״§„״£״®״·״§״¡ / Check for errors
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
#include "ast/property_nodes.h"
#include "ast/class_nodes.h"
#include "../errors/error_manager.h"

#include <vector>
#include <memory>
#include <string>

namespace Sad {
namespace Parser {

/**
 * @class ParserCore
 * @brief (AR) ״§„…״­„„ ״§„†״­ˆ ״§„״±״¦״³ „„״÷״© "״µ" - ״¨† ״´״¬״±״© AST …† ״§„״±…ˆ״².
 *        (EN) Main parser for Sad language - builds AST from tokens.
 * 
 * (AR) ״³״×״®״¯… ״×‚†״© Recursive Descent Parsing „״×״­„„ ״§„ƒˆ״¯ ״§„…״µ״¯״±.
 *      ״­״×״¸ ״¨€ current ˆ peek tokens „„…״¹״§†״© ״§„…״³״¨‚״© (lookahead).
 *      ״×״¨״¹ ״£ˆ„ˆ״§״× ״§„״¹ˆ״§…„ ״§„…״­״¯״¯״©  …ˆ״§״µ״§״× ״§„„״÷״©.
 * 
 * (EN) Uses Recursive Descent Parsing technique to analyze source code.
 *      Maintains current and peek tokens for lookahead.
 *      Follows operator precedence defined in language specification.
 */
class ParserCore {
public:
    /**
     * @brief (AR) ״§„״¨†״§״¡ - †״´״¦ …״­„„ †״­ˆ ״¬״¯״¯ …״¹ …״±״¬״¹ „„…״­„„ ״§„…״¹״¬….
     *        (EN) Constructor - Creates a new parser with reference to lexer.
     * 
     * @param lexer (AR) …״±״¬״¹ „„…״­„„ ״§„…״¹״¬… ״§„״° ˆ״± ״§„״±…ˆ״².
     *              (EN) Reference to lexer that provides tokens.
     */
    explicit ParserCore(Lexer::LexerCore& lexer);

    /**
     * @brief (AR) ״§„‡״¯… - †״¸ ״§„…ˆ״§״±״¯ ״§„…״³״×״®״¯…״©.
     *        (EN) Destructor - Cleans up resources.
     */
    ~ParserCore() = default;

    // ======================================================================
    // (AR) ״§„״¯ˆ״§„ ״§„״¹״§…״© / (EN) Public Interface
    // ======================================================================

    /**
     * @brief (AR) ״­„„ ״§„״¨״±†״§…״¬ ״§„ƒ״§…„ - †‚״·״© ״§„״¯״®ˆ„ ״§„״±״¦״³״© „„…״­„„.
     *        (EN) Parses complete program - main entry point for parser.
     * 
     * (AR) ‚ˆ… ״¨״×״­„„ ״¬…״¹ ״§„״×״µ״±״­״§״× ״¹„‰ …״³״×ˆ‰ ״§„״¨״±†״§…״¬ ˆ״¥״±״¬״§״¹ ‚״§״¦…״© …† ״§„״¬…„.
     *      ״³״×…״±  ״§„״×״­„„ ״­״×‰ †‡״§״© ״§„…„ (EOF).
     * 
     * (EN) Parses all top-level declarations and returns list of statements.
     *      Continues parsing until end of file (EOF).
     * 
     * @return (AR) ‚״§״¦…״© …† ״¬…„ ״§„״¨״±†״§…״¬ (״§„״×״µ״±״­״§״×״ ״§„״¬…„ ״§„״×†״°״©).
     *         (EN) List of program statements (declarations, executable statements).
     */
    AST::StmtList parseProgram();

    /**
     * @brief (AR) ״×״­‚‚ ״¥״°״§ ƒ״§†״× ‡†״§ƒ ״£״®״·״§״¡ ״×״­„„.
     *        (EN) Checks if there are any parsing errors.
     * 
     * @return (AR) ״µ״­״­ ״¥״°״§ ˆ״¬״¯״× ״£״®״·״§״¡״ ״®״·״£ ״¨״®„״§ ״°„ƒ.
     *         (EN) True if errors found, false otherwise.
     */
    bool hasErrors() const;

    /**
     * @brief (AR) ״·״¨״¹ ״¬…״¹ ״§„״£״®״·״§״¡ ״¥„‰ ״§„״®״±״¬ ״§„‚״§״³.
     *        (EN) Prints all errors to standard output.
     */
    void printErrors() const;

    /**
     * @brief (AR) ״±״¬״¹ ‚״§״¦…״© ״±״³״§״¦„ ״§„״£״®״·״§״¡.
     *        (EN) Returns list of error messages.
     * 
     * @return (AR) …״µˆ״© …† ״±״³״§״¦„ ״§„״£״®״·״§״¡.
     *         (EN) Vector of error messages.
     */
    std::vector<std::string> getErrors() const;

private:
    // ======================================================================
    // (AR) ״¯ˆ״§„ ״§„״×״­„„ ״§„״£״³״§״³״© / (EN) Core Parsing Functions
    // ======================================================================

    /**
     * @brief (AR) ״­„„ ״×״µ״±״­ ˆ״§״­״¯ (״¯״§„״©״ ״µ†״ …״×״÷״±״ ״¥„״®).
     *        (EN) Parses a single declaration (function, class, variable, etc.).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״§„״×״µ״±״­  AST.
     *         (EN) Pointer to declaration AST node.
     */
    AST::StmtPtr parseDeclaration();

    /**
     * @brief (AR) ״­„„ ״¬…„״© ˆ״§״­״¯״© (if, while, return, ״¥„״®).
     *        (EN) Parses a single statement (if, while, return, etc.).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״§„״¬…„״©  AST.
     *         (EN) Pointer to statement AST node.
     */
    AST::StmtPtr parseStatement();

    // ======================================================================
    // (AR) ״×״­„„ ״§„״×״µ״±״­״§״× / (EN) Declaration Parsing
    // ======================================================================

    /**
     * @brief (AR) ״­„„ ״×״µ״±״­ ״¯״§„״© (״¯״§„״© ״¹״§״¯״©״ ״·״±‚״©״ ״¯״§„״© …ˆ„‘״¯״©).
     *        (EN) Parses function declaration (regular function, method, generator).
     * 
     * @brief (AR) ״­„„ ״×״µ״±״­ ״¯״§„״© (function) …״¹ …״²״®״±״§״× ״§״®״×״§״±״©.
     *        (EN) Parses function declaration with optional decorators.
     * 
     * @param decorators (AR) ‚״§״¦…״© ״§„…״²״®״±״§״× ״§„…״·״¨‚״© ״¹„‰ ״§„״¯״§„״©
     *                   (EN) List of decorators applied to the function
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״µ״±״­ ״§„״¯״§„״©.
     *         (EN) Pointer to function declaration node.
     */
    AST::StmtPtr parseFunctionDecl(AST::ExprList decorators = AST::ExprList());

    /**
     * @brief (AR) ״­„„ ״×״µ״±״­ ״µ† (class) …״¹ ״§„״­‚ˆ„ ˆ״§„״·״±‚.
     *        (EN) Parses class declaration with fields and methods.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״µ״±״­ ״§„״µ†.
     *         (EN) Pointer to class declaration node.
     */
    AST::StmtPtr parseClassDecl();
    
    /**
     * @brief (AR) ״­„„ ״¬…„״© ״§״³״×״±״§״¯ - ״§״³״×ˆ״±״¯ [ˆ״­״¯״©] [ƒ€ ״§״³…]
     *        (EN) Parses import statement - import [module] [as name]
     * 
     * @details
     * (AR) ״¯״¹… ״§„״£†…״§״· ״§„״×״§„״©:
     *      - ״§״³״×ˆ״±״¯ ״±״§״¶״§״×
     *      - ״§״³״×ˆ״±״¯ ״±״§״¶״§״× ƒ€ …
     *      - ״§״³״×ˆ״±״¯ …ƒ״×״¨״×.״£״¯ˆ״§״×.״±״§״¶״©
     * 
     * (EN) Supports the following patterns:
     *      - import math
     *      - import math as m
     *      - import mylib.utils.math
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ImportStmt
     *         (EN) Pointer to ImportStmt node
     * 
     * @example
     * @code{.sad}
     * ״§״³״×ˆ״±״¯ ״±״§״¶״§״×
     * ״§״³״×ˆ״±״¯ ״±״§״¶״§״× ƒ€ …
     * @endcode
     */
    AST::StmtPtr parseImportStmt();
    
    /**
     * @brief (AR) ״­„„ ״¬…„״© ״§״³״×״±״§״¯ ״§†״×‚״§״¦ - …† [ˆ״­״¯״©] ״§״³״×ˆ״±״¯ [״¹†״§״µ״±]
     *        (EN) Parses from-import statement - from [module] import [items]
     * 
     * @details
     * (AR) ״¯״¹… ״§„״£†…״§״· ״§„״×״§„״©:
     *      - …† ״±״§״¶״§״× ״§״³״×ˆ״±״¯ ״¬״°״±״ ‚ˆ״©
     *      - …† ״±״§״¶״§״× ״§״³״×ˆ״±״¯ ״¬״°״± ƒ€ ״¬
     *      - …† ״±״§״¶״§״× ״§״³״×ˆ״±״¯ *
     * 
     * (EN) Supports the following patterns:
     *      - from math import sqrt, pow
     *      - from math import sqrt as s
     *      - from math import *
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© FromImportStmt
     *         (EN) Pointer to FromImportStmt node
     * 
     * @example
     * @code{.sad}
     * …† ״±״§״¶״§״× ״§״³״×ˆ״±״¯ ״¬״°״±״ ‚ˆ״©
     * …† ״±״§״¶״§״× ״§״³״×ˆ״±״¯ *
     * @endcode
     */
    AST::StmtPtr parseFromImportStmt();
    
    /**
     * @brief (AR) ״­„„ ״×״µ״±״­ ״×״µ״¯״± - ״µ״¯‘״± [״×״µ״±״­]
     *        (EN) Parses export declaration - export [declaration]
     * 
     * @details
     * (AR) ״¯״¹… ״×״µ״¯״±:
     *      - ״§„״¯ˆ״§„: ״µ״¯‘״± ״¯״§„״© ״­״³״§״¨() { ... }
     *      - ״§„״£״µ†״§: ״µ״¯‘״± ״µ† ״´ƒ„ { ... }
     *      - ״§„…״×״÷״±״§״×: ״µ״¯‘״± ״«״§״¨״× PI = 3.14
     * 
     * (EN) Supports exporting:
     *      - Functions: export function calc() { ... }
     *      - Classes: export class Shape { ... }
     *      - Variables: export const PI = 3.14
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ExportDecl
     *         (EN) Pointer to ExportDecl node
     * 
     * @example
     * @code{.sad}
     * ״µ״¯‘״± ״¯״§„״© …״¶״§״¹״©(״±‚… ״³) { ״¥״±״¬״§״¹ ״³ * 2 }
     * ״µ״¯‘״± ״«״§״¨״× PI = 3.14
     * @endcode
     */
    AST::StmtPtr parseExportDecl();

    // ======================================================================
    // (AR) ״¯ˆ״§„ ״×״­„„ ״§„״¨״±…״¬״© ״§„ƒ״§״¦†״© / (EN) OOP Parsing Functions
    // ======================================================================

    /**
     * @brief (AR) ״­„„ ״×״µ״±״­ ״®״§״µ״©  ״µ†
     *        (EN) Parses field declaration in class
     */
    std::unique_ptr<AST::FieldDecl> parseFieldDeclaration(AST::AccessModifier access, bool isStatic = false);

    /**
     * @brief (AR) ״­„„ ״×״µ״±״­ ״®״§״µ״© (Property) …״¹ getter/setter
     *        (EN) Parses property declaration with getter/setter
     */
    std::unique_ptr<AST::PropertyDecl> parsePropertyDeclaration(AST::AccessModifier access, bool isStatic = false);

    /**
     * @brief (AR) ״­„„ ״×״µ״±״­ ״·״±‚״©  ״µ†
     *        (EN) Parses method declaration in class
     */
    std::unique_ptr<AST::MethodDecl> parseMethodDeclaration(AST::AccessModifier access, bool isStatic, bool isVirtual, bool isAbstract);

    /**
     * @brief (AR) ״­„„ ״×״µ״±״­ ״¨״§† (constructor)
     *        (EN) Parses constructor declaration
     */
    std::unique_ptr<AST::ConstructorDecl> parseConstructorDeclaration(const std::string& className, AST::AccessModifier access);

    /**
     * @brief (AR) ״­„„ ״×״µ״±״­ ‡״¯״§… (destructor)
     *        (EN) Parses destructor declaration
     */
    std::unique_ptr<AST::DestructorDecl> parseDestructorDeclaration(const std::string& className, AST::AccessModifier access);

    /**
     * @brief (AR) ״­„„ …״¹״¯„״§״× ״§„״±״₪״© (״¹״§…״ ״®״§״µ״ …״­…) ˆ״§„…״¹״¯„״§״× (״«״§״¨״×״ ״§״×״±״§״¶״ …״¬״±״¯)
     *        (EN) Parses visibility modifiers and modifiers (static, virtual, abstract)
     */
    AST::AccessModifier parseModifiers(bool& isStatic, bool& isVirtual, bool& isAbstract);

    /**
     * @brief (AR) ״­„„ ״×״¹״¨״± new „״¥†״´״§״¡ ƒ״§״¦†
     *        (EN) Parses new expression for object instantiation
     */
    AST::ExprPtr parseNewExpression();

    /**
     * @brief (AR) ״­„„ ״×״¹״¨״± this
     *        (EN) Parses this expression
     */
    AST::ExprPtr parseThisExpression();

    /**
     * @brief (AR) ״­„„ ״×״¹״¨״± super
     *        (EN) Parses super expression
     */
    AST::ExprPtr parseSuperExpression();

    /**
     * @brief (AR) ״­„„ ‚״§״¦…״© ״×‡״¦״© ״§„״¨״§†
     *        (EN) Parses constructor initializer list
     */
    std::vector<AST::FieldInitializer> parseInitializerList();

    /**
     * @brief (AR) ״­„„ …״¹״§…„״§״× ״¯״§„״©/״·״±‚״©
     *        (EN) Parses function/method parameters
     */
    std::vector<AST::Parameter> parseParameters();

    /**
     * @brief (AR) ״­„„ ״×״µ״±״­ …״×״÷״± (var x = value).
     *        (EN) Parses variable declaration (var x = value).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״µ״±״­ ״§„…״×״÷״±.
     *         (EN) Pointer to variable declaration node.
     */
    AST::StmtPtr parseVarDecl();

    /**
     * @brief (AR) ״­„„ ״×״µ״±״­ enum …״¹ ‚״§״¦…״© ״§„״£״¹״¶״§״¡.
     *        (EN) Parses enum declaration with member list.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״µ״±״­ Enum.
     *         (EN) Pointer to enum declaration node.
     */
    AST::StmtPtr parseEnumDecl();

    /**
     * @brief (AR) ״­„„ ״¬…„״© ״§״³״×״±״§״¯ (״§״³״×ˆ״±״¯ module).
     *        (EN) Parses import statement (import module).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© ״§„״§״³״×״±״§״¯.
     *         (EN) Pointer to import statement node.
     */
    AST::StmtPtr parseImportStmt();

    /**
     * @brief (AR) ״­„„ ״¬…„״© ״×״µ״¯״± (״µ״¯‘״± function/class).
     *        (EN) Parses export statement (export function/class).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© ״§„״×״µ״¯״±.
     *         (EN) Pointer to export statement node.
     */
    AST::StmtPtr parseExportStmt();

    // ======================================================================
    // (AR) ״×״­„„ ״§„״¬…„ / (EN) Statement Parsing
    // ======================================================================

    /**
     * @brief (AR) ״­„„ ״¬…„״© if …״¹ else/else-if ״§״®״×״§״±״©.
     *        (EN) Parses if statement with optional else/else-if.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© If.
     *         (EN) Pointer to if statement node.
     */
    AST::StmtPtr parseIfStmt();

    /**
     * @brief (AR) ״­„„ ״­„‚״© while.
     *        (EN) Parses while loop.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© While.
     *         (EN) Pointer to while statement node.
     */
    AST::StmtPtr parseWhileStmt();

    /**
     * @brief (AR) ״­„„ ״­„‚״© for („ƒ„ element  collection).
     *        (EN) Parses for loop (for element in collection).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© For.
     *         (EN) Pointer to for statement node.
     */
    AST::StmtPtr parseForStmt();

    /**
     * @brief (AR) ״­„„ ״¬…„״© return …״¹ ‚…״© ״§״®״×״§״±״©.
     *        (EN) Parses return statement with optional value.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© Return.
     *         (EN) Pointer to return statement node.
     */
    AST::StmtPtr parseReturnStmt();

    /**
     * @brief (AR) ״­„„ ״¬…„״© yield („„״¯ˆ״§„ ״§„…ˆ„‘״¯״©).
     *        (EN) Parses yield statement (for generator functions).
     * 
     * Grammar / ״§„‚ˆ״§״¹״¯:
     *   yield_stmt ג†’ "yield" [ "from" ]? expression? ";"
     * 
     * Supports two forms:
     * - yield expr         : yields a single value
     * - yield from iterable: delegates to another generator
     * 
     * ״¯״¹… ״µ״÷״×†:
     * - yield expr         : ״×״¹״· ‚…״© ˆ״§״­״¯״©
     * - yield from iterable: ״×ˆ‘״¶ ״¥„‰ …ˆ„‘״¯ ״¢״®״±
     * 
     * @example Examples / ״£…״«„״©:
     * - yield 42;
     * - yield x * 2;
     * - yield from range(10);
     * - ״§״¹״· 100״›
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© Yield.
     *         (EN) Pointer to yield statement node.
     */
    AST::StmtPtr parseYieldStmt();

    /**
     * @brief (AR) ״­„„ ״¬…„״© break („„״®״±ˆ״¬ …† ״§„״­„‚״§״×).
     *        (EN) Parses break statement (exit loops).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© Break.
     *         (EN) Pointer to break statement node.
     */
    AST::StmtPtr parseBreakStmt();

    /**
     * @brief (AR) ״­„„ ״¬…„״© continue („„…״×״§״¨״¹״© ״¥„‰ ״§„״×ƒ״±״§״± ״§„״×״§„).
     *        (EN) Parses continue statement (skip to next iteration).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© Continue.
     *         (EN) Pointer to continue statement node.
     */
    AST::StmtPtr parseContinueStmt();

    /**
     * @brief (AR) ״­„„ ƒ״×„״© …† ״§„״¬…„ …״­״§״·״© ״¨״£‚ˆ״§״³ …״¹‚ˆ״© { }.
     *        (EN) Parses block of statements enclosed in braces { }.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ƒ״×„״© ״§„״¬…„.
     *         (EN) Pointer to block statement node.
     */
    AST::StmtPtr parseBlockStmt();

    /**
     * @brief (AR) ״­„„ ״¬…„״© try-catch-finally „„״×״¹״§…„ …״¹ ״§„״£״®״·״§״¡.
     *        (EN) Parses try-catch-finally statement for error handling.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© Try.
     *         (EN) Pointer to try statement node.
     */
    AST::StmtPtr parseTryStmt();

    /**
     * @brief (AR) ״­„„ ״¬…„״© raise „״±״¹ ״§״³״×״«†״§״¡.
     *        (EN) Parses raise statement to throw exception.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© Raise.
     *         (EN) Pointer to raise statement node.
     */
    AST::StmtPtr parseRaiseStmt();

    /**
     * @brief (AR) ״­„„ ״¬…„״© with „״¥״¯״§״±״© ״§„…ˆ״§״±״¯.
     *        (EN) Parses with statement for resource management.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© With.
     *         (EN) Pointer to with statement node.
     */
    AST::StmtPtr parseWithStmt();

    /**
     * @brief (AR) ״­„„ ״¬…„״© switch-case „„״×״­ƒ… …״×״¹״¯״¯ ״§„״±ˆ״¹.
     *        (EN) Parses switch-case statement for multi-way branching.
     * 
     * Grammar / ״§„‚ˆ״§״¹״¯:
     *   switch_stmt ג†’ KEYWORD_CASE "(" expr ")"
     *                 (KEYWORD_WHEN expr ":" stmt)*
     *                 [KEYWORD_DEFAULT ":" stmt]
     *                 KEYWORD_END
     * 
     * Syntax / ״§„†״­ˆ:
     *   ״­״§„״© (<expression>)
     *       ״¹†״¯…״§ <value>: <statement>
     *       [״¹†״¯…״§ <value>: <statement>]*
     *       [״§״×״±״§״¶: <statement>]
     *   †‡״§״©
     * 
     * @example Examples / ״£…״«„״©:
     * ״­״§„״© (ˆ…)
     *     ״¹†״¯…״§ 1: ״§״·״¨״¹("״§„״¥״«††")
     *     ״¹†״¯…״§ 2: ״§״·״¨״¹("״§„״«„״§״«״§״¡")
     *     ״§״×״±״§״¶: ״§״·״¨״¹("ˆ… ״¢״®״±")
     * †‡״§״©
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© Switch.
     *         (EN) Pointer to switch statement node.
     * 
     * @note (AR) ״§„״´״±״· ״¬״¨ ״£† ƒˆ† ״¨† ״£‚ˆ״§״³.
     *       (EN) Condition must be enclosed in parentheses.
     * @note (AR) „״§ ˆ״¬״¯ fall-through - †״° ״£ˆ„ ״­״§„״© …״·״§״¨‚״© ‚״·.
     *       (EN) No fall-through - executes only first matching case.
     * @note Spec reference: docs/language_spec/rules/04_syntax.md
     */
    AST::StmtPtr parseSwitchStmt();

    /**
     * @brief (AR) ״­„„ ״¬…„״© ״×״¹״¨״± (״×״¹״¨״± †״×‡ ״¨״§״µ„״© …†‚ˆ״·״©).
     *        (EN) Parses expression statement (expression followed by semicolon).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״¬…„״© ״§„״×״¹״¨״±.
     *         (EN) Pointer to expression statement node.
     */
    AST::StmtPtr parseExpressionStmt();

    // ======================================================================
    // (AR) ״×״­„„ ״§„״×״¹״¨״±״§״× / (EN) Expression Parsing
    // ======================================================================

    /**
     * @brief (AR) ״­„„ ״×״¹״¨״± (†‚״·״© ״¯״®ˆ„ „״×״­„„ ״§„״×״¹״¨״±״§״×).
     *        (EN) Parses expression (entry point for expression parsing).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״§„״×״¹״¨״±.
     *         (EN) Pointer to expression node.
     */
    AST::ExprPtr parseExpression();

    /**
     * @brief (AR) ״­„„ ״¹…„״© ״×״¹† (assignment).
     *        (EN) Parses assignment operation.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״§„״×״¹†.
     *         (EN) Pointer to assignment expression node.
     */
    AST::ExprPtr parseAssignment();

    /**
     * @brief (AR) ״­„„ ״§„״×״¹״¨״± ״§„״«„״§״« ״§„״´״±״· (ternary conditional).
     *        (EN) Parses ternary conditional expression.
     * 
     * Grammar / ״§„‚ˆ״§״¹״¯:
     *   ternary ג†’ logical_or ("?" expression ":" ternary)?
     * 
     * Syntax / ״§„†״­ˆ:
     *   condition ? true_expression : false_expression
     *   ״§„״´״±״· ״ ״×״¹״¨״±_״µ״­״­ : ״×״¹״¨״±_״®״·״£
     * 
     * @example Examples / ״£…״«„״©:
     * x > 0 ? "positive" : "negative"
     * age >= 18 ? "adult" : "minor"
     * ״§„״¹…״± >= 18 ״ "״¨״§„״÷" : "‚״§״µ״±"
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״§„״×״¹״¨״± ״§„״«„״§״« ״£ˆ ״×״¹״¨״± …†״·‚.
     *         (EN) Pointer to ternary expression or logical expression node.
     * 
     * @note Right-associative: a ? b : c ? d : e ג†’ a ? b : (c ? d : e)
     * @note Spec reference: docs/language_spec/rules/04_syntax.md
     */
    AST::ExprPtr parseTernary();

    /**
     * @brief (AR) ״­„„ ״§„״¹ˆ״§…„ ״§„…†״·‚״© (״£ˆ - OR).
     *        (EN) Parses logical OR operators.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״§„״×״¹״¨״± ״§„…†״·‚.
     *         (EN) Pointer to logical expression node.
     */
    AST::ExprPtr parseLogicalOr();

    /**
     * @brief (AR) ״­„„ ״§„״¹ˆ״§…„ ״§„…†״·‚״© (ˆ - AND).
     *        (EN) Parses logical AND operators.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״§„״×״¹״¨״± ״§„…†״·‚.
     *         (EN) Pointer to logical expression node.
     */
    AST::ExprPtr parseLogicalAnd();

    /**
     * @brief (AR) ״­„„ ״¹ˆ״§…„ ״§„…״³״§ˆ״§״© (== !=).
     *        (EN) Parses equality operators (== !=).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״§„…‚״§״±†״©.
     *         (EN) Pointer to comparison expression node.
     */
    AST::ExprPtr parseEquality();

    /**
     * @brief (AR) ״­„„ ״¹ˆ״§…„ ״§„…‚״§״±†״© (< <= > >=).
     *        (EN) Parses comparison operators (< <= > >=).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״§„…‚״§״±†״©.
     *         (EN) Pointer to comparison expression node.
     */
    AST::ExprPtr parseComparison();

    /**
     * @brief (AR) ״­„„ ״¹ˆ״§…„ ״§„״¬…״¹ ˆ״§„״·״±״­ (+ -).
     *        (EN) Parses addition and subtraction operators (+ -).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״§„״¬…״¹/״§„״·״±״­.
     *         (EN) Pointer to addition/subtraction expression node.
     */
    AST::ExprPtr parseTerm();

    /**
     * @brief (AR) ״­„„ ״¹ˆ״§…„ ״§„״¶״±״¨ ˆ״§„‚״³…״© (* / %).
     *        (EN) Parses multiplication and division operators (* / %).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״§„״¶״±״¨/״§„‚״³…״©.
     *         (EN) Pointer to multiplication/division expression node.
     */
    AST::ExprPtr parseFactor();

    /**
     * @brief (AR) ״­„„ ״§„״¹ˆ״§…„ ״§„״£״­״§״¯״© (- ! ++ --).
     *        (EN) Parses unary operators (- ! ++ --).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״£״­״§״¯.
     *         (EN) Pointer to unary expression node.
     */
    AST::ExprPtr parseUnary();

    /**
     * @brief (AR) ״­„„ ״¹״§…„ ״§„״£״³ (**).
     *        (EN) Parses power operator (**).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״§„״£״³.
     *         (EN) Pointer to power expression node.
     */
    AST::ExprPtr parsePower();

    /**
     * @brief (AR) ״­„„ ״§״³״×״¯״¹״§״¡״§״× ״§„״¯ˆ״§„ ˆ״§„ˆ״µˆ„ „„״£״¹״¶״§״¡ (״§„‡״±״³״©).
     *        (EN) Parses function calls and member access (indexing).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״§„״§״³״×״¯״¹״§״¡/״§„ˆ״µˆ„.
     *         (EN) Pointer to call/access expression node.
     */
    AST::ExprPtr parsePostfix();

    /**
     * @brief (AR) ״­„„ ״§„״×״¹״¨״±״§״× ״§„״£״³״§״³״© (״§„״£״±‚״§…״ ״§„†״µˆ״µ״ ״§„…״×״÷״±״§״×״ ״¥„״®).
     *        (EN) Parses primary expressions (numbers, strings, variables, etc.).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״£״³״§״³.
     *         (EN) Pointer to primary expression node.
     */
    AST::ExprPtr parsePrimary();

    /**
     * @brief (AR) ״­„„ ״¯״§„״© „״§…״¯״§ (lambda x: x + 1).
     *        (EN) Parses lambda function (lambda x: x + 1).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± „״§…״¯״§.
     *         (EN) Pointer to lambda expression node.
     */
    AST::ExprPtr parseLambda();
    
    /**
     * @brief (AR) ״­„„ …״²״®״± (decorator): @decorator ״£ˆ @decorator(args).
     *        (EN) Parses decorator: @decorator or @decorator(args).
     * 
     * Grammar / ״§„‚ˆ״§״¹״¯:
     *   decorator ::= '@' identifier ['(' [argument_list] ')']
     * 
     * Examples / ״£…״«„״©:
     *   - @staticmethod
     *   - @cache(maxsize=100)
     *   - @retry(times=3, delay=1.5)
     *   - @…״²״®״±
     *   - @״×״®״²†_…״₪‚״×(״­״¬…=100)
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״§„…״²״®״±.
     *         (EN) Pointer to decorator expression node.
     * 
     * @note (AR) ״¬״¨ ״£† ƒˆ† ״§„״±…״² ״§„״­״§„ ‡ˆ AT_SIGN (@)
     * @note (EN) Current token must be AT_SIGN (@)
     */
    AST::ExprPtr parseDecorator();

    /**
     * @brief (AR) ״­„„ arrow function: (x, y) => x + y.
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
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± arrow function (LambdaExpr).
     *         (EN) Pointer to arrow function expression node (LambdaExpr).
     */
    AST::ExprPtr parseArrowFunction();

    /**
     * @brief (AR) ״×״­‚‚ ״¥״°״§ ƒ״§† ״§„״×״³„״³„ ״§„״­״§„ arrow function.
     *        (EN) Checks if current sequence is arrow function.
     * 
     * Lookahead patterns:
     *   - identifier '=>'  (e.g., x => x * 2)
     *   - '(' ... ')' '=>'  (e.g., (x, y) => x + y)
     * 
     * Note: This function uses lookahead and restores parser position.
     * 
     * @return (AR) true ״¥״°״§ ƒ״§† arrow function״ ˆ״¥„״§ false.
     *         (EN) true if arrow function, false otherwise.
     */
    bool isArrowFunction();

    /**
     * @brief (AR) ״­„„ List Comprehension ([x*2 for x in list]).
     *        (EN) Parses list comprehension ([x*2 for x in list]).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© List Comprehension.
     *         (EN) Pointer to list comprehension node.
     */
    AST::ExprPtr parseListComprehension();

    /**
     * @brief (AR) ״­„„ Dictionary Comprehension ({k: v for k, v in dict}).
     *        (EN) Parses dictionary comprehension ({k: v for k, v in dict}).
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© Dict Comprehension.
     *         (EN) Pointer to dict comprehension node.
     */
    AST::ExprPtr parseDictComprehension();

    /**
     * @brief (AR) ״­„„ …״µˆ״© ״­״±״© [1, 2, 3].
     *        (EN) Parses array literal [1, 2, 3].
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״§„…״µˆ״©.
     *         (EN) Pointer to array expression node.
     */
    AST::ExprPtr parseArrayLiteral();

    /**
     * @brief (AR) ״­„„ ״®״±״·״© ״­״±״© {key: value}.
     *        (EN) Parses map literal {key: value}.
     * 
     * @return (AR) …״₪״´״± „״¹‚״¯״© ״×״¹״¨״± ״§„״®״±״·״©.
     *         (EN) Pointer to map expression node.
     */
    AST::ExprPtr parseMapLiteral();

    // ======================================================================
    // (AR) ״¯ˆ״§„ …״³״§״¹״¯״© / (EN) Helper Functions
    // ======================================================================

    /**
     * @brief (AR) ״×‚״¯… ״¥„‰ ״§„״±…״² ״§„״×״§„.
     *        (EN) Advances to next token.
     */
    void advance();

    /**
     * @brief (AR) ״×״­‚‚ ״¥״°״§ ƒ״§† ״§„״±…״² ״§„״­״§„ …† ״§„†ˆ״¹ ״§„…״­״¯״¯.
     *        (EN) Checks if current token is of specified type.
     * 
     * @param type (AR) †ˆ״¹ ״§„״±…״² ״§„…״×ˆ‚״¹. (EN) Expected token type.
     * @return (AR) ״µ״­״­ ״¥״°״§ ״×״·״§״¨‚ ״§„†ˆ״¹. (EN) True if type matches.
     */
    bool check(Lexer::TokenType type) const;

    /**
     * @brief (AR) ״×״­‚‚ ˆ״×‚״¯… ״¥״°״§ ƒ״§† ״§„״±…״² ״§„״­״§„ …† ״§„†ˆ״¹ ״§„…״­״¯״¯.
     *        (EN) Checks and advances if current token is of specified type.
     * 
     * @param type (AR) †ˆ״¹ ״§„״±…״² ״§„…״×ˆ‚״¹. (EN) Expected token type.
     * @return (AR) ״µ״­״­ ״¥״°״§ ״×… ״§„״×״·״§״¨‚ ˆ״§„״×‚״¯…. (EN) True if matched and advanced.
     */
    bool match(Lexer::TokenType type);

    /**
     * @brief (AR) ״×״­‚‚ ˆ״×‚״¯… ״¥״°״§ ƒ״§† ״§„״±…״² ״§„״­״§„ …† ״£״­״¯ ״§„״£†ˆ״§״¹ ״§„…״­״¯״¯״©.
     *        (EN) Checks and advances if current token is one of specified types.
     * 
     * @param types (AR) ‚״§״¦…״© ״£†ˆ״§״¹ ״§„״±…ˆ״² ״§„…״­״×…„״©. (EN) List of possible token types.
     * @return (AR) ״µ״­״­ ״¥״°״§ ״×… ״§„״×״·״§״¨‚. (EN) True if matched.
     */
    bool matchAny(const std::vector<Lexer::TokenType>& types);

    /**
     * @brief (AR) ״³״×‡„ƒ ״±…״²״§‹ …† ״§„†ˆ״¹ ״§„…״­״¯״¯ ״£ˆ ״±״¹ ״®״·״£.
     *        (EN) Consumes token of specified type or raises error.
     * 
     * @param type (AR) †ˆ״¹ ״§„״±…״² ״§„…״×ˆ‚״¹. (EN) Expected token type.
     * @param message (AR) ״±״³״§„״© ״§„״®״·״£ ״¥״°״§ „… ״×״·״§״¨‚. (EN) Error message if not matched.
     * @return (AR) ״§„״±…״² ״§„…״³״×‡„ƒ. (EN) Consumed token.
     */
    Lexer::Token consume(Lexer::TokenType type, const std::string& message);

    /**
     * @brief (AR) ״×״­‚‚ ״¥״°״§ ˆ״µ„†״§ „†‡״§״© ״§„…„.
     *        (EN) Checks if we reached end of file.
     * 
     * @return (AR) ״µ״­״­ ״¥״°״§ ˆ״µ„†״§ „„†‡״§״©. (EN) True if at end.
     */
    bool isAtEnd() const;

    /**
     * @brief (AR) ״¹״¯ ״§„״±…״² ״§„״­״§„.
     *        (EN) Returns current token.
     * 
     * @return (AR) ״§„״±…״² ״§„״­״§„. (EN) Current token.
     */
    const Lexer::Token& peek() const;

/**
     * @brief (AR) ״¹״¯ ״§„״±…״² ״§„״×״§„.
     *        (EN) Returns next token.
     * 
     * @return (AR) ״§„״±…״² ״§„״×״§„. (EN) Next token.
     */
    const Lexer::Token& peekNext() const;

    /**
     * @brief (AR) ״¹״¯ ״§„״±…״² ״§„״³״§״¨‚.
     *        (EN) Returns previous token.
     * 
     * @return (AR) ״§„״±…״² ״§„״³״§״¨‚. (EN) Previous token.
     */
    const Lexer::Token& previous() const;

    /**
     * @brief (AR) ״¨„״÷ ״¹† ״®״·״£ ״¨״±״³״§„״© ˆ״§‚״×״±״§״­ ״¥״µ„״§״­.
     *        (EN) Reports error with message and fix-it hint.
     * 
     * @param message (AR) ״±״³״§„״© ״§„״®״·״£. (EN) Error message.
     * @param fixText (AR) ״§„†״µ ״§„…‚״×״±״­ „„״¥״µ„״§״­. (EN) Suggested fix text.
     * @param fixDesc_ar (AR) ˆ״µ ״§„״¥״µ„״§״­ ״¨״§„״¹״±״¨״©. (EN) Arabic fix description.
     * @param fixDesc_en (AR) ˆ״µ ״§„״¥״µ„״§״­ ״¨״§„״¥†״¬„״²״©. (EN) English fix description.
     */
    void errorWithFixIt(const std::string& message, 
                        const std::string& fixText,
                        const std::string& fixDesc_ar,
                        const std::string& fixDesc_en);

    /**
     * @brief (AR) ״³״¬„ ״®״·״£ ״×״­„„ …״¹ ״±״³״§„״© ˆ…ƒ״§† ״§„״®״·״£.
     *        (EN) Records parsing error with message and location.
     * 
     * @param message (AR) ״±״³״§„״© ״§„״®״·״£. (EN) Error message.
     */
    void error(const std::string& message);

    /**
     * @brief (AR) ״³״¬„ ״®״·״£ …״¹ ״±״³״§„״© ״«†״§״¦״© ״§„„״÷״© ˆ״·״¨״§״¹״© ״§„ƒˆ״¯ ״§„…״µ״¯״±.
     *        (EN) Records error with bilingual message and prints source code.
     * 
     * @param message_ar (AR) ״±״³״§„״© ״§„״®״·״£ ״¨״§„״¹״±״¨״©.
     * @param message_en (EN) Error message in English.
     * @param showCode (AR) ״¹״±״¶ ״§„ƒˆ״¯ ״§„…״µ״¯״± (״§״×״±״§״¶: true).
     *                 (EN) Show source code (default: true).
     */
    void errorBilingual(const std::string& message_ar, 
                        const std::string& message_en,
                        bool showCode = true);

    /**
     * @brief (AR) †״´״¦ ״±״³״§„״© ״®״·״£ „״±…״² ״÷״± …״×ˆ‚״¹ …״¹ ״§„״×ˆ‚״¹.
     *        (EN) Creates error message for unexpected token with expectation.
     * 
     * @param expected_ar (AR) ״§„״±…״² ״§„…״×ˆ‚״¹ ״¨״§„״¹״±״¨״©.
     * @param expected_en (EN) Expected token in English.
     * @param context_ar (AR) ״§„״³״§‚ ״¨״§„״¹״±״¨״© (…״«״§„: " ״¬…„״© if").
     * @param context_en (EN) Context in English (e.g., "in if statement").
     */
    void errorExpectedToken(const std::string& expected_ar,
                           const std::string& expected_en,
                           const std::string& context_ar = "",
                           const std::string& context_en = "");

    /**
     * @brief (AR) †״´״¦ ״±״³״§„״© ״®״·״£ „״¬…„״© ״÷״± …ƒ״×…„״©.
     *        (EN) Creates error message for incomplete statement.
     * 
     * @param statement_ar (AR) †ˆ״¹ ״§„״¬…„״© ״¨״§„״¹״±״¨״©.
     * @param statement_en (EN) Statement type in English.
     * @param missing_ar (AR) ״§„״¹†״µ״± ״§„†״§‚״µ ״¨״§„״¹״±״¨״©.
     * @param missing_en (EN) Missing element in English.
     */
    void errorIncompleteStatement(const std::string& statement_ar,
                                  const std::string& statement_en,
                                  const std::string& missing_ar,
                                  const std::string& missing_en);

    /**
     * @brief (AR) ״­״§ˆ„ ״§„״×״¹״§ …† ״®״·״£ ״§„״×״­„„ ״¨״§„״§†״×‚״§„ „„״¬…„״© ״§„״×״§„״©.
     *        (EN) Attempts to recover from parsing error by advancing to next statement.
     */
    void synchronize();

    /**
     * @brief (AR) ״­„„ ‚״§״¦…״© …״¹״§…„״§״× ״§„״¯״§„״© (x, y, z).
     *        (EN) Parses function parameter list (x, y, z).
     * 
     * @return (AR) …״µˆ״© …† ״£״³…״§״¡ ״§„…״¹״§…„״§״×. (EN) Vector of parameter names.
     * 
     * @deprecated (AR) ״§״³״×״®״¯… parseTypedParameterList() „„״­״µˆ„ ״¹„‰ …״¹״§…„״§״× …ƒ״×ˆ״¨״©.
     *             (EN) Use parseTypedParameterList() for typed parameters.
     */
    std::vector<std::string> parseParameterList();

    /**
     * @brief (AR) ״­„„ ‚״§״¦…״© …״¹״§…„״§״× …ƒ״×ˆ״¨״©: (x: int, y: float).
     *        (EN) Parses typed parameter list: (x: int, y: float).
     * 
     * @details (AR) ״¯״¹… ״§„״×†״³‚״§״× ״§„״×״§„״©:
     *               - …״¹״§…„״§״× ״¨״³״·״©: (x, y, z) ג†’ ״¬…״¹‡״§ UNKNOWN
     *               - …״¹״§…„״§״× …ƒ״×ˆ״¨״©: (x: int, y: float) ג†’ ״¨״£†ˆ״§״¹ …״­״¯״¯״©
     *               - …״¹״§…„״§״× ״¨‚… ״§״×״±״§״¶״©: (x: int = 10) ג†’ ‚…״© ״§״¨״×״¯״§״¦״©
     *               - …״®״×„״·: (x, y: int, z: string = "hello")
     * 
     *          (EN) Supports the following formats:
     *               - Simple parameters: (x, y, z) ג†’ all UNKNOWN
     *               - Typed parameters: (x: int, y: float) ג†’ with specific types
     *               - Parameters with defaults: (x: int = 10) ג†’ initial value
     *               - Mixed: (x, y: int, z: string = "hello")
     * 
     * @return (AR) …״µˆ״© …† ƒ״§״¦†״§״× Parameter …״¹ ״§„״£״³…״§״¡ ˆ״§„״£†ˆ״§״¹ ˆ״§„‚… ״§„״§״×״±״§״¶״©.
     *         (EN) Vector of Parameter objects with names, types, and default values.
     * 
     * @example
     * @code{.cpp}
     * // Arabic example / …״«״§„ ״¹״±״¨
     * ״¯״§„״© ״¬…״¹(״£: ״±‚…״ ״¨: ״±‚…) { ״¥״±״¬״§״¹ ״£ + ״¨; }
     * 
     * // English example
     * function add(a: int, b: int) { return a + b; }
     * @endcode
     */
    std::vector<AST::Parameter> parseTypedParameterList();

    /**
     * @brief (AR) ״­„„ ‚״§״¦…״© ˆ״³״§״¦״· ״§״³״×״¯״¹״§״¡ ״¯״§„״© (f(1, 2, 3)).
     *        (EN) Parses function call argument list (f(1, 2, 3)).
     * 
     * @return (AR) …״µˆ״© …† ״×״¹״¨״±״§״× ״§„ˆ״³״§״¦״·. (EN) Vector of argument expressions.
     */
    AST::ExprList parseArgumentList();

    // ======================================================================
    // (AR) ״¯ˆ״§„ †״¸״§… ״§„״£†ˆ״§״¹ / (EN) Type System Functions
    // ======================================================================

    /**
     * @brief (AR) ״­„„ †ˆ״¹ ״¨״§†״§״× (int, float, string, etc).
     *        (EN) Parses data type (int, float, string, etc).
     * 
     * @details (AR) ״×״¯״¹… ‡״°‡ ״§„״¯״§„״© ״×״­„„:
     *               - ״§„״£†ˆ״§״¹ ״§„״£״³״§״³״©: ״±‚…״ ״¹״´״±״ †״µ״ …†״·‚״ ״±״§״÷
     *               - ״§„״£†ˆ״§״¹ ״§„״¥†״¬„״²״©: int, float, string, bool, void
     *               - ״§„״£†ˆ״§״¹ ״§„…״±ƒ״¨״©: …״µˆ״©״ ‚״§…ˆ״³ (array, dict)
     *               - ״§„״£†ˆ״§״¹ ״§„״¹״§…״©: Array<int>, Map<string, int>
     * 
     *          (EN) This function supports parsing:
     *               - Basic types: ״±‚…״ ״¹״´״±״ †״µ״ …†״·‚״ ״±״§״÷
     *               - English types: int, float, string, bool, void
     *               - Composite types: …״µˆ״©״ ‚״§…ˆ״³ (array, dict)
     *               - Generic types: Array<int>, Map<string, int>
     * 
     * @return (AR) †ˆ״¹ ״§„״¨״§†״§״× ״§„…״­„„ (UNKNOWN ״¥״°״§ ״´„).
     *         (EN) Parsed data type (UNKNOWN if failed).
     * 
     * @throws (AR) „״§ ״×„‚ ״§״³״×״«†״§״¡״§״×״ ״×״³״¬„ ״§„״®״·״£ ״¯״§״®„״§‹.
     *         (EN) Does not throw, logs error internally.
     * 
     * @example
     * @code{.cpp}
     * // ״×״­„„ †ˆ״¹ ״¨״³״· / Parse simple type
     * auto type1 = parseType(); // "int" ג†’ INTEGER
     * 
     * // ״×״­„„ †ˆ״¹ ״¹״§… / Parse generic type
     * auto type2 = parseType(); // "Array<int>" ג†’ ARRAY
     * @endcode
     */
    Data::DataType parseType();

    /**
     * @brief (AR) ״­„„ †ˆ״¹ ״¹״§… (Generic Type) …״«„ Array<T>.
     *        (EN) Parses generic type like Array<T>.
     * 
     * @details (AR) ״×״³״×״®״¯… „״×״­„„ ״§„״£†ˆ״§״¹ ״°״§״× ״§„…״¹״§…„״§״× ״§„״¹״§…״© …״«„:
     *               - Array<int>
     *               - Map<string, float>
     *               - …״µˆ״©<״±‚…>
     *               - ‚״§…ˆ״³<†״µ״ ״¹״´״±>
     * 
     *          (EN) Used to parse types with generic parameters like:
     *               - Array<int>
     *               - Map<string, float>
     *               - …״µˆ״©<״±‚…>
     *               - ‚״§…ˆ״³<†״µ״ ״¹״´״±>
     * 
     * @param baseType (AR) ״§„†ˆ״¹ ״§„״£״³״§״³ (ARRAY, MAP).
     *                 (EN) Base type (ARRAY, MAP).
     * 
     * @return (AR) †ˆ״¹ ״§„״¨״§†״§״× …״¹ …״¹„ˆ…״§״× ״§„…״¹״§…„״§״× ״§„״¹״§…״©.
     *         (EN) Data type with generic parameter information.
     * 
     * @note (AR) ״§„״×†״° ״§„״­״§„ ״³״¬„ ״§„…״¹״§…„״§״× ״§„״¹״§…״© „ƒ† „״§ ״³״×״®״¯…‡״§  runtime.
     *       (EN) Current implementation records generic parameters but doesn't use them at runtime.
     */
    Data::DataType parseGenericType(Data::DataType baseType);

    /**
     * @brief (AR) ״­״µ …״§ ״¥״°״§ ƒ״§† †ˆ״¹ ״§„״±…״² …״«„ †ˆ״¹ ״¨״§†״§״×.
     *        (EN) Checks if token type represents a data type.
     * 
     * @param tokenType (AR) †ˆ״¹ ״§„״±…״² ״§„…״±״§״¯ ״­״µ‡ (EN) Token type to check
     * @return (AR) true ״¥״°״§ ƒ״§† †ˆ״¹ ״¨״§†״§״× (EN) true if it's a data type
     */
    bool isTypeToken(Lexer::TokenType tokenType);
    
    /**
     * @brief (AR) ״§„״×״­‚‚ …† ״£† ״§„…״¹״±‘ ‡ˆ ״§״³… ״µ† …״³״¬‘„
     *        (EN) Checks if identifier is a registered class name
     * 
     * @param name (AR) ״§״³… ״§„…״¹״±‘ (EN) Identifier name
     * @return (AR) true ״¥״°״§ ƒ״§† ״§״³… ״µ† (EN) true if it's a class name
     */
    bool isClassName(const std::string& name);
    
    /**
     * @brief (AR) ״×״­„„ ״×״¹״¨״± ״¥†״´״§״¡ ƒ״§״¦† (״¬״¯״¯ ״µ†())
     *        (EN) Parse object creation expression (״¬״¯״¯ Class())
     * 
     * @return (AR) ״¹‚״¯״© ״×״¹״¨״± (EN) Expression node
     */
    std::unique_ptr<AST::Expr> parseNewExpr();
    
    /**
     * @brief (AR) ״­ˆ„ †ˆ״¹ ״§„״±…״² ״¥„‰ †ˆ״¹ ״¨״§†״§״×
     *        (EN) Converts token type to data type
     * 
     * @param tokenType (AR) †ˆ״¹ ״§„״±…״² (EN) Token type
     * @return (AR) †ˆ״¹ ״§„״¨״§†״§״× ״§„…‚״§״¨„ (EN) Corresponding data type
     */
    Data::DataType mapTokenTypeToDataType(Lexer::TokenType tokenType);

private:
    // ======================================================================
    // (AR) ״§„״£״¹״¶״§״¡ ״§„״®״§״µ״© / (EN) Private Members
    // ======================================================================

    Lexer::LexerCore& lexer_;           ///< (AR) …״±״¬״¹ „„…״­„„ ״§„…״¹״¬… (EN) Reference to lexer
    Lexer::Token current_;               ///< (AR) ״§„״±…״² ״§„״­״§„ (EN) Current token
    Lexer::Token previous_;              ///< (AR) ״§„״±…״² ״§„״³״§״¨‚ (EN) Previous token
    Lexer::Token nextToken_;             ///< (AR) ״§„״±…״² ״§„״×״§„ „„†״¸״± ״§„…״³״¨‚ (EN) Next token for lookahead
    bool panicMode_;                     ///< (AR) ˆ״¶״¹ ״§„״°״¹״± „„״×״¹״§ …† ״§„״£״®״·״§״¡ (EN) Panic mode for error recovery
    std::string filename_;               ///< (AR) ״§״³… ״§„…„ ״§„…״µ״¯״± (EN) Source filename
};

} // namespace Parser
} // namespace Sad
