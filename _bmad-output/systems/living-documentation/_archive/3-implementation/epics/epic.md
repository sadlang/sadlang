# Epic: sadinfo Unified Engine

> **Status:** Planning Complete | **Created:** 2026-05-15
> **Architecture:** [SADINFO_TARGET_ARCHITECTURE.md](../../docplan/sadinfo/SADINFO_TARGET_ARCHITECTURE.md)
> **YAML Contracts:** [DATA_SCHEMA_CONTRACTS.md](DATA_SCHEMA_CONTRACTS.md) (إلزامي لأي story تلمس `data/`)

---

## Vision

محرَّك معرفة موحَّد للغة ص يستهلكه LSP + Formatter + sad + sadc + Docs + AI Agents من Single Source of Truth (`data/`)، مع cache ذكي ثلاثي الطبقات، watcher، graph queries، sandbox أمني، CI/CD، logging مُهيكَل.

## Scope

| In | Out |
|----|-----|
| YAML schema موحَّد لكل entities | استبدال الـAST runtime |
| 3-tier cache (RAM/Memory-mapped/SQLite) | تغيير syntax اللغة |
| Reader API كـsingle entry للاستهلاك | UI/Web tools (epic منفصل) |
| Watcher + Merkle invalidation | تنفيذ المفسر/الكومبايلر نفسه |
| Tier 1/2/3 validation + sandbox | Cloud sync (مستقبلي) |
| Migration كاملة من v1 hardcoded | |
| Security hardening (path/size/depth) | |
| CI/CD + structured logging | |

## Success Criteria (Measurable)

| Metric | Target | Source |
|--------|--------|--------|
| LSP query (depth 5) | < 10ms p95 | benchmark suite |
| Reader lookup | < 1ms cached, < 50ms cold | benchmark |
| Watcher debounce | 10ms idle, 200ms burst | tests |
| Tier 3 example success rate | > 90% | migration report |
| Test coverage | > 80% lines, > 70% branches | Codecov |
| Performance regression | < 10% vs baseline | CI gate |
| Security vectors blocked | 12/12 (path traversal, size, depth, symlink, ...) | security suite |
| Hot-reload latency | < 500ms p95 (watcher 200ms burst + validate + aggregate + notify) | watcher tests |
| Zero data loss | atomic rename + lock | concurrency tests |

## Out-of-Scope (مؤجَّل)

- macOS support كامل (S-011b أساسي فقط، توسعة لاحقة)
- ترجمة fr/es (ar/en فقط في v2)
- Cloud sync / multi-host
- GUI inspector
- Migration لـ`وثائق/` legacy

## Epics Breakdown (E1–E11)

| ID | Epic | Stories | Phase |
|----|------|---------|-------|
| E0 | **Foundations & Schemas** | S-000a | 0 |
| E1 | Loader POC | S-001 | 1 |
| E2 | Tier 1/2 Validation | S-002, S-003 | 1 |
| E3 | State + Lock + Atomic Writes | S-007 | 2 |
| E4 | Aggregator + Merkle + RAM Cache | S-004, S-005, S-006 | 2 |
| E5 | SQLite Graph | S-008 | 3 |
| E6 | Reader API + Public Headers | S-009 | 4 |
| E7 | **Security Hardening** | S-009b | 4 |
| E8 | CLI + Watcher | S-010, S-011, S-011b | 5 |
| E9 | Tier 3 + Stats + CLI Polish | S-012, S-013, S-014 | 6 |
| E10 | **CI/CD + Documentation Gen** | S-014b | 6 |
| E11 | Migration + Legacy Removal | S-015a, S-015b, S-015c, S-015d, S-015e, S-016 | 7-8 |

## Phase Mapping (25 stories total: 24 + 1 prereq)

```
Phase 0 (Foundations):   S-000a
Phase 1 (POC):           S-001 → S-002 → S-003
Phase 2 (State+Agg):     S-007 → S-004 → S-005 → S-006
Phase 3 (SQLite):        S-008
Phase 4 (API+Security):  S-009 → S-009b
Phase 5 (CLI+Watcher):   S-010, S-011, S-011b
Phase 6 (Polish+CI):     S-012, S-013, S-014, S-014b
Phase 7 (Migration):     S-015a → S-015b → S-015c → S-015d → S-015e
Phase 8 (Cleanup):       S-016

Prerequisite: P1-sad-report-calls (يجب أن يكون جاهزاً قبل S-013)
```

## Stories Index

| ID | Story | Effort | Risk | Phase |
|----|-------|--------|------|-------|
| [S-000a](stories/S-000a-foundation-schemas.md) | Foundation Schemas + Logger | L | Low | 0 |
| [S-001](stories/S-001-loader-poc.md) | Loader POC | M | Low | 1 |
| [S-002](stories/S-002-tier1-syntactic.md) | Tier 1 Syntactic | M | Low | 1 |
| [S-003](stories/S-003-tier2-semantic.md) | Tier 2 Semantic | M | Med | 1 |
| [S-004](stories/S-004-hash-store-fastpath.md) | Hash Store + Fast-Path | M | Low | 2 |
| [S-005](stories/S-005-aggregator-merkle.md) | Aggregator + Merkle | L | Med | 2 |
| [S-006](stories/S-006-cache-ram.md) | RAM Cache | M | Low | 2 |
| [S-007](stories/S-007-state-and-lock.md) | State + Lock | M | Med | 2 |
| [S-008](stories/S-008-sqlite-graph.md) | SQLite Graph + WAL | L | Med | 3 |
| [S-009](stories/S-009-reader-api.md) | Reader API | M | Low | 4 |
| [S-009b](stories/S-009b-security-hardening.md) | **Security Hardening** | M | High | 4 |
| [S-010](stories/S-010-cli-validate-rebuild.md) | CLI validate/rebuild | M | Low | 5 |
| [S-011](stories/S-011-watcher.md) | Watcher (Win+Linux) | M | Med | 5 |
| [S-011b](stories/S-011b-watcher-macos.md) | Watcher macOS | M | Low | 5 |
| [S-012](stories/S-012-cli-query.md) | CLI query | S | Low | 6 |
| [S-013](stories/S-013-tier3-snapshots.md) | Tier 3 + Snapshots | L | Med | 6 |
| [S-014](stories/S-014-stats-logging.md) | Stats + Exit Codes | S | Low | 6 |
| [S-014b](stories/S-014b-ci-pipeline.md) | **CI/CD Pipeline** | M | Low | 6 |
| [S-015a](stories/S-015a-migrate-builtins.md) | Migrate Builtins | L | Med | 7 |
| [S-015b](stories/S-015b-migrate-keywords.md) | Migrate Keywords | L | Med | 7 |
| [S-015c](stories/S-015c-migrate-errors.md) | Migrate Errors | M | Low | 7 |
| [S-015d](stories/S-015d-migrate-lessons.md) | Migrate Lessons (MD→YAML SSoT) | XL | High | 7 |
| [S-015e](stories/S-015e-md-generator.md) | **MD Generator (YAML→MD)** | M | Low | 7 |
| [S-016](stories/S-016-legacy-removal.md) | Legacy Removal + MD untrack | M | Med | 8 |

**Prerequisite:**
- [P1-sad-report-calls](prerequisites/P1-sad-report-calls.md)
