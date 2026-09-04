# لغة ص (Sad Programming Language)

[![Release](https://img.shields.io/github/v/release/sad-lang/sad-language?include_prereleases&label=الإصدار)](https://github.com/sad-lang/sad-language/releases)
[![License](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Website](https://img.shields.io/badge/الموقع-sad--lang.org-brightgreen)](https://sad-lang.org)

> ### 🔻 هذا المستودعُ مسارٌ فرعيٌّ: **المحرّكُ الواحد**
>
> نسخةٌ كاملةُ التاريخِ من `sadlang/s-programming-language` عند `d6baa0d8`، غرضُها
> حذفُ المفسّرِ وإسنادُ كلِّ ما كان يقدّمُه إلى المترجم. الأصلُ قائمٌ ومستقلٌّ ولا
> يستبدلُه هذا حتى يَثبُتَ وفاؤه. **القواعدُ والحالةُ المقيسةُ في
> [`CLAUDE.md`](CLAUDE.md).**
>
> العدّادُ الحاكم: بذورُ `@skip_compiler` الحيّة — **٨٠ → صفر**.

لغة ص هي لغة برمجة عربية حديثة تدعم العربية والإنجليزية في الكلمات المفتاحية والمُعرّفات والنصوص.

### مساراتُ التنفيذ

| | المسار | المجلد | المخرج | الحالةُ في هذا المستودع |
|---|---|---|---|---|
| 🟪 | **مترجمٌ أصليّ (LLVM AOT)** | `compiler/` | `sad-build.exe` → ملف `.exe` | **المحرّكُ الوحيد** |
| 🟫 | خلفيّةٌ أصليّةٌ بلا LLVM | `compiler/` | `sad-build-native.exe` | قائمةٌ ومحدودةُ التغطية |
| 🟩 | ~~مفسّرٌ شجريّ~~ | ~~`interpreter/`~~ | ~~`sad-run.exe`~~ | ✅ **حُذِف** — ١١٩ ملفًّا · ٦١٬٢١٣ سطرًا |

> **تصحيح**: النسخةُ الأمُّ من هذا الملفّ كانت تُعلِنُ مسارًا ثالثًا — «آلةٌ
> افتراضيّة (Bytecode VM + JIT)» في `vm/`. **لا وجودَ لمجلّد `vm/` في الشجرة**؛
> الإعلانُ كان وصفَ نيّةٍ لا وصفَ واقع.

## 🎯 الواجهة الموحَّدة (sad)

كل الأدوات تُستدعى عبر نقطة دخول واحدة `sad`:

```powershell
sad build x.ص        # ترجمة بـ LLVM (← sad-build)
sad check x.ص        # فحص الملكية (← sad-check)
sad fmt x.ص          # تنسيق الكود (← sad-fmt)
sad --list           # عرض كل الأدوات المتاحة

# ⛔ زالا مع المفسّر: `sad run` (كان ← sad-run) و`sad repl` (كان ← sad-repl).
#    والتشغيلُ اليومَ: sad build x.ص -o x.exe ثمّ ./x.exe
```

`sad.exe` هو الموزّع الموحَّد نفسه — اسمٌ واحدٌ لا نسخةٌ ولقب. وكل أداة تنفيذيٌّ مستقل (`sad-<role>.exe`) يمكن استخدامه مباشرة.

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
- أدوات تطوير: مركز الأدوات (`sad`)، LSP، Formatter، Package Manager
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
                        ▲                    ▲
                        │ AST                │ AST
                        │                    │
              ┌─────────┴────────┐┌──────────┴───────────┐
              │ 🟪 خلفيّة LLVM   ││ 🟫 الخلفيّة السياديّة │
              │ compiler/        ││ compiler/ (بلا LLVM) │
              │                  ││                      │
              │ AST → SIR →      ││ AST → SIR → تعليمات  │
              │ LLVM IR → .exe   ││ أصليّة مباشرةً        │
              │                  ││                      │
              │ → sad-build      ││ → sad-build-native   │
              └──────────────────┘└──────────────────────┘
                          sad_compiler
```

> **مسارُ المحرّكِ الواحد:** حُذِفَ المفسّرُ الشجريُّ (`interpreter/`، ١١٩ ملفًّا)
> وأداتُه `sad-run`. والمترجّمُ وحدَه ينفّذُ برامجَ ص، بخلفيّتَين: LLVM والسياديّة.

> **⚠️ تصويبٌ مُدوَّن:** كان هذا الرسمُ يعلنُ مسارًا ثالثًا `vm/` (آلةٌ افتراضيّةٌ
> بـJIT). **لا وجودَ له في الشجرة** — أُزيلَ في PR ‏#96، وبقيَ الرسمُ يُعلِنُه.
> وخريطةٌ تُعلِنُ ما لا وجودَ له أسوأُ من غيابِ الخريطة.

> **القاعدة المعمارية:** كلُّ مسارٍ يعتمدُ على `shared/` فقط. ممنوعٌ تمامًا أن
> يستوردَ أيُّ مسارٍ من مسارٍ آخر.

> **توحيد الملكية:** فحصُ الملكيّةِ يُنفَّذ في `shared/ownership` فقط، لذا
> `sad-build` و`sad-check` يعطيان نتائجَ متطابقةً بايت-ببايت.

### تفاصيل المكونات

| المكوّن | المسار | الدور | مكتبة CMake |
|---|---|---|---|
| 🟦 النواة المشتركة | `shared/` | Lexer + Parser + AST + Value + Semantic + Builtins | `sad_shared` |
| 🟪 المترجم الأصلي | `compiler/` | AST → SIR → LLVM IR → Native | `sad_compiler` |
| 🟫 الخلفيّة السياديّة | `compiler/` | AST → SIR → تعليمات أصليّة بلا LLVM | `sad_compiler` |
| المكتبة القياسية | `stdlib/` | وحدات اللغة الأساسية | — |
| الأدوات | `tools/` | LSP + Formatter + Pkg | متعدد |
| البناء | `cmake/` | إعدادات CMake | — |
| التوثيق | `docs/` + `website/` | مرجع اللغة والموقع | — |

## البناء والتشغيل

```powershell
# تهيئة CMake لأول مرة
cmake -S . -B build

# 🟦 الطبقة المشتركة
cmake --build build --config Debug --target sad_shared

# 🎯 مركز الأدوات (sad.exe)
cmake --build build --config Debug --target sad

# 🟪 المترجم الأصلي (sad-build.exe)
cmake --build build --config Debug --target sad-build

# 🟫 الخلفيّة السياديّة بلا LLVM (sad-build-native.exe)
cmake --build build --config Debug --target sad-build-native

# ✓ فاحص الملكية الثابت (sad-check.exe)
cmake --build build --config Debug --target sad-check

# ترجمة ملف إلى تنفيذي أصلي ثمّ تشغيله
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
| `hub` | sad.exe (الواجهة الموحَّدة) |
| `runtime` | المكتبة القياسية + أرشيفات زمن التشغيل |
| `compiler` | sad-build.exe (LLVM AOT) |
| `fmt` | sad-fmt.exe (المنسّق) |
| `check` | sad-check.exe (فاحص الملكية الثابت) |
| `tools-extra` | sad-lsp + sad-pkg + sad-analyze |
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