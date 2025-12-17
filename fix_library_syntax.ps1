# سكريبت تصحيح نحو لغة ص في ملفات المكتبات
# Script to fix Sad Language syntax in library files

Write-Host "=== بدء تصحيح نحو لغة ص ===" -ForegroundColor Cyan

# الملفات المستهدفة
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
    $fullPath = Join-Path $PSScriptRoot $file
    
    if (!(Test-Path $fullPath)) {
        Write-Host "✗ الملف غير موجود: $file" -ForegroundColor Red
        continue
    }
    
    Write-Host "`n📝 معالجة: $file" -ForegroundColor Yellow
    
    # قراءة المحتوى
    $content = Get-Content $fullPath -Raw -Encoding UTF8
    $originalContent = $content
    $fileEdits = 0
    
    # النمط 1: استبدال دالة(معاملات) { ... }
    $pattern1 = 'دالة\s*([^\(]*)\(([^\)]*)\)\s*\{'
    $replacement1 = 'دالة $1($2)'
    if ($content -match $pattern1) {
        $content = $content -replace $pattern1, $replacement1
        $fileEdits++
    }
    
    # النمط 2: استبدال صنف اسم { ... }
    $pattern2 = 'صنف\s+(\S+)\s+يمتد\s+(\S+)\s*\{'
    $replacement2 = 'صنف $1 يمتد $2'
    if ($content -match $pattern2) {
        $content = $content -replace $pattern2, $replacement2
        $fileEdits++
    }
    
    # النمط 3: صنف بدون امتداد
    $pattern3 = 'صنف\s+(\S+)\s*\{'
    $replacement3 = 'صنف $1'
    if ($content -match $pattern3) {
        $content = $content -replace $pattern3, $replacement3
        $fileEdits++
    }
    
    # النمط 4: استبدال } وإلا {
    $pattern4 = '\}\s*وإلا\s*\{'
    $replacement4 = 'وإلا'
    if ($content -match $pattern4) {
        $content = $content -replace $pattern4, $replacement4
        $fileEdits++
    }
    
    # النمط 5: استبدال إذا (شرط) {
    $pattern5 = 'إذا\s*\(([^\)]+)\)\s*\{'
    $replacement5 = 'إذا ($1)'
    if ($content -match $pattern5) {
        $content = $content -replace $pattern5, $replacement5
        $fileEdits++
    }
    
    # النمط 6: استبدال لكل ... في ... {
    $pattern6 = 'لكل\s+(\S+)\s+في\s+([^\{]+)\s*\{'
    $replacement6 = 'لكل $1 في $2'
    if ($content -match $pattern6) {
        $content = $content -replace $pattern6, $replacement6
        $fileEdits++
    }
    
    # النمط 7: استبدال }) في نهاية الأسطر
    $content = $content -replace '\}\)', 'نهاية)'
    $fileEdits++
    
    # النمط 8: استبدال } منفردة
    $content = $content -replace '(?m)^\s*\}\s*$', 'نهاية'
    $fileEdits++
    
    # النمط 9: استبدال // بـ #
    $content = $content -replace '(?m)^(\s*)//\s*', '$1# '
    $fileEdits++
    
    # النمط 10: استبدال أرجع بـ إرجاع
    $content = $content -replace '\bأرجع\b', 'إرجاع'
    $fileEdits++
    
    # حفظ إذا تم التعديل
    if ($content -ne $originalContent) {
        Set-Content $fullPath -Value $content -Encoding UTF8 -NoNewline
        Write-Host "  ✓ تم التعديل ($fileEdits أنماط)" -ForegroundColor Green
        $totalEdits += $fileEdits
    } else {
        Write-Host "  - لا توجد تعديلات" -ForegroundColor Gray
    }
}

Write-Host "`n=== اكتمل التصحيح ===" -ForegroundColor Cyan
Write-Host "إجمالي الملفات المعالجة: $($files.Count)" -ForegroundColor White
Write-Host "إجمالي الأنماط المصححة: $totalEdits" -ForegroundColor Green
