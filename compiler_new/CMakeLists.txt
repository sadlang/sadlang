# ============================================================================
# Compiler - المترجم
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# Find LLVM (use the same path from main CMakeLists.txt)
if(NOT LLVM_FOUND)
    set(LLVM_DIR "C:/LLVM/lib/cmake/llvm" CACHE PATH "LLVM CMake directory")
    find_package(LLVM CONFIG)
endif()

if(LLVM_FOUND)
    message(STATUS "compiler_new: Found LLVM ${LLVM_PACKAGE_VERSION}")
    include_directories(${LLVM_INCLUDE_DIRS})
    add_definitions(${LLVM_DEFINITIONS})
else()
    message(WARNING "compiler_new: LLVM not found - some features disabled")
endif()

# ============================================================================
# Frontend Library (SIR)
# ============================================================================
file(GLOB FRONTEND_SOURCES "src/frontend/*.cpp")
file(GLOB FRONTEND_HEADERS "include/frontend/*.h")

add_library(sad_new_frontend STATIC ${FRONTEND_SOURCES})

target_include_directories(sad_new_frontend
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/frontend
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/parser/include
)

# ============================================================================
# Middle Library (Optimizer)
# FIXED: API updated to use new SIRInstruction API (January 2026)
# ============================================================================
file(GLOB MIDDLE_SOURCES "src/middle/*.cpp")
file(GLOB MIDDLE_HEADERS "include/middle/*.h")
add_library(sad_new_optimizer STATIC ${MIDDLE_SOURCES})

target_include_directories(sad_new_optimizer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/middle
        ${CMAKE_CURRENT_SOURCE_DIR}/include/frontend
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
)

target_link_libraries(sad_new_optimizer PUBLIC sad_new_frontend)

# ============================================================================
# Backend Library (LLVM)
# ============================================================================
if(LLVM_FOUND)
    file(GLOB LLVM_BACKEND_SOURCES "src/backend/llvm/*.cpp")
    file(GLOB LLVM_BACKEND_HEADERS "include/backend/llvm/*.h")

    add_library(sad_new_llvm_backend STATIC ${LLVM_BACKEND_SOURCES})

    target_include_directories(sad_new_llvm_backend
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/backend/llvm
            ${LLVM_INCLUDE_DIRS}
    )

    llvm_map_components_to_libnames(llvm_libs
        Core
        Support
        IRReader
        Passes
        ExecutionEngine
        OrcJIT
        MCJIT
        X86CodeGen
        X86AsmParser
        X86Desc
        X86Info
        native
    )

    target_link_libraries(sad_new_llvm_backend
        PUBLIC
            ${llvm_libs}
            sad_new_frontend
            # sad_new_optimizer  # DISABLED
    )
endif()

# ============================================================================
# JIT Library
# ============================================================================
if(LLVM_FOUND)
    set(JIT_SOURCES
        src/jit/jit_metrics_new.cpp
        src/jit/jit_engine.cpp
        src/jit/jit_cache.cpp
        src/jit/jit_hash.cpp
        src/jit/jit_profiler.cpp
        src/jit/jit_bridge.cpp
        src/jit/hot_path_detector.cpp
    )
    file(GLOB JIT_HEADERS "include/jit/*.h")
    add_library(sad_new_jit STATIC ${JIT_SOURCES})
    target_include_directories(sad_new_jit
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/jit
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/shared/lexer/include
            ${CMAKE_SOURCE_DIR}/shared/parser/include
            ${CMAKE_SOURCE_DIR}/shared/ast/include
            ${CMAKE_SOURCE_DIR}/shared/types/include
            ${CMAKE_SOURCE_DIR}/shared/errors/include
            ${LLVM_INCLUDE_DIRS}
    )
    target_link_libraries(sad_new_jit PUBLIC sad_new_llvm_backend ${llvm_libs})
endif()

# ============================================================================
# Bytecode Library - DISABLED: uses old SIR API, needs complete rewrite
# ============================================================================
# file(GLOB BYTECODE_SOURCES "src/bytecode/*.cpp")
# file(GLOB BYTECODE_HEADERS "include/bytecode/*.h")
# add_library(sad_new_bytecode STATIC ${BYTECODE_SOURCES})
# target_include_directories(sad_new_bytecode PUBLIC ...)

# ============================================================================
# Type System Library
# ============================================================================
file(GLOB TYPE_SYSTEM_SOURCES "src/types/*.cpp")
file(GLOB TYPE_SYSTEM_HEADERS "include/types/*.h")

add_library(sad_new_type_system STATIC ${TYPE_SYSTEM_SOURCES})

target_include_directories(sad_new_type_system
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/types
        ${CMAKE_SOURCE_DIR}/include
)

# ============================================================================
# Unified Compiler Library
# ============================================================================
add_library(sad_new_compiler INTERFACE)

if(LLVM_FOUND)
    target_link_libraries(sad_new_compiler
        INTERFACE
            sad_new_frontend
            # sad_new_optimizer  # DISABLED: PDB locking issue
            sad_new_llvm_backend
            sad_new_jit
            # sad_new_bytecode  # DISABLED: needs rewrite
            sad_new_type_system
    )
else()
    target_link_libraries(sad_new_compiler
        INTERFACE
            sad_new_frontend
            # sad_new_optimizer  # DISABLED
            # sad_new_bytecode  # DISABLED
            sad_new_type_system
    )
endif()

# للتوافق مع الكود القديم
target_include_directories(sad_new_compiler
    INTERFACE
        ${CMAKE_SOURCE_DIR}/compiler/frontend/include
        ${CMAKE_SOURCE_DIR}/compiler/optimizer/include
        ${CMAKE_SOURCE_DIR}/compiler/backends/llvm
        ${CMAKE_SOURCE_DIR}/compiler/jit/include
        ${CMAKE_SOURCE_DIR}/compiler/bytecode/include
        ${CMAKE_SOURCE_DIR}/compiler/type_system/include
)

message(STATUS "Configured sad_new_compiler library (frontend, optimizer, llvm_backend, jit, bytecode, type_system)")
