# ============================================================================
# Shared Components - المكونات المشتركة
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# مكتبة المكونات المشتركة
# ============================================================================
add_library(sad_shared STATIC
    # Lexer
    lexer/src/lexer_core.cpp
    lexer/src/lexer_keywords.cpp
    lexer/src/token.cpp
    
    # Parser
    parser/src/core/parser_core.cpp
    parser/src/core/parser_helpers.cpp
    parser/src/core/parser_main.cpp
    parser/src/declarations/parser_oop.cpp
    parser/src/declarations/parser_modules.cpp
    parser/src/statements/parser_statements.cpp
    parser/src/core/parser_expressions.cpp
    parser/src/declarations/parser_declarations.cpp
    parser/src/statements/parser_advanced.cpp
    parser/src/ui/parser_ui.cpp
    
    # AST
    ast/src/ast_node.cpp
    ast/src/ast_printer.cpp
    ast/src/declarations.cpp
    ast/src/expressions.cpp
    ast/src/module_nodes.cpp
    ast/src/property_nodes.cpp
    ast/src/statements.cpp
    ast/src/type_nodes.cpp
    ast/src/class_nodes.cpp          # OOP AST nodes (ThisExpr, SuperExpr, NewExpr, etc.)
    ast/src/ast_clone.cpp            # Deep clone for AST nodes (templates, macros, traits)
    
    # OOP Managers (مطلوبة للمترجم sadc)
    ${CMAKE_SOURCE_DIR}/interpreter/src/managers/class_manager.cpp
    
    # Types
    types/src/class_type.cpp
    types/src/generator.cpp
    types/src/object_instance.cpp
    types/src/value.cpp
    types/src/sad_type_system.cpp
    types/src/type_bridge.cpp         # ADR-01 Phase 1: جسر SadType ↔ Value
    
    # Errors
    errors/src/diagnostic.cpp
    errors/src/error_codes.cpp
    errors/src/error_manager.cpp
    errors/src/error_hints.cpp
    
    # Smart Errors - Phase 15
    errors/src/smart_errors.cpp
    errors/src/smart_suggestions.cpp
    errors/src/smart_type_explanations.cpp
    errors/src/smart_multi_error.cpp
    errors/src/smart_fix_suggestions.cpp
    errors/src/smart_teacher_mode.cpp
    errors/src/smart_error_recovery.cpp
    errors/src/smart_cascade_prevention.cpp
    # Pattern + Permission diagnostics (Fix #26 — نُقلت من compiler/src/diagnostics/)
    errors/src/pattern_diagnostics.cpp
    errors/src/permission_diagnostics.cpp
    
    # Modules
    modules/src/dependency_graph.cpp
    modules/src/module.cpp
    modules/src/module_cache.cpp
    modules/src/module_loader.cpp
    modules/src/module_resolver.cpp
    modules/src/module_validator.cpp
    modules/src/search_paths.cpp
    modules/src/symbol_resolver.cpp
    
    # Utils
    utils/src/string_utils.cpp
)

target_include_directories(sad_shared
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer/include
        ${CMAKE_CURRENT_SOURCE_DIR}/parser/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/include
        ${CMAKE_CURRENT_SOURCE_DIR}/types/include
        ${CMAKE_CURRENT_SOURCE_DIR}/errors/include
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/include
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/builtins/include           # السجل المركزي الموحّد (ADR-003)
        ${CMAKE_SOURCE_DIR}/interpreter/include/managers   # ClassManager header
)

# للتوافق مع الكود القديم
target_include_directories(sad_shared
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/parser
        ${CMAKE_SOURCE_DIR}/include/parser/ast
        ${CMAKE_SOURCE_DIR}/include/lexer
        ${CMAKE_SOURCE_DIR}/include/data/types
        ${CMAKE_SOURCE_DIR}/include/errors
        ${CMAKE_SOURCE_DIR}/include/modules
        ${CMAKE_SOURCE_DIR}/include/utils
        ${CMAKE_SOURCE_DIR}/interpreter/include
        ${CMAKE_SOURCE_DIR}/interpreter/include/managers
)

target_compile_features(sad_shared PUBLIC cxx_std_17)

# إصلاح مشكلة PDB / Fix PDB lock issue
if(MSVC)
    target_compile_options(sad_shared PRIVATE /FS /utf-8)
endif()

# رسالة تأكيد
message(STATUS "Configured sad_shared library")

# ============================================================================
# (AR) المكتبة الفرعية: فاحص الأنواع المشترك (sad_semantic_shared)
# (EN) Sub-library: shared type checker (sad_semantic_shared)
# (AR) نُقلت من compiler/ في Phase 3 (F-01) لإصلاح اعتماد المفسر على المترجم.
# (EN) Moved from compiler/ in Phase 3 (F-01) to fix interpreter→compiler dep.
# ============================================================================
add_subdirectory(semantic)

# ============================================================================
# (AR) المكتبة الفرعية: نظام الملكية الموحَّد (sad_ownership)
# (EN) Sub-library: unified ownership system (sad_ownership)
# (AR) نُقلت من compiler/src/semantic/ لتُستخدم من جميع المسارات (المفسّر/VM/
#      المترجم/WASM/أداة الفحص). تنشئ alias قديم باسم sad_semantic للتوافق.
# (EN) Moved from compiler/src/semantic/ to be consumed by all execution paths.
#      Creates legacy sad_semantic alias for backward compatibility.
# @phase Architecture Refactor: Ownership Unification
# ============================================================================
add_subdirectory(ownership)

# ============================================================================
# (AR) المكتبة الفرعية: الطبقة الأمنية المشتركة (sad_security_core)
# (EN) Sub-library: shared security layer (sad_security_core)
# (AR) نُقلت من compiler/src/security/ لتكون متاحة لكل المسارات (المترجم،
#      المفسر، VM، runtime، tools). توحّد BoundsChecker, SafeArithmetic,
#      InputSanitizer, SafeAllocator, TaintTracker تحت Sad::Security.
# (EN) Moved from compiler/src/security/ so all paths (compiler, interpreter,
#      VM, runtime, tools) can use it. Unifies BoundsChecker, SafeArithmetic,
#      InputSanitizer, SafeAllocator, TaintTracker under namespace Sad::Security.
# @phase Architecture Refactor: Security Layer Unification
# ============================================================================
add_subdirectory(security)

# ============================================================================
# (AR) مكتبة سياسة الذاكرة الموحَّدة — shared/memory_policy/
#      نُقلت من compiler/memory/ في إطار توحيد طبقة الذاكرة (المرحلة A).
#      تُتيح للمفسر والمترجم والآلة الافتراضية قراءة نفس أوضاع --dev/--prod/--learn
#      ومستويات الملكية واستراتيجيات GC من مصدر واحد.
# (EN) Unified memory-mode policy library — moved from compiler/memory/ as part
#      of the memory layer unification (Phase A). Interpreter, compiler, and VM
#      all consult the same source of truth for --dev/--prod/--learn,
#      ownership levels, and GC strategies.
# @phase Architecture Refactor: Memory Layer Unification — Phase A
# ============================================================================
add_subdirectory(memory_policy)

