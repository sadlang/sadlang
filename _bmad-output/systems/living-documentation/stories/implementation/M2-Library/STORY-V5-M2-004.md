---
id: S-V5-M2-004
title: "إيقاف data/language/ القديم (مع تحذير)"
type: implementation-story
milestone: M2-Library
scope: deprecation
status: ready
estimate: S
depends_on: [S-V5-M2-003, S-V5-M3-002]
date: 2026-06-05
governed_by: [ADR-004, ADR-003]
priority: low
---

# S-V5-M2-004 — إيقاف data/language/ القديم

## القصة
بصفتي **مشرف المستودع**، أريد **إزالة `data/language/` بعد ثبوت التطابق الكامل** حتى **يبقى مصدر حقيقة واحد (language-truth/)**.

## السياق
⚠️ خطوة لا رجعة فيها — تتطلب **تأكيداً صريحاً من المستخدم** قبل التنفيذ (operationalSafety). لا تُنفَّذ إلا بعد نجاح M3-002 (تطابق 100%).

## معايير القبول
- **Given** M3-002 يُثبت تطابق Truth ↔ Sad::Lexer::Generated 100%
- **When** نضيف تحذير إهمال ثم نحذف `data/language/` بعد تأكيد المستخدم
- **Then** لا مرجع متبقٍّ لـ `data/language/` + بناء أخضر

## المهام
- [ ] إضافة تحذير إهمال في `data/language/` (README + رسالة بناء)
- [ ] `grep` لكل مرجع `data/language/` وتحويله إلى `language-truth/`
- [ ] ⚠️ طلب تأكيد صريح من المستخدم قبل الحذف
- [ ] حذف `data/language/` بعد التأكيد + بناء أخضر

## المصدر في الكود
- `data/language/`، مراجع cmake/scripts

## تعريف "تم"
صفر مراجع لـ `data/language/` + بناء أخضر + موافقة مُسجَّلة.
