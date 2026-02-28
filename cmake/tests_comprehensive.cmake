# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/tests_comprehensive.cmake
# الوصف: الاختبارات الشاملة (900+ اختبار) / Comprehensive Test Suite (900+ tests)
# ═══════════════════════════════════════════════════════════════════════════════

if(NOT BUILD_TESTS)
    return()
endif()

message(STATUS "════════════════════════════════════════════════════════")
message(STATUS "  إعداد الاختبارات الشاملة / Setting up Comprehensive Tests")
message(STATUS "════════════════════════════════════════════════════════")

# ──────────────────────────────────────────────────────────────────────
# ماكرو لإنشاء اختبار شامل / Macro to create a comprehensive test
# ──────────────────────────────────────────────────────────────────────
macro(add_comprehensive_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME}
        ${CMAKE_SOURCE_DIR}/tests/comprehensive/${TEST_SOURCE}
    )

    target_link_libraries(${TEST_NAME} PRIVATE sad_shared)

    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/comprehensive
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
        ${CMAKE_SOURCE_DIR}/shared/modules/include
        ${CMAKE_SOURCE_DIR}/shared/utils/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/parser
        ${CMAKE_SOURCE_DIR}/include/parser/ast
        ${CMAKE_SOURCE_DIR}/include/lexer
        ${CMAKE_SOURCE_DIR}/include/data/types
        ${CMAKE_SOURCE_DIR}/include/errors
        ${CMAKE_SOURCE_DIR}/include/modules
        ${CMAKE_SOURCE_DIR}/include/utils
        ${CMAKE_SOURCE_DIR}/interpreter_new/include
        ${CMAKE_SOURCE_DIR}/interpreter_new/include/managers
    )

    if(MSVC)
        target_compile_options(${TEST_NAME} PRIVATE /W3 /utf-8 /FS /Zc:preprocessor)
    else()
        target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra)
    endif()

    target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)

    add_test(NAME "Comprehensive_${TEST_NAME}" COMMAND ${TEST_NAME})
    set_tests_properties("Comprehensive_${TEST_NAME}" PROPERTIES TIMEOUT 120)

    message(STATUS "  ✅ ${TEST_NAME}")
endmacro()

# ──────────────────────────────────────────────────────────────────────
# إنشاء جميع الاختبارات الشاملة / Create all comprehensive tests
# ──────────────────────────────────────────────────────────────────────

# 1. المحلل المعجمي / Lexer Tests (96 tests)
add_comprehensive_test(test_lexer_comprehensive test_lexer_comprehensive.cpp)

# 2. المحلل النحوي / Parser Tests (87 tests)
add_comprehensive_test(test_parser_comprehensive test_parser_comprehensive.cpp)

# 3. نظام القيم / Value System Tests (95 tests)
add_comprehensive_test(test_value_comprehensive test_value_comprehensive.cpp)

# 4. المفسر / Interpreter Tests (76 tests)
add_comprehensive_test(test_interpreter_comprehensive test_interpreter_comprehensive.cpp)
target_link_libraries(test_interpreter_comprehensive PRIVATE sad_new_interpreter sad_new_semantic)
target_sources(test_interpreter_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/tests/comprehensive/interpreter_test_stubs.cpp)

# 5. المكتبة القياسية / Standard Library Tests (90 tests)
add_comprehensive_test(test_stdlib_comprehensive test_stdlib_comprehensive.cpp)
target_sources(test_stdlib_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib/core/array_functions.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/core/type_functions.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/core/stdlib_manager.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/io/io_functions.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/string/string_functions.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/math/math_functions.cpp
    ${CMAKE_SOURCE_DIR}/interpreter_new/src/managers/function_manager.cpp)
target_include_directories(test_stdlib_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib ${CMAKE_SOURCE_DIR}/stdlib/core
    ${CMAKE_SOURCE_DIR}/stdlib/string ${CMAKE_SOURCE_DIR}/stdlib/math)

# 6. الأخطاء الذكية / Smart Errors Tests (38 tests)
add_comprehensive_test(test_errors_comprehensive test_errors_comprehensive.cpp)

# 7. الآلة الافتراضية والمترجم / VM & Compiler Tests (77 tests)
add_comprehensive_test(test_vm_compiler_comprehensive test_vm_compiler_comprehensive.cpp)
target_link_libraries(test_vm_compiler_comprehensive PRIVATE sad_vm sad_new_interpreter sad_new_frontend)
target_include_directories(test_vm_compiler_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/vm/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include/frontend
    ${CMAKE_SOURCE_DIR}/interpreter_new/include
    ${CMAKE_SOURCE_DIR}/interpreter_new/include/managers)

# 8. اختبارات التكامل / End-to-End Tests (47 tests)
add_comprehensive_test(test_e2e_comprehensive test_e2e_comprehensive.cpp)

# 9. الأدوات ونظام الوحدات / Utils & Modules Tests (95 tests)
add_comprehensive_test(test_utils_modules_comprehensive test_utils_modules_comprehensive.cpp)

# 10. المترجم و SIR / Compiler & SIR Tests (100 tests)
add_comprehensive_test(test_compiler_comprehensive test_compiler_comprehensive.cpp)
target_include_directories(test_compiler_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/vm/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include/frontend
    ${CMAKE_SOURCE_DIR}/compiler_new/include/backend)

# 11. الرسومات والأدوات والشبكة / Graphics, Tools & Network Tests (100 tests)
add_comprehensive_test(test_graphics_tools_comprehensive test_graphics_tools_comprehensive.cpp)
target_include_directories(test_graphics_tools_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/graphics/include
    ${CMAKE_SOURCE_DIR}/graphics
    ${CMAKE_SOURCE_DIR}/network/include
    ${CMAKE_SOURCE_DIR}/tools)

# ──────────────────────────────────────────────────────────────────────
# هدف مُجمّع / Combined Target
# ──────────────────────────────────────────────────────────────────────
add_custom_target(comprehensive_tests
    DEPENDS
        test_lexer_comprehensive
        test_parser_comprehensive
        test_value_comprehensive
        test_interpreter_comprehensive
        test_stdlib_comprehensive
        test_errors_comprehensive
        test_vm_compiler_comprehensive
        test_e2e_comprehensive
        test_utils_modules_comprehensive
        test_compiler_comprehensive
        test_graphics_tools_comprehensive
    COMMENT "بناء جميع الاختبارات الشاملة / Building all comprehensive tests"
)

message(STATUS "════════════════════════════════════════════════════════")
message(STATUS "  ✅ الاختبارات الشاملة: 11 ملف | 900+ اختبار")
message(STATUS "  📋 بناء الكل: cmake --build build --target comprehensive_tests")
message(STATUS "  🧪 تشغيل: ctest --test-dir build -R Comprehensive")
message(STATUS "════════════════════════════════════════════════════════")
