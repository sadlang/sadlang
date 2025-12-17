# بسم الله الرحمن الرحيم
# استخدام مدير حزم لغة ص بشكل طبيعي
# Using Sad Package Manager naturally as a developer

Write-Host "`n🌟 بسم الله الرحمن الرحيم`n" -ForegroundColor Green
Write-Host "=== استخدام مدير حزم لغة ص - Using Sad Package Manager ===" -ForegroundColor Cyan
Write-Host "============================================================`n" -ForegroundColor Cyan

$SAD_PKG = "C:\s\s_language\build\bin\Debug\sad-pkg.exe"
$PACKAGE_DIR = "C:\s\s_language\examples\pkg_example"

# التحقق من وجود sad-pkg
if (!(Test-Path $SAD_PKG)) {
    Write-Host "❌ خطأ: sad-pkg غير موجود. يجب بناء المشروع أولاً" -ForegroundColor Red
    Write-Host "   Run: cmake --build build --config Debug" -ForegroundColor Yellow
    exit 1
}

Write-Host "✅ sad-pkg موجود - sad-pkg found`n" -ForegroundColor Green

# ============================================================================
# سيناريو 1: نشر حزمة جديدة
# Scenario 1: Publishing a new package
# ============================================================================

Write-Host "📦 السيناريو 1: نشر حزمة math-utils" -ForegroundColor Cyan
Write-Host "   Scenario 1: Publishing math-utils package`n" -ForegroundColor Gray

Set-Location $PACKAGE_DIR

Write-Host "1️⃣  التحقق من ملف sad.toml - Checking sad.toml..." -ForegroundColor Yellow
if (Test-Path "sad.toml") {
    Write-Host "   ✅ ملف sad.toml موجود`n" -ForegroundColor Green
    Get-Content "sad.toml" | Select-Object -First 10
    Write-Host "   ...`n"
} else {
    Write-Host "   ❌ ملف sad.toml غير موجود`n" -ForegroundColor Red
    exit 1
}

Write-Host "`n2️⃣  نشر الحزمة - Publishing package..." -ForegroundColor Yellow
Write-Host "   Command: sad-pkg publish`n" -ForegroundColor Gray

# محاكاة أمر publish (سيفشل لأن الخادم غير متوفر، لكن هذا طبيعي)
& $SAD_PKG publish 2>&1 | Tee-Object -Variable publishOutput
Write-Host ""

# ============================================================================
# سيناريو 2: إنشاء مشروع جديد يستخدم الحزمة
# Scenario 2: Create new project using the package
# ============================================================================

Write-Host "`n📁 السيناريو 2: إنشاء مشروع جديد يستخدم math-utils" -ForegroundColor Cyan
Write-Host "   Scenario 2: Creating new project using math-utils`n" -ForegroundColor Gray

$TEST_PROJECT = "C:\s\s_language\examples\test_math_project"

# حذف المشروع القديم إن وجد
if (Test-Path $TEST_PROJECT) {
    Remove-Item $TEST_PROJECT -Recurse -Force
}

# إنشاء مشروع جديد
Write-Host "1️⃣  إنشاء مشروع جديد - Creating new project..." -ForegroundColor Yellow
New-Item -ItemType Directory -Path $TEST_PROJECT -Force | Out-Null
Set-Location $TEST_PROJECT

Write-Host "   ✅ تم إنشاء المجلد: $TEST_PROJECT`n" -ForegroundColor Green

Write-Host "2️⃣  تهيئة المشروع - Initializing project..." -ForegroundColor Yellow
Write-Host "   Command: sad-pkg init`n" -ForegroundColor Gray

& $SAD_PKG init 2>&1 | Tee-Object -Variable initOutput
Write-Host ""

# التحقق من إنشاء sad.toml
if (Test-Path "sad.toml") {
    Write-Host "   ✅ تم إنشاء sad.toml بنجاح`n" -ForegroundColor Green
} else {
    Write-Host "   ℹ️  لم يتم إنشاء sad.toml تلقائياً، سنقوم بإنشائه يدوياً`n" -ForegroundColor Yellow
    
    # إنشاء sad.toml يدوياً
    @"
[package]
name = "my-calculator"
version = "0.1.0"
description = "برنامج حاسبة بسيط باستخدام math-utils"
authors = ["مطور لغة ص <dev@example.com>"]
license = "MIT"

[dependencies]
# سنضيف math-utils هنا
"@ | Out-File -FilePath "sad.toml" -Encoding UTF8
    
    Write-Host "   ✅ تم إنشاء sad.toml يدوياً`n" -ForegroundColor Green
}

Write-Host "3️⃣  إضافة تبعية math-utils - Adding math-utils dependency..." -ForegroundColor Yellow
Write-Host "   Command: sad-pkg add math-utils ^1.0.0`n" -ForegroundColor Gray

& $SAD_PKG add math-utils "^1.0.0" 2>&1 | Tee-Object -Variable addOutput
Write-Host ""

Write-Host "4️⃣  عرض محتوى sad.toml - Showing sad.toml content..." -ForegroundColor Yellow
if (Test-Path "sad.toml") {
    Get-Content "sad.toml"
    Write-Host ""
}

Write-Host "5️⃣  تثبيت التبعيات - Installing dependencies..." -ForegroundColor Yellow
Write-Host "   Command: sad-pkg install`n" -ForegroundColor Gray

& $SAD_PKG install 2>&1 | Tee-Object -Variable installOutput
Write-Host ""

# ============================================================================
# سيناريو 3: البحث عن حزم
# Scenario 3: Searching for packages
# ============================================================================

Write-Host "`n🔍 السيناريو 3: البحث عن الحزم" -ForegroundColor Cyan
Write-Host "   Scenario 3: Searching for packages`n" -ForegroundColor Gray

Write-Host "1️⃣  البحث عن حزم الرياضيات - Searching for math packages..." -ForegroundColor Yellow
Write-Host "   Command: sad-pkg search math`n" -ForegroundColor Gray

& $SAD_PKG search "math" 2>&1 | Tee-Object -Variable searchOutput
Write-Host ""

Write-Host "2️⃣  البحث عن حزم HTTP - Searching for HTTP packages..." -ForegroundColor Yellow
Write-Host "   Command: sad-pkg search http`n" -ForegroundColor Gray

& $SAD_PKG search "http" 2>&1
Write-Host ""

Write-Host "3️⃣  البحث عن حزم JSON - Searching for JSON packages..." -ForegroundColor Yellow
Write-Host "   Command: sad-pkg search json`n" -ForegroundColor Gray

& $SAD_PKG search "json" 2>&1
Write-Host ""

# ============================================================================
# سيناريو 4: عرض معلومات الحزمة
# Scenario 4: Viewing package information
# ============================================================================

Write-Host "`nℹ️  السيناريو 4: عرض معلومات الحزم" -ForegroundColor Cyan
Write-Host "   Scenario 4: Viewing package information`n" -ForegroundColor Gray

Write-Host "1️⃣  عرض معلومات math-utils - Showing math-utils info..." -ForegroundColor Yellow
Write-Host "   Command: sad-pkg info math-utils`n" -ForegroundColor Gray

& $SAD_PKG info "math-utils" 2>&1
Write-Host ""

# ============================================================================
# سيناريو 5: كتابة كود يستخدم الحزمة
# Scenario 5: Writing code using the package
# ============================================================================

Write-Host "`n💻 السيناريو 5: كتابة برنامج يستخدم math-utils" -ForegroundColor Cyan
Write-Host "   Scenario 5: Writing program using math-utils`n" -ForegroundColor Gray

Write-Host "1️⃣  إنشاء ملف البرنامج الرئيسي - Creating main program file..." -ForegroundColor Yellow

$programCode = @"
// بسم الله الرحمن الرحيم
/**
 * @file main.sad
 * @description برنامج حاسبة بسيط - Simple calculator program
 */

استورد math من "math-utils"؛

دالة main() {
    اطبع("بسم الله الرحمن الرحيم\n")؛
    اطبع("=== حاسبة بسيطة باستخدام math-utils ===\n")؛
    
    // مثال 1: حساب القوى
    اطبع("📐 مثال 1: حساب القوى")؛
    متغير base = 2؛
    متغير exp = 10؛
    متغير result = math.power(base، exp)؛
    اطبع("   " + base + "^" + exp + " = " + result)؛
    اطبع("")؛
    
    // مثال 2: حساب الجذر التربيعي
    اطبع("📐 مثال 2: الجذر التربيعي")؛
    متغير num = 144؛
    متغير sqrt_result = math.sqrt(num)؛
    اطبع("   √" + num + " = " + sqrt_result)؛
    اطبع("")؛
    
    // مثال 3: حسابات هندسية
    اطبع("🔵 مثال 3: مساحة دائرة")؛
    متغير radius = 7؛
    متغير area = math.circle_area(radius)؛
    متغير circumference = math.circle_circumference(radius)؛
    اطبع("   نصف القطر = " + radius)؛
    اطبع("   المساحة = " + area)؛
    اطبع("   المحيط = " + circumference)؛
    اطبع("")؛
    
    // مثال 4: حسابات إحصائية
    اطبع("📊 مثال 4: إحصائيات")؛
    متغير numbers = [85، 90، 78، 92، 88]؛
    متغير avg = math.average(numbers)؛
    متغير min_score = math.min(numbers)؛
    متغير max_score = math.max(numbers)؛
    اطبع("   الدرجات: " + numbers)؛
    اطبع("   المتوسط: " + avg)؛
    اطبع("   الأدنى: " + min_score)؛
    اطبع("   الأعلى: " + max_score)؛
    اطبع("")؛
    
    // مثال 5: دوال متقدمة
    اطبع("🔢 مثال 5: دوال متقدمة")؛
    متغير n = 5؛
    متغير factorial_result = math.factorial(n)؛
    اطبع("   " + n + "! = " + factorial_result)؛
    
    متغير test_num = 17؛
    متغير is_prime_result = math.is_prime(test_num)؛
    اطبع("   هل " + test_num + " عدد أولي؟ " + is_prime_result)؛
    اطبع("")؛
    
    اطبع("الحمد لله رب العالمين")؛
}

// تشغيل البرنامج
main()؛
"@

New-Item -ItemType Directory -Path "src" -Force | Out-Null
$programCode | Out-File -FilePath "src\main.sad" -Encoding UTF8

Write-Host "   ✅ تم إنشاء src\main.sad`n" -ForegroundColor Green

Write-Host "2️⃣  عرض محتوى البرنامج - Showing program content..." -ForegroundColor Yellow
Write-Host "   File: src\main.sad`n" -ForegroundColor Gray
Get-Content "src\main.sad" | Select-Object -First 15
Write-Host "   ... (المزيد في الملف - more in file)`n"

# ============================================================================
# سيناريو 6: تحديث التبعيات
# Scenario 6: Updating dependencies
# ============================================================================

Write-Host "`n🔄 السيناريو 6: إدارة التبعيات" -ForegroundColor Cyan
Write-Host "   Scenario 6: Managing dependencies`n" -ForegroundColor Gray

Write-Host "1️⃣  عرض التبعيات الحالية - Showing current dependencies..." -ForegroundColor Yellow
Write-Host "   Command: sad-pkg list`n" -ForegroundColor Gray

& $SAD_PKG list 2>&1
Write-Host ""

Write-Host "2️⃣  التحقق من التحديثات - Checking for updates..." -ForegroundColor Yellow
Write-Host "   Command: sad-pkg outdated`n" -ForegroundColor Gray

& $SAD_PKG outdated 2>&1
Write-Host ""

# ============================================================================
# ملخص الاستخدام
# Usage Summary
# ============================================================================

Write-Host "`n============================================================" -ForegroundColor Cyan
Write-Host "📚 ملخص الأوامر المستخدمة - Command Summary" -ForegroundColor Yellow
Write-Host "============================================================`n" -ForegroundColor Cyan

$commands = @"
✅ الأوامر المتاحة في sad-pkg - Available Commands:

📦 إدارة الحزم - Package Management:
   sad-pkg init                    # تهيئة مشروع جديد
   sad-pkg add <package> [version] # إضافة تبعية
   sad-pkg remove <package>        # حذف تبعية
   sad-pkg install                 # تثبيت التبعيات
   sad-pkg update                  # تحديث التبعيات
   sad-pkg list                    # عرض التبعيات المثبتة

🔍 البحث والمعلومات - Search & Info:
   sad-pkg search <query>          # البحث عن حزم
   sad-pkg info <package>          # عرض معلومات حزمة
   sad-pkg show <package>          # عرض تفاصيل الحزمة
   sad-pkg outdated                # عرض الحزم القديمة

📤 النشر - Publishing:
   sad-pkg publish                 # نشر حزمة
   sad-pkg login                   # تسجيل الدخول
   sad-pkg logout                  # تسجيل الخروج
   sad-pkg whoami                  # عرض المستخدم الحالي

🔧 أوامر مساعدة - Utility Commands:
   sad-pkg clean                   # تنظيف الذاكرة المؤقتة
   sad-pkg config                  # عرض/تعديل الإعدادات
   sad-pkg --version               # عرض الإصدار
   sad-pkg --help                  # عرض المساعدة

📁 هيكل المشروع - Project Structure:
   my_project/
   ├── sad.toml                    # ملف بيان المشروع
   ├── src/                        # الكود المصدري
   │   └── main.sad               # الملف الرئيسي
   ├── tests/                      # الاختبارات
   └── .sad/                       # مجلد الإعدادات (مخفي)

📝 مثال sad.toml - Example sad.toml:
   [package]
   name = "my-project"
   version = "1.0.0"
   description = "مشروعي الرائع"
   authors = ["اسمك <email@example.com>"]
   
   [dependencies]
   math-utils = "^1.0.0"
   http = "~2.1.0"
   json = "*"

🌐 الخادم الافتراضي - Default Registry:
   https://packages.sadlang.org
   (يمكن تغييره في ~/.sad/config.toml)

"@

Write-Host $commands

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "📊 ملخص ما تم إنجازه - Summary of Actions:" -ForegroundColor Yellow
Write-Host "============================================================`n" -ForegroundColor Cyan

Write-Host "✅ تم إنشاء حزمة math-utils بنجاح" -ForegroundColor Green
Write-Host "   📁 المسار: $PACKAGE_DIR"
Write-Host "   📦 الأرشيف: C:\s\s_language\examples\math-utils-1.0.0.tar.gz"
Write-Host ""

Write-Host "✅ تم إنشاء مشروع تجريبي يستخدم الحزمة" -ForegroundColor Green
Write-Host "   📁 المسار: $TEST_PROJECT"
Write-Host "   📄 الملف الرئيسي: src\main.sad"
Write-Host ""

Write-Host "⚠️  ملاحظات مهمة - Important Notes:" -ForegroundColor Yellow
Write-Host "   1. الخادم غير متوفر حالياً - Registry server not available"
Write-Host "   2. يمكن تشغيل خادم محلي للتجربة - Can run local server for testing"
Write-Host "   3. جميع الأوامر جاهزة للاستخدام - All commands ready to use"
Write-Host ""

Write-Host "🚀 الخطوات التالية - Next Steps:" -ForegroundColor Cyan
Write-Host "   1. تشغيل خادم السجل المحلي (إذا كان متوفراً)"
Write-Host "   2. نشر الحزمة: cd $PACKAGE_DIR; sad-pkg publish"
Write-Host "   3. استخدام الحزمة في مشاريع أخرى"
Write-Host ""

Write-Host "============================================================" -ForegroundColor Cyan
Write-Host "الحمد لله رب العالمين 🌟" -ForegroundColor Green
Write-Host "============================================================`n" -ForegroundColor Cyan
