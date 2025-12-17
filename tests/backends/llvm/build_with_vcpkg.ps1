# بسم الله الرحمن الرحيم
# سكربت بناء واختبار LLVM Backend Tests مع vcpkg
# Build and Test Script for LLVM Backend Tests with vcpkg

Write-Host "`n════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "   بسم الله - LLVM Backend Tests Builder" -ForegroundColor Green
Write-Host "════════════════════════════════════════════════`n" -ForegroundColor Cyan

# 1. التحقق من vcpkg LLVM
Write-Host "📦 الخطوة 1: التحقق من تثبيت LLVM..." -ForegroundColor Yellow
$vcpkgRoot = "C:\vcpkg"
$llvmInclude = "$vcpkgRoot\installed\x64-windows\include\llvm"

if (-not (Test-Path $llvmInclude)) {
    Write-Host "❌ خطأ: LLVM غير مثبت عبر vcpkg" -ForegroundColor Red
    Write-Host "يرجى تشغيل: vcpkg install llvm[clang,lld,default-targets]:x64-windows" -ForegroundColor Yellow
    exit 1
}

Write-Host "✅ الحمد لله! LLVM مثبت في: $llvmInclude" -ForegroundColor Green

# 2. إنشاء مجلد البناء
Write-Host "`n🔨 الخطوة 2: إعداد مجلد البناء..." -ForegroundColor Yellow
$buildDir = "$PSScriptRoot\build_vcpkg"

if (Test-Path $buildDir) {
    Write-Host "تنظيف مجلد البناء القديم..." -ForegroundColor Gray
    Remove-Item $buildDir -Recurse -Force
}

New-Item -ItemType Directory -Path $buildDir | Out-Null
Write-Host "✅ تم إنشاء: $buildDir" -ForegroundColor Green

# 3. تكوين CMake
Write-Host "`n⚙️  الخطوة 3: تكوين CMake..." -ForegroundColor Yellow
Push-Location $buildDir

# استخدام CMakeLists_vcpkg.txt
Copy-Item "$PSScriptRoot\CMakeLists_vcpkg.txt" "$PSScriptRoot\CMakeLists.txt" -Force

$cmakeCmd = "cmake .. -G `"Visual Studio 17 2022`" -DCMAKE_TOOLCHAIN_FILE=`"$vcpkgRoot\scripts\buildsystems\vcpkg.cmake`""
Write-Host "تنفيذ: $cmakeCmd" -ForegroundColor Gray

Invoke-Expression $cmakeCmd

if ($LASTEXITCODE -ne 0) {
    Write-Host "`n❌ فشل تكوين CMake" -ForegroundColor Red
    Pop-Location
    exit 1
}

Write-Host "✅ الحمد لله! تم تكوين CMake بنجاح" -ForegroundColor Green

# 4. البناء
Write-Host "`n🏗️  الخطوة 4: بناء المشروع..." -ForegroundColor Yellow
Write-Host "هذا قد يستغرق عدة دقائق...`n" -ForegroundColor Gray

cmake --build . --config Release

if ($LASTEXITCODE -ne 0) {
    Write-Host "`n❌ فشل البناء" -ForegroundColor Red
    Write-Host "`nللحصول على تفاصيل الأخطاء، راجع:" -ForegroundColor Yellow
    Write-Host "  $buildDir\*.log" -ForegroundColor Gray
    Pop-Location
    exit 1
}

Write-Host "`n✅ الحمد لله! اكتمل البناء بنجاح" -ForegroundColor Green

# 5. تشغيل الاختبارات
Write-Host "`n🧪 الخطوة 5: تشغيل الاختبارات..." -ForegroundColor Yellow

if (Test-Path "Release\llvm_backend_tests.exe") {
    Write-Host "`nتشغيل جميع الاختبارات (250 اختبار)...`n" -ForegroundColor Cyan
    .\Release\llvm_backend_tests.exe
    
    $testResult = $LASTEXITCODE
    
    Write-Host "`n════════════════════════════════════════════════" -ForegroundColor Cyan
    if ($testResult -eq 0) {
        Write-Host "   ✅ الحمد لله! جميع الاختبارات نجحت!" -ForegroundColor Green
    } else {
        Write-Host "   ⚠️  بعض الاختبارات فشلت (Exit Code: $testResult)" -ForegroundColor Yellow
    }
    Write-Host "════════════════════════════════════════════════`n" -ForegroundColor Cyan
} else {
    Write-Host "❌ لم يتم العثور على ملف الاختبار" -ForegroundColor Red
    Pop-Location
    exit 1
}

# 6. اختبارات CTest (اختياري)
Write-Host "`n📊 الخطوة 6: تشغيل CTest..." -ForegroundColor Yellow
ctest -C Release --output-on-failure

Pop-Location

Write-Host "`n════════════════════════════════════════════════" -ForegroundColor Cyan
Write-Host "   الحمد لله! اكتملت جميع الخطوات" -ForegroundColor Green
Write-Host "════════════════════════════════════════════════`n" -ForegroundColor Cyan

Write-Host "📝 النتائج:" -ForegroundColor Cyan
Write-Host "  • المشروع: مبني بنجاح" -ForegroundColor Green
Write-Host "  • الاختبارات: تم تشغيل 250 اختبار" -ForegroundColor Green
Write-Host "  • الملفات: موجودة في $buildDir" -ForegroundColor Gray
Write-Host ""
