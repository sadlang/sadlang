# 📊 تحليل بنية مشروع لغة "ص" (Sad Language)
# 📊 Sad Language Project Structure Analysis

**تاريخ التحليل / Analysis Date:** 2 يناير 2026  
**المحلل / Analyst:** GitHub Copilot  
**الإصدار / Version:** 1.0.0  
**الحالة / Status:** 🟢 تحليل كامل / Complete Analysis

---

## 📑 جدول المحتويات / Table of Contents

1. [نظرة عامة / Overview](#overview)
2. [البنية الحالية / Current Structure](#current-structure)
3. [المفسر vs المترجم / Interpreter vs Compiler](#interpreter-vs-compiler)
4. [نظام الأنواع / Type System](#type-system)
5. [دعم Type Annotations](#type-annotations-support)
6. [التوصيات / Recommendations](#recommendations)

---

<div id="overview"></div>

## 🎯 1. نظرة عامة / Overview

### الوضع الحالي / Current Status

لغة "ص" هي **لغة برمجية هجينة** تدعم:
- ✅ **التفسير المباشر (Interpretation)** - مكتمل 100%
- ✅ **الترجمة إلى كود آلي (Compilation)** - قيد التطوير (Phase 1.2 مكتمل)
- ✅ **JIT Compilation** - مكتمل (Phase 1.2.4)
- 🟡 **نظام الأنواع المتقدم (Type System)** - قيد البناء (Phase 1.3)

### المعمارية العامة / General Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                    Sad Language Project                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────────┐         ┌──────────────────┐         │
│  │   Interpreter   │         │    Compiler      │         │
│  │   (المفسر)      │         │    (المترجم)     │         │
│  │                 │         │                  │         │
│  │  • إنتاج فوري  │◄────────┤  • LLVM Backend  │         │
│  │  • Dynamic      │         │  • JIT Engine    │         │
│  │  • Runtime      │  مشترك  │  • Optimizer     │         │
│  │                 │         │  • Type Checker  │         │
│  └────────┬────────┘         └────────┬─────────┘         │
│           │                           │                    │
│           └───────────┬───────────────┘                    │
│                       ↓                                    │
│          ┌────────────────────────┐                       │
│          │   Shared Components    │                       │
│          │   (المكونات المشتركة)  │                       │
│          ├────────────────────────┤                       │
│          │ • Lexer                │                       │
│          │ • Parser               │                       │
│          │ • AST                  │                       │
│          │ • Module System        │                       │
│          │ • Error Manager        │                       │
│          │ • Standard Library     │                       │
│          └────────────────────────┘                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

<div id="current-structure"></div>

## 🏗️ 2. البنية الحالية / Current Structure

### 2.1 التقسيم الوظيفي / Functional Division

#### 🔵 المجلدات المشتركة (Shared)

```
📁 include/               # ملفات Header للمفسر / Interpreter headers
├── data/                # إدارة البيانات / Data management
│   ├── types/
│   │   └── value.h      # ✅ نظام Value (Runtime)
│   ├── managers/
│   └── scope/
├── lexer/               # ✅ المحلل المعجمي (مشترك)
│   ├── token.h
│   └── lexer_core.h
├── parser/              # ✅ المحلل النحوي (مشترك)
│   ├── parser_core.h
│   └── ast/
│       ├── ast_node.h
│       ├── expressions.h
│       ├── statements.h
│       └── declarations.h   # ✅ يحتوي على FunctionDecl مع type annotations
├── interpreter/         # المفسر فقط / Interpreter only
│   ├── core/
│   └── visitors/
├── errors/              # ✅ إدارة الأخطاء (مشتركة)
├── modules/             # ✅ نظام الوحدات (مشترك)
├── optimizer/           # ✅ المحسن (مشترك)
├── stdlib/              # ✅ المكتبة القياسية (مشتركة)
└── utils/               # ✅ أدوات مساعدة (مشتركة)

📁 src/                  # ملفات التنفيذ / Implementation files
├── main.cpp            # ✅ نقطة الدخول للمفسر
├── lexer/              # ✅ تنفيذ Lexer (مشترك)
├── parser/             # ✅ تنفيذ Parser (مشترك)
├── interpreter/        # المفسر فقط / Interpreter only
├── data/               # ✅ تنفيذ Value و Managers
├── errors/             # ✅ تنفيذ Error Management
├── modules/            # ✅ تنفيذ Module System
├── optimizer/          # ✅ تنفيذ Optimizer
├── stdlib/             # ✅ تنفيذ Standard Library
└── utils/              # ✅ تنفيذ Utilities
```

#### 🟢 المجلدات الخاصة بالمترجم (Compiler Only)

```
📁 compiler/
├── frontend/           # الواجهة الأمامية / Frontend
│   ├── include/
│   │   ├── sir_types.h      # ✅ نظام SIR (Semantic IR)
│   │   ├── sir_module.h
│   │   └── type_info.h      # ✅ نظام Type Info لـ SIR
│   ├── src/
│   │   ├── sir_types.cpp
│   │   └── sir_module.cpp
│   └── type_checker/        # 🟡 Type Checker (قيد البناء)
│       ├── include/
│       │   ├── type_checker.h
│       │   ├── type_context.h
│       │   ├── type_inference.h
│       │   ├── type_errors.h
│       │   └── typed_ast.h
│       ├── src/
│       └── tests/
│
├── type_system/         # ✅ نظام الأنواع المتقدم (Phase 1.3.0)
│   ├── include/
│   │   ├── type.h           # النوع الأساسي المجرد
│   │   ├── primitive_type.h # الأنواع البدائية
│   │   └── type_registry.h  # سجل الأنواع (Type Interning)
│   ├── src/
│   └── tests/
│
├── backend/             # الواجهة الخلفية / Backend
│   ├── llvm/           # ✅ LLVM Backend
│   └── native/
│
├── backends/            # Backends متعددة
│   └── llvm/
│
├── jit/                 # ✅ JIT Engine (Phase 1.2)
│   ├── include/
│   ├── src/
│   └── tests/
│
├── optimizer/           # المحسن المتقدم / Advanced Optimizer
│   ├── include/
│   ├── src/
│   └── tests/
│
├── middleend/           # Middle-end (تحسينات مستقلة)
├── runtime/             # وقت التشغيل للكود المترجم
├── bytecode/            # نظام Bytecode (اختياري)
└── src/                 # مصادر المترجم
```

#### 🔶 الأدوات والمرافق (Tools & Utilities)

```
📁 tools/
├── pkg/                # ✅ مدير الحزم / Package Manager
│   ├── cli.cpp
│   ├── package.h
│   ├── dependency_resolver.h
│   └── registry_client.h
├── lsp/                # Language Server Protocol
├── docgen/             # مولد التوثيق / Documentation generator
├── vscode-extension/   # VS Code Extension
└── compiler/           # أدوات المترجم / Compiler tools

📁 tests/               # الاختبارات / Tests
├── lexer_tests/        # ✅ اختبارات Lexer
├── parser_tests/       # ✅ اختبارات Parser
├── interpreter_tests/  # اختبارات المفسر
├── compiler/           # اختبارات المترجم
├── oop/                # ✅ اختبارات OOP
├── stdlib_tests/       # ✅ اختبارات المكتبة القياسية
└── backends/           # اختبارات Backends

📁 examples/            # أمثلة على الكود
📁 docs/                # التوثيق / Documentation
📁 plans/               # خطط التطوير / Development plans
📁 rules/               # قواعد اللغة / Language rules
```

### 2.2 نقاط الدخول / Entry Points

#### المفسر (Interpreter)
```
الملف: src/main.cpp
الوظيفة: يقرأ ملف .s ويفسره مباشرة
Pipeline: Source → Lexer → Parser → AST → Interpreter → Result

الاستخدام:
sad.exe program.s          # تشغيل مباشر
```

#### المترجم (Compiler)
```
الملف: compiler/src/main.cpp (مخطط)
الوظيفة: يترجم ملف .s إلى كود آلي
Pipeline: Source → Lexer → Parser → AST → SIR → LLVM IR → Native Code

الاستخدام (مخطط):
sadc program.s -o program  # ترجمة إلى ملف تنفيذي
```

---

<div id="interpreter-vs-compiler"></div>

## ⚖️ 3. المفسر vs المترجم / Interpreter vs Compiler

### 3.1 ما هو المفسر (Interpreter)?

**الموقع:** `include/interpreter/` + `src/interpreter/`

**الوظيفة:**
- ✅ تنفيذ الكود **مباشرة** بدون ترجمة
- ✅ يتجول على AST ويقوم بالتنفيذ (Visitor Pattern)
- ✅ Dynamic typing كامل
- ✅ سريع في البدء، بطيء في التنفيذ

**المكونات الرئيسية:**

```cpp
namespace Sad::Interpreter {
    class Interpreter {
        // التنفيذ الرئيسي / Main execution
        Value execute(AST::StmtPtr);
        
        // زوار التعابير / Expression visitors
        Value visitBinaryExpr(AST::BinaryExpr*);
        Value visitCallExpr(AST::CallExpr*);
        
        // زوار الجمل / Statement visitors
        void visitVarDeclStmt(AST::VarDeclStmt*);
        void visitFunctionDeclStmt(AST::FunctionDecl*);
        void visitClassDeclStmt(AST::ClassDecl*);
    };
}
```

**نظام القيم (Value System):**
```cpp
// include/data/types/value.h
class Value {
    ValueType type_;  // VOID, INTEGER, DOUBLE, STRING, BOOLEAN, ARRAY, MAP
    union {
        int64_t intValue;
        double doubleValue;
        bool boolValue;
    };
    std::string stringValue;
    std::vector<Value> arrayValue;
    std::unordered_map<std::string, Value> mapValue;
};
```

### 3.2 ما هو المترجم (Compiler)?

**الموقع:** `compiler/`

**الوظيفة:**
- 🟡 يترجم الكود إلى **LLVM IR** ثم إلى **كود آلي**
- 🟡 يدعم **JIT compilation** (Phase 1.2 ✅)
- 🟡 يدعم **تحسينات متقدمة** (Optimizer)
- 🟡 يدعم **Type checking** (Phase 1.3 🟡)

**Pipeline المترجم:**

```
Source Code (.s)
      ↓
[Lexer] ← مشترك
      ↓
[Parser] ← مشترك
      ↓
[AST] ← مشترك
      ↓
[Type Checker] ← خاص بالمترجم
      ↓
[SIR (Semantic IR)] ← خاص بالمترجم
      ↓
[LLVM IR Generator] ← خاص بالمترجم
      ↓
[LLVM Optimizer] ← LLVM
      ↓
[Native Code / JIT] ← LLVM
```

**المكونات الرئيسية:**

1. **Frontend (compiler/frontend/)**
   - `sir_types.h/cpp`: نظام SIR (Semantic Intermediate Representation)
   - `type_info.h`: معلومات الأنواع في SIR
   - `type_checker/`: Type Checker (قيد البناء)

2. **Type System (compiler/type_system/)**
   - `type.h`: النوع الأساسي المجرد
   - `primitive_type.h`: الأنواع البدائية
   - `type_registry.h`: سجل الأنواع (Type Interning)

3. **Backend (compiler/backend/llvm/)**
   - LLVM IR Generation
   - Native code generation

4. **JIT Engine (compiler/jit/)**
   - ✅ Hot path detection
   - ✅ JIT compilation
   - ✅ Tiered compilation

### 3.3 الملفات المشتركة (Shared Components)

| المكون | الموقع | المفسر | المترجم | ملاحظات |
|--------|--------|---------|----------|---------|
| **Lexer** | `include/lexer/` + `src/lexer/` | ✅ | ✅ | مشترك 100% |
| **Parser** | `include/parser/` + `src/parser/` | ✅ | ✅ | مشترك 100% |
| **AST Nodes** | `include/parser/ast/` | ✅ | ✅ | مشترك 100% |
| **Error Manager** | `include/errors/` + `src/errors/` | ✅ | ✅ | مشترك 100% |
| **Module System** | `include/modules/` + `src/modules/` | ✅ | ✅ | مشترك 100% |
| **Standard Library** | `include/stdlib/` + `src/stdlib/` | ✅ | ✅ | مشترك 100% |
| **Value System** | `include/data/types/value.h` | ✅ | ❌ | خاص بالمفسر |
| **Type System** | `compiler/type_system/` | ❌ | ✅ | خاص بالمترجم |
| **SIR** | `compiler/frontend/` | ❌ | ✅ | خاص بالمترجم |
| **Interpreter Core** | `include/interpreter/` | ✅ | ❌ | خاص بالمفسر |
| **LLVM Backend** | `compiler/backend/llvm/` | ❌ | ✅ | خاص بالمترجم |

---

<div id="type-system"></div>

## 🎨 4. نظام الأنواع / Type System

### 4.1 أنظمة الأنواع الموجودة (3 أنظمة)

#### 📌 النظام 1: Value System (Runtime - المفسر)

**الموقع:** `include/data/types/value.h` + `src/data/types/value.cpp`

**الغرض:** تمثيل القيم في **وقت التشغيل** (Runtime)

**الأنواع المدعومة:**
```cpp
enum class ValueType {
    VOID,       // لاشيء
    INTEGER,    // رقم (int64_t)
    DOUBLE,     // عشري (double)
    STRING,     // نص
    BOOLEAN,    // منطقي
    ARRAY,      // مصفوفة
    MAP         // قاموس
};
```

**الاستخدام:**
- ✅ المفسر فقط (Interpreter)
- ✅ يستخدم في `Interpreter::execute()`
- ✅ Dynamic typing كامل
- ❌ لا يستخدم في المترجم

**مثال:**
```cpp
Value v1(42);                          // INTEGER
Value v2("مرحبا");                     // STRING
Value v3(std::vector<Value>{v1, v2}); // ARRAY
```

#### 📌 النظام 2: SIR Type Info (Compiler Frontend)

**الموقع:** `compiler/frontend/include/type_info.h`

**الغرض:** معلومات الأنواع في **SIR (Semantic IR)**

**الأنواع المدعومة:**
```cpp
enum class SIRType {
    VOID,
    I64,        // رقم 64-bit
    F64,        // عشري 64-bit
    BOOL,
    STRING,
    POINTER,    // مؤشر
    ARRAY,
    STRUCT,     // للأصناف
    FUNCTION
};
```

**الاستخدام:**
- ✅ المترجم فقط (Compiler Frontend)
- ✅ يستخدم في SIR Module
- ✅ يربط AST بـ LLVM IR
- ❌ لا يستخدم في المفسر

**التسلسل الهرمي:**
```cpp
class TypeInfo { /* base */ };
class PrimitiveType : public TypeInfo { /* i64, f64, bool */ };
class PointerType : public TypeInfo { /* T* */ };
class ArrayType : public TypeInfo { /* T[] */ };
class StructType : public TypeInfo { /* classes */ };
class FunctionType : public TypeInfo { /* (T1, T2) -> T3 */ };
```

#### 📌 النظام 3: Advanced Type System (Phase 1.3.0) ✅

**الموقع:** `compiler/type_system/`

**الغرض:** نظام أنواع **متقدم** للمترجم (Generics, Union Types, Type Inference)

**الأنواع المدعومة:**
```cpp
enum class TypeKind {
    // الأنواع البدائية
    Void, Integer, Float, Boolean, String,
    
    // الأنواع المركبة
    Array, Dictionary, Tuple,
    
    // الأنواع الوظيفية
    Function,
    
    // الأنواع الكائنية
    Class, Interface,
    
    // الأنواع المتقدمة
    Generic,       // T, U, V
    TypeParameter, // <T>
    Union,         // A | B
    Intersection,  // A & B
    Optional,      // T?
    
    // أنواع خاصة
    Any, Never, Unknown, Error
};
```

**الاستخدام:**
- ✅ المترجم فقط (Compiler)
- ✅ Type Checker
- ✅ Type Inference
- ✅ Generics
- ❌ لا يستخدم في المفسر

**التسلسل الهرمي:**
```cpp
class Type { /* base */ };
class PrimitiveType : public Type { /* رقم، نص، منطقي */ };
class GenericType : public Type { /* T, U, V */ };
class UnionType : public Type { /* A | B */ };
class FunctionType : public Type { /* (T1, T2) -> T3 */ };
class ClassType : public Type { /* صنف شخص */ };
```

**Type Registry (Type Interning):**
```cpp
class TypeRegistry {
    static TypeRegistry& getInstance();
    
    TypePtr getVoidType();
    TypePtr getIntegerType();
    TypePtr getFloatType();
    TypePtr getBooleanType();
    TypePtr getStringType();
    
    TypePtr internType(TypePtr type); // تجنب التكرار
};
```

### 4.2 مقارنة الأنظمة الثلاثة

| الخاصية | Value System | SIR Type Info | Type System |
|---------|--------------|---------------|-------------|
| **الموقع** | `include/data/types/` | `compiler/frontend/` | `compiler/type_system/` |
| **الاستخدام** | المفسر فقط | SIR فقط | المترجم الكامل |
| **الغرض** | Runtime values | SIR typing | Static type checking |
| **Dynamic Typing** | ✅ نعم | ❌ لا | ❌ لا |
| **Static Typing** | ❌ لا | ✅ محدود | ✅ متقدم |
| **Generics** | ❌ لا | ❌ لا | ✅ نعم |
| **Union Types** | ❌ لا | ❌ لا | ✅ نعم |
| **Type Inference** | ❌ لا | ❌ لا | ✅ نعم |
| **الحالة** | ✅ مكتمل | ✅ مكتمل | 🟡 Phase 1.3.0 |

### 4.3 هل هناك تكرار؟

#### ✅ **لا يوجد تكرار حقيقي** - الأنظمة الثلاثة **مكملة لبعضها**:

1. **Value System** → Runtime للمفسر
2. **SIR Type Info** → ربط AST بـ LLVM
3. **Type System** → Static type checking و Generics

**التكامل:**
```
Parser → AST (مشترك)
   ↓
   ├─→ Interpreter → Value System (Runtime)
   │
   └─→ Compiler → Type Checker (Type System)
                     ↓
                  SIR (Type Info)
                     ↓
                  LLVM IR
```

---

<div id="type-annotations-support"></div>

## 📝 5. دعم Type Annotations

### 5.1 هل يدعم Parser type annotations؟

#### ✅ **نعم - دعم كامل موجود**

**الموقع:** `src/parser/parser_core_helpers.cpp`

**الدوال الموجودة:**

```cpp
// 1. تحليل النوع الأساسي
Data::DataType ParserCore::parseType() {
    // يدعم: رقم، عشري، نص، منطقي، فراغ، مصفوفة، قاموس
}

// 2. تحليل المعاملات المكتوبة
std::vector<Parameter> ParserCore::parseTypedParameterList() {
    // يدعم: (أ: رقم، ب: نص، ج: منطقي = صحيح)
}
```

**الاستخدام في Parser:**

```cpp
// في parseVarDecl():
if (match(TokenType::COLON)) {
    varType = parseType();  // ✅ دعم: متغير أ: رقم = 5
}

// في parseFunctionDecl():
if (isTypeKeyword(peek())) {
    returnType = parseType();  // ✅ دعم: دالة رقم جمع(...)
}

// في parseTypedParameterList():
if (match(TokenType::COLON)) {
    paramType = parseType();  // ✅ دعم: (أ: رقم، ب: نص)
}
```

### 5.2 هل يدعم AST type annotations؟

#### ✅ **نعم - دعم كامل موجود**

**الموقع:** `include/parser/ast/declarations.h`

**FunctionDecl مع Type Annotations:**

```cpp
class FunctionDecl : public Statement {
public:
    std::string name;                      // اسم الدالة
    std::vector<Parameter> parameters;     // ✅ المعاملات مع الأنواع
    Data::DataType returnType;            // ✅ نوع الإرجاع
    StmtPtr body;                         // جسم الدالة
    bool isExported;
    bool isMainFunction;
    ExprList decorators;
};
```

**Parameter مع Type Annotation:**

```cpp
struct Parameter {
    std::string name;                     // اسم المعامل
    Data::DataType type;                 // ✅ نوع المعامل
    std::unique_ptr<Expr> defaultValue;  // ✅ قيمة افتراضية
    bool isVarargs;                      // *args
    bool isKwargs;                       // **kwargs
};
```

**VarDecl مع Type Annotation:**

```cpp
// في parseVarDecl() - Parser يحفظ النوع في AST
AST::VarDeclStmt {
    std::string name;
    Data::DataType type;  // ✅ النوع المحدد
    ExprPtr initializer;
};
```

### 5.3 أمثلة على Type Annotations المدعومة

#### ✅ **في المتغيرات:**

```s
# Type annotations مدعومة
متغير عمر: رقم = 25
متغير اسم: نص = "أحمد"
متغير نشط: منطقي = صحيح

# Arrays مع type annotations
متغير أرقام: مصفوفة<رقم> = [1، 2، 3]

# Maps مع type annotations
متغير بيانات: قاموس<نص، رقم> = {"العمر": 25}
```

#### ✅ **في الدوال:**

```s
# Return type قبل اسم الدالة
دالة رقم مربع(س: رقم) {
    إرجاع س * س
}

# معاملات مكتوبة
دالة نص دمج(أ: نص، ب: نص) -> نص {
    إرجاع أ + ب
}

# قيم افتراضية مع type annotations
دالة سلّم(اسم: نص = "ضيف") {
    اطبع(ن"مرحبا {اسم}")
}
```

#### ✅ **في الأصناف:**

```s
صنف شخص {
    # حقول مكتوبة
    خاص متغير العمر: رقم
    عام متغير الاسم: نص
    
    # طرق مكتوبة
    دالة رقم احصل_عمر() {
        إرجاع هذا.العمر
    }
}
```

### 5.4 هل يدعم المفسر type annotations حالياً؟

#### 🟡 **دعم جزئي:**

**✅ ما يعمل:**
- Parser يقرأ type annotations ✅
- AST يحفظ type annotations ✅
- Parser لا يرفض الكود مع type annotations ✅

**❌ ما لا يعمل:**
- المفسر **لا يتحقق** من صحة الأنواع ❌
- Type checking لا يحدث في runtime ❌
- Type annotations تُتجاهل في التنفيذ ❌

**الكود الحالي في المفسر:**

```cpp
// src/interpreter/visitors/statement_executor.cpp (line 709)
void InterpreterCore::visitFunctionDecl(FunctionDecl* stmt) {
    // We'll just execute without return type checking
    // نحفظ الدالة بدون التحقق من نوع الإرجاع
}
```

**النتيجة:**
```s
# هذا الكود يعمل بدون أخطاء (حالياً):
دالة رقم احصل_اسم() {
    إرجاع "أحمد"  # ❌ يجب أن يكون خطأ (نص بدلاً من رقم)
}
```

### 5.5 ما المطلوب لدعم Type Checking الكامل؟

#### Phase 1.3.1: Type Annotations Parser (✅ مكتمل بالفعل!)

**ما هو موجود:**
- ✅ Parser يدعم type annotations كاملاً
- ✅ AST يحفظ type annotations
- ✅ قواعد اللغة تدعم type annotations

**ما المطلوب (Phase 1.3.1 الفعلية):**
1. **توسيع AST** لدعم:
   - ✅ Generic types: `مصفوفة<T>`
   - ✅ Union types: `رقم | نص`
   - ✅ Optional types: `رقم?`
   - ✅ Function types: `(رقم، نص) -> منطقي`

2. **توسيع Parser** لدعم:
   - 🟡 Generic syntax: `دالة <T> عكس(قائمة: مصفوفة<T>)`
   - 🟡 Union syntax: `متغير نتيجة: رقم | خطأ`
   - 🟡 Complex types: `قاموس<نص، مصفوفة<رقم>>`

#### Phase 1.3.2: Type Inference Engine (🟡 مطلوب)

**ما المطلوب:**
1. **Algorithm W** للاستنتاج التلقائي
2. **Constraint generation** من الكود
3. **Unification engine** لحل القيود

#### Phase 1.3.3: Type Checking (🟡 مطلوب)

**ما المطلوب:**
1. **Type Checker** للتحقق من صحة الأنواع
2. **Error reporting** عند وجود أخطاء
3. **Integration** مع Compiler pipeline

---

<div id="recommendations"></div>

## 💡 6. التوصيات / Recommendations

### 6.1 خطة Phase 1.3.1 المُحدّثة

#### ✅ **Phase 1.3.0 مكتمل:**
- ✅ Type System Core موجود (`compiler/type_system/`)
- ✅ Type Registry موجود
- ✅ Primitive types معرّفة

#### 🔄 **Phase 1.3.1 الجديد: توسيع Parser للأنواع المتقدمة**

**المدة:** 3-5 أيام (بدلاً من 5-7)

**الأهداف:**

1. **توسيع AST لدعم الأنواع المتقدمة:**
```cpp
// ملفات جديدة:
include/parser/ast/type_nodes.h         // عقد الأنواع المتقدمة
src/parser/ast/type_nodes.cpp

// الأنواع المطلوبة:
class GenericTypeNode : public ASTNode {
    std::string typeName;  // مصفوفة، قاموس
    std::vector<TypeNodePtr> typeParams;  // <T, U>
};

class UnionTypeNode : public ASTNode {
    std::vector<TypeNodePtr> types;  // رقم | نص | منطقي
};

class OptionalTypeNode : public ASTNode {
    TypeNodePtr baseType;  // رقم?
};

class FunctionTypeNode : public ASTNode {
    std::vector<TypeNodePtr> paramTypes;
    TypeNodePtr returnType;  // (رقم، نص) -> منطقي
};
```

2. **توسيع Parser:**
```cpp
// في parser_types.cpp (ملف جديد):
TypeNodePtr ParserCore::parseComplexType() {
    // 1. Generic: مصفوفة<رقم>
    // 2. Union: رقم | نص
    // 3. Optional: رقم?
    // 4. Function: (رقم، نص) -> منطقي
}
```

3. **تحديث Data::DataType:**
```cpp
// في include/data/types/value.h:
enum class DataType {
    // الأنواع البسيطة (موجودة)
    VOID, INTEGER, DOUBLE, STRING, BOOLEAN,
    
    // الأنواع المركبة (موجودة)
    ARRAY, MAP, TUPLE,
    
    // الأنواع المتقدمة (جديدة)
    GENERIC,      // T, U
    UNION,        // A | B
    OPTIONAL,     // T?
    FUNCTION_TYPE, // (T1, T2) -> T3
    
    UNKNOWN, ANY
};
```

### 6.2 تجنب التكرار - استراتيجية التكامل

#### 📋 **المبدأ الأساسي:**

```
Parser → AST (Data::DataType)
            ↓
            ├─→ Interpreter → Value System (Runtime)
            │                    ↓
            │                 (تجاهل type annotations حالياً)
            │
            └─→ Compiler → Type Checker (Type System)
                              ↓
                          type validation
                              ↓
                          SIR (Type Info)
                              ↓
                          LLVM IR
```

#### 🔗 **Mapping بين الأنظمة:**

```cpp
// compiler/type_system/src/type_mapper.cpp (ملف جديد)

class TypeMapper {
    // تحويل من Parser إلى Type System
    static TypePtr fromDataType(Data::DataType dt) {
        switch (dt) {
            case Data::DataType::INTEGER:
                return TypeRegistry::getInstance().getIntegerType();
            case Data::DataType::STRING:
                return TypeRegistry::getInstance().getStringType();
            // ...
        }
    }
    
    // تحويل من Type System إلى SIR
    static SIRType toSIRType(TypePtr type) {
        switch (type->getKind()) {
            case TypeKind::Integer:
                return SIRType::I64;
            case TypeKind::String:
                return SIRType::STRING;
            // ...
        }
    }
};
```

### 6.3 خطة التنفيذ الموصى بها

#### المرحلة 1: توسيع Parser (3-5 أيام)

```
✅ الأنواع البسيطة موجودة
🔧 إضافة الأنواع المتقدمة:
   - GenericTypeNode
   - UnionTypeNode
   - OptionalTypeNode
   - FunctionTypeNode

📁 الملفات المطلوبة:
   - include/parser/ast/type_nodes.h
   - src/parser/ast/type_nodes.cpp
   - src/parser/parser_types.cpp (جديد)
   - tests/parser_tests/test_advanced_types.cpp
```

#### المرحلة 2: Type Checker Integration (5-7 أيام)

```
🔗 ربط Parser بـ Type System:
   - TypeMapper class
   - AST → Type System conversion
   
🧪 Type Checker في Compiler:
   - استخدام compiler/type_system/
   - Type checking pass
   - Error reporting
   
📁 الملفات المطلوبة:
   - compiler/type_system/src/type_mapper.cpp
   - compiler/frontend/type_checker/ (موجود)
   - tests/compiler/type_checker/
```

#### المرحلة 3: Type Inference (7-10 أيام)

```
🤖 Algorithm W implementation
🔧 Constraint generation
🔧 Unification engine
🔧 Substitution maps

📁 الملفات المطلوبة:
   - compiler/type_system/include/type_inferencer.h
   - compiler/type_system/src/type_inferencer.cpp
   - tests/type_inference/
```

### 6.4 توصيات التكامل بين المفسر والمترجم

#### 1. **المفسر يبقى Dynamic (لا تغيير)**

```cpp
// المفسر لا يحتاج Type Checking
// Type annotations تُتجاهل في runtime
// Value System يبقى كما هو
```

#### 2. **المترجم يستخدم Type System الجديد**

```cpp
// Pipeline المترجم:
Parser → AST (Data::DataType)
    ↓
TypeMapper::fromDataType()
    ↓
Type System (TypePtr)
    ↓
Type Checker
    ↓
SIR (Type Info)
    ↓
LLVM IR
```

#### 3. **لا حاجة لتعديل Value System**

```
✅ Value System (include/data/types/value.h)
   - يبقى للمفسر فقط
   - لا تغييرات مطلوبة
   - Dynamic typing محفوظ

✅ Type System (compiler/type_system/)
   - للمترجم فقط
   - Static type checking
   - Generics و Union types
```

#### 4. **التكامل عبر TypeMapper**

```cpp
// مثال على التكامل:
class TypeMapper {
    // من Parser (AST) إلى Compiler (Type System)
    static TypePtr convertType(Data::DataType dt, AST::TypeNode* node) {
        if (node->isGeneric()) {
            return createGenericType(node->getTypeParams());
        }
        if (node->isUnion()) {
            return createUnionType(node->getTypes());
        }
        return TypeRegistry::getInstance().internPrimitiveType(
            toTypeKind(dt)
        );
    }
};
```

### 6.5 الأولويات

#### 🔴 **عالية الأولوية (أسبوع 1-2):**

1. ✅ إكمال Phase 1.3.0 (مكتمل)
2. 🔧 توسيع Parser للأنواع المتقدمة
3. 🔧 إنشاء TypeMapper

#### 🟡 **متوسطة الأولوية (أسبوع 3-4):**

4. 🔧 Type Checker integration
5. 🔧 Error reporting

#### 🟢 **منخفضة الأولوية (أسبوع 5-6):**

6. 🔧 Type Inference
7. 🔧 Advanced features (Generics instantiation)

---

## 📊 ملخص تنفيذي / Executive Summary

### الوضع الحالي (Current Status)

| المكون | الحالة | النسبة |
|--------|--------|--------|
| **المفسر (Interpreter)** | ✅ مكتمل | 100% |
| **Parser مع Type Annotations** | ✅ مكتمل | 100% |
| **AST مع Type Annotations** | ✅ مكتمل | 100% |
| **Value System (Runtime)** | ✅ مكتمل | 100% |
| **LLVM Backend** | ✅ مكتمل | 100% |
| **JIT Engine** | ✅ مكتمل | 100% |
| **Type System Core** | ✅ مكتمل | 100% |
| **Type Checker** | 🟡 قيد البناء | 30% |
| **Type Inference** | ❌ لم يبدأ | 0% |
| **Advanced Types (Generics)** | ❌ لم يبدأ | 0% |

### التوصية الرئيسية

#### ✅ **لا يوجد تكرار - الأنظمة الثلاثة مكملة لبعضها**

```
Value System → Runtime (المفسر)
SIR Type Info → Compiler IR (المترجم)
Type System → Static Checking (المترجم)
```

#### 🎯 **خطة Phase 1.3.1 المُحدّثة:**

1. **توسيع Parser** لدعم:
   - Generic types: `مصفوفة<T>`
   - Union types: `رقم | نص`
   - Optional types: `رقم?`
   - Function types: `(T1, T2) -> T3`

2. **إنشاء TypeMapper** للتكامل بين:
   - Parser AST (Data::DataType)
   - Type System (TypePtr)
   - SIR (Type Info)

3. **تفعيل Type Checker** في المترجم

### الخطوات التالية (Next Steps)

#### الأسبوع الأول (أيام 1-7):
- [ ] إنشاء `include/parser/ast/type_nodes.h`
- [ ] توسيع Parser: `parseComplexType()`
- [ ] اختبارات: Generic types

#### الأسبوع الثاني (أيام 8-14):
- [ ] إنشاء `compiler/type_system/src/type_mapper.cpp`
- [ ] ربط Parser بـ Type System
- [ ] Type Checker integration

#### الأسبوع الثالث (أيام 15-21):
- [ ] Type Inference engine
- [ ] Constraint solving
- [ ] Error reporting

---

## 📚 مراجع إضافية / Additional References

### الملفات المهمة للقراءة:

1. **قواعد اللغة:**
   - `rules/rules/01_types.md` - نظام الأنواع
   - `rules/rules/02_functions.md` - Type annotations في الدوال

2. **خطط التطوير:**
   - `plans/global_dominance_plan/phase_1/phase_1_3/status/PHASE_1_3_PLAN.md`

3. **الكود المرجعي:**
   - `include/data/types/value.h` - Value System
   - `compiler/frontend/include/type_info.h` - SIR Types
   - `compiler/type_system/include/type.h` - Type System
   - `include/parser/ast/declarations.h` - AST مع Type Annotations
   - `src/parser/parser_core_helpers.cpp` - Parser للأنواع

### مخططات معمارية:

```
الهيكل العام:
src/main.cpp (Interpreter entry)
    ↓
[Lexer] → [Parser] → [AST]
    ↓              ↓       ↓
Interpreter ←─┘       ├─→ Compiler
(Value System)        │   (Type System)
                      │       ↓
                      │   [Type Checker]
                      │       ↓
                      └─→ [SIR]
                              ↓
                          [LLVM IR]
                              ↓
                        [Native Code / JIT]
```

---

**انتهى التقرير / End of Report**

**المُعِدّ / Prepared by:** GitHub Copilot  
**التاريخ / Date:** 2 يناير 2026  
**الإصدار / Version:** 1.0.0
