---
id: S-TS-P5
title: "إحياء suite اختبارات الأمان النوعي وربطه بـCI"
type: implementation-story
milestone: M1-Credibility
status: ready
estimate: M
depends_on: []
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
date: 2026-06-12
---

# S-TS-P5 — إحياء suite اختبارات الأمان النوعي

## القصة
بصفتي **فريق اللغة**, أريد **suite اختبارات نوعية حقيقيًّا مُشغَّلًا في CI** حتى **لا تُقدَّم ادّعاءات «تمرّ 100%» دون برهان**.

## السياق
ادّعت الوثيقة القديمة «34/34 P3 + 20 Type Safety» — غير قابل للإثبات: `tests/safety/` غير موجود، والملف الوحيد مؤرشف (`tests/_archive/safety/type_safety_regression.ص`)، ولا suite «P3» بذلك العدد.

## معايير القبول
- **AC1 — Given** الملف المؤرشف `tests/_archive/safety/type_safety_regression.ص`
  **When** نُحييه ضمن بنية الطبقات الثلاث الحالية (`tests/unit` أو `tests/behavior`)
  **Then** يعمل عبر `tests/runner.py` بنتيجة قابلة للاستشهاد.
- **AC2** التغطية تشمل البدائيات: `نوع(42)=رقم`, `نوع(3.14)=عشري`, `نوع("..")=نص`, `نوع(صحيح)=منطقي`, `نوع([..])=مصفوفة`.
- **AC3** يُربط بـCI (يظهر في بوّابة القرار).
- **AC4** لا أرقام وهمية — كل اختبار موجود ويمرّ فعلًا.

## المهام
- [ ] نقل/إحياء `type_safety_regression.ص` إلى مسار نشط.
- [ ] توسيعه ليغطّي مصفوفة البدائيات أعلاه.
- [ ] ربطه بـ`tests/runner.py` + CI.
- [ ] **إنشاء** `status/05_quality_gates.md` (غير موجود حاليًّا — تصحيح بعد نقد أميليا ع-7؛ `status/` فيه README.md فقط) وتوثيق العدد الحقيقي للاختبارات فيه.

## المصدر في الكود
- `tests/_archive/safety/type_safety_regression.ص`
- `tests/runner.py`, `tests/config.yaml`
- `_bmad-output/systems/type-system/status/` (يُنشأ فيه `05_quality_gates.md`)

## تعريف "تم"
suite مُشغَّل في CI بأرقام حقيقية + `status/05_quality_gates.md` مُنشأ بالأرقام الفعلية.
