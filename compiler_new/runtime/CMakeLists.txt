# =============================================================================
# ملف: CMakeLists.txt
# الوصف: بناء مكتبة Runtime للرسومات
# =============================================================================

cmake_minimum_required(VERSION 3.16)

# ═══════════════════════════════════════════════════════════════════════════
# مكتبة Runtime للرسومات
# ═══════════════════════════════════════════════════════════════════════════

# البحث عن SDL2
find_package(SDL2 QUIET)
find_package(SDL2_ttf QUIET)

# ═══════════════════════════════════════════════════════════════════════════
# ملفات المصدر
# ═══════════════════════════════════════════════════════════════════════════

set(SAD_GRAPHICS_RUNTIME_SOURCES
    sad_graphics_runtime.cpp
)

# ═══════════════════════════════════════════════════════════════════════════
# المكتبة الثابتة
# ═══════════════════════════════════════════════════════════════════════════

if(SDL2_FOUND)
    # مكتبة ثابتة للربط مع البرامج المترجمة
    add_library(sad_graphics_runtime STATIC ${SAD_GRAPHICS_RUNTIME_SOURCES})
    
    target_include_directories(sad_graphics_runtime PUBLIC
        ${SDL2_INCLUDE_DIRS}
    )
    
    target_link_libraries(sad_graphics_runtime PUBLIC
        ${SDL2_LIBRARIES}
    )
    
    if(SDL2_ttf_FOUND)
        target_link_libraries(sad_graphics_runtime PUBLIC
            SDL2_ttf::SDL2_ttf
        )
    endif()
    
    # تعيين خصائص C++17
    target_compile_features(sad_graphics_runtime PUBLIC cxx_std_17)
    
    # تصدير الرموز C
    if(WIN32)
        target_compile_definitions(sad_graphics_runtime PRIVATE
            _CRT_SECURE_NO_WARNINGS
        )
    endif()
    
    # نسخ إلى مجلد bin
    add_custom_command(TARGET sad_graphics_runtime POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:sad_graphics_runtime>
            ${CMAKE_BINARY_DIR}/bin/$<CONFIG>/libsad_graphics.a
        COMMENT "نسخ مكتبة Runtime للرسومات..."
    )
    
    message(STATUS "✓ تم تفعيل مكتبة Runtime للرسومات")
else()
    message(WARNING "⚠ SDL2 غير موجود - مكتبة Runtime للرسومات معطلة")
endif()

# ═══════════════════════════════════════════════════════════════════════════
# مكتبة ديناميكية (اختياري)
# ═══════════════════════════════════════════════════════════════════════════

option(BUILD_SHARED_GRAPHICS_RUNTIME "بناء مكتبة ديناميكية للرسومات" OFF)

if(BUILD_SHARED_GRAPHICS_RUNTIME AND SDL2_FOUND)
    add_library(sad_graphics_runtime_shared SHARED ${SAD_GRAPHICS_RUNTIME_SOURCES})
    
    target_include_directories(sad_graphics_runtime_shared PUBLIC
        ${SDL2_INCLUDE_DIRS}
    )
    
    target_link_libraries(sad_graphics_runtime_shared PUBLIC
        ${SDL2_LIBRARIES}
    )
    
    if(SDL2_ttf_FOUND)
        target_link_libraries(sad_graphics_runtime_shared PUBLIC
            SDL2_ttf::SDL2_ttf
        )
    endif()
    
    target_compile_features(sad_graphics_runtime_shared PUBLIC cxx_std_17)
    
    if(WIN32)
        set_target_properties(sad_graphics_runtime_shared PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
        )
    endif()
    
    message(STATUS "✓ تم تفعيل مكتبة Runtime الديناميكية للرسومات")
endif()
