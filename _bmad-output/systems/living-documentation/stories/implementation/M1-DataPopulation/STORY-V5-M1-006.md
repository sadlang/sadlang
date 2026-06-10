---
id: S-V5-M1-006
title: "نطاق operators — العوامل + الأسبقية والترابط"
type: implementation-story
milestone: M1-DataPopulation
scope: operators
entity_count: 40
codegen: gen_operators.py (جديد ❌)
status: ready
estimate: M
depends_on: [S-V5-M1-001]
date: 2026-06-05
---

# S-V5-M1-006 — نطاق operators

## القصة
بصفتي **فريق اللغة**، أريد **توثيق العوامل الـ~40 مع أسبقيتها وترابطها** حتى **تستطيع الأدوات تلوين/تحليل التعابير بدقة**.

## معايير القبول
- **Given** Schema operator جاهز (M0-003)
- **When** نكتب `language-truth/operators.yaml` + ننشئ `gen_operators.py`
- **Then** كل عامل له رمز + أسبقية + ترابط + C++ Generated + بناء أخضر

## المهام
- [ ] كتابة `operators.yaml`: حسابية (`+ - * / % **`)، مقارنة (`== != < > <= >=`)، منطقية (`&& || ! و أو ليس`)، إسناد، عضوية (`في`)، ثلاثي (`? :`)، وصول (`. [] ()`)
- [ ] لكل عامل: `precedence` + `associativity` + الرمز + الاسم العربي
- [ ] إنشاء `scripts/codegen/gen_operators.py` (نمط `gen_keywords.py`)
- [ ] ربط في `codegen.cmake` + T1 + بناء أخضر

## المصدر في الكود
- `shared/parser/src/core/parser_expressions.cpp` (جدول الأسبقية الفعلي)

## تعريف "تم"
YAML يطابق Schema + كل عامل له ID + أسبقية تطابق `parser_expressions` + بناء أخضر.
