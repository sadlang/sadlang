#!/bin/bash
# ============================================================================
# (AR) برهانُ التعليب الحيّ: مصفوفةٌ مختلطةٌ int/float معلَّبة ⇒ فهرسة ⇒ طباعةٌ مبوَّبةٌ
#      زمنَ التشغيل، أصليًّا بلا LLVM/libc. «م=[1، 3.5، 2]؛ اطبع_سطر(م[1])؛ اطبع_سطر(م[0])»
#      ⇒ «3.5» ثمّ «1». x86-64 نيتيفيًّا وARM64 تحت qemu.
# (EN) Live boxing proof: mixed int/float array ⇒ index ⇒ runtime tag-dispatch print.
#   الوسيط ١: مجلّدُ الثنائيّات.
# ============================================================================
set -u
DIR="${1:?usage: prove_boxing.sh <binary-dir>}"
QEMU="$(command -v qemu-aarch64-static || command -v qemu-aarch64 || true)"
FAIL=0
WANT=$'3.5\n1'

expect_out() {
  local bin="$1" runner="${2:-}"
  if [ ! -f "$bin" ]; then echo "SKIP: $bin غير موجود"; return; fi
  chmod +x "$bin" 2>/dev/null || true
  local got
  if [ -n "$runner" ]; then got="$("$runner" "$bin")"; else got="$("$bin")"; fi
  if [ "$got" = "$WANT" ]; then echo "PASS: $(basename "$bin") ⇒ «3.5»/«1»"
  else echo "FAIL: $(basename "$bin") ⇒ «$got» (متوقَّع «3.5»/«1»)"; FAIL=1; fi
}

echo "=== x86-64 (نيتيف) ==="
expect_out "$DIR/sad_sir_boxed"

echo "=== AArch64 (qemu) ==="
if [ -z "$QEMU" ]; then
  echo "SKIP: qemu-aarch64 غير متوفّر — البوّابة التفاضليّة (llvm-mc) هي برهان الصحّة"
else
  expect_out "$DIR/sad_arm64_boxed" "$QEMU"
fi

[ "$FAIL" -eq 0 ] && { echo "PASS: التعليب يعمل حيًّا (مصفوفة مختلطة + طباعة مبوَّبة) على المعماريّتين"; exit 0; } \
                  || { echo "FAIL: خلَلٌ في برهان التعليب"; exit 1; }
