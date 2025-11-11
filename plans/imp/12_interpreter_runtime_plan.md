# 🚀 خطة تنفيذ المُفسّر / Interpreter Runtime Implementation Plan

**التاريخ:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**المدة المقدرة:** 5 أسابيع

---

## 📋 ملخص / Summary

### (AR) الملخص
خطة تفصيلية لتطوير المفسّر (Interpreter Runtime) للغة "ص". يشمل Call Stack، Environment/Scope Management، Value System، Memory Management، وتكامل StdLib/FFI.

### (EN) Summary
Detailed plan for developing the Interpreter Runtime for Sad language. Includes Call Stack, Environment/Scope Management, Value System, Memory Management, and StdLib/FFI integration.

---

## 🎯 الأهداف / Objectives

1. ✅ تنفيذ Value System شامل (من 09_integration)
2. ✅ بناء Environment/Scope management
3. ✅ تنفيذ Call Stack + stack traces
4. ✅ معالجة Expressions و Statements
5. ✅ دعم Functions + Closures
6. ✅ دعم OOP (Classes, Inheritance)
7. ✅ Memory management (GC roadmap)
8. ✅ FFI لربط StdLib
9. ✅ Exception handling bilingual
10. ✅ Bytecode VM roadmap (Phase 3)

---

## 📦 الاعتماديات / Dependencies

### من language_spec:
- ✅ `01_types.md` - نظام الأنواع
- ✅ `02_functions.md` - Closures + higher-order
- ✅ `03_oop.md` - Classes + inheritance
- ✅ `04_python_features.md` - Generators

### من plans:
- ✅ `09_integration_and_architecture.md` - Value interface
- ✅ `10_lexer_implementation_plan.md` - Token
- ✅ `11_parser_and_ast_plan.md` - AST + Visitor

---

## 🏗️ البنية المعمارية / Architecture

### Value System (Extended from 09_integration)

```cpp
// include/data/types/value.h
namespace Sad::Data {

enum class ValueType {
    INTEGER,
    FLOAT,
    STRING,
    BOOLEAN,
    ARRAY,
    MAP,
    FUNCTION,
    CLASS,
    INSTANCE,
    NONE
};

class Value {
private:
    ValueType type;
    
    union {
        int64_t intValue;
        double floatValue;
        bool boolValue;
    };
    
    // Complex types (heap-allocated)
    std::shared_ptr<std::string> stringValue;
    std::shared_ptr<std::vector<Value>> arrayValue;
    std::shared_ptr<std::map<std::string, Value>> mapValue;
    std::shared_ptr<class Function> functionValue;
    std::shared_ptr<class ClassInstance> instanceValue;
    
public:
    // Constructors
    Value() : type(ValueType::NONE) {}
    Value(int64_t v) : type(ValueType::INTEGER), intValue(v) {}
    Value(double v) : type(ValueType::FLOAT), floatValue(v) {}
    Value(bool v) : type(ValueType::BOOLEAN), boolValue(v) {}
    Value(const std::string& v) 
        : type(ValueType::STRING), stringValue(std::make_shared<std::string>(v)) {}
    
    // Type checking
    bool isInteger() const { return type == ValueType::INTEGER; }
    bool isFloat() const { return type == ValueType::FLOAT; }
    bool isString() const { return type == ValueType::STRING; }
    bool isBool() const { return type == ValueType::BOOLEAN; }
    bool isArray() const { return type == ValueType::ARRAY; }
    bool isFunction() const { return type == ValueType::FUNCTION; }
    bool isNone() const { return type == ValueType::NONE; }
    
    // Getters (with type checking)
    int64_t asInteger() const {
        if (!isInteger()) throw RuntimeError("Value is not an integer");
        return intValue;
    }
    
    double asFloat() const {
        if (!isFloat()) throw RuntimeError("Value is not a float");
        return floatValue;
    }
    
    std::string asString() const {
        if (!isString()) throw RuntimeError("Value is not a string");
        return *stringValue;
    }
    
    // Operators
    Value operator+(const Value& other) const;
    Value operator-(const Value& other) const;
    Value operator*(const Value& other) const;
    Value operator/(const Value& other) const;
    bool operator==(const Value& other) const;
    
    // Conversion
    std::string toString() const;
    bool toBool() const;
    
    // Array operations
    static Value makeArray(const std::vector<Value>& elements) {
        Value v;
        v.type = ValueType::ARRAY;
        v.arrayValue = std::make_shared<std::vector<Value>>(elements);
        return v;
    }
    
    Value& operator[](size_t index) {
        if (!isArray()) throw RuntimeError("Value is not an array");
        return (*arrayValue)[index];
    }
};

// Operators implementation
Value Value::operator+(const Value& other) const {
    // Integer + Integer
    if (isInteger() && other.isInteger()) {
        return Value(intValue + other.intValue);
    }
    
    // Float + Float (or coercion)
    if (isFloat() || other.isFloat()) {
        double left = isFloat() ? floatValue : static_cast<double>(intValue);
        double right = other.isFloat() ? other.floatValue : static_cast<double>(other.intValue);
        return Value(left + right);
    }
    
    // String concatenation
    if (isString() && other.isString()) {
        return Value(*stringValue + *other.stringValue);
    }
    
    throw RuntimeError("Cannot add " + typeToString() + " and " + other.typeToString());
}

} // namespace Sad::Data
```

---

### Environment (Scope Management)

```cpp
// include/data/scope/environment.h
namespace Sad::Interpreter {

class Environment {
private:
    std::map<std::string, Value> variables;
    std::shared_ptr<Environment> parent; // for nested scopes
    
public:
    Environment() : parent(nullptr) {}
    Environment(std::shared_ptr<Environment> p) : parent(p) {}
    
    // Variable operations
    void define(const std::string& name, const Value& value) {
        DEBUG_PRINT("Defining variable: %s = %s", name.c_str(), value.toString().c_str());
        variables[name] = value;
    }
    
    Value get(const std::string& name) const {
        auto it = variables.find(name);
        if (it != variables.end()) {
            return it->second;
        }
        
        // Search in parent scope
        if (parent) {
            return parent->get(name);
        }
        
        throw RuntimeError("Undefined variable: " + name);
    }
    
    void set(const std::string& name, const Value& value) {
        auto it = variables.find(name);
        if (it != variables.end()) {
            it->second = value;
            DEBUG_PRINT("Set variable: %s = %s", name.c_str(), value.toString().c_str());
            return;
        }
        
        if (parent) {
            parent->set(name, value);
            return;
        }
        
        throw RuntimeError("Cannot assign to undefined variable: " + name);
    }
    
    bool exists(const std::string& name) const {
        if (variables.count(name) > 0) return true;
        if (parent) return parent->exists(name);
        return false;
    }
    
    // Scope management
    std::shared_ptr<Environment> createChild() {
        return std::make_shared<Environment>(shared_from_this());
    }
};

} // namespace Sad::Interpreter
```

---

### Call Stack

```cpp
// include/interpreter/core/call_stack.h
namespace Sad::Interpreter {

struct StackFrame {
    std::string functionName;
    Position position;
    std::shared_ptr<Environment> environment;
    
    std::string toString() const {
        return functionName + " at " + position.toString();
    }
};

class CallStack {
private:
    std::vector<StackFrame> frames;
    const size_t MAX_DEPTH = 1000;
    
public:
    void push(const std::string& name, const Position& pos, 
              std::shared_ptr<Environment> env) {
        if (frames.size() >= MAX_DEPTH) {
            throw RuntimeError("Stack overflow: maximum call depth exceeded");
        }
        
        StackFrame frame{name, pos, env};
        frames.push_back(frame);
        
        DEBUG_PRINT("Call stack push: %s (depth: %zu)", 
                    name.c_str(), frames.size());
    }
    
    void pop() {
        if (!frames.empty()) {
            DEBUG_PRINT("Call stack pop: %s", frames.back().functionName.c_str());
            frames.pop_back();
        }
    }
    
    std::string getStackTrace() const {
        std::ostringstream trace;
        trace << "Stack trace:\n";
        
        for (int i = frames.size() - 1; i >= 0; --i) {
            trace << "  " << frames[i].toString() << "\n";
        }
        
        return trace.str();
    }
    
    size_t depth() const { return frames.size(); }
    
    const StackFrame& top() const {
        if (frames.empty()) {
            throw RuntimeError("Call stack is empty");
        }
        return frames.back();
    }
};

} // namespace Sad::Interpreter
```

---

### Interpreter Core (Visitor Implementation)

```cpp
// include/interpreter/core/interpreter_core.h
namespace Sad::Interpreter {

class InterpreterCore : public AST::ASTVisitor {
private:
    std::shared_ptr<Environment> globalEnv;
    std::shared_ptr<Environment> currentEnv;
    CallStack callStack;
    Value lastValue; // result of last expression
    
public:
    InterpreterCore() {
        globalEnv = std::make_shared<Environment>();
        currentEnv = globalEnv;
        
        // Register built-in functions
        registerBuiltins();
    }
    
    // Execute program
    void execute(AST::ASTNode* ast) {
        try {
            ast->accept(*this);
        } catch (const RuntimeError& e) {
            ERROR_PRINT("Runtime error: %s\n%s", 
                       e.what(), callStack.getStackTrace().c_str());
            throw;
        }
    }
    
    // Expression visitors (return value via lastValue)
    void visitBinaryExpr(AST::BinaryExpr* expr) override {
        expr->left->accept(*this);
        Value left = lastValue;
        
        expr->right->accept(*this);
        Value right = lastValue;
        
        switch (expr->op) {
            case TokenType::PLUS:
                lastValue = left + right;
                break;
            case TokenType::MINUS:
                lastValue = left - right;
                break;
            case TokenType::MULTIPLY:
                lastValue = left * right;
                break;
            case TokenType::DIVIDE:
                if (right.asInteger() == 0) {
                    throw RuntimeError("Division by zero");
                }
                lastValue = left / right;
                break;
            case TokenType::EQUAL:
                lastValue = Value(left == right);
                break;
            case TokenType::GREATER_THAN:
                lastValue = Value(left > right);
                break;
            // ... other operators
            default:
                throw RuntimeError("Unknown binary operator");
        }
        
        DEBUG_PRINT("BinaryExpr result: %s", lastValue.toString().c_str());
    }
    
    void visitLiteralExpr(AST::LiteralExpr* expr) override {
        // Convert token to Value
        switch (expr->token.type) {
            case TokenType::INTEGER_LITERAL:
                lastValue = Value(std::get<int64_t>(expr->token.value));
                break;
            case TokenType::FLOAT_LITERAL:
                lastValue = Value(std::get<double>(expr->token.value));
                break;
            case TokenType::STRING_LITERAL:
                lastValue = Value(std::get<std::string>(expr->token.value));
                break;
            case TokenType::KEYWORD_TRUE:
                lastValue = Value(true);
                break;
            case TokenType::KEYWORD_FALSE:
                lastValue = Value(false);
                break;
            default:
                lastValue = Value(); // None
        }
        
        DEBUG_PRINT("Literal: %s", lastValue.toString().c_str());
    }
    
    void visitVariableExpr(AST::VariableExpr* expr) override {
        lastValue = currentEnv->get(expr->name);
        DEBUG_PRINT("Variable '%s' = %s", 
                    expr->name.c_str(), lastValue.toString().c_str());
    }
    
    void visitCallExpr(AST::CallExpr* expr) override {
        // Evaluate callee
        expr->callee->accept(*this);
        Value callee = lastValue;
        
        if (!callee.isFunction()) {
            throw RuntimeError("Cannot call non-function value");
        }
        
        // Evaluate arguments
        std::vector<Value> args;
        for (auto& arg : expr->arguments) {
            arg->accept(*this);
            args.push_back(lastValue);
        }
        
        // Call function
        lastValue = callFunction(callee, args);
    }
    
    // Statement visitors
    void visitVarDeclStmt(AST::VarDeclStmt* stmt) override {
        Value value;
        if (stmt->initializer) {
            stmt->initializer->accept(*this);
            value = lastValue;
        } else {
            value = Value(); // None
        }
        
        currentEnv->define(stmt->name, value);
        DEBUG_PRINT("Declared variable: %s = %s", 
                    stmt->name.c_str(), value.toString().c_str());
    }
    
    void visitIfStmt(AST::IfStmt* stmt) override {
        stmt->condition->accept(*this);
        
        if (lastValue.toBool()) {
            stmt->thenBranch->accept(*this);
        } else if (stmt->elseBranch) {
            stmt->elseBranch->accept(*this);
        }
    }
    
    void visitWhileStmt(AST::WhileStmt* stmt) override {
        while (true) {
            stmt->condition->accept(*this);
            
            if (!lastValue.toBool()) break;
            
            try {
                stmt->body->accept(*this);
            } catch (const BreakException&) {
                break;
            } catch (const ContinueException&) {
                continue;
            }
        }
    }
    
    void visitReturnStmt(AST::ReturnStmt* stmt) override {
        if (stmt->value) {
            stmt->value->accept(*this);
        } else {
            lastValue = Value(); // None
        }
        
        throw ReturnException(lastValue);
    }
    
    void visitBlockStmt(AST::BlockStmt* stmt) override {
        // Create new scope
        auto previousEnv = currentEnv;
        currentEnv = currentEnv->createChild();
        
        try {
            for (auto& s : stmt->statements) {
                s->accept(*this);
            }
        } catch (...) {
            currentEnv = previousEnv;
            throw;
        }
        
        currentEnv = previousEnv;
    }
    
private:
    void registerBuiltins() {
        // Built-in functions
        globalEnv->define("اطبع", Value::makeBuiltinFunction("print", builtinPrint));
        globalEnv->define("print", Value::makeBuiltinFunction("print", builtinPrint));
    }
    
    static Value builtinPrint(const std::vector<Value>& args) {
        for (size_t i = 0; i < args.size(); ++i) {
            std::cout << args[i].toString();
            if (i < args.size() - 1) std::cout << " ";
        }
        std::cout << "\n";
        return Value(); // None
    }
};

} // namespace Sad::Interpreter
```

---

## 📁 الملفات المطلوبة / Required Files

```
include/interpreter/
├── core/
│   ├── interpreter_core.h      ✅ UPDATE (add visitors)
│   ├── call_stack.h           🆕 NEW
│   └── exceptions.h           🆕 NEW (RuntimeError, Return, Break, Continue)
├── expressions/
│   ├── binary_evaluator.h     🆕 NEW
│   ├── call_evaluator.h       🆕 NEW
│   └── index_evaluator.h      🆕 NEW
├── statements/
│   ├── control_flow.h         🆕 NEW (if/while/for)
│   └── declarations.h         🆕 NEW
└── oop/
    ├── class_instance.h        🆕 NEW
    └── method_binding.h        🆕 NEW

include/data/
├── scope/
│   ├── environment.h           🆕 NEW
│   └── scope_manager.h         🆕 NEW
└── types/
    ├── value.h                 ✅ UPDATE (extend from 09_integration)
    ├── function.h              🆕 NEW
    └── class_type.h            🆕 NEW

src/interpreter/
├── core/
│   ├── interpreter_core.cpp    ✅ UPDATE
│   ├── call_stack.cpp         🆕 NEW
│   └── exceptions.cpp         🆕 NEW
├── expressions/
│   ├── binary_evaluator.cpp   🆕 NEW
│   ├── call_evaluator.cpp     🆕 NEW
│   └── index_evaluator.cpp    🆕 NEW
├── statements/
│   ├── control_flow.cpp       🆕 NEW
│   └── declarations.cpp       🆕 NEW
└── oop/
    ├── class_instance.cpp      🆕 NEW
    └── method_binding.cpp      🆕 NEW

tests/interpreter_tests/
├── value_test.cpp              🆕 NEW (15 tests)
├── environment_test.cpp        🆕 NEW (10 tests)
├── expressions_test.cpp        🆕 NEW (20 tests)
├── statements_test.cpp         🆕 NEW (15 tests)
├── functions_test.cpp          🆕 NEW (12 tests)
├── classes_test.cpp            🆕 NEW (10 tests)
└── integration_test.cpp        🆕 NEW (10 tests)
```

---

## 🔧 تقسيم العمل / Task Breakdown

### Week 1: Value System & Environment

#### Task 1.1: تطوير Value System (يوم 1-3)
**الملفات:**
- `include/data/types/value.h`
- `src/data/types/value.cpp`

**الخطوات:**
```cpp
// 1. إضافة جميع أنواع البيانات
enum class ValueType {
    INTEGER, FLOAT, STRING, BOOLEAN,
    ARRAY, MAP, FUNCTION, CLASS, INSTANCE, NONE
};

// 2. تنفيذ operators
Value Value::operator+(const Value& other) const { ... }
Value Value::operator-(const Value& other) const { ... }
// ... all operators

// 3. Array/Map operations
Value& operator[](size_t index);
Value& operator[](const std::string& key);

// 4. Type conversions
std::string toString() const;
bool toBool() const;
```

**الاختبارات:**
```cpp
TEST(ValueTest, IntegerOperations) {
    Value a(42);
    Value b(10);
    
    EXPECT_EQ((a + b).asInteger(), 52);
    EXPECT_EQ((a - b).asInteger(), 32);
    EXPECT_EQ((a * b).asInteger(), 420);
    EXPECT_EQ((a / b).asInteger(), 4);
}

TEST(ValueTest, TypeCoercion) {
    Value i(42);        // Integer
    Value f(3.14);      // Float
    
    Value result = i + f;
    EXPECT_TRUE(result.isFloat());
    EXPECT_DOUBLE_EQ(result.asFloat(), 45.14);
}

TEST(ValueTest, ArrayOperations) {
    Value arr = Value::makeArray({Value(1), Value(2), Value(3)});
    
    EXPECT_TRUE(arr.isArray());
    EXPECT_EQ(arr[0].asInteger(), 1);
    EXPECT_EQ(arr[2].asInteger(), 3);
}
```

**معايير القبول:**
- ✅ All 10 value types implemented
- ✅ All operators work
- ✅ Array/Map indexing works
- ✅ Tests pass (≥20 tests)

---

#### Task 1.2: Environment Management (يوم 4-5)
**الملفات:**
- `include/data/scope/environment.h`
- `src/data/scope/environment.cpp`

```cpp
// تنفيذ Environment class
class Environment {
    std::map<std::string, Value> variables;
    std::shared_ptr<Environment> parent;
    
public:
    void define(const std::string& name, const Value& value);
    Value get(const std::string& name) const;
    void set(const std::string& name, const Value& value);
    bool exists(const std::string& name) const;
    std::shared_ptr<Environment> createChild();
};
```

**الاختبارات:**
```cpp
TEST(EnvironmentTest, DefineAndGet) {
    Environment env;
    env.define("x", Value(42));
    
    EXPECT_EQ(env.get("x").asInteger(), 42);
}

TEST(EnvironmentTest, NestedScopes) {
    auto parent = std::make_shared<Environment>();
    parent->define("x", Value(10));
    
    auto child = parent->createChild();
    child->define("y", Value(20));
    
    EXPECT_EQ(child->get("x").asInteger(), 10); // from parent
    EXPECT_EQ(child->get("y").asInteger(), 20); // from child
}

TEST(EnvironmentTest, ShadowingVariable) {
    auto parent = std::make_shared<Environment>();
    parent->define("x", Value(10));
    
    auto child = parent->createChild();
    child->define("x", Value(20)); // shadow parent's x
    
    EXPECT_EQ(child->get("x").asInteger(), 20);
    EXPECT_EQ(parent->get("x").asInteger(), 10); // parent unchanged
}
```

---

### Week 2-3: Expression & Statement Evaluation

#### Task 2.1: Binary/Unary Expression Evaluator (يوم 6-8)
```cpp
void InterpreterCore::visitBinaryExpr(BinaryExpr* expr) {
    expr->left->accept(*this);
    Value left = lastValue;
    
    expr->right->accept(*this);
    Value right = lastValue;
    
    switch (expr->op) {
        case TokenType::PLUS:   lastValue = left + right; break;
        case TokenType::MINUS:  lastValue = left - right; break;
        case TokenType::MULTIPLY: lastValue = left * right; break;
        case TokenType::DIVIDE: lastValue = left / right; break;
        // ... all operators
    }
}
```

**الاختبارات:**
```cpp
TEST(InterpreterTest, ArithmeticExpressions) {
    // 2 + 3 * 4
    LexerCore lexer("2 + 3 * 4");
    ParserCore parser(lexer.tokenize());
    auto ast = parser.parse();
    
    InterpreterCore interp;
    interp.execute(ast.get());
    
    EXPECT_EQ(interp.getLastValue().asInteger(), 14);
}
```

---

#### Task 2.2: Control Flow (يوم 9-12)
```cpp
void InterpreterCore::visitIfStmt(IfStmt* stmt) {
    stmt->condition->accept(*this);
    
    if (lastValue.toBool()) {
        stmt->thenBranch->accept(*this);
    } else if (stmt->elseBranch) {
        stmt->elseBranch->accept(*this);
    }
}

void InterpreterCore::visitWhileStmt(WhileStmt* stmt) {
    while (true) {
        stmt->condition->accept(*this);
        if (!lastValue.toBool()) break;
        
        try {
            stmt->body->accept(*this);
        } catch (const BreakException&) {
            break;
        }
    }
}
```

---

### Week 4: Functions & Closures

#### Task 4.1: Function Implementation (يوم 13-17)
```cpp
// include/data/types/function.h
class Function {
public:
    std::string name;
    std::vector<Parameter> parameters;
    std::unique_ptr<AST::BlockStmt> body;
    std::shared_ptr<Environment> closure; // captured environment
    
    Value call(InterpreterCore* interpreter, const std::vector<Value>& args) {
        // Create function scope
        auto funcEnv = closure->createChild();
        
        // Bind parameters
        for (size_t i = 0; i < parameters.size(); ++i) {
            funcEnv->define(parameters[i].name, args[i]);
        }
        
        // Execute body
        auto previousEnv = interpreter->currentEnv;
        interpreter->currentEnv = funcEnv;
        
        try {
            body->accept(*interpreter);
        } catch (const ReturnException& ret) {
            interpreter->currentEnv = previousEnv;
            return ret.value;
        }
        
        interpreter->currentEnv = previousEnv;
        return Value(); // None
    }
};
```

**الاختبارات:**
```cpp
TEST(InterpreterTest, FunctionCall) {
    std::string code = R"(
        دالة جمع(رقم أ، رقم ب) -> رقم {
            إرجاع أ + ب
        }
        
        رقم نتيجة = جمع(10, 20)
    )";
    
    auto result = executeCode(code);
    EXPECT_EQ(result["نتيجة"].asInteger(), 30);
}

TEST(InterpreterTest, Closure) {
    std::string code = R"(
        دالة صانع_عداد(رقم بداية) -> دالة {
            رقم عداد = بداية
            
            إرجاع lambda() -> رقم {
                عداد = عداد + 1
                إرجاع عداد
            }
        }
        
        دالة عد = صانع_عداد(0)
        رقم أول = عد()
        رقم ثاني = عد()
    )";
    
    auto result = executeCode(code);
    EXPECT_EQ(result["أول"].asInteger(), 1);
    EXPECT_EQ(result["ثاني"].asInteger(), 2);
}
```

---

### Week 5: OOP & Integration

#### Task 5.1: Class Implementation (يوم 18-22)
```cpp
// include/interpreter/oop/class_instance.h
class ClassInstance {
public:
    std::string className;
    std::map<std::string, Value> fields;
    std::map<std::string, std::shared_ptr<Function>> methods;
    
    Value getField(const std::string& name) {
        auto it = fields.find(name);
        if (it != fields.end()) return it->second;
        
        throw RuntimeError("Undefined field: " + name);
    }
    
    void setField(const std::string& name, const Value& value) {
        fields[name] = value;
    }
    
    Value callMethod(const std::string& name, 
                     InterpreterCore* interpreter,
                     const std::vector<Value>& args) {
        auto it = methods.find(name);
        if (it == methods.end()) {
            throw RuntimeError("Undefined method: " + name);
        }
        
        // Bind 'this'
        auto methodEnv = it->second->closure->createChild();
        methodEnv->define("هذا", Value::makeInstance(this));
        
        return it->second->call(interpreter, args);
    }
};
```

**الاختبارات:**
```cpp
TEST(InterpreterTest, ClassBasic) {
    std::string code = R"(
        فئة شخص {
            نص اسم
            رقم عمر
            
            بناء(نص ا، رقم ع) {
                هذا.اسم = ا
                هذا.عمر = ع
            }
            
            دالة وصف() -> نص {
                إرجاع هذا.اسم + " عمره " + نص(هذا.عمر)
            }
        }
        
        شخص ص = شخص("أحمد"، 25)
        نص معلومات = ص.وصف()
    )";
    
    auto result = executeCode(code);
    EXPECT_EQ(result["معلومات"].asString(), "أحمد عمره 25");
}
```

---

## 🧪 الاختبارات / Tests Summary

### Unit Tests (92+ tests)
- Value operations (20 tests)
- Environment (10 tests)
- Expressions (20 tests)
- Statements (15 tests)
- Functions (12 tests)
- Classes (10 tests)
- Integration (5 tests)

---

## ⏱️ الجدول الزمني / Timeline

| الأسبوع | المهام | الساعات |
|---------|--------|---------|
| Week 1 | Value + Environment | 40 ساعة |
| Week 2-3 | Expressions + Statements | 80 ساعة |
| Week 4 | Functions + Closures | 40 ساعة |
| Week 5 | OOP + Integration | 40 ساعة |
| **Total** | **5 weeks** | **200 ساعة** |

---

## 🚀 أوامر PowerShell / PowerShell Commands

```powershell
# إنشاء فرع العمل
git checkout -b feat/interpreter/runtime-core

# بناء واختبار
cd build
cmake --build . --config Debug
ctest -R interpreter_tests --output-on-failure -C Debug

# Commit
git add include/interpreter/* include/data/* src/interpreter/* src/data/*
git commit -m "[interpreter] Implemented runtime with full OOP support

- Implemented Value system with 10 types
- Environment/scope management with closures
- Call stack with stack traces
- Expression/statement evaluation
- Functions with closures support
- Basic OOP (classes, methods, inheritance)
- 90+ unit tests with ≥80% coverage"

git push --set-upstream origin feat/interpreter/runtime-core
```

---

## ⚠️ المخاطر / Risks

| الخطر | التخفيف |
|-------|---------|
| Memory leaks (closures) | shared_ptr + tests |
| Stack overflow | MAX_DEPTH limit + detection |
| Type coercion bugs | شامل type checking tests |
| Closure bugs | Capture tests |

---

**آخر تحديث:** 4 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ
