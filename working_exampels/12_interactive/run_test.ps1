# Setup encoding
$OutputEncoding = [console]::InputEncoding = [console]::OutputEncoding = New-Object System.Text.UTF8Encoding
chcp 65001 | Out-Null

Write-Host "=====================================" -ForegroundColor Yellow
Write-Host "Running: 03_add_numbers.sad" -ForegroundColor Yellow
Write-Host "Please enter values when prompted" -ForegroundColor Yellow
Write-Host "=====================================" -ForegroundColor Yellow
Write-Host ""

# Run with test input
$input = @"
15
25
"@

$input | & "c:\s\s_language\build\bin\Release\sad.exe" "c:\s\s_language\working_exampels\12_interactive\03_add_numbers.sad"

Write-Host ""
Write-Host "=====================================" -ForegroundColor Green
Write-Host "Test completed!" -ForegroundColor Green
Write-Host "=====================================" -ForegroundColor Green
