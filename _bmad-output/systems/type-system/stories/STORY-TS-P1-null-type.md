---
id: S-TS-P1
title: "Null (عدم) نوع متمايز عن Void (فراغ)"
type: implementation-story
milestone: M2-CoreSafety
status: ready
estimate: M
depends_on: [S-TS-P0]
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
  - language-truth/types.yaml
date: 2026-06-12
---

# S-TS-P1 — Null (عدم) نوع متمايز عن Void (فراغ)

## القصة
بصفتي **مستخدم لغة ص**, أريد **`نوع(لاشيء)` يُرجع «عدم» لا «فراغ»** حتى **يصبح أمان Null حقيقيًّا ومميَّزًا عن Void، متوافقًا مع SoT**.

## السياق + التزام SoT-First (م-1, م-3)
> **تصحيح التزام بعد مراجعة مهارة sad-lang-dev:** SoT الأنواع هو `language-truth/types.yaml`، وفيه **`type.null` بكلمة «عدم»** (`token_type: TYPE_NULL`, `category: special`) **منفصلًا فعلًا** عن `type.void` («فراغ»). إذًا اسم نوع null الصحيح **«عدم»** لا «لاشيء» (النسخة الأولى كانت مخالفة للـSoT). وlexer يؤكّد: `TYPE_NULL`/`LITERAL_NULL` = «عدم / null» (`shared/lexer/include/token.h:303,311`).

الفجوة: **الـSoT يميّز null عن void، لكن التطبيق C++ لا يفعل.** لا توجد قيمة `SadTypeKind::Null` في التعداد؛ `LITERAL_NULL → Value()` = Void (`interpreter/src/visitors/expression_evaluator_core.cpp:278-279`)؛ التأكيدات تطابق `لاشيء→VOID` (`interpreter/src/builtins/builtin_module_assertions.cpp:533`). أمان Null = 0%. **هذه القصّة تُحاذي التطبيق مع الـSoT القائم** (لا تخترع نوعًا جديدًا — types.yaml يملكه أصلًا).

> **ملاحظة SoT-First (م-3):** `gen_types.py` **معطّل** (`cmake/codegen.cmake:237`)، فـ`SadTypeKind` في `sad_type_system.h` **يدوي لا مُولَّد** — يُحرَّر مباشرة (لا خرق توليد). لكن `types.yaml` يبقى المرجع الدلالي للأنواع السطحية؛ إن لزم حقل/وسم جديد يُحدَّث فيه أولًا.
> **دالة التعريب (ع-3):** `getTypeName()` يُرجع «VOID» إنجليزيًّا — مسار مختلف عن `نوع()` العربي. AC يخصّ مُسقِط `SadTypeKind`→الاسم العربي المسؤول عن `نوع()` (يُحدَّد في `interpreter/src/builtins/`).
> **ترتيب الجسر (ع-4):** لمس P1 للجسر محصور بقسم **ValueType/`inferSadType`** الباقي (لا دوال DataType التي يحذفها S-TS-P2.5b). نفّذ P1 قبل P2.5b.

## قرار معماري مطلوب
`عدم` (null) نوع بدائي/خاصّ مستقلّ `SadTypeKind::Null` يتوافق لاحقًا مع `Optional`. **ADR إلزامي** في `decisions/` (GR-02: لا حذف ADR لاحقًا).

## تحليل الأثر (delivery-checklist الجزء أ)
- **المفسّر** ✅ يتغيّر: بناء Null عند `LITERAL_NULL`، تعريب الاسم.
- **المترجم sadc** ✅ يجب فحصه: هل SIR/LLVM يميّز null عن void؟ إن لا → `@skip_compiler` موثّق أو إصلاح codegen (BF-08).
- **الجسر** ✅ قسم ValueType فقط.
- **sadinfo/LSP** ⚠️ يستهلكان types.yaml — `type.null` موجود فلا تغيير YAML متوقّع؛ يُتحقّق أن sadinfo يعكس «عدم».
- **التوافق الخلفي** (CW-24): إضافة قيمة enum مسموحة؛ تغيير معنى Void ممنوع.

## معايير القبول
- **AC1 — Given** enum بلا `Null`
  **When** نضيف `SadTypeKind::Null` (يصبح 50 قيمة) ونعالجه في `SadPrimitiveType`/السجلّ
  **Then** بناء `sad-run` (Debug) + `sadc` (Release) أخضر.
- **AC2** مُسقِط الاسم العربي لـ`نوع()` يُرجع «عدم» لـ Null (لا «فراغ»/«VOID»).
- **AC3** تحديث قسم ValueType من الجسر + تأكيد `builtin_module_assertions.cpp:533` (`{"عدم", ...}` لا «VOID»).
- **AC4 — إثبات حيّ (المفسّر + المترجم)** `نوع(لاشيء)` == «عدم»؛ `لاشيء == لاشيء` صحيح؛ `لاشيء` متمايز عن قيمة Void فارغة — في `sad-run` **و** `sadc` (أو `@skip_compiler` موثّق).
- **AC5** تحديث تعليق العدد (49→50) — تنسيق مع S-TS-P0.
- **AC6 — اختبار** `.ص` إيجابي + سلبي بصيغة `@expected` في `tests/behavior/sections/<قسم الأنواع>/`، و`runner.py --level P0 + P1` أخضر بلا تراجع.

## المهام
- [ ] ADR قرار Null مستقلّ في `decisions/` (GR-02) — موجود: [ADR-TYPESYSTEM-001](../decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md).
- [ ] إضافة `SadTypeKind::Null` + معالجة السجلّ.
- [ ] `LITERAL_NULL` يبني Null لا `Value()` الفارغ (`expression_evaluator_core.cpp:278`).
- [ ] مُسقِط `نوع()` → «عدم» (تحديد الدالة في `interpreter/src/builtins/`).
- [ ] **إعادة إرساء العوامل القائمة على `isNull`**: `visitOptionalChainExpr`/`visitNullCoalesceExpr` يفحصان `isVoid()` حاليًّا (`members_advanced.cpp:686,739`) — يجب أن يفحصا `isNull()`. (انحدار محتمل — اختبار إلزامي.)
- [ ] **تصحيح `isAssignableTo`**: الشرط `kind_==Void` (`sad_type_system.h:1501`) يُكمَّل/يُستبدَل بـ`Null`.
- [ ] تحديث قسم ValueType من الجسر + التأكيدات.
- [ ] فحص codegen المترجم (sadc) لتمييز null/void.
- [ ] اختبارات Null (إيجابي+سلبي) في suite (S-TS-P5) + runner.py.
- [ ] تحديث `status/` بدليل فعلي (grep/build) بعد الإنجاز (GR-01).

## المصدر في الكود
- `language-truth/types.yaml` (`type.null` — SoT)
- `shared/lexer/include/token.h:303,311` (TYPE_NULL/LITERAL_NULL = «عدم»)
- `shared/types/include/sad_type_system.h:71`
- `interpreter/src/visitors/expression_evaluator_core.cpp:278-279`
- `shared/types/src/value.cpp:1458` (getTypeName)
- `interpreter/src/builtins/builtin_module_assertions.cpp:533`
- مُسقِط `نوع()` في `interpreter/src/builtins/` (يُحدَّد)

## تعريف "تم" (delivery-checklist الجزء ب)
`نوع(لاشيء)`→«عدم» حيًّا في المفسّر **والمترجم** + اختبار إيجابي/سلبي يمرّ (P0+P1) + بناء Debug/Release نظيف + ADR موثّق + `status/` محدّث بدليل + قائمة ملفّات وسجل مطوّر.
