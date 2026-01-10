# المرحلة 5: تحديث نظام البناء CMake

## 🎯 الهدف

تحديث ملفات CMake لتدعم الهيكل الجديد للمشروع مع الحفاظ على التوافقية.

---

## 📁 الهيكل النهائي للمشروع

```
s_language/
├── CMakeLists.txt              # الرئيسي
│
├── shared/                     # المكونات المشتركة
│   ├── CMakeLists.txt
│   └── ...
│
├── interpreter/                # المفسر
│   ├── CMakeLists.txt
│   └── ...
│
├── compiler/                   # المترجم
│   ├── CMakeLists.txt
│   └── ...
│
├── runtime/                    # وقت التشغيل
│   ├── CMakeLists.txt
│   └── ...
│
├── tools/                      # الأدوات
│   ├── CMakeLists.txt
│   ├── repl/
│   ├── lsp/
│   └── pkg/
│
├── graphics/                   # الرسوميات
│   └── CMakeLists.txt
│
├── stdlib/                     # المكتبة القياسية
│   └── ...
│
└── tests/                      # الاختبارات
    └── CMakeLists.txt
```

---

## 🔧 CMakeLists.txt الرئيسي

### CMakeLists.txt (الجذر)

```cmake
# ============================================================================
# لغة ص - Sad Programming Language
# ============================================================================

cmake_minimum_required(VERSION 3.15)
project(sad VERSION 1.2.0 LANGUAGES CXX)

# ============================================================================
# إعدادات المشروع
# ============================================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# مسار الإخراج
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ============================================================================
# خيارات البناء
# ============================================================================
option(SAD_BUILD_INTERPRETER "Build the interpreter" ON)
option(SAD_BUILD_COMPILER "Build the LLVM compiler" ON)
option(SAD_BUILD_JIT "Build JIT support" ON)
option(SAD_BUILD_GRAPHICS "Build graphics support" OFF)
option(SAD_BUILD_TOOLS "Build additional tools" ON)
option(SAD_BUILD_TESTS "Build tests" ON)

# ============================================================================
# البحث عن LLVM (إذا كان المترجم مفعل)
# ============================================================================
if(SAD_BUILD_COMPILER OR SAD_BUILD_JIT)
    set(LLVM_DIR "C:/Program Files/LLVM/lib/cmake/llvm")
    find_package(LLVM 18 REQUIRED CONFIG)
    
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
    
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
endif()

# ============================================================================
# المكونات الأساسية
# ============================================================================

# المكونات المشتركة (مطلوبة دائماً)
add_subdirectory(shared)

# وقت التشغيل
add_subdirectory(runtime)

# ============================================================================
# المكونات الاختيارية
# ============================================================================

# المفسر
if(SAD_BUILD_INTERPRETER)
    add_subdirectory(interpreter)
endif()

# المترجم
if(SAD_BUILD_COMPILER)
    add_subdirectory(compiler)
endif()

# الرسوميات
if(SAD_BUILD_GRAPHICS)
    add_subdirectory(graphics)
endif()

# الأدوات
if(SAD_BUILD_TOOLS)
    add_subdirectory(tools)
endif()

# الاختبارات
if(SAD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ============================================================================
# البرنامج الرئيسي
# ============================================================================
add_executable(sad
    src/main.cpp
)

target_link_libraries(sad
    PRIVATE
        sad_shared
        sad_interpreter
        $<$<BOOL:${SAD_BUILD_COMPILER}>:sad_compiler>
        $<$<BOOL:${SAD_BUILD_GRAPHICS}>:sad_graphics>
)

target_compile_definitions(sad
    PRIVATE
        $<$<BOOL:${SAD_BUILD_COMPILER}>:SAD_HAS_COMPILER>
        $<$<BOOL:${SAD_BUILD_JIT}>:SAD_HAS_JIT>
        $<$<BOOL:${SAD_BUILD_GRAPHICS}>:SAD_HAS_GRAPHICS>
)

# ============================================================================
# التثبيت
# ============================================================================
install(TARGETS sad
    RUNTIME DESTINATION bin
)

install(DIRECTORY stdlib/
    DESTINATION lib/sad/stdlib
)

# ============================================================================
# ملخص البناء
# ============================================================================
message(STATUS "")
message(STATUS "=== SAD Build Configuration ===")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Interpreter: ${SAD_BUILD_INTERPRETER}")
message(STATUS "Compiler: ${SAD_BUILD_COMPILER}")
message(STATUS "JIT: ${SAD_BUILD_JIT}")
message(STATUS "Graphics: ${SAD_BUILD_GRAPHICS}")
message(STATUS "Tools: ${SAD_BUILD_TOOLS}")
message(STATUS "Tests: ${SAD_BUILD_TESTS}")
message(STATUS "================================")
message(STATUS "")
```

---

## 📝 CMakeLists.txt للمكونات الفرعية

### shared/CMakeLists.txt

```cmake
# ============================================================================
# Shared Components - المكونات المشتركة
# ============================================================================

add_library(sad_shared STATIC
    # Lexer
    src/lexer/lexer.cpp
    src/lexer/token.cpp
    src/lexer/keywords.cpp
    
    # Parser
    src/parser/parser.cpp
    src/parser/parser_expressions.cpp
    src/parser/parser_statements.cpp
    src/parser/parser_declarations.cpp
    src/parser/parser_patterns.cpp
    
    # AST
    src/ast/ast_node.cpp
    src/ast/ast_expression.cpp
    src/ast/ast_statement.cpp
    src/ast/ast_declaration.cpp
    src/ast/ast_visitors.cpp
    src/ast/ast_pattern.cpp
    
    # Types
    src/types/type_system.cpp
    src/types/type_checker.cpp
    
    # Errors
    src/errors/error_reporter.cpp
    src/errors/error_formatter.cpp
    src/errors/diagnostics.cpp
    
    # Modules
    src/modules/module_system.cpp
    src/modules/module_loader.cpp
    src/modules/module_resolver.cpp
    src/modules/import_handler.cpp
    
    # Utils
    src/utils/string_utils.cpp
)

target_include_directories(sad_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

### tools/CMakeLists.txt

```cmake
# ============================================================================
# Tools - الأدوات
# ============================================================================

# REPL
add_subdirectory(repl)

# LSP Server
add_subdirectory(lsp)

# Package Manager
add_subdirectory(pkg)

# Documentation Generator
add_subdirectory(docgen)
```

---

## 📋 خطوات التنفيذ

### الخطوة 5.1: إنشاء مجلد tools

```powershell
$base = "C:\s\s_language\tools"

New-Item -ItemType Directory -Path "$base\repl" -Force
New-Item -ItemType Directory -Path "$base\lsp" -Force
New-Item -ItemType Directory -Path "$base\pkg" -Force
New-Item -ItemType Directory -Path "$base\docgen" -Force
```

### الخطوة 5.2: نسخ الملفات الموجودة

```powershell
# نسخ main.cpp إلى src/
# (إذا لم يكن موجوداً)
```

### الخطوة 5.3: إنشاء ملفات CMake

```powershell
# إنشاء CMakeLists.txt في كل مجلد فرعي
```

### الخطوة 5.4: تحديث CMakeLists.txt الرئيسي

تبديل الملف الرئيسي بالنسخة الجديدة.

---

## 🔗 مخطط التبعيات

```
                    +-------------+
                    |     sad     |
                    | (executable)|
                    +------+------+
                           |
        +------------------+------------------+
        |                  |                  |
        v                  v                  v
+-------+------+   +-------+------+   +-------+------+
| sad_shared   |   |sad_interpreter|  | sad_compiler |
| (core lib)   |   |   (opt lib)  |  |  (opt lib)   |
+--------------+   +-------+------+  +-------+------+
                           |                  |
                           v                  v
                   +-------+------+   +-------+------+
                   | sad_runtime  |   |    LLVM      |
                   +--------------+   +--------------+
```

---

## ✅ قائمة التحقق

- [ ] إنشاء CMakeLists.txt الرئيسي الجديد
- [ ] إنشاء CMakeLists.txt لكل مكون
- [ ] تحديث مسارات Include
- [ ] إضافة خيارات البناء
- [ ] اختبار البناء بالكامل
- [ ] اختبار خيارات البناء المختلفة

---

## ⏭️ الخطوة التالية

المرحلة 6: الاختبار والتحقق
