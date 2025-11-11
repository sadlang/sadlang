# 🧪 معايير الجودة والاختبارات

**تاريخ الإنشاء:** 4 نوفمبر 2025  
**آخر تحديث:** 4 نوفمبر 2025

---

## 📋 جدول المحتويات

1. [معايير الجودة العامة](#معايير-الجودة-العامة)
2. [التوثيق (Documentation)](#التوثيق-documentation)
3. [أنماط الكود (Code Style)](#أنماط-الكود-code-style)
4. [نظام الـ Logging](#نظام-الـ-logging)
5. [الاختبارات (Testing)](#الاختبارات-testing)
6. [تغطية الكود (Code Coverage)](#تغطية-الكود-code-coverage)
7. [التحليل الثابت (Static Analysis)](#التحليل-الثابت-static-analysis)

---

## ✅ معايير الجودة العامة

### القواعد الذهبية

| القاعدة | الوصف | الأولوية |
|---------|-------|----------|
| **لا تكسر البناء** | الكود يجب أن يبني بدون أخطاء | 🔴 حرجة |
| **جميع الاختبارات تمر** | لا exceptions | 🔴 حرجة |
| **توثيق ثنائي اللغة** | AR + EN لكل دالة عامة | 🔴 حرجة |
| **DEBUG_PRINT** | في نقاط الدخول/الخروج | 🟠 مهمة |
| **معالجة الأخطاء** | لا تترك exceptions غير معالجة | 🔴 حرجة |
| **الأسماء الواضحة** | لا اختصارات غامضة | 🟠 مهمة |
| **تغطية ≥80%** | للمكونات الأساسية | 🟠 مهمة |
| **لا Code Smells** | استخدم clang-tidy | 🟡 مرغوبة |

---

### Code Quality Checklist

قبل كل commit:

- [ ] الكود يبني بدون أخطاء
- [ ] لا توجد تحذيرات compiler
- [ ] clang-format مطبق
- [ ] التوثيق محدث
- [ ] DEBUG_PRINT مضاف
- [ ] الاختبارات تمر
- [ ] لا memory leaks (إذا استخدمت Valgrind)
- [ ] الأسماء واضحة
- [ ] معالجة الأخطاء موجودة

---

## 📚 التوثيق (Documentation)

### قالب توثيق Doxygen الموحد

#### لدالة/method:

```cpp
/**
 * @brief (AR) وصف مختصر بالعربية - سطر واحد
 * @brief (EN) Short description in English - one line
 * 
 * @details (AR) 
 * وصف مفصل بالعربية (اختياري):
 * - نقطة 1
 * - نقطة 2
 * 
 * @details (EN)
 * Detailed description in English (optional):
 * - Point 1
 * - Point 2
 * 
 * @param paramName (Type) (AR) وصف المعامل بالعربية / (EN) Parameter description
 * @param anotherParam (Type) (AR) معامل آخر / (EN) Another parameter
 * 
 * @return (ReturnType) (AR) وصف القيمة المرجعة / (EN) Return value description
 * 
 * @throws ExceptionType (AR) متى يُرمى هذا الاستثناء / (EN) When this exception is thrown
 * @throws AnotherException (AR) استثناء آخر / (EN) Another exception
 * 
 * @note (AR) ملاحظة مهمة / (EN) Important note
 * 
 * @example
 * @code{.cpp}
 * // (AR) مثال الاستخدام:
 * // (EN) Usage example:
 * Lexer lexer("input.s");
 * auto tokens = lexer.tokenize();
 * @endcode
 * 
 * @see RelatedFunction
 * @see AnotherClass
 */
ReturnType functionName(Type paramName, Type anotherParam);
```

---

### أمثلة حقيقية

#### مثال 1: دالة بسيطة
```cpp
/**
 * @brief (AR) التقدم للرمز التالي في المحلل المعجمي
 * @brief (EN) Advance to the next token in the lexer
 * 
 * @return (Token) (AR) الرمز الحالي قبل التقدم / (EN) Current token before advancing
 * 
 * @throws std::out_of_range (AR) إذا وصلنا لنهاية الملف / (EN) If reached end of file
 * 
 * @example
 * @code{.cpp}
 * Token current = lexer.advance();
 * std::cout << current.toString() << std::endl;
 * @endcode
 */
Token advance();
```

#### مثال 2: دالة مع معاملات
```cpp
/**
 * @brief (AR) تحليل تعبير ثنائي (مثل: أ + ب)
 * @brief (EN) Parse binary expression (e.g., a + b)
 * 
 * @param left (unique_ptr<Expression>) (AR) التعبير الأيسر / (EN) Left expression
 * @param op (Token) (AR) العامل (PLUS, MINUS, ...) / (EN) Operator token
 * @param right (unique_ptr<Expression>) (AR) التعبير الأيمن / (EN) Right expression
 * 
 * @return (unique_ptr<BinaryOpExpr>) (AR) عقدة التعبير الثنائي / (EN) Binary operation node
 * 
 * @throws ParseError (AR) إذا كان العامل غير صحيح / (EN) If operator is invalid
 * 
 * @note (AR) يجب التحقق من أولوية العامل قبل الاستدعاء
 * @note (EN) Operator precedence should be checked before calling
 */
std::unique_ptr<BinaryOpExpr> parseBinaryOp(
    std::unique_ptr<Expression> left,
    Token op,
    std::unique_ptr<Expression> right
);
```

#### مثال 3: constructor
```cpp
/**
 * @brief (AR) المنشئ - إنشاء محلل معجمي من نص
 * @brief (EN) Constructor - Create lexer from source text
 * 
 * @param source (const string&) (AR) النص المصدري للتحليل / (EN) Source code to analyze
 * 
 * @throws std::invalid_argument (AR) إذا كان النص فارغاً / (EN) If source is empty
 * 
 * @example
 * @code{.cpp}
 * std::string code = "رقم س = 10";
 * LexerCore lexer(code);
 * @endcode
 */
explicit LexerCore(const std::string& source);
```

---

### توثيق رأس الملف

في بداية كل ملف `.h` أو `.cpp`:

```cpp
/**
 * @file filename.h
 * @brief (AR) وصف مختصر لوظيفة الملف
 * @brief (EN) Short description of file purpose
 * 
 * @details (AR)
 * وصف مفصل:
 * هذا الملف يحتوي على تعريفات...
 * 
 * @details (EN)
 * Detailed description:
 * This file contains definitions for...
 * 
 * @author Sad Language Team
 * @date 2025-11-04
 * @version 1.0
 * 
 * @copyright MIT License
 */

#pragma once

// ... الكود
```

---

### ملاحظات التوثيق

#### ✅ جيد:
```cpp
/**
 * @brief (AR) حساب القيمة المطلقة لعدد
 * @brief (EN) Calculate absolute value of number
 * @param value (int) (AR) العدد / (EN) The number
 * @return (int) (AR) القيمة المطلقة / (EN) Absolute value
 */
int abs(int value);
```

#### ❌ سيء:
```cpp
// حساب abs
int abs(int value);
```

#### ❌ سيء جداً:
```cpp
int abs(int value); // بدون توثيق نهائياً
```

---

## 🎨 أنماط الكود (Code Style)

### ملف .clang-format

```yaml
# C:\s\s_language\.clang-format
BasedOnStyle: Google
Language: Cpp
Standard: c++17

# المسافات البادئة
IndentWidth: 4
TabWidth: 4
UseTab: Never
ColumnLimit: 100

# الأقواس
BreakBeforeBraces: Attach
AllowShortFunctionsOnASingleLine: Empty
AllowShortIfStatementsOnASingleLine: Never
AllowShortLoopsOnASingleLine: false

# المحاذاة
AlignConsecutiveAssignments: true
AlignConsecutiveDeclarations: false
AlignOperands: true
AlignTrailingComments: true

# المسافات
SpaceAfterCStyleCast: true
SpaceBeforeParens: ControlStatements
SpacesInAngles: false
SpacesInContainerLiterals: false
SpacesInParentheses: false
SpacesInSquareBrackets: false

# التضمين
SortIncludes: true
IncludeBlocks: Regroup
```

### تطبيق clang-format

```powershell
# على ملف واحد
clang-format -i src/lexer/lexer_core.cpp

# على كل الملفات
Get-ChildItem -Path src,include -Recurse -Include *.cpp,*.h | 
    ForEach-Object { clang-format -i $_.FullName }

# فحص بدون تعديل
clang-format --dry-run --Werror src/lexer/lexer_core.cpp
```

---

### قواعد التسمية

| العنصر | النمط | مثال |
|--------|-------|------|
| **Class** | PascalCase | `LexerCore`, `ParserCore` |
| **Function** | camelCase | `advance()`, `parseExpression()` |
| **Variable** | snake_case | `current_token`, `source_code` |
| **Constant** | UPPER_SNAKE_CASE | `MAX_SIZE`, `DEBUG_MODE` |
| **Member Variable** | snake_case_ (مع _) | `position_`, `tokens_` |
| **Namespace** | PascalCase | `Sad::Lexer` |
| **Enum** | PascalCase | `TokenType`, `NodeType` |
| **Enum Value** | UPPER_SNAKE_CASE | `INTEGER`, `STRING` |

---

### أمثلة الأنماط

#### ✅ نمط جيد:

```cpp
namespace Sad {
namespace Lexer {

class LexerCore {
public:
    explicit LexerCore(const std::string& source);
    
    std::vector<Token> tokenize();
    Token getNextToken();
    
private:
    std::string source_;
    size_t position_;
    size_t line_;
    
    char peek() const;
    char advance();
    bool isAtEnd() const;
};

} // namespace Lexer
} // namespace Sad
```

#### ❌ نمط سيء:

```cpp
namespace sad {  // ❌ lowercase
namespace lexer {  // ❌ lowercase

class lexer_core {  // ❌ snake_case للـ class
public:
    lexer_core(std::string source);  // ❌ غير explicit، تمرير بالقيمة
    
    std::vector<Token> Tokenize();  // ❌ PascalCase
    Token GetNextToken();  // ❌ PascalCase
    
private:
    std::string Source;  // ❌ PascalCase، بدون _
    size_t Pos;  // ❌ اختصار
    
    char Peek();  // ❌ PascalCase
};

} } // ❌ بدون تعليقات
```

---

### قواعد التنسيق

#### 1. المسافات والأقواس:
```cpp
// ✅ جيد
if (condition) {
    doSomething();
}

for (int i = 0; i < size; i++) {
    process(i);
}

// ❌ سيء
if(condition){
    doSomething();
}

for(int i=0;i<size;i++){
    process(i);
}
```

#### 2. الدوال:
```cpp
// ✅ جيد
ReturnType functionName(Type param1, Type param2) {
    // ...
    return value;
}

// ❌ سيء
ReturnType functionName(Type param1,Type param2)
{
    //...
    return value;
}
```

#### 3. الأقواس في الشروط:
```cpp
// ✅ جيد - دائماً استخدم {}
if (condition) {
    statement();
}

// ❌ سيء - بدون {}
if (condition)
    statement();
```

---

## 📝 نظام الـ Logging

### تفعيل DEBUG_PRINT

#### في CMakeLists.txt:
```cmake
option(DEBUG "Enable debug output" OFF)

if(DEBUG)
    add_definitions(-DDEBUG=1)
endif()
```

#### في C++ (ملف مشترك: `include/utils/debug.h`):
```cpp
#pragma once
#include <iostream>
#include <string>

#ifdef DEBUG
    #define DEBUG_PRINT(tag, msg) \
        std::cout << "[DEBUG][" << tag << "] " << msg << std::endl
#else
    #define DEBUG_PRINT(tag, msg) ((void)0)
#endif

#ifdef DEBUG
    #define DEBUG_PRINT_ENTRY(tag, func) \
        std::cout << "[DEBUG][" << tag << "] → Entering " << func << std::endl
    #define DEBUG_PRINT_EXIT(tag, func) \
        std::cout << "[DEBUG][" << tag << "] ← Exiting " << func << std::endl
#else
    #define DEBUG_PRINT_ENTRY(tag, func) ((void)0)
    #define DEBUG_PRINT_EXIT(tag, func) ((void)0)
#endif
```

---

### استخدام DEBUG_PRINT

#### في Lexer:
```cpp
#include "utils/debug.h"

Token LexerCore::scanNumber() {
    DEBUG_PRINT_ENTRY("LEXER", "scanNumber()");
    
    std::string number;
    bool is_double = false;
    
    while (isDigit(peek()) || peek() == '.') {
        if (peek() == '.') {
            DEBUG_PRINT("LEXER", "Found decimal point");
            if (is_double) {
                DEBUG_PRINT("LEXER", "ERROR: Multiple decimal points");
                throw std::runtime_error("Multiple decimal points in number");
            }
            is_double = true;
        }
        number += advance();
    }
    
    DEBUG_PRINT("LEXER", "Scanned number: " + number);
    DEBUG_PRINT_EXIT("LEXER", "scanNumber()");
    
    return Token(is_double ? TokenType::DOUBLE : TokenType::INTEGER, 
                 number, line_, column_);
}
```

#### في Parser:
```cpp
std::unique_ptr<Expression> ParserCore::parsePrimary() {
    DEBUG_PRINT_ENTRY("PARSER", "parsePrimary()");
    
    if (match(TokenType::INTEGER)) {
        DEBUG_PRINT("PARSER", "Parsing integer literal");
        Token token = previous();
        DEBUG_PRINT_EXIT("PARSER", "parsePrimary()");
        return std::make_unique<LiteralExpr>(std::stoi(token.getLexeme()));
    }
    
    // ...
    
    DEBUG_PRINT("PARSER", "ERROR: Unexpected token");
    throw ParseError("Expected expression");
}
```

---

### مخرجات DEBUG نموذجية:

```
[DEBUG][LEXER] → Entering scanNumber()
[DEBUG][LEXER] Found decimal point
[DEBUG][LEXER] Scanned number: 3.14
[DEBUG][LEXER] ← Exiting scanNumber()
[DEBUG][PARSER] → Entering parsePrimary()
[DEBUG][PARSER] Parsing double literal
[DEBUG][PARSER] ← Exiting parsePrimary()
```

---

### مستويات Logging (متقدم - اختياري):

```cpp
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static void setLevel(LogLevel level);
    
    static void debug(const std::string& tag, const std::string& msg);
    static void info(const std::string& tag, const std::string& msg);
    static void warning(const std::string& tag, const std::string& msg);
    static void error(const std::string& tag, const std::string& msg);
    
private:
    static LogLevel current_level_;
};

// الاستخدام:
Logger::debug("LEXER", "Token scanned");
Logger::error("PARSER", "Syntax error at line 10");
```

---

## 🧪 الاختبارات (Testing)

### Framework: Google Test

#### إضافة اختبار جديد:

```cpp
// tests/lexer_tests/test_token.cpp

#include <gtest/gtest.h>
#include "../../include/lexer/token.h"

using namespace Sad::Lexer;

// Test Fixture (اختياري)
class TokenTest : public ::testing::Test {
protected:
    void SetUp() override {
        // إعداد قبل كل اختبار
    }
    
    void TearDown() override {
        // تنظيف بعد كل اختبار
    }
};

// اختبار بسيط
TEST(TokenTest, CreateIntegerToken) {
    Token token(TokenType::INTEGER, "42", 1, 1);
    
    EXPECT_EQ(token.getType(), TokenType::INTEGER);
    EXPECT_EQ(token.getLexeme(), "42");
    EXPECT_EQ(token.getLine(), 1);
    EXPECT_EQ(token.getColumn(), 1);
}

// اختبار مع Fixture
TEST_F(TokenTest, TokenTypeConversion) {
    Token token(TokenType::PLUS, "+", 1, 5);
    
    EXPECT_TRUE(token.isOperator());
    EXPECT_FALSE(token.isKeyword());
    EXPECT_FALSE(token.isLiteral());
}

// اختبار معالجة أخطاء
TEST(TokenTest, InvalidTokenThrows) {
    EXPECT_THROW({
        Token token(TokenType::UNKNOWN, "", 0, 0);
    }, std::invalid_argument);
}

// اختبار مع معاملات (Parameterized Test)
class TokenTypeTest : public ::testing::TestWithParam<std::pair<TokenType, bool>> {};

TEST_P(TokenTypeTest, CheckIsKeyword) {
    auto [type, expected] = GetParam();
    Token token(type, "test", 1, 1);
    EXPECT_EQ(token.isKeyword(), expected);
}

INSTANTIATE_TEST_SUITE_P(
    KeywordTests,
    TokenTypeTest,
    ::testing::Values(
        std::make_pair(TokenType::FUNCTION_KEYWORD, true),
        std::make_pair(TokenType::IF_KEYWORD, true),
        std::make_pair(TokenType::INTEGER, false),
        std::make_pair(TokenType::PLUS, false)
    )
);
```

---

### أنواع الاختبارات

#### 1. Unit Tests (اختبارات وحدة):
```cpp
// اختبار دالة واحدة بمعزل عن الباقي
TEST(LexerTest, PeekReturnsCurrentChar) {
    LexerCore lexer("abc");
    EXPECT_EQ(lexer.peek(), 'a');
    EXPECT_EQ(lexer.peek(), 'a'); // لا تتقدم
}
```

#### 2. Integration Tests (اختبارات تكامل):
```cpp
// اختبار عدة مكونات معاً
TEST(IntegrationTest, LexerToParserFlow) {
    std::string code = "رقم س = 10";
    
    LexerCore lexer(code);
    auto tokens = lexer.tokenize();
    
    ParserCore parser(tokens);
    auto ast = parser.parse();
    
    ASSERT_NE(ast, nullptr);
    EXPECT_EQ(ast->getType(), NodeType::VARIABLE_DECL);
}
```

#### 3. Edge Case Tests (اختبارات حالات حدية):
```cpp
TEST(LexerTest, EmptyInput) {
    LexerCore lexer("");
    auto tokens = lexer.tokenize();
    EXPECT_EQ(tokens.size(), 1); // فقط EOF
    EXPECT_EQ(tokens[0].getType(), TokenType::EOF_TOKEN);
}

TEST(LexerTest, VeryLongString) {
    std::string long_str(10000, 'a');
    LexerCore lexer("\"" + long_str + "\"");
    EXPECT_NO_THROW({
        auto tokens = lexer.tokenize();
    });
}
```

#### 4. Error Handling Tests (اختبارات معالجة أخطاء):
```cpp
TEST(LexerTest, UnclosedString) {
    LexerCore lexer("\"unclosed");
    EXPECT_THROW({
        lexer.tokenize();
    }, std::runtime_error);
}

TEST(ParserTest, MissingRightParen) {
    std::vector<Token> tokens = {
        Token(TokenType::LEFT_PAREN, "(", 1, 1),
        Token(TokenType::INTEGER, "5", 1, 2),
        Token(TokenType::EOF_TOKEN, "", 1, 3)
    };
    ParserCore parser(tokens);
    EXPECT_THROW({
        parser.parse();
    }, ParseError);
}
```

---

### Assertions في Google Test

| Assertion | الاستخدام | مثال |
|-----------|----------|------|
| `EXPECT_EQ(a, b)` | a == b | `EXPECT_EQ(token.getType(), TokenType::INTEGER)` |
| `EXPECT_NE(a, b)` | a != b | `EXPECT_NE(result, nullptr)` |
| `EXPECT_LT(a, b)` | a < b | `EXPECT_LT(tokens.size(), 100)` |
| `EXPECT_GT(a, b)` | a > b | `EXPECT_GT(value, 0)` |
| `EXPECT_TRUE(x)` | x صحيح | `EXPECT_TRUE(token.isKeyword())` |
| `EXPECT_FALSE(x)` | x خاطئ | `EXPECT_FALSE(tokens.empty())` |
| `EXPECT_THROW(stmt, exc)` | يرمي استثناء | `EXPECT_THROW(parse(), ParseError)` |
| `EXPECT_NO_THROW(stmt)` | لا يرمي | `EXPECT_NO_THROW(lexer.advance())` |
| `ASSERT_*` | نفس EXPECT لكن يوقف الاختبار | `ASSERT_NE(ast, nullptr)` |

---

### تشغيل الاختبارات

```powershell
# بناء الاختبارات
cd C:\s\s_language\build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DDEBUG=ON
cmake --build . --config Debug

# تشغيل كل الاختبارات
ctest --output-on-failure -C Debug

# تشغيل اختبارات محددة
ctest -R lexer --output-on-failure

# تشغيل مع تفاصيل
ctest --verbose

# تشغيل الـ executable مباشرة
.\Debug\sad_tests.exe

# تشغيل اختبار واحد
.\Debug\sad_tests.exe --gtest_filter=LexerTest.TokenizeInteger

# قائمة الاختبارات
.\Debug\sad_tests.exe --gtest_list_tests
```

---

## 📊 تغطية الكود (Code Coverage)

### الأهداف

| المكون | التغطية المستهدفة |
|--------|-------------------|
| Lexer | ≥ 90% |
| Parser | ≥ 85% |
| Interpreter | ≥ 80% |
| Data Layer | ≥ 85% |
| StdLib | ≥ 75% |
| **الإجمالي** | **≥ 80%** |

---

### قياس التغطية (Windows - gcov/lcov بديل)

#### باستخدام OpenCppCoverage:

```powershell
# تثبيت OpenCppCoverage
# من https://github.com/OpenCppCoverage/OpenCppCoverage/releases

# تشغيل مع قياس تغطية
OpenCppCoverage.exe --sources C:\s\s_language\src `
                    --sources C:\s\s_language\include `
                    --export_type html:coverage `
                    -- .\Debug\sad_tests.exe

# فتح التقرير
Start-Process coverage\index.html
```

---

### تحليل التغطية

#### ✅ تغطية جيدة (90%+):
```
File: lexer_core.cpp
Lines: 250/260 (96.2%)
Functions: 18/18 (100%)
Branches: 85/90 (94.4%)
```

#### ⚠️ تغطية مقبولة (75-89%):
```
File: parser_core.cpp
Lines: 200/250 (80.0%)
Functions: 15/18 (83.3%)
Branches: 70/100 (70.0%)
```

#### ❌ تغطية منخفضة (<75%):
```
File: graphics_lib.cpp
Lines: 50/150 (33.3%)
Functions: 5/15 (33.3%)
```
**→ يحتاج المزيد من الاختبارات!**

---

## 🔍 التحليل الثابت (Static Analysis)

### clang-tidy

#### التكوين (.clang-tidy):

```yaml
# C:\s\s_language\.clang-tidy
Checks: >
  *,
  -modernize-use-trailing-return-type,
  -readability-magic-numbers,
  -cppcoreguidelines-avoid-magic-numbers

CheckOptions:
  - key: readability-identifier-naming.ClassCase
    value: CamelCase
  - key: readability-identifier-naming.FunctionCase
    value: camelCase
  - key: readability-identifier-naming.VariableCase
    value: lower_case
```

#### تشغيل clang-tidy:

```powershell
# على ملف واحد
clang-tidy src/lexer/lexer_core.cpp -- -I include/

# على كل الملفات (مع compile_commands.json)
cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
clang-tidy -p . ../src/**/*.cpp

# إصلاح تلقائي (احذر!)
clang-tidy --fix src/lexer/lexer_core.cpp -- -I include/
```

---

### مشاكل شائعة يكتشفها clang-tidy:

#### 1. استخدام raw pointers:
```cpp
// ❌ سيء
Token* token = new Token(...);

// ✅ جيد
auto token = std::make_unique<Token>(...);
```

#### 2. Shadowing variables:
```cpp
// ❌ سيء
void func(int value) {
    int value = 5; // تظليل!
}

// ✅ جيد
void func(int value) {
    int local_value = 5;
}
```

#### 3. استخدام C-style casts:
```cpp
// ❌ سيء
double d = (double)int_value;

// ✅ جيد
double d = static_cast<double>(int_value);
```

---

## 📝 Checklist النهائية قبل PR

### Checklist شاملة:

```markdown
## Quality Checklist

### البناء والاختبارات
- [ ] `cmake --build build` يمر بدون أخطاء
- [ ] `ctest` جميع الاختبارات تمر
- [ ] لا توجد تحذيرات compiler
- [ ] DEBUG_PRINT يعمل عند تفعيل DEBUG

### التوثيق
- [ ] كل دالة عامة موثقة (AR+EN)
- [ ] أمثلة الاستخدام موجودة
- [ ] رأس الملف محدث
- [ ] التغييرات موثقة في CHANGELOG (إن وُجد)

### الكود
- [ ] clang-format مطبق
- [ ] الأسماء واضحة ومعبرة
- [ ] لا code smells (clang-tidy)
- [ ] معالجة الأخطاء مناسبة
- [ ] لا memory leaks

### الاختبارات
- [ ] اختبارات وحدة للدوال الجديدة
- [ ] اختبارات edge cases
- [ ] اختبارات error handling
- [ ] التغطية ≥ 80% للملفات الجديدة

### الجودة
- [ ] التوثيق ثنائي اللغة كامل
- [ ] DEBUG_PRINT في نقاط حرجة
- [ ] رسائل الأخطاء واضحة
- [ ] الكود يتبع نمط المشروع
```

---

## 🎯 ملخص المعايير

| المعيار | القيمة المستهدفة | الأولوية |
|---------|------------------|----------|
| **التغطية** | ≥ 80% | 🔴 عالية |
| **التوثيق** | 100% للدوال العامة | 🔴 عالية |
| **Compiler Warnings** | 0 | 🔴 عالية |
| **DEBUG_PRINT** | في الدوال الأساسية | 🟠 مهمة |
| **clang-tidy Issues** | 0 critical | 🟠 مهمة |
| **Code Style** | 100% متوافق | 🟡 مرغوبة |

---

**آخر تحديث:** 4 نوفمبر 2025  
**المرجع:** [الخطة الرئيسية](00_MASTER_PLAN.md) | [سير العمل](03_WORKFLOW_GUIDE.md)
