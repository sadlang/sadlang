# S-008: SQLite Graph + WAL + Cycle Protection

> **Phase:** 3 | **Effort:** L | **Risk:** Med
> **Depends on:** S-007 | **Blocks:** S-009
> **Arch ref:** §3.4

---

## User Story

**As** Reader API و LSP،
**I want** SQLite graph بـWAL mode + recursive CTE + cycle protection،
**So that** queries غير محدودة الـdepth بسرعة + قراءات لا تحجب الكتابة.

## Acceptance Criteria

| AC | معيار |
|----|------|
| AC-1 | جدول `entities (id, category, file_path, hash, version)` |
| AC-2 | جدول `relations (source_id, target_id, kind, depth_hint)` (kind: uses/extends/replaces) |
| AC-3 | PRAGMAs صريحة عند الفتح: `journal_mode=WAL`, `synchronous=NORMAL`, `wal_autocheckpoint=1000`, `busy_timeout=5000`, `read_uncommitted=true` |
| AC-4 | recursive CTE للـtraversal مع `max_depth` parameter (افتراضي 10) |
| AC-5 | max_depth قابل للتعديل عبر `data/_schemas/query_limits.yaml` |
| AC-6 | cycle protection: visited set في CTE أو explicit cycle check |
| AC-7 | LSP query (depth 5) < 10ms |
| AC-8 | concurrent readers + 1 writer بدون deadlock |
| AC-9 | SQLite corruption recovery: عند فتح فاسد → log + rebuild من `_state.json` |
| AC-10 | Schema versioning: جدول `schema_version` + migration runner (v1 → v2 تلقائي) |
| AC-11 | query limits من S-009b: depth، result count، timeout — enforced في كل query |

## Tasks

- [ ] T1: SQLite schema (CREATE TABLE statements)
- [ ] T2: IndexBuilder class
- [ ] T3: PRAGMAs application عند connection open
- [ ] T4: write API: bulk insert من AggregationResult
- [ ] T5: read API: traverse (source_id, kind, depth) → list<EntityRef>
- [ ] T6: recursive CTE query (parameterized max_depth)
- [ ] T7: cycle detection (WITH RECURSIVE + visited tracking)
- [ ] T8: query_limits.yaml + loader
- [ ] T9: corruption detection + rebuild path
- [ ] T10: concurrency tests
- [ ] T11: performance tests

## File List

**جديد:**
- `shared/sadinfo_core/include/index_builder.h`
- `shared/sadinfo_core/include/graph_query.h`
- `shared/sadinfo_core/src/index_builder/index_builder.cpp`
- `shared/sadinfo_core/src/index_builder/graph_query.cpp`
- `shared/sadinfo_core/src/index_builder/sqlite_helpers.cpp`
- `data/_schemas/query_limits.yaml`
- `tests/sadinfo/unit/graph_query_tests.cpp`
- `tests/sadinfo/concurrency/sqlite_concurrent_tests.cpp`
- `tests/sadinfo/performance/graph_query_bench.cpp`

**معدَّل:**
- `CMakeLists.txt` (إضافة SQLite kotak)
- `cmake/dependencies.cmake` (find_package SQLite3)

## Definition of Done

- [ ] LSP query < 10ms benchmark passed
- [ ] cycle test (A→B→A) لا hang
- [ ] WAL files (`.wal`, `.shm`) تُدار صحيحاً
- [ ] documentation عن schema في `docs/sadinfo/sqlite_schema.md`

## Notes للمطوِّر

- SQLite عبر CMake `find_package(SQLite3 REQUIRED)`
- اقترح schema:
  ```sql
  CREATE TABLE entities (
    id TEXT PRIMARY KEY,
    category TEXT NOT NULL,
    file_path TEXT NOT NULL,
    hash TEXT NOT NULL,
    version INTEGER NOT NULL
  );
  CREATE INDEX idx_entities_category ON entities(category);
  
  CREATE TABLE relations (
    source_id TEXT NOT NULL,
    target_id TEXT NOT NULL,
    kind TEXT NOT NULL,
    depth_hint INTEGER DEFAULT 0,
    PRIMARY KEY (source_id, target_id, kind),
    FOREIGN KEY (source_id) REFERENCES entities(id),
    FOREIGN KEY (target_id) REFERENCES entities(id)
  );
  CREATE INDEX idx_relations_source ON relations(source_id);
  CREATE INDEX idx_relations_target ON relations(target_id);
  ```
- recursive CTE example:
  ```sql
  WITH RECURSIVE traversal(id, depth, path) AS (
    SELECT target_id, 1, source_id || '->' || target_id
      FROM relations WHERE source_id = ? AND kind = ?
    UNION ALL
    SELECT r.target_id, t.depth + 1, t.path || '->' || r.target_id
      FROM relations r JOIN traversal t ON r.source_id = t.id
      WHERE t.depth < ?
        AND instr(t.path, r.target_id) = 0  -- cycle protection
  )
  SELECT DISTINCT id FROM traversal;
  ```
