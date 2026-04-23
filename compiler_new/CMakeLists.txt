# ============================================================================
# Compiler - ״§„…״×״±״¬…
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
    # „״§ †״«״¨‘״× LLVM_DIR ‡†״§ - ״£״× …† cmake/llvm.cmake ״£ˆ …† ״³״·״± ״§„״£ˆ״§…״±
    find_package(LLVM QUIET CONFIG)
endif()

if(LLVM_FOUND)
    message(STATUS "compiler_new: Found LLVM ${LLVM_PACKAGE_VERSION}")
    # (AR) ملاحظة: لا نستخدم include_directories هنا — نستخدم SAD_LLVM_INCLUDES
    #      في كل هدف يستخدم LLVM headers لدعم Debug/Release ثنائي الإعداد.
    # (EN) Note: Don't use include_directories here — use SAD_LLVM_INCLUDES
    #      in each target that uses LLVM headers for dual Debug/Release support.
    add_definitions(${LLVM_DEFINITIONS})
else()
    message(WARNING "compiler_new: LLVM not found - some features disabled")
endif()

# ============================================================================
# Frontend Library (SIR)
# ============================================================================
file(GLOB FRONTEND_SOURCES "src/frontend/*.cpp")
file(GLOB FRONTEND_HEADERS "include/frontend/*.h")

# (AR) ״§״³״×״¨״¹״§״¯ ״§„…„ ״§„‚״¯… sir_builder.cpp „״×״¬†״¨ ״§„״×ƒ״±״§״± …״¹ ״§„…„״§״× ״§„…‚״³…״©
# (EN) Exclude legacy sir_builder.cpp to avoid duplication with split files
list(FILTER FRONTEND_SOURCES EXCLUDE REGEX "sir_builder\\.cpp$")

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
        ${SAD_SHARED_DIR}/shared/builtins/include
)

# ״¥״µ„״§״­ ״×״­״°״±״§״× UTF-8 / Fix UTF-8 encoding warnings for frontend
if(MSVC)
    target_compile_options(sad_new_frontend PRIVATE /utf-8 /FS)
endif()

# ============================================================================
# Middle Library (Optimizer)
# FIXED: API updated to use new SIRInstruction API (January 2026)
# ============================================================================
file(GLOB MIDDLE_SOURCES "src/middle/*.cpp")
# (AR) ״×… ״¥״¹״§״¯״© ״×״¶…† async_transform.cpp ״¨״¹״¯ ״¥״µ„״§״­ ״§„״£״®״·״§״¡
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

# …-״¨03: ״¥״µ„״§״­ PDB locking ״¨״¥״¶״§״© /FS
if(MSVC)
    target_compile_options(sad_new_optimizer PRIVATE /FS /utf-8)
endif()

# ============================================================================
# Backend Library (LLVM)
# ============================================================================
if(LLVM_FOUND)
    file(GLOB LLVM_BACKEND_SOURCES "src/backend/llvm/*.cpp")
    file(GLOB LLVM_BACKEND_HEADERS "include/backend/llvm/*.h")
    
    # …-״¨01: ״¥״¶״§״© …„״§״× backend ״÷״±-LLVM ״§„…״·„ˆ״¨״© „†״¸״§… ״§„†״­„״©
    # ״×… ״¥״µ„״§״­ ״£״®״·״§״¡ ״§„״×״±״¬…״©  ״§„…„״§״× ״§„״×״§„״©:
    # - interrupt_handlers.cpp: ״¥״µ„״§״­ ״×״¹״§״±״¶ …״§ƒ״±ˆ״§״× Windows (BREAKPOINT/OVERFLOW)
    # - solidity_ffi.cpp: ״¥״¶״§״© <iomanip> ˆ <functional>
    # - pattern_codegen.cpp: ״¥״µ„״§״­ ״£״³…״§״¡ enum (NUMBERג†’DOUBLE, NULL_TYPEג†’VOID)
    #
    # ״§„…„״§״× ״§„״×״§„״© ״×״­״×״§״¬ ״¥״µ„״§״­״§״× ״£״¹…‚ ˆ״×… ״×״£״¬„‡״§:
    # - async_codegen.cpp: ״£״®״·״§״¡ …״×״¹״¯״¯״©  ״×״¹״±״§״× ״§„״£†ˆ״§״¹
    # - ffi_wrapper.cpp: ״£״®״·״§״¡  c_abi.h ˆ c_types.h headers
    # - inline_asm.cpp: ״£״®״·״§״¡  IRBuilder template
    # - llvm_compiler_pipeline.cpp: …״³״§״±״§״× include ״÷״± ״µ״­״­״©  ״§„‡״¯״±
    set(BACKEND_COMMON_SOURCES
        src/backend/entry_point.cpp
        src/backend/linker_script.cpp
        src/backend/linker_script_x86_template.cpp
        src/backend/panic_handler.cpp
        src/backend/c_abi.cpp
        src/backend/c_abi_codegen.cpp
        src/backend/interrupt_handlers.cpp
        src/backend/solidity_ffi.cpp
        src/backend/pattern_codegen.cpp
        src/backend/pattern_codegen_api.cpp
        src/backend/async_codegen.cpp
        src/backend/ffi_wrapper.cpp
        src/backend/inline_asm.cpp
        src/backend/arm64_codegen.cpp
        src/backend/compose_emitter.cpp
        src/backend/evm_codegen.cpp
        src/backend/extension_codegen.cpp
        src/backend/html_emitter.cpp
        src/backend/pipeline_chain.cpp
        src/backend/pipeline_chain_ops.cpp
        src/backend/pipeline_chain_fusion.cpp
        src/backend/pipeline_codegen.cpp
        src/backend/wasm_ast_visitor.cpp
        src/backend/wasm_bindgen.cpp
        src/backend/wasm_codegen.cpp
        src/backend/wasm_optimizer.cpp
        # (AR) …„״§״× wasm_direct ˆwasm32_target …״₪״¬„״© ג€” ״×״¹״×…״¯ ״¹„‰ ASTNode API …‚ˆ״¯
        # (EN) wasm_direct and wasm32_target deferred ג€” depend on missing ASTNode API
    )
    
    # …-״¨02: ״¥״¶״§״© …„״§״× pipeline
    set(PIPELINE_SOURCES
        src/pipeline/no_std_mode.cpp
        src/pipeline/no_std_integration.cpp
        src/pipeline/llvm_compiler_pipeline.cpp
        src/pipeline/freestanding_codegen_new.cpp
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
            ${CMAKE_CURRENT_SOURCE_DIR}/src/backend
            ${CMAKE_CURRENT_SOURCE_DIR}/src/backend/wasm_direct
            ${CMAKE_SOURCE_DIR}
            ${SAD_LLVM_INCLUDES}
    )

    # ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    # (AR) …ƒˆ†״§״× LLVM ״§„…״·„ˆ״¨״©:
    #   - X86: „„…״¶ (ˆ†״¯ˆ״²/„†ƒ״³ x86_64)
    #   - AArch64: „״£†״¯״±ˆ״¯ ARM64 ˆ macOS Apple Silicon
    #   - ARM: „״£†״¯״±ˆ״¯ ARM 32-bit (armeabi-v7a)
    #   - WebAssembly: „״×״·״¨‚״§״× ״§„ˆ״¨
    #   - native: „„…״¶ ״§„״­״§„
    # ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•ג•
    llvm_map_components_to_libnames(llvm_libs
        Core
        Support
        IRReader
        Passes
        ExecutionEngine
        OrcJIT
        MCJIT
        # X86 ג€” ״§„…״¶
        X86CodeGen
        X86AsmParser
        X86Desc
        X86Info
        # AArch64 ג€” ״£†״¯״±ˆ״¯ ARM64 + macOS Apple Silicon
        AArch64CodeGen
        AArch64AsmParser
        AArch64Desc
        AArch64Info
        # ARM ג€” ״£†״¯״±ˆ״¯ 32-bit
        ARMCodeGen
        ARMAsmParser
        ARMDesc
        ARMInfo
        # WebAssembly
        WebAssemblyCodeGen
        WebAssemblyAsmParser
        WebAssemblyDesc
        WebAssemblyInfo
        native
    )

    target_link_libraries(sad_new_llvm_backend
        PUBLIC
            ${llvm_libs}
            sad_new_frontend
            sad_new_optimizer
    )
    
    # …-״¨01: ״¥״µ„״§״­ ״×״±…״² Unicode „„…„״§״× ״§„״× ״×״­״×ˆ ״¹„‰ …״¹״±‘״§״× ״¹״±״¨״©
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
        src/jit/deoptimization.cpp
        src/jit/jit_trigger.cpp
        src/jit/tier_transition.cpp
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
            ${SAD_LLVM_INCLUDES}
    )
    target_link_libraries(sad_new_jit PUBLIC sad_new_llvm_backend ${llvm_libs})
endif()

# ============================================================================
# Bytecode Library (״×†״³‚ …„״§״× ״§„״¨״§״× ƒˆ״¯)
# ============================================================================
# (AR) ״§„…„״§״× ״§„…״³״×‚„״© (format): decoder, encoder, constant_pool, symbol_table, magic, version, validator, opcodes
# (EN) Standalone format files: decoder, encoder, constant_pool, symbol_table, magic, version, validator, opcodes
# (AR) …„״§״× SIR ״§„…״¹״×…״¯״© (emitter.cpp, bytecode.cpp) …״₪״¬„״© - ״×״­״×״§״¬ ״×״­״¯״« SIR API
# (EN) SIR-dependent files (emitter.cpp, bytecode.cpp) deferred - need SIR API update
file(GLOB BYTECODE_SOURCES "src/bytecode/*.cpp")
file(GLOB BYTECODE_HEADERS "include/bytecode/*.h")

# ״§״³״×״¨״¹״§״¯ ״§„…„״§״× ״§„…״¹״×…״¯״© ״¹„‰ SIR API ״§„‚״¯…
list(FILTER BYTECODE_SOURCES EXCLUDE REGEX "emitter\\.cpp")
list(FILTER BYTECODE_SOURCES EXCLUDE REGEX "emitter_optimize\\.cpp")
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
# Semantic Analysis Library (†״¸״§… ״§„…„ƒ״© ˆ״§„״§״³״×״¹״§״±״©)
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
file(GLOB SECURITY_DIAG_SOURCES "src/diagnostics/*.cpp")
file(GLOB SECURITY_SECURITY_SOURCES "src/security/compiler_security.cpp")

set(SECURITY_ALL_SOURCES ${SECURITY_CODEGEN_SOURCES} ${SECURITY_SEMA_SOURCES} ${SECURITY_DIAG_SOURCES} ${SECURITY_SECURITY_SOURCES})

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
# Memory System Library (†״¸״§… ״§„״°״§ƒ״±״© ״§„״°ƒ)
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

# (AR) c_types.cpp ״¹״×…״¯ ״¹„‰ ״£†ˆ״§״¹ FFI ״÷״± …ƒ״×…„״© ״§„״×״¹״± (CStructType, CUnionType, CEnumType, CTypedefType)
#      ˆ״­״×״§״¬ LLVM headers ״¹״¨״± c_abi.h - ״³״¹‘„ ״¹†״¯ ״§ƒ״×…״§„ ״×״¹״± ‡״°‡ ״§„״£†ˆ״§״¹
# (EN) c_types.cpp depends on incomplete FFI types (CStructType, CUnionType, CEnumType, CTypedefType)
#      and needs LLVM headers via c_abi.h - will be enabled when these types are fully defined
list(FILTER TYPE_SYSTEM_SOURCES EXCLUDE REGEX "c_types.*\\.cpp")

add_library(sad_new_type_system STATIC ${TYPE_SYSTEM_SOURCES})

# ״×״¹״·„ ״×״­״°״±״§״× Unicode „„†״µˆ״µ ״§„״¹״±״¨״© / Disable Unicode warnings for Arabic text
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
# Tools Library (״§„״£״¯ˆ״§״× ״§„…״¯…״¬״©: ״§„…†״³‘‚״ ״§„״­״§״±״³ ״§„״£…†״ ״§„״×ˆ״«‚״ ״§„״¨†״§״¡)
# Merged tools: formatter, security scanner, docs emitter, build system
# ============================================================================
set(TOOLS_SOURCES
    src/format/sad_formatter.cpp
    src/format/sad_formatter_rebuild.cpp
    src/security/security_scanner.cpp
    src/security/security_scanner_analysis.cpp
    src/security/taint_tracker.cpp
    src/meta/docs_emitter.cpp
    src/meta/docs_emitter_html.cpp
    src/build/build_system.cpp
)

# ״¥״¶״§״© …„״§״× …ˆ״¬ˆ״¯״© ״¹„״§‹
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
    message(STATUS "  [tools] No verified source files ג€” sad_new_tools skipped")
endif()

# ============================================================================
# SadUI IR Library (†״¸״§… ״§„ˆ״§״¬‡״§״× ״§„…ˆ״­״¯ ״¹״¨״± ״§„…†״µ״§״×)
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
    # (AR) ״×†״° UIParser ״§„…״·„ˆ״¨ …† sad_ui_pipeline.cpp
    # (EN) UIParser implementation required by sad_ui_pipeline.cpp
    ${SAD_SHARED_DIR}/shared/parser/src/specs/ui/parser_ui_maps.cpp
)

# ״§„״×״­‚‚ …† ˆ״¬ˆ״¯ ״§„…„״§״×
set(UI_IR_VERIFIED_SOURCES "")
foreach(src ${UI_IR_SOURCES})
    if(IS_ABSOLUTE "${src}")
        if(EXISTS "${src}")
            list(APPEND UI_IR_VERIFIED_SOURCES "${src}")
        else()
            message(STATUS "  [ui_ir] Skipped missing: ${src}")
        endif()
    else()
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
            list(APPEND UI_IR_VERIFIED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
        else()
            message(STATUS "  [ui_ir] Skipped missing: ${src}")
        endif()
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
    message(STATUS "  [ui_ir] No source files found ג€” sad_ui_ir skipped")
endif()

# ============================================================================
# Borrow Checker / Lifetime Library (״×״­„„ ״§„״£״¹…״§״± ˆ״§„״§״³״×״¹״§״±״©)
# Phase 20: T204-T206 (Lifetime Annotations)
# ============================================================================
file(GLOB BORROW_SOURCES "src/borrow/*.cpp")

if(BORROW_SOURCES)
    add_library(sad_new_borrow STATIC ${BORROW_SOURCES})
    target_include_directories(sad_new_borrow
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/borrow
            ${CMAKE_SOURCE_DIR}/include
            ${SAD_SHARED_DIR}/shared/ast/include
            ${SAD_SHARED_DIR}/shared/types/include
            ${SAD_SHARED_DIR}/shared/errors/include
    )
    if(MSVC)
        target_compile_options(sad_new_borrow PRIVATE /utf-8 /FS)
    endif()
    message(STATUS "Configured sad_new_borrow library (lifetime analysis)")
endif()

# ============================================================================
# Cross-Compilation Targets Library (״£‡״¯״§ ״§„״×״±״¬…״© ״§„…״×‚״§״·״¹״©)
# ============================================================================
file(GLOB TARGETS_SOURCES "src/targets/*.cpp")

if(TARGETS_SOURCES)
    add_library(sad_new_targets STATIC ${TARGETS_SOURCES})
    target_include_directories(sad_new_targets
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/targets
            ${CMAKE_SOURCE_DIR}/include
            ${SAD_SHARED_DIR}/shared/ast/include
            ${SAD_SHARED_DIR}/shared/types/include
            ${SAD_SHARED_DIR}/shared/errors/include
    )
    if(MSVC)
        target_compile_options(sad_new_targets PRIVATE /utf-8 /FS /std:c++20)
    endif()
    message(STATUS "Configured sad_new_targets library (cross-compilation)")
endif()

# ============================================================================
# Advanced Optimizer Library (…״­״³† …״×‚״¯… „״´״¬״±״© AST)
# ============================================================================
# (AR) optimization_pass.cpp ˆ advanced_optimizer.cpp „״§ ״¹״×…״¯״§† ״¹„‰ LLVM
# (AR) arabic_passes.cpp ״¹״×…״¯ ״¹„‰ LLVM ג€” ״¶״§ ‚״· ״¹†״¯ ״×ˆ״± LLVM
set(ADV_OPT_SOURCES
    src/optimizer/optimization_pass.cpp
    src/optimizer/advanced_optimizer.cpp
)
if(LLVM_FOUND)
    list(APPEND ADV_OPT_SOURCES src/optimizer/arabic_passes.cpp)
endif()

set(ADV_OPT_VERIFIED "")
foreach(src ${ADV_OPT_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
        list(APPEND ADV_OPT_VERIFIED "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    else()
        message(STATUS "  [adv_opt] Skipped missing: ${src}")
    endif()
endforeach()

if(ADV_OPT_VERIFIED)
    add_library(sad_new_advanced_optimizer STATIC ${ADV_OPT_VERIFIED})
    target_include_directories(sad_new_advanced_optimizer
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/optimizer
            ${CMAKE_SOURCE_DIR}/include
            ${SAD_SHARED_DIR}/shared/ast/include
            ${SAD_SHARED_DIR}/shared/types/include
            ${SAD_SHARED_DIR}/shared/errors/include
            ${SAD_SHARED_DIR}/shared/lexer/include
    )
    if(LLVM_FOUND)
        target_include_directories(sad_new_advanced_optimizer PUBLIC ${SAD_LLVM_INCLUDES})
    endif()
    if(MSVC)
        target_compile_options(sad_new_advanced_optimizer PRIVATE /utf-8 /FS)
    endif()
    message(STATUS "Configured sad_new_advanced_optimizer library")
endif()

# ============================================================================
# FFI Library (ˆ״§״¬‡״© ״§״³״×״¯״¹״§״¡ ״§„״¯ˆ״§„ ״§„״®״§״±״¬״©)
# ============================================================================
set(FFI_SOURCES
    src/ffi/c_api_bridge.cpp
    src/ffi/ownership_annotations.cpp
)

set(FFI_VERIFIED "")
foreach(src ${FFI_SOURCES})
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
        list(APPEND FFI_VERIFIED "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
    else()
        message(STATUS "  [ffi] Skipped missing: ${src}")
    endif()
endforeach()

if(FFI_VERIFIED)
    add_library(sad_new_ffi STATIC ${FFI_VERIFIED})
    target_include_directories(sad_new_ffi
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/ffi
            ${CMAKE_SOURCE_DIR}/include
            ${SAD_SHARED_DIR}/shared/ast/include
            ${SAD_SHARED_DIR}/shared/types/include
            ${SAD_SHARED_DIR}/shared/errors/include
    )
    if(MSVC)
        target_compile_options(sad_new_ffi PRIVATE /utf-8 /FS)
    endif()
    message(STATUS "Configured sad_new_ffi library (C API bridge, ownership annotations)")
endif()

# ============================================================================
# LLVM Abstraction Library (״·״¨‚״© ״×״¬״±״¯ LLVM)
# Phase 91: T1030
# ============================================================================
if(LLVM_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/abstraction/llvm_wrapper.cpp")
    add_library(sad_new_abstraction STATIC src/abstraction/llvm_wrapper.cpp)
    target_include_directories(sad_new_abstraction
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/src/abstraction
            ${CMAKE_SOURCE_DIR}/include
            ${SAD_LLVM_INCLUDES}
    )
    target_compile_definitions(sad_new_abstraction PRIVATE SAD_USE_LLVM=1)
    if(MSVC)
        target_compile_options(sad_new_abstraction PRIVATE /utf-8 /FS)
    endif()
    target_link_libraries(sad_new_abstraction PUBLIC ${llvm_libs})
    message(STATUS "Configured sad_new_abstraction library (LLVM wrapper)")
endif()

# ============================================================================
# Compile-Time Evaluation Library (״×‚… ˆ‚״× ״§„״×״±״¬…״©)
# ============================================================================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/comptime/comptime.cpp")
    add_library(sad_new_comptime STATIC src/comptime/comptime.cpp)
    target_include_directories(sad_new_comptime
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}/include
            ${CMAKE_CURRENT_SOURCE_DIR}/include/comptime
            ${CMAKE_SOURCE_DIR}/include
    )
    if(MSVC)
        target_compile_options(sad_new_comptime PRIVATE /utf-8 /FS)
    endif()
    message(STATUS "Configured sad_new_comptime library (compile-time evaluation)")
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
    # …-״¨04: ״×״¹„ …ƒ״×״¨״© ״§„״£…״§† (״¥״µ„״§״­ ״×״±…״² Unicode ״¨€ /utf-8)
    if(TARGET sad_new_security)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_security)
    endif()
    if(TARGET sad_new_tools)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_tools)
    endif()
    if(TARGET sad_ui_ir)
        target_link_libraries(sad_new_compiler INTERFACE sad_ui_ir)
    endif()
    # ״§„…ƒ״×״¨״§״× ״§„״¬״¯״¯״© ״§„…״¯…״¬״© (Phase 96)
    if(TARGET sad_new_borrow)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_borrow)
    endif()
    if(TARGET sad_new_targets)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_targets)
    endif()
    if(TARGET sad_new_advanced_optimizer)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_advanced_optimizer)
    endif()
    if(TARGET sad_new_ffi)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_ffi)
    endif()
    if(TARGET sad_new_abstraction)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_abstraction)
    endif()
    if(TARGET sad_new_comptime)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_comptime)
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
    # ״§„…ƒ״×״¨״§״× ״§„״¬״¯״¯״© ״§„…״¯…״¬״© (Phase 96) ג€” ״¨״¯ˆ† LLVM
    if(TARGET sad_new_borrow)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_borrow)
    endif()
    if(TARGET sad_new_targets)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_targets)
    endif()
    if(TARGET sad_new_advanced_optimizer)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_advanced_optimizer)
    endif()
    if(TARGET sad_new_ffi)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_ffi)
    endif()
    if(TARGET sad_new_comptime)
        target_link_libraries(sad_new_compiler INTERFACE sad_new_comptime)
    endif()
endif()

# „„״×ˆ״§‚ …״¹ ״§„ƒˆ״¯ ״§„‚״¯…
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
# Test Backend Executable (״§״®״×״¨״§״± ״§„״®„״©)
# ============================================================================
# (AR) ״×״·„״¨ LLVM ג€” ״¹†״¯ ״×ˆ״± Debug + Release ״¨†‰  ƒ„״§ ״§„ˆ״¶״¹†
#      ˆ״¥„״§ ״¨†‰  Release ‚״· (EXCLUDE_FROM_ALL)
# (EN) Requires LLVM ג€” when both Debug + Release available, builds in both configs
#      Otherwise Release-only (EXCLUDE_FROM_ALL)
if(LLVM_FOUND)
    if(SAD_LLVM_HAS_DEBUG)
        add_executable(test_backend tools/test_backend.cpp)
        message(STATUS "Configured test_backend executable (dual-config: Debug + Release)")
    else()
        add_executable(test_backend EXCLUDE_FROM_ALL tools/test_backend.cpp)
        message(STATUS "Configured test_backend executable (EXCLUDE_FROM_ALL ג€” Release only)")
    endif()
    target_include_directories(test_backend PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/frontend
        ${CMAKE_CURRENT_SOURCE_DIR}/include/backend/llvm
        ${SAD_SHARED_DIR}/shared/ast/include
        ${SAD_SHARED_DIR}/shared/types/include
        ${SAD_SHARED_DIR}/shared/errors/include
        ${SAD_SHARED_DIR}/shared/lexer/include
        ${SAD_LLVM_INCLUDES}
    )
    # (AR) ״±״¨״· …ƒ״×״¨״§״× LLVM ״¹״¨״± ״§„…״×״÷״± ״§„״¹״§… ״§„…״­״³ˆ״¨ …† cmake/llvm.cmake
    # (EN) Link LLVM libs via the global variable computed by cmake/llvm.cmake
    target_link_libraries(test_backend PRIVATE
        sad_new_llvm_backend
        sad_new_frontend
        sad_new_optimizer
        ${LLVM_LIBS}
    )
    if(MSVC)
        target_compile_options(test_backend PRIVATE /utf-8 /wd4819)
    endif()
endif()

# ============================================================================
# Tool Tests (״§״®״×״¨״§״±״§״× ״§„״£״¯ˆ״§״× ״§„…״¯…״¬״©)
# ============================================================================
if(TARGET sad_new_tools)
    # ״§״®״×״¨״§״± ״§„״­״§״±״³ ״§„״£…†
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

    # ״§״®״×״¨״§״± ״§„…†״³‘‚
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

    # ״§״®״×״¨״§״± …ˆ„‘״¯ ״§„״×ˆ״«‚
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

    # ״§״®״×״¨״§״± †״¸״§… ״§„״¨†״§״¡
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
# SIR Extended Library (Phase 95+: SIR Core ג€” ״¬…״¹ …„״§״× SIR)
# ============================================================================
set(SIR_EXTENDED_SOURCES
    src/sir/sir_validator.cpp
    src/sir/ast_to_sir.cpp
    src/sir/builtin_intrinsics.cpp
    src/sir/ownership_analysis.cpp
    src/sir/sir_borrow_check.cpp
    src/sir/sir_optimizer.cpp
    src/sir/sir_printer.cpp
    src/sir/sir_to_llvm.cpp
    # (AR) sir_to_llvm_v2.cpp …״₪״¬„ ג€” ״¹״×…״¯ ״¹„‰ TypeKind::Pointer/Widget ˆ compiler::instance ״§„…‚ˆ״¯״©
    # (EN) sir_to_llvm_v2.cpp deferred ג€” depends on missing TypeKind::Pointer/Widget and compiler::instance
)

# ״§„״×״­‚‚ …† ˆ״¬ˆ״¯ ״§„…„״§״×
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
            ${CMAKE_CURRENT_SOURCE_DIR}/include/frontend
            ${CMAKE_CURRENT_SOURCE_DIR}/src/sir
            ${CMAKE_SOURCE_DIR}/include
            ${SAD_SHARED_DIR}/shared/ast/include
            ${SAD_SHARED_DIR}/shared/types/include
            ${SAD_SHARED_DIR}/shared/errors/include
            ${SAD_SHARED_DIR}/shared/lexer/include
    )
    if(LLVM_FOUND)
        target_include_directories(sad_sir_extended PUBLIC ${SAD_LLVM_INCLUDES})
        target_link_libraries(sad_sir_extended PUBLIC sad_new_frontend)
    endif()
    if(MSVC)
        target_compile_options(sad_sir_extended PRIVATE /utf-8 /wd4819 /FS)
    endif()
    message(STATUS "Configured sad_sir_extended library (SIR core: ${SIR_VERIFIED_SOURCES})")
endif()

# ============================================================================
# Compiler Security Library (Phase 95: 200+ ״¥״µ„״§״­״§״× ״£…†״©)
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
# Graphics Backend Support (Phase 95: ״¯״¹… ״§„״±״³ˆ…״§״×  sadc)
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
# Runtime Library (…ƒ״×״¨״© Runtime „„״±״³ˆ…״§״×)
# ============================================================================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/runtime/CMakeLists.txt")
    add_subdirectory(runtime)
    message(STATUS "ג“ ״×… ״×״¹„ …״¬„״¯ runtime „„״±״³ˆ…״§״×")
endif()
