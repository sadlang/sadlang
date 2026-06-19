---
storyId: STORY-UI-W15-03
epic: EPIC-UI-W15
title: "وصل @حالة بإعادة البناء التلقائي"
status: DONE
priority: P1
date: 2026-06-08
author: Amelia (bmad-agent-dev)
estimate: "6 ساعات"
dependsOn: [STORY-UI-W15-02]
---

# STORY-UI-W15-03 — وصل `@حالة` بإعادة البناء التلقائي

> **ملاحظة تحديث (2026-06-18):** «الفجوة» الموصوفة أدناه (UIStateManager غير موصول)
> لم تَعُد قائمة في الكود الحالي — السلسلة موصولة بالكامل من طرف لطرف؛ الناقص كان
> **تغطية اختبارية** فقط، وقد أُضيفت. (نفس نمط الإنذار الخاطئ في P0-2.)

## القصة
بصفتي مطوّر تطبيقات، أريد أن يؤدي تغيير حقل `@حالة` إلى إعادة بناء الواجهة تلقائياً،
حتى تنعكس تغييرات البيانات على الشاشة دون إدارة يدوية.

## السياق التقني (من الكود)
- `UIStateManager` مُصمّم لهذا الغرض (`interpreter/include/ui/ui_state_manager.h`):
  observer على `setField` → إشعار → `scheduleRebuild` مع batching → استدعاء `بناء()`.
- النواة فيها `signal.h` (تفاعلية دقيقة، ADR-UI-10) و`reconciler.h` (Virtual DOM).
- **الفجوة:** `UIStateManager` غير موصول بمسار تعيين الحقول في
  `expression_evaluator_members_assign.cpp` لمكوّنات `واجهة_تصريحية`.

## معايير القبول
- AC-1: عند `جديد مكوّن()`, يُسجَّل observer على حقول `@حالة` فقط (لا الحقول العادية).
- AC-2: تعيين حقل `@حالة` (`م.عداد = 5`) يُخطر `UIStateManager`.
- AC-3: عدة تعيينات متتالية تُدمج في إعادة بناء واحدة (batching).
- AC-4: تعيين حقل عادي (غير `@حالة`) لا يُطلق إعادة بناء.
- AC-5: إعادة البناء تستدعي `بناء()` وتُنتج شجرة جديدة قابلة للرصد (عبر hook اختباري).

## المهام/المهام الفرعية
- [x] T1: عند إنشاء مكوّن (STORY-02)، تسجيله في `UIStateManager` مع قائمة حقول `@حالة`.
  - [x] اختبار: `registerComponent` + `isRegistered`/`componentCount` (AC-1).
- [x] T2: في مسار تعيين الحقل، فحص إن كان الحقل `@حالة` → استدعاء observer.
  - [x] اختبار: تعيين `@حالة` يُطلق إعادة البناء؛ تعيين حقل عادي لا يُطلقها (AC-2/AC-4).
- [x] T3: تنفيذ batching (دمج تغييرات الإطار الواحد في إعادة بناء واحدة).
  - [x] اختبار: 3 تعيينات متتالية → إعادة بناء واحدة (AC-3).
- [x] T4: استدعاء callback عند `flush()` + hook اختباري (`setRebuildCallback`) لرصد الاستدعاء.
  - [x] اختبار: `flush` يستدعي الـcallback مرة واحدة بعد التغيير (AC-5).
- [x] T5: اختبار وحدة شامل لسلسلة التفاعلية (6 حالات، كلها تنجح).

## ملاحظات
- لا تَصِل الـ Reconciler بالباطن هنا (ذلك خطة الباطن)؛ يكفي رصد إعادة البناء منطقياً.
- استفد من `signal.h` إن كان مدمجاً، وإلا اربط observer مباشرة (أبسط حل صحيح — BF-13).

## Dev Agent Record
**النتيجة (2026-06-18):** السلسلة التفاعلية موصولة ومُختبَرة بالكامل. خلافًا لما وصفته
الستوري، الكود الحالي يصل `UIStateManager` بمسار تعيين الحقول؛ كان الناقص تغطية اختبارية.

**التسلسل المُثبَت:** إنشاء `عداد()` → `visitNewExpr` يسجّل المكوّن
(`expression_evaluator_oop_new.cpp:622`) → `registerComponent` يربط `setOnFieldChanged`
→ تعيين `ع.عدد=5` يمرّ عبر `ObjectInstance::setField` (`members_assign.cpp:309`) →
يُطلق `onFieldChanged_` → `onStateFieldChanged` يصفّي حقول @حالة ويفحص تغيّر القيمة →
`scheduleRebuild` (عدّاد batching) → `flush()` يستدعي `rebuildCallback_` مرة واحدة.

**معايير القبول:** AC-1..AC-5 جميعها ✓ (إضافةً إلى: لا إعادة بناء عند تعيين نفس القيمة،
وفصل المراقب عند `unregisterComponent`).

**الدليل (GR-01):**
- `interpreter/src/ui/ui_state_manager.cpp` — تصفية الحقول (`:123`) + batching (`:154/:163`).
- `interpreter/src/visitors/expression_evaluator_members_assign.cpp:309` — التعيين عبر `setField`.
- `shared/types/src/object_instance.cpp:221` — `setField` يُطلق `onFieldChanged_`.
- اختبار الوحدة `test_ui_reactive_state` — 6/6 ينجح.

**ملاحظة:** `interpreter_test_stubs.cpp` يُبطل (no-op) دوال `UIStateManager`؛ لذا رُبط
اختبار التفاعلية بـ`sad_core` **بدون** ملف البدائل وإلا حجبت البدائل التنفيذ الحقيقي.

## File List
- `tests/unit/comprehensive/test_ui_reactive_state.cpp` (اختبار سلسلة التفاعلية) — جديد
- `cmake/tests_comprehensive.cmake` (تسجيل الهدف، بدون stubs) — مُعدَّل
