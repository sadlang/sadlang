# Story EM-V5-3 — تقاعد V4 (حذف data/language/ للأخطاء) (المرحلة ج)

**Epic**: EPIC-EM | **Story Points**: 3 | **الأولوية**: متوسطة | **الحالة**: ✅ مكتملة
**التبعيات**: EM-V5-1 + EM-V5-2 (مُنجزتان) | **المُسنَد إلى**: Amelia

---

## ✅ سجل المطوّر (Dev Agent Record) — Amelia، 2026-06-10

- **T1 ✅** — تأكيد: لا مرجع نشط في البناء/الكود يقرأ `data/language/error_messages.yaml`
  (هدف CMake يستخدم `--yaml-dir language-truth`؛ المراجع الباقية وثائق/أدوات هجرة لمرة واحدة).
- **T2 ✅** — حُذف `data/language/error_messages.yaml` (كان غير متتبَّع في git). دليل عدم التراجع:
  توليد الأخطاء V5 + 14 اختبار codegen تمرّ **بعد** الحذف (مستقلة عن V4).
- **T3 ✅** — حُدِّثت مراجع مهارة `sad-lang-dev` (error-system, error-yaml-structures, playbooks,
  workflow, architecture, systems-catalog, docs-yaml-system) لإزالة «data/language نشط». وعُلِّم
  `ADR-EM-3` **Superseded** بـ ADR-005 (GR-02 — لا حذف).
- **T4 ✅** — تحقّق: 14 اختبار codegen أخضر، توليد V5 + إسقاط sadinfo يعملان، صفر مرجع V4 فعّال متبقٍّ.

**ملاحظة نطاق:** رسائل وقت التشغيل ما زالت من `error_codes.cpp` (تكامل C++ = EM-3). حذف V4 بلا
أثر runtime (لم يكن مُجمَّعاً أصلاً).

**قائمة الملفات:** حذف `data/language/error_messages.yaml`؛ تعديل 7 ملفات مهارة + `ADR-EM-3.md`.

## ✅ مراجعة الكود (Amelia self-review، 2026-06-10)

- ✅ **فحص شامل للمستودع** (لا المهارة وحدها): **صفر مرجع نشط** للملف المحذوف في
  `.py/.cmake/CMakeLists/.cpp/.h` (باستثناء أدوات هجرة لمرة واحدة + تاريخ).
- ✅ **الاختبار المتخطّي V4 لا ينكسر** بعد الحذف (13 skipped، لا error).
- 🧹 **تنظيف بقايا V4 (إكمال التقاعد):** حُذف `data/language/error_messages.yaml.draft` +
  `migration_report.txt` (مخلّفات `migrate_error_messages.py`).
- 🧹 **حُذف اختبار V4 الميت** `test_gen_error_messages.py` — كان يتخطّى دائماً (مصدره محذوف)،
  ونفس دواله (`extract_enum_values/sort_entries/validate_semantically/load_error_directory`)
  مُغطّاة في `test_gen_error_messages_v5.py`. **دليل عدم فقدان تغطية:** 14 اختبار يمرّ.
- 🔎 ملاحظة: مسار `--yaml` V4 (backward-compat) يبقى في المولّد — غير مُستخدَم لكن غير ضار؛
  إزالته اختيارية (نطاق تنظيف لاحق).
**يحكمها**: [ADR-DOCS-V4-005](../../living-documentation/decisions/ADR-DOCS-V4-005-UNIFIED-SOT-MIGRATION-COMPLETION.md) (المرحلة ج) + [ADR-DOCS-V4-003](../../living-documentation/decisions/ADR-DOCS-V4-003-CODEGEN-INTEGRATION.md)
**التاريخ**: 2026-06-09

---

## الهدف
بعد ثبات التوليد من V5 (EM-V5-1) وإسقاط sadinfo (EM-V5-2)، تقاعُد المصدر V4
`data/language/error_messages.yaml` نهائياً (تنفيذ M2 من ADR-DOCS-V4-003).

## السياق
- لا يُحذف V4 قبل أن يصبح `language-truth/` المصدر النشط الفعلي للتوليد (تبعية صارمة).
- مخاطرة مُدارة: طوال EM-V5-1/2 يبقى V4 نشطاً؛ هذه الستوري آخر خطوة.

## التنفيذ (Tasks/Subtasks)
- [x] **T1** — تأكيد لا مرجع نشط يقرأ `data/language/error_messages.yaml` (grep شامل).
- [x] **T2** — حذف `data/language/error_messages.yaml`.
- [x] **T3** — تحديث مراجع `sad-lang-dev` (7 ملفات) + تعليم `ADR-EM-3` Superseded.
- [x] **T4** — تحقّق: 14 اختبار codegen أخضر + توليد V5/sadinfo يعملان + صفر مرجع V4 فعّال.
  (ملاحظة: `runner.py P1` الكامل = EM-3، لأن المُولَّد لا يُجمَّع بعد.)

## معايير القبول
- **Given** التوليد من `language-truth/` نشط ومستقر (EM-V5-1) و sadinfo مُولَّد (EM-V5-2)
- **When** يُحذف مصدر الأخطاء V4
- **Then** البناء أخضر ولا تراجع في أي رسالة خطأ
- **And** لا مرجع متبقٍّ لـ`data/language/error_messages.yaml`
- **And** المهارة والوثائق محدّثة (لا ملاحظات انتقالية متبقّية)

## تعريف "تم"
`data/language/` (للأخطاء) محذوف + صفر مراجع + بناء/اختبارات خضراء + توثيق محدّث (GR-01).
