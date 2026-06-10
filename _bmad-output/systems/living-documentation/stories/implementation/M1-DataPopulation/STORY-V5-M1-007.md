---
id: S-V5-M1-007
title: "نطاق directives — التوجيهات (@)"
type: implementation-story
milestone: M1-DataPopulation
scope: directives
entity_count: 7
codegen: gen_directives.py (جديد ❌)
status: ready
estimate: S
depends_on: [S-V5-M1-001]
date: 2026-06-05
---

# S-V5-M1-007 — نطاق directives

## القصة
بصفتي **فريق اللغة**، أريد **توثيق التوجيهات الـ7 (`@حجم`/`@ذري`/`@غير_آمن`/`@وقت_الترجمة`/`@متطاير`/`@تجميع`)** حتى **تعرف الأدوات التعليمات منخفضة المستوى**.

## معايير القبول
- **Given** Schema directive جاهز (M0-003)
- **When** نكتب `language-truth/directives.yaml` + ننشئ `gen_directives.py`
- **Then** كل توجيه له اسم + توقيع + وصف + C++ Generated + بناء أخضر

## المهام
- [ ] كتابة `directives.yaml`: `@حجم(نوع)`، `@ذري(عملية,...)`، `@غير_آمن`، `@وقت_الترجمة`، `@متطاير`، `@تجميع("code")`
- [ ] لكل توجيه: التوقيع + سياق الاستخدام (sadc فقط؟) + مثال
- [ ] إنشاء `gen_directives.py` + ربط في `codegen.cmake`
- [ ] T1 + بناء أخضر

## المصدر في الكود
- `shared/lexer/`، `shared/parser/` (معالجة `@`)

## تعريف "تم"
YAML يطابق Schema + كل توجيه له ID + توقيع موثَّق + بناء أخضر.
