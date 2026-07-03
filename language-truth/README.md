# language-truth/ — مصدر الحقيقة لبيانات لغة ص

> **الحالة:** **SoT الوحيد** (أُغلق M2-004 في 2026-07-03: حُذف `data/language/`).
> **المرجع:** [ARCHITECTURE.md](../_bmad-output/systems/living-documentation/ARCHITECTURE.md)

---

## ما هو هذا المجلد؟

`language-truth/` هو المصدر الموحَّد (Single Source of Truth) لكل بيانات لغة ص:
الكلمات المفتاحية، الدوال المدمجة، الأنواع، العوامل، التوجيهات، الأخطاء، المكتبة القياسية.

هذا المجلد هو **SoT الوحيد** للغة. اكتملت هجرة V5: حُذف `data/language/` الانتقاليّ في 2026-07-03
(قرار ADR-DOCS-V4-003 / إغلاق S-V5-M2-004).

---

## البنية

```
language-truth/
├── README.md              ← هذا الملف — نقطة بدء لأي فريق أداة
├── VERSIONING.md          ← سياسة Semantic Versioning للبيانات
│
├── _schemas/              ← JSON Schemas (Draft 2020-12) للتحقق
├── _meta/                 ← فهرس آلي + معلومات الإصدار
│
├── keywords.yaml          ← الكلمات المفتاحية (40 محجوزة + 25 سياقية + 9 أنواع)
├── operators.yaml         ← العوامل (~40 + أسبقية/ترابط)
├── type_methods.yaml      ← طرق الأنواع المدمجة (80 طريقة، 7 أهداف)
├── directives.yaml        ← التوجيهات (~7)
├── types.yaml             ← الأنواع المدمجة (9)
├── patterns.yaml          ← أنماط المطابقة (~8)
├── grammar_constructs.yaml ← عقود/تزامن/ماكرو/امتداد/عمر/async/ffi
├── oop_constructs.yaml    ← أصناف/بنى/تعداد/سمات/وراثة/خصائص/عوامل/قوالب
├── expr_constructs.yaml   ← استيعاب/أنابيب/lambda/closure/f-string/tuple
│
├── builtins/              ← الدوال المدمجة (مقسَّمة حسب المجال)
│   ├── core.yaml          ← دوال أساسية (اطبع، طول، نوع، ...)
│   ├── concurrency.yaml   ← قناة، مجموعة_انتظار، قفل، ...
│   ├── types.yaml         ← مُنشئو الأنواع (رقم، نص، عشري، ...)
│   └── _index.yaml        ← فهرس آلي
│
├── errors/                ← رسائل الأخطاء مقسَّمة حسب التصنيف
│   ├── lexical.yaml       ← LEX_* (~30 خطأ)
│   ├── syntactic.yaml     ← SYN_* (~50 خطأ)
│   ├── semantic.yaml      ← SEM_* (~40 خطأ)
│   ├── runtime.yaml       ← RUN_* (~30 خطأ)
│   ├── ownership.yaml     ← OWN_* (~20 خطأ)
│   ├── import.yaml        ← IMP_* (~10 أخطاء)
│   ├── io.yaml            ← IO_* (~10 أخطاء)
│   └── internal.yaml      ← INT_* (~10 أخطاء)
│
├── stdlib/                ← المكتبة القياسية
│   ├── modules.yaml       ← ~10 وحدة
│   └── functions.yaml     ← ~119 دالة
│
└── learning/              ← محتوى تعليمي (دروس/تمارين/أمثلة)
    ├── lessons.yaml       ← دروس تربط بـ Truth
    └── exercises.yaml     ← تمارين + أمثلة
```

---

## كيف تستخدم هذا المجلد؟

### فريق LSP / Formatter / Website
- اقرأ الملفات YAML مباشرة — هي SoT وقت التطوير.
- في وقت التشغيل: استخدم `libsadlangtruth` (C++ wrapper) أو الملفات المُولَّدة في `shared/*/generated/`.

### فريق Codegen
- أدوات Python في `scripts/codegen/gen_*.py` تقرأ هذا المجلد وتُنتج C++ Generated.
- الـ Schemas في `_schemas/*.json` تتحقق من YAML أثناء CMake.

### التحقق اليدوي
```bash
python x.py gen --check   # حارس الانجراف: يتأكد أن generated/ مطابق لـ YAML الحالي
```
> تحقق الـ Schema نفسه مدمج في المولّدات (`gen_keywords.py`/`gen_types.py`/`gen_error_messages.py`
> تتحقق بـ jsonschema قبل التوليد) — لا توجد أداة `validate_schemas.py` مستقلّة.

---

## إصدار البيانات

راجع [VERSIONING.md](./VERSIONING.md) لسياسة SemVer الكاملة.
الإصدار الحالي: `5.0.0`

---

## English Summary

`language-truth/` is the Single Source of Truth for all Sad programming language data
(keywords, builtins, types, operators, errors, stdlib).

It is the sole SoT; the transitional `data/language/` copy was removed on 2026-07-03 (M2-004 closed).
All tooling (LSP, Website, Formatter, Package Manager) should consume from this directory
via Python codegen → `shared/*/generated/*.h` or via `libsadlangtruth` C++ API.
