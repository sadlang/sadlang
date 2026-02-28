# ============================================================================
# Makefile — نظام البناء لـ BeeOS
# BeeOS Build System
#
# الأوامر:
#   make all      ← بناء كامل
#   make kernel   ← النواة فقط  
#   make iso      ← صورة ISO للإقلاع
#   make qemu     ← تشغيل فوري على QEMU
#   make debug    ← QEMU + GDB
#   make clean    ← تنظيف
#
# بسم الله الرحمن الرحيم
# ============================================================================

# ── الأدوات ──────────────────────────────────────────────────────────────────
NASM     := nasm
LD       := ld
QEMU     := qemu-system-x86_64
GRUB_MKR := grub-mkrescue
GRUB_CFG := grub.cfg

# ── المسارات ──────────────────────────────────────────────────────────────────
KERNEL_DIR := bee_os/kernel
TRAB_DIR   := $(KERNEL_DIR)/تراب
BUILD_DIR  := bee_os/build
ISO_DIR    := $(BUILD_DIR)/iso

# ── ملفات الإخراج ──────────────────────────────────────────────────────────
KERNEL_BIN := $(BUILD_DIR)/BeeOS.elf
KERNEL_ISO := $(BUILD_DIR)/BeeOS.iso

# ── ملفات المصدر ──────────────────────────────────────────────────────────
ASM_SRCS   := $(TRAB_DIR)/entry.asm
ASM_OBJS   := $(BUILD_DIR)/entry.o
LINKER     := $(TRAB_DIR)/رابط.ld

# ── معاملات NASM ──────────────────────────────────────────────────────────
NASM_FLAGS := -f elf64

# ── معاملات الرابط ──────────────────────────────────────────────────────────
LD_FLAGS   := -m elf_x86_64 -T $(LINKER)

# ── معاملات QEMU ──────────────────────────────────────────────────────────
QEMU_FLAGS := -m 128M -smp 2 -vga std -serial stdio \
              -netdev user,id=n0 -device e1000,netdev=n0 \
              -no-reboot -no-shutdown

# ============================================================================
# الأهداف الرئيسية
# ============================================================================

.PHONY: all kernel iso qemu debug clean test

all: kernel iso

## بناء النواة ELF
kernel: $(BUILD_DIR) $(KERNEL_BIN)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@echo "[🐝] إنشاء مجلد البناء..."

$(BUILD_DIR)/entry.o: $(TRAB_DIR)/entry.asm
	@echo "[🐝] تجميع entry.asm..."
	$(NASM) $(NASM_FLAGS) $< -o $@
	@echo "[🐝] ✓ entry.o جاهز"

$(KERNEL_BIN): $(ASM_OBJS)
	@echo "[🐝] الربط..."
	$(LD) $(LD_FLAGS) $(ASM_OBJS) -o $@
	@echo "[🐝] ✓ $(KERNEL_BIN) جاهز"
	@echo "[🐝]   الحجم: $$(du -h $(KERNEL_BIN) | cut -f1)"

## بناء صورة ISO مع GRUB2
iso: $(KERNEL_BIN)
	@echo "[🐝] بناء صورة ISO..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_BIN) $(ISO_DIR)/boot/BeeOS.elf
	@echo 'set timeout=3'                              >  $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'set default=0'                              >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo ''                                           >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo 'menuentry "🐝 BeeOS — نظام النحلة" {'     >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '    multiboot2 /boot/BeeOS.elf'             >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '    boot'                                   >> $(ISO_DIR)/boot/grub/grub.cfg
	@echo '}'                                          >> $(ISO_DIR)/boot/grub/grub.cfg
	$(GRUB_MKR) -o $(KERNEL_ISO) $(ISO_DIR)
	@echo "[🐝] ✓ $(KERNEL_ISO) جاهز"

## تشغيل على QEMU مباشرة
qemu: $(KERNEL_BIN)
	@echo "[🐝] تشغيل BeeOS على QEMU..."
	@echo "[🐝] بِسْمِ اللهِ الرَّحْمٰنِ الرَّحِيمِ"
	$(QEMU) -kernel $(KERNEL_BIN) $(QEMU_FLAGS)

## تشغيل مع GDB stub
debug: $(KERNEL_BIN)
	@echo "[🐝] وضع التصحيح — GDB على :1234"
	@echo "[🐝] شغّل: gdb -ex 'target remote :1234' $(KERNEL_BIN)"
	$(QEMU) -kernel $(KERNEL_BIN) $(QEMU_FLAGS) -s -S

## اختبار بالمفسر (بدون بناء)
test:
	@echo "[🐝] اختبار وحدات النواة بالمفسر..."
	@echo "= شاشة ="
	@build/bin/Debug/sad.exe bee_os/kernel/شاشة.ص 2>&1 | tail -5
	@echo "= ذاكرة ="
	@build/bin/Debug/sad.exe bee_os/kernel/ذاكرة.ص 2>&1 | tail -5
	@echo "= مقاطعات ="
	@build/bin/Debug/sad.exe bee_os/kernel/مقاطعات.ص 2>&1 | tail -5
	@echo "= جدولة ="
	@build/bin/Debug/sad.exe bee_os/kernel/جدولة.ص 2>&1 | tail -5
	@echo "= إقلاع ="
	@build/bin/Debug/sad.exe bee_os/kernel/إقلاع.ص 2>&1 | tail -5
	@echo "= نواة ="
	@build/bin/Debug/sad.exe bee_os/kernel/نواة.ص 2>&1 | tail -5
	@echo "[🐝] ✓ جميع الاختبارات اكتملت"

## تنظيف
clean:
	@echo "[🐝] تنظيف..."
	@rm -rf $(BUILD_DIR)
	@echo "[🐝] ✓ تم التنظيف"

## معلومات المشروع
info:
	@echo "═══════════════════════════════════════"
	@echo "  🐝  BeeOS v0.1.0 — نظام النحلة"
	@echo "═══════════════════════════════════════"
	@echo "  النواة:   $(KERNEL_BIN)"
	@echo "  ISO:      $(KERNEL_ISO)"
	@echo "  NASM:     $$($(NASM) --version 2>&1 | head -1)"
	@echo "  QEMU:     $$($(QEMU) --version 2>&1 | head -1)"
	@echo "═══════════════════════════════════════"
