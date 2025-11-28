# 📐 نظام إدارة الأخطاء - نظرة عامة والتصميم المعماري
## Error Management System - Overview and Architectural Design

<div dir="rtl">

## 📋 نظرة عامة / Overview

### الهدف من النظام
تطوير نظام شامل ومتكامل لإدارة الأخطاء في لغة "ص" (Sad Language) يوفر:
- تتبع دقيق للأخطاء مع معلومات الموقع (ملف، سطر، عمود)
- رسائل خطأ واضحة ومفيدة بالعربية والإنجليزية
- اقتراحات لإصلاح الأخطاء (fix-it hints)
- دعم IDE/LSP من خلال إخراج JSON
- معالجة أخطاء متعددة (لا يتوقف عند أول خطأ)
- استرداد ذكي من الأخطاء (error recovery)

### الوضع الحالي (Current State)

#### ✅ ما هو موجود:
1. **Lexer**: يستخدم exceptions عند الأخطاء
2. **Parser**: يجمع أخطاء في `std::vector<std::string>`
   - دوال: `error()`, `hasErrors()`, `getErrors()`, `printErrors()`
   - متغير: `panicMode_` لتجنب تتالي الأخطاء
3. **Interpreter**: استثناءات مخصصة:
   - `ExecutionError` - أخطاء التنفيذ
   - `RuntimeError` - أخطاء وقت التشغيل

#### ❌ ما هو مفقود:
1. نظام موحد لإدارة الأخطاء عبر جميع المكونات
2. تتبع دقيق للموقع (file, line, column)
3. مستويات الشدة (Error, Warning, Info, Note)
4. رموز أخطاء موحدة (Error Codes)
5. اقتراحات إصلاح (Fix-it hints)
6. إخراج JSON للـ IDE/LSP
7. دعم i18n (internationalization) للرسائل
8. نظام تسجيل مركزي (central logging)

---

## 🏗️ التصميم المعماري / Architectural Design

### المكونات الأساسية

```
┌─────────────────────────────────────────────────────────┐
│                    ErrorManager                         │
│  (المدير المركزي لجميع الأخطاء)                        │
│  - تجميع الأخطاء من جميع المكونات                      │
│  - تنسيق وعرض الأخطاء                                  │
│  - إخراج JSON للـ IDE                                  │
└────────────┬───────────────────────────────┬────────────┘
             │                               │
    ┌────────▼────────┐            ┌─────────▼─────────┐
    │   Diagnostic    │            │  DiagnosticSink   │
    │  (وصف الخطأ)    │            │  (مجمع الأخطاء)   │
    │  - الموقع       │            │  - قائمة          │
    │  - الرسالة      │            │    Diagnostics    │
    │  - الشدة        │            │  - الفرز والتصفية │
    │  - الكود        │            └───────────────────┘
    │  - الاقتراحات  │
    └─────────────────┘

┌──────────────────────────────────────────────────────────┐
│              مصادر الأخطاء / Error Sources               │
├──────────────┬─────────────┬──────────────┬─────────────┤
│    Lexer     │   Parser    │ Interpreter  │  Semantic   │
│              │             │              │  Analyzer   │
└──────────────┴─────────────┴──────────────┴─────────────┘
```

### تدفق البيانات (Data Flow)

```
1. Component يكتشف خطأ
   ↓
2. ينشئ Diagnostic object
   ↓
3. يرسله إلى DiagnosticSink
   ↓
4. ErrorManager يجمع من DiagnosticSink
   ↓
5. يطبق الفرز والتصفية
   ↓
6. ينسق للعرض (Console / JSON)
   ↓
7. يعرض للمستخدم أو IDE
```

---

## 📊 نماذج البيانات / Data Models

### 1. SourceLocation - موقع الخطأ

```cpp
struct SourceLocation {
    std::string filename;    // اسم الملف
    size_t line;            // رقم السطر (1-based)
    size_t column;          // رقم العمود (1-based)
    size_t offset;          // الإزاحة من بداية الملف
    size_t length;          // طول النص المرتبط بالخطأ
    
    std::string toString() const;
    bool isValid() const;
};
```

### 2. DiagnosticSeverity - مستوى الشدة

```cpp
enum class DiagnosticSeverity {
    ERROR,      // خطأ - يمنع الترجمة/التنفيذ
    WARNING,    // تحذير - قد يسبب مشاكل
    INFO,       // معلومة - للتوضيح فقط
    NOTE,       // ملاحظة - سياق إضافي
    HINT        // تلميح - اقتراح تحسين
};
```

### 3. ErrorCode - رموز الأخطاء

```cpp
enum class ErrorCode {
    // Lexical Errors (LEX001-LEX099)
    LEX_INVALID_CHARACTER,      // LEX001
    LEX_UNTERMINATED_STRING,    // LEX002
    LEX_INVALID_NUMBER,         // LEX003
    LEX_INVALID_ESCAPE,         // LEX004
    
    // Syntax Errors (SYN001-SYN099)
    SYN_UNEXPECTED_TOKEN,       // SYN001
    SYN_MISSING_SEMICOLON,      // SYN002
    SYN_UNCLOSED_BRACKET,       // SYN003
    SYN_MISSING_IDENTIFIER,     // SYN004
    
    // Semantic Errors (SEM001-SEM099)
    SEM_UNDEFINED_VARIABLE,     // SEM001
    SEM_TYPE_MISMATCH,          // SEM002
    SEM_REDEFINITION,           // SEM003
    SEM_UNDEFINED_FUNCTION,     // SEM004
    
    // Runtime Errors (RUN001-RUN099)
    RUN_DIVISION_BY_ZERO,       // RUN001
    RUN_INDEX_OUT_OF_RANGE,     // RUN002
    RUN_NULL_REFERENCE,         // RUN003
    RUN_STACK_OVERFLOW,         // RUN004
};

std::string getErrorCodeString(ErrorCode code);
std::string getErrorCodeDescription(ErrorCode code, Language lang);
```

### 4. FixItHint - اقتراح الإصلاح

```cpp
struct FixItHint {
    SourceLocation location;     // موقع الإصلاح
    std::string replacement;     // النص البديل
    std::string description_ar;  // وصف بالعربية
    std::string description_en;  // وصف بالإنجليزية
    
    enum Type {
        INSERT,     // إدراج نص
        REMOVE,     // حذف نص
        REPLACE     // استبدال نص
    } type;
};
```

### 5. Diagnostic - التشخيص الكامل

```cpp
class Diagnostic {
private:
    ErrorCode code_;
    DiagnosticSeverity severity_;
    SourceLocation location_;
    std::string message_ar_;
    std::string message_en_;
    std::vector<FixItHint> fixits_;
    std::vector<Diagnostic> notes_;  // ملاحظات مرتبطة
    
public:
    // البناء
    Diagnostic(ErrorCode code, 
               DiagnosticSeverity severity,
               SourceLocation location,
               const std::string& message_ar,
               const std::string& message_en);
    
    // إضافة ملاحظات واقتراحات
    void addNote(const Diagnostic& note);
    void addFixIt(const FixItHint& fixit);
    
    // التنسيق
    std::string format(Language lang, bool colorize) const;
    std::string formatJSON() const;
    
    // الوصول
    ErrorCode getCode() const;
    DiagnosticSeverity getSeverity() const;
    const SourceLocation& getLocation() const;
    const std::string& getMessage(Language lang) const;
};
```

---

## 🎯 واجهة برمجية (API Design)

### ErrorManager - المدير المركزي

```cpp
class ErrorManager {
public:
    // Singleton pattern
    static ErrorManager& getInstance();
    
    // إضافة تشخيص
    void report(const Diagnostic& diag);
    void reportError(ErrorCode code, 
                    const SourceLocation& loc,
                    const std::string& msg_ar,
                    const std::string& msg_en);
    void reportWarning(ErrorCode code, 
                      const SourceLocation& loc,
                      const std::string& msg_ar,
                      const std::string& msg_en);
    
    // إدارة التشخيصات
    size_t getErrorCount() const;
    size_t getWarningCount() const;
    bool hasErrors() const;
    void clear();
    
    // العرض والإخراج
    void printAll(Language lang = Language::ARABIC, 
                  bool colorize = true) const;
    std::string toJSON() const;
    
    // الإعدادات
    void setMaxErrors(size_t max);
    void setLanguage(Language lang);
    void enableColors(bool enable);
    void setOutputFile(const std::string& filename);
    
private:
    ErrorManager() = default;
    DiagnosticSink sink_;
    Language defaultLang_ = Language::ARABIC;
    bool colorizeOutput_ = true;
    size_t maxErrors_ = 100;
};
```

### DiagnosticBuilder - بناء سهل للتشخيصات

```cpp
class DiagnosticBuilder {
public:
    DiagnosticBuilder(ErrorCode code, 
                      DiagnosticSeverity severity,
                      const SourceLocation& loc);
    
    // إضافة الرسائل
    DiagnosticBuilder& withMessage(const std::string& ar, 
                                    const std::string& en);
    
    // إضافة اقتراحات
    DiagnosticBuilder& withFixIt(const FixItHint& hint);
    DiagnosticBuilder& withFixItInsert(const SourceLocation& loc,
                                        const std::string& text);
    DiagnosticBuilder& withFixItReplace(const SourceLocation& loc,
                                         const std::string& text);
    
    // إضافة ملاحظات
    DiagnosticBuilder& withNote(const Diagnostic& note);
    
    // الإنشاء والإبلاغ
    Diagnostic build();
    void report();  // ينشئ ويبلغ مباشرة
};

// مثال استخدام
DiagnosticBuilder(ErrorCode::SYN_MISSING_SEMICOLON,
                  DiagnosticSeverity::ERROR,
                  location)
    .withMessage("متوقع ';' في نهاية الجملة",
                 "Expected ';' at end of statement")
    .withFixItInsert(location, ";")
    .report();
```

---

## 🎨 تنسيق العرض / Display Formatting

### Console Output (مع ألوان)

```
❌ خطأ [SYN002]: متوقع ';' في نهاية الجملة
   Error [SYN002]: Expected ';' at end of statement

  --> test.s:5:20
   |
 5 | رقم س = 10 + 5
   |                ^ متوقع ';' هنا / Expected ';' here
   |
   = ملاحظة: الجمل في لغة ص يجب أن تنتهي بـ ';'
     Note: Statements in Sad language must end with ';'
   
💡 اقتراح / Suggestion: أضف ';' في نهاية السطر / Add ';' at end of line
```

### JSON Output (للـ IDE/LSP)

```json
{
  "diagnostics": [
    {
      "code": "SYN002",
      "severity": "error",
      "location": {
        "file": "test.s",
        "line": 5,
        "column": 20,
        "offset": 48,
        "length": 1
      },
      "message": {
        "ar": "متوقع ';' في نهاية الجملة",
        "en": "Expected ';' at end of statement"
      },
      "fixits": [
        {
          "type": "insert",
          "location": {
            "file": "test.s",
            "line": 5,
            "column": 20
          },
          "replacement": ";",
          "description": {
            "ar": "أضف ';' في نهاية السطر",
            "en": "Add ';' at end of line"
          }
        }
      ],
      "notes": []
    }
  ],
  "summary": {
    "errors": 1,
    "warnings": 0,
    "info": 0
  }
}
```

---

## 🔗 التكامل مع المكونات / Component Integration

### 1. Lexer Integration

```cpp
// في lexer_core.cpp
void LexerCore::scanToken() {
    char c = advance();
    switch (c) {
        case '"':
            if (!scanString()) {
                ErrorManager::getInstance().reportError(
                    ErrorCode::LEX_UNTERMINATED_STRING,
                    getCurrentLocation(),
                    "نص غير منتهي - لم يتم إغلاق علامات التنصيص",
                    "Unterminated string - missing closing quote"
                );
            }
            break;
        // ...
    }
}
```

### 2. Parser Integration

```cpp
// في parser_core.cpp
void ParserCore::consume(TokenType expected, 
                         const std::string& msg_ar,
                         const std::string& msg_en) {
    if (check(expected)) {
        advance();
        return;
    }
    
    DiagnosticBuilder(ErrorCode::SYN_UNEXPECTED_TOKEN,
                      DiagnosticSeverity::ERROR,
                      current().getLocation())
        .withMessage(msg_ar, msg_en)
        .withFixItInsert(current().getLocation(), 
                        getTokenString(expected))
        .report();
}
```

### 3. Interpreter Integration

```cpp
// في statement_executor.cpp
void StatementExecutor::visitForStmt(AST::ForStmt& node) {
    Data::Value iterable = evaluateExpression(*node.iterable);
    
    if (!iterable.isIterable()) {
        ErrorManager::getInstance().reportError(
            ErrorCode::RUN_TYPE_MISMATCH,
            node.getLocation(),
            "نوع غير قابل للتكرار في حلقة for",
            "Non-iterable type in for loop"
        );
        throw ExecutionError("Type error");
    }
    // ...
}
```

---

## 📈 خطة التنفيذ / Implementation Roadmap

### المرحلة 0: التصميم (مكتملة) ✅
- ✅ تحليل الوضع الحالي
- ✅ تصميم المعمارية
- ✅ تحديد نماذج البيانات
- ✅ تصميم الواجهات البرمجية

### المرحلة 1: هياكل البيانات (2-4 أيام)
- إنشاء الفئات الأساسية
- تطبيق ErrorCode enum
- تطبيق Diagnostic class
- تطبيق ErrorManager skeleton

### المرحلة 2: التكامل (3-7 أيام)
- دمج مع Lexer
- دمج مع Parser
- دمج مع Interpreter
- اختبارات التكامل

### المرحلة 3: التحسينات (2-5 أيام)
- إضافة Fix-it hints
- تحسين التنسيق
- إضافة JSON output
- إضافة i18n support

### المرحلة 4: الاختبارات والتوثيق (2-4 أيام)
- اختبارات شاملة
- توثيق API
- أمثلة الاستخدام
- دليل المساهمين

---

## 🎯 معايير النجاح / Success Criteria

### وظيفية:
- ✅ تتبع دقيق لموقع الأخطاء (file:line:col)
- ✅ رسائل واضحة بالعربية والإنجليزية
- ✅ دعم مستويات الشدة المختلفة
- ✅ اقتراحات إصلاح مفيدة
- ✅ إخراج JSON للـ IDE

### أداء:
- ✅ لا تأثير ملحوظ على سرعة الترجمة
- ✅ معالجة تصل لـ 1000 خطأ بكفاءة
- ✅ ذاكرة معقولة (< 10MB للأخطاء)

### جودة:
- ✅ تغطية اختبارات > 80%
- ✅ توثيق كامل للـ API
- ✅ أمثلة واضحة
- ✅ تعليقات ثنائية اللغة

---

## 📚 المراجع / References

1. **Clang Diagnostics**: https://clang.llvm.org/docs/InternalsManual.html#diagnostics
2. **Rust Error Handling**: https://doc.rust-lang.org/book/ch09-00-error-handling.html
3. **GCC Diagnostics**: https://gcc.gnu.org/onlinedocs/cpp/Diagnostics.html
4. **LSP Diagnostics**: https://microsoft.github.io/language-server-protocol/specifications/lsp/3.17/specification/#diagnostic

---

**تاريخ الإنشاء**: 23 نوفمبر 2025  
**الإصدار**: 1.0  
**الحالة**: تصميم مكتمل - جاهز للتنفيذ  
**المستندات**: docs/architecture/design.md

</div>
