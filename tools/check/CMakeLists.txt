# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# tools/check/CMakeLists.txt
# (AR) sad-check: فاحص الملكية والأنواع الساكن — يربط sad_ownership + sad_shared
#      (معجم/محلّل/AST) + sad_semantic_shared (فاحص الأنواع المشترك)؛ لا مفسّر/مترجم.
# (EN) sad-check: static ownership/type checker — links sad_ownership + sad_shared
#      (lexer/parser/AST) + sad_semantic_shared (shared type checker); no interpreter/compiler.
# ═══════════════════════════════════════════════════════════════════════════════

add_executable(sad-check src/main.cpp)

target_link_libraries(sad-check PRIVATE
    sad_ownership          # (AR) فحص الملكية الموحَّد / (EN) unified ownership check
    sad_shared             # (AR) lexer + parser + ast / (EN) lexer + parser + ast
    sad_semantic_shared    # (AR) فاحص الأنواع المشترك / (EN) shared type checker
)

target_include_directories(sad-check PRIVATE
    ${CMAKE_SOURCE_DIR}/tools/hub/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/shared/ownership/include
    ${CMAKE_SOURCE_DIR}/shared/semantic/include
    ${CMAKE_SOURCE_DIR}/shared/utils/include
    ${CMAKE_SOURCE_DIR}/shared/modules/include
)

set_target_properties(sad-check PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

if(MSVC)
    target_compile_options(sad-check PRIVATE /utf-8 /wd4819 /FS)
endif()

target_compile_features(sad-check PRIVATE cxx_std_17)

message(STATUS "✓ فاحص الملكية / Ownership Checker: sad-check")
