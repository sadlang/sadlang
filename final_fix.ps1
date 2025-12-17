# Final comprehensive syntax fix
Write-Host "Final syntax corrections..." -ForegroundColor Cyan

$files = Get-ChildItem "docs\lib_plan" -Recurse -Filter "*.md" | Where-Object { $_.Name -ne "README.md" -and $_.Name -ne "SYNTAX_CORRECTION_REPORT.md" }

$totalFixed = 0

foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw -Encoding UTF8
    $original = $content
    
    # Fix all remaining "arja3" to "irja3"
    $count = ([regex]::Matches($content, 'أرجع')).Count
    if ($count -gt 0) {
        $content = $content -replace 'أرجع', 'إرجاع'
        Write-Host "$($file.Name): Fixed $count instances of arja3" -ForegroundColor Yellow
        $totalFixed += $count
    }
    
    # Save if changed
    if ($content -ne $original) {
        Set-Content $file.FullName -Value $content -Encoding UTF8 -NoNewline
    }
}

Write-Host "`nTotal corrections: $totalFixed" -ForegroundColor Green
