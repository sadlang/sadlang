# توثيق بنية المفسر الحالي
# Current Interpreter Architecture Documentation

**التاريخ / Date:** 2 ديسمبر 2025  
**الحالة / Status:** المرحلة 0 - التوثيق / Phase 0 - Documentation  
**الإصدار / Version:** 1.0

---

## 🎯 نظرة عامة / Overview

### (AR) العربية

المفسر الحالي للغة ص مبني على بنية **Tree-Walking Interpreter** (مفسر يسير في الشجرة).
يقوم بتنفيذ البرنامج مباشرةً من شجرة AST بدون توليد كود وسيط أو كود آلة.

**المكونات الرئيسية:**
1. **Lexer** - المحلل المعجمي
2. **Parser** - المحلل النحوي
3. **AST** - الشجرة النحوية التجريدية
4. **Interpreter** - المفسر
5. **Runtime** - بيئة التشغيل

---

## 📐 العمارة الكاملة / Complete Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                    لغة ص - المفسر الحالي                       │
│                  Sad Language - Current Interpreter             │
└─────────────────────────────────────────────────────────────────┘

┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│   Source    │────▶│    Lexer    │────▶│   Tokens    │
│   .s file   │     │   المحلل    │     │   الرموز   │
│             │     │   المعجمي   │     │             │
└─────────────┘     └─────────────┘     └─────────────┘
                                              │
                                              ▼
┌─────────────┐     ┌─────────────┐     ┌─────────────┐
│ Execution   │◀────│ Interpreter │◀────│   Parser    │
│   التنفيذ   │     │   المفسر    │     │   المحلل    │
│             │     │             │     │   النحوي    │
└─────────────┘     └─────────────┘     └─────────────┘
      │                   │                    │
      │                   │                    ▼
      │                   │              ┌─────────────┐
      │                   │              │  AST Tree   │
      │                   └─────────────▶│   شجرة      │
      │                                  │   AST       │
      ▼                                  └─────────────┘
┌─────────────┐
│   Output    │
│   المخرجات  │
└─────────────┘
```

---

## 🔧 المكونات بالتفصيل / Components in Detail

### 1. Lexer - المحلل المعجمي

**الملفات:**
```
include/lexer/
├── token.h              # تعريف Token و TokenType
├── lexer_core.h         # واجهة المحلل المعجمي
└── lexer_keywords.h     # الكلمات المفتاحية

src/lexer/
├── token.cpp            # تنفيذ Token
├── lexer_core.cpp       # تنفيذ Lexer
└── lexer_keywords.cpp   # خريطة الكلمات المفتاحية
```

**المسؤوليات:**
- قراءة النص المصدري حرفاً بحرفاً
- تحويل النص إلى رموز (Tokens)
- التعرف على الكلمات المفتاحية العربية والإنجليزية
- معالجة UTF-8 والأحرف العربية
- تتبع موقع كل رمز (السطر والعمود)

**أنواع الرموز (TokenType):**
```cpp
// الكلمات المفتاحية / Keywords
KW_دالة,      // function
KW_إذا,       // if
KW_وإلا,      // else
KW_بينما,     // while
KW_لكل,       // for
KW_إرجاع,     // return
KW_صنف,       // class
KW_وراثة,     // extends

// المُعاملات / Operators
OP_PLUS,      // +
OP_MINUS,     // -
OP_MUL,       // *
OP_DIV,       // /
OP_MOD,       // %
OP_ASSIGN,    // =
OP_EQ,        // ==
OP_LT,        // <
OP_GT,        // >

// الثوابت / Literals
LIT_NUMBER,   // 42, 3.14
LIT_STRING,   // "مرحبا"
LIT_BOOLEAN,  // صح، خطأ

// المعرفات / Identifiers
IDENTIFIER,   // أسماء المتغيرات والدوال

// العلامات / Punctuation
LPAREN,       // (
RPAREN,       // )
LBRACE,       // {
RBRACE,       // }
SEMICOLON,    // ;
COMMA,        // ,
```

**مثال:**
```
Input:  دالة جمع(رقم أ، رقم ب)
Tokens: [KW_دالة] [IDENTIFIER:"جمع"] [LPAREN] [KW_رقم] [IDENTIFIER:"أ"] 
        [COMMA] [KW_رقم] [IDENTIFIER:"ب"] [RPAREN]
```

---

### 2. Parser - المحلل النحوي

**الملفات:**
```
include/parser/
├── parser_core.h        # واجهة المحلل النحوي

src/parser/
├── parser_core.cpp      # الدوال الرئيسية
├── parser_core_impl.cpp # تحليل التعابير
├── parser_core_helpers.cpp  # دوال مساعدة
└── parser_core_oop.cpp  # تحليل البرمجة الكائنية
```

**المسؤوليات:**
- بناء شجرة AST من الرموز
- التحقق من صحة القواعد النحوية
- معالجة الأولويات (Precedence)
- التعامل مع الأخطاء النحوية

**القواعد النحوية (Grammar Rules):**
```
Program         → Statement*
Statement       → FunctionDecl | VarDecl | ClassDecl | ExprStmt | ...
FunctionDecl    → "دالة" Identifier "(" Parameters ")" Block
VarDecl         → Type Identifier ("=" Expression)? ";"
ClassDecl       → "صنف" Identifier ("وراثة" Identifier)? ClassBody
Expression      → Assignment | Logical | Comparison | ...
Assignment      → Identifier "=" Expression
Logical         → Equality (("و" | "أو") Equality)*
Comparison      → Term ((">" | "<" | "==" | "!=") Term)*
Term            → Factor (("+" | "-") Factor)*
Factor          → Unary (("*" | "/" | "%") Unary)*
Unary           → ("!" | "-" | "++") Primary
Primary         → Number | String | Boolean | Identifier | "(" Expression ")"
```

**Precedence (الأولويات):**
```
1. Primary (literals, identifiers, parentheses)
2. Unary (!, -, ++, --)
3. Factor (*, /, %)
4. Term (+, -)
5. Comparison (>, <, ==, !=)
6. Logical (و، أو)
7. Assignment (=)
```

---

### 3. AST - الشجرة النحوية التجريدية

**الملفات:**
```
include/parser/ast/
├── ast_node.h           # الفئة الأساسية ASTNode
├── expressions.h        # عُقد التعابير
├── statements.h         # عُقد الجُمل
├── declarations.h       # عُقد التصريحات
└── ast_printer.h        # طباعة الشجرة

src/parser/ast/
├── ast_node.cpp
├── expressions.cpp
├── statements.cpp
├── declarations.cpp
└── ast_printer.cpp

src/ast/oop/
└── class_nodes.cpp      # عُقد البرمجة الكائنية
```

**التسلسل الهرمي (Hierarchy):**
```
ASTNode (abstract)
├── Expression
│   ├── LiteralExpr (42, "text", صح)
│   ├── IdentifierExpr (variable name)
│   ├── BinaryExpr (a + b)
│   ├── UnaryExpr (-x, !condition)
│   ├── CallExpr (function(args))
│   ├── ArrayAccessExpr (arr[i])
│   ├── MemberAccessExpr (obj.property)
│   └── LambdaExpr (دالة(x) -> x + 1)
│
└── Statement
    ├── ExpressionStmt (expression;)
    ├── VarDeclStmt (رقم x = 5;)
    ├── FunctionDeclStmt (دالة f() {...})
    ├── ClassDeclStmt (صنف C {...})
    ├── IfStmt (إذا {...} وإلا {...})
    ├── WhileStmt (بينما {...})
    ├── ForStmt (لكل {...})
    ├── ReturnStmt (إرجاع value;)
    └── BlockStmt ({...})
```

**مثال AST:**
```cpp
// Source: رقم س = 5 + 3؛
Program
└── VarDeclStmt
    ├── type: "رقم"
    ├── name: "س"
    └── initializer: BinaryExpr
        ├── operator: OP_PLUS
        ├── left: LiteralExpr(5)
        └── right: LiteralExpr(3)
```

---

### 4. Interpreter - المفسر

**الملفات:**
```
include/interpreter/
├── core/
│   ├── interpreter_core.h    # المفسر الرئيسي
│   └── builtin_registry.h    # سجل الدوال المضمنة
└── visitors/
    ├── expression_evaluator.h # تقييم التعابير
    ├── statement_executor.h   # تنفيذ الجُمل
    └── statement_executor_oop.h # تنفيذ OOP

src/interpreter/
├── core/
│   ├── interpreter_core.cpp
│   └── builtin_registry.cpp
├── visitors/
│   ├── expression_evaluator.cpp
│   ├── statement_executor.cpp
│   └── statement_executor_oop.cpp
└── exception.cpp
```

**المسؤوليات:**
- تنفيذ AST node by node
- إدارة المتغيرات والنطاقات (Scopes)
- إدارة الدوال واستدعاءاتها
- معالجة الأخطاء Runtime

**نمط الزائر (Visitor Pattern):**
```cpp
class ExpressionEvaluator {
public:
    Value evaluate(Expression* expr);
    
    Value visitLiteral(LiteralExpr* expr);
    Value visitBinary(BinaryExpr* expr);
    Value visitUnary(UnaryExpr* expr);
    Value visitCall(CallExpr* expr);
    // ... المزيد
};

class StatementExecutor {
public:
    void execute(Statement* stmt);
    
    void visitExpressionStmt(ExpressionStmt* stmt);
    void visitVarDecl(VarDeclStmt* stmt);
    void visitFunctionDecl(FunctionDeclStmt* stmt);
    void visitIf(IfStmt* stmt);
    void visitWhile(WhileStmt* stmt);
    // ... المزيد
};
```

**مثال تنفيذ:**
```cpp
// BinaryExpr: 5 + 3
Value ExpressionEvaluator::visitBinary(BinaryExpr* expr) {
    Value left = evaluate(expr->left.get());
    Value right = evaluate(expr->right.get());
    
    if (expr->op == OP_PLUS) {
        return Value(left.asNumber() + right.asNumber());
    }
    // ... عمليات أخرى
}
```

---

### 5. Runtime - بيئة التشغيل

**الملفات:**
```
include/data/
├── types/
│   ├── value.h              # نوع القيمة الديناميكية
│   ├── data_types.h         # أنواع البيانات
│   ├── class_type.h         # نوع الصنف
│   └── object_instance.h    # كائن الصنف
├── managers/
│   ├── variable_manager.h   # إدارة المتغيرات
│   ├── function_manager.h   # إدارة الدوال
│   ├── class_manager.h      # إدارة الأصناف
│   └── object_manager.h     # إدارة الكائنات
└── scope/
    └── scope_manager.h      # إدارة النطاقات

src/data/
├── types/
│   ├── value.cpp
│   ├── class_type.cpp
│   └── object_instance.cpp
└── managers/
    ├── variable_manager.cpp
    ├── function_manager.cpp
    ├── scope_manager.cpp
    ├── class_manager.cpp
    └── object_manager.cpp
```

#### 5.1 Value - القيمة الديناميكية

```cpp
class Value {
public:
    enum Type {
        TYPE_NULL,      // فارغ
        TYPE_NUMBER,    // رقم
        TYPE_STRING,    // نص
        TYPE_BOOLEAN,   // منطقي
        TYPE_ARRAY,     // مصفوفة
        TYPE_OBJECT,    // كائن
        TYPE_FUNCTION,  // دالة
        TYPE_POINTER    // مؤشر (Stage 1)
    };
    
private:
    Type type_;
    union {
        double number_;
        bool boolean_;
        std::string* string_;
        std::vector<Value>* array_;
        void* pointer_;
        // ... المزيد
    };
};
```

#### 5.2 VariableManager - إدارة المتغيرات

```cpp
class VariableManager {
public:
    void declare(const std::string& name, const Value& value);
    Value get(const std::string& name);
    void set(const std::string& name, const Value& value);
    bool exists(const std::string& name);
    
private:
    std::unordered_map<std::string, Value> variables_;
};
```

#### 5.3 ScopeManager - إدارة النطاقات

```cpp
class ScopeManager {
public:
    void enterScope();      // دخول نطاق جديد
    void exitScope();       // الخروج من النطاق
    
    void declare(const std::string& name, const Value& value);
    Value get(const std::string& name);
    void set(const std::string& name, const Value& value);
    
private:
    std::vector<std::unordered_map<std::string, Value>> scopes_;
    size_t currentScope_ = 0;
};
```

---

## 🔄 مسار التنفيذ الكامل / Complete Execution Flow

### (AR) العربية

```
1. قراءة الملف المصدري
   ↓
2. Lexer: تحويل النص إلى رموز
   Input:  دالة جمع(رقم أ، رقم ب) { إرجاع أ + ب؛ }
   Output: [KW_دالة, IDENTIFIER, LPAREN, ...]
   ↓
3. Parser: بناء شجرة AST
   Output: FunctionDeclStmt
           ├── name: "جمع"
           ├── params: [{"أ", "رقم"}, {"ب", "رقم"}]
           └── body: ReturnStmt
               └── BinaryExpr(أ + ب)
   ↓
4. Interpreter: تنفيذ الشجرة
   - تسجيل الدالة في FunctionManager
   - عند الاستدعاء:
     4.1. دخول نطاق جديد
     4.2. تمرير المعاملات
     4.3. تنفيذ Body
     4.4. إرجاع النتيجة
     4.5. الخروج من النطاق
   ↓
5. Output: النتيجة
```

---

## 🎨 ميزات مدعومة / Supported Features

### ✅ مكتمل / Completed

1. **أنواع البيانات الأساسية:**
   - رقم (Number) - `42`, `3.14`
   - نص (String) - `"مرحبا"`
   - منطقي (Boolean) - `صح`, `خطأ`
   - فارغ (Null) - `فارغ`

2. **العمليات:**
   - حسابية: `+`, `-`, `*`, `/`, `%`
   - مقارنة: `==`, `!=`, `>`, `<`, `>=`, `<=`
   - منطقية: `و`, `أو`, `!`
   - ثنائية (Stage 1): `&`, `|`, `^`, `<<`, `>>`

3. **التحكم في التدفق:**
   - إذا/وإلا: `إذا (condition) {...} وإلا {...}`
   - بينما: `بينما (condition) {...}`
   - لكل: `لكل (item في array) {...}`

4. **الدوال:**
   - تعريف: `دالة name(params) {...}`
   - استدعاء: `name(args)`
   - إرجاع: `إرجاع value؛`
   - Lambda: `دالة(x) -> x + 1`

5. **المصفوفات:**
   - تعريف: `مصفوفة arr = [1، 2، 3]؛`
   - الوصول: `arr[0]`
   - دوال مضمنة: `طول()`, `أضف()`, `احذف()`

6. **البرمجة الكائنية:**
   - الأصناف: `صنف Name {...}`
   - الوراثة: `صنف Child وراثة Parent {...}`
   - الكائنات: `Name obj = جديد Name()؛`
   - الخصائص: `obj.property`
   - الدوال: `obj.method()`

7. **Stage 1 (Low-Level):**
   - المؤشرات: `مؤشر<رقم> ptr = &x؛`
   - Dereferencing: `*ptr`
   - عناوين الذاكرة: `&variable`
   - عمليات ثنائية: `&`, `|`, `^`, `~`, `<<`, `>>`

8. **الدوال المضمنة:**
   - I/O: `اطبع()`, `اقرأ()`
   - Math: `جذر()`, `قوة()`, `جا()`, `جتا()`
   - String: `طول_النص()`, `قسم_النص()`
   - Array: `طول()`, `أضف()`, `احذف()`
   - Type: `نوع()`, `حول_لرقم()`, `حول_لنص()`

---

## ⚡ الأداء الحالي / Current Performance

### (AR) العربية

**الخصائص:**
- Tree-Walking Interpreter (بطيء)
- لا يوجد تحسين (No optimization)
- تفسير مباشر من AST
- استدعاء دوال C++ لكل عملية

**السرعة:**
- عمليات حسابية: ~100x أبطأ من C++
- استدعاء دوال: ~50x أبطأ من C++
- معالجة مصفوفات: ~200x أبطأ من C++

**الذاكرة:**
- AST كامل في الذاكرة
- لا يوجد garbage collection فعال
- تسرب محتمل في الدوال العودية

**القيود:**
- لا يمكن تجميع البرامج (No compilation)
- لا يمكن إنشاء executables
- يجب توزيع المفسر مع البرنامج

---

## 🚀 الخطوة التالية: المترجم / Next Step: Compiler

### لماذا نحتاج مترجم؟ / Why do we need a compiler?

1. **الأداء:** 50-100x أسرع
2. **Standalone Executables:** برامج مستقلة
3. **Optimization:** تحسينات متقدمة
4. **Cross-compilation:** تجميع لمنصات مختلفة
5. **Production-ready:** جاهز للإنتاج

### الهدف / Goal

تحويل هذا المفسر إلى مترجم متعدد الخلفيات:
- **Bytecode Backend:** للتطوير السريع (5-10x faster)
- **LLVM Backend:** للأداء العالي (50-100x faster)

---

**السابق / Previous:** [المرحلة 0: التحضير](../03_phase0_preparation.md)  
**التالي / Next:** [SIR Specification](phase0_sir_spec.md)
