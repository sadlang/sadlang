# بسم الله الرحمن الرحيم
# Sad Package Manager API Documentation
# توثيق واجهة برمجة تطبيقات مدير حزم لغة ص

**Version:** 1.0.0  
**Date:** January 7, 2026  
**Status:** Stable

---

## Table of Contents / جدول المحتويات

1. [Overview](#overview)
2. [Core Classes](#core-classes)
   - [Version](#version)
   - [VersionRequirement](#versionrequirement)
   - [Package](#package)
   - [DependencyResolver](#dependencyresolver)
   - [RegistryClient](#registryclient)
3. [CLI Commands](#cli-commands)
4. [File Formats](#file-formats)
5. [Examples](#examples)

---

## Overview / نظرة عامة

The Sad Package Manager (`sad-pkg`) provides a complete API for managing packages, dependencies, and versions in Sad Language projects.

مدير حزم Sad (`sad-pkg`) يوفر واجهة برمجية كاملة لإدارة الحزم والتبعيات والإصدارات في مشاريع لغة ص.

### Key Features / الميزات الرئيسية

- **Semantic Versioning** - Full SemVer 2.0.0 support
- **Dependency Resolution** - Automatic transitive dependency resolution
- **Version Constraints** - Support for ^, ~, >, >=, <, <=, * operators
- **TOML Configuration** - Human-readable package configuration
- **Registry Integration** - Package download from central registry
- **Conflict Detection** - Automatic version conflict detection

---

## Core Classes / الفئات الأساسية

### Version

**File:** `package.h`  
**Namespace:** `sad::pkg`

Represents a semantic version (major.minor.patch).

```cpp
class Version {
public:
    int major;
    int minor;
    int patch;
    std::string prerelease;  // e.g., "alpha.1"
    std::string build;       // e.g., "20130313144700"
    
    // Constructor
    Version(int maj = 0, int min = 0, int pat = 0, 
            const std::string& pre = "", const std::string& bld = "");
    
    // Parse from string
    static Version parse(const std::string& version_str);
    
    // Convert to string
    std::string to_string() const;
    
    // Comparison operators
    bool operator==(const Version& other) const;
    bool operator!=(const Version& other) const;
    bool operator<(const Version& other) const;
    bool operator>(const Version& other) const;
    bool operator<=(const Version& other) const;
    bool operator>=(const Version& other) const;
};
```

#### Methods / الدوال

##### `static Version parse(const std::string& version_str)`

Parse a version string into a Version object.

**Parameters:**
- `version_str` - Version string (e.g., "1.2.3", "2.0.0-alpha.1")

**Returns:** Version object

**Throws:** `std::invalid_argument` if format is invalid

**Examples:**
```cpp
Version v1 = Version::parse("1.2.3");
Version v2 = Version::parse("2.0.0-alpha.1");
Version v3 = Version::parse("1.0.0+build.123");
```

##### `std::string to_string() const`

Convert version to string representation.

**Returns:** Version string (e.g., "1.2.3-alpha+build")

**Example:**
```cpp
Version v(1, 2, 3, "alpha.1", "build.123");
std::cout << v.to_string(); // "1.2.3-alpha.1+build.123"
```

#### Comparison Operators / عوامل المقارنة

```cpp
Version v1 = Version::parse("1.0.0");
Version v2 = Version::parse("1.0.1");

assert(v1 < v2);   // true
assert(v2 > v1);   // true
assert(v1 <= v2);  // true
assert(v1 != v2);  // true
```

**Version Ordering Rules:**
1. Compare major, minor, patch numerically
2. Prerelease versions have lower precedence than stable
3. Build metadata is ignored in comparisons

---

### VersionRequirement

**File:** `package.h`  
**Namespace:** `sad::pkg`

Represents a version requirement constraint.

```cpp
class VersionRequirement {
public:
    enum class Operator {
        EXACT,      // "1.2.3"
        CARET,      // "^1.2.3" - compatible with 1.x.x
        TILDE,      // "~1.2.3" - compatible with 1.2.x
        GT,         // ">1.2.3"
        GTE,        // ">=1.2.3"
        LT,         // "<1.2.3"
        LTE,        // "<=1.2.3"
        WILDCARD    // "*" or "1.x.x"
    };
    
    Operator op;
    Version version;
    
    // Constructor
    VersionRequirement(Operator o, const Version& v);
    
    // Parse from string
    static VersionRequirement parse(const std::string& req_str);
    
    // Check if version satisfies requirement
    bool satisfies(const Version& v) const;
    
    // Convert to string
    std::string to_string() const;
};
```

#### Supported Operators / العوامل المدعومة

| Operator | Description | Example | Matches |
|----------|-------------|---------|---------|
| Exact | Exact version | `1.2.3` | 1.2.3 only |
| Caret `^` | Compatible updates | `^1.2.3` | >=1.2.3 <2.0.0 |
| Tilde `~` | Patch-level changes | `~1.2.3` | >=1.2.3 <1.3.0 |
| `>` | Greater than | `>1.2.3` | 1.2.4, 2.0.0, etc. |
| `>=` | Greater or equal | `>=1.2.3` | 1.2.3, 1.2.4, etc. |
| `<` | Less than | `<2.0.0` | 1.9.9, 1.0.0, etc. |
| `<=` | Less or equal | `<=2.0.0` | 2.0.0, 1.9.9, etc. |
| `*` | Any version | `*` | Any version |

#### Caret (^) Operator Rules / قواعد عامل الإقحام (^)

```
^1.2.3  := >=1.2.3 <2.0.0
^0.2.3  := >=0.2.3 <0.3.0  (special case for 0.x)
^0.0.3  := >=0.0.3 <0.0.4  (special case for 0.0.x)
```

#### Tilde (~) Operator Rules / قواعد عامل التيلدة (~)

```
~1.2.3  := >=1.2.3 <1.3.0
~1.2    := >=1.2.0 <1.3.0
~1      := >=1.0.0 <2.0.0
```

#### Examples / أمثلة

```cpp
// Exact version
VersionRequirement req1 = VersionRequirement::parse("1.2.3");
assert(req1.satisfies(Version::parse("1.2.3")));
assert(!req1.satisfies(Version::parse("1.2.4")));

// Caret (compatible updates)
VersionRequirement req2 = VersionRequirement::parse("^1.2.3");
assert(req2.satisfies(Version::parse("1.2.3")));
assert(req2.satisfies(Version::parse("1.3.0")));
assert(req2.satisfies(Version::parse("1.99.0")));
assert(!req2.satisfies(Version::parse("2.0.0")));

// Tilde (patch updates)
VersionRequirement req3 = VersionRequirement::parse("~1.2.3");
assert(req3.satisfies(Version::parse("1.2.3")));
assert(req3.satisfies(Version::parse("1.2.9")));
assert(!req3.satisfies(Version::parse("1.3.0")));

// Greater than
VersionRequirement req4 = VersionRequirement::parse(">1.2.3");
assert(!req4.satisfies(Version::parse("1.2.3")));
assert(req4.satisfies(Version::parse("1.2.4")));
assert(req4.satisfies(Version::parse("2.0.0")));

// Wildcard
VersionRequirement req5 = VersionRequirement::parse("*");
assert(req5.satisfies(Version::parse("0.0.1")));
assert(req5.satisfies(Version::parse("999.999.999")));
```

---

### Package

**File:** `package.h`  
**Namespace:** `sad::pkg`

Represents a Sad Language package with metadata and dependencies.

```cpp
class Package {
public:
    // Package metadata
    std::string name;
    Version version;
    std::string description;
    std::vector<std::string> authors;
    std::string license;
    std::string repository;
    std::string homepage;
    std::vector<std::string> keywords;
    
    // Build configuration
    std::string main_file;
    std::vector<std::string> source_dirs;
    std::string output_dir;
    
    // Dependencies
    std::map<std::string, std::string> dependencies;
    std::map<std::string, std::string> dev_dependencies;
    
    // Scripts
    std::map<std::string, std::string> scripts;
    
    // Methods
    static Package load_from_file(const std::string& filename);
    void save_to_file(const std::string& filename) const;
    bool is_valid() const;
};
```

#### Methods / الدوال

##### `static Package load_from_file(const std::string& filename)`

Load package from sad.toml file.

**Parameters:**
- `filename` - Path to sad.toml file

**Returns:** Package object

**Throws:** `std::runtime_error` if file not found or invalid format

**Example:**
```cpp
Package pkg = Package::load_from_file("sad.toml");
std::cout << "Package: " << pkg.name << " v" << pkg.version.to_string() << "\n";
```

##### `void save_to_file(const std::string& filename) const`

Save package to TOML file.

**Parameters:**
- `filename` - Output file path

**Example:**
```cpp
Package pkg;
pkg.name = "my_package";
pkg.version = Version::parse("1.0.0");
pkg.save_to_file("sad.toml");
```

##### `bool is_valid() const`

Check if package has required fields.

**Returns:** `true` if valid, `false` otherwise

**Required fields:**
- name (non-empty)
- version
- main_file (non-empty)

**Example:**
```cpp
Package pkg;
pkg.name = "test";
pkg.version = Version::parse("1.0.0");
pkg.main_file = "src/main.s";
assert(pkg.is_valid());
```

---

### DependencyResolver

**File:** `dependency_resolver.h`  
**Namespace:** `sad::pkg`

Resolves package dependencies with version constraints.

```cpp
class DependencyResolver {
public:
    DependencyResolver();
    
    // Resolve dependencies
    std::map<std::string, Package> resolve(
        const std::map<std::string, std::string>& dependencies
    );
    
    // Set registry client
    void set_registry(std::shared_ptr<RegistryClient> client);
};
```

#### Methods / الدوال

##### `std::map<std::string, Package> resolve(...)`

Resolve all dependencies including transitive dependencies.

**Parameters:**
- `dependencies` - Map of package names to version requirements

**Returns:** Map of resolved packages with their versions

**Throws:** `std::runtime_error` if:
- Version conflict detected
- Circular dependency found
- Package not found in registry

**Example:**
```cpp
DependencyResolver resolver;

std::map<std::string, std::string> deps = {
    {"http", "^1.0.0"},
    {"json", "~2.3.0"}
};

auto resolved = resolver.resolve(deps);

for (const auto& [name, pkg] : resolved) {
    std::cout << name << " " << pkg.version.to_string() << "\n";
}
```

#### Resolution Algorithm / خوارزمية الحل

1. **Parse Requirements** - Parse version strings into VersionRequirement objects
2. **Fetch Metadata** - Get available versions from registry
3. **Select Versions** - Choose highest compatible version for each package
4. **Check Conflicts** - Detect version conflicts between dependencies
5. **Resolve Transitive** - Recursively resolve dependencies of dependencies
6. **Detect Cycles** - Check for circular dependencies
7. **Return Result** - Return map of resolved packages

---

### RegistryClient

**File:** `registry_client.h`  
**Namespace:** `sad::pkg`

Client for interacting with package registry.

```cpp
class RegistryClient {
public:
    RegistryClient(const std::string& registry_url = "https://registry.sad-lang.org");
    
    // Search packages
    std::vector<PackageInfo> search(const std::string& query);
    
    // Get package info
    PackageInfo get_info(const std::string& package_name);
    
    // Get available versions
    std::vector<Version> get_versions(const std::string& package_name);
    
    // Download package
    void download(const std::string& package_name, 
                  const Version& version,
                  const std::string& output_dir);
    
    // Publish package
    void publish(const Package& pkg, const std::string& tarball_path);
};
```

#### Methods / الدوال

##### `std::vector<PackageInfo> search(const std::string& query)`

Search for packages in registry.

**Parameters:**
- `query` - Search query string

**Returns:** Vector of matching packages

**Example:**
```cpp
RegistryClient client;
auto results = client.search("http");

for (const auto& pkg_info : results) {
    std::cout << pkg_info.name << " - " << pkg_info.description << "\n";
}
```

##### `PackageInfo get_info(const std::string& package_name)`

Get detailed package information.

**Parameters:**
- `package_name` - Name of package

**Returns:** PackageInfo structure

**Example:**
```cpp
RegistryClient client;
PackageInfo info = client.get_info("http");

std::cout << "Name: " << info.name << "\n";
std::cout << "Latest: " << info.latest_version.to_string() << "\n";
std::cout << "Downloads: " << info.downloads << "\n";
```

##### `void download(...)`

Download and extract package.

**Parameters:**
- `package_name` - Package to download
- `version` - Specific version
- `output_dir` - Where to extract

**Example:**
```cpp
RegistryClient client;
client.download("http", Version::parse("1.0.0"), "sad_modules/http");
```

---

## CLI Commands / أوامر سطر الأوامر

### sad-pkg init

Initialize a new package.

```bash
sad-pkg init
```

**Interactive prompts:**
- Package name
- Version
- Description
- Author

**Creates:**
- `sad.toml` configuration file
- `src/` directory
- `src/main.s` entry point

---

### sad-pkg install

Install dependencies.

```bash
# Install all dependencies from sad.toml
sad-pkg install

# Install specific package
sad-pkg install <package>
```

**Actions:**
1. Read sad.toml
2. Resolve dependencies
3. Download packages
4. Extract to sad_modules/
5. Create sad.lock

---

### sad-pkg add

Add a dependency.

```bash
sad-pkg add <package> [version]
```

**Examples:**
```bash
sad-pkg add http ^1.0.0
sad-pkg add json ~2.3.0
sad-pkg add utils *
```

**Actions:**
1. Add to sad.toml [dependencies]
2. Run install

---

### sad-pkg remove

Remove a dependency.

```bash
sad-pkg remove <package>
```

**Actions:**
1. Remove from sad.toml
2. Remove from sad_modules/

---

### sad-pkg update

Update dependencies to latest compatible versions.

```bash
sad-pkg update
```

**Actions:**
1. Re-resolve dependencies
2. Download updates
3. Update sad.lock

---

### sad-pkg publish

Publish package to registry.

```bash
sad-pkg publish
```

**Requirements:**
- Valid sad.toml
- Authentication token
- README.md (recommended)

---

### sad-pkg search

Search for packages.

```bash
sad-pkg search <query>
```

**Example:**
```bash
sad-pkg search http
```

---

### sad-pkg info

Show package information.

```bash
sad-pkg info <package>
```

**Example:**
```bash
sad-pkg info http
```

---

### sad-pkg list

List installed packages.

```bash
sad-pkg list
```

---

### sad-pkg clean

Clean package cache.

```bash
sad-pkg clean
```

---

## File Formats / تنسيقات الملفات

### sad.toml

Package configuration file.

```toml
[package]
name = "my_package"
version = "1.0.0"
description = "My awesome package"
authors = ["Author Name <email@example.com>"]
license = "MIT"
repository = "https://github.com/user/repo"
homepage = "https://mypackage.com"
keywords = ["http", "web", "client"]

[build]
main = "src/main.s"
sources = ["src/", "lib/"]
output = "build/"

[dependencies]
http = "^1.0.0"
json = "~2.3.0"

[dev-dependencies]
test_framework = "^0.5.0"

[scripts]
build = "sad build"
test = "sad test"
deploy = "sad deploy --prod"
```

### sad.lock

Lock file with resolved versions.

```toml
# Generated by sad-pkg
# Do not edit manually

[[package]]
name = "http"
version = "1.2.5"
dependencies = ["url ^0.5.0", "socket ^2.1.0"]

[[package]]
name = "json"
version = "2.3.8"
dependencies = []

[[package]]
name = "url"
version = "0.5.2"
dependencies = []

[[package]]
name = "socket"
version = "2.1.3"
dependencies = []
```

---

## Examples / أمثلة

### Example 1: Create and Publish Package

```bash
# Initialize package
sad-pkg init
# Enter: my_http_client, 1.0.0, "HTTP client library", "Your Name"

# Add dependencies
sad-pkg add url ^0.5.0
sad-pkg add socket ^2.1.0

# Install dependencies
sad-pkg install

# Build and test
sad build
sad test

# Publish
sad-pkg publish
```

### Example 2: Using the API in C++

```cpp
#include <sad/pkg/package.h>
#include <sad/pkg/dependency_resolver.h>
#include <sad/pkg/registry_client.h>

using namespace sad::pkg;

int main() {
    // Load package
    Package pkg = Package::load_from_file("sad.toml");
    
    // Resolve dependencies
    DependencyResolver resolver;
    auto resolved = resolver.resolve(pkg.dependencies);
    
    // Download packages
    RegistryClient client;
    for (const auto& [name, dep_pkg] : resolved) {
        std::cout << "Downloading " << name << " " 
                  << dep_pkg.version.to_string() << "...\n";
        client.download(name, dep_pkg.version, "sad_modules/" + name);
    }
    
    std::cout << "All dependencies installed!\n";
    return 0;
}
```

### Example 3: Version Constraint Checking

```cpp
#include <sad/pkg/package.h>

using namespace sad::pkg;

int main() {
    // Parse requirement
    VersionRequirement req = VersionRequirement::parse("^1.2.3");
    
    // Check versions
    std::vector<std::string> versions = {
        "1.2.3", "1.2.4", "1.3.0", "1.99.0", "2.0.0"
    };
    
    for (const auto& v_str : versions) {
        Version v = Version::parse(v_str);
        if (req.satisfies(v)) {
            std::cout << v_str << " satisfies ^1.2.3\n";
        }
    }
    
    return 0;
}
```

---

## Error Handling / معالجة الأخطاء

### Common Exceptions / الاستثناءات الشائعة

```cpp
try {
    Package pkg = Package::load_from_file("sad.toml");
} catch (const std::runtime_error& e) {
    // File not found or invalid TOML
    std::cerr << "Error: " << e.what() << "\n";
}

try {
    Version v = Version::parse("invalid");
} catch (const std::invalid_argument& e) {
    // Invalid version format
    std::cerr << "Error: " << e.what() << "\n";
}

try {
    DependencyResolver resolver;
    auto resolved = resolver.resolve(dependencies);
} catch (const std::runtime_error& e) {
    // Version conflict or circular dependency
    std::cerr << "Error: " << e.what() << "\n";
}
```

---

**الحمد لله رب العالمين** 🤲

**Documentation complete! / التوثيق مكتمل!** ✅
