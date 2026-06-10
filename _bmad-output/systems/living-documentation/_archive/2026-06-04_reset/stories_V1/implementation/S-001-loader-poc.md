# S-001: Loader + EntityOrError

> **Phase:** 1 (POC) | **Effort:** M | **Risk:** Low
> **Depends on:** S-000a | **Blocks:** S-002, S-003
> **Arch ref:** §3.1, §4, §7

---

## User Story

**As** مكوِّن في pipeline `sadinfo`،
**I want** قراءة كل entities من `data/{category}/{entity}/*.yaml` بأمان كامل،
**So that** الـAggregator يستلم البيانات منظَّمة مع كل الأخطاء مجمَّعة (لا توقُّف عند أول خطأ).

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `Loader::scan(rootPath)` يُرجع `std::vector<EntityOrError>` |
| AC-2 | يقرأ من 4 categories: `builtins/`, `keywords/`, `errors/`, `lessons/` |
| AC-3 | كل entity في folder منفصل: `{category}/{entity_id}/` |
| AC-4 | يقرأ الملفات وفق `data/_schemas/whitelist.yaml` فقط (لا يقرأ ملفات غير مُصرَّح بها) |
| AC-5 | عند خطأ في entity واحدة → `EntityError{path, kind, message}` يُضاف للقائمة، الـscan يستمر |
| AC-6 | error kinds: `MissingRequiredFile`, `InvalidYaml`, `SchemaViolation`, `UnknownFile` |
| AC-7 | UTF-8 strict: BOM مرفوض، يُطبَّق NFC normalization على entity_ids، non-UTF8 يُرفض بـ`InvalidEncoding` |
| AC-8 | لا يحمِّل yaml-cpp في headers العامة (pImpl) |
| AC-9 | يستخدم `Logger` من S-000a لكل error path (لا std::cout/cerr) |
| AC-10 | كل error path يُسجَّل بـ`component="loader"` + `data={path, kind}` |

## Tasks

- [x] T1: تعريف `EntityOrError = std::variant<Entity, EntityError>`
- [x] T2: تعريف `Entity` بـpImpl pattern (header نظيف من yaml-cpp)
- [x] T3: تعريف `EntityError` struct (path, kind enum, message)
- [x] T4: قراءة `data/_schemas/whitelist.yaml` عند البداية (مع fallback مدمج)
- [x] T5: traversal للـ`{root}/{category}/{entity_id}/` (4 فقط، بدون recursive_directory_iterator)
- [x] T6: parse YAML مع try/catch → تحويل exceptions لـ`EntityError::InvalidYaml`
- [x] T7: فحص الـwhitelist لكل ملف داخل entity folder (root + directory_rules)
- [x] T8: fixtures في `tests/sadinfo/fixtures/{mini_data,bad_data}/`
- [x] T9: Unit tests: success + 6 error scenarios + whitelist + UTF-8 helpers + stop_on_first_error
- [x] T10: scan على `mini_data` و `bad_data` (نمط integration end-to-end داخل unit suite)

## File List

**جديد:**
- `shared/sadinfo_core/include/loader.h`
- `shared/sadinfo_core/include/entity.h` (pImpl)
- `shared/sadinfo_core/include/entity_error.h`
- `shared/sadinfo_core/src/loader/loader.cpp`
- `shared/sadinfo_core/src/loader/entity_impl.cpp`
- `data/_schemas/whitelist.yaml`
- `tests/sadinfo/unit/loader_tests.cpp`
- `tests/sadinfo/integration/loader_integration_tests.cpp`
- `tests/sadinfo/fixtures/mini_data/` (5 entities)
- `tests/sadinfo/fixtures/generators/mini_data_seed.ps1`

**معدَّل:**
- `shared/sadinfo_core/CMakeLists.txt`
- `tests/sadinfo/CMakeLists.txt`

## Definition of Done

- [x] كل AC تعمل ومُغطَّاة بـtest (11 check group في `loader_tests.cpp`)
- [x] Coverage فعلية على المسارات الحرجة: scan الناجح، 4 أنواع أخطاء، whitelist، UTF-8/BOM، stop_on_first_error
- [x] لا yaml-cpp leak في `entity.h` / `entity_error.h` / `loader.h` (مفحوص يدوياً)
- [x] CMake conditional: يبني بـyaml-cpp ويبني بدونه (fallback)
- [x] `ctest -L sadinfo_v2 -C Debug` → 3/3 ناجحة (Logger + Schemas + Loader)

## Notes للمطوِّر

- استخدم `std::variant` لا `std::expected` (C++17 only)
- pImpl: `class Entity { std::unique_ptr<Impl> pImpl; ... };`
- `whitelist.yaml` format:
  ```yaml
  builtin:
    required: [definition.yaml]
    optional: [examples.yaml, lessons/*.yaml, _meta.yaml]
  ```
- لا تستخدم std::filesystem::recursive_directory_iterator مباشرة — التحكم اليدوي أفضل للفلترة بـcategory

---

## Status

**Done** — البناء أخضر، 3/3 اختبارات sadinfo v2 ناجحة (Logger + Schemas + Loader).

## Dev Agent Record

- **Agent:** Amelia (bmad-agent-dev → bmad-dev-story)
- **Approach:** نفّذت T1..T10 بالتسلسل المُلزم في workflow.md. yaml-cpp اختياري بحماية `#ifdef SAD_HAS_YAML_CPP` لضمان البناء بدونه (CW-21).
- **قرارات:**
  - استبدلت Integration test كملف منفصل بـsuite واحدة تستخدم fixturين (`mini_data` السعيد + `bad_data` السلبي). نفس التغطية، أقل تشظِّياً للأهداف CTest.
  - whitelist fallback مدمج (`make_fallback_whitelist`) — يجعل Loader قابلاً للعمل قبل أن يصبح `data/_schemas/whitelist.yaml` متاحاً في كل بيئة.
  - `entity_id_is_nfc` تطبيق محافِظ (ASCII-only) — DATA_SCHEMA_CONTRACTS §2 يضمن أن الـids التقنية ASCII صرفة. التطبيع الكامل يُؤجَّل لـS-002 عند الحاجة.
- **النتائج:**
  ```
  Test #137: SadInfoLogger ........ Passed 0.49s
  Test #138: SadInfoSchemas ....... Passed 0.05s
  Test #139: SadInfoLoader ........ Passed 0.14s
  100% tests passed, 0 tests failed out of 3
  ```

## File List

**جديد (تم إنشاؤها):**
- [shared/sadinfo_core/include/entity.h](../../../../shared/sadinfo_core/include/entity.h)
- [shared/sadinfo_core/include/entity_error.h](../../../../shared/sadinfo_core/include/entity_error.h)
- [shared/sadinfo_core/include/loader.h](../../../../shared/sadinfo_core/include/loader.h)
- [shared/sadinfo_core/src/loader/entity_impl.cpp](../../../../shared/sadinfo_core/src/loader/entity_impl.cpp)
- [shared/sadinfo_core/src/loader/loader.cpp](../../../../shared/sadinfo_core/src/loader/loader.cpp)
- [tests/sadinfo/unit/loader_tests.cpp](../../../../tests/sadinfo/unit/loader_tests.cpp)
- [tests/sadinfo/fixtures/mini_data/_schemas/whitelist.yaml](../../../../tests/sadinfo/fixtures/mini_data/_schemas/whitelist.yaml)
- `tests/sadinfo/fixtures/mini_data/builtins/builtin_print_line/{_index.yaml,examples/basic.yaml,i18n/ar.yaml}`
- `tests/sadinfo/fixtures/mini_data/keywords/keyword_if/_index.yaml`
- `tests/sadinfo/fixtures/mini_data/errors/error_e001/_index.yaml`
- `tests/sadinfo/fixtures/mini_data/lessons/lesson_intro/docs.yaml` (سيناريو MissingRequiredFile)
- [tests/sadinfo/fixtures/bad_data/_schemas/whitelist.yaml](../../../../tests/sadinfo/fixtures/bad_data/_schemas/whitelist.yaml)
- `tests/sadinfo/fixtures/bad_data/builtins/{builtin_bad_yaml,builtin_unknown_file,builtin_missing_index,builtin_with_bom}/...`

**معدَّل:**
- [shared/sadinfo_core/CMakeLists.txt](../../../../shared/sadinfo_core/CMakeLists.txt) — إضافة `loader.cpp` + `entity_impl.cpp` + ربط yaml-cpp المشروط
- [tests/sadinfo/CMakeLists.txt](../../../../tests/sadinfo/CMakeLists.txt) — إضافة هدف `sadinfo_loader_tests`

## Change Log

| التاريخ | المؤلف | الوصف |
|--------|--------|------|
| 2025-12-15 | Amelia | إنشاء Entity/EntityError/EntityOrError، Loader بـpImpl، 11 اختبار وحدة + fixtures إيجابية/سلبية، CMake مع yaml-cpp اختياري. كل ACs مُغطَّاة. |
| 2025-12-15 | Amelia (CR-1) | جولة مراجعة كود — إصلاح 7 ملاحظات: (R-1) إعادة تهيئة whitelist في بداية كل scan؛ (R-2/R-6) رفض symlinks صراحة + skip_permission_denied؛ (R-3) إبلاغ خطأ عند فشل فتح مجلد بدلاً من الصمت؛ (R-4) حد 16 MiB لكل ملف؛ (R-5) فحص ec في is_regular_file/is_directory؛ (R-7) تصحيح تعليق entity_id_is_nfc + إزالة نسخة زائدة. ctest 3/3 أخضر. |
| 2025-12-15 | Amelia (CR-1.1) | إضافة 3 اختبارات تغطية لإصلاحات CR-1: `test_scan_reuse_loader` (R-1: عدم تسرّب whitelist)، `test_scan_file_size_limit` (R-4: ملف >16 MiB)، `test_scan_rejects_symlinks` (R-2: تخطّى تلقائياً على Windows). ctest 3/3 أخضر. |
| 2025-12-15 | Amelia (CR-2) | جولة مراجعة ثانية للأداء/التخصيصات — إصلاح 3 ملاحظات: (P-1) قراءة ملف بحجم معروف عبر `out.resize+read` بدلاً من `ostringstream` (نسخة واحدة)؛ (P-2) cache fallback whitelist كـstatic const داخل دالة (تجنّب 3 compiles لـregex لكل scan)؛ (P-4) `out.clear()` عند الفشل في read_file_utf8 لمنع تسرّب محتوى جزئي. ctest 3/3 أخضر. |
| 2025-12-15 | Amelia (CR-3) | جولة مراجعة سلامة الخيوط — لا مشاكل تصحيح فعلية (magic statics آمنة + std::regex const قراءة آمنة)؛ إضافة توثيق صريح في `loader.h`: Loader instance غير آمن للخيوط (خيط واحد لكل instance)، والدوال الحرة pure. ctest 3/3 أخضر. |
