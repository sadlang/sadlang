#!/usr/bin/env bash
# ============================================================================
# (SEM045 / RFC عقد الغياب — المرحلة أ٢ + سدّ ثقب المعاملات)
#
# (AR) يقيس ما لم يقسه ns07:
#      ① المفسّر — معاملُ دالةٍ مصنَّفٌ يستقبل «فراغ» (الثقب المعلَن في أ١):
#         الدرجات الأربع، والإيقافُ في الإنتاج قبل دخول الجسم.
#      ② المحلّل الساكن المشترك — «فراغ ساكن ⇒ خانة مصنّفة»: نداءُ دالةٍ لا
#         تحوي `ارجع` بقيمة، تصريحًا وإعادةَ إسناد، في المحرّكين.
#      ③ المترجم — الحارس الزمنيّ قبل STORE: قيمةُ خريطة_احصل الغائبة (وسمُها
#         «فراغ» وقتَ التشغيل فقط) تُكتَب في خانة نص: بلا أعلام تحذيرٌ ويكمل،
#         ‎--إنتاج إيقافٌ موضعيّ، ‎--جامع صمت.
#      ④ سلبيّان: الخانةُ المجرّدة (بلا نوع) لا تُحرَس؛ والبديل الثالث يلغي
#         الفراغَ أصلًا.
#
# (EN) Measures SEM045 stage أ٢ (shared static rule + compiler pre-STORE
#      runtime guard) and the typed-parameter hole fix, across the D6 axis.
#
# الاستعمال:  bash tests/behavior/null_safety/ns08_void_crossing_stage_a2_check.sh
# ============================================================================
set -u
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
RUN="$ROOT/build/bin/Debug/sad-run.exe"
[ -x "$RUN" ] || RUN="$ROOT/dist/Debug/sad-run.exe"
BUILD="$ROOT/build/bin/Debug/sad-build.exe"
[ -x "$BUILD" ] || BUILD="$ROOT/dist/Debug/sad-build.exe"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

pass=0; fail=0
check() { # وصف ، شرط(0=نجاح)
  if [ "$2" -eq 0 ]; then echo "  ✅ $1"; pass=$((pass+1));
  else echo "  ❌ $1"; fail=$((fail+1)); fi
}

# ── ① معامل مصنّف يستقبل فراغًا (المفسّر) ──────────────────────────────────
P1="$TMP/param.ص"
cat > "$P1" <<'SAD'
استورد خرائط
دالة استقبل(نص قيمة)
    اطبع_سطر("داخل_الدالة")
نهاية
متغير سجل = خريطة()
استقبل(خريطة_احصل(سجل، "لا_وجود_له"))
اطبع_سطر("بعد_النداء")
SAD

echo "== ①: معامل نص يستقبل فراغ خريطة_احصل (مفسّر) =="
err=$("$RUN" "$P1" 2>&1 1>/dev/null); rc=$?
echo "$err" | grep -q "تحذير نوع SEM045"; check "افتراضي: تحذير SEM045" $?
check "افتراضي: الخروج 0 (يكمل)" $([ $rc -eq 0 ] && echo 0 || echo 1)
err=$("$RUN" --جامع "$P1" 2>&1 1>/dev/null); rc=$?
echo "$err" | grep -q "SEM045"; g=$?
check "جامع: لا تشخيص SEM045" $([ $g -ne 0 ] && echo 0 || echo 1)
check "جامع: الخروج 0" $([ $rc -eq 0 ] && echo 0 || echo 1)
err=$("$RUN" --إنتاج "$P1" 2>&1 1>/dev/null); rc=$?
echo "$err" | grep -q "خطأ نوع SEM045"; check "إنتاج: وسم [خطأ نوع SEM045]" $?
check "إنتاج: خروج ≠ 0" $([ $rc -ne 0 ] && echo 0 || echo 1)
out=$("$RUN" --إنتاج "$P1" 2>/dev/null)
echo "$out" | grep -q "داخل_الدالة"; p=$?
check "إنتاج: جسم الدالة لا يُدخَل" $([ $p -ne 0 ] && echo 0 || echo 1)

# ── ② القاعدة الساكنة: دالة فراغ يقينيّ ⇒ خانة مصنّفة ──────────────────────
S1="$TMP/static_decl.ص"
cat > "$S1" <<'SAD'
دالة بلا_عائد()
    اطبع_سطر("جسم")
نهاية
متغير نص ناتج = بلا_عائد()
اطبع_سطر("بعد_الإسناد")
SAD
S2="$TMP/static_reassign.ص"
cat > "$S2" <<'SAD'
دالة بلا_عائد()
    اطبع_سطر("جسم")
نهاية
متغير نص ناتج = "قيمة_أولى"
ناتج = بلا_عائد()
اطبع_سطر("بعد_الإسناد")
SAD

echo "== ②-أ: ساكن — تصريح (مفسّر) =="
err=$("$RUN" "$S1" 2>&1 1>/dev/null); rc=$?
echo "$err" | grep -q "SEM045: الخانة 'ناتج'"; check "افتراضي: تشخيص المحلّل الساكن" $?
check "افتراضي: الخروج 0 (يكمل)" $([ $rc -eq 0 ] && echo 0 || echo 1)
err=$("$RUN" --إنتاج "$S1" 2>&1 1>/dev/null); rc=$?
check "إنتاج: خروج ≠ 0" $([ $rc -ne 0 ] && echo 0 || echo 1)
out=$("$RUN" --إنتاج "$S1" 2>/dev/null)
echo "$out" | grep -q "جسم"; p=$?
check "إنتاج: التنفيذ لا يبدأ أصلًا" $([ $p -ne 0 ] && echo 0 || echo 1)
err=$("$RUN" --جامع "$S1" 2>&1 1>/dev/null); rc=$?
echo "$err" | grep -q "SEM045"; g=$?
check "جامع: لا تشخيص SEM045" $([ $g -ne 0 ] && echo 0 || echo 1)

echo "== ②-ب: ساكن — إعادة إسناد (مفسّر) =="
err=$("$RUN" "$S2" 2>&1 1>/dev/null); rc=$?
echo "$err" | grep -q "SEM045: الخانة 'ناتج'"; check "افتراضي: تشخيص المحلّل الساكن" $?
check "افتراضي: الخروج 0 (يكمل)" $([ $rc -eq 0 ] && echo 0 || echo 1)

# (AR) المحلّل مشترك — يُقاس عبر المترجم أيضًا: افتراضيُّ سياسة المترجم صارم
#      (UltraStrict — سابقة فاحص الاستعارة) فالتشخيص الساكن يُفشِل الترجمة.
if [ -x "$BUILD" ]; then
  echo "== ②-ج: ساكن — عبر المترجم (المحلّل المشترك) =="
  out=$("$BUILD" "$S1" -o "$TMP/static_c.exe" 2>&1); rc=$?
  echo "$out" | grep -q "SEM045: الخانة 'ناتج'"; check "افتراضي: تشخيص SEM045 في مخرجات المترجم" $?
  check "افتراضي: الترجمة تفشل (سياسة المترجم الافتراضية صارمة)" $([ $rc -ne 0 ] && echo 0 || echo 1)
fi

# ── ③ الحارس الزمنيّ قبل STORE (المترجم) ────────────────────────────────────
DYN="$TMP/dyn_store.ص"
cat > "$DYN" <<'SAD'
استورد خرائط
متغير سجل = خريطة()
متغير غائب = خريطة_احصل(سجل، "لا_وجود_له")
متغير نص اسم = غائب
اطبع_سطر("بعد_الإسناد")
SAD

echo "== ③: خريطة_احصل غائبة ⇒ خانة نص (مترجم) =="
if [ -x "$BUILD" ]; then
  "$BUILD" "$DYN" -o "$TMP/dyn_default.exe" >/dev/null 2>&1
  check "افتراضي: الترجمة تنجح" $([ -x "$TMP/dyn_default.exe" ] && echo 0 || echo 1)
  if [ -x "$TMP/dyn_default.exe" ]; then
    out=$("$TMP/dyn_default.exe" 2>&1); rc=$?
    echo "$out" | grep -q "تحذير نوع SEM045"; check "افتراضي: تحذير SEM045 وقت التشغيل" $?
    echo "$out" | grep -q "بعد_الإسناد"; check "افتراضي: التنفيذ يكمل" $?
    check "افتراضي: الخروج 0" $([ $rc -eq 0 ] && echo 0 || echo 1)
  fi
  "$BUILD" --إنتاج "$DYN" -o "$TMP/dyn_prod.exe" >/dev/null 2>&1
  check "إنتاج: الترجمة تنجح (الفحص زمنيّ لا ساكن)" $([ -x "$TMP/dyn_prod.exe" ] && echo 0 || echo 1)
  if [ -x "$TMP/dyn_prod.exe" ]; then
    out=$("$TMP/dyn_prod.exe" 2>&1); rc=$?
    echo "$out" | grep -q "خطأ نوع SEM045"; check "إنتاج: وسم [خطأ نوع SEM045]" $?
    check "إنتاج: خروج ≠ 0 (يوقف)" $([ $rc -ne 0 ] && echo 0 || echo 1)
    echo "$out" | grep -q "بعد_الإسناد"; p=$?
    check "إنتاج: ما بعد الإسناد لا يُنفَّذ" $([ $p -ne 0 ] && echo 0 || echo 1)
  fi
  "$BUILD" --جامع "$DYN" -o "$TMP/dyn_gc.exe" >/dev/null 2>&1
  check "جامع: الترجمة تنجح" $([ -x "$TMP/dyn_gc.exe" ] && echo 0 || echo 1)
  if [ -x "$TMP/dyn_gc.exe" ]; then
    out=$("$TMP/dyn_gc.exe" 2>&1); rc=$?
    echo "$out" | grep -q "SEM045"; g=$?
    check "جامع: لا تشخيص SEM045" $([ $g -ne 0 ] && echo 0 || echo 1)
    check "جامع: الخروج 0" $([ $rc -eq 0 ] && echo 0 || echo 1)
  fi
else
  echo "  ⚠ sad-build غير موجود — قياس المترجم مُتخطًّى (يُبلَّغ لا يُخفى)"
  fail=$((fail+1))
fi

# ── ④ السلبيّان ─────────────────────────────────────────────────────────────
N1="$TMP/bare_slot.ص"
cat > "$N1" <<'SAD'
استورد خرائط
متغير سجل = خريطة()
متغير عنصر = "قيمة"
عنصر = خريطة_احصل(سجل، "لا_وجود_له")
اطبع_سطر("بعد_الإسناد")
SAD
N2="$TMP/default_arg.ص"
cat > "$N2" <<'SAD'
استورد خرائط
متغير سجل = خريطة()
متغير نص اسم = خريطة_احصل(سجل، "لا_وجود_له"، "بديل")
اطبع_سطر("النتيجة:" + اسم)
SAD

echo "== ④-أ: الخانة المجرّدة لا تُحرَس (مفسّر) =="
err=$("$RUN" --إنتاج "$N1" 2>&1 1>/dev/null); rc=$?
echo "$err" | grep -q "SEM045"; g=$?
check "إنتاج: لا تشخيص SEM045 (خانة مجرّدة)" $([ $g -ne 0 ] && echo 0 || echo 1)
check "إنتاج: الخروج 0" $([ $rc -eq 0 ] && echo 0 || echo 1)
if [ -x "$BUILD" ]; then
  echo "== ④-ب: الخانة المجرّدة لا تُحرَس (مترجم، --إنتاج) =="
  "$BUILD" --إنتاج "$N1" -o "$TMP/bare_prod.exe" >/dev/null 2>&1
  if [ -x "$TMP/bare_prod.exe" ]; then
    out=$("$TMP/bare_prod.exe" 2>&1); rc=$?
    echo "$out" | grep -q "SEM045"; g=$?
    check "إنتاج: لا تشخيص SEM045 (خانة مجرّدة)" $([ $g -ne 0 ] && echo 0 || echo 1)
    check "إنتاج: الخروج 0" $([ $rc -eq 0 ] && echo 0 || echo 1)
  else
    check "إنتاج: ترجمة الخانة المجرّدة تنجح" 1
  fi
  echo "== ④-ج: البديل الثالث يلغي الفراغ (مترجم، --إنتاج) =="
  "$BUILD" --إنتاج "$N2" -o "$TMP/def_prod.exe" >/dev/null 2>&1
  if [ -x "$TMP/def_prod.exe" ]; then
    out=$("$TMP/def_prod.exe" 2>&1); rc=$?
    echo "$out" | grep -q "SEM045"; g=$?
    check "إنتاج: لا تشخيص SEM045 مع البديل" $([ $g -ne 0 ] && echo 0 || echo 1)
    echo "$out" | grep -q "النتيجة:بديل"; check "القيمة هي البديل" $?
    check "إنتاج: الخروج 0" $([ $rc -eq 0 ] && echo 0 || echo 1)
  else
    check "إنتاج: ترجمة البديل الثالث تنجح" 1
  fi
else
  echo "  ⚠ sad-build غير موجود — سلبيّا المترجم (④-ب/④-ج) مُتخطَّيان (يُبلَّغ لا يُخفى)"
  fail=$((fail+1))
fi

echo "── النتيجة: نجح=$pass فشل=$fail ──"
[ "$fail" -eq 0 ]
