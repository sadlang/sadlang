#!/bin/bash
# (AR) برهانُ استدعاء الدوالّ الحيّ: يشغّل ثنائيَّ ELF مُخفَّضًا من برنامجٍ بدالّتين
#      («اجمع(أ،ب)=أ+ب» و«رئيسية=اجمع(40،2)») ويؤكّد الخروجَ ٤٢ — يُثبت call/ret
#      حقيقيّ + تمرير الوسائط في rdi/rsi (SysV) + الإرجاع في rax. بلا clang/lld/as.
set -u
BIN="${1:?usage: prove_sir_call.sh <call-binary>}"
echo "=== disasm .text (offset 0x78) ==="
objdump -D -b binary -m i386:x86-64 -M intel --start-address=0x78 "$BIN" 2>/dev/null \
    | grep -iE 'push|mov|add|call|ret|syscall|rbp' | head -28
chmod +x "$BIN" 2>/dev/null || true
"$BIN"
CODE=$?
echo "EXIT_CODE=$CODE"
[ "$CODE" -eq 42 ] && { echo "PASS: نداءُ دالّةٍ ص السياديّ (اجمع(40،2) عبر SysV) خرج ٤٢ / sovereign S-lang function call exited 42"; exit 0; } || { echo "FAIL: توقّعنا ٤٢ ووجدنا $CODE"; exit 1; }
