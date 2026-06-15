---
id: S-TS-P7
title: "interning للأنواع المركّبة (مقارنة مؤشر صحيحة)"
type: implementation-story
milestone: M4-Cleanup
status: ready
estimate: M
depends_on: [S-TS-P2.5b]
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
date: 2026-06-12
---

# S-TS-P7 — interning للأنواع المركّبة

## القصة
بصفتي **مطوّر نواة الأنواع**, أريد **مقارنة `==` صحيحة للأنواع المركّبة** حتى **لا أعتمد على مقارنة مؤشر مكسورة**.

## السياق
`SadTypeRegistry` (`sad_type_system.h:1272-1476`) يطبّق interning للبدائيات/الخاصّة المخزّنة مسبقًا فقط. الأنواع المركّبة والمتقدّمة تُنشأ بـ`make_shared` جديدًا كل مرّة → مقارنة المؤشر `==` غير صحيحة (نوعان متطابقان بنيويًّا بمؤشرين مختلفين).

## معايير القبول
- **AC1 — Given** `makeArray(elem)` يُنشئ مؤشرًا جديدًا كل مرّة (الاسم الصحيح `makeArray:1326` لا `getArrayType` — تصحيح بعد نقد أميليا ع-8)
  **When** نضيف cache بنيويًّا (مفتاح = توقيع بنيوي) أو نستبدل المقارنة ببنيوية موثّقة
  **Then** `makeArray(int) == makeArray(int)` صحيح (مقارنة متماثلة).
- **AC2** اختبار يثبت تطابق المركّبة المتماثلة.
- **AC3** لا تسرّب ذاكرة (cache مُدار).

## المهام
- [ ] تصميم مفتاح توقيع بنيوي للأنواع المركّبة.
- [ ] cache في `SadTypeRegistry` للمركّبة.
- [ ] (أو) توثيق صريح أن المقارنة بنيوية لا مؤشرية + ضبط `operator==`.
- [ ] اختبار وحدة.

## المصدر في الكود
- `shared/types/include/sad_type_system.h:1272-1476` (SadTypeRegistry)
- `shared/types/include/sad_type_system.h:1326` (`makeArray` — الاسم الفعلي)

## تعريف "تم"
مقارنة المركّبة المتماثلة صحيحة + اختبار يثبتها + لا تسرّب.
