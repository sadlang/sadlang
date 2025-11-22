# 🧪 نظام اختبارات لغة ص / S Language Test System

هذا المجلد يحتوي على جميع اختبارات لغة ص الشاملة.

---

## 📁 هيكل المجلد / Directory Structure

```
tests/
├── stdlib_tests/                    # اختبارات المكتبة القياسية
│   ├── string_functions_tests.cpp   # اختبارات دوال النصوص (44)
│   ├── array_functions_tests.cpp    # اختبارات دوال المصفوفات (45)
│   ├── math_functions_tests.cpp     # اختبارات الدوال الرياضية (62)
│   ├── CMakeLists.txt               # تكوين البناء
│   ├── README.md                    # توثيق الاختبارات
│   └── build/                       # ملفات البناء (يتم إنشاؤها)
│
├── parser_tests/                    # اختبارات المحلل النحوي (قادم)
├── lexer_tests/                     # اختبارات المحلل المعجمي (قادم)
├── interpreter_tests/               # اختبارات المفسر (قادم)
└── integration_tests/               # اختبارات التكامل (قادم)
```

---

## ✅ الاختبارات المكتملة / Completed Tests

### اختبارات المكتبة القياسية / Standard Library Tests

**الحالة**: ✅ **151/151 نجحت (100%)**

| المجموعة | الاختبارات | الحالة |
|----------|------------|---------|
| دوال النصوص | 44 | ✅ 100% |
| دوال المصفوفات | 45 | ✅ 100% |
| الدوال الرياضية | 62 | ✅ 100% |

**التقرير الكامل**: [STDLIB_TESTS_COMPREHENSIVE_REPORT.md](../docs/stlib_plan/STDLIB_TESTS_COMPREHENSIVE_REPORT.md)

---

## 🚀 تشغيل الاختبارات / Running Tests

### اختبارات المكتبة القياسية

#### Windows (PowerShell/CMD)

```powershell
# الانتقال للمجلد
cd C:\s\s_language\tests\stdlib_tests

# البناء
mkdir build
cd build
cmake ..
cmake --build . --config Release

# تشغيل الاختبارات
.\Release\string_functions_tests.exe
.\Release\array_functions_tests.exe
.\Release\math_functions_tests.exe

# أو تشغيل جميع الاختبارات معاً
ctest -C Release --verbose
```

#### Linux/macOS

```bash
# الانتقال للمجلد
cd /path/to/s_language/tests/stdlib_tests

# البناء
mkdir build && cd build
cmake ..
cmake --build .

# تشغيل الاختبارات
./string_functions_tests
./array_functions_tests
./math_functions_tests

# أو تشغيل جميع الاختبارات معاً
ctest --verbose
```

---

## 📊 نتائج الاختبارات الحالية / Current Test Results

### آخر تشغيل (22 نوفمبر 2025)

```
╔═══════════════════════════════════════════════════════════════╗
║           🎉 SUCCESS - ALL TESTS PASSED 100%                 ║
║           🎉 نجاح - جميع الاختبارات نجحت 100%               ║
╚═══════════════════════════════════════════════════════════════╝

Total Tests:   151
✅ Passed:     151 (100%)
❌ Failed:     0 (0%)
Duration:      ~3 seconds
```

---

## 🧪 أنواع الاختبارات / Test Types

### 1. اختبارات الوحدة / Unit Tests
- اختبار كل دالة على حدة
- تغطية الحالات الأساسية
- **الحالة**: ✅ مكتمل للمكتبة القياسية

### 2. اختبارات الحالات الحدية / Edge Case Tests
- قيم فارغة، null، سالبة
- أعداد كبيرة جداً
- نصوص خاصة (UTF-8، عربي)
- **الحالة**: ✅ مكتمل للمكتبة القياسية

### 3. اختبارات التكامل / Integration Tests
- استخدام عدة دوال معاً
- سيناريوهات واقعية
- **الحالة**: ✅ مكتمل للمكتبة القياسية

### 4. اختبارات الأداء / Performance Tests
- مصفوفات كبيرة (1000+ عنصر)
- عمليات متكررة
- **الحالة**: ✅ مكتمل للمكتبة القياسية

---

## 📝 كتابة اختبارات جديدة / Writing New Tests

### هيكل الاختبار الأساسي

```cpp
#include <iostream>
#include <vector>
#include "data/types/value.h"
#include "stdlib/[category]/[module].h"

using namespace Sad;
using namespace std;

// دالة مساعدة للتحقق
void assertTest(bool condition, const string& testName) {
    if (condition) {
        cout << "✅ PASS: " << testName << endl;
    } else {
        cout << "❌ FAIL: " << testName << endl;
    }
}

// دوال مساعدة لإنشاء القيم
Data::Value makeInt(int val) { return Data::Value(val); }
Data::Value makeString(const string& val) { return Data::Value(val); }

// دالة الاختبار
void testFunctionName() {
    cout << "\n══════════════════════════════════════" << endl;
    cout << "  Testing functionName()" << endl;
    cout << "══════════════════════════════════════" << endl;
    
    // الاختبار 1
    vector<Data::Value> args1 = { makeInt(5) };
    Data::Value result1 = Module::functionName(args1);
    assertTest(result1.toInt() == 25, "functionName: 5^2 = 25");
    
    // الاختبار 2 (حالة حدية)
    vector<Data::Value> args2 = { makeInt(0) };
    Data::Value result2 = Module::functionName(args2);
    assertTest(result2.toInt() == 0, "functionName: 0^2 = 0");
}

int main() {
    cout << "╔══════════════════════════════════════╗" << endl;
    cout << "║    Module Test Suite                 ║" << endl;
    cout << "╚══════════════════════════════════════╝" << endl;
    
    testFunctionName();
    
    return 0;
}
```

### إضافة الاختبار إلى CMake

```cmake
# في CMakeLists.txt
add_executable(new_tests
    new_tests.cpp
    ${CMAKE_SOURCE_DIR}/../../src/module/source.cpp
    ${CMAKE_SOURCE_DIR}/../../src/data/types/value.cpp
)

target_include_directories(new_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/../../include
)

add_test(NAME NewTests COMMAND new_tests)
```

---

## 📈 خطة الاختبارات المستقبلية / Future Testing Plan

### المرحلة القادمة (Q1 2026)

- [ ] اختبارات دوال الأنواع (Phase 5: Type Functions)
- [ ] اختبارات المحلل النحوي (Parser Tests)
- [ ] اختبارات المحلل المعجمي (Lexer Tests)
- [ ] اختبارات المفسر (Interpreter Tests)

### الأهداف طويلة المدى

- [ ] اختبارات البرمجة الكائنية (OOP Tests)
- [ ] اختبارات نظام الملفات (File I/O Tests)
- [ ] اختبارات نظام الأخطاء (Error Handling Tests)
- [ ] اختبارات التزامن (Concurrency Tests)
- [ ] اختبارات الأداء المتقدمة (Advanced Performance Tests)

---

## 🐛 الإبلاغ عن الأخطاء / Bug Reporting

إذا وجدت خطأ في الاختبارات:

1. تحقق من آخر إصدار
2. شغّل الاختبار مرة أخرى
3. احفظ نتيجة الخطأ
4. افتح issue على GitHub مع:
   - وصف المشكلة
   - نتيجة الاختبار الفاشل
   - البيئة (نظام التشغيل، المترجم، إلخ)

---

## 📚 موارد إضافية / Additional Resources

- [دليل المستخدم](../docs/USER_GUIDE.md)
- [التقرير الشامل للاختبارات](../docs/stlib_plan/STDLIB_TESTS_COMPREHENSIVE_REPORT.md)
- [خارطة الطريق](../ROADMAP.md)
- [توثيق المكتبة القياسية](../docs/stlib_plan/)

---

## 🤝 المساهمة / Contributing

نرحب بمساهماتكم في كتابة اختبارات جديدة!

**كيف تساهم:**
1. اختر مكوّن يحتاج اختبارات
2. اكتب الاختبارات وفق النمط الموحد
3. تأكد من نجاح جميع الاختبارات
4. أرسل Pull Request

---

**آخر تحديث**: 22 نوفمبر 2025 - 17:45  
**الحالة الحالية**: ✅ 151 اختبار نجحت (100%)
