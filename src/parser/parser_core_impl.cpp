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
            std::cout << "Parsing program...\n";
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
    // (AR) التحقق من المُزخرِفات قبل التصريح
    // (EN) Check for decorators before declaration
    ExprList decorators;
    while (check(TT::AT_SIGN)) {
        match(TT::AT_SIGN);  // consume @
        decorators.push_back(parseDecorator());
    }
    
    // Check for declaration keywords
    // (AR) التحقق من كلمات التصريح المفتاحية
    if (match(TT::KEYWORD_FUNCTION)) {
        return parseFunctionDecl(std::move(decorators));
    }
    
    if (match(TT::KEYWORD_CLASS)) {
        // TODO: Add decorator support for classes in future
        // (AR) ملاحظة: دعم المُزخرِفات للأصناف سيُضاف مستقبلاً
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات للأصناف غير مدعومة بعد. (EN) Class decorators not yet supported.");
        }
        return parseClassDecl();
    }
    
    if (match(TT::KEYWORD_VAR) || match(TT::KEYWORD_LET) || match(TT::KEYWORD_CONST)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مع المتغيرات. (EN) Decorators cannot be used with variables.");
        }
        return parseVarDecl();
    }
    
    if (match(TT::KEYWORD_ENUM)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مع التعدادات. (EN) Decorators cannot be used with enums.");
        }
        return parseEnumDecl();
    }
    
    if (match(TT::KEYWORD_IMPORT)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مع الاستيراد. (EN) Decorators cannot be used with import.");
        }
        return parseImportStmt();
    }
    
    if (match(TT::KEYWORD_EXPORT)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مع التصدير. (EN) Decorators cannot be used with export.");
        }
        return parseExportStmt();
    }
    
    // If decorators without valid target
    // (AR) إذا وُجدت مُزخرِفات بدون هدف صالح
    if (!decorators.empty()) {
        error("(AR) المُزخرِفات يجب أن تسبق تصريح دالة. (EN) Decorators must precede a function declaration.");
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
    
    if (match(TT::KEYWORD_YIELD)) {
        return parseYieldStmt();
    }
    
    if (match(TT::KEYWORD_BREAK)) {
        return parseBreakStmt();
    }
    
    if (match(TT::KEYWORD_CONTINUE)) {
        return parseContinueStmt();
    }
    
    // Check for block vs map literal
    // Strategy: { followed by expression is likely map if we find : early
    // Block: { stmt; stmt; }
    // Map: {k: v} or {k: v for x in list}
    // (AR) التحقق من block أو خريطة حرفية
    if (check(TT::BRACE_LEFT)) {
        std::cout << "Found BRACE_LEFT, trying to determine type\n";
        
        // Save position
        Token brace = current_;
        advance(); // consume {
        
        // Check for empty map
        if (check(TT::BRACE_RIGHT)) {
            std::cout << "Empty map: {}\n";
            consume(TT::BRACE_RIGHT, "Expected }");
            auto mapExpr = std::make_unique<MapExpr>(std::vector<MapPair>{}, brace.getPosition());
            return std::make_unique<ExprStmt>(std::move(mapExpr));
        }
        
        // Try to parse first expression (key)
        // This could be: identifier, number, string, or complex expression
        std::cout << "Parsing potential map key expression\n";
        ExprPtr firstKey = parseExpression();
        
        if (!firstKey) {
            std::cout << "Failed to parse key, treating as block\n";
            auto block = parseBlockStmt();
            return block;
        }
        
        // Check if followed by colon (map syntax)
        if (check(TT::COLON)) {
            std::cout << "Detected colon - this is a map!\n";
            consume(TT::COLON, "Expected :");
            
            // Parse first value
            ExprPtr firstValue = parseExpression();
            
            if (!firstValue) {
                error("Expected value expression after :");
                return nullptr;
            }
            
            // Check for dict comprehension
            if (check(TT::KEYWORD_FOR)) {
                std::cout << "Dict comprehension detected!\n";
                advance(); // consume 'for'
                
                Token loopVar = consume(TT::IDENTIFIER, "Expected loop variable");
                consume(TT::KEYWORD_IN, "Expected 'in'");
                auto iterable = parseExpression();
                
                ExprPtr condition = nullptr;
                if (match(TT::KEYWORD_IF)) {
                    condition = parseExpression();
                }
                
                consume(TT::BRACE_RIGHT, "Expected }");
                
                // Create dict comprehension
                auto dictComp = std::make_unique<DictComprehensionExpr>(
                    std::move(firstKey),
                    std::move(firstValue),
                    loopVar.getValue(),
                    std::move(iterable),
                    std::move(condition),
                    brace.getPosition()
                );
                
                return std::make_unique<ExprStmt>(std::move(dictComp));
            }
            
            // Regular map literal
            std::cout << "Regular map literal\n";
            std::vector<MapPair> pairs;
            pairs.emplace_back(std::move(firstKey), std::move(firstValue));
            
            while (match(TT::COMMA)) {
                if (check(TT::BRACE_RIGHT)) break;
                
                auto key = parseExpression();
                consume(TT::COLON, "Expected :");
                auto value = parseExpression();
                pairs.emplace_back(std::move(key), std::move(value));
            }
            
            consume(TT::BRACE_RIGHT, "Expected }");
            
            auto mapExpr = std::make_unique<MapExpr>(std::move(pairs), brace.getPosition());
            return std::make_unique<ExprStmt>(std::move(mapExpr));
        }
        
        // No colon found - this must be a block statement with expression
        // But we've already consumed { and parsed an expression
        // This is problematic - we need to handle this as expression statement in block
        std::cout << "No colon found, treating as block with expression statement\n";
        
        // We have an expression, make it an expression statement
        auto exprStmt = std::make_unique<ExprStmt>(std::move(firstKey));
        
        // Continue parsing rest of block
        StmtList statements;
        statements.push_back(std::move(exprStmt));
        
        // Parse remaining statements
        while (!check(TT::BRACE_RIGHT) && !isAtEnd()) {
            auto stmt = parseDeclaration();
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
        }
        
        consume(TT::BRACE_RIGHT, "Expected }");
        
        return std::make_unique<BlockStmt>(std::move(statements), brace.getPosition());
    }
    
    std::cout << "Parsing declaration after block/map check...\n";
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
 * @brief (AR) يحلل تصريح دالة مع مُزخرِفات اختيارية.
 *        (EN) Parses function declaration with optional decorators.
 * 
 * @param decorators (AR) قائمة المُزخرِفات (إن وُجدت)
 *                   (EN) List of decorators (if any)
 * 
 * Grammar / القواعد:
 *   function_decl → decorator* "function" IDENTIFIER "(" parameters ")" [ ":" type ] block
 * 
 * @example Examples / أمثلة:
 * - function test() {}
 * - @staticmethod\nfunction test() {}
 * - @cache(100)\n@memoize\nfunction expensive() {}
 */
StmtPtr ParserCore::parseFunctionDecl(ExprList decorators) {
    // Expect function name
    // (AR) توقع اسم الدالة
    Token name = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم الدالة. (EN) Expected function name.");
    
    // Parse parameter list (now with type annotations)
    // (AR) تحليل قائمة المعاملات (الآن مع تصريحات الأنواع)
    consume(TT::PAREN_LEFT, 
        "(AR) توقع '(' بعد اسم الدالة. (EN) Expected '(' after function name.");
    auto paramObjs = parseTypedParameterList();
    consume(TT::PAREN_RIGHT, 
        "(AR) توقع ')' بعد المعاملات. (EN) Expected ')' after parameters.");
    
    // Optional return type annotation: function name(...) : type
    // (AR) تصريح نوع الإرجاع الاختياري: دالة اسم(...) : نوع
    Data::DataType returnType = Data::DataType::UNKNOWN;
    if (match(TT::COLON)) {
        returnType = parseType();
    }
    
    // Parse function body
    // (AR) تحليل جسم الدالة
    consume(TT::BRACE_LEFT, 
        "(AR) توقع '{' قبل جسم الدالة. (EN) Expected '{' before function body.");
    auto body = parseBlockStmt();
    
    // (AR) إنشاء عقدة تصريح الدالة مع المُزخرِفات
    // (EN) Create function declaration node with decorators
    if (!decorators.empty()) {
        return std::make_unique<FunctionDecl>(
            name.getValue(),
            std::move(paramObjs),
            returnType,
            std::move(body),
            std::move(decorators),
            false,
            name.getPosition()
        );
    }
    
    // (AR) إنشاء عقدة تصريح الدالة بدون مُزخرِفات
    // (EN) Create function declaration node without decorators
    return std::make_unique<FunctionDecl>(
        name.getValue(),
        std::move(paramObjs),
        returnType,
        std::move(body),
        false,
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
        false,
        name.getPosition()
    );
}

/**
 * @brief (AR) يحلل تصريح متغير بصيغتين:
 *             1. var/let/const name : type = value;
 *             2. type name = value;
 *        (EN) Parses variable declaration in two formats:
 *             1. var/let/const name : type = value;
 *             2. type name = value;
 */
StmtPtr ParserCore::parseVarDecl() {
    Data::DataType varType = Data::DataType::UNKNOWN;
    Token name(TT::IDENTIFIER, "");  // Initialize with default
    
    // Check if we have type-first syntax: type IDENTIFIER
    // (AR) التحقق من صيغة النوع أولاً: نوع معرّف
    if (check(TT::IDENTIFIER)) {
        // Peek ahead to determine if this is a type name
        // (AR) النظر للأمام لتحديد ما إذا كان هذا اسم نوع
        Token possibleType = peek();
        TokenType typeToken = possibleType.getType();
        std::string typeName = possibleType.getValue();
        
        // Check if it's a known type name (int, string, float, bool, etc.)
        // (AR) التحقق مما إذا كان اسم نوع معروف
        bool isKnownType = (typeToken == TT::TYPE_INTEGER || typeName == "صحيح" ||
                            typeName == "عشري" || typeToken == TT::TYPE_DOUBLE ||
                           typeToken == TT::TYPE_STRING || typeName == "نص" ||
                           typeToken == TT::TYPE_BOOLEAN || typeName == "منطقي" ||
                           typeToken == TT::TYPE_VOID || typeName == "عدم" ||
                           typeToken == TT::TYPE_NULL || typeName == "تلقائي");
        
        if (isKnownType) {
            // Format 2: type IDENTIFIER = value;
            // (AR) الصيغة 2: نوع معرّف = قيمة;
            varType = parseType();
            name = consume(TT::IDENTIFIER, 
                "(AR) توقع اسم المتغير بعد النوع. (EN) Expected variable name after type.");
        } else {
            // Format 1: var/let/const IDENTIFIER : type = value;
            // (AR) الصيغة 1: var/let/const معرّف : نوع = قيمة;
            name = consume(TT::IDENTIFIER, 
                "(AR) توقع اسم المتغير. (EN) Expected variable name.");
            
            // Optional type annotation: name : type
            // (AR) تصريح النوع الاختياري: اسم : نوع
            if (match(TT::COLON)) {
                varType = parseType();
            }
        }
    } else {
        // Format 1: var/let/const IDENTIFIER : type = value;
        // (AR) الصيغة 1: var/let/const معرّف : نوع = قيمة;
        name = consume(TT::IDENTIFIER, 
            "(AR) توقع اسم المتغير. (EN) Expected variable name.");
        
        // Optional type annotation: name : type
        // (AR) تصريح النوع الاختياري: اسم : نوع
        if (match(TT::COLON)) {
            varType = parseType();
        }
    }
    
    // Optional initializer
    // (AR) المُهيّئ الاختياري
    ExprPtr initializer = nullptr;
    if (match(TT::OP_ASSIGN)) {
        initializer = parseExpression();
    }
    
    // Optional semicolon (support both Arabic and English)
    // (AR) فاصلة منقوطة اختيارية (دعم العربية والإنجليزية)
    if (check(TT::SEMICOLON)) {
        advance();
    }
    
    // Create variable declaration statement
    // (AR) إنشاء جملة تصريح المتغير
    return std::make_unique<VarDeclStmt>(
        name.getValue(),
        varType,
        std::move(initializer),
        false,
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
        std::move(members),
        false,
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
        "",
        std::vector<std::string>{},
        false,
        moduleName.getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة تصدير: صدّر identifier;
 *        (EN) Parses export statement: export identifier;
 */
StmtPtr ParserCore::parseExportStmt() {
    // Parse the declaration to be exported
    // (AR) تحليل التصريح المُصدّر
    auto declaration = parseDeclaration();
    
    // Create export statement node
    // (AR) إنشاء عقدة جملة التصدير
    return std::make_unique<ExportStmt>(
        std::move(declaration),
        previous().getPosition()
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
    if (!check(TT::SEMICOLON) && !check(TT::BRACE_RIGHT) && !isAtEnd()) {
        value = parseExpression();
    }
    
    // (AR) توقع الفاصلة المنقوطة
    // (EN) Expect semicolon
    consume(TT::SEMICOLON, 
        "(AR) توقع ';' بعد جملة yield. (EN) Expected ';' after yield statement.");
    
    // (AR) إنشاء عقدة جملة Yield
    // (EN) Create yield statement node
    return std::make_unique<YieldStmt>(
        std::move(value), 
        isYieldFrom, 
        yieldToken.getPosition()
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
    consume(TT::PAREN_LEFT,
        "(AR) توقع '(' بعد with. (EN) Expected '(' after with.");
    
    // Parse variable name
    // (AR) تحليل اسم المتغير
    Token varName = consume(TT::IDENTIFIER,
        "(AR) توقع اسم متغير. (EN) Expected variable name.");
    
    consume(TT::OP_ASSIGN,
        "(AR) توقع '=' في with. (EN) Expected '=' in with.");
    
    // Parse resource expression
    // (AR) تحليل تعبير المورد
    auto resource = parseExpression();
    
    consume(TT::PAREN_RIGHT,
        "(AR) توقع ')' بعد with. (EN) Expected ')' after with.");
    
    // Parse body
    // (AR) تحليل الجسم
    consume(TT::BRACE_LEFT, 
        "(AR) توقع '{' قبل جسم with. (EN) Expected '{' before with body.");
    auto body = parseBlockStmt();
    
    // Create with statement node
    // (AR) إنشاء عقدة جملة With
    return std::make_unique<WithStmt>(
        varName.getValue(),
        std::move(resource),
        std::move(body),
        varName.getPosition()
    );
}

/**
 * @brief (AR) يحلل جملة تعبير: تعبير;
 *        (EN) Parses expression statement: expression;
 */
StmtPtr ParserCore::parseExpressionStmt() {
    auto expr = parseExpression();
    
    // Semicolon is optional for expression statements
    // (AR) الفاصلة المنقوطة اختيارية لجمل التعبير
    if (check(TT::SEMICOLON)) {
        consume(TT::SEMICOLON, "");
    }
    
    // Create expression statement node
    // (AR) إنشاء عقدة جملة التعبير
    return std::make_unique<ExprStmt>(std::move(expr));
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
    // Lambda expression: lambda x: x + 1
    // (AR) تعبير لامدا
    if (match(TT::KEYWORD_LAMBDA)) {
        return parseLambda();
    }
    
    // TODO: Arrow function support needs proper lookahead implementation
    // Arrow function: (x, y) => x + y  OR  x => x * 2
    // (AR) دالة سهمية: (x, y) => x + y  أو  x => x * 2
    // if (isArrowFunction()) {
    //     return parseArrowFunction();
    // }
    
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
    std::vector<std::string> paramNames;
    if (!check(TT::COLON)) {
        paramNames = parseParameterList();
    }
    
    consume(TT::COLON, 
        "(AR) توقع ':' بعد معاملات lambda. (EN) Expected ':' after lambda parameters.");
    
    // Parse body expression
    // (AR) تحليل تعبير الجسم
    auto body = parseExpression();
    
    // Convert param names to Parameter objects
    // (AR) تحويل أسماء المعاملات إلى كائنات Parameter
    std::vector<Parameter> params;
    params.reserve(paramNames.size());
    for (const auto& name : paramNames) {
        params.emplace_back(name, Data::DataType::UNKNOWN, nullptr);
    }
    
    // Create lambda expression node
    // (AR) إنشاء عقدة تعبير Lambda
    return std::make_unique<LambdaExpr>(
        std::move(params),
        std::move(body),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل مُزخرِف (decorator): @decorator أو @decorator(args).
 *        (EN) Parses decorator: @decorator or @decorator(args).
 */
ExprPtr ParserCore::parseDecorator() {
    // Expect @ sign (should already be consumed by caller)
    // (AR) نتوقع @ (يجب أن يكون مُستهلكاً من المُستدعي)
    Token atSign = previous();
    
    // Expect decorator name
    // (AR) نتوقع اسم المُزخرِف
    Token decoratorName = consume(TT::IDENTIFIER,
        "(AR) توقع اسم المُزخرِف بعد @. "
        "(EN) Expected decorator name after @.");
    
    // Check for arguments
    // (AR) التحقق من الوسائط
    if (check(TT::PAREN_LEFT)) {
        // Decorator with arguments: @decorator(arg1, arg2, ...)
        // (AR) مُزخرِف مع وسائط
        advance(); // consume (
        
        ExprList args;
        
        // Parse arguments if not empty
        // (AR) تحليل الوسائط إذا لم تكن فارغة
        if (!check(TT::PAREN_RIGHT)) {
            args = parseArgumentList();
        }
        
        consume(TT::PAREN_RIGHT,
            "(AR) توقع ')' بعد وسائط المُزخرِف. "
            "(EN) Expected ')' after decorator arguments.");
        
        // Create decorator with arguments
        // (AR) إنشاء مُزخرِف مع وسائط
        return std::make_unique<DecoratorExpr>(
            decoratorName.getValue(),
            std::move(args),
            atSign.getPosition()
        );
    }
    
    // Decorator without arguments: @decorator
    // (AR) مُزخرِف بدون وسائط
    return std::make_unique<DecoratorExpr>(
        decoratorName.getValue(),
        atSign.getPosition()
    );
}

/**
 * @brief (AR) يحلل مصفوفة حرفية أو list comprehension: [1, 2, 3] أو [x * 2 for x in list].
 *        (EN) Parses array literal or list comprehension: [1, 2, 3] or [x * 2 for x in list].
 */
ExprPtr ParserCore::parseArrayLiteral() {
    // Check for empty array
    // (AR) التحقق من مصفوفة فارغة
    if (check(TT::BRACKET_RIGHT)) {
        consume(TT::BRACKET_RIGHT, "");
        return std::make_unique<ArrayExpr>(ExprList{}, previous().getPosition());
    }
    
    // Parse first element/expression
    // (AR) تحليل العنصر/التعبير الأول
    auto firstExpr = parseExpression();
    
    // Check if this is a list comprehension
    // (AR) التحقق إذا كان list comprehension
    if (check(TT::KEYWORD_FOR)) {
        // This is a list comprehension: [expr for var in iterable if cond]
        // (AR) هذا list comprehension
        
        advance(); // consume 'for'
        
        // Parse variable
        // (AR) تحليل المتغير
        Token var = consume(TT::IDENTIFIER, 
            "(AR) توقع اسم متغير في list comprehension. "
            "(EN) Expected variable name in list comprehension.");
        
        // Expect 'in' keyword
        // (AR) توقع كلمة 'في'
        consume(TT::KEYWORD_IN, 
            "(AR) توقع 'في' في list comprehension. "
            "(EN) Expected 'in' in list comprehension.");
        
        // Parse iterable
        // (AR) تحليل المجموعة القابلة للتكرار
        auto iterable = parseExpression();
        
        // Optional condition
        // (AR) الشرط الاختياري
        ExprPtr condition = nullptr;
        if (match(TT::KEYWORD_IF)) {
            condition = parseExpression();
        }
        
        consume(TT::BRACKET_RIGHT, 
            "(AR) توقع ']' بعد list comprehension. "
            "(EN) Expected ']' after list comprehension.");
        
        // Create list comprehension node
        // (AR) إنشاء عقدة List Comprehension
        return std::make_unique<ListComprehensionExpr>(
            std::move(firstExpr),
            var.getValue(),
            std::move(iterable),
            std::move(condition),
            var.getPosition()
        );
    }
    
    // Regular array literal
    // (AR) مصفوفة حرفية عادية
    ExprList elements;
    elements.push_back(std::move(firstExpr));
    
    // Parse remaining elements
    // (AR) تحليل العناصر المتبقية
    while (match(TT::COMMA)) {
        if (check(TT::BRACKET_RIGHT)) {
            break; // Trailing comma
        }
        elements.push_back(parseExpression());
    }
    
    consume(TT::BRACKET_RIGHT, 
        "(AR) توقع ']' بعد عناصر المصفوفة. "
        "(EN) Expected ']' after array elements.");
    
    // Create array expression node
    // (AR) إنشاء عقدة تعبير المصفوفة
    return std::make_unique<ArrayExpr>(
        std::move(elements),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل خريطة حرفية أو dict comprehension: {k: v} أو {k: v for k, v in items}.
 *        (EN) Parses map literal or dict comprehension: {k: v} or {k: v for k, v in items}.
 */
ExprPtr ParserCore::parseMapLiteral() {
    // Check for empty map
    // (AR) التحقق من خريطة فارغة
    if (check(TT::BRACE_RIGHT)) {
        consume(TT::BRACE_RIGHT, "");
        return std::make_unique<MapExpr>(std::vector<MapPair>{}, previous().getPosition());
    }
    
    // Parse first key expression (or key variable template in comprehension)
    // (AR) تحليل تعبير المفتاح الأول (أو قالب متغير المفتاح في comprehension)
    auto firstKey = parseExpression();
    if (!firstKey) {
        error("Failed to parse key expression in map literal");
        // Try to recover by consuming until }
        while (!check(TT::BRACE_RIGHT) && !isAtEnd()) {
            advance();
        }
        if (check(TT::BRACE_RIGHT)) consume(TT::BRACE_RIGHT, "");
        return nullptr;
    }
    
    consume(TT::COLON, 
        "(AR) توقع ':' بعد مفتاح الخريطة. "
        "(EN) Expected ':' after map key.");
    
    // Parse first value expression (or value variable template in comprehension)
    // (AR) تحليل تعبير القيمة الأولى (أو قالب متغير القيمة في comprehension)
    auto firstValue = parseExpression();
    if (!firstValue) {
        error("Failed to parse value expression in map literal");
        // Try to recover by consuming until }
        while (!check(TT::BRACE_RIGHT) && !isAtEnd()) {
            advance();
        }
        if (check(TT::BRACE_RIGHT)) consume(TT::BRACE_RIGHT, "");
        return nullptr;
    }
    
    // Check if this is a dict comprehension
    // (AR) التحقق إذا كان dict comprehension
    if (check(TT::KEYWORD_FOR)) {
        // This is a dict comprehension: {k: v for var in iterable if cond}
        // Note: firstKey and firstValue are the TEMPLATE expressions (k, v)
        // The loop variable comes AFTER 'for'
        // (AR) هذا dict comprehension - firstKey و firstValue هي القوالب
        
        advance(); // consume 'for'
        
        // Parse loop variable (can be single: 'x' or tuple: 'k, v')
        // For now, we support single variable only
        // (AR) تحليل متغير الحلقة
        Token loopVar = consume(TT::IDENTIFIER, 
            "(AR) توقع اسم متغير الحلقة في dict comprehension. "
            "(EN) Expected loop variable name in dict comprehension.");
        
        // Expect 'in' keyword
        // (AR) توقع كلمة 'في'
        consume(TT::KEYWORD_IN, 
            "(AR) توقع 'في' في dict comprehension. "
            "(EN) Expected 'in' in dict comprehension.");
        
        // Parse iterable
        // (AR) تحليل المجموعة القابلة للتكرار
        auto iterable = parseExpression();
        
        // Optional condition
        // (AR) الشرط الاختياري
        ExprPtr condition = nullptr;
        if (match(TT::KEYWORD_IF)) {
            condition = parseExpression();
        }
        
        consume(TT::BRACE_RIGHT, 
            "(AR) توقع '}' بعد dict comprehension. "
            "(EN) Expected '}' after dict comprehension.");
        
        // Create dict comprehension node
        // (AR) إنشاء عقدة Dict Comprehension
        return std::make_unique<DictComprehensionExpr>(
            std::move(firstKey),
            std::move(firstValue),
            loopVar.getValue(),
            std::move(iterable),
            std::move(condition),
            loopVar.getPosition()
        );
    }
    
    // Regular map literal
    // (AR) خريطة حرفية عادية
    std::vector<MapPair> pairs;
    pairs.emplace_back(std::move(firstKey), std::move(firstValue));
    
    // Parse remaining key-value pairs
    // (AR) تحليل أزواج المفتاح-القيمة المتبقية
    while (match(TT::COMMA)) {
        if (check(TT::BRACE_RIGHT)) {
            break; // Trailing comma
        }
        
        auto key = parseExpression();
        consume(TT::COLON, 
            "(AR) توقع ':' بعد مفتاح الخريطة. "
            "(EN) Expected ':' after map key.");
        auto value = parseExpression();
        pairs.emplace_back(std::move(key), std::move(value));
    }
    
    consume(TT::BRACE_RIGHT, 
        "(AR) توقع '}' بعد عناصر الخريطة. "
        "(EN) Expected '}' after map elements.");
    
    // Create map expression node
    // (AR) إنشاء عقدة تعبير الخريطة
    return std::make_unique<MapExpr>(
        std::move(pairs),
        previous().getPosition()
    );
}

} // namespace Parser
} // namespace Sad