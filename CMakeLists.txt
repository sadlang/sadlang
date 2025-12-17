# ======================================================================
# CMakeLists.txt - لغة ص البرمجية / Sad Programming Language
# ======================================================================
# الوصف / Description:
#   ملف بناء CMake الرئيسي لمشروع لغة ص البرمجية العربية
#   Main CMake build file for Sad Arabic Programming Language project
#
# المتطلبات / Requirements:
#   - CMake 3.15 أو أحدث / or newer
#   - مترجم C++17 / C++17 compatible compiler
#   - مكتبات النظام الأساسية / System standard libraries
#
# الاستخدام / Usage:
#   mkdir build && cd build
#   cmake ..
#   cmake --build .
# ======================================================================

cmake_minimum_required(VERSION 3.15)

# ======================================================================
# معلومات المشروع / Project Information
# ======================================================================
project(SadLanguage 
    VERSION 1.0.0
    DESCRIPTION "لغة ص - لغة برمجة عربية حديثة / Sad - Modern Arabic Programming Language"
    LANGUAGES CXX
)

# ======================================================================
# إعدادات C++ / C++ Settings
# ======================================================================
# استخدام معيار C++17 / Use C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ======================================================================
# مسارات الإخراج / Output Paths
# ======================================================================
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# ======================================================================
# خيارات البناء / Build Options
# ======================================================================
option(BUILD_TESTS "بناء الاختبارات / Build tests" OFF)
option(BUILD_EXAMPLES "بناء الأمثلة / Build examples" ON)
option(ENABLE_DEBUG_LOGGING "تفعيل سجلات التنقيح / Enable debug logging" OFF)
option(BUILD_PKG_MANAGER "بناء مدير الحزم / Build package manager" ON)

# ======================================================================
# تعريفات المعالج المسبق / Preprocessor Definitions
# ======================================================================
if(ENABLE_DEBUG_LOGGING)
    add_compile_definitions(SAD_DEBUG_ENABLED)
endif()

# إضافة تعريف لدعم UTF-8 على Windows
# Add definition for UTF-8 support on Windows
if(WIN32)
    add_compile_definitions(
        _UNICODE
        UNICODE
        NOMINMAX
    )
    # تفعيل دعم UTF-8 في Visual Studio
    # Enable UTF-8 support in Visual Studio
    add_compile_options(/utf-8)
endif()

# ======================================================================
# خيارات التحذيرات / Warning Options
# ======================================================================
if(MSVC)
    # خيارات مترجم Visual Studio / Visual Studio compiler options
    add_compile_options(
        /W4           # مستوى التحذيرات 4 / Warning level 4
        /WX-          # عدم اعتبار التحذيرات أخطاء / Don't treat warnings as errors
        /permissive-  # وضع المطابقة الصارمة / Standards conformance mode
        /FS           # إصلاح مشكلة PDB المتزامنة / Fix PDB synchronization issue
    )
else()
    # خيارات GCC/Clang
    add_compile_options(
        -Wall         # جميع التحذيرات / All warnings
        -Wextra       # تحذيرات إضافية / Extra warnings
        -Wpedantic    # تحذيرات المعيار / Pedantic warnings
    )
endif()

# ======================================================================
# مجلدات التضمين / Include Directories
# ======================================================================
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/compiler/frontend/include
)

# ======================================================================
# ملفات المصدر / Source Files
# ======================================================================

# الأدوات المساعدة / Utilities
set(UTILS_SOURCES
    src/utils/string_utils.cpp
)

# المحلل المعجمي / Lexer
set(LEXER_SOURCES
    src/lexer/token.cpp
    src/lexer/lexer_core.cpp
    src/lexer/lexer_keywords.cpp
)

# المحلل النحوي / Parser
set(PARSER_SOURCES
    src/parser/parser_core.cpp
    src/parser/parser_core_impl.cpp
    src/parser/parser_core_helpers.cpp
    src/parser/parser_core_oop.cpp
    src/parser/parser_modules.cpp
)

# البرمجة الكائنية - المحلل النحوي / OOP - Parser
# (AR) معطل مؤقتاً - يحتاج تكامل مع ParserCore
# (EN) Temporarily disabled - needs integration with ParserCore
set(OOP_PARSER_SOURCES
    # src/parser/oop/parser_classes.cpp
)

# عُقد الشجرة النحوية التجريدية / AST Nodes
set(AST_SOURCES
    src/parser/ast/ast_node.cpp
    src/parser/ast/expressions.cpp
    src/parser/ast/statements.cpp
    src/parser/ast/declarations.cpp
    src/parser/ast/property_nodes.cpp
    src/parser/ast/module_nodes.cpp
    src/parser/ast/ast_printer.cpp
)

# المفسر / Interpreter
set(INTERPRETER_SOURCES
    src/interpreter/core/interpreter_core.cpp
    src/interpreter/core/builtin_registry.cpp
    src/interpreter/visitors/expression_evaluator.cpp
    src/interpreter/visitors/statement_executor.cpp
    src/interpreter/visitors/statement_executor_oop.cpp
    src/interpreter/exception.cpp
)

# إدارة البيانات / Data Management
set(DATA_SOURCES
    src/data/types/value.cpp
    src/data/managers/variable_manager.cpp
    src/data/scope/scope_manager.cpp
    src/data/managers/function_manager.cpp
)

# البرمجة الكائنية - الأنواع / OOP - Types
set(OOP_TYPES_SOURCES
    src/data/types/class_type.cpp
    src/data/types/object_instance.cpp
)

# البرمجة الكائنية - المديران / OOP - Managers
set(OOP_MANAGERS_SOURCES
    src/data/managers/class_manager.cpp
    src/data/managers/object_manager.cpp
)

# البرمجة الكائنية - عُقد AST / OOP - AST Nodes
set(OOP_AST_SOURCES
    src/ast/oop/class_nodes.cpp
)

# البرمجة الكائنية - المحلل النحوي / OOP - Parser
# (AR) تم نقل دوال تحليل OOP إلى parser_core_oop.cpp
# (EN) OOP parsing functions moved to parser_core_oop.cpp
# set(OOP_PARSER_SOURCES
#     src/parser/oop/parser_classes.cpp
# )

# البرمجة الكائنية - المفسر / OOP - Interpreter
# (AR) معطل مؤقتاً - يحتاج تكامل مع InterpreterCore
# (EN) Temporarily disabled - needs integration with InterpreterCore
set(OOP_INTERPRETER_SOURCES
    # src/interpreter/oop/interpreter_classes.cpp
)

# المكتبات القياسية - الدوال المضمنة / Standard Libraries - Built-in Functions
set(BUILTINS_SOURCES
    src/stdlib/core/builtins.cpp
)

# إدارة الأخطاء / Error Management
# (AR) نظام إدارة الأخطاء الموحد - المرحلة 1 مكتملة
# (EN) Unified error management system - Phase 1 completed
set(ERROR_SOURCES
    src/errors/error_codes.cpp
    src/errors/diagnostic.cpp
    src/errors/error_manager.cpp
)

# المكتبات القياسية / Standard Libraries
# الحمد لله - تم تفعيل جميع المكتبات
set(STDLIB_SOURCES
    src/stdlib/core/stdlib_manager.cpp
    src/stdlib/core/type_functions.cpp
    src/stdlib/core/array_functions.cpp
    src/stdlib/core/other_functions.cpp
    src/stdlib/string/string_functions.cpp
    src/stdlib/math/math_functions.cpp
    src/stdlib/io/io_functions.cpp
    src/stdlib/filesystem/filesystem_module.cpp
    src/stdlib/http/http_module.cpp
    src/stdlib/database/database_module.cpp
    src/stdlib/json/json_module.cpp
    src/stdlib/xml/xml_module.cpp
    src/stdlib/crypto/crypto_module.cpp
)

# المحسن المتقدم / Advanced Optimizer (Phase 10)
set(OPTIMIZER_SOURCES
    src/optimizer/advanced_optimizer.cpp
    src/optimizer/optimization_pass.cpp
)

# البرمجة منخفضة المستوى / Low-Level Programming (Stage 1)
set(LOW_LEVEL_SOURCES
    src/low_level/pointer_type.cpp
    src/low_level/bitwise_ops.cpp
    src/low_level/memory_map.cpp
    src/low_level/io_ports.cpp
    src/low_level/interrupts.cpp
)

# المترجم - الواجهة الأمامية / Compiler - Frontend (Phase 1)
set(COMPILER_FRONTEND_SOURCES
    compiler/frontend/src/sir_types.cpp
    compiler/frontend/src/sir_module.cpp
)

# ======================================================================
# نظام الوحدات / Module System (Phase 2, 3 & 4)
# ======================================================================
set(MODULES_SOURCES
    src/modules/module.cpp
    src/modules/search_paths.cpp
    src/modules/module_cache.cpp
    src/modules/dependency_graph.cpp
    src/modules/module_loader.cpp
    src/modules/symbol_resolver.cpp
    src/modules/module_validator.cpp
)

# جميع ملفات المصدر / All source files
set(ALL_SOURCES
    ${UTILS_SOURCES}
    ${LEXER_SOURCES}
    ${PARSER_SOURCES}
    ${AST_SOURCES}
    ${INTERPRETER_SOURCES}
    ${DATA_SOURCES}
    ${OOP_TYPES_SOURCES}
    ${OOP_MANAGERS_SOURCES}
    ${OOP_AST_SOURCES}
    ${OOP_PARSER_SOURCES}
    ${OOP_INTERPRETER_SOURCES}
    ${ERROR_SOURCES}
    ${STDLIB_SOURCES}
    ${BUILTINS_SOURCES}
    ${OPTIMIZER_SOURCES}
    ${LOW_LEVEL_SOURCES}
    ${COMPILER_FRONTEND_SOURCES}
    ${MODULES_SOURCES}
)

# ======================================================================
# مكتبة لغة ص الأساسية / Sad Core Library
# ======================================================================
add_library(sad_core STATIC ${ALL_SOURCES})

# Find SQLite3 for Database Module
find_package(unofficial-sqlite3 CONFIG QUIET)
if(unofficial-sqlite3_FOUND)
    message(STATUS "✓ SQLite3 found - Database module enabled")
    target_link_libraries(sad_core PRIVATE unofficial::sqlite3::sqlite3)
    target_compile_definitions(sad_core PRIVATE HAS_SQLITE3)
else()
    find_package(SQLite3 QUIET)
    if(SQLite3_FOUND)
        message(STATUS "✓ SQLite3 found - Database module enabled")
        target_link_libraries(sad_core PRIVATE ${SQLite3_LIBRARIES})
        target_include_directories(sad_core PRIVATE ${SQLite3_INCLUDE_DIRS})
        target_compile_definitions(sad_core PRIVATE HAS_SQLITE3)
    else()
        message(STATUS "⚠ SQLite3 not found - Database module will have limited functionality")
    endif()
endif()

# Find OpenSSL for Crypto Module (optional)
find_package(OpenSSL QUIET)
if(OPENSSL_FOUND)
    message(STATUS "✓ OpenSSL found - Crypto module enabled")
    target_link_libraries(sad_core PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_include_directories(sad_core PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_compile_definitions(sad_core PRIVATE HAS_OPENSSL)
else()
    message(STATUS "⚠ OpenSSL not found - Crypto module will have limited functionality")
endif()

# تعيين خصائص المكتبة / Set library properties
set_target_properties(sad_core PROPERTIES
    OUTPUT_NAME "sad_core"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

# ======================================================================
# البرنامج التنفيذي الرئيسي / Main Executable
# ======================================================================
add_executable(sad src/main.cpp)

# ربط المكتبة الأساسية / Link core library
target_link_libraries(sad PRIVATE sad_core)

# تعيين خصائص البرنامج التنفيذي / Set executable properties
set_target_properties(sad PROPERTIES
    OUTPUT_NAME "sad"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# ======================================================================
# الاختبارات / Tests
# ======================================================================
if(BUILD_TESTS)
    enable_testing()
    
    # Find GTest package (optional)
    find_package(GTest CONFIG QUIET)
    
    # ملفات اختبارات المحلل النحوي / Parser test files
    # ملاحظة: نستخدم فقط test_runner.cpp لتجنب تضارب دوال main()
    # Note: Using only test_runner.cpp to avoid main() conflicts
    set(PARSER_TEST_SOURCES
        tests/parser_tests/test_runner.cpp
    )
    
    # Check if parser test files exist before adding them
    if(FALSE AND EXISTS "${CMAKE_SOURCE_DIR}/tests/parser_tests/test_lambda_simple.cpp")
        # برنامج تشغيل الاختبارات / Test runner executable (DISABLED - WIP)
        add_executable(parser_tests
            ${PARSER_TEST_SOURCES}
        )
        
        # ربط المكتبات المطلوبة / Link required libraries
        target_link_libraries(parser_tests PRIVATE sad_core)
        
        # إضافة مسار تضمين لملفات الاختبار / Add include path for test files
        target_include_directories(parser_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/tests/parser_tests
        )
        
        # تعيين خصائص البرنامج / Set executable properties
        set_target_properties(parser_tests PROPERTIES
            OUTPUT_NAME "parser_tests"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        # إضافة اختبارات CTest / Add CTest tests
        add_test(
            NAME ParserTests
            COMMAND parser_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات المحلل النحوي مفعلة / Parser tests enabled")
    else()
        message(STATUS "⚠ اختبارات المحلل النحوي غير متاحة / Parser tests not available")
    endif()
    
    # ======================================================================
    # اختبارات تحليل الوحدات (Module Parsing) / Module Parsing Tests
    # ======================================================================
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/parser/test_module_parsing.cpp")
        add_executable(test_module_parsing
            tests/parser/test_module_parsing.cpp
        )
        
        # Link required libraries
        target_link_libraries(test_module_parsing PRIVATE sad_core)
        
        # Add include directories
        target_include_directories(test_module_parsing PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/tests/parser
        )
        
        # Set executable properties
        set_target_properties(test_module_parsing PROPERTIES
            OUTPUT_NAME "test_module_parsing"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        # Add CTest test
        add_test(
            NAME ModuleParsingTests
            COMMAND test_module_parsing
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات تحليل الوحدات مفعلة / Module parsing tests enabled")
    else()
        message(STATUS "⚠ اختبارات تحليل الوحدات غير متاحة / Module parsing tests not available")
    endif()
    
    # ======================================================================
    # اختبارات محمل الوحدات (Module Loader) / Module Loader Tests - Phase 2
    # ======================================================================
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/modules/test_module_loader.cpp")
        add_executable(test_module_loader
            tests/modules/test_module_loader.cpp
        )
        
        # Link required libraries
        target_link_libraries(test_module_loader PRIVATE sad_core)
        
        # Add include directories
        target_include_directories(test_module_loader PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/tests/modules
        )
        
        # Set executable properties
        set_target_properties(test_module_loader PROPERTIES
            OUTPUT_NAME "test_module_loader"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        # Add CTest test
        add_test(
            NAME ModuleLoaderTests
            COMMAND test_module_loader
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات محمل الوحدات مفعلة / Module loader tests enabled")
    else()
        message(STATUS "⚠ اختبارات محمل الوحدات غير متاحة / Module loader tests not available")
    endif()
    
    # ======================================================================
    # اختبارات محلل الرموز (Symbol Resolver) / Symbol Resolver Tests - Phase 3
    # ======================================================================
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/modules/test_symbol_resolver.cpp")
        add_executable(test_symbol_resolver
            tests/modules/test_symbol_resolver.cpp
        )
        
        # Link required libraries
        target_link_libraries(test_symbol_resolver PRIVATE sad_core)
        
        # Add include directories
        target_include_directories(test_symbol_resolver PRIVATE
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/tests/modules
        )
        
        # Set executable properties
        set_target_properties(test_symbol_resolver PROPERTIES
            OUTPUT_NAME "test_symbol_resolver"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        # Add CTest test
        add_test(
            NAME SymbolResolverTests
            COMMAND test_symbol_resolver
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات محلل الرموز مفعلة / Symbol resolver tests enabled")
    else()
        message(STATUS "⚠ اختبارات محلل الرموز غير متاحة / Symbol resolver tests not available")
    endif()
    
    # ======================================================================
    # اختبارات محقق صحة الوحدات (Phase 4) / Module Validator Tests (Phase 4)
    # ======================================================================
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/modules/test_module_validator.cpp")
        add_executable(test_module_validator
            tests/modules/test_module_validator.cpp
        )
        
        target_link_libraries(test_module_validator PRIVATE sad_core)
        target_include_directories(test_module_validator PRIVATE
            ${CMAKE_SOURCE_DIR}/include
        )
        
        add_test(
            NAME ModuleValidatorTests
            COMMAND test_module_validator
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات محقق صحة الوحدات مفعلة / Module validator tests enabled")
    else()
        message(STATUS "⚠ اختبارات محقق صحة الوحدات غير متاحة / Module validator tests not available")
    endif()
    
    # ======================================================================
    # اختبارات التكامل الشاملة (Phase 5) / Integration Tests (Phase 5)
    # ======================================================================
    
    # End-to-End Integration Tests
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/modules/integration/test_end_to_end.cpp")
        add_executable(test_integration_e2e
            tests/modules/integration/test_end_to_end.cpp
        )
        
        target_link_libraries(test_integration_e2e
            PRIVATE
            GTest::gtest
            sad_core
        )
        
        target_include_directories(test_integration_e2e
            PRIVATE
            ${CMAKE_SOURCE_DIR}/include
        )
        
        add_test(
            NAME IntegrationEndToEndTests
            COMMAND test_integration_e2e
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات التكامل الشاملة مفعلة / End-to-end integration tests enabled")
    else()
        message(STATUS "⚠ اختبارات التكامل الشاملة غير متاحة / Integration tests not available")
    endif()
    
    # ======================================================================
    # اختبارات بناء SIR (Phase 1) / SIR Builder Tests (Phase 1)
    # ======================================================================
    
    # SIR Builder Tests
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/compiler/frontend/test_sir_builder.cpp")
        add_executable(sir_builder_tests
            tests/compiler/frontend/test_sir_builder.cpp
        )
        
        # Link required libraries
        target_link_libraries(sir_builder_tests PRIVATE sad_core)
        
        # Add include directories
        target_include_directories(sir_builder_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/compiler/frontend/include
            ${CMAKE_SOURCE_DIR}/parser/include
        )
        
        # Set executable properties
        set_target_properties(sir_builder_tests PROPERTIES
            OUTPUT_NAME "sir_builder_tests"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        # Add CTest test
        add_test(
            NAME SIRBuilderTests
            COMMAND sir_builder_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات بناء SIR مفعلة / SIR Builder tests enabled")
    else()
        message(STATUS "⚠ اختبارات بناء SIR غير متاحة / SIR Builder tests not available")
    endif()
    
    # SIR Integration Tests
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/compiler/frontend/test_sir_integration.cpp")
        add_executable(sir_integration_tests
            tests/compiler/frontend/test_sir_integration.cpp
        )
        
        # Link required libraries
        target_link_libraries(sir_integration_tests PRIVATE sad_core)
        
        # Add include directories
        target_include_directories(sir_integration_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/compiler/frontend/include
            ${CMAKE_SOURCE_DIR}/parser/include
        )
        
        # Set executable properties
        set_target_properties(sir_integration_tests PROPERTIES
            OUTPUT_NAME "sir_integration_tests"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        # Add CTest test
        add_test(
            NAME SIRIntegrationTests
            COMMAND sir_integration_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات تكامل SIR مفعلة / SIR Integration tests enabled")
    else()
        message(STATUS "⚠ اختبارات تكامل SIR غير متاحة / SIR Integration tests not available")
    endif()
    
    # SIR Edge Cases Tests
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/compiler/frontend/test_sir_edge_cases.cpp")
        add_executable(sir_edge_cases_tests
            tests/compiler/frontend/test_sir_edge_cases.cpp
        )
        
        # Link required libraries
        target_link_libraries(sir_edge_cases_tests PRIVATE sad_core)
        
        # Add include directories
        target_include_directories(sir_edge_cases_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/compiler/frontend/include
            ${CMAKE_SOURCE_DIR}/parser/include
        )
        
        # Set executable properties
        set_target_properties(sir_edge_cases_tests PROPERTIES
            OUTPUT_NAME "sir_edge_cases_tests"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        # Add CTest test
        add_test(
            NAME SIREdgeCasesTests
            COMMAND sir_edge_cases_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات الحالات الحدية SIR مفعلة / SIR Edge Cases tests enabled")
    else()
        message(STATUS "⚠ اختبارات الحالات الحدية SIR غير متاحة / SIR Edge Cases tests not available")
    endif()
    
    # ======================================================================
    # اختبارات دوال الإدخال والإخراج (Phase 1) / I/O Functions Tests (Phase 1)
    # ======================================================================
    
    # I/O Functions Test (Simple, no external dependency)
    add_executable(io_functions_tests
        tests/stdlib_tests/test_io_simple.cpp
    )
    
    # Link required libraries
    target_link_libraries(io_functions_tests PRIVATE sad_core)
    
    # Add include paths
    target_include_directories(io_functions_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )
    
    # Set executable properties
    set_target_properties(io_functions_tests PROPERTIES
        OUTPUT_NAME "io_functions_tests"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )
    
    # Add CTest test
    add_test(
        NAME IOFunctionsTests
        COMMAND io_functions_tests
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    
    # ======================================================================
    # اختبارات المحسن المتقدم (Phase 10) / Advanced Optimizer Tests (Phase 10)
    # ======================================================================
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/optimizer/advanced_optimizer_test.cpp")
        # Advanced Optimizer Tests - Main Test Suite
        add_executable(optimizer_tests
            tests/optimizer/advanced_optimizer_test.cpp
        )
        
        # Link required libraries
        target_link_libraries(optimizer_tests PRIVATE sad_core)
        
        # Add include paths
        target_include_directories(optimizer_tests PRIVATE
            ${CMAKE_SOURCE_DIR}
        )
        
        # Set executable properties
        set_target_properties(optimizer_tests PROPERTIES
            OUTPUT_NAME "optimizer_tests"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        # Add CTest test
        add_test(
            NAME OptimizerTests
            COMMAND optimizer_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات المحسن المتقدم مفعلة / Advanced Optimizer tests enabled")
    else()
        message(STATUS "⚠ اختبارات المحسن المتقدم غير متاحة / Advanced Optimizer tests not available")
    endif()
    
    # ======================================================================
    # اختبارات تفصيلية للمحسن (Phase 10) / Detailed Optimizer Tests (Phase 10)
    # ======================================================================
    
    # Simple Optimizer Tests (without GTest dependency)
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/optimizer/test_optimizer_simple.cpp")
        add_executable(optimizer_simple_tests
            tests/optimizer/test_optimizer_simple.cpp
        )
        target_link_libraries(optimizer_simple_tests PRIVATE sad_core)
        target_include_directories(optimizer_simple_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/include
        )
        set_target_properties(optimizer_simple_tests PROPERTIES
            OUTPUT_NAME "optimizer_simple_tests"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        add_test(NAME OptimizerSimpleTests COMMAND optimizer_simple_tests WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
        message(STATUS "✓ اختبارات المحسن البسيطة مفعلة / Simple Optimizer tests enabled")
    endif()
    
    # ======================================================================
    # اختبارات البرمجة الكائنية (Phase 0) / OOP Tests (Phase 0)
    # ======================================================================
    
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/oop_tests/class_declaration_tests.cpp")
        # OOP Tests executable
        add_executable(oop_tests
            tests/oop_tests/class_declaration_tests.cpp
        )
        
        # Link required libraries
        target_link_libraries(oop_tests PRIVATE sad_core)
        
        # Add include paths
        target_include_directories(oop_tests PRIVATE
            ${CMAKE_SOURCE_DIR}
        )
        
        # Set executable properties
        set_target_properties(oop_tests PROPERTIES
            OUTPUT_NAME "oop_tests"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        # Add CTest test
        add_test(
            NAME OOPTests
            COMMAND oop_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات البرمجة الكائنية / OOP tests enabled")
    else()
        message(STATUS "⚠ اختبارات البرمجة الكائنية غير متاحة / OOP tests not available")
    endif()
    
    # ======================================================================
    # اختبارات المترجم - الواجهة الأمامية (Phase 1) / Compiler Frontend Tests (Phase 1)
    # ======================================================================
    
    # SIR Types Tests
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/compiler/frontend/test_sir_types.cpp")
        add_executable(sir_types_tests
            tests/compiler/frontend/test_sir_types.cpp
            compiler/frontend/src/type_info.cpp
        )
        
        target_link_libraries(sir_types_tests PRIVATE sad_core)
        
        target_include_directories(sir_types_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/compiler/frontend/include
        )
        
        set_target_properties(sir_types_tests PROPERTIES
            OUTPUT_NAME "sir_types_tests"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        add_test(
            NAME SIRTypesTests
            COMMAND sir_types_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات أنواع SIR مفعلة / SIR Types tests enabled")
    else()
        message(STATUS "⚠ اختبارات أنواع SIR غير متاحة / SIR Types tests not available")
    endif()
    
    # SIR Instruction Tests
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/compiler/frontend/test_sir_instruction.cpp")
        add_executable(sir_instruction_tests
            tests/compiler/frontend/test_sir_instruction.cpp
        )
        
        target_link_libraries(sir_instruction_tests PRIVATE sad_core)
        
        target_include_directories(sir_instruction_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/compiler/frontend/include
        )
        
        set_target_properties(sir_instruction_tests PROPERTIES
            OUTPUT_NAME "sir_instruction_tests"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        add_test(
            NAME SIRInstructionTests
            COMMAND sir_instruction_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات تعليمات SIR مفعلة / SIR Instruction tests enabled")
    else()
        message(STATUS "⚠ اختبارات تعليمات SIR غير متاحة / SIR Instruction tests not available")
    endif()
    
    # SIR Module Tests - DISABLED (WIP)
    message(STATUS "⊘ اختبارات وحدات SIR معطلة مؤقتاً / SIR Module tests disabled (WIP)")
    
    # ======================================================================
    # اختبارات محسّن SIR (Phase 2) / SIR Optimizer Tests (Phase 2)
    # ======================================================================
    
    # Check if optimizer test directory and CMakeLists exist
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/compiler/optimizer/CMakeLists.txt")
        # Add optimizer tests subdirectory
        add_subdirectory(tests/compiler/optimizer)
        message(STATUS "✓ اختبارات محسّن SIR مفعلة / SIR Optimizer tests enabled")
    else()
        message(STATUS "⚠ اختبارات محسّن SIR غير متاحة / SIR Optimizer tests not available")
    endif()
    
    # ======================================================================
    # اختبارات المستوى المنخفض (Stage 1) / Low-Level Tests (Stage 1)
    # ======================================================================
    
    # Pointer System Tests - DISABLED (WIP)
    message(STATUS "⊘ اختبارات نظام المؤشرات معطلة مؤقتاً / Pointer system tests disabled (WIP)")
    
    # Bitwise Operations Tests
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/low_level/test_bitwise.cpp")
        add_executable(bitwise_tests
            tests/low_level/test_bitwise.cpp
        )
        
        target_link_libraries(bitwise_tests PRIVATE sad_core)
        
        target_include_directories(bitwise_tests PRIVATE
            ${CMAKE_SOURCE_DIR}/include
        )
        
        set_target_properties(bitwise_tests PROPERTIES
            OUTPUT_NAME "bitwise_tests"
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        )
        
        add_test(
            NAME BitwiseTests
            COMMAND bitwise_tests
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        )
        
        message(STATUS "✓ اختبارات العمليات الثنائية مفعلة / Bitwise operations tests enabled")
    else()
        message(STATUS "⚠ اختبارات العمليات الثنائية غير متاحة / Bitwise operations tests not available")
    endif()
    
    message(STATUS "✓ الاختبارات مفعلة / Tests enabled")
    message(STATUS "  - اختبارات دوال الإدخال/الإخراج / I/O Functions tests: tests/stdlib_tests/test_io_simple.cpp")
endif()

# ======================================================================
# الأمثلة / Examples
# ======================================================================
if(BUILD_EXAMPLES)
    # سيتم إضافة الأمثلة لاحقاً / Examples will be added later
    # add_subdirectory(examples)
    
    message(STATUS "✓ الأمثلة مفعلة / Examples enabled")
endif()

# ======================================================================
# أدوات التطوير / Development Tools
# ======================================================================

# خادم بروتوكول اللغة / Language Server Protocol (LSP)
option(BUILD_LSP_SERVER "بناء خادم LSP / Build LSP server" ON)

if(BUILD_LSP_SERVER)
    message(STATUS "==============================================")
    message(STATUS "بناء خادم LSP / Building LSP Server")
    message(STATUS "==============================================")
    
    # Find nlohmann/json library - مكتبة JSON
    find_package(nlohmann_json QUIET)
    
    if(NOT nlohmann_json_FOUND)
        message(STATUS "⚠ nlohmann/json not found, downloading...")
        include(FetchContent)
        FetchContent_Declare(
            json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.11.3
            GIT_SHALLOW TRUE
        )
        set(JSON_BuildTests OFF CACHE INTERNAL "")
        set(JSON_Install OFF CACHE INTERNAL "")
        FetchContent_MakeAvailable(json)
    endif()
    
    # Fetch CURL library for Firebase module loading (optional)
    # تحميل مكتبة CURL لتحميل الوحدات من Firebase (اختياري)
    option(ENABLE_FIREBASE_MODULES "Enable Firebase Module Loading in LSP" ON)
    
    if(ENABLE_FIREBASE_MODULES)
        find_package(CURL QUIET)
        if(NOT CURL_FOUND)
            message(STATUS "CURL not found, attempting to fetch...")
            # Will use system CURL if available
            set(CURL_LIBRARY "curl")
            set(CURL_INCLUDE_DIR "")
        endif()
    endif()
    
    # LSP Server executable - برنامج خادم LSP
    add_executable(sad-lsp
        tools/lsp/main.cpp
    )
    
    # تضمين الملفات / Include directories
    target_include_directories(sad-lsp PRIVATE
        ${CMAKE_SOURCE_DIR}/tools/lsp
        ${CMAKE_SOURCE_DIR}/include
    )
    
    # ربط المكتبات / Link libraries
    target_link_libraries(sad-lsp PRIVATE
        nlohmann_json::nlohmann_json
        sad_core
    )
    
    # Link CURL if Firebase modules enabled
    if(ENABLE_FIREBASE_MODULES AND CURL_FOUND)
        target_include_directories(sad-lsp PRIVATE ${CURL_INCLUDE_DIRS})
        target_link_libraries(sad-lsp PRIVATE ${CURL_LIBRARIES})
        target_compile_definitions(sad-lsp PRIVATE ENABLE_FIREBASE_MODULES=1)
    endif()
    
    # تعيين خصائص البناء / Set build properties
    set_target_properties(sad-lsp PROPERTIES
        OUTPUT_NAME "sad-lsp"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    )
    
    # تثبيت / Installation
    install(TARGETS sad-lsp
        RUNTIME DESTINATION bin
        COMPONENT tools
    )
    
    message(STATUS "✓ خادم LSP مفعّل / LSP Server enabled")
    message(STATUS "  - الملف التنفيذي / Executable: ${CMAKE_BINARY_DIR}/bin/sad-lsp")
    if(ENABLE_FIREBASE_MODULES)
        message(STATUS "  - Firebase Module Loading: ${CURL_FOUND}")
    endif()
    message(STATUS "  - VS Code Extension: tools/vscode-extension/")
    message(STATUS "==============================================")
else()
    message(STATUS "⊘ خادم LSP معطّل / LSP Server disabled")
endif()

# ======================================================================
# [Phase 8] Package Manager - مدير الحزم
# ======================================================================
if(BUILD_PKG_MANAGER)
    message(STATUS "==============================================")
    message(STATUS "[Phase 8] بناء مدير الحزم / Building Package Manager")
    message(STATUS "==============================================")
    
    # Find libcurl - البحث عن libcurl (optional)
    find_package(CURL QUIET)
    
    if(CURL_FOUND)
        message(STATUS "✓ CURL found - Package manager enabled")
        
        # بناء sad-pkg / Build sad-pkg executable
        add_executable(sad-pkg
            tools/pkg/cli.cpp
        )
        
        # تضمين الملفات / Include directories
        target_include_directories(sad-pkg PRIVATE
            ${CMAKE_SOURCE_DIR}/tools/pkg
            ${CMAKE_SOURCE_DIR}/include
            ${CURL_INCLUDE_DIRS}
        )
        
        # ربط المكتبات / Link libraries
        target_link_libraries(sad-pkg PRIVATE
            ${CURL_LIBRARIES}
            sad_core
        )
        
        # تعيين خصائص البناء / Set build properties
        set_target_properties(sad-pkg PROPERTIES
            OUTPUT_NAME "sad-pkg"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
        
        # تثبيت / Installation
        install(TARGETS sad-pkg
            RUNTIME DESTINATION bin
            COMPONENT tools
        )
        
        # Install package manager headers - تثبيت ملفات رأس مدير الحزم
        install(FILES
            tools/pkg/package.h
            tools/pkg/toml_parser.h
            tools/pkg/dependency_resolver.h
            tools/pkg/registry_client.h
            DESTINATION include/sad/pkg
            COMPONENT development
        )
        
        message(STATUS "✓ مدير الحزم مفعّل / Package Manager enabled")
        message(STATUS "  - الملف التنفيذي / Executable: ${CMAKE_BINARY_DIR}/bin/sad-pkg")
        message(STATUS "  - أوامر / Commands: init, install, add, remove, update, publish")
        message(STATUS "  - ملفات الرأس / Headers: tools/pkg/*.h")
        message(STATUS "==============================================")
    else()
        message(STATUS "⊘ مدير الحزم معطّل (CURL غير متاح) / Package Manager disabled (CURL not found)")
    endif()

# ======================================================================
# اختبارات وحدة نظام الملفات / Filesystem Module Tests
# ======================================================================
if(BUILD_TESTS)
    message(STATUS "==============================================")
    message(STATUS "✓ اختبارات نظام الملفات / Filesystem Tests")
    message(STATUS "==============================================")
    
    add_executable(filesystem_tests
        tests/stdlib/test_filesystem_module.cpp
    )
    
    target_include_directories(filesystem_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
    )
    
    target_link_libraries(filesystem_tests PRIVATE
        sad_core
        GTest::gtest
        GTest::gtest_main
    )
    
    set_target_properties(filesystem_tests PROPERTIES
        OUTPUT_NAME "filesystem_tests"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
    
    message(STATUS "  - الملف التنفيذي / Executable: ${CMAKE_BINARY_DIR}/tests/filesystem_tests")
    message(STATUS "  - عدد الاختبارات / Test Count: ~70 tests")
    message(STATUS "  - التغطية / Coverage: File I/O, Directory Ops, Paths, Permissions")
    message(STATUS "==============================================")
endif()

# ======================================================================
# اختبارات وحدة HTTP / HTTP Module Tests
# ======================================================================
if(BUILD_TESTS)
    message(STATUS "==============================================")
    message(STATUS "✓ اختبارات HTTP / HTTP Module Tests")
    message(STATUS "==============================================")
    
    add_executable(http_tests
        tests/stdlib/test_http_module.cpp
    )
    
    target_include_directories(http_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
    )
    
    target_link_libraries(http_tests PRIVATE
        sad_core
        GTest::gtest
        GTest::gtest_main
    )
    
    # Link platform-specific socket libraries
    if(WIN32)
        target_link_libraries(http_tests PRIVATE ws2_32)
    endif()
    
    set_target_properties(http_tests PROPERTIES
        OUTPUT_NAME "http_tests"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
    
    message(STATUS "  - الملف التنفيذي / Executable: ${CMAKE_BINARY_DIR}/tests/http_tests")
    message(STATUS "  - عدد الاختبارات / Test Count: ~50 tests")
    message(STATUS "  - التغطية / Coverage: HTTP Client, Server, URL Utils, Headers, Cookies")
    message(STATUS "==============================================")
endif()

# ======================================================================
# اختبارات وحدة قاعدة البيانات / Database Module Tests
# ======================================================================
if(BUILD_TESTS)
    message(STATUS "==============================================")
    message(STATUS "⊘ اختبارات قاعدة البيانات معطّلة / Database Module Tests Disabled")
    message(STATUS "==============================================")
    
    # Find SQLite3 library
    # find_package(SQLite3 REQUIRED)
    
    # add_executable(database_tests
    #     tests/stdlib/test_database_module.cpp
    # )
    
    # target_include_directories(database_tests PRIVATE
    #     ${CMAKE_SOURCE_DIR}/include
    #     ${CMAKE_SOURCE_DIR}/src
    #     ${SQLite3_INCLUDE_DIRS}
    # )
    
    # target_link_libraries(database_tests PRIVATE
    #     sad_core
    #     gtest
    #     gtest_main
    #     ${SQLite3_LIBRARIES}
    # )
    
    # set_target_properties(database_tests PROPERTIES
    #     OUTPUT_NAME "database_tests"
    #     RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    # )
    
    # message(STATUS "  - الملف التنفيذي / Executable: ${CMAKE_BINARY_DIR}/tests/database_tests")
    # message(STATUS "  - عدد الاختبارات / Test Count: ~56 tests")
    # message(STATUS "  - التغطية / Coverage: Connection, Queries, Transactions, Schema, QueryBuilder")
    # message(STATUS "==============================================")
endif()

# ======================================================================
# اختبارات وحدة JSON/XML / JSON/XML Module Tests
# ======================================================================
if(BUILD_TESTS)
    message(STATUS "==============================================")
    message(STATUS "✓ اختبارات JSON/XML / JSON/XML Module Tests")
    message(STATUS "==============================================")
    
    add_executable(json_xml_tests
        tests/stdlib/test_json_module.cpp
        tests/stdlib/test_xml_module.cpp
    )
    
    target_include_directories(json_xml_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
    )
    
    target_link_libraries(json_xml_tests PRIVATE
        sad_core
        GTest::gtest
        GTest::gtest_main
    )
    
    set_target_properties(json_xml_tests PROPERTIES
        OUTPUT_NAME "json_xml_tests"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
    
    message(STATUS "  - الملف التنفيذي / Executable: ${CMAKE_BINARY_DIR}/tests/json_xml_tests")
    message(STATUS "  - عدد الاختبارات / Test Count: ~105 tests (60 JSON + 45 XML)")
    message(STATUS "  - التغطية / Coverage: Parse, Generate, Query, Validate, Transform")
    message(STATUS "==============================================")
endif()

# ======================================================================
# اختبارات وحدة التشفير / Cryptography Module Tests
# ======================================================================
if(BUILD_TESTS AND OPENSSL_FOUND)
    message(STATUS "==============================================")
    message(STATUS "✓ اختبارات التشفير / Crypto Module Tests")
    message(STATUS "==============================================")
    
    add_executable(crypto_tests
        tests/stdlib/test_crypto_module.cpp
    )
    
    target_include_directories(crypto_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/src
        ${OPENSSL_INCLUDE_DIR}
    )
    
    target_link_libraries(crypto_tests PRIVATE
        sad_core
        GTest::gtest
        GTest::gtest_main
        OpenSSL::SSL
        OpenSSL::Crypto
    )
    
    set_target_properties(crypto_tests PROPERTIES
        OUTPUT_NAME "crypto_tests"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests"
    )
    
    message(STATUS "  - الملف التنفيذي / Executable: ${CMAKE_BINARY_DIR}/tests/crypto_tests")
    message(STATUS "  - عدد الاختبارات / Test Count: ~57 tests")
    message(STATUS "  - التغطية / Coverage: Hash, HMAC, AES, RSA, Password, Random, Encoding")
    message(STATUS "==============================================")
endif()

# ======================================================================
# رسائل التأكيد / Confirmation Messages
# ======================================================================
message(STATUS "==============================================")
message(STATUS "لغة ص البرمجية / Sad Programming Language")
message(STATUS "==============================================")
message(STATUS "الإصدار / Version: ${PROJECT_VERSION}")
message(STATUS "معيار C++ / C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "مجلد البناء / Build Directory: ${CMAKE_BINARY_DIR}")
message(STATUS "نوع البناء / Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "المترجم / Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "سجلات التنقيح / Debug Logging: ${ENABLE_DEBUG_LOGGING}")
message(STATUS "==============================================")

# Package Manager block ends here
endif()

# ======================================================================
# ملاحظات إضافية / Additional Notes
# ======================================================================
# Note for developers:
# - All source files should be UTF-8 encoded with BOM for Arabic support
# - Use the debugPrint() function for logging instead of std::cout
# - Follow the coding standards documented in docs/architecture/
# - Run tests before committing: cmake --build . --target test
# ======================================================================
