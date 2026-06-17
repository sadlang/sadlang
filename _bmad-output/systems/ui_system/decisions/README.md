# قرارات معمارية — نظام الواجهات (SadUI)

سجلّ القرارات المعمارية (ADRs) لنظام الواجهات التصريحي. القرارات هنا **منفّذة فعلاً في الكود**
(`status: ACCEPTED`) ومُستخرَجة من المحلل النحوي `shared/parser/src/ui/parser_ui.cpp`.

| المعرّف | العنوان | الحالة | الدليل في الكود |
|---------|---------|--------|------------------|
| [ADR-UI-01](ADR-UI-01-conditional-loop-rendering.md) | الرسم الشرطي والحلقات داخل الواجهات | ACCEPTED | `parseUIConditional`, `parseUILoop` |
| [ADR-UI-02](ADR-UI-02-fifteen-primitive-widgets.md) | تقليص العناصر إلى 15 أولياً + خطة الإهمال | ACCEPTED | `knownWidgets`, `deprecatedWidgets` |
| [ADR-UI-07](ADR-UI-07-typed-widget-parsing.md) | تحليل العناصر بنوع صريح بدل dynamic_cast | ACCEPTED | `parseWidgetExpressionTyped` |

> **ملاحظة:** الترقيم يتبع المعرّفات المذكورة في تعليقات الكود (UI-01، UID-02، UI-07).
> المعرّفات بين 03–06 مذكورة ضمنياً في الكود لكنها غير مذكورة صراحةً — تُوثَّق عند العثور
> على إشارة صريحة لها (التزاماً بـ GR-01: لا توثيق بلا دليل).
