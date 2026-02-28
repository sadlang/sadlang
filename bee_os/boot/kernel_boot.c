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
extern void __sad_main(void);

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

    __sad_main();

    serial_puts("\r\n[QEMU] __sad_main() returned OK\r\n");

    /* ---- Switch to BGA framebuffer (1024×768) ---- */
    serial_puts("[BOOT] Switching to Bochs VGA framebuffer...\r\n");

    vga_set_color(C_GREEN_BLUE);
    vga_puts("\n  [HD] Switching to 1024x768 framebuffer...\n");

    kdelay(5);  /* Small delay before mode switch */

    if (bga_init(1024, 768)) {
        /* Draw the HD desktop! */
        fb_draw_desktop();

        serial_puts("\r\n[FB] HD Desktop active! System running.\r\n");
        serial_puts("[FB] The screen shows the golden BeeOS theme.\r\n");
    } else {
        serial_puts("[FB] BGA init failed — staying in VGA text mode.\r\n");
        vga_set_color(C_RED_BLUE);
        vga_puts("\n  [WARN] Framebuffer unavailable — text mode.\n");
    }

    /* Halt — kernel stays in framebuffer mode */
    serial_puts("\r\n=== BeeOS HD: system idle (HLT loop) ===\r\n");
    for (;;)
        __asm__ volatile("cli; hlt");
}
