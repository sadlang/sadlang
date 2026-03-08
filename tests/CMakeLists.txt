# ========================================
# SadLanguage Tests Build Configuration
# إعدادات بناء اختبارات لغة سد
# ========================================

cmake_minimum_required(VERSION 3.15)

project(sad_tests
    VERSION 1.0.0
    LANGUAGES CXX
    DESCRIPTION "SadLanguage Test Suite"
)

# معايير C++17 / C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ========================================
# Test Suite - مجموعة الاختبارات
# ========================================

add_executable(test_suite
    test_suite.cpp
)

# ========================================
# LLVM Integration Test - اختبار تكامل LLVM (Phase 1.2.1)
# ========================================

if(ENABLE_LLVM_BACKEND AND LLVM_FOUND)
    add_executable(test_llvm_integration_simple
        jit/test_llvm_integration_simple.cpp
    )
    
    target_link_libraries(test_llvm_integration_simple
        PRIVATE
            sad_jit
    )
    
    target_include_directories(test_llvm_integration_simple
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../include
            ${CMAKE_CURRENT_SOURCE_DIR}/../compiler/jit/include
    )
    
    target_compile_definitions(test_llvm_integration_simple
        PRIVATE
            ENABLE_LLVM_BACKEND
    )
    
    message(STATUS "✅ Test: LLVM Integration test enabled")
else()
    message(STATUS "⚠️  Test: LLVM Integration test disabled (LLVM not found)")
endif()

# ========================================
# Module Parsing Tests - اختبارات تحليل الوحدات
# ========================================

add_executable(test_module_parsing
    parser/test_module_parsing.cpp
)

target_link_libraries(test_module_parsing
    PRIVATE
        sad_parser
        sad_lexer
)

target_include_directories(test_module_parsing
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../parser/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../lexer/include
)

target_link_libraries(test_suite
    PRIVATE
        sad_compiler
        sad_vm
        sad_runtime
        sad_bytecode
        sad_parser
        sad_lexer
        sad_optimizer
)

target_include_directories(test_suite
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../compiler/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../vm/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../runtime/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bytecode/include
)

# ========================================
# Benchmark - قياس الأداء
# ========================================

add_executable(benchmark
    benchmark.cpp
)

target_link_libraries(benchmark
    PRIVATE
        sad_compiler
        sad_vm
        sad_runtime
        sad_bytecode
        sad_parser
        sad_lexer
        sad_optimizer
)

target_include_directories(benchmark
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../compiler/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../vm/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../runtime/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../bytecode/include
)

# ========================================
# Compiler Options - خيارات التحويل
# ========================================

if(MSVC)
    target_compile_options(test_suite PRIVATE /W4)
    target_compile_options(test_module_parsing PRIVATE /W4)
    target_compile_options(benchmark PRIVATE /W4)
else()
    target_compile_options(test_suite PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(test_module_parsing PRIVATE -Wall -Wextra -pedantic)
    target_compile_options(benchmark PRIVATE -Wall -Wextra -pedantic)
endif()

# ========================================
# CTest Integration - تكامل CTest
# ========================================

enable_testing()

add_test(
    NAME "CompilerTests"
    COMMAND test_suite
)

add_test(
    NAME "ModuleParsingTests"
    COMMAND test_module_parsing
)

add_test(
    NAME "PerformanceBenchmark"
    COMMAND benchmark
)

# ========================================
# Catch2 Unit Tests (unit_tests)
# هدف اختبارات الوحدة باستخدام Catch2 - سيجمع جميع ملفات الاختبار في مجلدات lexer/parser/stdlib
# ========================================

file(GLOB UNIT_TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/lexer_tests/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/parser_tests/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/stdlib_tests/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unit/*.cpp
)

add_executable(unit_tests
    ${UNIT_TEST_SOURCES}
    test_main.cpp
)

target_include_directories(unit_tests
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared/lexer/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../shared/parser/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../graphics/third_party/catch2
)

target_link_libraries(unit_tests
    PRIVATE
        sad_lexer
        sad_parser
        sad_runtime
        sad_compiler
        sad_bytecode
)

add_test(NAME "UnitTests" COMMAND unit_tests)

# ========================================
# إطار الاختبارات الموحد / Unified Test Framework
# ========================================
add_subdirectory(framework)

# ========================================
# Summary - الملخص
# ========================================

message(STATUS "========================================")
message(STATUS "SadLanguage Test Suite")
message(STATUS "مجموعة اختبارات لغة سد")
message(STATUS "========================================")
message(STATUS "Test Executables:")
message(STATUS "  - test_suite  (مجموعة الاختبارات)")
message(STATUS "  - benchmark   (قياس الأداء)")
message(STATUS "========================================")
