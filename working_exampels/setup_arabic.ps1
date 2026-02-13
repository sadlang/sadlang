# ==========================================
# اعداد Terminal لدعم اللغة العربية
# Setup Terminal for Arabic Support
# ==========================================

Write-Host "Setting up Arabic support in terminal..." -ForegroundColor Cyan

# Set UTF-8 encoding
$OutputEncoding = [console]::InputEncoding = [console]::OutputEncoding = New-Object System.Text.UTF8Encoding
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$PSDefaultParameterValues['*:Encoding'] = 'utf8'

# Change code page to UTF-8
chcp 65001 | Out-Null

Write-Host "✓ UTF-8 encoding enabled" -ForegroundColor Green
Write-Host "✓ Code page set to 65001" -ForegroundColor Green
Write-Host ""
Write-Host "Terminal is now ready for Arabic!" -ForegroundColor Green
Write-Host "You can now run Sad programs with Arabic text." -ForegroundColor Cyan
Write-Host ""
