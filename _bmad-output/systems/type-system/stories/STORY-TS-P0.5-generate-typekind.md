---
id: S-TS-P0.5
title: "توليد SadTypeKind من types.yaml موسَّع (إنهاء خرق SoT)"
type: implementation-story
milestone: M1-Credibility
status: ready
estimate: L
depends_on: []
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
  - language-truth/types.yaml
  - language-truth/_schemas/type.schema.json
date: 2026-06-12
---

# S-TS-P0.5 — توليد SadTypeKind من types.yaml (الأساس المفقود)

> **قصّة أساس كشفها المستخدم:** الخطّة الأصلية قبلت أن `SadTypeKind` يدوي (خرق SoT). هذه القصّة تُصحّح الجذر: **الأنواع تُولَّد لا تُكتب يدويًّا** — فتُمنع الانحرافات (34/46/49) بنيويًّا.

## القصة
بصفتي **مشرف معمارية الأنواع**, أريد **توليد `SadTypeKind` (وما أمكن من السجلّ) من `types.yaml`** حتى **يصبح مصدر الحقيقة واحدًا، فلا انحراف بين YAML/الكود/الوثائق**.

## السياق
- `types.yaml` يحمل **9 أنواع سطحية** فقط؛ `SadTypeKind` يحمل **49 داخليًّا** — لا مصدر واحد للتصنيف الكامل.
- `gen_types.py` **مفقود** («فقدان بيانات سابق»، [codegen.cmake:6-13](../../../../cmake/codegen.cmake#L6)) ومُعطَّل في CMake. لكن `.pyc` مُخبَّأ موجود (`scripts/codegen/__pycache__/gen_types.cpython-314.pyc`) — **قابل للاسترجاع/إعادة البناء**.
- النتيجة: `SadTypeKind` يدوي = خرق القاعدة الذهبية (SoT-First)، ومصدر تضارب 34/46/49.

## معايير القبول
- **AC1 — توسعة SoT** `types.yaml` يُوسَّع ليحمل **كل** قيم النوع (49+) بحقول: `id`, `kind` (اسم enum), `category` (primitive/numeric/composite/algebraic/special/graphics…), `surface` (true إن له كلمة/رمز)، `description_ar/en`. مع مخطّط `type.schema.json` محدَّث.
- **AC2 — المولّد** `gen_types.py` (مُستعاد من `.pyc` أو مُعاد بناؤه) يُولّد `SadTypeKind` enum إلى `shared/types/generated/` (لا تحرير يدوي).
- **AC3 — تكامل CMake** `sad_add_codegen(types ...)` مُفعَّل في `cmake/codegen.cmake` (كان معطّلًا).
- **AC4 — لا انحراف** عدد القيم في YAML = enum المُولَّد = الوثائق (تُشتقّ من YAML) — يُمنع تضارب 34/46/49 بنيويًّا. (يجعل S-TS-P0 «تصحيح التعليق» بلا معنى — التعليق يُولَّد.)
- **AC5 — لا كسر** `SadTypeKind` المُولَّد مطابق للقيم الـ49 الحالية (ترتيب + أسماء) — بناء `sad-run`+`sadc` أخضر، لا انحدار.
- **AC6 — اختبار المولّد** اختبار بايثون (على نسق `test_gen_error_messages_v5`) يتحقّق من التوليد.

## المهام
- [x] إعادة بناء `gen_types.py` من جديد (على نسق `gen_keywords.py` + `_lib`) — قرار المستخدم: كتابة لا استرجاع.
- [x] توسعة `types.yaml` (9→**49** نوعًا، حقول غنية) + `type.schema.json` (سطحي+داخلي).
- [x] توليد `shared/types/generated/sad_type_kind_generated.h` + دمجه في `sad_type_system.h` عبر `#include`.
- [x] تفعيل كتلة Types Codegen في `cmake/codegen.cmake` + ضمّها لـ`sad_all_codegen`.
- [x] اختبار المولّد (`test_gen_types.py` — 6/6) + بناء `sad-run` أخضر.
- [x] تحديث `status/` بدليل (GR-01) — انظر [implementation_status](../status/implementation_status.md).

## سجل المطوّر (Dev Agent Record)

**ما نُفِّذ:** أصبح `SadTypeKind` (49 قيمة) **مُولَّدًا** من `language-truth/types.yaml` بدل التحرير اليدوي — إنهاء خرق SoT.

**الملفّات المتغيّرة (File List):**
- `scripts/codegen/gen_types.py` (جديد — المولّد)
- `scripts/codegen/test_gen_types.py` (جديد — 6 اختبارات)
- `language-truth/types.yaml` (9→49 نوعًا، حقول غنية: kind/category/surface/…)
- `language-truth/_schemas/type.schema.json` (مخطّط غني: سطحي+داخلي، token_type شرطي)
- `shared/types/generated/sad_type_kind_generated.h` (مُولَّد — مضموم)
- `shared/types/include/sad_type_system.h` (إزالة enum يدوي → `#include` المُولَّد)
- `cmake/codegen.cmake` (تفعيل كتلة types + `sad_all_codegen`)

**الأدلّة (GR-01):**
- التوليد: `[gen_types] ✓ 49 نوعًا → sad_type_kind_generated.h`.
- عدم انحدار: `diff` قيم enum المُولَّد مقابل الأصلي = **فارغ** (49 مطابقة بالترتيب).
- البناء: `sad-run.exe` يُبنى بـ EXIT 0.
- الاختبارات: `test_gen_types.py` 6/6 + `test_gen_error_messages_v5.py` 9/9 (لا انحدار).
- حيّ: `نوع(42)`=رقم، `نوع("..")`=نص، `نوع([..])`=مصفوفة.
- CMake: هدف `sad_types_codegen` يعمل (env OK).

**قرارات:** المولّد يُنتج الترويسة فقط (لا .cpp)؛ الترتيب محفوظ حرفيًّا (توافق ثنائي)؛ `عدم`/Null يبقى خارج الـenum حتى S-TS-P1.

## أثر على بقية القصص
- **S-TS-P0** (تصحيح تعليق 34→49): يصبح ثانويًّا — العدد يُولَّد.
- **S-TS-P1/P6** (تعديل enum): تمرّ عبر **YAML** بعد هذه القصّة (لا تحرير يدوي).
- **التوسّع للرسومات (ج3)**: أنواع `Color/Widget/Window/Event/Vector` تُسجَّل في YAML بفئة `graphics` — فيصبح توسيع نظام الرسومات = إضافة YAML + توليد، لا تعديل enum يدوي.

## المصدر في الكود
- `language-truth/types.yaml`, `language-truth/_schemas/type.schema.json`
- `scripts/codegen/__pycache__/gen_types.cpython-314.pyc` (مصدر الاسترجاع)
- `cmake/codegen.cmake:13` (التفعيل)
- `shared/types/include/sad_type_system.h:68-145` (enum الحالي — يصير مُولَّدًا)

## تعريف "تم"
`SadTypeKind` مُولَّد من `types.yaml`، CMake مُفعَّل، لا انحراف عددي، بناء+اختبارات خضراء، المُولَّد مضموم في الـcommit.
