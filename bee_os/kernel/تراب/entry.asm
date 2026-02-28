; ============================================================================
; entry.asm — نقطة دخول نظام النحلة BeeOS
; BeeOS Boot Entry — Multiboot2 Protocol
;
; الإقلاع: عبر GRUB2 / QEMU -kernel
; البروتوكول: Multiboot2
; المعمارية: x86_64
;
; مسار الإقلاع:
;   QEMU/GRUB → _start (هنا)
;   → تحقق من Multiboot2 Magic
;   → تجهيز Stack + GDT
;   → الانتقال للوضع 64-bit (Long Mode)
;   → استدعاء نواة_رئيسية() في ص
;
; بسم الله الرحمن الرحيم
; ============================================================================

BITS 32
SECTION .text

; ============================================================================
; رأس Multiboot2 — يجب أن يكون في أول 32KB من الصورة
; ============================================================================

ALIGN 8
multiboot2_header:
    DD 0xE85250D6                   ; Magic Multiboot2
    DD 0                             ; Architecture: i386 (ثم نتحول ل 64-bit)
    DD (multiboot2_header_end - multiboot2_header)
    DD -(0xE85250D6 + 0 + (multiboot2_header_end - multiboot2_header))

; علامة الإنهاء (مطلوبة)
.end_tag:
    DW 0    ; type = 0
    DW 0    ; flags
    DD 8    ; size
multiboot2_header_end:

; ============================================================================
; نقطة الدخول الرئيسية _start
; EAX = Magic (0x36d76289 إذا Multiboot2)
; EBX = عنوان بنية معلومات الإقلاع
; ============================================================================

GLOBAL _start
_start:
    ; أوقف المقاطعات مؤقتاً خلال الإقلاع
    CLI

    ; تجهيز المكدس (Stack) — 16KB
    MOV ESP, stack_top

    ; احفظ معلومات Multiboot2
    MOV [multiboot2_magic],  EAX
    MOV [multiboot2_info],   EBX

    ; تحقق من Magic
    CMP EAX, 0x36d76289
    JNE .no_multiboot

    ; اطبع رسالة OK عبر VGA (بسيطة)
    MOV WORD [0xB8000], 0x0F42   ; 'B' أبيض على أسود
    MOV WORD [0xB8002], 0x0F65   ; 'e'
    MOV WORD [0xB8004], 0x0F65   ; 'e'
    MOV WORD [0xB8006], 0x0F4F   ; 'O'
    MOV WORD [0xB8008], 0x0F53   ; 'S'

    ; ===================================================
    ; الانتقال إلى Long Mode (64-bit)
    ; ===================================================

    ; 1. تحقق من دعم CPUID و Long Mode
    ; (مبسّط — في الإنتاج نتحقق فعلاً)

    ; 2. تهيئة جداول الصفحات (Identity Mapping)
    CALL setup_paging

    ; 3. تفعيل PAE
    MOV EAX, CR4
    OR  EAX, (1 << 5)   ; PAE bit
    MOV CR4, EAX

    ; 4. تعيين LME في IA32_EFER MSR
    MOV ECX, 0xC0000080
    RDMSR
    OR  EAX, (1 << 8)   ; LME
    WRMSR

    ; 5. تفعيل Paging + Protected Mode
    MOV EAX, CR0
    OR  EAX, (1 << 31) | (1 << 0)
    MOV CR0, EAX

    ; 6. القفز إلى قطاع 64-bit
    LGDT [gdt64_ptr]
    JMP  0x08:.long_mode_entry

    BITS 64
.long_mode_entry:
    ; تعيين Data Segments
    MOV AX, 0x10
    MOV DS, AX
    MOV ES, AX
    MOV FS, AX
    MOV GS, AX
    MOV SS, AX

    ; استدعاء نواة ص الرئيسية
    ; EXTERN sad_kernel_main
    ; CALL sad_kernel_main

    ; مؤقتاً: حلقة هلت
    JMP .halt_loop

    BITS 32
.no_multiboot:
    ; اطبع خطأ على VGA
    MOV WORD [0xB8000], 0x4F45   ; 'E' أحمر
    MOV WORD [0xB8002], 0x4F52   ; 'R'
    MOV WORD [0xB8004], 0x4F52   ; 'R'
    MOV WORD [0xB8006], 0x4F21   ; '!'

    BITS 64
.halt_loop:
    CLI
    HLT
    JMP .halt_loop

; ============================================================================
; تهيئة جداول الصفحات — Identity Mapping للـ 4GB الأولى
; ============================================================================
    BITS 32
setup_paging:
    ; مسح منطقة جداول الصفحات (4 جداول × 4KB)
    MOV EDI, 0x1000
    MOV ECX, 1024 * 4
    XOR EAX, EAX
    REP STOSD

    ; PML4[0] → PDPT (0x2000)
    MOV DWORD [0x1000], 0x2003

    ; PDPT[0] → PD (0x3000)
    MOV DWORD [0x2000], 0x3003

    ; PD: Identity Map أول 1GB بصفحات 2MB
    MOV EDI, 0x3000
    MOV EAX, 0x000083  ; 2MB page | Present | Writable
    MOV ECX, 512
.pd_loop:
    MOV DWORD [EDI], EAX
    ADD EAX, 0x200000   ; +2MB
    ADD EDI, 8
    LOOP .pd_loop

    ; تعيين CR3 → PML4
    MOV EAX, 0x1000
    MOV CR3, EAX
    RET

; ============================================================================
; GDT 64-bit
; ============================================================================

ALIGN 8
gdt64:
    DQ 0                        ; Null Descriptor
    ; Code Segment 64-bit
    DQ (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
    ; Data Segment
    DQ (1 << 44) | (1 << 47) | (1 << 41)
gdt64_end:

gdt64_ptr:
    DW gdt64_end - gdt64 - 1   ; Limit
    DD gdt64                    ; Base (32-bit)

; ============================================================================
; بيانات Multiboot2
; ============================================================================

SECTION .data
ALIGN 8
multiboot2_magic: DD 0
multiboot2_info:  DD 0

; ============================================================================
; المكدس (Stack) — 16KB
; ============================================================================

SECTION .bss
ALIGN 16
stack_bottom:
    RESB 16384   ; 16KB
stack_top:
