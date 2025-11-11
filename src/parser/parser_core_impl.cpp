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

#include "../../include/parser/parser_core.h"
#include <iostream>
#include <sstream>

namespace Sad {
namespace Parser {

using namespace AST;
using namespace Lexer;
using TT = TokenType;

// ======================================================================
// (AR) البناء والهدم / (EN) Construction and Destruction
// ======================================================================

/**
 * @brief (AR) ينشئ محلل نحوي جديد مع مرجع للمحلل المعجمي.
 *        (EN) Creates new parser with reference to lexer.
 */
ParserCore::ParserCore(LexerCore& lexer)
    : lexer_(lexer)
    , current_(TT::END_OF_FILE, "")  // Initialize with dummy token
    , previous_(TT::END_OF_FILE, "") // Initialize with dummy token
    , panicMode_(false)
{
    // Initialize by fetching first token
    // (AR) التهيئة بجلب الرمز الأول
    advance();
}

// ======================================================================
// (AR) الدوال العامة / (EN) Public Interface
// ======================================================================

/**
 * @brief (AR) يحلل البرنامج الكامل ويرجع قائمة جمل.
 *        (EN) Parses complete program and returns statement list.
 */
StmtList ParserCore::parseProgram() {
    StmtList statements;
    
    // Parse until EOF
    // (AR) التحليل حتى نهاية الملف
    while (!isAtEnd()) {
        try {
            auto stmt = parseDeclaration();
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
        } catch (const std::exception& e) {
            error(e.what());
            synchronize();
        }
    }
    
    return statements;
}

/**
 * @brief (AR) يتحقق من وجود أخطاء تحليل.
 *        (EN) Checks for parsing errors.
 */
bool ParserCore::hasErrors() const {
    return !errors_.empty();
}

/**
 * @brief (AR) يطبع جميع الأخطاء.
 *        (EN) Prints all errors.
 */
void ParserCore::printErrors() const {
    for (const auto& err : errors_) {
        std::cerr << err << std::endl;
    }
}

/**
 * @brief (AR) يرجع قائمة الأخطاء.
 *        (EN) Returns error list.
 */
std::vector<std::string> ParserCore::getErrors() const {
    return errors_;
}

// ======================================================================
// (AR) دوال التحليل الأساسية / (EN) Core Parsing Functions
// ======================================================================

/**
 * @brief (AR) يحلل تصريح واحد (دالة، صنف، متغير، import، export).
 *        (EN) Parses single declaration (function, class, variable, import, export).
 */
StmtPtr ParserCore::parseDeclaration() {
    // Check for declaration keywords
    // (AR) التحقق من كلمات التصريح المفتاحية
    
    if (match(TT::KEYWORD_FUNCTION)) {
        return parseFunctionDecl();
    }
    
    if (match(TT::KEYWORD_CLASS)) {
        return parseClassDecl();
    }
    
    if (match(TT::KEYWORD_VAR) || match(TT::KEYWORD_LET) || match(TT::KEYWORD_CONST)) {
        return parseVarDecl();
    }
    
    if (match(TT::KEYWORD_ENUM)) {
        return parseEnumDecl();
    }
    
    if (match(TT::KEYWORD_IMPORT)) {
        return parseImportStmt();
    }
    
    if (match(TT::KEYWORD_EXPORT)) {
        return parseExportStmt();
    }
    
    // If no declaration keyword, parse as statement
    // (AR) إذا لم توجد كلمة تصريح، حلل كجملة
    return parseStatement();
}

/**
 * @brief (AR) يحلل جملة (if، while، for، return، إلخ).
 *        (EN) Parses statement (if, while, for, return, etc.).
 */
StmtPtr ParserCore::parseStatement() {
    // Control flow statements
    // (AR) جمل التحكم في التدفق
    
    if (match(TT::KEYWORD_IF)) {
        return parseIfStmt();
    }
    
    if (match(TT::KEYWORD_WHILE)) {
        return parseWhileStmt();
    }
    
    if (match(TT::KEYWORD_FOR)) {
        return parseForStmt();
    }
    
    if (match(TT::KEYWORD_RETURN)) {
        return parseReturnStmt();
    }
    
    if (match(TT::KEYWORD_BREAK)) {
        return parseBreakStmt();
    }
    
    if (match(TT::KEYWORD_CONTINUE)) {
        return parseContinueStmt();
    }
    
    if (match(TT::BRACE_LEFT)) {
        return parseBlockStmt();
    }
    
    if (match(TT::KEYWORD_TRY)) {
        return parseTryStmt();
    }
    
    if (match(TT::KEYWORD_THROW)) {
        return parseRaiseStmt();
    }
    
    // Default: expression statement
    // (AR) الافتراضي: جملة تعبير
    return parseExpressionStmt();
}

// ======================================================================
// (AR) تحليل التصريحات / (EN) Declaration Parsing
// ======================================================================

/**
 * @brief (AR) يحلل تصريح دالة: دالة اسم(معاملات) { جسم }.
 *        (EN) Parses function declaration: function name(params) { body }.
 */
StmtPtr ParserCore::parseFunctionDecl() {
    // Expect function name
    // (AR) توقع اسم الدالة
    Token name = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم الدالة. (EN) Expected function name.");
    
    // Parse parameter list
    // (AR) تحليل قائمة المعاملات
    consume(TT::PAREN_LEFT, 
        "(AR) توقع '(' بعد اسم الدالة. (EN) Expected '(' after function name.");
    auto params = parseParameterList();
    consume(TT::PAREN_RIGHT, 
        "(AR) توقع ')' بعد المعاملات. (EN) Expected ')' after parameters.");
    
    // Parse function body
    // (AR) تحليل جسم الدالة
    consume(TT::BRACE_LEFT, 
        "(AR) توقع '{' قبل جسم الدالة. (EN) Expected '{' before function body.");
    auto body = parseBlockStmt();
    
    // Create function declaration node
    // (AR) إنشاء عقدة تصريح الدالة
    return std::make_unique<FunctionDecl>(
        name.getValue(),
        params,
        std::move(body),
        name.getPosition()
    );
}

/**
 * @brief (AR) يحلل تصريح صنف: صنف اسم { حقول وطرق }.
 *        (EN) Parses class declaration: class name { fields and methods }.
 */
StmtPtr ParserCore::parseClassDecl() {
    // Expect class name
    // (AR) توقع اسم الصنف
    Token name = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم الصنف. (EN) Expected class name.");
    
    // Check for inheritance
    // (AR) التحقق من الوراثة
    std::string superclass;
    if (match(TT::KEYWORD_EXTENDS)) {
        Token super = consume(TT::IDENTIFIER, 
            "(AR) توقع اسم الصنف الأب. (EN) Expected superclass name.");
        superclass = super.getValue();
    }
    
    // Parse class body
    // (AR) تحليل جسم الصنف
    consume(TT::BRACE_LEFT, 
        "(AR) توقع '{' قبل جسم الصنف. (EN) Expected '{' before class body.");
    
    StmtList members;
    while (!check(TT::BRACE_RIGHT) && !isAtEnd()) {
        // Parse class members (fields, methods, constructor)
        // (AR) تحليل أعضاء الصنف (حقول، طرق، بناء)
        auto member = parseDeclaration();
        if (member) {
            members.push_back(std::move(member));
        }
    }
    
    consume(TT::BRACE_RIGHT, 
        "(AR) توقع '}' بعد جسم الصنف. (EN) Expected '}' after class body.");
    
    // Create class declaration node
    // (AR) إنشاء عقدة تصريح الصنف
    return std::make_unique<ClassDecl>(
        name.getValue(),
        superclass,
        std::move(members),
        name.getPosition()
    );
}

/**
 * @brief (AR) يحلل تصريح متغير: متغير اسم = قيمة;
 *        (EN) Parses variable declaration: var name = value;
 */
StmtPtr ParserCore::parseVarDecl() {
    // Expect variable name
    // (AR) توقع اسم المتغير
    Token name = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم المتغير. (EN) Expected variable name.");
    
    // Optional initializer
    // (AR) المُهيّئ الاختياري
    ExprPtr initializer = nullptr;
    if (match(TT::OP_ASSIGN)) {
        initializer = parseExpression();
    }
    
    consume(TT::SEMICOLON, 
        "(AR) توقع ';' بعد تصريح المتغير. (EN) Expected ';' after variable declaration.");
    
    // Create variable declaration statement
    // (AR) إنشاء جملة تصريح المتغير
    return std::make_unique<VarDeclStmt>(
        name.getValue(),
        std::move(initializer),
        name.getPosition()
    );
}

/**
 * @brief (AR) يحلل تصريح enum: enum اسم { عضو1، عضو2 }.
 *        (EN) Parses enum declaration: enum name { member1, member2 }.
 */
StmtPtr ParserCore::parseEnumDecl() {
    // Expect enum name
    // (AR) توقع اسم Enum
    Token name = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم Enum. (EN) Expected enum name.");
    
    consume(TT::BRACE_LEFT, 
        "(AR) توقع '{' قبل أعضاء Enum. (EN) Expected '{' before enum members.");
    
    // Parse enum members
    // (AR) تحليل أعضاء Enum
    std::vector<EnumMember> members;
    
    while (!check(TT::BRACE_RIGHT) && !isAtEnd()) {
        Token memberName = consume(TT::IDENTIFIER, 
            "(AR) توقع اسم عضو Enum. (EN) Expected enum member name.");
        
        ExprPtr value = nullptr;
        if (match(TT::OP_ASSIGN)) {
            // Explicit value
            // (AR) قيمة صريحة
            value = parseExpression();
        }
        
        members.push_back(EnumMember(memberName.getValue(), std::move(value)));
        
        if (!check(TT::BRACE_RIGHT)) {
            consume(TT::COMMA, 
                "(AR) توقع ',' بين أعضاء Enum. (EN) Expected ',' between enum members.");
        }
    }
    
    consume(TT::BRACE_RIGHT, 
        "(AR) توقع '}' بعد أعضاء Enum. (EN) Expected '}' after enum members.");
    
    // Create enum declaration node
    // (AR) إنشاء عقدة تصريح Enum
    return std::make_unique<EnumDecl>(
        name.getValue(),
        members,
        name.getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة استيراد: استورد module;
 *        (EN) Parses import statement: import module;
 */
StmtPtr ParserCore::parseImportStmt() {
    // Expect module name
    // (AR) توقع اسم الوحدة
    Token moduleName = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم الوحدة. (EN) Expected module name.");
    
    consume(TT::SEMICOLON, 
        "(AR) توقع ';' بعد جملة الاستيراد. (EN) Expected ';' after import statement.");
    
    // Create import statement node
    // (AR) إنشاء عقدة جملة الاستيراد
    return std::make_unique<ImportStmt>(
        moduleName.getValue(),
        moduleName.getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة تصدير: صدّر identifier;
 *        (EN) Parses export statement: export identifier;
 */
StmtPtr ParserCore::parseExportStmt() {
    // Expect exported name
    // (AR) توقع الاسم المُصدّر
    Token exportName = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم العنصر المُصدّر. (EN) Expected exported item name.");
    
    consume(TT::SEMICOLON, 
        "(AR) توقع ';' بعد جملة التصدير. (EN) Expected ';' after export statement.");
    
    // Create export statement node
    // (AR) إنشاء عقدة جملة التصدير
    return std::make_unique<ExportStmt>(
        exportName.getValue(),
        exportName.getPosition()
    );
}

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
        "(AR) توقع '(' بعد 'إذا'. (EN) Expected '(' after 'if'.");
    auto condition = parseExpression();
    consume(TT::PAREN_RIGHT, 
        "(AR) توقع ')' بعد الشرط. (EN) Expected ')' after condition.");
    
    // Parse then branch
    // (AR) تحليل فرع then
    auto thenBranch = parseStatement();
    
    // Parse optional else branch
    // (AR) تحليل فرع else الاختياري
    StmtPtr elseBranch = nullptr;
    if (match(TT::KEYWORD_ELSE)) {
        elseBranch = parseStatement();
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
        "(AR) توقع '(' بعد 'بينما'. (EN) Expected '(' after 'while'.");
    auto condition = parseExpression();
    consume(TT::PAREN_RIGHT, 
        "(AR) توقع ')' بعد الشرط. (EN) Expected ')' after condition.");
    
    // Parse body
    // (AR) تحليل الجسم
    auto body = parseStatement();
    
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
    consume(TT::PAREN_LEFT, 
        "(AR) توقع '(' بعد 'لكل'. (EN) Expected '(' after 'for'.");
    
    // Parse loop variable
    // (AR) تحليل متغير الحلقة
    Token var = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم متغير الحلقة. (EN) Expected loop variable name.");
    
    // Expect 'in' keyword (Arabic: في)
    // (AR) توقع كلمة 'في'
    if (!match(TT::KEYWORD_IN)) {
        error("(AR) توقع 'في' في حلقة for. (EN) Expected 'in' in for loop.");
    }
    
    // Parse collection expression
    // (AR) تحليل تعبير المجموعة
    auto collection = parseExpression();
    
    consume(TT::PAREN_RIGHT, 
        "(AR) توقع ')' بعد مجموعة for. (EN) Expected ')' after for collection.");
    
    // Parse body
    // (AR) تحليل الجسم
    auto body = parseStatement();
    
    // Create for statement node
    // (AR) إنشاء عقدة جملة For
    return std::make_unique<ForStmt>(
        var.getValue(),
        std::move(collection),
        std::move(body),
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
    if (!check(TT::SEMICOLON)) {
        value = parseExpression();
    }
    
    consume(TT::SEMICOLON, 
        "(AR) توقع ';' بعد جملة return. (EN) Expected ';' after return statement.");
    
    // Create return statement node
    // (AR) إنشاء عقدة جملة Return
    return std::make_unique<ReturnStmt>(
        std::move(value),
        keyword.getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة break: اخرج;
 *        (EN) Parses break statement: break;
 */
StmtPtr ParserCore::parseBreakStmt() {
    Token keyword = previous();
    consume(TT::SEMICOLON, 
        "(AR) توقع ';' بعد break. (EN) Expected ';' after break.");
    
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
    consume(TT::SEMICOLON, 
        "(AR) توقع ';' بعد continue. (EN) Expected ';' after continue.");
    
    // Create continue statement node
    // (AR) إنشاء عقدة جملة Continue
    return std::make_unique<ContinueStmt>(keyword.getPosition());
}

/**
 * @brief (AR) يحلل كتلة من الجمل: { جملة1; جملة2; }.
 *        (EN) Parses block of statements: { stmt1; stmt2; }.
 */
StmtPtr ParserCore::parseBlockStmt() {
    StmtList statements;
    
    // Parse statements until closing brace
    // (AR) تحليل الجمل حتى القوس المُغلق
    while (!check(TT::BRACE_RIGHT) && !isAtEnd()) {
        auto stmt = parseDeclaration();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }
    
    consume(TT::BRACE_RIGHT, 
        "(AR) توقع '}' بعد الكتلة. (EN) Expected '}' after block.");
    
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
    // Parse try block
    // (AR) تحليل كتلة try
    consume(TT::BRACE_LEFT, 
        "(AR) توقع '{' بعد 'حاول'. (EN) Expected '{' after 'try'.");
    auto tryBlock = parseBlockStmt();
    
    // Parse catch clauses
    // (AR) تحليل بنود catch
    std::vector<CatchClause> catchClauses;
    while (match(TT::KEYWORD_CATCH)) {
        consume(TT::PAREN_LEFT, 
            "(AR) توقع '(' بعد 'اصطد'. (EN) Expected '(' after 'catch'.");
        
        Token exceptionVar = consume(TT::IDENTIFIER, 
            "(AR) توقع اسم متغير الاستثناء. (EN) Expected exception variable name.");
        
        consume(TT::PAREN_RIGHT, 
            "(AR) توقع ')' بعد متغير الاستثناء. (EN) Expected ')' after exception variable.");
        
        consume(TT::BRACE_LEFT, 
            "(AR) توقع '{' قبل جسم catch. (EN) Expected '{' before catch body.");
        auto catchBody = parseBlockStmt();
        
        catchClauses.push_back(CatchClause(
            exceptionVar.getValue(), 
            Data::DataType::UNKNOWN,  // Type inference will be done in semantic analysis
            std::move(catchBody)
        ));
    }
    
    // Parse optional finally block
    // (AR) تحليل كتلة finally الاختيارية
    StmtPtr finallyBlock = nullptr;
    if (match(TT::KEYWORD_FINALLY)) {
        consume(TT::BRACE_LEFT, 
            "(AR) توقع '{' بعد 'أخيراً'. (EN) Expected '{' after 'finally'.");
        finallyBlock = parseBlockStmt();
    }
    
    // Create try statement node
    // (AR) إنشاء عقدة جملة Try
    return std::make_unique<TryStmt>(
        std::move(tryBlock),
        catchClauses,
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
    
    consume(TT::SEMICOLON, 
        "(AR) توقع ';' بعد جملة throw. (EN) Expected ';' after throw statement.");
    
    // Create raise statement node
    // (AR) إنشاء عقدة جملة Raise
    return std::make_unique<RaiseStmt>(
        std::move(exception),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة with: مع مورد { جسم }.
 *        (EN) Parses with statement: with resource { body }.
 */
StmtPtr ParserCore::parseWithStmt() {
    // Parse resource expression
    // (AR) تحليل تعبير المورد
    auto resource = parseExpression();
    
    // Parse body
    // (AR) تحليل الجسم
    consume(TT::BRACE_LEFT, 
        "(AR) توقع '{' قبل جسم with. (EN) Expected '{' before with body.");
    auto body = parseBlockStmt();
    
    // Create with statement node
    // (AR) إنشاء عقدة جملة With
    return std::make_unique<WithStmt>(
        std::move(resource),
        std::move(body),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة تعبير: تعبير;
 *        (EN) Parses expression statement: expression;
 */
StmtPtr ParserCore::parseExpressionStmt() {
    auto expr = parseExpression();
    consume(TT::SEMICOLON, 
        "(AR) توقع ';' بعد التعبير. (EN) Expected ';' after expression.");
    
    // Create expression statement node
    // (AR) إنشاء عقدة جملة التعبير
    return std::make_unique<ExprStmt>(
        std::move(expr),
        previous().getPosition()
    );
}

// ======================================================================
// (AR) تحليل التعبيرات / (EN) Expression Parsing
// ======================================================================

/**
 * @brief (AR) يحلل تعبير - نقطة الدخول.
 *        (EN) Parses expression - entry point.
 */
ExprPtr ParserCore::parseExpression() {
    return parseAssignment();
}

/**
 * @brief (AR) يحلل تعيين: اسم = قيمة.
 *        (EN) Parses assignment: name = value.
 */
ExprPtr ParserCore::parseAssignment() {
    auto expr = parseLogicalOr();
    
    // Check for assignment operator
    // (AR) التحقق من عامل التعيين
    if (match(TT::OP_ASSIGN)) {
        Token equals = previous();
        auto value = parseAssignment();
        
        // Left side must be a variable
        // (AR) الجانب الأيسر يجب أن يكون متغيراً
        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(
                var->name,
                std::move(value),
                equals.getPosition()
            );
        }
        
        error("(AR) هدف تعيين غير صالح. (EN) Invalid assignment target.");
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل عامل OR المنطقي: تعبير1 || تعبير2.
 *        (EN) Parses logical OR: expr1 || expr2.
 */
ExprPtr ParserCore::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    
    while (match(TT::OP_OR)) {
        Token op = previous();
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل عامل AND المنطقي: تعبير1 && تعبير2.
 *        (EN) Parses logical AND: expr1 && expr2.
 */
ExprPtr ParserCore::parseLogicalAnd() {
    auto expr = parseEquality();
    
    while (match(TT::OP_AND)) {
        Token op = previous();
        auto right = parseEquality();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل عوامل المساواة: == !=.
 *        (EN) Parses equality operators: == !=.
 */
ExprPtr ParserCore::parseEquality() {
    auto expr = parseComparison();
    
    while (matchAny({TT::OP_EQUAL, TT::OP_NOT_EQUAL})) {
        Token op = previous();
        auto right = parseComparison();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل عوامل المقارنة: < <= > >=.
 *        (EN) Parses comparison operators: < <= > >=.
 */
ExprPtr ParserCore::parseComparison() {
    auto expr = parseTerm();
    
    while (matchAny({TT::OP_LESS, TT::OP_LESS_EQUAL, 
                     TT::OP_GREATER, TT::OP_GREATER_EQUAL})) {
        Token op = previous();
        auto right = parseTerm();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل الجمع والطرح: + -.
 *        (EN) Parses addition and subtraction: + -.
 */
ExprPtr ParserCore::parseTerm() {
    auto expr = parseFactor();
    
    while (matchAny({TT::OP_PLUS, TT::OP_MINUS})) {
        Token op = previous();
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل الضرب والقسمة: * / %.
 *        (EN) Parses multiplication and division: * / %.
 */
ExprPtr ParserCore::parseFactor() {
    auto expr = parseUnary();
    
    while (matchAny({TT::OP_MULTIPLY, TT::OP_DIVIDE, TT::OP_MODULO})) {
        Token op = previous();
        auto right = parseUnary();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل العوامل الأحادية: - ! ++ --.
 *        (EN) Parses unary operators: - ! ++ --.
 */
ExprPtr ParserCore::parseUnary() {
    if (matchAny({TT::OP_NOT, TT::OP_MINUS, TT::OP_INCREMENT, TT::OP_DECREMENT})) {
        Token op = previous();
        auto right = parseUnary();
        return std::make_unique<UnaryExpr>(
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return parsePower();
}

/**
 * @brief (AR) يحلل عامل الأس: **.
 *        (EN) Parses power operator: **.
 */
ExprPtr ParserCore::parsePower() {
    auto expr = parsePostfix();
    
    if (match(TT::OP_POWER)) {
        Token op = previous();
        auto right = parsePower();  // Right associative
        expr = std::make_unique<BinaryExpr>(
            std::move(expr),
            op.getType(),
            std::move(right),
            op.getPosition()
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل استدعاءات الدوال والوصول: f() obj.member arr[i].
 *        (EN) Parses function calls and access: f() obj.member arr[i].
 */
ExprPtr ParserCore::parsePostfix() {
    auto expr = parsePrimary();
    
    while (true) {
        if (match(TT::PAREN_LEFT)) {
            // Function call
            // (AR) استدعاء دالة
            auto args = parseArgumentList();
            consume(TT::PAREN_RIGHT, 
                "(AR) توقع ')' بعد الوسائط. (EN) Expected ')' after arguments.");
            expr = std::make_unique<CallExpr>(
                std::move(expr),
                std::move(args),
                previous().getPosition()
            );
        }
        else if (match(TT::DOT)) {
            // Member access
            // (AR) الوصول لعضو
            Token member = consume(TT::IDENTIFIER, 
                "(AR) توقع اسم عضو بعد '.'. (EN) Expected member name after '.'.");
            expr = std::make_unique<MemberExpr>(
                std::move(expr),
                member.getValue(),
                member.getPosition()
            );
        }
        else if (match(TT::BRACKET_LEFT)) {
            // Index access
            // (AR) الوصول بالفهرس
            auto index = parseExpression();
            consume(TT::BRACKET_RIGHT, 
                "(AR) توقع ']' بعد الفهرس. (EN) Expected ']' after index.");
            expr = std::make_unique<IndexExpr>(
                std::move(expr),
                std::move(index),
                previous().getPosition()
            );
        }
        else {
            break;
        }
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل التعبيرات الأساسية: أرقام، نصوص، متغيرات.
 *        (EN) Parses primary expressions: numbers, strings, variables.
 */
ExprPtr ParserCore::parsePrimary() {
    // Literals
    // (AR) القيم الحرفية
    
    if (match(TT::LITERAL_TRUE)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    if (match(TT::LITERAL_FALSE)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    if (match(TT::LITERAL_NULL)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    if (match(TT::NUMBER_INTEGER) || match(TT::NUMBER_DOUBLE)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    if (match(TT::STRING_LITERAL)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    // Variable reference
    // (AR) مرجع متغير
    if (match(TT::IDENTIFIER)) {
        return std::make_unique<VariableExpr>(
            previous().getValue(),
            previous().getPosition()
        );
    }
    
    // Parenthesized expression
    // (AR) تعبير بين أقواس
    if (match(TT::PAREN_LEFT)) {
        auto expr = parseExpression();
        consume(TT::PAREN_RIGHT, 
            "(AR) توقع ')' بعد التعبير. (EN) Expected ')' after expression.");
        return expr;
    }
    
    // Array literal
    // (AR) مصفوفة حرفية
    if (match(TT::BRACKET_LEFT)) {
        return parseArrayLiteral();
    }
    
    // Map literal
    // (AR) خريطة حرفية
    if (match(TT::BRACE_LEFT)) {
        return parseMapLiteral();
    }
    
    error("(AR) توقع تعبير. (EN) Expected expression.");
    return nullptr;
}

/**
 * @brief (AR) يحلل دالة لامدا: lambda x: x + 1.
 *        (EN) Parses lambda function: lambda x: x + 1.
 */
ExprPtr ParserCore::parseLambda() {
    // Parse parameters
    // (AR) تحليل المعاملات
    std::vector<std::string> params;
    if (!check(TT::COLON)) {
        params = parseParameterList();
    }
    
    consume(TT::COLON, 
        "(AR) توقع ':' بعد معاملات lambda. (EN) Expected ':' after lambda parameters.");
    
    // Parse body expression
    // (AR) تحليل تعبير الجسم
    auto body = parseExpression();
    
    // Create lambda expression node
    // (AR) إنشاء عقدة تعبير Lambda
    return std::make_unique<LambdaExpr>(
        params,
        std::move(body),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل مصفوفة حرفية: [1, 2, 3].
 *        (EN) Parses array literal: [1, 2, 3].
 */
ExprPtr ParserCore::parseArrayLiteral() {
    ExprList elements;
    
    // Parse elements
    // (AR) تحليل العناصر
    if (!check(TT::BRACKET_RIGHT)) {
        do {
            elements.push_back(parseExpression());
        } while (match(TT::COMMA));
    }
    
    consume(TT::BRACKET_RIGHT, 
        "(AR) توقع ']' بعد عناصر المصفوفة. (EN) Expected ']' after array elements.");
    
    // Create array expression node
    // (AR) إنشاء عقدة تعبير المصفوفة
    return std::make_unique<ArrayExpr>(
        std::move(elements),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل خريطة حرفية: {key: value, ...}.
 *        (EN) Parses map literal: {key: value, ...}.
 */
ExprPtr ParserCore::parseMapLiteral() {
    std::vector<std::pair<ExprPtr, ExprPtr>> pairs;
    
    // Parse key-value pairs
    // (AR) تحليل أزواج المفتاح-القيمة
    if (!check(TT::BRACE_RIGHT)) {
        do {
            auto key = parseExpression();
            consume(TT::COLON, 
                "(AR) توقع ':' بعد مفتاح الخريطة. (EN) Expected ':' after map key.");
            auto value = parseExpression();
            pairs.push_back({std::move(key), std::move(value)});
        } while (match(TT::COMMA));
    }
    
    consume(TT::BRACE_RIGHT, 
        "(AR) توقع '}' بعد عناصر الخريطة. (EN) Expected '}' after map elements.");
    
    // Create map expression node
    // (AR) إنشاء عقدة تعبير الخريطة
    return std::make_unique<MapExpr>(
        std::move(pairs),
        previous().getPosition()
    );
}

} // namespace Parser
} // namespace Sad