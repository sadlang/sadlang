# Story EM-V5-4 — ربط كل نطاقات language-truth في CMake (توليد آلي)

**Epic**: EPIC-EM | **Story Points**: 8 | **الأولوية**: عالية | **الحالة**: ✅ مكتملة
**التبعيات**: EM-V5-1 (نمط ربط الأخطاء) | **المُسنَد إلى**: Amelia

---

## ✅ سجل المطوّر (Dev Agent Record) — Amelia، 2026-06-10

**النهج (DRY — CW-19):** بدل 10 كتل CMake مكرّرة، أُضيفت **دالة موحَّدة `sad_add_codegen(NAME
OUTPUTS… DEPS… ARGS…)`** في `cmake/codegen.cmake`، تُستدعى لكل نطاق. الوسائط **مطابقة لـ
`gen_all.py`** المثبت (المصدر الموثوق لواجهة كل مولّد).

**النطاقات المربوطة (10):** operators, directives, types, grammar, modules, stdlib, type_methods
(مخرَجان .h/.cpp), patterns, oop_constructs, expr_constructs (الأخيرة الثلاث عبر `gen_constructs.py`
بوسائط بنية). + هدف تجميعي **`sad_all_codegen`** يجمع كل الـ13 هدفاً (مع keywords/builtins/errors/sadinfo).

**التحقّق (دليل تشغيلي):**
- `cmake -S . -B build` → "All-domains codegen wired (V5): 13 targets" (exit 0).
- `cmake --build build --target sad_all_codegen` → **كل النطاقات الـ10 تتولّد** (directives,
  expr_constructs, grammar, modules, oop_constructs, operators, patterns, stdlib, type_methods,
  types) + builtins + sadinfo، **exit 0، لا أخطاء**.

**قائمة الملفات:** `cmake/codegen.cmake` (دالة `sad_add_codegen` + 10 استدعاءات + `sad_all_codegen`)
+ `cmake/libraries.cmake` (ربط `sad_shared` بـ `sad_all_codegen`).

## ✅ مراجعة الكود (Amelia self-review، 2026-06-10) — عيبان أُصلحا

- 🔧 **الأهداف لم تكن مربوطة بالبناء:** `libraries.cmake` كان يربط `sad_shared` بـ
  `sad_keywords_codegen` **فقط** — فالنطاقات الجديدة لا تُعاد تلقائياً في البناء العادي (فقط
  يدوياً). **الإصلاح:** `add_dependencies(sad_shared sad_all_codegen)` — أي تعديل YAML في أي نطاق
  يُعاد توليده عند بناء `sad_shared`. (يحقّق هدف الستوري فعلياً.)
- 🔧 **انعدام idempotency:** المولّدات تستخدم `write_if_changed` فلا تُحدِّث mtime عند عدم التغيير،
  فيعيد CMake التوليد كل بناء. **الإصلاح:** `sad_add_codegen` صار بنمط **stamp** (touch بعد كل
  توليد) + `BYPRODUCTS`. **دليل:** بناء أول = 10 توليدات، بناء ثانٍ = **0** (no-op).
- ✅ الوسائط مطابقة لـ `gen_all.py` (لا اختراع)؛ الدالة DRY (CW-19).
- 🔧 **تحسين تالٍ:** طُبِّق نمط stamp أيضاً على الكتل الثلاث القديمة (keywords, builtin_registry,
  error_messages) — كانت تحمل نفس عيب الـidempotency. **النتيجة: كل الـ13 هدفاً idempotent**
  (بناء ثانٍ = 0 توليد). نفس ملف `cmake/codegen.cmake`.
**يحكمها**: [ADR-DOCS-V4-005](../../living-documentation/decisions/ADR-DOCS-V4-005-UNIFIED-SOT-MIGRATION-COMPLETION.md) (ق-فرعي-3) + [ADR-DOCS-V4-003](../../living-documentation/decisions/ADR-DOCS-V4-003-CODEGEN-INTEGRATION.md)
**التاريخ**: 2026-06-10

---

## الهدف
ربط **كل نطاقات** `language-truth/` بالتوليد الآلي وقت البناء في `cmake/codegen.cmake`، بحيث
يُعاد توليد C++ لكل نطاق تلقائياً عند تغيّر YAML — لا تشغيل يدوي لـ`gen_all.py`.

## السياق
- القرار (ق-فرعي-3، محسوم 2026-06-10): ربط كل النطاقات في CMake (لا `gen_all.py` يدوي).
- الحالة الراهنة (دليل، ADR-005 §مصفوفة الحالة): **نطاقان فقط مربوطان** (`sad_keywords_codegen`،
  `sad_builtin_registry_codegen`)؛ بقية النطاقات baseline يدوي عبر `gen_all.py`.
- النمط المرجعي: كتلتا keywords/builtins في `cmake/codegen.cmake` (`add_custom_command` +
  `add_custom_target` + `DEPENDS` على ملفات YAML والمولّد).

## التنفيذ (Tasks/Subtasks)
> لكل نطاق: هدف `add_custom_target` يعتمد على `add_custom_command` (YAML → مولّد → مخرَج)،
> ويُربَط في تبعيات البناء، ويعتمد على `sad_check_codegen_env`.
- [x] **T1–T7** — operators, directives, types, type_methods, patterns, grammar, oop/expr_constructs,
  modules, stdlib — كلها مربوطة عبر `sad_add_codegen` (10 نطاقات).
- [x] **T8** — errors مربوطة في EM-V5-1 (`sad_error_messages_codegen`) — لا ازدواج؛ مُضمَّنة في التجميعي.
- [x] **T9** — هدف تجميعي `sad_all_codegen` (13 هدفاً).
- [x] **T10** — التحقّق: build target → كل النطاقات تتولّد، exit 0.

## معايير القبول
- **Given** كل نطاق له `language-truth/*.yaml` + `gen_*.py` + مخرَج مُولَّد
- **When** يُبنى المشروع بعد تغيير YAML في أي نطاق
- **Then** يُعاد توليد C++ لذلك النطاق تلقائياً (بلا تشغيل `gen_all.py` يدوي)
- **And** كل ≥13 نطاقاً مربوط بهدف CMake يعتمد على `sad_check_codegen_env`
- **And** البناء أخضر و`runner.py P1` بلا تراجع
- **And** الـ baseline المُولَّد يبقى مُلتزَماً في git (بناء بلا Python يبقى ممكناً من baseline)

## تعريف "تم"
كل النطاقات مربوطة في `cmake/codegen.cmake` + إعادة توليد آلية مُثبَتة لكل نطاق + لا تراجع (GR-01).
