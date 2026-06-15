# Story EM-3 — تكامل + تقليص C++

**Epic**: EPIC-EM | **Story Points**: 8 | **الأولوية**: حرجة | **الحالة**: جاهزة
**التبعيات**: EM-2 | **المُسنَد إلى**: Amelia

---

## الهدف
استبدال الجداول اليدوية في `error_codes.cpp` و `error_catalog_init.cpp` باستخدام `kErrorMessages` المولَّد. تقليص حجم الملفين بـ ≥ 70% و ≥ 65% على التوالي.

## السياق
- [shared/errors/src/error_codes.cpp](../../../../shared/errors/src/error_codes.cpp) (843 سطر) — تقليص إلى ≤ 250 سطر.
- [shared/errors/src/error_catalog_init.cpp](../../../../shared/errors/src/error_catalog_init.cpp) (213 سطر) — تقليص إلى ≤ 80 سطر.
- 5+ مواقع تستدعي `getErrorCodeString` و `getErrorDescription` و `getErrorCategory` — يجب ألا تتغير سلوكياً (BF-15).

## التنفيذ

- [ ] **T1** — قبل التغيير: تسجيل golden baseline
  - [ ] كتابة `tests/errors/generate_golden.cpp` صغير يطبع لكل ErrorCode:
    `<code>|<getErrorCodeString>|<getErrorDescription AR>|<getErrorDescription EN>|<getErrorCategory>|<render BRIEF AR>|<render BRIEF EN>`
  - [ ] تشغيله → `tests/errors/golden_before.txt`

- [ ] **T2** — إعادة كتابة `error_codes.cpp`
  - [ ] حذف `errorCodeStrings` map
  - [ ] حذف `errorDescriptions` map
  - [ ] إعادة كتابة `getErrorCodeString(code)` → `findByCode(code)->id`
  - [ ] إعادة كتابة `getErrorDescription(code, lang)` → `findByCode(code)->briefAr/En`
  - [ ] إعادة كتابة `getErrorCategory(code)` → `findByCode(code)->category` (لا `substr`)
  - [ ] إبقاء كل الدوال الأخرى كما هي
  - [ ] التأكد من توقيع APIs العامة دون تغيير (BF-15)

- [ ] **T3** — إعادة كتابة `error_catalog_init.cpp`
  - [ ] حذف كل 26 × `registerTemplate(...)` يدوياً
  - [ ] إضافة حلقة واحدة على `Generated::kErrorMessages`
  - [ ] الحلقة تبني `ErrorTemplate` من `GeneratedErrorEntry` (انظر tech-spec §7.2)

- [ ] **T4** — البناء
  ```powershell
  cmake --build build --config Debug --target sad
  cmake --build build --config Debug --target sadc
  ```

- [ ] **T5** — بعد التغيير: تسجيل golden after
  - [ ] تشغيل نفس البرنامج → `tests/errors/golden_after.txt`
  - [ ] `Compare-Object (gc golden_before.txt) (gc golden_after.txt)` = ∅

- [ ] **T6** — تشغيل `run_tests.ps1` والتحقق من `PASS:137 / FAIL:20`

## معايير القبول (AC)

- **AC-3.1**: ✅ `ErrorCatalog::registerDefaults()` < 30 سطر.
- **AC-3.2**: ✅ `getErrorCodeString`/`getErrorDescription`/`getErrorCategory` تستخدم `findByCode` (CW-19).
- **AC-3.3**: ✅ `error_codes.cpp` ≤ 250 سطر.
- **AC-3.4**: ✅ `error_catalog_init.cpp` ≤ 80 سطر.
- **AC-3.5**: ✅ `run_tests.ps1` = `PASS:137 / FAIL:20` بالضبط (BF-29).
- **AC-3.6**: ✅ `getErrorCategory()` لا يستخدم `substr` (G-2).
- **AC-3.7**: ✅ `getErrorDescription()` يستخدم `findByCode` (G-3).
- **AC-3.8**: ✅ `LocalizedString` يبقى متاحاً للتوافق الخلفي (G-4, BF-15).
- **AC-3.9**: ✅ golden round-trip diff = 0 (سيتأكد في EM-4).

## File List
- (سيُملأ)

## Dev Agent Record
- (سيُملأ)
