// ======================================================================
// lexer_core.cpp - تنفيذ المحلل المعجمي الأساسي / Lexer Core Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ كامل للمحلل المعجمي الذي يحول النص المصدري إلى رموز
//   يدعم الأرقام، النصوص، المعرفات العربية واللاتينية، والعوامل
//   يتخطى المسافات والتعليقات، ويتتبع المواقع بدقة
//
// English Description:
//   Complete lexer implementation that converts source code to tokens
//   Supports numbers, strings, Arabic/Latin identifiers, and operators
//   Skips whitespace and comments, tracks positions accurately
//
// ملاحظات / Notes:
//   - يستخدم UTF-8 للنصوص العربية / Uses UTF-8 for Arabic text
//   - يدعم التعليقات # و #* *# / Supports # and #* *# comments
//   - يحفظ موقع كل رمز للأخطاء الدقيقة / Stores token positions for precise errors
// ======================================================================

#include "../../include/lexer/lexer_core.h"
#include "../../include/utils/string_utils.h"
#include <cctype>
#include <stdexcept>
#include <sstream>

using namespace Sad::Utils;

namespace Sad {
namespace Lexer {

// ======================================================================
// المنشئ / Constructor
// ======================================================================

/**
 * @brief (AR) منشئ المحلل المعجمي - يهيئ الحالة ويستعد للمعالجة
 * @brief (EN) Lexer constructor - initializes state and prepares for processing
 *
 * @param source (const std::string&) — (AR) النص المصدري الكامل للبرنامج
 *                                        (EN) complete source code of the program
 *
 * مثال الاستخدام / Usage example:
 * @code
 * LexerCore lexer("رقم س = 42");
 * auto tokens = lexer.tokenize();
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: يهيئ جدول الكلمات المفتاحية تلقائياً
 * - EN: Automatically initializes keyword table
 * - AR: يبدأ من السطر 1 العمود 1
 * - EN: Starts at line 1, column 1
 */
LexerCore::LexerCore(const std::string& source)
    : source_(source)
    , current_(0)
    , line_(1)
    , column_(1)
    , start_position_(1, 1)
{
    DEBUG_PRINT("إنشاء محلل معجمي جديد - حجم المصدر: " + std::to_string(source_.length()) + " حرف");
    
    // تهيئة جدول الكلمات المفتاحية
    KeywordTable::initialize();
    DEBUG_PRINT("تم تهيئة جدول الكلمات المفتاحية - عدد الكلمات: " + std::to_string(KeywordTable::getAllKeywords().size()));
}

// ======================================================================
// دوال الملاحة والحالة / Navigation and State Functions
// ======================================================================

/**
 * @brief (AR) النظر إلى الحرف الحالي دون تحريك المؤشر
 * @brief (EN) Look at current character without moving pointer
 *
 * @return (char) — (AR) الحرف الحالي، أو '\0' إذا وصلنا للنهاية
 *                  (EN) current character, or '\0' if at end
 *
 * مثال الاستخدام / Usage example:
 * @code
 * if (peek() == '=') {
 *     advance(); // تقدم فقط إذا كان '='
 * }
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: آمن - لا يسبب خطأ عند تجاوز النهاية
 * - EN: Safe - doesn't cause error when past end
 * - AR: يُستخدم للنظر قبل اتخاذ القرار
 * - EN: Used to lookahead before making decision
 */
char LexerCore::peek() const {
    if (isAtEnd()) {
        return '\0';
    }
    return source_[current_];
}

/**
 * @brief (AR) النظر إلى حرف متقدم بمقدار offset من الموقع الحالي
 * @brief (EN) Look at character offset positions ahead from current position
 *
 * @param offset (size_t) — (AR) عدد الأحرف للنظر إلى الأمام (افتراضي: 1)
 *                          (EN) number of characters to look ahead (default: 1)
 *
 * @return (char) — (AR) الحرف المطلوب، أو '\0' إذا تجاوز النهاية
 *                  (EN) requested character, or '\0' if beyond end
 *
 * مثال الاستخدام / Usage example:
 * @code
 * if (peek() == '=' && peekNext() == '=') {
 *     // معالجة العامل ==
 * }
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: مفيد للتمييز بين العوامل (مثل = و ==)
 * - EN: Useful for distinguishing operators (like = and ==)
 */
char LexerCore::peekNext(size_t offset) const {
    size_t pos = current_ + offset;
    if (pos >= source_.length()) {
        return '\0';
    }
    return source_[pos];
}

/**
 * @brief (AR) التقدم حرف واحد للأمام وإرجاع الحرف المقروء
 * @brief (EN) Advance one character forward and return the read character
 *
 * @return (char) — (AR) الحرف الذي تم قراءته
 *                  (EN) character that was read
 *
 * مثال الاستخدام / Usage example:
 * @code
 * char c = advance(); // يتقدم ويحفظ الحرف
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: يحدّث line_ و column_ تلقائياً عند رؤية سطر جديد
 * - EN: Automatically updates line_ and column_ when seeing newline
 * - AR: يزيد column_ بمقدار 1 لكل حرف عادي
 * - EN: Increments column_ by 1 for each normal character
 */
char LexerCore::advance() {
    if (isAtEnd()) {
        return '\0';
    }
    
    char c = source_[current_++];
    
    // تحديث الموقع
    if (c == '\n') {
        line_++;
        column_ = 1;
        DEBUG_PRINT("سطر جديد - السطر الحالي: " + std::to_string(line_));
    } else {
        column_++;
    }
    
    return c;
}

/**
 * @brief (AR) التحقق من الوصول إلى نهاية النص المصدري
 * @brief (EN) Check if reached end of source code
 *
 * @return (bool) — (AR) true إذا وصلنا للنهاية، false إذا ما زال هناك أحرف
 *                  (EN) true if at end, false if more characters remain
 *
 * مثال الاستخدام / Usage example:
 * @code
 * while (!isAtEnd()) {
 *     Token token = nextToken();
 * }
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: تُستخدم في الحلقة الرئيسية للمحلل
 * - EN: Used in main lexer loop
 */
bool LexerCore::isAtEnd() const {
    return current_ >= source_.length();
}

/**
 * @brief (AR) الحصول على الموقع الحالي (سطر وعمود)
 * @brief (EN) Get current position (line and column)
 *
 * @return (Position) — (AR) هيكل Position يحتوي على line و column
 *                      (EN) Position struct containing line and column
 *
 * مثال الاستخدام / Usage example:
 * @code
 * Position pos = getCurrentPosition();
 * std::cout << "الموقع: " << pos.toString() << std::endl;
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: يُستخدم عند إنشاء رمز جديد لحفظ موقعه
 * - EN: Used when creating new token to save its position
 */
Position LexerCore::getCurrentPosition() const {
    return Position(line_, column_);
}

// ======================================================================
// دوال التخطي / Skip Functions
// ======================================================================

/**
 * @brief (AR) تخطي جميع المسافات البيضاء (مسافة، تاب، سطر جديد، إلخ)
 * @brief (EN) Skip all whitespace characters (space, tab, newline, etc.)
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: يتخطى ' ', '\t', '\n', '\r'
 * - EN: Skips ' ', '\t', '\n', '\r'
 * - AR: يتم استدعاؤها تلقائياً قبل معالجة كل رمز
 * - EN: Called automatically before processing each token
 */
void LexerCore::skipWhitespace() {
    while (!isAtEnd() && isWhitespace(peek())) {
        advance();
    }
}

/**
 * @brief (AR) تخطي التعليقات في الكود (# للسطر الواحد، #* *# للمتعدد)
 * @brief (EN) Skip comments in code (# for single line, #* *# for multi-line)
 *
 * @throws (std::runtime_error) — (AR) إذا كان تعليق متعدد الأسطر غير مغلق
 *                                 (EN) if multi-line comment is not closed
 *
 * مثال الاستخدام / Usage example:
 * @code
 * // في الكود:
 * # هذا تعليق سطر واحد
 * #* هذا تعليق
 *    متعدد الأسطر *#
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: تعليق السطر الواحد ينتهي بنهاية السطر
 * - EN: Single-line comment ends at line end
 * - AR: التعليق المتعدد يجب أن يُغلق بـ *#
 * - EN: Multi-line comment must be closed with *#
 */
void LexerCore::skipComment() {
    if (peek() != '#') {
        return;
    }
    
    advance(); // تخطي #
    
    // تحقق من نوع التعليق
    if (peek() == '*') {
        // تعليق متعدد الأسطر #* ... *#
        DEBUG_PRINT("بدء تعليق متعدد الأسطر");
        advance(); // تخطي *
        
        while (!isAtEnd()) {
            if (peek() == '*' && peekNext() == '#') {
                advance(); // تخطي *
                advance(); // تخطي #
                DEBUG_PRINT("نهاية تعليق متعدد الأسطر");
                return;
            }
            advance();
        }
        
        // إذا وصلنا هنا، التعليق لم يُغلق
        throw std::runtime_error("تعليق متعدد الأسطر غير مغلق - Multi-line comment not closed at " 
                               + getCurrentPosition().toString());
    } else {
        // تعليق سطر واحد # ... \n
        DEBUG_PRINT("تخطي تعليق سطر واحد");
        while (!isAtEnd() && peek() != '\n') {
            advance();
        }
    }
}

// ======================================================================
// دوال المسح - الأرقام / Scan Functions - Numbers
// ======================================================================

/**
 * @brief (AR) معالجة رقم صحيح أو عشري (مع دعم الأرقام السالبة)
 * @brief (EN) Process integer or decimal number (with negative number support)
 *
 * @return (Token) — (AR) رمز من نوع INTEGER أو DOUBLE
 *                   (EN) token of type INTEGER or DOUBLE
 *
 * @throws (std::runtime_error) — (AR) إذا كان الرقم بصيغة خاطئة
 *                                 (EN) if number has invalid format
 *
 * مثال الاستخدام / Usage example:
 * @code
 * // الأمثلة المدعومة:
 * // 42 → INTEGER
 * // -17 → INTEGER
 * // 3.14 → DOUBLE
 * // 0.5 → DOUBLE
 * // -2.71 → DOUBLE
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: يدعم نقطة عشرية واحدة فقط
 * - EN: Supports only one decimal point
 * - AR: يكشف الأرقام بصيغة خاطئة مثل 3.14.15
 * - EN: Detects invalid formats like 3.14.15
 */
Token LexerCore::scanNumber() {
    DEBUG_PRINT("بدء معالجة رقم عند الموقع: " + start_position_.toString());
    
    std::string numStr;
    bool isDouble = false;
    bool hasDecimalPoint = false;
    
    // معالجة الأرقام
    while (!isAtEnd() && (isDigit(peek()) || peek() == '.')) {
        if (peek() == '.') {
            // تحقق من عدم وجود نقطة عشرية مسبقاً
            if (hasDecimalPoint) {
                return makeError("رقم بصيغة خاطئة - نقطة عشرية متكررة / Invalid number format - duplicate decimal point");
            }
            
            // تحقق من وجود رقم بعد النقطة
            if (!isDigit(peekNext())) {
                return makeError("رقم بصيغة خاطئة - لا يوجد أرقام بعد النقطة / Invalid number - no digits after decimal point");
            }
            
            hasDecimalPoint = true;
            isDouble = true;
            numStr += advance();
        } else {
            numStr += advance();
        }
    }
    
    TokenType type = isDouble ? TokenType::NUMBER_DOUBLE : TokenType::NUMBER_INTEGER;
    DEBUG_PRINT("تم معالجة رقم: " + numStr + " - النوع: " + (isDouble ? "عشري" : "صحيح"));
    
    return Token(type, numStr, start_position_);
}

// ======================================================================
// دوال المسح - النصوص / Scan Functions - Strings
// ======================================================================

/**
 * @brief (AR) معالجة نص محاط بعلامات تنصيص مزدوجة "..."
 * @brief (EN) Process string enclosed in double quotes "..."
 *
 * @return (Token) — (AR) رمز من نوع LITERAL_STRING
 *                   (EN) token of type LITERAL_STRING
 *
 * @throws (std::runtime_error) — (AR) إذا كان النص غير مغلق
 *                                 (EN) if string is not closed
 *
 * مثال الاستخدام / Usage example:
 * @code
 * // الأمثلة المدعومة:
 * "مرحباً" → STRING
 * "سطر جديد: \n" → STRING (مع حرف خاص)
 * "نص متعدد
 *  الأسطر" → STRING (يدعم أسطر متعددة)
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: يدعم الحروف الخاصة: \n \t \\ \"
 * - EN: Supports escape sequences: \n \t \\ \"
 * - AR: يدعم النصوص متعددة الأسطر
 * - EN: Supports multi-line strings
 */
Token LexerCore::scanString() {
    DEBUG_PRINT("بدء معالجة نص عند الموقع: " + start_position_.toString());
    
    advance(); // تخطي علامة التنصيص الافتتاحية "
    
    std::string strValue;
    
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\') {
            // معالجة الحروف الخاصة
            advance(); // تخطي \
            
            if (isAtEnd()) {
                return makeError("نص غير مغلق - انتهى الملف / Unterminated string - EOF reached");
            }
            
            char escaped = advance();
            switch (escaped) {
                case 'n':  strValue += '\n'; break;
                case 't':  strValue += '\t'; break;
                case '\\': strValue += '\\'; break;
                case '"':  strValue += '"';  break;
                case 'r':  strValue += '\r'; break;
                default:
                    // حرف خاص غير معروف - نبقيه كما هو
                    strValue += '\\';
                    strValue += escaped;
                    DEBUG_PRINT("حرف خاص غير معروف: \\" + std::string(1, escaped));
            }
        } else {
            strValue += advance();
        }
    }
    
    if (isAtEnd()) {
        return makeError("نص غير مغلق - انتهى الملف / Unterminated string - EOF reached");
    }
    
    advance(); // تخطي علامة التنصيص الختامية "
    
    DEBUG_PRINT("تم معالجة نص: \"" + strValue + "\"");
    return Token(TokenType::STRING_LITERAL, strValue, start_position_);
}

// ======================================================================
// دوال المسح - المعرفات والكلمات المفتاحية / Scan Functions - Identifiers & Keywords
// ======================================================================

/**
 * @brief (AR) معالجة معرّف أو كلمة مفتاحية (دعم عربي ولاتيني)
 * @brief (EN) Process identifier or keyword (Arabic and Latin support)
 *
 * @return (Token) — (AR) رمز من نوع IDENTIFIER أو KEYWORD_*
 *                   (EN) token of type IDENTIFIER or KEYWORD_*
 *
 * مثال الاستخدام / Usage example:
 * @code
 * // أمثلة:
 * س → IDENTIFIER
 * count → IDENTIFIER
 * دالة → KEYWORD_FUNCTION
 * إذا → KEYWORD_IF
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: يبدأ بحرف أو _ ثم يتبعه أحرف أو أرقام أو _
 * - EN: Starts with letter or _ then followed by letters, digits, or _
 * - AR: يدعم الحروف العربية بالكامل (UTF-8)
 * - EN: Full Arabic character support (UTF-8)
 * - AR: يتحقق من جدول الكلمات المفتاحية تلقائياً
 * - EN: Automatically checks keyword table
 */
Token LexerCore::scanIdentifier() {
    DEBUG_PRINT("بدء معالجة معرف عند الموقع: " + start_position_.toString());
    
    std::string identifier;
    
    // الحرف الأول (حرف أو _)
    identifier += advance();
    
    // الأحرف التالية (حرف، رقم، أو _)
    while (!isAtEnd() && (isAlphaNumeric(peek()) || peek() == '_')) {
        identifier += advance();
    }
    
    DEBUG_PRINT("تم قراءة معرف: " + identifier);
    
    // تحقق إذا كانت كلمة مفتاحية
    if (KeywordTable::isKeyword(identifier)) {
        TokenType keywordType = KeywordTable::getKeywordType(identifier);
        DEBUG_PRINT("المعرف هو كلمة مفتاحية: " + identifier);
        return Token(keywordType, identifier, start_position_);
    }
    
    // معرف عادي
    DEBUG_PRINT("المعرف هو معرف عادي: " + identifier);
    return Token(TokenType::IDENTIFIER, identifier, start_position_);
}

// ======================================================================
// دوال المسح - العوامل / Scan Functions - Operators
// ======================================================================

/**
 * @brief (AR) معالجة عامل رياضي أو منطقي (أحادي أو ثنائي الحرف)
 * @brief (EN) Process mathematical or logical operator (single or double character)
 *
 * @return (Token) — (AR) رمز من نوع OPERATOR_*
 *                   (EN) token of type OPERATOR_*
 *
 * مثال الاستخدام / Usage example:
 * @code
 * // أمثلة العوامل:
 * + → OPERATOR_PLUS
 * += → OPERATOR_PLUS_ASSIGN
 * == → OPERATOR_EQUAL
 * != → OPERATOR_NOT_EQUAL
 * -> → OPERATOR_ARROW
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: يتحقق من العوامل الثنائية أولاً (==، !=، إلخ)
 * - EN: Checks double-character operators first (==, !=, etc.)
 * - AR: يرجع خطأ للرموز غير المعروفة
 * - EN: Returns error for unknown symbols
 */
Token LexerCore::scanOperator() {
    char c = peek();
    char next = peekNext();
    
    DEBUG_PRINT("بدء معالجة عامل: " + std::string(1, c));
    
    // عوامل ثنائية الحرف - تحقق أولاً
    if (c == '=' && next == '=') {
        advance(); advance();
        return Token(TokenType::OP_EQUAL, "==", start_position_);
    }
    if (c == '!' && next == '=') {
        advance(); advance();
        return Token(TokenType::OP_NOT_EQUAL, "!=", start_position_);
    }
    if (c == '<' && next == '=') {
        advance(); advance();
        return Token(TokenType::OP_LESS_EQUAL, "<=", start_position_);
    }
    if (c == '>' && next == '=') {
        advance(); advance();
        return Token(TokenType::OP_GREATER_EQUAL, ">=", start_position_);
    }
    if (c == '+' && next == '=') {
        advance(); advance();
        return Token(TokenType::OP_PLUS_ASSIGN, "+=", start_position_);
    }
    if (c == '-' && next == '=') {
        advance(); advance();
        return Token(TokenType::OP_MINUS_ASSIGN, "-=", start_position_);
    }
    if (c == '*' && next == '=') {
        advance(); advance();
        return Token(TokenType::OP_MULTIPLY_ASSIGN, "*=", start_position_);
    }
    if (c == '*' && next == '*') {
        advance(); advance();
        return Token(TokenType::OP_POWER, "**", start_position_);
    }
    if (c == '/' && next == '=') {
        advance(); advance();
        return Token(TokenType::OP_DIVIDE_ASSIGN, "/=", start_position_);
    }
    if (c == '+' && next == '+') {
        advance(); advance();
        return Token(TokenType::OP_INCREMENT, "++", start_position_);
    }
    if (c == '-' && next == '-') {
        advance(); advance();
        return Token(TokenType::OP_DECREMENT, "--", start_position_);
    }
    if (c == '&' && next == '&') {
        advance(); advance();
        return Token(TokenType::OP_AND, "&&", start_position_);
    }
    if (c == '|' && next == '|') {
        advance(); advance();
        return Token(TokenType::OP_OR, "||", start_position_);
    }
    if (c == '-' && next == '>') {
        advance(); advance();
        return Token(TokenType::ARROW, "->", start_position_);
    }
    if (c == '=' && next == '>') {
        advance(); advance();
        return Token(TokenType::FAT_ARROW, "=>", start_position_);
    }
    
    // عوامل أحادية الحرف
    advance();
    switch (c) {
        case '+': return Token(TokenType::OP_PLUS, "+", start_position_);
        case '-': return Token(TokenType::OP_MINUS, "-", start_position_);
        case '*': return Token(TokenType::OP_MULTIPLY, "*", start_position_);
        case '/': return Token(TokenType::OP_DIVIDE, "/", start_position_);
        case '%': return Token(TokenType::OP_MODULO, "%", start_position_);
        case '=': return Token(TokenType::OP_ASSIGN, "=", start_position_);
        case '<': return Token(TokenType::OP_LESS, "<", start_position_);
        case '>': return Token(TokenType::OP_GREATER, ">", start_position_);
        case '!': return Token(TokenType::OP_NOT, "!", start_position_);
        case '.': return Token(TokenType::DOT, ".", start_position_);
        default:
            return makeError("عامل غير معروف / Unknown operator: " + std::string(1, c));
    }
}

// ======================================================================
// الدوال الرئيسية - nextToken و tokenize / Main Functions
// ======================================================================

/**
 * @brief (AR) الحصول على الرمز التالي من النص المصدري
 * @brief (EN) Get next token from source code
 *
 * @return (Token) — (AR) الرمز التالي (أو EOF عند النهاية أو ERROR عند خطأ)
 *                   (EN) next token (or EOF at end or ERROR on error)
 *
 * مثال الاستخدام / Usage example:
 * @code
 * LexerCore lexer("رقم س = 5");
 * while (true) {
 *     Token token = lexer.nextToken();
 *     if (token.getType() == TokenType::END_OF_FILE) break;
 *     // معالجة الرمز
 * }
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: تتخطى المسافات والتعليقات تلقائياً
 * - EN: Automatically skips whitespace and comments
 * - AR: تحفظ الموقع الدقيق لكل رمز
 * - EN: Stores precise position for each token
 */
Token LexerCore::nextToken() {
    // تخطي المسافات والتعليقات
    while (!isAtEnd()) {
        skipWhitespace();
        
        if (peek() == '#') {
            skipComment();
        } else {
            break;
        }
    }
    
    // حفظ موقع بداية الرمز
    start_position_ = getCurrentPosition();
    
    // نهاية الملف
    if (isAtEnd()) {
        DEBUG_PRINT("الوصول لنهاية الملف");
        return Token(TokenType::END_OF_FILE, "", start_position_);
    }
    
    char c = peek();
    
    // أرقام
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // نصوص
    if (c == '"') {
        return scanString();
    }
    
    // معرفات وكلمات مفتاحية
    if (isAlpha(c) || c == '_') {
        return scanIdentifier();
    }
    
    // علامات الترقيم
    advance();
    switch (c) {
        case '(': return Token(TokenType::PAREN_LEFT, "(", start_position_);
        case ')': return Token(TokenType::PAREN_RIGHT, ")", start_position_);
        case '{': return Token(TokenType::BRACE_LEFT, "{", start_position_);
        case '}': return Token(TokenType::BRACE_RIGHT, "}", start_position_);
        case '[': return Token(TokenType::BRACKET_LEFT, "[", start_position_);
        case ']': return Token(TokenType::BRACKET_RIGHT, "]", start_position_);
        case ',': return Token(TokenType::COMMA, ",", start_position_);
        case ';': return Token(TokenType::SEMICOLON, ";", start_position_);
        case ':': return Token(TokenType::COLON, ":", start_position_);
        case '@': return Token(TokenType::AT_SIGN, "@", start_position_); // (AR) للمُزخرِفات / (EN) for decorators
    }
    
    // عوامل (نعيد الحرف للخلف) / Operators (backtrack character)
    current_--;
    column_--;
    return scanOperator();
}

/**
 * @brief (AR) تحويل النص المصدري بالكامل إلى قائمة من الرموز
 * @brief (EN) Convert entire source code to list of tokens
 *
 * @return (std::vector<Token>) — (AR) قائمة جميع الرموز (بما في ذلك EOF في النهاية)
 *                                 (EN) list of all tokens (including EOF at end)
 *
 * @throws (std::runtime_error) — (AR) عند وجود أخطاء معجمية
 *                                 (EN) on lexical errors
 *
 * مثال الاستخدام / Usage example:
 * @code
 * LexerCore lexer("رقم س = 42");
 * std::vector<Token> tokens = lexer.tokenize();
 * for (const auto& token : tokens) {
 *     std::cout << token.toString() << std::endl;
 * }
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: ترمي استثناء عند أول خطأ معجمي
 * - EN: Throws exception on first lexical error
 * - AR: تُضيف رمز EOF في نهاية القائمة
 * - EN: Adds EOF token at end of list
 */
std::vector<Token> LexerCore::tokenize() {
    DEBUG_PRINT("======= بدء عملية التحليل المعجمي الكامل =======");
    DEBUG_PRINT("حجم النص المصدري: " + std::to_string(source_.length()) + " حرف");
    
    std::vector<Token> tokens;
    
    while (!isAtEnd()) {
        Token token = nextToken();
        
        // تحقق من الأخطاء
        if (token.getType() == TokenType::INVALID) {
            DEBUG_PRINT("خطأ معجمي: " + token.getValue());
            throw std::runtime_error("خطأ معجمي / Lexical error at " + token.getPosition().toString() 
                                   + ": " + token.getValue());
        }
        
        tokens.push_back(token);
        
        // إذا وصلنا لـ EOF، توقف
        if (token.getType() == TokenType::END_OF_FILE) {
            break;
        }
    }
    
    DEBUG_PRINT("======= انتهت عملية التحليل المعجمي =======");
    DEBUG_PRINT("عدد الرموز المُستخرجة: " + std::to_string(tokens.size()));
    
    return tokens;
}

// ======================================================================
// الدوال المساعدة الخاصة / Private Helper Functions
// ======================================================================

/**
 * @brief (AR) التحقق إذا كان الحرف رقماً (0-9)
 * @brief (EN) Check if character is a digit (0-9)
 */
bool LexerCore::isDigit(char c) const {
    return c >= '0' && c <= '9';
}

/**
 * @brief (AR) التحقق إذا كان الحرف حرفاً أبجدياً (عربي أو لاتيني أو _)
 * @brief (EN) Check if character is alphabetic (Arabic, Latin, or _)
 *
 * ملاحظات / Notes:
 * - AR: يدعم الحروف العربية (UTF-8 نطاق 0x80-0xFF)
 * - EN: Supports Arabic characters (UTF-8 range 0x80-0xFF)
 * - AR: يدعم A-Z و a-z و _
 * - EN: Supports A-Z and a-z and _
 */
bool LexerCore::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c == '_') ||
           (static_cast<unsigned char>(c) >= 0x80); // UTF-8 للعربية
}

/**
 * @brief (AR) التحقق إذا كان الحرف حرفاً أو رقماً
 * @brief (EN) Check if character is alphanumeric
 */
bool LexerCore::isAlphaNumeric(char c) const {
    return isAlpha(c) || isDigit(c);
}

/**
 * @brief (AR) التحقق إذا كان الحرف مسافة بيضاء
 * @brief (EN) Check if character is whitespace
 */
bool LexerCore::isWhitespace(char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/**
 * @brief (AR) إنشاء رمز خطأ مع رسالة
 * @brief (EN) Create error token with message
 *
 * @param message (const std::string&) — (AR) رسالة الخطأ
 *                                        (EN) error message
 *
 * @return (Token) — (AR) رمز من نوع ERROR
 *                   (EN) token of type ERROR
 */
Token LexerCore::makeError(const std::string& message) {
    DEBUG_PRINT("إنشاء رمز خطأ: " + message);
    return Token(TokenType::INVALID, message, start_position_);
}

} // namespace Lexer
} // namespace Sad
