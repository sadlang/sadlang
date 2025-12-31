# ==============================================================================
# CMakeLists.txt - مكتبة الرسومات للغة ص
# Graphics Library for Sad Language
# ==============================================================================
# الوصف: ملف بناء CMake لمكتبة الرسومات
# Description: CMake build file for graphics library
# ==============================================================================

cmake_minimum_required(VERSION 3.15)

# ==============================================================================
# معلومات المشروع / Project Information
# ==============================================================================
project(SadGraphics
    VERSION 0.1.0
    DESCRIPTION "مكتبة الرسومات للغة ص / Graphics Library for Sad Language"
    LANGUAGES CXX
)

# ==============================================================================
# إعدادات C++ / C++ Settings
# ==============================================================================
set(CMAKE_CXX_STANDARD 17)                      # استخدام معيار C++17 / Use C++17 standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)             # المعيار مطلوب / Standard required
set(CMAKE_CXX_EXTENSIONS OFF)                   # بدون امتدادات / No extensions

# ==============================================================================
# خيارات البناء / Build Options
# ==============================================================================
option(SAD_GRAPHICS_BUILD_TESTS "بناء الاختبارات / Build tests" ON)
option(SAD_GRAPHICS_BUILD_EXAMPLES "بناء الأمثلة / Build examples" ON)
option(SAD_GRAPHICS_SHARED "بناء مكتبة مشتركة / Build shared library" ON)

# ==============================================================================
# البحث عن المكتبات المطلوبة / Find Required Libraries
# ==============================================================================

# SDL2 - مكتبة النوافذ والأحداث / Window and event library
set(SDL2_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/SDL2/SDL2-2.28.5)
set(SDL2_INCLUDE_DIRS ${SDL2_DIR}/include)
set(SDL2_LIBRARIES ${SDL2_DIR}/lib/x64/SDL2.lib ${SDL2_DIR}/lib/x64/SDL2main.lib)
message(STATUS "استخدام SDL2 من / Using SDL2 from: ${SDL2_DIR}")

# OpenGL - مكتبة الرسومات / Graphics library
find_package(OpenGL REQUIRED)
if(OPENGL_FOUND)
    message(STATUS "تم العثور على OpenGL / OpenGL found")
else()
    message(FATAL_ERROR "OpenGL مطلوب لكن لم يُعثر عليه / OpenGL required but not found")
endif()

# GLAD - محمل دوال OpenGL / OpenGL function loader
set(GLAD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include)
set(GLAD_SRC ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/glad.c)
message(STATUS "استخدام GLAD من / Using GLAD from: ${GLAD_INCLUDE_DIR}")

# ==============================================================================
# الملفات المصدرية / Source Files
# ==============================================================================

# ملفات الرأس / Header files
set(SAD_GRAPHICS_HEADERS
    include/core/types.h
    include/window/window.h
    include/rendering/context.h
    include/rendering/shader.h
    include/rendering/renderer2d.h
    include/input/input_manager.h
    include/texture/texture.h
    include/texture/image.h
    include/text/font.h
    include/resources/resource_manager.h
    include/ui/widget.h
    include/ui/label.h
    include/ui/button.h
    include/ui/textinput.h
    include/ui/checkbox.h
    include/ui/panel.h
    include/ui/slider.h
)

# ملفات المصدر / Source files
set(SAD_GRAPHICS_SOURCES
    src/core/types.cpp
    src/window/window.cpp
    src/rendering/context.cpp
    src/rendering/shader.cpp
    src/rendering/renderer2d.cpp
    src/input/input_manager.cpp
    src/texture/texture.cpp
    src/texture/image.cpp
    src/text/font.cpp
    src/resources/resource_manager.cpp
    src/ui/widget.cpp
    src/ui/label.cpp
    src/ui/button.cpp
    src/ui/textinput.cpp
    src/ui/checkbox.cpp
    src/ui/panel.cpp
    src/ui/slider.cpp
    ${GLAD_SRC}  # محمل GLAD / GLAD loader
)

# ==============================================================================
# إنشاء المكتبة / Create Library
# ==============================================================================

if(SAD_GRAPHICS_SHARED)
    # مكتبة مشتركة / Shared library
    add_library(sad_graphics SHARED
        ${SAD_GRAPHICS_HEADERS}
        ${SAD_GRAPHICS_SOURCES}
    )
    message(STATUS "بناء مكتبة مشتركة / Building shared library")
else()
    # مكتبة ساكنة / Static library
    add_library(sad_graphics STATIC
        ${SAD_GRAPHICS_HEADERS}
        ${SAD_GRAPHICS_SOURCES}
    )
    message(STATUS "بناء مكتبة ساكنة / Building static library")
endif()

# تسمية بديلة / Alias target
add_library(Sad::Graphics ALIAS sad_graphics)

# ==============================================================================
# مسارات التضمين / Include Directories
# ==============================================================================

target_include_directories(sad_graphics
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
        ${GLAD_INCLUDE_DIR}  # مسارات GLAD / GLAD paths
        ${SDL2_INCLUDE_DIRS}  # مسارات SDL2 / SDL2 paths
)

# ==============================================================================
# ربط المكتبات / Link Libraries
# ==============================================================================

target_link_libraries(sad_graphics
    PUBLIC
        ${SDL2_LIBRARIES}           # مكتبة SDL2 / SDL2 library
        ${OPENGL_LIBRARIES}         # مكتبات OpenGL / OpenGL libraries
)

# ==============================================================================
# خصائص الترجمة / Compile Properties
# ==============================================================================

# تعريفات الترجمة / Compile definitions
target_compile_definitions(sad_graphics
    PRIVATE
        $<$<CONFIG:Debug>:SAD_GRAPHICS_DEBUG>      # تعريف DEBUG في وضع التنقيح / DEBUG definition in debug mode
        $<$<CONFIG:Release>:SAD_GRAPHICS_RELEASE>  # تعريف RELEASE في وضع الإصدار / RELEASE definition in release mode
)

# خيارات الترجمة / Compile options
if(MSVC)
    # خيارات Visual Studio / Visual Studio options
    target_compile_options(sad_graphics PRIVATE
        /W4                         # مستوى التحذيرات 4 / Warning level 4
        /WX-                        # لا تجعل التحذيرات أخطاء / Don't treat warnings as errors
        /MP                         # ترجمة متوازية / Parallel compilation
        /utf-8                      # ترميز UTF-8 / UTF-8 encoding
    )
else()
    # خيارات GCC/Clang / GCC/Clang options
    target_compile_options(sad_graphics PRIVATE
        -Wall                       # جميع التحذيرات / All warnings
        -Wextra                     # تحذيرات إضافية / Extra warnings
        -Wpedantic                  # التزام بالمعيار / Standard compliance
        -finput-charset=UTF-8       # ترميز الإدخال UTF-8 / Input encoding UTF-8
        -fexec-charset=UTF-8        # ترميز التنفيذ UTF-8 / Execution encoding UTF-8
    )
endif()

# ==============================================================================
# التثبيت / Installation
# ==============================================================================

# تثبيت المكتبة / Install library
install(TARGETS sad_graphics
    EXPORT SadGraphicsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

# تثبيت ملفات الرأس / Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# تثبيت ملف التكوين / Install config file
install(EXPORT SadGraphicsTargets
    FILE SadGraphicsTargets.cmake
    NAMESPACE Sad::
    DESTINATION lib/cmake/SadGraphics
)

# ==============================================================================
# الاختبارات / Tests
# ==============================================================================

if(SAD_GRAPHICS_BUILD_TESTS)
    enable_testing()                # تفعيل الاختبارات / Enable testing
    add_subdirectory(tests)         # إضافة مجلد الاختبارات / Add tests directory
    message(STATUS "الاختبارات مفعّلة / Tests enabled")
endif()

# ==============================================================================
# الأمثلة / Examples
# ==============================================================================

if(SAD_GRAPHICS_BUILD_EXAMPLES)
    add_subdirectory(examples)      # إضافة مجلد الأمثلة / Add examples directory
    message(STATUS "الأمثلة مفعّلة / Examples enabled")
endif()

# ==============================================================================
# معلومات البناء / Build Information
# ==============================================================================

message(STATUS "═══════════════════════════════════════════════════════")
message(STATUS "مكتبة الرسومات للغة ص / Sad Graphics Library")
message(STATUS "الإصدار / Version: ${PROJECT_VERSION}")
message(STATUS "مترجم C++ / C++ Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "معيار C++ / C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "نوع البناء / Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "═══════════════════════════════════════════════════════")
