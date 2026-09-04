#!/bin/bash
# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# سكريبت بناء حزمة .deb — لغة ص / Sad Programming Language
# ═══════════════════════════════════════════════════════════════════════════════
#
# الاستخدام / Usage:
#   ./build-deb.sh 1.0.0 x86_64
#   ./build-deb.sh 1.0.0 aarch64
#
# المتطلبات:
#   - dpkg-deb (مثبت افتراضياً على Debian/Ubuntu)
#   - ملفات لغة ص المبنية
#
# النتيجة:
#   sad-lang_1.0.0_amd64.deb أو sad-lang_1.0.0_arm64.deb
#
# التثبيت:
#   sudo dpkg -i sad-lang_1.0.0_amd64.deb
#   أو: sudo apt install ./sad-lang_1.0.0_amd64.deb
#
# ═══════════════════════════════════════════════════════════════════════════════

set -e

VERSION="${1:-1.0.0}"
ARCH="${2:-x86_64}"
PACKAGE_NAME="sad-lang"

# تحويل بنية المعالج لتنسيق Debian
case "$ARCH" in
    x86_64|amd64)   DEB_ARCH="amd64" ;;
    aarch64|arm64)  DEB_ARCH="arm64" ;;
    armv7l|armhf)   DEB_ARCH="armhf" ;;
    *) echo "بنية غير مدعومة: $ARCH"; exit 1 ;;
esac

BUILD_DIR="${PACKAGE_NAME}_${VERSION}_${DEB_ARCH}"
echo "══════════════════════════════════════════════"
echo "  بناء حزمة deb: ${BUILD_DIR}.deb"
echo "══════════════════════════════════════════════"

# تنظيف المجلد السابق
rm -rf "$BUILD_DIR"

# ── إنشاء هيكل الحزمة ──
mkdir -p "$BUILD_DIR/DEBIAN"
mkdir -p "$BUILD_DIR/usr/bin"
mkdir -p "$BUILD_DIR/usr/share/sad-lang/stdlib"
mkdir -p "$BUILD_DIR/usr/share/doc/sad-lang"
mkdir -p "$BUILD_DIR/usr/share/man/man1"

# ── ملف التحكم / Control File ──
cat > "$BUILD_DIR/DEBIAN/control" <<EOF
Package: ${PACKAGE_NAME}
Version: ${VERSION}
Section: devel
Priority: optional
Architecture: ${DEB_ARCH}
Maintainer: Sad Language Team <team@sad-lang.org>
Homepage: https://github.com/sad-lang/sad-language
Description: لغة ص — لغة برمجة عربية حديثة
 لغة ص (Sad) هي لغة برمجة عربية حديثة مبنية بـ C++17.
 مكتبة قياسية غنية، خادم LSP، مدير حزم، وأداة تنسيق.
 .
 المكونات المتضمنة:
  - sad: مركز الأدوات — يُنادي البقيّة (sad build، sad check…)
  - sad-pkg: مدير الحزم
  - sad-lsp: خادم LSP للمحررات
 .
 لتثبيت المترجم (sad-build/sadc)، ثبّت حزمة sad-lang-compiler.
Depends: libc6 (>= 2.31), libstdc++6 (>= 10)
Recommends: sad-lang-compiler
EOF

# ── سكريبت ما بعد التثبيت / Post-Install Script ──
cat > "$BUILD_DIR/DEBIAN/postinst" <<'EOF'
#!/bin/sh
set -e
echo ""
echo "╔═══════════════════════════════════════════════╗"
echo "║     لغة ص — تم التثبيت بنجاح!                ║"
echo "╚═══════════════════════════════════════════════╝"
echo ""
echo "  sad --help              عرض المساعدة"
echo "  sad script.ص           تشغيل ملف"
echo "  sad-pkg init            إنشاء مشروع جديد"
echo ""
EOF
chmod 755 "$BUILD_DIR/DEBIAN/postinst"

# ── سكريبت ما بعد الإزالة / Post-Remove Script ──
cat > "$BUILD_DIR/DEBIAN/postrm" <<'EOF'
#!/bin/sh
set -e
if [ "$1" = "purge" ]; then
    rm -rf /usr/share/sad-lang
fi
EOF
chmod 755 "$BUILD_DIR/DEBIAN/postrm"

# ── نسخ الملفات التنفيذية ──
echo "  نسخ الملفات التنفيذية..."

# المصدر: مجلد البناء أو مجلد الإصدار
SRC_BIN="${SRC_DIR:-../../build/bin}"

for bin_file in sad sad-lsp sad-check sadc sad-build sad-pkg sad-fmt; do
    if [ -f "$SRC_BIN/$bin_file" ]; then
        cp "$SRC_BIN/$bin_file" "$BUILD_DIR/usr/bin/"
        chmod 755 "$BUILD_DIR/usr/bin/$bin_file"
        echo "    ✓ $bin_file"
    fi
done

# ── نسخ المكتبة القياسية ──
echo "  نسخ المكتبة القياسية..."
SRC_STDLIB="${SRC_DIR:-../../stdlib}"
if [ -d "$SRC_STDLIB" ]; then
    cp -r "$SRC_STDLIB"/* "$BUILD_DIR/usr/share/sad-lang/stdlib/" 2>/dev/null || true
    echo "    ✓ stdlib"
fi
# (AR) وحدات stdlib الخاصّة بالميزات (RFC #19): تُدمَج في نفس مجلّد stdlib المُوزَّع
# (EN) Per-feature stdlib modules (RFC #19): merged into the same shipped stdlib dir
FEAT_STDLIB="$(dirname "$SRC_STDLIB")/features/graphics/stdlib"
if [ -d "$FEAT_STDLIB" ]; then
    cp -r "$FEAT_STDLIB"/* "$BUILD_DIR/usr/share/sad-lang/stdlib/" 2>/dev/null || true
    echo "    ✓ features/graphics/stdlib (رسومات)"
fi

# ── نسخ التوثيق ──
if [ -f "../../README.md" ]; then
    cp "../../README.md" "$BUILD_DIR/usr/share/doc/sad-lang/"
fi

# ── ملف حقوق النشر / Copyright ──
cat > "$BUILD_DIR/usr/share/doc/sad-lang/copyright" <<EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: sad-lang
Upstream-Contact: team@sad-lang.org
Source: https://github.com/sad-lang/sad-language

Files: *
Copyright: 2024 Sad Language Team
License: MIT
EOF

# ── بناء الحزمة ──
echo "  بناء الحزمة..."
dpkg-deb --build --root-owner-group "$BUILD_DIR"

SIZE=$(du -h "${BUILD_DIR}.deb" | cut -f1)
echo ""
echo "══════════════════════════════════════════════"
echo "  ✓ تم بناء: ${BUILD_DIR}.deb ($SIZE)"
echo "══════════════════════════════════════════════"
echo ""
echo "  للتثبيت:"
echo "    sudo dpkg -i ${BUILD_DIR}.deb"
echo "    # أو"
echo "    sudo apt install ./${BUILD_DIR}.deb"
echo ""

# تنظيف
rm -rf "$BUILD_DIR"
