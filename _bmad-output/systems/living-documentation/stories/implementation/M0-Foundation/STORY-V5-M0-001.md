---
id: S-V5-M0-001
title: "إنشاء بنية language-truth/"
type: implementation-story
milestone: M0-Foundation
status: ready
estimate: S
depends_on: []
governed_by:
  - ../../../IMPLEMENTATION_PLAN.md
  - ../../../decisions/ADR-DOCS-V4-003-CODEGEN-INTEGRATION.md
date: 2026-06-05
---

# S-V5-M0-001 — إنشاء بنية `language-truth/`

## القصة
بصفتي **فريق اللغة**، أريد **مجلد `language-truth/` بالبنية المعتمدة** حتى **يصبح نقطة بدء مستقلة لأي أداة (LSP/Website/Formatter) دون خلط ببيانات أخرى**.

## السياق
`language-truth/` هو SoT الانتقالي المستقل (قرار المستخدم: النقل للاستقلالية). يعمل بالتوازي مع `data/language/` القائم بلا كسر بناء (ARCHITECTURE §1.2).

## معايير القبول
- **Given** المشروع يملك `data/language/keywords.yaml` يعمل
- **When** نُنشئ مجلد `language-truth/` بالبنية المعتمدة (ARCHITECTURE §1)
- **Then** المجلد يحوي الهيكل الكامل + `README.md` يشرح أنه SoT انتقالي
- **And** لا تأثير على البناء الحالي (`cmake --build` يبقى أخضر)

## المهام
- [x] `mkdir language-truth/{_schemas,builtins,stdlib,learning,errors,_meta}`
- [x] كتابة `language-truth/README.md` (نقطة بدء لفِرَق الأدوات)
- [x] كتابة `language-truth/VERSIONING.md` (SemVer للبيانات — ARCHITECTURE §7)
- [x] كتابة `language-truth/_meta/_version.yaml` + `_index.yaml`

## المصدر في الكود
- البنية المرجعية: `ARCHITECTURE.md §1.1`

## تعريف "تم"
البنية موجودة + لا تراجع في البناء + التوثيق ثنائي اللغة للـ README (CW-08).
