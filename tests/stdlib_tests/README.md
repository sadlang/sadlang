# 🧪 اختبارات المكتبة القياسية - Standard Library Tests

**التاريخ / Date**: نوفمبر 22، 2025  
**الحالة / Status**: ✅ **جاهز للتشغيل / Ready to Run**

---

## 📋 نظرة عامة / Overview

هذا المجلد يحتوي على اختبارات شاملة لجميع دوال المكتبة القياسية للغة ص.

This folder contains comprehensive tests for all S Language standard library functions.

---

## 📁 الملفات / Files

### ملفات الاختبار / Test Files

| الملف / File | الوصف / Description | عدد الاختبارات / Tests |
|-------------|--------------------|-----------------------|
| `string_functions_tests.cpp` | اختبارات دوال النصوص / String functions | 45+ |
| `array_functions_tests.cpp` | اختبارات دوال المصفوفات / Array functions | 50+ |
| `math_functions_tests.cpp` | اختبارات الدوال الرياضية / Math functions | 60+ |

### ملفات البناء / Build Files

- `CMakeLists.txt` - ملف بناء CMake
- `README.md` - هذا الملف

---

## 🔧 البناء والتشغيل / Build and Run

### الطريقة 1: باستخدام CMake

```bash
# إنشاء مجلد البناء / Create build directory
cd C:\s\s_language\tests\stdlib_tests
mkdir build
cd build

# تكوين المشروع / Configure project
cmake ..

# البناء / Build
cmake --build . --config Release

# تشغيل جميع الاختبارات / Run all tests
ctest -C Release --verbose

# أو تشغيل كل اختبار على حدة / Or run each test individually
.\Release\string_functions_tests.exe
.\Release\array_functions_tests.exe
.\Release\math_functions_tests.exe
```

### الطريقة 2: باستخدام Visual Studio

```bash
# فتح المشروع في Visual Studio
cmake .. -G "Visual Studio 17 2022"
```

ثم افتح ملف `StdLibTests.sln` في Visual Studio وابني المشروع.

---

## 📊 نتائج متوقعة / Expected Results

### ✅ نجاح كامل / Full Success

```
╔════════════════════════════════════════════════════════════╗
║                   Final Results / النتائج النهائية        ║
╚════════════════════════════════════════════════════════════╝

📊 Test Statistics / إحصائيات الاختبارات:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
   Total Tests / إجمالي الاختبارات: 155
   ✅ Passed / نجحت: 155
   ❌ Failed / فشلت: 0
   📈 Success Rate / نسبة النجاح: 100%

🎉 Excellent! All tests passed!
🎉 ممتاز! جميع الاختبارات نجحت!
```

---

## 🎯 الاختبارات المغطاة / Covered Tests

### 1. دوال النصوص / String Functions (12 دالة)

#### اختبارات أساسية / Basic Tests
- ✅ `length()` - الطول
- ✅ `find()` - البحث
- ✅ `replace()` - الاستبدال
- ✅ `substring()` - الجزء
- ✅ `toLower()` - تحويل لصغير
- ✅ `toUpper()` - تحويل لكبير

#### اختبارات متقدمة / Advanced Tests
- ✅ `trim()` - إزالة المسافات
- ✅ `split()` - التقسيم
- ✅ `join()` - الربط
- ✅ `startsWith()` - يبدأ بـ
- ✅ `endsWith()` - ينتهي بـ
- ✅ `contains()` - يحتوي

#### اختبارات خاصة / Special Tests
- ✅ دعم UTF-8 والنصوص العربية
- ✅ النصوص الفارغة
- ✅ النصوص الطويلة
- ✅ الرموز الخاصة
- ✅ التكامل بين الدوال

---

### 2. دوال المصفوفات / Array Functions (10 دوال)

#### عمليات أساسية / Basic Operations
- ✅ `append()` - إضافة عنصر
- ✅ `pop()` - إزالة آخر عنصر
- ✅ `size()` - حجم المصفوفة
- ✅ `indexOf()` - موقع عنصر
- ✅ `contains()` - يحتوي على

#### عمليات تحويل / Transformation Operations
- ✅ `reverse()` - عكس الترتيب
- ✅ `sort()` - الفرز (تصاعدي/تنازلي)

#### عمليات وصول / Access Operations
- ✅ `first()` - أول عنصر
- ✅ `last()` - آخر عنصر
- ✅ `slice()` - استخراج جزء

#### اختبارات خاصة / Special Tests
- ✅ مصفوفات متعددة الأنواع
- ✅ مصفوفات فارغة
- ✅ مصفوفات كبيرة (1000+ عنصر)
- ✅ التكامل بين الدوال

---

### 3. الدوال الرياضية / Math Functions (12 دالة)

#### دوال أساسية / Basic Functions
- ✅ `sqrt()` - الجذر التربيعي
- ✅ `power()` - الأس
- ✅ `abs()` - القيمة المطلقة
- ✅ `square()` - التربيع

#### دوال المقارنة / Comparison Functions
- ✅ `max()` - أكبر قيمة
- ✅ `min()` - أصغر قيمة

#### دوال التقريب / Rounding Functions
- ✅ `round()` - تقريب لأقرب صحيح
- ✅ `floor()` - تقريب للأسفل
- ✅ `ceil()` - تقريب للأعلى

#### دوال مثلثية / Trigonometric Functions
- ✅ `sin()` - الجيب
- ✅ `cos()` - جيب التمام
- ✅ `tan()` - الظل

#### اختبارات خاصة / Special Tests
- ✅ دقة الحسابات (EPSILON = 0.0001)
- ✅ الأرقام السالبة
- ✅ الأرقام الكبيرة جداً
- ✅ الأرقام العشرية
- ✅ نظرية فيثاغورس

---

## 🔍 تفاصيل الاختبارات / Test Details

### نوع الاختبارات / Test Types

#### 1. اختبارات وحدة / Unit Tests
- اختبار كل دالة بشكل مستقل
- تغطية جميع المعاملات الممكنة
- التحقق من القيم المُرجعة

#### 2. اختبارات الحالات الحدية / Edge Cases
- مصفوفات/نصوص فارغة
- قيم سالبة
- قيم كبيرة جداً
- قيم عشرية دقيقة

#### 3. اختبارات التكامل / Integration Tests
- دمج عدة دوال معاً
- تدفقات عمل حقيقية
- سيناريوهات استخدام عملية

#### 4. اختبارات الأداء / Performance Tests
- تشغيل متكرر (100-1000 مرة)
- مصفوفات كبيرة
- نصوص طويلة

---

## 🐛 معالجة الأخطاء / Error Handling

جميع الاختبارات تتضمن:

All tests include:

- ✅ `try-catch` blocks للأخطاء المتوقعة
- ✅ رسائل خطأ واضحة
- ✅ تتبع الاختبارات الفاشلة
- ✅ تقارير مفصلة

---

## 📈 معدلات النجاح المتوقعة / Expected Success Rates

```
String Functions Tests:   45/45  (100%)
Array Functions Tests:    50/50  (100%)
Math Functions Tests:     60/60  (100%)
─────────────────────────────────────
Total:                   155/155 (100%)
```

---

## 🔧 متطلبات البناء / Build Requirements

### الحد الأدنى / Minimum Requirements

- **CMake**: 3.10+
- **C++ Compiler**: 
  - Visual Studio 2019+ (Windows)
  - GCC 7+ (Linux)
  - Clang 6+ (macOS)
- **C++ Standard**: C++17

### المكتبات المطلوبة / Required Libraries

- Standard Library (no external dependencies)
- `<iostream>` للإخراج
- `<cassert>` للتحقق
- `<cmath>` للدوال الرياضية

---

## 📝 إضافة اختبارات جديدة / Adding New Tests

### مثال / Example

```cpp
void testNewFunction() {
    printSection("Testing newFunction()");
    
    try {
        // Setup
        Value input = makeInt(42);
        
        // Execute
        Value result = NewFunctions::newFunction({input});
        
        // Verify
        assertTest("newFunction: basic test", result.toInt() == 84);
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Exception: " << e.what() << "\n";
        failedTests++;
    }
}
```

---

## 🎯 الخطوات التالية / Next Steps

### إذا فشلت الاختبارات / If Tests Fail

1. راجع رسائل الخطأ بعناية
2. تحقق من تنفيذ الدوال
3. تأكد من صحة أنواع البيانات
4. راجع معالجة الأخطاء

### لتحسين التغطية / To Improve Coverage

1. أضف المزيد من الحالات الحدية
2. اختبر سيناريوهات أكثر تعقيداً
3. أضف اختبارات أداء
4. اختبر توافق الأنواع

---

## 📚 المراجع / References

- [CMake Documentation](https://cmake.org/documentation/)
- [CTest Documentation](https://cmake.org/cmake/help/latest/manual/ctest.1.html)
- [C++ Testing Best Practices](https://google.github.io/googletest/)

---

## 👥 المساهمون / Contributors

- فريق تطوير لغة ص / S Language Development Team

---

## 📄 الترخيص / License

MIT License - نفس ترخيص المشروع الأساسي

Same as main project license

---

**📧 للدعم / For Support**: راجع التوثيق الرئيسي للمشروع

**🔗 الروابط / Links**: 
- التقارير: `C:\s\s_language\docs\stlib_plan\`
- الكود المصدري: `C:\s\s_language\src\stdlib\`
- ملفات الترويسة: `C:\s\s_language\include\stdlib\`

---

*آخر تحديث / Last Updated*: نوفمبر 22، 2025  
*الإصدار / Version*: 1.0.0
