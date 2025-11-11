# ⚠️ الأخطاء والتشخيص / Errors & Diagnostics

**التاريخ:** 10 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** مرجع تفصيلي لنظام الأخطاء والتشخيص

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا الملف يوثق **نظام الأخطاء الكامل** للغة "ص"، بما في ذلك أنواع الأخطاء، الرسائل ثنائية اللغة (عربي/إنجليزي)، أكواد التشخيص، وآليات تسجيل الأخطاء (logging).

### (EN) Description
This file documents the **complete error system** for Sad language, including error types, bilingual messages (Arabic/English), diagnostic codes, and error logging mechanisms.

---

## 📦 الاعتماديات / Dependencies

- ✅ `include/errors/error_manager.h` - Error management
- ✅ `include/debug/` - Debug logging
- ✅ `docs/language_spec/05_python_features.md` - Exception handling

---

## 📋 أنواع الأخطاء / Error Categories

| # | النوع (AR) | Type (EN) | الرمز | Priority |
|---|----------|----------|-------|---------|
| 1 | خطأ معجمي | Lexer Error | LEX | P0 |
| 2 | خطأ نحوي | Syntax Error | SYN | P0 |
| 3 | خطأ دلالي | Semantic Error | SEM | P0 |
| 4 | خطأ أنواع | Type Error | TYP | P0 |
| 5 | خطأ وقت التشغيل | Runtime Error | RUN | P0 |
| 6 | خطأ استيراد | Import Error | IMP | P1 |
| 7 | خطأ ذاكرة | Memory Error | MEM | P1 |
| 8 | تحذير | Warning | WAR | P1 |
| 9 | معلومة | Info | INF | P2 |

---

## 1️⃣ Error Code System (نظام أكواد الأخطاء)

### Format

```
[CATEGORY][NUMBER]: Message (AR/EN)

Examples:
LEX001: رمز غير معروف / Unknown token
SYN042: توقع ')' / Expected ')'
TYP015: عدم تطابق الأنواع / Type mismatch
RUN008: القسمة على صفر / Division by zero
```

### Error Structure

```cpp
// include/errors/error.h
enum class ErrorCategory {
    Lexer,      // LEX
    Syntax,     // SYN
    Semantic,   // SEM
    Type,       // TYP
    Runtime,    // RUN
    Import,     // IMP
    Memory,     // MEM
    Warning,    // WAR
    Info        // INF
};

enum class ErrorSeverity {
    Info,       // معلومة / Informational
    Warning,    // تحذير / Warning
    Error,      // خطأ / Error
    Fatal       // خطأ فادح / Fatal error
};

class Error {
public:
    ErrorCategory category;
    ErrorSeverity severity;
    int code;
    std::string message_ar;  // Arabic message
    std::string message_en;  // English message
    std::string file;
    int line;
    int column;
    std::string source_line;
    std::vector<std::string> notes;  // Additional notes
    
    // Constructor
    Error(ErrorCategory cat, int code, 
          const std::string& msg_ar, const std::string& msg_en,
          const std::string& file, int line, int col);
    
    // Format error for display
    std::string format(bool use_arabic = true) const;
    
    // Add contextual note
    void addNote(const std::string& note);
};
```

---

## 2️⃣ Lexer Errors (أخطاء المحلل المعجمي)

### Error Codes

```cpp
// LEX001-LEX099: Lexer errors
LEX001: رمز غير معروف / Unknown token
LEX002: سلسلة نصية غير مغلقة / Unterminated string
LEX003: رقم غير صحيح / Invalid number format
LEX004: تعليق غير مغلق / Unterminated comment
LEX005: حرف غير صالح / Invalid character
LEX006: سلسلة Unicode غير صحيحة / Invalid Unicode escape
```

### Examples

```s
// LEX001: رمز غير معروف
رقم ع = 10 @ 5  // '@' رمز غير معروف

// المخرج / Output:
// [LEX001] خطأ معجمي في السطر 1، العمود 12:
// رمز غير معروف: '@'
//   رقم ع = 10 @ 5
//              ^

// LEX002: سلسلة غير مغلقة
نص ن = "مرحباً

// المخرج / Output:
// [LEX002] خطأ معجمي في السطر 1، العمود 9:
// سلسلة نصية غير مغلقة
//   نص ن = "مرحباً
//          ^

// LEX003: رقم غير صحيح
رقم ع = 10.5.3

// المخرج / Output:
// [LEX003] خطأ معجمي في السطر 1، العمود 9:
// رقم غير صحيح: '10.5.3'
//   رقم ع = 10.5.3
//          ^
```

---

## 3️⃣ Syntax Errors (أخطاء نحوية)

### Error Codes

```cpp
// SYN001-SYN099: Syntax errors
SYN001: توقع عبارة / Expected statement
SYN002: توقع تعبير / Expected expression
SYN003: توقع معرّف / Expected identifier
SYN004: توقع '{' / Expected '{'
SYN005: توقع '}' / Expected '}'
SYN010: توقع '(' / Expected '('
SYN011: توقع ')' / Expected ')'
SYN012: توقع '[' / Expected '['
SYN013: توقع ']' / Expected ']'
SYN020: توقع 'نهاية' / Expected 'نهاية'
SYN030: عدم تطابق الأقواس / Mismatched brackets
SYN040: جملة غير مكتملة / Incomplete statement
```

### Examples

```s
// SYN010-SYN011: توقع '(' أو ')'
دالة جمع رقم أ، رقم ب  // نسينا '('
    إرجاع أ + ب
نهاية

// المخرج / Output:
// [SYN010] خطأ نحوي في السطر 1، العمود 11:
// توقع '(' بعد اسم الدالة
//   دالة جمع رقم أ، رقم ب
//            ^
// ملاحظة: الصيغة الصحيحة: دالة جمع(رقم أ، رقم ب)

// SYN020: نسينا 'نهاية'
إذا ع > 10
    اطبع("كبير")
// نسينا 'نهاية'

// المخرج / Output:
// [SYN020] خطأ نحوي في السطر 3:
// توقع 'نهاية' لإغلاق كتلة 'إذا'
//   إذا ع > 10
//   ^

// SYN030: عدم تطابق الأقواس
مصفوفة م = [1، 2، 3)  // استخدمنا ')' بدلاً من ']'

// المخرج / Output:
// [SYN030] خطأ نحوي في السطر 1، العمود 21:
// عدم تطابق الأقواس: توقع ']' ولكن وجد ')'
//   مصفوفة م = [1، 2، 3)
//                       ^
```

---

## 4️⃣ Semantic Errors (أخطاء دلالية)

### Error Codes

```cpp
// SEM001-SEM099: Semantic errors
SEM001: متغير غير معرّف / Undefined variable
SEM002: دالة غير معرّفة / Undefined function
SEM003: صنف غير معرّف / Undefined class
SEM004: تعريف مكرر / Duplicate definition
SEM005: عدد معاملات خاطئ / Wrong number of arguments
SEM010: عضو غير موجود / Member not found
SEM015: لا يمكن تعيين قيمة / Cannot assign value
SEM020: عملية غير مدعومة / Unsupported operation
SEM025: الوصول إلى عضو خاص / Access to private member
```

### Examples

```s
// SEM001: متغير غير معرّف
اطبع(ع)  // 'ع' غير معرّف

// المخرج / Output:
// [SEM001] خطأ دلالي في السطر 1، العمود 7:
// متغير غير معرّف: 'ع'
//   اطبع(ع)
//        ^
// اقتراح: هل تقصد أحد هذه المتغيرات؟
//   - 'عدد' (السطر 5)
//   - 'عمر' (السطر 10)

// SEM002: دالة غير معرّفة
رقم ن = جذر_تكعيبي(27)  // دالة غير موجودة

// المخرج / Output:
// [SEM002] خطأ دلالي في السطر 1، العمود 9:
// دالة غير معرّفة: 'جذر_تكعيبي'
//   رقم ن = جذر_تكعيبي(27)
//          ^
// اقتراح: هل تقصد 'جذر' من 'رياضيات'؟
// استخدم: استورد رياضيات

// SEM004: تعريف مكرر
دالة جمع(رقم أ، رقم ب)
    إرجاع أ + ب
نهاية

دالة جمع(رقم س، رقم ص)  // تعريف مكرر
    إرجاع س + ص
نهاية

// المخرج / Output:
// [SEM004] خطأ دلالي في السطر 5، العمود 6:
// تعريف مكرر للدالة: 'جمع'
//   دالة جمع(رقم س، رقم ص)
//        ^
// ملاحظة: تم تعريفها مسبقاً في السطر 1

// SEM005: عدد معاملات خاطئ
دالة قوة(رقم أساس، رقم أس)
    إرجاع أساس ** أس
نهاية

رقم ن = قوة(2)  // نسينا معامل

// المخرج / Output:
// [SEM005] خطأ دلالي في السطر 5، العمود 9:
// عدد معاملات خاطئ للدالة 'قوة'
// توقع: 2 معاملات، ولكن تم تمرير: 1 معامل
//   رقم ن = قوة(2)
//          ^
// الصيغة الصحيحة: قوة(رقم أساس، رقم أس)
```

---

## 5️⃣ Type Errors (أخطاء الأنواع)

### Error Codes

```cpp
// TYP001-TYP099: Type errors
TYP001: عدم تطابق الأنواع / Type mismatch
TYP002: لا يمكن تحويل النوع / Cannot convert type
TYP003: نوع غير صالح للعملية / Invalid type for operation
TYP004: نوع إرجاع غير صحيح / Incorrect return type
TYP005: نوع معامل غير صحيح / Incorrect parameter type
```

### Examples

```s
// TYP001: عدم تطابق الأنواع
رقم ع: رقم = "نص"  // نوع خاطئ

// المخرج / Output:
// [TYP001] خطأ أنواع في السطر 1، العمود 17:
// عدم تطابق الأنواع
// متوقع: 'رقم'
// ولكن تم الحصول على: 'نص'
//   رقم ع: رقم = "نص"
//                ^

// TYP003: نوع غير صالح للعملية
نص اسم = "أحمد"
رقم عمر = 25
نص معلومات = اسم + عمر  // لا يمكن جمع نص مع رقم

// المخرج / Output:
// [TYP003] خطأ أنواع في السطر 3، العمود 23:
// نوع غير صالح للعملية '+'
// النوع الأيسر: 'نص'
// النوع الأيمن: 'رقم'
//   نص معلومات = اسم + عمر
//                      ^
// اقتراح: استخدم تحويل النوع: اسم + نص(عمر)

// TYP004: نوع إرجاع غير صحيح
دالة رقم جمع(رقم أ، رقم ب)
    إرجاع "نتيجة"  // نوع خاطئ
نهاية

// المخرج / Output:
// [TYP004] خطأ أنواع في السطر 2، العمود 12:
// نوع إرجاع غير صحيح
// الدالة 'جمع' يجب أن تُرجع: 'رقم'
// ولكن تم إرجاع: 'نص'
//       إرجاع "نتيجة"
//             ^
```

---

## 6️⃣ Runtime Errors (أخطاء وقت التشغيل)

### Error Codes

```cpp
// RUN001-RUN099: Runtime errors
RUN001: القسمة على صفر / Division by zero
RUN002: فهرس خارج النطاق / Index out of range
RUN003: مفتاح غير موجود / Key not found
RUN004: قيمة فارغة / Null value error
RUN005: تجاوز سعة المكدس / Stack overflow
RUN010: فشل تحويل النوع / Type conversion failed
RUN015: فشل استيراد / Import failed
RUN020: خطأ في الملف / File error
RUN025: خطأ في الشبكة / Network error
```

### Examples

```s
// RUN001: القسمة على صفر
دالة قسم(رقم أ، رقم ب)
    إرجاع أ / ب
نهاية

رقم نتيجة = قسم(10، 0)  // خطأ!

// المخرج / Output:
// [RUN001] خطأ وقت التشغيل في السطر 5:
// القسمة على صفر
//   رقم نتيجة = قسم(10، 0)
//               ^
// تتبع المكالمات:
//   في الدالة 'قسم' (السطر 2)
//   من الملف الرئيسي (السطر 5)

// RUN002: فهرس خارج النطاق
مصفوفة أرقام = [1، 2، 3]
اطبع(أرقام[10])  // خطأ!

// المخرج / Output:
// [RUN002] خطأ وقت التشغيل في السطر 2، العمود 14:
// فهرس خارج النطاق
// الفهرس: 10
// حجم المصفوفة: 3
// النطاق الصحيح: 0 إلى 2
//   اطبع(أرقام[10])
//               ^

// RUN003: مفتاح غير موجود
قاموس بيانات = {"اسم": "أحمد"، "عمر": 25}
اطبع(بيانات["مدينة"])  // خطأ!

// المخرج / Output:
// [RUN003] خطأ وقت التشغيل في السطر 2، العمود 17:
// مفتاح غير موجود: 'مدينة'
// المفاتيح المتاحة: ['اسم'، 'عمر']
//   اطبع(بيانات["مدينة"])
//                 ^
// اقتراح: استخدم 'في' للتحقق من وجود المفتاح:
//   إذا "مدينة" في بيانات
```

---

## 7️⃣ Error Manager (مدير الأخطاء)

### API

```cpp
// include/errors/error_manager.h
class ErrorManager {
private:
    std::vector<Error> errors;
    std::vector<Error> warnings;
    bool use_arabic;
    
public:
    // Language preference
    void setLanguage(bool arabic);
    
    // Report error
    void reportError(ErrorCategory category, int code,
                    const std::string& msg_ar, const std::string& msg_en,
                    const std::string& file, int line, int col);
    
    // Report warning
    void reportWarning(const std::string& msg_ar, const std::string& msg_en,
                      const std::string& file, int line, int col);
    
    // Check if has errors
    bool hasErrors() const;
    bool hasWarnings() const;
    
    // Get counts
    int getErrorCount() const;
    int getWarningCount() const;
    
    // Print all errors
    void printErrors() const;
    void printWarnings() const;
    
    // Clear errors
    void clear();
    
    // Get last error
    Error* getLastError();
};
```

### Usage Example

```cpp
// src/parser/parser_core.cpp
void Parser::parseIfStatement() {
    consume(TokenType::IF, "Expected 'إذا'");
    
    if (!check(TokenType::LEFT_PAREN)) {
        errorManager.reportError(
            ErrorCategory::Syntax,
            SYN010,
            "توقع '(' بعد 'إذا'",
            "Expected '(' after 'إذا'",
            currentFile,
            current.line,
            current.column
        );
        return nullptr;
    }
    
    // ... rest of parsing
}
```

---

## 8️⃣ Bilingual Messages (الرسائل ثنائية اللغة)

### Message Database

```cpp
// include/errors/error_messages.h
struct ErrorMessage {
    int code;
    const char* message_ar;
    const char* message_en;
    const char* suggestion_ar;
    const char* suggestion_en;
};

// Error messages database
static const ErrorMessage ERROR_MESSAGES[] = {
    // Lexer errors
    {LEX001, "رمز غير معروف", "Unknown token", 
             "تحقق من الرموز المستخدمة", "Check the tokens used"},
    
    {LEX002, "سلسلة نصية غير مغلقة", "Unterminated string",
             "أضف علامة اقتباس في نهاية السلسلة", "Add closing quote"},
    
    // Syntax errors
    {SYN010, "توقع '('", "Expected '('",
             "أضف '(' بعد اسم الدالة", "Add '(' after function name"},
    
    {SYN011, "توقع ')'", "Expected ')'",
             "أضف ')' لإغلاق القوس", "Add ')' to close parenthesis"},
    
    // ... more messages
};
```

### Language Selection

```s
// تحديد اللغة / Set language
@لغة "عربي"    // استخدام الرسائل بالعربية
@لغة "english"  // Use English messages

// Or via command line
// sad --lang ar script.s
// sad --lang en script.s
```

---

## 9️⃣ Diagnostic Codes (أكواد التشخيص)

### Code Format

```
[CAT][NUM]: [MSG_AR] / [MSG_EN]

Categories:
- LEX: Lexer (001-099)
- SYN: Syntax (001-099)
- SEM: Semantic (001-099)
- TYP: Type (001-099)
- RUN: Runtime (001-099)
- IMP: Import (001-099)
- MEM: Memory (001-099)
- WAR: Warning (001-099)
- INF: Info (001-099)
```

### Complete Error Codes Reference

```cpp
// docs/error_codes.md

## Lexer Errors (LEX001-LEX099)
LEX001: رمز غير معروف / Unknown token
LEX002: سلسلة غير مغلقة / Unterminated string
LEX003: رقم غير صحيح / Invalid number
LEX004: تعليق غير مغلق / Unterminated comment
LEX005: حرف غير صالح / Invalid character

## Syntax Errors (SYN001-SYN099)
SYN001: توقع عبارة / Expected statement
SYN002: توقع تعبير / Expected expression
SYN003: توقع معرّف / Expected identifier
SYN004: توقع '{' / Expected '{'
SYN005: توقع '}' / Expected '}'
SYN010: توقع '(' / Expected '('
SYN011: توقع ')' / Expected ')'
SYN020: توقع 'نهاية' / Expected 'نهاية'

## Semantic Errors (SEM001-SEM099)
SEM001: متغير غير معرّف / Undefined variable
SEM002: دالة غير معرّفة / Undefined function
SEM003: صنف غير معرّف / Undefined class
SEM004: تعريف مكرر / Duplicate definition
SEM005: عدد معاملات خاطئ / Wrong number of arguments

## Type Errors (TYP001-TYP099)
TYP001: عدم تطابق الأنواع / Type mismatch
TYP002: لا يمكن تحويل النوع / Cannot convert type
TYP003: نوع غير صالح للعملية / Invalid type for operation
TYP004: نوع إرجاع غير صحيح / Incorrect return type

## Runtime Errors (RUN001-RUN099)
RUN001: القسمة على صفر / Division by zero
RUN002: فهرس خارج النطاق / Index out of range
RUN003: مفتاح غير موجود / Key not found
RUN004: قيمة فارغة / Null value error
RUN005: تجاوز سعة المكدس / Stack overflow
```

---

## 🔟 Debug Logging (تسجيل الأخطاء)

### Log Levels

```cpp
// include/debug/logger.h
enum class LogLevel {
    DEBUG,      // تفاصيل تطوير / Development details
    INFO,       // معلومات عامة / General info
    WARNING,    // تحذير / Warning
    ERROR,      // خطأ / Error
    FATAL       // خطأ فادح / Fatal error
};

class Logger {
public:
    static void setLevel(LogLevel level);
    static void setOutput(const std::string& file);
    
    static void debug(const std::string& msg);
    static void info(const std::string& msg);
    static void warning(const std::string& msg);
    static void error(const std::string& msg);
    static void fatal(const std::string& msg);
    
    // Formatted logging
    template<typename... Args>
    static void debugf(const char* format, Args... args);
    
    template<typename... Args>
    static void errorf(const char* format, Args... args);
};
```

### Usage

```cpp
// src/lexer/lexer_core.cpp
Token Lexer::scanToken() {
    DEBUG_PRINT("Scanning token at position %d", position);
    
    char c = advance();
    
    if (isDigit(c)) {
        DEBUG_PRINT("Found digit: %c", c);
        return scanNumber();
    }
    
    if (isAlpha(c)) {
        DEBUG_PRINT("Found identifier start: %c", c);
        return scanIdentifier();
    }
    
    ERROR_PRINT("Unknown character: %c at line %d", c, line);
    errorManager.reportError(
        ErrorCategory::Lexer,
        LEX001,
        ن"رمز غير معروف: '{c}'",
        ن"Unknown token: '{c}'",
        filename,
        line,
        column
    );
    
    return Token::makeError();
}
```

---

## 📝 Complete Example (مثال كامل)

### Test File: `errors_examples.s`

```s
// ===========================================
// اختبار: الأخطاء والتشخيص
// Test: Errors and Diagnostics
// ===========================================

// 1. خطأ معجمي / Lexer error
// رقم ع = 10 @ 5  // رمز غير معروف: '@'

// 2. خطأ نحوي / Syntax error
// دالة test(  // توقع ')'
//     اطبع("test")
// نهاية

// 3. خطأ دلالي / Semantic error
// اطبع(متغير_غير_موجود)  // متغير غير معرّف

// 4. خطأ أنواع / Type error
// رقم ع: رقم = "نص"  // عدم تطابق الأنواع

// 5. خطأ وقت التشغيل (معالج) / Runtime error (handled)
حاول
    رقم ن = 10 / 0  // القسمة على صفر
امسك خطأ_قسمة خ
    اطبع("تم اصطياد خطأ: " + خ.رسالة)
نهاية

// 6. خطأ فهرس / Index error (handled)
حاول
    مصفوفة م = [1، 2، 3]
    اطبع(م[10])  // فهرس خارج النطاق
امسك خطأ_فهرس خ
    اطبع("خطأ فهرس: " + خ.رسالة)
نهاية

// 7. خطأ مفتاح / Key error (handled)
حاول
    قاموس ق = {"أ": 1}
    اطبع(ق["ب"])  // مفتاح غير موجود
امسك خطأ_مفتاح خ
    اطبع("خطأ مفتاح: " + خ.رسالة)
نهاية

اطبع("✓ جميع أمثلة الأخطاء تم اختبارها")
```

---

## 🔧 Implementation Files

### Headers

1. **`include/errors/error.h`** - Error class
2. **`include/errors/error_manager.h`** - Error management
3. **`include/errors/error_messages.h`** - Message database
4. **`include/errors/error_codes.h`** - Error code definitions
5. **`include/debug/logger.h`** - Debug logging

### Sources

1. **`src/errors/error_manager.cpp`**
2. **`src/errors/error_formatter.cpp`**
3. **`src/errors/error_messages.cpp`**
4. **`src/debug/logger.cpp`**

---

## 🧪 Tests

### File: `tests/spec_rules/errors_examples.s`

```s
// اختبار معالجة الأخطاء / Error handling test
حاول
    رقم ن = 10 / 0
امسك خطأ خ
    تأكد(خ.نوع == "خطأ_قسمة")
    اطبع("✓ تم اصطياد الخطأ بنجاح")
نهاية
```

---

## 🔧 Git Workflow

```powershell
git checkout -b feat/spec/errors-diagnostics
git add docs/language_spec/rules/09_errors_and_diagnostics.md
git add tests/spec_rules/errors_examples.s
git add include/errors/*.h
git add src/errors/*.cpp
git commit -m "[spec/errors] Add error system and diagnostics specification"
git push --set-upstream origin feat/spec/errors-diagnostics
```

---

**آخر تحديث:** 10 نوفمبر 2025  
**الحالة:** جاهز للتنفيذ ✅  
**الصفحات:** 20+
