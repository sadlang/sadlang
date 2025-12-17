# 🛠️ أدوات المطور / Developer Tools Plan

**المرحلة / Phase:** 1  
**المدة / Duration:** 1-2 أشهر / months  
**الأولوية / Priority:** P0 (حرجة / Critical)

---

## 📋 نظرة عامة / Overview

### (AR) الوصف
أدوات احترافية لتحسين تجربة المطور: خادم اللغة (LSP)، نظام إدارة الحزم، مصحح أخطاء تفاعلي، وواجهة تضمين. هذه الأدوات ضرورية لجعل لغة ص منافسة للغات الحديثة.

### (EN) Description
Professional tools to enhance developer experience: Language Server (LSP), package manager, interactive debugger, and embedding interface. These tools are essential to make Sad competitive with modern languages.

---

## 🎯 المشاريع الرئيسية / Main Projects

### 1. خادم اللغة / Language Server Protocol (LSP)

#### 1.1 نظرة عامة (Overview)

**الوصف:**
- خادم LSP مستقل لدعم جميع المحررات
- تكامل مع VS Code, Vim, Emacs, وغيرها
- استجابة سريعة (<200ms)

**الميزات الأساسية:**
```
✅ Completion (إكمال تلقائي)
✅ Diagnostics (تشخيص الأخطاء)
✅ Hover (معلومات عند التحويم)
✅ Go to Definition (الانتقال للتعريف)
✅ Find References (البحث عن المراجع)
✅ Rename (إعادة التسمية)
✅ Formatting (تنسيق الكود)
✅ Code Actions (إجراءات الكود)
```

---

#### 1.2 المعمارية / Architecture

```
┌─────────────────────────────────────────┐
│         Client (VS Code, etc)            │
└───────────────┬─────────────────────────┘
                │ JSON-RPC
┌───────────────▼─────────────────────────┐
│        LSP Server (s_lang_server)        │
├──────────────────────────────────────────┤
│  Protocol Handler                        │
│  ├─ textDocument/completion              │
│  ├─ textDocument/hover                   │
│  ├─ textDocument/definition              │
│  └─ textDocument/diagnostics             │
├──────────────────────────────────────────┤
│  Language Service                        │
│  ├─ Parser Cache                         │
│  ├─ Symbol Table                         │
│  ├─ Type Checker                         │
│  └─ Semantic Analyzer                    │
├──────────────────────────────────────────┤
│  Sad Core                                │
│  ├─ Lexer                                │
│  ├─ Parser                               │
│  └─ AST                                  │
└──────────────────────────────────────────┘
```

---

#### 1.3 هيكل المشروع / Project Structure

```
s_lang_server/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── lsp/
│   │   ├── protocol.h              # LSP protocol definitions
│   │   ├── server.h                # Main server class
│   │   ├── handlers.h              # Request handlers
│   │   └── json_rpc.h              # JSON-RPC implementation
│   └── services/
│       ├── completion_service.h    # Completion provider
│       ├── diagnostic_service.h    # Diagnostics
│       ├── hover_service.h         # Hover information
│       ├── definition_service.h    # Go to definition
│       ├── references_service.h    # Find references
│       └── formatting_service.h    # Code formatting
├── src/
│   ├── main.cpp                    # Entry point
│   ├── lsp/
│   │   ├── server.cpp
│   │   ├── handlers.cpp
│   │   └── json_rpc.cpp
│   └── services/
│       ├── completion_service.cpp
│       ├── diagnostic_service.cpp
│       ├── hover_service.cpp
│       ├── definition_service.cpp
│       ├── references_service.cpp
│       └── formatting_service.cpp
├── tests/
│   ├── protocol_tests.cpp
│   ├── completion_tests.cpp
│   └── integration_tests.cpp
└── clients/
    ├── vscode/                     # VS Code extension
    │   ├── package.json
    │   ├── src/extension.ts
    │   └── syntaxes/sad.tmLanguage.json
    └── vim/                        # Vim plugin
        └── sad.vim
```

---

#### 1.4 التنفيذ / Implementation

**المرحلة 1.1: الأساسيات (أسبوع 1-2)**

```cpp
// include/lsp/server.h
namespace SadLSP {

class LanguageServer {
public:
    explicit LanguageServer(int port = 0);
    
    void start();
    void stop();
    
    // Register handlers
    void on_initialize(std::function<void(const InitializeParams&)>);
    void on_did_open(std::function<void(const DidOpenParams&)>);
    void on_completion(std::function<CompletionList(const CompletionParams&)>);
    void on_hover(std::function<Hover(const HoverParams&)>);
    
private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace SadLSP
```

**المهام:**
- [ ] JSON-RPC parser/serializer
- [ ] Protocol message handling
- [ ] Document synchronization
- [ ] Basic server lifecycle

**المرحلة 1.2: الميزات الأساسية (أسبوع 3-4)**

```cpp
// include/services/completion_service.h
namespace SadLSP {

class CompletionService {
public:
    explicit CompletionService(Sad::Parser::Parser& parser);
    
    CompletionList complete(const std::string& uri,
                           Position position);
    
private:
    // الإكمال بناءً على السياق
    CompletionList complete_keywords();
    CompletionList complete_variables(Scope* scope);
    CompletionList complete_functions();
    CompletionList complete_methods(ClassType* type);
    CompletionList complete_imports();
};

} // namespace SadLSP
```

**المهام:**
- [ ] Keyword completion
- [ ] Variable/function completion
- [ ] Member access completion (obj.)
- [ ] Import path completion

**المرحلة 1.3: الميزات المتقدمة (أسبوع 5-6)**

```cpp
// include/services/diagnostic_service.h
namespace SadLSP {

class DiagnosticService {
public:
    std::vector<Diagnostic> analyze(const std::string& uri,
                                    const std::string& text);
    
private:
    // أنواع التحليل
    std::vector<Diagnostic> check_syntax();
    std::vector<Diagnostic> check_semantics();
    std::vector<Diagnostic> check_types();
    std::vector<Diagnostic> check_unused();
};

} // namespace SadLSP
```

**المهام:**
- [ ] Syntax error detection
- [ ] Semantic error detection
- [ ] Type checking
- [ ] Unused variable warnings
- [ ] Code style suggestions

---

#### 1.5 VS Code Extension

**package.json:**
```json
{
  "name": "sad-language",
  "displayName": "Sad Language Support",
  "description": "لغة ص - دعم كامل / Full Sad Language Support",
  "version": "1.0.0",
  "engines": {
    "vscode": "^1.60.0"
  },
  "categories": [
    "Programming Languages"
  ],
  "activationEvents": [
    "onLanguage:sad"
  ],
  "main": "./out/extension.js",
  "contributes": {
    "languages": [{
      "id": "sad",
      "aliases": ["Sad", "ص"],
      "extensions": [".s", ".sad"],
      "configuration": "./language-configuration.json"
    }],
    "grammars": [{
      "language": "sad",
      "scopeName": "source.sad",
      "path": "./syntaxes/sad.tmLanguage.json"
    }],
    "configuration": {
      "title": "Sad",
      "properties": {
        "sad.server.path": {
          "type": "string",
          "default": "s_lang_server",
          "description": "Path to language server"
        }
      }
    }
  }
}
```

**المهام:**
- [ ] Syntax highlighting
- [ ] Language configuration (brackets, comments)
- [ ] LSP client integration
- [ ] Snippets
- [ ] Debug adapter (لاحقاً)

---

### 2. نظام إدارة الحزم / Package Manager

#### 2.1 نظرة عامة (Overview)

**الوصف:**
- CLI لإدارة الحزم والتبعيات
- مستودع مركزي للحزم
- دعم الإصدارات المتعددة

**الأوامر الأساسية:**
```bash
# تهيئة مشروع جديد
sad init my_project

# تثبيت حزمة
sad install package_name
sad install package_name@1.2.3

# نشر حزمة
sad publish

# تحديث الحزم
sad update
sad update package_name

# إزالة حزمة
sad remove package_name

# البحث عن حزم
sad search keyword

# معلومات عن حزمة
sad info package_name
```

---

#### 2.2 صيغة الحزمة / Package Format

**sad.json (Package Manifest):**
```json
{
  "name": "my_package",
  "name_ar": "حزمتي",
  "version": "1.0.0",
  "description": "وصف الحزمة",
  "description_en": "Package description",
  "author": "Your Name",
  "license": "MIT",
  "keywords": ["web", "http", "ويب"],
  
  "main": "src/index.s",
  
  "dependencies": {
    "http_client": "^2.0.0",
    "json_parser": "~1.5.0"
  },
  
  "dev_dependencies": {
    "test_framework": "^3.0.0"
  },
  
  "scripts": {
    "test": "sad test",
    "build": "sad build",
    "start": "sad run src/main.s"
  },
  
  "repository": {
    "type": "git",
    "url": "https://github.com/user/my_package"
  }
}
```

---

#### 2.3 هيكل المشروع / Project Structure

```
s_tools/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── cli/
│   │   ├── command.h           # Base command class
│   │   ├── init_command.h
│   │   ├── install_command.h
│   │   ├── publish_command.h
│   │   └── search_command.h
│   ├── package/
│   │   ├── manifest.h          # sad.json parser
│   │   ├── registry.h          # Package registry client
│   │   ├── resolver.h          # Dependency resolver
│   │   └── downloader.h        # Package downloader
│   └── version/
│       ├── version.h           # Semantic versioning
│       └── constraint.h        # Version constraints
├── src/
│   ├── main.cpp
│   ├── cli/
│   │   ├── command.cpp
│   │   ├── init_command.cpp
│   │   ├── install_command.cpp
│   │   ├── publish_command.cpp
│   │   └── search_command.cpp
│   ├── package/
│   │   ├── manifest.cpp
│   │   ├── registry.cpp
│   │   ├── resolver.cpp
│   │   └── downloader.cpp
│   └── version/
│       ├── version.cpp
│       └── constraint.cpp
└── tests/
    ├── cli_tests.cpp
    ├── resolver_tests.cpp
    └── integration_tests.cpp
```

---

#### 2.4 التنفيذ / Implementation

**المرحلة 2.1: CLI الأساسي (أسبوع 1-2)**

```cpp
// include/cli/command.h
namespace SadTools {

class Command {
public:
    virtual ~Command() = default;
    virtual int execute(const std::vector<std::string>& args) = 0;
    virtual std::string help() const = 0;
};

class InitCommand : public Command {
public:
    int execute(const std::vector<std::string>& args) override;
    std::string help() const override;
    
private:
    void create_project_structure(const std::string& name);
    void create_manifest(const std::string& name);
};

} // namespace SadTools
```

**المهام:**
- [ ] Argument parsing
- [ ] Command routing
- [ ] Init command
- [ ] Help system

**المرحلة 2.2: Package Management (أسبوع 3-4)**

```cpp
// include/package/resolver.h
namespace SadTools {

class DependencyResolver {
public:
    struct Resolution {
        std::string package;
        std::string version;
        std::vector<Resolution> dependencies;
    };
    
    Resolution resolve(const Manifest& manifest);
    
private:
    bool check_compatibility(const std::string& constraint,
                            const std::string& version);
    Resolution resolve_recursive(const std::string& package,
                                const std::string& constraint);
};

} // namespace SadTools
```

**المهام:**
- [ ] Dependency resolution algorithm
- [ ] Version constraint checking
- [ ] Conflict detection
- [ ] Lock file generation

**المرحلة 2.3: Registry Integration (أسبوع 5-6)**

```cpp
// include/package/registry.h
namespace SadTools {

class PackageRegistry {
public:
    explicit PackageRegistry(const std::string& url);
    
    // البحث والاستعلام
    std::vector<PackageInfo> search(const std::string& query);
    PackageInfo get_info(const std::string& name);
    std::vector<std::string> get_versions(const std::string& name);
    
    // التنزيل والنشر
    void download(const std::string& name, 
                 const std::string& version,
                 const std::string& dest);
    void publish(const Manifest& manifest,
                const std::string& tarball);
    
private:
    std::string base_url_;
    // HTTP client
};

} // namespace SadTools
```

**المهام:**
- [ ] HTTP client integration
- [ ] Search API
- [ ] Download/upload
- [ ] Authentication

---

### 3. مصحح الأخطاء / Debugger

#### 3.1 نظرة عامة (Overview)

**الميزات:**
- نقاط إيقاف (Breakpoints)
- خطوات التنفيذ (Step, Next, Continue)
- فحص المتغيرات (Variable inspection)
- تتبع المكدس (Stack traces)
- تعبيرات الساعة (Watch expressions)

---

#### 3.2 المعمارية / Architecture

```
┌─────────────────────────────────────────┐
│         Client (VS Code DAP)             │
└───────────────┬─────────────────────────┘
                │ Debug Adapter Protocol
┌───────────────▼─────────────────────────┐
│       Debug Adapter (s_debugger)         │
├──────────────────────────────────────────┤
│  DAP Handler                             │
│  ├─ Launch/Attach                        │
│  ├─ Breakpoints                          │
│  ├─ Stepping                             │
│  └─ Variables                            │
├──────────────────────────────────────────┤
│  Debug Runtime                           │
│  ├─ Breakpoint Manager                   │
│  ├─ Step Controller                      │
│  ├─ Stack Inspector                      │
│  └─ Variable Inspector                   │
├──────────────────────────────────────────┤
│  Sad Interpreter (Modified)              │
│  ├─ Debug Hooks                          │
│  └─ Debug Info                           │
└──────────────────────────────────────────┘
```

---

#### 3.3 التنفيذ / Implementation

**تعديلات الـ Interpreter:**
```cpp
// في include/interpreter/core/interpreter_core.h
class InterpreterCore {
public:
    // إضافة دعم التصحيح
    void set_debug_mode(bool enabled);
    void add_breakpoint(const std::string& file, int line);
    void remove_breakpoint(const std::string& file, int line);
    
    void step_into();
    void step_over();
    void step_out();
    void continue_execution();
    
    StackFrame get_current_frame();
    std::vector<StackFrame> get_stack_trace();
    Value evaluate_expression(const std::string& expr);
    
private:
    bool debug_mode_ = false;
    std::set<std::pair<std::string, int>> breakpoints_;
    DebugState debug_state_;
};
```

**Debug Adapter:**
```cpp
// include/debugger/debug_adapter.h
namespace SadDebug {

class DebugAdapter {
public:
    void launch(const LaunchRequest& req);
    void attach(const AttachRequest& req);
    
    void set_breakpoints(const SetBreakpointsRequest& req);
    void continue_(const ContinueRequest& req);
    void step_in(const StepInRequest& req);
    void step_over(const StepOverRequest& req);
    void step_out(const StepOutRequest& req);
    
    StackTraceResponse stack_trace(const StackTraceRequest& req);
    ScopesResponse scopes(const ScopesRequest& req);
    VariablesResponse variables(const VariablesRequest& req);
    
private:
    Sad::Interpreter::InterpreterCore* interpreter_;
};

} // namespace SadDebug
```

**المهام:**
- [ ] DAP protocol implementation
- [ ] Breakpoint management
- [ ] Step control
- [ ] Variable inspection
- [ ] Watch expressions
- [ ] VS Code integration

**المدة:** 3-4 أسابيع

---

### 4. واجهة التضمين / Embedding API

#### 4.1 نظرة عامة (Overview)

**الوصف:**
- C API بسيط لتضمين مفسر ص
- FFI محسّن
- أمثلة للاستخدام مع C/C++/Rust

---

#### 4.2 C API Design

```c
// include/sad/embed.h
#ifndef SAD_EMBED_H
#define SAD_EMBED_H

#ifdef __cplusplus
extern "C" {
#endif

// Types
typedef struct SadVM SadVM;
typedef struct SadValue SadValue;
typedef struct SadError SadError;

// VM Management
SadVM* sad_vm_new();
void sad_vm_destroy(SadVM* vm);

// Code Execution
SadValue* sad_vm_eval(SadVM* vm, const char* code, SadError** error);
SadValue* sad_vm_eval_file(SadVM* vm, const char* path, SadError** error);

// Value Operations
int sad_value_is_int(const SadValue* val);
int sad_value_is_string(const SadValue* val);
long long sad_value_to_int(const SadValue* val);
const char* sad_value_to_string(const SadValue* val);
void sad_value_destroy(SadValue* val);

// Error Handling
const char* sad_error_message(const SadError* err);
int sad_error_line(const SadError* err);
void sad_error_destroy(SadError* err);

// Function Registration
typedef SadValue* (*SadNativeFunc)(SadVM*, SadValue** args, int argc);
void sad_vm_register_function(SadVM* vm, const char* name, 
                              SadNativeFunc func);

#ifdef __cplusplus
}
#endif

#endif // SAD_EMBED_H
```

---

#### 4.3 مثال الاستخدام / Usage Example

**C Example:**
```c
#include <sad/embed.h>
#include <stdio.h>

// دالة C لاستدعائها من ص
SadValue* my_print(SadVM* vm, SadValue** args, int argc) {
    if (argc > 0 && sad_value_is_string(args[0])) {
        printf("%s\n", sad_value_to_string(args[0]));
    }
    return NULL; // void
}

int main() {
    // إنشاء VM
    SadVM* vm = sad_vm_new();
    
    // تسجيل دالة C
    sad_vm_register_function(vm, "my_print", my_print);
    
    // تنفيذ كود ص
    SadError* err = NULL;
    SadValue* result = sad_vm_eval(vm, 
        "my_print(\"Hello from Sad!\")", &err);
    
    if (err) {
        fprintf(stderr, "Error: %s\n", sad_error_message(err));
        sad_error_destroy(err);
    } else {
        sad_value_destroy(result);
    }
    
    // تنظيف
    sad_vm_destroy(vm);
    return 0;
}
```

**المهام:**
- [ ] C API implementation
- [ ] C++ wrapper
- [ ] Rust bindings (optional)
- [ ] Examples
- [ ] Documentation

**المدة:** 2-3 أسابيع

---

## 📊 خطة التنفيذ / Implementation Plan

### التقسيم الزمني (Timeline)

**الشهر 1:**
- أسبوع 1-2: LSP أساسي (completion, diagnostics)
- أسبوع 3-4: Package manager CLI + manifest

**الشهر 2:**
- أسبوع 1-2: LSP متقدم (refactoring, formatting)
- أسبوع 2-3: Package manager (dependency resolution)
- أسبوع 3-4: Debugger أساسي

**الشهر 3 (اختياري):**
- أسبوع 1-2: Embedding API
- أسبوع 3-4: تحسينات ووثائق

---

## ✅ معايير القبول / Acceptance Criteria

### LSP
- ✅ Completion latency < 200ms
- ✅ Diagnostics accuracy > 95%
- ✅ VS Code extension published

### Package Manager
- ✅ Install/publish working
- ✅ Dependency resolution correct
- ✅ Registry API functional

### Debugger
- ✅ Breakpoints work
- ✅ Variable inspection accurate
- ✅ VS Code debugging integration

### Embedding API
- ✅ C API complete
- ✅ Examples compile and run
- ✅ Documentation comprehensive

---

**آخر تحديث:** 29 نوفمبر 2025  
**الحالة:** ✅ خطة جاهزة للتنفيذ
