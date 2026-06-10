---
title: "الطبقة 3 — التنفيذ (sadinfo CLI)"
parentSystem: living-documentation
layer: 3
date: 2026-06-01
status: ACTIVE
---

# ⚙️ الطبقة 3 — التنفيذ (sadinfo Implementation)

> تَستهلك المعمارية والـSchema من [2-architecture/](../2-architecture/) وتُنتج أداة CLI فعلية (`tools/sadinfo/`) + مكتبة (`shared/sadinfo_core/`).

> 📌 **ملاحظة**: محتوى README الأصلي للأداة (من sadinfo القديم) موجود في [README-tool.md](README-tool.md) — أما هذا README يَشرح دور الطبقة فقط.

## المسؤوليات

1. تنفيذ **`tools/sadinfo/`** — CLI رقيق يُصدِر JSON/YAML
2. تنفيذ **`shared/sadinfo_core/`** — مكتبة C++17 (Loader, Aggregator, Validator, Exporter, Watcher, Cache Manager)
3. اختبارات شاملة (Tier-1/2/3 Validators)
4. تَكامل مع LSP, الموقع, CI
5. هجرة البيانات من الكود إلى YAML (S-015a..S-015e)

## البنية

| المجلد | المحتوى | عدد الملفات |
|---|---|---|
| [planning/](planning/) | DATA_SCHEMA_CONTRACTS, test-strategy, README | 3+ |
| [epics/](epics/) | epic.md (إبيك تنفيذي واحد) + README | 2 |
| [stories/](stories/) | 24 ستوري S-000a → S-016 | 24 |
| [prerequisites/](prerequisites/) | P1-sad-report-calls.md | 1+ |
| [decisions/](decisions/) | ADRs تنفيذية | متغير |
| [sprints/](sprints/) | Sprint plans للتنفيذ | متغير |
| [status/](status/) | تقارير تقدُّم التنفيذ | متغير |

## مَسار الستوريات (24 ستوري)

```
S-000a-foundation-schemas → S-001-loader-poc → S-002-entity-view
       ↓
S-003-tier1-validator → S-004-hash-strategy → S-005-aggregator-merkle
       ↓
S-006-aliases → S-007-state-and-lock → S-008-sqlite-graph
       ↓
S-009-reader-api → S-009b-security-hardening → S-010-exporter
       ↓
S-011-watcher → S-011b-watcher-macos → S-012-tier2-validator
       ↓
S-013-tier3-snapshots → S-014-stats-logging → S-014b-ci-pipeline
       ↓
S-015a-migrate-builtins → S-015b-migrate-keywords → S-015c-migrate-errors
       ↓
S-015d-migrate-lessons → S-015e-md-generator → S-016-legacy-removal
```

## التَبعيات

- **من الأعلى**:
  - [2-architecture/planning/YAML_UNIFIED_SCHEMA_DESIGN.md](../2-architecture/planning/YAML_UNIFIED_SCHEMA_DESIGN.md) (schema المرجعي)
  - [2-architecture/decisions/ADR-SADINFO-ARCHITECTURE.md](../2-architecture/decisions/ADR-SADINFO-ARCHITECTURE.md) (معمارية الأداة)
- **إلى الأسفل**: `tools/sadinfo/`, `shared/sadinfo_core/`, `data/_schemas/`
