---
id: ARCH-BUILTINS-V1
title: "المعمارية التقنية لنظام توليد Builtins"
version: 1.0
status: APPROVED
date: 2026-06-06
---

# المعمارية: نظام توليد Builtins

---

## 0. خريطة المكونات

```
language-truth/builtins/*.yaml     ← SoT (مصدر الحقيقة)
  ↓ gen_builtins_registry.py (cmake custom_command)
shared/builtins/generated/
  ├── builtin_registry_generated.h   ← يحل محل builtin_registry.h
  └── module_definitions_generated.h ← يحل محل module_definitions.h
  ↓ #include (لا تغيير في باقي الكود)
shared/builtins/include/builtin_registry.h  ← يصبح:
  #include "generated/builtin_registry_generated.h"  (wrapper فقط)
```

---

## 1. بنية YAML

### 1.1 ملف واحد لكل namespace

```
language-truth/builtins/
├── core.yaml          ← Names::Core (3 دوال)
├── type_ctor.yaml     ← Names::TypeCtor (4 دوال)
├── concurrency.yaml   ← Names::Concurrency (6 دوال)
├── math.yaml          ← Names::Math (27 دالة)
├── strings.yaml       ← Names::Strings (17 دالة)
├── arrays.yaml        ← Names::Arrays (10 دوال)
├── basics.yaml        ← Names::Basics (9 دوال)
├── assertions.yaml    ← Names::Assertions (5 دوال)
├── maps.yaml          ← Names::Maps (~47 دالة)
├── sockets.yaml       ← Names::Sockets (15 دالة)
├── http_client.yaml   ← Names::HttpClient (18 دالة)
├── http_server.yaml   ← Names::HttpServer (15 دالة)
├── network_utils.yaml ← Names::NetworkUtils (12 دالة)
├── websocket_client.yaml ← Names::WebSocketClient (14 دالة)
└── _index.yaml        ← ترتيب الملفات + metadata
```

### 1.2 بنية كل إدخال في YAML

```yaml
- cpp_id: PRINT              # ثابت C++ — UPPER_SNAKE_CASE (لا تغيير)
  canonical: "اطبع"          # الاسم العربي الوحيد — NFC، لا aliases
  namespace: Core            # يطابق Names:: namespace في builtin_registry.h
  category: CORE_IO          # CORE_IO | TYPE_CONSTRUCTOR | MODULE_FUNCTION | TYPE_METHOD
  require_import: false      # false = متاح بدون استيراد
  module: NONE               # ModuleId (NONE | MATH | STRINGS | ...)
  compiler_strategy: INLINE  # INLINE | RUNTIME_CALL | LLVM_INTRINSIC
  since: "1.0.0"
  status: stable             # stable | experimental | deprecated
  description_ar: "طباعة قيمة بدون سطر جديد"
  description_en: "Print a value without newline"
```

---

## 2. gen_builtins_registry.py

### 2.1 المدخلات
```bash
python gen_builtins_registry.py \
  --yaml-dir language-truth/builtins/ \
  --index   language-truth/builtins/_index.yaml \
  --out-h   shared/builtins/generated/builtin_registry_generated.h \
  --quiet
```

### 2.2 المخرجات

الملف المُولَّد يحوي نفس بنية `builtin_registry.h` تماماً:

```cpp
// AUTO-GENERATED — DO NOT EDIT
// Source: language-truth/builtins/*.yaml
// Generator: scripts/codegen/gen_builtins_registry.py

#pragma once
#include "../builtin_categories.h"
#include "../module_definitions.h"
#include <string_view>
#include <array>

namespace Sad::Builtins::Names {

  namespace Core {
    // (AR) دوال الإدخال/الإخراج الأساسية
    // (EN) Core I/O functions — no import required
    inline constexpr std::string_view PRINT    = "اطبع";
    inline constexpr std::string_view PRINTLN  = "اطبع_سطر";
    inline constexpr std::string_view READ     = "اقرأ";
  }

  namespace TypeCtor {
    // (AR) بانيات الأنواع
    // (EN) Type constructors
    inline constexpr std::string_view TO_INT   = "رقم";
    // ...
  }

  // ... كل الـ namespaces
}

namespace Sad::Builtins {

  inline constexpr std::array<BuiltinEntry, 3> CORE_IO_BUILTINS = {{
    {Names::Core::PRINT,   BuiltinCategory::CORE_IO, ModuleId::NONE, CompilerStrategy::INLINE},
    {Names::Core::PRINTLN, BuiltinCategory::CORE_IO, ModuleId::NONE, CompilerStrategy::INLINE},
    {Names::Core::READ,    BuiltinCategory::CORE_IO, ModuleId::NONE, CompilerStrategy::INLINE},
  }};

  inline constexpr std::array<BuiltinEntry, 4> TYPE_CONSTRUCTOR_BUILTINS = {{ ... }};
  inline constexpr std::array<BuiltinEntry, 68> MODULE_FUNCTION_BUILTINS = {{ ... }};
  inline constexpr std::array<TypeMethodEntry, 32> TYPE_METHOD_BUILTINS = {{ ... }};

  // static_assertions
  static_assert(CORE_IO_BUILTINS.size() == 3, "...");
  static_assert(TYPE_CONSTRUCTOR_BUILTINS.size() == 4, "...");

  // findBuiltinByName, isAvailableWithoutImport, ...
}
```

### 2.3 التحقق (validation)
- كل `cpp_id`: UPPER_SNAKE_CASE
- كل `canonical`: NFC + لا فراغ
- لا `cpp_id` مكرر في نفس namespace
- `namespace` يطابق قيم محددة
- `category` صالح
- `module` متوافق مع `category`

---

## 3. تعديل builtin_registry.h

```cpp
// shared/builtins/include/builtin_registry.h — بعد التعديل
// يصبح wrapper فقط
#pragma once
#include "generated/builtin_registry_generated.h"
// [النص الأصلي يُحذف — يُدار من YAML الآن]
```

---

## 4. cmake/codegen.cmake

```cmake
# ─── (AR) Builtin Registry Codegen ───
set(SAD_BR_YAML_DIR  "${CMAKE_SOURCE_DIR}/language-truth/builtins"  CACHE INTERNAL "")
set(SAD_BR_INDEX     "${SAD_BR_YAML_DIR}/_index.yaml"               CACHE INTERNAL "")
set(SAD_BR_GEN_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/codegen/gen_builtins_registry.py" CACHE INTERNAL "")
set(SAD_BR_GEN_DIR   "${CMAKE_SOURCE_DIR}/shared/builtins/generated" CACHE INTERNAL "")
set(SAD_BR_GEN_H     "${SAD_BR_GEN_DIR}/builtin_registry_generated.h" CACHE INTERNAL "")

add_custom_command(
  OUTPUT  ${SAD_BR_GEN_H}
  COMMAND ${CMAKE_COMMAND} -E make_directory ${SAD_BR_GEN_DIR}
  COMMAND ${CMAKE_COMMAND} -E env PYTHONIOENCODING=utf-8
          ${Python3_EXECUTABLE} ${SAD_BR_GEN_SCRIPT}
              --yaml-dir ${SAD_BR_YAML_DIR}
              --index    ${SAD_BR_INDEX}
              --out-h    ${SAD_BR_GEN_H}
              --quiet
  DEPENDS ${SAD_BR_YAML_DIR}/*.yaml ${SAD_BR_GEN_SCRIPT}
  COMMENT "(sad) Generating builtin_registry_generated.h from YAML..."
  VERBATIM
)

add_custom_target(sad_builtin_registry_codegen
  DEPENDS ${SAD_BR_GEN_H}
)
add_dependencies(sad_shared_lib sad_builtin_registry_codegen)
```

---

## 5. التأثير على بقية الكود

| الملف | التأثير | الإجراء |
|-------|---------|---------|
| `shared/builtins/include/builtin_registry.h` | يصبح wrapper | `#include "generated/...h"` |
| `interpreter/src/builtins/builtin_registry.cpp` | لا تغيير | يستمر يستخدم `Names::Core::PRINT` |
| `interpreter/src/builtins/builtin_module_*.cpp` | لا تغيير | — |
| `compiler/src/frontend/sir_builder_module_check.cpp` | لا تغيير | — |
| `tools/lsp/...` | لا تغيير | — |

**القاعدة:** صفر تغيير في الكود خارج `builtin_registry.h` — فقط الـ header يتغير.

---

## 6. ADRs ذات الصلة

- [ADR-BUILTINS-001](../decisions/ADR-BUILTINS-001-YAML-CODEGEN.md) — اختيار YAML كـ SoT للـ builtins
