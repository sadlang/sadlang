/*
 * ═══════════════════════════════════════════════════════════════════
 *  BeeOS HD Framebuffer Desktop — رسم سطح المكتب بالبكسل
 * ═══════════════════════════════════════════════════════════════════
 *
 * يرسم سطح مكتب HD حقيقي على الـ framebuffer بعد إقلاع النواة.
 * يستخدم Bochs VGA Adapter (BGA) عبر port I/O لتفعيل الوضع الرسومي.
 *
 * -- Bochs VGA (BGA) —
 * QEMU -vga std يحاكي محوّل Bochs VGA الذي يدعم:
 *   Index port: 0x1CE | Data port: 0x1CF
 *   Registers: XRES, YRES, BPP, ENABLE, BANK, VIRT_WIDTH, ...
 *   LFB Address: BAR0 من PCI Device (Bus 0 Dev 2 Func 0)
 *
 * -- طبقات الرسم (painter's algorithm) —
 *   1. خلفية تدرج لوني (ذهبي → أزرق داكن)
 *   2. أيقونات سطح المكتب (7 أيقونات)
 *   3. نوافذ (عنوان + جسم + ظل)
 *   4. شريط المهام (أسفل الشاشة)
 *   5. مؤشر الفأرة (أعلى طبقة)
 *
 * بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ
 */

#include "beeos_kernel.h"

/* ─── Port I/O (inline assembly) ─── */
static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}
static inline void outw(unsigned short port, unsigned short val) {
    __asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}
static inline void outl(unsigned short port, unsigned int val) {
    __asm__ volatile("outl %0, %1" : : "a"(val), "Nd"(port));
}
static inline unsigned int inl(unsigned short port) {
    unsigned int ret;
    __asm__ volatile("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* ─── BGA (Bochs VGA) Constants ─── */
#define VBE_DISPI_IOPORT_INDEX    0x01CE
#define VBE_DISPI_IOPORT_DATA     0x01CF
#define VBE_DISPI_INDEX_ID        0x0
#define VBE_DISPI_INDEX_XRES      0x1
#define VBE_DISPI_INDEX_YRES      0x2
#define VBE_DISPI_INDEX_BPP       0x3
#define VBE_DISPI_INDEX_ENABLE    0x4
#define VBE_DISPI_INDEX_BANK      0x5
#define VBE_DISPI_INDEX_VIRT_W    0x6
#define VBE_DISPI_INDEX_VIRT_H    0x7
#define VBE_DISPI_DISABLED        0x00
#define VBE_DISPI_ENABLED         0x01
#define VBE_DISPI_LFB_ENABLED     0x40

/* PCI Configuration Space addresses */
#define PCI_CONFIG_ADDR  0x0CF8
#define PCI_CONFIG_DATA  0x0CFC

/* Write to BGA register */
static void bga_write(unsigned short index, unsigned short value) {
    outw(VBE_DISPI_IOPORT_INDEX, index);
    outw(VBE_DISPI_IOPORT_DATA, value);
}

/* Read PCI config dword */
static unsigned int pci_read32(unsigned char bus, unsigned char dev,
                               unsigned char func, unsigned char off) {
    unsigned int addr = 0x80000000
                      | ((unsigned int)bus << 16)
                      | ((unsigned int)dev << 11)
                      | ((unsigned int)func << 8)
                      | (off & 0xFC);
    outl(PCI_CONFIG_ADDR, addr);
    return inl(PCI_CONFIG_DATA);
}

/* ─── Framebuffer state ─── */
static unsigned int *g_fb = 0;
static unsigned int  g_width = 0;
static unsigned int  g_height = 0;
static unsigned int  g_pitch = 0;

/* Print hex value to serial */
static void serial_hex(unsigned int v) {
    char hex[9];
    for (int i = 7; i >= 0; i--) {
        int d = v & 0xF;
        hex[i] = d < 10 ? ('0' + d) : ('A' + d - 10);
        v >>= 4;
    }
    hex[8] = 0;
    serial_puts(hex);
}

static void serial_num(unsigned int n) {
    char buf[12];
    int i = 0;
    if (n == 0) { serial_putc('0'); return; }
    while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
    while (i > 0) serial_putc(buf[--i]);
}

/*
 * bga_init — تهيئة محوّل Bochs VGA عبر port I/O
 *
 * يستخدم BGA ports 0x1CE/0x1CF مباشرةً لتفعيل وضع رسومي.
 * يقرأ LFB address من PCI BAR0 للجهاز الصحيح.
 */
int bga_init(unsigned int width, unsigned int height) {
    serial_puts("[BGA] Bochs VGA init: ");
    serial_num(width);
    serial_puts("x");
    serial_num(height);
    serial_puts(" @32bpp\r\n");

    /* Try both common QEMU VGA PCI positions */
    unsigned int fb_addr = 0;
    int found_dev = -1;

    /* q35: dev 1, i440fx: dev 2 */
    for (int dev = 1; dev <= 2; dev++) {
        unsigned int id = pci_read32(0, dev, 0, 0);
        if (id == 0xFFFFFFFF || id == 0) continue;

        unsigned int classreg = pci_read32(0, dev, 0, 0x08);
        unsigned char base_class = (classreg >> 24) & 0xFF;

        if (base_class == 0x03) {
            unsigned int bar0 = pci_read32(0, dev, 0, 0x10);
            fb_addr = bar0 & 0xFFFFFFF0;
            found_dev = dev;

            serial_puts("[BGA] VGA at PCI 0:");
            serial_num(dev);
            serial_puts(".0 BAR0=0x");
            serial_hex(fb_addr);
            serial_puts("\r\n");
            break;
        }
    }

    if (fb_addr == 0) {
        serial_puts("[BGA] ERROR: No VGA found!\r\n");
        return 0;
    }

    /* Set BGA mode */
    serial_puts("[BGA] Setting mode...\r\n");
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write(VBE_DISPI_INDEX_XRES,   (unsigned short)width);
    bga_write(VBE_DISPI_INDEX_YRES,   (unsigned short)height);
    bga_write(VBE_DISPI_INDEX_BPP,    32);
    bga_write(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED);

    /* Initialize framebuffer state */
    g_fb = (unsigned int *)fb_addr;
    g_width = width;
    g_height = height;
    g_pitch = width * 4;

    serial_puts("[BGA] Mode set OK! LFB=0x");
    serial_hex(fb_addr);
    serial_puts("\r\n");

    return 1;
}

/* ─── Primitive: set pixel ─── */
void fb_pixel(int x, int y, unsigned int color) {
    if (x < 0 || y < 0 || (unsigned)x >= g_width || (unsigned)y >= g_height) return;
    unsigned int *row = (unsigned int *)((unsigned char *)g_fb + y * g_pitch);
    row[x] = color;
}

/* ─── Primitive: filled rectangle ─── */
void fb_rect(int x, int y, int w, int h, unsigned int color) {
    for (int j = y; j < y + h; j++)
        for (int i = x; i < x + w; i++)
            fb_pixel(i, j, color);
}

/* ─── Primitive: horizontal line ─── */
void fb_hline(int x, int y, int w, unsigned int color) {
    for (int i = x; i < x + w; i++)
        fb_pixel(i, y, color);
}

/* ─── Primitive: vertical line ─── */
void fb_vline(int x, int y, int h, unsigned int color) {
    for (int j = y; j < y + h; j++)
        fb_pixel(x, j, color);
}

/* ─── Primitive: outlined rectangle ─── */
void fb_outline(int x, int y, int w, int h, unsigned int color) {
    fb_hline(x, y, w, color);
    fb_hline(x, y + h - 1, w, color);
    fb_vline(x, y, h, color);
    fb_vline(x + w - 1, y, h, color);
}

/* ─── Color utilities ─── */
static unsigned int rgb(int r, int g, int b) {
    return ((unsigned int)r << 16) | ((unsigned int)g << 8) | (unsigned int)b;
}

static unsigned int lerp_color(unsigned int c1, unsigned int c2, int t, int max) {
    int r1 = (c1 >> 16) & 0xFF, g1 = (c1 >> 8) & 0xFF, b1 = c1 & 0xFF;
    int r2 = (c2 >> 16) & 0xFF, g2 = (c2 >> 8) & 0xFF, b2 = c2 & 0xFF;
    int r = r1 + (r2 - r1) * t / max;
    int g = g1 + (g2 - g1) * t / max;
    int b = b1 + (b2 - b1) * t / max;
    return rgb(r, g, b);
}

/* ═══════════════════════════════════════════════════════════════════
 *  BeeOS Mini Font — 5×7 bitmap font for ASCII
 * ═══════════════════════════════════════════════════════════════════
 * Each character is 5 pixels wide × 7 pixels tall.
 * Stored as 7 bytes per character (each byte = row, low 5 bits used).
 * Supports: A-Z, 0-9, space, common punctuation.
 */

/* Minimal 5×7 font for essential characters (space..~) */
static const unsigned char font5x7[96][7] = {
    /* space (32) */ {0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    /* !     (33) */ {0x04,0x04,0x04,0x04,0x00,0x04,0x00},
    /* "     (34) */ {0x0A,0x0A,0x00,0x00,0x00,0x00,0x00},
    /* #     (35) */ {0x0A,0x1F,0x0A,0x0A,0x1F,0x0A,0x00},
    /* $     (36) */ {0x04,0x0F,0x14,0x0E,0x05,0x1E,0x04},
    /* %     (37) */ {0x19,0x1A,0x02,0x04,0x0B,0x13,0x00},
    /* &     (38) */ {0x08,0x14,0x08,0x15,0x12,0x0D,0x00},
    /* '     (39) */ {0x04,0x04,0x00,0x00,0x00,0x00,0x00},
    /* (     (40) */ {0x02,0x04,0x04,0x04,0x04,0x02,0x00},
    /* )     (41) */ {0x08,0x04,0x04,0x04,0x04,0x08,0x00},
    /* *     (42) */ {0x00,0x0A,0x04,0x1F,0x04,0x0A,0x00},
    /* +     (43) */ {0x00,0x04,0x04,0x1F,0x04,0x04,0x00},
    /* ,     (44) */ {0x00,0x00,0x00,0x00,0x04,0x04,0x08},
    /* -     (45) */ {0x00,0x00,0x00,0x1F,0x00,0x00,0x00},
    /* .     (46) */ {0x00,0x00,0x00,0x00,0x00,0x04,0x00},
    /* /     (47) */ {0x01,0x02,0x04,0x08,0x10,0x00,0x00},
    /* 0     (48) */ {0x0E,0x11,0x13,0x15,0x19,0x0E,0x00},
    /* 1     (49) */ {0x04,0x0C,0x04,0x04,0x04,0x0E,0x00},
    /* 2     (50) */ {0x0E,0x11,0x01,0x06,0x08,0x1F,0x00},
    /* 3     (51) */ {0x0E,0x11,0x02,0x01,0x11,0x0E,0x00},
    /* 4     (52) */ {0x02,0x06,0x0A,0x12,0x1F,0x02,0x00},
    /* 5     (53) */ {0x1F,0x10,0x1E,0x01,0x11,0x0E,0x00},
    /* 6     (54) */ {0x06,0x08,0x1E,0x11,0x11,0x0E,0x00},
    /* 7     (55) */ {0x1F,0x01,0x02,0x04,0x08,0x08,0x00},
    /* 8     (56) */ {0x0E,0x11,0x0E,0x11,0x11,0x0E,0x00},
    /* 9     (57) */ {0x0E,0x11,0x11,0x0F,0x02,0x0C,0x00},
    /* :     (58) */ {0x00,0x04,0x00,0x00,0x04,0x00,0x00},
    /* ;     (59) */ {0x00,0x04,0x00,0x00,0x04,0x04,0x08},
    /* <     (60) */ {0x02,0x04,0x08,0x08,0x04,0x02,0x00},
    /* =     (61) */ {0x00,0x00,0x1F,0x00,0x1F,0x00,0x00},
    /* >     (62) */ {0x08,0x04,0x02,0x02,0x04,0x08,0x00},
    /* ?     (63) */ {0x0E,0x11,0x02,0x04,0x00,0x04,0x00},
    /* @     (64) */ {0x0E,0x11,0x17,0x15,0x17,0x10,0x0E},
    /* A     (65) */ {0x0E,0x11,0x11,0x1F,0x11,0x11,0x00},
    /* B     (66) */ {0x1E,0x11,0x1E,0x11,0x11,0x1E,0x00},
    /* C     (67) */ {0x0E,0x11,0x10,0x10,0x11,0x0E,0x00},
    /* D     (68) */ {0x1E,0x11,0x11,0x11,0x11,0x1E,0x00},
    /* E     (69) */ {0x1F,0x10,0x1E,0x10,0x10,0x1F,0x00},
    /* F     (70) */ {0x1F,0x10,0x1E,0x10,0x10,0x10,0x00},
    /* G     (71) */ {0x0E,0x11,0x10,0x17,0x11,0x0E,0x00},
    /* H     (72) */ {0x11,0x11,0x1F,0x11,0x11,0x11,0x00},
    /* I     (73) */ {0x0E,0x04,0x04,0x04,0x04,0x0E,0x00},
    /* J     (74) */ {0x07,0x02,0x02,0x02,0x12,0x0C,0x00},
    /* K     (75) */ {0x11,0x12,0x14,0x18,0x14,0x12,0x11},
    /* L     (76) */ {0x10,0x10,0x10,0x10,0x10,0x1F,0x00},
    /* M     (77) */ {0x11,0x1B,0x15,0x11,0x11,0x11,0x00},
    /* N     (78) */ {0x11,0x19,0x15,0x13,0x11,0x11,0x00},
    /* O     (79) */ {0x0E,0x11,0x11,0x11,0x11,0x0E,0x00},
    /* P     (80) */ {0x1E,0x11,0x11,0x1E,0x10,0x10,0x00},
    /* Q     (81) */ {0x0E,0x11,0x11,0x15,0x12,0x0D,0x00},
    /* R     (82) */ {0x1E,0x11,0x11,0x1E,0x14,0x12,0x00},
    /* S     (83) */ {0x0E,0x11,0x08,0x06,0x11,0x0E,0x00},
    /* T     (84) */ {0x1F,0x04,0x04,0x04,0x04,0x04,0x00},
    /* U     (85) */ {0x11,0x11,0x11,0x11,0x11,0x0E,0x00},
    /* V     (86) */ {0x11,0x11,0x11,0x0A,0x0A,0x04,0x00},
    /* W     (87) */ {0x11,0x11,0x11,0x15,0x15,0x0A,0x00},
    /* X     (88) */ {0x11,0x0A,0x04,0x04,0x0A,0x11,0x00},
    /* Y     (89) */ {0x11,0x0A,0x04,0x04,0x04,0x04,0x00},
    /* Z     (90) */ {0x1F,0x02,0x04,0x08,0x10,0x1F,0x00},
    /* [     (91) */ {0x0E,0x08,0x08,0x08,0x08,0x0E,0x00},
    /* \     (92) */ {0x10,0x08,0x04,0x02,0x01,0x00,0x00},
    /* ]     (93) */ {0x0E,0x02,0x02,0x02,0x02,0x0E,0x00},
    /* ^     (94) */ {0x04,0x0A,0x11,0x00,0x00,0x00,0x00},
    /* _     (95) */ {0x00,0x00,0x00,0x00,0x00,0x1F,0x00},
    /* `     (96) */ {0x08,0x04,0x00,0x00,0x00,0x00,0x00},
    /* a     (97) */ {0x00,0x0E,0x01,0x0F,0x11,0x0F,0x00},
    /* b     (98) */ {0x10,0x10,0x1E,0x11,0x11,0x1E,0x00},
    /* c     (99) */ {0x00,0x0E,0x10,0x10,0x11,0x0E,0x00},
    /* d    (100) */ {0x01,0x01,0x0F,0x11,0x11,0x0F,0x00},
    /* e    (101) */ {0x00,0x0E,0x11,0x1F,0x10,0x0E,0x00},
    /* f    (102) */ {0x06,0x08,0x1C,0x08,0x08,0x08,0x00},
    /* g    (103) */ {0x00,0x0F,0x11,0x0F,0x01,0x0E,0x00},
    /* h    (104) */ {0x10,0x10,0x16,0x19,0x11,0x11,0x00},
    /* i    (105) */ {0x04,0x00,0x0C,0x04,0x04,0x0E,0x00},
    /* j    (106) */ {0x02,0x00,0x02,0x02,0x12,0x0C,0x00},
    /* k    (107) */ {0x10,0x12,0x14,0x18,0x14,0x12,0x00},
    /* l    (108) */ {0x0C,0x04,0x04,0x04,0x04,0x0E,0x00},
    /* m    (109) */ {0x00,0x1A,0x15,0x15,0x11,0x11,0x00},
    /* n    (110) */ {0x00,0x16,0x19,0x11,0x11,0x11,0x00},
    /* o    (111) */ {0x00,0x0E,0x11,0x11,0x11,0x0E,0x00},
    /* p    (112) */ {0x00,0x1E,0x11,0x1E,0x10,0x10,0x00},
    /* q    (113) */ {0x00,0x0F,0x11,0x0F,0x01,0x01,0x00},
    /* r    (114) */ {0x00,0x16,0x19,0x10,0x10,0x10,0x00},
    /* s    (115) */ {0x00,0x0E,0x10,0x0E,0x01,0x1E,0x00},
    /* t    (116) */ {0x08,0x1C,0x08,0x08,0x09,0x06,0x00},
    /* u    (117) */ {0x00,0x11,0x11,0x11,0x13,0x0D,0x00},
    /* v    (118) */ {0x00,0x11,0x11,0x0A,0x0A,0x04,0x00},
    /* w    (119) */ {0x00,0x11,0x11,0x15,0x15,0x0A,0x00},
    /* x    (120) */ {0x00,0x11,0x0A,0x04,0x0A,0x11,0x00},
    /* y    (121) */ {0x00,0x11,0x0A,0x04,0x08,0x10,0x00},
    /* z    (122) */ {0x00,0x1F,0x02,0x04,0x08,0x1F,0x00},
    /* {    (123) */ {0x02,0x04,0x0C,0x04,0x04,0x02,0x00},
    /* |    (124) */ {0x04,0x04,0x04,0x04,0x04,0x04,0x00},
    /* }    (125) */ {0x08,0x04,0x06,0x04,0x04,0x08,0x00},
    /* ~    (126) */ {0x00,0x09,0x16,0x00,0x00,0x00,0x00},
    /*DEL  (127) */ {0x1F,0x1F,0x1F,0x1F,0x1F,0x1F,0x00},
};

/* Draw a single character (5×7 at scale) */
void fb_char(int cx, int cy, char ch, unsigned int color, int scale) {
    int idx = (unsigned char)ch - 32;
    if (idx < 0 || idx >= 96) idx = 0;
    for (int row = 0; row < 7; row++) {
        unsigned char bits = font5x7[idx][row];
        for (int col = 0; col < 5; col++) {
            if (bits & (0x10 >> col)) {
                for (int sy = 0; sy < scale; sy++)
                    for (int sx = 0; sx < scale; sx++)
                        fb_pixel(cx + col*scale + sx, cy + row*scale + sy, color);
            }
        }
    }
}

/* Draw a string at position (x, y) with given color and scale */
void fb_string(int x, int y, const char *s, unsigned int color, int scale) {
    int cx = x;
    for (; *s; s++) {
        if (*s == '\n') { y += 8 * scale; cx = x; continue; }
        fb_char(cx, y, *s, color, scale);
        cx += 6 * scale;
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  النحلة الذهبية — الألوان (السمة الافتراضية)
 * ═══════════════════════════════════════════════════════════════════ */

#define GOLD_DARK    0x8B6914   /* خلفية ذهبية داكنة */
#define GOLD_LIGHT   0xDAA520   /* ذهبي فاتح */
#define GOLD_HONEY   0xFFD700   /* عسلي ذهبي */
#define BLUE_DARK    0x0A1628   /* أزرق داكن */
#define BLUE_MED     0x1A3A5C   /* أزرق متوسط */
#define WHITE        0xFFFFFF
#define BLACK        0x000000
#define GRAY_LIGHT   0xE0E0E0
#define GRAY_MED     0xAAAAAA
#define GRAY_DARK    0x404040
#define RED_BTN      0xE04040   /* أحمر لزر الإغلاق */
#define GREEN_BTN    0x40C040   /* أخضر لزر التكبير */
#define YELLOW_BTN   0xFFCC00   /* أصفر لزر التصغير */
#define SHADOW       0x1A1A1A   /* ظل النوافذ */
#define TASKBAR_BG   0x2A1A08   /* خلفية شريط المهام */

/* ═══════════════════════════════════════════════════════════════════
 *  Layer 1: الخلفية — تدرج لوني عمودي
 * ═══════════════════════════════════════════════════════════════════
 *
 * تدرج من أزرق داكن (أعلى) إلى ذهبي/عسلي (أسفل)
 * مثل أفق صحراوي عند المغرب
 */
static void draw_background(void) {
    unsigned int top_color = BLUE_DARK;     /* 0x0A1628 — سماء الليل */
    unsigned int mid_color = BLUE_MED;      /* 0x1A3A5C — أفق */
    unsigned int bot_color = GOLD_DARK;     /* 0x8B6914 — رمال ذهبية */

    int h = (int)g_height - 48;  /* 48px reserved for taskbar */
    int mid = h / 2;

    for (int y = 0; y < mid; y++) {
        unsigned int c = lerp_color(top_color, mid_color, y, mid);
        fb_hline(0, y, (int)g_width, c);
    }
    for (int y = mid; y < h; y++) {
        unsigned int c = lerp_color(mid_color, bot_color, y - mid, h - mid);
        fb_hline(0, y, (int)g_width, c);
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  Layer 2: أيقونات سطح المكتب
 * ═══════════════════════════════════════════════════════════════════
 *
 * 7 أيقونات على الجانب الأيسر، كل واحدة 48×48 بكسل
 * مع حرف كبير كرمز + تسمية أسفلها
 */

/* Draw a simple icon: colored square with a letter + label below */
static void draw_icon(int x, int y, unsigned int bg, char letter, const char *label) {
    /* Shadow */
    fb_rect(x + 3, y + 3, 48, 48, SHADOW);
    /* Icon background */
    fb_rect(x, y, 48, 48, bg);
    /* Border */
    fb_outline(x, y, 48, 48, WHITE);
    /* Letter (centered, scale 3 → 15×21px, center in 48×48) */
    fb_char(x + 17, y + 14, letter, WHITE, 3);
    /* Label below */
    fb_string(x - 4, y + 54, label, WHITE, 1);
}

static void draw_icons(void) {
    int x = 24, spacing = 80;

    draw_icon(x, 30 + spacing * 0, 0x2A7FFF, 'F', "Files");
    draw_icon(x, 30 + spacing * 1, 0x555555, 'S', "Setup");
    draw_icon(x, 30 + spacing * 2, 0x3A9F3A, 'E', "Edit");
    draw_icon(x, 30 + spacing * 3, 0xE07020, 'C', "Calc");
    draw_icon(x, 30 + spacing * 4, 0x2060D0, 'W', "Web");
    draw_icon(x, 30 + spacing * 5, 0x1A1A1A, '>', "Shell");
    draw_icon(x, 30 + spacing * 6, 0x8040A0, 'U', "USB");
}

/* ═══════════════════════════════════════════════════════════════════
 *  Layer 3: النوافذ
 * ═══════════════════════════════════════════════════════════════════
 *
 * بنية النافذة:
 *   ┌──────────────────────────────────┐ ← عنوان (28px)
 *   │ [✕] Title                  [_][□]│
 *   ├──────────────────────────────────┤
 *   │                                  │
 *   │         المحتوى                   │ ← جسم النافذة
 *   │                                  │
 *   └──────────────────────────────────┘
 *   ↑ ظل 4px أسفل وعلى اليمين
 */

static void draw_window(int x, int y, int w, int h,
                        const char *title, unsigned int title_bg) {
    int title_h = 28;

    /* Drop shadow */
    fb_rect(x + 4, y + 4, w, h, SHADOW);

    /* Window body */
    fb_rect(x, y + title_h, w, h - title_h, WHITE);

    /* Title bar */
    fb_rect(x, y, w, title_h, title_bg);

    /* Title text */
    fb_string(x + 30, y + 8, title, WHITE, 2);

    /* Close button (red circle) */
    fb_rect(x + 6, y + 6, 16, 16, RED_BTN);
    fb_char(x + 9, y + 8, 'x', WHITE, 2);

    /* Minimize button */
    fb_rect(x + w - 44, y + 6, 16, 16, YELLOW_BTN);
    fb_char(x + w - 41, y + 8, '-', BLACK, 2);

    /* Maximize button */
    fb_rect(x + w - 24, y + 6, 16, 16, GREEN_BTN);
    fb_char(x + w - 21, y + 8, '+', WHITE, 2);

    /* Window border */
    fb_outline(x, y, w, h, GRAY_DARK);
}

static void draw_windows(void) {
    /* Window 1: File Manager (back) */
    draw_window(150, 60, 500, 400, "Files", GOLD_DARK);

    /* File list inside */
    fb_string(166, 100, "/home/bee/", GRAY_DARK, 2);
    fb_hline(160, 120, 480, GRAY_LIGHT);

    fb_rect(166, 130, 12, 12, 0x2A7FFF);
    fb_string(184, 130, "Documents/", BLACK, 2);

    fb_rect(166, 155, 12, 12, 0x2A7FFF);
    fb_string(184, 155, "Pictures/", BLACK, 2);

    fb_rect(166, 180, 12, 12, 0x2A7FFF);
    fb_string(184, 180, "Music/", BLACK, 2);

    fb_rect(166, 205, 12, 12, 0x3A9F3A);
    fb_string(184, 205, "hello.sad", BLACK, 2);

    fb_rect(166, 230, 12, 12, 0x3A9F3A);
    fb_string(184, 230, "notes.txt", BLACK, 2);

    /* Window 2: Text Editor (front, overlapping) */
    draw_window(380, 180, 520, 380, "Editor", 0x1A5A3A);

    /* Editor content */
    fb_string(396, 220, "# BeeOS Editor", 0x008000, 2);
    fb_string(396, 248, "print(\"Salam!\")", BLACK, 2);
    fb_string(396, 276, "x = 42 + 7", BLACK, 2);

    /* Cursor blink line */
    fb_vline(396 + 11 * 12, 276, 16, BLACK);

    /* Line numbers */
    fb_string(394, 220, "1", GRAY_MED, 1);
    fb_string(394, 248, "2", GRAY_MED, 1);
    fb_string(394, 276, "3", GRAY_MED, 1);
}

/* ═══════════════════════════════════════════════════════════════════
 *  Layer 4: شريط المهام
 * ═══════════════════════════════════════════════════════════════════
 *
 * شريط أسفل الشاشة (48 بكسل ارتفاع):
 *   [BeeOS] [Files] [Editor] ─── [Net] [Vol] [Clock]
 */

static void draw_taskbar(void) {
    int ty = (int)g_height - 48;
    int tw = (int)g_width;

    /* Taskbar background (dark with golden accent) */
    fb_rect(0, ty, tw, 48, TASKBAR_BG);

    /* Top golden line (1px accent) */
    fb_hline(0, ty, tw, GOLD_HONEY);

    /* BeeOS button */
    fb_rect(4, ty + 8, 80, 32, GOLD_DARK);
    fb_outline(4, ty + 8, 80, 32, GOLD_HONEY);
    fb_string(14, ty + 16, "BeeOS", GOLD_HONEY, 2);

    /* Running apps */
    fb_rect(92, ty + 8, 90, 32, 0x3A2A1A);
    fb_outline(92, ty + 8, 90, 32, GOLD_LIGHT);
    fb_string(100, ty + 16, "Files", WHITE, 2);

    fb_rect(190, ty + 8, 100, 32, 0x3A2A1A);
    fb_outline(190, ty + 8, 100, 32, GOLD_HONEY);
    fb_string(198, ty + 16, "Editor", WHITE, 2);

    /* System tray (right side) */
    int rx = tw - 200;

    /* Network indicator */
    fb_rect(rx, ty + 10, 28, 28, 0x204020);
    fb_char(rx + 7, ty + 14, 'N', 0x40FF40, 3);

    /* Volume */
    fb_rect(rx + 36, ty + 10, 28, 28, 0x202040);
    fb_char(rx + 43, ty + 14, 'V', 0x8080FF, 3);

    /* Clock */
    fb_rect(rx + 90, ty + 8, 100, 32, 0x1A1A1A);
    fb_string(rx + 98, ty + 16, "14:47", GOLD_HONEY, 2);
}

/* ═══════════════════════════════════════════════════════════════════
 *  Layer 5: مؤشر الفأرة
 * ═══════════════════════════════════════════════════════════════════
 *
 * سهم بسيط 12×18 بكسل (أسود مع حدود بيضاء)
 */

static void draw_cursor(int mx, int my) {
    /* Arrow shape — 12 rows */
    static const unsigned char arrow[18] = {
        0x80,  /* 1         */
        0xC0,  /* 11        */
        0xE0,  /* 111       */
        0xF0,  /* 1111      */
        0xF8,  /* 11111     */
        0xFC,  /* 111111    */
        0xFE,  /* 1111111   */
        0xFF,  /* 11111111  */
        0xFF,  /* 11111111  */
        0xFC,  /* 111111    */
        0xF8,  /* 11111     */
        0xD8,  /* 1101 1    */
        0x88,  /* 1   1     */
        0x0C,  /* 0000 11   */
        0x0C,  /* 0000 11   */
        0x06,  /* 000001 1  */
        0x06,  /*         1 */
        0x00,
    };
    /* Draw black fill */
    for (int row = 0; row < 18; row++) {
        unsigned char bits = arrow[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                fb_pixel(mx + col, my + row, BLACK);
            }
        }
    }
    /* White outline (left and top edge) */
    for (int row = 0; row < 16; row++) {
        unsigned char bits = arrow[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                /* Check if this is an edge pixel */
                int left  = (col == 0) || !(bits & (0x80 >> (col-1)));
                int above = (row == 0) || !(arrow[row-1] & (0x80 >> col));
                if (left || above)
                    fb_pixel(mx + col, my + row, WHITE);
                break;  /* only leftmost + top edge */
            }
        }
    }
}

/* ═══════════════════════════════════════════════════════════════════
 *  BeeOS Logo — شعار النحلة في أعلى اليمين
 * ═══════════════════════════════════════════════════════════════════ */

static void draw_logo(void) {
    int lx = (int)g_width - 200;
    int ly = 20;

    /* Hexagonal honeycomb shape */
    fb_rect(lx + 30, ly, 40, 8, GOLD_HONEY);
    fb_rect(lx + 20, ly + 8, 60, 8, GOLD_HONEY);
    fb_rect(lx + 10, ly + 16, 80, 8, GOLD_HONEY);
    fb_rect(lx + 10, ly + 24, 80, 8, GOLD_LIGHT);
    fb_rect(lx + 20, ly + 32, 60, 8, GOLD_LIGHT);
    fb_rect(lx + 30, ly + 40, 40, 8, GOLD_LIGHT);

    /* "BeeOS" text next to logo */
    fb_string(lx + 4, ly + 56, "BeeOS HD", GOLD_HONEY, 2);
    fb_string(lx + 10, ly + 78, "v0.2.0", GRAY_LIGHT, 1);
}

/* ═══════════════════════════════════════════════════════════════════
 *  الدالة الرئيسية: رسم كل شيء
 * ═══════════════════════════════════════════════════════════════════ */

void fb_draw_desktop(void) {
    if (!g_fb || g_width == 0 || g_height == 0) {
        serial_puts("[FB] ERROR: no framebuffer available!\r\n");
        return;
    }

    serial_puts("[FB] Launching Window Manager desktop...\r\n");

    /* تهيئة مدير النوافذ */
    wm_init();

    /* رسم كل شيء عبر مدير النوافذ */
    wm_draw_all();

    serial_puts("[FB] Window Manager desktop active!\r\n");
}
