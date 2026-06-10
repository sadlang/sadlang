---
id: S-V5-M1-014
title: "نطاق expr_constructs — أبنية التعابير"
type: implementation-story
milestone: M1-DataPopulation
scope: expr_constructs
entity_count: 12
codegen: gen_expr.py (جديد ❌)
status: ready
estimate: M
depends_on: [S-V5-M1-001]
date: 2026-06-05
---

# S-V5-M1-014 — نطاق expr_constructs ⭐

## القصة
بصفتي **فريق اللغة**، أريد **توثيق أبنية التعابير (استيعاب/أنابيب/lambda/closure/f-string/tuple/spread/ثلاثي)** حتى **تشرح الأدوات التعابير المتقدمة بدقة**.

## معايير القبول
- **Given** Schema expr_construct جاهز (M0-003)
- **When** نكتب `language-truth/expr_constructs.yaml` + ننشئ `gen_expr.py`
- **Then** كل بناء له صيغة + مثال Given/When/Then + بناء أخضر

## المهام
- [ ] الاستيعاب: list/dict comprehension
- [ ] الأنابيب: pipeline operator
- [ ] الدوال المجهولة: `لامدا` + closures
- [ ] f-string، tuple، spread (`...`)، العامل الثلاثي (`? :`)
- [ ] إنشاء `gen_expr.py` + ربط في `codegen.cmake` + T1 + بناء أخضر

## المصدر في الكود
- `shared/parser/src/specs/flow/{parser_comprehension,parser_dict_comprehension,parser_pipeline}.cpp`
- `shared/parser/src/core/parser_expressions.cpp`

## تعريف "تم"
YAML يطابق Schema + كل بناء له ID + مثال يُحاكي التنفيذ + بناء أخضر.
