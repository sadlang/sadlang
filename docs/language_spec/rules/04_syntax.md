# 📐 قواعد النحو العام / General Syntax Rules

**التاريخ:** 5 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مرجع تفصيلي لقواعد النحو

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **قواعد النحو العامة الكاملة** للغة "ص". يشمل التعابير (Expressions)، الجمل (Statements)، هياكل التحكم (Control Flow)، العوامل (Operators)، جدول الأسبقية (Precedence)، والتعليقات (Comments).

**ملاحظات مهمة:**
- 🔹 **الكتل (Blocks):** تنتهي بكلمة `نهاية` بدلاً من `}`
- 🔹 **الشروط:** توضع بين أقواس `إذا (شرط)` و `بينما (شرط)`
- 🔹 **الفاصلة المنقوطة:** يمكن استخدام `؛` (العربية U+061B) أو `;` (الإنجليزية) - كلاهما مدعوم
- 🔹 **الفاصلة:** يمكن استخدام `،` (العربية U+060C) أو `,` (الإنجليزية) في قوائم الوسائط
- 🔹 **حلقة For:** تستخدم `لكل` للحلقات من النوعين (for-each و traditional for)
- 🔹 **التعليقات:** تستخدم `#` للسطر الواحد، `#* *#` للكتلة، `##` أو `#** **#` للتوثيق

### (EN) Description
This file documents the **complete general syntax rules** for Sad language, including expressions, statements, control flow structures, operators, precedence table, and comments.

**Important Notes:**
- 🔹 **Blocks:** End with `نهاية` keyword instead of `}`
- 🔹 **Conditions:** Enclosed in parentheses `إذا (condition)` and `بينما (condition)`
- 🔹 **Semicolon:** Both `؛` (Arabic U+061B) and `;` (English) are supported
- 🔹 **Comma:** Both `،` (Arabic U+060C) and `,` (English) are supported in argument lists
- 🔹 **For Loops:** Use `لكل` for both for-each and traditional for loops
- 🔹 **Comments:** Use `#` for line, `#* *#` for block, `##` or `#** **#` for documentation

---

## 📦 الاعتماديات / Dependencies

- ✅ `docs/language_spec/06_syntax_examples.md` - Syntax examples
- ✅ `docs/language_spec/01_types.md` - Type system
- ✅ `docs/language_spec/02_functions.md` - Functions
- ✅ `plans/imp/11_parser_and_ast_plan.md` - AST structure

---

## 📋 قائمة المحتويات / Contents

| # | القسم (AR) | Section (EN) | الأولوية |
|---|-----------|-----------|---------|
| 1 | التعابير | Expressions | P0 |
| 2 | الجمل | Statements | P0 |
| 3 | هياكل التحكم | Control Flow | P0 |
| 4 | العوامل | Operators | P0 |
| 5 | جدول الأسبقية | Precedence Table | P0 |
| 6 | التعليقات | Comments | P0 |
| 7 | الترميز والنص | Encoding & RTL | P0 |
| 8 | الكلمات المحجوزة | Reserved Keywords | P0 |

---

## 1️⃣ Expressions (التعابير)

### Syntax (EBNF)
```ebnf
expression ::= assignment

assignment ::= (call '.')? IDENTIFIER '=' assignment
             | logical_or

logical_or  ::= logical_and ('||' logical_and)*
logical_and ::= equality ('&&' equality)*
equality    ::= comparison (('==' | '!=') comparison)*
comparison  ::= term (('>' | '>=' | '<' | '<=') term)*
term        ::= factor (('+' | '-') factor)*
factor      ::= unary (('*' | '/' | '%') unary)*
unary       ::= ('!' | '-' | '+') unary
              | power

power       ::= postfix ('**' unary)*
postfix     ::= primary (call | index | member)*

call        ::= '(' [arguments] ')'
index       ::= '[' expression ']'
member      ::= '.' IDENTIFIER

arguments   ::= expression ((',' | '،') expression)*

primary     ::= literal
              | IDENTIFIER
              | '(' expression ')'
              | array_literal
              | map_literal
              | lambda

literal     ::= INTEGER | FLOAT | STRING | BOOLEAN | NONE
```

### Semantics
- **Evaluation Order:** Left to right, respecting precedence
- **Short-Circuit:** `&&` and `||` use short-circuit evaluation
- **Type Coercion:** Automatic in mixed arithmetic operations

### Examples

```s
// تعابير حسابية / Arithmetic expressions
رقم مجموع = 10 + 20        // 30
رقم فرق = 50 - 15          // 35
رقم حاصل_ضرب = 6 * 7      // 42
رقم حاصل_قسمة = 100 / 4    // 25
رقم باقي = 17 % 5          // 2
رقم قوة = 2 ** 8           // 256

// تعابير منطقية / Logical expressions
منطقي نعم = صحيح && صحيح     // true
منطقي لا = صحيح && خطأ       // false
منطقي أو = صحيح || خطأ       // true
منطقي ليس = !صحيح            // false

// تعابير مقارنة / Comparison expressions
منطقي أكبر = 10 > 5         // true
منطقي أصغر = 5 < 10         // true
منطقي يساوي = 7 == 7        // true
منطقي لا_يساوي = 5 != 3     // true
منطقي أكبر_أو_يساوي = 5 >= 5 // true

// تعابير معقدة / Complex expressions
رقم نتيجة = (10 + 5) * 2 - 3  // 27
منطقي معقد = (5 > 3) && (10 < 20) || خطأ  // true

// تعابير السلاسل / String expressions
نص مدمج = "مرحبا" + " " + "عالم"  // "مرحبا عالم"
نص مكرر = "abc" * 3              // "abcabcabc"
منطقي يحتوي = "abc" في "abcdef"  // true

// تعابير المصفوفات / Array expressions
مصفوفة قائمة = [1, 2, 3]
رقم عنصر = قائمة[0]              // 1
مصفوفة مدمجة = [1, 2] + [3, 4]   // [1, 2, 3, 4]

// تعابير الخريطة / Map expressions
خريطة بيانات = {"الاسم": "أحمد", "العمر": 25}
نص اسم = بيانات["الاسم"]        // "أحمد"

// استدعاء دالة / Function call (with Arabic comma)
رقم مربع = pow(5، 2)            // 25
رقم مكعب = pow(3, 3)            // 27 (English comma also works)

# وصول للعضو / Member access
نقطة ن = نقطة(3, 4)
عشري مسافة = ن.مسافة_من_الأصل()  # 5.0

# تعابير لامدا / Lambda expressions
أي ضعف = (رقم ع) => ع * 2
اطبع(ضعف(21))                  # 42

# عملية ثلاثية (Ternary) / Ternary operator
رقم قيمة_مطلقة = (ع >= 0) ? ع : -ع

# دعم الفاصلة العربية / Arabic comma support
مصفوفة نقاط = [نقطة(3، 4)، نقطة(5، 6)، نقطة(7، 8)]
رقم نتيجة = جمع(10، 20، 30)  # Using Arabic comma
رقم نتيجة2 = جمع(10, 20, 30)  # Using English comma (both work)

# تعابير bitwise
رقم and = 0b1100 & 0b1010      # 0b1000 = 8
رقم or = 0b1100 | 0b1010       # 0b1110 = 14
رقم xor = 0b1100 ^ 0b1010      # 0b0110 = 6
رقم shift = 0b0010 << 2        # 0b1000 = 8
```

### AST Representation
```
BinaryExpr
├── left: LiteralExpr(10)
├── operator: PLUS
└── right: LiteralExpr(20)

Result: 30
```

### Implementation Notes

```cpp
// include/parser/ast/expression_nodes.h
class Expr {
public:
    virtual ~Expr() = default;
    virtual void accept(Visitor& visitor) = 0;
    
    // Source location for error reporting
    SourceLocation location;
};

class BinaryExpr : public Expr {
public:
    std::unique_ptr<Expr> left;
    TokenType operatorType;
    std::unique_ptr<Expr> right;
    
    BinaryExpr(std::unique_ptr<Expr> left,
               TokenType op,
               std::unique_ptr<Expr> right)
        : left(std::move(left)),
          operatorType(op),
          right(std::move(right)) {}
    
    void accept(Visitor& visitor) override;
};

class UnaryExpr : public Expr {
public:
    TokenType operatorType;
    std::unique_ptr<Expr> operand;
    
    UnaryExpr(TokenType op, std::unique_ptr<Expr> operand)
        : operatorType(op), operand(std::move(operand)) {}
    
    void accept(Visitor& visitor) override;
};

class TernaryExpr : public Expr {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> thenBranch;
    std::unique_ptr<Expr> elseBranch;
    
    TernaryExpr(std::unique_ptr<Expr> cond,
                std::unique_ptr<Expr> thenBr,
                std::unique_ptr<Expr> elseBr)
        : condition(std::move(cond)),
          thenBranch(std::move(thenBr)),
          elseBranch(std::move(elseBr)) {}
    
    void accept(Visitor& visitor) override;
};

// src/parser/parser_expressions.cpp
ExprPtr ParserCore::expression() {
    return assignment();
}

ExprPtr ParserCore::assignment() {
    ExprPtr expr = logicalOr();
    
    if (match(TokenType::ASSIGN)) {
        Token equals = previous();
        ExprPtr value = assignment();  // Right associative
        
        if (auto* varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(varExpr->name, std::move(value));
        } else if (auto* getExpr = dynamic_cast<GetExpr*>(expr.get())) {
            return std::make_unique<SetExpr>(
                std::move(getExpr->object),
                getExpr->name,
                std::move(value)
            );
        }
        
        error(equals, "Invalid assignment target");
    }
    
    return expr;
}

ExprPtr ParserCore::logicalOr() {
    ExprPtr expr = logicalAnd();
    
    while (match(TokenType::OR)) {
        Token op = previous();
        ExprPtr right = logicalAnd();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr), op.type, std::move(right)
        );
    }
    
    return expr;
}

ExprPtr ParserCore::logicalAnd() {
    ExprPtr expr = equality();
    
    while (match(TokenType::AND)) {
        Token op = previous();
        ExprPtr right = equality();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr), op.type, std::move(right)
        );
    }
    
    return expr;
}

ExprPtr ParserCore::equality() {
    ExprPtr expr = comparison();
    
    while (match({TokenType::EQUAL_EQUAL, TokenType::BANG_EQUAL})) {
        Token op = previous();
        ExprPtr right = comparison();
        expr = std::make_unique<BinaryExpr>(
            std::move(expr), op.type, std::move(right)
        );
    }
    
    return expr;
}

// Similar for comparison(), term(), factor(), unary(), power(), postfix()...

// Parsing function arguments (supports both Arabic and English commas)
std::vector<ExprPtr> ParserCore::parseArguments() {
    std::vector<ExprPtr> args;
    
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            args.push_back(expression());
        } while (match({TokenType::COMMA, TokenType::ARABIC_COMMA}));
    }
    
    return args;
}
```

---

## 2️⃣ Statements (الجمل)

### Syntax (EBNF)
```ebnf
statement ::= expr_stmt
            | var_decl
            | block
            | if_stmt
            | while_stmt
            | for_stmt
            | return_stmt
            | break_stmt
            | continue_stmt
            | import_stmt
            | class_decl
            | function_decl

expr_stmt ::= expression '؛'?

var_decl ::= type IDENTIFIER ['=' expression] '؛'?

block ::= statement* 'نهاية'

if_stmt ::= 'إذا' '(' expression ')' block ('وإلا' 'إذا' '(' expression ')' block)* ['وإلا' block]

while_stmt ::= 'بينما' '(' expression ')' block

for_stmt ::= 'لكل' IDENTIFIER 'في' expression block
           | 'لكل' '(' [var_decl] '؛' [expression] '؛' [expression] ')' block

return_stmt ::= 'إرجاع' [expression] '؛'?

break_stmt ::= 'اخرج' '؛'?

continue_stmt ::= 'تابع' '؛'?
```

### Semantics
- **Semicolons (؛ or ;):** Both Arabic (؛) and English (;) semicolons are supported and optional in most cases
- **Blocks:** End with 'نهاية' keyword, create new scope
- **Return:** Must be inside function
- **Break/Continue:** Must be inside loop

### Examples

```s
# جملة تعبير / Expression statement
اطبع("مرحبا")
5 + 10  # Valid but result discarded

# إعلان متغير / Variable declaration
رقم عمر = 25
نص اسم = "أحمد"
منطقي نشط  # Uninitialized, defaults to false

# كتلة / Block
رقم محلي = 10
اطبع(محلي)
نهاية
# محلي is not accessible here

# جملة if / If statement
إذا (عمر >= 18)
    اطبع("بالغ")
وإلا إذا (عمر >= 13)
    اطبع("مراهق")
وإلا
    اطبع("طفل")
نهاية

# حلقة while / While loop
رقم عداد = 0
بينما (عداد < 5)
    اطبع(عداد)
    عداد = عداد + 1
نهاية

# حلقة for..in / For-in loop
مصفوفة أرقام = [1, 2, 3, 4, 5]
لكل رقم في أرقام
    اطبع(رقم)
نهاية

# حلقة for تقليدية / Traditional for loop
لكل (رقم ع = 0؛ ع < 10؛ ع = ع + 1)
    اطبع(ع)
نهاية

# جملة return / Return statement
دالة جمع(رقم أ، رقم ب) -> رقم
    إرجاع أ + ب
نهاية

# break و continue
لكل رقم في مدى(10)
    إذا (رقم == 5)
        تابع  # Skip 5
    نهاية
    إذا (رقم == 8)
        اخرج   # Exit loop
    نهاية
    اطبع(رقم)
نهاية
# Output: 0 1 2 3 4 6 7

# جملة import / Import statement
استورد "math"
استورد "graphics" كـ رسوميات

# إعلان صنف / Class declaration
صنف شخص
    عام نص اسم
    عام رقم عمر
    
    باني(نص ا، رقم ع)
        هذا.اسم = ا
        هذا.عمر = ع
    نهاية
نهاية
```

### AST Representation
```
IfStmt
├── condition: BinaryExpr
│   ├── left: VariableExpr("عمر")
│   ├── operator: GREATER_EQUAL
│   └── right: LiteralExpr(18)
├── thenBranch: BlockStmt
│   └── ExprStmt(CallExpr("اطبع", ["بالغ"]))
├── elseIfBranches: [
│   └── ElseIfBranch
│       ├── condition: BinaryExpr(عمر >= 13)
│       └── thenBranch: BlockStmt(...)
│   ]
└── elseBranch: BlockStmt
    └── ExprStmt(CallExpr("اطبع", ["طفل"]))
```

### Implementation Notes

```cpp
// include/parser/ast/statement_nodes.h
class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void accept(Visitor& visitor) = 0;
    
    SourceLocation location;
};

class ExprStmt : public Stmt {
public:
    std::unique_ptr<Expr> expression;
    
    explicit ExprStmt(std::unique_ptr<Expr> expr)
        : expression(std::move(expr)) {}
    
    void accept(Visitor& visitor) override;
};

class VarDeclStmt : public Stmt {
public:
    Type* type;
    std::string name;
    std::unique_ptr<Expr> initializer;
    
    VarDeclStmt(Type* type, std::string name,
                std::unique_ptr<Expr> init = nullptr)
        : type(type), name(std::move(name)),
          initializer(std::move(init)) {}
    
    void accept(Visitor& visitor) override;
};

class BlockStmt : public Stmt {
public:
    std::vector<std::unique_ptr<Stmt>> statements;
    
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts)
        : statements(std::move(stmts)) {}
    
    void accept(Visitor& visitor) override;
};

class IfStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> thenBranch;
    std::vector<ElseIfBranch> elseIfBranches;
    std::unique_ptr<BlockStmt> elseBranch;
    
    IfStmt(std::unique_ptr<Expr> cond,
           std::unique_ptr<BlockStmt> thenBr,
           std::vector<ElseIfBranch> elseIfs = {},
           std::unique_ptr<BlockStmt> elseBr = nullptr)
        : condition(std::move(cond)),
          thenBranch(std::move(thenBr)),
          elseIfBranches(std::move(elseIfs)),
          elseBranch(std::move(elseBr)) {}
    
    void accept(Visitor& visitor) override;
};

struct ElseIfBranch {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> thenBranch;
};

class WhileStmt : public Stmt {
public:
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
    
    WhileStmt(std::unique_ptr<Expr> cond,
              std::unique_ptr<BlockStmt> body)
        : condition(std::move(cond)), body(std::move(body)) {}
    
    void accept(Visitor& visitor) override;
};

class ForStmt : public Stmt {
public:
    std::string variable;
    std::unique_ptr<Expr> iterable;
    std::unique_ptr<BlockStmt> body;
    
    ForStmt(std::string var,
            std::unique_ptr<Expr> iter,
            std::unique_ptr<BlockStmt> body)
        : variable(std::move(var)),
          iterable(std::move(iter)),
          body(std::move(body)) {}
    
    void accept(Visitor& visitor) override;
};

class ReturnStmt : public Stmt {
public:
    std::unique_ptr<Expr> value;
    
    explicit ReturnStmt(std::unique_ptr<Expr> val = nullptr)
        : value(std::move(val)) {}
    
    void accept(Visitor& visitor) override;
};

class BreakStmt : public Stmt {
public:
    void accept(Visitor& visitor) override;
};

class ContinueStmt : public Stmt {
public:
    void accept(Visitor& visitor) override;
};

// src/parser/parser_statements.cpp
StmtPtr ParserCore::statement() {
    // Check for specific statement types
    if (match(TokenType::KEYWORD_IF)) {
        return ifStatement();
    }
    if (match(TokenType::KEYWORD_WHILE)) {
        return whileStatement();
    }
    if (match(TokenType::KEYWORD_FOR)) {
        return forStatement();
    }
    if (match(TokenType::KEYWORD_RETURN)) {
        return returnStatement();
    }
    if (match(TokenType::KEYWORD_BREAK)) {
        return breakStatement();
    }
    if (match(TokenType::KEYWORD_CONTINUE)) {
        return continueStatement();
    }
    if (match(TokenType::KEYWORD_END)) {
        return nullptr;  // End of block marker
    }
    if (isTypeKeyword(peek())) {
        return varDeclaration();
    }
    
    // Default: expression statement
    return expressionStatement();
}

StmtPtr ParserCore::ifStatement() {
    // Already consumed 'إذا'
    
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'إذا'");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    
    auto thenBranch = blockStatement();  // Parse until 'وإلا' or 'نهاية'
    
    std::vector<ElseIfBranch> elseIfs;
    while (match(TokenType::KEYWORD_ELSE_IF)) {
        consume(TokenType::LEFT_PAREN, "Expected '(' after 'وإلا إذا'");
        auto elseIfCond = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
        auto elseIfBranch = blockStatement();
        elseIfs.push_back({std::move(elseIfCond), std::move(elseIfBranch)});
    }
    
    std::unique_ptr<BlockStmt> elseBranch;
    if (match(TokenType::KEYWORD_ELSE)) {
        elseBranch = blockStatement();
    }
    
    consume(TokenType::KEYWORD_END, "Expected 'نهاية' at end of if statement");
    
    return std::make_unique<IfStmt>(
        std::move(condition),
        std::move(thenBranch),
        std::move(elseIfs),
        std::move(elseBranch)
    );
}

StmtPtr ParserCore::whileStatement() {
    // Already consumed 'بينما'
    
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'بينما'");
    auto condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    
    auto body = blockStatement();
    
    consume(TokenType::KEYWORD_END, "Expected 'نهاية' at end of while loop");
    
    return std::make_unique<WhileStmt>(
        std::move(condition),
        std::move(body)
    );
}

StmtPtr ParserCore::blockStatement() {
    // Parse statements until we hit 'نهاية', 'وإلا', or 'وإلا إذا'
    std::vector<std::unique_ptr<Stmt>> statements;
    
    while (!check(TokenType::KEYWORD_END) && 
           !check(TokenType::KEYWORD_ELSE) &&
           !check(TokenType::KEYWORD_ELSE_IF) &&
           !isAtEnd()) {
        statements.push_back(statement());
    }
    
    return std::make_unique<BlockStmt>(std::move(statements));
}

StmtPtr ParserCore::forStatement() {
    // Already consumed 'لكل'
    
    Token loopType = previous();
    
    // Check if it's for-each or traditional for
    if (check(TokenType::LEFT_PAREN)) {
        // Traditional for: لكل (init؛ condition؛ increment) body نهاية
        consume(TokenType::LEFT_PAREN, "Expected '('");
        
        StmtPtr init;
        if (!match(TokenType::ARABIC_SEMICOLON)) {
            if (isTypeKeyword(peek())) {
                init = varDeclaration();
            } else {
                init = expressionStatement();
            }
        }
        
        ExprPtr condition;
        if (!check(TokenType::ARABIC_SEMICOLON)) {
            condition = expression();
        }
        consume(TokenType::ARABIC_SEMICOLON, "Expected '؛'");
        
        ExprPtr increment;
        if (!check(TokenType::RIGHT_PAREN)) {
            increment = expression();
        }
        consume(TokenType::RIGHT_PAREN, "Expected ')'");
        
        auto body = blockStatement();
        
        consume(TokenType::KEYWORD_END, "Expected 'نهاية' at end of for loop");
        
        // Desugar to while loop
        return std::make_unique<TraditionalForStmt>(
            std::move(init),
            std::move(condition),
            std::move(increment),
            std::move(body)
        );
    } else {
        // For-each: لكل variable في iterable body نهاية
        Token varName = consume(TokenType::IDENTIFIER, "Expected variable name");
        consume(TokenType::KEYWORD_IN, "Expected 'في'");
        auto iterable = expression();
        auto body = blockStatement();
        
        consume(TokenType::KEYWORD_END, "Expected 'نهاية' at end of for-each loop");
        
        return std::make_unique<ForStmt>(
            varName.lexeme,
            std::move(iterable),
            std::move(body)
        );
    }
}
            std::move(body)
        );
    }
}
```

---

## 3️⃣ Control Flow (هياكل التحكم)

### Syntax Summary
```ebnf
if_stmt      ::= 'إذا' '(' expr ')' block ('وإلا' 'إذا' '(' expr ')' block)* ['وإلا' block]
while_stmt   ::= 'بينما' '(' expr ')' block
for_in_stmt  ::= 'لكل' ID 'في' expr block
for_stmt     ::= 'لكل' '(' [stmt] '؛' [expr] '؛' [expr] ')' block
switch_stmt  ::= 'حالة' expr case_branch* [default_branch] 'نهاية'
try_stmt     ::= 'حاول' block ('امسك' ID block)+ ['أخيراً' block] 'نهاية'
```

### Examples

```s
// If-Else-If Chain
إذا (درجة >= 90)
    اطبع("ممتاز")
وإلا إذا (درجة >= 80)
    اطبع("جيد جداً")
وإلا إذا (درجة >= 70)
    اطبع("جيد")
وإلا إذا (درجة >= 60)
    اطبع("مقبول")
وإلا
    اطبع("راسب")
نهاية

// While Loop
رقم ع = 0
بينما (ع < 10)
    اطبع(ع)
    ع = ع + 1
نهاية

// For-Each Loop
مصفوفة أسماء = ["أحمد", "سارة", "محمد"]
لكل اسم في أسماء
    اطبع("مرحبا يا {اسم}")
نهاية

// Range Loop
لكل ع في مدى(0, 10, 2)
    اطبع(ع)  // 0, 2, 4, 6, 8
نهاية

// Traditional For Loop
لكل (رقم ع = 0؛ ع < 5؛ ع = ع + 1)
    اطبع(ع * ع)  // 0, 1, 4, 9, 16
نهاية

# Nested Loops
لكل (رقم ع = 1؛ ع <= 3؛ ع = ع + 1)
    لكل (رقم ص = 1؛ ص <= 3؛ ص = ص + 1)
        اطبع(ن"{ع} × {ص} = {ع * ص}")
    نهاية
نهاية

# Break and Continue
لكل رقم في مدى(20)
    إذا (رقم % 2 == 0)
        تابع  # Skip even numbers
    نهاية
    إذا (رقم > 10)
        اخرج   # Stop at 10
    نهاية
    اطبع(رقم)
نهاية
# Output: 1, 3, 5, 7, 9

# Switch/Match Statement (Phase 2)
حالة قيمة
    عندما 1:
        اطبع("واحد")
    عندما 2:
        اطبع("اثنان")
    عندما 3:
        اطبع("ثلاثة")
    افتراضي:
        اطبع("آخر")
نهاية

# Try-Catch (Phase 2)
حاول
    رقم نتيجة = 10 / 0
امسك خطأ
    اطبع(ن"خطأ: {خطأ.رسالة}")
أخيراً
    اطبع("تم التنفيذ")
نهاية
```

---

## 4️⃣ Operators (العوامل)

### Operator Categories

| الفئة (AR) | Category (EN) | العوامل (Operators) |
|-----------|--------------|-------------------|
| حسابية | Arithmetic | `+`, `-`, `*`, `/`, `%`, `**` |
| مقارنة | Comparison | `==`, `!=`, `<`, `>`, `<=`, `>=` |
| منطقية | Logical | `&&`, `\|\|`, `!` |
| bitwise | Bitwise | `&`, `\|`, `^`, `~`, `<<`, `>>` |
| إسناد | Assignment | `=`, `+=`, `-=`, `*=`, `/=`, `%=` |
| عضوية | Membership | `في` (in) |
| نطاق | Range | `..`, `..=` |
| ثلاثي | Ternary | `? :` |
| وصول | Access | `.`, `[]`, `()` |

### Operator Details

```s
# 1. Arithmetic Operators (عوامل حسابية)
رقم جمع = 10 + 5        # 15 - Addition
رقم طرح = 10 - 5        # 5  - Subtraction
رقم ضرب = 10 * 5        # 50 - Multiplication
رقم قسمة = 10 / 5       # 2  - Division
رقم باقي = 10 % 3       # 1  - Modulo
رقم قوة = 2 ** 8        # 256 - Power

# 2. Comparison Operators (عوامل مقارنة)
منطقي يساوي = 5 == 5           # true
منطقي لا_يساوي = 5 != 3        # true
منطقي أصغر = 3 < 5             # true
منطقي أكبر = 10 > 5            # true
منطقي أصغر_أو_يساوي = 5 <= 5   # true
منطقي أكبر_أو_يساوي = 10 >= 5  # true

# 3. Logical Operators (عوامل منطقية)
منطقي and = صحيح && صحيح    # true
منطقي or = صحيح || خطأ      # true
منطقي not = !صحيح           # false

# Short-circuit evaluation
منطقي نتيجة = (5 > 3) || (10 / 0)  # true (doesn't evaluate 10/0)

# 4. Bitwise Operators (عوامل bitwise)
رقم and = 0b1100 & 0b1010    # 0b1000 = 8
رقم or = 0b1100 | 0b1010     # 0b1110 = 14
رقم xor = 0b1100 ^ 0b1010    # 0b0110 = 6
رقم not = ~0b1010            # -11 (two's complement)
رقم shift_left = 0b0010 << 2  # 0b1000 = 8
رقم shift_right = 0b1000 >> 1 # 0b0100 = 4

# 5. Assignment Operators (عوامل إسناد)
رقم ع = 10
ع += 5    # ع = ع + 5  → 15
ع -= 3    # ع = ع - 3  → 12
ع *= 2    # ع = ع * 2  → 24
ع /= 4    # ع = ع / 4  → 6
ع %= 4    # ع = ع % 4  → 2
ع **= 3   # ع = ع ** 3 → 8

# 6. Membership Operator (عامل العضوية)
منطقي موجود = "أ" في "أبجد"      # true
منطقي موجود = 3 في [1, 2, 3, 4]  # true
منطقي موجود = "مفتاح" في {"مفتاح": "قيمة"}  # true

# 7. Range Operator (عامل النطاق)
مصفوفة نطاق1 = 0..5      # [0, 1, 2, 3, 4] (exclusive)
مصفوفة نطاق2 = 0..=5     # [0, 1, 2, 3, 4, 5] (inclusive)
مصفوفة نطاق3 = 0..10..2  # [0, 2, 4, 6, 8] (with step)

# 8. Ternary Operator (عامل ثلاثي)
رقم قيمة = (ع >= 0) ? ع : -ع  # Absolute value
نص حالة = (عمر >= 18) ? "بالغ" : "قاصر"

# 9. Access Operators (عوامل الوصول)
# Member access (.)
نقطة ن = نقطة(3, 4)
رقم س = ن.س              # 3

# Index access ([])
مصفوفة قائمة = [10, 20, 30]
رقم عنصر = قائمة[1]      # 20

# Function call (())
رقم نتيجة = جمع(5, 10)   # 15

# 10. String Operators (عوامل السلاسل)
نص دمج = "مرحبا" + " " + "عالم"  # Concatenation
نص تكرار = "abc" * 3              # Repetition: "abcabcabc"
رقم طول = طول("مرحبا")            # Length: 5
نص شريحة = "مرحبا"[0:3]           # Slice: "مرح"
```

### Implementation Notes

```cpp
// src/interpreter/expressions/binary_evaluator.cpp
Value InterpreterCore::visitBinaryExpr(BinaryExpr* expr) {
    // Evaluate operands
    expr->left->accept(*this);
    Value left = lastValue;
    
    // Short-circuit evaluation for logical operators
    if (expr->operatorType == TokenType::AND) {
        if (!left.toBool()) {
            return Value(false);  // Don't evaluate right
        }
        expr->right->accept(*this);
        return Value(lastValue.toBool());
    }
    
    if (expr->operatorType == TokenType::OR) {
        if (left.toBool()) {
            return Value(true);  // Don't evaluate right
        }
        expr->right->accept(*this);
        return Value(lastValue.toBool());
    }
    
    // Evaluate right operand
    expr->right->accept(*this);
    Value right = lastValue;
    
    // Perform operation based on operator type
    switch (expr->operatorType) {
        case TokenType::PLUS:
            return evaluatePlus(left, right);
        case TokenType::MINUS:
            return evaluateMinus(left, right);
        case TokenType::MULTIPLY:
            return evaluateMultiply(left, right);
        case TokenType::DIVIDE:
            return evaluateDivide(left, right);
        case TokenType::MODULO:
            return evaluateModulo(left, right);
        case TokenType::POWER:
            return evaluatePower(left, right);
        case TokenType::EQUAL_EQUAL:
            return Value(left == right);
        case TokenType::BANG_EQUAL:
            return Value(!(left == right));
        case TokenType::LESS:
            return evaluateLess(left, right);
        case TokenType::GREATER:
            return evaluateGreater(left, right);
        case TokenType::LESS_EQUAL:
            return evaluateLessEqual(left, right);
        case TokenType::GREATER_EQUAL:
            return evaluateGreaterEqual(left, right);
        case TokenType::BIT_AND:
            return evaluateBitAnd(left, right);
        case TokenType::BIT_OR:
            return evaluateBitOr(left, right);
        case TokenType::BIT_XOR:
            return evaluateBitXor(left, right);
        case TokenType::SHIFT_LEFT:
            return evaluateShiftLeft(left, right);
        case TokenType::SHIFT_RIGHT:
            return evaluateShiftRight(left, right);
        case TokenType::IN:
            return evaluateMembership(left, right);
        default:
            throw RuntimeError("Unknown binary operator");
    }
}

Value InterpreterCore::evaluatePlus(const Value& left, const Value& right) {
    // Integer + Integer
    if (left.isInteger() && right.isInteger()) {
        return Value(left.asInteger() + right.asInteger());
    }
    
    // Float + Float or mixed
    if (left.isNumber() && right.isNumber()) {
        double a = left.isFloat() ? left.asFloat() : (double)left.asInteger();
        double b = right.isFloat() ? right.asFloat() : (double)right.asInteger();
        return Value(a + b);
    }
    
    // String + String
    if (left.isString() && right.isString()) {
        return Value(left.asString() + right.asString());
    }
    
    // Array + Array
    if (left.isArray() && right.isArray()) {
        auto result = left.asArray();
        const auto& rightArr = right.asArray();
        result.insert(result.end(), rightArr.begin(), rightArr.end());
        return Value::makeArray(result);
    }
    
    throw RuntimeError("Invalid operand types for '+'");
}
```

---

## 5️⃣ Precedence Table (جدول الأسبقية)

### Operator Precedence (من الأعلى إلى الأدنى)

| الأولوية | العامل | الاتجاه | الوصف |
|---------|--------|---------|-------|
| 1 | `()` `[]` `.` | L → R | Call, Index, Member access |
| 2 | `**` | R → L | Exponentiation |
| 3 | `!` `-` `+` `~` | R → L | Unary (not, negate, plus, bitwise not) |
| 4 | `*` `/` `%` | L → R | Multiplication, Division, Modulo |
| 5 | `+` `-` | L → R | Addition, Subtraction |
| 6 | `<<` `>>` | L → R | Bitwise shift |
| 7 | `<` `<=` `>` `>=` | L → R | Comparison |
| 8 | `==` `!=` | L → R | Equality |
| 9 | `&` | L → R | Bitwise AND |
| 10 | `^` | L → R | Bitwise XOR |
| 11 | `\|` | L → R | Bitwise OR |
| 12 | `&&` | L → R | Logical AND |
| 13 | `\|\|` | L → R | Logical OR |
| 14 | `في` (in) | L → R | Membership |
| 15 | `? :` | R → L | Ternary conditional |
| 16 | `=` `+=` `-=` etc. | R → L | Assignment |

### Examples Demonstrating Precedence

```s
# Precedence examples
رقم ن1 = 2 + 3 * 4        # 14, not 20 (* before +)
رقم ن2 = (2 + 3) * 4      # 20 (parentheses override)
رقم ن3 = 2 ** 3 ** 2      # 512 (right associative: 2^(3^2))
رقم ن4 = -5 ** 2          # -25 (** before -)
رقم ن5 = (-5) ** 2        # 25

منطقي ب1 = صحيح || خطأ && خطأ  # true (&& before ||)
منطقي ب2 = (صحيح || خطأ) && خطأ # false

رقم ن6 = 10 + 5 * 2 - 3    # 17 (10 + 10 - 3)
رقم ن7 = 5 > 3 && 10 < 20   # true (> before &&)

# Assignment is right-associative
رقم أ = رقم ب = رقم ج = 5   # All three are 5

# Compound assignment
رقم ع = 10
ع += 5 * 2    # ع = ع + (5 * 2) = 20, not (ع + 5) * 2
```

---

## 6️⃣ Comments (التعليقات)

### Syntax (EBNF)
```ebnf
comment ::= line_comment | block_comment | doc_comment

line_comment  ::= '#' any_char* NEWLINE

block_comment ::= '#*' any_char* '*#'

doc_comment   ::= '##' any_char* NEWLINE
                | '#**' any_char* '**#'
```

### Examples

```s
# سطر تعليق واحد / Single line comment
# هذا تعليق بالعربية
# This is a comment in English

رقم عمر = 25  # تعليق في نهاية السطر / Inline comment

#* 
   تعليق متعدد الأسطر
   Multi-line comment
   يمكن أن يمتد على عدة أسطر
   Can span multiple lines
*#

#*
 * تعليق منسق
 * Formatted comment
 * مع نجوم على الجانب
 * With stars on the side
 *#

## تعليق توثيقي / Documentation comment
## يُستخدم لتوثيق الدوال والأصناف
## Used to document functions and classes
دالة جمع(رقم أ، رقم ب) -> رقم
    إرجاع أ + ب
نهاية

#**
 * @brief جمع رقمين / Add two numbers
 * @param أ الرقم الأول / First number
 * @param ب الرقم الثاني / Second number
 * @return المجموع / The sum
 **#
دالة جمع_موثق(رقم أ، رقم ب) -> رقم
    إرجاع أ + ب
نهاية

# تعليق متداخل (Phase 2)
#* هذا تعليق #* مع تعليق داخلي *# خارجي *#
```

### Implementation Notes

```cpp
// src/lexer/lexer_comments.cpp
void LexerCore::skipComment() {
    if (peek() == '#') {
        char next = peekNext();
        
        if (next == '#') {
            // Doc comment: ## or #**
            advance(); // consume first '#'
            advance(); // consume second '#'
            
            if (peek() == '*' && peekNext() == '*') {
                // Multi-line doc comment: #**...**#
                advance(); // consume first '*'
                advance(); // consume second '*'
                
                while (!(peek() == '*' && peekNext() == '*' && 
                        peekAhead(2) == '#') && !isAtEnd()) {
                    advance();
                }
                if (!isAtEnd()) {
                    advance(); // consume '*'
                    advance(); // consume '*'
                    advance(); // consume '#'
                }
            } else {
                // Single-line doc comment: ##...
                while (peek() != '\n' && !isAtEnd()) {
                    advance();
                }
            }
        } else if (next == '*') {
            // Block comment: #*...*#
            advance(); // consume '#'
            advance(); // consume '*'
            
            int depth = 1;  // For nested comments (Phase 2)
            
            while (depth > 0 && !isAtEnd()) {
                if (peek() == '#' && peekNext() == '*') {
                    advance();
                    advance();
                    depth++;  // Nested comment start
                } else if (peek() == '*' && peekNext() == '#') {
                    advance();
                    advance();
                    depth--;  // Nested comment end
                } else {
                    advance();
                }
            }
            
            if (depth > 0) {
                error("Unterminated block comment");
            }
        } else {
            // Line comment: #...
            while (peek() != '\n' && !isAtEnd()) {
                advance();
            }
        }
    }
}

Token LexerCore::scanDocComment() {
    // ## or #**
    std::string comment;
    
    if (peek() == '#' && peekNext() == '#') {
        advance(); // consume first '#'
        advance(); // consume second '#'
        
        if (peek() == '*' && peekNext() == '*') {
            // Multi-line doc comment: #**...**#
            advance(); // consume first '*'
            advance(); // consume second '*'
            
            while (!(peek() == '*' && peekNext() == '*' && 
                    peekAhead(2) == '#') && !isAtEnd()) {
                comment += advance();
            }
            
            if (!isAtEnd()) {
                advance(); // consume '*'
                advance(); // consume '*'
                advance(); // consume '#'
            }
        } else {
            // Single-line doc comment: ##...
            while (peek() != '\n' && !isAtEnd()) {
                comment += advance();
            }
        }
    }
    
    return Token(TokenType::DOC_COMMENT, comment, getCurrentPosition());
}
```

---

## 7️⃣ Encoding & RTL Support (الترميز ودعم النص من اليمين)

### UTF-8 Encoding
- **All source files:** UTF-8 encoded
- **Identifiers:** Support Arabic, English, numbers, underscore
- **Strings:** Full Unicode support

### RTL (Right-to-Left) Support
- **Direction:** Arabic text flows right-to-left
- **Mixing:** Can mix RTL (Arabic) and LTR (English) in same file
- **Comments:** Support both directions

### Examples

```s
# أسماء متغيرات بالعربية / Arabic variable names
رقم العمر = 25
نص الاسم_الكامل = "أحمد محمد"

# Mixing Arabic and English
رقم age = 25
نص fullName = "Ahmad Mohammad"

# RTL strings with Unicode
نص رسالة = "مرحباً بك في لغة ص 😊"
نص emojis = "🎉🚀💻📚"

# Mathematical symbols
رقم π = 3.14159
رقم Σ = مجموع([1, 2, 3, 4])

# Special characters in identifiers (Phase 2)
رقم المجموع_الكلي = 100
رقم total_sum = 100
```

### Implementation Notes

```cpp
// include/lexer/token.h
enum class TokenType {
    // ... existing tokens
    COMMA,               // , (regular comma)
    ARABIC_COMMA,        // ، (U+060C Arabic comma)
    SEMICOLON,           // ; (regular semicolon)
    ARABIC_SEMICOLON,    // ؛ (U+061B Arabic semicolon)
    KEYWORD_END,         // نهاية (end keyword)
    // ... other tokens
};

// src/lexer/lexer_core.cpp
Token LexerCore::scanToken() {
    char c = advance();
    
    switch (c) {
        case ';':
            return Token(TokenType::SEMICOLON, ";", getCurrentPosition());
        
        // ... other single character tokens
    }
    
    // Check for multi-byte UTF-8 characters
    if (c == '\xD8' && peek() == '\x9B') {  // ؛ is 0xD8 0x9B in UTF-8
        advance();  // consume second byte
        return Token(TokenType::ARABIC_SEMICOLON, "؛", getCurrentPosition());
    }
    
    if (c == '\xD8' && peek() == '\x8C') {  // ، is 0xD8 0x8C in UTF-8
        advance();  // consume second byte
        return Token(TokenType::ARABIC_COMMA, "،", getCurrentPosition());
    }
    
    // ... rest of lexer logic
}

// src/lexer/lexer_identifiers.cpp
bool LexerCore::isArabicLetter(char32_t c) {
    // Arabic Unicode ranges
    return (c >= 0x0600 && c <= 0x06FF) ||  // Arabic
           (c >= 0x0750 && c <= 0x077F) ||  // Arabic Supplement
           (c >= 0xFB50 && c <= 0xFDFF) ||  // Arabic Presentation Forms-A
           (c >= 0xFE70 && c <= 0xFEFF);    // Arabic Presentation Forms-B
}

bool LexerCore::isValidIdentifierStart(char32_t c) {
    return isArabicLetter(c) ||
           (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

bool LexerCore::isValidIdentifierChar(char32_t c) {
    return isValidIdentifierStart(c) ||
           (c >= '0' && c <= '9');
}

std::string LexerCore::scanIdentifier() {
    std::string identifier;
    
    // Read UTF-8 characters
    while (isValidIdentifierChar(peekUTF8()) && !isAtEnd()) {
        identifier += advanceUTF8();
    }
    
    return identifier;
}

// UTF-8 helper functions
char32_t LexerCore::peekUTF8() {
    size_t pos = current;
    return decodeUTF8(source, pos);
}

char32_t LexerCore::advanceUTF8() {
    char32_t c = decodeUTF8(source, current);
    // current is advanced by decodeUTF8
    return c;
}

char32_t LexerCore::decodeUTF8(const std::string& str, size_t& pos) {
    if (pos >= str.size()) return 0;
    
    unsigned char byte1 = str[pos++];
    
    // Single byte (ASCII)
    if ((byte1 & 0x80) == 0) {
        return byte1;
    }
    
    // Two bytes
    if ((byte1 & 0xE0) == 0xC0) {
        unsigned char byte2 = str[pos++];
        return ((byte1 & 0x1F) << 6) | (byte2 & 0x3F);
    }
    
    // Three bytes
    if ((byte1 & 0xF0) == 0xE0) {
        unsigned char byte2 = str[pos++];
        unsigned char byte3 = str[pos++];
        return ((byte1 & 0x0F) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
    }
    
    // Four bytes
    if ((byte1 & 0xF8) == 0xF0) {
        unsigned char byte2 = str[pos++];
        unsigned char byte3 = str[pos++];
        unsigned char byte4 = str[pos++];
        return ((byte1 & 0x07) << 18) | ((byte2 & 0x3F) << 12) |
               ((byte3 & 0x3F) << 6) | (byte4 & 0x3F);
    }
    
    throw LexerError("Invalid UTF-8 sequence");
}
```

---

## 8️⃣ Reserved Keywords (الكلمات المحجوزة)

### Complete Keyword List

| العربية | English | الفئة | Category |
|--------|---------|-------|----------|
| رقم | int | نوع | Type |
| عشري | float | نوع | Type |
| منطقي | bool | نوع | Type |
| نص | string | نوع | Type |
| مصفوفة | array | نوع | Type |
| خريطة | map | نوع | Type |
| أي | any | نوع | Type |
| دالة | func | تعريف | Definition |
| صنف | class | تعريف | Definition |
| باني | constructor | تعريف | Definition |
| ~باني | destructor | تعريف | Definition |
| إذا | if | تحكم | Control |
| وإلا | else | تحكم | Control |
| بينما | while | تحكم | Control |
| لكل | foreach | تحكم | Control |
| في | in | تحكم | Control |
| حالة | switch | تحكم | Control |
| عندما | case | تحكم | Control |
| افتراضي | default | تحكم | Control |
| إرجاع | return | تحكم | Control |
| اخرج | break | تحكم | Control |
| تابع | continue | تحكم | Control |
| حاول | try | استثناءات | Exception |
| امسك | catch | استثناءات | Exception |
| أخيراً | finally | استثناءات | Exception |
| ارمِ | throw | استثناءات | Exception |
| صحيح | true | قيمة | Value |
| خطأ | false | قيمة | Value |
| لاشيء | none | قيمة | Value |
| هذا | this | مرجع | Reference |
| الأساس | super | مرجع | Reference |
| عام | public | وصول | Access |
| خاص | private | وصول | Access |
| محمي | protected | وصول | Access |
| ثابت | static | معدِّل | Modifier |
| استورد | import | وحدة | Module |
| كـ | as | وحدة | Module |
| من | from | وحدة | Module |
| صدّر | export | وحدة | Module |
| نهاية | end | هيكل | Structure |

### Notes
- **نهاية (end):** Used to close blocks (if, while, for, functions, classes) instead of curly braces
- **؛ (Arabic semicolon - U+061B) or ; (English semicolon):** Both supported for statement termination and for loop separators
- **، (Arabic comma - U+060C) or , (English comma):** Both can be used interchangeably in function arguments and lists
- **لكل (foreach):** Used for both for-each loops (`لكل متغير في مجموعة`) and traditional for loops (`لكل (init؛ condition؛ increment)`)
- **Comments:** Use `#` for line comments, `#* *#` for block comments, `##` or `#** **#` for documentation comments

---

## 📝 Tasks (مهام التنفيذ)

### Task 1: Expression Parsing (5 أيام)
- [ ] Implement precedence climbing parser
- [ ] All binary operators
- [ ] Unary operators
- [ ] Ternary operator
- [ ] Member/index access
- **Files:** `src/parser/parser_expressions.cpp`
- **Tests:** `tests/spec_rules/syntax_expressions.s`

### Task 2: Statement Parsing (4 أيام)
- [ ] Variable declarations
- [ ] Expression statements
- [ ] Block statements with 'نهاية' keyword
- [ ] Return/break/continue
- [ ] Both Arabic (؛) and English (;) semicolons supported
- **Files:** `src/parser/parser_statements.cpp`, `include/lexer/token.h`
- **Tests:** `tests/spec_rules/syntax_statements.s`

### Task 3: Control Flow (3 أيام)
- [ ] If-else-if chains with parenthesized conditions
- [ ] While loops with parenthesized conditions
- [ ] For-each loops
- [ ] Traditional for loops with ؛ or ; separator
- [ ] All control structures end with 'نهاية'
- **Files:** `src/parser/parser_control_flow.cpp`
- **Tests:** `tests/spec_rules/syntax_control_flow.s`

### Task 4: Comments (يوم واحد)
- [ ] Line comments with #
- [ ] Block comments with #* *#
- [ ] Doc comments with ## and #** **#
- **Files:** `src/lexer/lexer_comments.cpp`

### Task 5: UTF-8 & RTL (3 أيام)
- [ ] UTF-8 decoding
- [ ] Arabic identifier support
- [ ] Unicode in strings
- **Files:** `src/lexer/lexer_unicode.cpp`, `src/utils/string_utils.cpp`
- **Tests:** `tests/spec_rules/syntax_unicode.s`

### Task 6: Operator Implementation (4 أيام)
- [ ] All arithmetic operators
- [ ] Comparison operators
- [ ] Logical operators with short-circuit
- [ ] Bitwise operators
- [ ] Compound assignment
- **Files:** `src/interpreter/expressions/binary_evaluator.cpp`
- **Tests:** `tests/spec_rules/syntax_operators.s`

**Total Time:** 20 يوماً (160 ساعة)

---

## 🔧 Git Workflow

```powershell
# إنشاء فرع / Create branch
git checkout -b feat/spec/syntax

# إضافة الملفات / Add files
git add docs/language_spec/rules/04_syntax.md
git add tests/spec_rules/syntax_*.s
git add src/parser/parser_expressions.cpp
git add src/parser/parser_statements.cpp
git add src/parser/parser_control_flow.cpp
git add src/lexer/lexer_comments.cpp
git add src/lexer/lexer_unicode.cpp
git add src/interpreter/expressions/binary_evaluator.cpp
git add include/lexer/token.h  # Add KEYWORD_END, ARABIC_SEMICOLON, ARABIC_COMMA

# Commit
git commit -m "[spec/syntax] Add complete syntax rules with Arabic support

- Expression parsing with full precedence table
- Statement parsing (var, expr, block with 'نهاية', return, etc.)
- Control flow (if-else, while, for-each, for)
- Conditions in parentheses: إذا (شرط) and بينما (شرط)
- Blocks end with 'نهاية' keyword instead of braces
- Arabic semicolon (؛) support throughout
- Arabic comma (،) and English comma (,) both supported in arguments
- Unified 'لكل' keyword for both for-each and traditional for loops
- All operators (arithmetic, logical, bitwise, etc.)
- Comments (line, block, doc)
- UTF-8 and RTL support
- Complete keyword list including 'نهاية'
- 6 test files with 80+ test cases"

# Push
git push --set-upstream origin feat/spec/syntax
```

---

## 📋 Summary of Syntax Changes

### ✅ Key Modifications from Standard Syntax:

1. **Block Termination:**
   - ❌ Old: `{ statements }`
   - ✅ New: `statements نهاية`

2. **Conditional Syntax:**
   - ❌ Old: `إذا condition { ... }`
   - ✅ New: `إذا (condition) ... نهاية`

3. **Loop Syntax:**
   - ❌ Old: `بينما condition { ... }`
   - ✅ New: `بينما (condition) ... نهاية`

4. **Semicolon:**
   - ❌ Old: `;` (ASCII semicolon)
   - ✅ New: `؛` (Arabic semicolon U+061B)

5. **Comma (both supported):**
   - ✅ `،` (Arabic comma U+060C) - Preferred
   - ✅ `,` (English comma) - Also accepted

6. **For Loops:**
   - ❌ Old: `لـ (init; cond; incr)` and `لكل elem في list`
   - ✅ New: Both use `لكل`:
     - For-each: `لكل متغير في مجموعة ... نهاية`
     - Traditional: `لكل (init؛ cond؛ incr) ... نهاية`

### Example Comparison:

**Old Syntax (C-style):**
```s
إذا x > 0 {
    اطبع("positive");
} وإلا {
    اطبع("negative");
}
```

**New Syntax (نهاية-style with Arabic punctuation):**
```s
إذا (x > 0)
    اطبع("positive")
وإلا
    اطبع("negative")
نهاية

// Function call examples
رقم نتيجة1 = جمع(5، 10، 15)  // Arabic comma ،
رقم نتيجة2 = جمع(5, 10, 15)  // English comma , (also works)
```

---

**آخر تحديث:** 5 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 70+
