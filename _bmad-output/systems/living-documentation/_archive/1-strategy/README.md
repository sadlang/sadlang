---
status: Active
layer: 1-strategy
parent_system: living-documentation
created: 2026-06-01
---

# الطبقة 1 — الاستراتيجية (Strategy)

## الغرض

هذه الطبقة الأولى في نظام التوثيق الموحَّد `living-documentation/`. تَحتوي على **القرارات الاستراتيجية، المتطلبات، الرؤية، والوثائق التأسيسية** لجميع جوانب التوثيق التلقائي في لغة ص.

> **استثناء بنيوي مُوثَّق:** هذه الطبقة تَستخدم **بنية مرقَّمة قديمة** (`01_prd/`..`08_implementation_artifacts/`) بدلاً من بنية الستة المعتادة (`planning/`, `epics/`, ...). تَم الإبقاء عليها كما هي لأن المحتوى مرتبط بتاريخ تَطور النظام، ولا يُعاد هيكلته (راجع `.github/copilot-instructions.md` — استثناء صريح).

## البنية

| المجلد | المحتوى |
|---|---|
| `01_prd/` | متطلبات المنتج (PRD) للنظام |
| `02_architecture/` | قرارات معمارية على المستوى الاستراتيجي |
| `03_epics_stories/` | الملاحم والقصص الاستراتيجية |
| `04_gap_analysis/` | تحليل الفجوات بين الواقع والمستهدف |
| `05_ux/` | تجربة المستخدم لتوثيق `sadinfo` |
| `06_testing/` | استراتيجية الاختبار الاستراتيجية |
| `07_party_sessions/` | جلسات النقاش متعددة الأطراف |
| `08_implementation_artifacts/` | مخرجات استراتيجية ساندة للتنفيذ |
| `99_arabic_misc/` | محتوى عربي متفرق |
| `INDEX.md` | فهرس تَفصيلي لجميع الملفات |

## الطبقات الأخرى

- **الطبقة 2:** [2-architecture/](../2-architecture/README.md) — البنية المعمارية التفصيلية (IR، schemas، ADRs)
- **الطبقة 3:** [3-implementation/](../3-implementation/README.md) — التنفيذ الفعلي (CLI tool `sadinfo`)

## مراجع

- النظام-الأب: [README.md](../README.md)
- البنية الموحَّدة: [UNIFIED_DOCS_ARCHITECTURE.md](../UNIFIED_DOCS_ARCHITECTURE.md)
- الفهرس الكامل: [INDEX.md](INDEX.md)
