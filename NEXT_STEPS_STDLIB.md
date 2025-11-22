# 🚀 الخطوات المتبقية لإكمال المكتبة القياسية
# Remaining Steps to Complete Standard Library

**الحالة الحالية / Current Status**: 39/49 دالة (79.6%)  
**آخر تحديث / Last Updated**: نوفمبر 22، 2025

---

## ✅ تم إكماله / Completed

### ✔️ المرحلة 1: دوال الإدخال والإخراج (5 دوال)
- `اطبع/print`
- `اطبع_سطر/println`
- `اقرأ/input`
- `اقرأ_سطر/readLine`
- `امسح/clear`

### ✔️ المرحلة 2: دوال النصوص (12 دالة)
- `طول/length`, `ابحث/find`, `استبدل/replace`
- `قطعة/substring`, `لأصغر/toLower`, `لأكبر/toUpper`
- `قلّم/trim`, `قسّم/split`, `اربط/join`
- `يبدأ_بـ/startsWith`, `ينتهي_بـ/endsWith`, `يحتوي/contains`

### ✔️ المرحلة 3: دوال المصفوفات (10 دوال) - **جديد!**
- `إضافة/append`, `إزالة/pop`, `حجم/size`
- `فهرس/indexOf`, `يحتوي/contains`, `قلب/reverse`
- `فرز/sort`, `أول/first`, `آخر/last`, `شريحة/slice`

### ✔️ المرحلة 4: الدوال الرياضية (12 دالة) - **جديد!**
- **أساسية**: `جذر/sqrt`, `أس/power`, `مطلق/abs`, `مربع/square`
- **مقارنة**: `أكبر/max`, `أصغر/min`
- **تقريب**: `تقريب/round`, `أرضية/floor`, `سقف/ceil`
- **مثلثية**: `جيب/sin`, `جيب_تمام/cos`, `ظل/tan`

---

## ⏳ المتبقي / Remaining

### 🔲 المرحلة 5: دوال الأنواع (10 دوال)

#### دوال التحويل / Conversion Functions (6)
```cpp
// 1. لنص / toString
نص toString(أي قيمة)
// مثال: لنص(42) → "42"

// 2. لرقم / toInt
رقم toInt(نص أو رقم_حقيقي)
// مثال: لرقم("123") → 123

// 3. لرقم_حقيقي / toFloat
رقم_حقيقي toFloat(نص أو رقم)
// مثال: لرقم_حقيقي("3.14") → 3.14

// 4. لمصفوفة / toArray
مصفوفة toArray(نص أو أي قيمة)
// مثال: لمصفوفة("مرحبا") → ['م', 'ر', 'ح', 'ب', 'ا']

// 5. لمنطقي / toBool
منطقي toBool(أي قيمة)
// مثال: لمنطقي(0) → خطأ, لمنطقي(1) → صحيح

// 6. لقاموس / toDict (اختياري)
قاموس toDict(مصفوفة من الأزواج)
```

#### دوال التحقق / Type Checking Functions (4)
```cpp
// 1. هو_رقم / isInt
منطقي isInt(أي قيمة)
// مثال: هو_رقم(42) → صحيح

// 2. هو_رقم_حقيقي / isFloat
منطقي isFloat(أي قيمة)
// مثال: هو_رقم_حقيقي(3.14) → صحيح

// 3. هو_نص / isString
منطقي isString(أي قيمة)
// مثال: هو_نص("مرحبا") → صحيح

// 4. هو_مصفوفة / isArray
منطقي isArray(أي قيمة)
// مثال: هو_مصفوفة([1, 2, 3]) → صحيح
```

---

## 📝 مهام التكامل الفورية / Immediate Integration Tasks

### 1. تحديث builtin_registry.cpp (عالي الأولوية)

يجب إضافة تسجيل لـ **22 دالة جديدة**:

```cpp
// في ملف: src/interpreter/core/builtin_registry.cpp

#include "stdlib/core/array_functions.h"
#include "stdlib/math/math_functions.h"

void registerBuiltinFunctions(Interpreter& interpreter) {
    // ... الدوال الموجودة
    
    // ===================================================================
    // دوال المصفوفات الجديدة / New Array Functions
    // ===================================================================
    
    // append / إضافة
    auto array_append_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(Core::ArrayFunctions::append(plainArgs));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("إضافة", array_append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("append", array_append_func);
    
    // pop / إزالة
    auto array_pop_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(Core::ArrayFunctions::pop(plainArgs));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("إزالة", array_pop_func);
    interpreter.getFunctionManager().registerBuiltinFunction("pop", array_pop_func);
    
    // size / حجم
    auto array_size_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(Core::ArrayFunctions::size(plainArgs));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("حجم", array_size_func);
    interpreter.getFunctionManager().registerBuiltinFunction("size", array_size_func);
    
    // ... كرر لباقي دوال المصفوفات (indexOf, contains, reverse, sort, first, last, slice)
    
    // ===================================================================
    // الدوال الرياضية الجديدة / New Math Functions
    // ===================================================================
    
    // sqrt / جذر
    auto math_sqrt_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(Math::MathFunctions::sqrt(plainArgs));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("جذر", math_sqrt_func);
    interpreter.getFunctionManager().registerBuiltinFunction("sqrt", math_sqrt_func);
    
    // power / أس
    auto math_power_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        std::vector<Data::Value> plainArgs;
        for (const auto& arg : args) plainArgs.push_back(*arg);
        return std::make_shared<Data::Value>(Math::MathFunctions::power(plainArgs));
    };
    interpreter.getFunctionManager().registerBuiltinFunction("أس", math_power_func);
    interpreter.getFunctionManager().registerBuiltinFunction("power", math_power_func);
    
    // ... كرر لباقي الدوال الرياضية (abs, max, min, round, floor, ceil, square, sin, cos, tan)
}
```

### 2. تحديث CMakeLists.txt

```cmake
# في ملف: CMakeLists.txt

# إضافة ملفات header
set(STDLIB_HEADERS
    include/stdlib/core/array_functions.h
    include/stdlib/math/math_functions.h
    # ... الملفات الموجودة
)

# إضافة ملفات source
set(STDLIB_SOURCES
    src/stdlib/core/array_functions.cpp
    src/stdlib/math/math_functions.cpp
    # ... الملفات الموجودة
)

# إضافة للمكتبة
add_library(sad_core STATIC
    ${STDLIB_SOURCES}
    ${STDLIB_HEADERS}
)

# تحديد مجلدات include
target_include_directories(sad_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/stdlib
        ${CMAKE_CURRENT_SOURCE_DIR}/include/stdlib/core
        ${CMAKE_CURRENT_SOURCE_DIR}/include/stdlib/math  # جديد
)
```

### 3. البناء والاختبار

```bash
# الانتقال لمجلد البناء
cd C:\s\s_language\build

# إعادة تكوين CMake
cmake ..

# البناء
cmake --build . --config Release

# الاختبار
.\bin\Release\sad.exe ..\examples\test_array.s
.\bin\Release\sad.exe ..\examples\test_math.s
```

---

## 📂 البنية النهائية للملفات / Final File Structure

```
s_language/
├── include/
│   └── stdlib/
│       ├── core/
│       │   ├── array_functions.h      ✅ جديد
│       │   └── builtins.h
│       ├── io/
│       │   └── io_functions.h
│       ├── math/
│       │   └── math_functions.h       ✅ جديد
│       └── string/
│           └── string_functions.h
├── src/
│   └── stdlib/
│       ├── core/
│       │   ├── array_functions.cpp    ✅ جديد
│       │   └── builtins.cpp
│       ├── io/
│       │   └── io_functions.cpp
│       ├── math/
│       │   └── math_functions.cpp     ✅ جديد
│       └── string/
│           └── string_functions.cpp
└── tests/
    ├── array_functions_tests.cpp      ⏳ مطلوب
    └── math_functions_tests.cpp       ⏳ مطلوب
```

---

## 🧪 ملفات اختبار مقترحة / Suggested Test Files

### test_array.s
```s
// اختبار دوال المصفوفات
اطبع_سطر("=== اختبار دوال المصفوفات ===")

مصفوفة قائمة = [1, 2, 3, 4, 5]
اطبع_سطر("القائمة الأصلية:", قائمة)

// إضافة وإزالة
قائمة = إضافة(قائمة, 6)
اطبع_سطر("بعد الإضافة:", قائمة)

رقم آخر_عنصر = إزالة(قائمة)
اطبع_سطر("العنصر المزال:", آخر_عنصر)
اطبع_سطر("بعد الإزالة:", قائمة)

// الحجم والبحث
اطبع_سطر("الحجم:", حجم(قائمة))
اطبع_سطر("موقع الرقم 3:", فهرس(قائمة, 3))
اطبع_سطر("يحتوي 4؟", يحتوي(قائمة, 4))

// التحويل
قائمة = قلب(قائمة)
اطبع_سطر("بعد القلب:", قائمة)

قائمة = فرز(قائمة, صحيح)
اطبع_سطر("بعد الفرز:", قائمة)

// الوصول
اطبع_سطر("أول عنصر:", أول(قائمة))
اطبع_سطر("آخر عنصر:", آخر(قائمة))

مصفوفة جزء = شريحة(قائمة, 1, 4)
اطبع_سطر("شريحة [1:4]:", جزء)
```

### test_math.s
```s
// اختبار الدوال الرياضية
اطبع_سطر("=== اختبار الدوال الرياضية ===")

// دوال أساسية
اطبع_سطر("جذر 16:", جذر(16))
اطبع_سطر("2 أس 3:", أس(2, 3))
اطبع_سطر("مطلق -5:", مطلق(-5))
اطبع_سطر("مربع 7:", مربع(7))

// مقارنة
اطبع_سطر("أكبر(5, 10, 3):", أكبر(5, 10, 3))
اطبع_سطر("أصغر(5, 10, 3):", أصغر(5, 10, 3))

// تقريب
اطبع_سطر("تقريب 3.7:", تقريب(3.7))
اطبع_سطر("أرضية 3.7:", أرضية(3.7))
اطبع_سطر("سقف 3.2:", سقف(3.2))

// مثلثية
رقم_حقيقي باي = 3.14159
اطبع_سطر("جيب 0:", جيب(0))
اطبع_سطر("جيب_تمام 0:", جيب_تمام(0))
اطبع_سطر("ظل π/4:", ظل(باي/4))
```

---

## 📊 خارطة الطريق / Roadmap

```
الأسبوع الحالي:
✅ المرحلة 3: دوال المصفوفات (مكتملة)
✅ المرحلة 4: الدوال الرياضية (مكتملة)
⏳ التكامل والتسجيل (قيد التنفيذ)

الأسبوع القادم:
⏳ المرحلة 5: دوال الأنواع (10 دوال)
⏳ اختبارات شاملة
⏳ تحسين الأداء
⏳ توثيق المستخدم النهائي

النتيجة النهائية:
🎯 49 دالة قياسية
🎯 دعم كامل للعربية والإنجليزية
🎯 توثيق شامل
🎯 اختبارات كاملة
```

---

## 🔗 روابط مفيدة / Useful Links

- **تقرير المرحلة 3 و 4**: `docs/stlib_plan/PHASE3_4_COMPLETION_REPORT.md`
- **تقرير المرحلة 2**: `docs/stlib_plan/PHASE2_STRING_COMPLETION_REPORT.md`
- **دليل المطور**: `docs/stlib_plan/DEVELOPER_QUICK_GUIDE.md`
- **خطة المكتبة القياسية**: `docs/stlib_plan/STDLIB_IMPLEMENTATION_PLAN.md`

---

## 💡 نصيحة سريعة / Quick Tip

عند إضافة دالة جديدة، اتبع هذا النمط:

1. ✅ أنشئ الـ header مع توثيق ثنائي اللغة
2. ✅ نفّذ الدالة مع معالجة الأخطاء
3. ⏳ سجّل في `builtin_registry.cpp` (عربي + إنجليزي)
4. ⏳ أضف للـ `CMakeLists.txt`
5. ⏳ اختبر الدالة
6. ⏳ وثّق مع أمثلة

---

**آخر تحديث**: نوفمبر 22، 2025  
**الحالة**: 79.6% مكتمل - المرحلة 5 متبقية!
