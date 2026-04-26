# مسارات WASM المؤرشفة (Dead/Deferred WASM Paths — Archived)

## السبب من الأرشفة

كان المشروع يحتوي على **6 مسارات WASM مختلفة** (14 ملف، ~150 KB) لتحويل
لغة ص إلى WebAssembly، لكن المسار الوحيد المُستخدم فعلياً هو
`cmake/wasm.cmake` + `tools/wasm/sad_wasm.cpp` (Emscripten — يجمع المفسر كاملاً
ويُصدّره JS+WASM، يستخدمه `website/build-wasm.ps1`).

## الخريطة الكاملة

| # | المسار | الحالة قبل | السبب |
|---|--------|----------|-------|
| 1 | `compiler/src/backend/wasm_codegen.cpp` | يُبنى لكن dead code | لا أحد يستدعي `WasmCodeGen` |
| 2 | `compiler/src/backend/wasm_ast_visitor.cpp/.h` | يُبنى لكن dead | مساعد للمسار 1 |
| 3 | `compiler/src/backend/wasm_optimizer.cpp` | يُبنى لكن dead | محسن للمسار 1 |
| 4 | `compiler/src/backend/wasm_bindgen.cpp` | يُبنى لكن dead | JS bindings للمسار 1 |
| 5 | `compiler/src/backend/wasm_direct/` (7 ملفات) | معطّل صراحة | `# wasm_direct deferred — needs ASTNode API update` |
| 6 | `compiler/src/backend/wasm_direct_emitter.h` | معطّل | header للمسار 5 |
| 7 | `compiler/src/backend/targets/wasm32_target.cpp` | معطّل | Target descriptor ميت |
| 8 | `compiler/src/targets/wasm.cpp` (805 سطر) | يتيم تماماً | غير مرجَّع في أي CMakeLists.txt |

## ما الذي بقي حياً (المسار الإنتاجي الوحيد)

```
cmake/wasm.cmake             ← مفعّل بـ -DBUILD_WASM=ON (يحتاج Emscripten)
tools/wasm/sad_wasm.cpp      ← entry point لـ Emscripten
tools/wasm/wasm_builtins.cpp
sad_ui/backends/web/         ← Emscripten EM_ASM للرسم في Canvas
website/build-wasm.ps1       ← سكريبت بناء الموقع
```

هذا المسار يعمل بفلسفة: **اجمع المفسر كله بـ Emscripten → احصل على
sad.wasm + sad.js → شغّل أي ملف ص في المتصفح**. هذا أبسط وأنضج من تحويل
sadc إلى مولّد WASM مستقل.

## التغييرات المصاحبة في compiler/CMakeLists.txt

1. أُزيلت `wasm_*.cpp` (4 ملفات) من `LLVM_BACKEND_SOURCES`.
2. أُزيلت `${CMAKE_CURRENT_SOURCE_DIR}/src/backend/wasm_direct` من include dirs.
3. أُزيلت `WebAssemblyCodeGen, WebAssemblyAsmParser, WebAssemblyDesc,
   WebAssemblyInfo` من `llvm_map_components_to_libnames`.

## كيفية الاسترجاع (إذا قُرّرت إكمال مسار sadc → WASM مباشر)

```powershell
Move-Item archived/wasm_dead_paths/compiler_backend/wasm_codegen.cpp     compiler/src/backend/
Move-Item archived/wasm_dead_paths/compiler_backend/wasm_ast_visitor.*   compiler/src/backend/
Move-Item archived/wasm_dead_paths/compiler_backend/wasm_optimizer.cpp   compiler/src/backend/
Move-Item archived/wasm_dead_paths/compiler_backend/wasm_bindgen.cpp     compiler/src/backend/
Move-Item archived/wasm_dead_paths/compiler_backend/wasm_direct          compiler/src/backend/
Move-Item archived/wasm_dead_paths/compiler_backend/wasm_direct_emitter.h compiler/src/backend/
Move-Item archived/wasm_dead_paths/compiler_backend/targets/wasm32_target.cpp compiler/src/backend/targets/
Move-Item archived/wasm_dead_paths/compiler_targets/*                    compiler/src/targets/
```

ثم استرجع المراجع في `compiler/CMakeLists.txt` (السطور المعلَّقة).

## التحقق بعد الأرشفة

- البناء: sad.exe + sadc.exe ✅
- اختبار: PASS=32 XFAIL=18 FAIL=0 (50 اختبار type-checker)
- لا تغيير في وظائف المفسر/المترجم الأصلية

## ملاحظة معمارية (BF-16 Lesson)

تم تأكيد "ميتة" بثلاث طرق:
1. `grep` لاستدعاءات `WasmCodeGen`/`WasmAstVisitor`/`WasmDirect` خارج ملفاتها → 0
2. فحص `cmake/wasm.cmake` و`tools/wasm/sad_wasm.cpp` → لا تستوردها
3. فحص `sad_ui/` و`website/` → تستخدم Emscripten + EM_ASM فقط، لا تتقاطع
