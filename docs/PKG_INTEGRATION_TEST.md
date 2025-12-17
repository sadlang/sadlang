# بسم الله الرحمن الرحيم

# Integration Testing Guide
# دليل اختبار التكامل بين المترجم والخادم

هذا الدليل يوضح كيفية اختبار التكامل الكامل بين مترجم لغة ص وخادم السجل.

---

## Prerequisites / المتطلبات الأساسية

### 1. Build the Package Manager / بناء مدير الحزم

```powershell
# في مستودع المترجم (C:\s\s_language)
cd C:\s\s_language
mkdir build
cd build
cmake .. -DBUILD_PKG_MANAGER=ON
cmake --build . --config Release
```

### 2. Start the Registry Server / تشغيل خادم السجل

إذا كان لديك خادم Dart:

```bash
# في مستودع الخادم
cd C:\kadahtech\s_website\s_module_server
dart pub get
dart run bin/server.dart
```

أو يمكنك استخدام خادم تجريبي محلي.

---

## Test Scenarios / سيناريوهات الاختبار

### Scenario 1: Search for Packages / البحث عن الحزم

**من سطر الأوامر:**

```powershell
# البحث عن حزمة
C:\s\s_language\build\bin\Release\sad-pkg.exe search "http"
```

**النتيجة المتوقعة:**
```
Searching for packages matching "http"...

Found 3 packages:
  1. http (v1.2.0) - HTTP client library
  2. http-server (v2.0.0) - HTTP server framework
  3. https-proxy (v0.5.0) - HTTPS proxy server
```

**Testing via C++ Code:**

```cpp
// test_registry_client.cpp
#include "tools/pkg/registry_client.h"
#include <iostream>

int main() {
    using namespace sad::pkg;
    
    try {
        // Create registry client
        RegistryClient registry("http://localhost:8080");
        
        // Search for packages
        std::cout << "البحث عن حزم HTTP...\n";
        auto results = registry.search("http", 5);
        
        std::cout << "النتائج: " << results.size() << " حزمة\n";
        for (const auto& pkg : results) {
            std::cout << "  - " << pkg << "\n";
        }
        
        std::cout << "✓ نجح الاختبار\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ فشل الاختبار: " << e.what() << "\n";
        return 1;
    }
}
```

---

### Scenario 2: Get Package Metadata / الحصول على بيانات الحزمة

**من سطر الأوامر:**

```powershell
# عرض معلومات حزمة
C:\s\s_language\build\bin\Release\sad-pkg.exe info http
```

**النتيجة المتوقعة:**
```
Package: http
Version: 1.2.0
Description: HTTP client library for Sad Language
License: MIT
Repository: https://github.com/sad-lang/http
Downloads: 1,523

Dependencies:
  - json ^2.1.0
  - crypto ~1.0.5
```

**Testing via C++ Code:**

```cpp
// test_package_metadata.cpp
#include "tools/pkg/registry_client.h"
#include <iostream>

int main() {
    using namespace sad::pkg;
    
    try {
        RegistryClient registry("http://localhost:8080");
        
        // Get package versions
        std::cout << "الحصول على إصدارات حزمة http...\n";
        auto versions = registry.get_available_versions("http");
        
        std::cout << "الإصدارات المتاحة:\n";
        for (const auto& ver : versions) {
            std::cout << "  - " << ver.to_string() << "\n";
        }
        
        // Get metadata for latest version
        if (!versions.empty()) {
            auto latest = versions[0];
            std::cout << "\nالحصول على بيانات الإصدار " << latest.to_string() << "...\n";
            auto pkg = registry.get_package_metadata("http", latest);
            
            std::cout << "الاسم: " << pkg.name << "\n";
            std::cout << "الإصدار: " << pkg.version.to_string() << "\n";
            std::cout << "الوصف: " << pkg.description << "\n";
            std::cout << "الترخيص: " << pkg.license << "\n";
            
            std::cout << "\nالتبعيات:\n";
            for (const auto& [name, version] : pkg.dependencies) {
                std::cout << "  - " << name << " " << version << "\n";
            }
        }
        
        std::cout << "\n✓ نجح الاختبار\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ فشل الاختبار: " << e.what() << "\n";
        return 1;
    }
}
```

---

### Scenario 3: Download and Install Package / تنزيل وتثبيت حزمة

**من سطر الأوامر:**

```powershell
# تثبيت حزمة
cd C:\my_project
C:\s\s_language\build\bin\Release\sad-pkg.exe add http ^1.2.0
C:\s\s_language\build\bin\Release\sad-pkg.exe install
```

**النتيجة المتوقعة:**
```
Adding dependency: http ^1.2.0
Updated sad.toml

Installing dependencies...
  Resolving http ^1.2.0...
  Found: http 1.2.0
  Dependencies:
    - json ^2.1.0
    - crypto ~1.0.5

  Downloading http 1.2.0... ✓
  Downloading json 2.1.3... ✓
  Downloading crypto 1.0.7... ✓

  Installing http 1.2.0... ✓
  Installing json 2.1.3... ✓
  Installing crypto 1.0.7... ✓

Successfully installed 3 packages in 2.3s
```

**Testing via C++ Code:**

```cpp
// test_download.cpp
#include "tools/pkg/registry_client.h"
#include "tools/pkg/dependency_resolver.h"
#include <iostream>
#include <filesystem>

int main() {
    using namespace sad::pkg;
    namespace fs = std::filesystem;
    
    try {
        RegistryClient registry("http://localhost:8080");
        PackageCache cache;
        
        // Create test package
        Package test_pkg;
        test_pkg.name = "test-project";
        test_pkg.version = Version(1, 0, 0);
        test_pkg.dependencies["http"] = "^1.2.0";
        
        // Resolve dependencies
        std::cout << "حل التبعيات...\n";
        DependencyResolver resolver(registry);
        auto resolved = resolver.resolve(test_pkg);
        
        std::cout << "التبعيات المحلولة:\n";
        for (const auto& [name, pkg] : resolved) {
            std::cout << "  - " << name << " " << pkg.version.to_string() << "\n";
        }
        
        // Download packages
        std::cout << "\nتنزيل الحزم...\n";
        for (const auto& [name, pkg] : resolved) {
            // Skip if already cached
            if (cache.is_cached(name, pkg.version)) {
                std::cout << "  ✓ " << name << " " << pkg.version.to_string() 
                         << " (cached)\n";
                continue;
            }
            
            // Download
            fs::path download_path = fs::temp_directory_path() / 
                                    (name + "-" + pkg.version.to_string() + ".tar.gz");
            
            std::cout << "  تنزيل " << name << " " << pkg.version.to_string() << "...";
            
            if (registry.download_package(name, pkg.version, download_path)) {
                std::cout << " ✓\n";
                
                // TODO: Extract and cache
                // extract_package(download_path, cache);
                
            } else {
                std::cout << " ✗\n";
                throw std::runtime_error("Failed to download " + name);
            }
        }
        
        std::cout << "\n✓ نجح الاختبار\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ فشل الاختبار: " << e.what() << "\n";
        return 1;
    }
}
```

---

### Scenario 4: Publish Package / نشر حزمة

**من سطر الأوامر:**

```powershell
# تسجيل الدخول
C:\s\s_language\build\bin\Release\sad-pkg.exe login

# نشر الحزمة
cd C:\my_library
C:\s\s_language\build\bin\Release\sad-pkg.exe publish
```

**النتيجة المتوقعة:**
```
Publishing mylib 1.0.0...

Creating package archive...
  ✓ Validating manifest
  ✓ Collecting files
  ✓ Creating tarball

Uploading to registry...
  ✓ Uploading (45 KB)
  ✓ Verifying checksum
  ✓ Processing

Successfully published mylib 1.0.0!
View at: https://packages.sadlang.org/packages/mylib/1.0.0
```

**Testing Publish (requires auth token):**

```cpp
// test_publish.cpp
#include "tools/pkg/registry_client.h"
#include <iostream>

int main() {
    using namespace sad::pkg;
    
    try {
        std::string token = "YOUR_AUTH_TOKEN";  // Get from login
        RegistryClient registry("http://localhost:8080");
        
        // Create package
        Package pkg;
        pkg.name = "test-lib";
        pkg.version = Version(1, 0, 0);
        pkg.description = "Test library";
        pkg.authors.push_back("Test Author <test@example.com>");
        pkg.license = "MIT";
        
        // Publish
        std::cout << "نشر الحزمة " << pkg.name << " " 
                 << pkg.version.to_string() << "...\n";
        
        if (registry.publish(pkg, token)) {
            std::cout << "✓ تم النشر بنجاح\n";
        } else {
            std::cout << "✗ فشل النشر\n";
            return 1;
        }
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "✗ فشل الاختبار: " << e.what() << "\n";
        return 1;
    }
}
```

---

## Integration Test Script / نص اختبار التكامل الكامل

قم بإنشاء ملف `test_integration.ps1`:

```powershell
# بسم الله الرحمن الرحيم
# Integration test script for Sad Package Manager
# نص اختبار التكامل لمدير حزم لغة ص

Write-Host "بسم الله الرحمن الرحيم" -ForegroundColor Cyan
Write-Host "اختبار التكامل لمدير حزم لغة ص`n" -ForegroundColor Cyan

$ErrorActionPreference = "Stop"
$sad_pkg = "C:\s\s_language\build\bin\Release\sad-pkg.exe"
$registry_url = "http://localhost:8080"

# Check if sad-pkg exists
if (!(Test-Path $sad_pkg)) {
    Write-Host "✗ لم يتم العثور على sad-pkg.exe" -ForegroundColor Red
    Write-Host "قم ببناء المشروع أولاً: cmake --build build --config Release" -ForegroundColor Yellow
    exit 1
}

# Test 1: Search
Write-Host "[1/5] اختبار البحث..." -ForegroundColor Yellow
& $sad_pkg search "http"
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ نجح اختبار البحث`n" -ForegroundColor Green
} else {
    Write-Host "✗ فشل اختبار البحث`n" -ForegroundColor Red
    exit 1
}

# Test 2: Package Info
Write-Host "[2/5] اختبار معلومات الحزمة..." -ForegroundColor Yellow
& $sad_pkg info http
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ نجح اختبار معلومات الحزمة`n" -ForegroundColor Green
} else {
    Write-Host "✗ فشل اختبار معلومات الحزمة`n" -ForegroundColor Red
    exit 1
}

# Test 3: Create Test Project
Write-Host "[3/5] إنشاء مشروع اختبار..." -ForegroundColor Yellow
$test_dir = Join-Path $env:TEMP "sad_test_$(Get-Random)"
New-Item -ItemType Directory -Path $test_dir -Force | Out-Null
Push-Location $test_dir

& $sad_pkg init
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ نجح إنشاء المشروع`n" -ForegroundColor Green
} else {
    Write-Host "✗ فشل إنشاء المشروع`n" -ForegroundColor Red
    Pop-Location
    exit 1
}

# Test 4: Add Dependency
Write-Host "[4/5] إضافة تبعية..." -ForegroundColor Yellow
& $sad_pkg add "json" "^2.0.0"
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ نجح إضافة التبعية`n" -ForegroundColor Green
} else {
    Write-Host "✗ فشل إضافة التبعية`n" -ForegroundColor Red
    Pop-Location
    exit 1
}

# Test 5: Install Dependencies
Write-Host "[5/5] تثبيت التبعيات..." -ForegroundColor Yellow
& $sad_pkg install
if ($LASTEXITCODE -eq 0) {
    Write-Host "✓ نجح تثبيت التبعيات`n" -ForegroundColor Green
} else {
    Write-Host "✗ فشل تثبيت التبعيات`n" -ForegroundColor Red
    Pop-Location
    exit 1
}

# Cleanup
Pop-Location
Remove-Item -Recurse -Force $test_dir

Write-Host "`n═══════════════════════════════════════" -ForegroundColor Cyan
Write-Host "✓ نجحت جميع الاختبارات!" -ForegroundColor Green
Write-Host "═══════════════════════════════════════`n" -ForegroundColor Cyan
Write-Host "الحمد لله رب العالمين" -ForegroundColor Cyan
```

قم بتشغيل الاختبار:

```powershell
.\test_integration.ps1
```

---

## Configuration / الإعدادات

يمكنك تعديل عنوان السجل في الملف:

**Windows:**
```
C:\Users\<YourName>\.sad\config.toml
```

**Unix/Linux/Mac:**
```
~/.sad/config.toml
```

محتوى الملف:

```toml
[registry]
url = "https://packages.sadlang.org"
# أو للاختبار المحلي:
# url = "http://localhost:8080"

[auth]
token = "your_api_token_here"
```

---

## Troubleshooting / استكشاف الأخطاء

### خطأ: "Failed to connect to registry"

**السبب:** الخادم غير متاح

**الحل:**
```powershell
# تحقق من تشغيل الخادم
curl http://localhost:8080/health

# أو
Invoke-WebRequest -Uri "http://localhost:8080/health"
```

---

### خطأ: "Package not found"

**السبب:** الحزمة غير موجودة في السجل

**الحل:**
```powershell
# تحقق من توفر الحزمة
C:\s\s_language\build\bin\Release\sad-pkg.exe search "package_name"
```

---

### خطأ: "Authentication required"

**السبب:** لم يتم تسجيل الدخول

**الحل:**
```powershell
# سجل الدخول أولاً
C:\s\s_language\build\bin\Release\sad-pkg.exe login
```

---

## Performance Testing / اختبار الأداء

```cpp
// benchmark_registry.cpp
#include "tools/pkg/registry_client.h"
#include <chrono>
#include <iostream>

int main() {
    using namespace sad::pkg;
    using namespace std::chrono;
    
    RegistryClient registry("http://localhost:8080");
    
    // Benchmark search
    auto start = high_resolution_clock::now();
    for (int i = 0; i < 100; i++) {
        registry.search("test", 10);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    
    std::cout << "100 عملية بحث: " << duration << " ms\n";
    std::cout << "متوسط: " << (duration / 100.0) << " ms/search\n";
    
    return 0;
}
```

---

## Next Steps / الخطوات التالية

1. ✓ تحديث registry_client.h لاستخدام JSON parsing
2. ✓ إنشاء API specification
3. ✓ إنشاء مثال Dart backend
4. ✓ إنشاء manifest schema
5. ✓ إنشاء دليل اختبار التكامل
6. ⏳ تنفيذ multipart upload في publish
7. ⏳ إضافة دعم التوقيعات (Ed25519)
8. ⏳ إضافة CI/CD pipeline
9. ⏳ إنشاء واجهة ويب للسجل (Flutter)
10. ⏳ نشر الخادم على cloud

---

الحمد لله رب العالمين
