# خادم بروتوكول اللغة (Language Server Protocol - LSP)

## ما هو LSP؟

خادم بروتوكول اللغة (LSP - Language Server Protocol) هو بروتوكول قياسي طورته Microsoft للتواصل بين محررات الكود وخوادم اللغات. يسمح هذا البروتوكول لمحرر الكود (مثل VS Code أو Vim أو Sublime) بالحصول على ميزات ذكية مثل الإكمال التلقائي، والتنقل إلى التعريفات، وعرض الأخطاء، وإعادة التسمية، دون أن يحتاج المحرر لفهم اللغة مباشرة.

الفكرة الأساسية هي فصل منطق اللغة عن المحرر. بدلاً من كتابة دعم للغة ص لكل محرر على حدة (عمل مكرر وضخم)، نكتب خادم LSP واحد يفهم لغة ص، وأي محرر يدعم بروتوكول LSP يمكنه الاستفادة من هذا الخادم. هذا يوفر الجهد ويضمن تجربة متسقة عبر جميع المحررات.

## بنية خادم LSP

يتواجد خادم LSP في المجلد `tools/lsp/` ويتكون من:

```
tools/lsp/
├── main.cpp                      # نقطة الدخول
├── lsp_server.h                  # الخادم الرئيسي
├── lsp_types.h                   # أنواع البروتوكول
├── firebase_module_loader.h      # محمل الوحدات
└── FIREBASE_MODULE_LOADER_GUIDE.md  # دليل المحمل
```

## كيف يعمل LSP؟

### التواصل عبر JSON-RPC

الخادم والمحرر يتواصلان عبر JSON-RPC على stdin/stdout:

```cpp
// من main.cpp
class JSONRPCHandler {
public:
    explicit JSONRPCHandler(LanguageServer& server) 
        : server_(server), running_(true) {
        log("LSP Server started - بدء خادم اللغة");
    }
    
    void run() {
        while (running_ && std::cin.good()) {
            try {
                // قراءة الرسالة
                auto message = read_message();
                if (!message) continue;
                
                // معالجة الرسالة
                auto response = handle_message(*message);
                
                // إرسال الرد
                if (response) {
                    send_message(*response);
                }
            } catch (const std::exception& e) {
                send_error(-1, -32603, "Internal error");
            }
        }
    }
};
```

### صيغة الرسائل

رسائل LSP تتبع صيغة محددة:

```
Content-Length: <length>\r\n
\r\n
<json content>
```

```cpp
std::optional<json> read_message() {
    std::string header;
    
    // قراءة الرؤوس
    while (std::getline(std::cin, header) && header != "\r") {
        if (header.find("Content-Length:") == 0) {
            size_t length = std::stoul(header.substr(15));
            
            // تخطي السطر الفارغ
            std::getline(std::cin, header);
            
            // قراءة المحتوى
            std::vector<char> buffer(length + 1);
            std::cin.read(buffer.data(), length);
            
            // تحليل JSON
            return json::parse(buffer.data());
        }
    }
    return std::nullopt;
}
```

## الميزات المدعومة

### الإكمال التلقائي (Auto-completion)

عند الكتابة، يقترح الخادم إكمالات مناسبة:

```cpp
json handleCompletion(const json& params) {
    std::string uri = params["textDocument"]["uri"];
    int line = params["position"]["line"];
    int character = params["position"]["character"];
    
    // الحصول على السياق
    std::string text = getDocumentText(uri);
    std::string prefix = getWordAtPosition(text, line, character);
    
    // جمع الاقتراحات
    json items = json::array();
    
    // الكلمات المفتاحية
    for (const auto& keyword : getKeywords()) {
        if (startsWith(keyword, prefix)) {
            items.push_back({
                {"label", keyword},
                {"kind", 14},  // Keyword
                {"detail", "كلمة مفتاحية / Keyword"}
            });
        }
    }
    
    // المتغيرات في النطاق
    for (const auto& var : getVariablesInScope(uri, line)) {
        if (startsWith(var.name, prefix)) {
            items.push_back({
                {"label", var.name},
                {"kind", 6},   // Variable
                {"detail", var.type}
            });
        }
    }
    
    // الدوال المتاحة
    for (const auto& func : getFunctions(uri)) {
        if (startsWith(func.name, prefix)) {
            items.push_back({
                {"label", func.name},
                {"kind", 3},   // Function
                {"detail", func.signature},
                {"documentation", func.doc}
            });
        }
    }
    
    return {{"items", items}};
}
```

### الانتقال إلى التعريف (Go to Definition)

```cpp
json handleGotoDefinition(const json& params) {
    std::string uri = params["textDocument"]["uri"];
    int line = params["position"]["line"];
    int character = params["position"]["character"];
    
    // الحصول على الرمز تحت المؤشر
    std::string symbol = getSymbolAtPosition(uri, line, character);
    
    // البحث عن التعريف
    auto definition = findDefinition(symbol, uri);
    
    if (definition) {
        return {
            {"uri", definition->uri},
            {"range", {
                {"start", {{"line", definition->line}, {"character", 0}}},
                {"end", {{"line", definition->line}, {"character", definition->length}}}
            }}
        };
    }
    
    return nullptr;
}
```

### عرض الأخطاء (Diagnostics)

الخادم يحلل الملف ويرسل الأخطاء للمحرر:

```cpp
void publishDiagnostics(const std::string& uri) {
    std::string text = getDocumentText(uri);
    
    // التحليل المعجمي
    Lexer::LexerCore lexer(text);
    
    // التحليل النحوي
    Parser::ParserCore parser(lexer);
    
    json diagnostics = json::array();
    
    try {
        auto ast = parser.parseProgram();
        
        // التحليل الدلالي
        auto errors = semanticAnalysis(ast);
        
        for (const auto& error : errors) {
            diagnostics.push_back({
                {"range", {
                    {"start", {{"line", error.line}, {"character", error.column}}},
                    {"end", {{"line", error.line}, {"character", error.column + error.length}}}
                }},
                {"severity", 1},  // Error
                {"message", error.message + " / " + error.messageAr},
                {"source", "sad-lsp"}
            });
        }
    }
    catch (const ParseError& e) {
        diagnostics.push_back({
            {"range", {
                {"start", {{"line", e.line}, {"character", e.column}}},
                {"end", {{"line", e.line}, {"character", e.column + 1}}}
            }},
            {"severity", 1},
            {"message", e.what()},
            {"source", "sad-lsp"}
        });
    }
    
    // إرسال التشخيصات
    send_notification("textDocument/publishDiagnostics", {
        {"uri", uri},
        {"diagnostics", diagnostics}
    });
}
```

### التمرير فوق الرمز (Hover)

عرض معلومات عند التمرير فوق رمز:

```cpp
json handleHover(const json& params) {
    std::string uri = params["textDocument"]["uri"];
    int line = params["position"]["line"];
    int character = params["position"]["character"];
    
    std::string symbol = getSymbolAtPosition(uri, line, character);
    auto info = getSymbolInfo(symbol, uri);
    
    if (info) {
        std::string content = "**" + info->name + "**\n\n";
        content += "النوع: " + info->type + "\n\n";
        content += info->documentation;
        
        return {
            {"contents", {
                {"kind", "markdown"},
                {"value", content}
            }}
        };
    }
    
    return nullptr;
}
```

### البحث عن المراجع (Find References)

```cpp
json handleFindReferences(const json& params) {
    std::string symbol = getSymbolAtPosition(/* ... */);
    
    json locations = json::array();
    
    // البحث في جميع الملفات
    for (const auto& file : getWorkspaceFiles()) {
        for (const auto& ref : findReferences(symbol, file)) {
            locations.push_back({
                {"uri", file},
                {"range", ref.range}
            });
        }
    }
    
    return locations;
}
```

### إعادة التسمية (Rename)

```cpp
json handleRename(const json& params) {
    std::string uri = params["textDocument"]["uri"];
    std::string newName = params["newName"];
    std::string oldName = getSymbolAtPosition(/* ... */);
    
    json changes = json::object();
    
    // جمع جميع المواقع التي تحتاج تغيير
    for (const auto& file : getWorkspaceFiles()) {
        json edits = json::array();
        
        for (const auto& ref : findReferences(oldName, file)) {
            edits.push_back({
                {"range", ref.range},
                {"newText", newName}
            });
        }
        
        if (!edits.empty()) {
            changes[file] = edits;
        }
    }
    
    return {{"changes", changes}};
}
```

### التنسيق (Formatting)

```cpp
json handleFormatting(const json& params) {
    std::string uri = params["textDocument"]["uri"];
    std::string text = getDocumentText(uri);
    
    // تنسيق الكود
    std::string formatted = formatCode(text);
    
    return json::array({
        {
            {"range", {
                {"start", {{"line", 0}, {"character", 0}}},
                {"end", {{"line", INT_MAX}, {"character", INT_MAX}}}
            }},
            {"newText", formatted}
        }
    });
}
```

## دورة حياة الاتصال

### التهيئة

```cpp
json handleInitialize(const json& params) {
    // حفظ إعدادات العميل
    clientCapabilities_ = params["capabilities"];
    
    // إرجاع قدرات الخادم
    return {
        {"capabilities", {
            {"textDocumentSync", 1},  // Full sync
            {"completionProvider", {
                {"triggerCharacters", {".", ":"}}
            }},
            {"hoverProvider", true},
            {"definitionProvider", true},
            {"referencesProvider", true},
            {"documentFormattingProvider", true},
            {"renameProvider", true},
            {"diagnosticProvider", {
                {"interFileDependencies", true},
                {"workspaceDiagnostics", true}
            }}
        }},
        {"serverInfo", {
            {"name", "sad-lsp"},
            {"version", "1.0.0"}
        }}
    };
}
```

### معالجة تغييرات الملفات

```cpp
void handleDidOpen(const json& params) {
    std::string uri = params["textDocument"]["uri"];
    std::string text = params["textDocument"]["text"];
    
    // تخزين المستند
    documents_[uri] = text;
    
    // نشر التشخيصات
    publishDiagnostics(uri);
}

void handleDidChange(const json& params) {
    std::string uri = params["textDocument"]["uri"];
    
    // تحديث المستند
    for (const auto& change : params["contentChanges"]) {
        documents_[uri] = change["text"];
    }
    
    // إعادة التحليل
    publishDiagnostics(uri);
}

void handleDidClose(const json& params) {
    std::string uri = params["textDocument"]["uri"];
    documents_.erase(uri);
}
```

## التسجيل والتصحيح

```cpp
void log(const std::string& message) {
    if (log_file_.is_open()) {
        auto now = std::time(nullptr);
        log_file_ << "[" << std::ctime(&now) << "] " << message << std::endl;
    }
}
```

ملف السجل يساعد في تصحيح المشاكل:

```
[Thu Jan 09 10:00:00 2026] LSP Server started
[Thu Jan 09 10:00:01 2026] Received: initialize
[Thu Jan 09 10:00:01 2026] Sending response: capabilities
[Thu Jan 09 10:00:02 2026] Received: textDocument/didOpen
[Thu Jan 09 10:00:02 2026] Publishing diagnostics for file.s
```

## إضافة VS Code

مع خادم LSP، يمكن إنشاء إضافة VS Code:

```
tools/vscode-extension/
├── package.json          # تعريف الإضافة
├── src/
│   └── extension.ts      # كود الإضافة
└── syntaxes/
    └── sad.tmLanguage.json  # تلوين الصيغة
```

## الخلاصة

خادم LSP للغة ص يوفر تجربة تطوير متقدمة في أي محرر يدعم البروتوكول. من الإكمال التلقائي إلى التنقل إلى التعريفات إلى عرض الأخطاء الفورية، كل هذه الميزات متاحة بفضل الخادم. البروتوكول القياسي يضمن أن أي محرر جديد يدعم LSP سيعمل تلقائياً مع لغة ص، مما يوفر الجهد ويوسع قاعدة المستخدمين المحتملين.
