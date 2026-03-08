#!/bin/bash
# ═══════════════════════════════════════════════════════════════
# سكريبت التوقيع الرقمي لـ macOS — لغة ص
# macOS Code Signing Script for Sad Programming Language
# ═══════════════════════════════════════════════════════════════
#
# الاستخدام:
#   ./codesign.sh <pkg_file> [DEVELOPER_ID]
#   ./codesign.sh ../output/sad-v1.0.0-macos.pkg "Developer ID Installer: Name (TEAMID)"
#
# المتطلبات:
#   - Apple Developer ID certificate مثبتة في Keychain
#   - Xcode CLI tools
#   - اتصال بالإنترنت (للتوثيق/Notarization)
#
# ═══════════════════════════════════════════════════════════════

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# === التحقق من المعاملات ===
if [ $# -lt 1 ]; then
    echo "الاستخدام: $0 <pkg_file> [DEVELOPER_ID]"
    echo "مثال: $0 ../output/sad-v1.0.0-macos.pkg"
    exit 1
fi

PKG_FILE="$1"
DEVELOPER_ID="${2:-}"

if [ ! -f "$PKG_FILE" ]; then
    echo "✗ الملف غير موجود: $PKG_FILE"
    exit 1
fi

echo "╔══════════════════════════════════════╗"
echo "║   التوقيع الرقمي — لغة ص           ║"
echo "╚══════════════════════════════════════╝"

# === البحث عن شهادة Developer ID ===
if [ -z "$DEVELOPER_ID" ]; then
    echo "→ البحث عن شهادات Developer ID..."
    CERTS=$(security find-identity -v -p basic 2>/dev/null | grep "Developer ID Installer" || true)
    
    if [ -z "$CERTS" ]; then
        echo "⚠️ لم يتم العثور على شهادة Developer ID Installer."
        echo "   لتثبيت شهادة:"
        echo "   1. سجل في https://developer.apple.com"
        echo "   2. أنشئ شهادة Developer ID Installer"
        echo "   3. ثبتها في Keychain Access"
        exit 1
    fi
    
    echo "الشهادات المتاحة:"
    echo "$CERTS"
    
    # استخدام أول شهادة
    DEVELOPER_ID=$(echo "$CERTS" | head -1 | sed 's/.*"\(.*\)".*/\1/')
    echo ""
    echo "→ استخدام: $DEVELOPER_ID"
fi

# === توقيع الحزمة ===
echo "→ توقيع الحزمة..."
SIGNED_PKG="${PKG_FILE%.pkg}-signed.pkg"

productsign \
    --sign "$DEVELOPER_ID" \
    "$PKG_FILE" \
    "$SIGNED_PKG"

echo "  ✓ تم التوقيع: $SIGNED_PKG"

# === التحقق من التوقيع ===
echo ""
echo "→ التحقق من التوقيع..."
pkgutil --check-signature "$SIGNED_PKG"

# === Notarization (اختياري) ===
echo ""
echo "─────────────────────────────────────"
echo "التوثيق مع Apple (Notarization):"
echo ""
echo "لتوثيق الحزمة مع Apple (مطلوب لـ macOS Ventura+):"
echo ""
echo "  # إعداد بيانات الاعتماد (مرة واحدة)"
echo "  xcrun notarytool store-credentials \"SAD_NOTARY\" \\"
echo "    --apple-id \"your@email.com\" \\"
echo "    --team-id \"TEAM_ID\" \\"
echo "    --password \"app-specific-password\""
echo ""
echo "  # إرسال للتوثيق"
echo "  xcrun notarytool submit \"$SIGNED_PKG\" \\"
echo "    --keychain-profile \"SAD_NOTARY\" \\"
echo "    --wait"
echo ""
echo "  # تثبيت stamp التوثيق"
echo "  xcrun stapler staple \"$SIGNED_PKG\""
echo "─────────────────────────────────────"

echo ""
echo "╔══════════════════════════════════════╗"
echo "║   ✓ تم التوقيع بنجاح!              ║"
echo "╚══════════════════════════════════════╝"
echo "الملف الموقّع: $SIGNED_PKG"
