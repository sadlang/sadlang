#!/usr/bin/env pwsh
# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════════════
# سكريبت تثبيت لغة ص — Windows
# Sad Programming Language Installer — Windows
# ═══════════════════════════════════════════════════════════════════════════════
#
# الاستخدام / Usage:
#   irm https://sad-lang.org/install.ps1 | iex
#
# خيارات متقدمة / Advanced:
#   .\install.ps1                          # تثبيت تفاعلي (يسألك ماذا تريد)
#   .\install.ps1 -Components full         # المفسر + المترجم + كل الأدوات
#   .\install.ps1 -Components compiler     # المترجم فقط (يتطلب LLVM)
#   .\install.ps1 -Components interpreter  # المفسر فقط
#   .\install.ps1 -Version 1.2.0           # إصدار محدد
#   .\install.ps1 -InstallDir "D:\sad"     # مجلد مخصص
#   .\install.ps1 -NoPath                  # لا يضيف لمتغير PATH
#   .\install.ps1 -Uninstall               # إزالة التثبيت
#
# ═══════════════════════════════════════════════════════════════════════════════
#
# مصدر التحميل / Download Source:
#   يتم تحميل الملفات من GitHub Releases:
#   https://github.com/SalehKadah/s-programming-language/releases
#
#   ملفات التحميل المتاحة لكل إصدار:
#   ┌─────────────────────────────────────────────────────────────────┐
#   │ sad-v{VER}-windows-x86_64.zip        المفسر + الأدوات الأساسية│
#   │ sad-full-v{VER}-windows-x86_64.zip   المفسر + المترجم + كل شيء│
#   │ sadc-v{VER}-windows-x86_64.zip       المترجم فقط (LLVM)       │
#   │                                                                │
#   │ نفس الملفات متاحة لـ:                                         │
#   │   linux-x86_64, linux-aarch64                                  │
#   │   macos-x86_64, macos-aarch64                                  │
#   └─────────────────────────────────────────────────────────────────┘
#
# ═══════════════════════════════════════════════════════════════════════════════

[CmdletBinding()]
param(
    [ValidateSet("interpreter", "compiler", "full")]
    [string]$Components,

    [string]$Version = "latest",
    [string]$InstallDir = "$env:LOCALAPPDATA\sad-lang",
    [switch]$NoPath,
    [switch]$Uninstall,
    [switch]$Force
)

# ──────────────────────────────────────────────────────────────────────
# الإعدادات / Configuration
# ──────────────────────────────────────────────────────────────────────
$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"

$REPO_OWNER = "SalehKadah"
$REPO_NAME  = "s-programming-language"
$GITHUB_API = "https://api.github.com/repos/$REPO_OWNER/$REPO_NAME"

# ──────────────────────────────────────────────────────────────────────
# الألوان والرموز / Colors & Symbols
# ──────────────────────────────────────────────────────────────────────
function Write-Logo {
    $logo = @"

    ╔═══════════════════════════════════════════════╗
    ║                                               ║
    ║         لغة ص — Sad Programming Language      ║
    ║              مُثبّت الإصدار v1.0              ║
    ║                                               ║
    ╚═══════════════════════════════════════════════╝

"@
    Write-Host $logo -ForegroundColor Cyan
}

function Write-Step  { param($msg) Write-Host "  [●] " -NoNewline -ForegroundColor Blue; Write-Host $msg }
function Write-OK    { param($msg) Write-Host "  [✓] " -NoNewline -ForegroundColor Green; Write-Host $msg }
function Write-Warn  { param($msg) Write-Host "  [⚠] " -NoNewline -ForegroundColor Yellow; Write-Host $msg }
function Write-Err   { param($msg) Write-Host "  [✗] " -NoNewline -ForegroundColor Red; Write-Host $msg }
function Write-Info  { param($msg) Write-Host "  [→] " -NoNewline -ForegroundColor DarkGray; Write-Host $msg -ForegroundColor DarkGray }

# ──────────────────────────────────────────────────────────────────────
# عرض قائمة الاختيار التفاعلية / Interactive Component Selection
# ──────────────────────────────────────────────────────────────────────
function Show-ComponentMenu {
    if ($PSBoundParameters.ContainsKey('Components')) { return }

    Write-Host "  ╔════════════════════════════════════════════════════════════════╗" -ForegroundColor White
    Write-Host "  ║  اختر ما تريد تثبيته / Choose what to install:              ║" -ForegroundColor White
    Write-Host "  ╠════════════════════════════════════════════════════════════════╣" -ForegroundColor White
    Write-Host "  ║                                                              ║" -ForegroundColor White
    Write-Host "  ║  [1] المفسر فقط (interpreter)" -NoNewline -ForegroundColor White
    Write-Host "                                ║" -ForegroundColor White
    Write-Host "  ║      sad.exe + المكتبة القياسية + الأدوات" -NoNewline -ForegroundColor DarkGray
    Write-Host "                  ║" -ForegroundColor White
    Write-Host "  ║      ← الأفضل لمعظم المستخدمين" -NoNewline -ForegroundColor Green
    Write-Host "                             ║" -ForegroundColor White
    Write-Host "  ║                                                              ║" -ForegroundColor White
    Write-Host "  ║  [2] المترجم فقط (compiler)" -NoNewline -ForegroundColor White
    Write-Host "                                 ║" -ForegroundColor White
    Write-Host "  ║      sadc.exe — يحوّل .ص إلى ملف تنفيذي أصلي" -NoNewline -ForegroundColor DarkGray
    Write-Host "             ║" -ForegroundColor White
    Write-Host "  ║      ⚠ يتطلب LLVM 18 مثبتاً" -NoNewline -ForegroundColor Yellow
    Write-Host "                               ║" -ForegroundColor White
    Write-Host "  ║                                                              ║" -ForegroundColor White
    Write-Host "  ║  [3] الحزمة الكاملة (full)" -NoNewline -ForegroundColor White
    Write-Host "                                  ║" -ForegroundColor White
    Write-Host "  ║      المفسر + المترجم + LSP + REPL + مدير الحزم" -NoNewline -ForegroundColor DarkGray
    Write-Host "            ║" -ForegroundColor White
    Write-Host "  ║      ← كل شيء في حزمة واحدة" -NoNewline -ForegroundColor Cyan
    Write-Host "                              ║" -ForegroundColor White
    Write-Host "  ║                                                              ║" -ForegroundColor White
    Write-Host "  ╚════════════════════════════════════════════════════════════════╝" -ForegroundColor White
    Write-Host ""

    $choice = Read-Host "  اختر رقم (1/2/3) [الافتراضي: 1]"
    switch ($choice) {
        "2" { $script:Components = "compiler" }
        "3" { $script:Components = "full" }
        default { $script:Components = "interpreter" }
    }

    $componentName = switch ($script:Components) {
        "interpreter" { "المفسر (sad.exe)" }
        "compiler"    { "المترجم (sadc.exe)" }
        "full"        { "الحزمة الكاملة (sad + sadc + أدوات)" }
    }
    Write-OK "تم اختيار: $componentName"
    Write-Host ""
}

# ──────────────────────────────────────────────────────────────────────
# التحقق من المتطلبات / Check Requirements
# ──────────────────────────────────────────────────────────────────────
function Test-Requirements {
    if (-not $IsWindows -and $PSVersionTable.PSVersion.Major -lt 6) {
        if ($env:OS -ne "Windows_NT") {
            Write-Err "هذا السكريبت مخصص لنظام Windows"
            Write-Info "لنظام Linux/macOS استخدم: curl -fsSL https://sad-lang.org/install.sh | sh"
            exit 1
        }
    }

    Write-Step "التحقق من الاتصال بالإنترنت..."
    try {
        $null = Invoke-RestMethod -Uri "https://api.github.com" -TimeoutSec 10
        Write-OK "الاتصال بالإنترنت متاح"
    }
    catch {
        Write-Err "لا يمكن الاتصال بالإنترنت"
        exit 1
    }

    # تحذير LLVM إذا اختار المترجم
    if ($Components -in @("compiler", "full")) {
        $script:LLVMPath = Find-LLVM
        if (-not $script:LLVMPath) {
            Write-Warn "LLVM 18 غير مثبت أو غير مكتشف — sadc يحتاج LLVM 18"
            Write-Host ""
            Write-Host "  ╔══════════════════════════════════════════════════════════╗" -ForegroundColor Yellow
            Write-Host "  ║           خيارات LLVM / LLVM Options                   ║" -ForegroundColor Yellow
            Write-Host "  ╠══════════════════════════════════════════════════════════╣" -ForegroundColor Yellow
            Write-Host "  ║  [1] تحميل وتثبيت LLVM 18 تلقائياً (موصى به)         ║" -ForegroundColor White
            Write-Host "  ║  [2] تحديد مسار LLVM المثبت يدوياً                    ║" -ForegroundColor White
            Write-Host "  ║  [3] تخطي (تثبيت sad بدون sadc — يمكن إضافة LLVM لاحقاً) ║" -ForegroundColor DarkGray
            Write-Host "  ╚══════════════════════════════════════════════════════════╝" -ForegroundColor Yellow
            Write-Host ""
            $llvmChoice = Read-Host "  اختر (1/2/3) [الافتراضي: 1]"
            switch ($llvmChoice) {
                "2" {
                    $manualPath = Read-Host "  أدخل مسار مجلد LLVM (مثال: C:\Program Files\LLVM)"
                    $manualPath = $manualPath.Trim().Trim('"')
                    $validated = Test-LLVMPath $manualPath
                    if ($validated) {
                        $script:LLVMPath = $validated.Path
                        Write-OK "LLVM مقبول: $($validated.Path) (الإصدار: $($validated.Version))"
                        Set-LLVMEnvironment $validated.Path
                    } else {
                        Write-Warn "المسار لا يحتوي على مكتبات LLVM صالحة — سيتم التثبيت بدون sadc"
                        if ($Components -eq "compiler") { $script:Components = "interpreter" }
                    }
                }
                "3" {
                    Write-Info "سيتم التثبيت بدون sadc — أضف LLVM لاحقاً وشغّل المثبت مجدداً"
                    if ($Components -eq "compiler") { $script:Components = "interpreter" }
                }
                default {
                    Install-LLVM
                }
            }
        }
    }
}

# ──────────────────────────────────────────────────────────────────────
# البحث الشامل عن LLVM / Find LLVM Everywhere
# ──────────────────────────────────────────────────────────────────────
function Find-LLVM {
    Write-Step "البحث عن LLVM على الجهاز..."

    # 1) البحث في PATH أولاً
    $llvmConfig = Get-Command "llvm-config.exe" -ErrorAction SilentlyContinue
    if ($llvmConfig) {
        $binDir = Split-Path $llvmConfig.Source
        $rootDir = Split-Path $binDir
        $result = Test-LLVMPath $rootDir
        if ($result) {
            Write-OK "LLVM موجود في PATH: $rootDir (الإصدار: $($result.Version))"
            Set-LLVMEnvironment $rootDir
            return $rootDir
        }
    }

    # 2) البحث في السجل (Registry)
    $regPaths = @(
        "HKLM:\SOFTWARE\LLVM\LLVM",
        "HKLM:\SOFTWARE\WOW6432Node\LLVM\LLVM",
        "HKCU:\SOFTWARE\LLVM\LLVM"
    )
    foreach ($regKey in $regPaths) {
        try {
            $val = Get-ItemPropertyValue -Path $regKey -Name "(default)" -ErrorAction Stop
            if ($val -and (Test-Path $val)) {
                $result = Test-LLVMPath $val
                if ($result) {
                    Write-OK "LLVM موجود في السجل: $val (الإصدار: $($result.Version))"
                    Set-LLVMEnvironment $val
                    return $val
                }
            }
        } catch {}
    }

    # 3) البحث في مسارات شائعة متعددة
    $commonPaths = @(
        "C:\Program Files\LLVM",
        "C:\Program Files\LLVM-18",
        "C:\LLVM",
        "C:\LLVM-18",
        "C:\LLVM-Dev",
        "C:\tools\LLVM",
        "C:\msys64\mingw64",
        "C:\msys64\ucrt64",
        "C:\msys64\clang64",
        "D:\Program Files\LLVM",
        "D:\LLVM",
        "$env:USERPROFILE\scoop\apps\llvm\current",
        "$env:LOCALAPPDATA\Programs\LLVM",
        "$env:ChocolateyInstall\lib\llvm\tools\llvm"
    )
    foreach ($p in $commonPaths) {
        if (Test-Path $p) {
            $result = Test-LLVMPath $p
            if ($result) {
                Write-OK "LLVM موجود: $p (الإصدار: $($result.Version))"
                Set-LLVMEnvironment $p
                return $p
            }
        }
    }

    # 4) البحث عبر Scoop / Chocolatey / WinGet
    foreach ($tool in @("scoop", "choco")) {
        $cmd = Get-Command $tool -ErrorAction SilentlyContinue
        if ($cmd) {
            Write-Info "$tool مثبت — محاولة الكشف عن LLVM..."
        }
    }

    Write-Warn "لم يُعثر على LLVM تلقائياً"
    return $null
}

# ──────────────────────────────────────────────────────────────────────
# التحقق من صحة مجلد LLVM / Validate LLVM Path
# ──────────────────────────────────────────────────────────────────────
function Test-LLVMPath {
    param([string]$Path)
    if (-not (Test-Path $Path)) { return $null }

    # ابحث عن llvm-config في bin أو مباشرة
    $candidates = @(
        (Join-Path $Path "bin\llvm-config.exe"),
        (Join-Path $Path "llvm-config.exe")
    )

    $llvmConfigExe = $null
    foreach ($c in $candidates) {
        if (Test-Path $c) { $llvmConfigExe = $c; break }
    }

    # تحقق من ملفات أساسية حتى بدون llvm-config
    $requiredBins = @("clang.exe", "llc.exe", "opt.exe", "lld-link.exe", "llvm-ar.exe")
    $binDir = Join-Path $Path "bin"
    $foundBins = @()
    foreach ($bin in $requiredBins) {
        $binPath = Join-Path $binDir $bin
        if (Test-Path $binPath) { $foundBins += $bin }
    }

    if ($foundBins.Count -lt 2 -and -not $llvmConfigExe) {
        return $null
    }

    # استخراج الإصدار
    $version = "unknown"
    if ($llvmConfigExe) {
        try {
            $version = (& $llvmConfigExe --version 2>$null).Trim()
        } catch {}
    } else {
        # محاولة clang
        $clangExe = Join-Path $binDir "clang.exe"
        if (Test-Path $clangExe) {
            try {
                $clangVer = (& $clangExe --version 2>$null | Select-Object -First 1)
                if ($clangVer -match "(\d+\.\d+\.\d+)") { $version = $matches[1] }
            } catch {}
        }
    }

    # تحقق أن الإصدار 14+ (يدعم sadc)
    $majorVersion = 0
    if ($version -match "^(\d+)") { $majorVersion = [int]$matches[1] }
    if ($majorVersion -lt 14 -and $majorVersion -ne 0) {
        Write-Warn "LLVM الإصدار $version قديم — sadc يحتاج LLVM 14+ (مُفضّل 18)"
        return $null
    }

    # التحقق من وجود ملفات المكتبات
    $libDir = Join-Path $Path "lib"
    $hasLibs = $false
    if (Test-Path $libDir) {
        $hasLibs = (Get-ChildItem $libDir -Filter "LLVM*.lib" -ErrorAction SilentlyContinue | Measure-Object).Count -gt 0
        if (-not $hasLibs) {
            $hasLibs = (Get-ChildItem $libDir -Filter "libLLVM*.a" -ErrorAction SilentlyContinue | Measure-Object).Count -gt 0
        }
    }

    $includeDir = Join-Path $Path "include\llvm"
    $hasHeaders = Test-Path $includeDir

    Write-Info "  [✓] ملفات تنفيذية: $($foundBins -join ', ')"
    if ($hasLibs)   { Write-Info "  [✓] مكتبات .lib/.a موجودة" }
    if ($hasHeaders){ Write-Info "  [✓] ملفات الترويسة include/llvm موجودة" }

    return @{
        Path        = $Path
        Version     = $version
        MajorVersion = $majorVersion
        HasLibs     = $hasLibs
        HasHeaders  = $hasHeaders
        BinDir      = $binDir
        Bins        = $foundBins
    }
}

# ──────────────────────────────────────────────────────────────────────
# حفظ متغيرات البيئة لـ LLVM / Save LLVM Environment
# ──────────────────────────────────────────────────────────────────────
function Set-LLVMEnvironment {
    param([string]$LLVMRoot)
    $binDir = Join-Path $LLVMRoot "bin"
    # حفظ LLVM_DIR
    [Environment]::SetEnvironmentVariable("LLVM_DIR", $LLVMRoot, "User")
    $env:LLVM_DIR = $LLVMRoot
    # إضافة bin لـ PATH إذا لم يكن موجوداً
    $currentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
    if ($currentPath -notlike "*$binDir*") {
        [Environment]::SetEnvironmentVariable("PATH", "$binDir;$currentPath", "User")
        $env:PATH = "$binDir;$env:PATH"
    }
    Write-OK "تم حفظ LLVM_DIR=$LLVMRoot في متغيرات البيئة"
}

# ──────────────────────────────────────────────────────────────────────
# تحميل وتثبيت LLVM تلقائياً / Auto-Download & Install LLVM
# ──────────────────────────────────────────────────────────────────────
function Install-LLVM {
    Write-Host ""
    Write-Step "تحميل LLVM 18 تلقائياً..."
    Write-Info "حجم الملف: ~500 MB — قد يستغرق بضع دقائق"
    Write-Host ""

    # اكتشاف مدير الحزم
    $packageManager = $null
    if (Get-Command "winget" -ErrorAction SilentlyContinue) {
        $packageManager = "winget"
    } elseif (Get-Command "choco" -ErrorAction SilentlyContinue) {
        $packageManager = "choco"
    } elseif (Get-Command "scoop" -ErrorAction SilentlyContinue) {
        $packageManager = "scoop"
    }

    if ($packageManager) {
        Write-Step "تم اكتشاف مدير الحزم: $packageManager"
        $proceed = Read-Host "  هل تريد تثبيت LLVM عبر $packageManager ؟ (ن/ل) [ن=نعم]"
        if ($proceed -notin @("ل", "لا", "n", "no")) {
            switch ($packageManager) {
                "winget" {
                    Write-Step "تشغيل: winget install LLVM.LLVM --version 18.1.8..."
                    try {
                        & winget install LLVM.LLVM --version 18.1.8 --silent --accept-package-agreements --accept-source-agreements
                        Write-OK "اكتمل تثبيت LLVM عبر winget"
                        $script:LLVMPath = Find-LLVM
                        return
                    } catch {
                        Write-Warn "فشل winget — سيتم التحميل المباشر"
                    }
                }
                "choco" {
                    Write-Step "تشغيل: choco install llvm --version 18.1.8..."
                    try {
                        & choco install llvm --version 18.1.8 -y
                        Write-OK "اكتمل تثبيت LLVM عبر Chocolatey"
                        $script:LLVMPath = Find-LLVM
                        return
                    } catch {
                        Write-Warn "فشل Chocolatey — سيتم التحميل المباشر"
                    }
                }
                "scoop" {
                    Write-Step "تشغيل: scoop install llvm..."
                    try {
                        & scoop install llvm
                        Write-OK "اكتمل تثبيت LLVM عبر Scoop"
                        $script:LLVMPath = Find-LLVM
                        return
                    } catch {
                        Write-Warn "فشل Scoop — سيتم التحميل المباشر"
                    }
                }
            }
        }
    }

    # تحميل مباشر من GitHub Releases
    Write-Step "تحميل LLVM 18.1.8 مباشرة من GitHub..."
    $llvmArch = if ([Environment]::Is64BitOperatingSystem) { "x64" } else { "x86" }
    $llvmUrl  = "https://github.com/llvm/llvm-project/releases/download/llvmorg-18.1.8/LLVM-18.1.8-win$($llvmArch).exe"
    $llvmInstaller = Join-Path $env:TEMP "LLVM-18.1.8-installer.exe"
    $llvmInstallDir = "C:\Program Files\LLVM"

    Write-Info "الرابط: $llvmUrl"
    Write-Info "حجم تقريبي: ~500 MB — الرجاء الانتظار..."
    Write-Host ""

    try {
        # عرض شريط تقدم
        $webClient = New-Object System.Net.WebClient
        $progress = 0
        $webClient.DownloadProgressChanged += {
            param($sender, $e)
            $pct = $e.ProgressPercentage
            if ($pct -ne $progress) {
                $progress = $pct
                $bar = "#" * ([math]::Floor($pct / 5))
                $empty = "-" * (20 - [math]::Floor($pct / 5))
                Write-Host "`r  [${bar}${empty}] ${pct}%" -NoNewline -ForegroundColor Cyan
            }
        }
        $webClient.DownloadFileTaskAsync($llvmUrl, $llvmInstaller).Wait()
        Write-Host ""
        Write-OK "تم تحميل LLVM ($([math]::Round((Get-Item $llvmInstaller).Length / 1MB))  MB)"

        Write-Step "تشغيل مثبت LLVM (صامت)..."
        $proc = Start-Process -FilePath $llvmInstaller -ArgumentList "/S /D=$llvmInstallDir" -Wait -PassThru
        if ($proc.ExitCode -eq 0 -or $proc.ExitCode -eq 3010) {
            Write-OK "تم تثبيت LLVM في: $llvmInstallDir"
            $script:LLVMPath = $llvmInstallDir
            Set-LLVMEnvironment $llvmInstallDir

            # التحقق النهائي
            $validated = Test-LLVMPath $llvmInstallDir
            if ($validated) {
                Write-OK "التحقق: LLVM $($validated.Version) جاهز للاستخدام مع sadc"
            }
        } else {
            Write-Warn "انتهى مثبت LLVM بكود خروج: $($proc.ExitCode)"
            Write-Info "يمكنك تثبيت LLVM يدوياً من: https://github.com/llvm/llvm-project/releases"
            if ($Components -eq "compiler") { $script:Components = "interpreter" }
        }
    } catch {
        Write-Warn "فشل تحميل LLVM: $_"
        Write-Info "حمّل LLVM يدوياً: https://github.com/llvm/llvm-project/releases/tag/llvmorg-18.1.8"
        if ($Components -eq "compiler") { $script:Components = "interpreter" }
    } finally {
        Remove-Item $llvmInstaller -ErrorAction SilentlyContinue
    }
}

# ──────────────────────────────────────────────────────────────────────
# تحديد بنية المعالج / Detect Architecture
# ──────────────────────────────────────────────────────────────────────
function Get-Architecture {
    try {
        $arch = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture
        switch ($arch) {
            "X64"   { return "x86_64" }
            "Arm64" { return "aarch64" }
            "X86"   { return "i686" }
        }
    } catch {}
    if ([Environment]::Is64BitOperatingSystem) { return "x86_64" } else { return "i686" }
}

# ──────────────────────────────────────────────────────────────────────
# جلب الإصدار / Get Release Info
# ──────────────────────────────────────────────────────────────────────
function Get-ReleaseInfo {
    if ($Version -eq "latest") {
        Write-Step "البحث عن أحدث إصدار من GitHub Releases..."
        $url = "$GITHUB_API/releases/latest"
    } else {
        Write-Step "البحث عن الإصدار v$Version..."
        $url = "$GITHUB_API/releases/tags/v$Version"
    }

    try {
        $release = Invoke-RestMethod -Uri $url -TimeoutSec 15
        $ver = $release.tag_name -replace '^v', ''
        Write-OK "الإصدار: v$ver"
        return @{ Version = $ver; Assets = $release.assets }
    }
    catch {
        Write-Err "الإصدار غير موجود"
        Write-Info "تحقق من: https://github.com/$REPO_OWNER/$REPO_NAME/releases"
        exit 1
    }
}

# ──────────────────────────────────────────────────────────────────────
# تحميل وتثبيت / Download & Install
# ──────────────────────────────────────────────────────────────────────
function Install-Sad {
    param($ReleaseInfo)

    $arch = Get-Architecture
    $version = $ReleaseInfo.Version

    # بادئة اسم الملف حسب المكون
    $prefix = switch ($Components) {
        "interpreter" { "sad" }
        "compiler"    { "sadc" }
        "full"        { "sad-full" }
    }

    # أسماء محتملة
    $possibleNames = @(
        "${prefix}-v${version}-windows-${arch}.zip",
        "${prefix}-v${version}-win-${arch}.zip",
        "${prefix}-${version}-windows-${arch}.zip",
        "${prefix}-windows-${arch}.zip"
    )

    Write-Step "البحث عن ملف التحميل ($Components → $prefix)..."

    $asset = $null
    foreach ($name in $possibleNames) {
        $asset = $ReleaseInfo.Assets | Where-Object { $_.name -eq $name }
        if ($asset) { break }
    }

    if (-not $asset) {
        Write-Err "لم يُعثر على ملف تحميل لـ '$Components' على منصتك ($arch)"
        Write-Info "الملفات المتاحة:"
        foreach ($a in $ReleaseInfo.Assets) { Write-Info "  - $($a.name)" }
        Write-Info ""
        Write-Info "ابنِ من المصدر: https://github.com/$REPO_OWNER/$REPO_NAME#البناء-والتشغيل"
        exit 1
    }

    $downloadUrl = $asset.browser_download_url
    $tempDir = Join-Path $env:TEMP "sad-install-$(Get-Random)"

    try {
        New-Item -ItemType Directory -Path $tempDir -Force | Out-Null
        $tempZip = Join-Path $tempDir $asset.name

        Write-Step "تحميل $($asset.name)..."
        Write-Info "المصدر: GitHub Releases (github.com/$REPO_OWNER/$REPO_NAME)"
        Write-Info "الرابط: $downloadUrl"
        Invoke-WebRequest -Uri $downloadUrl -OutFile $tempZip -UseBasicParsing

        $fileSize = [math]::Round((Get-Item $tempZip).Length / 1MB, 1)
        Write-OK "تم التحميل ($fileSize MB)"

        Write-Step "فك الضغط والتثبيت..."
        if (Test-Path $InstallDir) {
            if (-not $Force) {
                Write-Warn "مجلد التثبيت موجود: $InstallDir"
                $answer = Read-Host "  استبدال؟ (ن/ل) [ن=نعم]"
                if ($answer -notin @("ن", "نعم", "y", "yes", "")) { Write-Info "تم الإلغاء"; exit 0 }
            }
            Remove-Item -Path $InstallDir -Recurse -Force
        }

        New-Item -ItemType Directory -Path $InstallDir -Force | Out-Null
        $مؤقت = Join-Path ([System.IO.Path]::GetTempPath()) ("sad-" + [System.Guid]::NewGuid().ToString("N"))
        New-Item -ItemType Directory -Path $مؤقت -Force | Out-Null
        Expand-Archive -Path $tempZip -DestinationPath $مؤقت -Force

        # (AR) الأرشيف يحمل مجلّداً أعلى واحداً (sad-vX-windows-x86_64\) — يُقشَّر.
        #      وقبل التقشير كان bin ينشأ فارغاً ثم تُنقل إليه كلّ ‎*.exe‎ بالمسح
        #      المتكرّر — فينجح بالمصادفة ويترك stdlib في مجلّد آخر.
        # (EN) Strip the single top-level directory the archive carries.
        $جذر = Get-ChildItem -Path $مؤقت -Directory | Select-Object -First 1
        if ($null -ne $جذر -and (Test-Path (Join-Path $جذر.FullName "bin"))) {
            Copy-Item -Path (Join-Path $جذر.FullName "*") -Destination $InstallDir -Recurse -Force
        } else {
            Copy-Item -Path (Join-Path $مؤقت "*") -Destination $InstallDir -Recurse -Force
        }
        Remove-Item -Path $مؤقت -Recurse -Force -ErrorAction SilentlyContinue

        # تحديد مجلد bin
        $binDir = Join-Path $InstallDir "bin"
        if (-not (Test-Path $binDir)) {
            New-Item -ItemType Directory -Path $binDir -Force | Out-Null
            Get-ChildItem -Path $InstallDir -Filter "*.exe" -Recurse | Move-Item -Destination $binDir -ErrorAction SilentlyContinue
            Get-ChildItem -Path $InstallDir -Filter "*.dll" -Recurse | Move-Item -Destination $binDir -ErrorAction SilentlyContinue
        }

        # (AR) حكمٌ لا وصف: تثبيتٌ بلا أمرٍ واحدٍ قابلٍ للتشغيل إخفاقٌ يُعلَن.
        # (EN) A judgement, not a description: an install with no runnable
        #      command is a failure to announce, not a success to be disproved.
        # (AR) 🔑 هذه القائمةُ نسخةٌ ثالثةٌ من جدولِ الأدواتِ الواحد
        #      (scripts/ci/release_tools.sh: SAD_REQUIRED_*). المُثبِّتُ يُشحَنُ
        #      وحدَه فلا يستطيعُ استيرادَه، فيربطُ الطرفَينِ حارسُ
        #      scripts/ci/check_installer_tool_lists.py. كانت «compiler»
        #      تطلبُ sadc وحدَه و«full» تُغفِلُ sad-build، فيقبلُ المُثبِّتُ
        #      حزمةً يرفضُها المُصدِرُ ويقولُ للمستخدمِ «تمّ» وهي ناقصة.
        # (EN) A third copy of the single tool table; bound to it by
        #      check_installer_tool_lists.py. It had drifted: "compiler"
        #      required only sadc and "full" omitted sad-build.
        $مطلوب = switch ($Components) {
            # (AR) 🔑 أُضيف sad-build: لا شيءَ في المكوّنِ كان يُشغّلُ برنامجَ ص.
            "interpreter" { @("sad", "sad-build", "sad-lsp", "sad-check") }
            "compiler"    { @("sadc", "sad-build") }
            "full"        { @("sad", "sad-lsp", "sad-check", "sadc", "sad-build") }
            default       { @("sad") }
        }
        $ناقص = @()
        foreach ($أداة in $مطلوب) {
            $مسار = Join-Path $binDir "$أداة.exe"
            if (-not (Test-Path $مسار)) { $ناقص += $أداة }
        }
        if ($ناقص.Count -gt 0) {
            Write-Err "الحزمة ناقصة — أدوات موعودة غائبة عن $binDir : $($ناقص -join ' ')"
            Write-Info "الموجود فعلاً:"
            Get-ChildItem -Path $binDir -ErrorAction SilentlyContinue |
                ForEach-Object { Write-Info "  - $($_.Name)" }
            throw "توقّف التثبيت — لا تُترك أدوات ناقصة على الجهاز بصمت"
        }

        Write-OK "تم تثبيت الملفات في: $InstallDir"

        # عرض المكونات
        Write-Step "المكونات المثبتة:"
        $exes = Get-ChildItem -Path $binDir -Filter "*.exe" -ErrorAction SilentlyContinue
        foreach ($exe in $exes) {
            $desc = switch ($exe.BaseName) {
                "sad"            { "المفسر — يشغل ملفات .ص مباشرة" }
                "sadc"           { "المترجم — يحوّل .ص إلى ملف تنفيذي أصلي (LLVM)" }
                "sad-lsp"        { "خادم LSP — تكامل مع VS Code والمحررات" }
                "sad-pkg"        { "مدير الحزم — تثبيت المكتبات" }
                "sad-fmt"        { "أداة تنسيق الكود" }
                default          { $exe.BaseName }
            }
            Write-Info "  $($exe.Name) — $desc"
        }

        # إضافة PATH
        if (-not $NoPath) {
            Write-Step "إضافة لغة ص لمتغير PATH..."
            $currentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
            if ($currentPath -notlike "*$binDir*") {
                [Environment]::SetEnvironmentVariable("PATH", "$binDir;$currentPath", "User")
                $env:PATH = "$binDir;$env:PATH"
                Write-OK "تمت إضافة $binDir لمتغير PATH"
            } else {
                Write-OK "المسار موجود بالفعل في PATH"
            }
        }

        # ربط امتداد .ص
        $sadExePath = Join-Path $binDir "sad.exe"
        if (Test-Path $sadExePath) {
            Write-Step "ربط الملفات بامتداد .ص..."
            try {
                $null = New-Item -Path "HKCU:\Software\Classes\.ص" -Value "SadLanguage.File" -Force
                $null = New-Item -Path "HKCU:\Software\Classes\SadLanguage.File" -Force
                $null = New-Item -Path "HKCU:\Software\Classes\SadLanguage.File\DefaultIcon" -Value "$sadExePath,0" -Force
                $null = New-Item -Path "HKCU:\Software\Classes\SadLanguage.File\shell\open\command" -Value "`"$sadExePath`" `"%1`"" -Force
                Write-OK "تم ربط .ص مع sad.exe"
            } catch { Write-Warn "لم يتم ربط الامتداد (اختياري)" }
        }

        # كتابة معلومات التثبيت
        @{
            version     = $version
            components  = $Components
            arch        = $arch
            installDir  = $InstallDir
            binDir      = $binDir
            installedAt = (Get-Date -Format "yyyy-MM-dd HH:mm:ss")
            installedBy = "install.ps1"
            source      = "GitHub Releases (github.com/$REPO_OWNER/$REPO_NAME)"
        } | ConvertTo-Json | Out-File -FilePath (Join-Path $InstallDir "install-info.json") -Encoding utf8

        # التحقق
        Write-Step "التحقق من التثبيت..."
        # (AR) 🔑 سطرُ عرضٍ لا حَكَم — وكان يعرضُ الفشلَ نجاحًا. `--version`
        #      ليس عَلَمًا للمترجم: يردُّه «خيارًا غيرَ معروف» على stderr ويفشل،
        #      فكان السطرُ يقولُ «موجود ✓» على فشلٍ محقَّق.
        #      ⚠️ و`catch` كان يقعُ لسببٍ آخرَ غيرِ رمزِ الخروج: مع
        #      `$ErrorActionPreference = "Stop"` (في إعداداتِ الملفِّ أعلاه) تُحوَّلُ كتابةُ
        #      ثنائيٍّ أصليٍّ على stderr إلى `NativeCommandError` رافع. فحذفُه
        #      كان يُسقِطُ المثبِّتَ **بعدَ نجاحِ التثبيت**، عندَ خطوةِ التحقّقِ
        #      وحدَها. فيُعادُ `catch` صريحًا، والحكمُ برمزِ الخروجِ وبالمخرَج.
        #      ⚠️ ولا يُبتَرُ المخرَجُ قبلَ قراءةِ `$LASTEXITCODE`:
        #      `| Select-Object -First 1` يُنهي العمليّةَ الأصليّةَ فيُكتَبُ
        #      `-1` مكانَ رمزِ خروجِها. وليس ذلك حتميًّا: قِيسَ فوجِدَ أنّ
        #      الـ`-1` لا يُكتَبُ إلّا إن قُطِعَ الأنبوبُ **قبلَ** انتهاءِ
        #      العمليّة، فمخرَجٌ قصيرٌ ينجو ومخرَجٌ طويلٌ لا ينجو — أي حُكمٌ
        #      يتبعُ طولَ المخرَجِ لا العقد. فيُلتقَطُ كاملًا، ثمّ يُحكَم، ثمّ يُقَصّ.
        #      والإملاءُ لكلِّ أداةٍ من scripts/ci/release_tools.sh ·
        #      SAD_VERSION_FLAGS: `sad` مركزُ الأدواتِ خارجَ جدولِ الأعلامِ
        #      ويقبلُ `--version`، و`sadc` محرّكٌ ملزَمٌ بـ`--إصدار`
        #      (cli_flags.yaml · flag.version). ويُجرَّبُ الإملاءان: تمريرُ
        #      وسيطٍ عربيٍّ إلى ثنائيٍّ أصليٍّ قد تُفسدُه صفحةُ المحارفِ في
        #      Windows PowerShell 5.1. ولا يُصلِحُ ذلك سقوطٌ إلى `--version`:
        #      قِيسَ في الجدولِ المولَّدِ فليس فيه مرادفٌ إنجليزيّ، فالفرعُ لا
        #      ينجحُ في الحالَين معًا — وحُذف؛ والفشلُ يُعرَضُ «موجود ✓».
        #      ⚠️ وليس هذا نظيرَ tools/installers/unix/install.sh: ذاك يحكمُ
        #      بفراغِ المخرَجِ وحدَه، وهذا يشترطُ رمزَ الخروجِ والمخرَجَ معًا.
        #      ⚠️ ويُخفَّضُ `$ErrorActionPreference` محلّيًّا: تحتَ `Stop` تُحوَّلُ
        #      **أيُّ** كتابةٍ على stderr إلى استثناءٍ ولو خرجَتِ الأداةُ صفرًا —
        #      قِيسَ — فيسبقُ الاستثناءُ الحُكمَ برمزِ الخروجِ وبالمخرَجِ معًا،
        #      فيُهدَرُ سطرُ إصدارٍ صحيحٌ لأجلِ بايتِ تحذيرٍ واحد.
        # (EN) A display line, not a judgement — it used to display failure as
        #      success: --version is not a compiler flag, the compiler rejects
        #      it on stderr and fails, and the line reported "exists".
        #      catch DID fire, but from stderr under $ErrorActionPreference =
        #      "Stop" (set in this file's preamble), not from the exit code, so
        #      abort the installer after a successful install. And
        #      $LASTEXITCODE must be read BEFORE any truncating pipeline:
        #      Select-Object -First stops the upstream and writes -1 in its
        #      place. Spellings come from SAD_VERSION_FLAGS. $ErrorActionPreference
        #      is lowered locally: under Stop ANY stderr byte raises, even from a
        #      tool that exited 0, pre-empting both declared judgements.
        foreach ($exe in @("sad.exe", "sadc.exe")) {
            $exePath = Join-Path $binDir $exe
            if (Test-Path $exePath) {
                $versionLine = $null
                if ($exe -eq "sad.exe") { $spellings = @("--version") }
                else { $spellings = @("--إصدار") }
                foreach ($spelling in $spellings) {
                    if ($versionLine) { break }
                    $output = $null
                    $previousPreference = $ErrorActionPreference
                    $ErrorActionPreference = "Continue"
                    try {
                        $output = & $exePath $spelling 2>$null
                        if ($LASTEXITCODE -eq 0 -and $output) {
                            $versionLine = @($output)[0]
                        }
                    }
                    catch { }
                    finally { $ErrorActionPreference = $previousPreference }
                }
                if ($versionLine) { Write-OK "${exe}: $versionLine" }
                else { Write-OK "${exe}: موجود ✓" }
            }
        }
    }
    finally {
        if (Test-Path $tempDir) { Remove-Item -Path $tempDir -Recurse -Force -ErrorAction SilentlyContinue }
    }
}

# ──────────────────────────────────────────────────────────────────────
# إزالة التثبيت / Uninstall
# ──────────────────────────────────────────────────────────────────────
function Uninstall-Sad {
    Write-Logo
    Write-Host "  إزالة لغة ص..." -ForegroundColor Yellow
    Write-Host ""

    if (-not (Test-Path $InstallDir)) {
        Write-Warn "لغة ص غير مثبتة في: $InstallDir"
        exit 0
    }

    Write-Step "إزالة من PATH..."
    $currentPath = [Environment]::GetEnvironmentVariable("PATH", "User")
    $binDir = Join-Path $InstallDir "bin"
    $newPath = ($currentPath -split ";" | Where-Object { $_ -ne $binDir -and $_ -ne $InstallDir }) -join ";"
    [Environment]::SetEnvironmentVariable("PATH", $newPath, "User")
    Write-OK "تمت الإزالة من PATH"

    Write-Step "إزالة ربط .ص..."
    Remove-Item -Path "HKCU:\Software\Classes\.ص" -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -Path "HKCU:\Software\Classes\SadLanguage.File" -Recurse -Force -ErrorAction SilentlyContinue
    Write-OK "تم"

    Write-Step "حذف الملفات..."
    Remove-Item -Path $InstallDir -Recurse -Force
    Write-OK "تم حذف $InstallDir"

    Write-Host ""
    Write-Host "  ✓ تمت إزالة لغة ص بنجاح" -ForegroundColor Green
    Write-Host "    أعد فتح الطرفية لتحديث PATH" -ForegroundColor DarkGray
    Write-Host ""
}

# ══════════════════════════════════════════════════════════════════════
# نقطة الدخول / Entry Point
# ══════════════════════════════════════════════════════════════════════
Write-Logo

if ($Uninstall) { Uninstall-Sad; exit 0 }

Write-Host "  مرحباً! سيتم تثبيت لغة ص على جهازك." -ForegroundColor White
Write-Host "  المصدر: GitHub Releases (github.com/$REPO_OWNER/$REPO_NAME)" -ForegroundColor DarkGray
Write-Host "  المجلد: $InstallDir" -ForegroundColor DarkGray
Write-Host ""

Show-ComponentMenu
Test-Requirements

$releaseInfo = Get-ReleaseInfo
Install-Sad -ReleaseInfo $releaseInfo

$componentName = switch ($Components) {
    "interpreter" { "المفسر (sad.exe)" }
    "compiler"    { "المترجم (sadc.exe)" }
    "full"        { "الحزمة الكاملة" }
}

Write-Host ""
Write-Host "  ═══════════════════════════════════════════════" -ForegroundColor Green
Write-Host "  ✓ تم تثبيت $componentName v$($releaseInfo.Version) بنجاح!" -ForegroundColor Green
Write-Host "  ═══════════════════════════════════════════════" -ForegroundColor Green
Write-Host ""
Write-Host "  للبدء:" -ForegroundColor White
if ($Components -in @("interpreter", "full")) {
    Write-Host "    sad --help              عرض المساعدة" -ForegroundColor DarkGray
    Write-Host "    sad script.ص           تشغيل ملف" -ForegroundColor DarkGray
}
if ($Components -in @("compiler", "full")) {
    Write-Host "    sadc script.ص          ترجمة إلى ملف تنفيذي" -ForegroundColor DarkGray
}
if ($Components -eq "full") {
    Write-Host "    sad-pkg init            إنشاء مشروع جديد" -ForegroundColor DarkGray
}
Write-Host ""
Write-Host "  ⚡ أعد فتح الطرفية لتفعيل PATH" -ForegroundColor Yellow
Write-Host ""
