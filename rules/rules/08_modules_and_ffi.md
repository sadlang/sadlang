# 📦 النماذج و FFI / Modules & FFI

**التاريخ:** 10 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مرجع تفصيلي لنظام النماذج و FFI

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **نظام النماذج الكامل** للغة "ص"، بما في ذلك الاستيراد، التصدير، التحميل الديناميكي، FFI (Foreign Function Interface) للتكامل مع C/C++، ونظام الأمان (sandboxing).

### (EN) Description
This file documents the **complete module system** for Sad language, including import/export, dynamic loading, FFI (Foreign Function Interface) for C/C++ integration, and security sandboxing.

---

## 📦 الاعتماديات / Dependencies

- ✅ `docs/language_spec/02_functions.md` - Functions for module exports
- ✅ `docs/language_spec/03_oop.md` - OOP for module classes
- ✅ `docs/language_spec/06_cpp_features.md` - Native bindings (`@أصلي`)
- ✅ `plans/imp/13_stdlib_and_modules_plan.md` - Module implementation

---

## 📋 قائمة الميزات / Features List

| # | الميزة (AR) | Feature (EN) | الحالة | الأولوية |
|---|-----------|-----------|--------|---------|
| 1 | استيراد أساسي | Basic import | ✅ | P0 |
| 2 | استيراد انتقائي | Selective import | ✅ | P0 |
| 3 | تصدير | Export | ✅ | P0 |
| 4 | نماذج داخلية | Built-in modules | ✅ | P0 |
| 5 | نماذج مخصصة | Custom modules | ✅ | P0 |
| 6 | مسارات البحث | Search paths | ✅ | P1 |
| 7 | تحميل ديناميكي | Dynamic loading | 🔧 | P1 |
| 8 | FFI / C API | Native bindings | 🔧 | P1 |
| 9 | Sandboxing | Security | 🔧 | P2 |

---

## 1️⃣ Basic Import (الاستيراد الأساسي)

### Syntax

```ebnf
import_stmt ::= 'استورد' module_name ['كـ' alias]
              | 'من' module_name 'استورد' ('*' | identifier_list)
              
module_name ::= identifier ('.' identifier)*
identifier_list ::= identifier ('،' identifier)*
alias ::= identifier
```

### Examples

```s
// 1. استيراد نموذج كامل / Import entire module
استورد رياضيات

اطبع(رياضيات.ط)           // 3.14159...
اطبع(رياضيات.جذر(16))     // 4

// 2. استيراد مع اسم مستعار / Import with alias
استورد رياضيات كـ م

اطبع(م.ط)
اطبع(م.قوة(2، 10))        // 1024

// 3. استيراد انتقائي / Selective import
من رياضيات استورد جذر، قوة، ط

اطبع(ط)
اطبع(جذر(25))
اطبع(قوة(3، 3))           // 27

// 4. استيراد كل شيء / Import everything
من رياضيات استورد *

اطبع(جيب(ط / 2))         // 1.0
اطبع(جيب_تمام(0))        // 1.0
```

---

## 2️⃣ Module Structure (بنية النموذج)

### File: `رياضيات.ص` (Math Module Example)

```s
// ===========================================
// نموذج: رياضيات / Module: Math
// ===========================================

// ثوابت عامة / Public constants
صدر ط = 3.14159265359
صدر ه = 2.71828182846

// دوال عامة / Public functions
صدر دالة جذر(رقم ع)
    إرجاع ع ** 0.5
نهاية

صدر دالة قوة(رقم أساس، رقم أس)
    إرجاع أساس ** أس
نهاية

صدر دالة قيمة_مطلقة(رقم ع)
    إذا ع < 0
        إرجاع -ع
    نهاية
    إرجاع ع
نهاية

// دالة خاصة / Private function (not exported)
دالة _مساعد_داخلي(رقم س)
    إرجاع س * 2
نهاية

// صنف عام / Public class
صدر صنف متجه
    رقم س
    رقم ص
    
    باني(رقم س، رقم ص)
        هذا.س = س
        هذا.ص = ص
    نهاية
    
    دالة رقم طول()
        إرجاع جذر(هذا.س ** 2 + هذا.ص ** 2)
    نهاية
نهاية

// تهيئة النموذج / Module initialization
اطبع("تم تحميل نموذج 'رياضيات'")
```

### Usage

```s
استورد رياضيات

// استخدام الثوابت / Use constants
اطبع(رياضيات.ط)

// استخدام الدوال / Use functions
رقم ج = رياضيات.جذر(144)
اطبع(ج)  // 12

// استخدام الصنف / Use class
متجه م = رياضيات.متجه(3، 4)
اطبع(م.طول())  // 5.0

// لا يمكن الوصول للدوال الخاصة / Cannot access private functions
// رياضيات._مساعد_داخلي(10)  // خطأ! / Error!
```

---

## 3️⃣ Export System (نظام التصدير)

### Export Keyword

```ebnf
export_stmt ::= 'صدر' (var_decl | func_decl | class_decl)
```

### Visibility Rules

| الكلمة (AR) | Keyword (EN) | Visibility |
|-----------|-----------|-----------|
| `صدر` | export | Public - accessible from outside |
| (none) | (none) | Private - only within module |

### Examples

```s
// ملف: أدوات.ص / File: utils.s

// عام / Public
صدر دالة طباعة_جميلة(نص رسالة)
    اطبع(">>> " + رسالة + " <<<")
نهاية

// خاص / Private
دالة _معالج_داخلي(نص ن)
    إرجاع ن.استبدل(" "، "_")
نهاية

// عام / Public
صدر صنف سجل
    مصفوفة سجلات = []
    
    دالة أضف(نص رسالة)
        هذا.سجلات.أضف(رسالة)
    نهاية
    
    دالة اعرض()
        لكل س في هذا.سجلات
            طباعة_جميلة(س)
        نهاية
    نهاية
نهاية

// عام / Public
صدر رقم إصدار = 1.5
```

```s
// الاستخدام / Usage
استورد أدوات

أدوات.طباعة_جميلة("مرحباً")  // ✓
اطبع(أدوات.إصدار)            // 1.5 ✓

سجل س = أدوات.سجل()
س.أضف("سجل 1")
س.اعرض()                     // ✓

// أدوات._معالج_داخلي("test")  // ✗ خطأ!
```

---

## 4️⃣ Built-in Modules (النماذج الداخلية)

### Standard Library Modules

```s
// 1. رياضيات / Math
استورد رياضيات
رياضيات.جذر(16)
رياضيات.جيب(رياضيات.ط / 2)

// 2. سلاسل / Strings
استورد سلاسل
سلاسل.تقسيم("أ،ب،ج"، "،")
سلاسل.دمج(["أ"، "ب"]، "-")

// 3. وقت / Time
استورد وقت
وقت.الآن()
وقت.انتظر(1.5)

// 4. نظام / System
استورد نظام
نظام.المنصة()
نظام.تنفيذ("dir")

// 5. ملفات / Files
استورد ملفات
ملفات.اقرأ("بيانات.txt")
ملفات.اكتب("ناتج.txt"، "محتوى")

// 6. رسوم / Graphics
استورد رسوم
نافذة ن = رسوم.نافذة(800، 600)

// 7. شبكة / Network (future)
استورد شبكة
شبكة.طلب("https://api.example.com")
```

---

## 5️⃣ Module Search Paths (مسارات البحث)

### Search Order

```
1. الدليل الحالي / Current directory
   ./module.s

2. مكتبة قياسية / Standard library
   <stdlib>/module.s
   
3. مسارات إضافية / Additional paths (from environment)
   $SAD_PATH/module.s
   
4. مجلد النماذج المحلي / Local modules folder
   ~/.sad/modules/module.s
```

### API for Path Management

```cpp
// include/modules/module_loader.h
class ModuleLoader {
public:
    // Add search path
    static void addSearchPath(const std::string& path);
    
    // Get all search paths
    static std::vector<std::string> getSearchPaths();
    
    // Find module file
    static std::string findModule(const std::string& name);
    
    // Load module
    static Module* loadModule(const std::string& name);
    
    // Reload module
    static void reloadModule(const std::string& name);
};
```

### Examples

```s
// إضافة مسار بحث / Add search path
نظام.مسارات.أضف("/مسارات/نماذجي")

// عرض المسارات / Show paths
لكل مسار في نظام.مسارات.احصل()
    اطبع(مسار)
نهاية

// استيراد من مسار مخصص / Import from custom path
استورد نموذجي_الخاص
```

---

## 6️⃣ Dynamic Loading (التحميل الديناميكي)

### Runtime Import

```s
// تحميل نموذج في وقت التشغيل / Load module at runtime
دالة تحميل_نموذج(نص اسم)
    @إذا (نظام.موجود(اسم + ".ص"))
        إرجاع __استورد__(اسم)
    @وإلا
        اطبع(ن"النموذج '{اسم}' غير موجود")
        إرجاع لاغ
    @نهاية
نهاية

// استخدام / Usage
نص اسم_نموذج = "رياضيات"
نموذج م = تحميل_نموذج(اسم_نموذج)

إذا م != لاغ
    رقم نتيجة = م.جذر(25)
    اطبع(نتيجة)
نهاية
```

### Conditional Imports

```s
// استيراد حسب المنصة / Platform-specific import
@إذا نظام.المنصة() == "ويندوز"
    استورد نماذج_ويندوز
@وإلا_إذا نظام.المنصة() == "لينكس"
    استورد نماذج_لينكس
@نهاية

// استيراد حسب الميزة / Feature-based import
@إذا ميزة("رسوم")
    استورد رسوم
@نهاية
```

---

## 7️⃣ FFI / C API (التكامل مع C/C++)

### Native Binding with `@أصلي`

```s
// تعريف دالة أصلية / Declare native function
@أصلي("libmath")
دالة رقم جذر_سريع(رقم ع)
نهاية

@أصلي("libmath")
دالة رقم قوة_سريعة(رقم أساس، رقم أس)
نهاية

// استخدام / Usage
رقم ج = جذر_سريع(144)
اطبع(ج)  // 12 (computed by native C function)
```

### C API Header

```cpp
// include/api/sad_c_api.h
#ifndef SAD_C_API_H
#define SAD_C_API_H

#include <stdint.h>
#include <stdbool.h>

// Value types
typedef enum {
    SAD_TYPE_NULL,
    SAD_TYPE_BOOL,
    SAD_TYPE_INT,
    SAD_TYPE_FLOAT,
    SAD_TYPE_STRING,
    SAD_TYPE_ARRAY,
    SAD_TYPE_OBJECT
} SadValueType;

// Value structure
typedef struct SadValue {
    SadValueType type;
    union {
        bool b;
        int64_t i;
        double f;
        char* s;
        void* ptr;
    } data;
} SadValue;

// Function signature for native functions
typedef SadValue (*SadNativeFunc)(int argc, SadValue* argv);

// API functions
extern "C" {
    // Register native function
    void sad_register_function(const char* module_name, 
                               const char* func_name,
                               SadNativeFunc func);
    
    // Register native module
    void sad_register_module(const char* module_name,
                            void (*init_func)(void));
    
    // Value helpers
    SadValue sad_make_int(int64_t value);
    SadValue sad_make_float(double value);
    SadValue sad_make_string(const char* value);
    SadValue sad_make_bool(bool value);
    SadValue sad_make_null();
    
    // Type checking
    bool sad_is_int(SadValue v);
    bool sad_is_float(SadValue v);
    bool sad_is_string(SadValue v);
    
    // Value extraction
    int64_t sad_to_int(SadValue v);
    double sad_to_float(SadValue v);
    const char* sad_to_string(SadValue v);
    
    // Error handling
    void sad_throw_error(const char* message);
}

#endif // SAD_C_API_H
```

### C Native Module Example

```c
// libmath.c - Native math library
#include "sad_c_api.h"
#include <math.h>

// Native sqrt function
SadValue native_sqrt(int argc, SadValue* argv) {
    if (argc != 1) {
        sad_throw_error("sqrt expects 1 argument");
        return sad_make_null();
    }
    
    if (!sad_is_float(argv[0]) && !sad_is_int(argv[0])) {
        sad_throw_error("sqrt expects a number");
        return sad_make_null();
    }
    
    double value = sad_is_float(argv[0]) ? 
                   sad_to_float(argv[0]) : 
                   (double)sad_to_int(argv[0]);
    
    double result = sqrt(value);
    return sad_make_float(result);
}

// Native pow function
SadValue native_pow(int argc, SadValue* argv) {
    if (argc != 2) {
        sad_throw_error("pow expects 2 arguments");
        return sad_make_null();
    }
    
    double base = sad_to_float(argv[0]);
    double exponent = sad_to_float(argv[1]);
    
    double result = pow(base, exponent);
    return sad_make_float(result);
}

// Module initialization
void init_libmath() {
    sad_register_function("libmath", "جذر_سريع", native_sqrt);
    sad_register_function("libmath", "قوة_سريعة", native_pow);
}

// Entry point
extern "C" void sad_module_init() {
    init_libmath();
}
```

### Building Native Module

```cmake
# CMakeLists.txt for native module
cmake_minimum_required(VERSION 3.15)
project(libmath)

# Find Sad C API
find_package(SadLang REQUIRED)

# Create shared library
add_library(libmath SHARED libmath.c)
target_include_directories(libmath PRIVATE ${SADLANG_INCLUDE_DIRS})
target_link_libraries(libmath ${SADLANG_LIBRARIES})

# Install to modules directory
install(TARGETS libmath DESTINATION ${SADLANG_MODULE_DIR})
```

---

## 8️⃣ Sandboxing & Security (الأمان)

### Permission System

```s
// طلب صلاحيات / Request permissions
@صلاحيات(["ملفات.قراءة"، "شبكة.اتصال"])

استورد ملفات
استورد شبكة

// قراءة ملف (مسموح) / Read file (allowed)
نص محتوى = ملفات.اقرأ("بيانات.txt")

// الاتصال بالشبكة (مسموح) / Network access (allowed)
استجابة ج = شبكة.طلب("https://api.example.com")

// كتابة ملف (غير مسموح) / Write file (not allowed)
// ملفات.اكتب("ناتج.txt"، "محتوى")  // خطأ!
```

### Permission Types

```cpp
// include/modules/permissions.h
enum class Permission {
    FileRead,           // ملفات.قراءة
    FileWrite,          // ملفات.كتابة
    FileDelete,         // ملفات.حذف
    NetworkConnect,     // شبكة.اتصال
    NetworkListen,      // شبكة.استماع
    SystemExec,         // نظام.تنفيذ
    SystemEnv,          // نظام.بيئة
    GraphicsCreate,     // رسوم.إنشاء
    NativeLoad          // أصلي.تحميل
};

class Sandbox {
public:
    // Check permission
    static bool hasPermission(Permission perm);
    
    // Request permission
    static bool requestPermission(Permission perm);
    
    // Grant permission
    static void grantPermission(Permission perm);
    
    // Revoke permission
    static void revokePermission(Permission perm);
    
    // Get all permissions
    static std::vector<Permission> getAllPermissions();
};
```

### Restricted Operations

```s
// نموذج مقيد / Restricted module
@صلاحيات([])  // لا توجد صلاحيات / No permissions

استورد ملفات

حاول
    // محاولة قراءة ملف بدون صلاحية / Try to read without permission
    نص م = ملفات.اقرأ("بيانات.txt")
امسك خطأ_صلاحية خ
    اطبع("خطأ: لا توجد صلاحية لقراءة الملفات")
    اطبع(خ.رسالة)
نهاية
```

---

## 9️⃣ Module Caching (التخزين المؤقت)

### Cache Mechanism

```cpp
// include/modules/module_cache.h
class ModuleCache {
private:
    std::unordered_map<std::string, Module*> cache;
    
public:
    // Check if module is cached
    bool isCached(const std::string& name);
    
    // Get from cache
    Module* get(const std::string& name);
    
    // Add to cache
    void put(const std::string& name, Module* module);
    
    // Remove from cache
    void remove(const std::string& name);
    
    // Clear cache
    void clear();
    
    // Reload module
    void reload(const std::string& name);
};
```

### Examples

```s
// إعادة تحميل نموذج / Reload module
نظام.نماذج.إعادة_تحميل("رياضيات")

// مسح التخزين المؤقت / Clear cache
نظام.نماذج.مسح_مخزن()

// فحص التخزين / Check cache
إذا نظام.نماذج.موجود_في_مخزن("رياضيات")
    اطبع("النموذج محمل بالفعل")
نهاية
```

---

## 📝 Complete Example (مثال كامل)

### Custom Module: `حاسبة.ص`

```s
// ===========================================
// نموذج: حاسبة / Module: Calculator
// ===========================================

صدر رقم إصدار = 2.0

// عمليات أساسية / Basic operations
صدر دالة جمع(رقم أ، رقم ب)
    إرجاع أ + ب
نهاية

صدر دالة طرح(رقم أ، رقم ب)
    إرجاع أ - ب
نهاية

صدر دالة ضرب(رقم أ، رقم ب)
    إرجاع أ * ب
نهاية

صدر دالة قسمة(رقم أ، رقم ب)
    إذا ب == 0
        خطأ "لا يمكن القسمة على صفر"
    نهاية
    إرجاع أ / ب
نهاية

// عمليات متقدمة / Advanced operations
صدر صنف حاسبة_علمية
    مصفوفة سجل = []
    
    دالة احسب(نص عملية، رقم أ، رقم ب)
        رقم نتيجة
        
        إذا عملية == "+"
            نتيجة = جمع(أ، ب)
        وإلا_إذا عملية == "-"
            نتيجة = طرح(أ، ب)
        وإلا_إذا عملية == "*"
            نتيجة = ضرب(أ، ب)
        وإلا_إذا عملية == "/"
            نتيجة = قسمة(أ، ب)
        وإلا
            خطأ ن"عملية غير معروفة: {عملية}"
        نهاية
        
        // حفظ في السجل / Save to history
        هذا.سجل.أضف(ن"{أ} {عملية} {ب} = {نتيجة}")
        
        إرجاع نتيجة
    نهاية
    
    دالة اعرض_سجل()
        اطبع("=== سجل العمليات ===")
        لكل عملية في هذا.سجل
            اطبع(عملية)
        نهاية
    نهاية
نهاية

اطبع("✓ تم تحميل نموذج 'حاسبة' v" + نص(إصدار))
```

### Using the Module

```s
// استخدام نموذج 'حاسبة' / Use 'calculator' module

// 1. استيراد كامل / Full import
استورد حاسبة

اطبع(ن"الإصدار: {حاسبة.إصدار}")

رقم مجموع = حاسبة.جمع(10، 5)
رقم فرق = حاسبة.طرح(10، 5)
رقم حاصل = حاسبة.ضرب(10، 5)
رقم قسمة = حاسبة.قسمة(10، 5)

اطبع(ن"10 + 5 = {مجموع}")
اطبع(ن"10 - 5 = {فرق}")
اطبع(ن"10 * 5 = {حاصل}")
اطبع(ن"10 / 5 = {قسمة}")

// 2. استخدام الصنف / Use class
حاسبة_علمية ح = حاسبة.حاسبة_علمية()

ح.احسب("+"، 15، 8)
ح.احسب("-"، 20، 7)
ح.احسب("*"، 6، 9)
ح.احسب("/"، 100، 4)

ح.اعرض_سجل()

// 3. استيراد انتقائي / Selective import
من حاسبة استورد جمع، ضرب

رقم نتيجة = جمع(ضرب(3، 4)، 5)  // (3*4)+5 = 17
اطبع(ن"النتيجة: {نتيجة}")
```

---

## 🔧 Implementation Files

### Headers

1. **`include/modules/module.h`** - Module abstraction
2. **`include/modules/module_loader.h`** - Module loading
3. **`include/modules/module_cache.h`** - Caching
4. **`include/modules/permissions.h`** - Security
5. **`include/api/sad_c_api.h`** - C FFI API

### Sources

1. **`src/modules/module.cpp`**
2. **`src/modules/module_loader.cpp`**
3. **`src/modules/import_stmt.cpp`**
4. **`src/modules/export_registry.cpp`**
5. **`src/api/sad_c_api.cpp`**

---

## 🧪 Tests

### File: `tests/spec_rules/modules_import.s`

```s
// اختبار: نظام النماذج / Test: Module system
استورد حاسبة
من رياضيات استورد جذر، ط

// اختبار 1: استيراد أساسي / Test 1: Basic import
تأكد(حاسبة.جمع(2، 3) == 5)
تأكد(حاسبة.ضرب(4، 5) == 20)

// اختبار 2: استيراد انتقائي / Test 2: Selective import
تأكد(جذر(16) == 4)
تأكد(ط > 3.14 && ط < 3.15)

// اختبار 3: الصنف / Test 3: Class
حاسبة_علمية ح = حاسبة.حاسبة_علمية()
تأكد(ح.احسب("+"، 10، 5) == 15)

اطبع("✓ جميع الاختبارات نجحت!")
```

---

## 🔧 Git Workflow

```powershell
git checkout -b feat/spec/modules-ffi
git add docs/language_spec/rules/08_modules_and_ffi.md
git add tests/spec_rules/modules_import.s
git add include/modules/*.h
git add include/api/sad_c_api.h
git add src/modules/*.cpp
git commit -m "[spec/modules] Add module system and FFI specification"
git push --set-upstream origin feat/spec/modules-ffi
```

---

**آخر تحديث:** 10 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 25+
