#!/bin/bash
# ============================================================================
# (AR) **المُنادي المفقود** لبراهينِ الخلفيّةِ الأصليّة.
#
#      كان في المستودعِ سبعةَ عشرَ سكربتَ برهانٍ حيٍّ (prove_*.sh) **بلا مُنادٍ واحد**:
#      لا وظيفةَ CI تشغّلها، ولا مسارَ عدّاءٍ يمرّرُ `--خلفية-أصلية`. فكلُّ ما تحرسه
#      كان محروسًا في طرفيّةِ إنسانٍ يومَ كُتِب فحسب — ولذلك عاشت في الخلفيّةِ عيوبٌ
#      حيّةٌ (نصفُ فكِّ علبةِ Any في الفرعِ المُدمَج، ولا فكَّ أصلًا في AArch64) إلى أن
#      كُشفت يدويًّا بعد شهور. هذا الملفُّ يُغلق تلك الفجوةَ البنيويّة.
#
#      أربعةُ حرّاسٍ يفعلها ولا يفعلها تشغيلٌ يدويّ:
#        ① **الإنتاجُ ثمّ البرهان**: يشغّل مُنتِجاتِ الثنائيّات (اختباراتُ gtest الثلاثة
#           التي تكتب ELF إلى مجلّدِ عملها) داخلَ مجلّدٍ **يُمحى أوّلًا** ثمّ يبرهن عليه.
#           والمحوُ ليس تجميلًا: مُنتِجٌ ينهار قبل الكتابة يترك ثنائيَّ التشغيلةِ الماضية
#           فيُبرهَن عليه ويخضرّ — البقيّةُ أخطرُ من الغياب، لأنّ الغيابَ يُرى.
#           ويُحكَم برمزِ خروجِ المُنتِج أيضًا: مُنتِجٌ يُخفِق تأكيدًا **ومع ذلك يكتب**
#           الثنائيّاتِ (وهو ما حدث فعلًا حين انجرفت تسميةُ «فوق»⇒«أعلى») يجعل كلَّ
#           البراهينِ اللاحقةِ خضراءَ عن مُصدِرٍ مكسور.
#        ② **حارسُ التغطية**: يقارن قائمةَ السكربتات المُصرَّحَ بها هنا بما في المجلّد
#           فعلًا. سكربتُ برهانٍ جديدٌ لا يُصرَّح به يُخفِق البوّابةَ — فلا تعود الفجوةُ
#           نفسُها (سكربتٌ بلا مُنادٍ) بالتسلّل.
#        ③ **التخطّي إخفاقٌ افتراضيًّا**: كلُّ سكربتٍ من هؤلاء يطبع SKIP ويخرج ٠ حين
#           يغيب الثنائيُّ أو qemu. مجموعُ أصفارٍ كهذه يُقرأ «نجح الكلّ» وهو أخضرُ بلا
#           قياس. فالتخطّي هنا إخفاقٌ ما لم يُطلَب خلافُه صراحةً (SAD_PROOFS_ALLOW_SKIP=1).
#        ④ **غيابُ qemu إخفاق**: نصفُ البراهينِ على AArch64، وغيابُ المُشغِّل يحذفها
#           من الحساب بلا أثرٍ في المخرَج ⇒ «نجح الكلّ» عن معماريّةٍ واحدة.
#
# (EN) The missing caller for the native-backend live proofs: wipes an isolated
#      workdir, produces the ELF fixtures (judging producer exit codes), runs every
#      prove_*.sh against them, fails on any undeclared script (coverage guard), and
#      treats SKIP — including a missing qemu — as failure by default.
#
#      الاستعمال / usage:
#        run_native_proofs.sh --مجلد-العمل <dir> [--منتج <tests-bin-dir>]
#                             [--مصرف <sad-build> | --بلا-مصرف]
#      البيئة / env:
#        SAD_PROOFS_ALLOW_SKIP=1   يقبل التخطّي (qemu غائبٌ مثلًا) بلا إخفاق
# ============================================================================
set -u

HERE="$(cd "$(dirname "$0")" && pwd)"
WORK=""
PRODUCER=""
SADBUILD=""
NO_COMPILER=0

while [ $# -gt 0 ]; do
  case "$1" in
    --مجلد-العمل|--workdir)  WORK="${2:?}"; shift 2 ;;
    --منتج|--producer)       PRODUCER="${2:?}"; shift 2 ;;
    --مصرف|--compiler)       SADBUILD="${2:?}"; shift 2 ;;
    # (AR) إقرارٌ صريحٌ بغيابِ المصرّف: وظيفةُ CI تتحمّل إخفاقَ بناءِ sad-build عمدًا
    #      منذ إنشائها، فترقيتُه هنا إلى قفلِ دمجٍ قرارُ سياسةٍ منفصل. والإقرارُ
    #      **مُعلَنٌ** في المخرَج ولا يُحسَب تغطيةً — لا يمرّ صامتًا.
    --بلا-مصرف|--no-compiler) NO_COMPILER=1; shift ;;
    *) echo "وسيطٌ مجهول: $1" >&2; exit 2 ;;
  esac
done
[ -n "$WORK" ] || { echo "usage: run_native_proofs.sh --مجلد-العمل <dir> [--منتج <tests-bin-dir>] [--مصرف <sad-build> | --بلا-مصرف]" >&2; exit 2; }

ALLOW_SKIP="${SAD_PROOFS_ALLOW_SKIP:-0}"
DECLARED=""
FAILED=""
PASSED=0
SKIPPED=""

# ── ① مجلّدُ عملٍ نظيفٌ لكلّ تشغيلة ────────────────────────────────────────────
#    (AR) المسارُ يكتبه إنسانٌ أو وظيفةُ CI، فخطأٌ مطبعيٌّ واحدٌ مع `rm -rf` أعمى
#         يبتلع مجلّدًا حقيقيًّا. لا يُمحى إلّا ما يحمل بصمةَ هذا المُنادي أو كان
#         فارغًا، وما ليس مجلّدًا يُرفَض أوّلًا (شرطُ البصمةِ وحدَه يسقط على مسارِ ملفّ).
STAMP=".run_native_proofs"
if [ -e "$WORK" ] && [ ! -d "$WORK" ]; then
  echo "FAIL: «$WORK» موجودٌ وليس مجلّدًا — لن يُمحى"; exit 1
fi
if [ -d "$WORK" ] && [ ! -e "$WORK/$STAMP" ] && [ -n "$(ls -A "$WORK" 2>/dev/null)" ]; then
  echo "FAIL: «$WORK» ليس مجلّدَ عملٍ لهذا المُنادي (لا بصمة) وليس فارغًا — لن يُمحى"; exit 1
fi
rm -rf "$WORK" || exit 2
mkdir -p "$WORK" || exit 2
WORK="$(cd "$WORK" && pwd)"
: > "$WORK/$STAMP"

# ── الإنتاج: اختباراتُ gtest تكتب ثنائيّاتِ ELF إلى **مجلّدِ عملها** ─────────────
#    (AR) لذلك يُشغَّلُ كلٌّ منها وcwd = مجلّدُ العمل. ومن يشغّلها من جذرِ المستودع
#         يُخلّف مئةً وثمانيةً وثمانين ملفًّا هناك — وهو ما حدث فعلًا.
PRODUCERS="test_native_backend_m1 test_native_backend_arm64 test_native_sir_bridge"
if [ -n "$PRODUCER" ]; then
  PRODUCER="$(cd "$PRODUCER" && pwd)" || exit 2
  for t in $PRODUCERS; do
    exe="$PRODUCER/$t"
    [ -x "$exe" ] || exe="$PRODUCER/$t.exe"
    if [ ! -x "$exe" ]; then
      echo "❌ المُنتِجُ «$t» غير موجودٍ في «$PRODUCER» — لا ثنائيّاتِ برهان"
      FAILED="$FAILED منتج:$t"
      continue
    fi
    if ( cd "$WORK" && "$exe" >"$WORK/_producer_$t.log" 2>&1 ); then
      echo "✅ المُنتِجُ $t"
    else
      # (AR) الإخفاقُ هنا ليس تفصيلًا: هذه الاختباراتُ تكتب الثنائيّاتِ **ثمّ** تؤكّد،
      #      فقد تُخفِق تأكيدًا وتترك ثنائيّاتٍ سليمةَ التشغيل ⇒ تخضرّ كلُّ البراهينِ
      #      اللاحقةُ عن مُصدِرٍ مكسور. وهذا بالضبط ما جرى مع انجرافِ «فوق»⇒«أعلى»:
      #      رمزُ الخروج ١ ومع ذلك كُتب sad_m1_exit42 ومرّ برهانُه أخضرَ.
      echo "❌ المُنتِجُ $t أخفق (تأكيدٌ مكسورٌ ولو كُتبت الثنائيّات)"
      grep -i "FAIL" "$WORK/_producer_$t.log" 2>/dev/null | head -8 | sed 's/^/      /'
      FAILED="$FAILED منتج:$t"
    fi
  done
fi

# ── مُصرِّفُ الحالاتِ التي تُبنى من مصدرِ ص (لا من gtest) ─────────────────────────
FIXTURES_READY=0
if [ -n "$SADBUILD" ]; then
  FIXTURES_READY=1
  for fx in write_bytes_two_reads write_bytes_read_dir; do
    for pair in "x86:x86_64-unknown-linux-gnu" "arm:aarch64-unknown-linux-gnu"; do
      suffix="${pair%%:*}"; triple="${pair#*:}"
      if ! "$SADBUILD" "$HERE/fixtures/$fx.ص" --خلفية-أصلية --هدف="$triple" \
           -o "$WORK/$fx.$suffix" >/dev/null 2>&1; then
        echo "::warning::تعذّرت ترجمةُ $fx لـ$triple"
        FIXTURES_READY=0
      fi
    done
  done
fi

# ── ④ مُشغِّلُ AArch64 ────────────────────────────────────────────────────────
QEMU="$(command -v qemu-aarch64-static || command -v qemu-aarch64 || true)"

# ── ② جدولُ الاستدعاء: سكربتٌ ⇒ وسائطُه ──────────────────────────────────────
#    (AR) صُرِّح بكلِّ سكربتٍ ولو بوسيطٍ واحد. حارسُ التغطيةِ أدناه يقارن هذا الجدولَ
#         بمحتوى المجلّد، فالإضافةُ بلا تصريحٍ تُخفِق.
declare_only() { # <سكربت> <سببُ عدمِ التشغيل>
  DECLARED="$DECLARED $1"
  SKIPPED="$SKIPPED $1"
  echo "⏭️  $1 — $2"
}

run_proof() { # <سكربت> <وسائط…>
  local script="$1"; shift
  DECLARED="$DECLARED $script"
  local out rc
  # (AR) cwd = مجلّدُ العمل: بعضُ الحالاتِ تكتب ملفّاتٍ نسبيّةً وتقرأ «.»، فتشغيلُها
  #      من جذرِ المستودع يلوّثه ويقيس المجلّدَ الخطأ.
  out="$( cd "$WORK" && bash "$HERE/$script" "$@" 2>&1 )"; rc=$?
  # (AR) SKIP يُطبَع ولا يُغيّر رمزَ الخروج في هذه السكربتات ⇒ نلتقطه نصًّا،
  #      وإلّا مرّ «لم يُنفَّذ شيء» نجاحًا. (وسكربتا ARM64 يخرجان ٣ عند غيابِ qemu.)
  local skips
  skips="$(printf '%s\n' "$out" | grep -c '^SKIP')"
  if [ "$rc" -eq 0 ] && [ "$skips" -eq 0 ]; then
    echo "✅ $script"
    PASSED=$((PASSED+1))
    return 0
  fi
  if { [ "$rc" -eq 0 ] || [ "$rc" -eq 3 ]; } && [ "$ALLOW_SKIP" = "1" ]; then
    echo "⏭️  $script — تخطٍّ مقبولٌ بطلبٍ صريح (SAD_PROOFS_ALLOW_SKIP=1)"
    printf '%s\n' "$out" | sed 's/^/      /'
    SKIPPED="$SKIPPED $script"
    return 0
  fi
  echo "❌ $script (رمز $rc، تخطٍّ $skips)"
  printf '%s\n' "$out" | sed 's/^/      /'
  FAILED="$FAILED $script"
  return 1
}

echo "════════ براهينُ الخلفيّةِ الأصليّة — تنفيذٌ حيّ ════════"
echo "مجلّدُ العمل: $WORK"
echo "qemu-aarch64: ${QEMU:-«غير متوفّر»}"
echo

# (AR) غيابُ qemu يحذف نصفَ التغطيةِ (AArch64) بلا أثرٍ في المخرَج ⇒ إخفاقٌ صريح.
if [ -z "$QEMU" ]; then
  if [ "$ALLOW_SKIP" = "1" ]; then
    echo "⏭️  qemu-aarch64 غائب — براهينُ AArch64 كلُّها خارجَ القياسِ في هذه التشغيلة"
    SKIPPED="$SKIPPED qemu"
  else
    echo "❌ qemu-aarch64 غائب — نصفُ البراهينِ (AArch64) لا يُنفَّذ، والنتيجةُ ستُقرأ تغطيةً كاملة"
    FAILED="$FAILED qemu"
  fi
fi

run_proof prove_m1_exit42.sh          "$WORK/sad_m1_exit42"
run_proof prove_x86_loop42.sh         "$WORK/sad_m3x_loop42"
run_proof prove_sir_bridge.sh         "$WORK/sad_sir_bridge_exit42"
run_proof prove_sir_call.sh           "$WORK/sad_sir_call42"
run_proof prove_sir_memory.sh         "$WORK/sad_sir_while42"
run_proof prove_sir_branch.sh         "$WORK/sad_sir_if_true42" "$WORK/sad_sir_if_false42" "$WORK/sad_sir_backjump42"
run_proof prove_sir_spill.sh          "$WORK/sad_sir_nested42"  "$WORK/sad_sir_liveacross42"
run_proof prove_arm64_exit42.sh       "$WORK/sad_m3_arm64_exit42"
run_proof prove_arm64_calls_print.sh  "$WORK"
run_proof prove_float.sh              "$WORK"
run_proof prove_boxing.sh             "$WORK"
run_proof prove_boxing_str.sh         "$WORK"
run_proof prove_array_append.sh       "$WORK"
run_proof prove_array_bounds.sh       "$WORK"
run_proof prove_virtual_dispatch.sh   "$WORK/sad_sir_virtual42" "$WORK/sad_sir_virtual_livetemp42"
if [ -n "$QEMU" ]; then
  # (AR) نظيرُ الإرسالِ الافتراضيّ على AArch64 (blr عبر vtable) تحت qemu.
  run_proof prove_virtual_dispatch.sh "$WORK/sad_arm64_virtual42" "$WORK/sad_arm64_virtual_livetemp42" "$QEMU"
fi
run_proof prove_write_bytes.sh        "$WORK/sad_sir_writebytes"
if [ -n "$QEMU" ]; then
  # (AR) نظيرُ بايتاتِ الملفّات على AArch64: السكربتُ يقبل مُشغِّلًا، والتباعدُ بين
  #      المعماريّتين هو ما كشف عيبَ تراكبِ خاناتِ الخدشِ في x86 أوّلَ مرّة.
  run_proof prove_write_bytes.sh      "$WORK/sad_arm64_writebytes" "$QEMU"
fi

# (AR) حافّتا البايتات تلزمهما حالتان تُصرَّفان من مصدرِ ص — لا مُنتِجَ لهما في gtest
#      (وهذا سببُ بقاءِ prove_write_bytes_edges.sh بلا مُنادٍ أصلًا). فبلا مصرّفٍ
#      لا تُشغَّل: تشغيلُها على ثنائيٍّ غيرِ موجودٍ يُنتج «رمز ١٢٧» أحمرَ كاذبًا.
if [ "$FIXTURES_READY" = "1" ]; then
  run_proof prove_write_bytes_edges.sh "$WORK/write_bytes_two_reads.x86" "$WORK/write_bytes_read_dir.x86"
  if [ -n "$QEMU" ]; then
    run_proof prove_write_bytes_edges.sh "$WORK/write_bytes_two_reads.arm" "$WORK/write_bytes_read_dir.arm" "$QEMU"
  fi
elif [ -n "$SADBUILD" ]; then
  echo "❌ prove_write_bytes_edges.sh — أخفقت ترجمةُ حالاتِه من مصدرِ ص"
  DECLARED="$DECLARED prove_write_bytes_edges.sh"
  FAILED="$FAILED prove_write_bytes_edges.sh"
elif [ "$NO_COMPILER" = "1" ]; then
  declare_only prove_write_bytes_edges.sh "لا مُصرِّفَ في هذه التشغيلة (--بلا-مصرف) ⇒ حافّتا البايتات غيرُ مقيستين"
else
  echo "❌ prove_write_bytes_edges.sh — لا مُصرِّف (--مصرف) ⇒ لا حالاتِ برهان"
  DECLARED="$DECLARED prove_write_bytes_edges.sh"
  FAILED="$FAILED prove_write_bytes_edges.sh"
fi

if [ -n "$SADBUILD" ]; then
  run_proof prove_any_float.sh        "$SADBUILD" "$WORK/anyfloat"
elif [ "$NO_COMPILER" = "1" ]; then
  declare_only prove_any_float.sh "لا مُصرِّفَ في هذه التشغيلة (--بلا-مصرف) ⇒ فكُّ Any بوسمِه غيرُ مقيس"
else
  echo "❌ prove_any_float.sh — لا مُصرِّف (--مصرف) ⇒ لا برهانَ لفكِّ Any بوسمِه"
  DECLARED="$DECLARED prove_any_float.sh"
  FAILED="$FAILED prove_any_float.sh"
fi

if [ -n "$SADBUILD" ]; then
  run_proof prove_map_and_strings.sh  "$SADBUILD" "$WORK/mapstr"
elif [ "$NO_COMPILER" = "1" ]; then
  declare_only prove_map_and_strings.sh "لا مُصرِّفَ في هذه التشغيلة (--بلا-مصرف) ⇒ إسنادُ النصّ والخريطةُ غيرُ مقيسَين"
else
  echo "❌ prove_map_and_strings.sh — لا مُصرِّف (--مصرف) ⇒ لا برهانَ لإسنادِ النصّ ولا للخريطة"
  DECLARED="$DECLARED prove_map_and_strings.sh"
  FAILED="$FAILED prove_map_and_strings.sh"
fi

# ── ③ حارسُ التغطية ──────────────────────────────────────────────────────────
echo
MISSING=""
for f in "$HERE"/prove_*.sh; do
  name="$(basename "$f")"
  case " $DECLARED " in *" $name "*) ;; *) MISSING="$MISSING $name" ;; esac
done
if [ -n "$MISSING" ]; then
  echo "❌ حارسُ التغطية: سكربتاتُ برهانٍ بلا مُنادٍ في هذا الجدول:$MISSING"
  echo "   (أضِفها إلى run_native_proofs.sh — سكربتٌ بلا مُنادٍ لا يحرس شيئًا)"
  FAILED="$FAILED حارس_التغطية"
else
  echo "✅ حارسُ التغطية: كلُّ سكربتِ برهانٍ في المجلّد مُستدعًى"
fi

echo
if [ -n "$FAILED" ]; then
  echo "════ إخفاق:$FAILED"
  exit 1
fi
if [ -n "$SKIPPED" ]; then
  echo "════ نجاحٌ بتخطٍّ مُعلَنٍ صراحةً:$SKIPPED — التغطيةُ ناقصةٌ في هذه التشغيلة"
  exit 0
fi
echo "════ نجح $PASSED برهانًا حيًّا بلا تخطٍّ — الخلفيّةُ الأصليّةُ مقيسةٌ تنفيذًا لا ترجمةً"
exit 0
