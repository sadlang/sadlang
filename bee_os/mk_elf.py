#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
BeeOS ELF32 Builder v0.2 - Multiboot1 + NASM + x86 real code
"""
import struct, subprocess, sys, os, tempfile, shutil

NASM = r"C:\NASM\nasm.exe"
LOAD_ADDR = 0x100000   # 1MB standard

KERNEL_ASM = """
[BITS 32]
[org 0x100000]

MULTIBOOT_MAGIC  equ 0x1BADB002
MULTIBOOT_FLAGS  equ 0x00000000
MULTIBOOT_CKSUM  equ (-(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)) & 0xFFFFFFFF

mb_header:
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CKSUM

kernel_start:
    cli
    mov esp, 0x200000

    mov dx, 0x3F9
    xor al, al
    out dx, al

    mov dx, 0x3FB
    mov al, 0x80
    out dx, al

    mov dx, 0x3F8
    mov al, 0x01
    out dx, al

    mov dx, 0x3F9
    xor al, al
    out dx, al

    mov dx, 0x3FB
    mov al, 0x03
    out dx, al

    mov dx, 0x3FC
    mov al, 0x03
    out dx, al

    mov esi, msg_banner
    call serial_puts

    mov esi, msg_ok
    call serial_puts

    mov edi, 0x000B8000
    mov esi, msg_vga
    xor ecx, ecx
.vga_loop:
    mov al, [esi + ecx]
    test al, al
    jz .vga_done
    mov ah, 0x0A
    mov [edi], ax
    add edi, 2
    inc ecx
    jmp .vga_loop
.vga_done:
.halt:
    hlt
    jmp .halt

serial_puts:
    push eax
    push edx
.sp_loop:
    mov al, [esi]
    test al, al
    jz .sp_done
.sp_wait:
    mov dx, 0x3FD
    in al, dx
    and al, 0x20
    jz .sp_wait
    mov dx, 0x3F8
    mov al, [esi]
    out dx, al
    inc esi
    jmp .sp_loop
.sp_done:
    pop edx
    pop eax
    ret

msg_banner: db 13, 10
            db "====================================", 13, 10
            db " BeeOS v0.1  Bismillah!             ", 13, 10
            db " Allah Akbar! Kernel is alive!      ", 13, 10
            db "====================================", 13, 10, 0
msg_ok:     db "[BeeOS] Multiboot1 OK -- Serial OK", 13, 10, 0
msg_vga:    db "BeeOS: Bismillah Alhamdulillah!", 0
"""

def wrap_elf32(code, load_addr, entry_offset):
    ELF_HDR = 52
    PHDR    = 32
    FOFFSET = ELF_HDR + PHDR

    e_ident = (b'\x7fELF' + b'\x01\x01\x01\x00' + b'\x00'*8)
    elf_hdr = e_ident + struct.pack('<HHIIIIIHHHHHH',
        2, 3, 1,
        load_addr + entry_offset,
        ELF_HDR, 0, 0,
        ELF_HDR, PHDR, 1, 40, 0, 0,
    )
    phdr = struct.pack('<IIIIIIII',
        1, FOFFSET, load_addr, load_addr,
        len(code), len(code), 5, 0x1000,
    )
    return elf_hdr + phdr + code

def main():
    print("=" * 50)
    print("  BeeOS ELF32 Builder v0.2 (Multiboot1+NASM)")
    print("=" * 50)

    tmp = tempfile.mkdtemp(prefix="beeos_")
    asm_path = os.path.join(tmp, "kernel.asm")
    bin_path = os.path.join(tmp, "kernel.bin")

    try:
        with open(asm_path, "w", encoding="utf-8") as f:
            f.write(KERNEL_ASM)

        print("[mk] Assembling x86 kernel code...")
        r = subprocess.run(
            [NASM, "-f", "bin", "-o", bin_path, asm_path],
            capture_output=True, text=True
        )
        if r.returncode != 0:
            print("NASM error:\n" + r.stderr)
            sys.exit(1)

        with open(bin_path, "rb") as f:
            code = f.read()

        print(f"[mk] Code size: {len(code)} bytes")

        magic = struct.unpack_from('<I', code, 0)[0]
        flags = struct.unpack_from('<I', code, 4)[0]
        cksum = struct.unpack_from('<I', code, 8)[0]
        if (magic + flags + cksum) & 0xFFFFFFFF == 0:
            print(f"[mk] Multiboot1 checksum OK (magic=0x{magic:08X})")
        else:
            print("[mk] WARNING: Multiboot1 checksum BAD!")

        elf = wrap_elf32(code, LOAD_ADDR, 12)

        os.makedirs("bee_os/build", exist_ok=True)
        out = "bee_os/build/BeeOS.elf"
        with open(out, "wb") as f:
            f.write(elf)

        print(f"[mk] ELF32 size: {len(elf)} bytes")
        print(f"[mk] Saved: {out}")
        print()
        print("Run on QEMU:")
        print(f"  qemu-system-x86_64 -kernel {out} -m 128M -serial stdio -display none -no-reboot")
        print()
        print("BeeOS: Bismillah - Allahu Akbar! Ready to boot!")

    finally:
        shutil.rmtree(tmp, ignore_errors=True)

if __name__ == "__main__":
    main()
