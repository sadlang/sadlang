# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# tools/sadinfo/CMakeLists.txt
# (AR) sadinfo: أداة استخراج معلومات اللغة (كلمات، دوال مدمجة، أخطاء، توجيهات)
#      مستقلة تماماً عن المفسر والمترجم — تعتمد فقط على shared/lexer.
#      هذه هي الأداة الرسمية الوحيدة لتوليد بيانات التوثيق المنشورة في
#      الموقع وحزم npm/PyPI، لتفادي ازدواج التنفيذ بين sad و sadc.
# (EN) sadinfo: language information extractor (keywords, builtins, errors,
#      directives). Fully independent from interpreter and compiler — depends
#      only on shared/lexer. This is the SOLE official tool for generating
#      documentation data published on the website and npm/PyPI packages,
#      avoiding code duplication between sad and sadc.
# ═══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)

message(STATUS "════════════════════════════════════════════════════════")
message(STATUS "   إعداد أداة sadinfo - Configuring sadinfo")
message(STATUS "════════════════════════════════════════════════════════")

# ───────────────────────────────────────────────────────────────────────────────
# (AR) المصادر / (EN) Sources
# ───────────────────────────────────────────────────────────────────────────────
set(SADINFO_SOURCES
    sadinfo_main.cpp
    src/cli.cpp
    src/json_writer.cpp
    src/keyword_transforms.cpp   # (AR) تحويلات نقية على KeywordEntry (Story 1.2)
    src/dump_keywords.cpp
    src/dump_builtins.cpp
    src/dump_errors.cpp
    src/dump_directives.cpp
)

add_executable(sadinfo ${SADINFO_SOURCES})

target_compile_features(sadinfo PRIVATE cxx_std_17)

# ───────────────────────────────────────────────────────────────────────────────
# (AR) المسارات / (EN) Include directories
# ───────────────────────────────────────────────────────────────────────────────
target_include_directories(sadinfo PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/generated
    ${CMAKE_SOURCE_DIR}/shared/utils/include
)

# ───────────────────────────────────────────────────────────────────────────────
# (AR) الاعتمادات: فقط طبقة المعجم — لا مفسر ولا مترجم
# (EN) Dependencies: lexicon layer only — no interpreter, no compiler
# ───────────────────────────────────────────────────────────────────────────────
target_link_libraries(sadinfo PRIVATE
    sad_shared   # (AR) يوفر KeywordTable + allEntries() المولَّدة من YAML
                 # (EN) provides KeywordTable + YAML-generated allEntries()
)

# ───────────────────────────────────────────────────────────────────────────────
# (AR) دعم yaml-cpp الاختياري — يفعَّل ENABLE_YAML_CPP في cmake/yaml_cpp.cmake
# (EN) Optional yaml-cpp support — activated by ENABLE_YAML_CPP in
#      cmake/yaml_cpp.cmake. When disabled, dump_keywords falls back to the
#      built-in manual YAML writer.
# ───────────────────────────────────────────────────────────────────────────────
if(SAD_HAS_YAML_CPP AND TARGET yaml-cpp::yaml-cpp)
    target_link_libraries(sadinfo PRIVATE yaml-cpp::yaml-cpp)
    target_compile_definitions(sadinfo PRIVATE SAD_HAS_YAML_CPP=1)
    message(STATUS "  ↳ sadinfo: yaml-cpp مفعّل / yaml-cpp enabled")
elseif(SAD_HAS_YAML_CPP AND TARGET yaml-cpp)
    # (AR) بعض إصدارات yaml-cpp تكشف الاسم بدون namespace
    # (EN) Some yaml-cpp versions expose the name without namespace
    target_link_libraries(sadinfo PRIVATE yaml-cpp)
    target_compile_definitions(sadinfo PRIVATE SAD_HAS_YAML_CPP=1)
    message(STATUS "  ↳ sadinfo: yaml-cpp مفعّل (legacy target) / yaml-cpp enabled (legacy)")
else()
    message(STATUS "  ↳ sadinfo: كاتب YAML اليدوي / built-in YAML writer")
endif()

# ───────────────────────────────────────────────────────────────────────────────
# (AR) إعدادات MSVC / (EN) MSVC settings
# ───────────────────────────────────────────────────────────────────────────────
if(MSVC)
    target_compile_options(sadinfo PRIVATE /utf-8 /wd4819 /FS)
endif()

# ───────────────────────────────────────────────────────────────────────────────
# (AR) خصائص الإخراج / (EN) Output properties
# ───────────────────────────────────────────────────────────────────────────────
set_target_properties(sadinfo PROPERTIES
    OUTPUT_NAME "sadinfo"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

message(STATUS "✓ أداة معلومات اللغة / Language Info Tool: sadinfo")
