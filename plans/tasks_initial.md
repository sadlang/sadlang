# 📋 المهام التفصيلية - Detailed Tasks Breakdown

**التاريخ:** 11 نوفمبر 2025  
**الإصدار:** 1.0  
**الفرع:** `feat/implement/continue`  
**المرحلة الحالية:** Phase 3 - Parser & AST

---

## 📚 فهرس المهام / Task Index

- [A. Documentation Tasks](#a-documentation-tasks) - مهام التوثيق
- [B. Parser Core Tasks](#b-parser-core-tasks) - مهام المحلل النحوي
- [C. AST Tasks](#c-ast-tasks) - مهام شجرة الصيغة المجردة
- [D. Testing Tasks](#d-testing-tasks) - مهام الاختبار
- [E. Build & Integration](#e-build--integration) - مهام البناء والتكامل
- [F. Git & Version Control](#f-git--version-control) - مهام Git

---

## A. Documentation Tasks

### A.1 ✅ COMPLETED - استخراج قواعد النحو
**الحالة:** ✅ مكتمل  
**الملف:** `plans/extracted_rules.md`  
**الوصف:** استخراج جميع قواعد EBNF من 11 ملف في `docs/language_spec/rules/`

**التفاصيل:**
- ✅ قراءة `merged_grammar.md`
- ✅ قراءة ملفات القواعد الفردية (01-10)
- ✅ استخراج القواعد حسب الفئات:
  - Data Types (Integer, Float, Bool, String, Array, Map, Tuple)
  - Functions (declaration, lambda, closures, decorators)
  - OOP (class, inheritance, visibility, static)
  - Expressions & Operators (13 precedence levels)
  - Statements (if, while, for, return, break, continue)
  - Python Features (comprehensions, generators, context managers, exceptions)
  - C++ Features (types, generics, operator overloading)
  - Graphics, Modules, Comments
- ✅ إنشاء جدول أولوية العوامل
- ✅ توثيق الميزات الخاصة (optional parentheses, dual punctuation)
- ✅ إضافة أمثلة لكل فئة

**الإنجاز:** 100%  
**التاريخ:** 11 Nov 2025

---

### A.2 ✅ COMPLETED - تقرير تقدم التنفيذ
**الحالة:** ✅ مكتمل  
**الملف:** `plans/implementation_plan_started.md`  
**الوصف:** ملخص شامل لحالة المشروع

**التفاصيل:**
- ✅ نظرة عامة على المراحل (Phase 0-8)
- ✅ تفاصيل Phase 0 (Planning): 30 ملف، 18,000+ سطر
- ✅ تفاصيل Phase 1 (Infrastructure): 15 ملف، 2,000+ سطر، 50+ مجلد
- ✅ تفاصيل Phase 2 (Lexer): 6 ملفات، 1,991 سطر، 90+ token types، 45 keywords
- ✅ تفاصيل Phase 3 (Parser): بدء التنفيذ، قائمة بـ 27 ملف مطلوب
- ✅ المشاكل المحلولة والتحديات الحالية
- ✅ المقاييس والإحصائيات
- ✅ الخطوات التالية المباشرة
- ✅ معايير القبول

**الإنجاز:** 100%  
**التاريخ:** 11 Nov 2025

---

### A.3 🔄 IN PROGRESS - تفصيل المهام
**الحالة:** 🔄 جارٍ  
**الملف:** `plans/tasks_initial.md`  
**الوصف:** هذا الملف - تقسيم المهام التفصيلي

**التفاصيل:**
- 🔄 تصنيف المهام حسب المكونات (A-F)
- 🔄 تقدير الوقت لكل مهمة
- 🔄 تحديد الأولويات (P0, P1, P2)
- 🔄 تحديد التبعيات بين المهام
- 🔄 معايير القبول لكل مهمة
- 🔄 قائمة التسليمات (deliverables)

**الإنجاز:** 80%  
**الوقت المتبقي:** 1 hour

---

### A.4 ⏳ TODO - حالة المرحلة
**الحالة:** ⏳ قادم  
**الملف:** `plans/status/state_0002.md`  
**الوصف:** لقطة لحالة المشروع عند بدء Phase 3

**التفاصيل:**
- ⏳ معلومات الفرع Git
- ⏳ الملفات المكتملة في Phase 2
- ⏳ الملفات الجديدة في هذه الجلسة
- ⏳ النقاط البارزة (Highlights)
- ⏳ التحديات المواجهة
- ⏳ الخطوات التالية
- ⏳ التوقيع

**التقدير:** 2 hours  
**الأولوية:** P1  
**التبعيات:** A.1, A.2, A.3

---

## B. Parser Core Tasks

### B.1 ⏳ TODO - Parser Core Header
**الحالة:** ⏳ قادم  
**الملف:** `include/parser/parser_core.h`  
**الوصف:** تعريف فئة ParserCore الأساسية

**التفاصيل:**
```cpp
class ParserCore {
private:
    std::vector<Token> tokens;
    size_t current = 0;
    std::vector<std::string> errors;
    
    // Navigation
    Token peek() const;
    Token peekNext() const;
    Token advance();
    Token previous() const;
    bool match(TokenType type);
    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& message);
    bool isAtEnd() const;
    
    // Helpers
    void error(const Token& token, const std::string& message);
    void synchronize();
    
    // Expressions (by precedence - 13 levels)
    ExprPtr expression();        // Level 0
    ExprPtr assignment();        // Level 1
    ExprPtr logicalOr();         // Level 2
    ExprPtr logicalAnd();        // Level 3
    ExprPtr bitwiseOr();         // Level 4
    ExprPtr bitwiseXor();        // Level 5
    ExprPtr bitwiseAnd();        // Level 6
    ExprPtr equality();          // Level 7
    ExprPtr comparison();        // Level 8
    ExprPtr bitShift();          // Level 9
    ExprPtr term();              // Level 10
    ExprPtr factor();            // Level 11
    ExprPtr unary();             // Level 12
    ExprPtr power();             // Level 13
    ExprPtr postfix();           // Level 14
    ExprPtr primary();           // Level 15
    
    // Statements
    StmtPtr statement();
    StmtPtr declaration();
    StmtPtr varDeclaration();
    StmtPtr functionDeclaration();
    StmtPtr classDeclaration();
    StmtPtr ifStatement();
    StmtPtr whileStatement();
    StmtPtr forStatement();
    StmtPtr returnStatement();
    StmtPtr breakStatement();
    StmtPtr continueStatement();
    StmtPtr blockStatement();
    StmtPtr expressionStatement();
    
public:
    ParserCore(const std::vector<Token>& tokens);
    std::vector<StmtPtr> parse();
    std::vector<std::string> getErrors() const;
    bool hadError() const;
};
```

**الإنجاز المطلوب:**
- ⏳ تعريف جميع الدوال الخاصة
- ⏳ تعريف جميع الدوال العامة
- ⏳ توثيق Doxygen كامل
- ⏳ أمثلة استخدام
- ⏳ ملاحظات التنفيذ

**التقدير:** 4 hours  
**الأولوية:** P0  
**التبعيات:** C.1 (AST base classes)

---

### B.2 ⏳ TODO - Parser Core Implementation
**الحالة:** ⏳ قادم  
**الملف:** `src/parser/parser_core.cpp`  
**الوصف:** تنفيذ دوال ParserCore الأساسية

**التفاصيل:**
- ⏳ Constructor
- ⏳ Navigation functions (peek, advance, etc.)
- ⏳ Helper functions (match, check, consume, error)
- ⏳ Main parse() function
- ⏳ DEBUG_PRINT في كل دالة
- ⏳ Error handling شامل

**الكود المتوقع:** ~400 lines

**الإنجاز المطلوب:**
- ⏳ جميع الدوال المساعدة تعمل
- ⏳ error() تولد رسائل ثنائية اللغة
- ⏳ synchronize() تستعيد Parser بشكل صحيح
- ⏳ parse() تعالج قائمة العبارات

**التقدير:** 6 hours  
**الأولوية:** P0  
**التبعيات:** B.1

---

### B.3 ⏳ TODO - Expression Parsing
**الحالة:** ⏳ قادم  
**الملف:** `src/parser/parser_expressions.cpp`  
**الوصف:** تنفيذ جميع مستويات تحليل التعابير

**التفاصيل:**
- ⏳ expression() → assignment()
- ⏳ assignment() - إسناد متغيرات
- ⏳ logicalOr() - OR منطقي `||`
- ⏳ logicalAnd() - AND منطقي `&&`
- ⏳ bitwiseOr() - OR بت `|`
- ⏳ bitwiseXor() - XOR بت `^`
- ⏳ bitwiseAnd() - AND بت `&`
- ⏳ equality() - مساواة `==`، `!=`
- ⏳ comparison() - مقارنة `<`، `>`، `<=`، `>=`
- ⏳ bitShift() - إزاحة بت `<<`، `>>`
- ⏳ term() - جمع وطرح `+`، `-`
- ⏳ factor() - ضرب وقسمة `*`، `/`، `%`
- ⏳ unary() - عوامل أحادية `!`، `-`، `+`، `~`
- ⏳ power() - أس `**`
- ⏳ postfix() - استدعاء، فهرسة، عضو
- ⏳ primary() - ثوابت، معرفات، أقواس، مصفوفات، قواميس، lambdas

**الكود المتوقع:** ~600 lines

**الإنجاز المطلوب:**
- ⏳ جميع 16 دالة تحليل تعمل بشكل صحيح
- ⏳ Operator precedence صحيح تماماً
- ⏳ Left/Right associativity صحيح
- ⏳ معالجة أخطاء كل مستوى

**التقدير:** 12 hours  
**الأولوية:** P0  
**التبعيات:** B.2, C.2 (Expression AST nodes)

---

### B.4 ⏳ TODO - Statement Parsing
**الحالة:** ⏳ قادم  
**الملف:** `src/parser/parser_statements.cpp`  
**الوصف:** تنفيذ تحليل جميع أنواع العبارات

**التفاصيل:**
- ⏳ statement() - dispatcher
- ⏳ ifStatement() - `إذا`، `وإلا إذا`، `وإلا`
- ⏳ whileStatement() - `بينما`
- ⏳ forStatement() - `لكل` (variant 1: foreach, variant 2: C-style)
- ⏳ returnStatement() - `إرجاع`
- ⏳ breakStatement() - `اخرج`
- ⏳ continueStatement() - `تابع`
- ⏳ blockStatement() - `{ ... نهاية }`
- ⏳ expressionStatement() - `expr;`

**الكود المتوقع:** ~400 lines

**الإنجاز المطلوب:**
- ⏳ جميع أنواع العبارات تُحلل
- ⏳ معالجة `نهاية` keyword
- ⏳ nested statements
- ⏳ optional semicolons

**التقدير:** 10 hours  
**الأولوية:** P0  
**التبعيات:** B.2, C.3 (Statement AST nodes)

---

### B.5 ⏳ TODO - Declaration Parsing
**الحالة:** ⏳ قادم  
**الملف:** `src/parser/parser_declarations.cpp`  
**الوصف:** تنفيذ تحليل التصريحات

**التفاصيل:**
- ⏳ declaration() - dispatcher
- ⏳ varDeclaration() - `رقم س = 10`
- ⏳ functionDeclaration() - `دالة اسم(...) { ... نهاية }`
  - Parameters (with types, defaults)
  - Return type
  - Function body
- ⏳ classDeclaration() - `صنف اسم { ... نهاية }`
  - Base class(es)
  - Fields (visibility, static)
  - Methods (visibility, static)
  - Constructor `باني`
  - Destructor `مدمر`

**الكود المتوقع:** ~500 lines

**الإنجاز المطلوب:**
- ⏳ Variable declarations مع/بدون تهيئة
- ⏳ Function declarations كاملة
- ⏳ Class declarations كاملة
- ⏳ Inheritance parsing
- ⏳ Visibility modifiers

**التقدير:** 12 hours  
**الأولوية:** P0  
**التبعيات:** B.2, C.4 (Declaration AST nodes)

---

### B.6 ⏳ TODO - Error Recovery
**الحالة:** ⏳ قادم  
**الملف:** `src/parser/parser_error_recovery.cpp`  
**الوصف:** معالجة الأخطاء واستعادة Parser

**التفاصيل:**
- ⏳ synchronize() - إيجاد نقطة آمنة للاستمرار
- ⏳ Synchronization points:
  - `;` (semicolon)
  - `نهاية` (end keyword)
  - `دالة`، `صنف` (function/class keywords)
  - `إذا`، `بينما`، `لكل` (control flow keywords)
- ⏳ Error reporting:
  - Bilingual messages (Arabic/English)
  - Position information (line:column)
  - Context snippet
  - Suggestions for common mistakes

**الكود المتوقع:** ~200 lines

**الإنجاز المطلوب:**
- ⏳ Parser لا يتوقف عند أول خطأ
- ⏳ Multiple errors reported
- ⏳ Clear error messages
- ⏳ Helpful suggestions

**التقدير:** 6 hours  
**الأولوية:** P1  
**التبعيات:** B.2

---

## C. AST Tasks

### C.1 ⏳ TODO - AST Base Classes
**الحالة:** ⏳ قادم  
**الملفات:**
- `include/parser/ast/ast_node.h`
- `src/parser/ast/ast_node.cpp`

**الوصف:** تعريف الفئات الأساسية لـ AST

**التفاصيل:**
```cpp
// include/parser/ast/ast_node.h
namespace Sad::AST {

class ASTNode {
public:
    Position position;
    
    virtual ~ASTNode() = default;
    virtual std::string toString() const = 0;
    virtual void accept(class ASTVisitor& visitor) = 0;
};

class Expression : public ASTNode {
public:
    virtual DataType getType() const = 0;
};

class Statement : public ASTNode {
};

// Smart pointer aliases
using ExprPtr = std::unique_ptr<Expression>;
using StmtPtr = std::unique_ptr<Statement>;

} // namespace Sad::AST
```

**الإنجاز المطلوب:**
- ⏳ Base classes محددة
- ⏳ Virtual functions declared
- ⏳ Smart pointer aliases
- ⏳ Documentation كاملة

**التقدير:** 3 hours  
**الأولوية:** P0  
**التبعيات:** لا يوجد

---

### C.2 ⏳ TODO - Expression AST Nodes
**الحالة:** ⏳ قادم  
**الملفات:**
- `include/parser/ast/expressions.h`
- `src/parser/ast/expressions.cpp`

**الوصف:** تعريف جميع عقد التعابير

**التفاصيل:**
- ⏳ `BinaryExpr` - عامل ثنائي (left op right)
- ⏳ `UnaryExpr` - عامل أحادي (op operand)
- ⏳ `LiteralExpr` - ثابت (42، "text"، true)
- ⏳ `VariableExpr` - متغير (identifier)
- ⏳ `AssignExpr` - إسناد (name = value)
- ⏳ `CallExpr` - استدعاء (callee(args))
- ⏳ `IndexExpr` - فهرسة (object[index])
- ⏳ `MemberExpr` - عضو (object.member)
- ⏳ `ArrayExpr` - مصفوفة ([1, 2, 3])
- ⏳ `MapExpr` - قاموس ({key: value})
- ⏳ `LambdaExpr` - lambda ((x) => x * 2)
- ⏳ `ListComprehensionExpr` - list comprehension
- ⏳ `DictComprehensionExpr` - dict comprehension
- ⏳ `GeneratorExpr` - generator expression

**الكود المتوقع:** ~600 lines

**الإنجاز المطلوب:**
- ⏳ جميع العقد معرّفة
- ⏳ Constructors صحيحة
- ⏳ accept() methods implemented
- ⏳ toString() methods implemented
- ⏳ getType() methods implemented

**التقدير:** 10 hours  
**الأولوية:** P0  
**التبعيات:** C.1

---

### C.3 ⏳ TODO - Statement AST Nodes
**الحالة:** ⏳ قادم  
**الملفات:**
- `include/parser/ast/statements.h`
- `src/parser/ast/statements.cpp`

**الوصف:** تعريف جميع عقد العبارات

**التفاصيل:**
- ⏳ `ExprStmt` - عبارة تعبير (expr;)
- ⏳ `VarDeclStmt` - تصريح متغير (type name = init)
- ⏳ `IfStmt` - if (condition, thenBranch, elseBranch)
- ⏳ `WhileStmt` - while (condition, body)
- ⏳ `ForStmt` - for (variable, iterable, body)
- ⏳ `ForRangeStmt` - for (init, condition, increment, body)
- ⏳ `ReturnStmt` - return (value)
- ⏳ `BreakStmt` - break
- ⏳ `ContinueStmt` - continue
- ⏳ `BlockStmt` - block ({statements})
- ⏳ `TryStmt` - try-except-finally
- ⏳ `RaiseStmt` - raise exception
- ⏳ `WithStmt` - with (context manager)

**الكود المتوقع:** ~500 lines

**الإنجاز المطلوب:**
- ⏳ جميع العقد معرّفة
- ⏳ Constructors صحيحة
- ⏳ accept() methods implemented
- ⏳ toString() methods implemented

**التقدير:** 8 hours  
**الأولوية:** P0  
**التبعيات:** C.1

---

### C.4 ⏳ TODO - Declaration AST Nodes
**الحالة:** ⏳ قادم  
**الملفات:**
- `include/parser/ast/declarations.h`
- `src/parser/ast/declarations.cpp`

**الوصف:** تعريف عقد التصريحات

**التفاصيل:**
- ⏳ `Parameter` - struct (name, type, defaultValue)
- ⏳ `FunctionDecl` - function (name, params, returnType, body)
- ⏳ `ClassDecl` - class (name, baseClass, fields, methods)
- ⏳ `FieldDecl` - field (visibility, type, name, initializer)
- ⏳ `MethodDecl` - method (visibility, function)
- ⏳ `ConstructorDecl` - constructor (params, initializers, body)
- ⏳ `DestructorDecl` - destructor (body)
- ⏳ `EnumDecl` - enum (name, values)
- ⏳ `ImportStmt` - import (module, alias, items)
- ⏳ `ExportStmt` - export (declaration)

**الكود المتوقع:** ~400 lines

**الإنجاز المطلوب:**
- ⏳ جميع العقد معرّفة
- ⏳ Nested structures handled
- ⏳ accept() methods implemented
- ⏳ toString() methods implemented

**التقدير:** 8 hours  
**الأولوية:** P0  
**التبعيات:** C.1

---

### C.5 ⏳ TODO - Visitor Pattern
**الحالة:** ⏳ قادم  
**الملفات:**
- `include/parser/ast/ast_visitor.h`
- `include/parser/ast/ast_printer.h`
- `src/parser/ast/ast_printer.cpp`

**الوصف:** تنفيذ Visitor Pattern لـ AST

**التفاصيل:**
```cpp
// ast_visitor.h
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Expressions
    virtual void visitBinaryExpr(BinaryExpr* expr) = 0;
    virtual void visitUnaryExpr(UnaryExpr* expr) = 0;
    virtual void visitLiteralExpr(LiteralExpr* expr) = 0;
    // ... (14 expression visit methods)
    
    // Statements
    virtual void visitExprStmt(ExprStmt* stmt) = 0;
    virtual void visitVarDeclStmt(VarDeclStmt* stmt) = 0;
    virtual void visitIfStmt(IfStmt* stmt) = 0;
    // ... (13 statement visit methods)
    
    // Declarations
    virtual void visitFunctionDecl(FunctionDecl* decl) = 0;
    virtual void visitClassDecl(ClassDecl* decl) = 0;
    // ... (10 declaration visit methods)
};

// ast_printer.h
class ASTPrinter : public ASTVisitor {
private:
    int indent = 0;
    std::ostringstream output;
    
    void printIndent();
    
public:
    // Implement all visit methods
    void visitBinaryExpr(BinaryExpr* expr) override;
    // ...
    
    std::string getOutput() const;
};
```

**الكود المتوقع:** ~500 lines

**الإنجاز المطلوب:**
- ⏳ ASTVisitor interface complete
- ⏳ ASTPrinter implementation complete
- ⏳ Indentation correct
- ⏳ Output readable

**التقدير:** 8 hours  
**الأولوية:** P0  
**التبعيات:** C.2, C.3, C.4

---

### C.6 ⏳ TODO - AST Utilities
**الحالة:** ⏳ قادم  
**الملفات:**
- `include/parser/ast/ast_utils.h`
- `src/parser/ast/ast_utils.cpp`

**الوصف:** دوال مساعدة للـ AST

**التفاصيل:**
- ⏳ `cloneAST()` - نسخ شجرة AST
- ⏳ `compareAST()` - مقارنة شجرتين
- ⏳ `traverseAST()` - اجتياز الشجرة
- ⏳ `collectNodes()` - جمع عقد معينة
- ⏳ `replaceNode()` - استبدال عقدة
- ⏳ `validateAST()` - التحقق من صحة الشجرة
- ⏳ `serializeAST()` - تحويل لـ JSON/XML
- ⏳ `deserializeAST()` - قراءة من JSON/XML

**الكود المتوقع:** ~300 lines

**الإنجاز المطلوب:**
- ⏳ جميع الدوال المساعدة تعمل
- ⏳ Memory-safe (unique_ptr)
- ⏳ Efficient traversal

**التقدير:** 6 hours  
**الأولوية:** P2  
**التبعيات:** C.5

---

## D. Testing Tasks

### D.1 ⏳ TODO - Parser Basic Tests
**الحالة:** ⏳ قادم  
**الملف:** `tests/parser_tests/parser_basic_test.cpp`  
**الوصف:** اختبارات أساسية للـ Parser

**التفاصيل:**
- ⏳ Test construction
- ⏳ Test navigation (peek, advance, etc.)
- ⏳ Test match() and check()
- ⏳ Test error handling
- ⏳ Test synchronize()

**الاختبارات المطلوبة:** 10+ tests

**التقدير:** 4 hours  
**الأولوية:** P1  
**التبعيات:** B.2

---

### D.2 ⏳ TODO - Expression Tests
**الحالة:** ⏳ قادم  
**الملف:** `tests/parser_tests/expressions_test.cpp`  
**الوصف:** اختبارات تحليل التعابير

**التفاصيل:**
- ⏳ Binary expressions (all operators)
- ⏳ Unary expressions
- ⏳ Literals (int, float, string, bool, none)
- ⏳ Variables
- ⏳ Assignment
- ⏳ Call expressions
- ⏳ Index expressions
- ⏳ Member expressions
- ⏳ Array/Map literals
- ⏳ Lambda expressions
- ⏳ Comprehensions

**الاختبارات المطلوبة:** 25+ tests

**التقدير:** 8 hours  
**الأولوية:** P0  
**التبعيات:** B.3, C.2

---

### D.3 ⏳ TODO - Precedence Tests
**الحالة:** ⏳ قادم  
**الملف:** `tests/parser_tests/precedence_test.cpp`  
**الوصف:** اختبارات أولوية العوامل

**التفاصيل:**
- ⏳ `2 + 3 * 4` → `(2 + (3 * 4))`
- ⏳ `2 * 3 + 4` → `((2 * 3) + 4)`
- ⏳ `2 ** 3 ** 4` → `(2 ** (3 ** 4))` (right assoc)
- ⏳ `!x || y && z` → `((! x) || (y && z))`
- ⏳ `x = y = z` → `(x = (y = z))` (right assoc)
- ⏳ All 13 precedence levels

**الاختبارات المطلوبة:** 20+ tests

**التقدير:** 6 hours  
**الأولوية:** P0  
**التبعيات:** B.3, C.2

---

### D.4 ⏳ TODO - Statement Tests
**الحالة:** ⏳ قادم  
**الملف:** `tests/parser_tests/statements_test.cpp`  
**الوصف:** اختبارات تحليل العبارات

**التفاصيل:**
- ⏳ If statements (with/without else)
- ⏳ While loops
- ⏳ For loops (both variants)
- ⏳ Return statements
- ⏳ Break/Continue
- ⏳ Blocks
- ⏳ Expression statements
- ⏳ Nested statements
- ⏳ `نهاية` keyword handling

**الاختبارات المطلوبة:** 15+ tests

**التقدير:** 6 hours  
**الأولوية:** P0  
**التبعيات:** B.4, C.3

---

### D.5 ⏳ TODO - Declaration Tests
**الحالة:** ⏳ قادم  
**الملف:** `tests/parser_tests/declarations_test.cpp`  
**الوصف:** اختبارات تحليل التصريحات

**التفاصيل:**
- ⏳ Variable declarations
- ⏳ Function declarations (simple, with params, with defaults)
- ⏳ Class declarations (simple, with inheritance)
- ⏳ Methods (visibility, static)
- ⏳ Constructors/Destructors
- ⏳ Fields (visibility, static)

**الاختبارات المطلوبة:** 20+ tests

**التقدير:** 8 hours  
**الأولوية:** P0  
**التبعيات:** B.5, C.4

---

### D.6 ⏳ TODO - Error Recovery Tests
**الحالة:** ⏳ قادم  
**الملف:** `tests/parser_tests/error_recovery_test.cpp`  
**الوصف:** اختبارات استعادة الـ Parser

**التفاصيل:**
- ⏳ Missing semicolon
- ⏳ Missing `نهاية`
- ⏳ Invalid token
- ⏳ Multiple errors in one file
- ⏳ Synchronization points
- ⏳ Error message quality

**الاختبارات المطلوبة:** 10+ tests

**التقدير:** 4 hours  
**الأولوية:** P1  
**التبعيات:** B.6

---

### D.7 ⏳ TODO - Integration Tests
**الحالة:** ⏳ قادم  
**الملف:** `tests/parser_tests/integration_test.cpp`  
**الوصف:** اختبارات تكامل Lexer + Parser

**التفاصيل:**
- ⏳ Parse complete programs (from examples/)
- ⏳ `examples/basics/hello.s`
- ⏳ Function examples
- ⏳ OOP examples
- ⏳ Verify AST structure
- ⏳ Verify no errors

**الاختبارات المطلوبة:** 10+ tests

**التقدير:** 6 hours  
**الأولوية:** P1  
**التبعيات:** B.1-B.5, C.1-C.5

---

## E. Build & Integration

### E.1 ⏳ TODO - Update CMakeLists (src)
**الحالة:** ⏳ قادم  
**الملف:** `src/CMakeLists.txt`  
**الوصف:** إضافة ملفات Parser إلى البناء

**التفاصيل:**
```cmake
# Parser source files
set(PARSER_SOURCES
    parser/parser_core.cpp
    parser/parser_expressions.cpp
    parser/parser_statements.cpp
    parser/parser_declarations.cpp
    parser/parser_error_recovery.cpp
    parser/ast/ast_node.cpp
    parser/ast/expressions.cpp
    parser/ast/statements.cpp
    parser/ast/declarations.cpp
    parser/ast/ast_printer.cpp
    parser/ast/ast_utils.cpp
)

# Add to library
target_sources(sad_lib PRIVATE ${PARSER_SOURCES})
```

**التقدير:** 1 hour  
**الأولوية:** P0  
**التبعيات:** B.1-B.6, C.1-C.6

---

### E.2 ⏳ TODO - Update CMakeLists (tests)
**الحالة:** ⏳ قادم  
**الملف:** `tests/CMakeLists.txt`  
**الوصف:** إضافة اختبارات Parser

**التفاصيل:**
```cmake
# Parser tests
add_executable(parser_basic_test parser_tests/parser_basic_test.cpp)
target_link_libraries(parser_basic_test sad_lib gtest_main)
add_test(NAME ParserBasicTest COMMAND parser_basic_test)

add_executable(expressions_test parser_tests/expressions_test.cpp)
target_link_libraries(expressions_test sad_lib gtest_main)
add_test(NAME ExpressionsTest COMMAND expressions_test)

# ... (6 more test executables)
```

**التقدير:** 1 hour  
**الأولوية:** P1  
**التبعيات:** D.1-D.7

---

### E.3 ⏳ TODO - Update main.cpp
**الحالة:** ⏳ قادم  
**الملف:** `src/main.cpp`  
**الوصف:** إضافة Parser إلى CLI

**التفاصيل:**
- ⏳ إضافة `--parse` flag
- ⏳ استدعاء ParserCore بعد Lexer
- ⏳ طباعة AST (إذا طُلِب)
- ⏳ عرض أخطاء Parser

**التقدير:** 2 hours  
**الأولوية:** P1  
**التبعيات:** B.1, C.5

---

## F. Git & Version Control

### F.1 ⏳ TODO - Commit: Documentation
**الحالة:** ⏳ قادم  
**الملفات:**
- `plans/extracted_rules.md`
- `plans/implementation_plan_started.md`
- `plans/tasks_initial.md`
- `plans/status/state_0002.md`

**الأمر:**
```powershell
git add plans/
git commit -m "docs: add implementation tracking documents for Phase 3

- extracted_rules.md: Complete EBNF grammar from 11 specification files
- implementation_plan_started.md: Progress report for Phases 0-2
- tasks_initial.md: Detailed task breakdown for Phase 3
- state_0002.md: Project state snapshot at start of Phase 3

Related to Phase 3 (Parser & AST) implementation"
```

**التقدير:** 15 minutes  
**الأولوية:** P0  
**التبعيات:** A.1, A.2, A.3, A.4

---

### F.2 ⏳ TODO - Commit: AST Base Classes
**الحالة:** ⏳ قادم  
**الملفات:**
- `include/parser/ast/ast_node.h`
- `src/parser/ast/ast_node.cpp`

**الأمر:**
```powershell
git add include/parser/ast/ast_node.h src/parser/ast/ast_node.cpp
git commit -m "feat(ast): add base AST node classes

- ASTNode: base class with position and visitor support
- Expression: base expression class with type info
- Statement: base statement class
- Smart pointer aliases (ExprPtr, StmtPtr)
- Full Doxygen documentation

Part of Phase 3 (Parser & AST)"
```

**التقدير:** 10 minutes  
**الأولوية:** P0  
**التبعيات:** C.1

---

### F.3 ⏳ TODO - Commit: AST Node Definitions
**الحالة:** ⏳ قادم  
**الملفات:**
- `include/parser/ast/expressions.h`
- `include/parser/ast/statements.h`
- `include/parser/ast/declarations.h`
- `src/parser/ast/expressions.cpp`
- `src/parser/ast/statements.cpp`
- `src/parser/ast/declarations.cpp`

**الأمر:**
```powershell
git add include/parser/ast/ src/parser/ast/
git commit -m "feat(ast): add all AST node definitions

Expression nodes (14 types):
- BinaryExpr, UnaryExpr, LiteralExpr, VariableExpr
- AssignExpr, CallExpr, IndexExpr, MemberExpr
- ArrayExpr, MapExpr, LambdaExpr
- ListComprehensionExpr, DictComprehensionExpr, GeneratorExpr

Statement nodes (13 types):
- ExprStmt, VarDeclStmt, IfStmt, WhileStmt
- ForStmt, ForRangeStmt, ReturnStmt, BreakStmt, ContinueStmt
- BlockStmt, TryStmt, RaiseStmt, WithStmt

Declaration nodes (10 types):
- FunctionDecl, ClassDecl, FieldDecl, MethodDecl
- ConstructorDecl, DestructorDecl, EnumDecl
- ImportStmt, ExportStmt

All nodes implement accept() and toString() methods.
Full Doxygen documentation for all classes.

Part of Phase 3 (Parser & AST)"
```

**التقدير:** 10 minutes  
**الأولوية:** P0  
**التبعيات:** C.2, C.3, C.4

---

### F.4 ⏳ TODO - Commit: Visitor Pattern
**الحالة:** ⏳ قادم  
**الملفات:**
- `include/parser/ast/ast_visitor.h`
- `include/parser/ast/ast_printer.h`
- `src/parser/ast/ast_printer.cpp`

**الأمر:**
```powershell
git add include/parser/ast/ast_visitor.h include/parser/ast/ast_printer.h src/parser/ast/ast_printer.cpp
git commit -m "feat(ast): implement visitor pattern

- ASTVisitor: abstract base class with visit methods for all node types
- ASTPrinter: concrete visitor for pretty-printing AST
  - Proper indentation
  - Readable output format
  - Support for all node types
- Full Doxygen documentation

Usage:
  ASTPrinter printer;
  ast->accept(printer);
  std::cout << printer.getOutput();

Part of Phase 3 (Parser & AST)"
```

**التقدير:** 10 minutes  
**الأولوية:** P0  
**التبعيات:** C.5

---

### F.5 ⏳ TODO - Commit: Parser Core
**الحالة:** ⏳ قادم  
**الملفات:**
- `include/parser/parser_core.h`
- `src/parser/parser_core.cpp`
- `src/parser/parser_expressions.cpp`
- `src/parser/parser_statements.cpp`
- `src/parser/parser_declarations.cpp`
- `src/parser/parser_error_recovery.cpp`

**الأمر:**
```powershell
git add include/parser/ src/parser/
git commit -m "feat(parser): implement recursive descent parser

Parser features:
- Recursive descent algorithm
- 13 levels of operator precedence (correct)
- Expression parsing (16 functions)
- Statement parsing (9 types)
- Declaration parsing (3 types: var, function, class)
- Error recovery with synchronization
- Bilingual error messages (Arabic/English)

Files:
- parser_core.cpp: navigation, helpers, main parse()
- parser_expressions.cpp: all expression types
- parser_statements.cpp: all statement types
- parser_declarations.cpp: all declaration types
- parser_error_recovery.cpp: error handling and sync

Operator precedence (15 levels):
1. postfix: () [] .
2. unary: ! - + ~
3. power: **
4. factor: * / %
5. term: + -
6. bitshift: << >>
7. comparison: < > <= >=
8. equality: == !=
9. bitwise AND: &
10. bitwise XOR: ^
11. bitwise OR: |
12. logical AND: &&
13. logical OR: ||
14. assignment: = += -= *= /= %=

Full Doxygen documentation.
DEBUG_PRINT in all major functions.

Part of Phase 3 (Parser & AST)"
```

**التقدير:** 15 minutes  
**الأولوية:** P0  
**التبعيات:** B.1-B.6

---

### F.6 ⏳ TODO - Commit: Tests
**الحالة:** ⏳ قادم  
**الملفات:**
- `tests/parser_tests/` (all files)

**الأمر:**
```powershell
git add tests/parser_tests/
git commit -m "test(parser): add comprehensive parser test suite

Test files (7 files, 80+ tests):
- parser_basic_test.cpp: basic parser functionality (10 tests)
- expressions_test.cpp: expression parsing (25 tests)
- precedence_test.cpp: operator precedence (20 tests)
- statements_test.cpp: statement parsing (15 tests)
- declarations_test.cpp: declaration parsing (20 tests)
- error_recovery_test.cpp: error handling (10 tests)
- integration_test.cpp: full program parsing (10 tests)

Coverage:
- All AST node types tested
- All precedence levels verified
- Error recovery validated
- Integration with lexer confirmed

Uses Google Test framework.

Part of Phase 3 (Parser & AST)"
```

**التقدير:** 10 minutes  
**الأولوية:** P1  
**التبعيات:** D.1-D.7

---

### F.7 ⏳ TODO - Commit: Build Integration
**الحالة:** ⏳ قادم  
**الملفات:**
- `src/CMakeLists.txt`
- `tests/CMakeLists.txt`
- `src/main.cpp`

**الأمر:**
```powershell
git add src/CMakeLists.txt tests/CMakeLists.txt src/main.cpp
git commit -m "build: integrate parser into build system

Changes:
- src/CMakeLists.txt: added parser source files
- tests/CMakeLists.txt: added parser test executables
- main.cpp: added --parse flag for AST printing

Build:
  cd build
  cmake --build . --config Debug
  ctest -R parser_tests --output-on-failure -C Debug

Part of Phase 3 (Parser & AST)"
```

**التقدير:** 10 minutes  
**الأولوية:** P1  
**التبعيات:** E.1, E.2, E.3

---

## 📊 Summary Statistics

### Total Tasks
```
Documentation:     4 tasks   (A.1-A.4)
Parser Core:       6 tasks   (B.1-B.6)
AST:               6 tasks   (C.1-C.6)
Testing:           7 tasks   (D.1-D.7)
Build & Integration: 3 tasks (E.1-E.3)
Git:               7 tasks   (F.1-F.7)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total:            33 tasks
```

### Progress
```
✅ Completed:      3 tasks   (A.1, A.2, A.3)
🔄 In Progress:    1 task    (A.3 - 80%)
⏳ Todo:          29 tasks
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Progress:         ~10%
```

### Time Estimates
```
Documentation:     11 hours  (3 done, 2 remaining)
Parser Core:       56 hours
AST:               43 hours
Testing:           42 hours
Build:              4 hours
Git:                2 hours
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Total Remaining:  147 hours  (~4 weeks @ 8 hours/day)
```

### Priority Breakdown
```
P0 (Critical):    18 tasks   (must complete for Phase 3)
P1 (High):         9 tasks   (important for quality)
P2 (Medium):       3 tasks   (nice to have)
```

---

## 🎯 Next Immediate Steps

### Today (Priority Order)
1. ✅ Finish `tasks_initial.md` (this file) - 1 hour remaining
2. ⏳ Create `plans/status/state_0002.md` - 2 hours
3. ⏳ Start `include/parser/ast/ast_node.h` - 3 hours
4. ⏳ Commit documentation files (F.1) - 15 min

### This Week
- ⏳ Complete AST base classes (C.1)
- ⏳ Complete Expression AST nodes (C.2)
- ⏳ Complete Statement AST nodes (C.3)
- ⏳ Complete Visitor Pattern (C.5)
- ⏳ Start Parser Core (B.1, B.2)

### Next Week
- ⏳ Complete Parser Core (B.1-B.6)
- ⏳ Start Testing (D.1-D.3)
- ⏳ Build Integration (E.1)

---

## ✅ Task Acceptance Criteria

### For Each Task
- [ ] Code compiles without errors/warnings
- [ ] Follows project style guide (Doxygen, naming conventions)
- [ ] DEBUG_PRINT statements where appropriate
- [ ] Error handling implemented
- [ ] Unit tests written (if applicable)
- [ ] Documentation complete
- [ ] Git commit with clear message

### For Phase 3 Overall
- [ ] All 33 tasks completed
- [ ] 0 compilation errors
- [ ] ≥80 unit tests passing
- [ ] Test coverage ≥85%
- [ ] 0 memory leaks (Valgrind/AddressSanitizer)
- [ ] Operator precedence 100% correct
- [ ] Error recovery working reliably
- [ ] Bilingual error messages
- [ ] AST structure validated
- [ ] Integration with Lexer confirmed
- [ ] Documentation 100% complete
- [ ] All commits pushed to branch

---

**Status:** 🔄 In Progress (10% complete)  
**Last Updated:** 11 November 2025 | 11:00 UTC  
**Next Update:** After completing state_0002.md  
**Estimated Completion:** ~4 weeks from now
