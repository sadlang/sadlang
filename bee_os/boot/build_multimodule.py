"""
BeeOS Multi-Module QEMU Boot Build Script
Compiles all 11 kernel ص modules + C stubs, links into ELF, boots QEMU.

Toolchain:
  sadc → LLVM IR (.ll) per module
  patch target → x86_64-unknown-linux-musl (ELF)
  llc → .o per module
  clang → boot.S, C stubs → .o
  ld.lld → beeos.elf
  qemu-system-x86_64 -kernel beeos.elf -serial stdio

بِسْمِ اللَّهِ الرَّحْمَنِ الرَّحِيمِ
"""
import subprocess, os, sys, pathlib, time

ROOT = pathlib.Path(r"c:\s_lang\s-programming-language")
BOOT = ROOT / "bee_os" / "boot"
KERNEL = ROOT / "bee_os" / "kernel"
BUILD = ROOT / "build" / "bin" / "Release"
LLVM = pathlib.Path(r"C:\llvm_dev\LLVM\bin")
QEMU = pathlib.Path(r"C:\qemu")

CLANG    = LLVM / "clang.exe"
LLC      = LLVM / "llc.exe"
LLD      = LLVM / "ld.lld.exe"
READELF  = LLVM / "llvm-readelf.exe"
QEMU_X64 = QEMU / "qemu-system-x86_64.exe"

SADc = BUILD / "sadc.exe"

OUT = ROOT / "build" / "qemu_boot"
OUT.mkdir(exist_ok=True)

# All kernel modules in link order (dependencies satisfied left→right)
KERNEL_MODULES = [
    "بداية_نحلة",
    "تسلسلي_نحلة",
    "مقاطعات_نحلة",
    "ذاكرة_نحلة",
    "كومة_نحلة",
    "جدولة_نحلة",
    "اتصال_نحلة",
    "قرص_نحلة",
    "قرص_cache_نحلة",
    "ملفات_نحلة",
    "ملفات_عمليات_نحلة",
    "صدفة_نحلة",
    "عمليات_نحلة",
    "واجهة_نحلة",
    "واجهة_متقدمة_نحلة",
    "ملفات_متقدمة_نحلة",
    "ملفات_أمان_نحلة",
    "ملفات_أنظمة_نحلة",
    "ملفات_ذاكرة_نحلة",
    "طاقة_نحلة",
    "طاقة_حرارة_نحلة",
    "طاقة_أجهزة_نحلة",
    "شبكة_نحلة",
    "شبكة_arp_نحلة",
    "شبكة_ip_نحلة",
    "شبكة_tcp_نحلة",
    "شبكة_dns_نحلة",
    "pci_نحلة",
    "usb_نحلة",
    "usb_uhci_نحلة",
    "usb_ehci_نحلة",
    "usb_hid_نحلة",
    "usb_تخزين_نحلة",
    "usb_موزع_نحلة",
    "usb_xhci_نحلة",
    "ps2_نحلة",
    "acpi_نحلة",
    "ata_نحلة",
    "أجهزة_مدير_نحلة",
    "usb_صوت_نحلة",
    "usb_شبكة_نحلة",
    "fat32_نحلة",
    "fat32_دليل_نحلة",
    "fat32_ملف_نحلة",
    "ext2_نحلة",
    "ext2_عقد_نحلة",
    "ext2_دليل_نحلة",
    "vfs_نحلة",
    "vfs_مسار_نحلة",
    "ذاكرة_كتل_نحلة",
    "أقسام_نحلة",
    "يوميات_نحلة",
    "tmpfs_نحلة",
    "procfs_نحلة",
    "ntfs_نحلة",
    "ntfs_دليل_نحلة",
    "sysfs_نحلة",
    "devfs_نحلة",
    "أنابيب_نحلة",
    "مقابس_نحلة",
    "ضغط_نحلة",
    "تشفير_نحلة",
    "raid_نحلة",
    "أذونات_نحلة",
    "حصص_نحلة",
    "exfat_نحلة",
    "قفل_ملفات_نحلة",
    "iso9660_نحلة",
    "ذاكرة_مؤقتة_نحلة",
    "جدول_io_نحلة",
    "تحميل_نحلة",
    "مراقب_نحلة",
    "روابط_نحلة",
    "fsck_نحلة",
    "fat32_كتابة_نحلة",
    "btrfs_نحلة",
    "squashfs_نحلة",
    "overlayfs_نحلة",
    "أوامر_ملفات_نحلة",
]

# C source files to compile
C_SOURCES = [
    ("beeos_runtime.c", "beeos_runtime.o"),
    ("vga_printf.c",    "vga_printf.o"),
    ("kernel_boot.c",   "kernel_boot.o"),
    ("fb_desktop.c",    "fb_desktop.o"),
    ("settings_ui.c",   "settings_ui.o"),
    ("window_manager.c","window_manager.o"),
]

CFLAGS = [
    "--target=i686-elf",
    "-march=i686",
    "-ffreestanding", "-nostdlib", "-nostdinc",
    "-fno-stack-protector", "-fno-pic",
    "-O2",
    "-I", str(BOOT),
]

def run(cmd, **kw):
    cmd = [str(x) for x in cmd]
    print("  $ " + " ".join(cmd[:6]) + (" ..." if len(cmd) > 6 else ""))
    r = subprocess.run(cmd, capture_output=True, **kw)
    out = (r.stdout + r.stderr).decode("utf-8", "replace")
    if r.returncode != 0:
        print(f"  [FAIL] exit={r.returncode}")
        print(out[-3000:])
        return None
    return out

def run_or_die(cmd, **kw):
    r = run(cmd, **kw)
    if r is None:
        sys.exit(1)
    return r

print("=" * 60)
print("  BeeOS Multi-Module Builder")
print(f"  {len(KERNEL_MODULES)} kernel modules + {len(C_SOURCES)} C files")
print("=" * 60)

# ── Step 1: Compile all ص modules to LLVM IR ─────────────────────
print(f"\n[1] Compiling {len(KERNEL_MODULES)} ص modules → LLVM IR ...")
kernel_lls = []
for mod in KERNEL_MODULES:
    src = KERNEL / f"{mod}.ص"
    ll  = OUT / f"{mod}.ll"
    if not src.exists():
        print(f"  [SKIP] {mod}.ص not found")
        continue
    r = run([SADc, "--freestanding", "--no-main", "--emit-llvm",
             str(src), "-o", str(ll)])
    if r is None:
        print(f"  [FAIL] {mod}.ص compilation failed!")
        sys.exit(1)
    kernel_lls.append((mod, ll))
    print(f"    ✓ {mod}.ll")

print(f"  → {len(kernel_lls)} modules compiled OK")

# ── Step 2: Patch target triple for ELF ───────────────────────────
print("\n[2] Patching target triple → i686-unknown-linux-musl (32-bit ELF) ...")
patched_lls = []
for mod, ll in kernel_lls:
    content = ll.read_text(encoding="utf-8", errors="replace")

    # Patch target triple
    content = content.replace(
        'target triple = "x86_64-pc-windows-msvc"',
        'target triple = "i686-unknown-linux-musl"'
    )
    # Fix datalayout for 32-bit ELF
    content = content.replace(
        'target datalayout = "e-m:w-',
        'target datalayout = "e-m:e-'
    )
    # Remove Windows-specific attributes
    content = content.replace('dllimport ', '')
    content = content.replace('dllexport ', '')

    # Rename __sad_main → __sad_init_N to avoid duplicate symbols
    # Each module gets its own init function
    idx = KERNEL_MODULES.index(mod)
    init_name = f"__sad_init_{idx}"
    # Only rename if the module doesn't already define its own init function
    if f'@{init_name}()' not in content:
        content = content.replace('@__sad_main()', f'@{init_name}()')
        content = content.replace('@__sad_main', f'@{init_name}')
    else:
        # Module already has explicit init — just remove __sad_main if present
        # Replace __sad_main with a unique unused name to avoid conflicts
        fallback_name = f"__sad_module_entry_{idx}"
        content = content.replace('@__sad_main()', f'@{fallback_name}()')
        content = content.replace('@__sad_main', f'@{fallback_name}')
    # Rename main() too
    main_name = f"__sad_module_main_{idx}"
    content = content.replace('define i32 @main()', f'define i32 @{main_name}()')
    content = content.replace('call void @main()', f'call void @{main_name}()')

    # For the boot module (بداية_نحلة), add a C-callable alias for بداية_النواة
    if mod == "بداية_نحلة":
        # The function name in LLVM IR is UTF-8 escaped
        # بداية_النواة → \D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D8\A9
        ar_name = r'"\D8\A8\D8\AF\D8\A7\D9\8A\D8\A9_\D8\A7\D9\84\D9\86\D9\88\D8\A7\D8\A9"'
        # Add alias at end of file
        alias_line = f'\n@beeos_kernel_entry = alias void (), void ()* @{ar_name}\n'
        content += alias_line

    patched = OUT / f"{mod}_elf.ll"
    patched.write_text(content, encoding="utf-8")
    patched_lls.append((mod, patched))

print(f"    ✓ {len(patched_lls)} modules patched")

# ── Step 2.5: Compile built-in stubs (builtins.ll) ───────────────
print("\n[2.5] Compiling language builtins → .o ...")
builtins_ll = BOOT / "builtins.ll"
builtins_obj = OUT / "builtins.o"
if builtins_ll.exists():
    r = run([LLC, "--march=x86", "--relocation-model=static",
             "--filetype=obj", str(builtins_ll), "-o", str(builtins_obj)])
    if r is None:
        print("  [FAIL] builtins.ll compilation failed!")
        sys.exit(1)
    print("    ✓ builtins.o")
else:
    builtins_obj = None
    print("    [SKIP] builtins.ll not found")

# ── Step 3: LLVM IR → object files ───────────────────────────────
print("\n[3] llc: LLVM IR → .o ...")
kernel_objs = []
for mod, ll in patched_lls:
    obj = OUT / f"{mod}.o"
    r = run([LLC, "--march=x86", "--relocation-model=static",
             "--filetype=obj", str(ll), "-o", str(obj)])
    if r is None:
        print(f"  [FAIL] llc failed for {mod}")
        sys.exit(1)
    kernel_objs.append(obj)
    print(f"    ✓ {mod}.o")

# ── Step 4: Compile boot.S ────────────────────────────────────────
print("\n[4] clang: boot.S → boot.o ...")
boot_obj = OUT / "boot.o"
run_or_die([CLANG, "--target=i686-elf", "-march=i686",
            "-ffreestanding", "-nostdlib",
            "-fno-stack-protector", "-fno-pic",
            "-c", str(BOOT / "boot.S"), "-o", str(boot_obj)])
print("    ✓ boot.o")

# ── Step 5: Compile C files ──────────────────────────────────────
c_objs = []
for src_name, obj_name in C_SOURCES:
    src = BOOT / src_name
    if not src.exists():
        print(f"  [SKIP] {src_name} not found")
        continue
    print(f"\n[5] clang: {src_name} → {obj_name} ...")
    obj = OUT / obj_name
    r = run([CLANG] + CFLAGS + ["-c", str(src), "-o", str(obj)])
    if r is None:
        print(f"  [FAIL] {src_name}")
        sys.exit(1)
    c_objs.append(obj)
    print(f"    ✓ {obj_name}")

# ── Step 6: Link everything ──────────────────────────────────────
print(f"\n[6] ld.lld: linking {len(kernel_objs)} kernel + {len(c_objs)} C → beeos.elf ...")
beeos_elf = OUT / "beeos.elf"
link_cmd = [
    LLD,
    "-T", str(BOOT / "kernel.ld"),
    "--oformat=elf",
    "--allow-multiple-definition",
    str(boot_obj),
] + [str(o) for o in c_objs] + ([str(builtins_obj)] if builtins_obj else []) + [str(o) for o in kernel_objs] + [
    "-o", str(beeos_elf),
]
r = run(link_cmd)
if r is None:
    print("  [FATAL] Linking failed.")
    sys.exit(1)
print("    ✓ beeos.elf")

# ── Step 7: Verify ELF ───────────────────────────────────────────
print("\n[7] Verifying ELF ...")
if READELF.exists():
    r = subprocess.run([str(READELF), "--file-header", str(beeos_elf)],
                       capture_output=True)
    info = (r.stdout + r.stderr).decode("utf-8", "replace")
    for line in info.splitlines():
        if any(k in line for k in ["Class", "Type", "Machine", "Entry"]):
            print("   ", line.strip())

file_size = beeos_elf.stat().st_size
print(f"    Size: {file_size:,} bytes  ({file_size//1024} KB)")

# ── Step 8: Run QEMU ─────────────────────────────────────────────
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
        "-cpu", "qemu32",
        "-m", "64M",
        "-kernel", str(beeos_elf),
        "-serial", f"file:{serial_log}",
        "-no-reboot",
        "-display", "none",
    ], timeout=10, capture_output=True)
    print(f"QEMU exit code: {r.returncode}")
    qout = (r.stdout + r.stderr).decode("utf-8", "replace")
    if qout.strip():
        print("QEMU output:", qout[:400])
except subprocess.TimeoutExpired:
    print("[QEMU] Timed out (normal for HLT loop)")
except KeyboardInterrupt:
    print("\n[!] Stopped")

# Show serial output
time.sleep(1)
if serial_log.exists() and serial_log.stat().st_size > 0:
    print("\n=== Serial Output from BeeOS Kernel ===")
    log = serial_log.read_text(encoding="utf-8", errors="replace")
    print(log[:4000])
    if len(log) > 4000:
        print(f"\n... ({len(log)} bytes total)")
else:
    print("\n[!] No serial output captured.")
    # Try again with VGA window
    print("    Re-launching with VGA window ...")
    try:
        proc = subprocess.Popen([
            str(QEMU_X64),
            "-machine", "q35,kernel-irqchip=off",
            "-cpu", "qemu32",
            "-m", "64M",
            "-kernel", str(beeos_elf),
            "-serial", f"file:{serial_log}",
            "-no-reboot",
        ])
        time.sleep(8)
        proc.terminate()
        if serial_log.exists() and serial_log.stat().st_size > 0:
            print("\n=== Serial Output ===")
            print(serial_log.read_text(encoding="utf-8", errors="replace")[:4000])
    except Exception as e:
        print(f"  QEMU re-launch error: {e}")

print("\n=== Build Complete ===")
