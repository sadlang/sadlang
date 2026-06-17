---
task_id: T-0006
title: "إنشاء Foundation Schemas — البنية الأساسية لـYAML SoT"
assigned_to: agent_alpha
created_by: PM (John)
created_at: 2026-06-01T08:50:00Z
deadline: 2026-06-10
priority: P0
story_id: S-000a
milestone: M1
depends_on: [T-0008]  # يَنتظر مراجَعة Winston للـschema
guarded_files: []
status: queued
estimated_hours: 8
---

# 📋 T-0006 — Foundation Schemas

## السياق

S-000a هو الستوري الأساس لكل M1..M7. كل الستوريات اللاحقة تَعتمد على هذه الـschemas. تَأخر هذه = تَأخر كل شيء.

## المُدخلات

1. الستوري الكامل: [S-000a-foundation-schemas.md](../../../../systems/living-documentation/3-implementation/stories/S-000a-foundation-schemas.md)
2. المعمارية: [SADINFO Architecture ADR](../../../../systems/living-documentation/2-architecture/decisions/ADR-SADINFO-ARCHITECTURE.md)
3. **يَنتظر:** نتيجة T-0008 (مراجَعة Winston لـUnified Schema)

## المخرجات المطلوبة (DoD)

ملفات JSON Schema في `data/_schemas/_meta/`:

- [ ] `keyword.schema.json` — schema لكل entry في keywords.yaml
- [ ] `builtin.schema.json` — schema لكل entry في builtins.yaml
- [ ] `error.schema.json` — schema لرسائل الأخطاء
- [ ] `directive.schema.json` — schema للتَوجيهات (`@`)
- [ ] `index.schema.json` — schema للفهرس المُجمَّع
- [ ] `README.md` يَشرح كل schema بمَثال

## معايير القبول

- [ ] كل schema تَتبع JSON Schema Draft 2020-12
- [ ] `npx ajv compile -s data/_schemas/_meta/*.json` يَنجح بدون أخطاء
- [ ] كل schema تَحوي: `$id`, `$schema`, `title`, `description`, `required`, `properties`
- [ ] أمثلة `examples` في كل schema (≥ 2 لكل)

## القواعد الحَوكمية

- **CW-03 (فصل الاهتمامات):** schema تَصف البنية فقط — لا منطق
- **CW-08:** تَوثيق ثنائي اللغة في README
- **انتظر T-0008:** لا تَبدأ قبل أن يَكتب Winston نَتيجة مراجَعته

## كيفية الإبلاغ

نَفس [T-0001](T-0001-delta-keywords-yaml.md).
