# S-000a: Foundation Schemas + Logging + Baseline

> **Phase:** 0 (Pre-POC) | **Effort:** L | **Risk:** Low
> **Depends on:** None | **Blocks:** S-001, S-007, S-014, S-009, S-009b, S-013, **كل stories S-015\***
> **Arch ref:** [DATA_SCHEMA_CONTRACTS.md](../DATA_SCHEMA_CONTRACTS.md) + [YAML_UNIFIED_SCHEMA_DESIGN.md](../../../docplan/YAML_UNIFIED_SCHEMA_DESIGN.md) v1.0 + [SADINFO_TARGET_ARCHITECTURE.md](../../../docplan/sadinfo/SADINFO_TARGET_ARCHITECTURE.md) §3.1, §4, §8

---

## User Story

**As** فريق sadinfo v2،
**I want** كل الـschemas التأسيسية + إطار logging موحَّد + baseline أداء فارغ جاهزة قبل أي كود،
**So that** كل story لاحقة تجد contracts ثابتة لا تتغيَّر.

## Acceptance Criteria

### أ. Schemas الكيانات (JSON Schema draft-07 — وفق §4 في contracts)

| AC | معيار |
|----|------|
| AC-S1 | `data/_schemas/builtin.schema.json` (additionalProperties: false) |
| AC-S2 | `data/_schemas/keyword.schema.json` (reserved + contextual) |
| AC-S3 | `data/_schemas/error.schema.json` (code pattern `^E[0-9]{4}$`) |
| AC-S4 | `data/_schemas/lesson.schema.json` |
| AC-S5 | `data/_schemas/example.schema.json` (deterministic XOR expected_pattern+allow_nondet) |
| AC-S6 | `data/_schemas/exercise.schema.json` |
| AC-S7 | `data/_schemas/docs.schema.json` (summary required) |
| AC-S8 | `data/_schemas/i18n_overlay.schema.json` |

### ب. سياسات و meta

| AC | معيار |
|----|------|
| AC-P1 | `data/_schemas/VERSION.yaml`: default_language=ar, supported_languages=[ar,en,fr], fallback_strategy=show_default_with_marker |
| AC-P2 | `data/_schemas/i18n_policy.yaml`: required/optional/never_translate وفق §8 contracts |
| AC-P3 | `data/_schemas/whitelist.yaml`: فقط `{_index.yaml, docs.yaml, examples/**, exercises/**, i18n/**}` |
| AC-P4 | `data/_schemas/naming_rules.yaml`: إلزام `<folder> == <kind>_<id.last_segment>` |
| AC-P5 | `data/_schemas/query_limits.yaml` (max depth, max nodes) |
| AC-P6a | `data/_schemas/non_deterministic_builtins.yaml`: `الآن`, `عشوائي`, `وقت`, `معرِّف_فريد` (قيمة تختلف بين النداءات) |
| AC-P6b | `data/_schemas/side_effect_builtins.yaml`: `بيئة`, `اقرأ`, `اكتب_ملف` (يقرأ/يكتب حالة خارجية — يُعالَج بشكل منفصل في Tier 3) |
| AC-P7 | `data/_schemas/state_schema.json` (lock format, watcher PID, schema_version) |
| AC-P8 | `data/_meta/CODEOWNERS` YAML بصيغة: `{ teams: { core-team: [@user1, @user2], docs-team: [@user3] }, defaults: { builtins: core-team, keywords: core-team, errors: core-team, lessons: docs-team } }` |
| AC-P9 | S-009 Reader API يفرض `owners` ⊆ teams members في CODEOWNERS (loader fail-fast) |

### ج. Logger + Baseline

| AC | معيار |
|----|------|
| AC-L1 | `shared/sadinfo_core/include/logging.h` + `logging.cpp` (JSON structured) |
| AC-L2 | كل log entry: `{ts: ISO8601, level, component, msg, data}` |
| AC-L3 | `--log-level` (trace/debug/info/warn/error) + env `SADINFO_LOG_LEVEL` |
| AC-L4 | `tests/sadinfo/performance/baseline.json` بقيم `null` لكل scenario |
| AC-L5 | `scripts/measure_baseline.ps1` يحدِّث القيم محلياً |
| AC-L6 | `scripts/validate_schemas.py` يفحص 8 schemas + 4 fixtures بنجاح |
| AC-L7 | كل schema موثَّق بـ`README.md` في `_schemas/` |

## Tasks

- [x] T1: 8 ملفات `.schema.json` (builtin/keyword/error/lesson/example/exercise/docs/i18n_overlay)
- [x] T2: VERSION + i18n_policy + whitelist + naming_rules + query_limits + non_deterministic_builtins + side_effect_builtins + state_schema
- [x] T3: `_meta/CODEOWNERS` الأولي بصيغة YAML (teams + defaults) + JSON Schema له
- [x] T4: 4 fixtures صغيرة تحت `tests/sadinfo/fixtures/data/` (واحد لكل kind)
- [x] T5: implement logging (JSON line)
- [x] T6: env var parsing للـlog level
- [x] T7: baseline.json بـscenarios من test-strategy
- [x] T8: measure_baseline.ps1
- [x] T9: README لـ_schemas + أمثلة
- [x] T10: Unit tests للـlogger (10k msg/s)
- [x] T11: `scripts/validate_schemas.py` (jsonschema CLI)

## File List

**جديد — 8 schemas:**
- `data/_schemas/builtin.schema.json`
- `data/_schemas/keyword.schema.json`
- `data/_schemas/error.schema.json`
- `data/_schemas/lesson.schema.json`
- `data/_schemas/example.schema.json`
- `data/_schemas/exercise.schema.json`
- `data/_schemas/docs.schema.json`
- `data/_schemas/i18n_overlay.schema.json`

**جديد — سياسات + meta:**
- `data/_schemas/VERSION.yaml`
- `data/_schemas/i18n_policy.yaml`
- `data/_schemas/whitelist.yaml`
- `data/_schemas/naming_rules.yaml`
- `data/_schemas/query_limits.yaml`
- `data/_schemas/non_deterministic_builtins.yaml`
- `data/_schemas/side_effect_builtins.yaml`
- `data/_schemas/state_schema.json`
- `data/_schemas/codeowners.schema.json`
- `data/_schemas/README.md`
- `data/_meta/CODEOWNERS` (YAML)

**جديد — fixtures + tooling:**
- `tests/sadinfo/fixtures/data/builtins/builtin_demo/_index.yaml`
- `tests/sadinfo/fixtures/data/builtins/builtin_demo/docs.yaml`
- `tests/sadinfo/fixtures/data/keywords/keyword_demo/_index.yaml`
- `tests/sadinfo/fixtures/data/errors/error_demo/_index.yaml`
- `tests/sadinfo/fixtures/data/errors/error_demo/docs.yaml`
- `tests/sadinfo/fixtures/data/lessons/lesson_demo/_index.yaml`
- `tests/sadinfo/fixtures/data/lessons/lesson_demo/docs.yaml`
- `shared/sadinfo_core/include/logging.h`
- `shared/sadinfo_core/src/logging.cpp`
- `tests/sadinfo/performance/baseline.json`
- `scripts/measure_baseline.ps1`
- `scripts/validate_schemas.py`
- `tests/sadinfo/unit/logger_tests.cpp`
- `tests/sadinfo/unit/schema_validation_tests.cpp`

## Definition of Done

- [x] 8 schemas + 7 fixtures تجتاز `validate_schemas.py` (0 أخطاء، 0 تحذيرات)
- [ ] whitelist يرفض أي ملف خارج الـwhitelist (test سلبي — يأتي مع S-007 validator C++)
- [ ] naming_rules يرفض `print_line/` بدون `builtin_` prefix (test سلبي — مع S-007)
- [x] logger يجتاز test (no crash @ 10k msg/s — يحتاج CMake wiring لتشغيله فعلياً)
- [x] baseline.json valid JSON
- [x] CODEOWNERS يغطِّي 4 جذور (builtins/keywords/errors/lessons) ويُحقَّق ضد `codeowners.schema.json`
- [x] non_det + side_effect مفصولان (لا خلط بين عدم الحتمية والآثار الجانبية)

## Notes للمطوِّر

- **اقرأ أولاً:** [DATA_SCHEMA_CONTRACTS.md](../DATA_SCHEMA_CONTRACTS.md) — المرجع العملي الأساسي
- **سياسة الاسم الواحد:** لا aliases ولا name_en — كل schemas تستخدم `additionalProperties: false`
- **Logger:** لا تستخدم std::cout/cerr لاحقاً — كل شيء يمر بـ`Logger::info(component, msg, data)`
- baseline.json schema:
  ```json
  {
    "version": 1,
    "scenarios": {
      "aggregate_full_500": { "budget_ms": 500, "measured_ms": null },
      "aggregate_incremental_1": { "budget_ms": 50, "measured_ms": null }
    }
  }
  ```

---

## Status

**Done** — مغلقة بعد 7 جولات Code Review (CR-1 → CR-7). آخر القياسات: Memory throughput 132,656 msg/sec، File throughput 59,528 msg/sec (متوسط 5 جولات). كل من `SadInfoLogger` و`SadInfoSchemas` يمران (2/2 على `ctest -L sadinfo_v2`). 19/19 من روابط الوثائق الداخلية صحيحة.

## Dev Agent Record

### وكيل التنفيذ
- **النموذج:** Claude (Copilot)
- **المهارة:** bmad-agent-dev (تنفيذ متسلسل T1→T11)
- **تاريخ التنفيذ:** 2025-11

### القرارات التقنية

1. **schemas مبنية مسبقاً (T1):** اكتُشف أن الـ8 schemas كانت موجودة من جلسة سابقة مع حقل `since` إلزامي و`version_info` اختياري لـbuiltin فقط (لا keyword/error/lesson). الـfixtures التي كُتبت بدايةً بحسب ملخص قديم احتوت على `version_info` خطأً وأُعيد تصحيحها.
2. **Logger خفيف بدون JSON خارجي (T5):** نُفِّذ JSON serialization يدوياً (escape + variant) لتجنّب اعتماد nlohmann أو RapidJSON في طبقة الـcore — يبقي `shared/sadinfo_core/` نظيفاً.
3. **Thread-safety (T5):** mutex حول الكتابة فقط، البناء خارج القفل لتقليل التنازع. اختبار `test_thread_safety` يتحقق من عدم تداخل الأسطر بين 4 خيوط × 1000 رسالة.
4. **Validator C++ بسيط (T10):** الـschema_validation_tests.cpp يفحص توازن JSON + وجود `additionalProperties:false` + `schema_version` نصياً فقط. التحقق الدلالي الكامل في `validate_schemas.py` (Python + jsonschema) — تجنّب إضافة C++ JSON Schema validator في هذه القصة.
5. **CMake wiring مؤجَّل:** لم تُربط الاختبارات بـCMake بعد (BUILD_TESTS=OFF افتراضياً). الاختبارات قائمة بذاتها (`main()` + exit codes). الربط يأتي مع story لاحقة عند إضافة sadinfo target.

### نتائج الفحص

```
python scripts/validate_schemas.py
  → 8/8 schemas موجودة
  → جميع schemas صحيحة كـdraft-07
  → جميع 7 fixtures مرّت
  → جميع 9 ملفات سياسة موجودة وصالحة
  → الملخص: 0 أخطاء، 0 تحذيرات (exit code 0)
```

### نقاط متبقية (للقصص اللاحقة)

- **S-007:** اختبارات whitelist/naming السلبية (C++ validator)
- **بناء/تكامل CMake:** ربط logger_tests و schema_validation_tests بـCTest
- **قياس throughput فعلي:** تشغيل `logger_tests` بعد ربط CMake ثم تحديث baseline.json عبر `scripts/measure_baseline.ps1 -Scenario logger_throughput -Value <measured>`

### جولة المراجعة والتحسين (Amelia — CR)

تم تطبيق مراجعة شاملة على الكود المكتوب وإصلاح العيوب التالية:

| # | الملف | العيب | الإصلاح |
|---|------|------|--------|
| 1 | [logging.h](shared/sadinfo_core/include/logging.h) + [logging.cpp](shared/sadinfo_core/src/logging.cpp) | `level_` كان `Level` عادي → data race بين `set_level()` و `log()` في خيوط متوازية | تحويله إلى `std::atomic<Level>` مع `memory_order_relaxed` للقراءة/الكتابة |
| 2 | [logging.cpp](shared/sadinfo_core/src/logging.cpp) | constructor يتجاهل `SADINFO_LOG_LEVEL=off` (شرط `if (env != OFF)`) | حذف الشرط — دائماً يُحترم env بما فيه OFF |
| 3 | [logging.cpp](shared/sadinfo_core/src/logging.cpp) | `log()` يقرأ `level_` مرتين بدون قفل (TOCTOU) | لقطة atomic واحدة `const Level cur = level_.load()` |
| 4 | [logger_tests.cpp](tests/sadinfo/unit/logger_tests.cpp) | `SinkGuard` لا يحفظ المستوى الأصلي → اختبارات تتأثر ببعضها | حفظ `prev_level` في constructor واستعادته في destructor |
| 5 | [logger_tests.cpp](tests/sadinfo/unit/logger_tests.cpp) | غياب اختبار للـrace بين `set_level` و `log` | إضافة `test_concurrent_level_change` (smoke test، يلتقط race مع tsan) |
| 6 | [schema_validation_tests.cpp](tests/sadinfo/unit/schema_validation_tests.cpp) | `find_repo_root` يصعد 6 مستويات فقط ولا يدعم تغيير cwd | دعم `SAD_REPO_ROOT` env + رفع الحد إلى 8 مستويات + حماية من جذر بدون parent |
| 7 | [validate_schemas.py](scripts/validate_schemas.py) | لا يتحقق من CODEOWNERS ضد codeowners.schema.json | إضافة قسم 5 يفحص CODEOWNERS كـinstance ضد الـschema |
| 8 | [measure_baseline.ps1](scripts/measure_baseline.ps1) | `description` إجباري في `-List` (يكسر إن غاب) + خطأ إملائي `بnchmarks` | جعل `description` اختياري + تصحيح الإملاء |

### نتيجة المراجعة

```
python scripts/validate_schemas.py
  → 8/8 schemas + 7/7 fixtures + 9/9 policies + CODEOWNERS صالح
  → 0 أخطاء، 0 تحذيرات (exit 0)
```

Static analysis (`get_errors`) على الملفات الأربعة المعدّلة: 0 أخطاء.

### جولة ربط CMake وتشغيل فعلي (Amelia — CW)

تم ربط مكتبة `sadinfo_core` و اختباراتها بنظام البناء:

| الملف | الإضافة |
|------|---------|
| [shared/sadinfo_core/CMakeLists.txt](shared/sadinfo_core/CMakeLists.txt) | **جديد** — مكتبة STATIC: `logging.cpp` + Threads + `/utf-8 /W4` |
| [shared/CMakeLists.txt](shared/CMakeLists.txt) | `add_subdirectory(sadinfo_core)` محمي بـ`EXISTS` |
| [tests/sadinfo/CMakeLists.txt](tests/sadinfo/CMakeLists.txt) | هدفان جديدان: `sadinfo_logger_tests` و `sadinfo_schema_tests`؛ `SAD_REPO_ROOT` في ENVIRONMENT |
| [tests/sadinfo/CMakeLists.txt](tests/sadinfo/CMakeLists.txt) | حارس على `test_sadinfo_keyword_transforms` (مصدر مفقود مسبقاً) |

**نتيجة التشغيل الفعلي:**

```
cmake -S . -B build -DBUILD_TESTS=ON
  → sadinfo_core library configured
  → اختبارات sadinfo v2 (Logger + Schemas) — S-000a

cmake --build build --config Debug --target sadinfo_logger_tests sadinfo_schema_tests
  → BUILD_EXIT=0

ctest -R "SadInfoLogger|SadInfoSchemas" --output-on-failure
  → Test #137: SadInfoLogger ........ Passed    0.26 sec
  → Test #138: SadInfoSchemas ....... Passed    0.05 sec
  → 100% tests passed, 0 failed (TEST_EXIT=0)
```

**يشمل ذلك تشغيل:** `test_throughput_10k` (≥ 10k msg/s)، `test_thread_safety`، `test_concurrent_level_change` (smoke للـrace)، 5 اختبارات schema.

### جولة تقوية الاختبارات (Amelia — CR-2)

بعد ربط CMake اكتشفت مراجعة ثانية ضعفاً في 4 اختبارات وأُصلح:

| # | الموقع | العيب | الإصلاح | قاعدة |
|---|--------|------|--------|--------|
| 1 | `test_thread_safety` | يفحص عدد الأسطر فقط (`lines == THREADS*PER`)، لا يكشف تداخل البايتات تحت قفل ضعيف | فحص integrity صارم: كل سطر يبدأ بـ`{` وينتهي بـ`}` + `"tid":` يظهر **مرة واحدة بالضبط** في كل سطر | BF-22 |
| 2 | `test_concurrent_level_change` | كان `CHECK(true, ...)` — لا يفشل أبداً → عديم الفائدة بدون tsan | يفحص الآن أن كل سطر صدر متوازن (`{...}`) — التداخل سيكسر هذا قابل للملاحظة في CI عادي | BF-09 |
| 3 | `logger_tests.cpp` | أرقام سحرية متناثرة (`20000`, `4`, `1000`, `5000`, `10000.0`) | `constexpr int kThroughputN/kSafetyThreads/kSafetyPerThread/kRaceLogs` + `constexpr double kMinRatePerSec` في مكان واحد | CW-10 |
| 4 | `logger_tests.cpp` | `<string_view>` يُستخدم عبر transitive include | إضافة `#include <string_view>` صراحة | CW-06 |

**القياس الفعلي بعد التقوية:**

```
.\build\bin\Debug\sadinfo_logger_tests.exe
  → throughput: 153,997 msg/sec (×15 من الحد الأدنى 10k)
  → جميع الاختبارات مرّت ✓
```

```
ctest -R "SadInfoLogger|SadInfoSchemas"
  → Test #137: SadInfoLogger ........ Passed    0.24 sec
  → Test #138: SadInfoSchemas ....... Passed    0.02 sec
  → 100% passed (2/2)
```

### جولة المراجعة الثالثة (Amelia — CR-3)

مراجعة على ما كُتب في CR-2. اكتُشف 3 عيوب جوهرية:

| # | الموقع | العيب | الإصلاح | قاعدة |
|---|--------|------|--------|--------|
| 1 | [logging.cpp](shared/sadinfo_core/src/logging.cpp) `log()` | لا `flush()` بعد `write()` — عند crash يُفقد آخر سطر تشخيصي (مهم في الإنتاج مع `ofstream`/buffered cerr) | إضافة `sink_->flush()` تحت نفس القفل | BF-22 |
| 2 | [logger_tests.cpp](tests/sadinfo/unit/logger_tests.cpp) `test_concurrent_level_change` | `out[nl-1]` بدون فحص `nl == pos` → UB عند سطر فارغ بين `\n\n` (تداخل) — والأسوأ: قد يقرأ من سطر سابق فيمر التداخل خطأ | فحص صريح `if (nl == pos) ok=false` (السطر الفارغ = إشارة تداخل) | BF-06 |
| 3 | [logger_tests.cpp](tests/sadinfo/unit/logger_tests.cpp) `test_concurrent_level_change` | لا تحقق من أن INFO فعلاً صدر — لو الـtoggler ثبّت WARN بالصدفة طوال الوقت لمرّ الاختبار خطأً بـ`out.empty()` | عداد `inspected > 0` يتأكد أن السطور فُحصت فعلاً | BF-22 |

**أثر القياس بعد إضافة `flush()` لكل سطر:**

| المقياس | قبل CR-3 | بعد CR-3 |
|---------|---------|----------|
| Throughput (Debug) | 153,997 msg/sec | **121,924 msg/sec** (×12 من الحد الأدنى) |
| Test #137 | 0.24 s | 0.24 s |
| Test #138 | 0.02 s | 0.02 s |
| اختبارات ناجحة | 2/2 | **2/2** |

كلفة الـflush ~21% throughput مقابل ضمان عدم فقدان سطر تشخيصي عند crash — مقايضة سليمة لـlogger.

### جولة المراجعة الرابعة (Amelia — CR-4)

تركيز على نقاط القابلية للاختبار (CW-23) والفحوصات السطحية (BF-22):

| # | الموقع | العيب | الإصلاح | قاعدة |
|---|--------|------|--------|--------|
| 1 | [logging.h](shared/sadinfo_core/include/logging.h) + [logging.cpp](shared/sadinfo_core/src/logging.cpp) | لا API لإعادة قراءة `SADINFO_LOG_LEVEL` بعد إنشاء singleton — اختبار سلوك env غير ممكن | إضافة `Logger::reload_level_from_env()` عام | CW-23 |
| 2 | [schema_validation_tests.cpp](tests/sadinfo/unit/schema_validation_tests.cpp) `test_codeowners_meta_present` | يفحص الوجود فقط — ملف فاسد/فارغ يمر | فحص `!empty()` + وجود `schema_version` + إحدى المفاتيح المتوقعة (`teams`/`owners`/`entries`/`paths`) | BF-22 |
| 3 | [logger_tests.cpp](tests/sadinfo/unit/logger_tests.cpp) | لا اختبار لمسار env (ERROR/OFF/غياب) | إضافة `test_env_reload` — يضبط env عبر `_putenv_s`/`setenv` ثم يفحص أن `reload` يطبّق ERROR ثم OFF ثم يرجع لـINFO عند المسح | BF-22 |

**القياس بعد CR-4:**

| المقياس | قبل CR-4 | بعد CR-4 |
|---------|---------|----------|
| عدد اختبارات Logger | 9 | **10** |
| Throughput | 121,924 msg/sec | **122,603 msg/sec** |
| Test #137 SadInfoLogger | Passed | **Passed** |
| Test #138 SadInfoSchemas | Passed | **Passed** |
| CTest pass rate | 2/2 | **2/2** |

### جولة المراجعة الخامسة (Amelia — CR-5)

تركيز على CMake (CI hygiene) ووضوح الإبلاغ عند الفشل:

| # | الموقع | العيب | الإصلاح | قاعدة |
|---|--------|------|--------|--------|
| 1 | [tests/sadinfo/CMakeLists.txt](tests/sadinfo/CMakeLists.txt) `SadInfoLogger` | لا عزل من بيئة CI — لو `SADINFO_LOG_LEVEL=off` خارجياً، singleton يبدأ بـOFF | `set_tests_properties(... ENVIRONMENT "SADINFO_LOG_LEVEL=")` | BF-22 |
| 2 | [tests/sadinfo/CMakeLists.txt](tests/sadinfo/CMakeLists.txt) كلا الاختبارين | لا `TIMEOUT` ولا `LABELS` — لا يمكن `ctest -L sadinfo_v2` ولا حماية من تعليق | `TIMEOUT 60/30` + `LABELS "sadinfo;sadinfo_v2;unit"` | CW-23 |
| 3 | [schema_validation_tests.cpp](tests/sadinfo/unit/schema_validation_tests.cpp) `main()` | `find_repo_root` يفشل بصمت ويُرجع cwd — رسائل خطأ غامضة "file not found" | فحص صريح + رسالة `[FATAL]` + `return 2` + طباعة `repo root` | CW-22 |

**القياس بعد CR-5:**

| المقياس | قبل CR-5 | بعد CR-5 |
|---------|---------|----------|
| Throughput (Debug) | 122,603 msg/sec | **157,574 msg/sec** (×15+) |
| `ctest -L sadinfo_v2` | غير معرّف | **2/2 Passed** |
| رسالة عند فشل repo root | "file not found" | `[FATAL] ... اضبط SAD_REPO_ROOT` |

ارتفاع throughput سببه استقرار بيئة الاختبار (ENVIRONMENT منظف).

### جولة المراجعة السادسة (Amelia — CR-6) — مراجعة الوثائق

تركيز على دقة الوثائق المُنشأة في CR-5 ([README.md](docs/sadinfo_v2/README.md) و [BASELINE.md](docs/sadinfo_v2/BASELINE.md)):

| # | الموقع | العيب | الإصلاح | قاعدة |
|---|--------|------|--------|--------|
| 1 | [README.md](docs/sadinfo_v2/README.md) §6 | المسار `data/_index/builtins/` غير موجود — الموقع الحقيقي `data/language/` | تصحيح المسار + استخدام `python scripts/validate_schemas.py` بدون وسيط | BF-04 |
| 2 | [README.md](docs/sadinfo_v2/README.md) §4 | المثال JSON `"ts":"2025-..."` مضلّل — لا يوضّح صيغة ISO8601 microseconds | استبدال بمثال حقيقي `2025-11-22T12:34:56.789012Z` + تعليق وصفي | BF-22 |
| 3 | [README.md](docs/sadinfo_v2/README.md) §2 | `tests/sadinfo/fixtures/` غير دقيق — البنية الفعلية `fixtures/data/` فقط | تصحيح المسار للإشارة لـ`fixtures/data/` | BF-04 |
| 4 | [BASELINE.md](docs/sadinfo_v2/BASELINE.md) | الادعاء "157,574 msg/sec" قياس فردي غير موثّق — ادعاء غير مدعوم إحصائياً | تشغيل 5 مرات → متوسط **135,282** msg/sec (نطاق 122k–145k) — قياس صادق | BF-22 |
| 5 | [BASELINE.md](docs/sadinfo_v2/BASELINE.md) | الادعاء "تنظيف CI قلّل ضوضاء القياس" غير مبرر — لم يُختبر سببياً | تصحيح: لا تغيير في `logging.cpp` في CR-5؛ التباين طبيعي لـDebug | BF-04 |
| 6 | [README.md](docs/sadinfo_v2/README.md) §5 | يدّعي 157,574 msg/sec — لا يطابق القياس الإحصائي | تحديث للقيمة الصادقة 135,282 (متوسط 5) | BF-22 |

**القياس الإحصائي (5 تشغيلات):**

| المقياس | القيمة |
|---|---|
| Run 1 | 122,127 msg/sec |
| Run 2 | 135,440 msg/sec |
| Run 3 | 142,452 msg/sec |
| Run 4 | 145,492 msg/sec |
| Run 5 | 130,903 msg/sec |
| **Min** | 122,127 |
| **Median** | 135,440 |
| **Mean** | **135,282** |
| **Max** | 145,492 |

**التحقق من الروابط:** 19/19 رابط داخلي سليم في [README.md](docs/sadinfo_v2/README.md). CTest 2/2 Passed.

**درس مستفاد:** التوثيق نفسه يخضع لقواعد BF — لا أرقام بلا قياس، لا ادعاءات سببية بلا اختبار. الصدق الإحصائي > الأرقام البرّاقة.

### جولة المراجعة السابعة (Amelia — CR-7) — مراجعة الكود + الاختبارات

عودة للكود بعد جولات الوثائق. تركيز على **العيوب الحقيقية** في [logging.cpp](shared/sadinfo_core/src/logging.cpp) و [logger_tests.cpp](tests/sadinfo/unit/logger_tests.cpp):

| # | الموقع | العيب | الإصلاح | قاعدة |
|---|--------|------|--------|--------|
| 1 | [logger_tests.cpp `test_throughput_10k`](tests/sadinfo/unit/logger_tests.cpp) | **قياس مضلِّل:** throughput يُقاس على `stringstream` (في الذاكرة) — لا يعكس تكلفة `flush()` الحقيقية على stderr/ملف | إضافة قياس مزدوج: ذاكرة + ofstream حقيقي. الحد الأدنى يُطبَّق على الأبطأ | BF-22 |
| 2 | [logger_tests.cpp `test_concurrent_level_change`](tests/sadinfo/unit/logger_tests.cpp) | اختبار غير حتمي: toggle بين INFO/WARN قد يجعل كل الرسائل مرفوضة → `inspected > 0` flaky | toggle بين INFO/TRACE (كلاهما يسمح بـINFO) → حتمي تماماً | BF-22, CW-23 |
| 3 | [logger_tests.cpp `test_env_reload`](tests/sadinfo/unit/logger_tests.cpp) | يلوّث env العام بدون استعادة مضمونة عند فشل CHECK → يؤثر على الاختبارات اللاحقة | `EnvGuard` RAII يستعيد القيمة الأصلية في destructor | CW-23, BF-12 |
| 4 | [logging.h](shared/sadinfo_core/include/logging.h) `sink_` | غير مهيَّأ في declaration — يعتمد كلياً على ctor. أي ctor مستقبلي بلا تهيئة = UB | `std::ostream* sink_ = nullptr;` + ctor يضبط على `&std::cerr` | CW-15 |

**الاكتشاف الحاسم (CR-7):** القياس الصادق على ملف حقيقي = **59,528 msg/sec** (نصف القياس على الذاكرة 132,656). تكلفة `flush()` بعد كل log() تستهلك ~55٪ من الزمن — لكن هذا مقبول لـlogger تشخيصي حيث الأمان عند crash أهم من السرعة القصوى.

**الإحصائيات الجديدة (5 تشغيلات):**

| المقياس | Memory (stringstream) | File (ofstream + flush) |
|---|---|---|
| Run 1 | 135,341 | 65,351 |
| Run 2 | 141,578 | 59,411 |
| Run 3 | 132,845 | 59,596 |
| Run 4 | 115,074 | 60,675 |
| Run 5 | 138,441 | 52,609 |
| **Mean** | **132,656** | **59,528** |
| **Range** | 115k–142k | 53k–65k |

**التحقق:** Build=0، CTest 2/2 Passed (sadinfo_v2 label). جميع اختبارات الإصدار السابق تبقى ناجحة (no regression).

**درس مستفاد:** اختبار الأداء يجب أن يقيس **المسار الحقيقي للإنتاج**، لا المسار الأسرع. القياس على in-memory sink أعطى وهم 135k، بينما الحقيقة 60k. القاعدة الذهبية: **قِس ما يحدث في الإنتاج، ليس ما هو أسهل قياساً**.

### تحديث DoD

- [x] جميع اختبارات S-000a تعمل عبر CTest وتمر (2/2)
- [x] CMake wiring مكتمل لـ`sadinfo_core` + اختباري الـunit
- [ ] قياس throughput فعلي على Release وتحديث `baseline.json` (S-002 / لاحقاً)
