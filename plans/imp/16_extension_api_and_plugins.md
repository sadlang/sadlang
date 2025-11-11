# 🔌 خطة Extension API والـ Plugins / Extension API & Plugins Implementation Plan

**التاريخ:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**المدة المقدرة:** 1 أسبوع

---

## 📋 ملخص / Summary

### (AR) الملخص
خطة تفصيلية لتطوير Extension API وPlugin System للغة "ص". يسمح بتطوير extensions بلغات أخرى (C/C++/Python) وربطها ديناميكياً.

### (EN) Summary
Detailed plan for developing Extension API and Plugin System for Sad language. Allows developing extensions in other languages (C/C++/Python) and dynamically linking them.

---

## 🎯 الأهداف / Objectives

1. ✅ تعريف C API للـ language core
2. ✅ Plugin loading system (DLL/SO)
3. ✅ Native function registration
4. ✅ Value marshalling (C ↔ Sad)
5. ✅ Error handling في plugins
6. ✅ مثال Plugin بلغة C++
7. ⏳ Python bindings (Phase 3)
8. ⏳ Security/sandboxing (Phase 3)

---

## 📦 الاعتماديات / Dependencies

### من plans:
- ✅ `09_integration_and_architecture.md` - Core interfaces
- ✅ `12_interpreter_runtime_plan.md` - Value system
- ✅ `13_stdlib_and_modules_plan.md` - StdLib Manager

---

## 🏗️ البنية المعمارية / Architecture

### C API

```c
// include/api/sad_api.h
#ifndef SAD_API_H
#define SAD_API_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ===== Version =====
#define SAD_API_VERSION_MAJOR 1
#define SAD_API_VERSION_MINOR 0
#define SAD_API_VERSION_PATCH 0

// ===== Types =====
typedef enum {
    SAD_TYPE_NONE,
    SAD_TYPE_INTEGER,
    SAD_TYPE_FLOAT,
    SAD_TYPE_STRING,
    SAD_TYPE_BOOLEAN,
    SAD_TYPE_ARRAY,
    SAD_TYPE_MAP,
    SAD_TYPE_FUNCTION
} SadValueType;

// Opaque handle to Sad Value
typedef struct SadValue_* SadValue;

// Opaque handle to Sad Runtime
typedef struct SadRuntime_* SadRuntime;

// Native function signature
typedef SadValue (*SadNativeFunction)(SadRuntime* runtime, 
                                       SadValue* args, 
                                       int argc);

// ===== Value Creation =====
SadValue sad_value_new_none(void);
SadValue sad_value_new_int(int64_t value);
SadValue sad_value_new_float(double value);
SadValue sad_value_new_string(const char* value);
SadValue sad_value_new_bool(bool value);
SadValue sad_value_new_array(SadValue* elements, int count);

// ===== Value Getters =====
SadValueType sad_value_get_type(SadValue value);
int64_t sad_value_as_int(SadValue value);
double sad_value_as_float(SadValue value);
const char* sad_value_as_string(SadValue value);
bool sad_value_as_bool(SadValue value);

// ===== Value Type Checks =====
bool sad_value_is_int(SadValue value);
bool sad_value_is_float(SadValue value);
bool sad_value_is_string(SadValue value);
bool sad_value_is_bool(SadValue value);
bool sad_value_is_array(SadValue value);

// ===== Array Operations =====
int sad_array_size(SadValue array);
SadValue sad_array_get(SadValue array, int index);
void sad_array_set(SadValue array, int index, SadValue value);
void sad_array_push(SadValue array, SadValue value);

// ===== Runtime Functions =====
SadRuntime* sad_runtime_new(void);
void sad_runtime_free(SadRuntime* runtime);
void sad_runtime_register_function(SadRuntime* runtime, 
                                   const char* moduleName,
                                   const char* funcName,
                                   SadNativeFunction func);
SadValue sad_runtime_execute(SadRuntime* runtime, const char* code);

// ===== Error Handling =====
void sad_set_error(SadRuntime* runtime, const char* message);
const char* sad_get_error(SadRuntime* runtime);
bool sad_has_error(SadRuntime* runtime);
void sad_clear_error(SadRuntime* runtime);

// ===== Plugin Interface =====
typedef struct {
    const char* name;
    const char* version;
    const char* author;
    const char* description;
    
    // Initialization callback
    bool (*initialize)(SadRuntime* runtime);
    
    // Cleanup callback
    void (*finalize)(SadRuntime* runtime);
} SadPluginInfo;

// Plugin must export this symbol
#define SAD_PLUGIN_EXPORT \
    __declspec(dllexport) SadPluginInfo* sad_plugin_get_info(void)

#ifdef __cplusplus
}
#endif

#endif // SAD_API_H
```

---

### C++ Implementation (Bridge)

```cpp
// src/api/sad_api.cpp
#include "api/sad_api.h"
#include "data/types/value.h"
#include "interpreter/core/interpreter_core.h"
#include "stdlib/core/stdlib_manager.h"

using namespace Sad::Data;
using namespace Sad::Interpreter;
using namespace Sad::StdLib;

// ===== Internal Structures =====
struct SadValue_ {
    Value* value;
    
    SadValue_(Value* v) : value(v) {}
    ~SadValue_() { delete value; }
};

struct SadRuntime_ {
    InterpreterCore* interpreter;
    StdLibManager* stdlibManager;
    std::string lastError;
    
    SadRuntime_() {
        interpreter = new InterpreterCore();
        stdlibManager = new StdLibManager();
        stdlibManager->initializeStandardModules();
    }
    
    ~SadRuntime_() {
        delete interpreter;
        delete stdlibManager;
    }
};

// ===== Value Creation =====
SadValue sad_value_new_none(void) {
    return new SadValue_(new Value());
}

SadValue sad_value_new_int(int64_t value) {
    return new SadValue_(new Value(value));
}

SadValue sad_value_new_float(double value) {
    return new SadValue_(new Value(value));
}

SadValue sad_value_new_string(const char* value) {
    return new SadValue_(new Value(std::string(value)));
}

SadValue sad_value_new_bool(bool value) {
    return new SadValue_(new Value(value));
}

SadValue sad_value_new_array(SadValue* elements, int count) {
    std::vector<Value> vec;
    for (int i = 0; i < count; ++i) {
        vec.push_back(*elements[i]->value);
    }
    return new SadValue_(new Value(Value::makeArray(vec)));
}

// ===== Value Getters =====
SadValueType sad_value_get_type(SadValue value) {
    if (value->value->isInteger()) return SAD_TYPE_INTEGER;
    if (value->value->isFloat()) return SAD_TYPE_FLOAT;
    if (value->value->isString()) return SAD_TYPE_STRING;
    if (value->value->isBool()) return SAD_TYPE_BOOLEAN;
    if (value->value->isArray()) return SAD_TYPE_ARRAY;
    if (value->value->isFunction()) return SAD_TYPE_FUNCTION;
    return SAD_TYPE_NONE;
}

int64_t sad_value_as_int(SadValue value) {
    return value->value->asInteger();
}

double sad_value_as_float(SadValue value) {
    return value->value->asFloat();
}

const char* sad_value_as_string(SadValue value) {
    static std::string str;
    str = value->value->asString();
    return str.c_str();
}

bool sad_value_as_bool(SadValue value) {
    return value->value->asBool();
}

// ===== Type Checks =====
bool sad_value_is_int(SadValue value) {
    return value->value->isInteger();
}

bool sad_value_is_float(SadValue value) {
    return value->value->isFloat();
}

bool sad_value_is_string(SadValue value) {
    return value->value->isString();
}

bool sad_value_is_bool(SadValue value) {
    return value->value->isBool();
}

bool sad_value_is_array(SadValue value) {
    return value->value->isArray();
}

// ===== Array Operations =====
int sad_array_size(SadValue array) {
    return static_cast<int>(array->value->asArray().size());
}

SadValue sad_array_get(SadValue array, int index) {
    return new SadValue_(new Value(array->value->asArray()[index]));
}

void sad_array_set(SadValue array, int index, SadValue value) {
    array->value->asArray()[index] = *value->value;
}

void sad_array_push(SadValue array, SadValue value) {
    array->value->asArray().push_back(*value->value);
}

// ===== Runtime Functions =====
SadRuntime* sad_runtime_new(void) {
    return new SadRuntime_();
}

void sad_runtime_free(SadRuntime* runtime) {
    delete runtime;
}

void sad_runtime_register_function(SadRuntime* runtime,
                                   const char* moduleName,
                                   const char* funcName,
                                   SadNativeFunction func) {
    // Wrap C function as C++ lambda
    auto wrapper = [func, runtime](const std::vector<Value>& args) -> Value {
        // Convert args to C API
        std::vector<SadValue> cArgs;
        for (const auto& arg : args) {
            cArgs.push_back(new SadValue_(new Value(arg)));
        }
        
        // Call C function
        SadValue result = func(runtime, cArgs.data(), static_cast<int>(cArgs.size()));
        
        // Convert result back
        Value resultValue = *result->value;
        
        // Cleanup
        for (auto* arg : cArgs) delete arg;
        delete result;
        
        return resultValue;
    };
    
    runtime->stdlibManager->registerFunction(moduleName, funcName, wrapper);
    DEBUG_PRINT("Registered native function: %s.%s", moduleName, funcName);
}

SadValue sad_runtime_execute(SadRuntime* runtime, const char* code) {
    try {
        runtime->interpreter->execute(code);
        return new SadValue_(new Value(runtime->interpreter->getLastValue()));
    } catch (const std::exception& e) {
        sad_set_error(runtime, e.what());
        return sad_value_new_none();
    }
}

// ===== Error Handling =====
void sad_set_error(SadRuntime* runtime, const char* message) {
    runtime->lastError = message;
    ERROR_PRINT("Plugin error: %s", message);
}

const char* sad_get_error(SadRuntime* runtime) {
    return runtime->lastError.c_str();
}

bool sad_has_error(SadRuntime* runtime) {
    return !runtime->lastError.empty();
}

void sad_clear_error(SadRuntime* runtime) {
    runtime->lastError.clear();
}
```

---

### Plugin Loader

```cpp
// include/modules/plugin_loader.h
namespace Sad::Modules {

class PluginLoader {
private:
    std::map<std::string, void*> loadedPlugins; // DLL handles
    
public:
    bool loadPlugin(const std::string& path, SadRuntime* runtime) {
        DEBUG_PRINT("Loading plugin: %s", path.c_str());
        
        // Load DLL
        #ifdef _WIN32
            HMODULE handle = LoadLibraryA(path.c_str());
            if (!handle) {
                ERROR_PRINT("Failed to load plugin: %s", path.c_str());
                return false;
            }
            
            // Get plugin info function
            typedef SadPluginInfo* (*GetInfoFunc)();
            GetInfoFunc getInfo = (GetInfoFunc)GetProcAddress(handle, "sad_plugin_get_info");
            
            if (!getInfo) {
                ERROR_PRINT("Plugin missing sad_plugin_get_info: %s", path.c_str());
                FreeLibrary(handle);
                return false;
            }
            
            // Get plugin info
            SadPluginInfo* info = getInfo();
            if (!info) {
                ERROR_PRINT("Plugin returned null info: %s", path.c_str());
                FreeLibrary(handle);
                return false;
            }
            
            // Initialize plugin
            if (info->initialize && !info->initialize(runtime)) {
                ERROR_PRINT("Plugin initialization failed: %s", path.c_str());
                FreeLibrary(handle);
                return false;
            }
            
            loadedPlugins[path] = handle;
            
            DEBUG_PRINT("Plugin loaded: %s v%s by %s", 
                       info->name, info->version, info->author);
            
        #else
            // Linux: dlopen
            void* handle = dlopen(path.c_str(), RTLD_LAZY);
            // ... similar logic
        #endif
        
        return true;
    }
    
    void unloadPlugin(const std::string& path) {
        auto it = loadedPlugins.find(path);
        if (it != loadedPlugins.end()) {
            #ifdef _WIN32
                FreeLibrary((HMODULE)it->second);
            #else
                dlclose(it->second);
            #endif
            loadedPlugins.erase(it);
            DEBUG_PRINT("Plugin unloaded: %s", path.c_str());
        }
    }
    
    ~PluginLoader() {
        // Unload all plugins
        for (auto& [path, handle] : loadedPlugins) {
            unloadPlugin(path);
        }
    }
};

} // namespace Sad::Modules
```

---

### Example Plugin (C++)

```cpp
// examples/plugins/example_plugin.cpp
#include "api/sad_api.h"
#include <cmath>

// Plugin functions
SadValue plugin_square(SadRuntime* runtime, SadValue* args, int argc) {
    if (argc < 1) {
        sad_set_error(runtime, "square requires 1 argument");
        return sad_value_new_none();
    }
    
    if (sad_value_is_int(args[0])) {
        int64_t value = sad_value_as_int(args[0]);
        return sad_value_new_int(value * value);
    } else if (sad_value_is_float(args[0])) {
        double value = sad_value_as_float(args[0]);
        return sad_value_new_float(value * value);
    }
    
    sad_set_error(runtime, "square requires numeric argument");
    return sad_value_new_none();
}

SadValue plugin_factorial(SadRuntime* runtime, SadValue* args, int argc) {
    if (argc < 1) {
        sad_set_error(runtime, "factorial requires 1 argument");
        return sad_value_new_none();
    }
    
    int64_t n = sad_value_as_int(args[0]);
    if (n < 0) {
        sad_set_error(runtime, "factorial requires non-negative integer");
        return sad_value_new_none();
    }
    
    int64_t result = 1;
    for (int64_t i = 2; i <= n; ++i) {
        result *= i;
    }
    
    return sad_value_new_int(result);
}

// Plugin initialization
bool plugin_init(SadRuntime* runtime) {
    sad_runtime_register_function(runtime, "example", "مربع", plugin_square);
    sad_runtime_register_function(runtime, "example", "square", plugin_square);
    sad_runtime_register_function(runtime, "example", "مضروب", plugin_factorial);
    sad_runtime_register_function(runtime, "example", "factorial", plugin_factorial);
    
    return true;
}

// Plugin cleanup
void plugin_finalize(SadRuntime* runtime) {
    // Cleanup if needed
}

// Plugin info
static SadPluginInfo pluginInfo = {
    "Example Plugin",
    "1.0.0",
    "Sad Team",
    "Example plugin demonstrating the Plugin API",
    plugin_init,
    plugin_finalize
};

// Export plugin info
SAD_PLUGIN_EXPORT {
    return &pluginInfo;
}
```

---

### Using Plugin in Sad Code

```sad
// test.s
استخدم example

رقم x = 5
رقم مربع = example.مربع(x)
رقم مضروب = example.مضروب(x)

اطبع("مربع "، x، " = "، مربع)
اطبع("مضروب "، x، " = "، مضروب)
```

---

## 📁 الملفات المطلوبة / Required Files

```
include/api/
├── sad_api.h                    🆕 NEW (C API header)

src/api/
├── sad_api.cpp                  🆕 NEW (C API implementation)

include/modules/
├── plugin_loader.h              🆕 NEW

src/modules/
├── plugin_loader.cpp            🆕 NEW

examples/plugins/
├── example_plugin.cpp           🆕 NEW
├── CMakeLists.txt               🆕 NEW (plugin build)

tests/api_tests/
├── c_api_test.cpp               🆕 NEW (10 tests)
├── plugin_loader_test.cpp       🆕 NEW (8 tests)
```

---

## 🔧 تقسيم العمل / Task Breakdown

### يوم 1-3: C API

#### Task 1.1: C API Header (يوم 1)
**الملفات:**
- `include/api/sad_api.h`

**الخطوات:**
1. Define types (SadValue, SadRuntime, etc.)
2. Define value creation functions
3. Define value getters
4. Define runtime functions
5. Define error handling
6. Define plugin interface

**معايير القبول:**
- ✅ Header compiles in C and C++
- ✅ All types defined
- ✅ API is clear and documented

---

#### Task 1.2: C API Implementation (يوم 2-3)
**الملفات:**
- `src/api/sad_api.cpp`

**الخطوات:**
1. Implement value creation functions
2. Implement value getters
3. Implement type checks
4. Implement array operations
5. Implement runtime functions
6. Implement error handling

**الاختبارات:**
```cpp
TEST(CAPITest, ValueCreation) {
    SadValue v = sad_value_new_int(42);
    EXPECT_TRUE(sad_value_is_int(v));
    EXPECT_EQ(sad_value_as_int(v), 42);
}

TEST(CAPITest, ArrayOperations) {
    SadValue arr = sad_value_new_array(nullptr, 0);
    sad_array_push(arr, sad_value_new_int(1));
    sad_array_push(arr, sad_value_new_int(2));
    
    EXPECT_EQ(sad_array_size(arr), 2);
    EXPECT_EQ(sad_value_as_int(sad_array_get(arr, 0)), 1);
}

TEST(CAPITest, RuntimeExecution) {
    SadRuntime* runtime = sad_runtime_new();
    
    SadValue result = sad_runtime_execute(runtime, "رقم x = 5 + 3");
    
    EXPECT_FALSE(sad_has_error(runtime));
    sad_runtime_free(runtime);
}
```

---

### يوم 4-5: Plugin Loader

#### Task 2.1: Plugin Loader (يوم 4-5)
**الملفات:**
- `include/modules/plugin_loader.h`
- `src/modules/plugin_loader.cpp`

**الخطوات:**
1. Implement DLL loading (Windows/Linux)
2. Implement plugin info retrieval
3. Implement plugin initialization
4. Implement plugin unloading
5. Error handling for plugin errors

**الاختبارات:**
```cpp
TEST(PluginLoaderTest, LoadPlugin) {
    PluginLoader loader;
    SadRuntime* runtime = sad_runtime_new();
    
    bool success = loader.loadPlugin("plugins/example_plugin.dll", runtime);
    EXPECT_TRUE(success);
    
    sad_runtime_free(runtime);
}
```

---

### يوم 6-7: Example Plugin & Integration

#### Task 3.1: Example Plugin (يوم 6)
**الملفات:**
- `examples/plugins/example_plugin.cpp`
- `examples/plugins/CMakeLists.txt`

**CMakeLists for Plugin:**
```cmake
# examples/plugins/CMakeLists.txt
add_library(example_plugin SHARED example_plugin.cpp)
target_include_directories(example_plugin PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(example_plugin sad_api)
```

---

#### Task 3.2: Integration Test (يوم 7)
```cpp
TEST(PluginIntegration, CompleteFlow) {
    // Load plugin
    PluginLoader loader;
    SadRuntime* runtime = sad_runtime_new();
    loader.loadPlugin("plugins/example_plugin.dll", runtime);
    
    // Execute code using plugin
    const char* code = R"(
        استخدم example
        رقم نتيجة = example.مربع(5)
    )";
    
    SadValue result = sad_runtime_execute(runtime, code);
    EXPECT_FALSE(sad_has_error(runtime));
    
    // Cleanup
    sad_runtime_free(runtime);
}
```

---

## 🧪 الاختبارات / Tests Summary

### Unit Tests (18+ tests)
- C API (10 tests)
- Plugin Loader (8 tests)

---

## ⏱️ الجدول الزمني / Timeline

| اليوم | المهام | الساعات |
|-------|--------|---------|
| يوم 1-3 | C API | 24 ساعة |
| يوم 4-5 | Plugin Loader | 16 ساعة |
| يوم 6-7 | Example Plugin + Integration | 16 ساعة |
| **Total** | **1 week** | **56 ساعة** |

---

## 🚀 أوامر PowerShell / PowerShell Commands

```powershell
# إنشاء فرع العمل
git checkout -b feat/api/plugin-system

# بناء مع Plugins
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Debug -DBUILD_PLUGINS=ON
cmake --build . --config Debug

# اختبار
ctest -R api_tests --output-on-failure -C Debug

# Commit
git add include/api/* src/api/* include/modules/* src/modules/* examples/plugins/*
git commit -m "[api] Implemented C API and Plugin System

- C API for value creation/manipulation
- Plugin loader with DLL support
- Native function registration
- Error handling in plugins
- Example plugin demonstrating API
- 18+ unit tests with ≥85% coverage"

git push --set-upstream origin feat/api/plugin-system
```

---

## ⚠️ المخاطر / Risks

| الخطر | التخفيف |
|-------|---------|
| ABI compatibility | Versioned API, opaque handles |
| Memory leaks | Clear ownership rules, docs |
| Security | Sandboxing in Phase 3 |
| Cross-platform DLL loading | Abstraction layer |

---

## 🔮 Roadmap (Phase 3)

- **Python Bindings**: ctypes/CFFI wrapper
- **Sandboxing**: Restrict plugin capabilities
- **Hot Reload**: Dynamic plugin reload
- **Plugin Manager**: Discovery, versioning

---

**آخر تحديث:** 4 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ
