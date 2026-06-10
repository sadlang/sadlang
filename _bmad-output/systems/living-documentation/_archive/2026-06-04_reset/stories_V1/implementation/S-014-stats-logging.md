# S-014: Stats Dashboard + Exit Codes

> **Phase:** 6 | **Effort:** S | **Risk:** Low
> **Depends on:** S-009, S-000a (Logger) | **Blocks:** S-014b
> **Arch ref:** §3.4, §5
>
> **ملاحظة:** Logging foundation موجودة في S-000a. هذه الـstory تركِّز فقط على stats CLI + exit codes.

---

## User Story

**As** مطوِّر/CI، 
**I want** stats عن cache + structured logging + exit codes واضحة،
**So that** الـtroubleshooting سهل والـautomation موثوقة.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `sadinfo stats` يعرض: entity count لكل category، cache size، last update، Merkle root، invalidation count |
| AC-2 | `--format=json` لـmachine-readable |
| AC-3 | Exit codes table موجودة في `tools/sadinfo/include/exit_codes.h` |
| AC-4 | كل error path يستخدم enum من exit_codes (لا magic numbers) |
| AC-5 | stats command يستهلك Reader API فقط (لا direct SQLite/yaml) |

## Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | Generic error |
| 2 | Invalid args |
| 3 | Lock acquisition failed |
| 4 | Cache corrupted |
| 5 | Validation tier failed |
| 6 | Snapshot mismatch |
| 10 | Watcher init failed |
| 70 | Internal bug (assertion failed) — نتجنَّب 64 لأن `EX_USAGE` يستخدمه |
| 75 | Security violation (path traversal, oversized file) |

## Tasks

- [ ] T1: `tools/sadinfo/include/exit_codes.h` (enum)
- [ ] T2: `tools/sadinfo/src/commands/stats.cpp`
- [ ] T3: CLI arg parsing لـ`--format`
- [ ] T4: convert all error paths لاستخدام exit codes
- [ ] T5: Integration test: exit code coverage (كل exit code له test)

## File List

**جديد:**
- `tools/sadinfo/include/exit_codes.h`
- `tools/sadinfo/src/commands/stats.cpp`
- `tests/sadinfo/e2e/exit_codes_test.py`
- `tests/sadinfo/e2e/stats_command_test.py`

## Definition of Done

- [ ] كل exit code له test يحقِّق أنه يُرجَع في الـscenario الصحيح
- [ ] structured logging schema موثَّق
- [ ] `stats` command يعمل + JSON output

## Notes للمطوِّر

- JSON logger: استخدم nlohmann/json أو write يدوياً (بسيط)
- لا تكتب logs مباشرة بـstd::cout/cerr — wrap في logger
- exit codes تصبح ABI — لا تُعدِّل لاحقاً، فقط أضف
