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
    # (AR) Codegen v4.1: جدول الكلمات المولَّد من language-truth/keywords.yaml
    # (EN) Codegen v4.1: keyword table generated from language-truth/keywords.yaml
    lexer/generated/keywords_generated.cpp
    
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
    
    # Types
    types/src/class_type.cpp
    # (AR) ClassManager مواطن أساس (المحلّل يحتاجه) — نُقل من interpreter/ إلى types/
    #      (RFC sadlang-rfcs#10 م3 خطوة 2) ليوافق موقعُه طبقتَه ويزيل سبب تصدير interpreter/include.
    # (EN) ClassManager is a foundation citizen (parser needs it) — relocated from
    #      interpreter/ to types/ (RFC #10 phase-3 step-2), removing the reason for the leak.
    types/src/class_manager.cpp
    types/src/generator.cpp
    types/src/object_instance.cpp
    types/src/value.cpp
    types/src/sad_type_system.cpp

    # Errors
    errors/src/diagnostic.cpp
    errors/src/error_manager.cpp
    errors/src/error_hints.cpp
    # (AR) المرحلة 1 — سجل قوالب الأخطاء + مستويات الشرح
    # (EN) Phase 1 — Error template catalog + explanation levels
    errors/src/error_catalog.cpp
    # (AR) EM-3: المصدر الموحَّد المُولَّد من language-truth/errors/ يستبدل error_codes.cpp +
    #      error_catalog_init.cpp (يُعرّف getErrorCodeString/Description/Category + registerDefaults
    #      + kErrorMessages/findByCode/findById). دليل التكافؤ: YAML superset للقوالب اليدوية.
    # (EN) EM-3: generated single source replaces error_codes.cpp + error_catalog_init.cpp.
    errors/generated/error_messages_generated.cpp
    # (AR) EM-3: كتالوجات runtime اليدوية (9) + ترويستها أُزيلت — registerDefaults المُولَّد
    #      يُسجّل كل الـ75 خطأً من YAML (superset)، فلم يعد لها مستدعٍ (كود ميت مُثبَت).
    # (EN) EM-3: 9 manual runtime catalogs + helper removed — generated registerDefaults
    #      registers all 75 errors from YAML (superset); they had zero callers (dead code).
    errors/src/explanation_level.cpp
    errors/src/formatter.cpp
    
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
    # (AR) أُزيلت في تنظيف Phase F (0 مُستدعين): pattern_diagnostics.cpp, permission_diagnostics.cpp
    # (EN) Removed in Phase F cleanup (0 callers): pattern_diagnostics.cpp, permission_diagnostics.cpp

    # (AR) نقطة القرار الموحَّدة لأخطاء الملكية — توحيد سلوك --gc/--learn/--prod
    # (EN) Unified ownership-error decision point — unifies --gc/--learn/--prod behavior
    errors/src/builders/dispatch.cpp
    
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
        ${CMAKE_CURRENT_SOURCE_DIR}/lexer/generated         # (AR) keywords_generated.h
        ${CMAKE_CURRENT_SOURCE_DIR}/parser/include
        ${CMAKE_CURRENT_SOURCE_DIR}/ast/include
        ${CMAKE_CURRENT_SOURCE_DIR}/types/include
        ${CMAKE_CURRENT_SOURCE_DIR}/errors/include
        ${CMAKE_CURRENT_SOURCE_DIR}/errors/src             # Phase 4: catalog/runtime_template_helpers.h
        ${CMAKE_CURRENT_SOURCE_DIR}/modules/include
        ${CMAKE_CURRENT_SOURCE_DIR}/utils/include
        ${CMAKE_CURRENT_SOURCE_DIR}/builtins/include           # السجل المركزي الموحّد (ADR-003)
        ${CMAKE_CURRENT_SOURCE_DIR}/memory_gc/include          # B-step5b: pin.h يحتاج garbage_collector.h
)

# (AR) م3 خطوة 3 (RFC sadlang-rfcs#10): أُزيل تصدير interpreter/include +
#      interpreter/include/managers من sad_shared. بعد نقل class_manager.h إلى
#      types/include (خطوة 2) لم يَعُد أيُّ مصدرٍ في sad_shared يحتاج ترويسات المفسّر،
#      فلا يصحّ أن تُصدّرها النواةُ لمن يربطها (نظام المترجم خاصّةً). يثبّت حارسُ
#      check_interpreter_boundary.py السلامةَ، وأهدافُ المفسّر تأخذ مساراتها من
#      include_directories الجذر مباشرةً.
# (EN) Phase-3 step-3: removed the interpreter/include exports from sad_shared.
#      After relocating class_manager.h to types/include (step 2), no sad_shared
#      source needs interpreter headers, so the core must not propagate them to its
#      linkers (notably the compiler subsystem). Interpreter targets still get their
#      paths from the root include_directories.
# (AR) للتوافق مع الكود القديم / (EN) Legacy include paths for backward compatibility
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
)

target_compile_features(sad_shared PUBLIC cxx_std_17)

# إصلاح مشكلة PDB / Fix PDB lock issue
if(MSVC)
    target_compile_options(sad_shared PRIVATE /FS /utf-8)
endif()

# رسالة تأكيد / Confirmation message
message(STATUS "✓ تهيئة مكتبة sad_shared / Configured sad_shared library")

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
# (AR) المكتبة الفرعية: نظام أمان null الموحَّد (sad_null_safety)
# (EN) Sub-library: unified null-safety system (sad_null_safety)
# (AR) المكوّن المشترك NullSafetyAnalyzer يُستدعى من المحرّكين (المفسّر + sad-build)
#      ليكون تحليل أمان null مصدرَ حقيقة واحدًا — نظير sad_ownership.
#      NS-01: هيكل + استدعاء فقط (بلا منطق تحليل بعد).
# (EN) Shared NullSafetyAnalyzer invoked from both engines as a single source
#      of truth — mirroring sad_ownership. NS-01: scaffold + call sites only.
# @phase Null-Safety System: NS-01
# ============================================================================
add_subdirectory(null_safety)

# ============================================================================
# (AR) المكتبة الفرعية: قواعد البرنامج البنيويّة الموحَّدة (sad_program_rules)
# (EN) Sub-library: unified structural program rules (sad_program_rules)
# (AR) checkMainFunctionRule (SEM018) يُستدعى من المحرّكين ليكون مصدرَ حقيقة واحدًا
#      لقاعدة الدالة الرئيسية — إنهاءً لتباعد المحرّكات (كان الفحص محصورًا بالمفسّر).
# (EN) checkMainFunctionRule (SEM018) invoked from both engines as a single source
#      of truth for the main-function rule — ending the engine divergence.
# ============================================================================
add_subdirectory(program_rules)

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

# ============================================================================
# (AR) مكتبة محرك جامع القمامة الموحَّدة — shared/memory_gc/
#      نُقلت كاشفات الدورات من compiler/memory/ في إطار المرحلة B-step1.
#      المراحل اللاحقة (B2/B3) ستضيف محرك GC الفعلي وتربطه بالمسارات الثلاثة.
# (EN) Unified GC engine library — cycle detection moved from compiler/memory/
#      as part of Phase B-step1. Future phases (B2/B3) will add the GC engine
#      proper and wire it across interpreter / compiler / VM.
# @phase Architecture Refactor: Memory Layer Unification — Phase B
# ============================================================================
add_subdirectory(memory_gc)

# (AR) B-step5b (إصلاح TEST-004): types/src/value.cpp يستدعي محرك GC مباشرة
#      (defaultEngine/registerObject/setVisitor/isTracked) — التبعية تُعلَن هنا
#      حيث تنشأ (CW-06) لا في sad_core فقط، وإلا انكسر كل مستهلك يربط sad_shared
#      وحدها (اختبارات الحزمة الشاملة كانت تفشل ربطاً بـ LNK2019).
#      الدورة الساكنة sad_shared ⇄ sad_memory_gc مقصودة ومدعومة من CMake
#      (يكرّر المكتبتين على سطر الربط).
# (EN) B-step5b (TEST-004 fix): types/src/value.cpp calls the GC engine directly,
#      so the dependency must be declared where it originates (CW-06), not only
#      on sad_core — otherwise every consumer linking bare sad_shared breaks
#      (comprehensive tests failed with LNK2019). The static cycle
#      sad_shared ⇄ sad_memory_gc is intentional; CMake repeats both libs.
target_link_libraries(sad_shared PUBLIC sad_memory_gc)

# ============================================================================
# (AR) مكتبة متتبع الملكية في وقت التشغيل — shared/ownership_runtime/
#      نُقلت من compiler/{include/ffi/ffi_wrapper.h, src/backend/ffi_wrapper.cpp}
#      في إطار DEF-001. توفر OwnershipTracker (singleton آمن للخيوط) إضافةً إلى
#      طبقة C-ABI (sad_ownership_*) ليتمكن مخرج sad-build من تفعيل --prod بدون
#      الاعتماد على رموز C++ من المترجم.
# (EN) Runtime pointer ownership tracker — moved from compiler/ffi_wrapper.*
#      as part of DEF-001. Provides a thread-safe singleton plus a C-ABI
#      surface so sad-build-compiled output can enable --prod without linking any
#      compiler C++ symbols.
# @phase Architecture Refactor: Memory Layer Unification — Phase B / DEF-001
# ============================================================================
add_subdirectory(ownership_runtime)

