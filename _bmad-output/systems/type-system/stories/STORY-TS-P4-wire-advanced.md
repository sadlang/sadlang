---
id: S-TS-P4
title: "توصيل Optional/Future/Generator بالمفسّر والدلالات"
type: implementation-story
milestone: M3-AdvancedTypes
status: ready
estimate: L
depends_on: [S-TS-P2.5b]
governed_by:
  - ../docs/TYPE_SYSTEM_COMPLETION_PLAN.md
date: 2026-06-12
---

# S-TS-P4 — توصيل الأنواع المتقدّمة (هياكل غير موصولة)

## القصة
بصفتي **مستخدم لغة ص**, أريد **Optional/Future/Generator مستعملة فعليًّا** حتى **لا تبقى هياكل ميتة معرّفة بلا استخدام**.

## السياق
الأصناف موجودة: `SadOptionalType` (`sad_type_system.h:1018`)، `SadFutureType` (`:1174`)، `SadGeneratorType` (`:1217`) + دوالها `makeOptional:1387`/`makeFuture:1407`/`makeGenerator:1412`، لكن **بلا استخدام** في المفسّر/الدلالات/codegen. `shared/semantic` لا يذكرها إطلاقًا (grep=صفر). يُجنى أثرها بعد حذف دوال الجسر (S-TS-P2.5b).
> ملاحظة (ع-11): `makeResult` **غير موجودة** — `Result` ينشئها S-TS-P3 لا هذه القصّة.

## معايير القبول
لكل نوع (Optional، Future، Generator):
- **AC1 — Given** الصنف موجود بلا استخدام
  **When** نبنيه عند نقطة الإنشاء في المفسّر
  **Then** القيمة تحمل النوع الصحيح حيًّا.
- **AC2** تحقّق دلالي (المدقّق يتعرّف عليه).
- **AC3** (للمترجم) ربط codegen — أو توثيق صريح «غير مدعوم في codegen بعد».
- **AC4** اختبار حيّ لكل نوع.
- **AC5 — تحليل لاحقة `?` على الأنواع (فجوة كشفها نقد أميليا v2)** `parseType()` ([parser_helpers.cpp:1683](../../../../shared/parser/src/core/parser_helpers.cpp#L1683)) يقرأ لاحقة `?` بعد اسم النوع ويبني `OptionalTypeNode` ([type_nodes.h:239](../../../../shared/ast/include/type_nodes.h#L239) — ميت حاليًّا) → `SadOptionalType`. بدونها لا يمكن **كتابة** `رقم?` في المصدر (يبقى Optional حبيس الإنشاء البرمجي).
- **AC6 — فضّ غموض الثلاثي** تمييز `T?` (موضع نوع) عن `أ ? ب : ج` (تعبير) — قد يحتاج lookahead؛ يُصمَّم ويُختبَر (لا يُفترَض «موضع النوع كافٍ»).
- **AC7 — ملكية `isAssignableTo` (حسم ازدواج كشفته أميليا)** هذه القصّة **تملك حصريًّا** تصحيح `SadType::isAssignableTo` ([sad_type_system.h:1501](../../../../shared/types/include/sad_type_system.h#L1501)): الشرط `kind_==Void` يُكمَّل/يُستبدَل بـ`Null` (`Null <: T?`، `T <: T?`). تُستدعى من الدلالات (`type_checker.cpp:267`) والمفسّر (`:273` وغيره) — فالتصحيح هنا يخدمهما. (P9 يملك `areTypesCompatible`/استنتاج `لاشيء` — دالّة مختلفة، لا ازدواج.)

## المهام
- [ ] **`parseType` يستهلك لاحقة `?` + يبني `OptionalTypeNode` (إحياء العقدة الميتة)** + فضّ غموض الثلاثي.
- [ ] Optional: بناء + تحقّق + اختبار (بما فيه `رقم?` كتابةً في `.ص`).
- [ ] Future: ربط بنظام async (`async_runtime`) + اختبار.
- [ ] Generator: ربط بالمولّدات + اختبار.
- [ ] تحديث جدول التصنيف في `ARCHITECTURE_TYPE_SYSTEM.md` (هيكل فقط → مطبَّق).

## المصدر في الكود
- `shared/types/include/sad_type_system.h:1018,1174,1217`
- `shared/parser/src/core/parser_helpers.cpp:1610-1691` (`parseType` — يحتاج لاحقة `?`)
- `shared/ast/include/type_nodes.h:239` (`OptionalTypeNode` — ميت، يُحيا)
- `compiler/src/types/future_type.cpp`, `generator_type.cpp`
- `interpreter/src/...` (نقاط الإنشاء)

## تعريف "تم"
كل نوع يُبنى ويُتحقّق منه ويُختبر حيًّا + تحديث جدول التصنيف.
