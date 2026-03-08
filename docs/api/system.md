# وثائق API — وحدة system

> المسار: `stdlib/system/`
> مساحة الأسماء: `Sad::StdLib::System`

## نظرة عامة
توفر الوحدة دوال النظام الأساسية: معلومات المنصة، الوقت، متغيرات البيئة، وإدارة المجلد الحالي.

## الملفات الأساسية
- `system/system_functions.h`
- `system/system_functions.cpp`

## API رئيسية
- `SystemFunctions::platform()` -> `Value`
- `SystemFunctions::osName()` -> `Value`
- `SystemFunctions::timestamp()` -> `Value`
- `SystemFunctions::clock_time()` -> `Value`
- `SystemFunctions::getEnv(args)`
- `SystemFunctions::setEnv(args)`
- `SystemFunctions::currentDir(args)`
- `SystemFunctions::changeDir(args)`

## ملاحظات التوقيع
- أغلب الدوال المعتمدة على معاملات تستقبل:
  - `const std::vector<Sad::Data::Value>& args`

## مثال سريع
```cpp
using namespace Sad::StdLib::System;
using Sad::Data::Value;

std::vector<Value> args = {Value(std::string("PATH"))};
Value p = SystemFunctions::getEnv(args);
Value t = SystemFunctions::timestamp();
```

## ملاحظات
- `clock_time()` ترجع قيمة رقمية (DOUBLE) لا نص.
- دوال البيئة قد ترمي `std::invalid_argument` عند معاملات ناقصة.
