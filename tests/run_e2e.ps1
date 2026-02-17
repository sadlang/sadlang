Param(
    [string]$SadExe = "./build/bin/Debug/sad.exe",
    [string]$InputDir = "./tests/e2e_inputs",
    [string]$ExpectedExt = ".out"
)

Write-Host "Running end-to-end tests..."

if (-not (Test-Path $InputDir)) {
    Write-Host "No e2e inputs found at $InputDir" -ForegroundColor Yellow
    exit 0
}

$failed = @()

Get-ChildItem -Path $InputDir -Filter *.sad -File | ForEach-Object {
    $in = $_.FullName
    $base = [System.IO.Path]::GetFileNameWithoutExtension($in)
    $expected = Join-Path $_.DirectoryName ($base + $ExpectedExt)

    Write-Host "Running: $base.sad"

    $proc = Start-Process -FilePath $SadExe -ArgumentList $in -NoNewWindow -RedirectStandardOutput -Wait -PassThru
    $output = $proc.StandardOutput.ReadToEnd()

    if (Test-Path $expected) {
        $exp = Get-Content $expected -Raw
        if ($exp -ne $output) {
            Write-Host "❌ Mismatch for $base" -ForegroundColor Red
            $failed += $base
        } else {
            Write-Host "✅ $base" -ForegroundColor Green
        }
    } else {
        Write-Host "⚠️  No expected output for $base; skipping comparison" -ForegroundColor Yellow
    }
}

if ($failed.Count -gt 0) {
    Write-Host "Failures: $($failed -join ', ')" -ForegroundColor Red
    exit 1
}

Write-Host "E2E tests completed" -ForegroundColor Green
exit 0
