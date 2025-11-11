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
option(BUILD_TESTS "بناء الاختبارات / Build tests" ON)
option(BUILD_EXAMPLES "بناء الأمثلة / Build examples" ON)
option(ENABLE_DEBUG_LOGGING "تفعيل سجلات التنقيح / Enable debug logging" ON)

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
)

# عُقد الشجرة النحوية التجريدية / AST Nodes
set(AST_SOURCES
    src/parser/ast/ast_node.cpp
    src/parser/ast/expressions.cpp
    src/parser/ast/statements.cpp
    src/parser/ast/declarations.cpp
    src/parser/ast/ast_printer.cpp
)

# المفسر / Interpreter
set(INTERPRETER_SOURCES
    src/interpreter/core/interpreter_core.cpp
)

# إدارة البيانات / Data Management
set(DATA_SOURCES
    src/data/types/value.cpp
    src/data/managers/variable_manager.cpp
)

# إدارة الأخطاء / Error Management
set(ERROR_SOURCES
    src/errors/error_manager.cpp
)

# المكتبات القياسية / Standard Libraries
set(STDLIB_SOURCES
    src/stdlib/core/stdlib_manager.cpp
)

# جميع ملفات المصدر / All source files
set(ALL_SOURCES
    ${UTILS_SOURCES}
    ${LEXER_SOURCES}
    ${PARSER_SOURCES}
    ${AST_SOURCES}
    ${INTERPRETER_SOURCES}
    ${DATA_SOURCES}
    ${ERROR_SOURCES}
    ${STDLIB_SOURCES}
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
    
    # إضافة مجلد الاختبارات / Add tests subdirectory
    # سيتم إضافة الاختبارات لاحقاً / Tests will be added later
    # add_subdirectory(tests)
    
    message(STATUS "✓ الاختبارات مفعلة / Tests enabled")
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
