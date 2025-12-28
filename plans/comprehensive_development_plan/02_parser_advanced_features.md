# 📝 المرحلة 2: المحلل النحوي المتقدم
## Phase 2: Advanced Parser Features

**المدة:** أسبوعان (10 أيام عمل)  
**الأولوية:** 🔴 P0 - حرجة  
**التبعيات:** المرحلة 1 (Lexer متقدم)  
**التأثير:** يفتح Python/modern features

---

## 🎯 الأهداف

### الهدف الرئيسي
تطبيق جميع ميزات Parser المتقدمة من `rules/rules/05_python_features.md` و `04_syntax.md` والتي غير مطبقة حالياً.

### الأهداف الفرعية
1. ✅ List/Dict/Set Comprehensions
2. ✅ Pattern Matching (حالة/match)
3. ✅ Generator Expressions
4. ✅ Async/Await syntax
5. ✅ Decorator syntax
6. ✅ Walrus operator (:=)
7. ✅ Enhanced for loops
8. ✅ Structural unpacking

---

## 📊 التقييم الحالي

### ما موجود في Parser

```cpp
✅ Basic expressions (binary, unary, primary)
✅ Statements (if, while, for, return)
✅ Function declarations
✅ Class declarations (basic)
✅ Variable declarations
✅ Array/Map literals
```

### ما ناقص (من القواعد)

```cpp
✅ List comprehensions: [x*2 for x in list] - مكتمل
✅ Dict comprehensions: {k:v for k,v in items} - مكتمل
✅ Set comprehensions: {x for x in list} - مكتمل 🎉
✅ Walrus operator: if (x := f()) > 0 - مكتمل
❌ Generator expressions: (x*2 for x in list)
❌ Pattern matching: match x { case 1: ... }
❌ Decorators: @cache function f() {...}
❌ Async functions: async function f() {...}
❌ Await expressions: await f()
❌ Unpacking: a, b = [1, 2]
❌ Enhanced try-catch with multiple catches
```

---

## 📋 المهام التفصيلية

### المهمة 2.1: List Comprehensions (يومان)

**الصيغة المطلوبة (من القواعد):**

```s
// Basic comprehension
[x * 2 for x in numbers]
[س * 2 لكل س في أعداد]

// With condition
[x for x in numbers if x > 5]
[س لكل س في أعداد إذا س > 5]

// With parentheses in condition
[x for x in numbers if (x > 3 && x < 8)]

// Nested
[[x, y] for x in [1,2] for y in [3,4]]
```

**التنفيذ في Parser:**

```cpp
// include/parser/ast/comprehension_nodes.h
namespace AST {

class ListComprehension : public Expr {
public:
    std::unique_ptr<Expr> expression;      // The expression: x * 2
    std::string variable;                   // Loop variable: x
    std::unique_ptr<Expr> iterable;        // Source: numbers
    std::unique_ptr<Expr> condition;       // Optional filter: x > 5
    
    // For nested comprehensions
    std::vector<ComprehensionClause> clauses;
    
    ListComprehension(std::unique_ptr<Expr> expr,
                     std::string var,
                     std::unique_ptr<Expr> iter,
                     std::unique_ptr<Expr> cond = nullptr)
        : expression(std::move(expr)),
          variable(std::move(var)),
          iterable(std::move(iter)),
          condition(std::move(cond)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};

struct ComprehensionClause {
    std::string variable;
    std::unique_ptr<Expr> iterable;
    std::unique_ptr<Expr> condition;  // optional
};

} // namespace AST

// src/parser/parser_comprehensions.cpp
std::unique_ptr<AST::Expr> ParserCore::parseListComprehension() {
    // Already consumed '['
    
    // Parse expression
    auto expr = parseExpression();
    
    // Expect 'for' or 'لكل'
    if (!match(TT::KEYWORD_FOR)) {
        error("Expected 'for' or 'لكل' in comprehension");
    }
    
    // Parse variable
    if (!check(TT::IDENTIFIER)) {
        error("Expected variable name in comprehension");
    }
    std::string var = previous().value;
    
    // Expect 'in' or 'في'
    if (!match(TT::KEYWORD_IN)) {
        error("Expected 'in' or 'في' in comprehension");
    }
    
    // Parse iterable
    auto iterable = parseExpression();
    
    // Optional condition
    std::unique_ptr<AST::Expr> condition = nullptr;
    if (match(TT::KEYWORD_IF)) {
        // Optional parentheses
        bool has_parens = match(TT::LEFT_PAREN);
        
        condition = parseExpression();
        
        if (has_parens && !match(TT::RIGHT_PAREN)) {
            error("Expected ')' after condition");
        }
    }
    
    // Check for nested comprehensions
    std::vector<AST::ComprehensionClause> clauses;
    while (match(TT::KEYWORD_FOR)) {
        std::string nested_var = consume(TT::IDENTIFIER, 
            "Expected variable name").value;
        
        consume(TT::KEYWORD_IN, "Expected 'in' or 'في'");
        
        auto nested_iter = parseExpression();
        
        std::unique_ptr<AST::Expr> nested_cond = nullptr;
        if (match(TT::KEYWORD_IF)) {
            bool has_parens = match(TT::LEFT_PAREN);
            nested_cond = parseExpression();
            if (has_parens) match(TT::RIGHT_PAREN);
        }
        
        clauses.push_back({
            nested_var,
            std::move(nested_iter),
            std::move(nested_cond)
        });
    }
    
    // Expect ']'
    consume(TT::RIGHT_BRACKET, "Expected ']' after comprehension");
    
    auto comp = std::make_unique<AST::ListComprehension>(
        std::move(expr),
        var,
        std::move(iterable),
        std::move(condition)
    );
    
    comp->clauses = std::move(clauses);
    
    return comp;
}

// في parseArray() - تحديد comprehension vs array literal
std::unique_ptr<AST::Expr> ParserCore::parseArray() {
    consume(TT::LEFT_BRACKET, "Expected '['");
    
    // Empty array
    if (check(TT::RIGHT_BRACKET)) {
        advance();
        return std::make_unique<AST::ArrayLiteral>();
    }
    
    // Look ahead to determine if it's a comprehension
    size_t saved_pos = current_;
    auto expr = parseExpression();
    
    // Check if next is 'for' or 'لكل' - it's a comprehension
    if (check(TT::KEYWORD_FOR)) {
        current_ = saved_pos;  // Backtrack
        return parseListComprehension();
    }
    
    // Otherwise, it's an array literal
    current_ = saved_pos;
    return parseArrayLiteral();
}
```

**الاختبارات:**

```cpp
TEST(ParserComprehensions, BasicList) {
    Parser parser("[x * 2 for x in [1,2,3]]");
    auto ast = parser.parse();
    
    ASSERT_TRUE(ast != nullptr);
    auto comp = dynamic_cast<AST::ListComprehension*>(ast.get());
    ASSERT_TRUE(comp != nullptr);
    EXPECT_EQ(comp->variable, "x");
}

TEST(ParserComprehensions, WithCondition) {
    Parser parser("[x for x in numbers if x > 5]");
    auto ast = parser.parse();
    
    auto comp = dynamic_cast<AST::ListComprehension*>(ast.get());
    ASSERT_TRUE(comp->condition != nullptr);
}

TEST(ParserComprehensions, WithParenthesesInCondition) {
    Parser parser("[x for x in nums if (x > 3 && x < 8)]");
    auto ast = parser.parse();
    
    auto comp = dynamic_cast<AST::ListComprehension*>(ast.get());
    ASSERT_TRUE(comp->condition != nullptr);
}

TEST(ParserComprehensions, Nested) {
    Parser parser("[[x,y] for x in [1,2] for y in [3,4]]");
    auto ast = parser.parse();
    
    auto comp = dynamic_cast<AST::ListComprehension*>(ast.get());
    EXPECT_EQ(comp->clauses.size(), 1);
}
```

---

### المهمة 2.2: Dict & Set Comprehensions (يوم)

```cpp
// Dict comprehension
{k: v*2 for k, v in items}
{مفتاح: قيمة*2 لكل مفتاح، قيمة في عناصر}

// Set comprehension
{x for x in numbers if x > 0}
{س لكل س في أعداد إذا س > 0}

// التنفيذ
class DictComprehension : public Expr {
public:
    std::unique_ptr<Expr> key_expr;
    std::unique_ptr<Expr> value_expr;
    std::string key_var;
    std::string value_var;
    std::unique_ptr<Expr> iterable;
    std::unique_ptr<Expr> condition;
};

class SetComprehension : public Expr {
public:
    std::unique_ptr<Expr> expression;
    std::string variable;
    std::unique_ptr<Expr> iterable;
    std::unique_ptr<Expr> condition;
};

std::unique_ptr<AST::Expr> ParserCore::parseMapOrSetComprehension() {
    // Already consumed '{'
    
    auto expr = parseExpression();
    
    // Check if it's dict (has ':') or set
    if (match(TT::COLON)) {
        // Dict comprehension
        auto value_expr = parseExpression();
        
        consume(TT::KEYWORD_FOR, "Expected 'for' in dict comprehension");
        
        // Parse key, value variables
        std::string key_var = consume(TT::IDENTIFIER, "Expected key variable").value;
        consume(TT::COMMA, "Expected ',' between key and value");
        std::string val_var = consume(TT::IDENTIFIER, "Expected value variable").value;
        
        consume(TT::KEYWORD_IN, "Expected 'in'");
        auto iterable = parseExpression();
        
        std::unique_ptr<AST::Expr> condition = nullptr;
        if (match(TT::KEYWORD_IF)) {
            bool has_parens = match(TT::LEFT_PAREN);
            condition = parseExpression();
            if (has_parens) match(TT::RIGHT_PAREN);
        }
        
        consume(TT::RIGHT_BRACE, "Expected '}'");
        
        return std::make_unique<AST::DictComprehension>(
            std::move(expr), std::move(value_expr),
            key_var, val_var,
            std::move(iterable), std::move(condition)
        );
    }
    else if (check(TT::KEYWORD_FOR)) {
        // Set comprehension
        advance();
        
        std::string var = consume(TT::IDENTIFIER, "Expected variable").value;
        consume(TT::KEYWORD_IN, "Expected 'in'");
        auto iterable = parseExpression();
        
        std::unique_ptr<AST::Expr> condition = nullptr;
        if (match(TT::KEYWORD_IF)) {
            bool has_parens = match(TT::LEFT_PAREN);
            condition = parseExpression();
            if (has_parens) match(TT::RIGHT_PAREN);
        }
        
        consume(TT::RIGHT_BRACE, "Expected '}'");
        
        return std::make_unique<AST::SetComprehension>(
            std::move(expr), var,
            std::move(iterable), std::move(condition)
        );
    }
    
    // Regular map/set literal
    // ...
}
```

---

### المهمة 2.3: Pattern Matching (3 أيام)

**الصيغة المطلوبة (من القواعد):**

```s
// Basic match
match value 
    case 1: print("one")
    case 2: print("two")
    case _: print("other")
نهاية

// Arabic
طابق القيمة 
    حالة 1: اطبع("واحد")
    حالة 2: اطبع("اثنان")
    حالة _: اطبع("غير ذلك")
نهاية

// With patterns
match point 
    case [0, 0]: print("origin")
    case [x, 0]: print("on x-axis")
    case [0, y]: print("on y-axis")
    case [x, y]: print("point")
نهاية

// With guards
match x 
    case n  (if n > 0 | if (n > 0)): print("positive") 
    case n (if n < 0 | if (n < 0)): print("negative")
    case _: print("zero")
نهاية
```

**التنفيذ:**

```cpp
// include/parser/ast/match_nodes.h
namespace AST {

enum class PatternType {
    LITERAL,      // 1, "hello", true
    WILDCARD,     // _
    VARIABLE,     // x
    ARRAY,        // [x, y, z]
    MAP,          // {key: value}
    CONSTRUCTOR   // Point(x, y)
};

class Pattern {
public:
    PatternType type;
    std::string identifier;  // For VARIABLE
    std::unique_ptr<Expr> literal;  // For LITERAL
    std::vector<std::unique_ptr<Pattern>> sub_patterns;  // For ARRAY/MAP
    
    Pattern(PatternType t) : type(t) {}
};

class CaseClause {
public:
    std::unique_ptr<Pattern> pattern;
    std::unique_ptr<Expr> guard;  // Optional guard (if condition)
    std::vector<std::unique_ptr<Stmt>> body;
    
    CaseClause(std::unique_ptr<Pattern> p,
               std::vector<std::unique_ptr<Stmt>> b,
               std::unique_ptr<Expr> g = nullptr)
        : pattern(std::move(p)),
          body(std::move(b)),
          guard(std::move(g)) {}
};

class MatchStmt : public Stmt {
public:
    std::unique_ptr<Expr> value;
    std::vector<CaseClause> cases;
    
    MatchStmt(std::unique_ptr<Expr> v,
              std::vector<CaseClause> c)
        : value(std::move(v)),
          cases(std::move(c)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};

} // namespace AST

// src/parser/parser_match.cpp
std::unique_ptr<AST::Stmt> ParserCore::parseMatchStatement() {
    // Consume 'match' or 'طابق'
    consume(TT::KEYWORD_MATCH, "Expected 'match' or 'طابق'");
    
    // Parse expression to match
    auto value = parseExpression();
    
    // Expect '{'
    consume(TT::LEFT_BRACE, "Expected '{' after match value");
    
    // Parse case clauses
    std::vector<AST::CaseClause> cases;
    
    while (!check(TT::RIGHT_BRACE) && !isAtEnd()) {
        // Expect 'case' or 'حالة'
        consume(TT::KEYWORD_CASE, "Expected 'case' or 'حالة'");
        
        // Parse pattern
        auto pattern = parsePattern();
        
        // Optional guard
        std::unique_ptr<AST::Expr> guard = nullptr;
        if (match(TT::KEYWORD_IF)) {
            guard = parseExpression();
        }
        
        // Expect ':'
        consume(TT::COLON, "Expected ':' after case pattern");
        
        // Parse body
        std::vector<std::unique_ptr<AST::Stmt>> body;
        
        // Single statement or block
        if (check(TT::LEFT_BRACE)) {
            advance();
            while (!check(TT::RIGHT_BRACE) && !isAtEnd()) {
                body.push_back(parseStatement());
            }
            consume(TT::RIGHT_BRACE, "Expected '}' after case body");
        } else {
            body.push_back(parseStatement());
        }
        
        cases.emplace_back(
            std::move(pattern),
            std::move(body),
            std::move(guard)
        );
    }
    
    consume(TT::RIGHT_BRACE, "Expected '}' after match cases");
    
    return std::make_unique<AST::MatchStmt>(
        std::move(value),
        std::move(cases)
    );
}

std::unique_ptr<AST::Pattern> ParserCore::parsePattern() {
    // Wildcard: _
    if (match(TT::UNDERSCORE)) {
        return std::make_unique<AST::Pattern>(AST::PatternType::WILDCARD);
    }
    
    // Array pattern: [x, y, z]
    if (check(TT::LEFT_BRACKET)) {
        return parseArrayPattern();
    }
    
    // Map pattern: {key: value}
    if (check(TT::LEFT_BRACE)) {
        return parseMapPattern();
    }
    
    // Literal or variable
    if (check(TT::INTEGER_LITERAL) || check(TT::STRING_LITERAL) || 
        check(TT::BOOLEAN_LITERAL)) {
        auto pattern = std::make_unique<AST::Pattern>(AST::PatternType::LITERAL);
        pattern->literal = parsePrimary();
        return pattern;
    }
    
    // Variable pattern
    if (check(TT::IDENTIFIER)) {
        auto pattern = std::make_unique<AST::Pattern>(AST::PatternType::VARIABLE);
        pattern->identifier = advance().value;
        return pattern;
    }
    
    error("Expected pattern");
    return nullptr;
}

std::unique_ptr<AST::Pattern> ParserCore::parseArrayPattern() {
    consume(TT::LEFT_BRACKET, "Expected '['");
    
    auto pattern = std::make_unique<AST::Pattern>(AST::PatternType::ARRAY);
    
    if (!check(TT::RIGHT_BRACKET)) {
        do {
            pattern->sub_patterns.push_back(parsePattern());
        } while (match(TT::COMMA));
    }
    
    consume(TT::RIGHT_BRACKET, "Expected ']'");
    
    return pattern;
}
```

---

### المهمة 2.4: Generator Expressions (يوم)

```cpp
// Syntax: (x*2 for x in numbers)
// Similar to list comprehension but with ()

class GeneratorExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;
    std::string variable;
    std::unique_ptr<Expr> iterable;
    std::unique_ptr<Expr> condition;
    
    // Generator state (for interpreter)
    bool is_exhausted = false;
};

std::unique_ptr<AST::Expr> ParserCore::parseGeneratorOrTuple() {
    consume(TT::LEFT_PAREN, "Expected '('");
    
    if (check(TT::RIGHT_PAREN)) {
        advance();
        return std::make_unique<AST::TupleLiteral>();
    }
    
    // Look ahead
    size_t saved_pos = current_;
    auto expr = parseExpression();
    
    // Generator: (expr for ...)
    if (check(TT::KEYWORD_FOR)) {
        current_ = saved_pos;
        return parseGeneratorExpression();
    }
    
    // Tuple: (expr, ...)
    current_ = saved_pos;
    return parseTupleLiteral();
}
```

---

### المهمة 2.5: Async/Await (يومان)

```cpp
// Async function
async function fetchData() {
    var data = await httpGet("url")
    return data
}

// Arabic
غير_متزامن دالة جلب_البيانات() {
    var بيانات = انتظر httpGet("url")
    إرجاع بيانات
}

// AST Nodes
class AsyncFunctionDecl : public FunctionDecl {
public:
    bool is_async = true;
};

class AwaitExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;
    
    AwaitExpr(std::unique_ptr<Expr> e)
        : expression(std::move(e)) {}
};

// Parser
std::unique_ptr<AST::Stmt> ParserCore::parseFunctionDeclaration() {
    bool is_async = false;
    
    // Check for 'async' or 'غير_متزامن'
    if (match(TT::KEYWORD_ASYNC)) {
        is_async = true;
    }
    
    consume(TT::KEYWORD_FUNCTION, "Expected 'function'");
    
    // ... rest of function parsing
    
    if (is_async) {
        auto async_func = std::make_unique<AST::AsyncFunctionDecl>();
        // Copy data from regular function
        async_func->is_async = true;
        return async_func;
    }
    
    return func;
}

std::unique_ptr<AST::Expr> ParserCore::parseUnaryExpression() {
    // await expression
    if (match(TT::KEYWORD_AWAIT)) {
        auto expr = parseUnaryExpression();
        return std::make_unique<AST::AwaitExpr>(std::move(expr));
    }
    
    // ... other unary operators
}
```

---

### المهمة 2.6: Decorators (يوم)

```cpp
// Syntax
@cache
@log
function expensive() {
    // ...
}

// Arabic
@ذاكرة_مؤقتة
@سجل
دالة مكلفة() 
    // ...
نهاية

// AST
class Decorator {
public:
    std::string name;
    std::vector<std::unique_ptr<Expr>> arguments;
};

class FunctionDecl : public Stmt {
public:
    // ... existing members
    std::vector<Decorator> decorators;
};

// Parser
std::unique_ptr<AST::Stmt> ParserCore::parseFunctionDeclaration() {
    // Parse decorators
    std::vector<AST::Decorator> decorators;
    
    while (match(TT::AT)) {
        std::string name = consume(TT::IDENTIFIER, 
            "Expected decorator name").value;
        
        std::vector<std::unique_ptr<AST::Expr>> args;
        
        // Optional arguments
        if (match(TT::LEFT_PAREN)) {
            if (!check(TT::RIGHT_PAREN)) {
                do {
                    args.push_back(parseExpression());
                } while (match(TT::COMMA));
            }
            consume(TT::RIGHT_PAREN, "Expected ')'");
        }
        
        decorators.push_back({name, std::move(args)});
    }
    
    // Parse function
    consume(TT::KEYWORD_FUNCTION, "Expected 'function'");
    
    auto func = parseFunctionBody();
    func->decorators = std::move(decorators);
    
    return func;
}
```

---

### المهمة 2.7: Walrus Operator (نصف يوم)

```cpp
// Syntax: if (x := f()) > 0
// Assignment expression

class AssignmentExpr : public Expr {
public:
    std::string variable;
    std::unique_ptr<Expr> value;
};

std::unique_ptr<AST::Expr> ParserCore::parseExpression() {
    auto expr = parseLogicalOr();
    
    // Check for walrus operator :=
    if (match(TT::WALRUS)) {
        if (auto* var = dynamic_cast<AST::VariableExpr*>(expr.get())) {
            auto value = parseExpression();
            return std::make_unique<AST::AssignmentExpr>(
                var->name,
                std::move(value)
            );
        } else {
            error("Invalid target for assignment expression");
        }
    }
    
    return expr;
}
```

---

### المهمة 2.8: Structural Unpacking (يوم)

```cpp
// Tuple unpacking
var a, b = [1, 2]
متغير س، ص = [1، 2]

// Multiple assignment
var x, y, z = getTuple()

// In for loops
for key, value in items {
    // ...
}

// AST
class UnpackingStmt : public Stmt {
public:
    std::vector<std::string> variables;
    std::unique_ptr<Expr> value;
};

// Parser
std::unique_ptr<AST::Stmt> ParserCore::parseVarDeclaration() {
    consume(TT::KEYWORD_VAR, "Expected 'var'");
    
    // Check if it's unpacking
    std::vector<std::string> variables;
    variables.push_back(consume(TT::IDENTIFIER, "Expected variable").value);
    
    // Multiple variables?
    while (match(TT::COMMA)) {
        variables.push_back(consume(TT::IDENTIFIER, "Expected variable").value);
    }
    
    consume(TT::ASSIGN, "Expected '='");
    auto value = parseExpression();
    
    if (variables.size() > 1) {
        // Unpacking
        return std::make_unique<AST::UnpackingStmt>(
            std::move(variables),
            std::move(value)
        );
    } else {
        // Regular var declaration
        return std::make_unique<AST::VarDeclStmt>(
            variables[0],
            std::move(value)
        );
    }
}
```

---

## 📈 معايير النجاح

### التغطية - الحالة الحالية (19 ديسمبر 2025)
- ✅ **Walrus operator** 100% - مكتمل
- ✅ **Dict comprehensions** 100% - مكتمل
- ✅ **Set comprehensions** 100% - مكتمل 🎉
- ⏳ **Pattern matching** - التالي
- ⏳ **Async/await** - بعد Pattern Matching
- ❌ List comprehensions (nested) - موجود أساسي فقط
- ❌ Generators - ناقص
- ❌ Decorators - ناقص
- ❌ Unpacking - ناقص

### الاختبارات
- ✅ Set Comprehensions: 6/6 tests passed (100%)
- ✅ Dict Comprehensions: tested & working
- ✅ Walrus Operator: tested & working
- ⏳ Pattern matching tests - معلّق
- ⏳ Async/await tests - معلّق

### الأداء
- ✅ سرعة التحليل محفوظة
- ✅ حجم AST معقول
- ✅ فرض التفرد في Set: O(n²) - مقبول

---

## 📅 الجدول الزمني المحدّث

| الحالة | المهمة | الساعات الفعلية |
|------|---------|----------|
| ✅ مكتمل | Walrus operator + tests | 4 ساعات |
| ✅ مكتمل | Dict comprehensions + tests | 5 ساعات |
| ✅ مكتمل | Set comprehensions + tests | 6 ساعات |
| ⏳ التالي | Pattern matching + tests | ~15 ساعة |
| ⏳ معلّق | Async/await + tests | ~12 ساعة |

**التقدم الحالي:** 60% من المرحلة 2 (3/5 ميزات)  
**المتبقي:** Pattern Matching, Async/Await

---

## 📄 الوثائق المتوفرة

- ✅ `WALRUS_OPERATOR_COMPLETION.md` - تقرير Walrus Operator
- ✅ `DICT_COMPREHENSION_COMPLETION.md` - تقرير Dict Comprehensions
- ✅ `SET_COMPREHENSION_COMPLETION_REPORT.md` - تقرير Set Comprehensions
- ✅ `TODAYS_ACHIEVEMENT_DEC19.md` - ملخص إنجاز اليوم

---

**الحالة:** 🔄 قيد التنفيذ - 60%  
**آخر تحديث:** 19 ديسمبر 2025  
**الخطوة التالية:** تنفيذ Pattern Matching
