---
id: S-V5-M0-004
title: "اختبار T1 (Schema Validation) المبدئي"
type: implementation-story
milestone: M0-Foundation
status: ready
estimate: S
depends_on: [S-V5-M0-003]
governed_by:
  - ../../../IMPLEMENTATION_PLAN.md
date: 2026-06-05
---

# S-V5-M0-004 — اختبار T1 (Schema Validation) المبدئي

## القصة
بصفتي **مهندس CI**، أريد **اختبار يفحص كل YAML ضد Schema المقابل** حتى **يفشل البناء فوراً على أي انحراف بيانات**.

## السياق
T1 هو أول حارس جودة. يبدأ على keywords ويُصمَّم قابلاً للتوسعة لباقي النطاقات في M3-001.

## معايير القبول
- **Given** Schemas جاهزة (M0-003) + `keywords.yaml` موجود
- **When** نكتب `language-truth/tests/test_schema_validation.py`
- **Then** الاختبار يفحص كل YAML ضد Schema المقابل
- **And** يفشل البناء على أي انحراف

## المهام
- [x] كتابة `language-truth/tests/test_schema_validation.py` (pytest + jsonschema)
- [x] تغطية `keywords.yaml` كحالة أولى — 26 اختبار نجحت
- [x] هيكل قابل للتوسعة (`YAML_SCHEMA_PAIRS` تُملأ مع كل نطاق M1)
- [ ] ربط الاختبار في بوابة CI — مُرجَأ لـ M3

## المصدر في الكود
- نمط الاختبارات القائمة: `scripts/codegen/test_*.py`

## تعريف "تم"
`pytest language-truth/tests/test_schema_validation.py` أخضر على keywords + هيكل جاهز لباقي النطاقات.
