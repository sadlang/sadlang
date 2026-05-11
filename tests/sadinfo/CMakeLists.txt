# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# tests/sadinfo/CMakeLists.txt
# (AR) اختبارات وحدة لـ sadinfo (Story 1.2)
#      - test_keyword_transforms: اختبارات نقية لدوال التحويل
# (EN) Unit tests for sadinfo (Story 1.2)
#      - test_keyword_transforms: pure transformation function tests
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────────────
# (AR) test_keyword_transforms — اختبارات الدوال النقية
# ──────────────────────────────────────────────────────────────────────────────
add_executable(test_sadinfo_keyword_transforms
    test_keyword_transforms.cpp
    ${CMAKE_SOURCE_DIR}/tools/sadinfo/src/keyword_transforms.cpp
)

target_compile_features(test_sadinfo_keyword_transforms PRIVATE cxx_std_17)

target_include_directories(test_sadinfo_keyword_transforms PRIVATE
    ${CMAKE_SOURCE_DIR}/tools/sadinfo/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/generated
    ${CMAKE_SOURCE_DIR}/shared/utils/include
)

target_link_libraries(test_sadinfo_keyword_transforms PRIVATE
    sad_shared
)

if(MSVC)
    target_compile_options(test_sadinfo_keyword_transforms PRIVATE /utf-8 /wd4819 /FS)
endif()

set_target_properties(test_sadinfo_keyword_transforms PROPERTIES
    OUTPUT_NAME "test_sadinfo_keyword_transforms"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

add_test(NAME SadInfoKeywordTransforms
    COMMAND test_sadinfo_keyword_transforms
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# ──────────────────────────────────────────────────────────────────────────────
# (AR) اختبار snapshot ذهبي للـ ids — يقارن مخرج sadinfo بالملف المُسجَّل
# (EN) Golden snapshot test for stable ids — compares sadinfo output to recorded
# ──────────────────────────────────────────────────────────────────────────────
if(TARGET sadinfo)
    add_test(NAME SadInfoGoldenIds
        COMMAND ${CMAKE_COMMAND} -E env
            python "${CMAKE_SOURCE_DIR}/tests/sadinfo/check_golden_ids.py"
            "$<TARGET_FILE:sadinfo>"
            "${CMAKE_SOURCE_DIR}/tests/sadinfo/golden/keyword_ids.json"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    # ──────────────────────────────────────────────────────────────────
    # (AR) اختبار صحة YAML — يستخدم PyYAML لتحليل المخرج والتحقق من البنية
    #      يتخطى تلقائياً عند غياب PyYAML (لا يُفشل CI).
    # (EN) YAML validity test — uses PyYAML to parse and verify structure
    #      Skips automatically when PyYAML is missing (does not fail CI).
    # ──────────────────────────────────────────────────────────────────
    add_test(NAME SadInfoYamlOutput
        COMMAND ${CMAKE_COMMAND} -E env
            python "${CMAKE_SOURCE_DIR}/tests/sadinfo/check_yaml_output.py"
            "$<TARGET_FILE:sadinfo>"
            "${CMAKE_SOURCE_DIR}/tests/sadinfo/golden/keyword_ids.json"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

    # ──────────────────────────────────────────────────────────────────
    # (AR) Story 1.3: snapshot ذهبي لـ --dump-builtins (15 دالة + 43 طريقة)
    # (EN) Story 1.3: golden snapshot for --dump-builtins (15 fns + 43 methods)
    # ──────────────────────────────────────────────────────────────────
    add_test(NAME SadInfoBuiltinsGoldenIds
        COMMAND ${CMAKE_COMMAND} -E env
            python "${CMAKE_SOURCE_DIR}/tests/sadinfo/check_builtin_ids.py"
            "$<TARGET_FILE:sadinfo>"
            "${CMAKE_SOURCE_DIR}/tests/sadinfo/golden/builtin_ids.json"
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
endif()

message(STATUS "✓ اختبارات sadinfo / sadinfo tests enabled (Story 1.2)")
