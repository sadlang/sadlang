---
id: S-TS-P2.5b
title: "حذف دوال DataType من الجسر (لا الجسر بالكامل)"
type: implementation-story
milestone: M2-CoreSafety
status: ready
estimate: M
depends_on: [S-TS-P2, S-TS-P2.5a]
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
date: 2026-06-12
---

# S-TS-P2.5b — حذف دوال DataType من الجسر

> **تصحيح بعد نقد أميليا (ع-6):** الجسر **ليس** كله DataType. `type_bridge.cpp` فيه ثلاثة أقسام:
> - **DataType** (يُحذف): `toDataType:188`, `fromDataType:235`, `sadTypeToDataType:290`, `sadTypeFromDataType:299`.
> - **ValueType** (يبقى): `toValueType:55`, `fromValueType:160`, `sadTypeToValueType:277` — مستهلَك في الـruntime.
> - **runtime** (يبقى): `inferSadType:308`, `isValueCompatible:379` — لا علاقة لهما بـDataType.
> إذًا الهدف **حذف دوال DataType فقط**، لا الجسر بالكامل.

## القصة
بصفتي **مشرف معمارية الأنواع**, أريد **حذف دوال تحويل `DataType` من `type_bridge` و`data_types.h`** حتى **يكتمل التخلّص من النظام الأفقر دون كسر مستهلكي ValueType/runtime**.

## السياق
بعد S-TS-P2.5a (ترحيل الـAST/المحلّل) وS-TS-P2 (الدلالات)، تُصبح استدعاءات `sadTypeFromDataType`/`sadTypeToDataType`/`toDataType`/`fromDataType` صفرًا، فيصير حذفها آمنًا.

## معايير القبول
- **AC1 — Given** صفر مستهلك لدوال DataType في الجسر (بعد P2.5a + P2)
  **When** نحذف `toDataType`/`fromDataType`/`sadTypeToDataType`/`sadTypeFromDataType` من `type_bridge.{h,cpp}`
  **Then** البناء أخضر.
- **AC2** `data_types.h` يُحذف إن صار بلا مستهلك (`grep -rl DataType` = صفر خارج الأرشيف).
- **AC3 — صريح** أقسام ValueType (`toValueType`/`fromValueType`/`sadTypeToValueType`) و`inferSadType`/`isValueCompatible` **تبقى** سليمة.
- **AC4 — إثبات حيّ** نوع متقدّم (Optional) يعبر المصدر→AST→تنفيذ دون إسقاط إلى Void (لم يَعُد يمرّ بدالة DataType مُسقِطة).
- **AC5** بناء `sad-run` + المترجم = 0، كل الاختبارات خضراء.

## المهام
- [ ] التحقّق `grep -c` أن استدعاءات دوال DataType = 0.
- [ ] حذف الدوال الأربع من `type_bridge.h` + `type_bridge.cpp`.
- [ ] حذف `data_types.h` إن صار بلا مستهلك (وإلا توثيق المتبقّي).
- [ ] (اختياري) إزالة ثوابت `namespace ValueType` (`value.h:83-99`) بحثًا/استبدالًا.
- [ ] بناء + اختبارات + إثبات حيّ Optional.

## المصدر في الكود
- `shared/types/src/type_bridge.cpp:188,235,290,299` (DataType — يُحذف)
- `shared/types/src/type_bridge.cpp:55,160,277,308,379` (ValueType/runtime — يبقى)
- `shared/types/include/data_types.h:26`
- `shared/types/include/value.h:83-99`

## تعريف "تم"
دوال DataType محذوفة، أقسام ValueType/runtime سليمة، إثبات حيّ لعبور Optional، بناء+اختبارات خضراء.
