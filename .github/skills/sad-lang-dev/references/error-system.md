# نظام الأخطاء والتشخيص في لغة ص

نظام الأخطاء **مدفوع بالبيانات**: التعريفات في YAML، ويُولَّد منها كتالوج C++. كود المعالجة
والعرض في `shared/errors/`.

> ✅ **المصدر الحيّ الموحَّد (EM-3 + EM-CPP، مكتمل):** `language-truth/errors/*.yaml` (8 فئات، 84 رمزاً)
> هو المصدر الوحيد، ومربوط في `cmake/codegen.cmake` (هدف `sad_error_messages_codegen`). **المُولَّد
> صار المصدر الحيّ** (لا كتالوجات يدوية). لتغيير رسالة: عدّل `language-truth/errors/<category>.yaml` ثم أعد البناء.
>
> ✅ **الترحيل مكتمل (EM-CPP):** كل خطأ يراه المستخدم — في المفسر والمترجم، لغوياً أو داخلياً (ICE) —
> يأتي من الكتالوج. **لا نصّ رسالة مكتوب يدوياً في C++.** الدوال المضمنة 100% (ctx.error + BuiltinError)؛
> codegen المترجم 100% (رموز ICE). الاستثناء الوحيد: "Debugger disconnected" (تدفّق تحكّم لا رسالة).
>
> 🛡️ **حاجز render (الطبقة 1):** عند نقص placeholder، `substitute` يُفرّغه افتراضياً (لا يتسرّب
> `{key}` للمستخدم)؛ ضبط `SAD_DEBUG_PLACEHOLDERS` يُبقيه لكشف العلّة للمطوّر. **مع ذلك مرّر كل
> placeholder مطلوب** — الحاجز شبكة أمان لا رخصة إهمال.
>
> 📖 **المعمارية الكاملة بمخططات:** `_bmad-output/systems/error-messages/docs/ERROR_SYSTEM_GUIDE.md` ·
> **دليل ICE للمطوّرين:** `.../docs/ICE_DEVELOPER_GUIDE.md`.

## 1. البنية

```
language-truth/errors/*.yaml        ← المصدر الحيّ الموحَّد (8 فئات/84 رمزاً، مربوط في CMake)
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
| `generated/error_messages_generated.{h,cpp}` | **المُولَّد من YAML — المصدر الحيّ** (`kErrorMessages` + `registerDefaults`) |
| `include/runtime_throw.h` · `builtin_error.h` | `throwRuntime` (RuntimeAbort) · `BuiltinError`/`throwBuiltin` (حامل الطبقة الأدنى) |
| `src/error_catalog.cpp::substitute` | استبدال `{key}` — حاجز الطبقة 1 (يُفرّغ المفقود إلا في `SAD_DEBUG_PLACEHOLDERS`) |

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
> لكلٍّ `{ar,en}`). `language-truth/_schemas/error.schema.json` يصفها (الملفات الـ8 تجتازه بصفر
> مخالفات). **`data/language/error_messages.yaml` (V4) حُذف** (EM-V5-3) — المصدر الآن `language-truth/errors/`.
> كتالوج `sadinfo` (`E####`) كان إسقاطاً مُولَّداً (EM-V5-2)، ثم **تقاعدت أداة sadinfo وكتالوجها
> `data/` معاً (2026-07-03)** — 4 من 9 مصادرها لم تدخل git قطّ؛ تُعاد كتابتها من SoT إن لزمت.
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
5. **أطلِق الخطأ بالآلية الموحَّدة المناسبة لطبقتك** (BF-10، لا نص حر) — أربع نقاط دخول كلها للكتالوج:

   | أنت في... | الآلية | ملاحظة |
   |-----------|--------|--------|
   | تجميع (lexer/parser/semantic/**codegen**) | `EM.reportFromCatalog(code, location, ctx)` | له `SourceLocation` |
   | visitor/manager في المفسر | `throwRuntime(code, Position, {{...}})` | يرمي `RuntimeAbort` |
   | داخل lambda دالة مضمنة | **`ctx.error(code, {{...}})`** | يحقن `func/builtin` تلقائياً |
   | طبقة `shared/builtins` الأدنى (بلا Position) | **`throwBuiltin(code, {{...}})`** | حامل؛ يُكمل الموقع في `callNative` |
   | خطأ مترجم داخلي (ICE) في codegen | `cg_.reportError(INT_*, {{"detail", "<مُعرِّف>"}})` | راجع ICE_DEVELOPER_GUIDE |

   ⚠️ **مرّر كل placeholder يطلبه الـbrief** — وإلا (رغم حاجز الطبقة 1) يظهر فارغاً للمستخدم. لا
   تستخدم رمزاً يتطلّب placeholders إضافية (مثل `RUN_TYPE_CHECK_FAILED`: `{expected}`/`{actual}`)
   دون تمريرها؛ لفحص أرغ مضمنة استخدم `RUN_BUILTIN_REQUIRES_ARG` (يكفيه `{func}` المحقون).
6. **اكتب اختباراً** يعيد إنتاج الحالة (حارس وحدة في `scripts/codegen/test_gen_error_messages_v5.py`
   و/أو سلوكي end-to-end في `tests/builtin_errors/`).

## 5. مبادئ تصميم رسالة خطأ جيدة

- **`brief` يصف ما حدث، `fix_hint` يقول كيف يُصلَح** — الأخير هو الأثمن للمستخدم.
- **مزدوج اللغة دائماً** (`ar` + `en`) — حقل ناقص = فشل التحقق من Schema.
- **placeholders فقط ما هو مُعرَّف** — أي `{x}` غير مُدرج في `placeholders` يكسر العرض.
- **`detailed` يشرح السبب الجذري** لا يكرر `brief` — يظهر في وضع المعلّم.
- **منع التتالي:** خطأ واحد جذري أفضل من 10 أعراض. راجع `cascade_prevention.h` قبل إطلاق سلسلة.
- **`placeholders` بيانات لا نثر (EM-CPP):** مرّر مُعرِّفات/قيماً فقط؛ الجملة في YAML. لا تضع جملاً
  مكتوبة يدوياً في `{detail}` — يُعيد النصّ الخام للكود.
- **الأخطاء الداخلية (ICE) من الكتالوج:** خلل المترجم/المفسر «لا ينبغي أن يحدث» → فئة `internal`
  (`INT_*`)، موسومةً «خطأ مترجم/مفسر — أبلِغ» بشرح للمطوّر. لا تُسكِت ICE — أصلِح الطبقة القَبْلية.

## 6. التحقق

```powershell
# تحقق الـ Schema مدمج في المولّد (gen_error_messages.py يتحقق بـ jsonschema + 6 فحوص دلالية)
# لا توجد أداة scripts/validate_schemas.py مستقلّة في المستودع.

# اختبارات المولّد + حُرّاس الترحيل (الفعّال V5: منها حارس ICE وحارس placeholders)
python -m pytest scripts/codegen/test_gen_error_messages_v5.py -q

# اختبارات سلوكية end-to-end لأخطاء الدوال المضمنة (تتطلّب بناء sad-build)
python -m pytest tests/builtin_errors/ -q
```
