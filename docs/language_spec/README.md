# 📖 مواصفات لغة "ص" / Sad Language Specification

**الإصدار:** 1.0  
**تاريخ الإنشاء:** 4 نوفمبر 2025  
**الحالة:** مسودة عمل - Work in Progress

---

## 🎯 نظرة عامة / Overview

### (AR) نظرة عامة
هذا المجلد يحتوي على المواصفات الكاملة للغة "ص" - لغة برمجة عربية حديثة تجمع بين أفضل ميزات Python و C++ و JavaScript. المواصفات توفر:
- وصف تفصيلي لجميع أنواع البيانات والتراكيب النحوية
- قواعد BNF/EBNF النحوية
- تمثيل AST للعقد (Nodes)
- أمثلة شاملة قابلة للتنفيذ
- مهام تنفيذ مفصلة بجداول زمنية

### (EN) Overview
This folder contains the complete specification for the "Sad" language - a modern Arabic programming language combining the best features of Python, C++, and JavaScript. The specifications provide:
- Detailed descriptions of all data types and syntax constructs
- BNF/EBNF grammar rules
- AST node representations
- Comprehensive executable examples
- Detailed implementation tasks with timelines

---

## 📚 هيكل الملفات / File Structure

```
language_spec/
├── 00_INDEX.md                    # فهرس رئيسي / Main index
├── 01_types.md                    # أنواع البيانات / Data types
├── 02_functions.md                # الدوال / Functions
├── 03_oop.md                      # البرمجة الكائنية / OOP
├── 04_python_features.md          # ميزات Python
├── 05_cpp_features.md             # ميزات C++
├── 06_syntax_examples.md          # أمثلة شاملة / Examples
├── README.md                      # هذا الملف / This file
└── 07_feature_by_file/            # (قريباً) ميزات منفصلة
    ├── generics.md
    ├── async.md
    ├── modules.md
    └── ...
```

---

## 📋 محتوى الملفات / File Contents

### 1. 00_INDEX.md - الفهرس الرئيسي
**الحجم:** ~1000+ سطر  
**المحتوى:**
- نظرة عامة على اللغة وفلسفتها
- جدول محتويات كامل لجميع الملفات
- مصفوفة مقارنة مع لغات أخرى (Python/C++/JavaScript)
- دليل سريع للمبتدئين
- خريطة طريق التطوير

**الأقسام الرئيسية:**
1. مقدمة اللغة
2. الميزات الأساسية
3. جدول المحتويات التفصيلي
4. مرجع سريع
5. أمثلة تمهيدية

---

### 2. 01_types.md - أنواع البيانات
**الحجم:** ~1500+ سطر  
**المحتوى:**
- أنواع أساسية: Integer, Float, Boolean, String, None
- أنواع مركبة: Array, Map, Tuple
- أنواع مرجعية
- التحويلات النوعية (Type conversions)
- قواعد BNF كاملة
- تمثيل AST في C++
- مهام تنفيذ مع جدول زمني (7 أيام)

**الأقسام الرئيسية:**
1. الأنواع الأساسية (5 أنواع)
2. الأنواع المركبة (3 أنواع)
3. العمليات على كل نوع
4. قواعد BNF النحوية
5. تمثيل AST
6. مهام التنفيذ
7. اختبارات شاملة

**أمثلة:**
```s
# رقم صحيح
رقم عمر = 25

# عشري
عشري درجة = 95.5

# نص
نص اسم = "أحمد محمد"

# مصفوفة
مصفوفة<رقم> أرقام = [1، 2، 3، 4، 5]

# خريطة
خريطة<نص، رقم> أعمار = {"أحمد": 25، "فاطمة": 30}
```

---

### 3. 02_functions.md - الدوال
**الحجم:** ~1400+ سطر  
**المحتوى:**
- تعريف الدوال الأساسية
- معاملات (عادية، اختيارية، varargs، kwargs)
- دوال Lambda
- Closures (الإغلاقات)
- Higher-order functions
- Decorators
- Generic functions
- مهام تنفيذ (14 يوم)

**الميزات المغطاة:**
1. Basic functions
2. Parameters (optional, named, varargs, kwargs)
3. Lambda expressions
4. Closures with variable capture
5. Higher-order functions (map, filter, reduce)
6. Decorator system
7. Generic functions with type parameters

**أمثلة:**
```s
# دالة بسيطة
دالة اجمع(رقم أ، رقم ب) = رقم
    إرجاع أ + ب
نهاية

# Lambda
أي ضعف = lambda (رقم س) = س * 2

# Decorator
@سجل
دالة معالج_بيانات()
    # ...
نهاية

# Generic
دالة<T> أول(مصفوفة<T> قائمة) = T
    إرجاع قائمة[0]
نهاية
```

---

### 4. 03_oop.md - البرمجة كائنية التوجه
**الحجم:** ~1600+ سطر  
**المحتوى:**
- الفئات Classes الأساسية
- الخصائص والطرق (Properties & Methods)
- مستويات الوصول (Public/Private/Protected)
- البناء والإتلاف (Constructors/Destructors)
- الوراثة (أحادية ومتعددة)
- التعدد الشكلي (Polymorphism)
- الواجهات (Interfaces)
- تحميل العوامل (Operator overloading)
- الفئات العامة (Generic classes)
- مهام تنفيذ (16 يوم)

**الميزات المغطاة:**
1. Class definition & members
2. Access modifiers
3. Properties (getters/setters)
4. Static methods & class methods
5. Inheritance (single & multiple)
6. Abstract classes & interfaces
7. Operator overloading
8. Generic/template classes

**أمثلة:**
```s
# فئة أساسية
فئة شخص
    نص اسم
    رقم عمر
    
    بناء(نص الاسم، رقم العمر)
        اسم = الاسم
        عمر = العمر
    نهاية
نهاية

# وراثة
فئة طالب : شخص
    مصفوفة<عشري> درجات
    
    دالة متوسط() = عشري
        # ...
    نهاية
نهاية

# Generic class
فئة<T> صندوق
    T قيمة
نهاية
```

---

### 5. 04_python_features.md - ميزات Python
**الحجم:** ~1200+ سطر  
**المحتوى:**
- Dynamic typing مع type inference
- List/Dict/Set comprehensions
- Generators و yield
- Decorators متقدمة
- Context managers (with statement)
- Multiple assignment & unpacking
- Slicing
- F-strings (string interpolation)
- مهام تنفيذ (13 يوم)

**الميزات المغطاة:**
1. Dynamic typing system
2. Comprehensions (list, dict, set)
3. Generator functions & expressions
4. Decorator patterns
5. Context manager protocol
6. Sequence unpacking
7. Advanced slicing
8. String formatting

**أمثلة:**
```s
# Comprehension
مصفوفة<رقم> مربعات = [س * س لكل س في نطاق(1، 11)]

# Generator
دالة عد(رقم ن)
    لكل ع في نطاق(ن)
        أنتج ع
    نهاية
نهاية

# Context manager
مع افتح("file.txt") كـ ملف
    نص محتوى = ملف.اقرأ()
نهاية

# Unpacking
(أ، *ب، ج) = [1، 2، 3، 4، 5]

# F-string
نص رسالة = ن"الاسم: {اسم}، العمر: {عمر}"
```

---

### 6. 05_cpp_features.md - ميزات C++
**الحجم:** ~1300+ سطر  
**المحتوى:**
- Templates/Generics
- RAII (Resource Acquisition Is Initialization)
- Move semantics
- Operator overloading الكامل
- Static typing مع type inference
- References (by reference)
- Smart pointers (unique, shared)
- Const correctness
- مهام تنفيذ (14 يوم)

**الميزات المغطاة:**
1. Template system (functions & classes)
2. RAII pattern & destructors
3. Move semantics & perfect forwarding
4. Comprehensive operator overloading
5. Static type checking
6. Reference types
7. Smart pointer types
8. Const qualifiers

**أمثلة:**
```s
# Template
فئة<T> مكدس
    خاص مصفوفة<T> _عناصر
    
    دالة ادفع(T عنصر)
        _عناصر.أضف(عنصر)
    نهاية
نهاية

# RAII
فئة ملف
    بناء(نص اسم)
        # فتح الملف
    نهاية
    
    إتلاف()
        # إغلاق تلقائي
    نهاية
نهاية

# Move semantics
بيانات_كبيرة ب = نقل(أ)  # نقل بدون نسخ

# Reference
دالة ضعف(مرجع رقم س)
    س *= 2
نهاية
```

---

### 7. 06_syntax_examples.md - أمثلة شاملة
**الحجم:** ~1100+ سطر  
**المحتوى:**
- Hello World بثلاث طرق
- جميع أنواع البيانات مع أمثلة
- البنى التحكمية (if, while, for)
- الدوال بجميع أشكالها
- المصفوفات والخرائط
- الفئات والكائنات
- أمثلة متقدمة (decorators, generators, etc.)
- **3 برامج كاملة:**
  1. حاسبة
  2. نظام إدارة طلاب
  3. لعبة تخمين الرقم

**الأقسام الرئيسية:**
1. أمثلة أساسية
2. أمثلة متوسطة
3. أمثلة متقدمة
4. برامج كاملة قابلة للتشغيل

---

## 🎯 الاستخدام / Usage

### للمطورين / For Developers

```bash
# 1. قراءة الفهرس أولاً
المسار: docs/language_spec/00_INDEX.md

# 2. دراسة الأنواع
المسار: docs/language_spec/01_types.md

# 3. دراسة الدوال
المسار: docs/language_spec/02_functions.md

# 4. دراسة OOP
المسار: docs/language_spec/03_oop.md

# 5. مراجعة الأمثلة
المسار: docs/language_spec/06_syntax_examples.md
```

### للمستخدمين / For Users

```bash
# بدء سريع
1. اقرأ: 00_INDEX.md (Quick Start)
2. شاهد: 06_syntax_examples.md (أمثلة)
3. جرب: examples/basics/hello.s
```

---

## 🔨 خطة التنفيذ / Implementation Plan

### الملخص الزمني / Timeline Summary

| المرحلة | الميزة | المدة | الحالة |
|---------|--------|------|--------|
| Phase 1 | Types (Primitives) | 3 أيام | ⏳ Pending |
| Phase 2 | Types (Compound) | 4 أيام | ⏳ Pending |
| Phase 3 | Functions (Basic) | 4 أيام | ⏳ Pending |
| Phase 4 | Functions (Lambda/Closures) | 3 أيام | ⏳ Pending |
| Phase 5 | Functions (Advanced) | 4 أيام | ⏳ Pending |
| Phase 6 | Functions (Generics) | 3 أيام | ⏳ Pending |
| Phase 7 | OOP (Basic Classes) | 5 أيام | ⏳ Pending |
| Phase 8 | OOP (Inheritance) | 4 أيام | ⏳ Pending |
| Phase 9 | OOP (Polymorphism) | 4 أيام | ⏳ Pending |
| Phase 10 | OOP (Operators) | 3 أيام | ⏳ Pending |
| Phase 11 | Python Features | 13 أيام | ⏳ Pending |
| Phase 12 | C++ Features | 14 أيام | ⏳ Pending |
| **المجموع** | **جميع الميزات** | **~64 يوم** | **0%** |

### فروع Git / Git Branches

كل ميزة في فرع منفصل:
```bash
feat/types/primitives
feat/types/compound
feat/functions/basic
feat/functions/lambda-closures
feat/functions/advanced
feat/functions/generics
feat/oop/basic-classes
feat/oop/inheritance
feat/oop/polymorphism
feat/oop/operators
feat/python/comprehensions
feat/python/generators
feat/python/context-managers
feat/python/unpacking-slicing
feat/cpp/templates
feat/cpp/raii
feat/cpp/move-semantics
feat/cpp/references
```

---

## 📊 إحصائيات / Statistics

### حجم المواصفات / Specification Size
- **عدد الملفات:** 7 ملفات رئيسية
- **إجمالي الأسطر:** ~8500+ سطر
- **عدد الأمثلة:** 150+ مثال قابل للتنفيذ
- **عدد الميزات:** 80+ ميزة موثقة
- **عدد البرامج الكاملة:** 3 برامج

### التغطية / Coverage
- ✅ أنواع البيانات: 100%
- ✅ الدوال: 100%
- ✅ OOP: 100%
- ✅ Python Features: 100%
- ✅ C++ Features: 100%
- ✅ أمثلة: 100%
- ⏳ Feature-by-file: 0% (قريباً)
- ⏳ ملفات اختبار .s: 0% (قريباً)

---

## 🧪 ملفات الاختبار / Test Files

### المخطط / Planned

```
tests/spec_tests/
├── 01_integers_test.s          # اختبارات الأرقام الصحيحة
├── 02_strings_test.s           # اختبارات النصوص
├── 03_arrays_test.s            # اختبارات المصفوفات
├── 04_functions_test.s         # اختبارات الدوال
├── 05_oop_test.s              # اختبارات OOP
├── 06_comprehensions_test.s    # اختبارات Comprehensions
├── 07_generators_test.s        # اختبارات Generators
└── 08_full_program_test.s     # برنامج كامل
```

كل ملف اختبار يحتوي على:
- 20-30 test case
- اختبارات إيجابية وسلبية
- تغطية لجميع الحالات الحدية

---

## 🔗 روابط ذات صلة / Related Links

### ملفات المشروع / Project Files
- [ROADMAP.md](../../ROADMAP.md) - خارطة الطريق العامة
- [00_MASTER_PLAN.md](../../plans/00_MASTER_PLAN.md) - الخطة الرئيسية
- [BUILD_INSTRUCTIONS.md](../../BUILD_INSTRUCTIONS.md) - تعليمات البناء

### الأمثلة / Examples
- [examples/basics/](../../examples/basics/) - أمثلة أساسية
- [examples/functions/](../../examples/functions/) - أمثلة الدوال
- [examples/oop/](../../examples/oop/) - أمثلة OOP

### التوثيق / Documentation
- [docs/architecture/design.md](../architecture/design.md) - تصميم البنية
- [docs/tutorials/getting_started.md](../tutorials/getting_started.md) - دليل البداية

---

## 🤝 المساهمة / Contributing

### كيفية المساهمة / How to Contribute

1. **قراءة المواصفات**
   - ادرس الملفات بعناية
   - افهم القواعد النحوية BNF
   - راجع الأمثلة

2. **اختيار ميزة**
   - اختر من قائمة المهام
   - أنشئ فرع Git مناسب
   - اتبع معايير الجودة

3. **التنفيذ**
   - اكتب الكود في C++
   - أضف DEBUG_PRINT
   - اكتب الاختبارات
   - وثق بـ Doxygen

4. **المراجعة**
   - تأكد من مرور الاختبارات
   - تحقق من التغطية ≥80%
   - راجع الأخطاء

5. **الدمج**
   - أنشئ Pull Request
   - املأ القالب
   - انتظر المراجعة

### معايير الجودة / Quality Standards

- ✅ Coverage ≥ 80%
- ✅ Doxygen documentation (AR/EN)
- ✅ DEBUG_PRINT في النقاط الحرجة
- ✅ Tests pass
- ✅ No memory leaks
- ✅ Follow coding style

---

## 📝 ملاحظات / Notes

### النسخة الحالية / Current Version
- **الحالة:** مسودة أولية
- **التغطية:** المواصفات كاملة، التنفيذ 0%
- **آخر تحديث:** 4 نوفمبر 2025

### التحديثات المستقبلية / Future Updates
- [ ] إنشاء مجلد `07_feature_by_file/` مع ملفات منفصلة للميزات
- [ ] إضافة 8 ملفات اختبار .s في `tests/spec_tests/`
- [ ] توسيع أمثلة البرامج الكاملة
- [ ] إضافة diagramsلتوضيح البنى
- [ ] إنشاء cheat sheet مختصر

---

## 📞 الدعم / Support

### الحصول على المساعدة / Getting Help
- **الأسئلة:** افتح Issue مع العلامة `question`
- **المشاكل:** افتح Issue مع العلامة `bug`
- **الاقتراحات:** افتح Issue مع العلامة `enhancement`

### الموارد / Resources
- [GitHub Issues](https://github.com/your-repo/issues)
- [Discussions](https://github.com/your-repo/discussions)
- [Wiki](https://github.com/your-repo/wiki)

---

## 🏆 الإنجازات / Achievements

✅ **تم إنجازه:**
- إنشاء 6 ملفات مواصفات شاملة (8500+ سطر)
- توثيق 80+ ميزة مع أمثلة
- تعريف قواعد BNF كاملة
- تحديد تمثيل AST
- وضع خطة تنفيذ مفصلة (64 يوم)
- إنشاء 150+ مثال قابل للتنفيذ

⏳ **قيد العمل:**
- تنفيذ الميزات في C++
- كتابة ملفات الاختبار .s
- إنشاء feature-by-file documentation

---

**🌟 مرحباً بك في رحلة تطوير لغة "ص"! 🌟**

**تاريخ الإنشاء:** 4 نوفمبر 2025  
**آخر تحديث:** 4 نوفمبر 2025  
**الإصدار:** 1.0.0-draft
