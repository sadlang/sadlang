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
        ${CMAKE_SOURCE_DIR}/interpreter/include
        ${CMAKE_SOURCE_DIR}/interpreter/include/managers
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
target_link_libraries(test_interpreter_comprehensive PRIVATE sad_interpreter sad_semantic sad_profiler_lib)
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
    ${CMAKE_SOURCE_DIR}/interpreter/src/managers/function_manager.cpp)
target_include_directories(test_stdlib_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib ${CMAKE_SOURCE_DIR}/stdlib/core
    ${CMAKE_SOURCE_DIR}/stdlib/string ${CMAKE_SOURCE_DIR}/stdlib/math)

# 6. الأخطاء الذكية / Smart Errors Tests (38 tests)
add_comprehensive_test(test_errors_comprehensive test_errors_comprehensive.cpp)

# 6.5. استنساخ عُقد AST / AST Clone Tests (18 tests)
add_comprehensive_test(test_ast_clone test_ast_clone.cpp)

# 6.6. مستخرج التوثيق / Documentation Extractor Tests (28 tests)
add_comprehensive_test(test_docs_extractor_comprehensive test_docs_extractor_comprehensive.cpp)

# 7. الآلة الافتراضية والمترجم / VM & Compiler Tests (77 tests)
add_comprehensive_test(test_vm_compiler_comprehensive test_vm_compiler_comprehensive.cpp)
target_link_libraries(test_vm_compiler_comprehensive PRIVATE sad_vm sad_interpreter sad_frontend)
target_include_directories(test_vm_compiler_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/vm/include
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}/compiler/include/frontend
    ${CMAKE_SOURCE_DIR}/interpreter/include
    ${CMAKE_SOURCE_DIR}/interpreter/include/managers)

# 7.5. اختبارات محرك JIT / JIT Engine Tests (74 tests)
add_comprehensive_test(test_jit_comprehensive test_jit_comprehensive.cpp)
target_link_libraries(test_jit_comprehensive PRIVATE sad_vm)
target_include_directories(test_jit_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/vm/include)

# 8. اختبارات التكامل / End-to-End Tests (47 tests)
add_comprehensive_test(test_e2e_comprehensive test_e2e_comprehensive.cpp)

# 9. الأدوات ونظام الوحدات / Utils & Modules Tests (95 tests)
add_comprehensive_test(test_utils_modules_comprehensive test_utils_modules_comprehensive.cpp)

# 10. المترجم و SIR / Compiler & SIR Tests (100 tests)
add_comprehensive_test(test_compiler_comprehensive test_compiler_comprehensive.cpp)
target_include_directories(test_compiler_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/vm/include
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}/compiler/include/frontend
    ${CMAKE_SOURCE_DIR}/compiler/include/backend)

# 11.5. اختبارات نظام الاستثناءات / Throw-Catch Exception Tests (35 tests)
add_comprehensive_test(test_throw_catch_comprehensive test_throw_catch_comprehensive.cpp)
target_link_libraries(test_throw_catch_comprehensive PRIVATE sad_interpreter sad_semantic sad_profiler_lib)
target_sources(test_throw_catch_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/tests/comprehensive/interpreter_test_stubs.cpp)

# 11.6. اختبارات ?. و ?? / Optional Chain & Null Coalesce Tests (30+ tests)
add_comprehensive_test(test_optional_null_comprehensive test_optional_null_comprehensive.cpp)
target_link_libraries(test_optional_null_comprehensive PRIVATE sad_interpreter sad_semantic sad_profiler_lib)
target_sources(test_optional_null_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/tests/comprehensive/interpreter_test_stubs.cpp)

# 12. اختبارات الانحدار / Regression Tests (22 tests - bugs from مشاكل.md)
add_comprehensive_test(test_regression_comprehensive test_regression_comprehensive.cpp)

# تمرير مسار sad.exe ومجلد الاختبارات / Pass interpreter and test directory paths
target_compile_definitions(test_regression_comprehensive PRIVATE
    SAD_EXE_PATH="$<TARGET_FILE:sad>"
    REGRESSION_DIR="${CMAKE_SOURCE_DIR}/tests/regression"
)

# مهلة أطول للانحدار — بعض الاختبارات تتضمن حلقات لا نهائية / Longer timeout for regression
set_tests_properties("Comprehensive_test_regression_comprehensive" PROPERTIES TIMEOUT 300)

# ──────────────────────────────────────────────────────────────────────
# 13. اختبارات وحدات المكتبة القياسية المتقدمة / Advanced Stdlib Module Tests (Sprint 1 — ريم)
# ──────────────────────────────────────────────────────────────────────

# 13a. الشبكات / Network Module Tests
add_comprehensive_test(test_network_comprehensive test_network_comprehensive.cpp)
target_include_directories(test_network_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib/network
    ${CMAKE_SOURCE_DIR}/stdlib/network/include
    ${CMAKE_SOURCE_DIR}/stdlib/network/include/network
    ${CMAKE_SOURCE_DIR}/stdlib/network/include/socket
    ${CMAKE_SOURCE_DIR}/stdlib/network/include/http
    ${CMAKE_SOURCE_DIR}/stdlib/network/include/websocket)
target_sources(test_network_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib/network/src/network/network_error.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/network/src/socket/socket_address.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/network/src/http/http_base.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/network/src/http/http_request.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/network/src/http/http_response.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/network/src/http/http_cookie.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/network/src/http/http_url.cpp)
if(WIN32)
    target_link_libraries(test_network_comprehensive PRIVATE ws2_32)
    target_compile_definitions(test_network_comprehensive PRIVATE WIN32_LEAN_AND_MEAN)
endif()

# 13b. البرمجة غير المتزامنة / Async Module Tests
add_comprehensive_test(test_async_comprehensive test_async_comprehensive.cpp)
target_include_directories(test_async_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib)

# 13c. النظام / System Module Tests
add_comprehensive_test(test_system_comprehensive test_system_comprehensive.cpp)
target_include_directories(test_system_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib)
target_sources(test_system_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib/system/system_functions.cpp)

# 13d. النصوص العربية / Arabic Text Module Tests
add_comprehensive_test(test_text_comprehensive test_text_comprehensive.cpp)
target_include_directories(test_text_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib)

# 13e. الصور / Image Module Tests (header-only — stb_image_write.h unavailable)
add_comprehensive_test(test_image_comprehensive test_image_comprehensive.cpp)
target_include_directories(test_image_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib)

# ──────────────────────────────────────────────────────────────────────
# 14. اختبارات الواجهة الرسومية + المترجم SIR / UI Widget + Compiler SIR Tests
# ──────────────────────────────────────────────────────────────────────
add_comprehensive_test(test_ui_comprehensive test_ui_comprehensive.cpp)
target_link_libraries(test_ui_comprehensive PRIVATE sad_ui)
target_include_directories(test_ui_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/compiler/src/sir
    ${CMAKE_SOURCE_DIR}/compiler/src
    ${CMAKE_SOURCE_DIR}/sad_ui/core/include)

# 14a. اختبارات قبول المرحلة 0: Parser UI (إذا/وإلا + لكل + knownWidgets)
# Phase 0 Acceptance Tests: UI Parser (conditional rendering + loops + widget registry)
add_comprehensive_test(test_ui_phase0 test_ui_phase0.cpp)

# 14c. اختبارات قبول المرحلة 1: النظام التفاعلي (BindingCompiler)
# Phase 1 Acceptance Tests: Reactive System (BindingCompiler)
add_comprehensive_test(test_ui_phase1 test_ui_phase1.cpp)
target_link_libraries(test_ui_phase1 PRIVATE sad_ui)
target_include_directories(test_ui_phase1 PRIVATE
    ${CMAKE_SOURCE_DIR}/sad_ui/core/include
    ${CMAKE_SOURCE_DIR}/sad_ui/reactive
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
)

# 14b. اختبارات منصات الرسومات الشاملة / Comprehensive Backend Tests (IR + Pipeline + Codegen)
add_comprehensive_test(test_backends_comprehensive test_backends_comprehensive.cpp)
target_link_libraries(test_backends_comprehensive PRIVATE sad_ui)
target_include_directories(test_backends_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/sad_ui/core/include
    ${CMAKE_SOURCE_DIR}/sad_ui/pipeline/include
    ${CMAKE_SOURCE_DIR}/sad_ui/widgets/include
    ${CMAKE_SOURCE_DIR}/sad_ui/backends/desktop/include
    ${CMAKE_SOURCE_DIR}/sad_ui/backends/android/include
    ${CMAKE_SOURCE_DIR}/sad_ui/backends/ios/include
    ${CMAKE_SOURCE_DIR}/sad_ui/backends/web/include
    ${CMAKE_SOURCE_DIR}/sad_ui/backends/macos/include)

# 14c. اختبارات نظام الأحداث الموحد / Unified Event System Tests (IREventType)
add_comprehensive_test(test_event_system_comprehensive test_event_system_comprehensive.cpp)
target_link_libraries(test_event_system_comprehensive PRIVATE sad_ui)
target_include_directories(test_event_system_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/sad_ui/core/include
    ${CMAKE_SOURCE_DIR}/sad_ui/pipeline/include)

# 14d. اختبارات أداء Reconciler ونظام الأحداث المستقرة / Reconciler Performance Tests
add_comprehensive_test(test_reconciler_performance test_reconciler_performance.cpp)
target_link_libraries(test_reconciler_performance PRIVATE sad_ui)
target_include_directories(test_reconciler_performance PRIVATE
    ${CMAKE_SOURCE_DIR}/sad_ui/core/include
    ${CMAKE_SOURCE_DIR}/sad_ui/pipeline/include)

# ──────────────────────────────────────────────────────────────────────
# اختبارات .ص فردية مباشرة عبر CTest / Individual .ص CTest entries
# ──────────────────────────────────────────────────────────────────────
# تسجيل كل ملف .ص كاختبار CTest مستقل — يسهل التشخيص / Register each .ص as standalone CTest
set(REGRESSION_TEST_DIR "${CMAKE_SOURCE_DIR}/tests/regression")

# P0 — عاجل / Critical
set(REGRESSION_P0_TESTS
    test_p01_array_key_access
    test_p02_while_update
    test_p03_length_depth
    test_p09_reserved_words
    test_p10_void_return
    test_p11_custom_class_fields
    test_p12_integer_division
    test_p16_compound_field
    test_p18_this_method_save
    test_p19_hex_literals
)

# P1 — مهم / Important
set(REGRESSION_P1_TESTS
    test_p04_hamza_var
    test_p08_break
    test_p13_class_prefix
    test_p14_cascading_errors
    test_p17_undocumented_reserved
    test_p20_super_reserved
    test_p24_json_unified_builtin
    test_p25_xml_unified_builtin
    test_p26_json_invalid_input
    test_p27_xml_invalid_input
    test_p28_filesystem_smoke
    test_p29_filesystem_invalid_input
    test_p30_database_availability
    test_p31_json_unicode_parse
    test_p32_json_unicode_stringify
    test_p33_operator_overloading
    test_p34_conversion_operators
    test_p35_print_operator
)

# P2 — تحسين / Enhancement
set(REGRESSION_P2_TESTS
    test_p05_mixed_chars
    test_p06_large_numbers
    test_p07_array_pluseq
    test_p15_utf8_windows
    test_p21_template_reserved
    test_p22_true_literal
)

foreach(TEST_NAME IN LISTS REGRESSION_P0_TESTS REGRESSION_P1_TESTS REGRESSION_P2_TESTS)
    # تحقق من وجود الملف بلاحقة .ص
    set(TEST_FILE "${REGRESSION_TEST_DIR}/${TEST_NAME}.ص")
    if(EXISTS "${TEST_FILE}")
        add_test(
            NAME "Regression_${TEST_NAME}"
            COMMAND $<TARGET_FILE:sad> "${TEST_FILE}"
        )
        set_tests_properties("Regression_${TEST_NAME}" PROPERTIES
            TIMEOUT 15
            LABELS "regression"
        )
        message(STATUS "  📋 Regression: ${TEST_NAME}")
    endif()
endforeach()

# الحلقة اللانهائية تحتاج مهلة قصيرة / Infinite loop test needs shorter timeout
if(TARGET sad)
    set_tests_properties("Regression_test_p02_while_update" PROPERTIES TIMEOUT 5)
endif()

message(STATUS "  ✅ test_regression_comprehensive (30 regression tests)")

# ──────────────────────────────────────────────────────────────────────
# اختبارات ?. و ?? عبر ملفات .ص / Optional Chain & Null Coalesce .ص tests
# ──────────────────────────────────────────────────────────────────────
if(TARGET sad)
    set(OPT_TEST_DIR "${CMAKE_SOURCE_DIR}/tests/ownership")
    file(GLOB OPT_NULL_TESTS "${OPT_TEST_DIR}/test_optional_null*")
    foreach(OPT_FILE IN LISTS OPT_NULL_TESTS)
        get_filename_component(OPT_NAME "${OPT_FILE}" NAME_WE)
        add_test(NAME "OptionalNull_${OPT_NAME}" COMMAND $<TARGET_FILE:sad> "${OPT_FILE}")
        set_tests_properties("OptionalNull_${OPT_NAME}" PROPERTIES TIMEOUT 15 LABELS "optional_null")
        message(STATUS "  [test] OptionalNull: ${OPT_NAME}")
    endforeach()
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات المترجم freestanding عبر sadc / Freestanding Compiler Tests (sadc)
# ──────────────────────────────────────────────────────────────────────
# تتطلب بناء sadc (LLVM مفعّل) — تشغل سكريبت PowerShell يتحقق من أنماط LLVM IR
# Requires sadc target (LLVM enabled) — runs PowerShell script that verifies LLVM IR patterns
if(TARGET sadc AND WIN32)
    find_program(POWERSHELL_EXE powershell)
    if(POWERSHELL_EXE)
        add_test(
            NAME "Compiler_Freestanding_BugFixes"
            COMMAND ${POWERSHELL_EXE}
                -ExecutionPolicy Bypass
                -File "${CMAKE_SOURCE_DIR}/tests/compiler/run_freestanding_tests.ps1"
                -SadcPath "$<TARGET_FILE:sadc>"
                -TestDir "${CMAKE_SOURCE_DIR}/tests/compiler"
        )
        set_tests_properties("Compiler_Freestanding_BugFixes" PROPERTIES
            TIMEOUT 120
            LABELS "compiler;freestanding"
        )
        message(STATUS "  [test] Compiler_Freestanding_BugFixes (sadc IR verification)")
    else()
        message(STATUS "  [skip] Compiler_Freestanding_BugFixes: PowerShell not found")
    endif()
else()
    message(STATUS "  [skip] Compiler_Freestanding_BugFixes: sadc target not available or not Windows")
endif()

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
        test_ast_clone
        test_docs_extractor_comprehensive
        test_vm_compiler_comprehensive
        test_jit_comprehensive
        test_e2e_comprehensive
        test_utils_modules_comprehensive
        test_compiler_comprehensive
        test_regression_comprehensive
        test_optional_null_comprehensive
    COMMENT "بناء جميع الاختبارات الشاملة / Building all comprehensive tests"
)

message(STATUS "════════════════════════════════════════════════════════")
message(STATUS "  الاختبارات الشاملة: 12 ملف | 900+ اختبار + 30 انحدار + freestanding")
message(STATUS "  بناء الكل: cmake --build build --target comprehensive_tests")
message(STATUS "  تشغيل الكل: ctest --test-dir build -R Comprehensive")
message(STATUS "  تشغيل الانحدار: ctest --test-dir build -L regression")
message(STATUS "  تشغيل المترجم: ctest --test-dir build -L freestanding")
message(STATUS "════════════════════════════════════════════════════════")
