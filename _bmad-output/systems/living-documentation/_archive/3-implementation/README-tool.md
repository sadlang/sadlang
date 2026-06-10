# sadinfo v2 — Planning Artifacts

> **Total:** 25 markdown files (1 prereq + epic + sprint-plan + test-strategy + README + 24 stories)
> **Status:** Planning Complete (Critique merged + SSoT model finalized) | **Created:** 2026-05-15

---

## التركيب

```
sadinfo/
├── README.md                    ← أنت هنا
├── epic.md                      ← E0–E11 epics + measurable success criteria
├── sprint-plan.md               ← 8 sprints + dependency graph (Mermaid)
├── test-strategy.md             ← اختبارات + CI gates + coverage targets
├── prerequisites/
│   └── P1-sad-report-calls.md   ← يجب قبل S-013
└── stories/                     ← 23 stories
    ├── S-000a-foundation-schemas.md       (Phase 0 — يجب أولاً)
    ├── S-001-loader-poc.md
    ├── S-002-tier1-syntactic.md
    ├── S-003-tier2-semantic.md
    ├── S-004-hash-store-fastpath.md
    ├── S-005-aggregator-merkle.md
    ├── S-006-cache-ram.md
    ├── S-007-state-and-lock.md            (قبل S-005)
    ├── S-008-sqlite-graph.md
    ├── S-009-reader-api.md
    ├── S-009b-security-hardening.md       (مع S-009)
    ├── S-010-cli-validate-rebuild.md
    ├── S-011-watcher.md                   (Win+Linux)
    ├── S-011b-watcher-macos.md            (macOS منفصل)
    ├── S-012-cli-query.md
    ├── S-013-tier3-snapshots.md
    ├── S-014-stats-logging.md             (stats + exit codes فقط)
    ├── S-014b-ci-pipeline.md              (CI/CD + docs gen)
    ├── S-015a-migrate-builtins.md
    ├── S-015b-migrate-keywords.md
    ├── S-015c-migrate-errors.md
    ├── S-015d-migrate-lessons.md              (MD → YAML SSoT)
    ├── S-015e-md-generator.md                  (YAML → MD generated)
    └── S-016-legacy-removal.md                 (إضافة MD إلى .gitignore)
```

## أين تبدأ؟

| دور | ابدأ من |
|-----|---------|
| **أي مطوِّر يكتب YAML** | [DATA_SCHEMA_CONTRACTS.md](DATA_SCHEMA_CONTRACTS.md) ← **إلزامي قبل أي سطر في `data/`** |
| Tech Lead | [epic.md](epic.md) ← الرؤية + النطاق |
| Sprint Planner | [sprint-plan.md](sprint-plan.md) ← graph + sequence |
| QA / Test Architect | [test-strategy.md](test-strategy.md) ← pyramid + gates |
| Developer (Sprint 0) | [stories/S-000a-foundation-schemas.md](stories/S-000a-foundation-schemas.md) |
| Developer (POC) | [stories/S-001-loader-poc.md](stories/S-001-loader-poc.md) |
| Security Reviewer | [stories/S-009b-security-hardening.md](stories/S-009b-security-hardening.md) |

## وثائق تصميم أساسية (اقرأها أولاً)

- [YAML_UNIFIED_SCHEMA_DESIGN.md](../2-architecture/planning/YAML_UNIFIED_SCHEMA_DESIGN.md) — تصميم v1.0 النهائي للـYAML SSoT (المرجع الأصلي)
- [DOC_DISTRIBUTION_FLOWS.md](../2-architecture/planning/DOC_DISTRIBUTION_FLOWS.md) — مخططات تدفّق التوثيق
- [ADR-SADINFO-ARCHITECTURE.md](../2-architecture/decisions/ADR-SADINFO-ARCHITECTURE.md) — المعمارية الهدف
- [DATA_SCHEMA_CONTRACTS.md](planning/DATA_SCHEMA_CONTRACTS.md) — مرجع موجز يربط كل story ببنية YAML

## القواعد الذهبية

1. **S-000a أولاً** — كل شيء يعتمد عليه (schemas, i18n_policy, logger, baseline)
2. **S-007 قبل S-005** — لا يكتب Aggregator أي شيء قبل أن يكون قفل موجوداً
3. **S-009b مع S-009** — لا تطلق Reader علني بدون security validators
4. **P1 قبل S-013** — `sad --report-calls` متطلَّب خارجي
5. **S-014b قبل S-015a** — CI/CD يجب أن يكون جاهزاً قبل cycle migration
6. **YAML = SSoT الوحيد** — بعد S-015e: كل docs/*.md و وثائق/*.md تُولّد آلياً، لا تُحرّر يدوياً

## مراجع

- Architecture: [ADR-SADINFO-ARCHITECTURE.md](../2-architecture/decisions/ADR-SADINFO-ARCHITECTURE.md)
- YAML Schema: [YAML_UNIFIED_SCHEMA_DESIGN.md](../2-architecture/planning/YAML_UNIFIED_SCHEMA_DESIGN.md)
- Memory: `/memories/repo/keywords_yaml_sot_v41.md`
