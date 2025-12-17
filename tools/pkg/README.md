# بسم الله الرحمن الرحيم

# sad-pkg - Sad Language Package Manager
# مدير حزم لغة ص

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Version](https://img.shields.io/badge/Version-1.0.0-green.svg)](tools/pkg/)

## Overview / نظرة عامة

**sad-pkg** is the official package manager for Sad Language. It manages dependencies, installs packages from the central registry, and provides a seamless development experience for Sad projects.

**sad-pkg** هو مدير الحزم الرسمي للغة ص. يدير التبعيات، ويثبت الحزم من السجل المركزي، ويوفر تجربة تطوير سلسة لمشاريع لغة ص.

## Features / المزايا

### Core Features / المزايا الأساسية

- ✅ **TOML Configuration** - Declarative package configuration using `sad.toml`
- ✅ **Dependency Resolution** - Automatic dependency resolution with semantic versioning
- ✅ **Registry Integration** - Download packages from central registry
- ✅ **Package Caching** - Local cache for faster installs
- ✅ **Version Management** - Support for exact, caret (^), and tilde (~) version requirements
- ✅ **Conflict Detection** - Detects and reports version conflicts
- ✅ **Transitive Dependencies** - Automatically resolves nested dependencies
- ✅ **Colored Output** - Beautiful CLI with colored output

### Supported Commands / الأوامر المدعومة

```bash
sad-pkg init              # Initialize new package / إنشاء حزمة جديدة
sad-pkg install [pkg]     # Install dependencies / تثبيت التبعيات
sad-pkg add <pkg>         # Add dependency / إضافة تبعية
sad-pkg remove <pkg>      # Remove dependency / إزالة تبعية
sad-pkg update            # Update dependencies / تحديث التبعيات
sad-pkg publish           # Publish package / نشر الحزمة
sad-pkg search <query>    # Search packages / البحث عن الحزم
sad-pkg info <pkg>        # Show package info / عرض معلومات الحزمة
sad-pkg list              # List installed / سرد المثبتة
sad-pkg clean             # Clean cache / تنظيف التخزين المؤقت
```

## Installation / التثبيت

### From Source / من المصدر

```bash
# Clone repository - استنساخ المستودع
git clone https://github.com/sad-lang/sad.git
cd sad

# Build with CMake - البناء باستخدام CMake
mkdir build && cd build
cmake .. -DBUILD_PKG_MANAGER=ON
cmake --build .

# Install (optional) - التثبيت (اختياري)
cmake --install .
```

### Requirements / المتطلبات

- CMake 3.15+
- C++17 compiler
- libcurl (for HTTP requests)
- nlohmann/json (auto-downloaded by CMake)

## Quick Start / البدء السريع

### 1. Initialize a New Package / إنشاء حزمة جديدة

```bash
sad-pkg init
```

This creates a `sad.toml` file and basic project structure:

```
my_package/
├── sad.toml
└── src/
    └── main.s
```

### 2. Add Dependencies / إضافة التبعيات

```bash
# Add HTTP library - إضافة مكتبة HTTP
sad-pkg add http ^1.0.0

# Add JSON library - إضافة مكتبة JSON
sad-pkg add json ~2.3.0
```

### 3. Install Dependencies / تثبيت التبعيات

```bash
sad-pkg install
```

This resolves and installs all dependencies listed in `sad.toml`.

## sad.toml Format / صيغة sad.toml

### Basic Example / مثال بسيط

```toml
[package]
name = "my_awesome_package"
version = "1.0.0"
description = "An awesome Sad package"
authors = ["Your Name <your.email@example.com>"]
license = "MIT"
repository = "https://github.com/username/my_awesome_package"
homepage = "https://mypackage.example.com"
readme = "README.md"
keywords = ["web", "http", "api"]
categories = ["networking", "utilities"]

[dependencies]
http = "^1.0.0"      # Compatible with 1.x.x
json = "~2.3.0"      # Compatible with 2.3.x
crypto = ">=1.5.0"   # At least 1.5.0
database = "*"       # Any version

[dev-dependencies]
test_framework = "^0.5.0"
benchmarking = "^1.0.0"

[build-dependencies]
code_generator = "^2.0.0"

[build]
main = "src/main.s"
source-dirs = ["src", "lib"]
include-dirs = ["include"]
exclude = ["tests", "examples"]
```

### Version Requirements / متطلبات الإصدار

| Syntax | Meaning | Example |
|--------|---------|---------|
| `1.2.3` | Exact version | Must be 1.2.3 |
| `^1.2.3` | Caret (compatible) | ≥1.2.3, <2.0.0 |
| `~1.2.3` | Tilde (patch updates) | ≥1.2.3, <1.3.0 |
| `>=1.2.3` | Greater or equal | Any version ≥1.2.3 |
| `>1.2.3` | Greater than | Any version >1.2.3 |
| `<=1.2.3` | Less or equal | Any version ≤1.2.3 |
| `<1.2.3` | Less than | Any version <1.2.3 |
| `*` | Wildcard | Any version |

### Semantic Versioning / الإصدار الدلالي

Sad packages follow [Semantic Versioning 2.0.0](https://semver.org/):

- **Major** (`X.0.0`) - Breaking changes
- **Minor** (`0.X.0`) - New features (backward compatible)
- **Patch** (`0.0.X`) - Bug fixes

Example versions:
```
1.0.0
1.0.0-alpha.1      # Prerelease
1.0.0+build.123    # Build metadata
```

## Architecture / البنية المعمارية

### Components / المكونات

```
tools/pkg/
├── toml_parser.h          # TOML parser implementation
├── package.h              # Package metadata & management
├── dependency_resolver.h  # Dependency resolution algorithm
├── registry_client.h      # HTTP client for package registry
└── cli.cpp                # Command-line interface
```

### Key Classes / الفئات الرئيسية

#### 1. TomlParser
```cpp
class TomlParser {
    static TomlTable parse(const std::string& content);
    static TomlTable parse_file(const std::string& filepath);
    static void write_file(const std::string& filepath, const TomlTable& table);
};
```

#### 2. Package
```cpp
class Package {
    std::string name;
    Version version;
    std::vector<Dependency> dependencies;
    
    static Package from_file(const std::filesystem::path& toml_path);
    void save_to_file(const std::filesystem::path& toml_path) const;
    std::vector<std::string> validate() const;
};
```

#### 3. DependencyResolver
```cpp
class DependencyResolver {
    std::vector<ResolvedDependency> resolve(const Package& root_package, 
                                           bool include_dev = false);
    bool has_conflicts() const;
    const std::vector<DependencyConflict>& get_conflicts() const;
};
```

#### 4. RegistryClient
```cpp
class RegistryClient : public IPackageRegistry {
    std::vector<Version> get_available_versions(const std::string& package_name);
    Package get_package_metadata(const std::string& package_name, const Version& version);
    bool download_package(const std::string& package_name, const Version& version, 
                         const std::filesystem::path& dest_path);
};
```

## Examples / أمثلة

### Creating a Web Service Package / إنشاء حزمة خدمة ويب

```bash
# Initialize project - إنشاء المشروع
sad-pkg init
# Enter: "web_service", "0.1.0", "HTTP web service", "Your Name"

# Add dependencies - إضافة التبعيات
sad-pkg add http ^2.0.0
sad-pkg add json ^1.0.0
sad-pkg add router ^1.5.0

# Install dependencies - تثبيت التبعيات
sad-pkg install
```

**sad.toml**:
```toml
[package]
name = "web_service"
version = "0.1.0"
description = "HTTP web service"
authors = ["Your Name"]

[dependencies]
http = "^2.0.0"
json = "^1.0.0"
router = "^1.5.0"

[build]
main = "src/main.s"
source-dirs = ["src"]
```

### Publishing a Package / نشر حزمة

```bash
# Validate package - التحقق من الحزمة
sad-pkg build

# Publish to registry - النشر في السجل
sad-pkg publish

# Authenticate first if needed - المصادقة أولاً إذا لزم الأمر
export SAD_REGISTRY_TOKEN="your-token-here"
sad-pkg publish
```

## Testing / الاختبار

The package manager includes comprehensive tests:

```bash
# Build with tests - البناء مع الاختبارات
cmake .. -DBUILD_PKG_MANAGER=ON -DBUILD_TESTS=ON
cmake --build .

# Run tests - تشغيل الاختبارات
cd build
ctest -R test_package_manager
```

### Test Coverage / تغطية الاختبارات

- ✅ TOML parsing (strings, arrays, tables, inline tables)
- ✅ Version parsing and comparison
- ✅ Version requirement matching
- ✅ Package loading and saving
- ✅ Dependency resolution (simple, transitive, conflicts)
- ✅ Registry client operations

## Cache Management / إدارة التخزين المؤقت

### Cache Location / موقع التخزين المؤقت

**Windows**:
```
%LOCALAPPDATA%\sad\cache\
C:\Users\YourName\AppData\Local\sad\cache\
```

**Linux/macOS**:
```
~/.sad/cache/
```

### Cache Structure / هيكل التخزين المؤقت

```
cache/
├── http/
│   ├── 1.0.0/
│   │   ├── sad.toml
│   │   └── src/
│   └── 1.5.0/
│       └── ...
└── json/
    └── 2.3.0/
        └── ...
```

### Cache Commands / أوامر التخزين المؤقت

```bash
# View cache size - عرض حجم التخزين المؤقت
sad-pkg cache info

# Clear cache - مسح التخزين المؤقت
sad-pkg clean

# List cached packages - سرد الحزم المخزنة
sad-pkg list
```

## Registry API / واجهة السجل

### Endpoints / نقاط النهاية

```
GET  /api/search?q=<query>&limit=<n>     # Search packages
GET  /api/packages/<name>/versions        # List versions
GET  /api/packages/<name>/<version>       # Get metadata
GET  /api/packages/<name>/<version>/download  # Download archive
POST /api/publish                         # Publish package
POST /api/packages/<name>/<version>/yank  # Yank version
GET  /api/user/packages                   # User's packages
```

### Authentication / المصادقة

```bash
# Set authentication token - تعيين رمز المصادقة
export SAD_REGISTRY_TOKEN="your-token-here"

# Or use config file - أو استخدم ملف الإعدادات
echo "token = 'your-token-here'" > ~/.sad/config.toml
```

## Troubleshooting / حل المشاكل

### Common Issues / المشاكل الشائعة

#### 1. Dependency Conflicts / تعارض التبعيات

```
Error: Conflict for package 'http':
  - pkg_a requires ^1.0.0
  - pkg_b requires ^2.0.0
```

**Solution**: Update your dependencies or contact package maintainers.

#### 2. Package Not Found / الحزمة غير موجودة

```
Error: Package not found: unknown_package
```

**Solution**: Check package name spelling or search for similar packages:
```bash
sad-pkg search unknown
```

#### 3. Network Errors / أخطاء الشبكة

```
Error: Failed to download package: Connection timeout
```

**Solution**: Check your internet connection or try again later.

## Contributing / المساهمة

We welcome contributions! Please see [CONTRIBUTING.md](../../CONTRIBUTING.md).

### Development Setup / إعداد التطوير

```bash
# Clone and build - الاستنساخ والبناء
git clone https://github.com/sad-lang/sad.git
cd sad
mkdir build && cd build
cmake .. -DBUILD_PKG_MANAGER=ON -DBUILD_TESTS=ON
cmake --build .

# Run tests - تشغيل الاختبارات
ctest -R test_package_manager -V
```

### Code Style / أسلوب الكود

- C++17 features
- UTF-8 encoding with BOM
- Bilingual comments (Arabic + English)
- Google Test for testing

## License / الترخيص

MIT License - see [LICENSE](../../LICENSE) for details.

## Authors / المؤلفون

- Sad Language Team
- Contributors: see [CONTRIBUTORS.md](../../CONTRIBUTORS.md)

## Acknowledgments / شكر وتقدير

الحمد لله رب العالمين

- Inspired by Cargo (Rust), npm (Node.js), and pip (Python)
- Uses [nlohmann/json](https://github.com/nlohmann/json) for JSON parsing
- Uses [libcurl](https://curl.se/libcurl/) for HTTP requests

---

**Made with ❤️ for the Arabic programming community**  
**صُنع بـ ❤️ للمجتمع البرمجي العربي**
