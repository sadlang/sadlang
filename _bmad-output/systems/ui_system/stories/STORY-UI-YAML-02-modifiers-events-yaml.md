---
storyId: STORY-UI-YAML-02
epic: EPIC-UI-YAML
title: "نقل المعدّلات والأحداث إلى YAML"
status: READY
priority: P1
date: 2026-06-08
author: Amelia (bmad-agent-dev)
estimate: "5 ساعات"
dependsOn: [STORY-UI-YAML-01]
---

# STORY-UI-YAML-02 — نقل المعدّلات والأحداث إلى YAML

## القصة
بصفتي مشرف اللغة، أريد تعريف المعدّلات (`.حجم`/`.لون`/...) وأحداث UI في YAML،
حتى يكون نظام الرسومات كاملاً ضمن مصدر الحقيقة الواحد.

## السياق التقني
- `knownEvents` صلب في `parser_ui.cpp` (سطر ~172): 7 أحداث.
- `ModifierType` في `sad_ui/core/types.h`؛ المعالجة تربط الاسم العربي بالنوع برمجياً.
- النواة `IREventType` تدعم 33+ نوعاً — يجب أن يعكسها YAML.

## معايير القبول
- AC-1: `widgets.yaml` (أو ملف مرافق) يحوي قائمة المعدّلات (الاسم، نوع القيمة، العناصر المسموح عليها).
- AC-2: يحوي قائمة الأحداث المعروفة (الاسم العربي → `IREventType`).
- AC-3: `knownEvents` يُولَّد من YAML بدل الصلب.
- AC-4: لا تراجع — سلاسل المعدّلات والأحداث في `test_event_system.ص` تعمل.

## المهام
- [ ] T1: توسيع مخطط YAML بقسمي `modifiers` و`events`.
- [ ] T2: نقل الأحداث الـ7 + ربطها بـ 33 نوع IREventType.
- [ ] T3: نقل المعدّلات من `ModifierType` (مع نوع القيمة لكل معدّل).
- [ ] T4: توليد `knownEvents` + جدول المعدّلات؛ استبدال الصلب.
- [ ] T5: اختبار تراجع.

## Dev Agent Record
*(يُملأ أثناء التنفيذ)*

## File List
*(يُحدّث بعد كل مهمة)*
