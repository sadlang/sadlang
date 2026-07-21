#!/usr/bin/env bash
# بسم الله الرحمن الرحيم
# ============================================================================
#  مُعيد إنتاج المقام المرجعيّ لآبلتات صدَفة ص (sad-repl) — إثبات تنفيذيّ
#  Reproducer for the sad-repl applet reference denominator (executable proof)
# ----------------------------------------------------------------------------
#  يغلق ثغرة الثقة: حارس check_applet_coverage.py يقابل applets.yaml ⟷ المرجع
#  المُودَع (busybox_reference.yaml)؛ لكنّ المرجع نفسه مثبَّت بشريًّا + بصمة sha256.
#  هذا السكربت يعيد اشتقاق قائمة الـ403 آبلت من مصدر busybox من الصفر (نفس الوصفة
#  الّتي وُلِّد بها المرجع: make defconfig ⇒ `busybox --list` ⇒ فرز ⇒ إعادة tc)
#  ويقابلها بـdefconfig_applets في المرجع. اختلافٌ ⇒ فشل + فرق مطبوع.
#
#  لماذا محلّيّ/اختياريّ لا في مصفوفة CI الأساسيّة؟ يتطلّب بناء busybox كاملًا (build-
#  essential) بضع دقائق؛ وtc يفشل على ترويسات نواة حديثة (رموز TCA_CBQ_* أُزيلت) —
#  عطبٌ مضيفيّ لا يخصّ عضويّة المجموعة، فنعطّله للبناء ثمّ نعيده يدويًّا (كما وُلِّد
#  المرجع). يصلح للتشغيل عند ترقية إصدار busybox أو للتحقّق الدوريّ.
#
#  الاستعمال:
#     bash scripts/verify_busybox_reference.sh
#  المتطلّبات: gcc make wget tar bzip2 sha256sum nproc (لينكس/WSL). لا يمسّ الشجرة.
# ============================================================================
set -euo pipefail

HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
REF="$HERE/language-truth/tools/repl/busybox_reference.yaml"
[ -f "$REF" ] || { echo "خطأ: ملفّ المرجع مفقود: $REF"; exit 1; }

# --- كلّ النواتج داخل مجلّد مؤقّت خاصّ (يُنظَّف بـtrap؛ لا مسارات /tmp ثابتة تُصادَم
#     أو تُتبَع عبر روابط رمزيّة على مضيف مشترك — CWE-377) --------------------
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT
REFLIST="$WORK/ref_defconfig.txt"
BUILT="$WORK/built_applets.txt"
BUILDLOG="$WORK/bb_build.log"
DIFFOUT="$WORK/ref_diff.txt"

# --- استخراج الحقائق المثبَّتة من المرجع نفسه (مصدر واحد للحقيقة) --------------
BB_VERSION="$(sed -nE 's/^busybox_version:[[:space:]]*"([^"]+)".*/\1/p' "$REF" | head -1)"
BB_SHA256="$(sed -nE 's/^source_sha256:[[:space:]]*"([^"]+)".*/\1/p' "$REF" | head -1)"
[ -n "$BB_VERSION" ] || { echo "خطأ: تعذّر استخراج busybox_version من المرجع"; exit 1; }
[ -n "$BB_SHA256" ]  || { echo "خطأ: تعذّر استخراج source_sha256 من المرجع"; exit 1; }
echo "== المرجع: busybox $BB_VERSION (sha256 مثبَّت) =="

# قائمة defconfig_applets المُودَعة (بين 'defconfig_applets:' و 'extra_allowed:').
# نزع الاقتباس يسبق اقتطاع التعليق (متانة: لو حوى اسمٌ مقتبَسٌ رمز # لا يُبتَر) --
awk '
  /^defconfig_applets:/ { on=1; next }
  /^extra_allowed:/     { on=0 }
  on && /^[[:space:]]*-[[:space:]]/ {
    line=$0
    sub(/^[[:space:]]*-[[:space:]]*/, "", line)
    if (line ~ /^"/) {                              # قيمة مقتبَسة: خذ ما بين أوّل زوج اقتباس
      sub(/^"/, "", line); sub(/".*$/, "", line)
      gsub(/\\"/, "\"", line)
    } else {                                         # قيمة عارية: اقتطع التعليق السطريّ
      sub(/[[:space:]]*#.*$/, "", line)
      sub(/[[:space:]]+$/, "", line)
    }
    print line
  }
' "$REF" | LC_ALL=C sort -u > "$REFLIST"
REF_N="$(wc -l < "$REFLIST")"
echo "== المرجع يحوي $REF_N آبلت defconfig =="

# --- إعادة البناء والاشتقاق ------------------------------------------------
for t in gcc make wget tar bzip2 sha256sum nproc; do
  command -v "$t" >/dev/null 2>&1 || { echo "ينقص أداة البناء: $t"; exit 1; }
done
cd "$WORK"
TARBALL="busybox-${BB_VERSION}.tar.bz2"
echo "== جلب مصدر busybox $BB_VERSION والتحقّق من بصمته =="
wget -q "https://busybox.net/downloads/${TARBALL}" -O "$TARBALL" \
  || { echo "خطأ: فشل تنزيل المصدر من busybox.net"; exit 1; }
GOT="$(sha256sum "$TARBALL" | awk '{print $1}')"
[ "$GOT" = "$BB_SHA256" ] || { echo "بصمة المصدر لا تطابق المرجع! متوقَّع=$BB_SHA256 وصل=$GOT"; exit 1; }
echo "بصمة المصدر مطابقة ✔"
tar xf "$TARBALL" || { echo "خطأ: فشل فكّ الأرشيف"; exit 1; }
cd "busybox-${BB_VERSION}"
echo "== make defconfig =="
make defconfig >/dev/null 2>&1 || { echo "خطأ: فشل make defconfig"; exit 1; }
# tc: آبلت defconfig أصيل يفشل بناؤه على ترويسات نواة حديثة — نعطّله للبناء ثمّ
#     نعيده يدويًّا للقائمة (العطب مضيفيّ لا يخصّ عضويّة المجموعة). oldconfig يقرأ
#     الافتراضات من EOF (</dev/null) — بلا أنبوب `yes` (يتجنّب SIGPIPE مع pipefail).
sed -i 's/^CONFIG_TC=y/# CONFIG_TC is not set/' .config
sed -i 's/^CONFIG_FEATURE_TC_INGRESS=y/# CONFIG_FEATURE_TC_INGRESS is not set/' .config
make oldconfig </dev/null >/dev/null 2>&1 || { echo "خطأ: فشل make oldconfig"; exit 1; }
echo "== بناء busybox (قد يستغرق دقائق) =="
make -j"$(nproc)" >"$BUILDLOG" 2>&1 || { echo "فشل البناء — آخر السطور:"; tail -15 "$BUILDLOG"; exit 1; }
./busybox --list | LC_ALL=C sort -u > "$BUILT"
grep -qx tc "$BUILT" || { echo tc >> "$BUILT"; LC_ALL=C sort -u -o "$BUILT" "$BUILT"; }
BUILT_N="$(wc -l < "$BUILT")"
echo "== البناء الفعليّ أنتج $BUILT_N آبلت (بعد إعادة tc) =="

# --- المقابلة --------------------------------------------------------------
if diff -u "$REFLIST" "$BUILT" > "$DIFFOUT"; then
  echo "✓ المقام المرجعيّ متزامن مع بناء busybox $BB_VERSION الفعليّ: $REF_N/$BUILT_N متطابقة تمامًا."
  exit 0
else
  echo "✗ انحراف! المرجع المُودَع لا يطابق بناء busybox $BB_VERSION الفعليّ:"
  echo "   (- = في المرجع فقط، + = في البناء فقط)"
  cat "$DIFFOUT"
  echo "   الإصلاح: أعِد توليد defconfig_applets في busybox_reference.yaml من قائمة البناء."
  exit 1
fi
