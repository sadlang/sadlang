---
storyId: STORY-UI-W15-04
epic: EPIC-UI-W15
title: "دعم المترجم لـ«واجهة» عبر SIR"
status: IN_PROGRESS
priority: P0
date: 2026-06-19
author: Amelia (bmad-agent-dev)
estimate: "8 ساعات"
dependsOn: [STORY-UI-W15-02, STORY-UI-W15-03]
---

# STORY-UI-W15-04 — دعم المترجم لـ`واجهة` عبر SIR

## القصة
بصفتي مطوّر تطبيقات، أريد أن يترجم `sadc` مكوّنات `واجهة` إلى SIR/LLVM مثل المفسّر،
حتى يُنتج المكوّن المُصرَّح ثنائيًّا تنفيذيًّا بدل أن يُتخطّى بصمت.

## السياق التقني (من الكود)
- المفسّر يسجّل `UIDeclarationNode` كصنف عبر `visitUIDeclaration`
  (`statement_executor_oop_struct_test.cpp:503`): `@حالة`→حقول، الدوال→طُرق، `يرث`→أب.
- في المترجم، دوال زائر UI فارغة في `ASTVisitorBase`
  (`ast_visitor.h:1097` وما حولها)، فيتخطّى `SIRBuilder` عقد `واجهة` **بصمت** — لا خطأ،
  لكن **لا صنف**، فلا مخرجات.
- منطق بناء الأصناف في الواجهة الأمامية موجود ومُختبَر (`builders/class_main.cpp`،
  `SIRClass`)، فالحل الأنظف إعادة استخدامه (CW-19/CW-20) لا بناء مسار UI مستقل.

## معايير القبول
- AC-1: `sadc` لا يفشل على عقدة UI غير مدعومة (لا تخطٍّ صامت يُنتج لا شيء).
- AC-2: `UIWidgetExprNode` يُخفَض إلى نداء بناء العنصر في SIR.
- AC-3: `UIModifierNode` يُخفَض إلى ضبط خاصية.
- AC-4: تركيب `ClassDecl` مكافئ من `واجهة` (حقول `@حالة` + الطُرق + وراثة `يرث`).
- AC-5: `UIConditionalNode`/`UILoopNode` يُخفَضان إلى تحكّم تدفّق في SIR.
- AC-6: تكافؤ شجرة المفسّر↔المترجم لنفس مصدر `واجهة`.

## المهام/المهام الفرعية
- [x] T1 (AC-1+AC-4): **Phase 0** في `SIRBuilder::buildModule` — خفض كل
  `UIDeclarationNode` على المستوى الأعلى إلى `ClassDecl` في مكانه قبل المرحلة 1.
  - [x] `synthesizeClassFromUI` + `uiTypeNameToKind` (تحويل أسماء الأنواع العربية).
  - [x] إعادة استخدام مراحل بناء الأصناف القائمة (1.3/1.35/2A) دون مسار جديد.
  - [x] اختبار وحدة `test_ui_sir_lowering` (9/9): صنف + حقل @حالة + طريقة + وراثة.
- [ ] T2 (AC-2): خفض `UIWidgetExprNode` إلى نداء بناء العنصر.
- [ ] T3 (AC-3): خفض `UIModifierNode` إلى ضبط خاصية.
- [ ] T4 (AC-5): خفض `UIConditionalNode`/`UILoopNode` إلى تحكّم تدفّق.
- [ ] T5 (AC-6): إثبات تكافؤ الشجرة بين المفسّر والمترجم.
- [ ] T6: مكتبة UI زمن التشغيل + اختبار من طرف لطرف عبر `sadc` (محجوب: ربط Debug).

## Dev Agent Record
**النتيجة (2026-06-19) — زيادة أولى (AC-1 + AC-4):** أُضيفت **Phase 0** إلى
`SIRBuilder::buildModule`: تمريرة تسبق المرحلة 1 تستبدل كل `UIDeclarationNode` على
المستوى الأعلى بـ`ClassDecl` مكافئ (`stmt = synthesizeClassFromUI(*uiNode)`)، فتعالجها
مراحل بناء الأصناف القائمة. هذا يُبطل التخطّي الصامت (AC-1) ويُنتج صنف SIR كاملًا (AC-4).

**التحويل (`synthesizeClassFromUI`):**
- `stateDecls` (`@حالة`) → `FieldDecl` عام؛ نوعه عبر `uiTypeNameToKind`
  (رقم/عشري/منطقي/نص/مصفوفة → Integer/Float/Boolean/String/Array، وإلا Unknown يُستنتج لاحقًا).
- `methods` (`FunctionDecl`) → `MethodDecl` عام.
- `parentName` (`يرث`) → الصنف الأب في باني `ClassDecl` ذي الأب الواحد.

**الدليل (GR-01):**
- `compiler/src/frontend/sir_builder_module.cpp` — مساعِدا `uiTypeNameToKind` +
  `synthesizeClassFromUI` (namespace مجهول) + حلقة Phase 0 في `buildModule`.
- اختبار الوحدة `tests/unit/comprehensive/test_ui_sir_lowering.cpp` (9/9 ينجح):
  `getClass("عداد")` ≠ null، `getField("عدد")` ≠ null، `getMethod("عداد.زد")` ≠ null،
  `عداد_مزدوج.getParentClass()=="عداد"`، `getMethod("عداد_مزدوج.زد_مرتين")` ≠ null.
- مُسجَّل في `cmake/tests_comprehensive.cmake` (يُربط بـ`sad_core` + `sad_frontend`).

**ملاحظات تقنية:**
- الطُرق تُخزَّن في `SIRClass::methods_` باسم مؤهَّل `الصنف.الطريقة` (اصطلاح الأصناف
  القائم، `class_main.cpp:346`)، لا باسم مجرّد — فاللوكب يستخدم الاسم المؤهَّل.
- المحلّل يجرّد التشكيل من المُعرّفات (`زِد`→`زد`).
- **حاجز بيئيّ:** `sadc` لا يُربَط في تهيئة Debug (خطأ بيئة LLVM)، فالتحقّق من طرف لطرف
  (T6) غير متاح في هذه البيئة؛ الزيادة مُختبَرة على مستوى `sad_frontend` (يُبنى في Debug).

**الحالة:** IN_PROGRESS — AC-1 + AC-4 منجزان ومُختبَران؛ AC-2/3/5/6 + T6 متبقّية.

## File List
- `compiler/src/frontend/sir_builder_module.cpp` — Phase 0 + المساعِدان — مُعدَّل
- `tests/unit/comprehensive/test_ui_sir_lowering.cpp` — اختبار خفض UI→SIR — جديد
- `cmake/tests_comprehensive.cmake` — تسجيل الهدف (sad_core + sad_frontend) — مُعدَّل
