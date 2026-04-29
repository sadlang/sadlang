# لغة ص (Sad Programming Language)

[![Release](https://img.shields.io/github/v/release/sad-lang/sad-language?include_prereleases&label=الإصدار)](https://github.com/sad-lang/sad-language/releases)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Website](https://img.shields.io/badge/الموقع-sad--lang.org-brightgreen)](https://sad-lang.org)

لغة ص هي لغة برمجة عربية حديثة تدعم العربية والإنجليزية في الكلمات المفتاحية والمُعرّفات والنصوص،
وتوفّر **ثلاثة مسارات تنفيذ متوازية مستقلة** تشترك فقط في طبقة `shared/`:

| | المسار | المجلد | المخرج |
|---|---|---|---|
| 🟩 | **مفسر شجري (Tree-Walking)** | `interpreter/` | `sad-run.exe` |
| 🟧 | **آلة افتراضية (Bytecode VM + JIT)** | `vm/` | تنفيذ بايت كود |
| 🟪 | **مترجم أصلي (LLVM AOT)** | `compiler/` | `sad-build.exe` → ملف `.exe` |

> الآلة الافتراضية ليست جزءاً من المفسر الشجري — هما مساران منفصلان متوازيان، ولا أحد منهما يستورد من الآخر.

## 🎯 الواجهة الموحَّدة (sad-hub)

كل الأدوات تُستدعى عبر نقطة دخول واحدة `sad`:

```powershell
sad run x.ص          # تشغيل بالمفسر (← sad-run)
sad build x.ص        # ترجمة بـ LLVM (← sad-build)
sad check x.ص        # فحص الملكية (← sad-check)
sad fmt x.ص          # تنسيق الكود (← sad-fmt)
sad repl             # وضع تفاعلي (← sad-repl)
sad --list           # عرض كل الأدوات المتاحة
```

`sad.exe` نفسه = `sad-hub.exe` (موزّع موحَّد). كل أداة هي تنفيذي مستقل (`sad-<role>.exe`) يمكن استخدامه مباشرة.

📖 **[تصميم مركز الأدوات →](docs/architecture-tools-hub.md)**

## 📥 التثبيت السريع

### Windows
```powershell
irm https://sad-lang.org/install.ps1 | iex
```

### Linux / macOS
```bash
curl -fsSL https://sad-lang.org/install.sh | sh
```

### مديرو الحزم
```bash
# Windows (winget)
winget install SadLang.Sad

# Windows (scoop)
scoop install sad

# macOS / Linux (homebrew)
brew install sad-lang/tap/sad
```

📦 **[تحميل الإصدار الأخير →](https://github.com/sad-lang/sad-language/releases/latest)**

---

## نظرة سريعة

- نحو عربي واضح (`دالة`, `إذا`, `لكل`, `نهاية`)
- UTF-8 كامل في الكود والتوثيق
- مكتبة قياسية عربية (core, io, math, string, network, ...)
- أدوات تطوير: LSP، Formatter، Package Manager، REPL
- اختبارات شاملة (Comprehensive + Regression)

## مثال سريع

```sad
دالة جمع(أ، ب)
   ارجع أ + ب
نهاية

متغير الناتج = جمع(3، 5)
اطبع_سطر("الناتج: " + الناتج)
```

## 🏛️ بنية المشروع — البنية الثلاثية

```
┌─────────────────────────────────────────────────────────────┐
│  🟦 الطبقة 0 — النواة المشتركة (shared/)                    │
│  Lexer + Parser + AST + Types + Semantic + Errors           │
│  + Builtins/runtime + Modules                               │
└─────────────────────────────────────────────────────────────┘
         ▲              ▲              ▲
         │ AST          │ AST          │ AST
         │              │              │
  ┌──────┴───────┐┌─────┴────────┐┌────┴────────────┐
  │ 🟩 المسار 1  ││ 🟧 المسار 2  ││ 🟪 المسار 3     │
  │ interpreter/ ││ vm/          ││ compiler/       │
  │              ││              ││                 │
  │ Tree-Walking ││ Bytecode VM  ││ AST → SIR →     │
  │ على AST      ││ + JIT (LLVM) ││ LLVM IR → .exe  │
  │              ││              ││                 │
  │ → sad-run    ││ → bytecode   ││ → sad-build     │
  └──────────────┘└──────────────┘└─────────────────┘
        sad_interpreter   sad_vm        sad_compiler
```

> **القاعدة المعمارية:** كل مسار يعتمد على `shared/` فقط. ممنوع تماماً أن يستورد أي مسار من مسار آخر.

> **توحيد الملكية:** فحص الملكية يُنفَّذ في `shared/ownership` فقط، لذا `sad-run` و `sad-build` و `sad-check` يعطون نتائج متطابقة بايت-ببايت. راجع `_scratch/ownership_symmetry_test.ص`.

### تفاصيل المكونات

| المكوّن | المسار | الدور | مكتبة CMake |
|---|---|---|---|
| 🟦 النواة المشتركة | `shared/` | Lexer + Parser + AST + Value + Semantic + Builtins | `sad_shared` |
| 🟩 المفسر الشجري | `interpreter/` | Tree-Walking على AST | `sad_interpreter` |
| 🟧 الآلة الافتراضية | `vm/` | AST → Bytecode → Stack VM (+JIT) | `sad_vm` |
| 🟪 المترجم الأصلي | `compiler/` | AST → SIR → LLVM IR → Native | `sad_compiler` |
| المكتبة القياسية | `stdlib/` | وحدات اللغة الأساسية | — |
| الأدوات | `tools/` | LSP + Formatter + Pkg + REPL | متعدد |
| البناء | `cmake/` | إعدادات CMake | — |
| التوثيق | `docs/` + `website/` | مرجع اللغة والموقع | — |

## البناء والتشغيل

```powershell
# تهيئة CMake لأول مرة
cmake -S . -B build

# 🟦 الطبقة المشتركة
cmake --build build --config Debug --target sad_shared

# 🎯 مركز الأدوات (sad.exe = sad-hub.exe)
cmake --build build --config Debug --target sad_hub

# 🟩 المسار 1 — بناء المفسر الشجري (sad-run.exe)
cmake --build build --config Debug --target sad

# 🟧 المسار 2 — بناء الآلة الافتراضية
cmake --build build --config Debug --target sad_vm

# 🟪 المسار 3 — بناء المترجم الأصلي (sad-build.exe)
cmake --build build --config Debug --target sadc

# ✓ فاحص الملكية الثابت (sad-check.exe)
cmake --build build --config Debug --target sad_check

# تشغيل ملف عبر المفسر (مباشرة أو عبر الهب)
.\build\bin\Debug\sad-run.exe examples\test_simple.ص
.\build\bin\Debug\sad.exe run examples\test_simple.ص

# ترجمة ملف إلى تنفيذي أصلي
.\build\bin\Debug\sad-build.exe examples\test_simple.ص -o test.exe
.\build\bin\Debug\sad.exe build examples\test_simple.ص -o test.exe

# فحص الملكية ثابتاً (بدون تنفيذ)
.\build\bin\Debug\sad-check.exe examples\test_simple.ص
.\build\bin\Debug\sad.exe check examples\test_simple.ص
```

### بناء الحزم (CPack)

المشروع يدعم **9 مكوّنات منفصلة** للتوزيع — كل مكوّن يمكن تثبيته مستقلاً:

| المكوّن | المحتوى |
|---|---|
| `hub` | sad.exe + sad-hub.exe (الواجهة الموحَّدة) |
| `runtime` | sad-run.exe + المكتبة القياسية |
| `compiler` | sad-build.exe (LLVM AOT) |
| `fmt` | sad-fmt.exe (المنسّق) |
| `check` | sad-check.exe (فاحص الملكية الثابت) |
| `tools-extra` | sad-lsp + sad-pkg + sad-repl + sad-profiler + sad-analyze |
| `mobile` | sad-ios + sad-android + sad-apk |
| `docs` | التوثيق والكتاب |
| `examples` | برامج .ص نموذجية |

```powershell
# توليد ZIPs لكل مكوّن
cd build; cpack -G ZIP -C Debug
```

### الاختبارات

```powershell
# Enable tests on configure
cmake -S . -B build -DBUILD_TESTS=ON

# Build comprehensive tests
cmake --build build --config Debug --target comprehensive_tests

# Run ctest suites
ctest --test-dir build -R Comprehensive
```

## التوثيق الأساسي

- المرجع الكامل: `docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md`
- البرمجة الكائنية: `docs/07_البرمجة_الكائنية.md`
- الخطة السنوية 2026: `docs/ANNUAL_PLAN_2026.md`
- مهام السبرنت: `docs/SPRINT_1_TASKS.md`

## المساهمة

- دليل المساهمة: `CONTRIBUTING.md`
- قواعد السلوك: `CODE_OF_CONDUCT.md`
- سياسة الأمان: `SECURITY.md`

## الترخيص

هذا المشروع مرخّص تحت MIT. راجع `LICENSE`.