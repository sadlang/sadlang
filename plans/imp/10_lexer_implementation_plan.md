# 🔤 خطة تنفيذ المحلل المعجمي / Lexer Implementation Plan

**التاريخ:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**المدة المقدرة:** 3 أسابيع

---

## 📋 ملخص / Summary

### (AR) الملخص
خطة تفصيلية لتطوير وتحسين المحلل المعجمي (Lexer) للغة "ص". يشمل دعم Unicode/RTL الكامل، الكلمات المفتاحية العربية، ومعالجة جميع أنواع الرموز (tokens) المحددة في `docs/language_spec/01_types.md`.

### (EN) Summary
Detailed plan for developing and enhancing the Lexer for Sad language. Includes full Unicode/RTL support, Arabic keywords, and handling all token types defined in `docs/language_spec/01_types.md`.

---

## 🎯 الأهداف / Objectives

1. ✅ دعم كامل للغة العربية (Unicode + RTL)
2. ✅ تعريف جميع أنواع الرموز (90+ TokenType)
3. ✅ معالجة الأرقام (decimal, binary, octal, hex, scientific)
4. ✅ معالجة النصوص (single, double, triple quotes, f-strings)
5. ✅ معالجة المعرفات (عربي/إنجليزي)
6. ✅ معالجة العوامل والرموز الخاصة
7. ✅ معالجة التعليقات (سطر واحد ومتعددة)
8. ✅ تتبع المواقع الدقيقة (line, column, offset)
9. ✅ Streaming lexer للملفات الكبيرة
10. ✅ نقاط امتداد لـ macros/preprocessor

---

## 📦 الاعتماديات / Dependencies

### من language_spec:
- ✅ `01_types.md` - أنواع البيانات والصيغة النحوية
- ✅ `06_syntax_examples.md` - أمثلة الصيغة
- ✅ `02_functions.md` - كلمات مفتاحية للدوال
- ✅ `03_oop.md` - كلمات مفتاحية للـ OOP

### ملفات Lexer الحالية:
```
✅ include/lexer/token.h              (موجود - يحتاج تحديث)
✅ src/lexer/token.cpp                (موجود - يحتاج تحديث)
✅ include/lexer/lexer_keywords.h     (موجود - كامل)
✅ src/lexer/lexer_keywords.cpp       (موجود - كامل)
✅ include/lexer/lexer_core.h         (موجود - يحتاج توسيع)
✅ src/lexer/lexer_core.cpp           (موجود - يحتاج توسيع)
```

---

## 🏗️ البنية المعمارية / Architecture

### Token System

```cpp
// include/lexer/token.h
namespace Sad::Lexer {

// موقع الرمز في الملف
struct Position {
    size_t line;        // رقم السطر (يبدأ من 1)
    size_t column;      // رقم العمود (يبدأ من 1)
    size_t offset;      // الموقع الكلي في الملف
    std::string filename; // اسم الملف
    
    std::string toString() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

// أنواع الرموز (90+ types)
enum class TokenType {
    // ===== Keywords: Types =====
    KEYWORD_TYPE_INT,           // رقم / int
    KEYWORD_TYPE_FLOAT,         // عشري / float
    KEYWORD_TYPE_STRING,        // نص / str
    KEYWORD_TYPE_BOOL,          // منطقي / bool
    KEYWORD_TYPE_ARRAY,         // مصفوفة / array
    KEYWORD_TYPE_MAP,           // خريطة / map
    KEYWORD_TYPE_ANY,           // أي / any
    KEYWORD_TYPE_NONE,          // لاشيء / none
    
    // ===== Keywords: Control Flow =====
    KEYWORD_IF,                 // إذا / if
    KEYWORD_ELSE_IF,            // وإلا_إذا / else_if
    KEYWORD_ELSE,               // وإلا / else
    KEYWORD_WHILE,              // بينما / while
    KEYWORD_FOR,                // لكل / for
    KEYWORD_IN,                 // في / in
    KEYWORD_BREAK,              // توقف / break
    KEYWORD_CONTINUE,           // تخطى / continue
    KEYWORD_RETURN,             // إرجاع / return
    
    // ===== Keywords: Functions =====
    KEYWORD_FUNCTION,           // دالة / function
    KEYWORD_LAMBDA,             // lambda
    KEYWORD_YIELD,              // أنتج / yield
    
    // ===== Keywords: OOP =====
    KEYWORD_CLASS,              // فئة / class
    KEYWORD_CONSTRUCTOR,        // بناء / constructor
    KEYWORD_DESTRUCTOR,         // إتلاف / destructor
    KEYWORD_THIS,               // هذا / this
    KEYWORD_BASE,               // أساس / base
    KEYWORD_OPERATOR,           // عامل / operator
    KEYWORD_PUBLIC,             // عام / public
    KEYWORD_PRIVATE,            // خاص / private
    KEYWORD_PROTECTED,          // محمي / protected
    KEYWORD_STATIC,             // ثابت / static
    KEYWORD_ABSTRACT,           // مجرد / abstract
    KEYWORD_INTERFACE,          // واجهة / interface
    
    // ===== Keywords: Values =====
    KEYWORD_TRUE,               // صحيح / true
    KEYWORD_FALSE,              // خطأ / false
    KEYWORD_NULL,               // لاشيء / null / none
    
    // ===== Literals =====
    INTEGER_LITERAL,            // 42, 0b1010, 0xFF
    FLOAT_LITERAL,              // 3.14, 2.5e10
    STRING_LITERAL,             // "نص", 'text'
    FSTRING_LITERAL,            // ن"الاسم: {اسم}"
    
    // ===== Identifiers =====
    IDENTIFIER,                 // متغير, variable
    
    // ===== Operators: Arithmetic =====
    PLUS,                       // +
    MINUS,                      // -
    MULTIPLY,                   // *
    DIVIDE,                     // /
    MODULO,                     // %
    POWER,                      // **
    
    // ===== Operators: Comparison =====
    EQUAL,                      // ==
    NOT_EQUAL,                  // !=
    LESS_THAN,                  // <
    LESS_EQUAL,                 // <=
    GREATER_THAN,               // >
    GREATER_EQUAL,              // >=
    
    // ===== Operators: Logical =====
    AND,                        // &&
    OR,                         // ||
    NOT,                        // !
    
    // ===== Operators: Bitwise =====
    BITWISE_AND,                // &
    BITWISE_OR,                 // |
    BITWISE_XOR,                // ^
    BITWISE_NOT,                // ~
    LEFT_SHIFT,                 // <<
    RIGHT_SHIFT,                // >>
    
    // ===== Operators: Assignment =====
    ASSIGN,                     // =
    PLUS_ASSIGN,                // +=
    MINUS_ASSIGN,               // -=
    MULTIPLY_ASSIGN,            // *=
    DIVIDE_ASSIGN,              // /=
    
    // ===== Delimiters =====
    LEFT_PAREN,                 // (
    RIGHT_PAREN,                // )
    LEFT_BRACKET,               // [
    RIGHT_BRACKET,              // ]
    LEFT_BRACE,                 // {
    RIGHT_BRACE,                // }
    
    // ===== Punctuation =====
    COMMA,                      // ،
    SEMICOLON,                  // ;
    COLON,                      // :
    DOT,                        // .
    ARROW,                      // ->
    QUESTION,                   // ?
    
    // ===== Special =====
    NEWLINE,                    // \n
    END_OF_FILE,                // EOF
    ERROR,                      // Error token
    COMMENT                     // // or /* */
};

// الرمز Token
class Token {
public:
    TokenType type;
    std::string lexeme;         // النص الأصلي
    Position position;
    
    // القيمة المحللة (للـ literals)
    std::variant<
        std::monostate,         // لا قيمة
        int64_t,                // INTEGER
        double,                 // FLOAT
        std::string,            // STRING
        bool                    // BOOLEAN
    > value;
    
    Token(TokenType t, const std::string& lex, Position pos)
        : type(t), lexeme(lex), position(pos) {}
    
    // Utility methods
    bool isKeyword() const;
    bool isLiteral() const;
    bool isOperator() const;
    bool isIdentifier() const;
    
    std::string typeToString() const;
    std::string toString() const;
    std::string valueToString() const;
};

} // namespace Sad::Lexer
```

---

## 📁 الملفات المطلوبة / Required Files

### Files to Update:
```
include/lexer/
├── token.h                     ✅ UPDATE - إضافة Position, value variant
├── lexer_core.h                ✅ UPDATE - إضافة streaming support
├── lexer_keywords.h            ✅ OK - كامل (45 كلمة)
└── lexer_interface.h           🆕 NEW - واجهة قابلة للتوسع

src/lexer/
├── token.cpp                   ✅ UPDATE - تنفيذ utility methods
├── lexer_core.cpp              ✅ UPDATE - تحسين Unicode/RTL
├── lexer_keywords.cpp          ✅ OK - كامل
├── lexer_numbers.cpp           🆕 NEW - معالجة الأرقام المتقدمة
├── lexer_strings.cpp           🆕 NEW - معالجة النصوص + f-strings
├── lexer_identifiers.cpp       🆕 NEW - معالجة المعرفات
├── lexer_operators.cpp         🆕 NEW - معالجة العوامل
└── lexer_comments.cpp          🆕 NEW - معالجة التعليقات

tests/lexer_tests/
├── token_test.cpp              🆕 NEW
├── position_test.cpp           🆕 NEW
├── numbers_test.cpp            🆕 NEW
├── strings_test.cpp            🆕 NEW
├── unicode_test.cpp            🆕 NEW
├── keywords_test.cpp           🆕 NEW
└── integration_test.cpp        🆕 NEW
```

---

## 🔧 تقسيم العمل / Task Breakdown

### Week 1: Core Enhancements

#### Task 1.1: تحديث Token & Position (يوم 1-2)
**الملفات:**
- `include/lexer/token.h`
- `src/lexer/token.cpp`

**الخطوات:**
```cpp
// 1. إضافة Position struct
struct Position {
    size_t line = 1;
    size_t column = 1;
    size_t offset = 0;
    std::string filename = "";
};

// 2. إضافة value variant للـ Token
std::variant<std::monostate, int64_t, double, std::string, bool> value;

// 3. إضافة utility methods
bool Token::isKeyword() const {
    return type >= TokenType::KEYWORD_TYPE_INT && 
           type <= TokenType::KEYWORD_NULL;
}

bool Token::isLiteral() const {
    return type >= TokenType::INTEGER_LITERAL && 
           type <= TokenType::STRING_LITERAL;
}

std::string Token::toString() const {
    return typeToString() + " '" + lexeme + "' at " + position.toString();
}
```

**الاختبارات:**
```cpp
// tests/lexer_tests/token_test.cpp
TEST(TokenTest, CreationWithPosition) {
    Position pos{1, 5, 4, "test.s"};
    Token t(TokenType::INTEGER_LITERAL, "42", pos);
    
    EXPECT_EQ(t.type, TokenType::INTEGER_LITERAL);
    EXPECT_EQ(t.lexeme, "42");
    EXPECT_EQ(t.position.line, 1);
    EXPECT_EQ(t.position.column, 5);
}

TEST(TokenTest, ValueVariant) {
    Token t(TokenType::INTEGER_LITERAL, "42", {});
    t.value = int64_t(42);
    
    EXPECT_TRUE(std::holds_alternative<int64_t>(t.value));
    EXPECT_EQ(std::get<int64_t>(t.value), 42);
}

TEST(TokenTest, TypeChecking) {
    Token keyword(TokenType::KEYWORD_IF, "إذا", {});
    Token literal(TokenType::INTEGER_LITERAL, "42", {});
    
    EXPECT_TRUE(keyword.isKeyword());
    EXPECT_FALSE(keyword.isLiteral());
    EXPECT_TRUE(literal.isLiteral());
    EXPECT_FALSE(literal.isKeyword());
}
```

**Debug Points:**
```cpp
DEBUG_PRINT("Token created: %s at %s", 
            toString().c_str(), position.toString().c_str());
```

**معايير القبول:**
- ✅ Position tracking works
- ✅ Value variant works for all types
- ✅ Utility methods implemented
- ✅ Tests pass (≥10 tests)
- ✅ Doxygen docs complete

---

#### Task 1.2: معالجة الأرقام المتقدمة (يوم 3-4)
**ملف جديد:** `src/lexer/lexer_numbers.cpp`

**الوظائف:**
```cpp
// include/lexer/lexer_core.h (إضافة)
class LexerCore {
private:
    Token scanNumber();
    Token scanBinaryNumber();    // 0b1010
    Token scanOctalNumber();     // 0o17
    Token scanHexNumber();       // 0xFF
    Token scanDecimalNumber();   // 42 or 3.14
    Token scanScientificNumber();// 2.5e10
};

// src/lexer/lexer_numbers.cpp
Token LexerCore::scanNumber() {
    DEBUG_PRINT("Scanning number at pos %zu", currentPos);
    
    Position startPos = getCurrentPosition();
    
    // Check for prefix
    if (current == '0' && currentPos + 1 < source.length()) {
        char next = source[currentPos + 1];
        if (next == 'b' || next == 'B') return scanBinaryNumber();
        if (next == 'o' || next == 'O') return scanOctalNumber();
        if (next == 'x' || next == 'X') return scanHexNumber();
    }
    
    return scanDecimalNumber();
}

Token LexerCore::scanDecimalNumber() {
    std::string number;
    bool isFloat = false;
    
    // Integer part
    while (isDigit(peek())) {
        number += advance();
    }
    
    // Fractional part
    if (peek() == '.' && isDigit(peekNext())) {
        isFloat = true;
        number += advance(); // consume '.'
        while (isDigit(peek())) {
            number += advance();
        }
    }
    
    // Scientific notation
    if (peek() == 'e' || peek() == 'E') {
        isFloat = true;
        number += advance(); // consume 'e'
        if (peek() == '+' || peek() == '-') {
            number += advance();
        }
        while (isDigit(peek())) {
            number += advance();
        }
    }
    
    Token token(
        isFloat ? TokenType::FLOAT_LITERAL : TokenType::INTEGER_LITERAL,
        number,
        startPos
    );
    
    if (isFloat) {
        token.value = std::stod(number);
        DEBUG_PRINT("Float literal: %f", std::get<double>(token.value));
    } else {
        token.value = std::stoll(number);
        DEBUG_PRINT("Integer literal: %lld", std::get<int64_t>(token.value));
    }
    
    return token;
}

Token LexerCore::scanBinaryNumber() {
    advance(); // skip '0'
    advance(); // skip 'b'
    
    std::string binary;
    while (peek() == '0' || peek() == '1') {
        binary += advance();
    }
    
    if (binary.empty()) {
        return makeError("Invalid binary number");
    }
    
    int64_t value = std::stoll(binary, nullptr, 2);
    Token token(TokenType::INTEGER_LITERAL, "0b" + binary, getCurrentPosition());
    token.value = value;
    
    DEBUG_PRINT("Binary literal: 0b%s = %lld", binary.c_str(), value);
    return token;
}

Token LexerCore::scanHexNumber() {
    advance(); // skip '0'
    advance(); // skip 'x'
    
    std::string hex;
    while (isHexDigit(peek())) {
        hex += advance();
    }
    
    if (hex.empty()) {
        return makeError("Invalid hex number");
    }
    
    int64_t value = std::stoll(hex, nullptr, 16);
    Token token(TokenType::INTEGER_LITERAL, "0x" + hex, getCurrentPosition());
    token.value = value;
    
    DEBUG_PRINT("Hex literal: 0x%s = %lld", hex.c_str(), value);
    return token;
}
```

**الاختبارات:**
```cpp
// tests/lexer_tests/numbers_test.cpp
TEST(NumberTest, DecimalInteger) {
    LexerCore lexer("42");
    auto tokens = lexer.tokenize();
    
    ASSERT_EQ(tokens.size(), 2); // number + EOF
    EXPECT_EQ(tokens[0].type, TokenType::INTEGER_LITERAL);
    EXPECT_EQ(std::get<int64_t>(tokens[0].value), 42);
}

TEST(NumberTest, Float) {
    LexerCore lexer("3.14");
    auto tokens = lexer.tokenize();
    
    EXPECT_EQ(tokens[0].type, TokenType::FLOAT_LITERAL);
    EXPECT_DOUBLE_EQ(std::get<double>(tokens[0].value), 3.14);
}

TEST(NumberTest, Scientific) {
    LexerCore lexer("2.5e10");
    auto tokens = lexer.tokenize();
    
    EXPECT_EQ(tokens[0].type, TokenType::FLOAT_LITERAL);
    EXPECT_DOUBLE_EQ(std::get<double>(tokens[0].value), 2.5e10);
}

TEST(NumberTest, Binary) {
    LexerCore lexer("0b1010");
    auto tokens = lexer.tokenize();
    
    EXPECT_EQ(tokens[0].type, TokenType::INTEGER_LITERAL);
    EXPECT_EQ(std::get<int64_t>(tokens[0].value), 10);
}

TEST(NumberTest, Hex) {
    LexerCore lexer("0xFF");
    auto tokens = lexer.tokenize();
    
    EXPECT_EQ(tokens[0].type, TokenType::INTEGER_LITERAL);
    EXPECT_EQ(std::get<int64_t>(tokens[0].value), 255);
}

TEST(NumberTest, Octal) {
    LexerCore lexer("0o17");
    auto tokens = lexer.tokenize();
    
    EXPECT_EQ(tokens[0].type, TokenType::INTEGER_LITERAL);
    EXPECT_EQ(std::get<int64_t>(tokens[0].value), 15);
}
```

**معايير القبول:**
- ✅ Decimal, binary, octal, hex supported
- ✅ Float and scientific notation work
- ✅ Negative numbers handled
- ✅ Tests pass (≥15 tests)
- ✅ Error handling for invalid formats

---

### Week 2: Strings & Unicode

#### Task 2.1: معالجة النصوص + F-Strings (يوم 5-7)
**ملف جديد:** `src/lexer/lexer_strings.cpp`

```cpp
Token LexerCore::scanString() {
    char quote = advance(); // " أو '
    std::string str;
    Position startPos = getCurrentPosition();
    
    // Check for triple quotes
    if (peek() == quote && peekNext() == quote) {
        return scanMultilineString(quote);
    }
    
    // Check for f-string
    bool isFString = (previousChar() == 'ن' || previousChar() == 'f');
    
    while (peek() != quote && !isAtEnd()) {
        if (peek() == '\\') {
            advance(); // skip backslash
            str += scanEscapeSequence();
        } else {
            str += advance();
        }
    }
    
    if (isAtEnd()) {
        return makeError("Unterminated string");
    }
    
    advance(); // closing quote
    
    Token token(
        isFString ? TokenType::FSTRING_LITERAL : TokenType::STRING_LITERAL,
        str,
        startPos
    );
    token.value = str;
    
    DEBUG_PRINT("String literal: '%s'", str.c_str());
    return token;
}

char LexerCore::scanEscapeSequence() {
    char c = advance();
    switch (c) {
        case 'n': return '\n';
        case 't': return '\t';
        case 'r': return '\r';
        case '\\': return '\\';
        case '"': return '"';
        case '\'': return '\'';
        default:
            WARN_PRINT("Unknown escape sequence: \\%c", c);
            return c;
    }
}

Token LexerCore::scanMultilineString(char quote) {
    advance(); // second quote
    advance(); // third quote
    
    std::string str;
    
    while (true) {
        if (isAtEnd()) {
            return makeError("Unterminated multiline string");
        }
        
        // Check for closing """
        if (peek() == quote && peekNext() == quote && peekNextNext() == quote) {
            advance(); advance(); advance();
            break;
        }
        
        str += advance();
    }
    
    Token token(TokenType::STRING_LITERAL, str, getCurrentPosition());
    token.value = str;
    return token;
}
```

**الاختبارات:**
```cpp
TEST(StringTest, SimpleString) {
    LexerCore lexer("\"مرحبا\"");
    auto tokens = lexer.tokenize();
    
    EXPECT_EQ(tokens[0].type, TokenType::STRING_LITERAL);
    EXPECT_EQ(std::get<std::string>(tokens[0].value), "مرحبا");
}

TEST(StringTest, EscapeSequences) {
    LexerCore lexer("\"Line1\\nLine2\\t\"");
    auto tokens = lexer.tokenize();
    
    std::string expected = "Line1\nLine2\t";
    EXPECT_EQ(std::get<std::string>(tokens[0].value), expected);
}

TEST(StringTest, MultilineString) {
    LexerCore lexer(R"("""
        First line
        Second line
    """)");
    auto tokens = lexer.tokenize();
    
    EXPECT_EQ(tokens[0].type, TokenType::STRING_LITERAL);
    EXPECT_TRUE(std::get<std::string>(tokens[0].value).find("First line") != std::string::npos);
}

TEST(StringTest, FString) {
    LexerCore lexer("ن\"الاسم: {اسم}\"");
    auto tokens = lexer.tokenize();
    
    EXPECT_EQ(tokens[0].type, TokenType::FSTRING_LITERAL);
}
```

---

### Week 3: Integration & Testing

#### Task 3.1: Streaming Lexer (يوم 8-9)
```cpp
// include/lexer/lexer_interface.h
class ILexer {
public:
    virtual ~ILexer() = default;
    virtual Token nextToken() = 0;
    virtual bool hasMore() const = 0;
    virtual void reset() = 0;
};

// LexerCore يدعم streaming
class LexerCore : public ILexer {
private:
    size_t currentPos = 0;
    std::vector<Token> cachedTokens;
    bool streamMode = false;
    
public:
    void enableStreaming(bool enable) { streamMode = enable; }
    Token nextToken() override;
    bool hasMore() const override;
};
```

---

## 🧪 الاختبارات / Tests Summary

### Unit Tests (30+ tests)
```
tests/lexer_tests/
├── token_test.cpp          (10 tests)
├── position_test.cpp       (5 tests)
├── numbers_test.cpp        (15 tests)
├── strings_test.cpp        (12 tests)
├── unicode_test.cpp        (8 tests)
└── keywords_test.cpp       (10 tests)
```

### Integration Tests
```cpp
TEST(LexerIntegration, CompleteProgram) {
    std::string program = R"(
        رقم عمر = 25
        نص اسم = "أحمد"
        إذا عمر > 18
            اطبع("بالغ")
        نهاية
    )";
    
    LexerCore lexer(program);
    auto tokens = lexer.tokenize();
    
    // Verify token sequence
    EXPECT_EQ(tokens[0].type, TokenType::KEYWORD_TYPE_INT);    // رقم
    EXPECT_EQ(tokens[1].type, TokenType::IDENTIFIER);          // عمر
    EXPECT_EQ(tokens[2].type, TokenType::ASSIGN);              // =
    EXPECT_EQ(tokens[3].type, TokenType::INTEGER_LITERAL);     // 25
    // ...
}
```

---

## ⏱️ الجدول الزمني / Timeline

| الأسبوع | المهام | الساعات |
|---------|--------|---------|
| Week 1 | Token + Numbers | 40 ساعة |
| Week 2 | Strings + Unicode | 40 ساعة |
| Week 3 | Integration + Testing | 40 ساعة |
| **Total** | **3 weeks** | **120 ساعة** |

---

## 🚀 أوامر PowerShell / PowerShell Commands

```powershell
# إنشاء فرع العمل
cd C:\s\s_language
git checkout -b feat/lexer/enhanced-tokenization

# بناء واختبار
mkdir build -Force
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DDEBUG=ON
cmake --build . --config Debug

# تشغيل اختبارات Lexer فقط
ctest -R lexer_tests --output-on-failure -C Debug

# Commit
cd ..
git add include/lexer/* src/lexer/* tests/lexer_tests/*
git commit -m "[lexer] Enhanced tokenization with full Unicode support

- Added Position tracking (line, column, offset)
- Implemented value variant for Token
- Added support for binary/octal/hex/scientific numbers
- Implemented f-strings and multiline strings
- Added streaming lexer support
- 60+ unit tests with ≥90% coverage"

# Push
git push --set-upstream origin feat/lexer/enhanced-tokenization
```

---

## ⚠️ المخاطر / Risks

| الخطر | التخفيف |
|-------|---------|
| Unicode edge cases | اختبارات شاملة مع RTL |
| Performance مع streaming | Profiling + buffering |
| Escape sequences معقدة | مرجع واضح + tests |

---

**آخر تحديث:** 4 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ
