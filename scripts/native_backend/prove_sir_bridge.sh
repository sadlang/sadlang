#!/bin/bash
# (AR) برهانُ الجسر الحيّ: يشغّل ثنائيَّ ELF المُخفَّض من مصدر ص (دالة رئيسية() ارجع 40+2)
#      نيتيفيًّا على x86-64 ويؤكّد رمزَ الخروج ٤٢ — أوّل «ص → شيفرة آلة» تعمل بلا LLVM.
set -u
BIN="${1:?usage: prove_sir_bridge.sh <binary-path>}"
echo "=== disasm .text (offset 0x78) ==="
objdump -D -b binary -m i386:x86-64 -M intel --start-address=0x78 "$BIN" 2>/dev/null | grep -iE 'mov|add|sub|syscall' | head -10
chmod +x "$BIN" 2>/dev/null || true
"$BIN"
CODE=$?
echo "EXIT_CODE=$CODE"
[ "$CODE" -eq 42 ] && { echo "PASS: ثنائيّ ص السياديّ (مُخفَّض من SIR) خرج بـ٤٢ / sovereign S-lang binary lowered from SIR exited 42"; exit 0; } || { echo "FAIL: توقّعنا ٤٢ ووجدنا $CODE"; exit 1; }
