# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: tools/analizer/CMakeLists.txt
# الوصف: بناء أداة ومكتبة المحلل المتقدم للغة ص
# ═══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)

set(SAD_ANALIZER_SOURCES
    src/analyzer_engine.cpp
    src/ast_analysis_visitor.cpp
)

set(SAD_ANALIZER_HEADERS
    include/analysis_types.h
    include/analyzer_engine.h
    include/ast_analysis_visitor.h
)

add_library(sad_analizer_engine STATIC
    ${SAD_ANALIZER_SOURCES}
    ${SAD_ANALIZER_HEADERS}
)

target_include_directories(sad_analizer_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/shared/utils/include
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(sad_analizer_engine PUBLIC
    sad_core
)

target_compile_features(sad_analizer_engine PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(sad_analizer_engine PRIVATE /utf-8 /W4)
else()
    target_compile_options(sad_analizer_engine PRIVATE -Wall -Wextra -pedantic)
endif()

add_executable(sad-analizer
    src/main.cpp
)

target_link_libraries(sad-analizer PRIVATE sad_analizer_engine)

target_include_directories(sad-analizer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(sad-analizer PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(sad-analizer PRIVATE /utf-8 /W4)
else()
    target_compile_options(sad-analizer PRIVATE -Wall -Wextra -pedantic)
endif()

set_target_properties(sad-analizer PROPERTIES
    OUTPUT_NAME "sad-analizer"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "✓ المحلل المتقدم / Advanced Analizer: sad-analizer")
