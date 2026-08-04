---
epicId: EPIC-UI-YAML
title: "توحيد العناصر والمعدّلات والأخطاء في YAML (مصدر حقيقة واحد)"
status: PROPOSED
date: 2026-06-08
author: Amelia (bmad-agent-dev)
priority: P1
parentPlan: "خطة فرعية ضمن خطط نظام الرسومات المتعددة"
governanceAck: "قرأت 1-policy؛ آخر تحقق VERIFICATION_REPORT_2026-05-30؛ السبرنت SPRINT_CURRENT"
codeEvidence:
  - "keywords في data/language/keywords.yaml → keywords_generated.cpp (AUTO-GENERATED)"
  - "العناصر مُبرمَجة صلباً: shared/parser/src/ui/parser_ui.cpp (knownWidgets/deprecatedWidgets)"
  - "لا أنواع UI في language-truth/types.yaml (9 أنواع فقط)"
  - "لا أخطاء UI مخصّصة في data/language/error_messages.yaml"
---

> **تحديثُ حالة (لاحقٌ لهذه الوثيقة):** المقصدُ تحقّق بمسارٍ مختلف. مصدرُ حقيقةِ
> أسماءِ العُقَد هو `language-truth/ui_nodes.yaml` (لا `widgets.yaml` مستقلّ)، ومنه
> يُولَّد `shared/parser/include/generated/ui_parser_nodes_generated.h` فتُبنى منه
> `knownWidgets`/`containerWidgets`/`knownEvents` بلا اسمٍ حرفيٍّ في `parser_ui.cpp`.
> و`deprecatedWidgets` **حُذفت ولا تُولَّد**: قرارُ مالكٍ لاحقٌ ألغى التوافقَ الخلفيَّ
> كلَّه، فأيُّ ذكرٍ لها أدناه تاريخيٌّ لا مطلوب.

# EPIC-UI-YAML — توحيد عناصر الواجهة في YAML (مصدر حقيقة واحد)

## 1. الهدف

نقل تعريفات نظام الرسومات من كود C++ المُبرمَج صلباً إلى **YAML مولِّد للكود**، تماشياً مع
مبدأ «مصدر الحقيقة الواحد» (Single Source of Truth) المتبع في باقي المشروع (الكلمات،
الأنواع، الأخطاء كلها من YAML).

## 2. الواقع الحالي (فحص الكود 2026-06-08)

| العنصر | المصدر الحالي | متوافق مع SSoT؟ |
|--------|----------------|-------------------|
| الكلمتان `واجهة_تصريحية`/`اعرض` | ✅ `data/language/keywords.yaml` → codegen | ✅ نعم |
| أسماء العناصر الـ15 | ❌ `parser_ui.cpp` (`knownWidgets` صلب) | ❌ لا |
| العناصر المُهملة الـ55+ | ❌ `parser_ui.cpp` (`deprecatedWidgets` صلب) | ❌ لا |
| المعدّلات (`.حجم`/`.لون`/...) | ❌ مُبرمَجة في المعالجة | ❌ لا |
| أحداث UI (`knownEvents`) | ❌ `parser_ui.cpp` (صلب) | ❌ لا |
| تحذير الإهمال | ❌ `std::cerr` مباشر في `isKnownWidget` | ❌ لا |
| أنواع العناصر | ❌ غير موجودة في `types.yaml` | ❌ لا |
| أخطاء UI | ❌ تستخدم SEM001 العام، لا أكواد مخصّصة | ❌ لا |

> **الخلاصة:** نظام الرسومات هو **النظام الوحيد** الذي يكسر SSoT — تعريفاته مبعثرة في C++.

## 3. النطاق

### داخل النطاق
- إنشاء `data/language/widgets.yaml` (أو `language-truth/widgets.yaml`) يحوي:
  - العناصر الـ15 (الاسم، الفئة، حاوية/ورقي، الوسائط، المعدّلات المسموحة).
  - العناصر المُهملة الـ55+ (الاسم → البديل، مرحلة الإهمال).
  - المعدّلات وأنواع قيمها.
  - أحداث UI المعروفة.
- مولّد كود (codegen) يُنتج `knownWidgets`/`deprecatedWidgets`/`knownEvents` بدل التعريف الصلب.
- نقل تحذير الإهمال + أخطاء UI إلى `error_messages.yaml` (أكواد UIxxx ثنائية اللغة).
- (اختياري) تسجيل نوع `عنصر`/`واجهة` في `types.yaml`.

### خارج النطاق
- منطق التحليل نفسه (يبقى في parser، يقرأ من المُولَّد).
- الباطن (خطة منفصلة).

## 4. الستوريات

| المعرّف | العنوان | الأولوية |
|---------|---------|----------|
| [STORY-UI-YAML-01](../stories/STORY-UI-YAML-01-widgets-yaml-codegen.md) | إنشاء widgets.yaml + codegen للعناصر والمُهملة | P1 |
| [STORY-UI-YAML-02](../stories/STORY-UI-YAML-02-modifiers-events-yaml.md) | نقل المعدّلات والأحداث إلى YAML | P1 |
| [STORY-UI-YAML-03](../stories/STORY-UI-YAML-03-ui-errors-yaml.md) | أخطاء UI + تحذير الإهمال في error_messages.yaml | P2 |

## 5. معايير القبول للملحمة
- AC-E1: `knownWidgets`/`deprecatedWidgets`/`knownEvents` مولّدة من YAML (لا تعريف صلب في `parser_ui.cpp`).
- AC-E2: إضافة عنصر جديد = تعديل YAML + إعادة بناء (لا لمس C++).
- AC-E3: تحذير الإهمال وأخطاء UI من `error_messages.yaml` ثنائية اللغة.
- AC-E4: لا تراجع: كل اختبارات UI الحالية تمر بعد التوليد.

## 6. العلاقة بالملاحم الأخرى
- مكمّلة لـ [EPIC-UI-W15](EPIC-UI-W15-interpreter-compiler.md): W15 يُفعّل العناصر وظيفياً،
  وهذه الملحمة تُوحّد تعريفها في مصدر واحد. يُفضّل تنفيذ YAML أولاً ليبني W15 عليه.

## 7. المخاطر
| المخاطرة | التخفيف |
|----------|---------|
| تغيير بنية التوليد يكسر البناء | اتباع نمط `keywords.yaml` القائم تماماً (CW-04) |
| ازدواج مؤقت (YAML + صلب) | حذف الصلب فور التحقق من التوليد (BF-26) |

---
*خطة فرعية — Amelia (bmad-agent-dev) — 2026-06-08.*
