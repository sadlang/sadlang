# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: apps/CMakeLists.txt
# الوصف (AR): نقاط الدخول التنفيذيّة الرفيعة (lightweight mains) — المرحلة 3 من
#             RFC sadlang-rfcs#10 (فصل المحرّكات). كلّ تطبيق هنا «ورقة» رفيعة:
#             main وحيد فوق مكتبات محرّكه، لا منطق مكتبيّ. هذا يوضّح حدّ الطبقة L2
#             (التنفيذيّات تستهلك المكتبات ولا تُستهلَك) ويسند حارس الطبقات
#             (scripts/codegen/check_layering.py).
#               • apps/sad-run   — تطبيق المفسّر (يستهلك sad_core/sad_interp + الحزام)
#               • apps/sad-build — تطبيق المترجم (يستهلك sad_compiler + sad_shared)
#             نُقلت تعريفات الهدفين من cmake/executables.cmake (التي أبقت مكتبة
#             sad_mobile وثنائيّ flush_smoke). مصادر main وحدها انتقلت إلى apps/؛
#             مصادر سائق المترجم (compiler_driver_*.cpp) ومكتبة sad_mobile تبقى في
#             tools/compiler/ ويُشار إليها بمسار مطلق.
# الوصف (EN): Lightweight executable entry points (apps/) — RFC sadlang-rfcs#10
#             Phase 3 (engine separation). Each app is a thin leaf: a single main
#             over its engine's libraries, no library logic. Moved here from
#             cmake/executables.cmake (which kept the sad_mobile lib + flush_smoke).
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# المفسر الرئيسي / Main Interpreter (sad-run)
# (AR) اسم الهدف موحَّد مع اسم المخرج (sad-run.exe) — RFC «توحيد أسماء أهداف البناء».
#      الأمر: cmake --build build --config Debug --target sad-run
# (EN) Target name unified with output (sad-run.exe) — Build-Target-Naming RFC.
# ──────────────────────────────────────────────────────────────────────
add_executable(sad-run ${CMAKE_CURRENT_SOURCE_DIR}/sad-run/main.cpp)

target_link_libraries(sad-run PRIVATE
    sad_core
    sad_type_system
    sad_semantic
    sad_network
    sad_http
    sad_websocket
    sad_mobile
    $<$<TARGET_EXISTS:sad_graphics_bridge>:sad_graphics_bridge>  # (AR) م2-أ: جسر الواجهات (headless: يتلاشى) / (EN) UI bridge (headless: drops)
    $<$<TARGET_EXISTS:sad_graphics>:sad_graphics>
    sad_memory_policy   # (AR) Phase A2: سياسة الذاكرة الموحَّدة / (EN) Phase A2: unified memory policy
    sad_null_safety     # (AR) NS-01: نظام أمان null المشترك / (EN) NS-01: shared null-safety
    sad_program_rules   # (AR) قاعدة الدالة الرئيسية الموحَّدة (SEM018) / (EN) unified main-function rule
)

# (AR) علَم headless لـsad-run (يُطابق sad-repl): يُسقط جسر SDL عند SAD_ENABLE_GRAPHICS=OFF
# (EN) headless flag for sad-run (mirrors sad-repl): drops the SDL bridge when OFF
target_compile_definitions(sad-run PRIVATE SAD_ENABLE_GRAPHICS=$<BOOL:${SAD_ENABLE_GRAPHICS}>)

# (AR) ربط مكتبة مصحح الأداء / (EN) Link profiler library
if(TARGET sad_profiler_lib)
    target_link_libraries(sad-run PRIVATE sad_profiler_lib)
endif()

# (AR) ربط مكتبة وقت التشغيل إذا تم بناؤها / (EN) Link runtime if built
if(TARGET sad_rt_runtime)
    target_link_libraries(sad-run PRIVATE sad_rt_runtime)
endif()

# (AR) ملاحظة م3 شريحة2: أُسقط مسار vm/include الميّت (حُذفت الآلة في PR #96).
#      وأُضيف interpreter/include صراحةً بعد نقل main إلى apps/ (كان يُحَلّ نسبيًّا)
#      — وهو مشروع لأنّ sad-run هو تطبيق المفسّر (حدّ الحارس على نظام المترجم فقط).
# (EN) Phase-3 apps/ move: dropped the dead vm/include path (VM removed in #96) and
#      added interpreter/include explicitly (was resolved via a relative path) —
#      legitimate since sad-run is the interpreter app.
target_include_directories(sad-run PRIVATE
    ${CMAKE_SOURCE_DIR}/interpreter/include
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}/shared/ownership/include
    ${CMAKE_SOURCE_DIR}/shared/null_safety/include   # (AR) NS-01: رأس محلّل أمان null
    ${CMAKE_SOURCE_DIR}/shared/program_rules/include # (AR) قاعدة الدالة الرئيسية الموحَّدة (SEM018)
    ${CMAKE_SOURCE_DIR}/compiler/include/types
    ${CMAKE_SOURCE_DIR}/compiler/include/backend
    ${CMAKE_SOURCE_DIR}/tools/compiler/src
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/hot_reload/include
    # (AR) Phase 3 (F-01): فاحص الأنواع المشترك بعد نقله إلى shared/semantic/
    # (EN) Phase 3 (F-01): shared type checker after move to shared/semantic/
    ${CMAKE_SOURCE_DIR}/shared/semantic/include
    # (AR) Phase 4 (F-03): دوال utf8_utils وأدوات مشتركة دون مسارات نسبية
    # (EN) Phase 4 (F-03): utf8_utils + shared utilities without relative paths
    ${CMAKE_SOURCE_DIR}/shared/utils/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    ${CMAKE_SOURCE_DIR}/shared/modules/include
)

set_target_properties(sad-run PROPERTIES
    OUTPUT_NAME "sad-run"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
)

# (AR) م-٧: حجز 128م للمكدّس — مطابقةً لـsad-build. المفسّر يستهلك المكدّس أعمق من
#      المترجم لا أقلّ: كلّ نداءٍ في لغة ص يُترجَم إلى سلسلة أُطُر C++ متداخلة في
#      زوّار الشجرة، فالتعاود العاديّ (عاملي، اجتياز شجرة) كان يفجّر مكدّس ويندوز
#      الافتراضيّ (1م) عند عمق ~32 في بناء Debug — بانهيار صامت (0xC00000FD) لا
#      تشخيصَ فيه، بينما حارس maxCallDepth_=1000 لا تبلغه التنفيذة أصلًا.
#      بغير هذا الحجز كان الحارس المنطقيّ حبرًا على ورق.
# (EN) م-٧: reserve a 128MB stack, mirroring sad-build. Each Sad-level call expands
#      into a chain of nested C++ visitor frames, so ordinary recursion blew the
#      default 1MB Windows stack at depth ~32 in Debug — a silent 0xC00000FD crash,
#      with the maxCallDepth_=1000 guard never reached.
if(MSVC)
    target_link_options(sad-run PRIVATE /STACK:134217728)
elseif(APPLE)
    # (AR) رابط macOS (ld64) يستخدم -stack_size لا -z stacksize (خاص بـ GNU ld).
    # (EN) macOS ld64 uses -stack_size, not the GNU ld -z stacksize option.
    target_link_options(sad-run PRIVATE -Wl,-stack_size,0x8000000)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # (AR) ⚠ ‏«‎-z stack-size» تهجئة binutils ld/lld (‏«stacksize» بلا شرطة تهجئة
    #      Solaris فتُتجاهَل صامتة). وحتّى بالتهجئة الصحيحة لا يحكم PT_GNU_STACK
    #      مكدّسَ الخيط الرئيسيّ على لينكس — يشتقّه النواة من RLIMIT_STACK (٨م
    #      عادةً) — فيبقى الأثر على الخيوط المُنشأة وعلى الأنظمة التي تقرؤه.
    #      عدّاءٌ بـulimit -s 4096 يفشل رغم هذا السطر: دَينٌ مُعلَن لا مسدود.
    target_link_options(sad-run PRIVATE -Wl,-z,stack-size=134217728)
endif()

message(STATUS "✓ المفسر / Interpreter: sad-run (apps/sad-run)")

# ──────────────────────────────────────────────────────────────────────
# المترجم sad-build (يتطلب LLVM) / Compiler sad-build (requires LLVM)
# (AR) اسم الهدف موحَّد مع اسم المخرج (sad-build.exe) — RFC «توحيد أسماء أهداف البناء».
#      الأمر: cmake --build build --config Debug --target sad-build
# (EN) Target name unified with output (sad-build.exe) — Build-Target-Naming RFC.
# (AR) سائق المترجم (compiler_driver_*.cpp) يبقى في tools/compiler/ — ليس main بل
#      جسم تطبيق sad-build؛ يُشار إليه بمسار مطلق ويُجمَّع ضمن هذا الهدف.
# (EN) The compiler driver (compiler_driver_*.cpp) stays under tools/compiler/ — it
#      is the sad-build app body (not a main); referenced absolutely, compiled here.
# ──────────────────────────────────────────────────────────────────────
# ================================================================
# (AR) تضمين ملف runtime كبيانات في C++
# (EN) Embed runtime C file as C++ data header
# (AR) ⚠️ خارجَ كتلةِ LLVM عمدًا: `compiler_driver_linker.cpp` و`_build_utils.cpp`
#      يُضمِّنانِ هذه الترويسةَ المولَّدة، وهما من مصادرِ المترجمِ النحيلِ أيضًا.
# (EN) ⚠️ Deliberately outside the LLVM block: the linker and build-utils TUs
#      include this generated header, and both are thin-compiler sources too.
# ================================================================
set(SAD_RUNTIME_SOURCE "${CMAKE_SOURCE_DIR}/tools/compiler/runtime/sad_embedded_runtime.c")
set(SAD_RUNTIME_HEADER "${CMAKE_BINARY_DIR}/generated/sad_embedded_runtime_data.h")

add_custom_command(
    OUTPUT "${SAD_RUNTIME_HEADER}"
    COMMAND ${CMAKE_COMMAND}
        -DRUNTIME_INPUT_FILE=${SAD_RUNTIME_SOURCE}
        -DRUNTIME_OUTPUT_FILE=${SAD_RUNTIME_HEADER}
        -P ${CMAKE_SOURCE_DIR}/cmake/embed_runtime.cmake
    DEPENDS "${SAD_RUNTIME_SOURCE}"
    COMMENT "(AR) تضمين runtime... / (EN) Embedding runtime..."
)

# (AR) هدف وهمي لضمان التوليد قبل البناء
# (EN) Custom target to ensure generation before build
add_custom_target(generate_runtime_header DEPENDS "${SAD_RUNTIME_HEADER}")

# ═══════════════════════════════════════════════════════════════════════════
# (AR) مصادرُ سائقِ المترجمِ المشتركة — **كلُّها بلا LLVM**.
#      هذه القائمةُ نقطةُ الحقيقةِ الوحيدةُ للمترجمَين: `sad-build` (بـLLVM)
#      و`sad-build-native` (بلا LLVM). ولا يفترقانِ إلّا في ملفٍّ واحد:
#        • `sad-build`        ⟵ compiler_driver_backend.cpp             (يلمس LLVM)
#        • `sad-build-native` ⟵ compiler_driver_backend_llvm_absent.cpp (يرفض صراحةً)
#      وكِلا الملفَّين **يُصرَّفُ كاملًا في هدفِه**، فلا فرعَ ‎#ifdef‎ ميّتٌ ينجرفُ.
# (EN) The shared compiler-driver sources — all LLVM-free. This list is the
#      single source of truth for both compilers; they differ in exactly one TU
#      (the LLVM backend vs. its explicit-rejection counterpart), and each of
#      those is compiled in full in its own target — no dead #ifdef branch.
# ═══════════════════════════════════════════════════════════════════════════
set(SAD_DRIVER_COMMON_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/sad-build/main.cpp
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_frontend.cpp
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_diagnostics.cpp
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_analysis.cpp
    # (AR) المرحلةُ الوسطى (SIR ⇒ SIR) — مشتركةٌ كي لا يختلفَ ناتجُ المترجمَين
    # (EN) Middle end (SIR ⇒ SIR) — shared so both compilers produce the same code
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_middleend.cpp
    # (AR) الخلفيّة الأصليّة (بلا LLVM): SIR ⇒ شيفرةُ آلة ⇒ ELF64 ساكن
    # (EN) Native (no-LLVM) backend: SIR ⇒ machine code ⇒ static ELF64
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_native.cpp
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_cli.cpp
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_linker.cpp
    # (AR) الملفُّ كلُّه داخلَ ‎#ifdef HAS_EMBEDDED_LLD‎ — بلا العَلَمِ لا يُصرَّفُ منه شيء
    # (EN) The whole file sits inside #ifdef HAS_EMBEDDED_LLD — nothing without it
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_lld.cpp
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_build_utils.cpp
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_ui.cpp
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_android.cpp
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_android_linker.cpp
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_pkg.cpp
)

# (AR) مسارُ الترويساتِ المشتركُ بين المترجمَين — بلا مسارات LLVM.
# (EN) The include path shared by both compilers — with no LLVM paths.
set(SAD_DRIVER_COMMON_INCLUDES
    ${CMAKE_SOURCE_DIR}/tools/compiler
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/shared/lexer/include
    ${CMAKE_SOURCE_DIR}/shared/parser/include
    ${CMAKE_SOURCE_DIR}/shared/ast/include
    ${CMAKE_SOURCE_DIR}/shared/types/include
    ${CMAKE_SOURCE_DIR}/shared/errors/include
    # (AR) كتالوجُ الأخطاءِ المولَّد — الرفضُ الصريحُ في الهدفِ النحيلِ يقرأُ منه
    # (EN) The generated error catalog — the thin target reads its rejection from it
    ${CMAKE_SOURCE_DIR}/shared/errors/generated
    # (AR) Phase 3 (F-01): فاحص الأنواع المشترك / (EN) Shared type checker
    ${CMAKE_SOURCE_DIR}/shared/semantic/include
    # (AR) Phase 4 (F-03): utils + modules + hot_reload لتجنب المسارات النسبية
    # (EN) Phase 4 (F-03): utils + modules + hot_reload to avoid relative paths
    ${CMAKE_SOURCE_DIR}/shared/utils/include
    ${CMAKE_SOURCE_DIR}/shared/modules/include
    ${CMAKE_SOURCE_DIR}/shared/hot_reload/include
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}/compiler/include/frontend
    ${CMAKE_SOURCE_DIR}/shared/ownership/include
    ${CMAKE_SOURCE_DIR}/shared/null_safety/include   # (AR) NS-01: رأس محلّل أمان null
    ${CMAKE_SOURCE_DIR}/shared/program_rules/include # (AR) قاعدة الدالة الرئيسية الموحَّدة (SEM018)
    ${CMAKE_SOURCE_DIR}/compiler/include/types
    ${CMAKE_BINARY_DIR}/generated
)

# ──────────────────────────────────────────────────────────────────────
# (AR) دالّةٌ تُطبّقُ الخصائصَ المشتركةَ بين المترجمَين على هدفٍ منهما.
#      وجودُها يمنعُ العطبَ المقيسَ في هذا المستودعِ: «رقعةٌ صحيحةٌ في ملفٍّ
#      واحدٍ تترك أخاه» — فحجزُ المكدّسِ أو عَلَمُ SDL2 يُضبَطُ للاثنين معًا أو لا.
# (EN) One function applies every property both compilers share, so a fix cannot
#      land on one target and miss its sibling.
# ──────────────────────────────────────────────────────────────────────
function(sad_configure_compiler_target target_name)
    target_include_directories(${target_name} PRIVATE ${SAD_DRIVER_COMMON_INCLUDES})
    add_dependencies(${target_name} generate_runtime_header)

    # ═══════════════════════════════════════════════════════════════════════════
    # (AR) حقيقةُ SDL2 على POSIX ⇒ سائقُ الربط. المتغيّراتُ تُضبَط في
    #      features/graphics/CMakeLists.txt عند العثورِ على SDL2 نظاميّة. برامجُ
    #      الواجهةِ المُترجَمةُ تسحبُ رموزَ SDL_*/TTF_* عبورًا من sad_ui_runtime.cpp،
    #      وويندوزُ وحدَه كان يربطُ SDL2 (المُورَّدة) — فبقيَ لينكس/ماك يفشلُ ربطًا.
    #      نمرّرُ المسارَ نصًّا لأنّه لا يُعرَفُ إلّا وقتَ التهيئة.
    # (EN) Hand the POSIX SDL2 fact to the link driver: compiled UI programs pull
    #      SDL_*/TTF_* transitively, and only Windows ever linked SDL2, so Linux and
    #      macOS failed to link. The path is configure-time knowledge, so pass it in.
    # ═══════════════════════════════════════════════════════════════════════════
    if(SAD_UI_HOST_SDL2)
        target_compile_definitions(${target_name} PRIVATE SAD_UI_HOST_SDL2)
        if(SAD_UI_HOST_SDL2_LIB_DIR)
            target_compile_definitions(${target_name} PRIVATE
                SAD_UI_HOST_SDL2_LIB_DIR="${SAD_UI_HOST_SDL2_LIB_DIR}")
        endif()
        if(SAD_UI_HOST_SDL2_TTF)
            target_compile_definitions(${target_name} PRIVATE SAD_UI_HOST_SDL2_TTF)
        endif()
    endif()

    if(MSVC)
        target_compile_options(${target_name} PRIVATE /wd4819 /FS)
    endif()

    set_target_properties(${target_name} PROPERTIES
        OUTPUT_NAME "${target_name}"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    # (AR) زيادة حجم الـ stack إلى 128 ميغابايت لمعالجة الملفات الكبيرة
    # (EN) Increase stack size to 128MB for large source files
    if(MSVC)
        target_link_options(${target_name} PRIVATE /STACK:134217728)
    elseif(APPLE)
        # (AR) رابط macOS (ld64) يستخدم -stack_size لا -z stacksize (خاص بـ GNU ld).
        # (EN) macOS ld64 uses -stack_size, not the GNU ld -z stacksize option.
        target_link_options(${target_name} PRIVATE -Wl,-stack_size,0x8000000)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(${target_name} PRIVATE -Wl,-z,stack-size=134217728)
    endif()
endfunction()

# ══════════════════════════════════════════════════════════════════════════════
# (AR) المترجمُ النحيلُ السياديّ sad-build-native — بلا LLVM البتّة
# (EN) The thin sovereign compiler sad-build-native — no LLVM at all
# ──────────────────────────────────────────────────────────────────────────────
# (AR) يُعرَّفُ **خارجَ** كتلةِ ‎if(ENABLE_LLVM_BACKEND AND LLVM_FOUND)‎ عمدًا. كانت
#      الخلفيّةُ الأصليّةُ — وهي بلا LLVM في مصدرِها — محبوسةً داخلَ هدفٍ لا يُعرَّفُ
#      إلّا إذا وُجدت LLVM، فكانَ إطفاءُ LLVM يُسقِطُ المترجمَ كلَّه ومعه المسارُ
#      الذي لا يحتاجُها. وبهذا الهدفِ يُبنى الوضعُ الحرُّ والخلفيّةُ الأصليّةُ في
#      الخاناتِ الستِّ كلِّها (ثلاثُ منصّاتٍ × Debug/Release).
#      ⚠️ لا يربطُ ‎LLVM_LIBS‎ ولا ‎sad_llvm_backend‎ — فتعارضُ
#      ‎_ITERATOR_DEBUG_LEVEL‎ على MSVC لا يقعُ أصلًا، ويُبنى في Debug بلا شرط.
# (EN) Defined *outside* the LLVM block deliberately. The native backend — which
#      has no LLVM in its sources — used to be locked inside a target that only
#      exists when LLVM is found, so turning LLVM off dropped the whole compiler
#      and with it the one path that never needed LLVM. With this target the
#      freestanding mode and the native backend build in all six cells
#      (three platforms × Debug/Release). It links neither LLVM_LIBS nor
#      sad_llvm_backend, so the MSVC _ITERATOR_DEBUG_LEVEL clash cannot arise.
# ══════════════════════════════════════════════════════════════════════════════
add_executable(sad-build-native
    ${SAD_DRIVER_COMMON_SOURCES}
    ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_backend_llvm_absent.cpp
)

sad_configure_compiler_target(sad-build-native)

target_link_libraries(sad-build-native PRIVATE
    sad_shared sad_compiler_no_llvm sad_mobile sad_null_safety sad_program_rules
)

message(STATUS "✓ المترجم النحيل / Thin compiler: sad-build-native (no LLVM, all configs)")

# ──────────────────────────────────────────────────────────────────────
# المترجم sad-build (يتطلب LLVM) / Compiler sad-build (requires LLVM)
# ──────────────────────────────────────────────────────────────────────
if(ENABLE_LLVM_BACKEND AND LLVM_FOUND)

    # (AR) تهيئة بناء Debug: عند اكتشاف تثبيت LLVM Debug (SAD_LLVM_HAS_DEBUG) لا يُستثنى
    #      sad-build من ALL_BUILD. على غير MSVC البناء في Debug آمن (لا فصل ABI).
    #      ⚠️ هذا الاستثناءُ يخصُّ `sad-build` وحدَه؛ `sad-build-native` أعلاه لا
    #      يعنيه لأنّه لا يربطُ LLVM.
    # (EN) Debug-build setup: when a Debug LLVM is detected (SAD_LLVM_HAS_DEBUG),
    #      sad-build is not excluded from ALL. On non-MSVC, Debug build is ABI-safe.
    #      ⚠️ This exclusion is about sad-build only; sad-build-native above is
    #      unaffected because it links no LLVM.
    if(SAD_LLVM_HAS_DEBUG OR NOT MSVC)
        add_executable(sad-build
            ${SAD_DRIVER_COMMON_SOURCES}
            ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_backend.cpp)
        message(STATUS "   sad-build: all-config (built in Debug + Release)")
    else()
        add_executable(sad-build EXCLUDE_FROM_ALL
            ${SAD_DRIVER_COMMON_SOURCES}
            ${CMAKE_SOURCE_DIR}/tools/compiler/compiler_driver_backend.cpp)
        message(STATUS "   sad-build: Release-only (EXCLUDE_FROM_ALL — MSVC, no Debug LLVM)")
    endif()

    sad_configure_compiler_target(sad-build)

    # (AR) استخدام SAD_LLVM_INCLUDES — يُعرّف في cmake/llvm.cmake مع generator expression
    #      للاختيار بين مسار Debug و Release تلقائياً.
    # (EN) Use SAD_LLVM_INCLUDES — defined in cmake/llvm.cmake with a generator
    #      expression to select Debug vs Release include paths automatically.
    target_include_directories(sad-build PRIVATE
        ${CMAKE_SOURCE_DIR}/compiler/include/backend/llvm
        ${SAD_LLVM_INCLUDES}
    )

    target_link_libraries(sad-build PRIVATE
        sad_shared sad_compiler sad_mobile sad_null_safety sad_program_rules ${LLVM_LIBS}
    )

    # ═══════════════════════════════════════════════════════════════════════════
    # (AR) إصلاح الربط الدائري لمكتبات LLVM الساكنة في وضع Debug (assertions) — MSVC.
    #      إعادة سرد المكتبات المُعرِّفة في نهاية سطر الربط (مكافئ --start-group).
    # (EN) Fix circular static-lib link order for LLVM Debug (assertions) on MSVC:
    #      re-list the defining libs at the END of the link line (--start-group equiv).
    # ═══════════════════════════════════════════════════════════════════════════
    if(SAD_LLVM_HAS_DEBUG AND MSVC)
        set(_sad_llvm_debug_relist
            LLVMSelectionDAG LLVMTransformUtils LLVMCodeGen LLVMAnalysis
            LLVMScalarOpts LLVMMIRParser LLVMAsmPrinter LLVMMC LLVMMCParser
            LLVMTarget LLVMCore LLVMSupport LLVMBitReader LLVMipo LLVMInstCombine
        )
        foreach(_relib ${_sad_llvm_debug_relist})
            # (AR) اسم مجرّد يُحَلّ عبر link_directories المُدرِك للتهيئة (Debug فقط)
            # (EN) Bare name resolved via config-aware link dir (Debug only)
            target_link_libraries(sad-build PRIVATE
                "$<$<CONFIG:Debug>:${_relib}.lib>")
        endforeach()

        # (AR) مكتبات نظام Windows التي تعتمدها مكتبات LLVM (Winsock في LLVMSupport…).
        # (EN) Windows system libs that LLVM depends on (Winsock in LLVMSupport…).
        target_link_libraries(sad-build PRIVATE
            ws2_32 ntdll advapi32 shell32 ole32 uuid psapi)
    endif()

    # ربط مكتبات LLD إذا كانت متوفرة / Link LLD libraries if available
    if(HAS_EMBEDDED_LLD)
        target_link_libraries(sad-build PRIVATE ${LLD_LIBS})
        target_compile_definitions(sad-build PRIVATE HAS_EMBEDDED_LLD)
        message(STATUS "   sad-build: LLD مدمج / embedded LLD enabled")
    endif()

    message(STATUS "✓ المترجم / Compiler: sad-build (LLVM ${LLVM_PACKAGE_VERSION})")
else()
    message(STATUS "⊘ المترجم sad-build معطّل (LLVM غير متوفر) / sad-build disabled")
endif()
