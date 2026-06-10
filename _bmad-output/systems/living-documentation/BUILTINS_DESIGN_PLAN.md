# تصميم نظام الدوال المدمجة — الخطة الكاملة

**التاريخ:** 2026-06-06  
**الهدف:** `builtin_registry.h` يُولَّد تلقائياً من YAML بدل الكتابة اليدوية

---

## الوضع الحالي (المشكلة)

```
builtin_registry.h  ← مكتوب يدوياً ✋
  ↓ #include
builtin_registry.cpp, builtin_module_*.cpp, compiler/*.cpp, lsp/*.cpp
```

**المشكلة:**
- إضافة دالة جديدة → تعديل `builtin_registry.h` يدوياً
- LSP لا يعرف الدوال إلا من نفس الـ header
- لا يوجد "مصدر حقيقة" خارجي قابل للقراءة

---

## الحل المقترح (الهدف)

```
language-truth/builtins/*.yaml  ← SoT
    ↓ gen_builtins.py (وقت cmake --build)
shared/builtins/generated/builtin_names_generated.h  ← يحل محل builtin_registry.h
    ↓ #include (بدون تغيير في بقية الكود)
builtin_registry.cpp, builtin_module_*.cpp, ...
```

---

## قاعدة: اسم عربي واحد فقط لكل دالة (لا aliases)

```yaml
# صحيح ✅
- cpp_id: PRINT
  canonical: "اطبع"
  namespace: Core

# خطأ ❌ — لا aliases
- cpp_id: PRINT
  canonical: "اطبع"
  aliases: ["print", "طبع"]  ← ممنوع
```

---

## بنية YAML الكاملة للدوال المدمجة

### التسلسل الهرمي للوحدات

```
language-truth/builtins/
├── core.yaml          ← Core (بدون استيراد): اطبع، اطبع_سطر، اقرأ، طول، نوع
├── types.yaml         ← TypeCtor (بدون استيراد): رقم، عشري، نص، منطقي
├── concurrency.yaml   ← Concurrency (بدون استيراد): قناة، انتظر_الكل، عدد_المهام، مجموعة_انتظار، قفل، مستقبل
├── math.yaml          ← Math (وحدة رياضيات)
├── strings.yaml       ← Strings (وحدة نصوص)
├── arrays.yaml        ← Arrays (عمليات مصفوفات — مدمجة)
├── basics.yaml        ← Basics (وحدة أساسيات)
├── assertions.yaml    ← Assertions (وحدة تأكيدات)
├── maps.yaml          ← Maps (وحدة خرائط) — يشمل JSON/XML/regex/تاريخ/مسارات/base64/وظيفية
├── sockets.yaml       ← Sockets (وحدة مقابس) — TCP/UDP
├── http_client.yaml   ← HttpClient (وحدة شبكة_عالية)
├── http_server.yaml   ← HttpServer (وحدة شبكة_عالية)
├── network_utils.yaml ← NetworkUtils (وحدة شبكة_عالية)
├── websocket.yaml     ← WebSocket (وحدة شبكة_عالية)
└── _index.yaml        ← فهرس آلي
```

---

## جدول الدوال الكامل (مستخرج من builtin_registry.h)

### Core — بدون استيراد (5 دوال)

| cpp_id | الاسم | الوصف |
|--------|-------|-------|
| PRINT | اطبع | طباعة بدون سطر جديد |
| PRINTLN | اطبع_سطر | طباعة مع سطر جديد |
| READ | اقرأ | قراءة من المستخدم |
| LENGTH | طول | طول مجموعة أو نص |
| TYPE | نوع | نوع القيمة كنص |

### TypeCtor — بدون استيراد (4 دوال)

| cpp_id | الاسم | الوصف |
|--------|-------|-------|
| TO_INT | رقم | تحويل لعدد صحيح |
| TO_FLOAT | عشري | تحويل لعدد عشري |
| TO_STRING | نص | تحويل لنص |
| TO_BOOL | منطقي | تحويل لمنطقي |

### Concurrency — بدون استيراد (6 دوال)

| cpp_id | الاسم | الوصف |
|--------|-------|-------|
| CHANNEL | قناة | إنشاء قناة |
| WAIT_ALL | انتظر_الكل | انتظار جميع الخيوط |
| TASK_COUNT | عدد_المهام | عدد المهام النشطة |
| WAIT_GROUP | مجموعة_انتظار | إنشاء WaitGroup |
| MUTEX | قفل | إنشاء Mutex |
| FUTURE | مستقبل | إنشاء Future |

### Math — وحدة رياضيات (27 دالة)

| cpp_id | الاسم | الوصف |
|--------|-------|-------|
| SQRT | جذر | الجذر التربيعي |
| POWER | أس | الأسّ |
| ABS | مطلق | القيمة المطلقة |
| ROUND | تقريب | تقريب |
| FLOOR | أرضية | تقريب للأسفل |
| CEIL | سقف | تقريب للأعلى |
| SQUARE | مربع | مربع العدد |
| MAX | أكبر | القيمة الأكبر |
| MIN | أصغر | القيمة الأصغر |
| SUM | جمع | مجموع |
| SIN | جيب | جيب الزاوية |
| COS | جتا | جتا الزاوية |
| TAN | ظل | ظل الزاوية |
| ASIN | معكوس_جيب | جيب معكوس |
| ACOS | معكوس_جتا | جتا معكوس |
| ATAN | معكوس_ظل | ظل معكوس |
| ATAN2 | معكوس_ظل2 | ظل2 معكوس |
| LOG | لوغ | لوغاريتم طبيعي |
| LOG2 | لوغ2 | لوغاريتم ثنائي |
| LOG10 | لوغ10 | لوغاريتم عشري |
| EXP | أسي | الدالة الأسية |
| PI | ط | الثابت π |
| E | هـ | الثابت e |
| RANDOM | عشوائي | عدد عشوائي |
| RANDOM_INT | عشوائي_صحيح | صحيح عشوائي |
| CLAMP | حصر | حصر في نطاق |
| SIGN | إشارة | إشارة العدد |

> ملاحظة: `باي` (PI_ALT) و`عشوائي_نطاق` و`عشوائي_عشري` و`استيفاء_خطي` موجودة في الكود الحالي لكن **يجب تقليصها** لاسم واحد حسب القاعدة الجديدة.

### Strings — وحدة نصوص (17 دالة)

| cpp_id | الاسم | الوصف |
|--------|-------|-------|
| STR_LENGTH | طول_نص | طول النص |
| CHAR_CODE | رمز_حرف | رمز Unicode للحرف |
| FIND | بحث | موضع نص فرعي |
| REPLACE | استبدل | استبدال نص |
| SUBSTRING | استخراج | جزء من نص |
| TRIM | قص_أطراف | حذف مسافات |
| TO_UPPER | تحويل_كبير | تحويل كبير |
| TO_LOWER | تحويل_صغير | تحويل صغير |
| SPLIT | تقسيم | تقسيم بفاصل |
| JOIN | دمج | دمج قائمة |
| STARTS_WITH | يبدأ_ب | يبدأ بـ |
| ENDS_WITH | ينتهي_ب | ينتهي بـ |
| CONTAINS | يحتوي_على | يحتوي على |
| APPEND | أضف | إضافة لمصفوفة |
| REMOVE_ELEM | أزل | إزالة من مصفوفة |
| IS_INT | هو_رقم_صحيح | فحص النوع |
| IS_STRING | هو_نص | فحص النوع |

### Arrays — عمليات مصفوفات مدمجة (10 دوال)

| cpp_id | الاسم | الوصف |
|--------|-------|-------|
| ADD | إضافة_عنصر | إضافة |
| REMOVE | إزالة_عنصر | إزالة |
| SIZE | حجم_مصفوفة | الحجم |
| INDEX_OF | فهرس_مصفوفة | الفهرس |
| ARRAY_CONTAINS | يحتوي_عنصر | يحتوي |
| REVERSE | قلب | عكس |
| SORT | رتب | ترتيب |
| FIRST | الأول | الأول |
| LAST | الأخير | الأخير |
| SLICE | شريحة | شريحة |

### Basics — وحدة أساسيات (9 دوال)

| cpp_id | الاسم | الوصف |
|--------|-------|-------|
| RANGE | مدى | نطاق أرقام |
| EXIT | خروج | إنهاء البرنامج |
| ASSERT | تأكد | تأكيد شرط |
| READ_FILE | اقرأ_ملف | قراءة ملف |
| WRITE_FILE | اكتب_ملف | كتابة ملف |
| FILE_EXISTS | هل_موجود | وجود ملف |
| IS_FILE | هل_ملف | هو ملف |
| IS_DIR | هل_مجلد | هو مجلد |
| MKDIR | أنشئ_مجلد | إنشاء مجلد |

### Assertions — وحدة تأكيدات (5 دوال)

| cpp_id | الاسم | الوصف |
|--------|-------|-------|
| ASSERT_EQ | تأكد_يساوي | يساوي |
| ASSERT_NEQ | تأكد_لا_يساوي | لا يساوي |
| ASSERT_TRUE | تأكد_صحيح | صحيح |
| ASSERT_FALSE | تأكد_خطأ | خطأ |
| HASH | هاش | هاش القيمة |

### Maps — وحدة خرائط (كبيرة — ~47 دالة)
تشمل: JSON, XML, regex, تاريخ, مسارات, base64, URL, متغيرات البيئة, خرائط, وظيفية, مصفوفات متقدمة

### Sockets — وحدة مقابس (~15 دالة)
TCP + UDP + إعدادات

### HttpClient/HttpServer/NetworkUtils/WebSocket — وحدة شبكة_عالية

---

## البنية الموحَّدة لكل إدخال في YAML

```yaml
# بنية موحَّدة — لا aliases، لا أسماء إنجليزية داخل الدالة
- cpp_id: PRINT          # ثابت C++ — UPPER_SNAKE_CASE
  canonical: "اطبع"      # الاسم العربي الوحيد — NFC
  namespace: Core        # يطابق namespace في builtin_registry.h
  category: io           # التصنيف الدلالي
  require_import: false  # هل يحتاج استيراداً؟
  module: null           # null = بدون استيراد
  since: "1.0.0"
  status: stable
  signature:
    params: ["قيمة"]
    returns: "فراغ"
    is_variadic: true
  description_ar: "طباعة قيمة على الشاشة بدون سطر جديد"
  description_en: "Print a value without newline"
```

---

## الناتج المُولَّد (builtin_names_generated.h)

```cpp
// AUTO-GENERATED from language-truth/builtins/*.yaml
// DO NOT EDIT — تعديل YAML فقط

#pragma once
#include <string_view>

namespace Sad::Builtins::Names {

  namespace Core {
    inline constexpr std::string_view PRINT     = "اطبع";
    inline constexpr std::string_view PRINTLN   = "اطبع_سطر";
    inline constexpr std::string_view READ      = "اقرأ";
    inline constexpr std::string_view LENGTH    = "طول";
    inline constexpr std::string_view TYPE      = "نوع";
  }

  namespace TypeCtor {
    inline constexpr std::string_view TO_INT    = "رقم";
    inline constexpr std::string_view TO_FLOAT  = "عشري";
    inline constexpr std::string_view TO_STRING = "نص";
    inline constexpr std::string_view TO_BOOL   = "منطقي";
  }

  // ... باقي الـ namespaces
}
```

---

## خطوات التنفيذ

### الخطوة 1: كتابة YAML الكامل (language-truth/builtins/)
- كل namespace في ملف منفصل
- كل دالة: cpp_id + canonical + namespace + category + require_import + signature + since + status

### الخطوة 2: تحديث gen_builtins.py
- قراءة جميع الملفات في `language-truth/builtins/`
- توليد `builtin_names_generated.h` بنفس بنية `builtin_registry.h` تماماً
- الـ namespace path يبقى `Sad::Builtins::Names::X`

### الخطوة 3: تحديث cmake/codegen.cmake
```cmake
set(SAD_BN_YAML_FILES
  "${CMAKE_SOURCE_DIR}/language-truth/builtins/core.yaml"
  "${CMAKE_SOURCE_DIR}/language-truth/builtins/types.yaml"
  # ... باقي الملفات
)
add_custom_command(
  OUTPUT ${SAD_BN_GEN_H}
  COMMAND ${Python3_EXECUTABLE} ${SAD_BN_GEN_SCRIPT}
          --yaml ${SAD_BN_YAML_FILES}
          --out-h ${SAD_BN_GEN_H}
          --quiet
  DEPENDS ${SAD_BN_YAML_FILES}
)
```

### الخطوة 4: استبدال builtin_registry.h
```cpp
// قبل:
#include "builtin_registry.h"  // يدوي

// بعد:
#include "builtin_names_generated.h"  // مُولَّد
```
> ملاحظة: الـ namespace path يبقى نفسه — لا تغيير في باقي الكود.

### الخطوة 5: حذف builtin_registry.h (أو تحويله لـ #include فقط)

---

## التأثير على البناء

| المكون | التأثير |
|--------|---------|
| `builtin_registry.cpp` | لا تغيير — يستخدم `Names::Core::PRINT` كما كان |
| `builtin_module_*.cpp` | لا تغيير — نفس الـ namespaces |
| `compiler/...` | لا تغيير |
| `lsp/...` | لا تغيير |
| إضافة دالة جديدة | تعديل YAML فقط → cmake يُعيد البناء تلقائياً |

---

## الإجمالي التقديري للدوال

| الوحدة | العدد التقريبي |
|--------|--------------|
| Core + TypeCtor + Concurrency | 15 |
| Math | 27 |
| Strings | 17 |
| Arrays | 10 |
| Basics | 9 |
| Assertions | 5 |
| Maps (JSON/XML/regex/تاريخ/...) | ~47 |
| Sockets | ~15 |
| HttpClient/Server/NetworkUtils/WebSocket | ~40 |
| **المجموع** | **~185** |
