# ============================================================================
# Runtime - وقت التشغيل
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# VM Library
# ============================================================================
file(GLOB VM_SOURCES "src/vm/*.cpp")
add_library(sad_new_vm STATIC ${VM_SOURCES})
target_include_directories(sad_new_vm
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/vm
        ${CMAKE_SOURCE_DIR}/compiler_new/include
        ${CMAKE_SOURCE_DIR}/vm/include
)

# ============================================================================
# Memory Library
# ============================================================================
file(GLOB MEMORY_SOURCES "src/memory/*.cpp")
add_library(sad_new_memory STATIC ${MEMORY_SOURCES})
target_include_directories(sad_new_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/memory
)

# ============================================================================
# Thread Library
# ============================================================================
file(GLOB THREAD_SOURCES "src/thread/*.cpp")
add_library(sad_new_thread STATIC ${THREAD_SOURCES})
target_include_directories(sad_new_thread
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/thread
)

# ============================================================================
# FFI Library
# ============================================================================
file(GLOB FFI_SOURCES "src/ffi/*.cpp")
add_library(sad_new_ffi STATIC ${FFI_SOURCES})
target_include_directories(sad_new_ffi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/ffi
        ${CMAKE_CURRENT_SOURCE_DIR}/include/abi
)

# ============================================================================
# Exception Library
# ============================================================================
file(GLOB EXCEPTION_SOURCES "src/exception/*.cpp")
add_library(sad_new_exception STATIC ${EXCEPTION_SOURCES})
target_include_directories(sad_new_exception
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/exception
)

# ============================================================================
# Panic Library (Smart Panic System) — نظام الذعر الذكي
# Phase 95: T1044-T1047
# ============================================================================
file(GLOB PANIC_SOURCES "src/panic/*.cpp")
if(PANIC_SOURCES)
    add_library(sad_new_panic STATIC ${PANIC_SOURCES})
    target_include_directories(sad_new_panic
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/panic
    )
    if(MSVC)
        target_compile_options(sad_new_panic PRIVATE /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_new_panic library (smart panic, AI analyzer, recovery)")
endif()

# ============================================================================
# Sandbox Library (Permission Enforcement) — نظام الأذونات
# Phase 94: T1042
# ============================================================================
file(GLOB SANDBOX_SOURCES "src/sandbox/*.cpp")
if(SANDBOX_SOURCES)
    add_library(sad_new_sandbox STATIC ${SANDBOX_SOURCES})
    target_include_directories(sad_new_sandbox
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sandbox
    )
    if(MSVC)
        target_compile_options(sad_new_sandbox PRIVATE /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_new_sandbox library (permission enforcer)")
endif()

# ============================================================================
# Unified Runtime Library
# ============================================================================
add_library(sad_new_runtime INTERFACE)
target_link_libraries(sad_new_runtime
    INTERFACE
        sad_new_vm
        sad_new_memory
        sad_new_thread
        sad_new_ffi
        sad_new_exception
)

# Link optional security libraries if they were built
if(TARGET sad_new_panic)
    target_link_libraries(sad_new_runtime INTERFACE sad_new_panic)
endif()
if(TARGET sad_new_sandbox)
    target_link_libraries(sad_new_runtime INTERFACE sad_new_sandbox)
endif()

message(STATUS "Configured sad_new_runtime library (vm, memory, thread, ffi, exception, panic, sandbox)")

