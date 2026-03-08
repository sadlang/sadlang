# نظام دعم المنصات المتعددة — لغة ص
# Multi-Platform Backend Architecture — Sad Language

> **الهدف:** توثيق مسار الكود من ملف `.ص` حتى التنفيذ على كل منصة مدعومة.
> **مبدأ أساسي:** كود واحد → كل المنصات.

---

## 1. نظرة شاملة — طبقات اللغة

```
                    ╔═══════════════════════════════════════╗
                    ║        مصدر .ص  (الكود العربي)       ║
                    ╚══════════════════╤════════════════════╝
                                       │
    ═══════════════════════════════════════════════════════════════
    الطبقة 1: التحليل (Analysis Layer) — مشتركة لجميع المنصات
    ═══════════════════════════════════════════════════════════════
                                       │
                ┌──────────────────────┼──────────────────────┐
                │                      │                      │
         ┌──────▼──────┐     ┌────────▼────────┐    ┌───────▼───────┐
         │   Lexer     │     │    Parser       │    │   Sema        │
         │ المحلل      │────▶│ المحلل النحوي   │───▶│ التحقق       │
         │ المعجمي     │     │                 │    │ الدلالي       │
         │             │     │ 31 ملف، 25K سطر │    │               │
         │ token.h     │     │ parser_core.h   │    │ type_checker  │
         │ 120+ token  │     │ 32 ميزة نحوية  │    │ scope_check   │
         └─────────────┘     └─────────────────┘    └───────────────┘
                                       │
                           ┌───────────▼───────────┐
                           │       AST             │
                           │   شجرة البنية المجردة │
                           │   16 ملف header       │
                           │   ~50 نوع عقدة        │
                           └───────────┬───────────┘
                                       │
    ═══════════════════════════════════════════════════════════════
    الطبقة 2: التنفيذ (Execution Layer) — تختلف حسب المسار
    ═══════════════════════════════════════════════════════════════
                                       │
              ┌────────────────────────┼────────────────────────┐
              │                        │                        │
     ╔════════▼═════════╗   ╔═════════▼══════════╗   ╔════════▼═════════╗
     ║   المسار A       ║   ║   المسار B         ║   ║   المسار C       ║
     ║   المفسر         ║   ║   المترجم sadc     ║   ║   المنقّلات      ║
     ║   (تنفيذ فوري)   ║   ║   (LLVM native)    ║   ║   (Transpilers)  ║
     ╚════════╤═════════╝   ╚═════════╤══════════╝   ╚════════╤═════════╝
              │                        │                        │
    ══════════╧════════════════════════╧════════════════════════╧═══════
    الطبقة 3: المنصة (Platform Layer) — الإخراج النهائي
    ═══════════════════════════════════════════════════════════════════
              │                        │                        │
    ┌─────────┤               ┌────────┤                ┌───────┤
    ▼         ▼               ▼        ▼                ▼       ▼
  Desktop   WASM          x86_64    ARM64          Kotlin    Swift
  (SDL2)   (Emscr.)       .exe     .binary         .kt      .swift
                           WASM32   ESP32           HTML/JS
                           .wasm    .bin            .html
```

---

## 2. تفصيل كل مسار

---

### المسار A: المفسر (Interpreter Path)

> **الاستخدام:** التطوير السريع، REPL، تجريب الكود، تعليم.

```
ملف .ص ──▶ Lexer ──▶ Parser ──▶ AST ──▶ InterpreterCore ──▶ نتيجة فورية
                                           │
                                  ┌────────┼────────────┐
                                  │        │            │
                              Expression  Statement    Builtin
                              Evaluator   Executor     Registry
                                  │        │            │
                              تقييم       تنفيذ        39 جزء
                              التعبيرات   الجمل       ~500+ دالة
                                  │        │            مدمجة
                              Variable   Scope
                              Manager    Manager
                                  │
                              Object/Class
                              Manager
```

**الملفات الرئيسية:**
| الملف | الدور | الحجم |
|-------|-------|-------|
| `interpreter_core.cpp` | نقطة الدخول | 264 سطر header |
| `expression_evaluator_core.cpp` | تقييم التعبيرات | كبير |
| `expression_evaluator_members.cpp` | وصول الأعضاء (`.`) | 1,473 سطر |
| `expression_evaluator_calls.cpp` | استدعاء الدوال | كبير |
| `statement_executor.cpp` | تنفيذ: if, while, for, return | كبير |
| `variable_manager.cpp` | إدارة المتغيرات | 468 سطر |
| `scope_manager.cpp` | إدارة النطاقات | 251 سطر |
| `builtin_registry_part1-39.cpp` | 39 ملف دوال مدمجة | ضخم |

**المنصة الهدف:** حيث يعمل `sad.exe` — Windows, Linux, macOS.

**مسار WASM للمفسر:**
```
sad.exe ──[Emscripten]──▶ sad.wasm + sad.js ──▶ المتصفح
           cmake/wasm.cmake
           tools/wasm/sad_wasm.cpp
           tools/wasm/wasm_builtins.cpp
```

---

### المسار B: المترجم sadc (Compiler Path)

> **الاستخدام:** إنتاج ملفات تنفيذية أصلية عالية الأداء.

```
ملف .ص ──▶ Lexer ──▶ Parser ──▶ AST
                                  │
                          ┌───────▼────────┐
                          │   SIR Builder  │     frontend/sir_builder_*.cpp
                          │  AST → SIR     │     (13 ملف)
                          └───────┬────────┘
                                  │
                          ┌───────▼────────┐
                          │  SIR Optimizer │     optimizer/
                          │  تحسين SIR    │
                          └───────┬────────┘
                                  │
                          ┌───────▼────────┐
                          │  LLVM CodeGen  │     backend/llvm/ (48 ملف)
                          │  SIR → LLVM IR │     sir_to_llvm_v2.cpp (61K bytes)
                          └───────┬────────┘
                                  │
                          ┌───────▼────────┐
                          │  LLVM Backend  │     LLVM 18 libraries
                          │  IR → Machine  │
                          └───────┬────────┘
                                  │
                    ┌─────────────┼─────────────────────┐
                    │             │                     │
                ┌───▼───┐   ┌────▼────┐          ┌─────▼─────┐
                │x86_64 │   │ ARM64   │          │  WASM32   │
                │       │   │         │          │           │
                │ .exe  │   │ .binary │          │ .wasm     │
                │ ELF   │   │ Mach-O  │          │ + .js     │
                └───────┘   └─────────┘          └───────────┘
                Win/Linux    macOS/iOS             المتصفح
                             Android ARM
```

**SIR — التمثيل الوسيط الملكي (Sad Intermediate Representation):**

```
┌──────────────────────────────────────────────────────────────┐
│  SIR: 90+ تعليمة في 9+ فئات                                │
│                                                              │
│  ┌────────────────┐  ┌────────────────┐  ┌────────────────┐ │
│  │  الملكية       │  │  الذاكرة       │  │  الحساب        │ │
│  │  Alloc, Borrow │  │  Load, Store   │  │  Add, Sub      │ │
│  │  Move, Drop    │  │  StackAlloc    │  │  Mul, Div      │ │
│  │  Clone, Take   │  │  HeapAlloc     │  │  Mod, Neg      │ │
│  │  (12 تعليمة)   │  │  Free          │  │  (6 تعليمات)   │ │
│  └────────────────┘  └────────────────┘  └────────────────┘ │
│                                                              │
│  ┌────────────────┐  ┌────────────────┐  ┌────────────────┐ │
│  │  التحكم        │  │  الدوال        │  │  الأنواع       │ │
│  │  Jump, Branch  │  │  Call          │  │  Cast          │ │
│  │  Switch        │  │  CallIndirect  │  │  TypeCheck     │ │
│  │  Return        │  │  CallMethod    │  │  Sizeof        │ │
│  │  (5 تعليمات)   │  │  (3 تعليمات)   │  │  (3 تعليمات)   │ │
│  └────────────────┘  └────────────────┘  └────────────────┘ │
│                                                              │
│  ┌────────────────┐  ┌────────────────┐  ┌────────────────┐ │
│  │  منخفض المستوى│  │  الذرّية       │  │  CPU/أنظمة     │ │
│  │  RawLoad/Store │  │  AtomicLoad    │  │  Cli, Sti, Hlt │ │
│  │  PortIn/Out    │  │  AtomicStore   │  │  Cpuid, Rdmsr  │ │
│  │  Int, Iret     │  │  AtomicCmpXchg │  │  ReadCr, Lgdt  │ │
│  │  (24 تعليمة)   │  │  MemFence      │  │  SaveContext   │ │
│  └────────────────┘  └────────────────┘  └────────────────┘ │
└──────────────────────────────────────────────────────────────┘
```

**خط أنابيب الترجمة التفصيلي:**

```
llvm_compiler_pipeline.cpp (1,067 سطر):

1. Source → Tokens
   └── LexerCore::tokenize()

2. Tokens → AST
   └── ParserCore::parse()

3. AST → Type Check
   └── TypeChecker::check()  ← يضيف معلومات الأنواع

4. AST → SIR
   └── SIRBuilder::build()
       ├── sir_builder_statements.cpp  (جمل)
       ├── sir_builder_calls.cpp       (استدعاءات)
       ├── sir_builder_classes.cpp     (أصناف)
       ├── sir_builder_control_flow.cpp(تحكم)
       └── sir_builder_helpers.cpp     (مساعدات)

5. SIR → Optimize
   └── SIROptimizer::optimize()

6. SIR → LLVM IR
   └── LLVMCodeGen::generate()
       └── sir_to_llvm_v2.cpp (الملف الرئيسي — 61K bytes)

7. LLVM IR → Object Code
   └── LLVM TargetMachine::emit()

8. Object Code → Executable
   └── Linker (system ld/lld)
```

**الملفات الرئيسية (المترجم):**

| الملف | السطور | الدور |
|-------|--------|-------|
| `sir_opcodes.h` | 908 | تعريف 90+ تعليمة SIR |
| `sir_to_llvm_v2.cpp` | ~2000+ | تحويل SIR → LLVM IR |
| `llvm_compiler_pipeline.cpp` | 1,067 | خط الأنابيب الرئيسي |
| `sir_builder_*.cpp` | 13 ملف | AST → SIR |
| `llvm_codegen_part*.cpp` | 13 جزء | أجزاء LLVM codegen |
| `target_list.cpp` | 679 | قائمة الأهداف المدعومة |

---

### المسار C: المنقّلات (Transpiler Path)

> **الاستخدام:** استهداف منصات لها واجهات أصلية (Android Compose, iOS SwiftUI, Web HTML).

```
ملف .ص ──▶ Lexer ──▶ Parser ──▶ AST
                                  │
               ┌──────────────────┼──────────────────┐
               │                  │                  │
       ┌───────▼───────┐ ┌───────▼───────┐ ┌───────▼───────┐
       │ Android       │ │  iOS          │ │  Web          │
       │ Compose       │ │  SwiftUI      │ │  HTML/CSS     │
       │ Transpiler    │ │  Transpiler   │ │  Emitter      │
       │               │ │               │ │               │
       │ 1,412 سطر     │ │ 1,018 سطر    │ │ 52 سطر       │
       │ android_      │ │ ios_          │ │ html_         │
       │ compose.cpp   │ │ swiftui.cpp   │ │ emitter.cpp   │
       └───────┬───────┘ └───────┬───────┘ └───────┬───────┘
               │                  │                  │
               ▼                  ▼                  ▼
       ┌───────────────┐ ┌───────────────┐ ┌───────────────┐
       │  Kotlin Code  │ │  Swift Code   │ │  HTML/CSS/JS  │
       │  مع Compose   │ │  مع SwiftUI   │ │  مع RTL       │
       │               │ │               │ │               │
       │  @Composable  │ │  struct View  │ │  <div dir=rtl>│
       │  fun App() {  │ │  { body {     │ │  <script>     │
       │    Column {   │ │    VStack {   │ │               │
       │      Text()   │ │      Text()   │ │               │
       │    }          │ │    }          │ │               │
       │  }            │ │  } }         │ │               │
       └───────┬───────┘ └───────┬───────┘ └───────┬───────┘
               │                  │                  │
               ▼                  ▼                  ▼
       ┌───────────────┐ ┌───────────────┐ ┌───────────────┐
       │  Gradle Build │ │  Xcode Build  │ │  Browser      │
       │     ↓         │ │     ↓         │ │     ↓         │
       │  APK / AAB    │ │  IPA          │ │  SPA          │
       └───────────────┘ └───────────────┘ └───────────────┘
           Android            iOS              الويب
```

---

## 3. تفصيل كل منصة هدف

---

### 3.1 سطح المكتب (Desktop: Windows / Linux / macOS)

```
┌─────────────────────────────────────────────────────────┐
│  Desktop Target                                         │
│                                                         │
│  المسارات المتاحة:                                      │
│  ────────────────                                        │
│  1. المفسر (sad.exe شغّل ملف.ص)                         │
│     → تنفيذ فوري بدون ترجمة                             │
│     → يستخدم SDL2 + OpenGL للواجهات                     │
│                                                         │
│  2. المترجم (sadc ملف.ص --هدف سطح_مكتب)                │
│     → AST → SIR → LLVM IR → x86_64/ARM64 native       │
│     → ملف تنفيذي مستقل (.exe / ELF / Mach-O)          │
│     → رابط: sad_graphics (SDL2) + system libs          │
│                                                         │
│  الملفات الموجودة فعلاً:                                │
│  ─────────────────────                                   │
│  ✅ sad.exe (المفسر) — يعمل                              │
│  ✅ sadc (المترجم) — يعمل مع LLVM 18                    │
│  ✅ targets/x86_64.cpp — إعدادات Intel/AMD              │
│  ✅ targets/arm64.cpp — إعدادات Apple Silicon           │
│  ✅ SDL2 + OpenGL مدمج في graphics/                      │
│  ✅ sad_backend_desktop.h/cpp — واجهات SDL2              │
│                                                         │
│  الحالة: ✅ يعمل — يحتاج تثبيت وتحسين                   │
└─────────────────────────────────────────────────────────┘
```

**مسار الكود بالتفصيل:**
```
ملف.ص
  │
  ├──[المفسر]──▶ Lexer → Parser → AST → InterpreterCore
  │                                        │
  │                                   ExpressionEvaluator
  │                                   StatementExecutor
  │                                   BuiltinRegistry (500+ دالة)
  │                                        │
  │                                   إذا تضمن واجهات:
  │                                   → sad_ui_unified.h
  │                                   → sad_backend_desktop (SDL2)
  │                                   → نافذة SDL2 + OpenGL render
  │                                        │
  │                                        ▼
  │                                   ═══ نتيجة فورية ═══
  │
  └──[المترجم]──▶ Lexer → Parser → AST
                                      │
                              SIRBuilder (13 ملف)
                                      │
                              SIR (90+ تعليمة)
                                      │
                              SIROptimizer
                                      │
                              LLVMCodeGen → LLVM IR
                                      │
                              LLVM TargetMachine
                                      │
                              ┌───────┼───────┐
                              │               │
                          x86_64          ARM64
                          Windows         macOS
                          Linux           Linux ARM
                              │               │
                              ▼               ▼
                          .exe / ELF     Mach-O / ELF
```

---

### 3.2 الويب (Web: WASM + HTML/CSS/JS)

```
┌─────────────────────────────────────────────────────────┐
│  Web Target — مساران                                    │
│                                                         │
│  ═══════════════════════════════════════                 │
│  المسار 1: المفسر عبر WASM (الحالي)                     │
│  ═══════════════════════════════════════                 │
│                                                         │
│  sad.exe ──[Emscripten]──▶ sad.wasm                     │
│                                                         │
│  المفسر نفسه يعمل في المتصفح!                           │
│  يقرأ كود .ص ويفسره في WASM                             │
│                                                         │
│  الملفات:                                               │
│  ✅ cmake/wasm.cmake (102 سطر)                           │
│  ✅ tools/wasm/sad_wasm.cpp — نقطة الدخول               │
│  ✅ tools/wasm/wasm_builtins.cpp — دوال مدمجة للويب      │
│  ✅ build_wasm/sad.wasm — مترجم ومبني ✓                  │
│  ✅ build_wasm/sad.js — JS glue                          │
│                                                         │
│  الحالة: ✅ يعمل — يُستخدم في Playground                 │
│                                                         │
│  ═══════════════════════════════════════                 │
│  المسار 2: ترجمة أصلية إلى WASM (المستقبل)              │
│  ═══════════════════════════════════════                 │
│                                                         │
│  .ص → AST → SIR → LLVM IR → WASM32 binary              │
│                                                         │
│  الملفات:                                               │
│  ✅ wasm_codegen.cpp (472 سطر) — LLVM → WASM            │
│  ✅ targets/wasm32_target.cpp — WASI + Emscripten        │
│  ✅ wasm_bindgen.cpp — JS bindings                       │
│  ⚠️ html_emitter.cpp (52 سطر) — أساسي جداً              │
│                                                         │
│  الحالة: ⚠️ هيكل موجود — يحتاج تكامل واختبار           │
│                                                         │
│  ═══════════════════════════════════════                 │
│  UI في الويب                                            │
│  ═══════════════════════════════════════                 │
│                                                         │
│  sad_backend_wasm.h/cpp → Canvas2D via Emscripten        │
│  يرسم الـ widgets مباشرة على Canvas                      │
│  أو: html_emitter.cpp → HTML/CSS RTL-first              │
│                                                         │
│  الحالة: ⚠️ Backend موجود — يحتاج ربط بـ UI framework   │
└─────────────────────────────────────────────────────────┘
```

**مسار الكود:**
```
ملف.ص
  │
  ├──[المسار 1: المفسر في WASM]
  │   │
  │   sad_wasm.cpp يستقبل الكود كـ string
  │       │
  │   LexerCore → ParserCore → AST → InterpreterCore
  │       │
  │   wasm_builtins.cpp يوفر:
  │   - اطبع() → console.log()
  │   - ادخال() → prompt()
  │       │
  │   النتيجة تظهر في console المتصفح
  │
  └──[المسار 2: ترجمة أصلية]
      │
      sadc ملف.ص --هدف wasm32
          │
      AST → SIR → LLVM IR
          │
      LLVMCodeGen مع wasm32_target:
      - WASI environment → WASM standalone
      - Emscripten env  → WASM + JS
      - Browser env     → WASM + HTML
          │
      wasm_bindgen.cpp يُنشئ JS wrappers
          │
      النتيجة: .wasm + .js + .html
```

---

### 3.3 أندرويد (Android)

```
┌─────────────────────────────────────────────────────────┐
│  Android Target — مساران                                │
│                                                         │
│  ═══════════════════════════════════════                 │
│  المسار 1: Transpiler إلى Kotlin Compose (الرئيسي)      │
│  ═══════════════════════════════════════                 │
│                                                         │
│  .ص → AST → Kotlin/Jetpack Compose code → Gradle → APK │
│                                                         │
│  android_compose.cpp (1,412 سطر) يحوّل:                 │
│  - صنف → Kotlin class                                   │
│  - دالة → fun                                           │
│  - عمود() → Column {}                                   │
│  - صف() → Row {}                                        │
│  - نص_واجهة() → Text()                                  │
│  - زر() → Button()                                      │
│                                                         │
│  الملفات:                                               │
│  ✅ android_compose.cpp (1,412 سطر)                      │
│  ✅ compose_emitter.cpp — emitter خفيف (C API)          │
│  ✅ targets/android_ndk.cpp — NDK config                 │
│  ✅ tools/android/sad_android_builder.cpp                │
│  ✅ tools/apk_builder/ — بناء APK                        │
│  ✅ templates/android/ — قوالب مشروع Android             │
│  ✅ examples/app.apk — ملف APK مُنتج! ✓                 │
│                                                         │
│  الحالة: ✅ يعمل جزئيًا — transpiler مكتمل، بناء APK     │
│  موجود لكن يحتاج تصفية واختبار                          │
│                                                         │
│  ═══════════════════════════════════════                 │
│  المسار 2: LLVM Native + NDK (المستقبل)                 │
│  ═══════════════════════════════════════                 │
│                                                         │
│  .ص → AST → SIR → LLVM IR → ARM64 .so → NDK → APK     │
│                                                         │
│  الملفات:                                               │
│  ✅ targets/arm64.cpp — ARM64 target                     │
│  ⚠️ llvm_codegen_android.cpp — أساسي                    │
│  ✅ sad_backend_android.h/cpp — EGL+GLES3 backend       │
│                                                         │
│  الحالة: ⚠️ الهيكل موجود — يحتاج تكامل كامل            │
└─────────────────────────────────────────────────────────┘
```

**مسار الكود (Transpiler):**
```
ملف.ص
  │
  Parser → AST
  │
  AndroidComposeTranspiler::transpile(ast)
  │
  ├── visitClassDecl() → "class ClassName {"
  ├── visitFuncDecl()  → "fun funcName() {"
  ├── visitIfStmt()    → "if (cond) {"
  ├── visitWhileStmt() → "while (cond) {"
  │
  ├── visitUIColumn()  → "Column {"
  ├── visitUIRow()     → "Row {"
  ├── visitUIText()    → "Text(text, style=...)"
  ├── visitUIButton()  → "Button(onClick=...) {"
  │
  └── إنتاج ملفات:
      ├── MainActivity.kt
      ├── App.kt
      ├── build.gradle.kts
      └── AndroidManifest.xml
          │
          Gradle Build
          │
          app.apk
```

---

### 3.4 iOS

```
┌─────────────────────────────────────────────────────────┐
│  iOS Target                                             │
│                                                         │
│  .ص → AST → Swift/SwiftUI code → Xcode → IPA          │
│                                                         │
│  ios_swiftui.cpp (1,018 سطر) يحوّل:                    │
│  - صنف → Swift class                                    │
│  - دالة → func                                          │
│  - عمود() → VStack {}                                   │
│  - صف() → HStack {}                                     │
│  - نص_واجهة() → Text()                                  │
│  - زر() → Button() {}                                   │
│                                                         │
│  الملفات:                                               │
│  ✅ ios_swiftui.cpp (1,018 سطر)                          │
│  ✅ swiftui_emitter.cpp — emitter خفيف                  │
│  ✅ targets/ios_toolchain.cpp — iOS toolchain            │
│                                                         │
│  الحالة: ⚠️ Transpiler أساسي موجود — يحتاج:            │
│  - اختبار شامل                                          │
│  - تكامل Xcode build                                    │
│  - لا يعمل على Windows (يحتاج macOS)                    │
└─────────────────────────────────────────────────────────┘
```

---

### 3.5 Embedded / Bare Metal

```
┌─────────────────────────────────────────────────────────┐
│  Embedded / Bare Metal Target                           │
│                                                         │
│  .ص → AST → SIR → LLVM IR → bare metal binary          │
│                                                         │
│  وضع no_std / freestanding:                             │
│  - لا مكتبة قياسية (لا libc)                            │
│  - لا heap allocation                                    │
│  - لا exceptions                                         │
│  - وصول مباشر للأجهزة (ports, interrupts, memory)       │
│                                                         │
│  تعليمات SIR المنخفضة المستوى:                          │
│  - RawLoad8/16/32/64, RawStore8/16/32/64                │
│  - PortIn8/16/32, PortOut8/16/32                         │
│  - Cli, Sti, Hlt, Pause, Nop                            │
│  - Int, Iret (مقاطعات)                                   │
│  - Lgdt, Lidt, Lldt, Ltr (جداول وصفية)                  │
│  - AtomicLoad/Store/CmpXchg (عمليات ذرية)               │
│  - SaveContext, RestoreContext (تبديل السياق)            │
│  - Invlpg, Invpcid (ذاكرة افتراضية)                     │
│                                                         │
│  الملفات:                                               │
│  ✅ pipeline/freestanding_codegen.cpp                    │
│  ✅ pipeline/no_std_mode.cpp                             │
│  ✅ backend/interrupt_handlers.cpp                       │
│  ✅ backend/panic_handler.cpp                            │
│  ✅ backend/linker_script.cpp                            │
│  ✅ stdlib/low_level/ (26 ملف)                           │
│  ✅ stdlib/freestanding/نواة_منخفضة.ص                    │
│  ✅ targets/arduino.cpp                                  │
│  ✅ targets/esp32.cpp                                    │
│                                                         │
│  دليل العمل: BeeOS — نظام تشغيل مبني بالكامل بلغة ص    │
│                                                         │
│  الحالة: ✅ يعمل — BeeOS دليل حقيقي على القدرة          │
└─────────────────────────────────────────────────────────┘
```

---

## 4. طبقة واجهات المستخدم الموحدة (Unified UI Layer)

```
┌─────────────────────────────────────────────────────────┐
│  Sad UI Framework — التصميم المعماري                     │
│                                                         │
│  ┌─────────────────────────────────────────────────────┐│
│  │  الطبقة العليا: API عربي (واجهات.ص)               ││
│  │                                                     ││
│  │  عمود(), صف(), نص_واجهة(), زر(), حقل(),            ││
│  │  قائمة(), بطاقة(), صورة(), شريط_تنقل(),            ││
│  │  حوار(), نافذة(), تطبيق(), مشهد_3D()               ││
│  │  60+ دالة مصنع بأسماء عربية                         ││
│  └────────────────────────┬────────────────────────────┘│
│                           │                             │
│  ┌────────────────────────▼────────────────────────────┐│
│  │  الطبقة الوسطى: WidgetType enum (140+ نوع)        ││
│  │                                                     ││
│  │  sad_ui_unified.h:                                  ││
│  │  struct SadWidget {                                 ││
│  │    WidgetType type;     // نوع المكون               ││
│  │    string id;           // معرّف                    ││
│  │    string text;         // نص                       ││
│  │    int width, height;   // أبعاد                    ││
│  │    string color;        // لون                      ││
│  │    vector<SadWidget*> children; // أبناء            ││
│  │    map<string,string> props;   // خصائص إضافية      ││
│  │  }                                                  ││
│  │                                                     ││
│  │  abstract class SadUIBackend {                      ││
│  │    virtual void createWindow(...)                   ││
│  │    virtual void renderWidget(SadWidget*)            ││
│  │    virtual void handleEvent(...)                    ││
│  │  }                                                  ││
│  └────────────────────────┬────────────────────────────┘│
│                           │                             │
│  ┌────────────────────────▼────────────────────────────┐│
│  │  الطبقة السفلى: Backends المنصات                   ││
│  │                                                     ││
│  │  ┌──────────┐ ┌──────────┐ ┌────────┐ ┌──────────┐ ││
│  │  │ Desktop  │ │ Android  │ │  iOS   │ │   Web    │ ││
│  │  │ SDL2 +   │ │ EGL +    │ │ Metal  │ │ Canvas2D │ ││
│  │  │ OpenGL   │ │ GLES3    │ │ UIKit  │ │ via      │ ││
│  │  │          │ │ via NDK  │ │        │ │ Emscr.   │ ││
│  │  └──────────┘ └──────────┘ └────────┘ └──────────┘ ││
│  │                                                     ││
│  │  التحديد التلقائي:                                  ││
│  │  sad_ui_platform.h:                                 ││
│  │  #if __ANDROID__     → SadBackendAndroid            ││
│  │  #elif __APPLE__     → SadBackendIOS / Desktop      ││
│  │  #elif __EMSCRIPTEN__→ SadBackendWasm               ││
│  │  #else               → SadBackendDesktop            ││
│  └─────────────────────────────────────────────────────┘│
└─────────────────────────────────────────────────────────┘
```

**كيف يعمل عبر Transpilers:**
```
عندما يُستخدم مع Android transpiler:
  عمود(نص_واجهة("مرحبا"), زر("اضغط"))
    ↓
  Column { Text("مرحبا"); Button("اضغط") }

عندما يُستخدم مع iOS transpiler:
  عمود(نص_واجهة("مرحبا"), زر("اضغط"))
    ↓
  VStack { Text("مرحبا"); Button("اضغط") { } }

عندما يُستخدم مع المفسر (Desktop/WASM):
  عمود(نص_واجهة("مرحبا"), زر("اضغط"))
    ↓
  SadWidget(COLUMN, children=[
    SadWidget(TEXT, "مرحبا"),
    SadWidget(BUTTON, "اضغط")
  ])
    ↓
  SadBackendDesktop::renderWidget() → SDL2 draw calls
```

---

## 5. ملخص حالة كل منصة

| المنصة | المسار | الملفات | الحالة | الجهد المتبقي |
|--------|--------|---------|--------|--------------|
| **Desktop (Win/Linux/macOS)** | المفسر + المترجم LLVM | `sad.exe`, `sadc`, SDL2 backend | ✅ يعمل | تثبيت الأخطاء |
| **WASM (المفسر)** | Emscripten build | `sad.wasm`, `wasm_builtins.cpp` | ✅ يعمل | Playground UI |
| **WASM (مترجم)** | LLVM → WASM32 | `wasm_codegen.cpp`, `wasm32_target.cpp` | ⚠️ هيكل | تكامل + اختبار |
| **Android (Transpiler)** | AST → Kotlin Compose | `android_compose.cpp` (1,412 سطر) | ⚠️ أساسي | تحسين + اختبار |
| **Android (Native)** | LLVM → ARM64 .so → NDK | `arm64.cpp`, `android_ndk.cpp` | ⚠️ هيكل | بناء كامل |
| **iOS (Transpiler)** | AST → Swift/SwiftUI | `ios_swiftui.cpp` (1,018 سطر) | ⚠️ أساسي | Xcode تكامل |
| **Arduino** | LLVM → AVR | `arduino.cpp` | ⚠️ config فقط | runtime كامل |
| **ESP32** | LLVM → Xtensa/RISC-V | `esp32.cpp` | ⚠️ config فقط | runtime كامل |
| **Bare Metal** | LLVM → bare metal | 26+ ملف low-level | ✅ BeeOS يعمل | تثبيت |

---

## 6. رسم بياني: من كود ص إلى كل المنصات

```
                        ╔═══════════════╗
                        ║   كود .ص      ║
                        ║   عربي 100%   ║
                        ╚══════╤════════╝
                               │
                    ┌──────────┼──────────┐
                    │          │          │
              ┌─────▼─────┐┌──▼──┐ ┌─────▼─────┐
              │  المفسر   ││sadc │ │ Transpiler │
              │  (فوري)   ││LLVM │ │            │
              └─────┬─────┘└──┬──┘ └─────┬─────┘
                    │         │          │
    ┌───────────────┤    ┌────┤     ┌────┤
    ▼         ▼     │    ▼    ▼     ▼    ▼
 ┌──────┐┌──────┐   │ ┌────┐┌───┐┌────┐┌─────┐
 │ 🖥️   ││ 🌐   │   │ │ ⚡  ││📱 ││🌐  ││ 🔧  │
 │Desktop││WASM  │   │ │x86 ││ARM││WASM││Bare │
 │SDL2  ││Emscr.│   │ │_64 ││64 ││32  ││Metal│
 └──────┘└──────┘   │ └────┘└───┘└────┘└─────┘
                    │                    ┌─────┐
                    │ ┌────┐ ┌─────┐    │📱   │
                    │ │📱  │ │🍎   │    │ESP32│
                    │ │Kotn│ │Swift│    │Ardu │
                    │ │APK │ │IPA  │    └─────┘
                    │ └────┘ └─────┘
                    │ ┌──────┐
                    │ │🌐    │
                    │ │HTML  │
                    │ │CSS/JS│
                    │ └──────┘
                    │
            ══════════════════════
            9 أهداف من كود واحد
            ══════════════════════
```

---

*أُعدّت: مارس 2026 — لغة ص البرمجية*
*ملحق لـ: STRATEGIC_PLAN_V1.md*
