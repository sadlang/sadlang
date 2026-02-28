#!/bin/bash
# ============================================================================
# قيقم.sh — سكريبت تشغيل QEMU لنظام النحلة BeeOS
# BeeOS QEMU Launch Script
#
# الاستخدام:
#   ./قيقم.sh            ← تشغيل عادي
#   ./قيقم.sh --debug    ← مع GDB stub
#   ./قيقم.sh --nogfx    ← بدون واجهة رسومية (serial فقط)
#
# بسم الله الرحمن الرحيم
# ============================================================================

set -e

# ============================================================================
# ثوابت
# ============================================================================

KERNEL="build/BeeOS.elf"
QEMU="qemu-system-x86_64"
RAM="128M"
CORES="2"
OVMF="/usr/share/OVMF/OVMF_CODE.fd"

# ألوان
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # بدون لون

# ============================================================================
# دوال المساعدة
# ============================================================================

bee_print() {
    echo -e "${GREEN}[🐝 BeeOS]${NC} $1"
}

bee_error() {
    echo -e "${RED}[✗ خطأ]${NC} $1"
    exit 1
}

# ============================================================================
# التحقق
# ============================================================================

bee_print "بِسْمِ اللهِ الرَّحْمٰنِ الرَّحِيمِ"
bee_print "═══════════════════════════════════"
bee_print "  BeeOS QEMU Launcher v0.1.0"
bee_print "═══════════════════════════════════"

# تحقق من وجود QEMU
if ! command -v $QEMU &> /dev/null; then
    bee_error "QEMU غير مثبت! نفّذ: sudo apt install qemu-system-x86"
fi

# بناء النواة إذا لم تكن موجودة
if [ ! -f "$KERNEL" ]; then
    bee_print "بناء النواة..."
    make -C bee_os/kernel all
fi

# ============================================================================
# معاملات QEMU المشتركة
# ============================================================================

QEMU_COMMON=(
    -kernel "$KERNEL"
    -m "$RAM"
    -smp "$CORES"
    -no-reboot
    -no-shutdown
    # VGA/Framebuffer
    -vga std
    -display gtk,zoom-to-fit=on
    # Serial (للتصحيح)
    -serial stdio
    # شبكة e1000
    -netdev user,id=net0
    -device e1000,netdev=net0
    # قرص وهمي (للاختبار)
    -drive format=raw,file=bee_os/disk.img,if=none,id=hd0 2>/dev/null || true
)

# ============================================================================
# تشغيل QEMU حسب الوضع
# ============================================================================

case "${1:-}" in
    --debug)
        bee_print "وضع التصحيح — GDB على المنفذ 1234"
        bee_print "شغّل: gdb -ex 'target remote :1234' bee_os/build/BeeOS.elf"
        $QEMU "${QEMU_COMMON[@]}" \
            -s -S \
            2>&1
        ;;

    --nogfx)
        bee_print "وضع بدون رسوميات — serial فقط"
        $QEMU "${QEMU_COMMON[@]}" \
            -display none \
            -nographic \
            2>&1
        ;;

    --test)
        bee_print "وضع الاختبار التلقائي"
        timeout 30 $QEMU "${QEMU_COMMON[@]}" \
            -display none \
            -nographic \
            2>&1 | head -50
        bee_print "✓ اختبار QEMU اكتمل"
        ;;

    *)
        bee_print "تشغيل BeeOS على QEMU..."
        $QEMU "${QEMU_COMMON[@]}" \
            2>&1
        ;;
esac

bee_print "النظام أُغلق — الله يحفظك 🐝"
