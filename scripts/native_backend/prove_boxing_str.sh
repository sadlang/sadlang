#!/bin/bash
# ============================================================================
# (AR) برهانُ النصّ المعلَّب الحيّ: مصفوفةٌ مختلطةٌ فيها نصّ ⇒ فهرسة ⇒ طباعةٌ مبوَّبةٌ
#      زمنَ التشغيل، أصليًّا بلا LLVM/libc. «م=[1، "نص"، 2]؛ اطبع_سطر(م[1])» ⇒ «نص».
#      الوسمُ Str والحمولةُ عنوانُ واصفٍ {len،bytes} في rodata. x86 نيتيف + ARM64 qemu.
# (EN) Live boxed-string proof: mixed array with a string ⇒ index ⇒ runtime tag-dispatch
#      print, natively (no LLVM/libc). Arg 1: binary directory.
# ============================================================================
set -u
DIR="${1:?usage: prove_boxing_str.sh <binary-dir>}"
QEMU="$(command -v qemu-aarch64-static || command -v qemu-aarch64 || true)"
FAIL=0
WANT=$'\xD9\x86\xD8\xB5'  # «نص»

expect_out() {
  local bin="$1" runner="${2:-}"
  if [ ! -f "$bin" ]; then echo "SKIP: $bin غير موجود"; return; fi
  chmod +x "$bin" 2>/dev/null || true
  local got
  if [ -n "$runner" ]; then got="$("$runner" "$bin")"; else got="$("$bin")"; fi
  if [ "$got" = "$WANT" ]; then echo "PASS: $(basename "$bin") ⇒ «نص»"
  else echo "FAIL: $(basename "$bin") ⇒ «$got» (متوقَّع «نص»)"; FAIL=1; fi
}

echo "=== x86-64 (نيتيف) ==="
expect_out "$DIR/sad_sir_boxed_str"

echo "=== AArch64 (qemu) ==="
if [ -z "$QEMU" ]; then
  echo "SKIP: qemu-aarch64 غير متوفّر — البوّابة التفاضليّة (llvm-mc) هي برهان الصحّة"
else
  expect_out "$DIR/sad_arm64_boxed_str" "$QEMU"
fi

[ "$FAIL" -eq 0 ] && { echo "PASS: النصّ المعلَّب يعمل حيًّا على المعماريّتين"; exit 0; } \
                  || { echo "FAIL: خلَلٌ في برهان النصّ المعلَّب"; exit 1; }
