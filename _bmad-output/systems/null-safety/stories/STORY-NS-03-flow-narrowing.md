---
id: NS-03
title: "تحليل التدفّق (Smart Narrowing) — تضييق النوع بعد فحص null"
type: implementation-story
milestone: NS-M2-Flow
status: planned
estimate: L
depends_on: [NS-01]
governed_by:
  - ../planning/ARCHITECTURE.md
  - ../../type-system/decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md
date: 2026-06-13
---

# NS-03 — تحليل التدفّق (Smart Narrowing)

> هاجر من type-system/S-TS-P10 بعد تحديد أن التدفّق نظام مستقلّ (لا تمثيل أنواع).

## القصة
بصفتي **مستخدم لغة ص**, أريد **داخل `إذا (س != لاشيء)` أن يُعامَل `س: T؟` كـ`T`** حتى **أصل لأعضائه دون `؟.` بعد الفحص (smart cast)** — كـKotlin/TypeScript/Dart.

## السياق
يُبنى في `NullSafetyAnalyzer` المشترك (NS-01)، لا في المدقّق الدلالي (غير مُفعَّل في sad-run). بيئة `NullFlowEnvironment` تتبّع قابلية null بالنطاق.

## معايير القبول
- **AC1 — التضييق:** داخل `إذا (س != لاشيء)` حيث `س: T؟` → `س` يُعامَل `T` في `then`؛ `else` يحتفظ بـ`T؟`.
- **AC2 — تضييق عكسي:** `إذا (س == لاشيء) { ارجع }` → بعد الكتلة `س` مُضيَّق إلى `T` (خروج فرع العدم).
- **AC3 — التراجع:** خارج الكتلة يعود `س` إلى `T؟` (AC3 من P10 الأصلي).
- **AC4 — أنماط:** `!= لاشيء`، `== لاشيء`، `؟؟`، التأكيد `!!` (NS-05) كنقاط تضييق.
- **AC5 — السياسة:** كل تشخيص عبر `dispatch()` (gc=تجاهل/learn=تحذير/prod=قاتل).
- **AC6 — DoD:** اختبار `.ص` إيجابي+سلبي، كلا المحرّكين أو `@skip_compiler` موثّق.

## المصدر في الكود (متوقّع)
- `shared/null_safety/` (جديد) أو ضمن `shared/ownership/`.
- استدعاء من `interpreter/src/core/interpreter_core.cpp` + `tools/compiler/compiler_driver_analysis.cpp`.

## تعريف "تم"
`س.حقل` داخل `إذا (س != لاشيء)` دون `؟.`، يتراجع خارجها، الصرامة بالسياسة، اختبارات خضراء.
