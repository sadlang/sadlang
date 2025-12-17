# Fix Sad Language Syntax in Library Files
# Auto-correct syntax patterns to match Sad language rules

Write-Host "Starting syntax correction..." -ForegroundColor Cyan

$files = @(
    "docs\lib_plan\web\2_sad_universal_platform.md",
    "docs\lib_plan\web\3_sad_graphql_gateway.md",
    "docs\lib_plan\ai\1_sad_ai_framework.md",
    "docs\lib_plan\ai\2_sad_arabic_nlp.md",
    "docs\lib_plan\ai\3_sad_vision_multimedia.md",
    "docs\lib_plan\desktop\1_sad_gui_framework.md",
    "docs\lib_plan\desktop\2_sad_game_engine.md",
    "docs\lib_plan\desktop\3_sad_graphics_multimedia.md"
)

$totalEdits = 0

foreach ($file in $files) {
    Write-Host "`nProcessing: $file" -ForegroundColor Yellow
    
    if (!(Test-Path $file)) {
        Write-Host "File not found!" -ForegroundColor Red
        continue
    }
    
    $content = Get-Content $file -Raw -Encoding UTF8
    $original = $content
    
    # Replace closing braces with "nihaya"
    $content = $content -replace '\}\)', 'نهاية)'
    $content = $content -replace '(?m)^\s*\}\s*$', 'نهاية'
    
    # Replace // comments with #
    $content = $content -replace '(?m)^(\s*)//\s*', '$1# '
    
    # Replace "arja3" with "irja3"
    $content = $content -replace 'أرجع\b', 'إرجاع'
    
    if ($content -ne $original) {
        Set-Content $file -Value $content -Encoding UTF8 -NoNewline
        Write-Host "  EDITED" -ForegroundColor Green
        $totalEdits++
    } else {
        Write-Host "  No changes" -ForegroundColor Gray
    }
}

Write-Host "`nTotal files edited: $totalEdits" -ForegroundColor Green
Write-Host "Done!" -ForegroundColor Cyan
