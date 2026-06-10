---
id: S-V5-M3-001
title: "T1 — التحقق من المخطط لكل النطاقات"
type: implementation-story
milestone: M3-Quality
scope: validation-t1
status: ready
estimate: M
depends_on: [S-V5-M2-004]
date: 2026-06-05
governed_by: [ADR-004]
---

# S-V5-M3-001 — T1 Schema شامل

## القصة
بصفتي **بوابة جودة**, أريد **التحقق من كل ملف YAML مقابل مخططه** حتى **لا تدخل بيانات مكسورة الحقيقة (GR-01)**.

## معايير القبول
- **Given** كل النطاقات الـ14 مكتوبة + Schemas موجودة
- **When** نشغّل T1 على كل `language-truth/*.yaml`
- **Then** كل ملف يطابق مخططه + يفشل البناء على أي انحراف

## المهام
- [ ] اختبار T1 يكرّر على كل النطاقات (keywords...expr_constructs + stdlib + learning)
- [ ] كل ملف ← Schema المقابل (jsonschema)
- [ ] رسائل فشل واضحة (المسار + الحقل + السبب)
- [ ] دمج في CI + بناء أخضر

## المصدر في الكود
- `scripts/codegen/test_*.py`، `data/_schemas/*.schema.json`

## تعريف "تم"
T1 يغطي كل النطاقات + يفشل على انحراف Schema + أخضر على البيانات الصحيحة.
