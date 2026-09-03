# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/executables.cmake
# الوصف (AR): مكتبات الدعم التنفيذيّة (sad_mobile) + ثنائيّ الدخان (flush_smoke).
#             **التنفيذيّان الرئيسيّان sad-run وsad-build انتُقلا إلى apps/** (نقاط
#             دخول رفيعة) ضمن المرحلة 3 من RFC sadlang-rfcs#10 (فصل المحرّكات).
#             انظر apps/CMakeLists.txt المُدرَج عبر add_subdirectory(apps) في الجذر.
# الوصف (EN): Executable-support libraries (sad_mobile) + smoke binary (flush_smoke).
#             **The two main executables sad-run/sad-build moved to apps/** (thin
#             entry points) under RFC sadlang-rfcs#10 Phase 3. See apps/CMakeLists.txt.
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# مكتبة المنسّق / Formatter Library
# (AR) نُقلت إلى tools/formatter/ كأداة مستقلة (sad_formatter_lib) تعتمد
#      على tools/shared (sad_tools_shared) الذي يقرأ KeywordTable من
#      shared/lexer مباشرةً. الاسم البديل sad_formatter يُعرَّف هناك.
# (EN) Moved to tools/formatter/ as a standalone tool (sad_formatter_lib)
#      that depends on tools/shared (sad_tools_shared) which reads
#      KeywordTable from shared/lexer directly. The alias sad_formatter
#      is defined there.
# ─────────────────────────────────────────────────────────────────────

# ─────────────────────────────────────────────────────────────────────
# مكتبة تطبيقات الهاتف / Mobile Applications Library
# ─────────────────────────────────────────────────────────────────────
set(MOBILE_SOURCES
    tools/compiler/src/android_target.cpp
    tools/compiler/src/ios_target.cpp
    tools/compiler/src/mobile_project_gen.cpp
    tools/compiler/src/cli_commands.cpp
    tools/compiler/src/cli_mobile_manager.cpp
    # (AR) 🔑 حُذف run_command.cpp و test_command.cpp: كانا **شفرةً ميّتةً
    #      مُصرَّفة** — يُصرَّفان ويُربَطان، ولا يُبلَغان أبدًا. فـ`CommandManager`
    #      (الذي يُسجّلُ RunCommand وTestCommand) لا يُنشَأُ إلّا داخلَ
    #      `#ifdef SAD_CLI_MAIN`، والرمزُ لا يُعرَّفُ في أيِّ CMakeLists ولا
    #      سطرِ أمر. فكانا يحملانِ كلفةَ ترجمةٍ وخطرَ كسرٍ وصفرَ سلوك.
    #      ⚠️ دَينٌ مُقيَّد: `sad run` و`sad test` غيرُ مُنفَّذَين في مسارِ
    #      المحرّكِ الواحد. والتشغيلُ اليومَ: `sad build x.ص -o x.exe` ثمّ `./x.exe`.
    # (EN) Removed: compiled-but-unreachable code. CommandManager is only
    #      constructed inside #ifdef SAD_CLI_MAIN, which nothing defines.
    tools/compiler/src/build_command.cpp
)

add_library(sad_mobile STATIC ${MOBILE_SOURCES})

if(MSVC)
    target_compile_options(sad_mobile PRIVATE /wd4819 /FS)
endif()

target_include_directories(sad_mobile PUBLIC
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/tools/compiler/src
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/shared/ownership/include
    ${CMAKE_SOURCE_DIR}/shared/utils/include
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}/compiler/include/backend
)

# (AR) م3 (RFC sadlang-rfcs#10): run_command/test_command (أمرا «شغّل»/«اختبر») يضمّان
#      interpreter_core.h الذي يسحب ترويسات المفسّر متعدّيًا. بعد نقل الكتلة العامّة،
#      يأخذها sad_mobile صراحةً **PRIVATE** (لا PUBLIC) كي لا تتسرّب إلى sad-build الذي
#      يربط sad_mobile PUBLIC — فيبقى نظامُ المترجم نظيفًا من ترويسات المفسّر.
# (EN) Phase-3: run_command/test_command include interpreter_core.h (pulls interpreter
#      headers transitively). After the global-block move, sad_mobile takes them
#      PRIVATE so they don't leak to sad-build (which links sad_mobile PUBLIC).

target_compile_features(sad_mobile PUBLIC cxx_std_17)

# (AR) Ownership Unification: sad_mobile يستهلك ownership_manager.h ضمن sad_core
#      لذا يحتاج include path الجديد ورابط sad_ownership
# (EN) Ownership Unification: sad_mobile transitively consumes ownership_manager.h
#      so it needs the new include path and the sad_ownership link
target_link_libraries(sad_mobile PUBLIC sad_formatter sad_ownership sad_memory_policy)

set_target_properties(sad_mobile PROPERTIES
    OUTPUT_NAME "sad_mobile"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
)

message(STATUS "✓ تطبيقات الهاتف / Mobile: Android + iOS")

# ======================================================================
# (AR) هدف اختبار يدوي مؤقت لـ EM.flush()
# (EN) Temporary smoke executable for EM.flush()
# ======================================================================
if(EXISTS ${CMAKE_SOURCE_DIR}/_scratch/flush_smoke.cpp)
    add_executable(flush_smoke ${CMAKE_SOURCE_DIR}/_scratch/flush_smoke.cpp)
    target_link_libraries(flush_smoke PRIVATE sad_shared)
    target_include_directories(flush_smoke PRIVATE
        ${CMAKE_SOURCE_DIR}/shared/errors/include
    )
    if(MSVC)
        target_compile_options(flush_smoke PRIVATE /utf-8 /wd4819)
    endif()
endif()
