/**
 * @file smart_multi_error.cpp
 * @brief (AR) تطبيق نظام جمع الأخطاء المتعددة - Sad::Errors namespace
 * (AR) المهمة: T164 - Phase 15 (US13)
 */

#include "multi_error.h"
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                    CompilerError::format
// ═══════════════════════════════════════════════════════════════════════

std::string CompilerError::format(bool useArabic, bool showDetails) const {
    std::ostringstream oss;
    
    // severity emoji + code + message
    std::string emoji;
    switch (severity) {
        case ErrorSeverityLevel::HINT: emoji = "\xF0\x9F\x92\xA1"; break;
        case ErrorSeverityLevel::INFO: emoji = "\xE2\x84\xB9\xEF\xB8\x8F"; break;
        case ErrorSeverityLevel::WARNING: emoji = "\xE2\x9A\xA0\xEF\xB8\x8F"; break;
        case ErrorSeverityLevel::ERROR: emoji = "\xE2\x9D\x8C"; break;
        case ErrorSeverityLevel::FATAL: emoji = "\xF0\x9F\x94\xA5"; break;
    }
    
    oss << emoji << " [" << code << "] " << (useArabic ? message : messageEn) << "\n";
    oss << "   ";
    if (!location.filename.empty()) oss << location.filename << ":";
    oss << location.line << ":" << location.column << "\n";
    
    if (showDetails && !sourceLine.empty()) {
        oss << "\n   " << location.line << " | " << sourceLine << "\n";
        oss << "     | ";
        for (size_t i = 1; i < location.column; i++) oss << " ";
        oss << "^";
        if (location.length > 1) {
            for (size_t i = 1; i < location.length; i++) oss << "~";
        }
        oss << "\n";
    }
    
    if (showDetails && !related.empty()) {
        oss << "\n";
        for (const auto& rel : related) {
            oss << "   \xF0\x9F\x93\x8E " << rel.message << "\n";
        }
    }
    
    return oss.str();
}

// ═══════════════════════════════════════════════════════════════════════
//                    MultiErrorCollector
// ═══════════════════════════════════════════════════════════════════════

MultiErrorCollector::MultiErrorCollector()
    : nextErrorId_(1), maxErrors_(100)
    , errorCount_(0), warningCount_(0), fatalCount_(0)
    , failOnFirstError_(false) {}

void MultiErrorCollector::addError(CompilerError error) {
    error.errorId = nextErrorId_++;
    error.timestamp = getCurrentTimestamp();
    
    if (isDuplicate(error)) return;
    if (errors_.size() >= maxErrors_) return;
    
    errors_.push_back(error);
    updateCounts(error.severity);
}

void MultiErrorCollector::add(
    const std::string& code, const std::string& message,
    const std::string& messageEn, ErrorSeverityLevel severity,
    ErrorCategoryType category, const SourceLocation& location,
    const std::string& sourceLine
) {
    CompilerError error;
    error.code = code;
    error.message = message;
    error.messageEn = messageEn;
    error.severity = severity;
    error.category = category;
    error.location = location;
    error.sourceLine = sourceLine;
    addError(error);
}

void MultiErrorCollector::addFromDiagnostic(const Diagnostic& diag) {
    CompilerError error;
    error.code = getErrorCodeString(diag.getCode());
    error.message = diag.getMessage(Language::ARABIC);
    error.messageEn = diag.getMessage(Language::ENGLISH);
    
    switch (diag.getSeverity()) {
        case DiagnosticSeverity::ERROR: error.severity = ErrorSeverityLevel::ERROR; break;
        case DiagnosticSeverity::WARNING: error.severity = ErrorSeverityLevel::WARNING; break;
        case DiagnosticSeverity::INFO: error.severity = ErrorSeverityLevel::INFO; break;
        case DiagnosticSeverity::HINT: error.severity = ErrorSeverityLevel::HINT; break;
        default: error.severity = ErrorSeverityLevel::INFO; break;
    }
    
    error.location = diag.getLocation();
    addError(error);
}

const std::vector<CompilerError>& MultiErrorCollector::getErrors() const { return errors_; }

std::vector<CompilerError> MultiErrorCollector::getErrorsBySeverity(ErrorSeverityLevel severity) const {
    std::vector<CompilerError> filtered;
    for (const auto& e : errors_) {
        if (e.severity == severity) filtered.push_back(e);
    }
    return filtered;
}

std::vector<CompilerError> MultiErrorCollector::getErrorsForFile(const std::string& filename) const {
    std::vector<CompilerError> filtered;
    for (const auto& e : errors_) {
        if (e.location.filename == filename) filtered.push_back(e);
    }
    return filtered;
}

bool MultiErrorCollector::hasErrors() const { return errorCount_ > 0; }
bool MultiErrorCollector::hasFatalErrors() const { return fatalCount_ > 0; }
size_t MultiErrorCollector::errorCount() const { return errorCount_; }
size_t MultiErrorCollector::warningCount() const { return warningCount_; }
size_t MultiErrorCollector::totalCount() const { return errors_.size(); }

std::string MultiErrorCollector::formatAll(bool useArabic) const {
    std::ostringstream oss;
    
    oss << "\n\xE2\x95\x94\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x97\n";
    
    if (useArabic) {
        oss << "\xE2\x95\x91                    \xD9\x86\xD8\xAA\xD8\xA7\xD8\xA6\xD8\xAC \xD8\xA7\xD9\x84\xD8\xAA\xD8\xB1\xD8\xAC\xD9\x85\xD8\xA9                        \xE2\x95\x91\n";
    } else {
        oss << "\xE2\x95\x91                    Compilation Results                    \xE2\x95\x91\n";
    }
    
    oss << "\xE2\x95\x91   ";
    if (errorCount_ > 0) oss << "\xE2\x9D\x8C " << errorCount_ << (useArabic ? " \xD8\xA3\xD8\xAE\xD8\xB7\xD8\xA7\xD8\xA1  " : " errors  ");
    if (warningCount_ > 0) oss << "\xE2\x9A\xA0\xEF\xB8\x8F " << warningCount_ << (useArabic ? " \xD8\xAA\xD8\xAD\xD8\xB0\xD9\x8A\xD8\xB1\xD8\xA7\xD8\xAA  " : " warnings  ");
    if (errorCount_ == 0 && warningCount_ == 0) oss << (useArabic ? "\xE2\x9C\x85 \xD9\x84\xD8\xA7 \xD8\xA3\xD8\xAE\xD8\xB7\xD8\xA7\xD8\xA1" : "\xE2\x9C\x85 No errors");
    oss << "\n";
    
    oss << "\xE2\x95\x9A\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\xE2\x95\x9D\n";
    
    for (const auto& error : errors_) {
        oss << "\n" << error.format(useArabic, true);
    }
    
    return oss.str();
}

std::string MultiErrorCollector::getSummary(bool useArabic) const {
    std::ostringstream oss;
    if (errors_.empty()) {
        oss << (useArabic ? "\xE2\x9C\x85 \xD8\xA7\xD9\x84\xD8\xAA\xD8\xB1\xD8\xAC\xD9\x85\xD8\xA9 \xD9\x86\xD8\xAC\xD8\xAD\xD8\xAA \xD8\xA8\xD8\xAF\xD9\x88\xD9\x86 \xD8\xA3\xD8\xAE\xD8\xB7\xD8\xA7\xD8\xA1\n" : "\xE2\x9C\x85 Compilation succeeded\n");
    } else {
        if (useArabic) {
            oss << "\xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9 \xD8\xA7\xD9\x84\xD8\xAA\xD8\xB1\xD8\xAC\xD9\x85\xD8\xA9: ";
            if (errorCount_ > 0) oss << errorCount_ << " \xD8\xA3\xD8\xAE\xD8\xB7\xD8\xA7\xD8\xA1";
            if (errorCount_ > 0 && warningCount_ > 0) oss << ", ";
            if (warningCount_ > 0) oss << warningCount_ << " \xD8\xAA\xD8\xAD\xD8\xB0\xD9\x8A\xD8\xB1\xD8\xA7\xD8\xAA";
        } else {
            oss << "Result: " << errorCount_ << " errors, " << warningCount_ << " warnings";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string MultiErrorCollector::toJSON() const {
    std::ostringstream oss;
    oss << "{\"errors\":[";
    for (size_t i = 0; i < errors_.size(); i++) {
        if (i > 0) oss << ",";
        const auto& e = errors_[i];
        oss << "{\"code\":\"" << e.code 
            << "\",\"message\":\"" << e.message
            << "\",\"line\":" << e.location.line 
            << ",\"column\":" << e.location.column << "}";
    }
    oss << "],\"errorCount\":" << errorCount_ 
        << ",\"warningCount\":" << warningCount_ << "}";
    return oss.str();
}

void MultiErrorCollector::setMaxErrors(size_t max) { maxErrors_ = max; }
void MultiErrorCollector::setFailOnFirstError(bool fail) { failOnFirstError_ = fail; }

void MultiErrorCollector::clear() {
    errors_.clear();
    seenLocations_.clear();
    errorCount_ = warningCount_ = fatalCount_ = 0;
    nextErrorId_ = 1;
}

bool MultiErrorCollector::isDuplicate(const CompilerError& error) const {
    std::string key = error.location.filename + ":" + 
        std::to_string(error.location.line) + ":" + 
        std::to_string(error.location.column) + ":" + error.code;
    return seenLocations_.count(key) > 0;
}

void MultiErrorCollector::updateCounts(ErrorSeverityLevel severity) {
    switch (severity) {
        case ErrorSeverityLevel::ERROR: errorCount_++; break;
        case ErrorSeverityLevel::WARNING: warningCount_++; break;
        case ErrorSeverityLevel::FATAL: fatalCount_++; errorCount_++; break;
        default: break;
    }
}

std::string MultiErrorCollector::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss;
    struct tm buf;
#ifdef _WIN32
    localtime_s(&buf, &time);
#else
    localtime_r(&time, &buf);
#endif
    oss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string MultiErrorCollector::severityToString(ErrorSeverityLevel sev, bool arabic) const {
    if (arabic) {
        switch (sev) {
            case ErrorSeverityLevel::HINT: return "\xD8\xAA\xD9\x84\xD9\x85\xD9\x8A\xD8\xAD";
            case ErrorSeverityLevel::INFO: return "\xD9\x85\xD8\xB9\xD9\x84\xD9\x88\xD9\x85\xD8\xA9";
            case ErrorSeverityLevel::WARNING: return "\xD8\xAA\xD8\xAD\xD8\xB0\xD9\x8A\xD8\xB1";
            case ErrorSeverityLevel::ERROR: return "\xD8\xAE\xD8\xB7\xD8\xA3";
            case ErrorSeverityLevel::FATAL: return "\xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x82\xD8\xA7\xD8\xAA\xD9\x84";
        }
    } else {
        switch (sev) {
            case ErrorSeverityLevel::HINT: return "hint";
            case ErrorSeverityLevel::INFO: return "info";
            case ErrorSeverityLevel::WARNING: return "warning";
            case ErrorSeverityLevel::ERROR: return "error";
            case ErrorSeverityLevel::FATAL: return "fatal";
        }
    }
    return "";
}

std::string MultiErrorCollector::severityEmoji(ErrorSeverityLevel sev) const {
    switch (sev) {
        case ErrorSeverityLevel::HINT: return "\xF0\x9F\x92\xA1";
        case ErrorSeverityLevel::INFO: return "\xE2\x84\xB9\xEF\xB8\x8F";
        case ErrorSeverityLevel::WARNING: return "\xE2\x9A\xA0\xEF\xB8\x8F";
        case ErrorSeverityLevel::ERROR: return "\xE2\x9D\x8C";
        case ErrorSeverityLevel::FATAL: return "\xF0\x9F\x94\xA5";
    }
    return "";
}

} // namespace Errors
} // namespace Sad
