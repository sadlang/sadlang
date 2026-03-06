# build-wasm.ps1
# (AR) سكريبت بناء مفسر لغة ص كـ WebAssembly
# (EN) Script to build Sad interpreter as WebAssembly
#
# يتطلب: Emscripten SDK  https://emscripten.org/docs/getting_started/downloads.html
# Requires: Emscripten SDK

param(
    [string]$EmsdkDir  = "$env:USERPROFILE\emsdk",
    [string]$BuildDir  = "build_wasm",
    [string]$OutputDir = "website\docs\public",
    [switch]$InstallEmsdk,
    [switch]$Release
)

$Root = Split-Path -Parent $PSScriptRoot
Set-Location $Root

$Config = if ($Release) { "Release" } else { "Debug" }

Write-Host "`n=== لغة ص — بناء WebAssembly ===" -ForegroundColor Cyan

# ── التحقق من وجود Emscripten ──────────────────────────────────────────────
$emcc = Get-Command emcc -ErrorAction SilentlyContinue
if (-not $emcc) {
    if ($InstallEmsdk) {
        Write-Host "`nتثبيت Emscripten SDK..." -ForegroundColor Yellow
        git clone https://github.com/emscripten-core/emsdk.git $EmsdkDir
        & "$EmsdkDir\emsdk.bat" install latest
        & "$EmsdkDir\emsdk.bat" activate latest
        & "$EmsdkDir\emsdk_env.ps1"
        $emcc = Get-Command emcc -ErrorAction SilentlyContinue
    }
    if (-not $emcc) {
        Write-Host @"

❌ Emscripten غير موجود!

لتثبيته:
  1. git clone https://github.com/emscripten-core/emsdk.git $EmsdkDir
  2. cd $EmsdkDir
  3. .\emsdk.bat install latest
  4. .\emsdk.bat activate latest
  5. .\emsdk_env.ps1

ثم أعد تشغيل هذا السكريبت:
  .\website\build-wasm.ps1 -InstallEmsdk

أو لتجربة الـ Playground الآن بدون WASM:
  node website\api\server.js

"@ -ForegroundColor Red
        exit 1
    }
}

Write-Host "✅ Emscripten: $(emcc --version | Select-String 'emcc')" -ForegroundColor Green

# ── تهيئة cmake مع Emscripten ─────────────────────────────────────────────
Write-Host "`n[1/3] تهيئة cmake مع emcmake..." -ForegroundColor Cyan
$cmakeArgs = @(
    "-S", ".",
    "-B", $BuildDir,
    "-DCMAKE_BUILD_TYPE=$Config",
    "-DBUILD_WASM=ON",
    "-DBUILD_TESTS=OFF"
)
& emcmake cmake @cmakeArgs
if ($LASTEXITCODE -ne 0) { Write-Host "❌ cmake فشل" -ForegroundColor Red; exit 1 }

# ── البناء ────────────────────────────────────────────────────────────────
Write-Host "`n[2/3] بناء sad_wasm..." -ForegroundColor Cyan
& cmake --build $BuildDir --config $Config --target sad_wasm
if ($LASTEXITCODE -ne 0) { Write-Host "❌ البناء فشل" -ForegroundColor Red; exit 1 }

# ── نسخ الناتج ───────────────────────────────────────────────────────────
Write-Host "`n[3/3] نسخ الملفات إلى $OutputDir..." -ForegroundColor Cyan
New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null

$wasmSrc = Join-Path $BuildDir "sad.wasm"
$jsSrc   = Join-Path $BuildDir "sad.js"

if (Test-Path $wasmSrc) {
    Copy-Item $wasmSrc "$OutputDir\sad.wasm" -Force
    Copy-Item $jsSrc   "$OutputDir\sad.js"   -Force
    Write-Host "✅ تم النسخ:" -ForegroundColor Green
    $sz = (Get-Item "$OutputDir\sad.wasm").Length / 1KB
    Write-Host "   sad.wasm  $([math]::Round($sz)) KB"
    Write-Host "   sad.js    (glue)"
} else {
    # الاحتياط: ابحث عن الملف في build output
    $found = Get-ChildItem $BuildDir -Recurse -Filter "sad.wasm" | Select-Object -First 1
    if ($found) {
        Copy-Item $found.FullName "$OutputDir\sad.wasm" -Force
        Copy-Item (Join-Path $found.DirectoryName "sad.js") "$OutputDir\sad.js" -Force
        Write-Host "✅ تم النسخ من $($found.DirectoryName)" -ForegroundColor Green
    } else {
        Write-Host "⚠️  لم يُعثر على sad.wasm في $BuildDir" -ForegroundColor Yellow
    }
}

Write-Host "🎉 اكتمل البناء!`nالآن ابنِ الموقع وشغّله:`n  cd website`n  npm run build`n  npm run preview`nستظهر شارة WASM في Playground مما يعني تشغيل الكود في المتصفح مباشرة." -ForegroundColor Green
