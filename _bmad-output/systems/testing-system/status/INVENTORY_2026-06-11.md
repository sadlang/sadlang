---
title: "INVENTORY — جرد اختبارات لغة ص الحالية"
date: 2026-06-11
story: TEST-001
author: Amelia (Senior Software Engineer)
method: "Glob/Grep/find — قراءة فقط، بلا نقل (GR-01: بيانات فعلية مقيسة)"
---

# INVENTORY — جرد الاختبارات الحالية (T1, T2)

> مقيس فعلياً عبر `find` على `tests/` بتاريخ 2026-06-11. الأعداد = عدد الملفات
> الفعلي لكل امتداد. لا تخمين (GR-01).

## 1. تصنيف المجلدات → الطبقات الثلاث (AC-01)

### الطبقة 2 — سلوك (.ص، تنفيذ مزدوج) → `behavior/`

| المجلد القديم | عدد .ص | تصنيف | الوجهة |
|---|---|---|---|
| `dual_execution` | 837 | **المصدر الموثوق** (features الـ12 + P0_smoke) | `behavior/` (العمود) |
| `sad_examples` | 255 | أمثلة — مرشّح لتكامل/أرشفة | `behavior/sections/12_تكامل` أو `_archive` |
| `root_cleanup` | 145 | مبعثر — يحتاج تصنيف فردي | فرز → الأقسام أو `_archive` |
| `compiler` | 102 | سلوكي للمترجم | `behavior/` حسب الميزة |
| `regression` | 72 | منع تراجع | `behavior/sections/*/_regression` |
| `compiler_features` | 50 | ميزات المترجم | `behavior/` حسب الميزة |
| `stress` | 37 | ضغط — ليس سلوكياً صرفاً | `system/benchmark` (مراجعة) |
| `compiler_rules` | 29 | مصفوفة قواعد | `behavior/rules_matrix` |
| `ownership` | 20 | ملكية | `behavior/sections/08` + `unit/ownership` |
| `dev_tools_test` | 14 | أدوات تطوير | `system/` (مراجعة) |
| `modules` | 11 | استيراد | `behavior/sections/10_الاستيراد` |
| `quick` | 10 | فحص سريع بالمفسر | دمج في `P0_smoke`/أقسام |
| `stdlib` | 9 (+11cpp) | مكتبة (مختلط) | `behavior/09` + `unit/` |
| `grammar` | 8 | نحو | `behavior/sections/01` |
| `type_system` | 8 | أنواع | `behavior/02` + `unit/types` |
| `توثيق` | 6 | أمثلة توثيق | `system/docs` |
| `bare_metal` | 4 | منخفض المستوى | `behavior/sections/08` |
| `sad_tests` | 3 | عينات | `_archive` (مكرّر) |
| `docs_extraction` | 2 (+py) | استخراج توثيق | `system/docs` |
| `hub` | 2 | موزّع الأوامر | `system/` |
| `generics`,`safety`,`freestanding` | 1 لكلٍّ | شظايا | فرز → أقسام أو `_archive` |

### الطبقة 1 — وحدة (C++) → `unit/`

| المجلد القديم | عدد .cpp | تصنيف | الوجهة |
|---|---|---|---|
| `comprehensive` | 29 | حزمة شاملة (~900 اختبار) | `unit/` موزّعة حسب المكوّن |
| `integration` | 28 | تكامل C++ | `unit/` (مراجعة: بعضها e2e) |
| `unit` | 19 | وحدة فعلية | `unit/` مباشرة |
| `parser_tests` | 17 | نحوي (معطّل حالياً) | `unit/parser` (يوثَّق المعطّل) |
| `sadinfo` | 11 (+3py) | أداة | `system/sadinfo` |
| `interpreter_tests` | 10 | مفسر | `unit/interpreter` |
| `network` | 10 | شبكة C++ | `system/` خلف علم |
| `stdlib_tests` | 7 | مكتبة | `unit/` + `behavior/09` |
| `data_tests` | 6 | بيانات | `unit/types` |
| `lexer_tests` | 3 | معجمي | `unit/lexer` |
| `jit` | 3 | JIT | `unit/sir` أو `system/` |
| `backends`,`framework`,`low_level` | 2 لكلٍّ | بنية تحتية | `unit/` + `tests/framework` يبقى |
| `parser`,`oop_tests`,`pkg`,`tools`,`benchmark`,`lsp` | 1 لكلٍّ | شظايا | الطبقة المناسبة |

### الطبقة 3 — نظام وأدوات → `system/`

| المجلد القديم | المحتوى | الوجهة |
|---|---|---|
| `performance` | 5 ص + 5 py | `system/benchmark` |
| `benchmark`/`benchmarks` | قياسات | `system/benchmark` (دمج) |
| `fuzzing` | (فارغ حالياً) | `system/fuzzing` |
| `doc_gen_dual_execution` | 21 ص + py | `system/docs` |
| `builtin_errors` | 2 py | `system/` أو `unit/errors` |
| `lsp`,`sadinfo`,`pkg` | أدوات | `system/<أداة>` |

### مجلدات فارغة (للأرشفة/الحذف بعد تأكيد)

`async`, `compat`, `oop`, `spec_rules`, `stage1`, `benchmarks`, `fuzzing` — صفر ملفات.
تُؤرشف في `_archive/` مع توثيق (GR-04) ما لم تكن placeholders مقصودة.

## 2. التكرار المُكتشَف (مرشّحات الدمج)

| المجموعة المكرّرة | القرار |
|---|---|
| `parser` + `parser_tests` | دمج → `unit/parser` |
| `oop` + `oop_tests` | دمج → `unit/` (oop فارغ) |
| `stdlib` + `stdlib_tests` | فصل: .ص→behavior، .cpp→unit |
| `benchmark` + `benchmarks` + `performance` + `stress` | دمج → `system/benchmark` |
| `sad_tests` + `sad_examples` + `quick` | دمج/أرشفة → behavior |
| `docs_extraction` + `doc_gen_dual_execution` + `توثيق` | دمج → `system/docs` |

## 3. الأقسام الـ12 في dual_execution/features (AC-02 — الأساس)

| القسم | عدد .ص حالياً | ملاحظة تغطية أولية |
|---|---|---|
| 01_أساسيات_اللغة | 86 | جيد |
| 02_الأنواع_المدمجة | 16 | **فجوة محتملة** (أنواع كثيرة) |
| 03_البرمجة_الكائنية | 34 | متوسط |
| 04_مطابقة_الأنماط | 86 | جيد |
| 05_معالجة_الأخطاء | 25 | متوسط |
| 06_الدوال_المتقدمة | 12 | **فجوة** (لامدا/إغلاقات/أجّل) |
| 07_التزامن | 23 | يحتاج تطبيع (ADR-004) |
| 08_ميزات_متقدمة | 19 | **فجوة** (ماكرو/توجيهات/عقود/أعمار) |
| 09_المكتبة_القياسية | 121 | جيد |
| 10_الاستيراد | 2 | **فجوة حرجة** (+11 في `modules` تُدمج) |
| 11_سلبي | 20 | منخفض — يحتاج سلبي لكل قسم |
| 12_تكامل | 8 | منخفض |

> تفصيل ميزة↔ملف لكل قسم في مسودات `COVERAGE.md` (مجلد `coverage-drafts/`).

## 4. الأساس المرجعي (AC-03) — مقيس فعلياً

- **P0 (5 ملفات): 100%** (6.8s).
- **full (837 ملف): 83.2%** — نجح 696، فشل **141**، زمن 327s (`--cpu max`).
  - تكافؤ مزدوج (مفسر+مترجم): 641
  - مفسر فقط (`@expected`): 55

### تصنيف الإخفاقات الـ141 (نقطة الانطلاق الحقيقية)

| النوع | العدد | المعنى |
|---|---|---|
| `FAIL_COMPILE` | 59 | المترجم `sadc` يفشل في الترجمة — فجوات backend (أبرزها websocket/شبكة) |
| `FAIL_OUTPUT` | 52 | خرج المفسر ≠ المترجم — **بعضها رفرفة لا حتمية** (يؤكد R1) |
| `FAIL_INTERP` | 30 | المفسر نفسه يفشل |

### الإخفاقات حسب القسم (أعلى الأقسام)

| القسم/المجلد | إخفاقات | ملاحظة |
|---|---|---|
| 09_المكتبة_القياسية + stdlib | **38 + 28** | **الأعلى** — أغلبها شبكة/http/websocket (بيئة + backend) |
| advanced | 11 | ميزات متقدمة |
| 01_أساسيات_اللغة | 11 | مفاجئ لقسم ناضج — يحتاج فحص |
| 03_البرمجة_الكائنية + oop | 7 + 7 | وراثة/سمات |
| **07_التزامن + concurrency** | **6 + 3** | **يؤكد R1** — `058_channels_buffered`, `036_exception_concurrency_cross` بنوع FAIL_OUTPUT (رفرفة) |
| 11_سلبي + negative | 5 + 5 | سلبي |

### دليل يؤكد ADR-004 (GR-01)

- **R1 (التزامن، درجة 9):** `058_channels_buffered.ص` و`036_exception_concurrency_cross.ص`
  يفشلان بـ `FAIL_OUTPUT` (تباين خرج) — وهذا بالضبط نمط الرفرفة اللاحتمية الذي يعالجه
  وضع التطبيع في TEST-007. **مخاطرة مثبتة ببيانات، لا تخمين.**
- **تصحيح لازم لمسودة RISK:** `09_المكتبة` صُنّفت أولياً «صيانة P2» لكنها **أعلى
  الأقسام فشلاً** → تُرفع درجتها (انظر RISK_DRAFT المُصحَّح).
