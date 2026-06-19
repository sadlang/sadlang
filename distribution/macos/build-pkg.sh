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

cmake --build "$BUILD_DIR/cmake-build" --config Release --target sad-run -- -j"$(sysctl -n hw.ncpu)"

# === إعداد هيكل الحزمة ===
echo "→ إعداد هيكل الحزمة..."

# bin/
mkdir -p "$PKG_ROOT$INSTALL_PREFIX/bin"
cp "$BUILD_DIR/cmake-build/bin/sad" "$PKG_ROOT$INSTALL_PREFIX/bin/" 2>/dev/null || true

# stdlib/
mkdir -p "$PKG_ROOT$INSTALL_PREFIX/lib/sad/stdlib"
if [ -d "$PROJECT_ROOT/stdlib" ]; then
    cp -r "$PROJECT_ROOT/stdlib/"* "$PKG_ROOT$INSTALL_PREFIX/lib/sad/stdlib/" 2>/dev/null || true
fi

# man page
mkdir -p "$PKG_ROOT$INSTALL_PREFIX/share/man/man1"
cat > "$PKG_ROOT$INSTALL_PREFIX/share/man/man1/sad.1" << 'MANEOF'
.TH SAD 1 "2026" "v1.0.0" "Sad Programming Language"
.SH NAME
sad \- مفسر لغة ص / Sad Arabic Programming Language interpreter
.SH SYNOPSIS
.B sad
[\fIoptions\fR] [\fIfile.ص\fR]
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
.B sad script.ص
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
