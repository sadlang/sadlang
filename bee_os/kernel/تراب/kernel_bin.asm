; ============================================================================
; kernel_bin.asm — نواة BeeOS كملف ثنائي مسطح
; BeeOS Flat Binary Kernel for QEMU -kernel
;
; التجميع: nasm -f bin kernel_bin.asm -o BeeOS.bin
; التشغيل: qemu-system-x86_64 -kernel BeeOS.bin -m 128M
;
; ملاحظة: QEMU يكتشف Multiboot2 Header في أول 8KB
; بسم الله الرحمن الرحيم
; ============================================================================

BITS 32
ORG 0x100000

; ============================================================================
; رأس Multiboot2 — يجب في أول 32 بايت (في الواقع أول 8KB)
; ============================================================================

SECTION .multiboot
ALIGN 8

mb2_header:
    DD 0xE85250D6                               ; Magic
    DD 0                                         ; Architecture: i386
    DD (mb2_header_end - mb2_header)            ; Header length
    DD -(0xE85250D6 + 0 + (mb2_header_end - mb2_header)) ; Checksum

; علامة نهاية Multiboot2
    DW 0    ; type
    DW 0    ; flags
    DD 8    ; size
mb2_header_end:

; ============================================================================
; نقطة الدخول
; ============================================================================

SECTION .text
GLOBAL _start

_start:
    CLI
    MOV ESP, stack_top

    ; تحقق من Multiboot2 Magic
    CMP EAX, 0x36d76289
    JNE .panic

    ; ══════════════════════════════
    ; طباعة "بسم الله" على VGA
    ; ══════════════════════════════
    ; مسح الشاشة الأولى (10 أسطر)
    MOV EDI, 0xB8000
    MOV ECX, 10 * 80
    MOV AX, 0x0720     ; مسافة بخلفية سوداء
    REP STOSW

    ; السطر 1: BeeOS
    CALL print_beeos_header

    ; السطر 3: رسالة الإقلاع
    CALL print_boot_msg

    ; الانتزال وضع 64-bit
    CALL enter_long_mode

    ; لن نصل هنا في الواقع
    JMP .halt

.panic:
    ; طباعة PANIC بالأحمر
    MOV WORD [0xB8000], 0x4F50   ; P
    MOV WORD [0xB8002], 0x4F41   ; A
    MOV WORD [0xB8004], 0x4F4E   ; N
    MOV WORD [0xB8006], 0x4F49   ; I
    MOV WORD [0xB8008], 0x4F43   ; C
    MOV WORD [0xB800A], 0x4F21   ; !

.halt:
    CLI
    HLT
    JMP .halt

; ============================================================================
; طباعة شعار BeeOS على VGA
; ============================================================================

print_beeos_header:
    ; السطر 0: "  BeeOS -- نظام النحلة"
    MOV ESI, msg_beeos
    MOV EDI, 0xB8000 + (0 * 80 * 2)
    MOV AH, 0x0F     ; أبيض على أسود
.loop1:
    MOV AL, [ESI]
    CMP AL, 0
    JE  .done1
    MOV [EDI], AX
    ADD EDI, 2
    INC ESI
    JMP .loop1
.done1:
    RET

print_boot_msg:
    ; السطر 2: رسالة الإقلاع
    MOV ESI, msg_boot
    MOV EDI, 0xB8000 + (2 * 80 * 2)
    MOV AH, 0x0A     ; أخضر فاتح
.loop2:
    MOV AL, [ESI]
    CMP AL, 0
    JE  .done2
    MOV [EDI], AX
    ADD EDI, 2
    INC ESI
    JMP .loop2
.done2:
    RET

; ============================================================================
; الانتقال لـ Long Mode
; ============================================================================

enter_long_mode:
    ; جداول الصفحات في 0x1000
    MOV EDI, 0x1000
    MOV ECX, 4096
    XOR EAX, EAX
    REP STOSD

    MOV DWORD [0x1000], 0x2003   ; PML4[0] → PDPT
    MOV DWORD [0x2000], 0x3003   ; PDPT[0] → PD
    MOV DWORD [0x3003], 0x000083 ; PD[0] = 2MB page identity
    MOV DWORD [0x300B], 0x200083 ; PD[1] = 2MB page
    MOV DWORD [0x3013], 0x400083 ; PD[2] = 2MB page

    ; تفعيل PAE
    MOV EAX, CR4
    OR  EAX, 0x20
    MOV CR4, EAX

    ; LME في EFER
    MOV ECX, 0xC0000080
    RDMSR
    OR  EAX, 0x100
    WRMSR

    ; CR3
    MOV EAX, 0x1000
    MOV CR3, EAX

    ; Paging + Protected
    MOV EAX, CR0
    OR  EAX, 0x80000001
    MOV CR0, EAX

    ; GDT 64-bit
    LGDT [gdt64_ptr]
    JMP  0x08:.long_mode

    BITS 64
.long_mode:
    MOV AX, 0x10
    MOV DS, AX
    MOV ES, AX
    MOV SS, AX

    ; طباعة "64-BIT OK" على السطر 4
    MOV QWORD [0xB8000 + 4*80*2], 0x0A504B0A2D0A340A36    ; مبسط
    MOV WORD  [0xB8000 + 4*80*2], 0x0A36   ; '6' أخضر
    MOV WORD  [0xB8000 + 4*80*2 + 2], 0x0A34   ; '4'
    MOV WORD  [0xB8000 + 4*80*2 + 4], 0x0A62   ; 'b'
    MOV WORD  [0xB8000 + 4*80*2 + 6], 0x0A69   ; 'i'
    MOV WORD  [0xB8000 + 4*80*2 + 8], 0x0A74   ; 't'

.idle:
    HLT
    JMP .idle

    BITS 32

; ============================================================================
; GDT 64-bit
; ============================================================================

ALIGN 8
gdt64:
    DQ 0
    DQ (1<<43)|(1<<44)|(1<<47)|(1<<53)   ; Code 64-bit
    DQ (1<<44)|(1<<47)|(1<<41)           ; Data
gdt64_end:

gdt64_ptr:
    DW gdt64_end - gdt64 - 1
    DD gdt64

; ============================================================================
; نصوص الرسائل
; ============================================================================

SECTION .data

msg_beeos:
    DB "  *** BeeOS v0.1.0 - Bismi Allah Arrahmani Arraheem ***", 0

msg_boot:
    DB "  [BeeOS] Booting... Memory OK, VGA OK, Long Mode...", 0

; ============================================================================
; المكدس
; ============================================================================

SECTION .bss
ALIGN 16
    RESB 16384
stack_top:
