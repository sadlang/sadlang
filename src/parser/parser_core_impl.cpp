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
#include "../../include/data/managers/class_manager.h"
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
    
    // (AR) اسم الملف يجب أن يُعين من خارج Parser
    // (EN) Filename should be set from outside Parser
    // if (filename_.empty()) {
    //     filename_ = "<source>";
    // }
    
    // DEBUG: Disabled
    // std::cout << "[parser_core_impl.cpp] بدء parseProgram - current token: " 
    //           << static_cast<int>(current_.getType()) << " = '" 
    //           << current_.getValue() << "'\n";
    
    int statement_count = 0;
    int error_count = 0;
    
    // (AR) حماية ضد الحلقة اللانهائية - تتبع الموقع الحالي
    // (EN) Infinite loop protection - track current position
    size_t last_position = 0;
    int stuck_count = 0;
    const int MAX_STUCK_ITERATIONS = 3;  // Allow 3 attempts at same position
    
    // Parse until EOF
    // (AR) التحليل حتى نهاية الملف
    while (!isAtEnd()) {
        // (AR) فحص الحلقة اللانهائية: هل نحن عالقون في نفس الموقع؟
        // (EN) Infinite loop detection: are we stuck at the same position?
        size_t current_position = current_.getPosition().offset;
        
        if (current_position == last_position) {
            stuck_count++;
            if (stuck_count >= MAX_STUCK_ITERATIONS) {
                std::cerr << "\n";
                std::cerr << "❌❌❌ ========================================\n";
                std::cerr << "  🚨 (AR) اكتشاف حلقة لا نهائية!\n";
                std::cerr << "  🚨 (EN) Infinite Loop Detected!\n";
                std::cerr << "========================================\n";
                std::cerr << "📍 (AR) عالق في السطر " << current_.getPosition().line 
                          << ", العمود " << current_.getPosition().column << "\n";
                std::cerr << "📍 (EN) Stuck at line " << current_.getPosition().line 
                          << ", column " << current_.getPosition().column << "\n";
                std::cerr << "🔎 (AR) الرمز: '" << current_.getValue() << "'\n";
                std::cerr << "🔎 (EN) Token: '" << current_.getValue() << "'\n";
                std::cerr << "⚠ (AR) القفز للرمز التالي لكسر الحلقة...\n";
                std::cerr << "⚠ (EN) Forcing advance to break the loop...\n";
                std::cerr << "========================================\n\n";
                
                // Force advance to break infinite loop
                advance();
                stuck_count = 0;
                error_count++;
                continue;
            }
        } else {
            stuck_count = 0;
            last_position = current_position;
        }
        
        try {
            // DEBUG: Disabled
            // std::cout << "[parser_core_impl.cpp] داخل حلقة parseProgram - current token: " 
            //           << static_cast<int>(current_.getType()) << " = '" 
            //           << current_.getValue() << "'\n";
            
            auto stmt = parseDeclaration();
            if (stmt) {
                statements.push_back(std::move(stmt));
                statement_count++;
                // DEBUG: Disabled
                // std::cout << "[parser_core_impl.cpp] تمت إضافة جملة - العدد الكلي: " 
                //           << statements.size() << "\n";
            }
        } catch (const std::exception& e) {
            error_count++;
            
            // (AR) عرض معلومات تفصيلية عن الخطأ
            // (EN) Display detailed error information
            std::cerr << "\n";
            std::cerr << "❗❗❗ ========================================\n";
            std::cerr << "  ⛔ (AR) خطأ في التحليل النحوي\n";
            std::cerr << "  ⛔ (EN) Parsing Error\n";
            std::cerr << "========================================\n";
            std::cerr << "📄 (AR) الملف: " << (filename_.empty() ? "<source>" : filename_) << "\n";
            std::cerr << "📄 (EN) File: " << (filename_.empty() ? "<source>" : filename_) << "\n";
            std::cerr << "📍 (AR) السطر: " << current_.getPosition().line 
                      << ", العمود: " << current_.getPosition().column << "\n";
            std::cerr << "📍 (EN) Line: " << current_.getPosition().line 
                      << ", Column: " << current_.getPosition().column << "\n";
            std::cerr << "💬 (AR) الرسالة: " << e.what() << "\n";
            std::cerr << "💬 (EN) Message: " << e.what() << "\n";
            std::cerr << "========================================\n\n";
            
            error(e.what());
            synchronize();
        } catch (...) {
            error_count++;
            
            // (AR) معالجة أي استثناء غير معروف
            // (EN) Handle any unknown exception
            std::cerr << "\n";
            std::cerr << "❗❗❗ ========================================\n";
            std::cerr << "  ⛔ (AR) خطأ غير معروف في التحليل النحوي\n";
            std::cerr << "  ⛔ (EN) Unknown Parsing Error\n";
            std::cerr << "========================================\n";
            std::cerr << "📄 (AR) الملف: " << (filename_.empty() ? "<source>" : filename_) << "\n";
            std::cerr << "📄 (EN) File: " << (filename_.empty() ? "<source>" : filename_) << "\n";
            std::cerr << "📍 (AR) السطر: " << current_.getPosition().line 
                      << ", العمود: " << current_.getPosition().column << "\n";
            std::cerr << "📍 (EN) Line: " << current_.getPosition().line 
                      << ", Column: " << current_.getPosition().column << "\n";
            std::cerr << "========================================\n\n";
            
            errorBilingual(
                "خطأ غير معروف أثناء التحليل النحوي",
                "Unknown error during syntactic analysis"
            );
            synchronize();
        }
    }
    
    // (AR) طباعة ملخص التحليل
    // (EN) Print parsing summary
    std::cerr << "\n";
    std::cerr << "========================================\n";
    std::cerr << "  📊 (AR) ملخص التحليل / (EN) Parsing Summary\n";
    std::cerr << "========================================\n";
    std::cerr << "✓ (AR) عدد الجمل الناجحة: " << statement_count << "\n";
    std::cerr << "✓ (EN) Successful statements: " << statement_count << "\n";
    
    if (error_count > 0) {
        std::cerr << "❌ (AR) عدد الأخطاء: " << error_count << "\n";
        std::cerr << "❌ (EN) Error count: " << error_count << "\n";
    }
    std::cerr << "========================================\n\n";
    
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
    // (AR) التحقق من المُزخرِفات قبل التصريح
    // (EN) Check for decorators before declaration
    ExprList decorators;
    while (check(TT::AT_SIGN)) {
        match(TT::AT_SIGN);  // consume @
        decorators.push_back(parseDecorator());
    }
    
    // Check for declaration keywords
    // (AR) التحقق من كلمات التصريح المفتاحية
    
    // (AR) استيراد / (EN) Import
    if (match(TT::KEYWORD_IMPORT)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مع الاستيراد. (EN) Decorators cannot be used with imports.");
        }
        return parseImportStmt();
    }
    
    // (AR) استيراد انتقائي (من...استورد) / (EN) From-import
    if (match(TT::KEYWORD_FROM)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مع الاستيراد. (EN) Decorators cannot be used with imports.");
        }
        return parseFromImportStmt();
    }
    
    // (AR) تصدير / (EN) Export
    if (match(TT::KEYWORD_EXPORT)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مباشرة مع التصدير. (EN) Decorators cannot be used directly with export.");
        }
        return parseExportDecl();
    }
    
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
    
    // Check for type-first variable declaration: type IDENTIFIER = value;
    // Use proper lookahead to verify TYPE is followed by IDENTIFIER
    // (AR) التحقق من تصريح المتغير ببدء النوع: نوع معرّف = قيمة;
    // استخدام النظر المسبق الصحيح للتحقق أن TYPE متبوع بـ IDENTIFIER
    if (isTypeToken(current_.getType())) {
        // Look ahead to see if next token is IDENTIFIER
        const Token& nextTok = peekNext();
            
        if (nextTok.getType() == TT::IDENTIFIER) {
            // Valid variable declaration: TYPE IDENTIFIER
            if (!decorators.empty()) {
                error("(AR) المُزخرِفات لا تُستخدم مع المتغيرات. (EN) Decorators cannot be used with variables.");
            }
            return parseVarDecl();
        } else {
            // TYPE token not followed by IDENTIFIER - this is an error
            // The user wrote a type keyword but didn't follow it with a variable name
            // (AR) رمز النوع غير متبوع بمعرّف - خطأ نحوي
            std::string typeStr = current_.getValue();
            error(
                "(AR) خطأ نحوي: بعد كلمة النوع '" + typeStr + "' يجب أن يأتي اسم المتغير.\n" +
                "مثال صحيح: " + typeStr + " اسم_المتغير = قيمة\n" +
                "الموقع: السطر " + std::to_string(current_.getPosition().line) + "\n" +
                "(EN) Syntax error: After type keyword '" + typeStr + "' expected variable name.\n" +
                "Correct example: " + typeStr + " variable_name = value\n" +
                "Location: line " + std::to_string(current_.getPosition().line)
            );
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
        synchronize();
        return nullptr;
    }
    
    // Check for unexpected tokens that shouldn't appear at statement start
    // (AR) التحقق من الرموز غير المتوقعة في بداية الجملة
    if (check(TT::COLON) || check(TT::SEMICOLON) || check(TT::ARABIC_SEMICOLON) ||
        check(TT::COMMA) || check(TT::ARABIC_COMMA) ||
        check(TT::BRACE_RIGHT) || check(TT::BRACKET_RIGHT) || check(TT::PAREN_RIGHT)) {
        
        std::string tokenVal = current_.getValue();
        std::string tokenDesc;
        
        if (current_.getType() == TT::COLON) {
            tokenDesc = "colon ':'";
        } else if (current_.getType() == TT::SEMICOLON || current_.getType() == TT::ARABIC_SEMICOLON) {
            tokenDesc = "semicolon ';'";
        } else if (current_.getType() == TT::COMMA || current_.getType() == TT::ARABIC_COMMA) {
            tokenDesc = "comma ','";
        } else if (current_.getType() == TT::BRACE_RIGHT) {
            tokenDesc = "closing brace '}'";
        } else if (current_.getType() == TT::BRACKET_RIGHT) {
            tokenDesc = "closing bracket ']'";
        } else if (current_.getType() == TT::PAREN_RIGHT) {
            tokenDesc = "closing parenthesis ')'";
        }
        
        // Show detailed error message in both languages
        errorBilingual(
            "(AR) خطأ نحوي: رمز غير متوقع '" + tokenVal + "' (" + tokenDesc + ") في بداية جملة أو تصريح.\n" +
            "   تحقق من الكود - قد يكون هناك:\n" +
            "   - فاصل منقوط أو فاصل غير ضروري\n" +
            "   - قوس إغلاق بدون قوس فتح\n" +
            "   - جملة ناقصة أو غير صحيحة\n" +
            "   السطر: " + std::to_string(current_.getPosition().line),
            "(EN) Syntax error: unexpected token '" + tokenVal + "' (" + tokenDesc + ") at statement start.\n" +
            "   Check your code - there might be:\n" +
            "   - unnecessary semicolon or separator\n" +
            "   - closing bracket/brace without opening\n" +
            "   - incomplete or malformed statement\n" +
            "   Line: " + std::to_string(current_.getPosition().line)
        );
        synchronize();
        return nullptr;
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
        // Save position
        Token brace = current_;
        advance(); // consume {
        
        // Check for empty map
        if (check(TT::BRACE_RIGHT)) {
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
            
            while (match(TT::COMMA) || match(TT::ARABIC_COMMA)) {
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
    // Spec: docs/language_spec/rules/02_functions.md - function_decl ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
    // Optional return type BEFORE function name: دالة [type] name(...)
    // (AR) نوع الإرجاع الاختياري قبل اسم الدالة: دالة [نوع] اسم(...)
    Data::DataType returnType = Data::DataType::UNKNOWN;
    
    // Check if next token is a type keyword (before function name)
    // (AR) التحقق إذا كان الرمز التالي هو نوع (قبل اسم الدالة)
    if (check(TT::TYPE_INTEGER) || check(TT::TYPE_DOUBLE) || 
        check(TT::TYPE_STRING) || check(TT::TYPE_BOOLEAN) ||
        check(TT::TYPE_ARRAY) || check(TT::TYPE_MAP)) {
        returnType = parseType();
    }
    
    // Expect function name
    // (AR) توقع اسم الدالة
    Token name = consume(TT::IDENTIFIER, 
        "(AR) خطأ نحوي: بعد كلمة 'دالة' (أو بعد نوع الإرجاع) يجب أن يأتي اسم الدالة.\n"
        "مثال: دالة جمع(...) أو دالة رقم مربع(...)\n"
        "(EN) Syntax error: After 'function' keyword (or return type) expected function name.\n"
        "Example: function sum(...) or function int square(...)");
    
    // Parse parameter list (now with type annotations)
    // (AR) تحليل قائمة المعاملات (الآن مع تصريحات الأنواع)
    consume(TT::PAREN_LEFT, 
        "(AR) خطأ نحوي: بعد اسم الدالة يجب أن يأتي قوس مفتوح '('.\n"
        "مثال: دالة " + name.getValue() + "(...) \n"
        "(EN) Syntax error: After function name expected '('.\n"
        "Example: function " + name.getValue() + "(...)");
    auto paramObjs = parseTypedParameterList();
    consume(TT::PAREN_RIGHT, 
        "(AR) خطأ نحوي: بعد قائمة المعاملات يجب أن يأتي قوس مغلق ')'.\n"
        "مثال: دالة " + name.getValue() + "(معامل١، معامل٢) \n"
        "تأكد من أن قائمة المعاملات مكتملة ومفصولة بفواصل صحيحة.\n"
        "(EN) Syntax error: After parameter list expected ')'.\n"
        "Example: function " + name.getValue() + "(param1, param2)\n"
        "Make sure parameter list is complete and properly separated.");
    
    // Optional return type annotation AFTER parameters: function name(...) : type
    // (AR) تصريح نوع الإرجاع الاختياري بعد المعاملات: دالة اسم(...) : نوع
    // Note: This overrides the type specified before the function name (if any)
    if (match(TT::COLON)) {
        returnType = parseType();
    }
    
    // Parse function body - starts directly, ends with 'نهاية'
    // (AR) تحليل جسم الدالة - يبدأ مباشرة، ينتهي بـ 'نهاية'
    // Spec: docs/language_spec/rules/02_functions.md - function body ends with 'نهاية'
    auto body = parseBlockStmt();
    
    // (AR) التحقق من توافق نوع الإرجاع
    // (EN) Check return type compatibility
    // If function has UNKNOWN return type (no type specified), it should not have return statements with values
    if (returnType == Data::DataType::UNKNOWN) {
        // Check if body contains return statements with values
        // This is a simplified check - proper semantic analysis would be better
        // For now, we just warn at parse time
        // Note: This check will be done at runtime by the interpreter
    }
    
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
    
    // (AR) تسجيل الصنف مبكراً في ClassManager للسماح بمتغيرات من هذا النوع لاحقاً في نفس الملف
    // (EN) Register class early in ClassManager to allow variables of this type later in same file
    // هذا تسجيل مؤقت - سيتم تحديثه عند التنفيذ بكامل التفاصيل
    // This is temporary registration - will be updated during execution with full details
    auto* classManager = Data::ClassManager::getInstance();
    if (!classManager->hasClass(className)) {
        auto tempClassType = std::make_unique<Data::ClassType>(className);
        classManager->registerClass(std::move(tempClassType));
        std::cout << "[OOP] ✅ تسجيل مؤقت للصنف: " << className << " (أثناء التحليل)\n";
    }
    
    // (AR) استخدام جميع الأصناف الأساسية بدلاً من الأول فقط / (EN) Use all base classes instead of just first
    return std::make_unique<ClassDecl>(
        className,
        baseClassNames, // (AR) دعم الوراثة المتعددة الكامل / (EN) Full multiple inheritance support
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
        varType = parseType();
        
        // Check if we have an identifier after the type
        // (AR) تحقق مما إذا كان لدينا معرّف بعد النوع
        if (!check(TT::IDENTIFIER)) {
            // Missing identifier after type specification
            // (AR) معرّف مفقود بعد تحديد النوع
            errorBilingual(
                "توقع اسم متغير بعد تحديد النوع. اسم المتغير يجب أن يكون معرّف صحيح.",
                "Expected variable name after type specification. Variable name must be a valid identifier."
            );
            return nullptr;
        }
        
        name = peek();
        advance();
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
            
            // Check if identifier follows the class name
            // (AR) تحقق مما إذا كان معرّف يتبع اسم الصنف
            if (!check(TT::IDENTIFIER)) {
                errorBilingual(
                    "توقع اسم متغير من نوع الصنف '" + className + "'. اسم المتغير يجب أن يكون معرّف صحيح.",
                    "Expected variable name of class type '" + className + "'. Variable name must be a valid identifier."
                );
                return nullptr;
            }
            
            name = peek();
            advance();
        } else {
            // Format 1: var/let/const IDENTIFIER : type = value;
            // or just: IDENTIFIER = value; (type inference)
            // (AR) الصيغة 1: var/let/const معرّف : نوع = قيمة;
            // أو فقط: معرّف = قيمة; (استنتاج النوع)
            std::cout << "[parseVarDecl] Found identifier, parsing standard declaration\n";
            name = peek();
            advance();
            
            // Optional type annotation: name : type
            // (AR) تصريح النوع الاختياري: اسم : نوع
            if (match(TT::COLON)) {
                // We have a type annotation, parse it
                // (AR) لدينا تصريح نوع، قم بتحليله
                Data::DataType annotatedType = parseType();
                
                // Check if the type was parsed successfully
                // (AR) تحقق مما إذا تم تحليل النوع بنجاح
                if (annotatedType == Data::DataType::UNKNOWN) {
                    errorBilingual(
                        "نوع غير صحيح أو غير معروف بعد ':' في تصريح المتغير '" + name.getValue() + "'.",
                        "Invalid or unknown type after ':' in variable declaration '" + name.getValue() + "'."
                    );
                    return nullptr;
                }
                
                varType = annotatedType;
            }
        }
    } else {
        // Neither type token nor identifier - this is an error
        // (AR) لا رمز نوع ولا معرّف - هذا خطأ
        Token currentToken = peek();
        
        // Check what kind of token we got instead
        // (AR) تحقق من نوع الرمز الذي حصلنا عليه بدلاً من ذلك
        std::string errorMsg_ar = "صيغة تصريح متغير غير صحيحة.";
        std::string errorMsg_en = "Invalid variable declaration syntax.";
        
        // Provide more helpful error messages based on what we found
        // (AR) قدّم رسائل خطأ أكثر فائدة بناءً على ما وجدنا
        if (currentToken.getType() == TT::SEMICOLON) {
            errorMsg_ar = "لا يمكن تصريح متغير بدون اسم أو نوع.";
            errorMsg_en = "Cannot declare variable without name or type.";
        } else if (currentToken.getType() == TT::BRACE_LEFT || currentToken.getType() == TT::BRACE_RIGHT) {
            errorMsg_ar = "لا يمكن تصريح متغير في هذا الموقع. هل قصدت البحث عن شيء آخر؟";
            errorMsg_en = "Cannot declare variable at this location. Did you mean something else?";
        }
        
        errorBilingual(
            errorMsg_ar,
            errorMsg_en
        );
        return nullptr;
    }
    
    // Optional initializer
    // (AR) المُهيّئ الاختياري
    ExprPtr initializer = nullptr;
    if (match(TT::OP_ASSIGN)) {
        // We have an assignment operator, parse the expression
        // (AR) لدينا عامل إسناد، قم بتحليل التعبير
        initializer = parseExpression();
        
        // Check if expression parsing was successful
        // (AR) تحقق مما إذا كان تحليل التعبير ناجحاً
        if (!initializer) {
            errorBilingual(
                "تعبير غير صحيح في قيمة تهيئة المتغير '" + name.getValue() + "'. تأكد من أن التعبير صحيح.",
                "Invalid expression in variable initializer for '" + name.getValue() + "'. Ensure the expression is correct."
            );
            return nullptr;
        }
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
    if (!check(TT::IDENTIFIER)) {
        errorBilingual(
            "خطأ: توقعت اسم Enum بعد كلمة 'تعداد'. مثال: تعداد اللون",
            "Error: expected enum name after 'enum' keyword. Example: enum Color"
        );
        return nullptr;
    }
    Token name = peek();
    advance();
    
    if (!check(TT::BRACE_LEFT)) {
        errorBilingual(
            "خطأ: توقعت '{' قبل أعضاء Enum. الصيغة: تعداد اسم { أعضاء }",
            "Error: expected '{' before enum members. Format: enum name { members }"
        );
        return nullptr;
    }
    consume(TT::BRACE_LEFT, "");
    
    // Parse enum members
    // (AR) تحليل أعضاء Enum
    std::vector<EnumMember> members;
    
    // Check for empty enum
    if (check(TT::BRACE_RIGHT)) {
        errorBilingual(
            "خطأ: تعداد فارغ. يجب أن يحتوي التعداد على عضو واحد على الأقل.",
            "Error: empty enum. Enum must have at least one member."
        );
        advance(); // consume }
        return nullptr;
    }
    
    while (!check(TT::BRACE_RIGHT) && !isAtEnd()) {
        if (!check(TT::IDENTIFIER)) {
            errorBilingual(
                "خطأ: توقعت اسم عضو في Enum. الأعضاء يجب أن تكون معرفات.",
                "Error: expected member name in enum. Members must be identifiers."
            );
            return nullptr;
        }
        Token memberName = peek();
        advance();
        
        ExprPtr value = nullptr;
        if (match(TT::OP_ASSIGN)) {
            // Explicit value
            // (AR) قيمة صريحة
            value = parseExpression();
            if (!value) {
                errorBilingual(
                    "خطأ: قيمة عضو Enum غير صحيحة. يجب أن تكون القيمة تعبيراً صحيحاً.",
                    "Error: invalid enum member value. Value must be a valid expression."
                );
                return nullptr;
            }
        }
        
        members.push_back(EnumMember(memberName.getValue(), std::move(value)));
        
        if (!check(TT::BRACE_RIGHT)) {
            if (!check(TT::COMMA)) {
                errorBilingual(
                    "خطأ: توقعت ',' بين أعضاء Enum. الصيغة: العضو1, العضو2, ...",
                    "Error: expected ',' between enum members. Format: member1, member2, ..."
                );
                return nullptr;
            }
            consume(TT::COMMA, "");
        }
    }
    
    if (!check(TT::BRACE_RIGHT)) {
        errorBilingual(
            "خطأ: لم يتم إغلاق Enum. توقعت '}' في النهاية.",
            "Error: enum not closed. Expected '}' at end."
        );
        return nullptr;
    }
    consume(TT::BRACE_RIGHT, "");
    
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
 * @brief (AR) يحلل جملة تصدير: صدّر identifier;
 *        (EN) Parses export statement: export identifier;
 */
StmtPtr ParserCore::parseExportStmt() {
    // Parse the declaration to be exported
    // (AR) تحليل التصريح المُصدّر
    auto declaration = parseDeclaration();
    if (!declaration) {
        errorBilingual(
            "خطأ: فشل تحليل التصريح المُصدّر. يجب أن يتبع 'صدّر' تصريح صحيح (دالة، صنف، إلخ).",
            "Error: failed to parse export declaration. 'export' must be followed by a valid declaration (function, class, etc.)."
        );
        return nullptr;
    }
    
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
    
    // Parse return value if present (not semicolon)
    // (AR) تحليل قيمة الإرجاع إذا كانت موجودة (ليست فاصلة منقوطة)
    if (!check(TT::SEMICOLON)) {
        value = parseExpression();
    }
    
    // Semicolon is optional after return statement
    // (AR) الفاصلة المنقوطة اختيارية بعد جملة return
    if (check(TT::SEMICOLON)) {
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
        
        
        // (AR) قراءة نوع الاستثناء (اختياري) ثم اسم المتغير / (EN) Read optional exception type then variable name
        //  Note: Full implementation would require lookahead - simplified for now
        Data::DataType exceptionType = Data::DataType::UNKNOWN;
        
        Token exceptionVar = consume(TT::IDENTIFIER, 
            "(AR) توقع اسم متغير الاستثناء. (EN) Expected exception variable name.");        consume(TT::PAREN_RIGHT, 
            "(AR) توقع ')' بعد متغير الاستثناء. (EN) Expected ')' after exception variable.");
        
        // Parse catch body using Arabic syntax
        // (AR) تحليل جسم catch باستخدام الصيغة العربية
        auto catchBody = parseBlockStmt();
        
        catchClauses.push_back(CatchClause(
            exceptionVar.getValue(), 
            exceptionType,  // (AR) استخدام النوع المُحلل / (EN) Use parsed type
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
        
        // Check if expression parsed successfully
        // (AR) تحقق إذا تم تحليل التعبير بنجاح
        if (!expr) {
            errorBilingual(
                "تعبير غير صحيح بين الأقواس. تأكد من أن التعبير يحتوي على قيم أو عمليات صحيحة.",
                "Invalid expression in parentheses. Make sure the expression contains valid values or operations."
            );
            return nullptr;
        }
        
        consume(TT::PAREN_RIGHT, 
            "(AR) توقع ')' بعد التعبير. تأكد من إغلاق جميع الأقواس بشكل صحيح. (EN) Expected ')' after expression. Make sure all parentheses are properly closed.");
        return expr;
    }
    
    // Array literal
    // (AR) مصفوفة حرفية
    if (match(TT::BRACKET_LEFT)) {
        auto result = parseArrayLiteral();
        if (!result) {
            errorBilingual(
                "خطأ في تحليل مصفوفة حرفية. تأكد من صيغة المصفوفة: [عنصر1، عنصر2، ...] أو [x for x in list]",
                "Error parsing array literal. Make sure array syntax is correct: [elem1, elem2, ...] or [x for x in list]"
            );
            return nullptr;
        }
        return result;
    }
    
    // Map literal
    // (AR) خريطة حرفية
    if (match(TT::BRACE_LEFT)) {
        auto result = parseMapLiteral();
        if (!result) {
            errorBilingual(
                "خطأ في تحليل خريطة حرفية. تأكد من صيغة الخريطة: {مفتاح: قيمة، ...}",
                "Error parsing map literal. Make sure map syntax is correct: {key: value, ...}"
            );
            return nullptr;
        }
        return result;
    }
    
    // Provide more specific error message based on what we found
    // (AR) قدم رسالة خطأ أكثر تحديداً بناءً على ما وجدنا
    Token current = peek();
    std::string errorMsg_ar = "خطأ نحوي: توقعت تعبيراً (رقم، نص، معرّف، إلخ).";
    std::string errorMsg_en = "Syntax error: expected expression (number, string, identifier, etc.).";
    
    // Provide specific guidance based on the token type
    // (AR) قدم توجيهات محددة بناءً على نوع الرمز
    switch (current.getType()) {
        case TT::END_OF_FILE:
            errorMsg_ar = "خطأ: نهاية ملف غير متوقعة - تعبير غير مكتمل.";
            errorMsg_en = "Error: unexpected end of file - incomplete expression.";
            break;
        case TT::SEMICOLON:
            errorMsg_ar = "خطأ: فاصلة منقوطة بدون تعبير سابق. هل نسيت التعبير قبلها؟";
            errorMsg_en = "Error: semicolon without preceding expression. Did you forget the expression?";
            break;
        case TT::KEYWORD_END:
            errorMsg_ar = "خطأ: توقعت تعبيراً قبل كلمة 'نهاية'.";
            errorMsg_en = "Error: expected expression before 'end' keyword.";
            break;
        case TT::BRACE_RIGHT:
            errorMsg_ar = "خطأ: قوس معقوف إغلاق بدون تعبير. هل نسيت العنصر الأخير؟";
            errorMsg_en = "Error: closing brace without expression. Did you forget the last element?";
            break;
        case TT::BRACKET_RIGHT:
            errorMsg_ar = "خطأ: قوس مربع إغلاق بدون عنصر. هل نسيت العنصر الأخير؟";
            errorMsg_en = "Error: closing bracket without element. Did you forget the last element?";
            break;
        case TT::PAREN_RIGHT:
            errorMsg_ar = "خطأ: قوس إغلاق بدون تعبير. هل نسيت الوسيط أو التعبير؟";
            errorMsg_en = "Error: closing parenthesis without expression. Did you forget the argument or expression?";
            break;
        case TT::COMMA:
            errorMsg_ar = "خطأ: فاصلة بدون عنصر سابق. هل نسيت عنصراً أو معامل؟";
            errorMsg_en = "Error: comma without preceding element. Did you forget an element or argument?";
            break;
        default:
            errorMsg_ar = "خطأ نحوي: رمز غير متوقع '" + current.getValue() + "' في موضع تعبير.";
            errorMsg_en = "Syntax error: unexpected token '" + current.getValue() + "' in expression position.";
            break;
    }
    
    errorBilingual(errorMsg_ar, errorMsg_en);
    
    // (AR) استهلاك الرمز غير الصالح لمنع الحلقة اللانهائية
    // (EN) Consume the invalid token to prevent infinite loop
    // CRITICAL: Must advance to prevent infinite parsing loop
    if (!isAtEnd()) {
        advance();
    }
    
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
        if (paramNames.empty() && !check(TT::COLON)) {
            errorBilingual(
                "خطأ: فشل تحليل قائمة معاملات lambda أو توقعت ':'.",
                "Error: failed to parse lambda parameter list or expected ':'."
            );
            return nullptr;
        }
    }
    
    // Check for colon separator
    // (AR) التحقق من ':' الفاصل
    if (!check(TT::COLON)) {
        errorBilingual(
            "خطأ: توقعت ':' بعد معاملات lambda. الصيغة: (x: x * 2) أو (x, y: x + y)",
            "Error: expected ':' after lambda parameters. Format: (x: x * 2) or (x, y: x + y)"
        );
        return nullptr;
    }
    consume(TT::COLON, "");
    
    // Parse body expression
    // (AR) تحليل تعبير الجسم
    auto body = parseExpression();
    if (!body) {
        errorBilingual(
            "خطأ: فشل تحليل جسم lambda - تعبير غير صحيح.",
            "Error: failed to parse lambda body - invalid expression."
        );
        return nullptr;
    }
    
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
    if (!check(TT::IDENTIFIER)) {
        errorBilingual(
            "خطأ: توقعت اسم المُزخرِف بعد @. مثال: @property أو @staticmethod",
            "Error: expected decorator name after @. Example: @property or @staticmethod"
        );
        return nullptr;
    }
    Token decoratorName = peek();
    advance();
    
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
            if (args.empty() && !check(TT::PAREN_RIGHT)) {
                errorBilingual(
                    "خطأ: فشل تحليل وسائط المُزخرِف.",
                    "Error: failed to parse decorator arguments."
                );
                return nullptr;
            }
        }
        
        if (!check(TT::PAREN_RIGHT)) {
            errorBilingual(
                "خطأ: توقعت ')' بعد وسائط المُزخرِف. هل نسيت إغلاق الأقواس؟",
                "Error: expected ')' after decorator arguments. Did you forget to close the parenthesis?"
            );
            return nullptr;
        }
        consume(TT::PAREN_RIGHT, "");
        
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
    if (!firstExpr) {
        errorBilingual(
            "خطأ: لم يتم تحليل التعبير الأول في المصفوفة. تأكد من أن التعبير صحيح.",
            "Error: failed to parse first expression in array. Make sure the expression is valid."
        );
        return nullptr;
    }
    
    // Check if this is a list comprehension
    // (AR) التحقق إذا كان list comprehension
    if (check(TT::KEYWORD_FOR)) {
        // This is a list comprehension: [expr for var in iterable if cond]
        // (AR) هذا list comprehension
        
        advance(); // consume 'for'
        
        // Parse variable
        // (AR) تحليل المتغير
        if (!check(TT::IDENTIFIER)) {
            errorBilingual(
                "خطأ: توقعت اسم متغير بعد 'for' في list comprehension. مثال: [x for x in list]",
                "Error: expected variable name after 'for' in list comprehension. Example: [x for x in list]"
            );
            return nullptr;
        }
        Token var = peek();
        advance();
        
        // Expect 'in' keyword
        // (AR) توقع كلمة 'في'
        if (!check(TT::KEYWORD_IN)) {
            errorBilingual(
                "خطأ: توقعت 'في' بعد اسم المتغير في list comprehension. الصيغة: [expr for var in iterable]",
                "Error: expected 'in' after variable name in list comprehension. Format: [expr for var in iterable]"
            );
            return nullptr;
        }
        advance();
        
        // Parse iterable
        // (AR) تحليل المجموعة القابلة للتكرار
        auto iterable = parseExpression();
        if (!iterable) {
            errorBilingual(
                "خطأ: فشل تحليل المجموعة القابلة للتكرار بعد 'في'. تأكد من صيغة list comprehension.",
                "Error: failed to parse iterable after 'in'. Make sure list comprehension syntax is correct."
            );
            return nullptr;
        }
        
        // Optional condition
        // (AR) الشرط الاختياري
        ExprPtr condition = nullptr;
        if (match(TT::KEYWORD_IF)) {
            condition = parseExpression();
            if (!condition) {
                errorBilingual(
                    "خطأ: تعبير شرط غير صحيح بعد 'إذا' في list comprehension.",
                    "Error: invalid condition expression after 'if' in list comprehension."
                );
                return nullptr;
            }
        }
        
        if (!check(TT::BRACKET_RIGHT)) {
            errorBilingual(
                "خطأ: توقعت ']' في نهاية list comprehension. الصيغة: [expr for var in iterable if cond]",
                "Error: expected ']' at end of list comprehension. Format: [expr for var in iterable if cond]"
            );
            return nullptr;
        }
        consume(TT::BRACKET_RIGHT, "");
        
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
    while (match(TT::COMMA) || match(TT::ARABIC_COMMA)) {
        if (check(TT::BRACKET_RIGHT)) {
            break; // Trailing comma is allowed
        }
        
        auto elem = parseExpression();
        if (!elem) {
            errorBilingual(
                "خطأ: تعبير عنصر غير صحيح في المصفوفة. تأكد من صيغة العنصر.",
                "Error: invalid element expression in array. Make sure element syntax is correct."
            );
            return nullptr;
        }
        elements.push_back(std::move(elem));
    }
    
    if (!check(TT::BRACKET_RIGHT)) {
        errorBilingual(
            "خطأ: لم يتم إغلاق المصفوفة. توقعت ']'. هل نسيت إغلاق القوس المربع؟",
            "Error: array not closed. Expected ']'. Did you forget to close the bracket?"
        );
        return nullptr;
    }
    consume(TT::BRACKET_RIGHT, "");
    
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
            "خطأ: فشل تحليل مفتاح الخريطة - تعبير غير صحيح. تأكد من أن المفتاح عبارة عن نص أو رقم أو متغير.",
            "Error: failed to parse map key - invalid expression. Make sure the key is a string, number, or variable."
        );
        return nullptr;
    }
    
    // Expect colon after key
    // (AR) توقع ':' بعد المفتاح
    if (!check(TT::COLON)) {
        errorBilingual(
            "خطأ: توقعت ':' بعد مفتاح الخريطة. الصيغة: {مفتاح: قيمة، ...}",
            "Error: expected ':' after map key. Format: {key: value, ...}"
        );
        return nullptr;
    }
    consume(TT::COLON, "");
    
    // Parse first value expression (or value variable template in comprehension)
    // (AR) تحليل تعبير القيمة الأولى (أو قالب متغير القيمة في comprehension)
    auto firstValue = parseExpression();
    if (!firstValue) {
        errorBilingual(
            "خطأ: فشل تحليل قيمة الخريطة - تعبير غير صحيح. تأكد من أن القيمة صحيحة.",
            "Error: failed to parse map value - invalid expression. Make sure the value is valid."
        );
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
        if (!check(TT::IDENTIFIER)) {
            errorBilingual(
                "خطأ: توقعت اسم متغير حلقة بعد 'for' في dict comprehension. مثال: {k: v for item in list}",
                "Error: expected loop variable name after 'for' in dict comprehension. Example: {k: v for item in list}"
            );
            return nullptr;
        }
        Token loopVar = peek();
        advance();
        
        // Expect 'in' keyword
        // (AR) توقع كلمة 'في'
        if (!check(TT::KEYWORD_IN)) {
            errorBilingual(
                "خطأ: توقعت 'في' بعد متغير الحلقة. الصيغة: {k: v for var in iterable}",
                "Error: expected 'in' after loop variable. Format: {k: v for var in iterable}"
            );
            return nullptr;
        }
        advance();
        
        // Parse iterable
        // (AR) تحليل المجموعة القابلة للتكرار
        auto iterable = parseExpression();
        if (!iterable) {
            errorBilingual(
                "خطأ: فشل تحليل المجموعة القابلة للتكرار في dict comprehension.",
                "Error: failed to parse iterable in dict comprehension."
            );
            return nullptr;
        }
        
        // Optional condition
        // (AR) الشرط الاختياري
        ExprPtr condition = nullptr;
        if (match(TT::KEYWORD_IF)) {
            condition = parseExpression();
            if (!condition) {
                errorBilingual(
                    "خطأ: تعبير شرط غير صحيح بعد 'إذا' في dict comprehension.",
                    "Error: invalid condition expression after 'if' in dict comprehension."
                );
                return nullptr;
            }
        }
        
        if (!check(TT::BRACE_RIGHT)) {
            errorBilingual(
                "خطأ: توقعت '}' في نهاية dict comprehension.",
                "Error: expected '}' at end of dict comprehension."
            );
            return nullptr;
        }
        consume(TT::BRACE_RIGHT, "");
        
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
    while (match(TT::COMMA) || match(TT::ARABIC_COMMA)) {
        if (check(TT::BRACE_RIGHT)) {
            break; // Trailing comma is allowed
        }
        
        auto key = parseExpression();
        if (!key) {
            errorBilingual(
                "خطأ: فشل تحليل مفتاح إضافي في الخريطة.",
                "Error: failed to parse additional key in map."
            );
            return nullptr;
        }
        
        if (!check(TT::COLON)) {
            errorBilingual(
                "خطأ: توقعت ':' بعد مفتاح الخريطة. هل نسيت الفاصلة قبل المفتاح التالي؟",
                "Error: expected ':' after map key. Did you forget the colon before the value?"
            );
            return nullptr;
        }
        consume(TT::COLON, "");
        
        auto value = parseExpression();
        if (!value) {
            errorBilingual(
                "خطأ: فشل تحليل قيمة إضافية في الخريطة.",
                "Error: failed to parse additional value in map."
            );
            return nullptr;
        }
        
        pairs.emplace_back(std::move(key), std::move(value));
    }
    
    if (!check(TT::BRACE_RIGHT)) {
        errorBilingual(
            "خطأ: لم يتم إغلاق الخريطة. توقعت '}'. هل نسيت إغلاق الأقواس المعقوفة؟",
            "Error: map not closed. Expected '}'. Did you forget to close the braces?"
        );
        return nullptr;
    }
    consume(TT::BRACE_RIGHT, "");
    
    // Create map expression node
    // (AR) إنشاء عقدة تعبير الخريطة
    return std::make_unique<MapExpr>(
        std::move(pairs),
        previous().getPosition()
    );
}

} // namespace Parser
} // namespace Sad