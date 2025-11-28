/**
 * @file error_manager.h
 * @brief (AR) مدير الأخطاء المركزي للغة "ص"
 *        (EN) Central error manager for Sad language
 * 
 * (AR) يوفر واجهة موحدة لإدارة جميع الأخطاء والتحذيرات في اللغة
 *      يدعم التجميع، التنسيق، وإخراج JSON للـ IDE/LSP
 *      نمط Singleton لضمان نسخة واحدة عالمية
 * 
 * (EN) Provides unified interface for managing all errors and warnings
 *      Supports collection, formatting, and JSON output for IDE/LSP
 *      Singleton pattern ensures single global instance
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
 * 
 * (AR) يجمع ويحفظ جميع التشخيصات ويوفر إحصائيات عنها
 * (EN) Collects and stores all diagnostics and provides statistics
 */
class DiagnosticSink {
private:
    std::vector<Diagnostic> diagnostics_;  ///< (AR) جميع التشخيصات / (EN) All diagnostics
    size_t errorCount_ = 0;                ///< (AR) عدد الأخطاء / (EN) Error count
    size_t warningCount_ = 0;              ///< (AR) عدد التحذيرات / (EN) Warning count
    
public:
    /**
     * @brief (AR) يضيف تشخيصاً جديداً
     *        (EN) Adds new diagnostic
     * 
     * @param diag (AR) التشخيص / (EN) Diagnostic
     */
    void add(const Diagnostic& diag);
    
    /**
     * @brief (AR) يمسح جميع التشخيصات
     *        (EN) Clears all diagnostics
     */
    void clear();
    
    /**
     * @brief (AR) يرجع جميع التشخيصات
     *        (EN) Returns all diagnostics
     */
    const std::vector<Diagnostic>& getAll() const { return diagnostics_; }
    
    /**
     * @brief (AR) يرجع عدد الأخطاء
     *        (EN) Returns error count
     */
    size_t getErrorCount() const { return errorCount_; }
    
    /**
     * @brief (AR) يرجع عدد التحذيرات
     *        (EN) Returns warning count
     */
    size_t getWarningCount() const { return warningCount_; }
    
    /**
     * @brief (AR) يتحقق من وجود أخطاء
     *        (EN) Checks if errors exist
     */
    bool hasErrors() const { return errorCount_ > 0; }
};

/**
 * @class ErrorManager
 * @brief (AR) المدير المركزي للأخطاء - Singleton
 *        (EN) Central error manager - Singleton
 * 
 * (AR) يدير جميع الأخطاء والتحذيرات في اللغة بشكل مركزي
 *      يوفر واجهة موحدة للإبلاغ عن الأخطاء والعرض والتصدير
 *      آمن للاستخدام من خيوط متعددة (Thread-safe)
 * 
 * (EN) Centrally manages all errors and warnings in the language
 *      Provides unified interface for reporting, displaying, and exporting errors
 *      Thread-safe for use from multiple threads
 * 
 * @example
 * @code
 * // (AR) الإبلاغ عن خطأ / (EN) Report an error
 * ErrorManager::getInstance().reportError(
 *     ErrorCode::SYN_MISSING_SEMICOLON,
 *     SourceLocation("test.s", 5, 10),
 *     "متوقع ';' في نهاية الجملة",
 *     "Expected ';' at end of statement"
 * );
 * 
 * // (AR) عرض جميع الأخطاء / (EN) Display all errors
 * if (ErrorManager::getInstance().hasErrors()) {
 *     ErrorManager::getInstance().printAll(Language::ARABIC);
 * }
 * @endcode
 */
class ErrorManager {
private:
    DiagnosticSink sink_;                    ///< (AR) مجمع التشخيصات / (EN) Diagnostic sink
    Language defaultLang_ = Language::ARABIC; ///< (AR) اللغة الافتراضية / (EN) Default language
    bool colorizeOutput_ = true;             ///< (AR) استخدام ألوان / (EN) Use colors
    size_t maxErrors_ = 100;                 ///< (AR) الحد الأقصى للأخطاء / (EN) Maximum errors
    std::string outputFile_;                 ///< (AR) ملف الإخراج / (EN) Output file
    mutable std::mutex mutex_;               ///< (AR) للأمان من التزامن / (EN) For thread safety
    
    // (AR) Singleton - منع النسخ والإنشاء الخارجي
    // (EN) Singleton - prevent copying and external creation
    ErrorManager() = default;
    ErrorManager(const ErrorManager&) = delete;
    ErrorManager& operator=(const ErrorManager&) = delete;
    
public:
    /**
     * @brief (AR) يرجع النسخة الوحيدة (Singleton)
     *        (EN) Returns singleton instance
     * 
     * (AR) يضمن وجود نسخة واحدة فقط من ErrorManager في البرنامج
     * (EN) Ensures only one instance of ErrorManager exists in program
     * 
     * @return (AR) مرجع للنسخة الوحيدة / (EN) Reference to singleton instance
     */
    static ErrorManager& getInstance();
    
    // ====================================================================
    // (AR) إضافة التشخيصات / (EN) Adding Diagnostics
    // ====================================================================
    
    /**
     * @brief (AR) يبلغ عن خطأ
     *        (EN) Reports an error
     * 
     * (AR) يضيف خطأ جديداً إلى قائمة التشخيصات
     * (EN) Adds new error to diagnostics list
     * 
     * @param code (AR) رمز الخطأ / (EN) Error code
     * @param location (AR) الموقع في الكود / (EN) Location in code
     * @param message_ar (AR) الرسالة بالعربية / (EN) Arabic message
     * @param message_en (AR) الرسالة بالإنجليزية / (EN) English message
     * 
     * @example
     * @code
     * ErrorManager::getInstance().reportError(
     *     ErrorCode::LEX_INVALID_CHARACTER,
     *     SourceLocation("test.s", 1, 5),
     *     "رمز غير صالح '@'",
     *     "Invalid character '@'"
     * );
     * @endcode
     */
    void reportError(ErrorCode code,
                    const SourceLocation& location,
                    const std::string& message_ar,
                    const std::string& message_en);
    
    /**
     * @brief (AR) يبلغ عن تحذير
     *        (EN) Reports a warning
     * 
     * (AR) يضيف تحذيراً جديداً إلى قائمة التشخيصات
     * (EN) Adds new warning to diagnostics list
     * 
     * @param code (AR) رمز التحذير / (EN) Warning code
     * @param location (AR) الموقع في الكود / (EN) Location in code
     * @param message_ar (AR) الرسالة بالعربية / (EN) Arabic message
     * @param message_en (AR) الرسالة بالإنجليزية / (EN) English message
     */
    void reportWarning(ErrorCode code,
                      const SourceLocation& location,
                      const std::string& message_ar,
                      const std::string& message_en);
    
    /**
     * @brief (AR) يضيف تشخيص كامل
     *        (EN) Adds complete diagnostic
     * 
     * (AR) يضيف تشخيصاً كاملاً (قد يحتوي على fix-its وملاحظات)
     * (EN) Adds complete diagnostic (may contain fix-its and notes)
     * 
     * @param diag (AR) التشخيص / (EN) Diagnostic
     */
    void report(const Diagnostic& diag);
    
    // ====================================================================
    // (AR) الاستعلام / (EN) Queries
    // ====================================================================
    
    /**
     * @brief (AR) يرجع عدد الأخطاء
     *        (EN) Returns error count
     */
    size_t getErrorCount() const;
    
    /**
     * @brief (AR) يرجع عدد التحذيرات
     *        (EN) Returns warning count
     */
    size_t getWarningCount() const;
    
    /**
     * @brief (AR) يتحقق من وجود أخطاء
     *        (EN) Checks if errors exist
     */
    bool hasErrors() const;
    
    /**
     * @brief (AR) يرجع جميع التشخيصات
     *        (EN) Returns all diagnostics
     */
    const std::vector<Diagnostic>& getAllDiagnostics() const;
    
    // ====================================================================
    // (AR) العرض والإخراج / (EN) Display and Output
    // ====================================================================
    
    /**
     * @brief (AR) يطبع جميع التشخيصات
     *        (EN) Prints all diagnostics
     * 
     * (AR) يعرض جميع التشخيصات بتنسيق جميل في الطرفية
     * (EN) Displays all diagnostics with beautiful formatting in terminal
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
     * (AR) يصدر جميع التشخيصات بصيغة JSON للاستخدام في IDEs
     * (EN) Exports all diagnostics in JSON format for use in IDEs
     * 
     * @return (AR) نص JSON / (EN) JSON string
     */
    std::string toJSON() const;
    
    /**
     * @brief (AR) يحفظ التشخيصات في ملف
     *        (EN) Saves diagnostics to file
     * 
     * @param filename (AR) اسم الملف / (EN) File name
     */
    void saveToFile(const std::string& filename) const;
    
    // ====================================================================
    // (AR) الإعدادات / (EN) Settings
    // ====================================================================
    
    /**
     * @brief (AR) يحدد اللغة الافتراضية
     *        (EN) Sets default language
     */
    void setLanguage(Language lang);
    
    /**
     * @brief (AR) يحدد استخدام الألوان
     *        (EN) Sets colorize option
     */
    void setColorize(bool enable);
    
    /**
     * @brief (AR) يحدد الحد الأقصى للأخطاء
     *        (EN) Sets maximum errors
     */
    void setMaxErrors(size_t max);
    
    /**
     * @brief (AR) يحدد ملف الإخراج
     *        (EN) Sets output file
     */
    void setOutputFile(const std::string& filename);
    
    /**
     * @brief (AR) يرجع ملف الإخراج
     *        (EN) Returns output file
     */
    const std::string& getOutputFile() const { return outputFile_; }
    
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
 * (AR) يوفر واجهة سلسة (fluent interface) لبناء تشخيصات معقدة
 *      يسمح بإضافة fix-its وملاحظات بسهولة
 * 
 * (EN) Provides fluent interface for building complex diagnostics
 *      Allows easy addition of fix-its and notes
 * 
 * @example
 * @code
 * DiagnosticBuilder(ErrorCode::SYN_MISSING_SEMICOLON,
 *                   DiagnosticSeverity::ERROR,
 *                   SourceLocation("test.s", 5, 10))
 *     .withMessage("متوقع ';'", "Expected ';'")
 *     .withFixItInsert(SourceLocation("test.s", 5, 11), ";")
 *     .report();
 * @endcode
 */
class DiagnosticBuilder {
private:
    Diagnostic diag_;  ///< (AR) التشخيص قيد البناء / (EN) Diagnostic being built
    
public:
    /**
     * @brief (AR) البناء
     *        (EN) Constructor
     * 
     * @param code (AR) رمز الخطأ / (EN) Error code
     * @param severity (AR) مستوى الشدة / (EN) Severity level
     * @param location (AR) الموقع / (EN) Location
     */
    DiagnosticBuilder(ErrorCode code,
                     DiagnosticSeverity severity,
                     const SourceLocation& location);
    
    /**
     * @brief (AR) يضيف الرسالة
     *        (EN) Adds message
     * 
     * @param ar (AR) الرسالة بالعربية / (EN) Arabic message
     * @param en (AR) الرسالة بالإنجليزية / (EN) English message
     * @return (AR) مرجع لنفسه للتسلسل / (EN) Reference to self for chaining
     */
    DiagnosticBuilder& withMessage(const std::string& ar,
                                   const std::string& en);
    
    /**
     * @brief (AR) يضيف اقتراح إصلاح
     *        (EN) Adds fix-it hint
     */
    DiagnosticBuilder& withFixIt(const FixItHint& hint);
    
    /**
     * @brief (AR) يضيف اقتراح إدراج
     *        (EN) Adds insert fix-it
     */
    DiagnosticBuilder& withFixItInsert(const SourceLocation& loc,
                                       const std::string& text);
    
    /**
     * @brief (AR) يضيف اقتراح استبدال
     *        (EN) Adds replace fix-it
     */
    DiagnosticBuilder& withFixItReplace(const SourceLocation& loc,
                                        const std::string& text);
    
    /**
     * @brief (AR) يضيف ملاحظة
     *        (EN) Adds note
     */
    DiagnosticBuilder& withNote(const Diagnostic& note);
    
    /**
     * @brief (AR) يبني التشخيص
     *        (EN) Builds diagnostic
     */
    Diagnostic build();
    
    /**
     * @brief (AR) يبني ويبلغ فوراً
     *        (EN) Builds and reports immediately
     */
    void report();
};

} // namespace Errors
} // namespace Sad
