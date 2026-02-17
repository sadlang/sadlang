# ==============================================================================
# CMakeLists.txt للاختبارات / Tests CMakeLists.txt
# ==============================================================================

cmake_minimum_required(VERSION 3.15)

# ==============================================================================
# Catch2 - إطار الاختبار / Test Framework
# ==============================================================================
set(CATCH2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/third_party/catch2)

# ==============================================================================
# ملفات الاختبار / Test Files
# ==============================================================================

# اختبار الأنواع الأساسية / Core types tests (no GPU needed)
add_executable(test_types
    test_types.cpp
    test_main.cpp
)
target_include_directories(test_types PRIVATE ${CATCH2_INCLUDE_DIR})
target_link_libraries(test_types PRIVATE sad_graphics)
if(MSVC)
    target_compile_options(test_types PRIVATE /utf-8)
endif()
add_test(NAME "CoreTypes" COMMAND test_types)

# اختبار نظام الألوان / Color system tests (no GPU needed)
add_executable(test_color
    test_color.cpp
    test_main.cpp
)
target_include_directories(test_color PRIVATE ${CATCH2_INCLUDE_DIR})
target_link_libraries(test_color PRIVATE sad_graphics)
if(MSVC)
    target_compile_options(test_color PRIVATE /utf-8)
endif()
add_test(NAME "ColorSystem" COMMAND test_color)

# اختبار مدير الموارد / Resource manager tests (no GPU needed)
add_executable(test_resource_manager
    test_resource_manager.cpp
    test_main.cpp
)
target_include_directories(test_resource_manager PRIVATE ${CATCH2_INCLUDE_DIR})
target_link_libraries(test_resource_manager PRIVATE sad_graphics)
if(MSVC)
    target_compile_options(test_resource_manager PRIVATE /utf-8)
endif()
add_test(NAME "ResourceManager" COMMAND test_resource_manager)

# اختبار واجهة المستخدم / UI widget tests (no GPU needed)
add_executable(test_ui
    test_ui.cpp
    test_main.cpp
)
target_include_directories(test_ui PRIVATE ${CATCH2_INCLUDE_DIR})
target_link_libraries(test_ui PRIVATE sad_graphics)
if(MSVC)
    target_compile_options(test_ui PRIVATE /utf-8)
endif()
add_test(NAME "UIWidgets" COMMAND test_ui)

# اختبار النصوص العربية / Arabic text tests (no GPU needed)
add_executable(test_arabic_text
    test_arabic_text.cpp
    test_main.cpp
)
target_include_directories(test_arabic_text PRIVATE ${CATCH2_INCLUDE_DIR})
target_link_libraries(test_arabic_text PRIVATE sad_graphics)
if(MSVC)
    target_compile_options(test_arabic_text PRIVATE /utf-8)
endif()
add_test(NAME "ArabicText" COMMAND test_arabic_text)

# اختبار الأنظمة الجديدة / New systems tests (no GPU needed)
add_executable(test_systems
    test_systems.cpp
    test_main.cpp
)
target_include_directories(test_systems PRIVATE ${CATCH2_INCLUDE_DIR})
target_link_libraries(test_systems PRIVATE sad_graphics)
if(MSVC)
    target_compile_options(test_systems PRIVATE /utf-8)
endif()
add_test(NAME "Systems" COMMAND test_systems)

message(STATUS "الاختبارات مهيأة / Tests configured: test_types, test_color, test_resource_manager, test_ui, test_arabic_text, test_systems")
