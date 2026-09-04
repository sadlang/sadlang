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
#   .\install.ps1 -Components standard     # المترجم + المكتبة القياسية + الأدوات الأساسية
#   .\install.ps1 -Components full         # المترجم + كل الأدوات
#   .\install.ps1 -Version 1.2.0           # إصدار محدد
#   .\install.ps1 -InstallDir "D:\sad"     # مجلد مخصص
#   .\install.ps1 -NoPath                  # لا يضيف لمتغير PATH
#   .\install.ps1 -Uninstall               # إزالة التثبيت
#
# ═══════════════════════════════════════════════════════════════════════════════
#
# مصدر التحميل / Download Source:
#   يتم تحميل الملفات من GitHub Releases:
#   https://github.com/sadlang/sadlang/releases
#
#   ملفات التحميل المتاحة لكل إصدار:
#   ┌─────────────────────────────────────────────────────────────────┐
#   │ sad-v{VER}-windows-x86_64.zip        المترجم + الأدوات الأساسية│
#   │ sad-full-v{VER}-windows-x86_64.zip   المترجم + كل الأدوات     │
#   │                                                                │
#   │ نفس الملفات متاحة لـ:                                         │
#   │   linux-x86_64, linux-aarch64                                  │
#   │   macos-x86_64, macos-aarch64                                  │
#   └─────────────────────────────────────────────────────────────────┘
#
# ═══════════════════════════════════════════════════════════════════════════════

[CmdletBinding()]
param(
    [ValidateSet("standard", "full")]
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

# (AR) 🔑 المستودعُ العلنيُّ الذي يصدرُ منه الإصدارُ الرسميّ.
#      وكان يشيرُ إلى مستودعٍ صارَ **خاصًّا**، فكانت كلُّ محاولةِ تثبيتٍ في
#      العالمِ تقعُ على 404، واسمُ مستودعٍ خاصٍّ مكتوبٌ في سكربتٍ يُشحَنُ للناس.
# (EN) 🔑 The public repository the official release is issued from.
#      This pointed at a repository that has since been made PRIVATE, so every
#      install attempt in the wild would 404 — and a private repository's name
#      sat in a script shipped to users.
$REPO_OWNER = "sadlang"
$REPO_NAME  = "sadlang"
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
    Write-Host "  ║  [1] الحزمة القياسية (standard)" -NoNewline -ForegroundColor White
    Write-Host "                              ║" -ForegroundColor White
    Write-Host "  ║      sad.exe + المكتبة القياسية + الأدوات" -NoNewline -ForegroundColor DarkGray
    Write-Host "                  ║" -ForegroundColor White
    Write-Host "  ║      ← الأفضل لمعظم المستخدمين" -NoNewline -ForegroundColor Green
    Write-Host "                             ║" -ForegroundColor White
    Write-Host "  ║                                                              ║" -ForegroundColor White
    Write-Host "  ║  [2] الحزمة الكاملة (full)" -NoNewline -ForegroundColor White
    Write-Host "                                  ║" -ForegroundColor White
    Write-Host "  ║      + LSP + REPL + مدير الحزم + المنسّق" -NoNewline -ForegroundColor DarkGray
    Write-Host "                   ║" -ForegroundColor White
    Write-Host "  ║      ← كل شيء في حزمة واحدة" -NoNewline -ForegroundColor Cyan
    Write-Host "                              ║" -ForegroundColor White
    Write-Host "  ║                                                              ║" -ForegroundColor White
    Write-Host "  ╚════════════════════════════════════════════════════════════════╝" -ForegroundColor White
    Write-Host ""

    $choice = Read-Host "  اختر رقم (1/2) [الافتراضي: 1]"
    switch ($choice) {
        "2" { $script:Components = "full" }
        default { $script:Components = "standard" }
    }

    $componentName = switch ($script:Components) {
        "full" { "الحزمة الكاملة (sad + sadc + أدوات)" }
        default { "الحزمة القياسية (sad.exe)" }
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

}

# ──────────────────────────────────────────────────────────────────────
# (AR) 🔑 نُزعت آلةُ LLVM كاملةً (Find-LLVM · Test-LLVMPath ·
#      Set-LLVMEnvironment · Install-LLVM) — لأنّ شرطَها زال: الحزمُ صارت
#      تُربَطُ ساكنًا (`SAD_LLVM_STATIC=ON`) فلا تشترطُ شيئًا على جهازِ
#      المستخدم. ومعها زالَ مسارُ التراجعِ «إن غابت LLVM فثبّتِ المفسّرَ
#      بدلًا»: لا مفسّرَ بعدَ حذفِه، وكلتا الحزمتَينِ تحملانِ المترجِم.
#      والدعوى مقيسةٌ في مجرى الإصدار: فحصُ التبعيّاتِ لا يأذنُ بـ`libLLVM`.
# (EN) 🔑 The entire LLVM subsystem is removed — its premise is gone:
#      packages are linked statically and require nothing on the user's machine.
#      With it goes the "no LLVM ⇒ install the interpreter instead" fallback:
#      there is no interpreter left, and both packages carry the compiler.
#      Measured in the release workflow, whose dependency check allows no
#      libLLVM at all.
# ──────────────────────────────────────────────────────────────────────

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
        "full"  { "sad-full" }
        default { "sad" }
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
            "standard" { @("sad", "sad-build", "sad-lsp", "sad-check") }
            "full"     { @("sad", "sad-lsp", "sad-check", "sadc", "sad-build") }
            default    { @("sad") }
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
    "full"  { "الحزمة الكاملة" }
    default { "الحزمة القياسية (sad.exe)" }
}

Write-Host ""
Write-Host "  ═══════════════════════════════════════════════" -ForegroundColor Green
Write-Host "  ✓ تم تثبيت $componentName v$($releaseInfo.Version) بنجاح!" -ForegroundColor Green
Write-Host "  ═══════════════════════════════════════════════" -ForegroundColor Green
Write-Host ""
Write-Host "  للبدء:" -ForegroundColor White
Write-Host "    sad --help              عرض المساعدة" -ForegroundColor DarkGray
Write-Host "    sad script.ص           تشغيل ملف" -ForegroundColor DarkGray
if ($Components -eq "full") {
    Write-Host "    sadc script.ص          ترجمة إلى ملف تنفيذي" -ForegroundColor DarkGray
}
if ($Components -eq "full") {
    Write-Host "    sad-pkg init            إنشاء مشروع جديد" -ForegroundColor DarkGray
}
Write-Host ""
Write-Host "  ⚡ أعد فتح الطرفية لتفعيل PATH" -ForegroundColor Yellow
Write-Host ""
