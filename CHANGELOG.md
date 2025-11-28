# سجل التغييرات / Changelog

جميع التغييرات المهمة في المشروع موثقة في هذا الملف.
All notable changes to this project are documented in this file.

النسق مبني على [Keep a Changelog](https://keepachangelog.com/)  
The format is based on [Keep a Changelog](https://keepachangelog.com/)

---

## [1.2.0] - 2025-11-25

### أُضيف / Added

#### 🎯 المرحلة 6: الميزات الجديدة

**جملة Switch/Case:**
- إضافة `SwitchStmt` AST node في `statements.h`
- إضافة `CaseBranch` struct
- تنفيذ `parseSwitchStmt()` في parser
- تنفيذ `visitSwitchStmt()` في interpreter
- دعم الحالة الافتراضية `افتراضي:`
- الشرط يجب أن يكون بين أقواس: `حالة (تعبير)`
- لا يوجد fall-through (ينفذ حالة واحدة فقط)

**العامل الثلاثي Ternary:**
- إضافة `TernaryExpr` AST node في `expressions.h`
- إضافة دعم `?` token في lexer
- تنفيذ `parseTernary()` في parser مع right-associativity
- تنفيذ `visitTernaryExpr()` في interpreter
- دعم short-circuit evaluation
- دعم التداخل (nested ternary)

#### 📚 المرحلة 7: التوثيق

**ملفات توثيق جديدة:**
- `docs/PHASE6_FEATURES.md` - دليل شامل 800+ سطر
- `docs/examples/PHASE6_ADVANCED_EXAMPLES.md` - 10+ أمثلة متقدمة
- `docs/PHASE6_7_COMPLETION_REPORT.md` - تقرير إنجاز كامل

**تحديثات التوثيق:**
- توثيق `parseSwitchStmt()` في `parser_core.h`
- توثيق `parseTernary()` في `parser_core.h`
- توثيق `SwitchStmt` في `statements.h`
- توثيق `TernaryExpr` في `expressions.h`
- توثيق interpreter visitors

**ملفات اختبار جديدة:**
- `test_simple_switch.s`
- `test_switch_default.s`
- `test_switch.s`
- `test_ternary_num.s`
- `test_nested_ternary.s`

### عُدّل / Changed

**Lexer:**
- إضافة دعم رمز `?` في `lexer_core.cpp` (السطر 781)

**Parser:**
- تعديل `parseStatement()` لدعم `KEYWORD_CASE`
- تعديل `parseAssignment()` لاستدعاء `parseTernary()`
- تحديث grammar documentation

**AST Visitor:**
- إضافة `visitTernaryExpr()` abstract method
- إضافة `visitSwitchStmt()` abstract method
- تحديث `BaseASTVisitor` default implementations

### أُصلح / Fixed

- إصلاح مقارنة القيم في switch: استخدام `Value::equals()` بدلاً من `operator==`
- إصلاح missing header declarations في visitor classes
- إصلاح AST position tracking في parser

---

## [1.1.0] - 2025-11-22

### أُضيف / Added

#### المكتبة القياسية - المراحل 3 و 4

**دوال المصفوفات (10 دوال):**
- `طول()` / `length()` - Get array length
- `أضف()` / `push()` - Add element to end
- `احذف()` / `pop()` - Remove last element
- `أدرج()` / `insert()` - Insert at position
- `أزل()` / `remove()` - Remove at position
- `انعكس()` / `reverse()` - Reverse array
- `رتب()` / `sort()` - Sort array
- `خريطة()` / `map()` - Transform elements
- `رشح()` / `filter()` - Filter elements
- `اطوِ()` / `reduce()` - Reduce to single value

**الدوال الرياضية (12 دالة):**
- `قيمة_مطلقة()` / `abs()` - Absolute value
- `قوة()` / `pow()` - Power
- `جذر()` / `sqrt()` - Square root
- `دائري()` / `round()` - Round to nearest
- `أرضية()` / `floor()` - Round down
- `سقف()` / `ceil()` - Round up
- `جيب()` / `sin()` - Sine
- `جيب_تمام()` / `cos()` - Cosine
- `ظل()` / `tan()` - Tangent
- `لوغاريتم()` / `log()` - Natural logarithm
- `لوغاريتم10()` / `log10()` - Base-10 logarithm
- `أس()` / `exp()` - e^x

**ملفات جديدة:**
- `include/stdlib/core/array_functions.h`
- `src/stdlib/core/array_functions.cpp`
- `include/stdlib/math/math_functions.h`
- `src/stdlib/math/math_functions.cpp`
- `examples/test_array_functions.s`
- `examples/test_math_functions.s`

**توثيق:**
- `docs/stlib_plan/PHASE3_4_COMPLETION_REPORT.md`
- `SESSION_SUMMARY_STDLIB_PHASE3_4.md`
- `NEXT_STEPS_STDLIB.md`
- `STDLIB_UPDATE_README.md`
- `QUICK_SUMMARY.md`

---

## [1.0.0] - 2025-11-20

### أُضيف / Added

#### البنية الأساسية

**Lexer:**
- دعم الرموز العربية والإنجليزية
- Keywords ثنائية اللغة
- دعم Unicode

**Parser:**
- تحليل نحوي كامل
- دعم OOP (classes, inheritance)
- دعم الدوال والمتغيرات
- دعم التعابير والجمل

**Interpreter:**
- تنفيذ مباشر للكود
- دعم الأنواع الأساسية
- إدارة الذاكرة
- Error handling

**المكتبة القياسية - المرحلة 1 و 2:**
- دوال الإدخال/الإخراج
- دوال النصوص (17 دالة)

---

## القادم / Upcoming

### [1.3.0] - مخطط

**المرحلة 8: تحسينات الأداء**
- Jump table optimization للـ switch
- Bytecode compilation
- JIT compilation (مستقبلي)

**المرحلة 9: الميزات المتقدمة**
- Pattern matching
- Range expressions في switch
- Null-coalescing operator
- Optional chaining

**المرحلة 10: المكتبة القياسية - Phase 5**
- دوال الأنواع (10 دوال)
- File I/O functions
- Network functions

---

## أنواع التغييرات / Types of Changes

- **أُضيف / Added** - للميزات الجديدة
- **عُدّل / Changed** - للتغييرات في الميزات الموجودة
- **مُهمَل / Deprecated** - للميزات التي ستُزال قريباً
- **أُزيل / Removed** - للميزات المزالة
- **أُصلح / Fixed** - لإصلاحات الأخطاء
- **أمان / Security** - لإصلاحات الأمان

---

## الإصدارات / Versions

### نظام الإصدارات / Versioning

نستخدم [Semantic Versioning](https://semver.org/):
- **MAJOR**: تغييرات غير متوافقة
- **MINOR**: ميزات جديدة متوافقة
- **PATCH**: إصلاحات متوافقة

**مثال / Example**: `1.2.3`
- `1` = Major version
- `2` = Minor version (new features)
- `3` = Patch version (bug fixes)

---

## الروابط / Links

- [الإصدار الحالي / Current Release](https://github.com/sad-lang/releases/latest)
- [جميع الإصدارات / All Releases](https://github.com/sad-lang/releases)
- [التوثيق / Documentation](./DOCUMENTATION_INDEX.md)
- [دليل المساهمة / Contributing Guide](./CONTRIBUTING.md)

---

*آخر تحديث / Last Updated*: 2025-11-25
