# 🐍 ميزات Python في لغة "ص" / Python Features in Sad Language

**التاريخ:** 7 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مرجع تفصيلي لميزات Python

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق كيفية ترجمة وتكييف **ميزات Python الشهيرة** إلى لغة "ص"، بما في ذلك List/Dict Comprehensions، Generators، Decorators، Context Managers، ونظام الاستثناءات. الهدف هو الحفاظ على سهولة الاستخدام والوضوح مع دعم الصياغة العربية.

### (EN) Description
This file documents how to translate and adapt **popular Python features** into Sad language, including List/Dict Comprehensions, Generators, Decorators, Context Managers, and Exception System. The goal is to maintain ease of use and clarity while supporting Arabic syntax.

---

## 📦 الاعتماديات / Dependencies

- ✅ `docs/language_spec/02_functions.md` - Function system
- ✅ `docs/language_spec/03_oop.md` - OOP for context managers
- ✅ `docs/language_spec/04_syntax.md` - Control flow
- ✅ `plans/imp/12_interpreter_runtime_plan.md` - Runtime implementation
- ✅ `plans/imp/13_stdlib_and_modules_plan.md` - Standard library

---

## 📋 قائمة الميزات / Features List

| # | الميزة (AR) | Feature (EN) | الحالة | الأولوية |
|---|-----------|-----------|-------|---------|
| 1 | تضمينات القوائم | List Comprehensions | ✅ Core | P0 |
| 2 | تضمينات القواميس | Dict Comprehensions | ✅ Core | P0 |
| 3 | المولدات | Generators | ✅ Core | P0 |
| 4 | المزخرفات | Decorators | 🚧 Phase 2 | P1 |
| 5 | مديرو السياق | Context Managers | 🚧 Phase 2 | P1 |
| 6 | الاستثناءات | Exceptions | ✅ Core | P0 |
| 7 | تعبيرات Lambda | Lambda Expressions | ✅ Core | P0 |
| 8 | الفك التلقائي | Unpacking | ✅ Core | P1 |
| 9 | تعبيرات الشروط | Conditional Expressions | ✅ Core | P1 |
| 10 | المعاملات المسماة | Keyword Arguments | ✅ Core | P1 |

---

## 1️⃣ List Comprehensions (تضمينات القوائم)

### Syntax (EBNF)
```ebnf
list_comprehension ::= '[' expression 'لكل' IDENTIFIER 'في' iterable [if_clause] ']'

if_clause ::= 'إذا' condition
            | 'إذا' '(' condition ')'

condition ::= expression

// Alternative English-style
list_comprehension ::= '[' expression 'for' IDENTIFIER 'in' iterable ['if' '(' condition ')' | 'if' condition] ']'
```

**الشرح بالعربية:**
- `if_clause`: يدعم الشرط بدون أقواس `إذا شرط` أو مع أقواس `إذا (شرط)`
- الأقواس اختيارية لتحسين الوضوح في الشروط المعقدة

### الشرح بالعربية
تضمينات القوائم (List Comprehensions) تسمح بإنشاء قوائم جديدة من قوائم موجودة بطريقة موجزة وقابلة للقراءة. الصيغة الأساسية: `[تعبير لكل عنصر في قائمة]`

### (EN) Explanation
List Comprehensions allow creating new lists from existing iterables in a concise and readable way. Basic syntax: `[expression for item in list]`

### Semantics
- **Evaluation Order:** The iterable is evaluated first, then for each element, the expression is evaluated
- **Filtering:** Optional `if` clause filters elements before applying the expression
- **Type:** Result is always a `مصفوفة` (Array/List)
- **Scope:** Loop variable is local to the comprehension
- **Parentheses:** Optional parentheses around condition are supported for clarity (both `إذا شرط` and `إذا (شرط)` are valid)

### Examples

```s
// مثال بسيط / Simple example
// إنشاء قائمة مربعات الأعداد / Create list of squares
مصفوفة مربعات = [س * س لكل س في [1، 2، 3، 4، 5]]
اطبع(مربعات)  // [1، 4، 9، 16، 25]

// مع شرط / With condition
// الأعداد الزوجية فقط / Even numbers only
مصفوفة أعداد = [1، 2، 3، 4، 5، 6، 7، 8، 9، 10]
مصفوفة زوجية = [ع لكل ع في أعداد إذا ع % 2 == 0]
اطبع(زوجية)  // [2، 4، 6، 8، 10]

// مع أقواس في الشرط / With parentheses in condition
// أعداد أكبر من 5 / Numbers greater than 5
مصفوفة كبيرة = [ع لكل ع في أعداد إذا (ع > 5)]
اطبع(كبيرة)  // [6، 7، 8، 9، 10]

// شرط معقد مع أقواس / Complex condition with parentheses
مصفوفة مرشح = [ع لكل ع في أعداد إذا (ع > 3 && ع < 8)]
اطبع(مرشح)  // [4، 5، 6، 7]

// تطبيق دالة / Applying function
دالة رقم مضاعف(رقم س)
    إرجاع س * 2
نهاية

مصفوفة نتيجة = [مضاعف(س) لكل س في [1، 2، 3]]
اطبع(نتيجة)  // [2، 4، 6]

// تضمين متداخل / Nested comprehension
// إنشاء مصفوفة من الأزواج / Create array of pairs
مصفوفة أزواج = [[س، ص] لكل س في [1، 2] لكل ص في [3، 4]]
اطبع(أزواج)  // [[1، 3]، [1، 4]، [2، 3]، [2، 4]]

// مع نصوص / With strings
مصفوفة أسماء = ["أحمد"، "فاطمة"، "محمد"]
مصفوفة تحيات = ["مرحبا {اسم}" لكل اسم في أسماء]
اطبع(تحيات)  // ["مرحبا أحمد"، "مرحبا فاطمة"، "مرحبا محمد"]

// English-style syntax also supported
مصفوفة squares_en = [x * x for x in [1, 2, 3, 4, 5]]
```

### Token Analysis

**Example 1: Without parentheses in condition**
```s
[ع لكل ع في أعداد إذا ع % 2 == 0]
```
```
LEFT_BRACKET       "["
IDENTIFIER         "ع"
KEYWORD_FOR        "لكل"
IDENTIFIER         "ع"
KEYWORD_IN         "في"
IDENTIFIER         "أعداد"
KEYWORD_IF         "إذا"
IDENTIFIER         "ع"
MODULO             "%"
INTEGER_LITERAL    "2"
EQUAL_EQUAL        "=="
INTEGER_LITERAL    "0"
RIGHT_BRACKET      "]"
```

**Example 2: With parentheses in condition**
```s
[ع لكل ع في أعداد إذا (ع > 5)]
```
```
LEFT_BRACKET       "["
IDENTIFIER         "ع"
KEYWORD_FOR        "لكل"
IDENTIFIER         "ع"
KEYWORD_IN         "في"
IDENTIFIER         "أعداد"
KEYWORD_IF         "إذا"
LEFT_PAREN         "("        // Optional
IDENTIFIER         "ع"
GREATER            ">"
INTEGER_LITERAL    "5"
RIGHT_PAREN        ")"        // Optional
RIGHT_BRACKET      "]"
```

### AST Representation

**Note:** Both forms (with and without parentheses) produce identical AST

```
ListComprehension
├── expression: BinaryExpr
│   ├── left: VarExpr("ع")
│   ├── operator: MODULO
│   └── right: LiteralExpr(2)
├── variable: "ع"
├── iterable: VarExpr("أعداد")
└── condition: BinaryExpr
    ├── left: BinaryExpr(ع % 2)
    ├── operator: EQUAL_EQUAL
    └── right: LiteralExpr(0)

// الأقواس لا تؤثر على AST، فقط على الوضوح
// Parentheses don't affect AST, only clarity
```

### Implementation Notes

**Files to Modify:**
```cpp
// include/parser/ast/comprehension_nodes.h
class ListComprehension : public Expr {
public:
    std::unique_ptr<Expr> expression;       // The expression to evaluate
    std::string variable;                    // Loop variable name
    std::unique_ptr<Expr> iterable;         // The iterable to loop over
    std::unique_ptr<Expr> condition;        // Optional filter condition
    
    ListComprehension(std::unique_ptr<Expr> expr,
                     std::string var,
                     std::unique_ptr<Expr> iter,
                     std::unique_ptr<Expr> cond = nullptr)
        : expression(std::move(expr)),
          variable(std::move(var)),
          iterable(std::move(iter)),
          condition(std::move(cond)) {}
    
    void accept(Visitor& visitor) override;
};

// include/parser/parser_core.h
class ParserCore {
    // Parse list comprehension
    ExprPtr parseListComprehension();
    
    // Check if next tokens form a comprehension
    bool isListComprehension();
};

// src/parser/parser_comprehensions.cpp
ExprPtr ParserCore::parseListComprehension() {
    consume(TokenType::LEFT_BRACKET, "Expected '['");
    
    // Parse expression
    auto expr = expression();
    
    // Expect 'لكل' or 'for'
    if (!match({TokenType::KEYWORD_FOR, TokenType::KEYWORD_FOR_EN})) {
        error("Expected 'لكل' or 'for' in list comprehension");
    }
    
    // Parse loop variable
    Token var = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    // Expect 'في' or 'in'
    if (!match({TokenType::KEYWORD_IN, TokenType::KEYWORD_IN_EN})) {
        error("Expected 'في' or 'in' in list comprehension");
    }
    
    // Parse iterable
    auto iter = expression();
    
    // Optional condition
    std::unique_ptr<Expr> cond = nullptr;
    if (match({TokenType::KEYWORD_IF, TokenType::KEYWORD_IF_EN})) {
        // Check for optional parentheses
        bool hasParens = false;
        if (check(TokenType::LEFT_PAREN)) {
            hasParens = true;
            advance();  // consume '('
        }
        
        // Parse condition expression
        cond = expression();
        
        // Consume closing paren if we had opening paren
        if (hasParens) {
            consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
        }
    }
    
    consume(TokenType::RIGHT_BRACKET, "Expected ']'");
    
    return std::make_unique<ListComprehension>(
        std::move(expr),
        var.lexeme,
        std::move(iter),
        std::move(cond)
    );
}

bool ParserCore::isListComprehension() {
    // Look ahead to see if this is a comprehension
    // Pattern: [ expr لكل var في ...
    if (!check(TokenType::LEFT_BRACKET)) return false;
    
    size_t saved = current;
    advance();  // skip '['
    
    // Skip expression tokens until we find 'لكل' or 'for'
    int depth = 0;
    while (!isAtEnd()) {
        if (check(TokenType::LEFT_BRACKET) || 
            check(TokenType::LEFT_PAREN)) {
            depth++;
        } else if (check(TokenType::RIGHT_BRACKET) || 
                   check(TokenType::RIGHT_PAREN)) {
            if (depth == 0) break;
            depth--;
        } else if ((check(TokenType::KEYWORD_FOR) || 
                    check(TokenType::KEYWORD_FOR_EN)) && depth == 0) {
            current = saved;
            return true;
        }
        advance();
    }
    
    current = saved;
    return false;
}

// src/interpreter/expressions/comprehension_evaluator.cpp
Value InterpreterCore::visitListComprehension(ListComprehension* expr) {
    // Evaluate iterable
    expr->iterable->accept(*this);
    Value iterableValue = lastValue;
    
    if (!iterableValue.isArray()) {
        throw RuntimeError("List comprehension requires an iterable");
    }
    
    // Create new array for result
    std::vector<Value> result;
    
    // Create new scope for loop variable
    auto compEnv = new Environment(currentEnvironment);
    auto prevEnv = currentEnvironment;
    currentEnvironment = compEnv;
    
    // Iterate over elements
    const auto& elements = iterableValue.asArray();
    for (const auto& element : elements) {
        // Bind loop variable
        currentEnvironment->define(expr->variable, element);
        
        // Check condition if present
        bool includeElement = true;
        if (expr->condition) {
            expr->condition->accept(*this);
            if (!lastValue.isBool()) {
                throw RuntimeError("Comprehension condition must be boolean");
            }
            includeElement = lastValue.asBool();
        }
        
        // Evaluate expression if condition passed
        if (includeElement) {
            expr->expression->accept(*this);
            result.push_back(lastValue);
        }
    }
    
    // Restore environment
    currentEnvironment = prevEnv;
    delete compEnv;
    
    return Value::makeArray(result);
}
```

### Tests
**File:** `tests/spec_rules/comprehensions_list.s`
```s
// اختبار تضمينات القوائم الأساسية / Test basic list comprehensions
مصفوفة مربعات = [س * س لكل س في [1، 2، 3، 4]]
اطبع(مربعات)  // Expected: [1، 4، 9، 16]

// مع شرط بدون أقواس / With condition without parentheses
مصفوفة زوجية = [ع لكل ع في [1، 2، 3، 4، 5، 6] إذا ع % 2 == 0]
اطبع(زوجية)  // Expected: [2، 4، 6]

// مع شرط بأقواس / With condition with parentheses
مصفوفة كبيرة = [ع لكل ع في [1، 2، 3، 4، 5، 6، 7، 8] إذا (ع > 4)]
اطبع(كبيرة)  // Expected: [5، 6، 7، 8]

// شرط معقد مع أقواس / Complex condition with parentheses
مصفوفة نطاق = [ع لكل ع في [1، 2، 3، 4، 5، 6، 7، 8، 9، 10] إذا (ع > 3 && ع <= 7)]
اطبع(نطاق)  // Expected: [4، 5، 6، 7]

// تضمين متداخل / Nested comprehension
مصفوفة ضرب = [س * ص لكل س في [1، 2، 3] لكل ص في [10، 20]]
اطبع(ضرب)  // Expected: [10، 20، 20، 40، 30، 60]
```

**Expected Output:**
```
[1، 4، 9، 16]
[2، 4، 6]
[5، 6، 7، 8]
[4، 5، 6، 7]
[10، 20، 20، 40، 30، 60]
```

### Logging
```cpp
// src/interpreter/expressions/comprehension_evaluator.cpp
DEBUG_PRINT("Evaluating list comprehension with variable: %s", 
            expr->variable.c_str());
DEBUG_PRINT("Iterable size: %zu", elements.size());
DEBUG_PRINT("Result size: %zu", result.size());

// Note: Optional parentheses in condition are handled during parsing
// Both forms produce identical AST and runtime behavior
if (expr->condition) {
    DEBUG_PRINT("Applying filter condition to comprehension");
}
```

---

## 2️⃣ Dict Comprehensions (تضمينات القواميس)

### Syntax (EBNF)
```ebnf
dict_comprehension ::= '{' key_expr ':' value_expr 'لكل' IDENTIFIER 'في' iterable [if_clause] '}'

if_clause ::= 'إذا' condition
            | 'إذا' '(' condition ')'

// Alternative English-style
dict_comprehension ::= '{' key_expr ':' value_expr 'for' IDENTIFIER 'in' iterable ['if' '(' condition ')' | 'if' condition] '}'
```

**الشرح بالعربية:**
- يدعم الشرط بدون أقواس أو مع أقواس مثل list comprehensions

### الشرح بالعربية
تضمينات القواميس تسمح بإنشاء قواميس جديدة بطريقة موجزة. الصيغة: `{مفتاح: قيمة لكل عنصر في قائمة}`

### Examples

```s
// إنشاء قاموس من قائمة / Create dict from list
مصفوفة أعداد = [1، 2، 3، 4، 5]
قاموس مربعات = {ع: ع * ع لكل ع في أعداد}
اطبع(مربعات)  // {1: 1، 2: 4، 3: 9، 4: 16، 5: 25}

// مع شرط / With condition
قاموس زوجية = {ع: ع * 2 لكل ع في [1، 2، 3، 4، 5] إذا ع % 2 == 0}
اطبع(زوجية)  // {2: 4، 4: 8}

// مع شرط بأقواس / With condition with parentheses
قاموس كبيرة = {ع: ع * ع لكل ع في [1، 2، 3، 4، 5، 6] إذا (ع > 3)}
اطبع(كبيرة)  // {4: 16، 5: 25، 6: 36}

// من قائمة أسماء / From names list
مصفوفة أسماء = ["أحمد"، "فاطمة"، "محمد"]
قاموس أطوال = {اسم: اسم.طول() لكل اسم في أسماء}
اطبع(أطوال)  // {"أحمد": 4، "فاطمة": 5، "محمد": 4}

// عكس قاموس / Reverse dictionary
قاموس أصلي = {"أ": 1، "ب": 2، "ج": 3}
قاموس معكوس = {قيمة: مفتاح لكل مفتاح، قيمة في أصلي.عناصر()}
اطبع(معكوس)  // {1: "أ"، 2: "ب"، 3: "ج"}
```

### Implementation Notes
```cpp
// include/parser/ast/comprehension_nodes.h
class DictComprehension : public Expr {
public:
    std::unique_ptr<Expr> keyExpr;
    std::unique_ptr<Expr> valueExpr;
    std::string variable;
    std::unique_ptr<Expr> iterable;
    std::unique_ptr<Expr> condition;
    
    // For iterating over dict items (key, value pairs)
    std::string keyVariable;   // Optional second variable
};

// src/parser/parser_comprehensions.cpp
ExprPtr ParserCore::parseDictComprehension() {
    consume(TokenType::LEFT_BRACE, "Expected '{'");
    
    // Parse key expression
    auto keyExpr = expression();
    
    consume(TokenType::COLON, "Expected ':' in dict comprehension");
    
    // Parse value expression
    auto valueExpr = expression();
    
    // Expect 'لكل' or 'for'
    if (!match({TokenType::KEYWORD_FOR, TokenType::KEYWORD_FOR_EN})) {
        error("Expected 'لكل' or 'for' in dict comprehension");
    }
    
    // Parse loop variable
    Token var = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    // Expect 'في' or 'in'
    if (!match({TokenType::KEYWORD_IN, TokenType::KEYWORD_IN_EN})) {
        error("Expected 'في' or 'in' in dict comprehension");
    }
    
    // Parse iterable
    auto iter = expression();
    
    // Optional condition (with optional parentheses)
    std::unique_ptr<Expr> cond = nullptr;
    if (match({TokenType::KEYWORD_IF, TokenType::KEYWORD_IF_EN})) {
        // Check for optional parentheses
        bool hasParens = false;
        if (check(TokenType::LEFT_PAREN)) {
            hasParens = true;
            advance();  // consume '('
        }
        
        // Parse condition expression
        cond = expression();
        
        // Consume closing paren if we had opening paren
        if (hasParens) {
            consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
        }
    }
    
    consume(TokenType::RIGHT_BRACE, "Expected '}'");
    
    return std::make_unique<DictComprehension>(
        std::move(keyExpr),
        std::move(valueExpr),
        var.lexeme,
        std::move(iter),
        std::move(cond)
    );
}
```

---

## 3️⃣ Generators (المولدات)

### Syntax (EBNF)
```ebnf
generator_function ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block_with_yield

yield_statement ::= 'أنتج' expression
                  | 'yield' expression

generator_expr ::= '(' expression 'لكل' IDENTIFIER 'في' iterable [if_clause] ')'

if_clause ::= 'إذا' condition
            | 'إذا' '(' condition ')'
```

**الشرح بالعربية:**
- تعبيرات المولدات تدعم الأقواس الاختيارية في الشرط مثل list comprehensions

### الشرح بالعربية
المولدات (Generators) هي دوال خاصة تستخدم كلمة `أنتج` (yield) بدلاً من `إرجاع` (return). المولدات تُنتج قيماً واحدة تلو الأخرى وتحافظ على حالتها بين الاستدعاءات، مما يجعلها فعّالة للذاكرة عند العمل مع تسلسلات كبيرة.

### (EN) Explanation
Generators are special functions that use `أنتج` (yield) instead of `إرجاع` (return). They produce values one at a time and maintain their state between calls, making them memory-efficient for large sequences.

### Semantics
- **Lazy Evaluation:** Values are computed on-demand, not all at once
- **State Preservation:** Function state (local variables) preserved between yields
- **Memory Efficient:** Only one item in memory at a time
- **Iterable:** Generators are iterable and can be used in loops

### Examples

```s
// مولد بسيط / Simple generator
// توليد أعداد من 1 إلى ن / Generate numbers from 1 to n
دالة مولد_أعداد(رقم ن)
    رقم ع = 1
    بينما (ع <= ن)
        أنتج ع
        ع = ع + 1
    نهاية
نهاية

// استخدام المولد / Using the generator
لكل رقم في مولد_أعداد(5)
    اطبع(رقم)  // 1، 2، 3، 4، 5
نهاية

// مولد فيبوناتشي / Fibonacci generator
// توليد سلسلة فيبوناتشي لانهائية / Generate infinite Fibonacci sequence
دالة فيبوناتشي()
    رقم أ = 0
    رقم ب = 1
    
    بينما صحيح
        أنتج أ
        رقم مؤقت = أ
        أ = ب
        ب = مؤقت + ب
    نهاية
نهاية

// استخدام مع حد / Use with limit
رقم عداد = 0
لكل رقم في فيبوناتشي()
    إذا عداد >= 10
        اكسر
    نهاية
    اطبع(رقم)
    عداد = عداد + 1
نهاية
// Output: 0، 1، 1، 2، 3، 5، 8، 13، 21، 34

// مولد بمعامل / Generator with parameter
// قراءة ملف سطراً سطراً / Read file line by line
دالة اقرأ_سطور(نص اسم_ملف)
    ملف م = افتح(اسم_ملف، "قراءة")
    
    بينما !م.نهاية()
        نص سطر = م.اقرأ_سطر()
        أنتج سطر
    نهاية
    
    م.أغلق()
نهاية

// تعبير مولد / Generator expression
// مولد مربعات / Squares generator
أي مولد_مربعات = (س * س لكل س في [1، 2، 3، 4، 5])
لكل مربع في مولد_مربعات
    اطبع(مربع)
نهاية

// مولد بشرط / Generator with condition
دالة أعداد_زوجية(رقم بداية، رقم نهاية)
    رقم ع = بداية
    بينما ع <= نهاية
        إذا ع % 2 == 0
            أنتج ع
        نهاية
        ع = ع + 1
    نهاية
نهاية

// استخدام / Usage
لكل زوجي في أعداد_زوجية(1، 10)
    اطبع(زوجي)  // 2، 4، 6، 8، 10
نهاية
```

### AST Representation
```
GeneratorFunction
├── name: "مولد_أعداد"
├── parameters: [Parameter("ن", INTEGER)]
├── body: BlockStmt
│   ├── VarDeclStmt("ع", 1)
│   ├── WhileStmt
│   │   ├── condition: BinaryExpr(ع <= ن)
│   │   └── body: BlockStmt
│   │       ├── YieldStmt(VarExpr("ع"))
│   │       └── AssignStmt(ع، ع + 1)
└── isGenerator: true
```

### Implementation Notes

**Files to Modify:**
```cpp
// include/interpreter/core/generator.h
class Generator {
public:
    FunctionDecl* function;
    Environment* capturedEnv;
    size_t resumePoint;              // Where to resume execution
    std::map<std::string, Value> localVars;  // Preserved state
    bool finished;
    
    Generator(FunctionDecl* func, Environment* env)
        : function(func), 
          capturedEnv(env),
          resumePoint(0),
          finished(false) {}
    
    // Get next value from generator
    Value next();
    
    // Check if generator has more values
    bool hasNext() const { return !finished; }
    
    // Make generator iterable
    Value* begin();
    Value* end();
};

// include/parser/ast/statement_nodes.h
class YieldStmt : public Stmt {
public:
    std::unique_ptr<Expr> value;
    
    YieldStmt(std::unique_ptr<Expr> val)
        : value(std::move(val)) {}
    
    void accept(Visitor& visitor) override;
};

// src/interpreter/core/generator_runtime.cpp
Value Generator::next() {
    if (finished) {
        throw StopIteration();
    }
    
    // Create execution environment
    auto genEnv = new Environment(capturedEnv);
    
    // Restore local variables
    for (const auto& [name, value] : localVars) {
        genEnv->define(name, value);
    }
    
    auto prevEnv = interpreter->currentEnvironment;
    interpreter->currentEnvironment = genEnv;
    
    try {
        // Resume execution from last yield point
        interpreter->resumePoint = resumePoint;
        function->body->accept(*interpreter);
        
        // If we get here, generator finished without yield
        finished = true;
        
    } catch (const YieldException& ye) {
        // Save current state
        resumePoint = interpreter->currentPoint;
        
        // Save all local variables
        localVars.clear();
        for (const auto& [name, value] : genEnv->getAllBindings()) {
            localVars[name] = value;
        }
        
        interpreter->currentEnvironment = prevEnv;
        delete genEnv;
        
        return ye.value;
    }
    
    interpreter->currentEnvironment = prevEnv;
    delete genEnv;
    
    throw StopIteration();
}

// src/interpreter/statements/yield_interpreter.cpp
void InterpreterCore::visitYieldStmt(YieldStmt* stmt) {
    stmt->value->accept(*this);
    Value yieldValue = lastValue;
    
    DEBUG_PRINT("Yielding value from generator");
    
    // Throw yield exception to unwind stack
    throw YieldException(yieldValue, currentPoint);
}

// Exception classes
class YieldException : public std::exception {
public:
    Value value;
    size_t resumePoint;
    
    YieldException(Value v, size_t point)
        : value(v), resumePoint(point) {}
};

class StopIteration : public std::exception {
public:
    const char* what() const noexcept override {
        return "Generator exhausted";
    }
};
```

### Tests
**File:** `tests/spec_rules/generators.s`
```s
// اختبار مولد بسيط / Test simple generator
دالة عد(رقم ن)
    رقم ع = 0
    بينما ع < ن
        أنتج ع
        ع = ع + 1
    نهاية
نهاية

مصفوفة نتيجة = []
لكل رقم في عد(5)
    نتيجة.أضف(رقم)
نهاية
اطبع(نتيجة)  // Expected: [0، 1، 2، 3، 4]

// اختبار فيبوناتشي / Test Fibonacci
دالة فيبو(رقم حد)
    رقم أ = 0
    رقم ب = 1
    رقم عداد = 0
    
    بينما عداد < حد
        أنتج أ
        رقم مؤقت = أ + ب
        أ = ب
        ب = مؤقت
        عداد = عداد + 1
    نهاية
نهاية

لكل رقم في فيبو(7)
    اطبع(رقم)
نهاية
// Expected: 0، 1، 1، 2، 3، 5، 8
```

---

## 4️⃣ Decorators (المزخرفات)

### Syntax (EBNF)
```ebnf
decorated_function ::= decorator+ function_decl

decorator ::= '@' decorator_name ['(' [arg_list] ')'] NEWLINE
            | '@' IDENTIFIER ['(' [arg_list] ')'] NEWLINE

function_decl ::= 'دالة' [type] IDENTIFIER '(' [param_list] ')' block
```

### الشرح بالعربية
المزخرفات (Decorators) هي دوال تُغلّف دوال أخرى لإضافة وظائف إضافية دون تعديل الدالة الأصلية. تُستخدم كثيراً للتسجيل، التخزين المؤقت، التحقق من الصلاحيات، وقياس الأداء.

### Examples

```s
// مزخرف بسيط / Simple decorator
// مزخرف لقياس الوقت / Timing decorator
دالة قيس_وقت(دالة د)
    دالة مُغلّفة(*معاملات)
        رقم بداية = الوقت.الآن()
        أي نتيجة = د(*معاملات)
        رقم نهاية = الوقت.الآن()
        اطبع("الوقت المستغرق: {نهاية - بداية} ثانية")
        إرجاع نتيجة
    نهاية
    إرجاع مُغلّفة
نهاية

// استخدام المزخرف / Using decorator
@قيس_وقت
دالة رقم حساب_بطيء(رقم ن)
    رقم مجموع = 0
    لكل ع في نطاق(ن)
        مجموع = مجموع + ع
    نهاية
    إرجاع مجموع
نهاية

اطبع(حساب_بطيء(1000000))
// Output:
// الوقت المستغرق: 0.152 ثانية
// 499999500000

// مزخرف مع معاملات / Decorator with parameters
دالة كرّر(رقم مرات)
    دالة مزخرف(دالة د)
        دالة مُغلّفة(*معاملات)
            مصفوفة نتائج = []
            لكل _ في نطاق(مرات)
                نتائج.أضف(د(*معاملات))
            نهاية
            إرجاع نتائج
        نهاية
        إرجاع مُغلّفة
    نهاية
    إرجاع مزخرف
نهاية

@كرّر(3)
دالة نص سلّم()
    إرجاع "مرحبا!"
نهاية

اطبع(سلّم())  // ["مرحبا!"، "مرحبا!"، "مرحبا!"]

// مزخرف للتخزين المؤقت / Memoization decorator
قاموس خبأ = {}

دالة تذكير(دالة د)
    دالة مُغلّفة(*معاملات)
        نص مفتاح = نص(معاملات)
        
        إذا مفتاح في خبأ
            اطبع(ن"استخدام القيمة المخبأة لـ {مفتاح}")
            إرجاع خبأ[مفتاح]
        نهاية
        
        أي نتيجة = د(*معاملات)
        خبأ[مفتاح] = نتيجة
        إرجاع نتيجة
    نهاية
    إرجاع مُغلّفة
نهاية

@تذكير
دالة رقم فيبوناتشي(رقم ن)
    إذا ن <= 1
        إرجاع ن
    نهاية
    إرجاع فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)
نهاية

اطبع(فيبوناتشي(10))  // Much faster with memoization

// مزخرفات متعددة / Multiple decorators
@قيس_وقت
@تذكير
دالة رقم عملية_معقدة(رقم س، رقم ص)
    // حساب معقد / Complex calculation
    إرجاع س ** ص
نهاية
```

### Implementation Notes

```cpp
// include/parser/ast/decorator_nodes.h
class DecoratorExpr : public Expr {
public:
    std::string decoratorName;
    std::vector<std::unique_ptr<Expr>> arguments;
    
    DecoratorExpr(std::string name, 
                  std::vector<std::unique_ptr<Expr>> args = {})
        : decoratorName(std::move(name)),
          arguments(std::move(args)) {}
    
    void accept(Visitor& visitor) override;
};

class DecoratedFunction : public Stmt {
public:
    std::vector<std::unique_ptr<DecoratorExpr>> decorators;
    std::unique_ptr<FunctionDecl> function;
    
    DecoratedFunction(std::vector<std::unique_ptr<DecoratorExpr>> decs,
                     std::unique_ptr<FunctionDecl> func)
        : decorators(std::move(decs)),
          function(std::move(func)) {}
    
    void accept(Visitor& visitor) override;
};

// src/parser/parser_decorators.cpp
StmtPtr ParserCore::parseDecoratedFunction() {
    std::vector<std::unique_ptr<DecoratorExpr>> decorators;
    
    // Parse all decorators
    while (match(TokenType::AT)) {
        Token name = consume(TokenType::IDENTIFIER, "Expected decorator name");
        
        std::vector<std::unique_ptr<Expr>> args;
        if (match(TokenType::LEFT_PAREN)) {
            if (!check(TokenType::RIGHT_PAREN)) {
                do {
                    args.push_back(expression());
                } while (match({TokenType::COMMA, TokenType::ARABIC_COMMA}));
            }
            consume(TokenType::RIGHT_PAREN, "Expected ')'");
        }
        
        decorators.push_back(
            std::make_unique<DecoratorExpr>(name.lexeme, std::move(args))
        );
        
        consume(TokenType::NEWLINE, "Expected newline after decorator");
    }
    
    // Parse the function
    auto func = parseFunctionDecl();
    
    return std::make_unique<DecoratedFunction>(
        std::move(decorators),
        std::move(func)
    );
}

// src/interpreter/statements/decorator_interpreter.cpp
void InterpreterCore::visitDecoratedFunction(DecoratedFunction* stmt) {
    // First, define the original function
    stmt->function->accept(*this);
    Value funcValue = currentEnvironment->get(stmt->function->name);
    
    // Apply decorators in reverse order (bottom to top)
    for (auto it = stmt->decorators.rbegin(); 
         it != stmt->decorators.rend(); 
         ++it) {
        
        DecoratorExpr* decorator = it->get();
        
        // Get decorator function
        Value decoratorFunc = currentEnvironment->get(decorator->decoratorName);
        
        // Evaluate decorator arguments
        std::vector<Value> decoratorArgs;
        for (auto& arg : decorator->arguments) {
            arg->accept(*this);
            decoratorArgs.push_back(lastValue);
        }
        
        // If decorator has arguments, call it first to get the actual decorator
        if (!decoratorArgs.empty()) {
            decoratorFunc = callFunction(decoratorFunc, decoratorArgs);
        }
        
        // Apply decorator to function
        std::vector<Value> args = {funcValue};
        funcValue = callFunction(decoratorFunc, args);
    }
    
    // Replace function with decorated version
    currentEnvironment->assign(stmt->function->name, funcValue);
    
    DEBUG_PRINT("Applied %zu decorators to function: %s",
                stmt->decorators.size(),
                stmt->function->name.c_str());
}
```

### Tests
**File:** `tests/spec_rules/decorators.s`
```s
// اختبار مزخرف بسيط / Test simple decorator
دالة مضاعف(دالة د)
    دالة مُغلّفة(رقم س)
        إرجاع د(س) * 2
    نهاية
    إرجاع مُغلّفة
نهاية

@مضاعف
دالة رقم أضف_واحد(رقم س)
    إرجاع س + 1
نهاية

اطبع(أضف_واحد(5))  // Expected: 12 (not 6)
```

---

## 5️⃣ Context Managers (مديرو السياق)

### Syntax (EBNF)
```ebnf
with_statement ::= 'مع' expression ['كـ' IDENTIFIER] block
                 | 'with' expression ['as' IDENTIFIER] block

context_manager_class ::= class_with_enter_exit_methods
```

### الشرح بالعربية
مديرو السياق (Context Managers) يوفرون طريقة نظيفة لإدارة الموارد (مثل الملفات، الاتصالات) مع ضمان تنظيفها حتى لو حدث خطأ.

### Examples

```s
// استخدام مع الملفات / Use with files
مع افتح("بيانات.نص"، "قراءة") كـ ملف
    نص محتوى = ملف.اقرأ()
    اطبع(محتوى)
نهاية
// الملف يُغلق تلقائياً / File closes automatically

// صنف مدير سياق مخصص / Custom context manager class
صنف مؤقت
    خاص نص اسم
    
    باني(نص اسم)
        هذا.اسم = اسم
    نهاية
    
    // يُستدعى عند دخول السياق / Called on entering context
    عام دالة __دخول__()
        اطبع(ن"دخول: {هذا.اسم}")
        إرجاع هذا
    نهاية
    
    // يُستدعى عند خروج السياق / Called on exiting context
    عام دالة __خروج__(نوع_خطأ، قيمة_خطأ، تتبع)
        اطبع(ن"خروج: {هذا.اسم}")
        إرجاع خطأ  // لا تكبت الاستثناءات / Don't suppress exceptions
    نهاية
نهاية

// استخدام / Usage
مع مؤقت("عملية 1") كـ م
    اطبع("تنفيذ العملية")
نهاية
// Output:
// دخول: عملية 1
// تنفيذ العملية
// خروج: عملية 1

// مدير سياق لقياس الوقت / Timing context manager
صنف مقياس_وقت
    خاص عشري وقت_بداية
    خاص نص وصف
    
    باني(نص وصف)
        هذا.وصف = وصف
    نهاية
    
    عام دالة __دخول__()
        هذا.وقت_بداية = الوقت.الآن()
        إرجاع هذا
    نهاية
    
    عام دالة __خروج__(نوع_خطأ، قيمة_خطأ، تتبع)
        عشري مدة = الوقت.الآن() - هذا.وقت_بداية
        اطبع(ن"{هذا.وصف} استغرق {مدة} ثانية")
        إرجاع خطأ
    نهاية
نهاية

مع مقياس_وقت("حلقة كبيرة")
    رقم مجموع = 0
    لكل ع في نطاق(1000000)
        مجموع = مجموع + ع
    نهاية
    اطبع(ن"المجموع: {مجموع}")
نهاية
```

### Implementation Notes

```cpp
// include/parser/ast/context_manager_nodes.h
class WithStmt : public Stmt {
public:
    std::unique_ptr<Expr> contextExpr;     // Expression that provides context manager
    std::string targetVar;                  // Optional: variable to bind result of __enter__
    std::unique_ptr<BlockStmt> body;
    
    WithStmt(std::unique_ptr<Expr> ctx,
             std::string target,
             std::unique_ptr<BlockStmt> body)
        : contextExpr(std::move(ctx)),
          targetVar(std::move(target)),
          body(std::move(body)) {}
    
    void accept(Visitor& visitor) override;
};

// src/interpreter/statements/with_interpreter.cpp
void InterpreterCore::visitWithStmt(WithStmt* stmt) {
    // Evaluate context expression
    stmt->contextExpr->accept(*this);
    Value contextManager = lastValue;
    
    if (!contextManager.isObject()) {
        throw RuntimeError("'with' requires a context manager object");
    }
    
    ObjectInstance* obj = contextManager.asObject();
    
    // Call __enter__ method
    Value enterResult = obj->callMethod("__دخول__", {}, this);
    
    // Create new scope for 'with' block
    auto withEnv = new Environment(currentEnvironment);
    auto prevEnv = currentEnvironment;
    currentEnvironment = withEnv;
    
    // Bind target variable if specified
    if (!stmt->targetVar.empty()) {
        currentEnvironment->define(stmt->targetVar, enterResult);
    }
    
    // Execute body with exception handling
    bool exceptionOccurred = false;
    Value exceptionValue;
    
    try {
        stmt->body->accept(*this);
        
    } catch (const RuntimeError& e) {
        exceptionOccurred = true;
        // Prepare exception info for __exit__
        // ...
    }
    
    // Always call __exit__ (even if exception occurred)
    currentEnvironment = prevEnv;
    
    std::vector<Value> exitArgs = {
        Value::makeNull(),  // exception type
        Value::makeNull(),  // exception value
        Value::makeNull()   // traceback
    };
    
    if (exceptionOccurred) {
        // Pass exception info to __exit__
        // exitArgs[0] = exception type
        // exitArgs[1] = exception value
        // ...
    }
    
    Value exitResult = obj->callMethod("__خروج__", exitArgs, this);
    
    delete withEnv;
    
    // Re-raise exception if __exit__ didn't suppress it
    if (exceptionOccurred && !exitResult.asBool()) {
        throw;  // Re-throw the caught exception
    }
    
    DEBUG_PRINT("Completed 'with' statement for context manager");
}
```

---

## 6️⃣ Exceptions (الاستثناءات)

### Syntax (EBNF)
```ebnf
try_statement ::= 'جرب' block except_clauses [else_clause] [finally_clause]
                | 'try' block except_clauses ['else' block] ['finally' block]

except_clauses ::= except_clause+

except_clause ::= 'اعترض' [exception_type ['كـ' IDENTIFIER]] block
                | 'except' [exception_type ['as' IDENTIFIER]] block

else_clause ::= 'وإلا' block

finally_clause ::= 'أخيراً' block
                  | 'finally' block

raise_statement ::= 'ارفع' [expression]
                  | 'raise' [expression]
```

### الشرح بالعربية
نظام الاستثناءات يوفر طريقة منظمة للتعامل مع الأخطاء. يمكنك رفع استثناءات مخصصة واعتراضها والتعامل معها.

### Examples

```s
// مثال بسيط / Simple example
جرب
    رقم نتيجة = 10 / 0  // خطأ قسمة على صفر / Division by zero error
    اطبع(نتيجة)
اعترض خطأ_قسمة كـ خ
    اطبع("خطأ: {خ.رسالة}")
نهاية
// Output: خطأ: لا يمكن القسمة على صفر

// استثناءات متعددة / Multiple exceptions
جرب
    مصفوفة قائمة = [1، 2، 3]
    اطبع(قائمة[10])  // خطأ فهرس / Index error
اعترض خطأ_فهرس كـ خ
    اطبع("الفهرس خارج النطاق")
اعترض خطأ_نوع كـ خ
    اطبع("نوع خاطئ")
اعترض كـ خ
    اطبع("خطأ غير متوقع: {خ}")
نهاية

// مع else و finally
جرب
    رقم عدد = رقم(إدخال("أدخل عدداً: "))
    رقم نتيجة = 100 / عدد
اعترض خطأ_قيمة
    اطبع("يرجى إدخال عدد صحيح")
اعترض خطأ_قسمة
    اطبع("لا يمكن القسمة على صفر")
وإلا
    اطبع("النتيجة: {نتيجة}")
أخيراً
    اطبع("تنظيف الموارد")
نهاية

// رفع استثناء مخصص / Raising custom exception
دالة تحقق_عمر(رقم عمر)
    إذا (عمر < 0)
        ارفع خطأ_قيمة("العمر لا يمكن أن يكون سالباً")
    نهاية
    
    إذا (عمر > 150)
        ارفع خطأ_قيمة("العمر غير معقول")
    نهاية
    
    إرجاع صحيح
نهاية

جرب
    تحقق_عمر(-5)
اعترض خطأ_قيمة كـ خ
    اطبع(ن"خطأ في التحقق: {خ.رسالة}")
نهاية

// صنف استثناء مخصص / Custom exception class
صنف خطأ_مخصص يرث خطأ
    عام نص كود_خطأ
    
    باني(نص رسالة، نص كود)
        الأساس(رسالة)
        هذا.كود_خطأ = كود
    نهاية
نهاية

دالة معالج_بيانات(قاموس بيانات)
    إذا (!"اسم" في بيانات)
        ارفع خطأ_مخصص("حقل 'اسم' مطلوب"، "ERR_MISSING_FIELD")
    نهاية
نهاية

جرب
    معالج_بيانات({})
اعترض خطأ_مخصص كـ خ
    اطبع(ن"خطأ [{خ.كود_خطأ}]: {خ.رسالة}")
نهاية
// Output: خطأ [ERR_MISSING_FIELD]: حقل 'اسم' مطلوب
```

### Implementation Notes

```cpp
// include/errors/exceptions.h
class SadException : public std::exception {
protected:
    std::string messageAr;
    std::string messageEn;
    std::string code;
    int line;
    std::string file;
    
public:
    SadException(const std::string& msgAr, 
                 const std::string& msgEn = "",
                 const std::string& code = "ERR_RUNTIME",
                 int line = -1,
                 const std::string& file = "")
        : messageAr(msgAr),
          messageEn(msgEn),
          code(code),
          line(line),
          file(file) {}
    
    virtual const char* what() const noexcept override {
        return messageAr.c_str();
    }
    
    const std::string& getMessageAr() const { return messageAr; }
    const std::string& getMessageEn() const { return messageEn; }
    const std::string& getCode() const { return code; }
    int getLine() const { return line; }
    const std::string& getFile() const { return file; }
};

// Specific exception types
class DivisionByZeroError : public SadException {
public:
    DivisionByZeroError(int line = -1, const std::string& file = "")
        : SadException("لا يمكن القسمة على صفر", 
                      "Cannot divide by zero",
                      "ERR_DIV_ZERO",
                      line, file) {}
};

class IndexError : public SadException {
public:
    IndexError(const std::string& details, int line = -1)
        : SadException("خطأ في الفهرس: " + details,
                      "Index error: " + details,
                      "ERR_INDEX",
                      line) {}
};

class TypeError : public SadException {
public:
    TypeError(const std::string& expected, 
              const std::string& got,
              int line = -1)
        : SadException(
            "نوع خاطئ: متوقع " + expected + " لكن حصلنا على " + got,
            "Type error: expected " + expected + " but got " + got,
            "ERR_TYPE",
            line) {}
};

class ValueError : public SadException {
public:
    ValueError(const std::string& details, int line = -1)
        : SadException("خطأ في القيمة: " + details,
                      "Value error: " + details,
                      "ERR_VALUE",
                      line) {}
};

// include/parser/ast/exception_nodes.h
class TryStmt : public Stmt {
public:
    std::unique_ptr<BlockStmt> tryBlock;
    std::vector<std::unique_ptr<ExceptClause>> exceptClauses;
    std::unique_ptr<BlockStmt> elseBlock;      // Optional
    std::unique_ptr<BlockStmt> finallyBlock;   // Optional
    
    void accept(Visitor& visitor) override;
};

class ExceptClause {
public:
    std::string exceptionType;      // Empty means catch all
    std::string varName;            // Variable to bind exception to
    std::unique_ptr<BlockStmt> body;
};

class RaiseStmt : public Stmt {
public:
    std::unique_ptr<Expr> exception;  // Can be null for re-raise
    
    void accept(Visitor& visitor) override;
};

// src/interpreter/statements/exception_interpreter.cpp
void InterpreterCore::visitTryStmt(TryStmt* stmt) {
    bool exceptionCaught = false;
    SadException* caughtException = nullptr;
    
    try {
        // Execute try block
        stmt->tryBlock->accept(*this);
        
        // If no exception, execute else block if present
        if (stmt->elseBlock && !exceptionCaught) {
            stmt->elseBlock->accept(*this);
        }
        
    } catch (SadException& e) {
        // Try to match with except clauses
        for (auto& exceptClause : stmt->exceptClauses) {
            if (exceptClause->exceptionType.empty() ||
                matchesExceptionType(e, exceptClause->exceptionType)) {
                
                exceptionCaught = true;
                
                // Create new scope for except block
                auto exceptEnv = new Environment(currentEnvironment);
                auto prevEnv = currentEnvironment;
                currentEnvironment = exceptEnv;
                
                // Bind exception to variable if specified
                if (!exceptClause->varName.empty()) {
                    currentEnvironment->define(
                        exceptClause->varName,
                        Value::makeException(e)
                    );
                }
                
                // Execute except block
                exceptClause->body->accept(*this);
                
                currentEnvironment = prevEnv;
                delete exceptEnv;
                
                break;  // Stop after first match
            }
        }
        
        // If not caught, save for re-throwing after finally
        if (!exceptionCaught) {
            caughtException = &e;
        }
    }
    
    // Always execute finally block
    if (stmt->finallyBlock) {
        stmt->finallyBlock->accept(*this);
    }
    
    // Re-throw if exception wasn't caught
    if (caughtException) {
        throw *caughtException;
    }
    
    DEBUG_PRINT("Completed try-except statement, caught: %d", exceptionCaught);
}

void InterpreterCore::visitRaiseStmt(RaiseStmt* stmt) {
    if (!stmt->exception) {
        // Re-raise current exception
        if (currentException) {
            throw *currentException;
        } else {
            throw RuntimeError("No active exception to re-raise");
        }
    }
    
    // Evaluate exception expression
    stmt->exception->accept(*this);
    Value exceptionValue = lastValue;
    
    if (exceptionValue.isObject()) {
        // Custom exception object
        ObjectInstance* obj = exceptionValue.asObject();
        std::string message = obj->getField("رسالة").asString();
        throw SadException(message);
    } else if (exceptionValue.isString()) {
        // Simple string message
        throw SadException(exceptionValue.asString());
    } else {
        throw TypeError("Exception must be string or exception object", -1);
    }
}
```

### Tests
**File:** `tests/spec_rules/errors_examples.s`
```s
// اختبار اعتراض أخطاء / Test exception handling
جرب
    رقم س = 10 / 0
اعترض خطأ_قسمة
    اطبع("تم اعتراض خطأ القسمة")
نهاية
// Expected: "تم اعتراض خطأ القسمة"

// اختبار رفع استثناء / Test raising exception
دالة تحقق(رقم ع)
    إذا ع < 0
        ارفع خطأ_قيمة("العدد سالب")
    نهاية
نهاية

جرب
    تحقق(-5)
اعترض خطأ_قيمة كـ خ
    اطبع("تم اعتراض الخطأ")
نهاية
// Expected: "تم اعتراض الخطأ"
```

---

## 📝 ملخص التنفيذ / Implementation Summary

### Files to Create

1. **Comprehensions:**
   - `include/parser/ast/comprehension_nodes.h`
   - `src/parser/parser_comprehensions.cpp`
   - `src/interpreter/expressions/comprehension_evaluator.cpp`

2. **Generators:**
   - `include/interpreter/core/generator.h`
   - `src/interpreter/core/generator_runtime.cpp`
   - `src/interpreter/statements/yield_interpreter.cpp`

3. **Decorators:**
   - `include/parser/ast/decorator_nodes.h`
   - `src/parser/parser_decorators.cpp`
   - `src/interpreter/statements/decorator_interpreter.cpp`

4. **Context Managers:**
   - `include/parser/ast/context_manager_nodes.h`
   - `src/parser/parser_with.cpp`
   - `src/interpreter/statements/with_interpreter.cpp`

5. **Exceptions:**
   - `include/errors/exceptions.h`
   - `include/parser/ast/exception_nodes.h`
   - `src/parser/parser_exceptions.cpp`
   - `src/interpreter/statements/exception_interpreter.cpp`

### Token Types to Add

```cpp
// include/lexer/token.h
enum class TokenType {
    // ... existing tokens
    
    // Comprehensions & Generators
    KEYWORD_FOR,        // لكل
    KEYWORD_FOR_EN,     // for
    KEYWORD_IN,         // في
    KEYWORD_IN_EN,      // in
    KEYWORD_YIELD,      // أنتج
    KEYWORD_YIELD_EN,   // yield
    
    // Decorators
    AT,                 // @
    
    // Context Managers
    KEYWORD_WITH,       // مع
    KEYWORD_WITH_EN,    // with
    KEYWORD_AS,         // كـ
    KEYWORD_AS_EN,      // as
    
    // Exceptions
    KEYWORD_TRY,        // جرب
    KEYWORD_TRY_EN,     // try
    KEYWORD_EXCEPT,     // اعترض
    KEYWORD_EXCEPT_EN,  // except
    KEYWORD_RAISE,      // ارفع
    KEYWORD_RAISE_EN,   // raise
    KEYWORD_FINALLY,    // أخيراً
    KEYWORD_FINALLY_EN, // finally
};
```

### Keywords to Add

```cpp
// src/lexer/lexer_keywords.cpp
void LexerKeywords::initializeKeywords() {
    keywords["لكل"] = TokenType::KEYWORD_FOR;
    keywords["for"] = TokenType::KEYWORD_FOR_EN;
    keywords["في"] = TokenType::KEYWORD_IN;
    keywords["in"] = TokenType::KEYWORD_IN_EN;
    keywords["أنتج"] = TokenType::KEYWORD_YIELD;
    keywords["yield"] = TokenType::KEYWORD_YIELD_EN;
    keywords["مع"] = TokenType::KEYWORD_WITH;
    keywords["with"] = TokenType::KEYWORD_WITH_EN;
    keywords["كـ"] = TokenType::KEYWORD_AS;
    keywords["as"] = TokenType::KEYWORD_AS_EN;
    keywords["جرب"] = TokenType::KEYWORD_TRY;
    keywords["try"] = TokenType::KEYWORD_TRY_EN;
    keywords["اعترض"] = TokenType::KEYWORD_EXCEPT;
    keywords["except"] = TokenType::KEYWORD_EXCEPT_EN;
    keywords["ارفع"] = TokenType::KEYWORD_RAISE;
    keywords["raise"] = TokenType::KEYWORD_RAISE_EN;
    keywords["أخيراً"] = TokenType::KEYWORD_FINALLY;
    keywords["finally"] = TokenType::KEYWORD_FINALLY_EN;
}
```

---

## 🔧 Git Workflow

```powershell
# إنشاء فرع / Create branch
git checkout -b feat/spec/python-features

# إضافة الملفات / Add files
git add docs/language_spec/rules/05_python_features.md
git add tests/spec_rules/comprehensions_list.s
git add tests/spec_rules/comprehensions_dict.s
git add tests/spec_rules/generators.s
git add tests/spec_rules/decorators.s
git add tests/spec_rules/errors_examples.s

# Commit
git commit -m "[spec/python] Add Python features specification with optional parentheses

- List/Dict comprehensions with optional parentheses in if clause
- Support for both 'إذا شرط' and 'إذا (شرط)' syntax
- Generators with yield
- Decorators (function wrappers)
- Context managers (with statement)
- Exception handling (try/except/finally)
- All features with AR/EN dual syntax
- Comprehensive test files for each feature
- Updated parser to handle optional parentheses in conditions"

# Push
git push --set-upstream origin feat/spec/python-features
```

---

**آخر تحديث:** 10 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 48+  
**تحديث:** دعم الأقواس الاختيارية في شروط التضمينات
