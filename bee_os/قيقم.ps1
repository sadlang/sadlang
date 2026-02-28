# ============================================================================
# قيقم.ps1 — تشغيل QEMU على Windows لنظام النحلة BeeOS
# BeeOS QEMU Launcher — Windows PowerShell
#
# الاستخدام:
#   .\قيقم.ps1              ← تشغيل عادي
#   .\قيقم.ps1 -Debug       ← مع GDB stub (المنفذ 1234)
#   .\قيقم.ps1 -Build       ← بناء ثم تشغيل
#
# بسم الله الرحمن الرحيم
# ============================================================================

param(
    [switch]$Debug,
    [switch]$Build,
    [switch]$NoGfx,
    [switch]$Test
)

# ============================================================================
# إعدادات
# ============================================================================

$KERNEL       = "bee_os\kernel\build\BeeOS.elf"
$QEMU         = "qemu-system-x86_64"
$RAM          = "128M"
$CORES        = 2
$NASM         = "nasm"

# مسارات مثبّتة شائعة على Windows
$QEMU_PATHS = @(
    "C:\Program Files\qemu\qemu-system-x86_64.exe",
    "C:\qemu\qemu-system-x86_64.exe",
    "qemu-system-x86_64.exe"
)

# ============================================================================
# دوال
# ============================================================================

function Bee-Print { param($msg); Write-Host "[🐝 BeeOS] $msg" -ForegroundColor Green }
function Bee-Error { param($msg); Write-Host "[✗ خطأ] $msg" -ForegroundColor Red; exit 1 }

# ============================================================================
# شعار
# ============================================================================

Write-Host ""
Write-Host "╔══════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║   بِسْمِ اللهِ الرَّحْمٰنِ الرَّحِيمِ     ║" -ForegroundColor Cyan
Write-Host "║   🐝  BeeOS QEMU Launcher v0.1.0          ║" -ForegroundColor Cyan
Write-Host "╚══════════════════════════════════════════╝" -ForegroundColor Cyan
Write-Host ""

# ============================================================================
# البحث عن QEMU
# ============================================================================

$qemu_exe = $null
foreach ($path in $QEMU_PATHS) {
    if (Test-Path $path) {
        $qemu_exe = $path
        break
    }
}

if ($null -eq $qemu_exe) {
    $found = Get-Command qemu-system-x86_64 -ErrorAction SilentlyContinue
    if ($found) {
        $qemu_exe = $found.Source
    }
}

if ($null -eq $qemu_exe) {
    Bee-Error "QEMU غير مثبت!`nحمّله من: https://www.qemu.org/download/#windows"
}

Bee-Print "QEMU: $qemu_exe"

# ============================================================================
# بناء النواة (اختياري)
# ============================================================================

if ($Build) {
    Bee-Print "بناء النواة..."

    # التحقق من NASM
    $nasm = Get-Command nasm -ErrorAction SilentlyContinue
    if (-not $nasm) {
        Bee-Error "NASM غير مثبت! حمّله من: https://www.nasm.us/"
    }

    # تجميع entry.asm
    $entry_src = "bee_os\kernel\تراب\entry.asm"
    $entry_obj = "bee_os\kernel\build\entry.o"
    New-Item -ItemType Directory -Path "bee_os\kernel\build" -Force | Out-Null

    Bee-Print "تجميع entry.asm..."
    & nasm -f elf64 $entry_src -o $entry_obj
    if ($LASTEXITCODE -ne 0) { Bee-Error "فشل تجميع entry.asm" }

    # الربط
    Bee-Print "الربط..."
    & ld -T "bee_os\kernel\تراب\رابط.ld" -m elf_x86_64 $entry_obj -o $KERNEL
    if ($LASTEXITCODE -ne 0) { Bee-Error "فشل الربط" }

    Bee-Print "✓ NواةOS.elf مبنية بنجاح"
}

# ============================================================================
# التحقق من وجود صورة النواة
# ============================================================================

if (-not (Test-Path $KERNEL)) {
    Write-Host ""
    Write-Host "⚠  ملف النواة غير موجود: $KERNEL" -ForegroundColor Yellow
    Write-Host "   شغّل: .\قيقم.ps1 -Build  لبناء النواة أولاً" -ForegroundColor Yellow
    Write-Host ""

    # تشغيل تجريبي محاكاة
    Bee-Print "تشغيل محاكاة النواة بالمفسر..."
    & "build\bin\Debug\sad.exe" "bee_os\kernel\نواة.ص"
    exit 0
}

# ============================================================================
# تجميع معاملات QEMU
# ============================================================================

$qemu_args = @(
    "-kernel", $KERNEL,
    "-m", $RAM,
    "-smp", $CORES,
    "-no-reboot",
    "-no-shutdown",
    "-vga", "std",
    "-serial", "stdio",
    "-netdev", "user,id=net0",
    "-device", "e1000,netdev=net0"
)

if ($Debug) {
    Bee-Print "وضع التصحيح — GDB على :1234"
    Bee-Print "شغّل في terminal آخر: gdb -ex 'target remote :1234' $KERNEL"
    $qemu_args += @("-s", "-S")
}

if ($NoGfx) {
    $qemu_args += @("-display", "none", "-nographic")
}

# ============================================================================
# تشغيل QEMU
# ============================================================================

Bee-Print "تشغيل BeeOS على QEMU..."
Write-Host ""

& $qemu_exe @qemu_args

Write-Host ""
Bee-Print "النظام أُغلق — الله يحفظك 🐝"
