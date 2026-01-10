# المرحلة 2: تحليل مجلد compiler

## 📅 التاريخ: 8 يناير 2026

---

## 📊 الوضع الحالي للمجلدات

### 1. compiler/frontend/ (11 ملف)

| المجلد | الملفات |
|--------|---------|
| include/ | sir_builder.h, sir_instruction.h, sir_module.h, sir_types.h, type_info.h |
| src/ | sir_builder.cpp, sir_instruction.cpp, sir_module.cpp, sir_module_test.cpp, sir_types.cpp, type_info.cpp |

### 2. compiler/optimizer/ (16 ملف)

| المجلد | الملفات |
|--------|---------|
| include/ | optimizer.h, pass.h, pass_manager.h, constant_folding_pass.h, copy_propagation_pass.h, cse_pass.h, dead_code_elimination_pass.h, register_coalescing_pass.h |
| src/ | optimizer.cpp, pass.cpp, pass_manager.cpp, constant_folding_pass.cpp, copy_propagation_pass.cpp, cse_pass.cpp, dead_code_elimination_pass.cpp, register_coalescing_pass.cpp |

### 3. compiler/type_system/ (34 ملف)

| المجلد | الملفات |
|--------|---------|
| include/ (17) | constraint.h, constraint_solver.h, generic_instantiation.h, generic_type.h, optional_type.h, primitive_type.h, substitution.h, type.h, type_constraint.h, type_guard.h, type_inferencer.h, type_narrowing.h, type_parameter.h, type_registry.h, type_variable.h, unification.h, union_type.h |
| src/ (17) | نفس الملفات بامتداد .cpp |

### 4. compiler/jit/ (14 ملف)

| المجلد | الملفات |
|--------|---------|
| include/ (7) | hot_path_detector.h, jit_bridge.h, jit_cache.h, jit_engine.h, jit_hash.h, jit_metrics.h, jit_profiler.h |
| src/ (7) | نفس الملفات بامتداد .cpp |

### 5. compiler/bytecode/ (6 ملفات)

| المجلد | الملفات |
|--------|---------|
| include/ | bytecode.h, emitter.h, opcodes.h |
| src/ | bytecode.cpp, emitter.cpp, opcodes.cpp |

### 6. compiler/pipeline/ (فارغ)

- include/ - فارغ
- src/ - فارغ
- llvm/ - (سيُحذف - تم نقله في المرحلة 1)

### 7. compiler/backend/ و compiler/backends/ (شبه فارغة)

تم نقل الملفات في المرحلة 1.

---

## 📊 ملخص الملفات

| المكون | Headers | Sources | المجموع |
|--------|---------|---------|---------|
| frontend | 5 | 6 | 11 |
| optimizer | 8 | 8 | 16 |
| type_system | 17 | 17 | 34 |
| jit | 7 | 7 | 14 |
| bytecode | 3 | 3 | 6 |
| **المجموع** | **40** | **41** | **81** |

---

## 🎯 خطة التنظيم

نظراً لأن المرحلة 1 نجحت وتم بناء LLVM في `src/compiler/llvm/`، 
سنقوم بنقل بقية المكونات إلى `src/compiler/` بهيكل واضح:

```
src/compiler/
├── llvm/           # ✅ تم (المرحلة 1)
├── frontend/       # SIR و type_info
├── optimizer/      # المحسّن
├── type_system/    # نظام الأنواع
├── jit/            # محرك JIT
└── bytecode/       # Bytecode
```

---

## ✅ الإجراء المطلوب

نقل المكونات مع الحفاظ على هيكل include/src لكل مكون.
