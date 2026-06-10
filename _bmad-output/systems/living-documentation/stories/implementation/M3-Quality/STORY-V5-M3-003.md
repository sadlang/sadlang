---
id: S-V5-M3-003
title: "T3+T4 — الاكتمال واقتراحات الإصلاح"
type: implementation-story
milestone: M3-Quality
scope: validation-t3-t4
status: ready
estimate: M
depends_on: [S-V5-M3-002]
date: 2026-06-05
governed_by: [ADR-004]
---

# S-V5-M3-003 — T3 Completeness + T4 Suggestions

## القصة
بصفتي **بوابة جودة**، أريد **ضمان اكتمال الحقول الإلزامية واقتراحات إصلاح ثنائية اللغة** حتى **تكون كل رسالة خطأ مفيدة وكل كيان كامل**.

## معايير القبول
- **Given** كل النطاقات + T2 ناجح
- **When** نشغّل T3 (اكتمال) + T4 (اقتراحات)
- **Then** كل كيان له `id`/`since`/`status` + كل خطأ له `fix_suggestion_ar/en`

## المهام
- [ ] T3: كل كيان يملك `id` + `since` + `status` (لا حقل إلزامي ناقص)
- [ ] T4: كل رسالة خطأ تملك `fix_suggestion_ar` + `fix_suggestion_en`
- [ ] رسائل فشل تُحدّد الكيان والحقل الناقص
- [ ] دمج في CI + بناء أخضر

## المصدر في الكود
- `data/language/error_messages.yaml` (المصدر) ← `language-truth/errors.yaml`
- `scripts/codegen/gen_error_messages.py`

## تعريف "تم"
T3+T4 يفشلان على نقص + ينجحان على البيانات الكاملة + مدمجان في CI.
