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
    LANGUAGES C CXX
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
    # النواة / Core
    include/core/types.h
    include/core/math3d.h
    include/core/event_bus.h
    include/core/ecs.h
    include/core/state_machine.h
    include/core/serialization.h
    include/core/profiler.h
    include/core/tween.h
    include/core/timer.h
    include/core/scene.h

    # النافذة والإدخال / Window & Input
    include/window/window.h
    include/input/input_manager.h

    # التصيير / Rendering
    include/rendering/context.h
    include/rendering/shader.h
    include/rendering/renderer2d.h
    include/rendering/renderer3d.h
    include/rendering/framebuffer.h
    include/rendering/camera2d.h
    include/rendering/camera3d.h
    include/rendering/sprite.h
    include/rendering/mesh.h
    include/rendering/material.h
    include/rendering/lighting.h

    # النصوص / Textures & Text
    include/texture/texture.h
    include/texture/image.h
    include/texture/texture_atlas.h
    include/text/font.h
    include/text/arabic_text.h

    # التأثيرات / Effects
    include/effects/particle_system.h
    include/effects/post_processing.h

    # العالم / World
    include/world/tilemap.h

    # الفيزياء / Physics
    include/physics/physics2d.h

    # الصوت / Audio
    include/audio/audio_system.h

    # الموارد / Resources
    include/resources/resource_manager.h

    # الرسم / Drawing
    include/drawing/shapes.h

    # نظام التشغيل / OS
    include/os/os_framebuffer.h

    # واجهة المستخدم لسطح المكتب / Desktop UI
    include/ui/widget.h
    include/ui/label.h
    include/ui/button.h
    include/ui/textinput.h
    include/ui/checkbox.h
    include/ui/panel.h
    include/ui/slider.h
    include/ui/ui_layout.h

    # واجهة المستخدم للهاتف / Mobile UI
    include/ui/mobile/mobile_core.h
    include/ui/mobile/mobile_buttons.h
    include/ui/mobile/mobile_input.h
    include/ui/mobile/mobile_navigation.h
    include/ui/mobile/mobile_lists.h
    include/ui/mobile/mobile_dialogs.h
    include/ui/mobile/mobile_display.h
    include/ui/mobile/mobile_media.h
    include/ui/mobile/mobile_advanced.h
)

# ملفات المصدر / Source files
set(SAD_GRAPHICS_SOURCES
    # النواة / Core
    src/core/types.cpp
    src/core/math.cpp
    src/core/tween.cpp
    src/core/timer.cpp
    src/core/scene.cpp

    # النافذة والإدخال / Window & Input
    src/window/window.cpp
    src/input/input_manager.cpp

    # التصيير / Rendering
    src/rendering/context.cpp
    src/rendering/shader.cpp
    src/rendering/renderer2d.cpp
    src/rendering/renderer3d.cpp
    src/rendering/framebuffer.cpp
    src/rendering/camera2d.cpp
    src/rendering/camera3d.cpp
    src/rendering/sprite.cpp
    src/rendering/mesh.cpp
    src/rendering/material.cpp
    src/rendering/lighting.cpp

    # النصوص / Textures & Text
    src/texture/texture.cpp
    src/texture/image.cpp
    src/texture/texture_atlas.cpp
    src/text/font.cpp
    src/text/arabic_text.cpp

    # التأثيرات / Effects
    src/effects/particle_system.cpp
    src/effects/post_processing.cpp

    # العالم / World
    src/world/tilemap.cpp

    # الفيزياء / Physics
    src/physics/physics2d.cpp

    # الصوت / Audio
    src/audio/audio_system.cpp

    # الموارد / Resources
    src/resources/resource_manager.cpp

    # الرسم / Drawing
    src/drawing/shapes.cpp

    # واجهة المستخدم لسطح المكتب / Desktop UI
    src/ui/widget.cpp
    src/ui/label.cpp
    src/ui/button.cpp
    src/ui/textinput.cpp
    src/ui/checkbox.cpp
    src/ui/panel.cpp
    src/ui/slider.cpp
    src/ui/ui_layout.cpp

    # واجهة المستخدم للهاتف / Mobile UI
    src/ui/mobile/mobile_core.cpp
    src/ui/mobile/mobile_buttons.cpp
    src/ui/mobile/mobile_input.cpp
    src/ui/mobile/mobile_navigation.cpp
    src/ui/mobile/mobile_lists.cpp
    src/ui/mobile/mobile_dialogs.cpp
    src/ui/mobile/mobile_display.cpp
    src/ui/mobile/mobile_media.cpp

    # محمل GLAD / GLAD loader
    ${GLAD_SRC}
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
        $<BUILD_INTERFACE:${SDL2_INCLUDE_DIRS}>
        $<BUILD_INTERFACE:${GLAD_INCLUDE_DIR}>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
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
