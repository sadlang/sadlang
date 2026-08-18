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

    # (AR) اختبار انحدار جلسة متعدّدة الجمل: يتحقّق أنّ REPL ينفّذ جلسةً كاملة بلا تلف
    #      قيَم ولا انهيار (إصلاح «مرسى الأشجار» ضدّ use-after-free): أرقام متمايزة
    #      (111/222)، تعريف دالّة واستدعاؤها (مربع(5)=25، كان ينهار)، صنف متعدّد الأسطر
    #      (نقطة(42).س=42)، وكتلة else-if (الفرع الصحيح=800 لا 700).
    # (EN) Multi-statement session regression: asserts the REPL runs a whole session with
    #      no value corruption and no crash (the AST-arena fix against use-after-free):
    #      distinct numbers (111/222), a function defined then called (مربع(5)=25, used to
    #      crash), a multiline class (نقطة(42).س=42), and an else-if block (correct branch
    #      = 800, not 700). The driver pipes the .ص session to stdin (non-interactive).
    add_test(NAME HubRepl_Session
        COMMAND ${CMAKE_COMMAND}
            -DREPL_BIN=$<TARGET_FILE:sad-repl>
            -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_session_input.txt
            -DEXPECT=111|222|25|42|800|18
            -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
    set_tests_properties(HubRepl_Session PROPERTIES
        LABELS "hub;sad-repl;regression")

    # ══════════════════════════════════════════════════════════════════════════
    # (AR) ثلاثيّةُ «الحقلُ الصنفيُّ عبر ‹:reset›» — ضابطان وتقاطعٌ، لا حالةٌ مفردة.
    #      العطبُ المقيس (2026-08-15): بعد أن صار الحقلُ الذي نوعُه صنفٌ يُنشَأ
    #      ضمنًا، حملت ClassField مؤشّرًا **مستعارًا** إلى عقدةِ المُهيّئِ في الشجرة؛
    #      و‹:reset› يُحرّر مرسى الأشجار بينما يبقى ClassType حيًّا في مفردةِ
    #      ClassManager ⇒ أوّلُ إنشاءٍ بعد الإعادة يُسقط العمليّةَ بـSIGSEGV.
    #      العلاجُ: البنيةُ تحمل **اسمًا** يملك نفسَه لا إشارةً إلى شجرةٍ تُحرَّر.
    #
    #      ولماذا ثلاثةُ اختباراتٍ لا واحد: نجاحُ التقاطعِ وحدَه لا يُميّز الإصلاحَ
    #      من غيابِ الشرطِ أصلًا. الضابطان يُثبتان أنّ ‹:reset› يعمل بلا حقلٍ صنفيّ،
    #      وأنّ الحقلَ الصنفيَّ يعمل بلا ‹:reset› — فيبقى التقاطعُ هو المقيسَ فعلًا.
    #
    #      ⚠️ حدٌّ مُعلَن: المُشغّلُ يحكم بظهورِ السلاسلِ وبعلامةِ «Access violation»،
    #      ولا يفحص رمزَ الخروج. والانهيارُ المقيسُ كان صامتًا (rc=139 بلا علامة)،
    #      فالذي يلتقطه هنا هو **غيابُ العلامةِ النصّيّة** بعد الانهيار لا الرمز.
    # (EN) A three-way «class-typed field across ‹:reset›» set: two controls plus the
    #      intersection. Passing the intersection alone would not distinguish the fix
    #      from the precondition simply being absent. Note the driver asserts on output
    #      substrings, not on the exit code — the measured crash was a silent SIGSEGV,
    #      caught here by the marker line failing to appear.
    # ══════════════════════════════════════════════════════════════════════════
    add_test(NAME HubRepl_PlainClassReset
        COMMAND ${CMAKE_COMMAND}
            -DREPL_BIN=$<TARGET_FILE:sad-repl>
            -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_plain_class_reset_input.txt
            -DEXPECT=PLAIN_RESET_OK 5
            -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
    set_tests_properties(HubRepl_PlainClassReset PROPERTIES
        LABELS "hub;sad-repl;regression")

    add_test(NAME HubRepl_ClassFieldNoReset
        COMMAND ${CMAKE_COMMAND}
            -DREPL_BIN=$<TARGET_FILE:sad-repl>
            -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_class_field_noreset_input.txt
            -DEXPECT=FIELD_NORESET_OK 9
            -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
    set_tests_properties(HubRepl_ClassFieldNoReset PROPERTIES
        LABELS "hub;sad-repl;regression")

    add_test(NAME HubRepl_ClassFieldAcrossReset
        COMMAND ${CMAKE_COMMAND}
            -DREPL_BIN=$<TARGET_FILE:sad-repl>
            -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_class_field_reset_input.txt
            -DEXPECT=RESET_FIELD_OK 7
            -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
    set_tests_properties(HubRepl_ClassFieldAcrossReset PROPERTIES
        LABELS "hub;sad-repl;regression")

    # (AR) إرغونوميا الصدَفة: تشغيل برنامج خارجيّ عبر ‹:شغّل›. فشل الإطلاق (أمر مجهول)
    #      محمول عبر المنصّات (يُبلَّغ باسم البرنامج في الخطأ) — يُتوقّع ظهور اسم الأمر.
    # (EN) Shell ergonomics: run an external program via ‹:run›. Launch failure
    #      (unknown command) is portable; the program name appears in the error.
    add_test(NAME HubRepl_RunFail
        COMMAND ${CMAKE_COMMAND}
            -DREPL_BIN=$<TARGET_FILE:sad-repl>
            -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_run_fail_input.txt
            -DEXPECT=__sados_no_such_cmd__
            -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
    set_tests_properties(HubRepl_RunFail PROPERTIES
        LABELS "hub;sad-repl;shell")

    # (AR) نجاح التشغيل: ‹:شغّل echo …› يجب أن يُظهر مخرَج البرنامج. echo كثنائيّ خارجيّ
    #      متوفّر على POSIX فقط (على ويندوز أمر صدَفة لا exe) ⇒ يُسجَّل خارج ويندوز.
    # (EN) Run success: ‹:run echo …› must surface program output. echo-as-exe exists
    #      on POSIX only (a shell builtin on Windows) => registered off-Windows.
    if(NOT WIN32)
        add_test(NAME HubRepl_RunEcho
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_run_echo_input.txt
                -DEXPECT=SADOS_RUN_OK
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunEcho PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) أنبوب: stdout المرحلة الأولى (echo) يُغذّي stdin الثانية (tr) — التحويل إلى
        #      أحرف كبيرة يُثبت أنّ tr عالج مخرَج echo عبر الأنبوب لا سطرًا خامًّا.
        # (EN) pipe: stage-1 stdout (echo) feeds stage-2 stdin (tr); the uppercasing proves
        #      tr processed echo's piped output. POSIX-only (tr as an external exe).
        add_test(NAME HubRepl_RunPipe
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_run_pipe_input.txt
                -DEXPECT=SADOS_PIPE
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunPipe PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) وسائط مقتبَسة: ‹printf [%s] "a b" c› — الاقتباس يُبقي «a b» وسيطًا واحدًا،
        #      فيطبع printf ‹[a b][c]›. لو قُسِّم بالمسافات لطُبع ‹[a][b][c]› ⇒ ‹[a b]› يُثبت الاقتباس.
        # (EN) quoted args: quoting keeps «a b» a single arg so printf emits ‹[a b][c]›;
        #      a whitespace split would emit ‹[a][b][c]›, so ‹[a b]› proves quote handling.
        add_test(NAME HubRepl_RunQuoted
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_run_quoted_input.txt
                -DEXPECT=[a b]
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunQuoted PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) إعادة توجيه الإخراج ‹>›: يكتب echo إلى ملفّ ثمّ يقرؤه cat في نفس الجلسة —
        #      ظهور النصّ في مخرَج cat يُثبت أنّ ‹>› كتب الملفّ فعلًا (ذهاب-إياب).
        # (EN) output redirection ‹>›: echo writes a file, cat reads it back in the same
        #      session; the text in cat's output proves ‹>› actually wrote the file.
        add_test(NAME HubRepl_RunRedirOut
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_redir_out_input.txt
                -DEXPECT=SADOS_ROUNDTRIP
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunRedirOut PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) الإلحاق ‹>>›: ‹>› ثمّ ‹>>› ثمّ cat — بقاء LINE1 وإضافة LINE2 يُثبتان الإلحاق
        #      (لا البتر). / (EN) append ‹>>›: keeping LINE1 and adding LINE2 proves append.
        add_test(NAME HubRepl_RunRedirAppend
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_redir_app_input.txt
                -DEXPECT=LINE1|LINE2
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunRedirAppend PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) إعادة توجيه الإدخال ‹<›: cat يقرأ من ملفٍّ عبر ‹<› فيظهر محتواه. / (EN) input
        #      redirection ‹<›: cat reads a file via ‹<›, surfacing its content.
        add_test(NAME HubRepl_RunRedirIn
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_redir_in_input.txt
                -DEXPECT=SADOS_IN
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunRedirIn PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) توجيه + أنبوب: ‹echo | tr > ملف› — التوجيه على المرحلة الأخيرة يفوز على وراثة
        #      stdout، فيُكتب الناتج المحوّل للملفّ. / (EN) pipe + redirect: ‹>› on the last stage
        #      wins over inherited stdout, writing the transformed output to the file.
        add_test(NAME HubRepl_RunRedirPipe
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_redir_pipe_input.txt
                -DEXPECT=SADPIPE
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunRedirPipe PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) توسيع متغيّرات: ‹:بيئة V=…› ثمّ ‹$V›/‹${V}› — ظهور القيمتين يُثبت كِلا الصيغتين.
        # (EN) variable expansion: ‹:env V=…› then ‹$V›/‹${V}› — both values prove both forms.
        add_test(NAME HubRepl_EnvExpand
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_env_expand_input.txt
                -DEXPECT=PLAIN_OK|BRACED_OK
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_EnvExpand PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) الوراثة: ما يضبطه ‹:بيئة› يرثه ما يُطلَق — printenv (برنامج خارجيّ) يقرأه من بيئته.
        # (EN) inheritance: what ‹:env› sets is inherited by launches — printenv reads it.
        add_test(NAME HubRepl_EnvInherit
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_env_inherit_input.txt
                -DEXPECT=INHERIT_OK
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_EnvInherit PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) الاقتباس المفرد يمنع التوسيع: ‹'$SQ'› يبقى حرفيًّا (‹$SQ› لا قيمته). / (EN) single
        #      quotes suppress expansion: ‹'$SQ'› stays literal (‹$SQ›, not its value).
        add_test(NAME HubRepl_EnvSingleQuote
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_env_singlequote_input.txt
                -DEXPECT=$SQ
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_EnvSingleQuote PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) اسم متغيّر عربيّ: ‹:بيئة سين=…› ثمّ ‹${سين}› يُوسَّع (سيادة عربيّة). / (EN) Arabic
        #      var name: ‹:env سين=…› then ‹${سين}› expands (Arabic-first sovereignty).
        add_test(NAME HubRepl_EnvArabic
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_env_arabic_input.txt
                -DEXPECT=عربي_ناجح
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_EnvArabic PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) متغيّر غير مُعرَّف يُوسَّع لفراغ: ‹echo [${غير_معرّف}]› ⇒ ‹[]›. / (EN) an unset var
        #      expands to empty: ‹echo [${undef}]› ⇒ ‹[]›.
        add_test(NAME HubRepl_EnvUnset
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_env_unset_input.txt
                -DEXPECT=[]
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_EnvUnset PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) توجيه الخطأ ‹2›: برنامجٌ يكتب لـstderr يُوجَّه لملفٍّ ثمّ يقرؤه cat — ظهور النصّ
        #      يُثبت أنّ ‹2›، لا ‹>›، التقط مجرى الخطأ (stdout لم يُلمَس). / (EN) stderr ‹2›: a
        #      program writing to stderr is redirected to a file, then cat reads it — the text
        #      proves ‹2› (not ‹>›) captured the error stream (stdout untouched).
        add_test(NAME HubRepl_RunRedirErr
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_redir_err_input.txt
                -DEXPECT=SADOS_ERR
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunRedirErr PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) الدمج ‹&>›: الإخراج والخطأ معًا إلى ملفٍّ واحد — ظهور السطرين (OUT من stdout و
        #      ERR من stderr) يُثبت أنّ الخطأ تبع الإخراج لوجهته. / (EN) combined ‹&>›: both
        #      stdout and stderr to one file — both lines proving stderr followed stdout.
        add_test(NAME HubRepl_RunRedirBoth
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_redir_both_input.txt
                -DEXPECT=OUT_LINE|ERR_LINE
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunRedirBoth PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) ‹2>&1› مع أنبوب: خطأُ المرحلة يُدمَج في إخراجها فيمرّ عبر الأنبوب لـcat — ظهور
        #      النصّ يُثبت أنّ الخطأ صار يتبع stdout (الأنبوب). / (EN) ‹2>&1› with a pipe: the
        #      stage's stderr merges into its stdout and flows through the pipe to cat.
        add_test(NAME HubRepl_RunRedirErrToOut
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_redir_err2out_input.txt
                -DEXPECT=TOSTDOUT
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunRedirErrToOut PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) الإلحاق ‹2>>›: ‹2›› ثمّ ‹2>>› ثمّ cat — بقاء E1 وإضافة E2 يُثبتان إلحاق الخطأ
        #      (لا البتر). / (EN) stderr append ‹2>>›: keeping E1 and adding E2 proves append.
        add_test(NAME HubRepl_RunRedirErrAppend
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_redir_errapp_input.txt
                -DEXPECT=E1|E2
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunRedirErrAppend PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) الصيغة الملاصقة ‹2>ملف› (بلا فراغ): يجب أن يُتعرَّف المِقبض عند بداية الوسيط
        #      فيُوجَّه الخطأ للملفّ — يحصّن تمييز «مِقبض عند البداية» عن «رقمٌ نصّيّ». / (EN)
        #      adjacent ‹2>file› (no space): the fd prefix must be recognized at a token start,
        #      redirecting stderr — hardens fd-at-start vs. a literal digit.
        add_test(NAME HubRepl_RunRedirErrAdjacent
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_redir_erradj_input.txt
                -DEXPECT=ADJ_ERR
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_RunRedirErrAdjacent PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) ‹$?›: رمز خروج آخر ‹:شغّل›. ‹exit 5› ⇒ ‹A=5›، ثمّ ‹exit 0› ⇒ ‹B=0› — يُثبتان
        #      التقاط الرمز غير الصفريّ وإعادةَ ضبطه بعد النجاح. / (EN) ‹$?›: last ‹:run› exit
        #      code. ‹exit 5› ⇒ ‹A=5› then ‹exit 0› ⇒ ‹B=0› prove non-zero capture and reset.
        add_test(NAME HubRepl_SpecialExitCode
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_special_exit_input.txt
                -DEXPECT=A=5|B=0
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_SpecialExitCode PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) ‹$?›=127 عند تعذّر الإطلاق (اصطلاح «الأمر غير موجود»). / (EN) ‹$?›=127 on a launch
        #      failure (the "command not found" idiom).
        add_test(NAME HubRepl_SpecialNotFound
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_special_notfound_input.txt
                -DEXPECT=RC=127
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_SpecialNotFound PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) ‹$$›: مِقبض الصدَفة عددٌ موجب — ‹test $$ -gt 0› يطبع العلامة فقط إن توسّع لعددٍ
        #      موجب (لو فرغ لفشل التعبير). / (EN) ‹$$›: the shell pid is a positive integer —
        #      ‹test $$ -gt 0› prints the marker only if it expanded to a positive number.
        add_test(NAME HubRepl_SpecialPid
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_special_pid_input.txt
                -DEXPECT=PID_POSITIVE
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_SpecialPid PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) الاقتباس المفرد يمنع توسيع الوسائط الخاصّة: ‹'$?'› يبقى حرفيًّا. / (EN) single
        #      quotes suppress special-param expansion: ‹'$?'› stays literal.
        add_test(NAME HubRepl_SpecialSingleQuote
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_special_squote_input.txt
                -DEXPECT=$?
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_SpecialSingleQuote PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) التلاصق: ‹$?› يتوسّع لرمزٍ ثمّ يبقى ما بعده حرفيًّا — ‹echo P=$?Q› بعد ‹exit 4›
        #      ⇒ ‹P=4Q› (لا التهام جشع للحرف التالي). / (EN) adjacency: ‹$?› expands to just the
        #      code, the following char stays literal — ‹P=$?Q› after ‹exit 4› ⇒ ‹P=4Q›.
        add_test(NAME HubRepl_SpecialAdjacent
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_special_adjacent_input.txt
                -DEXPECT=P=4Q
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_SpecialAdjacent PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) ‹&&› يعمل بعد نجاح: ‹exit 0 && echo AND_RAN› ⇒ ظهور العلامة. / (EN) ‹&&› runs after
        #      success: ‹exit 0 && echo AND_RAN› ⇒ the marker appears.
        add_test(NAME HubRepl_ChainAndOk
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_chain_and_ok_input.txt
                -DEXPECT=AND_RAN
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ChainAndOk PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) ‹&&› يتخطّى بعد فشل، و‹||› يعمل: ‹exit 1 && echo AND_BAD || echo AND_SKIPPED› ⇒
        #      ‹AND_SKIPPED› فقط (لو عمل ‹&&› خطأً لطُبع AND_BAD وتخطّى ‹||›). / (EN) ‹&&› skips
        #      after failure and ‹||› runs ⇒ ‹AND_SKIPPED› (a wrong ‹&&› run would print AND_BAD
        #      and skip ‹||›, so this uniquely proves the skip).
        add_test(NAME HubRepl_ChainAndSkip
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_chain_and_skip_input.txt
                -DEXPECT=AND_SKIPPED
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ChainAndSkip PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) ‹||› يعمل بعد فشل: ‹false || echo OR_RAN› ⇒ ظهور العلامة. / (EN) ‹||› runs after
        #      failure: ‹false || echo OR_RAN› ⇒ the marker appears.
        add_test(NAME HubRepl_ChainOrOk
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_chain_or_ok_input.txt
                -DEXPECT=OR_RAN
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ChainOrOk PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) ‹||› يتخطّى بعد نجاح: ‹exit 0 || exit 9› ثمّ ‹$?› ⇒ ‹RC=0› (لو عمل ‹||› خطأً لصار
        #      الرمز 9). / (EN) ‹||› skips after success: ‹exit 0 || exit 9› then ‹$?› ⇒ ‹RC=0› (a
        #      wrong ‹||› run would make the code 9), uniquely proving the skip via the exit code.
        add_test(NAME HubRepl_ChainOrSkip
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_chain_or_skip_input.txt
                -DEXPECT=RC=0
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ChainOrSkip PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) تكامل: أنبوبٌ كمقطعٍ ثمّ ‹&&›: ‹echo hello | tr a-z A-Z && echo PIPE_CHAIN_OK› ⇒
        #      ‹HELLO› (الأنبوب) و‹PIPE_CHAIN_OK› (السلسلة بعد نجاحه). / (EN) integration: a pipe
        #      as a segment then ‹&&› ⇒ ‹HELLO› (pipe) and ‹PIPE_CHAIN_OK› (chain after success).
        add_test(NAME HubRepl_ChainWithPipe
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_chain_pipe_input.txt
                -DEXPECT=HELLO|PIPE_CHAIN_OK
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ChainWithPipe PROPERTIES
            LABELS "hub;sad-repl;shell")

        # (AR) مقاطعة (Ctrl-C) تُجهض السلسلة كلّها: مقطعٌ يقتل نفسه بـSIGINT ثمّ ‹|| exit 3›؛
        #      الصحيح يُجهض قبل الـ‹||› فيبقى الرمز 130 (128+SIGINT) ⇒ ‹RC=130›. لو عُومِلت
        #      المقاطعة فشلًا عاديًّا لعمل ‹|| exit 3› وصار الرمز 3 — فـ‹RC=130› يُثبت الإجهاض.
        # (EN) an interrupt (Ctrl-C) aborts the whole chain: a segment kills itself with SIGINT
        #      then ‹|| exit 3›; correct behavior aborts before the ‹||› so the code stays 130
        #      (128+SIGINT) ⇒ ‹RC=130›. Treating the interrupt as an ordinary failure would run
        #      ‹|| exit 3› making the code 3 — so ‹RC=130› proves the abort.
        add_test(NAME HubRepl_ChainInterruptAbort
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_chain_interrupt_input.txt
                -DEXPECT=RC=130
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ChainInterruptAbort PROPERTIES
            LABELS "hub;sad-repl;shell")

        # ── الموزِّع العربيّ (اسم أمرٍ عربيّ صريح → exec) / Arabic dispatcher ──
        # (AR) أمرٌ عربيّ مباشر: ‹:اطبع X› ⇒ echo X (توزيع على مستوى الأمر). / (EN) a direct
        #      Arabic command: ‹:اطبع X› ⇒ echo X (command-level dispatch).
        add_test(NAME HubRepl_ArabicEcho
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_ar_echo_input.txt
                -DEXPECT=ARABIC_ECHO_OK
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ArabicEcho PROPERTIES LABELS "hub;sad-repl;shell")

        # (AR) أنبوب عربيّ: ‹اطبع hello | بدّل_محارف a-z A-Z› ⇒ HELLO (ترجمة argv[0] لكلّ مرحلة).
        # (EN) Arabic pipe: ‹اطبع | بدّل_محارف› ⇒ HELLO (per-stage argv[0] translation).
        add_test(NAME HubRepl_ArabicPipe
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_ar_pipe_input.txt
                -DEXPECT=HELLO
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ArabicPipe PROPERTIES LABELS "hub;sad-repl;shell")

        # (AR) سلسلة عربيّة: ‹:صحيح && اطبع CHAIN_AR_OK› ⇒ يعمل بعد نجاح (ترجمة في مقطعٍ لاحق).
        # (EN) Arabic chain: ‹:صحيح && اطبع …› ⇒ runs after success (translation in a later segment).
        add_test(NAME HubRepl_ArabicChain
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_ar_chain_input.txt
                -DEXPECT=CHAIN_AR_OK
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ArabicChain PROPERTIES LABELS "hub;sad-repl;shell")

        # (AR) أمر عربيّ + توجيه + أمر عربيّ ثانٍ: ‹اطبع … > ملف› ثمّ ‹اعرض ملف› ⇒ ذهاب-إياب.
        # (EN) Arabic command + redirect + second Arabic command: ‹اطبع … > f› then ‹اعرض f›.
        add_test(NAME HubRepl_ArabicRedirect
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_ar_redirect_input.txt
                -DEXPECT=REDIR_AR_OK
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ArabicRedirect PROPERTIES LABELS "hub;sad-repl;shell")

        # (AR) توافق خلفيّ: ‹:شغّل echo X› الإنجليزيّ ما زال يعمل (لا اسم عربيّ ⇒ يمرّ كما هو).
        # (EN) backward compat: English ‹:شغّل echo X› still works (no Arabic key ⇒ passes as-is).
        add_test(NAME HubRepl_ArabicCompat
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_ar_compat_input.txt
                -DEXPECT=BACKWARD_OK
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ArabicCompat PROPERTIES LABELS "hub;sad-repl;shell")

        # (AR) تسلسل أوامر عربيّة: ‹أنشئ_مجلّد› ثمّ ‹المس› ثمّ ‹اسرد› ⇒ ظهور الملفّ (mkdir/touch/ls).
        # (EN) a sequence of Arabic commands: ‹أنشئ_مجلّد›/‹المس›/‹اسرد› ⇒ the file appears.
        add_test(NAME HubRepl_ArabicLs
            COMMAND ${CMAKE_COMMAND}
                -DREPL_BIN=$<TARGET_FILE:sad-repl>
                -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_ar_ls_input.txt
                -DEXPECT=ONLYFILE_AR
                -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
        set_tests_properties(HubRepl_ArabicLs PROPERTIES LABELS "hub;sad-repl;shell")
    endif()

    # (AR) رابطٌ معلّق بلا طرف (‹echo x &&›) خطأ تحليلٍ محمول = مرحلةٌ فارغة. / (EN) a dangling
    #      connector (‹echo x &&›) is a portable parse error = an empty stage.
    add_test(NAME HubRepl_ChainDangling
        COMMAND ${CMAKE_COMMAND}
            -DREPL_BIN=$<TARGET_FILE:sad-repl>
            -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_chain_dangling_input.txt
            -DEXPECT=Empty pipeline stage
            -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
    set_tests_properties(HubRepl_ChainDangling PROPERTIES
        LABELS "hub;sad-repl;shell")

    # (AR) أخطاء التحليل محمولة (تحليلٌ محض، بلا إطلاق برنامج): اقتباسٌ غير مُغلَق
    #      ومرحلةُ أنبوبٍ فارغة يجب أن يُبلَّغ عنهما عبر كتالوج SoT (الجزء الإنجليزيّ ASCII).
    # (EN) parse errors are portable (pure parsing, no launch): an unterminated quote and
    #      an empty pipeline stage must be reported via the SoT catalog (ASCII English part).
    add_test(NAME HubRepl_RunBadQuote
        COMMAND ${CMAKE_COMMAND}
            -DREPL_BIN=$<TARGET_FILE:sad-repl>
            -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_run_badquote_input.txt
            -DEXPECT=Unterminated quote
            -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
    set_tests_properties(HubRepl_RunBadQuote PROPERTIES
        LABELS "hub;sad-repl;shell")

    add_test(NAME HubRepl_RunEmptyStage
        COMMAND ${CMAKE_COMMAND}
            -DREPL_BIN=$<TARGET_FILE:sad-repl>
            -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_run_emptystage_input.txt
            -DEXPECT=Empty pipeline stage
            -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
    set_tests_properties(HubRepl_RunEmptyStage PROPERTIES
        LABELS "hub;sad-repl;shell")

    # (AR) إعادة توجيه بلا هدف (محمول، خطأ تحليل بلا إطلاق). / (EN) redirection with no target
    #      (portable, a parse error with no launch).
    add_test(NAME HubRepl_RunRedirNoTarget
        COMMAND ${CMAKE_COMMAND}
            -DREPL_BIN=$<TARGET_FILE:sad-repl>
            -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_redir_notarget_input.txt
            -DEXPECT=Redirection with no target file
            -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
    set_tests_properties(HubRepl_RunRedirNoTarget PROPERTIES
        LABELS "hub;sad-repl;shell")

    # (AR) اسم متغيّر بيئة غير صالح (محمول، خطأ من ‹:بيئة› بلا إطلاق). / (EN) invalid env-var name
    #      (portable, an ‹:env› error with no launch).
    add_test(NAME HubRepl_EnvBadName
        COMMAND ${CMAKE_COMMAND}
            -DREPL_BIN=$<TARGET_FILE:sad-repl>
            -DINPUT_FILE=${CMAKE_CURRENT_SOURCE_DIR}/repl_env_badname_input.txt
            -DEXPECT=Invalid environment variable name
            -P ${CMAKE_CURRENT_SOURCE_DIR}/repl_session_test.cmake)
    set_tests_properties(HubRepl_EnvBadName PROPERTIES
        LABELS "hub;sad-repl;shell")

    # (AR) اختبارات sad-repl: تشغيل + أنابيب + إعادة توجيه + متغيّرات بيئة. بعضها خاصّ بـPOSIX
    #      (echo/tr/printf/printenv كثنائيّات + الأنابيب/التوجيه)، محروسٌ بـif(NOT WIN32) أعلاه.
    # (EN) sad-repl tests: run + pipes + redirection + env vars. Some are POSIX-only
    #      (echo/tr/printf/printenv as exes + pipes/redirection), guarded by if(NOT WIN32) above.
    message(STATUS "✓ اختبارات الهب: أُضيفت اختبارات sad-repl (تشغيل/أنابيب/توجيه/بيئة)")
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
