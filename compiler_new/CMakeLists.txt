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
        ${CMAKE_SOURCE_DIR}/shared/modules/include
)

# ============================================================================
# Middle Library (Optimizer)
# FIXED: API updated to use new SIRInstruction API (January 2026)
# ============================================================================
file(GLOB MIDDLE_SOURCES "src/middle/*.cpp")
list(REMOVE_ITEM MIDDLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/middle/async_transform.cpp")  # TODO: Fix missing includes
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

# م-ب03: إصلاح PDB locking بإضافة /FS
if(MSVC)
    target_compile_options(sad_new_optimizer PRIVATE /FS /utf-8)
endif()

# ============================================================================
# Backend Library (LLVM)
# ============================================================================
if(LLVM_FOUND)
    file(GLOB LLVM_BACKEND_SOURCES "src/backend/llvm/*.cpp")
    file(GLOB LLVM_BACKEND_HEADERS "include/backend/llvm/*.h")
    
    # م-ب01: إضافة ملفات backend غير-LLVM المطلوبة لنظام النحلة
    # تم إصلاح أخطاء الترجمة في الملفات التالية:
    # - interrupt_handlers.cpp: إصلاح تعارض ماكروات Windows (BREAKPOINT/OVERFLOW)
    # - solidity_ffi.cpp: إضافة <iomanip> و <functional>
    # - pattern_codegen.cpp: إصلاح أسماء enum (NUMBER→DOUBLE, NULL_TYPE→VOID)
    #
    # الملفات التالية تحتاج إصلاحات أعمق وتم تأجيلها:
    # - async_codegen.cpp: أخطاء متعددة في تعريفات الأنواع
    # - ffi_wrapper.cpp: أخطاء في c_abi.h و c_types.h headers
    # - inline_asm.cpp: أخطاء في IRBuilder template
    # - llvm_compiler_pipeline.cpp: مسارات include غير صحيحة في الهيدر
    set(BACKEND_COMMON_SOURCES
        src/backend/entry_point.cpp
        src/backend/linker_script.cpp
        src/backend/panic_handler.cpp
        src/backend/c_abi.cpp
        src/backend/interrupt_handlers.cpp
        src/backend/solidity_ffi.cpp
        src/backend/pattern_codegen.cpp
    )
    
    # م-ب02: إضافة ملفات pipeline
    set(PIPELINE_SOURCES
        src/pipeline/no_std_mode.cpp
        src/pipeline/no_std_integration.cpp
    )

    add_library(sad_new_llvm_backend STATIC 
        ${LLVM_BACKEND_SOURCES}
        ${BACKEND_COMMON_SOURCES}
        ${PIPELINE_SOURCES}
    )

    target_include_directories(sad_new_llvm_backend
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/backend/llvm
            ${CMAKE_CURRENT_SOURCE_DIR}/include/backend
            ${CMAKE_CURRENT_SOURCE_DIR}/include/pipeline
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
            sad_new_optimizer
    )
    
    # م-ب01: إصلاح ترميز Unicode للملفات التي تحتوي على معرّفات عربية
    if(MSVC)
        target_compile_options(sad_new_llvm_backend PRIVATE /utf-8 /wd4819 /FS)
    endif()
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
# Semantic Analysis Library (نظام الملكية والاستعارة)
# ============================================================================
file(GLOB SEMANTIC_SOURCES "src/semantic/*.cpp")
file(GLOB SEMANTIC_HEADERS "include/semantic/*.h")

add_library(sad_new_semantic STATIC ${SEMANTIC_SOURCES})

if(MSVC)
    target_compile_options(sad_new_semantic PRIVATE /wd4819 /FS)
endif()

target_include_directories(sad_new_semantic
    PUBLIC
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/semantic
        ${CMAKE_CURRENT_SOURCE_DIR}/include/frontend
        ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(sad_new_semantic PUBLIC sad_shared sad_new_type_system)

# ============================================================================
# Security Library (Triple Guard + Permissions + Diagnostics)
# Phase 92: T1031-T1034 (Triple Guard)
# Phase 94: T1041, T1043 (Permission checker + Arabic errors)
# ============================================================================
file(GLOB SECURITY_CODEGEN_SOURCES "src/codegen/guard_*.cpp")
file(GLOB SECURITY_SEMA_SOURCES "src/sema/permission_*.cpp")
file(GLOB SECURITY_DIAG_SOURCES "src/diagnostics/permission_*.cpp")

set(SECURITY_ALL_SOURCES ${SECURITY_CODEGEN_SOURCES} ${SECURITY_SEMA_SOURCES} ${SECURITY_DIAG_SOURCES})

if(SECURITY_ALL_SOURCES)
    add_library(sad_new_security STATIC ${SECURITY_ALL_SOURCES})
    target_include_directories(sad_new_security
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/codegen
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sema
            ${CMAKE_CURRENT_SOURCE_DIR}/src/diagnostics
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/shared/ast/include
            ${CMAKE_SOURCE_DIR}/shared/types/include
            ${CMAKE_SOURCE_DIR}/shared/errors/include
            ${CMAKE_SOURCE_DIR}/shared/lexer/include
    )
    if(MSVC)
        target_compile_options(sad_new_security PRIVATE /utf-8 /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_new_security library (triple guard, permissions, diagnostics)")
endif()

# ============================================================================
# Memory System Library (نظام الذاكرة الذكي)
# ============================================================================
file(GLOB MEMORY_SOURCES "src/memory/*.cpp")
file(GLOB MEMORY_HEADERS "include/memory/*.h")

add_library(sad_new_memory STATIC ${MEMORY_SOURCES})

if(MSVC)
    target_compile_options(sad_new_memory PRIVATE /wd4819 /FS)
endif()

target_include_directories(sad_new_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/memory
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
)

# ============================================================================
# Type System Library
# ============================================================================
file(GLOB TYPE_SYSTEM_SOURCES "src/types/*.cpp")
file(GLOB TYPE_SYSTEM_HEADERS "include/types/*.h")

# استبعاد الملفات ذات الأخطاء مؤقتاً / Exclude files with errors temporarily
list(REMOVE_ITEM TYPE_SYSTEM_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/types/c_types.cpp"
)

add_library(sad_new_type_system STATIC ${TYPE_SYSTEM_SOURCES})

# تعطيل تحذيرات Unicode للنصوص العربية / Disable Unicode warnings for Arabic text
if(MSVC)
    target_compile_options(sad_new_type_system PRIVATE /wd4819 /FS)
endif()

target_include_directories(sad_new_type_system
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/types
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
)

# ============================================================================
# Unified Compiler Library
# ============================================================================
add_library(sad_new_compiler INTERFACE)

if(LLVM_FOUND)
    target_link_libraries(sad_new_compiler
        INTERFACE
            sad_new_frontend
            sad_new_optimizer
            sad_new_llvm_backend
            sad_new_jit
            # sad_new_bytecode  # DISABLED: needs rewrite
            sad_new_type_system
            sad_new_semantic
            sad_new_memory
    )
    # م-ب04: تفعيل مكتبة الأمان (إصلاح ترميز Unicode بـ /utf-8)
    if(TARGET sad_new_security)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_security)
    endif()
else()
    target_link_libraries(sad_new_compiler
        INTERFACE
            sad_new_frontend
            sad_new_optimizer
            # sad_new_bytecode  # DISABLED
            sad_new_type_system
            sad_new_semantic
            sad_new_memory
    )
    if(TARGET sad_new_security)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_security)
    endif()
    # (EN) Disabled temporarily (same reason as above)
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

message(STATUS "Configured sad_new_compiler library (frontend, optimizer, llvm_backend, jit, type_system, semantic, memory)")
