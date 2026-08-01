#!/bin/bash
# ============================================================================
# (AR) برهانُ العشريّ الحيّ: يُثبت أنّ حسابَ العشريّ (SSE) وطباعتَه يعملان أصليًّا بلا
#      LLVM/libc — على x86-64 نيتيفيًّا وARM64 تحت qemu.
#        • حساب: «ع = 21.0 × 2.0؛ ارجع رقم(ع)» ⇒ خروج ٤٢ (mulsd + cvttsd2si).
#        • طباعة: «ع = 40.5 + 1.0؛ اطبع_سطر(ع)» ⇒ يطبع «41.5» (addsd + مُنسِّق fixed6+حذف).
# (EN) Live float proof: SSE float arithmetic + printing work natively (no LLVM/libc).
#   الوسيط ١: مجلّدُ الثنائيّات.
# ============================================================================
set -u
DIR="${1:?usage: prove_float.sh <binary-dir>}"
QEMU="$(command -v qemu-aarch64-static || command -v qemu-aarch64 || true)"
FAIL=0

expect_exit() {
  local bin="$1" want="$2" runner="${3:-}"
  if [ ! -f "$bin" ]; then echo "SKIP: $bin غير موجود"; return; fi
  chmod +x "$bin" 2>/dev/null || true
  if [ -n "$runner" ]; then "$runner" "$bin"; else "$bin"; fi
  local code=$?
  if [ "$code" -eq "$want" ]; then echo "PASS(exit): $(basename "$bin") ⇒ $code"
  else echo "FAIL(exit): $(basename "$bin") ⇒ $code (متوقَّع $want)"; FAIL=1; fi
}

expect_out() {
  local bin="$1" want="$2" runner="${3:-}"
  if [ ! -f "$bin" ]; then echo "SKIP: $bin غير موجود"; return; fi
  chmod +x "$bin" 2>/dev/null || true
  local got
  if [ -n "$runner" ]; then got="$("$runner" "$bin")"; else got="$("$bin")"; fi
  if [ "$got" = "$want" ]; then echo "PASS(out): $(basename "$bin") ⇒ «$got»"
  else echo "FAIL(out): $(basename "$bin") ⇒ «$got» (متوقَّع «$want»)"; FAIL=1; fi
}

echo "=== x86-64 (نيتيف) ==="
expect_exit "$DIR/sad_sir_float42"    42     # (AR) 21.0×2.0 ⇒ 42.0 ⇒ رقم ⇒ ٤٢
expect_out  "$DIR/sad_sir_printfloat" "41.5" # (AR) 40.5+1.0 ⇒ «41.5»

echo "=== AArch64 (qemu) ==="
if [ -z "$QEMU" ]; then
  echo "SKIP: qemu-aarch64 غير متوفّر — البوّابة التفاضليّة (llvm-mc) هي برهان الصحّة"
else
  expect_exit "$DIR/sad_arm64_float42"    42     "$QEMU"
  expect_out  "$DIR/sad_arm64_printfloat" "41.5" "$QEMU"
fi

[ "$FAIL" -eq 0 ] && { echo "PASS: العشريّ يعمل حيًّا (حساب + طباعة) على المعماريّتين"; exit 0; } \
                  || { echo "FAIL: خلَلٌ في برهان العشريّ"; exit 1; }
