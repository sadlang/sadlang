# 🔧 قواعد الدوال / Functions Rules

**التاريخ:** 5 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مرجع تفصيلي لنظام الدوال

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **نظام الدوال الكامل** للغة "ص". يشمل إعلان الدوال، المعاملات (Parameters)، القيم الافتراضية، الدوال المجهولة (Lambda)، الإغلاقات (Closures)، المزخرفات (Decorators)، والدوال العامة (Generics).

### (EN) Description
This file documents the **complete function system** for Sad language, including function declarations, parameters, default values, lambdas, closures, decorators, and generic functions.

---

## 📦 الاعتماديات / Dependencies

- ✅ `docs/language_spec/02_functions.md` - Function specification
- ✅ `docs/language_spec/01_types.md` - Type system
- ✅ `plans/imp/11_parser_and_ast_plan.md` - Function AST nodes
- ✅ `plans/imp/12_interpreter_runtime_plan.md` - Function execution

---

## 📋 قائمة الميزات / Features List

| # | الميزة (AR) | Feature (EN) | الحالة |
|---|-----------|-----------|-------|
| 1 | إعلان بسيط | Basic Declaration | ✅ Core |
| 2 | معاملات | Parameters | ✅ Core |
| 3 | قيم افتراضية | Default Values | ✅ Core |
| 4 | معاملات متغيرة | Varargs | ✅ Advanced |
| 5 | معاملات مسماة | Keyword Args | ✅ Advanced |
| 6 | دوال مجهولة | Lambda Functions | ✅ Core |
| 7 | إغلاقات | Closures | ✅ Advanced |
| 8 | دوال عليا | Higher-Order | ✅ Core |
| 9 | مزخرفات | Decorators | 🚧 Phase 2 |
| 10 | دوال عامة | Generic Functions | 🚧 Phase 2 |
| 11 | توثيق | Docstrings | ✅ Core |
| 12 | معاملات type hints | Type Hints | 🚧 Phase 2 |

---

## 1️⃣ Basic Function Declaration (الإعلان البسيط)

### Syntax / البناء النحوي (EBNF)
```ebnf
function_decl ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block

param_list ::= param ((',' | '،') param)*
param      ::= IDENTIFIER [':' type] ['=' expression]

block ::= statement* 'نهاية'
```

### Semantics
- **Declaration:** Function definitions create callable objects
- **Return Type:** Optional type before function name (e.g., `دالة رقم جمع(...)`)
- **Scope:** Functions have their own local scope
- **Return:** Explicit `إرجاع` (return) or implicit `None`
- **Name:** Must be unique in current scope (supports overloading in Phase 2)
- **Punctuation:** Both `،` and `,` supported for parameters; both `؛` and `;` supported for statements

**الشرح بالعربية:**
- `function_decl`: إعلان الدالة يبدأ بكلمة 'دالة'، يتبعها نوع الإرجاع الاختياري، ثم اسم الدالة، قائمة المعاملات بين أقواس، وجسم الدالة
- `param_list`: قائمة المعاملات مفصولة بفاصلة (عربية ، أو إنجليزية ,)
- `param`: كل معامل له اسم، ونوع اختياري، وقيمة افتراضية اختيارية
- `block`: جسم الدالة ينتهي بكلمة 'نهاية'

### Semantics / الدلالات
- **Declaration / الإعلان:** تعريف الدالة ينشئ كائن قابل للاستدعاء
- **Return Type / نوع الإرجاع:** النوع اختياري ويأتي قبل اسم الدالة (مثال: `دالة رقم جمع(...)`)
- **Scope / النطاق:** كل دالة لها نطاقها المحلي الخاص
- **Return / الإرجاع:** إرجاع صريح باستخدام `إرجاع` أو إرجاع ضمني لـ `None`
- **Name / الاسم:** يجب أن يكون فريداً في النطاق الحالي (يدعم التحميل الزائد في المرحلة 2)
- **Punctuation / علامات الترقيم:** تدعم الفاصلة العربية ، والإنجليزية , للمعاملات؛ والفاصلة المنقوطة العربية ؛ والإنجليزية ; للجمل

### Examples

```s
# دالة بسيطة بدون معاملات / Simple function without parameters
دالة قل_مرحبا()
    اطبع("مرحبا!")
نهاية

قل_مرحبا()  # Call: prints "مرحبا!"

# دالة مع معامل واحد / Function with one parameter
دالة رقم مربع(رقم ع)
    إرجاع ع * ع
نهاية

رقم نتيجة = مربع(5)  # 25

# دالة مع عدة معاملات / Function with multiple parameters
دالة رقم جمع(رقم أ، رقم ب)
    إرجاع أ + ب
نهاية

اطبع(جمع(3, 7))  # 10

# دالة بدون قيمة إرجاع / Function without return (returns None)
دالة اطبع_معلومات(نص اسم، رقم عمر)
    اطبع(ن"الاسم: {اسم}")
    اطبع(ن"العمر: {عمر}")
نهاية

اطبع_معلومات("أحمد", 25)

# دالة مع عدة نقاط إرجاع / Function with multiple returns
دالة رقم قيمة_مطلقة(رقم ع)
    إذا (ع >= 0)
        إرجاع ع
    وإلا
        إرجاع -ع
    نهاية
نهاية

اطبع(قيمة_مطلقة(-42))  # 42
```

### Token Analysis
```
KEYWORD_FUNC    "دالة"
KEYWORD_TYPE_INT "رقم"      # Optional return type
IDENTIFIER      "مربع"
LEFT_PAREN      "("
KEYWORD_TYPE_INT "رقم"
IDENTIFIER      "ع"
RIGHT_PAREN     ")"
KEYWORD_END     "نهاية"     # Block terminator
```

### AST Representation
```
FunctionDeclStmt
├── name: "مربع"
├── returnType: INTEGER (optional, comes before name)
├── parameters: [
│   └── Parameter
│       ├── name: "ع"
│       ├── type: INTEGER
│       └── defaultValue: null
│   ]
└── body: BlockStmt
    └── ReturnStmt
        └── expression: BinaryExpr
            ├── left: VariableExpr("ع")
            ├── operator: MULTIPLY
            └── right: VariableExpr("ع")
```

### Implementation Notes

**Files to Modify:**
```cpp
// include/parser/ast/statement_nodes.h
class FunctionDeclStmt : public Stmt {
public:
    std::string name;
    Type* returnType;                     // Optional, comes before name
    std::vector<Parameter> parameters;
    std::unique_ptr<BlockStmt> body;
    std::string docstring;                // Optional documentation
    
    FunctionDeclStmt(std::string name,
                     Type* returnType,
                     std::vector<Parameter> params,
                     std::unique_ptr<BlockStmt> body)
        : name(std::move(name)),
          returnType(returnType),
          parameters(std::move(params)),
          body(std::move(body)) {}
    
    void accept(Visitor& visitor) override;
};

struct Parameter {
    std::string name;
    Type* type;                           // Optional type hint
    std::unique_ptr<Expr> defaultValue;   // Optional default
    bool isVarargs = false;               // *args
    bool isKwargs = false;                // **kwargs
};

// src/parser/parser_functions.cpp
StmtPtr ParserCore::parseFunctionDecl() {
    consume(TokenType::KEYWORD_FUNC, "Expected 'دالة'");
    
    // Check for optional return type before function name
    Type* returnType = nullptr;
    if (isTypeKeyword(peek())) {
        returnType = parseType();
    }
    
    Token name = consume(TokenType::IDENTIFIER, "Expected function name");
    
    consume(TokenType::LEFT_PAREN, "Expected '('");
    
    std::vector<Parameter> params;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            params.push_back(parseParameter());
        } while (match({TokenType::COMMA, TokenType::ARABIC_COMMA}));  // Support both commas
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')'");
    
    // Parse function body (statements until 'نهاية')
    auto body = blockStatement();
    
    consume(TokenType::KEYWORD_END, "Expected 'نهاية' at end of function");
    
    return std::make_unique<FunctionDeclStmt>(
        name.lexeme,
        returnType,
        std::move(params),
        std::move(body)
    );
}

Parameter ParserCore::parseParameter() {
    Token name = consume(TokenType::IDENTIFIER, "Expected parameter name");
    
    Parameter param;
    param.name = name.lexeme;
    
    // Type hint
    if (match(TokenType::COLON)) {
        param.type = parseType();
    }
    
    // Default value
    if (match(TokenType::ASSIGN)) {
        param.defaultValue = expression();
    }
    
    return param;
}

// include/data/types/value.h
class FunctionValue {
public:
    std::string name;
    std::vector<Parameter> parameters;
    FunctionDeclStmt* declaration;
    Environment* closure;  // Captured environment for closures
    
    FunctionValue(const std::string& name,
                  FunctionDeclStmt* decl,
                  Environment* closure)
        : name(name), declaration(decl), closure(closure) {}
    
    Value call(Interpreter* interpreter, 
               const std::vector<Value>& args);
};

class Value {
    std::shared_ptr<FunctionValue> functionValue;
    
public:
    static Value makeFunction(FunctionValue* fn);
    FunctionValue* asFunction();
    bool isFunction() const;
    bool isCallable() const;
};

// src/interpreter/statements/function_interpreter.cpp
void InterpreterCore::visitFunctionDeclStmt(FunctionDeclStmt* stmt) {
    // Create function value with current environment as closure
    auto fnValue = std::make_shared<FunctionValue>(
        stmt->name,
        stmt,
        currentEnvironment  // Capture current scope
    );
    
    Value fn = Value::makeFunction(fnValue.get());
    
    // Define in current environment
    currentEnvironment->define(stmt->name, fn);
    
    DEBUG_PRINT("Defined function: %s with %zu parameters",
                stmt->name.c_str(), stmt->parameters.size());
}

// src/interpreter/expressions/call_evaluator.cpp
Value InterpreterCore::visitCallExpr(CallExpr* expr) {
    // Evaluate callee
    expr->callee->accept(*this);
    Value callee = lastValue;
    
    if (!callee.isCallable()) {
        throw RuntimeError("Cannot call non-callable value");
    }
    
    // Evaluate arguments
    std::vector<Value> args;
    for (auto& argExpr : expr->arguments) {
        argExpr->accept(*this);
        args.push_back(lastValue);
    }
    
    // Call function
    return callee.asFunction()->call(this, args);
}

Value FunctionValue::call(Interpreter* interpreter, 
                          const std::vector<Value>& args) {
    // Check argument count
    size_t minArgs = 0;
    size_t maxArgs = parameters.size();
    
    for (const auto& param : parameters) {
        if (param.defaultValue == nullptr) {
            minArgs++;
        }
    }
    
    if (args.size() < minArgs || args.size() > maxArgs) {
        throw RuntimeError(
            "Function '" + name + "' expects " + 
            std::to_string(minArgs) + "-" + std::to_string(maxArgs) + 
            " arguments, got " + std::to_string(args.size())
        );
    }
    
    // Create new environment for function scope
    auto fnEnv = new Environment(closure);  // Parent is closure environment
    
    // Bind parameters
    for (size_t i = 0; i < parameters.size(); i++) {
        Value argValue;
        
        if (i < args.size()) {
            argValue = args[i];
        } else if (parameters[i].defaultValue) {
            // Evaluate default value
            parameters[i].defaultValue->accept(*interpreter);
            argValue = interpreter->lastValue;
        } else {
            throw RuntimeError("Missing argument: " + parameters[i].name);
        }
        
        fnEnv->define(parameters[i].name, argValue);
    }
    
    // Execute function body
    auto previousEnv = interpreter->currentEnvironment;
    interpreter->currentEnvironment = fnEnv;
    
    try {
        declaration->body->accept(*interpreter);
        
        // Restore environment
        interpreter->currentEnvironment = previousEnv;
        delete fnEnv;
        
        // Return None if no explicit return
        return Value();
        
    } catch (const ReturnException& ret) {
        // Restore environment
        interpreter->currentEnvironment = previousEnv;
        delete fnEnv;
        
        return ret.value;
    }
}

// include/interpreter/core/return_exception.h
class ReturnException : public std::exception {
public:
    Value value;
    
    explicit ReturnException(const Value& v) : value(v) {}
};

// src/interpreter/statements/return_interpreter.cpp
void InterpreterCore::visitReturnStmt(ReturnStmt* stmt) {
    Value returnValue;
    
    if (stmt->value) {
        stmt->value->accept(*this);
        returnValue = lastValue;
    }
    
    throw ReturnException(returnValue);
}
```

### Tests
**File:** `tests/spec_rules/functions_basic.s`
```s
# اختبار دالة بسيطة / Test simple function
دالة رقم مضاعفة(رقم ع)
    إرجاع ع * 2
نهاية

اطبع(مضاعفة(5))    # Expected: 10
اطبع(مضاعفة(21))   # Expected: 42

# اختبار دالة بدون إرجاع / Test function without return
دالة تحية(نص اسم)
    اطبع("مرحبا يا {اسم}!")
نهاية

تحية("أحمد")  # Expected: "مرحبا يا أحمد!"

# اختبار عدة معاملات / Test multiple parameters
دالة رقم جمع_ثلاثة(رقم أ، رقم ب، رقم ج)
    إرجاع أ + ب + ج
نهاية
}

اطبع(جمع_ثلاثة(1، 2، 3))  # Expected: 6
```

**Expected Output:**
```
10
42
مرحبا يا أحمد!
6
```

### Logging
```cpp
// src/parser/parser_functions.cpp
DEBUG_PRINT("Parsing function: %s with %zu parameters", 
            name.c_str(), params.size());

// src/interpreter/statements/function_interpreter.cpp
DEBUG_PRINT("Calling function: %s with %zu arguments",
            name.c_str(), args.size());

DEBUG_PRINT("Function %s returned: %s",
            name.c_str(), result.toString().c_str());
```

---

## 2️⃣ Default Parameters (المعاملات الافتراضية)

### Syntax / البناء النحوي (EBNF)
```ebnf
param_with_default ::= IDENTIFIER [':' type] '=' expression
```


### Semantics
- **Evaluation:** Default values evaluated at call time, not definition time
- **Order:** Parameters with defaults must come after non-default parameters
- **Scope:** Default expressions have access to closure scope
**الشرح بالعربية:**
- `param_with_default`: معامل له قيمة افتراضية يتكون من: اسم المعامل، نوع اختياري بعد نقطتين، ثم علامة = متبوعة بتعبير القيمة الافتراضية

### Semantics / الدلالات
- **Evaluation / التقييم:** القيم الافتراضية يتم تقييمها وقت استدعاء الدالة وليس وقت تعريفها
- **Order / الترتيب:** المعاملات ذات القيم الافتراضية يجب أن تأتي بعد المعاملات الإلزامية
- **Scope / النطاق:** تعبيرات القيم الافتراضية لها إمكانية الوصول إلى نطاق الإغلاق

### Examples

```s
# معامل افتراضي واحد / Single default parameter
دالة رقم قوة(رقم أساس، رقم أس = 2)
    إرجاع أساس ** أس
نهاية

اطبع(قوة(5))      # 25 (uses default: 5^2)
اطبع(قوة(2, 3))   # 8  (2^3)

# عدة معاملات افتراضية / Multiple default parameters
دالة تحية(نص اسم، نص تحية = "مرحبا"، نص نهاية = "!")
    اطبع(تحية + " يا " + اسم + نهاية)
نهاية

تحية("أحمد")                    # "مرحبا يا أحمد!"
تحية("سارة"، "أهلا")            # "أهلا يا سارة!"
تحية("محمد"، "السلام عليكم"، ".") # "السلام عليكم يا محمد."

# قيم افتراضية معقدة / Complex default values
دالة مصفوفة أنشئ_قائمة(رقم حجم، أي قيمة_افتراضية = 0)
    مصفوفة نتيجة = []
    لكل ع في مدى(حجم)
        نتيجة.أضف(قيمة_افتراضية)
    نهاية
    إرجاع نتيجة
نهاية

مصفوفة أصفار = أنشئ_قائمة(3)        # [0, 0, 0]
مصفوفة واحدات = أنشئ_قائمة(3, 1)    # [1, 1, 1]
```

### Implementation Notes

```cpp
// src/interpreter/expressions/call_evaluator.cpp (continued)
Value FunctionValue::call(Interpreter* interpreter, 
                          const std::vector<Value>& args) {
    // ... (previous code)
    
    // Bind parameters with defaults
    for (size_t i = 0; i < parameters.size(); i++) {
        Value argValue;
        
        if (i < args.size()) {
            // Use provided argument
            argValue = args[i];
        } else if (parameters[i].defaultValue) {
            // Evaluate default value in closure environment
            auto prevEnv = interpreter->currentEnvironment;
            interpreter->currentEnvironment = closure;
            
            parameters[i].defaultValue->accept(*interpreter);
            argValue = interpreter->lastValue;
            
            interpreter->currentEnvironment = prevEnv;
        } else {
            throw RuntimeError("Missing required argument: " + parameters[i].name);
        }
        
        fnEnv->define(parameters[i].name, argValue);
        
        DEBUG_PRINT("Bound parameter '%s' = %s",
                    parameters[i].name.c_str(),
                    argValue.toString().c_str());
    }
    
    // ... (rest of function execution)
}
```

### Tests
**File:** `tests/spec_rules/functions_defaults.s`
```s
دالة طباعة_معلومات(نص اسم، رقم عمر = 0، نص مدينة = "غير معروف")
    اطبع(ن"{اسم}، {عمر}، {مدينة}")
نهاية

طباعة_معلومات("أحمد")              # Expected: "أحمد، 0، غير معروف"
طباعة_معلومات("سارة"، 25)          # Expected: "سارة، 25، غير معروف"
طباعة_معلومات("محمد"، 30، "الرياض") # Expected: "محمد، 30، الرياض"
```

---

## 3️⃣ Lambda Functions (الدوال المجهولة)

### Syntax / البناء النحوي (EBNF)
```ebnf
lambda_expr ::= 'دالة' [type] '(' [param_list] ')' '=>' expression
              | '(' [param_list] ')' '=>' expression
```

**الشرح بالعربية:**
- `lambda_expr`: دالة مجهولة (lambda) إما تبدأ بكلمة 'دالة' متبوعة بنوع الإرجاع الاختياري قبل الأقواس، أو بدونها
- الصيغة الكاملة: `دالة [نوع] (معاملات) => تعبير` - نوع الإرجاع يأتي بعد كلمة 'دالة' وقبل الأقواس
- الصيغة المختصرة: `(معاملات) => تعبير` - بدون كلمة 'دالة' ونوع الإرجاع

### Semantics / الدلالات
- **Anonymous / مجهولة:** ليس لها اسم دالة
- **Single Expression / تعبير واحد:** الجسم عبارة عن تعبير واحد (إرجاع ضمني)
- **First-Class / من الدرجة الأولى:** يمكن تمريرها كمعاملات أو إرجاعها من دوال
- **Closures / إغلاقات:** تحتجز المتغيرات من النطاق المحيط

### Examples

```s
# لامدا بسيطة (صيغة مختصرة) / Simple lambda (short form)
أي مربع = (رقم ع) => ع * ع
اطبع(مربع(5))  # 25

# لامدا بنوع إرجاع صريح (صيغة كاملة) / Lambda with explicit return type (full form)
أي مكعب = دالة رقم (رقم ع) => ع * ع * ع
اطبع(مكعب(3))  # 27

# لامدا مع عدة معاملات / Lambda with multiple parameters
أي جمع = (رقم أ، رقم ب) => أ + ب
اطبع(جمع(3، 7))  # 10

# لامدا مع نوع إرجاع ومعاملات متعددة / Lambda with return type and multiple parameters
أي ضرب = دالة رقم (رقم أ، رقم ب) => أ * ب
اطبع(ضرب(4، 5))  # 20

# لامدا بدون معاملات / Lambda without parameters
أي احصل_على_42 = () => 42
اطبع(احصل_على_42())  # 42

# لامدا بنوع إرجاع بدون معاملات / Lambda with return type, no parameters
أي نص_ثابت = دالة نص () => "مرحبا"
اطبع(نص_ثابت())  # "مرحبا"

# استخدام مع دوال عليا / Use with higher-order functions
مصفوفة أرقام = [1، 2، 3، 4، 5]
مصفوفة مربعات = أرقام.map((ع) => ع * ع)
اطبع(مربعات)  # [1، 4، 9، 16، 25]

# تصفية / Filtering
مصفوفة زوجية = أرقام.filter((ع) => ع % 2 == 0)
اطبع(زوجية)  # [2، 4]

# طي / Reduce/Fold
رقم مجموع = أرقام.reduce((مجموع، ع) => مجموع + ع، 0)
اطبع(مجموع)  # 15
```

### Token Analysis

**صيغة مختصرة / Short form:** `(رقم ع) => ع * ع`
```
LEFT_PAREN      "("
KEYWORD_TYPE_INT "رقم"
IDENTIFIER      "ع"
RIGHT_PAREN     ")"
ARROW_DOUBLE    "=>"
IDENTIFIER      "ع"
MULTIPLY        "*"
IDENTIFIER      "ع"
```

**صيغة كاملة / Full form:** `دالة رقم (رقم ع) => ع * ع`
```
KEYWORD_FUNC    "دالة"
KEYWORD_TYPE_INT "رقم"        # Return type (before parameters)
LEFT_PAREN      "("
KEYWORD_TYPE_INT "رقم"
IDENTIFIER      "ع"
RIGHT_PAREN     ")"
ARROW_DOUBLE    "=>"
IDENTIFIER      "ع"
MULTIPLY        "*"
IDENTIFIER      "ع"
```

### AST Representation

**صيغة مختصرة / Short form:**
```
LambdaExpr
├── returnType: null (inferred)
├── parameters: [
│   └── Parameter
│       ├── name: "ع"
│       └── type: INTEGER
│   ]
└── body: BinaryExpr
    ├── left: VariableExpr("ع")
    ├── operator: MULTIPLY
    └── right: VariableExpr("ع")
```

**صيغة كاملة / Full form:**
```
LambdaExpr
├── returnType: INTEGER (explicit, before parameters)
├── parameters: [
│   └── Parameter
│       ├── name: "ع"
│       └── type: INTEGER
│   ]
└── body: BinaryExpr
    ├── left: VariableExpr("ع")
    ├── operator: MULTIPLY
    └── right: VariableExpr("ع")
```

### Implementation Notes

```cpp
// include/parser/ast/expression_nodes.h
class LambdaExpr : public Expr {
public:
    std::vector<Parameter> parameters;
    Type* returnType;                     // Optional, comes BEFORE parameters
    std::unique_ptr<Expr> body;           // Single expression
    
    LambdaExpr(std::vector<Parameter> params,
               Type* returnType,
               std::unique_ptr<Expr> body)
        : parameters(std::move(params)),
          returnType(returnType),
          body(std::move(body)) {}
    
    void accept(Visitor& visitor) override;
};

// src/parser/parser_expressions.cpp
ExprPtr ParserCore::parseLambda() {
    Type* returnType = nullptr;
    
    // Check if this is full form: 'دالة' [type] '(' ... ')' '=>' expr
    if (match(TokenType::KEYWORD_FUNC)) {
        // Optional return type BEFORE parameters
        if (isTypeKeyword(peek())) {
            returnType = parseType();
        }
    }
    // Otherwise short form: '(' ... ')' '=>' expr
    
    consume(TokenType::LEFT_PAREN, "Expected '('");
    
    std::vector<Parameter> params;
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            params.push_back(parseParameter());
        } while (match(TokenType::COMMA) || match(TokenType::ARABIC_COMMA));
    }
    
    consume(TokenType::RIGHT_PAREN, "Expected ')'");
    
    consume(TokenType::ARROW_DOUBLE, "Expected '=>'");
    
    auto body = expression();
    
    return std::make_unique<LambdaExpr>(
        std::move(params), returnType, std::move(body)
    );
}

// src/interpreter/expressions/lambda_evaluator.cpp
Value InterpreterCore::visitLambdaExpr(LambdaExpr* expr) {
    // Create anonymous function value
    auto fnValue = std::make_shared<FunctionValue>(
        "<lambda>",
        expr,
        currentEnvironment  // Capture closure
    );
    
    return Value::makeFunction(fnValue.get());
}

// For lambda calls:
Value FunctionValue::callLambda(Interpreter* interpreter,
                                const std::vector<Value>& args) {
    auto lambda = dynamic_cast<LambdaExpr*>(declaration);
    
    // ... (parameter binding same as regular function)
    
    // Evaluate body expression (not a block statement)
    lambda->body->accept(*interpreter);
    return interpreter->lastValue;  // Implicit return
}
```

### Tests
**File:** `tests/spec_rules/functions_lambda.s`
```s
# لامدا بسيطة (صيغة مختصرة) / Simple lambda (short form)
أي ضعف = (رقم ع) => ع * 2
اطبع(ضعف(21))  # Expected: 42

# لامدا بنوع إرجاع صريح (صيغة كاملة) / Lambda with explicit return type (full form)
أي تربيع = دالة رقم (رقم س) => س * س
اطبع(تربيع(7))  # Expected: 49

# لامدا مع map
مصفوفة أرقام = [1، 2، 3]
مصفوفة مربعات = أرقام.map((ع) => ع * ع)
اطبع(مربعات)   # Expected: [1، 4، 9]

# لامدا مع نوع إرجاع في map / Lambda with return type in map
مصفوفة نصوص = أرقام.map(دالة نص (رقم ع) => ن"رقم {ع}")
اطبع(نصوص)     # Expected: ["رقم 1"، "رقم 2"، "رقم 3"]

# لامدا مع filter
مصفوفة أكبر_من_2 = أرقام.filter((ع) => ع > 2)
اطبع(أكبر_من_2)  # Expected: [3]

# لامدا معقدة بنوع إرجاع / Complex lambda with return type
أي معالج = دالة نص (رقم ع، نص بادئة) => بادئة + ن": {ع * 10}"
اطبع(معالج(5، "النتيجة"))  # Expected: "النتيجة: 50"
```

---

## 4️⃣ Closures (الإغلاقات)

### Syntax / البناء النحوي
الإغلاقات تستخدم نفس صيغة الدوال أو اللامدا العادية، لكنها تحتجز المتغيرات من النطاق المحيط.

Closures use standard function/lambda syntax but capture variables from enclosing scope.


### Semantics
- **Capture:** Functions capture references to variables in enclosing scope
- **Lifetime:** Captured variables live as long as the closure lives
- **Modification:** Closures can modify captured variables

### Semantics / الدلالات
- **Capture / الاحتجاز:** الدوال تحتجز مراجع للمتغيرات في النطاق المحيط
- **Lifetime / العمر:** المتغيرات المحتجزة تظل حية طالما الإغلاق حي
- **Modification / التعديل:** الإغلاقات يمكنها تعديل المتغيرات المحتجزة

### Examples

```s
# إغلاق بسيط / Simple closure
دالة دالة أنشئ_عداد()
    رقم عدد = 0
    
    إرجاع () => {
        عدد = عدد + 1
        إرجاع عدد
    }
نهاية

أي عداد1 = أنشئ_عداد()
اطبع(عداد1())  # 1
اطبع(عداد1())  # 2
اطبع(عداد1())  # 3

أي عداد2 = أنشئ_عداد()
اطبع(عداد2())  # 1 (independent counter)

# إغلاق مع معاملات / Closure with parameters
دالة دالة أنشئ_مضاعف(رقم ضارب)
    إرجاع (رقم ع) => ع * ضارب
نهاية

أي ضعف = أنشئ_مضاعف(2)
أي ثلاثة_أضعاف = أنشئ_مضاعف(3)

اطبع(ضعف(5))          # 10
اطبع(ثلاثة_أضعاف(5))  # 15

# إغلاق معقد / Complex closure
دالة خريطة أنشئ_آلة_حاسبة()
    رقم ذاكرة = 0
    
    إرجاع {
        "اجمع": (رقم ع) => {
            ذاكرة = ذاكرة + ع
            إرجاع ذاكرة
        }،
        "اطرح": (رقم ع) => {
            ذاكرة = ذاكرة - ع
            إرجاع ذاكرة
        }،
        "احصل": () => ذاكرة،
        "امسح": () => { ذاكرة = 0 }
    }
نهاية

خريطة calc = أنشئ_آلة_حاسبة()
اطبع(calc["اجمع"](10))  # 10
اطبع(calc["اجمع"](5))   # 15
اطبع(calc["اطرح"](3))   # 12
اطبع(calc["احصل"]())    # 12
```

### Implementation Notes

```cpp
# include/data/scope/environment.h
class Environment {
    Environment* parent;
    std::map<std::string, Value> variables;
    std::set<std::string> capturedVariables;  // Track captured vars
    
public:
    Environment(Environment* parent = nullptr);
    
    // For closures: create a copy of captured variables
    Environment* createClosure(const std::set<std::string>& captures);
    
    void define(const std::string& name, const Value& value);
    Value get(const std::string& name);
    void set(const std::string& name, const Value& value);
    
    bool has(const std::string& name) const;
    
    // Mark variable as captured
    void markCaptured(const std::string& name);
};

// src/data/scope/environment.cpp
Environment* Environment::createClosure(const std::set<std::string>& captures) {
    auto closure = new Environment(nullptr);  // No parent - self-contained
    
    // Copy captured variables
    for (const auto& name : captures) {
        if (has(name)) {
            closure->define(name, get(name));
            markCaptured(name);
        }
    }
    
    return closure;
}

// src/interpreter/core/closure_analyzer.cpp
class ClosureAnalyzer {
public:
    // Analyze function body to find captured variables
    std::set<std::string> analyzeCapturedVariables(
        Stmt* functionBody,
        const std::set<std::string>& localVars,
        Environment* enclosingScope
    ) {
        std::set<std::string> captured;
        
        // Traverse AST and find all variable references
        for (auto& varRef : findAllVariableReferences(functionBody)) {
            // If variable is not local and exists in enclosing scope
            if (localVars.find(varRef) == localVars.end() &&
                enclosingScope->has(varRef)) {
                captured.insert(varRef);
            }
        }
        
        return captured;
    }
};
```

### Tests
**File:** `tests/spec_rules/functions_closures.s`
```s
# اختبار إغلاق / Closure test
دالة دالة مصنع_مضيف(رقم ع)
    إرجاع (رقم س) => س + ع
نهاية

أي أضف_10 = مصنع_مضيف(10)
أي أضف_20 = مصنع_مضيف(20)

اطبع(أضف_10(5))  # Expected: 15
اطبع(أضف_20(5))  # Expected: 25

# اختبار تعديل متغير محتجز / Test modifying captured variable
دالة دالة عداد()
    رقم ن = 0
    إرجاع () => {
        ن = ن + 1
        إرجاع ن
    }
نهاية

أي عد = عداد()
اطبع(عد())  # Expected: 1
اطبع(عد())  # Expected: 2
اطبع(عد())  # Expected: 3
```

---

## 5️⃣ Higher-Order Functions (الدوال العليا)


### Semantics
- **Function as Parameter:** Functions can accept other functions as arguments
- **Function as Return:** Functions can return other functions
- **Common Patterns:** map, filter, reduce, compose

### Semantics / الدلالات
- **Function as Parameter / دالة كمعامل:** الدوال يمكنها قبول دوال أخرى كمعاملات
- **Function as Return / دالة كإرجاع:** الدوال يمكنها إرجاع دوال أخرى
- **Common Patterns / الأنماط الشائعة:** map (تطبيق)، filter (تصفية)، reduce (طي)، compose (تركيب)

### Examples

```s
# دالة تقبل دالة / Function accepting function
دالة رقم طبق(دالة ف، رقم ع)
    إرجاع ف(ع)
نهاية

أي مربع = (رقم ع) => ع * ع
اطبع(طبق(مربع، 5))  # 25

# دالة تعيد دالة / Function returning function
دالة دالة مؤلف(دالة ف، دالة ج)
    إرجاع (أي ع) => ف(ج(ع))
نهاية

أي ضعف = (رقم ع) => ع * 2
أي زائد_واحد = (رقم ع) => ع + 1

أي ضعف_ثم_زائد_واحد = مؤلف(زائد_واحد، ضعف)
اطبع(ضعف_ثم_زائد_واحد(5))  # 11 (5*2 + 1)

# map implementation
دالة مصفوفة map(مصفوفة قائمة، دالة ف)
    مصفوفة نتيجة = []
    لكل عنصر في قائمة
        نتيجة.أضف(ف(عنصر))
    نهاية
    إرجاع نتيجة
نهاية

مصفوفة أرقام = [1، 2، 3، 4]
مصفوفة مربعات = map(أرقام، (ع) => ع * ع)
اطبع(مربعات)  # [1، 4، 9، 16]

# filter implementation
دالة مصفوفة filter(مصفوفة قائمة، دالة شرط)
    مصفوفة نتيجة = []
    لكل عنصر في قائمة
        إذا شرط(عنصر)
            نتيجة.أضف(عنصر)
        نهاية
    نهاية
    إرجاع نتيجة
نهاية

مصفوفة زوجية = filter(أرقام، (ع) => ع % 2 == 0)
اطبع(زوجية)  # [2، 4]

# reduce implementation
دالة أي reduce(مصفوفة قائمة، دالة ف، أي قيمة_أولية)
    أي مجمع = قيمة_أولية
    لكل عنصر في قائمة
        مجمع = ف(مجمع، عنصر)
    نهاية
    إرجاع مجمع
نهاية

رقم مجموع = reduce(أرقام، (مجمع، ع) => مجمع + ع، 0)
اطبع(مجموع)  # 10
```

---

## 📝 Tasks (مهام التنفيذ)

### Task 1: Basic Functions (3 أيام)
- [ ] Function declaration parsing
- [ ] Parameter binding
- [ ] Function calls
- [ ] Return statements
- **Files:** `src/parser/parser_functions.cpp`, `src/interpreter/statements/function_interpreter.cpp`
- **Tests:** `tests/spec_rules/functions_basic.s`
- **Acceptance:** All basic function tests pass

### Task 2: Default Parameters (يوم واحد)
- [ ] Parse default values
- [ ] Evaluate defaults at call time
- [ ] Validate parameter order
- **Files:** `src/parser/parser_functions.cpp`, `src/interpreter/expressions/call_evaluator.cpp`
- **Tests:** `tests/spec_rules/functions_defaults.s`

### Task 3: Lambda Functions (يومان)
- [ ] Lambda expression parsing
- [ ] Lambda evaluation
- [ ] Integration with higher-order functions
- **Files:** `src/parser/parser_expressions.cpp`, `src/interpreter/expressions/lambda_evaluator.cpp`
- **Tests:** `tests/spec_rules/functions_lambda.s`

### Task 4: Closures (3 أيام)
- [ ] Environment capture analysis
- [ ] Closure creation
- [ ] Captured variable lifetime management
- **Files:** `src/data/scope/environment.cpp`, `src/interpreter/core/closure_analyzer.cpp`
- **Tests:** `tests/spec_rules/functions_closures.s`

### Task 5: Higher-Order Functions (يوم واحد)
- [ ] Built-in map, filter, reduce
- [ ] Function composition utilities
- **Files:** `src/stdlib/core/functional.cpp`
- **Tests:** `tests/spec_rules/functions_higher_order.s`

**Total Time:** 10 أيام (80 ساعة)

---

## 🔧 Git Workflow

```powershell
# إنشاء فرع / Create branch
git checkout -b feat/spec/functions

# إضافة الملفات / Add files
git add docs/language_spec/rules/02_functions.md
git add tests/spec_rules/functions_*.s
git add include/parser/ast/function_nodes.h
git add src/parser/parser_functions.cpp
git add src/interpreter/statements/function_interpreter.cpp
git add src/interpreter/expressions/call_evaluator.cpp
git add src/interpreter/expressions/lambda_evaluator.cpp
git add src/data/scope/environment.cpp

# Commit
git commit -m "[spec/functions] Add complete function system rules

- Basic function declarations with parameters
- Default parameters with lazy evaluation
- Lambda expressions with closure support
- Closures with variable capture
- Higher-order functions (map, filter, reduce)
- 5 test files with 60+ test cases"

# Push
git push --set-upstream origin feat/spec/functions
```

---

**آخر تحديث:** 5 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 55+
