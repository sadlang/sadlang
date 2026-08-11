#!/bin/bash
# ============================================================================
# (AR) برهانُ الإرسالِ الافتراضيّ الحيّ: يشغّل ثنائيَّي ELF خفّضتهما الخلفيّةُ السياديّة
#      (بلا clang/lld/as) من برنامجِ ص فيه «قط يرث حيوان» ويتجاوز «صوت»، ويؤكّد رمزَ
#      الخروجِ ٤٢ لكليهما. الوسيطان: ثنائيُّ الإرسالِ البسيط، وثنائيُّ الإرسالِ ومؤقّتٌ
#      حيٌّ عبرَ النداء. ووسيطٌ ثالثٌ اختياريٌّ: مُشغِّلٌ (qemu-aarch64) لبرهانِ AArch64.
#
#      ولمَ برهانٌ حيٌّ وفي gtest تأكيدٌ بايتيٌّ أصلًا؟ لأنّ التأكيدَ البايتيَّ يشهد
#      بوجودِ `call rax`/`blr` ولا يشهد بصحّةِ ما تُنادى: خانةُ vtable خاطئةٌ أو جدولٌ
#      مُبدَّلُ المدخلَين يُبقيان البايتَ ويكسران الجواب. ورمزُ الخروجِ هو الشاهدُ الوحيدُ
#      على الصحّة. والعيّنةُ مبنيّةٌ لذلك: طرحٌ لا جمع (٤٤‑٢) فينقلب الجوابُ إلى ٢١٤
#      إن انعكس الجدول، ولا طرفَ صفريٌّ فتُميَّزَ القيمةُ الضائعةُ من الإرسالِ الصحيح.
# (EN) Live virtual-dispatch proof: runs the two sovereign-backend ELF binaries and
#      asserts exit code 42 for both. The gtest byte assertion only witnesses that an
#      indirect call exists; only the exit code witnesses that it dispatches correctly.
# ============================================================================
set -u
BIN_SIMPLE="${1:?usage: prove_virtual_dispatch.sh <virtual-binary> <livetemp-binary> [runner]}"
BIN_LIVETEMP="${2:?usage: prove_virtual_dispatch.sh <virtual-binary> <livetemp-binary> [runner]}"
RUNNER="${3:-}" # (AR) اختياريّ: qemu-aarch64 لبرهانِ ARM64 على مضيفِ x86-64

EXPECT=42
FAIL=0

# (AR) وسمُ المعماريّةِ في المخرَج: بلا مُشغِّلٍ ⇒ المضيفُ نفسُه، ومعه ⇒ الهدفُ المُحاكى.
#      بدونه تتطابقُ أربعةُ أسطرِ PASS في سجلِّ CI فلا يُعرَف أيُّها أخفق.
if [ -n "$RUNNER" ]; then ARCH="aarch64"; else ARCH="host"; fi

run_one() {
    BIN="$1"
    LABEL="$2"
    if [ ! -f "$BIN" ]; then
        echo "FAIL: $LABEL — الثنائيُّ غيرُ موجود: $BIN"
        FAIL=1
        return
    fi
    chmod +x "$BIN" 2>/dev/null || true
    $RUNNER "$BIN"
    CODE=$?
    echo "$LABEL EXIT_CODE=$CODE"
    if [ "$CODE" -eq "$EXPECT" ]; then
        echo "PASS: $LABEL — الإرسالُ الافتراضيُّ خرج بـ٤٢ / virtual dispatch exited 42"
    elif [ "$CODE" -eq 214 ]; then
        echo "FAIL: $LABEL — رمزُ الخروجِ ٢١٤ (‑٤٢) ⇒ جدولُ الدوالِّ مُبدَّلُ المدخلَين"
        FAIL=1
    else
        echo "FAIL: $LABEL — توقّعنا ٤٢ ووجدنا $CODE / expected 42, got $CODE"
        FAIL=1
    fi
}

run_one "$BIN_SIMPLE" "virtual[$ARCH]"
run_one "$BIN_LIVETEMP" "virtual_livetemp[$ARCH]"

exit "$FAIL"
