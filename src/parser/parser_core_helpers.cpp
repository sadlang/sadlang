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
    
    // Skip whitespace and comments in both current and nextToken_
    // (AR) تجاوز المسافات والتعليقات في كل من current و nextToken_
    while (current_.getType() == TT::WHITESPACE || 
           current_.getType() == TT::COMMENT ||
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
    bool result = current_.getType() == TT::END_OF_FILE;
    if (result) {
        std::cout << "[parser_core_helpers.cpp] isAtEnd() = true - current token type: " 
                  << static_cast<int>(current_.getType()) << "\n";
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
 * @brief (AR) يحلل قائمة معاملات مكتوبة: (x: int, y: float = 10).
 *        (EN) Parses typed parameter list: (x: int, y: float = 10).
 */
std::vector<Parameter> ParserCore::parseTypedParameterList() {
    std::vector<Parameter> parameters;
    
    // Parse parameters
    // (AR) تحليل المعاملات
    if (!check(TT::PAREN_RIGHT)) {
        do {
            // Parse parameter name
            // (AR) تحليل اسم المعامل
            Token paramName = consume(TT::IDENTIFIER, 
                "(AR) توقع اسم معامل. (EN) Expected parameter name.");
            
            // Optional type annotation: name : type
            // (AR) تصريح النوع الاختياري: اسم : نوع
            Data::DataType paramType = Data::DataType::UNKNOWN;
            if (match(TT::COLON)) {
                paramType = parseType();
            }
            
            // Optional default value: name : type = value
            // (AR) القيمة الافتراضية الاختيارية: اسم : نوع = قيمة
            ExprPtr defaultValue = nullptr;
            if (match(TT::OP_ASSIGN)) {
                defaultValue = parseExpression();
            }
            
            // Create parameter object
            // (AR) إنشاء كائن المعامل
            parameters.emplace_back(
                paramName.getValue(),
                paramType,
                std::move(defaultValue)
            );
            
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

} // namespace Parser
} // namespace Sad
