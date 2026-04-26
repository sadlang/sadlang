# مكتبات runtime_new المؤرشفة

## السياق

في 2025 — أثناء مرحلة 0.5 من تنظيف بنية المشروع — تم تشخيص ست مكتبات
في `runtime_new/` بأنها **dead code فعلياً**:

- `src/vm/` + `include/vm/` — VM3 (الآلة الافتراضية الثالثة)
- `src/memory/` + `include/memory/` — GC + allocator تجريبيان
- `src/thread/` + `include/thread/` — مكتبة خيوط مستقلة
- `src/exception/` + `include/exception/` — هرمية استثناءات C++
- `src/panic/` — Smart Panic system (AI analyzer + recovery + reporter)
- `src/sandbox/` — Permission enforcer

## دليل أنها ميتة

```
dumpbin /symbols build/bin/Release/sad.exe   → 0 رمز من runtime_new
dumpbin /symbols build/bin/Release/sadc.exe  → 0 رمز من runtime_new (الـ 14 مرجعاً نصية فقط)
sad_rt_panic.lib   = 1,394 بايت (stub)
sad_rt_sandbox.lib = 1,392 بايت (stub)
```

linker حذف جميع الرموز عبر dead-code elimination. لم تكن أي من هذه المكتبات
مرتبطة فعلياً بالملفات التنفيذية النهائية.

## لماذا ميتة

- `__sad_alloc_jmpbuf`, `__sad_push_handler`, `__sad_raise` — **inline في codegen**
  (`builder_->CreateAlloca`, `CreateGEP` مباشرة) وليست external calls
- `__sad_panic_handler` — يولّد كنص LLVM IR من `compiler_new/src/backend/panic_handler.cpp`
  وليس استدعاء لمكتبة خارجية
- المفسر `sad` يستخدم `std::shared_ptr` للكائنات (refcount ضمني) — لا يحتاج GC
- لا توجد `extern "C"` تصدّر الرموز كـ runtime ABI

## كيفية الاستعادة (إن لزم)

```powershell
Move-Item archived/runtime_experiments/src_vm runtime_new/src/vm
Move-Item archived/runtime_experiments/include_vm runtime_new/include/vm
# ... وهكذا
# ثم استعادة CMakeLists.txt.original إلى runtime_new/CMakeLists.txt
```

## ما المتبقي في `runtime_new/` (نشط فعلاً)

| المكتبة | الاستخدام |
|---------|-----------|
| `sad_rt_abi` | C ABI marshalling (يحتاجه `خارجي` functions) |
| `sad_rt_ffi` | FFI bindings + firewall + sandbox checks |
| `sad_rt_ui` | UI widgets (يربطه `llvm_codegen_ui.cpp`) |
| `sad_rt_freestanding` | اختياري — لـ OS / UEFI development |
