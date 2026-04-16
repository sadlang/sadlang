# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/executables.cmake
# الوصف: الملفات التنفيذية الرئيسية / Main Executable Targets
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# المفسر الرئيسي / Main Interpreter (sad)
# ──────────────────────────────────────────────────────────────────────
add_executable(sad tools/compiler/main_simple.cpp)

target_link_libraries(sad PRIVATE
    sad_core
    sad_vm
    sad_new_type_system
    sad_new_semantic
    sad_network
    sad_http
    sad_websocket
    sad_mobile
    sad_ui
)

# (AR) ربط مكتبة مصحح الأداء / (EN) Link profiler library
if(TARGET sad_profiler_lib)
    target_link_libraries(sad PRIVATE sad_profiler_lib)
endif()

# (AR) ربط مكتبة وقت التشغيل إذا تم بناؤها / (EN) Link runtime if built
if(TARGET sad_rt_runtime)
    target_link_libraries(sad PRIVATE sad_rt_runtime)
endif()

target_include_directories(sad PRIVATE
    ${CMAKE_SOURCE_DIR}/vm/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include/semantic
    ${CMAKE_SOURCE_DIR}/compiler_new/include/types
    ${CMAKE_SOURCE_DIR}/compiler_new/include/backend
    ${CMAKE_SOURCE_DIR}/tools/compiler/src
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/hot_reload/include
)

set_target_properties(sad PROPERTIES
    OUTPUT_NAME "sad"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

message(STATUS "✓ المفسر / Interpreter: sad")

# ──────────────────────────────────────────────────────────────────────
# مكتبة المنسّق / Formatter Library
# ─────────────────────────────────────────────────────────────────────
add_library(sad_formatter STATIC
    compiler_new/src/format/sad_formatter.cpp
    compiler_new/src/format/sad_formatter_rebuild.cpp
)

target_include_directories(sad_formatter PUBLIC
    ${CMAKE_SOURCE_DIR}/compiler_new/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(sad_formatter PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(sad_formatter PRIVATE /wd4819 /FS)
endif()

message(STATUS "✓ منسّق الكود / Formatter: sad_formatter")

# ─────────────────────────────────────────────────────────────────────
# مكتبة تطبيقات الهاتف / Mobile Applications Library
# ─────────────────────────────────────────────────────────────────────
set(MOBILE_SOURCES
    tools/compiler/src/android_target.cpp
    tools/compiler/src/ios_target.cpp
    tools/compiler/src/mobile_project_gen.cpp
    tools/compiler/src/cli_commands.cpp
    tools/compiler/src/cli_mobile_manager.cpp
    tools/compiler/src/run_command.cpp
    tools/compiler/src/test_command.cpp
    tools/compiler/src/build_command.cpp
    shared/parser/src/specs/ui/parser_ui_maps.cpp
)

# (AR) التحقق من وجود build_system.cpp قبل إضافته
# (EN) Check if build_system.cpp exists before adding it
if(EXISTS "${CMAKE_SOURCE_DIR}/compiler_new/src/build/build_system.cpp")
    list(APPEND MOBILE_SOURCES "compiler_new/src/build/build_system.cpp")
endif()

add_library(sad_mobile STATIC ${MOBILE_SOURCES})

if(MSVC)
    target_compile_options(sad_mobile PRIVATE /wd4819 /FS)
endif()

target_include_directories(sad_mobile PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/tools/compiler/src
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include/backend
)

target_compile_features(sad_mobile PUBLIC cxx_std_17)

target_link_libraries(sad_mobile PUBLIC sad_formatter)

set_target_properties(sad_mobile PROPERTIES
    OUTPUT_NAME "sad_mobile"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

message(STATUS "✓ تطبيقات الهاتف / Mobile: Android + iOS")

# ──────────────────────────────────────────────────────────────────────
# المترجم sadc (يتطلب LLVM) / Compiler sadc (requires LLVM)
# ──────────────────────────────────────────────────────────────────────
if(ENABLE_LLVM_BACKEND AND LLVM_FOUND)

    # ================================================================
    # (AR) تضمين ملف runtime كبيانات في C++
    # (EN) Embed runtime C file as C++ data header
    # ================================================================
    set(SAD_RUNTIME_SOURCE "${CMAKE_SOURCE_DIR}/tools/compiler/runtime/sad_embedded_runtime.c")
    set(SAD_RUNTIME_HEADER "${CMAKE_BINARY_DIR}/generated/sad_embedded_runtime_data.h")

    add_custom_command(
        OUTPUT "${SAD_RUNTIME_HEADER}"
        COMMAND ${CMAKE_COMMAND}
            -DRUNTIME_INPUT_FILE=${SAD_RUNTIME_SOURCE}
            -DRUNTIME_OUTPUT_FILE=${SAD_RUNTIME_HEADER}
            -P ${CMAKE_SOURCE_DIR}/cmake/embed_runtime.cmake
        DEPENDS "${SAD_RUNTIME_SOURCE}"
        COMMENT "(AR) تضمين runtime... / (EN) Embedding runtime..."
    )

    # (AR) هدف وهمي لضمان التوليد قبل البناء
    # (EN) Custom target to ensure generation before build
    add_custom_target(generate_runtime_header DEPENDS "${SAD_RUNTIME_HEADER}")

    add_executable(sadc
        tools/compiler/main.cpp
        tools/compiler/compiler_driver_frontend.cpp
        tools/compiler/compiler_driver_diagnostics.cpp
        tools/compiler/compiler_driver_analysis.cpp
        tools/compiler/compiler_driver_backend.cpp
        tools/compiler/compiler_driver_cli.cpp
        tools/compiler/compiler_driver_linker.cpp
        tools/compiler/compiler_driver_lld.cpp
        tools/compiler/compiler_driver_build_utils.cpp
        tools/compiler/compiler_driver_ui.cpp
        tools/compiler/compiler_driver_android.cpp
        tools/compiler/compiler_driver_android_linker.cpp
        tools/compiler/compiler_driver_pkg.cpp
    )

    target_include_directories(sadc PRIVATE
        ${CMAKE_SOURCE_DIR}/tools/compiler
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
        ${CMAKE_SOURCE_DIR}/compiler_new/include
        ${CMAKE_SOURCE_DIR}/compiler_new/include/frontend
        ${CMAKE_SOURCE_DIR}/compiler_new/include/semantic
        ${CMAKE_SOURCE_DIR}/compiler_new/include/types
        ${CMAKE_SOURCE_DIR}/compiler_new/include/backend/llvm
        ${LLVM_INCLUDE_DIRS}
        ${CMAKE_BINARY_DIR}/generated
    )

    # (AR) ضمان توليد الهيدر قبل بناء sadc
    # (EN) Ensure runtime header is generated before building sadc
    add_dependencies(sadc generate_runtime_header)

    target_link_libraries(sadc PRIVATE
        sad_shared sad_new_compiler sad_mobile ${LLVM_LIBS}
    )

    # ربط مكتبات LLD إذا كانت متوفرة / Link LLD libraries if available
    if(HAS_EMBEDDED_LLD)
        target_link_libraries(sadc PRIVATE ${LLD_LIBS})
        target_compile_definitions(sadc PRIVATE HAS_EMBEDDED_LLD)
        message(STATUS "   sadc: LLD مدمج / embedded LLD enabled")
    endif()

    if(MSVC)
        target_compile_options(sadc PRIVATE /wd4819 /FS)
    endif()

    set_target_properties(sadc PROPERTIES
        OUTPUT_NAME "sadc"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    # (AR) زيادة حجم الـ stack إلى 128 ميغابايت لمعالجة الملفات الكبيرة
    # (EN) Increase stack size to 128MB for large source files
    if(MSVC)
        target_link_options(sadc PRIVATE /STACK:134217728)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(sadc PRIVATE -Wl,-z,stacksize=134217728)
    endif()

    message(STATUS "✓ المترجم / Compiler: sadc (LLVM ${LLVM_PACKAGE_VERSION})")
else()
    message(STATUS "⊘ المترجم sadc معطّل (LLVM غير متوفر) / sadc disabled")
endif()
