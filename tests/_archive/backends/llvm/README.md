# اختبارات LLVM Backend / LLVM Backend Tests

**بسم الله الرحمن الرحيم**

## نظرة عامة / Overview

هذا الدليل يحتوي على مجموعة شاملة من الاختبارات لمكونات LLVM Backend في لغة SadLang.

This directory contains a comprehensive test suite for the LLVM Backend components of the SadLang compiler.

---

## البنية / Structure

```
tests/backends/llvm/
├── test_framework.h         إطار عمل الاختبار / Test framework
├── test_helpers.h           دوال مساعدة / Helper functions
├── test_runner.cpp          برنامج التشغيل الرئيسي / Main runner
├── test_llvm_linker.cpp     اختبارات الرابط (40 اختبار) / Linker tests (40 tests)
├── test_llvm_optimizer.cpp  اختبارات المُحسّن (قريباً) / Optimizer tests (soon)
├── test_llvm_target.cpp     اختبارات الهدف (قريباً) / Target tests (soon)
├── test_llvm_debug.cpp      اختبارات التنقيح (قريباً) / Debug tests (soon)
├── test_llvm_runtime.cpp    اختبارات وقت التشغيل (قريباً) / Runtime tests (soon)
├── test_llvm_ir_gen.cpp     اختبارات مولد IR (قريباً) / IR gen tests (soon)
├── test_integration.cpp     اختبارات التكامل (قريباً) / Integration tests (soon)
├── CMakeLists.txt           تكوين البناء / Build configuration
└── README.md                هذا الملف / This file
```

---

## إطار عمل الاختبار / Test Framework

### الميزات / Features

- ✅ **إطار بسيط وفعال**: لا حاجة لمكتبات خارجية / Simple and effective: No external dependencies
- ✅ **ماكروهات سهلة الاستخدام**: TEST, EXPECT_TRUE, EXPECT_EQ, إلخ / Easy-to-use macros
- ✅ **تقارير مفصلة**: عرض النتائج مع الوقت والموقع / Detailed reports with time and location
- ✅ **دعم المجموعات**: تشغيل اختبارات محددة / Suite support: Run specific tests
- ✅ **ثنائي اللغة**: عربي وإنجليزي / Bilingual: Arabic and English

### الماكروهات المتاحة / Available Macros

```cpp
TEST(SuiteName, TestName)          // تعريف اختبار / Define a test
TEST_DISABLED(SuiteName, TestName) // اختبار معطّل / Disabled test

EXPECT_TRUE(condition)             // توقع صحيح / Expect true
EXPECT_FALSE(condition)            // توقع خطأ / Expect false
EXPECT_EQ(expected, actual)        // توقع متساوي / Expect equal
EXPECT_NE(not_expected, actual)    // توقع مختلف / Expect not equal
EXPECT_NULL(pointer)               // توقع null / Expect null
EXPECT_NOT_NULL(pointer)           // توقع ليس null / Expect not null
EXPECT_THROW(statement, exception) // توقع استثناء / Expect exception
EXPECT_NO_THROW(statement)         // توقع عدم استثناء / Expect no exception
```

### مثال / Example

```cpp
#include "test_framework.h"

TEST(MyComponent, BasicTest) {
    int result = 1 + 1;
    EXPECT_EQ(2, result);
}

TEST(MyComponent, PointerTest) {
    int* ptr = new int(42);
    EXPECT_NOT_NULL(ptr);
    EXPECT_EQ(42, *ptr);
    delete ptr;
}
```

---

## البناء والتشغيل / Building and Running

### المتطلبات / Requirements

- **CMake** 3.15+
- **LLVM** 14.0+
- **C++17** أو أحدث / or newer
- **Windows** PowerShell أو **Linux/macOS** Bash

### خطوات البناء / Build Steps

#### على Windows:

```powershell
# الانتقال إلى دليل الاختبارات / Navigate to tests directory
cd tests\backends\llvm

# إنشاء دليل البناء / Create build directory
mkdir build
cd build

# تكوين CMake / Configure CMake
cmake ..

# البناء / Build
cmake --build . --config Release

# تشغيل الاختبارات / Run tests
.\Release\llvm_backend_tests.exe
```

#### على Linux/macOS:

```bash
# الانتقال إلى دليل الاختبارات / Navigate to tests directory
cd tests/backends/llvm

# إنشاء دليل البناء / Create build directory
mkdir build
cd build

# تكوين CMake / Configure CMake
cmake ..

# البناء / Build
make -j$(nproc)

# تشغيل الاختبارات / Run tests
./llvm_backend_tests
```

### استخدام CTest

```bash
# تشغيل جميع الاختبارات / Run all tests
ctest --output-on-failure

# تشغيل اختبارات محددة / Run specific tests
ctest -R Linker --output-on-failure

# تشغيل مطوّل / Verbose run
ctest -V
```

---

## الاختبارات المتاحة / Available Tests

### 1. اختبارات الرابط / Linker Tests (40 اختبار / tests) ✅

**الملف / File**: `test_llvm_linker.cpp`

**المكونات المختبرة / Tested Components**:
- ✅ التهيئة الأساسية / Basic initialization
- ✅ تعيين أنواع الربط / Setting linking types
- ✅ تعيين أنماط الربط / Setting linking modes
- ✅ مستويات LTO / LTO levels
- ✅ إدارة الملفات والمكتبات / File and library management
- ✅ الأعلام والخيارات / Flags and options
- ✅ الأعلام والمكتبات الافتراضية / Default flags and libraries
- ✅ دعم متعدد المنصات / Cross-platform support

**الاختبارات / Tests**:
1. BasicInitialization - التهيئة الأساسية
2. InitializationWithTarget - تهيئة مع هدف مخصص
3. SetLinkingTypeExecutable - تعيين نوع تنفيذي
4. SetLinkingTypeSharedLibrary - تعيين نوع مكتبة مشتركة
5. SetLinkingTypeStaticLibrary - تعيين نوع مكتبة ثابتة
6. SetLinkingModeStatic - تعيين نمط ثابت
7. SetLinkingModeDynamic - تعيين نمط ديناميكي
8. SetLinkingModePIE - تعيين نمط PIE
9. SetLTOLevelNone - تعيين LTO بدون
10. SetLTOLevelThin - تعيين LTO Thin
... (30 اختبار إضافي / 30 more tests)

### 2. اختبارات المُحسّن / Optimizer Tests ⏳ (قريباً / Soon)

**الملف / File**: `test_llvm_optimizer.cpp` (قريباً)

**المكونات المختبرة / Tested Components**:
- التهيئة / Initialization
- مستويات التحسين (O0-O3, Os, Oz) / Optimization levels
- 15+ ممرات تحسين / 15+ optimization passes
- الإحصائيات / Statistics
- التحقق من الوحدة / Module verification

### 3. اختبارات الهدف / Target Tests ⏳ (قريباً / Soon)

**الملف / File**: `test_llvm_target.cpp` (قريباً)

**المكونات المختبرة / Tested Components**:
- إدارة الهدف / Target management
- 4 معماريات / 4 architectures
- 14 تكوين منصة / 14 platform configurations
- 5 أنواع ملفات إخراج / 5 output file types
- الترجمة المتقاطعة / Cross-compilation

### 4. اختبارات التنقيح / Debug Tests ⏳ (قريباً / Soon)

**الملف / File**: `test_llvm_debug.cpp` (قريباً)

**المكونات المختبرة / Tested Components**:
- مستويات التنقيح / Debug levels
- معلومات DWARF / DWARF information
- 20 نوع / 20 types
- دعم GDB/LLDB / GDB/LLDB support

### 5. اختبارات وقت التشغيل / Runtime Tests ⏳ (قريباً / Soon)

**الملف / File**: `test_llvm_runtime.cpp` (قريباً)

**المكونات المختبرة / Tested Components**:
- إدارة الذاكرة / Memory management
- جامع النفايات / Garbage collector
- المصفوفات / Arrays
- السلاسل النصية / Strings
- الإدخال/الإخراج / I/O

### 6. اختبارات مولد IR / IR Generator Tests ⏳ (قريباً / Soon)

**الملف / File**: `test_llvm_ir_gen.cpp` (قريباً)

**المكونات المختبرة / Tested Components**:
- توليد IR من SIR / IR generation from SIR
- جميع ميزات اللغة / All language features
- التحقق من الصحة / Correctness verification

### 7. اختبارات التكامل / Integration Tests ⏳ (قريباً / Soon)

**الملف / File**: `test_integration.cpp` (قريباً)

**المكونات المختبرة / Tested Components**:
- خط الأنابيب الكامل / Full pipeline
- من المصدر إلى التنفيذ / Source to executable
- اختبارات end-to-end / End-to-end tests

---

## الخيارات المتقدمة / Advanced Options

### تشغيل مجموعة محددة / Run Specific Suite

```bash
./llvm_backend_tests --suite LLVMLinker
```

### الوضع المطوّل / Verbose Mode

```bash
./llvm_backend_tests --verbose
```

### الوضع الهادئ / Quiet Mode

```bash
./llvm_backend_tests --quiet
```

### عرض المساعدة / Display Help

```bash
./llvm_backend_tests --help
```

---

## المساهمة / Contributing

### إضافة اختبار جديد / Adding a New Test

1. **افتح ملف الاختبار المناسب** / Open appropriate test file
2. **استخدم ماكرو TEST** / Use TEST macro:

```cpp
TEST(ComponentName, TestName) {
    // كود الاختبار / Test code
    EXPECT_TRUE(condition);
}
```

3. **أعد البناء** / Rebuild:

```bash
cd build
cmake --build .
```

4. **شغّل الاختبار** / Run test:

```bash
./llvm_backend_tests
```

### إضافة مجموعة جديدة / Adding a New Suite

1. **أنشئ ملف اختبار جديد** / Create new test file:

```cpp
// test_new_component.cpp
#include "test_framework.h"
#include "test_helpers.h"

TEST(NewComponent, FirstTest) {
    EXPECT_TRUE(true);
}

// المزيد من الاختبارات... / More tests...
```

2. **أضف الملف إلى CMakeLists.txt** / Add file to CMakeLists.txt:

```cmake
set(TEST_SOURCES
    test_runner.cpp
    test_llvm_linker.cpp
    test_new_component.cpp  # جديد / New
)
```

3. **أعد تكوين البناء** / Reconfigure build:

```bash
cd build
cmake ..
cmake --build .
```

---

## الإحصائيات / Statistics

### الحالة الحالية / Current Status

| المكون / Component | الاختبارات / Tests | الحالة / Status |
|-------------------|---------------------|-----------------|
| Linker            | 40                  | ✅ مكتمل / Complete |
| Optimizer         | 0                   | ⏳ قريباً / Soon |
| Target            | 0                   | ⏳ قريباً / Soon |
| Debug             | 0                   | ⏳ قريباً / Soon |
| Runtime           | 0                   | ⏳ قريباً / Soon |
| IR Generator      | 0                   | ⏳ قريباً / Soon |
| Integration       | 0                   | ⏳ قريباً / Soon |
| **المجموع / Total** | **40**          | **20%**         |

### الهدف / Target

- **إجمالي الاختبارات المستهدفة / Target Total Tests**: 200+
- **الاختبارات الحالية / Current Tests**: 40
- **التقدم / Progress**: 20%
- **المتبقي / Remaining**: 160+

---

## استكشاف الأخطاء / Troubleshooting

### خطأ: لم يتم العثور على LLVM / Error: LLVM not found

**الحل / Solution**:
```bash
# تعيين متغير البيئة / Set environment variable
export LLVM_DIR=/path/to/llvm/lib/cmake/llvm

# أو / Or
cmake -DLLVM_DIR=/path/to/llvm/lib/cmake/llvm ..
```

### خطأ: undefined reference to llvm::...

**الحل / Solution**:
تأكد من ربط جميع مكتبات LLVM المطلوبة في CMakeLists.txt

Make sure all required LLVM libraries are linked in CMakeLists.txt

### فشل الاختبار: لم يتم العثور على الملف / Test fails: File not found

**الحل / Solution**:
تأكد من تشغيل الاختبارات من دليل البناء الصحيح

Make sure tests are run from the correct build directory

---

## الموارد / Resources

### الوثائق / Documentation

- [دليل LLVM](https://llvm.org/docs/)
- [دليل مستخدم SadLang](../../docs/USER_GUIDE.md)
- [دليل المعمارية](../../docs/architecture/)

### الدعم / Support

- **البريد الإلكتروني / Email**: support@sadlang.org
- **GitHub Issues**: https://github.com/sadlang/sadlang/issues

---

## الترخيص / License

هذا المشروع مرخص تحت رخصة MIT - انظر ملف [LICENSE](../../../LICENSE) للتفاصيل.

This project is licensed under the MIT License - see the [LICENSE](../../../LICENSE) file for details.

---

**الحمد لله رب العالمين**

**Praise be to Allah, Lord of the Worlds**

---

_آخر تحديث / Last Updated: ديسمبر 2025 / December 2025_
