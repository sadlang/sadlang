# Story EM-CPP-T2 — توحيد Tier 2: أخطاء الكود المُترجَم بـ ErrorCode (sad_panic)

**Epic**: [EPIC-EM-CPP](../epics/EPIC-EM-CPP-MIGRATION.md) (ملحق Tier 2) | **النقاط**: 8 | **الأولوية**: منخفضة (مستقبلية) | **الحالة**: جاهزة
**التبعيات**: لا شيء (مستقلة عن BuiltinContext — مسار مختلف) | **المُسنَد إلى**: TBD
**يحكمها**: [ADR-EM-CPP-1](../decisions/ADR-EM-CPP-1-BUILTIN-CONTEXT.md) §نطاق المترجم + `docs/ERROR_SYSTEM_GUIDE.md` §3-ب

---

## الهدف
توحيد أخطاء **الكود المُترجَم وقت التشغيل** مع YAML عبر **نداء موحَّد** من codegen، بحيث:
- **هدف مُضيف** → رسائل **غنية (Tier 1)** عبر `ErrorCatalog` (كالمفسر تماماً).
- **هدف freestanding** → رسائل **أدنى (Tier 2)** عبر جدول `const char*` مُولَّد (بلا STL).

> **تصحيح جوهري:** ليست كل رسائل المترجم محدودة. **المُضيف غنيّ كالمفسر**؛ التقييد **فقط** في
> freestanding (لا STL). راجع الدليل §3-ب.

## التصميم: نداء موحَّد + runtime حسب الهدف
1. **codegen المترجم** يُصدر نداءً موحَّداً واحداً:
   `sad_runtime_error(ErrorCode code, SourceLoc, placeholders)` — بدل النص الخام.
2. **مكتبة وقت التشغيل المربوطة تختلف حسب الهدف:**
   - **runtime مُضيف:** يحقّق `sad_runtime_error` بالرندرة عبر **`ErrorCatalog`** (STL متاح) →
     رسائل غنية ثنائية اللغة + placeholders (Tier 1).
   - **runtime freestanding:** يحقّقه بـ`sad_panic` + **جدول `const char*` مُولَّد من YAML**
     (بلا STL) → رسالة قصيرة + halt (Tier 2).
3. **مولّد الجدول الـfreestanding** يُنتج `static const char* const SAD_ERR_BRIEF[]` + تعداد رموز —
   من نفس `language-truth/errors/` (مصدر موحَّد).

## التنفيذ (Tasks)
- [ ] **T1** — جرد مواقع إصدار أخطاء التشغيل في codegen (bounds/division/null...) + توحيدها على
  نداء `sad_runtime_error(code,...)`.
- [ ] **T2** — runtime مُضيف: تحقيق `sad_runtime_error` بالرندرة عبر `ErrorCatalog` (ربط sad_shared
  أو واجهة الكتالوج) → **رسائل غنية كالمفسر**.
- [ ] **T3** — مولّد جدول `const char*` freestanding من YAML + تعداد رموز.
- [ ] **T4** — runtime freestanding: `sad_panic(code,...)` يبحث في الجدول.
- [ ] **T5** — اختبار: ثنائي مُضيف بقسمة صفر → **رسالة غنية**؛ ثنائي freestanding → رسالة الجدول.

## معايير القبول
- **0 نص خطأ خام** في codegen (كله `sad_runtime_error(code,...)`).
- **هدف مُضيف:** رسالة غنية من `ErrorCatalog` (مطابقة للمفسر).
- **هدف freestanding:** رسالة من الجدول المُولَّد (بلا STL، يبني bare-metal).
- بناء sadc أخضر · لا تراجع.

## ملاحظات
- المصدر موحَّد (YAML يغذّي الكتالوج الغني **والجدول** الأدنى).
- **فقط freestanding محدود** (قيد STL)؛ كل الأهداف الأخرى غنية.
