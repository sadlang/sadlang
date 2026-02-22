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
    sad_rt_runtime
)

# ربط مكتبة الواجهات الرسومية / Link UI IR library
if(TARGET sad_ui_ir)
    target_link_libraries(sad PRIVATE sad_ui_ir)
endif()

target_include_directories(sad PRIVATE
    ${CMAKE_SOURCE_DIR}/vm/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include
    ${CMAKE_SOURCE_DIR}/compiler_new/include/ui
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
    tools/compiler/src/cli_commands.cpp
    tools/compiler/src/cli_mobile_manager.cpp
    tools/compiler/src/run_command.cpp
    tools/compiler/src/test_command.cpp
    tools/compiler/src/build_command.cpp
    compiler_new/src/format/sad_formatter.cpp
    compiler_new/src/build/build_system.cpp
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
        tools/compiler/compiler_driver_frontend.cpp
        tools/compiler/compiler_driver_backend.cpp
        tools/compiler/compiler_driver_ui.cpp
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
        ${CMAKE_SOURCE_DIR}/compiler_new/include/ui
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

# ──────────────────────────────────────────────────────────────────────
# التطبيق التجريبي — ٨ واجهات / Demo App — 8 screens
# ──────────────────────────────────────────────────────────────────────
if(TARGET sad_ui_ir)
    add_executable(sad_ui_demo tools/ui_demo/ui_demo_main.cpp)

    target_include_directories(sad_ui_demo PRIVATE
        ${CMAKE_SOURCE_DIR}/compiler_new/include
        ${CMAKE_SOURCE_DIR}/compiler_new/include/ui
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
    )

    target_link_libraries(sad_ui_demo PRIVATE
        sad_ui_ir
        sad_shared
        sad_mobile
    )

    if(MSVC)
        target_compile_options(sad_ui_demo PRIVATE /wd4819 /FS)
    endif()

    set_target_properties(sad_ui_demo PROPERTIES
        OUTPUT_NAME "sad_ui_demo2"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    message(STATUS "✓ التطبيق التجريبي / Demo: sad_ui_demo (8 screens)")
endif()

# ──────────────────────────────────────────────────────────────────────
# تطبيق سطح المكتب التجريبي — SDL2 مباشر / Desktop Demo — Direct SDL2
# ──────────────────────────────────────────────────────────────────────
add_executable(sad_desktop_demo
    tools/ui_demo/sad_ui_desktop_demo.cpp
    stdlib/graphics/sad_ui_fonts_arabic.cpp
    stdlib/graphics/sad_ui_layout_render.cpp
    stdlib/graphics/sad_ui_api.cpp
    stdlib/graphics/sad_navigator.cpp
    stdlib/graphics/sad_state.cpp
    stdlib/graphics/sad_animation.cpp
)

target_include_directories(sad_desktop_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib/graphics
    ${CMAKE_SOURCE_DIR}/stdlib/graphics/third_party/SDL2/SDL2-2.28.5/include
    ${CMAKE_SOURCE_DIR}/graphics/third_party
)

target_link_libraries(sad_desktop_demo PRIVATE
    sad_graphics
)

if(MSVC)
    target_compile_options(sad_desktop_demo PRIVATE /wd4819 /FS /utf-8)
endif()

set_target_properties(sad_desktop_demo PROPERTIES
    OUTPUT_NAME "sad_desktop_demo"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# Copy SDL2.dll for desktop demo
if(EXISTS ${SDL2_DLL_PATH})
    add_custom_command(TARGET sad_desktop_demo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SDL2_DLL_PATH}" $<TARGET_FILE_DIR:sad_desktop_demo>
    )
endif()

message(STATUS "✓ تطبيق سطح المكتب / Desktop Demo: sad_desktop_demo (8 screens, SDL2)")
