# المرحلة 4: نظام الحزم
# Phase 4: Package System

**الحالة / Status:** 📅 مُخطط / Planned  
**الأولوية / Priority:** P1 (عالية / High)  
**الوقت المقدر / Estimated Time:** 3-4 أسابيع  
**المتطلبات السابقة / Prerequisites:** ✅ المرحلة 1، ✅ المرحلة 2

---

## 📋 نظرة عامة / Overview

### بالعربية
نظام حزم حديث لإدارة المكتبات الخارجية، مشابه لـ npm/cargo/pip. يوفر:
- **ملف manifest** (`sad.json`) لوصف المشروع
- **مدير حزم** (`sad-pkg`) لتثبيت/إزالة الحزم
- **مستودع مركزي** لاستضافة الحزم
- **Semantic Versioning** لإدارة الإصدارات
- **Lockfile** لضمان التناسق
- **Scripts** للبناء والنشر

### English
A modern package management system for external libraries, similar to npm/cargo/pip, providing:
- **Manifest file** (`sad.json`) for project description
- **Package manager** (`sad-pkg`) for install/remove packages
- **Central registry** for hosting packages
- **Semantic Versioning** for version management
- **Lockfile** for consistency
- **Scripts** for build and deployment

---

## 🎯 الأهداف / Goals

### Core Features
1. ⬜ ملف `sad.json` لوصف المشروع
2. ⬜ أداة CLI (`sad-pkg`)
3. ⬜ تثبيت/إزالة الحزم
4. ⬜ إدارة الاعتماديات التلقائية
5. ⬜ Lockfile (`sad-lock.json`)
6. ⬜ مستودع مركزي
7. ⬜ Scripts للمشروع

### Advanced Features
- ⬜ Workspaces (Monorepo)
- ⬜ Private packages
- ⬜ Package signing
- ⬜ Local registry mirror

---

## 📄 ملف sad.json / Project Manifest

### البنية الأساسية / Basic Structure

```json
{
  "name": "my-awesome-app",
  "version": "1.0.0",
  "description": "تطبيق رائع بلغة ص",
  "author": "محمد أحمد <mohamed@example.com>",
  "license": "MIT",
  
  "main": "src/main.s",
  "bin": {
    "my-app": "bin/cli.s"
  },
  
  "scripts": {
    "build": "sad compile src/main.s -o build/app",
    "test": "sad test tests/",
    "start": "sad run src/main.s",
    "clean": "rm -rf build/"
  },
  
  "dependencies": {
    "math-utils": "^2.0.0",
    "logger": "~1.5.0",
    "http-client": ">=3.0.0 <4.0.0"
  },
  
  "devDependencies": {
    "test-framework": "^1.0.0",
    "linter": "^0.8.0"
  },
  
  "sadLanguage": {
    "version": ">=1.0.0",
    "features": ["oop", "modules"]
  },
  
  "repository": {
    "type": "git",
    "url": "https://github.com/user/my-awesome-app"
  },
  
  "keywords": ["web", "api", "json"],
  
  "publishConfig": {
    "registry": "https://packages.sad-lang.org"
  }
}
```

---

### الحقول / Fields Explanation

| الحقل | Field | الوصف | Description |
|------|-------|-------|-------------|
| `name` | - | اسم الحزمة (فريد) | Package name (unique) |
| `version` | - | إصدار Semver | Semver version |
| `description` | - | وصف قصير | Short description |
| `main` | - | نقطة الدخول الرئيسية | Main entry point |
| `bin` | - | تنفيذيات CLI | CLI executables |
| `scripts` | - | أوامر مخصصة | Custom commands |
| `dependencies` | - | اعتماديات الإنتاج | Production dependencies |
| `devDependencies` | - | اعتماديات التطوير | Development dependencies |

---

## 🛠️ أداة CLI: sad-pkg

### الأوامر الأساسية / Basic Commands

```bash
# التهيئة / Initialize
sad-pkg init                    # إنشاء sad.json
sad-pkg init --yes              # بدون أسئلة

# التثبيت / Install
sad-pkg install                 # تثبيت كل الاعتماديات
sad-pkg install math-utils      # تثبيت حزمة محددة
sad-pkg install math-utils@2.0.0  # إصدار محدد
sad-pkg install math-utils --save-dev  # كـ devDependency

# الإزالة / Uninstall
sad-pkg uninstall math-utils    # حذف حزمة

# التحديث / Update
sad-pkg update                  # تحديث كل الحزم
sad-pkg update math-utils       # تحديث حزمة محددة

# البحث / Search
sad-pkg search http             # البحث في المستودع
sad-pkg info math-utils         # معلومات حزمة

# النشر / Publish
sad-pkg publish                 # نشر الحزمة
sad-pkg unpublish               # إلغاء النشر

# Scripts
sad-pkg run build               # تشغيل script
sad-pkg run test                # تشغيل الاختبارات

# الأدوات / Utils
sad-pkg list                    # عرض الحزم المثبتة
sad-pkg outdated                # عرض الحزم القديمة
sad-pkg audit                   # فحص الأمان
sad-pkg clean                   # مسح الكاش
```

---

### بنية المجلدات / Directory Structure

```
my-project/
├── sad.json                    # Manifest
├── sad-lock.json               # Lockfile
├── src/
│   ├── main.s
│   └── lib/
├── tests/
├── build/
└── sad_modules/                # الحزم المثبتة
    ├── math-utils/
    │   ├── sad.json
    │   ├── src/
    │   └── README.md
    └── logger/
        ├── sad.json
        └── src/
```

---

## 🔒 Lockfile: sad-lock.json

### الغرض / Purpose

- **ثبات الإصدارات** عبر جميع البيئات
- **تسريع التثبيت** (لا حاجة لحل الاعتماديات)
- **أمان أفضل** (checksums)

---

### البنية / Structure

```json
{
  "lockfileVersion": 1,
  "generated": "2025-12-08T10:30:00Z",
  "packages": {
    "math-utils": {
      "version": "2.1.5",
      "resolved": "https://registry.sad-lang.org/math-utils/-/math-utils-2.1.5.tar.gz",
      "integrity": "sha512-abc123...",
      "dependencies": {
        "core-types": "^1.0.0"
      }
    },
    "logger": {
      "version": "1.5.3",
      "resolved": "https://registry.sad-lang.org/logger/-/logger-1.5.3.tar.gz",
      "integrity": "sha256-def456...",
      "dependencies": {}
    },
    "core-types": {
      "version": "1.2.0",
      "resolved": "https://registry.sad-lang.org/core-types/-/core-types-1.2.0.tar.gz",
      "integrity": "sha256-ghi789..."
    }
  }
}
```

---

## 🌐 المستودع المركزي / Central Registry

### البنية المعمارية / Architecture

```
┌─────────────────────────────────────────┐
│     packages.sad-lang.org               │
│  ┌────────────────────────────────────┐ │
│  │    Web Frontend (Search/Browse)    │ │
│  └────────────┬───────────────────────┘ │
│               │                          │
│  ┌────────────▼───────────────────────┐ │
│  │        API Server (REST)           │ │
│  │  - Publish                         │ │
│  │  - Search                          │ │
│  │  - Download                        │ │
│  │  - Metadata                        │ │
│  └────────────┬───────────────────────┘ │
│               │                          │
│  ┌────────────▼───────────────────────┐ │
│  │      Database (PostgreSQL)         │ │
│  │  - Package metadata                │ │
│  │  - Users                           │ │
│  │  - Downloads stats                 │ │
│  └────────────────────────────────────┘ │
│                                          │
│  ┌──────────────────────────────────┐   │
│  │    CDN / Storage (S3/MinIO)      │   │
│  │  - Package tarballs              │   │
│  └──────────────────────────────────┘   │
└─────────────────────────────────────────┘
```

---

### API Endpoints

```
GET  /api/v1/packages                  # قائمة الحزم
GET  /api/v1/packages/:name            # معلومات حزمة
GET  /api/v1/packages/:name/:version   # إصدار محدد
POST /api/v1/packages                  # نشر حزمة
DELETE /api/v1/packages/:name/:version # حذف

GET  /api/v1/search?q=http             # بحث
GET  /api/v1/stats/:name               # إحصائيات
```

---

## 💻 التنفيذ / Implementation

### 1. Package Manager Core

**ملف:** `src/pkg/package_manager.h`

```cpp
// include/pkg/package_manager.h
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "pkg/manifest.h"
#include "pkg/registry.h"

namespace Sad {
namespace Pkg {

/**
 * مدير الحزم / Package Manager
 */
class PackageManager {
public:
    /**
     * تهيئة مشروع جديد
     * Initialize new project
     */
    void init(const std::string& projectPath, bool interactive = true);
    
    /**
     * تثبيت اعتماديات
     * Install dependencies
     */
    void install(const std::string& projectPath);
    
    /**
     * تثبيت حزمة محددة
     * Install specific package
     */
    void installPackage(const std::string& packageName,
                       const std::string& versionSpec,
                       bool isDev = false);
    
    /**
     * إزالة حزمة
     * Uninstall package
     */
    void uninstallPackage(const std::string& packageName);
    
    /**
     * تحديث حزم
     * Update packages
     */
    void update(const std::vector<std::string>& packageNames = {});
    
    /**
     * نشر حزمة
     * Publish package
     */
    void publish(const std::string& projectPath);
    
    /**
     * البحث عن حزم
     * Search packages
     */
    std::vector<PackageInfo> search(const std::string& query);
    
private:
    std::unique_ptr<Registry> registry_;
    std::unique_ptr<DependencyResolver> resolver_;
    
    Manifest loadManifest(const std::string& path);
    void saveManifest(const std::string& path, const Manifest& manifest);
    
    Lockfile loadLockfile(const std::string& path);
    void saveLockfile(const std::string& path, const Lockfile& lockfile);
    
    void downloadPackage(const PackageInfo& pkg, 
                        const std::string& destination);
    void extractPackage(const std::string& tarball,
                       const std::string& destination);
    
    bool verifyChecksum(const std::string& file,
                       const std::string& expectedHash);
};

} // namespace Pkg
} // namespace Sad
```

---

### 2. Manifest Parser

```cpp
// include/pkg/manifest.h
#pragma once

#include <string>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Sad {
namespace Pkg {

/**
 * بيانات المشروع / Project Manifest
 */
struct Manifest {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string main;
    
    std::map<std::string, std::string> bin;
    std::map<std::string, std::string> scripts;
    std::map<std::string, std::string> dependencies;
    std::map<std::string, std::string> devDependencies;
    
    struct Repository {
        std::string type;
        std::string url;
    } repository;
    
    std::vector<std::string> keywords;
    
    static Manifest parse(const std::string& jsonContent);
    std::string toJson() const;
    
    bool validate() const;
};

} // namespace Pkg
} // namespace Sad
```

---

### 3. Dependency Resolver

```cpp
// include/pkg/dependency_resolver.h
#pragma once

#include <string>
#include <vector>
#include <map>
#include "pkg/version.h"

namespace Sad {
namespace Pkg {

/**
 * حل الاعتماديات / Dependency Resolver
 */
class DependencyResolver {
public:
    /**
     * حل شجرة الاعتماديات
     * Resolve dependency tree
     * 
     * @param dependencies خريطة الاعتماديات
     * @return قائمة الحزم المرتبة للتثبيت
     */
    std::vector<ResolvedPackage> resolve(
        const std::map<std::string, std::string>& dependencies);
    
    /**
     * كشف التعارضات
     * Detect conflicts
     */
    std::vector<Conflict> detectConflicts();
    
private:
    struct DependencyNode {
        std::string name;
        Version version;
        std::map<std::string, std::string> dependencies;
    };
    
    std::map<std::string, DependencyNode> dependencyGraph_;
    
    void buildGraph(const std::string& packageName,
                   const std::string& versionSpec);
    
    std::vector<ResolvedPackage> topologicalSort();
};

/**
 * حزمة محلولة / Resolved Package
 */
struct ResolvedPackage {
    std::string name;
    Version version;
    std::string url;
    std::string checksum;
};

/**
 * تعارض / Conflict
 */
struct Conflict {
    std::string packageName;
    std::vector<std::string> requiredBy;
    std::vector<std::string> conflictingVersions;
};

} // namespace Pkg
} // namespace Sad
```

---

## 🧪 الاختبارات / Testing

### Test Cases

```cpp
/**
 * اختبار 1: تحليل Manifest
 */
TEST(PackageSystem, ManifestParsing) {
    std::string jsonContent = R"({
        "name": "test-pkg",
        "version": "1.0.0",
        "dependencies": {
            "math-utils": "^2.0.0"
        }
    })";
    
    auto manifest = Manifest::parse(jsonContent);
    
    EXPECT_EQ(manifest.name, "test-pkg");
    EXPECT_EQ(manifest.version, "1.0.0");
    EXPECT_EQ(manifest.dependencies.size(), 1);
}

/**
 * اختبار 2: حل الاعتماديات
 */
TEST(PackageSystem, DependencyResolution) {
    DependencyResolver resolver;
    
    std::map<std::string, std::string> deps = {
        {"pkg-a", "^1.0.0"},
        {"pkg-b", "~2.5.0"}
    };
    
    auto resolved = resolver.resolve(deps);
    
    EXPECT_GT(resolved.size(), 0);
}

/**
 * اختبار 3: كشف التعارضات
 */
TEST(PackageSystem, ConflictDetection) {
    // pkg-a يحتاج pkg-c@1.0.0
    // pkg-b يحتاج pkg-c@2.0.0
    // يجب كشف التعارض
    
    // ...
}
```

---

## 📚 أمثلة عملية / Practical Examples

### مثال 1: إنشاء مشروع جديد

```bash
$ mkdir my-app && cd my-app
$ sad-pkg init

Package name: (my-app) 
Version: (1.0.0) 
Description: تطبيق ويب بلغة ص
Entry point: (src/main.s) 
Author: محمد أحمد
License: (MIT) 

Created sad.json
```

---

### مثال 2: تثبيت اعتماديات

```bash
$ sad-pkg install http-server logger

✓ Resolving dependencies...
✓ Fetching packages...
  ├─ http-server@3.2.1
  ├─ logger@1.5.0
  └─ core-types@1.0.5 (peer dependency)

✓ Installed 3 packages in 2.3s
```

---

### مثال 3: نشر حزمة

```bash
$ sad-pkg publish

Preparing to publish my-awesome-lib@1.0.0

✓ Running tests...
✓ Building package...
✓ Generating checksums...
✓ Uploading to registry...

Published successfully!
View at: https://packages.sad-lang.org/my-awesome-lib
```

---

## 📊 مؤشرات الأداء / Performance Metrics

- ⚡ **وقت حل الاعتماديات:** < 500ms لـ 50 حزمة
- ⚡ **سرعة التحميل:** تعتمد على الشبكة
- ⚡ **سرعة الاستخراج:** < 100ms لكل حزمة
- ✅ **دقة حل التعارضات:** 100%
- 💾 **حجم الكاش:** يُدار تلقائياً

---

## ✅ قائمة المهام / Task Checklist

### الأسبوع 1
- [ ] تنفيذ `Manifest` parser
- [ ] تنفيذ `Lockfile` handler
- [ ] CLI الأساسي (`sad-pkg`)

### الأسبوع 2
- [ ] `DependencyResolver`
- [ ] حل التعارضات
- [ ] Download/Extract logic

### الأسبوع 3
- [ ] Registry client
- [ ] Publish workflow
- [ ] Authentication

### الأسبوع 4
- [ ] Scripts runner
- [ ] Caching system
- [ ] اختبارات شاملة
- [ ] التوثيق

---

## 🚀 الخطوة التالية / Next Step

بعد إكمال هذه المرحلة، انتقل إلى:
**[المرحلة 5: FFI المتقدم](phase5_ffi_advanced.md)**

---

**آخر تحديث / Last Updated:** 8 ديسمبر 2025
