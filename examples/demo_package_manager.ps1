# Using Sad Package Manager - Demo Script

Write-Host "`n=== Sad Package Manager Demo ===" -ForegroundColor Cyan
Write-Host "================================`n" -ForegroundColor Cyan

$SAD_PKG = "C:\s\s_language\build\bin\Debug\sad-pkg.exe"
$PACKAGE_DIR = "C:\s\s_language\examples\pkg_example"
$TEST_PROJECT = "C:\s\s_language\examples\test_math_project"

# Check if sad-pkg exists
if (!(Test-Path $SAD_PKG)) {
    Write-Host "ERROR: sad-pkg not found. Build the project first." -ForegroundColor Red
    Write-Host "Run: cmake --build build --config Debug" -ForegroundColor Yellow
    exit 1
}

Write-Host "Found: sad-pkg.exe`n" -ForegroundColor Green

# =========================
# Scenario 1: Publish Package
# =========================

Write-Host "Scenario 1: Publishing math-utils package" -ForegroundColor Cyan
Write-Host "==========================================`n" -ForegroundColor Cyan

Set-Location $PACKAGE_DIR

Write-Host "Step 1: Verify sad.toml exists..." -ForegroundColor Yellow
if (Test-Path "sad.toml") {
    Write-Host "OK: sad.toml found`n" -ForegroundColor Green
    Get-Content "sad.toml" -Head 10
    Write-Host "..."
} else {
    Write-Host "ERROR: sad.toml not found`n" -ForegroundColor Red
    exit 1
}

Write-Host "`nStep 2: Publish package..." -ForegroundColor Yellow
Write-Host "Command: sad-pkg publish`n" -ForegroundColor Gray

& $SAD_PKG publish

# =========================
# Scenario 2: Create New Project
# =========================

Write-Host "`n`nScenario 2: Create new project using math-utils" -ForegroundColor Cyan
Write-Host "================================================`n" -ForegroundColor Cyan

# Remove old project
if (Test-Path $TEST_PROJECT) {
    Remove-Item $TEST_PROJECT -Recurse -Force
}

# Create new project
Write-Host "Step 1: Create project directory..." -ForegroundColor Yellow
New-Item -ItemType Directory -Path $TEST_PROJECT -Force | Out-Null
Set-Location $TEST_PROJECT
Write-Host "OK: Created $TEST_PROJECT`n" -ForegroundColor Green

Write-Host "Step 2: Initialize project..." -ForegroundColor Yellow
Write-Host "Command: sad-pkg init`n" -ForegroundColor Gray

& $SAD_PKG init

# Create sad.toml manually if init doesn't work
if (!(Test-Path "sad.toml")) {
    Write-Host "Creating sad.toml manually...`n" -ForegroundColor Yellow
    
@"
[package]
name = "my-calculator"
version = "0.1.0"
description = "Simple calculator using math-utils"
authors = ["Sad Developer <dev@example.com>"]
license = "MIT"

[dependencies]

"@ | Out-File -FilePath "sad.toml" -Encoding UTF8
    
    Write-Host "OK: sad.toml created`n" -ForegroundColor Green
}

Write-Host "Step 3: Add math-utils dependency..." -ForegroundColor Yellow
Write-Host "Command: sad-pkg add math-utils ^1.0.0`n" -ForegroundColor Gray

& $SAD_PKG add "math-utils" "^1.0.0"

Write-Host "`nStep 4: Show sad.toml content..." -ForegroundColor Yellow
if (Test-Path "sad.toml") {
    Get-Content "sad.toml"
}

Write-Host "`nStep 5: Install dependencies..." -ForegroundColor Yellow
Write-Host "Command: sad-pkg install`n" -ForegroundColor Gray

& $SAD_PKG install

# =========================
# Scenario 3: Search Packages
# =========================

Write-Host "`n`nScenario 3: Search for packages" -ForegroundColor Cyan
Write-Host "================================`n" -ForegroundColor Cyan

Write-Host "Step 1: Search for math packages..." -ForegroundColor Yellow
Write-Host "Command: sad-pkg search math`n" -ForegroundColor Gray

& $SAD_PKG search "math"

Write-Host "`nStep 2: Search for http packages..." -ForegroundColor Yellow
Write-Host "Command: sad-pkg search http`n" -ForegroundColor Gray

& $SAD_PKG search "http"

# =========================
# Scenario 4: Package Info
# =========================

Write-Host "`n`nScenario 4: View package information" -ForegroundColor Cyan
Write-Host "=====================================`n" -ForegroundColor Cyan

Write-Host "Step 1: Show math-utils info..." -ForegroundColor Yellow
Write-Host "Command: sad-pkg info math-utils`n" -ForegroundColor Gray

& $SAD_PKG info "math-utils"

# =========================
# Scenario 5: Create Code Using Package
# =========================

Write-Host "`n`nScenario 5: Create program using math-utils" -ForegroundColor Cyan
Write-Host "============================================`n" -ForegroundColor Cyan

Write-Host "Step 1: Create main program file..." -ForegroundColor Yellow

New-Item -ItemType Directory -Path "src" -Force | Out-Null

$programCode = @"
// Math Calculator using math-utils

import math from "math-utils";

function main() {
    print("=== Simple Calculator ===\n");
    
    // Example 1: Power calculation
    print("Example 1: Power");
    var base = 2;
    var exp = 10;
    var result = math.power(base, exp);
    print("   " + base + "^" + exp + " = " + result);
    print("");
    
    // Example 2: Square root
    print("Example 2: Square Root");
    var num = 144;
    var sqrt_result = math.sqrt(num);
    print("   sqrt(" + num + ") = " + sqrt_result);
    print("");
    
    // Example 3: Geometric calculations
    print("Example 3: Circle Area");
    var radius = 7;
    var area = math.circle_area(radius);
    var circumference = math.circle_circumference(radius);
    print("   Radius = " + radius);
    print("   Area = " + area);
    print("   Circumference = " + circumference);
    print("");
    
    // Example 4: Statistics
    print("Example 4: Statistics");
    var numbers = [85, 90, 78, 92, 88];
    var avg = math.average(numbers);
    var min_score = math.min(numbers);
    var max_score = math.max(numbers);
    print("   Scores: " + numbers);
    print("   Average: " + avg);
    print("   Min: " + min_score);
    print("   Max: " + max_score);
    print("");
    
    print("Done!");
}

main();
"@

$programCode | Out-File -FilePath "src\main.sad" -Encoding UTF8

Write-Host "OK: Created src\main.sad`n" -ForegroundColor Green

Write-Host "Step 2: Show program content..." -ForegroundColor Yellow
Get-Content "src\main.sad" -Head 20
Write-Host "... (more in file)`n"

# =========================
# Scenario 6: Manage Dependencies
# =========================

Write-Host "`nScenario 6: Manage dependencies" -ForegroundColor Cyan
Write-Host "================================`n" -ForegroundColor Cyan

Write-Host "Step 1: List installed dependencies..." -ForegroundColor Yellow
Write-Host "Command: sad-pkg list`n" -ForegroundColor Gray

& $SAD_PKG list

Write-Host "`nStep 2: Check for updates..." -ForegroundColor Yellow
Write-Host "Command: sad-pkg outdated`n" -ForegroundColor Gray

& $SAD_PKG outdated

# =========================
# Summary
# =========================

Write-Host "`n`n================================" -ForegroundColor Cyan
Write-Host "Summary of Actions" -ForegroundColor Yellow
Write-Host "================================`n" -ForegroundColor Cyan

Write-Host "Created package: math-utils" -ForegroundColor Green
Write-Host "  Location: $PACKAGE_DIR"
Write-Host "  Archive: C:\s\s_language\examples\math-utils-1.0.0.tar.gz"
Write-Host ""

Write-Host "Created test project: my-calculator" -ForegroundColor Green
Write-Host "  Location: $TEST_PROJECT"
Write-Host "  Main file: src\main.sad"
Write-Host ""

Write-Host "Available Commands:" -ForegroundColor Yellow
Write-Host "  sad-pkg init           - Initialize project"
Write-Host "  sad-pkg add <pkg>      - Add dependency"
Write-Host "  sad-pkg install        - Install dependencies"
Write-Host "  sad-pkg search <query> - Search packages"
Write-Host "  sad-pkg info <pkg>     - Show package info"
Write-Host "  sad-pkg publish        - Publish package"
Write-Host "  sad-pkg list           - List dependencies"
Write-Host "  sad-pkg update         - Update dependencies"
Write-Host ""

Write-Host "Next Steps:" -ForegroundColor Cyan
Write-Host "  1. Start registry server (if available)"
Write-Host "  2. Publish: cd $PACKAGE_DIR; sad-pkg publish"
Write-Host "  3. Use package in other projects"
Write-Host ""

Write-Host "================================" -ForegroundColor Cyan
Write-Host "Done!" -ForegroundColor Green
Write-Host "================================`n" -ForegroundColor Cyan
