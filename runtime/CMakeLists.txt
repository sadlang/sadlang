# ============================================================================
# Runtime - وقت التشغيل (مُقَلَّص بعد أرشفة المكونات الميتة)
# (AR) في 2025 شُخِّصت ~6 مكتبات runtime بأنها dead code يحذفها linker تماماً
#      من sad-run و sad-build. أُرشفت ثمّ حُذفت في م5 (RFC sadlang-rfcs#10).
#      يحتفظ هذا الملف بالمكتبات النشطة فعلياً فقط:
#        - sad_rt_abi  : C ABI marshalling (نشط)
#        - sad_rt_ffi  : FFI bindings + firewall + sandbox (نشط، يستخدم abi)
#        - sad_rt_ui   : UI runtime (نشط، يربطه llvm_codegen_ui)
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
#      يربط بين llvm_codegen_ui.cpp ومكتبة sad_ui
# (EN) C API implementation called by LLVM-compiled code for UI widgets
#      Bridges llvm_codegen_ui.cpp to the sad_ui library
# ============================================================================
if(TARGET sad_ui)
    add_library(sad_rt_ui STATIC sad_ui_runtime.cpp)
    target_include_directories(sad_rt_ui
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/sad_ui/core/include
            ${CMAKE_SOURCE_DIR}/sad_ui/backends/desktop/include
            ${CMAKE_SOURCE_DIR}/graphics/third_party
    )
    target_link_libraries(sad_rt_ui PUBLIC sad_ui)
    if(MSVC)
        target_compile_options(sad_rt_ui PRIVATE /utf-8 /wd4819 /FS)
    endif()
    target_link_libraries(sad_rt_runtime INTERFACE sad_rt_ui)
    message(STATUS "  [sad_rt_ui] ✓ مكتبة وقت التشغيل للواجهات مفعّلة / UI runtime enabled")
else()
    message(STATUS "  [sad_rt_ui] ⚠ sad_ui غير موجود — مكتبة الواجهات معطّلة / sad_ui not found — UI runtime disabled")
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
