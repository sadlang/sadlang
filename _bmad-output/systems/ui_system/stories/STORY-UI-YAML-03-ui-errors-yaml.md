---
storyId: STORY-UI-YAML-03
epic: EPIC-UI-YAML
title: "أخطاء UI + تحذير الإهمال في error_messages.yaml"
status: READY
priority: P2
date: 2026-06-08
author: Amelia (bmad-agent-dev)
estimate: "4 ساعات"
dependsOn: [STORY-UI-YAML-01]
---

# STORY-UI-YAML-03 — أخطاء UI + تحذير الإهمال في error_messages.yaml

## القصة
بصفتي مطوّر تطبيقات، أريد رسائل خطأ وتحذيرات UI واضحة ثنائية اللغة من مصدر موحّد،
حتى تكون تجربة الأخطاء في الواجهات متسقة مع باقي اللغة.

## السياق التقني
- تحذير الإهمال مُبرمَج صلباً: `std::cerr` في `isKnownWidget` (`parser_ui.cpp`).
- لا أكواد خطأ UI مخصّصة في `data/language/error_messages.yaml` — تسقط على SEM001 العام.
- النمط المرجعي: أكواد LEX/SEM/PARSE الموجودة في `error_messages.yaml` (ثنائية اللغة + hint).

## معايير القبول
- AC-1: أكواد UI مخصّصة في `error_messages.yaml` (مثل: عنصر غير معروف، معدّل غير صالح، حدث مجهول).
- AC-2: تحذير الإهمال يُقرأ من YAML (لا `std::cerr` صلب) ويبقى ثنائي اللغة.
- AC-3: كل رسالة فيها `ar` + `en` + `hint` (متبعة نمط الأكواد القائمة).
- AC-4: لا تراجع — رسائل الأخطاء الأخرى سليمة.

## المهام
- [ ] T1: تعريف أكواد UI (UI001+) في `error_messages.yaml` بصيغة ثنائية اللغة.
- [ ] T2: نقل نص تحذير الإهمال إلى YAML + استبدال `std::cerr` بقراءة من سجل الرسائل.
- [ ] T3: ربط أخطاء المحلل النحوي للـ UI بالأكواد الجديدة.
- [ ] T4: اختبار: استخدام عنصر مُهمل يُصدر التحذير من YAML؛ عنصر مجهول يُصدر UI001.
- [ ] T5: اختبار تراجع لرسائل الأخطاء.

## Dev Agent Record
*(يُملأ أثناء التنفيذ)*

## File List
*(يُحدّث بعد كل مهمة)*
