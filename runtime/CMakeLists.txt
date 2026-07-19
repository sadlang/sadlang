# ============================================================================
# Runtime - وقت التشغيل (مُقَلَّص بعد أرشفة المكونات الميتة)
# (AR) في 2025 شُخِّصت ~6 مكتبات runtime بأنها dead code يحذفها linker تماماً
#      من sad-run و sad-build. أُرشفت ثمّ حُذفت في م5 (RFC sadlang-rfcs#10).
#      يحتفظ هذا الملف بالمكتبات النشطة فعلياً فقط:
#        - sad_rt_abi  : C ABI marshalling (نشط)
#        - sad_rt_ffi  : FFI bindings + firewall + sandbox (نشط، يستخدم abi)
#        - sad_graphics_runtime   : UI runtime (نشط، يربطه llvm_codegen_ui)
#        - sad_rt_freestanding : للـ OS / UEFI development (اختياري)
# (EN) ~6 runtime libs were diagnosed as dead code (panic, sandbox, vm, memory,
#      thread, exception). Archived, then deleted in phase 5 (RFC #10).
#      Only actively-linked libs remain here.
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# ABI Library — نظام تكامل C ABI
# (AR) يوفر تحويل الأنواع بين لغة ص ولغة C
# (EN) Provides type conversion between Sad and C
# ============================================================================
file(GLOB ABI_SOURCES "src/abi/*.cpp")
if(ABI_SOURCES)
    add_library(sad_rt_abi STATIC ${ABI_SOURCES})
    target_include_directories(sad_rt_abi
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/abi
    )
    if(MSVC)
        target_compile_options(sad_rt_abi PRIVATE /utf-8 /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_rt_abi library (C ABI, type marshalling)")
endif()

# ============================================================================
# FFI Library
# ============================================================================
file(GLOB FFI_SOURCES "src/ffi/*.cpp")
if(FFI_SOURCES)
    add_library(sad_rt_ffi STATIC ${FFI_SOURCES})
    target_include_directories(sad_rt_ffi
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/ffi
            ${CMAKE_CURRENT_SOURCE_DIR}/include/abi
    )
    if(MSVC)
        target_compile_options(sad_rt_ffi PRIVATE /utf-8 /wd4819 /FS /std:c++20)
    endif()
    # (AR) ربط FFI بمكتبة ABI / (EN) Link FFI to ABI
    if(TARGET sad_rt_abi)
        target_link_libraries(sad_rt_ffi PUBLIC sad_rt_abi)
    endif()
    message(STATUS "Configured sad_rt_ffi library")
endif()

# ============================================================================
# Unified Runtime Library — المكتبة الموحدة لوقت التشغيل
# ============================================================================
add_library(sad_rt_runtime INTERFACE)
if(TARGET sad_rt_abi)
    target_link_libraries(sad_rt_runtime INTERFACE sad_rt_abi)
endif()
if(TARGET sad_rt_ffi)
    target_link_libraries(sad_rt_runtime INTERFACE sad_rt_ffi)
endif()

message(STATUS "Configured sad_rt_runtime (abi + ffi)")

# ============================================================================
# UI Runtime Library — مكتبة وقت التشغيل للواجهات
# (AR) تنفيذ دوال C التي يستدعيها الكود المُترجم بـ LLVM لعناصر الواجهة
#      يربط بين llvm_codegen_ui.cpp ومكتبة sad_graphics
# (EN) C API implementation called by LLVM-compiled code for UI widgets
#      Bridges llvm_codegen_ui.cpp to the sad_graphics library
# ============================================================================
if(TARGET sad_graphics)
    add_library(sad_graphics_runtime STATIC ${CMAKE_SOURCE_DIR}/features/graphics/runtime/sad_ui_runtime.cpp)
    target_include_directories(sad_graphics_runtime
        PUBLIC
            ${CMAKE_SOURCE_DIR}/features/graphics/runtime
            ${CMAKE_SOURCE_DIR}/features/graphics/core/include
            ${CMAKE_SOURCE_DIR}/features/graphics/backends/desktop/include
            ${CMAKE_SOURCE_DIR}/features/graphics/third_party
    )
    target_link_libraries(sad_graphics_runtime PUBLIC sad_graphics)
    if(MSVC)
        target_compile_options(sad_graphics_runtime PRIVATE /utf-8 /wd4819 /FS)
    endif()
    target_link_libraries(sad_rt_runtime INTERFACE sad_graphics_runtime)
    message(STATUS "  [sad_graphics_runtime] ✓ مكتبة وقت التشغيل للواجهات مفعّلة / UI runtime enabled")
else()
    message(STATUS "  [sad_graphics_runtime] ⚠ sad_graphics غير موجود — مكتبة الواجهات معطّلة / sad_graphics not found — UI runtime disabled")
endif()

# ============================================================================
# UI Runtime (Freestanding) — جسر وقت التشغيل بخلفيّة fb0/evdev بلا SDL2
# (AR) نفس sad_ui_runtime.cpp مُصرَّفًا بـSAD_UI_FREESTANDING (لا SAD_UI_USE_SDL2)
#      ومربوطًا بـsad_graphics_freestanding — فيسلك sad_app_run فرعَ fb0/evdev.
#      يستهلكه ربطُ sad-build عند «--حرّ» (بديل sad_graphics_runtime + SDL2).
# (EN) The same bridge TU built with SAD_UI_FREESTANDING and linked to
#      sad_graphics_freestanding ⇒ sad_app_run takes the fb0/evdev branch.
# ============================================================================
if(TARGET sad_graphics_freestanding)
    add_library(sad_graphics_runtime_freestanding STATIC
        ${CMAKE_SOURCE_DIR}/features/graphics/runtime/sad_ui_runtime.cpp)
    target_include_directories(sad_graphics_runtime_freestanding
        PUBLIC
            ${CMAKE_SOURCE_DIR}/features/graphics/runtime
            ${CMAKE_SOURCE_DIR}/features/graphics/core/include
            ${CMAKE_SOURCE_DIR}/features/graphics/backends/freestanding/include
            ${CMAKE_SOURCE_DIR}/features/graphics/backends/web/include
            ${CMAKE_SOURCE_DIR}/features/graphics/third_party
    )
    # (AR) SAD_UI_FREESTANDING يُفعّل فرع fb0/evdev في sad_app_run؛ لا SDL2.
    target_compile_definitions(sad_graphics_runtime_freestanding PUBLIC SAD_UI_FREESTANDING)
    target_link_libraries(sad_graphics_runtime_freestanding PUBLIC sad_graphics_freestanding)
    if(MSVC)
        target_compile_options(sad_graphics_runtime_freestanding PRIVATE /utf-8 /wd4819 /FS)
    endif()
    message(STATUS "  [sad_graphics_runtime_freestanding] ✓ جسر وقت التشغيل الحرّ (fb0/evdev، بلا SDL2)")
endif()

# ============================================================================
# Freestanding Low-Level Runtime — مكتبة وقت التشغيل بلا نظام تشغيل
# (AR) تُبنى فقط عند الطلب. تحتوي على تنفيذات sad_ll_* لوضع --freestanding
# (EN) Built only on request. Contains sad_ll_* implementations for --freestanding mode
# ============================================================================
set(FREESTANDING_SOURCES
    freestanding/sad_lowlevel_rt.c
    freestanding/sad_core.cpp
    freestanding/sad_panic.cpp
    freestanding/sad_allocator.cpp
    freestanding/sad_bump_allocator.cpp
    freestanding/sad_port_io.cpp
)

option(BUILD_FREESTANDING_RT "Build freestanding low-level runtime" OFF)
if(BUILD_FREESTANDING_RT)
    add_library(sad_rt_freestanding STATIC ${FREESTANDING_SOURCES})
    target_include_directories(sad_rt_freestanding
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/freestanding
    )
    if(MSVC)
        target_compile_options(sad_rt_freestanding PRIVATE /utf-8 /GS- /FS)
        target_compile_definitions(sad_rt_freestanding PRIVATE _NO_CRT_STDIO_INLINE)
    else()
        target_compile_options(sad_rt_freestanding PRIVATE
            -ffreestanding -nostdlib -fno-exceptions -fno-rtti
            -mno-red-zone -fno-stack-protector
        )
    endif()
    message(STATUS "Configured sad_rt_freestanding library (bare-metal low-level runtime)")
endif()
