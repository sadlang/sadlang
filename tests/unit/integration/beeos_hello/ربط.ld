/* ════════════════════════════════════════════════════════════════════════════════
 * سكربت ربط نواة نظام النحلة — x86_64 bare metal
 * BeeOS Kernel Linker Script — x86_64 bare metal
 *
 * يُحدد تخطيط الذاكرة للنواة المُصغّرة
 * Defines memory layout for the mini kernel
 * ════════════════════════════════════════════════════════════════════════════════ */

ENTRY(_start)

SECTIONS
{
    /* تحميل النواة عند 1MB / Load kernel at 1MB */
    . = 1M;

    /* ═══ رأس Multiboot / Multiboot Header ═══ */
    .multiboot ALIGN(4) :
    {
        KEEP(*(.multiboot))
    }

    /* ═══ الكود / Code ═══ */
    .text ALIGN(4K) :
    {
        *(.text)
        *(.text.*)
    }

    /* ═══ بيانات القراءة فقط / Read-only data ═══ */
    .rodata ALIGN(4K) :
    {
        *(.rodata)
        *(.rodata.*)
    }

    /* ═══ بيانات مُهيأة / Initialized data ═══ */
    .data ALIGN(4K) :
    {
        *(.data)
        *(.data.*)
    }

    /* ═══ جداول الصفحات / Page tables ═══ */
    .page_tables ALIGN(4K) :
    {
        *(.page_tables)
    }

    /* ═══ جدول المقاطعات / Interrupt table ═══ */
    .interrupt_table ALIGN(16) :
    {
        KEEP(*(.interrupt_table))
    }

    /* ═══ بيانات غير مُهيأة / Uninitialized data (BSS) ═══ */
    .bss ALIGN(4K) :
    {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    }

    /* ═══ المكدس / Stack ═══ */
    . = ALIGN(16);
    __stack_bottom = .;
    . += 16K;  /* مكدس 16KB / 16KB stack */
    __stack_top = .;

    /* ═══ نهاية النواة / End of kernel ═══ */
    __kernel_end = .;

    /* ═══ تنظيف / Cleanup ═══ */
    /DISCARD/ :
    {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}

/* ═══ تصريحات / Assertions ═══ */
ASSERT(__kernel_end - 1M < 64K, "خطأ: النواة أكبر من 64KB / Error: Kernel exceeds 64KB")
