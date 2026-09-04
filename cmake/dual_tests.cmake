# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# ملف: cmake/dual_tests.cmake
# الوصف: اختبارات التنفيذ المزدوج — مقارنة المفسر والمترجم (ADR-03)
# ═══════════════════════════════════════════════════════════════════════════════
#
# (AR) هذا الملف يُعرّف أهداف cmake لتشغيل اختبارات dual_execution عبر runner.py.
#      يتطلب Python 3 لتشغيل runner.py.
#
#      الأهداف:
#        dual_tests_p0 — اختبارات أساسية (كل commit)
#        dual_tests_p1 — اختبارات كاملة (كل PR)
#        dual_tests_p2 — اختبارات متقدمة (Nightly)
#        dual_tests    — جميع الاختبارات
#
# (EN) Defines cmake targets for running dual_execution tests via runner.py.
#      Requires Python 3.
# ═══════════════════════════════════════════════════════════════════════════════

# (AR) البحث عن Python 3
find_package(Python3 COMPONENTS Interpreter QUIET)

if(NOT Python3_FOUND)
    message(STATUS "⚠ Python3 غير متاح — اختبارات dual_execution معطّلة")
    return()
endif()

set(DUAL_RUNNER "${CMAKE_SOURCE_DIR}/tests/runner.py")
# (AR) 🔑 مسارُ المحرّكِ الواحد: لا مفسّر — العدّاءُ يُشغَّلُ بالمترجمِ وحدَه.
set(DUAL_COMPILER "$<TARGET_FILE:sad-build>")


# (AR) التحقق من وجود runner.py
if(NOT EXISTS "${DUAL_RUNNER}")
    message(STATUS "⚠ runner.py غير موجود — اختبارات dual_execution معطّلة")
    return()
endif()

# ══════════════════════════════════════════════════════════════════════════
# (AR) 🔑 حارسُ عقدِ مسارِ المحرّك — **يُسجَّلُ قبلَ بوّابةِ `sad-build`**.
#      لا يحتاجُ مُصرِّفًا ولا محرّكًا مبنيًّا: أمرُه پايثونٌ خالصٌ يستوردُ
#      `tests/runner.py` وحدةً ويفحصُ عقدَ المسار. فلو بقيَ خلفَ البوّابةِ
#      أدناه لاختفى من `ctest -N` في كلِّ تهيئةٍ بلا LLVM **بلا أثر**.
#
#      ⚠️ وهذا هو الدرسُ المدوَّنُ في `cmake/tests.cmake:464` بعينِه:
#      «كان الشرطُ `TARGET sad-run`. فلمّا زال الهدفُ سقطتِ البوّابةُ صامتةً
#      … وهذا أخبثُ من إخفاق». فلا يُعادُ ارتكابُه في الملفِّ المجاور.
#
#      ويلزمُه `runner.py` (يستوردُه)، فمَوضِعُه بعدَ التحقّقِ من وجودِه.
# (EN) 🔑 Engine-path contract guard — registered BEFORE the sad-build gate.
#      It needs neither compiler nor built engine: its command is pure Python
#      importing tests/runner.py as a module. Left behind the gate below it
#      would silently vanish from `ctest -N` in every LLVM-off configuration —
#      the exact lesson already recorded at cmake/tests.cmake:464. It does need
#      runner.py (it imports it), hence its placement after that check.
# ══════════════════════════════════════════════════════════════════════════
if(BUILD_TESTS)
    add_test(
        NAME DualRunner_EnginePathContract
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE}
                "${CMAKE_SOURCE_DIR}/tests/system/harness/test_engine_path_is_absolute.py"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualRunner_EnginePathContract PROPERTIES
        LABELS "harness;dual_execution;contract"
        TIMEOUT 120
    )
endif()

# ══════════════════════════════════════════════════════════════════════════
# (AR) 🔑 الهدفُ **مشروطٌ**، فلا يُذكَرُ ذكرًا غيرَ مشروط.
#
#      كان هنا `$<TARGET_FILE:sad-run>`، وهدفُ المفسّرِ يُعرَّفُ دائمًا، فلم
#      يكن للسؤالِ موضع. ولمّا حلَّ `sad-build` محلَّه تبدَّلَ الأساسُ ولم
#      يتبدَّلِ الشرط: `sad-build` يُعرَّفُ داخلَ بوّابةِ الخلفيّةِ LLVM
#      (apps/CMakeLists.txt) فلا وجودَ له حين `ENABLE_LLVM_BACKEND=OFF`.
#
#      وهذا مقيسٌ لا مُتوقَّع: بوّابةُ الرسوماتِ للمنصّاتِ الثلاثِ تُهيّئُ
#      بـ`-DENABLE_LLVM_BACKEND=OFF`، فأخفقتِ التهيئةُ في ويندوز ولينكس
#      وماك جميعًا بـ`No target "sad-build"` من `add_test`.
#
#      والسابقةُ قائمةٌ في هذا المستودعِ نفسِه: `doc_gen_dual_tests.cmake`
#      يسألُ `if(TARGET sad-build)` قبلَ أن يعتمدَ عليه.
#
#      ⚠️ ولا يُسكَتُ الحارسُ: التعطيلُ يُعلَنُ في سجلِّ التهيئةِ
#      بسببِه، فمَن قرأَ السجلَّ عرفَ لماذا غابت هذه الاختبارات. وهي لا
#      تستطيعُ العملَ بحالٍ — في مسارِ المحرّكِ الواحدِ المترجِمُ **هو**
#      المحرّك، فلا بديلَ عنه يُشغَّلُ به العدّاء.
# (EN) 🔑 The target is CONDITIONAL, so it is not named unconditionally.
#
#      This used to be $<TARGET_FILE:sad-run>, and the interpreter target was
#      always defined, so the question never arose. When sad-build replaced it
#      the ground changed but the condition did not: sad-build is defined inside
#      the LLVM-backend gate, so it does not exist under ENABLE_LLVM_BACKEND=OFF.
#      Measured, not predicted: the graphics platform gate configures with LLVM
#      off and its configure step failed on all three platforms with
#      `No target "sad-build"`. The precedent already exists here —
#      doc_gen_dual_tests.cmake asks `if(TARGET sad-build)` before depending on it.
#      ⚠️ The guard is not silenced: the skip is announced in the configure
#      log with its reason. These tests cannot run at all without it — on the
#      single-engine track the compiler IS the engine.
# ══════════════════════════════════════════════════════════════════════════
if(NOT TARGET sad-build)
    message(STATUS "⚠ الهدف sad-build غير مُعرَّف (ENABLE_LLVM_BACKEND=OFF) — اختبارات dual_execution معطّلة")
    return()
endif()

message(STATUS "✓ اختبارات التنفيذ المزدوج / Dual execution tests enabled")

# ──────────────────────────────────────────────────────────────────────
# (AR) معاملات مشتركة
# ──────────────────────────────────────────────────────────────────────
set(_DUAL_COMMON_ARGS
    "${CMAKE_COMMAND}" -E env
    "PYTHONIOENCODING=utf-8"
    "PYTHONUTF8=1"
    "${Python3_EXECUTABLE}" "${DUAL_RUNNER}"
    --compiler "${DUAL_COMPILER}"
    --report
    --no-color
)

# (AR) معاملات خاصة بالاختبارات الحرجة: مفسر فقط
#      هذه الاختبارات موسومة @skip_compiler وبالتالي لا تحتاج تمرير --compiler
#      تجنب هذا يمنع سحب هدف sad-build في Debug عندما يكون LLVM مربوطاً بنمط Release.
set(_DUAL_CRITICAL_ARGS
    "${CMAKE_COMMAND}" -E env
    "PYTHONIOENCODING=utf-8"
    "PYTHONUTF8=1"
    "${Python3_EXECUTABLE}" "${DUAL_RUNNER}"
    --compiler "${DUAL_COMPILER}"
    --report
    --no-color
)


# ──────────────────────────────────────────────────────────────────────
# P0: اختبارات أساسية — core فقط (كل commit)
# ──────────────────────────────────────────────────────────────────────
add_custom_target(dual_tests_p0
    COMMAND ${_DUAL_COMMON_ARGS} --level P0
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "🔄 تشغيل اختبارات P0 (أساسية) — behavior/P0_smoke"
    DEPENDS sad-build
    USES_TERMINAL
)

# ──────────────────────────────────────────────────────────────────────
# P1: اختبارات كاملة — core + oop (كل PR)
# ──────────────────────────────────────────────────────────────────────
add_custom_target(dual_tests_p1
    COMMAND ${_DUAL_COMMON_ARGS} --level P1
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "🔄 تشغيل اختبارات P1 (كاملة) — behavior (P1: أساسيات + أنواع)"
    DEPENDS sad-build
    USES_TERMINAL
)

# ──────────────────────────────────────────────────────────────────────
# P2: اختبارات متقدمة — كل المجلدات (Nightly)
# ──────────────────────────────────────────────────────────────────────
add_custom_target(dual_tests_p2
    COMMAND ${_DUAL_COMMON_ARGS} --level P2
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "🔄 تشغيل اختبارات P2 (متقدمة) — behavior/sections/*"
    DEPENDS sad-build
    USES_TERMINAL
)

# ──────────────────────────────────────────────────────────────────────
# Critical Safety: اختبارات حرجة — ضغط التزامن + تدفق الاستثناء + سلبيات + تقاطع + مركب
# ──────────────────────────────────────────────────────────────────────
add_custom_target(dual_tests_critical
    COMMAND ${_DUAL_CRITICAL_ARGS} --file 027_concurrency_stress_suite.ص
    COMMAND ${_DUAL_CRITICAL_ARGS} --file 028_concurrency_negative_suite.ص
    COMMAND ${_DUAL_CRITICAL_ARGS} --file 035_exception_flow_control_suite.ص
    COMMAND ${_DUAL_CRITICAL_ARGS} --file 036_exception_concurrency_cross.ص
    COMMAND ${_DUAL_CRITICAL_ARGS} --file 037_compound_high_density.ص
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "🚨 تشغيل الاختبارات الحرجة: التزامن + الاستثناءات + السلبيات + التقاطع + المركب"
    DEPENDS sad-build
    USES_TERMINAL
)

# ──────────────────────────────────────────────────────────────────────
# Burn-in: تشغيل تكراري 50 جولة لاكتشاف flakiness والتسربات
# ──────────────────────────────────────────────────────────────────────
add_custom_target(dual_tests_burnin
    COMMAND ${_DUAL_CRITICAL_ARGS} --file 027_concurrency_stress_suite.ص --repeat 50
    COMMAND ${_DUAL_CRITICAL_ARGS} --file 035_exception_flow_control_suite.ص --repeat 50
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "🔥 Burn-in: 50 جولة للسويتين الحرجتين (اكتشاف flakiness)"
    DEPENDS sad-build
    USES_TERMINAL
)

# ──────────────────────────────────────────────────────────────────────
# الهدف الشامل — جميع الاختبارات
# ──────────────────────────────────────────────────────────────────────
add_custom_target(dual_tests
    COMMAND ${_DUAL_COMMON_ARGS}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "🔄 تشغيل جميع اختبارات التنفيذ المزدوج — behavior"
    DEPENDS sad-build
    USES_TERMINAL
)

# ──────────────────────────────────────────────────────────────────────
# (AR) تسجيل في CTest (اختياري)
# ──────────────────────────────────────────────────────────────────────
if(BUILD_TESTS)
    add_test(
        NAME DualExecution_P0
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --level P0 --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_P0 PROPERTIES
        LABELS "P0;dual_execution"
        TIMEOUT 120
    )

    add_test(
        NAME DualExecution_P1
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --level P1 --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_P1 PROPERTIES
        LABELS "P1;dual_execution"
        TIMEOUT 300
    )

    add_test(
        NAME DualExecution_Critical_Concurrency
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --file 027_concurrency_stress_suite.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Critical_Concurrency PROPERTIES
        LABELS "critical;dual_execution;concurrency;safety"
        TIMEOUT 180
    )

    add_test(
        NAME DualExecution_Critical_ExceptionFlow
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --file 035_exception_flow_control_suite.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Critical_ExceptionFlow PROPERTIES
        LABELS "critical;dual_execution;exception;safety"
        TIMEOUT 180
    )

    add_test(
        NAME DualExecution_Critical_ConcurrencyNegative
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --file 028_concurrency_negative_suite.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Critical_ConcurrencyNegative PROPERTIES
        LABELS "critical;dual_execution;concurrency;safety;negative"
        TIMEOUT 120
    )

    add_test(
        NAME DualExecution_Critical_ExceptionConcurrencyCross
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --file 036_exception_concurrency_cross.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Critical_ExceptionConcurrencyCross PROPERTIES
        LABELS "critical;dual_execution;exception;concurrency;safety"
        TIMEOUT 120
    )

    add_test(
        NAME DualExecution_Critical_CompoundHighDensity
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --file 037_compound_high_density.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Critical_CompoundHighDensity PROPERTIES
        LABELS "critical;dual_execution;compound;safety"
        TIMEOUT 120
    )

    # ──────────────────────────────────────────────────────────────────────
    # (AR) اختبارات تغطية الفجوات — Milestone 2
    # ──────────────────────────────────────────────────────────────────────
    add_test(
        NAME DualExecution_Coverage_Lambda
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --file 038_lambda_expressions.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Coverage_Lambda PROPERTIES
        LABELS "coverage;dual_execution;lambda;core"
        TIMEOUT 60
    )

    add_test(
        NAME DualExecution_Coverage_StringsFStrings
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --file 039_string_methods_fstrings.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Coverage_StringsFStrings PROPERTIES
        LABELS "coverage;dual_execution;strings;core"
        TIMEOUT 60
    )

    add_test(
        NAME DualExecution_Coverage_LogicalOperators
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --file 040_logical_operators_arabic.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Coverage_LogicalOperators PROPERTIES
        LABELS "coverage;dual_execution;operators;core"
        TIMEOUT 60
    )

    add_test(
        NAME DualExecution_Coverage_BreakContinue
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --file 041_break_continue_nested.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Coverage_BreakContinue PROPERTIES
        LABELS "coverage;dual_execution;loops;core"
        TIMEOUT 60
    )

    add_test(
        NAME DualExecution_Coverage_StructsAdvanced
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --file 042_structs_advanced.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Coverage_StructsAdvanced PROPERTIES
        LABELS "coverage;dual_execution;structs;oop"
        TIMEOUT 60
    )

    add_test(
        NAME DualExecution_Coverage_ContextualMapsMembership
        COMMAND ${CMAKE_COMMAND} -E env
            "PYTHONIOENCODING=utf-8"
            "PYTHONUTF8=1"
            ${Python3_EXECUTABLE} "${DUAL_RUNNER}"
                --compiler "${DUAL_COMPILER}"
                --file 043_contextual_maps_membership.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Coverage_ContextualMapsMembership PROPERTIES
        LABELS "coverage;dual_execution;maps;membership;core"
        TIMEOUT 60
    )

endif()
