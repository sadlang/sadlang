---
id: S-V5-M1-013
title: "نطاق oop_constructs — أبنية البرمجة الكائنية"
type: implementation-story
milestone: M1-DataPopulation
scope: oop_constructs
entity_count: 20
codegen: gen_oop.py (جديد ❌)
status: ready
estimate: L
depends_on: [S-V5-M1-001]
date: 2026-06-05
---

# S-V5-M1-013 — نطاق oop_constructs ⭐

## القصة
بصفتي **فريق اللغة**، أريد **توثيق أبنية OOP (أصناف/بنى/تعداد/سمات/وراثة/خصائص/عوامل/قوالب)** حتى **تشرح الأدوات أكبر سطح لغوي بأمثلة صحيحة**.

## السياق
الكلمات مغطاة كـ keywords (M1-001)، لكن هذا النطاق يوثّق **كيفية تركيبها** (قواعد + أمثلة) من `parser/specs/oop/`.

## معايير القبول
- **Given** Schema grammar_construct/oop_construct جاهز (M0-003)
- **When** نكتب `language-truth/oop_constructs.yaml` + ننشئ `gen_oop.py`
- **Then** كل بناء له صيغة + مثال Given/When/Then + بناء أخضر

## المهام
- [ ] الأصناف: `صنف`/`باني`/`هدم`/`هذا`/`الأساس`/`جديد`
- [ ] البنى: `بنية`، التعداد: `تعداد`
- [ ] السمات: `سمة`/`نفّذ`، الوراثة: `يرث`
- [ ] الخصائص: `خاصية`/`احصل`/`عيّن`
- [ ] محددات الوصول: `عام`/`خاص`/`محمي`/`ساكن`/`مجرد`
- [ ] تحميل العوامل: `عامل`، القوالب: generics `<ت>`
- [ ] إنشاء `gen_oop.py` + ربط في `codegen.cmake` + T1 + بناء أخضر

## المصدر في الكود
- `shared/parser/src/specs/oop/{parser_class,parser_constructor,parser_extension}.cpp`
- `shared/parser/src/declarations/parser_oop.cpp`
- `interpreter/src/visitors/expression_evaluator_oop*.cpp`

## تعريف "تم"
YAML يطابق Schema + كل بناء له ID + مثال يُحاكي التنفيذ + بناء أخضر.
