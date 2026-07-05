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
# مكتبة المفسّر / Interpreter Library (sad_interp ؛ sad_core = alias توافق)
# ──────────────────────────────────────────────────────────────────────
# (AR) تمّ حذف فرع ENABLE_FREESTANDING (لم يُعرّف أبداً ومصادره حُذفت في م5)
# (EN) ENABLE_FREESTANDING branch removed (never defined; sources deleted in phase 5)

add_library(sad_interp STATIC ${ALL_SOURCES})
# (AR) تصحيح اسم (RFC sadlang-rfcs#10): الهدف الحقيقيّ صار sad_interp (= المفسّر الشجريّ،
#      لا نواة اللغة؛ النواة هي sad_shared). نُبقي sad_core كاسم بديل (alias) للتوافق
#      مع المستهلكين القائمين (التنفيذيّات/الأدوات/الاختبارات) ريثما يُهاجَرون تباعًا.
# (EN) Name correction: the real target is now sad_interp (the tree-walking interpreter,
#      NOT the language core — that is sad_shared). sad_core kept as a compat ALIAS.
add_library(sad_core ALIAS sad_interp)

# ----------------------------------------------------------------------
# (AR) م2-ج (sadlang-rfcs#10): مكتبة المدمجات النقيّة sad_builtins.
#      تعتمد sad_shared فقط (Value/Types/Errors)؛ لا FunctionManager. sad_core
#      يربطها PUBLIC فتتوفّر لكل المستهلكين، فتُكسَر دورة sad_core<->builtins.
# (EN) Phase 2-C: pure builtins library. Depends only on sad_shared; no
#      FunctionManager. sad_core links it PUBLIC. Breaks the core<->builtins cycle.
# ----------------------------------------------------------------------
add_library(sad_builtins STATIC ${SAD_BUILTINS_LIB_SOURCES})
target_link_libraries(sad_builtins PUBLIC sad_shared)
if(MSVC)
    target_compile_options(sad_builtins PRIVATE /FS /utf-8 /Z7)
endif()
target_link_libraries(sad_interp PUBLIC sad_builtins)

# ----------------------------------------------------------------------
# (AR) م2-ج شريحة2 (sadlang-rfcs#10): مكتبة مدمجات النواة/العتاد sad_lowlevel.
#      26 ملفًّا من stdlib/low_level/src — كود عتاد/نواة بحت لا يلمس Data::Value.
#      يعتمد sad_security_core فقط (safe_arithmetic.h)؛ sad_core يربطه PUBLIC.
#      اللِحام (interpreter/src/builtins/builtin_kernel_*.cpp) يبقى في القلب.
# (EN) Phase 2-C slice2: pure kernel/low-level builtins library. 26 files from
#      stdlib/low_level/src; no Data::Value. Depends only on sad_security_core
#      (safe_arithmetic.h). sad_core links it PUBLIC; the glue stays in the core.
# ----------------------------------------------------------------------
add_library(sad_lowlevel STATIC ${LOW_LEVEL_SOURCES})
target_link_libraries(sad_lowlevel PUBLIC sad_security_core)
if(MSVC)
    target_compile_options(sad_lowlevel PRIVATE /FS /utf-8 /Z7)
endif()
target_link_libraries(sad_interp PUBLIC sad_lowlevel)



# ──────────────────────────────────────────────────────────────────────
# (AR) Codegen v4.1: ضمان توليد keywords_generated.{h,cpp} قبل الترجمة
#      sad_shared هو من يحوي lexer_keywords.cpp و keywords_generated.cpp،
#      لذلك يربط الاعتماد عليه؛ sad_interp يربط sad_shared فيرث الاعتماد.
# (EN) Codegen v4.1: ensure keywords_generated.{h,cpp} is built first.
#      sad_shared owns lexer_keywords.cpp + keywords_generated.cpp,
#      so the dependency lives there; sad_interp links sad_shared.
# ──────────────────────────────────────────────────────────────────────
add_dependencies(sad_shared sad_keywords_codegen)

# (AR) EM-V5-4 / ق-فرعي-3 (ADR-DOCS-V4-005): ربط كل نطاقات language-truth بالبناء.
#      sad_all_codegen يجمع النطاقات الأربعة العاملة (types/keywords/builtins/error_messages)؛
#      idempotent عبر stamp فلا يُعيد التوليد بلا تغيير YAML.
# (EN) Wire all language-truth domains into the build so any YAML edit auto-regenerates.
add_dependencies(sad_shared sad_all_codegen)

# (AR) Phase: dedup sad_shared/sad_interp — sad_interp يربط sad_shared كـ PUBLIC
#      ليُمرّر includes ويتجنب ازدواج بناء (lexer/parser/ast/types/errors/modules/utils
#      + class_manager). كان كل ملف من 49 ملفاً مشتركاً يُترجَم مرتين قبل هذا الإصلاح.
# (EN) Phase: dedup sad_shared/sad_interp — sad_interp links sad_shared PUBLIC to
#      forward includes and avoid build duplication (lexer/parser/ast/types/errors/
#      modules/utils + class_manager). Each of 49 shared files used to compile twice
#      before this fix.
target_link_libraries(sad_interp PUBLIC sad_shared)

# (AR) Ownership Unification: sad_interp يربط sad_ownership ليستهلك المفسّر
#      نظام الملكية الموحَّد عبر wrapper في interpreter/src/managers/ownership_manager.cpp
# (EN) Ownership Unification: sad_interp links sad_ownership so the interpreter
#      consumes the unified ownership system via the wrapper in
#      interpreter/src/managers/ownership_manager.cpp
target_link_libraries(sad_interp PUBLIC sad_ownership)

# (AR) NS-01: نظام أمان null المشترك — sad_interp يجمّع interpreter_core.cpp الذي
#      يستدعي NullSafetyAnalyzer كنقطة حقيقة واحدة مشتركة مع المترجم sad-build.
# (EN) NS-01: shared null-safety — sad_interp compiles interpreter_core.cpp which
#      invokes NullSafetyAnalyzer (single source of truth shared with sad-build).
target_link_libraries(sad_interp PUBLIC sad_null_safety)
target_include_directories(sad_interp PRIVATE ${CMAKE_SOURCE_DIR}/shared/null_safety/include)

# (AR) قاعدة الدالة الرئيسية الموحَّدة (SEM018) — sad_interp يجمّع interpreter_core.cpp
#      الذي يستدعي checkMainFunctionRule (مصدر حقيقة واحد مشترك مع sad-build).
# (EN) Unified main-function rule (SEM018) — sad_interp compiles interpreter_core.cpp
#      which invokes checkMainFunctionRule (single source of truth shared with sad-build).
target_link_libraries(sad_interp PUBLIC sad_program_rules)
target_include_directories(sad_interp PRIVATE ${CMAKE_SOURCE_DIR}/shared/program_rules/include)

# (AR) الطبقة الأمنية المشتركة (BoundsChecker, SafeArithmetic, InputSanitizer,
#      SafeAllocator, TaintTracker). sad_interp يجمّع مصادر المفسر التي تستخدم
#      assertSafeCast<int>(...) في أي تحويل size_t→int، فلا بد من الربط هنا
#      أيضاً (موازٍ لربط sad_interpreter PUBLIC sad_security_core).
# (EN) Shared security primitives. sad_interp compiles interpreter sources that
#      call assertSafeCast<int>(...), so it must link sad_security_core too,
#      mirroring the sad_interpreter linkage.
target_link_libraries(sad_interp PUBLIC sad_security_core)

# (AR) Phase A2: سياسة الذاكرة الموحَّدة — sad_interp يجمّع interpreter sources
#      التي تتضمن ownership_manager.h المعتمد على memory/policy/gc_mode.h
# (EN) Phase A2: unified memory policy — sad_interp compiles interpreter sources
#      that include ownership_manager.h which now depends on memory/policy/gc_mode.h
target_link_libraries(sad_interp PUBLIC sad_memory_policy)

# (AR) Phase B-step3: محرك GC الموحَّد — interpreter_core.cpp يضمّن الآن
#      memory/gc/engine/garbage_collector.h ليُفعِّل/يُعلِّق المحرك حسب gcStrategy.
# (EN) Phase B-step3: unified GC engine — interpreter_core.cpp now includes the
#      engine header to toggle the engine per gcStrategy.
target_link_libraries(sad_interp PUBLIC sad_memory_gc)

# ----------------------------------------------------------------------
# (AR) م3 خطوة 4 (RFC sadlang-rfcs#10): مكتبة خدمات وقت التشغيل المشتركة sad_runtime.
#      تُستخرَج المدراء الثلاثة (function/object/ownership) من قلب المفسّر إلى هدفٍ
#      ساكنٍ منفصل، فيصير لها موضعٌ طبقيٌّ مستقلٌّ عن المشي على الشجرة — وهو موضع
#      شقيق الآلة الافتراضية عند عودتها (تشترك المحرّكات في هذه الخدمات لا في الـwalker).
#      الاتّجاه أحاديّ: sad_interp → sad_runtime. sad_runtime يربط فقط طبقات الأساس/
#      المشترك التي تحتاجها المدراء (sad_shared/ownership/memory/security)، ولا يربط
#      sad_interp ⇒ لا دورة. مساراتُ التضمين (interpreter/include …) عامّةٌ من الجذر.
# (EN) Phase-3 step-4: shared runtime-services library sad_runtime. The three managers
#      (function/object/ownership) extracted from the interpreter core into a separate
#      static target — a layer position independent of tree walking, the future sibling
#      slot for the VM. One-directional: sad_interp → sad_runtime (no cycle).
# ----------------------------------------------------------------------
add_library(sad_runtime STATIC ${SAD_RUNTIME_SOURCES})
target_link_libraries(sad_runtime PUBLIC
    sad_shared
    sad_ownership
    sad_memory_policy
    sad_memory_gc
    sad_security_core
)
# (AR) م3 (RFC sadlang-rfcs#10): sad_runtime لا يربط sad_interp (الاتّجاه أحاديّ)،
#      فيأخذ مسارات ترويسات المفسّر التي تحتاجها مصادرُه (managers/builtins/core…)
#      PRIVATE صراحةً، بعد إزالة الكتلة العامّة من الجذر.
# (EN) sad_runtime doesn't link sad_interp (one-directional), so it takes the
#      interpreter header paths its sources need explicitly as PRIVATE.
target_include_directories(sad_runtime PRIVATE
    ${CMAKE_SOURCE_DIR}/interpreter/include
    ${CMAKE_SOURCE_DIR}/interpreter/include/core
    ${CMAKE_SOURCE_DIR}/interpreter/include/managers
    ${CMAKE_SOURCE_DIR}/interpreter/include/builtins
)
if(MSVC)
    target_compile_options(sad_runtime PRIVATE /FS /utf-8 /Z7)
endif()
set_target_properties(sad_runtime PROPERTIES
    OUTPUT_NAME "sad_runtime"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)
target_link_libraries(sad_interp PUBLIC sad_runtime)
message(STATUS "✓ طبقة خدمات وقت التشغيل sad_runtime (function/object/ownership managers) / runtime-services layer")

if(MSVC)
    target_compile_options(sad_interp PRIVATE /FS /utf-8 /Z7)
endif()

# (AR) تفعيل المسار الحقيقي لوحدة HTTP داخل sad_interp وربط مكتبات الشبكة
#      حتى تتوفر دوال features/network/core للمفسر ولكل المستهلكين لـ sad_interp.
# (EN) Enable the real HTTP path inside sad_interp and link network libraries
#      so features/network/core builtins are available to the interpreter and all sad_interp consumers.
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
    target_link_libraries(sad_interp PRIVATE sad_rt_runtime)
    target_include_directories(sad_interp PRIVATE ${CMAKE_SOURCE_DIR}/runtime/include)
    message(STATUS "✓ ربط runtime بالمفسر / Linked runtime to interpreter")
endif()

# ربط صNet — مكتبة الشبكات اللامركزية / Link SadNet
if(TARGET sadnet)
    target_link_libraries(sad_interp PRIVATE sadnet)
    target_compile_definitions(sad_interp PRIVATE HAS_SADNET)
    message(STATUS "✓ ربط صNet بالمفسر / Linked SadNet to interpreter")
endif()

# SQLite3 (اختياري) / SQLite3 (optional)
find_package(unofficial-sqlite3 CONFIG QUIET)
if(unofficial-sqlite3_FOUND)
    target_link_libraries(sad_builtins PRIVATE unofficial::sqlite3::sqlite3)
    target_compile_definitions(sad_builtins PRIVATE HAS_SQLITE3)
    message(STATUS "✓ SQLite3: مفعّل / Enabled")
else()
    find_package(SQLite3 QUIET)
    if(SQLite3_FOUND)
        target_link_libraries(sad_builtins PRIVATE ${SQLite3_LIBRARIES})
        target_include_directories(sad_builtins PRIVATE ${SQLite3_INCLUDE_DIRS})
        target_compile_definitions(sad_builtins PRIVATE HAS_SQLITE3)
        message(STATUS "✓ SQLite3: مفعّل / Enabled")
    else()
        message(STATUS "⚠ SQLite3: غير موجود / Not found")
    endif()
endif()

# OpenSSL (اختياري) / OpenSSL (optional)
find_package(OpenSSL QUIET)
if(OPENSSL_FOUND)
    target_link_libraries(sad_builtins PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_include_directories(sad_builtins PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_compile_definitions(sad_builtins PRIVATE HAS_OPENSSL)
    message(STATUS "✓ OpenSSL: مفعّل / Enabled")
else()
    message(STATUS "⚠ OpenSSL: غير موجود / Not found")
endif()

set_target_properties(sad_interp PROPERTIES
    OUTPUT_NAME "sad_interp"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

# ──────────────────────────────────────────────────────────────────────
# (AR) م2-أ (sadlang-rfcs#10): طبقة جسر الواجهات sad_graphics_bridge — عكس الاعتماد.
#      sad_core لم يعد يربط sad_graphics. بدلًا من ذلك تُجمَّع ملفّات الواجهات المعتمِدة
#      على sad_graphics (INTERPRETER_UI_BRIDGE_SOURCES) في هذا الهدف المنفصل الذي يربط
#      sad_core (PUBLIC) + sad_graphics (PRIVATE). يثبّت نفسه وقت التشغيل عبر
#      installSadGraphicsBridge() (الترويسة العامّة interpreter/include/ui/sad_graphics_bridge.h)،
#      فلا يعرف القلب شيئًا عن sad_graphics. المستهلكون (sad-run/profiler/wasm) يربطون
#      sad_graphics_bridge ويستدعون التثبيت قبل إنشاء المفسّر.
# (EN) Phase 2-A: UI bridge layer — dependency inversion. sad_core no longer links
#      sad_graphics; the sad_graphics-dependent UI files compile into this separate target which
#      links sad_core PUBLIC + sad_graphics PRIVATE and self-installs at runtime via
#      installSadGraphicsBridge(). Consumers (sad-run/profiler/wasm) link sad_graphics_bridge.
# ──────────────────────────────────────────────────────────────────────
if(TARGET sad_graphics)
    add_library(sad_graphics_bridge STATIC ${INTERPRETER_UI_BRIDGE_SOURCES})

    # (AR) PUBLIC sad_core: الجسر يستهلك Value/ExpressionEvaluator/UIStateManager/
    #      BuiltinModuleRegistry ويُمرّر هذا الاعتماد لمن يربطه. PRIVATE sad_graphics:
    #      نوع الواجهات لا يظهر في واجهة الجسر العامّة (الترويسة العامّة مجرّدة).
    # (EN) PUBLIC sad_core (consumes core types, propagates to linkers); PRIVATE sad_graphics
    #      (UI types never leak through the bridge's public surface).
    target_link_libraries(sad_graphics_bridge PUBLIC sad_core)
    target_link_libraries(sad_graphics_bridge PRIVATE sad_graphics)

    # (AR) هذه الملفّات كانت تُجمَّع داخل القلب (sad_interp)، فترث كلَّ مسارات تضمينه
    #      الخاصّة (hot_reload/semantic/null_safety/runtime/sqlite/openssl…) عبر خاصّية
    #      الهدف $<TARGET_PROPERTY:sad_interp,…>، بالإضافة إلى مسارات sad_graphics.
    # (EN) These files used to compile inside the core (sad_interp), so they inherit all
    #      of its PRIVATE include dirs (hot_reload/semantic/null_safety/runtime/…) via the
    #      $<TARGET_PROPERTY:sad_interp,…> expression, plus the sad_graphics paths.
    target_include_directories(sad_graphics_bridge PRIVATE
        $<TARGET_PROPERTY:sad_interp,INCLUDE_DIRECTORIES>
        ${CMAKE_SOURCE_DIR}/features/graphics/core/include
        ${CMAKE_SOURCE_DIR}/features/graphics/backends/desktop/include
    )

    if(MSVC)
        # (AR) /utf-8 ضروريّ لحرفيّات النصّ العربيّة في ملفّات الواجهات.
        target_compile_options(sad_graphics_bridge PRIVATE /FS /utf-8 /Z7)
    endif()

    # (AR) على Linux، SDL2 include يحتاج مسارًا صريحًا / (EN) On Linux, SDL2 needs explicit include path
    if(UNIX)
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(SDL2_PC QUIET sdl2)
            if(SDL2_PC_FOUND)
                target_include_directories(sad_graphics_bridge PRIVATE ${SDL2_PC_INCLUDE_DIRS})
            endif()
        endif()
    endif()

    set_target_properties(sad_graphics_bridge PROPERTIES
        OUTPUT_NAME "sad_graphics_bridge"
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    )
    message(STATUS "✓ طبقة جسر الواجهات sad_graphics_bridge (sad_core لم يعد يعتمد sad_graphics) / UI bridge decoupled")
endif()

# (AR) ربط مكتبة مصحح الأداء / (EN) Link profiler library
if(TARGET sad_profiler_lib)
    target_link_libraries(sad_interp PRIVATE sad_profiler_lib)
    target_include_directories(sad_interp PRIVATE ${CMAKE_SOURCE_DIR}/shared/profiler/include)
    message(STATUS "✓ ربط مصحح الأداء بالمفسر / Linked profiler to interpreter")
endif()

# (AR) إضافة مسارات إعادة التحميل الساخن / (EN) Add hot reload include paths
target_include_directories(sad_interp PRIVATE ${CMAKE_SOURCE_DIR}/shared/hot_reload/include)
message(STATUS "✓ إعادة التحميل الساخن / Hot Reload")

# (AR) Phase 3 (F-01): مسار فاحص الأنواع المشترك بعد نقله من compiler/.
#      المفسر يضم "semantic/type_checker.h" مباشرة دون مسار نسبي.
# (EN) Phase 3 (F-01): shared type checker include path after move from compiler/.
#      Interpreter includes "semantic/type_checker.h" directly without relative path.
target_include_directories(sad_interp PRIVATE ${CMAKE_SOURCE_DIR}/shared/semantic/include)

# (AR) هيدرات مكتبة الرسومات المولَّدة من مصدر الحقيقة (تعداد الألوان المدمَج
#      color_prelude_generated.h): يضمّها القلب interpreter_core.cpp لتسجيل «ألوان»
#      عند الإقلاع. هيدرٌ ورقيّ مُلتزَم ذاتيّ الاكتفاء ⇒ لا اعتماد ربط، الطبقيّة محفوظة.
# (EN) SoT-generated graphics headers (builtin color enum prelude): included by the
#      core to register `ألوان` at startup. Header-only committed file — no link dep.
target_include_directories(sad_interp PRIVATE ${CMAKE_SOURCE_DIR}/features/graphics/core/include)

# ──────────────────────────────────────────────────────────────────────
# (AR) م3 (RFC sadlang-rfcs#10): ترويسات المفسّر الخاصّة كـPUBLIC على sad_interp
#      بدل الكتلة العامّة include_directories في الجذر. هذا يقصر رؤية ترويسات
#      المفسّر على من يربط sad_interp (المفسّر + أدواته + اختباراته + جسر الواجهات
#      عبر $<TARGET_PROPERTY>)، فلا يراها نظامُ المترجم الذي يربط sad_shared فقط.
#      (sad_runtime لا يربط sad_interp ⇒ يأخذ المسارات PRIVATE أدناه؛ sad-run/sad-build
#       لهما مساراتهما الصريحة في apps/.)
# (EN) Phase-3: interpreter-private headers as PUBLIC on sad_interp instead of the
#      root global include_directories block. Scopes interpreter header visibility to
#      sad_interp linkers only (interp + tools + tests + ui_bridge via TARGET_PROPERTY),
#      so the compiler subsystem (links sad_shared only) never sees them.
# ──────────────────────────────────────────────────────────────────────
target_include_directories(sad_interp PUBLIC
    ${CMAKE_SOURCE_DIR}/interpreter/include
    ${CMAKE_SOURCE_DIR}/interpreter/include/core
    ${CMAKE_SOURCE_DIR}/interpreter/include/visitors
    ${CMAKE_SOURCE_DIR}/interpreter/include/managers
    ${CMAKE_SOURCE_DIR}/interpreter/include/builtins
    ${CMAKE_SOURCE_DIR}/interpreter/include/debug
    ${CMAKE_SOURCE_DIR}/interpreter/include/ui
    ${CMAKE_SOURCE_DIR}/interpreter/src/ui
)

# ──────────────────────────────────────────────────────────────────────
# المكونات المشتركة والمترجم / Shared & Compiler (if not already added)
# ──────────────────────────────────────────────────────────────────────
if(NOT TARGET sad_shared)
    add_subdirectory(shared)
endif()
if(NOT TARGET sad_compiler)
    add_subdirectory(compiler)
endif()
