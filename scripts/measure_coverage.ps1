#Requires -Version 5.1
<#
.SYNOPSIS
    قياس تغطية الكود (Code Coverage) لمفسر لغة ص باستخدام LLVM Source-Based Coverage.

.DESCRIPTION
    المراحل:
      1. بناء sad.exe مع أعلام coverage (clang-cl + -fprofile-instr-generate)
      2. تشغيل 107 اختبار — يولّد ملفات .profraw لكل اختبار
      3. دمج الملفات بـ llvm-profdata merge
      4. توليد تقرير نصي + HTML بـ llvm-cov show/report
      5. طباعة ملخص النسب للملفات الرئيسية

.PARAMETER SkipBuild
    تجاوز مرحلة البناء (إذا كان sad_cov.exe موجوداً من قبل)

.PARAMETER HtmlOnly
    توليد HTML فقط من profdata موجود مسبقاً

.PARAMETER Verbose
    طباعة تفاصيل إضافية

.EXAMPLE
    .\scripts\measure_coverage.ps1
    .\scripts\measure_coverage.ps1 -SkipBuild
    .\scripts\measure_coverage.ps1 -Verbose
#>

param(
    [switch]$SkipBuild,
    [switch]$HtmlOnly,
    [switch]$Verbose
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# ═══════════════════════════════════════════════════════════════
# المسارات الثابتة
# ═══════════════════════════════════════════════════════════════
$ROOT = "C:\s_lang\s-programming-language"
$LLVM_BIN = "C:\llvm_dev\LLVM\bin"
$LLVM_COV = "$LLVM_BIN\llvm-cov.exe"
$LLVM_PROF = "$LLVM_BIN\llvm-profdata.exe"
$CLANGCL = "$LLVM_BIN\clang-cl.exe"
$NINJA = (Get-Command ninja -ErrorAction SilentlyContinue)?.Source

$BUILD_COV = "$ROOT\build_cov"
$PROFILES = "$BUILD_COV\profiles"
$SAD_COV = "$BUILD_COV\bin\sad_cov.exe"
$PROFDATA = "$BUILD_COV\coverage.profdata"
$HTML_DIR = "$ROOT\build\coverage_html"
$TESTS_DIR = "$ROOT\tests\dual_execution"

# مجلدات الكود المصدري المُشملة في التغطية
$SOURCE_ROOTS = @(
    "$ROOT\interpreter_new\src",
    "$ROOT\interpreter_new\include",
    "$ROOT\shared\lexer\src",
    "$ROOT\shared\parser\src",
    "$ROOT\shared\types\src",
    "$ROOT\shared\ast\src"
)

# ═══════════════════════════════════════════════════════════════
# دوال مساعدة
# ═══════════════════════════════════════════════════════════════

function Write-Step([string]$msg) {
    Write-Host "`n$('═' * 55)" -ForegroundColor Cyan
    Write-Host "  $msg" -ForegroundColor Cyan
    Write-Host "$('═' * 55)" -ForegroundColor Cyan
}

function Write-OK([string]$msg) { Write-Host "  ✅ $msg" -ForegroundColor Green }
function Write-Warn([string]$msg) { Write-Host "  ⚠️  $msg" -ForegroundColor Yellow }
function Write-Err([string]$msg) { Write-Host "  ❌ $msg" -ForegroundColor Red }

function Require-File([string]$path, [string]$label) {
    if (-not (Test-Path $path)) {
        Write-Err "$label غير موجود: $path"
        exit 1
    }
}

# ═══════════════════════════════════════════════════════════════
# التحقق من المتطلبات
# ═══════════════════════════════════════════════════════════════

Write-Step "التحقق من المتطلبات"

Require-File $LLVM_COV      "llvm-cov.exe"
Require-File $LLVM_PROF     "llvm-profdata.exe"
Require-File $CLANGCL       "clang-cl.exe"
Require-File "$ROOT\CMakeLists.txt" "CMakeLists.txt"

Write-OK "llvm-cov + llvm-profdata موجودان في $LLVM_BIN"

$CMAKE = (Get-Command cmake -ErrorAction SilentlyContinue)?.Source
if (-not $CMAKE) {
    $CMAKE = "C:\Program Files\CMake\bin\cmake.exe"
    if (-not (Test-Path $CMAKE)) {
        Write-Err "cmake غير موجود في PATH أو المسار الافتراضي"
        exit 1
    }
}
Write-OK "cmake: $CMAKE"

if ($NINJA) {
    Write-OK "ninja: $NINJA"
    $GENERATOR = "Ninja"
}
else {
    Write-Warn "ninja غير موجود — استخدام NMake Makefiles"
    $GENERATOR = "NMake Makefiles"
}

# ═══════════════════════════════════════════════════════════════
# المرحلة 1: بناء sad_cov.exe مع أعلام التغطية
# ═══════════════════════════════════════════════════════════════

if (-not $SkipBuild -and -not $HtmlOnly) {
    Write-Step "بناء sad_cov.exe مع LLVM Source Coverage"

    # إنشاء مجلد البناء
    if (-not (Test-Path $BUILD_COV)) {
        New-Item -ItemType Directory -Path $BUILD_COV | Out-Null
    }

    # أعلام التغطية لـ clang-cl
    $COV_FLAGS = "-fprofile-instr-generate -fcoverage-mapping"
    $COV_LINK = "-fprofile-instr-generate"

    Write-Host "  تهيئة CMake مع clang-cl..."

    $cmakeArgs = @(
        "-S", $ROOT,
        "-B", $BUILD_COV,
        "-G", $GENERATOR,
        "-DCMAKE_C_COMPILER=$CLANGCL",
        "-DCMAKE_CXX_COMPILER=$CLANGCL",
        "-DCMAKE_BUILD_TYPE=Debug",
        "-DCMAKE_CXX_FLAGS=$COV_FLAGS",
        "-DCMAKE_C_FLAGS=$COV_FLAGS",
        "-DCMAKE_EXE_LINKER_FLAGS=$COV_LINK",
        "-DBUILD_TESTS=OFF",
        "-DENABLE_LLVM_BACKEND=OFF",
        # تسمية الهدف sad_cov عبر output name
        "-DCMAKE_RUNTIME_OUTPUT_DIRECTORY=$BUILD_COV\bin"
    )

    if ($Verbose) {
        & $CMAKE @cmakeArgs
    }
    else {
        & $CMAKE @cmakeArgs 2>&1 | Select-Object -Last 5
    }

    if ($LASTEXITCODE -ne 0) {
        Write-Err "فشل تهيئة CMake — كود الخروج: $LASTEXITCODE"
        exit 1
    }

    Write-Host "  بناء هدف sad..."

    if ($Verbose) {
        & $CMAKE --build $BUILD_COV --target sad-run --config Debug
    }
    else {
        & $CMAKE --build $BUILD_COV --target sad-run --config Debug 2>&1 | Select-Object -Last 8
    }

    if ($LASTEXITCODE -ne 0) {
        Write-Err "فشل بناء sad — كود الخروج: $LASTEXITCODE"
        exit 1
    }

    # البحث عن sad.exe في مجلدات مختلفة
    $candidatePaths = @(
        "$BUILD_COV\bin\sad.exe",
        "$BUILD_COV\bin\Debug\sad.exe",
        "$BUILD_COV\bin\RelWithDebInfo\sad.exe"
    )
    $builtExe = $candidatePaths | Where-Object { Test-Path $_ } | Select-Object -First 1

    if (-not $builtExe) {
        Write-Err "لم يُعثر على sad.exe بعد البناء في $BUILD_COV\bin\"
        exit 1
    }

    # نسخه بأسم sad_cov.exe للتمييز
    Copy-Item $builtExe $SAD_COV -Force
    Write-OK "تم بناء: $SAD_COV"

}
else {
    Write-Step "تجاوز البناء — فحص sad_cov.exe"
    if (-not (Test-Path $SAD_COV)) {
        Write-Err "sad_cov.exe غير موجود: $SAD_COV — شغّل بدون -SkipBuild"
        exit 1
    }
    Write-OK "sad_cov.exe موجود: $SAD_COV"
}

# ═══════════════════════════════════════════════════════════════
# المرحلة 2: جمع ملفات الاختبار
# ═══════════════════════════════════════════════════════════════

if (-not $HtmlOnly) {
    Write-Step "تشغيل الاختبارات لجمع بيانات التغطية"

    # تنظيف ملفات profraw القديمة
    if (Test-Path $PROFILES) {
        Remove-Item "$PROFILES\*.profraw" -Force -EA SilentlyContinue
    }
    else {
        New-Item -ItemType Directory -Path $PROFILES | Out-Null
    }

    # جمع جميع ملفات الاختبار
    $testFiles = Get-ChildItem -Path $TESTS_DIR -Recurse -Filter "*.ص" | 
    Select-Object -ExpandProperty FullName |
    Sort-Object

    Write-Host "  الاختبارات: $($testFiles.Count) ملف"

    $passCount = 0
    $failCount = 0
    $idx = 0

    foreach ($testFile in $testFiles) {
        $idx++
        $testName = [System.IO.Path]::GetFileNameWithoutExtension($testFile)
        $profrawFile = "$PROFILES\$testName.profraw"

        # تعيين متغير البيئة لتحديد مسار ملف البروفايل
        $env:LLVM_PROFILE_FILE = $profrawFile

        try {
            $proc = Start-Process -FilePath $SAD_COV `
                -ArgumentList "`"$testFile`"" `
                -NoNewWindow -PassThru -Wait `
                -RedirectStandardOutput "$PROFILES\${testName}_out.txt" `
                -RedirectStandardError  "$PROFILES\${testName}_err.txt"

            if ($proc.ExitCode -eq 0) {
                $passCount++
                if ($Verbose) { Write-Host "    [$idx/$($testFiles.Count)] ✅ $testName" }
            }
            else {
                $failCount++
                if ($Verbose) { Write-Host "    [$idx/$($testFiles.Count)] ⚠️  $testName (exit: $($proc.ExitCode))" }
            }
        }
        catch {
            $failCount++
            if ($Verbose) { Write-Warn "خطأ في تشغيل: $testName — $_" }
        }
    }

    $env:LLVM_PROFILE_FILE = $null

    Write-OK "تشغيل الاختبارات: $passCount نجح / $failCount فشل (من أصل $($testFiles.Count))"

    # فحص عدد ملفات profraw المولّدة
    $profrawFiles = Get-ChildItem "$PROFILES\*.profraw" -EA SilentlyContinue
    if ($profrawFiles.Count -eq 0) {
        Write-Err "لم تُولَّد أي ملفات .profraw — تأكد من صحة sad_cov.exe"
        exit 1
    }
    Write-OK "ملفات .profraw المولّدة: $($profrawFiles.Count)"

    # ═══════════════════════════════════════════════════════════════
    # المرحلة 3: دمج ملفات .profraw
    # ═══════════════════════════════════════════════════════════════

    Write-Step "دمج ملفات .profraw → coverage.profdata"

    $profrawList = ($profrawFiles | Select-Object -ExpandProperty FullName) -join " "
    $mergeArgs = @("merge") + $profrawFiles.FullName + @("-output", $PROFDATA)

    & $LLVM_PROF @mergeArgs

    if ($LASTEXITCODE -ne 0) {
        Write-Err "فشل llvm-profdata merge — كود الخروج: $LASTEXITCODE"
        exit 1
    }

    if (-not (Test-Path $PROFDATA)) {
        Write-Err "لم يُنشأ coverage.profdata"
        exit 1
    }
    $profdataSize = (Get-Item $PROFDATA).Length / 1KB
    Write-OK "coverage.profdata: $([math]::Round($profdataSize, 1)) KB"
}

# ═══════════════════════════════════════════════════════════════
# المرحلة 4: توليد التقرير النصي
# ═══════════════════════════════════════════════════════════════

Write-Step "توليد التقرير النصي (llvm-cov report)"

Require-File $PROFDATA "coverage.profdata"

# بناء قائمة مسارات المصدر للتصفية
$sourceArgs = @()
foreach ($src in $SOURCE_ROOTS) {
    if (Test-Path $src) {
        $sourceArgs += "--sources=$src"
    }
}

$reportArgs = @(
    "report",
    $SAD_COV,
    "--instr-profile=$PROFDATA",
    "--ignore-filename-regex=third_party|_test|\.h$"
) + $sourceArgs

Write-Host ""
& $LLVM_COV @reportArgs

# حفظ التقرير النصي في ملف
$textReportPath = "$ROOT\build\coverage_report.txt"
& $LLVM_COV @reportArgs 2>&1 | Out-File $textReportPath -Encoding UTF8
Write-OK "التقرير النصي: $textReportPath"

# ═══════════════════════════════════════════════════════════════
# المرحلة 5: توليد تقرير HTML
# ═══════════════════════════════════════════════════════════════

Write-Step "توليد تقرير HTML (llvm-cov show)"

if (-not (Test-Path $HTML_DIR)) {
    New-Item -ItemType Directory -Path $HTML_DIR | Out-Null
}

$showArgs = @(
    "show",
    $SAD_COV,
    "--instr-profile=$PROFDATA",
    "--format=html",
    "--output-dir=$HTML_DIR",
    "--show-line-counts-or-regions",
    "--ignore-filename-regex=third_party|_test",
    "--project-title=لغة ص - تغطية الكود"
) + $sourceArgs

& $LLVM_COV @showArgs

if ($LASTEXITCODE -eq 0) {
    Write-OK "تقرير HTML: $HTML_DIR\index.html"
}
else {
    Write-Warn "فشل توليد HTML — كود الخروج: $LASTEXITCODE"
}

# ═══════════════════════════════════════════════════════════════
# الملخص النهائي
# ═══════════════════════════════════════════════════════════════

Write-Step "ملخص التغطية"

# استخراج الإجمالي من التقرير
if (Test-Path $textReportPath) {
    $totalLine = Get-Content $textReportPath | Select-String "TOTAL" | Select-Object -Last 1
    if ($totalLine) {
        Write-Host ""
        Write-Host "  $totalLine" -ForegroundColor Yellow
        Write-Host ""
    }
}

Write-Host "  الملفات:" -ForegroundColor White
Write-Host "    📄 تقرير نصي : $textReportPath" -ForegroundColor Gray
Write-Host "    🌐 تقرير HTML: $HTML_DIR\index.html" -ForegroundColor Gray
Write-Host "    📦 profdata  : $PROFDATA" -ForegroundColor Gray
Write-Host ""
Write-Host "  لتحديث الخطة شغّل:" -ForegroundColor White
Write-Host "    llvm-cov report $SAD_COV --instr-profile=$PROFDATA --summary-only" -ForegroundColor DarkCyan
Write-Host ""
