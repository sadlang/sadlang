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
set(STDLIB_SOURCES
    src/stdlib/core/stdlib_manager.cpp
    src/stdlib/io/io_functions.cpp
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
)

# ======================================================================
# مكتبة لغة ص الأساسية / Sad Core Library
# ======================================================================
add_library(sad_core STATIC ${ALL_SOURCES})

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
    
    # ملفات اختبارات المحلل النحوي / Parser test files
    # ملاحظة: نستخدم فقط test_runner.cpp لتجنب تضارب دوال main()
    # Note: Using only test_runner.cpp to avoid main() conflicts
    set(PARSER_TEST_SOURCES
        tests/parser_tests/test_runner.cpp
    )
    
    # Check if parser test files exist before adding them
    if(EXISTS "${CMAKE_SOURCE_DIR}/tests/parser_tests/test_lambda_simple.cpp")
        # برنامج تشغيل الاختبارات / Test runner executable
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
        ${CMAKE_SOURCE_DIR}
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

# ======================================================================
# ملاحظات إضافية / Additional Notes
# ======================================================================
# Note for developers:
# - All source files should be UTF-8 encoded with BOM for Arabic support
# - Use the debugPrint() function for logging instead of std::cout
# - Follow the coding standards documented in docs/architecture/
# - Run tests before committing: cmake --build . --target test
# ======================================================================
