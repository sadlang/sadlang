// ======================================================================
// token.h - فئة الرمز / Token Class
// ======================================================================
// الوصف بالعربية:
//   تعريف أنواع الرموز (TokenType) وفئة الرمز (Token) ومعلومات الموقع (Position)
//   يستخدم المحلل المعجمي (Lexer) هذه الفئات لتمثيل الرموز المستخرجة من النص المصدري
//
// English Description:
//   Definition of token types (TokenType), Token class, and Position information
//   The Lexer uses these classes to represent tokens extracted from source code
//
// ملاحظات / Notes:
//   - جميع الكلمات المفتاحية العربية ممثلة في TokenType
//     All Arabic keywords are represented in TokenType
//   - Position يحفظ رقم السطر والعمود لرسائل الأخطاء الدقيقة
//     Position stores line and column for precise error messages
//   - Token غير قابل للتغيير بعد الإنشاء (immutable)
//     Token is immutable after creation
// ======================================================================

#pragma once

#include <string>
#include <memory>

namespace Sad {
namespace Lexer {

// ======================================================================
// هيكل الموقع / Position Structure
// ======================================================================
/**
 * @brief (AR) يمثل موقع الرمز في ملف المصدر (رقم السطر والعمود)
 * @brief (EN) Represents token position in source file (line and column number)
 * 
 * @struct Position
 * 
 * ملاحظات إضافية:
 * - AR: يبدأ العد من 1 (السطر 1، العمود 1)
 * - EN: Counting starts from 1 (line 1, column 1)
 * - AR: يُستخدم في رسائل الأخطاء لإرشاد المبرمج بدقة
 * - EN: Used in error messages to guide programmer accurately
 */
struct Position {
    size_t line;      ///< رقم السطر / Line number (1-based)
    size_t column;    ///< رقم العمود / Column number (1-based)
    
    /**
     * @brief (AR) منشئ افتراضي - يبدأ من السطر 1 العمود 1
     * @brief (EN) Default constructor - starts at line 1, column 1
     */
    Position() : line(1), column(1) {}
    
    /**
     * @brief (AR) منشئ بقيم محددة
     * @brief (EN) Constructor with specific values
     * 
     * @param l (size_t) — (AR) رقم السطر / (EN) line number
     * @param c (size_t) — (AR) رقم العمود / (EN) column number
     */
    Position(size_t l, size_t c) : line(l), column(c) {}
    
    /**
     * @brief (AR) تحويل الموقع إلى نص للطباعة
     * @brief (EN) Convert position to string for printing
     * 
     * @return (std::string) — (AR) نص بصيغة "السطر:العمود" / (EN) string in format "line:column"
     * 
     * مثال:
     * Position pos(5, 10);
     * std::cout << pos.toString(); // "5:10"
     */
    std::string toString() const {
        return std::to_string(line) + ":" + std::to_string(column);
    }
};

// ======================================================================
// تعداد أنواع الرموز / Token Types Enumeration
// ======================================================================
/**
 * @enum TokenType
 * @brief (AR) جميع أنواع الرموز المدعومة في لغة ص
 * @brief (EN) All token types supported in Sad language
 * 
 * ملاحظات إضافية:
 * - AR: مقسّمة إلى فئات منطقية (كلمات مفتاحية، عوامل، قيم، إلخ)
 * - EN: Organized into logical categories (keywords, operators, literals, etc.)
 * - AR: كل كلمة مفتاحية عربية لها رمز خاص
 * - EN: Each Arabic keyword has its own token type
 */
enum class TokenType {
    // ========== الكلمات المفتاحية - تعريف الدوال والبنيات / Keywords - Functions & Structures ==========
    KEYWORD_FUNCTION,       ///< دالة / function
    KEYWORD_RETURN,         ///< إرجاع / return
    KEYWORD_CLASS,          ///< صنف / class
    KEYWORD_NEW,            ///< جديد / new
    KEYWORD_THIS,           ///< هذا / this
    KEYWORD_CONSTRUCTOR,    ///< منشئ / constructor
    KEYWORD_LAMBDA,         ///< لامدا / lambda
    KEYWORD_YIELD,          ///< اعطِ / yield
    
    // ========== الكلمات المفتاحية - التحكم في التدفق / Keywords - Control Flow ==========
    KEYWORD_IF,             ///< إذا / if
    KEYWORD_ELSE,           ///< وإلا / else
    KEYWORD_ELSE_IF,        ///< وإلا_إذا / else if
    KEYWORD_WHILE,          ///< بينما / while
    KEYWORD_FOR,            ///< لكل / for
    KEYWORD_IN,             ///< في / in
    KEYWORD_BREAK,          ///< اخرج / break
    KEYWORD_CONTINUE,       ///< استمر / continue
    
    // ========== الكلمات المفتاحية - التعامل مع الأخطاء / Keywords - Error Handling ==========
    KEYWORD_TRY,            ///< حاول / try
    KEYWORD_CATCH,          ///< اصطد / catch
    KEYWORD_THROW,          ///< ارمِ / throw
    KEYWORD_FINALLY,        ///< أخيراً / finally
    
    // ========== الكلمات المفتاحية - التحكم بالوصول / Keywords - Access Control ==========
    KEYWORD_PUBLIC,         ///< عام / public
    KEYWORD_PRIVATE,        ///< خاص / private
    KEYWORD_PROTECTED,      ///< محمي / protected
    
    // ========== الكلمات المفتاحية - الوراثة / Keywords - Inheritance ==========
    KEYWORD_EXTENDS,        ///< يرث / extends
    KEYWORD_IMPLEMENTS,     ///< ينفذ / implements
    KEYWORD_INTERFACE,      ///< واجهة / interface
    KEYWORD_ABSTRACT,       ///< مجرد / abstract
    KEYWORD_VIRTUAL,        ///< افتراضي / virtual
    KEYWORD_OVERRIDE,       ///< تجاوز / override
    
    // ========== الكلمات المفتاحية - الوحدات / Keywords - Modules ==========
    KEYWORD_IMPORT,         ///< استورد / import
    KEYWORD_EXPORT,         ///< صدّر / export
    KEYWORD_MODULE,         ///< وحدة / module
    KEYWORD_PACKAGE,        ///< حزمة / package
    
    // ========== الكلمات المفتاحية - المتغيرات / Keywords - Variables ==========
    KEYWORD_VAR,            ///< متغير / var
    KEYWORD_CONST,          ///< ثابت / const
    KEYWORD_LET,            ///< اجعل / let
    KEYWORD_STATIC,         ///< ساكن / static
    KEYWORD_ENUM,           ///< تعداد / enum
    
    // ========== أنواع البيانات الأساسية / Basic Data Types ==========
    TYPE_INTEGER,           ///< رقم / integer
    TYPE_DOUBLE,            ///< عشري / double/float
    TYPE_STRING,            ///< نص / string
    TYPE_BOOLEAN,           ///< منطقي / boolean
    TYPE_VOID,              ///< فراغ / void
    TYPE_NULL,              ///< عدم / null
    TYPE_ARRAY,             ///< مصفوفة / array
    TYPE_MAP,               ///< خريطة / map
    
    // ========== القيم الحرفية / Literal Values ==========
    LITERAL_TRUE,           ///< صحيح / true
    LITERAL_FALSE,          ///< خطأ / false
    LITERAL_NULL,           ///< عدم / null
    
    // ========== المعرّفات والقيم / Identifiers & Values ==========
    IDENTIFIER,             ///< معرّف (اسم متغير، دالة، صنف) / identifier
    NUMBER_INTEGER,         ///< رقم صحيح / integer literal (123)
    NUMBER_DOUBLE,          ///< رقم عشري / double literal (3.14)
    STRING_LITERAL,         ///< نص حرفي / string literal ("text")
    
    // ========== العوامل الحسابية / Arithmetic Operators ==========
    OP_PLUS,                ///< + جمع / addition
    OP_MINUS,               ///< - طرح / subtraction
    OP_MULTIPLY,            ///< * ضرب / multiplication
    OP_DIVIDE,              ///< / قسمة / division
    OP_MODULO,              ///< % باقي القسمة / modulo
    OP_POWER,               ///< ** أس / power
    
    // ========== عوامل التعيين / Assignment Operators ==========
    OP_ASSIGN,              ///< = تعيين / assignment
    OP_PLUS_ASSIGN,         ///< += جمع وتعيين / add and assign
    OP_MINUS_ASSIGN,        ///< -= طرح وتعيين / subtract and assign
    OP_MULTIPLY_ASSIGN,     ///< *= ضرب وتعيين / multiply and assign
    OP_DIVIDE_ASSIGN,       ///< /= قسمة وتعيين / divide and assign
    
    // ========== عوامل المقارنة / Comparison Operators ==========
    OP_EQUAL,               ///< == يساوي / equal to
    OP_NOT_EQUAL,           ///< != لا يساوي / not equal to
    OP_LESS,                ///< < أصغر من / less than
    OP_LESS_EQUAL,          ///< <= أصغر أو يساوي / less than or equal
    OP_GREATER,             ///< > أكبر من / greater than
    OP_GREATER_EQUAL,       ///< >= أكبر أو يساوي / greater than or equal
    
    // ========== العوامل المنطقية / Logical Operators ==========
    OP_AND,                 ///< && و / logical AND
    OP_OR,                  ///< || أو / logical OR
    OP_NOT,                 ///< ! ليس / logical NOT
    
    // ========== عوامل الزيادة والنقصان / Increment/Decrement ==========
    OP_INCREMENT,           ///< ++ زيادة / increment
    OP_DECREMENT,           ///< -- نقصان / decrement
    
    // ========== الأقواس والرموز الخاصة / Brackets & Special Symbols ==========
    PAREN_LEFT,             ///< ( قوس يسار / left parenthesis
    PAREN_RIGHT,            ///< ) قوس يمين / right parenthesis
    BRACE_LEFT,             ///< { قوس معقوف يسار / left brace
    BRACE_RIGHT,            ///< } قوس معقوف يمين / right brace
    BRACKET_LEFT,           ///< [ قوس مربع يسار / left bracket
    BRACKET_RIGHT,          ///< ] قوس مربع يمين / right bracket
    
    // ========== علامات الترقيم / Punctuation ==========
    COMMA,                  ///< , فاصلة / comma
    SEMICOLON,              ///< ; فاصلة منقوطة / semicolon
    COLON,                  ///< : نقطتان / colon
    DOT,                    ///< . نقطة / dot (member access)
    ARROW,                  ///< -> سهم / arrow
    FAT_ARROW,              ///< => سهم عريض / fat arrow (lambda/arrow functions)
    QUESTION,               ///< ? علامة استفهام / question mark
    AT_SIGN,                ///< @ علامة آت / at sign (decorators)
    
    // ========== رموز خاصة / Special Tokens ==========
    COMMENT,                ///< تعليق / comment
    NEWLINE,                ///< سطر جديد / newline
    END_OF_FILE,            ///< نهاية الملف / end of file
    INVALID,                ///< رمز غير صالح / invalid token
    WHITESPACE              ///< مسافة (عادة تُتجاهل) / whitespace
};

// ======================================================================
// فئة الرمز / Token Class
// ======================================================================
/**
 * @class Token
 * @brief (AR) يمثل رمزاً واحداً من البرنامج المصدري
 * @brief (EN) Represents a single token from source code
 * 
 * ملاحظات إضافية:
 * - AR: كل رمز يحتوي على: النوع، القيمة، الموقع
 * - EN: Each token contains: type, value, position
 * - AR: الرمز غير قابل للتغيير بعد الإنشاء (immutable)
 * - EN: Token is immutable after creation
 * - AR: يُستخدم في بناء شجرة التحليل النحوي (AST)
 * - EN: Used in building Abstract Syntax Tree (AST)
 */
class Token {
public:
    // ==================================================================
    // المنشئات / Constructors
    // ==================================================================
    
    /**
     * @brief (AR) منشئ كامل مع جميع المعلومات
     * @brief (EN) Full constructor with all information
     * 
     * @param type (TokenType) — (AR) نوع الرمز / (EN) token type
     * @param value (std::string) — (AR) قيمة الرمز النصية / (EN) token text value
     * @param position (Position) — (AR) موقع الرمز في المصدر / (EN) token position in source
     * 
     * مثال:
     * Token token(TokenType::NUMBER_INTEGER, "42", Position(1, 5));
     * 
     * ملاحظات إضافية:
     * - AR: القيمة تُحفظ كما هي من المصدر (قبل التحويل)
     * - EN: Value is stored as-is from source (before conversion)
     */
    Token(TokenType type, const std::string& value, const Position& position);
    
    /**
     * @brief (AR) منشئ مبسط بدون موقع (للاختبار)
     * @brief (EN) Simplified constructor without position (for testing)
     * 
     * @param type (TokenType) — (AR) نوع الرمز / (EN) token type
     * @param value (std::string) — (AR) قيمة الرمز / (EN) token value
     */
    Token(TokenType type, const std::string& value);
    
    /**
     * @brief (AR) منشئ مبسط بنوع فقط (لرموز بدون قيمة)
     * @brief (EN) Simplified constructor with type only (for valueless tokens)
     * 
     * @param type (TokenType) — (AR) نوع الرمز / (EN) token type
     * 
     * مثال:
     * Token token(TokenType::PAREN_LEFT); // "("
     */
    explicit Token(TokenType type);
    
    // ==================================================================
    // الوظائف الاستعلامية / Getter Functions
    // ==================================================================
    
    /**
     * @brief (AR) الحصول على نوع الرمز
     * @brief (EN) Get token type
     * 
     * @return (TokenType) — (AR) نوع الرمز / (EN) token type
     */
    TokenType getType() const { return type_; }
    
    /**
     * @brief (AR) الحصول على قيمة الرمز النصية
     * @brief (EN) Get token text value
     * 
     * @return (std::string) — (AR) القيمة النصية / (EN) text value
     */
    std::string getValue() const { return value_; }
    
    /**
     * @brief (AR) الحصول على موقع الرمز
     * @brief (EN) Get token position
     * 
     * @return (Position) — (AR) موقع الرمز / (EN) token position
     */
    Position getPosition() const { return position_; }
    
    // ==================================================================
    // وظائف الفحص / Check Functions
    // ==================================================================
    
    /**
     * @brief (AR) التحقق من أن الرمز من نوع معين
     * @brief (EN) Check if token is of specific type
     * 
     * @param type (TokenType) — (AR) النوع المراد المقارنة به / (EN) type to compare with
     * @return (bool) — (AR) true إذا كان النوع مطابق / (EN) true if types match
     * 
     * مثال:
     * if (token.is(TokenType::KEYWORD_IF)) {
     *     // معالجة شرط if
     * }
     */
    bool is(TokenType type) const { return type_ == type; }
    
    /**
     * @brief (AR) التحقق من أن الرمز أحد عدة أنواع
     * @brief (EN) Check if token is one of several types
     * 
     * @param types (std::initializer_list<TokenType>) — (AR) قائمة الأنواع / (EN) list of types
     * @return (bool) — (AR) true إذا كان النوع ضمن القائمة / (EN) true if type is in list
     * 
     * مثال:
     * if (token.isOneOf({TokenType::OP_PLUS, TokenType::OP_MINUS})) {
     *     // معالجة عملية جمع أو طرح
     * }
     */
    bool isOneOf(std::initializer_list<TokenType> types) const;
    
    /**
     * @brief (AR) التحقق من أن الرمز كلمة مفتاحية
     * @brief (EN) Check if token is a keyword
     * 
     * @return (bool) — (AR) true إذا كان الرمز كلمة مفتاحية / (EN) true if token is keyword
     */
    bool isKeyword() const;
    
    /**
     * @brief (AR) التحقق من أن الرمز عامل (operator)
     * @brief (EN) Check if token is an operator
     * 
     * @return (bool) — (AR) true إذا كان الرمز عامل / (EN) true if token is operator
     */
    bool isOperator() const;
    
    /**
     * @brief (AR) التحقق من أن الرمز قيمة حرفية (literal)
     * @brief (EN) Check if token is a literal
     * 
     * @return (bool) — (AR) true إذا كان الرمز قيمة حرفية / (EN) true if token is literal
     */
    bool isLiteral() const;
    
    // ==================================================================
    // وظائف التحويل / Conversion Functions
    // ==================================================================
    
    /**
     * @brief (AR) تحويل الرمز إلى نص للطباعة
     * @brief (EN) Convert token to string for printing
     * 
     * @return (std::string) — (AR) تمثيل نصي للرمز / (EN) string representation of token
     * 
     * مثال:
     * Token token(TokenType::NUMBER_INTEGER, "42", Position(1, 5));
     * std::cout << token.toString();
     * // Output: "Token(NUMBER_INTEGER, '42', 1:5)"
     */
    std::string toString() const;
    
    /**
     * @brief (AR) الحصول على اسم نوع الرمز
     * @brief (EN) Get token type name
     * 
     * @return (std::string) — (AR) اسم النوع / (EN) type name
     * 
     * مثال:
     * Token token(TokenType::KEYWORD_IF);
     * std::cout << token.getTypeName(); // "KEYWORD_IF"
     */
    std::string getTypeName() const;
    
    /**
     * @brief (AR) تحويل نوع الرمز إلى اسم (دالة ثابتة)
     * @brief (EN) Convert token type to name (static function)
     * 
     * @param type (TokenType) — (AR) نوع الرمز / (EN) token type
     * @return (std::string) — (AR) اسم النوع / (EN) type name
     */
    static std::string typeToString(TokenType type);

private:
    TokenType type_;        ///< نوع الرمز / Token type
    std::string value_;     ///< قيمة الرمز النصية / Token text value
    Position position_;     ///< موقع الرمز / Token position
};

} // namespace Lexer
} // namespace Sad
