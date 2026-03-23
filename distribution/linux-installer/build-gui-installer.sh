#!/bin/bash
# ═══════════════════════════════════════════════════════════════════════════════
# سكريبت بناء المثبت الرسومي — Build GUI Installer
# ═══════════════════════════════════════════════════════════════════════════════
# يدمج سكريبت المثبت مع tarball البيانات في ملف واحد ذاتي الاستخراج
#
# الاستخدام: ./build-gui-installer.sh
# الناتج:   sad-installer-1.0.0-linux-x64.sh
# ═══════════════════════════════════════════════════════════════════════════════

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
VERSION="1.0.0"
TARBALL="${SCRIPT_DIR}/../output/sad-1.0.0-linux-x64.tar.gz"
INSTALLER_SCRIPT="${SCRIPT_DIR}/sad-installer.sh"
OUTPUT="${SCRIPT_DIR}/../output/sad-installer-${VERSION}-linux-x64.sh"

if [ ! -f "$TARBALL" ]; then
    echo "خطأ: لم يتم العثور على الأرشيف: $TARBALL"
    echo "قم ببناء الأرشيف أولاً."
    exit 1
fi

if [ ! -f "$INSTALLER_SCRIPT" ]; then
    echo "خطأ: لم يتم العثور على سكريبت المثبت: $INSTALLER_SCRIPT"
    exit 1
fi

echo "═══════════════════════════════════════════"
echo "  بناء المثبت الرسومي v${VERSION}"
echo "═══════════════════════════════════════════"

# دمج السكريبت مع الأرشيف (تحويل لأسطر Unix أولاً)
sed 's/\r$//' "$INSTALLER_SCRIPT" > "${OUTPUT}.tmp"
cat "${OUTPUT}.tmp" "$TARBALL" > "$OUTPUT"
rm -f "${OUTPUT}.tmp"

echo ""
echo "  ✓ حجم السكريبت: $(wc -c < "$INSTALLER_SCRIPT") بايت"
echo "  ✓ حجم الأرشيف:  $(wc -c < "$TARBALL") بايت"
echo "  ✓ حجم المثبت:   $(wc -c < "$OUTPUT") بايت"
echo ""
echo "  الملف: $OUTPUT"
echo ""
echo "  للتثبيت:"
echo "    bash $OUTPUT"
echo "═══════════════════════════════════════════"
