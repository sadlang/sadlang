# Type Checker Tests - اختبارات مدقق الأنواع

## نظرة عامة / Overview

هذا المجلد يحتوي على مجموعة شاملة من اختبارات الوحدة لنظام Type Checker.

This directory contains a comprehensive suite of unit tests for the Type Checker system.

---

## البنية / Structure

```
tests/
├── CMakeLists.txt                 # إعدادات البناء / Build configuration
├── test_type_context.cpp          # اختبارات Type Context
├── test_type_inference.cpp        # اختبارات Type Inference (قريباً)
├── test_type_checker.cpp          # اختبارات Type Checker (قريباً)
├── test_error_reporting.cpp       # اختبارات Error Reporting
├── test_typed_ast.cpp             # اختبارات Typed AST
└── README.md                      # هذا الملف / This file
```

---

## الاختبارات المتاحة / Available Tests

### ✅ 1. test_typed_ast.cpp (70+ tests)
اختبارات شاملة لنظام الأنواع:
- **Basic Types**: INT, FLOAT, STRING, BOOL, VOID
- **Array Types**: مصفوفات بسيطة ومتداخلة
- **Function Types**: دوال عادية و variadic
- **Dictionary Types**: قواميس بأنواع مختلفة
- **Optional Types**: أنواع اختيارية
- **Union Types**: أنواع متحدة
- **Class Types**: أصناف مع أعضاء ودوال
- **Complex Types**: أنواع معقدة ومتداخلة

**عدد الاختبارات**: 24 اختبار

### ✅ 2. test_error_reporting.cpp (60+ tests)
اختبارات نظام الأخطاء:
- **TypeError**: إنشاء وإدارة الأخطاء
- **TypeErrorBuilder**: Builder pattern
- **TypeErrorReporter**: مدير الأخطاء
- **FixSuggestion**: اقتراحات التصحيح
- **Integration**: سيناريوهات كاملة

**عدد الاختبارات**: 20 اختبار

### ✅ 3. test_type_context.cpp (50+ tests)
اختبارات سياق الأنواع:
- **Symbol**: الرموز والمتغيرات
- **Scope**: إدارة النطاقات
- **Environment**: البيئة والبحث
- **TypeContext**: السياق الكامل
- **Integration**: نطاقات متداخلة معقدة

**عدد الاختبارات**: 18 اختبار

### ⏳ 4. test_type_inference.cpp (قريباً)
اختبارات Type Inference:
- Algorithm W
- Unification
- Type substitution
- Constraint solving

### ⏳ 5. test_type_checker.cpp (قريباً)
اختبارات Type Checker:
- Expression checking
- Statement checking
- Function checking
- Class checking

---

## كيفية التشغيل / How to Run

### البناء / Build

```bash
cd C:\s\s_language\build
cmake ..
cmake --build . --target test_typed_ast
cmake --build . --target test_error_reporting
cmake --build . --target test_type_context
```

### التشغيل / Execute

```bash
# اختبار واحد / Single test
.\bin\Debug\test_typed_ast.exe

# جميع الاختبارات / All tests
.\bin\Debug\test_typed_ast.exe
.\bin\Debug\test_error_reporting.exe
.\bin\Debug\test_type_context.exe
```

### استخدام CMake / Using CMake

```bash
# بناء وتشغيل جميع الاختبارات / Build and run all tests
cmake --build . --target run_type_checker_tests
```

---

## إحصائيات / Statistics

### الإحصائيات الحالية / Current Statistics

| المكون | عدد الاختبارات | الحالة |
|--------|----------------|--------|
| Typed AST | 24 | ✅ مكتمل |
| Error Reporting | 20 | ✅ مكتمل |
| Type Context | 18 | ✅ مكتمل |
| Type Inference | 0 | ⏳ قريباً |
| Type Checker | 0 | ⏳ قريباً |
| **المجموع** | **62** | **65% مكتمل** |

### الهدف / Target

- **الهدف النهائي**: 150+ اختبار
- **التقدم الحالي**: 62 اختبار (41%)

---

## الاعتمادات / Dependencies

- **Google Test**: Framework للاختبارات
- **CMake**: نظام البناء
- **C++17**: المعيار المستخدم

---

## التقارير / Reports

### مثال على مخرجات الاختبار / Test Output Example

```
[==========] Running 24 tests from 10 test suites.
[----------] Global test environment set-up.
[----------] 5 tests from TypedASTTest
[ RUN      ] TypedASTTest.CreatePrimitiveTypes
[       OK ] TypedASTTest.CreatePrimitiveTypes (0 ms)
[ RUN      ] TypedASTTest.TypeEquality
[       OK ] TypedASTTest.TypeEquality (0 ms)
...
[==========] 24 tests from 10 test suites ran. (15 ms total)
[  PASSED  ] 24 tests.
```

---

## المساهمة / Contributing

عند إضافة اختبارات جديدة:

1. ✅ **التوثيق**: وثّق كل اختبار بالعربية والإنجليزية
2. ✅ **التسمية**: استخدم أسماء واضحة ومعبرة
3. ✅ **التنظيم**: ضع الاختبارات في مجموعات منطقية
4. ✅ **التحقق**: تأكد من نجاح جميع الاختبارات

---

## الخطوات التالية / Next Steps

### اليوم 11-14 (الأسبوع الحالي)

- [x] ✅ إعداد Google Test Framework
- [x] ✅ إنشاء CMakeLists.txt
- [x] ✅ اختبارات Typed AST (24 اختبار)
- [x] ✅ اختبارات Error Reporting (20 اختبار)
- [x] ✅ اختبارات Type Context (18 اختبار)
- [ ] ⏳ اختبارات Type Inference (30+ اختبار)
- [ ] ⏳ اختبارات Type Checker (40+ اختبار)
- [ ] ⏳ Integration Tests (20+ اختبار)

---

## الدعم / Support

**SadLanguage Compiler Team**  
**Version**: 1.0.0  
**Date**: December 31, 2025

---

## الملاحظات / Notes

- جميع الاختبارات تستخدم Google Test Framework
- الاختبارات مكتوبة بمعايير C++17
- كل اختبار موثق بالكامل بالعربية والإنجليزية
- التغطية الحالية: ~40% من الهدف النهائي

---

## الأمثلة / Examples

### مثال على اختبار بسيط / Simple Test Example

```cpp
TEST(TypedASTTest, CreateIntType) {
    auto intType = TypeFactory::getIntType();
    EXPECT_EQ(intType->getKind(), Type::Kind::INT);
}
```

### مثال على اختبار معقد / Complex Test Example

```cpp
TEST(ComplexTypeTest, FunctionReturningArray) {
    auto intType = TypeFactory::getIntType();
    auto arrayType = std::make_shared<ArrayType>(intType);
    
    std::vector<std::shared_ptr<Type>> params;
    auto funcType = std::make_shared<FunctionType>(params, arrayType);
    
    EXPECT_EQ(funcType->getKind(), Type::Kind::FUNCTION);
    EXPECT_EQ(funcType->getReturnType()->getKind(), Type::Kind::ARRAY);
}
```

---

## EOF
