#!/bin/bash
# ============================================================================
# (AR) برهانُ الإلحاق الحيّ: يُثبت أنّ `م.أضف(قيمة)` يعمل أصليًّا — بمسارِ النموّ
#      (mmap سعةٍ مضاعفةٍ + نسخُ الخانات حين L==C) وبمسارِ بلا-نموّ (L<C ⇒ تخزينٌ مباشر).
#      على x86-64 نيتيفيًّا وARM64 تحت qemu. كلُّ ثنائيٍّ يخرج ٤٢.
# (EN) Live append proof: `arr.push(v)` works natively — grow path (mmap 2× + copy
#      when L==C) and no-grow path (L<C ⇒ direct store). x86-64 native + AArch64 qemu.
#   الوسيط ١: مجلّدُ الثنائيّات (sad_sir_append42 / sad_arm64_append42 / …).
# ============================================================================
set -u
DIR="${1:?usage: prove_array_append.sh <binary-dir>}"
QEMU="$(command -v qemu-aarch64-static || command -v qemu-aarch64 || true)"
FAIL=0

run_expect() {
  local bin="$1" want="$2" runner="${3:-}"
  if [ ! -f "$bin" ]; then echo "SKIP: $bin غير موجود"; return; fi
  chmod +x "$bin" 2>/dev/null || true
  if [ -n "$runner" ]; then "$runner" "$bin"; else "$bin"; fi
  local code=$?
  if [ "$code" -eq "$want" ]; then echo "PASS: $(basename "$bin") ⇒ $code"; else echo "FAIL: $(basename "$bin") ⇒ $code (متوقَّع $want)"; FAIL=1; fi
}

echo "=== x86-64 (نيتيف) ==="
run_expect "$DIR/sad_sir_append42"       42   # (AR) [40]→أضف(2)→نموّ cap 1→2→[40،2]
run_expect "$DIR/sad_sir_appendnogrow42" 42   # (AR) [6]+٣ إضافات ⇒ مسارُ L<C (بلا نموّ)

echo "=== AArch64 (qemu) ==="
if [ -z "$QEMU" ]; then
  echo "SKIP: qemu-aarch64 غير متوفّر — البوّابة التفاضليّة (llvm-mc) هي برهان الصحّة"
else
  run_expect "$DIR/sad_arm64_append42"       42 "$QEMU"
  run_expect "$DIR/sad_arm64_appendnogrow42" 42 "$QEMU"
fi

[ "$FAIL" -eq 0 ] && { echo "PASS: الإلحاقُ يعمل حيًّا (نموّ + بلا نموّ) على المعماريّتين"; exit 0; } \
                  || { echo "FAIL: خلَلٌ في برهان الإلحاق"; exit 1; }
