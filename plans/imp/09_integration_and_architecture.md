# 🔗 خطة التكامل والمعمارية / Integration and Architecture Plan

**التاريخ:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** تخطيط تفصيلي

---

## 📋 ملخص تنفيذي / Executive Summary

### (AR) الملخص
هذا الملف يحدد كيفية دمج جميع مكونات لغة "ص" (Lexer، Parser، Interpreter، StdLib، Graphics) في نظام متماسك وقابل للتوسع. يركز على:
- واجهات التبادل بين المكونات
- تدفق البيانات من الكود المصدري إلى التنفيذ
- معايير التصميم لتقليل الاقتران وتسهيل الإضافات
- نظام إدارة الأخطاء الموحد
- استراتيجية الاختبار المتكاملة

### (EN) Summary
Defines integration strategy for all Sad language components (Lexer, Parser, Interpreter, StdLib, Graphics) into a cohesive, extensible system. Focuses on component interfaces, data flow, design patterns for loose coupling, unified error handling, and integrated testing strategy.

---

## 🎯 الأهداف / Objectives

### الأهداف الرئيسية:
1. ✅ تحديد واجهات واضحة بين المكونات
2. ✅ ضمان تدفق بيانات سلس من Lexer → Parser → Interpreter
3. ✅ تصميم نظام Value موحد لجميع أنواع البيانات
4. ✅ إنشاء نظام أخطاء ثنائي اللغة متسق
5. ✅ وضع معايير للتوسع المستقبلي
6. ✅ تحديد نقاط الامتداد (Extension Points)

### النتائج المتوقعة:
- Architecture document شامل
- Interface definitions في headers
- Data flow diagrams
- Error handling guidelines
- Testing integration strategy

---

## 📦 الاعتماديات / Dependencies

### من language_spec:
- ✅ `00_INDEX.md` - فهم عام للغة
- ✅ `01_types.md` - نظام الأنواع
- ✅ `02_functions.md` - نظام الدوال
- ✅ `03_oop.md` - نظام الكائنات

### من plans الحالية:
- ✅ `00_MASTER_PLAN.md` - الرؤية العامة
- ✅ `01_FILES_INVENTORY.md` - قائمة الملفات
- ✅ `02_PHASE_BREAKDOWN.md` - تقسيم المراحل
- ✅ `04_QUALITY_STANDARDS.md` - معايير الجودة

---

## 🏗️ معمارية النظام / System Architecture

### 1. التدفق الكلي / Overall Flow

```
┌─────────────────────────────────────────────────────┐
│                  Source Code (.s)                    │
│                   "رقم س = 5"                        │
└────────────────────┬────────────────────────────────┘
                     │
                     ▼
┌─────────────────────────────────────────────────────┐
│                   LEXER LAYER                        │
│  ┌──────────────────────────────────────────────┐  │
│  │  LexerCore::tokenize()                       │  │
│  │  - Unicode/RTL support                       │  │
│  │  - Keyword recognition (عربي/latin)         │  │
│  │  - Number/String/Identifier parsing          │  │
│  └──────────────────┬───────────────────────────┘  │
└─────────────────────┼───────────────────────────────┘
                      │ vector<Token>
                      ▼
┌─────────────────────────────────────────────────────┐
│                   PARSER LAYER                       │
│  ┌──────────────────────────────────────────────┐  │
│  │  ParserCore::parse()                         │  │
│  │  - Grammar validation (EBNF)                 │  │
│  │  - AST construction                          │  │
│  │  - Error recovery                            │  │
│  └──────────────────┬───────────────────────────┘  │
└─────────────────────┼───────────────────────────────┘
                      │ unique_ptr<AST::Program>
                      ▼
┌─────────────────────────────────────────────────────┐
│                 INTERPRETER LAYER                    │
│  ┌──────────────────────────────────────────────┐  │
│  │  InterpreterCore::execute()                  │  │
│  │  - AST traversal (Visitor pattern)           │  │
│  │  - Scope management                          │  │
│  │  - Value evaluation                          │  │
│  └──────────────────┬───────────────────────────┘  │
└─────────────────────┼───────────────────────────────┘
                      │ Value results
                      ▼
┌─────────────────────────────────────────────────────┐
│              DATA MANAGEMENT LAYER                   │
│  ┌────────────┬─────────────┬───────────────────┐  │
│  │ Variables  │  Functions  │    Classes        │  │
│  │  Manager   │   Manager   │    Manager        │  │
│  └────────────┴─────────────┴───────────────────┘  │
└─────────────────────┬───────────────────────────────┘
                      │
    ┌─────────────────┼─────────────────┐
    │                 │                 │
    ▼                 ▼                 ▼
┌─────────┐    ┌──────────┐    ┌──────────────┐
│ StdLib  │    │ Graphics │    │   Output     │
│ Modules │    │ Subsystem│    │ "س = 5"      │
└─────────┘    └──────────┘    └──────────────┘
```

### 2. الواجهات الأساسية / Core Interfaces

#### 2.1 Token Interface
```cpp
// include/lexer/token.h
namespace Sad {
namespace Lexer {

struct Position {
    size_t line;
    size_t column;
    size_t offset;
    std::string filename;
};

enum class TokenType {
    // Keywords
    KEYWORD_TYPE_INT,      // رقم
    KEYWORD_TYPE_FLOAT,    // عشري
    KEYWORD_IF,            // إذا
    // ... (90+ types)
    
    // Literals
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    
    // Operators
    PLUS, MINUS, MULTIPLY,
    
    END_OF_FILE,
    ERROR
};

class Token {
public:
    TokenType type;
    std::string lexeme;
    Position position;
    std::variant<int64_t, double, std::string> value;
    
    // Methods
    bool isKeyword() const;
    bool isLiteral() const;
    bool isOperator() const;
    std::string toString() const;
};

} // namespace Lexer
} // namespace Sad
```

#### 2.2 AST Node Interface
```cpp
// include/parser/ast/ast_node.h
namespace Sad {
namespace Parser {
namespace AST {

enum class NodeType {
    PROGRAM,
    EXPRESSION,
    STATEMENT,
    DECLARATION,
    // ...
};

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual NodeType getType() const = 0;
    virtual std::string toString() const = 0;
    virtual void accept(ASTVisitor* visitor) = 0;
    
    Position position;  // من Lexer
};

class Expression : public ASTNode {
public:
    virtual Data::Value evaluate(InterpreterCore* interpreter) = 0;
};

class Statement : public ASTNode {
public:
    virtual void execute(InterpreterCore* interpreter) = 0;
};

} // namespace AST
} // namespace Parser
} // namespace Sad
```

#### 2.3 Value Interface (نظام القيم الموحد)
```cpp
// include/data/types/value.h
namespace Sad {
namespace Data {

enum class ValueType {
    INTEGER,    // رقم
    FLOAT,      // عشري
    BOOLEAN,    // منطقي
    STRING,     // نص
    ARRAY,      // مصفوفة
    MAP,        // خريطة
    FUNCTION,   // دالة
    OBJECT,     // كائن
    NONE        // لاشيء
};

class Value {
public:
    ValueType type;
    
    // Union لتوفير الذاكرة
    union {
        int64_t intValue;
        double floatValue;
        bool boolValue;
    };
    
    // Smart pointers للأنواع المعقدة
    std::shared_ptr<std::string> stringValue;
    std::shared_ptr<std::vector<Value>> arrayValue;
    std::shared_ptr<std::map<std::string, Value>> mapValue;
    
    // Constructors
    Value();
    Value(int64_t val);
    Value(double val);
    Value(bool val);
    Value(const std::string& val);
    
    // Type checking
    bool isInteger() const { return type == ValueType::INTEGER; }
    bool isNumeric() const { return type == ValueType::INTEGER || type == ValueType::FLOAT; }
    
    // Conversions
    int64_t toInteger() const;
    double toFloat() const;
    std::string toString() const;
    bool toBoolean() const;
    
    // Operations
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    bool operator==(const Value& other) const;
};

} // namespace Data
} // namespace Sad
```

---

## 📁 الملفات المطلوبة / Required Files

### Core Integration Files

```
include/
├── core/
│   ├── sad_core.h              # ترويسة رئيسية موحدة
│   ├── interfaces.h            # جميع الواجهات المشتركة
│   └── common_types.h          # الأنواع المشتركة
├── lexer/
│   ├── token.h                 # ✅ موجود - تحديث
│   ├── lexer_interface.h       # NEW - واجهة Lexer
│   └── lexer_core.h            # ✅ موجود - تحديث
├── parser/
│   ├── parser_interface.h      # NEW - واجهة Parser
│   ├── ast/
│   │   ├── ast_node.h          # ✅ موجود - تحديث
│   │   ├── ast_visitor.h       # NEW - Visitor pattern
│   │   └── ast_factory.h       # NEW - Factory pattern
│   └── parser_core.h           # ✅ موجود - تحديث
├── interpreter/
│   ├── interpreter_interface.h # NEW - واجهة Interpreter
│   ├── core/
│   │   ├── interpreter_core.h  # ✅ موجود - تحديث
│   │   └── execution_context.h # NEW - سياق التنفيذ
│   └── visitors/
│       └── eval_visitor.h      # NEW - تقييم AST
├── data/
│   ├── types/
│   │   ├── value.h             # ✅ موجود - تحديث جذري
│   │   ├── type_system.h       # NEW - نظام الأنواع
│   │   └── conversions.h       # NEW - تحويلات الأنواع
│   └── managers/
│       ├── scope_manager.h     # NEW - إدارة النطاقات
│       ├── variable_manager.h  # ✅ موجود - تحديث
│       └── function_manager.h  # NEW
└── errors/
    ├── error_interface.h       # NEW - واجهة الأخطاء
    ├── error_manager.h         # ✅ موجود - تحديث
    └── error_codes.h           # NEW - أكواد الأخطاء

src/
├── core/
│   └── sad_engine.cpp          # NEW - محرك رئيسي
├── lexer/
│   └── (ملفات موجودة)
├── parser/
│   ├── ast/
│   │   ├── ast_visitor.cpp     # NEW
│   │   └── ast_factory.cpp     # NEW
│   └── parser_core.cpp         # ✅ موجود - تحديث
├── interpreter/
│   ├── core/
│   │   ├── interpreter_core.cpp # ✅ موجود - تحديث
│   │   └── execution_context.cpp # NEW
│   └── visitors/
│       └── eval_visitor.cpp    # NEW
└── data/
    ├── types/
    │   ├── value.cpp           # ✅ موجود - تحديث جذري
    │   └── conversions.cpp     # NEW
    └── managers/
        └── scope_manager.cpp   # NEW
```

---

## 🔧 تقسيم العمل / Task Breakdown

### Sprint 1: Core Interfaces (أسبوع 1-2)

#### Task 1.1: تحديث Token System
**المدة:** 2 أيام  
**الملفات:**
- `include/lexer/token.h`
- `src/lexer/token.cpp`

**الخطوات:**
1. إضافة `Position` struct كامل
2. إضافة `value` variant للقيم
3. إضافة utility methods
4. توثيق Doxygen ثنائي اللغة

**معايير القبول:**
- ✅ Build successful
- ✅ Token creation tests pass
- ✅ Position tracking works
- ✅ Doxygen generates docs

**Debug Points:**
```cpp
DEBUG_PRINT("Token created: type=%d, lexeme='%s', line=%zu", 
            static_cast<int>(type), lexeme.c_str(), position.line);
```

---

#### Task 1.2: إنشاء Value System الموحد
**المدة:** 3 أيام  
**الملفات:**
- `include/data/types/value.h`
- `src/data/types/value.cpp`
- `include/data/types/conversions.h`
- `src/data/types/conversions.cpp`

**الخطوات:**
1. تعريف `ValueType` enum
2. تنفيذ `Value` class مع union
3. إضافة constructors لكل نوع
4. تنفيذ type conversions
5. تنفيذ operators (+, -, *, /, ==, !=, etc.)
6. إضافة utility methods

**مثال Implementation:**
```cpp
// include/data/types/value.h
namespace Sad::Data {

class Value {
public:
    ValueType type;
    
    // Constructors
    Value() : type(ValueType::NONE) {}
    Value(int64_t val) : type(ValueType::INTEGER), intValue(val) {}
    Value(double val) : type(ValueType::FLOAT), floatValue(val) {}
    Value(const std::string& val) 
        : type(ValueType::STRING), 
          stringValue(std::make_shared<std::string>(val)) {}
    
    // Arithmetic operators
    Value operator+(const Value& other) const {
        if (type == ValueType::INTEGER && other.type == ValueType::INTEGER) {
            return Value(intValue + other.intValue);
        }
        if (isNumeric() && other.isNumeric()) {
            return Value(toFloat() + other.toFloat());
        }
        if (type == ValueType::STRING && other.type == ValueType::STRING) {
            return Value(*stringValue + *other.stringValue);
        }
        throw TypeError("Cannot add types");
    }
    
    // Type checking
    bool isInteger() const { return type == ValueType::INTEGER; }
    bool isNumeric() const { 
        return type == ValueType::INTEGER || type == ValueType::FLOAT; 
    }
    
    // Conversions
    double toFloat() const {
        if (type == ValueType::INTEGER) return static_cast<double>(intValue);
        if (type == ValueType::FLOAT) return floatValue;
        throw TypeError("Cannot convert to float");
    }
};

} // namespace Sad::Data
```

**Debug Points:**
```cpp
DEBUG_PRINT("Value created: type=%d", static_cast<int>(type));
DEBUG_PRINT("Value operation: %s + %s = %s", 
            lhs.toString().c_str(), rhs.toString().c_str(), 
            result.toString().c_str());
```

**الاختبارات:**
```cpp
// tests/data_tests/value_test.cpp
TEST(ValueTest, IntegerCreation) {
    Value v(42);
    EXPECT_EQ(v.type, ValueType::INTEGER);
    EXPECT_EQ(v.toInteger(), 42);
}

TEST(ValueTest, Arithmetic) {
    Value a(5);
    Value b(3);
    Value result = a + b;
    EXPECT_EQ(result.toInteger(), 8);
}

TEST(ValueTest, TypeConversion) {
    Value i(10);
    Value f = i.toFloat();
    EXPECT_EQ(f.type, ValueType::FLOAT);
    EXPECT_DOUBLE_EQ(f.toFloat(), 10.0);
}

TEST(ValueTest, StringConcatenation) {
    Value s1("مرحبا");
    Value s2(" بك");
    Value result = s1 + s2;
    EXPECT_EQ(result.toString(), "مرحبا بك");
}
```

**معايير القبول:**
- ✅ جميع constructors تعمل
- ✅ العمليات الحسابية صحيحة
- ✅ Type conversions آمنة
- ✅ String operations تدعم Unicode
- ✅ Coverage ≥ 85%

---

#### Task 1.3: إنشاء AST Visitor Pattern
**المدة:** 2 أيام  
**الملفات:**
- `include/parser/ast/ast_visitor.h`
- `src/parser/ast/ast_visitor.cpp`

**الخطوات:**
1. تعريف `ASTVisitor` interface
2. تحديث `ASTNode` لدعم `accept()`
3. إنشاء `EvalVisitor` للتقييم

**مثال Implementation:**
```cpp
// include/parser/ast/ast_visitor.h
namespace Sad::Parser::AST {

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Expressions
    virtual void visit(IntegerLiteral* node) = 0;
    virtual void visit(BinaryOperation* node) = 0;
    virtual void visit(VariableRef* node) = 0;
    
    // Statements
    virtual void visit(VariableDecl* node) = 0;
    virtual void visit(Assignment* node) = 0;
    virtual void visit(IfStatement* node) = 0;
    virtual void visit(WhileLoop* node) = 0;
    
    // Functions
    virtual void visit(FunctionDef* node) = 0;
    virtual void visit(FunctionCall* node) = 0;
};

// تحديث AST nodes
class Expression : public ASTNode {
public:
    virtual void accept(ASTVisitor* visitor) = 0;
};

class IntegerLiteral : public Expression {
public:
    int64_t value;
    
    void accept(ASTVisitor* visitor) override {
        visitor->visit(this);
    }
};

} // namespace Sad::Parser::AST
```

**معايير القبول:**
- ✅ Visitor pattern implemented
- ✅ All AST nodes support accept()
- ✅ Tests demonstrate traversal

---

### Sprint 2: Integration Pipeline (أسبوع 3-4)

#### Task 2.1: إنشاء SadEngine (المحرك الرئيسي)
**المدة:** 3 أيام  
**الملفات:**
- `include/core/sad_engine.h`
- `src/core/sad_engine.cpp`

**الوظيفة:**
```cpp
// include/core/sad_engine.h
namespace Sad {

class SadEngine {
public:
    // Initialize all subsystems
    SadEngine();
    ~SadEngine();
    
    // Main execution pipeline
    bool executeFile(const std::string& filename);
    bool executeString(const std::string& source);
    
    // Get results
    Data::Value getLastResult() const;
    std::vector<Error::ErrorInfo> getErrors() const;
    
    // Configuration
    void setDebugMode(bool enable);
    void setStdLibPath(const std::string& path);
    
private:
    std::unique_ptr<Lexer::LexerCore> lexer_;
    std::unique_ptr<Parser::ParserCore> parser_;
    std::unique_ptr<Interpreter::InterpreterCore> interpreter_;
    std::unique_ptr<Error::ErrorManager> errorManager_;
    
    bool debugMode_;
    
    // Pipeline methods
    std::vector<Lexer::Token> tokenize(const std::string& source);
    std::unique_ptr<AST::Program> parse(const std::vector<Lexer::Token>& tokens);
    Data::Value execute(AST::Program* ast);
};

} // namespace Sad
```

**Implementation:**
```cpp
// src/core/sad_engine.cpp
namespace Sad {

bool SadEngine::executeString(const std::string& source) {
    DEBUG_PRINT("=== Starting execution ===");
    
    try {
        // Step 1: Tokenize
        DEBUG_PRINT("Step 1: Tokenization");
        auto tokens = tokenize(source);
        if (errorManager_->hasErrors()) {
            ERROR_PRINT("Lexical errors found");
            return false;
        }
        INFO_PRINT("Tokenization complete: %zu tokens", tokens.size());
        
        // Step 2: Parse
        DEBUG_PRINT("Step 2: Parsing");
        auto ast = parse(tokens);
        if (errorManager_->hasErrors()) {
            ERROR_PRINT("Syntax errors found");
            return false;
        }
        INFO_PRINT("Parsing complete");
        
        // Step 3: Execute
        DEBUG_PRINT("Step 3: Execution");
        auto result = execute(ast.get());
        INFO_PRINT("Execution complete");
        
        return true;
        
    } catch (const std::exception& e) {
        ERROR_PRINT("Execution failed: %s", e.what());
        return false;
    }
}

} // namespace Sad
```

**الاختبارات:**
```cpp
// tests/integration_tests/engine_test.cpp
TEST(SadEngineTest, SimpleExpression) {
    SadEngine engine;
    engine.setDebugMode(true);
    
    EXPECT_TRUE(engine.executeString("رقم س = 5 + 3"));
    // Verify no errors
    EXPECT_EQ(engine.getErrors().size(), 0);
}

TEST(SadEngineTest, FullProgram) {
    SadEngine engine;
    
    std::string program = R"(
        رقم س = 10
        رقم ص = 20
        رقم مجموع = س + ص
        اطبع(مجموع)
    )";
    
    EXPECT_TRUE(engine.executeString(program));
}
```

**معايير القبول:**
- ✅ Pipeline كامل يعمل
- ✅ Error handling في كل مرحلة
- ✅ Debug logging شامل
- ✅ Integration tests pass

---

## 🔌 نقاط الامتداد / Extension Points

### 1. Custom Token Handlers
```cpp
// include/lexer/token_handler.h
namespace Sad::Lexer {

class ITokenHandler {
public:
    virtual ~ITokenHandler() = default;
    virtual bool canHandle(char current, const std::string& source, size_t pos) = 0;
    virtual Token handle(const std::string& source, size_t& pos) = 0;
};

// في LexerCore
class LexerCore {
    std::vector<std::unique_ptr<ITokenHandler>> customHandlers_;
public:
    void registerTokenHandler(std::unique_ptr<ITokenHandler> handler);
};

} // namespace Sad::Lexer
```

### 2. Custom AST Visitors
```cpp
// يمكن للمستخدمين إنشاء visitors خاصة
class MyCustomVisitor : public AST::ASTVisitor {
    // تنفيذ custom logic
};
```

### 3. Native Function Binding
```cpp
// include/interpreter/native_binding.h
namespace Sad::Interpreter {

using NativeFunction = std::function<Data::Value(const std::vector<Data::Value>&)>;

class NativeBindings {
public:
    void registerFunction(const std::string& name, NativeFunction func);
};

} // namespace Sad::Interpreter
```

---

## 🧪 استراتيجية الاختبار / Testing Strategy

### 1. Unit Tests (70% coverage target)
```
tests/
├── lexer_tests/
│   ├── token_test.cpp
│   ├── lexer_core_test.cpp
│   └── unicode_test.cpp
├── parser_tests/
│   ├── expression_parser_test.cpp
│   ├── statement_parser_test.cpp
│   └── error_recovery_test.cpp
├── interpreter_tests/
│   ├── value_test.cpp
│   ├── execution_test.cpp
│   └── scope_test.cpp
└── data_tests/
    ├── value_test.cpp
    └── type_conversion_test.cpp
```

### 2. Integration Tests (20% coverage)
```
tests/integration_tests/
├── engine_test.cpp           # Lexer → Parser → Interpreter
├── stdlib_integration_test.cpp
└── error_flow_test.cpp
```

### 3. End-to-End Tests (10% coverage)
```
tests/e2e_tests/
├── programs/
│   ├── 01_hello_world.s
│   ├── 02_variables.s
│   ├── 03_functions.s
│   └── 04_classes.s
└── e2e_runner.cpp
```

### مثال E2E Test:
```cpp
// tests/e2e_tests/e2e_runner.cpp
TEST(E2ETest, HelloWorld) {
    SadEngine engine;
    
    std::string program = readFile("tests/e2e_tests/programs/01_hello_world.s");
    EXPECT_TRUE(engine.executeFile("tests/e2e_tests/programs/01_hello_world.s"));
    
    // Verify output
    auto output = captureOutput();
    EXPECT_EQ(output, "مرحباً بالعالم!\n");
}
```

---

## 📊 Logging Strategy

### مستويات Logging:
```cpp
// include/debug/debug_print.h
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG][%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define INFO_PRINT(fmt, ...) \
        fprintf(stderr, "[INFO] " fmt "\n", ##__VA_ARGS__)
    #define WARN_PRINT(fmt, ...) \
        fprintf(stderr, "[WARN] " fmt "\n", ##__VA_ARGS__)
    #define ERROR_PRINT(fmt, ...) \
        fprintf(stderr, "[ERROR][%s:%d] " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...)
    #define INFO_PRINT(fmt, ...)
    #define WARN_PRINT(fmt, ...) \
        fprintf(stderr, "[WARN] " fmt "\n", ##__VA_ARGS__)
    #define ERROR_PRINT(fmt, ...) \
        fprintf(stderr, "[ERROR] " fmt "\n", ##__VA_ARGS__)
#endif
```

### نقاط Logging الحرجة:
1. **Lexer:**
   - DEBUG: كل token يتم إنشاؤه
   - WARN: أحرف غير معروفة
   - ERROR: أخطاء في الصيغة

2. **Parser:**
   - DEBUG: كل AST node يتم إنشاؤه
   - WARN: تجاوز أخطاء محتمل
   - ERROR: أخطاء نحوية

3. **Interpreter:**
   - DEBUG: كل عملية تنفيذ
   - INFO: استدعاء دوال
   - ERROR: أخطاء runtime

---

## 🚀 CI/CD Pipeline

### PowerShell Build Script
```powershell
# build.ps1
param(
    [switch]$Debug,
    [switch]$Release,
    [switch]$Test,
    [switch]$Clean
)

$BuildDir = "build"
$BuildType = if ($Release) { "Release" } else { "Debug" }

if ($Clean) {
    Remove-Item -Recurse -Force $BuildDir -ErrorAction SilentlyContinue
}

# Create build directory
New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null
Set-Location $BuildDir

# Configure
cmake .. -G "Ninja" `
    -DCMAKE_BUILD_TYPE=$BuildType `
    -DDEBUG=$(if ($Debug) { "ON" } else { "OFF" })

if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed"
    exit 1
}

# Build
cmake --build . --config $BuildType

if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed"
    exit 1
}

# Test
if ($Test) {
    ctest --output-on-failure -C $BuildType
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Tests failed"
        exit 1
    }
}

Set-Location ..
Write-Host "✅ Build successful!" -ForegroundColor Green
```

### الاستخدام:
```powershell
# Build debug with tests
.\build.ps1 -Debug -Test

# Build release
.\build.ps1 -Release

# Clean build
.\build.ps1 -Clean -Debug
```

---

## ⏱️ التقدير الزمني / Time Estimates

### Sprint Breakdown (8 weeks)

| Sprint | المهام | المدة | المطورين |
|--------|--------|------|----------|
| Sprint 1 | Core Interfaces | أسبوعان | 2 |
| Sprint 2 | Integration Pipeline | أسبوعان | 2 |
| Sprint 3 | Testing & Documentation | أسبوع | 2 |
| Sprint 4 | Optimization & Polish | أسبوع | 2 |
| Sprint 5-8 | Component Implementation | 4 أسابيع | 3-4 |

**إجمالي:** 8 أسابيع × 2-4 مطورين = 16-32 مطور-أسبوع

---

## ⚠️ المخاطر / Risks

| الخطر | الاحتمال | التأثير | التخفيف |
|-------|----------|---------|---------|
| تعارض في الواجهات | متوسط | عالي | تحديد الواجهات مبكراً + مراجعات |
| Memory leaks في Value system | متوسط | عالي | استخدام smart pointers + Valgrind |
| أداء بطيء | منخفض | متوسط | Profiling + optimization passes |
| Unicode issues | عالي | متوسط | اختبارات شاملة للـ RTL والعربية |
| Integration bugs | عالي | عالي | Integration tests مبكرة |

---

## 📚 المراجع / References

- `docs/language_spec/00_INDEX.md` - فهرس اللغة
- `docs/architecture/design.md` - التصميم المعماري
- `plans/00_MASTER_PLAN.md` - الخطة الرئيسية
- `plans/04_QUALITY_STANDARDS.md` - معايير الجودة

---

## ✅ Checklist للإكمال

- [ ] جميع الواجهات محددة في headers
- [ ] Value system مُنفذ ومختبر
- [ ] Visitor pattern مُطبق على AST
- [ ] SadEngine pipeline يعمل
- [ ] Integration tests تمر
- [ ] Documentation كامل
- [ ] CI/CD pipeline configured
- [ ] Code coverage ≥ 80%

---

**آخر تحديث:** 4 نوفمبر 2025  
**الحالة:** جاهز للمراجعة والتنفيذ
