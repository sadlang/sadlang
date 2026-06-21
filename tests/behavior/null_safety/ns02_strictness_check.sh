#!/usr/bin/env bash
# ============================================================================
# (NS-02) فحص صرامة أمان null الثلاثيّة عبر المحرّكين — ADR-NS-001 D6/D8/D10
#
# (AR) المُشغّل القياسيّ (runner.py) يقارن stdout بلا أعلام، فلا يستطيع تغطية
#      اختلاف السلوك حسب علم الذاكرة. هذا السكربت يؤكّد الصرامة الثلاثيّة لحالة P9
#      (إسناد «لاشيء» حرفيًّا لنوع غير اختياري) في **المفسّر والمترجم** معًا:
#        --learn (افتراضيّ) → تحذير على stderr + خروج 0 (التنفيذ يكمل)
#        --gc               → صامت (لا تحذير أمان null)
#        --prod             → خطأ قاتل (خروج ≠ 0 / فشل البناء)
#
# (EN) Asserts the three NS-02 strictness levels for the P9 case across both
#      engines (interpreter + compiler), which runner.py cannot cover (no flags).
#
# الاستعمال:  bash tests/behavior/null_safety/ns02_strictness_check.sh
# ============================================================================
set -u
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
RUN="$ROOT/build/bin/Debug/sad-run.exe"
BUILD="$ROOT/build/bin/Release/sad-build.exe"
TMP="$(mktemp -d)"
SRC="$TMP/p9.ص"
printf 'متغير س: رقم = لاشيء\nاطبع_سطر("AFTER:" + (س ؟؟ 1))\n' > "$SRC"

pass=0; fail=0
check() { # وصف ، شرط(0=نجاح)
  if [ "$2" -eq 0 ]; then echo "  ✅ $1"; pass=$((pass+1));
  else echo "  ❌ $1"; fail=$((fail+1)); fi
}

echo "== المفسّر (sad-run) =="
# --learn: تحذير على stderr + خروج 0
err=$("$RUN" --learn "$SRC" 2>&1 1>/dev/null); rc=$?
echo "$err" | grep -q "اختياري"; check "learn: تحذير P9 يظهر" $?
check "learn: الخروج 0 (يكمل)" $([ $rc -eq 0 ] && echo 0 || echo 1)
# --gc: لا تحذير أمان null
err=$("$RUN" --gc "$SRC" 2>&1 1>/dev/null)
echo "$err" | grep -q "اختياري"; g=$?; check "gc: لا تحذير أمان null" $([ $g -ne 0 ] && echo 0 || echo 1)
# --prod: خطأ قاتل (خروج ≠ 0)
"$RUN" --prod "$SRC" >/dev/null 2>&1; rc=$?
check "prod: خطأ قاتل (خروج ≠ 0)" $([ $rc -ne 0 ] && echo 0 || echo 1)

echo "== المترجم (sad-build) =="
if [ -x "$BUILD" ]; then
  out="$TMP/p9.exe"
  # --learn: يبني بنجاح (خروج 0)
  "$BUILD" --learn "$SRC" -o "$out" >/dev/null 2>&1; check "learn: البناء ينجح" $?
  # --prod: يفشل البناء (خروج ≠ 0 أو رسالة خطأ)
  perr=$("$BUILD" --prod "$SRC" -o "$out" 2>&1);
  echo "$perr" | grep -qiE "error|خطأ"; check "prod: البناء يبلّغ خطأ P9" $?
else
  echo "  (تخطّي المترجم — sad-build.exe غير موجود)"
fi

rm -rf "$TMP"
echo "── النتيجة: نجح=$pass فشل=$fail ──"
[ "$fail" -eq 0 ]
