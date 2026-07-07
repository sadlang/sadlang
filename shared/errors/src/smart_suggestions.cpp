/**
 * @file smart_suggestions.cpp
 * @brief (AR) تطبيق نظام اقتراحات الإصلاح الذكي
 *        (EN) Smart suggestions implementation - unified with Sad::Errors
 * 
 * (AR) المهمة: T161 - Phase 15 (US13)
 * (AR) يعيد تصدير المنطق من suggestions.cpp الأصلي مع namespace موحد
 */

#include "suggestions.h"
#include <sstream>
#include <algorithm>
#include <cmath>

namespace Sad {
namespace Errors {

namespace {
// (AR) مفتاح داخليّ يميّز نوع الرمز غير المعرّف (متغيّر/دالّة) لتوجيه الاقتراحات:
//      اقتراح «استورد وحدة» منطقيّ لدالّة غير معرّفة (قد تكون من مكتبة غير مستوردة)
//      لا لمتغيّر غير معرّف (حيث «عرّفه» هو الجواب الغالب). [تقليل ضجيج التلميحات]
// (EN) Internal key marking the undefined-symbol kind (variable/function) to steer
//      suggestions: an "import a module" hint fits an undefined FUNCTION (may be from
//      an unimported library) but not a bare undefined VARIABLE (whose usual answer is
//      "define it"). Showing import for every undefined symbol is misleading noise.
constexpr const char* kUndefKindKey      = "undef_kind";
constexpr const char* kUndefKindFunction = "function";
} // namespace

// ═══════════════════════════════════════════════════════════════════════
//                    SmartSuggestion::format
// ═══════════════════════════════════════════════════════════════════════

std::string SmartSuggestion::format(bool useArabic) const {
    std::ostringstream oss;
    oss << "\u202B\xF0\x9F\x92\xA1 " << (useArabic ? title : titleEn) << "\n";
    if (!description.empty()) {
        oss << "   " << (useArabic ? description : descriptionEn) << "\n";
    }
    if (!codeExample.empty()) {
        oss << "\n   " << (useArabic ? "\xD9\x85\xD8\xAB\xD8\xA7\xD9\x84:" : "Example:") << "\n";
        oss << "   \xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\xE2\x94\x80\n";
        std::istringstream stream(codeExample);
        std::string line;
        while (std::getline(stream, line)) {
            oss << "   " << line << "\n";
        }
    }
    return oss.str();
}

// ═══════════════════════════════════════════════════════════════════════
//                    SuggestionEngine
// ═══════════════════════════════════════════════════════════════════════

SuggestionEngine::SuggestionEngine() {
    initializePatterns();
}

void SuggestionEngine::initializePatterns() {
    commonPatterns_["type_conversion"] = {
        "\xD9\x86\xD8\xB5", "\xD8\xB9\xD8\xAF\xD8\xAF",
        "\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD", "\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A",
        "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A"
    };
    commonPatterns_["reserved_words"] = {
        "\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9", "\xD8\xA5\xD8\xB0\xD8\xA7",
        "\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7", "\xD8\xB7\xD8\xA7\xD9\x84\xD9\x85\xD8\xA7",
        "\xD9\x84\xD9\x83\xD9\x84", "\xD8\xA3\xD8\xB1\xD8\xAC\xD8\xB9",
        "\xD8\xB5\xD9\x86\xD9\x81", "\xD8\xAC\xD8\xAF\xD9\x8A\xD8\xAF",
        "\xD9\x87\xD8\xB0\xD8\xA7", "\xD8\xB9\xD8\xAF\xD9\x85"
    };
}

std::vector<SmartSuggestion> SuggestionEngine::generateSuggestions(
    SmartErrorCategory category,
    const std::string& errorCode,
    const std::string& context,
    const std::unordered_map<std::string, std::string>& details
) {
    std::vector<SmartSuggestion> suggestions;
    
    switch (category) {
        case SmartErrorCategory::TYPE_MISMATCH:
            suggestions = generateTypeMismatchSuggestions(details);
            break;
        case SmartErrorCategory::UNDEFINED_SYMBOL:
            suggestions = generateUndefinedSymbolSuggestions(details, context);
            break;
        case SmartErrorCategory::SYNTAX_ERROR:
            suggestions = generateSyntaxSuggestions(details, context);
            break;
        case SmartErrorCategory::IMPORT_ERROR:
            suggestions = generateImportSuggestions(details);
            break;
        case SmartErrorCategory::ARGUMENT_ERROR:
            suggestions = generateArgumentSuggestions(details);
            break;
        case SmartErrorCategory::ARABIC_ENCODING:
            suggestions = generateArabicEncodingSuggestions(details);
            break;
        case SmartErrorCategory::NAMING_CONVENTION:
            suggestions = generateNamingSuggestions(details);
            break;
        default:
            break;
    }
    
    std::sort(suggestions.begin(), suggestions.end(),
        [](const SmartSuggestion& a, const SmartSuggestion& b) {
            return static_cast<int>(a.confidence) < static_cast<int>(b.confidence);
        });
    
    return suggestions;
}

std::vector<SmartSuggestion> SuggestionEngine::generateFromErrorCode(
    ErrorCode code,
    const std::string& context,
    const std::unordered_map<std::string, std::string>& details
) {
    SmartErrorCategory category;
    std::string codeStr = getErrorCodeString(code);
    
    // تحديد الفئة من رمز الخطأ
    if (code == ErrorCode::SEM_TYPE_MISMATCH)
        category = SmartErrorCategory::TYPE_MISMATCH;
    else if (code == ErrorCode::SEM_UNDEFINED_VARIABLE || code == ErrorCode::SEM_UNDEFINED_FUNCTION)
        category = SmartErrorCategory::UNDEFINED_SYMBOL;
    else if (code == ErrorCode::SYN_UNEXPECTED_TOKEN || code == ErrorCode::SYN_MISSING_SEMICOLON ||
             code == ErrorCode::SYN_UNCLOSED_BRACKET)
        category = SmartErrorCategory::SYNTAX_ERROR;
    else if (code == ErrorCode::RUN_IMPORT_ERROR)
        category = SmartErrorCategory::IMPORT_ERROR;
    else if (code == ErrorCode::SEM_WRONG_ARG_COUNT)
        category = SmartErrorCategory::ARGUMENT_ERROR;
    else if (code == ErrorCode::LEX_INVALID_UTF8)
        category = SmartErrorCategory::ARABIC_ENCODING;
    else
        category = SmartErrorCategory::SYNTAX_ERROR;

    // (AR) مرّر نوع الرمز غير المعرّف (دالّة؟) كي يُقصَر اقتراح الاستيراد على الدوالّ.
    // (EN) Thread the undefined-symbol kind (is it a function?) so the import hint is
    //      restricted to functions, not bare undefined variables.
    if (code == ErrorCode::SEM_UNDEFINED_FUNCTION) {
        std::unordered_map<std::string, std::string> enriched = details;
        enriched[kUndefKindKey] = kUndefKindFunction;
        return generateSuggestions(category, codeStr, context, enriched);
    }
    return generateSuggestions(category, codeStr, context, details);
}

std::vector<std::string> SuggestionEngine::findSimilarSymbols(
    const std::string& symbol,
    const std::vector<std::string>& available
) {
    std::vector<std::string> similar;
    
    for (const auto& candidate : available) {
        size_t dist = levenshteinDistance(symbol, candidate);
        if (dist > 0 && dist <= 3) {
            similar.push_back(candidate);
        }
    }
    
    std::sort(similar.begin(), similar.end(),
        [&](const std::string& a, const std::string& b) {
            return levenshteinDistance(symbol, a) < levenshteinDistance(symbol, b);
        });
    
    if (similar.size() > 3) similar.resize(3);
    return similar;
}

size_t SuggestionEngine::levenshteinDistance(const std::string& s1, const std::string& s2) {
    size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<size_t>> dp(len1 + 1, std::vector<size_t>(len2 + 1));
    for (size_t i = 0; i <= len1; i++) dp[i][0] = i;
    for (size_t j = 0; j <= len2; j++) dp[0][j] = j;
    for (size_t i = 1; i <= len1; i++) {
        for (size_t j = 1; j <= len2; j++) {
            size_t cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            dp[i][j] = std::min({dp[i-1][j] + 1, dp[i][j-1] + 1, dp[i-1][j-1] + cost});
        }
    }
    return dp[len1][len2];
}

// ═══════════════════════════════════════════════════════════════════════
//                    مولدات الاقتراحات المتخصصة
// ═══════════════════════════════════════════════════════════════════════

std::vector<SmartSuggestion> SuggestionEngine::generateTypeMismatchSuggestions(
    const std::unordered_map<std::string, std::string>& details
) {
    std::vector<SmartSuggestion> suggestions;
    std::string expected = details.count("expected") ? details.at("expected") : "";
    std::string actual = details.count("actual") ? details.at("actual") : "";
    std::string expr = details.count("expression") ? details.at("expression") : "";
    
    if (!expected.empty() && !actual.empty()) {
        SmartSuggestion conv;
        conv.title = "\xD8\xAD\xD9\x88\xD9\x91\xD9\x84 \xD8\xA7\xD9\x84\xD9\x82\xD9\x8A\xD9\x85\xD8\xA9 \xD8\xA5\xD9\x84\xD9\x89 \xD8\xA7\xD9\x84\xD9\x86\xD9\x88\xD8\xB9 \xD8\xA7\xD9\x84\xD9\x85\xD8\xB7\xD9\x84\xD9\x88\xD8\xA8";
        conv.titleEn = "Convert value to expected type";
        
        if (expected == "\xD9\x86\xD8\xB5" || expected == "string") {
            conv.description = "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD9\x85 \xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD9\x86\xD8\xB5() \xD9\x84\xD9\x84\xD8\xAA\xD8\xAD\xD9\x88\xD9\x8A\xD9\x84";
            conv.descriptionEn = "Use text() function to convert";
            conv.codeExample = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9 = \xD9\x86\xD8\xB5(" + expr + ")";
            conv.confidence = ConfidenceLevel::HIGH;
        } else if (expected == "\xD8\xB9\xD8\xAF\xD8\xAF" || expected == "number") {
            conv.description = "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD9\x85 \xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xB9\xD8\xAF\xD8\xAF() \xD9\x84\xD9\x84\xD8\xAA\xD8\xAD\xD9\x88\xD9\x8A\xD9\x84";
            conv.descriptionEn = "Use number() function to convert";
            conv.codeExample = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x86\xD8\xAA\xD9\x8A\xD8\xAC\xD8\xA9 = \xD8\xB9\xD8\xAF\xD8\xAF(" + expr + ")";
            conv.confidence = ConfidenceLevel::HIGH;
        }
        conv.type = SuggestionType::REPLACEMENT;
        suggestions.push_back(conv);
    }
    
    return suggestions;
}

std::vector<SmartSuggestion> SuggestionEngine::generateUndefinedSymbolSuggestions(
    const std::unordered_map<std::string, std::string>& details,
    const std::string& context
) {
    std::vector<SmartSuggestion> suggestions;
    std::string symbol = details.count("symbol") ? details.at("symbol") : "";
    
    // اقتراح: تعريف المتغير
    {
        SmartSuggestion define;
        define.title = "\xD8\xB9\xD8\xB1\xD9\x91\xD9\x81 \xD8\xA7\xD9\x84\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xA3\xD9\x88\xD9\x84\xD8\xA7\xD9\x8B";
        define.titleEn = "Define the variable first";
        define.description = "\xD8\xA7\xD9\x84\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x85\xD8\xB9\xD8\xB1\xD9\x91\xD9\x81. \xD8\xA3\xD8\xB6\xD9\x81 \xD8\xAA\xD8\xB9\xD8\xB1\xD9\x8A\xD9\x81\xD8\xA7\xD9\x8B \xD9\x84\xD9\x87 \xD9\x82\xD8\xA8\xD9\x84 \xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD8\xA7\xD9\x85\xD9\x87";
        define.descriptionEn = "Variable is undefined. Add declaration before use";
        define.codeExample = "\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 " + symbol + " = \xD9\x82\xD9\x8A\xD9\x85\xD8\xA9_\xD8\xA7\xD8\xA8\xD8\xAA\xD8\xAF\xD8\xA7\xD8\xA6\xD9\x8A\xD8\xA9";
        define.type = SuggestionType::INSERTION;
        define.confidence = ConfidenceLevel::MEDIUM;
        suggestions.push_back(define);
    }
    
    // (AR) اقتراح «استورد وحدة» للدوالّ غير المعرّفة فقط: قد تكون الدالّة من مكتبة لم
    //      تُستورَد. أمّا المتغيّر غير المعرّف فجوابه الغالب «عرّفه»، وإظهار الاستيراد له
    //      تلميحٌ عامّ مُضلِّل ينطبق على كلّ رمز غير معرّف بلا قيمة. [تقليل ضجيج التلميحات]
    // (EN) Offer "import a module" only for undefined FUNCTIONS (may come from an
    //      unimported library). For a bare undefined variable "define it" is the answer;
    //      the generic import hint applies to every undefined symbol and adds no value.
    const bool isFunction =
        details.count(kUndefKindKey) && details.at(kUndefKindKey) == kUndefKindFunction;
    if (isFunction)
    {
        SmartSuggestion import;
        import.title = "\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x88\xD8\xB1\xD8\xAF \xD8\xA7\xD9\x84\xD9\x88\xD8\xAD\xD8\xAF\xD8\xA9 \xD8\xA7\xD9\x84\xD9\x85\xD8\xB7\xD9\x84\xD9\x88\xD8\xA8\xD8\xA9";
        import.titleEn = "Import required module";
        import.description = "\xD9\x82\xD8\xAF \xD9\x8A\xD9\x83\xD9\x88\xD9\x86 \xD8\xA7\xD9\x84\xD8\xB1\xD9\x85\xD8\xB2 \xD9\x85\xD9\x88\xD8\xAC\xD9\x88\xD8\xAF\xD8\xA7\xD9\x8B \xD9\x81\xD9\x8A \xD9\x88\xD8\xAD\xD8\xAF\xD8\xA9 \xD9\x84\xD9\x85 \xD8\xAA\xD8\xB3\xD8\xAA\xD9\x88\xD8\xB1\xD8\xAF\xD9\x87\xD8\xA7";
        import.descriptionEn = "Symbol may exist in an unimported module";
        import.codeExample = "\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x88\xD8\xB1\xD8\xAF \"\xD8\xA7\xD8\xB3\xD9\x85_\xD8\xA7\xD9\x84\xD9\x88\xD8\xAD\xD8\xAF\xD8\xA9\"";
        import.type = SuggestionType::INSERTION;
        import.confidence = ConfidenceLevel::LOW;
        suggestions.push_back(import);
    }
    
    return suggestions;
}

std::vector<SmartSuggestion> SuggestionEngine::generateSyntaxSuggestions(
    const std::unordered_map<std::string, std::string>& details,
    const std::string& context
) {
    std::vector<SmartSuggestion> suggestions;
    std::string expected = details.count("expected") ? details.at("expected") : "";
    std::string found = details.count("found") ? details.at("found") : "";
    
    if (expected.find("(") != std::string::npos || expected.find(")") != std::string::npos ||
        expected.find("{") != std::string::npos || expected.find("}") != std::string::npos) {
        SmartSuggestion brackets;
        brackets.title = "\xD8\xAA\xD8\xAD\xD9\x82\xD9\x82 \xD9\x85\xD9\x86 \xD8\xA7\xD9\x84\xD8\xA3\xD9\x82\xD9\x88\xD8\xA7\xD8\xB3";
        brackets.titleEn = "Check brackets";
        brackets.description = "\xD8\xAA\xD8\xA3\xD9\x83\xD8\xAF \xD9\x85\xD9\x86 \xD8\xA5\xD8\xBA\xD9\x84\xD8\xA7\xD9\x82 \xD8\xAC\xD9\x85\xD9\x8A\xD8\xB9 \xD8\xA7\xD9\x84\xD8\xA3\xD9\x82\xD9\x88\xD8\xA7\xD8\xB3 \xD8\xA7\xD9\x84\xD9\x85\xD9\x81\xD8\xAA\xD9\x88\xD8\xAD\xD8\xA9";
        brackets.descriptionEn = "Make sure all open brackets are closed";
        brackets.type = SuggestionType::HINT;
        brackets.confidence = ConfidenceLevel::HIGH;
        suggestions.push_back(brackets);
    }
    
    if (found == ";") {
        SmartSuggestion semicolon;
        semicolon.title = "\xD8\xA3\xD8\xB2\xD9\x84 \xD8\xA7\xD9\x84\xD9\x81\xD8\xA7\xD8\xB5\xD9\x84\xD8\xA9 \xD8\xA7\xD9\x84\xD9\x85\xD9\x86\xD9\x82\xD9\x88\xD8\xB7\xD8\xA9";
        semicolon.titleEn = "Remove semicolon";
        semicolon.description = "\xD9\x84\xD8\xBA\xD8\xA9 \xD8\xB3\xD8\xB9\xD8\xAF \xD9\x84\xD8\xA7 \xD8\xAA\xD8\xAD\xD8\xAA\xD8\xA7\xD8\xAC \xD9\x81\xD8\xA7\xD8\xB5\xD9\x84\xD8\xA9 \xD9\x85\xD9\x86\xD9\x82\xD9\x88\xD8\xB7\xD8\xA9";
        semicolon.descriptionEn = "Sad language doesn't need semicolons";
        semicolon.type = SuggestionType::DELETION;
        semicolon.confidence = ConfidenceLevel::HIGH;
        suggestions.push_back(semicolon);
    }
    
    return suggestions;
}

std::vector<SmartSuggestion> SuggestionEngine::generateImportSuggestions(
    const std::unordered_map<std::string, std::string>& details
) {
    std::vector<SmartSuggestion> suggestions;
    SmartSuggestion check;
    check.title = "\xD8\xAA\xD8\xAD\xD9\x82\xD9\x82 \xD9\x85\xD9\x86 \xD8\xA7\xD8\xB3\xD9\x85 \xD8\xA7\xD9\x84\xD9\x88\xD8\xAD\xD8\xAF\xD8\xA9";
    check.titleEn = "Check module name";
    check.description = "\xD8\xAA\xD8\xA3\xD9\x83\xD8\xAF \xD9\x85\xD9\x86 \xD9\x83\xD8\xAA\xD8\xA7\xD8\xA8\xD8\xA9 \xD8\xA7\xD8\xB3\xD9\x85 \xD8\xA7\xD9\x84\xD9\x88\xD8\xAD\xD8\xAF\xD8\xA9 \xD8\xA8\xD8\xB4\xD9\x83\xD9\x84 \xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD";
    check.descriptionEn = "Make sure module name is spelled correctly";
    check.type = SuggestionType::HINT;
    check.confidence = ConfidenceLevel::HIGH;
    suggestions.push_back(check);
    return suggestions;
}

std::vector<SmartSuggestion> SuggestionEngine::generateArgumentSuggestions(
    const std::unordered_map<std::string, std::string>& details
) {
    std::vector<SmartSuggestion> suggestions;
    std::string fn = details.count("function") ? details.at("function") : "";
    std::string exp = details.count("expected_count") ? details.at("expected_count") : "";
    std::string act = details.count("actual_count") ? details.at("actual_count") : "";
    
    SmartSuggestion fix;
    fix.title = "\xD8\xB5\xD8\xAD\xD9\x91\xD8\xAD \xD8\xB9\xD8\xAF\xD8\xAF \xD8\xA7\xD9\x84\xD9\x85\xD8\xB9\xD8\xA7\xD9\x85\xD9\x84\xD8\xA7\xD8\xAA";
    fix.titleEn = "Fix argument count";
    fix.description = "\xD8\xA7\xD9\x84\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 " + fn + " \xD8\xAA\xD8\xAA\xD9\x88\xD9\x82\xD8\xB9 " + exp + " \xD9\x85\xD8\xB9\xD8\xA7\xD9\x85\xD9\x84\xD8\xA7\xD8\xAA";
    fix.descriptionEn = "Function " + fn + " expects " + exp + " arguments, got " + act;
    fix.type = SuggestionType::REPLACEMENT;
    fix.confidence = ConfidenceLevel::HIGH;
    suggestions.push_back(fix);
    return suggestions;
}

std::vector<SmartSuggestion> SuggestionEngine::generateArabicEncodingSuggestions(
    const std::unordered_map<std::string, std::string>& details
) {
    std::vector<SmartSuggestion> suggestions;
    SmartSuggestion enc;
    enc.title = "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD9\x85 \xD8\xAA\xD8\xB1\xD9\x85\xD9\x8A\xD8\xB2 UTF-8";
    enc.titleEn = "Use UTF-8 encoding";
    enc.description = "\xD8\xAA\xD8\xA3\xD9\x83\xD8\xAF \xD9\x85\xD9\x86 \xD8\xAD\xD9\x81\xD8\xB8 \xD8\xA7\xD9\x84\xD9\x85\xD9\x84\xD9\x81 \xD8\xA8\xD8\xAA\xD8\xB1\xD9\x85\xD9\x8A\xD8\xB2 UTF-8";
    enc.descriptionEn = "Make sure file is saved with UTF-8 encoding";
    enc.type = SuggestionType::HINT;
    enc.confidence = ConfidenceLevel::HIGH;
    suggestions.push_back(enc);
    return suggestions;
}

std::vector<SmartSuggestion> SuggestionEngine::generateNamingSuggestions(
    const std::unordered_map<std::string, std::string>& details
) {
    std::vector<SmartSuggestion> suggestions;
    SmartSuggestion naming;
    naming.title = "\xD8\xA7\xD8\xAA\xD8\xA8\xD8\xB9 \xD8\xA3\xD8\xB3\xD9\x84\xD9\x88\xD8\xA8 \xD8\xA7\xD9\x84\xD8\xAA\xD8\xB3\xD9\x85\xD9\x8A\xD8\xA9";
    naming.titleEn = "Follow naming convention";
    naming.description = "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD9\x85 \xD8\xA3\xD8\xB3\xD9\x85\xD8\xA7\xD8\xA1 \xD9\x88\xD8\xA7\xD8\xB6\xD8\xAD\xD8\xA9 \xD9\x88\xD8\xB0\xD8\xA7\xD8\xAA \xD9\x85\xD8\xB9\xD9\x86\xD9\x89";
    naming.descriptionEn = "Use clear and meaningful names";
    naming.type = SuggestionType::HINT;
    naming.confidence = ConfidenceLevel::LOW;
    suggestions.push_back(naming);
    return suggestions;
}

} // namespace Errors
} // namespace Sad
