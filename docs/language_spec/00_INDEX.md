# 📚 مواصفات لغة "ص" (Sad Language Specification)

**تاريخ الإنشاء:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مسودة عمل (Working Draft)

---

## 🎯 نظرة عامة / Overview

### (AR) نظرة عامة
هذه المواصفات الرسمية للغة البرمجة "ص" - أول لغة برمجة عربية حديثة واحترافية. تجمع اللغة بين سهولة Python وقوة C++ مع دعم كامل للبرمجة الكائنية والبرمجة الوظيفية.

### (EN) Overview
This is the formal specification for Sad Programming Language - the first modern professional Arabic programming language. The language combines Python's ease of use with C++'s power, with full support for object-oriented and functional programming.

---

## 📋 جدول المحتويات / Table of Contents

### 1️⃣ الأساسيات / Fundamentals

| الملف / File | الوصف (AR) | Description (EN) | الحالة / Status |
|-------------|-----------|-----------------|-----------------|
| [01_types.md](01_types.md) | أنواع البيانات الأساسية والمتقدمة | Basic and advanced data types | ✅ مكتمل |
| [02_functions.md](02_functions.md) | الدوال والبرمجة الوظيفية | Functions and functional programming | ✅ مكتمل |
| [03_oop.md](03_oop.md) | البرمجة الكائنية | Object-oriented programming | ✅ مكتمل |

### 2️⃣ الميزات المتقدمة / Advanced Features

| الملف / File | الوصف (AR) | Description (EN) | الحالة / Status |
|-------------|-----------|-----------------|-----------------|
| [04_python_features.md](04_python_features.md) | ميزات مستوحاة من Python | Python-inspired features | ✅ مكتمل |
| [05_cpp_features.md](05_cpp_features.md) | ميزات مستوحاة من C++ | C++-inspired features | ✅ مكتمل |
| [06_syntax_examples.md](06_syntax_examples.md) | أمثلة كاملة للصيغ النحوية | Complete syntax examples | ✅ مكتمل |

### 3️⃣ ميزات مفصلة / Detailed Features

| الملف / File | الوصف (AR) | Description (EN) | الحالة / Status |
|-------------|-----------|-----------------|-----------------|
| [07_feature_by_file/generics.md](07_feature_by_file/generics.md) | القوالب العامة | Generics/Templates | 📝 قادم |
| [07_feature_by_file/gc_model.md](07_feature_by_file/gc_model.md) | نموذج جمع القمامة | Garbage collection model | 📝 قادم |
| [07_feature_by_file/async.md](07_feature_by_file/async.md) | البرمجة غير المتزامنة | Asynchronous programming | 📝 قادم |
| [07_feature_by_file/modules.md](07_feature_by_file/modules.md) | نظام الوحدات | Module system | 📝 قادم |
| [07_feature_by_file/ffi.md](07_feature_by_file/ffi.md) | الربط بلغات أخرى | Foreign function interface | 📝 قادم |

---

## 🎨 فلسفة التصميم / Design Philosophy

### المبادئ الأساسية / Core Principles

#### 1. سهولة القراءة / Readability
```s
# (AR) الكود يجب أن يكون واضحاً مثل اللغة الطبيعية
# (EN) Code should be as clear as natural language

دالة احسب_المجموع(قائمة أرقام)  
    رقم مجموع = 0
    لكل رقم في أرقام
        مجموع = مجموع + رقم
    نهاية
    إرجاع مجموع
نهاية
```

#### 2. الأداء / Performance
- (AR) توازن بين سهولة الاستخدام والأداء العالي
- (EN) Balance between ease of use and high performance
- JIT compilation للكود الحرج
- Memory management ذكي (GC + RAII)

#### 3. التوافقية / Compatibility
- (AR) قابلة للتكامل مع C/C++ و Python
- (EN) Interoperable with C/C++ and Python
- FFI بسيط وآمن
- Native bindings سهلة

---

## 📊 نظرة على الأنواع / Type System Overview

### التصنيف / Classification

```
أنواع البيانات (Data Types)
├── أساسية (Primitive)
│   ├── رقم (Integer)
│   ├── عشري (Float)
│   ├── منطقي (Boolean)
│   └── نص (String)
│
├── مركبة (Compound)
│   ├── مصفوفة (Array/List)
│   ├── خريطة (Map/Dict)
│   └── صف (Tuple)
│
├── مرجعية (Reference)
│   ├── دالة (Function)
│   └── كائن (Object)
│
└── خاصة (Special)
    ├── لاشيء (None/Null)
    └── نوع (Type)
```

---

## 🔤 البنية النحوية الأساسية / Basic Syntax

### الكلمات المفتاحية / Keywords (45 كلمة)

#### التحكم / Control Flow
```
إذا (if) | وإلا (else) | وإلا_إذا (elif)
بينما (while) | لكل (for) | في (in)
اخرج (break) | استمر (continue) | إرجاع (return)
```

#### التعريفات / Declarations
```
دالة (function) | صنف (class) | متغير (var)
ثابت (const) | استورد (import) | صدّر (export)
```

#### الأنواع / Types
```
رقم (int) | عشري (float) | نص (string)
منطقي (bool) | فراغ (void) | لاشيء (none)
```

#### البرمجة الكائنية / OOP
```
جديد (new) | هذا (this) | منشئ (constructor)
عام (public) | خاص (private) | محمي (protected)
يرث (extends) | ينفذ (implements)
```

---

## 🚀 مثال تطبيق كامل / Complete Application Example

```s
# ========================================
# (AR) مثال: نظام إدارة طلاب
# (EN) Example: Student Management System
# ========================================

استورد معياري.قوائم
استورد معياري.إخراج

# (AR) تعريف صنف الطالب
# (EN) Define Student class
صنف طالب
    خاص:
        نص الاسم
        رقم العمر
        عشري المعدل
    
    عام:
        # (AR) المنشئ
        # (EN) Constructor
        منشئ طالب(نص اسم, رقم عمر, عشري معدل)
            هذا.الاسم = اسم
            هذا.العمر = عمر
            هذا.المعدل = معدل
        نهاية
        
        # (AR) دالة للطباعة
        # (EN) Print method
        دالة اطبع_المعلومات()
            اطبع("الاسم: ", هذا.الاسم)
            اطبع("العمر: ", هذا.العمر)
            اطبع("المعدل: ", هذا.المعدل)
        نهاية
        
        # (AR) دالة للتحقق من النجاح
        # (EN) Check if passed
        دالة منطقي ناجح() 
            إرجاع هذا.المعدل >= 50.0
        نهاية
نهاية

# (AR) الدالة الرئيسية
# (EN) Main function
دالة رئيسية() = رقم
    # (AR) إنشاء قائمة طلاب
    # (EN) Create student list
    مصفوفة<طالب> طلاب = []
    
    # (AR) إضافة طلاب
    # (EN) Add students
    طلاب.أضف(طالب("أحمد محمد", 20, 85.5))
    طلاب.أضف(طالب("فاطمة علي", 19, 92.0))
    طلاب.أضف(طالب("خالد سعيد", 21, 45.0))
    
    # (AR) طباعة معلومات الطلاب الناجحين
    # (EN) Print info of passed students
    اطبع("=== الطلاب الناجحون ===")
    لكل طالب في طلاب
        إذا طالب.ناجح()
            طالب.اطبع_المعلومات()
            اطبع("---")
        نهاية
    نهاية
    
    إرجاع 0
نهاية
```

**الإخراج المتوقع / Expected Output:**
```
=== الطلاب الناجحون ===
الاسم: أحمد محمد
العمر: 20
المعدل: 85.5
---
الاسم: فاطمة علي
العمر: 19
المعدل: 92.0
---
```

---

## 🔧 التنفيذ / Implementation

### مراحل التنفيذ / Implementation Phases

| المرحلة / Phase | المكون / Component | الوقت المقدر / Est. Time | الحالة / Status |
|----------------|-------------------|------------------------|-----------------|
| 1 | Lexer | 1 أسبوع / week | ✅ مكتمل |
| 2 | Parser & AST | 2 أسابيع / weeks | 🔄 جاري |
| 3 | Type System | 1 أسبوع / week | ⏳ قادم |
| 4 | Interpreter | 1.5 أسبوع / week | ⏳ قادم |
| 5 | OOP System | 2 أسابيع / weeks | ⏳ قادم |
| 6 | Standard Library | 1 أسبوع / week | ⏳ قادم |
| 7 | Advanced Features | 3 أسابيع / weeks | ⏳ قادم |

---

## 📖 كيفية قراءة المواصفات / How to Read This Spec

### للمطورين / For Developers
1. ابدأ بـ [01_types.md](01_types.md) لفهم نظام الأنواع
2. اقرأ [02_functions.md](02_functions.md) للدوال
3. راجع [03_oop.md](03_oop.md) للبرمجة الكائنية
4. استكشف الميزات المتقدمة حسب الحاجة

### لمصممي اللغات / For Language Designers
1. راجع فلسفة التصميم أعلاه
2. ادرس قواعد BNF/EBNF في كل ملف
3. قارن مع [المعمارية](../../معمارية_لغة_ص.md)
4. راجع ملفات الميزات المفصلة

### لمستخدمي اللغة / For Language Users
1. ابدأ بـ [06_syntax_examples.md](06_syntax_examples.md)
2. راجع الأمثلة في كل قسم
3. جرب الأمثلة في `tests/spec_tests/`
4. ارجع للمواصفات عند الحاجة

---

## 🧪 الاختبارات / Tests

### موقع الاختبارات / Test Location
```
tests/spec_tests/
├── 01_integers_test.s          # اختبارات الأرقام الصحيحة
├── 02_strings_test.s           # اختبارات النصوص
├── 03_functions_test.s         # اختبارات الدوال
├── 04_classes_test.s           # اختبارات الأصناف
├── 05_generators_test.s        # اختبارات المولدات
├── 06_generics_test.s          # اختبارات القوالب
├── 07_exceptions_test.s        # اختبارات الاستثناءات
└── 08_modules_test.s           # اختبارات الوحدات
```

### تشغيل الاختبارات / Running Tests
```powershell
# تشغيل كل اختبارات المواصفات
cd C:\s\s_language\build
ctest -R spec_tests --output-on-failure

# تشغيل اختبار محدد
sad.exe tests\spec_tests\03_functions_test.s
```

---

## 🔗 روابط مفيدة / Useful Links

### الوثائق الداخلية / Internal Documentation
- [المعمارية الكاملة](../../معمارية_لغة_ص.md)
- [خارطة الطريق](../../ROADMAP.md)
- [خطة التطوير](../../plans/00_MASTER_PLAN.md)
- [دليل المساهمة](../../plans/08_QUICK_START.md)

### المراجع الخارجية / External References
- [Python Language Reference](https://docs.python.org/3/reference/)
- [C++ Standard](https://isocpp.org/std/the-standard)
- [LLVM Language Reference](https://llvm.org/docs/LangRef.html)

---

## 📝 سجل التغييرات / Changelog

### الإصدار 1.0 (4 نوفمبر 2025)
- ✅ إنشاء البنية الأساسية للمواصفات
- ✅ تحديد أنواع البيانات الأساسية
- ✅ تصميم بنية الدوال
- ✅ تصميم نظام OOP
- ✅ تحديد الميزات المستوحاة من Python و C++

---

## 👥 المساهمون / Contributors

- **Sad Language Team** - التصميم والتنفيذ / Design & Implementation
- **المجتمع** - المراجعة والتغذية الراجعة / Review & Feedback

---

## 📄 الترخيص / License

هذه المواصفات مفتوحة المصدر تحت رخصة MIT.  
This specification is open source under MIT License.

---

## 📞 التواصل / Contact

- **GitHub:** https://github.com/yourusername/s_language
- **Issues:** للأسئلة والاقتراحات / For questions and suggestions
- **Email:** dev@sadlang.org

---

**آخر تحديث:** 4 نوفمبر 2025  
**Last Updated:** November 4, 2025

**لنبني معاً لغة برمجة عربية احترافية! 🇸🇦✨**  
**Let's build a professional Arabic programming language together! 🇸🇦✨**
