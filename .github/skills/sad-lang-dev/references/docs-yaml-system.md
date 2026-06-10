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
| `gen_operators.py` | `operators.yaml` | كود العوامل المُولَّد |
| `gen_directives.py` | `directives.yaml` | كود التوجيهات |
| `gen_types.py`, `gen_type_methods.py` | `types.yaml`, `type_methods.yaml` | الأنواع والطرق |
| `gen_grammar.py`, `gen_constructs.py` | `*_constructs.yaml` | بنى القواعد |
| `gen_stdlib.py`, `gen_modules.py` | `stdlib/*.yaml` | المكتبة القياسية |
| `gen_docs.py` | متعدد | وثائق |
| `gen_all.py` | الكل | يشغّل كل المولّدات |

**التوليد التلقائي وقت البناء — ✅ كل النطاقات مربوطة (EM-V5-4):** `cmake/codegen.cmake` يربط
**13 هدفاً** عبر دالة موحَّدة `sad_add_codegen` + هدف تجميعي `sad_all_codegen`: keywords، builtins،
errors، sadinfo (إسقاط)، operators، directives، types، type_methods، grammar، modules، stdlib،
patterns، oop/expr_constructs. أي تعديل YAML في أي نطاق يُعاد توليده **آلياً** وقت البناء.
لا حاجة لتشغيل `gen_all.py` يدوياً بعد الآن (يبقى أداةً للتشغيل الشامل خارج البناء).

> **تنبيه دقّة:** توليد الأخطاء يقرأ V5، لكن **C++ لا يُجمّع المُولَّد بعد** (رسائل وقت التشغيل ما زالت
> من `error_codes.cpp` اليدوي) — التكامل في نطاق `EM-3`.

### متطلبات بيئة Python

```powershell
# Python 3.9+ مطلوب
pip install pyyaml jsonschema
```

## 3. التحقق من الـ Schema

> ⚠️ **حالة الـ Schemas غير موحَّدة (V4→V5):** يوجد مجلدا schemas: `data/_schemas/` (يستخدمه
> `validate_schemas.py` فعلياً) و`language-truth/_schemas/` (V5). وقد **لا تتطابق** الملفات الفعلية
> مع schema مجلدها (خاصةً الأخطاء — راجع [./error-system.md](./error-system.md) §3). لا تفترض
> أن كل YAML يجتاز schema حتى تكتمل الهجرة؛ **اعتمد ملفاً شقيقاً في نفس المجلد كمرجع بنية**.

الأداة النشطة تتحقق من `data/_schemas/` مقابل fixtures الخاصة بـ sadinfo:

```powershell
python scripts/validate_schemas.py            # يقبل --strict / --verbose فقط (لا --truth-dir)
```

> فشل التحقق = حقل ناقص أو نوع خاطئ في الـ fixtures المعنيّة. تحقّق من البنية الفعلية بـ Read.

## 4. أداة `sadinfo` — الاستعلام عن بيانات اللغة

`tools/sadinfo/` أداة CLI تقرأ بيانات اللغة المُولَّدة/الموثّقة وتسمح بالاستعلام عنها
(الكلمات، الدوال، الأخطاء، الأنواع). تُستخدم في الاختبارات (`tests/sadinfo/fixtures/`)
وللتحقق من اتساق البيانات. راجع `tools/sadinfo/README.md`.

## 5. سياسة الإصدار (VERSIONING.md)

البيانات تتبع SemVer (الإصدار الحالي `5.0.0`):
- **MAJOR**: كسر توافق (حذف كلمة/دالة/رمز خطأ، تغيير معنى).
- **MINOR**: إضافة متوافقة (كلمة/دالة/خطأ جديد).
- **PATCH**: تصحيح رسالة/وصف بلا تغيير سلوكي.

> `status: deprecated` بدلاً من الحذف المباشر — حافظ على التوافق الخلفي (CW-24).

## 6. الإجراء عند تعديل أي بيانات لغة

1. عدّل ملف YAML المصدر في `language-truth/` (لا تلمس `generated/`).
2. تحقق من البنية: قارن بملف شقيق في نفس المجلد + `python scripts/validate_schemas.py` (يتحقق من fixtures sadinfo).
3. أعد التوليد (CMake أو `gen_*.py` يدوياً، أو `gen_all.py`).
4. ابنِ واختبر المفسر والمترجم.
5. حدّث رقم الإصدار في `VERSIONING.md`/`_meta/` حسب نوع التغيير.

## 7. تنبيهات

- `language-truth/` هو المصدر الموحَّد المعتمَد (ADR-DOCS-V4-005). بقايا `data/language/` تُتقاعَد
  تدريجياً: أخطاء V4 حُذفت (EM-V5-3)؛ `keywords.yaml` يبقى مؤقتاً حتى M2-004.
- ملفات `_bmad-output/` تخضع لحوكمة منفصلة — اقرأ سياستها قبل لمسها (راجع `copilot-instructions.md`).
