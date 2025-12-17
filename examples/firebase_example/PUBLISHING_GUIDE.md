# 🚀 Firebase Example Package - Publishing Guide
# دليل نشر حزمة مثال Firebase

> **الحمد لله رب العالمين** - بسم الله الرحمن الرحيم

## 📋 Table of Contents | محتويات الدليل

1. [Overview](#overview-نظرة-عامة)
2. [Prerequisites](#prerequisites-المتطلبات)
3. [Package Structure](#package-structure-هيكل-الحزمة)
4. [Publishing Process](#publishing-process-عملية-النشر)
5. [Verification](#verification-التحقق)
6. [Installation Guide](#installation-guide-دليل-التثبيت)
7. [Troubleshooting](#troubleshooting-استكشاف-الأخطاء)

---

## Overview | نظرة عامة

هذا الدليل يشرح كيفية نشر حزمة **firebase-example** على مستودع حزم لغة Sad الرسمي.

This guide explains how to publish the **firebase-example** package to the official Sad Language package repository.

### Package Details | تفاصيل الحزمة

```toml
[package]
name = "firebase-example"
version = "1.0.0"
description = "Firebase integration example for Sad Language"
description_ar = "مثال تكامل Firebase مع لغة ص"
authors = ["Sad Language Team <dev@sadlang.org>"]
license = "MIT"
repository = "https://github.com/sadlang/firebase-example"
```

---

## Prerequisites | المتطلبات

### Required Tools | الأدوات المطلوبة

```bash
# Sad Language Package Manager
sad-pkg --version
# Output: sad-pkg version 2.0.0

# Git (for version control)
git --version
# Output: git version 2.40.0

# CMake (for building)
cmake --version
# Output: cmake version 3.26.0
```

### API Token | رمز الوصول

1. Sign up/Login to: https://packages.sadlang.org
2. Go to Settings > API Tokens
3. Create new token with `publish` scope
4. Copy and save securely

```bash
export SAD_PKG_TOKEN="your_api_token_here"
```

---

## Package Structure | هيكل الحزمة

```
firebase-example/
├── sad.toml                 # Package manifest
├── package_registry.s       # Sad Language registry implementation
├── main.s                   # Main example code
├── firebase.sad             # Firebase module implementation
├── README.md                # Documentation
├── LICENSE                  # MIT License
├── firestore.rules          # Firestore security rules
├── storage.rules            # Cloud Storage rules
└── examples/
    ├── basic_usage.s        # Basic usage example
    ├── advanced_usage.s     # Advanced usage example
    └── firebase_config.s    # Firebase configuration
```

### File Descriptions | وصف الملفات

#### **sad.toml** - Package Manifest
الملف الرئيسي الذي يحتوي على معلومات الحزمة

```toml
[package]
name = "firebase-example"
version = "1.0.0"
description = "Firebase integration example for Sad Language"
description_ar = "مثال تكامل Firebase مع لغة ص"
authors = ["Sad Language Team <dev@sadlang.org>"]
license = "MIT"
keywords = ["firebase", "integration", "example", "firestore", "storage"]
repository = "https://github.com/sadlang/firebase-example"
documentation = "https://sadlang.org/docs/firebase-example"
homepage = "https://sadlang.org"

[dependencies]
firebase = "^1.0.0"
json = "^2.0.0"
http = "^1.5.0"

[features]
firestore-support = true
storage-support = true
authentication = false

[metadata]
stability = "stable"
maintenance = "actively-developed"
```

#### **package_registry.s** - Registry Implementation
تنفيذ سجل الحزم في لغة Sad

```sad
// تصنيف الحزم
فئة PackageRegistry {
    // نشر حزمة جديدة
    دالة public نشر(packagePath, metadata) -> نتيجة {
        // تحميل الحزمة على Firestore
        // حفظ البيانات الوصفية
        // إرجاع معرف الحزمة
    }
    
    // البحث عن الحزم
    دالة public ابحث(query, limit=10) -> قائمة[الحزمة] {
        // البحث في Firestore
        // تصفية النتائج
    }
    
    // تنزيل حزمة
    دالة public نزل(name, version, destPath) {
        // تنزيل من Cloud Storage
        // التحقق من التكامل
        // فك الضغط
    }
}
```

#### **main.s** - Main Example
مثال الاستخدام الرئيسي

```sad
استورد firebase من "firebase"؛
استورد registry من "firebase-example"؛

// إنشاء مسجل الحزم
متغير pkg_registry = registry.create_registry()؛

// نشر حزمة
متغير result = pkg_registry.publish(
    "path/to/package",
    {
        name: "my-package",
        version: "1.0.0",
        description: "My awesome package"
    }
)؛

// البحث عن حزم
متغير packages = pkg_registry.search("firebase", limit=10)؛

// الحصول على معلومات الحزمة
متغير info = pkg_registry.get_package_info("firebase-example", "1.0.0")؛
```

---

## Publishing Process | عملية النشر

### Step 1: Validation | الخطوة 1: التحقق

```bash
# Validate package structure
sad-pkg validate

# Expected output:
# ✓ sad.toml found
# ✓ Required files present
# ✓ Manifest syntax valid
# ✓ All dependencies resolvable
# ✓ Validation passed!
```

### Step 2: Authentication | الخطوة 2: المصادقة

```bash
# Login to registry
sad-pkg login --token $SAD_PKG_TOKEN

# Verify authentication
sad-pkg whoami

# Expected output:
# Username: sadlang
# Email: dev@sadlang.org
# Status: active
# Scopes: publish, yank, manage-team
```

### Step 3: Prepare Release | الخطوة 3: تحضير الإصدار

```bash
# Update version if needed
sad-pkg version patch  # 1.0.0 -> 1.0.1
sad-pkg version minor  # 1.0.0 -> 1.1.0
sad-pkg version major  # 1.0.0 -> 2.0.0

# Commit version changes
git add sad.toml
git commit -m "Bump version to 1.0.0"
git tag -a v1.0.0 -m "Release version 1.0.0"
```

### Step 4: Package Build | الخطوة 4: بناء الحزمة

```bash
# Create package archive
sad-pkg pack

# Output:
# Creating archive: firebase-example-1.0.0.sad.tar.gz
# Files: 23
# Size: 2.3 MB
# Checksum: sha256:abc123...
# ✓ Package archive created
```

### Step 5: Publish | الخطوة 5: النشر

```bash
# Publish package
sad-pkg publish

# Interactive prompt:
# ? Package name: firebase-example
# ? Version: 1.0.0
# ? Description: Firebase integration example...
# ? Confirm publish? (y/n) y

# Processing:
# Uploading package... [████████████████████] 100%
# Creating Firestore document...
# Indexing in registry...
# Generating documentation...
# Notifying subscribers...

# Success:
# ✅ Package published successfully!
# Package ID: pkg_1a2b3c4d5e6f7g8h
# Published at: 2025-12-03T14:30:00Z
# Registry URL: https://packages.sadlang.org/packages/firebase-example
```

---

## Verification | التحقق

### Verify Package is Published | التحقق من نشر الحزمة

```bash
# Get package info
sad-pkg info firebase-example

# Output:
# Name: firebase-example
# Latest Version: 1.0.0
# License: MIT
# Authors: Sad Language Team <dev@sadlang.org>
# Published: 2025-12-03T14:30:00Z
# Downloads: 42
# Stars: 5
```

### Search for Package | البحث عن الحزمة

```bash
# Search in registry
sad-pkg search firebase

# Output:
# firebase-example (1.0.0) ⭐ 5
#   Firebase integration example for Sad Language
#   by Sad Language Team
#
# firebase (1.0.5) ⭐ 23
#   Official Firebase SDK for Sad Language
#   by Google
```

### View Package Details | عرض تفاصيل الحزمة

```bash
# Show detailed package info
sad-pkg show firebase-example@1.0.0

# Output:
# Name: firebase-example
# Version: 1.0.0
# Stability: stable
# Maintenance: actively-developed
# 
# Description:
#   Firebase integration example for Sad Language
#   مثال تكامل Firebase مع لغة ص
#
# Authors:
#   - Sad Language Team <dev@sadlang.org>
#
# Dependencies:
#   - firebase ^1.0.0
#   - json ^2.0.0
#   - http ^1.5.0
#
# Files: 23
# Size: 2.3 MB
# Checksum: sha256:abc123...
#
# URLs:
#   Homepage: https://sadlang.org
#   Repository: https://github.com/sadlang/firebase-example
#   Documentation: https://sadlang.org/docs/firebase-example
#   Issues: https://github.com/sadlang/firebase-example/issues
```

---

## Installation Guide | دليل التثبيت

### For Users | للمستخدمين

#### Method 1: Using sad-pkg | الطريقة 1: استخدام sad-pkg

```bash
# Add to sad.toml
cat >> sad.toml << 'EOF'

[dependencies]
firebase-example = "^1.0.0"
EOF

# Install dependencies
sad-pkg install

# View installed packages
sad-pkg list
```

#### Method 2: Direct Installation | الطريقة 2: التثبيت المباشر

```bash
# Install specific package
sad-pkg install firebase-example

# Install specific version
sad-pkg install firebase-example@1.0.0

# Install with features
sad-pkg install firebase-example --features=firestore-support,storage-support
```

### Usage in Code | الاستخدام في الكود

```sad
// Import the package
استورد firebase_example من "firebase-example"؛

// Create registry instance
متغير registry = firebase_example.create_registry()؛

// Publish a new package
متغير result = registry.publish(
    "path/to/my-package",
    {
        name: "my-awesome-package",
        version: "1.0.0",
        description: "My awesome Sad Language package",
        authors: ["Me <me@example.com>"]
    }
)؛

// Check result
اذا (result.success) {
    اطبع("✅ Package published: " + result.package_id)؛
} آخر {
    اطبع("❌ Error: " + result.error)؛
}
```

---

## Troubleshooting | استكشاف الأخطاء

### Common Issues | المشاكل الشائعة

#### Issue 1: Invalid Token | المشكلة 1: رمز غير صحيح

```bash
# Error:
# ❌ ERROR: Invalid or expired authentication token

# Solution:
sad-pkg login --token $NEW_TOKEN
sad-pkg whoami  # Verify new token
```

#### Issue 2: Dependency Resolution | المشكلة 2: حل المتطلبات

```bash
# Error:
# ❌ ERROR: Cannot resolve dependency: firebase@^1.0.0

# Solution:
sad-pkg search firebase     # Find available versions
sad-pkg show firebase@1.5.0 # Check specific version
# Update sad.toml with available version
sad-pkg deps check          # Verify dependencies
```

#### Issue 3: Network Issues | المشكلة 3: مشاكل الشبكة

```bash
# Error:
# ❌ ERROR: Connection timeout to packages.sadlang.org

# Solution:
# Check network connection
ping packages.sadlang.org

# Try again with retry
sad-pkg publish --retry 5

# Check registry status
sad-pkg registry status
```

#### Issue 4: Package Already Exists | المشكلة 4: الحزمة موجودة بالفعل

```bash
# Error:
# ❌ ERROR: Package firebase-example@1.0.0 already exists

# Solution:
# Increment version number
sad-pkg version patch  # 1.0.0 -> 1.0.1

# Or yank old version (admin only)
sad-pkg yank firebase-example@1.0.0
```

---

## Advanced Topics | مواضيع متقدمة

### Publish with GitHub Release | النشر مع إصدار GitHub

```bash
# Create GitHub release
sad-pkg release create-github \
    --owner sadlang \
    --repo firebase-example \
    --version 1.0.0 \
    --notes "Initial release of Firebase integration example"

# Generate changelog
sad-pkg changelog generate --from v0.0.0 --to v1.0.0
```

### Publish with CI/CD | النشر عبر CI/CD

```yaml
# .github/workflows/publish.yml
name: Publish Package

on:
  release:
    types: [created]

jobs:
  publish:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - uses: sadlang/setup-sad@v2
      - run: sad-pkg validate
      - run: sad-pkg publish
        env:
          SAD_PKG_TOKEN: ${{ secrets.SAD_PKG_TOKEN }}
```

### Maintain Multiple Versions | الحفاظ على نسخ متعددة

```bash
# Create maintenance branch
git checkout -b maintenance/1.0

# Make bug fixes
# ...

# Bump patch version
sad-pkg version patch  # 1.0.0 -> 1.0.1

# Publish patch
sad-pkg publish

# Return to main
git checkout main
```

---

## Statistics & Analytics | الإحصائيات والتحليلات

### Track Package Performance | تتبع أداء الحزمة

```bash
# Get package statistics
sad-pkg stats firebase-example

# Output:
# Downloads:
#   Total: 1,234
#   Monthly: 456
#   Weekly: 89
#
# Engagement:
#   Stars: 23
#   Forks: 5
#   Issues: 2
#   Pull Requests: 1
#
# Users:
#   Unique Installs: 234
#   Current Users: 156
#   User Retention: 67%
```

---

## Community & Support | المجتمع والدعم

### Share with Community | شارك مع المجتمع

- [Sad Language Discord](https://discord.sadlang.org)
- [GitHub Discussions](https://github.com/sadlang/firebase-example/discussions)
- [Stack Overflow](https://stackoverflow.com/questions/tagged/sad-language)

### Get Help | احصل على المساعدة

```bash
# View help
sad-pkg publish --help

# Check documentation
sad-pkg docs view firebase-example

# Report issues
sad-pkg issues report firebase-example

# Contact support
sad-pkg support contact --issue "Publishing failed"
```

---

## Conclusion | الخلاصة

🎉 **Congratulations!** You've successfully published your Firebase example package to the Sad Language package registry!

🎉 **مبروك!** لقد نجحت في نشر حزمة مثال Firebase على سجل حزم لغة Sad بنجاح!

### Next Steps | الخطوات التالية

1. ✅ Monitor package downloads and user feedback
2. ✅ Update documentation as needed
3. ✅ Release new versions with improvements
4. ✅ Engage with community users
5. ✅ Consider contributing to the Sad Language ecosystem

---

## 🙏 Gratitude | الشكر والتقدير

**الحمد لله رب العالمين** على إتمام هذا المشروع

Thank you for contributing to the Sad Language ecosystem!

شكراً لمساهمتك في بناء مجتمع لغة Sad

---

**Last Updated**: December 3, 2025  
**Version**: 1.0.0  
**License**: MIT  
**Author**: Sad Language Team

**بسم الله الرحمن الرحيم - والحمد لله رب العالمين**
