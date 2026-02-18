# بسم الله الرحمن الرحيم

<div dir="rtl" align="right">

# 📖 التوثيق التقني لأدوات التطوير

## دليل المطور الداخلي | الإصدار 1.0.0

---

## 🏗️ هيكل المشروع

```
tools/
├── pkg/                          # مدير الحزم
│   ├── src/
│   │   ├── package_manager.cpp   # نواة مدير الحزم
│   │   ├── package_manager.hpp
│   │   ├── dependency_resolver.cpp  # محلل التبعيات
│   │   ├── dependency_resolver.hpp
│   │   ├── registry_client.cpp   # عميل السجل
│   │   └── registry_client.hpp
│   └── CMakeLists.txt
│
├── lsp/                          # خادم اللغة
│   ├── src/
│   │   ├── lsp_server.cpp        # نواة الخادم
│   │   ├── completion.cpp        # الإكمال التلقائي
│   │   ├── completion.hpp
│   │   ├── diagnostics.cpp       # التشخيصات
│   │   ├── diagnostics.hpp
│   │   ├── hover.cpp             # معلومات التمرير
│   │   ├── hover.hpp
│   │   ├── goto_definition.cpp   # الانتقال للتعريف
│   │   └── goto_definition.hpp
│   └── CMakeLists.txt
│
├── compiler/                     # أدوات المترجم
│   ├── src/
│   │   ├── formatter.cpp         # منسق الكود
│   │   └── ...
│   └── CMakeLists.txt
│
└── vscode-extension/             # إضافة VS Code
    ├── src/
    │   └── extension.ts
    ├── syntaxes/
    │   └── sad.tmLanguage.json
    ├── snippets/
    │   └── sad.json
    └── package.json
```

---

## 📦 مدير الحزم - التفاصيل التقنية

### الصنف الرئيسي: `PackageManager`

```cpp
/**
 * @class PackageManager
 * @brief مدير الحزم الرئيسي للغة ص
 * 
 * يتعامل مع:
 * - تحليل ملفات ص.toml
 * - إدارة التبعيات
 * - تثبيت وإزالة الحزم
 * - إنشاء ملفات القفل
 */
class PackageManager {
public:
    // تهيئة مشروع جديد
    bool init_project(const std::string& name);
    
    // إضافة تبعية جديدة
    bool add_dependency(const std::string& name, 
                       const std::string& version_req);
    
    // إزالة تبعية
    bool remove_dependency(const std::string& name);
    
    // تثبيت جميع التبعيات
    bool install_all();
    
    // تحديث الحزم
    bool update_packages();
    
    // عرض شجرة التبعيات
    void print_dependency_tree();
};
```

### نظام الإصدارات: `SemanticVersion`

```cpp
/**
 * @struct SemanticVersion
 * @brief تمثيل إصدار SemVer 2.0.0
 * 
 * يدعم:
 * - الإصدارات العادية: 1.2.3
 * - الإصدارات التجريبية: 1.0.0-alpha.1
 * - البيانات الوصفية: 1.0.0+build.123
 */
struct SemanticVersion {
    int major;           // الإصدار الرئيسي
    int minor;           // الإصدار الفرعي
    int patch;           // الترقيع
    std::string prerelease;  // معرف قبل الإصدار
    std::string build;   // بيانات البناء
    
    // مقارنة الإصدارات
    bool operator<(const SemanticVersion& other) const;
    bool operator==(const SemanticVersion& other) const;
    
    // تحليل من نص
    static SemanticVersion parse(const std::string& str);
    
    // التحويل لنص
    std::string to_string() const;
};
```

### متطلبات الإصدار: `VersionRequirement`

```cpp
/**
 * @class VersionRequirement
 * @brief تمثيل متطلب إصدار مرن
 * 
 * يدعم:
 * - ^1.2.3  - متوافق (Caret)
 * - ~1.2.3  - تصحيحات (Tilde)
 * - >=1.0   - أكبر أو يساوي
 * - <2.0    - أصغر من
 * - 1.0.0   - محدد بالضبط
 * - >=1.0, <2.0 - نطاق
 */
class VersionRequirement {
public:
    // التحقق من توافق إصدار
    bool satisfies(const SemanticVersion& version) const;
    
    // تحليل من نص
    static VersionRequirement parse(const std::string& str);
    
    // دمج متطلبين
    VersionRequirement intersect(const VersionRequirement& other) const;
};
```

### محلل التبعيات: `DependencyResolver`

```cpp
/**
 * @class DependencyResolver
 * @brief محلل التبعيات باستخدام خوارزمية PubGrub
 * 
 * المميزات:
 * - حل التبعيات المعقدة
 * - كشف التعارضات
 * - رسائل خطأ واضحة بالعربية
 * - دعم التبعيات المتشعبة
 */
class DependencyResolver {
public:
    // حل التبعيات
    ResolveResult resolve(
        const std::vector<PackageConstraint>& constraints
    );
    
    // بناء شجرة التبعيات
    DependencyTree build_tree(const ResolveResult& result);
    
    // الترتيب التوبولوجي للتثبيت
    std::vector<Package> topological_sort(const DependencyTree& tree);
};
```

### عميل السجل: `RegistryClient`

```cpp
/**
 * @class RegistryClient
 * @brief عميل HTTP للتواصل مع سجل الحزم
 * 
 * الميزات:
 * - تخزين مؤقت ذكي
 * - دعم المصادقة
 * - ضغط وفك ضغط الحزم
 */
class RegistryClient {
public:
    // البحث عن حزم
    std::vector<PackageInfo> search(const std::string& query);
    
    // جلب معلومات حزمة
    std::optional<PackageInfo> get_package_info(const std::string& name);
    
    // تنزيل حزمة
    bool download_package(const std::string& name, 
                         const std::string& version,
                         const std::string& dest_path);
    
    // نشر حزمة
    bool publish_package(const std::string& tarball_path,
                        const std::string& token);
};
```

---

## 🖥️ خادم اللغة (LSP) - التفاصيل التقنية

### البروتوكول

خادم LSP يتبع مواصفات [Language Server Protocol 3.17](https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/).

```
العميل (VS Code) ←─ JSON-RPC 2.0 ─→ خادم ص
```

### الرسائل المدعومة

| الرسالة | الاتجاه | الوصف |
|---------|---------|-------|
| `initialize` | → | بدء الجلسة |
| `initialized` | → | تأكيد التهيئة |
| `shutdown` | → | إغلاق الجلسة |
| `textDocument/didOpen` | → | فتح مستند |
| `textDocument/didChange` | → | تغيير مستند |
| `textDocument/didClose` | → | إغلاق مستند |
| `textDocument/completion` | ↔ | الإكمال التلقائي |
| `textDocument/hover` | ↔ | معلومات التمرير |
| `textDocument/definition` | ↔ | الانتقال للتعريف |
| `textDocument/references` | ↔ | إيجاد المراجع |
| `textDocument/formatting` | ↔ | تنسيق المستند |
| `textDocument/publishDiagnostics` | ← | نشر التشخيصات |

### الصنف الرئيسي: `LanguageServer`

```cpp
/**
 * @class LanguageServer
 * @brief خادم LSP للغة ص
 * 
 * يوفر:
 * - الإكمال التلقائي الذكي
 * - تشخيصات فورية
 * - معلومات التمرير
 * - التنقل في الكود
 */
class LanguageServer {
public:
    // تشغيل الخادم
    void run();
    
    // معالجة الرسائل
    void handle_message(const json& message);
    
private:
    // معالجات محددة
    json handle_initialize(const json& params);
    json handle_completion(const json& params);
    json handle_hover(const json& params);
    json handle_definition(const json& params);
    json handle_references(const json& params);
    json handle_formatting(const json& params);
    
    // نشر التشخيصات
    void publish_diagnostics(const std::string& uri);
};
```

### مزود الإكمال: `CompletionProvider`

```cpp
/**
 * @class CompletionProvider
 * @brief مزود الإكمال التلقائي
 * 
 * يقدم:
 * - الكلمات المفتاحية
 * - الأنواع الأساسية
 * - الدوال المدمجة
 * - القصاصات الذكية
 * - الرموز المحلية
 */
class CompletionProvider {
public:
    std::vector<CompletionItem> get_completions(
        const std::string& document,
        Position position,
        const DocumentContext& context
    );
    
private:
    // الكلمات المفتاحية
    static const std::vector<std::string> KEYWORDS;
    
    // الأنواع الأساسية
    static const std::vector<std::string> TYPES;
    
    // الدوال المدمجة مع توثيقها
    static const std::map<std::string, std::string> BUILT_IN_FUNCTIONS;
    
    // القصاصات
    static const std::vector<SnippetInfo> SNIPPETS;
};
```

### مزود التشخيصات: `DiagnosticsProvider`

```cpp
/**
 * @class DiagnosticsProvider
 * @brief مزود تشخيصات الكود
 * 
 * يكتشف:
 * - أخطاء الصياغة
 * - المتغيرات غير المعرفة
 * - أخطاء الأنواع
 * - الكود غير القابل للوصول
 */
class DiagnosticsProvider {
public:
    std::vector<Diagnostic> analyze(const std::string& document);
    
private:
    // فحوصات محددة
    void check_brackets(/* ... */);
    void check_keywords(/* ... */);
    void check_variables(/* ... */);
    void check_imports(/* ... */);
    void check_unreachable(/* ... */);
};
```

### هيكل الأخطاء

```cpp
/**
 * @struct Diagnostic
 * @brief تمثيل تشخيص (خطأ/تحذير)
 */
struct Diagnostic {
    Range range;              // الموقع في المستند
    DiagnosticSeverity severity;  // error, warning, info, hint
    std::string code;         // مثل "ص-001"
    std::string message;      // الرسالة بالعربية
    std::string source;       // "ص-خادم"
};

// رموز الأخطاء
enum class ErrorCode {
    UNMATCHED_BRACKETS = 1,   // ص-001
    MISSING_KEYWORD = 2,      // ص-002
    UNDEFINED_VARIABLE = 3,   // ص-003
    IMPORT_FAILED = 4,        // ص-004
    TYPE_MISMATCH = 5,        // ص-005
    UNDEFINED_FUNCTION = 6,   // ص-006
    WRONG_ARGUMENTS = 7,      // ص-007
    UNREACHABLE_CODE = 8,     // ص-008
    GENERAL_WARNING = 9,      // ص-009
    SYNTAX_ERROR = 10         // ص-010
};
```

---

## ✨ منسق الكود - التفاصيل التقنية

### الصنف الرئيسي: `CodeFormatter`

```cpp
/**
 * @class CodeFormatter
 * @brief منسق كود لغة ص
 * 
 * يطبق قواعد تنسيق موحدة:
 * - المسافات البادئة
 * - المسافات حول العوامل
 * - الأسطر الفارغة
 * - طول السطر الأقصى
 */
class CodeFormatter {
public:
    // تنسيق كود
    std::string format(const std::string& source);
    
    // تعيين الخيارات
    void set_options(const FormatterOptions& opts);
    
private:
    FormatterOptions options_;
    
    // المعجم (Lexer) للتحليل
    std::vector<Token> tokenize(const std::string& source);
    
    // إعادة بناء الكود المنسق
    std::string rebuild(const std::vector<Token>& tokens);
};
```

### خيارات التنسيق

```cpp
/**
 * @struct FormatterOptions
 * @brief خيارات تنسيق الكود
 */
struct FormatterOptions {
    int indent_size = 4;              // حجم المسافة البادئة
    bool use_spaces = true;           // مسافات vs Tab
    int max_line_length = 80;         // طول السطر الأقصى
    bool space_after_comma = true;    // مسافة بعد الفاصلة
    bool space_around_operators = true;  // مسافات حول العوامل
    bool blank_line_between_functions = true;  // سطر فارغ بين الدوال
    bool braces_same_line = true;     // الأقواس على نفس السطر
};
```

### الكلمات المفتاحية للمسافات البادئة

```cpp
// كلمات تفتح كتلة (تزيد المسافة البادئة)
const std::set<std::string> BLOCK_OPENERS = {
    "دالة", "إذا", "وإلا", "وإلا_إذا",
    "طالما", "لكل", "صنف", "سمة", "تعداد", "بنية"
};

// كلمات تغلق كتلة (تقلل المسافة البادئة)
const std::set<std::string> BLOCK_CLOSERS = {
    "نهاية"
};

// كلمات تغلق وتفتح (تقلل ثم تزيد)
const std::set<std::string> BLOCK_CONTINUERS = {
    "وإلا", "وإلا_إذا"
};
```

---

## 💻 إضافة VS Code - التفاصيل التقنية

### هيكل الملفات

```
vscode-extension/
├── package.json              # تكوين الإضافة
├── src/
│   └── extension.ts          # الكود الرئيسي
├── syntaxes/
│   └── sad.tmLanguage.json   # قواعد التلوين
├── snippets/
│   └── sad.json              # القصاصات
└── language-configuration.json  # تكوين اللغة
```

### الدالة الرئيسية

```typescript
/**
 * تفعيل الإضافة
 */
export function activate(context: vscode.ExtensionContext) {
    // إنشاء قناة الإخراج
    outputChannel = vscode.window.createOutputChannel('Sad Language Server');
    
    // تسجيل الأوامر
    registerCommands(context);
    
    // تسجيل الميزات الإضافية
    registerAdditionalFeatures(context);
    
    // بدء خادم اللغة
    startLanguageServer(context);
}
```

### الأوامر المسجلة

```typescript
// أوامر أساسية
'sad.restartServer'      // إعادة تشغيل الخادم
'sad.showOutputChannel'  // عرض السجل

// أوامر التشغيل
'sad.run'                // تشغيل البرنامج
'sad.compile'            // تصريف البرنامج

// أوامر التنسيق
'sad.format'             // تنسيق الكود

// أوامر إدارة الحزم
'sad.pkg.init'           // تهيئة مشروع
'sad.pkg.install'        // تثبيت حزمة
'sad.pkg.remove'         // إزالة حزمة
'sad.pkg.update'         // تحديث الحزم
'sad.pkg.search'         // البحث عن حزم
```

### تكوين عميل اللغة

```typescript
const clientOptions: LanguageClientOptions = {
    documentSelector: [
        { scheme: 'file', language: 'sad' },
        { scheme: 'untitled', language: 'sad' }
    ],
    synchronize: {
        fileEvents: vscode.workspace.createFileSystemWatcher('**/*.{s,sad}')
    },
    initializationOptions: {
        locale: vscode.env.language,
        formatting: {
            tabSize: config.get('formatting.tabSize', 4),
            insertSpaces: config.get('formatting.insertSpaces', true)
        }
    }
};
```

---

## 🧪 الاختبارات

### اختبارات الوحدة

```cpp
// tests/tools/test_tools_integration.cpp

// اختبارات الإصدار الدلالي
TEST(SemanticVersion, Parse) { /* ... */ }
TEST(SemanticVersion, Compare) { /* ... */ }
TEST(SemanticVersion, Equality) { /* ... */ }

// اختبارات محلل التبعيات
TEST(DependencyResolver, SimpleChain) { /* ... */ }
TEST(DependencyResolver, DiamondDependency) { /* ... */ }
TEST(DependencyResolver, ConflictDetection) { /* ... */ }

// اختبارات منسق الكود
TEST(CodeFormatter, Indentation) { /* ... */ }
TEST(CodeFormatter, NestedBlocks) { /* ... */ }
TEST(CodeFormatter, OperatorSpacing) { /* ... */ }

// اختبارات LSP
TEST(LSPServer, Completion) { /* ... */ }
TEST(LSPServer, Diagnostics) { /* ... */ }
TEST(LSPServer, Hover) { /* ... */ }
```

### تشغيل الاختبارات

```bash
# بناء وتشغيل اختبارات الأدوات
cmake --build build --target test_tools_integration
./build/bin/test_tools_integration
```

---

## 🔧 البناء والتكوين

### CMake

```cmake
# tools/CMakeLists.txt

# مدير الحزم
add_subdirectory(pkg)

# خادم LSP
add_subdirectory(lsp)

# منسق الكود
add_executable(sad-format
    compiler/src/formatter.cpp
)
```

### بناء المشروع الكامل

```bash
# تكوين البناء
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# بناء جميع الأهداف
cmake --build build

# بناء هدف محدد
cmake --build build --target sad-pkg
cmake --build build --target sad-lsp
cmake --build build --target sad-format
```

---

## 📝 إرشادات التطوير

### أسلوب الكود

1. **التعليقات بالعربية**: جميع التعليقات والتوثيق بالعربية
2. **أسماء الدوال**: بالإنجليزية للتوافق مع C++
3. **رسائل المستخدم**: بالعربية دائماً
4. **التوثيق**: استخدام Doxygen

### مثال على التوثيق

```cpp
/**
 * @brief تحليل إصدار من نص - Parse version from string
 * 
 * يحول نص إصدار SemVer إلى كائن SemanticVersion
 * 
 * @param str النص المراد تحليله (مثل "1.2.3-alpha")
 * @return SemanticVersion الإصدار المُحلل
 * @throws std::invalid_argument إذا كان التنسيق غير صالح
 * 
 * @example
 *     auto v = SemanticVersion::parse("1.2.3");
 *     // v.major = 1, v.minor = 2, v.patch = 3
 */
static SemanticVersion parse(const std::string& str);
```

---

## 🔗 الروابط المفيدة

- [مواصفات LSP](https://microsoft.github.io/language-server-protocol/)
- [مواصفات SemVer](https://semver.org/lang/ar/)
- [VS Code Extension API](https://code.visualstudio.com/api)
- [خوارزمية PubGrub](https://nex3.medium.com/pubgrub-2fb6470504f)

---

<div align="center">

**الحمد لله رب العالمين**

</div>

</div>
