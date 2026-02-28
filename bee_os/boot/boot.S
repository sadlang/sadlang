/* ================================================================
   BeeOS Boot Stub — Multiboot1, 32-bit Protected Mode
   Simple: no long mode needed (kernel runs in 32-bit PM)
   بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ
   ================================================================ */

/* Multiboot1 magic values */
#define MB1_MAGIC    0x1BADB002
#define MB1_FLAGS    0x00000003   /* ALIGN(1) + MEM_INFO(2) */
#define MB1_CHECKSUM (-(MB1_MAGIC + MB1_FLAGS))

/* ---- Multiboot1 Header (aligned, within first 8KB) ---- */
.section .mb1hdr, "a"
.align 4
.long MB1_MAGIC
.long MB1_FLAGS
.long MB1_CHECKSUM

/* ---- 32-bit entry ---- */
.section .text
.code32
.global _start
.type _start, @function
_start:
    cli
    /* Load our stack */
    mov $(_stk_top), %esp
    and $-16, %esp

    /* eax = 0x2BADB002 (Multiboot magic), ebx = Multiboot info ptr */
    /* Pass both to kernel_boot_main(magic, info_ptr) via cdecl */
    push %ebx
    push %eax
    call kernel_boot_main
    add $8, %esp

    /* Should never return */
.Lhalt:
    cli
    hlt
    jmp .Lhalt

/* ---- BSS: 16KB kernel stack ---- */
.section .bss
.align 16
_stk:     .space 16384
_stk_top:
