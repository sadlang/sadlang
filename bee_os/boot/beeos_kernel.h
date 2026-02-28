/*
 * BeeOS Kernel Headers
 * Shared declarations for boot stubs (32-bit i686)
 * بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ
 */
#pragma once

/* VGA helpers */
void vga_clear(unsigned char color);
void vga_set_color(unsigned char c);
void vga_putc(char c);
void vga_puts(const char *s);

/* Color macro: fg (0-15) + bg (0-7) */
#define COLOR(fg, bg) ((unsigned char)(((bg) << 4) | (fg)))

/* Serial helpers */
void serial_init(void);
void serial_putc(char c);
void serial_puts(const char *s);

/* printf (provided by vga_printf.c, used by ص IR) */
int printf(const char *fmt, ...);
int puts(const char *s);

/* Framebuffer Desktop (provided by fb_desktop.c) */
void fb_init(unsigned int *addr, unsigned int w, unsigned int h, unsigned int pitch);
void fb_draw_desktop(void);

/* Bochs VGA Adapter init (provided by fb_desktop.c) */
int bga_init(unsigned int width, unsigned int height);

/* Settings UI (provided by settings_ui.c) */
void settings_draw(int tab);
void settings_draw_default(void);

/* Window Manager (provided by window_manager.c) */
void wm_init(void);
void wm_draw_all(void);
