# لغة ص (Sad Programming Language)

[![Release](https://img.shields.io/github/v/release/sad-lang/sad-language?include_prereleases&label=الإصدار)](https://github.com/sad-lang/sad-language/releases)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Website](https://img.shields.io/badge/الموقع-sad--lang.org-brightgreen)](https://sad-lang.org)

لغة ص هي لغة برمجة عربية حديثة تدعم العربية والإنجليزية في الكلمات المفتاحية والمُعرّفات والنصوص،
وتوفّر مسارين للتنفيذ:
- مفسر مباشر `sad`
- مترجم أصلي عبر LLVM `sadc`

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

## بنية المشروع

| المكوّن | المسار | الدور |
|---|---|---|
| النواة المشتركة | `shared/` | Lexer + Parser + AST + Value |
| المفسر | `interpreter_new/` | تنفيذ البرامج مباشرة |
| المترجم | `compiler_new/` | AST -> SIR -> LLVM -> Executable |
| الآلة الافتراضية | `vm/` | مسار bytecode |
| المكتبة القياسية | `stdlib/` | وحدات اللغة الأساسية |
| الأدوات | `tools/` | LSP + Formatter + Pkg + REPL |
| البناء | `cmake/` | إعدادات CMake |
| التوثيق | `docs/` + `website/` | مرجع اللغة والموقع |

## البناء والتشغيل

```powershell
# Configure
cmake -S . -B build

# Build interpreter
cmake --build build --config Debug --target sad

# Build compiler
cmake --build build --config Debug --target sadc

# Run a Sad file
.\build\bin\Debug\sad.exe examples\test_simple.ص
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