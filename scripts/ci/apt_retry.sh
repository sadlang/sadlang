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

# (AR) 🔑 إعادةُ المحاولةِ على المرآةِ العاطلةِ نفسِها ليست إعادةً. قِيسَ ذلك في
#      ١٩ آب ٢٠٢٦: ثلاثُ محاولاتٍ بلغت حدَّها ٣٠٠ ث كلُّها على
#      `azure.archive.ubuntu.com` (سطورُ `Ign:` متكرّرة) بينما
#      `archive.ubuntu.com` في السجلِّ نفسِه يُجيبُ `Hit:`. فلا بدَّ أن يتغيّرَ
#      **شيءٌ** بين المحاولتَين، وإلّا فالإعادةُ إنفاقُ زمنٍ بلا رجاء.
# (EN) Retrying the same dead mirror is not a retry. Measured 2026-08-19: three
#      300 s attempts all stalled on azure.archive.ubuntu.com while
#      archive.ubuntu.com answered Hit: in the very same log.
# 🔴 (AR) وأسماءُ متغيّراتِ الصدَفةِ **لاتينيّةٌ إلزامًا**: bash لا يقبلُ حرفًا
#      عربيًّا في اسمِ متغيّر، فـ`قائمة=/etc/…` تُنفَّذُ **أمرًا** لا إسنادًا،
#      و`$قائمة` تبقى نصًّا حرفيًّا. وهذا ما عطّلَ إسقاطَ المرآةِ في شوطِ
#      0ce56e40: الدالّةُ نُوديت وردَّت «لا قائمةَ» لأنّ المسارَ لم يُسنَدْ قطُّ،
#      و`bash -n` لا يراه لأنّه سليمُ النحوِ تمامًا. (وأسماءُ الدوالِّ العربيّةُ
#      مقبولةٌ — الفرقُ في المتغيّراتِ وحدَها، وذاك ما جعلَ العطبَ خفيًّا.)
# 🔴 (EN) Shell variable names must be ASCII: bash rejects Arabic letters in a
#      variable name, so `قائمة=…` runs as a command and `$قائمة` stays
#      literal — syntactically valid, so `bash -n` never sees it.
# ⚠️ (AR) يُعلِنُ قرارَه في كلِّ فرعٍ ولا يمتنعُ صامتًا: امتناعٌ بلا سببٍ مطبوعٍ
#      يُقرأُ «لم يلزمْ» وقد يكون «لم يستطعْ» — وهو عينُ فخِّ «الأخضرُ لم يُقَس».
#      قِيسَ في شوطِ بوّابةِ الرسوماتِ على 0ce56e40: مهلةُ ٣٠٠ ث وقعت ولم يُطبَعْ
#      سطرُ إسقاطٍ البتّةَ، فلم يُعرَفْ أيُّ شرطٍ ردَّه.
# ⚠️ (EN) Announce the decision on every branch; a silent decline reads as
#      "not needed" when it may be "not possible".
اسقط_المرآة_العالقة() {
    mirror_list=/etc/apt/apt-mirrors.txt
    if [ ! -f "$mirror_list" ]; then
        echo "ℹ️ لا $mirror_list — لا مرآةَ تُسقَط"
        return 0
    fi
    if ! grep -q 'azure\.archive\.ubuntu\.com' "$mirror_list"; then
        echo "ℹ️ لا مرآةَ azure في $mirror_list — لا شيءَ يُسقَط"
        return 0
    fi
    # (AR) لا تُترَكُ القائمةُ فارغةً: يلزم بقاءُ مرآةٍ واحدةٍ على الأقلّ.
    # (EN) Never empty the list: at least one mirror must survive.
    remaining=$(grep -cv 'azure\.archive\.ubuntu\.com' "$mirror_list")
    if [ "$remaining" -lt 1 ]; then
        echo "⚠️ azure هي المرآةُ الوحيدةُ في $mirror_list — لا تُسقَط"
        return 0
    fi
    echo "🔀 إسقاطُ مرآةِ azure ($remaining مرآةً تبقى) — المحاولةُ التالية عليها"
    if sudo sed -i '/azure\.archive\.ubuntu\.com/d' "$mirror_list"; then
        echo "✅ أُسقِطت · القائمةُ الآن:"
        sed 's/^/     /' "$mirror_list"
    else
        echo "⚠️ تعذّرَ تعديلُ $mirror_list — يُمضى بلا إسقاط"
    fi
}

attempt=1
while [ "$attempt" -le "$ATTEMPTS" ]; do
    echo "▶️ apt $1 — المحاولة $attempt من $ATTEMPTS (حدّ $PER_ATTEMPT_SECONDS ث)"
    # (AR) `timeout` يقتل المحاولةَ العالقةَ وحدَها لا الوظيفةَ كلَّها.
    # (EN) timeout kills the stuck attempt, not the whole job.
    # ⚠️ (AR) الرمزُ يُلتقَطُ من الأمرِ مباشرةً لا من بعدِ `if`: كانت الصيغةُ
    #      `if sudo timeout …; then … fi; status=$?` تُعطي **صفرًا دائمًا** في
    #      مسارِ الإخفاق (قيمةُ جملةِ `if` نفسِها)، فماتَ فرعُ ١٢٤ وطُبِعَ
    #      «أخفقت برمز 0» على مهلةٍ بلغت حدَّها تمامًا. قِيسَ في شوط 32226941372.
    # ⚠️ (EN) Capture the status from the command itself: `status=$?` after an
    #      `if` block reads the `if` statement's own status — always 0 on the
    #      failure path — which killed the 124 branch and printed "code 0".
    sudo timeout "$PER_ATTEMPT_SECONDS" apt-get "$@"
    status=$?
    if [ "$status" -eq 0 ]; then
        echo "✅ apt $1 — نجحت في المحاولة $attempt"
        exit 0
    fi
    if [ "$status" -eq 124 ]; then
        echo "⏰ المحاولة $attempt تجاوزت $PER_ATTEMPT_SECONDS ث — يُعاد"
    else
        echo "⚠️ المحاولة $attempt أخفقت برمز $status — يُعاد"
    fi
    اسقط_المرآة_العالقة
    attempt=$((attempt + 1))
    [ "$attempt" -le "$ATTEMPTS" ] && sleep 10
done

# (AR) الإخفاقُ بعدَ استنفادِ المحاولاتِ يُعلَنُ ولا يُبتلَع.
# (EN) Exhausted retries are reported, never swallowed.
echo "::error::apt $1 أخفقت بعد $ATTEMPTS محاولات بحدّ $PER_ATTEMPT_SECONDS ث لكلٍّ" >&2
exit 1
