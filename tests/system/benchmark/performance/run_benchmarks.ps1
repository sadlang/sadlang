# ===================================================================
# Performance Benchmark: Sad vs Python vs Node.js
# ===================================================================
# Usage:
#   .\run_benchmarks.ps1
#   .\run_benchmarks.ps1 -Iterations 10
#   .\run_benchmarks.ps1 -OutputJson results.json
# ===================================================================

[CmdletBinding()]
param(
    [int]$Iterations = 5,
    [string]$OutputJson = "",
    [switch]$SkipPython,
    [switch]$SkipNode
)

$ErrorActionPreference = "Stop"
$ScriptDir = $PSScriptRoot
$BenchDir = Join-Path $ScriptDir "benchmarks"

# -------------------------------------------------------------------
# Output functions
# -------------------------------------------------------------------
function Write-Banner {
    Write-Host ""
    Write-Host "  ========================================================" -ForegroundColor Cyan
    Write-Host "      Performance Benchmark: Sad vs Python vs Node.js     " -ForegroundColor Cyan
    Write-Host "  ========================================================" -ForegroundColor Cyan
    Write-Host ""
}

function Write-Section { param($m) Write-Host "`n=== $m ===" -ForegroundColor Yellow }
function Write-Test { param($m) Write-Host "  [>>] $m" -ForegroundColor Blue }
function Write-Result { param($lang, $time) Write-Host "      $lang : $time ms" -ForegroundColor Green }
function Write-Err { param($m) Write-Host "      ERROR: $m" -ForegroundColor Red }

# -------------------------------------------------------------------
# Tool detection
# -------------------------------------------------------------------
$script:sadPath = $null
$script:pythonCmd = $null
$script:nodeCmd = $null

function Find-Tools {
    Write-Section "Detecting Tools"
    
    # Sad - build path manually for PS 5.1 compatibility
    $rootDir = Split-Path (Split-Path $ScriptDir -Parent) -Parent
    $p = "$rootDir\build\bin\Debug\sad.exe"
    if (Test-Path $p) {
        $script:sadPath = (Resolve-Path $p).Path
        Write-Host "  [OK] sad.exe: $($script:sadPath)" -ForegroundColor Green
    } else {
        $p = "$rootDir\build\bin\Release\sad.exe"
        if (Test-Path $p) {
            $script:sadPath = (Resolve-Path $p).Path
            Write-Host "  [OK] sad.exe: $($script:sadPath)" -ForegroundColor Green
        } else {
            Write-Host "  [!!] sad.exe: NOT FOUND" -ForegroundColor Red
            return $false
        }
    }
    
    # Python
    if (Get-Command python -ErrorAction SilentlyContinue) {
        $script:pythonCmd = "python"
        $ver = & python --version 2>&1
        Write-Host "  [OK] python: $ver" -ForegroundColor Green
    } elseif (Get-Command python3 -ErrorAction SilentlyContinue) {
        $script:pythonCmd = "python3"
        $ver = & python3 --version 2>&1
        Write-Host "  [OK] python3: $ver" -ForegroundColor Green
    } else {
        Write-Host "  [--] python: NOT FOUND (skipping)" -ForegroundColor Yellow
    }
    
    # Node
    if (Get-Command node -ErrorAction SilentlyContinue) {
        $script:nodeCmd = "node"
        $ver = & node --version 2>&1
        Write-Host "  [OK] node: $ver" -ForegroundColor Green
    } else {
        Write-Host "  [--] node: NOT FOUND (skipping)" -ForegroundColor Yellow
    }
    
    return $true
}

# -------------------------------------------------------------------
# Time measurement
# -------------------------------------------------------------------
function Measure-Execution {
    param(
        [string]$Command,
        [string]$Arg,
        [int]$Iterations = 5
    )
    
    $times = @()
    
    for ($i = 1; $i -le $Iterations; $i++) {
        $sw = [System.Diagnostics.Stopwatch]::StartNew()
        try {
            $null = & $Command $Arg 2>&1
            $sw.Stop()
            $times += $sw.ElapsedMilliseconds
        } catch {
            return @{ success = $false; error = $_.Exception.Message }
        }
    }
    
    $avg = ($times | Measure-Object -Average).Average
    $min = ($times | Measure-Object -Minimum).Minimum
    $max = ($times | Measure-Object -Maximum).Maximum
    
    return @{
        success = $true
        avg = [math]::Round($avg, 2)
        min = $min
        max = $max
        times = $times
    }
}

# -------------------------------------------------------------------
# Results storage
# -------------------------------------------------------------------
$script:results = @()

function Run-Benchmark {
    param(
        [string]$Name,
        [string]$SadFile,
        [string]$PythonFile,
        [string]$NodeFile
    )
    
    Write-Test $Name
    
    $benchResult = @{
        name = $Name
        sad = @{ avg = 0; min = 0; max = 0; error = "" }
        python = @{ avg = 0; min = 0; max = 0; error = "" }
        node = @{ avg = 0; min = 0; max = 0; error = "" }
    }
    
    # Sad
    if ($script:sadPath -and (Test-Path $SadFile)) {
        $res = Measure-Execution -Command $script:sadPath -Arg $SadFile -Iterations $Iterations
        if ($res.success) {
            $benchResult.sad = @{ avg = $res.avg; min = $res.min; max = $res.max }
            Write-Result "Sad   " $res.avg
        } else {
            $benchResult.sad.error = $res.error
            Write-Err "Sad: $($res.error)"
        }
    } else {
        Write-Err "Sad file not found: $SadFile"
    }
    
    # Python
    if ($script:pythonCmd -and !$SkipPython -and (Test-Path $PythonFile)) {
        $res = Measure-Execution -Command $script:pythonCmd -Arg $PythonFile -Iterations $Iterations
        if ($res.success) {
            $benchResult.python = @{ avg = $res.avg; min = $res.min; max = $res.max }
            Write-Result "Python" $res.avg
        } else {
            $benchResult.python.error = $res.error
            Write-Err "Python: $($res.error)"
        }
    }
    
    # Node
    if ($script:nodeCmd -and !$SkipNode -and (Test-Path $NodeFile)) {
        $res = Measure-Execution -Command $script:nodeCmd -Arg $NodeFile -Iterations $Iterations
        if ($res.success) {
            $benchResult.node = @{ avg = $res.avg; min = $res.min; max = $res.max }
            Write-Result "Node  " $res.avg
        } else {
            $benchResult.node.error = $res.error
            Write-Err "Node: $($res.error)"
        }
    }
    
    $script:results += $benchResult
}

# -------------------------------------------------------------------
# Print summary
# -------------------------------------------------------------------
function Print-Summary {
    Write-Section "Results Summary"
    
    $header = "{0,-25} {1,12} {2,12} {3,12}" -f "Test", "Sad (ms)", "Python", "Node.js"
    Write-Host "  $header"
    Write-Host "  $("-" * 61)"
    
    foreach ($r in $script:results) {
        $line = "{0,-25} {1,12} {2,12} {3,12}" -f $r.name, $r.sad.avg, $r.python.avg, $r.node.avg
        Write-Host "  $line"
    }
    
    Write-Host ""
}

function Export-Json {
    param([string]$Path)
    
    $data = @{
        timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
        iterations = $Iterations
        results = $script:results
    }
    
    $data | ConvertTo-Json -Depth 5 | Set-Content $Path -Encoding UTF8
    Write-Host "  Saved results to: $Path" -ForegroundColor Green
}

# -------------------------------------------------------------------
# Main
# -------------------------------------------------------------------
Write-Banner

if (!(Find-Tools)) {
    Write-Host "  FAILED: Required tools not found" -ForegroundColor Red
    exit 1
}

Write-Section "Running Benchmarks ($Iterations iterations)"

# Use .ص extension for Sad files (Arabic extension)
$sadExt = [char]0x0635  # Arabic letter Sad

Run-Benchmark "Fibonacci (25)" `
    (Join-Path $BenchDir "fib.$sadExt") `
    (Join-Path $BenchDir "fib.py") `
    (Join-Path $BenchDir "fib.js")

Run-Benchmark "Loop Sum (100K)" `
    (Join-Path $BenchDir "loop.$sadExt") `
    (Join-Path $BenchDir "loop.py") `
    (Join-Path $BenchDir "loop.js")

Run-Benchmark "Array Operations (10K)" `
    (Join-Path $BenchDir "array.$sadExt") `
    (Join-Path $BenchDir "array.py") `
    (Join-Path $BenchDir "array.js")

Run-Benchmark "String Concat (1K)" `
    (Join-Path $BenchDir "string.$sadExt") `
    (Join-Path $BenchDir "string.py") `
    (Join-Path $BenchDir "string.js")

Run-Benchmark "Math Intensive (50K)" `
    (Join-Path $BenchDir "math.$sadExt") `
    (Join-Path $BenchDir "math.py") `
    (Join-Path $BenchDir "math.js")

Print-Summary

if ($OutputJson) {
    Export-Json -Path $OutputJson
}

Write-Host "  Benchmark complete!" -ForegroundColor Green
Write-Host ""
