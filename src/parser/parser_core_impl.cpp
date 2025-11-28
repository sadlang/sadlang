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
    , nextToken_(TT::END_OF_FILE, "") // Initialize with dummy token for lookahead
    , panicMode_(false)
    , filename_("<source>")  // (AR) اسم ملف افتراضي / (EN) default filename
{
    // Initialize by fetching first two tokens for proper lookahead
    // (AR) التهيئة بجلب أول رمزين للنظر المسبق الصحيح
    
    // Fetch first token into current_, skipping whitespace/comments
    current_ = lexer_.nextToken();
    while (current_.getType() == TT::WHITESPACE || 
           current_.getType() == TT::COMMENT ||
           current_.getType() == TT::NEWLINE) {
        current_ = lexer_.nextToken();
    }
    
    // Fetch second token into nextToken_, also skipping whitespace/comments
    nextToken_ = lexer_.nextToken();
    while (nextToken_.getType() == TT::WHITESPACE || 
           nextToken_.getType() == TT::COMMENT ||
           nextToken_.getType() == TT::NEWLINE) {
        nextToken_ = lexer_.nextToken();
    }
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
    
    // DEBUG: Disabled
    // std::cout << "[parser_core_impl.cpp] بدء parseProgram - current token: " 
    //           << static_cast<int>(current_.getType()) << " = '" 
    //           << current_.getValue() << "'\n";
    
    // Parse until EOF
    // (AR) التحليل حتى نهاية الملف
    while (!isAtEnd()) {
        try {
            // DEBUG: Disabled
            // std::cout << "[parser_core_impl.cpp] داخل حلقة parseProgram - current token: " 
            //           << static_cast<int>(current_.getType()) << " = '" 
            //           << current_.getValue() << "'\n";
            auto stmt = parseDeclaration();
            if (stmt) {
                statements.push_back(std::move(stmt));
                // DEBUG: Disabled
                // std::cout << "[parser_core_impl.cpp] تمت إضافة جملة - العدد الكلي: " 
                //           << statements.size() << "\n";
            }
        } catch (const std::exception& e) {
            error(e.what());
            synchronize();
        }
    }
    
    // DEBUG: Disabled
    // std::cout << "[parser_core_impl.cpp] انتهى parseProgram - عدد الجمل: " 
    //           << statements.size() << "\n";
    
    return statements;
}

/**
 * @brief (AR) يتحقق من وجود أخطاء تحليل.
 *        (EN) Checks for parsing errors.
 */
bool ParserCore::hasErrors() const {
    return Errors::ErrorManager::getInstance().hasErrors();
}

/**
 * @brief (AR) يطبع جميع الأخطاء.
 *        (EN) Prints all errors.
 */
void ParserCore::printErrors() const {
    // (AR) استخدام ErrorManager لطباعة الأخطاء بشكل جميل
    // (EN) Use ErrorManager to print errors beautifully
    Errors::ErrorManager::getInstance().printAll();
}

/**
 * @brief (AR) يرجع قائمة الأخطاء.
 *        (EN) Returns error list.
 */
std::vector<std::string> ParserCore::getErrors() const {
    // (AR) تحويل التشخيصات إلى قائمة نصية
    // (EN) Convert diagnostics to text list
    std::vector<std::string> result;
    const auto& diagnostics = Errors::ErrorManager::getInstance().getAllDiagnostics();
    
    for (const auto& diag : diagnostics) {
        result.push_back(diag.format(Errors::Language::ENGLISH, false));
    }
    
    return result;
}

// ======================================================================
// (AR) دوال التحليل الأساسية / (EN) Core Parsing Functions
// ======================================================================

/**
 * @brief (AR) يحلل تصريح واحد (دالة، صنف، متغير، import، export).
 *        (EN) Parses single declaration (function, class, variable, import, export).
 */
StmtPtr ParserCore::parseDeclaration() {
std::cout << "Parsing declaration...\n";

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
    
    // (AR) دعم الثوابت مع كلمة ثابت / (EN) Support constants with const keyword
    if (match(TT::KEYWORD_CONST)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مع المتغيرات. (EN) Decorators cannot be used with variables.");
        }
        return parseVarDecl();
    }
    
    std::cout << "Checking for type-first variable declaration...\n";
    
    // Check for type-first variable declaration: type IDENTIFIER = value;
    // Use proper lookahead to verify TYPE is followed by IDENTIFIER
    // (AR) التحقق من تصريح المتغير ببدء النوع: نوع معرّف = قيمة;
    // استخدام النظر المسبق الصحيح للتحقق أن TYPE متبوع بـ IDENTIFIER
    if (isTypeToken(current_.getType())) {
        // Look ahead to see if next token is IDENTIFIER
        const Token& nextTok = peekNext();
         std::cout << "Found TYPE IDENTIFIER pattern - parsing as variable declaration\n"<< nextTok.getTypeName()<< "\n position: "<< nextTok.getPosition().toString() <<"\n";
            
        if (nextTok.getType() == TT::IDENTIFIER) {
            // Valid variable declaration: TYPE IDENTIFIER
            std::cout << "Found TYPE IDENTIFIER pattern - parsing as variable declaration\n"<< nextTok.getTypeName()<< "\n position: "<< nextTok.getPosition().toString() <<"\n";
            if (!decorators.empty()) {
                error("(AR) المُزخرِفات لا تُستخدم مع المتغيرات. (EN) Decorators cannot be used with variables.");
            }
            return parseVarDecl();
        } else {
            // TYPE token not followed by IDENTIFIER - this is an error
            // The user wrote a type keyword but didn't follow it with a variable name
            std::cout << "TYPE token not followed by IDENTIFIER - syntax error\n";
            errorExpectedToken("معرّف (اسم متغير)", "identifier (variable name)", "بعد نوع البيانات", "after data type");
            // Try to recover by synchronizing to next statement
            synchronize();
            return nullptr;
        }
    }
    
    // Check for class-typed variable declaration: ClassName varName = ...
    // (AR) التحقق من تصريح متغير من نوع صنف: اسم_الصنف اسم_المتغير = ...
    if (check(TT::IDENTIFIER)) {
        // Peek to see if this looks like: IDENTIFIER IDENTIFIER ASSIGN
        // or: IDENTIFIER IDENTIFIER = جديد
        const Token& nextTok = peekNext();
        if (nextTok.getType() == TT::IDENTIFIER) {
            // Could be class-typed variable, but we can't check ClassManager during parsing
            // So we use a heuristic: if current looks like a type name (starts with capital letter
            // or is registered as a class), treat as variable declaration
            // For now, always treat IDENTIFIER IDENTIFIER pattern as variable declaration
            // (AR) نستخدم قاعدة: إذا وجدنا معرّف متبوع بمعرّف، نعتبره تصريح متغير
            std::cout << "Found IDENTIFIER IDENTIFIER pattern - treating as variable declaration\n";
            if (!decorators.empty()) {
                error("(AR) المُزخرِفات لا تُستخدم مع المتغيرات. (EN) Decorators cannot be used with variables.");
            }
            return parseVarDecl();
        }
    }
    
    // Check if IDENTIFIER - could be function call or expression statement
    // (AR) التحقق إذا كان معرّف - قد يكون استدعاء دالة أو جملة تعبير
    if (check(TT::IDENTIFIER)) {
        // This is either a function call or expression statement
        // Let parseStatement handle it
        std::cout << "Found IDENTIFIER - parsing as statement\n";
        return parseStatement();
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
    
    std::cout << "Parsing statement...\n"<< current_.getTypeName() << "\n";
    if (match(TT::KEYWORD_IF)) {
        return parseIfStmt();
    }
    
    if (match(TT::KEYWORD_WHILE)) {
        return parseWhileStmt();
    }
    
    if (match(TT::KEYWORD_FOR)) {
        return parseForStmt();
    }
    
    if (match(TT::KEYWORD_CASE)) {
        return parseSwitchStmt();
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
                errorExpectedToken("تعبير قيمة", "value expression", "بعد ':' في list comprehension", "after ':' in list comprehension");
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
    
    std::cout << "Parsing declaration after block/map check...\n" ;
    std::cout <<"\n position: "<< current_.getPosition().toString() <<"\n"<< current_.getTypeName()  << "\n";
    ;
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
    
    // Parse function body - starts directly, ends with 'نهاية'
    // (AR) تحليل جسم الدالة - يبدأ مباشرة، ينتهي بـ 'نهاية'
    // Spec: docs/language_spec/rules/02_functions.md - function body ends with 'نهاية'
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
 * @brief (AR) يحلل تصريح صنف كامل مع جميع الأعضاء
 *        (EN) Parses complete class declaration with all members
 * 
 * الصيغة / Syntax:
 *   صنف اسم_الصنف [يرث اسم_الأساس] {
 *       أعضاء...
 *   }
 */
StmtPtr ParserCore::parseClassDecl() {
    std::cout << "[OOP] بدء تحليل تصريح صنف\n";
    
    // (AR) اسم الصنف / (EN) Class name
    // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ::= 'صنف' IDENTIFIER ...
    Token nameToken = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم الصنف بعد 'صنف'. (EN) Expected class name after 'class'.");
    std::string className = nameToken.getValue();
    
    std::cout << "[OOP] اسم الصنف: " << className << "\n";
    
    // (AR) الوراثة (اختياري) - دعم كلا من ':' و'يرث'
    // (EN) Inheritance (optional) - support both ':' and 'يرث'
    // Spec: docs\language_spec\rules\03_oop.md §1,2 - [(':' | 'يرث') base_class_list]
    std::vector<std::string> baseClassNames;
    if (matchAny({TT::COLON, TT::KEYWORD_INHERITS})) {
        // (AR) قائمة الأصناف الأساسية (دعم الفاصلة العربية والإنجليزية)
        // (EN) Base class list (support both Arabic and English commas)
        // Spec: docs\language_spec\rules\03_oop.md §1 - base_class_list ::= IDENTIFIER ((',' | '،') IDENTIFIER)*
        do {
            Token baseToken = consume(TT::IDENTIFIER,
                "(AR) توقع اسم الصنف الأساسي. (EN) Expected base class name.");
            baseClassNames.push_back(baseToken.getValue());
            std::cout << "[OOP] يرث من: " << baseToken.getValue() << "\n";
        } while (matchAny({TT::COMMA, TT::ARABIC_COMMA})); // Support both commas
    }
    
    // (AR) جسم الصنف مباشرة بدون أقواس - ينتهي بـ 'نهاية'
    // (EN) Class body directly without braces - ends with 'نهاية'
    // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ends with 'نهاية' NOT '}'
    // Note: NO BRACE_LEFT here! Class body is parsed directly
    
    // (AR) تحليل أعضاء الصنف / (EN) Parse class members
    // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ends with 'نهاية'
    StmtList members;
    while (!check(TT::KEYWORD_END) && !isAtEnd()) {
        // (AR) تحليل حقول الصنف: نوع اسم_الحقل [؛]
        // (EN) Parse class fields: type field_name [;]
        
        // Check for visibility modifiers or field declaration
        bool isStatic = false;
        bool isVirtual = false;
        bool isAbstract = false;
        AccessModifier access = parseModifiers(isStatic, isVirtual, isAbstract);
        
        // Check if it's a property (starts with 'خاصية' or 'property')
        if (check(TT::KEYWORD_PROPERTY)) {
            advance(); // consume 'خاصية'
            auto property = parsePropertyDeclaration(access, isStatic);
            if (property) {
                members.push_back(std::move(property));
            }
            continue;
        }
        
        // Check if it's a method (starts with 'دالة' or 'function')
        if (check(TT::KEYWORD_FUNCTION)) {
            advance(); // consume 'دالة'
            auto method = parseMethodDeclaration(access, isStatic, isVirtual, isAbstract);
            if (method) {
                members.push_back(std::move(method));
            }
            continue;
        }
        
        // Check for constructor ('باني' keyword or class name followed by '(')
        if (check(TT::KEYWORD_CONSTRUCTOR_ALT) ||  // 'باني'
            (check(TT::IDENTIFIER) && current_.getValue() == className && 
             peekNext().getType() == TT::PAREN_LEFT)) {
            
            if (check(TT::KEYWORD_CONSTRUCTOR_ALT)) {
                advance(); // consume 'باني'
            } else {
                advance(); // consume class name
            }
            
            auto constructor = parseConstructorDeclaration(className, access);
            if (constructor) {
                members.push_back(std::move(constructor));
            }
            continue;
        }
        
        // Check for destructor (keyword 'مدمر')
        if (check(TT::KEYWORD_DESTRUCTOR)) {
            auto destructor = parseDestructorDeclaration(className, access);
            if (destructor) {
                members.push_back(std::move(destructor));
            }
            continue;
        }
        
        // Otherwise, parse as field declaration
        // Field syntax: type name [= value] [;]
        // Support both built-in types (نص، رقم) and class types (شخص، حيوان)
        if (isTypeToken(current_.getType()) || 
            (check(TT::IDENTIFIER) && isClassName(current_.getValue()))) {
            auto field = parseFieldDeclaration(access, isStatic);
            if (field) {
                members.push_back(std::move(field));
            }
        } else {
            // Unknown member, skip to avoid infinite loop
            error("(AR) عضو صنف غير معروف. (EN) Unknown class member.");
            advance(); // Skip token
        }
    }
    
    // (AR) توقع 'نهاية' في نهاية تعريف الصنف
    // (EN) Expect 'نهاية' at end of class definition
    // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ends with 'نهاية'
    consume(TT::KEYWORD_END,
        "(AR) توقع 'نهاية' بعد جسم الصنف. (EN) Expected 'نهاية' after class body.");
    
    std::cout << "[OOP] انتهى تحليل صنف '" << className << "' - "
              << members.size() << " أعضاء\n";
    
    // Store first base class for backward compatibility, or empty string
    std::string firstBaseClass = baseClassNames.empty() ? "" : baseClassNames[0];
    
    return std::make_unique<ClassDecl>(
        className,
        firstBaseClass, // TODO: Update ClassDecl to support multiple base classes
        std::move(members),
        false,
        nameToken.getPosition()
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
    std::string className = "";  // For class-typed variables
    Token name(TT::IDENTIFIER, "");  // Initialize with default
    
    // Check if we have type-first syntax: TYPE IDENTIFIER = value;
    // (AR) التحقق من صيغة النوع أولاً: نوع معرّف = قيمة;
    if (isTypeToken(current_.getType())) {
        // Format 2: TYPE IDENTIFIER = value;
        // Current token is already a type token (TYPE_INTEGER, TYPE_STRING, etc.)
        // (AR) الصيغة 2: نوع معرّف = قيمة;
        // الرمز الحالي هو بالفعل رمز نوع
        std::cout << "[parseVarDecl] Found type token, parsing type-first declaration\n";
        varType = parseType();
        name = consume(TT::IDENTIFIER, 
            "(AR) توقع اسم المتغير بعد النوع. (EN) Expected variable name after type.");
    } else if (check(TT::IDENTIFIER)) {
        // Check if this identifier is a class name (for class-typed variables)
        // (AR) التحقق مما إذا كان هذا المعرّف هو اسم صنف
        if (isClassName(current_.getValue())) {
            // Class-typed variable: ClassName varName = ...;
            // (AR) متغير من نوع صنف: اسم_الصنف اسم_المتغير = ...;
            std::cout << "[parseVarDecl] Found class name, parsing class-typed variable\n";
            className = current_.getValue();
            varType = Data::DataType::OBJECT;
            advance();  // Consume class name
            name = consume(TT::IDENTIFIER,
                "(AR) توقع اسم المتغير بعد نوع الصنف. (EN) Expected variable name after class type.");
        } else {
            // Format 1: var/let/const IDENTIFIER : type = value;
            // or just: IDENTIFIER = value; (type inference)
            // (AR) الصيغة 1: var/let/const معرّف : نوع = قيمة;
            // أو فقط: معرّف = قيمة; (استنتاج النوع)
            std::cout << "[parseVarDecl] Found identifier, parsing standard declaration\n";
            name = consume(TT::IDENTIFIER, 
                "(AR) توقع اسم المتغير. (EN) Expected variable name.");
            
            // Optional type annotation: name : type
            // (AR) تصريح النوع الاختياري: اسم : نوع
            if (match(TT::COLON)) {
                varType = parseType();
            }
        }
    } else {
        // Neither type token nor identifier - this is an error
        // (AR) لا رمز نوع ولا معرّف - هذا خطأ
        errorIncompleteStatement(
            "تصريح المتغير",
            "variable declaration",
            "نوع البيانات أو اسم المتغير",
            "data type or variable name"
        );
        return nullptr;
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
    
    // Parse then branch - directly as block (spec 04_syntax.md)
    // (AR) تحليل فرع then - مباشرة ككتلة
    auto thenBranch = parseBlockStmt();
    
    // Parse optional else branch
    // (AR) تحليل فرع else الاختياري
    StmtPtr elseBranch = nullptr;
    if (match(TT::KEYWORD_ELSE)) {
        // Check for else-if
        if (check(TT::KEYWORD_IF)) {
            advance(); // consume 'if'
            elseBranch = parseIfStmt(); // Recursive for else-if
        } else {
            elseBranch = parseBlockStmt(); // Regular else block
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
        "(AR) توقع '(' بعد 'بينما'. (EN) Expected '(' after 'while'.");
    auto condition = parseExpression();
    consume(TT::PAREN_RIGHT, 
        "(AR) توقع ')' بعد الشرط. (EN) Expected ')' after condition.");
    
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
    consume(TT::PAREN_LEFT, 
        "(AR) توقع '(' بعد 'لكل'. (EN) Expected '(' after 'for'.");
    
    // Parse loop variable
    // (AR) تحليل متغير الحلقة
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
    
    consume(TT::PAREN_RIGHT, 
        "(AR) توقع ')' بعد مجموعة for. (EN) Expected ')' after for collection.");
    
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
    
    // Parse statements until 'نهاية' keyword (spec 04_syntax.md)
    // (AR) تحليل الجمل حتى كلمة 'نهاية'
    while (!check(TT::KEYWORD_END) && !isAtEnd()) {
        auto stmt = parseDeclaration();
        if (stmt) {
            statements.push_back(std::move(stmt));
        }
    }
    
    consume(TT::KEYWORD_END, 
        "(AR) توقع 'نهاية' بعد الكتلة. (EN) Expected 'نهاية' after block.");
    
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
        
        Token exceptionVar = consume(TT::IDENTIFIER, 
            "(AR) توقع اسم متغير الاستثناء. (EN) Expected exception variable name.");
        
        consume(TT::PAREN_RIGHT, 
            "(AR) توقع ')' بعد متغير الاستثناء. (EN) Expected ')' after exception variable.");
        
        // Parse catch body using Arabic syntax
        // (AR) تحليل جسم catch باستخدام الصيغة العربية
        auto catchBody = parseBlockStmt();
        
        catchClauses.push_back(CatchClause(
            exceptionVar.getValue(), 
            Data::DataType::UNKNOWN,  // Type inference will be done in semantic analysis
            std::move(catchBody)
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
            throw std::runtime_error(
                "(AR) توقع 'عندما' أو 'افتراضي' في جملة حالة. "
                "(EN) Expected 'when' or 'default' in switch statement at line " + 
                std::to_string(current_.getPosition().line));
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
    auto expr = parseTernary();
    
    // Check for assignment operator
    // (AR) التحقق من عامل التعيين
    if (match(TT::OP_ASSIGN)) {
        Token equals = previous();
        auto value = parseAssignment();
        
        // Left side must be a variable or member access
        // (AR) الجانب الأيسر يجب أن يكون متغيراً أو وصول لعضو
        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(
                var->name,
                std::move(value),
                equals.getPosition()
            );
        }
        
        // Support member access assignment: obj.field = value
        // (AR) دعم تعيين قيمة لعضو الكائن
        if (auto* member = dynamic_cast<MemberExpr*>(expr.get())) {
            // Extract object and member name, then create MemberAssignExpr
            // Need to clone the object expression since we're consuming expr
            ExprPtr objectCopy;
            
            // We need to transfer ownership properly
            // Release the MemberExpr and extract its parts
            std::unique_ptr<MemberExpr> memberPtr(static_cast<MemberExpr*>(expr.release()));
            
            return std::make_unique<MemberAssignExpr>(
                std::move(memberPtr->object),
                memberPtr->member,
                std::move(value),
                equals.getPosition()
            );
        }
        
        errorBilingual(
            "خطأ: هدف الإسناد غير صالح - يجب أن يكون معرّفاً أو حقل كائن",
            "Error: invalid assignment target - must be identifier or object field"
        );
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل التعبير الثلاثي الشرطي: شرط ? صحيح : خطأ.
 *        (EN) Parses ternary conditional: condition ? true : false.
 * 
 * Grammar / القواعد:
 *   ternary → logical_or ("?" expression ":" ternary)?
 * 
 * Right-associative: a ? b : c ? d : e → a ? b : (c ? d : e)
 */
ExprPtr ParserCore::parseTernary() {
    auto expr = parseLogicalOr();
    
    // Check for ternary operator ?
    // (AR) التحقق من عامل الثلاثي ؟
    if (match(TT::QUESTION)) {
        Token questionToken = previous();
        // Parse true branch
        // (AR) تحليل الفرع الصحيح
        auto trueExpr = parseExpression();
        
        // Expect colon
        // (AR) توقع نقطتان
        consume(TT::COLON,
            "(AR) توقع ':' في التعبير الثلاثي. (EN) Expected ':' in ternary expression.");
        
        // Parse false branch (recursive for right-associativity)
        // (AR) تحليل الفرع الخاطئ (تكراري لتحقيق الربط الأيمن)
        auto falseExpr = parseTernary();
        
        // Create ternary expression node
        // (AR) إنشاء عقدة التعبير الثلاثي
        return std::make_unique<TernaryExpr>(
            std::move(expr),
            std::move(trueExpr),
            std::move(falseExpr),
            questionToken.getPosition()
        );
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
            // Member access or method call
            // (AR) الوصول لعضو أو استدعاء طريقة
            Token member = consume(TT::IDENTIFIER, 
                "(AR) توقع اسم عضو بعد '.'. (EN) Expected member name after '.'.");
            
            // Check if this is a method call: obj.method()
            if (check(TT::PAREN_LEFT)) {
                match(TT::PAREN_LEFT);
                auto args = parseArgumentList();
                consume(TT::PAREN_RIGHT, 
                    "(AR) توقع ')' بعد وسائط الطريقة. (EN) Expected ')' after method arguments.");
                expr = std::make_unique<MethodCallExpr>(
                    std::move(expr),
                    member.getValue()
                );
                // Add arguments
                auto* methodCall = static_cast<MethodCallExpr*>(expr.get());
                methodCall->arguments = std::move(args);
            } else {
                // Regular member access: obj.field
                expr = std::make_unique<MemberExpr>(
                    std::move(expr),
                    member.getValue(),
                    member.getPosition()
                );
            }
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
    // OOP: new expression for object instantiation
    // (AR) تعبير جديد لإنشاء كائن
    if (match(TT::KEYWORD_NEW)) {
        return parseNewExpr();
    }
    
    // OOP: this expression for current object
    // (AR) تعبير هذا للكائن الحالي
    if (match(TT::KEYWORD_THIS)) {
        return std::make_unique<ThisExpr>();
    }
    
    // OOP: super expression for base class
    // (AR) تعبير الأساس للصنف الأب
    if (match(TT::KEYWORD_SUPER)) {
        return std::make_unique<SuperExpr>();
    }
    
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
    
    errorBilingual(
        "خطأ نحوي: توقعت تعبيراً (رقم، نص، معرّف، إلخ) في السطر " + std::to_string(current_.getPosition().line),
        "Syntax error: expected expression (number, string, identifier, etc.) at line " + std::to_string(current_.getPosition().line)
    );
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
        errorBilingual(
            "خطأ في تحليل مفتاح الخريطة - تعبير غير صالح",
            "Failed to parse key expression in map literal - invalid expression"
        );
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
        errorBilingual(
            "خطأ في تحليل قيمة الخريطة - تعبير غير صالح",
            "Failed to parse value expression in map literal - invalid expression"
        );
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