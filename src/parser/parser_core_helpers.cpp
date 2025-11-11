/**
 * @file parser_core_helpers.cpp
 * @brief (AR) دوال مساعدة للمحلل النحوي - الجزء الثاني من التنفيذ.
 *        (EN) Parser helper functions - second part of implementation.
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
    
    // Create dict comprehension node
    // (AR) إنشاء عقدة Dict Comprehension
    return std::make_unique<DictComprehensionExpr>(
        std::move(keyExpr),
        std::move(valueExpr),
        keyVar.getValue(),
        valueVar.getValue(),
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
    current_ = lexer_.nextToken();
    
    // Skip whitespace and comments
    // (AR) تجاوز المسافات والتعليقات
    while (current_.getType() == TT::WHITESPACE || 
           current_.getType() == TT::COMMENT ||
           current_.getType() == TT::NEWLINE) {
        current_ = lexer_.nextToken();
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
    
    error(message);
    // Return dummy token to allow continuation
    // (AR) إرجاع رمز وهمي للسماح بالاستمرار
    return current_;
}

/**
 * @brief (AR) يتحقق من الوصول لنهاية الملف.
 *        (EN) Checks if reached end of file.
 */
bool ParserCore::isAtEnd() const {
    return current_.getType() == TT::END_OF_FILE;
}

/**
 * @brief (AR) يرجع الرمز الحالي.
 *        (EN) Returns current token.
 */
const Token& ParserCore::peek() const {
    return current_;
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
void ParserCore::error(const std::string& message) {
    if (panicMode_) return;  // Avoid error cascades
    
    panicMode_ = true;
    
    // Build error message with location
    // (AR) بناء رسالة الخطأ مع الموقع
    std::stringstream ss;
    ss << "[Line " << current_.getPosition().line 
       << ", Col " << current_.getPosition().column << "] "
       << message;
    
    errors_.push_back(ss.str());
}

/**
 * @brief (AR) يحاول التعافي من خطأ بالتقدم للجملة التالية.
 *        (EN) Attempts error recovery by advancing to next statement.
 */
void ParserCore::synchronize() {
    panicMode_ = false;
    
    advance();
    
    // Skip until statement boundary
    // (AR) التجاوز حتى حدود الجملة
    while (!isAtEnd()) {
        if (previous().getType() == TT::SEMICOLON) return;
        
        // Stop at statement keywords
        // (AR) التوقف عند كلمات الجمل المفتاحية
        switch (current_.getType()) {
            case TT::KEYWORD_CLASS:
            case TT::KEYWORD_FUNCTION:
            case TT::KEYWORD_VAR:
            case TT::KEYWORD_FOR:
            case TT::KEYWORD_IF:
            case TT::KEYWORD_WHILE:
            case TT::KEYWORD_RETURN:
                return;
            default:
                break;
        }
        
        advance();
    }
}

/**
 * @brief (AR) يحلل قائمة معاملات دالة: (x, y, z).
 *        (EN) Parses function parameter list: (x, y, z).
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
        } while (match(TT::COMMA));
    }
    
    return parameters;
}

/**
 * @brief (AR) يحلل قائمة وسائط استدعاء: f(1, 2, 3).
 *        (EN) Parses function call argument list: f(1, 2, 3).
 */
ExprList ParserCore::parseArgumentList() {
    ExprList arguments;
    
    // Parse arguments
    // (AR) تحليل الوسائط
    if (!check(TT::PAREN_RIGHT)) {
        do {
            arguments.push_back(parseExpression());
        } while (match(TT::COMMA));
    }
    
    return arguments;
}

} // namespace Parser
} // namespace Sad
