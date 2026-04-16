# Build libsad_app.so for Android ARM architectures
# Usage: .\build_android.ps1 [-ABI arm64-v8a|armeabi-v7a|all]

param(
    [string]$ABI = "all",
    [string]$NDK = "",
    [string]$Platform = "android-24",
    [switch]$Clean,
    [switch]$Verbose
)

function Write-Title {
    param([string]$Text)
    Write-Host ""
    Write-Host "=======================================================" -ForegroundColor Cyan
    Write-Host "  $Text" -ForegroundColor Yellow
    Write-Host "=======================================================" -ForegroundColor Cyan
}

function Write-Step {
    param([string]$Text)
    Write-Host "  -> $Text" -ForegroundColor Green
}

function Write-Err {
    param([string]$Text)
    Write-Host "  X $Text" -ForegroundColor Red
}

Write-Title "Building Sad for Android"

# Find NDK
if (-not $NDK) {
    $sdkPath = "$env:LOCALAPPDATA\Android\Sdk"
    if (Test-Path "$sdkPath\ndk") {
        $ndkVersions = Get-ChildItem "$sdkPath\ndk" -Directory | Sort-Object Name -Descending
        if ($ndkVersions) {
            $NDK = $ndkVersions[0].FullName
        }
    }
}

if (-not $NDK -or -not (Test-Path $NDK)) {
    Write-Err "Android NDK not found"
    Write-Host "  Specify NDK path: -NDK path\to\ndk"
    exit 1
}

$toolchainFile = "$NDK\build\cmake\android.toolchain.cmake"
if (-not (Test-Path $toolchainFile)) {
    Write-Err "Toolchain file not found: $toolchainFile"
    exit 1
}

Write-Step "NDK: $NDK"

# Determine ABIs
$abis = @()
if ($ABI -eq "all") {
    $abis = @("arm64-v8a", "armeabi-v7a")
} else {
    $abis = @($ABI)
}

Write-Step "ABIs: $($abis -join ', ')"
Write-Step "Platform: $Platform"

# Get paths
$rootDir = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$sourceDir = $PSScriptRoot
$buildBaseDir = "$rootDir\build_android"

foreach ($currentABI in $abis) {
    Write-Title "Building $currentABI"
    
    $buildDir = "$buildBaseDir\$currentABI"
    
    if ($Clean -and (Test-Path $buildDir)) {
        Write-Step "Cleaning..."
        Remove-Item -Recurse -Force $buildDir
    }
    
    if (-not (Test-Path $buildDir)) {
        New-Item -ItemType Directory -Path $buildDir -Force | Out-Null
    }
    
    Write-Step "Configuring CMake..."
    
    # Use Android SDK's CMake and Ninja
    $sdkCmake = "$env:LOCALAPPDATA\Android\Sdk\cmake\3.22.1\bin\cmake.exe"
    $sdkNinja = "$env:LOCALAPPDATA\Android\Sdk\cmake\3.22.1\bin\ninja.exe"
    
    if (-not (Test-Path $sdkCmake)) {
        $sdkCmake = "cmake"  # Fallback to system cmake
    }
    if (-not (Test-Path $sdkNinja)) {
        Write-Err "Ninja not found. Install Android SDK CMake component."
        continue
    }
    
    $cmakeArgs = @(
        "-S", $sourceDir,
        "-B", $buildDir,
        "-G", "Ninja",
        "-DCMAKE_TOOLCHAIN_FILE=$toolchainFile",
        "-DANDROID_ABI=$currentABI",
        "-DANDROID_PLATFORM=$Platform",
        "-DCMAKE_BUILD_TYPE=Release",
        "-DCMAKE_MAKE_PROGRAM=$sdkNinja"
    )
    
    if ($Verbose) {
        $cmakeArgs += "-DCMAKE_VERBOSE_MAKEFILE=ON"
    }
    
    & $sdkCmake $cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        Write-Err "CMake configuration failed for $currentABI"
        continue
    }
    
    Write-Step "Building..."
    & $sdkCmake --build $buildDir --config Release
    if ($LASTEXITCODE -ne 0) {
        Write-Err "Build failed for $currentABI"
        continue
    }
    
    $outputLib = "$buildDir\libs\$currentABI\libsad_app.so"
    if (Test-Path $outputLib) {
        $size = (Get-Item $outputLib).Length / 1KB
        Write-Host "  OK libsad_app.so ($currentABI) - $([math]::Round($size, 1)) KB" -ForegroundColor Green
    } else {
        Write-Err "Library not found: $outputLib"
    }
}

Write-Title "Build Summary"

foreach ($currentABI in $abis) {
    $outputLib = "$buildBaseDir\$currentABI\libs\$currentABI\libsad_app.so"
    if (Test-Path $outputLib) {
        Write-Host "  OK $currentABI : $outputLib" -ForegroundColor Green
    } else {
        Write-Host "  X $currentABI : not built" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "To copy libraries to Android project:"
Write-Host "  Copy-Item build_android\*\libs\*\*.so -Destination app\src\main\jniLibs\ -Recurse" -ForegroundColor Gray
Write-Host ""
