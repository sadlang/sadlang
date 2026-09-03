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
    target_link_libraries(parser_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
    target_include_directories(parser_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests/unit/parser)
    set_target_properties(parser_tests PROPERTIES
        OUTPUT_NAME "parser_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME ParserTests COMMAND parser_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات المحلل النحوي / Parser tests enabled")
else()
    message(STATUS "⚠ اختبارات المحلل النحوي غير متاحة / Parser tests not available")
endif()

# ──────────────────────────────────────────────────────────────────────
# (AR) اختبارات وحدة المحلّل لصياغة الاستيعابات بترتيب «أنتج» (RFC 25 م1ب)
#      تستعمل إطار sad_test.h وتتحقّق من عقد AST (List/Set/Dict ComprehensionExpr).
# (EN) Parser unit tests for the «أنتج» comprehension order (RFC 25 م1ب);
#      use the sad_test.h harness and assert on AST nodes.
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/parser/test_comprehensions_antaj.cpp")
    add_executable(comprehension_antaj_tests tests/unit/parser/test_comprehensions_antaj.cpp)
    target_link_libraries(comprehension_antaj_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
    target_include_directories(comprehension_antaj_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/framework
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include)
    set_target_properties(comprehension_antaj_tests PROPERTIES
        OUTPUT_NAME "comprehension_antaj_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME ComprehensionAntajTests COMMAND comprehension_antaj_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    set_property(TEST ComprehensionAntajTests APPEND PROPERTY LABELS "Unit")
    message(STATUS "✓ اختبارات استيعاب أنتج / Comprehension أنتج unit tests enabled")
else()
    message(STATUS "⚠ اختبارات استيعاب أنتج غير متاحة / Comprehension أنتج tests not available")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات تحليل الوحدات / Module Parsing Tests
# ──────────────────────────────────────────────────────────────────────
if(FALSE AND EXISTS "${CMAKE_SOURCE_DIR}/tests/unit/parser/test_module_parsing.cpp")
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
    target_link_libraries(test_type_nodes PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
    target_link_libraries(test_module_loader PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
    target_link_libraries(test_symbol_resolver PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
    target_link_libraries(test_module_validator PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
    target_link_libraries(test_integration_e2e PRIVATE GTest::gtest sad_shared sad_frontend)
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
    target_link_libraries(sir_builder_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
    target_link_libraries(sir_integration_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
    target_link_libraries(sir_edge_cases_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
target_link_libraries(io_functions_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
target_include_directories(io_functions_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
set_target_properties(io_functions_tests PROPERTIES
    OUTPUT_NAME "io_functions_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_test(NAME IOFunctionsTests COMMAND io_functions_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

# ──────────────────────────────────────────────────────────────────────
# اختبارات Code Actions وتعافي الأخطاء في LSP / LSP Code Actions Tests
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/tests/system/lsp/test_lsp_code_actions.cpp")
    add_executable(lsp_code_actions_tests tests/system/lsp/test_lsp_code_actions.cpp)
    # (AR) م4: اختبار LSP تحليلٌ ساكن ⇒ يربط sad_lsp_engine (يوفّر sad_shared+type_system
    #      عامًّا) بدل sad_core (المفسّر الكامل). لا رموز Sad::Interpreter:: في اختبارات LSP.
    # (EN) Phase-4: LSP test is static analysis ⇒ links sad_lsp_engine (PUBLIC sad_shared
    #      + sad_type_system) instead of sad_core (the whole interpreter). No Sad::Interpreter:: symbols.
    target_link_libraries(lsp_code_actions_tests PRIVATE sad_lsp_engine)
    set_target_properties(lsp_code_actions_tests PROPERTIES
        OUTPUT_NAME "lsp_code_actions_tests" RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
    add_test(NAME LspCodeActionsTests COMMAND lsp_code_actions_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    message(STATUS "✓ اختبارات Code Actions في LSP / LSP Code Actions tests enabled")
else()
    message(STATUS "⚠ اختبارات Code Actions غير متاحة / LSP Code Actions tests not available")
endif()

# ──────────────────────────────────────────────────────────────────────
# اختبارات محرّك LSP الشاملة + اختبارات السلوك / LSP Engine & Behavior Tests
# (AR) تختبر صنف LspEngine مباشرةً (القلب المجّانيّ بعد إزالة المزوّدات المتقدّمة).
#      تربط sad_lsp_engine (المحرّك، يوفّر sad_shared عامًّا — م4)، وتستعمل إطار sad_test.h.
# (EN) Test the LspEngine class directly (free core after removing advanced providers).
#      Link sad_lsp_engine (engine, PUBLIC sad_shared — Phase-4); use the sad_test.h harness.
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
        # (AR) م4: sad_core أُسقط (sad_lsp_engine يوفّر sad_shared عامًّا)
        # (EN) Phase-4: sad_core dropped (sad_lsp_engine provides sad_shared PUBLICly)
        target_link_libraries(${_target} PRIVATE sad_lsp_engine)
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
    target_link_libraries(optimizer_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
    target_link_libraries(optimizer_simple_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
    target_link_libraries(sir_types_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
    target_link_libraries(sir_instruction_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
    target_link_libraries(bitwise_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
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
    target_link_libraries(filesystem_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system GTest::gtest GTest::gtest_main)
    set_target_properties(filesystem_tests PROPERTIES
        OUTPUT_NAME "filesystem_tests" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
    message(STATUS "✓ اختبارات نظام الملفات / Filesystem tests enabled")

    # اختبارات HTTP / HTTP Tests
    add_executable(http_tests tests/unit/stdlib/test_http_module.cpp)
    target_include_directories(http_tests PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(http_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system GTest::gtest GTest::gtest_main)
    if(WIN32)
        target_link_libraries(http_tests PRIVATE ws2_32)
    endif()
    set_target_properties(http_tests PROPERTIES
        OUTPUT_NAME "http_tests" RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")
    message(STATUS "✓ اختبارات HTTP / HTTP tests enabled")

    # اختبارات JSON/XML / JSON/XML Tests
    add_executable(json_xml_tests tests/unit/stdlib/test_json_module.cpp tests/unit/stdlib/test_xml_module.cpp)
    target_include_directories(json_xml_tests PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/src)
    target_link_libraries(json_xml_tests PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system GTest::gtest GTest::gtest_main)
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
# (AR) 🔑 كان الشرطُ `TARGET sad-run`. فلمّا زال الهدفُ سقطتِ البوّابةُ **صامتةً**:
#      `ctest -N` لا يعدُّها، ولا رسالةَ تُطبَع، وبقيَ `NFR_Gate_SelfTest` أخضرَ
#      وحدَه — فقُرِئ الأخضرُ حراسةً والحارسُ غائب. وهذا أخبثُ من إخفاق.
#      والمقيسُ صار **زمنَ الترجمة** لا زمنَ التفسير، وخطُّ الأساسِ أُعيدت
#      معايرتُه في الجلسةِ نفسِها (٢٠٢٦-٠٩-٠٣) — إذ لا يُقرَنُ قياسُ اليومِ
#      بأساسٍ قيسَ لمحرّكٍ آخر.
# (EN) The condition used to be TARGET sad-run; when the target went, the gate
#      vanished silently — ctest -N counted zero while its self-test stayed green.
#      What is measured is now COMPILE time, and the baseline was recalibrated in
#      the same session: today's sample cannot pair with another engine's baseline.
if(Python3_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/tests/system/benchmark/nfr_gate.py" AND TARGET sad-build)
    add_test(
        NAME NFR_Gate
        COMMAND ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/tests/system/benchmark/nfr_gate.py"
                --مترجم "$<TARGET_FILE:sad-build>"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    # (AR) RUN_SERIAL: يقيس أزمنة ترجمة بالمللي ثانية — حساس لمنافسة CPU مع أي
    #      اختبار يعمل بالتوازي (ctest -j)؛ شوهد يفشل بفارق ~8% تحت تزامن -j4
    #      بينما ينجح بسهولة عند العزل. والترجمةُ أثقلُ من التشغيلِ فالحساسيّةُ أبقى.
    # (EN) RUN_SERIAL: millisecond-scale compile timings — sensitive to CPU
    #      contention under ctest -j; observed failing by ~8% at -j4 while passing
    #      easily when isolated. Compilation is heavier than a run, so this stands.
    set_tests_properties(NFR_Gate PROPERTIES TIMEOUT 180 LABELS "System;nfr" RUN_SERIAL TRUE)
    message(STATUS "✓ بوّابة NFR / NFR gate enabled (tests/system/benchmark)")
endif()

# ──────────────────────────────────────────────────────────────────────
# (AR) الاختبارُ الذاتيُّ لبوّابة NFR — بذرتان متعاكستان على دالّةِ الحكمِ الخالصة.
#      ⚠️ ولا يحتاج مفسّرًا مبنيًّا: يُغذّي `evaluate` بقياساتٍ مُصطنَعةٍ ويشترطُ
#      حكمَين متعاكسَين — سكوتًا على آلةٍ بطيئةٍ بالتساوي، وإخفاقًا على تراجعٍ
#      حقيقيٍّ ولو كانت الآلةُ بطيئةً فوقَه. لأنّ تحويلَ الحكمِ إلى نسبةٍ يوسّعُ
#      المقبولَ، وتوسيعٌ بلا شاهدٍ يُخفِقُ هو الطريقُ إلى حارسٍ لا يعضّ.
# (EN) NFR gate self-test: feeds the pure verdict function synthetic samples and
#      demands opposite verdicts — silent on a uniformly slow machine, failing on
#      a real regression even under one. Making a verdict relative widens what is
#      accepted; widening without a seed that still fails yields a toothless guard.
# ──────────────────────────────────────────────────────────────────────
if(Python3_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/tests/system/benchmark/nfr_gate.py")
    add_test(
        NAME NFR_Gate_SelfTest
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/tests/system/benchmark/nfr_gate.py"
                --self-test
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(NFR_Gate_SelfTest PROPERTIES TIMEOUT 60 LABELS "System;nfr;contract")
endif()

# ─────────────────────────────────────────────────────────────────────
# (AR) حارسُ سجلِّ الحمرةِ المقيسة — `tests/behavior/DECLARED_REDS.tsv`.
#      المجلّدان `grammar_gaps` و`_regression` خارجَ كلِّ مستوًى يُشغّلُه CI،
#      ففيهما ٦١ ملفًّا أحمرَ لا يراها أحد. هذا الاختبارُ في وضعِ **البِنية**:
#      يفحصُ صحّةَ السجلِّ ووجودَ ملفّاتِه بلا تشغيلِ حالةٍ واحدة — رخيصٌ ويصلحُ
#      لكلِّ منصّة. والحكمُ الكاملُ (--measure) في خطوةٍ داخلَ وظيفةِ `🐧 Linux (Release)`ٍ لأنّه يلزمه
#      محرّكان مبنيّان و.
#      🔑 والحارسُ يقولُ صراحةً «لم أَقِسْ» في هذا الوضع، فلا يُقرأُ أخضرُه
#      برهانًا على الحمرة — وهو عينُ العطبِ الذي بُنيَ ليمنعه.
# (EN) Guard for the measured-reds registry, in STRUCTURE mode: validates the
#      registry's shape and that every listed file exists, without running a
#      single case. Full judgement (--measure) lives in its own CI job. The
#      guard explicitly says "not measured" here, so its green is never read as
#      proof about the reds themselves.
# ─────────────────────────────────────────────────────────────────────
if(Python3_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/tests/system/harness/test_declared_reds_registry.py")
    add_test(
        NAME DeclaredReds_RegistryShape
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE}
                "${CMAKE_SOURCE_DIR}/tests/system/harness/test_declared_reds_registry.py"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DeclaredReds_RegistryShape PROPERTIES
        TIMEOUT 60 LABELS "System;harness;contract")
endif()

# ─────────────────────────────────────────────────────────────────────
# (AR) بوّابة وصل الخلفيّة الأصليّة (بلا LLVM) بالمُشغِّل — تُشغّل sad-build
#      نفسَه بـ«--خلفية-أصلية» وتحكم على بنية ELF64 الناتجة. اختباراتُ
#      الوحدة (NativeSirBridge وأخواتُها) تستدعي الهيدرات مباشرةً، فتخضرّ
#      حتّى لو لم يبلُغِ المخفّضَ مستخدِمٌ من سطر الأوامر؛ هذه البوّابة تسدّ
#      تلك الفجوة بالذّات. لا تُنفّذ الثنائيّات (ELF لينكس) — برهانُ التشغيل
#      الحيّ في scripts/native_backend/prove_*.sh.
# (EN) Gate proving the native (no-LLVM) backend is reachable from the driver:
#      runs sad-build with the flag and judges the resulting ELF64. Unit tests
#      call the headers directly and stay green even when no user can reach the
#      lowerer; this gate closes exactly that gap.
# ─────────────────────────────────────────────────────────────────────
if(Python3_FOUND AND TARGET sad-build
   AND EXISTS "${CMAKE_SOURCE_DIR}/tests/system/native_backend/native_flag_gate.py")
    add_test(
        NAME NativeBackendFlagGate
        COMMAND ${Python3_EXECUTABLE}
                "${CMAKE_SOURCE_DIR}/tests/system/native_backend/native_flag_gate.py"
                --compiler "$<TARGET_FILE:sad-build>"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(NativeBackendFlagGate PROPERTIES
        TIMEOUT 300 LABELS "System;native")
    message(STATUS "✓ بوّابة الخلفيّة الأصليّة / native-backend flag gate enabled")
endif()

# ─────────────────────────────────────────────────────────────────────
# (AR) مِقياسُ سطحِ اللغة — يشتقُّ عددَ كلِّ سطحٍ لغويٍّ من language-truth/
#      ويقارنُه بالسِّجلِّ المُودَع. أيُّ توسُّعٍ في اللغةِ لم يُودَع في السِّجلِّ
#      يُفشِلُ هذه البوّابة — فيصيرُ «كم توسَّعت اللغة؟» جوابًا مقيسًا لا نثرًا.
#      ⚠️ ولا يحرسُ أسطرَ الكودِ ولا عددَ الاختبارات: تلك تتغيّرُ كلَّ إيداعٍ،
#      فحارسٌ عليها يحمرُّ دائمًا ثمّ يُعطَّل ثمّ يخضرُّ بلا قياس.
#      التحديثُ قرارٌ بشريّ: python tests/metrics/surface/surface_map.py --update
# (EN) Language-surface meter: derives every countable surface from the SoT and
#      fails on any drift from the committed ledger. LOC/test counts are
#      deliberately NOT guarded (they change every commit).
# ─────────────────────────────────────────────────────────────────────
if(Python3_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/tests/metrics/surface/surface_map.py")
    add_test(
        NAME Lang_Surface_Check
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/tests/metrics/surface/surface_map.py"
                --check
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(Lang_Surface_Check PROPERTIES
        TIMEOUT 120 LABELS "System;metrics;contract")

    # (AR) الاختبارُ الذاتيُّ للمِقياس: بذورٌ متعاكسةٌ على دالّةِ الحكمِ الخالصة —
    #      سكوتٌ على التطابق، وإخفاقٌ على كلِّ صورةٍ من صورِ الانجرافِ الستّ.
    #      فحارسٌ لا تُبرهَنُ عضّتُه حارسٌ لا يعضّ.
    # (EN) Meter self-test: opposite seeds on the pure verdict function.
    add_test(
        NAME Lang_Surface_SelfTest
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/tests/metrics/surface/surface_map.py"
                --self-test
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(Lang_Surface_SelfTest PROPERTIES
        TIMEOUT 60 LABELS "System;metrics;contract")
    message(STATUS "✓ مِقياسُ سطحِ اللغة / language-surface meter enabled")
endif()

# ─────────────────────────────────────────────────────────────────────
# (AR) مِقياسُ الحجمِ الحتميّ — يقيسُ لكلِّ برنامجٍ مرجعيٍّ الرموزَ الخارجيّةَ
#      المجرورة (سطورُ `declare` في IR) وعددَ سطورِ IR وحجمَ ELF الأصليِّ
#      بالبايت، ويقارنُها بالسِّجلِّ المُودَع. رمزٌ خارجيٌّ يُجَرُّ حديثًا — `pow`
#      أو `floor` مثلًا — يُفشِلُ البوّابة، فجرُّ مكتبةٍ كاملةٍ لا يمرُّ صامتًا.
#      🔑 الهدفُ مثبَّتٌ في الأداة (x86_64-unknown-linux-gnu) فلا يقيسُ الرقمُ
#         آلةَ العدّاء، وحجمُ الثنائيِّ بعدَ رابطِ النظامِ خارجَ النطاقِ عمدًا.
#      ⚠️ ملاحظةٌ معلنة: تطابقُ المخرَجِ بين تهيئتَي Debug وRelease **غيرُ
#         مقيسٍ بعد**؛ كتلُ NDEBUG في مولّد الشيفرةِ المفحوصةُ طباعةٌ فقط، فلا
#         آليّةَ معروفةً للاختلاف. إن احمرَّت البوّابةُ في تهيئةٍ دونَ أخرى فذاك
#         نفسُه اكتشافٌ يستحقُّ المعالجة لا كتمَ الحارس.
#      التحديثُ قرارٌ بشريّ: --update --compiler <sad-build>
# (EN) Deterministic size meter: per-program dragged external symbols, IR line
#      count and native ELF byte size, compared against a committed ledger. A
#      newly dragged symbol (pow/floor) fails the gate. Target triple is pinned
#      inside the tool; final linked binary size is deliberately out of scope.
# ─────────────────────────────────────────────────────────────────────
if(Python3_FOUND AND TARGET sad-build
   AND EXISTS "${CMAKE_SOURCE_DIR}/tests/metrics/size/size_map.py")
    add_test(
        NAME Lang_Size_Check
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/tests/metrics/size/size_map.py"
                --check --compiler "$<TARGET_FILE:sad-build>"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(Lang_Size_Check PROPERTIES
        TIMEOUT 900 LABELS "System;metrics;contract")
    message(STATUS "✓ مِقياسُ الحجمِ الحتميّ / deterministic size meter enabled")
endif()

# (AR) الاختبارُ الذاتيُّ لمِقياسِ الحجم — لا يحتاجُ مترجمًا مبنيًّا: يُغذّي
#      الحكمَ الخالصَ ببذورٍ متعاكسة. فبوّابةٌ لا تُبرهَنُ عضّتُها إلّا بترجمةٍ
#      كاملةٍ لا تُبرهِنُها على عدّاءٍ بلا مترجم.
# (EN) Size-meter self-test: opposite seeds on the pure verdict; no compiler
#      needed, so the bite is proven even where sad-build was not built.
if(Python3_FOUND AND EXISTS "${CMAKE_SOURCE_DIR}/tests/metrics/size/size_map.py")
    add_test(
        NAME Lang_Size_SelfTest
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${CMAKE_SOURCE_DIR}/tests/metrics/size/size_map.py"
                --self-test
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(Lang_Size_SelfTest PROPERTIES
        TIMEOUT 60 LABELS "System;metrics;contract")
endif()

message(STATUS "✓ الاختبارات مفعلة / Tests enabled")
