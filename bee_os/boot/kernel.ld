/* BeeOS Kernel Linker Script — ELF32 container with x86_64 code */

ENTRY(_start)

SECTIONS
{
    /* Load kernel at 1MB physical (standard for Multiboot kernels) */
    . = 1M;

    /* Multiboot1 header must be within first 8KB */
    .mb1hdr : {
        *(.mb1hdr)
    }

    /* All code + rodata */
    .text ALIGN(4096) : {
        *(.text*)
        *(.rodata*)
    }

    /* Initialized data */
    .data ALIGN(4096) : {
        *(.data*)
    }

    /* Uninitialized data */
    .bss ALIGN(4096) : {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
        _bss_end = .;
    }

    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
        *(.gnu*)
    }
}
