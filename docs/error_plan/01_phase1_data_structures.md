# 📦 المرحلة 1: هياكل البيانات والواجهات الأساسية
## Phase 1: Core Data Structures and Interfaces

<div dir="rtl">

## 📋 نظرة عامة / Overview

**المدة المتوقعة**: 2-4 أيام  
**الأولوية**: عالية جداً (P0)  
**التبعيات**: المرحلة 0 (التصميم)

### الأهداف
- إنشاء جميع هياكل البيانات الأساسية
- تطبيق واجهات ErrorManager
- إعداد البنية التحتية للتشخيصات
- إضافة تعليقات ثنائية اللغة شاملة

---

## 📝 المهام التفصيلية / Detailed Tasks

### المهمة 1.1: إنشاء ملف error_codes.h ✅

**الوصف**: تعريف جميع رموز الأخطاء بشكل منظم

**الملف**: `include/errors/error_codes.h`

```cpp
/**
 * @file error_codes.h
 * @brief (AR) رموز الأخطاء الموحدة للغة "ص"
 *        (EN) Unified error codes for Sad language
 * 
 * (AR) هذا الملف يحتوي على جميع رموز الأخطاء المستخدمة في اللغة
 *      مصنفة حسب المرحلة: معجمية، نحوية، دلالية، تنفيذية
 * 
 * (EN) This file contains all error codes used in the language
 *      classified by phase: lexical, syntax, semantic, runtime
 * 
 * @author Sad Language Development Team
 * @date November 23, 2025
 * @version 1.0
 */

#pragma once

#include <string>
#include <unordered_map>

namespace Sad {
namespace Errors {

/**
 * @enum ErrorCode
 * @brief (AR) رموز الأخطاء المصنفة
 *        (EN) Classified error codes
 */
enum class ErrorCode {
    // ====================================================================
    // (AR) أخطاء معجمية / (EN) Lexical Errors
    // Range: LEX001 - LEX099
    // ====================================================================
    
    LEX_INVALID_CHARACTER,       ///< LEX001: (AR) رمز غير صالح
                                ///< (EN) Invalid character
    
    LEX_UNTERMINATED_STRING,     ///< LEX002: (AR) نص غير مغلق
                                ///< (EN) Unterminated string
    
    LEX_INVALID_NUMBER,          ///< LEX003: (AR) رقم غير صالح
                                ///< (EN) Invalid number format
    
    LEX_INVALID_ESCAPE,          ///< LEX004: (AR) تسلسل هروب غير صالح
                                ///< (EN) Invalid escape sequence
    
    LEX_NUMBER_TOO_LARGE,        ///< LEX005: (AR) رقم كبير جداً
                                ///< (EN) Number too large
    
    LEX_INVALID_UTF8,            ///< LEX006: (AR) ترميز UTF-8 غير صالح
                                ///< (EN) Invalid UTF-8 encoding
    
    // ====================================================================
    // (AR) أخطاء نحوية / (EN) Syntax Errors
    // Range: SYN001 - SYN099
    // ====================================================================
    
    SYN_UNEXPECTED_TOKEN,        ///< SYN001: (AR) رمز غير متوقع
                                ///< (EN) Unexpected token
    
    SYN_MISSING_SEMICOLON,       ///< SYN002: (AR) فاصلة منقوطة مفقودة
                                ///< (EN) Missing semicolon
    
    SYN_UNCLOSED_BRACKET,        ///< SYN003: (AR) قوس غير مغلق
                                ///< (EN) Unclosed bracket
    
    SYN_MISSING_IDENTIFIER,      ///< SYN004: (AR) معرّف مفقود
                                ///< (EN) Missing identifier
    
    SYN_INVALID_EXPRESSION,      ///< SYN005: (AR) تعبير غير صالح
                                ///< (EN) Invalid expression
    
    SYN_UNEXPECTED_EOF,          ///< SYN006: (AR) نهاية ملف غير متوقعة
                                ///< (EN) Unexpected end of file
    
    SYN_MISSING_COLON,           ///< SYN007: (AR) نقطتان مفقودتان
                                ///< (EN) Missing colon
    
    SYN_INVALID_ASSIGNMENT,      ///< SYN008: (AR) إسناد غير صالح
                                ///< (EN) Invalid assignment
    
    // ====================================================================
    // (AR) أخطاء دلالية / (EN) Semantic Errors
    // Range: SEM001 - SEM099
    // ====================================================================
    
    SEM_UNDEFINED_VARIABLE,      ///< SEM001: (AR) متغير غير معرّف
                                ///< (EN) Undefined variable
    
    SEM_TYPE_MISMATCH,           ///< SEM002: (AR) عدم تطابق الأنواع
                                ///< (EN) Type mismatch
    
    SEM_REDEFINITION,            ///< SEM003: (AR) إعادة تعريف
                                ///< (EN) Redefinition
    
    SEM_UNDEFINED_FUNCTION,      ///< SEM004: (AR) دالة غير معرّفة
                                ///< (EN) Undefined function
    
    SEM_WRONG_ARG_COUNT,         ///< SEM005: (AR) عدد معاملات خاطئ
                                ///< (EN) Wrong argument count
    
    SEM_INVALID_OPERATION,       ///< SEM006: (AR) عملية غير صالحة
                                ///< (EN) Invalid operation
    
    SEM_CONST_ASSIGNMENT,        ///< SEM007: (AR) إسناد لثابت
                                ///< (EN) Assignment to constant
    
    SEM_UNDEFINED_CLASS,         ///< SEM008: (AR) صنف غير معرّف
                                ///< (EN) Undefined class
    
    SEM_PRIVATE_ACCESS,          ///< SEM009: (AR) وصول لعضو خاص
                                ///< (EN) Access to private member
    
    // ====================================================================
    // (AR) أخطاء وقت التشغيل / (EN) Runtime Errors
    // Range: RUN001 - RUN099
    // ====================================================================
    
    RUN_DIVISION_BY_ZERO,        ///< RUN001: (AR) قسمة على صفر
                                ///< (EN) Division by zero
    
    RUN_INDEX_OUT_OF_RANGE,      ///< RUN002: (AR) فهرس خارج النطاق
                                ///< (EN) Index out of range
    
    RUN_NULL_REFERENCE,          ///< RUN003: (AR) مرجع فارغ
                                ///< (EN) Null reference
    
    RUN_STACK_OVERFLOW,          ///< RUN004: (AR) تجاوز سعة المكدس
                                ///< (EN) Stack overflow
    
    RUN_KEY_NOT_FOUND,           ///< RUN005: (AR) مفتاح غير موجود
                                ///< (EN) Key not found
    
    RUN_INVALID_CAST,            ///< RUN006: (AR) تحويل نوع غير صالح
                                ///< (EN) Invalid type cast
    
    RUN_FILE_ERROR,              ///< RUN007: (AR) خطأ في الملف
                                ///< (EN) File error
    
    RUN_IMPORT_ERROR,            ///< RUN008: (AR) خطأ في الاستيراد
                                ///< (EN) Import error
};

/**
 * @brief (AR) اللغة المستخدمة في الرسائل
 *        (EN) Language used for messages
 */
enum class Language {
    ARABIC,      ///< (AR) العربية
    ENGLISH,     ///< (EN) English
    BOTH         ///< (AR) كلاهما / (EN) Both
};

/**
 * @brief (AR) يحول رمز الخطأ إلى نص
 *        (EN) Converts error code to string
 * 
 * @param code (AR) رمز الخطأ / (EN) Error code
 * @return (AR) النص المقابل (مثل "SYN002") / (EN) Corresponding string
 */
std::string getErrorCodeString(ErrorCode code);

/**
 * @brief (AR) يرجع وصف الخطأ باللغة المحددة
 *        (EN) Returns error description in specified language
 * 
 * @param code (AR) رمز الخطأ / (EN) Error code
 * @param lang (AR) اللغة / (EN) Language
 * @return (AR) الوصف / (EN) Description
 */
std::string getErrorDescription(ErrorCode code, Language lang);

/**
 * @brief (AR) يرجع اسم الفئة (معجمي، نحوي، إلخ)
 *        (EN) Returns category name (lexical, syntax, etc.)
 * 
 * @param code (AR) رمز الخطأ / (EN) Error code
 * @return (AR) اسم الفئة / (EN) Category name
 */
std::string getErrorCategory(ErrorCode code);

} // namespace Errors
} // namespace Sad
```

**الوقت المتوقع**: 2 ساعة  
**الاختبارات**: اختبارات وحدات لكل دالة مساعدة

---

### المهمة 1.2: إنشاء ملف source_location.h ✅

**الوصف**: هيكل لتتبع موقع الأخطاء في الكود المصدري

**الملف**: `include/errors/source_location.h`

```cpp
/**
 * @file source_location.h
 * @brief (AR) تتبع موقع الأخطاء في الكود المصدري
 *        (EN) Track error locations in source code
 * 
 * @author Sad Language Development Team
 * @date November 23, 2025
 */

#pragma once

#include <string>
#include <sstream>

namespace Sad {
namespace Errors {

/**
 * @struct SourceLocation
 * @brief (AR) موقع في الكود المصدري
 *        (EN) Location in source code
 */
struct SourceLocation {
    std::string filename;    ///< (AR) اسم الملف / (EN) File name
    size_t line;            ///< (AR) رقم السطر (1-based) / (EN) Line number
    size_t column;          ///< (AR) رقم العمود (1-based) / (EN) Column number
    size_t offset;          ///< (AR) الإزاحة من البداية / (EN) Offset from start
    size_t length;          ///< (AR) طول النص / (EN) Text length
    
    /**
     * @brief (AR) البناء الافتراضي
     *        (EN) Default constructor
     */
    SourceLocation()
        : filename(""), line(0), column(0), offset(0), length(0) {}
    
    /**
     * @brief (AR) بناء مع قيم
     *        (EN) Constructor with values
     */
    SourceLocation(const std::string& file, size_t ln, size_t col,
                   size_t off = 0, size_t len = 1)
        : filename(file), line(ln), column(col), offset(off), length(len) {}
    
    /**
     * @brief (AR) يتحقق من صحة الموقع
     *        (EN) Checks if location is valid
     * 
     * @return (AR) true إذا كان صالحاً / (EN) true if valid
     */
    bool isValid() const {
        return line > 0 && column > 0;
    }
    
    /**
     * @brief (AR) يحول الموقع إلى نص
     *        (EN) Converts location to string
     * 
     * @return (AR) نص بصيغة "file:line:col" / (EN) String in format "file:line:col"
     */
    std::string toString() const {
        if (!isValid()) {
            return "<unknown location>";
        }
        
        std::ostringstream oss;
        if (!filename.empty()) {
            oss << filename << ":";
        }
        oss << line << ":" << column;
        return oss.str();
    }
    
    /**
     * @brief (AR) يحول الموقع إلى نص موسع
     *        (EN) Converts location to extended string
     * 
     * @return (AR) نص مفصل / (EN) Detailed string
     */
    std::string toDetailedString() const {
        std::ostringstream oss;
        oss << toString();
        if (length > 1) {
            oss << " (length: " << length << ")";
        }
        return oss.str();
    }
    
    /**
     * @brief (AR) يقارن موقعين
     *        (EN) Compares two locations
     */
    bool operator<(const SourceLocation& other) const {
        if (filename != other.filename) return filename < other.filename;
        if (line != other.line) return line < other.line;
        return column < other.column;
    }
    
    bool operator==(const SourceLocation& other) const {
        return filename == other.filename &&
               line == other.line &&
               column == other.column;
    }
};

/**
 * @brief (AR) نطاق في الكود المصدري
 *        (EN) Range in source code
 */
struct SourceRange {
    SourceLocation start;    ///< (AR) نقطة البداية / (EN) Start location
    SourceLocation end;      ///< (AR) نقطة النهاية / (EN) End location
    
    SourceRange() = default;
    
    SourceRange(const SourceLocation& s, const SourceLocation& e)
        : start(s), end(e) {}
    
    bool isValid() const {
        return start.isValid() && end.isValid();
    }
    
    std::string toString() const {
        return start.toString() + " to " + end.toString();
    }
};

} // namespace Errors
} // namespace Sad
```

**الوقت المتوقع**: 1 ساعة  
**الاختبارات**: اختبارات للدوال toString و isValid

---

### المهمة 1.3: إنشاء ملف diagnostic.h ✅

**الوصف**: فئة Diagnostic الكاملة

**الملف**: `include/errors/diagnostic.h`

```cpp
/**
 * @file diagnostic.h
 * @brief (AR) نظام التشخيص الكامل للأخطاء
 *        (EN) Complete diagnostic system for errors
 * 
 * @author Sad Language Development Team
 * @date November 23, 2025
 */

#pragma once

#include "error_codes.h"
#include "source_location.h"
#include <string>
#include <vector>
#include <memory>

namespace Sad {
namespace Errors {

/**
 * @enum DiagnosticSeverity
 * @brief (AR) مستوى شدة التشخيص
 *        (EN) Diagnostic severity level
 */
enum class DiagnosticSeverity {
    ERROR,      ///< (AR) خطأ - يمنع التنفيذ / (EN) Error - prevents execution
    WARNING,    ///< (AR) تحذير - مشكلة محتملة / (EN) Warning - potential issue
    INFO,       ///< (AR) معلومة - للتوضيح / (EN) Info - for clarification
    NOTE,       ///< (AR) ملاحظة - سياق إضافي / (EN) Note - additional context
    HINT        ///< (AR) تلميح - اقتراح / (EN) Hint - suggestion
};

/**
 * @brief (AR) يحول مستوى الشدة إلى نص
 *        (EN) Converts severity to string
 */
std::string severityToString(DiagnosticSeverity severity);

/**
 * @brief (AR) يرجع رمز اللون المناسب
 *        (EN) Returns appropriate color code
 */
std::string severityToColor(DiagnosticSeverity severity);

/**
 * @struct FixItHint
 * @brief (AR) اقتراح لإصلاح الخطأ
 *        (EN) Suggestion to fix the error
 */
struct FixItHint {
    /**
     * @enum Type
     * @brief (AR) نوع الإصلاح / (EN) Fix type
     */
    enum class Type {
        INSERT,     ///< (AR) إدراج نص / (EN) Insert text
        REMOVE,     ///< (AR) حذف نص / (EN) Remove text
        REPLACE     ///< (AR) استبدال نص / (EN) Replace text
    };
    
    Type type;                      ///< (AR) نوع الإصلاح / (EN) Fix type
    SourceLocation location;         ///< (AR) موقع الإصلاح / (EN) Fix location
    std::string replacement;         ///< (AR) النص البديل / (EN) Replacement text
    std::string description_ar;      ///< (AR) وصف بالعربية / (EN) Arabic description
    std::string description_en;      ///< (AR) وصف بالإنجليزية / (EN) English description
    
    FixItHint(Type t, const SourceLocation& loc, const std::string& repl,
              const std::string& desc_ar, const std::string& desc_en)
        : type(t), location(loc), replacement(repl),
          description_ar(desc_ar), description_en(desc_en) {}
};

/**
 * @class Diagnostic
 * @brief (AR) تشخيص كامل لخطأ أو تحذير
 *        (EN) Complete diagnostic for error or warning
 */
class Diagnostic {
private:
    ErrorCode code_;                        ///< (AR) رمز الخطأ / (EN) Error code
    DiagnosticSeverity severity_;           ///< (AR) مستوى الشدة / (EN) Severity level
    SourceLocation location_;               ///< (AR) الموقع / (EN) Location
    std::string message_ar_;                ///< (AR) الرسالة بالعربية / (EN) Arabic message
    std::string message_en_;                ///< (AR) الرسالة بالإنجليزية / (EN) English message
    std::vector<FixItHint> fixits_;        ///< (AR) اقتراحات الإصلاح / (EN) Fix-it hints
    std::vector<Diagnostic> notes_;         ///< (AR) ملاحظات مرتبطة / (EN) Related notes
    
public:
    /**
     * @brief (AR) البناء
     *        (EN) Constructor
     */
    Diagnostic(ErrorCode code, DiagnosticSeverity severity,
               const SourceLocation& location,
               const std::string& message_ar,
               const std::string& message_en);
    
    // Getters
    ErrorCode getCode() const { return code_; }
    DiagnosticSeverity getSeverity() const { return severity_; }
    const SourceLocation& getLocation() const { return location_; }
    const std::string& getMessage(Language lang) const;
    const std::vector<FixItHint>& getFixIts() const { return fixits_; }
    const std::vector<Diagnostic>& getNotes() const { return notes_; }
    
    /**
     * @brief (AR) يضيف ملاحظة مرتبطة
     *        (EN) Adds related note
     */
    void addNote(const Diagnostic& note);
    
    /**
     * @brief (AR) يضيف اقتراح إصلاح
     *        (EN) Adds fix-it hint
     */
    void addFixIt(const FixItHint& fixit);
    
    /**
     * @brief (AR) ينسق التشخيص للعرض
     *        (EN) Formats diagnostic for display
     * 
     * @param lang (AR) اللغة / (EN) Language
     * @param colorize (AR) استخدام ألوان / (EN) Use colors
     * @param sourceCode (AR) الكود المصدري (اختياري) / (EN) Source code (optional)
     * @return (AR) النص المنسق / (EN) Formatted text
     */
    std::string format(Language lang, bool colorize = true,
                      const std::string& sourceCode = "") const;
    
    /**
     * @brief (AR) يحول التشخيص إلى JSON
     *        (EN) Converts diagnostic to JSON
     * 
     * @return (AR) نص JSON / (EN) JSON string
     */
    std::string toJSON() const;
};

} // namespace Errors
} // namespace Sad
```

**الوقت المتوقع**: 3 ساعات  
**الاختبارات**: اختبارات شاملة للتنسيق وJSON

---

### المهمة 1.4: إنشاء error_manager.h المحدث ✅

**الوصف**: واجهة ErrorManager الكاملة

**الملف**: `include/errors/error_manager.h`

```cpp
/**
 * @file error_manager.h
 * @brief (AR) مدير الأخطاء المركزي للغة "ص"
 *        (EN) Central error manager for Sad language
 * 
 * (AR) يوفر واجهة موحدة لإدارة جميع الأخطاء والتحذيرات في اللغة
 *      يدعم التجميع، التنسيق، وإخراج JSON للـ IDE/LSP
 * 
 * (EN) Provides unified interface for managing all errors and warnings
 *      Supports collection, formatting, and JSON output for IDE/LSP
 * 
 * @author Sad Language Development Team
 * @date November 23, 2025
 * @version 1.0
 * @see docs/architecture/design.md
 * @see docs/error_plan/00_overview_and_design.md
 */

#pragma once

#include "diagnostic.h"
#include "error_codes.h"
#include "source_location.h"
#include <vector>
#include <memory>
#include <mutex>

namespace Sad {
namespace Errors {

/**
 * @class DiagnosticSink
 * @brief (AR) مجمع التشخيصات
 *        (EN) Diagnostic collector
 */
class DiagnosticSink {
private:
    std::vector<Diagnostic> diagnostics_;
    size_t errorCount_ = 0;
    size_t warningCount_ = 0;
    
public:
    void add(const Diagnostic& diag);
    void clear();
    const std::vector<Diagnostic>& getAll() const { return diagnostics_; }
    size_t getErrorCount() const { return errorCount_; }
    size_t getWarningCount() const { return warningCount_; }
    bool hasErrors() const { return errorCount_ > 0; }
};

/**
 * @class ErrorManager
 * @brief (AR) المدير المركزي للأخطاء - Singleton
 *        (EN) Central error manager - Singleton
 * 
 * @example
 * @code
 * ErrorManager::getInstance().reportError(
 *     ErrorCode::SYN_MISSING_SEMICOLON,
 *     location,
 *     "متوقع ';' في نهاية الجملة",
 *     "Expected ';' at end of statement"
 * );
 * @endcode
 */
class ErrorManager {
private:
    DiagnosticSink sink_;
    Language defaultLang_ = Language::ARABIC;
    bool colorizeOutput_ = true;
    size_t maxErrors_ = 100;
    std::string outputFile_;
    mutable std::mutex mutex_;  // Thread safety
    
    // Singleton
    ErrorManager() = default;
    ErrorManager(const ErrorManager&) = delete;
    ErrorManager& operator=(const ErrorManager&) = delete;
    
public:
    /**
     * @brief (AR) يرجع النسخة الوحيدة (Singleton)
     *        (EN) Returns singleton instance
     */
    static ErrorManager& getInstance();
    
    // ====================================================================
    // (AR) إضافة التشخيصات / (EN) Adding Diagnostics
    // ====================================================================
    
    /**
     * @brief (AR) يبلغ عن خطأ
     *        (EN) Reports an error
     * 
     * @param code (AR) رمز الخطأ / (EN) Error code
     * @param location (AR) الموقع / (EN) Location
     * @param message_ar (AR) الرسالة بالعربية / (EN) Arabic message
     * @param message_en (AR) الرسالة بالإنجليزية / (EN) English message
     */
    void reportError(ErrorCode code,
                    const SourceLocation& location,
                    const std::string& message_ar,
                    const std::string& message_en);
    
    /**
     * @brief (AR) يبلغ عن تحذير
     *        (EN) Reports a warning
     */
    void reportWarning(ErrorCode code,
                      const SourceLocation& location,
                      const std::string& message_ar,
                      const std::string& message_en);
    
    /**
     * @brief (AR) يضيف تشخيص كامل
     *        (EN) Adds complete diagnostic
     */
    void report(const Diagnostic& diag);
    
    // ====================================================================
    // (AR) الاستعلام / (EN) Queries
    // ====================================================================
    
    size_t getErrorCount() const;
    size_t getWarningCount() const;
    bool hasErrors() const;
    const std::vector<Diagnostic>& getAllDiagnostics() const;
    
    // ====================================================================
    // (AR) العرض والإخراج / (EN) Display and Output
    // ====================================================================
    
    /**
     * @brief (AR) يطبع جميع التشخيصات
     *        (EN) Prints all diagnostics
     * 
     * @param lang (AR) اللغة / (EN) Language
     * @param colorize (AR) استخدام ألوان / (EN) Use colors
     */
    void printAll(Language lang = Language::ARABIC,
                  bool colorize = true) const;
    
    /**
     * @brief (AR) يحول جميع التشخيصات إلى JSON
     *        (EN) Converts all diagnostics to JSON
     * 
     * @return (AR) نص JSON / (EN) JSON string
     */
    std::string toJSON() const;
    
    /**
     * @brief (AR) يحفظ التشخيصات في ملف
     *        (EN) Saves diagnostics to file
     */
    void saveToFile(const std::string& filename) const;
    
    // ====================================================================
    // (AR) الإعدادات / (EN) Settings
    // ====================================================================
    
    void setLanguage(Language lang);
    void setColorize(bool enable);
    void setMaxErrors(size_t max);
    void setOutputFile(const std::string& filename);
    
    /**
     * @brief (AR) يمسح جميع التشخيصات
     *        (EN) Clears all diagnostics
     */
    void clear();
};

/**
 * @class DiagnosticBuilder
 * @brief (AR) بناء سهل للتشخيصات
 *        (EN) Easy diagnostic building
 * 
 * @example
 * @code
 * DiagnosticBuilder(ErrorCode::SYN_MISSING_SEMICOLON,
 *                   DiagnosticSeverity::ERROR,
 *                   location)
 *     .withMessage("متوقع ';'", "Expected ';'")
 *     .withFixItInsert(location, ";")
 *     .report();
 * @endcode
 */
class DiagnosticBuilder {
private:
    Diagnostic diag_;
    
public:
    DiagnosticBuilder(ErrorCode code,
                     DiagnosticSeverity severity,
                     const SourceLocation& location);
    
    DiagnosticBuilder& withMessage(const std::string& ar,
                                   const std::string& en);
    DiagnosticBuilder& withFixIt(const FixItHint& hint);
    DiagnosticBuilder& withFixItInsert(const SourceLocation& loc,
                                       const std::string& text);
    DiagnosticBuilder& withFixItReplace(const SourceLocation& loc,
                                        const std::string& text);
    DiagnosticBuilder& withNote(const Diagnostic& note);
    
    Diagnostic build();
    void report();  // Build and report immediately
};

} // namespace Errors
} // namespace Sad
```

**الوقت المتوقع**: 2 ساعة  
**الاختبارات**: اختبارات لـ Singleton وthread-safety

---

## 📊 التقدم والإنجاز / Progress Tracking

### قائمة المهام (Checklist):

- [ ] المهمة 1.1: error_codes.h (2 ساعة)
- [ ] المهمة 1.2: source_location.h (1 ساعة)
- [ ] المهمة 1.3: diagnostic.h (3 ساعات)
- [ ] المهمة 1.4: error_manager.h (2 ساعة)
- [ ] المهمة 1.5: تطبيق error_codes.cpp (2 ساعة)
- [ ] المهمة 1.6: تطبيق diagnostic.cpp (4 ساعات)
- [ ] المهمة 1.7: تطبيق error_manager.cpp (4 ساعات)
- [ ] المهمة 1.8: اختبارات الوحدات (4 ساعات)

**إجمالي الوقت المتوقع**: 22 ساعة (~3 أيام عمل)

---

## 🧪 استراتيجية الاختبار / Testing Strategy

### اختبارات الوحدات:
```cpp
// test_error_codes.cpp
TEST(ErrorCodesTest, GetCodeString) {
    EXPECT_EQ(getErrorCodeString(ErrorCode::SYN_MISSING_SEMICOLON),
              "SYN002");
}

// test_source_location.cpp
TEST(SourceLocationTest, ToString) {
    SourceLocation loc("test.s", 5, 10);
    EXPECT_EQ(loc.toString(), "test.s:5:10");
}

// test_diagnostic.cpp
TEST(DiagnosticTest, Format) {
    Diagnostic diag(ErrorCode::SYN_MISSING_SEMICOLON,
                   DiagnosticSeverity::ERROR,
                   SourceLocation("test.s", 5, 10),
                   "متوقع ';'",
                   "Expected ';'");
    std::string formatted = diag.format(Language::ENGLISH, false);
    EXPECT_TRUE(formatted.find("Expected ';'") != std::string::npos);
}
```

---

## 📈 معايير الإنجاز / Completion Criteria

### يعتبر المرحلة 1 مكتملة عندما:
- ✅ جميع ملفات الترويسة (.h) منشأة ومتكاملة
- ✅ جميع ملفات المصدر (.cpp) منفذة
- ✅ اختبارات الوحدات تمر بنجاح (> 90%)
- ✅ التعليقات ثنائية اللغة كاملة
- ✅ التوثيق API مكتمل
- ✅ Compilation بدون أخطاء أو تحذيرات

---

**التاريخ**: 23 نوفمبر 2025  
**الحالة**: جاهز للتنفيذ  
**المرحلة التالية**: المرحلة 2 - التكامل مع Lexer/Parser

</div>
