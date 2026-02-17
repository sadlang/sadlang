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
    
    // Fetch first token into current_, skipping whitespace/comments/doc-comments
    current_ = lexer_.nextToken();
    while (current_.getType() == TT::WHITESPACE || 
           current_.getType() == TT::COMMENT ||
           current_.getType() == TT::DOC_COMMENT ||
           current_.getType() == TT::NEWLINE) {
        current_ = lexer_.nextToken();
    }
    
    // Fetch second token into nextToken_, also skipping whitespace/comments/doc-comments
    nextToken_ = lexer_.nextToken();
    while (nextToken_.getType() == TT::WHITESPACE || 
           nextToken_.getType() == TT::COMMENT ||
           nextToken_.getType() == TT::DOC_COMMENT ||
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
    
    // (AR) عداد للدوال الرئيسية - للتأكد من وجود واحدة فقط
    // (EN) Counter for main functions - to ensure only one exists
    int mainFunctionCount = 0;
    
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
                // (AR) التحقق إذا كانت الجملة دالة رئيسية
                // (EN) Check if statement is a main function
                if (auto* funcDecl = dynamic_cast<FunctionDecl*>(stmt.get())) {
                    if (funcDecl->isMainFunction) {
                        mainFunctionCount++;
                        // (AR) التحقق من عدم تكرار الدالة الرئيسية
                        // (EN) Check for duplicate main function
                        if (mainFunctionCount > 1) {
                            std::cerr << "\n";
                            std::cerr << "❌ ========================================\n";
                            std::cerr << "  ⛔ (AR) خطأ: تم تعريف الدالة الرئيسية أكثر من مرة!\n";
                            std::cerr << "  ⛔ (EN) Error: Main function defined more than once!\n";
                            std::cerr << "========================================\n";
                            std::cerr << "📍 (AR) السطر: " << current_.getPosition().line << "\n";
                            std::cerr << "📍 (EN) Line: " << current_.getPosition().line << "\n";
                            std::cerr << "💬 (AR) يجب أن يكون هناك دالة رئيسية واحدة فقط في البرنامج\n";
                            std::cerr << "💬 (EN) There can only be one main function in a program\n";
                            std::cerr << "========================================\n\n";
                            
                            error("(AR) تم تعريف الدالة الرئيسية أكثر من مرة. "
                                  "(EN) Main function defined more than once.");
                        }
                    }
                }
                
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
    
    // (AR) دالة غير متزامنة / (EN) Async function
    if (match(TT::KEYWORD_ASYNC)) {
        if (!check(TT::KEYWORD_FUNCTION)) {
            error("(AR) خطأ نحوي: يجب أن تتبع 'غير_متزامن' بـ 'دالة'. (EN) Syntax error: 'async' must be followed by 'function'.");
            return nullptr;
        }
        advance(); // (AR) استهلاك 'دالة' / (EN) consume 'function'
        return parseFunctionDecl(std::move(decorators), true, false); // (AR) تمرير is_async=true / (EN) pass is_async=true
    }
    
    if (match(TT::KEYWORD_FUNCTION)) {
        // (AR) التحقق إذا كانت دالة مولد: دالة مولد اسم()
        // (EN) Check if generator function: function generator name()
        bool isGenerator = match(TT::KEYWORD_GENERATOR);
        return parseFunctionDecl(std::move(decorators), false, isGenerator);
    }
    
    // ======================================================================
    // (AR) دعم القوالب (Templates - Phase 7B)
    // (EN) Template support (Phase 7B)
    // ======================================================================
    if (match(TT::KEYWORD_TEMPLATE)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات للقوالب غير مدعومة بعد. (EN) Template decorators not yet supported.");
        }
        return parseTemplateDecl();
    }
    
    // (AR) دعم فضاء الأسماء (Namespaces - Phase 7B.5)
    // (EN) Namespace support (Phase 7B.5)
    if (match(TT::KEYWORD_NAMESPACE)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لفضاء الأسماء غير مدعومة. (EN) Namespace decorators not supported.");
        }
        return parseNamespaceDecl();
    }
    
    if (match(TT::KEYWORD_CLASS)) {
        // TODO: Add decorator support for classes in future
        // (AR) ملاحظة: دعم المُزخرِفات للأصناف سيُضاف مستقبلاً
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات للأصناف غير مدعومة بعد. (EN) Class decorators not yet supported.");
        }
        return parseClassDecl();
    }
    
    // (AR) دعم المتغيرات مع كلمة متغير / (EN) Support variables with var keyword
    if (match(TT::KEYWORD_VAR)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مع المتغيرات. (EN) Decorators cannot be used with variables.");
        }
        return parseVarDecl();
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
        } else if (nextTok.getType() == TT::PAREN_LEFT) {
            // Type keyword used as function call: نص(...) → treat as expression statement
            // (AR) كلمة نوع مستخدمة كاستدعاء دالة: نص(...) → تعامل كجملة تعبير
            return parseStatement();
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
    
    if (match(TT::KEYWORD_MATCH)) {
        return parseMatchStmt();
    }
    
    if (match(TT::KEYWORD_RETURN)) {
        return parseReturnStmt();
    }
    
    if (match(TT::KEYWORD_YIELD)) {
        return parseYieldStmt();
    }
    
    if (match(TT::KEYWORD_WITH)) {
        return parseWithStmt();
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
        ExprPtr firstKey = parseExpression();
        
        if (!firstKey) {
            std::cout << "Failed to parse key, treating as block\n";
            auto block = parseBlockStmt();
            return block;
        }
        
        // Check if followed by colon (map syntax)
        if (check(TT::COLON)) {
            consume(TT::COLON, "Expected :");
            
            // Parse first value
            ExprPtr firstValue = parseExpression();
            
            if (!firstValue) {
                errorExpectedToken("تعبير قيمة", "value expression", "بعد ':' في list comprehension", "after ':' in list comprehension");
                return nullptr;
            }
            
            // Check for dict comprehension
            if (check(TT::KEYWORD_FOR)) {
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
StmtPtr ParserCore::parseFunctionDecl(ExprList decorators, bool is_async, bool is_generator) {
    // Spec: docs/language_spec/rules/02_functions.md - function_decl ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
    // Optional return type BEFORE function name: دالة [type] name(...)
    // (AR) نوع الإرجاع الاختياري قبل اسم الدالة: دالة [نوع] اسم(...)
    Data::DataType returnType = Data::DataType::UNKNOWN;
    
    // (AR) راية للإشارة إذا كانت هذه هي الدالة الرئيسية
    // (EN) Flag to indicate if this is the main function
    bool isMain = false;
    Token name(TT::IDENTIFIER, "", Lexer::Position());  // (AR) تعريف name مسبقاً / (EN) Define name upfront
    
    // Check if next token is a type keyword (before function name)
    // (AR) التحقق إذا كان الرمز التالي هو نوع (قبل اسم الدالة)
    // BUT only if it's NOT followed by '(' — otherwise the type keyword IS the function name
    // e.g., "دالة نص مربع(...)" = return type string, name مربع
    //       "دالة نص(...)"       = function named نص (no return type)
    if ((check(TT::TYPE_INTEGER) || check(TT::TYPE_DOUBLE) || 
         check(TT::TYPE_STRING) || check(TT::TYPE_BOOLEAN) ||
         check(TT::TYPE_ARRAY) || check(TT::TYPE_MAP)) &&
        nextToken_.getType() != TT::PAREN_LEFT) {
        returnType = parseType();
    }
    
    // (AR) التحقق إذا كانت الدالة الرئيسية (قبل توقع اسم الدالة)
    // (EN) Check if this is the main function (before expecting function name)
    if (check(TT::KEYWORD_MAIN)) {
        // (AR) هذه هي الدالة الرئيسية - استخدام رمز KEYWORD_MAIN
        // (EN) This is the main function - consume KEYWORD_MAIN token
        Token mainToken = current_;  // (AR) حفظ الرمز قبل advance / (EN) Save token before advance
        advance();  // (AR) استهلاك الرمز / (EN) Consume token
        isMain = true;
        
        // (AR) إذا لم يتم تحديد نوع الإرجاع، استخدام صحيح (int) كإعداد افتراضي للدالة الرئيسية
        // (EN) If return type not specified, use INTEGER as default for main function
        if (returnType == Data::DataType::UNKNOWN) {
            returnType = Data::DataType::INTEGER;
        }
        
        // (AR) استخدام "رئيسية" كاسم للدالة
        // (EN) Use "main" as the function name
        name = Token(TT::IDENTIFIER, "رئيسية", mainToken.getPosition());
    } else if (isTypeToken(current_.getType())) {
        // Type keyword used as function name (e.g., دالة نص(props))
        // (AR) كلمة نوع مستخدمة كاسم دالة (مثلاً: دالة نص(props))
        auto tok = current_;
        advance();
        name = Token(TT::IDENTIFIER, tok.getValue(), tok.getPosition());
    } else {
        // Expect function name (for regular functions)
        // (AR) توقع اسم الدالة (للدوال العادية)
        name = consume(TT::IDENTIFIER, 
            "(AR) خطأ نحوي: بعد كلمة 'دالة' (أو بعد نوع الإرجاع) يجب أن يأتي اسم الدالة.\n"
            "مثال: دالة جمع(...) أو دالة رقم مربع(...)\n"
            "(EN) Syntax error: After 'function' keyword (or return type) expected function name.\n"
            "Example: function sum(...) or function int square(...)");
    }
    
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
    // OR with "ترجع" keyword: function name(...) ترجع type
    // (AR) تصريح نوع الإرجاع الاختياري بعد المعاملات: دالة اسم(...) : نوع
    // (AR) أو مع كلمة "ترجع": دالة اسم(...) ترجع نوع
    // Note: This overrides the type specified before the function name (if any)
    if (match(TT::COLON)) {
        returnType = parseType();
    } else if (match(TT::KEYWORD_RETURNS) || match(TT::ARROW)) {
        // Support "ترجع" keyword or "->" for return type
        if (isTypeToken(current_.getType())) {
            returnType = mapTokenTypeToDataType(current_.getType());
            advance();
        } else if (check(TT::IDENTIFIER)) {
            // Could be a custom type or class name
            returnType = Data::DataType::OBJECT;
            advance();
        }
    } else if (returnType == Data::DataType::UNKNOWN && isTypeToken(current_.getType())) {
        // Type without "ترجع" keyword (for backward compatibility)
        returnType = mapTokenTypeToDataType(current_.getType());
        advance();
    }
    
    // (AR) التحقق من صحة توقيع الدالة الرئيسية
    // (EN) Validate main function signature
    if (isMain) {
        // (AR) التحقق من نوع الإرجاع - يجب أن يكون صحيح (int)
        // (EN) Check return type - must be INTEGER
        if (returnType != Data::DataType::INTEGER && returnType != Data::DataType::UNKNOWN) {
            std::cerr << "\n";
            std::cerr << "⚠️ ========================================\n";
            std::cerr << "  ⛔ (AR) تحذير: نوع إرجاع الدالة الرئيسية يجب أن يكون 'صحيح'\n";
            std::cerr << "  ⛔ (EN) Warning: Main function return type must be 'int'\n";
            std::cerr << "========================================\n";
            std::cerr << "📍 (AR) السطر: " << name.getPosition().line << "\n";
            std::cerr << "📍 (EN) Line: " << name.getPosition().line << "\n";
            std::cerr << "💬 (AR) نوع الإرجاع الحالي غير صحيح\n";
            std::cerr << "💬 (EN) Current return type is invalid\n";
            std::cerr << "✓ (AR) التوقيع الصحيح: دالة صحيح رئيسية() أو دالة صحيح رئيسية(نص[] الوسائط)\n";
            std::cerr << "✓ (EN) Correct signature: function int main() or function int main(string[] args)\n";
            std::cerr << "========================================\n\n";
        }
        
        // (AR) التحقق من المعاملات - يجب أن تكون () أو (نص[] الوسائط)
        // (EN) Check parameters - must be () or (string[] args)
        if (paramObjs.size() > 1) {
            std::cerr << "\n";
            std::cerr << "⚠️ ========================================\n";
            std::cerr << "  ⛔ (AR) خطأ: الدالة الرئيسية تقبل معامل واحد فقط أو لا شيء\n";
            std::cerr << "  ⛔ (EN) Error: Main function accepts zero or one parameter only\n";
            std::cerr << "========================================\n";
            std::cerr << "📍 (AR) السطر: " << name.getPosition().line << "\n";
            std::cerr << "📍 (EN) Line: " << name.getPosition().line << "\n";
            std::cerr << "💬 (AR) عدد المعاملات الحالي: " << paramObjs.size() << "\n";
            std::cerr << "💬 (EN) Current parameter count: " << paramObjs.size() << "\n";
            std::cerr << "✓ (AR) التوقيع الصحيح: دالة صحيح رئيسية() أو دالة صحيح رئيسية(نص[] الوسائط)\n";
            std::cerr << "✓ (EN) Correct signature: function int main() or function int main(string[] args)\n";
            std::cerr << "========================================\n\n";
            
            error("(AR) الدالة الرئيسية تقبل معامل واحد فقط (نص[] الوسائط) أو لا شيء. "
                  "(EN) Main function accepts zero or one parameter (string[] args) only.");
        } else if (paramObjs.size() == 1) {
            // (AR) التحقق من أن المعامل من نوع نص[] (string array)
            // (EN) Check that parameter is of type string[] (string array)
            if (paramObjs[0].type != Data::DataType::ARRAY) {
                std::cerr << "\n";
                std::cerr << "⚠️ ========================================\n";
                std::cerr << "  ⛔ (AR) تحذير: معامل الدالة الرئيسية يجب أن يكون من نوع نص[]\n";
                std::cerr << "  ⛔ (EN) Warning: Main function parameter must be of type string[]\n";
                std::cerr << "========================================\n";
                std::cerr << "📍 (AR) السطر: " << name.getPosition().line << "\n";
                std::cerr << "📍 (EN) Line: " << name.getPosition().line << "\n";
                std::cerr << "💬 (AR) نوع المعامل الحالي غير صحيح\n";
                std::cerr << "💬 (EN) Current parameter type is invalid\n";
                std::cerr << "✓ (AR) التوقيع الصحيح: دالة صحيح رئيسية(نص[] الوسائط)\n";
                std::cerr << "✓ (EN) Correct signature: function int main(string[] args)\n";
                std::cerr << "========================================\n\n";
            }
        }
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
        auto funcDecl = std::make_unique<FunctionDecl>(
            name.getValue(),
            std::move(paramObjs),
            returnType,
            std::move(body),
            std::move(decorators),
            false,  // (AR) مُصدَّرة / (EN) exported
            is_async,  // (AR) غير متزامنة / (EN) async
            is_generator,  // (AR) مولد / (EN) generator
            name.getPosition()
        );
        // (AR) تعيين راية الدالة الرئيسية
        // (EN) Set main function flag
        funcDecl->isMainFunction = isMain;
        return funcDecl;
    }
    
    // (AR) إنشاء عقدة تصريح الدالة بدون مُزخرِفات
    // (EN) Create function declaration node without decorators
    auto funcDecl = std::make_unique<FunctionDecl>(
        name.getValue(),
        std::move(paramObjs),
        returnType,
        std::move(body),
        false,  // (AR) مُصدَّرة / (EN) exported
        is_async,  // (AR) غير متزامنة / (EN) async
        is_generator,  // (AR) مولد / (EN) generator
        name.getPosition()
    );
    // (AR) تعيين راية الدالة الرئيسية
    // (EN) Set main function flag
    funcDecl->isMainFunction = isMain;
    return funcDecl;
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
#ifdef DEBUG_OOP
    std::cout << "[OOP] بدء تحليل تصريح صنف\n";
#endif
    
    // (AR) اسم الصنف / (EN) Class name
    // Spec: docs\language_spec\rules\03_oop.md §1 - class_decl ::= 'صنف' IDENTIFIER ...
    Token nameToken = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم الصنف بعد 'صنف'. (EN) Expected class name after 'class'.");
    std::string className = nameToken.getValue();
    
#ifdef DEBUG_OOP
    std::cout << "[OOP] اسم الصنف: " << className << "\n";
#endif
    
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
#ifdef DEBUG_OOP
            std::cout << "[OOP] يرث من: " << baseToken.getValue() << "\n";
#endif
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
        
        // Check for operator overload (keyword 'عامل')
        // (AR) التحقق من تحميل العامل الزائد / (EN) Check for operator overload
        if (check(TT::KEYWORD_OPERATOR)) {
            advance(); // consume 'عامل'
            auto operatorDecl = parseOperatorDecl();
            if (operatorDecl) {
                // (AR) تعيين معدّل الوصول / (EN) Set access modifier
                if (auto* opDecl = dynamic_cast<AST::OperatorDecl*>(operatorDecl.get())) {
                    opDecl->access = access;
                }
                members.push_back(std::move(operatorDecl));
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
    
#ifdef DEBUG_OOP
    std::cout << "[OOP] انتهى تحليل صنف '" << className << "' - "
              << members.size() << " أعضاء\n";
#endif
    
    // (AR) تسجيل الصنف مبكراً في ClassManager للسماح بمتغيرات من هذا النوع لاحقاً في نفس الملف
    // (EN) Register class early in ClassManager to allow variables of this type later in same file
    // هذا تسجيل مؤقت - سيتم تحديثه عند التنفيذ بكامل التفاصيل
    // This is temporary registration - will be updated during execution with full details
    auto* classManager = Data::ClassManager::getInstance();
    if (!classManager->hasClass(className)) {
        auto tempClassType = std::make_unique<Data::ClassType>(className);
        classManager->registerClass(std::move(tempClassType));
#ifdef DEBUG_OOP
        std::cout << "[OOP] ✅ تسجيل مؤقت للصنف: " << className << " (أثناء التحليل)\n";
#endif
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
    if (check(TT::SEMICOLON) || check(TT::ARABIC_SEMICOLON)) {
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
    if (!check(TT::SEMICOLON) && !check(TT::ARABIC_SEMICOLON)) {
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
    return parsePipeline();
}

/**
 * @brief (AR) يحلل عامل الأنبوب |> مع إزالة السكر النحوي.
 *        (EN) Parses pipeline operator |> with desugaring.
 *
 * Desugaring rules:
 *   a |> f       →  f(a)
 *   a |> f(b, c) →  f(a, b, c)
 *   a |> f |> g  →  g(f(a))
 */
ExprPtr ParserCore::parsePipeline() {
    auto expr = parseAssignment();
    
    while (match(TT::OP_PIPE_ARROW)) {
        auto pos = previous().getPosition();
        auto right = parseAssignment();
        
        if (!right) {
            errorBilingual(
                "خطأ: توقعت تعبير بعد '|>'.",
                "Error: expected expression after '|>'."
            );
            return nullptr;
        }
        
        // Desugar: if right is a CallExpr, prepend expr as first argument
        // (AR) إزالة السكر: إذا كان الجانب الأيمن استدعاء دالة، أدخل التعبير كأول معامل
        if (auto* call = dynamic_cast<CallExpr*>(right.get())) {
            // a |> f(b, c) → f(a, b, c)
            ExprList newArgs;
            newArgs.push_back(std::move(expr));
            for (auto& arg : call->arguments) {
                newArgs.push_back(std::move(arg));
            }
            expr = std::make_unique<CallExpr>(
                std::move(call->callee),
                std::move(newArgs),
                pos
            );
        } else {
            // a |> f → f(a)
            ExprList args;
            args.push_back(std::move(expr));
            expr = std::make_unique<CallExpr>(
                std::move(right),
                std::move(args),
                pos
            );
        }
    }
    
    return expr;
}

/**
 * @brief (AR) يحلل تعيين: اسم = قيمة، أو Walrus: اسم := قيمة.
 *        (EN) Parses assignment: name = value, or Walrus: name := value.
 */
ExprPtr ParserCore::parseAssignment() {
    auto expr = parseTernary();
    
    // (AR) فحص عامل Walrus := (إسناد داخل تعبير) / (EN) Check for Walrus operator := (assignment expression)
    if (match(TT::OP_WALRUS)) {
        Token walrus = previous();
        auto value = parseAssignment();
        
        // (AR) Walrus يتطلب متغيراً على اليسار / (EN) Walrus requires variable on left
        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<WalrusExpr>(
                var->name,
                std::move(value),
                walrus.getPosition()
            );
        }
        
        errorBilingual(
            "خطأ: عامل Walrus (:=) يتطلب اسم متغير على اليسار",
            "Error: Walrus operator (:=) requires variable name on left"
        );
    }
    
    // Check for assignment operator
    // (AR) التحقق من عامل التعيين العادي
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
        
        // دعم الإسناد بالفهرس: م[0] = قيمة ، قاموس["مفتاح"] = قيمة
        // (EN) Support index assignment: arr[0] = value, map["key"] = value
        if (auto* indexExpr = dynamic_cast<IndexExpr*>(expr.get())) {
            std::unique_ptr<IndexExpr> indexPtr(static_cast<IndexExpr*>(expr.release()));
            
            return std::make_unique<IndexAssignExpr>(
                std::move(indexPtr->object),
                std::move(indexPtr->index),
                std::move(value),
                equals.getPosition()
            );
        }
        
        errorBilingual(
            "خطأ: هدف الإسناد غير صالح - يجب أن يكون معرّفاً أو حقل كائن أو فهرس مصفوفة",
            "Error: invalid assignment target - must be identifier, object field, or array index"
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
    // (AR) تحليل عامل الاستعارة & / (EN) Parse borrow operator &
    if (match(TT::AMPERSAND)) {
        auto pos = previous().getPosition();
        // (AR) التحقق من &متغير / &mut (استعارة قابلة للتعديل)
        // (EN) Check for &متغير / &mut (mutable borrow)
        bool isMut = false;
        if (check(TT::KEYWORD_VAR) || 
            (check(TT::IDENTIFIER) && peek().getValue() == "mut")) {
            isMut = true;
            advance();
        }
        // (AR) اسم المتغير المُستعار / (EN) Variable name to borrow
        if (!check(TT::IDENTIFIER)) {
            error("\xD9\x85\xD8\xAA\xD9\x88\xD9\x82\xD8\xB9 \xD8\xA7\xD8\xB3\xD9\x85 \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xA8\xD8\xB9\xD8\xAF & / Expected variable name after &");
            return std::make_unique<LiteralExpr>(Token(TT::LITERAL_NULL, "null", pos));
        }
        std::string varName = peek().getValue();
        advance();
        return std::make_unique<BorrowExpr>(varName, isMut, pos);
    }
    
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
    // Await expression: await expr
    // (AR) تعبير الانتظار: انتظر تعبير
    if (match(TT::KEYWORD_AWAIT)) {
        Token awaitToken = previous();
        auto expr = parseTernary();  // (AR) تحليل تعبير ذو أولوية أعلى / (EN) Parse higher precedence expression
        if (!expr) {
            errorBilingual(
                "خطأ نحوي: يجب أن يتبع 'انتظر' بتعبير.",
                "Syntax error: 'await' must be followed by an expression."
            );
            return nullptr;
        }
        return std::make_unique<AwaitExpr>(std::move(expr), awaitToken.getPosition());
    }
    
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
    
    // STRING_LITERAL: Regular strings / النصوص العادية
    // (AR) معالجة النص الحرفي العادي
    if (match(TT::STRING_LITERAL)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    // STRING_RAW: Raw strings r"..." treat as regular string
    // (AR) النصوص الخام r"..." تُعامل كنص عادي (لا معالجة للـ escape)
    // Lexer already processed escapes (or didn't), just use as-is
    // المحلل المعجمي بالفعل عالج (أو لم يعالج) الـ escapes، استخدمها كما هي
    if (match(TT::STRING_RAW)) {
        auto rawToken = previous();
        // Convert STRING_RAW token to STRING_LITERAL for interpreter
        // تحويل رمز STRING_RAW إلى STRING_LITERAL للمفسر
        Lexer::Token stringToken(TT::STRING_LITERAL, rawToken.getValue(), rawToken.getPosition());
        return std::make_unique<LiteralExpr>(stringToken);
    }
    
    // STRING_FSTRING: F-strings f"{expr}" - convert to string concatenation
    // (AR) النصوص المنسقة f"{expr}" - تحويل إلى تسلسل نصوص
    // This is a simplified implementation - full implementation would parse expressions
    // هذا تنفيذ مبسط - التنفيذ الكامل يحلل التعبيرات داخل الأقواس
    if (match(TT::STRING_FSTRING)) {
        auto fstringToken = previous();
        std::string fstring = fstringToken.getValue();
        
        // For now, treat f-string as regular string (simplified)
        // حالياً، نعامل f-string كنص عادي (مبسط)
        // TODO Phase 2: Implement full f-string expression parsing
        // TODO المرحلة 2: تنفيذ تحليل كامل لتعبيرات f-string
        
        // Check if f-string contains expressions {...}
        // تحقق إذا كان f-string يحتوي على تعبيرات {...}
        if (fstring.find('{') != std::string::npos) {
            // Parse and expand f-string expressions
            // تحليل وتوسيع تعبيرات f-string
            return parseFStringExpr(fstring, fstringToken.getPosition());
        } else {
            // No expressions, treat as regular string
            // لا يوجد تعبيرات، عامله كنص عادي
            Lexer::Token stringToken(TT::STRING_LITERAL, fstring, fstringToken.getPosition());
            return std::make_unique<LiteralExpr>(stringToken);
        }
    }
    
    // Variable reference or Template Instantiation
    // (AR) مرجع متغير أو تنفيذ قالب
    if (match(TT::IDENTIFIER)) {
        std::string identName = previous().getValue();
        auto identPos = previous().getPosition();
        
        // (AR) تحقق إذا كان تنفيذ قالب: اسم<نوع>
        // (EN) Check if template instantiation: name<type>
        // (AR) نتحقق أن الرمز بعد < هو نوع مدمج فقط (رقم، نص، إلخ)
        //      لتجنب التعارض مع عامل المقارنة <
        // (EN) Only attempt template parsing if the token after < is a
        //      built-in type keyword, to avoid conflict with < operator
        if (check(TT::OP_LESS) && isTypeToken(nextToken_.getType())) {
            // (AR) حاول تحليل كتنفيذ قالب
            // (EN) Try to parse as template instantiation
            auto templateExpr = parseTemplateInstantiation(identName, identPos);
            if (templateExpr) {
                return templateExpr;
            }
            // (AR) إذا فشل، اعتبره متغير عادي
            // (EN) If failed, treat as regular variable
        }
        
        return std::make_unique<VariableExpr>(
            identName,
            identPos
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
    
    // Type keywords used as expressions (e.g., نص(...) as function call)
    // (AR) كلمات أنواع مستخدمة كتعبيرات (مثلاً نص(...) كاستدعاء دالة)
    // This allows type names to be used as function names in expression position
    if (isTypeToken(current_.getType())) {
        auto tok = current_;
        advance();
        return std::make_unique<VariableExpr>(tok.getValue(), tok.getPosition());
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
 * @brief (AR) يحلل f-string ويحوله إلى تسلسل نصوص.
 *        (EN) Parses f-string and converts to string concatenation.
 * 
 * Converts f"Hello {name}!" to: "Hello " + str(name) + "!"
 * يحول f"مرحبا {الاسم}!" إلى: "مرحبا " + str(الاسم) + "!"
 */
ExprPtr ParserCore::parseFStringExpr(const std::string& fstring, const Lexer::Position& pos) {
    // Build concatenation of string parts and expressions
    // بناء تسلسل من أجزاء النصوص والتعبيرات
    
    ExprPtr result = nullptr;
    size_t i = 0;
    std::string currentText;
    
    while (i < fstring.length()) {
        if (fstring[i] == '{' && (i + 1 < fstring.length() && fstring[i + 1] != '{')) {
            // Found expression start
            // وجدنا بداية تعبير
            
            // Add accumulated text as string literal
            // أضف النص المتراكم كنص حرفي
            if (!currentText.empty()) {
                auto textLiteral = std::make_unique<LiteralExpr>(
                    Lexer::Token(TT::STRING_LITERAL, currentText, pos)
                );
                if (result) {
                    // Concatenate with previous parts
                    // تسلسل مع الأجزاء السابقة
                    result = std::make_unique<BinaryExpr>(
                        std::move(result),
                        TT::OP_PLUS,
                        std::move(textLiteral),
                        pos
                    );
                } else {
                    result = std::move(textLiteral);
                }
                currentText.clear();
            }
            
            // Find closing brace
            // ابحث عن القوس الإغلاق
            size_t exprStart = i + 1;
            size_t braceDepth = 1;
            i++;
            while (i < fstring.length() && braceDepth > 0) {
                if (fstring[i] == '{') braceDepth++;
                else if (fstring[i] == '}') braceDepth--;
                if (braceDepth > 0) i++;
            }
            
            if (braceDepth != 0) {
                errorBilingual(
                    "خطأ: قوس غير مُغلق في f-string. تأكد من إغلاق جميع الأقواس {...}",
                    "Error: unclosed brace in f-string. Make sure all braces {...} are closed."
                );
                return nullptr;
            }
            
            // Extract expression text
            // استخراج نص التعبير
            std::string exprText = fstring.substr(exprStart, i - exprStart);
            
            // For now, treat expression as variable name (simplified)
            // حالياً، نعامل التعبير كاسم متغير (مبسط)
            // TODO Phase 2: Parse full expression from text
            // TODO المرحلة 2: تحليل التعبير الكامل من النص
            
            auto exprNode = std::make_unique<VariableExpr>(exprText, pos);
            
            // Wrap in str() call to convert to string
            // لف في استدعاء str() للتحويل إلى نص
            std::vector<ExprPtr> strArgs;
            strArgs.push_back(std::move(exprNode));
            auto strCall = std::make_unique<CallExpr>(
                std::make_unique<VariableExpr>("str", pos),
                std::move(strArgs)
            );
            
            if (result) {
                // Concatenate with previous parts
                // تسلسل مع الأجزاء السابقة
                result = std::make_unique<BinaryExpr>(
                    std::move(result),
                    TT::OP_PLUS,
                    std::move(strCall),
                    pos
                );
            } else {
                result = std::move(strCall);
            }
            
            i++; // Move past closing brace
        }
        else if (fstring[i] == '{' && i + 1 < fstring.length() && fstring[i + 1] == '{') {
            // Escaped brace {{
            // قوس مُتجاوز {{
            currentText += '{';
            i += 2;
        }
        else if (fstring[i] == '}' && i + 1 < fstring.length() && fstring[i + 1] == '}') {
            // Escaped brace }}
            // قوس مُتجاوز }}
            currentText += '}';
            i += 2;
        }
        else {
            // Regular character
            // حرف عادي
            currentText += fstring[i];
            i++;
        }
    }
    
    // Add any remaining text
    // أضف أي نص متبقي
    if (!currentText.empty()) {
        auto textLiteral = std::make_unique<LiteralExpr>(
            Lexer::Token(TT::STRING_LITERAL, currentText, pos)
        );
        if (result) {
            result = std::make_unique<BinaryExpr>(
                std::move(result),
                TT::OP_PLUS,
                std::move(textLiteral),
                pos
            );
        } else {
            result = std::move(textLiteral);
        }
    }
    
    return result ? std::move(result) : std::make_unique<LiteralExpr>(
        Lexer::Token(TT::STRING_LITERAL, "", pos)
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
    // Check for empty map/set
    // (AR) التحقق من خريطة/مجموعة فارغة
    if (check(TT::BRACE_RIGHT)) {
        consume(TT::BRACE_RIGHT, "");
        // Empty {} defaults to empty map (like Python)
        // (AR) {} فارغة تُعتبر خريطة فارغة (مثل Python)
        return std::make_unique<MapExpr>(std::vector<MapPair>{}, previous().getPosition());
    }
    
    // Parse first expression using parseTernary to avoid consuming 'for' keyword
    // (AR) تحليل التعبير الأول باستخدام parseTernary لتجنب استهلاك 'for'
    auto firstKey = parseTernary();
    if (!firstKey) {
        errorBilingual(
            "خطأ: فشل تحليل التعبير - تعبير غير صحيح.",
            "Error: failed to parse expression - invalid expression."
        );
        return nullptr;
    }
    
    // Check if this is a set comprehension: {expr for var in iterable}
    // (AR) التحقق إذا كان set comprehension
    if (check(TT::KEYWORD_FOR)) {
        // This is a set comprehension: {expr for var in iterable if cond}
        // (AR) هذا set comprehension
        
        advance(); // consume 'for'
        
        // Parse loop variable
        // (AR) تحليل متغير الحلقة
        if (!check(TT::IDENTIFIER)) {
            errorBilingual(
                "خطأ: توقعت اسم متغير حلقة بعد 'for' في set comprehension. مثال: {x for x in list}",
                "Error: expected loop variable name after 'for' in set comprehension. Example: {x for x in list}"
            );
            return nullptr;
        }
        Token loopVar = peek();
        advance();
        
        // Expect 'in' keyword
        // (AR) توقع كلمة 'في'
        if (!check(TT::KEYWORD_IN)) {
            errorBilingual(
                "خطأ: توقعت 'في' بعد متغير الحلقة. الصيغة: {expr for var in iterable}",
                "Error: expected 'in' after loop variable. Format: {expr for var in iterable}"
            );
            return nullptr;
        }
        advance();
        
        // Parse iterable
        // (AR) تحليل المجموعة القابلة للتكرار
        auto iterable = parseExpression();
        if (!iterable) {
            errorBilingual(
                "خطأ: فشل تحليل المجموعة القابلة للتكرار في set comprehension.",
                "Error: failed to parse iterable in set comprehension."
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
                    "خطأ: تعبير شرط غير صحيح بعد 'إذا' في set comprehension.",
                    "Error: invalid condition expression after 'if' in set comprehension."
                );
                return nullptr;
            }
        }
        
        if (!check(TT::BRACE_RIGHT)) {
            errorBilingual(
                "خطأ: توقعت '}' في نهاية set comprehension.",
                "Error: expected '}' at end of set comprehension."
            );
            return nullptr;
        }
        consume(TT::BRACE_RIGHT, "");
        
        // Create set comprehension node
        // (AR) إنشاء عقدة Set Comprehension
        return std::make_unique<SetComprehensionExpr>(
            std::move(firstKey),
            loopVar.getValue(),
            std::move(iterable),
            std::move(condition)
        );
    }
    
    // Check if this is a dict (has colon) or set (no colon)
    // (AR) التحقق إذا كان dict (له :) أو set (بدون :)
    if (!check(TT::COLON)) {
        errorBilingual(
            "خطأ: توقعت ':' بعد مفتاح الخريطة. الصيغة: {مفتاح: قيمة، ...}. لـ Set Comprehension استخدم: {expr for x in list}",
            "Error: expected ':' after map key. Format: {key: value, ...}. For Set Comprehension use: {expr for x in list}"
        );
        return nullptr;
    }
    consume(TT::COLON, "");
    
    // Parse first value expression using parseTernary to avoid consuming 'for'
    // (AR) تحليل تعبير القيمة باستخدام parseTernary لتجنب استهلاك 'for'
    auto firstValue = parseTernary();
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

// ============================================================================
// (AR) تحليل Pattern Matching / (EN) Pattern Matching Parsing
// ============================================================================

/**
 * @brief (AR) يحلل جملة match لمطابقة الأنماط
 *        (EN) Parses match statement for pattern matching
 */
StmtPtr ParserCore::parseMatchStmt() {
    // Already consumed 'match'
    // (AR) تم استهلاك 'match' بالفعل
    
    // Parse value to match against
    // (AR) تحليل القيمة المُختبرة
    auto value = parseExpression();
    
    if (!value) {
        errorBilingual(
            "خطأ: توقعت تعبير بعد 'match'",
            "Error: Expected expression after 'match'"
        );
        return nullptr;
    }
    
    // (AR) اختيارياً: يمكن قبول '{' للتوافق القديم
    // (EN) Optionally accept '{' for backward compatibility
    bool useBraces = match(TT::BRACE_LEFT);
    
    // Parse case clauses
    // (AR) تحليل فروع case
    std::vector<AST::CaseClause> cases;
    
    auto isMatchEnd = [&]() -> bool {
        if (useBraces) return check(TT::BRACE_RIGHT);
        return check(TT::KEYWORD_END);
    };
    
    while (!isMatchEnd() && !isAtEnd()) {
        if (check(TT::KEYWORD_CASE)) {
            cases.push_back(parseCaseClause());
        } else {
            errorBilingual(
                "خطأ: توقعت 'حالة' أو 'نهاية'",
                "Error: Expected 'case' or 'end'"
            );
            return nullptr;
        }
    }
    
    if (cases.empty()) {
        errorBilingual(
            "خطأ: جملة match يجب أن تحتوي على فرع case واحد على الأقل",
            "Error: Match statement must have at least one case clause"
        );
        return nullptr;
    }
    
    // Expect closing: 'نهاية' or '}'
    // (AR) توقع إنهاء: 'نهاية' أو '}'
    if (useBraces) {
        if (!match(TT::BRACE_RIGHT)) {
            errorBilingual(
                "خطأ: توقعت '}' في نهاية جملة match",
                "Error: Expected '}' at end of match statement"
            );
            return nullptr;
        }
    } else {
        if (!match(TT::KEYWORD_END)) {
            errorBilingual(
                "خطأ: توقعت 'نهاية' في نهاية جملة match",
                "Error: Expected 'end' at end of match statement"
            );
            return nullptr;
        }
    }
    
    return std::make_unique<AST::MatchStmt>(
        std::move(value),
        std::move(cases),
        previous().getPosition()
    );
}

/**
 * @brief (AR) يحلل فرع case واحد
 *        (EN) Parses one case clause
 */
AST::CaseClause ParserCore::parseCaseClause() {
    // Consume 'case'
    // (AR) استهلاك 'case'
    if (!match(TT::KEYWORD_CASE)) {
        errorBilingual(
            "خطأ: توقعت 'case'",
            "Error: Expected 'case'"
        );
        return AST::CaseClause(nullptr, nullptr, {});
    }
    
    // Parse pattern
    // (AR) تحليل النمط
    auto pattern = parsePattern();
    
    if (!pattern) {
        errorBilingual(
            "خطأ: توقعت نمط بعد 'case'",
            "Error: Expected pattern after 'case'"
        );
        return AST::CaseClause(nullptr, nullptr, {});
    }
    
    // Parse optional guard (if condition)
    // (AR) تحليل guard اختياري (شرط if)
    ExprPtr guard = nullptr;
    if (match(TT::KEYWORD_IF)) {
        guard = parseExpression();
        
        if (!guard) {
            errorBilingual(
                "خطأ: توقعت تعبير بعد 'if' في guard",
                "Error: Expected expression after 'if' in guard"
            );
        }
    }
    
    // Expect colon
    // (AR) توقع نقطتين رأسيتين
    if (!match(TT::COLON)) {
        errorBilingual(
            "خطأ: توقعت ':' بعد نمط case",
            "Error: Expected ':' after case pattern"
        );
        return AST::CaseClause(nullptr, nullptr, {});
    }
    
    // Parse body - multiple statements until next 'case' or 'end' or '}'
    // (AR) تحليل الجسم - جمل متعددة حتى 'حالة' أو 'نهاية' أو '}' التالية
    std::vector<StmtPtr> body;
    
    while (!check(TT::KEYWORD_CASE) && !check(TT::KEYWORD_END) && 
           !check(TT::BRACE_RIGHT) && !isAtEnd()) {
        auto stmt = parseDeclaration();
        if (stmt) {
            body.push_back(std::move(stmt));
        } else {
            synchronize();
        }
    }
    
    return AST::CaseClause(
        std::move(pattern),
        std::move(guard),
        std::move(body)
    );
}

/**
 * @brief (AR) يحلل نمط
 *        (EN) Parses a pattern
 */
std::unique_ptr<AST::Pattern> ParserCore::parsePattern() {
    // Wildcard: _ (using IDENTIFIER)
    // (AR) النمط الشامل: _ (باستخدام IDENTIFIER)
    if (check(TT::IDENTIFIER) && current_.getValue() == "_") {
        advance();
        return std::make_unique<AST::WildcardPattern>();
    }
    
    // List pattern: [...]
    // (AR) نمط قائمة: [...]
    if (check(TT::BRACKET_LEFT)) {
        return parseListPattern();
    }
    
    // Parse primary pattern (literal or variable)
    // (AR) تحليل نمط أساسي (قيمة حرفية أو متغير)
    auto primary = parsePrimaryPattern();
    
    if (!primary) {
        return nullptr;
    }
    
    // Check for OR pattern: a | b | c (using OP_OR)
    // (AR) التحقق من نمط OR: a | b | c
    if (check(TT::OP_OR)) {
        std::vector<std::unique_ptr<AST::Pattern>> alternatives;
        alternatives.push_back(std::move(primary));
        
        while (match(TT::OP_OR)) {
            auto alt = parsePrimaryPattern();
            if (alt) {
                alternatives.push_back(std::move(alt));
            } else {
                errorBilingual(
                    "خطأ: توقعت نمط بعد '||'",
                    "Error: Expected pattern after '||'"
                );
                break;
            }
        }
        
        return std::make_unique<AST::OrPattern>(std::move(alternatives));
    }
    
    return primary;
}

/**
 * @brief (AR) يحلل نمط أساسي (literal, variable, wildcard)
 *        (EN) Parses primary pattern (literal, variable, wildcard)
 */
std::unique_ptr<AST::Pattern> ParserCore::parsePrimaryPattern() {
    // Number literal (INTEGER or DOUBLE)
    // (AR) قيمة رقمية حرفية
    if (check(TT::NUMBER_INTEGER)) {
        Token token = current_;
        advance();
        double value = std::stod(token.getValue());
        return std::make_unique<AST::LiteralPattern>(Data::Value(value));
    }
    
    if (check(TT::NUMBER_DOUBLE)) {
        Token token = current_;
        advance();
        double value = std::stod(token.getValue());
        return std::make_unique<AST::LiteralPattern>(Data::Value(value));
    }
    
    // String literal
    // (AR) قيمة نصية حرفية
    if (check(TT::STRING_LITERAL)) {
        Token token = current_;
        advance();
        return std::make_unique<AST::LiteralPattern>(Data::Value(token.getValue()));
    }
    
    // Boolean literal: true/false
    // (AR) قيمة منطقية حرفية
    if (check(TT::LITERAL_TRUE)) {
        advance();
        return std::make_unique<AST::LiteralPattern>(Data::Value(true));
    }
    
    if (check(TT::LITERAL_FALSE)) {
        advance();
        return std::make_unique<AST::LiteralPattern>(Data::Value(false));
    }
    
    // null
    // (AR) قيمة null
    if (check(TT::LITERAL_NULL)) {
        advance();
        return std::make_unique<AST::LiteralPattern>(Data::Value());
    }
    
    // Variable pattern (identifier)
    // (AR) نمط متغير (معرّف)
    if (check(TT::IDENTIFIER)) {
        Token token = current_;
        advance();
        return std::make_unique<AST::VariablePattern>(token.getValue());
    }
    
    errorBilingual(
        "خطأ: توقعت نمط (رقم، نص، متغير، أو '_')",
        "Error: Expected pattern (number, string, variable, or '_')"
    );
    return nullptr;
}

/**
 * @brief (AR) يحلل نمط قائمة [...]
 *        (EN) Parses list pattern [...]
 */
std::unique_ptr<AST::Pattern> ParserCore::parseListPattern() {
    if (!match(TT::BRACKET_LEFT)) {
        errorBilingual(
            "خطأ: توقعت '[' لبداية نمط القائمة",
            "Error: Expected '[' for list pattern"
        );
        return nullptr;
    }
    
    std::vector<std::unique_ptr<AST::Pattern>> elements;
    bool has_rest = false;
    std::string rest_name;
    
    // Empty list: []
    // (AR) قائمة فارغة: []
    if (check(TT::BRACKET_RIGHT)) {
        advance();
        return std::make_unique<AST::ListPattern>(std::move(elements), false, "");
    }
    
    // Parse elements
    // (AR) تحليل العناصر
    do {
        // Rest pattern: *rest (using OP_MULTIPLY)
        // (AR) نمط الباقي: *rest
        if (check(TT::OP_MULTIPLY)) {
            advance();
            
            if (!check(TT::IDENTIFIER)) {
                errorBilingual(
                    "خطأ: توقعت اسم متغير بعد '*' في نمط القائمة",
                    "Error: Expected variable name after '*' in list pattern"
                );
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
        if (pattern) {
            elements.push_back(std::move(pattern));
        } else {
            // Error in pattern
            // (AR) خطأ في النمط
            return nullptr;
        }
        
    } while (matchAny({TT::COMMA, TT::ARABIC_COMMA}));
    
    if (!match(TT::BRACKET_RIGHT)) {
        errorBilingual(
            "خطأ: توقعت ']' في نهاية نمط القائمة",
            "Error: Expected ']' at end of list pattern"
        );
        return nullptr;
    }
    
    return std::make_unique<AST::ListPattern>(
        std::move(elements),
        has_rest,
        rest_name
    );
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
std::vector<AST::TypeParameter> ParserCore::parseTemplateParameters() {
    std::vector<AST::TypeParameter> params;
    
    // (AR) توقع '<' / (EN) Expect '<'
    if (!match(TT::OP_LESS)) {
        errorBilingual(
            "خطأ نحوي: توقعت '<' بعد 'قالب'",
            "Syntax error: Expected '<' after 'template'"
        );
        return params;
    }
    
    // (AR) تحليل معاملات الأنواع
    // (EN) Parse type parameters
    do {
        // (AR) توقع 'نوع' أو 'typename'
        // (EN) Expect 'typename' keyword
        if (!match(TT::KEYWORD_TYPENAME)) {
            errorBilingual(
                "خطأ نحوي: توقعت 'نوع' في معامل القالب",
                "Syntax error: Expected 'typename' in template parameter"
            );
            break;
        }
        
        // (AR) توقع اسم المعامل (مثل T أو ت)
        // (EN) Expect parameter name (e.g., T)
        if (!check(TT::IDENTIFIER)) {
            errorBilingual(
                "خطأ نحوي: توقعت اسم معامل النوع",
                "Syntax error: Expected type parameter name"
            );
            break;
        }
        
        std::string paramName = current_.getValue();
        advance();
        
        // (AR) تحقق من وجود قيد (constraint)
        // (EN) Check for constraint
        std::string constraint;
        if (match(TT::COLON)) {
            if (!check(TT::IDENTIFIER)) {
                errorBilingual(
                    "خطأ نحوي: توقعت اسم القيد بعد ':'",
                    "Syntax error: Expected constraint name after ':'"
                );
            } else {
                constraint = current_.getValue();
                advance();
            }
        }
        
        params.emplace_back(paramName, constraint);
        
    } while (matchAny({TT::COMMA, TT::ARABIC_COMMA}));
    
    // (AR) توقع '>' / (EN) Expect '>'
    if (!match(TT::OP_GREATER)) {
        errorBilingual(
            "خطأ نحوي: توقعت '>' لإنهاء معاملات القالب",
            "Syntax error: Expected '>' to close template parameters"
        );
    }
    
    return params;
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
StmtPtr ParserCore::parseTemplateDecl() {
    auto startPos = previous_.getPosition();
    
    // (AR) تحليل معاملات الأنواع
    // (EN) Parse type parameters
    auto typeParams = parseTemplateParameters();
    
    if (typeParams.empty()) {
        errorBilingual(
            "خطأ نحوي: القالب يحتاج إلى معامل نوع واحد على الأقل",
            "Syntax error: Template requires at least one type parameter"
        );
        return nullptr;
    }
    
    // (AR) التحقق من نوع التصريح (دالة أو صنف)
    // (EN) Check declaration type (function or class)
    if (match(TT::KEYWORD_FUNCTION)) {
        // (AR) تحليل دالة قالب
        // (EN) Parse template function
        
        // (AR) توقع اسم الدالة
        // (EN) Expect function name
        if (!check(TT::IDENTIFIER)) {
            errorBilingual(
                "خطأ نحوي: توقعت اسم الدالة بعد 'دالة'",
                "Syntax error: Expected function name after 'function'"
            );
            return nullptr;
        }
        
        std::string funcName = current_.getValue();
        advance();
        
        // (AR) تحليل المعاملات
        // (EN) Parse parameters
        if (!match(TT::PAREN_LEFT)) {
            errorBilingual(
                "خطأ نحوي: توقعت '(' بعد اسم الدالة",
                "Syntax error: Expected '(' after function name"
            );
            return nullptr;
        }
        
        std::vector<AST::Parameter> params;
        if (!check(TT::PAREN_RIGHT)) {
            do {
                // (AR) تحليل معامل واحد: اسم: نوع
                // (EN) Parse single parameter: name: type
                if (!check(TT::IDENTIFIER) && !isTypeToken(current_.getType())) {
                    errorBilingual(
                        "خطأ نحوي: توقعت اسم أو نوع المعامل",
                        "Syntax error: Expected parameter name or type"
                    );
                    break;
                }
                
                std::string paramName;
                Data::DataType paramType = Data::DataType::NONE;
                std::string templateTypeName;  // (AR) لحفظ اسم نوع القالب / (EN) To store template type name
                
                // (AR) معامل يبدأ بالنوع أو الاسم
                // (EN) Parameter starts with type or name
                if (isTypeToken(current_.getType())) {
                    // (AR) نوع مدمج مثل: رقم س
                    // (EN) Built-in type like: int x
                    paramType = mapTokenTypeToDataType(current_.getType());
                    advance();
                    if (check(TT::IDENTIFIER)) {
                        paramName = current_.getValue();
                        advance();
                    }
                } else if (check(TT::IDENTIFIER)) {
                    // (AR) قد يكون: ت س (نوع قالب + اسم) أو س: ت (اسم + نوع)
                    // (EN) Could be: T x (template type + name) or x: T (name + type)
                    std::string firstIdent = current_.getValue();
                    advance();
                    
                    if (check(TT::IDENTIFIER)) {
                        // (AR) صيغة: ت س (نوع معرف + اسم)
                        // (EN) Format: T x (identifier type + name)
                        templateTypeName = firstIdent;
                        paramName = current_.getValue();
                        paramType = Data::DataType::OBJECT;  // Template type as OBJECT
                        advance();
                    } else if (match(TT::COLON)) {
                        // (AR) صيغة: س: ت (اسم + نوع)
                        // (EN) Format: x: T (name + type)
                        paramName = firstIdent;
                        if (isTypeToken(current_.getType())) {
                            paramType = mapTokenTypeToDataType(current_.getType());
                            advance();
                        } else if (check(TT::IDENTIFIER)) {
                            // (AR) قد يكون معامل نوع من القالب
                            // (EN) Might be a template type parameter
                            templateTypeName = current_.getValue();
                            paramType = Data::DataType::OBJECT;
                            advance();
                        }
                    } else {
                        // (AR) فقط اسم بدون نوع
                        // (EN) Just name without type
                        paramName = firstIdent;
                    }
                }
                
                params.emplace_back(paramName, paramType);
                
            } while (matchAny({TT::COMMA, TT::ARABIC_COMMA}));
        }
        
        if (!match(TT::PAREN_RIGHT)) {
            errorBilingual(
                "خطأ نحوي: توقعت ')' بعد معاملات الدالة",
                "Syntax error: Expected ')' after function parameters"
            );
            return nullptr;
        }
        
        // (AR) تحليل نوع الإرجاع
        // (EN) Parse return type
        Data::DataType returnType = Data::DataType::NONE;
        std::string returnTypeName;
        
        // (AR) البحث عن نوع الإرجاع بعد المعاملات مع كلمة "ترجع" الاختيارية
        // (EN) Look for return type after parameters with optional "ترجع" keyword
        
        // Check for "ترجع" keyword or arrow
        if (match(TT::KEYWORD_RETURNS) || match(TT::ARROW)) {
            if (isTypeToken(current_.getType())) {
                returnType = mapTokenTypeToDataType(current_.getType());
                advance();
            } else if (check(TT::IDENTIFIER)) {
                returnTypeName = current_.getValue();
                returnType = Data::DataType::OBJECT;
                advance();
            }
        } else if (isTypeToken(current_.getType())) {
            // Built-in type without "ترجع"
            returnType = mapTokenTypeToDataType(current_.getType());
            advance();
        } else if (check(TT::IDENTIFIER)) {
            // Template type parameter as return type without "ترجع"
            // Check if it looks like a type parameter from template
            std::string possibleType = current_.getValue();
            
            // Check if this identifier is one of the template type parameters
            bool isTemplateParam = false;
            for (const auto& param : typeParams) {
                if (param.name == possibleType) {
                    isTemplateParam = true;
                    break;
                }
            }
            
            // Accept it as return type if it's a template parameter
            if (isTemplateParam) {
                returnTypeName = possibleType;
                returnType = Data::DataType::OBJECT;
                advance();
            }
        }
        
        // (AR) تحليل جسم الدالة
        // (EN) Parse function body
        StmtPtr body = nullptr;
        if (check(TT::BRACE_LEFT)) {
            body = parseBlockStmt();
        } else if (check(TT::KEYWORD_END)) {
            // (AR) دالة فارغة / (EN) Empty function
            advance();
        } else {
            // (AR) حاول قراءة الجسم
            body = parseBlockStmt();
        }
        
        return std::make_unique<AST::TemplateFunctionDecl>(
            std::move(typeParams),
            funcName,
            std::move(params),
            returnType,
            std::move(body),
            false,  // isExported
            startPos
        );
    }
    else if (match(TT::KEYWORD_CLASS)) {
        // (AR) تحليل صنف قالب
        // (EN) Parse template class
        
        // (AR) توقع اسم الصنف
        // (EN) Expect class name
        if (!check(TT::IDENTIFIER)) {
            errorBilingual(
                "خطأ نحوي: توقعت اسم الصنف بعد 'صنف'",
                "Syntax error: Expected class name after 'class'"
            );
            return nullptr;
        }
        
        std::string className = current_.getValue();
        advance();
        
        // (AR) التحقق من الوراثة
        // (EN) Check for inheritance
        std::vector<std::string> baseClasses;
        if (match(TT::KEYWORD_INHERITS) || match(TT::KEYWORD_EXTENDS)) {
            do {
                if (!check(TT::IDENTIFIER)) {
                    errorBilingual(
                        "خطأ نحوي: توقعت اسم الصنف الأب",
                        "Syntax error: Expected base class name"
                    );
                    break;
                }
                baseClasses.push_back(current_.getValue());
                advance();
            } while (matchAny({TT::COMMA, TT::ARABIC_COMMA}));
        }
        
        // (AR) تحليل أعضاء الصنف
        // (EN) Parse class members
        AST::StmtList members;
        
        // (AR) دالة مساعدة لتحليل أعضاء الصنف القالبي
        // ======================================================================
        // تتعامل هذه الدالة مع جميع أنواع أعضاء الصنف القالبي:
        //   1. خاصية (KEYWORD_PROPERTY) — حقل بسيط أو خاصية كاملة مع getter/setter
        //   2. دالة/طريقة (KEYWORD_FUNCTION) — طريقة عادية
        //   3. بناء (KEYWORD_CONSTRUCTOR_ALT) — دالة البناء
        //   4. هدم (KEYWORD_DESTRUCTOR) — دالة الهدم
        //   5. عامل (KEYWORD_OPERATOR) — تحميل العوامل الزائد
        //   6. متغير (KEYWORD_VAR) — حقل بنوع ديناميكي
        //   7. معامل نوع قالب كاسم نوع (مثل: ت اسم_الحقل)
        //   8. نوع مدمج عادي (نص، رقم، منطقي، إلخ) — حقل بنوع ثابت
        // ======================================================================
        auto parseTemplateClassMember = [this, &className, &typeParams]() -> StmtPtr {
            bool isStatic = false;
            bool isVirtual = false;
            bool isAbstract = false;
            AccessModifier access = parseModifiers(isStatic, isVirtual, isAbstract);
            
            // ─────────────────────────────────────────────────────────────
            // (AR) [1] التحقق من كلمة 'خاصية' (KEYWORD_PROPERTY)
            // ─────────────────────────────────────────────────────────────
            // الصيغ المدعومة:
            //   أ) خاصية ت اسم         — حقل بنوع قالب (ت = معامل نوع قالب)
            //   ب) خاصية نوع اسم احصل...نهاية — خاصية كاملة مع getter/setter
            //   ج) خاصية اسم            — حقل بنوع ديناميكي (OBJECT)
            //   د) خاصية نوع اسم        — حقل بنوع مدمج (بدون getter/setter)
            // ─────────────────────────────────────────────────────────────
            if (check(TT::KEYWORD_PROPERTY)) {
                advance(); // (AR) استهلاك 'خاصية'
                
                // (AR) الحالة (أ): التالي هو معامل نوع قالب (مثل: خاصية ت محتوى)
                if (check(TT::IDENTIFIER)) {
                    std::string nextIdent = current_.getValue();
                    bool isTemplateTypeParam = false;
                    for (const auto& tp : typeParams) {
                        if (tp.name == nextIdent) {
                            isTemplateTypeParam = true;
                            break;
                        }
                    }
                    if (isTemplateTypeParam) {
                        // (AR) النوع هو معامل قالب — نعامله كـ OBJECT (نوع عام)
                        advance(); // (AR) استهلاك اسم معامل النوع
                        Token nameToken = consume(TT::IDENTIFIER,
                            "(AR) توقع اسم الحقل بعد نوع القالب. (EN) Expected field name after template type.");
                        ExprPtr initializer = nullptr;
                        if (match(TT::OP_ASSIGN)) {
                            initializer = parseExpression();
                        }
                        if (matchAny({TT::SEMICOLON, TT::ARABIC_SEMICOLON})) {}
                        return std::make_unique<FieldDecl>(nameToken.getValue(), Data::DataType::OBJECT,
                            std::move(initializer), access, isStatic, nameToken.getPosition());
                    }
                }
                
                // (AR) الحالة (ب)/(د): التالي هو رمز نوع مدمج (نص، رقم، منطقي...)
                if (isTypeToken(current_.getType())) {
                    // (AR) تحقق مما إذا كان هناك 'احصل' بعد النوع والاسم
                    //      لتحديد إذا كانت خاصية كاملة أم حقل بسيط
                    // (EN) Check if there's a 'get' keyword after type+name to determine
                    //      full property vs simple field
                    return parsePropertyDeclaration(access, isStatic);
                }
                
                // (AR) الحالة (ج): خاصية متبوعة مباشرة باسم فقط (بدون نوع)
                //      نعاملها كحقل بنوع OBJECT (ديناميكي)
                if (check(TT::IDENTIFIER)) {
                    Token nameToken = current_;
                    advance(); // (AR) استهلاك اسم الحقل
                    ExprPtr initializer = nullptr;
                    if (match(TT::OP_ASSIGN)) {
                        initializer = parseExpression();
                    }
                    if (matchAny({TT::SEMICOLON, TT::ARABIC_SEMICOLON})) {}
                    return std::make_unique<FieldDecl>(nameToken.getValue(), Data::DataType::OBJECT,
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
            if (check(TT::KEYWORD_FUNCTION)) {
                advance();
                return parseMethodDeclaration(access, isStatic, isVirtual, isAbstract);
            }
            
            // ─────────────────────────────────────────────────────────────
            // (AR) [3] التحقق من دالة البناء (KEYWORD_CONSTRUCTOR_ALT = بناء)
            // ─────────────────────────────────────────────────────────────
            if (check(TT::KEYWORD_CONSTRUCTOR_ALT) ||
                (check(TT::IDENTIFIER) && current_.getValue() == className && 
                 peekNext().getType() == TT::PAREN_LEFT)) {
                if (check(TT::KEYWORD_CONSTRUCTOR_ALT)) {
                    advance();
                } else {
                    advance();
                }
                return parseConstructorDeclaration(className, access);
            }
            
            // ─────────────────────────────────────────────────────────────
            // (AR) [4] التحقق من دالة الهدم (KEYWORD_DESTRUCTOR = هدم)
            // ─────────────────────────────────────────────────────────────
            if (check(TT::KEYWORD_DESTRUCTOR)) {
                return parseDestructorDeclaration(className, access);
            }
            
            // ─────────────────────────────────────────────────────────────
            // (AR) [5] التحقق من تحميل العوامل الزائد (KEYWORD_OPERATOR = عامل)
            // ─────────────────────────────────────────────────────────────
            if (check(TT::KEYWORD_OPERATOR)) {
                advance(); // (AR) استهلاك 'عامل'
                auto operatorDecl = parseOperatorDecl();
                if (operatorDecl) {
                    if (auto* opDecl = dynamic_cast<AST::OperatorDecl*>(operatorDecl.get())) {
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
            if (check(TT::KEYWORD_VAR)) {
                advance(); // (AR) استهلاك 'متغير'
                Token nameToken = consume(TT::IDENTIFIER,
                    "(AR) توقع اسم الحقل. (EN) Expected field name.");
                ExprPtr initializer = nullptr;
                if (match(TT::OP_ASSIGN)) {
                    initializer = parseExpression();
                }
                if (matchAny({TT::SEMICOLON, TT::ARABIC_SEMICOLON})) {}
                return std::make_unique<FieldDecl>(nameToken.getValue(), Data::DataType::OBJECT, 
                    std::move(initializer), access, isStatic, nameToken.getPosition());
            }

            // ─────────────────────────────────────────────────────────────
            // (AR) [7] التحقق إذا كان المعرّف الحالي هو اسم معامل نوع قالب
            //      مثال: ت اسم_الحقل — حيث ت هو معامل النوع في القالب
            // (EN) Check if current identifier is a template type parameter name
            // ─────────────────────────────────────────────────────────────
            if (check(TT::IDENTIFIER)) {
                std::string identName = current_.getValue();
                bool isTemplateTypeParam = false;
                for (const auto& tp : typeParams) {
                    if (tp.name == identName) {
                        isTemplateTypeParam = true;
                        break;
                    }
                }
                if (isTemplateTypeParam) {
                    // (AR) هذا معامل نوع قالب - نعامله كنوع حقل OBJECT (عام)
                    advance(); // (AR) استهلاك اسم معامل النوع
                    Token nameToken = consume(TT::IDENTIFIER,
                        "(AR) توقع اسم الحقل. (EN) Expected field name.");
                    ExprPtr initializer = nullptr;
                    if (match(TT::OP_ASSIGN)) {
                        initializer = parseExpression();
                    }
                    if (matchAny({TT::SEMICOLON, TT::ARABIC_SEMICOLON})) {}
                    return std::make_unique<FieldDecl>(nameToken.getValue(), Data::DataType::OBJECT,
                        std::move(initializer), access, isStatic, nameToken.getPosition());
                }
            }

            // ─────────────────────────────────────────────────────────────
            // (AR) [8] حقل بنوع مدمج عادي (نص، رقم، منطقي، إلخ)
            // (EN) Field with built-in type (string, number, boolean, etc.)
            // ─────────────────────────────────────────────────────────────
            if (isTypeToken(current_.getType()) || 
                (check(TT::IDENTIFIER) && isClassName(current_.getValue()))) {
                return parseFieldDeclaration(access, isStatic);
            }
            
            error("(AR) عضو صنف غير معروف. (EN) Unknown class member.");
            advance();
            return nullptr;
        };
        
        if (match(TT::BRACE_LEFT)) {
            while (!check(TT::BRACE_RIGHT) && !isAtEnd()) {
                auto member = parseTemplateClassMember();
                if (member) {
                    members.push_back(std::move(member));
                }
            }
            if (!match(TT::BRACE_RIGHT)) {
                errorBilingual(
                    "خطأ نحوي: توقعت '}' لإنهاء الصنف",
                    "Syntax error: Expected '}' to close class"
                );
            }
        } else {
            // (AR) صيغة بدون أقواس: صنف اسم ... نهاية
            // (EN) No-brace syntax: class name ... end
            while (!check(TT::KEYWORD_END) && !isAtEnd()) {
                auto member = parseTemplateClassMember();
                if (member) {
                    members.push_back(std::move(member));
                }
            }
            if (!match(TT::KEYWORD_END)) {
                errorBilingual(
                    "خطأ نحوي: توقعت 'نهاية' لإنهاء الصنف",
                    "Syntax error: Expected 'end' to close class"
                );
            }
        }
        
        return std::make_unique<AST::TemplateClassDecl>(
            std::move(typeParams),
            className,
            baseClasses,
            std::move(members),
            false,  // isExported
            startPos
        );
    }
    else {
        errorBilingual(
            "خطأ نحوي: توقعت 'دالة' أو 'صنف' بعد معاملات القالب",
            "Syntax error: Expected 'function' or 'class' after template parameters"
        );
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
StmtPtr ParserCore::parseNamespaceDecl() {
    auto startPos = previous_.getPosition();
    
    // (AR) توقع اسم فضاء الأسماء
    // (EN) Expect namespace name
    if (!check(TT::IDENTIFIER)) {
        errorBilingual(
            "خطأ نحوي: توقعت اسم فضاء الأسماء",
            "Syntax error: Expected namespace name"
        );
        return nullptr;
    }
    
    std::string nsName = current_.getValue();
    advance();
    
    // (AR) تحليل أعضاء فضاء الأسماء
    // (EN) Parse namespace members
    AST::StmtList members;
    
    if (match(TT::BRACE_LEFT)) {
        // (AR) صيغة الأقواس: namespace name { ... }
        // (EN) Brace syntax
        while (!check(TT::BRACE_RIGHT) && !isAtEnd()) {
            auto decl = parseDeclaration();
            if (decl) {
                members.push_back(std::move(decl));
            }
        }
        if (!match(TT::BRACE_RIGHT)) {
            errorBilingual(
                "خطأ نحوي: توقعت '}' لإنهاء فضاء الأسماء",
                "Syntax error: Expected '}' to close namespace"
            );
        }
    } else {
        // (AR) صيغة بدون أقواس: فضاء اسم ... نهاية_فضاء
        // (EN) No-brace syntax
        while (!check(TT::KEYWORD_END_NAMESPACE) && !check(TT::KEYWORD_END) && !isAtEnd()) {
            auto decl = parseDeclaration();
            if (decl) {
                members.push_back(std::move(decl));
            }
        }
        if (!match(TT::KEYWORD_END_NAMESPACE) && !match(TT::KEYWORD_END)) {
            errorBilingual(
                "خطأ نحوي: توقعت 'نهاية_فضاء' لإنهاء فضاء الأسماء",
                "Syntax error: Expected 'end_namespace' to close namespace"
            );
        }
    }
    
    return std::make_unique<AST::NamespaceDecl>(
        nsName,
        std::move(members),
        startPos
    );
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
StmtPtr ParserCore::parseOperatorDecl() {
    auto startPos = previous_.getPosition();
    
    // (AR) توقع رمز العامل
    // (EN) Expect operator symbol
    std::string opSymbol;
    
    // (AR) التحقق من أنواع العوامل المختلفة
    // (EN) Check for different operator types
    switch (current_.getType()) {
        case TT::OP_PLUS:
            opSymbol = "+"; break;
        case TT::OP_MINUS:
            opSymbol = "-"; break;
        case TT::OP_MULTIPLY:
            opSymbol = "*"; break;
        case TT::OP_DIVIDE:
            opSymbol = "/"; break;
        case TT::OP_MODULO:
            opSymbol = "%"; break;
        case TT::OP_EQUAL:
            opSymbol = "=="; break;
        case TT::OP_NOT_EQUAL:
            opSymbol = "!="; break;
        case TT::OP_LESS:
            opSymbol = "<"; break;
        case TT::OP_GREATER:
            opSymbol = ">"; break;
        case TT::OP_LESS_EQUAL:
            opSymbol = "<="; break;
        case TT::OP_GREATER_EQUAL:
            opSymbol = ">="; break;
        case TT::BRACKET_LEFT:
            advance();
            if (match(TT::BRACKET_RIGHT)) {
                opSymbol = "[]";
            } else {
                errorBilingual(
                    "خطأ نحوي: توقعت ']' بعد '['",
                    "Syntax error: Expected ']' after '['"
                );
                return nullptr;
            }
            break;
        case TT::PAREN_LEFT:
            advance();
            if (match(TT::PAREN_RIGHT)) {
                opSymbol = "()";
            } else {
                errorBilingual(
                    "خطأ نحوي: توقعت ')' بعد '('",
                    "Syntax error: Expected ')' after '('"
                );
                return nullptr;
            }
            break;
        default:
            errorBilingual(
                "خطأ نحوي: رمز عامل غير معروف",
                "Syntax error: Unknown operator symbol"
            );
            return nullptr;
    }
    
    if (opSymbol != "[]" && opSymbol != "()") {
        advance();  // (AR) استهلاك رمز العامل
    }
    
    // (AR) تحليل المعاملات
    // (EN) Parse parameters
    if (!match(TT::PAREN_LEFT)) {
        errorBilingual(
            "خطأ نحوي: توقعت '(' بعد رمز العامل",
            "Syntax error: Expected '(' after operator symbol"
        );
        return nullptr;
    }
    
    std::vector<AST::Parameter> params;
    if (!check(TT::PAREN_RIGHT)) {
        do {
            if (!check(TT::IDENTIFIER)) {
                errorBilingual(
                    "خطأ نحوي: توقعت اسم المعامل",
                    "Syntax error: Expected parameter name"
                );
                break;
            }
            
            std::string paramName = current_.getValue();
            advance();
            
            Data::DataType paramType = Data::DataType::NONE;
            if (match(TT::COLON)) {
                if (isTypeToken(current_.getType())) {
                    paramType = mapTokenTypeToDataType(current_.getType());
                    advance();
                } else if (check(TT::IDENTIFIER)) {
                    paramType = Data::DataType::OBJECT;
                    advance();
                }
            }
            
            params.emplace_back(paramName, paramType);
            
        } while (matchAny({TT::COMMA, TT::ARABIC_COMMA}));
    }
    
    if (!match(TT::PAREN_RIGHT)) {
        errorBilingual(
            "خطأ نحوي: توقعت ')' بعد معاملات العامل",
            "Syntax error: Expected ')' after operator parameters"
        );
        return nullptr;
    }
    
    // (AR) تحليل نوع الإرجاع
    // (EN) Parse return type
    Data::DataType returnType = Data::DataType::NONE;
    if (isTypeToken(current_.getType())) {
        returnType = mapTokenTypeToDataType(current_.getType());
        advance();
    } else if (match(TT::ARROW)) {
        if (isTypeToken(current_.getType())) {
            returnType = mapTokenTypeToDataType(current_.getType());
            advance();
        }
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
        startPos
    );
}

// =========================================================================
// (AR) تحليل تنفيذ القوالب / (EN) Template Instantiation Parsing
// =========================================================================

/**
 * @brief (AR) يتحقق إذا كان الرمز الحالي بداية type argument
 *        (EN) Checks if current token starts a type argument
 */
bool ParserCore::isTypeArgumentStart() {
    // (AR) أنواع مدمجة / (EN) Built-in types
    if (isTypeToken(current_.getType())) {
        return true;
    }
    
    // (AR) معرّف يمكن أن يكون اسم صنف أو معامل قالب
    // (EN) Identifier could be class name or template parameter
    if (check(TT::IDENTIFIER)) {
        return true;
    }
    
    return false;
}

/**
 * @brief (AR) يحلل تنفيذ قالب مثل: أكبر<رقم>(10, 20)
 *        (EN) Parses template instantiation like: max<int>(10, 20)
 */
ExprPtr ParserCore::parseTemplateInstantiation(const std::string& templateName,
                                                const Lexer::Position& pos) {
    // (AR) حفظ الموقع الحالي للرجوع إذا لم يكن قالب
    // (EN) Save current position to rollback if not a template
    // Note: We'll use peek/advance pattern instead of manual index tracking
    
    // (AR) محاولة قراءة <
    // (EN) Try to consume <
    if (!match(TT::OP_LESS)) {
        return nullptr;
    }
    
    // (AR) تحليل وسائط الأنواع
    // (EN) Parse type arguments
    std::vector<Data::DataType> typeArgs;
    
    // (AR) التحقق من وجود نوع بعد <
    // (EN) Check for type after <
    if (!isTypeArgumentStart()) {
        // (AR) ليس تنفيذ قالب
        // (EN) Not template instantiation
        return nullptr;
    }
    
    do {
        Data::DataType argType = Data::DataType::UNKNOWN;
        std::string typeName;
        
        if (isTypeToken(current_.getType())) {
            // (AR) نوع مدمج
            // (EN) Built-in type
            argType = mapTokenTypeToDataType(current_.getType());
            advance();
        } else if (check(TT::IDENTIFIER)) {
            // (AR) قد يكون اسم صنف
            // (EN) Could be class name
            typeName = current_.getValue();
            argType = Data::DataType::OBJECT;
            advance();
        } else {
            // (AR) خطأ: توقعت نوع
            // (EN) Error: expected type
            return nullptr;
        }
        
        typeArgs.push_back(argType);
        
    } while (matchAny({TT::COMMA, TT::ARABIC_COMMA}));
    
    // (AR) توقع >
    // (EN) Expect >
    if (!match(TT::OP_GREATER)) {
        // (AR) ليس تنفيذ قالب صالح
        // (EN) Not valid template instantiation
        return nullptr;
    }
    
    // (AR) إنشاء عقدة TemplateInstantiation
    // (EN) Create TemplateInstantiation node
    return std::make_unique<AST::TemplateInstantiation>(
        templateName,
        std::move(typeArgs),
        pos
    );
}

} // namespace Parser
} // namespace Sad