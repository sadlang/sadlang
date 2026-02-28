"""Rebuild BeeOS kernel: i686 32-bit Multiboot1"""
import subprocess, pathlib, sys

ROOT = pathlib.Path(r'c:\s\s-programming-language')
BOOT = ROOT / 'bee_os' / 'boot'
OUT  = ROOT / 'build' / 'qemu_boot'
LLVM = pathlib.Path(r'C:\llvm_dev\LLVM\bin')
CLANG = str(LLVM / 'clang.exe')
LLC   = str(LLVM / 'llc.exe')
LLD   = str(LLVM / 'ld.lld.exe')
SADc  = str(ROOT / 'build' / 'bin' / 'Release' / 'sadc.exe')

OUT.mkdir(exist_ok=True)

def run(cmd, ok_msg=None):
    cmd = [str(x) for x in cmd]
    print('  $', cmd[0].split('\\')[-1], '...')
    r = subprocess.run(cmd, capture_output=True)
    out = (r.stdout + r.stderr).decode('utf-8', 'replace')
    if r.returncode != 0:
        print('[FAIL]', out[-500:])
        sys.exit(1)
    if ok_msg:
        print(' ', ok_msg)
    return out

print('='*60)
print('  BeeOS HD i686 Multiboot1 Builder')
print('='*60)

# 1: sadc -> LLVM IR
print('\n[1] sadc LLVM IR ...')
msvc_ll  = OUT / 'beeos_kernel.ll'
run([SADc, ROOT / 'bee_os' / 'kernel' / '\u0646\u0648\u0627\u0629_hd.\u0635',
     '--emit-llvm', '-o', msvc_ll], 'beeos_kernel.ll OK')

# 2: Patch target to i686
print('\n[2] Patch target -> i686 ...')
i686_ll = OUT / 'beeos_kernel_i686.ll'
txt = msvc_ll.read_text(encoding='utf-8', errors='replace')
txt = (txt
       .replace('target triple = "x86_64-pc-windows-msvc"',
                'target triple = "i686-unknown-linux-musl"')
       .replace('target datalayout = "e-m:w-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"',
                'target datalayout = "e-m:e-p:32:32-i64:32-f64:32:64-f80:32-n8:16:32-S128"')
       .replace('dllimport ', ''))
i686_ll.write_text(txt, encoding='utf-8')
print('  target -> i686-unknown-linux-musl OK')

# 3: llc IR -> i686 obj
print('\n[3] llc: IR -> kernel_ir_i686.o ...')
kernel_obj = OUT / 'kernel_ir_i686.o'
run([LLC, '--march=x86', '--relocation-model=static',
     '--filetype=obj', str(i686_ll), '-o', kernel_obj], 'kernel_ir_i686.o OK')

# 4: boot.S
print('\n[4] boot.S -> boot.o ...')
boot_obj = OUT / 'boot.o'
run([CLANG, '--target=i686-elf', '-ffreestanding', '-nostdlib',
     '-fno-stack-protector', '-fno-pic',
     '-c', BOOT / 'boot.S', '-o', boot_obj], 'boot.o OK')

# 5: C files
CFLAGS = ['--target=i686-elf', '-ffreestanding', '-nostdlib', '-nostdinc',
          '-fno-stack-protector', '-fno-pic', '-O2',
          '-mno-sse', '-mno-sse2', '-mno-mmx', '-mno-80387',
          '-I', str(BOOT)]
for src, obj in [('vga_printf.c', 'vga_printf_i686.o'),
                 ('kernel_boot.c', 'kernel_boot_i686.o'),
                 ('fb_desktop.c', 'fb_desktop_i686.o'),
                 ('settings_ui.c', 'settings_ui_i686.o'),
                 ('window_manager.c', 'window_manager_i686.o')]:
    print(f'\n[5] {src} ...')
    run([CLANG] + CFLAGS + ['-c', BOOT / src, '-o', OUT / obj], f'{obj} OK')

# 6: Link as ELF32
print('\n[6] Linking -> beeos32.elf ...')
elf = OUT / 'beeos32.elf'
run([LLD, '-T', BOOT / 'kernel.ld', '-m', 'elf_i386',
     str(boot_obj),
     str(OUT / 'vga_printf_i686.o'),
     str(OUT / 'kernel_boot_i686.o'),
     str(OUT / 'fb_desktop_i686.o'),
     str(OUT / 'settings_ui_i686.o'),
     str(OUT / 'window_manager_i686.o'),
     str(kernel_obj),
     '-o', elf], 'beeos32.elf OK')

sz = elf.stat().st_size
print(f'\n  beeos32.elf: {sz:,} bytes ({sz//1024} KB)')

# 7: Verify
r = subprocess.run([str(LLVM / 'llvm-readelf.exe'), '--file-header', str(elf)],
                   capture_output=True)
for line in (r.stdout + r.stderr).decode('utf-8', 'replace').splitlines():
    if any(k in line for k in ['Class', 'Machine', 'Entry', 'Type']):
        print('  ', line.strip())
print('\nRun: C:\\qemu\\qemu-system-i386.exe -m 64M -kernel', elf, '-serial file:serial.log\n')
