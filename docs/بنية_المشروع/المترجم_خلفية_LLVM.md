# خلفية LLVM — `compiler_new/src/backend/llvm/`

> **الدور:** تحويل SIR إلى LLVM IR ثم إلى كود أصلي أو ملف تنفيذي  
> **الفضاء:** `Sad::Compiler::LLVM`  
> **المدخل:** `SIRModule` (محسّن)  
> **المخرج:** `llvm::Module` → `.o` → `.exe`  
> **الاعتماد:** LLVM 18 (اختياري — يُفعَّل بـ `ENABLE_LLVM_BACKEND=ON`)

---

## نظرة عامة

الخلفية LLVM هي **المرحلة الأخيرة** في المترجم. تحوّل كل تعليمة SIR إلى مجموعة تعليمات LLVM IR.  
هذا أكبر مكون في المشروع: **55 ملف, ~38,000+ سطر**.

```
SIRModule → [LLVMCodeGen] → llvm::Module → [LLVM Backend] → .o → [Linker] → .exe
```

---

## جدول الملفات (55 ملف)

### النواة الأساسية

| الملف | الدور | الدوال الرئيسية |
|-------|------|-----------------|
| `llvm_codegen_init.cpp` | ⚡ التهيئة — إنشاء Module و Context | `initializeModule()`, `initializeLLVM()` |
| `llvm_codegen_instructions.cpp` | ⚡ **الحلقة الرئيسية** — ترجمة كل `SIRInstruction` | `generateInstruction()`, `generateAlloc()`, `generateStore()`, `generateLoad()`, `generateCall()`, `generateReturn()` |
| `llvm_codegen_output.cpp` | إخراج النتيجة (IR, .o, .exe) | `emitObjectFile()`, `emitLLVMIR()`, `link()` |
| `llvm_codegen_functions.cpp` | توليد الدوال | `generateFunction()`, `generateFunctionDecl()` |
| `llvm_entry_point.cpp` | نقطة الدخول `main()` | `generateEntryPoint()`, `generateMainWrapper()` |
| `llvm_type_mapper.cpp` | تحويل أنواع SIR → LLVM | `mapType()`, `getSadStringType()`, `getSadArrayType()` |
| `llvm_type_mapper_composite.cpp` | أنواع مركّبة (أصناف, arrays) | `mapClassType()`, `mapArrayType()` |

### العمليات الحسابية والمنطقية

| الملف | الدور |
|-------|------|
| `llvm_codegen_arithmetic.cpp` | +, -, *, /, %, ** |
| `llvm_codegen_branch_call.cpp` | تفريعات + استدعاءات |
| `llvm_codegen_store.cpp` | تخزين القيم في الذاكرة |

### العمليات على الأنواع

| الملف | الدور |
|-------|------|
| `llvm_codegen_string_ops.cpp` | عمليات النصوص (ربط, مقارنة, طباعة) |
| `llvm_codegen_array_ops.cpp` | عمليات المصفوفات (إنشاء, وصول, تعديل) |
| `llvm_codegen_array_file_coro.cpp` | مصفوفات + ملفات + coroutines |
| `llvm_codegen_objects_arrays.cpp` | كائنات + مصفوفات مختلطة |
| `llvm_codegen_enum_ops.cpp` | عمليات التعدادات |
| `llvm_codegen_aggregate_ops.cpp` | عمليات على الهياكل المركبة |
| `llvm_codegen_type_conversions.cpp` | تحويلات بين الأنواع |
| `llvm_dict_support.cpp` | دعم القواميس/الخرائط |
| `llvm_array_support.cpp` | دعم المصفوفات المحسّن |

### الدوال المدمجة

| الملف | الدور |
|-------|------|
| `llvm_codegen_builtins.cpp` | توليد استدعاءات الدوال المدمجة |
| `llvm_codegen_builtin_funcs.cpp` | تنفيذ الدوال المدمجة في LLVM IR |

### التزامن

| الملف | الدور |
|-------|------|
| `llvm_codegen_concurrency.cpp` | ⚠️ goroutines, channels, select (~1924 سطر) |

### الأنظمة المنخفضة المستوى

| الملف | الدور |
|-------|------|
| `llvm_codegen_freestanding.cpp` | وضع freestanding (بلا مكتبة قياسية) |
| `llvm_codegen_freestanding_io.cpp` | I/O في وضع freestanding |
| `llvm_codegen_lowlevel.cpp` | عمليات منخفضة المستوى |
| `llvm_codegen_memory_control.cpp` | تحكم بالذاكرة |
| `llvm_codegen_hardware_ffi.cpp` | FFI للعتاد |
| `llvm_codegen_directives.cpp` | التوجيهات (`@حجم`, `@ذري`, `@غير_آمن`) |
| `llvm_inline_asm.cpp` | تجميع مضمّن (`@تجميع`) |
| `llvm_atomics.cpp` | عمليات ذرية |
| `llvm_volatile_ops.cpp` | عمليات متطايرة |
| `llvm_port_io_intrinsics.cpp` | منافذ I/O (in/out) |
| `llvm_cpu_regs.cpp` | سجلات المعالج |

### الربط والأهداف

| الملف | الدور |
|-------|------|
| `llvm_linker.cpp` | الربط (linking) |
| `llvm_target.cpp` | إدارة الأهداف (x86_64, ARM, ...) |
| `llvm_bare_metal_linker.cpp` | ربط bare-metal |
| `llvm_multiboot.cpp` | رؤوس Multiboot |
| `llvm_paging.cpp` | إعداد الصفحات (paging) |
| `llvm_custom_sections.cpp` | أقسام مخصصة في ELF |
| `llvm_linker.cpp` | ربط |
| `toolchain_detection.cpp` | كشف toolchain (gcc, clang, MSVC) |

### التحسين

| الملف | الدور |
|-------|------|
| `llvm_optimizer.cpp` | تحسينات LLVM (O0-O3) |

### وقت التشغيل

| الملف | الدور |
|-------|------|
| `llvm_runtime.cpp` | دوال وقت التشغيل (GC, allocator) |
| `llvm_runtime_io_gc.cpp` | I/O + جمع القمامة |
| `llvm_drop_glue.cpp` | drop glue (تحرير الموارد) |

### العربية والتصحيح

| الملف | الدور |
|-------|------|
| `arabic_normalization.cpp` | تطبيع النصوص العربية |
| `arabic_optimizer.cpp` | تحسينات خاصة بالعربية |
| `arabic_string_pool.cpp` | تجميع الثوابت النصية |
| `bidi_cache.cpp` | تخزين مؤقت للاتجاه (BiDi) |
| `tashkeel_optimizer.cpp` | تحسين التشكيل |
| `llvm_debug.cpp` | معلومات التصحيح (DWARF) |

### التوثيق الداخلي

| الملف | الدور |
|-------|------|
| `LLVM_CODEGEN_STRUCTURE.md` | توثيق بنية الـ codegen |

---

## مسار التنفيذ

```
LLVMCodeGen::generate(sirModule)
    │
    ├── initializeModule()            ← إنشاء llvm::Module + Context
    ├── mapTypes()                    ← تحويل أنواع SIR → LLVM
    │
    ├── لكل function في sirModule:
    │   ├── generateFunction()
    │   │   ├── createBasicBlocks()  ← إنشاء الكتل
    │   │   └── لكل instruction:
    │   │       └── generateInstruction()
    │   │           ├── ALLOC → llvm::CreateAlloca()
    │   │           ├── STORE → llvm::CreateStore()
    │   │           ├── LOAD → llvm::CreateLoad()
    │   │           ├── CALL → llvm::CreateCall()
    │   │           ├── BINOP → llvm::CreateAdd/Sub/Mul/...
    │   │           ├── BRANCH → llvm::CreateCondBr()
    │   │           ├── JUMP → llvm::CreateBr()
    │   │           ├── PHI → llvm::CreatePHI()
    │   │           └── RETURN → llvm::CreateRet()
    │   │
    │   └── generateEntryPoint()     ← main() wrapper
    │
    ├── llvm_optimizer.cpp           ← تحسينات LLVM
    └── emitObjectFile() / link()    ← إنتاج .exe
```

---

## العلاقات

- **🔗 يستهلك:** `SIRModule` (من الواجهة الأمامية)
- **🔗 ينتج:** `llvm::Module` → `.o` → `.exe`
- **🔗 يعتمد على:** LLVM 18 (`#ifdef HAS_LLVM`)

---

## ⚠️ ملاحظات مهمة

1. **أكبر مكون في المشروع** — 55 ملف, ~38,000+ سطر
2. **`#ifdef HAS_LLVM`** — كل الكود محمي بهذا الشرط (LLVM اختياري)
3. **`sadc` يجب بناؤه بـ `--config Release`** — بسبب عدم تطابق مكتبات LLVM في Debug
4. **`llvm_codegen_concurrency.cpp`** أكبر ملف فردي (~1924 سطر)
5. **ترتيب BasicBlocks مهم** — لا تستخدم `std::set` (يرتب أبجدياً ويكسر التنفيذ)
