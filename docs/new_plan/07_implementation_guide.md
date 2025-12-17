# 🛠️ دليل التنفيذ / Implementation Guide

**الهدف / Goal:** دليل عملي خطوة بخطوة لتنفيذ المراحل المقترحة

**تاريخ / Date:** 29 نوفمبر 2025

---

## 📋 نظرة عامة / Overview

هذا الدليل يوفر إرشادات تفصيلية لكيفية البدء في تنفيذ كل مرحلة، بما في ذلك:
- الإعداد والبنية التحتية
- معايير الكود
- سير العمل
- الأدوات المطلوبة

This guide provides detailed instructions on how to start implementing each phase, including:
- Setup and infrastructure
- Code standards
- Workflow
- Required tools

---

## 🚀 البدء السريع / Quick Start

### المتطلبات الأساسية / Prerequisites

```bash
# أدوات التطوير الأساسية
- CMake >= 3.20
- C++17 compiler (MSVC 2022, GCC 11+, Clang 13+)
- Git
- Python 3.8+ (للسكريبتات)
- Node.js 18+ (للأدوات)

# مكتبات خارجية (سيتم تنزيلها تلقائياً)
- libcurl (للشبكات)
- sqlite3 (لقاعدة البيانات)
- nlohmann/json (لـ JSON)
- Skia (للرسومات)
```

---

### إعداد البيئة / Environment Setup

#### Windows
```powershell
# تثبيت الأدوات عبر Chocolatey
choco install cmake git python nodejs visualstudio2022-workload-nativecross

# استنساخ المستودع
git clone https://github.com/mohasalahh/s_language.git
cd s_language

# بناء المشروع
mkdir build
cd build
cmake ..
cmake --build . --config Release

# تشغيل الاختبارات
ctest -C Release
```

#### Linux
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install -y cmake g++ git python3 nodejs libcurl4-openssl-dev libsqlite3-dev

# Fedora/RHEL
sudo dnf install -y cmake gcc-c++ git python3 nodejs libcurl-devel sqlite-devel

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Test
ctest
```

---

## 📂 هيكل المشروع / Project Structure

### البنية الحالية
```
s_language/
├── include/            # Header files
│   ├── ast/           # AST nodes
│   ├── lexer/         # Lexer
│   ├── parser/        # Parser
│   ├── interpreter/   # Interpreter
│   └── stdlib/        # Standard library
├── src/               # Implementation files
├── tests/             # Unit tests
├── examples/          # Example programs
└── docs/              # Documentation
```

### البنية المستقبلية (بعد Phase 6)
```
Sad Ecosystem/
├── s_language/        # Core language (compiler, interpreter)
├── s_lang_server/     # LSP server
├── s_tools/           # CLI tools (sad command)
├── s_graphics/        # Graphics engine
├── s_ai/              # AI service
├── s_stdlib/          # Extended standard library
└── vscode-sad/        # VS Code extension
```

---

## 🎨 معايير الكود / Code Standards

### C++ Style Guide

```cpp
// 1. Naming conventions
class MyClass {};              // PascalCase for classes
void myFunction() {}           // camelCase for functions
int my_variable = 0;           // snake_case for variables
const int MY_CONSTANT = 42;    // UPPER_CASE for constants

// 2. Namespaces
namespace Sad {
namespace Stdlib {
namespace Net {
    // Implementation
}
}
}

// أو استخدام nested namespace (C++17)
namespace Sad::Stdlib::Net {
    // Implementation
}

// 3. Header guards
#pragma once  // مفضل
// أو
#ifndef SAD_MODULE_CLASS_H
#define SAD_MODULE_CLASS_H
// ...
#endif

// 4. Smart pointers
std::unique_ptr<Object> obj;    // Ownership واضح
std::shared_ptr<Object> shared; // مشاركة
// تجنب raw pointers إلا للمراجع غير المالكة

// 5. Error handling
class SadException : public std::exception {
    std::string message_;
public:
    explicit SadException(const std::string& msg) : message_(msg) {}
    const char* what() const noexcept override { return message_.c_str(); }
};

// 6. Documentation (Doxygen style)
/**
 * @brief Brief description
 * 
 * Detailed description here.
 * 
 * @param param1 Description of param1
 * @param param2 Description of param2
 * @return Return value description
 * @throws SadException if error occurs
 */
int myFunction(int param1, double param2);
```

---

### لغة ص (Sad) Style Guide

```sad
# 1. التسمية
صنف شخص {}              # PascalCase للأصناف
دالة حساب_المجموع() {}   # snake_case للدوال
متغير اسم = "أحمد"      # snake_case للمتغيرات
ثابت PI = 3.14159       # UPPER_CASE للثوابت

# 2. المسافات البادئة: 4 مسافات
دالة مثال() {
    إذا (شرط) {
        اطبع("نعم")
    }
}

# 3. التعليقات
# تعليق سطر واحد

/**
 * تعليق متعدد الأسطر
 * للتوثيق
 */
دالة دالة_مهمة() {
    // Implementation
}

# 4. معالجة الأخطاء
دالة قسمة(أ، ب) {
    إذا (ب == 0) {
        ارمي خطأ("القسمة على صفر")
    }
    أرجع أ / ب
}

جرّب {
    متغير نتيجة = قسمة(10، 0)
} اصطد (خطأ) {
    اطبع("خطأ: " + خطأ)
}
```

---

## 🔄 سير العمل / Workflow

### Git Workflow

```bash
# 1. إنشاء فرع جديد
git checkout -b feature/phase1-lsp-server

# 2. العمل على الميزة
# ... كتابة الكود ...

# 3. Commit مع رسائل واضحة
git add .
git commit -m "feat(lsp): implement basic completion

- Add completion provider
- Support for variables and functions
- Tests included

Refs: #123"

# 4. Push و Pull Request
git push origin feature/phase1-lsp-server
# ثم إنشاء PR على GitHub

# 5. بعد المراجعة والموافقة
git checkout main
git merge feature/phase1-lsp-server
```

### Commit Message Format
```
<type>(<scope>): <subject>

<body>

<footer>

# Types:
- feat: ميزة جديدة
- fix: إصلاح خطأ
- docs: تحديث التوثيق
- style: تنسيق الكود
- refactor: إعادة هيكلة
- test: إضافة اختبارات
- chore: مهام صيانة

# Examples:
feat(stdlib): add HTTP client support
fix(parser): handle Arabic identifiers correctly
docs(readme): update installation instructions
```

---

### CI/CD Pipeline

```yaml
# .github/workflows/ci.yml
name: CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ${{ matrix.os }}
    strategy:
      matrix:
        os: [ubuntu-latest, windows-latest, macos-latest]
        build_type: [Debug, Release]
    
    steps:
    - uses: actions/checkout@v3
    
    - name: Configure CMake
      run: cmake -B build -DCMAKE_BUILD_TYPE=${{ matrix.build_type }}
    
    - name: Build
      run: cmake --build build --config ${{ matrix.build_type }}
    
    - name: Test
      run: cd build && ctest -C ${{ matrix.build_type }} --output-on-failure
    
    - name: Upload artifacts
      uses: actions/upload-artifact@v3
      with:
        name: sad-${{ matrix.os }}-${{ matrix.build_type }}
        path: build/sad*
```

---

## 📝 دليل التنفيذ لكل مرحلة / Phase Implementation Guides

### Phase 0: Core Improvements

#### الخطوة 1: Performance Analysis
```bash
# إعداد أدوات Profiling
# Windows: Visual Studio Profiler
# Linux: perf, valgrind

# مثال على Windows
# 1. فتح sad.exe في Visual Studio
# 2. Debug > Performance Profiler
# 3. اختيار CPU Usage
# 4. تشغيل مع برنامج اختبار

# مثال على Linux
perf record -g ./sad examples/benchmark.s
perf report
```

#### الخطوة 2: Object Layout Optimization
```cpp
// قبل
class ObjectInstance {
    std::map<std::string, Value> fields;  //느리다
};

// بعد
class ObjectInstance {
    // استخدام vector للوصول الأسرع
    std::vector<Value> fields;
    
    // Cache للأسماء
    static std::unordered_map<std::string, size_t> field_indices;
    
    Value& getField(const std::string& name) {
        auto it = field_indices.find(name);
        if (it != field_indices.end()) {
            return fields[it->second];  // O(1)
        }
        // Fallback
    }
};
```

---

### Phase 1: LSP Server

#### الخطوة 1: Project Setup
```bash
# إنشاء مشروع جديد
mkdir s_lang_server
cd s_lang_server

# بنية المشروع
mkdir -p src/{lsp,analyzer,utils}
mkdir -p include/{lsp,analyzer,utils}
mkdir tests

# CMakeLists.txt
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.20)
project(s_lang_server)

set(CMAKE_CXX_STANDARD 17)

# Dependencies
find_package(nlohmann_json REQUIRED)

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Executable
add_executable(s_lang_server ${SOURCES})
target_include_directories(s_lang_server PRIVATE include)
target_link_libraries(s_lang_server PRIVATE nlohmann_json::nlohmann_json)
EOF
```

#### الخطوة 2: LSP Protocol Implementation
```cpp
// include/lsp/protocol.h
#pragma once
#include <nlohmann/json.hpp>

namespace Sad::LSP {

struct Position {
    int line;
    int character;
};

struct Range {
    Position start;
    Position end;
};

struct TextDocumentIdentifier {
    std::string uri;
};

struct CompletionParams {
    TextDocumentIdentifier textDocument;
    Position position;
};

// JSON serialization
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Position, line, character)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Range, start, end)

} // namespace
```

```cpp
// src/lsp/server.cpp
#include "lsp/server.h"
#include <iostream>
#include <sstream>

namespace Sad::LSP {

void Server::run() {
    while (true) {
        // قراءة رسالة LSP
        auto message = readMessage();
        if (!message) break;
        
        // معالجة
        auto response = handleMessage(*message);
        
        // إرسال الرد
        sendMessage(response);
    }
}

nlohmann::json Server::handleMessage(const nlohmann::json& message) {
    std::string method = message["method"];
    
    if (method == "initialize") {
        return handleInitialize(message["params"]);
    } else if (method == "textDocument/completion") {
        return handleCompletion(message["params"]);
    }
    // ... المزيد
}

nlohmann::json Server::handleCompletion(const nlohmann::json& params) {
    CompletionParams cp = params.get<CompletionParams>();
    
    // تحليل المستند
    auto doc = documents_[cp.textDocument.uri];
    auto items = analyzer_.getCompletions(doc, cp.position);
    
    return {
        {"id", currentId_},
        {"result", items}
    };
}

} // namespace
```

#### الخطوة 3: Testing
```cpp
// tests/lsp_tests.cpp
#include <gtest/gtest.h>
#include "lsp/server.h"

TEST(LSPServer, BasicCompletion) {
    Sad::LSP::Server server;
    
    // إرسال طلب completion
    nlohmann::json request = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "textDocument/completion"},
        {"params", {
            {"textDocument", {{"uri", "file:///test.s"}}},
            {"position", {{"line", 0}, {"character", 5}}}
        }}
    };
    
    auto response = server.handleMessage(request);
    
    ASSERT_TRUE(response.contains("result"));
    ASSERT_GT(response["result"].size(), 0);
}
```

---

### Phase 2: Package Manager

#### الخطوة 1: sad.json Format
```json
{
  "name": "my-package",
  "version": "1.0.0",
  "description": "وصف الحزمة",
  "author": "أحمد محمد",
  "license": "MIT",
  "keywords": ["utility", "helpers"],
  
  "main": "src/main.s",
  
  "dependencies": {
    "http-client": "^2.1.0",
    "json-parser": "~1.5.2"
  },
  
  "devDependencies": {
    "test-framework": "^3.0.0"
  },
  
  "scripts": {
    "test": "sad test",
    "build": "sad build"
  },
  
  "repository": {
    "type": "git",
    "url": "https://github.com/user/repo"
  }
}
```

#### الخطوة 2: CLI Implementation
```cpp
// src/tools/package_manager.cpp
namespace Sad::Tools {

class PackageManager {
public:
    void init(const std::string& name) {
        // إنشاء sad.json
        nlohmann::json pkg = {
            {"name", name},
            {"version", "1.0.0"},
            {"description", ""},
            {"main", "src/main.s"},
            {"dependencies", nlohmann::json::object()}
        };
        
        std::ofstream file("sad.json");
        file << pkg.dump(2);
        
        std::cout << "تم إنشاء المشروع: " << name << std::endl;
    }
    
    void install(const std::string& package = "") {
        if (package.empty()) {
            // تثبيت جميع التبعيات من sad.json
            installAll();
        } else {
            // تثبيت حزمة معينة
            installPackage(package);
        }
    }
    
private:
    void installPackage(const std::string& name) {
        // 1. تحليل اسم الحزمة والإصدار
        auto [pkgName, version] = parsePackageSpec(name);
        
        // 2. حل التبعيات
        auto resolved = resolver_.resolve(pkgName, version);
        
        // 3. تنزيل
        downloader_.download(resolved);
        
        // 4. تحديث sad.json
        updateManifest(pkgName, version);
    }
    
    DependencyResolver resolver_;
    PackageDownloader downloader_;
};

} // namespace
```

---

### Phase 3: Stdlib Expansion

#### مثال: HTTP Client Implementation
```cpp
// include/stdlib/net/http_client.h
#pragma once
#include <curl/curl.h>
#include <string>
#include <map>

namespace Sad::Stdlib::Net {

struct HttpResponse {
    int statusCode;
    std::string body;
    std::map<std::string, std::string> headers;
    std::string error;
    
    bool ok() const { return statusCode >= 200 && statusCode < 300; }
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();
    
    HttpResponse get(const std::string& url,
                     const std::map<std::string, std::string>& headers = {},
                     int timeout = 30);
    
    HttpResponse post(const std::string& url,
                      const std::string& body,
                      const std::map<std::string, std::string>& headers = {},
                      int timeout = 30);
    
private:
    CURL* curl_;
    
    HttpResponse perform(const std::string& method,
                         const std::string& url,
                         const std::string& body,
                         const std::map<std::string, std::string>& headers,
                         int timeout);
    
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    static size_t headerCallback(char* buffer, size_t size, size_t nitems, std::map<std::string, std::string>* userp);
};

} // namespace
```

```cpp
// src/stdlib/net/http_client.cpp
#include "stdlib/net/http_client.h"
#include <algorithm>

namespace Sad::Stdlib::Net {

HttpClient::HttpClient() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl_ = curl_easy_init();
}

HttpClient::~HttpClient() {
    if (curl_) {
        curl_easy_cleanup(curl_);
    }
    curl_global_cleanup();
}

HttpResponse HttpClient::get(const std::string& url,
                              const std::map<std::string, std::string>& headers,
                              int timeout) {
    return perform("GET", url, "", headers, timeout);
}

HttpResponse HttpClient::perform(const std::string& method,
                                  const std::string& url,
                                  const std::string& body,
                                  const std::map<std::string, std::string>& headers,
                                  int timeout) {
    HttpResponse response;
    
    if (!curl_) {
        response.error = "cURL not initialized";
        return response;
    }
    
    // إعداد URL
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    
    // إعداد الطريقة
    if (method == "POST") {
        curl_easy_setopt(curl_, CURLOPT_POST, 1L);
        curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, body.c_str());
    }
    
    // إعداد الرؤوس
    struct curl_slist* headerList = nullptr;
    for (const auto& [key, value] : headers) {
        std::string header = key + ": " + value;
        headerList = curl_slist_append(headerList, header.c_str());
    }
    if (headerList) {
        curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headerList);
    }
    
    // Timeout
    curl_easy_setopt(curl_, CURLOPT_TIMEOUT, timeout);
    
    // Callbacks
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &response.body);
    curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(curl_, CURLOPT_HEADERDATA, &response.headers);
    
    // تنفيذ الطلب
    CURLcode res = curl_easy_perform(curl_);
    
    if (res != CURLE_OK) {
        response.error = curl_easy_strerror(res);
        response.statusCode = 0;
    } else {
        long code;
        curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &code);
        response.statusCode = static_cast<int>(code);
    }
    
    // تنظيف
    if (headerList) {
        curl_slist_free_all(headerList);
    }
    
    return response;
}

size_t HttpClient::writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

size_t HttpClient::headerCallback(char* buffer, size_t size, size_t nitems, 
                                  std::map<std::string, std::string>* userp) {
    size_t totalSize = size * nitems;
    std::string header(buffer, totalSize);
    
    // تحليل الرأس
    auto colonPos = header.find(':');
    if (colonPos != std::string::npos) {
        std::string key = header.substr(0, colonPos);
        std::string value = header.substr(colonPos + 1);
        
        // إزالة المسافات
        key.erase(key.find_last_not_of(" \t\r\n") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t\r\n") + 1);
        
        (*userp)[key] = value;
    }
    
    return totalSize;
}

} // namespace
```

#### التكامل مع لغة ص
```cpp
// bindings/stdlib_bindings.cpp
void registerHttpClient(Interpreter* interp) {
    // دالة http_get
    interp->registerBuiltin("http_get", [](const std::vector<Value>& args) -> Value {
        if (args.size() < 1) {
            throw SadException("http_get يتطلب على الأقل معامل واحد");
        }
        
        std::string url = args[0].asString();
        std::map<std::string, std::string> headers;
        int timeout = 30;
        
        // معاملات اختيارية
        if (args.size() >= 2) {
            // تحليل headers من map
        }
        
        Sad::Stdlib::Net::HttpClient client;
        auto response = client.get(url, headers, timeout);
        
        // تحويل إلى Value (map)
        return responseToValue(response);
    });
}
```

---

## ✅ Checklist للبدء / Getting Started Checklist

### إعداد البيئة
- [ ] تثبيت الأدوات (CMake, Compiler, Git)
- [ ] استنساخ المستودع
- [ ] بناء المشروع بنجاح
- [ ] تشغيل الاختبارات

### الفهم
- [ ] قراءة التوثيق الأساسي
- [ ] فهم بنية المشروع
- [ ] مراجعة الكود الموجود
- [ ] تشغيل أمثلة

### المساهمة
- [ ] إنشاء Issue للميزة المراد إضافتها
- [ ] مناقشة التصميم
- [ ] إنشاء فرع جديد
- [ ] كتابة الكود + الاختبارات
- [ ] إرسال Pull Request

---

**آخر تحديث:** 29 نوفمبر 2025  
**الحالة:** ✅ جاهز للاستخدام
