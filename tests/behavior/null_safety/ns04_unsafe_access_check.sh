#!/usr/bin/env bash
# ============================================================================
# (NS-04) فحص صرامة رصد الوصول الخام على T؟ عبر المحرّكين — ADR-NS-001 D6/D11
#
# (AR) المُشغّل القياسيّ (runner.py) يقارن stdout بلا أعلام، فلا يغطّي اختلاف
#      السلوك حسب علم الذاكرة. هذا السكربت يؤكّد صرامة NS-04 الثلاثية لحالة
#      الوصول الخام (`م.الطول` على `نص؟`) في **المفسّر والمترجم** معًا:
#        --learn (افتراضيّ) → تحذير على stderr + خروج 0 (التنفيذ/البناء يكمل)
#        --gc               → صامت (لا تحذير أمان null)
#        --prod             → خطأ قاتل (خروج ≠ 0 / فشل البناء) [D11]
#      ويؤكّد **عدم** الرصد للوصول الآمن `؟.` (لا إيجابيّ كاذب).
#
# (EN) Asserts the three NS-04 strictness levels for raw-access on optionals
#      across both engines, plus no-false-positive on safe `?.` access.
#
# الاستعمال:  bash tests/behavior/null_safety/ns04_unsafe_access_check.sh
# ============================================================================
set -u
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
RUN="$ROOT/build/bin/Debug/sad-run.exe"
BUILD="$ROOT/build/bin/Release/sad-build.exe"
TMP="$(mktemp -d)"
# (AR) الدالّة غير مُستدعاة: الرصد ساكن، فلا انهيار زمنيّ على عدم.
RAW="$TMP/raw.ص"
printf 'دالة خطر(م: نص؟)\n    اطبع_سطر(م.الطول)\nنهاية\nاطبع_سطر("OK")\n' > "$RAW"
# (AR) وصول آمن `؟.` — يجب ألا يُرصَد إطلاقًا (لا إيجابيّ كاذب).
SAFE="$TMP/safe.ص"
printf 'دالة آمن(ك: نص؟)\n    اطبع_سطر(ك؟.الطول)\nنهاية\nاطبع_سطر("OK")\n' > "$SAFE"

pass=0; fail=0
check() { # وصف ، شرط(0=نجاح)
  if [ "$2" -eq 0 ]; then echo "  ✅ $1"; pass=$((pass+1));
  else echo "  ❌ $1"; fail=$((fail+1)); fi
}

echo "== المفسّر (sad-run) =="
err=$("$RUN" --learn "$RAW" 2>&1 1>/dev/null); rc=$?
echo "$err" | grep -q "غير آمن"; check "learn: تحذير NS-04 يظهر" $?
check "learn: الخروج 0 (يكمل)" $([ $rc -eq 0 ] && echo 0 || echo 1)
err=$("$RUN" --gc "$RAW" 2>&1 1>/dev/null)
echo "$err" | grep -q "غير آمن"; g=$?; check "gc: لا تحذير" $([ $g -ne 0 ] && echo 0 || echo 1)
"$RUN" --prod "$RAW" >/dev/null 2>&1; rc=$?
check "prod: خطأ قاتل (خروج ≠ 0)" $([ $rc -ne 0 ] && echo 0 || echo 1)
# (AR) لا إيجابيّ كاذب على الوصول الآمن حتى في --prod
"$RUN" --prod "$SAFE" >/dev/null 2>&1; rc=$?
check "safe: '؟.' لا يُرصَد (prod خروج 0)" $([ $rc -eq 0 ] && echo 0 || echo 1)

echo "== المترجم (sad-build) =="
if [ -x "$BUILD" ]; then
  out="$TMP/raw.exe"
  "$BUILD" --learn "$RAW" -o "$out" >/dev/null 2>&1; check "learn: البناء ينجح" $?
  perr=$("$BUILD" --prod "$RAW" -o "$out" 2>&1)
  echo "$perr" | grep -qiE "error|خطأ|غير آمن"; check "prod: البناء يبلّغ خطأ NS-04" $?
  "$BUILD" --prod "$SAFE" -o "$out" >/dev/null 2>&1; check "safe: '؟.' يبني في prod" $?
else
  echo "  (تخطّي المترجم — sad-build.exe غير موجود)"
fi

rm -rf "$TMP"
echo "── النتيجة: نجح=$pass فشل=$fail ──"
[ "$fail" -eq 0 ]
