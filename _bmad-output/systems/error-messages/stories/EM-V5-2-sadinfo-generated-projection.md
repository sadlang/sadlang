# Story EM-V5-2 — تحويل كتالوج sadinfo إلى إسقاط مُولَّد (المرحلة ب)

**Epic**: EPIC-EM | **Story Points**: 5 | **الأولوية**: متوسطة | **الحالة**: 🟡 قيد التنفيذ (T1–T3 ✅)
**التبعيات**: EM-V5-1 | **المُسنَد إلى**: Amelia

---

## ✅ سجل المطوّر (Dev Agent Record) — Amelia، 2026-06-10

**القرار التصميمي (تعيين الفئات — حُسم مع المالك):** lexical→lexer، syntactic→parser،
semantic→type، runtime→runtime، **ownership→runtime، import→linker**، io→io. الرمز `E####`
تسلسلي عالمي بترتيب (category, id). حقول بلا مصدر V5: `owners=["@sadlang-core"]`,
`since="0.1.0"`, `severity="error"`. الاسم من `title.ar`.

**ما نُفِّذ ومُتحقَّق:**
- **T1 ✅** — حُدِّدت الحقول الغنية الناقصة (category map, owners, since, E-code) وحُسمت.
- **T2 ✅** — `gen_sadinfo_errors.py` يُسقِط من `language-truth/errors/` (يعيد استخدام
  `load_error_directory`). **دليل:** `✓ 75 كيان → data/errors`، exit 0؛ كل كيان يُتحقَّق منه
  مقابل `data/_schemas/error.schema.json` داخل المولّد.
- **T3 ✅** — `data/_schemas/error.schema.json` عقدٌ للإسقاط المُولَّد (المولّد يفشل إن خالف كيانٌ
  الـschema). لا مصدر يدوي.
- **CMake** — رُبط `sad_sadinfo_errors_codegen` في `cmake/codegen.cmake` (configure + build exit 0).
- **اختبار** — `test_gen_sadinfo_errors.py` (6 اختبارات، كلها تمرّ: تحقّق schema، تسلسل E####،
  تفرّد id، اكتمال التعيين، الاسم من title.ar، idempotent).

**مراجعة الكود (Amelia self-review، 2026-06-10) — عيبان أُصلحا:**
- 🔧 **رموز `E####` كانت موضعية (غير مستقرة)** — إدراج خطأ يُزيح كل الرموز التالية، يكسر عقد
  «مفتاح أساسي ثابت». **الإصلاح:** `stable_code()` يشتقّ الرمز من (فهرس الفئة×1000 + رقم id) —
  مستقر. دليل: `LEX001→E0001`, `SEM002→E2002` (semantic=فئة 2). اختبار `test_codes_are_stable_not_positional`.
- 🔧 **لا تنظيف للكيانات اليتيمة** — حذف خطأ من V5 كان يترك مجلد sadinfo معلّقاً. **الإصلاح:**
  المولّد يحذف `error_*` غير الموجودة (مرآة وفيّة). دليل: مجلد وهمي يُحذف عند إعادة التوليد ✓.
- ✅ تحقّق schema **قبل** أي كتابة (لا كتابة جزئية عند الفشل).
- الاختبارات: **7** (كانت 6) — أُضيف اختبار استقرار الرمز. كل اختبارات codegen V5+sadinfo (14) تمرّ.

**متبقٍّ:** T4 — مواءمة `sadinfo --errors` (EM-5) ليقرأ `data/errors/` المُولَّد (تكامل أداة sadinfo).

**قائمة الملفات (File List):**
- `scripts/codegen/gen_sadinfo_errors.py` (**جديد** — المُسقِط)
- `scripts/codegen/test_gen_sadinfo_errors.py` (**جديد** — 6 اختبارات)
- `cmake/codegen.cmake` (كتلة `sad_sadinfo_errors_codegen`)
- `data/errors/error_*/_index.yaml` (**مُولَّد** — 75 كياناً baseline)
**يحكمها**: [ADR-DOCS-V4-005](../../living-documentation/decisions/ADR-DOCS-V4-005-UNIFIED-SOT-MIGRATION-COMPLETION.md) (المرحلة ب + ق-فرعي-2)
**التاريخ**: 2026-06-09

---

## الهدف
جعل كتالوج بيانات `sadinfo` للأخطاء **إسقاطاً مُولَّداً** من `language-truth/errors/*.yaml`،
بدل كونه مصدراً يدوياً موازياً.

## السياق
- القرار (ق-فرعي-2، محسوم): sadinfo إسقاط مُولَّد لا مصدر — اتجاه التوليد يحدّد المصدر (استشارة PM).
- البنية الحالية: `data/_schemas/error.schema.json` (v1) + كيانات `errors/<id>/_index.yaml`
  (`code: E####`, `kind`, `name`, `severity`, `owners`) — نظام عرض/فهرسة، يُصان يدوياً اليوم.
- الحقول الغنية (`owners`/`severity`/i18n/summary) قد تحتاج إضافة إلى `language-truth/` أو overlay.

## التنفيذ (Tasks/Subtasks)
- [x] **T1** — حُدِّدت الحقول الغنية (category map + owners + since + E-code) وحُسمت كافتراضات موثّقة.
- [x] **T2** — `gen_sadinfo_errors.py` يُنتج كتالوج sadinfo من `language-truth/errors/*.yaml` (75 كياناً).
- [x] **T3** — `data/_schemas/error.schema.json` عقد الإسقاط (تحقّق داخل المولّد) — لا مصدر يدوي.
- [ ] **T4** — مواءمة `sadinfo --errors` (EM-5) ليقرأ `data/errors/` المُولَّد (تكامل أداة).

## معايير القبول
- **Given** `language-truth/errors/*.yaml` هو المصدر
- **When** يُشغَّل توليد كتالوج sadinfo
- **Then** يُنتَج كتالوج الكيانات (`E####`/`owners`/`severity`) آلياً من المصدر
- **And** لا ملف كتالوج sadinfo يُحرَّر يدوياً (كله مُولَّد)
- **And** `validate_schemas.py` أخضر على الإسقاط

## تعريف "تم"
كتالوج sadinfo مُولَّد بالكامل من `language-truth/` + لا مصدر يدوي موازٍ + تحقّق أخضر (GR-01).
