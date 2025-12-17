# المرحلة 1: Frontend
# Phase 1: Frontend Development

**المدة / Duration:** 4-6 أسابيع / weeks  
**الأولوية / Priority:** P0 (حرجة / Critical)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### (AR) العربية

تطوير Frontend محسّن يشمل:
- Lexer محسّن مع دعم Unicode كامل
- Parser قوي يبني AST كامل
- Type Checker متقدم مع type inference
- Semantic Analyzer شامل
- Symbol Table Management
- Error reporting واضح ومفيد

### (EN) English

Develop enhanced Frontend including:
- Improved Lexer with full Unicode support
- Robust Parser building complete AST
- Advanced Type Checker with inference
- Comprehensive Semantic Analyzer
- Symbol Table Management
- Clear and helpful error reporting

---

## 🎯 الأهداف / Goals

### (AR) العربية

✅ **الأهداف الرئيسية:**
1. Lexer سريع ودقيق (دعم عربي + إنجليزي)
2. Parser يبني AST صحيح 100%
3. Type system قوي يدعم generics
4. Semantic analysis شامل
5. Error messages مفيدة مع source locations
6. Performance: < 10ms لكل 1000 LOC

---

## 📝 المهام التفصيلية / Detailed Tasks

### 1. Lexer Enhancement (أسبوع واحد)

#### (AR) العربية

**الميزات المطلوبة:**

```cpp
// compiler/frontend/lexer.h
class Lexer {
private:
    نص source
    رقم position
    رقم line
    رقم column
    
public:
    دالة tokenize(نص source) -> مصفوفة<Token>
    
    # دعم Unicode
    دالة is_arabic_letter(رقم ch) -> منطقي
    دالة is_english_letter(رقم ch) -> منطقي
    
    # معالجة الأرقام
    دالة scan_number() -> Token
    دالة scan_binary() -> Token      # 0b1010
    دالة scan_hex() -> Token         # 0xABCD
    
    # معالجة النصوص
    دالة scan_string() -> Token
    دالة scan_interpolated() -> Token  # ن"اسمي {name}"
    
    # معالجة التعليقات
    دالة skip_line_comment() -> فارغ     # #
    دالة skip_block_comment() -> فارغ    # #* *#
    
    # معالجة الكلمات المفتاحية
    دالة is_keyword(نص word) -> منطقي
نهاية
```

**Token Types:**
```cpp
enum TokenType {
    # Keywords
    KW_دالة, KW_صنف, KW_إذا, KW_بينما, KW_لكل,
    KW_إرجاع, KW_نهاية, KW_وإلا, KW_function, KW_class,
    
    # Types
    KW_رقم, KW_نص, KW_منطقي, KW_مصفوفة, KW_قاموس,
    KW_مؤشر, KW_number, KW_string, KW_boolean,
    
    # Literals
    LIT_NUMBER,    # 123, 3.14
    LIT_STRING,    # "text"
    LIT_TRUE,      # صحيح, true
    LIT_FALSE,     # خطأ, false
    LIT_NULL,      # فارغ, null
    
    # Identifiers
    IDENTIFIER,    # variable_name, متغير
    
    # Operators
    OP_PLUS,       # +
    OP_MINUS,      # -
    OP_STAR,       # *
    OP_SLASH,      # /
    OP_PERCENT,    # %
    OP_AND,        # &
    OP_OR,         # |
    OP_XOR,        # ^
    OP_NOT,        # ~
    OP_SHL,        # <<
    OP_SHR,        # >>
    OP_EQ,         # ==
    OP_NE,         # !=
    OP_LT,         # <
    OP_LE,         # <=
    OP_GT,         # >
    OP_GE,         # >=
    OP_ASSIGN,     # =
    
    # Delimiters
    LPAREN,        # (
    RPAREN,        # )
    LBRACKET,      # [
    RBRACKET,      # ]
    COMMA,         # ،، ,
    DOT,           # .
    ARROW,         # ->
    
    # Special
    NEWLINE,
    EOF,
    ERROR
}
```

**المهام:**
- [ ] تطبيق Token structure
- [ ] تطبيق Lexer class
- [ ] دعم Unicode (UTF-8)
- [ ] معالجة جميع token types
- [ ] اختبارات Lexer (50+ tests)
- [ ] Performance testing

---

### 2. Parser Development (أسبوعان)

#### (AR) العربية

**AST Structure:**

```cpp
// compiler/frontend/ast.h

# Base node
class ASTNode {
    نص type
    SourceLocation location
نهاية

# Program
class Program : ASTNode {
    مصفوفة<مؤشر<Statement>> statements
نهاية

# Declarations
class FunctionDecl : ASTNode {
    نص name
    مصفوفة<Parameter> parameters
    Type return_type
    مؤشر<BlockStatement> body
نهاية

class ClassDecl : ASTNode {
    نص name
    مصفوفة<نص> type_parameters    # Generics
    مصفوفة<مؤشر<FunctionDecl>> methods
    مصفوفة<مؤشر<VariableDecl>> fields
نهاية

class VariableDecl : ASTNode {
    نص name
    Type type
    مؤشر<Expression> initializer
نهاية

# Statements
class IfStatement : ASTNode {
    مؤشر<Expression> condition
    مؤشر<BlockStatement> then_block
    مؤشر<BlockStatement> else_block
نهاية

class WhileLoop : ASTNode {
    مؤشر<Expression> condition
    مؤشر<BlockStatement> body
نهاية

class ForLoop : ASTNode {
    نص variable
    مؤشر<Expression> iterable
    مؤشر<BlockStatement> body
نهاية

class ReturnStatement : ASTNode {
    مؤشر<Expression> value
نهاية

# Expressions
class BinaryExpression : ASTNode {
    نص operator
    مؤشر<Expression> left
    مؤشر<Expression> right
نهاية

class CallExpression : ASTNode {
    مؤشر<Expression> callee
    مصفوفة<مؤشر<Expression>> arguments
نهاية

class IndexExpression : ASTNode {
    مؤشر<Expression> object
    مؤشر<Expression> index
نهاية

class MemberExpression : ASTNode {
    مؤشر<Expression> object
    نص member
نهاية
```

**Parser Class:**
```cpp
// compiler/frontend/parser.h
class Parser {
private:
    مصفوفة<Token> tokens
    رقم current
    
public:
    دالة parse() -> مؤشر<Program>
    
    # Top-level
    دالة parse_declaration() -> مؤشر<Declaration>
    دالة parse_function() -> مؤشر<FunctionDecl>
    دالة parse_class() -> مؤشر<ClassDecl>
    
    # Statements
    دالة parse_statement() -> مؤشر<Statement>
    دالة parse_if() -> مؤشر<IfStatement>
    دالة parse_while() -> مؤشر<WhileLoop>
    دالة parse_for() -> مؤشر<ForLoop>
    دالة parse_return() -> مؤشر<ReturnStatement>
    
    # Expressions (Pratt parsing)
    دالة parse_expression(رقم precedence) -> مؤشر<Expression>
    دالة parse_primary() -> مؤشر<Expression>
    دالة parse_binary(مؤشر<Expression> left, رقم prec) -> مؤشر<Expression>
    دالة parse_call(مؤشر<Expression> callee) -> مؤشر<Expression>
    
    # Helpers
    دالة expect(TokenType type) -> Token
    دالة match(TokenType type) -> منطقي
    دالة error(نص message) -> فارغ
نهاية
```

**المهام:**
- [ ] تطبيق جميع AST nodes
- [ ] تطبيق Parser مع Pratt parsing
- [ ] معالجة الأخطاء النحوية
- [ ] Source location tracking
- [ ] اختبارات Parser (100+ tests)
- [ ] AST printer للتصحيح

---

### 3. Type Checker (أسبوع واحد)

#### (AR) العربية

**Type System:**

```cpp
// compiler/frontend/types.h

# Base type
class Type {
    نص name
    
    دالة is_compatible(مؤشر<Type> other) -> منطقي
    دالة to_string() -> نص
نهاية

# Primitive types
class PrimitiveType : Type {
    # رقم، نص، منطقي
نهاية

# Compound types
class ArrayType : Type {
    مؤشر<Type> element_type
نهاية

class MapType : Type {
    مؤشر<Type> key_type
    مؤشر<Type> value_type
نهاية

class PointerType : Type {
    مؤشر<Type> base_type
نهاية

class FunctionType : Type {
    مصفوفة<مؤشر<Type>> parameter_types
    مؤشر<Type> return_type
نهاية

# Generics
class GenericType : Type {
    نص type_parameter
    مصفوفة<مؤشر<Type>> constraints
نهاية
```

**Type Checker:**
```cpp
// compiler/frontend/type_checker.h
class TypeChecker {
private:
    مؤشر<SymbolTable> symbols
    مصفوفة<Error> errors
    
public:
    دالة check(مؤشر<ASTNode> ast) -> منطقي
    
    # Type inference
    دالة infer_type(مؤشر<Expression> expr) -> مؤشر<Type>
    
    # Type checking
    دالة check_function(مؤشر<FunctionDecl> func) -> فارغ
    دالة check_statement(مؤشر<Statement> stmt) -> فارغ
    دالة check_expression(مؤشر<Expression> expr) -> مؤشر<Type>
    
    # Type compatibility
    دالة is_assignable(مؤشر<Type> from, مؤشر<Type> to) -> منطقي
    دالة unify_types(مؤشر<Type> a, مؤشر<Type> b) -> مؤشر<Type>
    
    # Generics
    دالة instantiate_generic(مؤشر<GenericType> generic,
                             مصفوفة<مؤشر<Type>> args) -> مؤشر<Type>
نهاية
```

**المهام:**
- [ ] تطبيق Type classes
- [ ] تطبيق Type Checker
- [ ] Type inference algorithm
- [ ] Generics support
- [ ] اختبارات Type checking (80+ tests)

---

### 4. Semantic Analyzer (أسبوع واحد)

#### (AR) العربية

**Symbol Table:**

```cpp
// compiler/frontend/symbol_table.h
class Symbol {
    نص name
    مؤشر<Type> type
    SymbolKind kind  # variable, function, class
    مؤشر<ASTNode> declaration
نهاية

class Scope {
    مؤشر<Scope> parent
    قاموس<نص، مؤشر<Symbol>> symbols
    
    دالة define(نص name, مؤشر<Symbol> symbol) -> فارغ
    دالة lookup(نص name) -> مؤشر<Symbol>
    دالة lookup_local(نص name) -> مؤشر<Symbol>
نهاية

class SymbolTable {
    مؤشر<Scope> global_scope
    مؤشر<Scope> current_scope
    
    دالة enter_scope() -> فارغ
    دالة exit_scope() -> فارغ
    دالة define(نص name, مؤشر<Symbol> symbol) -> فارغ
    دالة resolve(نص name) -> مؤشر<Symbol>
نهاية
```

**Semantic Analyzer:**
```cpp
// compiler/frontend/semantic.h
class SemanticAnalyzer {
private:
    مؤشر<SymbolTable> symbols
    مصفوفة<Error> errors
    
public:
    دالة analyze(مؤشر<Program> program) -> منطقي
    
    # Analysis passes
    دالة check_declarations() -> فارغ
    دالة check_scopes() -> فارغ
    دالة check_initialization() -> فارغ
    دالة check_reachability() -> فارغ
    دالة check_unused() -> فارغ
    
    # Specific checks
    دالة check_function_returns(مؤشر<FunctionDecl> func) -> منطقي
    دالة check_variable_usage(مؤشر<VariableDecl> var) -> فارغ
    دالة check_dead_code(مؤشر<BlockStatement> block) -> فارغ
نهاية
```

**المهام:**
- [ ] تطبيق Symbol Table
- [ ] تطبيق Semantic Analyzer
- [ ] Scope management
- [ ] Variable initialization checking
- [ ] Dead code detection
- [ ] اختبارات Semantic (60+ tests)

---

### 5. Error Reporting (متوازي مع المهام السابقة)

#### (AR) العربية

**Error System:**

```cpp
// compiler/frontend/errors.h
class SourceLocation {
    نص filename
    رقم line
    رقم column
    رقم length
نهاية

class Error {
    ErrorLevel level      # error, warning, info
    ErrorCode code
    نص message
    SourceLocation location
    مصفوفة<نص> suggestions
نهاية

class ErrorReporter {
    مصفوفة<Error> errors
    
    دالة report_error(ErrorCode code, SourceLocation loc, نص msg) -> فارغ
    دالة report_warning(ErrorCode code, SourceLocation loc, نص msg) -> فارغ
    دالة print_errors() -> فارغ
    دالة has_errors() -> منطقي
نهاية
```

**Error Messages (أمثلة):**
```
error: undefined variable 'x'
  --> test.s:5:10
   |
 5 |     اطبع(x)
   |          ^ undefined variable
   |
   = help: did you mean 'y'?

error: type mismatch in assignment
  --> test.s:10:5
   |
10 |     x = "text"
   |     ^^^^^^^^^^ expected 'رقم', found 'نص'
```

**المهام:**
- [ ] تطبيق Error classes
- [ ] تطبيق ErrorReporter
- [ ] Source location tracking
- [ ] Colored output
- [ ] Suggestions system
- [ ] Multi-language messages (AR/EN)

---

## 📊 معايير النجاح / Success Criteria

### (AR) العربية

✅ **المرحلة مكتملة عندما:**
- [ ] Lexer يعالج جميع tokens بنجاح
- [ ] Parser يبني AST صحيح لجميع الاختبارات
- [ ] Type Checker يكتشف جميع أخطاء الأنواع
- [ ] Semantic Analyzer يكتشف جميع الأخطاء الدلالية
- [ ] Error messages واضحة ومفيدة
- [ ] Performance: < 10ms لكل 1000 LOC
- [ ] Test coverage > 85%
- [ ] جميع Stage 1 tests تمر

---

## 🧪 الاختبارات / Tests

### (AR) العربية

**Lexer Tests (50+):**
- tokenization صحيح
- Unicode handling
- Error recovery
- Performance

**Parser Tests (100+):**
- جميع constructs
- Error recovery
- Source locations
- Edge cases

**Type Checker Tests (80+):**
- Primitive types
- Compound types
- Generics
- Type inference
- Error detection

**Semantic Tests (60+):**
- Scope resolution
- Initialization
- Reachability
- Dead code

---

**السابق / Previous:** [المرحلة 0: التحضير](03_phase0_preparation.md)  
**التالي / Next:** [المرحلة 2: Middle-end](05_phase2_middleend.md)
