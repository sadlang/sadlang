# بسم الله الرحمن الرحيم
# Script to publish math-utils package to registry
# سكريبت لنشر حزمة math-utils على السجل

Write-Host "بسم الله الرحمن الرحيم`n" -ForegroundColor Green
Write-Host "=== نشر حزمة لغة ص - Sad Package Publisher ===" -ForegroundColor Cyan
Write-Host "================================================`n" -ForegroundColor Cyan

# الإعدادات
$PACKAGE_DIR = "C:\s\s_language\examples\pkg_example"
$PACKAGE_NAME = "math-utils"
$PACKAGE_VERSION = "1.0.0"
$ARCHIVE_NAME = "$PACKAGE_NAME-$PACKAGE_VERSION.tar.gz"
$ARCHIVE_PATH = "C:\s\s_language\examples\$ARCHIVE_NAME"
$REGISTRY_URL = "http://localhost:8080"

# يمكن تغيير URL السجل من سطر الأوامر
if ($args.Count -gt 0) {
    $REGISTRY_URL = $args[0]
}

Write-Host "📋 معلومات الحزمة - Package Information:" -ForegroundColor Yellow
Write-Host "   Name: $PACKAGE_NAME"
Write-Host "   Version: $PACKAGE_VERSION"
Write-Host "   Directory: $PACKAGE_DIR"
Write-Host "   Archive: $ARCHIVE_PATH"
Write-Host "   Registry: $REGISTRY_URL`n"

# ============================================================================
# الخطوة 1: التحقق من وجود الحزمة
# Step 1: Verify package exists
# ============================================================================

Write-Host "📦 الخطوة 1: التحقق من الحزمة - Step 1: Verifying package..." -ForegroundColor Cyan

if (!(Test-Path $PACKAGE_DIR)) {
    Write-Host "❌ خطأ: مجلد الحزمة غير موجود - Error: Package directory not found" -ForegroundColor Red
    Write-Host "   Path: $PACKAGE_DIR"
    exit 1
}

if (!(Test-Path "$PACKAGE_DIR\sad.toml")) {
    Write-Host "❌ خطأ: ملف sad.toml غير موجود - Error: sad.toml not found" -ForegroundColor Red
    exit 1
}

Write-Host "✅ الحزمة موجودة - Package exists`n" -ForegroundColor Green

# ============================================================================
# الخطوة 2: إنشاء الأرشيف
# Step 2: Create archive
# ============================================================================

Write-Host "📦 الخطوة 2: إنشاء الأرشيف - Step 2: Creating archive..." -ForegroundColor Cyan

# حذف الأرشيف القديم إن وجد
if (Test-Path $ARCHIVE_PATH) {
    Write-Host "   حذف الأرشيف القديم - Removing old archive..."
    Remove-Item $ARCHIVE_PATH -Force
}

# إنشاء أرشيف جديد
Set-Location $PACKAGE_DIR
& tar -czf $ARCHIVE_PATH .

if (!(Test-Path $ARCHIVE_PATH)) {
    Write-Host "❌ فشل إنشاء الأرشيف - Failed to create archive" -ForegroundColor Red
    exit 1
}

$archiveSize = (Get-Item $ARCHIVE_PATH).Length
Write-Host "✅ تم إنشاء الأرشيف - Archive created: $archiveSize bytes`n" -ForegroundColor Green

# ============================================================================
# الخطوة 3: التحقق من الخادم
# Step 3: Check server
# ============================================================================

Write-Host "🌐 الخطوة 3: التحقق من الخادم - Step 3: Checking server..." -ForegroundColor Cyan

try {
    $response = Invoke-WebRequest -Uri "$REGISTRY_URL/health" -Method GET -TimeoutSec 5 -ErrorAction Stop
    Write-Host "✅ الخادم يعمل - Server is running" -ForegroundColor Green
    Write-Host "   Status: $($response.StatusCode)`n"
} catch {
    Write-Host "❌ تحذير: لا يمكن الاتصال بالخادم - Warning: Cannot connect to server" -ForegroundColor Yellow
    Write-Host "   URL: $REGISTRY_URL/health"
    Write-Host "   Error: $($_.Exception.Message)"
    Write-Host "`n⚠️  تأكد من تشغيل خادم السجل أولاً - Make sure registry server is running first`n" -ForegroundColor Yellow
    
    Write-Host "💡 لتشغيل خادم محلي باستخدام Dart:" -ForegroundColor Cyan
    Write-Host "   1. cd C:\kadahtech\s_website\s_module_server"
    Write-Host "   2. dart run bin/server.dart`n"
    
    $continue = Read-Host "هل تريد المتابعة على أي حال؟ (y/n) - Continue anyway?"
    if ($continue -ne 'y' -and $continue -ne 'Y') {
        exit 1
    }
}

# ============================================================================
# الخطوة 4: تسجيل الدخول والحصول على رمز المصادقة
# Step 4: Login and get authentication token
# ============================================================================

Write-Host "🔐 الخطوة 4: المصادقة - Step 4: Authentication..." -ForegroundColor Cyan

# طلب بيانات المستخدم
Write-Host "`nأدخل بيانات الدخول - Enter login credentials:"
$username = Read-Host "   Username"
$password = Read-Host "   Password" -AsSecureString
$passwordPlain = [Runtime.InteropServices.Marshal]::PtrToStringAuto([Runtime.InteropServices.Marshal]::SecureStringToBSTR($password))

# محاولة تسجيل الدخول
Write-Host "`n   جاري تسجيل الدخول - Logging in..."

try {
    $loginBody = @{
        username = $username
        password = $passwordPlain
    } | ConvertTo-Json

    $loginResponse = Invoke-RestMethod -Uri "$REGISTRY_URL/api/v1/auth/login" -Method POST -Body $loginBody -ContentType "application/json" -TimeoutSec 10
    
    if ($loginResponse.token) {
        $authToken = $loginResponse.token
        Write-Host "✅ تم تسجيل الدخول بنجاح - Login successful" -ForegroundColor Green
        Write-Host "   Token: $($authToken.Substring(0, [Math]::Min(20, $authToken.Length)))...`n"
    } else {
        Write-Host "❌ فشل تسجيل الدخول - Login failed: No token received" -ForegroundColor Red
        exit 1
    }
} catch {
    Write-Host "❌ فشل تسجيل الدخول - Login failed" -ForegroundColor Red
    Write-Host "   Error: $($_.Exception.Message)`n"
    
    Write-Host "⚠️  يمكنك إدخال رمز المصادقة يدوياً - You can enter auth token manually:" -ForegroundColor Yellow
    $authToken = Read-Host "   Auth Token (أو اضغط Enter للمتابعة بدون رمز - or press Enter to skip)"
    
    if ([string]::IsNullOrWhiteSpace($authToken)) {
        Write-Host "`n❌ لا يمكن رفع الحزمة بدون رمز مصادقة - Cannot upload package without auth token" -ForegroundColor Red
        exit 1
    }
}

# ============================================================================
# الخطوة 5: رفع الحزمة
# Step 5: Upload package
# ============================================================================

Write-Host "📤 الخطوة 5: رفع الحزمة - Step 5: Uploading package..." -ForegroundColor Cyan

try {
    # استخدام curl لرفع الملف (multipart/form-data)
    Write-Host "   جاري رفع الملف - Uploading file..."
    
    $curlCommand = "curl -X POST -H `"Authorization: Bearer $authToken`" -F `"package=@$ARCHIVE_PATH`" $REGISTRY_URL/api/v1/packages/publish"
    
    Write-Host "`n   Command: $curlCommand`n"
    
    $uploadResult = & curl -X POST -H "Authorization: Bearer $authToken" -F "package=@$ARCHIVE_PATH" "$REGISTRY_URL/api/v1/packages/publish" 2>&1
    
    Write-Host "   Response: $uploadResult"
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "`n✅ تم رفع الحزمة بنجاح - Package uploaded successfully!`n" -ForegroundColor Green
    } else {
        Write-Host "`n⚠️  قد تكون هناك مشكلة في الرفع - There might be an issue with upload" -ForegroundColor Yellow
        Write-Host "   Exit code: $LASTEXITCODE`n"
    }
} catch {
    Write-Host "❌ فشل رفع الحزمة - Upload failed" -ForegroundColor Red
    Write-Host "   Error: $($_.Exception.Message)`n"
    
    Write-Host "💡 يمكنك رفع الحزمة يدوياً باستخدام:" -ForegroundColor Cyan
    Write-Host "   curl -X POST \\"
    Write-Host "     -H `"Authorization: Bearer $authToken`" \\"
    Write-Host "     -F `"package=@$ARCHIVE_PATH`" \\"
    Write-Host "     $REGISTRY_URL/api/v1/packages/publish`n"
}

# ============================================================================
# الخطوة 6: التحقق من الرفع
# Step 6: Verify upload
# ============================================================================

Write-Host "🔍 الخطوة 6: التحقق من الرفع - Step 6: Verifying upload..." -ForegroundColor Cyan

Start-Sleep -Seconds 2

try {
    $searchUrl = "$REGISTRY_URL/api/v1/search?q=$PACKAGE_NAME&limit=5"
    $searchResponse = Invoke-RestMethod -Uri $searchUrl -Method GET -TimeoutSec 10
    
    if ($searchResponse.packages) {
        $found = $false
        foreach ($pkg in $searchResponse.packages) {
            if ($pkg.name -eq $PACKAGE_NAME) {
                $found = $true
                Write-Host "✅ تم العثور على الحزمة في السجل - Package found in registry!" -ForegroundColor Green
                Write-Host "   Name: $($pkg.name)"
                Write-Host "   Version: $($pkg.version)"
                Write-Host "   Description: $($pkg.description)`n"
                break
            }
        }
        
        if (!$found) {
            Write-Host "⚠️  لم يتم العثور على الحزمة في نتائج البحث - Package not found in search results" -ForegroundColor Yellow
            Write-Host "   قد تحتاج إلى الانتظار قليلاً للفهرسة - May need to wait for indexing`n"
        }
    } else {
        Write-Host "⚠️  لا توجد نتائج بحث - No search results" -ForegroundColor Yellow
    }
} catch {
    Write-Host "⚠️  فشل البحث - Search failed" -ForegroundColor Yellow
    Write-Host "   Error: $($_.Exception.Message)`n"
}

# ============================================================================
# ملخص
# Summary
# ============================================================================

Write-Host "================================================" -ForegroundColor Cyan
Write-Host "📊 ملخص - Summary:`n" -ForegroundColor Yellow

Write-Host "   Package: $PACKAGE_NAME v$PACKAGE_VERSION"
Write-Host "   Archive: $ARCHIVE_PATH ($archiveSize bytes)"
Write-Host "   Registry: $REGISTRY_URL"
Write-Host "`n   للبحث عن الحزمة - To search for package:"
Write-Host "   sad-pkg search $PACKAGE_NAME"
Write-Host "`n   لتثبيت الحزمة - To install package:"
Write-Host "   sad-pkg add $PACKAGE_NAME ^$PACKAGE_VERSION"
Write-Host "`n   لعرض معلومات الحزمة - To view package info:"
Write-Host "   sad-pkg info $PACKAGE_NAME`n"

Write-Host "================================================" -ForegroundColor Cyan
Write-Host "الحمد لله رب العالمين" -ForegroundColor Green

# الحمد لله رب العالمين
