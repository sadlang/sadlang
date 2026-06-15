---
adrId: ADR-UI-07
title: "تحليل العناصر بنوع صريح (parseWidgetExpressionTyped) بدل dynamic_cast"
status: ACCEPTED
date: 2026-06-08
deciders: ["فريق لغة ص (Parser/UI)"]
supersedes: null
supersededBy: null
evidence:
  - shared/parser/src/ui/parser_ui.cpp (parseWidgetExpression, parseWidgetExpressionTyped)
---

# ADR-UI-07 — تحليل العناصر بنوع صريح بدل dynamic_cast

## السياق

دالة `parseWidgetExpression()` تُرجع `ExprPtr` (النوع العام للتعابير) لأنها تُستدعى من سياق
التعابير العادية (مثل `اعرض زر(...)` أو `متغير ز = زر(...)`). لكن داخل
`parseWidgetChildren()` نحتاج تحديداً إلى `unique_ptr<UIWidgetExprNode>` لتخزينه في قائمة
أبناء الحاوية. الحصول عليه من `ExprPtr` كان يتطلب `dynamic_cast` في كل موضع — وهو نمط هشّ
ومكلف ويخالف CW-13 (عدم تجاوز نظام الأنواع).

## الخيارات المدروسة

1. **`dynamic_cast<UIWidgetExprNode*>` على ناتج `parseWidgetExpression`**
   - ✅ لا دالة جديدة.
   - ❌ هشّ (يفشل صامتاً لو تغيّر النوع)، تكلفة RTTI، تكرار في كل نداء (CW-13/CW-19).
2. **تغيير توقيع `parseWidgetExpression` ليُرجع النوع الصريح**
   - ✅ لا ازدواج.
   - ❌ يكسر مستدعيه في سياق التعابير العامة التي تتوقع `ExprPtr` (مخالفة BF-15).
3. **دالة موازية `parseWidgetExpressionTyped()` تُرجع `unique_ptr<UIWidgetExprNode>`** ⭐
   - ✅ نوع صريح بلا `dynamic_cast` داخل `parseWidgetChildren`.
   - ✅ تحافظ على `parseWidgetExpression` للتوافق الخلفي (BF-15).
   - ❌ ازدواج جزئي في منطق التحليل بين الدالتين.

## القرار

اخترنا **الخيار 3**: إضافة `parseWidgetExpressionTyped()` التي تكرّر منطق تحليل العنصر
(الوسائط المسماة/الموضعية + سلسلة المعدّلات + كتلة الأبناء) لكن تُرجع
`unique_ptr<UIWidgetExprNode>` مباشرةً. تُستخدم حصراً داخل `parseWidgetChildren()` وفروع
`parseUIConditional`/`parseUILoop`، بينما تبقى `parseWidgetExpression()` لسياق التعابير العامة.

## النتائج (Consequences)

### إيجابية
- إزالة كل `dynamic_cast` من مسار تحليل أبناء الحاويات (التزام CW-13).
- وضوح النية: استدعاء داخل شجرة الواجهة يُعيد نوع الواجهة مباشرةً.
- صفر كسر للتوافق الخلفي (BF-15).

### سلبية
- ازدواج منطق بين `parseWidgetExpression` و`parseWidgetExpressionTyped` (~60 سطراً) —
  مرشّح لإعادة هيكلة لاحقة باستخراج دالة مساعدة مشتركة (CW-19) إن استقرّت القواعد.

### حيادية
- أي تغيير في قواعد تحليل العنصر يجب تطبيقه في الموضعين حتى تُوحَّدا.

## مراجع
- `shared/parser/src/ui/parser_ui.cpp` — `parseWidgetExpression` (سطر ~426)، `parseWidgetExpressionTyped` (سطر ~725).
