/*
 * ============================================================================
 * Type Errors - نظام إدارة أخطاء الأنواع
 * Type Errors - Type Error Management System
 * ============================================================================
 * 
 * الوصف (Description):
 *   نظام شامل لإدارة وإبلاغ أخطاء الأنواع مع رسائل واضحة بالعربية
 *   ودعم لتتبع الموقع في الكود واقتراحات التصحيح.
 * 
 *   Comprehensive system for managing and reporting type errors with clear
 *   Arabic messages, source location tracking, and fix suggestions.
 * 
 * المكونات الرئيسية (Main Components):
 *   - TypeError: تمثيل خطأ نوع واحد / Single type error representation
 *   - TypeErrorBuilder: بناء أخطاء معقدة / Builder for complex errors
 *   - TypeErrorReporter: مدير الأخطاء الرئيسي / Main error manager
 *   - FixSuggestion: اقتراحات التصحيح / Fix suggestions
 * 
 * الميزات (Features):
 *   1. رسائل خطأ واضحة بالعربية والإنجليزية
 *   2. تتبع دقيق للموقع في الكود المصدري
 *   3. اقتراحات تلقائية لتصحيح الأخطاء
 *   4. دعم للأخطاء متعددة الأسطر والملفات
 *   5. تصنيف الأخطاء حسب الخطورة
 *   6. دعم للألوان في terminal output
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 31, 2025
 * الإصدار (Version): 1.0.0
 * ============================================================================
 */

#ifndef SAD_TYPE_ERRORS_H
#define SAD_TYPE_ERRORS_H

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "../../../../include/lexer/token.h"
#include "typed_ast.h"

namespace Sad {
namespace TypeChecker {

// تصاعد الخطورة / Severity levels
enum class Severity {
    ERROR,      // خطأ فادح / Fatal error
    WARNING,    // تحذير / Warning
    NOTE,       // ملاحظة / Note
    HINT        // إشارة / Hint
};

// ============================================================================
// FixSuggestion - اقتراح تصحيح
// ============================================================================

/**
 * @brief اقتراح لتصحيح خطأ / Fix suggestion for an error
 * 
 * يحتوي على رسالة الاقتراح وموقع التطبيق والكود البديل
 * Contains suggestion message, application location, and replacement code
 */
class FixSuggestion {
public:
    std::string message;              ///< رسالة الاقتراح / Suggestion message
    Lexer::Position location;         ///< موقع التطبيق / Application location
    std::optional<std::string> replacement; ///< الكود البديل / Replacement code
    
    /**
     * @brief البناء الأساسي / Basic constructor
     */
    FixSuggestion(const std::string& msg, const Lexer::Position& loc)
        : message(msg), location(loc) {}
    
    /**
     * @brief البناء مع كود بديل / Constructor with replacement
     */
    FixSuggestion(const std::string& msg, const Lexer::Position& loc, 
                  const std::string& repl)
        : message(msg), location(loc), replacement(repl) {}
};

// ============================================================================
// TypeError - خطأ نوع واحد
// ============================================================================

/**
 * @brief تمثيل خطأ نوع واحد / Representation of a single type error
 * 
 * يحتوي على كل المعلومات اللازمة لعرض خطأ بشكل واضح ومفيد
 * Contains all information needed to display an error clearly and helpfully
 */
class TypeError {
public:
    // ========================================================================
    // البيانات الأساسية / Core Data
    // ========================================================================
    
    Severity severity;                ///< مستوى الخطورة / Severity level
    std::string code;                 ///< رمز الخطأ (مثل E001) / Error code (e.g., E001)
    std::string messageAr;            ///< الرسالة بالعربية / Message in Arabic
    std::string messageEn;            ///< الرسالة بالإنجليزية / Message in English
    Lexer::Position primaryLocation;  ///< الموقع الرئيسي / Primary location
    
    // ========================================================================
    // المعلومات الإضافية / Additional Information
    // ========================================================================
    
    std::vector<Lexer::Position> secondaryLocations; ///< مواقع ثانوية / Secondary locations
    std::vector<FixSuggestion> suggestions;          ///< اقتراحات التصحيح / Fix suggestions
    std::optional<std::string> notes;                ///< ملاحظات إضافية / Additional notes
    
    // أنواع متعلقة بالخطأ / Related types
    std::shared_ptr<Type> expectedType;  ///< النوع المتوقع / Expected type
    std::shared_ptr<Type> actualType;    ///< النوع الفعلي / Actual type
    
    /**
     * @brief البناء الأساسي / Basic constructor
     * @param sev مستوى الخطورة / Severity level
     * @param errorCode رمز الخطأ / Error code
     * @param msgAr الرسالة بالعربية / Arabic message
     * @param msgEn الرسالة بالإنجليزية / English message
     * @param loc الموقع / Location
     */
    TypeError(Severity sev, const std::string& errorCode,
              const std::string& msgAr, const std::string& msgEn,
              const Lexer::Position& loc)
        : severity(sev), code(errorCode), messageAr(msgAr), 
          messageEn(msgEn), primaryLocation(loc) {}
    
    /**
     * @brief إضافة موقع ثانوي / Add secondary location
     * @param loc الموقع / Location
     */
    void addSecondaryLocation(const Lexer::Position& loc) {
        secondaryLocations.push_back(loc);
    }
    
    /**
     * @brief إضافة اقتراح تصحيح / Add fix suggestion
     * @param suggestion الاقتراح / Suggestion
     */
    void addSuggestion(const FixSuggestion& suggestion) {
        suggestions.push_back(suggestion);
    }
    
    /**
     * @brief إضافة ملاحظة / Add note
     * @param note الملاحظة / Note
     */
    void addNote(const std::string& note) {
        if (notes.has_value()) {
            notes.value() += "\n" + note;
        } else {
            notes = note;
        }
    }
    
    /**
     * @brief تعيين الأنواع المتعلقة / Set related types
     * @param expected النوع المتوقع / Expected type
     * @param actual النوع الفعلي / Actual type
     */
    void setTypes(std::shared_ptr<Type> expected, std::shared_ptr<Type> actual) {
        expectedType = expected;
        actualType = actual;
    }
};

// ============================================================================
// TypeErrorBuilder - بناء أخطاء معقدة
// ============================================================================

/**
 * @brief Builder pattern لبناء أخطاء معقدة / Builder for complex errors
 * 
 * يسمح ببناء أخطاء خطوة بخطوة بطريقة سلسة
 * Allows building errors step by step in a fluent manner
 * 
 * @example
 * @code{.cpp}
 * auto error = TypeErrorBuilder::error("E001")
 *     .messageAr("تعارض في الأنواع")
 *     .messageEn("Type mismatch")
 *     .location(expr->position)
 *     .expectedType(intType)
 *     .actualType(stringType)
 *     .addSuggestion("جرب تحويل النوع باستخدام int()", location)
 *     .build();
 * @endcode
 */
class TypeErrorBuilder {
private:
    TypeError error_; ///< الخطأ قيد البناء / Error being built
    
    /**
     * @brief البناء الخاص / Private constructor
     */
    TypeErrorBuilder(Severity sev, const std::string& code)
        : error_(sev, code, "", "", Lexer::Position()) {}

public:
    /**
     * @brief إنشاء builder لخطأ / Create error builder
     * @param code رمز الخطأ / Error code
     * @return Builder instance
     */
    static TypeErrorBuilder error(const std::string& code) {
        return TypeErrorBuilder(Severity::ERROR, code);
    }
    
    /**
     * @brief إنشاء builder لتحذير / Create warning builder
     * @param code رمز التحذير / Warning code
     * @return Builder instance
     */
    static TypeErrorBuilder warning(const std::string& code) {
        return TypeErrorBuilder(Severity::WARNING, code);
    }
    
    /**
     * @brief تعيين الرسالة بالعربية / Set Arabic message
     */
    TypeErrorBuilder& messageAr(const std::string& msg) {
        error_.messageAr = msg;
        return *this;
    }
    
    /**
     * @brief تعيين الرسالة بالإنجليزية / Set English message
     */
    TypeErrorBuilder& messageEn(const std::string& msg) {
        error_.messageEn = msg;
        return *this;
    }
    
    /**
     * @brief تعيين الموقع الرئيسي / Set primary location
     */
    TypeErrorBuilder& location(const Lexer::Position& loc) {
        error_.primaryLocation = loc;
        return *this;
    }
    
    /**
     * @brief إضافة موقع ثانوي / Add secondary location
     */
    TypeErrorBuilder& secondaryLocation(const Lexer::Position& loc) {
        error_.addSecondaryLocation(loc);
        return *this;
    }
    
    /**
     * @brief تعيين النوع المتوقع / Set expected type
     */
    TypeErrorBuilder& expectedType(std::shared_ptr<Type> type) {
        error_.expectedType = type;
        return *this;
    }
    
    /**
     * @brief تعيين النوع الفعلي / Set actual type
     */
    TypeErrorBuilder& actualType(std::shared_ptr<Type> type) {
        error_.actualType = type;
        return *this;
    }
    
    /**
     * @brief إضافة اقتراح / Add suggestion
     */
    TypeErrorBuilder& addSuggestion(const std::string& msg, const Lexer::Position& loc) {
        error_.addSuggestion(FixSuggestion(msg, loc));
        return *this;
    }
    
    /**
     * @brief إضافة اقتراح مع كود بديل / Add suggestion with replacement
     */
    TypeErrorBuilder& addSuggestion(const std::string& msg, const Lexer::Position& loc,
                                    const std::string& replacement) {
        error_.addSuggestion(FixSuggestion(msg, loc, replacement));
        return *this;
    }
    
    /**
     * @brief إضافة ملاحظة / Add note
     */
    TypeErrorBuilder& addNote(const std::string& note) {
        error_.addNote(note);
        return *this;
    }
    
    /**
     * @brief بناء الخطأ النهائي / Build final error
     */
    TypeError build() const {
        return error_;
    }
};

// ============================================================================
// TypeErrorReporter - مدير الأخطاء الرئيسي
// ============================================================================

/**
 * @brief مدير الأخطاء الرئيسي / Main error manager
 * 
 * يدير جميع الأخطاء والتحذيرات ويوفر واجهة لطباعتها وإدارتها
 * Manages all errors and warnings and provides interface for printing and managing
 */
class TypeErrorReporter {
private:
    std::vector<TypeError> errors_;      ///< قائمة الأخطاء / Error list
    std::vector<TypeError> warnings_;    ///< قائمة التحذيرات / Warning list
    bool useColors_;                     ///< استخدام الألوان / Use colors
    bool showSuggestions_;               ///< عرض الاقتراحات / Show suggestions
    std::string currentFile_;            ///< الملف الحالي / Current file
    std::vector<std::string> sourceLines_; ///< أسطر الكود المصدري / Source code lines

public:
    /**
     * @brief البناء الافتراضي / Default constructor
     */
    TypeErrorReporter()
        : useColors_(true), showSuggestions_(true) {}
    
    // ========================================================================
    // إدارة الأخطاء / Error Management
    // ========================================================================
    
    /**
     * @brief إضافة خطأ / Add error
     * @param error الخطأ / Error
     */
    void addError(const TypeError& error) {
        if (error.severity == Severity::ERROR) {
            errors_.push_back(error);
        } else if (error.severity == Severity::WARNING) {
            warnings_.push_back(error);
        }
    }
    
    /**
     * @brief إضافة خطأ بسيط / Add simple error
     * @param code رمز الخطأ / Error code
     * @param messageAr الرسالة بالعربية / Arabic message
     * @param messageEn الرسالة بالإنجليزية / English message
     * @param location الموقع / Location
     */
    void addError(const std::string& code, const std::string& messageAr,
                  const std::string& messageEn, const Lexer::Position& location) {
        errors_.push_back(TypeError(Severity::ERROR, code, messageAr, messageEn, location));
    }
    
    /**
     * @brief إضافة تحذير / Add warning
     * @param code رمز التحذير / Warning code
     * @param messageAr الرسالة بالعربية / Arabic message
     * @param messageEn الرسالة بالإنجليزية / English message
     * @param location الموقع / Location
     */
    void addWarning(const std::string& code, const std::string& messageAr,
                    const std::string& messageEn, const Lexer::Position& location) {
        warnings_.push_back(TypeError(Severity::WARNING, code, messageAr, messageEn, location));
    }
    
    // ========================================================================
    // الحصول على المعلومات / Getting Information
    // ========================================================================
    
    /**
     * @brief هل توجد أخطاء؟ / Has errors?
     */
    bool hasErrors() const {
        return !errors_.empty();
    }
    
    /**
     * @brief هل توجد تحذيرات؟ / Has warnings?
     */
    bool hasWarnings() const {
        return !warnings_.empty();
    }
    
    /**
     * @brief عدد الأخطاء / Error count
     */
    size_t getErrorCount() const {
        return errors_.size();
    }
    
    /**
     * @brief عدد التحذيرات / Warning count
     */
    size_t getWarningCount() const {
        return warnings_.size();
    }
    
    /**
     * @brief الحصول على جميع الأخطاء / Get all errors
     */
    const std::vector<TypeError>& getErrors() const {
        return errors_;
    }
    
    /**
     * @brief الحصول على جميع التحذيرات / Get all warnings
     */
    const std::vector<TypeError>& getWarnings() const {
        return warnings_;
    }
    
    // ========================================================================
    // الإعدادات / Settings
    // ========================================================================
    
    /**
     * @brief تعيين استخدام الألوان / Set color usage
     */
    void setUseColors(bool useColors) {
        useColors_ = useColors;
    }
    
    /**
     * @brief تعيين عرض الاقتراحات / Set show suggestions
     */
    void setShowSuggestions(bool show) {
        showSuggestions_ = show;
    }
    
    /**
     * @brief تعيين الملف الحالي / Set current file
     */
    void setCurrentFile(const std::string& file) {
        currentFile_ = file;
    }
    
    /**
     * @brief تعيين أسطر الكود المصدري / Set source lines
     */
    void setSourceLines(const std::vector<std::string>& lines) {
        sourceLines_ = lines;
    }
    
    // ========================================================================
    // الطباعة / Printing
    // ========================================================================
    
    /**
     * @brief طباعة جميع الأخطاء / Print all errors
     */
    void printAllErrors() const;
    
    /**
     * @brief طباعة جميع التحذيرات / Print all warnings
     */
    void printAllWarnings() const;
    
    /**
     * @brief طباعة خطأ واحد / Print single error
     */
    void printError(const TypeError& error) const;
    
    /**
     * @brief طباعة ملخص / Print summary
     */
    void printSummary() const;
    
    // ========================================================================
    // إعادة التعيين / Reset
    // ========================================================================
    
    /**
     * @brief مسح جميع الأخطاء والتحذيرات / Clear all errors and warnings
     */
    void clear() {
        errors_.clear();
        warnings_.clear();
    }

private:
    /**
     * @brief الحصول على رمز اللون / Get color code
     */
    std::string getColorCode(Severity severity) const;
    
    /**
     * @brief إعادة تعيين اللون / Reset color
     */
    std::string resetColor() const;
    
    /**
     * @brief طباعة سطر من الكود / Print code line
     */
    void printCodeLine(const Lexer::Position& location) const;
    
    /**
     * @brief طباعة مؤشر للموقع / Print location pointer
     */
    void printLocationPointer(const Lexer::Position& location) const;
    
    /**
     * @brief طباعة الاقتراحات / Print suggestions
     */
    void printSuggestions(const std::vector<FixSuggestion>& suggestions) const;
};

} // namespace TypeChecker
} // namespace Sad

#endif // SAD_TYPE_ERRORS_H
