---
status: ACTIVE
lastUpdated: 2026-06-10
completionPercentage: 100
scope: "هجرة V5 (V4 data/language → language-truth/ موحَّد) — لا V4 الأصلية"
---

# حالة التنفيذ — نظام Error Messages (هجرة V5)

> **GR-01:** كل ادّعاء أدناه موثَّق بدليل من الكود/القرص. حالة هجرة V5 **في بدايتها** —
> أُنجز التصميم والقرارات + خطوة schema واحدة فقط.

## 1. القرارات (محسومة)

| القرار | الحالة | الدليل |
|--------|--------|--------|
| المصدر الموحَّد `language-truth/` | ✅ محسوم | [ADR-DOCS-V4-005](../../living-documentation/decisions/ADR-DOCS-V4-005-UNIFIED-SOT-MIGRATION-COMPLETION.md) (Accepted) |
| ق-فرعي-1: صيغة الرسائل **المتداخلة** | ✅ محسوم | ADR-005 §القرارات الفرعية |
| ق-فرعي-2: sadinfo **إسقاط مُولَّد** | ✅ محسوم | ADR-005 §القرارات الفرعية |
| ق-فرعي-3: ربط **كل النطاقات** في CMake | ✅ محسوم | ADR-005 §القرارات الفرعية |

## 2. ستوريات هجرة V5

| الستوري | المرحلة | الحالة | الدليل |
|---------|---------|--------|--------|
| [EM-V5-1](../stories/EM-V5-1-rewire-to-language-truth.md) | أ — توليد من language-truth + CMake | T0–T2 ✅ / T3 بديل ✅ / T4 → EM-3 | المولّد V5 + CMake (exit 0)، 75 رسالة |
| [EM-V5-2](../stories/EM-V5-2-sadinfo-generated-projection.md) | ب — sadinfo إسقاط مُولَّد | T1–T3 ✅ / T4 (sadinfo --errors) | 75 كياناً، CMake، 6 اختبارات تمرّ |
| [EM-V5-3](../stories/EM-V5-3-retire-v4-data-language.md) | ج — حذف data/language V4 | ✅ مكتملة | V4 محذوف، 7 ملفات مهارة + ADR-EM-3 Superseded، 14 اختبار أخضر |
| [EM-V5-4](../stories/EM-V5-4-wire-all-domains-cmake.md) | ربط كل النطاقات في CMake | ✅ مكتملة | دالة sad_add_codegen + 10 نطاقات + sad_all_codegen (13 هدف، build exit 0) |

## 3. الإنجاز الموثَّق (بدليل)

### ✅ EM-V5-1 / T0 — تصحيح schema الأخطاء (2026-06-09)
- `language-truth/_schemas/error.schema.json` يصف الصيغة المتداخلة المُعتمَدة.
- **الدليل:** الملفات الثمانية (231 خطأً) تجتاز الـ schema بصفر مخالفات (`jsonschema Draft202012Validator`).

## 4. مصفوفة حالة توليد النطاقات (دليل: cmake/codegen.cmake + shared/*/generated/)

| مربوط آلياً في CMake | baseline يدوي (gen_all.py) | يقرأ V4 بعد |
|---|---|---|
| keywords، builtins | operators, directives, types, type_methods, patterns, grammar/oop/expr_constructs, modules, stdlib (≥10) | errors |

**الهدف (ق-فرعي-3):** نقل كل عمود «baseline يدوي» + «errors» إلى عمود «مربوط آلياً» عبر `EM-V5-4` و`EM-V5-1`.

## 5. ✅ حاجز BLK-1 — مُحَلّ (2026-06-10)

كان: `internal.yaml` فيه 156 رمز `INT_*` يتيماً (enum=75، YAML=231). **الحل:** المالك حذف
`internal.yaml` → V5 = 75 = enum تماماً (orphan=0). EM-V5-1 لم تعد محجوبة.

## 6. الإنجاز الموثَّق الإضافي (EM-V5-1، 2026-06-10)

- **T1 ✅** المولّد يقرأ `language-truth/errors/` عبر `--yaml-dir` (75 رسالة، exit 0، 6 فحوص دلالية).
- **T2 ✅** `sad_error_messages_codegen` مربوط في CMake (configure + build exit 0).
- **الملفات:** `gen_error_messages.py`, `cmake/codegen.cmake`, `error.schema.json`,
  `shared/errors/generated/error_messages_generated.{h,cpp}`.

## 7. ✅ EM-3 (تكامل C++) — مكتمل ومُتحقَّق سلوكياً (2026-06-10)

- ✅ أُضيف `error_messages_generated.cpp` للبناء؛ حُذف `error_codes.cpp` + `error_catalog_init.cpp`
  (مستبدَلان بالكامل — الكتالوج المُولَّد superset، دليل: `RUN_FILE_ERROR` يتطابق + 52 RUN ⊇ 49).
- ✅ `sad_shared` يُربط بلا تكرار/فقد رموز؛ `sad-run.exe` يُبنى (exit 0).
- ✅ **تحقّق سلوكي:** خطأ وقت تشغيل يُعرَض `[SEM001]` + «متغير غير معرّف» **من الكتالوج المُولَّد**.
- 🔧 إصلاح عطل سابق غير متعلّق (لفكّ البناء): `TM::Array::COUNT_ALT`→`COUNT` (لقب لم يُولَّد قط).

## 8. ✅ تنظيف الكود الميت — مُنجَز ومُتحقَّق (2026-06-10)

- ✅ حُذفت **10 ملفات** ميتة: 9 × `runtime_*_catalog.cpp` + `runtime_template_helpers.h`
  (مجلد `errors/src/catalog/` فارغ الآن) + أُزيلت سطورها من `shared/CMakeLists.txt`.
- ✅ **دليل عدم التراجع:** `sad_shared` + `sad-run` يُبنيان (exit 0)؛ خطأ runtime حقيقي يُعرَض
  **`[RUN002]`** + «الفهرس 10 خارج نطاق ... (الطول 3)» — قالب كان في الكتالوج المحذوف، الآن من المُولَّد.

## 9. تصحيح: «فقدان code_example» كان إيجابية كاذبة (2026-06-10)

تحليل أدقّ (تقسيم على حدود `registerTemplate`): **أقصى 9 وسائط نصية** في أي قالب يدوي، و**0
قالب** يمرّر `codeExample` (الوسيط العاشر دائماً افتراضي ""). وكتالوجات runtime لا تضعه أيضاً.
**إذن لا فقدان محتوى — تكافؤ كامل 100%.** (التقدير السابق «≤2» كان خطأ regex.)

## ✅ الخلاصة (الكتالوج): نظام error-messages مكتمل — مصدر موحَّد من YAML إلى وقت التشغيل، تكافؤ كامل، صفر كود ميت.

## 10. إبيك جديد: ترحيل مواقع رمي C++ (EPIC-EM-CPP)

الكتالوج مُولَّد 100%، لكن **~310 موقع رمي خام** في C++ (254 منها في الدوال المضمنة) ما زالت
تكتب نصوصاً بدل الإشارة لرموز `ErrorCode`. أُنشئ [EPIC-EM-CPP](../epics/EPIC-EM-CPP-MIGRATION.md)
لإكمال التوحيد عبر **`BuiltinContext`** (عقد استدعاء جديد — [ADR-EM-CPP-1](../decisions/ADR-EM-CPP-1-BUILTIN-CONTEXT.md)):
`EM-CPP-0` (تقديم BuiltinContext) → بايلوت math (`EM-CPP-1`) → توسعة (maps/assertions/kernel/...) →
كنس غير المضمّنة. **التوثيق المرجعي:** `docs/ERROR_SYSTEM_GUIDE.md`.
- EM-V5-4: ربط ≥10 نطاقات في CMake.
- EM-V5-2: مولّد إسقاط sadinfo.
- EM-V5-3: حذف V4 (بعد أ+ب).
