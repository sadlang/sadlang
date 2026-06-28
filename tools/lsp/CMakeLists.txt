# ══════════════════════════════════════════════════════════════════════════════
# CMakeLists.txt - خادم بروتوكول اللغة الثوري للغة ص
#                  Revolutionary Language Server Protocol for Sad Language
# ══════════════════════════════════════════════════════════════════════════════
#
# بنية البناء:
# ┌─────────────────────────────────────────────────────────────────────┐
# │  sad-lsp-server  (الملف التنفيذي النهائي)                           │
# │     ├── sad_lsp_engine  (مكتبة المحرك الأساسي)                      │
# │     │     ├── sad_shared       (المحلل المعجمي + النحوي + AST)       │
# │     │     ├── sad_type_system  (SadType — تحليل الأنواع الساكن)      │
# │     │     ├── sad_formatter    (تنسيق المستندات)                    │
# │     │     └── nlohmann_json    (مكتبة JSON)                         │
# │     └── sad_lsp_transport  (طبقة النقل JSON-RPC)                    │
# └─────────────────────────────────────────────────────────────────────┘
#
# ملاحظة (م4 — RFC sadlang-rfcs#10): خادم LSP تحليلٌ ساكن، فلا يربط المفسّر
# الكامل (sad_core)؛ راجع تعليق target_link_libraries أدناه.
#
# الأهداف / Targets:
#   sad_lsp_engine    - مكتبة ثابتة تحتوي على جميع مزودات الميزات / static lib with all feature providers
#   sad-lsp-server    - الملف التنفيذي للخادم / the server executable
#
# ══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)

# ──────────────────────────────────────────────────
# (AR) مكتبة محرك LSP الأساسية — تحتوي على كل منطق التحليل والإكمال والتشخيصات
# (EN) LSP engine core library — all analysis, completion and diagnostics logic
# ──────────────────────────────────────────────────
add_library(sad_lsp_engine STATIC
    # المحرك الأساسي - التحليل وإدارة المستندات وفهرسة الرموز
    src/engine/document_store.cpp
    src/engine/analysis_pipeline.cpp
    src/engine/symbol_index.cpp
    src/engine/scope_tracker.cpp
    src/engine/lsp_engine_core.cpp

    # المزودات - كل ميزة LSP في ملف مستقل
    src/providers/completion_provider.cpp
    src/providers/diagnostics_provider.cpp
    src/providers/hover_provider.cpp
    src/providers/definition_provider.cpp
    src/providers/references_provider.cpp
    src/providers/symbols_provider.cpp
    src/providers/document_symbols_provider.cpp
    src/providers/formatting_provider.cpp
    src/providers/semantic_tokens_provider.cpp
    src/providers/signature_help_provider.cpp
    src/providers/rename_provider.cpp
    src/providers/code_actions_provider.cpp
    src/providers/inlay_hints_provider.cpp
    src/providers/folding_provider.cpp

    # ══════════════════════════════════════════════════════════════════════════
    # الميزات الثورية الجديدة - المرحلة 3 / Revolutionary features - Phase 3
    # ══════════════════════════════════════════════════════════════════════════

    # عدسات الكود - عرض معلومات تفاعلية فوق الدوال والأصناف
    src/providers/code_lens_provider.cpp

    # نطاق التحديد الذكي - توسيع التحديد بذكاء
    src/providers/selection_range_provider.cpp

    # شجرة الاستدعاءات - من يستدعي من
    src/providers/call_hierarchy_provider.cpp

    # شجرة الأنواع - الوراثة (الآباء والأبناء)
    src/providers/type_hierarchy_provider.cpp

    # روابط المستند - مسارات الاستيراد القابلة للنقر
    src/providers/document_links_provider.cpp

    # التنسيق أثناء الكتابة - تنسيق تلقائي فوري
    src/providers/on_type_formatting_provider.cpp

    # ══════════════════════════════════════════════════════════════════════════
    # كاشف المشاكل المعروفة (موصول بخطّ التحليل، جزء من القلب المجّانيّ)
    # يحذر المبرمجين من الأنماط التي تسبب مشاكل موثقة في لغة ص
    # ══════════════════════════════════════════════════════════════════════════
    #
    # ملاحظة: حُذفت المزوّدات «المتقدّمة» الخمسة (AI/أمان/أداء/أسلوب/إعادة هيكلة)
    # لأنها كانت تُبنى دون أن يستدعيها المحرّك أو طبقة النقل (معزولة)، وهي مادة
    # منتجات مدفوعة منفصلة لا جزء من خادم LSP المجّانيّ المفتوح.
    src/providers/known_issues_detector.cpp

    # أدوات مساعدة عربية / Arabic helper utilities
    src/utils/arabic_utils.cpp
    src/utils/uri_utils.cpp
)

target_include_directories(sad_lsp_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/types/generated
    ${CMAKE_SOURCE_DIR}/shared/lexer/generated
    ${CMAKE_SOURCE_DIR}/shared/utils/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/shared/builtins/include
    ${CMAKE_SOURCE_DIR}/shared/builtins/generated
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}/include
)

# (AR) م4 (RFC sadlang-rfcs#10): إعادة توصيل الأدوات إلى sad_shared الحقيقيّ.
#      خادم LSP تحليلٌ ساكن (معجم/محلّل/AST/أنواع) — لا يستعمل أيّ رمز Sad::Interpreter::
#      ⇒ يربط sad_shared (النواة) + sad_type_system (SadType) بدل sad_core (المفسّر كاملًا)،
#      فلا يُسحَب المفسّر الشجريّ + المدمجات + وقت التشغيل في ثنائيّ أداةٍ لا تحتاجها.
# (EN) Phase-4: re-wire tools to the real sad_shared. The LSP server is static analysis
#      (lexer/parser/AST/types) — uses zero Sad::Interpreter:: symbols ⇒ links sad_shared
#      + sad_type_system instead of sad_core (the whole interpreter).
target_link_libraries(sad_lsp_engine PUBLIC
    sad_shared
    sad_type_system
    sad_formatter
)

target_compile_features(sad_lsp_engine PUBLIC cxx_std_17)

if(MSVC)
    # (AR) دعم UTF-8 لنظام ويندوز / (EN) UTF-8 support on Windows
    target_compile_options(sad_lsp_engine PRIVATE /utf-8 /W4)
else()
    target_compile_options(sad_lsp_engine PRIVATE -Wall -Wextra -pedantic)
endif()

# ──────────────────────────────────────────────────
# (AR) الملف التنفيذي لخادم LSP — يحتوي على طبقة النقل JSON-RPC و main()
# (EN) LSP server executable — JSON-RPC transport layer + main()
# ──────────────────────────────────────────────────
add_executable(sad-lsp-server
    src/transport/json_rpc_transport.cpp
    src/transport/lsp_main.cpp
)

target_include_directories(sad-lsp-server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/transport
)

target_link_libraries(sad-lsp-server PRIVATE
    sad_lsp_engine
    nlohmann_json::nlohmann_json
)

if(MSVC)
    target_compile_options(sad-lsp-server PRIVATE /utf-8 /W4)
else()
    target_compile_options(sad-lsp-server PRIVATE -Wall -Wextra -pedantic)
endif()

set_target_properties(sad-lsp-server PROPERTIES
    OUTPUT_NAME "sad-lsp"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ──────────────────────────────────────────────────
# ملخص البناء / Build summary
# ──────────────────────────────────────────────────
message(STATUS "══════════════════════════════════════════════")
message(STATUS "  خادم LSP الثوري للغة ص")
message(STATUS "  Revolutionary LSP Server for Sad Language")
message(STATUS "══════════════════════════════════════════════")
message(STATUS "  المكتبة / Library: sad_lsp_engine")
message(STATUS "  التنفيذي / Executable: sad-lsp-server → sad-lsp")
message(STATUS "══════════════════════════════════════════════")
