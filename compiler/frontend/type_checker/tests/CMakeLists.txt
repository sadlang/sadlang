# ============================================================================
# Type Checker Tests - CMake Configuration
# اختبارات مدقق الأنواع - إعدادات CMake
# ============================================================================
# 
# الوصف (Description):
#   إعدادات CMake لبناء اختبارات Type Checker باستخدام Google Test
#   CMake configuration for building Type Checker tests using Google Test
# 
# المؤلف (Author): SadLanguage Compiler Team
# التاريخ (Date): December 31, 2025
# الإصدار (Version): 1.0.0
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# Google Test Setup / إعداد Google Test
# ============================================================================

# البحث عن Google Test أو تنزيله / Find or download Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ============================================================================
# Test Executables / برامج الاختبار
# ============================================================================

# اختبار Type Context / Type Context Tests
add_executable(test_type_context
    test_type_context.cpp
)

target_link_libraries(test_type_context
    PRIVATE
        GTest::gtest_main
)

target_include_directories(test_type_context
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# ============================================================================

# اختبار Type Inference / Type Inference Tests  
add_executable(test_type_inference
    test_type_inference.cpp
)

target_link_libraries(test_type_inference
    PRIVATE
        GTest::gtest_main
)

target_include_directories(test_type_inference
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# ============================================================================

# اختبار Type Checker / Type Checker Tests
add_executable(test_type_checker
    test_type_checker.cpp
)

target_link_libraries(test_type_checker
    PRIVATE
        GTest::gtest_main
)

target_include_directories(test_type_checker
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# ============================================================================

# اختبار Error Reporting / Error Reporting Tests
add_executable(test_error_reporting
    test_error_reporting.cpp
)

target_link_libraries(test_error_reporting
    PRIVATE
        GTest::gtest_main
)

target_include_directories(test_error_reporting
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# ============================================================================

# اختبار Typed AST / Typed AST Tests
add_executable(test_typed_ast
    test_typed_ast.cpp
)

target_link_libraries(test_typed_ast
    PRIVATE
        GTest::gtest_main
)

target_include_directories(test_typed_ast
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../../include
)

# ============================================================================
# Test Discovery / اكتشاف الاختبارات
# ============================================================================

include(GoogleTest)

gtest_discover_tests(test_type_context)
gtest_discover_tests(test_type_inference)
gtest_discover_tests(test_type_checker)
gtest_discover_tests(test_error_reporting)
gtest_discover_tests(test_typed_ast)

# ============================================================================
# Custom Test Target / هدف اختبار مخصص
# ============================================================================

add_custom_target(run_type_checker_tests
    COMMAND test_type_context
    COMMAND test_type_inference
    COMMAND test_type_checker
    COMMAND test_error_reporting
    COMMAND test_typed_ast
    DEPENDS 
        test_type_context
        test_type_inference
        test_type_checker
        test_error_reporting
        test_typed_ast
    COMMENT "Running Type Checker Test Suite / تشغيل مجموعة اختبارات مدقق الأنواع"
)

# ============================================================================
# EOF
# ============================================================================
