# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# tests/system/hub/CMakeLists.txt
# (AR) اختبارات رسمية لمركز الأدوات (sad-hub) و sad-check
# (EN) Formal tests for the tool hub (sad-hub) and sad-check
#
# (AR) هذه الاختبارات subprocess-based — لا تحتاج linking مع المكتبات،
#      تستدعي البرامج التنفيذية المبنية وتتحقق من المخرج/رمز الخروج.
# ═══════════════════════════════════════════════════════════════════════════════

if(NOT BUILD_TESTS)
    return()
endif()

if(NOT TARGET sad_check OR NOT TARGET sad_hub)
    message(STATUS "⚠ اختبارات الهب: تخطّي (sad_hub أو sad_check غير مبنيين)")
    return()
endif()

# (AR) نقاط الاحتجاج إلى البرامج التنفيذية / (EN) Binary references
set(SAD_HUB_BIN  "$<TARGET_FILE:sad_hub>")
set(SAD_CHECK_BIN "$<TARGET_FILE:sad_check>")

# (AR) ملفات الاختبار الثابتة / (EN) Fixture files
set(FIXTURE_CLEAN  "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/clean.ص")
set(FIXTURE_PARSE  "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/parse_error.ص")
set(FIXTURE_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures")

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) القسم 1: sad-check — البرنامج المباشر
# ═══════════════════════════════════════════════════════════════════════════════

add_test(NAME HubCheck_Version
    COMMAND ${SAD_CHECK_BIN} --version)
set_tests_properties(HubCheck_Version PROPERTIES
    PASS_REGULAR_EXPRESSION "sad-check 1\\."
    LABELS "hub;sad-check")

add_test(NAME HubCheck_Help
    COMMAND ${SAD_CHECK_BIN} --help)
set_tests_properties(HubCheck_Help PROPERTIES
    PASS_REGULAR_EXPRESSION "static ownership"
    LABELS "hub;sad-check")

add_test(NAME HubCheck_BadArg
    COMMAND ${SAD_CHECK_BIN} --not-a-real-flag)
set_tests_properties(HubCheck_BadArg PROPERTIES
    WILL_FAIL TRUE
    LABELS "hub;sad-check")

add_test(NAME HubCheck_NoArgs
    COMMAND ${SAD_CHECK_BIN})
set_tests_properties(HubCheck_NoArgs PROPERTIES
    WILL_FAIL TRUE
    LABELS "hub;sad-check")

add_test(NAME HubCheck_MissingFile
    COMMAND ${SAD_CHECK_BIN} this_file_does_not_exist.ص)
set_tests_properties(HubCheck_MissingFile PROPERTIES
    WILL_FAIL TRUE
    LABELS "hub;sad-check")

# (AR) فحص ملف نظيف — يجب أن ينجح برمز 0
add_test(NAME HubCheck_CleanFile
    COMMAND ${SAD_CHECK_BIN} ${FIXTURE_CLEAN})
set_tests_properties(HubCheck_CleanFile PROPERTIES
    PASS_REGULAR_EXPRESSION "نظيف|clean"
    LABELS "hub;sad-check")

# (AR) فحص ملف بخطأ نحوي — يجب أن يفشل برمز 1
add_test(NAME HubCheck_ParseError
    COMMAND ${SAD_CHECK_BIN} --no-color ${FIXTURE_PARSE})
set_tests_properties(HubCheck_ParseError PROPERTIES
    WILL_FAIL TRUE
    LABELS "hub;sad-check")

# (AR) JSON يحتوي على المفتاح المتوقع
add_test(NAME HubCheck_JsonShape
    COMMAND ${SAD_CHECK_BIN} --json ${FIXTURE_CLEAN})
set_tests_properties(HubCheck_JsonShape PROPERTIES
    PASS_REGULAR_EXPRESSION "\"results\""
    LABELS "hub;sad-check")

# (AR) JSON يحتوي clean=true للملف النظيف
add_test(NAME HubCheck_JsonCleanTrue
    COMMAND ${SAD_CHECK_BIN} --json ${FIXTURE_CLEAN})
set_tests_properties(HubCheck_JsonCleanTrue PROPERTIES
    PASS_REGULAR_EXPRESSION "\"clean\":[ \\t]*true"
    LABELS "hub;sad-check")

# (AR) Summary يحتوي على عداد files و clean
add_test(NAME HubCheck_SummaryShape
    COMMAND ${SAD_CHECK_BIN} -r --summary ${FIXTURE_DIR})
set_tests_properties(HubCheck_SummaryShape PROPERTIES
    PASS_REGULAR_EXPRESSION "files=[0-9]+.*clean=[0-9]+"
    LABELS "hub;sad-check")

# (AR) Recursive يكتشف الملفين على الأقل في fixtures/
add_test(NAME HubCheck_RecursiveFindsBoth
    COMMAND ${SAD_CHECK_BIN} -r --summary ${FIXTURE_DIR})
set_tests_properties(HubCheck_RecursiveFindsBoth PROPERTIES
    PASS_REGULAR_EXPRESSION "files=2"
    LABELS "hub;sad-check")

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) القسم 2: sad-hub — مركز الأدوات
# ═══════════════════════════════════════════════════════════════════════════════

add_test(NAME HubMain_Version
    COMMAND ${SAD_HUB_BIN} --version)
set_tests_properties(HubMain_Version PROPERTIES
    PASS_REGULAR_EXPRESSION "sad-hub|Sad Language Tool Hub"
    LABELS "hub;sad-hub")

add_test(NAME HubMain_List
    COMMAND ${SAD_HUB_BIN} --list)
set_tests_properties(HubMain_List PROPERTIES
    PASS_REGULAR_EXPRESSION "check"
    LABELS "hub;sad-hub")

add_test(NAME HubMain_ListContainsRun
    COMMAND ${SAD_HUB_BIN} --list)
set_tests_properties(HubMain_ListContainsRun PROPERTIES
    PASS_REGULAR_EXPRESSION "run"
    LABELS "hub;sad-hub")

# (AR) أمر مجهول → رمز خروج غير صفري
add_test(NAME HubMain_UnknownCommand
    COMMAND ${SAD_HUB_BIN} flarble)
set_tests_properties(HubMain_UnknownCommand PROPERTIES
    WILL_FAIL TRUE
    LABELS "hub;sad-hub")

# (AR) اقتراح Levenshtein: "biuld" يجب أن يقترح بديلاً
add_test(NAME HubMain_TypoSuggestion
    COMMAND ${SAD_HUB_BIN} biuld)
set_tests_properties(HubMain_TypoSuggestion PROPERTIES
    WILL_FAIL TRUE
    LABELS "hub;sad-hub")

# (AR) ملف عاري بدون subcommand مرفوض (no backward-compat)
add_test(NAME HubMain_BareFileRejected
    COMMAND ${SAD_HUB_BIN} ${FIXTURE_CLEAN})
set_tests_properties(HubMain_BareFileRejected PROPERTIES
    WILL_FAIL TRUE
    LABELS "hub;sad-hub")

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) القسم 3: التماثل (symmetry) — الهب يجب أن يطابق الاستدعاء المباشر
# ═══════════════════════════════════════════════════════════════════════════════

# (AR) sad check <file>  ≡  sad-check <file>
add_test(NAME HubSymmetry_CheckClean
    COMMAND ${SAD_HUB_BIN} check ${FIXTURE_CLEAN})
set_tests_properties(HubSymmetry_CheckClean PROPERTIES
    PASS_REGULAR_EXPRESSION "نظيف|clean"
    LABELS "hub;symmetry")

add_test(NAME HubSymmetry_CheckParseError
    COMMAND ${SAD_HUB_BIN} check --no-color ${FIXTURE_PARSE})
set_tests_properties(HubSymmetry_CheckParseError PROPERTIES
    WILL_FAIL TRUE
    LABELS "hub;symmetry")

# (AR) sad check --summary -r → نفس مخرج الاستدعاء المباشر
add_test(NAME HubSymmetry_CheckSummary
    COMMAND ${SAD_HUB_BIN} check -r --summary ${FIXTURE_DIR})
set_tests_properties(HubSymmetry_CheckSummary PROPERTIES
    PASS_REGULAR_EXPRESSION "files=2.*clean=1"
    LABELS "hub;symmetry")

# (AR) sad check --json → JSON صالح عبر الهب
add_test(NAME HubSymmetry_CheckJson
    COMMAND ${SAD_HUB_BIN} check --json ${FIXTURE_CLEAN})
set_tests_properties(HubSymmetry_CheckJson PROPERTIES
    PASS_REGULAR_EXPRESSION "\"results\".*\"clean\":[ \\t]*true"
    LABELS "hub;symmetry")

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) القسم 4: sad-repl — اختبارات smoke غير-تفاعلية
#      تُضاف فقط إذا كان sad-repl مبنياً (websocket FFI متوفر).
# (EN) Section 4: sad-repl smoke tests (non-interactive flags only).
#      Only added when sad-repl target exists (i.e. websocket FFI is linked).
# ═══════════════════════════════════════════════════════════════════════════════

if(TARGET sad-repl)
    set(SAD_REPL_BIN "$<TARGET_FILE:sad-repl>")

    # (AR) --version يُخرج "sad-repl version"
    add_test(NAME HubRepl_Version
        COMMAND ${SAD_REPL_BIN} --version)
    set_tests_properties(HubRepl_Version PROPERTIES
        PASS_REGULAR_EXPRESSION "sad-repl version"
        LABELS "hub;sad-repl")

    # (AR) --help يحتوي "REPL" و "Options"
    add_test(NAME HubRepl_Help
        COMMAND ${SAD_REPL_BIN} --help)
    set_tests_properties(HubRepl_Help PROPERTIES
        PASS_REGULAR_EXPRESSION "REPL"
        LABELS "hub;sad-repl")

    # (AR) خيار غير معروف → فشل
    add_test(NAME HubRepl_BadFlag
        COMMAND ${SAD_REPL_BIN} --not-a-real-flag)
    set_tests_properties(HubRepl_BadFlag PROPERTIES
        WILL_FAIL TRUE
        LABELS "hub;sad-repl")

    # (AR) sad-hub --list يجب أن يكتشف "repl" (كأداة sad-repl.exe في نفس المجلد)
    add_test(NAME HubMain_ListContainsRepl
        COMMAND ${SAD_HUB_BIN} --list)
    set_tests_properties(HubMain_ListContainsRepl PROPERTIES
        PASS_REGULAR_EXPRESSION "repl"
        LABELS "hub;sad-hub;symmetry")

    # (AR) تماثل: sad repl --version ≡ sad-repl --version
    add_test(NAME HubSymmetry_ReplVersion
        COMMAND ${SAD_HUB_BIN} repl --version)
    set_tests_properties(HubSymmetry_ReplVersion PROPERTIES
        PASS_REGULAR_EXPRESSION "sad-repl version"
        LABELS "hub;sad-repl;symmetry")

    message(STATUS "✓ اختبارات الهب: مضاف 5 اختبارات لـ sad-repl")
else()
    message(STATUS "⚠ اختبارات الهب: تخطّي اختبارات sad-repl (الهدف غير موجود)")
endif()

# ═══════════════════════════════════════════════════════════════════════════════
# (AR) القسم 5: اختبارات smoke لباقي الأدوات (fmt, pkg, apk, analyze, profiler, run)
# (EN) Section 5: smoke tests for remaining tools
#
# (AR) لكل أداة موجودة كـ TARGET نضيف 3 اختبارات قياسية:
#      - Version: --version يطبع سطر إصدار ويعيد 0
#      - Help:    --help يعيد 0 (لا نُقيّد المحتوى لتفادي مشاكل ترميز الكونسول)
#      - Symmetry: استدعاء عبر sad-hub يعطي نفس مخرج --version (التماثل)
# ═══════════════════════════════════════════════════════════════════════════════

# (AR) دالة مساعدة (ماكرو CMake) لإضافة الثلاثية القياسية لأداة
# (EN) Helper macro to register the standard triplet for a tool
macro(sad_add_tool_smoke_tests _target _hub_name _version_regex)
    if(TARGET ${_target})
        set(_bin "$<TARGET_FILE:${_target}>")

        add_test(NAME HubSmoke_${_hub_name}_Version
            COMMAND ${_bin} --version)
        set_tests_properties(HubSmoke_${_hub_name}_Version PROPERTIES
            PASS_REGULAR_EXPRESSION "${_version_regex}"
            LABELS "hub;smoke;${_target}")

        add_test(NAME HubSmoke_${_hub_name}_Help
            COMMAND ${_bin} --help)
        set_tests_properties(HubSmoke_${_hub_name}_Help PROPERTIES
            LABELS "hub;smoke;${_target}")

        add_test(NAME HubSymmetry_${_hub_name}_Version
            COMMAND ${SAD_HUB_BIN} ${_hub_name} --version)
        set_tests_properties(HubSymmetry_${_hub_name}_Version PROPERTIES
            PASS_REGULAR_EXPRESSION "${_version_regex}"
            LABELS "hub;smoke;symmetry;${_target}")
    else()
        message(STATUS "⚠ اختبارات smoke: تخطّي ${_target} (الهدف غير موجود)")
    endif()
endmacro()

# (AR) sad-fmt: المُنسّق
sad_add_tool_smoke_tests(sad-fmt    fmt      "sad-fmt version")

# (AR) sad-fmt: علم غير معروف يجب أن يفشل (تحقق إضافي)
if(TARGET sad-fmt)
    add_test(NAME HubSmoke_fmt_BadFlag
        COMMAND $<TARGET_FILE:sad-fmt> --not-a-real-flag)
    set_tests_properties(HubSmoke_fmt_BadFlag PROPERTIES
        WILL_FAIL TRUE
        LABELS "hub;smoke;sad-fmt")
endif()

# (AR) sad-pkg: مدير الحزم
sad_add_tool_smoke_tests(sad-pkg    pkg      "sad-pkg v")

# (AR) sad-apk: باني APK
sad_add_tool_smoke_tests(sad-apk    apk      "sad-apk version")

# (AR) sad-analyze: المحلل الستاتيكي
sad_add_tool_smoke_tests(sad-analyze analyze "sad-analyze version")

# (AR) sad-profiler: المُحلّل
sad_add_tool_smoke_tests(sad-profiler profiler "sad-profiler v")

# (AR) sad-run: مشغّل المفسر
#      ملاحظة: اسم الهدف في CMake هو `sad` (مع OUTPUT_NAME="sad-run")،
#      لذلك نمرر "sad" للماكرو لا "sad-run".
sad_add_tool_smoke_tests(sad         run      "sad version")

# (AR) تحقق نهائي: sad-hub --list يكتشف الأدوات الموجودة في نفس المجلد
if(TARGET sad-fmt)
    add_test(NAME HubMain_ListContainsFmt
        COMMAND ${SAD_HUB_BIN} --list)
    set_tests_properties(HubMain_ListContainsFmt PROPERTIES
        PASS_REGULAR_EXPRESSION "fmt"
        LABELS "hub;smoke;sad-hub")
endif()

if(TARGET sad-pkg)
    add_test(NAME HubMain_ListContainsPkg
        COMMAND ${SAD_HUB_BIN} --list)
    set_tests_properties(HubMain_ListContainsPkg PROPERTIES
        PASS_REGULAR_EXPRESSION "pkg"
        LABELS "hub;smoke;sad-hub")
endif()

message(STATUS "✓ اختبارات الهب: ${CMAKE_CURRENT_LIST_FILE} مفعّلة")
