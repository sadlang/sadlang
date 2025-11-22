# ==============================================================================
# CMakeLists.txt للاختبارات الشاملة للمكتبة القياسية
# CMakeLists.txt for Standard Library Comprehensive Tests
# ==============================================================================

cmake_minimum_required(VERSION 3.10)

# ==============================================================================
# إعدادات المشروع / Project Settings
# ==============================================================================
project(StdLibTests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==============================================================================
# مسارات الـ Include / Include Paths
# ==============================================================================
include_directories(
    ${CMAKE_SOURCE_DIR}/../../include
    ${CMAKE_SOURCE_DIR}/../../include/stdlib
    ${CMAKE_SOURCE_DIR}/../../include/stdlib/string
    ${CMAKE_SOURCE_DIR}/../../include/stdlib/core
    ${CMAKE_SOURCE_DIR}/../../include/stdlib/math
)

# ==============================================================================
# ملفات المصدر المشتركة / Shared Source Files
# ==============================================================================
set(STDLIB_SOURCES
    ${CMAKE_SOURCE_DIR}/../../src/stdlib/string/string_functions.cpp
    ${CMAKE_SOURCE_DIR}/../../src/stdlib/core/array_functions.cpp
    ${CMAKE_SOURCE_DIR}/../../src/stdlib/math/math_functions.cpp
)

# ==============================================================================
# اختبارات دوال النصوص / String Functions Tests
# ==============================================================================
add_executable(string_functions_tests
    string_functions_tests.cpp
    ${CMAKE_SOURCE_DIR}/../../src/stdlib/string/string_functions.cpp
    ${CMAKE_SOURCE_DIR}/../../src/data/types/value.cpp
)

target_include_directories(string_functions_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/../../include
)

# ==============================================================================
# اختبارات دوال المصفوفات / Array Functions Tests
# ==============================================================================
add_executable(array_functions_tests
    array_functions_tests.cpp
    ${CMAKE_SOURCE_DIR}/../../src/stdlib/core/array_functions.cpp
    ${CMAKE_SOURCE_DIR}/../../src/data/types/value.cpp
)

target_include_directories(array_functions_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/../../include
)

# ==============================================================================
# اختبارات الدوال الرياضية / Math Functions Tests
# ==============================================================================
add_executable(math_functions_tests
    math_functions_tests.cpp
    ${CMAKE_SOURCE_DIR}/../../src/stdlib/math/math_functions.cpp
    ${CMAKE_SOURCE_DIR}/../../src/data/types/value.cpp
)

target_include_directories(math_functions_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/../../include
)

# ==============================================================================
# اختبارات دوال الأنواع / Type Functions Tests
# ==============================================================================
add_executable(type_functions_tests
    type_functions_tests.cpp
    ${CMAKE_SOURCE_DIR}/../../src/stdlib/core/type_functions.cpp
    ${CMAKE_SOURCE_DIR}/../../src/data/types/value.cpp
)

target_include_directories(type_functions_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/../../include
)

# ==============================================================================
# اختبارات الدوال الأخرى / Other Functions Tests
# ==============================================================================
add_executable(other_functions_tests
    other_functions_tests.cpp
    ${CMAKE_SOURCE_DIR}/../../src/stdlib/core/other_functions.cpp
    ${CMAKE_SOURCE_DIR}/../../src/data/types/value.cpp
)

target_include_directories(other_functions_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/../../include
)

# ==============================================================================
# إضافة اختبارات CTest / Add CTest Tests
# ==============================================================================
enable_testing()

add_test(NAME StringFunctionsTests COMMAND string_functions_tests)
add_test(NAME ArrayFunctionsTests COMMAND array_functions_tests)
add_test(NAME MathFunctionsTests COMMAND math_functions_tests)
add_test(NAME TypeFunctionsTests COMMAND type_functions_tests)
add_test(NAME OtherFunctionsTests COMMAND other_functions_tests)

# ==============================================================================
# خيارات الترجمة / Compilation Options
# ==============================================================================
if(MSVC)
    # Visual Studio specific options
    target_compile_options(string_functions_tests PRIVATE /W4 /utf-8)
    target_compile_options(array_functions_tests PRIVATE /W4 /utf-8)
    target_compile_options(math_functions_tests PRIVATE /W4 /utf-8)
    target_compile_options(type_functions_tests PRIVATE /W4 /utf-8)
    target_compile_options(other_functions_tests PRIVATE /W4 /utf-8)
else()
    # GCC/Clang options
    target_compile_options(string_functions_tests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(array_functions_tests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(math_functions_tests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(type_functions_tests PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(other_functions_tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ==============================================================================
# معلومات البناء / Build Information
# ==============================================================================
message(STATUS "═══════════════════════════════════════════════════════")
message(STATUS "  Standard Library Tests Configuration")
message(STATUS "  اختبارات المكتبة القياسية")
message(STATUS "═══════════════════════════════════════════════════════")
message(STATUS "  Tests:")
message(STATUS "    - String Functions Tests")
message(STATUS "    - Array Functions Tests")
message(STATUS "    - Math Functions Tests")
message(STATUS "    - Type Functions Tests")
message(STATUS "    - Other Functions Tests")
message(STATUS "═══════════════════════════════════════════════════════")
