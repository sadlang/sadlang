# YAML Data Contracts — ربط الـstories بتصميم v1.0

> **الهدف:** مرجع موجز للمطوّر عن بنية YAML النهائية (v1.0). لا تكتب سطر واحد في `data/` دون قراءة هذا أولاً.
> **المرجع الأصلي:** [YAML_UNIFIED_SCHEMA_DESIGN.md](../../docplan/YAML_UNIFIED_SCHEMA_DESIGN.md) (v1.0 نهائي)
> **سياسة:** أي تغيير في البنية يتطلّب تعديل التصميم الأصلي أولاً، ثم هذا الملف.

---

## 1. البنية الجذرية

```
data/
├── _schemas/          → JSON Schemas + سياسات (تُنشأ في S-000a)
├── _meta/CODEOWNERS   → ملكية الكيانات (YAML — S-000a)
├── builtins/          → S-015a
├── keywords/          → S-015b
├── errors/            → S-015c
└── lessons/           → S-015d
```

## 2. تسمية مجلد الكيان (إلزامي)

```
<folder_name> == <kind>_<id.last_segment>
```

| صحيح | خطأ |
|--------|------|
| `data/builtins/builtin_print_line/` | `data/builtins/print_line/` |
| `data/keywords/keyword_if/` | `data/keywords/if/` |
| `data/errors/error_unexpected_token/` | `data/errors/E0001/` |
| `data/lessons/lesson_01_hello_world/` | `data/lessons/01_hello/` |

## 3. Whitelist صارم داخل أي مجلد كيان

فقط هذه العناصر مسموحة (أي شيء آخر يفشل validation):

| الملف/المجلد | إلزامي؟ | الغرض |
|--------------|-----------|--------|
| `_index.yaml` | ✅ دائماً | البيانات الأساسية + الميتاداتا |
| `docs.yaml` | مستحسن | الوصف التفصيلي + see_also |
| `examples/*.yaml` | اختياري | مثال لكل ملف |
| `exercises/*.yaml` | اختياري | تمرين لكل ملف |
| `i18n/{lang}.yaml` | اختياري | overlay ترجمات (en/fr/...) |

## 4. بنية `_index.yaml` (حسب الـkind)

### 4.1 Builtin

```yaml
schema_version: 1
id: builtin.print_line          # ASCII technical id
kind: builtin
name: اطبع_سطر                 # الاسم العربي الوحيد — لا aliases
category: io
since: 0.1.0

signature:
  params:
    - name: قيمة                  # عربي فقط
      type: any
      required: true
  returns: void
  is_variadic: false             # true يعني params الأخيرة (...اسم)

tooling:
  lsp:
    completion: true
    snippet: "اطبع_سطر(${1:قيمة})"
    documentation_hover: true
  formatter:
    breaks_line: true
  repl:
    suggest_after: ["متغير", "ثابت"]

version_info:
  added: 0.1.0
  deprecated: null
  replaced_by: null

owners: [@author1]              # يُحقَّق ضد _meta/CODEOWNERS (يفرضه S-009 loader)
```

### 4.2 Keyword

```yaml
schema_version: 1
id: keyword.if
kind: keyword
name: إذا
category: control_flow
reserved: true                  # true = محجوزة | false = سياقية
since: 0.1.0
contextual_scope: null          # للسياقية فقط (مثال: "after_function")
owners: [@core-team]
```

### 4.3 Error

```yaml
schema_version: 1
id: error.unexpected_token
kind: error
code: E0001                     # pattern: ^E\d{4}$ — معرِّف تقني ثابت
name: توكن_غير_متوقّع           # الاسم العربي للعرض (قابل للـoverride في i18n/*.yaml)
category: lexer                 # lexer | parser | type | runtime | security
severity: error                 # error | warning | hint
since: 0.1.0
owners: [@core-team]
```

> **ملاحظة:** `code` ASCII ثابت (للأدوات)، `name` نص للعرض (يمكن لـi18n إعطاء بديل بكل لغة).

### 4.4 Lesson

```yaml
schema_version: 1
id: lesson.01_hello_world
kind: lesson
title: "أول برنامج"
level: beginner                 # beginner | intermediate | advanced
order: 1
prerequisites: []
tags: [intro, io]
uses: [builtin.print_line]      # reverse-index source
owners: [@docs-team]
```

## 5. بنية `docs.yaml`

```yaml
schema_version: 1
summary: "سطر واحد يلخّص الكيان."          # إلزامي
description: |                                  # markdown داخل string — اختياري
  وصف متعدّد الأسطر.
  يدعم *italic* و `code` و [روابط](builtin.print_line).
notes:                                          # اختياري
  - "ملاحظة أولى"
  - "ملاحظة ثانية"
see_also:                                       # refs validated ضد reverse_index
  - builtin.print_line
  - builtin.read
related_lessons:
  - lesson.01_hello_world
```

## 6. بنية `examples/{name}.yaml`

```yaml
schema_version: 1
title: "عنوان المثال"
order: 1.0                      # float للإدراج بين 1.0 و 2.0
runnable: true                  # true = يُنفَّذ في Tier 3 | false = يُتخطّى
deterministic: true             # true = expected_output | false = expected_pattern
code: |
  اطبع_سطر("مرحبا")
expected_output: "مرحبا\n"        # deterministic فقط
# OR:
# expected_pattern: "^\\d{4}-\\d{2}-\\d{2}\\n$"  # non-deterministic فقط
```

**قواعد صارمة (يفرضها schema):**
- `runnable: false` ⟹ Tier 3 يتخطّى المثال (لا تنفيذ، لا فشل)
- `deterministic: true` ⟹ يجب وجود `expected_output` فقط (XOR)
- `deterministic: false` ⟹ يجب وجود `expected_pattern` فقط (XOR)
- استدعاء أي builtin مذكور في `_schemas/non_deterministic_builtins.yaml` ⟹ يفشل validator إذا `deterministic: true`

## 7. بنية `exercises/{name}.yaml`

```yaml
schema_version: 1
title: "عنوان التمرين"
difficulty: easy                # easy | medium | hard
topics: [io, basics]
prompt: "وصف المطلوب من المتعلّم."
starter_code: |
  # أكمل هنا
solution: |
  اطبع_سطر("حل")
test_strategy: regex            # exact | regex | property
expected_pattern: "^.+\\n$"
next_exercise: "builtin.print_line.exercises.print_twice"
```

**صيغة `next_exercise` (resolver رسمي):**
```
<kind>.<id_last_segment>.exercises.<exercise_filename_without_ext>
```
يُحلّ إلى المسار:
```
data/<kind>s/<kind>_<id_last_segment>/exercises/<exercise_filename>.yaml
```
مثال: `builtin.print_line.exercises.print_twice` → `data/builtins/builtin_print_line/exercises/print_twice.yaml`.

## 8. بنية `i18n/{lang}.yaml` (overlay فقط)

```yaml
schema_version: 1
name: "Print Line"               # override لـ_index.yaml.name (للأخطاء/الـbuiltins/الـkeywords)
docs:
  summary: "English summary."
  description: |
    English description.
  notes:
    - "Note 1"
examples:
  basic:
    title: "Simple print"        # فقط title يُترجم
exercises:
  print_name:
    title: "Print your name"
    prompt: "Write a program..."
```

**حقول قابلة للـoverride:** `name` (للعرض)، كل `docs.*`، `examples.<n>.title`، `exercises.<n>.{title, prompt}`.
**حقول لا تُترجم أبداً:** `id`, `code`, `expected_output`, `expected_pattern`, `signature.*.name`, `category`, `severity`, `reserved`, `kind`.

## 9. خريطة الـstories → الـcontracts

| Story | الـcontracts المتأثّرة |
|-------|----------------------|
| [S-000a](stories/S-000a-foundation-schemas.md) | إنشاء كل _schemas/*.schema.json + whitelist + naming_rules + CODEOWNERS |
| [S-009](stories/S-009-reader-api.md) | Reader API يحترم whitelist + naming_rules + i18n_policy + يفرض owners ⊆ CODEOWNERS |
| [S-009b](stories/S-009b-security-hardening.md) | path validators ترفض أي مسار خارج whitelist |
| [S-011](stories/S-011-watcher.md) | Watcher يغفل أي ملف خارج whitelist |
| [S-013](stories/S-013-tier3-snapshots.md) | Tier 3 يقرأ `examples/*.yaml` ويحترم runnable+deterministic XOR |
| [S-015a](stories/S-015a-migrate-builtins.md) | بنية builtin: `_index.yaml` + `docs.yaml` + `examples/*` |
| [S-015b](stories/S-015b-migrate-keywords.md) | بنية keyword: `_index.yaml` + `docs.yaml` |
| [S-015c](stories/S-015c-migrate-errors.md) | بنية error: `_index.yaml` + `docs.yaml` + `i18n/en.yaml` |
| [S-015d](stories/S-015d-migrate-lessons.md) | بنية lesson: كل الـwhitelist (الأغنى) |
| [S-015e](stories/S-015e-md-generator.md) | templates تقرأ كل الـwhitelist (لكل الكيانات) وتولّد MD |

## 10. سياسة الاسم الواحد (حرجة)

- ❌ لا `aliases` عربية (لا `اطبع` كاختصار لـ`اطبع_سطر`)
- ❌ لا `name_en` في `_index.yaml` — اللغة الافتراضية عربية
- ❌ لا أسماء بديلة للمعاملات
- ✅ `id` ASCII فقط للأدوات (file paths, JSON keys)
- ✅ `i18n/*.yaml` تترجم **النصوص فقط** (name display + docs)، لا تغيّر `id` ولا `code`

## 11. Cache (غير mutable — خارج data/)

```
.sadinfo_cache/                 ← git-ignored
├── entity_hashes.json
├── reverse_index.json
└── aggregated/
    ├── builtins.json
    ├── keywords.json
    ├── errors.json
    ├── lessons.json            ← مكافئ للكيانات الأخرى
    └── full_export.json
```

## 12. سياسة `schema_version` (الترقية)

- القيمة الحالية: **1** لكل الكيانات
- تغيير breaking ⟹ زيادة المؤشّر (1 → 2)
- عند وجود v2:
  1. تُكتب schemas جديدة `*.v2.schema.json` بجانب القديمة
  2. Loader يقبل الإصدارين ويوسم القديم `deprecated`
  3. Story جديدة (S-Mxx) تُهاجر الكيانات + script migration تلقائي
  4. بعد deadline (sprint كامل)، v1 schemas تُزال
- ممنوع تعديل semantics لحقل موجود دون bump؛ ممنوع إعادة استخدام اسم حقل بمعنى مختلف.
