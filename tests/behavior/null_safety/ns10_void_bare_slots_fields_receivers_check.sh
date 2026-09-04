#!/usr/bin/env bash
# ============================================================================
# ns10 — موجة الديون الموروثة لعقد الغياب (RFC عقد الغياب):
#   ① الخانة المجرَّدة من نداءٍ فراغيّ (تصريح أعلى/محلّيّ/إعادة إسناد) —
#      كانت تُفجّر LLVM («null constant») أعلى-مستوًى وتطبع «0» كاذبًا محلّيًّا.
#   ② حارس مستقبِل مدمجات الخرائط — «خريطة_احصل(5، ك)» كانت SIGSEGV مترجَمةً.
#   ③ حقول الأصناف المصنَّفة — «فراغ» كان يعبرها صامتًا في المحرّكين.
#   ④ الكتابة المفهرسة بقيمةٍ فراغ — تكافؤ «لاشيء»/«فراغ» في الخريطة.
#
# التشغيل من جذر المستودع:
#   bash tests/behavior/null_safety/ns10_void_bare_slots_fields_receivers_check.sh
# ============================================================================
#
# ⚠️ **دَينٌ مُقيَّدٌ — هذا السكربتُ لا يُنادَى من أيِّ بوّابةٍ ولا سيرِ عمل.**
#
#      تسعةُ نداءاتٍ للمفسّرِ واثنانِ للمترجم. قِيس ٢٠٢٦-٠٩-٠٤: **لا يبدأُ أصلًا** — يشترطُ وجودَ `sad-run` فيخرجُ برسالةِ «ابنِ sad-run وsad-build أولًا»، وهي تأمرُ المستخدمَ ببناءِ هدفٍ لا وجودَ له. فصفرُ توكيدٍ يُقاس.
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
RUN="$ROOT/build/bin/Debug/sad-run.exe"
BUILD="$ROOT/build/bin/Debug/sad-build.exe"
TMP="${TMPDIR:-/tmp}/ns10_$$"
mkdir -p "$TMP"
trap 'rm -rf "$TMP"' EXIT

# (AR) فشلٌ مبهمٌ يصير رسالةً واضحة — الثنائيان شرطُ القياس كلِّه.
[ -x "$RUN" ] && [ -x "$BUILD" ] || { echo "❌ ابنِ sad-run وsad-build أولًا (build/bin/Debug)"; exit 2; }

PASS=0; FAIL=0
ok()   { PASS=$((PASS+1)); echo "  ✅ $1"; }
bad()  { FAIL=$((FAIL+1)); echo "  ❌ $1"; }
check() { # check <وصف> <شرط صحيح؟>
  if [ "$2" = "1" ]; then ok "$1"; else bad "$1"; fi
}

# ---------------------------------------------------------------- عيّنات
cat > "$TMP/bare_top.ص" <<'EOF'
دالة فراغية()
	اطبع("جانب")
نهاية

متغير س = فراغية()
اطبع(س)
اطبع(نوع(س))
اطبع(س == لاشيء)
EOF

cat > "$TMP/bare_local.ص" <<'EOF'
دالة فراغية()
	اطبع("جانب")
نهاية

دالة عمل()
	متغير س = فراغية()
	اطبع(س)
	اطبع(نوع(س))
نهاية

عمل()
EOF

cat > "$TMP/bare_reassign.ص" <<'EOF'
دالة فراغية()
	اطبع("جانب")
نهاية

متغير س = 1
س = فراغية()
اطبع(س)
اطبع(نوع(س))
EOF

cat > "$TMP/bare_local_reassign.ص" <<'EOF'
دالة فراغية()
	اطبع("جانب")
نهاية

دالة عمل()
	متغير س = 1
	س = فراغية()
	اطبع(س)
	اطبع(نوع(س))
نهاية

عمل()
EOF

cat > "$TMP/idx_array_void.ص" <<'EOF'
دالة فراغية()
	اطبع("جانب")
نهاية

متغير قائمة = [1، 2، 3]
قائمة[1] = فراغية()
اطبع(قائمة[1])
اطبع("بعدها")
EOF

cat > "$TMP/recv_literal.ص" <<'EOF'
استورد خرائط

متغير ن = خريطة_احصل(5، "مفتاح")
اطبع(ن)
EOF

cat > "$TMP/recv_string_var.ص" <<'EOF'
استورد خرائط

متغير م = "نص"
اطبع(خريطة_حجم(م))
EOF

cat > "$TMP/recv_map_via_param.ص" <<'EOF'
استورد خرائط

دالة اقرأ(سجل، مفتاح)
	ارجع خريطة_احصل(سجل، مفتاح)
نهاية

متغير بيانات = {"اسم": "سعاد"}
اطبع(اقرأ(بيانات، "اسم"))
EOF

cat > "$TMP/recv_dyn_nonmap.ص" <<'EOF'
استورد خرائط

متغير سجل = {"أ": 1}
متغير غائب = خريطة_احصل(سجل، "لا")
اطبع(خريطة_حجم(غائب))
EOF

cat > "$TMP/field_this.ص" <<'EOF'
دالة فراغية()
	اطبع("جانب")
نهاية

صنف عداد
	رقم قيمة

	دالة بناء()
		هذا.قيمة = 7
	نهاية

	دالة خرب()
		هذا.قيمة = فراغية()
	نهاية
نهاية

متغير ع = عداد()
ع.خرب()
اطبع("بعدها")
EOF

cat > "$TMP/field_var_obj.ص" <<'EOF'
دالة فراغية()
	اطبع("جانب")
نهاية

صنف نقطة
	رقم س

	دالة بناء()
		هذا.س = 3
	نهاية
نهاية

متغير ن = نقطة()
ن.س = فراغية()
اطبع("بعدها")
EOF

cat > "$TMP/field_bare_ok.ص" <<'EOF'
دالة فراغية()
	اطبع("جانب")
نهاية

صنف حقيبة
	متغير محتوى

	دالة بناء()
		هذا.محتوى = 1
	نهاية
نهاية

متغير ح = حقيبة()
ح.محتوى = فراغية()
اطبع("بعدها")
EOF

cat > "$TMP/idx_map_void.ص" <<'EOF'
استورد خرائط

دالة فراغية()
	اطبع("جانب")
نهاية

متغير سجل = {"أ": 1}
سجل["أ"] = فراغية()
اطبع(سجل["أ"])
اطبع(نوع(سجل["أ"]))
EOF

cat > "$TMP/idx_map_normal.ص" <<'EOF'
استورد خرائط

متغير سجل = {"أ": 1}
سجل["ب"] = 2
اطبع(خريطة_حجم(سجل))
اطبع(سجل["ب"])
EOF

bld() { # bld <اسم> → brc في $?، الثنائي في $TMP/<اسم>.exe
  "$BUILD" "$TMP/$1.ص" -o "$TMP/$1.exe" > "$TMP/$1.blog" 2>&1
}

echo "═══ ① الخانة المجرَّدة من نداءٍ فراغيّ (تكافؤ مزدوج) ═══"
for s in bare_top bare_local bare_reassign bare_local_reassign; do
  iout=$("$RUN" "$TMP/$s.ص" 2>/dev/null)
  bld "$s"; brc=$?
  check "$s: الترجمة تنجح (كان انهيار/كذب)" "$([ $brc -eq 0 ] && echo 1 || echo 0)"
  if [ $brc -eq 0 ]; then
    cout=$("$TMP/$s.exe" 2>/dev/null)
    check "$s: تطابق حرفيّ مفسّر/مترجَم" "$([ "$iout" = "$cout" ] && echo 1 || echo 0)"
    check "$s: المخرَج يحمل «لاشيء» لا «0»" "$(echo "$cout" | grep -q "لاشيء" && ! echo "$cout" | grep -qx "0" && echo 1 || echo 0)"
  fi
done
iout=$("$RUN" "$TMP/bare_top.ص" 2>/dev/null)
check "bare_top: المفسّر — نوع()=فراغ و==لاشيء خطأ" "$(echo "$iout" | grep -q "فراغ" && echo "$iout" | grep -q "خطأ" && echo 1 || echo 0)"

echo "═══ ② حارس مستقبِل مدمجات الخرائط ═══"
bld recv_literal
check "ثابت عددي مستقبِلًا ⇒ رفض ترجمة" "$([ $? -ne 0 ] && grep -q "ليس خريطةً" "$TMP/recv_literal.blog" && echo 1 || echo 0)"
bld recv_string_var
check "متغيّر نصّيّ مستقبِلًا ⇒ رفض ترجمة" "$([ $? -ne 0 ] && grep -q "ليس خريطةً" "$TMP/recv_string_var.blog" && echo 1 || echo 0)"
bld recv_map_via_param
if [ $? -eq 0 ]; then
  out=$("$TMP/recv_map_via_param.exe" 2>/dev/null)
  check "شاهد سلبيّ: خريطة عبر معاملٍ غير مصنَّف تعمل (لا إيجابيّ كاذب)" "$([ "$out" = "سعاد" ] && echo 1 || echo 0)"
else
  bad "شاهد سلبيّ: خريطة عبر معامل — فشلت الترجمة"
fi
bld recv_dyn_nonmap
if [ $? -eq 0 ]; then
  out=$("$TMP/recv_dyn_nonmap.exe" 2>&1)
  # (AR) 2026-08-23 — توحيد RUN037: المستقبِلُ هنا **غيابٌ موسومٌ** (مفتاحٌ
  #      غائبٌ من خريطة_احصل)، والمفسّرُ يرفع عليه RUN037 باسمِ المدمجة —
  #      فصار المترجَمُ يطابقه بدل رسالةِ «ليست خريطة» غيرِ المرمَّزةِ التي
  #      كان هذا الفحصُ يثبّتها. مقصدُ الفحصِ باقٍ: خطأٌ نظيفٌ لا SIGSEGV.
  # (EN) RUN037 unification: the receiver is TAGGED ABSENCE, and the
  #      interpreter raises RUN037 with the builtin's name — the compiled
  #      binary now matches it instead of the uncoded "not a map" line this
  #      check used to pin. The intent stands: a clean error, no SIGSEGV.
  check "مستقبِل موسوم غائب ⇒ RUN037 باسم المدمجة (مطابقة المفسّر) لا SIGSEGV" "$(echo "$out" | grep -q "خريطة_حجم" && echo "$out" | grep -q "بوسائط ناقصة أو من نوع خاطئ" && echo 1 || echo 0)"
else
  bad "مستقبِل موسوم — فشلت الترجمة"
fi

echo "═══ ③ حقول الأصناف المصنَّفة ═══"
iout=$("$RUN" "$TMP/field_this.ص" 2>&1)
check "هذا.حقل مصنَّف (افتراضيّ): تحذير SEM045 ويستمرّ" "$(echo "$iout" | grep -q "SEM045" && echo "$iout" | grep -q "بعدها" && echo 1 || echo 0)"
"$RUN" --إنتاج "$TMP/field_this.ص" > "$TMP/ft_prod.out" 2>&1
prc=$?
check "هذا.حقل مصنَّف (--إنتاج): إيقاف" "$([ $prc -ne 0 ] && ! grep -q "بعدها" "$TMP/ft_prod.out" && echo 1 || echo 0)"
gout=$("$RUN" --جامع "$TMP/field_this.ص" 2>&1)
check "هذا.حقل مصنَّف (--جامع): صمت" "$(! echo "$gout" | grep -q "SEM045" && echo "$gout" | grep -q "بعدها" && echo 1 || echo 0)"
bld field_this
frc=$?
# (AR) لا يكفي rc≠0 — فشلُ ترجمةٍ لأيّ سببٍ آخرَ كان سيُخضّر الفحصَ كذبًا؛
#      يُشترَط نصُّ خطأ النوع نفسُه (رصدُ مراجعة الجودة).
check "هذا.حقل مصنَّف مترجَمًا: رفضُ الفاحص بخطأ النوع" "$([ $frc -ne 0 ] && grep -q "عدم تطابق الأنواع" "$TMP/field_this.blog" && echo 1 || echo 0)"
iout=$("$RUN" "$TMP/field_var_obj.ص" 2>&1)
check "كائن.حقل مصنَّف (مفسّر): تحذير SEM045" "$(echo "$iout" | grep -q "SEM045" && echo 1 || echo 0)"
iout=$("$RUN" "$TMP/field_bare_ok.ص" 2>&1)
check "شاهد سلبيّ: حقل مجرَّد يقبل الفراغ بلا SEM045" "$(! echo "$iout" | grep -q "SEM045" && echo "$iout" | grep -q "بعدها" && echo 1 || echo 0)"

echo "═══ ④ الكتابة المفهرسة بقيمة فراغ ═══"
iout=$("$RUN" "$TMP/idx_map_void.ص" 2>/dev/null)
bld idx_map_void
if [ $? -eq 0 ]; then
  cout=$("$TMP/idx_map_void.exe" 2>/dev/null)
  check "خريطة[ك] = فراغيّة(): تطابق مفسّر/مترجَم" "$([ "$iout" = "$cout" ] && echo 1 || echo 0)"
  check "القراءة «لاشيء» ونوعها «فراغ»" "$(echo "$cout" | grep -q "لاشيء" && echo "$cout" | grep -q "فراغ" && echo 1 || echo 0)"
else
  bad "idx_map_void: فشلت الترجمة"
fi
bld idx_map_normal
if [ $? -eq 0 ]; then
  out=$("$TMP/idx_map_normal.exe" 2>/dev/null)
  check "شاهد سلبيّ: الكتابة المفهرسة العاديّة سليمة" "$(echo "$out" | grep -q "2" && echo 1 || echo 0)"
else
  bad "idx_map_normal: فشلت الترجمة"
fi
# (AR) المصفوفة المتجانسة حدٌّ مُعلَن (خانات خام بلا وسم): يُثبَت أنها لا
#      تنهار مترجَمةً — القيمةُ «0» الكاذبة هي السلوك المقيس المُعلَن اليوم.
bld idx_array_void
if [ $? -eq 0 ]; then
  aout=$("$TMP/idx_array_void.exe" 2>&1); arc=$?
  check "حدّ معلن: مصفوفة[ف] = فراغيّة() لا تنهار مترجَمةً" "$([ $arc -eq 0 ] && echo "$aout" | grep -q "بعدها" && echo 1 || echo 0)"
else
  bad "idx_array_void: فشلت الترجمة"
fi

echo ""
echo "═══════════════════════════════════"
echo "النتيجة: نجح $PASS / فشل $FAIL"
echo "═══════════════════════════════════"
[ $FAIL -eq 0 ]
