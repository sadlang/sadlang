# تنسيق Bytecode المؤرشف (Bytecode Format — Archived)

## السبب من الأرشفة

هذه المكتبة (`sad_bytecode`) كانت تصميماً لتنسيق توزيع `.sadbc`
(magic header + opcodes + constant pool + symbol table + validator)
ولم يُكمَل أبداً.

### الحقائق التي أدت للأرشفة (تحقق فعلي):
1. **3 من 11 ملف معطّلة في البناء**: `emitter.cpp`, `emitter_optimize.cpp`,
   `bytecode.cpp` — مستثناة بـ `list(FILTER ... EXCLUDE)` مع تعليق
   "deferred - need SIR API update" منذ فترة طويلة.
2. **8 ملفات مبنية لكن غير مستخدمة**: جميع `#include` لـ headers هذه
   المكتبة كانت داخلية فقط (داخل bytecode/) — لا أحد خارجها يستوردها.
3. **الاستخدام الخارجي الوحيد** كان `runtime/include/bytecode_compat.h`،
   لكن `bytecode_compat.h` بدوره يُستخدم فقط من `archived/runtime_experiments/`
   (مؤرشف سابقاً).
4. **C ABI exports غير مستدعاة**: دوال `sad_bytecode_validator_new`,
   `sad_bytecode_validate`, `sad_bytecode_magic_string`, ... موجودة كـ
   exports لكنها ليست مستدعاة من أي مكان داخل المشروع.
5. **VM الفعلية مستقلة**: مجلد `vm/` يستخدم opcodes خاصة به في
   `sad_vm_opcodes.h` ولا يتقاطع مع هذا التنسيق.
6. **لا قراءة/كتابة لـ `.sadbc`**: لا في sad.exe ولا في sadc.exe.

## ما الذي تحتويه الأرشيف

```
archived/bytecode_format/
  src/        ← 11 ملف .cpp (8 مبنية + 3 معطلة)
  include/    ← 5 ملفات header (bytecode.h, format.h, opcodes.h, ...)
  README.md   ← هذا الملف
```

## كيفية الاسترجاع

إذا أردت إكمال هذا التنسيق مستقبلاً:

```powershell
Move-Item archived/bytecode_format/src compiler/src/bytecode
Move-Item archived/bytecode_format/include compiler/include/bytecode
```

ثم استرجع الكتلة في `compiler/CMakeLists.txt` (تم تعليقها):

```cmake
file(GLOB BYTECODE_SOURCES "src/bytecode/*.cpp")
file(GLOB BYTECODE_HEADERS "include/bytecode/*.h")
list(FILTER BYTECODE_SOURCES EXCLUDE REGEX "emitter\\.cpp")
list(FILTER BYTECODE_SOURCES EXCLUDE REGEX "emitter_optimize\\.cpp")
list(FILTER BYTECODE_SOURCES EXCLUDE REGEX "bytecode\\.cpp")
add_library(sad_bytecode STATIC ${BYTECODE_SOURCES})
```

وأرجع `sad_bytecode` إلى قائمة `target_link_libraries(sad_compiler ...)`.

## الخطوات لإكمال الميزة (إذا قُرر استرجاعها)

1. تحديث `emitter.cpp` ليستخدم SIR API الحالي (الذي تطور كثيراً).
2. إضافة قارئ `.sadbc` في sad.exe (للسماح بتشغيل bytecode).
3. كتابة `.sadbc` كـ output target في sadc.exe.
4. تصميم VM تنفذ هذه opcodes (أو دمجها مع `vm/sad_vm_*`).

## تاريخ الأرشفة

- التاريخ: في إطار "إصلاح 3" من خطة توحيد المكونات.
- اختبار التحقق بعد الأرشفة: PASS=32 XFAIL=18 FAIL=0 (50 اختبار type-checker).
