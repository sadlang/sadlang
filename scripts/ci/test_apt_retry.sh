#!/usr/bin/env bash
# ─────────────────────────────────────────────────────────────────────────────
# (AR) قياسُ `apt_retry.sh` — وأخصُّ ما فيه: **فرعُ إسقاطِ المرآة**.
#
#      🔑 لماذا وُجِد: الفرعُ شُحِنَ إلى الإنتاجِ ولم يُقَسْ قطُّ. لا يُنادى إلّا
#      حين تُخفِقُ `apt` فعلًا، وفي الشوطِ الأخيرِ لم تُخفِقْ — فالشجرةُ خضراءُ
#      والفرعُ مجهول. وقبلَ ذلك كان **ميّتًا بالفعل**: `قائمة=/etc/…` اسمُ
#      متغيّرٍ عربيٌّ لا يقبله bash، فكان يُنفَّذُ أمرًا وتبقى `$قائمة` نصًّا،
#      فتردُّ الدالّةُ «لا قائمةَ» وهي تُخفِقُ لا تمتنع. و`bash -n` لا يرى ذلك.
#
#      ⚠️ ولا يُحاكى الفرعُ بإبدالِه: محاكاةُ الحلقةِ في تلك الجولةِ أبدلت نداءَ
#      الدالّةِ بـ`:` فاختبرت كلَّ شيءٍ **إلّا** ما أُضيف. فهنا يُشغَّلُ المُغلِّفُ
#      نفسُه بـ`apt-get` زائفٍ يُخفِق، فتُنادي الحلقةُ الدالّةَ كما في الإنتاج.
#
#      🔴 وأسماءُ المتغيّراتِ هنا **لاتينيّةٌ إلزامًا** — والدوالُّ عربيّة. أوّلُ
#      صياغةٍ لهذا الملفِّ كتبت `نجح=0` و`أخفق=0`، فانهارَ الملفُّ كلُّه وطبعَ
#      «✅ مقيسٌ» في آخرِ سطرٍ رغمَ أنّ كلَّ فحصٍ فيه أخفق: أداةُ القياسِ كذبت
#      خضراءَ بالعلّةِ عينِها التي جاءت تقيسُها.
#
# (EN) Measures apt_retry.sh, above all the mirror-drop branch, which shipped
#      unexercised and had already been silently dead once (Arabic shell
#      variable). Variables here are ASCII by force — the first draft of this
#      very file used Arabic ones and printed a green summary over total failure.
# ─────────────────────────────────────────────────────────────────────────────
set -uo pipefail

WRAPPER="$(cd "$(dirname "$0")" && pwd)/apt_retry.sh"
WORKSPACE="$(mktemp -d)"
LOG="$WORKSPACE/log.txt"
COUNTER="$WORKSPACE/count"
trap 'rm -rf "$WORKSPACE"' EXIT

passed=0
failed=0

احكم() {
    if [ "$2" = "نعم" ]; then
        echo "  ✅ $1"
        passed=$((passed + 1))
    else
        echo "  ❌ $1"
        failed=$((failed + 1))
    fi
}

يحوي() { if [ -f "$1" ] && grep -qF "$2" "$1"; then echo نعم; else echo لا; fi; }
لا_يحوي() { if [ -f "$1" ] && grep -qF "$2" "$1"; then echo لا; else echo نعم; fi; }

# (AR) `apt-get` زائفٌ يُقادُ بعدّادٍ على القرص — فتُبلَغُ كلُّ أفرعِ الحلقة.
اصنع_apt() {
    local mode="$1" bin="$WORKSPACE/fake-apt"
    rm -f "$COUNTER"
    {
        echo '#!/usr/bin/env bash'
        echo "n=\$(cat '$COUNTER' 2>/dev/null || echo 0); n=\$((n+1)); echo \"\$n\" > '$COUNTER'"
        case "$mode" in
            always_fail)    echo 'exit 100' ;;
            ok_on_second)   echo 'if [ "$n" -ge 2 ]; then exit 0; fi; exit 100' ;;
            hang)           echo 'sleep 30' ;;
        esac
    } > "$bin"
    chmod +x "$bin"
    echo "$bin"
}

اصنع_قائمة() {
    local list="$WORKSPACE/mirrors.txt" line
    : > "$list"
    for line in "$@"; do echo "$line" >> "$list"; done
    echo "$list"
}

شغّل() {
    SAD_APT_MIRROR_LIST="$1" SAD_APT_BIN="$2" SAD_APT_SUDO= \
    SAD_APT_ATTEMPTS="$3" SAD_APT_TIMEOUT="$4" SAD_APT_BACKOFF=0 \
        bash "$WRAPPER" update > "$LOG" 2>&1
    echo $?
}

echo "══ ١) الفرعُ الذي لم يُقَسْ قطُّ: إسقاطُ azure من قائمةٍ فيها غيرُها ══"
list=$(اصنع_قائمة "http://azure.archive.ubuntu.com/ubuntu/" "http://archive.ubuntu.com/ubuntu/")
apt_fail=$(اصنع_apt always_fail)
code=$(شغّل "$list" "$apt_fail" 2 5)
احكم "أُعلِنَ الإسقاطُ في السجلّ"       "$(يحوي "$LOG" 'إسقاطُ مرآةِ azure')"
احكم "أُعلِنَ نجاحُ التعديل"             "$(يحوي "$LOG" 'أُسقِطت')"
احكم "azure اختفت من القائمةِ فعلًا"     "$(لا_يحوي "$list" 'azure')"
احكم "archive.ubuntu.com بقيت"          "$(يحوي "$list" 'archive.ubuntu.com')"
احكم "الرمزُ ١ بعدَ استنفادِ المحاولات"   "$([ "$code" = 1 ] && echo نعم || echo لا)"
احكم "أُصدِرَ ::error:: ولم يُبتلَعْ"     "$(يحوي "$LOG" '::error::')"

echo "══ ٢) قائمةٌ من azure وحدَها — لا تُفرَّغُ القائمة ══"
list=$(اصنع_قائمة "http://azure.archive.ubuntu.com/ubuntu/")
code=$(شغّل "$list" "$apt_fail" 2 5)
احكم "امتنعَ وأعلنَ السبب"               "$(يحوي "$LOG" 'المرآةُ الوحيدةُ')"
احكم "القائمةُ لم تُمَسّ"                 "$(يحوي "$list" 'azure')"

echo "══ ٣) قائمةٌ بلا azure — امتناعٌ مُعلَن ══"
list=$(اصنع_قائمة "http://archive.ubuntu.com/ubuntu/")
code=$(شغّل "$list" "$apt_fail" 2 5)
احكم "أعلنَ «لا مرآةَ azure»"            "$(يحوي "$LOG" 'لا مرآةَ azure')"

echo "══ ٤) لا قائمةَ أصلًا — امتناعٌ مُعلَن ══"
code=$(شغّل "$WORKSPACE/no-such-file.txt" "$apt_fail" 2 5)
احكم "أعلنَ غيابَ القائمة"               "$(يحوي "$LOG" 'لا مرآةَ تُسقَط')"

echo "══ ٥) نجاحٌ في المحاولةِ الثانية — والإسقاطُ وقعَ بينهما ══"
list=$(اصنع_قائمة "http://azure.archive.ubuntu.com/ubuntu/" "http://archive.ubuntu.com/ubuntu/")
apt_second=$(اصنع_apt ok_on_second)
code=$(شغّل "$list" "$apt_second" 3 5)
احكم "الرمزُ صفرٌ"                       "$([ "$code" = 0 ] && echo نعم || echo لا)"
احكم "أُعلِنَ النجاحُ في المحاولة ٢"      "$(يحوي "$LOG" 'نجحت في المحاولة 2')"
احكم "أُسقِطت azure بين المحاولتَين"      "$(لا_يحوي "$list" 'azure')"

echo "══ ٦) مهلةٌ تبلغُ حدَّها — فرعُ ١٢٤ حيٌّ لا يُقرأُ «رمز 0» ══"
list=$(اصنع_قائمة "http://azure.archive.ubuntu.com/ubuntu/" "http://archive.ubuntu.com/ubuntu/")
apt_hang=$(اصنع_apt hang)
code=$(شغّل "$list" "$apt_hang" 2 1)
احكم "طُبِعَ سطرُ تجاوزِ المهلة"          "$(يحوي "$LOG" 'تجاوزت 1 ث')"
احكم "لم يُقرأْ الرمزُ صفرًا خطأً"         "$(لا_يحوي "$LOG" 'أخفقت برمز 0')"
احكم "الرمزُ ١ لا صفر"                    "$([ "$code" = 1 ] && echo نعم || echo لا)"

echo "─────────────────────────────────────────────"
echo "  نجح: $passed   ·   أخفق: $failed"
if [ "$passed" -eq 0 ]; then
    echo "::error::قياسُ apt_retry.sh: صفرُ فحصٍ نجح — الأداةُ نفسُها معطوبة"
    exit 1
fi
if [ "$failed" -ne 0 ]; then
    echo "::error::قياسُ apt_retry.sh: $failed فحصًا أخفق"
    exit 1
fi
# (AR) العددُ يُشتقُّ ولا يُكتَب: عددٌ منثورٌ نسخةٌ ثانيةٌ لحقيقةٍ تنجرف.
echo "✅ apt_retry.sh مقيسٌ في $passed فحصًا — وفرعُ إسقاطِ المرآةِ منها"
