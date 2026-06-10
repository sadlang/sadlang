# خطة دمج YAML → C++ Codegen — نظام Language Truth V5

**التاريخ:** 2026-06-06  
**الهدف:** كل الكود يستخدم ثوابت مُولَّدة من YAML — لا hardcode في أي مكان.

---

## المبدأ الأساسي

```
YAML (SoT)
  ↓ gen_*.py (وقت cmake --build)
Generated C++ Header (ثوابت constexpr)
  ↓ #include "..._generated.h"
كل كود اللغة: مفسر + مترجم + LSP + formatter + REPL
```

**القاعدة:** أي مقارنة نصية مع اسم كلمة/طريقة/دالة **يجب** أن تستخدم ثابتاً من ملف مُولَّد.

---

## النظام الحالي (ما يعمل فعلاً ✅)

### keywords — مكتمل
```
language-truth/keywords.yaml
  ↓ gen_keywords.py
shared/lexer/generated/keywords_generated.h
  ↓ #include
shared/lexer/src/lexer_keywords.cpp  → Lexer يعرف الكلمات
```

---

## الخطة الكاملة لكل نطاق

---

### 1. type_methods — طرق الأنواع (الأعلى أولوية)

**المشكلة الحالية:** كل مقارنة في `expression_evaluator_oop_*.cpp` مثل:
```cpp
if (m == "\xD8\xA3\xD8\xB6\xD9\x81" || m == "\xD8\xA7\xD8\xB6\xD9\x81")  // hardcoded أضف/اضف
```

**الحل:**

**الخطوة 1:** `gen_type_methods.py` يقرأ `language-truth/type_methods.yaml` وينتج:
```cpp
// shared/builtins/generated/type_methods_generated.h
// AUTO-GENERATED from language-truth/type_methods.yaml

#pragma once
#include <string_view>

namespace Sad::TypeMethods {

  namespace Array {
    // أضف — aliases: اضف
    constexpr std::string_view PUSH      = "أضف";
    constexpr std::string_view PUSH_ALT  = "اضف";
    // احذف_اخير — aliases: none
    constexpr std::string_view POP       = "احذف_اخير";
    // رتب
    constexpr std::string_view SORT      = "رتب";
    // ... كل 35 طريقة
  }

  namespace String {
    constexpr std::string_view LENGTH    = "طول";
    constexpr std::string_view CONTAINS  = "يحتوي";
    // ... كل 15 طريقة
  }

  namespace Map {
    constexpr std::string_view GET       = "احصل";
    constexpr std::string_view SET       = "عيّن";
    constexpr std::string_view SET_ALT   = "عين";
    // ... كل 11 طريقة
  }

  namespace Channel {
    constexpr std::string_view SEND      = "أرسل";
    constexpr std::string_view SEND_ALT  = "ارسل";
    // ... كل 11 طريقة
  }

  namespace Mutex {
    constexpr std::string_view LOCK      = "اقفل";
    constexpr std::string_view UNLOCK    = "افتح";
    // ... كل 4 طرق
  }

  namespace WaitGroup {
    constexpr std::string_view ADD       = "أضف";
    constexpr std::string_view ADD_ALT   = "اضف";
    // ... كل 4 طرق
  }

  namespace Future {
    constexpr std::string_view GET       = "احصل";
    constexpr std::string_view SET       = "عيّن";
    constexpr std::string_view SET_ALT   = "عين";
    // ... كل 3 طرق
  }

} // namespace Sad::TypeMethods
```

**الخطوة 2:** تعديل `expression_evaluator_oop_array_methods.cpp`:
```cpp
// قبل:
if (m == "\xD8\xA3\xD8\xB6\xD9\x81" || m == "\xD8\xA7\xD8\xB6\xD9\x81")

// بعد:
#include "type_methods_generated.h"
namespace TM = Sad::TypeMethods;
if (m == TM::Array::PUSH || m == TM::Array::PUSH_ALT)
```

**نتيجة:** إضافة طريقة جديدة لمصفوفة → تعديل `type_methods.yaml` فقط.

---

### 2. builtins — الدوال المدمجة (موجود جزئياً)

**الوضع:** `gen_builtins.py` يعمل + `builtin_names_generated.h` يُولَّد بالفعل  
**المشكلة:** cmake لا يزال يشير لـ `data/stdlib/core.yaml` لا `language-truth/builtins/`

**التعديل في cmake/codegen.cmake:**
```cmake
# الآن:
set(SAD_BN_YAML "${CMAKE_SOURCE_DIR}/data/stdlib/core.yaml")

# بعد التعديل:
set(SAD_BN_YAML_FILES
  "${CMAKE_SOURCE_DIR}/language-truth/builtins/core.yaml"
  "${CMAKE_SOURCE_DIR}/language-truth/builtins/types.yaml"
  "${CMAKE_SOURCE_DIR}/language-truth/builtins/concurrency.yaml"
)
```

**الكود الحالي في `builtin_registry.cpp` يستخدم:**
```cpp
namespace Bs = Builtins::Names::Strings;
interpreter.getFunctionManager().registerBuiltinFunction(
  std::string(Bs::APPEND), append_func);
// ↑ Bs::APPEND مُولَّد من builtin_names_generated.h ✅ — يعمل فعلاً
```

---

### 3. errors — رسائل الأخطاء (gen_error_messages.py موجود)

**الوضع:** `gen_error_messages.py` يعمل + `data/language/error_messages.yaml` موجود  
**الحل:** تحديث cmake لتشير إلى `language-truth/errors/*.yaml` بدل الملف الموحَّد

**الناتج:**
```cpp
// error_messages_generated.h
namespace Sad::Errors {
  namespace Lexical {
    constexpr const char* INVALID_CHAR = "LEX_INVALID_CHARACTER";
    constexpr const char* INVALID_CHAR_MSG_AR = "محرف غير صالح ...";
    constexpr const char* INVALID_CHAR_MSG_EN = "Invalid character ...";
  }
  // ...
}
```

**الاستخدام في كود الأخطاء:**
```cpp
#include "error_messages_generated.h"
// بدل:
errorManager->report("LEX_INVALID_CHARACTER", "محرف غير صالح ...");
// يصبح:
errorManager->report(Sad::Errors::Lexical::INVALID_CHAR,
                     Sad::Errors::Lexical::INVALID_CHAR_MSG_AR);
```

---

### 4. operators — العوامل (يحتاج gen جديد)

**الحل:**
```cpp
// operators_generated.h
namespace Sad::Operators {
  constexpr std::string_view AND_WORD = "و";
  constexpr std::string_view OR_WORD  = "أو";
  constexpr std::string_view NOT_WORD = "ليس";
  // ...
}
```

**الاستخدام في parser:**
```cpp
// بدل:
if (current_.getValue() == "و") { ... }
// يصبح:
if (current_.getValue() == Sad::Operators::AND_WORD) { ... }
```

---

### 5. stdlib modules — أسماء الوحدات

**الحل:**
```cpp
// stdlib_modules_generated.h
namespace Sad::StdLib {
  constexpr std::string_view STRINGS   = "نصوص";
  constexpr std::string_view MATH      = "رياضيات";
  constexpr std::string_view BASICS    = "أساسيات";
  // ...
}
```

**الاستخدام في `builtin_registry.cpp`:**
```cpp
// بدل:
{"أضف", "نصوص"},
// يصبح:
{std::string(Bs::APPEND), std::string(Sad::StdLib::STRINGS)},
```

---

## خطة التنفيذ (الترتيب المقترح)

| المرحلة | العمل | الأولوية | التأثير |
|---------|-------|---------|---------|
| **1** | ربط cmake لـ builtins من `language-truth/builtins/` | فوري (codegen موجود) | منخفض المخاطرة |
| **2** | كتابة `gen_type_methods.py` + تعديل `expression_evaluator_oop_*.cpp` | M2-001/002 | تأثير كبير — 3 ملفات C++ |
| **3** | ربط cmake لـ errors من `language-truth/errors/` | M2-003 | تحديث كود الأخطاء |
| **4** | `gen_operators.py` + تعديل `parser_expressions.cpp` | M2 | parser |
| **5** | `gen_stdlib_modules.py` + تعديل `builtin_registry.cpp` | M2 | modules |
| **6** | تحديث LSP/formatter لاستخدام `libsadlangtruth` | M2-004 | أدوات |
| **7** | حذف `data/language/` (M2-004) | آخر خطوة | تنظيف |

---

## الملفات المتأثرة عند إنجاز الخطة كاملاً

```
YAML changes:
  language-truth/*.yaml (المصدر الوحيد)

New generated files:
  shared/builtins/generated/type_methods_generated.h      ← جديد
  shared/builtins/generated/operators_generated.h         ← جديد
  shared/errors/generated/error_messages_generated.h      ← موجود، يُحدَّث
  shared/stdlib/generated/stdlib_modules_generated.h      ← جديد
  shared/lexer/generated/keywords_generated.h             ← موجود ✅

Modified C++ files:
  interpreter/src/visitors/expression_evaluator_oop_array_methods.cpp
  interpreter/src/visitors/expression_evaluator_oop_string_map_methods.cpp
  interpreter/src/visitors/expression_evaluator_oop_concurrency.cpp
  interpreter/src/builtins/builtin_registry.cpp
  shared/parser/src/core/parser_expressions.cpp
  shared/parser/src/core/parser_main.cpp
  (+ أي ملف يحتوي hardcoded Arabic string)

cmake changes:
  cmake/codegen.cmake (إضافة targets جديدة)
```

---

## التأكيد النهائي للحالة الهدف

بعد اكتمال الخطة:

```
// YAML
type_methods.yaml:
  - target: مصفوفة
    method: "أضف"
    aliases: ["اضف"]
    cpp_id: ARRAY_PUSH

// Generated
type_methods_generated.h:
  namespace Array { constexpr std::string_view PUSH = "أضف"; }

// Interpreter uses
expression_evaluator_oop_array_methods.cpp:
  if (m == TM::Array::PUSH || m == TM::Array::PUSH_ALT) { ... }

// LSP uses
lsp_completion.cpp:
  for (auto& method : TypeMethodsRegistry::getArrayMethods()) {
      completions.push_back(method.name);  // ← نفس المصدر
  }

// Formatter uses
formatter.cpp:
  if (token == TM::Array::PUSH) { ... }  // ← نفس المصدر
```

**الحد الفاصل:** YAML واحد → جميع الأنظمة تتفق. لا يمكن أن يعرف LSP طريقة لا يعرفها المفسر.

---

*هذا الملف يوضح الخطة الكاملة — يُحدَّث مع كل مرحلة تنفيذ.*
