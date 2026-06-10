---
title: "الطبقة 2 — المعمارية والـSchema الموحَّد"
parentSystem: living-documentation
layer: 2
date: 2026-06-01
status: ACTIVE
---

# 🏗️ الطبقة 2 — المعمارية (Architecture & Unified Schema)

> تَستهلك الـPRD من [1-strategy/](../1-strategy/) وتُنتج المعمارية الموحَّدة + الـYAML Schema الذي تَستخدمه [3-implementation/](../3-implementation/) لبناء `sadinfo`.

## المسؤوليات

1. **ARCHITECTURE_MAP** — خريطة معمارية كاملة لنظام التوثيق
2. **YAML_UNIFIED_SCHEMA_DESIGN** — تصميم schema موحَّد لجميع بيانات اللغة (Single Source of Truth)
3. **DOC_FLOW_REALITY** و **DOC_DISTRIBUTION_FLOWS** — مخططات تدفُّق البيانات الفعلية
4. **ADRs** — جميع القرارات المعمارية (داخل `decisions/`)
5. **Stories معمارية** — Stories لتنسيق معمارية النظام (1.4 diataxis, 2.0 website-move, 3.1 render-lsp, ...)

## البنية

| المجلد | المحتوى | عدد الملفات |
|---|---|---|
| [planning/](planning/) | ARCHITECTURE_MAP, YAML_UNIFIED_SCHEMA_DESIGN, DOC_FLOW_REALITY, DOC_DISTRIBUTION_FLOWS, README | 5 |
| [epics/](epics/) | ADR-006b-epics.md (الـEpic = ADR معماري) | 2 |
| [stories/](stories/) | 13 ستوري معمارية (1.4, 2.0, 3.1, 4.3, 5.x, utm-6.3..6.7) | 13 |
| [decisions/](decisions/) | ADR-SADINFO-ARCHITECTURE.md + ADRs أخرى | متغير |
| [sprints/](sprints/) | Sprint plans للعمل المعماري | متغير |
| [status/](status/) | تقارير حالة + reviews | 2+ |

## وثائق المفتاح

- [planning/ARCHITECTURE_MAP.md](planning/ARCHITECTURE_MAP.md) — نقطة البداية
- [planning/YAML_UNIFIED_SCHEMA_DESIGN.md](planning/YAML_UNIFIED_SCHEMA_DESIGN.md) — تصميم schema الـSoT (v1.0 نهائي)
- [planning/DOC_FLOW_REALITY.md](planning/DOC_FLOW_REALITY.md) — التدفُّق الفعلي
- [decisions/ADR-SADINFO-ARCHITECTURE.md](decisions/ADR-SADINFO-ARCHITECTURE.md) — معمارية sadinfo المستهدفة

## التَبعيات

- **من الأعلى**: [1-strategy/01_prd/prd-docs-system-v2.md](../1-strategy/01_prd/prd-docs-system-v2.md) (المتطلبات FR-A..FR-E)
- **إلى الأسفل**: [3-implementation/planning/DATA_SCHEMA_CONTRACTS.md](../3-implementation/planning/DATA_SCHEMA_CONTRACTS.md) (تَنفيذ الـschema)
