# المرحلة 7: الأدوات والبنية التحتية
# Phase 7: Tooling & Infrastructure

**المدة / Duration:** 4-6 أسابيع / weeks  
**الأولوية / Priority:** P2 (متوسطة / Medium)  
**الحالة / Status:** 🔴 لم تبدأ / Not Started

---

## 📋 نظرة عامة / Overview

### (AR) العربية

تطوير أدوات التطوير الأساسية:
- Language Server Protocol (LSP)
- Debugger integration (GDB/LLDB)
- Package manager (sad-pkg)
- Documentation generator (sad-doc)
- Build system integration
- IDE extensions

---

## 📝 المهام / Tasks

### 1. Language Server Protocol (أسبوعان)

#### (AR) العربية

**LSP Server:**

```cpp
// tools/lsp/server.h
class LanguageServer {
private:
    # Document management
    قاموس<نص، مؤشر<Document>> documents
    
    # Analysis
    مؤشر<Compiler> compiler
    مؤشر<SymbolTable> symbols
    
public:
    # LSP methods
    دالة initialize(InitializeParams params) -> InitializeResult
    دالة text_document_did_open(DidOpenParams params) -> فارغ
    دالة text_document_did_change(DidChangeParams params) -> فارغ
    
    # Code intelligence
    دالة text_document_hover(HoverParams params) -> Hover
    دالة text_document_completion(CompletionParams params) -> مصفوفة<CompletionItem>
    دالة text_document_definition(DefinitionParams params) -> Location
    دالة text_document_references(ReferencesParams params) -> مصفوفة<Location>
    
    # Diagnostics
    دالة publish_diagnostics(نص uri, مصفوفة<Diagnostic> diagnostics) -> فارغ
    
    # Code actions
    دالة text_document_code_action(CodeActionParams params) -> مصفوفة<CodeAction>
    دالة text_document_formatting(FormattingParams params) -> مصفوفة<TextEdit>
نهاية
```

**LSP Features:**

**1. Hover Information:**
```s
# Hovering over variable shows:
رقم x = 42
# Type: رقم
# Value: 42
# Defined: line 5
```

**2. Auto-completion:**
```s
# Typing "اط" shows:
اطبع(...)      # Print to stdout
اطبع_خطأ(...)  # Print to stderr
```

**3. Go to Definition:**
```s
دالة جمع(رقم أ, رقم ب) -> رقم  # Definition
    إرجاع أ + ب
نهاية

دالة main()
    اطبع(جمع(1, 2))  # Ctrl+Click → jumps to definition
نهاية
```

**4. Find References:**
```s
# Find all usages of 'جمع'
- line 10: دالة جمع(...)
- line 15: جمع(1, 2)
- line 20: x = جمع(a, b)
```

**5. Diagnostics:**
```
Error: undefined variable 'y'
  → test.s:5:10

Warning: unused variable 'x'
  → test.s:3:5
```

**6. Code Actions:**
```s
# Quick fixes:
رقم x  # ← shows: "Initialize variable"

# Refactorings:
دالة complex_function()
    # code...
نهاية
# ← shows: "Extract to separate file"
```

**المهام:**
- [ ] LSP server skeleton
- [ ] Document management
- [ ] Hover information
- [ ] Auto-completion
- [ ] Go to definition
- [ ] Find references
- [ ] Diagnostics
- [ ] Code actions
- [ ] Formatting
- [ ] اختبارات LSP (50+ tests)

---

### 2. Debugger Integration (أسبوع واحد)

#### (AR) العربية

**GDB/LLDB Support:**

```bash
# Debug with GDB
sadc program.s -o program -g
gdb program

# GDB commands work:
(gdb) break main
(gdb) run
(gdb) print x
(gdb) step
(gdb) continue
```

**Debug Info:**

```cpp
// compiler/backends/llvm/debug.h
class DebugInfoEmitter {
public:
    # Source mapping
    دالة emit_line_info(رقم line, رقم column) -> فارغ
    
    # Variable info
    دالة emit_local_variable(نص name, مؤشر<Type> type, رقم scope_level) -> فارغ
    
    # Function info
    دالة emit_function_info(نص name, رقم start_line) -> فارغ
نهاية
```

**Debugger Commands:**

```
# الأوامر العربية للـ debugger
كسر main          # break main
تشغيل             # run
اطبع x            # print x
خطوة              # step
استمرار          # continue
```

**المهام:**
- [ ] DWARF debug info generation
- [ ] Source line mapping
- [ ] Variable inspection
- [ ] Breakpoint support
- [ ] Stack trace with Arabic names
- [ ] اختبارات Debugger (20+ tests)

---

### 3. Package Manager (أسبوعان)

#### (AR) العربية

**sad-pkg:**

```bash
# Initialize project
sad-pkg init

# Install package
sad-pkg install math-lib

# Update packages
sad-pkg update

# Publish package
sad-pkg publish
```

**Package Structure:**

```
my-package/
├── sad.toml          # Package manifest
├── src/
│   └── main.s
├── tests/
│   └── test_main.s
└── docs/
    └── README.md
```

**sad.toml:**

```toml
[package]
name = "my-package"
version = "1.0.0"
authors = ["Author Name"]
license = "MIT"
description = "Package description"

[dependencies]
math-lib = "2.0"
string-utils = "1.5"

[dev-dependencies]
test-framework = "1.0"
```

**Package Manager API:**

```cpp
// tools/pkg/manager.h
class PackageManager {
public:
    # Package operations
    دالة install(نص package_name) -> منطقي
    دالة uninstall(نص package_name) -> منطقي
    دالة update(نص package_name) -> منطقي
    دالة publish(نص package_path) -> منطقي
    
    # Dependency resolution
    دالة resolve_dependencies(مصفوفة<Dependency> deps) -> مصفوفة<Package>
    
    # Registry
    دالة search(نص query) -> مصفوفة<PackageInfo>
    دالة fetch_package(نص name, نص version) -> مؤشر<Package>
نهاية
```

**المهام:**
- [ ] Package manifest format
- [ ] Dependency resolver
- [ ] Registry client
- [ ] Install/uninstall commands
- [ ] Version management
- [ ] اختبارات Package Manager (30+ tests)

---

### 4. Documentation Generator (أسبوع واحد)

#### (AR) العربية

**sad-doc:**

```bash
# Generate documentation
sad-doc src/ -o docs/

# Options
sad-doc src/ --format html
sad-doc src/ --format markdown
sad-doc src/ --format pdf
```

**Documentation Comments:**

```s
#*
 * يجمع رقمين
 * 
 * @param أ الرقم الأول
 * @param ب الرقم الثاني
 * @return مجموع الرقمين
 * 
 * @example
 * رقم نتيجة = جمع(5, 3)  # 8
 *#
دالة جمع(رقم أ, رقم ب) -> رقم
    إرجاع أ + ب
نهاية
```

**Generated HTML:**

```html
<div class="function">
  <h3>جمع</h3>
  <p>يجمع رقمين</p>
  
  <h4>Parameters</h4>
  <ul>
    <li><code>أ: رقم</code> - الرقم الأول</li>
    <li><code>ب: رقم</code> - الرقم الثاني</li>
  </ul>
  
  <h4>Returns</h4>
  <p><code>رقم</code> - مجموع الرقمين</p>
  
  <h4>Example</h4>
  <pre><code>رقم نتيجة = جمع(5, 3)  # 8</code></pre>
</div>
```

**المهام:**
- [ ] Documentation parser
- [ ] HTML generator
- [ ] Markdown generator
- [ ] Cross-references
- [ ] Search functionality
- [ ] اختبارات Doc Generator (20+ tests)

---

### 5. IDE Extensions (أسبوع واحد)

#### (AR) العربية

**VS Code Extension:**

```json
{
  "name": "sad-language",
  "displayName": "Sad Language Support",
  "description": "Language support for Sad",
  "version": "1.0.0",
  "engines": {
    "vscode": "^1.60.0"
  },
  "categories": ["Programming Languages"],
  "activationEvents": [
    "onLanguage:sad"
  ],
  "contributes": {
    "languages": [{
      "id": "sad",
      "aliases": ["Sad", "sad"],
      "extensions": [".s"],
      "configuration": "./language-configuration.json"
    }],
    "grammars": [{
      "language": "sad",
      "scopeName": "source.sad",
      "path": "./syntaxes/sad.tmLanguage.json"
    }]
  }
}
```

**Syntax Highlighting:**

```json
{
  "patterns": [
    {
      "name": "keyword.control.sad",
      "match": "\\b(دالة|إذا|بينما|لكل|إرجاع|نهاية)\\b"
    },
    {
      "name": "storage.type.sad",
      "match": "\\b(رقم|نص|منطقي|مصفوفة|قاموس)\\b"
    },
    {
      "name": "string.quoted.double.sad",
      "begin": "\"",
      "end": "\""
    }
  ]
}
```

**المهام:**
- [ ] VS Code extension
- [ ] Syntax highlighting
- [ ] LSP client integration
- [ ] Debugger integration
- [ ] Snippets
- [ ] Theme support
- [ ] اختبارات Extension (15+ tests)

---

### 6. Build System Integration (أسبوع واحد)

#### (AR) العربية

**CMake Integration:**

```cmake
# FindSad.cmake
find_program(SADC_EXECUTABLE
    NAMES sadc
    PATHS /usr/local/bin /usr/bin
)

function(add_sad_executable target)
    add_custom_command(
        OUTPUT ${target}
        COMMAND ${SADC_EXECUTABLE} ${ARGN} -o ${target}
        DEPENDS ${ARGN}
    )
    add_custom_target(${target}_build ALL DEPENDS ${target})
endfunction()

# Usage:
add_sad_executable(myprogram src/main.s)
```

**Makefile Integration:**

```makefile
# Makefile for Sad projects
SADC = sadc
SOURCES = $(wildcard src/*.s)
OBJECTS = $(SOURCES:.s=.o)
TARGET = program

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(SADC) $(OBJECTS) -o $(TARGET)

%.o: %.s
	$(SADC) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)
```

**المهام:**
- [ ] CMake module
- [ ] Makefile templates
- [ ] Build scripts
- [ ] CI/CD integration
- [ ] اختبارات Build System (10+ tests)

---

## 📊 معايير النجاح / Success Criteria

✅ **المرحلة مكتملة عندما:**
- [ ] LSP يعمل في VS Code
- [ ] Debugger يدعم GDB/LLDB
- [ ] Package manager يدير dependencies
- [ ] Documentation generator ينتج docs جيدة
- [ ] IDE extensions متاحة
- [ ] Build system integration يعمل

---

**السابق / Previous:** [المرحلة 6: التحسينات](09_phase6_optimizations.md)  
**التالي / Next:** [الجدول الزمني](11_timeline.md)
