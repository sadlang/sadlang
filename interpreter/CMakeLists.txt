# ============================================================================
# Interpreter - المفسر
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# مكتبة المفسر
# ============================================================================
add_library(sad_interpreter STATIC
    # النواة
    src/core/interpreter_core.cpp
    src/ast_printer.cpp
    src/exception.cpp
    
    # Visitors
    src/visitors/expression_evaluator_core.cpp
    src/visitors/expression_evaluator_calls.cpp
    src/visitors/expression_evaluator_members.cpp
    src/visitors/expression_evaluator_oop.cpp
    src/visitors/statement_executor.cpp
    src/visitors/statement_executor_oop.cpp
    src/visitors/statement_executor_modules.cpp
    
    # Managers
    src/managers/class_manager.cpp
    src/managers/function_manager.cpp
    src/managers/object_manager.cpp
    src/managers/variable_manager.cpp
    src/managers/ownership_manager.cpp
    src/managers/scope_manager.cpp
    
    # OOP - تم إصلاحه!
    src/oop/interpreter_classes_fixed.cpp
    
    # Debug
    src/debug/debug_server.cpp
    
    # NOTE: builtin_registry.cpp is NOT included here because it references
    # 34 part files that are compiled in sad_core (cmake/sources.cmake).
    # The sad_interpreter library provides the interpreter core only.
)

target_include_directories(sad_interpreter
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core
        ${CMAKE_CURRENT_SOURCE_DIR}/include/visitors
        ${CMAKE_CURRENT_SOURCE_DIR}/include/managers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/builtins
        ${CMAKE_CURRENT_SOURCE_DIR}/include/debug
        # stdlib headers needed by builtin_registry.cpp
        ${CMAKE_SOURCE_DIR}/stdlib/core
        ${CMAKE_SOURCE_DIR}/stdlib
)

# للتوافق مع الكود القديم
target_include_directories(sad_interpreter
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/interpreter
        ${CMAKE_SOURCE_DIR}/include/interpreter/core
        ${CMAKE_SOURCE_DIR}/include/interpreter/visitors
        ${CMAKE_SOURCE_DIR}/include/data/managers
        ${CMAKE_SOURCE_DIR}/include/data/scope
        # (AR) Phase 3 (F-01): فاحص الأنواع نُقل إلى shared/semantic/.
        #      أُزيل compiler/include + compiler/include/semantic (F-10) لأن المفسر
        #      يجب ألا يعتمد على رؤوس المترجم. type_checker.h يأتي عبر sad_shared.
        # (EN) Phase 3 (F-01): type checker moved to shared/semantic/.
        #      Removed compiler/include + compiler/include/semantic (F-10) — interpreter
        #      must not depend on compiler headers. type_checker.h comes via sad_shared.
        ${CMAKE_SOURCE_DIR}/shared/semantic/include
)

target_link_libraries(sad_interpreter
    PUBLIC
        sad_shared
        # (AR) Phase 3 (F-01): ربط فاحص الأنواع المشترك / (EN) Link shared type checker
        sad_semantic_shared
)

target_compile_features(sad_interpreter PUBLIC cxx_std_17)

# إصلاح تحذيرات UTF-8 / Fix UTF-8 encoding warnings
if(MSVC)
    target_compile_options(sad_interpreter PRIVATE /utf-8)
endif()

# رسالة تأكيد
message(STATUS "Configured sad_interpreter library")
