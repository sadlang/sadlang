# بسم الله الرحمن الرحيم

# Sad Language Package Registry Integration
# تكامل سجل حزم لغة ص

**الحمد لله رب العالمين**

---

## Overview / نظرة عامة

هذا المستودع يحتوي على نظام متكامل لإدارة حزم لغة ص، يتضمن:

- 🔧 **مدير الحزم (sad-pkg)**: أداة CLI لإدارة الحزم
- 🌐 **عميل السجل**: مكتبة C++ للاتصال بخادم السجل
- 📡 **واجهة API**: مواصفات REST API للتواصل
- 🖥️ **مثال خادم Dart**: مثال كامل لخادم backend
- 📄 **توثيق شامل**: دليل كامل للاستخدام والتطوير

---

## Quick Start / البدء السريع

### 1. بناء مدير الحزم

```powershell
# استنساخ المستودع
git clone https://github.com/sad-lang/sad.git
cd sad

# بناء المشروع مع مدير الحزم
mkdir build
cd build
cmake .. -DBUILD_PKG_MANAGER=ON
cmake --build . --config Release
```

### 2. استخدام مدير الحزم

```powershell
# إنشاء مشروع جديد
sad-pkg init

# إضافة تبعية
sad-pkg add http ^1.2.0

# تثبيت التبعيات
sad-pkg install

# البحث عن حزم
sad-pkg search "json"

# عرض معلومات حزمة
sad-pkg info http
```

---

## Documentation / التوثيق

### 📚 مستندات للمستخدمين

| المستند | الوصف | الرابط |
|---------|--------|--------|
| **Package Manager README** | دليل استخدام مدير الحزم | [tools/pkg/README.md](../tools/pkg/README.md) |
| **Manifest Schema** | مخطط ملف sad.toml | [PKG_MANIFEST_SCHEMA.md](PKG_MANIFEST_SCHEMA.md) |
| **Integration Testing** | دليل اختبار التكامل | [PKG_INTEGRATION_TEST.md](PKG_INTEGRATION_TEST.md) |

### 🛠️ مستندات للمطورين

| المستند | الوصف | الرابط |
|---------|--------|--------|
| **API Specification** | مواصفات REST API | [PKG_REGISTRY_API.md](PKG_REGISTRY_API.md) |
| **Dart Backend Example** | مثال خادم Dart | [DART_BACKEND_EXAMPLE.md](DART_BACKEND_EXAMPLE.md) |
| **Registry Client** | مكتبة عميل السجل | [tools/pkg/registry_client.h](../tools/pkg/registry_client.h) |

---

## Architecture / المعمارية

```
┌─────────────────────────────────────────────────────────────┐
│                    Sad Language Compiler                    │
│                         مترجم لغة ص                        │
└────────────────────────┬────────────────────────────────────┘
                         │
                         │ يستخدم / uses
                         ▼
┌─────────────────────────────────────────────────────────────┐
│                  Package Manager (sad-pkg)                  │
│                     مدير الحزم (sad-pkg)                    │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │     CLI      │  │   Resolver   │  │    Cache     │      │
│  │   واجهة CLI │  │  محلل التبعيات│  │ ذاكرة مؤقتة │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└────────────────────────┬────────────────────────────────────┘
                         │
                         │ HTTP/REST
                         ▼
┌─────────────────────────────────────────────────────────────┐
│              Registry Client (C++ Library)                  │
│                 عميل السجل (مكتبة C++)                      │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │ HTTP Client  │  │ JSON Parser  │  │  Validator   │      │
│  │  عميل HTTP   │  │  محلل JSON   │  │   محقق       │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└────────────────────────┬────────────────────────────────────┘
                         │
                         │ HTTPS/TLS
                         ▼
┌─────────────────────────────────────────────────────────────┐
│           Package Registry Server (Dart/Flutter)            │
│              خادم سجل الحزم (Dart/Flutter)                  │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │  REST API    │  │   Database   │  │   Storage    │      │
│  │ واجهة REST   │  │ قاعدة بيانات│  │    تخزين     │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐      │
│  │     Auth     │  │    Search    │  │   Analytics  │      │
│  │   مصادقة     │  │     بحث      │  │   تحليلات    │      │
│  └──────────────┘  └──────────────┘  └──────────────┘      │
└─────────────────────────────────────────────────────────────┘
```

---

## Features / المزايا

### ✅ المزايا المنفذة

- [x] مدير حزم كامل (sad-pkg CLI)
- [x] حل التبعيات التلقائي
- [x] دعم Semantic Versioning
- [x] ذاكرة تخزين مؤقت محلية
- [x] عميل HTTP لـ REST API
- [x] تحليل JSON باستخدام nlohmann/json
- [x] البحث عن الحزم
- [x] تنزيل وتثبيت الحزم
- [x] عرض معلومات الحزم
- [x] دعم التبعيات المتداخلة
- [x] كشف تعارض الإصدارات

### ⏳ المزايا قيد التطوير

- [ ] نشر الحزم (multipart upload)
- [ ] توقيع الحزم (Ed25519)
- [ ] التحقق من التوقيعات
- [ ] تحديث تلقائي للتبعيات
- [ ] دعم features الاختيارية
- [ ] بناء متعدد المنصات
- [ ] CI/CD integration
- [ ] واجهة ويب للسجل

---

## API Endpoints / نقاط API

### 🔍 البحث والاستعلام

```http
GET /api/v1/search?q={query}&limit={limit}
GET /api/v1/packages/{name}
GET /api/v1/packages/{name}/versions
GET /api/v1/packages/{name}/{version}
```

### 📥 التنزيل

```http
GET /api/v1/packages/{name}/{version}/download
```

### 📤 النشر

```http
POST /api/v1/packages/publish
POST /api/v1/auth/login
```

### 🔐 الإدارة

```http
GET /api/v1/user/packages
DELETE /api/v1/packages/{name}/{version}/yank
PUT /api/v1/packages/{name}/{version}/unyank
```

للتفاصيل الكاملة، راجع [PKG_REGISTRY_API.md](PKG_REGISTRY_API.md)

---

## Example Usage / أمثلة الاستخدام

### إنشاء مشروع جديد

```powershell
# إنشاء مشروع
mkdir my_project
cd my_project
sad-pkg init

# إضافة تبعيات
sad-pkg add http ^1.2.0
sad-pkg add json ~2.1.0

# تثبيت
sad-pkg install
```

### ملف sad.toml

```toml
[package]
name = "my-project"
version = "1.0.0"
description = "My awesome project"
description_ar = "مشروعي الرائع"
authors = ["أحمد محمد <ahmad@example.com>"]
license = "MIT"

[dependencies]
http = "^1.2.0"
json = "~2.1.0"
```

### استخدام الحزم في الكود

```sad
# استيراد الحزم
استورد http من "http"
استورد json من "json"

# استخدام الدوال
دالة main() {
    متغير response = http.get("https://api.example.com/data")
    متغير data = json.parse(response.body)
    
    اطبع(data["message"])
}
```

---

## Development Setup / إعداد بيئة التطوير

### متطلبات البناء

- CMake 3.15+
- C++17 compiler (MSVC 2019+, GCC 9+, Clang 10+)
- libcurl
- nlohmann/json (يتم تنزيله تلقائياً)

### بناء المشروع

```powershell
# Windows
mkdir build
cd build
cmake .. -DBUILD_PKG_MANAGER=ON
cmake --build . --config Release

# Linux/Mac
mkdir build && cd build
cmake .. -DBUILD_PKG_MANAGER=ON
make -j$(nproc)
```

### تشغيل الاختبارات

```powershell
# اختبار الوحدات
ctest --output-on-failure

# اختبار التكامل
.\docs\test_integration.ps1
```

---

## Server Setup / إعداد الخادم

### خادم Dart (مُوصى به)

```bash
# إنشاء مشروع
mkdir sad_registry_server
cd sad_registry_server

# تهيئة المشروع
dart create -t server-shelf .

# إضافة التبعيات
# راجع DART_BACKEND_EXAMPLE.md للتفاصيل

# تشغيل الخادم
dart run bin/server.dart
```

### قاعدة البيانات (PostgreSQL)

```sql
-- إنشاء قاعدة البيانات
CREATE DATABASE sad_registry;

-- إنشاء الجداول
-- راجع DART_BACKEND_EXAMPLE.md للمخطط الكامل
```

---

## Configuration / الإعدادات

### ملف الإعدادات

**Windows:** `C:\Users\<YourName>\.sad\config.toml`  
**Unix/Linux/Mac:** `~/.sad/config.toml`

```toml
[registry]
url = "https://packages.sadlang.org"
# أو للتطوير المحلي:
# url = "http://localhost:8080"

[auth]
token = ""

[cache]
path = "~/.sad/cache"
max_size = "1GB"

[build]
parallel = true
jobs = 8
```

---

## Contributing / المساهمة

نرحب بمساهماتكم! يرجى قراءة دليل المساهمة:

1. Fork المستودع
2. إنشاء فرع للميزة (`git checkout -b feature/amazing-feature`)
3. Commit التغييرات (`git commit -m 'إضافة ميزة رائعة'`)
4. Push إلى الفرع (`git push origin feature/amazing-feature`)
5. فتح Pull Request

---

## Testing / الاختبار

### اختبارات الوحدات

```cpp
// tests/pkg/test_version.cpp
#include "tools/pkg/package.h"
#include <gtest/gtest.h>

TEST(VersionTest, ParseValid) {
    auto v = Version::parse("1.2.3");
    EXPECT_EQ(v.major, 1);
    EXPECT_EQ(v.minor, 2);
    EXPECT_EQ(v.patch, 3);
}
```

### اختبارات التكامل

```powershell
# تشغيل جميع الاختبارات
.\docs\test_integration.ps1
```

---

## Troubleshooting / استكشاف الأخطاء

### مشكلة: "CURL not found"

```powershell
# تثبيت CURL عبر vcpkg
vcpkg install curl:x64-windows
```

### مشكلة: "Failed to connect to registry"

```powershell
# تحقق من تشغيل الخادم
curl http://localhost:8080/health
```

### مشكلة: "Package not found"

```powershell
# تحقق من توفر الحزمة
sad-pkg search "package_name"
```

---

## Performance / الأداء

### معايير الأداء

- **البحث**: < 50ms (متوسط)
- **تنزيل حزمة صغيرة (< 1MB)**: < 500ms
- **حل التبعيات (10 حزم)**: < 200ms
- **التثبيت (10 حزم)**: < 5s

---

## Security / الأمان

### التوقيعات

جميع الحزم يجب أن تكون موقعة باستخدام Ed25519:

```bash
# إنشاء مفتاح
sad-pkg key generate

# توقيع حزمة
sad-pkg sign my-package-1.0.0.tar.gz

# التحقق من التوقيع
sad-pkg verify my-package-1.0.0.tar.gz
```

### HTTPS

جميع الاتصالات مع السجل يجب أن تستخدم HTTPS/TLS.

---

## Roadmap / خارطة الطريق

### Q1 2026

- [x] إصدار مدير الحزم v1.0
- [x] مواصفات API v1
- [x] مثال خادم Dart
- [ ] إطلاق سجل رسمي

### Q2 2026

- [ ] واجهة ويب للسجل
- [ ] دعم المصادقة OAuth
- [ ] CI/CD integration
- [ ] تحليلات استخدام الحزم

### Q3 2026

- [ ] دعم Private registries
- [ ] Mirror support
- [ ] CDN integration
- [ ] Mobile app

---

## License / الترخيص

هذا المشروع مرخص تحت رخصة MIT - راجع ملف [LICENSE](../LICENSE) للتفاصيل.

---

## Credits / الاعتمادات

- **Sad Language Team** - التطوير الأساسي
- **Contributors** - المساهمون في المشروع
- **nlohmann/json** - مكتبة JSON parser
- **libcurl** - مكتبة HTTP client

---

## Contact / الاتصال

- **Website:** https://sadlang.org
- **Email:** support@sadlang.org
- **GitHub:** https://github.com/sad-lang/sad
- **Discord:** https://discord.gg/sadlang

---

## Resources / المصادر

### Documentation / التوثيق

- [Package Manager Guide](../tools/pkg/README.md)
- [API Specification](PKG_REGISTRY_API.md)
- [Manifest Schema](PKG_MANIFEST_SCHEMA.md)
- [Integration Testing](PKG_INTEGRATION_TEST.md)
- [Dart Backend Example](DART_BACKEND_EXAMPLE.md)

### Tools / الأدوات

- **sad-pkg**: Package manager CLI
- **sad**: Compiler
- **sad-lsp**: Language Server
- **sad-fmt**: Code formatter
- **sad-doc**: Documentation generator

---

**بسم الله الرحمن الرحيم**  
**الحمد لله رب العالمين**
