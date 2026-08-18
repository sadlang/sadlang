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
set(DUAL_INTERP "$<TARGET_FILE:sad-run>")

# (AR) التحقق من وجود runner.py
if(NOT EXISTS "${DUAL_RUNNER}")
    message(STATUS "⚠ runner.py غير موجود — اختبارات dual_execution معطّلة")
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
    --interp "${DUAL_INTERP}"
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
    --interp "${DUAL_INTERP}"
    --report
    --no-color
)

# (AR) إذا كان المترجم مُفعّلاً، نضيف مساره
if(TARGET sad-build)
    list(APPEND _DUAL_COMMON_ARGS --compiler "$<TARGET_FILE:sad-build>")
endif()

# ──────────────────────────────────────────────────────────────────────
# P0: اختبارات أساسية — core فقط (كل commit)
# ──────────────────────────────────────────────────────────────────────
add_custom_target(dual_tests_p0
    COMMAND ${_DUAL_COMMON_ARGS} --level P0
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "🔄 تشغيل اختبارات P0 (أساسية) — behavior/P0_smoke"
    DEPENDS sad-run
    USES_TERMINAL
)

# ──────────────────────────────────────────────────────────────────────
# P1: اختبارات كاملة — core + oop (كل PR)
# ──────────────────────────────────────────────────────────────────────
add_custom_target(dual_tests_p1
    COMMAND ${_DUAL_COMMON_ARGS} --level P1
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "🔄 تشغيل اختبارات P1 (كاملة) — behavior (P1: أساسيات + أنواع)"
    DEPENDS sad-run
    USES_TERMINAL
)

# ──────────────────────────────────────────────────────────────────────
# P2: اختبارات متقدمة — كل المجلدات (Nightly)
# ──────────────────────────────────────────────────────────────────────
add_custom_target(dual_tests_p2
    COMMAND ${_DUAL_COMMON_ARGS} --level P2
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "🔄 تشغيل اختبارات P2 (متقدمة) — behavior/sections/*"
    DEPENDS sad-run
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
    DEPENDS sad-run
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
    DEPENDS sad-run
    USES_TERMINAL
)

# ──────────────────────────────────────────────────────────────────────
# الهدف الشامل — جميع الاختبارات
# ──────────────────────────────────────────────────────────────────────
add_custom_target(dual_tests
    COMMAND ${_DUAL_COMMON_ARGS}
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMENT "🔄 تشغيل جميع اختبارات التنفيذ المزدوج — behavior"
    DEPENDS sad-run
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
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
                --interp "${DUAL_INTERP}"
                --file 043_contextual_maps_membership.ص --no-color
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    set_tests_properties(DualExecution_Coverage_ContextualMapsMembership PROPERTIES
        LABELS "coverage;dual_execution;maps;membership;core"
        TIMEOUT 60
    )

    # ──────────────────────────────────────────────────────────────────
    # (AR) حارسُ عقدِ مسارِ المحرّك — لا يحتاج مُصرِّفًا ولا محرّكًا مبنيًّا.
    #      كلُّ اختباراتِ DualExecution أعلاه تمرّرُ `$<TARGET_FILE:sad-run>`
    #      وهو **مطلقٌ** بحكمِ CMake، فتنجو من العطبِ ولا تكشفُه. والذي كشفَه
    #      هو خطوةُ CI التي تمرّرُ `build/bin/sad-run` نسبيًّا — وكانت تبتلعُ
    #      إخفاقَها. فالحارسُ هنا يُثبِّتُ العقدَ حيثما نُودِيَ المُشغِّل.
    # (EN) Engine-path contract guard: needs neither compiler nor built engine.
    #      Every DualExecution test above passes an absolute $<TARGET_FILE:...>,
    #      so they survive the defect without exposing it; the CI step that
    #      passes a relative path is what broke — and it swallowed its failure.
    # ──────────────────────────────────────────────────────────────────
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
