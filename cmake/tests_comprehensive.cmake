# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/tests_comprehensive.cmake
# الوصف: الاختبارات الشاملة (900+ اختبار) / Comprehensive Test Suite (900+ tests)
# ═══════════════════════════════════════════════════════════════════════════════

if(NOT BUILD_TESTS)
    return()
endif()

message(STATUS "════════════════════════════════════════════════════════")
message(STATUS "  إعداد الاختبارات الشاملة / Setting up Comprehensive Tests")
message(STATUS "════════════════════════════════════════════════════════")

# ──────────────────────────────────────────────────────────────────────
# ماكرو لإنشاء اختبار شامل / Macro to create a comprehensive test
# ──────────────────────────────────────────────────────────────────────
macro(add_comprehensive_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME}
        ${CMAKE_SOURCE_DIR}/tests/unit/comprehensive/${TEST_SOURCE}
    )

    target_link_libraries(${TEST_NAME} PRIVATE sad_shared)

    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/tests/unit/comprehensive
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
        ${CMAKE_SOURCE_DIR}/shared/modules/include
        ${CMAKE_SOURCE_DIR}/shared/utils/include
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include/parser
        ${CMAKE_SOURCE_DIR}/include/parser/ast
        ${CMAKE_SOURCE_DIR}/include/lexer
        ${CMAKE_SOURCE_DIR}/include/data/types
        ${CMAKE_SOURCE_DIR}/include/errors
        ${CMAKE_SOURCE_DIR}/include/modules
        ${CMAKE_SOURCE_DIR}/include/utils
    )

    if(MSVC)
        target_compile_options(${TEST_NAME} PRIVATE /W3 /utf-8 /FS /Zc:preprocessor)
    else()
        target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra)
    endif()

    target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)

    add_test(NAME "Comprehensive_${TEST_NAME}" COMMAND ${TEST_NAME})
    # (AR) TEST-004 AC-02: وسم Unit — تشغيل طبقة الوحدة كاملة بـ ctest -L Unit
    # (EN) TEST-004 AC-02: Unit label — run the whole unit layer via ctest -L Unit
    set_tests_properties("Comprehensive_${TEST_NAME}" PROPERTIES TIMEOUT 120 LABELS "Unit")

    message(STATUS "  ✅ ${TEST_NAME}")
endmacro()

# ──────────────────────────────────────────────────────────────────────
# إنشاء جميع الاختبارات الشاملة / Create all comprehensive tests
# ──────────────────────────────────────────────────────────────────────

# 1. المحلل المعجمي / Lexer Tests (96 tests)
add_comprehensive_test(test_lexer_comprehensive test_lexer_comprehensive.cpp)

# 1.b. اختبارات بنية v4.1 — YAML SoT / v4.1 keywords-as-data tests
add_comprehensive_test(test_keywords_v41_comprehensive test_keywords_v41_comprehensive.cpp)

# 2. المحلل النحوي / Parser Tests (87 tests)
add_comprehensive_test(test_parser_comprehensive test_parser_comprehensive.cpp)

# 3. نظام القيم / Value System Tests (95 tests)
add_comprehensive_test(test_value_comprehensive test_value_comprehensive.cpp)

# 3.b. عقدُ الحاويةِ المرتّبةِ بالإدخال / Insertion-ordered map contract (ISSUE-180)
# (AR) تُختبَرُ في طبقتِها لا عبرَ المفسّر: العطبُ الذي وقعَ فيها فعلًا (فهرسُ
#      مُكرِّراتٍ يُنسَخُ فيُشيرُ إلى عُقَدِ الأصل) لم يظهرْ إلّا تعليقًا صامتًا
#      في المفسّرِ بعد بناءٍ كامل.
add_comprehensive_test(test_insertion_ordered_map_comprehensive test_insertion_ordered_map_comprehensive.cpp)

# (AR) 🔑 حُذف `test_interpreter_comprehensive` وأنصابُه مع المفسّر.

# 5. المكتبة القياسية / Standard Library Tests (90 tests)
add_comprehensive_test(test_stdlib_comprehensive test_stdlib_comprehensive.cpp)
target_sources(test_stdlib_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/shared/builtins/src/runtime/array_functions.cpp
    ${CMAKE_SOURCE_DIR}/shared/builtins/src/runtime/type_functions.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/io/io_functions.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/string/string_functions.cpp
    ${CMAKE_SOURCE_DIR}/stdlib/math/math_functions.cpp)
target_include_directories(test_stdlib_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib ${CMAKE_SOURCE_DIR}/shared/builtins/include/runtime
    ${CMAKE_SOURCE_DIR}/stdlib/string ${CMAKE_SOURCE_DIR}/stdlib/math)

# 6. الأخطاء الذكية / Smart Errors Tests (38 tests)
add_comprehensive_test(test_errors_comprehensive test_errors_comprehensive.cpp)

# 6.5. استنساخ عُقد AST / AST Clone Tests (18 tests)
add_comprehensive_test(test_ast_clone test_ast_clone.cpp)

# 6.6. مستخرج التوثيق / Documentation Extractor Tests (28 tests)
add_comprehensive_test(test_docs_extractor_comprehensive test_docs_extractor_comprehensive.cpp)

# 8. اختبارات التكامل / End-to-End Tests (47 tests)
add_comprehensive_test(test_e2e_comprehensive test_e2e_comprehensive.cpp)

# 9. الأدوات ونظام الوحدات / Utils & Modules Tests (95 tests)
add_comprehensive_test(test_utils_modules_comprehensive test_utils_modules_comprehensive.cpp)

# 10. المترجم و SIR / Compiler & SIR Tests (100 tests)
add_comprehensive_test(test_compiler_comprehensive test_compiler_comprehensive.cpp)
target_include_directories(test_compiler_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/vm/include
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}/compiler/include/frontend
    ${CMAKE_SOURCE_DIR}/compiler/include/backend)

# (AR) 🔑 حُذف `test_throw_catch_comprehensive` مع المفسّر (كان يضمُّ ترويساتِه وأنصابَه).

# (AR) 🔑 حُذف `test_optional_null_comprehensive` مع المفسّر (كان يضمُّ ترويساتِه وأنصابَه).

# (AR) 🔑 حُذف `test_regression_comprehensive`: كان يستدعي `sad-run` عبر
#      SAD_EXE_PATH. تغطيةُ `tests/behavior/_regression` باقيةٌ عبر العدّاء
#      (dual_tests.cmake) وقد صار مترجِمًا فقط.

# ──────────────────────────────────────────────────────────────────────
# 13. اختبارات وحدات المكتبة القياسية المتقدمة / Advanced Stdlib Module Tests (Sprint 1 — ريم)
# ──────────────────────────────────────────────────────────────────────

# 13a. الشبكات / Network Module Tests
add_comprehensive_test(test_network_comprehensive test_network_comprehensive.cpp)
target_include_directories(test_network_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/features/network/core
    ${CMAKE_SOURCE_DIR}/features/network/core/include
    ${CMAKE_SOURCE_DIR}/features/network/core/include/network
    ${CMAKE_SOURCE_DIR}/features/network/core/include/socket
    ${CMAKE_SOURCE_DIR}/features/network/core/include/http
    ${CMAKE_SOURCE_DIR}/features/network/core/include/websocket)
target_sources(test_network_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/features/network/core/src/network/network_error.cpp
    ${CMAKE_SOURCE_DIR}/features/network/core/src/socket/socket_address.cpp
    ${CMAKE_SOURCE_DIR}/features/network/core/src/http/http_base.cpp
    ${CMAKE_SOURCE_DIR}/features/network/core/src/http/http_request.cpp
    ${CMAKE_SOURCE_DIR}/features/network/core/src/http/http_response.cpp
    ${CMAKE_SOURCE_DIR}/features/network/core/src/http/http_cookie.cpp
    ${CMAKE_SOURCE_DIR}/features/network/core/src/http/http_url.cpp)
if(WIN32)
    target_link_libraries(test_network_comprehensive PRIVATE ws2_32)
    target_compile_definitions(test_network_comprehensive PRIVATE WIN32_LEAN_AND_MEAN)
endif()

# 13b. النظام / System Module Tests
add_comprehensive_test(test_system_comprehensive test_system_comprehensive.cpp)
target_include_directories(test_system_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib)
target_sources(test_system_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib/system/system_functions.cpp)

# 13d. النصوص العربية / Arabic Text Module Tests
add_comprehensive_test(test_text_comprehensive test_text_comprehensive.cpp)
target_include_directories(test_text_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib)

# 13e. الصور / Image Module Tests (header-only — stb_image_write.h unavailable)
add_comprehensive_test(test_image_comprehensive test_image_comprehensive.cpp)
target_include_directories(test_image_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/stdlib)

# ──────────────────────────────────────────────────────────────────────
# 14. اختبارات الواجهة الرسومية + المترجم SIR / UI Widget + Compiler SIR Tests
# ──────────────────────────────────────────────────────────────────────
# (AR) مُعطَّل: test_ui_comprehensive.cpp كود ميّت لا يُترجَم — يضمّ مجلدًا محذوفًا
#      (compiler/src/sir/ غير موجود) ورأسًا محذوفًا (sir_opcodes.h) ويستعمل قيم
#      تعداد UI أُزيلت (Opcode::UICreateWidget… غير معرّفة إطلاقًا) عند تقليص كتالوج
#      SadUI. لا يمكن إصلاحه لأنه يختبر وظائف أُزيلت عمدًا؛ يُعاد تفعيله فقط إن
#      أُعيدت أوبكودات UI في SIR. (كان يكسر بناء الاختبارات على كل المنصّات.)
# (EN) Disabled: test_ui_comprehensive.cpp is uncompilable dead code — it includes
#      a deleted dir (compiler/src/sir/) and header (sir_opcodes.h) and uses removed
#      UI opcode enum values (Opcode::UICreateWidget… undefined). It tests removed
#      functionality, so it is disabled until UI SIR opcodes are reintroduced.
# add_comprehensive_test(test_ui_comprehensive test_ui_comprehensive.cpp)
# target_link_libraries(test_ui_comprehensive PRIVATE sad_graphics)
# target_include_directories(test_ui_comprehensive PRIVATE
#     ${CMAKE_SOURCE_DIR}/compiler/src/sir
#     ${CMAKE_SOURCE_DIR}/compiler/src
#     ${CMAKE_SOURCE_DIR}/features/graphics/core/include)

# 14a. اختبارات قبول المرحلة 0: Parser UI (إذا/وإلا + لكل + knownWidgets)
# Phase 0 Acceptance Tests: UI Parser (conditional rendering + loops + widget registry)
add_comprehensive_test(test_ui_phase0 test_ui_phase0.cpp)

# 14c. اختبارات قبول المرحلة 1: النظام التفاعلي (BindingCompiler)
# Phase 1 Acceptance Tests: Reactive System (BindingCompiler)
add_comprehensive_test(test_ui_phase1 test_ui_phase1.cpp)
target_link_libraries(test_ui_phase1 PRIVATE sad_graphics)
target_include_directories(test_ui_phase1 PRIVATE
    ${CMAKE_SOURCE_DIR}/features/graphics/core/include
    ${CMAKE_SOURCE_DIR}/features/graphics/reactive
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
)

# 14a-bis. اختبارات السلسلة التفاعلية لـ@حالة / Reactive @state chain Tests (STORY-UI-W15-03)
# تختبر UIStateManager مباشرة: تسجيل → مراقبة → تصفية → batching → flush
# (AR) نربط sad_core للتنفيذ الحقيقي لـUIStateManager — بدون interpreter_test_stubs
#      لأن تلك البدائل تُبطل (no-op) دوال UIStateManager وتُفشل اختبار التفاعلية.
# (EN) Link the REAL UIStateManager from sad_core — NOT interpreter_test_stubs,
#      whose no-op overrides would shadow the implementation under test.
# (AR) 🔑 كلُّ مسارات تضمينِ هذا الهدفِ كانت شجرةَ المفسّر — زالت معها.

# (AR) 🔑 حُذف `test_ui_reactive_state`: موضوعُه `UIStateManager` الخاصُّ
#      بالمفسّر (`interpreter/src/ui/ui_state_manager.cpp`) — والتعليقُ
#      الأصليُّ كان يشترطُ «المُدير الحقيقيّ من sad_core لا الأنصاب».
#      زالَ الموضوعُ فزالَ الاختبار؛ ولا نظيرَ له في المترجم.
# (EN) Removed: its subject was the interpreter's UIStateManager.
# 14a-ter. خفض «واجهة» إلى SIR في المترجم / UI -> SIR lowering (STORY-UI-W15-04)
# (AR) يثبت أن SIRBuilder يخفض UIDeclarationNode إلى ClassDecl (Phase 0) فيظهر
#      صنف SIR بحقول @حالة ودواله مع احترام «يرث». يُربط بـsad_core + sad_frontend.
# (EN) Verifies SIRBuilder lowers UIDeclarationNode -> ClassDecl (Phase 0):
#      a SIR class with @state fields + methods + يرث inheritance.
add_comprehensive_test(test_ui_sir_lowering test_ui_sir_lowering.cpp)
target_link_libraries(test_ui_sir_lowering PRIVATE sad_shared sad_frontend sad_builtins sad_lowlevel sad_type_system)
target_include_directories(test_ui_sir_lowering PRIVATE
    ${CMAKE_SOURCE_DIR}/compiler/include/frontend
    ${CMAKE_SOURCE_DIR}/compiler/include)

# 14b. اختبارات منصات الرسومات الشاملة / Comprehensive Backend Tests (IR + Pipeline + Codegen)
add_comprehensive_test(test_backends_comprehensive test_backends_comprehensive.cpp)
target_link_libraries(test_backends_comprehensive PRIVATE sad_graphics)
target_include_directories(test_backends_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/features/graphics/core/include
    ${CMAKE_SOURCE_DIR}/features/graphics/pipeline/include
    ${CMAKE_SOURCE_DIR}/features/graphics/widgets/include
    ${CMAKE_SOURCE_DIR}/features/graphics/backends/desktop/include
    ${CMAKE_SOURCE_DIR}/features/graphics/backends/android/include
    ${CMAKE_SOURCE_DIR}/features/graphics/backends/ios/include
    ${CMAKE_SOURCE_DIR}/features/graphics/backends/web/include
    ${CMAKE_SOURCE_DIR}/features/graphics/backends/macos/include)

# 14c. اختبارات نظام الأحداث الموحد / Unified Event System Tests (IREventType)
add_comprehensive_test(test_event_system_comprehensive test_event_system_comprehensive.cpp)
target_link_libraries(test_event_system_comprehensive PRIVATE sad_graphics)
target_include_directories(test_event_system_comprehensive PRIVATE
    ${CMAKE_SOURCE_DIR}/features/graphics/core/include
    ${CMAKE_SOURCE_DIR}/features/graphics/pipeline/include)

# 14d. اختبارات أداء Reconciler ونظام الأحداث المستقرة / Reconciler Performance Tests
add_comprehensive_test(test_reconciler_performance test_reconciler_performance.cpp)
target_link_libraries(test_reconciler_performance PRIVATE sad_graphics)
target_include_directories(test_reconciler_performance PRIVATE
    ${CMAKE_SOURCE_DIR}/features/graphics/core/include
    ${CMAKE_SOURCE_DIR}/features/graphics/pipeline/include)

# 14e. اختبار ذهبيّ لطبقة التخطيط + الرسم عبر RecordingRenderer (headless حتميّ)
# (AR) يمرّر أشجار IR عبر layout() + render() بمُصيّر يسجّل نداءات الرسم نصّيًّا،
#      فيغطّي المنطق المشترك أسفل شجرة IR الذي لا تمسّه طباعة_شجرة — بلا SDL2/بكسل.
# (EN) Golden test of the shared layout()+render() logic below the IR tree via a
#      draw-call-recording renderer — no SDL2/pixels, fully deterministic for CI.
add_comprehensive_test(test_ui_render_displaylist test_ui_render_displaylist.cpp)
target_link_libraries(test_ui_render_displaylist PRIVATE sad_graphics)
target_include_directories(test_ui_render_displaylist PRIVATE
    ${CMAKE_SOURCE_DIR}/features/graphics/core/include)

# 14e-1. بوّابة البرهان البصريّ — لقطات بكسل ذهبيّة (headless حتميّ)
# (AR) الاختبار أعلاه يقيس **نداءات الرسم** ويُعلن حدّه: لا بكسل يُقاس. هذا
#      ينقّط الأشجار نفسها إلى بكسلات فعليّة عبر المُصيّر الحرّ — رسّام برمجيّ
#      صرف بخطّ نقطيّ **مورَّد في المستودع** وتشكيل عربيّ لنا، بلا تنعيم حواف.
#      الهدف يربط sad_graphics (ومعها SDL2 بوصفها PUBLIC)، لكنّه **لا يفتح
#      نافذة ولا سياق عرض ولا خطّ نظام** — فلا يلزمه xvfb ولا عتاد رسوميّ،
#      والتنقيط كلّه حساب محلّيّ. (لقطة SDL2 نفسها مستحيلة حتميًّا: خطّ النظام
#      يختلف بين المنصّات، والرسّام مسرَّع عتاديًّا، والنافذة مفروضة الظهور.)
# (EN) Pixel-level golden gate via the freestanding software rasterizer: vendored
#      bitmap font, no antialiasing, no GPU, no system font. It links SDL2
#      transitively but never opens a window or a render context, so no xvfb.
add_comprehensive_test(test_ui_render_pixels test_ui_render_pixels.cpp)
target_link_libraries(test_ui_render_pixels PRIVATE sad_graphics)
target_include_directories(test_ui_render_pixels PRIVATE
    ${CMAKE_SOURCE_DIR}/features/graphics/core/include
    ${CMAKE_SOURCE_DIR}/features/graphics/backends/freestanding/include)
# (AR) مجلّد اللقطات يُحقَن مسارًا مطلقًا: الاختبار يُشغَّل من مجلّدات عملٍ
#      مختلفة (ctest، البوّابة، اليد) فمسارٌ نسبيّ يعني إخفاقًا رهين المكان.
target_compile_definitions(test_ui_render_pixels PRIVATE
    SAD_UI_PIXEL_GOLDEN_DIR="${CMAKE_SOURCE_DIR}/tests/unit/comprehensive/golden/pixels")
# (AR) دمجُ الضربِ والجمعِ (FMA) مُفعَّلٌ افتراضًا في clang، ويعطي نتيجةً تختلفُ
#      بأدنى وحدةٍ عن الحسابِ غيرِ المدموج. حسابُ التخطيطِ عائمٌ ثمّ يُقتطَعُ إلى
#      بكسلٍ صحيح، فاختلافُ ULP عند حدٍّ تامٍّ يُزيحُ حافّةً كاملة ⇒ لقطةٌ تُخفِقُ
#      على معماريّةِ آرم وتنجحُ على x86 بلا تغييرٍ في الكود. المنعُ يجعلُ
#      الحتميّةَ مضمونةً بالبناءِ لا متوقّعةً منه. (MSVC لا يدمجُ افتراضًا.)
# (EN) clang contracts a*b+c into FMA by default; layout math is float then
#      truncated to int pixels, so a one-ULP difference at an exact boundary
#      shifts a whole edge — green on x86, red on arm64, with no code change.
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(test_ui_render_pixels PRIVATE -ffp-contract=off)
endif()

# 14e-2. حارس دلالة السحب المستمرّ (FR-009) — يقفل عقد OnDrag المشترك بين
# (AR) كلّ المنصّات: إطلاق مستمرّ بدلتا تفاضليّة على العنصر المُمسَك، بلا إعادة
#      إطلاق إجماليّ عند الرفع ولا سحب شبحيّ. C++ مستضاف بلا SDL2/عتاد.
# (EN) Regression guard for the shared continuous-drag contract (incremental
#      deltas, grabbed-node identity, no total-delta replay, no ghost drag).
add_comprehensive_test(test_ui_drag_semantics test_ui_drag_semantics.cpp)
target_link_libraries(test_ui_drag_semantics PRIVATE sad_graphics)
target_include_directories(test_ui_drag_semantics PRIVATE
    ${CMAKE_SOURCE_DIR}/features/graphics/core/include)

# 14e-2. اختبار قلب إرسال الأحداث المشترك — حارس انحدار لـ dispatchEvent
# (AR) الطور مُعلَن على المعالِج (افتراضه None ⇒ صفر انحدار)، طور الهدف يُطلق كلّ
#      المطابقات (توحيد المفسّر/المترجم)، وترتيب التقاط←هدف←فقاعات مع فحص
#      إيقاف الانتشار بين كلّ معالِجَين. C++ مستضاف بلا عتاد.
# (EN) Regression guard for the shared 3-phase event dispatcher.
add_comprehensive_test(test_ui_event_dispatch test_ui_event_dispatch.cpp)
target_link_libraries(test_ui_event_dispatch PRIVATE sad_graphics)
target_include_directories(test_ui_event_dispatch PRIVATE
    ${CMAKE_SOURCE_DIR}/features/graphics/core/include)

# 14f. اختبار المُشكِّل العربيّ للوضع الحرّ — حارس انحدار لـ arabic_shaper (٢٠ حالة)
# (AR) C++ خالص مستضاف: يجمع arabic_shaper.cpp + الاختبار مباشرةً بلا SDL2/fb0/عتاد،
#      فيُبنى على كلّ منصّات CI. كان يُشغَّل يدويًّا فقط عبر «بناء-عرض-fb.sh» — الآن
#      مربوط بـCTest (وسم Unit) كي يشغّله CI حارسًا للانحدار (فشل ذرّيّ: خروج ≠ 0).
# (EN) Pure hosted C++: compiles arabic_shaper.cpp + the test directly (no SDL2/fb0/
#      hardware), so it builds on every CI platform. Previously only run by hand via
#      «بناء-عرض-fb.sh»; now wired into CTest (Unit label) as a CI regression guard.
add_executable(test_arabic_shaper_freestanding
    ${CMAKE_SOURCE_DIR}/features/graphics/backends/freestanding/tools/arabic_shaper_test.cpp
    ${CMAKE_SOURCE_DIR}/features/graphics/backends/freestanding/src/arabic_shaper.cpp)
target_include_directories(test_arabic_shaper_freestanding PRIVATE
    ${CMAKE_SOURCE_DIR}/features/graphics/backends/freestanding/include)
target_compile_features(test_arabic_shaper_freestanding PRIVATE cxx_std_17)
if(MSVC)
    target_compile_options(test_arabic_shaper_freestanding PRIVATE /W3 /utf-8 /FS)
else()
    target_compile_options(test_arabic_shaper_freestanding PRIVATE -Wall -Wextra)
endif()
add_test(NAME "Comprehensive_test_arabic_shaper_freestanding"
         COMMAND test_arabic_shaper_freestanding)
set_tests_properties("Comprehensive_test_arabic_shaper_freestanding"
    PROPERTIES TIMEOUT 60 LABELS "Unit")
message(STATUS "  ✅ test_arabic_shaper_freestanding")

# (AR) 🔑 حُذف `test_applet_normalize`: موضوعُه موزِّعُ آبلتاتِ `sad-repl`
#      ومصدرُه المولَّد `repl_sot_generated.cpp` — والأداةُ ومصدرُ حقيقتِها
#      زالا مع المفسّر.
# (EN) Removed: its subject (the sad-repl applet dispatcher and its generated
#      SoT) went with the interpreter.

# ──────────────────────────────────────────────────────────────────────
# اختبارات .ص فردية مباشرة عبر CTest / Individual .ص CTest entries
# ──────────────────────────────────────────────────────────────────────
# تسجيل كل ملف .ص كاختبار CTest مستقل — يسهل التشخيص / Register each .ص as standalone CTest
set(REGRESSION_TEST_DIR "${CMAKE_SOURCE_DIR}/tests/behavior/_regression")

# P0 — عاجل / Critical
set(REGRESSION_P0_TESTS
    test_p01_array_key_access
    test_p02_while_update
    test_p03_length_depth
    test_p09_reserved_words
    test_p10_void_return
    test_p11_custom_class_fields
    test_p12_integer_division
    test_p16_compound_field
    test_p18_this_method_save
    test_p19_hex_literals
)

# P1 — مهم / Important
set(REGRESSION_P1_TESTS
    test_p04_hamza_var
    test_p08_break
    test_p13_class_prefix
    test_p14_cascading_errors
    test_p17_undocumented_reserved
    test_p20_super_reserved
    test_p24_json_unified_builtin
    test_p25_xml_unified_builtin
    test_p26_json_invalid_input
    test_p27_xml_invalid_input
    test_p28_filesystem_smoke
    test_p29_filesystem_invalid_input
    test_p30_database_availability
    test_p31_json_unicode_parse
    test_p32_json_unicode_stringify
    test_p33_operator_overloading
    test_p34_conversion_operators
    test_p35_print_operator
)

# P2 — تحسين / Enhancement
set(REGRESSION_P2_TESTS
    test_p05_mixed_chars
    test_p06_large_numbers
    test_p07_array_pluseq
    test_p15_utf8_windows
    test_p21_template_reserved
    test_p22_true_literal
)

# (AR) 🔑 حُذفت حلقةُ `add_test(... $<TARGET_FILE:sad-run>)`: استدعاءٌ مباشرٌ
#      للمفسّرِ يتخطّى العدّاء. البذورُ نفسُها تُشغَّلُ عبر dual_tests.cmake.

# (AR) 🔑 نُزِعَ ضبطُ مهلةِ "Regression_test_p02_while_update": الاختبارُ نفسُه
#      لم يعُدْ يُسجَّل (حلقةُ `add_test` أعلاه حُذفت مع المفسّر)، فكان الضبطُ
#      يقعُ على اسمٍ لا وجودَ له داخلَ `if(TARGET sad-run)` — فلا يُنفَّذُ ولا
#      يُخفِق. وحمايةُ الحلقةِ اللانهائيّةِ باقيةٌ عبر مهلةِ العدّاء (`--timeout`).
# (EN) Removed: the test it configured is no longer registered, so the property
#      sat on a nonexistent name inside a dead TARGET guard. Infinite-loop
#      protection remains via the runner's own timeout.

# ──────────────────────────────────────────────────────────────────────
# اختبارات السلسلة الاختياريّة ?. ودمج العدم ?? عبر ملفات .ص / Optional Chain & Null Coalesce .ص tests
# ──────────────────────────────────────────────────────────────────────
# (AR) 🔑 حُذفت حلقةُ `OptionalNull_*`: كانت تُشغّلُ بذورَ `?.`/`??` بـ`sad-run`
#      مباشرةً. وبزوالِ الهدفِ صارت تسقطُ **صامتةً** داخلَ `if(TARGET sad-run)` —
#      وسقوطٌ لا صوتَ له أسوأُ من إخفاق. والتغطيةُ لم تُفقَد: البذرتان تسكنانِ
#      `tests/behavior/_regression` ويحكمُ عليهما العدّاءُ بالمترجّم. وقد أظهرَ
#      رفعُ الغطاءِ حمرةً كانت مخبوءةً: `test_optional_null_advanced.ص` يُخفِقُ
#      بـ«Failed to build if condition» — مُقيَّدةٌ في `DECLARED_REDS.tsv`.
# (EN) Removed: the loop ran the ?./?? seeds through sad-run directly, so it went
#      silently dead inside a TARGET guard — worse than a failure. Coverage is not
#      lost: both seeds live in behavior/_regression and the runner judges them with
#      the compiler. Lifting the cover surfaced a red that the guard had hidden.

# ──────────────────────────────────────────────────────────────────────
# اختبارات المترجم freestanding عبر sad-build / Freestanding Compiler Tests (sad-build)
# ──────────────────────────────────────────────────────────────────────
# تتطلب بناء sad-build (LLVM مفعّل) — تشغل سكريبت PowerShell يتحقق من أنماط LLVM IR
# Requires sad-build target (LLVM enabled) — runs PowerShell script that verifies LLVM IR patterns
if(TARGET sad-build AND WIN32)
    find_program(POWERSHELL_EXE powershell)
    if(POWERSHELL_EXE)
        add_test(
            NAME "Compiler_Freestanding_BugFixes"
            COMMAND ${POWERSHELL_EXE}
                -ExecutionPolicy Bypass
                -File "${CMAKE_SOURCE_DIR}/tests/_archive/compiler/run_freestanding_tests.ps1"
                -SadcPath "$<TARGET_FILE:sad-build>"
                -TestDir "${CMAKE_SOURCE_DIR}/tests/_archive/compiler"
        )
        set_tests_properties("Compiler_Freestanding_BugFixes" PROPERTIES
            TIMEOUT 120
            LABELS "compiler;freestanding"
        )
        message(STATUS "  [test] Compiler_Freestanding_BugFixes (sad-build IR verification)")
    else()
        message(STATUS "  [skip] Compiler_Freestanding_BugFixes: PowerShell not found")
    endif()
else()
    message(STATUS "  [skip] Compiler_Freestanding_BugFixes: sadc target not available or not Windows")
endif()

# ──────────────────────────────────────────────────────────────────────
# هدف مُجمّع / Combined Target
# ──────────────────────────────────────────────────────────────────────
add_custom_target(comprehensive_tests
    DEPENDS
        test_lexer_comprehensive
        test_keywords_v41_comprehensive
        test_parser_comprehensive
        test_value_comprehensive
        test_insertion_ordered_map_comprehensive
        test_stdlib_comprehensive
        test_errors_comprehensive
        test_ast_clone
        test_docs_extractor_comprehensive
        test_e2e_comprehensive
        test_utils_modules_comprehensive
        test_compiler_comprehensive

    # ملاحظة: اختبارات الرسومات المربوطة بـsad_graphics (test_ui_*, test_backends_*,
    # test_event_system_*, test_reconciler_*, test_ui_render_displaylist) مستثناة
    # عمدًا من هذا الهدف المُجمّع (تتطلّب sad_graphics)؛ تُسجَّل فرديًّا عبر add_test
    # وتُبنى/تُشغَّل بـ ctest -R Comprehensive. أضِف أيّ اختبار رسومات جديد بالنمط نفسه.
    COMMENT "بناء جميع الاختبارات الشاملة / Building all comprehensive tests"
)

message(STATUS "════════════════════════════════════════════════════════")
message(STATUS "  الاختبارات الشاملة: 900+ اختبار + انحدار + رسومات headless + freestanding")
message(STATUS "  بناء الكل: cmake --build build --target comprehensive_tests")
message(STATUS "  تشغيل الكل: ctest --test-dir build -R Comprehensive")
message(STATUS "  تشغيل الانحدار: ctest --test-dir build -L regression")
message(STATUS "  تشغيل المترجم: ctest --test-dir build -L freestanding")
message(STATUS "════════════════════════════════════════════════════════")
