# TODO: المرحلة 4 - نواة المُفسِّر
# TODO: Phase 4 - Interpreter Core

**تاريخ التحديث / Last Updated:** نوفمبر 21، 2025  
**الحالة العامة / Overall Status:** 🔄 قيد التنفيذ / In Progress (50%)

---

## 🎯 الهدف العام / Overall Goal

بناء نواة المُفسِّر الكاملة التي تستطيع تنفيذ برامج لغة ص باستخدام نمط الزائر (Visitor Pattern).

---

## 📊 نظرة عامة على التقدم / Progress Overview

```
المرحلة 4.1: واجهة الزائر        ✅ مكتملة   100%
المرحلة 4.2: مُقيِّم التعابير     ✅ مكتملة   100%
المرحلة 4.3: منفّذ الجُمل        ⏸️ لم تبدأ    0%
المرحلة 4.4: الميزات المتقدمة    ⏸️ لم تبدأ    0%
----------------------------------------
الإجمالي / Total:                      50%
```

---

## ✅ المرحلة 4.1: واجهة الزائر (Visitor Interface) - مكتملة

**الحالة / Status:** ✅ مكتملة 100%  
**التاريخ / Date:** نوفمبر 21، 2025

### المنجزات / Achievements
- ✅ إنشاء BaseASTVisitor مع 38 دالة افتراضية
- ✅ دعم جميع أنواع العُقد: Expressions, Statements, Declarations
- ✅ توثيق ثنائي اللغة (عربي/إنجليزي)
- ✅ نمط تصميم نظيف وقابل للتوسّع

### الملفات / Files
- `include/interpreter/visitors/base_ast_visitor.h` (331 سطر)

---

## ✅ المرحلة 4.2: مُقيِّم التعابير (Expression Evaluator) - مكتملة

**الحالة / Status:** ✅ مكتملة 100%  
**التاريخ / Date:** نوفمبر 21، 2025  
**الاختبارات / Tests:** 37/37 ✅

### المنجزات / Achievements
- ✅ تنفيذ ExpressionEvaluator يرث من BaseASTVisitor
- ✅ تقييم القيم الحرفية (6 أنواع)
- ✅ عمليات المتغيرات (قراءة/كتابة)
- ✅ العمليات الحسابية (6 أنواع: +, -, *, /, %, **)
- ✅ عمليات المقارنة (6 أنواع: ==, !=, <, >, <=, >=)
- ✅ العمليات المنطقية (AND, OR)
- ✅ العمليات الأحادية (-, !, +)
- ✅ المصفوفات (إنشاء، فهرسة)
- ✅ القواميس (إنشاء، فهرسة)
- ✅ التعابير المتداخلة
- ✅ رسائل خطأ ثنائية اللغة

### الملفات / Files
- `include/interpreter/visitors/expression_evaluator.h` (210 سطر)
- `src/interpreter/visitors/expression_evaluator.cpp` (442 سطر)
- `tests/interpreter_tests/test_expression_evaluator.cpp` (954 سطر)

### التقرير / Report
📄 `phases/phase4/TASK_4.2_EXPRESSION_EVALUATOR_COMPLETE.md`

---

## ⏸️ المرحلة 4.3: منفّذ الجُمل (Statement Executor) - لم تبدأ

**الحالة / Status:** ⏸️ لم تبدأ 0%  
**الأولوية / Priority:** 🔴 عالية جداً / Very High  
**الوقت المتوقع / Estimated Time:** 8-12 ساعة

### الهدف / Goal
تنفيذ StatementExecutor لتنفيذ جميع أنواع الجُمل في لغة ص.

### المهام المطلوبة / Required Tasks

#### 4.3.1 تصميم StatementExecutor
- [ ] إنشاء فئة StatementExecutor ترث من BaseASTVisitor
- [ ] تكامل مع ExpressionEvaluator
- [ ] تكامل مع VariableManager وFunctionManager
- [ ] نظام إدارة التدفق (flow control)

#### 4.3.2 الجُمل الأساسية / Basic Statements
- [ ] **VarDeclStmt** - تعريف المتغيرات
  - تعريف بدون قيمة: `متغير x: عدد`
  - تعريف مع قيمة: `متغير y: عدد = 10`
  - استنتاج النوع: `متغير z = "text"`
- [ ] **ExpressionStmt** - تعابير مستقلة
  - استدعاء دوال
  - إسناد متغيرات
- [ ] **BlockStmt** - كتل الكود
  - إدارة النطاقات (scopes)
  - دخول/خروج من الكتل

#### 4.3.3 جُمل التحكم بالتدفق / Control Flow Statements
- [ ] **IfStmt** - الشرط
  - if بسيط
  - if-else
  - if-elif-else (سلاسل شرطية)
- [ ] **WhileStmt** - حلقة while
  - شرط وكتلة تنفيذ
  - دعم break/continue
- [ ] **ForStmt** - حلقة for
  - for بثلاثة أجزاء: `for (i = 0; i < 10; i++)`
  - دعم break/continue
- [ ] **ForRangeStmt** - حلقة for-range
  - `for x in array`
  - `for key, value in map`

#### 4.3.4 جُمل إدارة الدوال / Function Management
- [ ] **FuncDeclStmt** - تعريف الدوال
  - دوال بسيطة: `دالة add(a, b) { ... }`
  - معاملات اختيارية: `دالة func(x = 10)`
  - تخزين الدالة في FunctionManager
- [ ] **CallExpr** - استدعاء الدوال
  - تقييم المعاملات
  - استدعاء الدالة
  - إرجاع القيمة
- [ ] **ReturnStmt** - الإرجاع
  - return بدون قيمة
  - return مع قيمة

#### 4.3.5 جُمل التحكم بالحلقات / Loop Control
- [ ] **BreakStmt** - الخروج من الحلقة
  - كسر حلقة for
  - كسر حلقة while
  - فحص السياق (داخل حلقة فقط)
- [ ] **ContinueStmt** - الانتقال للتكرار التالي
  - متابعة في for
  - متابعة في while
  - فحص السياق (داخل حلقة فقط)

#### 4.3.6 معالجة الأخطاء / Error Handling
- [ ] **TryStmt** - try-catch-finally
  - كتلة try
  - كتلة catch مع نوع الخطأ
  - كتلة finally (اختيارية)
- [ ] **RaiseStmt** - إلقاء الأخطاء
  - raise مع رسالة
  - raise مع نوع خطأ

#### 4.3.7 الاختبارات / Tests (30+ tests)
- [ ] 5 اختبارات تعريف المتغيرات
- [ ] 3 اختبارات كتل الكود
- [ ] 6 اختبارات الشروط (if/else/elif)
- [ ] 4 اختبارات حلقة while
- [ ] 4 اختبارات حلقة for
- [ ] 3 اختبارات for-range
- [ ] 5 اختبارات تعريف واستدعاء الدوال
- [ ] 2 اختبارات break/continue
- [ ] 3 اختبارات try-catch-raise

### الملفات المطلوبة / Required Files
```
include/interpreter/visitors/
  └── statement_executor.h          (تصميم)

src/interpreter/visitors/
  └── statement_executor.cpp        (تنفيذ)

tests/interpreter_tests/
  └── test_statement_executor.cpp   (30+ اختبارات)
```

### المعايير / Criteria
- ✅ تنفيذ جميع أنواع الجُمل (10+ أنواع)
- ✅ تكامل كامل مع ExpressionEvaluator
- ✅ إدارة صحيحة للنطاقات
- ✅ معالجة أخطاء آمنة
- ✅ رسائل خطأ ثنائية اللغة
- ✅ 30+ اختبار (معدل نجاح 100%)

---

## ⏸️ المرحلة 4.4: الميزات المتقدمة - لم تبدأ

**الحالة / Status:** ⏸️ لم تبدأ 0%  
**الأولوية / Priority:** 🟡 متوسطة / Medium  
**الوقت المتوقع / Estimated Time:** 6-8 ساعات

### المهام المطلوبة / Required Tasks

#### 4.4.1 Lambda Expressions
- [ ] تحليل Lambda في Parser
- [ ] تنفيذ Lambda في Evaluator
- [ ] دوال عليا (Higher-order functions)

#### 4.4.2 List Comprehensions
- [ ] تحليل Comprehensions في Parser
- [ ] تنفيذ في Evaluator
- [ ] دعم الشروط (if)

#### 4.4.3 Generators & Yield
- [ ] تنفيذ YieldStmt
- [ ] Generator functions
- [ ] Iterator protocol

#### 4.4.4 الاختبارات / Tests (20+ tests)
- [ ] 8 اختبارات Lambda
- [ ] 8 اختبارات Comprehensions
- [ ] 4 اختبارات Generators

---

## 📁 هيكل الملفات المتوقع / Expected File Structure

```
include/interpreter/
  └── visitors/
      ├── base_ast_visitor.h        ✅ موجود
      ├── expression_evaluator.h    ✅ موجود
      └── statement_executor.h      ⏸️ مطلوب

src/interpreter/
  └── visitors/
      ├── expression_evaluator.cpp  ✅ موجود
      └── statement_executor.cpp    ⏸️ مطلوب

tests/interpreter_tests/
  ├── test_expression_evaluator.cpp ✅ موجود (37 اختبار)
  └── test_statement_executor.cpp   ⏸️ مطلوب (30+ اختبار)

phases/phase4/
  ├── TASK_4.1_VISITOR_COMPLETE.md       ⏸️ مطلوب
  ├── TASK_4.2_EXPRESSION_EVALUATOR_COMPLETE.md ✅ موجود
  ├── TASK_4.3_STATEMENT_EXECUTOR_COMPLETE.md   ⏸️ مطلوب
  └── TASK_4.4_ADVANCED_FEATURES_COMPLETE.md    ⏸️ مطلوب
```

---

## 🎯 الأهداف القادمة / Upcoming Goals

### الهدف الفوري / Immediate Goal (التالي)
🔴 **البدء في المرحلة 4.3: منفّذ الجُمل**
- تصميم StatementExecutor
- تنفيذ الجُمل الأساسية
- اختبار التكامل مع ExpressionEvaluator

### الهدف قصير المدى / Short-term Goal (1-2 أسابيع)
- ✅ إكمال جميع الجُمل الأساسية والمتقدمة
- ✅ 30+ اختبار نجاح 100%
- ✅ تكامل كامل بين Evaluator و Executor

### الهدف متوسط المدى / Medium-term Goal (2-4 أسابيع)
- ✅ إضافة الميزات المتقدمة (Lambda, Comprehensions, Generators)
- ✅ تحسينات الأداء
- ✅ تقارير تفصيلية للمراحل

---

## 📊 إحصائيات المشروع / Project Statistics

### المرحلة 4 - حتى الآن
```
الملفات المُنشأة / Files Created:      3
الأسطر المكتوبة / Lines Written:       1,937
الاختبارات / Tests:                    37/37 ✅
معدل النجاح / Success Rate:           100%
```

### المشروع الكامل
```
المراحل المكتملة / Completed Phases:  3.5 / 7
الاختبارات الكلية / Total Tests:     162 + 37 = 199 ✅
إجمالي الأسطر / Total Lines:         ~11,000+
```

---

## 🚀 البدء بالمرحلة التالية / Starting Next Phase

لبدء المرحلة 4.3:
```bash
# 1. إنشاء فئة StatementExecutor
# 2. تنفيذ الجُمل الأساسية (VarDecl, Block, If)
# 3. إضافة اختبارات أولية
# 4. التكامل مع ExpressionEvaluator
# 5. توسيع الجُمل المتقدمة (Loops, Functions)
```

---

**آخر تحديث / Last Updated:** نوفمبر 21، 2025  
**الحالة / Status:** 🟢 جاهز للمرحلة 4.3 / Ready for Phase 4.3
