# بناء شامل لمشروع لغة ص
# Sad Language Full Build Script
# يبني المترجم والمفسر والمكتبة القياسية والأدوات

param(
    [switch]$Clean,
    [switch]$Release,
    [switch]$Tests,
    [switch]$Verbose,
    [string]$Target = "all"
)

$ErrorActionPreference = "Stop"
$ROOT = $PSScriptRoot
$BUILD_DIR = Join-Path $ROOT "build"
$BUILD_TYPE = if ($Release) { "Release" } else { "Debug" }

Write-Host "╔══════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║     بناء مشروع لغة ص - النسخة الشاملة    ║" -ForegroundColor Cyan
Write-Host "╚══════════════════════════════════════════╝" -ForegroundColor Cyan
Write-Host ""
Write-Host "  نوع البناء: $BUILD_TYPE" -ForegroundColor Yellow
Write-Host "  الهدف: $Target" -ForegroundColor Yellow
Write-Host ""

# تنظيف إذا طُلب
if ($Clean) {
    Write-Host "[تنظيف] حذف مجلد البناء..." -ForegroundColor Yellow
    if (Test-Path $BUILD_DIR) {
        Remove-Item -Recurse -Force $BUILD_DIR
    }
}

# إنشاء مجلد البناء
if (-not (Test-Path $BUILD_DIR)) {
    New-Item -ItemType Directory -Path $BUILD_DIR | Out-Null
}

function Build-Component {
    param(
        [string]$Name,
        [string]$SourceDir,
        [string]$BuildSubDir
    )
    
    $componentBuild = Join-Path $BUILD_DIR $BuildSubDir
    
    Write-Host "`n[$Name] بدء البناء..." -ForegroundColor Green
    
    if (-not (Test-Path $componentBuild)) {
        New-Item -ItemType Directory -Path $componentBuild | Out-Null
    }
    
    Push-Location $componentBuild
    try {
        # CMake configure
        $cmakeArgs = @(
            "-S", $SourceDir,
            "-B", ".",
            "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
        )
        
        if ($Verbose) {
            $cmakeArgs += "-DCMAKE_VERBOSE_MAKEFILE=ON"
        }
        
        Write-Host "  [CMake] تهيئة..." -ForegroundColor DarkGray
        & cmake @cmakeArgs 2>&1 | ForEach-Object {
            if ($Verbose) { Write-Host "    $_" -ForegroundColor DarkGray }
        }
        
        if ($LASTEXITCODE -ne 0) {
            Write-Host "  [خطأ] فشل تهيئة CMake لـ $Name" -ForegroundColor Red
            return $false
        }
        
        # CMake build
        $buildArgs = @("--build", ".", "--config", $BUILD_TYPE, "-j", [Environment]::ProcessorCount)
        
        Write-Host "  [بناء] ترجمة..." -ForegroundColor DarkGray
        & cmake @buildArgs 2>&1 | ForEach-Object {
            if ($Verbose -or $_ -match "error|خطأ") { 
                Write-Host "    $_" -ForegroundColor $(if ($_ -match "error|خطأ") { "Red" } else { "DarkGray" })
            }
        }
        
        if ($LASTEXITCODE -ne 0) {
            Write-Host "  [خطأ] فشل بناء $Name" -ForegroundColor Red
            return $false
        }
        
        Write-Host "  [نجاح] تم بناء $Name بنجاح ✓" -ForegroundColor Green
        return $true
    }
    finally {
        Pop-Location
    }
}

$results = @{}
$startTime = Get-Date

# بناء المكونات حسب الهدف
if ($Target -eq "all" -or $Target -eq "shared") {
    $results["المكتبة المشتركة"] = Build-Component -Name "المكتبة المشتركة (shared)" `
        -SourceDir (Join-Path $ROOT "shared") `
        -BuildSubDir "shared"
}

if ($Target -eq "all" -or $Target -eq "compiler") {
    $results["المترجم"] = Build-Component -Name "المترجم (compiler_new)" `
        -SourceDir (Join-Path $ROOT "compiler_new") `
        -BuildSubDir "compiler_new"
}

if ($Target -eq "all" -or $Target -eq "interpreter") {
    $results["المفسر"] = Build-Component -Name "المفسر (interpreter_new)" `
        -SourceDir (Join-Path $ROOT "interpreter_new") `
        -BuildSubDir "interpreter_new"
}

# تشغيل الاختبارات إذا طُلب
if ($Tests) {
    Write-Host "`n[اختبارات] تشغيل مجموعة الاختبارات..." -ForegroundColor Magenta
    
    $testBuild = Join-Path $BUILD_DIR "tests"
    if (Test-Path (Join-Path $ROOT "tests/CMakeLists.txt")) {
        Build-Component -Name "الاختبارات" -SourceDir (Join-Path $ROOT "tests") -BuildSubDir "tests"
        
        Push-Location $testBuild
        & ctest --output-on-failure -C $BUILD_TYPE 2>&1 | ForEach-Object {
            Write-Host "  $_" -ForegroundColor DarkGray
        }
        Pop-Location
    }
}

# ملخص النتائج
$elapsed = (Get-Date) - $startTime

Write-Host "`n╔══════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║              ملخص البناء                  ║" -ForegroundColor Cyan
Write-Host "╚══════════════════════════════════════════╝" -ForegroundColor Cyan

$allSuccess = $true
foreach ($component in $results.Keys) {
    $status = if ($results[$component]) { "✓ نجاح" } else { "✗ فشل"; $allSuccess = $false }
    $color = if ($results[$component]) { "Green" } else { "Red" }
    Write-Host "  $component : $status" -ForegroundColor $color
}

Write-Host "`n  الوقت المستغرق: $($elapsed.TotalSeconds.ToString('F1')) ثانية" -ForegroundColor Yellow

if ($allSuccess) {
    Write-Host "`n  [نجاح] تم بناء جميع المكونات بنجاح! ✓" -ForegroundColor Green
    exit 0
} else {
    Write-Host "`n  [تحذير] فشل بناء بعض المكونات" -ForegroundColor Red
    exit 1
}
