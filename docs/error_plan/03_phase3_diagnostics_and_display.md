# 🎨 المرحلة 3: التشخيصات المتقدمة والعرض
## Phase 3: Advanced Diagnostics and Display

<div dir="rtl">

## 📋 نظرة عامة / Overview

**المدة المتوقعة**: 2-5 أيام  
**الأولوية**: متوسطة (P1)  
**التبعيات**: المرحلة 2 (التكامل مع Lexer/Parser)

### الأهداف
- تطبيق عرض ملون وجميل للتشخيصات
- إضافة دعم JSON كامل للـ IDE/LSP
- تطبيق Fix-it Hints تفاعلي
- إنشاء تقارير HTML للأخطاء
- دعم Multiple Languages في العرض

---

## 🎯 المهام التفصيلية / Detailed Tasks

### المهمة 3.1: نظام الألوان والتنسيق ✅

**الملف**: `src/errors/diagnostic_formatter.cpp`

#### التطبيق:

```cpp
/**
 * @file diagnostic_formatter.cpp
 * @brief (AR) تنسيق وعرض التشخيصات بأشكال متعددة
 *        (EN) Format and display diagnostics in multiple formats
 * 
 * @author Sad Language Development Team
 * @date November 23, 2025
 */

#include "diagnostic_formatter.h"
#include <sstream>
#include <iomanip>

namespace Sad {
namespace Errors {

/**
 * @brief (AR) رموز ألوان ANSI
 *        (EN) ANSI color codes
 */
namespace Colors {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    
    // (AR) الألوان الأساسية / (EN) Basic colors
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    
    // (AR) ألوان ساطعة / (EN) Bright colors
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_CYAN = "\033[96m";
}

/**
 * @brief (AR) يرجع اللون المناسب لمستوى الشدة
 *        (EN) Returns appropriate color for severity level
 */
std::string DiagnosticFormatter::getSeverityColor(DiagnosticSeverity severity) {
    switch (severity) {
        case DiagnosticSeverity::ERROR:
            return Colors::BRIGHT_RED + Colors::BOLD;
        case DiagnosticSeverity::WARNING:
            return Colors::BRIGHT_YELLOW + Colors::BOLD;
        case DiagnosticSeverity::INFO:
            return Colors::BRIGHT_CYAN;
        case DiagnosticSeverity::NOTE:
            return Colors::BRIGHT_BLUE;
        case DiagnosticSeverity::HINT:
            return Colors::GREEN;
        default:
            return Colors::RESET;
    }
}

/**
 * @brief (AR) يرجع رمز الشدة (emoji)
 *        (EN) Returns severity icon (emoji)
 */
std::string DiagnosticFormatter::getSeverityIcon(DiagnosticSeverity severity) {
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
 * @brief (AR) ينسق تشخيص واحد بشكل جميل
 *        (EN) Formats single diagnostic beautifully
 * 
 * Format:
 * ❌ خطأ [SYN002]: test.s:5:10
 * │ متوقع ';' بعد الجملة
 * │
 * │ 5 │ عرض(5)
 * │   │       ^ هنا
 * │
 * └─ 💡 اقتراح: أضف ';' بعد العبارة
 */
std::string DiagnosticFormatter::format(
    const Diagnostic& diag,
    Language lang,
    bool colorize,
    const std::string& sourceCode
) {
    std::ostringstream oss;
    
    // (AR) السطر الأول: الأيقونة + المستوى + الرمز + الموقع
    // (EN) First line: Icon + Level + Code + Location
    if (colorize) {
        oss << getSeverityColor(diag.getSeverity());
    }
    
    oss << getSeverityIcon(diag.getSeverity()) << " "
        << severityToString(diag.getSeverity()) << " "
        << "[" << getErrorCodeString(diag.getCode()) << "]: "
        << diag.getLocation().toString();
    
    if (colorize) {
        oss << Colors::RESET;
    }
    oss << "\n";
    
    // (AR) السطر الثاني: الرسالة
    // (EN) Second line: Message
    oss << "│ " << diag.getMessage(lang) << "\n";
    
    // (AR) عرض الكود المصدري مع السهم
    // (EN) Show source code with arrow
    if (!sourceCode.empty()) {
        oss << formatSourceSnippet(
            sourceCode,
            diag.getLocation(),
            colorize
        );
    }
    
    // (AR) Fix-it Hints
    // (EN) Fix-it Hints
    const auto& fixits = diag.getFixIts();
    if (!fixits.empty()) {
        oss << "│\n";
        for (const auto& fixit : fixits) {
            oss << "└─ " << getSeverityIcon(DiagnosticSeverity::HINT) << " ";
            
            if (colorize) {
                oss << Colors::GREEN;
            }
            
            oss << (lang == Language::ARABIC ? fixit.description_ar : fixit.description_en);
            
            if (colorize) {
                oss << Colors::RESET;
            }
            
            oss << "\n";
        }
    }
    
    // (AR) الملاحظات المرتبطة
    // (EN) Related notes
    const auto& notes = diag.getNotes();
    for (const auto& note : notes) {
        oss << "\n";
        oss << format(note, lang, colorize, sourceCode);
    }
    
    return oss.str();
}

/**
 * @brief (AR) ينسق مقتطف من الكود المصدري مع السهم
 *        (EN) Formats source code snippet with arrow
 * 
 * Example output:
 * │
 * │ 5 │ عرض(5)
 * │   │       ^ هنا
 * │
 */
std::string DiagnosticFormatter::formatSourceSnippet(
    const std::string& sourceCode,
    const SourceLocation& location,
    bool colorize
) {
    std::ostringstream oss;
    
    // (AR) استخراج السطر من الكود
    // (EN) Extract line from source
    std::vector<std::string> lines = splitLines(sourceCode);
    
    if (location.line == 0 || location.line > lines.size()) {
        return "";
    }
    
    std::string line = lines[location.line - 1];
    
    oss << "│\n";
    
    // (AR) رقم السطر + الكود
    // (EN) Line number + Code
    oss << "│ " << std::setw(4) << location.line << " │ ";
    
    if (colorize) {
        oss << Colors::CYAN;
    }
    
    oss << line;
    
    if (colorize) {
        oss << Colors::RESET;
    }
    
    oss << "\n";
    
    // (AR) السطر مع السهم
    // (EN) Line with arrow
    oss << "│ " << std::string(4, ' ') << " │ "
        << std::string(location.column - 1, ' ');
    
    if (colorize) {
        oss << Colors::BRIGHT_RED + Colors::BOLD;
    }
    
    // (AR) رسم خط متعرج تحت الخطأ
    // (EN) Draw squiggly line under error
    size_t length = std::max(location.length, size_t(1));
    if (length == 1) {
        oss << "^";
    } else {
        oss << "^" << std::string(length - 1, '~');
    }
    
    oss << " هنا";  // "here" in Arabic
    
    if (colorize) {
        oss << Colors::RESET;
    }
    
    oss << "\n│\n";
    
    return oss.str();
}

/**
 * @brief (AR) يقسم النص إلى أسطر
 *        (EN) Splits text into lines
 */
std::vector<std::string> DiagnosticFormatter::splitLines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    
    while (std::getline(stream, line)) {
        lines.push_back(line);
    }
    
    return lines;
}

/**
 * @brief (AR) ينسق عدة تشخيصات معاً
 *        (EN) Formats multiple diagnostics together
 */
std::string DiagnosticFormatter::formatAll(
    const std::vector<Diagnostic>& diagnostics,
    Language lang,
    bool colorize,
    const std::string& sourceCode
) {
    std::ostringstream oss;
    
    // (AR) ملخص في البداية
    // (EN) Summary at the beginning
    size_t errorCount = 0;
    size_t warningCount = 0;
    
    for (const auto& diag : diagnostics) {
        if (diag.getSeverity() == DiagnosticSeverity::ERROR) {
            errorCount++;
        } else if (diag.getSeverity() == DiagnosticSeverity::WARNING) {
            warningCount++;
        }
    }
    
    if (colorize) {
        oss << Colors::BOLD;
    }
    
    oss << "═══════════════════════════════════════════════\n";
    oss << "  ";
    
    if (lang == Language::ARABIC) {
        oss << "تقرير التشخيص";
    } else {
        oss << "Diagnostic Report";
    }
    
    oss << "\n";
    oss << "═══════════════════════════════════════════════\n";
    
    if (colorize) {
        oss << Colors::RESET;
    }
    
    // (AR) عرض كل تشخيص
    // (EN) Display each diagnostic
    for (size_t i = 0; i < diagnostics.size(); i++) {
        oss << format(diagnostics[i], lang, colorize, sourceCode);
        
        if (i < diagnostics.size() - 1) {
            oss << "\n";
            oss << "───────────────────────────────────────────────\n";
            oss << "\n";
        }
    }
    
    // (AR) الملخص في النهاية
    // (EN) Summary at the end
    oss << "\n";
    
    if (colorize) {
        oss << Colors::BOLD;
    }
    
    oss << "═══════════════════════════════════════════════\n";
    
    if (errorCount > 0) {
        if (colorize) {
            oss << Colors::BRIGHT_RED;
        }
        oss << "❌ " << errorCount;
        if (lang == Language::ARABIC) {
            oss << " خطأ";
        } else {
            oss << " error(s)";
        }
        if (colorize) {
            oss << Colors::RESET + Colors::BOLD;
        }
    }
    
    if (warningCount > 0) {
        if (errorCount > 0) oss << "  ";
        
        if (colorize) {
            oss << Colors::BRIGHT_YELLOW;
        }
        oss << "⚠️  " << warningCount;
        if (lang == Language::ARABIC) {
            oss << " تحذير";
        } else {
            oss << " warning(s)";
        }
        if (colorize) {
            oss << Colors::RESET + Colors::BOLD;
        }
    }
    
    oss << "\n";
    oss << "═══════════════════════════════════════════════\n";
    
    if (colorize) {
        oss << Colors::RESET;
    }
    
    return oss.str();
}

} // namespace Errors
} // namespace Sad
```

**الوقت المتوقع**: 4-6 ساعات  
**الاختبارات**: اختبار العرض في terminal مختلف

---

### المهمة 3.2: إخراج JSON للـ IDE ✅

**الملف**: `src/errors/json_exporter.cpp`

#### التطبيق:

```cpp
/**
 * @file json_exporter.cpp
 * @brief (AR) تصدير التشخيصات بصيغة JSON للـ IDE/LSP
 *        (EN) Export diagnostics as JSON for IDE/LSP
 * 
 * @author Sad Language Development Team
 * @date November 23, 2025
 */

#include "json_exporter.h"
#include <sstream>
#include <iomanip>

namespace Sad {
namespace Errors {

/**
 * @brief (AR) يهرب النص للـ JSON
 *        (EN) Escapes string for JSON
 */
std::string JSONExporter::escapeString(const std::string& str) {
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

/**
 * @brief (AR) يحول تشخيص واحد إلى JSON
 *        (EN) Converts single diagnostic to JSON
 * 
 * JSON Schema (LSP-compatible):
 * {
 *   "code": "SYN002",
 *   "severity": "error",
 *   "message": {
 *     "ar": "متوقع ';' بعد الجملة",
 *     "en": "Expected ';' after statement"
 *   },
 *   "location": {
 *     "file": "test.s",
 *     "line": 5,
 *     "column": 10,
 *     "offset": 42,
 *     "length": 1
 *   },
 *   "fixes": [
 *     {
 *       "type": "insert",
 *       "location": { "line": 5, "column": 11 },
 *       "text": ";",
 *       "description": {
 *         "ar": "أضف ';'",
 *         "en": "Add ';'"
 *       }
 *     }
 *   ],
 *   "notes": []
 * }
 */
std::string JSONExporter::toJSON(const Diagnostic& diag) {
    std::ostringstream oss;
    
    oss << "{\n";
    
    // Code
    oss << "  \"code\": \"" << escapeString(getErrorCodeString(diag.getCode())) << "\",\n";
    
    // Severity
    oss << "  \"severity\": \"" << severityToString(diag.getSeverity()) << "\",\n";
    
    // Message (bilingual)
    oss << "  \"message\": {\n";
    oss << "    \"ar\": \"" << escapeString(diag.getMessage(Language::ARABIC)) << "\",\n";
    oss << "    \"en\": \"" << escapeString(diag.getMessage(Language::ENGLISH)) << "\"\n";
    oss << "  },\n";
    
    // Location
    const auto& loc = diag.getLocation();
    oss << "  \"location\": {\n";
    oss << "    \"file\": \"" << escapeString(loc.filename) << "\",\n";
    oss << "    \"line\": " << loc.line << ",\n";
    oss << "    \"column\": " << loc.column << ",\n";
    oss << "    \"offset\": " << loc.offset << ",\n";
    oss << "    \"length\": " << loc.length << "\n";
    oss << "  }";
    
    // Fix-it hints
    const auto& fixits = diag.getFixIts();
    if (!fixits.empty()) {
        oss << ",\n  \"fixes\": [\n";
        
        for (size_t i = 0; i < fixits.size(); i++) {
            const auto& fix = fixits[i];
            
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
            oss << "      \"text\": \"" << escapeString(fix.replacement) << "\",\n";
            
            // Description
            oss << "      \"description\": {\n";
            oss << "        \"ar\": \"" << escapeString(fix.description_ar) << "\",\n";
            oss << "        \"en\": \"" << escapeString(fix.description_en) << "\"\n";
            oss << "      }\n";
            
            oss << "    }";
            
            if (i < fixits.size() - 1) {
                oss << ",";
            }
            oss << "\n";
        }
        
        oss << "  ]";
    }
    
    // Related notes
    const auto& notes = diag.getNotes();
    if (!notes.empty()) {
        oss << ",\n  \"notes\": [\n";
        
        for (size_t i = 0; i < notes.size(); i++) {
            oss << "    " << toJSON(notes[i]);
            
            if (i < notes.size() - 1) {
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

/**
 * @brief (AR) يحول مجموعة تشخيصات إلى JSON
 *        (EN) Converts diagnostic collection to JSON
 */
std::string JSONExporter::toJSON(const std::vector<Diagnostic>& diagnostics) {
    std::ostringstream oss;
    
    oss << "{\n";
    oss << "  \"version\": \"1.0\",\n";
    oss << "  \"language\": \"sad\",\n";
    
    // Statistics
    size_t errorCount = 0;
    size_t warningCount = 0;
    
    for (const auto& diag : diagnostics) {
        if (diag.getSeverity() == DiagnosticSeverity::ERROR) {
            errorCount++;
        } else if (diag.getSeverity() == DiagnosticSeverity::WARNING) {
            warningCount++;
        }
    }
    
    oss << "  \"statistics\": {\n";
    oss << "    \"total\": " << diagnostics.size() << ",\n";
    oss << "    \"errors\": " << errorCount << ",\n";
    oss << "    \"warnings\": " << warningCount << "\n";
    oss << "  },\n";
    
    // Diagnostics array
    oss << "  \"diagnostics\": [\n";
    
    for (size_t i = 0; i < diagnostics.size(); i++) {
        std::string diagJSON = toJSON(diagnostics[i]);
        
        // Indent
        std::istringstream stream(diagJSON);
        std::string line;
        bool first = true;
        
        while (std::getline(stream, line)) {
            if (!first) oss << "\n";
            oss << "    " << line;
            first = false;
        }
        
        if (i < diagnostics.size() - 1) {
            oss << ",";
        }
        oss << "\n";
    }
    
    oss << "  ]\n";
    oss << "}\n";
    
    return oss.str();
}

} // namespace Errors
} // namespace Sad
```

**الوقت المتوقع**: 3-4 ساعات  
**الاختبارات**: التحقق من صحة JSON schema

---

### المهمة 3.3: تقارير HTML ✅

**الملف**: `src/errors/html_exporter.cpp`

```cpp
/**
 * @brief (AR) يصدر تشخيصات كـ HTML
 *        (EN) Exports diagnostics as HTML
 */
std::string HTMLExporter::toHTML(const std::vector<Diagnostic>& diagnostics) {
    std::ostringstream oss;
    
    // HTML Header
    oss << R"(<!DOCTYPE html>
<html dir="rtl" lang="ar">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>تقرير الأخطاء - Sad Language</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: #f5f5f5;
            padding: 20px;
        }
        .container {
            max-width: 1200px;
            margin: 0 auto;
            background: white;
            border-radius: 8px;
            box-shadow: 0 2px 10px rgba(0,0,0,0.1);
            padding: 30px;
        }
        h1 {
            color: #333;
            margin-bottom: 10px;
            border-bottom: 3px solid #e74c3c;
            padding-bottom: 10px;
        }
        .summary {
            background: #ecf0f1;
            padding: 15px;
            border-radius: 5px;
            margin: 20px 0;
            display: flex;
            gap: 20px;
        }
        .summary-item {
            display: flex;
            align-items: center;
            gap: 5px;
        }
        .diagnostic {
            border: 1px solid #ddd;
            border-radius: 5px;
            padding: 15px;
            margin: 15px 0;
            background: #fafafa;
        }
        .diagnostic.error { border-right: 4px solid #e74c3c; }
        .diagnostic.warning { border-right: 4px solid #f39c12; }
        .diagnostic-header {
            display: flex;
            align-items: center;
            gap: 10px;
            margin-bottom: 10px;
        }
        .severity { font-weight: bold; font-size: 14px; }
        .severity.error { color: #e74c3c; }
        .severity.warning { color: #f39c12; }
        .code {
            background: #34495e;
            color: white;
            padding: 2px 8px;
            border-radius: 3px;
            font-family: monospace;
            font-size: 12px;
        }
        .location {
            color: #7f8c8d;
            font-family: monospace;
            font-size: 13px;
        }
        .message {
            margin: 10px 0;
            line-height: 1.6;
        }
        .source-snippet {
            background: #2c3e50;
            color: #ecf0f1;
            padding: 15px;
            border-radius: 5px;
            margin: 10px 0;
            font-family: 'Courier New', monospace;
            overflow-x: auto;
        }
        .line-number {
            color: #7f8c8d;
            margin-left: 10px;
            user-select: none;
        }
        .error-marker {
            color: #e74c3c;
            font-weight: bold;
        }
        .fixit {
            background: #d5f4e6;
            border-right: 3px solid #27ae60;
            padding: 10px;
            margin: 10px 0;
            border-radius: 3px;
        }
        .fixit-icon { color: #27ae60; }
    </style>
</head>
<body>
    <div class="container">
        <h1>📊 تقرير التشخيص - Diagnostic Report</h1>
)";
    
    // Summary
    size_t errorCount = 0;
    size_t warningCount = 0;
    
    for (const auto& diag : diagnostics) {
        if (diag.getSeverity() == DiagnosticSeverity::ERROR) errorCount++;
        else if (diag.getSeverity() == DiagnosticSeverity::WARNING) warningCount++;
    }
    
    oss << "        <div class=\"summary\">\n";
    oss << "            <div class=\"summary-item\">\n";
    oss << "                <span>❌</span>\n";
    oss << "                <strong>" << errorCount << "</strong>\n";
    oss << "                <span>أخطاء (Errors)</span>\n";
    oss << "            </div>\n";
    oss << "            <div class=\"summary-item\">\n";
    oss << "                <span>⚠️</span>\n";
    oss << "                <strong>" << warningCount << "</strong>\n";
    oss << "                <span>تحذيرات (Warnings)</span>\n";
    oss << "            </div>\n";
    oss << "        </div>\n";
    
    // Each diagnostic
    for (const auto& diag : diagnostics) {
        std::string severityClass = (diag.getSeverity() == DiagnosticSeverity::ERROR) 
                                    ? "error" : "warning";
        
        oss << "        <div class=\"diagnostic " << severityClass << "\">\n";
        
        // Header
        oss << "            <div class=\"diagnostic-header\">\n";
        oss << "                <span class=\"severity " << severityClass << "\">";
        oss << severityToString(diag.getSeverity()) << "</span>\n";
        oss << "                <span class=\"code\">" << getErrorCodeString(diag.getCode()) << "</span>\n";
        oss << "                <span class=\"location\">" << diag.getLocation().toString() << "</span>\n";
        oss << "            </div>\n";
        
        // Message
        oss << "            <div class=\"message\">\n";
        oss << "                <strong>🇸🇦 العربية:</strong> " << diag.getMessage(Language::ARABIC) << "<br>\n";
        oss << "                <strong>🇬🇧 English:</strong> " << diag.getMessage(Language::ENGLISH) << "\n";
        oss << "            </div>\n";
        
        // Fix-its
        const auto& fixits = diag.getFixIts();
        if (!fixits.empty()) {
            for (const auto& fix : fixits) {
                oss << "            <div class=\"fixit\">\n";
                oss << "                <span class=\"fixit-icon\">💡</span> ";
                oss << fix.description_ar << " / " << fix.description_en << "\n";
                oss << "            </div>\n";
            }
        }
        
        oss << "        </div>\n";
    }
    
    // Footer
    oss << R"(    </div>
</body>
</html>
)";
    
    return oss.str();
}
```

**الوقت المتوقع**: 2-3 ساعات

---

## 📊 التقدم والإنجاز / Progress Tracking

### قائمة المهام (Checklist):

- [ ] المهمة 3.1: نظام الألوان والتنسيق (4-6 ساعات)
- [ ] المهمة 3.2: JSON Exporter (3-4 ساعات)
- [ ] المهمة 3.3: HTML Exporter (2-3 ساعات)
- [ ] اختبارات التنسيق (3-4 ساعات)
- [ ] دعم IDEs (VSCode extension) (6-8 ساعات - اختياري)
- [ ] تحديث التوثيق (1-2 ساعة)

**إجمالي الوقت المتوقع**: 19-27 ساعة (~3-5 أيام عمل)

---

## 🧪 استراتيجية الاختبار / Testing Strategy

```cpp
// test_diagnostic_formatting.cpp
TEST(DiagnosticFormatterTest, ColorizedOutput) {
    Diagnostic diag = createTestDiagnostic();
    std::string formatted = DiagnosticFormatter::format(
        diag, Language::ARABIC, true, sourceCode
    );
    
    EXPECT_TRUE(formatted.find("\033[") != std::string::npos);  // Has colors
    EXPECT_TRUE(formatted.find("❌") != std::string::npos);  // Has icon
}

// test_json_export.cpp
TEST(JSONExporterTest, ValidJSON) {
    std::vector<Diagnostic> diags = createTestDiagnostics();
    std::string json = JSONExporter::toJSON(diags);
    
    // Verify valid JSON
    EXPECT_NO_THROW(nlohmann::json::parse(json));
}
```

---

## 📈 معايير الإنجاز / Completion Criteria

- ✅ عرض ملون وجميل في Terminal
- ✅ JSON صحيح متوافق مع LSP
- ✅ HTML reports كاملة
- ✅ Fix-it hints واضحة
- ✅ دعم لغتين كامل
- ✅ الاختبارات تمر (> 90%)

---

**التاريخ**: 23 نوفمبر 2025  
**الحالة**: جاهز للتنفيذ  
**المرحلة التالية**: المرحلة 4 - الاختبار والتوثيق

</div>
