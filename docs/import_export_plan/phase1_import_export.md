# المرحلة 1: نظام الاستيراد والتصدير
# Phase 1: Import/Export System

**الحالة / Status:** 🚧 قيد التنفيذ / In Progress  
**الأولوية / Priority:** P0 (حرجة / Critical)  
**الوقت المقدر / Estimated Time:** 2-3 أسابيع

---

## 📋 نظرة عامة / Overview

### بالعربية
هذه المرحلة تركز على بناء نظام استيراد وتصدير قوي ومرن للغة ص. يشمل ذلك:
- **الاستيراد الأساسي** (Basic Import)
- **الاستيراد الانتقائي** (Selective Import)
- **الاستيراد مع الاسم المستعار** (Aliased Import)
- **نظام التصدير الواضح** (Explicit Export System)
- **التحميل الديناميكي** (Dynamic Loading)

### English
This phase focuses on building a robust and flexible import/export system for Sad language, including:
- **Basic Import**
- **Selective Import**
- **Aliased Import**
- **Explicit Export System**
- **Dynamic Loading**

---

## 🎯 الأهداف / Goals

### الأساسية / Core Goals
1. ✅ استيراد وحدة كاملة
2. ✅ استيراد انتقائي للدوال/الأصناف
3. ✅ استيراد مع تغيير الاسم
4. ✅ استيراد شامل (wildcard)
5. ✅ تصدير واضح للعناصر العامة
6. ⬜ استيراد ديناميكي في وقت التشغيل
7. ⬜ إعادة تصدير من وحدات أخرى

### المميزات الإضافية / Extended Features
- ⬜ استيراد مشروط حسب المنصة
- ⬜ استيراد متداخل (nested imports)
- ⬜ Namespace management
- ⬜ Circular dependency detection

---

## 📝 النحو / Syntax

### 1. الاستيراد الأساسي / Basic Import

```ebnf
import_stmt ::= 'استورد' module_name ['كـ' alias]
module_name ::= identifier ('.' identifier)*
```

**أمثلة / Examples:**
```s
# استيراد وحدة كاملة
استورد رياضيات

# استخدام الوحدة
اطبع(رياضيات.ط)           # 3.14159...
رقم ج = رياضيات.جذر(16)    # 4

# استيراد مع اسم مستعار
استورد رياضيات كـ م

اطبع(م.ط)
رقم ق = م.قوة(2، 10)       # 1024
```

---

### 2. الاستيراد الانتقائي / Selective Import

```ebnf
from_import ::= 'من' module_name 'استورد' import_list
import_list ::= '*' | identifier ('،' identifier)*
```

**أمثلة / Examples:**
```s
# استيراد دوال محددة
من رياضيات استورد جذر، قوة، ط

اطبع(ط)              # لا حاجة لـ رياضيات.ط
رقم ج = جذر(25)       # 5
رقم ق = قوة(3، 3)     # 27

# استيراد كل شيء (wildcard)
من رياضيات استورد *

اطبع(جيب(ط / 2))    # 1.0
اطبع(جيب_تمام(0))   # 1.0
```

---

### 3. نظام التصدير / Export System

```ebnf
export_decl ::= 'صدر' (var_decl | func_decl | class_decl)
```

**مثال وحدة / Module Example:**
```s
# ملف: رياضيات.ص / File: math.s

# ثوابت عامة (مصدرة)
صدر ط = 3.14159265359
صدر ه = 2.71828182846

# دالة عامة (مصدرة)
صدر دالة رقم جذر(رقم ع)
    إرجاع ع ** 0.5
نهاية

صدر دالة رقم قوة(رقم أساس، رقم أس)
    إرجاع أساس ** أس
نهاية

# دالة خاصة (غير مصدرة)
دالة _مساعد_داخلي(رقم س)
    إرجاع س * 2
نهاية

# صنف عام (مصدر)
صدر صنف متجه
    رقم س
    رقم ص
    
    باني(رقم س، رقم ص)
        هذا.س = س
        هذا.ص = ص
    نهاية
    
    دالة رقم طول()
        إرجاع جذر(هذا.س ** 2 + هذا.ص ** 2)
    نهاية
نهاية
```

**الاستخدام / Usage:**
```s
استورد رياضيات

# يمكن الوصول للعناصر المصدرة فقط
اطبع(رياضيات.ط)              # ✓ عام
رقم ج = رياضيات.جذر(144)      # ✓ عام
متجه م = رياضيات.متجه(3، 4)   # ✓ عام
اطبع(م.طول())                # ✓ 5.0

# لا يمكن الوصول للدوال الخاصة
# رياضيات._مساعد_داخلي(10)  # ✗ خطأ!
```

---

## 🏗️ البنية المعمارية / Architecture

### مكونات النظام / System Components

```
┌─────────────────────────────────────────┐
│         Parser (محلل نحوي)              │
│  - parseImportStmt()                   │
│  - parseFromImport()                   │
│  - parseExportDecl()                   │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│         AST Nodes (عقد الشجرة)         │
│  - ImportStmt                          │
│  - ExportDecl                          │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│     Module Resolver (محلل الوحدات)     │
│  - resolveModule(name)                 │
│  - findModuleFile(name)                │
│  - parseModule(file)                   │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│    Symbol Table (جدول الرموز)          │
│  - addImportedSymbol()                 │
│  - checkVisibility()                   │
│  - resolveSymbol()                     │
└─────────────────────────────────────────┘
```

---

## 💻 التنفيذ / Implementation

### 1. AST Nodes

**ملف:** `src/parser/ast/import_export_nodes.h`

```cpp
// include/ast/import_export_nodes.h
#pragma once

#include "ast_node.h"
#include <vector>
#include <string>

namespace Sad {
namespace AST {

/**
 * عقدة استيراد / Import statement node
 * 
 * مثال: استورد رياضيات كـ م
 * Example: import math as m
 */
class ImportStmt : public Stmt {
public:
    std::vector<std::string> modulePath;  // ["رياضيات"] أو ["pkg", "math"]
    std::string alias;                    // "م" أو ""
    std::vector<std::string> symbols;     // [] للاستيراد الكامل
    bool isWildcard;                      // من ... استورد *
    
    ImportStmt(
        std::vector<std::string> path,
        std::string alias = "",
        std::vector<std::string> syms = {},
        bool wildcard = false
    ) : modulePath(std::move(path)),
        alias(std::move(alias)),
        symbols(std::move(syms)),
        isWildcard(wildcard) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

/**
 * عقدة تصدير / Export declaration node
 * 
 * مثال: صدر دالة جذر(رقم ع)
 * Example: export function sqrt(num x)
 */
class ExportDecl : public Stmt {
public:
    StmtPtr declaration;  // الإعلان المُصدَّر
    
    explicit ExportDecl(StmtPtr decl)
        : declaration(std::move(decl)) {}
    
    void accept(ASTVisitor& visitor) override;
    std::string toString() const override;
};

} // namespace AST
} // namespace Sad
```

---

### 2. Parser Methods

**ملف:** `src/parser/parser_core_imports.cpp`

```cpp
// src/parser/parser_core_imports.cpp
#include "parser_core.h"
#include "ast/import_export_nodes.h"

namespace Sad {

/**
 * تحليل جملة استيراد
 * Parse import statement
 * 
 * Syntax:
 *   - استورد نموذج [كـ اسم]
 *   - من نموذج استورد (دالة | *)
 */
StmtPtr ParserCore::parseImportStmt() {
    Token importToken = previous();
    
    // التحقق من نمط "من ... استورد"
    if (match(TokenType::KEYWORD_FROM)) {
        return parseFromImport();
    }
    
    // نمط عادي: استورد نموذج [كـ اسم]
    std::vector<std::string> modulePath = parseModulePath();
    
    std::string alias;
    if (match(TokenType::KEYWORD_AS)) {
        Token aliasToken = consume(TokenType::IDENTIFIER, 
                                   "Expected alias name after 'كـ'");
        alias = aliasToken.value;
    }
    
    consume(TokenType::SEMICOLON, "Expected ';' after import");
    
    return std::make_unique<ImportStmt>(
        std::move(modulePath),
        std::move(alias),
        std::vector<std::string>(),
        false
    );
}

/**
 * تحليل جملة "من ... استورد"
 * Parse "from ... import" statement
 */
StmtPtr ParserCore::parseFromImport() {
    std::vector<std::string> modulePath = parseModulePath();
    
    consume(TokenType::KEYWORD_IMPORT, 
            "Expected 'استورد' after module name");
    
    // التحقق من نمط wildcard: من نموذج استورد *
    if (match(TokenType::STAR)) {
        consume(TokenType::SEMICOLON, "Expected ';'");
        return std::make_unique<ImportStmt>(
            std::move(modulePath),
            "",
            std::vector<std::string>(),
            true  // wildcard
        );
    }
    
    // تحليل قائمة الرموز: من نموذج استورد أ، ب، ج
    std::vector<std::string> symbols;
    do {
        Token sym = consume(TokenType::IDENTIFIER, 
                           "Expected symbol name");
        symbols.push_back(sym.value);
    } while (match(TokenType::COMMA));
    
    consume(TokenType::SEMICOLON, "Expected ';'");
    
    return std::make_unique<ImportStmt>(
        std::move(modulePath),
        "",
        std::move(symbols),
        false
    );
}

/**
 * تحليل مسار الوحدة
 * Parse module path
 * 
 * مثال: pkg.subpkg.module
 */
std::vector<std::string> ParserCore::parseModulePath() {
    std::vector<std::string> path;
    
    Token name = consume(TokenType::IDENTIFIER, 
                        "Expected module name");
    path.push_back(name.value);
    
    // دعم المسارات المتداخلة: pkg.subpkg.module
    while (match(TokenType::DOT)) {
        name = consume(TokenType::IDENTIFIER, 
                      "Expected module name after '.'");
        path.push_back(name.value);
    }
    
    return path;
}

/**
 * تحليل إعلان التصدير
 * Parse export declaration
 * 
 * Syntax: صدر (دالة | صنف | متغير)
 */
StmtPtr ParserCore::parseExportDecl() {
    Token exportToken = previous();
    
    // تحليل الإعلان التالي
    StmtPtr decl = parseDeclaration();
    
    if (!decl) {
        error("Expected declaration after 'صدر'");
        return nullptr;
    }
    
    return std::make_unique<ExportDecl>(std::move(decl));
}

} // namespace Sad
```

---

### 3. Module Resolver

**ملف:** `src/modules/module_resolver.h`

```cpp
// include/modules/module_resolver.h
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>

namespace Sad {
namespace Modules {

/**
 * محلل الوحدات / Module Resolver
 * 
 * مسؤول عن:
 * - إيجاد ملفات الوحدات
 * - تحليل الوحدات
 * - إدارة الاعتماديات
 * - منع التبعيات الدائرية
 */
class ModuleResolver {
public:
    /**
     * حل وحدة وتحميلها
     * Resolve and load a module
     * 
     * @param modulePath مسار الوحدة (["pkg", "math"])
     * @return المودول المحلل أو nullptr
     */
    Module* resolveModule(const std::vector<std::string>& modulePath);
    
    /**
     * إيجاد ملف وحدة
     * Find module file
     * 
     * @param modulePath مسار الوحدة
     * @return المسار الكامل للملف أو ""
     */
    std::string findModuleFile(const std::vector<std::string>& modulePath);
    
    /**
     * إضافة مسار بحث
     * Add search path
     */
    void addSearchPath(const std::string& path);
    
    /**
     * الحصول على مسارات البحث
     * Get search paths
     */
    std::vector<std::string> getSearchPaths() const;
    
private:
    std::vector<std::string> searchPaths_;
    std::map<std::string, std::unique_ptr<Module>> loadedModules_;
    std::vector<std::string> loadingStack_;  // لكشف التبعيات الدائرية
    
    bool detectCircularDependency(const std::string& moduleName);
};

/**
 * وحدة / Module
 */
struct Module {
    std::string name;
    std::string filePath;
    std::map<std::string, Symbol> exports;  // الرموز المصدرة
    std::vector<std::string> dependencies;  // الاعتماديات
    
    bool isExported(const std::string& symbolName) const {
        return exports.find(symbolName) != exports.end();
    }
};

} // namespace Modules
} // namespace Sad
```

---

## 🧪 الاختبارات / Testing

### Test Cases

**ملف:** `tests/modules/test_import_export.cpp`

```cpp
#include <gtest/gtest.h>
#include "modules/module_resolver.h"
#include "parser/parser_core.h"

using namespace Sad;

/**
 * اختبار 1: استيراد أساسي
 */
TEST(ImportExport, BasicImport) {
    const char* code = R"(
        استورد رياضيات
        اطبع(رياضيات.ط)
    )";
    
    Parser parser(code);
    auto ast = parser.parse();
    
    ASSERT_NE(ast, nullptr);
    // التحقق من وجود ImportStmt
    // ...
}

/**
 * اختبار 2: استيراد مع اسم مستعار
 */
TEST(ImportExport, ImportWithAlias) {
    const char* code = R"(
        استورد رياضيات كـ م
        اطبع(م.جذر(16))
    )";
    
    // ...
}

/**
 * اختبار 3: استيراد انتقائي
 */
TEST(ImportExport, SelectiveImport) {
    const char* code = R"(
        من رياضيات استورد جذر، قوة
        رقم ج = جذر(25)
    )";
    
    // ...
}

/**
 * اختبار 4: استيراد wildcard
 */
TEST(ImportExport, WildcardImport) {
    const char* code = R"(
        من رياضيات استورد *
        اطبع(جيب(ط))
    )";
    
    // ...
}

/**
 * اختبار 5: التصدير
 */
TEST(ImportExport, ExportSystem) {
    const char* code = R"(
        صدر دالة رقم جذر(رقم ع)
            إرجاع ع ** 0.5
        نهاية
    )";
    
    // ...
}

/**
 * اختبار 6: الوصول للعناصر الخاصة (يجب أن يفشل)
 */
TEST(ImportExport, PrivateAccessFails) {
    const char* code = R"(
        استورد نموذج
        # محاولة الوصول لدالة خاصة
        نموذج._دالة_خاصة()
    )";
    
    // يجب أن يعطي خطأ
    // ...
}
```

---

## 📚 أمثلة عملية / Practical Examples

### مثال 1: مكتبة رياضيات

**ملف:** `stdlib/math.s`
```s
# مكتبة الرياضيات / Math Library

صدر ط = 3.14159265359
صدر ه = 2.71828182846

صدر دالة رقم جذر(رقم ع)
    إرجاع ع ** 0.5
نهاية

صدر دالة رقم قوة(رقم أساس، رقم أس)
    إرجاع أساس ** أس
نهاية

صدر دالة رقم قيمة_مطلقة(رقم ع)
    إذا ع < 0
        إرجاع -ع
    نهاية
    إرجاع ع
نهاية

# دالة مساعدة خاصة
دالة _تحقق_صحة(رقم ع)
    إرجاع !ع.نان() && !ع.لانهائي()
نهاية
```

**الاستخدام:**
```s
استورد رياضيات

رقم نصف_القطر = 5.0
رقم المساحة = رياضيات.ط * رياضيات.قوة(نصف_القطر، 2)
اطبع(ن"مساحة الدائرة: {المساحة}")

# أو استخدام استيراد انتقائي
من رياضيات استورد ط، قوة

رقم المساحة2 = ط * قوة(نصف_القطر، 2)
```

---

### مثال 2: نظام تسجيل (Logging)

**ملف:** `utils/logger.s`
```s
# نظام التسجيل / Logging System

صدر صنف سجل
    قائمة رسائل = []
    نص مستوى = "INFO"
    
    دالة معلومات(نص رسالة)
        هذا._سجل("INFO"، رسالة)
    نهاية
    
    دالة تحذير(نص رسالة)
        هذا._سجل("WARN"، رسالة)
    نهاية
    
    دالة خطأ(نص رسالة)
        هذا._سجل("ERROR"، رسالة)
    نهاية
    
    # دالة خاصة
    دالة _سجل(نص مستوى، نص رسالة)
        نص طابع = وقت.الآن().تنسيق("%Y-%m-%d %H:%M:%S")
        نص سطر = ن"[{طابع}] [{مستوى}] {رسالة}"
        هذا.رسائل.أضف(سطر)
        اطبع(سطر)
    نهاية
نهاية

صدر متغير سجل_عام = سجل()
```

**الاستخدام:**
```s
من utils.logger استورد سجل، سجل_عام

# إنشاء سجل خاص
متغير سجلي = سجل()
سجلي.معلومات("بدء التطبيق")
سجلي.تحذير("ذاكرة منخفضة")

# أو استخدام السجل العام
سجل_عام.معلومات("معالجة البيانات")
```

---

## 🔧 ملفات التنفيذ / Implementation Files

### الملفات المطلوبة / Required Files

```
src/
├── parser/
│   ├── ast/
│   │   └── import_export_nodes.{h,cpp}    # عقد AST
│   └── parser_core_imports.cpp             # تحليل الاستيراد/التصدير
│
├── modules/
│   ├── module_resolver.{h,cpp}             # محلل الوحدات
│   ├── module_loader.{h,cpp}               # محمل الوحدات
│   └── symbol_table.{h,cpp}                # جدول الرموز
│
└── interpreter/
    └── import_executor.cpp                 # تنفيذ الاستيراد

tests/
└── modules/
    ├── test_import_export.cpp              # اختبارات الاستيراد/التصدير
    ├── test_module_resolver.cpp            # اختبارات المحلل
    └── fixtures/                           # ملفات اختبار
        ├── math.s
        ├── logger.s
        └── nested/
            └── submodule.s
```

---

## ✅ قائمة المهام / Task Checklist

### الأسبوع 1: الأساسيات

- [ ] إنشاء AST nodes للاستيراد/التصدير
- [ ] تنفيذ `parseImportStmt()` في Parser
- [ ] تنفيذ `parseExportDecl()` في Parser
- [ ] إضافة keywords للـ Lexer
- [ ] اختبارات Parser الأساسية

### الأسبوع 2: المحلل

- [ ] تنفيذ `ModuleResolver` class
- [ ] نظام مسارات البحث
- [ ] كشف التبعيات الدائرية
- [ ] اختبارات المحلل
- [ ] دمج مع الـ Interpreter

### الأسبوع 3: المميزات المتقدمة

- [ ] الاستيراد الديناميكي (`__استورد__`)
- [ ] إعادة التصدير
- [ ] التحسينات والتخزين المؤقت
- [ ] اختبارات شاملة
- [ ] التوثيق

---

## 📈 مؤشرات الأداء / Performance Metrics

- ⚡ **وقت تحليل الاستيراد:** < 1ms لكل جملة
- ⚡ **وقت حل الوحدة:** < 10ms لكل وحدة
- ⚡ **استهلاك الذاكرة:** < 100KB لكل وحدة محملة
- ✅ **تغطية الاختبارات:** > 90%

---

## 🚀 الخطوة التالية / Next Step

بعد إكمال هذه المرحلة، انتقل إلى:
**[المرحلة 2: محمل الوحدات الذكي](phase2_module_loader.md)**

---

**آخر تحديث / Last Updated:** 8 ديسمبر 2025
