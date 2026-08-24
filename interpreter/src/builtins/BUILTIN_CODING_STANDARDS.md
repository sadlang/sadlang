# قواعد كتابة ملفات الدوال المضمنة في المفسر — الإصدار 2.0
# Interpreter Builtin Module Coding Standards — v2.0

> **مستوى الالتزام: إلزامي** — أي مخالفة يجب إصلاحها قبل الدمج (merge).  
> هذا الملف يحدد القواعد **الصارمة والشاملة** لكتابة وتنظيم ملفات `interpreter_new/src/builtins/builtin_*.cpp`.  
> آخر تحديث: يناير 2025 — الإصدار 2.0

---

## فهرس المحتويات

1. [تسمية الملفات](#1-تسمية-الملفات)
2. [بنية رأس الملف](#2-بنية-رأس-الملف)
3. [ترتيب الـ Includes](#3-ترتيب-الـ-includes)
4. [بنية الدالة الرئيسية](#4-بنية-الدالة-الرئيسية)
5. [تنظيم الأقسام](#5-تنظيم-الأقسام)
6. [تسجيل الدوال — القواعد الست](#6-تسجيل-الدوال--القواعد-الست)
7. [التحقق من المعاملات](#7-التحقق-من-المعاملات)
8. [قواعد الأمان](#8-قواعد-الأمان)
9. [قواعد الأداء](#9-قواعد-الأداء)
10. [قواعد التزامن والخيوط](#10-قواعد-التزامن-والخيوط)
11. [ملف builtin_registry.cpp](#11-ملف-builtin_registrycpp)
12. [ملف builtin_common.h](#12-ملف-builtin_commonh)
13. [قائمة الانتهاكات الشائعة](#13-قائمة-الانتهاكات-الشائعة)
14. [إحصائيات الملفات](#14-إحصائيات-الملفات)
15. [إضافة دالة جديدة](#15-إضافة-دالة-جديدة)
16. [إضافة وحدة جديدة](#16-إضافة-وحدة-جديدة)
17. [قائمة المراجعة قبل الدمج](#17-قائمة-المراجعة-قبل-الدمج)
18. [أنماط مرفوضة ومقبولة](#18-أنماط-مرفوضة-ومقبولة)

---

## 1. تسمية الملفات

| النمط | الاستخدام | مثال |
|-------|-----------|------|
| `builtin_core_*.cpp` | دوال أساسية تُحمَّل تلقائياً بدون استيراد | `builtin_core_io.cpp` |
| `builtin_module_*.cpp` | وحدة تحتاج `استورد` | `builtin_module_strings.cpp` |
| `builtin_kernel_*.cpp` | دوال نظام التشغيل (kernel/OS) | `builtin_kernel_cpu.cpp` |
| `builtin_registry.cpp` | السجل المركزي + `suggestModuleForFunction` | ملف واحد فقط |
| `builtin_common.h` | Header موحد للـ includes المشتركة | ملف واحد فقط |

### ممنوع بشكل مطلق:
- ❌ أسماء عامة: `builtin_part1.cpp`، `builtin_extras.cpp`، `builtin_new.cpp`
- ❌ أرقام تسلسلية: `builtin_3.cpp`، `builtin_utils2.cpp`
- ❌ اختصارات غامضة: `builtin_misc.cpp`، `builtin_tmp.cpp`
- ❌ ملفات بدون بادئة `builtin_`: `helpers.cpp`، `utils.cpp`

### مسموح:
- ✅ أسماء وصفية: `builtin_module_crypto.cpp`، `builtin_kernel_display.cpp`
- ✅ أسماء مركبة واضحة: `builtin_module_advanced_math.cpp`

---

## 2. بنية رأس الملف (File Header)

### النموذج الإلزامي:

```cpp
/**
 * @file builtin_module_strings.cpp
 * @brief (AR) وحدة النصوص — دوال معالجة النصوص والمصفوفات والتحقق من الأنواع
 * @brief (EN) Strings module — string/array manipulation and type checking
 *
 * @details
 * (AR) الأقسام:
 *   1. دوال المصفوفات (أضف، أزل، فرز، عكس...)
 *   2. دوال النصوص (قسم، صل، استبدل، استخراج...)
 *   3. دوال التحقق من الأنواع (هو_رقم_صحيح، هو_نص...)
 *   4. دوال الرياضيات الأساسية (أكبر، أصغر...)
 *
 * @note يتطلب: builtin_common.h (الـ headers المشتركة)
 * @see builtin_registry.cpp — التسجيل المركزي
 * @see BUILTIN_CODING_STANDARDS.md — قواعد الكتابة
 */
```

### القواعد الصارمة:

| القاعدة | التفصيل |
|---------|---------|
| H1 | `@file` يجب أن يطابق اسم الملف **حرفياً** — أي اختلاف = انتهاك |
| H2 | `@brief` ثنائي اللغة: `(AR)` أولاً ثم `(EN)` — كلاهما إلزامي |
| H3 | `@details` يسرد أقسام الملف مرقمة (1، 2، 3...) |
| H4 | `@note` يذكر الاعتماديات الأساسية |
| H5 | `@see` يشير إلى الملفات المرتبطة (`builtin_registry.cpp` + هذا الملف) |
| H6 | لا يوجد `@author` أو `@date` — يُستبدل بتاريخ Git |

---

## 3. ترتيب الـ Includes

### الترتيب الإلزامي (4 مستويات):

```cpp
// المستوى 1: الـ header الموحد (دائماً أولاً)
#include "builtin_common.h"

// المستوى 2: headers خاصة بهذا الملف فقط (project-specific)
#include "crypto/sha256.h"
#include "object_instance.h"

// المستوى 3: standard library (ترتيب أبجدي إلزامي)
#include <algorithm>
#include <chrono>
#include <regex>

// المستوى 4: platform-specific (داخل #ifdef فقط)
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#endif
```

### الحدود القصوى:

| الحد | القيمة | التفصيل |
|------|--------|---------|
| I1 | **12 سطر include كحد أقصى** | لا يُحسب `builtin_common.h` ولا `#ifdef` platform |
| I2 | **0 includes مكررة مع builtin_common.h** | إذا كان في الـ umbrella، لا تكرره |
| I3 | **0 includes غير مستخدمة** | كل include يجب أن يُستخدم فعلياً في الملف |
| I4 | لا `#include` داخل دالة أو namespace | كل includes في أعلى الملف فقط |

### محتويات `builtin_common.h` (لا تكررها):
```
builtins.h, interpreter_core.h, io/io_functions.h, type_functions.h,
array_functions.h, other_functions.h, string/string_functions.h,
math/math_functions.h, math/advanced_math.h, system/system_functions.h,
filesystem/filesystem_module.h, <memory>, <string>, <vector>
```

---

## 4. بنية الدالة الرئيسية

### البنية الإلزامية:

```cpp
namespace Sad {
namespace Interpreter {

using namespace StdLib;

void registerBuiltinsModuleName(Interpreter& interpreter) {
    // ... أقسام الدوال ...
}

} // namespace Interpreter
} // namespace Sad
```

### القواعد:

| القاعدة | التفصيل |
|---------|---------|
| F1 | كل ملف يحتوي على **دالة تسجيل واحدة فقط** |
| F2 | اسم الدالة: `registerBuiltins` + اسم الوحدة بـ PascalCase |
| F3 | المعامل: `Interpreter& interpreter` (بالمرجع، ليس بالمؤشر) |
| F4 | مساحات الأسماء: `Sad::Interpreter` دائماً |
| F5 | `using namespace StdLib;` مسموح **مرة واحدة** فقط — قبل الدالة أو في بدايتها |
| F6 | لا تعريفات عامة (global variables) خارج الدالة — إلا `static` للحالة المشتركة |
| F7 | الحد الأقصى لحجم الملف: **2500 سطر** — إذا تجاوز، قسّمه |

---

## 5. تنظيم الأقسام

### فاصل القسم الرئيسي (إلزامي):

```cpp
    // ═══════════════════════════════════════════════════════════════
    // (AR) عنوان القسم بالعربية / (EN) Section Title in English
    // ═══════════════════════════════════════════════════════════════
```

### فاصل القسم الفرعي (اختياري — للملفات الكبيرة):

```cpp
    // ─── اسم_الدالة / function_name ───────────────────────────────
```

### القواعد:

| القاعدة | التفصيل |
|---------|---------|
| SEC1 | الفاصل الرئيسي: **67 حرف `═`** بين `//` |
| SEC2 | العنوان: `(AR) وصف_عربي / (EN) English description` |
| SEC3 | سطر فارغ **واحد** قبل الفاصل وبعده |
| SEC4 | لا خلط بين أنماط (`===` و `---` و `═══`) — فقط `═══` للرئيسي و `───` للفرعي |
| SEC5 | لا رموز تزيينية (`✅`، `📌`، `🔹`...) — التعليقات نصية فقط |
| SEC6 | لا أرقام مراحل (`Phase 16`، `المرحلة 5`) — استخدم أرقام أقسام (`القسم 1`) |

---

## 6. تسجيل الدوال — القواعد الست

### R1: تعليق إلزامي قبل كل lambda

```cpp
    // (AR) أضف — إضافة عنصر لنهاية مصفوفة
    // (EN) append — add element to end of array
    auto append_func = [](const std::vector<std::shared_ptr<Data::Value>>& args) {
        return BuiltinFunctions::append(args);
    };
```

**كل lambda بدون تعليق = انتهاك.** التعليق يجب أن يتضمن:
- اسم الدالة العربي
- وصف مختصر لما تفعله
- النسخة الإنجليزية

### R2: تسمية متغير lambda

| مقبول | مرفوض | السبب |
|-------|-------|-------|
| `append_func` | `fn1` | غير وصفي |
| `json_parse_func` | `jp` | غير واضح |
| `sha256_hash_func` | `f` | لا معنى |
| `sort_array_fn` | `دالة_فرز` | لا عربية في أسماء C++ |
| `tcp_send_func` | `lambda_3` | رقم بدون وصف |

**الصيغة المطلوبة:** `<وصف_إنجليزي_مختصر>_func` أو `<وصف>_fn`

### R3: تسجيل بأسماء اثنين فقط (عربي + إنجليزي)

```cpp
    // ✅ صحيح — اسم عربي واحد + إنجليزي واحد
    interpreter.getFunctionManager().registerBuiltinFunction("أضف", append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("append", append_func);

    // ❌ ممنوع — أسماء عربية متعددة
    interpreter.getFunctionManager().registerBuiltinFunction("أضف", append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("اضف", append_func);     // مكرر!
    interpreter.getFunctionManager().registerBuiltinFunction("إضافة", append_func);   // مكرر!

    // ❌ ممنوع — أسماء إنجليزية متعددة
    interpreter.getFunctionManager().registerBuiltinFunction("append", append_func);
    interpreter.getFunctionManager().registerBuiltinFunction("push", append_func);     // مكرر!
```

**القاعدة الصارمة:** اسم عربي واحد + اسم إنجليزي واحد = **تسجيلان فقط لكل دالة**.

### R4: رسائل الأخطاء ثنائية اللغة

```cpp
    // ✅ صحيح
    throw std::runtime_error("أضف: يتطلب وسيطاً واحداً على الأقل / append requires at least 1 argument");

    // ✅ صحيح أيضاً (بترميز AR/EN)
    throw std::runtime_error("(AR) يتطلب نص / (EN) requires string");

    // ❌ ممنوع — إنجليزي فقط
    throw std::runtime_error("requires string");

    // ⚠️ مقبول — عربي فقط (اللغة عربية أساساً)
    throw std::runtime_error("يتطلب نص");
```

**الأولوية:** ثنائي > عربي فقط > **ممنوع:** إنجليزي فقط

### R5: قواعد التقاط Lambda

```cpp
    // ✅ لا تحتاج المفسر — بدون التقاط
    auto abs_func = [](const auto& args) { ... };

    // ✅ تحتاج المفسر — التقاط محدد
    auto eval_func = [&interpreter](const auto& args) { ... };

    // ✅ تحتاج متغير ساكن — التقاط محدد
    auto counter_func = [&counter](const auto& args) { ... };

    // ❌ ممنوع بشكل مطلق — التقاط شامل
    auto bad_func = [=](const auto& args) { ... };
    auto worse_func = [&](const auto& args) { ... };
```

**ممنوع:** `[=]` و `[&]` — يجب تحديد كل متغير يُلتقط صراحةً.

### R6: نوع الإرجاع

```cpp
    // ✅ سطر واحد — implicit
    auto abs_func = [](const auto& args) {
        return BuiltinFunctions::abs(args);
    };

    // ✅ أكثر من سطر — explicit
    auto parse_func = [](const auto& args) -> std::shared_ptr<Data::Value> {
        if (args.empty()) throw std::runtime_error("...");
        auto str = args[0]->toString();
        // ... منطق معقد ...
        return std::make_shared<Data::Value>(result);
    };
```

---

## 7. التحقق من المعاملات

### النمط الإلزامي:

```cpp
    auto my_func = [](const std::vector<std::shared_ptr<Data::Value>>& args)
        -> std::shared_ptr<Data::Value> {
        
        // الخطوة 1: فحص العدد
        if (args.empty()) {
            throw std::runtime_error("دالتي: يتطلب وسيطاً / my_func requires argument");
        }
        
        // الخطوة 2: فحص النوع
        if (args[0]->getType() != Data::ValueType::STRING) {
            throw std::runtime_error("دالتي: يتطلب نص / my_func requires string");
        }
        
        // الخطوة 3: استخراج القيمة
        auto str = args[0]->toString();
        
        // الخطوة 4: فحص القيمة (إذا لزم)
        if (str.empty()) {
            throw std::runtime_error("دالتي: النص فارغ / my_func: empty string");
        }
        
        // الخطوة 5: المنطق
        // ...
        
        return result;
    };
```

### القواعد:

| القاعدة | التفصيل |
|---------|---------|
| P1 | فحص العدد **أولاً** قبل أي عملية |
| P2 | فحص النوع **ثانياً** قبل استخراج القيمة |
| P3 | لا تستخدم `assert()` — فقط `throw std::runtime_error(...)` |
| P4 | لا تتجاهل أخطاء — كل مسار إما يرجع قيمة أو يرمي |
| P5 | لا تصل لـ `args[i]` بدون فحص `args.size() > i` |
| P6 | رسالة الخطأ تبدأ باسم الدالة العربي ثم `/` ثم الإنجليزي |

### قيمة «لا نتيجة» — فراغٌ لا عدم (P7)

| القاعدة | التفصيل |
|---------|---------|
| P7-أ | بحثٌ بلا مطابقة (مفتاح غائب، متغيّر بيئة غائب، سجلّ غير موجود) يُرجِع **فراغًا**: `std::make_shared<Data::Value>()` — هذا نصُّ مصدرِ الحقيقة (`types.yaml` → `type.void`) |
| P7-ب | المساعدةُ تُسمّى **`makeVoidVal`** كما في وحدات الخرائط الخمس — لا `makeNull`: اسمُ العدمِ على قيمةِ فراغٍ يوهم أنّ `== لاشيء` يفحص الغياب، والحقّ أنّ `فراغ == لاشيء` ⇒ «خطأ» (قِيست الكذبةُ في وحدة الشبكة اللامركزية وأُصلحت ٢٠٢٦-٠٨-٢١) |
| P7-ج | **العدمُ الصريح** (عقدٌ يَعِد بقيمة `لاشيء` تُقارَن بها) يُبنى بـ`Data::Value::makeNull()` وحدَها — لا بالمُنشئ الافتراضي |
| P7-د | فحصُ الغيابِ الصادقُ في لغة ص صِدقيّةُ القيمةِ أو `نوع()` — البذرة الحارسة: `sections/02_الأنواع_المدمجة/05_لاشيء_والفراغ/100_lookup_miss_is_void_not_null.ص` |
| P7-هـ | **استثناء الجلب المصنَّف** (RFC عقد الغياب — المرحلة ب): `خريطة_اجلب_نص/رقم/منطقي` تُرجِع عند الغياب **عدمًا** (`makeNull`) لا فراغًا — عقدُها المعلَنُ اختياريٌّ يراه «؟؟»؛ والحضورُ بنوعٍ مغايرٍ أو بعدمٍ مخزَّنٍ خطأُ تشغيلٍ صريح `RUN074` كي لا تكون للعدمِ قناتان مُنتِجتان. P7-أ باقٍ عقدَ الجلبِ **الديناميكي** (`خريطة_احصل`) وحده — البذرة الحارسة: `101_typed_fetch_absence_is_null.ص` |

---

## 8. قواعد الأمان

### S1: تنفيذ أوامر النظام

```cpp
    // ❌ ممنوع بشكل مطلق — command injection
    system(args[0]->toString().c_str());

    // ✅ صحيح — مع تنظيف المدخلات
    std::string cmd = args[0]->toString();
    // فحص الأحرف الخطيرة
    static const std::string forbidden = ";|&`$(){}[]<>!";
    for (char c : forbidden) {
        if (cmd.find(c) != std::string::npos) {
            throw std::runtime_error("أمر خطير: يحتوي أحرف محظورة / unsafe command: forbidden characters");
        }
    }
```

### S2: مسارات الملفات

```cpp
    // ❌ ممنوع — path traversal
    std::ifstream file(args[0]->toString());

    // ✅ صحيح — مع فحص
    std::string path = args[0]->toString();
    if (path.find("..") != std::string::npos) {
        throw std::runtime_error("مسار غير آمن / unsafe path traversal");
    }
    // فحص إضافي: لا مسارات مطلقة غير متوقعة
    if (path.length() > 0 && (path[0] == '/' || (path.length() > 1 && path[1] == ':'))) {
        throw std::runtime_error("مسار مطلق غير مسموح / absolute path not allowed");
    }
```

### S3: إدارة الذاكرة

```cpp
    // ❌ ممنوع
    void* p = malloc(size);
    char* buf = new char[size];

    // ✅ صحيح
    auto ptr = std::make_shared<Data::Value>(...);
    std::vector<char> buf(size);
    auto unique = std::make_unique<SomeType>(...);
```

### S4: حدود المصفوفات

```cpp
    // ❌ ممنوع
    auto& elem = arr[index];  // بدون فحص

    // ✅ صحيح
    if (index < 0 || index >= static_cast<int64_t>(arr.size())) {
        throw std::runtime_error("فهرس خارج الحدود / index out of bounds");
    }
    auto& elem = arr[index];
```

### S5: تجنب تسريب المعلومات

```cpp
    // ❌ ممنوع — كشف تفاصيل النظام في رسائل الخطأ
    throw std::runtime_error("Error: file not found at /home/user/secret/file.txt");

    // ✅ صحيح
    throw std::runtime_error("الملف غير موجود / file not found");
```

### S6: حدود الحجم

```cpp
    // فحص حجم المدخلات لمنع هجمات DoS
    if (str.length() > 10'000'000) {  // 10 MB
        throw std::runtime_error("النص كبير جداً / input too large");
    }
```

---

## 9. قواعد الأداء

| القاعدة | التفصيل |
|---------|---------|
| PERF1 | مرّر `const std::string&` وليس `std::string` بالقيمة |
| PERF2 | استخدم `std::string_view` للمقارنات بدون نسخ |
| PERF3 | لا تنسخ `shared_ptr` بدون سبب — استخدم `const auto&` |
| PERF4 | تجنب إنشاء `std::map` أو `std::unordered_map` داخل lambda — استخدم `static` |
| PERF5 | استخدم `reserve()` للمصفوفات إذا كان الحجم معروفاً |
| PERF6 | لا تحول القيمة بـ `toString()` ثم ترجعها كـ `Value(string)` — ارجعها مباشرة |

```cpp
    // ❌ بطيء — نسخة غير ضرورية
    auto val = args[0];  // نسخة shared_ptr

    // ✅ أسرع — بالمرجع
    const auto& val = args[0];
```

---

## 10. قواعد التزامن والخيوط

### CONC1: لا متغيرات عامة بدون حماية

```cpp
    // ❌ خطير — race condition
    static int counter = 0;
    counter++;

    // ✅ آمن — ذري
    static std::atomic<int64_t> counter{0};
    counter.fetch_add(1);

    // ✅ آمن — قفل
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    counter++;
```

### CONC2: لا deadlock

```cpp
    // ❌ خطير — قفلان بترتيب مختلف
    std::lock_guard<std::mutex> lock1(mutex_a);
    std::lock_guard<std::mutex> lock2(mutex_b);

    // ✅ آمن — std::lock
    std::lock(mutex_a, mutex_b);
    std::lock_guard<std::mutex> lock1(mutex_a, std::adopt_lock);
    std::lock_guard<std::mutex> lock2(mutex_b, std::adopt_lock);
```

### CONC3: القنوات والمستقبلات

```cpp
    // ✅ القناة آمنة ذاتياً (SadChannel يستخدم mutex داخلي)
    // لا تقفل mutex خارجي عند التعامل مع القنوات
```

---

## 11. ملف builtin_registry.cpp

### البنية الإلزامية:

```
┌─────────────────────────────────────────┐
│ 1. الرأس (@file, @brief)              │
│ 2. Includes                             │
│ 3. Extern declarations                  │
│ 4. registerAllBuiltins()               │
│ 5. loadModule()                         │
│ 6. suggestModuleForFunction()          │
│ 7. Free function wrapper                │
└─────────────────────────────────────────┘
```

### قواعد خاصة:

| القاعدة | التفصيل |
|---------|---------|
| REG1 | كل ملف `builtin_*.cpp` له `extern void registerBuiltins*()` واحد |
| REG2 | `suggestModuleForFunction` يُولَّد بسكريبت Python — **لا تحرره يدوياً** |
| REG3 | الأسماء في خريطة الاقتراحات بـ UTF-8 (ليس `\xNN` escape) |
| REG4 | الأقسام في الخريطة مفصولة بـ `// ─── الوحدة ───` |
| REG5 | `loadModule()` يجب أن يغطي كل أسماء الوحدات ومرادفاتها |

---

## 12. ملف builtin_common.h

### المحتويات الثابتة:

```cpp
#pragma once

// ═══════════════════════════════════════════════════════════════
// (AR) Headers المشتركة بين جميع ملفات الدوال المضمنة
// (EN) Common headers shared by all builtin module files
// ═══════════════════════════════════════════════════════════════

// Project Headers (11)
#include "builtins.h"
#include "interpreter_core.h"
#include "io/io_functions.h"
#include "type_functions.h"
#include "array_functions.h"
#include "other_functions.h"
#include "string/string_functions.h"
#include "math/math_functions.h"
#include "math/advanced_math.h"
#include "system/system_functions.h"
#include "filesystem/filesystem_module.h"

// Standard Headers (3)
#include <memory>
#include <string>
#include <vector>
```

### قواعد التحديث:

| القاعدة | التفصيل |
|---------|---------|
| COM1 | لا تُضف header إلا إذا كان مستخدماً في **4+ ملفات** |
| COM2 | لا تُضف headers خاصة بالمنصة (`#ifdef`) هنا |
| COM3 | لا تُضف headers ثقيلة (مثل `<regex>` أو `<thread>`) — أبقها في الملفات المحتاجة |
| COM4 | أي حذف من builtin_common.h يتطلب التأكد من **جميع** الملفات المستخدمة |

---

## 13. قائمة الانتهاكات الشائعة

| # | الانتهاك | الخطورة | المثال | الحل |
|---|----------|---------|--------|------|
| V01 | `@file` لا يطابق اسم الملف | 🔴 عالية | `@file part1.cpp` في `strings.cpp` | صحح `@file` |
| V02 | أسماء عربية مكررة لنفس الدالة | 🔴 عالية | `"اطبع"` و `"إطبع"` | أبقِ اسماً واحداً |
| V03 | include غير مستخدم | 🟡 متوسطة | `#include <bitset>` بلا استخدام | احذفه |
| V04 | رسالة خطأ إنجليزية فقط | 🟡 متوسطة | `"requires string"` | أضف عربي |
| V05 | lambda بدون تعليق | 🟡 متوسطة | `auto fn = [...]` بدون شرح | أضف تعليق |
| V06 | تسمية غير وصفية | 🟡 متوسطة | `auto fn1`, `auto f2` | اسم وصفي |
| V07 | `[=]` أو `[&]` التقاط شامل | 🔴 عالية | `auto f = [&](...)` | حدد ما يُلتقط |
| V08 | خلط أنماط الفواصل | 🟢 منخفضة | `===` و `═══` | وحّد إلى `═══` |
| V09 | وصول لمصفوفة بدون فحص | 🔴 عالية | `args[0]` بدون `.empty()` | أضف فحص |
| V10 | `system()` بدون تنظيف | 🔴 حرجة | `system(input.c_str())` | نظّف المدخلات |
| V11 | متغير عام بدون mutex/atomic | 🔴 عالية | `static int count;` | استخدم `atomic` |
| V12 | ملف يتجاوز 2500 سطر | 🟡 متوسطة | `maps.cpp` = 2201 سطر | قسّم الملف |
| V13 | `malloc`/`new` يدوي | 🔴 عالية | `void* p = malloc(n)` | `make_shared` |
| V14 | `args[i]` بفهرس ثابت بدون فحص حجم | 🔴 عالية | `args[2]` مباشرة | فحص `size()` |
| V15 | إنشاء helper كبير داخل lambda | 🟡 متوسطة | 100+ سطر في lambda | أنقله لدالة |
| V16 | includes غير مرتبة أبجدياً | 🟢 منخفضة | `<map>` قبل `<chrono>` | رتّب |

---

## 14. إحصائيات الملفات (مُحدَّثة)

### ملفات الوحدات:

| الملف | الأسطر | Inc | Lambda | Reg | Throw | الحالة |
|-------|--------|-----|--------|-----|-------|--------|
| `builtin_core_io.cpp` | 283 | 10 | 15 | 38 | 0 | ✅ |
| `builtin_module_strings.cpp` | 624 | 6 | 51 | 133 | 0 | ✅ |
| `builtin_module_basics.cpp` | 302 | 5 | 18 | 41 | 19 | ✅ |
| `builtin_module_math.cpp` | 572 | 8 | 49 | 108 | 19 | ✅ |
| `builtin_module_assertions.cpp` | 1164 | 13 | 43 | 93 | 60 | ✅ |
| `builtin_module_async.cpp` | 537 | 14 | 27 | 67 | 14 | ✅ |
| `builtin_module_maps.cpp` | 2201 | 20 | 101 | 205 | 110 | ⚠️ كبير |
| `builtin_module_sockets.cpp` | 657 | 19 | 18 | 37 | 0 | ✅ |
| `builtin_module_platform.cpp` | 81 | 5 | 0 | 4 | 0 | ✅ |
| `builtin_module_exceptions.cpp` | 165 | 5 | 10 | 20 | 1 | ✅ |
| `builtin_module_ffi.cpp` | 329 | 7 | 15 | 30 | 0 | ✅ |
| `builtin_registry.cpp` | 1195 | 8 | 0 | 1 | 0 | ✅ |

### ملفات النواة (kernel):

| الملف | الأسطر | Inc | Lambda | Reg | Throw |
|-------|--------|-----|--------|-----|-------|
| `builtin_kernel_cpu.cpp` | 977 | 28 | 81 | 178 | 41 |
| `builtin_kernel_usb.cpp` | 630 | 7 | 34 | 68 | 0 |
| `builtin_kernel_acpi.cpp` | 600 | 8 | 34 | 67 | 1 |
| `builtin_kernel_uefi.cpp` | 576 | 9 | 37 | 74 | 0 |
| `builtin_kernel_gpu.cpp` | 502 | 7 | 25 | 50 | 0 |
| `builtin_kernel_storage.cpp` | 384 | 6 | 30 | 60 | 0 |
| `builtin_kernel_serial.cpp` | 383 | 6 | 30 | 60 | 0 |
| `builtin_kernel_network.cpp` | 373 | 6 | 30 | 60 | 0 |
| `builtin_kernel_timers.cpp` | 364 | 6 | 30 | 60 | 0 |
| `builtin_kernel_audio.cpp` | 363 | 6 | 30 | 60 | 0 |
| `builtin_kernel_threads.cpp` | 335 | 10 | 20 | 37 | 0 |
| `builtin_kernel_memory.cpp` | 330 | 6 | 20 | 40 | 0 |

### الإجماليات:

| المقياس | القيمة |
|---------|--------|
| إجمالي الملفات | 24 ملف |
| إجمالي الأسطر | ~12,593 سطر |
| إجمالي Lambda | ~633 دالة |
| إجمالي Registrations | ~1,474 تسجيل |
| إجمالي Throw | ~264 رسالة خطأ |
| تغطية التعليقات | 100% (لكل lambda) |
| رسائل إنجليزية فقط | 0 |

---

## 15. إضافة دالة مضمنة جديدة (خطوات مفصلة)

### الخطوة 1: تحديد الوحدة
اختر الملف المناسب من جدول القسم 14. إذا لم تناسب أي وحدة، أنشئ وحدة جديدة (القسم 16).

### الخطوة 2: إضافة التعليق

```cpp
    // (AR) اسم_عربي — وصف الدالة بالعربية
    // (EN) english_name — function description in English
```

### الخطوة 3: كتابة Lambda

```cpp
    auto my_func = [](const std::vector<std::shared_ptr<Data::Value>>& args)
        -> std::shared_ptr<Data::Value> {
        // فحص المعاملات
        if (args.empty()) {
            throw std::runtime_error("دالتي: يتطلب وسيطاً / my_func requires argument");
        }
        // المنطق
        auto result = /* ... */;
        return std::make_shared<Data::Value>(result);
    };
```

### الخطوة 4: التسجيل

```cpp
    interpreter.getFunctionManager().registerBuiltinFunction("اسم_عربي", my_func);
    interpreter.getFunctionManager().registerBuiltinFunction("english_name", my_func);
```

### الخطوة 5: تحديث السجل

```powershell
# حذف obj القديم
Remove-Item build\sad_core.dir\Debug\builtin_module_*.obj

# إعادة توليد suggestModuleForFunction
python scripts/gen_suggest.py

# بناء
cmake --build build --config Debug --target sad-run

# اختبار
.\build\bin\Debug\sad.exe _t1.ص
```

### الخطوة 6: اكتب اختبار

```sad
# _test_my_func.ص
استورد اسم_الوحدة
متغير ن = دالتي(وسيط)
اطبع_سطر(ن)
```

---

## 16. إضافة وحدة جديدة (خطوات مفصلة)

### الخطوة 1: إنشاء الملف

```
الاسم: builtin_module_<name>.cpp
المسار: interpreter_new/src/builtins/
```

### الخطوة 2: كتابة الهيكل الكامل

```cpp
/**
 * @file builtin_module_newmodule.cpp
 * @brief (AR) وحدة الاسم — وصف مختصر
 * @brief (EN) NewModule — short description
 *
 * @details
 * (AR) الأقسام:
 *   1. القسم الأول
 *
 * @note يتطلب: builtin_common.h
 * @see builtin_registry.cpp
 * @see BUILTIN_CODING_STANDARDS.md
 */

#include "builtin_common.h"

namespace Sad {
namespace Interpreter {

using namespace StdLib;

void registerBuiltinsNewModule(Interpreter& interpreter) {
    // ═══════════════════════════════════════════════════════════════
    // (AR) القسم 1: الوصف / (EN) Section 1: Description
    // ═══════════════════════════════════════════════════════════════

    // (AR) دالتي — وصف
    // (EN) my_func — description
    auto my_func = [](const auto& args) -> std::shared_ptr<Data::Value> {
        // ...
    };
    interpreter.getFunctionManager().registerBuiltinFunction("دالتي", my_func);
    interpreter.getFunctionManager().registerBuiltinFunction("my_func", my_func);
}

} // namespace Interpreter
} // namespace Sad
```

### الخطوة 3: التسجيل في cmake/sources.cmake

```cmake
set(INTERPRETER_BUILTINS_SOURCES
    # ...
    ${INTERPRETER_BUILTINS_DIR}/builtin_module_newmodule.cpp
)
```

### الخطوة 4: التسجيل في builtin_registry.cpp

```cpp
// في أعلى الملف
extern void registerBuiltinsNewModule(Interpreter& interpreter);

// في registerAllBuiltins()
registerBuiltinsNewModule(interpreter);

// في loadModule()
if (name == "اسم_الوحدة" || name == "module_name") {
    registerBuiltinsNewModule(interpreter);
    return true;
}
```

### الخطوة 5: تحديث suggestModuleForFunction

```powershell
python scripts/gen_suggest.py
```

---

## 17. قائمة المراجعة قبل الدمج (Merge Checklist)

استخدم هذه القائمة قبل دمج أي تعديل على ملفات الدوال المضمنة:

### الرأس والبنية:
- [ ] `@file` يطابق اسم الملف الفعلي
- [ ] `@brief` ثنائي اللغة (AR/EN)
- [ ] `@details` يسرد الأقسام
- [ ] Includes مرتبة ولا تتجاوز 12

### الدوال:
- [ ] كل lambda لها تعليق `(AR)/(EN)`
- [ ] كل lambda لها اسم وصفي (`*_func` أو `*_fn`)
- [ ] كل دالة مسجلة باسمين (عربي + إنجليزي) — لا أكثر
- [ ] لا أسماء عربية مكررة
- [ ] لا تسجيل مكرر لنفس الاسم

### المعاملات والأخطاء:
- [ ] فحص عدد المعاملات قبل الوصول
- [ ] فحص الأنواع قبل الاستخدام
- [ ] رسائل الأخطاء غير إنجليزية فقط
- [ ] لا `assert()` — فقط `throw`

### الأمان:
- [ ] لا `system()` بدون تنظيف
- [ ] لا وصول لملفات بدون فحص المسار
- [ ] لا `malloc`/`new` يدوي
- [ ] لا وصول لمصفوفة بدون فحص الحدود

### الأداء والتزامن:
- [ ] لا نسخ `shared_ptr` بدون سبب
- [ ] لا `[=]` أو `[&]` شامل
- [ ] متغيرات `static` محمية بـ `mutex`/`atomic`

### البناء والاختبار:
- [ ] `cmake --build build --config Debug --target sad-run` نجح
- [ ] جميع `_t*.ص` نجحت (11/11)
- [ ] `suggestModuleForFunction` محدّث

---

## 18. أنماط مرفوضة ومقبولة

### النمط 1: تسجيل مجمّع vs فردي

```cpp
    // ❌ مرفوض — تسجيل بعيد عن التعريف
    auto f1 = [](...) { ... };
    auto f2 = [](...) { ... };
    auto f3 = [](...) { ... };
    // ... 50 سطر ...
    interpreter.getFunctionManager().registerBuiltinFunction("ا", f1);
    interpreter.getFunctionManager().registerBuiltinFunction("ب", f2);
    interpreter.getFunctionManager().registerBuiltinFunction("ج", f3);

    // ✅ مقبول — تسجيل مباشرة بعد التعريف
    auto f1 = [](...) { ... };
    interpreter.getFunctionManager().registerBuiltinFunction("ا", f1);
    interpreter.getFunctionManager().registerBuiltinFunction("a", f1);

    auto f2 = [](...) { ... };
    interpreter.getFunctionManager().registerBuiltinFunction("ب", f2);
    interpreter.getFunctionManager().registerBuiltinFunction("b", f2);
```

### النمط 2: دوال مساعدة

```cpp
    // ❌ مرفوض — دالة مساعدة كبيرة داخل lambda
    auto parse_func = [&](...) {
        // 150 سطر من المنطق
    };

    // ✅ مقبول — استخراج المنطق لدالة منفصلة
    static std::shared_ptr<Data::Value> parseImpl(const std::string& input) {
        // 150 سطر
    }
    auto parse_func = [](const auto& args) {
        return parseImpl(args[0]->toString());
    };
```

### النمط 3: قيم افتراضية

```cpp
    // ✅ مقبول — قيمة افتراضية آمنة لمعامل اختياري
    auto split_func = [](const auto& args) {
        if (args.empty()) throw std::runtime_error("...");
        std::string delimiter = args.size() >= 2 ? args[1]->toString() : " ";
        // ...
    };

    // ❌ مرفوض — تجاهل المعامل الإلزامي بصمت
    auto split_func = [](const auto& args) {
        std::string text = args.size() > 0 ? args[0]->toString() : "";  // لا!
        // النص الفارغ ليس "قيمة افتراضية" — هو خطأ
    };
```

### النمط 4: إرجاع القيم

```cpp
    // ✅ مقبول — قيمة فارغة (VOID/NIL)
    return std::make_shared<Data::Value>();

    // ✅ مقبول — قيمة محددة
    return std::make_shared<Data::Value>(result);

    // ❌ مرفوض بشكل مطلق — إرجاع nullptr (يسبب segfault)
    return nullptr;
```

### النمط 5: المقارنة بين الأنواع

```cpp
    // ✅ مقبول — مقارنة enum صريحة
    if (args[0]->getType() != Data::ValueType::STRING) { ... }

    // ❌ مرفوض — مقارنة بالنص
    if (args[0]->getTypeAsString() != "string") { ... }  // بطيء وهش
```

### النمط 6: معالجة الأخطاء

```cpp
    // ✅ مقبول — throw مع رسالة واضحة
    throw std::runtime_error("دالتي: المعامل الأول يجب أن يكون رقم / my_func: first argument must be number");

    // ❌ مرفوض — throw بدون رسالة
    throw std::runtime_error("");

    // ❌ مرفوض — إرجاع قيمة خطأ صامتة
    return std::make_shared<Data::Value>(-1);  // الخطأ يجب أن يكون استثناء، لا قيمة سحرية
```

---

> **ملاحظة أخيرة:** هذه القواعد تُطبَّق على الكود المكتوب يدوياً. الكود المُولَّد بسكريبتات (مثل `suggestModuleForFunction`) له معايير مختلفة ويُوثَّق بتعليق `// AUTO-GENERATED — DO NOT EDIT MANUALLY`.
