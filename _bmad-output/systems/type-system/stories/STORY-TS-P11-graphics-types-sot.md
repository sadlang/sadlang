---
id: S-TS-P11
title: "تسجيل أنواع الرسومات في SoT (إثبات قابلية التوسّع)"
type: implementation-story
milestone: M4-Cleanup
status: ready
estimate: M
depends_on: [S-TS-P0.5, S-TS-P6]
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
  - language-truth/types.yaml
date: 2026-06-12
---

# S-TS-P11 — تسجيل أنواع الرسومات في SoT

> **يجيب سؤال المستخدم ج3:** هل يتوسّع نظام الأنواع للرسومات؟ هذه القصّة **تُثبت** أن التوسّع = إضافة YAML + توليد (بعد P0.5)، لا تعديل enum يدوي.

## القصة
بصفتي **مطوّر نظام الرسومات في ص**, أريد **أنواع الرسومات (Color/Widget/Window/Event/Vector وما يلزم) مُسجَّلة في `types.yaml` ومُولَّدة** حتى **أوسّع نظام الرسومات دون تعديل enum يدوي ولا خرق SoT**.

## السياق
- الـenum فيه قسم «أنواع الرسومات» ([sad_type_system.h:132-136](../../../../shared/types/include/sad_type_system.h#L132)): Color, Widget, Window, Event (+ Vector) — **بلا SoT سطحي** (لا كلمات في `types.yaml`/`keywords.yaml`).
- يوجد مجلد `graphics/` (حاليًّا `third_party/stb_truetype.h` فقط) — بنية تحتية مبدئية.
- يعتمد على **S-TS-P0.5** (التوليد): بدونه كل نوع رسومي = تعديل enum يدوي = خطر انحراف.

## معايير القبول
- **AC1 — تسجيل SoT** أنواع الرسومات القائمة (Color/Widget/Window/Event/Vector) تُسجَّل في `types.yaml` بفئة `graphics`، مع `description_ar/en` و`surface` (هل لها كلمة؟).
- **AC2 — توليد** تُولَّد ضمن `SadTypeKind` عبر `gen_types.py` (P0.5) — لا تعديل enum يدوي.
- **AC3 — قرار السطح** لكل نوع رسومي: هل يحتاج كلمة سطحية (مثل `لون`) + token؟ إن نعم → يُسجَّل في `keywords.yaml` (تشابك lexer/parser). إن لا → نوع داخلي فقط.
- **AC4 — قابلية التوسّع المُثبَتة** إضافة نوع رسومي جديد (مثل `Point`/`Rect` تجريبيًّا) = مدخل YAML + توليد + بناء أخضر — **دون لمس C++ يدويًّا** (إثبات حيّ للمبدأ).
- **AC5 — DoD §0.5** بناء `sad-run`+`sadc`، لا انحدار، اختبار.

## المهام
- [ ] تسجيل أنواع الرسومات الخمسة في `types.yaml` (فئة graphics).
- [ ] قرار السطح لكل نوع (كلمة + token أم داخلي).
- [ ] توليد + بناء (يثبت أن enum يحوي الأنواع دون تحرير يدوي).
- [ ] إثبات حيّ: إضافة `Point` تجريبيًّا من YAML فقط.
- [ ] تحديث جدول التصنيف في `ARCHITECTURE_TYPE_SYSTEM.md`.

## المصدر في الكود
- `language-truth/types.yaml` (فئة graphics)
- `shared/types/include/sad_type_system.h:132-136,244-250`
- `graphics/` (البنية التحتية)

## تعريف "تم"
أنواع الرسومات مُسجَّلة في SoT ومُولَّدة، إضافة نوع جديد من YAML فقط تعمل، لا انحدار = **إثبات قابلية التوسّع عبر التوليد**.
