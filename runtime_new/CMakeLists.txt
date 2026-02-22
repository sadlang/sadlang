# ============================================================================
# Runtime - وقت التشغيل
# (AR) البادئة sad_rt_ بدلاً من sad_new_ لتجنب تعارض الأسماء مع compiler_new
# (EN) Using sad_rt_ prefix instead of sad_new_ to avoid name clashes with compiler_new
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# VM Library — مكتبة الآلة الافتراضية
# ============================================================================
file(GLOB VM_SOURCES "src/vm/*.cpp")
add_library(sad_rt_vm STATIC ${VM_SOURCES})
target_include_directories(sad_rt_vm
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vm
        ${CMAKE_SOURCE_DIR}/compiler_new/include
        ${CMAKE_SOURCE_DIR}/vm/include
)
if(MSVC)
    target_compile_options(sad_rt_vm PRIVATE /utf-8 /wd4819 /FS)
endif()

# ============================================================================
# Memory Library — مكتبة إدارة الذاكرة
# ============================================================================
file(GLOB MEMORY_SOURCES "src/memory/*.cpp")
add_library(sad_rt_memory STATIC ${MEMORY_SOURCES})
target_include_directories(sad_rt_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/memory
)
if(MSVC)
    target_compile_options(sad_rt_memory PRIVATE /utf-8 /wd4819 /FS)
endif()

# ============================================================================
# Thread Library — مكتبة خيوط التنفيذ
# ============================================================================
file(GLOB THREAD_SOURCES "src/thread/*.cpp")
add_library(sad_rt_thread STATIC ${THREAD_SOURCES})
target_include_directories(sad_rt_thread
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/thread
)
if(MSVC)
    target_compile_options(sad_rt_thread PRIVATE /utf-8 /wd4819 /FS)
endif()

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
# (AR) ربط FFI بمكتبة ABI — تحتاجها لتعريفات CABI و TypeMarshaller
# (EN) Link FFI to ABI library — needed for CABI and TypeMarshaller definitions
if(TARGET sad_rt_abi)
    target_link_libraries(sad_rt_ffi PUBLIC sad_rt_abi)
endif()

# ============================================================================
# Exception Library
# ============================================================================
file(GLOB EXCEPTION_SOURCES "src/exception/*.cpp")
add_library(sad_rt_exception STATIC ${EXCEPTION_SOURCES})
target_include_directories(sad_rt_exception
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/exception
)
if(MSVC)
    target_compile_options(sad_rt_exception PRIVATE /utf-8 /wd4819 /FS)
endif()

# ============================================================================
# Panic Library (Smart Panic System) — نظام الذعر الذكي
# Phase 95: T1044-T1047
# ============================================================================
file(GLOB PANIC_SOURCES "src/panic/*.cpp")
if(PANIC_SOURCES)
    add_library(sad_rt_panic STATIC ${PANIC_SOURCES})
    target_include_directories(sad_rt_panic
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/panic
    )
    if(MSVC)
        target_compile_options(sad_rt_panic PRIVATE /utf-8 /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_rt_panic library (smart panic, AI analyzer, recovery)")
endif()

# ============================================================================
# Sandbox Library (Permission Enforcement) — نظام الأذونات
# Phase 94: T1042
# ============================================================================
file(GLOB SANDBOX_SOURCES "src/sandbox/*.cpp")
if(SANDBOX_SOURCES)
    add_library(sad_rt_sandbox STATIC ${SANDBOX_SOURCES})
    target_include_directories(sad_rt_sandbox
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sandbox
    )
    if(MSVC)
        target_compile_options(sad_rt_sandbox PRIVATE /utf-8 /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_rt_sandbox library (permission enforcer)")
endif()

# ============================================================================
# Unified Runtime Library — المكتبة الموحدة لوقت التشغيل
# ============================================================================
add_library(sad_rt_runtime INTERFACE)
target_link_libraries(sad_rt_runtime
    INTERFACE
        sad_rt_vm
        sad_rt_memory
        sad_rt_thread
        sad_rt_ffi
        sad_rt_exception
)

# (AR) ربط مكتبة ABI إذا تم بناؤها / (EN) Link ABI library if built
if(TARGET sad_rt_abi)
    target_link_libraries(sad_rt_runtime INTERFACE sad_rt_abi)
endif()

# Link optional security libraries if they were built
if(TARGET sad_rt_panic)
    target_link_libraries(sad_rt_runtime INTERFACE sad_rt_panic)
endif()
if(TARGET sad_rt_sandbox)
    target_link_libraries(sad_rt_runtime INTERFACE sad_rt_sandbox)
endif()

message(STATUS "Configured sad_rt_runtime library (vm, memory, thread, ffi, exception, abi, panic, sandbox)")

