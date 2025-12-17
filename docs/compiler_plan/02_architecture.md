# البنية المعمارية للمترجم
# Compiler Architecture

**التاريخ / Date:** 2 ديسمبر 2025  
**الإصدار / Version:** 1.0

---

## 📐 نظرة عامة معمارية / Architecture Overview

### (AR) العربية

المترجم يتبع بنية **Multi-Pass Multi-Backend** كلاسيكية:

```
┌─────────────────────────────────────────────────────────────┐
│                    المصدر / Source Code (.s)                │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                   FRONTEND (التحليل)                         │
│  ┌──────────┐  ┌──────────┐  ┌────────────┐  ┌───────────┐│
│  │  Lexer   │→ │  Parser  │→ │ Type Check │→ │ Semantic  ││
│  │ المحلل   │  │  المحلل  │  │   فحص      │  │   دلالي   ││
│  │  اللفظي  │  │  النحوي  │  │  الأنواع   │  │           ││
│  └──────────┘  └──────────┘  └────────────┘  └───────────┘│
│                           │                                  │
│                           ▼                                  │
│                 ┌──────────────────┐                        │
│                 │   AST (شجرة)     │                        │
│                 └──────────────────┘                        │
└────────────────────────┬────────────────────────────────────┘
                         │
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                  MIDDLE-END (التحسين)                        │
│  ┌────────────┐  ┌────────────┐  ┌──────────────────────┐  │
│  │  SIR Gen   │→ │ Optimizer  │→ │  Control Flow       │  │
│  │  مولد SIR  │  │  محسّن     │  │  تحليل التدفق       │  │
│  └────────────┘  └────────────┘  └──────────────────────┘  │
│                           │                                  │
│                           ▼                                  │
│                 ┌──────────────────┐                        │
│                 │   SIR (وسيط)     │                        │
│                 └──────────────────┘                        │
└────────────────────────┬────────────────────────────────────┘
                         │
         ┌───────────────┴───────────────┐
         │                               │
         ▼                               ▼
┌──────────────────┐          ┌──────────────────────┐
│ BACKEND: Bytecode│          │  BACKEND: LLVM IR    │
│   ┌──────────┐   │          │   ┌──────────────┐  │
│   │ Emitter  │   │          │   │ IR Generator │  │
│   └────┬─────┘   │          │   └──────┬───────┘  │
│        │         │          │          │          │
│        ▼         │          │          ▼          │
│   ┌──────────┐   │          │   ┌──────────────┐  │
│   │ .sbc     │   │          │   │  LLVM Opts   │  │
│   └────┬─────┘   │          │   └──────┬───────┘  │
│        │         │          │          │          │
│        ▼         │          │          ▼          │
│   ┌──────────┐   │          │   ┌──────────────┐  │
│   │  sadvm   │   │          │   │   Native     │  │
│   │  (VM)    │   │          │   │   (ELF/PE)   │  │
│   └──────────┘   │          │   └──────────────┘  │
└──────────────────┘          └──────────────────────┘
         │                               │
         └───────────────┬───────────────┘
                         │
                         ▼
                ┌─────────────────┐
                │   Runtime       │
                │   (libsad.so)   │
                └─────────────────┘
```

### (EN) English

The compiler follows a classic **Multi-Pass Multi-Backend** architecture as shown in the diagram above.

---

## 🏗️ المكونات الرئيسية / Main Components

### 1️⃣ Frontend (الواجهة الأمامية)

#### (AR) العربية

**المسؤوليات:**
- تحليل الكود المصدري (.s files)
- بناء شجرة AST كاملة
- التحقق من الأنواع والدلالات
- إدارة جدول الرموز
- إنتاج رسائل خطأ واضحة

**المكونات الفرعية:**

##### 1. Lexer (المحلل اللفظي)
```cpp
// compiler/frontend/lexer.h
class Lexer {
    # تحويل النص إلى tokens
    دالة tokenize(نص source) -> مصفوفة<Token>
    
    # دعم Unicode (عربي + إنجليزي)
    دالة is_arabic_identifier(نص ch) -> منطقي
    
    # معالجة التعليقات # و #* *#
    دالة skip_comment() -> فارغ
نهاية
```

**Token Types:**
- Keywords: دالة، إذا، بينما، إرجاع، etc.
- Identifiers: متغير_1، variable_name
- Literals: 123، "نص"، true
- Operators: +، -،*،/،==، etc.
- Delimiters: (، )، نهاية، etc.

##### 2. Parser (المحلل النحوي)
```cpp
// compiler/frontend/parser.h
class Parser {
    # بناء AST من tokens
    دالة parse() -> مؤشر<ASTNode>
    
    # معالجة التعابير
    دالة parse_expression() -> مؤشر<Expression>
    
    # معالجة التصريحات
    دالة parse_statement() -> مؤشر<Statement>
    
    # معالجة الدوال
    دالة parse_function() -> مؤشر<FunctionDecl>
نهاية
```

**AST Node Types:**
- Program
- FunctionDecl
- ClassDecl
- VariableDecl
- IfStatement
- WhileLoop
- ForLoop
- ReturnStatement
- BinaryExpression
- CallExpression

##### 3. Type Checker (فاحص الأنواع)
```cpp
// compiler/frontend/type_checker.h
class TypeChecker {
    # التحقق من الأنواع
    دالة check(مؤشر<ASTNode> ast) -> منطقي
    
    # استنتاج الأنواع
    دالة infer_type(مؤشر<Expression> expr) -> Type
    
    # التحقق من التوافق
    دالة is_compatible(Type a, Type b) -> منطقي
نهاية
```

**Type System:**
- Primitive: رقم، نص، منطقي
- Compound: مصفوفة<T>، قاموس<K،V>
- Pointers: مؤشر<T>
- Functions: دالة<T(Args)>
- Generics: صنف<T>

##### 4. Semantic Analyzer (المحلل الدلالي)
```cpp
// compiler/frontend/semantic.h
class SemanticAnalyzer {
    # تحليل دلالي
    دالة analyze(مؤشر<ASTNode> ast) -> منطقي
    
    # التحقق من الوصول
    دالة check_reachability() -> فارغ
    
    # التحقق من التهيئة
    دالة check_initialization() -> فارغ
نهاية
```

#### (EN) English

Same component breakdown with code examples.

---

### 2️⃣ Middle-end (الطبقة الوسطى)

#### (AR) العربية

**المسؤوليات:**
- تحويل AST إلى SIR (Simple IR)
- تحسينات مستقلة عن المنصة
- تحليل التدفق والبيانات
- إزالة الكود الميت

**SIR Design (Simple Intermediate Representation):**

```
# SIR هو Three-Address Code بسيط

# مثال: a = b + c * d
t1 = mul c, d
t2 = add b, t1
a = t2

# أنواع التعليمات:
- Arithmetic: add, sub, mul, div, mod
- Bitwise: and, or, xor, not, shl, shr
- Comparison: eq, ne, lt, le, gt, ge
- Control Flow: jmp, br, call, ret
- Memory: load, store, alloc, free
- Special: phi (SSA form)
```

**SIR Structure:**
```cpp
// compiler/ir/sir.h
class SIRInstruction {
    نص opcode          # add, sub, mul, etc.
    مصفوفة<نص> operands # [dest, src1, src2]
    نص type            # رقم، نص، etc.
نهاية

class SIRFunction {
    نص name
    مصفوفة<SIRInstruction> instructions
    مصفوفة<نص> parameters
    نص return_type
نهاية

class SIRModule {
    مصفوفة<SIRFunction> functions
    مصفوفة<SIRGlobal> globals
نهاية
```

**Optimization Passes:**
```cpp
// compiler/ir/optimizer.h
class Optimizer {
    # إزالة الكود الميت
    دالة eliminate_dead_code(مؤشر<SIRModule> module) -> فارغ
    
    # تبسيط الثوابت
    دالة constant_folding(مؤشر<SIRModule> module) -> فارغ
    
    # نشر الثوابت
    دالة constant_propagation(مؤشر<SIRModule> module) -> فارغ
    
    # دمج الدوال
    دالة inline_functions(مؤشر<SIRModule> module) -> فارغ
نهاية
```

#### (EN) English

Same IR design and optimization passes.

---

### 3️⃣ Backend: Bytecode (مولد البايت كود)

#### (AR) العربية

**المسؤوليات:**
- تحويل SIR إلى bytecode
- إنتاج ملفات .sbc (Sad Bytecode)
- VM implementation للتنفيذ

**Bytecode Format:**
```
# تصميم Bytecode بسيط stack-based

Opcode  | Mnemonic    | Operands       | Description
--------|-------------|----------------|------------------
0x01    | PUSH        | value          | Push to stack
0x02    | POP         | -              | Pop from stack
0x10    | ADD         | -              | a + b
0x11    | SUB         | -              | a - b
0x12    | MUL         | -              | a * b
0x13    | DIV         | -              | a / b
0x20    | LOAD        | offset         | Load local
0x21    | STORE       | offset         | Store local
0x22    | LOAD_GLOBAL | index          | Load global
0x23    | STORE_GLOBAL| index          | Store global
0x30    | JMP         | address        | Unconditional jump
0x31    | JMP_IF      | address        | Jump if true
0x32    | JMP_IF_NOT  | address        | Jump if false
0x40    | CALL        | func_index     | Call function
0x41    | RET         | -              | Return
0x50    | ALLOC       | size           | Allocate memory
0x51    | FREE        | -              | Free memory
```

**VM Architecture:**
```cpp
// vm/vm.h
class SadVM {
    # Stack للعمليات
    مصفوفة<قيمة> stack
    
    # Call frames
    مصفوفة<Frame> frames
    
    # Heap للذاكرة الديناميكية
    Heap heap
    
    # تنفيذ bytecode
    دالة execute(مصفوفة<بايت> bytecode) -> قيمة
    
    # معالجة التعليمات
    دالة dispatch(Opcode op) -> فارغ
نهاية
```

**File Format (.sbc):**
```
┌──────────────────────────┐
│ Magic Number (4 bytes)   │ "SADC"
├──────────────────────────┤
│ Version (2 bytes)        │ 0x0001
├──────────────────────────┤
│ Flags (2 bytes)          │
├──────────────────────────┤
│ Constants Pool Size      │
├──────────────────────────┤
│ Constants Pool           │
│   - Numbers              │
│   - Strings              │
│   - etc.                 │
├──────────────────────────┤
│ Functions Count          │
├──────────────────────────┤
│ Function Table           │
│   - Name                 │
│   - Parameters           │
│   - Bytecode             │
└──────────────────────────┘
```

#### (EN) English

Same bytecode design and VM architecture.

---

### 4️⃣ Backend: LLVM (مولد الكود الأصلي)

#### (AR) العربية

**المسؤوليات:**
- تحويل SIR إلى LLVM IR
- استخدام LLVM optimizations
- إنتاج native executables
- دعم cross-compilation

**LLVM IR Generation:**
```cpp
// compiler/backends/llvm/codegen.h
class LLVMCodeGen {
    # سياق LLVM
    مؤشر<llvm::LLVMContext> context
    
    # المودول
    مؤشر<llvm::Module> module
    
    # البناء
    مؤشر<llvm::IRBuilder> builder
    
    # توليد IR
    دالة generate(مؤشر<SIRModule> sir) -> مؤشر<llvm::Module>
    
    # توليد دالة
    دالة emit_function(مؤشر<SIRFunction> func) -> مؤشر<llvm::Function>
نهاية
```

**Compilation Pipeline:**
```
SIR → LLVM IR → LLVM Opts → Assembly → Object → Executable

# أوامر التنفيذ:
sadc program.s -o program.sbc         # Bytecode
sadc program.s -o program --llvm      # Native (LLVM)
sadc program.s -o program.o --emit-obj # Object file
```

**Optimization Levels:**
- `-O0`: بدون تحسينات (للتطوير)
- `-O1`: تحسينات أساسية
- `-O2`: تحسينات عادية (default)
- `-O3`: تحسينات قصوى
- `-Os`: تحسين الحجم

#### (EN) English

Same LLVM integration details.

---

### 5️⃣ Runtime System (نظام التشغيل)

#### (AR) العربية

**المكونات:**

##### 1. Memory Management
```cpp
// runtime/memory.h
class MemoryManager {
    # تخصيص
    دالة allocate(رقم size) -> مؤشر<فارغ>
    
    # تحرير
    دالة deallocate(مؤشر<فارغ> ptr) -> فارغ
    
    # GC (اختياري)
    دالة collect() -> فارغ
نهاية
```

##### 2. FFI (Foreign Function Interface)
```cpp
// runtime/ffi.h
class FFI {
    # استدعاء دالة C
    دالة call_c_function(نص name, مصفوفة<قيمة> args) -> قيمة
    
    # تحميل مكتبة
    دالة load_library(نص path) -> مؤشر<Library>
نهاية
```

##### 3. Standard Library
```cpp
// runtime/stdlib.h
# الدوال المدمجة
دالة اطبع(نص msg) -> فارغ
دالة اقرأ() -> نص
دالة طول(مصفوفة<T> arr) -> رقم
دالة خصص_ذاكرة(رقم size) -> مؤشر<فارغ>
دالة حرر(مؤشر<فارغ> ptr) -> فارغ
```

#### (EN) English

Same runtime components.

---

## 📂 بنية المشروع / Project Structure

```
c:\s\s_language\
├── compiler\
│   ├── frontend\
│   │   ├── lexer.h / lexer.cpp
│   │   ├── parser.h / parser.cpp
│   │   ├── ast.h / ast.cpp
│   │   ├── type_checker.h / type_checker.cpp
│   │   └── semantic.h / semantic.cpp
│   ├── ir\
│   │   ├── sir.h / sir.cpp
│   │   ├── sir_builder.h / sir_builder.cpp
│   │   └── optimizer.h / optimizer.cpp
│   ├── backends\
│   │   ├── bytecode\
│   │   │   ├── emitter.h / emitter.cpp
│   │   │   └── format.h
│   │   └── llvm\
│   │       ├── codegen.h / codegen.cpp
│   │       └── jit.h / jit.cpp
│   └── driver\
│       └── sadc.cpp (main compiler driver)
├── vm\
│   ├── vm.h / vm.cpp
│   ├── stack.h / stack.cpp
│   ├── heap.h / heap.cpp
│   └── sadvm.cpp (VM driver)
├── runtime\
│   ├── memory.h / memory.cpp
│   ├── ffi.h / ffi.cpp
│   ├── stdlib.h / stdlib.cpp
│   └── libsad.cpp (runtime library)
├── tests\
│   ├── frontend\
│   ├── ir\
│   ├── bytecode\
│   ├── llvm\
│   └── integration\
└── tools\
    ├── sad-ld (linker)
    ├── sad-pkg (package manager)
    └── sad-doc (documentation)
```

---

**السابق / Previous:** [نظرة عامة وأهداف](01_overview_goals.md)  
**التالي / Next:** [المرحلة 0: التحضير](03_phase0_preparation.md)
