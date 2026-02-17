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
    parser/src/parser_core.cpp
    parser/src/parser_core_helpers.cpp
    parser/src/parser_core_impl.cpp
    parser/src/parser_core_oop.cpp
    parser/src/parser_modules.cpp
    
    # AST
    ast/src/ast_node.cpp
    ast/src/ast_printer.cpp
    ast/src/declarations.cpp
    ast/src/expressions.cpp
    ast/src/module_nodes.cpp
    ast/src/property_nodes.cpp
    ast/src/statements.cpp
    ast/src/type_nodes.cpp
    
    # Types
    types/src/class_type.cpp
    types/src/generator.cpp
    types/src/object_instance.cpp
    types/src/value.cpp
    
    # Errors
    errors/src/diagnostic.cpp
    errors/src/error_codes.cpp
    errors/src/error_manager.cpp
    
    # Smart Errors - Phase 15
    errors/src/smart_errors.cpp
    errors/src/smart_suggestions.cpp
    errors/src/smart_type_explanations.cpp
    errors/src/smart_multi_error.cpp
    errors/src/smart_fix_suggestions.cpp
    errors/src/smart_teacher_mode.cpp
    errors/src/smart_error_recovery.cpp
    errors/src/smart_cascade_prevention.cpp
    
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
        ${CMAKE_SOURCE_DIR}/interpreter_new/include
        ${CMAKE_SOURCE_DIR}/interpreter_new/include/managers
)

target_compile_features(sad_shared PUBLIC cxx_std_17)

# إصلاح مشكلة PDB / Fix PDB lock issue
if(MSVC)
    target_compile_options(sad_shared PRIVATE /FS)
endif()

# رسالة تأكيد
message(STATUS "Configured sad_shared library")
