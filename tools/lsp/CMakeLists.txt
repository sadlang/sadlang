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
#   sad_lsp_engine    - مكتبة ثابتة: النواة (core) + مزوّدات الميزات المصنّفة (features)
#                       static lib: the core + the grouped feature providers
#   sad-lsp-server    - الملف التنفيذي للخادم / the server executable
#
# ══════════════════════════════════════════════════════════════════════════════

cmake_minimum_required(VERSION 3.15)

# ══════════════════════════════════════════════════════════════════════════════
# (AR) توليد أوصاف hover للكلمات المفتاحية — نظام داخليّ خاصّ بأداة LSP
#      يستهلك مصدر الحقيقة (language-truth/keywords.yaml) للتحقّق فقط ولا يعدّله؛
#      البيانات نفسها (الأوصاف) مملوكة للأداة في tools/lsp/data/keyword_docs.yaml.
#      تُولَّد الترويسة في مجلّد البناء (غير متتبَّعة) فلا تُلوِّث المصدر ولا تحتاج
#      فحص تزامن codegen. (يحقّق: الأدوات تستهلك SoT ولا توسّعه.)
# (EN) Generate keyword hover descriptions — an LSP-internal system. It CONSUMES
#      the SoT (language-truth/keywords.yaml) for validation only, never edits it;
#      the descriptions live in the tool-owned tools/lsp/data/keyword_docs.yaml.
#      The header is generated into the build tree (untracked).
# ══════════════════════════════════════════════════════════════════════════════
if(NOT Python3_FOUND)
    find_package(Python3 3.9 COMPONENTS Interpreter REQUIRED)
endif()

set(LSP_KW_DOCS_YAML   "${CMAKE_CURRENT_SOURCE_DIR}/data/keyword_docs.yaml")
set(LSP_KW_DOCS_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/scripts/gen_keyword_docs.py")
set(LSP_KW_DOCS_SOT    "${CMAKE_SOURCE_DIR}/language-truth/keywords.yaml")
set(LSP_KW_DOCS_GENDIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(LSP_KW_DOCS_HEADER "${LSP_KW_DOCS_GENDIR}/keyword_docs_generated.h")

add_custom_command(
    OUTPUT  "${LSP_KW_DOCS_HEADER}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LSP_KW_DOCS_GENDIR}"
    COMMAND ${CMAKE_COMMAND} -E env PYTHONIOENCODING=utf-8
            ${Python3_EXECUTABLE} "${LSP_KW_DOCS_SCRIPT}"
            --docs "${LSP_KW_DOCS_YAML}"
            --sot  "${LSP_KW_DOCS_SOT}"
            --out  "${LSP_KW_DOCS_HEADER}"
    DEPENDS "${LSP_KW_DOCS_YAML}" "${LSP_KW_DOCS_SCRIPT}" "${LSP_KW_DOCS_SOT}"
    COMMENT "(sad-lsp) Generating keyword hover docs (consumes SoT, tool-owned data)"
    VERBATIM
)
add_custom_target(sad_lsp_keyword_docs_codegen DEPENDS "${LSP_KW_DOCS_HEADER}")

# ──────────────────────────────────────────────────
# (AR) مكتبة محرك LSP الأساسية — تحتوي على كل منطق التحليل والإكمال والتشخيصات
# (EN) LSP engine core library — all analysis, completion and diagnostics logic
# ──────────────────────────────────────────────────
add_library(sad_lsp_engine STATIC
    # ══════════════════════════════════════════════════════════════════════════
    # النواة (core) — التحليل وإدارة المستندات وفهرسة الرموز والأدوات المساعدة
    # The core — analysis, document store, symbol index, and helper utilities
    # ══════════════════════════════════════════════════════════════════════════
    src/core/lsp_engine_core.cpp
    src/core/document_store.cpp
    src/core/analysis_pipeline.cpp
    src/core/symbol_index.cpp
    src/core/scope_tracker.cpp
    src/core/arabic_utils.cpp
    src/core/uri_utils.cpp
    src/core/sot_vocab.cpp

    # ══════════════════════════════════════════════════════════════════════════
    # المميّزات (features) — كل ميزة LSP في ملف مستقل، مصنّفة حسب الغرض
    # Features — each LSP capability in its own file, grouped by purpose
    # ══════════════════════════════════════════════════════════════════════════

    # تحرير (editing): إكمال، تواقيع، تنسيق، تنسيق أثناء الكتابة
    src/features/editing/completion_provider.cpp
    src/features/editing/signature_help_provider.cpp
    src/features/editing/formatting_provider.cpp
    src/features/editing/on_type_formatting_provider.cpp

    # فهم (comprehension): تحويم، تلوين دلاليّ، تلميحات داخليّة، عدسات الكود
    src/features/comprehension/hover_provider.cpp
    src/features/comprehension/semantic_tokens_provider.cpp
    src/features/comprehension/inlay_hints_provider.cpp
    src/features/comprehension/code_lens_provider.cpp

    # تنقّل (navigation): تعريف، مراجع، رموز، رموز المستند، روابط المستند
    src/features/navigation/definition_provider.cpp
    src/features/navigation/references_provider.cpp
    src/features/navigation/symbols_provider.cpp
    src/features/navigation/document_symbols_provider.cpp
    src/features/navigation/document_links_provider.cpp

    # بنية (structure): شجرة الاستدعاءات، شجرة الأنواع، نطاق التحديد، الطيّ
    src/features/structure/call_hierarchy_provider.cpp
    src/features/structure/type_hierarchy_provider.cpp
    src/features/structure/selection_range_provider.cpp
    src/features/structure/folding_provider.cpp

    # جودة (quality): تشخيصات، كاشف المشاكل المعروفة، إعادة تسمية، إجراءات الكود
    # ملاحظة: أيّ ميزات معزولة لا يستدعيها المحرّك ولا طبقة النقل ليست جزءًا من
    #         القلب المفتوح وتُعالَج خارج نطاق هذه المكتبة.
    src/features/quality/diagnostics_provider.cpp
    src/features/quality/known_issues_detector.cpp
    src/features/quality/rename_provider.cpp
    src/features/quality/code_actions_provider.cpp
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

# (AR) ترويسة أوصاف hover المولَّدة (نظام الأداة الداخليّ) — خاصّة بالمحرّك.
# (EN) Generated keyword-docs header (tool-internal) — private to the engine.
target_include_directories(sad_lsp_engine PRIVATE "${LSP_KW_DOCS_GENDIR}")
add_dependencies(sad_lsp_engine sad_lsp_keyword_docs_codegen)

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
