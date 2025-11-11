# 🏗️ خطة البناء و CI/CD / Build, CI/CD & Tools Implementation Plan

**التاريخ:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**المدة المقدرة:** 1 أسبوع

---

## 📋 ملخص / Summary

### (AR) الملخص
خطة تفصيلية لتحسين نظام البناء (CMake)، إعداد CI/CD pipelines، وأدوات التطوير للغة "ص".

### (EN) Summary
Detailed plan for enhancing the build system (CMake), setting up CI/CD pipelines, and development tools for Sad language.

---

## 🎯 الأهداف / Objectives

1. ✅ تحسين CMakeLists.txt
2. ✅ Debug/Release configurations
3. ✅ Testing framework integration (Google Test)
4. ✅ GitHub Actions CI pipeline
5. ✅ PowerShell build scripts
6. ✅ Code coverage reporting
7. ✅ Documentation generation (Doxygen)
8. ✅ Release packaging

---

## 📦 الاعتماديات / Dependencies

### من plans:
- ✅ `09_integration_and_architecture.md` - Build structure
- ✅ All implementation plans (10-14)

### أدوات:
- **CMake** ≥3.20
- **Ninja** (build system)
- **Google Test** (testing)
- **Doxygen** (documentation)
- **GitHub Actions** (CI/CD)

---

## 🏗️ البنية المعمارية / Architecture

### Enhanced CMakeLists.txt

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.20)
project(SadLanguage VERSION 1.0.0 LANGUAGES CXX)

# ===== Options =====
option(DEBUG "Enable debug mode" OFF)
option(BUILD_TESTS "Build tests" ON)
option(BUILD_DOCS "Build documentation" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(GRAPHICS_SDL2 "Enable SDL2 graphics backend" OFF)
option(GRAPHICS_WIN32 "Enable Win32 GDI graphics backend" OFF)

# ===== C++ Standard =====
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ===== Output Directories =====
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ===== Include Directories =====
include_directories(${CMAKE_SOURCE_DIR}/include)

# ===== Compiler Flags =====
if(MSVC)
    # Visual Studio
    add_compile_options(/W4 /WX)
    
    if(DEBUG)
        add_compile_options(/Zi /Od)
        add_compile_definitions(DEBUG _DEBUG)
    else()
        add_compile_options(/O2)
        add_compile_definitions(NDEBUG)
    endif()
else()
    # GCC/Clang
    add_compile_options(-Wall -Wextra -Werror)
    
    if(DEBUG)
        add_compile_options(-g -O0)
        add_compile_definitions(DEBUG _DEBUG)
    else()
        add_compile_options(-O3)
        add_compile_definitions(NDEBUG)
    endif()
endif()

# ===== Source Files =====
set(LEXER_SOURCES
    src/lexer/token.cpp
    src/lexer/lexer_keywords.cpp
    src/lexer/lexer_core.cpp
    src/lexer/lexer_numbers.cpp
    src/lexer/lexer_strings.cpp
    src/lexer/lexer_identifiers.cpp
    src/lexer/lexer_operators.cpp
    src/lexer/lexer_comments.cpp
)

set(PARSER_SOURCES
    src/parser/parser_core.cpp
    src/parser/parser_expressions.cpp
    src/parser/parser_statements.cpp
    src/parser/parser_declarations.cpp
    src/parser/parser_error_recovery.cpp
    src/parser/ast/ast_node.cpp
    src/parser/ast/expressions.cpp
    src/parser/ast/statements.cpp
    src/parser/ast/declarations.cpp
    src/parser/ast/ast_printer.cpp
    src/parser/ast/ast_utils.cpp
)

set(INTERPRETER_SOURCES
    src/interpreter/core/interpreter_core.cpp
    src/interpreter/core/call_stack.cpp
    src/interpreter/core/exceptions.cpp
    src/interpreter/expressions/binary_evaluator.cpp
    src/interpreter/expressions/call_evaluator.cpp
    src/interpreter/expressions/index_evaluator.cpp
    src/interpreter/statements/control_flow.cpp
    src/interpreter/statements/declarations.cpp
    src/interpreter/oop/class_instance.cpp
    src/interpreter/oop/method_binding.cpp
)

set(DATA_SOURCES
    src/data/types/value.cpp
    src/data/managers/variable_manager.cpp
    src/data/scope/environment.cpp
)

set(STDLIB_SOURCES
    src/stdlib/core/stdlib_manager.cpp
    src/stdlib/io/io_module.cpp
    src/stdlib/io/print.cpp
    src/stdlib/io/input.cpp
    src/stdlib/io/file_io.cpp
    src/stdlib/string/string_module.cpp
    src/stdlib/string/manipulation.cpp
    src/stdlib/math/math_module.cpp
    src/stdlib/math/basic.cpp
    src/stdlib/math/trigonometry.cpp
    src/stdlib/math/random.cpp
    src/stdlib/time/time_module.cpp
    src/stdlib/time/datetime.cpp
    src/stdlib/time/timing.cpp
)

set(UTILS_SOURCES
    src/utils/string_utils.cpp
    src/errors/error_manager.cpp
)

# ===== Graphics Sources (optional) =====
if(GRAPHICS_SDL2 OR GRAPHICS_WIN32)
    set(GRAPHICS_SOURCES
        src/stdlib/graphics/graphics_module.cpp
    )
    
    if(GRAPHICS_SDL2)
        list(APPEND GRAPHICS_SOURCES src/stdlib/graphics/backends/sdl2_backend.cpp)
        add_compile_definitions(GRAPHICS_SDL2)
        
        # Find SDL2
        find_package(SDL2 REQUIRED)
        include_directories(${SDL2_INCLUDE_DIRS})
    endif()
    
    if(GRAPHICS_WIN32)
        list(APPEND GRAPHICS_SOURCES src/stdlib/graphics/backends/win32_backend.cpp)
        add_compile_definitions(GRAPHICS_WIN32)
    endif()
    
    list(APPEND STDLIB_SOURCES ${GRAPHICS_SOURCES})
endif()

# ===== Executable =====
add_executable(sad
    src/main.cpp
    ${LEXER_SOURCES}
    ${PARSER_SOURCES}
    ${INTERPRETER_SOURCES}
    ${DATA_SOURCES}
    ${STDLIB_SOURCES}
    ${UTILS_SOURCES}
)

# Link libraries
if(GRAPHICS_SDL2)
    target_link_libraries(sad ${SDL2_LIBRARIES})
endif()

# ===== Testing =====
if(BUILD_TESTS)
    enable_testing()
    
    # Find Google Test
    find_package(GTest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    
    # Lexer Tests
    add_executable(lexer_tests
        tests/lexer_tests/token_test.cpp
        tests/lexer_tests/position_test.cpp
        tests/lexer_tests/numbers_test.cpp
        tests/lexer_tests/strings_test.cpp
        tests/lexer_tests/unicode_test.cpp
        tests/lexer_tests/keywords_test.cpp
        tests/lexer_tests/integration_test.cpp
        ${LEXER_SOURCES}
        ${UTILS_SOURCES}
    )
    target_link_libraries(lexer_tests GTest::GTest GTest::Main)
    add_test(NAME LexerTests COMMAND lexer_tests)
    
    # Parser Tests
    add_executable(parser_tests
        tests/parser_tests/parser_basic_test.cpp
        tests/parser_tests/expressions_test.cpp
        tests/parser_tests/statements_test.cpp
        tests/parser_tests/declarations_test.cpp
        tests/parser_tests/precedence_test.cpp
        tests/parser_tests/error_recovery_test.cpp
        tests/parser_tests/integration_test.cpp
        ${LEXER_SOURCES}
        ${PARSER_SOURCES}
        ${UTILS_SOURCES}
    )
    target_link_libraries(parser_tests GTest::GTest GTest::Main)
    add_test(NAME ParserTests COMMAND parser_tests)
    
    # Interpreter Tests
    add_executable(interpreter_tests
        tests/interpreter_tests/value_test.cpp
        tests/interpreter_tests/environment_test.cpp
        tests/interpreter_tests/expressions_test.cpp
        tests/interpreter_tests/statements_test.cpp
        tests/interpreter_tests/functions_test.cpp
        tests/interpreter_tests/classes_test.cpp
        tests/interpreter_tests/integration_test.cpp
        ${LEXER_SOURCES}
        ${PARSER_SOURCES}
        ${INTERPRETER_SOURCES}
        ${DATA_SOURCES}
        ${UTILS_SOURCES}
    )
    target_link_libraries(interpreter_tests GTest::GTest GTest::Main)
    add_test(NAME InterpreterTests COMMAND interpreter_tests)
    
    # StdLib Tests
    add_executable(stdlib_tests
        tests/stdlib_tests/io_module_test.cpp
        tests/stdlib_tests/string_module_test.cpp
        tests/stdlib_tests/math_module_test.cpp
        tests/stdlib_tests/time_module_test.cpp
        tests/stdlib_tests/module_loading_test.cpp
        ${STDLIB_SOURCES}
        ${DATA_SOURCES}
        ${UTILS_SOURCES}
    )
    target_link_libraries(stdlib_tests GTest::GTest GTest::Main)
    add_test(NAME StdLibTests COMMAND stdlib_tests)
endif()

# ===== Code Coverage =====
if(ENABLE_COVERAGE AND NOT MSVC)
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

# ===== Documentation =====
if(BUILD_DOCS)
    find_package(Doxygen REQUIRED)
    
    set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/docs)
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_EXTRACT_ALL YES)
    set(DOXYGEN_EXTRACT_PRIVATE YES)
    set(DOXYGEN_EXTRACT_STATIC YES)
    
    doxygen_add_docs(docs
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        COMMENT "Generating API documentation with Doxygen"
    )
endif()

# ===== Installation =====
install(TARGETS sad DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY docs/ DESTINATION share/docs OPTIONAL)

# ===== Print Configuration =====
message(STATUS "===== Sad Language Build Configuration =====")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Debug Mode: ${DEBUG}")
message(STATUS "Build Tests: ${BUILD_TESTS}")
message(STATUS "Build Docs: ${BUILD_DOCS}")
message(STATUS "Coverage: ${ENABLE_COVERAGE}")
message(STATUS "Graphics SDL2: ${GRAPHICS_SDL2}")
message(STATUS "Graphics Win32: ${GRAPHICS_WIN32}")
message(STATUS "===========================================")
```

---

## 📁 الملفات المطلوبة / Required Files

```
Root/
├── CMakeLists.txt                   ✅ UPDATE (comprehensive)
├── .github/
│   └── workflows/
│       ├── ci.yml                   🆕 NEW (GitHub Actions)
│       ├── release.yml              🆕 NEW (Release workflow)
│       └── docs.yml                 🆕 NEW (Documentation)
├── scripts/
│   ├── build.ps1                    🆕 NEW (PowerShell build)
│   ├── test.ps1                     🆕 NEW (PowerShell test)
│   ├── clean.ps1                    🆕 NEW (PowerShell clean)
│   ├── coverage.ps1                 🆕 NEW (Coverage report)
│   └── release.ps1                  🆕 NEW (Package release)
├── Doxyfile                         🆕 NEW (Doxygen config)
└── .gitignore                       ✅ UPDATE
```

---

## 🔧 تقسيم العمل / Task Breakdown

### يوم 1-2: تحسين CMake

#### Task 1.1: Enhanced CMakeLists.txt
**الملفات:**
- `CMakeLists.txt`

**الخطوات:**
1. إضافة Options (DEBUG, BUILD_TESTS, etc.)
2. تنظيم Source files
3. إضافة Testing support
4. إضافة Documentation support
5. إضافة Installation rules

**معايير القبول:**
- ✅ CMake configures without errors
- ✅ All targets build successfully
- ✅ Tests run via CTest
- ✅ Options work correctly

---

### يوم 3-4: PowerShell Scripts

#### Task 2.1: Build Scripts
**ملف:** `scripts/build.ps1`

```powershell
# scripts/build.ps1
param(
    [string]$BuildType = "Debug",
    [switch]$EnableGraphics,
    [switch]$EnableTests,
    [switch]$EnableCoverage,
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

Write-Host "===== Sad Language Build Script =====" -ForegroundColor Cyan
Write-Host "Build Type: $BuildType" -ForegroundColor Yellow

# Project root
$ProjectRoot = Split-Path -Parent $PSScriptRoot
Set-Location $ProjectRoot

# Clean build directory
if ($Clean) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    if (Test-Path "build") {
        Remove-Item -Recurse -Force "build"
    }
}

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Set-Location "build"

# CMake options
$CMakeOptions = @(
    "-G", "Ninja",
    "-DCMAKE_BUILD_TYPE=$BuildType"
)

if ($BuildType -eq "Debug") {
    $CMakeOptions += "-DDEBUG=ON"
}

if ($EnableTests) {
    $CMakeOptions += "-DBUILD_TESTS=ON"
}

if ($EnableCoverage) {
    $CMakeOptions += "-DENABLE_COVERAGE=ON"
}

if ($EnableGraphics) {
    $CMakeOptions += "-DGRAPHICS_SDL2=ON"
    $CMakeOptions += "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
}

# Configure
Write-Host "Configuring CMake..." -ForegroundColor Yellow
& cmake $CMakeOptions ..
if ($LASTEXITCODE -ne 0) {
    Write-Error "CMake configuration failed!"
    exit 1
}

# Build
Write-Host "Building..." -ForegroundColor Yellow
& cmake --build . --config $BuildType
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed!"
    exit 1
}

Write-Host "Build completed successfully!" -ForegroundColor Green
Write-Host "Executable: $ProjectRoot\build\bin\sad.exe" -ForegroundColor Cyan
```

---

#### Task 2.2: Test Script
**ملف:** `scripts/test.ps1`

```powershell
# scripts/test.ps1
param(
    [string]$Filter = "",
    [switch]$Verbose
)

$ErrorActionPreference = "Stop"

$ProjectRoot = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $ProjectRoot "build"

if (-not (Test-Path $BuildDir)) {
    Write-Error "Build directory not found. Run build.ps1 first."
    exit 1
}

Set-Location $BuildDir

Write-Host "===== Running Tests =====" -ForegroundColor Cyan

$CTestArgs = @(
    "--output-on-failure",
    "-C", "Debug"
)

if ($Filter) {
    $CTestArgs += "-R", $Filter
}

if ($Verbose) {
    $CTestArgs += "-V"
}

& ctest $CTestArgs
if ($LASTEXITCODE -ne 0) {
    Write-Error "Tests failed!"
    exit 1
}

Write-Host "All tests passed!" -ForegroundColor Green
```

---

#### Task 2.3: Coverage Script
**ملف:** `scripts/coverage.ps1`

```powershell
# scripts/coverage.ps1
$ErrorActionPreference = "Stop"

Write-Host "===== Code Coverage Report =====" -ForegroundColor Cyan

$ProjectRoot = Split-Path -Parent $PSScriptRoot

# Build with coverage
& "$PSScriptRoot\build.ps1" -EnableTests -EnableCoverage -Clean
if ($LASTEXITCODE -ne 0) {
    Write-Error "Build with coverage failed!"
    exit 1
}

# Run tests
& "$PSScriptRoot\test.ps1"
if ($LASTEXITCODE -ne 0) {
    Write-Error "Tests failed!"
    exit 1
}

# Generate coverage report (gcov/lcov)
Set-Location "$ProjectRoot\build"

Write-Host "Generating coverage report..." -ForegroundColor Yellow
& lcov --capture --directory . --output-file coverage.info
& lcov --remove coverage.info '/usr/*' --output-file coverage.info
& lcov --remove coverage.info '*/tests/*' --output-file coverage.info
& genhtml coverage.info --output-directory coverage_html

Write-Host "Coverage report generated: build/coverage_html/index.html" -ForegroundColor Green
```

---

### يوم 5-7: CI/CD Pipelines

#### Task 3.1: GitHub Actions CI
**ملف:** `.github/workflows/ci.yml`

```yaml
name: CI

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main, develop ]

jobs:
  build-windows:
    runs-on: windows-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup Ninja
      run: choco install ninja
    
    - name: Setup vcpkg
      run: |
        git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
        C:\vcpkg\bootstrap-vcpkg.bat
        C:\vcpkg\vcpkg integrate install
    
    - name: Install dependencies
      run: C:\vcpkg\vcpkg install gtest:x64-windows
    
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DDEBUG=ON -DBUILD_TESTS=ON -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake
    
    - name: Build
      run: cmake --build build --config Debug
    
    - name: Run tests
      run: |
        cd build
        ctest --output-on-failure -C Debug
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v3
      with:
        name: sad-windows
        path: build/bin/sad.exe

  build-linux:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y ninja-build libgtest-dev
    
    - name: Configure CMake
      run: |
        mkdir build
        cd build
        cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DDEBUG=ON -DBUILD_TESTS=ON
    
    - name: Build
      run: cmake --build build --config Debug
    
    - name: Run tests
      run: |
        cd build
        ctest --output-on-failure -C Debug
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v3
      with:
        name: sad-linux
        path: build/bin/sad

  coverage:
    runs-on: ubuntu-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y ninja-build libgtest-dev lcov
    
    - name: Configure CMake with coverage
      run: |
        mkdir build
        cd build
        cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON -DENABLE_COVERAGE=ON
    
    - name: Build
      run: cmake --build build --config Debug
    
    - name: Run tests
      run: |
        cd build
        ctest --output-on-failure -C Debug
    
    - name: Generate coverage report
      run: |
        cd build
        lcov --capture --directory . --output-file coverage.info
        lcov --remove coverage.info '/usr/*' --output-file coverage.info
        lcov --list coverage.info
    
    - name: Upload coverage to Codecov
      uses: codecov/codecov-action@v3
      with:
        files: ./build/coverage.info
        flags: unittests
        name: codecov-umbrella
```

---

#### Task 3.2: Release Workflow
**ملف:** `.github/workflows/release.yml`

```yaml
name: Release

on:
  push:
    tags:
      - 'v*'

jobs:
  release:
    runs-on: windows-latest
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Setup Build Environment
      run: choco install ninja
    
    - name: Build Release
      run: |
        mkdir build
        cd build
        cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release
        cmake --build . --config Release
    
    - name: Package
      run: |
        mkdir release
        cp build/bin/sad.exe release/
        cp README.md release/
        cp LICENSE release/
        
    - name: Create Archive
      run: Compress-Archive -Path release/* -DestinationPath sad-windows-x64.zip
    
    - name: Create Release
      uses: softprops/action-gh-release@v1
      with:
        files: sad-windows-x64.zip
        body: |
          ## Sad Language ${{ github.ref_name }}
          
          ### Changes
          See CHANGELOG.md for details.
          
          ### Installation
          Extract the archive and add to PATH.
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

---

## 🧪 الاختبارات / Tests

### PowerShell Script Tests
```powershell
# Test build script
.\scripts\build.ps1 -BuildType Debug -EnableTests -Clean

# Test test script
.\scripts\test.ps1 -Verbose

# Test specific test suite
.\scripts\test.ps1 -Filter "lexer_tests"
```

---

## ⏱️ الجدول الزمني / Timeline

| اليوم | المهام | الساعات |
|-------|--------|---------|
| يوم 1-2 | Enhanced CMake | 16 ساعة |
| يوم 3-4 | PowerShell Scripts | 16 ساعة |
| يوم 5-7 | CI/CD Pipelines | 24 ساعة |
| **Total** | **1 week** | **56 ساعة** |

---

## 🚀 أوامر PowerShell / PowerShell Commands

```powershell
# إنشاء فرع العمل
git checkout -b feat/build/ci-cd-setup

# اختبار build scripts
.\scripts\build.ps1 -EnableTests -Clean
.\scripts\test.ps1

# Commit
git add CMakeLists.txt scripts/ .github/
git commit -m "[build] Enhanced build system and CI/CD pipelines

- Comprehensive CMakeLists.txt with options
- PowerShell build/test/coverage scripts
- GitHub Actions CI for Windows/Linux
- Release workflow with artifacts
- Code coverage integration"

git push --set-upstream origin feat/build/ci-cd-setup
```

---

## ⚠️ المخاطر / Risks

| الخطر | التخفيف |
|-------|---------|
| CI timeouts | Optimize build, use caching |
| Cross-platform issues | Test on multiple OS |
| Coverage accuracy | Exclude tests from coverage |

---

**آخر تحديث:** 4 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ
