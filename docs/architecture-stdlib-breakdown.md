# تفكيك مجلد `stdlib/` — المكتبة القياسية للغة ص

> **تاريخ التحديث:** 29 أبريل 2026
> **الغرض:** تحديد كل وحدة في `stdlib/`، طريقة بنائها، أي مسار تنفيذ يخدمها، وكيف تُستخدم.

---

## 🏗️ بنية stdlib المختلطة

`stdlib/` ليس مجلداً موحداً — بل **مزيج من 3 أنواع مختلفة من المكتبات**:

| النوع | الوصف | الأمثلة |
|---|---|---|
| **Builtins (C++)** | تُجمَّع داخل `sad_core` مباشرة كـ STDLIB_SOURCES | `string/`, `math/`, `io/`, `filesystem/`, `database/`, `json/`, `xml/`, `crypto/`, `system/` |
| **مكتبات منفصلة (C++)** | `add_library()` خاص + ربط بـ `sad_core` لاحقاً | `network/` → `sad_network`, `sad_http`, `sad_websocket` |
| **وحدات `.ص` خالصة** | كود لغة ص يُستورد عبر `استورد` | `جسون/`, `إضافات/`, `*.ص` في الجذر |
| **C++ منخفض المستوى** | LOW_LEVEL_SOURCES — تُجمَّع في `sad_core` بشرط | `low_level/`, `freestanding/` |

---

## 📊 الجدول الكامل لوحدات stdlib

### 1️⃣ Builtins المُجمَّعة في `sad_core` (STDLIB_SOURCES)

> مصدر: [cmake/sources.cmake#L210-L226](cmake/sources.cmake#L210)

| الوحدة | الملف C++ | الدوال المُسجَّلة | يخدم |
|---|---|---|---|
| `string/` | `stdlib/string/string_functions.cpp` | تقسيم، استبدال، تحويل حالة، إلخ | المسار 1 + 4 |
| `math/` | `stdlib/math/math_functions.cpp` + `advanced_math.cpp` | sin, cos, log, sqrt, π, ... | المسار 1 + 4 |
| `io/` | `stdlib/io/io_functions.cpp` | اطبع، اقرأ، print_line | جميع المسارات |
| `filesystem/` | `stdlib/filesystem/filesystem_module.cpp` | قراءة/كتابة ملفات، تصفّح | المسار 1 + 3 (لا 4) |
| `database/` | `stdlib/database/database_module.cpp` | SQLite bindings | المسار 1 (لا 4) |
| `json/` | `stdlib/json/json_module.cpp` | JSON parse/serialize | المسار 1 + 3 |
| `xml/` | `stdlib/xml/xml_module.cpp` | XML parsing | المسار 1 + 3 |
| `crypto/` | `stdlib/crypto/crypto_module.cpp` | SHA-256, AES, ... | المسار 1 + 3 |
| `system/` | `stdlib/system/system_functions.cpp` | OS info, env vars | المسار 1 |

> ⚠️ **`crypto_builtins.cpp` معطّل في البناء** — يستخدم نظام include مختلف (تعليق في sources.cmake:223).

### 2️⃣ Builtins للنواة المشتركة `shared/builtins/runtime/`

> ✅ **هذه ليست stdlib بالمعنى التقليدي** — بل runtime أساسي يُحمَّل تلقائياً.

| الملف | الغرض |
|---|---|
| `stdlib_manager.cpp` | مسجّل الدوال + lazy loading |
| `type_functions.cpp` | `رقم()`, `نص()`, `عشري()`, `منطقي()`, `طول()`, `نوع()` |
| `array_functions.cpp` | طرق المصفوفات (`اضف`, `احذف`, `رتب`, ...) |
| `other_functions.cpp` | باقي الدوال المضمنة |

**لا تحتاج استيراد** — متاحة دائماً.

### 3️⃣ مكتبات الشبكات (مكتبات منفصلة)

> مصدر: [cmake/network.cmake](cmake/network.cmake)

| المكتبة | المصادر | يربط بـ | يخدم |
|---|---|---|---|
| `sad_network` | `network/src/{network,socket,tcp,udp}/*.cpp` | `ws2_32` (Windows) | sad.exe |
| `sad_http` | `network/src/http/*.cpp` (8 ملفات) | `sad_network` | sad.exe |
| `sad_websocket` | `network/src/websocket/*.cpp` | `sad_network`, `ws2_32` | sad.exe |

**ربط تلقائي بـ `sad_core`:** `target_link_libraries(sad_core PUBLIC sad_network sad_http sad_websocket)` يحدث بعد التحقق من وجود الأهداف.

**تعريفات شرطية:** `HAS_NETWORK_LIB` → يُفعَّل في `sad_core` تلقائياً.

### 4️⃣ المكتبات منخفضة المستوى (LOW_LEVEL_SOURCES)

> مصدر: [cmake/sources.cmake#L240-L268](cmake/sources.cmake#L240)

| الفئة | الملفات | الغرض |
|---|---|---|
| **CPU/Memory** | `pointer_type`, `bitwise_ops`, `cpu`, `paging`, `gdt`, `memory_map` | إدارة المعالج والذاكرة |
| **مقاطعات** | `interrupts`, `apic`, `hpet`, `timer` | المقاطعات والمؤقتات |
| **I/O** | `io_ports`, `serial`, `framebuffer`, `audio` | إدخال/إخراج هاردوير |
| **حافلات** | `pci`, `dma`, `usb`, `nvme` | حافلات ومتحكمات |
| **نظام** | `boot`, `uefi`, `acpi`, `vfs`, `syscall`, `scheduler`, `sync`, `network_stack` | بنية النواة |

**يخدم:** `sadc --freestanding` (المسار 3 فقط — تطوير نواة OS)

### 5️⃣ وحدات لغة ص الخالصة

#### في `stdlib/freestanding/`

| الملف | الغرض |
|---|---|
| `نواة_منخفضة.ص` | تصدير 50+ FFI binding للدوال C++ منخفضة المستوى (`sad_ll_*`) |

> هذا هو **الجسر الرسمي** بين كود `.ص` ومكتبات `low_level/` المُجمَّعة في `sad_core`/freestanding mode.

#### في جذر `stdlib/`

| الملف | الغرض | يخدم |
|---|---|---|
| `خرائط.ص` | عمليات خرائط متقدمة | جميع المسارات |
| `رسومات.ص` | API رسومات SDL2-like | sad.exe (المسار 1) |
| `رياضيات.ص` | wrapper لدوال رياضية | جميع المسارات |
| `شبكات.ص` | wrapper لدوال الشبكة | sad.exe |
| `مصفوفات.ص` | عمليات مصفوفات إضافية | جميع المسارات |
| `ملفات.ص` | wrapper لـ filesystem | المسار 1 + 3 |
| `نصوص.ص` | عمليات نصوص متقدمة | جميع المسارات |
| `وقت.ص` | تواريخ ومؤقتات | جميع المسارات |

#### في مجلدات فرعية

| المجلد | المحتوى | يخدم |
|---|---|---|
| `جسون/` | wrapper `.ص` لـ `json_module.cpp` | المسار 1 |
| `نص/` | عمليات نصوص بلغة ص | جميع المسارات |
| `إضافات/` | extensions مفيدة | جميع المسارات |
| `ويب/` | عمليات HTTP بلغة ص | sad.exe |

### 6️⃣ Async (مختلط)

> مجلد `stdlib/async/src/` — مكتوب بلغة ص (`runtime.ص`, `channel.ص`, `async_pipeline.ص`)

| الملف | الغرض |
|---|---|
| `runtime.ص` | محرّك runtime للـ async/await + tasks |
| `channel.ص` | قنوات + bcast |
| `async_pipeline.ص` | تدفقات غير متزامنة |

**حالة:** قيد التطوير — يستخدم ميزات `*ثابت خام`, `*خام` (raw pointers) — يدعمه `sadc --type-check` بشكل كامل، ودعم جزئي في `sad`.

---

## 🔍 خارطة "ماذا يحتاج استيراد؟"

> **القاعدة الذهبية:** كل ما في `shared/builtins/runtime/` لا يحتاج استيراد. كل شيء آخر يحتاج `استورد`.

### بدون استيراد (مدمج تلقائياً)

```sad
اطبع("مرحبا")        # من io_functions.cpp
متغير ع = طول([1,2])  # من type_functions.cpp
[1,2,3].اضف(4)       # من array_functions.cpp
```

### يحتاج استيراد

```sad
استورد رياضيات       # → math_functions.cpp
استورد نصوص          # → string_functions.cpp (طرق متقدمة)
استورد ملفات         # → filesystem_module.cpp
استورد جسون          # → json_module.cpp
استورد قاعدة_بيانات  # → database_module.cpp
استورد تشفير         # → crypto_module.cpp
استورد شبكة         # → sad_network bindings
استورد http          # → sad_http bindings
```

> 📌 **جدول كامل:** راجع `.github/skills/sad-builtins/SKILL.md`

---

## 🚨 خارطة الدعم لكل مسار

| الوحدة | المسار 1 (sad) | المسار 2 (--vm) | المسار 3 (sadc) | المسار 4 (wasm) |
|---|---|---|---|---|
| `shared/builtins/runtime` | ✅ | ✅ | ✅ | ✅ |
| `string`, `math`, `io` | ✅ | ✅ | ✅ | ✅ |
| `filesystem` | ✅ | ⚠️ جزئي | ✅ | ❌ |
| `database` | ✅ | ❌ | ✅ | ❌ |
| `json`, `xml` | ✅ | ⚠️ جزئي | ✅ | ⚠️ |
| `crypto` | ✅ | ❌ | ✅ | ❌ |
| `system` | ✅ | ⚠️ | ✅ | ❌ |
| `network/*` (sad_network, http, websocket) | ✅ | ❌ | ⚠️ يحتاج ربط يدوي | ❌ |
| `low_level/*` | ❌ | ❌ | ✅ (`--freestanding`) | ❌ |
| `freestanding/نواة_منخفضة.ص` | ❌ | ❌ | ✅ (`--freestanding`) | ❌ |
| `async/*.ص` | ⚠️ تطوير | ❌ | ⚠️ تطوير | ❌ |

---

## 🔬 اكتشافات معمارية مهمة

### الاكتشاف 1: تشتت تعريف stdlib

stdlib مُعرَّف في **3 مواقع مختلفة**:
- `cmake/sources.cmake` (STDLIB_SOURCES + LOW_LEVEL_SOURCES) — تُدمج في `sad_core`
- `cmake/network.cmake` (NETWORK_SOURCES + HTTP + WEBSOCKET) — مكتبات منفصلة
- `cmake/tests_comprehensive.cmake` — قائمة مطابقة لـ STDLIB_SOURCES (ازدواج!)

**التحسين المقترح:** توحيد تعريف stdlib في `cmake/stdlib.cmake` واحد.

### الاكتشاف 2: ازدواج "stdlib" بين `shared/` و `stdlib/`

- `shared/builtins/runtime/` — runtime أساسي (لا استيراد)
- `stdlib/` — وحدات اختيارية (مع استيراد)

**معمارياً صحيح**، لكن الاسم مضلِّل — `stdlib/` ليس "standard library" بالمعنى التقليدي بل "extension library".

### الاكتشاف 3: `crypto_builtins.cpp` معطَّل

تعليق في sources.cmake:223 يقول إنه "يستخدم نظام include مختلف". هذا debt تقني.

### الاكتشاف 4: الوحدات `.ص` ليست مُسجَّلة في CMake

ملفات `*.ص` في `stdlib/` (خرائط، رسومات، ...) لا تُذكر في أي CMakeLists — تُحمَّل وقت التشغيل عبر نظام `استورد` من `shared/modules/module_loader.cpp`.

**الفائدة:** لا حاجة لإعادة بناء عند تعديل وحدة `.ص`.

### الاكتشاف 5: `low_level/` يخدم مساراً واحداً فقط

كل ملفات `LOW_LEVEL_SOURCES` (24 ملف) تُجمَّع في `sad_core` لكنها **لا تُستخدم إلا في `sadc --freestanding`**. هذا hint لإعادة هيكلة:

**التحسين المقترح:** فصل `LOW_LEVEL_SOURCES` إلى مكتبة منفصلة `sad_low_level` تُربط فقط عند `--freestanding`.

### الاكتشاف 6: المسار 4 (WASM) محدود جداً

WASM يدعم فقط:
- ✅ Lexer + Parser + AST + Interpreter Core
- ✅ `builtin_functions.cpp` (الأساسيات فقط)
- ❌ كل `STDLIB_SOURCES` — غير مُجمَّع في WASM
- ❌ كل `NETWORK_SOURCES` — غير مُجمَّع في WASM

**نتيجة:** كود `.ص` يستخدم `استورد رياضيات` لن يعمل في المتصفح حالياً.

---

## 📋 ملخص تنفيذي

| الميزة | عدد الملفات تقريباً | حالة الإنتاج |
|---|---|---|
| Builtins runtime (`shared/builtins/runtime/`) | 4 | ✅ مستقر |
| stdlib core (`STDLIB_SOURCES`) | 9 وحدات | ✅ مستقر |
| stdlib low-level (`LOW_LEVEL_SOURCES`) | 24 ملف | ✅ مستقر (للـ freestanding) |
| Network (sad_network/http/websocket) | 17 ملف | ✅ مستقر |
| وحدات `.ص` | ~15 ملف | ⚠️ متفاوت |
| stdlib async | 3 ملفات `.ص` | 🔄 قيد التطوير |
| crypto_builtins | 1 ملف معطَّل | ❌ debt تقني |

---

## 🔗 المراجع

- [docs/architecture-cli-features.md](docs/architecture-cli-features.md)
- [docs/architecture-tools-breakdown.md](docs/architecture-tools-breakdown.md)
- [docs/project-overview.md](docs/project-overview.md)
- [cmake/sources.cmake](cmake/sources.cmake) — STDLIB_SOURCES + LOW_LEVEL_SOURCES
- [cmake/network.cmake](cmake/network.cmake) — sad_network + sad_http + sad_websocket
- [cmake/libraries.cmake](cmake/libraries.cmake) — sad_core ربط
- [.github/skills/sad-builtins/SKILL.md](.github/skills/sad-builtins/SKILL.md) — مرجع الدوال المضمنة
