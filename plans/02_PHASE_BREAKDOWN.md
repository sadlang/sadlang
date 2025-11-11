# 📋 تقسيم المراحل والمهام التفصيلية

**تاريخ الإنشاء:** 4 نوفمبر 2025  
**آخر تحديث:** 4 نوفمبر 2025

---

## 🎯 نظرة عامة

تقسيم شامل لجميع مراحل التطوير مع مهام يومية قابلة للقياس.

---

## ⚙️ Phase 0: إعداد بيئة العمل (1-2 يوم)

### الهدف
تجهيز بيئة تطوير احترافية مع أدوات الجودة والاختبار.

### المهام التفصيلية

#### المهمة 0.1: تحديث CMakeLists.txt (2 ساعة)
**الملفات:**
- `C:\s\s_language\CMakeLists.txt`

**التغييرات المطلوبة:**
```cmake
# إضافة دعم C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# إضافة خيارات DEBUG
option(DEBUG "Enable debug output" OFF)
if(DEBUG)
    add_definitions(-DDEBUG=1)
endif()

# إضافة Google Test
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)
enable_testing()

# Target للاختبارات
add_executable(sad_tests
    tests/lexer_tests/test_token.cpp
    tests/lexer_tests/test_lexer.cpp
    # ... المزيد
)
target_link_libraries(sad_tests gtest_main)
```

**الأوامر:**
```powershell
cd C:\s\s_language
git checkout -b feat/setup/cmake-update
# تعديل CMakeLists.txt
git add CMakeLists.txt
git commit -m "[setup] update CMake with testing support"
```

**معايير القبول:**
- ✅ CMake يبني المشروع بنجاح
- ✅ خيار DEBUG يعمل
- ✅ Google Test متكامل

**الوقت:** 2 ساعة

---

#### المهمة 0.2: إضافة clang-format (1 ساعة)
**الملفات:**
- `C:\s\s_language\.clang-format` (جديد)

**المحتوى:**
```yaml
BasedOnStyle: Google
IndentWidth: 4
ColumnLimit: 100
AllowShortFunctionsOnASingleLine: Empty
AlignConsecutiveAssignments: true
```

**الأوامر:**
```powershell
# إنشاء الملف
New-Item -Path "C:\s\s_language\.clang-format" -ItemType File

# تطبيق التنسيق على كل الملفات
Get-ChildItem -Path src,include -Recurse -Include *.cpp,*.h | 
    ForEach-Object { clang-format -i $_.FullName }
```

**معايير القبول:**
- ✅ ملف .clang-format موجود
- ✅ جميع الملفات منسقة

**الوقت:** 1 ساعة

---

#### المهمة 0.3: إعداد GitHub Actions CI (2 ساعة)
**الملفات:**
- `C:\s\s_language\.github\workflows\ci.yml` (جديد)

**المحتوى:**
```yaml
name: CI Build and Test

on: [push, pull_request]

jobs:
  build:
    runs-on: windows-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup CMake
      uses: lukka/get-cmake@latest
    
    - name: Configure
      run: cmake -B build -DCMAKE_BUILD_TYPE=Debug -DDEBUG=ON
    
    - name: Build
      run: cmake --build build --config Debug
    
    - name: Test
      run: cd build && ctest --output-on-failure -C Debug
```

**معايير القبول:**
- ✅ CI يعمل على كل push
- ✅ الاختبارات تُشغل تلقائياً

**الوقت:** 2 ساعة

---

#### المهمة 0.4: إنشاء ملفات الاختبار الأولية (3 ساعات)
**الملفات:**
- `C:\s\s_language\tests\lexer_tests\test_token.cpp` (جديد)
- `C:\s\s_language\tests\lexer_tests\test_lexer.cpp` (جديد)

**مثال test_token.cpp:**
```cpp
#include <gtest/gtest.h>
#include "../../include/lexer/token.h"

using namespace Sad::Lexer;

TEST(TokenTest, CreateIntegerToken) {
    Token token(TokenType::INTEGER, "42", 1, 1);
    EXPECT_EQ(token.getType(), TokenType::INTEGER);
    EXPECT_EQ(token.getLexeme(), "42");
}

TEST(TokenTest, IsKeyword) {
    Token token(TokenType::FUNCTION_KEYWORD, "دالة", 1, 1);
    EXPECT_TRUE(token.isKeyword());
}

// المزيد من الاختبارات...
```

**الأوامر:**
```powershell
cd C:\s\s_language
git checkout -b feat/testing/initial-tests
# إنشاء ملفات الاختبار
mkdir tests\lexer_tests -Force
# كتابة الاختبارات
git add tests/
git commit -m "[testing] add initial lexer tests"
```

**معايير القبول:**
- ✅ 10+ اختبارات للـ Token
- ✅ 15+ اختبارات للـ Lexer
- ✅ جميع الاختبارات تمر

**الوقت:** 3 ساعات

---

### ملخص Phase 0

| المهمة | الوقت | الملفات | الأولوية |
|--------|-------|---------|----------|
| 0.1 CMake | 2 ساعة | 1 | 🔴 عالية جداً |
| 0.2 clang-format | 1 ساعة | 1 | 🟠 متوسطة |
| 0.3 CI | 2 ساعة | 1 | 🟠 متوسطة |
| 0.4 Tests | 3 ساعات | 2+ | 🔴 عالية |
| **الإجمالي** | **8 ساعات** | **5+** | - |

---

## 🌳 Phase 2: المحلل النحوي (Parser) (2 أسبوع)

### الأسبوع الأول: AST و Parser Core

#### المهمة 2.1: تصميم AST Nodes الأساسية (يوم 1)

##### 2.1.1: ASTNode Base Class (3 ساعات)
**الملفات:**
- `include/parser/ast/ast_node.h`
- `src/parser/ast/ast_node.cpp`

**المحتوى المطلوب في ast_node.h:**
```cpp
#pragma once
#include <memory>
#include <string>
#include <vector>

namespace Sad {
namespace AST {

/**
 * @brief (AR) أنواع العقد في شجرة البناء النحوي
 * @brief (EN) Types of nodes in Abstract Syntax Tree
 */
enum class NodeType {
    // التعبيرات / Expressions
    LITERAL,           // قيمة ثابتة / Literal value
    IDENTIFIER,        // معرف / Identifier
    BINARY_OP,         // عملية ثنائية / Binary operation
    UNARY_OP,          // عملية أحادية / Unary operation
    CALL,              // استدعاء دالة / Function call
    
    // العبارات / Statements
    VARIABLE_DECL,     // تعريف متغير / Variable declaration
    ASSIGNMENT,        // تعيين قيمة / Assignment
    IF_STMT,           // عبارة شرطية / If statement
    WHILE_STMT,        // حلقة while / While loop
    FOR_STMT,          // حلقة for / For loop
    RETURN_STMT,       // عبارة إرجاع / Return statement
    FUNCTION_DECL,     // تعريف دالة / Function declaration
    
    // البرمجة الكائنية / OOP
    CLASS_DECL,        // تعريف صنف / Class declaration
    METHOD_CALL,       // استدعاء دالة في صنف / Method call
    
    // الرسومات / Graphics
    WIDGET_DECL,       // تعريف ويدجت / Widget declaration
    BUILD_METHOD       // دالة البناء / Build method
};

/**
 * @brief (AR) العقدة الأساسية في شجرة البناء النحوي
 * @brief (EN) Base node in Abstract Syntax Tree
 * 
 * جميع عقد AST ترث من هذه الفئة
 * All AST nodes inherit from this class
 */
class ASTNode {
public:
    virtual ~ASTNode() = default;
    
    /**
     * @brief (AR) الحصول على نوع العقدة
     * @brief (EN) Get the node type
     * @return (NodeType) نوع العقدة / Node type
     */
    virtual NodeType getType() const = 0;
    
    /**
     * @brief (AR) تحويل العقدة لنص للطباعة
     * @brief (EN) Convert node to string for printing
     * @return (std::string) تمثيل نصي / String representation
     */
    virtual std::string toString() const = 0;
    
    /**
     * @brief (AR) قبول زائر للمعالجة (نمط الزائر)
     * @brief (EN) Accept a visitor for processing (Visitor pattern)
     * @param visitor الزائر / The visitor
     */
    virtual void accept(class ASTVisitor& visitor) = 0;
};

// باقي التعريفات...

} // namespace AST
} // namespace Sad
```

**الأوامر:**
```powershell
cd C:\s\s_language
git checkout -b feat/parser/ast-base
# إنشاء الملفات
mkdir include\parser\ast -Force
mkdir src\parser\ast -Force
# كتابة الكود
git add include/parser/ast/ src/parser/ast/
git commit -m "[parser] add AST base node"
```

**معايير القبول:**
- ✅ ASTNode مُعرّف مع virtual methods
- ✅ NodeType enum كامل
- ✅ توثيق Doxygen ثنائي اللغة
- ✅ يبني بدون أخطاء

**الوقت:** 3 ساعات

---

##### 2.1.2: Expression Nodes (4 ساعات)
**الملفات:**
- `include/parser/ast/ast_expressions.h`
- `src/parser/ast/ast_expressions.cpp`

**العقد المطلوبة:**
- `LiteralExpr` - قيم ثابتة
- `IdentifierExpr` - معرفات
- `BinaryOpExpr` - عمليات ثنائية (+, -, *, /)
- `UnaryOpExpr` - عمليات أحادية (-, !)
- `CallExpr` - استدعاء دوال

**مثال LiteralExpr:**
```cpp
/**
 * @brief (AR) عقدة تعبير قيمة ثابتة
 * @brief (EN) Literal expression node
 */
class LiteralExpr : public Expression {
public:
    /**
     * @brief (AR) المنشئ
     * @brief (EN) Constructor
     * @param value (Value) القيمة / The value
     */
    explicit LiteralExpr(const Data::Value& value);
    
    NodeType getType() const override { return NodeType::LITERAL; }
    std::string toString() const override;
    void accept(ASTVisitor& visitor) override;
    
    const Data::Value& getValue() const { return value_; }
    
private:
    Data::Value value_;
};
```

**معايير القبول:**
- ✅ 5 فئات Expression محددة
- ✅ كل فئة موثقة بالكامل
- ✅ DEBUG_PRINT في المنشئات

**الوقت:** 4 ساعات

---

##### 2.1.3: Statement Nodes (4 ساعات)
**الملفات:**
- `include/parser/ast/ast_statements.h`
- `src/parser/ast/ast_statements.cpp`

**العقد المطلوبة:**
- `VariableDeclStmt` - تعريف متغير
- `AssignmentStmt` - تعيين قيمة
- `IfStmt` - شرط
- `WhileStmt` - حلقة while
- `ForStmt` - حلقة for
- `ReturnStmt` - إرجاع
- `FunctionDeclStmt` - تعريف دالة

**معايير القبول:**
- ✅ 7 فئات Statement محددة
- ✅ توثيق كامل
- ✅ يبني بنجاح

**الوقت:** 4 ساعات

---

#### ملخص يوم 1 (المهمة 2.1)

| المهمة الفرعية | الوقت | الملفات | الحالة |
|----------------|-------|---------|--------|
| 2.1.1 Base | 3 ساعات | 2 | 📝 |
| 2.1.2 Expressions | 4 ساعات | 2 | 📝 |
| 2.1.3 Statements | 4 ساعات | 2 | 📝 |
| **الإجمالي** | **11 ساعة** | **6** | - |

---

#### المهمة 2.2: Parser Core (يوم 2)

##### 2.2.1: Parser Base Class (4 ساعات)
**الملفات:**
- `include/parser/parser_core.h`
- `src/parser/parser_core.cpp`

**الدوال الأساسية المطلوبة:**
```cpp
class ParserCore {
public:
    explicit ParserCore(std::vector<Token> tokens);
    
    // تحليل البرنامج الكامل
    std::unique_ptr<AST::ASTNode> parse();
    
private:
    std::vector<Token> tokens_;
    size_t current_;
    
    // دوال مساعدة
    Token peek() const;              // رؤية الرمز الحالي
    Token peekNext() const;          // رؤية الرمز التالي
    Token advance();                 // التقدم للرمز التالي
    bool check(TokenType type) const;// التحقق من النوع
    bool match(TokenType type);      // المطابقة والتقدم
    Token consume(TokenType type, const std::string& msg); // استهلاك رمز
    
    bool isAtEnd() const;
    void synchronize();              // استعادة التزامن بعد خطأ
};
```

**معايير القبول:**
- ✅ جميع الدوال المساعدة منفذة
- ✅ معالجة أخطاء أولية
- ✅ DEBUG_PRINT في كل دالة

**الوقت:** 4 ساعات

---

##### 2.2.2: Parser Helper Methods (3 ساعات)
**الملف:**
- `src/parser/parser_core.cpp` (إكمال)

**الدوال الإضافية:**
```cpp
// معالجة الأخطاء
void ParserCore::reportError(const std::string& message);
void ParserCore::synchronize();

// التحقق المتقدم
bool ParserCore::matchAny(const std::vector<TokenType>& types);
Token ParserCore::previous() const;
```

**معايير القبول:**
- ✅ معالجة أخطاء نحوية واضحة
- ✅ رسائل خطأ بالعربية
- ✅ اختبارات أولية

**الوقت:** 3 ساعات

---

#### ملخص يوم 2 (المهمة 2.2)

| المهمة الفرعية | الوقت | الحالة |
|----------------|-------|--------|
| 2.2.1 Base | 4 ساعات | 📝 |
| 2.2.2 Helpers | 3 ساعات | 📝 |
| **الإجمالي** | **7 ساعات** | - |

---

#### المهمة 2.3: تحليل التعبيرات (أيام 3-4)

##### 2.3.1: Expression Parsing - Primary (يوم 3 - 4 ساعات)
**الملف:**
- `src/parser/parser_expressions.cpp` (جديد)

**الدوال:**
```cpp
/**
 * @brief (AR) تحليل تعبير أساسي (literal, identifier, grouped)
 * @brief (EN) Parse primary expression
 * @return (unique_ptr<Expression>) العقدة الناتجة / Resulting node
 */
std::unique_ptr<Expression> ParserCore::parsePrimary() {
    DEBUG_PRINT("Parsing primary expression");
    
    // رقم
    if (match(TokenType::INTEGER) || match(TokenType::DOUBLE)) {
        return std::make_unique<LiteralExpr>(/* ... */);
    }
    
    // نص
    if (match(TokenType::STRING)) {
        return std::make_unique<LiteralExpr>(/* ... */);
    }
    
    // معرف
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<IdentifierExpr>(/* ... */);
    }
    
    // تعبير بين أقواس
    if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }
    
    throw error("Expected expression");
}
```

**معايير القبول:**
- ✅ يحلل literals صحيح
- ✅ يحلل identifiers
- ✅ يحلل تعبيرات بأقواس
- ✅ اختبارات تمر

**الوقت:** 4 ساعات

---

##### 2.3.2: Binary Operations (يوم 3-4 - 6 ساعات)
**الملف:**
- `src/parser/parser_expressions.cpp` (إكمال)

**تطبيق أولوية العوامل (Operator Precedence):**
```cpp
// الأولوية من الأقل للأعلى:
// 1. OR (||)
// 2. AND (&&)
// 3. EQUALITY (==, !=)
// 4. COMPARISON (<, >, <=, >=)
// 5. TERM (+, -)
// 6. FACTOR (*, /)
// 7. UNARY (-, !)
// 8. PRIMARY

std::unique_ptr<Expression> parseExpression() {
    return parseOr();
}

std::unique_ptr<Expression> parseOr() {
    auto expr = parseAnd();
    while (match(TokenType::OR)) {
        Token op = previous();
        auto right = parseAnd();
        expr = std::make_unique<BinaryOpExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

// ... باقي مستويات الأولوية
```

**معايير القبول:**
- ✅ أولوية العوامل صحيحة
- ✅ يحلل تعبيرات معقدة
- ✅ اختبارات شاملة

**الوقت:** 6 ساعات

---

##### 2.3.3: Function Calls (يوم 4 - 3 ساعات)
**الملف:**
- `src/parser/parser_expressions.cpp` (إكمال)

```cpp
std::unique_ptr<Expression> parseCall() {
    auto expr = parsePrimary();
    
    while (true) {
        if (match(TokenType::LEFT_PAREN)) {
            expr = finishCall(std::move(expr));
        } else {
            break;
        }
    }
    
    return expr;
}

std::unique_ptr<Expression> finishCall(std::unique_ptr<Expression> callee) {
    std::vector<std::unique_ptr<Expression>> arguments;
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            arguments.push_back(parseExpression());
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments");
    
    return std::make_unique<CallExpr>(std::move(callee), std::move(arguments));
}
```

**معايير القبول:**
- ✅ يحلل استدعاءات دوال
- ✅ يدعم معاملات متعددة
- ✅ اختبارات تمر

**الوقت:** 3 ساعات

---

#### ملخص أيام 3-4 (المهمة 2.3)

| اليوم | المهمة | الوقت | الحالة |
|-------|--------|-------|--------|
| 3 | Primary + Binary (جزء) | 8 ساعات | 📝 |
| 4 | Binary (تكملة) + Calls | 5 ساعات | 📝 |
| **الإجمالي** | | **13 ساعة** | - |

---

#### المهمة 2.4: تحليل العبارات (أيام 5-7)

##### 2.4.1: Variable Declaration & Assignment (يوم 5)
**الملف:**
- `src/parser/parser_statements.cpp` (جديد)

```cpp
std::unique_ptr<Statement> ParserCore::parseVariableDecl() {
    // رقم س = 10
    // نص اسم = "أحمد"
    
    Token type = advance(); // نوع المتغير
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    std::unique_ptr<Expression> initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = parseExpression();
    }
    
    return std::make_unique<VariableDeclStmt>(type, name, std::move(initializer));
}

std::unique_ptr<Statement> ParserCore::parseAssignment() {
    // س = 20
    Token name = previous();
    consume(TokenType::EQUAL, "Expected '=' in assignment");
    auto value = parseExpression();
    
    return std::make_unique<AssignmentStmt>(name, std::move(value));
}
```

**الوقت:** 4 ساعات

---

##### 2.4.2: If Statement (يوم 5-6)
**الملف:**
- `src/parser/parser_statements.cpp` (إكمال)

```cpp
std::unique_ptr<Statement> ParserCore::parseIfStatement() {
    // إذا شرط
    //     عبارات
    // وإلا_إذا شرط
    //     عبارات
    // وإلا
    //     عبارات
    // نهاية
    
    consume(TokenType::IF_KEYWORD, "Expected 'إذا'");
    auto condition = parseExpression();
    
    std::vector<std::unique_ptr<Statement>> thenBranch;
    while (!check(TokenType::ELSE_IF_KEYWORD) && 
           !check(TokenType::ELSE_KEYWORD) && 
           !check(TokenType::END_KEYWORD)) {
        thenBranch.push_back(parseStatement());
    }
    
    // معالجة elif و else...
    
    consume(TokenType::END_KEYWORD, "Expected 'نهاية' after if");
    
    return std::make_unique<IfStmt>(/* ... */);
}
```

**الوقت:** 4 ساعات

---

##### 2.4.3: Loops (يوم 6)
**الملف:**
- `src/parser/parser_statements.cpp` (إكمال)

```cpp
// While loop
std::unique_ptr<Statement> ParserCore::parseWhileStatement() {
    // بينما شرط
    //     عبارات
    // نهاية
}

// For loop
std::unique_ptr<Statement> ParserCore::parseForStatement() {
    // لكل عنصر في مصفوفة
    //     عبارات
    // نهاية
}
```

**الوقت:** 4 ساعات

---

##### 2.4.4: Return Statement (يوم 7)
**الملف:**
- `src/parser/parser_statements.cpp` (إكمال)

```cpp
std::unique_ptr<Statement> ParserCore::parseReturnStatement() {
    // إرجاع قيمة
    consume(TokenType::RETURN_KEYWORD, "Expected 'إرجاع'");
    
    std::unique_ptr<Expression> value = nullptr;
    if (!check(TokenType::NEWLINE) && !check(TokenType::END_KEYWORD)) {
        value = parseExpression();
    }
    
    return std::make_unique<ReturnStmt>(std::move(value));
}
```

**الوقت:** 2 ساعة

---

#### ملخص أيام 5-7 (المهمة 2.4)

| اليوم | المهمة | الوقت | الحالة |
|-------|--------|-------|--------|
| 5 | VarDecl + Assignment + If (جزء) | 8 ساعات | 📝 |
| 6 | If (تكملة) + Loops | 8 ساعات | 📝 |
| 7 | Return + Testing | 6 ساعات | 📝 |
| **الإجمالي** | | **22 ساعة** | - |

---

#### المهمة 2.5: Function Parsing (أيام 8-9)

##### 2.5.1: Function Declaration (يوم 8)
**الملف:**
- `src/parser/parser_functions.cpp` (جديد)

```cpp
std::unique_ptr<Statement> ParserCore::parseFunctionDecl() {
    // دالة اسم_الدالة(نوع معامل1, نوع معامل2) = نوع_الإرجاع
    //     عبارات
    //     إرجاع قيمة
    // نهاية
    
    consume(TokenType::FUNCTION_KEYWORD, "Expected 'دالة'");
    Token name = consume(TokenType::IDENTIFIER, "Expected function name");
    
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name");
    
    // تحليل المعاملات
    std::vector<Parameter> parameters;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            Token paramType = advance();
            Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name");
            parameters.push_back({paramType, paramName});
        } while (match(TokenType::COMMA));
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters");
    
    // نوع الإرجاع
    Token returnType;
    if (match(TokenType::EQUAL)) {
        returnType = advance();
    }
    
    // الجسم
    std::vector<std::unique_ptr<Statement>> body;
    while (!check(TokenType::END_KEYWORD)) {
        body.push_back(parseStatement());
    }
    
    consume(TokenType::END_KEYWORD, "Expected 'نهاية' after function");
    
    return std::make_unique<FunctionDeclStmt>(name, parameters, returnType, std::move(body));
}
```

**معايير القبول:**
- ✅ يحلل تعريفات الدوال
- ✅ يدعم معاملات متعددة
- ✅ يحلل أنواع الإرجاع
- ✅ اختبارات شاملة

**الوقت:** 6 ساعات

---

##### 2.5.2: اختبارات Parser (يوم 9)
**الملفات:**
- `tests/parser_tests/test_expressions.cpp` (جديد)
- `tests/parser_tests/test_statements.cpp` (جديد)
- `tests/parser_tests/test_functions.cpp` (جديد)

**أمثلة الاختبارات:**
```cpp
TEST(ParserTest, ParseSimpleExpression) {
    std::vector<Token> tokens = {
        Token(TokenType::INTEGER, "5", 1, 1),
        Token(TokenType::PLUS, "+", 1, 3),
        Token(TokenType::INTEGER, "3", 1, 5),
        Token(TokenType::EOF_TOKEN, "", 1, 6)
    };
    
    ParserCore parser(tokens);
    auto ast = parser.parse();
    
    EXPECT_NE(ast, nullptr);
    EXPECT_EQ(ast->getType(), NodeType::BINARY_OP);
}

TEST(ParserTest, ParseVariableDeclaration) {
    // رقم س = 10
    std::vector<Token> tokens = {
        Token(TokenType::INTEGER_TYPE, "رقم", 1, 1),
        Token(TokenType::IDENTIFIER, "س", 1, 5),
        Token(TokenType::EQUAL, "=", 1, 7),
        Token(TokenType::INTEGER, "10", 1, 9),
        Token(TokenType::EOF_TOKEN, "", 1, 11)
    };
    
    ParserCore parser(tokens);
    auto ast = parser.parse();
    
    EXPECT_EQ(ast->getType(), NodeType::VARIABLE_DECL);
}

// 20+ اختبار إضافي...
```

**معايير القبول:**
- ✅ 30+ اختبار للـ Parser
- ✅ تغطية ≥ 85%
- ✅ جميع الاختبارات تمر

**الوقت:** 8 ساعات

---

### ملخص كامل Phase 2 (Parser)

| الأسبوع | الأيام | المهام | الوقت الكلي | الحالة |
|---------|--------|---------|-------------|--------|
| 1 | 1-2 | AST + Parser Core | 18 ساعة | 📝 |
| 1 | 3-4 | Expressions | 13 ساعة | 📝 |
| 2 | 5-7 | Statements | 22 ساعة | 📝 |
| 2 | 8-9 | Functions + Tests | 14 ساعة | 📝 |
| **الإجمالي** | **9 أيام** | **4 مراحل** | **67 ساعة** | - |

---

## 💾 Phase 3: نظام البيانات (Data Layer) (1 أسبوع)

### المهمة 3.1: Value System (يومان)

#### 3.1.1: Value Class (يوم 1 - 6 ساعات)
**الملفات:**
- `include/data/types/value.h`
- `src/data/types/value.cpp`

**المتطلبات:**
```cpp
class Value {
public:
    // المنشئات لكل نوع
    Value();                          // VOID
    explicit Value(int val);          // INTEGER
    explicit Value(double val);       // DOUBLE
    explicit Value(const std::string& val);  // STRING
    explicit Value(bool val);         // BOOLEAN
    
    // الحصول على النوع
    ValueType getType() const;
    
    // التحويل
    int toInt() const;
    double toDouble() const;
    std::string toString() const;
    bool toBool() const;
    
    // العمليات الحسابية
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    Value operator%(const Value& other) const;
    
    // العمليات المنطقية
    Value operator==(const Value& other) const;
    Value operator!=(const Value& other) const;
    Value operator<(const Value& other) const;
    Value operator>(const Value& other) const;
    Value operator<=(const Value& other) const;
    Value operator>=(const Value& other) const;
    Value operator&&(const Value& other) const;
    Value operator||(const Value& other) const;
    Value operator!() const;
    
private:
    ValueType type_;
    std::variant<std::monostate, int, double, std::string, bool> data_;
};
```

**معايير القبول:**
- ✅ جميع العمليات منفذة
- ✅ معالجة أخطاء الأنواع
- ✅ توثيق كامل
- ✅ اختبارات شاملة (30+)

**الوقت:** 6 ساعات

---

#### 3.1.2: Array Type (يوم 2 - 4 ساعات)
**الملفات:**
- `include/data/types/array.h`
- `src/data/types/array.cpp`

**الوقت:** 4 ساعات

---

### المهمة 3.2: Variable Manager (يومان)

#### 3.2.1: Scope Manager (يوم 3 - 5 ساعات)
**الملفات:**
- `include/data/scope/scope_manager.h`
- `src/data/scope/scope_manager.cpp`

```cpp
class ScopeManager {
public:
    void pushScope();               // إنشاء نطاق جديد
    void popScope();                // إزالة النطاق الحالي
    size_t getCurrentDepth() const; // عمق النطاق الحالي
    
private:
    std::vector<Scope> scopes_;
};
```

**الوقت:** 5 ساعات

---

#### 3.2.2: Variable Manager (يوم 4 - 6 ساعات)
**الملفات:**
- `include/data/managers/variable_manager.h`
- `src/data/managers/variable_manager.cpp`

```cpp
class VariableManager {
public:
    explicit VariableManager(std::shared_ptr<ScopeManager> scope_mgr);
    
    void define(const std::string& name, const Value& value);
    void assign(const std::string& name, const Value& value);
    Value get(const std::string& name) const;
    bool exists(const std::string& name) const;
    void remove(const std::string& name);
    
private:
    std::shared_ptr<ScopeManager> scope_mgr_;
    std::vector<std::unordered_map<std::string, Value>> scopes_;
};
```

**الوقت:** 6 ساعات

---

### المهمة 3.3: Function Manager (يوم 5)

**الملفات:**
- `include/data/managers/function_manager.h`
- `src/data/managers/function_manager.cpp`

```cpp
struct FunctionDefinition {
    std::string name;
    std::vector<Parameter> parameters;
    TokenType returnType;
    std::unique_ptr<AST::Statement> body;
};

class FunctionManager {
public:
    void define(const FunctionDefinition& func);
    FunctionDefinition* get(const std::string& name);
    bool exists(const std::string& name) const;
    
private:
    std::unordered_map<std::string, FunctionDefinition> functions_;
};
```

**الوقت:** 6 ساعات

---

### ملخص Phase 3

| اليوم | المهمة | الوقت | الحالة |
|-------|--------|-------|--------|
| 1 | Value Class | 6 ساعات | 📝 |
| 2 | Array + Tests | 4 ساعات | 📝 |
| 3 | Scope Manager | 5 ساعات | 📝 |
| 4 | Variable Manager | 6 ساعات | 📝 |
| 5 | Function Manager + Tests | 6 ساعات | 📝 |
| **الإجمالي** | **5 أيام** | **27 ساعة** | - |

---

## ⚙️ Phase 4: المفسر التنفيذي (Interpreter) (1.5 أسبوع)

_[سيتم تفصيله في تحديث لاحق - نفس مستوى التفصيل]_

---

## 📚 Phase 5-9: المراحل المتبقية

_[سيتم تفصيلها تباعاً مع التقدم في التطوير]_

---

## 📊 ملخص إجمالي

| المرحلة | المدة | الملفات | الأسطر | الحالة |
|---------|-------|---------|--------|--------|
| Phase 0 | 1 يوم | 5+ | ~500 | ⏳ |
| Phase 1 | أسبوع | 6 | 2,450 | ✅ |
| Phase 2 | أسبوعان | 15 | 2,600 | 📝 |
| Phase 3 | أسبوع | 14 | 2,570 | 📝 |
| Phase 4 | 1.5 أسبوع | 15 | 3,090 | 📝 |
| Phase 5+ | 6 أسابيع | 40+ | 8,000+ | 📝 |
| **الإجمالي** | **12 أسبوع** | **~90** | **~19,000** | - |

---

**آخر تحديث:** 4 نوفمبر 2025  
**المرجع:** [الخطة الرئيسية](00_MASTER_PLAN.md) | [قائمة الملفات](01_FILES_INVENTORY.md)
