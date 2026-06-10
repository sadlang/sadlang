---
id: S-V5-M1-001
title: "نطاق keywords — إثراء subcategory"
type: implementation-story
milestone: M1-DataPopulation
scope: keywords
entity_count: 91
codegen: gen_keywords.py (موجود ✅)
status: ready
estimate: M
depends_on: [S-V5-M0-004]
date: 2026-06-05
---

# S-V5-M1-001 — نطاق keywords

## القصة
بصفتي **فريق اللغة**، أريد **إثراء `keywords.yaml` بحقول `subcategory` و `id`** حتى **تستطيع الأدوات التصنيف الدلالي (محجوزة/سياقية/أنواع)**.

## معايير القبول
- **Given** Schema keywords جاهز (M0-003)
- **When** نُضيف `id` فريد + `subcategory` لكل الكلمات الـ91
- **Then** الكلمات مُتحقَّقة بـ Schema + C++ Generated مُولَّد + البناء أخضر

## المهام
- [x] إضافة `id` فريد (نمط `KW-RES-001` إلخ) لكل الـ91 إدخال
- [x] إضافة `subcategory` + `since: "1.0.0"` لكل إدخال
- [x] تحديث `keywords.schema.json` لقبول الحقول الجديدة
- [x] تشغيل `gen_keywords.py` + تأكيد `keywords_generated.cpp` مطابق
- [x] T1 (Schema) أخضر — 26 اختبار ✅

## المصدر في الكود
- `shared/lexer/` (40 محجوزة + 25 سياقية + 9 أنواع)
- `scripts/codegen/gen_keywords.py`

## تعريف "تم"
YAML يطابق Schema 100% + كل كلمة لها ID فريد + `since` + البناء أخضر.
