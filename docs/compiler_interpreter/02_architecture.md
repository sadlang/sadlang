# البنية المعمارية: المفسر والمترجم
# Architecture: Interpreter and Compiler

## 🏗️ البنية الحالية للمفسر / Current Interpreter Architecture

### نظرة عامة / Overview

المفسر الحالي للغة ص مبني على نمط Visitor Pattern ويتكون من عدة طبقات:

The current Sad interpreter is built on the Visitor Pattern and consists of multiple layers:

```
┌─────────────────────────────────────────────────────────────┐
│                    Sad Interpreter Architecture              │
│                        بنية مفسر لغة ص                      │
└─────────────────────────────────────────────────────────────┘

[1] Lexical Analysis Layer / طبقة التحليل المعجمي
    ┌──────────────────────────────────────────────┐
    │  lexer_core.cpp (1000+ lines)                │
    │  ├─ tokenize()         : النصل لـ Tokens     │
    │  ├─ scanToken()        : مسح الرموز          │
    │  ├─ isArabicLetter()   : دعم العربية         │
    │  └─ Keywords Registry  : 60+ كلمة محجوزة     │
    │                                              │
    │  Token Types: 40+ نوع                        │
    │  - Identifiers (عربي/English)               │
    │  - Numbers (١٢٣ / 123)                       │
    │  - Strings, Operators, Punctuation           │
    └──────────────────────────────────────────────┘
           ↓ std::vector<Token>

[2] Syntax Analysis Layer / طبقة التحليل النحوي
    ┌──────────────────────────────────────────────┐
    │  parser_core_impl.cpp (2165 lines)           │
    │  ├─ parseProgram()     : بناء AST            │
    │  ├─ parseStatement()   : جمل البرنامج        │
    │  ├─ parseExpression()  : التعابير            │
    │  └─ parseClassDecl()   : البرمجة الكائنية    │
    │                                              │
    │  Grammar Rules: 30+ قاعدة نحوية             │
    │  Error Recovery: synchronize()               │
    └──────────────────────────────────────────────┘
           ↓ std::unique_ptr<AST>

[3] AST Layer / طبقة الشجرة النحوية
    ┌──────────────────────────────────────────────┐
    │  AST Nodes (15+ types)                       │
    │  ├─ Statements:                              │
    │  │   • IfStmt, WhileStmt, ForStmt            │
    │  │   • ReturnStmt, TryStmt                   │
    │  │   • ClassDecl, FunctionDecl               │
    │  ├─ Expressions:                             │
    │  │   • BinaryExpr, UnaryExpr                 │
    │  │   • CallExpr, IndexExpr                   │
    │  │   • MemberExpr, NewExpr                   │
    │  └─ Visitor Interface:                       │
    │      • visitXXXStmt()                         │
    │      • visitXXXExpr()                         │
    └──────────────────────────────────────────────┘
           ↓ Visitor Pattern

[4] Interpreter Layer / طبقة المفسر
    ┌──────────────────────────────────────────────┐
    │  interpreter_core.cpp (800+ lines)           │
    │  ├─ statement_executor.cpp                   │
    │  │   └─ visitXXXStmt() : تنفيذ الجمل        │
    │  ├─ expression_evaluator.cpp                 │
    │  │   └─ visitXXXExpr() : تقييم التعابير     │
    │  └─ interpreter_classes.cpp                  │
    │      └─ OOP Support : الكائنات والوراثة      │
    │                                              │
    │  Components:                                 │
    │  • Environment (نطاقات المتغيرات)            │
    │  • FunctionManager (إدارة الدوال)            │
    │  • ClassManager (إدارة الأصناف)              │
    └──────────────────────────────────────────────┘
           ↓ Direct Execution

[5] Runtime Layer / طبقة وقت التشغيل
    ┌──────────────────────────────────────────────┐
    │  Builtin Functions Registry                  │
    │  builtin_registry.cpp (324 lines)            │
    │                                              │
    │  Categories (60+ functions):                 │
    │  ├─ I/O: اطبع، اقرأ                         │
    │  ├─ Math: الجذر، أكبر، أصغر                 │
    │  ├─ Strings: نص_طول، قسّم، صل               │
    │  ├─ Arrays: الطول، أضف، أزل                 │
    │  ├─ Type: النوع، لنص، لرقم                  │
    │  └─ Filesystem: اقرأ_ملف، اكتب_ملف          │
    └──────────────────────────────────────────────┘
           ↓ std::shared_ptr<Value>

[6] Value System / نظام القيم
    ┌──────────────────────────────────────────────┐
    │  Data::Value (Polymorphic)                   │
    │  ├─ IntValue    : الأعداد الصحيحة           │
    │  ├─ FloatValue  : الأعداد العشرية            │
    │  ├─ StringValue : النصوص                     │
    │  ├─ BoolValue   : المنطقية                   │
    │  ├─ ListValue   : القوائم                    │
    │  ├─ DictValue   : القواميس                   │
    │  └─ ObjectValue : الكائنات                   │
    └──────────────────────────────────────────────┘
           ↓ Program Output
```

### المكونات الرئيسية / Core Components

#### 1. Lexer (المحلل المعجمي)

**الملفات / Files:**
- `src/lexer/lexer_core.cpp` (1000+ lines)
- `src/lexer/lexer_keywords.cpp` (200+ lines)
- `src/lexer/token.cpp` (150+ lines)

**الوظائف / Functions:**
```cpp
std::vector<Token> tokenize(const std::string& source);
Token scanToken();
bool isArabicLetter(char32_t ch);
TokenType identifierType(const std::string& text);
```

**المميزات / Features:**
- ✅ دعم Unicode (UTF-8)
- ✅ الأرقام العربية (١٢٣٤٥٦٧٨٩٠)
- ✅ المعرفات العربية والإنجليزية
- ✅ 60+ كلمة محجوزة (عربي + English)
- ✅ معالجة الأخطاء المعجمية

---

#### 2. Parser (المحلل النحوي)

**الملفات / Files:**
- `src/parser/parser_core_impl.cpp` (2165 lines)
- `src/parser/parser_core_helpers.cpp` (952 lines)
- `src/parser/parser_core_oop.cpp` (800+ lines)
- `src/parser/ast/*.cpp` (10+ files)

**الوظائف / Functions:**
```cpp
std::unique_ptr<Program> parseProgram();
std::unique_ptr<Stmt> parseStatement();
std::unique_ptr<Expr> parseExpression();
std::unique_ptr<ClassDecl> parseClassDecl();
void synchronize(); // Error recovery
```

**القواعد النحوية / Grammar Rules:**
- Statements: if, while, for, switch, return, try/catch
- Expressions: binary, unary, call, index, member, new
- Declarations: var, function, class
- OOP: class, extends, new, this

**معالجة الأخطاء / Error Handling:**
- Panic mode recovery
- Detailed bilingual error messages
- Location tracking (line, column)
- Synchronization on statement boundaries

---

#### 3. AST (الشجرة النحوية المجردة)

**التسلسل الهرمي / Hierarchy:**

```cpp
// Base classes
class ASTNode { virtual ~ASTNode() = default; };
class Stmt : public ASTNode { };
class Expr : public ASTNode { };

// Statement types (15+)
class IfStmt : public Stmt { ... };
class WhileStmt : public Stmt { ... };
class ForStmt : public Stmt { ... };
class ReturnStmt : public Stmt { ... };
class TryStmt : public Stmt { ... };
class ClassDecl : public Stmt { ... };
class FunctionDecl : public Stmt { ... };

// Expression types (15+)
class BinaryExpr : public Expr { ... };
class UnaryExpr : public Expr { ... };
class CallExpr : public Expr { ... };
class IndexExpr : public Expr { ... };
class MemberExpr : public Expr { ... };
class NewExpr : public Expr { ... };
```

**Visitor Pattern:**
```cpp
class Visitor {
    virtual void visitIfStmt(IfStmt* stmt) = 0;
    virtual void visitWhileStmt(WhileStmt* stmt) = 0;
    virtual Value visitBinaryExpr(BinaryExpr* expr) = 0;
    // ... 30+ visit methods
};
```

---

#### 4. Interpreter (المفسر)

**الملفات / Files:**
- `src/interpreter/core/interpreter_core.cpp` (800+ lines)
- `src/interpreter/visitors/statement_executor.cpp` (1200+ lines)
- `src/interpreter/visitors/expression_evaluator.cpp` (900+ lines)
- `src/interpreter/oop/interpreter_classes.cpp` (600+ lines)

**المكونات / Components:**

**أ. Environment (البيئة)**
```cpp
class Environment {
    std::map<std::string, std::shared_ptr<Value>> variables;
    std::shared_ptr<Environment> parent;  // Lexical scope
    
    void define(const std::string& name, Value value);
    Value get(const std::string& name);
    void set(const std::string& name, Value value);
};
```

**ب. FunctionManager (مدير الدوال)**
```cpp
class FunctionManager {
    std::map<std::string, BuiltinFunction> builtins;
    std::map<std::string, UserFunction> userFunctions;
    
    void registerBuiltinFunction(const std::string& name, 
                                  BuiltinFunction func);
    Value callFunction(const std::string& name, 
                      const std::vector<Value>& args);
};
```

**ج. ClassManager (مدير الأصناف)**
```cpp
class ClassManager {
    std::map<std::string, ClassDecl*> classes;
    
    void registerClass(const std::string& name, ClassDecl* decl);
    std::shared_ptr<ObjectValue> createInstance(
        const std::string& className, 
        const std::vector<Value>& args);
};
```

**د. StatementExecutor (منفذ الجمل)**
```cpp
class StatementExecutor : public Visitor {
    void visitIfStmt(IfStmt* stmt) override {
        Value condition = evaluate(stmt->condition);
        if (condition.toBool()) {
            execute(stmt->thenBranch);
        } else if (stmt->elseBranch) {
            execute(stmt->elseBranch);
        }
    }
    
    void visitWhileStmt(WhileStmt* stmt) override {
        while (evaluate(stmt->condition).toBool()) {
            execute(stmt->body);
        }
    }
};
```

**ه. ExpressionEvaluator (مقيّم التعابير)**
```cpp
class ExpressionEvaluator : public Visitor {
    Value visitBinaryExpr(BinaryExpr* expr) override {
        Value left = evaluate(expr->left);
        Value right = evaluate(expr->right);
        
        switch (expr->op.type) {
            case PLUS: return left + right;
            case MINUS: return left - right;
            case STAR: return left * right;
            case SLASH: return left / right;
            // ... 20+ operators
        }
    }
};
```

---

#### 5. Builtin Functions (الدوال المدمجة)

**الملف / File:**
- `src/interpreter/core/builtin_registry.cpp` (324 lines)

**التسجيل / Registration:**
```cpp
void registerBuiltinFunctions(Interpreter& interpreter) {
    // I/O Functions
    interpreter.registerBuiltin("اطبع", print_func);
    interpreter.registerBuiltin("اقرأ", input_func);
    
    // Math Functions
    interpreter.registerBuiltin("الجذر", sqrt_func);
    interpreter.registerBuiltin("أكبر", max_func);
    
    // String Functions
    interpreter.registerBuiltin("نص_طول", str_len_func);
    interpreter.registerBuiltin("قسّم", split_func);
    
    // Filesystem Functions
    interpreter.registerBuiltin("اقرأ_ملف", read_file_func);
    interpreter.registerBuiltin("اكتب_ملف", write_file_func);
    
    // ... 60+ total functions
}
```

**الفئات / Categories:**
1. **I/O**: اطبع، اقرأ، اطبع_خطأ
2. **Math**: الجذر، أكبر، أصغر، جمع، القيمة_المطلقة
3. **Strings**: نص_طول، أحرف_كبيرة، أحرف_صغيرة، قسّم، صل
4. **Arrays**: الطول، أضف، أزل
5. **Type**: النوع، لنص، لرقم، لعشري
6. **Filesystem**: اقرأ_ملف، اكتب_ملف، احذف_ملف، هل_موجود

---

## 🔧 البنية المقترحة للمترجم / Proposed Compiler Architecture

### نظرة عامة / Overview

```
┌─────────────────────────────────────────────────────────────┐
│                    Sad Compiler Architecture                 │
│                       بنية مترجم لغة ص                      │
└─────────────────────────────────────────────────────────────┘

[1-3] Frontend (مشترك مع المفسر)
      Lexer → Parser → AST
      (نفس المكونات / Same components)
           ↓

[4] Semantic Analyzer / المحلل الدلالي
    ┌──────────────────────────────────────────────┐
    │  semantic_analyzer.cpp (NEW)                 │
    │  ├─ Type Checking    : التحقق من الأنواع    │
    │  ├─ Scope Analysis   : تحليل النطاق         │
    │  ├─ Symbol Table     : جدول الرموز          │
    │  └─ Error Detection  : كشف الأخطاء الدلالية │
    └──────────────────────────────────────────────┘
           ↓ Annotated AST

[5] IR Generator / مولد التمثيل الوسيط
    ┌──────────────────────────────────────────────┐
    │  SIR (Sad Intermediate Representation)       │
    │  or LLVM IR                                  │
    │                                              │
    │  Options:                                    │
    │  ├─ Custom SIR (بسيط، سريع)                 │
    │  │   • سهل التصحيح                          │
    │  │   • تحسينات خاصة بلغة ص                  │
    │  │   • يحتاج تطوير Backend كامل             │
    │  │                                           │
    │  └─ LLVM IR (قوي، معقد)                     │
    │      • تحسينات عالمية المستوى               │
    │      • دعم جميع المنصات                     │
    │      • اعتماد على LLVM                      │
    └──────────────────────────────────────────────┘
           ↓ IR Code

[6] Optimizer / المحسّن
    ┌──────────────────────────────────────────────┐
    │  optimizer.cpp                               │
    │  ├─ Constant Folding                         │
    │  ├─ Dead Code Elimination                    │
    │  ├─ Inline Expansion                         │
    │  ├─ Loop Optimization                        │
    │  └─ Register Allocation                      │
    └──────────────────────────────────────────────┘
           ↓ Optimized IR

[7] Code Generator / مولد الكود
    ┌──────────────────────────────────────────────┐
    │  codegen.cpp                                 │
    │  ├─ x86-64 Backend (Windows, Linux)          │
    │  ├─ ARM64 Backend (macOS, Mobile)            │
    │  └─ WebAssembly Backend (Web)                │
    │                                              │
    │  Output:                                     │
    │  • Object Files (.obj / .o)                  │
    │  • Assembly (.asm)                           │
    │  • Machine Code (binary)                     │
    └──────────────────────────────────────────────┘
           ↓ Object Code

[8] Linker / الرابط
    ┌──────────────────────────────────────────────┐
    │  linker.cpp                                  │
    │  ├─ Link Object Files                        │
    │  ├─ Resolve Symbols                          │
    │  ├─ Link Runtime Library                     │
    │  └─ Generate Executable                      │
    └──────────────────────────────────────────────┘
           ↓ 
    ┌──────────────────────────────────────────────┐
    │  program.exe (Windows)                       │
    │  program (Linux/macOS)                       │
    │  program.wasm (Web)                          │
    └──────────────────────────────────────────────┘
```

### الموارد الموجودة / Existing Resources

في المشروع الحالي، توجد بالفعل بنية أساسية للمترجم:

```
compiler/
├── frontend/
│   ├── include/
│   │   ├── sir/              # ✅ SIR موجود
│   │   │   ├── sir_module.h
│   │   │   ├── sir_function.h
│   │   │   ├── sir_basic_block.h
│   │   │   ├── sir_instruction.h
│   │   │   └── sir_builder.h
│   │   └── semantic/
│   │       └── semantic_analyzer.h
│   └── src/
│       └── sir/              # ✅ التنفيذ موجود
│           ├── sir_module.cpp
│           ├── sir_function.cpp
│           ├── sir_basic_block.cpp
│           ├── sir_instruction.cpp
│           └── sir_builder.cpp
│
optimizer/                    # 🔄 قيد التطوير
└── include/
    └── optimizer.h

bytecode/                     # 🔄 بديل: Bytecode VM
└── include/
    └── vm.h
```

**الحالة / Status:**
- ✅ **Frontend**: مكتمل 100% (Lexer, Parser, AST)
- ✅ **SIR System**: مكتمل 95% (Module, Function, BasicBlock, Instructions)
- 🔄 **Optimizer**: قيد التخطيط (0%)
- ❌ **Backend**: غير مبدوء (0%)
- ❌ **Linker**: غير مبدوء (0%)

---

## 🔄 تدفق التنفيذ / Execution Flow

### المفسر / Interpreter

```
program.s
   ↓ 50-100ms (startup)
[Lexer] → Tokens
   ↓ ~10ms
[Parser] → AST
   ↓ ~20ms
[Interpreter] → Direct Execution
   ↓ Runtime (slow)
Output
```

**الخصائص / Characteristics:**
- وقت بدء سريع: ~80-120ms
- لا يوجد ملف وسيط
- التنفيذ المباشر سطر بسطر
- استهلاك ذاكرة: 30-100 MB
- سرعة: أبطأ 10-50x من native code

### المترجم / Compiler

```
program.s
   ↓ 100-500ms (compilation)
[Lexer] → Tokens
   ↓
[Parser] → AST
   ↓
[Semantic Analyzer] → Annotated AST
   ↓
[IR Generator] → IR Code
   ↓
[Optimizer] → Optimized IR
   ↓
[Code Generator] → Object Files
   ↓
[Linker] → Executable
   ↓ ~1ms (execution start)
[Native Execution] → Fast Runtime
   ↓
Output
```

**الخصائص / Characteristics:**
- وقت ترجمة: 0.5-3 seconds
- ينتج ملف تنفيذي: program.exe
- تنفيذ أصلي سريع
- استهلاك ذاكرة: 5-20 MB
- سرعة: قريبة من C++ (0.8-1.2x)

---

## 📊 مقارنة المكونات / Component Comparison

| المكون / Component | المفسر / Interpreter | المترجم / Compiler |
|-------------------|---------------------|-------------------|
| **Lexer** | ✅ Shared | ✅ Shared |
| **Parser** | ✅ Shared | ✅ Shared |
| **AST** | ✅ Shared | ✅ Shared |
| **Semantic Analysis** | ❌ Runtime only | ✅ Compile-time |
| **Type Checking** | ⚠️ Runtime | ✅ Compile-time |
| **Optimization** | ❌ Limited | ✅ Extensive |
| **Code Generation** | ❌ None | ✅ Native code |
| **Linking** | ❌ Not needed | ✅ Required |
| **Runtime Library** | ✅ Large (~5MB) | ✅ Small (~500KB) |
| **Debugging Info** | ✅ Excellent | ⚠️ Good (needs DWARF) |

---

**التالي**: [الأداء / Performance →](03_performance.md)
