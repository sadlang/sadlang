#!/bin/bash
# (AR) برهانُ تدفّق التحكّم الحيّ: يشغّل ثنائيَّ اللولب (يعدّ حتّى ٤٢) نيتيفيًّا على
#      x86-64 (بلا qemu) ويؤكّد رمزَ الخروج ٤٢ — إثباتُ صحّة ترميز add/cmp/jne النسبيّ.
set -u
BIN="${1:?usage: prove_x86_loop42.sh <binary-path>}"
echo "=== disasm .text (offset 0x78) ==="
objdump -D -b binary -m i386:x86-64 -M intel --start-address=0x78 "$BIN" 2>/dev/null | grep -iE 'mov|add|cmp|jne|syscall' | head -8
chmod +x "$BIN" 2>/dev/null || true
"$BIN"
CODE=$?
echo "EXIT_CODE=$CODE"
[ "$CODE" -eq 42 ] && { echo "PASS: لولبُ التحكّم السياديّ خرج بـ٤٢ نيتيفيًّا / control-flow loop exited 42"; exit 0; } || { echo "FAIL: توقّعنا ٤٢ ووجدنا $CODE"; exit 1; }
