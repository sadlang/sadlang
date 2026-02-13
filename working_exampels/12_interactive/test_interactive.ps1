# Set UTF-8 encoding for proper Arabic display
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$PSDefaultParameterValues['*:Encoding'] = 'utf8'
chcp 65001 | Out-Null

Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "Testing Interactive Example" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

$sadPath = "c:\s\s_language\build\bin\Release\sad.exe"
$examplePath = "c:\s\s_language\working_exampels\12_interactive\01_greeting.sad"

if (Test-Path $sadPath) {
    Write-Host "Running example with interpreter..." -ForegroundColor Green
    Write-Host ""
    
    # Run the program
    & $sadPath $examplePath
    
    Write-Host ""
    Write-Host "=====================================" -ForegroundColor Cyan
} else {
    Write-Host "ERROR: Interpreter not found!" -ForegroundColor Red
}
