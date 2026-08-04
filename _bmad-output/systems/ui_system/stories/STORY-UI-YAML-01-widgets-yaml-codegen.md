---
storyId: STORY-UI-YAML-01
epic: EPIC-UI-YAML
title: "إنشاء widgets.yaml + codegen للعناصر والمُهملة"
status: READY
priority: P1
date: 2026-06-08
author: Amelia (bmad-agent-dev)
estimate: "6 ساعات"
---

> **تحديثُ حالة (لاحقٌ لهذه الوثيقة):** المقصدُ تحقّق بمسارٍ مختلف. مصدرُ حقيقةِ
> أسماءِ العُقَد هو `language-truth/ui_nodes.yaml` (لا `widgets.yaml` مستقلّ)، ومنه
> يُولَّد `shared/parser/include/generated/ui_parser_nodes_generated.h` فتُبنى منه
> `knownWidgets`/`containerWidgets`/`knownEvents` بلا اسمٍ حرفيٍّ في `parser_ui.cpp`.
> و`deprecatedWidgets` **حُذفت ولا تُولَّد**: قرارُ مالكٍ لاحقٌ ألغى التوافقَ الخلفيَّ
> كلَّه، فأيُّ ذكرٍ لها أدناه تاريخيٌّ لا مطلوب.

# STORY-UI-YAML-01 — إنشاء widgets.yaml + codegen

## القصة
بصفتي مشرف اللغة، أريد تعريف العناصر الـ15 والمُهملة الـ55+ في `widgets.yaml` يولّد كوداً،
حتى لا تبقى مُبرمَجة صلباً في `parser_ui.cpp` (التزام SSoT).

## السياق التقني
- النمط المرجعي: `data/language/keywords.yaml` → `keywords_generated.cpp` (AUTO-GENERATED).
- الهدف الحالي الصلب: `parser_ui.cpp` `knownWidgets` (سطر ~67) + `deprecatedWidgets` (~100).

## معايير القبول
- AC-1: `data/language/widgets.yaml` يحوي 15 عنصراً (اسم، فئة، حاوية/ورقي) + 55+ مُهملاً (اسم→بديل، مرحلة).
- AC-2: مولّد يُنتج `widgets_generated.{h,cpp}` بـ `knownWidgets`/`deprecatedWidgets`/`containerWidgets`.
- AC-3: `parser_ui.cpp` يقرأ من المُولَّد بدل التعريف الصلب.
- AC-4: لا تراجع — اختبارات UI تمر؛ تحذير الإهمال يبقى يعمل.

## المهام
- [ ] T1: تصميم مخطط `widgets.yaml` (متبعاً نمط keywords.yaml).
- [ ] T2: تعبئة الـ15 + الـ55+ من القيم الصلبة الحالية (نقل أمين، لا تغيير دلالة).
- [ ] T3: كتابة مولّد Python (مثل codegen الكلمات) + ربطه بـ cmake.
- [ ] T4: استبدال التعريف الصلب في `parser_ui.cpp` بالمُولَّد + حذف الصلب (BF-26).
- [ ] T5: اختبار تراجع شامل (BF-29).

## Dev Agent Record
*(يُملأ أثناء التنفيذ)*

## File List
*(يُحدّث بعد كل مهمة)*
