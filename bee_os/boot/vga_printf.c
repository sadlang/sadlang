/*
 * BeeOS Kernel VGA + Serial Printf
 * VGA text mode 80×25 + COM1 serial output
 * بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ
 */

#include "beeos_kernel.h"

/* ---- VGA Text Mode (0xB8000) ---- */
#define VGA_ADDR  0xB8000UL
#define VGA_COLS  80
#define VGA_ROWS  25

#define COLOR(fg, bg) ((unsigned char)(((bg) << 4) | (fg)))

static volatile unsigned short *vga = (volatile unsigned short *)VGA_ADDR;
static int vga_row = 0;
static int vga_col = 0;
static unsigned char vga_color = COLOR(7, 1);  /* white on blue */

void vga_set_color(unsigned char c) { vga_color = c; }

static void vga_scroll(void) {
    for (int r = 0; r < VGA_ROWS - 1; r++)
        for (int c = 0; c < VGA_COLS; c++)
            vga[r * VGA_COLS + c] = vga[(r + 1) * VGA_COLS + c];
    for (int c = 0; c < VGA_COLS; c++)
        vga[(VGA_ROWS - 1) * VGA_COLS + c] = ((unsigned short)vga_color << 8) | ' ';
    vga_row = VGA_ROWS - 1;
}

void vga_clear(unsigned char color) {
    vga_color = color;
    for (int i = 0; i < VGA_ROWS * VGA_COLS; i++)
        vga[i] = ((unsigned short)color << 8) | ' ';
    vga_row = 0;
    vga_col = 0;
}

void vga_putc(char c) {
    if (c == '\r') { vga_col = 0; return; }
    if (c == '\n') {
        vga_col = 0;
        vga_row++;
        if (vga_row >= VGA_ROWS) vga_scroll();
        return;
    }
    if (c == '\t') {
        vga_col = (vga_col + 8) & ~7;
        if (vga_col >= VGA_COLS) { vga_col = 0; vga_row++; }
        return;
    }
    if (vga_col >= VGA_COLS) {
        vga_col = 0;
        vga_row++;
        if (vga_row >= VGA_ROWS) vga_scroll();
    }
    vga[vga_row * VGA_COLS + vga_col] = ((unsigned short)vga_color << 8) | (unsigned char)c;
    vga_col++;
}

void vga_puts(const char *s) {
    if (!s) return;
    while (*s) vga_putc(*s++);
}

/* ---- Serial Port COM1 (0x3F8) for nographic mode ---- */
#define COM1 0x3F8

static inline void outb(unsigned short port, unsigned char val) {
    __asm__ volatile("outb %0,%1" :: "a"(val), "Nd"(port));
}
static inline unsigned char inb(unsigned short port) {
    unsigned char ret;
    __asm__ volatile("inb %1,%0" : "=a"(ret) : "Nd"(port));
    return ret;
}

void serial_init(void) {
    outb(COM1 + 1, 0x00);  /* disable interrupts */
    outb(COM1 + 3, 0x80);  /* enable DLAB */
    outb(COM1 + 0, 0x03);  /* baud rate 38400 divisor low */
    outb(COM1 + 1, 0x00);  /* divisor high */
    outb(COM1 + 3, 0x03);  /* 8N1 */
    outb(COM1 + 2, 0xC7);  /* enable FIFO */
    outb(COM1 + 4, 0x0B);  /* IRQs, RTS/DSR */
}

void serial_putc(char c) {
    while ((inb(COM1 + 5) & 0x20) == 0);
    outb(COM1, (unsigned char)c);
    if (c == '\n') {
        while ((inb(COM1 + 5) & 0x20) == 0);
        outb(COM1, '\r');
    }
}

void serial_puts(const char *s) {
    if (!s) return;
    while (*s) serial_putc(*s++);
}

/* ---- Combined output (VGA + Serial) ---- */
static void kputc(char c) {
    vga_putc(c);
    serial_putc(c);
}

static void kputs(const char *s) {
    if (!s) s = "(null)";
    while (*s) kputc(*s++);
}

/* ---- Integer to string helpers ---- */
static void kitoa_dec(long long n, char *buf, int *len) {
    *len = 0;
    int neg = 0;
    if (n < 0) { neg = 1; n = -n; }
    if (n == 0) { buf[(*len)++] = '0'; }
    else {
        char tmp[24];
        int t = 0;
        while (n > 0) { tmp[t++] = '0' + (n % 10); n /= 10; }
        for (int i = t - 1; i >= 0; i--) buf[(*len)++] = tmp[i];
    }
    if (neg) { /* insert - at front */
        for (int i = *len; i > 0; i--) buf[i] = buf[i-1];
        buf[0] = '-';
        (*len)++;
    }
    buf[*len] = 0;
}

static void kitoa_hex(unsigned long long n, char *buf, int *len) {
    static const char h[] = "0123456789abcdef";
    *len = 0;
    if (n == 0) { buf[(*len)++] = '0'; buf[*len] = 0; return; }
    char tmp[20];
    int t = 0;
    while (n > 0) { tmp[t++] = h[n & 0xF]; n >>= 4; }
    for (int i = t - 1; i >= 0; i--) buf[(*len)++] = tmp[i];
    buf[*len] = 0;
}

/* ---- printf (i686 cdecl — no va_list, raw stack access) ----
   The ص IR always calls: printf("%s\0", string_ptr)
   On i686 cdecl: [esp+4]=fmt, [esp+8]=first_arg, ...
   We access args directly via &fmt+1 (next 4-byte slot after fmt param).
 ---- */
int printf(const char *fmt, ...) {
    if (!fmt) return 0;

    /* &fmt + 1 == pointer to first variadic argument on stack (i686 cdecl) */
    const void **argp = (const void **)(&fmt + 1);

    for (const char *p = fmt; *p; p++) {
        if (*p != '%') {
            kputc(*p);
        } else {
            p++;
            /* skip length modifier */
            while (*p == 'l' || *p == 'h' || *p == 'z') p++;
            switch (*p) {
            case 's': {
                const char *s = (const char *)(*argp++);
                kputs(s ? s : "(null)");
                break;
            }
            case 'd': case 'i': {
                int n = (int)(long)(*argp++);
                char buf[12]; int i = 11;
                buf[i] = 0;
                int neg = 0;
                if (n < 0) { neg = 1; n = -n; }
                if (n == 0) { buf[--i] = '0'; }
                else { while (n > 0) { buf[--i] = '0' + n%10; n /= 10; } }
                if (neg) buf[--i] = '-';
                kputs(buf + i);
                break;
            }
            case 'x': case 'X': {
                unsigned int n = (unsigned int)(unsigned long)(*argp++);
                char buf[10]; int i = 9;
                static const char h[] = "0123456789abcdef";
                buf[i] = 0;
                if (n == 0) { buf[--i] = '0'; }
                else { while (n > 0) { buf[--i] = h[n&15]; n >>= 4; } }
                if (*p == 'X') {
                    for (char *q = buf+i; *q; q++) if (*q>='a') *q -= 32;
                }
                kputs(buf + i);
                break;
            }
            case 'c': {
                int c = (int)(long)(*argp++);
                kputc((char)c);
                break;
            }
            case 'p': {
                unsigned int n = (unsigned int)(unsigned long)(*argp++);
                kputs("0x");
                char buf[10]; int i = 9;
                static const char h[] = "0123456789abcdef";
                buf[i] = 0;
                if (n == 0) { buf[--i] = '0'; }
                else { while (n > 0) { buf[--i] = h[n&15]; n >>= 4; } }
                kputs(buf + i);
                break;
            }
            case '%':
                kputc('%');
                break;
            default:
                kputc('%'); kputc(*p);
                break;
            }
        }
    }
    return 0;
}

int puts(const char *s) {
    kputs(s);
    kputc('\n');
    return 0;
}
