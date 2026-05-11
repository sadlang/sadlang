# sadinfo — أداة فحص لغة ص / Sad Language Inspector

> **النسخة / Version:** 0.2.0  
> **مخطط البيانات / Schema:** 1.1.0  
> **الحالة / Status:** Story 1.2 ✓

أداة سطر أوامر خفيفة لاستخراج معلومات لغة ص بصيغ منظمة (JSON / YAML).
تُستخدم من المحررات، أنظمة CI، ومولدات الوثائق.

A lightweight CLI to inspect the Sad programming language metadata in
structured formats (JSON / YAML). Designed for editor integrations, CI
pipelines, and documentation generators.

---

## التثبيت / Installation

تُبنى مع المشروع الأساسي:

```powershell
cmake -S . -B build
cmake --build build --config Debug --target sadinfo
.\build\bin\Debug\sadinfo.exe --help
```

البناء سريع: الأداة تربط بـ `sad_shared` فقط (بلا LLVM/مفسر/مترجم).
Fast build: links only against `sad_shared` (no LLVM/interpreter/compiler).

---

## الاستخدام السريع / Quick Usage

```powershell
# 1) عرض جميع الكلمات المفتاحية بصيغة JSON (افتراضي)
sadinfo --dump-keywords

# 2) إخراج إلى ملف
sadinfo --dump-keywords -o keywords.json

# 3) صيغة YAML مدمجة
sadinfo --dump-keywords --format yaml --compact -o keywords.yaml

# 4) كلمات محجوزة فقط
sadinfo --dump-keywords --filter category=reserved

# 5) كلمات سياقية تعمل كفاتحات كتلة
sadinfo --dump-keywords --filter category=contextual --filter role=block_opener

# 6) فقط الكلمات التي لها aliases
sadinfo --dump-keywords --filter has-aliases

# 7) شامل المُهملة (للمراجعات)
sadinfo --dump-keywords --include-deprecated

# 8) مُجمَّعة بالفئة + بدون الإنجليزية (مخرج عربي صرف)
sadinfo --dump-keywords --by-category --lang ar

# 9) Story 1.3 — تفريغ الدوال المدمجة + الطرق المضمنة (15 دالة + 43 طريقة)
sadinfo --dump-builtins -o builtins.json

# 10) كل دوال التزامن فقط
sadinfo --dump-builtins --filter category=concurrency

# 11) كل طرق المصفوفات بصيغة YAML
sadinfo --dump-builtins --filter host=array --format yaml

# 12) مخرج مدمج عربي صرف للدوال
sadinfo --dump-builtins --minimal --lang ar
```

---

## جدول الخيارات / Options

| الخيار / Flag | الوصف (AR) | Description (EN) |
|---|---|---|
| `--dump-keywords` | استخراج بيانات الكلمات المفتاحية | Dump keyword metadata |
| `--dump-builtins` | استخراج الدوال المدمجة + الطرق المضمنة | Dump builtin functions + methods |
| `-o, --output <file>` | كتابة المخرج إلى ملف بدلاً من stdout | Write output to file |
| `--pretty` | تنسيق مقروء (افتراضي) | Human-readable format (default) |
| `--compact` | تنسيق مدمج بلا مسافات زائدة | Compact format |
| `--format <fmt>` | `json` (افتراضي) أو `yaml` | `json` (default) or `yaml` |
| `--lang <lang>` | `ar`/`en`/`both` (افتراضي) | Language filter |
| `--minimal` | حقول مختصرة فقط | Minimal field set |
| `--by-category` | تجميع المخرج حسب الفئة (keywords فقط) | Group output by category (keywords only) |
| `--include-deprecated` | شمول الكلمات المُهملة (keywords فقط) | Include deprecated entries (keywords only) |
| `--filter key=value` | تصفية (يمكن تكرارها) | Filter (repeatable) |
| `-h, --help` | عرض المساعدة | Show help |

### مفاتيح التصفية / Filter Keys

#### للكلمات / Keywords (`--dump-keywords`)

| المفتاح | القيمة | المثال |
|---|---|---|
| `category` | `reserved` \| `operator` \| `contextual` \| `builtin_type` | `--filter category=reserved` |
| `role` | اسم دور (مثل `block_opener`) | `--filter role=block_opener` |
| `token-type` | اسم TokenType الكامل | `--filter token-type=KEYWORD_FUNCTION` |
| `has-aliases` | (بدون قيمة) — مَن لديه aliases | `--filter has-aliases` |

#### للمدمجات / Builtins (`--dump-builtins`)

| المفتاح | القيمة | المثال |
|---|---|---|
| `category` | `printing` \| `input` \| `reflection` \| `type_ctor` \| `concurrency` | `--filter category=concurrency` |
| `host` | `array` \| `string` \| `map` \| `channel` (للطرق فقط) | `--filter host=array` |

التصفية تتراكم بمنطق AND. مفاتيح غير معروفة تُرفض بصراحة (exit=2).
Filters are AND-combined. Unknown keys are explicitly rejected (exit=2).

---

## مخطط المخرج / Output Schema

### الجذر / Root (default — flat list)

```json
{
  "schemaVersion": "1.1.0",
  "tool": "sadinfo",
  "toolVersion": "0.2.0",
  "totalCount": 90,
  "categoryCounts": {
    "reserved": 40, "operator": 3, "contextual": 38, "builtin_type": 9
  },
  "keywords": [ /* مُدخلات */ ]
}
```

مع `--by-category`: يُستبدل `keywords` بـ:

```json
"categories": {
  "reserved":      [ ... ],
  "operator":      [ ... ],
  "contextual":    [ ... ],
  "builtin_type":  [ ... ]
}
```

### المُدخل / Entry (ترتيب الحقول مضمون / field order is stable)

| الحقل | النوع | ملاحظات |
|---|---|---|
| `id` | string | معرف ثابت — مفتاح الترجمة الأساسي. لا يتغير عبر الإصدارات. |
| `word` | string | الكلمة العربية كما تظهر في الكود |
| `english` | string | الاسم الإنجليزي (يُحذف مع `--lang ar`) |
| `category` | string | `reserved` \| `operator` \| `contextual` \| `builtin_type` |
| `tokenType` | number | قيمة عددية لـ TokenType من المُحلل المعجمي |
| `emittedByLexer` | boolean | هل يُنتجها المُحلل المعجمي مباشرة |
| `deprecated` | boolean | يظهر **فقط** عندما يكون `true` |
| `aliases` | string[] | أسماء بديلة (يظهر فقط إذا كان غير فارغ) |
| `roles` | string[] | أدوار نحوية (يظهر فقط إذا كان غير فارغ) |

### قاعدة الـ id الثابت / Stable ID Rule

`id` مُشتق من `english` بشكل حتمي:

```
keyword.<english>           # category=reserved
operator.<english>          # category=operator
contextual.<english>        # category=contextual
type.<english>              # category=builtin_type
literal.<english>           # tokenType.startsWith("LITERAL_")
```

- `english` يُسوَّى: lowercase، استبدال المسافات/الشُرَط بـ `_`، حذف لاحقة `_deprecated`.
- مثال: `give_deprecated` → `id = "contextual.give"`.

> **ضمان عدم الكسر:** أي تعديل على `english` في `data/language/keywords.yaml`
> يكسر اختبار snapshot `tests/sadinfo/golden/keyword_ids.json` صراحة في CI.

---

## رموز الخروج / Exit Codes

| الكود | المعنى |
|---|---|
| 0 | نجاح |
| 1 | فشل تنفيذ (مثل تعذر فتح الملف) |
| 2 | خطأ في وسيطات سطر الأوامر |

---

## مكتبة YAML / YAML Library

**`yaml-cpp v0.8.0`** مُدمَجة عبر CMake `FetchContent` (الإعداد:
[`cmake/yaml_cpp.cmake`](../../cmake/yaml_cpp.cmake)). يُنتج `sadinfo`
مخرج YAML قانونياً (canonical) بـ `YAML::Emitter` مع حماية كاملة
للحروف العربية وعلامات الاقتباس.

### تعطيلها / Disabling

```powershell
cmake -S . -B build -DENABLE_YAML_CPP=OFF
```

عند التعطيل، يسقط `sadinfo` تلقائياً إلى **كاتب YAML يدوي مدمج**
(احتياط) يُنتج نفس البنية بدون أي اعتماديات. يُستخدم في:

- بيئات بناء معزولة بدون اتصال بالشبكة.
- بناءات بأقل قدر ممكن من التبعيات.

لا فرق في الـ schema أو في الحقول — فقط في طريقة الهروب
(yaml-cpp يستخدم تنسيقاً أكثر صرامة).

---

## الاختبارات / Tests

```powershell
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build --config Debug --target test_sadinfo_keyword_transforms
ctest --test-dir build -C Debug -R SadInfo --output-on-failure
```

- `SadInfoKeywordTransforms` — 291 فحص للدوال النقية (تحويلات/تصفية/تجميع).
- `SadInfoGoldenIds` — مقارنة snapshot لـ 91 معرفاً ثابتاً.

---

## التغييرات / Changelog

راجع [CHANGELOG.md](CHANGELOG.md).
