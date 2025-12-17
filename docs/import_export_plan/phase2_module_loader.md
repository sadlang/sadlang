# المرحلة 2: محمل الوحدات الذكي
# Phase 2: Smart Module Loader

**الحالة / Status:** 📅 مُخطط / Planned  
**الأولوية / Priority:** P1 (عالية / High)  
**الوقت المقدر / Estimated Time:** 2-3 أسابيع  
**المتطلبات السابقة / Prerequisites:** ✅ المرحلة 1

---

## 📋 نظرة عامة / Overview

### بالعربية
محمل وحدات ذكي ومُحسَّن يوفر:
- **نظام مسارات بحث متقدم** مع أولويات
- **تخزين مؤقت ذكي** لتسريع التحميل
- **تحليل اعتماديات تلقائي** وحل الترتيب
- **تحميل كسول** (Lazy Loading) للأداء
- **إعادة تحميل ساخن** (Hot Reload) للتطوير
- **إدارة إصدارات** متقدمة

### English
A smart and optimized module loader providing:
- **Advanced search path system** with priorities
- **Intelligent caching** for faster loading
- **Automatic dependency analysis** and resolution
- **Lazy loading** for performance
- **Hot reload** for development
- **Advanced version management**

---

## 🎯 الأهداف / Goals

### Core Features
1. ⬜ نظام مسارات بحث متعدد المستويات
2. ⬜ تخزين مؤقت للوحدات المُحلَّلة
3. ⬜ حل الاعتماديات التلقائي
4. ⬜ كشف التبعيات الدائرية
5. ⬜ التحميل الكسول (on-demand)
6. ⬜ إعادة التحميل الساخن
7. ⬜ دعم الإصدارات (Semver)

### Advanced Features
- ⬜ Pre-compilation caching
- ⬜ Parallel module loading
- ⬜ Module aliasing في التكوين
- ⬜ Conditional module loading

---

## 🗺️ نظام مسارات البحث / Search Path System

### ترتيب البحث / Search Order

```
1. الدليل الحالي / Current Directory
   ./module.s
   ./module/init.s

2. دليل المشروع / Project Directory
   ./lib/module.s
   ./modules/module.s

3. المكتبة القياسية / Standard Library
   <SAD_HOME>/stdlib/module.s

4. مسارات إضافية من التكوين / Config Paths
   ~/.sad/config.json → searchPaths

5. متغير البيئة SAD_PATH
   $SAD_PATH/module.s

6. مسارات النظام / System Paths
   /usr/local/lib/sad/module.s
```

---

### API للمسارات / Path API

```cpp
// include/modules/search_paths.h
#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace Sad {
namespace Modules {

/**
 * مدير مسارات البحث / Search Path Manager
 */
class SearchPathManager {
public:
    /**
     * الحصول على Instance
     */
    static SearchPathManager& getInstance();
    
    /**
     * إضافة مسار بحث
     * Add search path
     * 
     * @param path المسار المطلق
     * @param priority الأولوية (أعلى = يُفحص أولاً)
     */
    void addPath(const std::string& path, int priority = 50);
    
    /**
     * إزالة مسار
     */
    void removePath(const std::string& path);
    
    /**
     * الحصول على جميع المسارات مرتبة حسب الأولوية
     */
    std::vector<std::string> getPaths() const;
    
    /**
     * البحث عن وحدة
     * Search for module
     * 
     * @param moduleName اسم الوحدة
     * @return المسار الكامل أو ""
     */
    std::string findModule(const std::string& moduleName) const;
    
    /**
     * تحميل مسارات من ملف التكوين
     */
    void loadFromConfig(const std::string& configFile);
    
    /**
     * تحميل من متغير البيئة SAD_PATH
     */
    void loadFromEnvironment();
    
private:
    SearchPathManager();
    
    struct PathEntry {
        std::string path;
        int priority;
        
        bool operator<(const PathEntry& other) const {
            return priority > other.priority;  // Higher priority first
        }
    };
    
    std::vector<PathEntry> paths_;
    void initialize();
};

} // namespace Modules
} // namespace Sad
```

---

### أمثلة الاستخدام / Usage Examples

```s
# ملف: sad.config.json
{
    "searchPaths": [
        {
            "path": "./lib",
            "priority": 100
        },
        {
            "path": "./vendor/packages",
            "priority": 50
        },
        {
            "path": "~/.sad/modules",
            "priority": 30
        }
    ]
}
```

```s
# من داخل لغة ص / From within Sad language
استورد نظام.مسارات

# عرض المسارات الحالية
لكل مسار في نظام.مسارات.احصل()
    اطبع(مسار)
نهاية

# إضافة مسار جديد
نظام.مسارات.أضف("/مشاريعي/وحدات")

# البحث عن وحدة
نص مسار = نظام.مسارات.ابحث("رياضيات")
إذا مسار != ""
    اطبع(ن"وجدت: {مسار}")
نهاية
```

---

## 💾 نظام التخزين المؤقت / Caching System

### بنية الكاش / Cache Architecture

```
┌─────────────────────────────────────────┐
│         Module Cache                    │
│  ┌────────────────────────────────┐    │
│  │   Memory Cache (RAM)            │    │
│  │   - Parsed AST                  │    │
│  │   - Symbol Tables               │    │
│  │   - Metadata                    │    │
│  └────────────────────────────────┘    │
│                                         │
│  ┌────────────────────────────────┐    │
│  │   Disk Cache (.sad/cache/)     │    │
│  │   - Pre-compiled Bytecode      │    │
│  │   - Dependency Graph           │    │
│  │   - Checksums                  │    │
│  └────────────────────────────────┘    │
└─────────────────────────────────────────┘
```

---

### التنفيذ / Implementation

```cpp
// include/modules/module_cache.h
#pragma once

#include <string>
#include <memory>
#include <map>
#include <chrono>

namespace Sad {
namespace Modules {

/**
 * إدخال الكاش / Cache Entry
 */
struct CacheEntry {
    std::string moduleName;
    std::string filePath;
    std::unique_ptr<Module> module;
    
    std::chrono::system_clock::time_point loadTime;
    std::chrono::system_clock::time_point fileModTime;
    size_t fileSize;
    std::string checksum;
    
    bool isValid() const;
};

/**
 * مدير الكاش / Cache Manager
 */
class ModuleCache {
public:
    static ModuleCache& getInstance();
    
    /**
     * الحصول على وحدة من الكاش
     * Get module from cache
     * 
     * @param moduleName اسم الوحدة
     * @return Module* أو nullptr
     */
    Module* get(const std::string& moduleName);
    
    /**
     * إضافة وحدة للكاش
     * Add module to cache
     */
    void put(const std::string& moduleName, 
             std::unique_ptr<Module> module,
             const std::string& filePath);
    
    /**
     * تحقق من صحة الكاش
     * Check if cache is valid
     */
    bool isValid(const std::string& moduleName) const;
    
    /**
     * مسح الكاش
     * Clear cache
     */
    void clear();
    void clearModule(const std::string& moduleName);
    
    /**
     * حفظ الكاش على القرص
     * Save cache to disk
     */
    void saveToDisk();
    
    /**
     * تحميل الكاش من القرص
     * Load cache from disk
     */
    void loadFromDisk();
    
    /**
     * إحصائيات الكاش
     */
    struct Stats {
        size_t hits;
        size_t misses;
        size_t evictions;
        size_t memoryUsage;
        
        double hitRate() const {
            return hits / static_cast<double>(hits + misses);
        }
    };
    
    Stats getStats() const;
    
private:
    ModuleCache();
    
    std::map<std::string, CacheEntry> cache_;
    Stats stats_;
    
    std::string calculateChecksum(const std::string& filePath) const;
    bool needsReload(const CacheEntry& entry) const;
};

} // namespace Modules
} // namespace Sad
```

---

### استراتيجية الكاش / Caching Strategy

```
1. البحث في الكاش / Cache Lookup
   ↓
   موجود؟ / Exists?
   ├─ نعم → تحقق من الصلاحية / Check validity
   │         ├─ صالح → إرجاع من الكاش ✓
   │         └─ غير صالح → إعادة تحميل
   │
   └─ لا → تحميل من الملف
           ↓
           تحليل (Parse)
           ↓
           إضافة للكاش
           ↓
           إرجاع الوحدة
```

---

## 🔗 تحليل الاعتماديات / Dependency Analysis

### رسم الاعتماديات / Dependency Graph

```cpp
// include/modules/dependency_graph.h
#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>

namespace Sad {
namespace Modules {

/**
 * عقدة في رسم الاعتماديات
 */
struct DependencyNode {
    std::string moduleName;
    std::vector<std::string> dependencies;
    int loadOrder;  // ترتيب التحميل
    bool isLoaded;
};

/**
 * رسم الاعتماديات / Dependency Graph
 */
class DependencyGraph {
public:
    /**
     * إضافة وحدة
     */
    void addModule(const std::string& moduleName, 
                   const std::vector<std::string>& deps);
    
    /**
     * حل ترتيب التحميل (Topological Sort)
     * Resolve loading order
     * 
     * @return قائمة الوحدات بالترتيب الصحيح
     */
    std::vector<std::string> resolveLoadOrder();
    
    /**
     * كشف التبعيات الدائرية
     * Detect circular dependencies
     * 
     * @return true إذا وُجدت تبعية دائرية
     */
    bool hasCircularDependency();
    
    /**
     * الحصول على المسار الدائري
     * Get circular path
     */
    std::vector<std::string> getCircularPath();
    
    /**
     * الحصول على اعتماديات وحدة
     */
    std::vector<std::string> getDependencies(
        const std::string& moduleName) const;
    
    /**
     * الحصول على الوحدات التي تعتمد على وحدة معينة
     */
    std::vector<std::string> getDependents(
        const std::string& moduleName) const;
    
    /**
     * رسم تخطيطي (للتصور)
     */
    std::string toDot() const;  // GraphViz DOT format
    
private:
    std::map<std::string, DependencyNode> nodes_;
    
    bool dfs(const std::string& node,
             std::set<std::string>& visited,
             std::set<std::string>& recStack,
             std::vector<std::string>& path);
};

} // namespace Modules
} // namespace Sad
```

---

### مثال توضيحي / Example

```
# ملف: app.s
استورد logging
استورد database
استورد utils

# ملف: logging.s  
استورد utils

# ملف: database.s
استورد logging
استورد utils

# ملف: utils.s
# لا اعتماديات

الرسم / Graph:
    app
    ├─ logging
    │  └─ utils
    ├─ database
    │  ├─ logging
    │  │  └─ utils
    │  └─ utils
    └─ utils

ترتيب التحميل / Load Order:
1. utils       (لا اعتماديات)
2. logging     (يعتمد على utils)
3. database    (يعتمد على logging و utils)
4. app         (يعتمد على الكل)
```

---

## ⚡ التحميل الكسول / Lazy Loading

### الفكرة / Concept

```
# بدلاً من تحميل كل الوحدات فوراً:
استورد وحدة_كبيرة_جداً  # تحميل كامل (بطيء)

# استخدم التحميل الكسول:
@كسول استورد وحدة_كبيرة_جداً  # لا تحميل

# التحميل يحدث عند أول استخدام:
وحدة_كبيرة_جداً.دالة()  # الآن يتم التحميل
```

---

### التنفيذ / Implementation

```cpp
// include/modules/lazy_loader.h
#pragma once

namespace Sad {
namespace Modules {

/**
 * محمل كسول / Lazy Loader
 */
class LazyModuleProxy {
public:
    LazyModuleProxy(const std::string& moduleName);
    
    /**
     * الحصول على الوحدة (تحميل إذا لزم الأمر)
     */
    Module* getModule();
    
    /**
     * هل تم التحميل؟
     */
    bool isLoaded() const { return loaded_; }
    
private:
    std::string moduleName_;
    Module* module_;
    bool loaded_;
};

} // namespace Modules
} // namespace Sad
```

---

## 🔥 إعادة التحميل الساخن / Hot Reload

### للتطوير / For Development

```cpp
// include/modules/hot_reload.h
#pragma once

#include <thread>
#include <atomic>

namespace Sad {
namespace Modules {

/**
 * مراقب التغييرات / File Watcher
 */
class ModuleWatcher {
public:
    /**
     * بدء المراقبة
     */
    void start();
    
    /**
     * إيقاف المراقبة
     */
    void stop();
    
    /**
     * تسجيل callback عند التغيير
     */
    void onModuleChanged(
        std::function<void(const std::string&)> callback);
    
private:
    std::thread watcherThread_;
    std::atomic<bool> running_;
    
    void watchLoop();
    void checkModuleChanges();
};

/**
 * مدير إعادة التحميل
 */
class HotReloadManager {
public:
    /**
     * تفعيل إعادة التحميل الساخن
     */
    void enable();
    
    /**
     * تعطيل
     */
    void disable();
    
    /**
     * إعادة تحميل وحدة
     */
    bool reloadModule(const std::string& moduleName);
    
private:
    ModuleWatcher watcher_;
    
    void handleModuleChanged(const std::string& moduleName);
};

} // namespace Modules
} // namespace Sad
```

---

### الاستخدام / Usage

```s
# تفعيل إعادة التحميل الساخن في وضع التطوير
إذا وضع_التطوير
    نظام.تحميل_ساخن.فعل()
نهاية

# الآن أي تغيير في الملفات سيُعاد تحميله تلقائياً!
```

---

## 📦 إدارة الإصدارات / Version Management

### تنسيق Semantic Versioning

```
<major>.<minor>.<patch>

مثال: 2.5.3
- Major: 2 (تغييرات كبيرة غير متوافقة)
- Minor: 5 (ميزات جديدة متوافقة)
- Patch: 3 (إصلاحات)
```

---

### API الإصدارات / Versions API

```cpp
// include/modules/version.h
#pragma once

#include <string>

namespace Sad {
namespace Modules {

/**
 * معلومات الإصدار
 */
struct Version {
    int major;
    int minor;
    int patch;
    
    Version(int maj = 0, int min = 0, int pat = 0)
        : major(maj), minor(min), patch(pat) {}
    
    static Version parse(const std::string& versionStr);
    std::string toString() const;
    
    bool isCompatibleWith(const Version& required) const;
    
    bool operator==(const Version& other) const;
    bool operator<(const Version& other) const;
    bool operator>(const Version& other) const;
};

/**
 * متطلبات الإصدار
 */
struct VersionRequirement {
    enum class Operator {
        EXACT,      // ==
        GTE,        // >=
        GT,         // >
        LTE,        // <=
        LT,         // <
        CARET,      // ^ (متوافق)
        TILDE       // ~ (تقريبي)
    };
    
    Operator op;
    Version version;
    
    bool isSatisfiedBy(const Version& candidate) const;
    
    static VersionRequirement parse(const std::string& reqStr);
};

} // namespace Modules
} // namespace Sad
```

---

### الاستخدام / Usage

```s
# ملف: sad.json
{
    "name": "my-app",
    "version": "1.0.0",
    "dependencies": {
        "math-lib": "^2.0.0",      # >= 2.0.0 && < 3.0.0
        "utils": "~1.5.0",          # >= 1.5.0 && < 1.6.0
        "logger": ">=1.2.0"         # >= 1.2.0
    }
}
```

```cpp
// في الكود / In code
Version required = Version::parse("2.5.0");
Version installed = Version::parse("2.6.1");

if (installed.isCompatibleWith(required)) {
    // استخدم الوحدة
}
```

---

## 🧪 الاختبارات / Testing

### Test Cases

```cpp
// tests/modules/test_module_loader.cpp

/**
 * اختبار 1: نظام المسارات
 */
TEST(ModuleLoader, SearchPaths) {
    auto& manager = SearchPathManager::getInstance();
    
    manager.addPath("/custom/path", 100);
    auto paths = manager.getPaths();
    
    ASSERT_GT(paths.size(), 0);
    EXPECT_EQ(paths[0], "/custom/path");
}

/**
 * اختبار 2: التخزين المؤقت
 */
TEST(ModuleLoader, Caching) {
    auto& cache = ModuleCache::getInstance();
    cache.clear();
    
    // تحميل وحدة
    auto module = loadModule("test_module");
    ASSERT_NE(module, nullptr);
    
    // التحميل الثاني يجب أن يأتي من الكاش
    auto stats1 = cache.getStats();
    auto module2 = loadModule("test_module");
    auto stats2 = cache.getStats();
    
    EXPECT_EQ(stats2.hits, stats1.hits + 1);
}

/**
 * اختبار 3: تحليل الاعتماديات
 */
TEST(ModuleLoader, DependencyResolution) {
    DependencyGraph graph;
    
    graph.addModule("A", {"B", "C"});
    graph.addModule("B", {"C"});
    graph.addModule("C", {});
    
    auto order = graph.resolveLoadOrder();
    
    ASSERT_EQ(order.size(), 3);
    EXPECT_EQ(order[0], "C");
    EXPECT_EQ(order[1], "B");
    EXPECT_EQ(order[2], "A");
}

/**
 * اختبار 4: كشف التبعيات الدائرية
 */
TEST(ModuleLoader, CircularDependency) {
    DependencyGraph graph;
    
    graph.addModule("A", {"B"});
    graph.addModule("B", {"C"});
    graph.addModule("C", {"A"});  // دائري!
    
    EXPECT_TRUE(graph.hasCircularDependency());
    
    auto path = graph.getCircularPath();
    EXPECT_GT(path.size(), 0);
}

/**
 * اختبار 5: إدارة الإصدارات
 */
TEST(ModuleLoader, VersionManagement) {
    Version v1 = Version::parse("2.5.3");
    Version v2 = Version::parse("2.6.0");
    
    EXPECT_LT(v1, v2);
    
    VersionRequirement req = VersionRequirement::parse("^2.0.0");
    EXPECT_TRUE(req.isSatisfiedBy(v1));
    EXPECT_TRUE(req.isSatisfiedBy(v2));
}
```

---

## 📊 مؤشرات الأداء / Performance Metrics

### الأهداف / Targets

- ⚡ **وقت البحث عن وحدة:** < 1ms
- ⚡ **وقت التحميل من الكاش:** < 0.5ms
- ⚡ **وقت التحميل من الملف:** < 20ms
- ⚡ **دقة كشف التبعيات الدائرية:** 100%
- ⚡ **استهلاك الذاكرة:** < 50KB لكل وحدة مُخَزَّنة
- ✅ **معدل نجاح الكاش:** > 80%

---

## 📚 ملفات التنفيذ / Implementation Files

```
src/modules/
├── search_paths.{h,cpp}           # نظام المسارات
├── module_cache.{h,cpp}           # التخزين المؤقت
├── dependency_graph.{h,cpp}       # تحليل الاعتماديات
├── lazy_loader.{h,cpp}            # التحميل الكسول
├── hot_reload.{h,cpp}             # إعادة التحميل الساخن
├── version.{h,cpp}                # إدارة الإصدارات
└── module_loader.{h,cpp}          # المحمل الرئيسي (يجمع كل شيء)

tests/modules/
├── test_search_paths.cpp
├── test_module_cache.cpp
├── test_dependency_graph.cpp
├── test_lazy_loading.cpp
├── test_hot_reload.cpp
└── test_version.cpp
```

---

## ✅ قائمة المهام / Task Checklist

### الأسبوع 1
- [ ] تنفيذ `SearchPathManager`
- [ ] نظام الأولويات للمسارات
- [ ] تحميل من التكوين/البيئة
- [ ] اختبارات المسارات

### الأسبوع 2
- [ ] تنفيذ `ModuleCache`
- [ ] الكاش في الذاكرة
- [ ] الكاش على القرص
- [ ] التحقق من الصلاحية (checksums)
- [ ] اختبارات الكاش

### الأسبوع 3
- [ ] تنفيذ `DependencyGraph`
- [ ] حل ترتيب التحميل
- [ ] كشف التبعيات الدائرية
- [ ] التحميل الكسول
- [ ] إعادة التحميل الساخن
- [ ] نظام الإصدارات
- [ ] اختبارات شاملة

---

## 🚀 الخطوة التالية / Next Step

بعد إكمال هذه المرحلة، انتقل إلى:
**[المرحلة 3: خادم اللغة (LSP)](phase3_language_server.md)**

---

**آخر تحديث / Last Updated:** 8 ديسمبر 2025
