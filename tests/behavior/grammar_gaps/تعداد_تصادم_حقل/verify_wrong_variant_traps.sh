#!/usr/bin/env bash
# ============================================================================
# (ISSUE-080/ب) فحص أنّ الوصول لحقلٍ على حالةٍ خاطئة يُوقف المترجم (خروج ≠0)
#
# (AR) المُشغّل القياسيّ (runner.py:428) يُرجع FAIL_INTERP قبل تشغيل المترجم عند
#      خطأ تشغيل المفسّر، وكتلة @expect_error تفحص المفسّر وحده — فلا يثبّت أيّ
#      منهما إصلاحَ جانب المترجم للعلّة (ب). هذا السكربت يسدّ ذلك: يترجم الكاشف
#      ويشغّل التنفيذيّ ويؤكّد **خروجًا ≠0** (trap حتميّ بدل قمامة صامتة rc=0).
#      تطابقُ رسالة RUN005 البايتيّ للمفسّر غير عمليّ (ErrorManager كامل)؛ الثابت
#      المضمون = عدم إنتاج قيمةٍ كاذبة (الوصول لحالةٍ لا تحوي الحقل يوقف التنفيذ).
# (EN) The standard runner short-circuits interpreter runtime errors before the
#      compiler, and @expect_error checks the interpreter only — neither locks the
#      compiler-side fix for cause (b). This script compiles the detector, runs the
#      executable, and asserts exit≠0 (deterministic trap instead of silent rc=0
#      garbage). Byte-matching the interpreter's RUN005 is impractical; the
#      guaranteed invariant is «no fabricated value».
#
# الاستعمال:  bash tests/behavior/grammar_gaps/تعداد_تصادم_حقل/verify_wrong_variant_traps.sh
# ============================================================================
set -u
ROOT="$(cd "$(dirname "$0")/../../../.." && pwd)"
RUN="$ROOT/build/bin/Release/sad-run.exe"
BUILD="$ROOT/build/bin/Release/sad-build.exe"
DIR="$(dirname "$0")"
WRONG="$DIR/002_wrong_variant_field.ص"
TMP="$(mktemp -d)"

pass=0; fail=0
check() { if [ "$2" -eq 0 ]; then echo "  ✅ $1"; pass=$((pass+1)); else echo "  ❌ $1"; fail=$((fail+1)); fi; }

echo "== العلّة (ب): وصول حقلٍ لحالةٍ خاطئة =="
# (AR) المفسّر يرفع RUN005 (خروج ≠0) — المرجع.
"$RUN" "$WRONG" >/dev/null 2>&1; irc=$?
check "المفسّر: خطأ تشغيل (خروج ≠0)" $([ $irc -ne 0 ] && echo 0 || echo 1)

if [ -x "$BUILD" ]; then
  OUT="$TMP/wrong.exe"
  "$BUILD" "$WRONG" -o "$OUT" >/dev/null 2>&1; check "المترجم: البناء ينجح (خطأ زمن تشغيل لا ترجمة)" $?
  "$OUT" >/dev/null 2>&1; crc=$?
  # (AR) الجوهر: خروج ≠0 (trap) لا 0 (قمامة صامتة). قبل ISSUE-080 كان rc=0 يطبع 7.
  check "المترجم: التنفيذيّ يتوقّف بخروج ≠0 (لا قمامة صامتة)" $([ $crc -ne 0 ] && echo 0 || echo 1)
else
  echo "  (تخطّي المترجم — sad-build.exe غير موجود)"
fi

rm -rf "$TMP"
echo "── النتيجة: نجح=$pass فشل=$fail ──"
[ "$fail" -eq 0 ]
