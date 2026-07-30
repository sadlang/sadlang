#!/bin/bash
# (AR) برهانُ تدفّق التحكّم الحيّ: يشغّل ثنائيَّي ELF مُخفَّضَين من مصدر ص فيهما «إذا/وإلا»
#      عبر كتلٍ متعدّدة — أحدهما يخرج ٤٢ عبر فرع then (شرطٌ صادق)، والآخر عبر فرع else
#      (شرطٌ كاذب). نجاحُ كليهما بـ٤٢ يُثبت صحّةَ jCC + jmp + ترقيعِ rel32 في الاتّجاهين.
#      لا clang/lld/as ولا زمن تشغيل.
set -u
TRUE_BIN="${1:?usage: prove_sir_branch.sh <if-true> <if-false> <back-jump>}"
FALSE_BIN="${2:?usage: prove_sir_branch.sh <if-true> <if-false> <back-jump>}"
BACK_BIN="${3:?usage: prove_sir_branch.sh <if-true> <if-false> <back-jump>}"

run_case()
{
    local label="$1" bin="$2"
    echo "=== $label : disasm .text (offset 0x78) ==="
    objdump -D -b binary -m i386:x86-64 -M intel --start-address=0x78 "$bin" 2>/dev/null \
        | grep -iE 'mov|cmp|jmp|jg|jl|je|jne|syscall' | head -14
    chmod +x "$bin" 2>/dev/null || true
    "$bin"
    local code=$?
    echo "$label EXIT_CODE=$code"
    [ "$code" -eq 42 ] || { echo "FAIL: $label توقّعنا ٤٢ ووجدنا $code"; return 1; }
    return 0
}

ok=0
run_case "IF-TRUE(then)" "$TRUE_BIN"  || ok=1
echo
run_case "IF-FALSE(else)" "$FALSE_BIN" || ok=1
echo
run_case "BACK-JUMP(neg-rel32)" "$BACK_BIN" || ok=1

echo
if [ "$ok" -eq 0 ]; then
    echo "PASS: التفرّعُ السياديّ (إذا/وإلا + قفزٌ خلفيّ، كتلٌ متعدّدة) خرج ٤٢ في كلّ الحالات / sovereign branching (if/else + backward jump, multi-block) exited 42 in all cases"
    exit 0
fi
echo "FAIL: أخفق أحدُ اتّجاهَي التفرّع"
exit 1
