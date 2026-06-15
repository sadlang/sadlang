---
project: s-programming-language
feature: EPIC-EM (Error Messages YAML Migration)
status: alignment-note
created: 2026-05-19
author: Amelia (bmad-agent-dev)
relates_to:
  - "_bmad-output/systems/doc-ir/ADR-006a_توحيد_codegen.md"
  - "_bmad-output/systems/doc-ir/ADR-006b-epics.md"
---

# مواءمة Epic رسائل الأخطاء مع نظام التوثيق الموحَّد

> **الغرض:** إثبات أن هذا الEpic ليس نظام توثيق منفصلاً، بل **تطبيق فرعي** لنظام `docplan` الموجود — يستخدم نفس البنية، نفس الفلسفة، نفس مولِّدات codegen.

---

## 1. النظامان المرئيان ظاهرياً

| البُعد | `_bmad-output/systems/doc-ir/` (الموجود) | `_bmad-output/systems/error-messages/` (هذا الEpic) |
|---|---|---|
| الموضوع | نظام التوليد الموحَّد (UTM-6) + توثيق Diátaxis | ترحيل رسائل الأخطاء إلى YAML |
| التنسيق | ADR-XXX + spec + epics + story-X.Y | PRD + Epic + Tech Spec + EM-N stories |
| النطاق | معماري شامل (cross-cutting) | تطبيقي محدَّد (single feature) |
| المصدر | bmad-create-epics-and-stories | bmad-agent-dev (Amelia) |

**الوهم:** يبدوان نظامين مختلفين.
**الحقيقة:** الثاني = **تطبيق عملي للأول** على ميزة واحدة.

---

## 2. لماذا يُعدّ هذا الEpic **امتداداً** لـ ADR-006a وليس بديلاً

[ADR-006a](../docplan/ADR-006a_توحيد_codegen.md) أنشأ **النظام الأم** لـ codegen:

```
data/language/*.yaml  →  scripts/codegen/_lib/{emit,loader}.py  →  scripts/codegen/gen_*.py  →  shared/**/generated/*_generated.{h,cpp}
```

هذا الEpic **يضيف عنصراً واحداً** لهذا النظام:

```
data/language/error_messages.yaml  →  scripts/codegen/gen_error_messages.py  →  shared/errors/generated/error_messages_generated.{h,cpp}
```

كل القرارات المعمارية المُلزِمة (D-1..D-5 في [PRD](prd-error-messages.md)) **مستمَدَّة مباشرة** من ADR-006a:

| القرار في EPIC-EM | المصدر في ADR-006a |
|---|---|
| استخدام `_lib/loader.py` و `_lib/emit.py` | §2.1 (الطبقة 1: المكتبة المشتركة) |
| YAML SOT + JSON Schema draft-07 | §2.2 (الطبقة 2: schema-driven) |
| `*.baseline` files committed | §2.3 (deterministic build) |
| CMake `add_custom_command` + `add_custom_target` | §2.4 (CMake orchestration) |
| ترتيب الإدخالات ثابت (CW-27) | §3.1 (deterministic ordering) |

→ **لا تكرار**: هذا الEpic لا يخترع آلية جديدة، بل **يستهلك** الآلية القائمة.

---

## 3. لماذا اخترنا تنسيق PRD+Epic+TechSpec بدل ADR

| الموقف | الأفضل |
|---|---|
| قرار معماري شامل يؤثر على عدة طبقات | **ADR** (مثل ADR-006a, 006b, 007) |
| feature واحدة تطبّق قراراً معمارياً موجوداً | **PRD + Epic + Stories** (هذا الEpic) |

ADR-006a **قرَّر** أن كل أسماء/بيانات اللغة تُرحَّل إلى YAML.
هذا الEpic **ينفِّذ** ذلك القرار على رسائل الأخطاء.

→ **لا حاجة لـ ADR جديد**: لا قرار معماري جديد يُتَّخذ هنا.

البديل الذي رفضناه: كتابة ADR-009 جديد. سبب الرفض: سيكرر ~80% من ADR-006a — انتهاك CW-19 (DRY).

---

## 4. التوافق مع stories الـUTM الموجودة

النظام الموجود يستخدم numbering هرمي: `story-utm-6.3.md`, `story-utm-6.4.md`, ...
هذا الEpic يستخدم: `EM-1.md`, ..., `EM-5.md`.

**اقتراح المواءمة (اختياري — لاحقاً):**

| الاسم الحالي | اقتراح الاسم الموحَّد |
|---|---|
| `EM-1-extract-sot.md` | `story-utm-9.1-extract-error-sot.md` |
| `EM-2-generator.md` | `story-utm-9.2-error-generator.md` |
| `EM-3-integration.md` | `story-utm-9.3-error-integration.md` |
| `EM-4-verification.md` | `story-utm-9.4-error-verification.md` |
| `EM-5-sadinfo.md` | `story-utm-9.5-sadinfo-errors.md` |

حيث `utm-9` = الرقم الفرعي التالي بعد UTM-6/7/8 الحالية.
**القرار الحالي:** الإبقاء على `EM-N` لتمييز Epic مستقل ذاتياً. التوحيد يمكن تأجيله للـrefactor النهائي.

---

## 5. أين تضع البحث/المراجعة المستقبلية

| تبحث عن... | اذهب إلى... |
|---|---|
| المعمارية العامة لـ codegen | [docplan/ADR-006a](../docplan/ADR-006a_توحيد_codegen.md) |
| توليد التوثيق (pipeline) | [docplan/ADR-006b-spec](../docplan/ADR-006b-spec.md) + [ADR-006b-epics](../docplan/ADR-006b-epics.md) |
| كيف تُرحَّل ميزة جديدة إلى YAML (مثال عملي) | **هذا الEpic** (eroor_system/) |
| رسائل الأخطاء تحديداً | [eroor_system/README.md](README.md) |

---

## 6. الخلاصة في سطرين

**لا يوجد نظامان للتوثيق** — يوجد:
1. **نظام معماري واحد** (`docplan/`) يقرِّر القواعد العامة (ADR-006a, b, 007, 008).
2. **تطبيقات عملية للقواعد** (`eroor_system/`, مستقبلاً `network_messages/`, إلخ) — كل تطبيق يحوي PRD + Epic + Stories.

**القاعدة:** ADR للقرارات المعمارية الجديدة، PRD/Epic للتطبيقات.
هذا الEpic **يُكرِّس** هذه الفلسفة ولا يكسرها.
