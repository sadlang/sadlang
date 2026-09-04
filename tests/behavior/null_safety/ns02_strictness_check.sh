#!/usr/bin/env bash
# ============================================================================
# (NS-02) فحص صرامة أمان null الثلاثيّة عبر المحرّكين — ADR-NS-001 D6/D8/D10
#
# (AR) المُشغّل القياسيّ (runner.py) يقارن stdout بلا أعلام، فلا يستطيع تغطية
#      اختلاف السلوك حسب علم الذاكرة. هذا السكربت يؤكّد الصرامة الثلاثيّة لحالة P9
#      (إسناد «لاشيء» حرفيًّا لنوع غير اختياري) في **المفسّر والمترجم** معًا:
#        --تعلم (افتراضيّ) → تحذير على stderr + خروج 0 (التنفيذ يكمل)
#        --جامع               → صامت (لا تحذير أمان null)
#        --إنتاج             → خطأ قاتل (خروج ≠ 0 / فشل البناء)
#
# (EN) Asserts the three NS-02 strictness levels for the P9 case across both
#      engines (interpreter + compiler), which runner.py cannot cover (no flags).
#
# الاستعمال:  bash tests/behavior/null_safety/ns02_strictness_check.sh
#
# ⚠️ **دَينٌ مُقيَّدٌ — هذا السكربتُ لا يُنادَى من أيِّ بوّابةٍ ولا سيرِ عمل.**
#
#      ثلاثةُ نداءاتٍ للمفسّرِ وثلاثةٌ للمترجم. قِيس ٢٠٢٦-٠٩-٠٤: المترجمُ ١ نجح / ١ فشل («learn: البناء ينجح»)، والمفسّرُ ٤ إخفاقاتٍ سببُها غيابُه. فالإخفاقُ المترجِميُّ الواحدُ يحتاجُ فحصًا: أهو عيبٌ قائمٌ أم توكيدٌ لم يكنْ يصدُقُ إلّا في عالمِ المحرّكَين.
#
#      وسببُ التقييدِ أنّ «متجاهَل» يجبُ أن يبقى قرارًا لا نسيانًا: حارسٌ
#      لا يُنادَى ليس حارسًا، وسكوتُه يُقرأُ خضرةً وهو صمتٌ. والوصلُ
#      يحتاجُ فصلَ نصفِ المفسّرِ أوّلًا — وذاك عملٌ مقصودٌ لا يُقحَمُ في
#      رقعةِ تنظيف. والنظيرُ الموصولُ: `ns03` (بوّابةٌ في `ci.yml`).
# (EN) TRACKED DEBT — no gate or workflow invokes this script. "Ignored"
#      must stay a decision, never an oversight: a guard nobody calls is
#      not a guard, and its silence reads as green while it is only
#      silence. Wiring it up requires splitting the interpreter half out
#      first, which is deliberate work, not a cleanup patch. The wired
#      sibling is ns03 (a gate in ci.yml).
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
# --تعلم: تحذير على stderr + خروج 0
err=$("$RUN" --تعلم "$SRC" 2>&1 1>/dev/null); rc=$?
echo "$err" | grep -q "اختياري"; check "learn: تحذير P9 يظهر" $?
check "learn: الخروج 0 (يكمل)" $([ $rc -eq 0 ] && echo 0 || echo 1)
# --جامع: لا تحذير أمان null
err=$("$RUN" --جامع "$SRC" 2>&1 1>/dev/null)
echo "$err" | grep -q "اختياري"; g=$?; check "gc: لا تحذير أمان null" $([ $g -ne 0 ] && echo 0 || echo 1)
# --إنتاج: خطأ قاتل (خروج ≠ 0)
"$RUN" --إنتاج "$SRC" >/dev/null 2>&1; rc=$?
check "prod: خطأ قاتل (خروج ≠ 0)" $([ $rc -ne 0 ] && echo 0 || echo 1)

echo "== المترجم (sad-build) =="
if [ -x "$BUILD" ]; then
  out="$TMP/p9.exe"
  # --تعلم: يبني بنجاح (خروج 0)
  "$BUILD" --تعلم "$SRC" -o "$out" >/dev/null 2>&1; check "learn: البناء ينجح" $?
  # --إنتاج: يفشل البناء (خروج ≠ 0 أو رسالة خطأ)
  perr=$("$BUILD" --إنتاج "$SRC" -o "$out" 2>&1);
  echo "$perr" | grep -qiE "error|خطأ"; check "prod: البناء يبلّغ خطأ P9" $?
else
  echo "  (تخطّي المترجم — sad-build.exe غير موجود)"
fi

rm -rf "$TMP"
echo "── النتيجة: نجح=$pass فشل=$fail ──"
[ "$fail" -eq 0 ]
