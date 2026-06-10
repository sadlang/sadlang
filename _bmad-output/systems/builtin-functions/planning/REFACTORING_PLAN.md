# خطة إزالة النصوص العربية الـ Hardcoded من كود C++
**التاريخ:** 2026-06-06  
**الهدف:** كل اسم دالة مدمجة في C++ يُعبَّر بثابت مُولَّد من YAML — لا نصوص حرفية مباشرة.

---

## القاعدة الذهبية

```cpp
// ❌ ممنوع — hardcoded string
registerBuiltinFunction("اطبع", print_func);
if (funcName == "جذر") { ... }

// ✅ مطلوب — ثابت مُولَّد
#include "builtin_registry.h"
using namespace Sad::Builtins::Names;
registerBuiltinFunction(std::string(Core::PRINT), print_func);
if (funcName == Math::SQRT) { ... }
```

---

## الإنجاز الحالي (محدَّث 2026-06-07)

### مُحوَّلة بالكامل ✅

| الملف | عدد التحويلات |
|-------|--------------|
| `expression_evaluator_oop_array_methods.cpp` | 11+ طريقة |
| `expression_evaluator_oop_string_map_methods.cpp` | 9+ طرق |
| `expression_evaluator_oop_concurrency.cpp` | 19 طريقة |
| `builtin_core_io.cpp` | 13 دالة |
| `builtin_registry.cpp` | 338 اسم وحدة |
| `builtin_module_basics.cpp` | 18 دالة |
| `builtin_module_assertions.cpp` | 35 دالة (17 تأكيد + 18 غلاف C) |
| `builtin_module_async.cpp` | 27 دالة |
| `builtin_module_exceptions.cpp` | 10 دوال |
| `builtin_module_ffi.cpp` | 15 دالة |
| `builtin_module_maps_json_xml.cpp` | 7 أسماء |
| `builtin_module_strings.cpp` | نظيف مسبقاً (يستخدم Bs::) |
| `builtin_module_math.cpp` | نظيف مسبقاً (يستخدم Bm::) |

**جميع ملفات `builtin_module_*.cpp` خالية الآن من النصوص العربية الحرفية في التسجيل.**

### YAML namespaces مُضافة
`AsyncAdvanced`, `Exceptions`, `FFI` (+ C stdlib wrappers)

---

## المتبقي: ملفات النواة (kernel) — P2

---

## الملفات المتبقية (مُرتَّبة بالأولوية)

### المجموعة A — تسجيل الدوال (builtin_*.cpp)

هذه الملفات تستخدم `registerBuiltinFunction("نص", func)` — يجب استبداله بالثوابت.

| الملف | عدد الدوال | الأولوية | الستوري |
|-------|-----------|---------|--------|
| `interpreter/src/builtins/builtin_core_io.cpp` | 15 | P0 | S-BF-R01 |
| `interpreter/src/builtins/builtin_module_strings.cpp` | 21+ | P0 | S-BF-R02 |
| `interpreter/src/builtins/builtin_module_math.cpp` | 33+ | P0 | S-BF-R03 |
| `interpreter/src/builtins/builtin_module_basics.cpp` | 10 | P0 | S-BF-R04 |
| `interpreter/src/builtins/builtin_module_assertions.cpp` | 5 | P0 | S-BF-R05 |
| `interpreter/src/builtins/builtin_module_maps.cpp` | 83+ | P1 | S-BF-R06 |
| `interpreter/src/builtins/builtin_module_async.cpp` | 50+ | P1 | S-BF-R07 |
| `interpreter/src/builtins/builtin_module_platform.cpp` | 9 | P1 | S-BF-R08 |
| `interpreter/src/builtins/builtin_module_http.cpp` | 18+ | P1 | S-BF-R09 |
| `interpreter/src/builtins/builtin_module_sockets.cpp` | 19 | P1 | S-BF-R10 |
| `interpreter/src/builtins/builtin_module_exceptions.cpp` | ~5 | P2 | S-BF-R11 |

### المجموعة B — نواة النظام (kernel_*.cpp)

| الملف | عدد الدوال | الأولوية | الستوري |
|-------|-----------|---------|--------|
| `builtin_kernel_network.cpp` | 33 | P2 | S-BF-R12 |
| `builtin_kernel_gpu.cpp` | 25 | P2 | S-BF-R13 |
| `builtin_kernel_audio.cpp` | 25 | P2 | S-BF-R14 |
| `builtin_kernel_serial.cpp` | 30 | P2 | S-BF-R15 |
| `builtin_kernel_memory.cpp` | 20 | P2 | S-BF-R16 |
| `builtin_kernel_storage.cpp` | 6 | P2 | S-BF-R17 |
| `builtin_kernel_usb.cpp` | 10 | P2 | S-BF-R18 |

### المجموعة C — المترجم (compiler/)

| الملف | عدد الدوال | الأولوية | الستوري |
|-------|-----------|---------|--------|
| `compiler/src/frontend/builders/builtins_core.cpp` | 15+ | P0 | S-BF-R19 |
| `compiler/src/frontend/builders/builtins_*.cpp` | 100+ | P1 | S-BF-R20 |
| `compiler/src/frontend/builders/method_call_*.cpp` | 50+ | P1 | S-BF-R21 |
| `compiler/src/backend/c_abi.cpp` | 80+ | P2 | S-BF-R22 |
| `compiler/src/types/*.cpp` | 20+ | P2 | S-BF-R23 |
| `compiler/src/backend/llvm/builders/oop/*.cpp` | ~10 | P2 | S-BF-R24 |

### المجموعة D — أدوات (tools/)

| الملف | عدد الدوال | الأولوية | الستوري |
|-------|-----------|---------|--------|
| `interpreter/src/visitors/expression_evaluator_ui.cpp` | 10+ | P2 | S-BF-R25 |
| `tools/lsp/src/providers/*.cpp` | varies | P2 | S-BF-R26 |

---

## منهجية التحويل لكل ملف

### الخطوة 1: إضافة include
```cpp
// في رأس الملف
#include "builtin_registry.h"
namespace Bn = Sad::Builtins::Names;  // اختصار مريح
```

### الخطوة 2: استبدال registerBuiltinFunction
```cpp
// قبل:
interpreter.getFunctionManager().registerBuiltinFunction("اطبع", print_func);

// بعد:
interpreter.getFunctionManager().registerBuiltinFunction(std::string(Bn::Core::PRINT), print_func);
```

### الخطوة 3: استبدال المقارنات
```cpp
// قبل:
if (funcName == "جذر" || funcName == "\xD8\xAC\xD8\xB0\xD8\xB1")

// بعد:
if (funcName == Bn::Math::SQRT)
```

### الخطوة 4: التحقق من البناء
```bash
cmake --build build --config Debug --target sad
```

---

## الأثر المتوقع

- **الآن:** 500+ string literal عربي في C++
- **بعد الإنجاز:** صفر string literal — كل اسم ثابت مُولَّد من YAML
- **الفائدة:** تعديل اسم دالة = تعديل YAML + cmake يُعيد البناء

---

## القيد الصارم: اسم واحد فقط

كل استخدام يجب أن يحوي ثابتاً واحداً بلا OR:
```cpp
// ❌ مرفوض (اسمان بديلان)
if (m == Bn::Array::PUSH || m == "ادفع")

// ✅ مقبول (ثابت واحد)
if (m == Bn::Array::PUSH)
```

> **ملاحظة:** أثناء الانتقال، يُسمح باحتفاظ النص القديم مع الثابت الجديد مؤقتاً لضمان التوافق الخلفي. يُزال النص القديم في المرحلة النهائية.
