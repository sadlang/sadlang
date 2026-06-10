# S-005: Aggregator + Merkle Root + Deletion Detection

> **Phase:** 2 | **Effort:** L | **Risk:** Med
> **Depends on:** S-003, S-004, S-007 | **Blocks:** S-006
> **Arch ref:** §3.2

---

## User Story

**As** pipeline orchestrator،
**I want** aggregator يبني Merkle tree من كل entities، يكتشف additions/modifications/deletions تزايدياً،
**So that** incremental updates سريعة وموثوقة.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | `Aggregator::aggregateFull(entities) → AggregationResult` يحسب Merkle root |
| AC-2 | `Aggregator::aggregateIncremental(prev_state, changed_files)` يُحدِّث Merkle بدون إعادة بناء كامل |
| AC-3 | كل entity = leaf، كل category = inner node، root = global |
| AC-4 | Deletion detection: مقارنة entity_ids الحالية مع الـsnapshot السابق |
| AC-5 | deletions تُسجَّل في `_state.json` تحت `deleted: [...]` |
| AC-6 | كل deletion تُكتب تلقائياً في `_meta/aliases.yaml` كـ`auto_deleted_at: <timestamp>` (لا تُحذف فعلياً، فقط تُؤرشَف) |
| AC-7 | full aggregate < 500ms لـ500 entity |
| AC-8 | incremental aggregate < 50ms لـ1 entity |
| AC-9 | كل cache writes تستخدم `CacheLock` من S-007 (single writer enforced) |
| AC-10 | كل عملية aggregate تُسجَّل بـlogger (`component="aggregator"`, action, duration_ms) |

## Tasks

- [ ] T1: تعريف `AggregationResult { merkle_root, entities_map, deletions }`
- [ ] T2: implement `aggregateFull` (bottom-up Merkle)
- [ ] T3: implement `aggregateIncremental` (path-based Merkle update)
- [ ] T4: deletion detection (set difference)
- [ ] T5: integration مع `_state.json` reader/writer (placeholder حتى S-007)
- [ ] T6: Unit tests: full + incremental + deletion
- [ ] T7: Integration test: deletion_scenario fixture
- [ ] T8: Performance test مع 500 entity

## File List

**جديد:**
- `shared/sadinfo_core/include/aggregator.h`
- `shared/sadinfo_core/include/aggregation_result.h`
- `shared/sadinfo_core/src/aggregator/aggregator.cpp`
- `shared/sadinfo_core/src/aggregator/merkle.cpp`
- `tests/sadinfo/unit/aggregator_tests.cpp`
- `tests/sadinfo/integration/deletion_tests.cpp`
- `tests/sadinfo/fixtures/deletion_scenario/`

## Definition of Done

- [ ] Perf budgets مُحقَّقة + مسجَّلة في baseline.json
- [ ] deletion scenario test يمر
- [ ] لا memory leak

## Notes للمطوِّر

- Merkle hash لكل leaf = `fullHash(entity files concatenated)`
- inner node hash = `SHA256(child1 || child2 || ...)`
- incremental: حدَّث فقط الـpath من leaf للـroot (log n)
- deletion: لا تحذف من `entities/` مباشرة — اتركها لـS-016
