#!/usr/bin/env bash
# ═══════════════════════════════════════════════════════════════════════════════
# (AR) ns09 — المرحلة ب من RFC عقد الغياب: الجلب المصنَّف خريطة_اجلب_نص/رقم/منطقي
#      + حارس معامل الدالة المصنَّف في المسار المترجَم (SEM045)
#      + صدق فاحص الأنواع («وُجد 'فراغ'» لا «وُجد 'رقم'» المختلَق)
#      العقد: الغيابُ «لاشيء» حصرًا؛ الحضورُ بنوعٍ مغايرٍ أو بعدمٍ مخزَّنٍ
#      خطأُ تشغيلٍ صريح RUN074؛ لا وسيطَ بديلًا ثالثًا — التحصيل بـ«؟؟».
# (EN) ns09 — absence-contract RFC stage ب: typed map fetch, compiled-path
#      typed-param guard (SEM045), and the truthful type-checker Void inference.
# ═══════════════════════════════════════════════════════════════════════════════
#
# ⚠️ **دَينٌ مُقيَّدٌ — هذا السكربتُ لا يُنادَى من أيِّ بوّابةٍ ولا سيرِ عمل.**
#
#      خمسةُ نداءاتٍ للمفسّرِ وأربعةَ عشرَ للمترجم. قِيس ٢٠٢٦-٠٩-٠٤: ١٣ إخفاقًا كلُّها موسومةٌ «مفسّر» — أي أنّ نصفَه المترجِميَّ (وهو أكبرُ نصفَيه) يعمل.
#
#      وسببُ التقييدِ أنّ «متجاهَل» يجبُ أن يبقى قرارًا لا نسيانًا: حارسٌ
#      لا يُنادَى ليس حارسًا، وسكوتُه يُقرأُ خضرةً وهو صمتٌ. والوصلُ
#      يحتاجُ فصلَ نصفِ المفسّرِ أوّلًا — وذاك عملٌ مقصودٌ لا يُقحَمُ في
#      رقعةِ تنظيف. والنظيرُ الموصولُ: ns03 (بوّابةٌ في ci.yml).
# (EN) TRACKED DEBT — no gate or workflow invokes this script. "Ignored"
#      must stay a decision, never an oversight: a guard nobody calls is
#      not a guard, and its silence reads as green while it is only
#      silence. Wiring it up requires splitting the interpreter half out
#      first, which is deliberate work, not a cleanup patch. The wired
#      sibling is ns03 (a gate in ci.yml).

set -u
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
RUN="$ROOT/build/bin/Debug/sad-run.exe";  [ -x "$RUN" ]  || RUN="$ROOT/dist/Debug/sad-run.exe"
BUILD="$ROOT/build/bin/Debug/sad-build.exe"; [ -x "$BUILD" ] || BUILD="$ROOT/dist/Debug/sad-build.exe"
TMP="$(mktemp -d)"; trap 'rm -rf "$TMP"' EXIT
PASS=0; FAIL=0
check() { # $1=label  $2=rc(0=pass)
  if [ "$2" -eq 0 ]; then PASS=$((PASS+1)); else FAIL=$((FAIL+1)); echo "  ✗ $1"; fi
}

# ── ① الجلب المصنَّف — المصفوفة الرباعية (مفسّر) ─────────────────────────────
S1="$TMP/typed_fetch.ص"
cat > "$S1" <<'SAD'
استورد خرائط
متغير سجل = خريطة()
سجل["الاسم"] = "سعاد"
سجل["العمر"] = 30
سجل["نشط"] = صحيح
اطبع_سطر(خريطة_اجلب_نص(سجل، "الاسم") ؟؟ "بديل")
اطبع_سطر(خريطة_اجلب_نص(سجل، "لا_وجود") ؟؟ "بديل")
اطبع_سطر(خريطة_اجلب_رقم(سجل، "العمر") ؟؟ 0)
اطبع_سطر(خريطة_اجلب_رقم(سجل، "لا_وجود") ؟؟ 77)
اطبع_سطر(خريطة_اجلب_منطقي(سجل، "نشط") ؟؟ خطأ)
اطبع_سطر("B2:" + (خريطة_اجلب_منطقي(سجل، "لا_وجود") ؟؟ صحيح))
اطبع_سطر("تمام")
SAD

echo "== ①: الجلب المصنَّف — حاضر/غائب (مفسّر) =="
out=$("$RUN" "$S1" 2>&1); rc=$?
echo "$out" | grep -q "سعاد";  check "مفسّر: الحاضر النصي يمرّ" $?
echo "$out" | grep -q "بديل";  check "مفسّر: الغائب النصي ⇒ ؟؟ يرى العدم" $?
echo "$out" | grep -q "30";    check "مفسّر: الحاضر الرقمي يمرّ" $?
echo "$out" | grep -q "77";    check "مفسّر: الغائب الرقمي ⇒ ؟؟ يرى العدم" $?
echo "$out" | grep -qx "صحيح"; check "مفسّر: المنطقي الحاضر يمرّ" $?
echo "$out" | grep -q "B2:صحيح"; check "مفسّر: المنطقي الغائب ⇒ ؟؟ يرى العدم" $?
echo "$out" | grep -q "تمام";  check "مفسّر: التنفيذ يكتمل" $?
check "مفسّر: الخروج 0" $([ $rc -eq 0 ] && echo 0 || echo 1)

# ── ② الحضور بنوع مغاير ⇒ RUN074 (مفسّر) ────────────────────────────────────
S2="$TMP/typed_fetch_mismatch.ص"
cat > "$S2" <<'SAD'
استورد خرائط
متغير سجل = خريطة()
سجل["العمر"] = 30
اطبع_سطر(خريطة_اجلب_نص(سجل، "العمر") ؟؟ "بديل")
اطبع_سطر("بعد_الخطأ")
SAD

echo "== ②: حاضر بنوع مغاير ⇒ خطأ صريح (مفسّر) =="
out=$("$RUN" "$S2" 2>&1); rc=$?
echo "$out" | grep -q "RUN074"; check "مفسّر: رمز RUN074 في التشخيص" $?
check "مفسّر: التنفيذ يقف (لا بعد_الخطأ)" $([ $(echo "$out" | grep -c "بعد_الخطأ") -eq 0 ] && echo 0 || echo 1)
check "مفسّر: الخروج ≠ 0" $([ $rc -ne 0 ] && echo 0 || echo 1)

# ── ③ العدم المخزَّن حضورٌ بنوع مغاير لا غياب (مفسّر) ───────────────────────
S3="$TMP/typed_fetch_stored_null.ص"
cat > "$S3" <<'SAD'
استورد خرائط
متغير سجل = خريطة()
سجل["فارغ"] = لاشيء
اطبع_سطر(خريطة_اجلب_نص(سجل، "فارغ") ؟؟ "بديل")
SAD

echo "== ③: عدم مخزَّن ⇒ خطأ لا عدم صامت (مفسّر) =="
out=$("$RUN" "$S3" 2>&1); rc=$?
echo "$out" | grep -q "RUN074"; check "مفسّر: العدم المخزَّن يرفع RUN074" $?
# (AR) -x: التشخيص يقتبس سطرَ المصدر وفيه «بديل» — يُحسَب سطرُ المخرجات وحده (قِيس)
check "مفسّر: لا «بديل» (ليست قناة عدم ثانية)" $([ $(echo "$out" | grep -cx "بديل") -eq 0 ] && echo 0 || echo 1)

# ── ④ الجلب المصنَّف مترجَمًا — حاضر/غائب + النوع المغاير ────────────────────
echo "== ④: الجلب المصنَّف (مترجم) =="
if [ -x "$BUILD" ]; then
  "$BUILD" "$S1" -o "$TMP/tf.exe" >/dev/null 2>&1
  check "مترجم: ترجمة المصفوفة تنجح" $([ -x "$TMP/tf.exe" ] && echo 0 || echo 1)
  if [ -x "$TMP/tf.exe" ]; then
    out=$("$TMP/tf.exe" 2>&1); rc=$?
    echo "$out" | grep -q "سعاد"; check "مترجم: الحاضر النصي يمرّ" $?
    echo "$out" | grep -q "بديل"; check "مترجم: الغائب النصي ⇒ ؟؟ يرى العدم" $?
    echo "$out" | grep -q "30";   check "مترجم: الحاضر الرقمي يمرّ" $?
    echo "$out" | grep -q "77";   check "مترجم: الغائب الرقمي ⇒ ؟؟ يرى العدم" $?
    echo "$out" | grep -qx "صحيح"; check "مترجم: المنطقي الحاضر يمرّ" $?
    echo "$out" | grep -q "B2:صحيح"; check "مترجم: المنطقي الغائب ⇒ ؟؟ يرى العدم" $?
    echo "$out" | grep -q "تمام"; check "مترجم: التنفيذ يكتمل" $?
  fi
  "$BUILD" "$S2" -o "$TMP/tfm.exe" >/dev/null 2>&1
  if [ -x "$TMP/tfm.exe" ]; then
    out=$("$TMP/tfm.exe" 2>&1); rc=$?
    echo "$out" | grep -q "RUN074"; check "مترجم: النوع المغاير يرفع RUN074" $?
    check "مترجم: التنفيذ يقف" $([ $(echo "$out" | grep -c "بعد_الخطأ") -eq 0 ] && echo 0 || echo 1)
    check "مترجم: الخروج ≠ 0" $([ $rc -ne 0 ] && echo 0 || echo 1)
  else
    check "مترجم: ترجمة النوع المغاير تنجح" 1
  fi
  "$BUILD" "$S3" -o "$TMP/tfn.exe" >/dev/null 2>&1
  if [ -x "$TMP/tfn.exe" ]; then
    out=$("$TMP/tfn.exe" 2>&1)
    echo "$out" | grep -q "RUN074"; check "مترجم: العدم المخزَّن يرفع RUN074" $?
  else
    check "مترجم: ترجمة العدم المخزَّن تنجح" 1
  fi
else
  check "sad-build مفقود — أذرع المترجم لم تُقس" 1
fi

# ── ⑤ حارس معامل الدالة المصنَّف — المسار المترجَم (SEM045) ─────────────────
P1="$TMP/param_typed.ص"
cat > "$P1" <<'SAD'
استورد خرائط
دالة استقبل(نص قيمة)
    اطبع_سطر("داخل_الدالة")
نهاية
متغير سجل = خريطة()
استقبل(خريطة_احصل(سجل، "لا_وجود_له"))
اطبع_سطر("بعد_النداء")
SAD

echo "== ⑤: فراغ ⇒ معامل مصنَّف (مترجم) =="
if [ -x "$BUILD" ]; then
  "$BUILD" "$P1" -o "$TMP/pt_default.exe" >/dev/null 2>&1
  if [ -x "$TMP/pt_default.exe" ]; then
    out=$("$TMP/pt_default.exe" 2>&1); rc=$?
    echo "$out" | grep -q "تحذير نوع SEM045"; check "افتراضي: تحذير SEM045" $?
    echo "$out" | grep -q "بعد_النداء"; check "افتراضي: التنفيذ يكمل" $?
    check "افتراضي: الخروج 0" $([ $rc -eq 0 ] && echo 0 || echo 1)
  else
    check "افتراضي: الترجمة تنجح" 1
  fi
  "$BUILD" --إنتاج "$P1" -o "$TMP/pt_prod.exe" >/dev/null 2>&1
  if [ -x "$TMP/pt_prod.exe" ]; then
    out=$("$TMP/pt_prod.exe" 2>&1); rc=$?
    echo "$out" | grep -q "خطأ نوع SEM045"; check "إنتاج: خطأ SEM045" $?
    check "إنتاج: جسم الدالة لا يُدخَل" $([ $(echo "$out" | grep -c "داخل_الدالة") -eq 0 ] && echo 0 || echo 1)
    check "إنتاج: الخروج ≠ 0" $([ $rc -ne 0 ] && echo 0 || echo 1)
  else
    check "إنتاج: الترجمة تنجح" 1
  fi
  "$BUILD" --جامع "$P1" -o "$TMP/pt_perm.exe" >/dev/null 2>&1
  if [ -x "$TMP/pt_perm.exe" ]; then
    out=$("$TMP/pt_perm.exe" 2>&1)
    check "جامع: صمت تام" $([ $(echo "$out" | grep -c "SEM045") -eq 0 ] && echo 0 || echo 1)
  else
    check "جامع: الترجمة تنجح" 1
  fi
  # (AR) الشاهدان السالبان: المعامل المجرّد و«أي» ليسا في سجلّ التصنيف فلا يُحرَسان
  for shape in "قيمة" "أي قيمة"; do
    N="$TMP/param_neg.ص"
    sed "s/نص قيمة/$shape/" "$P1" > "$N"
    "$BUILD" --إنتاج "$N" -o "$TMP/pn.exe" >/dev/null 2>&1
    if [ -x "$TMP/pn.exe" ]; then
      out=$("$TMP/pn.exe" 2>&1); rc=$?
      check "سالب ($shape): لا SEM045 والخروج 0" $([ $(echo "$out" | grep -c "SEM045") -eq 0 ] && [ $rc -eq 0 ] && echo 0 || echo 1)
    else
      check "سالب ($shape): الترجمة تنجح" 1
    fi
    rm -f "$TMP/pn.exe"
  done
  # (AR) شاهد سالب ثالث: قيمة حاضرة تعبر المعامل بلا تشخيص
  H="$TMP/param_hit.ص"
  sed 's/خريطة_احصل(سجل، "لا_وجود_له")/خريطة_احصل(سجل، "أ")/; s/متغير سجل = خريطة()/متغير سجل = خريطة()\nسجل["أ"] = "حاضرة"/' "$P1" > "$H"
  "$BUILD" --إنتاج "$H" -o "$TMP/ph.exe" >/dev/null 2>&1
  if [ -x "$TMP/ph.exe" ]; then
    out=$("$TMP/ph.exe" 2>&1); rc=$?
    check "سالب (قيمة حاضرة): تعبر بلا SEM045" $([ $(echo "$out" | grep -c "SEM045") -eq 0 ] && [ $rc -eq 0 ] && echo 0 || echo 1)
  else
    check "سالب (قيمة حاضرة): الترجمة تنجح" 1
  fi
else
  check "sad-build مفقود — حارس المعاملات لم يُقس" 1
fi

# ── ⑥ صدق فاحص الأنواع: «وُجد 'فراغ'» لا «وُجد 'رقم'» ───────────────────────
T1="$TMP/checker_truth.ص"
cat > "$T1" <<'SAD'
دالة لا_شيء()
    اطبع_سطر("مرحبا")
نهاية
نص س = لا_شيء()
SAD
T2="$TMP/checker_num.ص"
cat > "$T2" <<'SAD'
دالة لا_شيء()
    اطبع_سطر("مرحبا")
نهاية
رقم س = لا_شيء()
SAD

echo "== ⑥: فاحص الأنواع الصادق (مترجم) =="
if [ -x "$BUILD" ]; then
  out=$("$BUILD" "$T1" -o "$TMP/t1.exe" 2>&1); rc=$?
  echo "$out" | grep -q "وُجد 'فراغ'"; check "نص←فراغ: التشخيص «وُجد 'فراغ'»" $?
  check "نص←فراغ: لا «وُجد 'رقم'» المختلَقة" $([ $(echo "$out" | grep -c "وُجد 'رقم'") -eq 0 ] && echo 0 || echo 1)
  check "نص←فراغ: الترجمة تفشل" $([ $rc -ne 0 ] && echo 0 || echo 1)
  out=$("$BUILD" "$T2" -o "$TMP/t2.exe" 2>&1); rc=$?
  echo "$out" | grep -q "وُجد 'فراغ'"; check "رقم←فراغ: يُرفض الآن (كان يمرّ صامتًا)" $?
  check "رقم←فراغ: الترجمة تفشل" $([ $rc -ne 0 ] && echo 0 || echo 1)
else
  check "sad-build مفقود — فاحص الأنواع لم يُقس" 1
fi

# ── ⑦ حارسا الاتجاهين: العقود القائمة لا تُمسّ ───────────────────────────────
G1="$TMP/guard_dynamic.ص"
cat > "$G1" <<'SAD'
استورد خرائط
متغير سجل = خريطة()
متغير غائب = خريطة_احصل(سجل، "لا_وجود")
اطبع_سطر(نوع(غائب))
اطبع_سطر(خريطة_احصل(سجل، "لا_وجود"، "الثالث"))
SAD

echo "== ⑦: الجلب الديناميكي القائم لا يُمسّ =="
out=$("$RUN" "$G1" 2>&1); rc=$?
echo "$out" | grep -q "فراغ";  check "مفسّر: غياب خريطة_احصل ما زال «فراغ»" $?
echo "$out" | grep -q "الثالث"; check "مفسّر: الوسيط البديل الثالث نافذ" $?
check "مفسّر: الخروج 0" $([ $rc -eq 0 ] && echo 0 || echo 1)

echo ""
echo "═══ ns09: نجح $PASS / فشل $FAIL ═══"
[ $FAIL -eq 0 ]
