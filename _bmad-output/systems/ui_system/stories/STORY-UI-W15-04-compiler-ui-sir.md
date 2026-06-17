---
storyId: STORY-UI-W15-04
epic: EPIC-UI-W15
title: "دعم المترجم: زيارة عقد UI في SIR→LLVM"
status: READY
priority: P1
date: 2026-06-08
author: Amelia (bmad-agent-dev)
estimate: "10 ساعات"
dependsOn: [STORY-UI-W15-01, STORY-UI-W15-02]
---

# STORY-UI-W15-04 — دعم المترجم: زيارة عقد UI في SIR→LLVM

## القصة
بصفتي مطوّر تطبيقات، أريد أن يترجم `sadc` ملفاً فيه `واجهة_تصريحية` وعناصر إلى ملف
تنفيذي أصلي، حتى أبني تطبيقات واجهة مُترجمة (لا مفسَّرة فقط).

## السياق التقني (من الكود)
- **المترجم خالٍ تماماً من دعم UI:** `grep visitUIWidget|UIDeclarationNode compiler/` = فارغ.
- مسار المترجم: AST → SIRBuilder → SIROptimizer → LLVMCodeGen (`compiler/src/{frontend,backend}`).
- عقد AST الجاهزة: `UIDeclarationNode`, `UIWidgetExprNode`, `UIModifierNode`,
  `UIEventHandlerNode`, `UIConditionalNode`, `UILoopNode`, `UIStateDecl`.

## النهج (حد أدنى صحيح — لا تحسين سابق لأوانه، CW-30)
ترجمة شجرة UI إلى **استدعاءات وقت تشغيل** (runtime calls) تبني الشجرة، بدل توليد رسم
مضمّن. أي: `زر("x")` → استدعاء دالة وقت تشغيل `sad_ui_make_button("x")` تعيد مؤشر عقدة.

## معايير القبول
- AC-1: `sadc ملف_فيه_واجهة.ص` لا يفشل بخطأ «عقدة AST غير مدعومة».
- AC-2: `UIWidgetExprNode` يُترجَم إلى استدعاء بناء عنصر في SIR ثم LLVM.
- AC-3: `UIModifierNode` (سلسلة المعدّلات) يُترجَم إلى استدعاءات تعيين خصائص.
- AC-4: `UIDeclarationNode` يُترجَم كصنف (إعادة استخدام مسار الأصناف في SIR).
- AC-5: `UIConditionalNode`/`UILoopNode` يُترجَمان لتدفق تحكم صحيح في LLVM IR.
- AC-6: مقارنة المفسر/المترجم (BF-08): نفس بنية الشجرة الناتجة من الطرفين.

## المهام/المهام الفرعية
- [ ] T1: تعريف تعليمات SIR (أو استدعاءات runtime) لبناء عقد UI وتعيين الخصائص.
  - [ ] اختبار: `زر("x")` يولّد SIR صحيحاً.
- [ ] T2: زيارة `UIWidgetExprNode` في SIRBuilder (الوسائط المسماة/الموضعية + الأبناء).
  - [ ] اختبار وحدة لكل من العناصر الـ15.
- [ ] T3: زيارة `UIModifierNode` → استدعاءات تعيين خصائص متسلسلة.
- [ ] T4: زيارة `UIDeclarationNode` كصنف (إعادة استخدام منطق الأصناف، CW-20).
- [ ] T5: زيارة `UIConditionalNode`/`UILoopNode` → كتل LLVM (then/else/loop_body) بأسماء واصفة (CW-11).
- [ ] T6: توليد LLVM للاستدعاءات + ربط مكتبة وقت تشغيل UI.
- [ ] T7: اختبار تكامل: `sadc` يبني `.ص` فيه `واجهة_تصريحية` إلى تنفيذي، وتشغيله لا ينهار.
- [ ] T8: اختبار تراجع: بناء `sadc` لكل الاختبارات غير-UI يبقى ناجحاً (BF-29).

## ملاحظات
- ابدأ بالحد الأدنى: بناء الشجرة عبر runtime؛ الرسم الفعلي مسؤولية الباطن (خطة منفصلة).
- التزم تسلسل الطبقات Lexer→Parser→AST→SIR→LLVM (CW-02)؛ لا تقفز.
- وثّق أي تعليمة SIR جديدة (CW-24 توافق خلفي: إضافة opcode مسموحة، تعديل ممنوع).

## Dev Agent Record
*(يُملأ أثناء التنفيذ)*

## File List
*(يُحدّث بعد كل مهمة)*
