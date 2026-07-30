#!/bin/bash
# ============================================================================
# (AR) برهانُ م١ الحيّ: يُشغّل ثنائيَّ ELF64 الساكن الذي أنتجه محرّكُ الخلفيّة
#      السياديّة (بلا clang/lld/as) ويؤكّد أنّه يخرج برمز ٤٢ على Linux.
#      يُنتَج الثنائيّ باختبار test_native_backend_m1 (TEST NativeElf.WritesFile)
#      باسم `sad_m1_exit42` في مجلّد التشغيل.
#
#      الاستعمال (على Linux، أو WSL مع تعطيل تحويل المسار في git-bash):
#          bash scripts/native_backend/prove_m1_exit42.sh <مسار_الثنائيّ>
#      مثال WSL من git-bash:
#          MSYS_NO_PATHCONV=1 wsl bash /mnt/c/.../prove_m1_exit42.sh /mnt/c/.../sad_m1_exit42
#
# (EN) m1 live proof: runs the static ELF64 produced by the sovereign backend
#      encoder (no clang/lld/as) and asserts it exits with code 42 on Linux.
# ============================================================================
set -u

BIN="${1:-./sad_m1_exit42}"
EXPECTED=42

if [ ! -f "$BIN" ]; then
    echo "خطأ: الثنائيّ غير موجود / binary not found: $BIN" >&2
    exit 2
fi

TMP="$(mktemp)"
cp "$BIN" "$TMP"
chmod +x "$TMP"
"$TMP"
CODE=$?
rm -f "$TMP"

echo "رمز الخروج / exit code = $CODE (المتوقَّع / expected $EXPECTED)"
if [ "$CODE" -eq "$EXPECTED" ]; then
    echo "PASS: الثنائيّ السياديّ خرج بـ$EXPECTED بلا أدواتٍ أجنبيّة"
    exit 0
else
    echo "FAIL: رمز خروجٍ غير متوقَّع" >&2
    exit 1
fi
