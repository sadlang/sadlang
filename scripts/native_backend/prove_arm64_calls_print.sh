#!/bin/bash
# ============================================================================
# (AR) برهانٌ حيٌّ لتكافؤ AArch64 الكامل (نداء + طباعة) — يشغّل ثنائيّاتِ ELF64 التي
#      كتبها محرّكُ الخلفيّة السياديّة (بلا clang/lld/as) تحت qemu-aarch64، ويؤكّد:
#      رمزَ الخروج للنداءات (٤٢)، ومخرَجَ stdout للطباعة («مرحبا\n» و«42\n»). الوسيط:
#      مجلّدُ الثنائيّات (build حيث نفّذ الاختبارُ فكتبها).
# (EN) Live proof of full AArch64 parity (calls + printing). Runs the sovereign
#      backend's ELF64 binaries under qemu-aarch64: asserts exit 42 for calls, and
#      exact stdout for prints. Arg: directory holding the test-written binaries.
# ============================================================================
set -u
DIR="${1:?usage: prove_arm64_calls_print.sh <binaries-dir>}"
QEMU="$(command -v qemu-aarch64-static || command -v qemu-aarch64 || true)"

if [ -z "$QEMU" ]; then
  echo "SKIP: qemu-aarch64 غير متوفّر / not available — البوّابة التفاضليّة (llvm-mc) هي برهان الصحّة"
  exit 3
fi

fails=0

check_exit() { # <file> <expected-code>
  local bin="$DIR/$1" want="$2"
  chmod +x "$bin" 2>/dev/null || true
  "$QEMU" "$bin"; local code=$?
  if [ "$code" -eq "$want" ]; then
    echo "PASS exit: $1 → $code"
  else
    echo "FAIL exit: $1 توقّعنا $want ووجدنا $code"; fails=$((fails+1))
  fi
}

check_stdout() { # <file> <expected-stdout>
  local bin="$DIR/$1" want="$2"
  chmod +x "$bin" 2>/dev/null || true
  local out; out="$("$QEMU" "$bin")"
  if [ "$out" = "$want" ]; then
    echo "PASS stdout: $1 → «$out»"
  else
    echo "FAIL stdout: $1 توقّعنا «$want» ووجدنا «$out»"; fails=$((fails+1))
  fi
}

# ── النداءات (رمزُ الخروج ٤٢) ──
check_exit "sad_arm64_call42"        42
check_exit "sad_arm64_nested42"      42
check_exit "sad_arm64_liveacross42"  42

# ── الطباعة (مخرَجُ stdout الحرفيّ) ──
check_stdout "sad_arm64_printstr"      "مرحبا"
check_stdout "sad_arm64_printnum"      "42"
check_stdout "sad_arm64_printcomputed" "42"

if [ "$fails" -eq 0 ]; then
  echo "ALL PASS: تكافؤُ AArch64 الكامل مُبرهَنٌ حيًّا (نداء + طباعة) بلا LLVM"
  exit 0
fi
echo "FAILURES=$fails"
exit 1
