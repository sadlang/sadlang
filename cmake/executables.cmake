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
    sad_graphics
)

target_include_directories(sad PRIVATE
    ${CMAKE_SOURCE_DIR}/vm/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include/semantic
    ${CMAKE_SOURCE_DIR}/compiler_new/include/types
    ${CMAKE_SOURCE_DIR}/compiler_new/include/backend
    ${CMAKE_SOURCE_DIR}/tools/compiler/src
    ${CMAKE_SOURCE_DIR}/shared/parser/include
)

set_target_properties(sad PROPERTIES
    OUTPUT_NAME "sad"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# نسخ SDL2.dll / Copy SDL2.dll
set(SDL2_DLL_PATH "${CMAKE_SOURCE_DIR}/graphics/third_party/SDL2/SDL2-2.28.5/lib/x64/SDL2.dll")
if(EXISTS ${SDL2_DLL_PATH})
    add_custom_command(TARGET sad POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_DLL_PATH}" $<TARGET_FILE_DIR:sad>
        COMMENT "نسخ SDL2.dll / Copying SDL2.dll"
    )
endif()

# نسخ مكتبة الواجهات / Copy UI library
add_custom_command(TARGET sad POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:sad>/stdlib"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/stdlib/واجهات.ص" "$<TARGET_FILE_DIR:sad>/stdlib/واجهات.ص"
    COMMENT "نسخ مكتبة الواجهات / Copying UI library"
)

message(STATUS "✓ المفسر / Interpreter: sad")

# ──────────────────────────────────────────────────────────────────────
# مكتبة تطبيقات الهاتف / Mobile Applications Library
# ──────────────────────────────────────────────────────────────────────
set(MOBILE_SOURCES
    tools/compiler/src/android_target.cpp
    tools/compiler/src/ios_target.cpp
    tools/compiler/src/mobile_project_gen.cpp
    tools/compiler/src/cli_main.cpp
    tools/compiler/src/build_command.cpp
    tools/compiler/src/run_command.cpp
    tools/compiler/src/test_command.cpp
    tools/compiler/src/formatter.cpp
    shared/parser/src/ui_parser.cpp
    compiler_new/src/backend/android_compose.cpp
    compiler_new/src/backend/ios_swiftui.cpp
)

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

set_target_properties(sad_mobile PROPERTIES
    OUTPUT_NAME "sad_mobile"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

message(STATUS "✓ تطبيقات الهاتف / Mobile: Android + iOS")

# ──────────────────────────────────────────────────────────────────────
# المترجم sadc (يتطلب LLVM) / Compiler sadc (requires LLVM)
# ──────────────────────────────────────────────────────────────────────
if(ENABLE_LLVM_BACKEND AND LLVM_FOUND)
    add_executable(sadc
        tools/compiler/main.cpp
        tools/compiler/compiler_driver.cpp
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
    )

    target_link_libraries(sadc PRIVATE
        sad_shared sad_new_compiler sad_mobile sad_graphics ${LLVM_LIBS}
    )

    if(MSVC)
        target_compile_options(sadc PRIVATE /wd4819 /FS)
    endif()

    set_target_properties(sadc PROPERTIES
        OUTPUT_NAME "sadc"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    install(TARGETS sadc RUNTIME DESTINATION bin COMPONENT compiler)
    message(STATUS "✓ المترجم / Compiler: sadc (LLVM ${LLVM_PACKAGE_VERSION})")
else()
    message(STATUS "⊘ المترجم sadc معطّل (LLVM غير متوفر) / sadc disabled")
endif()
