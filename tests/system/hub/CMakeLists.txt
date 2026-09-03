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

if(NOT TARGET sad-check OR NOT TARGET sad)
    message(STATUS "⚠ اختبارات الهب: تخطّي (sad أو sad-check غير مبنيين)")
    return()
endif()

# (AR) نقاط الاحتجاج إلى البرامج التنفيذية / (EN) Binary references
set(SAD_HUB_BIN  "$<TARGET_FILE:sad>")
set(SAD_CHECK_BIN "$<TARGET_FILE:sad-check>")

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
    PASS_REGULAR_EXPRESSION "^sad |Sad Language Tool Hub"
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
# (AR) 🔑 حُذف القسم 4 (sad-repl): ٤٩ اختبارَ دخانٍ وأنابيبَ وتوجيهٍ وآبلتات.
#      الأداةُ `tools/repl` ومصدرُ حقيقتِها زالا مع المفسّر، وكانت الكتلةُ
#      محروسةً بـ`if(TARGET sad-repl)` — فبزوالِ الهدفِ سقطت **صامتةً**: لا
#      اختبارٌ يُسجَّل ولا رسالةٌ تُطبَع ولا إخفاق. وسقوطٌ لا صوتَ له يُقرأ
#      «مُغطًّى» وهو غيابٌ تامّ، ولذلك يُقيَّدُ هنا بدل أن يُطوى.
#      ⚠️ قدرةٌ مفقودةٌ مُعلَنة: لا صدفةَ تفاعليّةٌ في مسارِ المحرّكِ الواحد،
#      فلا اختباراتِ أنابيبَ ولا توجيهَ آبلتات. إحياؤها يلزمُه أداةٌ جديدةٌ
#      مبنيّةٌ على المترجّم، لا إعادةَ تشغيلِ ما زال.
# (EN) Removed section 4 (sad-repl): 49 smoke/pipe/routing/applet tests. The tool
#      and its SoT went with the interpreter, and the block sat behind
#      if(TARGET sad-repl) — so it went silently dead: nothing registered, nothing
#      printed, nothing failed. A silent drop reads as "covered" while being a
#      total absence, so it is recorded here instead of folded away.
#      Declared capability loss: the single-engine track has no interactive shell,
#      hence no pipe or applet-routing tests. Reviving them needs a new
#      compiler-based tool, not a re-run of what was deleted.

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
# (AR) 🔑 إملاءُ عَلَمِ الإصدار/المساعدة **مُعامَلٌ** لا مفترَض. المفسّرُ والمترجمُ
#      مُدرَجانِ في `engines` بـlanguage-truth/cli_flags.yaml فاسمُهما القانونيُّ
#      عربيٌّ وحيد (`--إصدار`/`--مساعدة`) وقد أُلغيت المرادفاتُ الإنجليزيّةُ عمدًا،
#      بينما الأدواتُ المساعدةُ خارجَ الجدولِ فتقبلُ الإنجليزيّة. ماكرو يكتبُ
#      `--version` حرفيًّا كان يقيسُ افتراضَه لا عقدَ الأداة.
# (EN) The version/help spelling is a PARAMETER, not an assumption. Interpreter
#      and compiler are listed under `engines` in cli_flags.yaml, so their sole
#      canonical long flags are Arabic; the English synonyms were deliberately
#      removed. Auxiliary tools are outside that table and keep English. A macro
#      hard-coding `--version` measured its own assumption, not the tool.
macro(sad_add_tool_smoke_tests _target _hub_name _version_regex _version_flag _help_flag)
    if(TARGET ${_target})
        set(_bin "$<TARGET_FILE:${_target}>")

        add_test(NAME HubSmoke_${_hub_name}_Version
            COMMAND ${_bin} ${_version_flag})
        set_tests_properties(HubSmoke_${_hub_name}_Version PROPERTIES
            PASS_REGULAR_EXPRESSION "${_version_regex}"
            LABELS "hub;smoke;${_target}")

        add_test(NAME HubSmoke_${_hub_name}_Help
            COMMAND ${_bin} ${_help_flag})
        set_tests_properties(HubSmoke_${_hub_name}_Help PROPERTIES
            LABELS "hub;smoke;${_target}")

        add_test(NAME HubSymmetry_${_hub_name}_Version
            COMMAND ${SAD_HUB_BIN} ${_hub_name} ${_version_flag})
        set_tests_properties(HubSymmetry_${_hub_name}_Version PROPERTIES
            PASS_REGULAR_EXPRESSION "${_version_regex}"
            LABELS "hub;smoke;symmetry;${_target}")
    else()
        message(STATUS "⚠ اختبارات smoke: تخطّي ${_target} (الهدف غير موجود)")
    endif()
endmacro()

# (AR) sad-fmt: المُنسّق
sad_add_tool_smoke_tests(sad-fmt    fmt      "sad-fmt version"    --version --help)

# (AR) sad-fmt: علم غير معروف يجب أن يفشل (تحقق إضافي)
if(TARGET sad-fmt)
    add_test(NAME HubSmoke_fmt_BadFlag
        COMMAND $<TARGET_FILE:sad-fmt> --not-a-real-flag)
    set_tests_properties(HubSmoke_fmt_BadFlag PROPERTIES
        WILL_FAIL TRUE
        LABELS "hub;smoke;sad-fmt")
endif()

# (AR) sad-pkg: مدير الحزم
sad_add_tool_smoke_tests(sad-pkg    pkg      "sad-pkg v"          --version --help)

# (AR) sad-apk: باني APK
sad_add_tool_smoke_tests(sad-apk    apk      "sad-apk version"    --version --help)

# (AR) sad-analyze: المحلل الستاتيكي
sad_add_tool_smoke_tests(sad-analyze analyze "sad-analyze version" --version --help)

# (AR) 🔑 حُذف دخانُ sad-profiler: الأداةُ (`tools/profiler`) زالت مع المفسّر،
#      وكان `if(TARGET ...)` داخلَ الماكرو يُسقِطُ الخانتَين صامتتَين.
# (EN) Removed: the profiler tool went with the interpreter, and the macro's
#      internal TARGET guard dropped both entries silently.

# (AR) 🔑 حُذف دخانُ sad-run: الهدفُ زال مع المفسّر. وتغطيةُ «هل يُشغَّلُ برنامجُ
#      ص؟» انتقلت إلى المترجّم: `sad-build` مقيسٌ في شوطِ السلوكِ كلِّه.
# (EN) Removed: the target went with the interpreter. "Can a ص program run?" is
#      now covered by the compiler across the whole behavior suite.

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
