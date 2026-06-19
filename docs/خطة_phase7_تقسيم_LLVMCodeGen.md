# خطة Phase 7 — تقسيم god-class `LLVMCodeGen` إلى مكوّنات (F-07)

> **المرجع:** متابعة Phase 6 (مكتملة في `5feffcb6`)
> **الذاكرة المرتبطة:** `/memories/repo/compiler_fix_notes.md` (دروس Phase 6)
> **الحالة:** مُجدوَلة على عدة جلسات
> **آخر تحديث:** 2025-12

---

## 1. الواقع الحالي (الأرقام)

| البند | القيمة |
|-------|--------|
| الـheader | `compiler/include/backend/llvm/llvm_codegen.h` (1679 سطر) |
| الـcpp files | ~79 ملف في `compiler/src/backend/llvm/llvm_codegen_*.cpp` |
| إجمالي السطور | ~50,000+ سطر |
| تعريفات methods | ~150 method declaration |
| أقسام بانر | ~30 قسم منطقي |
| اختبارات (baseline) | 793/837 (94.7%) |
| أكبر cpp | `objects_arrays.cpp` (1049)، `builtins.cpp` (971)، `strings.cpp` (963) |

## 2. لماذا التقسيم؟ (نفس مبررات Phase 6)

- **CW-01 (SRP):** صنف واحد يحوي ~150 method
- **CW-05 (≤800 سطر):** header 1679 سطر؛ 6+ ملفات cpp تتجاوز 800 سطر
- صعوبة الفهم/الاختبار/التوسعة
- خطر تعارض التعديلات بين فرق مختلفة

## 3. التصميم النهائي المستهدف

```
┌─────────────────────────────────────────────────────────┐
│  LLVMCodeGen (facade) — public API ثابت                 │
│  └── يرث من LLVMCodeGenContext (state base)             │
│  ├── unique_ptr<ArithmeticCodeGen>   arith_             │
│  ├── unique_ptr<MemoryCodeGen>       memory_            │
│  ├── unique_ptr<ControlFlowCodeGen>  control_           │
│  ├── unique_ptr<BuiltinsCodeGen>     builtins_          │
│  ├── unique_ptr<CollectionsCodeGen>  collections_       │
│  ├── unique_ptr<OOPCodeGen>          oop_               │
│  ├── unique_ptr<ConcurrencyCodeGen>  concur_            │
│  ├── unique_ptr<FreestandingCodeGen> freestanding_      │
│  └── unique_ptr<ModuleCodeGen>       moduleEmit_        │
└─────────────────────────────────────────────────────────┘

LLVMCodeGenContext (struct — كل الحالة المشتركة):
  - context_, module_, builder_
  - currentFunction_, currentBlock_
  - namedValues_, globalValues_, classTypes_
  - vtableMap_, virtualMethodMap_
  - errors_, warnings_, optLevel_, ltoMode_
  - typeMapper_, optimizer_
  - moduleMode_, freestanding_
  - مكتبات runtime helpers cache
```

---

## 4. خطوات التنفيذ (10 خطوات)

> كل خطوة: استخراج → بناء sadc + sad → اختبارات → commit + push → تحديث الذاكرة.

| # | الخطوة | الـcpp المستهدفة | تقدير # methods |
|---|--------|------------------|----------------|
| 0 | استخراج `LLVMCodeGenContext` (base struct) | — | (بنية فقط) |
| 1 | `ArithmeticCodeGen` | arithmetic, arith bitwise, comparison, type_conversion | ~20 |
| 2 | `MemoryCodeGen` | store, alloca_move, instr_core (memory ops), bounds | ~15 |
| 3 | `ControlFlowCodeGen` | branch_call, exceptions, phi/select | ~15 |
| 4 | `BuiltinsCodeGen` | builtins, builtins_math_async, builtins_network, runtime_io_gc | ~30 |
| 5 | `CollectionsCodeGen` | strings, arabic_string_pool, maps, arrays (objects_arrays, array_ops, array_file_coro), aggregate_ops, enum_ops | ~30 |
| 6 | `OOPCodeGen` | oop_ops, classes_vtables, closures, drop_glue | ~20 |
| 7 | `ConcurrencyCodeGen` | coroutines, channels, async parts | ~15 |
| 8 | `FreestandingCodeGen` | freestanding, freestanding_conv, lowlevel, lowlevel_uefi, hardware_ffi, hardware_ffi2, security | ~25 |
| 9 | `ModuleCodeGen` (ختام) | init, module emit, output, errors, ui, directives, optimizer wrapper | ~20 |

**الإجمالي:** ~190 method (بعضها مكرر في الأقسام).

---

## 5. الأنماط المعتمدة (دروس Phase 6)

### 5.1 بنية sub-codegen
```cpp
// header
namespace Sad { namespace LLVM {
class LLVMCodeGen; // forward
class ArithmeticCodeGen {
    LLVMCodeGen& cg_;
public:
    explicit ArithmeticCodeGen(LLVMCodeGen& cg) : cg_(cg) {}
    llvm::Value* emitAdd(...);
    // ...
};
}}
```

### 5.2 wrappers في LLVMCodeGen
```cpp
llvm::Value* emitAdd(...) { return arith_->emitAdd(...); }
```

### 5.3 friend declarations
```cpp
class LLVMCodeGen : public LLVMCodeGenContext {
    friend class ArithmeticCodeGen;
    // ...
};
```

### 5.4 دروس واجبة التطبيق
- **L1:** الأنواع المتداخلة في base class **لا** تنتقل تلقائياً — استخدم `LLVMCodeGenContext::TypeName`
- **L2:** type aliases في namespace خارجي → استخدم الأنواع الحقيقية في sub-headers
- **L3:** forward-decl `class X;` يفشل إذا كان X هو `using` alias → `#include` بدلاً منه
- **L4:** عند نقل دالة لها default arg → الـwrapper يحتفظ بـ`= nullptr`
- **L5:** سكربت إضافة `cg_.` تلقائياً قد يصيب دوال LLVMCodeGen المتبقية → راجع `grep "LLVMCodeGen::"` بعد التشغيل
- **L6:** تحديث `cmake/sources.cmake` بإزالة الإدخالات المنقولة وإضافة الجديدة

---

## 6. خطة التحقق لكل خطوة

1. `cmake --build build --config Release --target sad-build` → 0 أخطاء
2. `cmake --build build --config Debug --target sad-run` → 0 أخطاء
3. `python tests/dual_execution/runner.py` → ≥790/837 (baseline 793)
4. `git commit` بصياغة `refactor(phase-7-step-N): فصل XCodeGen (~M methods, K ملفات)`
5. `git push origin graphic`
6. تحديث `/memories/repo/compiler_fix_notes.md` بالدروس الجديدة

---

## 7. المخاطر

- **R1:** LLVM types معقدة — احتمال أخطاء type-mismatch أعلى من Phase 6
- **R2:** runtime helpers مشتركة بين أقسام (مثل `getOrCreatePrintfDecl`) → قد تتطلب نقلها للـContext
- **R3:** بعض الدوال تعتمد على بعضها عبر الأقسام → قد نحتاج public helpers في Context
- **R4:** Freestanding/UEFI كبير ومعقد — يُترك للخطوة 8 (متأخر) لتراكم الخبرة

---

## 8. الحالة

- [ ] Step 0 — LLVMCodeGenContext
- [ ] Step 1 — ArithmeticCodeGen
- [ ] Step 2 — MemoryCodeGen
- [ ] Step 3 — ControlFlowCodeGen
- [ ] Step 4 — BuiltinsCodeGen
- [ ] Step 5 — CollectionsCodeGen
- [ ] Step 6 — OOPCodeGen
- [ ] Step 7 — ConcurrencyCodeGen
- [ ] Step 8 — FreestandingCodeGen
- [ ] Step 9 — ModuleCodeGen (ختام Phase 7)
