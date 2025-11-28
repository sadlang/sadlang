/**
 * @file diagnostic.cpp
 * @brief (AR) تطبيق نظام التشخيص للأخطاء
 *        (EN) Implementation of diagnostic system for errors
 * 
 * (AR) يحتوي على تطبيقات كاملة لفئة Diagnostic وأنظمة التنسيق
 * (EN) Contains complete implementations of Diagnostic class and formatting systems
 * 
 * @author Sad Language Development Team
 * @date November 23, 2025
 * @version 1.0
 */

#include "errors/diagnostic.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Sad {
namespace Errors {

// ====================================================================
// (AR) دوال المساعدة / (EN) Helper Functions
// ====================================================================

/**
 * @brief (AR) يحول مستوى الشدة إلى نص
 *        (EN) Converts severity to string
 */
std::string severityToString(DiagnosticSeverity severity) {
    switch (severity) {
        case DiagnosticSeverity::ERROR:
            return "error";
        case DiagnosticSeverity::WARNING:
            return "warning";
        case DiagnosticSeverity::INFO:
            return "info";
        case DiagnosticSeverity::NOTE:
            return "note";
        case DiagnosticSeverity::HINT:
            return "hint";
        default:
            return "unknown";
    }
}

/**
 * @brief (AR) يرجع رمز اللون المناسب
 *        (EN) Returns appropriate color code
 */
std::string severityToColor(DiagnosticSeverity severity) {
    switch (severity) {
        case DiagnosticSeverity::ERROR:
            return "\033[91m\033[1m";  // Bright Red + Bold
        case DiagnosticSeverity::WARNING:
            return "\033[93m\033[1m";  // Bright Yellow + Bold
        case DiagnosticSeverity::INFO:
            return "\033[96m";         // Bright Cyan
        case DiagnosticSeverity::NOTE:
            return "\033[94m";         // Bright Blue
        case DiagnosticSeverity::HINT:
            return "\033[32m";         // Green
        default:
            return "\033[0m";          // Reset
    }
}

/**
 * @brief (AR) يرجع الأيقونة المناسبة
 *        (EN) Returns appropriate icon
 */
static std::string getSeverityIcon(DiagnosticSeverity severity) {
    switch (severity) {
        case DiagnosticSeverity::ERROR:
            return "❌";
        case DiagnosticSeverity::WARNING:
            return "⚠️ ";
        case DiagnosticSeverity::INFO:
            return "ℹ️ ";
        case DiagnosticSeverity::NOTE:
            return "📝";
        case DiagnosticSeverity::HINT:
            return "💡";
        default:
            return "  ";
    }
}

/**
 * @brief (AR) يقسم النص إلى أسطر
 *        (EN) Splits text into lines
 */
static std::vector<std::string> splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

/**
 * @brief (AR) يهرب النص لـ JSON
 *        (EN) Escapes string for JSON
 */
static std::string escapeJSON(const std::string& str) {
    std::ostringstream oss;
    
    for (char c : str) {
        switch (c) {
            case '"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (c < 0x20) {
                    oss << "\\u" << std::hex << std::setw(4)
                        << std::setfill('0') << static_cast<int>(c);
                } else {
                    oss << c;
                }
        }
    }
    
    return oss.str();
}

// ====================================================================
// (AR) تطبيق Diagnostic / (EN) Diagnostic Implementation
// ====================================================================

/**
 * @brief (AR) البناء
 *        (EN) Constructor
 */
Diagnostic::Diagnostic(ErrorCode code, DiagnosticSeverity severity,
                       const SourceLocation& location,
                       const std::string& message_ar,
                       const std::string& message_en)
    : code_(code), severity_(severity), location_(location),
      message_ar_(message_ar), message_en_(message_en) {}

/**
 * @brief (AR) البناء الافتراضي
 *        (EN) Default constructor
 */
Diagnostic::Diagnostic()
    : code_(ErrorCode::LEX_INVALID_CHARACTER),
      severity_(DiagnosticSeverity::ERROR) {}

/**
 * @brief (AR) يرجع الرسالة باللغة المحددة
 *        (EN) Returns message in specified language
 */
const std::string& Diagnostic::getMessage(Language lang) const {
    if (lang == Language::ARABIC) {
        return message_ar_;
    } else {
        return message_en_;
    }
}

/**
 * @brief (AR) يضيف ملاحظة مرتبطة
 *        (EN) Adds related note
 */
void Diagnostic::addNote(const Diagnostic& note) {
    notes_.push_back(note);
}

/**
 * @brief (AR) يضيف اقتراح إصلاح
 *        (EN) Adds fix-it hint
 */
void Diagnostic::addFixIt(const FixItHint& fixit) {
    fixits_.push_back(fixit);
}

/**
 * @brief (AR) ينسق التشخيص للعرض
 *        (EN) Formats diagnostic for display
 */
std::string Diagnostic::format(Language lang, bool colorize,
                               const std::string& sourceCode) const {
    std::ostringstream oss;
    const std::string RESET = "\033[0m";
    
    // (AR) السطر الأول: الأيقونة + المستوى + الرمز + الموقع
    // (EN) First line: Icon + Level + Code + Location
    if (colorize) {
        oss << severityToColor(severity_);
    }
    
    oss << getSeverityIcon(severity_) << " "
        << severityToString(severity_) << " "
        << "[" << getErrorCodeString(code_) << "]: "
        << location_.toString();
    
    if (colorize) {
        oss << RESET;
    }
    oss << "\n";
    
    // (AR) السطر الثاني: الرسالة
    // (EN) Second line: Message
    oss << "│ " << getMessage(lang) << "\n";
    
    // (AR) عرض الكود المصدري مع السهم
    // (EN) Show source code with arrow
    if (!sourceCode.empty() && location_.isValid()) {
        std::vector<std::string> lines = splitLines(sourceCode);
        
        if (location_.line > 0 && location_.line <= lines.size()) {
            std::string line = lines[location_.line - 1];
            
            oss << "│\n";
            
            // (AR) رقم السطر + الكود
            // (EN) Line number + Code
            oss << "│ " << std::setw(4) << location_.line << " │ ";
            
            if (colorize) {
                oss << "\033[36m";  // Cyan
            }
            
            oss << line;
            
            if (colorize) {
                oss << RESET;
            }
            
            oss << "\n";
            
            // (AR) السطر مع السهم
            // (EN) Line with arrow
            oss << "│ " << std::string(4, ' ') << " │ "
                << std::string(location_.column > 0 ? location_.column - 1 : 0, ' ');
            
            if (colorize) {
                oss << "\033[91m\033[1m";  // Bright Red + Bold
            }
            
            // (AR) رسم خط متعرج تحت الخطأ
            // (EN) Draw squiggly line under error
            size_t length = std::max(location_.length, size_t(1));
            if (length == 1) {
                oss << "^";
            } else {
                oss << "^" << std::string(length - 1, '~');
            }
            
            if (lang == Language::ARABIC) {
                oss << " هنا";
            } else {
                oss << " here";
            }
            
            if (colorize) {
                oss << RESET;
            }
            
            oss << "\n│\n";
        }
    }
    
    // (AR) Fix-it Hints
    // (EN) Fix-it Hints
    if (!fixits_.empty()) {
        for (const auto& fixit : fixits_) {
            oss << "└─ " << getSeverityIcon(DiagnosticSeverity::HINT) << " ";
            
            if (colorize) {
                oss << "\033[32m";  // Green
            }
            
            oss << (lang == Language::ARABIC ? fixit.description_ar : fixit.description_en);
            
            if (colorize) {
                oss << RESET;
            }
            
            oss << "\n";
        }
    }
    
    // (AR) الملاحظات المرتبطة
    // (EN) Related notes
    for (const auto& note : notes_) {
        oss << "\n";
        oss << note.format(lang, colorize, sourceCode);
    }
    
    return oss.str();
}

/**
 * @brief (AR) يحول التشخيص إلى JSON
 *        (EN) Converts diagnostic to JSON
 */
std::string Diagnostic::toJSON() const {
    std::ostringstream oss;
    
    oss << "{\n";
    
    // Code
    oss << "  \"code\": \"" << escapeJSON(getErrorCodeString(code_)) << "\",\n";
    
    // Severity
    oss << "  \"severity\": \"" << severityToString(severity_) << "\",\n";
    
    // Message (bilingual)
    oss << "  \"message\": {\n";
    oss << "    \"ar\": \"" << escapeJSON(message_ar_) << "\",\n";
    oss << "    \"en\": \"" << escapeJSON(message_en_) << "\"\n";
    oss << "  },\n";
    
    // Location
    oss << "  \"location\": {\n";
    oss << "    \"file\": \"" << escapeJSON(location_.filename) << "\",\n";
    oss << "    \"line\": " << location_.line << ",\n";
    oss << "    \"column\": " << location_.column << ",\n";
    oss << "    \"offset\": " << location_.offset << ",\n";
    oss << "    \"length\": " << location_.length << "\n";
    oss << "  }";
    
    // Fix-it hints
    if (!fixits_.empty()) {
        oss << ",\n  \"fixes\": [\n";
        
        for (size_t i = 0; i < fixits_.size(); i++) {
            const auto& fix = fixits_[i];
            
            oss << "    {\n";
            
            // Type
            std::string typeStr;
            switch (fix.type) {
                case FixItHint::Type::INSERT: typeStr = "insert"; break;
                case FixItHint::Type::REMOVE: typeStr = "remove"; break;
                case FixItHint::Type::REPLACE: typeStr = "replace"; break;
            }
            oss << "      \"type\": \"" << typeStr << "\",\n";
            
            // Location
            oss << "      \"location\": {\n";
            oss << "        \"line\": " << fix.location.line << ",\n";
            oss << "        \"column\": " << fix.location.column << "\n";
            oss << "      },\n";
            
            // Text
            oss << "      \"text\": \"" << escapeJSON(fix.replacement) << "\",\n";
            
            // Description
            oss << "      \"description\": {\n";
            oss << "        \"ar\": \"" << escapeJSON(fix.description_ar) << "\",\n";
            oss << "        \"en\": \"" << escapeJSON(fix.description_en) << "\"\n";
            oss << "      }\n";
            
            oss << "    }";
            
            if (i < fixits_.size() - 1) {
                oss << ",";
            }
            oss << "\n";
        }
        
        oss << "  ]";
    }
    
    // Related notes
    if (!notes_.empty()) {
        oss << ",\n  \"notes\": [\n";
        
        for (size_t i = 0; i < notes_.size(); i++) {
            std::string noteJSON = notes_[i].toJSON();
            
            // Indent note JSON
            std::istringstream stream(noteJSON);
            std::string line;
            bool first = true;
            
            while (std::getline(stream, line)) {
                if (!first) oss << "\n";
                oss << "    " << line;
                first = false;
            }
            
            if (i < notes_.size() - 1) {
                oss << ",";
            }
            oss << "\n";
        }
        
        oss << "  ]";
    } else {
        oss << ",\n  \"notes\": []";
    }
    
    oss << "\n}";
    
    return oss.str();
}

} // namespace Errors
} // namespace Sad
