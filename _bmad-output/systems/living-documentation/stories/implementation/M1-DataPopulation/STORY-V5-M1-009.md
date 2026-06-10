---
id: S-V5-M1-009
title: "نطاق patterns — أنماط المطابقة"
type: implementation-story
milestone: M1-DataPopulation
scope: patterns
entity_count: 8
codegen: gen_patterns.py (جديد ❌)
status: ready
estimate: M
depends_on: [S-V5-M1-001]
date: 2026-06-05
---

# S-V5-M1-009 — نطاق patterns ⭐

## القصة
بصفتي **فريق اللغة**، أريد **توثيق أنماط المطابقة الـ~8 (حرفي/شامل/متغير/قائمة/OR/نطاق/بنية/ربط)** حتى **تشرح الأدوات `طابق`/`عندما` بدقة**.

## معايير القبول
- **Given** Schema pattern جاهز (M0-003)
- **When** نكتب `language-truth/patterns.yaml` + ننشئ `gen_patterns.py`
- **Then** كل نمط له اسم + صيغة + مثال + C++ Generated + بناء أخضر

## المهام
- [ ] كتابة `patterns.yaml`: حرفي (`عندما 42`)، شامل (`_`/`افتراضي`)، متغير، قائمة (`[أ، ب]`)، OR (`1 || 2`)، نطاق (`1..10`)، بنية (`{س: س}`)، ربط (`ن @ 1..10`)
- [ ] لكل نمط: الصيغة + الوصف + مثال Given/When/Then
- [ ] إنشاء `gen_patterns.py` + ربط في `codegen.cmake`
- [ ] T1 + بناء أخضر

## المصدر في الكود
- `shared/parser/src/specs/patterns/`
- `compiler/src/frontend/sir_builder_match_patterns.cpp`

## تعريف "تم"
YAML يطابق Schema + كل نمط له ID + مثال + تطابق مع `parser/specs/patterns` + بناء أخضر.
