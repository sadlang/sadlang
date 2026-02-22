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
    
    // ======================================================================
    // (AR) دعم الدوال الخارجية (External Functions / FFI)
    // (EN) External function support (FFI)
    // ======================================================================
    if (match(TT::KEYWORD_EXTERN)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مع الدوال الخارجية. (EN) Decorators cannot be used with extern functions.");
        }
        // (AR) توقع 'دالة' بعد 'خارجي'
        // (EN) Expect 'function' after 'extern'
        if (!match(TT::KEYWORD_FUNCTION)) {
            error("(AR) خطأ نحوي: توقع 'دالة' بعد 'خارجي'. (EN) Syntax error: expected 'function' after 'extern'.");
            return nullptr;
        }
        return parseExternFunctionDecl();
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
    
    if (match(TT::KEYWORD_ABSTRACT)) {
        // (AR) صنف مجرد: مجرد صنف اسم_الصنف ... نهاية
        // (EN) Abstract class: abstract class ClassName ... end
        if (!match(TT::KEYWORD_CLASS)) {
            error("(AR) توقع 'صنف' بعد 'مجرد'. (EN) Expected 'class' after 'abstract'.");
        }
        auto classDecl = parseClassDecl();
        if (auto* cd = dynamic_cast<AST::ClassDecl*>(classDecl.get())) {
            cd->isAbstract = true;
        }
        return classDecl;
    }
    
    if (match(TT::KEYWORD_CLASS)) {
        // (AR) دعم المُزخرِفات للأصناف: @مزخرف صنف اسم_الصنف ... نهاية
        // (EN) Support class decorators: @decorator class ClassName ... end
        auto classDecl = parseClassDecl();
        if (auto* cd = dynamic_cast<AST::ClassDecl*>(classDecl.get())) {
            cd->decorators = std::move(decorators);
        }
        return classDecl;
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) دعم الواجهات/السمات: سمة اسم_الواجهة ... نهاية
    // (EN) Interface/Trait support: trait TraitName ... end
    // ═══════════════════════════════════════════════════════════════════
    if (match(TT::KEYWORD_TRAIT)) {
        return parseTraitDecl();
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // (AR) دعم تنفيذ الواجهات: نفّذ اسم_الواجهة لـ اسم_الصنف ... نهاية
    // (EN) Impl block: impl TraitName for ClassName ... end
    // ═══════════════════════════════════════════════════════════════════
    if (match(TT::KEYWORD_IMPL)) {
        return parseImplDecl();
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
        pendingConst_ = true;
        auto result = parseVarDecl();
        pendingConst_ = false;
        return result;
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
    
    if (match(TT::KEYWORD_STRUCT)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مع البنى. (EN) Decorators cannot be used with structs.");
        }
        return parseStructDecl();
    }
    
    if (match(TT::KEYWORD_TEST)) {
        if (!decorators.empty()) {
            error("(AR) المُزخرِفات لا تُستخدم مع الاختبارات. (EN) Decorators cannot be used with tests.");
        }
        return parseTestDecl();
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


} // namespace Parser
} // namespace Sad
