# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/executables.cmake
# الوصف: الملفات التنفيذية الرئيسية / Main Executable Targets
# ═══════════════════════════════════════════════════════════════════════════════

# ──────────────────────────────────────────────────────────────────────
# المفسر الرئيسي / Main Interpreter (sad-run)
# (AR) اسم الهدف موحَّد مع اسم المخرج (sad-run.exe) — RFC «توحيد أسماء أهداف البناء».
#      كان الهدف سابقاً اسمه `sad` بمخرج `sad-run`؛ هذا الانفصام أُزيل لمنع
#      الثنائيات البائتة (stale) واضطراب أدوات الاختبار. الأمر:
#      cmake --build build --config Debug --target sad-run
# (EN) Target name unified with output name (sad-run.exe) — Build-Target-Naming RFC.
#      Formerly named `sad` with output `sad-run`; the split is removed to prevent
#      stale binaries and test-tooling confusion.
# ──────────────────────────────────────────────────────────────────────
add_executable(sad-run tools/compiler/main_simple.cpp)

target_link_libraries(sad-run PRIVATE
    sad_core
    sad_vm
    sad_type_system
    sad_semantic
    sad_network
    sad_http
    sad_websocket
    sad_mobile
    sad_ui
    sad_memory_policy   # (AR) Phase A2: سياسة الذاكرة الموحَّدة / (EN) Phase A2: unified memory policy
    sad_null_safety     # (AR) NS-01: نظام أمان null المشترك / (EN) NS-01: shared null-safety
)

# (AR) ربط مكتبة مصحح الأداء / (EN) Link profiler library
if(TARGET sad_profiler_lib)
    target_link_libraries(sad-run PRIVATE sad_profiler_lib)
endif()

# (AR) ربط مكتبة وقت التشغيل إذا تم بناؤها / (EN) Link runtime if built
if(TARGET sad_rt_runtime)
    target_link_libraries(sad-run PRIVATE sad_rt_runtime)
endif()

target_include_directories(sad-run PRIVATE
    ${CMAKE_SOURCE_DIR}/vm/include
    ${CMAKE_SOURCE_DIR}/compiler/include
    ${CMAKE_SOURCE_DIR}/shared/ownership/include
    ${CMAKE_SOURCE_DIR}/shared/null_safety/include   # (AR) NS-01: رأس محلّل أمان null
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

message(STATUS "✓ المفسر / Interpreter: sad-run (الهدف: sad-run)")

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
    tools/compiler/src/run_command.cpp
    tools/compiler/src/test_command.cpp
    tools/compiler/src/build_command.cpp
    shared/parser/src/specs/ui/parser_ui_maps.cpp
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

# ──────────────────────────────────────────────────────────────────────
# المترجم sad-build (يتطلب LLVM) / Compiler sad-build (requires LLVM)
# (AR) اسم الهدف موحَّد مع اسم المخرج (sad-build.exe) — RFC «توحيد أسماء أهداف البناء».
#      كان الهدف سابقاً اسمه `sadc` بمخرج `sad-build`؛ هذا الانفصام أوهَم أدوات
#      الاختبار أنّ ثنائياً باسم `sadc.exe` موجود (ولا هدف يُنتجه)، فبقي بائتاً.
#      الأمر بعد التوحيد: cmake --build build --config Debug --target sad-build
# (EN) Target name unified with output name (sad-build.exe) — Build-Target-Naming RFC.
#      Formerly named `sadc` with output `sad-build`; the split misled test tooling
#      into expecting a non-existent `sadc.exe`, leaving it stale.
# ──────────────────────────────────────────────────────────────────────
if(ENABLE_LLVM_BACKEND AND LLVM_FOUND)

    # ================================================================
    # (AR) تضمين ملف runtime كبيانات في C++
    # (EN) Embed runtime C file as C++ data header
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

    # (AR) مصادر المترجم — مُعرَّفة مرّة واحدة لتفادي التكرار (CW-19 DRY)
    # (EN) Compiler sources — defined once to avoid duplication (CW-19 DRY)
    set(SAD_BUILD_SOURCES
        tools/compiler/main.cpp
        tools/compiler/compiler_driver_frontend.cpp
        tools/compiler/compiler_driver_diagnostics.cpp
        tools/compiler/compiler_driver_analysis.cpp
        tools/compiler/compiler_driver_backend.cpp
        tools/compiler/compiler_driver_cli.cpp
        tools/compiler/compiler_driver_linker.cpp
        tools/compiler/compiler_driver_lld.cpp
        tools/compiler/compiler_driver_build_utils.cpp
        tools/compiler/compiler_driver_ui.cpp
        tools/compiler/compiler_driver_android.cpp
        tools/compiler/compiler_driver_android_linker.cpp
        tools/compiler/compiler_driver_pkg.cpp
    )

    # (AR) تهيئة بناء Debug: عند اكتشاف تثبيت LLVM Debug (SAD_LLVM_HAS_DEBUG، مثلاً
    #      C:/llvm_dev/LLVM-Debug) لا يُستثنى sad-build من ALL_BUILD ويُتاح هدفه في
    #      كلا الوضعين. تنبيه: ربط Debug ينجح فقط مع LLVM Debug مُطابق ABI تمامًا؛
    #      وإلا تتسرّب مكتبات Release (LLVMPasses/Coroutines) عبر link_directories
    #      فينشأ تعارض _ITERATOR_DEBUG_LEVEL/RuntimeLibrary — والمسار المدعوم عمليًّا
    #      هو بناء sad-build في Release (وهو ما يقرأه runner.py وtests/config.yaml).
    # (EN) Debug-build setup: when a Debug LLVM install is detected
    #      (SAD_LLVM_HAS_DEBUG), sad-build is not excluded from ALL_BUILD and its
    #      target is available in both configs. NOTE: Debug linking only succeeds
    #      with a fully ABI-consistent Debug LLVM; otherwise Release libs
    #      (LLVMPasses/Coroutines) leak via link_directories causing
    #      _ITERATOR_DEBUG_LEVEL/RuntimeLibrary mismatch. The practically supported
    #      path is building sad-build in Release (what runner.py/config.yaml read).
    # (AR) قيد ربط Debug خاصّ بـMSVC وحده: تعارض _ITERATOR_DEBUG_LEVEL/RuntimeLibrary
    #      ينشأ فقط حين تُربط مكتبات LLVM Release في تهيئة Debug على MSVC بلا LLVM
    #      Debug مطابق. على Linux/GCC/Clang لا يوجد هذا الفصل ABI، فبناء sad-build في
    #      Debug آمن — وهو لازم لتشغيل اختبارات DualExecution/DocGen في وظيفة Debug.
    # (EN) The Debug-linking constraint is MSVC-only: the _ITERATOR_DEBUG_LEVEL/
    #      RuntimeLibrary mismatch occurs solely when Release LLVM libs link into a
    #      Debug config on MSVC without a matching Debug LLVM. On Linux/GCC/Clang
    #      there is no such ABI split, so building sad-build in Debug is safe — and
    #      it is required to run the DualExecution/DocGen tests in the Debug job.
    if(SAD_LLVM_HAS_DEBUG OR NOT MSVC)
        add_executable(sad-build ${SAD_BUILD_SOURCES})
        message(STATUS "   sad-build: all-config (built in Debug + Release)")
    else()
        add_executable(sad-build EXCLUDE_FROM_ALL ${SAD_BUILD_SOURCES})
        message(STATUS "   sad-build: Release-only (EXCLUDE_FROM_ALL — MSVC, no Debug LLVM)")
    endif()

    # (AR) استخدام SAD_LLVM_INCLUDES — يُعرّف في cmake/llvm.cmake مع generator expression
    #      للاختيار بين مسار Debug و Release تلقائياً.
    # (EN) Use SAD_LLVM_INCLUDES — defined in cmake/llvm.cmake with generator expression
    #      to automatically select between Debug and Release paths.

    target_include_directories(sad-build PRIVATE
        ${CMAKE_SOURCE_DIR}/tools/compiler
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
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
        ${CMAKE_SOURCE_DIR}/compiler/include/types
        ${CMAKE_SOURCE_DIR}/compiler/include/backend/llvm
        ${SAD_LLVM_INCLUDES}
        ${CMAKE_BINARY_DIR}/generated
    )

    # (AR) ضمان توليد الهيدر قبل بناء sad-build
    # (EN) Ensure runtime header is generated before building sad-build
    add_dependencies(sad-build generate_runtime_header)

    target_link_libraries(sad-build PRIVATE
        sad_shared sad_compiler sad_mobile sad_null_safety ${LLVM_LIBS}
    )

    # ═══════════════════════════════════════════════════════════════════════════
    # (AR) إصلاح الربط الدائري لمكتبات LLVM الساكنة في وضع Debug (assertions)
    # ───────────────────────────────────────────────────────────────────────────
    # في تثبيت LLVM Debug، التوكيدات مفعّلة فتُشير مكتبات الخلفيات المعمارية
    # (Hexagon/PowerPC/BPF/MSP430/VE…) إلى رموز التفريغ والتوسيع المعرَّفة في
    # مكتبات أساسية: SDNode::dump/dumpr في LLVMSelectionDAG، و
    # SCEVExpander::expandCodeFor في LLVMTransformUtils. رابط MSVC يعالج كل مكتبة
    # ساكنة مرّة واحدة بترتيبها، فالرموز التي تُطلَب *بعد* معالجة مكتبتها تبقى غير
    # محلولة (LNK2001/LNK2019). في Release لا تظهر لأن دوال dump مُستبعَدة بـNDEBUG.
    #
    # الحل المشروع (مكافئ ‎--start-group/--end-group‎ على GNU): إعادة سرد المكتبات
    # المُعرِّفة في *نهاية* سطر الربط لتهيئة Debug فقط، فيمنح الرابطَ مرورًا ثانيًا
    # يحلّ الإشارات العكسية. ليس ترقيعًا بل النمط القياسي لاعتمادات المكتبات الساكنة
    # الدائرية على MSVC.
    #
    # (EN) Fix circular static-lib link order for LLVM Debug (assertions) build:
    # re-list the defining libs at the END of the Debug link line so MSVC's linker
    # gets a second pass to resolve the back-references (GNU --start-group equiv).
    # ═══════════════════════════════════════════════════════════════════════════
    if(SAD_LLVM_HAS_DEBUG AND MSVC)
        # (AR) المكتبات الأساسية التي تُشير إليها خلفيات المعالجات (assertions on)
        #      عبر إشارات عكسية: التفريغ/التوسيع/تحليل/تحويل/مُحلِّل MIR…
        # (EN) Core libs referenced back by per-target backends in the assertions
        #      build: dump/expand/analysis/transform/MIR-parser…
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

        # (AR) مكتبات نظام Windows التي تعتمدها مكتبات LLVM (Winsock في
        #      LLVMSupport: WSAStartup/socket/bind…، إضافةً لاعتمادات النظام
        #      المعتادة). نسرد كل مكتبات Debug بالأسماء، فلا يأتي اعتماد النظام
        #      تلقائيًا من إغلاق llvm_map — لذا نضيفه صراحةً.
        # (EN) Windows system libs that LLVM depends on (Winsock in LLVMSupport:
        #      WSAStartup/socket/bind…, plus the usual system deps). Since we list
        #      Debug libs by bare name, the system deps no longer come transitively
        #      from llvm_map's closure — so we add them explicitly.
        target_link_libraries(sad-build PRIVATE
            ws2_32 ntdll advapi32 shell32 ole32 uuid psapi)
    endif()

    # ربط مكتبات LLD إذا كانت متوفرة / Link LLD libraries if available
    if(HAS_EMBEDDED_LLD)
        target_link_libraries(sad-build PRIVATE ${LLD_LIBS})
        target_compile_definitions(sad-build PRIVATE HAS_EMBEDDED_LLD)
        message(STATUS "   sad-build: LLD مدمج / embedded LLD enabled")
    endif()

    if(MSVC)
        target_compile_options(sad-build PRIVATE /wd4819 /FS)
    endif()

    set_target_properties(sad-build PROPERTIES
        OUTPUT_NAME "sad-build"
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    )

    # (AR) زيادة حجم الـ stack إلى 128 ميغابايت لمعالجة الملفات الكبيرة
    # (EN) Increase stack size to 128MB for large source files
    if(MSVC)
        target_link_options(sad-build PRIVATE /STACK:134217728)
    elseif(APPLE)
        # (AR) رابط macOS (ld64) يستخدم -stack_size لا -z stacksize (خاص بـ GNU ld).
        # (EN) macOS ld64 uses -stack_size, not the GNU ld -z stacksize option.
        target_link_options(sad-build PRIVATE -Wl,-stack_size,0x8000000)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_link_options(sad-build PRIVATE -Wl,-z,stacksize=134217728)
    endif()

    message(STATUS "✓ المترجم / Compiler: sad-build (LLVM ${LLVM_PACKAGE_VERSION})")
else()
    message(STATUS "⊘ المترجم sad-build معطّل (LLVM غير متوفر) / sad-build disabled")
endif()

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
