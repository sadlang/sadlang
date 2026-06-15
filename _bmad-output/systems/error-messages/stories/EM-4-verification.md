# Story EM-4 — اختبارات الإثبات + golden test

**Epic**: EPIC-EM | **Story Points**: 3 | **الأولوية**: حرجة | **الحالة**: جاهزة
**التبعيات**: EM-3 | **المُسنَد إلى**: Amelia

---

## الهدف
إثبات أن الترحيل لم يكسر أي سلوك + إضافة اختبارات C++ شاملة لـ`findByCode` و `findById`.

## التنفيذ

- [ ] **T1** — إنشاء `tests/errors/test_generated_messages.cpp`
  - [ ] `test_count_matches_enum_size` — عدد entries = عدد قيم enum
  - [ ] `test_all_ids_unique`
  - [ ] `test_all_codes_unique`
  - [ ] `test_findByCode_for_each_known_code` — لكل قيمة enum يُرجع entry صحيح
  - [ ] `test_findByCode_invalid_returns_nullptr` — `static_cast<ErrorCode>(999999)` → `nullptr`
  - [ ] `test_findById_for_each_id`
  - [ ] `test_specific_SEM_TYPE_MISMATCH` — فحص محتوى entry بالاسم

- [ ] **T2** — إضافة الاختبارات إلى `tests/CMakeLists.txt`

- [ ] **T3** — `Compare-Object (gc golden_before.txt) (gc golden_after.txt)` يجب أن يُرجع 0 فروق (BF-22)

- [ ] **T4** — تشغيل comprehensive tests
  ```powershell
  cmake --build build --config Debug --target comprehensive_tests
  ctest --test-dir build -R Comprehensive
  ```

- [ ] **T5** — تشغيل `.\run_tests.ps1` والتحقق من `PASS:137 / FAIL:20`

- [ ] **T6** — اختبار إضافة خطأ تجريبي (PLAN-AC-6)
  - [ ] إضافة `TEST_DUMMY_ERROR` إلى `ErrorCode` enum
  - [ ] إضافة entry في `error_messages.yaml`
  - [ ] إعادة بناء — يجب أن ينجح بدون أي تعديل في `.cpp`
  - [ ] حذف الـtest entry بعد التأكُّد

## معايير القبول (AC)

- **AC-4.1**: ✅ `tests/errors/test_generated_messages.cpp` ينجح بكل الـ7 cases.
- **AC-4.2**: ✅ golden round-trip diff = 0 (BF-22).
- **AC-4.3**: ✅ `run_tests.ps1` = `PASS:137 / FAIL:20` بالضبط.
- **AC-4.4**: ✅ comprehensive tests تنجح بنفس نتائج ما قبل التغيير.
- **AC-4.5**: ✅ إضافة خطأ تجريبي = تعديل YAML + enum فقط (لا `.cpp`).
- **AC-4.6**: ✅ تحديث [/memories/repo/yaml_cpp_integration.md](../../../../memories/repo/yaml_cpp_integration.md) بـsection جديد عن error_messages.

## File List
- (سيُملأ)

## Dev Agent Record
- (سيُملأ)
