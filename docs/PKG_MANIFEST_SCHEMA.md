# بسم الله الرحمن الرحيم

# Sad Language Package Manifest Schema
# مخطط ملف بيانات حزم لغة ص

## Overview / نظرة عامة

This document describes the schema for `sad.toml`, the package manifest file used in Sad Language projects.

توضح هذه الوثيقة مخطط ملف `sad.toml`، ملف بيانات الحزمة المستخدم في مشاريع لغة ص.

---

## File Format / صيغة الملف

Package manifests use TOML (Tom's Obvious, Minimal Language) format.

ملفات بيانات الحزم تستخدم صيغة TOML (لغة توم الواضحة والبسيطة).

---

## Schema / المخطط

### [package] Section / قسم [package]

معلومات أساسية عن الحزمة

| Field | Type | Required | Description | الوصف |
|-------|------|----------|-------------|--------|
| `name` | string | ✓ | Package name (lowercase, alphanumeric, hyphens) | اسم الحزمة (أحرف صغيرة، أرقام، شرطات) |
| `version` | string | ✓ | Semantic version (e.g., "1.0.0") | الإصدار الدلالي |
| `description` | string | ✓ | Short description (English) | وصف مختصر (بالإنجليزية) |
| `description_ar` | string | ✗ | Short description (Arabic) | وصف مختصر (بالعربية) |
| `authors` | array | ✓ | List of authors with emails | قائمة المؤلفين مع البريد الإلكتروني |
| `license` | string | ✓ | License identifier (e.g., "MIT", "Apache-2.0") | معرّف الترخيص |
| `repository` | string | ✗ | Git repository URL | رابط مستودع Git |
| `homepage` | string | ✗ | Package homepage URL | رابط الصفحة الرئيسية |
| `documentation` | string | ✗ | Documentation URL | رابط التوثيق |
| `readme` | string | ✗ | Path to README file (default: "README.md") | مسار ملف README |
| `keywords` | array | ✗ | Search keywords | كلمات مفتاحية للبحث |
| `categories` | array | ✗ | Package categories | تصنيفات الحزمة |

**Example / مثال:**

```toml
[package]
name = "http-client"
version = "1.2.0"
description = "Simple HTTP client library for Sad Language"
description_ar = "مكتبة عميل HTTP بسيطة للغة ص"
authors = [
    "أحمد محمد <ahmad@example.com>",
    "فاطمة علي <fatima@example.com>"
]
license = "MIT"
repository = "https://github.com/sad-lang/http-client"
homepage = "https://sadlang.org/packages/http-client"
documentation = "https://docs.sadlang.org/http-client"
readme = "README.md"
keywords = ["http", "client", "rest", "api", "web"]
categories = ["network", "web"]
```

---

### [dependencies] Section / قسم [dependencies]

التبعيات الإنتاجية المطلوبة

Dependencies required for the package to run in production.

**Format / الصيغة:**

```toml
[dependencies]
package-name = "version-requirement"
```

**Version Requirements / متطلبات الإصدار:**

| Requirement | Example | Description | الوصف |
|-------------|---------|-------------|--------|
| Exact | `= 1.2.3` | Exactly 1.2.3 | بالضبط 1.2.3 |
| Caret | `^1.2.3` | >= 1.2.3 and < 2.0.0 | من 1.2.3 حتى قبل 2.0.0 |
| Tilde | `~1.2.3` | >= 1.2.3 and < 1.3.0 | من 1.2.3 حتى قبل 1.3.0 |
| Wildcard | `1.2.*` | Any patch version of 1.2 | أي إصدار تصحيح من 1.2 |
| Range | `>= 1.0.0, < 2.0.0` | Between 1.0.0 and 2.0.0 | بين 1.0.0 و 2.0.0 |

**Example / مثال:**

```toml
[dependencies]
json = "^2.1.0"           # >= 2.1.0 and < 3.0.0
crypto = "~1.5.2"         # >= 1.5.2 and < 1.6.0
filesystem = "= 1.0.0"    # Exactly 1.0.0
xml = "1.3.*"             # Any patch version of 1.3
database = ">= 2.0.0, < 3.0.0"  # Range
```

---

### [dev-dependencies] Section / قسم [dev-dependencies]

تبعيات التطوير والاختبار

Dependencies only needed during development and testing.

**Example / مثال:**

```toml
[dev-dependencies]
test = "^1.0.0"
benchmark = "~0.5.0"
```

---

### [build-dependencies] Section / قسم [build-dependencies]

تبعيات البناء

Dependencies needed only during the build process.

**Example / مثال:**

```toml
[build-dependencies]
code-gen = "^2.0.0"
compiler-plugin = "1.1.0"
```

---

### [lib] Section / قسم [lib]

إعدادات المكتبة

Library settings and entry points.

| Field | Type | Required | Description | الوصف |
|-------|------|----------|-------------|--------|
| `name` | string | ✗ | Library name (defaults to package name) | اسم المكتبة |
| `path` | string | ✗ | Path to main library file (default: "src/lib.s") | مسار ملف المكتبة الرئيسي |

**Example / مثال:**

```toml
[lib]
name = "http"
path = "src/lib.s"
```

---

### [[bin]] Section / قسم [[bin]]

تطبيقات قابلة للتنفيذ

Executable binaries provided by the package.

| Field | Type | Required | Description | الوصف |
|-------|------|----------|-------------|--------|
| `name` | string | ✓ | Binary name | اسم التطبيق |
| `path` | string | ✓ | Path to main file | مسار الملف الرئيسي |

**Example / مثال:**

```toml
[[bin]]
name = "http-server"
path = "src/bin/server.s"

[[bin]]
name = "http-client"
path = "src/bin/client.s"
```

---

### [scripts] Section / قسم [scripts]

نصوص مخصصة

Custom scripts for common tasks.

**Example / مثال:**

```toml
[scripts]
test = "sad test"
build = "sad build --release"
docs = "sad doc --open"
format = "sad fmt"
lint = "sad lint"
```

---

### [features] Section / قسم [features]

ميزات اختيارية

Optional features that can be enabled.

**Example / مثال:**

```toml
[features]
default = ["ssl", "compression"]  # Default features / الميزات الافتراضية
ssl = []                          # SSL support / دعم SSL
compression = ["zlib"]            # Compression with zlib / الضغط مع zlib
full = ["ssl", "compression", "async"]  # All features / جميع الميزات
```

---

### [profile] Section / قسم [profile]

إعدادات البناء

Build profiles for different configurations.

#### [profile.dev] - Development / التطوير

```toml
[profile.dev]
opt-level = 0          # No optimization / بدون تحسين
debug = true           # Include debug info / تضمين معلومات التصحيح
```

#### [profile.release] - Production / الإنتاج

```toml
[profile.release]
opt-level = 3          # Maximum optimization / أقصى تحسين
debug = false          # No debug info / بدون معلومات التصحيح
lto = true            # Link-time optimization / تحسين وقت الربط
```

---

### [target] Section / قسم [target]

إعدادات المنصة

Platform-specific configuration.

**Example / مثال:**

```toml
[target.'cfg(windows)']
dependencies = { winapi = "^0.3" }

[target.'cfg(unix)']
dependencies = { libc = "^0.2" }
```

---

## Complete Example / مثال كامل

```toml
# بسم الله الرحمن الرحيم
# sad.toml - Package manifest for Sad Language
# ملف بيانات الحزمة للغة ص

[package]
name = "awesome-lib"
version = "2.1.0"
description = "An awesome library for Sad Language"
description_ar = "مكتبة رائعة للغة ص"
authors = [
    "محمد أحمد <mohammed@example.com>",
    "سارة علي <sara@example.com>"
]
license = "MIT"
repository = "https://github.com/user/awesome-lib"
homepage = "https://awesome-lib.sadlang.org"
documentation = "https://docs.sadlang.org/awesome-lib"
readme = "README.md"
keywords = ["awesome", "utility", "tools"]
categories = ["development-tools", "utilities"]

[lib]
name = "awesome"
path = "src/lib.s"

[[bin]]
name = "awesome-cli"
path = "src/bin/cli.s"

[dependencies]
json = "^2.1.0"
http = "~1.5.0"
crypto = "= 1.0.0"
filesystem = "1.2.*"

[dev-dependencies]
test = "^1.0.0"
benchmark = "~0.5.0"
mock = "^2.0.0"

[build-dependencies]
code-gen = "^2.0.0"

[features]
default = ["ssl"]
ssl = []
async = ["tokio"]
full = ["ssl", "async"]

[scripts]
test = "sad test"
build = "sad build --release"
docs = "sad doc --open"
format = "sad fmt"
lint = "sad lint"
check = "sad check"

[profile.dev]
opt-level = 0
debug = true

[profile.release]
opt-level = 3
debug = false
lto = true

[target.'cfg(windows)']
dependencies = { winapi = "^0.3" }

[target.'cfg(unix)']
dependencies = { libc = "^0.2" }
```

---

## Package Name Rules / قواعد تسمية الحزم

اسم الحزمة يجب أن:

1. يبدأ بحرف (a-z)
2. يحتوي فقط على:
   - أحرف صغيرة (a-z)
   - أرقام (0-9)
   - شرطات (-)
3. ألا يحتوي على شرطات متتالية (--)
4. ألا ينتهي بشرطة (-)
5. يكون بين 2 و 64 حرفاً

**Valid Names / أسماء صالحة:**
- `my-package`
- `http-client`
- `json2xml`

**Invalid Names / أسماء غير صالحة:**
- `MyPackage` (uppercase / أحرف كبيرة)
- `-package` (starts with hyphen / يبدأ بشرطة)
- `my--package` (consecutive hyphens / شرطات متتالية)
- `package-` (ends with hyphen / ينتهي بشرطة)

---

## Version Format / صيغة الإصدار

يتبع نظام الإصدار الدلالي (Semantic Versioning):

```
MAJOR.MINOR.PATCH[-PRERELEASE][+BUILD]
```

**Examples / أمثلة:**
- `1.0.0` - Stable release / إصدار مستقر
- `1.0.0-alpha.1` - Alpha prerelease / إصدار تجريبي ألفا
- `1.0.0-beta.2` - Beta prerelease / إصدار تجريبي بيتا
- `1.0.0-rc.1` - Release candidate / مرشح للإصدار
- `1.0.0+20251211` - With build metadata / مع بيانات البناء

---

## Validation / التحقق

قبل النشر، يتم التحقق من:

1. ✓ صحة صيغة TOML
2. ✓ وجود الحقول المطلوبة
3. ✓ صحة أسماء الحزم
4. ✓ صحة صيغة الإصدار
5. ✓ صحة متطلبات الإصدار للتبعيات
6. ✓ وجود الملفات المشار إليها
7. ✓ صحة عناوين URL

---

## Tools / الأدوات

### Validate Manifest / التحقق من الملف

```bash
sad-pkg check
```

### Generate Manifest / إنشاء ملف

```bash
sad-pkg init
```

### Update Dependencies / تحديث التبعيات

```bash
sad-pkg update
```

---

## See Also / انظر أيضاً

- [Package Registry API](PKG_REGISTRY_API.md)
- [Dart Backend Example](DART_BACKEND_EXAMPLE.md)
- [Package Manager README](../tools/pkg/README.md)

---

الحمد لله رب العالمين
