# cmake/wasm.cmake
# (AR) إعدادات بناء WebAssembly باستخدام Emscripten
# (EN) WebAssembly build configuration using Emscripten
#
# الاستخدام / Usage:
#   emcmake cmake -S . -B build_wasm -DBUILD_WASM=ON -DCMAKE_BUILD_TYPE=Release
#   cmake --build build_wasm --target sad_wasm
#
# المتطلبات / Requirements:
#   - Emscripten SDK 3.1+  (https://emscripten.org/docs/getting_started/downloads.html)
#   - emsdk_env.ps1 (Windows) or emsdk_env.sh (Linux/macOS) must be activated

if(NOT EMSCRIPTEN)
    message(STATUS "[wasm] Emscripten not detected — skipping sad_wasm target")
    return()
endif()

message(STATUS "[wasm] Building sad_wasm with Emscripten ${EMSCRIPTEN_VERSION}")

# ── مصادر نواة المفسر ──────────────────────────────────────────────────────
set(SAD_WASM_CORE_SOURCES
    # Lexer
    ${SAD_SOURCE_DIR}/shared/lexer/src/lexer_core.cpp
    ${SAD_SOURCE_DIR}/shared/lexer/src/lexer_keywords.cpp
    # Parser
    ${SAD_SOURCE_DIR}/shared/parser/src/parser_core.cpp
    # AST
    ${SAD_SOURCE_DIR}/shared/ast/src/ast_node.cpp
    # Types
    ${SAD_SOURCE_DIR}/shared/types/src/value.cpp
    # Interpreter core
    ${SAD_SOURCE_DIR}/interpreter_new/src/core/interpreter_core.cpp
    ${SAD_SOURCE_DIR}/interpreter_new/src/core/statement_executor.cpp
    ${SAD_SOURCE_DIR}/interpreter_new/src/core/expression_evaluator.cpp
    ${SAD_SOURCE_DIR}/interpreter_new/src/builtin/builtin_functions.cpp
    # WASM entry point
    ${SAD_SOURCE_DIR}/tools/wasm/sad_wasm.cpp
)

add_executable(sad_wasm ${SAD_WASM_CORE_SOURCES})

target_include_directories(sad_wasm PRIVATE
    ${SAD_SOURCE_DIR}/shared/lexer/include
    ${SAD_SOURCE_DIR}/shared/parser/include
    ${SAD_SOURCE_DIR}/shared/ast/include
    ${SAD_SOURCE_DIR}/shared/types/include
    ${SAD_SOURCE_DIR}/shared/errors/include
    ${SAD_SOURCE_DIR}/interpreter_new/include
    ${SAD_SOURCE_DIR}
)

# ── خيارات Emscripten ────────────────────────────────────────────────────────
set(SAD_WASM_EXPORTED_FUNCTIONS
    "_sad_execute"
    "_sad_version"
    "_malloc"
    "_free"
)

set(SAD_WASM_EXPORTED_RUNTIME
    "cwrap"
    "ccall"
    "UTF8ToString"
    "stringToUTF8"
    "lengthBytesUTF8"
)

# تحويل القوائم إلى صيغة JSON
string(REPLACE ";" "," _fns  "${SAD_WASM_EXPORTED_FUNCTIONS}")
string(REPLACE ";" "," _rt   "${SAD_WASM_EXPORTED_RUNTIME}")

set_target_properties(sad_wasm PROPERTIES
    SUFFIX ".js"
    OUTPUT_NAME "sad"
)

target_link_options(sad_wasm PRIVATE
    -O2
    -sWASM=1
    -sALLOW_MEMORY_GROWTH=1
    -sMAXIMUM_MEMORY=256MB
    -sMODULARIZE=1
    -sEXPORT_NAME=SadWasm
    "-sEXPORTED_FUNCTIONS=[${_fns}]"
    "-sEXPORTED_RUNTIME_METHODS=[${_rt}]"
    -sFILESYSTEM=0
    -sDYNAMIC_EXECUTION=0
    --no-entry
)

# ── نسخ الناتج إلى مجلد الموقع ───────────────────────────────────────────
set(WASM_OUTPUT_DIR "${SAD_SOURCE_DIR}/website/docs/public")
add_custom_command(TARGET sad_wasm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${WASM_OUTPUT_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:sad_wasm>"           "${WASM_OUTPUT_DIR}/sad.js"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:sad_wasm>/sad.wasm" "${WASM_OUTPUT_DIR}/sad.wasm"
    COMMENT "[wasm] Copied sad.js + sad.wasm → website/docs/public/"
    VERBATIM
)

message(STATUS "[wasm] Target: sad_wasm → ${WASM_OUTPUT_DIR}/sad.js + sad.wasm")
