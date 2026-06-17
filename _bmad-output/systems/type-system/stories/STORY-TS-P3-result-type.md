---
id: S-TS-P3
title: "Result: من قيمة enum ميتة إلى تطبيق فعلي"
type: implementation-story
milestone: M3-AdvancedTypes
status: ready
estimate: M
depends_on: [S-TS-P2.5b]
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
date: 2026-06-12
---

# S-TS-P3 — Result نوع جبري مطبَّق

## القصة
بصفتي **مستخدم لغة ص**, أريد **`Result<T,E>` نوعًا حقيقيًّا** حتى **أمثّل النجاح/الخطأ دون أن يُسقَط إلى Void**.

## السياق
`Result` قيمة enum موجودة (`shared/types/include/sad_type_system.h`) **بلا** `SadResultType` ولا `makeResult`؛ الجسر يُسقطه إلى Void (`type_bridge.cpp:120`) — قيمة enum ميتة. ينبغي تنفيذه بعد حذف دوال DataType من الجسر (S-TS-P2.5b).

## معايير القبول
- **AC1 — Given** لا صنف `SadResultType`
  **When** ننشئه على نسق `SadOptionalType` (`sad_type_system.h:1018`)
  **Then** يُبنى ويُقارَن بنيويًّا.
- **AC2** `makeResult` في `SadTypeRegistry` + منطق `isAssignableTo`.
- **AC3** توصيل دلالي (المدقّق يتعرّف عليه).
- **AC4** اختبار وحدة `Result<T,E>` (مسار نجاح + مسار خطأ).

## المهام
- [ ] `SadResultType` (على نسق `SadOptionalType`).
- [ ] `makeResult` + `isAssignableTo`.
- [ ] توصيل دلالي + (إن لزم) المفسّر.
- [ ] اختبار وحدة في suite (S-TS-P5).

## المصدر في الكود
- `shared/types/include/sad_type_system.h:1018` (نموذج SadOptionalType)
- `shared/types/src/type_bridge.cpp:120`

## تعريف "تم"
`SadResultType` مبنيّ ومُتحقَّق منه + اختبار نجاح/خطأ يمرّ.
