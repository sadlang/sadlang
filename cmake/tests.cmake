# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/tests.cmake
# الوصف: جميع الاختبارات (unit, integration, stdlib) / All Tests
# ═══════════════════════════════════════════════════════════════════════════════

if(NOT BUILD_TESTS)
    return()
endif()

enable_testing()

# Find GTest (optional)
find_package(GTest CONFIG QUIET)

# ──────────────────────────────────────────────────────────────────────
# اختبارات المحلل النحوي / Parser Tests
# ──────────────────────────────────────────────────────────────────────
set(PARSER_TEST_SOURCES tests/unit/parser/test_runner.cpp)

if(EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/parser/test_lambda_simple.cpp")
    add_executable(parser_tests ${PARSER_TEST_SOURCES})
    target_link_libraries(parser_tests PRIVATE sad_core)
    target_include_directories(parser_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests/unit/parser)
    set_target_properties(parser_tests PROPERTIES
        OUTPUT_NAME "parser_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME ParserTests COMMAND parser_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات المحلل النحوي / Parser tests enabled")
else()
    message(STATUS "⚠ اختبارات المحلل النحوي غير متاحة / Parser tests not available")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات تحليل الوحدات / Module Parsing Tests
# ──────────────────────────────────────────────────────────────────────
if(FALSE AND EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/parser/test_module_parsing.cpp")
    add_executable(test_module_parsing tests/unit/parser/test_module_parsing.cpp)
    target_link_libraries(test_module_parsing PRIVATE sad_core)
    target_include_directories(test_module_parsing PRIVATE
        ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/tests/unit/parser)
    set_target_properties(test_module_parsing PROPERTIES
        OUTPUT_NAME "test_module_parsing" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME ModuleParsingTests COMMAND test_module_parsing WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات تحليل الوحدات / Module parsing tests enabled")
else()
    message(STATUS "⚠ معطّل: includes قديمة تحتاج إعادة كتابة / Disabled: stale includes need rewrite (test_module_parsing)")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات عُقد الأنواع / Type Nodes Tests
# ──────────────────────────────────────────────────────────────────────
if(FALSE AND EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/parser/test_type_nodes.cpp")
    add_executable(test_type_nodes tests/unit/parser/test_type_nodes.cpp)
    target_link_libraries(test_type_nodes PRIVATE sad_core)
    target_include_directories(test_type_nodes PRIVATE
        ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/tests/unit/parser)
    set_target_properties(test_type_nodes PROPERTIES
        OUTPUT_NAME "test_type_nodes" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME TypeNodesTests COMMAND test_type_nodes WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات عُقد الأنواع / Type nodes tests enabled")
else()
    message(STATUS "⚠ معطّل: includes قديمة تحتاج إعادة كتابة / Disabled: stale includes need rewrite (test_type_nodes)")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات محمل الوحدات / Module Loader Tests
# ──────────────────────────────────────────────────────────────────────
if(FALSE AND EXISTS "${CMAKE_SOURCE_DIR}/tests/modules/test_module_loader.cpp")
    add_executable(test_module_loader tests/modules/test_module_loader.cpp)
    target_link_libraries(test_module_loader PRIVATE sad_core)
    target_include_directories(test_module_loader PRIVATE
        ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/tests/modules)
    set_target_properties(test_module_loader PROPERTIES
        OUTPUT_NAME "test_module_loader" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME ModuleLoaderTests COMMAND test_module_loader WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات محمل الوحدات / Module loader tests enabled")
else()
    message(STATUS "⚠ معطّل: includes قديمة تحتاج إعادة كتابة / Disabled: stale includes need rewrite (test_module_loader)")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات محلل الرموز / Symbol Resolver Tests
# ──────────────────────────────────────────────────────────────────────
if(FALSE AND EXISTS "${CMAKE_SOURCE_DIR}/tests/modules/test_symbol_resolver.cpp")
    add_executable(test_symbol_resolver tests/modules/test_symbol_resolver.cpp)
    target_link_libraries(test_symbol_resolver PRIVATE sad_core)
    target_include_directories(test_symbol_resolver PRIVATE
        ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/tests/modules)
    set_target_properties(test_symbol_resolver PROPERTIES
        OUTPUT_NAME "test_symbol_resolver" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME SymbolResolverTests COMMAND test_symbol_resolver WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات محلل الرموز / Symbol resolver tests enabled")
else()
    message(STATUS "⚠ معطّل: includes قديمة تحتاج إعادة كتابة / Disabled: stale includes need rewrite (test_symbol_resolver)")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات محقق صحة الوحدات / Module Validator Tests
# ──────────────────────────────────────────────────────────────────────
if(FALSE AND EXISTS "${CMAKE_SOURCE_DIR}/tests/modules/test_module_validator.cpp")
    add_executable(test_module_validator tests/modules/test_module_validator.cpp)
    target_link_libraries(test_module_validator PRIVATE sad_core)
    target_include_directories(test_module_validator PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME ModuleValidatorTests COMMAND test_module_validator WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات محقق صحة الوحدات / Module validator tests enabled")
else()
    message(STATUS "⚠ معطّل: includes قديمة تحتاج إعادة كتابة / Disabled: stale includes need rewrite (test_module_validator)")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات التكامل الشاملة / End-to-End Integration Tests
# ──────────────────────────────────────────────────────────────────────
if(GTest_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/tests/modules/integration/test_end_to_end.cpp")
    add_executable(test_integration_e2e tests/modules/integration/test_end_to_end.cpp)
    target_link_libraries(test_integration_e2e PRIVATE GTest::gtest sad_core)
    target_include_directories(test_integration_e2e PRIVATE ${CMAKE_SOURCE_DIR}/include)
    add_test(NAME IntegrationEndToEndTests COMMAND test_integration_e2e WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات التكامل / End-to-end integration tests enabled")
else()
    message(STATUS "⚠ اختبارات التكامل غير متاحة / Integration tests not available")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات بناء SIR / SIR Builder Tests
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/sir/test_sir_builder.cpp")
    add_executable(sir_builder_tests tests/unit/sir/test_sir_builder.cpp)
    target_link_libraries(sir_builder_tests PRIVATE sad_core)
    target_include_directories(sir_builder_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/compiler/frontend/include ${CMAKE_SOURCE_DIR}/parser/include)
    set_target_properties(sir_builder_tests PROPERTIES
        OUTPUT_NAME "sir_builder_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME SIRBuilderTests COMMAND sir_builder_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات بناء SIR / SIR Builder tests enabled")
else()
    message(STATUS "⚠ اختبارات بناء SIR غير متاحة / SIR Builder tests not available")
endif()

# SIR Integration Tests
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/sir/test_sir_integration.cpp")
    add_executable(sir_integration_tests tests/unit/sir/test_sir_integration.cpp)
    target_link_libraries(sir_integration_tests PRIVATE sad_core)
    target_include_directories(sir_integration_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/compiler/frontend/include ${CMAKE_SOURCE_DIR}/parser/include)
    set_target_properties(sir_integration_tests PROPERTIES
        OUTPUT_NAME "sir_integration_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME SIRIntegrationTests COMMAND sir_integration_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات تكامل SIR / SIR Integration tests enabled")
else()
    message(STATUS "⚠ اختبارات تكامل SIR غير متاحة / SIR Integration tests not available")
endif()

# SIR Edge Cases Tests
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/sir/test_sir_edge_cases.cpp")
    add_executable(sir_edge_cases_tests tests/unit/sir/test_sir_edge_cases.cpp)
    target_link_libraries(sir_edge_cases_tests PRIVATE sad_core)
    target_include_directories(sir_edge_cases_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/compiler/frontend/include ${CMAKE_SOURCE_DIR}/parser/include)
    set_target_properties(sir_edge_cases_tests PROPERTIES
        OUTPUT_NAME "sir_edge_cases_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME SIREdgeCasesTests COMMAND sir_edge_cases_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات الحالات الحدية SIR / SIR Edge Cases tests enabled")
else()
    message(STATUS "⚠ اختبارات الحالات الحدية SIR غير متاحة / SIR Edge Cases tests not available")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات دوال الإدخال والإخراج / I/O Functions Tests
# ──────────────────────────────────────────────────────────────────────
add_executable(io_functions_tests tests/unit/stdlib/test_io_simple.cpp)
target_link_libraries(io_functions_tests PRIVATE sad_core)
target_include_directories(io_functions_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
set_target_properties(io_functions_tests PROPERTIES
    OUTPUT_NAME "io_functions_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_test(NAME IOFunctionsTests COMMAND io_functions_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# ──────────────────────────────────────────────────────────────────────
# اختبارات Code Actions وتعافي الأخطاء في LSP / LSP Code Actions Tests
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/system/lsp/test_lsp_code_actions.cpp")
    add_executable(lsp_code_actions_tests tests/system/lsp/test_lsp_code_actions.cpp)
    target_link_libraries(lsp_code_actions_tests PRIVATE sad_core)
    set_target_properties(lsp_code_actions_tests PROPERTIES
        OUTPUT_NAME "lsp_code_actions_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME LspCodeActionsTests COMMAND lsp_code_actions_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات Code Actions في LSP / LSP Code Actions tests enabled")
else()
    message(STATUS "⚠ اختبارات Code Actions غير متاحة / LSP Code Actions tests not available")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات محرّك LSP الشاملة + اختبارات السلوك / LSP Engine & Behavior Tests
# تختبر صنف LspEngine مباشرةً (القلب المجّانيّ بعد إزالة المزوّدات المتقدّمة).
# تربط sad_lsp_engine (المحرّك) + sad_core، وتستعمل إطار sad_test.h.
# ──────────────────────────────────────────────────────────────────────
foreach(_lsp_test
        test_lsp_engine_comprehensive:LspEngineComprehensiveTests:lsp_engine_comprehensive_tests
        test_lsp_navigation_comprehensive:LspNavigationComprehensiveTests:lsp_navigation_comprehensive_tests
        test_lsp_behavior:LspBehaviorTests:lsp_behavior_tests)
    string(REPLACE ":" ";" _parts "${_lsp_test}")
    list(GET _parts 0 _src)
    list(GET _parts 1 _ctest_name)
    list(GET _parts 2 _target)
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/system/lsp/${_src}.cpp")
        add_executable(${_target} tests/system/lsp/${_src}.cpp)
        target_link_libraries(${_target} PRIVATE sad_lsp_engine sad_core)
        target_include_directories(${_target} PRIVATE
            ${CMAKE_SOURCE_DIR}/tools/lsp/include
            ${CMAKE_SOURCE_DIR}/tests/framework)
        set_target_properties(${_target} PROPERTIES
            OUTPUT_NAME "${_target}" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
        add_test(NAME ${_ctest_name} COMMAND ${_target} WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        set_tests_properties(${_ctest_name} PROPERTIES TIMEOUT 120 LABELS "System")
        message(STATUS "✓ ${_ctest_name} مفعّلة / enabled")
    else()
        message(STATUS "⚠ ${_src} غير متاح / not available")
    endif()
endforeach()

# ──────────────────────────────────────────────────────────────────────
# اختبارات محسّن اللغة العربية / Arabic Optimizer Tests
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/test_arabic_optimizer.cpp")
    add_executable(test_arabic_optimizer tests/test_arabic_optimizer.cpp)
    target_include_directories(test_arabic_optimizer PRIVATE
        ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src/compiler/llvm/include)
    set_target_properties(test_arabic_optimizer PROPERTIES
        OUTPUT_NAME "test_arabic_optimizer" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME ArabicOptimizerTests COMMAND test_arabic_optimizer WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات محسّن اللغة العربية (standalone) / Arabic Optimizer tests enabled")
else()
    message(STATUS "⊘ اختبارات محسّن اللغة العربية معطلة (ملف مفقود) / Arabic Optimizer tests disabled (missing file)")
endif()

# ──────────────────────────────────────────────────────────────────────
# قياسات أداء التحسينات / Optimization Benchmarks
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/benchmark_arabic_opt.cpp")
    add_executable(benchmark_arabic_opt tests/benchmark_arabic_opt.cpp)
    target_include_directories(benchmark_arabic_opt PRIVATE ${CMAKE_SOURCE_DIR}/include)
    set_target_properties(benchmark_arabic_opt PROPERTIES
        OUTPUT_NAME "benchmark_arabic_opt" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    message(STATUS "✓ قياسات أداء محسّن اللغة العربية / Arabic Optimizer benchmarks enabled")
else()
    message(STATUS "⊘ قياسات أداء محسّن اللغة العربية معطلة (ملف مفقود) / Arabic Optimizer benchmarks disabled (missing file)")
endif()

# ──────────────────────────────────────────────────────────────────────
# المحسن المتقدم (معطل - يحتاج GTest) / Advanced Optimizer (disabled)
# ──────────────────────────────────────────────────────────────────────
# DISABLED: uses old include path (parser/parser_core.h)
if(FALSE AND EXISTS "${CMAKE_SOURCE_DIR}/tests/optimizer/advanced_optimizer_test.cpp")
    add_executable(optimizer_tests tests/optimizer/advanced_optimizer_test.cpp)
    target_link_libraries(optimizer_tests PRIVATE sad_core)
    target_include_directories(optimizer_tests PRIVATE ${CMAKE_SOURCE_DIR})
    set_target_properties(optimizer_tests PROPERTIES
        OUTPUT_NAME "optimizer_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME OptimizerTests COMMAND optimizer_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات المحسن المتقدم / Advanced Optimizer tests enabled")
else()
    message(STATUS "⚠ اختبارات المحسن المتقدم غير متاحة / Advanced Optimizer tests not available")
endif()

# المحسن البسيط / Simple Optimizer Tests
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/optimizer/test_optimizer_simple.cpp")
    add_executable(optimizer_simple_tests tests/optimizer/test_optimizer_simple.cpp)
    target_link_libraries(optimizer_simple_tests PRIVATE sad_core)
    target_include_directories(optimizer_simple_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
    set_target_properties(optimizer_simple_tests PROPERTIES
        OUTPUT_NAME "optimizer_simple_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME OptimizerSimpleTests COMMAND optimizer_simple_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات المحسن البسيطة / Simple Optimizer tests enabled")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات البرمجة الكائنية / OOP Tests
# DISABLED: includes reference old paths (include/data/types/, include/parser/ast/)
# that no longer exist after shared/ reorganization. Needs full rewrite.
# ──────────────────────────────────────────────────────────────────────
if(FALSE AND EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/oop/class_declaration_tests.cpp")
    add_executable(oop_tests tests/unit/oop/class_declaration_tests.cpp)
    target_link_libraries(oop_tests PRIVATE sad_core)
    target_include_directories(oop_tests PRIVATE ${CMAKE_SOURCE_DIR})
    set_target_properties(oop_tests PROPERTIES
        OUTPUT_NAME "oop_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME OOPTests COMMAND oop_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات البرمجة الكائنية / OOP tests enabled")
else()
    message(STATUS "⚠ اختبارات البرمجة الكائنية معطلة (تحتاج إعادة كتابة) / OOP tests disabled (needs rewrite)")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات الواجهة الأمامية للمترجم / Compiler Frontend Tests
# ──────────────────────────────────────────────────────────────────────

# SIR Types Tests
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/sir/test_sir_types.cpp"
   AND EXISTS "${CMAKE_SOURCE_DIR}/compiler/frontend/src/type_info.cpp")
    add_executable(sir_types_tests
        tests/unit/sir/test_sir_types.cpp
        compiler/frontend/src/type_info.cpp)
    target_link_libraries(sir_types_tests PRIVATE sad_core)
    target_include_directories(sir_types_tests PRIVATE ${CMAKE_SOURCE_DIR}/compiler/frontend/include)
    set_target_properties(sir_types_tests PROPERTIES
        OUTPUT_NAME "sir_types_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME SIRTypesTests COMMAND sir_types_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات أنواع SIR / SIR Types tests enabled")
else()
    message(STATUS "⚠ اختبارات أنواع SIR غير متاحة / SIR Types tests not available")
endif()

# SIR Instruction Tests (updated to current API - February 2026)
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/sir/test_sir_instruction.cpp")
    add_executable(sir_instruction_tests tests/unit/sir/test_sir_instruction.cpp)
    target_link_libraries(sir_instruction_tests PRIVATE sad_core)
    target_include_directories(sir_instruction_tests PRIVATE ${CMAKE_SOURCE_DIR}/compiler/frontend/include)
    set_target_properties(sir_instruction_tests PROPERTIES
        OUTPUT_NAME "sir_instruction_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME SIRInstructionTests COMMAND sir_instruction_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات تعليمات SIR / SIR Instruction tests enabled")
else()
    message(STATUS "⚠ اختبارات تعليمات SIR غير متاحة / SIR Instruction tests not available")
endif()

message(STATUS "⊘ اختبارات وحدات SIR معطلة مؤقتاً / SIR Module tests disabled (WIP)")

# SIR Optimizer Tests
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/sir/optimizer/CMakeLists.txt")
    add_subdirectory(tests/unit/sir/optimizer)
    message(STATUS "✓ اختبارات محسّن SIR / SIR Optimizer tests enabled")
else()
    message(STATUS "⚠ اختبارات محسّن SIR غير متاحة / SIR Optimizer tests not available")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات المستوى المنخفض / Low-Level Tests
# ──────────────────────────────────────────────────────────────────────
message(STATUS "⊘ اختبارات نظام المؤشرات معطلة مؤقتاً / Pointer system tests disabled (WIP)")

if(EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/low_level/test_bitwise.cpp")
    add_executable(bitwise_tests tests/unit/low_level/test_bitwise.cpp)
    target_link_libraries(bitwise_tests PRIVATE sad_core)
    target_include_directories(bitwise_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
    set_target_properties(bitwise_tests PROPERTIES
        OUTPUT_NAME "bitwise_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME BitwiseTests COMMAND bitwise_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات العمليات الثنائية / Bitwise operations tests enabled")
else()
    message(STATUS "⚠ اختبارات العمليات الثنائية غير متاحة / Bitwise operations tests not available")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات المكتبات القياسية / Standard Library Tests
# ──────────────────────────────────────────────────────────────────────

# اختبارات نظام الملفات / Filesystem Tests (تحتاج GTest)
if(GTest_FOUND)
    add_executable(filesystem_tests tests/unit/stdlib/test_filesystem_module.cpp)
    target_include_directories(filesystem_tests PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(filesystem_tests PRIVATE sad_core GTest::gtest GTest::gtest_main)
    set_target_properties(filesystem_tests PROPERTIES
        OUTPUT_NAME "filesystem_tests" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
    message(STATUS "✓ اختبارات نظام الملفات / Filesystem tests enabled")

    # اختبارات HTTP / HTTP Tests
    add_executable(http_tests tests/unit/stdlib/test_http_module.cpp)
    target_include_directories(http_tests PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(http_tests PRIVATE sad_core GTest::gtest GTest::gtest_main)
    if(WIN32)
        target_link_libraries(http_tests PRIVATE ws2_32)
    endif()
    set_target_properties(http_tests PROPERTIES
        OUTPUT_NAME "http_tests" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
    message(STATUS "✓ اختبارات HTTP / HTTP tests enabled")

    # اختبارات JSON/XML / JSON/XML Tests
    add_executable(json_xml_tests tests/unit/stdlib/test_json_module.cpp tests/unit/stdlib/test_xml_module.cpp)
    target_include_directories(json_xml_tests PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(json_xml_tests PRIVATE sad_core GTest::gtest GTest::gtest_main)
    set_target_properties(json_xml_tests PROPERTIES
        OUTPUT_NAME "json_xml_tests" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
    message(STATUS "✓ اختبارات JSON/XML / JSON/XML tests enabled")
else()
    message(STATUS "⚠ اختبارات GTest (filesystem, http, json_xml) غير متاحة — GTest not found")
endif()

# اختبارات HTTP الجديدة (تحتاج GTest) / New HTTP Tests (requires GTest)
if(BUILD_TESTS AND SAD_ENABLE_NETWORK_TESTS AND TARGET sad_http AND GTest_FOUND)
    add_subdirectory(tests/system/network/http)
endif()

# اختبارات التشفير / Cryptography Tests
if(OPENSSL_FOUND)
    add_executable(crypto_tests tests/unit/stdlib/test_crypto_module.cpp)
    target_include_directories(crypto_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(crypto_tests PRIVATE
        sad_core GTest::gtest GTest::gtest_main OpenSSL::SSL OpenSSL::Crypto)
    set_target_properties(crypto_tests PROPERTIES
        OUTPUT_NAME "crypto_tests" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات الوحدات الإضافية / Additional Module Tests (Sprint 1 — ريم)
# ──────────────────────────────────────────────────────────────────────
# ملاحظة: تم نقل الاختبارات إلى tests/unit/comprehensive/ واستخدام sad_test_framework.h
# بدلاً من GTest. انظر cmake/tests_comprehensive.cmake للتسجيل.
# Note: Tests moved to tests/unit/comprehensive/ using sad_test_framework.h
# instead of GTest. See cmake/tests_comprehensive.cmake for registration.

# Type Checker Tests
if(EXISTS "${CMAKE_SOURCE_DIR}/compiler/frontend/type_checker/tests/CMakeLists.txt")
    add_subdirectory(compiler/frontend/type_checker/tests)
    message(STATUS "✓ اختبارات Type Checker / Type Checker tests enabled")
else()
    message(STATUS "⚠ اختبارات Type Checker غير متاحة / Type Checker tests not available")
endif()

# ──────────────────────────────────────────────────────────────────────
# (AR) اختبارات الهب و sad-check / (EN) Hub & sad-check tests
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/system/hub/CMakeLists.txt")
    add_subdirectory(tests/system/hub)
endif()

# ──────────────────────────────────────────────────────────────────────
# (AR) اختبارات sadinfo (Story 1.2)
# (EN) sadinfo tests (Story 1.2)
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/system/sadinfo/CMakeLists.txt")
    add_subdirectory(tests/system/sadinfo)
endif()

# ──────────────────────────────────────────────────────────────────────
# (AR) TEST-004 AC-02: وسم Unit للاختبارات الفردية لطبقة الوحدة —
#      يُكمل وسم ماكرو الحزمة الشاملة في tests_comprehensive.cmake،
#      فيشغّل `ctest -L Unit` الطبقة كلها. حارس if(TEST) يتخطى المعطّل.
# (EN) TEST-004 AC-02: Unit label for individual unit-layer tests —
#      complements the comprehensive macro labeling, so `ctest -L Unit`
#      runs the whole layer. The if(TEST) guard skips disabled ones.
# ──────────────────────────────────────────────────────────────────────
foreach(_unit_test IN ITEMS ParserTests SIRBuilderTests SIRIntegrationTests
        SIREdgeCasesTests SIRInstructionTests SIRTypesTests IOFunctionsTests
        BitwiseTests TypeBridgeTests LspCodeActionsTests OptimizerSimpleTests)
    if(TEST ${_unit_test})
        set_property(TEST ${_unit_test} APPEND PROPERTY LABELS "Unit")
    endif()
endforeach()

# ──────────────────────────────────────────────────────────────────────
# (AR) TEST-005 AC-04: بوّابة العتبات غير الوظيفية (NFR) — تقيس أزمنة برامج
#      مرجعية (وسيط 7 تشغيلات) وتفشل عند تجاوز خط الأساس المقيس × (1+التسامح)
#      من tests/system/benchmark/nfr_thresholds.yaml — لا أرقام سحرية (CW-10).
#      إعادة المعايرة قرار بشري: python nfr_gate.py --calibrate (ليست خطوة CI).
# (EN) TEST-005 AC-04: NFR gate — median-of-7 timings of reference programs;
#      fails on regression beyond calibrated baseline × (1 + tolerance).
#      Recalibration is a human action (--calibrate), never a CI step.
# ──────────────────────────────────────────────────────────────────────
find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/tests/system/benchmark/nfr_gate.py" AND TARGET sad-run)
    # (AR) ملاحظة: هدف المفسر اسمه `sad` (OUTPUT_NAME=sad-run)؛ `sad-run` مجرد
    #      custom target للاكتشاف ولا يملك TARGET_FILE.
    # (EN) Note: the interpreter target is `sad` (OUTPUT_NAME=sad-run).
    add_test(
        NAME NFR_Gate
        COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/tests/system/benchmark/nfr_gate.py"
                --interp "$<TARGET_FILE:sad-run>"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    # (AR) RUN_SERIAL: يقيس أزمنة تنفيذ بالمللي ثانية على برامج صغيرة جداً
    #      (overhead بدء العملية يُشكِّل أغلب الزمن المقيس) — حساس جداً لمنافسة
    #      CPU مع أي اختبار آخر يعمل بالتوازي (ctest -j)؛ شوهد عملياً يفشل
    #      بفارق ~8% فقط تحت تزامن -j4 بينما ينجح بسهولة عند العزل.
    # (EN) RUN_SERIAL: measures millisecond-scale timings of tiny programs
    #      (process-startup overhead dominates) — highly sensitive to CPU
    #      contention from any test running concurrently (ctest -j); observed
    #      failing by only ~8% under -j4 contention while passing easily isolated.
    set_tests_properties(NFR_Gate PROPERTIES TIMEOUT 180 LABELS "System;nfr" RUN_SERIAL TRUE)
    message(STATUS "✓ بوّابة NFR / NFR gate enabled (tests/system/benchmark)")
endif()

message(STATUS "✓ الاختبارات مفعلة / Tests enabled")
