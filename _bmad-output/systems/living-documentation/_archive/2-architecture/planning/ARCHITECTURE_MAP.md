# 🗺️ الخريطة المعمارية الشاملة للغة ص — نقاط الدخول وتدفقات التحكم

> **الغرض:** هذه الوثيقة تجيب على السؤال الجوهري: *"كيف تبدأ اللغة بالبناء أصلاً وما هي نقطة الدخول التي يمكن من خلالها التحكم باللغة؟"*
>
> **المبدأ الحاكم:** التحكم باللغة ≠ تعديل `main.cpp`. التحكم الحقيقي يبدأ من **مصدر الحقيقة (Source of Truth — SoT)** وينحدر عبر طبقات `Lexer → Parser → AST → SIR → LLVM` وفق قاعدة CW-02 (Layered Architecture).

---

## 1️⃣ المنظور الكلي — ثلاث نقاط دخول مستقلة

```mermaid
graph TB
    subgraph SoT["🎯 (1) نقطة التحكم باللغة — Source of Truth"]
        Y1[data/language/keywords.yaml<br/>40 كلمة محجوزة ✅]
        Y2["data/language/error_messages.yaml<br/>❌ غير منفَّذ (مخطط فقط)"]
        CPP_BI[tools/sadinfo/src/builtin_data.cpp<br/>15 fn + 43 method<br/>⚠️ hardcoded C++]
        H1[shared/lexer/include/token.h<br/>TokenType enum]
        H2[shared/types/include/value.h<br/>Value + ValueType]
        H3[compiler_new/src/sir/sir_opcodes.h<br/>12 SIR opcodes]
    end

    subgraph Build["🔨 (2) نقطة دخول البناء — Build Entry"]
        CM[CMakeLists.txt الجذري]
        EX[cmake/executables.cmake]
        T1[target: sad]
        T2[target: sadc]
        T3[targets: lsp, formatter, repl...]
    end

    subgraph Runtime["⚡ (3) نقطة دخول التنفيذ — Runtime Entry"]
        MAIN[tools/compiler/main_simple.cpp<br/>int main]
        LEX[LexerCore]
        PAR[ParserCore]
        AST[AST Program]
        INT[InterpreterCore]
        VM[Bytecode VM]
    end

    SoT -.->|يُغذّي| Build
    SoT -.->|يُولِّد headers لـ| Runtime
    Build -->|يُنتج| Runtime

    CM --> EX
    EX --> T1
    EX --> T2
    EX --> T3
    T1 -.->|ELF/EXE| MAIN

    MAIN --> LEX --> PAR --> AST
    AST --> INT
    AST --> VM

    style SoT fill:#fff4e6,stroke:#ff9800,stroke-width:3px
    style Build fill:#e3f2fd,stroke:#1976d2,stroke-width:2px
    style Runtime fill:#e8f5e9,stroke:#388e3c,stroke-width:2px
```

---

## 2️⃣ نقطة التحكم باللغة (SoT) — ★ الأهم

> **القاعدة:** إذا أردت تغيير "شخصية" اللغة (كلمة، رسالة، نوع، عملية)، **ابدأ هنا**. كل شيء آخر إما مُولَّد منها أو مُستهلِك لها.

```mermaid
graph LR
    subgraph SoT_Layers["مصادر الحقيقة"]
        direction TB
        KW[(keywords.yaml<br/>40 كلمة محجوزة ✅)]
        EM[(error_messages.yaml<br/>❌ لم يُنفَّذ)]
        BI[(builtin_data.cpp<br/>15 fn + 43 method<br/>⚠️ hardcoded C++)]
        TT[token.h<br/>TokenType enum]
        VAL[value.h<br/>Value variant]
        SIR[sir_opcodes.h<br/>12 opcodes]
    end

    subgraph Generators["مولّدات الكود وأدوات الفحص"]
        GenKW[scripts/codegen/gen_keywords.py<br/>→ keywords_generated.* ✅]
        SI[tools/sadinfo CLI<br/>→ JSON/YAML stdout ✅]
    end

    subgraph Consumers["المستهلكون"]
        LEX2[Lexer]
        PAR2[Parser]
        SEM[Semantic Analyzer]
        INT2[Interpreter]
        COMP[Compiler sadc]
        LSP2[LSP Server]
        FMT[Formatter]
    end

    KW -->|python| GenKW
    KW -->|yaml-cpp| SI
    BI -->|"#include"| SI
    GenKW -->|"*_generated.h"| LEX2

    TT --> LEX2
    TT --> PAR2
    TT --> LSP2
    TT --> FMT

    VAL --> INT2
    VAL --> SEM

    SIR --> COMP

    SI -.->|JSON/YAML| LSP2
    SI -.->|JSON/YAML| FMT

    style KW fill:#c8e6c9,stroke:#2e7d32,stroke-width:2px
    style EM fill:#ffebee,stroke:#c62828,stroke-width:2px
    style BI fill:#fff3e0,stroke:#ef6c00,stroke-width:2px
    style SI fill:#c8e6c9,stroke:#2e7d32,stroke-width:2px
    style TT fill:#fff4e6,stroke:#ff9800,stroke-width:2px
    style VAL fill:#fff4e6,stroke:#ff9800,stroke-width:2px
    style SIR fill:#fff4e6,stroke:#ff9800,stroke-width:2px
```

### جدول مصادر الحقيقة الكامل

| الطبقة الدلالية | مصدر الحقيقة | المسار | يُولِّد / يُستهلَك بواسطة |
|---|---|---|---|
| **الكلمات المحجوزة (40)** | YAML | [data/language/keywords.yaml](data/language/keywords.yaml) | يُحمَّل في `KeywordTable::initialize()` داخل [shared/lexer/src/lexer_keywords.cpp](shared/lexer/src/lexer_keywords.cpp) |
| **الكلمات السياقية** | C++ مباشرة | [shared/parser/](shared/parser/) | نمط التحقق المزدوج `check(TT::KW_X) ‖ (IDENTIFIER && value=="عربي")` |
| **رسائل الأخطاء** | ❌ مخطط فقط | غير موجود — جزء من ADR-EM-3 لكن لم يُنفَّذ | لا مولِّد بعد. الرسائل حالياً hardcoded في shared/errors/ |
| **الدوال المدمجة (15)** | ⚠️ C++ | [tools/sadinfo/src/builtin_data.cpp](tools/sadinfo/src/builtin_data.cpp) | يُستهلك مباشرة بواسطة sadinfo CLI — لم يُنقل لـYAML بعد |
| **أداة الفحص العامة** | ✅ C++ + yaml-cpp | [tools/sadinfo/](tools/sadinfo/) | CLI يُخرج JSON/YAML للمحررات وCI ومولِّدات التوثيق |
| **أنواع التوكنات** | enum C++ | [shared/lexer/include/token.h](shared/lexer/include/token.h) | يُستهلَك في Parser, AST visitors, LSP semantic tokens |
| **عقد AST** | C++ classes | [shared/ast/include/](shared/ast/include/) | يُزار من InterpreterCore + SIRBuilder + Formatter |
| **قيم وقت التشغيل** | C++ variant | [shared/types/include/value.h](shared/types/include/value.h) | InterpreterCore + VM + جميع built-ins |
| **SIR opcodes (12)** | enum C++ | `compiler_new/src/sir/sir_opcodes.h` | SIRBuilder → SIROptimizer → LLVMCodeGen |
| **المكتبة القياسية** | ملفات `.ص` | [stdlib/](stdlib/) | تُحمَّل بـ `استورد` عبر ModuleResolver |
| **الدوال المضمنة (~21)** | C++ مباشرة | [shared/builtins/](shared/builtins/) | تُسجَّل تلقائياً في scope الجذري |

---

## 3️⃣ نقطة دخول البناء — كيف يُبنى المشروع

```mermaid
flowchart TD
    Start([المستخدم: cmake -S . -B build]) --> Root[CMakeLists.txt الجذري]

    Root --> Include1[cmake/llvm.cmake<br/>اكتشاف LLVM]
    Root --> Include2[cmake/platform.cmake<br/>تكوينات المنصة]
    Root --> Include3[cmake/executables.cmake]

    Include3 --> Tgt1{ENABLE_LLVM_BACKEND?}

    Tgt1 -->|نعم| BuildSadc[add_executable sadc<br/>tools/compiler/main.cpp<br/>+ 13 compiler_driver_*.cpp]
    Tgt1 -->|لا| SkipSadc[تخطّي sadc]

    Include3 --> BuildSad[add_executable sad<br/>tools/compiler/main_simple.cpp]

    BuildSad --> LinkSad[link: sad_core + sad_vm<br/>+ sad_type_system + sad_semantic<br/>+ sad_network + sad_http<br/>+ sad_websocket + sad_mobile<br/>+ sad_ui + sad_memory_policy]

    BuildSadc --> LinkSadc[link: sad_core + LLVM<br/>+ sad_codegen + sad_sir]

    LinkSad --> Out1([bin/Debug/sad.exe<br/>المفسر])
    LinkSadc --> Out2([bin/Debug/sadc.exe<br/>المترجم])

    Include3 --> OtherTgts[lsp, formatter, repl,<br/>pkg, analyze, check, hub]

    style Start fill:#e3f2fd,stroke:#1976d2
    style Out1 fill:#c8e6c9,stroke:#388e3c,stroke-width:2px
    style Out2 fill:#c8e6c9,stroke:#388e3c,stroke-width:2px
```

### أوامر البناء العملية

```powershell
# تهيئة لمرة واحدة
cmake -S . -B build

# المفسر فقط (الأسرع)
cmake --build build --config Debug --target sad

# المترجم (يتطلب LLVM 18)
cmake --build build --config Debug --target sadc

# كل شيء
cmake --build build --config Debug
```

### مفتاح الفهم: `sad_core` هو المكتبة الأم

ربط `sad` يبدأ بـ `sad_core` — وهذا يشمل lexer, parser, ast, types, errors, semantic. أي تغيير في SoT يجب أن يُعاد بناء `sad_core` ليصل إلى الـbinaries.

---

## 4️⃣ نقطة دخول التنفيذ — رحلة ملف `.ص`

```mermaid
sequenceDiagram
    actor User as المستخدم
    participant Shell as PowerShell
    participant Main as main_simple.cpp
    participant Lex as LexerCore
    participant Par as ParserCore
    participant Sem as SemanticAnalyzer
    participant Int as InterpreterCore
    participant VM as Bytecode VM

    User->>Shell: sad.exe file.ص
    Shell->>Main: int main(argc, argv)
    Main->>Main: parse CLI flags<br/>(--vm, --ownership, --gc, --debug-server...)
    Main->>Main: ErrorManager::clear() + setSourceCode
    Main->>Lex: LexerCore lex(source)
    Lex-->>Main: token stream (lazy)

    Main->>Par: ParserCore par(lex)
    Par->>Par: parseProgram()
    Par-->>Main: Program AST

    alt parser has errors
        Par->>Main: printErrors() + return 1
    end

    Main->>Sem: type-check + ownership (اختياري)
    Sem-->>Main: AST مُحلَّل

    alt --vm flag set
        Main->>VM: compile to bytecode + execute
    else default
        Main->>Int: InterpreterCore::execute(program)
        Int->>Int: AST tree-walking
    end

    Int-->>User: output / exit code
```

### الملف الواحد المفتاح: `main_simple.cpp`

[tools/compiler/main_simple.cpp](tools/compiler/main_simple.cpp) — حوالي 700 سطر، يحوي:
- تحليل الأعلام (CLI flags)
- تهيئة `ErrorManager` للتشخيصات الموحَّدة
- استدعاء سلسلة `LexerCore → ParserCore → InterpreterCore` (السطور 597-612)
- 3 أوضاع تنفيذ: المفسر الافتراضي، VM، debug server
- مسارات بديلة: `--docs` (استخراج توثيق)، `--profile` (تنميط)، `--hot-reload`

---

## 5️⃣ تدفق ميزة جديدة عبر الطبقات

> **سيناريو:** أريد إضافة كلمة مفتاحية جديدة `كرّر` (repeat). من أين أبدأ؟

```mermaid
flowchart LR
    Start([أريد كلمة كرّر]) --> S1

    subgraph Phase1["الطبقة 1: SoT"]
        S1[1. أضف entry في<br/>data/language/keywords.yaml]
        S2[2. أضف KEYWORD_REPEAT<br/>إلى token.h enum]
    end

    subgraph Phase2["الطبقة 2: Lexer"]
        S3[3. سجّل في<br/>lexer_keywords.cpp]
    end

    subgraph Phase3["الطبقة 3: AST"]
        S4[4. أنشئ RepeatStatementNode<br/>في shared/ast/include/]
    end

    subgraph Phase4["الطبقة 4: Parser"]
        S5[5. أضف قاعدة parseRepeat<br/>في shared/parser/]
    end

    subgraph Phase5["الطبقة 5: Visitors"]
        S6[6. interpreter visitor]
        S7[7. compiler/SIR visitor]
        S8[8. LSP semantic tokens]
        S9[9. formatter]
    end

    subgraph Phase6["الطبقة 6: التحقق"]
        S10[10. اختبارات في<br/>tests/comprehensive/]
        S11[11. توثيق في<br/>docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md]
    end

    S1 --> S2 --> S3 --> S4 --> S5
    S5 --> S6
    S5 --> S7
    S5 --> S8
    S5 --> S9
    S6 --> S10
    S7 --> S10
    S10 --> S11

    style Phase1 fill:#fff4e6,stroke:#ff9800
    style Phase2 fill:#e3f2fd,stroke:#1976d2
    style Phase3 fill:#f3e5f5,stroke:#7b1fa2
    style Phase4 fill:#e8f5e9,stroke:#388e3c
    style Phase5 fill:#fce4ec,stroke:#c2185b
    style Phase6 fill:#fffde7,stroke:#fbc02d
```

> **القاعدة الذهبية (CW-02):** كل طبقة تعتمد فقط على ما تحتها. لا تقفز ولا تعكس الاتجاه.

---

## 6️⃣ المنتجات الثلاثة وعلاقتها بنقاط الدخول

```mermaid
graph TB
    subgraph Products["منتجات لغة ص الثلاثة"]
        P1[🟢 sad<br/>المفسر]
        P2[🔵 sadc<br/>المترجم]
        P3[🟣 VM<br/>الآلة الافتراضية]
    end

    subgraph Entries["نقاط الدخول"]
        E1[tools/compiler/main_simple.cpp]
        E2[tools/compiler/main.cpp]
    end

    subgraph Shared["النواة المشتركة"]
        C1[sad_core<br/>lexer+parser+ast+types+errors]
    end

    subgraph Backends["الواجهات الخلفية"]
        B1[InterpreterCore<br/>AST tree-walking]
        B2[SIRBuilder → LLVM<br/>native binary]
        B3[VMCompiler → VMExecutor<br/>bytecode]
    end

    E1 --> P1
    E1 --> P3
    E2 --> P2

    P1 --> C1 --> B1
    P2 --> C1 --> B2
    P3 --> C1 --> B3

    style P1 fill:#c8e6c9,stroke:#388e3c,stroke-width:2px
    style P2 fill:#bbdefb,stroke:#1976d2,stroke-width:2px
    style P3 fill:#e1bee7,stroke:#7b1fa2,stroke-width:2px
    style C1 fill:#fff4e6,stroke:#ff9800,stroke-width:3px
```

> **ملاحظة:** المفسر والـVM يشتركان في نفس الـbinary (`sad.exe`) والـentry point، يفترقان بعد الـAST عبر علم `--vm`. المترجم `sadc` له binary منفصل و entry منفصل لأنه يحتاج LLVM.

---

## 7️⃣ خلاصة عملية — أين تبدأ حسب هدفك

| هدفك | ابدأ من |
|---|---|
| أريد إضافة كلمة مفتاحية | [data/language/keywords.yaml](data/language/keywords.yaml) → [shared/lexer/include/token.h](shared/lexer/include/token.h) |
| أريد تحسين رسالة خطأ | ⚠️ النظام غير موحَّد بعد — حالياً حرِّر مباشرة في `shared/errors/` C++ |
| أريد تصدير metadata اللغة | استخدم [tools/sadinfo/](tools/sadinfo/): `sadinfo --dump-keywords --format yaml` |
| أريد إضافة نوع جديد (مثل `مركّب`) | [shared/types/include/value.h](shared/types/include/value.h) → كل المُقيّمات |
| أريد إضافة تحسين للمترجم | `compiler_new/src/sir/sir_opcodes.h` → SIROptimizer |
| أريد إضافة دالة مضمنة | [shared/builtins/](shared/builtins/) + سجّلها في init |
| أريد إضافة وحدة قياسية | [stdlib/](stdlib/) — اكتب ملف `.ص` |
| أريد فهم كيف يُنفَّذ برنامج | [tools/compiler/main_simple.cpp:597](tools/compiler/main_simple.cpp) سطور 597-612 |
| أريد فهم البناء | [CMakeLists.txt](CMakeLists.txt) → [cmake/executables.cmake](cmake/executables.cmake) |

---

## 8️⃣ المراجع الأساسية للقراءة بالترتيب

1. [shared/lexer/include/token.h](shared/lexer/include/token.h) — كل أنواع الرموز
2. [shared/lexer/src/lexer_keywords.cpp](shared/lexer/src/lexer_keywords.cpp) — تسجيل 40 كلمة
3. [shared/types/include/value.h](shared/types/include/value.h) — نوع القيم الموحَّد
4. [tools/compiler/main_simple.cpp](tools/compiler/main_simple.cpp) — نقطة دخول التنفيذ
5. [cmake/executables.cmake](cmake/executables.cmake) — تجميع `sad` و `sadc`
6. [docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md](docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md) — مواصفات اللغة الكاملة

---

**التاريخ:** هذه الوثيقة جزء من سلسلة `_bmad-output/systems/doc-ir/`. تُحدَّث عند أي تغيير معماري كبير.
