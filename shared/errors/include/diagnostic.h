/**
 * @file diagnostic.h
 * @brief (AR) نظام التشخيص الكامل للأخطاء والتحذيرات
 *        (EN) Complete diagnostic system for errors and warnings
 * 
 * (AR) يوفر هذا الملف نظاماً متكاملاً لإدارة التشخيصات في لغة "ص"
 *      يتضمن مستويات شدة، رسائل ثنائية اللغة، واقتراحات إصلاح
 * 
 * (EN) This file provides integrated diagnostic management system for Sad language
 *      Includes severity levels, bilingual messages, and fix-it suggestions
 * 
 * @author Sad Language Development Team
 * @date November 23, 2025
 * @version 1.0
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
 * 
 * (AR) يحدد مدى خطورة المشكلة من خطأ قاتل إلى تلميح
 * (EN) Defines severity from fatal error to hint
 */
enum class DiagnosticSeverity {
    ERROR,      ///< (AR) خطأ - يمنع التنفيذ / (EN) Error - prevents execution
    WARNING,    ///< (AR) تحذير - مشكلة محتملة / (EN) Warning - potential issue
    INFO,       ///< (AR) معلومة - للتوضيح فقط / (EN) Info - for clarification only
    NOTE,       ///< (AR) ملاحظة - سياق إضافي / (EN) Note - additional context
    HINT        ///< (AR) تلميح - اقتراح للتحسين / (EN) Hint - suggestion for improvement
};

/**
 * @brief (AR) يحول مستوى الشدة إلى نص
 *        (EN) Converts severity to string
 * 
 * @param severity (AR) مستوى الشدة / (EN) Severity level
 * @return (AR) النص المقابل / (EN) Corresponding string
 */
std::string severityToString(DiagnosticSeverity severity);

/**
 * @brief (AR) يرجع رمز اللون المناسب لمستوى الشدة
 *        (EN) Returns appropriate color code for severity
 * 
 * (AR) يُرجع رمز ANSI للون المناسب (أحمر للأخطاء، أصفر للتحذيرات، إلخ)
 * (EN) Returns ANSI color code (red for errors, yellow for warnings, etc.)
 * 
 * @param severity (AR) مستوى الشدة / (EN) Severity level
 * @return (AR) رمز اللون ANSI / (EN) ANSI color code
 */
std::string severityToColor(DiagnosticSeverity severity);

/**
 * @struct FixItHint
 * @brief (AR) اقتراح لإصلاح الخطأ
 *        (EN) Suggestion to fix the error
 * 
 * (AR) يمثل اقتراحاً محدداً لإصلاح الخطأ مع الموقع والنص البديل
 * (EN) Represents specific suggestion to fix error with location and replacement text
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
    
    /**
     * @brief (AR) البناء
     *        (EN) Constructor
     * 
     * @param t (AR) نوع الإصلاح / (EN) Fix type
     * @param loc (AR) موقع الإصلاح / (EN) Fix location
     * @param repl (AR) النص البديل / (EN) Replacement text
     * @param desc_ar (AR) الوصف بالعربية / (EN) Arabic description
     * @param desc_en (AR) الوصف بالإنجليزية / (EN) English description
     */
    FixItHint(Type t, const SourceLocation& loc, const std::string& repl,
              const std::string& desc_ar, const std::string& desc_en)
        : type(t), location(loc), replacement(repl),
          description_ar(desc_ar), description_en(desc_en) {}
    
    /**
     * @brief (AR) البناء الافتراضي
     *        (EN) Default constructor
     */
    FixItHint() : type(Type::INSERT) {}
};

/**
 * @class Diagnostic
 * @brief (AR) تشخيص كامل لخطأ أو تحذير
 *        (EN) Complete diagnostic for error or warning
 * 
 * (AR) يمثل تشخيصاً كاملاً يتضمن:
 *      - رمز الخطأ
 *      - مستوى الشدة
 *      - الموقع في الكود
 *      - رسائل ثنائية اللغة
 *      - اقتراحات إصلاح
 *      - ملاحظات مرتبطة
 * 
 * (EN) Represents complete diagnostic including:
 *      - Error code
 *      - Severity level
 *      - Location in code
 *      - Bilingual messages
 *      - Fix-it suggestions
 *      - Related notes
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
     * 
     * (AR) ينشئ تشخيصاً جديداً مع جميع المعلومات الأساسية
     * (EN) Creates new diagnostic with all basic information
     * 
     * @param code (AR) رمز الخطأ / (EN) Error code
     * @param severity (AR) مستوى الشدة / (EN) Severity level
     * @param location (AR) الموقع / (EN) Location
     * @param message_ar (AR) الرسالة بالعربية / (EN) Arabic message
     * @param message_en (AR) الرسالة بالإنجليزية / (EN) English message
     */
    Diagnostic(ErrorCode code, DiagnosticSeverity severity,
               const SourceLocation& location,
               const std::string& message_ar,
               const std::string& message_en);
    
    /**
     * @brief (AR) البناء الافتراضي
     *        (EN) Default constructor
     */
    Diagnostic();
    
    // ====================================================================
    // (AR) الوصول للبيانات / (EN) Data Access
    // ====================================================================
    
    /**
     * @brief (AR) يرجع رمز الخطأ
     *        (EN) Returns error code
     */
    ErrorCode getCode() const { return code_; }
    
    /**
     * @brief (AR) يرجع مستوى الشدة
     *        (EN) Returns severity level
     */
    DiagnosticSeverity getSeverity() const { return severity_; }
    
    /**
     * @brief (AR) يرجع الموقع
     *        (EN) Returns location
     */
    const SourceLocation& getLocation() const { return location_; }
    
    /**
     * @brief (AR) يرجع الرسالة باللغة المحددة
     *        (EN) Returns message in specified language
     * 
     * @param lang (AR) اللغة / (EN) Language
     * @return (AR) الرسالة / (EN) Message
     */
    const std::string& getMessage(Language lang) const;
    
    /**
     * @brief (AR) يرجع جميع اقتراحات الإصلاح
     *        (EN) Returns all fix-it hints
     */
    const std::vector<FixItHint>& getFixIts() const { return fixits_; }
    
    /**
     * @brief (AR) يرجع جميع الملاحظات المرتبطة
     *        (EN) Returns all related notes
     */
    const std::vector<Diagnostic>& getNotes() const { return notes_; }
    
    // ====================================================================
    // (AR) إضافة معلومات إضافية / (EN) Adding Additional Information
    // ====================================================================
    
    /**
     * @brief (AR) يضيف ملاحظة مرتبطة
     *        (EN) Adds related note
     * 
     * (AR) تُستخدم الملاحظات لتوفير سياق إضافي (مثل موقع التعريف الأصلي)
     * (EN) Notes are used to provide additional context (like original definition location)
     * 
     * @param note (AR) الملاحظة / (EN) Note
     */
    void addNote(const Diagnostic& note);
    
    /**
     * @brief (AR) يضيف اقتراح إصلاح
     *        (EN) Adds fix-it hint
     * 
     * (AR) يضيف اقتراحاً لكيفية إصلاح الخطأ
     * (EN) Adds suggestion for how to fix the error
     * 
     * @param fixit (AR) اقتراح الإصلاح / (EN) Fix-it hint
     */
    void addFixIt(const FixItHint& fixit);
    
    // ====================================================================
    // (AR) التنسيق والإخراج / (EN) Formatting and Output
    // ====================================================================
    
    /**
     * @brief (AR) ينسق التشخيص للعرض
     *        (EN) Formats diagnostic for display
     * 
     * (AR) ينسق التشخيص بشكل جميل مع ألوان وأسهم وأرقام أسطر
     * (EN) Formats diagnostic beautifully with colors, arrows, and line numbers
     * 
     * @param lang (AR) اللغة / (EN) Language
     * @param colorize (AR) استخدام ألوان / (EN) Use colors
     * @param sourceCode (AR) الكود المصدري (اختياري) / (EN) Source code (optional)
     * @return (AR) النص المنسق / (EN) Formatted text
     * 
     * @example
     * @code
     * std::string formatted = diag.format(Language::ARABIC, true, sourceCode);
     * std::cout << formatted << std::endl;
     * @endcode
     */
    std::string format(Language lang, bool colorize = true,
                      const std::string& sourceCode = "") const;
    
    /**
     * @brief (AR) يحول التشخيص إلى JSON
     *        (EN) Converts diagnostic to JSON
     * 
     * (AR) يحول التشخيص إلى صيغة JSON متوافقة مع LSP للاستخدام في IDEs
     * (EN) Converts diagnostic to LSP-compatible JSON format for use in IDEs
     * 
     * @return (AR) نص JSON / (EN) JSON string
     * 
     * @example
     * @code
     * std::string json = diag.toJSON();
     * std::ofstream file("diagnostics.json");
     * file << json;
     * @endcode
     */
    std::string toJSON() const;
};

} // namespace Errors
} // namespace Sad
