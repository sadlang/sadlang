/**
 * رابط.ld — سكريبت الرابط لنظام النحلة BeeOS
 * BeeOS Linker Script — Kernel Memory Layout
 *
 * خريطة الذاكرة:
 *   0x0000_0000 - 0x0010_0000 : منطقة الـ 1MB الأولى (محجوزة للـ BIOS/Multiboot)
 *   0x0010_0000              : نقطة تحميل النواة (1MB)
 *   text / rodata / data / bss : وراء بعضها
 *
 * بسم الله الرحمن الرحيم
 */

ENTRY(_start)

SECTIONS {
    /* تبدأ النواة عند 1MB */
    . = 0x100000;

    /* ─── code ─── */
    .text ALIGN(4096) : {
        /* رأس Multiboot2 أولاً */
        KEEP(*(.multiboot2_header))
        *(.text)
        *(.text.*)
    }

    /* ─── read-only data ─── */
    .rodata ALIGN(4096) : {
        *(.rodata)
        *(.rodata.*)
    }

    /* ─── data ─── */
    .data ALIGN(4096) : {
        *(.data)
        *(.data.*)
    }

    /* ─── BSS (صفر بالإقلاع) ─── */
    .bss ALIGN(4096) : {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }

    /* نهاية النواة */
    kernel_end = .;

    /* احتفظ بجداول الصفحات في 0x1000 */
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.*)
    }
}
