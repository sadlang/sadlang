# نظام التوثيق و YAML — مصدر الحقيقة (Single Source of Truth)

`language-truth/` هو المصدر الموحَّد لكل بيانات لغة ص. كل الأدوات (المفسر، المترجم، LSP،
المنسّق، مدير الحزم، الموقع، التوثيق) تستهلك منه — مباشرةً وقت التطوير، أو عبر كود C++ مُولَّد.

## 1. بنية `language-truth/`

```
language-truth/
├── README.md, VERSIONING.md   ← نقطة البدء + سياسة SemVer للبيانات (الإصدار 5.0.0)
├── _schemas/                  ← JSON Schemas (Draft 2020-12) — تتحقق من كل YAML
├── _meta/                     ← فهرس آلي + معلومات الإصدار
│
├── keywords.yaml              ← الكلمات (40 محجوزة + سياقية + أنواع)
├── operators.yaml             ← العوامل + الأسبقية/الترابط
├── type_methods.yaml          ← طرق الأنواع المدمجة
├── directives.yaml            ← التوجيهات (@)
├── types.yaml                 ← الأنواع المدمجة (9)
├── patterns.yaml              ← أنماط المطابقة
├── grammar_constructs.yaml    ← عقود/تزامن/ماكرو/امتداد/عمر/async/ffi
├── oop_constructs.yaml        ← أصناف/بنى/تعداد/سمات/وراثة/خصائص/عوامل/قوالب
├── expr_constructs.yaml       ← استيعاب/أنابيب/lambda/closure/f-string/tuple
│
├── builtins/  *.yaml          ← الدوال المدمجة (مقسّمة بالمجال) + _index.yaml
├── errors/    *.yaml          ← رسائل الأخطاء (مقسّمة بالتصنيف)
├── stdlib/    modules.yaml, functions.yaml
└── learning/  lessons.yaml, exercises.yaml
```

## 2. خط أنابيب التوليد (Codegen)

```
language-truth/*.yaml  →  scripts/codegen/gen_*.py  →  shared/*/generated/*.{h,cpp}
     (المصدر)                  (مولّدات Python)              (مُولَّد — لا يُحرَّر)
```

أدوات التوليد الرئيسية في `scripts/codegen/`:

| المولّد | المصدر | الإخراج |
|---------|--------|--------|
| `gen_keywords.py` | `keywords.yaml` | `shared/lexer/generated/keywords_generated.{h,cpp}` |
| `gen_builtins_registry.py` | `builtins/*.yaml` | `shared/builtins/generated/builtin_registry_generated.h` |
| `gen_error_messages.py` | `language-truth/errors/*.yaml` (V5 — `--yaml-dir`، مربوط في CMake منذ EM-V5-1) | `shared/errors/generated/error_messages_generated.{h,cpp}` |
| `gen_types.py` | `types.yaml` | كود الأنواع المُولَّد |
| `gen_parser_grammar_docs.py` | `grammar/*.yaml` | `docs/parser_rule/_generated` |
| `gen_tmgrammar.py` | `language-truth/` | قواعد TextMate لإبراز المحرّرات |

> مولّدات النطاقات الأخرى (operators/directives/type_methods/grammar/modules/stdlib/…)
> **غير موجودة بعد** — سقالاتها معطَّلة في `cmake/codegen.cmake` (انظر أدناه).
> المُنسِّق الشامل هو `python x.py gen` (مع حارس انجراف `x.py gen --check`).

**التوليد التلقائي وقت البناء (EM-V5-4):** `cmake/codegen.cmake` يربط **4 نطاقات عاملة** بهدفٍ
تجميعيّ `sad_all_codegen`: types، keywords، builtins، error_messages. أي تعديل YAML في نطاق
عامل يُعاد توليده **آلياً** وقت البناء. النطاقات التسعة الباقية (operators، directives،
grammar، modules، stdlib، type_methods، patterns، oop/expr_constructs) **سقالات معطَّلة**
(مولّداتها مفقودة/غير مستهلَكة — انظر التعليق فوق `sad_add_codegen` في `codegen.cmake`).
للتشغيل الشامل خارج البناء استخدم `python x.py gen` (لا يوجد `gen_all.py`).

> **تنبيه دقّة:** توليد الأخطاء يقرأ V5، لكن **C++ لا يُجمّع المُولَّد بعد** (رسائل وقت التشغيل ما زالت
> من `error_codes.cpp` اليدوي) — التكامل في نطاق `EM-3`.

### متطلبات بيئة Python

```powershell
# Python 3.9+ مطلوب
pip install pyyaml jsonschema
```

## 3. التحقق من الـ Schema

> ⚠️ **المخططات الآن في مكان واحد:** `language-truth/_schemas/` (حُذف `data/_schemas/` مع
> تقاعد sadinfo — 2026-07-03). وقد **لا تتطابق** كل الملفات الفعلية مع schema مجلدها
> (راجع [./error-system.md](./error-system.md) §3). لا تفترض أن كل YAML يجتاز الـ schema؛
> **اعتمد ملفاً شقيقاً في نفس المجلد كمرجع بنية**.

> ⚠️ **لا توجد أداة تحقّق مستقلّة في المستودع** (`scripts/validate_schemas.py` غير موجود).
> التحقق الفعلي يجري داخل المولّدات نفسها (`gen_keywords.py`/`gen_types.py`/`gen_error_messages.py`
> تتحقق بـ jsonschema قبل التوليد) وفي اختباراتها (`scripts/codegen/test_*.py`).

## 4. أداة `sadinfo` — متقاعدة (2026-07-03)

كانت أداة CLI للاستعلام عن بيانات اللغة. **تقاعدت**: نصف مصادرها لم يدخل git قطّ،
ومخطط كتالوجها مفقود، وهدفها لم يكن مربوطاً في CMake. حُذفت مع `data/` كتالوجِها.
إن لزمت لاحقاً تُعاد كتابتها من `language-truth/` مباشرة (SoT).

## 5. سياسة الإصدار (VERSIONING.md)

البيانات تتبع SemVer (الإصدار الحالي `5.0.0`):
- **MAJOR**: كسر توافق (حذف كلمة/دالة/رمز خطأ، تغيير معنى).
- **MINOR**: إضافة متوافقة (كلمة/دالة/خطأ جديد).
- **PATCH**: تصحيح رسالة/وصف بلا تغيير سلوكي.

> `status: deprecated` بدلاً من الحذف المباشر — حافظ على التوافق الخلفي (CW-24).

## 6. الإجراء عند تعديل أي بيانات لغة

1. عدّل ملف YAML المصدر في `language-truth/` (لا تلمس `generated/`).
2. تحقق من البنية: قارن بملف شقيق في نفس المجلد (المولّد `gen_*.py` يتحقق من المخطط عند التوليد).
3. أعد التوليد (CMake أو `gen_*.py` يدوياً، أو `python x.py gen`).
4. ابنِ واختبر المفسر والمترجم.
5. حدّث رقم الإصدار في `VERSIONING.md`/`_meta/` حسب نوع التغيير.

## 7. تنبيهات

- `language-truth/` هو المصدر الموحَّد الوحيد (ADR-DOCS-V4-005). بقايا `data/language/` زالت
  كلّها: أخطاء V4 حُذفت (EM-V5-3)، و`keywords.yaml` القديم حُذف بإغلاق M2-004 (2026-07-03).
- ملفات `_bmad-output/` تخضع لحوكمة منفصلة — اقرأ سياستها قبل لمسها (راجع `copilot-instructions.md`).
