#!/bin/bash
# ============================================================================
# (AR) برهانُ م٦ الحيّ: تصريحُ المتغيّرِ وطباعتُه على **RISC-V RV64** بلا LLVM.
#
#      لماذا وُجد هذا الملفّ: كان `language-truth/backend/targets.yaml` يرفع حالةَ
#      riscv64 من `planned` إلى `lowered` مستندًا إلى نصٍّ يقول «نُفِّذ تحت
#      qemu-riscv64-static وطابق المفسّرَ في تسعِ حالاتٍ حدّيّة» — وكان ذلك التشغيلُ
#      قد جرى فعلًا، لكنّه جرى في **طرفيّةٍ** لا في الشجرة. فمسحُ المستودعِ عن
#      «qemu-riscv64» لم يكن يجد إلّا سطرَ الادّعاءِ نفسَه: لا سكربتَ ولا وظيفةَ CI
#      ولا هدفَ CTest. وبرهانٌ لا يُعيد أحدٌ إنتاجَه دعوى، مهما صدق قائلُها.
#      (كشفته مراجعةُ أميليا الخصميّة، 2026-08-13، بندُ ع-٣.)
#
#      ما يُقاس هنا — إحدى عشرةَ حالةً عدديّةً حدّيّة + المنطقيُّ + النصّ:
#        • ٠ (الحلقةُ «افعل ثمّ افحص» تطبعه بلا حالةٍ خاصّة)
#        • موجبٌ صغير · سالب
#        • ٢٠٤٧ و٢٠٤٨ — طرفا الفوريِّ ذي الاثنَي عشرَ بتًّا (حدُّ ADDI/SD)
#        • ٣٠٠٠٠٠٠٠٠٠ — يتجاوز ٣٢ بتًّا فيلزمه بناءُ الثابتِ بالشرائح
#        • حدّا i64 كلاهما فعلًا — ومنهما −2^63، الحالةُ التي لا مقدارَ موجبًا لها.
#          (⚠️ كان هذا السطرُ يدّعي «كلاهما» والمصفوفةُ تحوي −2^63+1 وحدَه — دعوى
#           سبقت القياس، كشفتها مراجعةُ أميليا ع-٣؛ فأُدخلت الحالةُ لا الادّعاء.)
#        • طبيعي64: 2^64−1 و2^63 — المسارُ اللا-موقَّع. كان يطبع «‑1» صامتًا (ع-١)
#        • منطقيّ: «صحيح»/«خطأ» لا «1»/«0» (ع-٦ في المراجعةِ الأولى)
#        • نصٌّ حرفيّ: يمرُّ بترقيعِ عنوانِ rodata بـLUI+ADDI
#
#      المتوقَّعُ أدناه ليس مكتوبًا بالحدس: هو مخرَجُ **المفسّرِ** لكلِّ حالة، وهو
#      عقدُ التكافؤِ المزدوج نفسُه الذي تقيسه مصفوفةُ السلوك.
#
# (EN) Live m6 proof: variable declaration + print on RISC-V RV64, no LLVM.
#      Exists because targets.yaml claimed a QEMU run that lived only in a
#      terminal — an unreproducible proof is a claim. Expected values are the
#      interpreter's own output (the dual-execution parity contract).
#
#      الاستعمال / usage:
#        prove_riscv64_decl_parity.sh <sad-build-path> [workdir]
# ============================================================================
set -u
BUILD="${1:?usage: prove_riscv64_decl_parity.sh <sad-build-path> [workdir]}"
WORK="${2:-$(mktemp -d)}"
FAIL=0
RAN=0

# (AR) مجلّدٌ نظيفٌ لكلّ تشغيلة — وثنائيُّ تشغيلةٍ ماضيةٍ باقٍ يُخضِرُّ البرهانَ عن
#      مُصرِّفٍ لم يعمل. ولا يُمحى إلّا ما يحمل بصمةَ هذا السكربت أو كان فارغًا:
#      الوسيطُ ٢ مسارٌ حرٌّ يكتبه إنسان، وخطأٌ مطبعيٌّ مع `rm -rf` أعمى يبتلع مجلّدًا.
STAMP_NAME=".prove_riscv64_decl_parity"
if [ -e "$WORK" ] && [ ! -d "$WORK" ]; then
  echo "FAIL: «$WORK» موجودٌ وليس مجلّدًا — لن يُمحى"
  exit 1
fi
if [ -d "$WORK" ] && [ ! -e "$WORK/$STAMP_NAME" ] && [ -n "$(ls -A "$WORK" 2>/dev/null)" ]; then
  echo "FAIL: «$WORK» ليس مجلّدَ عملٍ لهذا السكربت (لا بصمة) وليس فارغًا — لن يُمحى"
  exit 1
fi
rm -rf "$WORK"
mkdir -p "$WORK"
: > "$WORK/$STAMP_NAME"

# (AR) على ويندوز (Git Bash) يُترجَم بـsad-build.exe محلّيًّا ويُنفَّذ ثنائيُّ لينكس عبر WSL.
WSLRUN=""
case "$(uname -s)" in MINGW*|MSYS*|CYGWIN*) WSLRUN="1";; esac
to_linux_path() {
  if [ -n "$WSLRUN" ]; then
    printf '/mnt/%s' "$(cd "$(dirname "$1")" && pwd -W 2>/dev/null | tr 'A-Z' 'a-z' | tr -d ':')/$(basename "$1")"
  else printf '%s' "$1"; fi
}
run_bin() { # <ثنائيّ> <مُشغِّل>
  local bin="$1" runner="$2"
  if [ -n "$WSLRUN" ]; then
    # (AR) مقصدٌ متفرّدٌ لكلّ ثنائيّ: مسارٌ مشترَكٌ يعني أنّ إخفاقَ النسخِ يُنفّذ
    #      ثنائيَّ الحالةِ السابقة فيُقاس غيرُ المقصود.
    local tgt="/tmp/_rv_$(basename "$bin")"
    wsl.exe -e bash -c "rm -f '$tgt' && cp '$(to_linux_path "$bin")' '$tgt' && chmod +x '$tgt' && $runner '$tgt'" 2>/dev/null
  else
    chmod +x "$bin" 2>/dev/null || true
    "$runner" "$bin" 2>/dev/null
  fi
}

# (AR) qemu قد يكون داخل WSL لا على ويندوز ⇒ ابحث عنه حيث سيُنفَّذ الثنائيّ.
if [ -n "$WSLRUN" ]; then
  QEMU="$(wsl.exe -e bash -c 'command -v qemu-riscv64-static || command -v qemu-riscv64' 2>/dev/null | tr -d '\r')"
else
  QEMU="$(command -v qemu-riscv64-static || command -v qemu-riscv64 || true)"
fi
if [ -z "$QEMU" ]; then
  # (AR) التخطّي إخفاقٌ افتراضيًّا: مجموعُ أصفارٍ بلا مُشغِّلٍ يُقرأ «نجح الكلّ».
  echo "SKIP: لا qemu-riscv64 — لا برهانَ حيًّا على RV64"
  [ "${SAD_PROOFS_ALLOW_SKIP:-0}" = "1" ] && exit 0
  exit 1
fi

# (AR) الحالات: «اسم|المتوقَّع|القيمةُ المُصرَّحة». الشكلُ واحدٌ عمدًا — تصريحٌ ثمّ
#      طباعةٌ — لأنّ المقيسَ قاعدةُ `gr.decl.variable` على هذا الهدف لا اللغةُ كلُّها.
# (AR) الحقلُ الثالثُ **نصُّ التصريحِ كاملًا** لا القيمةَ وحدَها، كي تدخل الحالاتُ
#      المكتوبةُ النوعِ (طبيعي64) المصفوفةَ نفسَها بلا مسارٍ ثانٍ يُنسى تحديثُه.
# (AR) ⚠️ وكانت حالتا `طبيعي64` مكتوبتَين بالصيغةِ اللاحقةِ «متغير س: طبيعي64» —
#      وهي **منفَّذةٌ في المحلِّل وغيرُ منصوصةٍ في `gr.decl.variable`**: بحثُ COLON في
#      `language-truth/grammar/20_declarations.yaml` يُخرِج صفرًا، وEBNF يضع النوعَ
#      قبلَ الاسمِ بلا نقطتين. فكان البرهانُ يقيس بابًا خارجَ المواصفة. حُوِّلتا إلى
#      الصيغةِ المنصوصةِ «متغير طبيعي64 س»، والدلالةُ واحدةٌ مقيسةً (‑1 ⇒ 2^64−1 في
#      الصيغتين). والدرسُ أنّ ما يقبله المحلِّلُ ليس ما تَعِدُ به اللغة.
CASES="
zero|0|متغير س = 0
small|5|متغير س = 5
neg|-7|متغير س = -7
max12|2047|متغير س = 2047
over12|2048|متغير س = 2048
big32|3000000000|متغير س = 3000000000
i64max|9223372036854775807|متغير س = 9223372036854775807
i64min|-9223372036854775808|متغير س = -9223372036854775808
i64min_plus1|-9223372036854775807|متغير س = -9223372036854775807
uint64_max|18446744073709551615|متغير طبيعي64 س = 18446744073709551615
uint64_high|9223372036854775808|متغير طبيعي64 س = 9223372036854775808
bool_true|صحيح|متغير س = صحيح
bool_false|خطأ|متغير س = خطأ
"

echo "── برهانُ م٦: تصريحُ المتغيّرِ على RISC-V RV64 (qemu: $QEMU) ──"
OLDIFS="$IFS"
IFS='
'
# (AR) وكلُّ حالةٍ تُقاس عند **مستويَي تحسين**: -O0 و-O2. ليس تزيُّدًا — بل لأنّ
#      غيابَ هذا البُعدِ أخفى عيبًا حقيقيًّا: `متغير س = 2048` يُبنى معه `%0 = move`
#      ميّتٌ يحذفه DCE، فكان RV64 يمرُّ في -O2 (الافتراضيّ) ويُخفِق في -O0 بـ«أوپكود
#      ٣٩ غير مدعوم» — أي أنّ «lowered» كانت مشروطةً بتشغيلِ المُحسِّن دون أن يقول
#      ذلك أحد. x86_64 وarm64 يمرّان في المستويَين. كشفه اختبارُ الجسرِ الوحدويّ
#      (يبني SIR بلا مُحسِّن) لا هذا البرهانُ الحيّ — ولذا أُضيف البُعدُ هنا.
for line in $CASES; do
  [ -z "$line" ] && continue
  IFS='|' read -r name want decl <<EOF
$line
EOF
  IFS="$OLDIFS"
  for opt in -O0 -O2; do
    src="$WORK/rv_${name}${opt}.ص"
    elf="$WORK/rv_${name}${opt}.elf"
    printf '\xef\xbb\xbf' > "$src"
    printf '%s\nاطبع_سطر(س)\n' "$decl" >> "$src"

    if ! "$BUILD" "$src" --خلفية-أصلية --هدف=riscv64-unknown-linux-elf "$opt" -o "$elf" >/dev/null 2>&1 \
       || [ ! -s "$elf" ]; then
      echo "FAIL بناء: $name $opt — لم يُنتَج ثنائيّ"
      FAIL=$((FAIL+1)); RAN=$((RAN+1))
      continue
    fi

    got="$(run_bin "$elf" "$QEMU" | tr -d '\r')"
    RAN=$((RAN+1))
    if [ "$got" = "$want" ]; then
      echo "PASS $name $opt → «$got»"
    else
      echo "FAIL $name $opt — توقّعنا «$want» ووجدنا «$got»"
      FAIL=$((FAIL+1))
    fi
  done
  IFS='
'
done
IFS="$OLDIFS"

# (AR) سعةُ الإطار: خانةُ ٨ بايت لكلِّ ALLOC/MOVE/NEG، والإزاحاتُ فوريٌّ ١٢-بت
#      موقَّعٌ ⇒ سقفٌ عند ٢٠٤٧ بايتًا. وفي -O0 ينجو MOVE من DCE فتُستهلَك خانتان
#      لكلِّ تصريح. هذه الحالةُ تُثبّت **المدى المضمون** (١٠٠ تصريحًا في -O0) كي لا
#      يُقرأ الحدُّ الذي وُثِّق في targets.yaml ادّعاءً. وما فوقَه يُرفَض صراحةً
#      (kFrameTooLarge) لا يُبتَر صامتًا. (ع-٥ في مراجعةِ أميليا.)
src="$WORK/rv_manyvars.ص"
elf="$WORK/rv_manyvars.elf"
printf '\xef\xbb\xbf' > "$src"
i=1
while [ "$i" -le 100 ]; do
  printf 'متغير م%s = %s\n' "$i" "$i" >> "$src"
  i=$((i+1))
done
printf 'اطبع_سطر(م100)\n' >> "$src"
if "$BUILD" "$src" --خلفية-أصلية --هدف=riscv64-unknown-linux-elf -O0 -o "$elf" >/dev/null 2>&1 && [ -s "$elf" ]; then
  got="$(run_bin "$elf" "$QEMU" | tr -d '\r')"
  RAN=$((RAN+1))
  if [ "$got" = "100" ]; then echo "PASS manyvars -O0 (١٠٠ تصريحًا) → «$got»"
  else echo "FAIL manyvars -O0 — توقّعنا «100» ووجدنا «$got»"; FAIL=$((FAIL+1)); fi
else
  echo "FAIL بناء: manyvars -O0 — المدى المضمون (١٠٠ تصريحًا) لم يعد يُبنى"
  FAIL=$((FAIL+1)); RAN=$((RAN+1))
fi

# (AR) نصٌّ حرفيّ: مسارٌ آخرُ تمامًا (rodata + ترقيعُ LUI+ADDI) لا يمرُّ بـitoa.
src="$WORK/rv_literal.ص"
elf="$WORK/rv_literal.elf"
printf '\xef\xbb\xbf' > "$src"
printf 'اطبع_سطر("مرحبا")\n' >> "$src"
if "$BUILD" "$src" --خلفية-أصلية --هدف=riscv64-unknown-linux-elf -o "$elf" >/dev/null 2>&1 && [ -s "$elf" ]; then
  got="$(run_bin "$elf" "$QEMU" | tr -d '\r')"
  RAN=$((RAN+1))
  if [ "$got" = "مرحبا" ]; then echo "PASS literal → «$got»"
  else echo "FAIL literal — توقّعنا «مرحبا» ووجدنا «$got»"; FAIL=$((FAIL+1)); fi
else
  echo "FAIL بناء: literal — لم يُنتَج ثنائيّ"; FAIL=$((FAIL+1)); RAN=$((RAN+1))
fi

# (AR) وحارسٌ سالب: ما هو خارجَ نطاقِ هذا المخفّضِ يجب أن **يُخفِق صراحةً** لا أن
#      يُنتج ثنائيًّا مبتورًا. الجمعُ ليس من الخمسةِ المدعومة ⇒ رفضٌ مُعلَن.
#      ⚠️ وكانت الصياغةُ الأولى لهذا الحارس `متغير س = 1 + 2` فأخفق الحارسُ نفسُه:
#      المُصرِّفُ **قَبِلها وطبع 3**، لأنّ الواجهةَ تطوي الثابتَين قبل SIR فلا يبلغ
#      المخفّضَ أوپكودُ جمعٍ أصلًا. أي أنّ ذلك الحارسَ كان يقيس طاويَ الثوابتِ لا
#      رفضَ المخفّض. فليكن المعاملان **متغيّرَين** كي يبقى الجمعُ إلى زمنِ التخفيض.
src="$WORK/rv_unsupported.ص"
printf '\xef\xbb\xbf' > "$src"
printf 'متغير أ = 1\nمتغير ب = 2\nمتغير ج = أ + ب\nاطبع_سطر(ج)\n' >> "$src"
# (AR) ⚠️ والمعيارُ **نصُّ التشخيصِ لا مجرّدُ رمزِ خروجٍ غيرِ صفريّ**: «أخفق البناء»
#      يخضرُّ على أيِّ سببٍ كان — خطأٍ نحويٍّ، أو ملفٍّ مفقود، أو تشديدٍ لاحقٍ في
#      المُحلِّل. قِيس: مصدرٌ فيه هراءٌ نحويٌّ محضٌ كان يُقرأ PASS في هذا الحارس.
#      فلْيُلزَم أن يكون الرافضُ **هذا المخفّضَ** بذكرِ الأوپكود.
guard_out="$("$BUILD" "$src" --خلفية-أصلية --هدف=riscv64-unknown-linux-elf -o "$WORK/rv_unsupported.elf" 2>&1)"
guard_rc=$?
if [ "$guard_rc" -eq 0 ]; then
  echo "FAIL حارسٌ سالب: قُبِل ما هو خارجَ نطاقِ المخفّض بلا تشخيص"
  FAIL=$((FAIL+1))
elif ! printf '%s' "$guard_out" | grep -q "riscv64 opcode "; then
  echo "FAIL حارسٌ سالب: رُفض لسببٍ آخرَ لا لقصورِ المخفّض — لا ذكرَ لـ«riscv64 opcode»"
  FAIL=$((FAIL+1))
else
  echo "PASS حارسٌ سالب: مخفّضُ RV64 هو الرافضُ صراحةً (riscv64 opcode)"
fi
RAN=$((RAN+1))

echo "── المقيس: $RAN · الإخفاقات: $FAIL ──"
if [ "$FAIL" -eq 0 ]; then
  echo "ALL PASS: قاعدةُ تصريحِ المتغيّر مُبرهَنةٌ حيًّا على RV64 بلا LLVM"
  exit 0
fi
exit 1
