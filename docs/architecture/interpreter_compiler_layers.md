# طبقات المُفسِّر والمُترجم في لغة ص

> **الغرض:** خريطة معمارية كاملة لطبقات `interpreter/` (تنفيذ مباشر) و`compiler/` (توليد ملف تنفيذي أصلي)، توضح كيف يتدفق الكود من المصدر إلى التنفيذ في كل مسار، وأين تلتقي الطبقتان في النواة المشتركة `shared/`.
>
> **القاعدة الذهبية (CW-02):** كل طبقة تعتمد فقط على ما تحتها. اتجاه التدفق ثابت: `Lexer → Parser → AST → (المسار يتفرّع)`.

---

## 📚 فهرس سريع

| ما تبحث عنه | اذهب إلى |
|---|---|
| نظرة عامة على المسارين | [§1 النظرة الكلية](#1-النظرة-الكلية-طائر-فوق-المشروع) |
| Lexer / Parser / AST / Types | [§2 النواة المشتركة](#2-النواة-المشتركة-shared) |
| كيف يعمل المُفسِّر داخلياً؟ | [§3 طبقات المُفسِّر](#3-طبقات-المفسر-interpretersrc) |
| Managers (6) و Visitors (27) و Builtins | [§3.2](#32-تفصيل-طبقات-المفسر) |
| كيف يعمل المُترجم داخلياً؟ | [§4 طبقات المُترجم](#4-طبقات-المترجم-compilersrc) |
| Frontend / Types / SIR Optimizer / Backend | [§4.2](#42-تفصيل-طبقات-المترجم) |
| مقارنة المُفسِّر vs المُترجم | [§5 جدول المقارنة](#5-مقارنة-جنبية-المفسر-vs-المترجم) |
| VM، Runtime ABI، FFI، Debug، UI | [§5.5 طبقات إضافية](#55-طبقات-إضافية-لم-تذكر-في-الرسم-العام) |
| كيف تُعالَج الأخطاء؟ | [§5.5 معالجة الأخطاء](#معالجة-الأخطاء-عبر-الطبقات) |
| ما تشاركه الطبقتان | [§6 ملتقى الطبقات](#6-أين-تلتقي-الطبقتان) |
| أوامر CMake والمكتبات | [§7 مكتبات البناء](#7-مكتبات-البناء-cmake) |
| ملخص بصري نهائي | [§8 ملخص بصري](#8-ملخص-بصري-نهائي) |
| روابط لوثائق ذات صلة | [§9 مراجع](#9-مراجع-داخلية) |

---

## 1. النظرة الكلية (طائر فوق المشروع)

```
                          ملف مصدر .ص
                                │
                                ▼
        ┌───────────────────────────────────────────────┐
        │   النواة المشتركة  (shared/)                    │
        │   ─────────────────────────                    │
        │   Lexer  →  Parser  →  AST  →  Types (Value)   │
        └───────────────────────────────────────────────┘
                                │
                ┌───────────────┴────────────────┐
                ▼                                ▼
   ┌─────────────────────┐          ┌──────────────────────────┐
   │   المُفسِّر          │          │   المُترجم                │
   │   interpreter/      │          │   compiler/              │
   │                     │          │                          │
   │   تنفيذ مباشر       │          │   AST → SIR → LLVM IR    │
   │   على شجرة AST      │          │   → ملف تنفيذي أصلي      │
   │                     │          │                          │
   │   sad-run.exe       │          │   sad-build.exe (sadc)   │
   └─────────────────────┘          └──────────────────────────┘
                │                                │
                ▼                                ▼
         تنفيذ فوري                       برنامج .exe مستقل
       (Tree-Walking)                  (Native Executable)
```

| المسار | السرعة | استخدام الذاكرة | يحتاج LLVM | الاستخدام النموذجي |
|---|---|---|---|---|
| المُفسِّر | متوسط | منخفض | ❌ | تطوير، REPL، scripts، اختبار |
| المُترجم | عالي جداً | منخفض جداً (وقت التشغيل) | ✅ Release | إنتاج، توزيع، أنظمة embedded |

---

## 2. النواة المشتركة (`shared/`) — أول 4 طبقات

```
ملف .ص (UTF-8)
    │
    ▼
┌─────────────────────────────────────────────────────────────┐
│ الطبقة 1: Lexer  (shared/lexer/)                             │
│ ──────────────────────────────                              │
│   - LexerCore: يحوّل النص إلى Token                          │
│   - KeywordTable: 40 كلمة محجوزة عربية                       │
│   - دعم UTF-8 + كلمات سياقية                                 │
│                                                              │
│   مدخل:   "دالة جمع(أ، ب) ارجع أ + ب نهاية"                  │
│   مخرج:   [KW_FUNC, IDENT(جمع), LPAREN, IDENT(أ), COMMA,    │
│            IDENT(ب), RPAREN, KW_RETURN, IDENT(أ), PLUS,     │
│            IDENT(ب), KW_END]                                 │
└─────────────────────────────────────────────────────────────┘
    │
    ▼
┌─────────────────────────────────────────────────────────────┐
│ الطبقة 2: Parser  (shared/parser/)                           │
│ ──────────────────────────────                              │
│   - ParserCore: يبني شجرة AST من Tokens                     │
│   - يفهم: دوال، أصناف، شروط، حلقات، match، try              │
│   - ينتج AST منظم في shared/ast/                            │
│                                                              │
│   مدخل:   stream of Tokens                                   │
│   مخرج:   FunctionDecl                                       │
│              ├─ name: "جمع"                                  │
│              ├─ params: [أ, ب]                              │
│              └─ body: [Return(BinOp(+, أ, ب))]              │
└─────────────────────────────────────────────────────────────┘
    │
    ▼
┌─────────────────────────────────────────────────────────────┐
│ الطبقة 3: AST + Types  (shared/ast/, shared/types/)          │
│ ──────────────────────────────────────                      │
│   - ASTNode: قاعدة كل العقد (Statement, Expression, Decl)   │
│   - Value: نوع موحد لقيم runtime (variant على ValueType)    │
│   - ObjectInstance, ClassType: تمثيل runtime للأصناف         │
│   - TypeBridge: جسر بين compile-time و runtime              │
│                                                              │
│   ⚠ راجع: docs/architecture/type_system_layers.md           │
└─────────────────────────────────────────────────────────────┘
    │
    ├──────────────────────┬──────────────────────────────────┐
    ▼                      ▼                                   ▼
 المُفسِّر               المُترجم                            VM (legacy)
```

**ملفات حاكمة:**
- [shared/lexer/include/token.h](../../shared/lexer/include/token.h)
- [shared/lexer/src/lexer_keywords.cpp](../../shared/lexer/src/lexer_keywords.cpp)
- [shared/types/include/value.h](../../shared/types/include/value.h)
- [shared/ast/include/](../../shared/ast/include/)

---

## 3. طبقات المُفسِّر (`interpreter/`)

### 3.1 المخطط المعماري

```
                          AST  (من النواة المشتركة)
                                  │
                                  ▼
   ┌─────────────────────────────────────────────────────────────┐
   │            InterpreterCore  (interpreter/src/core/)          │
   │            ──────────────────────────────────               │
   │            نقطة الدخول الوحيدة + Orchestrator                │
   │            يربط جميع الطبقات أدناه                            │
   └─────────────────────────────────────────────────────────────┘
                                  │
       ┌──────────────────────────┼─────────────────────────────┐
       ▼                          ▼                             ▼
   ┌──────────┐         ┌──────────────────┐         ┌──────────────────┐
   │ Managers │         │     Visitors      │         │     Builtins     │
   │ المديرون │         │     الزائرون      │         │   الدوال المدمجة │
   ├──────────┤         ├──────────────────┤         ├──────────────────┤
   │ Scope    │         │ StatementExecutor│         │ core_io          │
   │ Variable │   ◄──►  │ ExpressionEvalu- │   ◄──► │ math, strings    │
   │ Function │         │   ator (19 ملف)  │         │ async, ffi, http │
   │ Class    │         │ statement_*      │         │ kernel_*         │
   │ Object   │         │   (9 ملف)         │         │ ui_*             │
   │ Ownership│         │                  │         │ + builtin_registry│
   └──────────┘         └──────────────────┘         └──────────────────┘
       │                          │                             │
       └──────────────────────────┼─────────────────────────────┘
                                  ▼
                       ┌──────────────────────┐
                       │  shared/types/Value  │
                       │  shared/ast/         │
                       │  channel.h (تزامن)   │
                       └──────────────────────┘
                                  │
                                  ▼
                            تنفيذ فوري
```

### 3.2 الطبقات بالتفصيل

#### الطبقة A: Core (نقطة الدخول)

| الملف | الدور |
|---|---|
| [interpreter/src/core/interpreter_core.cpp](../../interpreter/src/core/interpreter_core.cpp) | `InterpreterCore` — يدير الدورة الكاملة: تحميل، تشغيل، إدارة النطاقات |

#### الطبقة B: Managers (إدارة الحالة)

تحت [interpreter/src/managers/](../../interpreter/src/managers/) — كل manager مسؤول عن جانب واحد من الحالة (CW-01 SRP):

| الـ Manager | المسؤولية |
|---|---|
| `ScopeManager` | بناء/تدمير النطاقات (scopes) المتداخلة |
| `VariableManager` | تعريف، قراءة، إسناد المتغيرات |
| `FunctionManager` | تسجيل الدوال (مشترك بين الـ goroutines — read-only) |
| `ClassManager` | تسجيل الأصناف، السمات، الوراثة |
| `ObjectManager` | إنشاء/تتبع `ObjectInstance` |
| `OwnershipManager` | فحص قواعد الملكية والاستعارة |

#### الطبقة C: Visitors (التنفيذ الفعلي)

تحت [interpreter/src/visitors/](../../interpreter/src/visitors/) — تطبق نمط Visitor على AST:

**StatementExecutor** (9 ملفات): ينفّذ الجمل
- `statement_executor.cpp` — الموزّع الرئيسي
- `statement_executor_control.cpp` — `إذا`, `بينما`, `لكل`, `طابق`
- `statement_executor_control_exceptions.cpp` — `حاول/امسك/أخيراً/ارمي`
- `statement_executor_functions*.cpp` — تعريف دوال + قوالب
- `statement_executor_oop*.cpp` — `صنف`, `بنية`, `سمة`
- `statement_executor_modules.cpp` — `استورد`

**ExpressionEvaluator** (19 ملف): يقيّم التعبيرات
- `expression_evaluator_core.cpp` — الموزّع
- `_binary_ops`, `_binary_logic` — العمليات الثنائية
- `_calls*` (5 ملفات) — استدعاءات الدوال + الماكروز + dispatch
- `_members*` (3 ملفات) — `obj.field`, `obj.method()`
- `_oop*` (5 ملفات) — `جديد`, طرق المصفوفات/النصوص/الخرائط، التزامن
- `_overloads.cpp` — حلّ overload
- `_ui.cpp` — تكامل UI

#### الطبقة D: Builtins (الدوال المدمجة)

تحت [interpreter/src/builtins/](../../interpreter/src/builtins/) — مسجّلة في `builtin_registry.cpp`:

| الفئة | الملفات |
|---|---|
| **Core I/O** | `builtin_core_io.cpp` (`اطبع`, `اطبع_سطر`, `اقرأ`) |
| **وحدات قياسية** | `builtin_module_*` — basics, math, strings, maps, async, exceptions, assertions, platform, ffi |
| **شبكة** | `builtin_module_http.cpp`, `_sockets.cpp`, `_websocket.cpp`, `_sadnet.cpp` |
| **kernel (نظام تشغيل)** | `builtin_kernel_*` (12 ملف) — cpu, gpu, memory, network, storage, threads, timers, usb, acpi, audio, serial, uefi |
| **UI** | تحت [interpreter/src/ui/](../../interpreter/src/ui/) — bridge + widgets + 12+ ملف builtins |

> **معايير كتابة builtins:** [interpreter/src/builtins/BUILTIN_CODING_STANDARDS.md](../../interpreter/src/builtins/BUILTIN_CODING_STANDARDS.md)

#### الطبقة E: Debug + UI

- [interpreter/src/debug/debug_server.cpp](../../interpreter/src/debug/debug_server.cpp) — خادم debug
- [interpreter/src/ui/](../../interpreter/src/ui/) — جسر SDL2 + widgets reactive

### 3.3 تدفق المُفسِّر (سيناريو حقيقي)

```sad
دالة جمع(أ، ب)
    ارجع أ + ب
نهاية
متغير ن = جمع(3، 5)
اطبع_سطر(ن)
```

```
1. Lexer        → tokens
2. Parser       → AST [FunctionDecl(جمع), VarDecl(ن), Call(اطبع_سطر)]
3. InterpreterCore.run(AST):
   │
   ├─ StatementExecutor يزور FunctionDecl
   │     └─ FunctionManager.register("جمع", FuncObj)
   │
   ├─ StatementExecutor يزور VarDecl(ن = جمع(3,5))
   │     ├─ ExpressionEvaluator يقيّم Call(جمع, [3,5])
   │     │     ├─ FunctionManager.lookup("جمع") → FuncObj
   │     │     ├─ ScopeManager.push() (نطاق جديد)
   │     │     ├─ VariableManager.define(أ=3, ب=5)
   │     │     ├─ StatementExecutor يزور Return(BinOp(+,أ,ب))
   │     │     │     ├─ ExpressionEvaluator.eval(أ) → Value(3)
   │     │     │     ├─ ExpressionEvaluator.eval(ب) → Value(5)
   │     │     │     └─ binary_ops.add(3,5) → Value(8)
   │     │     └─ ScopeManager.pop()
   │     └─ VariableManager.define(ن = Value(8))
   │
   └─ StatementExecutor يزور Call(اطبع_سطر, [ن])
         ├─ ExpressionEvaluator.eval(ن) → Value(8)
         └─ builtin_registry["اطبع_سطر"].invoke([8])
                 └─ stdout: "8\n"
```

---

## 4. طبقات المُترجم (`compiler/`)

### 4.1 المخطط المعماري

```
                        AST  (من النواة المشتركة)
                              │
                              ▼
   ┌──────────────────────────────────────────────────────────────┐
   │   Frontend  (compiler/src/frontend/)                          │
   │   ─────────────────────────                                   │
   │   - SIRBuilder: يحوّل AST → SIR (Sad IR)                      │
   │     • sir_builder_module.cpp     (الموزّع)                     │
   │     • sir_builder_functions.cpp  (دوال)                       │
   │     • sir_builder_match_patterns.cpp                          │
   │     • sir_builder_scope_loop.cpp                              │
   │   - Builders فرعية (50+ ملف):                                  │
   │     arithmetic/, collections/, oop/, memory/, directives/,    │
   │     platform/, builtins/, core/                                │
   │   - sir_frontend_optimizer.cpp: تحسينات على مستوى AST/Frontend│
   └──────────────────────────────────────────────────────────────┘
                              │
                              ▼  SIR Module
   ┌──────────────────────────────────────────────────────────────┐
   │   Type System  (compiler/src/types/)                          │
   │   ─────────────────────────                                   │
   │   - 18 ملف Hindley-Milner (راجع type_system_layers.md)        │
   │   - Type Inference, Constraint Solving, Generics, Traits      │
   │   - تُستدعى من Frontend والـ Optimizer                         │
   └──────────────────────────────────────────────────────────────┘
                              │
                              ▼  SIR + Type Info
   ┌──────────────────────────────────────────────────────────────┐
   │   SIR Optimizer  (compiler/src/sir_optimizer/)                │
   │   ──────────────────────────────                             │
   │   16 pass تعمل بالتسلسل (راجع optimization_layers.md):        │
   │   • constant_folding, dce, cse, copy_propagation              │
   │   • licm, strength_reduction, peephole, branch_folding        │
   │   • inlining, tail_call, sroa, register_coalescing             │
   │   • async_transform                                            │
   │   تُدار عبر optimizer.cpp + pass_manager.cpp                   │
   └──────────────────────────────────────────────────────────────┘
                              │
                              ▼  Optimized SIR
   ┌──────────────────────────────────────────────────────────────┐
   │   Backend  (compiler/src/backend/)                            │
   │   ─────────────────────────                                   │
   │                                                               │
   │   ┌────────────────────────────────────────────┐              │
   │   │ LLVM Backend  (backend/llvm/)              │              │
   │   │   - llvm_codegen_init/context/instructions │              │
   │   │   - llvm_type_mapper (SIR types → LLVM)    │              │
   │   │   - llvm_optimizer (LLVM PassBuilder)      │              │
   │   │     + arabic_passes (string pool, bidi,    │              │
   │   │       tashkeel optimization)               │              │
   │   │   - llvm_runtime, llvm_drop_glue           │              │
   │   │   - 50+ builders/* (oop, arithmetic, ...)  │              │
   │   └────────────────────────────────────────────┘              │
   │                                                               │
   │   ┌────────────────────────────────────────────┐              │
   │   │ ميزات منخفضة المستوى                       │              │
   │   │   - arm64_codegen, c_abi, ffi_wrapper       │              │
   │   │   - inline_asm, panic_handler               │              │
   │   │   - interrupt_handlers (للنواة)             │              │
   │   │   - linker_script, multiboot                │              │
   │   │   - pattern_codegen, async_codegen          │              │
   │   │   - pipeline_chain (تسلسل عمليات SIMD)     │              │
   │   │   - solidity_ffi (عقود ذكية)                │              │
   │   └────────────────────────────────────────────┘              │
   │                                                               │
   │   ┌────────────────────────────────────────────┐              │
   │   │ مُولّدات بديلة (Meta + بدائل)              │              │
   │   │   - meta/docs_emitter (HTML docs)          │              │
   │   │   - html_emitter, compose_emitter          │              │
   │   └────────────────────────────────────────────┘              │
   └──────────────────────────────────────────────────────────────┘
                              │
                              ▼  LLVM IR
                    ┌──────────────────┐
                    │   LLVM 18        │
                    │  (third_party)   │
                    │   IR → Object    │
                    └──────────────────┘
                              │
                              ▼  .obj/.o
                    ┌──────────────────┐
                    │   System Linker  │
                    │  (link.exe/ld)   │
                    └──────────────────┘
                              │
                              ▼
                       ملف تنفيذي .exe
                       (Native Code)
```

### 4.2 الطبقات بالتفصيل

#### الطبقة 1: Frontend — `AST → SIR`

[compiler/src/frontend/](../../compiler/src/frontend/) — يأخذ AST ويبني تمثيلاً وسيطاً اسمه **SIR** (Sad Intermediate Representation):

| الملف | الدور |
|---|---|
| `sir_builder_module.cpp` | الموزّع الرئيسي + بناء الوحدات |
| `sir_builder_functions.cpp` | بناء SIR للدوال |
| `sir_builder_match_patterns.cpp` | تحويل `طابق/عندما` إلى SIR |
| `sir_builder_scope_loop.cpp` | حلقات + نطاقات |
| `sir_builder_helpers.cpp` | أدوات مساعدة |
| `sir_module.cpp`, `sir_instruction.cpp`, `sir_types.cpp` | تعريف بنية SIR |
| `sir_frontend_optimizer*.cpp` | تحسينات مبكرة (قبل المُحسِّن الكامل) |
| `type_info.cpp` | حفر معلومات الأنواع في SIR |

تحت [compiler/src/frontend/builders/](../../compiler/src/frontend/builders/) — 50+ ملف منظّم في 8 فئات:
`arithmetic/`, `collections/`, `oop/`, `memory/`, `directives/`, `platform/`, `builtins/`, `core/`

#### الطبقة 2: Type System

18 ملف Hindley-Milner. **مفصّل في وثيقة منفصلة:**
👉 [compiler/docs/type_system_layers.md](../../compiler/docs/type_system_layers.md)

#### الطبقة 3: SIR Optimizer

16 pass على SIR (المُحسِّن الرسمي الوحيد). **مفصّل في وثيقة منفصلة:**
👉 [compiler/docs/optimization_layers.md](../../compiler/docs/optimization_layers.md)

#### الطبقة 4: Backend — `SIR → LLVM IR → Native`

[compiler/src/backend/](../../compiler/src/backend/) — تتفرّع إلى:

**LLVM Backend** [compiler/src/backend/llvm/](../../compiler/src/backend/llvm/) — 35+ ملف:
| الفئة | الملفات |
|---|---|
| Codegen أساسي | `llvm_codegen_init/context/instructions`, `llvm_type_mapper*` |
| Runtime | `llvm_runtime`, `llvm_runtime_io_gc`, `llvm_drop_glue`, `llvm_entry_point` |
| تحسينات LLVM | `llvm_optimizer.cpp` + `arabic_passes.cpp` + `tashkeel_optimizer.cpp` + `arabic_string_pool.cpp` + `bidi_cache.cpp` + `arabic_normalization.cpp` |
| منخفضة المستوى | `llvm_atomics`, `llvm_volatile_ops`, `llvm_inline_asm`, `llvm_paging`, `llvm_port_io_intrinsics`, `llvm_cpu_regs`, `llvm_kernel_sync*`, `llvm_interrupt_handlers`, `llvm_multiboot`, `llvm_bare_metal_linker`, `llvm_custom_sections` |
| د
| الفئة | الملفات |
|---|---|
| دعم بيانات | `llvm_array_support`, `llvm_dict_support` |
| Linker + Target | `llvm_linker`, `llvm_target`, `llvm_debug` |

تحت [compiler/src/backend/llvm/builders/](../../compiler/src/backend/llvm/builders/) — 50+ ملف منظّم: builtins (async, ffi, math, network, security, simd, system, ui, uefi, embedded, os_*)، expression (basic, binary_op, collections, comprehensions, members, ...)، statement (assign_if, control_branch, defer, exceptions, extension, generators, loops, match, property, ...)، method_call (array_basic, array_hof, concurrency, string_map)، class (constructor, operator, trait_impl)، template (type_inference, lambda_inference, infer_return).

**ميزات منخفضة المستوى** [compiler/src/backend/](../../compiler/src/backend/) (مباشرة):
| الملف | الدور |
|---|---|
| `arm64_codegen.cpp` | توليد ARM64 (بديل لـ x86-64) |
| `c_abi.cpp`, `c_abi_codegen.cpp` | توافق C ABI |
| `ffi_wrapper.cpp` | تغليف استدعاءات FFI |
| `inline_asm.cpp` | تجميع مضمن (`@تجميع`) |
| `panic_handler.cpp` | معالج الأخطاء القاتلة |
| `interrupt_handlers*.cpp` | معالجات المقاطعات (لنواة Ufuq) |
| `linker_script*.cpp` | توليد linker scripts |
| `pattern_codegen*.cpp` | توليد كود مطابقة الأنماط |
| `async_codegen.cpp` | توليد goroutines + futures |
| `pipeline_chain*.cpp` | تسلسل عمليات SIMD/خطوط أنابيب |
| `solidity_ffi.cpp` | عقود ذكية (Solidity) |
| `entry_point.cpp` | نقطة دخول البرنامج |
| `extension_codegen.cpp` | طرق الامتداد |
| `compose_emitter.cpp`, `html_emitter.cpp` | مُولّدات بديلة (UI/Web) |

#### الطبقة 5: Meta — توليد الوثائق

[compiler/src/meta/docs_emitter*.cpp](../../compiler/src/meta/) — يستخرج الوثائق من تعليقات `##` و`#** **#` ويُولّد HTML.

#### الطبقات الإضافية في `compiler/include/`

- [compiler/include/runtime/](../../compiler/include/runtime/) — ABI لوقت التشغيل المستقل
- [compiler/include/kernel/](../../compiler/include/kernel/) — دعم نظام التشغيل Ufuq
- [compiler/include/crypto/](../../compiler/include/crypto/) — تشفير
- [compiler/include/format/](../../compiler/include/format/) — تنسيق ELF/PE
- [compiler/include/packages/](../../compiler/include/packages/) — مدير الحزم
- [compiler/include/suggestions/](../../compiler/include/suggestions/) — اقتراحات الأخطاء
- [compiler/include/testing/](../../compiler/include/testing/) — اختبار الخصائص

### 4.3 تدفق المُترجم (نفس السيناريو)

```sad
دالة جمع(أ، ب)
    ارجع أ + ب
نهاية
متغير ن = جمع(3، 5)
اطبع_سطر(ن)
```

```
1. Lexer → Parser → AST  (نفس النواة المشتركة)
                       │
2. Frontend.SIRBuilder │
       │               ▼
       │  SIR Module:
       │    func @جمع(i64 %أ, i64 %ب) -> i64
       │      entry:
       │        %t1 = add i64 %أ, %ب
       │        ret i64 %t1
       │    func @main() -> i32
       │      entry:
       │        %ن = call i64 @جمع(i64 3, i64 5)
       │        call void @اطبع_سطر(i64 %ن)
       │        ret i32 0
       │
3. Type System.infer  → يستنتج: أ:رقم, ب:رقم, ن:رقم
       │
4. SIR Optimizer (16 pass):
       │   - constant_folding: 3+5 → 8
       │   - inlining: قد يدمج جمع(3,5) → 8 مباشرة
       │   - dce: يحذف التعليمات الميتة
       │
5. LLVM Backend:
       │   - llvm_type_mapper: i64 → LLVM i64
       │   - llvm_codegen: ينتج LLVM IR
       │   - arabic_passes: pool للسلاسل العربية
       │   - llvm_optimizer: PassBuilder (O2)
       │
6. LLVM 18 → object file (.obj/.o)
       │
7. System Linker (link.exe / ld) → app.exe
       │
8. تشغيل app.exe → stdout: "8"
```

---

## 5. مقارنة جنبية: المُفسِّر vs المُترجم

| الجانب | المُفسِّر | المُترجم |
|---|---|---|
| **نقطة الدخول** | `InterpreterCore.run(AST)` | `Frontend.SIRBuilder.build(AST)` |
| **التمثيل الوسيط** | لا يوجد (يعمل على AST مباشرة) | SIR ثم LLVM IR |
| **إدارة الذاكرة** | Managers + RAII C++ | OwnershipManager (compile-time) + GC اختياري |
| **التحسينات** | محدودة (constant folding بسيط) | 16 pass SIR + LLVM PassBuilder كامل (O0-O3) |
| **التزامن** | `channel.h` + std::thread | `async_codegen` + LLVM coroutines |
| **FFI** | `builtin_module_ffi.cpp` (dlopen/LoadLibrary) | `ffi_wrapper.cpp` + `c_abi_codegen.cpp` |
| **UI** | `interpreter/src/ui/` (SDL2 مباشر) | غير مدعوم بعد (يستخدم المُفسِّر) |
| **خادم Debug** | `debug_server.cpp` (DAP) | غير متاح (debug عبر gdb/lldb) |
| **حجم الكود** | ~27 visitor (18 expression + 9 statement) + 6 manager + 32 builtin | ~12 frontend + 16 optimizer + 35 LLVM + 50+ builders |
| **الناتج** | لا شيء (تنفيذ فوري) | ملف تنفيذي أصلي |
| **يعتمد على LLVM** | لا | نعم (Release فقط) |
| **منصات** | جميع المنصات | حسب targets LLVM (x86-64, ARM64, riscv...) |

> ⚠️ **تنبيه حول الأرقام:** أعداد الملفات (visitors=27، passes=16، LLVM=35، types=18) **لقطة لحظية** قابلة للتغيير مع كل إضافة ميزة. للحصول على الأرقام الفعلية: `Get-ChildItem <مسار> -Filter *.cpp | Measure-Object`.

---

## 5.5 طبقات إضافية لم تُذكر في الرسم العام

ركّز هذا التوثيق على المسار الرئيسي (Lex→Parse→AST→[Interp | SIR→LLVM]). توجد طبقات معمارية أخرى تتقاطع مع المسار:

| الطبقة | المجلد | الدور | تستخدم في |
|---|---|---|---|
| **VM (legacy)** | [vm/](../../vm/) | آلة افتراضية للبايت كود مربوطة بالمفسر مباشرة | المُفسِّر فقط |
| **Runtime ABI** | [runtime/freestanding/](../../runtime/freestanding/) | ABI/FFI مستقل (بدون libc) لبرامج bare-metal | المُترجم (نواة Ufuq) |
| **FFI الديناميكي** | [interpreter/src/builtins/builtin_module_ffi.cpp](../../interpreter/src/builtins/) | dlopen/LoadLibrary لاستدعاء مكتبات C في وقت التشغيل | المُفسِّر |
| **FFI الثابت** | [compiler/src/backend/ffi_wrapper.cpp](../../compiler/src/backend/) + `c_abi_codegen.cpp` | تغليف استدعاءات C ABI في الكود المُولّد | المُترجم |
| **Debug Server** | [interpreter/src/debug/debug_server.cpp](../../interpreter/src/debug/) | بروتوكول DAP | المُفسِّر فقط |
| **UI Bridge** | [interpreter/src/ui/](../../interpreter/src/ui/) | جسر SDL2 + widgets | المُفسِّر فقط |

> **ملاحظة معمارية:** `ExpressionEvaluator` يستدعي `StatementExecutor` (للحلقات/الشروط داخل lambda)، و`StatementExecutor` يستدعي `ExpressionEvaluator` (لتقييم تعابير الجمل). هذا **اعتماد متبادل مقصود** وليس انتهاكاً لطبقات CW-02 — كلاهما في نفس طبقة Visitors.

### معالجة الأخطاء عبر الطبقات

```
┌──────────────────────────────────────────────────────────┐
│ خطأ Lex/Parse → ParseError → InterpreterCore يطبع + exit │
├──────────────────────────────────────────────────────────┤
│ خطأ Runtime  → C++ exception (UserThrown/RuntimeError)   │
│              → ينتشر عبر visitors → امسك/أخيراً → main()  │
├──────────────────────────────────────────────────────────┤
│ خطأ Goroutine → يُلتقط داخل Goroutine → channel أخطاء    │
│              → main thread يستقبل ويتعامل (لا crash)     │
├──────────────────────────────────────────────────────────┤
│ خطأ Compile  → SIRBuilder.reportError() → exit non-zero  │
│              → خطأ LLVM IR → llvm::report_fatal_error    │
└──────────────────────────────────────────────────────────┘
```

---

## 6. أين تلتقي الطبقتان؟

```
                    shared/  (طبقة مشتركة 100%)
                       │
        ┌──────────────┼───────────────┐
        ▼              ▼               ▼
      Lexer         Parser           Types
      (نفس       (نفس الـ          (Value, AST,
      الكود)      ParserCore)       TypeBridge)
        │              │               │
        └──────┬───────┴───────┬───────┘
               │               │
               ▼               ▼
         interpreter/     compiler/
         (مستهلك)         (مستهلك)
```

**القاعدة:** أي تعديل في `shared/` يجب اختباره في **كلا المسارين** (المُفسِّر والمُترجم) لأن أي كسر سيُعطّل الاثنين.

> **اختبارات Dual Execution:** [tests/dual_execution/runner.py](../../tests/dual_execution/runner.py) — يشغّل كل اختبار مرتين (مرة بالمُفسِّر، مرة بالمُترجم) ويقارن المخرجات.

---

## 7. مكتبات البناء (CMake)

```
sad_core (مكتبة شاملة)
    │
    ├─ sad_lexer                  (shared/lexer/)
    ├─ sad_parser                 (shared/parser/)
    ├─ sad_ast                    (shared/ast/)
    ├─ sad_type_system            (compiler/src/types/ + shared/types/)
    ├─ sad_semantic_shared        (semantic analysis مشترك)
    ├─ sad_ownership              (OwnershipManager)
    │
    ├─ sad_interpreter            (interpreter/src/)
    │     ├─ managers, visitors, builtins
    │     ├─ debug_server
    │     └─ UI bridge (sad_rt_ui)
    │
    ├─ sad_frontend               (compiler/src/frontend/)
    ├─ sad_optimizer              (compiler/src/sir_optimizer/)
    ├─ sad_compiler               (INTERFACE — يجمع frontend+optimizer+llvm)
    ├─ sad_llvm_backend           (compiler/src/backend/llvm/)
    │     └─ + arabic_passes (مدمج عبر GLOB)
    │
    ├─ sad_rt_abi                 (runtime ABI)
    ├─ sad_rt_ffi                 (runtime FFI)
    ├─ sad_rt_ui                  (runtime UI)
    ├─ sad_security_core          (أمان)
    ├─ sad_websocket              (شبكة)
    └─ sad_profiler               (قياس أداء)

التنفيذيات:
    sad-run.exe   (Debug + Release)  ← يربط: sad_core + interpreter
    sad-build.exe (Release فقط)      ← يربط: sad_core + compiler + LLVM 18
    sadc          (alias لـ sad-build)
```

أوامر البناء:
```powershell
# المُفسِّر فقط (Debug سريع)
cmake --build build --config Debug --target sad-run

# المُترجم (Release — يحتاج LLVM)
cmake --build build --config Release --target sad-build

# كل شيء + اختبارات
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build --config Debug
```

---

## 8. ملخص بصري نهائي

```
╔═════════════════════════════════════════════════════════════════════╗
║                          مصدر .ص                                     ║
╠═════════════════════════════════════════════════════════════════════╣
║                                                                     ║
║   shared/lexer  →  shared/parser  →  shared/ast  →  shared/types    ║
║       (Lex)         (Parse)            (AST)         (Value)        ║
║                                                                     ║
║                            ╳ تفرّع ╳                                  ║
║                                                                     ║
║   ┌──────────────────────────┐     ┌──────────────────────────────┐ ║
║   │  interpreter/             │     │  compiler/                   │ ║
║   │  ────────────             │     │  ──────────                  │ ║
║   │                           │     │                              │ ║
║   │  Core                     │     │  frontend (SIRBuilder)       │ ║
║   │   ├ Managers (6)          │     │       │                      │ ║
║   │   ├ Visitors (28)         │     │  types (HM, 18 ملف)          │ ║
║   │   └ Builtins (32+)        │     │       │                      │ ║
║   │                           │     │  sir_optimizer (16 pass)     │ ║
║   │  → تنفيذ مباشر            │     │       │                      │ ║
║   │                           │     │  backend/llvm + arabic_passes│ ║
║   │  ⇒ sad-run.exe            │     │       │                      │ ║
║   │                           │     │  → LLVM IR → linker          │ ║
║   │                           │     │  ⇒ sad-build.exe → app.exe   │ ║
║   └──────────────────────────┘     └──────────────────────────────┘ ║
║                                                                     ║
╚═════════════════════════════════════════════════════════════════════╝
```

---

## 9. مراجع داخلية

- نظام الأنواع: [compiler/docs/type_system_layers.md](../../compiler/docs/type_system_layers.md)
- طبقات المحسن: [compiler/docs/optimization_layers.md](../../compiler/docs/optimization_layers.md)
- معايير builtins: [interpreter/src/builtins/BUILTIN_CODING_STANDARDS.md](../../interpreter/src/builtins/BUILTIN_CODING_STANDARDS.md)
- مرجع اللغة الكامل: [docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md](../SAD_LANGUAGE_COMPLETE_REFERENCE.md)
- خريطة المشروع (memory): `/memories/repo/project_full_map.md`
