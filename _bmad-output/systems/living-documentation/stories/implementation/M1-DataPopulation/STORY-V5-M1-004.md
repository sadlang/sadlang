---
id: S-V5-M1-004
title: "نطاق modules — الوحدات والاستيراد"
type: implementation-story
milestone: M1-DataPopulation
scope: modules
entity_count: 10
codegen: gen_modules.py (موجود ✅)
status: ready
estimate: M
depends_on: [S-V5-M1-001]
priority: high
date: 2026-06-05
---

# S-V5-M1-004 — نطاق modules ⭐

## القصة
بصفتي **فريق اللغة**، أريد **توثيق الوحدات القابلة للاستيراد وصادراتها (رياضيات/نصوص/أساسيات/تشفير/شبكة/...)** حتى **تعرف الأدوات ما يُستورَد عبر `استورد`/`من ... استورد`**.

## السياق
`gen_modules.py` مُسجَّل في `gen_all.py` ويتوقع `modules.yaml` + `module.schema.json`.

## معايير القبول
- **Given** Schema module جاهز + `gen_modules.py` موجود
- **When** نكتب `language-truth/stdlib/modules.yaml` بالوحدات وصادراتها
- **Then** الكيانات مُتحقَّقة + `modules_generated.h` مُولَّد + البناء أخضر

## المهام
- [ ] كتابة `stdlib/modules.yaml` (رياضيات/نصوص/أساسيات/تأكيدات/تزامن_متقدم/خرائط/منصة/شبكة_عالية/تشفير/مقابس)
- [ ] لكل وحدة: الاسم العربي + البدائل + قائمة الصادرات
- [ ] ربط `gen_modules.py` في `codegen.cmake`
- [ ] T1 (Schema) + بناء أخضر

## المصدر في الكود
- `shared/parser/src/declarations/parser_modules.cpp`
- `interpreter/src/visitors/statement_executor_modules.cpp`
- `scripts/codegen/gen_modules.py`، `stdlib/`

## تعريف "تم"
YAML يطابق Schema + كل وحدة لها ID فريد + صادراتها موثَّقة + بناء أخضر.
