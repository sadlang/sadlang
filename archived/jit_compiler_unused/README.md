# JIT Compiler Library — مُؤرشَفة (Unused)

## ملخص

أُرشِفت مكتبة `sad_jit` التي كانت في `compiler/src/jit/` و `compiler/include/jit/` في
**Fix #6**.

| المقياس | القيمة |
|---|---|
| **عدد الملفات** | 18 ملف (11 .cpp + 7 .h) |
| **إجمالي الأسطر** | 9,034 سطر |
| **الحجم الإجمالي** | ~370 KB |
| **حالة البناء قبل الأرشفة** | يُبنى ضمن `add_library(sad_jit STATIC)` |
| **الاستخدامات الخارجية** | **0** — لم يُربط بأي مفسر/مترجم/اختبار |

## السبب الجذري للأرشفة

### 1. معزول كلياً عن النظام
بحث شامل بـ `Select-String` عبر كل `*.cpp/*.h/CMakeLists.txt` (مع استثناء المجلد نفسه):
```
JITEngine, JITCache, HotPathDetector, TierTransition, jit_bridge, jit_engine
→ 0 استدعاءات خارجية
```
المكتبة كانت تُبنى وتُربط بـ `sad_compiler` كـ INTERFACE link، لكن لا أحد يستدعي أي رمز
من رموزها العامة.

### 2. غير مكتملة الربط بـ LLVM ORC
الواجهات في `jit_engine.h` تعتمد على **forward declarations فقط**:
```cpp
// ملاحظة: سنستخدم forward declarations لتجنب الاعتماد على LLVM headers
// Note: Using forward declarations to avoid dependency on LLVM headers
// حتى يكتمل تثبيت LLVM / Until LLVM installation completes
```
هذا التعليق يعود لـ ~2024. تثبيت LLVM **اكتمل** منذ زمن (`HAS_LLVM` مفعّل في كل شيء)،
لكن JIT لم يتلقَّ الربط الفعلي بـ `llvm::orc::LLJIT`.

### 3. مكرَّرة معمارياً مع `vm/sad_jit`
يوجد محرك JIT آخر **حي ويعمل** في:
- `vm/include/sad_jit.h` (17,950 byte) — أسماء عربية: `محرك_JIT`، `إعدادات_JIT`
- `vm/src/sad_jit.cpp` (41,339 byte) — يُترجم **بايت كود VM** إلى native عبر LLVM ORC

### الفرق الجوهري

| البُعد | `compiler/src/jit/` (مُؤرشف) | `vm/sad_jit` (حي) |
|---|---|---|
| الإدخال | LLVM IR / AST (افتراضياً) | بايت كود VM |
| الجمهور | **لا أحد** | VM فقط |
| ربط ORC | forward decls فقط | `#ifdef HAS_LLVM` + ORC حقيقي |
| الاختبارات | 0 | داخل `vm/CMakeLists.txt` |
| التصميم | 4 modes + tier transition + deopt + bridge — طموح جداً | `سجّل_دالة → نفّذ_دالة` — عملي |

## التحقق قبل الأرشفة

```powershell
# 1. لا أحد يستورد من مجلد jit/
Select-String -Path *.cpp,*.h -Pattern '#include.*"jit/' | Where-Object { $_.Path -notmatch '\\jit\\' }
# النتيجة: 0

# 2. لا أحد يستدعي الرموز
Select-String -Path *.cpp,*.h -Pattern 'JITEngine|JITCache|HotPathDetector|TierTransition' | Where-Object { $_.Path -notmatch '\\jit\\' }
# النتيجة: 0

# 3. لا أعلام CLI
Select-String -Path tools,compiler -Pattern '--jit|enable_jit|تجميع_فوري'
# النتيجة: 0
```

## التغييرات على `compiler/CMakeLists.txt`

1. **حُذفت كتلة كاملة** (سطور 221-251 سابقاً):
   - `set(JIT_SOURCES ...)` بـ 10 مصادر
   - `add_library(sad_jit STATIC ...)`
   - `target_include_directories(sad_jit PUBLIC ...)`
   - `target_link_libraries(sad_jit PUBLIC sad_llvm_backend ${llvm_libs})`
2. **حُذفت إشارة** `sad_jit` من `target_link_libraries(sad_compiler INTERFACE ...)` ضمن
   كتلة `if(LLVM_FOUND)`.
3. أُضيف تعليق توثيقي مكان الكتلة المحذوفة يشرح السبب ويُشير إلى هذا README.

## استعادة الملفات (إذا تقرّر إحياء JIT)

```powershell
Move-Item archived/jit_compiler_unused/src/jit       compiler/src/
Move-Item archived/jit_compiler_unused/include/jit   compiler/include/
# ثم استعادة كتلة JIT Library في compiler/CMakeLists.txt
# ثم إضافة sad_jit مرة أخرى لـ target_link_libraries(sad_compiler INTERFACE ...)
```

## خطة الإحياء (إذا دعت الحاجة لاحقاً)

لتحويل `compiler/src/jit/` من تصميم نظري إلى مكتبة عاملة، يلزم:

1. **استبدال forward decls بـ LLVM ORC headers الحقيقية**:
   ```cpp
   #include <llvm/ExecutionEngine/Orc/LLJIT.h>
   #include <llvm/ExecutionEngine/Orc/ThreadSafeModule.h>
   ```
2. **تنفيذ `JITEngine::compileFunction(IR)`** فعلياً عبر `LLJIT::addIRModule()`.
3. **ربط `InterpreterCore::callFunction()`** ليرسل دوال ساخنة (`call_count > threshold`)
   إلى JIT — وهذا يتطلب توليد LLVM IR من AST في وقت التشغيل (طبقة جديدة).
4. **مزامنة `Value` (المفسر) ↔ LLVM types (JIT)** — الجزء الأصعب: المفسر يستخدم
   `std::variant`، JIT يحتاج تمثيل LLVM ثابت.
5. **CLI flag**: `--jit-threshold=N` في `tools/compiler/` و`tools/repl/`.
6. **اختبارات أداء** تُثبت تسريعاً فعلياً (وإلا فلا قيمة).

**التقدير:** أسابيع إلى أشهر من العمل المتفرّغ — وغير مبرَّر طالما `sadc` (AOT) يلبي
الحاجة للأكواد الحرجة.

## التحقق بعد الأرشفة

- `cmake --build build --config Release --target sad sadc` → ✅ نجح
- اختبارات `tests/compiler_features/` → `PASS=32 XFAIL=18 FAIL=0` ✅
- `vm/sad_jit` (الحي) لم يتأثر — يبقى ضمن `vm` library ✅

## الدرس المُستفاد (BF-31 — للذاكرة)

> **"مكتبة تُبنى" لا يعني "مكتبة تعمل".** ابحث عن استدعاءات الرموز فعلياً
> (`Select-String` للأنواع/الوظائف العامة) قبل الافتراض أن ملفاً مدمج. مكتبة معزولة
> = ميتة، حتى لو كانت في CMake.

تاريخ الأرشفة: ضمن جلسة Fix #6 (تنظيف الميزات غير المكتملة).
