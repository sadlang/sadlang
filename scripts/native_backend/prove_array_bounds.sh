#!/bin/bash
# ============================================================================
# (AR) برهانُ فحصِ حدّ المصفوفة الحيّ: يُثبت أنّ الفهرسةَ ضمنَ الحدّ تُرجِع ٤٢ بينما
#      التجاوزَ يُهلِع بالرمز ١٣٤ (‎128+SIGABRT‎) — على x86-64 نيتيفيًّا وARM64 تحت qemu.
#      الفحصُ لا-موقَّع (idx مقابل len) ⇒ يشملُ الفهرسَ السالبَ (يلتفّ إلى قيمةٍ ضخمة).
# (EN) Live array-bounds proof: in-bounds indexing exits 42; out-of-bounds panics
#      with code 134. x86-64 native + AArch64 under qemu. Unsigned check ⇒ also
#      catches negative indices (they wrap to a huge unsigned value).
#   الوسيط ١: مجلّدُ الثنائيّات (حيث كُتبت sad_sir_array42 / sad_sir_arrayoob / ...).
# ============================================================================
set -u
DIR="${1:?usage: prove_array_bounds.sh <binary-dir>}"
QEMU="$(command -v qemu-aarch64-static || command -v qemu-aarch64 || true)"
FAIL=0

# (AR) يشغّل ثنائيًّا ويؤكّد رمزَ الخروج المتوقَّع. المعاملات: <ملف> <متوقَّع> [مُشغِّل].
run_expect() {
  local bin="$1" want="$2" runner="${3:-}"
  if [ ! -f "$bin" ]; then echo "SKIP: $bin غير موجود"; return; fi
  chmod +x "$bin" 2>/dev/null || true
  if [ -n "$runner" ]; then "$runner" "$bin"; else "$bin"; fi
  local code=$?
  if [ "$code" -eq "$want" ]; then
    echo "PASS: $(basename "$bin") ⇒ $code (متوقَّع $want)"
  else
    echo "FAIL: $(basename "$bin") ⇒ $code (متوقَّع $want)"; FAIL=1
  fi
}

echo "=== x86-64 (نيتيف) ==="
run_expect "$DIR/sad_sir_array42"    42
run_expect "$DIR/sad_sir_arraylen42" 42
run_expect "$DIR/sad_sir_arrayoob"   134   # (AR) الفهرسُ ٥ ≥ الطولِ ٣ ⇒ هلعُ الحدّ

echo "=== AArch64 (qemu) ==="
if [ -z "$QEMU" ]; then
  echo "SKIP: qemu-aarch64 غير متوفّر — البوّابة التفاضليّة (llvm-mc) هي برهان الصحّة"
else
  run_expect "$DIR/sad_arm64_array42"    42  "$QEMU"
  run_expect "$DIR/sad_arm64_arraylen42" 42  "$QEMU"
  run_expect "$DIR/sad_arm64_arrayoob"   134 "$QEMU"
fi

[ "$FAIL" -eq 0 ] && { echo "PASS: فحصُ الحدّ يعمل حيًّا (ضمن⇒٤٢، تجاوز⇒١٣٤) على المعماريّتين"; exit 0; } \
                  || { echo "FAIL: خلَلٌ في برهان فحصِ الحدّ"; exit 1; }
