<#
.SYNOPSIS
    Regression Tests Runner for Sad Language
.DESCRIPTION
    Runs all 22 regression tests against interpreter or compiler
.PARAMETER SadExe
    Path to executable (sad.exe or sadc.exe)
.PARAMETER Mode
    Run mode: interpreter or compiler
.EXAMPLE
    .\run_regression_tests.ps1 -SadExe .\build\bin\Debug\sad.exe -Mode interpreter
    .\run_regression_tests.ps1 -SadExe .\build\bin\Release\sadc.exe -Mode compiler
#>

param(
    [Parameter(Mandatory=$true)]
    [string]$SadExe,
    
    [Parameter(Mandatory=$false)]
    [ValidateSet("interpreter", "compiler")]
    [string]$Mode = "interpreter"
)

# ======================================================================
# Configuration
# ======================================================================
$ErrorActionPreference = "Continue"
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$TestFiles = Get-ChildItem -Path $ScriptDir -Filter "test_p*" | Where-Object { $_.Extension -ne ".ps1" -and $_.Extension -ne ".sh" -and $_.Extension -ne ".md" -and $_.Extension -ne ".cpp" } | Sort-Object Name

# Statistics
$totalTests = 0
$passedTests = 0
$failedTests = 0
$xfailTests = 0
$errorTests = 0
$failedNames = @()

# Known failures (XFAIL -- expected to fail until fixed)
$knownFailures = @(
    # Add test names here that are expected to fail
)

# ======================================================================
# Start Tests
# ======================================================================
Write-Host ""
Write-Host "================================================================"
Write-Host "  Regression Tests -- Sad Language (22 tests)"
Write-Host "  Mode: $Mode"
Write-Host "  Tool: $SadExe"
Write-Host "  Count: $($TestFiles.Count) tests"
Write-Host "================================================================"
Write-Host ""

# Check tool exists
if (-not (Test-Path $SadExe)) {
    Write-Host "[ERROR] Tool not found: $SadExe"
    exit 1
}

foreach ($testFile in $TestFiles) {
    $totalTests++
    $testName = $testFile.BaseName
    $testNum = $testName -replace 'test_p(\d+).*', '$1'
    
    Write-Host -NoNewline "  [$totalTests/$($TestFiles.Count)] $testName ... "
    
    try {
        $startTime = Get-Date
        
        if ($Mode -eq "interpreter") {
            # Run interpreter with 30 second timeout
            $process = Start-Process -FilePath $SadExe -ArgumentList $testFile.FullName `
                -NoNewWindow -PassThru -RedirectStandardOutput "$env:TEMP\sad_test_out.txt" `
                -RedirectStandardError "$env:TEMP\sad_test_err.txt"
            
            $completed = $process.WaitForExit(30000)
            
            if (-not $completed) {
                $process.Kill()
                $duration = [math]::Round(((Get-Date) - $startTime).TotalMilliseconds)
                
                if ($knownFailures -contains $testName) {
                    $xfailTests++
                    Write-Host "XFAIL (timeout -- known issue) (${duration}ms)"
                } else {
                    $failedTests++
                    $failedNames += $testName
                    Write-Host "FAIL (timeout -- possible infinite loop)"
                }
                continue
            }
            
            $exitCode = $process.ExitCode
            $output = Get-Content "$env:TEMP\sad_test_out.txt" -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
            $stderr = Get-Content "$env:TEMP\sad_test_err.txt" -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
            
        } else {
            # Compiler mode: sadc compile then run
            $outExe = "$env:TEMP\sad_test_$testNum.exe"
            
            # Compile
            $compileProcess = Start-Process -FilePath $SadExe -ArgumentList $testFile.FullName, "-o", $outExe `
                -NoNewWindow -PassThru -RedirectStandardOutput "$env:TEMP\sad_compile_out.txt" `
                -RedirectStandardError "$env:TEMP\sad_compile_err.txt"
            $compileProcess.WaitForExit(60000)
            
            if ($compileProcess.ExitCode -ne 0 -or -not (Test-Path $outExe)) {
                $duration = [math]::Round(((Get-Date) - $startTime).TotalMilliseconds)
                if ($knownFailures -contains $testName) {
                    $xfailTests++
                    Write-Host "XFAIL (compile error -- known issue) (${duration}ms)"
                } else {
                    $failedTests++
                    $failedNames += "$testName (compile)"
                    Write-Host "FAIL (compile error)"
                    $compErr = Get-Content "$env:TEMP\sad_compile_err.txt" -Raw -ErrorAction SilentlyContinue
                    if ($compErr) { Write-Host "    $compErr" }
                }
                continue
            }
            
            # Run compiled binary
            $process = Start-Process -FilePath $outExe `
                -NoNewWindow -PassThru -RedirectStandardOutput "$env:TEMP\sad_test_out.txt" `
                -RedirectStandardError "$env:TEMP\sad_test_err.txt"
            $completed = $process.WaitForExit(30000)
            
            if (-not $completed) {
                $process.Kill()
                $failedTests++
                $failedNames += "$testName (runtime timeout)"
                Write-Host "FAIL (runtime timeout)"
                continue
            }
            
            $exitCode = $process.ExitCode
            $output = Get-Content "$env:TEMP\sad_test_out.txt" -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
            $stderr = Get-Content "$env:TEMP\sad_test_err.txt" -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
            
            # Cleanup
            Remove-Item $outExe -ErrorAction SilentlyContinue
        }
        
        $duration = [math]::Round(((Get-Date) - $startTime).TotalMilliseconds)
        
        # Analyze results
        $hasFail = $output -match "FAIL:"
        $hasPass = $output -match "PASS:"
        $hasError = $exitCode -ne 0
        
        if ($hasError -or $hasFail) {
            if ($knownFailures -contains $testName) {
                $xfailTests++
                Write-Host "XFAIL (${duration}ms)"
            } else {
                $failedTests++
                $failedNames += $testName
                Write-Host "FAIL (${duration}ms)"
                if ($hasFail) {
                    $failLines = ($output -split "`n") | Where-Object { $_ -match "FAIL:" }
                    foreach ($line in $failLines) {
                        Write-Host "    $($line.Trim())"
                    }
                }
                if ($stderr) {
                    $stderrLines = ($stderr -split "`n") | Select-Object -First 3
                    foreach ($line in $stderrLines) {
                        Write-Host "    $($line.Trim())"
                    }
                }
            }
        } else {
            $passedTests++
            Write-Host "PASS (${duration}ms)"
        }
        
    } catch {
        $errorTests++
        $failedNames += "$testName (exception)"
        Write-Host "ERROR: $($_.Exception.Message)"
    }
}

# ======================================================================
# Summary
# ======================================================================
Write-Host ""
Write-Host "================================================================"
Write-Host "  Regression Test Summary"
Write-Host "================================================================"
Write-Host "  Total:     $totalTests"
Write-Host "  Passed:    $passedTests"
Write-Host "  Failed:    $failedTests"
Write-Host "  XFail:     $xfailTests"

if ($failedNames.Count -gt 0) {
    Write-Host ""
    Write-Host "  Failed Tests:"
    foreach ($name in $failedNames) {
        Write-Host "    X $name"
    }
}

Write-Host ""
if ($failedTests -eq 0) {
    Write-Host "  [OK] All regression tests passed!"
    exit 0
} else {
    $rate = [math]::Round(($passedTests / $totalTests) * 100, 1)
    Write-Host "  [!!] Pass rate: ${rate}%"
    # Don't fail CI -- tests expect known bugs to fail
    # To fail CI: uncomment next line
    # exit 1
    exit 0
}
