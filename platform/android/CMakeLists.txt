# ═══════════════════════════════════════════════════════════════════════════════
# CMakeLists.txt - بناء مكتبة ص الأصلية لأندرويد
# Sad Native Library Build for Android — Full Interpreter Integration
# ═══════════════════════════════════════════════════════════════════════════════
#
# الاستخدام (Usage):
#   cmake -S platform/android -B build_android \
#         -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
#         -DANDROID_ABI=arm64-v8a \
#         -DANDROID_PLATFORM=android-24
#   cmake --build build_android
#
# ═══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.22.1)

project(sad_android 
    VERSION 1.0.0 
    LANGUAGES CXX
    DESCRIPTION "مكتبة ص الأصلية لأندرويد / Sad Native Library for Android"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ═══════════════════════════════════════════════════════════════════════════════
# التحقق من بيئة Android
# ═══════════════════════════════════════════════════════════════════════════════

if(NOT ANDROID)
    message(FATAL_ERROR "This CMakeLists.txt is for Android NDK builds only.
Use: -DCMAKE_TOOLCHAIN_FILE=\$ANDROID_NDK/build/cmake/android.toolchain.cmake")
endif()

message(STATUS "═══════════════════════════════════════════════════════════")
message(STATUS "  بناء مكتبة ص لأندرويد / Building Sad for Android")
message(STATUS "═══════════════════════════════════════════════════════════")
message(STATUS "  ANDROID_ABI:      ${ANDROID_ABI}")
message(STATUS "  ANDROID_PLATFORM: ${ANDROID_PLATFORM}")
message(STATUS "  ANDROID_NDK:      ${ANDROID_NDK}")
message(STATUS "═══════════════════════════════════════════════════════════")

# ═══════════════════════════════════════════════════════════════════════════════
# مسارات المشروع الجذرية
# ═══════════════════════════════════════════════════════════════════════════════

get_filename_component(SAD_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)

# ═══════════════════════════════════════════════════════════════════════════════
# ملفات مصدر المفسر الكامل (Lexer + Parser + AST + Interpreter + Stdlib)
# ═══════════════════════════════════════════════════════════════════════════════

# --- Utilities ---
set(SAD_UTILS_SOURCES
    ${SAD_ROOT_DIR}/shared/utils/src/string_utils.cpp
)

# --- Lexer ---
set(SAD_LEXER_SOURCES
    ${SAD_ROOT_DIR}/shared/lexer/src/token.cpp
    ${SAD_ROOT_DIR}/shared/lexer/src/lexer_core.cpp
    ${SAD_ROOT_DIR}/shared/lexer/src/lexer_keywords.cpp
)

# --- Parser ---
set(SAD_PARSER_SOURCES
    ${SAD_ROOT_DIR}/shared/parser/src/core/parser_core.cpp
    ${SAD_ROOT_DIR}/shared/parser/src/core/parser_main.cpp
    ${SAD_ROOT_DIR}/shared/parser/src/declarations/parser_declarations.cpp
    ${SAD_ROOT_DIR}/shared/parser/src/statements/parser_statements.cpp
    ${SAD_ROOT_DIR}/shared/parser/src/core/parser_expressions.cpp
    ${SAD_ROOT_DIR}/shared/parser/src/statements/parser_advanced.cpp
    ${SAD_ROOT_DIR}/shared/parser/src/core/parser_helpers.cpp
    ${SAD_ROOT_DIR}/shared/parser/src/declarations/parser_oop.cpp
    ${SAD_ROOT_DIR}/shared/parser/src/declarations/parser_modules.cpp
)

# --- AST ---
set(SAD_AST_SOURCES
    ${SAD_ROOT_DIR}/shared/ast/src/ast_node.cpp
    ${SAD_ROOT_DIR}/shared/ast/src/expressions.cpp
    ${SAD_ROOT_DIR}/shared/ast/src/statements.cpp
    ${SAD_ROOT_DIR}/shared/ast/src/declarations.cpp
    ${SAD_ROOT_DIR}/shared/ast/src/property_nodes.cpp
    ${SAD_ROOT_DIR}/shared/ast/src/module_nodes.cpp
    ${SAD_ROOT_DIR}/shared/ast/src/ast_printer.cpp
    ${SAD_ROOT_DIR}/shared/ast/src/type_nodes.cpp
    ${SAD_ROOT_DIR}/shared/ast/src/class_nodes.cpp
    ${SAD_ROOT_DIR}/shared/ast/src/ast_clone.cpp
)

# --- Interpreter Core ---
set(SAD_INTERP_SOURCES
    ${SAD_ROOT_DIR}/interpreter/src/core/interpreter_core.cpp
    ${SAD_ROOT_DIR}/interpreter/src/visitors/expression_evaluator_core.cpp
    ${SAD_ROOT_DIR}/interpreter/src/visitors/expression_evaluator_calls.cpp
    ${SAD_ROOT_DIR}/interpreter/src/visitors/expression_evaluator_oop.cpp
    ${SAD_ROOT_DIR}/interpreter/src/visitors/expression_evaluator_members.cpp
    ${SAD_ROOT_DIR}/interpreter/src/visitors/statement_executor.cpp
    ${SAD_ROOT_DIR}/interpreter/src/visitors/statement_executor_oop.cpp
    ${SAD_ROOT_DIR}/interpreter/src/visitors/statement_executor_modules.cpp
    ${SAD_ROOT_DIR}/interpreter/src/exception.cpp
    ${SAD_ROOT_DIR}/interpreter/src/debug/debug_server.cpp
)

# --- Managers ---
set(SAD_MANAGERS_SOURCES
    ${SAD_ROOT_DIR}/interpreter/src/managers/variable_manager.cpp
    ${SAD_ROOT_DIR}/interpreter/src/managers/function_manager.cpp
    ${SAD_ROOT_DIR}/interpreter/src/managers/scope_manager.cpp
    ${SAD_ROOT_DIR}/interpreter/src/managers/ownership_manager.cpp
    ${SAD_ROOT_DIR}/interpreter/src/managers/class_manager.cpp
    ${SAD_ROOT_DIR}/interpreter/src/managers/object_manager.cpp
)

# --- Data Types ---
set(SAD_DATA_SOURCES
    ${SAD_ROOT_DIR}/shared/types/src/value.cpp
    ${SAD_ROOT_DIR}/shared/types/src/generator.cpp
    ${SAD_ROOT_DIR}/shared/types/src/class_type.cpp
    ${SAD_ROOT_DIR}/shared/types/src/object_instance.cpp
)

# --- Errors ---
set(SAD_ERROR_SOURCES
    ${SAD_ROOT_DIR}/shared/errors/src/error_codes.cpp
    ${SAD_ROOT_DIR}/shared/errors/src/diagnostic.cpp
    ${SAD_ROOT_DIR}/shared/errors/src/error_manager.cpp
    ${SAD_ROOT_DIR}/shared/errors/src/smart_errors.cpp
    ${SAD_ROOT_DIR}/shared/errors/src/smart_suggestions.cpp
    ${SAD_ROOT_DIR}/shared/errors/src/smart_type_explanations.cpp
    ${SAD_ROOT_DIR}/shared/errors/src/smart_multi_error.cpp
    ${SAD_ROOT_DIR}/shared/errors/src/smart_fix_suggestions.cpp
    ${SAD_ROOT_DIR}/shared/errors/src/smart_teacher_mode.cpp
    ${SAD_ROOT_DIR}/shared/errors/src/smart_error_recovery.cpp
    ${SAD_ROOT_DIR}/shared/errors/src/smart_cascade_prevention.cpp
)

# --- SadUI Core (واجهات — HTML codegen فقط على أندرويد) ---
set(SAD_UI_SOURCES
    ${SAD_ROOT_DIR}/sad_ui/core/src/types.cpp
    ${SAD_ROOT_DIR}/sad_ui/core/src/ir.cpp
    ${SAD_ROOT_DIR}/sad_ui/core/src/ir_builder.cpp
    ${SAD_ROOT_DIR}/sad_ui/core/src/node.cpp
    ${SAD_ROOT_DIR}/sad_ui/core/src/style.cpp
    ${SAD_ROOT_DIR}/sad_ui/core/src/layout.cpp
    ${SAD_ROOT_DIR}/sad_ui/core/src/state.cpp
    ${SAD_ROOT_DIR}/sad_ui/core/src/animation.cpp
    ${SAD_ROOT_DIR}/sad_ui/core/src/hybrid_routing.cpp
    ${SAD_ROOT_DIR}/sad_ui/core/src/reconciler.cpp
    ${SAD_ROOT_DIR}/sad_ui/backends/web/src/html_codegen.cpp
    ${SAD_ROOT_DIR}/interpreter/src/ui/ui_bridge.cpp
    ${SAD_ROOT_DIR}/interpreter/src/ui/ui_bridge_events.cpp
    ${SAD_ROOT_DIR}/interpreter/src/ui/ui_bridge_platform.cpp
)

# --- Builtins (core + modules + kernel) ---
file(GLOB SAD_BUILTIN_MODULES "${SAD_ROOT_DIR}/interpreter/src/builtins/builtin_module_*.cpp")
file(GLOB SAD_BUILTIN_KERNELS "${SAD_ROOT_DIR}/interpreter/src/builtins/builtin_kernel_*.cpp")
set(SAD_BUILTINS_SOURCES
    ${SAD_ROOT_DIR}/interpreter/src/builtins/builtin_registry.cpp
    ${SAD_ROOT_DIR}/interpreter/src/builtins/builtin_core_io.cpp
    ${SAD_BUILTIN_MODULES}
    ${SAD_BUILTIN_KERNELS}
    ${SAD_ROOT_DIR}/stdlib/core/builtins.cpp
)

# --- Stdlib (أنواع أساسية + system) ---
set(SAD_STDLIB_SOURCES
    ${SAD_ROOT_DIR}/stdlib/core/stdlib_manager.cpp
    ${SAD_ROOT_DIR}/stdlib/core/type_functions.cpp
    ${SAD_ROOT_DIR}/stdlib/core/array_functions.cpp
    ${SAD_ROOT_DIR}/stdlib/core/other_functions.cpp
    ${SAD_ROOT_DIR}/stdlib/string/string_functions.cpp
    ${SAD_ROOT_DIR}/stdlib/math/math_functions.cpp
    ${SAD_ROOT_DIR}/stdlib/math/advanced_math.cpp
    ${SAD_ROOT_DIR}/stdlib/io/io_functions.cpp
    ${SAD_ROOT_DIR}/stdlib/system/system_functions.cpp
    ${SAD_ROOT_DIR}/stdlib/filesystem/filesystem_module.cpp
    ${SAD_ROOT_DIR}/stdlib/json/json_module.cpp
    ${SAD_ROOT_DIR}/stdlib/xml/xml_module.cpp
)

# --- Low Level (serial, bitwise, etc.) ---
file(GLOB SAD_LOW_LEVEL_SOURCES "${SAD_ROOT_DIR}/stdlib/low_level/src/*.cpp")

# --- Modules ---
set(SAD_MODULES_SOURCES
    ${SAD_ROOT_DIR}/shared/modules/src/module.cpp
    ${SAD_ROOT_DIR}/shared/modules/src/search_paths.cpp
    ${SAD_ROOT_DIR}/shared/modules/src/module_cache.cpp
    ${SAD_ROOT_DIR}/shared/modules/src/dependency_graph.cpp
    ${SAD_ROOT_DIR}/shared/modules/src/module_loader.cpp
    ${SAD_ROOT_DIR}/shared/modules/src/module_resolver.cpp
    ${SAD_ROOT_DIR}/shared/modules/src/symbol_resolver.cpp
    ${SAD_ROOT_DIR}/shared/modules/src/module_validator.cpp
)

# --- Compiler Frontend (optimizer only — TypeChecker guarded out on Android) ---
set(SAD_COMPILER_SOURCES
    ${SAD_ROOT_DIR}/compiler/src/optimizer/advanced_optimizer.cpp
    ${SAD_ROOT_DIR}/compiler/src/optimizer/optimization_pass.cpp
)
set(SAD_ANDROID_SOURCES
    src/sad_app_main.cpp
    src/sad_android_bridge.cpp
    src/sad_android_runtime.cpp
    src/native_ui_storage.cpp
    src/native_ui_builder.cpp
)

# ═══════════════════════════════════════════════════════════════════════════════
# المكتبة المشتركة الرئيسية
# ═══════════════════════════════════════════════════════════════════════════════

add_library(sad_app SHARED
    ${SAD_ANDROID_SOURCES}
    ${SAD_UTILS_SOURCES}
    ${SAD_LEXER_SOURCES}
    ${SAD_PARSER_SOURCES}
    ${SAD_AST_SOURCES}
    ${SAD_INTERP_SOURCES}
    ${SAD_MANAGERS_SOURCES}
    ${SAD_DATA_SOURCES}
    ${SAD_ERROR_SOURCES}
    ${SAD_BUILTINS_SOURCES}
    ${SAD_STDLIB_SOURCES}
    ${SAD_LOW_LEVEL_SOURCES}
    ${SAD_MODULES_SOURCES}
    ${SAD_COMPILER_SOURCES}
    ${SAD_UI_SOURCES}
)

# ═══════════════════════════════════════════════════════════════════════════════
# مسارات Include
# ═══════════════════════════════════════════════════════════════════════════════

target_include_directories(sad_app PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${SAD_ROOT_DIR}/shared/lexer/include
    ${SAD_ROOT_DIR}/shared/parser/include
    ${SAD_ROOT_DIR}/shared/ast/include
    ${SAD_ROOT_DIR}/shared/types/include
    ${SAD_ROOT_DIR}/shared/errors/include
    ${SAD_ROOT_DIR}/shared/utils/include
    ${SAD_ROOT_DIR}/shared/modules/include
    ${SAD_ROOT_DIR}/interpreter/include
    ${SAD_ROOT_DIR}/interpreter/include/core
    ${SAD_ROOT_DIR}/interpreter/include/visitors
    ${SAD_ROOT_DIR}/interpreter/include/managers
    ${SAD_ROOT_DIR}/interpreter/include/debug
    ${SAD_ROOT_DIR}/stdlib/core
    ${SAD_ROOT_DIR}/stdlib/string
    ${SAD_ROOT_DIR}/stdlib/math
    ${SAD_ROOT_DIR}/stdlib/io
    ${SAD_ROOT_DIR}/stdlib
    ${SAD_ROOT_DIR}/stdlib/filesystem
    ${SAD_ROOT_DIR}/stdlib/http
    ${SAD_ROOT_DIR}/stdlib/database
    ${SAD_ROOT_DIR}/stdlib/json
    ${SAD_ROOT_DIR}/stdlib/xml
    ${SAD_ROOT_DIR}/stdlib/crypto
    ${SAD_ROOT_DIR}/stdlib/system
    ${SAD_ROOT_DIR}/stdlib/low_level/include
    ${SAD_ROOT_DIR}/compiler/include
    ${SAD_ROOT_DIR}/compiler/include/semantic
    ${SAD_ROOT_DIR}/compiler/src/optimizer
    ${SAD_ROOT_DIR}/sad_ui/core/include
    ${SAD_ROOT_DIR}/sad_ui/pipeline/include
    ${SAD_ROOT_DIR}/sad_ui/widgets/include
    ${SAD_ROOT_DIR}/sad_ui/backends/web/include
    ${SAD_ROOT_DIR}/interpreter/src/ui
)

# ═══════════════════════════════════════════════════════════════════════════════
# تعريفات الماكرو
# ═══════════════════════════════════════════════════════════════════════════════

target_compile_definitions(sad_app PRIVATE
    SAD_PLATFORM_ANDROID=1
    SAD_ANDROID=1
    SAD_MOBILE=1
    SAD_NO_SDL2=1
    SAD_NO_FILESYSTEM=1
    SAD_NO_NETWORK=1
    SAD_NO_DATABASE=1
    SAD_NO_SYSTEM=1
    SAD_NO_COMPOSE_CODEGEN=1
    SAD_NO_SWIFTUI_CODEGEN=1
    SAD_NO_APPKIT_CODEGEN=1
)

# ═══════════════════════════════════════════════════════════════════════════════
# مكتبات Android الأصلية
# ═══════════════════════════════════════════════════════════════════════════════

find_library(log-lib log)
find_library(android-lib android)

target_link_libraries(sad_app
    ${log-lib}
    ${android-lib}
)

# ═══════════════════════════════════════════════════════════════════════════════
# خصائص المكتبة
# ═══════════════════════════════════════════════════════════════════════════════

set_target_properties(sad_app PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/libs/${ANDROID_ABI}"
    # إزالة البادئة lib من اسم الملف
    PREFIX ""
    OUTPUT_NAME "libsad_app"
)

# ═══════════════════════════════════════════════════════════════════════════════
# رسالة النجاح
# ═══════════════════════════════════════════════════════════════════════════════

message(STATUS "")
message(STATUS "  [✓] libsad_app.so سيُبنى في:")
message(STATUS "      ${CMAKE_BINARY_DIR}/libs/${ANDROID_ABI}/libsad_app.so")
message(STATUS "")
