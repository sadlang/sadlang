# S-003: Tier 1 Validator (Inline)

> **Phase:** 1 (POC) | **Effort:** S | **Risk:** Low
> **Depends on:** S-001 | **Blocks:** S-005
> **Arch ref:** §3.3 (Tier 1)

---

## User Story

**As** loader pipeline،
**I want** فحوصات Tier 1 (cheap) تجري inline لكل entity وقت التحميل،
**So that** الأخطاء الأساسية تُكتشف فوراً قبل بناء cache.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `Validator::validateTier1(entity) → std::vector<ValidationError>` |
| AC-2 | يفحص: YAML syntax (تم بـloader)، required fields، types، enum values |
| AC-3 | يفحص: ID matches folder name (`builtin_print_line/` يحوي `id: builtin_print_line`) |
| AC-4 | يفحص: لا duplicate IDs داخل category |
| AC-5 | يفحص: schema version مدعوم (`schema: 1`) |
| AC-6 | < 1ms لكل entity (يجري inline في scan) |
| AC-7 | errors تُجمَّع في نفس `EntityOrError` (لا exception) |

## Tasks

- [x] T1: تعريف `ValidationError { entity_id, kind, field, message }`
- [x] T2: enum `Tier1ErrorKind { MissingField, WrongType, InvalidEnum, IdMismatch, DuplicateId, UnsupportedSchema, MissingIndexFile, InvalidYamlStructure }`
- [x] T3: schema definitions لكل category في `data/_schemas/tier1.*.yaml` + whitelist.yaml
- [x] T4: implementation `tier1_cheap.cpp` (مُسمَّى وفق §4 من SADINFO_TARGET_ARCHITECTURE.md)
- [x] T5: hooks في Loader (`apply_tier1_validation` + `DuplicateIdTracker` لكل category)
- [x] T6: Unit tests: 16 unit tests (8 error kinds + valid + edge cases)
- [x] T7: Integration test مع `validated_data/` fixture (10 كيانات: 5 ناجحة + 5 SchemaViolation)

## File List

**جديد:**
- `shared/sadinfo_core/include/validator.h`
- `shared/sadinfo_core/include/validation_error.h` — يضم `to_string(Tier1ErrorKind)` snake_case
- `shared/sadinfo_core/include/tier1_schema.h` — `Tier1Schema` + `Tier1SchemaRegistry` (singleton)
- `shared/sadinfo_core/src/validator/tier1_cheap.cpp` — Tier1 cheap validator (<1ms inline)
- `shared/sadinfo_core/src/validator/tier1_schema.cpp` — تحميل tier1.*.yaml من القرص
- `data/_schemas/tier1.builtin.yaml` + `tier1.keyword.yaml` + `tier1.error.yaml` + `tier1.lesson.yaml`
- `data/_schemas/whitelist.yaml` — whitelist للحقول المسموحة
- `tests/sadinfo/unit/validator_tier1_tests.cpp` — 16 unit tests
- `tests/sadinfo/fixtures/validated_data/` — 10 fixtures عبر 4 categories
- `tests/sadinfo/fixtures/validated_data/_schemas/` — نسخة محلية من tier1 schemas

**معدَّل:**
- `shared/sadinfo_core/CMakeLists.txt` — إضافة `src/validator/tier1_cheap.cpp` + `tier1_schema.cpp`
- `shared/sadinfo_core/src/loader/loader.cpp` — `apply_tier1_validation()` helper + `DuplicateIdTracker` per-category داخل `scan()`
- `tests/sadinfo/unit/loader_tests.cpp` — `test_scan_validated_data_schema_violations()` (T7)

## Definition of Done

- [x] 8 error kinds + valid مُختبَرة (16 unit + 1 integration)
- [x] Perf budget < 1ms/entity (مقاس inline دون allocations زائدة)
- [x] schema files موثَّقة بـcomments عربية + ثنائية اللغة في APIs العامة (CW-08)
- [x] error messages عربية واضحة (مثل `"id_mismatch: id داخل YAML لا يطابق اسم المجلد"`)
- [x] ctest sadinfo: **6/6 PASSED**

## Dev Agent Record (Amelia)

### Implementation Summary

نُفِّذت القصة على 7 مهام (T1–T7) + جولتي مراجعة Amelia (refactor) + إصلاح حرج BF-04 + إعادة تسمية معمارية.

### Key Decisions

1. **CW-19 (DRY) — Refactor #2**: استخراج `try_get_scalar_string()` كـnamespace-private helper في `tier1_cheap.cpp` لإزالة 3 تكرارات لنفس نمط `IsScalar() + as<string>() + try/catch`.
2. **CW-22 (Error handling unified)**: كل Tier1 violation يُحوَّل في `apply_tier1_validation()` إلى رسالة `"kind_sv: message; kind_sv: message"` ويُرجَع كـ `EntityError{folder, SchemaViolation, combined}`.
3. **CW-25/CW-28**: التحقق inline بدون نسخ YAML — يستخدم `const YAML::Node &`.
4. **Architectural compliance**: إعادة تسمية `validator_tier1.cpp → tier1_cheap.cpp` ليطابق §4 من `SADINFO_TARGET_ARCHITECTURE.md`.

### Critical Fix — BF-04 (Root Cause Analysis)

**Bug**: validator كان يقارن اسم المجلد الكامل (`builtin_demo`) بـ`id_suffix` المستخرج من YAML (`demo`)، فيُطلق `IdMismatch` خاطئاً لكل الكيانات.

**Root cause**: عدم احترام اتفاقية أن `Entity` يُبنى بـ`cat.prefix + id_segment`، فاسم المجلد على القرص هو `<kind>_<id>` بينما `id` في YAML هو الجزء بعد البادئة فقط.

**Fix** (BF-10 — في الطبقة الصحيحة): في section 6 من `tier1_cheap.cpp`، نزع البادئة `<kind>_` من `folder_full` قبل المقارنة:
```cpp
std::string folder_id = folder_full;
const std::string prefix = std::string(kind_sv) + "_";
if (folder_id.rfind(prefix, 0) == 0) folder_id = folder_id.substr(prefix.size());
if (folder_id != id_suffix) report(IdMismatch, ...);
```

### Discovery — DuplicateId Unreachable from Disk Scan

اكتُشف أثناء توسيع fixtures أن `DuplicateIdTracker` لا يمكن تفعيله من scan قرصي:
- `Entity::id()` مشتق من اسم المجلد (`cat.prefix + folder_name`)
- أسماء المجلدات فريدة بطبيعة الـFS
- ∴ tracker (الذي يفهرس على `entity.id()`) لا يرى تكراراً أبداً

**القرار**: حُذف مجلد `keyword_dup2` التجريبي. الفحص يبقى نافذاً للاختبارات الوحدوية التي تبني `Entity` يدوياً (موجود في `validator_tier1_tests.cpp`). وُثِّق في تعليق T7 وفي `compiler_fix_notes` (درس BF-30).

### File List (Final — مُحدَّث بعد كل T)

راجع قسم **File List** أعلاه.

### Tests Status

- **Unit (validator_tier1)**: 16/16 ✅
- **Unit (loader incl. T7)**: 13/13 ✅
- **ctest sadinfo label**: **6/6 PASSED** بدون أخطاء.

### Lessons (BF-30)

1. **BF-04**: اقرأ اتفاقيات بناء الكائنات قبل كتابة منطق المقارنة — `Entity::id()` ليس مرادفاً لـ`folder_name`.
2. **Design discovery**: ميزة في الواجهة (`DuplicateIdTracker`) لا تعني تلقائياً أنها قابلة للتفعيل من جميع المسارات — `entity.id()` المشتق من FS يجعل بعض الفحوصات نظرية فقط.
3. **CW-19 + CW-21**: نمط مكرر 3 مرات = استخراج helper إلزامي. الـheader (`tier1_cheap.cpp` namespace anonymous) يحافظ على واجهة نظيفة.

### Status

✅ **DONE** — 2025-12-15 (Amelia/bmad-agent-dev)
