# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: CMakeLists.txt
# الوصف: بناء أداة تنسيق كود لغة ص (sad-fmt)
# Build configuration for Sad Language Code Formatter
# ═══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)

message(STATUS "════════════════════════════════════════════════════════")
message(STATUS "   إعداد أداة التنسيق - Configuring Code Formatter")
message(STATUS "════════════════════════════════════════════════════════")

# ═══════════════════════════════════════════════════════════════════════════════
# الملفات المصدرية - Source Files
# ═══════════════════════════════════════════════════════════════════════════════

set(FORMATTER_SOURCES
    fmt_main.cpp
    ${CMAKE_SOURCE_DIR}/compiler_new/src/format/sad_formatter.cpp
    ${CMAKE_SOURCE_DIR}/compiler_new/src/format/sad_formatter_rebuild.cpp
)

# ═══════════════════════════════════════════════════════════════════════════════
# إنشاء الملف التنفيذي - Create Executable
# ═══════════════════════════════════════════════════════════════════════════════

add_executable(sad-fmt ${FORMATTER_SOURCES})

# تعيين معيار C++17 / Set C++17 standard
target_compile_features(sad-fmt PUBLIC cxx_std_17)

# مسارات التضمين / Include directories
target_include_directories(sad-fmt PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include
    ${CMAKE_SOURCE_DIR}/tools/compiler/src
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
)

# تعطيل تحذيرات Unicode على Windows / Disable Unicode warnings on Windows
if(MSVC)
    target_compile_options(sad-fmt PRIVATE /wd4819 /FS /utf-8)
endif()

# مسار الإخراج / Output directory
set_target_properties(sad-fmt PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    OUTPUT_NAME "sad-fmt"
)

message(STATUS "✓ أداة التنسيق تم تكوينها بنجاح / Formatter configured")
message(STATUS "  - الاسم / Name: sad-fmt")
message(STATUS "  - المسار / Path: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/sad-fmt.exe")
