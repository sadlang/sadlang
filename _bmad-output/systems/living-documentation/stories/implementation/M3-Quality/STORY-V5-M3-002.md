---
id: S-V5-M3-002
title: "T2 — تطابق الحقيقة مع الكود الفعلي"
type: implementation-story
milestone: M3-Quality
scope: validation-t2
status: ready
estimate: L
depends_on: [S-V5-M3-001, S-V5-M2-002]
date: 2026-06-05
governed_by: [ADR-004]
---

# S-V5-M3-002 — T2 Language Match

## القصة
بصفتي **بوابة جودة**، أريد **إثبات تطابق Truth مع المُولَّد الفعلي 100%** حتى **تكون الحقيقة مطابقة للُّغة الحقيقية لا ادعاءً (GR-01)**.

## معايير القبول
- **Given** Wrapper + C++ Generated جاهزان
- **When** نقارن `language-truth` ↔ `Sad::Lexer::Generated::allEntries()`
- **Then** تطابق 100% (لا كيان زائد/ناقص) + يفشل على أي فرق

## المهام
- [ ] اختبار يقارن مجموعة IDs Truth بمجموعة IDs Generated لكل نطاق
- [ ] فحص الأسماء العربية (احترام U+0651 شدّة، مثل `عيّن`)
- [ ] تقرير فروق صريح (زائد/ناقص/مختلف)
- [ ] دمج في CI + بناء أخضر

## المصدر في الكود
- `shared/lexer/` (Generated)، `shared/langtruth/`

## تعريف "تم"
T2 يُثبت تطابق 100% + يفشل على أي انحراف + يحترم الشدّة في الأسماء.
