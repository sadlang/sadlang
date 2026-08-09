#!/bin/bash
# (AR) برهانُ بايتاتِ الملفّات الحيّ: يشغّل ثنائيَّ ELF المُخفَّض من مصدرِ ص الذي يكتب
#      [72، 0، 255، 66] ثمّ يقرؤها، ويؤكّد ثلاثةَ أشياء — أنّ الطولَ المطبوعَ ٤ (فالبايتُ
#      الصفريُّ لم يقطع القراءة)، وأنّ الملفّ المكتوبَ حجمُه ٤ بايتات، وأنّ محتواه
#      «48 00 ff 42» حرفيًّا. هذا هو الحاجزُ الذي وُلدت له المدمجتان: المسارُ النصّيّ
#      (fputs/‎<<‎) يقف عند أوّل 0x00 فيستحيل رأسُ ELF.
# (EN) Live raw-bytes proof: runs the lowered ELF that writes [72,0,255,66] and reads
#      it back, asserting the printed length is 4 (the NUL did not truncate), the file
#      is 4 bytes, and its content is exactly "48 00 ff 42".
set -u
BIN="${1:?usage: prove_write_bytes.sh <binary-path> [runner]}"
RUNNER="${2:-}"   # (AR) اختياريّ: qemu-aarch64 لبرهانِ ARM64 على مضيفِ x86-64

OUT_FILE="sad_bytes_rt.bin"   # (AR) الاسمُ نفسُه في مصدر ص (نسبيٌّ لمجلّد العمل)
EXPECT_LEN=4
EXPECT_HEX="48 00 ff 42"

cd "$(dirname "$BIN")" || exit 1
BIN_NAME="$(basename "$BIN")"
rm -f "$OUT_FILE"
chmod +x "$BIN_NAME" 2>/dev/null || true

PRINTED="$($RUNNER ./"$BIN_NAME")"
CODE=$?
echo "PRINTED=$PRINTED EXIT_CODE=$CODE"

FAIL=0
[ "$CODE" -eq 0 ] || { echo "FAIL: رمزُ الخروج $CODE والمتوقَّع ٠"; FAIL=1; }
[ "$PRINTED" = "$EXPECT_LEN" ] || { echo "FAIL: الطولُ المطبوع «$PRINTED» والمتوقَّع $EXPECT_LEN — البايتُ الصفريُّ قطعَ القراءة"; FAIL=1; }

if [ ! -f "$OUT_FILE" ]; then
    echo "FAIL: لم يُكتب الملفّ $OUT_FILE"
    FAIL=1
else
    SIZE=$(stat -c%s "$OUT_FILE")
    HEX=$(od -An -tx1 "$OUT_FILE" | tr -s ' ' | sed 's/^ //;s/ $//')
    echo "SIZE=$SIZE HEX=$HEX"
    [ "$SIZE" -eq "$EXPECT_LEN" ] || { echo "FAIL: حجمُ الملفّ $SIZE والمتوقَّع $EXPECT_LEN"; FAIL=1; }
    [ "$HEX" = "$EXPECT_HEX" ] || { echo "FAIL: المحتوى «$HEX» والمتوقَّع «$EXPECT_HEX»"; FAIL=1; }
fi

if [ "$FAIL" -eq 0 ]; then
    echo "PASS: اكتب_بايتات/اقرأ_بايتات أصليًّا بلا libc — بايتٌ صفريٌّ مضمَّنٌ نجا ذهابًا وإيابًا / raw-bytes round trip with an embedded NUL survived, natively and libc-free"
    exit 0
fi
echo "FAIL: برهانُ بايتاتِ الملفّات أخفق"
exit 1
