#!/usr/bin/env bash
# ─────────────────────────────────────────────────────────────────────────────
# (AR) أيُّ جالبٍ شبكيٍّ بمحاولاتٍ محدودةِ الزمن — أخو `apt_retry.sh` وقد وُسِّع.
#
#      🔑 الدافعُ مقيسٌ لا مُفترَض. في ١٩ آب ٢٠٢٦ مُسِحت آخرُ ٢٠٠ شوطٍ في
#      المستودع، ووسيطُ زمنِ الشوطِ نحوُ ٢٠ دقيقة. وفيها **أربعةُ أشواطٍ**
#      خرجت عن السرب، وكلُّها عالقةٌ على خطوةِ جلبٍ شبكيّ:
#
#        · 32066236587 و32066237695 — 🪟 Windows (Release)، خطوةُ «تحميل LLVM 18»
#          ٥:٠١:٢١ لكلٍّ على `Invoke-WebRequest` — ولم يقطعْهما حدُّ زمنٍ البتّةَ
#          بل إلغاءُ التزامنِ حين دُفِعَ تغييرٌ جديد. أي أنّهما كانا سيمضيان.
#        · 32094983203 — «قياس منافذ العائلة»، خطوةُ «تثبيت LLVM 18»: ٦:٠٠:٠٢
#        · 32096599168 — 🐧 Linux (Debug)، خطوةُ «تثبيت المتطلبات»:   ٦:٠٠:٠٧
#          وهذان بلغا سقفَ ﭼِتهَب الافتراضيَّ (٣٦٠ دقيقة) لأنّ وظيفتَيهما كانتا
#          بلا `timeout-minutes` — فالسقفُ الوحيدُ كان سقفَ المنصّةِ لا سقفَنا.
#
#      فالعلّةُ صنفٌ لا حادثة: **جلبٌ شبكيٌّ بلا إعادةِ محاولةٍ وبلا حدٍّ زمنيّ**.
#      وقد سُدَّ منها `apt` وحدَه؛ وهذا يسدُّ إخوتَه: brew وwget وcurl وchoco…
#
#      ولا يُبتلَعُ الإخفاقُ النهائيّ: بعدَ استنفادِ المحاولاتِ يخرجُ برمزٍ غيرِ
#      صفرٍ ويُصدِرُ `::error::` — فالمتصلُ هو من يقرّرُ التسامحَ لا نحن.
#
# (EN) Generic bounded-and-retried network fetch — sibling of apt_retry.sh.
#      Motivation measured on 2026-08-19 over the last 200 runs (median ≈20 min):
#      four outliers, all stuck on a network fetch step — two Windows jobs at
#      5h01m21s on Invoke-WebRequest (killed only by concurrency cancellation,
#      never by a timeout) and two jobs at exactly 6h, GitHub's default ceiling,
#      because their jobs declared no timeout-minutes.
#
# الاستعمال / usage:
#      bash scripts/ci/net_retry.sh brew install sdl2 sdl2_ttf
#      bash scripts/ci/net_retry.sh curl -fsSL -o out.yaml https://example/x.yaml
#      bash scripts/ci/net_retry.sh wget -qO- https://example/key.gpg
#
# ⚠️ لا يُمرَّرُ عبرَه فعلٌ **غيرُ متكرّرِ الأثر** (POST مثلًا) — فإعادتُه تُكرّرُ
#    أثرَه. تلك تُحَدُّ بالزمنِ فقط وتُعلَّلُ بعلامةِ `SAD_CI_NO_RETRY` في مكانها.
# ─────────────────────────────────────────────────────────────────────────────
set -uo pipefail

ATTEMPTS="${SAD_NET_ATTEMPTS:-3}"
PER_ATTEMPT_SECONDS="${SAD_NET_TIMEOUT:-600}"
BACKOFF_SECONDS="${SAD_NET_BACKOFF:-10}"

if [ "$#" -eq 0 ]; then
    echo "::error::net_retry.sh: لا أمر — المتوقّع أمرُ الجلبِ ووسائطُه" >&2
    exit 2
fi

LABEL="$1"

# (AR) `timeout` من coreutils وليس في كلِّ منصّة: ماك يأتي بلا coreutils افتراضًا.
#      فلا يُدَّعى حدٌّ زمنيٌّ لا وجودَ له — يُعلَنُ غيابُه ويُبقى على الإعادة،
#      وحدُّ الوظيفةِ (`timeout-minutes`) هو الشبكةُ الأخيرةُ حينئذٍ.
# (EN) coreutils `timeout` is absent on macOS runners. Never claim a bound we
#      do not have: announce its absence and fall back to the job-level timeout.
if command -v timeout >/dev/null 2>&1; then
    BOUND="timeout"
elif command -v gtimeout >/dev/null 2>&1; then
    BOUND="gtimeout"
else
    BOUND=""
    echo "⚠️ لا `timeout` ولا `gtimeout` على هذه المنصّة — تُعادُ المحاولاتُ بلا حدٍّ لكلٍّ"
fi

attempt=1
while [ "$attempt" -le "$ATTEMPTS" ]; do
    if [ -n "$BOUND" ]; then
        echo "▶️ $LABEL — المحاولة $attempt من $ATTEMPTS (حدّ $PER_ATTEMPT_SECONDS ث)"
        "$BOUND" "$PER_ATTEMPT_SECONDS" "$@"
        status=$?
    else
        echo "▶️ $LABEL — المحاولة $attempt من $ATTEMPTS (بلا حدٍّ لكلِّ محاولة)"
        "$@"
        status=$?
    fi

    if [ "$status" -eq 0 ]; then
        echo "✅ $LABEL — نجحت في المحاولة $attempt"
        exit 0
    fi
    if [ "$status" -eq 124 ]; then
        echo "⏰ المحاولة $attempt تجاوزت $PER_ATTEMPT_SECONDS ث — يُعاد"
    else
        echo "⚠️ المحاولة $attempt أخفقت برمز $status — يُعاد"
    fi
    attempt=$((attempt + 1))
    [ "$attempt" -le "$ATTEMPTS" ] && sleep "$BACKOFF_SECONDS"
done

echo "::error::$LABEL أخفق بعد $ATTEMPTS محاولات بحدّ $PER_ATTEMPT_SECONDS ث لكلٍّ" >&2
exit 1
