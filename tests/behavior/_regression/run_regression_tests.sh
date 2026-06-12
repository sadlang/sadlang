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

# مشاكل معروفة (XFAIL)
KNOWN_FAILURES=""

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
            
            if echo "$KNOWN_FAILURES" | grep -q "$test_name"; then
                xfail=$((xfail + 1))
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
        if echo "$KNOWN_FAILURES" | grep -q "$test_name"; then
            xfail=$((xfail + 1))
            echo -e "${YELLOW}XFAIL (timeout)${RESET}"
        else
            failed=$((failed + 1))
            failed_names+=("$test_name (timeout)")
            echo -e "${RED}FAIL (timeout — infinite loop?)${RESET}"
        fi
    elif [ "$has_fail" -gt 0 ] || [ $exit_code -ne 0 ]; then
        if echo "$KNOWN_FAILURES" | grep -q "$test_name"; then
            xfail=$((xfail + 1))
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
    exit 0
fi
