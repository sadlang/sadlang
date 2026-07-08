# بسم الله الرحمن الرحيم
# ==============================================================================
# (AR) مُشغّل اختبار جلسة REPL متعدّدة الجمل (غير تفاعليّة عبر stdin).
#      يُغذّي ملفَّ جلسةٍ إلى sad-repl ويتحقّق من ظهور كلّ سلسلة متوقّعة في المخرَج،
#      ومن غياب مؤشّر الانهيار. اختبارُ انحدارٍ لإصلاح «مرسى الأشجار» (use-after-free):
#      جمل متتالية بلا تلف قيَم، تعريف دالّة/صنف بلا انهيار، وكتلة else-if.
# (EN) Multi-statement REPL session test driver (non-interactive via stdin).
#      Feeds a session file to sad-repl and asserts every expected substring appears
#      in the output and no crash marker is present. Regression test for the AST-arena
#      fix (use-after-free): sequential statements with no value corruption, function/
#      class definitions with no crash, and an else-if block.
#
# (AR) المعاملات / (EN) Parameters:
#   -DREPL_BIN=<path to sad-repl executable>
#   -DINPUT_FILE=<path to the session .ص file (piped to stdin)>
#   -DEXPECT=<expected substrings separated by '|' (not ';', which is a CMake list sep)>
# ==============================================================================

if(NOT REPL_BIN OR NOT INPUT_FILE OR NOT DEFINED EXPECT)
    message(FATAL_ERROR "repl_session_test: REPL_BIN و INPUT_FILE و EXPECT مطلوبة / required")
endif()

# (AR) شغّل REPL وامنحه الجلسة عبر stdin / (EN) run the REPL, feeding the session via stdin
execute_process(
    COMMAND "${REPL_BIN}"
    INPUT_FILE "${INPUT_FILE}"
    OUTPUT_VARIABLE _out
    ERROR_VARIABLE _err
    RESULT_VARIABLE _rc
)

set(_all "${_out}${_err}")

# (AR) لا يجوز ظهور مؤشّر انهيار (كان تعريف الدالّة/الصنف ينهار قبل الإصلاح).
# (EN) no crash marker may appear (function/class definition used to crash pre-fix).
string(FIND "${_all}" "Access violation" _crash)
if(NOT _crash EQUAL -1)
    message(FATAL_ERROR "repl_session_test: رُصد انهيار / crash detected (Access violation)\n--- output ---\n${_all}")
endif()

# (AR) كلّ سلسلة متوقّعة يجب أن تظهر / (EN) every expected substring must appear
string(REPLACE "|" ";" _expects "${EXPECT}")
foreach(_needle IN LISTS _expects)
    string(FIND "${_all}" "${_needle}" _pos)
    if(_pos EQUAL -1)
        message(FATAL_ERROR "repl_session_test: السلسلة المتوقّعة '${_needle}' غير موجودة / expected substring not found\n--- output ---\n${_all}")
    endif()
endforeach()

message(STATUS "repl_session_test: PASS")
