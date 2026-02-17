# بسم الله الرحمن الرحيم
# Sad Build System - نظام بناء لغة ص

نظام بناء احترافي لمشاريع لغة ص، مشابه لـ `cargo build` (Rust) و `go build` (Go).

## ✨ الميزات / Features

- ⚡ **Fast Incremental Builds** - بناء متدرج سريع مع تخزين مؤقت
- 🔄 **Watch Mode** - إعادة بناء تلقائية عند تغيير الملفات
- 🎯 **Task Runner** - تشغيل مهام مخصصة مع حل التبعيات
- 📦 **TOML Configuration** - إعدادات بسيطة عبر sad.toml
- 🌍 **Multi-Platform** - دعم Windows, Linux, macOS
- 🚀 **Optimization Levels** - مستويات تحسين متعددة

## 📦 التثبيت / Installation

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
cmake --install .
```

## 🚀 الاستخدام السريع / Quick Start

### 1. إنشاء sad.toml

```toml
[package]
name = "my_project"
version = "1.0.0"
description = "My Sad project"

[build]
main = "src/main.s"
output = "build/"
optimization = "full"

[dependencies]
http = "^1.0.0"

[tasks.build]
command = "sadc"
args = ["${main}", "-o", "${output}/app"]
description = "Build the project"

[tasks.run]
command = "${output}/app"
description = "Run the application"
depends = ["build"]
```

### 2. بناء المشروع

```bash
# بناء عادي
sad build

# بناء محسّن (release)
sad build --release

# بناء نظيف (إعادة بناء كل شيء)
sad build --clean

# بناء مع تفاصيل
sad build --verbose
```

### 3. تشغيل المشروع

```bash
# تشغيل
sad run

# تشغيل مع وسائط
sad run -- --help

# تشغيل مع watch mode
sad run --watch
```

### 4. وضع المراقبة

```bash
# مراقبة src/ و lib/
sad watch

# مراقبة مجلدات محددة
sad watch src/ lib/
```

## 📋 الأوامر / Commands

### Build

```bash
sad build                # بناء المشروع
sad build --release      # بناء محسّن
sad build --clean        # بناء نظيف
sad build --verbose      # بناء مع تفاصيل
sad build --target wasm  # بناء لمنصة محددة
```

### Run

```bash
sad run                  # تشغيل المشروع
sad run -- arg1 arg2     # تشغيل مع وسائط
sad run --watch          # تشغيل مع إعادة بناء تلقائية
```

### Test

```bash
sad test                 # تشغيل الاختبارات
sad test --verbose       # اختبارات مع تفاصيل
sad test --coverage      # اختبارات مع تغطية
```

### Tasks

```bash
sad task <name>          # تشغيل مهمة مخصصة
sad tasks                # سرد جميع المهام
```

### Clean

```bash
sad clean                # تنظيف مخرجات البناء
sad clean --all          # تنظيف كل شيء (بما في ذلك cache)
```

### Watch

```bash
sad watch                # مراقبة التغييرات
sad watch src/ lib/      # مراقبة مجلدات محددة
```

## 📝 sad.toml Configuration

### Package Section

```toml
[package]
name = "project_name"
version = "1.0.0"
description = "Project description"
authors = ["Author <email@example.com>"]
license = "MIT"
```

### Build Section

```toml
[build]
main = "src/main.s"              # Main entry point
output = "build/"                # Output directory
sources = ["src/", "lib/"]       # Source directories
optimization = "full"            # none, basic, full
target = "x86_64"                # x86_64, arm64, wasm
type = "binary"                  # binary, library
```

### Dependencies

```toml
[dependencies]
http = "^1.0.0"
json = "~2.3.0"

[dev-dependencies]
test_framework = "^0.5.0"
```

### Custom Tasks

```toml
[tasks.deploy]
command = "scp"
args = ["${output}/app", "server:/var/www/"]
description = "Deploy to server"
depends = ["build", "test"]

[tasks.format]
command = "sadfmt"
args = ["src/"]
description = "Format source code"
```

## 🎯 Variable Substitution

يدعم نظام البناء استبدال المتغيرات:

- `${main}` - ملف الدخول الرئيسي
- `${output}` - مجلد الإخراج
- `${name}` - اسم المشروع
- `${version}` - نسخة المشروع

**مثال:**
```toml
[tasks.build]
command = "sadc"
args = ["${main}", "-o", "${output}/${name}"]
```

## ⚡ Build Caching

يستخدم نظام البناء تخزين مؤقت ذكي لتسريع البناء المتدرج:

```
First build:  2-5 seconds (10 files)
Incremental:  0.3-1 second (1 changed file)
Cache hit:    80-90%
```

### Cache Structure

```
.sad_cache/
├── cache.json          # Metadata
└── objects/            # Compiled files
    ├── abc123.o
    └── def456.o
```

### Cache Invalidation

يتم إبطال الcache تلقائياً عند:
- تغيير الملف المصدر
- تغيير مستوى التحسين
- تغيير المنصة المستهدفة
- تغيير نسخة المترجم

## 🔄 Watch Mode

وضع المراقبة يكتشف التغييرات تلقائياً:

```bash
$ sad watch
Watching src/
→ File changed: src/main.s
→ Rebuilding...
→ Build successful in 0.8s

→ File changed: src/utils.s
→ Rebuilding...
→ Build successful in 0.3s  (cached: 9/10 files)
```

### Configuration

```cpp
WatcherConfig config;
config.poll_interval = std::chrono::milliseconds(500);
config.debounce_delay = std::chrono::milliseconds(1000);
config.ignore_patterns = {"*.o", "build/", ".git/"};
```

## 📊 API Usage

### C++ API

```cpp
#include <sad/build/build_system.h>

using namespace sad::build;

int main() {
    // Initialize
    BuildSystem bs("sad.toml");
    
    // Build
    BuildOptions opts;
    opts.release = true;
    BuildResult result = bs.build(opts);
    
    if (result.success) {
        std::cout << "Build successful!\n";
        std::cout << "Time: " << result.build_time_seconds << "s\n";
        std::cout << "Compiled: " << result.files_compiled << "\n";
        std::cout << "Cached: " << result.files_cached << "\n";
        
        // Run
        bs.run();
    }
    
    return 0;
}
```

### Task Runner API

```cpp
TaskRunner runner;

Task build_task;
build_task.name = "build";
build_task.command = "sadc";
build_task.args = {"src/main.s", "-o", "build/app"};
runner.registerTask(build_task);

Task run_task;
run_task.name = "run";
run_task.command = "build/app";
run_task.depends = {"build"};
runner.registerTask(run_task);

TaskResult result = runner.run("run");
```

## 🏗️ Architecture

```
BuildSystem (Main Interface)
    │
    ├── ConfigParser (Parse sad.toml)
    ├── Builder (Compile & Link)
    │   └── CacheManager (Build Cache)
    ├── TaskRunner (Execute Tasks)
    └── Watcher (File Monitoring)
```

## 🎯 Examples

### Example 1: CLI Application

```toml
[package]
name = "my_cli"
version = "1.0.0"

[build]
main = "src/main.s"
output = "build/"

[tasks.install]
command = "cp"
args = ["${output}/my_cli", "/usr/local/bin/"]
description = "Install CLI tool"
depends = ["build"]
```

```bash
sad build
sad run -- --help
sad task install
```

### Example 2: Web Application

```toml
[package]
name = "my_web_app"
version = "1.0.0"

[dependencies]
http = "^1.0.0"

[build]
main = "src/main.s"
output = "build/"
optimization = "full"

[tasks.dev]
command = "${output}/my_web_app"
args = ["--port", "3000", "--reload"]
description = "Run development server"
depends = ["build"]

[tasks.deploy]
command = "rsync"
args = ["-avz", "${output}/", "server:/var/www/"]
description = "Deploy to production"
depends = ["build", "test"]
```

```bash
sad task dev          # Development server
sad task deploy       # Deploy to production
```

## 🐛 Troubleshooting

### Build fails

```bash
# Clean build
sad clean --all
sad build --verbose
```

### Cache issues

```bash
# Clear cache
sad clean --all

# Rebuild
sad build
```

### Watch mode not detecting changes

```bash
# Check watched paths
sad watch --verbose
```

## 📄 License

MIT License

## 🤝 Contributing

Contributions welcome! Please follow the coding standards and add tests for new features.

## 📞 Support

For issues and questions:
- GitHub Issues: [github.com/sad-lang/build](https://github.com/sad-lang/build)
- Documentation: [docs.sad-lang.org/build](https://docs.sad-lang.org/build)

---

**الحمد لله رب العالمين** 🤲
