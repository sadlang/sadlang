---
id: S-TS-P10
title: "تضييق النوع بالتدفّق (smart narrowing) لأمان null"
type: implementation-story
milestone: M3-AdvancedTypes
status: ready
estimate: L
depends_on: [S-TS-P9]
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
  - ../decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md
date: 2026-06-12
---

# S-TS-P10 — تضييق النوع بالتدفّق

> **فُصِلت عن S-TS-P9 بعد نقد أميليا:** تضييق التدفّق بنية flow-analysis كاملة (L وحده)، لا AC ضمن قصّة الفرض.

## القصة
بصفتي **مستخدم لغة ص**, أريد **داخل `إذا (س != لاشيء)` أن يُعامَل `س` كنوع غير اختياري** حتى **أصل لأعضائه دون `؟.` بعد الفحص (smart cast)**.

## السياق
المدقّق الدلالي **لا يملك أي تحليل تدفّق**: `visitIfStmt` ([type_checker_stmt_visitors.cpp:133-159](../../../../shared/semantic/src/semantic/type_checker_stmt_visitors.cpp#L133)) يقرأ الشرط لفحص نوعه (boolean) ثم `enterScope/accept/exitScope` فقط — **لا قراءة لنمط الشرط ولا تضييق**. و`TypeEnvironment` = `bind/lookup` بسيط بلا flow. هذه القصّة تُدخِل البنية من الصفر. ينفّذ [ADR-TYPESYSTEM-001 §5](../decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md).

## معايير القبول
- **AC1 — التضييق** داخل `إذا (س != لاشيء)` حيث `س: T؟`: يُعامَل `س` كـ`T` (غير اختياري) في فرع `then`؛ و`else` يحتفظ بـ`T؟`.
- **AC2 — الوصول بعد الفحص** `س.حقل` داخل الفرع المُضيَّق = مقبول دون `؟.` (لا خطأ من P9-AC3).
- **AC3 — التراجع** خارج الكتلة يعود `س` إلى `T؟`.
- **AC4 — أنماط** تغطية `!= لاشيء` و`== لاشيء` (عكسي) و`؟؟`/التأكيد كنقاط تضييق.
- **AC5 — DoD §0.5** اختبار `.ص` إيجابي+سلبي، بناء Debug+Release، كلا المحرّكين أو `@skip_compiler` موثّق.

## المهام
- [ ] بنية flow type-environment (تضييق/تراجع بالنطاق) في المدقّق.
- [ ] قراءة نمط شرط `if` في `visitIfStmt` (تمييز `س != لاشيء`).
- [ ] تطبيق التضييق على الفرع + التراجع عند الخروج.
- [ ] (المترجم) محاذاة codegen إن لزم.
- [ ] اختبارات + تحديث `status/` بدليل.

## المصدر في الكود
- `shared/semantic/src/semantic/type_checker_stmt_visitors.cpp:133-159` (visitIfStmt)
- `shared/semantic/...` (TypeEnvironment — يُوسَّع لـflow)

## تعريف "تم"
`س.حقل` يعمل داخل `إذا (س != لاشيء)` دون `؟.`، يتراجع خارجها، اختبارات خضراء في المحرّكين.
