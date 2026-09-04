#!/bin/bash
# ═══════════════════════════════════════════════════════════════
# سكريبت بناء حزمة macOS (.pkg) للغة ص
# macOS Package Builder for Sad Programming Language
# ═══════════════════════════════════════════════════════════════
#
# الاستخدام:
#   ./build-pkg.sh [VERSION]
#   ./build-pkg.sh 1.0.0
#
# المتطلبات:
#   - macOS 12+
#   - CMake 3.15+
#   - pkgbuild, productbuild (مدمجة مع Xcode CLI tools)
#
# ═══════════════════════════════════════════════════════════════

set -euo pipefail

# === إعدادات ===
VERSION="${1:-1.0.0}"
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build-macos"
STAGING_DIR="$BUILD_DIR/staging"
PKG_ROOT="$BUILD_DIR/pkg-root"
OUTPUT_DIR="$PROJECT_ROOT/distribution/output"
IDENTIFIER="org.sad-lang.sad"
INSTALL_PREFIX="/usr/local"

echo "╔══════════════════════════════════════╗"
echo "║   بناء حزمة macOS — لغة ص v$VERSION  ║"
echo "╚══════════════════════════════════════╝"

# === تنظيف وإعداد ===
echo ""
echo "→ تنظيف مجلدات البناء..."
rm -rf "$BUILD_DIR"
mkdir -p "$STAGING_DIR" "$PKG_ROOT" "$OUTPUT_DIR"

# === بناء المشروع ===
echo "→ بناء المشروع بـ CMake..."
cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR/cmake-build" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"

# (AR) 🔑 كان الهدفُ `sad-run` — المحرّكُ المحذوف. والسطرُ ١٥ فيه
#      `set -euo pipefail`، فكانت حزمةُ macOS **تموتُ عند أوّلِ خطوةِ بناء**
#      ولا تصلُ إلى شيءٍ ممّا بعدَها. ⚠️ ولم يظهرْ ذلك في CI لأنّ هذا
#      السكربتَ لا يُنادى من أيِّ مجرًى — يُخفِقُ تحتَ يدٍ بشريّةٍ وحدَها.
#      والمبنيُّ الآن ما يُنسَخُ فعلًا أدناه: `sad` (المركز) و`sad-build`.
# (EN) The target was sad-run, the deleted engine, and line 15 sets
#      `set -euo pipefail` — so the macOS package died at its first build step
#      and reached nothing after it. CI never showed this because no workflow
#      invokes this script; it fails only under a human hand. The hub and the
#      compiler are built here and mandatorily copied below.
cmake --build "$BUILD_DIR/cmake-build" --config Release --target sad sad-build -- -j"$(sysctl -n hw.ncpu)"

# === إعداد هيكل الحزمة ===
echo "→ إعداد هيكل الحزمة..."

# bin/
# (AR) 🔑 كان هنا نسخُ `sad` وحدَه بـ`|| true` — والمترجّمُ يُبنى فوقُ ثمّ
#      يُهمَل. وأخطرُ من النقصِ أنّ `|| true` **يبتلعُ غيابَه**: تنجحُ الحزمةُ
#      وتُشحَنُ بلا مترجّم، وصفحةُ الدليلِ أسفلَه تقولُ للمستخدمِ `sad build`.
#      فالإخفاقُ العالي صار صامتًا. والنسخُ الآن **مُلزِمٌ** لِما لا حزمةَ بدونه.
#      و`sadc` لقبُ `sad-build` المنشور (scripts/ci/release_tools.sh).
# (EN) Only `sad` was copied, with `|| true` — the compiler was built above and
#      then dropped. Worse than the omission: `|| true` swallowed its absence, so
#      the package succeeded and shipped with no compiler while the man page five
#      lines down tells the user to run `sad build`. A loud failure had become a
#      silent one. These two copies are now mandatory; sadc is sad-build's
#      published alias per scripts/ci/release_tools.sh.
mkdir -p "$PKG_ROOT$INSTALL_PREFIX/bin"
for _bin in sad sad-build; do
    if [ ! -f "$BUILD_DIR/cmake-build/bin/$_bin" ]; then
        echo "✗ لم يُبنَ '$_bin' — لا تُحزَّم حزمةٌ ناقصة" >&2
        exit 1
    fi
    cp "$BUILD_DIR/cmake-build/bin/$_bin" "$PKG_ROOT$INSTALL_PREFIX/bin/"
done
cp "$PKG_ROOT$INSTALL_PREFIX/bin/sad-build" "$PKG_ROOT$INSTALL_PREFIX/bin/sadc"
for _bin in sad-lsp sad-fmt sad-pkg sad-check; do
    [ -f "$BUILD_DIR/cmake-build/bin/$_bin" ] &&
        cp "$BUILD_DIR/cmake-build/bin/$_bin" "$PKG_ROOT$INSTALL_PREFIX/bin/"
done

# stdlib/
mkdir -p "$PKG_ROOT$INSTALL_PREFIX/lib/sad/stdlib"
if [ -d "$PROJECT_ROOT/stdlib" ]; then
    cp -r "$PROJECT_ROOT/stdlib/"* "$PKG_ROOT$INSTALL_PREFIX/lib/sad/stdlib/" 2>/dev/null || true
    cp -r "$PROJECT_ROOT/features/graphics/stdlib/"* "$PKG_ROOT$INSTALL_PREFIX/lib/sad/stdlib/" 2>/dev/null || true
fi

# man page
mkdir -p "$PKG_ROOT$INSTALL_PREFIX/share/man/man1"
cat > "$PKG_ROOT$INSTALL_PREFIX/share/man/man1/sad.1" << 'MANEOF'
.TH SAD 1 "2026" "v1.0.0" "Sad Programming Language"
.SH NAME
sad \- مركز أدوات لغة ص / Sad Arabic Programming Language tool hub
.SH SYNOPSIS
.B sad
\fIcommand\fR [\fIoptions\fR] [\fIfile.ص\fR]
.SH DESCRIPTION
لغة ص هي لغة برمجة عربية حديثة.
.SH OPTIONS
.TP
.B \-\-help
عرض المساعدة
.TP
.B \-\-version
عرض الإصدار
.SH EXAMPLES
.B sad build script.ص
.SH AUTHORS
فريق لغة ص
MANEOF

# === بناء الحزمة ===
echo "→ بناء component package..."
pkgbuild \
    --root "$PKG_ROOT" \
    --identifier "$IDENTIFIER" \
    --version "$VERSION" \
    --install-location "/" \
    "$STAGING_DIR/sad-core.pkg"

# === إعداد Distribution XML ===
echo "→ إنشاء Distribution XML..."
cat > "$STAGING_DIR/distribution.xml" << DISTEOF
<?xml version="1.0" encoding="utf-8"?>
<installer-gui-script minSpecVersion="2">
    <title>لغة ص — Sad Programming Language v$VERSION</title>
    <organization>$IDENTIFIER</organization>
    <domains enable_localSystem="true"/>
    <options customize="never" require-scripts="false"/>
    
    <welcome language="ar" mime-type="text/html"><![CDATA[
        <html dir="rtl"><body>
        <h1>مرحباً بك في لغة ص</h1>
        <p>الإصدار $VERSION</p>
        <p>لغة ص هي لغة برمجة عربية حديثة مطورة بـ C++17.</p>
        <p>سيتم تثبيت:</p>
        <ul>
            <li>المفسر <code>sad</code></li>
            <li>المكتبة القياسية</li>
        </ul>
        </body></html>
    ]]></welcome>
    
    <conclusion language="ar" mime-type="text/html"><![CDATA[
        <html dir="rtl"><body>
        <h1>تم التثبيت بنجاح! ✓</h1>
        <p>للبدء، افتح Terminal واكتب:</p>
        <pre>sad --help</pre>
        <pre>sad script.ص</pre>
        </body></html>
    ]]></conclusion>
    
    <choices-outline>
        <line choice="default"/>
    </choices-outline>
    
    <choice id="default" title="لغة ص">
        <pkg-ref id="$IDENTIFIER"/>
    </choice>
    
    <pkg-ref id="$IDENTIFIER" version="$VERSION" onConclusion="none">sad-core.pkg</pkg-ref>
</installer-gui-script>
DISTEOF

# === بناء الحزمة النهائية ===
PKG_NAME="sad-v${VERSION}-macos.pkg"
echo "→ بناء الحزمة النهائية: $PKG_NAME"
productbuild \
    --distribution "$STAGING_DIR/distribution.xml" \
    --package-path "$STAGING_DIR" \
    "$OUTPUT_DIR/$PKG_NAME"

# === التحقق ===
echo ""
echo "→ التحقق من الحزمة..."
pkgutil --check-signature "$OUTPUT_DIR/$PKG_NAME" 2>/dev/null || echo "  (غير موقّعة — استخدم codesign.sh للتوقيع)"

PKG_SIZE=$(du -h "$OUTPUT_DIR/$PKG_NAME" | cut -f1)

echo ""
echo "╔══════════════════════════════════════╗"
echo "║   ✓ تم بناء الحزمة بنجاح!          ║"
echo "╚══════════════════════════════════════╝"
echo ""
echo "الملف: $OUTPUT_DIR/$PKG_NAME"
echo "الحجم: $PKG_SIZE"
echo ""
echo "للتثبيت:"
echo "  sudo installer -pkg $OUTPUT_DIR/$PKG_NAME -target /"
echo ""
echo "للتوقيع الرقمي:"
echo "  ./codesign.sh $OUTPUT_DIR/$PKG_NAME"
