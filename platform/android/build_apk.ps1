# ═══════════════════════════════════════════════════════════════════════════════
# build_apk.ps1 — بناء APK لتطبيق ص على أندرويد
# Build APK for Sad Language Android App
# ═══════════════════════════════════════════════════════════════════════════════

param(
    [string]$Abi = "x86_64",
    [switch]$Install,
    [switch]$Clean
)

$ErrorActionPreference = "Stop"
$SDK = "$env:LOCALAPPDATA\Android\Sdk"
$BT = "$SDK\build-tools\36.1.0"
$PLATFORM = "$SDK\platforms\android-34"
$NDK = "$SDK\ndk\29.0.14206865"
$CMAKE_BIN = "$SDK\cmake\3.22.1\bin\cmake.exe"
$NINJA = "$SDK\cmake\3.22.1\bin\ninja.exe"
$ADB = "$SDK\platform-tools\adb.exe"

$ROOT = Split-Path -Parent $PSScriptRoot
$ROOT = Split-Path -Parent $ROOT
$APP_DIR = "$PSScriptRoot\app"
$BUILD_DIR = "$ROOT\build_android\$Abi"
$APK_DIR = "$ROOT\build_android\apk_build"

Write-Host "═══════════════════════════════════════" -ForegroundColor Cyan
Write-Host "  بناء APK لتطبيق ص / Building Sad APK" -ForegroundColor Cyan
Write-Host "  ABI: $Abi" -ForegroundColor Yellow
Write-Host "═══════════════════════════════════════" -ForegroundColor Cyan

# --- تنظيف (اختياري) ---
if ($Clean -and (Test-Path $APK_DIR)) {
    Remove-Item -Recurse -Force $APK_DIR
}

# --- إنشاء المجلدات ---
$dirs = @(
    "$APK_DIR",
    "$APK_DIR\lib\$Abi",
    "$APK_DIR\classes",
    "$APK_DIR\res\values"
)
foreach ($d in $dirs) {
    if (-not (Test-Path $d)) { New-Item -ItemType Directory -Path $d -Force | Out-Null }
}

# ═══════════════════════════════════════════════════════════════════════════════
# الخطوة 1: بناء المكتبة الأصلية (إذا لم تكن موجودة)
# ═══════════════════════════════════════════════════════════════════════════════

$SO_FILE = "$BUILD_DIR\libs\$Abi\libsad_app.so"
if (-not (Test-Path $SO_FILE)) {
    Write-Host "[1/6] بناء libsad_app.so..." -ForegroundColor Yellow
    $TOOLCHAIN = "$NDK\build\cmake\android.toolchain.cmake"
    
    & $CMAKE_BIN -S "$PSScriptRoot" -B $BUILD_DIR -G Ninja `
        -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" `
        -DANDROID_ABI=$Abi `
        -DANDROID_PLATFORM=android-24 `
        -DCMAKE_MAKE_PROGRAM="$NINJA"
    
    & $CMAKE_BIN --build $BUILD_DIR
    
    if (-not (Test-Path $SO_FILE)) {
        Write-Error "فشل بناء libsad_app.so"
        exit 1
    }
} else {
    Write-Host "[1/6] libsad_app.so موجود بالفعل ✓" -ForegroundColor Green
}

# نسخ المكتبة
Copy-Item $SO_FILE "$APK_DIR\lib\$Abi\" -Force
Write-Host "  نسخ .so → lib/$Abi/ ✓" -ForegroundColor Green

# ═══════════════════════════════════════════════════════════════════════════════
# الخطوة 2: تجميع Java
# ═══════════════════════════════════════════════════════════════════════════════

Write-Host "[2/6] تجميع ملفات Java..." -ForegroundColor Yellow

$JAVA_FILES = Get-ChildItem -Path "$APP_DIR\java" -Filter "*.java" -Recurse
$javaArgs = @("-source", "1.8", "-target", "1.8", 
              "-bootclasspath", "$PLATFORM\android.jar",
              "-d", "$APK_DIR\classes")
$javaArgs += $JAVA_FILES.FullName

& javac @javaArgs 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Error "فشل تجميع Java"
    exit 1
}
Write-Host "  Java compiled ✓" -ForegroundColor Green

# ═══════════════════════════════════════════════════════════════════════════════
# الخطوة 3: تحويل إلى DEX
# ═══════════════════════════════════════════════════════════════════════════════

Write-Host "[3/6] تحويل إلى DEX..." -ForegroundColor Yellow

# جمع كل ملفات .class
$classFiles = Get-ChildItem -Path "$APK_DIR\classes" -Filter "*.class" -Recurse
& cmd /c "$BT\d8.bat" --output "$APK_DIR" $classFiles.FullName 2>&1
if (-not (Test-Path "$APK_DIR\classes.dex")) {
    Write-Error "فشل إنشاء classes.dex"
    exit 1
}
Write-Host "  DEX created ✓" -ForegroundColor Green

# ═══════════════════════════════════════════════════════════════════════════════
# الخطوة 4: بناء APK
# ═══════════════════════════════════════════════════════════════════════════════

Write-Host "[4/6] بناء APK..." -ForegroundColor Yellow

$APK_UNALIGNED = "$APK_DIR\sad-unaligned.apk"
$APK_ALIGNED = "$APK_DIR\sad-aligned.apk"
$APK_FINAL = "$APK_DIR\sad-app.apk"

# إنشاء APK أساسي مع الـ manifest
& "$BT\aapt.exe" package -f -M "$APP_DIR\AndroidManifest.xml" `
    -I "$PLATFORM\android.jar" `
    -F $APK_UNALIGNED 2>&1

# إضافة classes.dex
Push-Location $APK_DIR
& "$BT\aapt.exe" add $APK_UNALIGNED classes.dex 2>&1
Pop-Location

# إضافة المكتبات الأصلية
Push-Location $APK_DIR
& "$BT\aapt.exe" add $APK_UNALIGNED "lib/$Abi/libsad_app.so" 2>&1
Pop-Location

Write-Host "  APK packaged ✓" -ForegroundColor Green

# ═══════════════════════════════════════════════════════════════════════════════
# الخطوة 5: المحاذاة
# ═══════════════════════════════════════════════════════════════════════════════

Write-Host "[5/6] محاذاة APK..." -ForegroundColor Yellow

if (Test-Path $APK_ALIGNED) { Remove-Item $APK_ALIGNED }
& "$BT\zipalign.exe" -v 4 $APK_UNALIGNED $APK_ALIGNED 2>&1 | Select-Object -Last 3
Write-Host "  Aligned ✓" -ForegroundColor Green

# ═══════════════════════════════════════════════════════════════════════════════
# الخطوة 6: التوقيع
# ═══════════════════════════════════════════════════════════════════════════════

Write-Host "[6/6] توقيع APK..." -ForegroundColor Yellow

$KEYSTORE = "$APK_DIR\debug.keystore"
if (-not (Test-Path $KEYSTORE)) {
    & keytool -genkeypair -v -keystore $KEYSTORE `
        -storepass android -keypass android `
        -alias androiddebugkey -keyalg RSA -keysize 2048 -validity 10000 `
        -dname "CN=Sad Debug, OU=Sad, O=Sad, L=Unknown, S=Unknown, C=SA" 2>&1 | Out-Null
}

if (Test-Path $APK_FINAL) { Remove-Item $APK_FINAL }
& cmd /c "$BT\apksigner.bat" sign --ks $KEYSTORE --ks-pass pass:android `
    --key-pass pass:android --out $APK_FINAL $APK_ALIGNED 2>&1

if (Test-Path $APK_FINAL) {
    $size = [math]::Round((Get-Item $APK_FINAL).Length / 1MB, 1)
    Write-Host ""
    Write-Host "═══════════════════════════════════════" -ForegroundColor Green
    Write-Host "  ✅ تم بناء APK بنجاح!" -ForegroundColor Green
    Write-Host "  📦 $APK_FINAL" -ForegroundColor Cyan
    Write-Host "  📏 الحجم: ${size} MB" -ForegroundColor Cyan
    Write-Host "═══════════════════════════════════════" -ForegroundColor Green
} else {
    Write-Error "فشل توقيع APK"
    exit 1
}

# ═══════════════════════════════════════════════════════════════════════════════
# التثبيت (اختياري)
# ═══════════════════════════════════════════════════════════════════════════════

if ($Install) {
    Write-Host ""
    Write-Host "تثبيت APK على المحاكي..." -ForegroundColor Yellow
    & $ADB install -r $APK_FINAL
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✅ تم التثبيت! جاري تشغيل التطبيق..." -ForegroundColor Green
        & $ADB shell am start -n "com.sad.app/.MainActivity"
    } else {
        Write-Error "فشل التثبيت"
    }
}
