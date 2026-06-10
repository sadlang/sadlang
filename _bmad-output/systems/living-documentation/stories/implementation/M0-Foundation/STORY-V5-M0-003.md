---
id: S-V5-M0-003
title: "JSON Schemas الأساسية لكل النطاقات"
type: implementation-story
milestone: M0-Foundation
status: ready
estimate: M
depends_on: [S-V5-M0-001]
governed_by:
  - ../../../IMPLEMENTATION_PLAN.md
date: 2026-06-05
---

# S-V5-M0-003 — JSON Schemas الأساسية

## القصة
بصفتي **حارس جودة البيانات**, أريد **JSON Schema لكل نطاق من النطاقات الـ14** حتى **يُرفض أي YAML منحرف وقت البناء (ND-V4-3)**.

## السياق
نُعيد استخدام Schemas القائمة في `data/_schemas/` حيثما أمكن (builtin/error/lesson/exercise/example) ونكتب الجديدة. كل Schema يحوي `CommonFields` (id, schema_version, since, status).

## معايير القبول
- **Given** نملك `keywords.schema.json` فقط جاهزاً
- **When** نكتب/ننقل Schemas لكل النطاقات المُخطَّطة (ARCHITECTURE §1)
- **Then** كل نطاق له `_schemas/<scope>.schema.json` صالح (jsonschema metaschema)
- **And** كل Schema يحوي `CommonFields` (id, schema_version, since, status)

## المهام
- [x] إعادة استخدام: `builtin_function` من `data/_schemas/builtin.schema.json` (نُسِخ)
- [x] كتابة جديدة: `operator.schema.json`, `directive.schema.json`, `type.schema.json`
- [x] كتابة جديدة: `grammar_rule.schema.json` (يغطي grammar/oop/expr constructs)
- [x] كتابة جديدة: `error.schema.json`, `stdlib_module.schema.json`, `stdlib_function.schema.json`
- [x] التحقق من كل Schema (JSON valid) — جميعها اجتازت
- [ ] نقل: `type_method.schema.json` (من `data/language/`) — مُرجَأ لـ M1-003

## المصدر في الكود
- `data/_schemas/` (Schemas قائمة للإعادة)
- `data/language/type_methods.schema.json`, `keywords.schema.json`

## تعريف "تم"
كل Schema صالح + يحوي CommonFields + اختبار metaschema أخضر.
