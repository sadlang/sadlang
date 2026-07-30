#!/bin/bash
# ============================================================================
# (AR) برهانُ «خروج ٤٢» الحيّ لعائلة fixed32 (AArch64) — يشغّل ثنائيَّ ELF64
#      الذي كتبه محرّكُ الخلفيّة السياديّة (بلا clang/lld/as) تحت qemu-aarch64
#      ويؤكّد رمزَ الخروج ٤٢. الوسيط: مسارُ الثنائيّ (sad_m3_arm64_exit42).
# (EN) Live exit-42 proof for the AArch64 (fixed32) family. Runs the sovereign
#      backend's ELF64 binary under qemu-aarch64-static and asserts exit code 42.
# ============================================================================
set -u
BIN="${1:?usage: prove_arm64_exit42.sh <binary-path>}"
QEMU="$(command -v qemu-aarch64-static || command -v qemu-aarch64 || true)"

if [ -z "$QEMU" ]; then
  echo "SKIP: qemu-aarch64 غير متوفّر / not available — البوّابة التفاضليّة (llvm-mc) هي برهان الصحّة"
  exit 3
fi

chmod +x "$BIN" 2>/dev/null || true
"$QEMU" "$BIN"
CODE=$?
echo "EXIT_CODE=$CODE"
if [ "$CODE" -eq 42 ]; then
  echo "PASS: ثنائيّ AArch64 السياديّ خرج بـ٤٢ تحت qemu / sovereign ARM64 binary exited 42"
  exit 0
else
  echo "FAIL: توقّعنا ٤٢ ووجدنا $CODE / expected 42, got $CODE"
  exit 1
fi
