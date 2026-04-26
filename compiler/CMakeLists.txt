# ============================================================================
# Compiler — المترجم
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
 # (AR) لا نُثبّت LLVM_DIR هنا — يأتي من cmake/llvm.cmake أو من سطر الأوامر
 find_package(LLVM QUIET CONFIG)
endif()

if(LLVM_FOUND)
 message(STATUS "compiler_new: Found LLVM ${LLVM_PACKAGE_VERSION}")
 # (AR) ملاحظة: لا نستخدم include_directories هنا — نستخدم SAD_LLVM_INCLUDES
 # في كل هدف يستخدم LLVM headers لدعم Debug/Release ثنائي الإعداد.
 # (EN) Note: Don't use include_directories here — use SAD_LLVM_INCLUDES
 # in each target that uses LLVM headers for dual Debug/Release support.
 add_definitions(${LLVM_DEFINITIONS})
else()
 message(WARNING "compiler_new: LLVM not found - some features disabled")
endif()

# ============================================================================
# Frontend Library (SIR)
# ============================================================================
file(GLOB FRONTEND_SOURCES "src/frontend/*.cpp")
file(GLOB FRONTEND_HEADERS "include/frontend/*.h")

# (AR) استبعاد الملف القديم sir_builder.cpp لتجنب التكرار مع الملفات المُقسَّمة
# (EN) Exclude legacy sir_builder.cpp to avoid duplication with split files
list(FILTER FRONTEND_SOURCES EXCLUDE REGEX "sir_builder\\.cpp$")

add_library(sad_frontend STATIC ${FRONTEND_SOURCES})

target_include_directories(sad_frontend
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
 # (AR) Phase 4 (F-03): إضافة utils لتجنب مسارات #include النسبية
 # (EN) Phase 4 (F-03): add utils to avoid relative #include workarounds
 ${SAD_SHARED_DIR}/shared/utils/include
 ${SAD_SHARED_DIR}/shared/hot_reload/include
)

# (AR) إصلاح تحذيرات UTF-8 / Fix UTF-8 encoding warnings for frontend
if(MSVC)
 target_compile_options(sad_frontend PRIVATE /utf-8 /FS)
endif()

# ============================================================================
# Middle Library (Optimizer)
# FIXED: API updated to use new SIRInstruction API (January 2026)
# ============================================================================
file(GLOB MIDDLE_SOURCES "src/middle/*.cpp")
# (AR) تم إعادة تضمين async_transform.cpp بعد إصلاح الأخطاء
# (EN) Re-included async_transform.cpp after fixing errors
file(GLOB MIDDLE_HEADERS "include/middle/*.h")
add_library(sad_optimizer STATIC ${MIDDLE_SOURCES})

target_include_directories(sad_optimizer
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

target_link_libraries(sad_optimizer PUBLIC sad_frontend)

# (AR) م-ب03: إصلاح PDB locking بإضافة /FS
if(MSVC)
 target_compile_options(sad_optimizer PRIVATE /FS /utf-8)
endif()

# ============================================================================
# Backend Library (LLVM)
# ============================================================================
if(LLVM_FOUND)
 file(GLOB LLVM_BACKEND_SOURCES "src/backend/llvm/*.cpp")
 file(GLOB LLVM_BACKEND_HEADERS "include/backend/llvm/*.h")
 
 # - 01: backend -LLVM
 # :
 # - interrupt_handlers.cpp: Windows (BREAKPOINT/OVERFLOW)
 # - solidity_ffi.cpp: <iomanip> <functional>
 # - pattern_codegen.cpp: enum (NUMBER DOUBLE, NULL_TYPE VOID)
 #
 # :
 # - async_codegen.cpp:
 # - ffi_wrapper.cpp: c_abi.h c_types.h headers
 # - inline_asm.cpp: £®·§¡  IRBuilder template
 # - llvm_compiler_pipeline.cpp: include
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
 # (AR) evm_codegen.cpp مُؤرشف في archived/experimental_types/ — معزول، لم يُربط بـ AST nodes
 # (EN) evm_codegen.cpp archived in archived/experimental_types/ — isolated, never linked to AST nodes
 src/backend/extension_codegen.cpp
 src/backend/html_emitter.cpp
 src/backend/pipeline_chain.cpp
 src/backend/pipeline_chain_ops.cpp
 src/backend/pipeline_chain_fusion.cpp
 src/backend/pipeline_codegen.cpp
 # (AR) جميع مسارات WASM مؤرشفة في archived/wasm_dead_paths/
 # (EN) All WASM paths archived in archived/wasm_dead_paths/
 # المسار الحي الوحيد لتشغيل لغة ص في المتصفح: cmake/wasm.cmake (Emscripten → sad_wasm)
 )
 
 # - 02: pipeline
 set(PIPELINE_SOURCES
 src/pipeline/no_std_mode.cpp
 src/pipeline/no_std_integration.cpp
 src/pipeline/llvm_compiler_pipeline.cpp
 src/pipeline/freestanding_codegen_new.cpp
 )

 add_library(sad_llvm_backend STATIC 
 ${LLVM_BACKEND_SOURCES}
 ${BACKEND_COMMON_SOURCES}
 ${PIPELINE_SOURCES}
 )

 target_include_directories(sad_llvm_backend
 PUBLIC
 ${CMAKE_CURRENT_SOURCE_DIR}/include
 ${CMAKE_CURRENT_SOURCE_DIR}/include/backend/llvm
 ${CMAKE_CURRENT_SOURCE_DIR}/include/backend
 ${CMAKE_CURRENT_SOURCE_DIR}/include/pipeline
 ${CMAKE_CURRENT_SOURCE_DIR}/src/backend
 ${CMAKE_SOURCE_DIR}
 ${SAD_LLVM_INCLUDES}
 )

 #
 # (AR) LLVM :
 # - X86: ( / x86_64)
 # - AArch64: ARM64 macOS Apple Silicon
 # - ARM: ARM 32-bit (armeabi-v7a)
 # - WebAssembly:
 # - native:
 #
 llvm_map_components_to_libnames(llvm_libs
 Core
 Support
 IRReader
 Passes
 ExecutionEngine
 OrcJIT
 MCJIT
 # X86 — المضيف
 X86CodeGen
 X86AsmParser
 X86Desc
 X86Info
 # AArch64 — أندرويد ARM64 + macOS Apple Silicon
 AArch64CodeGen
 AArch64AsmParser
 AArch64Desc
 AArch64Info
 # ARM — أندرويد 32-bit
 ARMCodeGen
 ARMAsmParser
 ARMDesc
 ARMInfo
 # (AR) WebAssembly LLVM components مؤرشفة — انظر archived/wasm_dead_paths/
 # (EN) WebAssembly LLVM components archived — see archived/wasm_dead_paths/
 native
 )

 target_link_libraries(sad_llvm_backend
 PUBLIC
 ${llvm_libs}
 sad_frontend
 sad_optimizer
 )
 
 # - 01: Unicode
 if(MSVC)
 target_compile_options(sad_llvm_backend PRIVATE /utf-8 /wd4819 /FS)
 endif()
endif()

# ============================================================================
# JIT Library — مُؤرشَفة في archived/jit_compiler_unused/
# ============================================================================
# (AR) كانت مكتبة sad_jit (9034 سطر) تُبنى لكن لا تُستخدم أبداً:
# - لا تُستدعى من المفسر ولا المترجم ولا أي اختبار
# - lexer/parser/visitor — لا أحد يربطها
# - الواجهات تستخدم forward declarations لـ LLVM ORC ("until LLVM installation completes")
# - مكرَّرة معمارياً مع vm/sad_jit (الذي يعمل فعلاً مع VM bytecode)
# (EN) sad_jit library (9034 lines) was built but never used:
# - Not called by interpreter, compiler, or any test
# - Forward decls only for LLVM ORC ("until LLVM installation completes")
# - Architecturally duplicated with vm/sad_jit (which actually works on VM bytecode)
# انظر archived/jit_compiler_unused/README.md للتفاصيل الكاملة وخطة الإحياء.

# ============================================================================
# Bytecode Library ( )
# ============================================================================
# (AR) الملفات المستقلة (format): decoder, encoder, constant_pool, symbol_table, magic, version, validator, opcodes
# (EN) Standalone format files: decoder, encoder, constant_pool, symbol_table, magic, version, validator, opcodes
# (AR) ملفات SIR المعتمدة (emitter.cpp, bytecode.cpp) مؤجلة — تحتاج تحديث SIR API
# (EN) SIR-dependent files (emitter.cpp, bytecode.cpp) deferred - need SIR API update
# (AR) مُؤرشف: انظر archived/bytecode_format/README.md
# (EN) Archived: see archived/bytecode_format/README.md
# تنسيق توزيع .sadbc لم يُكمَل، 0 استدعاءات داخلية، مكتبة ميتة.

# ============================================================================
# Semantic Analysis Library ( )
# ============================================================================
# (AR) ملاحظة (Phase 3 F-01): type_checker.{h,cpp,*_visitors.cpp} نُقلت إلى
#      shared/semantic/ كمكتبة sad_semantic_shared، لأن المفسر يحتاجها أيضاً.
#      هنا نضم باقي ملفات الفحص الدلالي الخاصة بالمترجم فقط (borrow checker،
#      lifetime analyzer، إلخ) ونربط sad_semantic_shared كـ PUBLIC dep ليظل
#      العملاء قادرين على استعمال type_checker عبر sad_semantic.
# (EN) Note (Phase 3 F-01): type_checker.{h,cpp,*_visitors.cpp} moved to
#      shared/semantic/ as sad_semantic_shared, since the interpreter needs
#      it too. Here we keep only compiler-specific semantic files (borrow
#      checker, lifetime analyzer, etc.) and link sad_semantic_shared as a
#      PUBLIC dep so consumers can still reach type_checker via sad_semantic.
# ============================================================================
file(GLOB SEMANTIC_SOURCES "src/semantic/*.cpp")
file(GLOB SEMANTIC_HEADERS "include/semantic/*.h")

add_library(sad_semantic STATIC ${SEMANTIC_SOURCES})

if(MSVC)
 target_compile_options(sad_semantic PRIVATE /utf-8 /wd4819 /FS)
endif()

target_include_directories(sad_semantic
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

# (AR) sad_semantic_shared يصدر type_checker للعملاء عبر هذه المكتبة.
# (EN) sad_semantic_shared re-exports type_checker for clients via this lib.
target_link_libraries(sad_semantic PUBLIC sad_shared sad_type_system sad_semantic_shared)


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
 add_library(sad_security STATIC ${SECURITY_ALL_SOURCES})
 target_include_directories(sad_security
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
 target_compile_options(sad_security PRIVATE /utf-8 /wd4819 /FS)
 endif()
 message(STATUS "Configured sad_security library (triple guard, permissions, diagnostics)")
endif()

# ============================================================================
# Memory System Library ( )
# ============================================================================
file(GLOB MEMORY_SOURCES "src/memory/*.cpp")
file(GLOB MEMORY_HEADERS "include/memory/*.h")

add_library(sad_memory STATIC ${MEMORY_SOURCES})

if(MSVC)
 target_compile_options(sad_memory PRIVATE /utf-8 /wd4819 /FS)
endif()

target_include_directories(sad_memory
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

# (AR) c_types.cpp FFI (CStructType, CUnionType, CEnumType, CTypedefType)
# LLVM headers c_abi.h -
# (EN) c_types.cpp depends on incomplete FFI types (CStructType, CUnionType, CEnumType, CTypedefType)
# and needs LLVM headers via c_abi.h - will be enabled when these types are fully defined
list(FILTER TYPE_SYSTEM_SOURCES EXCLUDE REGEX "c_types.*\\.cpp")

add_library(sad_type_system STATIC ${TYPE_SYSTEM_SOURCES})

# Unicode / Disable Unicode warnings for Arabic text
if(MSVC)
 target_compile_options(sad_type_system PRIVATE /utf-8 /wd4819 /FS)
endif()

target_include_directories(sad_type_system
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
# Tools Library ( : )
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

#
set(TOOLS_VERIFIED_SOURCES "")
foreach(src ${TOOLS_SOURCES})
 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
 list(APPEND TOOLS_VERIFIED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
 else()
 message(STATUS " [tools] Skipped missing: ${src}")
 endif()
endforeach()

if(TOOLS_VERIFIED_SOURCES)
 add_library(sad_tools STATIC ${TOOLS_VERIFIED_SOURCES})
 target_include_directories(sad_tools
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
 target_compile_options(sad_tools PRIVATE /utf-8 /wd4819 /FS)
 endif()
 message(STATUS "Configured sad_tools library (formatter, guard, docs, build)")
else()
 message(STATUS " [tools] No verified source files €” sad_tools skipped")
endif()

# ============================================================================
# SadUI IR Library ( )
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
 # (AR) UIParser sad_ui_pipeline.cpp
 # (EN) UIParser implementation required by sad_ui_pipeline.cpp
 ${SAD_SHARED_DIR}/shared/parser/src/specs/ui/parser_ui_maps.cpp
)

#
set(UI_IR_VERIFIED_SOURCES "")
foreach(src ${UI_IR_SOURCES})
 if(IS_ABSOLUTE "${src}")
 if(EXISTS "${src}")
 list(APPEND UI_IR_VERIFIED_SOURCES "${src}")
 else()
 message(STATUS " [ui_ir] Skipped missing: ${src}")
 endif()
 else()
 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
 list(APPEND UI_IR_VERIFIED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
 else()
 message(STATUS " [ui_ir] Skipped missing: ${src}")
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
 message(STATUS " [ui_ir] No source files found €” sad_ui_ir skipped")
endif()

# ============================================================================
# Borrow Checker / Lifetime Library ( )
# Phase 20: T204-T206 (Lifetime Annotations)
# ============================================================================
file(GLOB BORROW_SOURCES "src/borrow/*.cpp")

if(BORROW_SOURCES)
 add_library(sad_borrow STATIC ${BORROW_SOURCES})
 target_include_directories(sad_borrow
 PUBLIC
 ${CMAKE_CURRENT_SOURCE_DIR}/include
 ${CMAKE_CURRENT_SOURCE_DIR}/src/borrow
 ${CMAKE_SOURCE_DIR}/include
 ${SAD_SHARED_DIR}/shared/ast/include
 ${SAD_SHARED_DIR}/shared/types/include
 ${SAD_SHARED_DIR}/shared/errors/include
 )
 if(MSVC)
 target_compile_options(sad_borrow PRIVATE /utf-8 /FS)
 endif()
 message(STATUS "Configured sad_borrow library (lifetime analysis)")
endif()

# ============================================================================
# Cross-Compilation Targets Library ( )
# ============================================================================
file(GLOB TARGETS_SOURCES "src/targets/*.cpp")

if(TARGETS_SOURCES)
 add_library(sad_targets STATIC ${TARGETS_SOURCES})
 target_include_directories(sad_targets
 PUBLIC
 ${CMAKE_CURRENT_SOURCE_DIR}/include
 ${CMAKE_CURRENT_SOURCE_DIR}/src/targets
 ${CMAKE_SOURCE_DIR}/include
 ${SAD_SHARED_DIR}/shared/ast/include
 ${SAD_SHARED_DIR}/shared/types/include
 ${SAD_SHARED_DIR}/shared/errors/include
 )
 if(MSVC)
 target_compile_options(sad_targets PRIVATE /utf-8 /FS /std:c++20)
 endif()
 message(STATUS "Configured sad_targets library (cross-compilation)")
endif()

# ============================================================================
# Advanced Optimizer Library ( AST)
# ============================================================================
# (AR) optimization_pass.cpp advanced_optimizer.cpp LLVM
# (AR) arabic_passes.cpp LLVM LLVM
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
 message(STATUS " [adv_opt] Skipped missing: ${src}")
 endif()
endforeach()

if(ADV_OPT_VERIFIED)
 add_library(sad_advanced_optimizer STATIC ${ADV_OPT_VERIFIED})
 target_include_directories(sad_advanced_optimizer
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
 target_include_directories(sad_advanced_optimizer PUBLIC ${SAD_LLVM_INCLUDES})
 endif()
 if(MSVC)
 target_compile_options(sad_advanced_optimizer PRIVATE /utf-8 /FS)
 endif()
 message(STATUS "Configured sad_advanced_optimizer library")
endif()

# ============================================================================
# FFI Library ( )
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
 message(STATUS " [ffi] Skipped missing: ${src}")
 endif()
endforeach()

if(FFI_VERIFIED)
 add_library(sad_ffi STATIC ${FFI_VERIFIED})
 target_include_directories(sad_ffi
 PUBLIC
 ${CMAKE_CURRENT_SOURCE_DIR}/include
 ${CMAKE_CURRENT_SOURCE_DIR}/include/ffi
 ${CMAKE_SOURCE_DIR}/include
 ${SAD_SHARED_DIR}/shared/ast/include
 ${SAD_SHARED_DIR}/shared/types/include
 ${SAD_SHARED_DIR}/shared/errors/include
 )
 if(MSVC)
 target_compile_options(sad_ffi PRIVATE /utf-8 /FS)
 endif()
 message(STATUS "Configured sad_ffi library (C API bridge, ownership annotations)")
endif()

# ============================================================================
# LLVM Abstraction Library (·¨‚© ×¬±¯ LLVM)
# Phase 91: T1030
# ============================================================================
if(LLVM_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/abstraction/llvm_wrapper.cpp")
 add_library(sad_abstraction STATIC src/abstraction/llvm_wrapper.cpp)
 target_include_directories(sad_abstraction
 PUBLIC
 ${CMAKE_CURRENT_SOURCE_DIR}/include
 ${CMAKE_CURRENT_SOURCE_DIR}/src/abstraction
 ${CMAKE_SOURCE_DIR}/include
 ${SAD_LLVM_INCLUDES}
 )
 target_compile_definitions(sad_abstraction PRIVATE SAD_USE_LLVM=1)
 if(MSVC)
 target_compile_options(sad_abstraction PRIVATE /utf-8 /FS)
 endif()
 target_link_libraries(sad_abstraction PUBLIC ${llvm_libs})
 message(STATUS "Configured sad_abstraction library (LLVM wrapper)")
endif()

# ============================================================================
# Compile-Time Evaluation Library ( )
# ============================================================================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/comptime/comptime.cpp")
 add_library(sad_comptime STATIC src/comptime/comptime.cpp)
 target_include_directories(sad_comptime
 PUBLIC
 ${CMAKE_CURRENT_SOURCE_DIR}/include
 ${CMAKE_CURRENT_SOURCE_DIR}/include/comptime
 ${CMAKE_SOURCE_DIR}/include
 )
 if(MSVC)
 target_compile_options(sad_comptime PRIVATE /utf-8 /FS)
 endif()
 message(STATUS "Configured sad_comptime library (compile-time evaluation)")
endif()

# ============================================================================
# Unified Compiler Library
# ============================================================================
add_library(sad_compiler INTERFACE)

if(LLVM_FOUND)
 target_link_libraries(sad_compiler
 INTERFACE
 sad_frontend
 sad_optimizer
 sad_llvm_backend
 sad_type_system
 sad_semantic
 sad_memory
 )
 # - 04: ( Unicode /utf-8)
 if(TARGET sad_security)
 target_link_libraries(sad_compiler INTERFACE sad_security)
 endif()
 if(TARGET sad_tools)
 target_link_libraries(sad_compiler INTERFACE sad_tools)
 endif()
 if(TARGET sad_ui_ir)
 target_link_libraries(sad_compiler INTERFACE sad_ui_ir)
 endif()
 # (Phase 96)
 if(TARGET sad_borrow)
 target_link_libraries(sad_compiler INTERFACE sad_borrow)
 endif()
 if(TARGET sad_targets)
 target_link_libraries(sad_compiler INTERFACE sad_targets)
 endif()
 if(TARGET sad_advanced_optimizer)
 target_link_libraries(sad_compiler INTERFACE sad_advanced_optimizer)
 endif()
 if(TARGET sad_ffi)
 target_link_libraries(sad_compiler INTERFACE sad_ffi)
 endif()
 if(TARGET sad_abstraction)
 target_link_libraries(sad_compiler INTERFACE sad_abstraction)
 endif()
 if(TARGET sad_comptime)
 target_link_libraries(sad_compiler INTERFACE sad_comptime)
 endif()
else()
 target_link_libraries(sad_compiler
 INTERFACE
 sad_frontend
 sad_optimizer
 sad_type_system
 sad_semantic
 sad_memory
 )
 if(TARGET sad_security)
 target_link_libraries(sad_compiler INTERFACE sad_security)
 endif()
 if(TARGET sad_tools)
 target_link_libraries(sad_compiler INTERFACE sad_tools)
 endif()
 if(TARGET sad_ui_ir)
 target_link_libraries(sad_compiler INTERFACE sad_ui_ir)
 endif()
 # (Phase 96) LLVM
 if(TARGET sad_borrow)
 target_link_libraries(sad_compiler INTERFACE sad_borrow)
 endif()
 if(TARGET sad_targets)
 target_link_libraries(sad_compiler INTERFACE sad_targets)
 endif()
 if(TARGET sad_advanced_optimizer)
 target_link_libraries(sad_compiler INTERFACE sad_advanced_optimizer)
 endif()
 if(TARGET sad_ffi)
 target_link_libraries(sad_compiler INTERFACE sad_ffi)
 endif()
 if(TARGET sad_comptime)
 target_link_libraries(sad_compiler INTERFACE sad_comptime)
 endif()
endif()

#
target_include_directories(sad_compiler
 INTERFACE
 ${CMAKE_SOURCE_DIR}/compiler/frontend/include
 ${CMAKE_SOURCE_DIR}/compiler/optimizer/include
 ${CMAKE_SOURCE_DIR}/compiler/backends/llvm
 ${CMAKE_SOURCE_DIR}/compiler/jit/include
 ${CMAKE_SOURCE_DIR}/compiler/bytecode/include
 ${CMAKE_SOURCE_DIR}/compiler/type_system/include
)

message(STATUS "Configured sad_compiler library (frontend, optimizer, llvm_backend, jit, type_system, semantic, memory)")

# ============================================================================
# Test Backend Executable ( )
# ============================================================================
# (AR) LLVM Debug + Release
# Release (EXCLUDE_FROM_ALL)
# (EN) Requires LLVM when both Debug + Release available, builds in both configs
# Otherwise Release-only (EXCLUDE_FROM_ALL)
if(LLVM_FOUND)
 if(SAD_LLVM_HAS_DEBUG)
 add_executable(test_backend tools/test_backend.cpp)
 message(STATUS "Configured test_backend executable (dual-config: Debug + Release)")
 else()
 add_executable(test_backend EXCLUDE_FROM_ALL tools/test_backend.cpp)
 message(STATUS "Configured test_backend executable (EXCLUDE_FROM_ALL €” Release only)")
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
 # (AR) LLVM cmake/llvm.cmake
 # (EN) Link LLVM libs via the global variable computed by cmake/llvm.cmake
 target_link_libraries(test_backend PRIVATE
 sad_llvm_backend
 sad_frontend
 sad_optimizer
 ${LLVM_LIBS}
 )
 if(MSVC)
 target_compile_options(test_backend PRIVATE /utf-8 /wd4819)
 endif()
endif()

# ============================================================================
# Tool Tests ( )
# ============================================================================
if(TARGET sad_tools)
 #
 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_security_scanner.cpp")
 add_executable(test_security_scanner tests/test_security_scanner.cpp)
 target_link_libraries(test_security_scanner PRIVATE sad_tools)
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

 #
 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_formatter.cpp")
 add_executable(test_formatter tests/test_formatter.cpp)
 target_link_libraries(test_formatter PRIVATE sad_tools)
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

 #
 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_docs_emitter.cpp")
 add_executable(test_docs_emitter tests/test_docs_emitter.cpp)
 target_link_libraries(test_docs_emitter PRIVATE sad_tools)
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

 #
 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/test_build_system.cpp")
 add_executable(test_build_system tests/test_build_system.cpp)
 target_link_libraries(test_build_system PRIVATE sad_tools)
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
# SIR Extended Library (Phase 95+: SIR Core SIR)
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
 # (AR) sir_to_llvm_v2.cpp TypeKind::Pointer/Widget compiler::instance
 # (EN) sir_to_llvm_v2.cpp deferred depends on missing TypeKind::Pointer/Widget and compiler::instance
)

#
set(SIR_VERIFIED_SOURCES "")
foreach(src ${SIR_EXTENDED_SOURCES})
 if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
 list(APPEND SIR_VERIFIED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${src}")
 else()
 message(STATUS " [sir_ext] Skipped missing: ${src}")
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
 target_link_libraries(sad_sir_extended PUBLIC sad_frontend)
 endif()
 if(MSVC)
 target_compile_options(sad_sir_extended PRIVATE /utf-8 /wd4819 /FS)
 endif()
 message(STATUS "Configured sad_sir_extended library (SIR core: ${SIR_VERIFIED_SOURCES})")
endif()

# ============================================================================
# Compiler Security Library (Phase 95: 200+ )
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
# Graphics Backend Support (Phase 95: sadc)
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
# Runtime Library ( Runtime )
# ============================================================================
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/runtime/CMakeLists.txt")
 add_subdirectory(runtime)
 message(STATUS " ×… ×¹ …¬¯ runtime ±³…§×")
endif()
