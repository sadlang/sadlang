# البنية المعمارية لمولد LLVM في مترجم لغة ص

> **الإصدار:** Phase 10 (commit `3d6ce8bf`)
> **المسار:** `compiler/{include,src}/backend/llvm/builders/`
> **الحالة:** 36 sub-codegen، 89 ملف، 8 مجلدات وظيفية، ~30,000 سطر
> **الاختبارات:** 793/837 (94.7%) — صفر تراجع منذ Phase 8

---

## 1. الفلسفة المعمارية

### 1.1 المشكلة التاريخية
قبل Phase 8، كان `llvm_codegen.cpp` يحتوي على **525+ method** في ملف واحد بحجم آلاف الأسطر. كل ميزة جديدة (UI، SIMD، concurrency، closures...) تُضاف إلى نفس الملف، مما جعل:
- التجميع بطيئاً (إعادة بناء كاملة لأي تعديل صغير)
- مراجعة الكود مستحيلة (صراعات merge مستمرة)
- التعاون متعذراً (كل المطورين يكتبون في نفس الملف)
- التنقل في IDE بطيئاً (ملف 30K سطر)

### 1.2 الحل المعتمد: Sub-CodeGen Pattern
- كل **مجال وظيفي** يحصل على صنف منفصل (`XCodeGen`)
- جميع sub-codegens تشترك في مرجع واحد إلى `LLVMCodeGen` (`cg_`)
- `LLVMCodeGen` يرث من `LLVMCodeGenContext` (struct بحقول public)
- الحقول المشتركة (`context_`, `module_`, `builder_`, `typeMapper_`, `errors_`...) يمكن لكل sub-codegen الوصول إليها عبر `cg_.X`
- لا توجد علاقة `friend class` (تمت إزالة 35 friend في Phase 8)

### 1.3 المبادئ الحاكمة (CW-01 → CW-30)
- **CW-01 (SRP):** كل sub-codegen مسؤول عن مجال واحد فقط
- **CW-02 (Layered):** `Lexer → Parser → AST → SIR → LLVM` — sub-codegens في الطبقة الأخيرة فقط
- **CW-05 (حجم الملف):** أكبر ملف ops هو 2486 سطر (`arith_*.cpp` مجموع) — معظم الملفات < 1500 سطر
- **CW-19 (DRY):** المنطق المشترك في `LLVMCodeGen` نفسه (`reportError`, `getLLVMType`...)
- **CW-20 (OCP):** إضافة sub-codegen جديد لا يتطلب تعديل أي sub-codegen موجود

---

## 2. التقسيم إلى 8 فئات وظيفية

```
compiler/include/backend/llvm/builders/
├── core/             ← 4 sub-codegens (الأساسيات)
├── arithmetic/       ← 3 (الحساب والتحكم)
├── memory/           ← 3 (الذاكرة والمؤشرات)
├── oop/              ← 5 (البرمجة الكائنية)
├── collections/      ← 5 (المصفوفات والخرائط والنصوص)
├── builtins/         ← 5 (الدوال المضمنة)
├── platform/         ← 7 (المنصة والـ FFI)
└── directives/       ← 4 (التوجيهات والتزامن)
```

---

## 3. تفصيل الفئات الثماني

### 3.1 `core/` — الأساسيات (4 sub-codegens / 7 ops files / 3,519 سطر)

| Sub-CodeGen | ملفات Ops | الأسطر | الدور |
|---|---|---|---|
| `instr_core_codegen` | `instr_core_ops.cpp` | 660 | dispatcher مركزي لتوليد تعليمات SIR |
| `output_codegen` | `output_ops.cpp` | 382 | كتابة LLVM IR إلى ملفات `.ll` و `.o` |
| `types_codegen` | `types_ops.cpp` | 232 | تحويل أنواع SIR → LLVM (17 method) |
| `freestanding_codegen` | `freestanding_*_ops.cpp` (4) | 2245 | دعم freestanding/kernel mode (لا libc) |

**نقاط مهمة:**
- `types_codegen` هو **آخر sub-codegen مُستخرج** (Phase 9)
- `freestanding_codegen` يحوي conv/io/sprintf/base — أدوات أساسية للنواة بدون C runtime
- `instr_core_codegen` هو **نقطة الدخول** — يستقبل SIR instruction ويوزع إلى الـ sub-codegen المناسب

---

### 3.2 `arithmetic/` — الحساب وتحكم التدفق (3 sub-codegens / 8 ops files / 3,897 سطر)

| Sub-CodeGen | ملفات Ops | الأسطر | الدور |
|---|---|---|---|
| `arithmetic_codegen` | `arith_main/cmp/extras/resolve/type_conv.cpp` | 2486 | جميع العمليات الحسابية (`+`, `-`, `*`, `/`, `%`, `**`, مقارنات) |
| `controlflow_codegen` | `cf_branch_call.cpp`, `cf_return_switch.cpp` | 881 | `if`, `while`, `for`, `match`, `switch`, return |
| `exception_codegen` | `exception_ops.cpp` | 530 | `try`/`catch`/`throw`/`finally` (Itanium C++ ABI) |

**نقاط مهمة:**
- `arithmetic` هو الأكبر — مقسّم إلى 5 ملفات ops حسب نوع العملية
- `arith_resolve.cpp` يتعامل مع overload resolution لـ `+` (string concat vs numeric add)
- `controlflow` يستخدم `BasicBlock ordering` بترتيب الإدراج (وليس أبجدياً) — ⚠️ راجع compiler_fix_notes.md

---

### 3.3 `memory/` — الذاكرة والإغلاقات (3 sub-codegens / 5 ops files / 2,601 سطر)

| Sub-CodeGen | ملفات Ops | الأسطر | الدور |
|---|---|---|---|
| `memory_codegen` | `mem_alloca/load/store.cpp` | 1606 | إدارة ذاكرة المكدس (alloca/load/store) |
| `closure_codegen` | `closure_ops.cpp` | 844 | بناء الإغلاقات وتمرير المتغيرات الملتقطة |
| `aggregate_ops_codegen` | `agg_ops.cpp` | 151 | بنى مجمعة (struct/array literals) |

**نقاط مهمة:**
- `memory_codegen` يحوي ownership tracking (Phase 6 SIR ownership opcodes)
- `closure_codegen` يدعم capture-by-value و capture-by-reference
- `aggregate_ops` صغير لأنه يتعامل فقط مع insertvalue/extractvalue

---

### 3.4 `oop/` — البرمجة الكائنية (5 sub-codegens / 6 ops files / 4,548 سطر)

| Sub-CodeGen | ملفات Ops | الأسطر | الدور |
|---|---|---|---|
| `classes_vtables_codegen` | `classes_vtables_ops.cpp` | 787 | جداول الدوال الافتراضية (vtables)، dispatch ديناميكي |
| `oop_ops_codegen` | `oop_ops.cpp` | 931 | إنشاء الكائنات (`جديد`)، الوصول للحقول، `هذا` |
| `objects_arrays_codegen` | `objects_arrays_ops.cpp` | 1050 | مصفوفات الكائنات، تحويلات النوع لـ `أي` |
| `enum_ops_codegen` | `enum_ops.cpp` | 669 | التعدادات (tagged unions) |
| `functions_codegen` | `functions_ops.cpp`, `functions2_ops.cpp` | 1111 | تعريف الدوال، الباني/الهادم، lambda |

**نقاط مهمة:**
- vtables تُبنى لكل صنف يحوي method `مجرد` أو override
- `functions2_ops.cpp` فُصل لتقليل وقت التجميع (CW-05)
- `enum_ops` يستخدم tagged-union pattern مع discriminant byte

---

### 3.5 `collections/` — المصفوفات والخرائط والنصوص (5 sub-codegens / 7 ops files / 4,147 سطر)

| Sub-CodeGen | ملفات Ops | الأسطر | الدور |
|---|---|---|---|
| `array_ops_codegen` | `array_ops.cpp` | 681 | مصفوفات ديناميكية (vec equiv) |
| `array_builtins_codegen` | `array_builtins.cpp` | 660 | `.اضف()`, `.احذف()`, `.رتب()`, `.خريطة()`... |
| `map_ops_codegen` | `map_ops.cpp` | 712 | الخرائط (hash maps) |
| `string_ops_codegen` | `string_ops.cpp` | 1446 | عمليات النصوص الأساسية (concat, slice...) |
| `strings_codegen` | `strings_ops.cpp`, `string_ops2_ops.cpp`, `string_ops_ops.cpp` | 648 | دوال نصوص مكتبة `نصوص` المتقدمة |

**نقاط مهمة:**
- التمييز بين `string_ops` (operators) و `strings` (library functions) دلالي مهم
- `array_builtins` يدعم functional methods (map/filter/reduce)
- Map يستخدم open addressing مع linear probing

---

### 3.6 `builtins/` — الدوال المضمنة (5 sub-codegens / 6 ops files / 3,052 سطر)

| Sub-CodeGen | ملفات Ops | الأسطر | الدور |
|---|---|---|---|
| `builtin_funcs_codegen` | `builtin_funcs_ops.cpp`, `builtins_math_async_ops.cpp` | 776 | `اطبع`, `طول`, `نوع`, `رقم()`, `نص()`... |
| `io_builtins_codegen` | `io_builtins_ops.cpp` | 508 | `اقرأ`, `اطبع_سطر`, file I/O |
| `math_builtins_codegen` | `math_builtins.cpp` | 489 | sin/cos/log/exp/pow/sqrt + ثوابت π/e |
| `network_builtins_codegen` | `builtins_network_ops.cpp` | 897 | TCP/UDP, HTTP بدائي |
| `security_builtins_codegen` | `security_builtins_ops.cpp` | 382 | hashing, encryption (SHA-256, AES-CTR) |

**نقاط مهمة:**
- `builtin_funcs` يحوي ~21 دالة لا تحتاج `استورد`
- `network/security` تتطلب stdlib مرتبطة (`sad_network` library)

---

### 3.7 `platform/` — المنصة و FFI و UI (7 sub-codegens / 9 ops files / 5,184 سطر)

| Sub-CodeGen | ملفات Ops | الأسطر | الدور |
|---|---|---|---|
| `instr_platform_codegen` | `instr_platform_ops.cpp` | 318 | تعليمات خاصة بالمنصة (platform-specific intrinsics) |
| `instr_lowlevel_codegen` | `instr_lowlevel_ops.cpp` | 406 | تعليمات منخفضة المستوى (asm inline, registers) |
| `lowlevel_codegen` | `lowlevel_ops.cpp`, `lowlevel_uefi_ops.cpp` | 1429 | UEFI/kernel boot، MSR، CPU features |
| `ui_codegen` | `ui_ops.cpp` | 504 | SDL2 + OpenGL bindings، widgets |
| `file_casts_codegen` | `file_casts_ops.cpp` | 375 | ptrtoint/inttoptr/bitcast الآمنة |
| `hardware_ffi_codegen` | `hardware_ffi_ops.cpp`, `hardware_ffi_ops2.cpp` | 1512 | استدعاءات C ABI، struct passing |
| `ffi_remain_codegen` | `ffi_remain_ops.cpp` | 112 | بقية حالات FFI الخاصة |

**نقاط مهمة:**
- `lowlevel_codegen` هو الأكبر header (183 سطر) — يحوي UEFI bootloader support لـ Ufuq OS
- `hardware_ffi` يدعم تمرير structs بقيمة عبر System V AMD64 ABI
- `ui_codegen` مرتبط بمكتبة `sad_ui` و `graphics/`

---

### 3.8 `directives/` — التوجيهات والتزامن (4 sub-codegens / 5 ops files / 2,835 سطر)

| Sub-CodeGen | ملفات Ops | الأسطر | الدور |
|---|---|---|---|
| `directives_codegen` | `directives_ops.cpp` | 461 | `@غير_آمن`, `@وقت_الترجمة`, `@متطاير`, `@حجم` |
| `simd_codegen` | `simd_ops.cpp` | 484 | تعليمات SIMD (Phase 3) — vectors |
| `concurrency_codegen` | `concurrency_ops.cpp`, `channels_ops.cpp` | 1274 | goroutines, channels, mutex, futures, WaitGroup |
| `coroutines_codegen` | `coroutines_ops.cpp` | 616 | async/await (LLVM coroutine intrinsics) |

**نقاط مهمة:**
- `concurrency` هو الأكبر — يحوي SadChannel، GoroutineManager
- `coroutines` يستخدم LLVM coro intrinsics (`@llvm.coro.*`)
- `directives_codegen` يعالج توجيهات `@` التي تظهر كـ AST nodes خاصة

---

## 4. أنماط البرمجة المعتمدة

### 4.1 نمط Sub-CodeGen القياسي
```cpp
// ملف header (مثال: arithmetic_codegen.h)
namespace Sad::LLVM {
class LLVMCodeGen;  // forward decl

class ArithmeticCodeGen {
public:
    explicit ArithmeticCodeGen(LLVMCodeGen &cg) : cg_(cg) {}

    // (AR) توليد عملية جمع | (EN) Generate add operation
    llvm::Value *generateAdd(const SIR::Instruction &inst);

    // ... باقي الدوال العامة

private:
    LLVMCodeGen &cg_;  // المرجع الوحيد للحالة المشتركة
};
}
```

### 4.2 نمط استخدام `cg_` في ملفات ops
```cpp
// ملف ops (مثال: arith_main.cpp)
#include "builders/arithmetic/arithmetic_codegen.h"
#include "backend/llvm/llvm_codegen.h"

using namespace Sad::LLVM;

llvm::Value *ArithmeticCodeGen::generateAdd(const SIR::Instruction &inst) {
    auto *lhs = cg_.getLLVMValue(inst.operands[0]);  // وصول عبر cg_
    auto *rhs = cg_.getLLVMValue(inst.operands[1]);
    if (!lhs || !rhs) {
        cg_.reportError("Invalid operands");
        return nullptr;
    }
    return cg_.builder_->CreateAdd(lhs, rhs, "add_result");
}
```

### 4.3 نمط الاستدعاء من `LLVMCodeGen`
```cpp
// llvm_codegen.h
class LLVMCodeGen : public LLVMCodeGenContext {
private:
    std::unique_ptr<ArithmeticCodeGen> arithmetic_;
    std::unique_ptr<TypesCodeGen> types_;
    // ... 34 آخرين

public:
    // wrapper inline (تفويض إلى sub-codegen)
    llvm::Type *getInt64Type() { return types_->getInt64Type(); }
};

// llvm_codegen_init.cpp constructor
LLVMCodeGen::LLVMCodeGen(...) {
    arithmetic_ = std::make_unique<ArithmeticCodeGen>(*this);
    types_ = std::make_unique<TypesCodeGen>(*this);
    // ...
}
```

---

## 5. مخطط التدفق الكامل

```
                    ┌──────────────────┐
                    │  SIR Instruction │
                    └────────┬─────────┘
                             ↓
                ┌────────────────────────┐
                │  instr_core_codegen    │  ← dispatcher مركزي
                │  switch (instr.opcode) │
                └────────┬───────────────┘
                         ↓
        ┌────────────────┼────────────────┐
        ↓                ↓                ↓
  ┌──────────┐   ┌──────────────┐  ┌─────────────┐
  │arithmetic│   │   memory     │  │     oop     │  ... (8 categories)
  │codegens  │   │   codegens   │  │   codegens  │
  └────┬─────┘   └──────┬───────┘  └──────┬──────┘
       │                │                  │
       └────────────────┼──────────────────┘
                        ↓
              ┌──────────────────┐
              │  cg_.builder_    │  ← LLVM IRBuilder
              │  (مشترك)         │
              └────────┬─────────┘
                       ↓
              ┌──────────────────┐
              │  LLVM IR Module  │
              └────────┬─────────┘
                       ↓
              ┌──────────────────┐
              │   output_codegen │  → .o / .exe
              └──────────────────┘
```

---

## 6. إحصائيات شاملة

| الفئة | sub-codegens | headers (أسطر) | ops files | ops (أسطر) | المجموع |
|---|---:|---:|---:|---:|---:|
| core | 4 | 196 | 7 | 3,519 | 3,715 |
| arithmetic | 3 | 149 | 8 | 3,897 | 4,046 |
| memory | 3 | 109 | 5 | 2,601 | 2,710 |
| oop | 5 | 198 | 6 | 4,548 | 4,746 |
| collections | 5 | 218 | 7 | 4,147 | 4,365 |
| builtins | 5 | 210 | 6 | 3,052 | 3,262 |
| platform | 7 | 471 | 9 | 5,184 | 5,655 |
| directives | 4 | 170 | 5 | 2,835 | 3,005 |
| **المجموع** | **36** | **1,721** | **53** | **29,255** | **30,976** |

---

## 7. تاريخ التطوير (Phases)

| Phase | Commit | المنجز |
|---|---|---|
| 6 | (تاريخي) | SIR ownership opcodes (12 opcodes) |
| 7 | (تاريخي) | بدء استخراج sub-codegens (5-10) |
| 8 step 1-9 | (تاريخي) | استخراج 25 sub-codegen إضافي |
| 8 step 10 | `b504ceb2` | mega-extraction: 35 sub-codegens / 525 methods |
| 8 cleanup | `3f72dafb` | إزالة 35 friend class، تعميم `context_info_` + 48 wrappers |
| 9 | `ce07eac9` | استخراج `TypesCodeGen` (17 methods)، init.cpp 669→348 |
| **10** | **`3d6ce8bf`** | **إعادة تنظيم في 8 مجلدات وظيفية، 89 ملف** |

---

## 8. القواعد الذهبية لإضافة sub-codegen جديد

1. **اختر الفئة الصحيحة** — راجع وصف 8 فئات أعلاه
2. **أنشئ header** في `compiler/include/backend/llvm/builders/CAT/X_codegen.h`
3. **أنشئ ops file** في `compiler/src/backend/llvm/builders/CAT/X_ops.cpp`
4. **أضف include** في `llvm_codegen.h`: `#include "builders/CAT/X_codegen.h"`
5. **أضف member**: `std::unique_ptr<XCodeGen> x_;`
6. **هيّئ في constructor**: `x_ = std::make_unique<XCodeGen>(*this);`
7. **أضف wrappers public** للوصول الخارجي (إن لزم)
8. **أعد تشغيل cmake** لإلتقاط الملفات الجديدة (GLOB_RECURSE):
   ```powershell
   cmake -S . -B build
   cmake --build build --config Release --target sad-build
   ```
9. **شغّل الاختبارات**: `python tests/dual_execution/runner.py`
10. **commit + push**

---

## 9. مراجع متقاطعة

- `/memories/repo/compiler_fix_notes.md` — تاريخ كل phase وأخطاء معروفة
- `compiler/include/backend/llvm/llvm_codegen.h` — الـ class الرئيسي
- `compiler/src/backend/llvm/llvm_codegen_init.cpp` — تهيئة sub-codegens
- `compiler/src/sir/sir_opcodes.h` — قائمة كل SIR opcodes
- `docs/SAD_LANGUAGE_COMPLETE_REFERENCE.md` — مواصفات اللغة الكاملة

---

**نهاية الوثيقة المعمارية — Phase 10**
