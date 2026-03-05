/*
 * BeeOS Kernel Boot Entry (32-bit i686)
 * Initializes hardware stubs, shows splash, calls ص kernel,
 * then switches to Bochs VGA framebuffer HD desktop.
 * بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ
 */

#include "beeos_kernel.h"

/* VGA color codes (fg + bg) */
#define C_BLUE_WHITE  COLOR(15, 1)
#define C_CYAN_BLUE   COLOR(11, 1)
#define C_YELLOW_BLUE COLOR(14, 1)
#define C_GREEN_BLUE  COLOR(10, 1)
#define C_RED_BLUE    COLOR(12, 1)
#define C_WHITE_BLACK COLOR(7, 0)

/* Declared in beeos_kernel.ll (compiled from ص language) */
/* Each module gets its own init function (__sad_init_N) */
extern void __sad_init_0(void);   /* بداية_نحلة */
extern void __sad_init_1(void);   /* تسلسلي_نحلة */
extern void __sad_init_2(void);   /* مقاطعات_نحلة */
extern void __sad_init_3(void);   /* ذاكرة_نحلة */
extern void __sad_init_4(void);   /* كومة_نحلة */
extern void __sad_init_5(void);   /* جدولة_نحلة */
extern void __sad_init_6(void);   /* اتصال_نحلة */
extern void __sad_init_7(void);   /* قرص_نحلة */
extern void __sad_init_8(void);   /* قرص_cache_نحلة */
extern void __sad_init_9(void);   /* ملفات_نحلة */
extern void __sad_init_10(void);  /* ملفات_عمليات_نحلة */
extern void __sad_init_11(void);  /* صدفة_نحلة */
extern void __sad_init_12(void);  /* عمليات_نحلة */
extern void __sad_init_13(void);  /* واجهة_نحلة */
extern void __sad_init_14(void);  /* واجهة_متقدمة_نحلة */
extern void __sad_init_15(void);  /* ملفات_متقدمة_نحلة */
extern void __sad_init_16(void);  /* ملفات_أمان_نحلة */
extern void __sad_init_17(void);  /* ملفات_أنظمة_نحلة */
extern void __sad_init_18(void);  /* ملفات_ذاكرة_نحلة */
extern void __sad_init_19(void);  /* طاقة_نحلة */
extern void __sad_init_20(void);  /* طاقة_حرارة_نحلة */
extern void __sad_init_21(void);  /* طاقة_أجهزة_نحلة */
extern void __sad_init_22(void);  /* شبكة_نحلة */
extern void __sad_init_23(void);  /* شبكة_arp_نحلة */
extern void __sad_init_24(void);  /* شبكة_ip_نحلة */
extern void __sad_init_25(void);  /* شبكة_tcp_نحلة */
extern void __sad_init_26(void);  /* شبكة_dns_نحلة */
extern void __sad_init_27(void);  /* pci_نحلة */
extern void __sad_init_28(void);  /* usb_نحلة */
extern void __sad_init_29(void);  /* usb_uhci_نحلة */
extern void __sad_init_30(void);  /* usb_ehci_نحلة */
extern void __sad_init_31(void);  /* usb_hid_نحلة */
extern void __sad_init_32(void);  /* usb_تخزين_نحلة */
extern void __sad_init_33(void);  /* usb_موزع_نحلة */
extern void __sad_init_34(void);  /* usb_xhci_نحلة */
extern void __sad_init_35(void);  /* ps2_نحلة */
extern void __sad_init_36(void);  /* acpi_نحلة */
extern void __sad_init_37(void);  /* ata_نحلة */
extern void __sad_init_38(void);  /* أجهزة_مدير_نحلة */
extern void __sad_init_39(void);  /* usb_صوت_نحلة */
extern void __sad_init_40(void);  /* usb_شبكة_نحلة */
extern void __sad_init_41(void);  /* fat32_نحلة */
extern void __sad_init_42(void);  /* fat32_دليل_نحلة */
extern void __sad_init_43(void);  /* fat32_ملف_نحلة */
extern void __sad_init_44(void);  /* ext2_نحلة */
extern void __sad_init_45(void);  /* ext2_عقد_نحلة */
extern void __sad_init_46(void);  /* ext2_دليل_نحلة */
extern void __sad_init_47(void);  /* vfs_نحلة */
extern void __sad_init_48(void);  /* vfs_مسار_نحلة */
extern void __sad_init_49(void);  /* ذاكرة_كتل_نحلة */
extern void __sad_init_50(void);  /* أقسام_نحلة */
extern void __sad_init_51(void);  /* يوميات_نحلة */
extern void __sad_init_52(void);  /* tmpfs_نحلة */
extern void __sad_init_53(void);  /* procfs_نحلة */
extern void __sad_init_54(void);  /* ntfs_نحلة */
extern void __sad_init_55(void);  /* ntfs_دليل_نحلة */
extern void __sad_init_56(void);  /* sysfs_نحلة */
extern void __sad_init_57(void);  /* devfs_نحلة */
extern void __sad_init_58(void);  /* أنابيب_نحلة */
extern void __sad_init_59(void);  /* مقابس_نحلة */
extern void __sad_init_60(void);  /* ضغط_نحلة */
extern void __sad_init_61(void);  /* تشفير_نحلة */
extern void __sad_init_62(void);  /* raid_نحلة */
extern void __sad_init_63(void);  /* أذونات_نحلة */
extern void __sad_init_64(void);  /* حصص_نحلة */
extern void __sad_init_65(void);  /* exfat_نحلة */
extern void __sad_init_66(void);  /* قفل_ملفات_نحلة */
extern void __sad_init_67(void);  /* iso9660_نحلة */
extern void __sad_init_68(void);  /* ذاكرة_مؤقتة_نحلة */
extern void __sad_init_69(void);  /* جدول_io_نحلة */
extern void __sad_init_70(void);  /* تحميل_نحلة */
extern void __sad_init_71(void);  /* مراقب_نحلة */
extern void __sad_init_72(void);  /* روابط_نحلة */
extern void __sad_init_73(void);  /* fsck_نحلة */
extern void __sad_init_74(void);  /* fat32_كتابة_نحلة */
extern void __sad_init_75(void);  /* btrfs_نحلة */
extern void __sad_init_76(void);  /* squashfs_نحلة */
extern void __sad_init_77(void);  /* overlayfs_نحلة */
extern void __sad_init_78(void);  /* أوامر_ملفات_نحلة */

/* Main entry from بداية_نحلة.ص — aliased as beeos_kernel_entry in link step */
extern void beeos_kernel_entry(void);

/* Simple delay (busy-wait) */
static void kdelay(int n) {
    for (volatile int i = 0; i < n * 100000; i++);
}

/* Print a full-width horizontal line */
static void kline(char ch) {
    for (int i = 0; i < 78; i++) vga_putc(ch);
    vga_putc('\n');
}

/* Center text in 80-col screen */
static void kcenter(const char *s) {
    int len = 0;
    for (const char *p = s; *p; p++) len++;
    int pad = (78 - len) / 2;
    for (int i = 0; i < pad; i++) vga_putc(' ');
    vga_puts(s);
    vga_putc('\n');
}

/*
 * kernel_boot_main — called from boot.S
 * Arguments passed via stack (cdecl): magic, info_ptr
 */
void kernel_boot_main(unsigned int mb_magic, unsigned int mb_info_ptr) {
    /* Disable interrupts immediately — no IDT installed yet */
    __asm__ volatile("cli");

    /* Mask all PIC interrupts to prevent spurious IRQs */
    __asm__ volatile("outb %0, %1" : : "a"((unsigned char)0xFF), "Nd"((unsigned short)0x21));
    __asm__ volatile("outb %0, %1" : : "a"((unsigned char)0xFF), "Nd"((unsigned short)0xA1));

    /* Init serial COM1 */
    serial_init();

    /* Blue screen — VGA text splash */
    vga_clear(C_BLUE_WHITE);
    vga_set_color(C_YELLOW_BLUE);

    kline('=');
    vga_set_color(C_CYAN_BLUE);
    kcenter("BeeOS HD v0.2.0");
    kcenter("Compiled from Sad Language via LLVM 18");
    vga_set_color(C_YELLOW_BLUE);
    kline('=');

    vga_set_color(C_WHITE_BLACK);
    serial_puts("\r\n");
    serial_puts("=== BeeOS HD Serial Boot Log ===\r\n");
    serial_puts("Toolchain: sadc -> LLVM IR -> llc -> ld.lld -> QEMU\r\n\r\n");

    /* ---- launch the ص-compiled kernel (prints init to serial+VGA) ---- */
    vga_set_color(C_GREEN_BLUE);
    vga_puts("\n  [QEMU] Starting BeeOS HD Kernel...\n\n");
    serial_puts("[QEMU] Entering __sad_main()\r\n");
    vga_set_color(C_BLUE_WHITE);

    /* ---- Switch to BGA framebuffer (1024×768) FIRST ---- */
    serial_puts("[BOOT] Switching to Bochs VGA framebuffer...\r\n");
    kdelay(3);

    if (bga_init(1024, 768)) {
        /* Draw the HD desktop! */
        fb_draw_desktop();
        serial_puts("[FB] HD Desktop drawn!\r\n");
    } else {
        serial_puts("[FB] BGA init failed — staying in VGA text mode.\r\n");
        vga_set_color(C_RED_BLUE);
        vga_puts("\n  [WARN] Framebuffer unavailable — text mode.\n");
    }

    /* ---- Initialize ص kernel modules ---- */
    serial_puts("[INIT] init_0 (boot)...\r\n");
    __sad_init_0();   /* بداية — global var init */
    serial_puts("[INIT] init_1 (serial)...\r\n");
    __sad_init_1();   /* تسلسلي */
    serial_puts("[INIT] init_2 (interrupts)...\r\n");
    __sad_init_2();   /* مقاطعات */
    serial_puts("[INIT] init_3 (memory)...\r\n");
    __sad_init_3();   /* ذاكرة */
    serial_puts("[INIT] init_4 (heap)...\r\n");
    __sad_init_4();   /* كومة */
    serial_puts("[INIT] init_5 (scheduler)...\r\n");
    __sad_init_5();   /* جدولة */
    serial_puts("[INIT] init_6 (ipc)...\r\n");
    __sad_init_6();   /* اتصال */
    serial_puts("[INIT] init_7 (disk)...\r\n");
    __sad_init_7();   /* قرص */
    serial_puts("[INIT] init_8 (cache)...\r\n");
    __sad_init_8();   /* قرص_cache */
    serial_puts("[INIT] init_9 (fs)...\r\n");
    __sad_init_9();   /* ملفات */
    serial_puts("[INIT] init_10 (vfs)...\r\n");
    __sad_init_10();  /* ملفات_عمليات */
    serial_puts("[INIT] init_11 (shell)...\r\n");
    __sad_init_11();  /* صدفة */
    serial_puts("[INIT] init_12 (process)...\r\n");
    __sad_init_12();  /* عمليات */
    serial_puts("[INIT] init_13 (gui)...\r\n");
    __sad_init_13();  /* واجهة */
    serial_puts("[INIT] init_14 (adv_gui)...\r\n");
    __sad_init_14();  /* واجهة_متقدمة */
    serial_puts("[INIT] init_15 (adv_fs)...\r\n");
    __sad_init_15();  /* ملفات_متقدمة */
    serial_puts("[INIT] init_16 (security)...\r\n");
    __sad_init_16();  /* أمان_ملفات */
    serial_puts("[INIT] init_17 (virt_fs)...\r\n");
    __sad_init_17();  /* أنظمة_ملفات */
    serial_puts("[INIT] init_18 (file_mem)...\r\n");
    __sad_init_18();  /* ذاكرة_ملفات */
    serial_puts("[INIT] init_19 (power)...\r\n");
    __sad_init_19();  /* طاقة */
    serial_puts("[INIT] init_20 (thermal)...\r\n");
    __sad_init_20();  /* حرارة */
    serial_puts("[INIT] init_21 (dev_pm)...\r\n");
    __sad_init_21();  /* طاقة_أجهزة */
    serial_puts("[INIT] init_22 (net_core)...\r\n");
    __sad_init_22();  /* شبكة */
    serial_puts("[INIT] init_23 (arp)...\r\n");
    __sad_init_23();  /* شبكة_arp */
    serial_puts("[INIT] init_24 (ipv4)...\r\n");
    __sad_init_24();  /* شبكة_ip */
    serial_puts("[INIT] init_25 (tcp_udp)...\r\n");
    __sad_init_25();  /* شبكة_tcp */
    serial_puts("[INIT] init_26 (dns_dhcp)...\r\n");
    __sad_init_26();  /* شبكة_dns */
    serial_puts("[INIT] init_27 (pci)...\r\n");
    __sad_init_27();  /* pci */
    serial_puts("[INIT] init_28 (usb_core)...\r\n");
    __sad_init_28();  /* usb */
    serial_puts("[INIT] init_29 (uhci)...\r\n");
    __sad_init_29();  /* usb_uhci */
    serial_puts("[INIT] init_30 (ehci)...\r\n");
    __sad_init_30();  /* usb_ehci */
    serial_puts("[INIT] init_31 (hid)...\r\n");
    __sad_init_31();  /* usb_hid */
    serial_puts("[INIT] init_32 (storage)...\r\n");
    __sad_init_32();  /* usb_تخزين */
    serial_puts("[INIT] init_33 (hub)...\r\n");
    __sad_init_33();  /* usb_موزع */
    serial_puts("[INIT] init_34 (xhci)...\r\n");
    __sad_init_34();  /* usb_xhci */
    serial_puts("[INIT] init_35 (ps2)...\r\n");
    __sad_init_35();  /* ps2 */
    serial_puts("[INIT] init_36 (acpi)...\r\n");
    __sad_init_36();  /* acpi */
    serial_puts("[INIT] init_37 (ata)...\r\n");
    __sad_init_37();  /* ata */
    serial_puts("[INIT] init_38 (devmgr)...\r\n");
    __sad_init_38();  /* أجهزة_مدير */
    serial_puts("[INIT] init_39 (usb_audio)...\r\n");
    __sad_init_39();  /* usb_صوت */
    serial_puts("[INIT] init_40 (usb_cdc)...\r\n");
    __sad_init_40();  /* usb_شبكة */
    serial_puts("[INIT] init_41 (fat32)...\r\n");
    __sad_init_41();  /* fat32 */
    serial_puts("[INIT] init_42 (fat32_dir)...\r\n");
    __sad_init_42();  /* fat32_دليل */
    serial_puts("[INIT] init_43 (fat32_file)...\r\n");
    __sad_init_43();  /* fat32_ملف */
    serial_puts("[INIT] init_44 (ext2)...\r\n");
    __sad_init_44();  /* ext2 */
    serial_puts("[INIT] init_45 (ext2_inode)...\r\n");
    __sad_init_45();  /* ext2_عقد */
    serial_puts("[INIT] init_46 (ext2_dir)...\r\n");
    __sad_init_46();  /* ext2_دليل */
    serial_puts("[INIT] init_47 (vfs)...\r\n");
    __sad_init_47();  /* vfs */
    serial_puts("[INIT] init_48 (vfs_path)...\r\n");
    __sad_init_48();  /* vfs_مسار */
    serial_puts("[INIT] init_49 (block_cache)...\r\n");
    __sad_init_49();  /* ذاكرة_كتل */
    serial_puts("[INIT] init_50 (partitions)...\r\n");
    __sad_init_50();  /* أقسام */
    serial_puts("[INIT] init_51 (journal)...\r\n");
    __sad_init_51();  /* يوميات */
    serial_puts("[INIT] init_52 (tmpfs)...\r\n");
    __sad_init_52();  /* tmpfs */
    serial_puts("[INIT] init_53 (procfs)...\r\n");
    __sad_init_53();  /* procfs */
    serial_puts("[INIT] init_54 (ntfs)...\r\n");
    __sad_init_54();  /* ntfs */
    serial_puts("[INIT] init_55 (ntfs_dir)...\r\n");
    __sad_init_55();  /* ntfs_dir */
    serial_puts("[INIT] init_56 (sysfs)...\r\n");
    __sad_init_56();  /* sysfs */
    serial_puts("[INIT] init_57 (devfs)...\r\n");
    __sad_init_57();  /* devfs */
    serial_puts("[INIT] init_58 (pipes)...\r\n");
    __sad_init_58();  /* pipes */
    serial_puts("[INIT] init_59 (sockets)...\r\n");
    __sad_init_59();  /* sockets */
    serial_puts("[INIT] init_60 (compress)...\r\n");
    __sad_init_60();  /* compress */
    serial_puts("[INIT] init_61 (encrypt)...\r\n");
    __sad_init_61();  /* encrypt */
    serial_puts("[INIT] init_62 (raid)...\r\n");
    __sad_init_62();  /* raid */
    serial_puts("[INIT] init_63 (acl)...\r\n");
    __sad_init_63();  /* permissions */
    serial_puts("[INIT] init_64 (quotas)...\r\n");
    __sad_init_64();  /* quotas */
    serial_puts("[INIT] init_65 (exfat)...\r\n");
    __sad_init_65();  /* exfat */
    serial_puts("[INIT] init_66 (flock)...\r\n");
    __sad_init_66();  /* file_locking */
    serial_puts("[INIT] init_67 (iso9660)...\r\n");
    __sad_init_67();  /* iso9660 */
    serial_puts("[INIT] init_68 (bcache)...\r\n");
    __sad_init_68();  /* block_cache */
    serial_puts("[INIT] init_69 (iosched)...\r\n");
    __sad_init_69();  /* io_scheduler */
    serial_puts("[INIT] init_70 (mount)...\r\n");
    __sad_init_70();  /* mount */
    serial_puts("[INIT] init_71 (inotify)...\r\n");
    __sad_init_71();  /* inotify */
    serial_puts("[INIT] init_72 (symlinks)...\r\n");
    __sad_init_72();  /* symlinks */
    serial_puts("[INIT] init_73 (fsck)...\r\n");
    __sad_init_73();  /* fsck */
    serial_puts("[INIT] init_74 (fat32_write)...\r\n");
    __sad_init_74();  /* fat32_write */
    serial_puts("[INIT] init_75 (btrfs)...\r\n");
    __sad_init_75();  /* btrfs */
    serial_puts("[INIT] init_76 (squashfs)...\r\n");
    __sad_init_76();  /* squashfs */
    serial_puts("[INIT] init_77 (overlayfs)...\r\n");
    __sad_init_77();  /* overlayfs */
    serial_puts("[INIT] init_78 (shell_fs)...\r\n");
    __sad_init_78();  /* shell_fs_commands */
    serial_puts("[INIT] All 79 modules initialized OK\r\n");

    /* Now call the real kernel entry point */
    serial_puts("[BOOT] Calling beeos_kernel_entry()...\r\n");
    beeos_kernel_entry();

    serial_puts("\r\n[QEMU] __sad_main() returned OK\r\n");

    /* Halt — kernel stays in framebuffer mode */
    serial_puts("\r\n=== BeeOS HD: system idle (HLT loop) ===\r\n");
    for (;;)
        __asm__ volatile("cli; hlt");
}
