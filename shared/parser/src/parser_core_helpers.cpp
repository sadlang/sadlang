/**
 * @file parser_core_helpers.cpp
 * @brief (AR) دوال مساعدة للمحلل النحوي - الجزء الثاني من التنفيذ.
 *        (EN) Parser helper functions - second part of implementation.
 * 
 * @author Sad Language Development Team
 * @date 2025-11-11
 * @license MIT License
 */

#include "parser_core.h"
#include "class_manager.h"
#include <iostream>
#include <sstream>

namespace Sad {
namespace Parser {

using namespace AST;
using namespace Lexer;
using TT = TokenType;

// ======================================================================
// (AR) دوال List/Dict Comprehensions / (EN) Comprehension Functions
// ======================================================================

/**
 * @brief (AR) يحلل List Comprehension: [expr for var in iter if cond].
 *        (EN) Parses list comprehension: [expr for var in iter if cond].
 */
ExprPtr ParserCore::parseListComprehension() {
    // Parse expression
    // (AR) تحليل التعبير
    auto expr = parseExpression();
    
    // Expect 'for' keyword
    // (AR) توقع كلمة 'لكل'
    if (!match(TT::KEYWORD_FOR)) {
        error("(AR) توقع 'لكل' في list comprehension. (EN) Expected 'for' in list comprehension.");
    }
    
    // Parse variable
    // (AR) تحليل المتغير
    Token var = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم متغير. (EN) Expected variable name.");
    
    // Expect 'in' keyword
    // (AR) توقع كلمة 'في'
    if (!match(TT::KEYWORD_IN)) {
        error("(AR) توقع 'في' في list comprehension. (EN) Expected 'in' in list comprehension.");
    }
    
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
        "(AR) توقع ']' بعد list comprehension. (EN) Expected ']' after list comprehension.");
    
    // Create list comprehension node
    // (AR) إنشاء عقدة List Comprehension
    return std::make_unique<ListComprehensionExpr>(
        std::move(expr),
        var.getValue(),
        std::move(iterable),
        std::move(condition),
        var.getPosition()
    );
}

/**
 * @brief (AR) يحلل Dict Comprehension: {k: v for k, v in dict if cond}.
 *        (EN) Parses dictionary comprehension: {k: v for k, v in dict if cond}.
 */
ExprPtr ParserCore::parseDictComprehension() {
    // Parse key expression
    // (AR) تحليل تعبير المفتاح
    auto keyExpr = parseExpression();
    
    consume(TT::COLON, 
        "(AR) توقع ':' بعد مفتاح dict comprehension. (EN) Expected ':' after dict comprehension key.");
    
    // Parse value expression
    // (AR) تحليل تعبير القيمة
    auto valueExpr = parseExpression();
    
    // Expect 'for' keyword
    // (AR) توقع كلمة 'لكل'
    if (!match(TT::KEYWORD_FOR)) {
        error("(AR) توقع 'لكل' في dict comprehension. (EN) Expected 'for' in dict comprehension.");
    }
    
    // Parse key variable
    // (AR) تحليل متغير المفتاح
    Token keyVar = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم متغير المفتاح. (EN) Expected key variable name.");
    
    consume(TT::COMMA, 
        "(AR) توقع ',' بعد متغير المفتاح. (EN) Expected ',' after key variable.");
    
    // Parse value variable
    // (AR) تحليل متغير القيمة
    Token valueVar = consume(TT::IDENTIFIER, 
        "(AR) توقع اسم متغير القيمة. (EN) Expected value variable name.");
    
    // Expect 'in' keyword
    // (AR) توقع كلمة 'في'
    if (!match(TT::KEYWORD_IN)) {
        error("(AR) توقع 'في' في dict comprehension. (EN) Expected 'in' in dict comprehension.");
    }
    
    // Parse iterable
    // (AR) تحليل المجموعة
    auto iterable = parseExpression();
    
    // Optional condition
    // (AR) الشرط الاختياري
    ExprPtr condition = nullptr;
    if (match(TT::KEYWORD_IF)) {
        condition = parseExpression();
    }
    
    consume(TT::BRACE_RIGHT, 
        "(AR) توقع '}' بعد dict comprehension. (EN) Expected '}' after dict comprehension.");
    
    // Create dict comprehension node (use keyVar as main variable)
    // (AR) إنشاء عقدة Dict Comprehension (استخدم keyVar كمتغير رئيسي)
    // Note: Current implementation uses single variable, consider enhancing for key-value pairs
    return std::make_unique<DictComprehensionExpr>(
        std::move(keyExpr),
        std::move(valueExpr),
        keyVar.getValue(),
        std::move(iterable),
        std::move(condition),
        keyVar.getPosition()
    );
}

// ======================================================================
// (AR) الدوال المساعدة / (EN) Helper Functions
// ======================================================================

/**
 * @brief (AR) يتقدم إلى الرمز التالي.
 *        (EN) Advances to next token.
 */
void ParserCore::advance() {
    previous_ = current_;
    current_ = nextToken_; // Move nextToken_ to current_
    
    // Fetch new nextToken_ for lookahead
    nextToken_ = lexer_.nextToken();
    
    // Skip whitespace, comments, and doc-comments in both current and nextToken_
    // (AR) تجاوز المسافات والتعليقات والتعليقات التوثيقية في كل من current و nextToken_
    while (current_.getType() == TT::WHITESPACE || 
           current_.getType() == TT::COMMENT ||
           current_.getType() == TT::DOC_COMMENT ||
           current_.getType() == TT::NEWLINE) {
        current_ = nextToken_;
        nextToken_ = lexer_.nextToken();
    }
}

/**
 * @brief (AR) يتحقق من نوع الرمز الحالي دون التقدم.
 *        (EN) Checks current token type without advancing.
 */
bool ParserCore::check(TokenType type) const {
    if (isAtEnd()) return false;
    return current_.getType() == type;
}

/**
 * @brief (AR) يتطابق ويتقدم إذا كان الرمز من النوع المحدد.
 *        (EN) Matches and advances if token is of specified type.
 */
bool ParserCore::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

/**
 * @brief (AR) يتطابق مع أي نوع من القائمة ويتقدم.
 *        (EN) Matches any type from list and advances.
 */
bool ParserCore::matchAny(const std::vector<TokenType>& types) {
    for (const auto& type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

/**
 * @brief (AR) يستهلك رمزاً من النوع المحدد أو يرفع خطأ.
 *        (EN) Consumes token of specified type or raises error.
 */
Token ParserCore::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return previous();
    }
    
    // (AR) إنشاء رسالة خطأ ثنائية اللغة تلقائية
    // (EN) Create automatic bilingual error message
    std::string expected_ar;
    std::string expected_en;
    std::string fixText;
    std::string fixDesc_ar;
    std::string fixDesc_en;
    bool addFixIt = false;
    
    switch (type) {
        case TT::PAREN_RIGHT:
            expected_ar = "قوس إغلاق ')'";
            expected_en = "closing parenthesis ')'";
            fixText = ")";
            fixDesc_ar = "أضف ')' هنا";
            fixDesc_en = "Add ')' here";
            addFixIt = true;
            break;
        case TT::PAREN_LEFT:
            expected_ar = "قوس فتح '('";
            expected_en = "opening parenthesis '('";
            fixText = "(";
            fixDesc_ar = "أضف '(' هنا";
            fixDesc_en = "Add '(' here";
            addFixIt = true;
            break;
        case TT::BRACKET_RIGHT:
            expected_ar = "قوس مربع إغلاق ']'";
            expected_en = "closing bracket ']'";
            fixText = "]";
            fixDesc_ar = "أضف ']' هنا";
            fixDesc_en = "Add ']' here";
            addFixIt = true;
            break;
        case TT::BRACKET_LEFT:
            expected_ar = "قوس مربع فتح '['";
            expected_en = "opening bracket '['";
            fixText = "[";
            fixDesc_ar = "أضف '[' هنا";
            fixDesc_en = "Add '[' here";
            addFixIt = true;
            break;
        case TT::BRACE_RIGHT:
            expected_ar = "قوس معقوف إغلاق '}'";
            expected_en = "closing brace '}'";
            fixText = "}";
            fixDesc_ar = "أضف '}' هنا";
            fixDesc_en = "Add '}' here";
            addFixIt = true;
            break;
        case TT::BRACE_LEFT:
            expected_ar = "قوس معقوف فتح '{'";
            expected_en = "opening brace '{'";
            fixText = "{";
            fixDesc_ar = "أضف '{' هنا";
            fixDesc_en = "Add '{' here";
            addFixIt = true;
            break;
        case TT::SEMICOLON:
        case TT::ARABIC_SEMICOLON:
            expected_ar = "فاصلة منقوطة ';'";
            expected_en = "semicolon ';'";
            fixText = ";";
            fixDesc_ar = "أضف ';' في نهاية الجملة";
            fixDesc_en = "Add ';' at end of statement";
            addFixIt = true;
            break;
        case TT::COMMA:
            expected_ar = "فاصلة ','";
            expected_en = "comma ','";
            fixText = ",";
            fixDesc_ar = "أضف ',' للفصل بين العناصر";
            fixDesc_en = "Add ',' to separate items";
            addFixIt = true;
            break;
        case TT::COLON:
            expected_ar = "نقطتان ':'";
            expected_en = "colon ':'";
            fixText = ":";
            fixDesc_ar = "أضف ':' هنا";
            fixDesc_en = "Add ':' here";
            addFixIt = true;
            break;
        case TT::KEYWORD_END:
            expected_ar = "كلمة 'نهاية'";
            expected_en = "keyword 'نهاية' (end)";
            fixText = "نهاية";
            fixDesc_ar = "أضف 'نهاية' لإغلاق الكتلة";
            fixDesc_en = "Add 'نهاية' to close block";
            addFixIt = true;
            break;
        case TT::IDENTIFIER:
            expected_ar = "معرّف (اسم متغير أو دالة)";
            expected_en = "identifier (variable or function name)";
            break;
        case TT::OP_ASSIGN:
            expected_ar = "علامة إسناد '='";
            expected_en = "assignment '='";
            break;
        case TT::ARROW:
            expected_ar = "سهم '->'";
            expected_en = "arrow '->'";
            break;
        default:
            expected_ar = "رمز من نوع " + std::to_string(static_cast<int>(type));
            expected_en = "token of type " + std::to_string(static_cast<int>(type));
            break;
    }
    
    // (AR) إذا كانت رسالة مخصصة موجودة، استخدمها
    // (EN) If custom message exists, use it
    if (!message.empty() && message.find("Expected") == std::string::npos) {
        error(message);
    } else {
        // (AR) إنشاء رسالة تلقائية
        // (EN) Create automatic message
        std::string msg_ar = "خطأ نحوي: توقعت " + expected_ar + 
                            "، لكن وجدت '" + current_.getValue() + "'";
        std::string msg_en = "Syntax error: expected " + expected_en + 
                            ", but found '" + current_.getValue() + "'";
        
        if (addFixIt) {
            errorWithFixIt(msg_ar, fixText, fixDesc_ar, fixDesc_en);
        } else {
            errorBilingual(msg_ar, msg_en);
        }
    }
    
    // Return dummy token to allow continuation
    // (AR) إرجاع رمز وهمي للسماح بالاستمرار
    return current_;
}

/**
 * @brief (AR) يتحقق من الوصول لنهاية الملف.
 *        (EN) Checks if reached end of file.
 */
bool ParserCore::isAtEnd() const {
    bool result = current_.getType() == TT::END_OF_FILE;
    if (result) {
        // DEBUG: Disabled
        // std::cout << "[parser_core_helpers.cpp] isAtEnd() = true - current token type: " 
        //           << static_cast<int>(current_.getType()) << "\n";
    }
    return result;
}

/**
 * @brief (AR) يرجع الرمز الحالي.
 *        (EN) Returns current token.
 */
const Token& ParserCore::peek() const {
    return current_;
}

/**
 * @brief (AR) يرجع الرمز التالي (lookahead 2) بشكل صحيح.
 *        (EN) Returns next token (lookahead 2) correctly.
 * 
 * @note (AR) الآن تستخدم nextToken_ المخزن مسبقًا لتوفير نظر مسبق حقيقي.
 *       (EN) Now uses pre-cached nextToken_ to provide true lookahead.
 */
const Token& ParserCore::peekNext() const {
    return nextToken_;
}

/**
 * @brief (AR) يرجع الرمز السابق.
 *        (EN) Returns previous token.
 */
const Token& ParserCore::previous() const {
    return previous_;
}

/**
 * @brief (AR) يسجل خطأ تحليل مع معلومات المكان.
 *        (EN) Records parsing error with location information.
 */
void ParserCore::errorWithFixIt(const std::string& message,
                                const std::string& fixText,
                                const std::string& fixDesc_ar,
                                const std::string& fixDesc_en) {
    if (panicMode_) return;  // Avoid error cascades
    
    panicMode_ = true;
    
    // (AR) بناء SourceLocation من الرمز الحالي
    // (EN) Build SourceLocation from current token
    Errors::SourceLocation loc(
        filename_.empty() ? "<source>" : filename_,
        current_.getPosition().line,
        current_.getPosition().column,
        current_.getPosition().offset,
        current_.getPosition().length
    );
    
    // (AR) إنشاء FixItHint
    // (EN) Create FixItHint
    Errors::FixItHint fixIt(
        Errors::FixItHint::Type::INSERT,
        loc,
        fixText,
        fixDesc_ar,
        fixDesc_en
    );
    
    // (AR) إنشاء Diagnostic كامل مع Fix-it hint
    // (EN) Create complete Diagnostic with Fix-it hint
    Errors::Diagnostic diag(
        Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
        Errors::DiagnosticSeverity::ERROR,
        loc,
        message,  // Arabic message
        message   // English message (same for now)
    );
    diag.addFixIt(fixIt);
    
    // (AR) استخدام ErrorManager لتسجيل الخطأ
    // (EN) Use ErrorManager to report error
    Errors::ErrorManager::getInstance().report(diag);
}

void ParserCore::error(const std::string& message) {
    // (AR) لا نتجاهل الأخطاء حتى في panic mode - نعرضها دائماً
    // (EN) Don't ignore errors even in panic mode - always display them
    // if (panicMode_) return;  // REMOVED: We want to see ALL errors
    
    panicMode_ = true;
    
    // (AR) بناء SourceLocation من الرمز الحالي
    // (EN) Build SourceLocation from current token
    Errors::SourceLocation loc(
        filename_.empty() ? "<source>" : filename_,
        current_.getPosition().line,
        current_.getPosition().column,
        current_.getPosition().offset,
        current_.getPosition().length
    );
    
    // (AR) إضافة معلومات السياق لرسالة الخطأ
    // (EN) Add context information to error message
    std::string enhanced_ar = "⛔ " + message;
    std::string enhanced_en = "⛔ " + message;
    
    // إضافة معلومات عن الرمز الحالي
    enhanced_ar += "\n   📍 الموقع: السطر " + std::to_string(loc.line) + 
                   "، العمود " + std::to_string(loc.column);
    enhanced_en += "\n   📍 Location: line " + std::to_string(loc.line) + 
                   ", column " + std::to_string(loc.column);
    
    if (current_.getType() != TokenType::END_OF_FILE) {
        enhanced_ar += "\n   🔎 الرمز الحالي: '" + current_.getValue() + "'";
        enhanced_en += "\n   🔎 Current token: '" + current_.getValue() + "'";
    } else {
        enhanced_ar += "\n   🔎 الرمز الحالي: <نهاية الملف>";
        enhanced_en += "\n   🔎 Current token: <end of file>";
    }
    
    // (AR) استخدام ErrorManager لتسجيل الخطأ
    // (EN) Use ErrorManager to report error
    Errors::ErrorManager::getInstance().reportError(
        Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
        loc,
        enhanced_ar,
        enhanced_en
    );
    
    // (AR) طباعة الخطأ فوراً للمستخدم
    // (EN) Print error immediately for user
    std::cerr << "\n" << enhanced_ar << "\n" << enhanced_en << "\n" << std::endl;
}

/**
 * @brief (AR) يسجل خطأ مع رسالة ثنائية اللغة وطباعة الكود المصدري.
 *        (EN) Records error with bilingual message and prints source code.
 */
void ParserCore::errorBilingual(const std::string& message_ar,
                                const std::string& message_en) {
    // (AR) لا نتجاهل الأخطاء حتى في panic mode - نعرضها دائماً
    // (EN) Don't ignore errors even in panic mode - always display them
    // if (panicMode_) return;  // REMOVED: We want to see ALL errors
    
    panicMode_ = true;
    
    Errors::SourceLocation loc(
        filename_.empty() ? "<source>" : filename_,
        current_.getPosition().line,
        current_.getPosition().column,
        current_.getPosition().offset,
        current_.getPosition().length
    );
    
    // (AR) إضافة معلومات السياق لرسالة الخطأ
    // (EN) Add context information to error message
    std::string enhanced_ar = "⛔ " + message_ar;
    std::string enhanced_en = "⛔ " + message_en;
    
    // إضافة معلومات عن الرمز الحالي والموقع
    enhanced_ar += "\n   📍 الموقع: السطر " + std::to_string(loc.line) + 
                   "، العمود " + std::to_string(loc.column);
    enhanced_en += "\n   📍 Location: line " + std::to_string(loc.line) + 
                   ", column " + std::to_string(loc.column);
    
    if (current_.getType() != TokenType::END_OF_FILE) {
        enhanced_ar += "\n   🔎 الرمز الحالي: '" + current_.getValue() + "'";
        enhanced_en += "\n   🔎 Current token: '" + current_.getValue() + "'";
    } else {
        enhanced_ar += "\n   🔎 الرمز الحالي: <نهاية الملف>";
        enhanced_en += "\n   🔎 Current token: <end of file>";
    }
    
    Errors::ErrorManager::getInstance().reportError(
        Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
        loc,
        message_ar,
        message_en
    );
    
    // (AR) طباعة الخطأ فوراً للمستخدم مع التفاصيل
    // (EN) Print error immediately for user with details
    std::cerr << "\n" << enhanced_ar << "\n" << enhanced_en << "\n" << std::endl;
}

/**
 * @brief (AR) يُنشئ رسالة خطأ لرمز غير متوقع مع التوقع.
 *        (EN) Creates error message for unexpected token with expectation.
 */
void ParserCore::errorExpectedToken(const std::string& expected_ar,
                                    const std::string& expected_en,
                                    const std::string& context_ar,
                                    const std::string& context_en) {
    if (panicMode_) return;
    panicMode_ = true;
    
    Errors::SourceLocation loc(
        filename_.empty() ? "<source>" : filename_,
        current_.getPosition().line,
        current_.getPosition().column,
        current_.getPosition().offset,
        current_.getPosition().length
    );
    
    // (AR) بناء رسالة الخطأ
    // (EN) Build error message
    std::string msg_ar = "خطأ نحوي: توقعت " + expected_ar;
    std::string msg_en = "Syntax error: expected " + expected_en;
    
    if (!context_ar.empty()) {
        msg_ar += " " + context_ar;
        msg_en += " " + context_en;
    }
    
    msg_ar += "، لكن وجدت '" + current_.getValue() + "'";
    msg_en += ", but found '" + current_.getValue() + "'";
    
    // (AR) إضافة معلومات عن الموقع
    // (EN) Add location information
    msg_ar += " في السطر " + std::to_string(loc.line) + 
              "، العمود " + std::to_string(loc.column);
    msg_en += " at line " + std::to_string(loc.line) + 
              ", column " + std::to_string(loc.column);
    
    Errors::ErrorManager::getInstance().reportError(
        Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,
        loc,
        msg_ar,
        msg_en
    );
}

/**
 * @brief (AR) يُنشئ رسالة خطأ لجملة غير مكتملة.
 *        (EN) Creates error message for incomplete statement.
 */
void ParserCore::errorIncompleteStatement(const std::string& statement_ar,
                                          const std::string& statement_en,
                                          const std::string& missing_ar,
                                          const std::string& missing_en) {
    if (panicMode_) return;
    panicMode_ = true;
    
    Errors::SourceLocation loc(
        filename_.empty() ? "<source>" : filename_,
        current_.getPosition().line,
        current_.getPosition().column,
        current_.getPosition().offset,
        current_.getPosition().length
    );
    
    std::string msg_ar = "خطأ: " + statement_ar + " غير مكتملة - ينقصها " + missing_ar;
    std::string msg_en = "Error: incomplete " + statement_en + " - missing " + missing_en;
    
    msg_ar += " في السطر " + std::to_string(loc.line);
    msg_en += " at line " + std::to_string(loc.line);
    
    Errors::ErrorManager::getInstance().reportError(
        Errors::ErrorCode::SYN_UNEXPECTED_TOKEN,  // استخدام كود موجود
        loc,
        msg_ar,
        msg_en
    );
}

/**
 * @brief (AR) يحاول التعافي من خطأ بالتقدم للجملة التالية.
 *        (EN) Attempts error recovery by advancing to next statement.
 */
void ParserCore::synchronize() {
    // (AR) عرض رسالة التعافي
    // (EN) Display recovery message
    std::cerr << "\n🔧 (AR) محاولة التعافي من الخطأ...\n";
    std::cerr << "🔧 (EN) Attempting error recovery...\n" << std::endl;
    
    panicMode_ = false;
    
    advance();
    
    int tokens_skipped = 0;
    
    // Skip until statement boundary
    // (AR) التجاوز حتى حدود الجملة
    while (!isAtEnd()) {
        if (previous().getType() == TT::SEMICOLON) {
            std::cerr << "✓ (AR) تم التعافي عند نهاية الجملة (تجاوز " << tokens_skipped << " رمز)\n";
            std::cerr << "✓ (EN) Recovered at statement end (skipped " << tokens_skipped << " tokens)\n\n";
            return;
        }
        
        // Stop at statement keywords
        // (AR) التوقف عند كلمات الجمل المفتاحية
        switch (current_.getType()) {
            case TT::KEYWORD_CLASS:
            case TT::KEYWORD_FUNCTION:
            case TT::KEYWORD_CONST:
            case TT::KEYWORD_FOR:
            case TT::KEYWORD_IF:
            case TT::KEYWORD_WHILE:
            case TT::KEYWORD_RETURN:
            case TT::TYPE_INTEGER:
            case TT::TYPE_DOUBLE:
            case TT::TYPE_STRING:
            case TT::TYPE_BOOLEAN:
                std::cerr << "✓ (AR) تم التعافي عند بداية تصريح/جملة جديدة (تجاوز " << tokens_skipped << " رمز)\n";
                std::cerr << "✓ (EN) Recovered at new declaration/statement (skipped " << tokens_skipped << " tokens)\n\n";
                return;
            default:
                break;
        }
        
        tokens_skipped++;
        advance();
    }
    
    std::cerr << "⚠ (AR) وصلنا لنهاية الملف أثناء التعافي\n";
    std::cerr << "⚠ (EN) Reached end of file during recovery\n\n";
}

/**
 * @brief (AR) يحلل قائمة معاملات الدالة: (x, y, z) أو (x، y، z).
 *        (EN) Parses function parameter list: (x, y, z) or (x، y، z).
 */
std::vector<std::string> ParserCore::parseParameterList() {
    std::vector<std::string> parameters;
    
    // Parse parameters
    // (AR) تحليل المعاملات
    if (!check(TT::PAREN_RIGHT)) {
        do {
            Token param = consume(TT::IDENTIFIER, 
                "(AR) توقع اسم معامل. (EN) Expected parameter name.");
            parameters.push_back(param.getValue());
        } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));  // (AR) دعم الفاصلة العربية (،)
    }
    
    return parameters;
}

/**
 * @brief (AR) يحلل قائمة معاملات مكتوبة: (x: int, y: float = 10) أو (int x, float y = 10).
 *        (EN) Parses typed parameter list: (x: int, y: float = 10) or (int x, float y = 10).
 */
std::vector<Parameter> ParserCore::parseTypedParameterList() {
    std::vector<Parameter> parameters;
    
    // Parse parameters
    // (AR) تحليل المعاملات
    if (!check(TT::PAREN_RIGHT)) {
        do {
            // (AR) دعم صيغتين: "نوع اسم" أو "اسم: نوع"
            // (EN) Support two syntaxes: "type name" or "name: type"
            Data::DataType paramType = Data::DataType::UNKNOWN;
            
            // Check if next token is a type keyword (type-first syntax: "رقم س")
            // (AR) تحقق إذا كان الرمز التالي كلمة مفتاحية لنوع (صيغة النوع أولاً: "رقم س")
            if (check(TT::TYPE_INTEGER) || check(TT::TYPE_DOUBLE) || check(TT::TYPE_STRING) || 
                check(TT::TYPE_BOOLEAN) || check(TT::TYPE_VOID) || check(TT::TYPE_NULL) ||
                check(TT::TYPE_ARRAY) || check(TT::TYPE_MAP)) {
                // Type-first syntax: "int x"
                // (AR) صيغة النوع أولاً: "رقم س"
                paramType = parseType();
                Token paramName = consume(TT::IDENTIFIER, 
                    "(AR) توقع اسم معامل بعد النوع. (EN) Expected parameter name after type.");
                
                // Optional default value: type name = value
                // (AR) القيمة الافتراضية الاختيارية: نوع اسم = قيمة
                ExprPtr defaultValue = nullptr;
                if (match(TT::OP_ASSIGN)) {
                    defaultValue = parseExpression();
                }
                
                parameters.emplace_back(
                    paramName.getValue(),
                    paramType,
                    std::move(defaultValue)
                );
            } else if (check(TT::IDENTIFIER) && peekNext().getType() == TT::IDENTIFIER) {
                // ═══════════════════════════════════════════════════════════════
                // (AR) صيغة نوع الصنف: "اسم_صنف اسم_معامل"
                //      مثال: "شخص ش" أو "سيارة س"
                //      يُعامل المعرّف الأول كاسم صنف والمعرّف الثاني كاسم المعامل
                //
                // (EN) Class-type syntax: "ClassName paramName"
                //      Example: "Person p" or "Car c"
                //      Treats first identifier as class type, second as parameter name
                // ═══════════════════════════════════════════════════════════════
                std::string className = current_.getValue();
                advance();  // (AR) تخطي اسم الصنف / (EN) skip class name
                Token paramName = consume(TT::IDENTIFIER,
                    "(AR) توقع اسم معامل بعد اسم الصنف. (EN) Expected parameter name after class type.");
                
                // (AR) القيمة الافتراضية الاختيارية
                // (EN) Optional default value
                ExprPtr defaultValue = nullptr;
                if (match(TT::OP_ASSIGN)) {
                    defaultValue = parseExpression();
                }
                
                parameters.emplace_back(
                    paramName.getValue(),
                    Data::DataType::OBJECT,
                    std::move(defaultValue),
                    className
                );
            } else {
                // Name-first syntax: "x: int"
                // (AR) صيغة الاسم أولاً: "س: رقم"
                Token paramName = consume(TT::IDENTIFIER, 
                    "(AR) توقع اسم معامل. (EN) Expected parameter name.");
                
                // Optional type annotation: name : type
                // (AR) تصريح النوع الاختياري: اسم : نوع
                if (match(TT::COLON)) {
                    paramType = parseType();
                }
                
                // Optional default value: name : type = value
                // (AR) القيمة الافتراضية الاختيارية: اسم : نوع = قيمة
                ExprPtr defaultValue = nullptr;
                if (match(TT::OP_ASSIGN)) {
                    defaultValue = parseExpression();
                }
                
                parameters.emplace_back(
                    paramName.getValue(),
                    paramType,
                    std::move(defaultValue)
                );
            }
            
        } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));  // (AR) دعم الفاصلة العربية (،)
    }
    
    return parameters;
}

/**
 * @brief (AR) يحلل قائمة وسائط استدعاء: f(1, 2, 3) أو f(1، 2، 3).
 *        (EN) Parses function call argument list: f(1, 2, 3) or f(1، 2، 3).
 */
ExprList ParserCore::parseArgumentList() {
    ExprList arguments;
    
    // Parse arguments
    // (AR) تحليل الوسائط
    if (!check(TT::PAREN_RIGHT)) {
        do {
            arguments.push_back(parseExpression());
        } while (match(TT::COMMA) || match(TT::ARABIC_COMMA));  // (AR) دعم الفاصلة العربية (،)
    }
    
    return arguments;
}

// ======================================================================
// (AR) دوال نظام الأنواع / (EN) Type System Functions
// ======================================================================

/**
 * @brief (AR) يحلل نوع بيانات أساسي أو مركب.
 *        (EN) Parses basic or composite data type.
 * 
 * @details (AR) تدعم الأنواع الأساسية بالعربية والإنجليزية:
 *               - رقم / int → INTEGER
 *               - عشري / float → FLOAT
 *               - نص / string → STRING
 *               - منطقي / bool → BOOLEAN
 *               - فراغ / void → NONE
 *               - مصفوفة / array → ARRAY (with optional generic params)
 *               - قاموس / dict/map → MAP (with optional generic params)
 * 
 *          (EN) Supports basic types in Arabic and English:
 *               - رقم / int → INTEGER
 *               - عشري / float → FLOAT
 *               - نص / string → STRING
 *               - منطقي / bool → BOOLEAN
 *               - فراغ / void → NONE
 *               - مصفوفة / array → ARRAY (with optional generic params)
 *               - قاموس / dict/map → MAP (with optional generic params)
 */
Data::DataType ParserCore::parseType() {
    using DT = Data::DataType;
    
    // ========== الأنواع الأساسية - العربية ==========
    // Basic Types - Arabic
    
    if (match(TT::TYPE_INTEGER)) {
        // "رقم" → INTEGER
        return DT::INTEGER;
    }
    
    if (match(TT::TYPE_DOUBLE)) {
        // "عشري" → FLOAT
        return DT::FLOAT;
    }
    
    if (match(TT::TYPE_STRING)) {
        // "نص" → STRING
        return DT::STRING;
    }
    
    if (match(TT::TYPE_BOOLEAN)) {
        // "منطقي" → BOOLEAN
        return DT::BOOLEAN;
    }
    
    if (match(TT::TYPE_VOID)) {
        // "فراغ" → NONE
        return DT::NONE;
    }
    
    if (match(TT::TYPE_NULL)) {
        // "عدم" / "null" → NONE
        return DT::NONE;
    }
    
    // ========== الأنواع المركبة ==========
    // Composite Types
    
    if (match(TT::TYPE_ARRAY)) {
        // "مصفوفة" / "array" → ARRAY
        // Check for generic type: Array<int>
        if (check(TT::OP_LESS)) {
            return parseGenericType(DT::ARRAY);
        }
        return DT::ARRAY;
    }
    
    if (match(TT::TYPE_MAP)) {
        // "قاموس" / "dict" / "map" → MAP
        // Check for generic type: Map<string, int>
        if (check(TT::OP_LESS)) {
            return parseGenericType(DT::MAP);
        }
        return DT::MAP;
    }
    
    // ========== Type not found ==========
    error("(AR) توقع نوع بيانات صحيح (رقم، نص، منطقي، إلخ). "
          "(EN) Expected valid data type (int, string, bool, etc).");
    return DT::UNKNOWN;
}

/**
 * @brief (AR) يحلل نوع عام مع معاملات: Array<T>, Map<K, V>.
 *        (EN) Parses generic type with parameters: Array<T>, Map<K, V>.
 * 
 * @param baseType (AR) النوع الأساسي (ARRAY أو MAP).
 *                 (EN) Base type (ARRAY or MAP).
 * 
 * @details (AR) مثال: Array<int> → يحلل int كمعامل عام
 *               مثال: Map<string, float> → يحلل string و float
 * 
 *          (EN) Example: Array<int> → parses int as generic parameter
 *               Example: Map<string, float> → parses string and float
 * 
 * @note (AR) التنفيذ الحالي يقرأ المعاملات العامة لكن لا يخزنها.
 *            سيتم تحسينه في المرحلة التالية لدعم Type Checking الكامل.
 * 
 *       (EN) Current implementation reads generic parameters but doesn't store them.
 *            Will be enhanced in next phase to support full Type Checking.
 */
Data::DataType ParserCore::parseGenericType(Data::DataType baseType) {
    // Consume '<'
    consume(TT::OP_LESS, 
            "(AR) توقع '<' بعد اسم النوع العام. "
            "(EN) Expected '<' after generic type name.");
    
    // Parse first type parameter
    // (AR) تحليل معامل النوع الأول
    Data::DataType param1 = parseType();
    (void)param1; // Suppress unused variable warning
    
    // For Map type, parse second parameter
    // (AR) للنوع Map، تحليل المعامل الثاني
    if (baseType == Data::DataType::MAP) {
        consume(TT::COMMA, 
                "(AR) توقع ',' بين معاملات Map. "
                "(EN) Expected ',' between Map parameters.");
        
        Data::DataType param2 = parseType();
        (void)param2; // Suppress unused variable warning
    }
    
    // Consume '>'
    consume(TT::OP_GREATER, 
            "(AR) توقع '>' بعد معاملات النوع العام. "
            "(EN) Expected '>' after generic type parameters.");
    
    // TODO: Store generic parameters in AST for type checking
    // (AR) مستقبلاً: حفظ المعاملات العامة في AST للتحقق من الأنواع
    
    return baseType;
}

// ======================================================================
// (AR) دوال Arrow Functions / (EN) Arrow Function Support
// ======================================================================

/**
 * @brief (AR) يتحقق إذا كان التسلسل الحالي arrow function.
 *        (EN) Checks if current sequence is arrow function.
 * 
 * Lookahead patterns:
 *   1. identifier '=>'  (e.g., x => x * 2)
 *   2. '(' ... ')' '=>'  (e.g., (x, y) => x + y)
 * 
 * @return (AR) true إذا كان arrow function، وإلا false.
 *         (EN) true if arrow function, false otherwise.
 */
bool ParserCore::isArrowFunction() {
    // TODO: Temporarily disabled - needs proper implementation
    // (AR) معطل مؤقتاً - يحتاج تنفيذ صحيح
    return false;
}

/**
 * @brief (AR) يحلل arrow function: (x, y) => x + y.
 *        (EN) Parses arrow function: (x, y) => x + y.
 * 
 * Grammar:
 *   arrow_function ::= '(' [typed_param_list] ')' '=>' expression
 *                    | identifier '=>' expression
 *                    | '(' ')' '=>' expression
 * 
 * Examples:
 *   - (x, y) => x + y
 *   - x => x * 2
 *   - () => 42
 *   - (x: int, y: int) => x + y
 * 
 * @return (AR) مؤشر لعقدة تعبير arrow function (LambdaExpr).
 *         (EN) Pointer to arrow function expression node (LambdaExpr).
 */
ExprPtr ParserCore::parseArrowFunction() {
    // TODO: Temporarily disabled - needs proper implementation
    // (AR) معطل مؤقتاً - يحتاج تنفيذ صحيح
    error("Arrow functions not yet implemented");
    return nullptr;
}

// ======================================================================
// (AR) دوال فحص النوع / (EN) Type Checking Functions
// ======================================================================

/**
 * @brief (AR) يفحص ما إذا كان الرمز الحالي رمز نوع بيانات.
 *        (EN) Checks if current token is a data type token.
 * 
 * (AR) يتحقق من أن الرمز الحالي يمثل نوع بيانات أساسي (رقم، عشري، نص، إلخ)
 * (EN) Checks if current token represents a basic data type (int, float, string, etc)
 */
bool ParserCore::isTypeToken(TokenType tokenType) {
    using TT = TokenType;
    
    // Basic data types
    // (AR) أنواع البيانات الأساسية
    return tokenType == TT::TYPE_INTEGER ||
           tokenType == TT::TYPE_DOUBLE ||
           tokenType == TT::TYPE_STRING ||
           tokenType == TT::TYPE_BOOLEAN ||
           tokenType == TT::TYPE_VOID ||
           tokenType == TT::TYPE_NULL ||
           tokenType == TT::TYPE_ARRAY ||
           tokenType == TT::TYPE_MAP;
}

Data::DataType ParserCore::mapTokenTypeToDataType(TokenType tokenType) {
    using TT = TokenType;
    
    switch (tokenType) {
        case TT::TYPE_INTEGER:  return Data::DataType::INTEGER;
        case TT::TYPE_DOUBLE:   return Data::DataType::FLOAT;
        case TT::TYPE_STRING:   return Data::DataType::STRING;
        case TT::TYPE_BOOLEAN:  return Data::DataType::BOOLEAN;
        case TT::TYPE_VOID:     return Data::DataType::NONE;
        case TT::TYPE_NULL:     return Data::DataType::NONE;
        case TT::TYPE_ARRAY:    return Data::DataType::ARRAY;
        case TT::TYPE_MAP:      return Data::DataType::MAP;
        default:                return Data::DataType::UNKNOWN;
    }
}

bool ParserCore::isClassName(const std::string& name) {
    // (AR) التحقق من أن المعرّف هو اسم صنف مسجّل في ClassManager
    // (EN) Check if identifier is registered class name in ClassManager
    
    auto* classManager = Data::ClassManager::getInstance();
    return classManager->hasClass(name);
}

} // namespace Parser
} // namespace Sad
