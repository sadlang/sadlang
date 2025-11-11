# ⚙️ أوامر البناء والاختبار

**تاريخ الإنشاء:** 4 نوفمبر 2025  
**آخر تحديث:** 4 نوفمبر 2025

---

## 📋 جدول المحتويات

1. [المتطلبات المسبقة](#المتطلبات-المسبقة)
2. [أوامر البناء الأساسية](#أوامر-البناء-الأساسية)
3. [أوامر الاختبار](#أوامر-الاختبار)
4. [أوامر التنسيق والتحليل](#أوامر-التنسيق-والتحليل)
5. [أوامر Git الشائعة](#أوامر-git-الشائعة)
6. [سكريبتات مساعدة](#سكريبتات-مساعدة)

---

## 💻 المتطلبات المسبقة

### الأدوات المطلوبة (Windows)

| الأداة | الإصدار الأدنى | التحميل | الغرض |
|--------|----------------|---------|-------|
| **CMake** | 3.15+ | [cmake.org](https://cmake.org/download/) | بناء المشروع |
| **Visual Studio** | 2019+ | [visualstudio.com](https://visualstudio.microsoft.com/) | Compiler (MSVC) |
| **Git** | 2.30+ | [git-scm.com](https://git-scm.com/) | Version Control |
| **Ninja** (اختياري) | 1.10+ | [ninja-build.org](https://ninja-build.org/) | Build System سريع |
| **clang-format** (اختياري) | 12+ | مع LLVM/Clang | تنسيق الكود |
| **clang-tidy** (اختياري) | 12+ | مع LLVM/Clang | تحليل ثابت |

---

### التحقق من التثبيت

```powershell
# التحقق من CMake
cmake --version

# التحقق من Git
git --version

# التحقق من Visual Studio (البحث عن MSVC)
where cl

# التحقق من Ninja (اختياري)
ninja --version

# التحقق من clang-format (اختياري)
clang-format --version
```

---

## 🔨 أوامر البناء الأساسية

### البناء من الصفر (أول مرة)

```powershell
# الانتقال لمجلد المشروع
cd C:\s\s_language

# إنشاء مجلد البناء
mkdir build
cd build

# التكوين (Configure)
cmake .. -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Debug -DDEBUG=ON

# البناء (Build)
cmake --build . --config Debug

# أو باستخدام Ninja (أسرع)
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DDEBUG=ON
ninja
```

---

### البناء السريع (بعد التغييرات)

```powershell
# من مجلد s_language
cd C:\s\s_language\build

# البناء فقط
cmake --build . --config Debug

# أو مع Ninja
ninja

# البناء المتوازي (أسرع)
cmake --build . --config Debug --parallel 8
```

---

### البناء للإصدار (Release)

```powershell
cd C:\s\s_language\build

# تكوين Release
cmake .. -DCMAKE_BUILD_TYPE=Release -DDEBUG=OFF

# البناء
cmake --build . --config Release
```

---

### البناء النظيف (Clean Build)

```powershell
# حذف مجلد البناء
cd C:\s\s_language
Remove-Item -Recurse -Force build

# إعادة البناء
mkdir build
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DDEBUG=ON
ninja
```

---

### خيارات CMake المفيدة

```powershell
# تفعيل DEBUG
cmake .. -DDEBUG=ON

# تفعيل الاختبارات
cmake .. -DBUILD_TESTING=ON

# تحديد compiler معين
cmake .. -DCMAKE_CXX_COMPILER=clang++

# تصدير compile_commands.json (لـ clang-tidy)
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Verbose output
cmake .. --verbose

# البناء مع تفاصيل
cmake --build . --config Debug --verbose
```

---

## 🧪 أوامر الاختبار

### تشغيل كل الاختبارات

```powershell
cd C:\s\s_language\build

# تشغيل عبر CTest
ctest --output-on-failure -C Debug

# أو مباشرة
.\Debug\sad_tests.exe

# مع تفاصيل أكثر
ctest --verbose -C Debug
```

---

### تشغيل اختبارات محددة

```powershell
# اختبارات Lexer فقط
ctest -R lexer --output-on-failure

# اختبارات Parser فقط
ctest -R parser --output-on-failure

# اختبار واحد
.\Debug\sad_tests.exe --gtest_filter=LexerTest.TokenizeInteger

# مجموعة اختبارات
.\Debug\sad_tests.exe --gtest_filter=LexerTest.*

# استبعاد اختبارات
.\Debug\sad_tests.exe --gtest_filter=-ParserTest.*
```

---

### خيارات Google Test

```powershell
# قائمة الاختبارات المتاحة
.\Debug\sad_tests.exe --gtest_list_tests

# تشغيل عدد محدد من المرات
.\Debug\sad_tests.exe --gtest_repeat=10

# إيقاف عند أول فشل
.\Debug\sad_tests.exe --gtest_break_on_failure

# عشوائية ترتيب الاختبارات
.\Debug\sad_tests.exe --gtest_shuffle

# مخرجات XML (للـ CI)
.\Debug\sad_tests.exe --gtest_output=xml:test_results.xml

# مخرجات JSON
.\Debug\sad_tests.exe --gtest_output=json:test_results.json
```

---

### تقرير التغطية (Coverage)

```powershell
# باستخدام OpenCppCoverage (Windows)
OpenCppCoverage.exe `
    --sources C:\s\s_language\src `
    --sources C:\s\s_language\include `
    --excluded_sources C:\s\s_language\tests `
    --export_type html:coverage_report `
    --export_type cobertura:coverage.xml `
    -- .\Debug\sad_tests.exe

# فتح التقرير
Start-Process coverage_report\index.html
```

---

## 🎨 أوامر التنسيق والتحليل

### clang-format (تنسيق الكود)

```powershell
cd C:\s\s_language

# على ملف واحد
clang-format -i src\lexer\lexer_core.cpp

# على كل ملفات src
Get-ChildItem -Path src -Recurse -Include *.cpp,*.h | 
    ForEach-Object { clang-format -i $_.FullName }

# على كل ملفات include
Get-ChildItem -Path include -Recurse -Include *.cpp,*.h | 
    ForEach-Object { clang-format -i $_.FullName }

# على المشروع كله
Get-ChildItem -Path src,include -Recurse -Include *.cpp,*.h | 
    ForEach-Object { clang-format -i $_.FullName }

# فحص بدون تعديل (Dry run)
clang-format --dry-run --Werror src\lexer\lexer_core.cpp
```

---

### clang-tidy (التحليل الثابت)

```powershell
cd C:\s\s_language\build

# على ملف واحد
clang-tidy ..\src\lexer\lexer_core.cpp -p . -- -I ..\include

# على كل الملفات
Get-ChildItem -Path ..\src -Recurse -Include *.cpp | 
    ForEach-Object { clang-tidy $_.FullName -p . }

# إصلاح تلقائي (احذر!)
clang-tidy --fix ..\src\lexer\lexer_core.cpp -p .

# فقط فحص أخطاء حرجة
clang-tidy ..\src\lexer\lexer_core.cpp -p . `
    --checks='-*,clang-analyzer-*,bugprone-*'
```

---

### CppCheck (أداة تحليل إضافية)

```powershell
# تثبيت CppCheck من: http://cppcheck.sourceforge.net/

# تحليل المشروع
cppcheck --enable=all --inconclusive --xml --xml-version=2 `
    -I include/ src/ 2> cppcheck_report.xml

# تحليل بسيط
cppcheck src/
```

---

## 📦 أوامر Git الشائعة

### إنشاء فرع جديد

```powershell
cd C:\s\s_language

# تحديث develop
git checkout develop
git pull origin develop

# إنشاء فرع
git checkout -b feat/parser/ast-nodes

# التأكد من الفرع الحالي
git branch --show-current
```

---

### Commit والـ Push

```powershell
# إضافة الملفات
git add src/parser/parser_core.cpp include/parser/parser_core.h

# أو إضافة كل التغييرات
git add .

# Commit
git commit -m "[parser] implement parser core"

# Push لأول مرة
git push --set-upstream origin feat/parser/ast-nodes

# Push عادي
git push
```

---

### التحديث من develop

```powershell
# حفظ التغييرات الحالية
git stash

# التحديث
git checkout develop
git pull origin develop
git checkout feat/parser/ast-nodes
git rebase develop

# استرجاع التغييرات
git stash pop
```

---

### حل Conflicts

```powershell
# عند حدوث conflict أثناء rebase
# حرر الملفات يدوياً، ثم:
git add <resolved-files>
git rebase --continue

# للإلغاء
git rebase --abort

# Push بعد الحل
git push origin feat/parser/ast-nodes --force
```

---

### أوامر مفيدة أخرى

```powershell
# حالة المستودع
git status

# عرض التغييرات
git diff

# عرض التاريخ
git log --oneline --graph --all -20

# التراجع عن commit (بدون حذف التغييرات)
git reset --soft HEAD~1

# التراجع عن commit (مع حذف التغييرات)
git reset --hard HEAD~1

# تنظيف الملفات غير المتتبعة
git clean -fd

# عرض الفروع
git branch -a

# حذف فرع محلي
git branch -d feat/old-branch

# حذف فرع من remote
git push origin --delete feat/old-branch
```

---

## 🔧 سكريبتات مساعدة

### سكريبت بناء كامل (build.ps1)

```powershell
# C:\s\s_language\scripts\build.ps1

param(
    [string]$BuildType = "Debug",
    [switch]$Clean,
    [switch]$Test
)

$ErrorActionPreference = "Stop"
$ProjectRoot = "C:\s\s_language"

Write-Host "=== Building Sad Language ===" -ForegroundColor Cyan
Write-Host "Build Type: $BuildType" -ForegroundColor Yellow

# Clean build إذا طُلب
if ($Clean) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item -Path "$ProjectRoot\build" -Recurse -Force -ErrorAction SilentlyContinue
}

# إنشاء مجلد build
if (!(Test-Path "$ProjectRoot\build")) {
    New-Item -Path "$ProjectRoot\build" -ItemType Directory | Out-Null
}

# CMake Configure
Write-Host "Configuring with CMake..." -ForegroundColor Yellow
Set-Location "$ProjectRoot\build"
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=$BuildType -DDEBUG=ON

# Build
Write-Host "Building..." -ForegroundColor Yellow
ninja

# Test إذا طُلب
if ($Test) {
    Write-Host "Running tests..." -ForegroundColor Yellow
    ctest --output-on-failure -C $BuildType
}

Write-Host "=== Build Complete ===" -ForegroundColor Green
```

**الاستخدام:**
```powershell
# بناء Debug
.\scripts\build.ps1

# بناء Release
.\scripts\build.ps1 -BuildType Release

# بناء نظيف مع اختبارات
.\scripts\build.ps1 -Clean -Test
```

---

### سكريبت اختبار (test.ps1)

```powershell
# C:\s\s_language\scripts\test.ps1

param(
    [string]$Filter = "*",
    [switch]$Coverage,
    [switch]$Verbose
)

$ErrorActionPreference = "Stop"
$BuildDir = "C:\s\s_language\build"

Set-Location $BuildDir

if ($Coverage) {
    Write-Host "Running tests with coverage..." -ForegroundColor Cyan
    OpenCppCoverage.exe `
        --sources C:\s\s_language\src `
        --sources C:\s\s_language\include `
        --export_type html:coverage `
        -- .\Debug\sad_tests.exe --gtest_filter=$Filter
    
    Start-Process coverage\index.html
} elseif ($Verbose) {
    ctest --verbose -R $Filter
} else {
    ctest --output-on-failure -R $Filter
}
```

**الاستخدام:**
```powershell
# تشغيل كل الاختبارات
.\scripts\test.ps1

# اختبارات Lexer فقط
.\scripts\test.ps1 -Filter "lexer"

# مع تغطية
.\scripts\test.ps1 -Coverage
```

---

### سكريبت تنسيق (format.ps1)

```powershell
# C:\s\s_language\scripts\format.ps1

param(
    [switch]$Check,
    [string]$Path = ""
)

$ProjectRoot = "C:\s\s_language"

if ($Path) {
    $Files = Get-ChildItem -Path $Path -Recurse -Include *.cpp,*.h
} else {
    $Files = Get-ChildItem -Path "$ProjectRoot\src","$ProjectRoot\include" `
             -Recurse -Include *.cpp,*.h
}

Write-Host "Formatting $($Files.Count) files..." -ForegroundColor Cyan

foreach ($File in $Files) {
    if ($Check) {
        clang-format --dry-run --Werror $File.FullName
    } else {
        clang-format -i $File.FullName
    }
}

Write-Host "Done!" -ForegroundColor Green
```

**الاستخدام:**
```powershell
# تنسيق كل الملفات
.\scripts\format.ps1

# فحص فقط
.\scripts\format.ps1 -Check

# تنسيق مجلد محدد
.\scripts\format.ps1 -Path C:\s\s_language\src\lexer
```

---

### سكريبت فحص شامل (check.ps1)

```powershell
# C:\s\s_language\scripts\check.ps1

$ErrorActionPreference = "Stop"
$ProjectRoot = "C:\s\s_language"

Write-Host "=== Running Quality Checks ===" -ForegroundColor Cyan

# 1. البناء
Write-Host "`n1. Building..." -ForegroundColor Yellow
Set-Location "$ProjectRoot\build"
ninja
if ($LASTEXITCODE -ne 0) { 
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1 
}

# 2. الاختبارات
Write-Host "`n2. Running tests..." -ForegroundColor Yellow
ctest --output-on-failure
if ($LASTEXITCODE -ne 0) { 
    Write-Host "Tests failed!" -ForegroundColor Red
    exit 1 
}

# 3. التنسيق
Write-Host "`n3. Checking format..." -ForegroundColor Yellow
& "$ProjectRoot\scripts\format.ps1" -Check
if ($LASTEXITCODE -ne 0) { 
    Write-Host "Format check failed!" -ForegroundColor Red
    exit 1 
}

# 4. clang-tidy (اختياري)
if (Get-Command clang-tidy -ErrorAction SilentlyContinue) {
    Write-Host "`n4. Running clang-tidy..." -ForegroundColor Yellow
    clang-tidy "$ProjectRoot\src\lexer\lexer_core.cpp" -p .
}

Write-Host "`n=== All Checks Passed ===" -ForegroundColor Green
```

**الاستخدام:**
```powershell
# قبل كل commit أو PR
.\scripts\check.ps1
```

---

## 📝 أوامر سريعة مفيدة

### احصائيات المشروع

```powershell
# عدد أسطر الكود
Get-ChildItem -Path src,include -Recurse -Include *.cpp,*.h | 
    Get-Content | Measure-Object -Line

# عدد الملفات
(Get-ChildItem -Path src,include -Recurse -Include *.cpp,*.h).Count

# أحجام الملفات
Get-ChildItem -Path src,include -Recurse -Include *.cpp,*.h | 
    Select-Object Name, @{Name="Lines";Expression={(Get-Content $_.FullName | Measure-Object -Line).Lines}} | 
    Sort-Object Lines -Descending
```

---

### فتح المحرر

```powershell
# فتح VS Code
code C:\s\s_language

# فتح Visual Studio
devenv C:\s\s_language\build\sad_language.sln
```

---

### تنظيف سريع

```powershell
# حذف ملفات البناء
Remove-Item -Recurse -Force build

# حذف الملفات المؤقتة
Get-ChildItem -Recurse -Include *.obj,*.o,*.pdb,*.ilk | Remove-Item

# تنظيف Git
git clean -fdx
```

---

## 🎯 Quick Reference

### البناء والاختبار (الأكثر استخداماً)

```powershell
# بناء سريع
cd C:\s\s_language\build
ninja

# اختبار سريع
ctest --output-on-failure

# بناء + اختبار
ninja && ctest --output-on-failure

# تنسيق الكود
Get-ChildItem -Path ..\src,..\include -Recurse -Include *.cpp,*.h | 
    ForEach-Object { clang-format -i $_.FullName }

# فحص شامل
..\scripts\check.ps1
```

---

## 📞 المساعدة

إذا واجهت مشكلة مع الأوامر:

1. **راجع الوثائق:**
   - [CMake Documentation](https://cmake.org/documentation/)
   - [Google Test](https://github.com/google/googletest)
   - [clang-format](https://clang.llvm.org/docs/ClangFormat.html)

2. **افتح Issue:**
   - على GitHub مع تفاصيل الخطأ

3. **اسأل الفريق:**
   - في Discord أو قنوات التواصل

---

**آخر تحديث:** 4 نوفمبر 2025  
**المرجع:** [الخطة الرئيسية](00_MASTER_PLAN.md) | [دليل سير العمل](03_WORKFLOW_GUIDE.md)
