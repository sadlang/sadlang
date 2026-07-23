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
 # (AR) ?? ?????? LLVM_DIR ??? — ???? ?? cmake/llvm.cmake ?? ?? ??? ???????
 find_package(LLVM QUIET CONFIG)
endif()

if(LLVM_FOUND)
 message(STATUS "compiler_new: Found LLVM ${LLVM_PACKAGE_VERSION}")
 # (AR) ??????: ?? ?????? include_directories ??? — ?????? SAD_LLVM_INCLUDES
 # ?? ?? ??? ?????? LLVM headers ???? Debug/Release ????? ???????.
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
# (AR) Phase 6 — Step 2: ?? ????? builders/ ???????? (MethodCallBuilder ??????)
# (EN) Phase 6 — Step 2: include separated builders/ files (MethodCallBuilder etc.)
file(GLOB FRONTEND_BUILDERS_SOURCES "src/frontend/builders/*.cpp")
list(APPEND FRONTEND_SOURCES ${FRONTEND_BUILDERS_SOURCES})
file(GLOB FRONTEND_HEADERS "include/frontend/*.h")
file(GLOB FRONTEND_BUILDERS_HEADERS "include/frontend/builders/*.h")
list(APPEND FRONTEND_HEADERS ${FRONTEND_BUILDERS_HEADERS})

# (AR) ??????? ????? ?????? sir_builder.cpp ????? ??????? ?? ??????? ??????????
# (EN) Exclude legacy sir_builder.cpp to avoid duplication with split files
list(FILTER FRONTEND_SOURCES EXCLUDE REGEX "sir_builder\\.cpp$")

add_library(sad_frontend STATIC ${FRONTEND_SOURCES})

# (AR) ?????? ??????? ???????? (assertSafeCast<int> ?? class_main, class_constructor,
#      class_trait_impl, class_operator, builtins_simd, expression_objects, sir_builder_functions).
# (EN) Shared security primitives (assertSafeCast<int> used in builders).
if(TARGET sad_security_core)
    target_link_libraries(sad_frontend PUBLIC sad_security_core)
endif()

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
 # (AR) Phase 4 (F-03): ????? utils ????? ?????? #include ???????
 # (EN) Phase 4 (F-03): add utils to avoid relative #include workarounds
 ${SAD_SHARED_DIR}/shared/utils/include
 ${SAD_SHARED_DIR}/shared/hot_reload/include
 # (AR) م١ RFC اللهجات: جدول لهجة التجميع المولَّد (asm_dialect_generated.h) —
 #      يستهلكه statement_asm.cpp لخفض «تجميع … نهاية» إلى llvm::InlineAsm.
 # (EN) M1 native-dialects: generated assembly-dialect table for statement_asm.cpp.
 ${SAD_SHARED_DIR}/shared/dialects/generated
)

# (AR) مسار هيدرات sad_ui (ui_modifiers.h المولَّد من language-truth) — PRIVATE:
#      يستهلكه call_method_dispatch.cpp وحده لأسماء معدّلات SadUI القانونيّة. هيدر
#      ورقيّ (constexpr + inline) بلا أيّ اعتماد ربط، فالطبقيّة محفوظة: sad_frontend
#      لا يربط sad_graphics. (نفس المسار الذي يضمّه المفسّر في cmake/libraries.cmake.)
# (EN) sad_ui header path (generated ui_modifiers.h) — PRIVATE, consumed only by
#      call_method_dispatch.cpp. Header-only, no link dependency; layering preserved.
target_include_directories(sad_frontend PRIVATE
 ${CMAKE_SOURCE_DIR}/features/graphics/core/include
 # (② rfcs#46) تخطيط «حدث» المولَّد (SAD_EVENT_FIELDS/SAD_EVENT_STRUCT_NAME) —
 #   يستهلكه sir_builder_module.cpp لتوليد صنف «حدث» المضمَّن من مصدر الحقيقة.
 ${CMAKE_SOURCE_DIR}/shared/types/generated)

# (AR) ????? ??????? UTF-8 / Fix UTF-8 encoding warnings for frontend
if(MSVC)
 target_compile_options(sad_frontend PRIVATE /utf-8 /FS)
endif()

# ============================================================================
# Middle Library (Optimizer)
# FIXED: API updated to use new SIRInstruction API (January 2026)
# ============================================================================
file(GLOB MIDDLE_SOURCES "src/sir_optimizer/*.cpp")
# (AR) ?? ????? ????? async_transform.cpp ??? ????? ???????
# (EN) Re-included async_transform.cpp after fixing errors
file(GLOB MIDDLE_HEADERS "include/sir_optimizer/*.h")
add_library(sad_optimizer STATIC ${MIDDLE_SOURCES})

target_include_directories(sad_optimizer
 PUBLIC
 ${CMAKE_CURRENT_SOURCE_DIR}/include
 ${CMAKE_CURRENT_SOURCE_DIR}/include/sir_optimizer
 ${CMAKE_CURRENT_SOURCE_DIR}/include/frontend
 ${CMAKE_SOURCE_DIR}/include
 ${SAD_SHARED_DIR}/shared/ast/include
 ${SAD_SHARED_DIR}/shared/types/include
 ${SAD_SHARED_DIR}/shared/errors/include
 ${SAD_SHARED_DIR}/shared/lexer/include
)

target_link_libraries(sad_optimizer PUBLIC sad_frontend)

# (AR) ?????? ??????? ???????? (assertSafeCast<int> ?? async_transform/branch_folding/cse/inlining/licm).
# (EN) Shared security primitives.
if(TARGET sad_security_core)
    target_link_libraries(sad_optimizer PUBLIC sad_security_core)
endif()

# (AR) ?-?03: ????? PDB locking ?????? /FS
if(MSVC)
 target_compile_options(sad_optimizer PRIVATE /FS /utf-8)
endif()

# ============================================================================
# Backend Library (LLVM)
# ============================================================================
if(LLVM_FOUND)
 # (AR) ISSUE-076: يشمل sad_dyn_repr.cpp (تمثيل %SadDyn) عبر GLOB أدناه
 # (EN) ISSUE-076: includes sad_dyn_repr.cpp (%SadDyn representation) via the GLOB below
 file(GLOB LLVM_BACKEND_SOURCES "src/backend/llvm/*.cpp")
 file(GLOB LLVM_BACKEND_HEADERS "include/backend/llvm/*.h")
 # (AR) Phase 7 — Step 1: ?? ????? backend/llvm/builders/ (sub-codegens)
 # (EN) Phase 7 — Step 1: include backend/llvm/builders/ files (sub-codegens)
 file(GLOB_RECURSE LLVM_BACKEND_BUILDERS_SOURCES "src/backend/llvm/builders/*.cpp")
 list(APPEND LLVM_BACKEND_SOURCES ${LLVM_BACKEND_BUILDERS_SOURCES})
 file(GLOB_RECURSE LLVM_BACKEND_BUILDERS_HEADERS "include/backend/llvm/builders/*.h")
 list(APPEND LLVM_BACKEND_HEADERS ${LLVM_BACKEND_BUILDERS_HEADERS})

 # - 01: backend -LLVM
 # :
 # - interrupt_handlers.cpp: Windows (BREAKPOINT/OVERFLOW)
 # - solidity_ffi.cpp: <iomanip> <functional>
 # - pattern_codegen.cpp: enum (NUMBER DOUBLE, NULL_TYPE VOID)
 #
 # :
 # - async_codegen.cpp:
 # - ffi_wrapper.cpp: c_abi.h c_types.h headers
 # - inline_asm.cpp: IRBuilder template
 # - llvm_compiler_pipeline.cpp: include
 set(BACKEND_COMMON_SOURCES
 src/backend/linker_script.cpp
 src/backend/linker_script_x86_template.cpp
 src/backend/panic_handler.cpp
 src/backend/c_abi.cpp
 src/backend/solidity_ffi.cpp
 src/backend/pattern_codegen.cpp
 src/backend/pattern_codegen_api.cpp
 src/backend/async_codegen.cpp
 src/backend/ffi_wrapper.cpp
 src/backend/arm64_codegen.cpp
 src/backend/compose_emitter.cpp
 # (AR) evm_codegen.cpp حُذف في م5 (RFC sadlang-rfcs#10) — كان معزولًا، لم يُربط بعقد AST مطلقًا
 # (EN) evm_codegen.cpp deleted in phase 5 (RFC #10) — was isolated, never linked to AST nodes
 src/backend/extension_codegen.cpp
 src/backend/html_emitter.cpp
 src/backend/pipeline_chain.cpp
 src/backend/pipeline_chain_ops.cpp
 src/backend/pipeline_chain_fusion.cpp
 src/backend/pipeline_codegen.cpp
 # (AR) كلّ مسارات WASM حُذفت في م5 (RFC sadlang-rfcs#10) — المسار الحيّ: cmake/wasm.cmake (Emscripten → sad_wasm)
 # (EN) All WASM paths deleted in phase 5 (RFC #10) — live path: cmake/wasm.cmake (Emscripten → sad_wasm)
 )

 # - 02: pipeline
 # (AR) مصادر compiler/src/pipeline/ حُذفت في م5 (RFC sadlang-rfcs#10) — كانت كلّ رموزها
 #      (LLVMCompilerPipeline, NoStdModeManager, ...) ميتة وقت التشغيل (0 مستدعٍ خارجيّ).
 # (EN) compiler/src/pipeline/ sources deleted in phase 5 (RFC #10) — all symbols
 #      (LLVMCompilerPipeline, NoStdModeManager, ...) were dead at runtime (0 external callers).

 add_library(sad_llvm_backend STATIC
 ${LLVM_BACKEND_SOURCES}
 ${BACKEND_COMMON_SOURCES}
 )

 target_include_directories(sad_llvm_backend
 PUBLIC
 ${CMAKE_CURRENT_SOURCE_DIR}/include
 ${CMAKE_CURRENT_SOURCE_DIR}/include/backend/llvm
 ${CMAKE_CURRENT_SOURCE_DIR}/include/backend
 ${CMAKE_CURRENT_SOURCE_DIR}/include/pipeline
 ${CMAKE_CURRENT_SOURCE_DIR}/src/backend
 ${CMAKE_SOURCE_DIR}
 # (② rfcs#46) تخطيط «حدث» المولَّد — يستهلكه ui_ops.cpp لبناء بنية الحدث في thunk
 #   المعالِج بتخطيطٍ مطابقٍ للجسر بايتيًّا (SAD_EVENT_FIELDS + POD).
 ${CMAKE_SOURCE_DIR}/shared/types/generated
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
 # X86 — ??????
 X86CodeGen
 X86AsmParser
 X86Desc
 X86Info
 # AArch64 — ??????? ARM64 + macOS Apple Silicon
 AArch64CodeGen
 AArch64AsmParser
 AArch64Desc
 AArch64Info
 # ARM — ??????? 32-bit
 ARMCodeGen
 ARMAsmParser
 ARMDesc
 ARMInfo
 # (AR) مكوّنات WebAssembly في LLVM حُذفت في م5 (RFC sadlang-rfcs#10) — المسار الحيّ: cmake/wasm.cmake
 # (EN) WebAssembly LLVM components deleted in phase 5 (RFC #10) — live path: cmake/wasm.cmake
 native
 )

 # (AR) مصدر حقيقة واحد لمكتبات LLVM: حين يتوفّر تثبيت Debug (بناء مزدوج) نستعمل
 #      قائمة ${LLVM_LIBS} الموحَّدة المُحتسَبة في cmake/llvm.cmake (إغلاق كامل لكل
 #      تهيئة بصيغة debug/optimized) بدل القائمة المحلية الجزئية بمسارات Release —
 #      التي كانت تُربَط PUBLIC فتتسرّب إلى بناء sad-build في Debug (LNK2005/ABI)
 #      وتفتقر إلى حواف Debug (LNK2019). هذا يضمن الاتساق ويمنع التكرار (CW-19).
 # (EN) Single source of truth for LLVM libs: when a Debug install exists
 #      (dual build), use the unified ${LLVM_LIBS} computed in cmake/llvm.cmake
 #      (full per-config closure with debug/optimized) instead of this local
 #      Release-only subset, which leaked into the Debug sad-build (LNK2005/ABI)
 #      and lacked Debug-only edges (LNK2019).
 if(SAD_LLVM_HAS_DEBUG AND MSVC)
   set(llvm_libs ${LLVM_LIBS})
 endif()

 target_link_libraries(sad_llvm_backend
 PUBLIC
 ${llvm_libs}
 sad_frontend
 sad_optimizer
 sad_memory_gc  # (AR) Phase B-step2: ???? GC ???????? ???????? llvm_runtime*.cpp
 sad_ownership_runtime  # (AR) ffi_wrapper.h ??? ownership/runtime/ownership_tracker.h
 )

 # - 01: Unicode
 if(MSVC)
 target_compile_options(sad_llvm_backend PRIVATE /utf-8 /wd4819 /FS)
 endif()
endif()

# ============================================================================
# JIT Library — حُذفت في م5 (RFC sadlang-rfcs#10)
# ============================================================================
# (AR) مكتبة sad_jit (9034 سطرًا) كانت تُبنى ولا تُستعمل قطّ:
# - لا يستدعيها المفسّر ولا المترجم ولا أيّ اختبار
# - تعريفات أماميّة فقط لـ LLVM ORC ("until LLVM installation completes")
# - مكرَّرة معماريًّا مع vm/sad_jit (الذي يعمل فعلًا على VM bytecode)
# (EN) sad_jit library (9034 lines) was built but never used:
# - Not called by interpreter, compiler, or any test
# - Forward decls only for LLVM ORC ("until LLVM installation completes")
# - Architecturally duplicated with vm/sad_jit (which actually works on VM bytecode)

# ============================================================================
# Bytecode Library — حُذفت في م5 (RFC sadlang-rfcs#10)
# ============================================================================
# (AR) ملفّات الصيغة المستقلّة (format): decoder, encoder, constant_pool, symbol_table, magic, version, validator, opcodes
# (EN) Standalone format files: decoder, encoder, constant_pool, symbol_table, magic, version, validator, opcodes
# (AR) الملفّات المعتمدة على SIR (emitter.cpp, bytecode.cpp) كانت مؤجَّلة — تحتاج تحديث SIR API
# (EN) SIR-dependent files (emitter.cpp, bytecode.cpp) were deferred - needed SIR API update
# (AR) صيغة ملفّ .sadbc لم تُدمَج في pipeline: 0 مستهلكين خارجيّين.

# ============================================================================
# Semantic Analysis Library — ????? ??? shared/ownership/ (Ownership Unification)
# ============================================================================
# (AR) ????? ????? ???????: ????? ????? ??????? ?????? ???????? ??????????
#      (borrow_checker, lifetime_analyzer, move_analyzer, ownership_tracker,
#      unsafe_checker, pattern_ownership, exhaustiveness, this_binding,
#      cascade_prevention) ????? ?? compiler/src/semantic/ ???
#      shared/ownership/src/ ???????? ?? ???? ???????? (???????? VM? ????????
#      WASM? ???? ????? ????????).
#
#      ??????? ??????? ?? sad_ownership? ????? alias ???? ???? sad_semantic
#      ??????? ??????. ??????? ??????? ??? ??? cmake/libraries.cmake.
#
# (EN) Architecture refactor: ownership/borrow checking files moved from
#      compiler/src/semantic/ to shared/ownership/src/ to be consumed by all
#      execution paths. New library: sad_ownership (with legacy sad_semantic
#      alias). Actual add_subdirectory done via cmake/libraries.cmake.
# ============================================================================


# ============================================================================
# Security Library (Triple Guard + Permissions + Diagnostics)
# Phase 92: T1031-T1034 (Triple Guard)
# Phase 94: T1041, T1043 (Permission checker + Arabic errors)
# ============================================================================
# (AR) ??????: ???? compiler_security.cpp + compiler_security_arithmetic_mem.cpp
#      ??? shared/security/ (????? sad_security_core) ?? ????? ????? ?????
#      ?????????. ?????? guard_*/permission_*/diagnostics ?? ????? ???? ???
#      ?????? ??? ????? no-op ?????? ?????? ??????? ?????????? (Triple Guard).
# (EN) Note: compiler_security.cpp + compiler_security_arithmetic_mem.cpp were
#      relocated to shared/security/ (sad_security_core) in the unified security
#      refactor. The guard_*/permission_*/diagnostics directories don't exist
#      yet, so this block is currently a no-op kept as a hook for future phases.
file(GLOB SECURITY_CODEGEN_SOURCES "src/codegen/guard_*.cpp")
file(GLOB SECURITY_SEMA_SOURCES "src/sema/permission_*.cpp")
file(GLOB SECURITY_DIAG_SOURCES "src/diagnostics/*.cpp")

set(SECURITY_ALL_SOURCES ${SECURITY_CODEGEN_SOURCES} ${SECURITY_SEMA_SOURCES} ${SECURITY_DIAG_SOURCES})

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
 target_link_libraries(sad_security PUBLIC sad_security_core)
 if(MSVC)
 target_compile_options(sad_security PRIVATE /utf-8 /wd4819 /FS)
 endif()
 message(STATUS "Configured sad_security library (triple guard, permissions, diagnostics)")
endif()

# ============================================================================
# (AR) ????? sad_memory ?????? ?? ??????? B-step1 — ?????? ????????? ??? shared/:
#        - gc_mode.* ? memory_mode_flag.* ? shared/memory_policy/  (??????? A1)
#        - cycle_detector*                  ? shared/memory_gc/    (??????? B-step1)
#        - gc_fallback_mode.cpp            ? ???? (???? ???? ??????)
#      ???????? ??????? (?????/?????/VM) ???? ???? sad_memory_policy ? sad_memory_gc
#      ?????? ?? shared/.
# (EN) sad_memory library removed in Phase B-step1 — contents migrated to shared/.
#      Interpreter, compiler, and VM now link sad_memory_policy and sad_memory_gc
#      from shared/ directly.
# ============================================================================

# ============================================================================
# Type System Library
# ============================================================================
# (AR) ????? ????? ?????? ???? ??????? — ?? ????? 19 ??? ??? ?? ???
#      ?????? ?? ?? ???? ???? compiler/src/types (???? ????? ????? ?????).
#      ??????? ???????? ??????? ?? shared/semantic/include/semantic/type_checker.h
#      ?????? (10 ?????) ?? ????? ??? type_inferencer/constraint_solver (5 ?????).
# (EN) Explicit list of type-system sources after removing 19 dead files
#      that were never referenced outside compiler/src/types. Remaining files
#      are consumed by shared/semantic (directly or transitively).
set(TYPE_SYSTEM_SOURCES
    src/types/arabic_types.cpp
    src/types/constraint.cpp
    src/types/constraint_solver.cpp
    src/types/enum_types.cpp
    src/types/future_type.cpp
    src/types/generator_type.cpp
    src/types/generics.cpp
    src/types/primitive_type.cpp
    src/types/struct_types.cpp
    src/types/substitution.cpp
    src/types/trait_system.cpp
    src/types/type.cpp
    src/types/type_inferencer.cpp
    src/types/type_registry.cpp
    src/types/type_variable.cpp
    src/types/unification.cpp
    src/types/union_type.cpp
    src/types/union_types.cpp
)
file(GLOB TYPE_SYSTEM_HEADERS "include/types/*.h")

add_library(sad_type_system STATIC ${TYPE_SYSTEM_SOURCES})

# (AR) ?????? ??????? ???????? (assertSafeCast<int> ?? constraint_solver).
# (EN) Shared security primitives.
if(TARGET sad_security_core)
    target_link_libraries(sad_type_system PUBLIC sad_security_core)
endif()

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

# (AR) ?????? ??????? ???????? (assertSafeCast<int> ?? constraint_solver).
# (EN) Shared security primitives.
if(TARGET sad_security_core)
    target_link_libraries(sad_type_system PUBLIC sad_security_core)
endif()

# ============================================================================
# Tools Library ( : )
# Merged tools: formatter, docs emitter, build system
# (AR) ?????? ????? security_scanner*/taint_tracker.cpp — ?????? ???
#      tools/security-scanner/ (sad_security_scanner_lib + sad-guard).
# (EN) Removed security_scanner*/taint_tracker.cpp — moved to
#      tools/security-scanner/ (sad_security_scanner_lib + sad-guard).
# (AR) ?????? ????? format/sad_formatter*.cpp — ?????? ???
#      tools/formatter/ (sad_formatter_lib) ?????? ??? tools/shared
#      (sad_tools_shared) ???? ???? KeywordTable ?? shared/lexer ???????.
# (EN) Removed format/sad_formatter*.cpp — moved to tools/formatter/
#      (sad_formatter_lib) which depends on tools/shared (sad_tools_shared)
#      that reads KeywordTable from shared/lexer directly.
# ============================================================================
set(TOOLS_SOURCES
 src/meta/docs_emitter.cpp
 src/meta/docs_emitter_html.cpp
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
 # (AR) ?????? ??????? ???????? (assertSafeCast<int> ?? docs_emitter_html).
 # (EN) Shared security primitives.
 if(TARGET sad_security_core)
     target_link_libraries(sad_tools PUBLIC sad_security_core)
 endif()
 message(STATUS "Configured sad_tools library (formatter, docs, build)")
else()
 message(STATUS " [tools] No verified source files — sad_tools skipped")
endif()

# ============================================================================
# SadUI IR Library — مكتبة التمثيل الوسيط لواجهات SadUI
# ============================================================================
# (AR) ملاحظة: مصادر src/ui/sad_ui_ir*.cpp غير موجودة على القرص حاليًّا، فتُصفَّى
#      عبر UI_IR_VERIFIED_SOURCES أدناه وتُتخطّى المكتبة بأمان (لا فشل بناء).
#      الكتلة محفوظة للتفعيل المستقبليّ عند توفّر المصادر؛ لا تُحذف.
# (EN) Note: src/ui/sad_ui_ir*.cpp are absent on disk right now, so they are
#      filtered by UI_IR_VERIFIED_SOURCES below and the library is safely skipped
#      (no build failure). Block kept for future activation; do not delete.
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
 # (AR) تنفيذ UIParser المطلوب من sad_ui_pipeline.cpp
 # (EN) UIParser implementation required by sad_ui_pipeline.cpp
 ${SAD_SHARED_DIR}/shared/parser/src/specs/ui/parser_ui_maps.cpp
)

# (AR) تصفية المصادر الموجودة فعلًا على القرص فقط / (EN) keep only sources present on disk
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
 message(STATUS " [ui_ir] No source files found — sad_ui_ir skipped")
endif()

# ============================================================================
# Borrow Checker / Lifetime Library — REMOVED
# ============================================================================
# (AR) ???? compiler/src/borrow/ ???????: ????? Phase 20 (T204-T206) ?? ???????
#      ?? (0 ???????). ????? ?????? ??????? ????lifetimes ?? shared/ownership/.
# (EN) compiler/src/borrow/ deleted entirely: Phase 20 prototype with zero
#      external callers. Real ownership/lifetime checking in shared/ownership/.
# ============================================================================

# ============================================================================
# Cross-Compilation Targets Library — REMOVED
# ============================================================================
# (AR) ???? compiler/src/targets/ ???????: ????? Phase 24 (T235/T236/T241) ?? ????? ???????.
#      ??????: tools/compiler/compiler_driver.h::TargetTriple +
#      compiler/src/backend/llvm/llvm_target.cpp::LLVMTargetManager.
# (EN) compiler/src/targets/ deleted: Phase 24 prototype never wired into the driver.
#      Real path: tools/compiler/compiler_driver.h::TargetTriple + LLVMTargetManager.
# ============================================================================

# ============================================================================
# (AR) ?????? ????? sad_advanced_optimizer ??????? (CW-19 + BF-09).
#      ???? ???? 3 ??????? ????? ?? ConstantFoldingPass / DeadCodeElimination /
#      ... ??? ????? AST ???? ?????? ?? (?????? ???? SIR-only ???
#      sad_optimizer ?? compiler/src/sir_optimizer/). ????? ????? ???????:
#        - compiler/src/optimizer/optimization_pass.cpp + .h
#        - compiler/src/optimizer/advanced_optimizer.cpp + .h
#        - compiler/src/optimizer/optimizer_o2.h
#        - compiler/src/optimizer/optimization_pass_*.cpp (4 ????? ?????)
#        - tests/optimizer/ (8 ???????? ??? CMakeLists)
#      ????? ??? arabic_passes.cpp ????? ??? compiler/src/backend/llvm/
#      ?????? ???????? ??? sad_llvm_backend ???????? ?? PassBuilder.
#      ????: compiler/docs/optimization_layers.md
# (EN) sad_advanced_optimizer library fully removed (CW-19 + BF-09).
#      It contained 3 duplicate AST-level pass implementations with no live
#      consumers (live path is SIR-only via sad_optimizer in compiler/src/sir_optimizer/).
#      Only arabic_passes.cpp survived — relocated to compiler/src/backend/llvm/
#      and registered in the LLVM PassBuilder.
#      See: compiler/docs/optimization_layers.md
# ============================================================================

# ============================================================================
# (AR) ?????? ????? sad_ffi — ???? ???? c_api_bridge.cpp ? ownership_annotations.cpp
#      ??????? ??? ??? ??????? (?? ?????? ????? ????????). ?????? ????
#      ??? FFI (c_abi.h, ffi_wrapper.h) ?? ???? ?? compiler/include/ffi/
#      ???????? ??? sad_llvm_backend.
# (EN) sad_ffi library removed — c_api_bridge.cpp and ownership_annotations.cpp
#      were dead code (no external consumers). The live FFI core (c_abi.h,
#      ffi_wrapper.h) remains under compiler/include/ffi/ and is compiled
#      via sad_llvm_backend.
# ============================================================================

# ============================================================================
# LLVM Abstraction Library — طبقة تجريد LLVM
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
 sad_memory_gc
 sad_memory_policy
 )
 # (AR) ?????? ??????? ???????? (BoundsChecker/SafeArithmetic/InputSanitizer/...)
 # (EN) Shared security primitives (BoundsChecker/SafeArithmetic/InputSanitizer/...)
 if(TARGET sad_security_core)
 target_link_libraries(sad_compiler INTERFACE sad_security_core)
 endif()
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
 if(TARGET sad_abstraction)
 target_link_libraries(sad_compiler INTERFACE sad_abstraction)
 endif()
else()
 target_link_libraries(sad_compiler
 INTERFACE
 sad_frontend
 sad_optimizer
 sad_type_system
 sad_semantic
 sad_memory_gc
 sad_memory_policy
 )
 # (AR) ?????? ??????? ???????? (BoundsChecker/SafeArithmetic/InputSanitizer/...)
 # (EN) Shared security primitives (BoundsChecker/SafeArithmetic/InputSanitizer/...)
 if(TARGET sad_security_core)
 target_link_libraries(sad_compiler INTERFACE sad_security_core)
 endif()
 if(TARGET sad_security)
 target_link_libraries(sad_compiler INTERFACE sad_security)
 endif()
 if(TARGET sad_tools)
 target_link_libraries(sad_compiler INTERFACE sad_tools)
 endif()
 if(TARGET sad_ui_ir)
 target_link_libraries(sad_compiler INTERFACE sad_ui_ir)
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
 message(STATUS "Configured test_backend executable (EXCLUDE_FROM_ALL — Release only)")
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
 # (AR) إصلاح ربط: LLVMSupport.lib (raw_socket_stream في LLVM الأحدث) يستدعي
 #      دوال Winsock (accept/bind/connect/listen/socket/WSA*) ⇒ يجب ربط ws2_32.
 #      عطل سابق يظهر فقط حين تتوفّر LLVM Debug (test_backend ضمن ALL لا EXCLUDE).
 # (EN) Link fix: LLVMSupport (raw_socket_stream) pulls Winsock symbols; link ws2_32.
 target_link_libraries(test_backend PRIVATE ws2_32)
 endif()
endif()

# ============================================================================
# Tool Tests ( )
# ============================================================================
if(TARGET sad_tools)
 # (AR) test_security_scanner ???? ??? tools/security-scanner/tests/ ??? ???
 #      ?????? ?????? ??? ???? ??????. ????? ???? ?? tools/security-scanner/CMakeLists.txt.
 # (EN) test_security_scanner moved to tools/security-scanner/tests/ alongside
 #      the scanner relocation. It's now built by tools/security-scanner/CMakeLists.txt.

 #
 # (AR) test_formatter ???? ??? tools/formatter/tests/ ?? ??? ?????? ??????
 #      ??? ???? ?????? (sad_formatter_lib). ????? ???? ??
 #      tools/formatter/CMakeLists.txt ??? ????? ???????? ???????.
 # (EN) test_formatter moved to tools/formatter/tests/ alongside the formatter
 #      relocation. Now built by tools/formatter/CMakeLists.txt when tool
 #      tests are enabled.

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
endif()

# ============================================================================
# (AR) ?????? ?? ????? ????? ?????: ????? sad_sir_extended ??? ???????
#      (sir_validator, ast_to_sir, builtin_intrinsics, sir_optimizer,
#       sir_printer, sir_to_llvm). ???? ????? ??? ?? ?????? ?? ??????.
#      ?????? ??????? ???? ??? sad_frontend (compiler/src/frontend/sir_*).
# (EN) Removed in dead-code cleanup: sad_sir_extended library and all its
#      sources. They were compiled but never linked. Real work is done by
#      sad_frontend (compiler/src/frontend/sir_*).
# ============================================================================

# ============================================================================
# Compiler Security Library (Phase 95)
# ============================================================================
# (AR) ?????? ???? sad_compiler_security ???????: ?????? ??????
#      (compiler_security.cpp) ????? ??? shared/security/ ???? ??
#      sad_security_core. ?? ?????? ???? ??? ?? ???? ???? sad_security_core.
# (EN) The sad_compiler_security block was removed entirely: its only source
#      (compiler_security.cpp) moved to shared/security/ as part of
#      sad_security_core. Former consumers should now link sad_security_core.

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
 message(STATUS "تمّ تضمين مجلّد runtime الفرعيّ / Added runtime subdirectory")
endif()
