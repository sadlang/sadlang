#!/bin/bash
# (AR) برهانُ حافّتَي بايتاتِ الملفّات — حالتان كانتا تمرّان خضراوين قبل إصلاحهما،
#      وكلتاهما ثبّتَها هذا السكربت كي لا تعودا صامتتين:
#
#      ١. «الطولان المتراكبان»: قراءتان في تعبيرٍ واحد (٣ + ٥ = ٨). كان مخفّضُ x86
#         يُهمل ضمَّ hasFileBytes إلى شرطِ حجزِ منطقةِ الانسكاب، فتركب خاناتُ الخدش
#         خاناتِ الانسكاب وتُعيد ٥ بدل ٨ — سوءُ ترجمةٍ صامتٌ لا انهيار. ARM64 كان
#         صحيحًا، فالتباعدُ بين المعماريّتين هو ما يكشفه هذا الفحص.
#
#      ٢. «قراءةُ مجلّد»: open ينجح وread يُعيد ‎-EISDIR‎. كان العائدُ السالبُ يُستعمَل
#         طولًا ⇒ حجمُ تخصيصٍ سالبٌ ⇒ كتابةٌ على مؤشّرِ خطأ ⇒ SIGSEGV (١٣٩) على
#         المعماريّتين. المتوقَّعُ الآن مصفوفةٌ فارغة (طول ٠) وخروجٌ نظيف.
#
# (EN) Two edge proofs for the raw-bytes builtins, both of which passed green before
#      they were fixed: (1) two reads in one expression — x86 omitted hasFileBytes
#      from the spill-area condition, so scratch slots aliased spill slots and it
#      returned 5 instead of 8 (a silent miscompile, while ARM64 was correct);
#      (2) reading a directory — read returns -EISDIR, and the negative return used
#      as a length produced a negative allocation and a SIGSEGV on both targets.
set -u
BIN_TWO="${1:?usage: prove_write_bytes_edges.sh <two-reads-binary> <read-dir-binary> [runner]}"
BIN_DIR="${2:?usage: prove_write_bytes_edges.sh <two-reads-binary> <read-dir-binary> [runner]}"
RUNNER="${3:-}"   # (AR) اختياريّ: qemu-aarch64-static لبرهانِ ARM64 على مضيفِ x86-64

EXPECT_TWO=8   # (AR) ٣ + ٥ — الطولان لا يتراكبان
EXPECT_DIR=0   # (AR) قراءةُ مجلّدٍ ⇒ مصفوفةٌ فارغةٌ لا انهيار

FAIL=0

run_case()
{
    local bin="$1" expect="$2" label="$3"
    chmod +x "$bin" 2>/dev/null || true
    $RUNNER "$bin"
    local code=$?
    echo "$label EXIT=$code (المتوقَّع $expect)"
    if [ "$code" -ne "$expect" ]; then
        if [ "$code" -ge 128 ]; then
            echo "FAIL: $label انهار بإشارة $((code - 128))"
        else
            echo "FAIL: $label أعاد $code والمتوقَّع $expect"
        fi
        FAIL=1
    fi
}

run_case "$BIN_TWO" "$EXPECT_TWO" "الطولان-المتراكبان"
run_case "$BIN_DIR" "$EXPECT_DIR" "قراءةُ-مجلّد"

if [ "$FAIL" -eq 0 ]; then
    echo "PASS: حافّتا بايتاتِ الملفّات سليمتان — لا تراكبَ خاناتٍ ولا طولَ سالب / no scratch-slot aliasing and no negative length"
    exit 0
fi
echo "FAIL: برهانُ الحافّتين أخفق"
exit 1
