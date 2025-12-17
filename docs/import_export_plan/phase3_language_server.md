# المرحلة 3: خادم اللغة (Language Server)
# Phase 3: Language Server Protocol (LSP)

**الحالة / Status:** 📅 مُخطط / Planned  
**الأولوية / Priority:** P0 (حرجة / Critical)  
**الوقت المقدر / Estimated Time:** 4-6 أسابيع  
**المتطلبات السابقة / Prerequisites:** ✅ المرحلة 1، ✅ المرحلة 2

---

## 📋 نظرة عامة / Overview

### بالعربية
خادم لغة كامل (LSP) لدعم لغة ص في جميع IDEs الحديثة. يوفر:
- **إكمال تلقائي ذكي** (IntelliSense)
- **الانتقال للتعريف** و **البحث عن المراجع**
- **تشخيص أخطاء فوري** (Diagnostics)
- **إعادة التسمية الذكية** (Rename Refactoring)
- **تنسيق تلقائي** (Auto-formatting)
- **Hover Information** و **Signature Help**
- **Semantic Highlighting**

### English
A complete Language Server Protocol implementation for Sad language supporting all modern IDEs, providing:
- **Smart Auto-completion** (IntelliSense)
- **Go to Definition** & **Find References**
- **Real-time Diagnostics**
- **Intelligent Rename Refactoring**
- **Auto-formatting**
- **Hover Information** & **Signature Help**
- **Semantic Highlighting**

---

## 🎯 الميزات المستهدفة / Target Features

### Core LSP Features (P0)
1. ⬜ **Text Synchronization** - مزامنة النصوص
2. ⬜ **Diagnostics** - تشخيص الأخطاء
3. ⬜ **Completion** - الإكمال التلقائي
4. ⬜ **Hover** - معلومات عند التمرير
5. ⬜ **Signature Help** - مساعدة التوقيع
6. ⬜ **Go to Definition** - الانتقال للتعريف
7. ⬜ **Find References** - البحث عن المراجع
8. ⬜ **Document Symbols** - رموز المستند

### Advanced Features (P1)
9. ⬜ **Rename** - إعادة التسمية
10. ⬜ **Formatting** - التنسيق
11. ⬜ **Code Actions** - إجراءات الكود
12. ⬜ **Semantic Tokens** - Highlighting متقدم

### Extended Features (P2)
13. ⬜ **Workspace Symbols** - رموز Workspace
14. ⬜ **Code Lens** - عدسات الكود
15. ⬜ **Inlay Hints** - تلميحات مضمنة
16. ⬜ **Call Hierarchy** - تسلسل الاستدعاءات

---

## 🏗️ البنية المعمارية / Architecture

```
┌──────────────────────────────────────────────────┐
│              IDE (VS Code, etc.)                 │
└────────────────┬─────────────────────────────────┘
                 │ LSP Protocol (JSON-RPC)
                 ▼
┌──────────────────────────────────────────────────┐
│           Sad Language Server                    │
│  ┌────────────────────────────────────────────┐ │
│  │         Protocol Handler                   │ │
│  │  - Initialize                              │ │
│  │  - Text Document Sync                      │ │
│  │  - Request Dispatcher                      │ │
│  └───────────────┬────────────────────────────┘ │
│                  │                               │
│  ┌───────────────▼────────────────────────────┐ │
│  │      Document Manager                      │ │
│  │  - Document Store                          │ │
│  │  - Version Tracking                        │ │
│  │  - Change Detection                        │ │
│  └───────────────┬────────────────────────────┘ │
│                  │                               │
│  ┌───────────────▼────────────────────────────┐ │
│  │      Analysis Engine                       │ │
│  │  ├─ Lexer                                  │ │
│  │  ├─ Parser                                 │ │
│  │  ├─ Semantic Analyzer                      │ │
│  │  ├─ Type Checker                           │ │
│  │  └─ Symbol Table Builder                   │ │
│  └───────────────┬────────────────────────────┘ │
│                  │                               │
│  ┌───────────────▼────────────────────────────┐ │
│  │       Feature Providers                    │ │
│  │  ├─ Completion Provider                    │ │
│  │  ├─ Hover Provider                         │ │
│  │  ├─ Definition Provider                    │ │
│  │  ├─ References Provider                    │ │
│  │  ├─ Rename Provider                        │ │
│  │  └─ Formatting Provider                    │ │
│  └────────────────────────────────────────────┘ │
└──────────────────────────────────────────────────┘
```

---

## 💻 التنفيذ الأساسي / Core Implementation

### 1. LSP Server Entry Point

**ملف:** `src/lsp/sad_language_server.h`

```cpp
// include/lsp/sad_language_server.h
#pragma once

#include <string>
#include <memory>
#include "lsp/protocol.h"
#include "lsp/document_manager.h"
#include "lsp/analysis_engine.h"

namespace Sad {
namespace LSP {

/**
 * خادم لغة ص / Sad Language Server
 */
class SadLanguageServer {
public:
    SadLanguageServer();
    ~SadLanguageServer();
    
    /**
     * بدء الخادم / Start server
     * @param inputStream مدخل (stdin)
     * @param outputStream مخرج (stdout)
     */
    void run(std::istream& input, std::ostream& output);
    
    /**
     * معالجة رسالة LSP
     * Process LSP message
     */
    void processMessage(const std::string& message);
    
private:
    std::unique_ptr<DocumentManager> documentManager_;
    std::unique_ptr<AnalysisEngine> analysisEngine_;
    
    // Protocol Handlers
    void handleInitialize(const json& params);
    void handleTextDocumentDidOpen(const json& params);
    void handleTextDocumentDidChange(const json& params);
    void handleTextDocumentDidClose(const json& params);
    
    // Feature Handlers
    void handleCompletion(const json& params);
    void handleHover(const json& params);
    void handleDefinition(const json& params);
    void handleReferences(const json& params);
    void handleRename(const json& params);
    void handleFormatting(const json& params);
    
    // Response Helpers
    void sendResponse(const json& response);
    void sendNotification(const std::string& method, const json& params);
    void sendDiagnostics(const std::string& uri, 
                        const std::vector<Diagnostic>& diagnostics);
};

} // namespace LSP
} // namespace Sad
```

---

### 2. Document Manager

**ملف:** `src/lsp/document_manager.h`

```cpp
// include/lsp/document_manager.h
#pragma once

#include <string>
#include <map>
#include <memory>

namespace Sad {
namespace LSP {

/**
 * مستند / Document
 */
struct Document {
    std::string uri;
    std::string text;
    int version;
    std::string languageId;
    
    // بيانات التحليل المخزنة
    std::unique_ptr<AST::Program> ast;
    std::unique_ptr<SymbolTable> symbols;
    std::vector<Diagnostic> diagnostics;
    
    // الطوابع الزمنية
    std::chrono::system_clock::time_point lastModified;
    std::chrono::system_clock::time_point lastAnalyzed;
    
    bool needsAnalysis() const;
};

/**
 * مدير المستندات / Document Manager
 */
class DocumentManager {
public:
    /**
     * فتح مستند / Open document
     */
    void openDocument(const std::string& uri,
                     const std::string& text,
                     const std::string& languageId);
    
    /**
     * تحديث مستند / Update document
     */
    void updateDocument(const std::string& uri,
                       int version,
                       const std::vector<TextEdit>& changes);
    
    /**
     * إغلاق مستند / Close document
     */
    void closeDocument(const std::string& uri);
    
    /**
     * الحصول على مستند / Get document
     */
    Document* getDocument(const std::string& uri);
    
    /**
     * الحصول على جميع المستندات المفتوحة
     */
    std::vector<std::string> getOpenDocuments() const;
    
    /**
     * تطبيق تغييرات نصية
     */
    void applyTextEdits(Document* doc, 
                       const std::vector<TextEdit>& edits);
    
private:
    std::map<std::string, std::unique_ptr<Document>> documents_;
    
    void invalidateAnalysis(Document* doc);
};

} // namespace LSP
} // namespace Sad
```

---

### 3. Analysis Engine

**ملف:** `src/lsp/analysis_engine.h`

```cpp
// include/lsp/analysis_engine.h
#pragma once

#include "parser/parser.h"
#include "semantic/semantic_analyzer.h"
#include "lsp/document_manager.h"

namespace Sad {
namespace LSP {

/**
 * محرك التحليل / Analysis Engine
 */
class AnalysisEngine {
public:
    /**
     * تحليل مستند / Analyze document
     */
    void analyzeDocument(Document* doc);
    
    /**
     * إعادة التحليل إذا لزم الأمر
     */
    void reanalyzeIfNeeded(Document* doc);
    
    /**
     * الحصول على رموز المستند
     */
    std::vector<SymbolInformation> getDocumentSymbols(
        const Document* doc);
    
    /**
     * الحصول على اقتراحات الإكمال
     */
    std::vector<CompletionItem> getCompletions(
        const Document* doc,
        const Position& position);
    
    /**
     * الحصول على معلومات Hover
     */
    std::optional<Hover> getHover(
        const Document* doc,
        const Position& position);
    
    /**
     * الانتقال للتعريف
     */
    std::optional<Location> getDefinition(
        const Document* doc,
        const Position& position);
    
    /**
     * البحث عن المراجع
     */
    std::vector<Location> findReferences(
        const Document* doc,
        const Position& position,
        bool includeDeclaration);
    
private:
    std::unique_ptr<Parser> parser_;
    std::unique_ptr<SemanticAnalyzer> semanticAnalyzer_;
    
    // Helper methods
    Symbol* findSymbolAtPosition(
        const Document* doc,
        const Position& position);
    
    std::vector<Diagnostic> convertErrors(
        const std::vector<Error>& errors);
};

} // namespace LSP
} // namespace Sad
```

---

## 🎨 مزودو الميزات / Feature Providers

### 1. Completion Provider (الإكمال التلقائي)

```cpp
// src/lsp/providers/completion_provider.cpp

std::vector<CompletionItem> CompletionProvider::provide(
    const Document* doc,
    const Position& position) 
{
    std::vector<CompletionItem> items;
    
    // 1. الكلمات المفتاحية / Keywords
    if (isInKeywordContext(doc, position)) {
        addKeywordCompletions(items);
    }
    
    // 2. المتغيرات والدوال في السياق / Variables & Functions
    auto symbols = getVisibleSymbols(doc, position);
    for (const auto& sym : symbols) {
        CompletionItem item;
        item.label = sym.name;
        item.kind = symbolKindToCompletionKind(sym.kind);
        item.detail = sym.type;
        item.documentation = sym.documentation;
        items.push_back(item);
    }
    
    // 3. أعضاء الصنف / Class Members
    if (auto dotContext = getDotContext(doc, position)) {
        auto members = getClassMembers(dotContext->className);
        for (const auto& member : members) {
            items.push_back(createMemberCompletion(member));
        }
    }
    
    // 4. الوحدات المستوردة / Imported Modules
    auto modules = getImportedModules(doc);
    for (const auto& mod : modules) {
        items.push_back(createModuleCompletion(mod));
    }
    
    return items;
}
```

**مثال الاستخدام / Usage Example:**
```s
متغير عداد = 10
دالة زيادة(رقم ع)
    إرجاع ع + 1
نهاية

# عند كتابة: ع
# يظهر: عداد، ع (parameter)

# عند كتابة: زيا
# يظهر: زيادة(رقم ع) → رقم
```

---

### 2. Hover Provider (معلومات التمرير)

```cpp
// src/lsp/providers/hover_provider.cpp

std::optional<Hover> HoverProvider::provide(
    const Document* doc,
    const Position& position)
{
    auto symbol = findSymbolAtPosition(doc, position);
    if (!symbol) {
        return std::nullopt;
    }
    
    Hover hover;
    
    // بناء المحتوى / Build content
    std::stringstream content;
    
    // التوقيع / Signature
    content << "```sad\n";
    content << symbol->signature;
    content << "\n```\n\n";
    
    // التوثيق / Documentation
    if (!symbol->documentation.empty()) {
        content << symbol->documentation << "\n\n";
    }
    
    // معلومات إضافية / Additional info
    content << "**النوع:** " << symbol->type << "\n";
    content << "**المكان:** " << symbol->location << "\n";
    
    hover.contents = content.str();
    hover.range = symbol->range;
    
    return hover;
}
```

**مثال / Example:**
```s
دالة رقم حساب_المساحة(رقم العرض، رقم الطول)
    # التوثيق: يحسب مساحة المستطيل
    إرجاع العرض * الطول
نهاية

# عند التمرير فوق "حساب_المساحة":
┌─────────────────────────────────────┐
│ ```sad                              │
│ دالة رقم حساب_المساحة(             │
│     رقم العرض،                     │
│     رقم الطول                       │
│ )                                   │
│ ```                                 │
│                                     │
│ يحسب مساحة المستطيل                 │
│                                     │
│ **النوع:** دالة                    │
│ **المكان:** file.s:1:1             │
└─────────────────────────────────────┘
```

---

### 3. Definition Provider (الانتقال للتعريف)

```cpp
// src/lsp/providers/definition_provider.cpp

std::optional<Location> DefinitionProvider::provide(
    const Document* doc,
    const Position& position)
{
    // 1. إيجاد الرمز عند المؤشر
    auto symbol = findSymbolAtPosition(doc, position);
    if (!symbol) {
        return std::nullopt;
    }
    
    // 2. إذا كان استخدام، ابحث عن التعريف
    if (symbol->isUsage) {
        symbol = findDefinition(symbol->name, doc);
    }
    
    if (!symbol || !symbol->location) {
        return std::nullopt;
    }
    
    // 3. إرجاع الموقع
    Location location;
    location.uri = symbol->location->uri;
    location.range = symbol->location->range;
    
    return location;
}
```

**مثال / Example:**
```s
# ملف: math.s
صدر دالة رقم جذر(رقم ع)  # ← التعريف (Definition)
    إرجاع ع ** 0.5
نهاية

# ملف: main.s
استورد رياضيات

رقم نتيجة = رياضيات.جذر(16)  # ← الاستخدام (Usage)
                               # F12 أو Ctrl+Click
                               # ينقل لـ math.s:1
```

---

### 4. References Provider (البحث عن المراجع)

```cpp
// src/lsp/providers/references_provider.cpp

std::vector<Location> ReferencesProvider::provide(
    const Document* doc,
    const Position& position,
    bool includeDeclaration)
{
    std::vector<Location> locations;
    
    // 1. إيجاد الرمز
    auto symbol = findSymbolAtPosition(doc, position);
    if (!symbol) {
        return locations;
    }
    
    // 2. إضافة التعريف إذا طُلب
    if (includeDeclaration && symbol->definition) {
        locations.push_back(*symbol->definition);
    }
    
    // 3. البحث في جميع المستندات المفتوحة
    for (const auto& [uri, openDoc] : getAllDocuments()) {
        auto refs = findReferencesInDocument(
            openDoc.get(), 
            symbol->name
        );
        locations.insert(locations.end(), refs.begin(), refs.end());
    }
    
    return locations;
}
```

---

### 5. Rename Provider (إعادة التسمية)

```cpp
// src/lsp/providers/rename_provider.cpp

std::optional<WorkspaceEdit> RenameProvider::provide(
    const Document* doc,
    const Position& position,
    const std::string& newName)
{
    // 1. التحقق من صحة الاسم الجديد
    if (!isValidIdentifier(newName)) {
        return std::nullopt;
    }
    
    // 2. إيجاد جميع المراجع
    auto references = findAllReferences(doc, position);
    if (references.empty()) {
        return std::nullopt;
    }
    
    // 3. بناء التعديلات
    WorkspaceEdit edit;
    for (const auto& ref : references) {
        TextEdit textEdit;
        textEdit.range = ref.range;
        textEdit.newText = newName;
        edit.changes[ref.uri].push_back(textEdit);
    }
    
    return edit;
}
```

**مثال / Example:**
```s
متغير عداد = 0  # ← F2 وأدخل "counter"

دالة زيادة_العداد()
    عداد = عداد + 1  # ← سيتغير تلقائياً
نهاية

اطبع(عداد)  # ← سيتغير تلقائياً

# النتيجة:
متغير counter = 0

دالة زيادة_العداد()
    counter = counter + 1
نهاية

اطبع(counter)
```

---

### 6. Formatting Provider (التنسيق التلقائي)

```cpp
// src/lsp/providers/formatting_provider.cpp

std::vector<TextEdit> FormattingProvider::provide(
    const Document* doc,
    const FormattingOptions& options)
{
    // 1. تحليل المستند
    auto ast = parseDocument(doc);
    if (!ast) {
        return {};
    }
    
    // 2. تطبيق قواعد التنسيق
    Formatter formatter(options);
    std::string formatted = formatter.format(ast.get());
    
    // 3. إنشاء TextEdit لاستبدال كل المستند
    TextEdit edit;
    edit.range = getFullDocumentRange(doc);
    edit.newText = formatted;
    
    return {edit};
}
```

**قواعد التنسيق / Formatting Rules:**
```
1. المسافات البادئة: 4 spaces أو tab
2. سطر جديد قبل/بعد الدوال
3. مسافة بعد الفواصل
4. مسافة حول العوامل
5. محاذاة الأصناف
```

---

## 📡 بروتوكول LSP / LSP Protocol

### JSON-RPC Messages

#### 1. Initialize Request
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "method": "initialize",
  "params": {
    "processId": 12345,
    "rootUri": "file:///path/to/project",
    "capabilities": {
      "textDocument": {
        "completion": {"completionItem": {"snippetSupport": true}},
        "hover": {"contentFormat": ["markdown"]},
        "definition": {},
        "references": {},
        "rename": {}
      }
    }
  }
}
```

#### 2. Response
```json
{
  "jsonrpc": "2.0",
  "id": 1,
  "result": {
    "capabilities": {
      "textDocumentSync": 2,
      "completionProvider": {"triggerCharacters": [".", ":"]},
      "hoverProvider": true,
      "definitionProvider": true,
      "referencesProvider": true,
      "renameProvider": true,
      "documentFormattingProvider": true
    },
    "serverInfo": {
      "name": "sad-language-server",
      "version": "1.0.0"
    }
  }
}
```

---

## 🔌 تكامل VS Code / VS Code Integration

### Extension Structure

```
sad-vscode/
├── package.json              # Manifest
├── src/
│   ├── extension.ts         # Entry point
│   └── client.ts            # LSP Client
├── syntaxes/
│   └── sad.tmLanguage.json  # Syntax Highlighting
└── language-configuration.json
```

---

### package.json

```json
{
  "name": "sad-language",
  "displayName": "Sad Language Support",
  "description": "Language support for Sad programming language",
  "version": "1.0.0",
  "publisher": "sad-lang",
  "engines": {
    "vscode": "^1.75.0"
  },
  "categories": ["Programming Languages"],
  "activationEvents": ["onLanguage:sad"],
  "main": "./out/extension.js",
  "contributes": {
    "languages": [{
      "id": "sad",
      "aliases": ["Sad", "sad"],
      "extensions": [".s", ".sad"],
      "configuration": "./language-configuration.json"
    }],
    "grammars": [{
      "language": "sad",
      "scopeName": "source.sad",
      "path": "./syntaxes/sad.tmLanguage.json"
    }],
    "configuration": {
      "title": "Sad Language",
      "properties": {
        "sad.trace.server": {
          "type": "string",
          "enum": ["off", "messages", "verbose"],
          "default": "off"
        },
        "sad.maxNumberOfProblems": {
          "type": "number",
          "default": 100
        }
      }
    }
  }
}
```

---

### Extension Client (extension.ts)

```typescript
import * as vscode from 'vscode';
import {
    LanguageClient,
    LanguageClientOptions,
    ServerOptions,
    TransportKind
} from 'vscode-languageclient/node';

let client: LanguageClient;

export function activate(context: vscode.ExtensionContext) {
    // Server executable
    const serverExecutable = {
        command: 'sad-language-server',
        args: []
    };
    
    const serverOptions: ServerOptions = {
        run: serverExecutable,
        debug: serverExecutable
    };
    
    // Client options
    const clientOptions: LanguageClientOptions = {
        documentSelector: [{ scheme: 'file', language: 'sad' }],
        synchronize: {
            fileEvents: vscode.workspace.createFileSystemWatcher('**/.s')
        }
    };
    
    // Create client
    client = new LanguageClient(
        'sadLanguageServer',
        'Sad Language Server',
        serverOptions,
        clientOptions
    );
    
    // Start client
    client.start();
}

export function deactivate(): Thenable<void> | undefined {
    if (!client) {
        return undefined;
    }
    return client.stop();
}
```

---

## 🧪 الاختبارات / Testing

### Test Cases

```cpp
/**
 * اختبار 1: الإكمال التلقائي
 */
TEST(LSP, Completion) {
    const char* code = R"(
        متغير عداد = 10
        متغير ع|
    )";
    
    auto completions = getCompletions(code, Position{1, 8});
    
    ASSERT_GT(completions.size(), 0);
    EXPECT_TRUE(contains(completions, "عداد"));
}

/**
 * اختبار 2: الانتقال للتعريف
 */
TEST(LSP, GoToDefinition) {
    const char* code = R"(
        دالة جذر(رقم ع)
            إرجاع ع ** 0.5
        نهاية
        
        رقم نتيجة = جذر(16)
    )";
    
    auto location = getDefinition(code, Position{4, 17});
    
    ASSERT_TRUE(location.has_value());
    EXPECT_EQ(location->range.start.line, 0);
}

/**
 * اختبار 3: البحث عن المراجع
 */
TEST(LSP, FindReferences) {
    const char* code = R"(
        متغير عداد = 0
        عداد = عداد + 1
        اطبع(عداد)
    )";
    
    auto refs = findReferences(code, Position{0, 7});
    
    EXPECT_EQ(refs.size(), 4);  // التعريف + 3 استخدامات
}
```

---

## 📚 ملفات التنفيذ / Implementation Files

```
src/lsp/
├── sad_language_server.{h,cpp}      # الخادم الرئيسي
├── protocol.{h,cpp}                 # معالجة البروتوكول
├── document_manager.{h,cpp}         # إدارة المستندات
├── analysis_engine.{h,cpp}          # محرك التحليل
│
├── providers/
│   ├── completion_provider.{h,cpp}
│   ├── hover_provider.{h,cpp}
│   ├── definition_provider.{h,cpp}
│   ├── references_provider.{h,cpp}
│   ├── rename_provider.{h,cpp}
│   └── formatting_provider.{h,cpp}
│
└── utils/
    ├── position.{h,cpp}
    ├── range.{h,cpp}
    └── text_edit.{h,cpp}

tools/vscode-extension/
├── package.json
├── src/
│   ├── extension.ts
│   └── client.ts
└── syntaxes/
    └── sad.tmLanguage.json
```

---

## ✅ قائمة المهام / Task Checklist

### الأسبوع 1-2: الأساسيات
- [ ] بنية LSP Server الأساسية
- [ ] معالجة بروتوكول JSON-RPC
- [ ] Document Manager
- [ ] Text Synchronization
- [ ] Diagnostics

### الأسبوع 3-4: الميزات الأساسية
- [ ] Completion Provider
- [ ] Hover Provider
- [ ] Definition Provider
- [ ] References Provider

### الأسبوع 5-6: الميزات المتقدمة
- [ ] Rename Provider
- [ ] Formatting Provider
- [ ] Semantic Highlighting
- [ ] Code Actions
- [ ] VS Code Extension

---

## 🚀 الخطوة التالية / Next Step

بعد إكمال هذه المرحلة، انتقل إلى:
**[المرحلة 4: نظام الحزم](phase4_package_system.md)**

---

**آخر تحديث / Last Updated:** 8 ديسمبر 2025
