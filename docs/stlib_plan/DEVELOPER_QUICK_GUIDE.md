# 🚀 دليل المطور السريع - Quick Developer Guide
## إكمال المكتبات القياسية / Completing Standard Library

---

## 📋 قائمة المراجعة السريعة / Quick Checklist

### ✅ Phase 1: I/O Functions
- [x] ملفات الترويسة والتنفيذ
- [x] 5 دوال مكتملة
- [x] مسجلة في builtin_registry.cpp
- [x] تم الاختبار

### ✅ Phase 2: String Functions  
- [x] ملفات الترويسة والتنفيذ
- [x] 12 دالة مكتملة
- [ ] **التالي: التسجيل في builtin_registry.cpp**
- [ ] **التالي: الاختبار**

### ⏳ Phase 3: Array Functions
- [ ] إنشاء array_functions.h
- [ ] إنشاء array_functions.cpp
- [ ] تسجيل 10 دوال
- [ ] الاختبار

### ⏳ Phase 4: Math Functions
- [ ] إنشاء math_functions.h
- [ ] إنشاء math_functions.cpp
- [ ] تسجيل 12 دالة
- [ ] الاختبار

---

## 🎯 الخطوات العملية الآن / Practical Steps Now

### الخطوة 1: تحديث CMakeLists.txt

```cmake
# إضافة في: CMakeLists.txt
# أضف هذا السطر في قسم STDLIB_SOURCES:

set(STDLIB_SOURCES
    src/stdlib/io/io_functions.cpp
    src/stdlib/string/string_functions.cpp  # <-- أضف هذا السطر
    src/stdlib/core/builtins.cpp
    # ... الملفات الأخرى
)
```

### الخطوة 2: تسجيل دوال String

أضف في `src/interpreter/core/builtin_registry.cpp`:

```cpp
// في بداية الملف:
#include "stdlib/string/string_functions.h"

// في دالة registerBuiltinFunctions():
void registerBuiltinFunctions(/* ... */) {
    // ... الدوال الموجودة ...
    
    // ============================================================
    // (AR) دوال النصوص / (EN) String Functions - Phase 2
    // ============================================================
    
    using namespace Sad::StdLib::String;
    
    // length / طول
    registerFunction("length", 
        [](const std::vector<std::shared_ptr<Data::Value>>& args) {
            std::vector<Data::Value> vals;
            for (const auto& arg : args) vals.push_back(*arg);
            auto result = StringFunctions::length(vals);
            return std::make_shared<Data::Value>(result);
        });
    registerFunction("طول", /* نفس الكود */);
    
    // find / بحث
    registerFunction("find", 
        [](const std::vector<std::shared_ptr<Data::Value>>& args) {
            std::vector<Data::Value> vals;
            for (const auto& arg : args) vals.push_back(*arg);
            auto result = StringFunctions::find(vals);
            return std::make_shared<Data::Value>(result);
        });
    registerFunction("بحث", /* نفس الكود */);
    
    // replace / استبدل
    registerFunction("replace", /* ... */);
    registerFunction("استبدل", /* ... */);
    
    // substring / استخراج
    registerFunction("substring", /* ... */);
    registerFunction("استخراج", /* ... */);
    
    // toLower / تحويل_صغير
    registerFunction("toLower", /* ... */);
    registerFunction("تحويل_صغير", /* ... */);
    
    // toUpper / تحويل_كبير
    registerFunction("toUpper", /* ... */);
    registerFunction("تحويل_كبير", /* ... */);
    
    // trim / قص_أطراف
    registerFunction("trim", /* ... */);
    registerFunction("قص_أطراف", /* ... */);
    
    // split / تقسيم
    registerFunction("split", /* ... */);
    registerFunction("تقسيم", /* ... */);
    
    // join / دمج
    registerFunction("join", /* ... */);
    registerFunction("دمج", /* ... */);
    
    // startsWith / يبدأ_ب
    registerFunction("startsWith", /* ... */);
    registerFunction("يبدأ_ب", /* ... */);
    
    // endsWith / ينتهي_ب
    registerFunction("endsWith", /* ... */);
    registerFunction("ينتهي_ب", /* ... */);
    
    // contains / يحتوي_على
    registerFunction("contains", /* ... */);
    registerFunction("يحتوي_على", /* ... */);
}
```

### الخطوة 3: بناء المشروع

```powershell
cd C:\s\s_language
cmake --build build --config Debug
```

### الخطوة 4: اختبار دوال String

أنشئ ملف `test_string.s`:

```s
# اختبار دوال النصوص
# Test string functions

# 1. length / طول
النص1 = "مرحبا بالعالم"
طبع_سطر("الطول: " + طول(النص1))  # يجب أن يطبع: 13

# 2. find / بحث
الموقع = بحث(النص1, "العالم")
طبع_سطر("موقع 'العالم': " + الموقع)  # يجب أن يطبع: 7

# 3. replace / استبدل
النص2 = استبدل(النص1, "العالم", "الأصدقاء")
طبع_سطر(النص2)  # يجب أن يطبع: مرحبا بالأصدقاء

# 4. substring / استخراج
الجزء = استخراج(النص1, 0, 5)
طبع_سطر("الجزء: " + الجزء)  # يجب أن يطبع: مرحبا

# 5. startsWith / يبدأ_ب
إذا يبدأ_ب(النص1, "مرحبا") {
    طبع_سطر("النص يبدأ بـ 'مرحبا'")
}

# 6. split / تقسيم
الكلمات = تقسيم("واحد,اثنان,ثلاثة", ",")
طبع_سطر("عدد الكلمات: " + حجم(الكلمات))
```

---

## 📝 قالب إنشاء Phase جديدة / Template for New Phase

### مثال: Array Functions (Phase 3)

#### 1. إنشاء array_functions.h

```cpp
#pragma once
#include <string>
#include <memory>
#include <vector>
#include "data/types/value.h"

namespace Sad {
namespace StdLib {
namespace Core {

class ArrayFunctions {
public:
    ArrayFunctions() = delete;
    
    // الدوال / Functions
    static Data::Value append(const std::vector<Data::Value>& args);
    static Data::Value pop(const std::vector<Data::Value>& args);
    static Data::Value size(const std::vector<Data::Value>& args);
    // ... الدوال الأخرى
    
private:
    static bool validateArguments(const std::vector<Data::Value>& args,
                                  size_t minArgs, int maxArgs = -1);
};

}}}
```

#### 2. إنشاء array_functions.cpp

```cpp
#include "stdlib/core/array_functions.h"
#include <algorithm>
#include <stdexcept>

namespace Sad {
namespace StdLib {
namespace Core {

Data::Value ArrayFunctions::append(const std::vector<Data::Value>& args) {
    validateArguments(args, 2, 2);
    
    if (args[0].getType() != Data::DataType::ARRAY) {
        throw std::invalid_argument("First argument must be an array");
    }
    
    std::vector<Data::Value> arr = args[0].toArray();
    arr.push_back(args[1]);
    
    return Data::Value(arr);
}

// ... تنفيذ باقي الدوال

}}}
```

#### 3. التسجيل في builtin_registry.cpp

```cpp
#include "stdlib/core/array_functions.h"

// في registerBuiltinFunctions():
using namespace Sad::StdLib::Core;

registerFunction("append", [](auto& args) {
    std::vector<Data::Value> vals;
    for (const auto& arg : args) vals.push_back(*arg);
    return std::make_shared<Data::Value>(
        ArrayFunctions::append(vals));
});
registerFunction("إضافة", /* نفس الكود */);
```

---

## 🎓 نصائح للمطورين / Developer Tips

### ✅ أفضل الممارسات / Best Practices

1. **التعليقات ثنائية اللغة:**
```cpp
// (AR) تحويل النص إلى أحرف صغيرة
// (EN) Convert string to lowercase
```

2. **معالجة الأخطاء:**
```cpp
if (args.empty()) {
    throw std::invalid_argument(
        "(AR) يجب توفير معامل واحد على الأقل / "
        "(EN) At least one argument required");
}
```

3. **دعم UTF-8:**
```cpp
// استخدم utf8Length بدلاً من str.length()
size_t len = utf8Length(str);
```

4. **الاختبار:**
```cpp
// اختبر الحالات الحدودية
TEST(StringFunctions, EmptyString) {
    auto result = StringFunctions::length({Data::Value("")});
    EXPECT_EQ(result.toInt(), 0);
}
```

### ⚠️ الأخطاء الشائعة / Common Mistakes

1. ❌ **نسيان التسجيل:**
   - لا تنس تسجيل الدالة في builtin_registry.cpp

2. ❌ **عدم معالجة UTF-8:**
   - استخدم utf8Length، ليس str.length()

3. ❌ **عدم فحص المعاملات:**
   - دائماً استخدم validateArguments()

4. ❌ **نسيان الأسماء العربية:**
   - سجل كل دالة مرتين: بالعربي والإنجليزي

---

## 📊 جدول الأولويات / Priority Table

| المرحلة | الأولوية | الوقت | الحالة |
|---------|---------|------|--------|
| Phase 1: I/O | ✅ مكتمل | 3 أيام | Done |
| Phase 2: String | ✅ مكتمل | 2 أيام | Done |
| Phase 3: Array | 🔴 عالية | 5 أيام | Todo |
| Phase 4: Math | 🟠 متوسطة | 4 أيام | Todo |
| Phase 5: Type | 🟡 منخفضة | 4 أيام | Todo |

---

## 🔗 روابط مفيدة / Useful Links

### الملفات الموجودة / Existing Files
- `include/stdlib/io/io_functions.h` ✅
- `include/stdlib/string/string_functions.h` ✅
- `src/stdlib/io/io_functions.cpp` ✅
- `src/stdlib/string/string_functions.cpp` ✅
- `src/interpreter/core/builtin_registry.cpp` (للتحديث)

### المستندات / Documentation
- `docs/stlib_plan/STDLIB_BUILDING_PLAN.md`
- `docs/stlib_plan/PHASE2_STRING_COMPLETION_REPORT.md`
- `docs/stlib_plan/PHASE_TRACKING.md`

---

## 🎯 الخطوة التالية الفورية / Immediate Next Step

**الآن:** قم بتحديث `builtin_registry.cpp` لتسجيل دوال String

**الأمر:**
```powershell
# 1. افتح الملف
code src/interpreter/core/builtin_registry.cpp

# 2. أضف #include في الأعلى
#include "stdlib/string/string_functions.h"

# 3. سجل الدوال الـ 12 (عربي + إنجليزي = 24 تسجيل)

# 4. ابنِ المشروع
cmake --build build --config Debug

# 5. اختبر
.\build\bin\Debug\sad.exe test_string.s
```

---

✅ **Phase 2 مكتمل! الآن سجل الدوال واختبرها.**
