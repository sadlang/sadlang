#!/usr/bin/env bash
# ─────────────────────────────────────────────────────────────────────────────
# (AR) apt بمحاولاتٍ محدودةِ الزمن — لأنّ الانقطاعَ يقع ولا يُبلِّغُ عن نفسه.
#
#      🔑 الدافعُ مقيسٌ لا مُفترَض: في ٢٠٢٦-٠٨-١٨ عند ٠٢:٠٣ ت.ع علقت **ثلاثُ
#      وظائفَ** في الثانيةِ نفسِها على **خطوتَي `apt-get` مختلفتَين**:
#        · 🐧 Linux (Debug)  — «تثبيت المتطلبات»   ٦٠:٠٧ ثمّ قُطِعت
#        · 🔍 تحليل ساكن ×٢  — «تثبيت أدوات التحليل» ٣٠:٠٨ ثمّ قُطِعت
#      وخطُّ الأساسِ لخطوةِ التحليلِ **١١ ثانية**، وفي الإعادةِ بعدَ ٤٧ دقيقةً
#      نجحت في **١:٣٠**. فالعطبُ في المرآةِ لا في الحزم.
#
#      ⚠️ ولم يكن يُمسِكُه إلّا حدُّ الوظيفةِ (٣٠ و٦٠ دقيقة) — أي بعدَ أن يُنفَقَ
#      الزمنُ كلُّه. وهنا يُقصَرُ الانتظارُ إلى `SAD_APT_TIMEOUT` لكلِّ محاولة،
#      ويُعادُ ما يُعادُ منه: أغلبُ هذه الانقطاعاتِ تتعافى في المحاولةِ الثانية.
#
#      ولا يُبتلَعُ الإخفاقُ النهائيّ: بعدَ استنفادِ المحاولاتِ يخرجُ برمزٍ غيرِ صفر
#      ويُصدِرُ `::error::` — فالمتصل هو من يقرّر التسامحَ (بـ`|| true`) لا نحن.
#
# (EN) apt with bounded, retried attempts. Motivation is measured, not assumed:
#      on 2026-08-18 at 02:03 UTC three jobs hung simultaneously on two different
#      apt-get steps (60:07 and 30:08 before their job timeouts fired) while the
#      baseline for one of them is 11 s — and a rerun 47 min later took 1:30.
#      Only the job-level timeout caught it, i.e. after the whole budget burned.
#
# الاستعمال / usage:
#      bash scripts/ci/apt_retry.sh update
#      bash scripts/ci/apt_retry.sh install -y cmake g++ ...
# ─────────────────────────────────────────────────────────────────────────────
set -uo pipefail

ATTEMPTS="${SAD_APT_ATTEMPTS:-3}"
PER_ATTEMPT_SECONDS="${SAD_APT_TIMEOUT:-300}"

if [ "$#" -eq 0 ]; then
    echo "::error::apt_retry.sh: لا أمر — المتوقّع update أو install" >&2
    exit 2
fi

attempt=1
while [ "$attempt" -le "$ATTEMPTS" ]; do
    echo "▶️ apt $1 — المحاولة $attempt من $ATTEMPTS (حدّ $PER_ATTEMPT_SECONDS ث)"
    # (AR) `timeout` يقتل المحاولةَ العالقةَ وحدَها لا الوظيفةَ كلَّها.
    # (EN) timeout kills the stuck attempt, not the whole job.
    if sudo timeout "$PER_ATTEMPT_SECONDS" apt-get "$@"; then
        echo "✅ apt $1 — نجحت في المحاولة $attempt"
        exit 0
    fi
    status=$?
    if [ "$status" -eq 124 ]; then
        echo "⏰ المحاولة $attempt تجاوزت $PER_ATTEMPT_SECONDS ث — يُعاد"
    else
        echo "⚠️ المحاولة $attempt أخفقت برمز $status — يُعاد"
    fi
    attempt=$((attempt + 1))
    [ "$attempt" -le "$ATTEMPTS" ] && sleep 10
done

# (AR) الإخفاقُ بعدَ استنفادِ المحاولاتِ يُعلَنُ ولا يُبتلَع.
# (EN) Exhausted retries are reported, never swallowed.
echo "::error::apt $1 أخفقت بعد $ATTEMPTS محاولات بحدّ $PER_ATTEMPT_SECONDS ث لكلٍّ" >&2
exit 1
