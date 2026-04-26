# ══════════════════════════════════════════════════════════════════════════════
# CMakeLists.txt - خادم بروتوكول اللغة الثوري للغة ص
# ══════════════════════════════════════════════════════════════════════════════
#
# بنية البناء:
# ┌─────────────────────────────────────────────────────────────────────┐
# │  sad-lsp-server  (الملف التنفيذي النهائي)                           │
# │     ├── sad_lsp_engine  (مكتبة المحرك الأساسي)                      │
# │     │     ├── sad_core  (المحلل المعجمي + النحوي + AST)              │
# │     │     └── nlohmann_json  (مكتبة JSON)                           │
# │     └── sad_lsp_transport  (طبقة النقل JSON-RPC)                    │
# └─────────────────────────────────────────────────────────────────────┘
#
# الأهداف:
#   sad_lsp_engine    - مكتبة ثابتة تحتوي على جميع مزودات الميزات
#   sad-lsp-server    - الملف التنفيذي للخادم
#
# ══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)

# ──────────────────────────────────────────────────
# مكتبة محرك LSP الأساسية
# تحتوي على كل منطق التحليل والإكمال والتشخيصات
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
    # الميزات الثورية الجديدة - المرحلة ٣
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
    # المزودات الثورية المتقدمة - تحسينات متقدمة لـ LSP
    # ══════════════════════════════════════════════════════════════════════════
    
    # نظام المساعد الذكي بالذكاء الاصطناعي
    # يوفر اقتراحات كود ذكية وإكمال سياقي متقدم
    src/providers/ai_copilot_provider.cpp
    
    # محلل الأداء الفوري
    # يكتشف مشاكل الأداء ويقترح تحسينات
    src/providers/performance_analyzer_provider.cpp
    
    # محلل الأمان
    # يفحص الكود بحثاً عن ثغرات أمنية
    src/providers/security_analyzer_provider.cpp
    
    # مدقق أنماط الكود والممارسات الأفضل
    # يتحقق من اتباع معايير الكتابة الجيدة
    src/providers/style_checker_provider.cpp
    
    # مساعد إعادة الهيكلة الذكي
    # يوفر عمليات إعادة هيكلة آمنة وذكية
    src/providers/refactoring_provider.cpp
    
    # كاشف المشاكل المعروفة
    # يحذر المبرمجين من الأنماط التي تسبب مشاكل موثقة
    src/providers/known_issues_detector.cpp

    # أدوات مساعدة عربية
    src/utils/arabic_utils.cpp
    src/utils/uri_utils.cpp
)

target_include_directories(sad_lsp_engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/utils/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(sad_lsp_engine PUBLIC
    sad_core
    sad_formatter
)

target_compile_features(sad_lsp_engine PUBLIC cxx_std_17)

if(MSVC)
    # دعم UTF-8 لنظام ويندوز
    target_compile_options(sad_lsp_engine PRIVATE /utf-8 /W4)
else()
    target_compile_options(sad_lsp_engine PRIVATE -Wall -Wextra -pedantic)
endif()

# ──────────────────────────────────────────────────
# الملف التنفيذي لخادم LSP
# يحتوي على طبقة النقل JSON-RPC و main()
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
# ملخص البناء
# ──────────────────────────────────────────────────
message(STATUS "══════════════════════════════════════════════")
message(STATUS "  خادم LSP الثوري للغة ص")
message(STATUS "  Revolutionary LSP Server for Sad Language")
message(STATUS "══════════════════════════════════════════════")
message(STATUS "  المكتبة: sad_lsp_engine")
message(STATUS "  التنفيذي: sad-lsp-server → sad-lsp")
message(STATUS "══════════════════════════════════════════════")
