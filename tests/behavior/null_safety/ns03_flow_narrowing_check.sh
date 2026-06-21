#!/usr/bin/env bash
# ============================================================================
# (NS-03) فحص تحليل التدفّق (smart narrowing) عبر المحرّكين — ADR-NS-001 D1/D2/D4/D5
#
# (AR) يؤكّد أن تضييق NS-03 يكبح تحذير NS-04 للوصول الخام **عند إثبات عدم العدم**،
#      ويُبقيه حين لا يُثبَت. يُفحَص التشخيص الساكن (رسالة "غير آمن") في وضع
#      `--learn` على المفسّر **والمترجم** معًا (محور التحليل، لا codegen):
#        - تضييق then:      'إذا (م != لاشيء) { م.عضو }'      → لا تحذير
#        - خروج عكسيّ (D4): 'إذا (م == لاشيء){ارجع} م.عضو'   → لا تحذير
#        - خارج الحرس:                       'م.عضو'         → تحذير
#        - التحوّر (D2):    'إذا(م!=لاشيء){ م=لاشيء؛ م.عضو }' → تحذير
#
# (AR) ملاحظة: نفحص **stderr** للتشخيص الساكن فقط؛ توليد LLVM للوصول على اختياريّ
#      داخل شرط غير منجَز بعد (NS-06)، فلا نعتمد على نجاح بناء المترجم هنا.
# (EN) Asserts NS-03 narrowing suppresses the NS-04 raw-access warning when non-null
#      is proven, and keeps it otherwise — checking the static diagnostic on stderr
#      for both engines (analysis axis; compiler codegen for this case is NS-06).
#
# الاستعمال:  bash tests/behavior/null_safety/ns03_flow_narrowing_check.sh
# ============================================================================
set -u
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
RUN="$ROOT/build/bin/Debug/sad-run.exe"
BUILD="$ROOT/build/bin/Release/sad-build.exe"
TMP="$(mktemp -d)"

THEN="$TMP/then.ص";   printf 'دالة د(م: نص؟)\n    إذا (م != لاشيء)\n        اطبع_سطر(م.الطول)\n    نهاية\nنهاية\n' > "$THEN"
REV="$TMP/rev.ص";     printf 'دالة د(م: نص؟)\n    إذا (م == لاشيء)\n        ارجع\n    نهاية\n    اطبع_سطر(م.الطول)\nنهاية\n' > "$REV"
OUT="$TMP/out.ص";     printf 'دالة د(م: نص؟)\n    اطبع_سطر(م.الطول)\nنهاية\n' > "$OUT"
MUT="$TMP/mut.ص";     printf 'دالة د(م: نص؟)\n    إذا (م != لاشيء)\n        م = لاشيء\n        اطبع_سطر(م.الطول)\n    نهاية\nنهاية\n' > "$MUT"

pass=0; fail=0
check() { if [ "$2" -eq 0 ]; then echo "  ✅ $1"; pass=$((pass+1)); else echo "  ❌ $1"; fail=$((fail+1)); fi; }
# has_warn engine file → 0 إن ظهر تحذير "غير آمن"
has_warn() { "$1" --learn "$2" >/dev/null 2>"$TMP/e"; grep -q "غير آمن" "$TMP/e"; }

for eng_name in "المفسّر:$RUN" "المترجم:$BUILD"; do
  name="${eng_name%%:*}"; eng="${eng_name#*:}"
  echo "== $name =="
  if [ ! -x "$eng" ]; then echo "  (تخطّي — غير موجود)"; continue; fi
  has_warn "$eng" "$THEN"; check "تضييق then: لا تحذير"          $([ $? -ne 0 ] && echo 0 || echo 1)
  has_warn "$eng" "$REV";  check "خروج عكسيّ (D4): لا تحذير"      $([ $? -ne 0 ] && echo 0 || echo 1)
  has_warn "$eng" "$OUT";  check "خارج الحرس: تحذير يظهر"        $?
  has_warn "$eng" "$MUT";  check "التحوّر (D2): تحذير يظهر"       $?
done

rm -rf "$TMP"
echo "── النتيجة: نجح=$pass فشل=$fail ──"
[ "$fail" -eq 0 ]
