# ═══════════════════════════════════════════════════════════════════════════
# CMake لدعم الرسومات في المترجم
# Graphics Support for Compiler
# ═══════════════════════════════════════════════════════════════════════════

# ملفات مصدر نظام الرسومات للمترجم
set(COMPILER_GRAPHICS_SOURCES
    # LLVM Graphics CodeGen
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_new/src/backend/llvm/llvm_graphics_codegen.cpp
    
    # SIR Graphics Intrinsics
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_new/src/sir/sir_graphics_intrinsics.cpp
    
    # Graphics Linker
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_new/src/backend/graphics_linker.cpp
)

# ملفات الرأس
set(COMPILER_GRAPHICS_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_new/include/backend/llvm/llvm_graphics_codegen.h
)

# إضافة ملفات للمترجم إذا كان LLVM متوفراً
if(HAS_LLVM)
    message(STATUS "🎨 تفعيل دعم الرسومات في المترجم / Enabling graphics support in compiler")
    
    # إضافة المصادر لمكتبة المترجم
    target_sources(sadc_lib PRIVATE ${COMPILER_GRAPHICS_SOURCES})
    
    # إضافة تعريف الماكرو
    target_compile_definitions(sadc_lib PRIVATE HAS_GRAPHICS_SUPPORT)
    
    # ربط SDL2 إذا كان متوفراً
    find_package(SDL2 QUIET)
    if(SDL2_FOUND)
        message(STATUS "📦 تم العثور على SDL2 / SDL2 found")
        target_include_directories(sadc_lib PRIVATE ${SDL2_INCLUDE_DIRS})
        target_link_libraries(sadc_lib PRIVATE ${SDL2_LIBRARIES})
    else()
        message(STATUS "📦 SDL2 غير موجود - الرسومات في وقت التشغيل فقط / SDL2 not found - runtime graphics only")
    endif()
endif()

# ═══════════════════════════════════════════════════════════════════════════
# خيارات البناء
# Build Options
# ═══════════════════════════════════════════════════════════════════════════

option(ENABLE_GRAPHICS_COMPILER "تفعيل ترجمة الرسومات / Enable graphics compilation" ON)

if(ENABLE_GRAPHICS_COMPILER AND HAS_LLVM)
    set(GRAPHICS_COMPILER_ENABLED TRUE)
    message(STATUS "✅ دعم الرسومات مفعّل / Graphics support enabled")
else()
    set(GRAPHICS_COMPILER_ENABLED FALSE)
    message(STATUS "ℹ️ دعم الرسومات معطّل / Graphics support disabled")
endif()

# تصدير المتغير
set(GRAPHICS_COMPILER_ENABLED ${GRAPHICS_COMPILER_ENABLED} PARENT_SCOPE)
