# OOP Tests Runner Script
param(
    [string]$TestPattern = "test_*.s",
    [switch]$Verbose,
    [switch]$StopOnError
)

$TestsPath = "c:\s\s_language\tests\oop"
$SadExecutable = "c:\s\s_language\build\bin\Debug\sad.exe"

Write-Host "`n========================================"
Write-Host "   OOP Tests Runner"
Write-Host "========================================`n"

if (-not (Test-Path $SadExecutable)) {
    Write-Host "ERROR: Interpreter not found" -ForegroundColor Red
    exit 1
}

if (-not (Test-Path $TestsPath)) {
    Write-Host "ERROR: Tests directory not found" -ForegroundColor Red
    exit 1
}

$TestFiles = Get-ChildItem -Path $TestsPath -Filter $TestPattern | Sort-Object Name

if ($TestFiles.Count -eq 0) {
    Write-Host "WARNING: No test files found" -ForegroundColor Yellow
    exit 0
}

Write-Host "Number of tests: $($TestFiles.Count)`n" -ForegroundColor Cyan

$TotalTests = 0
$PassedTests = 0
$FailedTests = 0
$FailedTestsList = @()

foreach ($TestFile in $TestFiles) {
    $TotalTests++
    $TestName = $TestFile.Name
    
    Write-Host "[$TotalTests/$($TestFiles.Count)] $TestName" -ForegroundColor Cyan
    
    $Output = & $SadExecutable $TestFile.FullName 2>&1
    $ExitCode = $LASTEXITCODE
    
    if ($ExitCode -eq 0) {
        Write-Host "   PASSED" -ForegroundColor Green
        $PassedTests++
    } else {
        Write-Host "   FAILED (Exit: $ExitCode)" -ForegroundColor Red
        $FailedTests++
        $FailedTestsList += $TestName
        
        if ($StopOnError) {
            Write-Host "`nStopping on error" -ForegroundColor Red
            break
        }
    }
    
    Write-Host ""
}

Write-Host "`n========================================"
Write-Host "   Summary"
Write-Host "========================================"
Write-Host "Total:  $TotalTests"
Write-Host "Passed: $PassedTests" -ForegroundColor Green
Write-Host "Failed: $FailedTests" -ForegroundColor $(if ($FailedTests -gt 0) { "Red" } else { "Green" })

$Rate = if ($TotalTests -gt 0) { [math]::Round(($PassedTests / $TotalTests) * 100, 2) } else { 0 }
Write-Host "Rate:   $Rate%" -ForegroundColor $(if ($Rate -eq 100) { "Green" } else { "Yellow" })

if ($FailedTests -gt 0) {
    Write-Host "`nFailed:" -ForegroundColor Red
    foreach ($Test in $FailedTestsList) {
        Write-Host "  - $Test" -ForegroundColor Red
    }
}

Write-Host "`n========================================`n"
exit $FailedTests
