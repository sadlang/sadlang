# Story EM-1 — استخراج SOT (YAML + Schema + 203 entry)

**Epic**: EPIC-EM | **Story Points**: 8 | **الأولوية**: حرجة | **الحالة**: جاهزة
**التبعيات**: لا شيء | **المُسنَد إلى**: Amelia

---

## الهدف
استخراج كل رسائل الأخطاء (203 ErrorCode) من ملفات C++ إلى `data/language/error_messages.yaml` + إنشاء Schema JSON للتحقُّق.

## السياق
المصادر الحالية:
- [shared/errors/src/error_codes.cpp:30-280](../../../../shared/errors/src/error_codes.cpp) — `errorCodeStrings` map (203 إدخال)
- [shared/errors/src/error_codes.cpp:282-780](../../../../shared/errors/src/error_codes.cpp) — `errorDescriptions` map (203 إدخال)
- [shared/errors/src/error_catalog_init.cpp:60-200](../../../../shared/errors/src/error_catalog_init.cpp) — 26 template غني (`registerTemplate`)
- [shared/errors/include/error_codes.h](../../../../shared/errors/include/error_codes.h) — `ErrorCode` enum (203 قيمة)

## التنفيذ (Tasks/Subtasks)

- [ ] **T1** — إنشاء `scripts/codegen/migrate_error_messages.py` (مؤقت)
  - [ ] قراءة `errorCodeStrings` بـregex → `(ErrorCode, id)`
  - [ ] قراءة `errorDescriptions` بـregex → `(ErrorCode, briefAr, briefEn)`
  - [ ] قراءة `registerTemplate(...)` بـregex → template كامل لـ26 خطأ
  - [ ] دمج البيانات + استنتاج `category` من id prefix
  - [ ] كتابة `data/language/error_messages.yaml.draft`
  - [ ] كتابة `migration_report.txt` (إحصائيات)

- [ ] **T2** — إنشاء `data/language/error_messages.schema.json` (JSON Schema draft-07)
  - [ ] انظر [tech-spec-error-messages.md §3.2](../tech-spec-error-messages.md)

- [ ] **T3** — تشغيل migrate script + مراجعة بشرية للـdraft
  - [ ] مقارنة عدد الـentries مع enum (يجب 203)
  - [ ] مراجعة كل entry بسرعة لاكتشاف أخطاء regex
  - [ ] تنظيف اليدوي + تحسين برتايل (rearrange) لـ category

- [ ] **T4** — تحقُّق الـSchema يدوياً
  ```powershell
  python -c "import json, yaml, jsonschema; jsonschema.validate(yaml.safe_load(open('data/language/error_messages.yaml', encoding='utf-8')), json.load(open('data/language/error_messages.schema.json', encoding='utf-8')))"
  ```

- [ ] **T5** — اعتماد `error_messages.yaml` (إزالة `.draft`)

## معايير القبول (AC)

- **AC-1.1**: ✅ `data/language/error_messages.yaml` يحوي كل الـ203 ErrorCode بـ `id` + `category` + `title.{ar,en}` + `brief.{ar,en}`.
- **AC-1.2**: ✅ كل الـ26 ErrorCode التي لها templates غنية في `error_catalog_init.cpp` تُنقل بكل حقولها (`detailed`, `teacher`, `fix_hint`, `code_example`).
- **AC-1.3**: ✅ `data/language/error_messages.schema.json` موجود ويتحقَّق بنجاح.
- **AC-1.4**: ✅ `migrate_error_messages.py` يولِّد diff report مع الأصل (BF-22).
- **AC-1.5**: ✅ مراجعة بشرية مكتملة (لا regex وحده).
- **AC-1.6**: ✅ ترتيب الإدخالات: حسب `category` ثم `id` تصاعدياً (CW-27).

## الاختبار
- يدوي: تحقُّق Schema (T4).
- آلي مؤجَّل لـEM-2 (في pytest).

## ملاحظات للمنفِّذ
- الـregex قد يفشل على نصوص متعددة الأسطر — استخدم `re.DOTALL`.
- النصوص العربية في C++ قد تستخدم escape sequences — افحص بـUTF-8 raw.
- لا تستعجل المراجعة — كل خطأ مفقود يكتشف لاحقاً يكلف 10× الوقت.

## File List (يُحدَّث أثناء التنفيذ)
- (سيُملأ)

## Dev Agent Record
- (سيُملأ)
