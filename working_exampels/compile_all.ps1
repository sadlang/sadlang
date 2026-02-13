# Compile all .sad files to exe
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "Sad Language Compiler - Batch Compile" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

$compilerPath = "C:\s\s_language\build\bin\Release\sadc.exe"
$clangPath = "C:\LLVM\bin\clang.exe"

if (-not (Test-Path $compilerPath)) {
    Write-Host "ERROR: Compiler not found" -ForegroundColor Red
    exit 1
}

$clangAvailable = Test-Path $clangPath

$sadFiles = Get-ChildItem -Path "C:\s\s_language\working_exampels" -Filter "*.sad" -Recurse
$totalFiles = $sadFiles.Count
$successCount = 0
$failCount = 0
$llvmCount = 0
$exeCount = 0

Write-Host "Found $totalFiles .sad files" -ForegroundColor Green
Write-Host ""

foreach ($file in $sadFiles) {
    $fileName = $file.Name
    $fileDir = $file.DirectoryName
    $baseName = $file.BaseName
    
    Write-Host "[$($successCount + $failCount + 1)/$totalFiles] $fileName" -ForegroundColor Cyan
    
    $llFile = Join-Path $fileDir "$baseName.ll"
    $exeFile = Join-Path $fileDir "$baseName.exe"
    
    try {
        $output = & $compilerPath $file.FullName --emit-llvm -o $llFile 2>&1 | Out-String
        
        if ($LASTEXITCODE -eq 0 -and (Test-Path $llFile)) {
            Write-Host "  [OK] LLVM IR generated" -ForegroundColor Green
            $llvmCount++
            
            if ($clangAvailable) {
                try {
                    $clangOutput = & $clangPath -o $exeFile $llFile 2>&1 | Out-String
                    
                    if ($LASTEXITCODE -eq 0 -and (Test-Path $exeFile)) {
                        Write-Host "  [OK] EXE generated: $baseName.exe" -ForegroundColor Green
                        $exeCount++
                        $successCount++
                    } else {
                        Write-Host "  [WARN] EXE generation failed" -ForegroundColor Yellow
                        $successCount++
                    }
                } catch {
                    Write-Host "  [WARN] Clang error" -ForegroundColor Yellow
                    $successCount++
                }
            } else {
                $successCount++
            }
        } else {
            Write-Host "  [FAIL] Compilation failed" -ForegroundColor Red
            $failCount++
        }
    } catch {
        Write-Host "  [ERROR] Exception occurred" -ForegroundColor Red
        $failCount++
    }
    
    Write-Host ""
}

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "Results: Success=$successCount Failed=$failCount" -ForegroundColor White
Write-Host "LLVM IR=$llvmCount EXE=$exeCount" -ForegroundColor White
Write-Host "=====================================" -ForegroundColor Cyan
