# Story 1.4 — إصلاح المخالفات الحالية + حذفها من allowlist

**Epic:** 1 — نظام Contract-as-Code
**Story:** 1.4
**الحالة:** backlog
**الأولوية:** P1
**الاعتمادية:** Story 1.1 + 1.2 + 1.3 مكتملات
**المرجع:** `prd-contract-as-code.md` — مرحلة الترحيل

---

## وصف القصة

**كمطوِّر في لغة ص،**
أريد إصلاح المخالفات الموجودة في الملفات الحالية،
**حتى** تُحذف من allowlist ويُصبح النظام نظيفاً تماماً.

---

## معايير القبول (AC)

### AC-1.4.1: إصلاح expression_evaluator_ui.cpp
- [ ] إزالة مقارنات النصوص العربية المباشرة (4 مواضع في السطور 88-103)
- [ ] الاستعاضة بمقارنة `sad::ui::UINodeType` (enum) بدلاً من النصوص
- [ ] الدالة `arabicNameToNodeType()` مُستدعاة بالفعل في السطر 63 — استخدم نتيجتها
- [ ] الكود يُبنى بدون أخطاء بعد الإصلاح
- [ ] المفسر يعمل بشكل صحيح مع ملفات `.ص` التي تستخدم widgets

### AC-1.4.2: تنظيف allowlist
- [ ] حذف `interpreter/src/visitors/expression_evaluator_ui.cpp` من allowlist
- [ ] `cmake -B build` ينجح بدون `SAD_SKIP_STANDARDS_GUARD=ON`

### AC-1.4.3: فحص ملفات أخرى
- [ ] شغّل `check_code_standards.py` على كامل المشروع
- [ ] أي مخالفات إضافية: أضفها للـ allowlist مع تعليق + تاريخ + المسؤول
- [ ] وثّق قائمة المخالفات المتبقية في allowlist

---

## المهام التقنية

### Task 1: تحليل expression_evaluator_ui.cpp:88-103

المشكلة الحالية:
```cpp
// السطر 88
if (node.widgetName == "\xd9\x86\xd8\xb5" || node.widgetName == "Text")
    builder->setProperty("text_value", firstArgStr);
else if (node.widgetName == "\xd8\xb2\xd8\xb1" || node.widgetName == "Button")
    ...
```

الحل الصحيح (استخدام nodeType المحسوب في السطر 63):
```cpp
// nodeType تم حسابه بالفعل في السطر 63
if (nodeType == sad::ui::UINodeType::Text)
    builder->setProperty("text_value", firstArgStr);
else if (nodeType == sad::ui::UINodeType::Button)
    ...
```

### Task 2: اختبار المفسر بعد الإصلاح
- شغّل ملف `.ص` يستخدم `نص("مرحبا")`
- تحقق أن السلوك لم يتغير

### Task 3: فحص شامل للمشروع
```powershell
python scripts/lint/check_code_standards.py `
    --dirs compiler/src interpreter/src vm/src shared/ `
    --spec scripts/lint/code_standards.yaml
```
- وثّق النتائج في allowlist

---

## سياق التنفيذ

### تعريفات UINodeType الموجودة
- `interpreter/src/visitors/expression_evaluator_ui.cpp:63` — يستدعي `arabicNameToNodeType()`
- `arabicNameToNodeType()` تُعيد `std::optional<UINodeType>`
- المتغير `nodeType` موجود بالفعل في نطاق الدالة

### ملاحظة مهمة
- المتغير `nodeType` مُعيَّن من `arabicNameToNodeType()` في السطر 63
- المقارنات في السطور 88-103 يجب أن تستخدم `nodeType` المحسوب
- لا تُعيد استدعاء `arabicNameToNodeType()` — استخدم المتغير الموجود

---

## قائمة الملفات المُنشأة/المُعدَّلة
- [ ] `interpreter/src/visitors/expression_evaluator_ui.cpp` ← تعديل (إصلاح CS-01)
- [ ] `scripts/lint/code_standards_allowlist.txt` ← تعديل (حذف السطر المُصلح)

---

## Dev Agent Record
*(تُملأ من قبل وكيل التطوير أثناء التنفيذ)*

**ما تم تنفيذه:**

**الاختبارات التي أُنشئت:**

**القرارات المتخذة:**

---
