# خالد — المترجم والنواة (Compiler & Core Engine)

> الدور: مهندس المترجم الرئيسي
> المسؤولية: sadc (LLVM)، محرك SIR، VM، أهداف الترجمة، JIT، BeeOS runtime
> الملف المرجعي: `docs/plan/01_STRATEGIC_PLAN.md`

---

## الملكية الحصرية

```
compiler_new/            ← كل ملفات المترجم
├── src/
│   ├── frontend/        ← تحليل + تحقق أنواع
│   ├── sir/             ← SIR opcodes + builder + optimizer
│   ├── backend/         ← LLVM codegen + WASM + ARM64
│   ├── codegen/         ← توليد الكود
│   ├── types/           ← نظام الأنواع
│   └── semantic/        ← التحليل الدلالي
├── include/
└── tests/

vm/                      ← الآلة الافتراضية
├── bytecode/
├── executor/
└── gc/

cmake/                   ← ملفات البناء
├── llvm.cmake
├── platform.cmake
├── wasm.cmake
└── executables.cmake

shared/lexer/            ← تغييرات tokens فقط (بالتنسيق مع المفسر)
shared/parser/           ← قراءة فقط (لفهم AST)
```

### القاعدة الذهبية
> خالد لا يعدّل: `interpreter_new/`، `tools/`، `docs/`، `graphics/`، `platform/`
> أي تغيير في `shared/` يتم بعد code review

---

## 2026 — التفصيل الشهري

---

### الربع الأول (مارس — مايو 2026): "تثبيت المترجم"

#### مارس 2026

| الأسبوع | المهمة | الملفات | المُخرج | معيار القبول |
|---------|--------|---------|---------|-------------|
| 1 | إصلاح LLVM Debug linking | `cmake/llvm.cmake` | sadc يبني في Debug + Release | `cmake --build build --config Debug --target sadc` ينجح |
| 2 | إصلاح `_ITERATOR_DEBUG_LEVEL` mismatch | `cmake/llvm.cmake`, `cmake/platform.cmake` | 0 أخطاء ربط | 0 linker errors |
| 3 | sadc: اختبار 10 برامج .ص أساسية | `compiler_new/tests/` | 10 برامج → 10 .exe | كل .exe يعطي نتيجة صحيحة |
| 4 | توثيق حل LLVM مع تعليمات بناء | `compiler_new/docs/` | README محدث | مطور جديد يستطيع بناء sadc بنجاح |

**المُخرج الشهري**: sadc يبني في Debug و Release بدون أخطاء، 10 برامج تترجم بنجاح

#### أبريل 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | sadc: دعم OOP — أصناف + باني | `compiler_new/src/codegen/class_codegen.cpp` | `صنف شخص باني(...) نهاية` → LLVM struct + constructor |
| 2 | sadc: دعم OOP — وراثة | `compiler_new/src/codegen/inheritance_codegen.cpp` | `يرث` → LLVM vtable |
| 3 | sadc: دعم OOP — دوال عضوية + هذا | `compiler_new/src/codegen/method_codegen.cpp` | `هذا.اسم` → LLVM GEP |
| 4 | sadc: اختبار 5 برامج OOP | `compiler_new/tests/oop/` | 5 برامج OOP → .exe |

**المُخرج الشهري**: OOP كامل في sadc (أصناف + وراثة + methods + this)

#### مايو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | sadc: Pattern Matching → LLVM | `compiler_new/src/codegen/match_codegen.cpp` | `طابق/عندما` → LLVM switch/br |
| 2 | sadc: Closures + Lambdas | `compiler_new/src/codegen/lambda_codegen.cpp` | `لامدا(س) س * 2 نهاية` → LLVM function ptr + capture |
| 3 | sadc: Error Handling → LLVM | `compiler_new/src/codegen/exception_codegen.cpp` | `حاول/امسك/ارمي` → LLVM landing pads |
| 4 | اختبار شامل: 20 برنامج | `compiler_new/tests/` | 20/20 pass مع sadc |

**المُخرج الشهري**: 20+ برنامج .ص يعمل عبر sadc (OOP + match + lambda + exceptions)

**معايير القبول Q1:**
- [ ] sadc يبني في Debug بـ 0 أخطاء
- [ ] 20+ برنامج .ص → .exe ناجح
- [ ] OOP + Pattern Matching + Lambdas + Exceptions في codegen
- [ ] كل اختبار مع assertion تلقائي (لا اختبار يدوي)

---

### الربع الثاني (يونيو — أغسطس 2026): "التوسع"

#### يونيو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | sadc: Module System — استورد/صدّر | `compiler_new/src/codegen/module_codegen.cpp` | `استورد "وحدة"` → LLVM module linking |
| 2 | sadc: Module resolution + paths | `compiler_new/src/frontend/module_resolver.cpp` | مسارات نسبية ومطلقة |
| 3 | sadc: Standard library linking | `compiler_new/src/linker/` | ربط stdlib مع executables |
| 4 | اختبار: 5 برامج multi-module | `compiler_new/tests/modules/` | 5 برامج بوحدات متعددة تعمل |

#### يوليو 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | sadc: Generics — monomorphization | `compiler_new/src/codegen/generics_codegen.cpp` | `دالة<ن>` → نسخ متخصصة |
| 2 | sadc: Generics — type inference | `compiler_new/src/types/type_inference.cpp` | استنتاج أنواع تلقائي |
| 3 | sadc: SIR Optimizer — 5 passes | `compiler_new/src/sir/sir_optimizer.cpp` | DCE, const fold, inline, CSE, loop simplify |
| 4 | Benchmark: مقارنة sadc vs interpreter | `compiler_new/benchmarks/` | تقرير أداء مع أرقام |

#### أغسطس 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | sadc: ARM64 target | `compiler_new/src/backend/arm64_codegen.cpp` | Cross-compile لـ aarch64 |
| 2 | sadc: ARM64 testing | `compiler_new/tests/arm64/` | 10 برامج على ARM64 (RPi/M1) |
| 3 | sadc: WASM target تحسين | `compiler_new/src/backend/wasm_codegen.cpp` | Array + Import + Memory management |
| 4 | اختبار شامل: 50 برنامج | `compiler_new/tests/` | 50/50 pass عبر sadc |

**معايير القبول Q2:**
- [ ] Module system يعمل (استورد/صدّر)
- [ ] Generics أساسية مع monomorphization
- [ ] 5 optimization passes مع قياسات
- [ ] ARM64 cross-compilation يعمل
- [ ] WASM output محسّن
- [ ] 50 برنامج .ص → .exe ناجح

---

### الربع الثالث (سبتمبر — نوفمبر 2026): "الصقل"

#### سبتمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | sadc: WASM output مباشر (بدون Emscripten) | `compiler_new/src/backend/wasm_direct.cpp` | `.ص → .wasm` مباشر |
| 2 | sadc: WASM memory management | `compiler_new/src/backend/wasm_memory.cpp` | GC/linear memory |
| 3 | sadc: WASM imports/exports | `compiler_new/src/backend/wasm_interface.cpp` | JavaScript interop |
| 4 | WASM: 10 برامج في المتصفح | `compiler_new/tests/wasm/` | 10 برامج .wasm تعمل |

#### أكتوبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | sadc: Optimization -O2 | `compiler_new/src/sir/sir_optimizer_advanced.cpp` | Loop unroll, vectorize hints |
| 2 | sadc: Debug info (DWARF) | `compiler_new/src/backend/debug_info.cpp` | Source maps لـ GDB/LLDB |
| 3 | sadc: Profile-guided optimization | `compiler_new/src/sir/pgo.cpp` | PGO مع LLVM |
| 4 | Benchmark: أداء مقارب لـ C | `compiler_new/benchmarks/` | ضمن 2x من C في microbenchmarks |

#### نوفمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | sadc: 100 برنامج اختبار | `compiler_new/tests/comprehensive/` | كل ميزة لغوية → executable |
| 3 | sadc: Async/Await codegen | `compiler_new/src/codegen/async_codegen.cpp` | LLVM coroutines |
| 4 | تثبيت + إصلاح أخطاء نهائي | `compiler_new/` | 0 أخطاء P0 |

**معايير القبول Q3:**
- [ ] WASM مباشر (بدون Emscripten) يعمل
- [ ] Debug symbols (DWARF) تعمل مع LLDB
- [ ] -O2 أداء ضمن 2x من C
- [ ] 100 برنامج اختبار ناجح
- [ ] Async/Await → LLVM coroutines

---

### الربع الرابع (ديسمبر 2026): "الإطلاق"

#### ديسمبر 2026

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | sadc v1.0: cross-compilation matrix | `cmake/` | x86_64 + ARM64 + WASM builds |
| 2 | sadc v1.0: release builds (stripped) | `cmake/executables.cmake` | Optimized, stripped executables |
| 3 | sadc v1.0: documentation | `compiler_new/docs/` | `sadc --help` + man page |
| 4 | v1.0 release preparation | `compiler_new/` | Smoke tests مكتملة |

**معايير القبول Q4:**
- [ ] sadc v1.0 مستقر
- [ ] 3 targets: x86_64, ARM64, WASM
- [ ] 0 أخطاء P0/P1
- [ ] 200+ برنامج يترجم بنجاح
- [ ] وثائق كاملة

---

## 2027 — التفصيل الشهري

---

### الربع الأول (يناير — مارس 2027): "ما بعد v1.0"

#### يناير 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | v1.1 hotfixes | `compiler_new/` | إصلاح أخطاء مكتشفة بعد الإطلاق |
| 3 | sadc: Generics متقدمة (constraints) | `compiler_new/src/codegen/generics_advanced.cpp` | `دالة<ن: رقمي>` |
| 4 | sadc: Generics — associated types | `compiler_new/src/types/associated_types.cpp` | أنواع مرتبطة بـ Traits |

#### فبراير 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | sadc: Advanced Generics — تحسين | `compiler_new/src/codegen/` | Type erasure + boxing |
| 3-4 | sadc: Async/Await تحسين | `compiler_new/src/codegen/async_v2.cpp` | Structured concurrency support |

#### مارس 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | sadc: Async codegen — scheduler | `compiler_new/src/runtime/scheduler.cpp` | Task scheduler مدمج |
| 2 | sadc: Async I/O integration | `compiler_new/src/runtime/async_io.cpp` | Non-blocking I/O compile support |
| 3-4 | v1.3 release + testing | `compiler_new/tests/` | 300+ برنامج ناجح |

**معايير القبول Q1 2027:**
- [ ] Generics متقدمة (constraints + associated types)
- [ ] Async/Await مع structured concurrency
- [ ] v1.3 مستقر مع 300+ برنامج

---

### الربع الثاني (أبريل — يونيو 2027): "JIT"

#### أبريل 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | JIT Architecture design | `compiler_new/src/jit/jit_architecture.md` | وثيقة تصميم JIT |
| 2 | JIT: LLVM ORC integration | `compiler_new/src/jit/jit_engine.cpp` | LLVM ORC JIT engine |
| 3 | JIT: hot code detection | `compiler_new/src/jit/hot_tracker.cpp` | تتبع الدوال الساخنة |
| 4 | JIT: basic compilation | `compiler_new/src/jit/jit_compiler.cpp` | دوال بسيطة → native عند الحاجة |

#### مايو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | JIT: tier system (interpret → compile) | `compiler_new/src/jit/tiered_engine.cpp` | مستويان: مفسر → JIT |
| 2 | JIT: OSR (on-stack replacement) | `compiler_new/src/jit/osr.cpp` | تبديل أثناء التشغيل |
| 3 | JIT: deoptimization | `compiler_new/src/jit/deopt.cpp` | رجوع من JIT للمفسر |
| 4 | JIT: benchmark vs AOT vs interpreter | `compiler_new/benchmarks/jit/` | مقارنة أداء ثلاثية |

#### يونيو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | JIT: testing + stabilization | `compiler_new/tests/jit/` | 50 برنامج JIT ناجح |
| 3-4 | v1.7 release مع JIT | `compiler_new/` | خيار `--jit` عامل |

**معايير القبول Q2 2027:**
- [ ] JIT يعمل مع LLVM ORC
- [ ] أداء JIT ≥ 70% من AOT
- [ ] 50 برنامج JIT بدون crashes
- [ ] `sad --jit script.ص` يعمل

---

### الربع الثالث (يوليو — سبتمبر 2027): "Concurrency + BeeOS"

#### يوليو 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Concurrency: thread codegen | `compiler_new/src/codegen/thread_codegen.cpp` | OS threads → LLVM |
| 2 | Concurrency: mutex/channel | `compiler_new/src/codegen/sync_codegen.cpp` | Synchronization primitives |
| 3 | Concurrency: atomic operations | `compiler_new/src/codegen/atomic_codegen.cpp` | `@ذري(...)` → LLVM atomics |
| 4 | Concurrency: testing | `compiler_new/tests/concurrency/` | 20 برنامج متوازي |

#### أغسطس 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | Self-hosting: lexer بلغة ص | `compiler_new/self/lexer.ص` | Lexer مكتوب بلغة ص نفسها |
| 2 | Self-hosting: token types | `compiler_new/self/tokens.ص` | Token enum بلغة ص |
| 3-4 | Self-hosting: parser أساسي | `compiler_new/self/parser.ص` | Parser لمجموعة فرعية |

#### سبتمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | BeeOS: freestanding runtime | `compiler_new/src/runtime/freestanding.cpp` | Runtime بدون malloc/libc |
| 2 | BeeOS: BeeWax Memory Manager | `bee_os/kernel/memory/` | Buddy allocator بالعربي |
| 3 | BeeOS: BeeSwarm Scheduler | `bee_os/kernel/scheduler/` | Multilevel queue scheduler |
| 4 | BeeOS: integration test | `bee_os/tests/` | Boot في QEMU مع scheduler |

**معايير القبول Q3 2027:**
- [ ] Concurrency: threads + mutex + channels + atomics
- [ ] Self-hosting: lexer + parser بلغة ص (درجة أولى)
- [ ] BeeOS: boot + scheduler + memory في QEMU
- [ ] 500+ برنامج ناجح عبر sadc

---

### الربع الرابع (أكتوبر — ديسمبر 2027): "v2.0"

#### أكتوبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | BeeOS: BeeFS filesystem | `bee_os/kernel/fs/` | نظام ملفات أولي |
| 2 | BeeOS: shell أساسي | `bee_os/kernel/shell/` | أوامر: ls, cat, echo |
| 3 | sadc: bare metal target | `compiler_new/src/backend/bare_metal.cpp` | `.ص → ELF` بدون OS |
| 4 | تكامل: BeeOS + sadc | `bee_os/` | برامج ص تعمل على BeeOS |

#### نوفمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1-2 | sadc v2.0: تثبيت نهائي | `compiler_new/` | 1000+ برنامج ناجح |
| 3 | sadc v2.0: performance regression tests | `compiler_new/benchmarks/` | لا تراجع أداء |
| 4 | sadc v2.0: documentation update | `compiler_new/docs/` | وثائق v2.0 |

#### ديسمبر 2027

| الأسبوع | المهمة | الملفات | المُخرج |
|---------|--------|---------|---------|
| 1 | v2.0 release candidate | `compiler_new/` | RC1 جاهز |
| 2 | v2.0 smoke tests | `compiler_new/tests/` | كل المنصات والأهداف |
| 3 | v2.0 release builds | `cmake/` | Stripped, optimized |
| 4 | 🎉 v2.0 release | `compiler_new/` | إطلاق رسمي |

**معايير القبول Q4 2027:**
- [ ] 1000+ برنامج يعمل عبر sadc
- [ ] JIT + AOT + WASM + bare metal
- [ ] BeeOS demo: boot + shell + filesystem في QEMU
- [ ] Self-hosting: lexer + parser
- [ ] v2.0 stable release

---

## الأدوات والتقنيات المطلوبة

| الأداة | الغرض | الإصدار |
|--------|--------|---------|
| LLVM | Backend codegen | 18.x → 19.x (2027) |
| CMake | Build system | 3.28+ |
| QEMU | BeeOS testing | latest |
| Raspberry Pi / M1 | ARM64 testing | أي جهاز ARM64 |
| Valgrind/ASan | Memory safety | latest |
| perf / VTune | Performance profiling | latest |

---

## المخاطر الخاصة بخالد

| المخاطرة | خطة التخفيف |
|----------|-------------|
| LLVM API changes (18→19) | Compatibility layer + pinned LLVM version |
| JIT complexity | البدء بـ ORC البسيط، تصعيد تدريجي |
| BeeOS bare metal issues | QEMU أولاً، عتاد حقيقي لاحقاً |
| Self-hosting scope creep | فقط lexer + parser في 2027 |
| ARM64 testing hardware | GitHub Actions runners أو cloud ARM |

---

## ملخص المُخرجات السنوية

```
2026:
  ✅ sadc يبني Debug + Release
  ✅ 200+ برنامج → executable
  ✅ OOP + Match + Lambda + Async + Modules + Generics
  ✅ ARM64 + WASM targets
  ✅ -O2 optimization
  ✅ DWARF debug info

2027:
  ✅ JIT Compiler (LLVM ORC)
  ✅ Concurrency (threads + mutex + channels)
  ✅ Self-hosting بداية (lexer + parser بلغة ص)
  ✅ BeeOS (boot + scheduler + memory + fs + shell)
  ✅ 1000+ برنامج ناجح
  ✅ v2.0 stable
```
