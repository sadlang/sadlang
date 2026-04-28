# مخططات Mermaid للبنية المعمارية

> ملحق لـ [compiler_architecture_phase10.md](compiler_architecture_phase10.md)

---

## 1. مخطط الطبقات الكلي للمترجم

```mermaid
flowchart TD
    A[" مصدر .ص "] --> B[" LexerCore<br/>shared/lexer "]
    B --> C[" ParserCore<br/>shared/parser "]
    C --> D[" AST<br/>shared/ast "]
    D --> E[" SIRBuilder<br/>compiler/src/sir "]
    E --> F[" SIROptimizer<br/>12 ownership opcodes "]
    F --> G[" LLVMCodeGen<br/>compiler/src/backend/llvm "]
    G --> H[" LLVM IR Module "]
    H --> I[" LLVM Optimizer<br/>O0/O1/O2/O3 "]
    I --> J[" Object File .o "]
    J --> K[" Linker "]
    K --> L[" Native Executable "]

    style G fill:#ffeaa7,stroke:#fdcb6e,stroke-width:3px
    style L fill:#55efc4,stroke:#00b894,stroke-width:2px
```

---

## 2. البنية الداخلية لـ LLVMCodeGen (8 فئات / 36 sub-codegen)

```mermaid
flowchart TB
    LCG["<b>LLVMCodeGen</b><br/><i>: public LLVMCodeGenContext</i><br/>context_, module_, builder_,<br/>typeMapper_, errors_,<br/>reportError(), getLLVMValue()"]

    LCG --> CORE["<b>core/</b> 4 sub-codegens"]
    LCG --> ARITH["<b>arithmetic/</b> 3"]
    LCG --> MEM["<b>memory/</b> 3"]
    LCG --> OOP["<b>oop/</b> 5"]
    LCG --> COLL["<b>collections/</b> 5"]
    LCG --> BUILT["<b>builtins/</b> 5"]
    LCG --> PLAT["<b>platform/</b> 7"]
    LCG --> DIR["<b>directives/</b> 4"]

    CORE --> C1["instr_core"]
    CORE --> C2["output"]
    CORE --> C3["types"]
    CORE --> C4["freestanding"]

    ARITH --> A1["arithmetic"]
    ARITH --> A2["controlflow"]
    ARITH --> A3["exception"]

    MEM --> M1["memory"]
    MEM --> M2["closure"]
    MEM --> M3["aggregate_ops"]

    OOP --> O1["classes_vtables"]
    OOP --> O2["oop_ops"]
    OOP --> O3["objects_arrays"]
    OOP --> O4["enum_ops"]
    OOP --> O5["functions"]

    COLL --> L1["array_ops"]
    COLL --> L2["array_builtins"]
    COLL --> L3["map_ops"]
    COLL --> L4["string_ops"]
    COLL --> L5["strings"]

    BUILT --> B1["builtin_funcs"]
    BUILT --> B2["io_builtins"]
    BUILT --> B3["math_builtins"]
    BUILT --> B4["network_builtins"]
    BUILT --> B5["security_builtins"]

    PLAT --> P1["instr_platform"]
    PLAT --> P2["instr_lowlevel"]
    PLAT --> P3["lowlevel"]
    PLAT --> P4["ui"]
    PLAT --> P5["file_casts"]
    PLAT --> P6["hardware_ffi"]
    PLAT --> P7["ffi_remain"]

    DIR --> D1["directives"]
    DIR --> D2["simd"]
    DIR --> D3["concurrency"]
    DIR --> D4["coroutines"]

    style LCG fill:#ffeaa7,stroke:#e17055,stroke-width:3px
    style CORE fill:#a29bfe,color:#fff
    style ARITH fill:#fd79a8,color:#fff
    style MEM fill:#74b9ff,color:#fff
    style OOP fill:#55efc4
    style COLL fill:#ffeaa7
    style BUILT fill:#fab1a0
    style PLAT fill:#81ecec
    style DIR fill:#dfe6e9
```

---

## 3. تدفق توليد تعليمة SIR واحدة

```mermaid
sequenceDiagram
    participant Caller as LLVMCodeGen::generate()
    participant Core as instr_core_codegen
    participant Sub as XCodeGen<br/>(arithmetic/oop/...)
    participant CG as cg_<br/>(LLVMCodeGen ref)
    participant LLVM as LLVM IRBuilder

    Caller->>Core: generateInstruction(inst)
    Core->>Core: switch(inst.opcode)

    alt opcode == ADD
        Core->>Sub: arithmetic_->generateAdd(inst)
    else opcode == NEW_OBJECT
        Core->>Sub: oop_ops_->generateNew(inst)
    else opcode == GO
        Core->>Sub: concurrency_->generateGo(inst)
    end

    Sub->>CG: cg_.getLLVMValue(operand)
    CG-->>Sub: llvm::Value*
    Sub->>CG: cg_.builder_
    CG-->>Sub: IRBuilder ref
    Sub->>LLVM: CreateAdd / CreateCall / ...
    LLVM-->>Sub: llvm::Value* result
    Sub-->>Core: result
    Core-->>Caller: result
```

---

## 4. علاقات الاعتمادية (Dependencies)

```mermaid
flowchart LR
    subgraph CTX[" LLVMCodeGenContext (struct, public fields) "]
        C1["context_"]
        C2["module_"]
        C3["builder_"]
        C4["typeMapper_"]
        C5["errors_"]
        C6["reportError()"]
    end

    subgraph LCG[" LLVMCodeGen : public LLVMCodeGenContext "]
        L1["arithmetic_"]
        L2["types_"]
        L3["oop_ops_"]
        L4["...33 آخرون"]
    end

    subgraph SUB[" XCodeGen (مثال: ArithmeticCodeGen) "]
        S1["LLVMCodeGen &cg_"]
        S2["generateAdd()"]
        S3["generateMul()"]
    end

    CTX -.وراثة.-> LCG
    LCG -.تملك.-> S1
    S1 -.وصول.-> CTX

    style CTX fill:#dfe6e9
    style LCG fill:#ffeaa7
    style SUB fill:#74b9ff,color:#fff
```

**ملاحظات:**
- `LLVMCodeGenContext` يحوي **حقول public فقط** — لا توجد `friend class` بعد Phase 8
- `LLVMCodeGen` يرث منه ويضيف 36 `unique_ptr<XCodeGen>`
- كل sub-codegen يحوي `LLVMCodeGen &cg_` ويصل لكل شيء عبره

---

## 5. هيكل المجلدات الفعلي (شجرة)

```mermaid
flowchart TD
    R["compiler/{include,src}/backend/llvm/builders/"] --> CORE["📁 core/"]
    R --> ARITH["📁 arithmetic/"]
    R --> MEM["📁 memory/"]
    R --> OOP["📁 oop/"]
    R --> COLL["📁 collections/"]
    R --> BUILT["📁 builtins/"]
    R --> PLAT["📁 platform/"]
    R --> DIR["📁 directives/"]

    CORE --> CORE_FILES["instr_core, output,<br/>types, freestanding<br/>(7 ops files)"]
    ARITH --> ARITH_FILES["arithmetic, controlflow,<br/>exception<br/>(8 ops files)"]
    MEM --> MEM_FILES["memory, closure,<br/>aggregate_ops<br/>(5 ops files)"]
    OOP --> OOP_FILES["classes_vtables, oop_ops,<br/>objects_arrays, enum_ops,<br/>functions (6 ops files)"]
    COLL --> COLL_FILES["array_ops, array_builtins,<br/>map_ops, string_ops,<br/>strings (7 ops files)"]
    BUILT --> BUILT_FILES["builtin_funcs, io, math,<br/>network, security<br/>(6 ops files)"]
    PLAT --> PLAT_FILES["instr_platform, instr_lowlevel,<br/>lowlevel, ui, file_casts,<br/>hardware_ffi, ffi_remain<br/>(9 ops files)"]
    DIR --> DIR_FILES["directives, simd,<br/>concurrency, coroutines<br/>(5 ops files)"]
```

---

## 6. توزيع الأسطر حسب الفئة

```mermaid
pie title توزيع 30,976 سطر على 8 فئات
    "platform" : 5655
    "oop" : 4746
    "collections" : 4365
    "arithmetic" : 4046
    "core" : 3715
    "builtins" : 3262
    "directives" : 3005
    "memory" : 2710
```

---

## 7. مراحل التطور (Phases Timeline)

```mermaid
gantt
    title تطور بنية مولد LLVM
    dateFormat  X
    axisFormat  Phase %s

    section الاستخراج
    Phase 6 ownership opcodes      :p6, 0, 1
    Phase 7 first sub-codegens     :p7, 1, 1
    Phase 8 step 1-9 (25 sub)      :p8a, 2, 1
    Phase 8 step 10 mega           :p8b, 3, 1

    section التنظيف
    Phase 8 cleanup (friends)      :p8c, 4, 1
    Phase 9 TypesCodeGen extract   :p9, 5, 1

    section إعادة التنظيم
    Phase 10 8 subfolders          :crit, p10, 6, 1
    Phase 10 doc + diagrams        :active, p10d, 7, 1
```

---

**مرجع البيانات:** [compiler_architecture_phase10.md](compiler_architecture_phase10.md) §6 (الإحصائيات الكاملة)
