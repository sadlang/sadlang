"""
BeeOS QEMU Boot Build Script
Compiles everything and runs in QEMU

Toolchain:
  sadc → LLVM IR → llc → .o
  clang → boot.S, vga_printf.c, kernel_boot.c → .o
  ld.lld → beeos.elf
  qemu-system-x86_64 -kernel beeos.elf -serial stdio
"""
import subprocess, os, sys, pathlib, re

ROOT = pathlib.Path(r"c:\s\s-programming-language")
BOOT = ROOT / "bee_os" / "boot"
BUILD = ROOT / "build" / "bin" / "Release"
LLVM = pathlib.Path(r"C:\llvm_dev\LLVM\bin")
QEMU = pathlib.Path(r"C:\qemu")

CLANG   = LLVM / "clang.exe"
LLC     = LLVM / "llc.exe"
LLD     = LLVM / "ld.lld.exe"
QEMU_X64 = QEMU / "qemu-system-x86_64.exe"

SADc = ROOT / "build" / "bin" / "Release" / "sadc.exe"
SAD  = ROOT / "build" / "bin" / "Debug" / "sad.exe"
KERNEL_SAD = ROOT / "bee_os" / "kernel" / "نواة_مترجمة.ص"

OUT = ROOT / "build" / "qemu_boot"
OUT.mkdir(exist_ok=True)

def run(cmd, **kw):
    cmd = [str(x) for x in cmd]
    print("  $ " + " ".join(cmd[:5]) + (" ..." if len(cmd) > 5 else ""))
    r = subprocess.run(cmd, capture_output=True, **kw)
    out = (r.stdout + r.stderr).decode("utf-8", "replace")
    if r.returncode != 0:
        print("  [FAIL]", out[-400:])
        sys.exit(r.returncode)
    return out

print("=" * 60)
print("  BeeOS QEMU Boot Builder")
print("=" * 60)

# ── Step 1: Compile ص kernel to LLVM IR ─────────────────────────
print("\n[1] Compiling نواة_مترجمة.ص → LLVM IR (Windows MSVC target) ...")
kernel_msvc_ll = BUILD / "beeos_kernel.ll"
run([SADc, str(KERNEL_SAD), "--emit-llvm", "-o", str(kernel_msvc_ll)])
print("    ✓ beeos_kernel.ll generated")

# ── Step 2: Patch target triple to Linux ELF ────────────────────
print("\n[2] Patching target triple → x86_64-unknown-linux-musl ...")
kernel_elf_ll = OUT / "beeos_kernel_elf.ll"
content = kernel_msvc_ll.read_text(encoding="utf-8", errors="replace")
content = content.replace(
    'target triple = "x86_64-pc-windows-msvc"',
    'target triple = "x86_64-unknown-linux-musl"'
).replace(
    'target datalayout = "e-m:w-',
    'target datalayout = "e-m:e-'
).replace('dllimport ', '')
kernel_elf_ll.write_text(content, encoding="utf-8")
print("    ✓ target → x86_64-unknown-linux-musl")

# ── Step 3: Compile LLVM IR → object file ───────────────────────
print("\n[3] llc: LLVM IR → kernel_ir.o ...")
kernel_obj = OUT / "kernel_ir.o"
run([LLC, "--march=x86-64", "--relocation-model=static",
     "--filetype=obj", str(kernel_elf_ll), "-o", str(kernel_obj)])
print("    ✓ kernel_ir.o")

# ── Step 4: Compile boot.S ──────────────────────────────────────
print("\n[4] clang: boot.S → boot.o ...")
boot_obj = OUT / "boot.o"
run([CLANG, "--target=x86_64-elf", "-ffreestanding", "-nostdlib",
     "-fno-stack-protector", "-fno-pic",
     "-c", str(BOOT / "boot.S"), "-o", str(boot_obj)])
print("    ✓ boot.o")

# ── Step 5: Compile C files ─────────────────────────────────────
CFLAGS = [
    "--target=x86_64-elf",
    "-ffreestanding", "-nostdlib", "-nostdinc",
    "-fno-stack-protector", "-fno-pic", "-mno-red-zone",
    "-mcmodel=large", "-O2",
    "-I", str(BOOT),
]

for src_name, obj_name in [
    ("vga_printf.c", "vga_printf.o"),
    ("kernel_boot.c", "kernel_boot.o"),
]:
    print(f"\n[5] clang: {src_name} → {obj_name} ...")
    src = BOOT / src_name
    obj = OUT / obj_name
    run([CLANG] + CFLAGS + ["-c", str(src), "-o", str(obj)])
    print(f"    ✓ {obj_name}")

# ── Step 6: Link everything ─────────────────────────────────────
print("\n[6] ld.lld: linking → beeos.elf ...")
beeos_elf = OUT / "beeos.elf"
run([LLD,
     "-T", str(BOOT / "kernel.ld"),
     "--oformat=elf",
     str(boot_obj),
     str(OUT / "vga_printf.o"),
     str(OUT / "kernel_boot.o"),
     str(kernel_obj),
     "-o", str(beeos_elf)])
print("    ✓ beeos.elf")

# ── Verify ELF ──────────────────────────────────────────────────
print("\n[7] Verifying ELF ...")
r = subprocess.run([str(LLVM / "llvm-readelf.exe"), "--file-header", str(beeos_elf)],
                   capture_output=True)
info = (r.stdout + r.stderr).decode("utf-8", "replace")
for line in info.splitlines():
    if any(k in line for k in ["Class", "Type", "Machine", "Entry"]):
        print("   ", line.strip())

file_size = beeos_elf.stat().st_size
print(f"    Size: {file_size:,} bytes  ({file_size//1024} KB)")

# ── Step 7: Run QEMU ─────────────────────────────────────────────
print()
print("=" * 60)
print("  Launching QEMU (serial output follows)...")
print("  Press Ctrl+C to stop")
print("=" * 60)
print()

serial_log = OUT / "serial.log"
if serial_log.exists():
    serial_log.unlink()

try:
    r = subprocess.run([
        str(QEMU_X64),
        "-machine", "q35",
        "-m", "64M",
        "-kernel", str(beeos_elf),
        "-serial", f"file:{serial_log}",
        "-no-reboot",
        "-display", "none",
    ], timeout=8, capture_output=True)
    print(f"QEMU exit code: {r.returncode}")
    qout = (r.stdout + r.stderr).decode("utf-8", "replace")
    if qout.strip():
        print("QEMU stderr:", qout[:300])
except subprocess.TimeoutExpired:
    pass
except KeyboardInterrupt:
    print("\n[!] Stopped")

# Show serial output
import time
time.sleep(1)
if serial_log.exists() and serial_log.stat().st_size > 0:
    print("\n=== Serial Output from BeeOS Kernel ===")
    print(serial_log.read_text(encoding="utf-8", errors="replace")[:3000])
else:
    print("\n[!] No serial output — checking VGA-only mode ...")
    # Try without -nographic (with display window)
    print("    Re-launching QEMU with VGA window ...")
    import threading
    proc = subprocess.Popen([
        str(QEMU_X64),
        "-machine", "q35,kernel-irqchip=off",
        "-m", "64M",
        "-kernel", str(beeos_elf),
        "-serial", f"file:{serial_log}",
        "-no-reboot",
    ])
    time.sleep(8)
    proc.terminate()
    if serial_log.exists() and serial_log.stat().st_size > 0:
        print("\n=== Serial Output ===")
        print(serial_log.read_text(encoding="utf-8", errors="replace")[:3000])
    else:
        print("[!] Still no serial output — kernel may have crashed at boot")
