/**
 * @file multi_error.h
 * @brief (AR) نظام جمع الأخطاء المتعددة
 *        (EN) Multi-error collection system
 * 
 * (AR) المهمة: T164 - Phase 15 (US13)
 */

#pragma once

#include "error_codes.h"
#include "diagnostic.h"
#include "source_location.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                    هياكل البيانات | Data Structures
// ═══════════════════════════════════════════════════════════════════════

enum class ErrorSeverityLevel {
    HINT,       ///< تلميح
    INFO,       ///< معلومة
    WARNING,    ///< تحذير
    ERROR,      ///< خطأ
    FATAL       ///< قاتل
};

enum class ErrorCategoryType {
    LEXER,      ///< أخطاء معجمية
    PARSER,     ///< أخطاء نحوية
    SEMANTIC,   ///< أخطاء دلالية
    TYPE,       ///< أخطاء الأنواع
    RUNTIME,    ///< أخطاء وقت التشغيل
    IO,         ///< أخطاء ملفات
    INTERNAL    ///< أخطاء داخلية
};

struct RelatedInfo {
    std::string message;
    SourceLocation location;
    
    RelatedInfo() = default;
    RelatedInfo(const std::string& msg, const SourceLocation& loc)
        : message(msg), location(loc) {}
};

struct CompilerError {
    std::string code;
    std::string message;
    std::string messageEn;
    ErrorSeverityLevel severity;
    ErrorCategoryType category;
    SourceLocation location;
    std::string sourceLine;
    std::vector<RelatedInfo> related;
    std::string timestamp;
    size_t errorId;
    
    CompilerError()
        : severity(ErrorSeverityLevel::ERROR)
        , category(ErrorCategoryType::SEMANTIC)
        , errorId(0) {}
    
    std::string format(bool useArabic = true, bool showDetails = true) const;
};

// ═══════════════════════════════════════════════════════════════════════
//                   جامع الأخطاء | Error Collector
// ═══════════════════════════════════════════════════════════════════════

class MultiErrorCollector {
public:
    MultiErrorCollector();
    
    /// (AR) إضافة خطأ كامل
    void addError(CompilerError error);
    
    /// (AR) إضافة خطأ بسيط
    void add(const std::string& code,
             const std::string& message,
             const std::string& messageEn,
             ErrorSeverityLevel severity,
             ErrorCategoryType category,
             const SourceLocation& location,
             const std::string& sourceLine = "");
    
    /// (AR) إضافة من Diagnostic الحالي
    void addFromDiagnostic(const Diagnostic& diag);
    
    /// الاستعلام
    const std::vector<CompilerError>& getErrors() const;
    std::vector<CompilerError> getErrorsBySeverity(ErrorSeverityLevel severity) const;
    std::vector<CompilerError> getErrorsForFile(const std::string& filename) const;
    bool hasErrors() const;
    bool hasFatalErrors() const;
    size_t errorCount() const;
    size_t warningCount() const;
    size_t totalCount() const;
    
    /// التنسيق
    std::string formatAll(bool useArabic = true) const;
    std::string getSummary(bool useArabic = true) const;
    std::string toJSON() const;
    
    /// الإعدادات
    void setMaxErrors(size_t max);
    void setFailOnFirstError(bool fail);    
    void clear();

private:
    std::vector<CompilerError> errors_;
    std::unordered_set<std::string> seenLocations_;
    size_t nextErrorId_;
    size_t maxErrors_;
    size_t errorCount_;
    size_t warningCount_;
    size_t fatalCount_;
    bool failOnFirstError_;
    
    bool isDuplicate(const CompilerError& error) const;
    void updateCounts(ErrorSeverityLevel severity);
    std::string getCurrentTimestamp() const;
    std::string severityToString(ErrorSeverityLevel sev, bool arabic = true) const;
    std::string severityEmoji(ErrorSeverityLevel sev) const;
};

} // namespace Errors
} // namespace Sad
