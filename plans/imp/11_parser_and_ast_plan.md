# 🌳 خطة المُحلل النحوي و AST / Parser & AST Implementation Plan

**التاريخ:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**المدة المقدرة:** 4 أسابيع

---

## 📋 ملخص / Summary

### (AR) الملخص
خطة تفصيلية لتطوير المحلل النحوي (Parser) وشجرة الصيغة المجردة (AST) للغة "ص". يشمل EBNF grammar، Recursive Descent Parser، معالجة الأخطاء، والـ Visitor Pattern.

### (EN) Summary
Detailed plan for developing the Parser and Abstract Syntax Tree (AST) for Sad language. Includes EBNF grammar, Recursive Descent Parser, error handling, and Visitor Pattern.

---

## 🎯 الأهداف / Objectives

1. ✅ تعريف EBNF grammar كامل للغة
2. ✅ بناء AST classes لجميع العُقد
3. ✅ تنفيذ Recursive Descent Parser
4. ✅ معالجة أولوية العوامل (operator precedence)
5. ✅ Error recovery و bilingual diagnostics
6. ✅ Visitor Pattern لاجتياز الشجرة
7. ✅ تحليل دلالي أساسي (type checking)
8. ✅ AST optimization passes

---

## 📦 الاعتماديات / Dependencies

### من language_spec:
- ✅ `02_functions.md` - القواعد النحوية للدوال
- ✅ `03_oop.md` - القواعد النحوية للـ OOP
- ✅ `06_syntax_examples.md` - أمثلة الصيغة
- ✅ `01_types.md` - أنواع البيانات

### من plans:
- ✅ `09_integration_and_architecture.md` - AST interfaces
- ✅ `10_lexer_implementation_plan.md` - Token definitions

---

## 🏗️ البنية المعمارية / Architecture

### EBNF Grammar (مبسّط)

```ebnf
# ===== Program =====
program         ::= statement* EOF

# ===== Statements =====
statement       ::= declaration
                  | expression_stmt
                  | if_stmt
                  | while_stmt
                  | for_stmt
                  | return_stmt
                  | break_stmt
                  | continue_stmt
                  | block

declaration     ::= var_decl | func_decl | class_decl

var_decl        ::= TYPE IDENTIFIER ("=" expression)? ";"

func_decl       ::= "دالة" IDENTIFIER "(" parameters? ")" "->" TYPE block

class_decl      ::= "فئة" IDENTIFIER (":" IDENTIFIER)? class_body

# ===== Expressions =====
expression      ::= assignment

assignment      ::= logical_or (("=" | "+=" | "-=") assignment)?

logical_or      ::= logical_and ("||" logical_and)*

logical_and     ::= equality ("&&" equality)*

equality        ::= comparison (("==" | "!=") comparison)*

comparison      ::= term (("<" | ">" | "<=" | ">=") term)*

term            ::= factor (("+" | "-") factor)*

factor          ::= unary (("*" | "/" | "%") unary)*

unary           ::= ("!" | "-" | "~") unary | power

power           ::= postfix ("**" unary)?

postfix         ::= primary (call | index | member)*

call            ::= "(" arguments? ")"
index           ::= "[" expression "]"
member          ::= "." IDENTIFIER

primary         ::= LITERAL
                  | IDENTIFIER
                  | "(" expression ")"
                  | lambda
                  | array_literal
                  | map_literal

# ===== Literals =====
array_literal   ::= "[" (expression ("," expression)*)? "]"
map_literal     ::= "{" (map_pair ("," map_pair)*)? "}"
map_pair        ::= expression ":" expression
lambda          ::= "lambda" "(" parameters? ")" "->" expression
```

---

### AST Node Hierarchy

```cpp
// include/parser/ast/ast_node.h
namespace Sad::AST {

// ===== Base Node =====
class ASTNode {
public:
    Position position;
    
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// ===== Expressions =====
class Expression : public ASTNode {
public:
    virtual DataType getType() const = 0;
};

// Binary Expression: a + b, a * b
class BinaryExpr : public Expression {
public:
    std::unique_ptr<Expression> left;
    TokenType op;
    std::unique_ptr<Expression> right;
    
    BinaryExpr(std::unique_ptr<Expression> l, TokenType o, 
               std::unique_ptr<Expression> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitBinaryExpr(this);
    }
    
    std::string toString() const override {
        return "(" + left->toString() + " " + 
               tokenTypeToString(op) + " " + 
               right->toString() + ")";
    }
};

// Unary Expression: -x, !flag
class UnaryExpr : public Expression {
public:
    TokenType op;
    std::unique_ptr<Expression> operand;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitUnaryExpr(this);
    }
};

// Literal: 42, "text", true
class LiteralExpr : public Expression {
public:
    Token token;
    
    LiteralExpr(const Token& t) : token(t) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitLiteralExpr(this);
    }
    
    DataType getType() const override {
        switch (token.type) {
            case TokenType::INTEGER_LITERAL: return DataType::INTEGER;
            case TokenType::FLOAT_LITERAL: return DataType::FLOAT;
            case TokenType::STRING_LITERAL: return DataType::STRING;
            case TokenType::KEYWORD_TRUE:
            case TokenType::KEYWORD_FALSE: return DataType::BOOL;
            default: return DataType::UNKNOWN;
        }
    }
};

// Variable: اسم, x
class VariableExpr : public Expression {
public:
    std::string name;
    
    VariableExpr(const std::string& n) : name(n) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitVariableExpr(this);
    }
};

// Call Expression: دالة(arg1, arg2)
class CallExpr : public Expression {
public:
    std::unique_ptr<Expression> callee;
    std::vector<std::unique_ptr<Expression>> arguments;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitCallExpr(this);
    }
};

// Array Literal: [1, 2, 3]
class ArrayExpr : public Expression {
public:
    std::vector<std::unique_ptr<Expression>> elements;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitArrayExpr(this);
    }
};

// Index Expression: arr[0]
class IndexExpr : public Expression {
public:
    std::unique_ptr<Expression> object;
    std::unique_ptr<Expression> index;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitIndexExpr(this);
    }
};

// Member Access: obj.property
class MemberExpr : public Expression {
public:
    std::unique_ptr<Expression> object;
    std::string member;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitMemberExpr(this);
    }
};

// Lambda: lambda(x) -> x * 2
class LambdaExpr : public Expression {
public:
    std::vector<Parameter> parameters;
    std::unique_ptr<Expression> body;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitLambdaExpr(this);
    }
};

// ===== Statements =====
class Statement : public ASTNode {
};

// Variable Declaration: رقم عمر = 25
class VarDeclStmt : public Statement {
public:
    DataType type;
    std::string name;
    std::unique_ptr<Expression> initializer;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitVarDeclStmt(this);
    }
};

// Expression Statement: print(x);
class ExprStmt : public Statement {
public:
    std::unique_ptr<Expression> expr;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitExprStmt(this);
    }
};

// If Statement
class IfStmt : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> thenBranch;
    std::unique_ptr<Statement> elseBranch; // optional
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitIfStmt(this);
    }
};

// While Statement
class WhileStmt : public Statement {
public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitWhileStmt(this);
    }
};

// For Statement: لكل عنصر في قائمة
class ForStmt : public Statement {
public:
    std::string variable;
    std::unique_ptr<Expression> iterable;
    std::unique_ptr<Statement> body;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitForStmt(this);
    }
};

// Return Statement
class ReturnStmt : public Statement {
public:
    std::unique_ptr<Expression> value; // optional
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitReturnStmt(this);
    }
};

// Block: { ... }
class BlockStmt : public Statement {
public:
    std::vector<std::unique_ptr<Statement>> statements;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitBlockStmt(this);
    }
};

// Function Declaration
class FunctionDecl : public Statement {
public:
    std::string name;
    std::vector<Parameter> parameters;
    DataType returnType;
    std::unique_ptr<BlockStmt> body;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitFunctionDecl(this);
    }
};

// Class Declaration
class ClassDecl : public Statement {
public:
    std::string name;
    std::string baseClass; // optional
    std::vector<std::unique_ptr<VarDeclStmt>> fields;
    std::vector<std::unique_ptr<FunctionDecl>> methods;
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitClassDecl(this);
    }
};

} // namespace Sad::AST
```

---

### Visitor Pattern

```cpp
// include/parser/ast/ast_visitor.h
namespace Sad::AST {

class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Expressions
    virtual void visitBinaryExpr(BinaryExpr* expr) = 0;
    virtual void visitUnaryExpr(UnaryExpr* expr) = 0;
    virtual void visitLiteralExpr(LiteralExpr* expr) = 0;
    virtual void visitVariableExpr(VariableExpr* expr) = 0;
    virtual void visitCallExpr(CallExpr* expr) = 0;
    virtual void visitArrayExpr(ArrayExpr* expr) = 0;
    virtual void visitIndexExpr(IndexExpr* expr) = 0;
    virtual void visitMemberExpr(MemberExpr* expr) = 0;
    virtual void visitLambdaExpr(LambdaExpr* expr) = 0;
    
    // Statements
    virtual void visitVarDeclStmt(VarDeclStmt* stmt) = 0;
    virtual void visitExprStmt(ExprStmt* stmt) = 0;
    virtual void visitIfStmt(IfStmt* stmt) = 0;
    virtual void visitWhileStmt(WhileStmt* stmt) = 0;
    virtual void visitForStmt(ForStmt* stmt) = 0;
    virtual void visitReturnStmt(ReturnStmt* stmt) = 0;
    virtual void visitBlockStmt(BlockStmt* stmt) = 0;
    virtual void visitFunctionDecl(FunctionDecl* decl) = 0;
    virtual void visitClassDecl(ClassDecl* decl) = 0;
};

// Example: AST Printer
class ASTPrinter : public ASTVisitor {
private:
    int indent = 0;
    std::ostringstream output;
    
    void printIndent() {
        for (int i = 0; i < indent; ++i) output << "  ";
    }
    
public:
    void visitBinaryExpr(BinaryExpr* expr) override {
        printIndent();
        output << "BinaryExpr(" << tokenTypeToString(expr->op) << ")\n";
        
        indent++;
        expr->left->accept(*this);
        expr->right->accept(*this);
        indent--;
    }
    
    void visitLiteralExpr(LiteralExpr* expr) override {
        printIndent();
        output << "Literal(" << expr->token.lexeme << ")\n";
    }
    
    std::string getOutput() const { return output.str(); }
};

} // namespace Sad::AST
```

---

## 📁 الملفات المطلوبة / Required Files

```
include/parser/
├── parser_core.h                ✅ UPDATE - إضافة methods
├── ast/
│   ├── ast_node.h              🆕 NEW - Base classes
│   ├── expressions.h           🆕 NEW - Expression nodes
│   ├── statements.h            🆕 NEW - Statement nodes
│   ├── declarations.h          🆕 NEW - Declaration nodes
│   ├── ast_visitor.h           🆕 NEW - Visitor interface
│   ├── ast_printer.h           🆕 NEW - Pretty printer
│   └── ast_utils.h             🆕 NEW - Utility functions

src/parser/
├── parser_core.cpp              ✅ UPDATE
├── parser_expressions.cpp       🆕 NEW - Expression parsing
├── parser_statements.cpp        🆕 NEW - Statement parsing
├── parser_declarations.cpp      🆕 NEW - Declaration parsing
├── parser_error_recovery.cpp    🆕 NEW - Error handling
├── ast/
│   ├── ast_node.cpp            🆕 NEW
│   ├── expressions.cpp         🆕 NEW
│   ├── statements.cpp          🆕 NEW
│   ├── declarations.cpp        🆕 NEW
│   ├── ast_printer.cpp         🆕 NEW
│   └── ast_utils.cpp           🆕 NEW

tests/parser_tests/
├── parser_basic_test.cpp        🆕 NEW
├── expressions_test.cpp         🆕 NEW
├── statements_test.cpp          🆕 NEW
├── declarations_test.cpp        🆕 NEW
├── precedence_test.cpp          🆕 NEW
├── error_recovery_test.cpp      🆕 NEW
└── integration_test.cpp         🆕 NEW
```

---

## 🔧 تقسيم العمل / Task Breakdown

### Week 1: AST Foundation

#### Task 1.1: AST Node Classes (يوم 1-3)
**الملفات:**
- `include/parser/ast/ast_node.h`
- `include/parser/ast/expressions.h`
- `include/parser/ast/statements.h`

**الخطوات:**
```cpp
// 1. Define base ASTNode
class ASTNode {
public:
    Position position;
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
};

// 2. Define Expression hierarchy (8 classes)
class Expression : public ASTNode { ... };
class BinaryExpr : public Expression { ... };
class UnaryExpr : public Expression { ... };
// ... (see Architecture section)

// 3. Define Statement hierarchy (9 classes)
class Statement : public ASTNode { ... };
class VarDeclStmt : public Statement { ... };
// ...

// 4. Add smart pointer support
using ExprPtr = std::unique_ptr<Expression>;
using StmtPtr = std::unique_ptr<Statement>;
```

**الاختبارات:**
```cpp
TEST(ASTNodeTest, BinaryExprCreation) {
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::INTEGER_LITERAL, "2", Position{}));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::INTEGER_LITERAL, "3", Position{}));
    
    BinaryExpr expr(std::move(left), TokenType::PLUS, std::move(right));
    
    EXPECT_EQ(expr.op, TokenType::PLUS);
    EXPECT_NE(expr.left, nullptr);
    EXPECT_NE(expr.right, nullptr);
}
```

**معايير القبول:**
- ✅ All AST node classes defined
- ✅ Smart pointers work correctly
- ✅ accept() methods compile
- ✅ Tests pass (≥15 tests)

---

#### Task 1.2: Visitor Pattern (يوم 4-5)
**الملفات:**
- `include/parser/ast/ast_visitor.h`
- `include/parser/ast/ast_printer.h`
- `src/parser/ast/ast_printer.cpp`

```cpp
// ASTPrinter implementation
class ASTPrinter : public ASTVisitor {
public:
    void visitBinaryExpr(BinaryExpr* expr) override {
        output << "(";
        expr->left->accept(*this);
        output << " " << tokenTypeToString(expr->op) << " ";
        expr->right->accept(*this);
        output << ")";
    }
    
    void visitLiteralExpr(LiteralExpr* expr) override {
        output << expr->token.lexeme;
    }
    
    // ... implement all visit methods
};

// Usage:
auto expr = parseBinaryExpression("2 + 3 * 4");
ASTPrinter printer;
expr->accept(printer);
std::cout << printer.getOutput(); // (2 + (3 * 4))
```

**الاختبارات:**
```cpp
TEST(VisitorTest, ASTPrinter) {
    // Build: 2 + 3
    auto left = std::make_unique<LiteralExpr>(Token(TokenType::INTEGER_LITERAL, "2", Position{}));
    auto right = std::make_unique<LiteralExpr>(Token(TokenType::INTEGER_LITERAL, "3", Position{}));
    auto expr = std::make_unique<BinaryExpr>(std::move(left), TokenType::PLUS, std::move(right));
    
    ASTPrinter printer;
    expr->accept(printer);
    
    EXPECT_EQ(printer.getOutput(), "(2 + 3)");
}
```

---

### Week 2: Parser Core

#### Task 2.1: Recursive Descent Parser (يوم 6-10)
**الملفات:**
- `include/parser/parser_core.h`
- `src/parser/parser_core.cpp`
- `src/parser/parser_expressions.cpp`

```cpp
// include/parser/parser_core.h
class ParserCore {
private:
    std::vector<Token> tokens;
    size_t current = 0;
    std::vector<std::string> errors;
    
    // Helpers
    Token peek() const;
    Token advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& message);
    
    // Expressions (by precedence)
    ExprPtr expression();
    ExprPtr assignment();
    ExprPtr logicalOr();
    ExprPtr logicalAnd();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr power();
    ExprPtr postfix();
    ExprPtr primary();
    
    // Statements
    StmtPtr statement();
    StmtPtr declaration();
    StmtPtr varDeclaration();
    StmtPtr ifStatement();
    StmtPtr whileStatement();
    StmtPtr forStatement();
    StmtPtr returnStatement();
    StmtPtr blockStatement();
    
public:
    ParserCore(const std::vector<Token>& tokens) : tokens(tokens) {}
    
    std::unique_ptr<ASTNode> parse();
    std::vector<std::string> getErrors() const { return errors; }
};

// src/parser/parser_expressions.cpp
ExprPtr ParserCore::expression() {
    DEBUG_PRINT("Parsing expression at token: %s", peek().lexeme.c_str());
    return assignment();
}

ExprPtr ParserCore::assignment() {
    ExprPtr expr = logicalOr();
    
    if (match(TokenType::ASSIGN)) {
        Token equals = previous();
        ExprPtr value = assignment();
        
        // Check if expr is assignable (variable, index, member)
        if (auto* var = dynamic_cast<VariableExpr*>(expr.get())) {
            return std::make_unique<AssignExpr>(var->name, std::move(value));
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
        expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, std::move(right));
    }
    
    return expr;
}

ExprPtr ParserCore::term() {
    ExprPtr expr = factor();
    
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = previous();
        ExprPtr right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op.type, std::move(right));
        
        DEBUG_PRINT("Parsed binary expr: %s", expr->toString().c_str());
    }
    
    return expr;
}

ExprPtr ParserCore::primary() {
    // Literals
    if (match(TokenType::INTEGER_LITERAL) || 
        match(TokenType::FLOAT_LITERAL) ||
        match(TokenType::STRING_LITERAL)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    // Keywords
    if (match(TokenType::KEYWORD_TRUE) || match(TokenType::KEYWORD_FALSE)) {
        return std::make_unique<LiteralExpr>(previous());
    }
    
    // Identifiers
    if (match(TokenType::IDENTIFIER)) {
        return std::make_unique<VariableExpr>(previous().lexeme);
    }
    
    // Grouping
    if (match(TokenType::LEFT_PAREN)) {
        ExprPtr expr = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return expr;
    }
    
    // Array literal
    if (match(TokenType::LEFT_BRACKET)) {
        std::vector<ExprPtr> elements;
        if (!check(TokenType::RIGHT_BRACKET)) {
            do {
                elements.push_back(expression());
            } while (match(TokenType::COMMA));
        }
        consume(TokenType::RIGHT_BRACKET, "Expected ']' after array elements");
        return std::make_unique<ArrayExpr>(std::move(elements));
    }
    
    error(peek(), "Expected expression");
    return nullptr;
}
```

**الاختبارات:**
```cpp
TEST(ParserTest, SimpleBinary) {
    std::vector<Token> tokens = {
        Token(TokenType::INTEGER_LITERAL, "2", Position{}),
        Token(TokenType::PLUS, "+", Position{}),
        Token(TokenType::INTEGER_LITERAL, "3", Position{}),
        Token(TokenType::END_OF_FILE, "", Position{})
    };
    
    ParserCore parser(tokens);
    auto ast = parser.parse();
    
    ASSERT_NE(ast, nullptr);
    EXPECT_TRUE(dynamic_cast<BinaryExpr*>(ast.get()) != nullptr);
}

TEST(ParserTest, Precedence) {
    // 2 + 3 * 4 should parse as 2 + (3 * 4)
    LexerCore lexer("2 + 3 * 4");
    auto tokens = lexer.tokenize();
    
    ParserCore parser(tokens);
    auto ast = parser.parse();
    
    ASTPrinter printer;
    ast->accept(printer);
    
    EXPECT_EQ(printer.getOutput(), "(2 + (3 * 4))");
}

TEST(ParserTest, ArrayLiteral) {
    LexerCore lexer("[1, 2, 3]");
    auto tokens = lexer.tokenize();
    
    ParserCore parser(tokens);
    auto ast = parser.parse();
    
    auto* arrayExpr = dynamic_cast<ArrayExpr*>(ast.get());
    ASSERT_NE(arrayExpr, nullptr);
    EXPECT_EQ(arrayExpr->elements.size(), 3);
}
```

---

### Week 3: Statements & Declarations

#### Task 3.1: Statement Parsing (يوم 11-13)
```cpp
StmtPtr ParserCore::statement() {
    if (match(TokenType::KEYWORD_IF)) return ifStatement();
    if (match(TokenType::KEYWORD_WHILE)) return whileStatement();
    if (match(TokenType::KEYWORD_FOR)) return forStatement();
    if (match(TokenType::KEYWORD_RETURN)) return returnStatement();
    if (match(TokenType::LEFT_BRACE)) return blockStatement();
    
    return expressionStatement();
}

StmtPtr ParserCore::ifStatement() {
    ExprPtr condition = expression();
    StmtPtr thenBranch = statement();
    StmtPtr elseBranch = nullptr;
    
    if (match(TokenType::KEYWORD_ELSE)) {
        elseBranch = statement();
    }
    
    return std::make_unique<IfStmt>(
        std::move(condition), 
        std::move(thenBranch), 
        std::move(elseBranch)
    );
}

StmtPtr ParserCore::varDeclaration() {
    Token typeToken = advance();
    DataType type = tokenToDataType(typeToken);
    
    Token name = consume(TokenType::IDENTIFIER, "Expected variable name");
    
    ExprPtr initializer = nullptr;
    if (match(TokenType::ASSIGN)) {
        initializer = expression();
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
    
    return std::make_unique<VarDeclStmt>(type, name.lexeme, std::move(initializer));
}
```

**الاختبارات:**
```cpp
TEST(ParserTest, IfStatement) {
    LexerCore lexer("إذا x > 10 { اطبع(x) }");
    ParserCore parser(lexer.tokenize());
    auto ast = parser.parse();
    
    auto* ifStmt = dynamic_cast<IfStmt*>(ast.get());
    ASSERT_NE(ifStmt, nullptr);
    EXPECT_NE(ifStmt->condition, nullptr);
    EXPECT_NE(ifStmt->thenBranch, nullptr);
}

TEST(ParserTest, VarDeclaration) {
    LexerCore lexer("رقم عمر = 25;");
    ParserCore parser(lexer.tokenize());
    auto ast = parser.parse();
    
    auto* varDecl = dynamic_cast<VarDeclStmt*>(ast.get());
    ASSERT_NE(varDecl, nullptr);
    EXPECT_EQ(varDecl->name, "عمر");
    EXPECT_EQ(varDecl->type, DataType::INTEGER);
    EXPECT_NE(varDecl->initializer, nullptr);
}
```

---

### Week 4: Error Handling & Integration

#### Task 4.1: Error Recovery (يوم 14-16)
```cpp
void ParserCore::error(const Token& token, const std::string& message) {
    std::string errorMsg = "[Parser Error] " + 
                          token.position.toString() + ": " + 
                          message + "\n" +
                          "  at '" + token.lexeme + "'";
    
    errors.push_back(errorMsg);
    ERROR_PRINT("%s", errorMsg.c_str());
}

void ParserCore::synchronize() {
    advance();
    
    while (!isAtEnd()) {
        if (previous().type == TokenType::SEMICOLON) return;
        
        switch (peek().type) {
            case TokenType::KEYWORD_IF:
            case TokenType::KEYWORD_WHILE:
            case TokenType::KEYWORD_FOR:
            case TokenType::KEYWORD_FUNCTION:
            case TokenType::KEYWORD_CLASS:
            case TokenType::KEYWORD_RETURN:
                return;
            default:
                advance();
        }
    }
}
```

---

## 🧪 الاختبارات / Tests Summary

### Unit Tests (40+ tests)
- Basic expressions (10 tests)
- Precedence (8 tests)
- Statements (12 tests)
- Declarations (10 tests)

### Integration Tests
```cpp
TEST(ParserIntegration, CompleteProgram) {
    std::string program = R"(
        رقم عمر = 25
        نص اسم = "أحمد"
        
        دالة تحقق_عمر(رقم ع) -> منطقي {
            إذا ع >= 18 {
                إرجاع صحيح
            } وإلا {
                إرجاع خطأ
            }
        }
        
        منطقي بالغ = تحقق_عمر(عمر)
    )";
    
    LexerCore lexer(program);
    ParserCore parser(lexer.tokenize());
    auto ast = parser.parse();
    
    EXPECT_TRUE(parser.getErrors().empty());
    EXPECT_NE(ast, nullptr);
}
```

---

## ⏱️ الجدول الزمني / Timeline

| الأسبوع | المهام | الساعات |
|---------|--------|---------|
| Week 1 | AST Nodes + Visitor | 40 ساعة |
| Week 2 | Expression Parsing | 40 ساعة |
| Week 3 | Statements + Declarations | 40 ساعة |
| Week 4 | Error Handling + Integration | 40 ساعة |
| **Total** | **4 weeks** | **160 ساعة** |

---

## 🚀 أوامر PowerShell / PowerShell Commands

```powershell
# إنشاء فرع العمل
git checkout -b feat/parser/ast-implementation

# بناء واختبار
cd build
cmake --build . --config Debug
ctest -R parser_tests --output-on-failure -C Debug

# Commit
git add include/parser/* src/parser/* tests/parser_tests/*
git commit -m "[parser] Implemented recursive descent parser with AST

- Added AST node hierarchy (18 classes)
- Implemented Visitor pattern
- Recursive descent parser with precedence
- Error recovery with bilingual diagnostics
- 50+ unit tests with ≥85% coverage"

git push --set-upstream origin feat/parser/ast-implementation
```

---

## ⚠️ المخاطر / Risks

| الخطر | التخفيف |
|-------|---------|
| Precedence bugs | جدول اختبارات شامل |
| Memory leaks | استخدام unique_ptr |
| Error cascades | Synchronization points |

---

**آخر تحديث:** 4 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ
