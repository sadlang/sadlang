# Setup encoding
$OutputEncoding = [console]::InputEncoding = [console]::OutputEncoding = New-Object System.Text.UTF8Encoding
chcp 65001 | Out-Null

Write-Host "Testing: 04_simple_add.sad" -ForegroundColor Cyan
Write-Host ""

$input = @"
15
25
"@

$input | & "c:\s\s_language\build\bin\Release\sad.exe" "c:\s\s_language\working_exampels\12_interactive\04_simple_add.sad"

Write-Host ""
Write-Host "Test completed!" -ForegroundColor Green
