# المرحلة 7: تحديث CMakeLists.txt الرئيسي

## 🎯 الهدف

تحديث ملف CMakeLists.txt الرئيسي ليعكس الهيكل الجديد للمشروع.

---

## 📍 الوضع الحالي

- ملف CMakeLists.txt كبير ومعقد (~1928 سطر)
- يحتوي على مسارات قديمة
- بعض الأقسام معطلة
- خلط بين التكوينات المختلفة

---

## 📁 الهيكل الجديد للمشروع (بعد المراحل 1-6)

```
s_language/
├── src/
│   ├── core/               # المرحلة 3
│   ├── compiler/           # المرحلة 1 + 2
│   ├── runtime/            # المرحلة 4
│   ├── interpreter/        # المرحلة 3
│   ├── optimizer/          # المرحلة 3
│   ├── low_level/          # المرحلة 3
│   └── main.cpp
├── stdlib/                  # المرحلة 5
├── tests/                   # المرحلة 6
├── tools/
├── docs/
├── examples/
└── CMakeLists.txt
```

---

## 📝 هيكل CMakeLists.txt الجديد المقترح

```cmake
# ======================================================================
# CMakeLists.txt - لغة ص البرمجية / Sad Programming Language
# ======================================================================
cmake_minimum_required(VERSION 3.15)

project(SadLanguage 
    VERSION 1.0.0
    DESCRIPTION "لغة ص - لغة برمجة عربية حديثة"
    LANGUAGES CXX
)

# ======================================================================
# الإعدادات العامة / General Settings
# ======================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# مسارات الإخراج / Output paths
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ======================================================================
# الخيارات / Options
# ======================================================================
option(BUILD_TESTS "بناء الاختبارات" ON)
option(BUILD_BENCHMARKS "بناء قياسات الأداء" OFF)
option(BUILD_TOOLS "بناء الأدوات" ON)
option(ENABLE_LLVM "تفعيل LLVM" ON)

# ======================================================================
# التبعيات / Dependencies
# ======================================================================
include(cmake/dependencies.cmake)

# ======================================================================
# مسارات التضمين / Include Directories
# ======================================================================
include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/stdlib
)

# ======================================================================
# المكتبات الفرعية / Sub-libraries
# ======================================================================

# === Core Library ===
add_subdirectory(src/core)

# === Compiler Library ===
add_subdirectory(src/compiler)

# === Runtime Library ===
add_subdirectory(src/runtime)

# === Interpreter Library ===
add_subdirectory(src/interpreter)

# === Stdlib Library ===
add_subdirectory(stdlib)

# ======================================================================
# البرنامج الرئيسي / Main Executable
# ======================================================================
add_executable(sad src/main.cpp)
target_link_libraries(sad PRIVATE
    sad_core
    sad_compiler
    sad_runtime
    sad_interpreter
    sad_stdlib
)

# ======================================================================
# الأدوات / Tools
# ======================================================================
if(BUILD_TOOLS)
    add_subdirectory(tools/lsp)
    add_subdirectory(tools/repl)
    add_subdirectory(tools/pkg)
    add_subdirectory(tools/docgen)
endif()

# ======================================================================
# الاختبارات / Tests
# ======================================================================
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ======================================================================
# قياسات الأداء / Benchmarks
# ======================================================================
if(BUILD_BENCHMARKS)
    add_subdirectory(tests/benchmarks)
endif()

# ======================================================================
# معلومات البناء / Build Info
# ======================================================================
message(STATUS "====================================")
message(STATUS "لغة ص البرمجية / Sad Language")
message(STATUS "====================================")
message(STATUS "الإصدار: ${PROJECT_VERSION}")
message(STATUS "المترجم: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "LLVM: ${ENABLE_LLVM}")
message(STATUS "====================================")
```

---

## 📁 ملفات CMake الفرعية

### src/core/CMakeLists.txt

```cmake
# ======================================================================
# Core Library
# ======================================================================

set(CORE_SOURCES
    # Lexer
    lexer/lexer_core.cpp
    lexer/lexer_keywords.cpp
    lexer/token.cpp
    
    # Parser
    parser/parser_core.cpp
    parser/parser_core_helpers.cpp
    parser/parser_core_impl.cpp
    parser/parser_core_oop.cpp
    parser/parser_modules.cpp
    
    # AST
    ast/ast_node.cpp
    ast/declarations.cpp
    ast/expressions.cpp
    ast/statements.cpp
    ast/type_nodes.cpp
    ast/module_nodes.cpp
    ast/property_nodes.cpp
    
    # Modules
    modules/module.cpp
    modules/module_loader.cpp
    modules/module_cache.cpp
    modules/module_resolver.cpp
    modules/module_validator.cpp
    modules/dependency_graph.cpp
    modules/symbol_resolver.cpp
    modules/search_paths.cpp
    
    # Utils
    utils/string_utils.cpp
)

add_library(sad_core STATIC ${CORE_SOURCES})

target_include_directories(sad_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
```

### src/compiler/CMakeLists.txt

```cmake
# ======================================================================
# Compiler Library
# ======================================================================

set(COMPILER_SOURCES
    # Frontend (SIR)
    frontend/sir/sir_builder.cpp
    frontend/sir/sir_instruction.cpp
    frontend/sir/sir_module.cpp
    frontend/sir/sir_types.cpp
    frontend/type_checker/type_info.cpp
    
    # Middle (Optimizer)
    middle/optimizer/optimizer.cpp
    middle/optimizer/pass_manager.cpp
    middle/optimizer/pass.cpp
    middle/optimizer/passes/constant_folding_pass.cpp
    middle/optimizer/passes/dead_code_elimination_pass.cpp
    middle/optimizer/passes/copy_propagation_pass.cpp
    middle/optimizer/passes/cse_pass.cpp
    
    # Middle (Type System)
    middle/type_system/type_registry.cpp
    middle/type_system/types/primitive_type.cpp
    middle/type_system/types/generic_type.cpp
    middle/type_system/types/union_type.cpp
    middle/type_system/types/optional_type.cpp
    middle/type_system/inference/type_inferencer.cpp
    middle/type_system/inference/type_narrowing.cpp
    middle/type_system/constraints/constraint.cpp
    middle/type_system/constraints/constraint_solver.cpp
    
    # Backend (Bytecode)
    backend/bytecode/bytecode_file.cpp
    
    # JIT
    jit/engine/jit_engine.cpp
    jit/engine/jit_cache.cpp
    jit/profiler/jit_profiler.cpp
    jit/profiler/jit_metrics.cpp
    jit/profiler/hot_path_detector.cpp
    jit/bridge/jit_bridge.cpp
)

# LLVM sources (if enabled)
if(ENABLE_LLVM AND LLVM_FOUND)
    list(APPEND COMPILER_SOURCES
        backend/llvm/src/llvm_codegen.cpp
        backend/llvm/src/llvm_generator.cpp
        backend/llvm/src/llvm_optimizer.cpp
        backend/llvm/src/llvm_type_mapper.cpp
        backend/llvm/src/llvm_array_support.cpp
        backend/llvm/src/llvm_class_support.cpp
        backend/llvm/src/llvm_dict_support.cpp
        backend/llvm/src/llvm_control_flow.cpp
        backend/llvm/src/llvm_expression_builder.cpp
        backend/llvm/src/llvm_memory_manager.cpp
        backend/llvm/src/llvm_runtime.cpp
        backend/llvm/src/llvm_linker.cpp
        backend/llvm/src/llvm_target.cpp
        backend/llvm/src/llvm_debug.cpp
        backend/llvm/src/arabic_optimizer.cpp
        backend/llvm/src/toolchain_detection.cpp
    )
endif()

add_library(sad_compiler STATIC ${COMPILER_SOURCES})

target_include_directories(sad_compiler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if(ENABLE_LLVM AND LLVM_FOUND)
    target_link_libraries(sad_compiler PUBLIC ${LLVM_LIBRARIES})
    target_compile_definitions(sad_compiler PUBLIC ENABLE_LLVM_BACKEND)
endif()
```

### src/runtime/CMakeLists.txt

```cmake
# ======================================================================
# Runtime Library
# ======================================================================

set(RUNTIME_SOURCES
    # VM
    vm/vm.cpp
    vm/vm_gc.cpp
    vm/vm_helpers.cpp
    vm/vm_opcodes.cpp
    vm/bytecode/bytecode_file.cpp
    
    # Memory
    memory/allocator.cpp
    memory/gc.cpp
    
    # Thread
    thread/thread.cpp
    
    # FFI
    ffi/ffi.cpp
    
    # Exception
    exception/exception.cpp
)

add_library(sad_runtime STATIC ${RUNTIME_SOURCES})

target_include_directories(sad_runtime PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
```

### stdlib/CMakeLists.txt

```cmake
# ======================================================================
# Standard Library
# ======================================================================

set(STDLIB_SOURCES
    # Core
    core/builtins.cpp
    core/array_functions.cpp
    core/type_functions.cpp
    core/other_functions.cpp
    core/stdlib_manager.cpp
    
    # IO
    io/io_functions.cpp
    
    # Math
    math/math_functions.cpp
    
    # String
    string/string_functions.cpp
    
    # Filesystem
    filesystem/filesystem_module.cpp
    filesystem/filesystem_builtins.cpp
    
    # Network
    network/core/socket_base.cpp
    network/core/socket_address.cpp
    network/core/network_error.cpp
    network/tcp/tcp_socket.cpp
    network/udp/udp_socket.cpp
    network/http/http_module.cpp
    network/http/http_builtins.cpp
    network/bindings/network_bindings.cpp
    network/bindings/http_bindings.cpp
    
    # JSON
    json/json_module.cpp
    json/json_builtins.cpp
    
    # XML
    xml/xml_module.cpp
    xml/xml_builtins.cpp
    
    # Database
    database/database_module.cpp
    database/database_builtins.cpp
    
    # Crypto
    crypto/crypto_module.cpp
    crypto/crypto_builtins.cpp
    
    # Image
    image/image_module.cpp
    image/image_builtins.cpp
    
    # Graphics
    graphics/graphics_module.cpp
    graphics/graphics_builtins.cpp
)

add_library(sad_stdlib STATIC ${STDLIB_SOURCES})

target_include_directories(sad_stdlib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Platform-specific linking
if(WIN32)
    target_link_libraries(sad_stdlib PUBLIC ws2_32)
endif()
```

---

## 📁 ملف cmake/dependencies.cmake

```cmake
# ======================================================================
# Dependencies Configuration
# ======================================================================

# LLVM
if(ENABLE_LLVM)
    set(LLVM_DIR "C:/LLVM/lib/cmake/llvm" CACHE PATH "LLVM CMake dir")
    find_package(LLVM CONFIG)
    
    if(LLVM_FOUND)
        message(STATUS "✅ Found LLVM ${LLVM_PACKAGE_VERSION}")
        include_directories(${LLVM_INCLUDE_DIRS})
        link_directories(${LLVM_LIBRARY_DIRS})
        add_definitions(${LLVM_DEFINITIONS})
        
        llvm_map_components_to_libnames(LLVM_LIBRARIES
            Core Support ExecutionEngine MCJIT OrcJIT
            RuntimeDyld Target X86 native
            MC CodeGen AsmParser AsmPrinter
        )
    else()
        message(WARNING "⚠ LLVM not found, disabling LLVM backend")
        set(ENABLE_LLVM OFF)
    endif()
endif()

# SQLite3
find_package(SQLite3)
if(SQLite3_FOUND)
    message(STATUS "✅ Found SQLite3")
endif()

# OpenSSL
find_package(OpenSSL)
if(OpenSSL_FOUND)
    message(STATUS "✅ Found OpenSSL")
endif()

# GoogleTest (for tests)
if(BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()
```

---

## 📋 خطوات التنفيذ

### الخطوة 1: إنشاء مجلد cmake

```powershell
New-Item -ItemType Directory -Path "C:\s\s_language\cmake" -Force
```

### الخطوة 2: إنشاء ملف dependencies.cmake

```powershell
# إنشاء ملف التبعيات
# (المحتوى موضح أعلاه)
```

### الخطوة 3: إنشاء ملفات CMakeLists.txt الفرعية

- `src/core/CMakeLists.txt`
- `src/compiler/CMakeLists.txt`
- `src/runtime/CMakeLists.txt`
- `src/interpreter/CMakeLists.txt`
- `stdlib/CMakeLists.txt`
- `tests/CMakeLists.txt`

### الخطوة 4: تحديث CMakeLists.txt الرئيسي

- استبدال المحتوى القديم بالهيكل الجديد
- التأكد من صحة المسارات

### الخطوة 5: الاختبار

```powershell
# تنظيف البناء القديم
Remove-Item -Recurse -Force "C:\s\s_language\build"

# إعادة التكوين
cmake -B build -DCMAKE_BUILD_TYPE=Release

# البناء
cmake --build build --config Release

# تشغيل الاختبارات
ctest --test-dir build -C Release
```

---

## ✅ قائمة التحقق النهائية

- [ ] إنشاء مجلد cmake
- [ ] إنشاء dependencies.cmake
- [ ] إنشاء CMakeLists.txt لـ src/core
- [ ] إنشاء CMakeLists.txt لـ src/compiler
- [ ] إنشاء CMakeLists.txt لـ src/runtime
- [ ] إنشاء CMakeLists.txt لـ src/interpreter
- [ ] إنشاء CMakeLists.txt لـ stdlib
- [ ] إنشاء CMakeLists.txt لـ tests
- [ ] تحديث CMakeLists.txt الرئيسي
- [ ] تكوين CMake بنجاح
- [ ] بناء المشروع بنجاح
- [ ] تشغيل الاختبارات بنجاح
- [ ] التحقق من عدم وجود تحذيرات أو أخطاء

---

## ⚠️ ملاحظات هامة

1. **النسخ الاحتياطي**: احتفظ بنسخة من CMakeLists.txt القديم
2. **التدرج**: يمكن تطبيق التغييرات تدريجياً
3. **Git**: استخدم branches منفصلة للتجربة
4. **CI/CD**: تأكد من تحديث ملفات CI/CD إذا وجدت

---

## 🎉 الخلاصة

بعد إتمام جميع المراحل السبع، سيكون المشروع منظماً بالشكل التالي:

```
s_language/
├── cmake/               # ملفات CMake المساعدة
├── src/                 # الكود المصدري (موحد)
│   ├── core/           # النواة
│   ├── compiler/       # المترجم
│   ├── runtime/        # بيئة التشغيل
│   ├── interpreter/    # المفسر
│   └── main.cpp
├── stdlib/             # المكتبة القياسية
├── tests/              # الاختبارات
│   ├── unit/
│   ├── integration/
│   ├── e2e/
│   └── benchmarks/
├── tools/              # الأدوات
├── docs/               # التوثيق
├── examples/           # الأمثلة
└── CMakeLists.txt      # البناء الرئيسي
```

---

## 🔙 الملفات السابقة

- [نظرة عامة](00_overview.md)
- [المرحلة 1: توحيد LLVM](01_phase1_llvm_consolidation.md)
- [المرحلة 2: تنظيم compiler](02_phase2_compiler_reorganization.md)
- [المرحلة 3: توحيد include/src](03_phase3_include_src_unification.md)
- [المرحلة 4: دمج runtime و vm](04_phase4_runtime_vm_merge.md)
- [المرحلة 5: تنظيف stdlib](05_phase5_stdlib_cleanup.md)
- [المرحلة 6: تنظيم الاختبارات](06_phase6_tests_organization.md)
