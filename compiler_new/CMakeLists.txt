# ============================================================================
# Compiler - المترجم
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# C++17 required for std::optional, structured bindings, etc.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Resolve shared directory path (works both standalone and as subdirectory)
if(NOT DEFINED SAD_SHARED_DIR)
    if(EXISTS "${CMAKE_SOURCE_DIR}/shared")
        set(SAD_SHARED_DIR "${CMAKE_SOURCE_DIR}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../shared")
        set(SAD_SHARED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
    else()
        set(SAD_SHARED_DIR "${CMAKE_SOURCE_DIR}")
    endif()
endif()

# Find LLVM (use the same path from main CMakeLists.txt)
if(NOT LLVM_FOUND)
    # لا نُثبّت LLVM_DIR هنا - يأتي من cmake/llvm.cmake أو من سطر الأوامر
    find_package(LLVM QUIET CONFIG)
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
        ${SAD_SHARED_DIR}/include
        ${SAD_SHARED_DIR}/shared/ast/include
        ${SAD_SHARED_DIR}/shared/types/include
        ${SAD_SHARED_DIR}/shared/errors/include
        ${SAD_SHARED_DIR}/shared/lexer/include
        ${SAD_SHARED_DIR}/shared/parser/include
        ${SAD_SHARED_DIR}/shared/modules/include
)

# إصلاح تحذيرات UTF-8 / Fix UTF-8 encoding warnings for frontend
if(MSVC)
    target_compile_options(sad_new_frontend PRIVATE /utf-8 /FS)
endif()

# ============================================================================
# Middle Library (Optimizer)
# FIXED: API updated to use new SIRInstruction API (January 2026)
# ============================================================================
file(GLOB MIDDLE_SOURCES "src/middle/*.cpp")
# (AR) تم إعادة تضمين async_transform.cpp بعد إصلاح الأخطاء
# (EN) Re-included async_transform.cpp after fixing errors
file(GLOB MIDDLE_HEADERS "include/middle/*.h")
add_library(sad_new_optimizer STATIC ${MIDDLE_SOURCES})

target_include_directories(sad_new_optimizer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/middle
        ${CMAKE_CURRENT_SOURCE_DIR}/include/frontend
        ${CMAKE_SOURCE_DIR}/include
        ${SAD_SHARED_DIR}/shared/ast/include
        ${SAD_SHARED_DIR}/shared/types/include
        ${SAD_SHARED_DIR}/shared/errors/include
        ${SAD_SHARED_DIR}/shared/lexer/include
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
        src/backend/async_codegen.cpp
        src/backend/ffi_wrapper.cpp
        src/backend/inline_asm.cpp
    )
    
    # م-ب02: إضافة ملفات pipeline
    set(PIPELINE_SOURCES
        src/pipeline/no_std_mode.cpp
        src/pipeline/no_std_integration.cpp
        src/pipeline/llvm_compiler_pipeline.cpp
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
            ${SAD_SHARED_DIR}/shared/lexer/include
            ${SAD_SHARED_DIR}/shared/parser/include
            ${SAD_SHARED_DIR}/shared/ast/include
            ${SAD_SHARED_DIR}/shared/types/include
            ${SAD_SHARED_DIR}/shared/errors/include
            ${LLVM_INCLUDE_DIRS}
    )
    target_link_libraries(sad_new_jit PUBLIC sad_new_llvm_backend ${llvm_libs})
endif()

# ============================================================================
# Bytecode Library (تنسيق ملفات البايت كود)
# ============================================================================
# (AR) الملفات المستقلة (format): decoder, encoder, constant_pool, symbol_table, magic, version, validator, opcodes
# (EN) Standalone format files: decoder, encoder, constant_pool, symbol_table, magic, version, validator, opcodes
# (AR) ملفات SIR المعتمدة (emitter.cpp, bytecode.cpp) مؤجلة - تحتاج تحديث SIR API
# (EN) SIR-dependent files (emitter.cpp, bytecode.cpp) deferred - need SIR API update
file(GLOB BYTECODE_SOURCES "src/bytecode/*.cpp")
file(GLOB BYTECODE_HEADERS "include/bytecode/*.h")

# استبعاد الملفات المعتمدة على SIR API القديم
list(FILTER BYTECODE_SOURCES EXCLUDE REGEX "emitter\\.cpp")
list(FILTER BYTECODE_SOURCES EXCLUDE REGEX "bytecode\\.cpp")

add_library(sad_new_bytecode STATIC ${BYTECODE_SOURCES})

target_include_directories(sad_new_bytecode
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/bytecode
        ${CMAKE_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(sad_new_bytecode PRIVATE /utf-8 /wd4819 /FS)
endif()

message(STATUS "Configured sad_new_bytecode library (format utilities)")

# ============================================================================
# Semantic Analysis Library (نظام الملكية والاستعارة)
# ============================================================================
file(GLOB SEMANTIC_SOURCES "src/semantic/*.cpp")
file(GLOB SEMANTIC_HEADERS "include/semantic/*.h")

add_library(sad_new_semantic STATIC ${SEMANTIC_SOURCES})

if(MSVC)
    target_compile_options(sad_new_semantic PRIVATE /utf-8 /wd4819 /FS)
endif()

target_include_directories(sad_new_semantic
    PUBLIC
        ${SAD_SHARED_DIR}/shared/types/include
        ${SAD_SHARED_DIR}/shared/ast/include
        ${SAD_SHARED_DIR}/shared/lexer/include
        ${SAD_SHARED_DIR}/shared/parser/include
        ${SAD_SHARED_DIR}/shared/errors/include
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
            ${SAD_SHARED_DIR}/shared/ast/include
            ${SAD_SHARED_DIR}/shared/types/include
            ${SAD_SHARED_DIR}/shared/errors/include
            ${SAD_SHARED_DIR}/shared/lexer/include
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
    target_compile_options(sad_new_memory PRIVATE /utf-8 /wd4819 /FS)
endif()

target_include_directories(sad_new_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/memory
        ${CMAKE_SOURCE_DIR}/include
        ${SAD_SHARED_DIR}/shared/ast/include
        ${SAD_SHARED_DIR}/shared/types/include
        ${SAD_SHARED_DIR}/shared/errors/include
)

# ============================================================================
# Type System Library
# ============================================================================
file(GLOB TYPE_SYSTEM_SOURCES "src/types/*.cpp")
file(GLOB TYPE_SYSTEM_HEADERS "include/types/*.h")

# (AR) c_types.cpp يعتمد على أنواع FFI غير مكتملة التعريف (CStructType, CUnionType, CEnumType, CTypedefType)
#      ويحتاج LLVM headers عبر c_abi.h - سيُفعَّل عند اكتمال تعريف هذه الأنواع
# (EN) c_types.cpp depends on incomplete FFI types (CStructType, CUnionType, CEnumType, CTypedefType)
#      and needs LLVM headers via c_abi.h - will be enabled when these types are fully defined
list(FILTER TYPE_SYSTEM_SOURCES EXCLUDE REGEX "c_types\\.cpp")

add_library(sad_new_type_system STATIC ${TYPE_SYSTEM_SOURCES})

# تعطيل تحذيرات Unicode للنصوص العربية / Disable Unicode warnings for Arabic text
if(MSVC)
    target_compile_options(sad_new_type_system PRIVATE /utf-8 /wd4819 /FS)
endif()

target_include_directories(sad_new_type_system
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/types
        ${CMAKE_SOURCE_DIR}/include
        ${SAD_SHARED_DIR}/shared/lexer/include
        ${SAD_SHARED_DIR}/shared/ast/include
        ${SAD_SHARED_DIR}/shared/types/include
        ${SAD_SHARED_DIR}/shared/errors/include
)

# ============================================================================
# Tools Library (الأدوات المدمجة: المنسّق، الحارس الأمني، التوثيق، البناء)
# Merged tools: formatter, security scanner, docs emitter, build system
# ============================================================================
set(TOOLS_SOURCES
    src/format/sad_formatter.cpp
    src/security/security_scanner.cpp
    src/security/taint_tracker.cpp
    src/meta/docs_emitter.cpp
    src/build/build_system.cpp
)

# إضافة ملفات موجودة فعلاً
set(TOOLS_VERIFIED_SOURCES "")
foreach(src ${TOOLS_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
        list(APPEND TOOLS_VERIFIED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    else()
        message(STATUS "  [tools] Skipped missing: ${src}")
    endif()
endforeach()

if(TOOLS_VERIFIED_SOURCES)
    add_library(sad_new_tools STATIC ${TOOLS_VERIFIED_SOURCES})
    target_include_directories(sad_new_tools
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/format
            ${CMAKE_CURRENT_SOURCE_DIR}/include/security
            ${CMAKE_CURRENT_SOURCE_DIR}/include/meta
            ${CMAKE_CURRENT_SOURCE_DIR}/include/build
            ${CMAKE_SOURCE_DIR}/include
            ${SAD_SHARED_DIR}/shared/ast/include
            ${SAD_SHARED_DIR}/shared/types/include
            ${SAD_SHARED_DIR}/shared/errors/include
            ${SAD_SHARED_DIR}/shared/lexer/include
            ${SAD_SHARED_DIR}/shared/parser/include
    )
    if(MSVC)
        target_compile_options(sad_new_tools PRIVATE /utf-8 /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_new_tools library (formatter, guard, docs, build)")
else()
    message(STATUS "  [tools] No verified source files — sad_new_tools skipped")
endif()

# ============================================================================
# SadUI IR Library (نظام الواجهات الموحد عبر المنصات)
# ============================================================================
set(UI_IR_SOURCES
    src/ui/sad_ui_ir.cpp
    src/ui/sad_ui_ir_builder.cpp
    src/ui/sad_ui_ir_validator.cpp
    src/ui/sad_ui_ir_optimizer.cpp
    src/ui/desktop_sdl2_codegen.cpp
    src/ui/android_compose_codegen.cpp
    src/ui/ios_swiftui_codegen.cpp
    src/ui/web_html_codegen.cpp
    src/ui/sad_ui_pipeline.cpp
)

# التحقق من وجود الملفات
set(UI_IR_VERIFIED_SOURCES "")
foreach(src ${UI_IR_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
        list(APPEND UI_IR_VERIFIED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    else()
        message(STATUS "  [ui_ir] Skipped missing: ${src}")
    endif()
endforeach()

if(UI_IR_VERIFIED_SOURCES)
    add_library(sad_ui_ir STATIC ${UI_IR_VERIFIED_SOURCES})
    target_include_directories(sad_ui_ir
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/ui
            ${CMAKE_SOURCE_DIR}/include
            ${SAD_SHARED_DIR}/shared/ast/include
            ${SAD_SHARED_DIR}/shared/types/include
            ${SAD_SHARED_DIR}/shared/errors/include
            ${SAD_SHARED_DIR}/shared/lexer/include
            ${SAD_SHARED_DIR}/shared/parser/include
    )
    if(MSVC)
        target_compile_options(sad_ui_ir PRIVATE /utf-8 /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_ui_ir library (SadUI IR: builder, validator, optimizer, codegen)")
else()
    message(STATUS "  [ui_ir] No source files found — sad_ui_ir skipped")
endif()

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
            sad_new_bytecode
            sad_new_type_system
            sad_new_semantic
            sad_new_memory
    )
    # م-ب04: تفعيل مكتبة الأمان (إصلاح ترميز Unicode بـ /utf-8)
    if(TARGET sad_new_security)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_security)
    endif()
    if(TARGET sad_new_tools)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_tools)
    endif()
    if(TARGET sad_ui_ir)
        target_link_libraries(sad_new_compiler INTERFACE sad_ui_ir)
    endif()
else()
    target_link_libraries(sad_new_compiler
        INTERFACE
            sad_new_frontend
            sad_new_optimizer
            sad_new_bytecode
            sad_new_type_system
            sad_new_semantic
            sad_new_memory
    )
    if(TARGET sad_new_security)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_security)
    endif()
    if(TARGET sad_new_tools)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_tools)
    endif()
    if(TARGET sad_ui_ir)
        target_link_libraries(sad_new_compiler INTERFACE sad_ui_ir)
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

# ============================================================================
# Test Backend Executable (اختبار الخلفية)
# ============================================================================
if(LLVM_FOUND)
    add_executable(test_backend tools/test_backend.cpp)
    target_include_directories(test_backend PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/frontend
        ${CMAKE_CURRENT_SOURCE_DIR}/include/backend/llvm
        ${SAD_SHARED_DIR}/shared/ast/include
        ${SAD_SHARED_DIR}/shared/types/include
        ${SAD_SHARED_DIR}/shared/errors/include
        ${SAD_SHARED_DIR}/shared/lexer/include
        ${LLVM_INCLUDE_DIRS}
    )
    # Link all LLVM static libraries (exclude LLVM-C.lib DLL import)
    file(GLOB LLVM_ALL_LIBS "C:/LLVM/lib/LLVM*.lib")
    list(REMOVE_ITEM LLVM_ALL_LIBS "C:/LLVM/lib/LLVM-C.lib")
    target_link_libraries(test_backend PRIVATE
        sad_new_llvm_backend
        sad_new_frontend
        sad_new_optimizer
        ${LLVM_ALL_LIBS}
    )
    if(MSVC)
        target_compile_options(test_backend PRIVATE /utf-8 /wd4819)
    endif()
    message(STATUS "Configured test_backend executable")
endif()

# ============================================================================
# Tool Tests (اختبارات الأدوات المدمجة)
# ============================================================================
if(TARGET sad_new_tools)
    # اختبار الحارس الأمني
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_security_scanner.cpp")
        add_executable(test_security_scanner tests/test_security_scanner.cpp)
        target_link_libraries(test_security_scanner PRIVATE sad_new_tools)
        target_include_directories(test_security_scanner PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include
        )
        if(MSVC)
            target_compile_options(test_security_scanner PRIVATE /utf-8 /wd4819 /FS)
        endif()
        set_target_properties(test_security_scanner PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
        message(STATUS "Configured test: test_security_scanner")
    endif()

    # اختبار المنسّق
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_formatter.cpp")
        add_executable(test_formatter tests/test_formatter.cpp)
        target_link_libraries(test_formatter PRIVATE sad_new_tools)
        target_include_directories(test_formatter PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include
        )
        if(MSVC)
            target_compile_options(test_formatter PRIVATE /utf-8 /wd4819 /FS)
        endif()
        set_target_properties(test_formatter PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
        message(STATUS "Configured test: test_formatter")
    endif()

    # اختبار مولّد التوثيق
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_docs_emitter.cpp")
        add_executable(test_docs_emitter tests/test_docs_emitter.cpp)
        target_link_libraries(test_docs_emitter PRIVATE sad_new_tools)
        target_include_directories(test_docs_emitter PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include
        )
        if(MSVC)
            target_compile_options(test_docs_emitter PRIVATE /utf-8 /wd4819 /FS)
        endif()
        set_target_properties(test_docs_emitter PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
        message(STATUS "Configured test: test_docs_emitter")
    endif()

    # اختبار نظام البناء
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_build_system.cpp")
        add_executable(test_build_system tests/test_build_system.cpp)
        target_link_libraries(test_build_system PRIVATE sad_new_tools)
        target_include_directories(test_build_system PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include
        )
        if(MSVC)
            target_compile_options(test_build_system PRIVATE /utf-8 /wd4819 /FS)
        endif()
        set_target_properties(test_build_system PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/tests)
        message(STATUS "Configured test: test_build_system")
    endif()
endif()

# ============================================================================
# SIR Extended Library (Phase 95: تحسينات SIR الشاملة)
# ============================================================================
set(SIR_EXTENDED_SOURCES
    src/sir/sir_validator.cpp
    src/sir/sir_to_llvm_v2.cpp
)

# التحقق من وجود الملفات
set(SIR_VERIFIED_SOURCES "")
foreach(src ${SIR_EXTENDED_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
        list(APPEND SIR_VERIFIED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    else()
        message(STATUS "  [sir_ext] Skipped missing: ${src}")
    endif()
endforeach()

if(SIR_VERIFIED_SOURCES)
    add_library(sad_sir_extended STATIC ${SIR_VERIFIED_SOURCES})
    target_include_directories(sad_sir_extended
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sir
            ${CMAKE_SOURCE_DIR}/include
            ${SAD_SHARED_DIR}/shared/ast/include
            ${SAD_SHARED_DIR}/shared/types/include
            ${SAD_SHARED_DIR}/shared/errors/include
    )
    if(MSVC)
        target_compile_options(sad_sir_extended PRIVATE /utf-8 /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_sir_extended library (validator, llvm_v2)")
endif()

# ============================================================================
# Compiler Security Library (Phase 95: 200+ إصلاحات أمنية)
# ============================================================================
set(SECURITY_EXT_SOURCES
    src/security/compiler_security.cpp
)

set(SECURITY_EXT_VERIFIED "")
foreach(src ${SECURITY_EXT_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
        list(APPEND SECURITY_EXT_VERIFIED "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    endif()
endforeach()

if(SECURITY_EXT_VERIFIED)
    add_library(sad_compiler_security STATIC ${SECURITY_EXT_VERIFIED})
    target_include_directories(sad_compiler_security
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include
    )
    if(MSVC)
        target_compile_options(sad_compiler_security PRIVATE /utf-8 /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_compiler_security library (200+ security fixes)")
endif()

# ============================================================================
# Graphics Backend Support (Phase 95: دعم الرسومات في sadc)
# ============================================================================
set(GRAPHICS_BACKEND_SOURCES
    src/backend/sadc_graphics_support.cpp
)

set(GRAPHICS_BACKEND_VERIFIED "")
foreach(src ${GRAPHICS_BACKEND_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
        list(APPEND GRAPHICS_BACKEND_VERIFIED "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    endif()
endforeach()

if(GRAPHICS_BACKEND_VERIFIED)
    add_library(sad_graphics_backend STATIC ${GRAPHICS_BACKEND_VERIFIED})
    target_include_directories(sad_graphics_backend
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/include
            ${SAD_SHARED_DIR}/shared/types/include
    )
    if(MSVC)
        target_compile_options(sad_graphics_backend PRIVATE /utf-8 /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_graphics_backend library (sadc graphics support)")
endif()

# ============================================================================
# Runtime Library (مكتبة Runtime للرسومات)
# ============================================================================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/runtime/CMakeLists.txt")
    add_subdirectory(runtime)
    message(STATUS "✓ تم تفعيل مجلد runtime للرسومات")
endif()
