# بسم الله الرحمن الرحيم
# sad-pkg Usage Examples
# أمثلة استخدام sad-pkg

**Version:** 1.0.0  
**Date:** January 7, 2026

---

## Table of Contents / جدول المحتويات

1. [Getting Started](#getting-started)
2. [Basic Usage](#basic-usage)
3. [Advanced Usage](#advanced-usage)
4. [Real-World Projects](#real-world-projects)
5. [Best Practices](#best-practices)

---

## Getting Started / البدء

### Example 1: Create Your First Package

```bash
# Navigate to your project directory
mkdir my_first_package
cd my_first_package

# Initialize package
sad-pkg init
```

**Interactive prompts:**
```
Package name (default: my_first_package): hello_world
Version (default: 0.1.0): 0.1.0
Description: My first Sad package
Author: Ahmed Ali <ahmed@example.com>

✓ Package initialized successfully!
ℹ Created sad.toml and src/main.s
ℹ Run 'sad-pkg install' to install dependencies
```

**Created files:**
```
hello_world/
├── sad.toml
└── src/
    └── main.s
```

**sad.toml:**
```toml
[package]
name = "hello_world"
version = "0.1.0"
description = "My first Sad package"
authors = ["Ahmed Ali <ahmed@example.com>"]

[build]
main = "src/main.s"
sources = ["src/"]
output = "build/"
```

**src/main.s:**
```sad
// بسم الله الرحمن الرحيم
// hello_world - Main entry point

دالة رئيسية() {
    اطبع("مرحباً من hello_world!");
    أرجع 0;
}
```

---

## Basic Usage / الاستخدام الأساسي

### Example 2: Add Dependencies

```bash
# Add HTTP client library
sad-pkg add http ^1.0.0

# Add JSON parser library
sad-pkg add json ~2.3.0

# Add utilities (any version)
sad-pkg add utils *
```

**Updated sad.toml:**
```toml
[package]
name = "hello_world"
version = "0.1.0"
description = "My first Sad package"
authors = ["Ahmed Ali <ahmed@example.com>"]

[build]
main = "src/main.s"
sources = ["src/"]
output = "build/"

[dependencies]
http = "^1.0.0"
json = "~2.3.0"
utils = "*"
```

### Example 3: Install Dependencies

```bash
sad-pkg install
```

**Output:**
```
Resolving dependencies...
  ✓ http ^1.0.0 -> 1.2.5
  ✓ json ~2.3.0 -> 2.3.8
  ✓ utils * -> 3.1.0
  ℹ Resolving transitive dependencies...
  ✓ url ^0.5.0 -> 0.5.2 (from http)
  ✓ socket ^2.1.0 -> 2.1.3 (from http)

Downloading packages...
  ✓ http 1.2.5 (downloaded 245 KB)
  ✓ json 2.3.8 (downloaded 180 KB)
  ✓ utils 3.1.0 (downloaded 95 KB)
  ✓ url 0.5.2 (downloaded 60 KB)
  ✓ socket 2.1.3 (downloaded 120 KB)

Installing packages...
  ✓ Extracted to sad_modules/

✓ All dependencies installed!
  Total: 5 packages, 700 KB
```

**Created structure:**
```
hello_world/
├── sad.toml
├── sad.lock
├── src/
│   └── main.s
└── sad_modules/
    ├── http/
    ├── json/
    ├── utils/
    ├── url/
    └── socket/
```

### Example 4: Use Dependencies in Code

**src/main.s:**
```sad
// بسم الله الرحمن الرحيم
استورد "http" كـ HTTP;
استورد "json" كـ JSON;

دالة رئيسية() {
    // Create HTTP client
    عميل = HTTP.عميل();
    
    // Make GET request
    استجابة = عميل.احصل("https://api.example.com/data");
    
    // Parse JSON response
    بيانات = JSON.حلل(استجابة.محتوى);
    
    // Print result
    اطبع("البيانات:", بيانات);
    
    أرجع 0;
}
```

### Example 5: Remove Dependency

```bash
sad-pkg remove utils
```

**Output:**
```
Removing utils from sad.toml...
  ✓ Removed utils from dependencies
  ✓ Removed from sad_modules/utils

✓ Dependency removed successfully!
```

---

## Advanced Usage / الاستخدام المتقدم

### Example 6: CLI Application with Commands

**sad.toml:**
```toml
[package]
name = "my_cli"
version = "1.0.0"
description = "CLI application with commands"
authors = ["Developer <dev@example.com>"]

[build]
main = "src/main.s"
sources = ["src/"]
output = "build/"
type = "binary"

[dependencies]
cli_parser = "^1.5.0"
colors = "^0.8.0"

[scripts]
build = "sad build"
run = "sad run"
test = "sad test"
install-global = "cp build/my_cli /usr/local/bin/"
```

**src/main.s:**
```sad
// بسم الله الرحمن الرحيم
استورد "cli_parser" كـ CLI;
استورد "colors" كـ ألوان;

دالة عرض_مساعدة() {
    اطبع(ألوان.أزرق("my_cli - أداة سطر الأوامر"));
    اطبع("");
    اطبع("الاستخدام:");
    اطبع("  my_cli <command> [options]");
    اطبع("");
    اطبع("الأوامر:");
    اطبع("  init      - إنشاء مشروع جديد");
    اطبع("  build     - بناء المشروع");
    اطبع("  run       - تشغيل المشروع");
    اطبع("  help      - عرض المساعدة");
}

دالة رئيسية(وسائط: [نص]) {
    محلل = CLI.محلل();
    أوامر = محلل.حلل(وسائط);
    
    إذا (أوامر.طول == 0) {
        عرض_مساعدة();
        أرجع 1;
    }
    
    أمر = أوامر[0];
    
    إذا (أمر == "init") {
        اطبع(ألوان.أخضر("✓ إنشاء مشروع جديد..."));
        // Initialize logic
    } وإلا_إذا (أمر == "build") {
        اطبع(ألوان.أصفر("بناء المشروع..."));
        // Build logic
    } وإلا_إذا (أمر == "help") {
        عرض_مساعدة();
    } وإلا {
        اطبع(ألوان.أحمر("✗ أمر غير معروف:", أمر));
        أرجع 1;
    }
    
    أرجع 0;
}
```

**Usage:**
```bash
# Build
sad-pkg install
sad build

# Run
./build/my_cli init
./build/my_cli build
./build/my_cli help
```

### Example 7: HTTP Server Application

**sad.toml:**
```toml
[package]
name = "web_server"
version = "2.0.0"
description = "Simple HTTP server"

[dependencies]
http_server = "^3.0.0"
router = "^1.2.0"
json = "^2.0.0"
logger = "^0.5.0"

[scripts]
dev = "sad run -- --port 3000 --reload"
prod = "sad build --release && ./build/web_server --port 80"
test = "sad test"
```

**src/main.s:**
```sad
// بسم الله الرحمن الرحيم
استورد "http_server" كـ خادم_HTTP;
استورد "router" كـ موجه;
استورد "json" كـ JSON;
استورد "logger" كـ مسجل;

دالة معالج_الرئيسية(طلب، استجابة) {
    استجابة.أرسل(JSON.نص({
        "message": "مرحباً من خادم Sad!",
        "version": "2.0.0"
    }));
}

دالة معالج_المستخدمين(طلب، استجابة) {
    مستخدمون = [
        {"id": 1, "name": "أحمد"},
        {"id": 2, "name": "فاطمة"},
        {"id": 3, "name": "محمد"}
    ];
    
    استجابة.أرسل_JSON(مستخدمون);
}

دالة رئيسية() {
    // Create server
    خادم = خادم_HTTP.إنشاء();
    
    // Setup router
    موجه = موجه.جديد();
    موجه.احصل("/", معالج_الرئيسية);
    موجه.احصل("/users", معالج_المستخدمين);
    
    // Setup logger
    مسجل.إعداد({"level": "info"});
    
    // Start server
    منفذ = 3000;
    مسجل.معلومات("Starting server on port", منفذ);
    خادم.استمع(منفذ, موجه);
    
    أرجع 0;
}
```

**Run server:**
```bash
sad-pkg install
sad-pkg run dev
```

**Output:**
```
[INFO] Starting server on port 3000
[INFO] Server listening on http://localhost:3000
[INFO] Press Ctrl+C to stop
```

### Example 8: Library Package

**sad.toml:**
```toml
[package]
name = "string_utils"
version = "1.5.0"
description = "String manipulation utilities"
authors = ["Utils Team <team@utils.com>"]
license = "MIT"
repository = "https://github.com/sad-lang/string_utils"
keywords = ["string", "text", "utilities"]

[build]
main = "src/lib.s"
sources = ["src/"]
output = "build/"
type = "library"

[dependencies]
# No dependencies - pure library

[dev-dependencies]
test_framework = "^1.0.0"
benchmark = "^0.3.0"

[scripts]
test = "sad test"
bench = "sad benchmark"
docs = "sad doc"
```

**src/lib.s:**
```sad
// بسم الله الرحمن الرحيم
// string_utils - String manipulation library

// Reverse string - عكس النص
صدّر دالة عكس(نص: نص) -> نص {
    نتيجة = "";
    لكل حرف في نص.عكس() {
        نتيجة += حرف;
    }
    أرجع نتيجة;
}

// Convert to uppercase - تحويل إلى أحرف كبيرة
صدّر دالة أحرف_كبيرة(نص: نص) -> نص {
    // Implementation
    أرجع نص.أحرف_كبيرة();
}

// Convert to lowercase - تحويل إلى أحرف صغيرة
صدّر دالة أحرف_صغيرة(نص: نص) -> نص {
    أرجع نص.أحرف_صغيرة();
}

// Count words - عد الكلمات
صدّر دالة عد_كلمات(نص: نص) -> عدد {
    كلمات = نص.قسم(" ");
    أرجع كلمات.طول;
}

// Trim whitespace - إزالة المسافات
صدّر دالة تنظيف(نص: نص) -> نص {
    أرجع نص.تنظيف();
}
```

**Publish library:**
```bash
# Test
sad-pkg run test

# Publish to registry
sad-pkg publish
```

**Others can use it:**
```bash
sad-pkg add string_utils ^1.5.0
```

---

## Real-World Projects / مشاريع واقعية

### Example 9: RESTful API with Database

**sad.toml:**
```toml
[package]
name = "todo_api"
version = "1.0.0"
description = "RESTful TODO API with database"

[dependencies]
http_server = "^3.0.0"
router = "^1.2.0"
database = "^2.5.0"
json = "^2.0.0"
validator = "^0.8.0"
logger = "^0.5.0"
dotenv = "^1.0.0"

[dev-dependencies]
test_framework = "^1.0.0"

[scripts]
dev = "sad run -- --env development"
prod = "sad build --release && ./build/todo_api --env production"
test = "sad test"
migrate = "sad run scripts/migrate.s"
```

**src/main.s:**
```sad
// بسم الله الرحمن الرحيم
استورد "http_server" كـ HTTP;
استورد "router" كـ R;
استورد "database" كـ DB;
استورد "json" كـ JSON;
استورد "validator" كـ V;
استورد "dotenv";

// Database connection
قاعدة_بيانات: DB.اتصال;

// Initialize database
دالة إعداد_قاعدة_بيانات() {
    إعدادات = dotenv.تحميل();
    قاعدة_بيانات = DB.اتصال(إعدادات["DATABASE_URL"]);
    قاعدة_بيانات.اتصل();
}

// GET /todos - List all todos
دالة احصل_مهام(طلب، استجابة) {
    مهام = قاعدة_بيانات.استعلام("SELECT * FROM todos");
    استجابة.أرسل_JSON(مهام);
}

// POST /todos - Create todo
دالة أنشئ_مهمة(طلب، استجابة) {
    بيانات = JSON.حلل(طلب.محتوى);
    
    // Validate
    صحة = V.تحقق(بيانات, {
        "title": "required|string|max:200",
        "completed": "boolean"
    });
    
    إذا (!صحة.ناجح) {
        استجابة.حالة(400).أرسل_JSON({
            "error": "Validation failed",
            "errors": صحة.أخطاء
        });
        أرجع;
    }
    
    // Insert
    مهمة_جديدة = قاعدة_بيانات.أدخل("todos", بيانات);
    استجابة.حالة(201).أرسل_JSON(مهمة_جديدة);
}

// PUT /todos/:id - Update todo
دالة حدث_مهمة(طلب، استجابة) {
    معرف = طلب.معاملات["id"];
    بيانات = JSON.حلل(طلب.محتوى);
    
    محدثة = قاعدة_بيانات.حدث("todos", معرف, بيانات);
    
    إذا (محدثة) {
        استجابة.أرسل_JSON(محدثة);
    } وإلا {
        استجابة.حالة(404).أرسل_JSON({"error": "Not found"});
    }
}

// DELETE /todos/:id - Delete todo
دالة احذف_مهمة(طلب، استجابة) {
    معرف = طلب.معاملات["id"];
    حذف = قاعدة_بيانات.احذف("todos", معرف);
    
    إذا (حذف) {
        استجابة.حالة(204).أرسل();
    } وإلا {
        استجابة.حالة(404).أرسل_JSON({"error": "Not found"});
    }
}

دالة رئيسية() {
    // Setup database
    إعداد_قاعدة_بيانات();
    
    // Create server
    خادم = HTTP.إنشاء();
    موجه = R.جديد();
    
    // Routes
    موجه.احصل("/todos", احصل_مهام);
    موجه.ضع("/todos", أنشئ_مهمة);
    موجه.حدث("/todos/:id", حدث_مهمة);
    موجه.احذف("/todos/:id", احذف_مهمة);
    
    // Start
    منفذ = 3000;
    خادم.استمع(منفذ, موجه);
    
    أرجع 0;
}
```

### Example 10: Desktop Application

**sad.toml:**
```toml
[package]
name = "text_editor"
version = "1.0.0"
description = "Simple desktop text editor"

[dependencies]
gui = "^2.0.0"
file_system = "^1.5.0"
syntax_highlighter = "^0.8.0"

[build]
type = "application"
target = "desktop"
```

**src/main.s:**
```sad
// بسم الله الرحمن الرحيم
استورد "gui" كـ واجهة;
استورد "file_system" كـ ملفات;
استورد "syntax_highlighter" كـ تلوين;

صنف محرر_نصوص {
    نافذة: واجهة.نافذة;
    محتوى_النص: نص;
    مسار_الملف: نص;
    
    دالة __إنشاء__() {
        هذا.نافذة = واجهة.نافذة({
            "title": "محرر نصوص",
            "width": 800,
            "height": 600
        });
        
        هذا.إعداد_القوائم();
        هذا.إعداد_محرر_النص();
    }
    
    دالة إعداد_القوائم() {
        قائمة_ملف = هذا.نافذة.إضافة_قائمة("ملف");
        قائمة_ملف.إضافة_عنصر("جديد", هذا.ملف_جديد);
        قائمة_ملف.إضافة_عنصر("فتح", هذا.فتح_ملف);
        قائمة_ملف.إضافة_عنصر("حفظ", هذا.حفظ_ملف);
        قائمة_ملف.إضافة_فاصل();
        قائمة_ملف.إضافة_عنصر("خروج", هذا.خروج);
    }
    
    دالة إعداد_محرر_النص() {
        هذا.محرر = واجهة.محرر_نص({
            "syntax": "sad",
            "theme": "dark"
        });
        
        هذا.نافذة.إضافة_مكون(هذا.محرر);
    }
    
    دالة ملف_جديد() {
        هذا.محتوى_النص = "";
        هذا.محرر.نص = "";
    }
    
    دالة فتح_ملف() {
        مسار = واجهة.حوار_فتح_ملف();
        إذا (مسار) {
            هذا.مسار_الملف = مسار;
            هذا.محتوى_النص = ملفات.اقرأ(مسار);
            هذا.محرر.نص = هذا.محتوى_النص;
        }
    }
    
    دالة حفظ_ملف() {
        إذا (هذا.مسار_الملف) {
            ملفات.اكتب(هذا.مسار_الملف, هذا.محرر.نص);
        } وإلا {
            مسار = واجهة.حوار_حفظ_ملف();
            إذا (مسار) {
                هذا.مسار_الملف = مسار;
                ملفات.اكتب(مسار, هذا.محرر.نص);
            }
        }
    }
    
    دالة خروج() {
        هذا.نافذة.أغلق();
    }
    
    دالة تشغيل() {
        هذا.نافذة.اعرض();
    }
}

دالة رئيسية() {
    تطبيق = واجهة.تطبيق();
    محرر = محرر_نصوص.جديد();
    محرر.تشغيل();
    
    أرجع 0;
}
```

---

## Best Practices / أفضل الممارسات

### 1. Version Constraints

**Good practices:**
```toml
[dependencies]
# Use caret for libraries you trust
http = "^1.0.0"  # Will accept 1.x.x

# Use tilde for more conservative updates
json = "~2.3.0"  # Will only accept 2.3.x

# Use exact for critical dependencies
security = "1.5.2"  # Only 1.5.2

# Avoid wildcards in production
utils = "*"  # ❌ Bad for production
```

### 2. Organize Dependencies

```toml
[dependencies]
# Core dependencies
http_server = "^3.0.0"
database = "^2.5.0"

# Utilities
json = "^2.0.0"
logger = "^0.5.0"

[dev-dependencies]
# Testing only
test_framework = "^1.0.0"
mock = "^0.8.0"
benchmark = "^0.3.0"
```

### 3. Use Scripts

```toml
[scripts]
# Development
dev = "sad run -- --reload"
watch = "sad watch src/"

# Testing
test = "sad test"
test-watch = "sad test --watch"
coverage = "sad test --coverage"

# Production
build = "sad build --release"
deploy = "scripts/deploy.sh"
```

### 4. Lock File

```bash
# Always commit sad.lock
git add sad.lock

# Ensures reproducible builds
sad-pkg install  # Uses sad.lock if exists
```

---

**الحمد لله رب العالمين** 🤲

**Examples complete! / الأمثلة مكتملة!** ✅
