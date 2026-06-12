# SIR Optimizer Tests
# اختبارات محسّن SIR

هذا المجلد يحتوي على جميع اختبارات محسّن SIR.  
This directory contains all SIR optimizer tests.

---

## 📁 الملفات / Files

### أدوات الاختبار / Test Utilities
- **sir_test_utils.h** (200 lines) - واجهة أدوات الاختبار
- **sir_test_utils.cpp** (450 lines) - تطبيق أدوات الاختبار

### ملفات الاختبار / Test Files  
- **test_constant_folding.cpp** (514 lines) - 30+ اختبار لطي الثوابت
- **test_dead_code_elimination.cpp** (523 lines) - 25+ اختبار لحذف الكود الميت
- **test_optimizer.cpp** (494 lines) - 30+ اختبار للتكامل

### البناء / Build
- **CMakeLists.txt** (200 lines) - إعدادات CMake

---

## 🚀 البناء والتشغيل / Build and Run

### المتطلبات / Requirements
- CMake 3.15+
- Visual Studio 2017+
- Google Test (يُنصح به / recommended)

### التثبيت / Installation

#### الطريقة 1: استخدام vcpkg (موصى بها)
```powershell
# تثبيت Google Test
vcpkg install gtest:x64-windows
vcpkg integrate install
```

#### الطريقة 2: يدوياً
انظر [GTEST_SETUP.md](../../GTEST_SETUP.md) للتعليمات المفصلة.

### البناء / Building

```powershell
# في مجلد build
cd C:\s\s_language\build

# تكوين مع الاختبارات
cmake .. -DBUILD_TESTS=ON -DCMAKE_TOOLCHAIN_FILE=<vcpkg-path>/scripts/buildsystems/vcpkg.cmake

# بناء
cmake --build . --config Debug
```

### التشغيل / Running

```powershell
# جميع الاختبارات
ctest -C Debug --verbose

# اختبارات منفصلة
.\bin\Debug\constant_folding_tests.exe
.\bin\Debug\dead_code_elimination_tests.exe
.\bin\Debug\optimizer_integration_tests.exe
```

---

## 📝 كتابة الاختبارات / Writing Tests

### استخدام SIRTestBuilder

```cpp
#include "sir_test_utils.h"

TEST_F(MyTest, TestName) {
    // إنشاء وحدة اختبار
    SIRTestBuilder builder;
    auto module = builder.createModule("test");
    auto func = builder.createFunction("main");
    auto block = builder.createBlock("entry");
    
    // إضافة تعليمات
    builder.addInstruction(block, 
        builder.createLoadConst(42, "%1"));
    builder.addInstruction(block, 
        builder.createReturn("%1"));
    builder.addBlock(func, block);
    
    // تشغيل المحسّن
    Optimizer opt(OptimizationLevel::O2);
    opt.optimize(module);
    
    // التحقق
    EXPECT_EQ(countInstructions(func), 2);
}
```

### الدوال المساعدة السريعة / Quick Helpers

```cpp
// وحدات جاهزة للاختبار
auto module1 = createSimpleModule();
auto module2 = createConstantArithmeticModule();
auto module3 = createDeadCodeModule();
auto module4 = createUnreachableBlocksModule();
auto module5 = createBranchingModule();
auto module6 = createComplexModule();
```

### دوال التحقق / Verification Functions

```cpp
// عد التعليمات والكتل
int instCount = countInstructions(func);
int blockCount = countBlocks(func);

// تحقق من وجود تعليمة
bool hasAdd = hasInstruction(func, SIROpcode::ADD_I64);

// طباعة للتنقيح
printFunction(func);
printModule(module);
```

---

## 📊 الاختبارات المتاحة / Available Tests

### 1. Constant Folding (30+ tests)
- ✅ Integer arithmetic (add, sub, mul, div, mod)
- ✅ Float arithmetic (add, sub, mul, div)
- ✅ Bitwise operations (and, or, xor, not, shl, shr)
- ✅ Comparisons (eq, ne, lt, le, gt, ge)
- ✅ Unary operations (neg, not)
- ✅ Overflow protection
- ✅ Division by zero protection

### 2. Dead Code Elimination (25+ tests)
- ✅ Side effects detection
- ✅ Dead instruction removal
- ✅ Liveness analysis
- ✅ Unreachable block removal
- ✅ Definition-use chains
- ✅ BFS traversal

### 3. Optimizer Integration (30+ tests)
- ✅ Optimization levels (O0, O1, O2, O3)
- ✅ Pass management
- ✅ Iterative optimization
- ✅ Statistics collection
- ✅ Real-world examples
- ✅ Performance tests

**المجموع / Total**: 85+ tests

---

## 🎯 الحالة / Status

```
Test Infrastructure:    ✅ COMPLETE (100%)
Test Implementation:    ⏳ IN PROGRESS (10%)
Test Execution:         ⏳ PENDING (0%)
```

### ما هو جاهز / What's Ready
- ✅ جميع أدوات الإنشاء
- ✅ جميع الدوال المساعدة
- ✅ جميع دوال التحقق
- ✅ بنية CMake كاملة
- ✅ تكامل Google Test

### ما يحتاج عمل / What Needs Work
- ⏳ تطبيق الاختبارات الفعلية (استبدال SUCCEED)
- ⏳ تشغيل وتحقق
- ⏳ إصلاح الأخطاء

---

## 📚 المراجع / References

- [Test Infrastructure Report](../../TEST_INFRASTRUCTURE_REPORT.md)
- [Quick Status](../../QUICK_TEST_STATUS.md)
- [Google Test Setup](../../GTEST_SETUP.md)
- [Session Completion](../../SESSION_COMPLETION_DEC3_EVENING.md)

---

## 🔧 استكشاف الأخطاء / Troubleshooting

### لا يمكن العثور على Google Test
```
⚠ Google Test not found
```
**الحل / Solution**: اتبع التعليمات في [GTEST_SETUP.md](../../GTEST_SETUP.md)

### أخطاء الربط
```
unresolved external symbol
```
**الحل / Solution**: تأكد من بناء sad_core أولاً

### أخطاء التضمين
```
cannot open include file
```
**الحل / Solution**: تحقق من مسارات التضمين في CMakeLists.txt

---

## 💡 نصائح / Tips

1. **استخدم Quick Helpers** - أسرع من البناء يدوياً
2. **اطبع للتنقيح** - استخدم printFunction/printModule
3. **ابدأ بسيط** - اختبر وحدة صغيرة أولاً
4. **تحقق بعد كل خطوة** - لا تبني الكثير قبل الاختبار

---

## 📞 الدعم / Support

إذا واجهت مشاكل، راجع:
- التوثيق في المجلد الجذر
- ملفات الرأس للواجهات API
- الاختبارات الموجودة كأمثلة

---

**الحمد لله / Alhamdulillah**

*آخر تحديث: ٣ ديسمبر ٢٠٢٥*  
*Last Updated: December 3, 2025*
