# المرحلة 3: إعادة تنظيم مجلد المترجم (Compiler)

## 🎯 الهدف

تنظيم جميع ملفات المترجم في مجلد `compiler/` واحد يشمل:
- Frontend (SIR Builder)
- Middle (Optimizer)
- Backend (LLVM Codegen)
- JIT (Just-In-Time)
- Bytecode

---

## 📍 الملفات الحالية للمترجم

### مجلد compiler/ الحالي

```
compiler/
├── frontend/
│   ├── include/ (sir_builder.h, sir_instruction.h, sir_module.h, sir_types.h, ...)
│   └── src/ (sir_builder.cpp, ...)
│
├── backends/
│   └── llvm/
│       ├── include/ (llvm_codegen.h, llvm_context.h, llvm_function_codegen.h, ...)
│       └── src/ (~25 ملف .cpp)
│
├── jit/
│   ├── include/ (jit_compiler.h, jit_context.h, jit_execution_engine.h, ...)
│   └── src/ (jit_*.cpp files)
│
├── optimizer/
│   ├── include/ (optimization_pass.h, pass_manager.h, ...)
│   └── src/ (*.cpp files)
│
├── type_system/
│   ├── include/ (type_checker.h, ...)
│   └── src/ (*.cpp files)
│
├── bytecode/
│   ├── include/ (bytecode_generator.h, opcodes.h)
│   └── src/ (bytecode_generator.cpp)
│
├── backend/
│   └── ... (ملفات قديمة)
│
├── pipeline/
│   └── ... (ملفات pipeline)
│
└── src/
    └── ... (ملفات متفرقة)
```

---

## 📁 الهيكل الجديد المقترح

```
compiler/
├── include/
│   ├── frontend/
│   │   ├── sir_builder.h
│   │   ├── sir_instruction.h
│   │   ├── sir_module.h
│   │   ├── sir_types.h
│   │   └── sir_visitor.h
│   │
│   ├── middle/
│   │   ├── optimization_pass.h
│   │   ├── pass_manager.h
│   │   ├── constant_folding.h
│   │   ├── dead_code_elimination.h
│   │   └── inline_pass.h
│   │
│   ├── backend/
│   │   └── llvm/
│   │       ├── llvm_codegen.h
│   │       ├── llvm_context.h
│   │       ├── llvm_function_codegen.h
│   │       ├── llvm_expression_codegen.h
│   │       ├── llvm_type_converter.h
│   │       ├── llvm_string_builder.h
│   │       ├── llvm_debug_info.h
│   │       └── llvm_module_builder.h
│   │
│   ├── jit/
│   │   ├── jit_compiler.h
│   │   ├── jit_context.h
│   │   ├── jit_execution_engine.h
│   │   ├── jit_optimizer.h
│   │   └── jit_memory_manager.h
│   │
│   ├── bytecode/
│   │   ├── bytecode_generator.h
│   │   ├── bytecode_file.h
│   │   └── opcodes.h
│   │
│   └── types/
│       ├── type_checker.h
│       └── type_inference.h
│
├── src/
│   ├── frontend/
│   │   ├── sir_builder.cpp
│   │   ├── sir_builder_functions.cpp
│   │   ├── sir_builder_expressions.cpp
│   │   ├── sir_builder_statements.cpp
│   │   ├── sir_builder_patterns.cpp
│   │   ├── sir_instruction.cpp
│   │   └── sir_module.cpp
│   │
│   ├── middle/
│   │   ├── pass_manager.cpp
│   │   ├── constant_folding.cpp
│   │   ├── dead_code_elimination.cpp
│   │   └── inline_pass.cpp
│   │
│   ├── backend/
│   │   └── llvm/
│   │       ├── llvm_codegen.cpp
│   │       ├── llvm_context.cpp
│   │       ├── llvm_function_codegen.cpp
│   │       ├── llvm_expression_codegen.cpp
│   │       ├── llvm_statement_codegen.cpp
│   │       ├── llvm_type_converter.cpp
│   │       ├── llvm_string_builder.cpp
│   │       ├── llvm_builtin_codegen.cpp
│   │       ├── llvm_debug_info.cpp
│   │       ├── llvm_module_builder.cpp
│   │       ├── llvm_io_codegen.cpp
│   │       ├── llvm_runtime_io_gc.cpp
│   │       └── llvm_runtime_*.cpp
│   │
│   ├── jit/
│   │   ├── jit_compiler.cpp
│   │   ├── jit_context.cpp
│   │   ├── jit_execution_engine.cpp
│   │   ├── jit_optimizer.cpp
│   │   └── jit_memory_manager.cpp
│   │
│   ├── bytecode/
│   │   ├── bytecode_generator.cpp
│   │   └── bytecode_file.cpp
│   │
│   └── types/
│       ├── type_checker.cpp
│       └── type_inference.cpp
│
└── CMakeLists.txt
```

---

## 📋 خطوات التنفيذ

### الخطوة 3.1: إنشاء الهيكل الجديد

```powershell
$base = "C:\s\s_language\compiler_new"

# Include directories
New-Item -ItemType Directory -Path "$base\include\frontend" -Force
New-Item -ItemType Directory -Path "$base\include\middle" -Force
New-Item -ItemType Directory -Path "$base\include\backend\llvm" -Force
New-Item -ItemType Directory -Path "$base\include\jit" -Force
New-Item -ItemType Directory -Path "$base\include\bytecode" -Force
New-Item -ItemType Directory -Path "$base\include\types" -Force

# Source directories
New-Item -ItemType Directory -Path "$base\src\frontend" -Force
New-Item -ItemType Directory -Path "$base\src\middle" -Force
New-Item -ItemType Directory -Path "$base\src\backend\llvm" -Force
New-Item -ItemType Directory -Path "$base\src\jit" -Force
New-Item -ItemType Directory -Path "$base\src\bytecode" -Force
New-Item -ItemType Directory -Path "$base\src\types" -Force
```

### الخطوة 3.2: نسخ ملفات Frontend (SIR)

```powershell
# Headers
Copy-Item "compiler\frontend\include\*.h" -Destination "compiler_new\include\frontend\"

# Sources
Copy-Item "compiler\frontend\src\*.cpp" -Destination "compiler_new\src\frontend\"
```

### الخطوة 3.3: نسخ ملفات Middle (Optimizer)

```powershell
# Headers
Copy-Item "compiler\optimizer\include\*.h" -Destination "compiler_new\include\middle\"

# Sources
Copy-Item "compiler\optimizer\src\*.cpp" -Destination "compiler_new\src\middle\"
```

### الخطوة 3.4: نسخ ملفات Backend (LLVM)

```powershell
# Headers
Copy-Item "compiler\backends\llvm\include\*.h" -Destination "compiler_new\include\backend\llvm\"

# Sources
Copy-Item "compiler\backends\llvm\src\*.cpp" -Destination "compiler_new\src\backend\llvm\"
```

### الخطوة 3.5: نسخ ملفات JIT

```powershell
# Headers
Copy-Item "compiler\jit\include\*.h" -Destination "compiler_new\include\jit\"

# Sources
Copy-Item "compiler\jit\src\*.cpp" -Destination "compiler_new\src\jit\"
```

### الخطوة 3.6: نسخ ملفات Bytecode

```powershell
# من compiler/bytecode
Copy-Item "compiler\bytecode\include\*.h" -Destination "compiler_new\include\bytecode\"
Copy-Item "compiler\bytecode\src\*.cpp" -Destination "compiler_new\src\bytecode\"

# من bytecode/ الجذر
Copy-Item "bytecode\include\*.h" -Destination "compiler_new\include\bytecode\"
Copy-Item "bytecode\src\*.cpp" -Destination "compiler_new\src\bytecode\"
```

### الخطوة 3.7: نسخ ملفات Type System

```powershell
# Headers
Copy-Item "compiler\type_system\include\*.h" -Destination "compiler_new\include\types\"

# Sources
Copy-Item "compiler\type_system\src\*.cpp" -Destination "compiler_new\src\types\"
```

---

## 🔧 إنشاء CMakeLists.txt

### compiler/CMakeLists.txt

```cmake
# ============================================================================
# Compiler - المترجم
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# Find LLVM
find_package(LLVM 18 REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# ============================================================================
# Frontend Library (SIR)
# ============================================================================
add_library(sad_frontend STATIC
    src/frontend/sir_builder.cpp
    src/frontend/sir_builder_functions.cpp
    src/frontend/sir_builder_expressions.cpp
    src/frontend/sir_builder_statements.cpp
    src/frontend/sir_builder_patterns.cpp
    src/frontend/sir_instruction.cpp
    src/frontend/sir_module.cpp
)

target_include_directories(sad_frontend
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/shared/include
)

# ============================================================================
# Middle Library (Optimizer)
# ============================================================================
add_library(sad_optimizer STATIC
    src/middle/pass_manager.cpp
    src/middle/constant_folding.cpp
    src/middle/dead_code_elimination.cpp
    src/middle/inline_pass.cpp
)

target_include_directories(sad_optimizer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============================================================================
# Backend Library (LLVM)
# ============================================================================
add_library(sad_llvm_backend STATIC
    src/backend/llvm/llvm_codegen.cpp
    src/backend/llvm/llvm_context.cpp
    src/backend/llvm/llvm_function_codegen.cpp
    src/backend/llvm/llvm_expression_codegen.cpp
    src/backend/llvm/llvm_statement_codegen.cpp
    src/backend/llvm/llvm_type_converter.cpp
    src/backend/llvm/llvm_string_builder.cpp
    src/backend/llvm/llvm_builtin_codegen.cpp
    src/backend/llvm/llvm_debug_info.cpp
    src/backend/llvm/llvm_module_builder.cpp
    src/backend/llvm/llvm_io_codegen.cpp
    src/backend/llvm/llvm_runtime_io_gc.cpp
    # ... المزيد من الملفات حسب الحاجة
)

target_include_directories(sad_llvm_backend
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(sad_llvm_backend
    PUBLIC
        LLVM
        sad_frontend
        sad_optimizer
)

# ============================================================================
# JIT Library
# ============================================================================
add_library(sad_jit STATIC
    src/jit/jit_compiler.cpp
    src/jit/jit_context.cpp
    src/jit/jit_execution_engine.cpp
    src/jit/jit_optimizer.cpp
    src/jit/jit_memory_manager.cpp
)

target_include_directories(sad_jit
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${LLVM_INCLUDE_DIRS}
)

target_link_libraries(sad_jit
    PUBLIC
        sad_llvm_backend
)

# ============================================================================
# Bytecode Library
# ============================================================================
add_library(sad_bytecode STATIC
    src/bytecode/bytecode_generator.cpp
    src/bytecode/bytecode_file.cpp
)

target_include_directories(sad_bytecode
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============================================================================
# Type System Library
# ============================================================================
add_library(sad_type_system STATIC
    src/types/type_checker.cpp
    src/types/type_inference.cpp
)

target_include_directories(sad_type_system
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============================================================================
# Unified Compiler Library
# ============================================================================
add_library(sad_compiler INTERFACE)

target_link_libraries(sad_compiler
    INTERFACE
        sad_frontend
        sad_optimizer
        sad_llvm_backend
        sad_jit
        sad_bytecode
        sad_type_system
)
```

---

## 📊 ملخص الملفات

| المكون | Headers | Sources | المجموع |
|--------|---------|---------|---------|
| frontend (SIR) | ~10 | ~15 | ~25 |
| middle (optimizer) | ~8 | ~6 | ~14 |
| backend (LLVM) | ~15 | ~25 | ~40 |
| jit | ~6 | ~6 | ~12 |
| bytecode | ~3 | ~2 | ~5 |
| types | ~3 | ~3 | ~6 |
| **المجموع** | **~45** | **~57** | **~102** |

---

## 🔗 التبعيات

```
sad_compiler
├── sad_frontend → sad_shared
├── sad_optimizer → sad_frontend
├── sad_llvm_backend → sad_frontend, sad_optimizer, LLVM
├── sad_jit → sad_llvm_backend
├── sad_bytecode → sad_shared
└── sad_type_system → sad_shared
```

---

## ✅ قائمة التحقق

- [ ] إنشاء هيكل المجلدات الجديد
- [ ] نسخ ملفات Frontend
- [ ] نسخ ملفات Middle
- [ ] نسخ ملفات Backend (LLVM)
- [ ] نسخ ملفات JIT
- [ ] نسخ ملفات Bytecode
- [ ] نسخ ملفات Type System
- [ ] إنشاء CMakeLists.txt
- [ ] التحقق من روابط LLVM
- [ ] البناء والتحقق

---

## ⚠️ ملاحظات مهمة

1. **LLVM Path**: تأكد من تحديث مسار LLVM في CMakeLists.txt الرئيسي
2. **Include Paths**: قد تحتاج لتحديث مسارات #include في الملفات
3. **Circular Dependencies**: تجنب التبعيات الدائرية بين المكتبات

---

## ⏭️ الخطوة التالية

المرحلة 4: إعادة تنظيم runtime/
