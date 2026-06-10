# S-013: Tier 3 Snapshots (Requires P1)

> **Phase:** 6 | **Effort:** L | **Risk:** Med (depends on P1)
> **Depends on:** S-009, S-009b, **[P1: sad --report-calls](../prerequisites/P1-sad-report-calls.md)**
> **Blocks:** S-015d
> **Arch ref:** §3.3 (Tier 3)

---

## User Story

**As** مسؤول CI/جودة الـdocs،
**I want** فحص فعلي للأمثلة في lessons (لا regex)،
**So that** ما يظهر للمستخدم النهائي يعمل فعلاً.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `Validator::validateTier3(aggregationResult)` يجري على lessons فقط |
| AC-2 | لكل example في lesson → كتابة tmp `.ص` → استدعاء `sad --report-calls` |
| AC-3 | فحص: لا nondet_calls (الآن، عشوائي، ...) إلا إذا الـlesson تسمح صراحة |
| AC-4 | إذا P1 غير متوفر → warning + regex fallback (lower accuracy) |
| AC-5 | snapshots: حفظ stdout المتوقَّع في `snapshots/{lesson_id}/{example_id}.txt` |
| AC-6 | comparison: تنفيذ المثال فعلياً + مقارنة مع snapshot |
| AC-7 | `--update-snapshots` flag لتحديث الـexpected |
| AC-8 | تكلفة عالية → يجري في CI فقط (`BUILD_DOCS_SNAPSHOTS=ON`) |
| AC-9 | parallel execution (thread pool) للأمثلة |
| AC-10 | كل example يجري داخل sandbox من S-009b: لا file I/O، لا network، لا subprocess |
| AC-11 | timeout صارم: 5s لكل example (configurable عبر `query_limits.yaml`) |
| AC-12 | tmp file يُكتب في `%TEMP%/sadinfo_tier3_<uuid>/` ويُحذف بعد التشغيل (defer) |
| AC-13 | snapshot mismatch يُظهر diff واضح (libdiff أو unified format) |

## Tasks

- [ ] T1: `validator_tier3.cpp`
- [ ] T2: example extractor (من lesson YAML)
- [ ] T3: `sad --report-calls` invoker + JSON parser
- [ ] T4: snapshot manager (read/write/compare)
- [ ] T5: parallel executor (thread pool)
- [ ] T6: regex fallback عند غياب P1
- [ ] T7: CLI: `sadinfo validate --snapshots`
- [ ] T8: CMake `BUILD_DOCS_SNAPSHOTS` option
- [ ] T9: Integration tests مع `non_det_examples/` fixture

## File List

**جديد:**
- `shared/sadinfo_core/src/validator/validator_tier3.cpp`
- `shared/sadinfo_core/src/validator/snapshot_manager.cpp`
- `shared/sadinfo_core/src/validator/sad_invoker.cpp`
- `tools/sadinfo/src/commands/validate.cpp` (تحديث لـ--snapshots)
- `tests/sadinfo/integration/tier3_tests.cpp`
- `tests/sadinfo/fixtures/non_det_examples/`
- `tests/sadinfo/snapshots/` (تُولَّد runtime)

## Definition of Done

- [ ] Tier 3 يكتشف example بـ`الآن()` ويُحذِّر
- [ ] snapshot mismatch يُوقف CI
- [ ] regex fallback لا يكسر إذا P1 غير متوفر
- [ ] perf مقبول (يجري في CI، ليس inline)

## Notes للمطوِّر

- `non_deterministic_builtins.yaml` مشترك مع P1
- snapshot format: نص خام أو JSON حسب طبيعة المثال
- parallel: thread pool بحجم `std::thread::hardware_concurrency()`
- اعرض progress bar لـcommands الطويلة
