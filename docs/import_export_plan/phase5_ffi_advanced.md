# المرحلة 5: FFI المتقدم
# Phase 5: Advanced Foreign Function Interface

**الحالة / Status:** 📅 مُخطط / Planned  
**الأولوية / Priority:** P1 (عالية / High)  
**الوقت المقدر / Estimated Time:** 3-4 أسابيع  
**المتطلبات السابقة / Prerequisites:** ✅ المرحلة 1

---

## 📋 نظرة عامة / Overview

### بالعربية
نظام FFI متقدم للتكامل السلس مع C/C++. يوفر:
- **ربط دوال C/C++** التلقائي
- **Struct/Class mapping** ثنائي الاتجاه
- **Callbacks** من C++ إلى Sad
- **إدارة ذاكرة آمنة** تلقائية
- **توليد bindings** من header files
- **دعم shared libraries** الديناميكي

### English
Advanced FFI system for seamless C/C++ integration, providing:
- **Automatic C/C++ function binding**
- **Bi-directional struct/class mapping**
- **Callbacks** from C++ to Sad
- **Automatic safe memory management**
- **Binding generation** from header files
- **Dynamic shared library** support

---

## 🎯 الأهداف / Goals

### Core Features
1. ✅ `@أصلي` للدوال الأصلية (موجود حالياً)
2. ⬜ Struct mapping تلقائي
3. ⬜ Class binding مع constructors/destructors
4. ⬜ Callbacks من C++ إلى Sad
5. ⬜ Exception handling
6. ⬜ Memory management آمن
7. ⬜ Shared library loader

### Advanced Features
- ⬜ توليد bindings تلقائي من `.h`
- ⬜ دعم C++ templates
- ⬜ دعم STL containers
- ⬜ Thread safety

---

## 🔗 ربط الدوال الأساسي / Basic Function Binding

### النحو الحالي / Current Syntax

```s
# تعريف دالة أصلية
@أصلي("libmath")
دالة رقم sqrt(رقم ع)
نهاية

# الاستخدام
رقم نتيجة = sqrt(16.0)  # يستدعي دالة C
اطبع(نتيجة)  # 4.0
```

---

### التحسينات المقترحة / Proposed Enhancements

```s
# 1. تحديد المكتبة بوضوح
@أصلي(library: "libmath.so", symbol: "sqrt")
دالة رقم جذر_أصلي(رقم ع)
نهاية

# 2. مع معلومات النوع الكاملة
@أصلي("libc")
@c_signature("int strcmp(const char*, const char*)")
دالة رقم مقارنة_نصوص(نص أ، نص ب)
نهاية

# 3. دوال variadic
@أصلي("libc")
@variadic
دالة رقم printf(نص تنسيق، ...)
نهاية
```

---

## 🏗️ Struct Mapping

### C Struct → Sad Class

**C Header (math_types.h):**
```c
typedef struct {
    double x;
    double y;
} Point2D;

double point_distance(Point2D* p1, Point2D* p2);
```

**Sad Binding:**
```s
# تعريف Struct Mapping
@c_struct("Point2D", "libgeometry")
صنف نقطة_2د
    @c_field("x") رقم س
    @c_field("y") رقم ص
    
    # يُولَّد تلقائياً
    باني(رقم س، رقم ص)
        هذا.س = س
        هذا.ص = ص
    نهاية
نهاية

# ربط الدالة
@أصلي("libgeometry")
@c_signature("double point_distance(Point2D*, Point2D*)")
دالة رقم مسافة(نقطة_2د ن1، نقطة_2د ن2)
نهاية

# الاستخدام
متغير ن1 = نقطة_2د(3.0، 4.0)
متغير ن2 = نقطة_2د(0.0، 0.0)
رقم مسافة = مسافة(ن1، ن2)
اطبع(مسافة)  # 5.0
```

---

### Sad Class → C Struct (Export)

```s
# تصدير صنف إلى C
@c_export("SadString")
صنف نص_ص
    نص محتوى
    رقم طول
    
    باني(نص ن)
        هذا.محتوى = ن
        هذا.طول = ن.طول()
    نهاية
    
    @c_method("sad_string_concat")
    دالة نص دمج(نص آخر)
        إرجاع هذا.محتوى + آخر
    نهاية
نهاية
```

**الـ C API المُولَّد:**
```c
// Generated: sad_string.h
typedef struct {
    const char* content;
    size_t length;
} SadString;

SadString* sad_string_create(const char* str);
void sad_string_destroy(SadString* s);
SadString* sad_string_concat(SadString* s, const char* other);
```

---

## 🔄 Callbacks

### من C إلى Sad

**C Library:**
```c
// file_utils.h
typedef void (*FileCallback)(const char* filename, int size);

void iterate_files(const char* dir, FileCallback callback);
```

**Sad Binding:**
```s
# تعريف نوع Callback
@c_callback("FileCallback")
نوع معالج_ملف = دالة(نص اسم_الملف، رقم حجم)

# ربط الدالة
@أصلي("libfileutils")
دالة تصفح_ملفات(نص مجلد، معالج_ملف معالج)
نهاية

# الاستخدام
دالة معالج_ملف_خاص(نص اسم، رقم حجم)
    اطبع(ن"ملف: {اسم} - حجم: {حجم} بايت")
نهاية

تصفح_ملفات("/tmp", معالج_ملف_خاص)
```

---

### من Sad إلى C (Export Callback)

```s
# تصدير دالة Sad كـ C callback
@c_export_callback("sad_comparator")
دالة رقم مقارن_نصوص(نص أ، نص ب)
    إذا أ < ب
        إرجاع -1
    وإلا_إذا أ > ب
        إرجاع 1
    نهاية
    إرجاع 0
نهاية

# استخدامها في C
@أصلي("libc")
دالة ترتيب(مصفوفة قائمة، دالة مقارن)
نهاية

متغير أسماء = ["محمد", "أحمد", "علي"]
ترتيب(أسماء, مقارن_نصوص)
```

---

## 🧠 إدارة الذاكرة / Memory Management

### RAII Wrappers

```cpp
// include/ffi/native_handle.h
#pragma once

#include <memory>
#include <functional>

namespace Sad {
namespace FFI {

/**
 * مؤشر ذكي لموارد C
 * Smart pointer for C resources
 */
template<typename T>
class NativeHandle {
public:
    using Deleter = std::function<void(T*)>;
    
    NativeHandle(T* ptr, Deleter deleter)
        : ptr_(ptr, deleter) {}
    
    T* get() const { return ptr_.get(); }
    T* operator->() const { return ptr_.get(); }
    
    // منع النسخ، السماح بالنقل
    NativeHandle(const NativeHandle&) = delete;
    NativeHandle& operator=(const NativeHandle&) = delete;
    
    NativeHandle(NativeHandle&&) = default;
    NativeHandle& operator=(NativeHandle&&) = default;
    
private:
    std::unique_ptr<T, Deleter> ptr_;
};

} // namespace FFI
} // namespace Sad
```

---

### الاستخدام في Sad

```s
# تعريف مورد يحتاج تنظيف
@أصلي("libfile")
@returns_handle(deleter: "fclose")
دالة مؤشر فتح_ملف(نص مسار، نص وضع)
نهاية

# الاستخدام - التنظيف تلقائي!
متغير ملف = فتح_ملف("data.txt", "r")
# ... استخدم الملف
# نهاية الدالة - يُغلق الملف تلقائياً (fclose)
```

---

## 🔧 توليد Bindings تلقائي

### أداة: sad-bindgen

```bash
# توليد bindings من C header
$ sad-bindgen libmath.h -o bindings/math.s

Parsing libmath.h...
Found 15 functions
Found 3 structs
Found 2 enums

Generated:
  - bindings/math.s       (Sad declarations)
  - bindings/math_impl.cpp (C++ glue code)
```

---

### مثال Output

**Input (libmath.h):**
```c
double sqrt(double x);
double pow(double base, double exp);

typedef struct {
    double x, y;
} Vector2;

Vector2 vector_add(Vector2 a, Vector2 b);
```

**Output (bindings/math.s):**
```s
# Auto-generated bindings for libmath
# DO NOT EDIT - Generated by sad-bindgen

@أصلي("libmath")
دالة رقم sqrt(رقم x)
نهاية

@أصلي("libmath")
دالة رقم pow(رقم base، رقم exp)
نهاية

@c_struct("Vector2", "libmath")
صنف متجه_2
    رقم x
    رقم y
نهاية

@أصلي("libmath")
دالة متجه_2 vector_add(متجه_2 a، متجه_2 b)
نهاية
```

---

## 🚀 Shared Library Loader

### Dynamic Loading

```s
# تحميل مكتبة ديناميكياً
استورد نظام.مكتبات

# فتح المكتبة
متغير مكتبة = نظام.مكتبات.فتح("libcustom.so")

# الحصول على دالة
متغير دالة_مخصصة = مكتبة.احصل_على("custom_function")

# استدعاء الدالة
متغير نتيجة = دالة_مخصصة.استدعي([42، "test"])

# إغلاق المكتبة
مكتبة.أغلق()
```

---

### API التحميل الديناميكي

```cpp
// include/ffi/dynamic_loader.h
#pragma once

#include <string>
#include <memory>
#include <map>

namespace Sad {
namespace FFI {

/**
 * محمل المكتبات الديناميكي
 */
class DynamicLibrary {
public:
    /**
     * فتح مكتبة
     */
    static std::shared_ptr<DynamicLibrary> open(
        const std::string& libPath);
    
    /**
     * الحصول على رمز (symbol)
     */
    void* getSymbol(const std::string& symbolName);
    
    /**
     * التحقق من وجود رمز
     */
    bool hasSymbol(const std::string& symbolName);
    
    /**
     * إغلاق المكتبة
     */
    ~DynamicLibrary();
    
private:
    void* handle_;
    std::string path_;
    
    DynamicLibrary(void* handle, const std::string& path);
};

/**
 * مدير المكتبات
 */
class LibraryManager {
public:
    static LibraryManager& getInstance();
    
    /**
     * تحميل مكتبة
     */
    std::shared_ptr<DynamicLibrary> loadLibrary(
        const std::string& name);
    
    /**
     * إلغاء تحميل مكتبة
     */
    void unloadLibrary(const std::string& name);
    
    /**
     * الحصول على مكتبة محملة
     */
    std::shared_ptr<DynamicLibrary> getLibrary(
        const std::string& name);
    
private:
    std::map<std::string, std::shared_ptr<DynamicLibrary>> libraries_;
};

} // namespace FFI
} // namespace Sad
```

---

## 🛡️ Exception Handling

### معالجة أخطاء C++

```s
# دالة C++ قد ترمي استثناءات
@أصلي("libcpputils")
@throws("std::runtime_error", "std::invalid_argument")
دالة نص معالج_json(نص json_str)
نهاية

# الاستخدام مع معالجة الأخطاء
حاول
    متغير بيانات = معالج_json("{\"key\": \"value\"}")
    اطبع(بيانات)
أمسك خطأ
    اطبع(ن"خطأ: {خطأ.رسالة}")
نهاية
```

---

### التنفيذ

```cpp
// src/ffi/exception_bridge.cpp

// Wrapper تلقائي حول دوال C++
extern "C" {
    const char* sad_process_json_wrapper(const char* json_str) {
        try {
            // استدعاء الدالة الأصلية
            std::string result = process_json(json_str);
            return strdup(result.c_str());
        }
        catch (const std::runtime_error& e) {
            sad_set_last_error("RuntimeError", e.what());
            return nullptr;
        }
        catch (const std::invalid_argument& e) {
            sad_set_last_error("InvalidArgument", e.what());
            return nullptr;
        }
        catch (...) {
            sad_set_last_error("UnknownError", "Unknown C++ exception");
            return nullptr;
        }
    }
}
```

---

## 📚 أمثلة عملية / Practical Examples

### مثال 1: ربط مكتبة SQLite

```s
# ملف: bindings/sqlite.s

@c_struct("sqlite3", "libsqlite3")
صنف قاعدة_بيانات
    # مؤشر داخلي - يُدار تلقائياً
نهاية

@أصلي("libsqlite3")
@returns_handle(deleter: "sqlite3_close")
دالة قاعدة_بيانات sqlite3_open(نص مسار)
نهاية

@أصلي("libsqlite3")
دالة رقم sqlite3_exec(
    قاعدة_بيانات قب،
    نص استعلام،
    دالة معالج،
    مؤشر بيانات
)
نهاية

# الاستخدام
متغير قب = sqlite3_open("test.db")

دالة معالج_صف(مؤشر بيانات، رقم عدد_أعمدة، مصفوفة قيم، مصفوفة أسماء)
    لكل قيمة في قيم
        اطبع(قيمة)
    نهاية
    إرجاع 0
نهاية

sqlite3_exec(قب, "SELECT * FROM users", معالج_صف, فارغ)
# التنظيف تلقائي!
```

---

### مثال 2: ربط OpenGL

```s
# ملف: bindings/opengl.s

@أصلي("libGL")
دالة glClear(رقم mask)
نهاية

@أصلي("libGL")
دالة glClearColor(رقم r، رقم g، رقم b، رقم a)
نهاية

@أصلي("libGL")
دالة glBegin(رقم mode)
نهاية

@أصلي("libGL")
دالة glEnd()
نهاية

@أصلي("libGL")
دالة glVertex3f(رقم x، رقم y، رقم z)
نهاية

# الثوابت
صدر GL_COLOR_BUFFER_BIT = 0x00004000
صدر GL_TRIANGLES = 0x0004

# استخدام
دالة رسم()
    glClearColor(0.0, 0.0, 0.0, 1.0)
    glClear(GL_COLOR_BUFFER_BIT)
    
    glBegin(GL_TRIANGLES)
        glVertex3f(0.0, 1.0, 0.0)
        glVertex3f(-1.0, -1.0, 0.0)
        glVertex3f(1.0, -1.0, 0.0)
    glEnd()
نهاية
```

---

### مثال 3: Callback معقد (libcurl)

```s
@أصلي("libcurl")
صنف curl_handle
نهاية

@c_callback("curl_write_callback")
نوع معالج_كتابة = دالة(مؤشر محتوى، رقم حجم، رقم nmemb، مؤشر userdata) → رقم

@أصلي("libcurl")
دالة curl_handle curl_easy_init()
نهاية

@أصلي("libcurl")
دالة رقم curl_easy_setopt(curl_handle مؤشر، رقم خيار، مؤشر قيمة)
نهاية

@أصلي("libcurl")
دالة رقم curl_easy_perform(curl_handle مؤشر)
نهاية

# الاستخدام
متغير استجابة = ""

دالة رقم معالج_بيانات(مؤشر محتوى، رقم حجم، رقم nmemb، مؤشر userdata)
    متغير بيانات = محتوى_كـ_نص(محتوى، حجم * nmemb)
    استجابة += بيانات
    إرجاع حجم * nmemb
نهاية

متغير curl = curl_easy_init()
curl_easy_setopt(curl, CURLOPT_URL, "https://api.example.com")
curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, معالج_بيانات)
curl_easy_perform(curl)

اطبع(استجابة)
```

---

## 🧪 الاختبارات / Testing

### Test Cases

```cpp
/**
 * اختبار 1: استدعاء دالة C بسيطة
 */
TEST(FFI, SimpleCall) {
    // sqrt من libm
    auto result = callNativeFunction("libm", "sqrt", {16.0});
    EXPECT_DOUBLE_EQ(result.asDouble(), 4.0);
}

/**
 * اختبار 2: Struct mapping
 */
TEST(FFI, StructMapping) {
    // إنشاء Point2D
    auto point = createNativeStruct("Point2D", {3.0, 4.0});
    
    EXPECT_DOUBLE_EQ(point.getField("x").asDouble(), 3.0);
    EXPECT_DOUBLE_EQ(point.getField("y").asDouble(), 4.0);
}

/**
 * اختبار 3: Callback
 */
TEST(FFI, Callback) {
    int callCount = 0;
    
    auto callback = [&](int value) {
        callCount++;
        return value * 2;
    };
    
    registerCallback("test_callback", callback);
    callNativeFunction("libtest", "iterate", {5, "test_callback"});
    
    EXPECT_EQ(callCount, 5);
}

/**
 * اختبار 4: Exception handling
 */
TEST(FFI, ExceptionHandling) {
    EXPECT_THROW(
        callNativeFunction("libcpp", "throw_exception", {}),
        SadRuntimeError
    );
}
```

---

## 📊 مؤشرات الأداء / Performance Metrics

- ⚡ **Overhead استدعاء FFI:** < 10ns للدوال البسيطة
- ⚡ **Struct marshalling:** < 50ns للـ struct صغير
- ⚡ **Callback overhead:** < 100ns
- 🛡️ **Thread safety:** كامل مع locks حسب الحاجة
- 💾 **Memory leaks:** صفر (RAII + GC)

---

## ✅ قائمة المهام / Task Checklist

### الأسبوع 1
- [ ] تحسين `@أصلي` الحالي
- [ ] Struct mapping أساسي
- [ ] Type conversion helpers

### الأسبوع 2
- [ ] Callback system
- [ ] Exception bridge
- [ ] Memory management (RAII)

### الأسبوع 3
- [ ] Dynamic library loader
- [ ] `sad-bindgen` tool
- [ ] دعم C++ classes

### الأسبوع 4
- [ ] STL bindings
- [ ] Thread safety
- [ ] اختبارات شاملة
- [ ] التوثيق

---

## 📚 ملفات التنفيذ / Implementation Files

```
src/ffi/
├── native_function.{h,cpp}      # استدعاء دوال C
├── struct_mapping.{h,cpp}       # Struct ↔ Class
├── callback_bridge.{h,cpp}      # Callbacks
├── exception_bridge.{h,cpp}     # Exception handling
├── native_handle.{h,cpp}        # إدارة الذاكرة
├── dynamic_loader.{h,cpp}       # تحميل .so/.dll
└── type_conversion.{h,cpp}      # تحويل الأنواع

tools/
└── sad-bindgen/
    ├── main.cpp
    ├── header_parser.{h,cpp}
    ├── binding_generator.{h,cpp}
    └── templates/
        ├── function.template
        └── struct.template

tests/ffi/
├── test_native_calls.cpp
├── test_struct_mapping.cpp
├── test_callbacks.cpp
├── test_exceptions.cpp
└── fixtures/
    ├── libtest.c
    └── libtest.h
```

---

## 🎓 مراجع / References

- [FFI Best Practices](https://www.rust-lang.org/what/ffi)
- [ctypes Documentation](https://docs.python.org/3/library/ctypes.html)
- [JNA (Java Native Access)](https://github.com/java-native-access/jna)
- [SWIG](http://www.swig.org/)

---

## 🏁 الخاتمة / Conclusion

بإكمال جميع المراحل الخمس، ستمتلك لغة ص:
- ✅ نظام استيراد/تصدير قوي
- ✅ محمل وحدات ذكي
- ✅ خادم لغة كامل (LSP)
- ✅ نظام حزم حديث
- ✅ FFI متقدم

**🎉 لغة جاهزة للإنتاج!**

---

**آخر تحديث / Last Updated:** 8 ديسمبر 2025
