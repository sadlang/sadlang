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
 * @brief (AR) الحصول على الموقع الحالي (سطر وعمود وإزاحة)
 * @brief (EN) Get current position (line, column, and offset)
 *
 * @return (Position) — (AR) هيكل Position يحتوي على line و column و offset
 *                      (EN) Position struct containing line, column, and offset
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
    return Position(line_, column_, current_, 0);  // length will be set by caller
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
 * @brief (AR) تخطي التعليقات في الكود (# للسطر الواحد، #* *# للمتعدد، ## و #** **# للتوثيق)
 * @brief (EN) Skip comments in code (# for single line, #* *# for multi-line, ## and #** **# for doc)
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
 * ## تعليق توثيقي سطر واحد
 * #** تعليق توثيقي
 *     متعدد الأسطر **#
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: التعليقات التوثيقية لا يتم تخطيها - تُعامل كـ tokens
 * - EN: Doc comments are not skipped - treated as tokens
 * - AR: تعليق السطر الواحد ينتهي بنهاية السطر
 * - EN: Single-line comment ends at line end
 * - AR: التعليق المتعدد يجب أن يُغلق بـ *#
 * - EN: Multi-line comment must be closed with *#
 */
void LexerCore::skipComment() {
    if (peek() != '#') {
        return;
    }
    
    // لا نستهلك # هنا - nextToken سيتعامل معها
    // Don't consume # here - nextToken will handle it
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
    
    // فحص الأرقام بصيغ خاصة (Binary, Octal, Hex)
    // Check for special number formats (0b, 0o, 0x)
    if (peek() == '0' && !isAtEnd()) {
        char nextChar = peekNext();
        
        // Binary: 0b1010
        if (nextChar == 'b' || nextChar == 'B') {
            numStr += advance(); // consume '0'
            numStr += advance(); // consume 'b'
            
            // قراءة الأرقام الثنائية (0,1)
            if (isAtEnd() || (peek() != '0' && peek() != '1')) {
                return makeError("رقم ثنائي بصيغة خاطئة / Invalid binary number format");
            }
            
            while (!isAtEnd() && (peek() == '0' || peek() == '1')) {
                numStr += advance();
            }
            
            DEBUG_PRINT("تم معالجة رقم ثنائي: " + numStr);
            return Token(TokenType::NUMBER_INTEGER, numStr, start_position_);
        }
        
        // Octal: 0o17
        if (nextChar == 'o' || nextChar == 'O') {
            numStr += advance(); // consume '0'
            numStr += advance(); // consume 'o'
            
            // قراءة الأرقام الثمانية (0-7)
            if (isAtEnd() || !isDigit(peek()) || peek() > '7') {
                return makeError("رقم ثماني بصيغة خاطئة / Invalid octal number format");
            }
            
            while (!isAtEnd() && isDigit(peek()) && peek() <= '7') {
                numStr += advance();
            }
            
            DEBUG_PRINT("تم معالجة رقم ثماني: " + numStr);
            return Token(TokenType::NUMBER_INTEGER, numStr, start_position_);
        }
        
        // Hexadecimal: 0xFF
        if (nextChar == 'x' || nextChar == 'X') {
            numStr += advance(); // consume '0'
            numStr += advance(); // consume 'x'
            
            // قراءة الأرقام الست عشرية (0-9, A-F, a-f)
            if (isAtEnd() || !isHexDigit(peek())) {
                return makeError("رقم ست عشري بصيغة خاطئة / Invalid hexadecimal number format");
            }
            
            while (!isAtEnd() && isHexDigit(peek())) {
                numStr += advance();
            }
            
            DEBUG_PRINT("تم معالجة رقم ست عشري: " + numStr);
            return Token(TokenType::NUMBER_INTEGER, numStr, start_position_);
        }
    }
    
    // معالجة الأرقام العادية (عشرية وعشرية عائمة) - مع دعم الأرقام العربية
    // Handle regular numbers (decimal and floating point) - with Arabic digit support
    while (!isAtEnd()) {
        char currentChar = peek();
        unsigned char currentByte = static_cast<unsigned char>(currentChar);
        
        // فحص إذا كان رقماً إنجليزياً (0-9)
        if (isDigit(currentChar)) {
            numStr += advance();
            continue;
        }
        
        // فحص إذا كان رقماً عربياً (UTF-8: 0xD9 متبوعاً بـ 0xA0-0xA9)
        if (currentByte == 0xD9 && current_ + 1 < source_.length()) {
            unsigned char nextByte = static_cast<unsigned char>(source_[current_ + 1]);
            if (StringUtils::isArabicDigit(nextByte)) {
                // تحويل الرقم العربي إلى إنجليزي
                char englishDigit = StringUtils::arabicDigitToEnglish(nextByte);
                numStr += englishDigit;
                
                // تقدم بايتين (UTF-8 للرقم العربي)
                advance(); // 0xD9
                advance(); // 0xA0-0xA9
                continue;
            }
        }
        
        // فحص النقطة العشرية
        if (currentChar == '.') {
            // تحقق من عدم وجود نقطة عشرية مسبقاً
            if (hasDecimalPoint) {
                return makeError("رقم بصيغة خاطئة - نقطة عشرية متكررة / Invalid number format - duplicate decimal point");
            }
            
            // تحقق من وجود رقم بعد النقطة (إنجليزي أو عربي)
            char nextChar = peekNext();
            unsigned char nextByte = static_cast<unsigned char>(nextChar);
            bool hasDigitAfter = isDigit(nextChar);
            
            // فحص إذا كان الرقم التالي عربياً
            if (!hasDigitAfter && nextByte == 0xD9 && current_ + 2 < source_.length()) {
                unsigned char secondNextByte = static_cast<unsigned char>(source_[current_ + 2]);
                hasDigitAfter = StringUtils::isArabicDigit(secondNextByte);
            }
            
            if (!hasDigitAfter) {
                return makeError("رقم بصيغة خاطئة - لا يوجد أرقام بعد النقطة / Invalid number - no digits after decimal point");
            }
            
            hasDecimalPoint = true;
            isDouble = true;
            numStr += advance();
            continue;
        }
        
        // إذا لم يكن رقماً أو نقطة، نتوقف
        break;
    }
    
    // فحص الصيغة العلمية (Scientific notation): 1.5e10, 2e-5
    if (!isAtEnd() && (peek() == 'e' || peek() == 'E')) {
        isDouble = true;
        numStr += advance(); // consume 'e' or 'E'
        
        // فحص إشارة اختيارية (+/-)
        if (!isAtEnd() && (peek() == '+' || peek() == '-')) {
            numStr += advance();
        }
        
        // يجب أن يكون هناك رقم بعد 'e'
        if (isAtEnd() || !isDigit(peek())) {
            return makeError("صيغة علمية خاطئة / Invalid scientific notation");
        }
        
        while (!isAtEnd() && isDigit(peek())) {
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
            // معالجة الحروف الخاصة / Handle escape sequences
            advance(); // تخطي \
            
            if (isAtEnd()) {
                return makeError("نص غير مغلق - انتهى الملف / Unterminated string - EOF reached");
            }
            
            char escaped = peek();
            switch (escaped) {
                case 'n':  advance(); strValue += '\n'; break;
                case 't':  advance(); strValue += '\t'; break;
                case '\\': advance(); strValue += '\\'; break;
                case '"':  advance(); strValue += '"';  break;
                case 'r':  advance(); strValue += '\r'; break;
                case 'b':  advance(); strValue += '\b'; break; // Backspace
                case 'f':  advance(); strValue += '\f'; break; // Form feed
                case 'v':  advance(); strValue += '\v'; break; // Vertical tab
                case '0':  advance(); strValue += '\0'; break; // Null character
                
                // (AR) Unicode 16-bit: \uXXXX (4 digits hex)
                // (EN) Unicode 16-bit: \uXXXX (4 hex digits)
                case 'u': {
                    advance(); // skip 'u'
                    std::string hexCode;
                    for (int i = 0; i < 4; i++) {
                        if (isAtEnd() || !isHexDigit(peek())) {
                            return makeError("Unicode escape sequence غير صحيح / Invalid unicode escape \\u");
                        }
                        hexCode += advance();
                    }
                    // تحويل hex إلى رقم / Convert hex to number
                    int codepoint = std::stoi(hexCode, nullptr, 16);
                    // تحويل إلى UTF-8 / Convert to UTF-8
                    if (codepoint < 0x80) {
                        strValue += static_cast<char>(codepoint);
                    } else if (codepoint < 0x800) {
                        strValue += static_cast<char>(0xC0 | (codepoint >> 6));
                        strValue += static_cast<char>(0x80 | (codepoint & 0x3F));
                    } else {
                        strValue += static_cast<char>(0xE0 | (codepoint >> 12));
                        strValue += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                        strValue += static_cast<char>(0x80 | (codepoint & 0x3F));
                    }
                    DEBUG_PRINT("Unicode escape: \\u" + hexCode + " -> codepoint " + std::to_string(codepoint));
                    break;
                }
                
                // (AR) Unicode 32-bit: \UXXXXXXXX (8 digits hex)
                // (EN) Unicode 32-bit: \UXXXXXXXX (8 hex digits)
                case 'U': {
                    advance(); // skip 'U'
                    std::string hexCode;
                    for (int i = 0; i < 8; i++) {
                        if (isAtEnd() || !isHexDigit(peek())) {
                            return makeError("Unicode escape sequence غير صحيح / Invalid unicode escape \\U");
                        }
                        hexCode += advance();
                    }
                    // تحويل hex إلى رقم / Convert hex to number
                    long codepoint = std::stol(hexCode, nullptr, 16);
                    // تحويل إلى UTF-8 (full range) / Convert to UTF-8 (full range)
                    if (codepoint < 0x80) {
                        strValue += static_cast<char>(codepoint);
                    } else if (codepoint < 0x800) {
                        strValue += static_cast<char>(0xC0 | (codepoint >> 6));
                        strValue += static_cast<char>(0x80 | (codepoint & 0x3F));
                    } else if (codepoint < 0x10000) {
                        strValue += static_cast<char>(0xE0 | (codepoint >> 12));
                        strValue += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                        strValue += static_cast<char>(0x80 | (codepoint & 0x3F));
                    } else {
                        strValue += static_cast<char>(0xF0 | (codepoint >> 18));
                        strValue += static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F));
                        strValue += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                        strValue += static_cast<char>(0x80 | (codepoint & 0x3F));
                    }
                    DEBUG_PRINT("Unicode escape: \\U" + hexCode + " -> codepoint " + std::to_string(codepoint));
                    break;
                }
                
                // (AR) Hex escape: \xHH (1-2 digits hex)
                // (EN) Hex escape: \xHH (1-2 hex digits)
                case 'x': {
                    advance(); // skip 'x'
                    std::string hexCode;
                    // قراءة 1-2 أرقام hex / Read 1-2 hex digits
                    for (int i = 0; i < 2 && !isAtEnd() && isHexDigit(peek()); i++) {
                        hexCode += advance();
                    }
                    if (hexCode.empty()) {
                        return makeError("Hex escape sequence غير صحيح / Invalid hex escape \\x");
                    }
                    int value = std::stoi(hexCode, nullptr, 16);
                    strValue += static_cast<char>(value);
                    DEBUG_PRINT("Hex escape: \\x" + hexCode + " -> " + std::to_string(value));
                    break;
                }
                
                // (AR) Octal escape: \0-377 (1-3 digits octal)
                // (EN) Octal escape: \0-377 (1-3 octal digits)
                default: {
                    if (escaped >= '0' && escaped <= '7') {
                        std::string octalCode;
                        octalCode += advance();
                        // قراءة حتى 2 أرقام إضافية / Read up to 2 more digits
                        for (int i = 0; i < 2 && !isAtEnd() && peek() >= '0' && peek() <= '7'; i++) {
                            octalCode += advance();
                        }
                        int value = std::stoi(octalCode, nullptr, 8);
                        if (value > 255) {
                            return makeError("Octal escape قيمته أكبر من 255 / Octal escape value > 255");
                        }
                        strValue += static_cast<char>(value);
                        DEBUG_PRINT("Octal escape: \\" + octalCode + " -> " + std::to_string(value));
                    } else {
                        // حرف خاص غير معروف - نبقيه كما هو
                        // Unknown escape - keep as is
                        advance();
                        strValue += '\\';
                        strValue += escaped;
                        DEBUG_PRINT("حرف خاص غير معروف: \\" + std::string(1, escaped));
                    }
                    break;
                }
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

/**
 * @brief (AR) معالجة نص خام r"..." - بدون تفسير حروف الهروب
 * @brief (EN) Process raw string r"..." - no escape sequence interpretation
 *
 * @return (Token) — (AR) رمز من نوع STRING_RAW
 *                   (EN) token of type STRING_RAW
 *
 * @throws (std::runtime_error) — (AR) إذا كان النص غير مغلق
 *                                 (EN) if string is not closed
 *
 * مثال الاستخدام / Usage example:
 * @code
 * // الأمثلة المدعومة:
 * r"\n\t" → STRING_RAW (القيمة: "\\n\\t" حرفياً)
 * r"C:\Users\name" → STRING_RAW (بدون تفسير \U و \n)
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: لا يفسر حروف الهروب - كل شيء حرفي
 * - EN: No escape sequence interpretation - everything is literal
 * - AR: مفيد لأسماء الملفات في Windows و regex patterns
 * - EN: Useful for Windows file paths and regex patterns
 */
Token LexerCore::scanRawString(bool skipPrefix) {
    DEBUG_PRINT("بدء معالجة نص خام عند الموقع: " + start_position_.toString());
    
    if (!skipPrefix) {
        advance(); // تخطي 'r' (if not already consumed for Arabic prefix)
    }
    advance(); // تخطي علامة التنصيص الافتتاحية "
    
    std::string strValue;
    
    while (!isAtEnd() && peek() != '"') {
        // (AR) في النصوص الخام، نأخذ كل شيء حرفياً - حتى \ لا يُعامل معاملة خاصة
        // (EN) In raw strings, we take everything literally - even \ is not special
        strValue += advance();
    }
    
    if (isAtEnd()) {
        return makeError("نص خام غير مغلق - انتهى الملف / Unterminated raw string - EOF reached");
    }
    
    advance(); // تخطي علامة التنصيص الختامية "
    
    DEBUG_PRINT("تم معالجة نص خام: r\"" + strValue + "\"");
    return Token(TokenType::STRING_RAW, strValue, start_position_);
}

/**
 * @brief (AR) معالجة نص منسق f"{expr}" - يحتوي على تعبيرات داخل {}
 * @brief (EN) Process formatted string f"{expr}" - contains expressions inside {}
 *
 * @return (Token) — (AR) رمز من نوع STRING_FSTRING
 *                   (EN) token of type STRING_FSTRING
 *
 * @throws (std::runtime_error) — (AR) إذا كان النص غير مغلق أو التعبير غير صحيح
 *                                 (EN) if string is not closed or expression is invalid
 *
 * مثال الاستخدام / Usage example:
 * @code
 * // الأمثلة المدعومة:
 * f"Hello {name}" → STRING_FSTRING
 * f"Result: {x + y}" → STRING_FSTRING
 * f"Multiple {a} and {b}" → STRING_FSTRING
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: يدعم التعبيرات داخل {} لتنسيق النصوص
 * - EN: Supports expressions inside {} for string formatting
 * - AR: يحفظ النص كاملاً مع التعبيرات - Parser سيحللها لاحقاً
 * - EN: Stores entire string with expressions - Parser will analyze later
 * - AR: يدعم escape sequences عادية خارج التعبيرات
 * - EN: Supports normal escape sequences outside expressions
 */
Token LexerCore::scanFString(bool skipPrefix) {
    DEBUG_PRINT("بدء معالجة نص منسق عند الموقع: " + start_position_.toString());
    
    if (!skipPrefix) {
        advance(); // تخطي 'f' (if not already consumed for Arabic prefix)
    }
    advance(); // تخطي علامة التنصيص الافتتاحية "
    
    std::string fullString;
    int braceDepth = 0;
    
    while (!isAtEnd() && (peek() != '"' || braceDepth > 0)) {
        char c = peek();
        
        // (AR) تتبع الأقواس المعقوفة { } لمعرفة متى نكون داخل تعبير
        // (EN) Track curly braces { } to know when we're inside an expression
        if (c == '{') {
            if (peekNext() == '{') {
                // (AR) {{ تعني حرف { حرفي
                // (EN) {{ means literal { character
                fullString += advance(); // first {
                fullString += advance(); // second {
                continue;
            }
            braceDepth++;
            fullString += advance();
            continue;
        }
        
        if (c == '}') {
            if (peekNext() == '}') {
                // (AR) }} تعني حرف } حرفي
                // (EN) }} means literal } character
                fullString += advance(); // first }
                fullString += advance(); // second }
                continue;
            }
            braceDepth--;
            if (braceDepth < 0) {
                return makeError("نص منسق - قوس } بدون قوس { مطابق / F-string - } without matching {");
            }
            fullString += advance();
            continue;
        }
        
        // (AR) معالجة escape sequences خارج التعبيرات فقط
        // (EN) Handle escape sequences outside expressions only
        if (c == '\\' && braceDepth == 0) {
            fullString += advance(); // backslash
            if (!isAtEnd()) {
                fullString += advance(); // الحرف التالي
            }
            continue;
        }
        
        fullString += advance();
    }
    
    if (braceDepth != 0) {
        return makeError("نص منسق - قوس { غير مغلق / F-string - unclosed {");
    }
    
    if (isAtEnd()) {
        return makeError("نص منسق غير مغلق - انتهى الملف / Unterminated f-string - EOF reached");
    }
    
    advance(); // تخطي علامة التنصيص الختامية "
    
    DEBUG_PRINT("تم معالجة نص منسق: f\"" + fullString + "\"");
    return Token(TokenType::STRING_FSTRING, fullString, start_position_);
}

// ======================================================================
// دوال المسح - التعليقات التوثيقية / Scan Functions - Documentation Comments
// ======================================================================

/**
 * @brief (AR) معالجة تعليق توثيقي ## أو #** **#
 * @brief (EN) Process documentation comment ## or #** **#
 *
 * @return (Token) — (AR) رمز من نوع DOC_COMMENT
 *                   (EN) token of type DOC_COMMENT
 *
 * @throws (std::runtime_error) — (AR) إذا كان التعليق التوثيقي متعدد الأسطر غير مغلق
 *                                 (EN) if multi-line doc comment is not closed
 *
 * مثال الاستخدام / Usage example:
 * @code
 * // الأمثلة المدعومة:
 * ## تعليق توثيقي سطر واحد
 * #** تعليق توثيقي
 *     متعدد الأسطر
 *     مع شرح مفصل **#
 * @endcode
 *
 * ملاحظات إضافية / Additional notes:
 * - AR: يُستخدم لتوليد الوثائق تلقائياً
 * - EN: Used for automatic documentation generation
 * - AR: يحفظ المحتوى كاملاً مع المسافات
 * - EN: Preserves full content with whitespace
 */
Token LexerCore::scanDocComment() {
    DEBUG_PRINT("بدء معالجة تعليق توثيقي عند الموقع: " + start_position_.toString());
    
    advance(); // skip first #
    char second = advance(); // should be # or *
    
    std::string docContent;
    
    if (second == '#') {
        // ## (single-line doc comment)
        // (AR) قراءة باقي السطر
        // (EN) Read rest of line
        while (!isAtEnd() && peek() != '\n') {
            docContent += advance();
        }
        
        DEBUG_PRINT("تم معالجة تعليق توثيقي سطر واحد: " + docContent);
        return Token(TokenType::DOC_COMMENT, docContent, start_position_);
    }
    
    // #** **# (multi-line doc comment)
    if (second == '*') {
        if (isAtEnd() || peek() != '*') {
            return makeError("تعليق توثيقي بصيغة خاطئة - يجب أن يبدأ بـ #** / Invalid doc comment - must start with #**");
        }
        advance(); // skip third *
        
        // (AR) قراءة حتى نجد **#
        // (EN) Read until we find **#
        while (!isAtEnd()) {
            if (peek() == '*' && peekNext() == '*' && 
                current_ + 2 < source_.length() && source_[current_ + 2] == '#') {
                // وجدنا النهاية **#
                advance(); // skip first *
                advance(); // skip second *
                advance(); // skip #
                DEBUG_PRINT("تم معالجة تعليق توثيقي متعدد الأسطر");
                return Token(TokenType::DOC_COMMENT, docContent, start_position_);
            }
            docContent += advance();
        }
        
        return makeError("تعليق توثيقي متعدد الأسطر غير مغلق / Unterminated multi-line doc comment");
    }
    
    return makeError("تعليق توثيقي بصيغة خاطئة / Invalid doc comment format");
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
        // (AR) فحص خاص: إذا كان الحرف الحالي 0xD8، تحقق من البايت التالي
        // (EN) Special check: If current byte is 0xD8, check next byte
        // لمنع قراءة ، (0xD8 0x8C) أو ؛ (0xD8 0x9B) كجزء من المعرف
        // To prevent reading ، (0xD8 0x8C) or ؛ (0xD8 0x9B) as part of identifier
        if (static_cast<unsigned char>(peek()) == 0xD8) {
            // تحقق من البايت التالي
            if (current_ + 1 < source_.length()) {
                unsigned char next = static_cast<unsigned char>(source_[current_ + 1]);
                if (next == 0x8C || next == 0x9B) {
                    // هذا فاصلة عربية أو فاصلة منقوطة عربية - لا نقرأها
                    break;
                }
            }
        }
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
    // تخطي المسافات
    skipWhitespace();
    
    // حفظ موقع بداية الرمز
    start_position_ = getCurrentPosition();
    
    // (AR) معالجة التعليقات والتعليقات التوثيقية
    // (EN) Handle comments and doc comments
    if (peek() == '#') {
        // (AR) فحص التعليقات التوثيقية أولاً
        // (EN) Check doc comments first
        char next = peekNext();
        
        // ## (doc comment single line)
        if (next == '#') {
            return scanDocComment();
        }
        
        // #** **# (doc comment multi-line)
        if (next == '*' && current_ + 2 < source_.length() && source_[current_ + 2] == '*') {
            return scanDocComment();
        }
        
        // #* *# (regular multi-line comment)
        if (next == '*') {
            advance(); // skip #
            advance(); // skip *
            
            while (!isAtEnd()) {
                if (peek() == '*' && peekNext() == '#') {
                    advance(); // skip *
                    advance(); // skip #
                    DEBUG_PRINT("نهاية تعليق متعدد الأسطر");
                    return nextToken(); // recurse to get next real token
                }
                advance();
            }
            
            throw std::runtime_error("تعليق متعدد الأسطر غير مغلق - Multi-line comment not closed at " 
                                   + getCurrentPosition().toString());
        }
        
        // # (regular single-line comment)
        advance(); // skip #
        while (!isAtEnd() && peek() != '\n') {
            advance();
        }
        DEBUG_PRINT("تخطي تعليق سطر واحد");
        return nextToken(); // recurse to get next real token
    }
    
    // نهاية الملف
    if (isAtEnd()) {
        DEBUG_PRINT("الوصول لنهاية الملف");
        return Token(TokenType::END_OF_FILE, "", start_position_);
    }
    
    char c = peek();
    unsigned char current_byte = static_cast<unsigned char>(c);
    
    // فحص الأرقام العربية (UTF-8: 0xD9 متبوعاً بـ 0xA0-0xA9)
    // Check for Arabic digits (UTF-8: 0xD9 followed by 0xA0-0xA9)
    if (current_byte == 0xD9 && !isAtEnd() && current_ + 1 < source_.length()) {
        unsigned char next_byte = static_cast<unsigned char>(source_[current_ + 1]);
        if (StringUtils::isArabicDigit(next_byte)) {
            return scanNumber();
        }
    }
    
    // أرقام إنجليزية (0-9)
    // English digits (0-9)
    if (isDigit(c)) {
        return scanNumber();
    }
    
    // نصوص / Strings
    // (AR) فحص النصوص الخاصة قبل النصوص العادية: r"..." (raw), f"..." (f-string)
    // (EN) Check special strings before regular strings: r"..." (raw), f"..." (f-string)
    // (AR) دعم البادئات العربية: ح"..." (raw), م"..." (f-string)
    // (EN) Support Arabic prefixes: ح"..." (raw), م"..." (f-string)
    
    // Raw strings: r"..." or ح"..."
    // (AR) النصوص الخام: r أو ح
    if (c == 'r' && peekNext() == '"') {
        return scanRawString();
    }
    // ح (U+062D) = 0xD8 0xAD in UTF-8 (حاء - raw string prefix)
    if (static_cast<unsigned char>(c) == 0xD8 && (current_ + 1) < source_.length()) {
        unsigned char next = static_cast<unsigned char>(source_[current_ + 1]);
        if (next == 0xAD && (current_ + 2) < source_.length() && source_[current_ + 2] == '"') {
            // Found ح"
            advance(); // consume 0xD8
            advance(); // consume 0xAD
            return scanRawString(true); // skipPrefix = true (already consumed)
        }
    }
    
    // F-strings: f"..." or م"..."
    // (AR) النصوص المنسقة: f أو م
    if (c == 'f' && peekNext() == '"') {
        return scanFString();
    }
    // م (U+0645) = 0xD9 0x85 in UTF-8 (ميم - f-string prefix)
    if (static_cast<unsigned char>(c) == 0xD9 && (current_ + 1) < source_.length()) {
        unsigned char next = static_cast<unsigned char>(source_[current_ + 1]);
        if (next == 0x85 && (current_ + 2) < source_.length() && source_[current_ + 2] == '"') {
            // Found م"
            advance(); // consume 0xD9
            advance(); // consume 0x85
            return scanFString(true); // skipPrefix = true (already consumed)
        }
    }
    
    // Regular strings
    if (c == '"') {
        return scanString();
    }
    
    // (AR) فحص الفاصلة والفاصلة المنقوطة العربية (UTF-8 multi-byte) - يجب أن يكون قبل scanIdentifier!
    // (EN) Check for Arabic comma and semicolon (UTF-8 multi-byte) - MUST be before scanIdentifier!
    // Spec: docs\language_spec\rules\03_oop.md §1
    // ، (U+060C) = 0xD8 0x8C in UTF-8
    // ؛ (U+061B) = 0xD8 0x9B in UTF-8
    if (static_cast<unsigned char>(c) == 0xD8 && (current_ + 1) < source_.length()) {
        unsigned char next = static_cast<unsigned char>(source_[current_ + 1]);
        if (next == 0x8C) {
            // ، Arabic comma
            advance(); // consume 0xD8
            advance(); // consume 0x8C
            return Token(TokenType::ARABIC_COMMA, "،", start_position_);
        } else if (next == 0x9B) {
            // ؛ Arabic semicolon
            advance(); // consume 0xD8
            advance(); // consume 0x9B
            return Token(TokenType::ARABIC_SEMICOLON, "؛", start_position_);
        }
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
        case '?': return Token(TokenType::QUESTION, "?", start_position_); // (AR) للعامل الثلاثي / (EN) for ternary operator
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
 * @brief (AR) التحقق إذا كان الحرف رقماً ست عشرياً (0-9, A-F, a-f)
 * @brief (EN) Check if character is a hexadecimal digit (0-9, A-F, a-f)
 */
bool LexerCore::isHexDigit(char c) const {
    return (c >= '0' && c <= '9') || 
           (c >= 'A' && c <= 'F') || 
           (c >= 'a' && c <= 'f');
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
    // (AR) فحص الأحرف الإنجليزية و _
    // (EN) Check English letters and _
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')) {
        return true;
    }
    
    // (AR) فحص الأحرف العربية (UTF-8 multi-byte)
    // (EN) Check Arabic characters (UTF-8 multi-byte)
    // استبعاد الفاصلة العربية ، (0xD8 0x8C) والفاصلة المنقوطة العربية ؛ (0xD8 0x9B)
    // Exclude Arabic comma (0xD8 0x8C) and Arabic semicolon (0xD8 0x9B)
    unsigned char uc = static_cast<unsigned char>(c);
    if (uc == 0xD8) {
        // نحتاج للتحقق من البايت التالي - لكن isAlpha لا تستطيع الوصول إليه
        // نقبل 0xD8 هنا، وسنتحقق في scanIdentifier من أن التسلسل ليس ، أو ؛
        return true;
    }
    return uc >= 0x80; // UTF-8 للعربية
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

/**
 * @brief (AR) إنشاء رمز مع حساب الطول تلقائياً من الموقع الحالي
 * @brief (EN) Create token with automatic length calculation from current position
 * 
 * @details (AR) تحسب الطول = current_ - start_position_.offset
 *               ثم تُنشئ Position جديد مع الطول المحسوب
 *          (EN) Calculates length = current_ - start_position_.offset
 *               Then creates new Position with calculated length
 * 
 * @param type (TokenType) — (AR) نوع الرمز / (EN) token type
 * @param value (std::string) — (AR) قيمة الرمز / (EN) token value
 * @return (Token) — (AR) الرمز مع الموقع والطول / (EN) token with position and length
 * 
 * مثال:
 * // عند المعالجة: start_position_.offset = 10, current_ = 15
 * return makeToken(TokenType::IDENTIFIER, "hello"); // length = 5
 */
Token LexerCore::makeToken(TokenType type, const std::string& value) {
    size_t length = current_ - start_position_.offset;
    Position pos(start_position_.line, start_position_.column, 
                 start_position_.offset, length);
    return Token(type, value, pos);
}

} // namespace Lexer
} // namespace Sad
