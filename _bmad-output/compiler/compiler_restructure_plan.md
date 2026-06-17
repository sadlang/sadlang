# خطة إعادة هيكلة `compiler/` — تقرير شامل

> **التاريخ:** مايو 2026  
> **النطاق:** مجلد `compiler/` بالكامل (مع التركيز على `compiler/src/backend/`)  
> **المنهجية:** قواعد CW-01 → CW-30 + BF-09 (حلول جذرية)

---

## 1. الوضع الحالي — تشخيص بنيوي

### 1.1 شجرة المجلد الفعلية

```
compiler/
├── CMakeLists.txt                    ← 850 سطر (مُتضخّم — يخالف CW-05)
├── benchmarks/
├── docs/
├── tests/
├── tools/
├── include/
│   ├── backend/
│   │   ├── android_compose.hpp
│   │   ├── async_codegen.hpp
│   │   ├── ios_swiftui.hpp
│   │   ├── ir_generator.hpp
│   │   ├── pipeline_chain.h
│   │   └── llvm/                     ← 37 ملف cpp مقابلة
│   ├── crypto/
│   ├── ffi/
│   ├── format/
│   ├── frontend/
│   │   ├── builders/
│   │   ├── sir_*.h                   ← 8 headers
│   ├── kernel/
│   ├── meta/
│   ├── packages/
│   ├── runtime/
│   ├── sir_optimizer/
│   ├── suggestions/
│   ├── testing/
│   └── types/
└── src/
    ├── frontend/                     ← SIR builder + types (12 ملف)
    │   └── builders/
    ├── sir_optimizer/                ← 15 ملف pass
    ├── meta/                         ← docs emitter (2 ملف)
    ├── types/                        ← نظام الأنواع (18 ملف)
    └── backend/                      ← 27 ملف + llvm/ (37 ملف)
```

### 1.2 خريطة طبقات المترجم الحالية

```
                    ┌─────────────────────────────────────────────┐
                    │  المصدر .ص (UTF-8)                           │
                    └──────────────────┬──────────────────────────┘
                                       ▼
        ┌──────────────────────────────────────────────────────────┐
        │  shared/lexer  →  shared/parser  →  shared/ast           │
        │  (مشترك بين المفسر والمترجم)                              │
        └──────────────────┬───────────────────────────────────────┘
                           ▼
        ┌──────────────────────────────────────────────────────────┐
        │            compiler/src/frontend/    (sad_frontend)       │
        │  ┌──────────────────────────────────────────────────┐    │
        │  │ SIR Builder: AST → SIR (Sad IR)                  │    │
        │  │  - sir_builder_module.cpp                         │    │
        │  │  - sir_builder_functions.cpp                      │    │
        │  │  - sir_builder_match_patterns.cpp                 │    │
        │  │  - sir_builder_scope_loop.cpp                     │    │
        │  │  - builders/* (sub-builders)                      │    │
        │  └──────────────────────────────────────────────────┘    │
        └──────────────────┬───────────────────────────────────────┘
                           ▼
        ┌──────────────────────────────────────────────────────────┐
        │         compiler/src/sir_optimizer/   (sad_optimizer)     │
        │  ┌──────────────────────────────────────────────────┐    │
        │  │ Passes على SIR:                                   │    │
        │  │  constant_folding · dce · cse · inlining · licm   │    │
        │  │  sroa · peephole · branch_folding · tail_call     │    │
        │  │  copy_propagation · strength_reduction · ...      │    │
        │  └──────────────────────────────────────────────────┘    │
        └──────────────────┬───────────────────────────────────────┘
                           ▼
        ┌──────────────────────────────────────────────────────────┐
        │     compiler/src/backend/    (sad_llvm_backend)           │
        │  ┌──────────────────────────────────────────────────┐    │
        │  │  backend/llvm/   ← قلب LLVM codegen (37 ملف)      │    │
        │  │   - llvm_codegen_*  (instructions, init, context) │    │
        │  │   - llvm_type_mapper, llvm_runtime, llvm_target   │    │
        │  │   - arabic_*  (تحسينات خاصة بالعربية)              │    │
        │  │   - llvm_kernel_sync, llvm_paging  (kernel mode)  │    │
        │  └──────────────────────────────────────────────────┘    │
        │  ┌──────────────────────────────────────────────────┐    │
        │  │  backend/   ← مكونات backend الأخرى (27 ملف)      │    │
        │  │   ┌─ FFI:        c_abi, ffi_wrapper, solidity_ffi│    │
        │  │   ┌─ Codegen:    pattern, async, extension       │    │
        │  │   ┌─ Pipeline:   pipeline_chain, pipeline_codegen│    │
        │  │   ┌─ Targets:    arm64_codegen, entry_point      │    │
        │  │   ┌─ Embedded:   interrupt_handlers, panic       │    │
        │  │   ┌─ Linker:     linker_script, inline_asm       │    │
        │  │   └─ Emitters:   html, compose                   │    │
        │  └──────────────────────────────────────────────────┘    │
        └──────────────────┬───────────────────────────────────────┘
                           ▼
                    ┌──────────────────────┐
                    │  ملف تنفيذي (.exe/.elf)│
                    └──────────────────────┘
```

### 1.3 المشاكل المُكتشفة (Inventory)

| # | المشكلة | المخالفة | الدليل |
|---|---|---|---|
| **P-1** | `interrupt_handlers_codegen.cpp` يتيم (لا يُبنى) | CW-19, BF-09 | غير موجود في `BACKEND_COMMON_SOURCES` |
| **P-2** | `pipeline_codegen_optimizer.cpp` يتيم (لا يُبنى) | CW-19, BF-09 | غير موجود في `BACKEND_COMMON_SOURCES` |
| **P-3** | ازدواجية محتملة: `c_abi.cpp` ↔ `c_abi_codegen.cpp` | CW-03 | كلاهما يُبنى — يحتاج فحص محتوى |
| **P-4** | ازدواجية محتملة: `inline_asm.cpp` ↔ `llvm/llvm_inline_asm.cpp` | CW-03 | الأول في backend/ والثاني في backend/llvm/ |
| **P-5** | ازدواجية محتملة: `interrupt_handlers.cpp` ↔ `llvm/llvm_interrupt_handlers.cpp` | CW-03 | نفس النمط |
| **P-6** | ازدواجية محتملة: `entry_point.cpp` ↔ `llvm/llvm_entry_point.cpp` | CW-03 | نفس النمط |
| **P-7** | `backend/` يخلط مفاهيم: FFI + Codegen + Linker + Emitters + Embedded | CW-03 (Separation) | كل شيء في مجلد واحد بدون مجلدات فرعية |
| **P-8** | `CMakeLists.txt` 850 سطر — يحوي تعليقات أرشفة قديمة | CW-05 | الملف يجب < 800 سطر |
| **P-9** | `compiler/include/` فيه مجلدات بدون `compiler/src/` مقابل: `crypto/`, `ffi/`, `format/`, `kernel/`, `packages/`, `runtime/`, `suggestions/`, `testing/` | CW-04 | عدم تناسق بين include/src |
| **P-10** | `compiler/src/comptime/` و `compiler/src/build/` ✅ **حُذفا للتو** | — | تم |

---

## 2. تحليل الازدواجيات في backend/ (P-3 → P-6)

### الفرضية: ملفات `XXX.cpp` في `backend/` هي **stub قديم**، وملفات `llvm_XXX.cpp` في `backend/llvm/` هي التنفيذ الحقيقي.

```
backend/                            backend/llvm/
├── c_abi.cpp                      ─┐
├── c_abi_codegen.cpp              ─┼──?── (لا يوجد llvm_c_abi.cpp — قد لا تكون ازدواجية)
├── inline_asm.cpp                  ─┼──?── llvm_inline_asm.cpp
├── interrupt_handlers.cpp         ─┼──?── llvm_interrupt_handlers.cpp
├── entry_point.cpp                 ─┼──?── llvm_entry_point.cpp
├── async_codegen.cpp               │
├── pattern_codegen.cpp             │     (لا مقابل في llvm/)
├── pattern_codegen_api.cpp         │
├── extension_codegen.cpp           │
└── ...                             ─┘
```

**الإجراء:** تحليل diff لكل زوج محتمل قبل أي حذف.

---

## 3. الهيكل المُقترح بعد إعادة الهيكلة

### 3.1 شجرة الهدف

```
compiler/
├── CMakeLists.txt                          ← مختصر إلى < 400 سطر
├── cmake/                                   ← جديد: تجزئة CMake
│   ├── frontend.cmake
│   ├── optimizer.cmake
│   ├── backend.cmake
│   ├── llvm_backend.cmake
│   └── tests.cmake
│
├── include/
│   ├── frontend/        ← (كما هو، منظم)
│   ├── optimizer/       ← rename من sir_optimizer/ للتوحيد
│   ├── backend/
│   │   ├── ffi/                            ← جديد
│   │   ├── codegen/                        ← جديد
│   │   ├── targets/                        ← جديد
│   │   ├── embedded/                       ← جديد
│   │   ├── pipeline/                       ← جديد
│   │   ├── emitters/                       ← جديد
│   │   └── llvm/                           ← LLVM-specific
│   └── types/                              ← rename للتوحيد
│
└── src/
    ├── frontend/        ← (كما هو)
    ├── optimizer/       ← rename
    ├── types/
    └── backend/
        ├── ffi/                            ← c_abi, ffi_wrapper, solidity_ffi
        ├── codegen/                        ← pattern, async, extension
        ├── targets/                        ← arm64, entry_point
        ├── embedded/                       ← interrupt_handlers, panic, inline_asm
        ├── pipeline/                       ← pipeline_chain*, pipeline_codegen
        ├── emitters/                       ← html, compose
        ├── linker/                         ← linker_script*
        └── llvm/                           ← (كما هو، 37 ملف)
```

### 3.2 خريطة طبقات الهدف

```
┌─────────────────────────────────────────────────────────────────┐
│  المصدر .ص                                                       │
└────────────────────────┬────────────────────────────────────────┘
                         ▼
┌─────────────────────────────────────────────────────────────────┐
│  Lexer → Parser → AST              (shared/)                    │
└────────────────────────┬────────────────────────────────────────┘
                         ▼
┌─────────────────────────────────────────────────────────────────┐
│  SIR Builder                       (compiler/src/frontend/)     │
│  AST → SIR (Sad Intermediate Representation)                    │
└────────────────────────┬────────────────────────────────────────┘
                         ▼
┌─────────────────────────────────────────────────────────────────┐
│  SIR Optimizer                     (compiler/src/optimizer/)    │
│  Passes: const-fold, DCE, CSE, inlining, ...                    │
└────────────────────────┬────────────────────────────────────────┘
                         ▼
┌─────────────────────────────────────────────────────────────────┐
│  Backend Pipeline                  (compiler/src/backend/)       │
│                                                                  │
│   ┌────────────┐  ┌────────────┐  ┌────────────┐               │
│   │  codegen/  │  │ pipeline/  │  │ embedded/  │               │
│   └─────┬──────┘  └─────┬──────┘  └─────┬──────┘               │
│         │               │                │                       │
│         └───────────────┼────────────────┘                       │
│                         ▼                                        │
│         ┌──────────────────────────────────┐                    │
│         │  backend/llvm/                    │                    │
│         │  SIR → LLVM IR → Object File      │                    │
│         └──────────────┬───────────────────┘                    │
│                        ▼                                         │
│         ┌──────────────────────────────────┐                    │
│         │  ffi/  +  emitters/  +  linker/  │                    │
│         │  (Output formatters)              │                    │
│         └──────────────────────────────────┘                    │
└────────────────────────┬────────────────────────────────────────┘
                         ▼
                ┌────────────────┐
                │  ملف تنفيذي     │
                └────────────────┘
```

### 3.3 خريطة الاعتماديات بين المكتبات

```
                    sad_security_core (shared)
                            ▲
                            │
        ┌───────────────────┼───────────────────┐
        │                   │                   │
   sad_frontend        sad_optimizer       sad_llvm_backend
        ▲                   ▲                   ▲
        │                   │                   │
        └───────────────────┴───────────────────┘
                            │
                            ▼
                       sad_compiler
                       (INTERFACE)
                            ▲
                            │
                  ┌─────────┴─────────┐
                  │                   │
                sad-build           sad-run
                (sadc)              (sad)
```

---

## 4. خطة التنفيذ المرحلية

### المرحلة 1: تنظيف فوري آمن (لا مخاطر)
- [x] حذف `compiler/src/comptime/` ✅ مُنفّذ
- [x] حذف `compiler/src/build/` ✅ مُنفّذ
- [ ] حذف `interrupt_handlers_codegen.cpp` (يتيم)
- [ ] حذف `pipeline_codegen_optimizer.cpp` (يتيم)
- [ ] التحقق: `cmake --build` ينجح بعد كل حذف

### المرحلة 2: تحليل الازدواجيات
- [ ] diff بين `c_abi.cpp` و `c_abi_codegen.cpp`
- [ ] diff بين `inline_asm.cpp` و `llvm/llvm_inline_asm.cpp`
- [ ] diff بين `interrupt_handlers.cpp` و `llvm/llvm_interrupt_handlers.cpp`
- [ ] diff بين `entry_point.cpp` و `llvm/llvm_entry_point.cpp`
- [ ] قرار: دمج / حذف / إبقاء لكل زوج

### المرحلة 3: إعادة تنظيم backend/ بمجلدات فرعية
- [ ] إنشاء المجلدات الفرعية الستة
- [ ] نقل الملفات (مع تحديث `#include` المتأثرة)
- [ ] تحديث `CMakeLists.txt` لاستخدام `file(GLOB_RECURSE)` على المجلدات الجديدة
- [ ] التحقق: `cmake --build` ينجح

### المرحلة 4: تجزئة CMakeLists.txt
- [ ] استخراج `frontend`, `optimizer`, `backend` إلى ملفات `cmake/*.cmake`
- [ ] الإبقاء على `compiler/CMakeLists.txt` كـ orchestrator فقط

### المرحلة 5: تنظيف include/
- [ ] فحص المجلدات بدون مقابل src/ (P-9): هل هي headers-only legitimate أم مهجورة؟

---

## 5. مقاييس النجاح

| المقياس | قبل | الهدف |
|---|---|---|
| ملفات يتيمة في `backend/` | 2 | 0 |
| حجم `compiler/CMakeLists.txt` | 850 سطر | < 400 سطر |
| مجلدات فرعية في `backend/` | 1 (`llvm/`) | 7 |
| ازدواجيات مشبوهة | 4 محتملة | 0 |
| Build time (Debug) | الحالي | ≤ الحالي |
| اختبارات dual-execution | الحالي | الحالي + 0 تراجع |

---

## 6. المخاطر والتخفيف

| الخطر | احتمال | التخفيف |
|---|---|---|
| كسر `#include` بعد نقل الملفات | عالٍ | استخدام `target_include_directories` بمسارات public |
| فقد رمز يُستخدم بشكل ضمني | متوسط | اختبار build كامل بعد كل خطوة (BF-29) |
| ازدواجية تكون فعلاً متعمدة (واحدة للمفسر والأخرى للمترجم) | منخفض | فحص dependency graph قبل الحذف |
| تأثر مشروع `sad_mobile` | منخفض | تم بالفعل تنظيفه من `comptime`/`build_system` |

---

## 7. الخطوة الفورية المطلوبة

تنفيذ **المرحلة 1** فقط الآن (حذف الملفين اليتيمين) ثم العودة لاتخاذ القرار حول المراحل 2-5.
