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
#   https://github.com/sad-lang/sad-language/releases
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

$REPO_OWNER = "sad-lang"
$REPO_NAME  = "sad-language"
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
        Write-Step "التحقق من وجود LLVM (مطلوب للمترجم)..."
        $llvmFound = $false
        foreach ($p in @("C:\Program Files\LLVM", "C:\LLVM", "C:\LLVM-Dev")) {
            if (Test-Path $p) { $llvmFound = $true; Write-OK "LLVM موجود: $p"; break }
        }
        if (-not $llvmFound) {
            Write-Warn "LLVM غير مثبت — sadc يحتاج LLVM 18 لتحويل .ص إلى ملف تنفيذي"
            Write-Info "حمّل LLVM من: https://github.com/llvm/llvm-project/releases"
            Write-Info "المفسر sad.exe يعمل بدون LLVM"
        }
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
        Expand-Archive -Path $tempZip -DestinationPath $InstallDir -Force

        # تحديد مجلد bin
        $binDir = Join-Path $InstallDir "bin"
        if (-not (Test-Path $binDir)) {
            New-Item -ItemType Directory -Path $binDir -Force | Out-Null
            Get-ChildItem -Path $InstallDir -Filter "*.exe" -Recurse | Move-Item -Destination $binDir -ErrorAction SilentlyContinue
            Get-ChildItem -Path $InstallDir -Filter "*.dll" -Recurse | Move-Item -Destination $binDir -ErrorAction SilentlyContinue
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
                "sad-lsp-server" { "خادم LSP الجديد" }
                "sad-pkg"        { "مدير الحزم — تثبيت المكتبات" }
                "sad-repl"       { "بيئة تفاعلية" }
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
        foreach ($exe in @("sad.exe", "sadc.exe")) {
            $exePath = Join-Path $binDir $exe
            if (Test-Path $exePath) {
                try { $v = & $exePath --version 2>&1; Write-OK "${exe}: $v" }
                catch { Write-OK "${exe}: موجود ✓" }
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
    Write-Host "    sad-repl                بيئة تفاعلية" -ForegroundColor DarkGray
}
Write-Host ""
Write-Host "  ⚡ أعد فتح الطرفية لتفعيل PATH" -ForegroundColor Yellow
Write-Host ""
