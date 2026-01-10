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

message(STATUS "Configured sad_new_runtime library (vm, memory, thread, ffi, exception)")

