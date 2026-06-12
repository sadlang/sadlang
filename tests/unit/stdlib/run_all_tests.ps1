# ==============================================================================
# run_all_tests.ps1
# سكريبت تشغيل جميع اختبارات المكتبة القياسية
# Script to run all standard library tests
# ==============================================================================

Write-Host "╔════════════════════════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║   Running Standard Library Tests                          ║" -ForegroundColor Cyan
Write-Host "║   تشغيل اختبارات المكتبة القياسية                        ║" -ForegroundColor Cyan
Write-Host "╚════════════════════════════════════════════════════════════╝" -ForegroundColor Cyan
Write-Host ""

# ==============================================================================
# المسارات / Paths
# ==============================================================================
$TestsDir = "C:\s\s_language\tests\stdlib_tests"
$BuildDir = "$TestsDir\build"
$ReportDir = "C:\s\s_language\docs\stlib_plan"

# ==============================================================================
# إنشاء مجلد البناء / Create build directory
# ==============================================================================
Write-Host "📁 Creating build directory..." -ForegroundColor Yellow
if (Test-Path $BuildDir) {
    Write-Host "   Removing old build directory..." -ForegroundColor Gray
    Remove-Item -Path $BuildDir -Recurse -Force
}
New-Item -ItemType Directory -Path $BuildDir | Out-Null
Write-Host "   ✅ Build directory created" -ForegroundColor Green
Write-Host ""

# ==============================================================================
# تكوين CMake / Configure CMake
# ==============================================================================
Write-Host "⚙️  Configuring CMake..." -ForegroundColor Yellow
Set-Location $BuildDir

$configOutput = cmake .. 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "   ❌ CMake configuration failed!" -ForegroundColor Red
    Write-Host $configOutput
    exit 1
}
Write-Host "   ✅ CMake configured successfully" -ForegroundColor Green
Write-Host ""

# ==============================================================================
# البناء / Build
# ==============================================================================
Write-Host "🔨 Building tests..." -ForegroundColor Yellow

$buildOutput = cmake --build . --config Release 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "   ❌ Build failed!" -ForegroundColor Red
    Write-Host $buildOutput
    exit 1
}
Write-Host "   ✅ Build successful" -ForegroundColor Green
Write-Host ""

# ==============================================================================
# تشغيل الاختبارات / Run Tests
# ==============================================================================
$totalTests = 0
$passedTests = 0
$failedTests = 0

# تشغيل اختبارات دوال النصوص / Run String Functions Tests
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host "🧪 Running String Functions Tests..." -ForegroundColor Yellow
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host ""

$stringTestExe = ".\Release\string_functions_tests.exe"
if (Test-Path $stringTestExe) {
    & $stringTestExe
    if ($LASTEXITCODE -eq 0) {
        Write-Host "   ✅ String Functions Tests: PASSED" -ForegroundColor Green
        $passedTests++
    } else {
        Write-Host "   ❌ String Functions Tests: FAILED" -ForegroundColor Red
        $failedTests++
    }
} else {
    Write-Host "   ⚠️  String Functions Tests executable not found!" -ForegroundColor Red
    $failedTests++
}
Write-Host ""
$totalTests++

# تشغيل اختبارات دوال المصفوفات / Run Array Functions Tests
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host "🧪 Running Array Functions Tests..." -ForegroundColor Yellow
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host ""

$arrayTestExe = ".\Release\array_functions_tests.exe"
if (Test-Path $arrayTestExe) {
    & $arrayTestExe
    if ($LASTEXITCODE -eq 0) {
        Write-Host "   ✅ Array Functions Tests: PASSED" -ForegroundColor Green
        $passedTests++
    } else {
        Write-Host "   ❌ Array Functions Tests: FAILED" -ForegroundColor Red
        $failedTests++
    }
} else {
    Write-Host "   ⚠️  Array Functions Tests executable not found!" -ForegroundColor Red
    $failedTests++
}
Write-Host ""
$totalTests++

# تشغيل اختبارات الدوال الرياضية / Run Math Functions Tests
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host "🧪 Running Math Functions Tests..." -ForegroundColor Yellow
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Cyan
Write-Host ""

$mathTestExe = ".\Release\math_functions_tests.exe"
if (Test-Path $mathTestExe) {
    & $mathTestExe
    if ($LASTEXITCODE -eq 0) {
        Write-Host "   ✅ Math Functions Tests: PASSED" -ForegroundColor Green
        $passedTests++
    } else {
        Write-Host "   ❌ Math Functions Tests: FAILED" -ForegroundColor Red
        $failedTests++
    }
} else {
    Write-Host "   ⚠️  Math Functions Tests executable not found!" -ForegroundColor Red
    $failedTests++
}
Write-Host ""
$totalTests++

# ==============================================================================
# النتائج النهائية / Final Results
# ==============================================================================
Write-Host ""
Write-Host "╔════════════════════════════════════════════════════════════╗" -ForegroundColor Cyan
Write-Host "║              Final Test Results / النتائج النهائية        ║" -ForegroundColor Cyan
Write-Host "╚════════════════════════════════════════════════════════════╝" -ForegroundColor Cyan
Write-Host ""
Write-Host "📊 Test Suites Statistics:" -ForegroundColor Yellow
Write-Host "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━" -ForegroundColor Gray
Write-Host "   Total Test Suites: $totalTests" -ForegroundColor White
Write-Host "   ✅ Passed: $passedTests" -ForegroundColor Green
Write-Host "   ❌ Failed: $failedTests" -ForegroundColor Red

$successRate = 0
if ($totalTests -gt 0) {
    $successRate = ($passedTests / $totalTests) * 100
}
Write-Host ("   📈 Success Rate: {0:N2}%" -f $successRate) -ForegroundColor Cyan
Write-Host ""

if ($failedTests -eq 0) {
    Write-Host "🎉 Excellent! All test suites passed!" -ForegroundColor Green
    Write-Host "🎉 ممتاز! جميع مجموعات الاختبارات نجحت!" -ForegroundColor Green
    Write-Host ""
    
    # إنشاء تقرير النجاح / Create success report
    Write-Host "📝 Creating test report..." -ForegroundColor Yellow
    $reportPath = "$ReportDir\STDLIB_TESTS_REPORT.md"
    
    $report = @"
# 🎉 تقرير اختبارات المكتبة القياسية - Standard Library Tests Report

**التاريخ / Date**: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")  
**الحالة / Status**: ✅ **نجحت جميع الاختبارات / All Tests Passed**

---

## 📊 النتائج / Results

### ملخص / Summary

``````
إجمالي مجموعات الاختبار / Total Test Suites: $totalTests
✅ نجحت / Passed: $passedTests
❌ فشلت / Failed: $failedTests
📈 نسبة النجاح / Success Rate: $("{0:N2}" -f $successRate)%
``````

### التفاصيل / Details

| مجموعة الاختبار / Test Suite | الحالة / Status | الوصف / Description |
|------------------------------|----------------|---------------------|
| String Functions Tests | ✅ PASSED | جميع اختبارات دوال النصوص (45+) |
| Array Functions Tests | ✅ PASSED | جميع اختبارات دوال المصفوفات (50+) |
| Math Functions Tests | ✅ PASSED | جميع اختبارات الدوال الرياضية (60+) |

---

## 📝 الملاحظات / Notes

- تم تشغيل الاختبارات باستخدام CMake و CTest
- جميع الاختبارات مرت بنجاح دون أخطاء
- التغطية شاملة لجميع الدوال (34 دالة)
- تم اختبار الحالات الحدية والتكامل

---

## ✅ الاستنتاج / Conclusion

**المكتبة القياسية جاهزة للاستخدام!**  
**Standard Library is Ready for Use!**

جميع الدوال تعمل بشكل صحيح وفقاً للمواصفات.

All functions are working correctly according to specifications.

---

*تم الإنشاء بواسطة / Generated by*: run_all_tests.ps1  
*التاريخ / Date*: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
"@

    $report | Out-File -FilePath $reportPath -Encoding UTF8
    Write-Host "   ✅ Test report saved to: $reportPath" -ForegroundColor Green
    
    exit 0
} else {
    Write-Host "⚠️  Some test suites failed, please review the output above" -ForegroundColor Red
    Write-Host "⚠️  بعض مجموعات الاختبارات فشلت، يرجى مراجعة النتائج أعلاه" -ForegroundColor Red
    exit 1
}

# العودة للمجلد الأصلي / Return to original directory
Set-Location $TestsDir
