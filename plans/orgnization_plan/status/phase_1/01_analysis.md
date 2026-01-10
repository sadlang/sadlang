# المرحلة 1: تحليل ملفات LLVM

## 📅 التاريخ: 8 يناير 2026

---

## 📊 تحليل الوضع الحالي

### الموقع 1: compiler/backends/llvm (31 ملف)

هذا هو الموقع الأكثر اكتمالاً ويحتوي على:

| نوع الملف | العدد | الملفات |
|-----------|-------|---------|
| Header (.h) | 15 | arabic_optimizer.h, llvm_array_support.h, llvm_class_support.h, llvm_codegen.h, llvm_control_flow.h, llvm_debug.h, llvm_dict_support.h, llvm_expression_builder.h, llvm_linker.h, llvm_memory_manager.h, llvm_optimizer.h, llvm_runtime.h, llvm_target.h, llvm_type_mapper.h, toolchain_detection.h |
| Source (.cpp) | 16 | arabic_optimizer.cpp, llvm_array_support.cpp, llvm_class_support.cpp, llvm_codegen.cpp, llvm_control_flow.cpp, llvm_debug.cpp, llvm_dict_support.cpp, llvm_expression_builder.cpp, llvm_linker.cpp, llvm_memory_manager.cpp, llvm_optimizer.cpp, llvm_runtime.cpp, llvm_runtime_io_gc.cpp, llvm_target.cpp, llvm_type_mapper.cpp, toolchain_detection.cpp |

### الموقع 2: compiler/backend/llvm (4 ملفات)

| المجلد | الملفات |
|--------|---------|
| include/ | llvm_context.h, llvm_generator.h |
| src/ | llvm_context.cpp, llvm_generator.cpp |

⚠️ **ملاحظة:** هذه الملفات قد تكون مكررة أو نسخ مختلفة!

### الموقع 3: compiler/pipeline/llvm (2 ملفات + CMakeLists.txt)

| المجلد | الملفات |
|--------|---------|
| include/ | llvm_compiler_pipeline.h |
| src/ | llvm_compiler_pipeline.cpp |

---

## 🔍 تحليل التكرار

### الملفات المحتمل تكرارها:

| الملف | الموقع 1 (backends) | الموقع 2 (backend) |
|-------|---------------------|-------------------|
| llvm_generator.h | ❌ | ✅ |
| llvm_generator.cpp | ❌ | ✅ |
| llvm_context.h | ❌ | ✅ |
| llvm_context.cpp | ❌ | ✅ |

### الخلاصة:
- **backends/llvm**: يحتوي على الملفات الأساسية للـ codegen والدعم
- **backend/llvm**: يحتوي على context و generator (مختلفين عن backends)
- **pipeline/llvm**: يحتوي على compiler pipeline

**لا يوجد تكرار فعلي** - كل موقع يحتوي على ملفات فريدة!

---

## 📁 الملفات الفريدة لكل موقع

### من backends/llvm (31 ملف):
- Arabic optimizer
- LLVM codegen core
- Type mapper
- Array/Class/Dict support
- Control flow
- Expression builder
- Memory manager
- Runtime
- Linker
- Target
- Debug
- Toolchain detection

### من backend/llvm (4 ملفات):
- llvm_context.h/cpp
- llvm_generator.h/cpp

### من pipeline/llvm (2 ملفات):
- llvm_compiler_pipeline.h/cpp

---

## ✅ الإجراء المطلوب

**المجموع الكلي:** 37 ملف LLVM سيتم توحيدها في:
```
src/compiler/llvm/
├── include/    (17 ملف .h)
└── src/        (20 ملف .cpp)
```

---

## 📋 الخطوة التالية

إنشاء الهيكل الجديد ونقل الملفات.
