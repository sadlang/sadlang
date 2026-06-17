# Story EM-V5-1 — تحويل توليد الأخطاء إلى language-truth/ (المرحلة أ)

**Epic**: EPIC-EM | **Story Points**: 8 | **الأولوية**: حرجة | **الحالة**: 🟡 قيد التنفيذ (T0–T2 ✅)
**التبعيات**: EM-4 | **المُسنَد إلى**: Amelia

---

## ✅ سجل المطوّر (Dev Agent Record) — Amelia، 2026-06-10

**ما نُفِّذ ومُتحقَّق:**
- **T0 ✅** — `language-truth/_schemas/error.schema.json` يصف الصيغة المتداخلة (دليل: الملفات تجتاز schema، 0 مخالفات).
- **حلّ BLK-1 ✅** — المالك حذف `internal.yaml` (156 رمز `INT_*` يتيماً). النتيجة: V5 = 75 رمزاً =
  `ErrorCode` enum تماماً (orphan=0, missing=0).
- **T1 ✅** — عُدِّل `gen_error_messages.py`: أُضيف `--yaml-dir` + `load_error_directory()` (دمج 7
  ملفات فئات + بناء `categories`/prefix بترتيب رسمي). **دليل:** `--yaml-dir language-truth/errors`
  → `🔁 75 رسالة`، exit 0، كل التحقّقات الدلالية الستة مرّت.
- **T2 ✅** — رُبط `sad_error_messages_codegen` في `cmake/codegen.cmake` (نمط builtins). **دليل:**
  `cmake -S . -B build` → "Error messages codegen configured (V5)" (exit 0)؛
  `cmake --build build --target sad_error_messages_codegen` → توليد ناجح (exit 0).

**قرار تحقّق T3/T4 (شفافية):** "golden مقابل V4" **ساقط** — لا baseline V4 مُولَّد مُلتزَم، و
`data/language/error_messages.yaml` نفسه فيه 156 يتيماً (يفشل أيضاً). التحقّق المُعتمَد بدلاً منه:
**التحقّقات الدلالية الستة للمولّد + تغطية 75/75 من enum**. التكامل في C++ (تجميع المُولَّد +
تقليص `error_codes.cpp`) **خارج نطاق EM-V5-1** → نطاق `EM-3`. المُولَّد لا يُجمَّع في أي target بعد
(لا خطر تكرار رموز الآن).

**قائمة الملفات (File List):**
- `scripts/codegen/gen_error_messages.py` (تعديل — `--yaml-dir` + `load_error_directory` + تصحيح ترويسة/docstring المصدر)
- `scripts/codegen/test_gen_error_messages_v5.py` (**جديد** — 7 اختبارات لمسار V5، كلها تمرّ)
- `cmake/codegen.cmake` (إضافة كتلة `sad_error_messages_codegen`)
- `language-truth/_schemas/error.schema.json` (T0)
- `shared/errors/generated/error_messages_generated.{h,cpp}` (مُولَّد — baseline)
- `language-truth/errors/internal.yaml` (محذوف — المالك)

**مراجعة الكود (Amelia self-review، 2026-06-10):**
- ✅ أُضيفت تغطية اختبار (7 اختبارات: بنية الدمج، prefix/ترتيب رسمي، تفرّد، 6 فحوص دلالية،
  تطابق enum، idempotent). كانت ناقصة — أُصلحت.
- ✅ صُحِّحت ترويسة الملف المُولَّد + docstring (كانت تشير إلى `data/language/` V4 الخطأ).
- 🔎 ملاحظة: اختبارات V4 القديمة (`test_gen_error_messages.py`) تتخطّى لغياب
  `data/language/error_messages.schema.json` (فجوة سابقة، لا تراجع من هذا التغيير).
- 🔎 مسار `--yaml` V4 سليم منطقياً (لم يتغيّر؛ نُقل `validate_schema` إلى فرع else).

---

## 🚫 حاجز (Blocker) BLK-1 — اكتُشِف 2026-06-10، ✅ مُحَلّ (حذف internal.yaml)

**الوصف:** كتالوج رسائل الأخطاء (V4 و V5 **متطابقان**: 231 رمزاً) يحوي **156 رمزاً `INT_*` في
`internal.yaml` غير موجودة في `ErrorCode` enum** (`shared/errors/include/error_codes.h`).
- **الدليل:** `enum = 75` رمزاً؛ `INT_INVARIANT` يظهر **0** مرة في الهيدر؛ الـ156 يتيماً كلها في
  `internal.yaml` بادئة `INT_`. (قياس بدالة المولّد `extract_enum_values`، 2026-06-10.)
- **الأثر:** المولّد `gen_error_messages.py` يرفض هذا (تحقّق دلالي #6: orphan codes) — فلا يمكن
  توليد كتالوج من V5 (ولا V4) قبل حلّ التعارُض. الملفات السبعة الأخرى (75 رمزاً) تطابق enum.
- **ليس خطأ في T1** — دريفت بيانات سابق لوجودي بين YAML و enum.

**خيارات الحل (قرار مطلوب — ق-فرعي-4):**
1. **إضافة الـ156 `INT_*` إلى `ErrorCode` enum** (إن كانت أخطاء invariant مستخدمة فعلاً).
2. **إقصاء `internal.yaml`** من توليد الكتالوج (إن كانت الثوابت الداخلية تُدار بآلية أخرى).
3. **حذف الـ156** من `internal.yaml` (إن كانت aspirational/مُولَّدة بالخطأ).
**يحكمها**: [ADR-DOCS-V4-005](../../living-documentation/decisions/ADR-DOCS-V4-005-UNIFIED-SOT-MIGRATION-COMPLETION.md) (المرحلة أ)
**التاريخ**: 2026-06-09

---

## الهدف
نقل مصدر توليد كتالوج رسائل الأخطاء من `data/language/error_messages.yaml` (V4) إلى
`language-truth/errors/*.yaml` (V5 — المصدر الموحَّد)، وربط التوليد بـ CMake.

## السياق
- القرار: `language-truth/` هو المصدر الموحَّد المطلق (ADR-DOCS-V4-005، ق-فرعي محسومة).
- الصيغة المُعتمَدة: **المتداخلة** (`title/brief/fix_hint/detailed`) — ق-فرعي-1.
- الوضع الحالي: `gen_error_messages.py` يقرأ من `data/language/error_messages.yaml`، و**توليد الأخطاء
  غير مربوط في `cmake/codegen.cmake`** (بخلاف الكلمات والدوال المضمنة).

## التنفيذ (Tasks/Subtasks)
- [x] **T0** — تصحيح `language-truth/_schemas/error.schema.json` ليصف الصيغة المتداخلة
  - دليل: الملفات تجتاز الـ schema بصفر مخالفات (`Draft202012Validator`).
- [x] **T1** — `gen_error_messages.py` يقرأ من `language-truth/errors/*.yaml` (`--yaml-dir` + دمج)
  - دليل: `🔁 75 رسالة`، exit 0، التحقّقات الدلالية الستة مرّت.
- [x] **T2** — ربط `sad_error_messages_codegen` في `cmake/codegen.cmake`
  - دليل: configure + build target كلاهما exit 0.
- [~] **T3** — golden مقابل V4 **ساقط** (لا baseline V4)؛ بديله: 6 فحوص دلالية + تغطية 75/75 enum ✅.
- [ ] **T4** — بناء `sad`/`sadc` كامل + لا تراجع: **مؤجَّل لـ EM-3** (المُولَّد لا يُجمَّع بعد — لا تكامل C++).

## معايير القبول
- **Given** `language-truth/errors/*.yaml` بالصيغة المتداخلة المُعتمَدة
- **When** يُبنى المشروع
- **Then** يُولَّد `shared/errors/generated/` من `language-truth/` (لا من `data/language/`)
- **And** كل رسائل الأخطاء مطابقة لـbaseline V4 (golden) بلا تراجع
- **And** التوليد يُعاد تلقائياً عند تغيّر أي ملف أخطاء (custom_command يعتمد عليه)

## تعريف "تم"
التوليد من V5 مربوط في CMake + golden أخضر + لا تراجع + المُولَّد مُلتزَم في git (GR-01).
