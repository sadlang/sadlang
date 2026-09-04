#!/usr/bin/env pwsh
# (AR) ⚠️ **لا تضعْ BOM في مقدّمةِ هذا الملفّ.** النواةُ لا تطابقُ `#!`
#      إلّا في الإزاحةِ صفرًا، فبايتاتُ BOM قبلَه تجعلُ `./create-release.ps1`
#      يُخفِقُ بـexec format error على لينكس وماك. وقد أُضيفَ BOM هنا مرّةً
#      لإسكاتِ أخطاءِ تحليلٍ ظهرت تحتَ Windows PowerShell 5.1 — والقياسُ
#      كان بمفسّرٍ خاطئ: الشِّبانغُ أعلاه يُعلِنُ `pwsh` (٧+)، وهو يقرأ
#      UTF-8 بلا BOM على المنصّاتِ الثلاث. فالملفُّ يبقى بلا BOM.
# (EN) Do NOT put a BOM at the head of this file. The kernel matches #! at
#      offset 0 only, so BOM bytes in front of it make ./create-release.ps1
#      fail with exec format error on Linux and macOS. A BOM was added here
#      once to silence parse errors seen under Windows PowerShell 5.1 —
#      measured with the wrong interpreter: the shebang above declares pwsh
#      7+, which reads BOM-less UTF-8 on all three platforms.
# بسم الله الرحمن الرحيم
# ═══════════════════════════════════════════════════════════════════════
# سكريبت إعداد إصدار جديد — لغة ص
# Create New Release — Sad Programming Language
# ═══════════════════════════════════════════════════════════════════════
#
# الاستخدام:
#   .\create-release.ps1 -Version "2.0.0"
#   .\create-release.ps1 -Version "2.0.0" -Release
#   .\create-release.ps1 -Version "2.0.0" -Push
# ═══════════════════════════════════════════════════════════════════════

[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)]
    [string]$Version,
    
    [switch]$Release,
    [switch]$Push,
    [switch]$DryRun
)

$ErrorActionPreference = "Stop"
$ScriptDir = $PSScriptRoot
$RepoRoot = Split-Path $ScriptDir -Parent

# الألوان
function Write-Banner {
    Write-Host ""
    Write-Host "  ╔═══════════════════════════════════════════════════════════╗" -ForegroundColor Cyan
    Write-Host "  ║        لغة ص — إنشاء الإصدار v$Version                   ║" -ForegroundColor Cyan
    Write-Host "  ║        Sad Language — Release Builder                     ║" -ForegroundColor Cyan
    Write-Host "  ╚═══════════════════════════════════════════════════════════╝" -ForegroundColor Cyan
    Write-Host ""
}

function Write-Step  { param($m) Write-Host "  [>>] " -ForegroundColor Blue   -NoNewline; Write-Host $m }
function Write-OK    { param($m) Write-Host "  [✓]  " -ForegroundColor Green  -NoNewline; Write-Host $m }
function Write-Warn  { param($m) Write-Host "  [!]  " -ForegroundColor Yellow -NoNewline; Write-Host $m }
function Write-Error2{ param($m) Write-Host "  [X]  " -ForegroundColor Red    -NoNewline; Write-Host $m }

Write-Banner

# ═══════════════════════════════════════════════════════════════════════
#  الخطوة 1: تحديث رقم الإصدار في الملفات
# ═══════════════════════════════════════════════════════════════════════

Write-Step "تحديث أرقام الإصدار..."

# CMakeLists.txt
$cmakePath = Join-Path $RepoRoot "CMakeLists.txt"
if (Test-Path $cmakePath) {
    $content = Get-Content $cmakePath -Raw
    $content = $content -replace 'project\(SadLanguage VERSION \d+\.\d+\.\d+', "project(SadLanguage VERSION $Version"
    if (-not $DryRun) {
        Set-Content $cmakePath $content -Encoding UTF8
    }
    Write-OK "CMakeLists.txt"
}

# CHANGELOG.md — إضافة قسم جديد
$changelogPath = Join-Path $RepoRoot "CHANGELOG.md"
if (Test-Path $changelogPath) {
    $date = Get-Date -Format "yyyy-MM-dd"
    $newSection = @"

## [$Version] - $date

### أضيف (Added)
- دعم Android SDK كامل (35+ ملف Kotlin/JNI)
- مكونات: NFC, HealthConnect, Maps, Billing, Firebase, WorkManager, Bluetooth
- واجهة Compose UI مع محرر كود ومحطة REPL

### تحسينات (Changed)
- تحسين أداء المفسر
- تحديث نظام الأنواع

### إصلاحات (Fixed)
- إصلاحات متعددة للثبات

"@
    $content = Get-Content $changelogPath -Raw
    $content = $content -replace '(# Changelog\s*)', "`$1$newSection"
    if (-not $DryRun) {
        Set-Content $changelogPath $content -Encoding UTF8
    }
    Write-OK "CHANGELOG.md"
}

# ═══════════════════════════════════════════════════════════════════════
#  الخطوة 2: البناء
# ═══════════════════════════════════════════════════════════════════════

Write-Step "بناء المشروع (Release)..."

if (-not $DryRun) {
    $BuildDir = Join-Path $RepoRoot "build"
    
    # Configure if needed
    if (-not (Test-Path (Join-Path $BuildDir "CMakeCache.txt"))) {
        cmake -S $RepoRoot -B $BuildDir
    }
    
    # ══════════════════════════════════════════════════════════════════
    # (AR) 🔑 **الهدفُ الأوّلُ كان `sad-run` — المحرّكَ المحذوف.**
    #
    #      ⚠️ وأخطرُ من ذلك أنّ إخفاقَه **لا يوقفُ شيئًا**:
    #      `$ErrorActionPreference = "Stop"` (السطر ٢٤) لا يسري على أمرٍ
    #      أصليٍّ في پاورشِل ٥٫١ — يُطبَعُ الخطأُ ثمّ يمضي السكربتُ إلى
    #      التحزيمِ بحزمةٍ ناقصةٍ ويقولُ «البناء مكتمل».
    #      فهو يُخفِقُ ويكذبُ معًا، وهو أسوأُ من الإخفاقِ وحدَه.
    #
    #      فالحكمُ من `$LASTEXITCODE` بعدَ كلِّ هدف. والأهدافُ صارت ما يُشحَنُ
    #      فعلًا: المركزُ والمترجمُ وما يليهما.
    # (EN) The first target was sad-run, the deleted engine — and worse, its
    #      failure stopped nothing: $ErrorActionPreference = "Stop" does not
    #      apply to native commands in PowerShell 5.1, so the error printed and
    #      the script walked on to packaging an incomplete bundle and announced
    #      "build complete". It failed and lied at once, which is worse than
    #      failing. The verdict now comes from $LASTEXITCODE after each target,
    #      and the targets are what actually ships.
    # ══════════════════════════════════════════════════════════════════
    foreach ($target in @("sad", "sad-build", "sad-lsp", "sad-pkg", "sad-fmt")) {
        cmake --build $BuildDir --config Release --target $target
        if ($LASTEXITCODE -ne 0) {
            throw "فشل بناء الهدف '$target' (رمز $LASTEXITCODE) — لا تُحزَّم حزمةٌ ناقصة"
        }
    }
}

Write-OK "البناء مكتمل"

# ═══════════════════════════════════════════════════════════════════════
#  الخطوة 3: إنشاء حزمة التوزيع
# ═══════════════════════════════════════════════════════════════════════

Write-Step "إنشاء حزمة التوزيع..."

$DistDir = Join-Path $ScriptDir "output"
$PackageDir = Join-Path $DistDir "sad-v$Version-windows-x86_64"
$ZipPath = "$PackageDir.zip"

if (-not $DryRun) {
    # إنشاء مجلد الحزمة
    New-Item -ItemType Directory -Path $PackageDir -Force | Out-Null
    
    $BinDir = Join-Path $RepoRoot "build\bin\Release"
    
    # نسخ الملفات التنفيذية
    Copy-Item (Join-Path $BinDir "sad.exe") $PackageDir -Force
    if (Test-Path (Join-Path $BinDir "sad-lsp.exe")) {
        Copy-Item (Join-Path $BinDir "sad-lsp.exe") $PackageDir -Force
    }
    if (Test-Path (Join-Path $BinDir "sad-pkg.exe")) {
        Copy-Item (Join-Path $BinDir "sad-pkg.exe") $PackageDir -Force
    }
    if (Test-Path (Join-Path $BinDir "sad-fmt.exe")) {
        Copy-Item (Join-Path $BinDir "sad-fmt.exe") $PackageDir -Force
    }
    
    # نسخ DLLs
    Get-ChildItem (Join-Path $BinDir "*.dll") | ForEach-Object {
        Copy-Item $_.FullName $PackageDir -Force
    }
    
    # نسخ المكتبة القياسية
    $StdlibSrc = Join-Path $RepoRoot "stdlib"
    $StdlibDst = Join-Path $PackageDir "stdlib"
    if (Test-Path $StdlibSrc) {
        Copy-Item $StdlibSrc $StdlibDst -Recurse -Force
    }
    # (AR) وحدات stdlib الخاصّة بالميزات (RFC #19): تُدمَج في نفس مجلّد stdlib (رسومات)
    # (EN) Per-feature stdlib modules (RFC #19): merged into the same stdlib dir (رسومات)
    $FeatStdlibSrc = Join-Path $RepoRoot "features\graphics\stdlib"
    if ((Test-Path $FeatStdlibSrc) -and (Test-Path $StdlibDst)) {
        Copy-Item (Join-Path $FeatStdlibSrc "*") $StdlibDst -Recurse -Force
    }
    
    # نسخ الأمثلة
    $ExamplesSrc = Join-Path $RepoRoot "examples"
    $ExamplesDst = Join-Path $PackageDir "examples"
    if (Test-Path $ExamplesSrc) {
        Copy-Item $ExamplesSrc $ExamplesDst -Recurse -Force
    }
    
    # نسخ الوثائق
    Copy-Item (Join-Path $RepoRoot "README.md") $PackageDir -Force
    Copy-Item (Join-Path $RepoRoot "LICENSE") $PackageDir -Force
    
    # إنشاء ملف الإصدار
    @{
        version = $Version
        date = (Get-Date -Format "yyyy-MM-dd")
        platform = "windows-x86_64"
        components = @("sad", "sad-lsp", "sad-pkg", "sad-fmt", "stdlib")
    } | ConvertTo-Json | Set-Content (Join-Path $PackageDir "version.json") -Encoding UTF8
    
    # ضغط الملفات
    Compress-Archive -Path $PackageDir -DestinationPath $ZipPath -Force
    
    # حساب SHA256
    $hash = (Get-FileHash $ZipPath -Algorithm SHA256).Hash.ToLower()
    Set-Content "$ZipPath.sha256" "$hash  sad-v$Version-windows-x86_64.zip" -Encoding ASCII
}

Write-OK "الحزمة: $ZipPath"

# ═══════════════════════════════════════════════════════════════════════
#  الخطوة 4: إنشاء Git Tag (اختياري)
# ═══════════════════════════════════════════════════════════════════════

if ($Release) {
    Write-Step "إنشاء Git Tag..."
    
    if (-not $DryRun) {
        Set-Location $RepoRoot
        git add -A
        git commit -m "Release v$Version"
        git tag -a "v$Version" -m "Sad Language v$Version"
        
        if ($Push) {
            Write-Step "رفع إلى GitHub..."
            git push origin main
            git push origin "v$Version"
            Write-OK "تم الرفع"
        }
    }
    
    Write-OK "Git Tag: v$Version"
}

# ═══════════════════════════════════════════════════════════════════════
#  ملخص
# ═══════════════════════════════════════════════════════════════════════

Write-Host ""
Write-Host "  ═══════════════════════════════════════════════════════════" -ForegroundColor Green
Write-Host "  ✓ الإصدار v$Version جاهز للنشر!" -ForegroundColor Green
Write-Host "  ═══════════════════════════════════════════════════════════" -ForegroundColor Green
Write-Host ""
Write-Host "  الملفات المُنشأة:" -ForegroundColor White
Write-Host "    • $ZipPath" -ForegroundColor DarkGray
Write-Host "    • $ZipPath.sha256" -ForegroundColor DarkGray
Write-Host ""

if (-not $Release) {
    Write-Host "  للنشر الرسمي:" -ForegroundColor Yellow
    Write-Host "    .\create-release.ps1 -Version $Version -Release -Push" -ForegroundColor Cyan
}

Write-Host ""
