---
title: "نظام الاختبارات الموحّد"
englishName: "testing-system"
date: 2026-06-11
status: ACTIVE
owner: TBD
relatedDocuments:
  - planning/prd.md
  - planning/architecture.md
  - status/implementation_status.md
purpose: |
  نظام اختبارات موحّد ومنظَّم للغة ص يستبدل تشتّت 50+ مجلداً تحت tests/
  ببنية من ثلاث طبقات: اختبارات وحدة (C++)، اختبارات سلوك (.ص مزدوجة التنفيذ
  عبر المفسر والمترجم)، واختبارات نظام وأدوات — مع تغطية قابلة للتدقيق لكل
  أقسام اللغة الـ12 ومستويات هرمية P0→full مربوطة بدورة CI.
---

# نظام الاختبارات الموحّد (Testing System)

## نظرة عامة

نظام يوحّد اختبارات لغة ص في **ثلاث طبقات** فقط (وحدة / سلوك / نظام)، يجعل العمود
الفقري هو محرّك التنفيذ المزدوج (`runner.py`) الذي يقارن مخرجات المفسر والمترجم
(ADR-03)، ويربط كل قسم من أقسام اللغة الـ12 بتغطية اختبار قابلة للتدقيق.

## الحالة الحالية

| البند | القيمة |
|---|---|
| النسبة المنجَزة | 100% (الملحمة مُغلقة — ACTIVE) |
| اختبارات تمر | behavior 86.0% (393/457) · unit 52 · P0 100% — صفر تراجع |
| القصص المنجَزة | 7/7 (TEST-001..007) |
| آخر تحديث | 2026-06-12 (RETRO) |

## روابط سريعة

- 📋 [PRD](planning/prd.md)
- 🏗️ [Architecture](planning/architecture.md)
- 📊 [Implementation Status](status/implementation_status.md)
- 📁 [Stories](stories/)
- 📐 [Decisions (ADRs)](decisions/)
- 🗂️ [Epics / Backlog](epics/BACKLOG.md)

## المبدأ الجوهري

> **ثلاث طبقات لا أكثر — يُصنَّف الاختبار حسب *ماذا* يتحقق، لا حسب *أي ميزة*.**

```
الطبقة 1 — وحدة (C++):     مكوّن معزول (Lexer/Parser/AST/SIR/Value)   tests/unit/
الطبقة 2 — سلوك (.ص):      تنفيذ مزدوج مفسر+مترجم لكل أقسام اللغة     tests/behavior/
الطبقة 3 — نظام وأدوات:    LSP/formatter/pkg/sadinfo/codegen/أداء    tests/system/
```

## كيفية المساهمة

1. اقرأ [PRD](planning/prd.md) لفهم المشكلة والأهداف.
2. راجع [Architecture](planning/architecture.md) للبنية والطبقات الثلاث.
3. اختر قصة من [stories/](stories/) ضمن `not-started`.
4. التزم بحوكمة `_bmad-output/governance/1-policy/` وقائمة التسليم في مهارة `sad-lang-dev`.
