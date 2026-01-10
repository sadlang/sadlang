# ============================================================================
# Interpreter - المفسر
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# مكتبة المفسر
# ============================================================================
add_library(sad_new_interpreter STATIC
    # النواة
    src/core/interpreter_core.cpp
    src/ast_printer.cpp
    src/exception.cpp
    
    # Visitors
    src/visitors/expression_evaluator.cpp
    src/visitors/statement_executor.cpp
    src/visitors/statement_executor_oop.cpp
    
    # Managers
    src/managers/class_manager.cpp
    src/managers/function_manager.cpp
    src/managers/object_manager.cpp
    src/managers/variable_manager.cpp
    
    # OOP - تم إصلاحه!
    src/oop/interpreter_classes_fixed.cpp
    
    # Builtins - مستبعد مؤقتاً للإصلاح  
    # src/builtins/builtin_registry.cpp
)

target_include_directories(sad_new_interpreter
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/core
        ${CMAKE_CURRENT_SOURCE_DIR}/include/visitors
        ${CMAKE_CURRENT_SOURCE_DIR}/include/managers
        ${CMAKE_CURRENT_SOURCE_DIR}/include/builtins
)

# للتوافق مع الكود القديم
target_include_directories(sad_new_interpreter
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/interpreter
        ${CMAKE_SOURCE_DIR}/include/interpreter/core
        ${CMAKE_SOURCE_DIR}/include/interpreter/visitors
        ${CMAKE_SOURCE_DIR}/include/data/managers
        ${CMAKE_SOURCE_DIR}/include/data/scope
)

target_link_libraries(sad_new_interpreter
    PUBLIC
        sad_shared
)

target_compile_features(sad_new_interpreter PUBLIC cxx_std_17)

# رسالة تأكيد
message(STATUS "Configured sad_new_interpreter library")
