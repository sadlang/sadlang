# نظام الأخطاء والتشخيص في لغة ص

نظام الأخطاء **مدفوع بالبيانات**: التعريفات في YAML، ويُولَّد منها كتالوج C++. كود المعالجة
والعرض في `shared/errors/`.

> ✅ **المصدر النشط (V5، منذ EM-V5-1/EM-V5-3):** `language-truth/errors/*.yaml` هو المصدر الموحَّد،
> ومربوط في `cmake/codegen.cmake` (هدف `sad_error_messages_codegen`). **`data/language/error_messages.yaml`
> (V4) حُذف** (EM-V5-3). لتغيير رسالة خطأ: عدّل `language-truth/errors/<category>.yaml` ثم أعد البناء.
>
> ⚠️ **بقية الهجرة:** رسائل وقت التشغيل ما زالت من `error_codes.cpp` اليدوي — تجميع المُولَّد
> (جعله المصدر الحي) في نطاق `EM-3`.

## 1. البنية

```
language-truth/errors/*.yaml        ← المصدر الموحَّد النشط (V5، 7 فئات، مربوط في CMake)
        │  scripts/codegen/gen_error_messages.py --yaml-dir
        ▼
shared/errors/generated/            ← مُولَّد وقت البناء: error_messages_generated.{h,cpp}
shared/errors/include/error_codes.h ← تعداد ErrorCode (مصدر يدوي يُحدَّث مع الأخطاء)
        │
        ▼
shared/errors/  (كود المعالجة والعرض — يُحرَّر يدوياً)
```

### ملفات `shared/errors/` المفتاحية

| الملف | الدور |
|------|------|
| `include/error_codes.h` | تعداد `enum class ErrorCode` + `getErrorCodeString()` + `Language` |
| `include/error_catalog.h` | `ErrorCatalog`, `ErrorTemplate`, `RenderContext`, `Rendered` — قوالب العرض |
| `include/diagnostic.h` | بنية التشخيص الواحد (الموقع، الشدّة، الرسالة) |
| `include/error_manager.h` | جمع التشخيصات وإدارتها |
| `include/formatter.h` | تنسيق الإخراج للمستخدم |
| `include/fix_suggestions.h`, `suggestions.h`, `error_hints.h` | تلميحات الإصلاح |
| `include/smart_errors.h`, `cascade_prevention.h` | منع الأخطاء المتتالية (cascade) |
| `include/teacher_mode.h`, `explanation_level.h` | مستويات الشرح (مبتدئ/خبير) |
| `src/catalog/runtime_*_catalog.cpp` | كتالوجات أخطاء وقت التشغيل مقسّمة بالمجال |

## 2. تصنيفات الأخطاء (البادئات)

| البادئة | الملف YAML | المدى | المجال |
|---------|-----------|------|--------|
| `LEX_*` | `lexical.yaml` | LEX001+ | معجمي |
| `SYN_*` | `syntactic.yaml` | SYN001+ | نحوي |
| `SEM_*` | `semantic.yaml` | SEM001+ | دلالي |
| `RUN_*` | `runtime.yaml` | RUN001+ | وقت التشغيل |
| `OWN_*` | `ownership.yaml` | OWN001+ | الملكية |
| `IMP_*` | `import.yaml` | IMP001+ | الاستيراد |
| `IO_*` | `io.yaml` | IO001+ | إدخال/إخراج |
| `INT_*` | `internal.yaml` | INT001+ | داخلي |

## 3. بنية تعريف خطأ في YAML

> ✅ **الصيغة المُعتمَدة رسمياً (ADR-DOCS-V4-005): المتداخلة** أدناه (`title/brief/fix_hint/detailed`
> لكلٍّ `{ar,en}`). `language-truth/_schemas/error.schema.json` يصفها (الملفات الـ7 تجتازه بصفر
> مخالفات). **`data/language/error_messages.yaml` (V4) حُذف** (EM-V5-3) — المصدر الآن `language-truth/errors/`.
> كتالوج `sadinfo` (`E####`) صار **إسقاطاً مُولَّداً** (EM-V5-2).
>
> **القاعدة العملية:** انسخ بنية **خطأ شقيق في نفس الملف**.
>
> 📖 **الشرح الكامل للبنى + الهجرة + القرار المعتمَد:** [./error-yaml-structures.md](./error-yaml-structures.md).

**البنية المطابقة للملفات الفعلية** (كل ملف يبدأ بـ `version/category/description_ar/en/errors:`
ثم قائمة الأخطاء):
```yaml
- code: LEX_INVALID_CHARACTER      # اسم C++ (يصبح ErrorCode::LEX_INVALID_CHARACTER)
  id: LEX001                        # معرّف نصي ثابت (للوثائق والإحالة)
  category: lexical
  title:
    ar: رمز غير صالح
    en: Invalid character
  brief:                            # الرسالة المختصرة (تدعم placeholders)
    ar: رمز غير صالح '{char}' في المصدر
    en: Invalid character '{char}' in source
  fix_hint:                         # تلميح الإصلاح (الأهم لتجربة المستخدم)
    ar: احذف الرمز '{char}' أو استبدله برمز صالح...
    en: Remove '{char}' or replace it...
  placeholders:                     # المتغيرات المسموحة في القوالب
  - char
  detailed:                         # شرح تفصيلي (لوضع المعلّم/المستوى المتقدم)
    ar: المحلّل المعجمي قرأ رمزاً لا ينتمي إلى أبجدية لغة ص...
    en: The lexer encountered a character outside the language alphabet...
```
> **انتبه:** `code` هنا اسم رمزي (`LEX_INVALID_CHARACTER`) لا يطابق نمط `CAT_NNNN` الذي يطلبه
> schema الخاص بـ V5 — دليل إضافي على عدم اكتمال الهجرة. اتبع الملفات الفعلية.

## 4. إضافة رمز خطأ جديد (الإجراء الصحيح)

> **يُمنع** تحرير الملفات تحت `shared/errors/generated/` يدوياً. ابدأ من YAML.

1. **عرّف الرمز** في تعداد `enum class ErrorCode` في `shared/errors/include/error_codes.h`
   (مصدر يدوي) بالاسم `<PREFIX>_<NAME>` ضمن المدى الصحيح، مع تعليق `id`.
2. **أضِف الرسالة** في `language-truth/errors/<category>.yaml` (المصدر الموحَّد) بالصيغة المتداخلة
   (`code/id/category/title/brief/fix_hint/detailed` بـ `ar`+`en`، و`placeholders` اختياري) —
   انسخ بنية خطأ شقيق في نفس الملف.
3. **اختر `id` تسلسلياً** (آخر رقم + 1 — لا تُعِد استخدام رقم محذوف؛ التوافق الخلفي CW-24).
4. **أعد التوليد** — أعد بناء CMake (هدف `sad_error_messages_codegen` يُعاد آلياً)، أو يدوياً:
   ```powershell
   python scripts/codegen/gen_error_messages.py `
       --yaml-dir    language-truth/errors `
       --schema      language-truth/_schemas/error.schema.json `
       --enum-header shared/errors/include/error_codes.h `
       --header      shared/errors/generated/error_messages_generated.h `
       --source      shared/errors/generated/error_messages_generated.cpp
   ```
   > المولّد يفرض: كل `code` موجود في enum، تطابق id↔prefix، تفرّد، تغطية كاملة (6 فحوص دلالية).
5. **أطلِق الخطأ بالمسار الموحَّد** (BF-10): `Sad::Errors::throwRuntime(ErrorCode::<NAME>, pos, {{...}})`
   (راجع `runtime_throw.h` — لا نص حر؛ راجع [./builtins-system.md](./builtins-system.md) للنمط).
6. **اكتب اختباراً** يعيد إنتاج الحالة التي تُطلِق الخطأ (`tests/unit/errors/` أو `tests/dual_execution/errors/`).

## 5. مبادئ تصميم رسالة خطأ جيدة

- **`brief` يصف ما حدث، `fix_hint` يقول كيف يُصلَح** — الأخير هو الأثمن للمستخدم.
- **مزدوج اللغة دائماً** (`ar` + `en`) — حقل ناقص = فشل التحقق من Schema.
- **placeholders فقط ما هو مُعرَّف** — أي `{x}` غير مُدرج في `placeholders` يكسر العرض.
- **`detailed` يشرح السبب الجذري** لا يكرر `brief` — يظهر في وضع المعلّم.
- **منع التتالي:** خطأ واحد جذري أفضل من 10 أعراض. راجع `cascade_prevention.h` قبل إطلاق سلسلة.

## 6. التحقق

```powershell
# تحقق من مطابقة كل YAML للـ Schema
python scripts/validate_schemas.py            # يقبل --strict / --verbose فقط (لا --truth-dir)

# اختبارات المولّد نفسه
python scripts/codegen/test_gen_error_messages.py
```
