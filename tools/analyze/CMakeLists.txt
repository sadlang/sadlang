# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف / File: tools/analyze/CMakeLists.txt
# (AR) الوصف: بناء أداة ومكتبة المحلل المتقدم للغة ص
# (EN) Build the Sad advanced analyzer library + CLI (sad-analyze)
# ═══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)

set(SAD_ANALYZE_SOURCES
    src/analyzer_engine.cpp
    src/ast_analysis_visitor.cpp
)

set(SAD_ANALYZE_HEADERS
    include/analysis_types.h
    include/analyzer_engine.h
    include/ast_analysis_visitor.h
)

# (AR) المكتبة الساكنة: محرّك التحليل (يستهلكها sad-analyze والاختبارات)
# (EN) Static library: analyzer engine (consumed by sad-analyze and tests)
add_library(sad_analyze_engine STATIC
    ${SAD_ANALYZE_SOURCES}
    ${SAD_ANALYZE_HEADERS}
)

target_include_directories(sad_analyze_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/shared/utils/include
    ${CMAKE_SOURCE_DIR}/include
)

# (AR) م4 (RFC sadlang-rfcs#10): أداة التحليل الساكن لا تستعمل رموز المفسّر ⇒ تربط
#      sad_shared + sad_type_system بدل sad_core (المفسّر الكامل).
# (EN) Phase-4: static-analysis tool uses no interpreter symbols ⇒ links sad_shared
#      + sad_type_system instead of sad_core.
target_link_libraries(sad_analyze_engine PUBLIC
    sad_shared
    sad_type_system
)

target_compile_features(sad_analyze_engine PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(sad_analyze_engine PRIVATE /utf-8 /W4)
else()
    target_compile_options(sad_analyze_engine PRIVATE -Wall -Wextra -pedantic)
endif()

# (AR) التنفيذي: واجهة سطر الأوامر sad-analyze
# (EN) Executable: the sad-analyze CLI
add_executable(sad-analyze
    src/main.cpp
)

target_link_libraries(sad-analyze PRIVATE sad_analyze_engine)

target_include_directories(sad-analyze PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# (AR) PRIVATE: لا أحد يربط التنفيذيّ، فلا داعي لنشر الميزة عامًّا
# (EN) PRIVATE: nothing links the executable, so no need to propagate the feature
target_compile_features(sad-analyze PRIVATE cxx_std_17)

if(MSVC)
    target_compile_options(sad-analyze PRIVATE /utf-8 /W4)
else()
    target_compile_options(sad-analyze PRIVATE -Wall -Wextra -pedantic)
endif()

set_target_properties(sad-analyze PROPERTIES
    OUTPUT_NAME "sad-analyze"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

message(STATUS "✓ المحلل المتقدم / Advanced Analyzer: sad-analyze")
