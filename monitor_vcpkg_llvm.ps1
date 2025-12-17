# بسم الله الرحمن الرحيم
# سكربت مراقبة تثبيت LLVM عبر vcpkg
# LLVM Installation Monitor Script

param(
    [int]$CheckIntervalMinutes = 5,
    [int]$MaxWaitMinutes = 120
)

Write-Host "`n════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "   بسم الله - مراقب تثبيت LLVM vcpkg" -ForegroundColor Green
Write-Host "════════════════════════════════════════════════`n" -ForegroundColor Cyan

$startTime = Get-Date
$llvmPath = "C:\vcpkg\installed\x64-windows\include\llvm"
$buildPath = "C:\vcpkg\buildtrees\llvm"
$checkCount = 0
$maxChecks = [math]::Ceiling($MaxWaitMinutes / $CheckIntervalMinutes)

Write-Host "📊 الإعدادات:" -ForegroundColor Yellow
Write-Host "  • فحص كل: $CheckIntervalMinutes دقائق" -ForegroundColor Gray
Write-Host "  • الحد الأقصى للانتظار: $MaxWaitMinutes دقيقة" -ForegroundColor Gray
Write-Host "  • عدد الفحوصات المتوقعة: ~$maxChecks فحص`n" -ForegroundColor Gray

while ($checkCount -lt $maxChecks) {
    $checkCount++
    $elapsed = (Get-Date) - $startTime
    $elapsedMin = [math]::Round($elapsed.TotalMinutes, 1)
    
    Write-Host "════════════════════════════════════════════════" -ForegroundColor Cyan
    Write-Host "  الفحص #$checkCount - الوقت المنقضي: $elapsedMin دقيقة" -ForegroundColor Yellow
    Write-Host "════════════════════════════════════════════════" -ForegroundColor Cyan
    
    # 1. فحص إكمال التثبيت
    if (Test-Path $llvmPath) {
        Write-Host "`n✅ الحمد لله! LLVM SDK مُثبت بنجاح!" -ForegroundColor Green
        Write-Host "`n📊 الإحصائيات النهائية:" -ForegroundColor Cyan
        Write-Host "  • الوقت الإجمالي: $elapsedMin دقيقة" -ForegroundColor Gray
        
        $headers = Get-ChildItem $llvmPath -Directory | Select-Object -First 10 Name
        Write-Host "  • ملفات Header متوفرة: $($headers.Count)" -ForegroundColor Gray
        Write-Host "`n📁 بعض الملفات المتوفرة:" -ForegroundColor Yellow
        $headers | ForEach-Object { Write-Host "    - $($_.Name)" -ForegroundColor Gray }
        
        Write-Host "`n🎯 الخطوة التالية:" -ForegroundColor Green
        Write-Host "  تشغيل: .\build_with_vcpkg.ps1" -ForegroundColor Cyan
        Write-Host "`nالحمد لله على إتمام النعمة! 🌟`n" -ForegroundColor Green
        break
    }
    
    # 2. فحص التقدم
    if (Test-Path $buildPath) {
        $files = Get-ChildItem $buildPath -Recurse -File -ErrorAction SilentlyContinue
        $totalSize = ($files | Measure-Object -Property Length -Sum).Sum / 1GB
        $fileCount = ($files | Measure-Object).Count
        
        Write-Host "`n⏳ البناء جارٍ..." -ForegroundColor Yellow
        Write-Host "  📁 عدد الملفات: $fileCount" -ForegroundColor Cyan
        Write-Host "  💾 الحجم: $([math]::Round($totalSize, 2)) GB" -ForegroundColor Cyan
        
        # تقدير النسبة (تقريبي بناءً على 8-10 GB كحد أقصى)
        $estimatedProgress = [math]::Min(100, [math]::Round(($totalSize / 10) * 100, 1))
        Write-Host "  📈 التقدم التقريبي: ~$estimatedProgress%" -ForegroundColor Cyan
    } else {
        Write-Host "`n⏳ لا يزال في مرحلة التحضير..." -ForegroundColor Yellow
    }
    
    # 3. فحص العمليات النشطة
    $activeProcesses = Get-Process -Name "vcpkg","cl","link","cmake","ninja","msbuild" -ErrorAction SilentlyContinue
    if ($activeProcesses) {
        Write-Host "`n✅ العمليات النشطة: $($activeProcesses.Count)" -ForegroundColor Green
        $activeProcesses | Select-Object -First 5 | ForEach-Object {
            $cpu = [math]::Round($_.CPU, 1)
            $mem = [math]::Round($_.WS / 1MB, 1)
            Write-Host "    • $($_.ProcessName): CPU=$($cpu)s, Memory=$($mem)MB" -ForegroundColor Gray
        }
    } else {
        Write-Host "`n⚠️  تحذير: لا توجد عمليات نشطة!" -ForegroundColor Red
        Write-Host "    قد تكون العملية توقفت أو اكتملت" -ForegroundColor Yellow
    }
    
    # 4. آخر الملفات المعدلة
    $recentFiles = Get-ChildItem $buildPath -Recurse -File -ErrorAction SilentlyContinue | 
        Where-Object { $_.LastWriteTime -gt (Get-Date).AddMinutes(-$CheckIntervalMinutes) } |
        Sort-Object LastWriteTime -Descending |
        Select-Object -First 5
    
    if ($recentFiles) {
        Write-Host "`n📝 آخر الملفات المعدلة:" -ForegroundColor Cyan
        $recentFiles | ForEach-Object {
            $time = $_.LastWriteTime.ToString("HH:mm:ss")
            Write-Host "    • $time - $($_.Name)" -ForegroundColor Gray
        }
    }
    
    # 5. الانتظار حتى الفحص القادم
    if ($checkCount -lt $maxChecks) {
        Write-Host "`n⏰ الفحص القادم بعد $CheckIntervalMinutes دقائق..." -ForegroundColor Gray
        Write-Host "   (Ctrl+C للإيقاف)`n" -ForegroundColor DarkGray
        Start-Sleep -Seconds ($CheckIntervalMinutes * 60)
    }
}

# النتيجة النهائية
if (-not (Test-Path $llvmPath)) {
    Write-Host "`n════════════════════════════════════════════════" -ForegroundColor Red
    Write-Host "  ⚠️  انتهى وقت المراقبة" -ForegroundColor Yellow
    Write-Host "════════════════════════════════════════════════" -ForegroundColor Red
    Write-Host "`nLLVM لم يكتمل بعد بعد $MaxWaitMinutes دقيقة" -ForegroundColor Yellow
    Write-Host "قد يحتاج المزيد من الوقت للإكمال.`n" -ForegroundColor Gray
}
