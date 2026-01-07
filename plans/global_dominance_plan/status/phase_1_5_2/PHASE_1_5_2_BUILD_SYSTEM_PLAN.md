# بسم الله الرحمن الرحيم
# 🔨 تقرير Phase 1.5.2 - Build System Implementation
# Phase 1.5.2 Report - Build System for Sad Projects

**التاريخ / Date:** 7 يناير 2026  
**المرحلة / Phase:** Phase 1.5.2 - Build System  
**الحالة / Status:** 🔴 **لم تبدأ / Not Started (0% → 100%)**

---

## 📋 ملخص تنفيذي / Executive Summary

Build System هو نظام لبناء وإدارة مشاريع لغة ص. سيكون مشابهاً لـ:
- `cargo build` (Rust)
- `go build` (Go)
- `npm run build` (Node.js)

---

## 🎯 الأهداف / Goals

### الهدف الرئيسي
توفير نظام بناء بسيط وقوي لمشاريع لغة ص يعتمد على ملف `sad.toml`.

### الميزات المطلوبة
1. ✅ قراءة `sad.toml`
2. ✅ بناء المشروع
3. ✅ تشغيل المشروع
4. ✅ اختبار المشروع
5. ✅ Custom tasks
6. ✅ Caching للبناء السريع
7. ✅ Watch mode (إعادة بناء تلقائية)

---

## 📝 تصميم sad.toml / sad.toml Design

### الهيكل الأساسي

```toml
[package]
name = "my_project"
version = "1.0.0"
description = "My Sad project"
authors = ["Ahmed <ahmed@example.com>"]

[build]
# Main entry point - نقطة الدخول الرئيسية
main = "src/main.s"

# Output directory - مجلد الإخراج
output = "build/"

# Source directories - مجلدات المصدر
sources = ["src/", "lib/"]

# Optimization level - مستوى التحسين
# Options: "none", "basic", "full"
optimization = "full"

# Target platform - المنصة المستهدفة
# Options: "x86_64", "arm64", "wasm"
target = "x86_64"

[dependencies]
http = "^1.0.0"
json = "~2.3.0"

[dev-dependencies]
test_framework = "^0.5.0"

[tasks]
# Custom build tasks - مهام بناء مخصصة

[tasks.build]
command = "sadc"
args = ["${main}", "-o", "${output}/app"]
description = "Build the project"

[tasks.run]
command = "${output}/app"
description = "Run the application"
depends = ["build"]

[tasks.test]
command = "sadc"
args = ["tests/*.s", "--test"]
description = "Run tests"

[tasks.clean]
command = "rm"
args = ["-rf", "${output}"]
description = "Clean build artifacts"

[tasks.watch]
command = "watch"
args = ["src/", "--exec", "build"]
description = "Watch for changes and rebuild"
```

---

## 🏗️ البنية المعمارية / Architecture

### المكونات الأساسية

```
BuildSystem
├── ConfigParser (قراءة sad.toml)
├── TaskRunner (تنفيذ المهام)
├── Builder (البناء)
│   ├── Compiler Integration
│   ├── Dependency Manager
│   └── Output Manager
├── Cache Manager (التخزين المؤقت)
└── Watcher (مراقبة التغييرات)
```

### تدفق البناء

```
1. Parse sad.toml
2. Resolve dependencies
3. Check cache
4. If outdated:
   ├─ Compile sources
   ├─ Link binaries
   └─ Update cache
5. Run post-build tasks
```

---

## 📂 البنية الملفات المقترحة / Proposed File Structure

### ملفات جديدة مطلوبة

```
tools/build/
├── include/
│   ├── build_system.h          # النظام الرئيسي
│   ├── config_parser.h         # قراءة sad.toml
│   ├── task_runner.h           # تنفيذ المهام
│   ├── builder.h               # محرك البناء
│   ├── cache_manager.h         # إدارة التخزين المؤقت
│   └── watcher.h               # مراقبة التغييرات
│
├── src/
│   ├── build_system.cpp
│   ├── config_parser.cpp
│   ├── task_runner.cpp
│   ├── builder.cpp
│   ├── cache_manager.cpp
│   └── watcher.cpp
│
├── main.cpp                    # نقطة الدخول
├── CMakeLists.txt
└── README.md
```

---

## 💻 الأوامر المطلوبة / Required Commands

### sad build
```bash
# Build the project - بناء المشروع
sad build

# Build with optimization - بناء مع تحسين
sad build --release

# Build for specific target - بناء لمنصة محددة
sad build --target wasm

# Clean build - بناء نظيف
sad build --clean
```

### sad run
```bash
# Run the project - تشغيل المشروع
sad run

# Run with arguments - تشغيل مع وسائط
sad run -- arg1 arg2

# Run in watch mode - تشغيل مع مراقبة
sad run --watch
```

### sad test
```bash
# Run tests - تشغيل الاختبارات
sad test

# Run specific test - تشغيل اختبار محدد
sad test --test integration_test

# Run tests with coverage - اختبارات مع تغطية
sad test --coverage
```

### sad task
```bash
# List all tasks - سرد جميع المهام
sad task list

# Run custom task - تشغيل مهمة مخصصة
sad task run my_task

# Run multiple tasks - تشغيل عدة مهام
sad task run build test
```

### sad clean
```bash
# Clean build artifacts - تنظيف مخرجات البناء
sad clean

# Clean everything including cache - تنظيف كل شيء
sad clean --all
```

### sad watch
```bash
# Watch for changes - مراقبة التغييرات
sad watch

# Watch specific directories - مراقبة مجلدات محددة
sad watch src/ lib/

# Watch and run custom task - مراقبة وتشغيل مهمة
sad watch --task build
```

---

## 🔧 تنفيذ build_system.h

```cpp
// بسم الله الرحمن الرحيم
/**
 * @file build_system.h
 * @brief Build System for Sad Language Projects
 * 
 * نظام بناء مشاريع لغة ص
 * Build system for Sad Language projects
 */

#pragma once

#include "config_parser.h"
#include "task_runner.h"
#include "builder.h"
#include "cache_manager.h"
#include <string>
#include <memory>

namespace sad {
namespace build {

/**
 * @brief Build options - خيارات البناء
 */
struct BuildOptions {
    std::string target = "x86_64";      ///< Target platform
    std::string optimization = "none";  ///< Optimization level
    bool clean = false;                 ///< Clean build
    bool release = false;               ///< Release build
    bool verbose = false;               ///< Verbose output
};

/**
 * @brief Build result - نتيجة البناء
 */
struct BuildResult {
    bool success = false;
    std::string output_path;
    std::string error_message;
    double build_time_seconds = 0.0;
};

/**
 * @brief Main build system class
 */
class BuildSystem {
public:
    /**
     * @brief Initialize build system
     * @param config_path Path to sad.toml
     */
    explicit BuildSystem(const std::string& config_path = "sad.toml");

    /**
     * @brief Build the project
     */
    BuildResult build(const BuildOptions& options = BuildOptions{});

    /**
     * @brief Run the project
     */
    int run(const std::vector<std::string>& args = {});

    /**
     * @brief Run tests
     */
    BuildResult test(const std::string& test_name = "");

    /**
     * @brief Run custom task
     */
    int runTask(const std::string& task_name);

    /**
     * @brief Clean build artifacts
     */
    bool clean(bool all = false);

    /**
     * @brief Watch for changes
     */
    void watch(const std::vector<std::string>& paths = {});

private:
    std::unique_ptr<ConfigParser> config_;
    std::unique_ptr<TaskRunner> task_runner_;
    std::unique_ptr<Builder> builder_;
    std::unique_ptr<CacheManager> cache_;
};

} // namespace build
} // namespace sad
```

---

## 🔧 تنفيذ task_runner.h

```cpp
// بسم الله الرحمن الرحيم
/**
 * @file task_runner.h
 * @brief Task Runner - منفذ المهام
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>

namespace sad {
namespace build {

/**
 * @brief Task definition - تعريف المهمة
 */
struct Task {
    std::string name;
    std::string description;
    std::string command;
    std::vector<std::string> args;
    std::vector<std::string> depends;  ///< Dependencies
    bool is_background = false;
};

/**
 * @brief Task execution result
 */
struct TaskResult {
    bool success = false;
    int exit_code = 0;
    std::string output;
    std::string error;
};

/**
 * @brief Task runner class
 */
class TaskRunner {
public:
    /**
     * @brief Register a task
     */
    void registerTask(const Task& task);

    /**
     * @brief Run a task
     */
    TaskResult run(const std::string& task_name);

    /**
     * @brief Run multiple tasks
     */
    bool runAll(const std::vector<std::string>& task_names);

    /**
     * @brief List all tasks
     */
    std::vector<Task> listTasks() const;

    /**
     * @brief Get task by name
     */
    const Task* getTask(const std::string& name) const;

private:
    std::map<std::string, Task> tasks_;
    
    TaskResult executeCommand(const std::string& command,
                            const std::vector<std::string>& args);
    
    bool resolveDependencies(const std::string& task_name,
                           std::vector<std::string>& order);
};

} // namespace build
} // namespace sad
```

---

## 📊 Cache System / نظام التخزين المؤقت

### الغرض
تسريع البناء بتخزين نتائج البناء السابقة وإعادة استخدامها إذا لم تتغير الملفات.

### الآلية

```cpp
/**
 * @file cache_manager.h
 * @brief Build Cache Manager
 */

class CacheManager {
public:
    /**
     * @brief Check if file is cached and up-to-date
     */
    bool isCached(const std::string& source_file);

    /**
     * @brief Get cached output
     */
    std::string getCachedOutput(const std::string& source_file);

    /**
     * @brief Update cache
     */
    void updateCache(const std::string& source_file,
                    const std::string& output_file);

    /**
     * @brief Clear cache
     */
    void clear();

private:
    struct CacheEntry {
        std::string source_hash;
        std::time_t timestamp;
        std::string output_path;
    };

    std::map<std::string, CacheEntry> cache_;
    std::string cache_dir_ = ".sad_cache/";
};
```

### Cache File Format (.sad_cache/cache.json)

```json
{
  "version": "1.0.0",
  "entries": [
    {
      "source": "src/main.s",
      "hash": "abc123...",
      "timestamp": 1704671234,
      "output": "build/main.o"
    }
  ]
}
```

---

## 🔍 Watch Mode / وضع المراقبة

### الغرض
مراقبة التغييرات في الملفات المصدر وإعادة البناء تلقائياً.

### التنفيذ

```cpp
/**
 * @file watcher.h
 * @brief File System Watcher
 */

class Watcher {
public:
    using Callback = std::function<void(const std::string& path)>;

    /**
     * @brief Start watching directories
     */
    void watch(const std::vector<std::string>& paths,
              Callback on_change);

    /**
     * @brief Stop watching
     */
    void stop();

private:
    void pollChanges();
    std::map<std::string, std::time_t> file_timestamps_;
};
```

### استخدام

```bash
# Watch and rebuild on changes
sad watch
# → Watching src/
# → File changed: src/main.s
# → Rebuilding...
# → Build successful in 1.2s
```

---

## 🎯 أمثلة الاستخدام / Usage Examples

### مثال 1: مشروع بسيط

```toml
# sad.toml
[package]
name = "hello"
version = "1.0.0"

[build]
main = "main.s"
output = "build/"
```

```bash
# Build
$ sad build
Building hello v1.0.0...
Compiling main.s
Linking build/hello
Finished in 0.5s

# Run
$ sad run
مرحباً بكم في لغة ص!
```

### مثال 2: مشروع مع dependencies

```toml
[package]
name = "web_app"
version = "1.0.0"

[dependencies]
http = "^1.0.0"
json = "~2.3.0"

[build]
main = "src/main.s"
output = "build/"
```

```bash
$ sad build
Resolving dependencies...
  - http v1.2.0
  - json v2.3.5
Building web_app v1.0.0...
Compiling src/main.s
Linking build/web_app
Finished in 2.1s
```

### مثال 3: Custom Tasks

```toml
[tasks.deploy]
command = "scp"
args = ["${output}/app", "server:/var/www/"]
description = "Deploy to server"
depends = ["build", "test"]
```

```bash
$ sad task run deploy
Running task 'build'...
✓ Build successful
Running task 'test'...
✓ Tests passed
Running task 'deploy'...
✓ Deployed to server
```

---

## 📈 خطة التنفيذ / Implementation Plan

### الأسبوع 1: Core System

**يوم 1-2:** ConfigParser
- قراءة sad.toml
- Validation
- Default values

**يوم 3-4:** TaskRunner
- Task registration
- Task execution
- Dependency resolution

**يوم 5:** Builder
- Integration مع المترجم
- Output management

### الأسبوع 2: Advanced Features

**يوم 1-2:** CacheManager
- Cache implementation
- Hash calculation
- Cache invalidation

**يوم 3-4:** Watcher
- File monitoring
- Auto-rebuild
- Debouncing

**يوم 5:** Testing & Documentation
- Unit tests
- Integration tests
- Documentation

---

## 🎯 معايير الإكمال / Completion Criteria

Phase 1.5.2 مكتملة عندما:

### Code ✅
- [ ] جميع header files (6 files)
- [ ] جميع implementation files (6 files)
- [ ] main.cpp للـ CLI
- [ ] CMakeLists.txt

### Commands ✅
- [ ] `sad build` يعمل
- [ ] `sad run` يعمل
- [ ] `sad test` يعمل
- [ ] `sad task` يعمل
- [ ] `sad clean` يعمل
- [ ] `sad watch` يعمل

### Features ✅
- [ ] يقرأ sad.toml بنجاح
- [ ] يبني المشاريع
- [ ] Cache يعمل (تسريع 5x+)
- [ ] Watch mode يعمل
- [ ] Custom tasks تعمل

### Tests ✅
- [ ] 10+ unit tests
- [ ] 5+ integration tests
- [ ] End-to-end tests

### Documentation ✅
- [ ] README.md
- [ ] API documentation
- [ ] Usage examples
- [ ] Configuration guide

---

## 🏆 النتيجة المتوقعة / Expected Outcome

Build System احترافي مشابه لـ:
- ✅ cargo (Rust)
- ✅ go build (Go)
- ✅ npm scripts (Node.js)

لكن:
- ✅ أبسط في الاستخدام
- ✅ أسرع (caching)
- ✅ دعم كامل للعربية
- ✅ مدمج مع sad-pkg

---

**الحالة:** 0%  
**الهدف:** 100%  
**المدة المتوقعة:** 2 أسبوع

**الحمد لله رب العالمين** 🤲
