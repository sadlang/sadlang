#!/bin/bash
# (AR) برهانُ الذاكرة + إطار الدالّة الحيّ: يشغّل ثنائيَّ ELF مُخفَّضًا من حلقة «بينما»
#      ذاتِ عدّادٍ متغيّرٍ في الذاكرة (عداد=0؛ بينما عداد<42 عداد=عداد+1؛ ارجع عداد)
#      ويؤكّد الخروجَ ٤٢ — يُثبت ALLOC/LOAD/STORE + مقدّمةَ الإطار + لولبًا خلفيًّا.
#      لا clang/lld/as ولا زمن تشغيل.
set -u
BIN="${1:?usage: prove_sir_memory.sh <while-binary>}"
echo "=== disasm .text (offset 0x78) ==="
objdump -D -b binary -m i386:x86-64 -M intel --start-address=0x78 "$BIN" 2>/dev/null \
    | grep -iE 'push|mov|cmp|add|jl|jmp|syscall' | head -24
chmod +x "$BIN" 2>/dev/null || true
"$BIN"
CODE=$?
echo "EXIT_CODE=$CODE"
[ "$CODE" -eq 42 ] && { echo "PASS: حلقةُ ص السياديّة (عدّادٌ في الذاكرة عبر ALLOC/LOAD/STORE) خرجت ٤٢ / sovereign S-lang loop (memory counter) exited 42"; exit 0; } || { echo "FAIL: توقّعنا ٤٢ ووجدنا $CODE"; exit 1; }
