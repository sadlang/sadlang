# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/libraries.cmake
# الوصف: المكتبات الأساسية والفرعية / Core & Sub-Libraries
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# وقت التشغيل الجديد / Runtime New (VM, Memory, Thread, FFI, ABI)
# ──────────────────────────────────────────────────────────────────────
if(EXISTS "${CMAKE_SOURCE_DIR}/runtime/CMakeLists.txt")
    add_subdirectory(runtime)
    message(STATUS "✓ وقت التشغيل الجديد / Runtime New: VM, Memory, Thread, FFI, ABI, Exception, Sandbox")
endif()

# ──────────────────────────────────────────────────────────────────────
# مكتبة النواة / Core Library (sad_core)
# ──────────────────────────────────────────────────────────────────────
# (AR) تمّ حذف فرع ENABLE_FREESTANDING (لم يُعرّف أبداً ومصادره أُرشفت)
# (EN) ENABLE_FREESTANDING branch removed (never defined; sources archived)

add_library(sad_core STATIC ${ALL_SOURCES})

# ──────────────────────────────────────────────────────────────────────
# (AR) Codegen v4.1: ضمان توليد keywords_generated.{h,cpp} قبل الترجمة
#      sad_shared هو من يحوي lexer_keywords.cpp و keywords_generated.cpp،
#      لذلك يربط الاعتماد عليه؛ sad_core يربط sad_shared فيرث الاعتماد.
# (EN) Codegen v4.1: ensure keywords_generated.{h,cpp} is built first.
#      sad_shared owns lexer_keywords.cpp + keywords_generated.cpp,
#      so the dependency lives there; sad_core links sad_shared.
# ──────────────────────────────────────────────────────────────────────
add_dependencies(sad_shared sad_keywords_codegen)

# (AR) EM-V5-4 / ق-فرعي-3 (ADR-DOCS-V4-005): ربط كل نطاقات language-truth بالبناء.
#      sad_all_codegen يجمع الـ13 هدفاً (keywords/builtins/errors/sadinfo + 10 نطاقات)؛
#      idempotent عبر stamp فلا يُعيد التوليد بلا تغيير YAML.
# (EN) Wire all language-truth domains into the build so any YAML edit auto-regenerates.
add_dependencies(sad_shared sad_all_codegen)

# (AR) Phase: dedup sad_shared/sad_core — sad_core يربط sad_shared كـ PUBLIC
#      ليُمرّر includes ويتجنب ازدواج بناء (lexer/parser/ast/types/errors/modules/utils
#      + class_manager). كان كل ملف من 49 ملفاً مشتركاً يُترجَم مرتين قبل هذا الإصلاح.
# (EN) Phase: dedup sad_shared/sad_core — sad_core links sad_shared PUBLIC to
#      forward includes and avoid build duplication (lexer/parser/ast/types/errors/
#      modules/utils + class_manager). Each of 49 shared files used to compile twice
#      before this fix.
target_link_libraries(sad_core PUBLIC sad_shared)

# (AR) Ownership Unification: sad_core يربط sad_ownership ليستهلك المفسّر
#      نظام الملكية الموحَّد عبر wrapper في interpreter/src/managers/ownership_manager.cpp
# (EN) Ownership Unification: sad_core links sad_ownership so the interpreter
#      consumes the unified ownership system via the wrapper in
#      interpreter/src/managers/ownership_manager.cpp
target_link_libraries(sad_core PUBLIC sad_ownership)

# (AR) NS-01: نظام أمان null المشترك — sad_core يجمّع interpreter_core.cpp الذي
#      يستدعي NullSafetyAnalyzer كنقطة حقيقة واحدة مشتركة مع المترجم sadc.
# (EN) NS-01: shared null-safety — sad_core compiles interpreter_core.cpp which
#      invokes NullSafetyAnalyzer (single source of truth shared with sadc).
target_link_libraries(sad_core PUBLIC sad_null_safety)
target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/shared/null_safety/include)

# (AR) الطبقة الأمنية المشتركة (BoundsChecker, SafeArithmetic, InputSanitizer,
#      SafeAllocator, TaintTracker). sad_core يجمّع مصادر المفسر التي تستخدم
#      assertSafeCast<int>(...) في أي تحويل size_t→int، فلا بد من الربط هنا
#      أيضاً (موازٍ لربط sad_interpreter PUBLIC sad_security_core).
# (EN) Shared security primitives. sad_core compiles interpreter sources that
#      call assertSafeCast<int>(...), so it must link sad_security_core too,
#      mirroring the sad_interpreter linkage.
target_link_libraries(sad_core PUBLIC sad_security_core)

# (AR) Phase A2: سياسة الذاكرة الموحَّدة — sad_core يجمّع interpreter sources
#      التي تتضمن ownership_manager.h المعتمد على memory/policy/gc_mode.h
# (EN) Phase A2: unified memory policy — sad_core compiles interpreter sources
#      that include ownership_manager.h which now depends on memory/policy/gc_mode.h
target_link_libraries(sad_core PUBLIC sad_memory_policy)

# (AR) Phase B-step3: محرك GC الموحَّد — interpreter_core.cpp يضمّن الآن
#      memory/gc/engine/garbage_collector.h ليُفعِّل/يُعلِّق المحرك حسب gcStrategy.
# (EN) Phase B-step3: unified GC engine — interpreter_core.cpp now includes the
#      engine header to toggle the engine per gcStrategy.
target_link_libraries(sad_core PUBLIC sad_memory_gc)

if(MSVC)
    target_compile_options(sad_core PRIVATE /FS /utf-8 /Z7)
endif()

# (AR) تفعيل المسار الحقيقي لوحدة HTTP داخل sad_core وربط مكتبات الشبكة
#      حتى تتوفر دوال stdlib/network للمفسر ولكل المستهلكين لـ sad_core.
# (EN) Enable the real HTTP path inside sad_core and link network libraries
#      so stdlib/network builtins are available to the interpreter and all sad_core consumers.
# ملاحظة (AR): الربط الفعلي يتم في cmake/network.cmake لأن أهداف sad_network/
#              sad_http/sad_websocket تُعرَّف هناك بعد تحميل هذا الملف.
# Note (EN): Actual linking happens in cmake/network.cmake because the
#            sad_network/sad_http/sad_websocket targets are defined there
#            AFTER this file has been included.

# ربط مكتبة وقت التشغيل بالمفسر / Link runtime to interpreter
# (AR) sad_rt_runtime معرّف في runtime/CMakeLists.txt كمكتبة INTERFACE تجمع abi + ffi + ui
# (EN) sad_rt_runtime is defined in runtime/CMakeLists.txt as an INTERFACE library
#      aggregating abi + ffi + ui
if(TARGET sad_rt_runtime)
    target_link_libraries(sad_core PRIVATE sad_rt_runtime)
    target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/runtime/include)
    message(STATUS "✓ ربط runtime بالمفسر / Linked runtime to interpreter")
endif()

# ربط صNet — مكتبة الشبكات اللامركزية / Link SadNet
if(TARGET sadnet)
    target_link_libraries(sad_core PRIVATE sadnet)
    target_compile_definitions(sad_core PRIVATE HAS_SADNET)
    message(STATUS "✓ ربط صNet بالمفسر / Linked SadNet to interpreter")
endif()

# SQLite3 (اختياري) / SQLite3 (optional)
find_package(unofficial-sqlite3 CONFIG QUIET)
if(unofficial-sqlite3_FOUND)
    target_link_libraries(sad_core PRIVATE unofficial::sqlite3::sqlite3)
    target_compile_definitions(sad_core PRIVATE HAS_SQLITE3)
    message(STATUS "✓ SQLite3: مفعّل / Enabled")
else()
    find_package(SQLite3 QUIET)
    if(SQLite3_FOUND)
        target_link_libraries(sad_core PRIVATE ${SQLite3_LIBRARIES})
        target_include_directories(sad_core PRIVATE ${SQLite3_INCLUDE_DIRS})
        target_compile_definitions(sad_core PRIVATE HAS_SQLITE3)
        message(STATUS "✓ SQLite3: مفعّل / Enabled")
    else()
        message(STATUS "⚠ SQLite3: غير موجود / Not found")
    endif()
endif()

# OpenSSL (اختياري) / OpenSSL (optional)
find_package(OpenSSL QUIET)
if(OPENSSL_FOUND)
    target_link_libraries(sad_core PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_include_directories(sad_core PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_compile_definitions(sad_core PRIVATE HAS_OPENSSL)
    message(STATUS "✓ OpenSSL: مفعّل / Enabled")
else()
    message(STATUS "⚠ OpenSSL: غير موجود / Not found")
endif()

set_target_properties(sad_core PROPERTIES
    OUTPUT_NAME "sad_core"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

# ──────────────────────────────────────────────────────────────────────
# (AR) م2-أ (sadlang-rfcs#10): طبقة جسر الواجهات sad_ui_bridge — عكس الاعتماد.
#      sad_core لم يعد يربط sad_ui. بدلًا من ذلك تُجمَّع ملفّات الواجهات المعتمِدة
#      على sad_ui (INTERPRETER_UI_BRIDGE_SOURCES) في هذا الهدف المنفصل الذي يربط
#      sad_core (PUBLIC) + sad_ui (PRIVATE). يثبّت نفسه وقت التشغيل عبر
#      installSadUIBridge() (الترويسة العامّة interpreter/include/ui/sad_ui_bridge.h)،
#      فلا يعرف القلب شيئًا عن sad_ui. المستهلكون (sad-run/profiler/wasm) يربطون
#      sad_ui_bridge ويستدعون التثبيت قبل إنشاء المفسّر.
# (EN) Phase 2-A: UI bridge layer — dependency inversion. sad_core no longer links
#      sad_ui; the sad_ui-dependent UI files compile into this separate target which
#      links sad_core PUBLIC + sad_ui PRIVATE and self-installs at runtime via
#      installSadUIBridge(). Consumers (sad-run/profiler/wasm) link sad_ui_bridge.
# ──────────────────────────────────────────────────────────────────────
if(TARGET sad_ui)
    add_library(sad_ui_bridge STATIC ${INTERPRETER_UI_BRIDGE_SOURCES})

    # (AR) PUBLIC sad_core: الجسر يستهلك Value/ExpressionEvaluator/UIStateManager/
    #      BuiltinModuleRegistry ويُمرّر هذا الاعتماد لمن يربطه. PRIVATE sad_ui:
    #      نوع الواجهات لا يظهر في واجهة الجسر العامّة (الترويسة العامّة مجرّدة).
    # (EN) PUBLIC sad_core (consumes core types, propagates to linkers); PRIVATE sad_ui
    #      (UI types never leak through the bridge's public surface).
    target_link_libraries(sad_ui_bridge PUBLIC sad_core)
    target_link_libraries(sad_ui_bridge PRIVATE sad_ui)

    # (AR) هذه الملفّات كانت تُجمَّع داخل sad_core، فترث كلَّ مسارات تضمينه الخاصّة
    #      (hot_reload/semantic/null_safety/runtime/sqlite/openssl…) عبر خاصّية الهدف،
    #      بالإضافة إلى مسارات sad_ui. مسارات المفسّر عامّة عبر include_directories الجذر.
    # (EN) These files used to compile inside sad_core, so inherit all of its PRIVATE
    #      include dirs (hot_reload/semantic/null_safety/runtime/…) via the target
    #      property, plus the sad_ui paths. Interpreter paths are global at the root.
    target_include_directories(sad_ui_bridge PRIVATE
        $<TARGET_PROPERTY:sad_core,INCLUDE_DIRECTORIES>
        ${CMAKE_SOURCE_DIR}/sad_ui/core/include
        ${CMAKE_SOURCE_DIR}/sad_ui/backends/desktop/include
    )

    if(MSVC)
        # (AR) /utf-8 ضروريّ لحرفيّات النصّ العربيّة في ملفّات الواجهات.
        target_compile_options(sad_ui_bridge PRIVATE /FS /utf-8 /Z7)
    endif()

    # (AR) على Linux، SDL2 include يحتاج مسارًا صريحًا / (EN) On Linux, SDL2 needs explicit include path
    if(UNIX)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(SDL2_PC QUIET sdl2)
            if(SDL2_PC_FOUND)
                target_include_directories(sad_ui_bridge PRIVATE ${SDL2_PC_INCLUDE_DIRS})
            endif()
        endif()
    endif()

    set_target_properties(sad_ui_bridge PROPERTIES
        OUTPUT_NAME "sad_ui_bridge"
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )
    message(STATUS "✓ طبقة جسر الواجهات sad_ui_bridge (sad_core لم يعد يعتمد sad_ui) / UI bridge decoupled")
endif()

# (AR) ربط مكتبة مصحح الأداء / (EN) Link profiler library
if(TARGET sad_profiler_lib)
    target_link_libraries(sad_core PRIVATE sad_profiler_lib)
    target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/shared/profiler/include)
    message(STATUS "✓ ربط مصحح الأداء بالمفسر / Linked profiler to interpreter")
endif()

# (AR) إضافة مسارات إعادة التحميل الساخن / (EN) Add hot reload include paths
target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/shared/hot_reload/include)
message(STATUS "✓ إعادة التحميل الساخن / Hot Reload")

# (AR) Phase 3 (F-01): مسار فاحص الأنواع المشترك بعد نقله من compiler/.
#      المفسر يضم "semantic/type_checker.h" مباشرة دون مسار نسبي.
# (EN) Phase 3 (F-01): shared type checker include path after move from compiler/.
#      Interpreter includes "semantic/type_checker.h" directly without relative path.
target_include_directories(sad_core PRIVATE ${CMAKE_SOURCE_DIR}/shared/semantic/include)

# ──────────────────────────────────────────────────────────────────────
# المكونات المشتركة والمترجم / Shared & Compiler (if not already added)
# ──────────────────────────────────────────────────────────────────────
if(NOT TARGET sad_shared)
    add_subdirectory(shared)
endif()
if(NOT TARGET sad_compiler)
    add_subdirectory(compiler)
endif()
