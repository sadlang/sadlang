/**
 * @file smart_fix_suggestions.cpp
 * @brief (AR) تطبيق اقتراحات الإصلاح - Sad::Errors
 * (AR) المهمة: T166 - Phase 15 (US13)
 */

#include "fix_suggestions.h"
#include <sstream>
#include <algorithm>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                    FixSuggestionItem::format
// ═══════════════════════════════════════════════════════════════════════

std::string FixSuggestionItem::format(bool useArabic) const {
    std::ostringstream oss;
    
    // Icon based on type
    std::string icon;
    switch (type) {
        case FixType::QUICK_FIX: icon = "\xF0\x9F\x94\xA7"; break;
        case FixType::SUGGESTION: icon = "\xF0\x9F\x92\xA1"; break;
        case FixType::REFACTOR: icon = "\xF0\x9F\x94\x84"; break;
        case FixType::LEARNING: icon = "\xF0\x9F\x93\x9A"; break;
        case FixType::WARNING_FIX: icon = "\xE2\x9A\xA0\xEF\xB8\x8F"; break;
    }
    
    oss << icon << " " << (useArabic ? title : titleEn) << "\n";
    if (!description.empty()) {
        oss << "   " << (useArabic ? description : descriptionEn) << "\n";
    }
    if (!explanation.empty()) {
        oss << "   \xF0\x9F\x93\x96 " << explanation << "\n";
    }
    if (!codeExample.empty()) {
        oss << "\n   " << (useArabic ? "\xD9\x85\xD8\xAB\xD8\xA7\xD9\x84:" : "Example:") << "\n";
        std::istringstream stream(codeExample);
        std::string line;
        while (std::getline(stream, line)) {
            oss << "   " << line << "\n";
        }
    }
    return oss.str();
}

// ═══════════════════════════════════════════════════════════════════════
//                    FixSuggestionManager
// ═══════════════════════════════════════════════════════════════════════

FixSuggestionManager::FixSuggestionManager() {
    initializeCommonPatterns();
}

void FixSuggestionManager::initializeCommonPatterns() {
    commonPatterns_["brackets"] = {"(", ")", "{", "}", "[", "]"};
    commonPatterns_["keywords"] = {
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9", "\xD8\xA5\xD8\xB0\xD8\xA7",
        "\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7", "\xD8\xB7\xD8\xA7\xD9\x84\xD9\x85\xD8\xA7",
        "\xD9\x84\xD9\x83\xD9\x84", "\xD8\xA3\xD8\xB1\xD8\xAC\xD8\xB9",
        "\xD8\xB5\xD9\x86\xD9\x81", "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1",
        "\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA"
    };
}

std::vector<FixSuggestionItem> FixSuggestionManager::getFixSuggestions(
    ErrorCode code, const std::string& context,
    const std::unordered_map<std::string, std::string>& details
) {
    std::vector<FixSuggestionItem> fixes;
    
    switch (code) {
        case ErrorCode::SYN_MISSING_SEMICOLON: {
            FixSuggestionItem fix;
            fix.title = "\xD9\x84\xD8\xA7 \xD8\xAD\xD8\xA7\xD8\xAC\xD8\xA9 \xD9\x84\xD9\x84\xD9\x81\xD8\xA7\xD8\xB5\xD9\x84\xD8\xA9 \xD8\xA7\xD9\x84\xD9\x85\xD9\x86\xD9\x82\xD9\x88\xD8\xB7\xD8\xA9";
            fix.titleEn = "No semicolon needed";
            fix.description = "\xD9\x84\xD8\xBA\xD8\xA9 \xD8\xB3\xD8\xB9\xD8\xAF \xD9\x84\xD8\xA7 \xD8\xAA\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD9\x85 \xD8\xA7\xD9\x84\xD9\x81\xD8\xA7\xD8\xB5\xD9\x84\xD8\xA9 \xD8\xA7\xD9\x84\xD9\x85\xD9\x86\xD9\x82\xD9\x88\xD8\xB7\xD8\xA9";
            fix.descriptionEn = "Sad language doesn't use semicolons";
            fix.type = FixType::QUICK_FIX;
            fix.priority = FixPriority::HIGH;
            fixes.push_back(fix);
            break;
        }
        case ErrorCode::SYN_UNCLOSED_BRACKET: {
            FixSuggestionItem fix;
            fix.title = "\xD8\xA3\xD8\xBA\xD9\x84\xD9\x82 \xD8\xA7\xD9\x84\xD9\x82\xD9\x88\xD8\xB3";
            fix.titleEn = "Close the bracket";
            fix.description = "\xD8\xAA\xD8\xA3\xD9\x83\xD8\xAF \xD9\x85\xD9\x86 \xD8\xA5\xD8\xBA\xD9\x84\xD8\xA7\xD9\x82 \xD8\xAC\xD9\x85\xD9\x8A\xD8\xB9 \xD8\xA7\xD9\x84\xD8\xA3\xD9\x82\xD9\x88\xD8\xA7\xD8\xB3";
            fix.descriptionEn = "Make sure all brackets are closed";
            fix.type = FixType::QUICK_FIX;
            fix.priority = FixPriority::CRITICAL;
            fixes.push_back(fix);
            break;
        }
        case ErrorCode::SEM_UNDEFINED_VARIABLE: {
            FixSuggestionItem fix;
            fix.title = "\xD8\xB9\xD8\xB1\xD9\x91\xD9\x81 \xD8\xA7\xD9\x84\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xA3\xD9\x88\xD9\x84\xD8\xA7\xD9\x8B";
            fix.titleEn = "Define variable first";
            fix.description = "\xD8\xA3\xD8\xB6\xD9\x81 \xD8\xAA\xD8\xB9\xD8\xB1\xD9\x8A\xD9\x81\xD8\xA7\xD9\x8B \xD9\x84\xD9\x84\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x82\xD8\xA8\xD9\x84 \xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD8\xA7\xD9\x85\xD9\x87";
            fix.descriptionEn = "Add variable definition before use";
            fix.type = FixType::SUGGESTION;
            fix.priority = FixPriority::HIGH;
            if (details.count("symbol")) {
                fix.codeExample = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 " + details.at("symbol") + " = ...";
            }
            fixes.push_back(fix);
            break;
        }
        case ErrorCode::SEM_TYPE_MISMATCH: {
            fixes = getTypeFixes(
                details.count("expected") ? details.at("expected") : "",
                details.count("actual") ? details.at("actual") : "",
                details.count("expression") ? details.at("expression") : ""
            );
            break;
        }
        case ErrorCode::SEM_WRONG_ARG_COUNT: {
            FixSuggestionItem fix;
            fix.title = "\xD8\xB5\xD8\xAD\xD9\x91\xD8\xAD \xD8\xB9\xD8\xAF\xD8\xAF \xD8\xA7\xD9\x84\xD9\x85\xD8\xB9\xD8\xA7\xD9\x85\xD9\x84\xD8\xA7\xD8\xAA";
            fix.titleEn = "Fix argument count";
            fix.type = FixType::QUICK_FIX;
            fix.priority = FixPriority::HIGH;
            fixes.push_back(fix);
            break;
        }
        default: break;
    }
    
    return fixes;
}

std::vector<FixSuggestionItem> FixSuggestionManager::getSyntaxFixes(
    const std::string& expected, const std::string& found, const std::string& context
) {
    std::vector<FixSuggestionItem> fixes;
    
    if (found == ";") {
        FixSuggestionItem fix;
        fix.title = "\xD8\xA3\xD8\xB2\xD9\x84 \xD8\xA7\xD9\x84\xD9\x81\xD8\xA7\xD8\xB5\xD9\x84\xD8\xA9 \xD8\xA7\xD9\x84\xD9\x85\xD9\x86\xD9\x82\xD9\x88\xD8\xB7\xD8\xA9";
        fix.titleEn = "Remove semicolon";
        fix.type = FixType::QUICK_FIX;
        fix.priority = FixPriority::HIGH;
        fixes.push_back(fix);
    }
    
    if (expected == ")" || expected == "}" || expected == "]") {
        FixSuggestionItem fix;
        fix.title = "\xD8\xA3\xD8\xB6\xD9\x81 '" + expected + "' \xD8\xA7\xD9\x84\xD9\x85\xD9\x81\xD9\x82\xD9\x88\xD8\xAF\xD8\xA9";
        fix.titleEn = "Add missing '" + expected + "'";
        fix.type = FixType::QUICK_FIX;
        fix.priority = FixPriority::CRITICAL;
        fixes.push_back(fix);
    }
    
    return fixes;
}

std::vector<FixSuggestionItem> FixSuggestionManager::getTypeFixes(
    const std::string& expectedType, const std::string& actualType,
    const std::string& expression
) {
    std::vector<FixSuggestionItem> fixes;
    
    if (!expectedType.empty() && !actualType.empty()) {
        FixSuggestionItem fix;
        fix.title = "\xD8\xAD\xD9\x88\xD9\x91\xD9\x84 \xD8\xA7\xD9\x84\xD9\x82\xD9\x8A\xD9\x85\xD8\xA9";
        fix.titleEn = "Convert value";
        fix.description = "\xD8\xAD\xD9\x88\xD9\x91\xD9\x84 \xD9\x85\xD9\x86 '" + actualType + "' \xD8\xA5\xD9\x84\xD9\x89 '" + expectedType + "'";
        fix.descriptionEn = "Convert from '" + actualType + "' to '" + expectedType + "'";
        if (!expression.empty()) {
            fix.codeExample = expectedType + "(" + expression + ")";
        }
        fix.type = FixType::QUICK_FIX;
        fix.priority = FixPriority::HIGH;
        fixes.push_back(fix);
    }
    
    return fixes;
}

std::vector<FixSuggestionItem> FixSuggestionManager::getCommonFixes(
    const std::string& errorCode, const std::string& context
) {
    // Return empty for unrecognized patterns
    return {};
}

std::vector<FixItHint> FixSuggestionManager::toFixItHints(const std::vector<FixSuggestionItem>& fixes) {
    std::vector<FixItHint> hints;
    for (const auto& fix : fixes) {
        for (const auto& edit : fix.edits) {
            FixItHint hint;
            hint.type = FixItHint::Type::REPLACE;
            hint.replacement = edit.newText;
            hint.description_ar = fix.title;
            hint.description_en = fix.titleEn;
            hint.location = SourceLocation(
                "", edit.startLine, edit.startColumn, 0,
                edit.endColumn > edit.startColumn ? edit.endColumn - edit.startColumn : 0
            );
            hints.push_back(hint);
        }
    }
    return hints;
}

} // namespace Errors
} // namespace Sad
