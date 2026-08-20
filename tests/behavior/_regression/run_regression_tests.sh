#!/usr/bin/env bash
# ======================================================================
# اختبارات الانحدار للغة ص / Regression Tests Runner
# Sad Programming Language — Linux/macOS
# ======================================================================
# الاستخدام:
#   ./run_regression_tests.sh <sad_executable> <mode>
#   mode: interpreter | compiler
# ======================================================================

set -euo pipefail

SAD_EXE="${1:-}"
MODE="${2:-interpreter}"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# ألوان
RED='\033[31m'
GREEN='\033[32m'
YELLOW='\033[33m'
CYAN='\033[36m'
BOLD='\033[1m'
RESET='\033[0m'

# إحصائيات
total=0
passed=0
failed=0
xfail=0
failed_names=()

# التحقق من المعاملات
if [ -z "$SAD_EXE" ]; then
    echo -e "${RED}Usage: $0 <sad_executable> [interpreter|compiler]${RESET}"
    exit 1
fi

if [ ! -x "$SAD_EXE" ]; then
    echo -e "${RED}❌ Executable not found: $SAD_EXE${RESET}"
    exit 1
fi

# ══════════════════════════════════════════════════════════════════════
# (AR) الحمرةُ المعروفةُ **تُشتقُّ من السجلِّ ولا تُكتَبُ هنا** — توأمُ ما في
#      `run_regression_tests.ps1`. وسُدَّ معها فخّان:
#        ① قائمةٌ فارغةٌ تجعل كلَّ دَينٍ مُعلَنٍ إخفاقًا. وهي أحمرَّت على
#           ويندوز يومَ صار مترجمُها يُبنى فعلًا؛ وهذا التوأمُ لم يُشغَّل
#           بوضعِ المترجمِ بعدُ، فسُدَّ اليومَ **قبلَ أن يكلّف**.
#        ② والمطابقةُ كانت **جزئيّةً**: `grep -q "$name"` يجعل `test_p3`
#           يطابق `test_p30`، فتبتلع القائمةُ اسمًا لم يُسجَّل قطُّ. صارت
#           `-qxF`: سطرٌ كاملٌ حرفيّ.
#      🔑 والمصدرُ سجلٌّ محروسٌ في الاتّجاهَين بـ`test_declared_reds_registry.py`،
#      فليست قائمةَ إذنٍ تبلى: صفٌّ يخضرُّ يُخفِقُ هناك حتّى يُحذَف، وأحمرُ
#      ليس فيه يُخفِقُ هناك حتّى يُسجَّل.
# (EN) Known reds are DERIVED from the registry, twin of the .ps1. Two traps
#      sealed: an empty list turned every declared debt into a failure, and the
#      match was a SUBSTRING (test_p3 matched test_p30) — now -qxF, exact line.
# ══════════════════════════════════════════════════════════════════════
REGISTRY_PATH="$SCRIPT_DIR/../DECLARED_REDS.tsv"
if [ ! -f "$REGISTRY_PATH" ]; then
    echo -e "${RED}Registry not found: $REGISTRY_PATH${RESET}"
    exit 1
fi
KNOWN_FAILURES="$(grep '^_regression/' "$REGISTRY_PATH" | cut -f1 | sed -e 's#^_regression/##' -e 's#[.][^.]*$##' || true)"
# (AR) قائمةٌ خاويةٌ تعني انكسارَ الاشتقاقِ لا سدادَ الدَّين.
# (EN) An empty list means the derivation broke, not that the debt was paid.
if [ -z "$KNOWN_FAILURES" ]; then
    echo -e "${RED}No _regression rows parsed from $REGISTRY_PATH${RESET}"
    exit 1
fi
xfail_names=()

echo ""
echo -e "${BOLD}╔════════════════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}║  🧪 اختبارات الانحدار — لغة ص                        ║${RESET}"
echo -e "${BOLD}║  Regression Tests — Sad Language                      ║${RESET}"
echo -e "${BOLD}╠════════════════════════════════════════════════════════╣${RESET}"
echo -e "${BOLD}║  Mode: $MODE${RESET}"
echo -e "${BOLD}║  Tool: $SAD_EXE${RESET}"
echo -e "${BOLD}╚════════════════════════════════════════════════════════╝${RESET}"
echo ""

# البحث عن ملفات الاختبار
test_files=$(find "$SCRIPT_DIR" -name "test_p*.ص" | sort)
test_count=$(echo "$test_files" | wc -l)

for test_file in $test_files; do
    total=$((total + 1))
    test_name=$(basename "$test_file" .ص)
    
    printf "${CYAN}  [%d/%d] ${RESET}%s ... " "$total" "$test_count" "$test_name"
    
    start_time=$(date +%s%N 2>/dev/null || date +%s)
    
    if [ "$MODE" = "interpreter" ]; then
        # تشغيل المفسر مع timeout
        output=$(timeout 30 "$SAD_EXE" "$test_file" 2>/tmp/sad_test_err.txt || true)
        exit_code=$?
        stderr=$(cat /tmp/sad_test_err.txt 2>/dev/null || echo "")
    else
        # وضع المترجم
        out_exe="/tmp/sad_test_$(echo "$test_name" | tr -dc '0-9').out"
        
        # ترجمة
        compile_output=$("$SAD_EXE" "$test_file" -o "$out_exe" 2>/tmp/sad_compile_err.txt || true)
        compile_exit=$?
        
        if [ $compile_exit -ne 0 ] || [ ! -x "$out_exe" ]; then
            end_time=$(date +%s%N 2>/dev/null || date +%s)
            
            if printf "%s
" "$KNOWN_FAILURES" | grep -qxF "$test_name"; then
                xfail=$((xfail + 1))
                xfail_names+=("$test_name (compile)")
                echo -e "${YELLOW}XFAIL (compile error)${RESET}"
            else
                failed=$((failed + 1))
                failed_names+=("$test_name (compile)")
                echo -e "${RED}FAIL (compile error)${RESET}"
                compile_err=$(cat /tmp/sad_compile_err.txt 2>/dev/null | head -3)
                [ -n "$compile_err" ] && echo -e "    ${RED}$compile_err${RESET}"
            fi
            continue
        fi
        
        # تشغيل
        output=$(timeout 30 "$out_exe" 2>/tmp/sad_test_err.txt || true)
        exit_code=$?
        stderr=$(cat /tmp/sad_test_err.txt 2>/dev/null || echo "")
        rm -f "$out_exe"
    fi
    
    end_time=$(date +%s%N 2>/dev/null || date +%s)
    
    # تحليل النتائج
    has_fail=$(echo "$output" | grep -c "FAIL:" || true)
    has_timeout=$( [ $exit_code -eq 124 ] && echo "1" || echo "0")
    
    if [ "$has_timeout" = "1" ]; then
        if printf "%s
" "$KNOWN_FAILURES" | grep -qxF "$test_name"; then
            xfail=$((xfail + 1))
            xfail_names+=("$test_name (timeout)")
            echo -e "${YELLOW}XFAIL (timeout)${RESET}"
        else
            failed=$((failed + 1))
            failed_names+=("$test_name (timeout)")
            echo -e "${RED}FAIL (timeout — infinite loop?)${RESET}"
        fi
    elif [ "$has_fail" -gt 0 ] || [ $exit_code -ne 0 ]; then
        if printf "%s
" "$KNOWN_FAILURES" | grep -qxF "$test_name"; then
            xfail=$((xfail + 1))
            xfail_names+=("$test_name")
            echo -e "${YELLOW}XFAIL${RESET}"
        else
            failed=$((failed + 1))
            failed_names+=("$test_name")
            echo -e "${RED}FAIL${RESET}"
            echo "$output" | grep "FAIL:" | while read -r line; do
                echo -e "    ${RED}$line${RESET}"
            done
            [ -n "$stderr" ] && echo "$stderr" | head -3 | while read -r line; do
                echo -e "    ${RED}$line${RESET}"
            done
        fi
    else
        passed=$((passed + 1))
        echo -e "${GREEN}PASS${RESET}"
    fi
done

# ======================================================================
# الملخص
# ======================================================================
echo ""
echo -e "${BOLD}╔════════════════════════════════════════════════════════╗${RESET}"
echo -e "${BOLD}║  📊 ملخص اختبارات الانحدار / Regression Test Summary ║${RESET}"
echo -e "${BOLD}╚════════════════════════════════════════════════════════╝${RESET}"
echo -e "${CYAN}  المجموع / Total:     ${RESET}$total"
echo -e "${GREEN}  نجح / Passed:        ${RESET}$passed"
echo -e "${RED}  فشل / Failed:        ${RESET}$failed"
echo -e "${YELLOW}  فشل متوقع / XFail:   ${RESET}$xfail"

# (AR) الأسماءُ تُطبَع دائمًا: عددٌ بلا أسماءٍ لا يكشف تبدُّلَ المجموعة.
# (EN) Names always printed: a count alone hides a change of membership.
if [ ${#xfail_names[@]} -gt 0 ]; then
    echo ""
    echo -e "${YELLOW}${BOLD}  حمرةٌ مسجَّلةٌ في DECLARED_REDS.tsv / XFail:${RESET}"
    for name in "${xfail_names[@]}"; do
        echo -e "${YELLOW}    ~ $name${RESET}"
    done
fi

if [ ${#failed_names[@]} -gt 0 ]; then
    echo ""
    echo -e "${RED}${BOLD}  الاختبارات الفاشلة / Failed Tests:${RESET}"
    for name in "${failed_names[@]}"; do
        echo -e "${RED}    ✗ $name${RESET}"
    done
fi

echo ""
if [ $failed -eq 0 ]; then
    echo -e "${GREEN}${BOLD}  ✅ جميع اختبارات الانحدار نجحت!${RESET}"
    echo -e "${GREEN}${BOLD}  ✅ All regression tests passed!${RESET}"
    exit 0
else
    rate=$(echo "scale=1; $passed * 100 / $total" | bc 2>/dev/null || echo "?")
    echo -e "${RED}${BOLD}  ❌ نسبة النجاح: ${rate}%${RESET}"
    exit 1
fi
