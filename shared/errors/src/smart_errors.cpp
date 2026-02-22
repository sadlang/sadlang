/**
 * @file smart_errors.cpp
 * @brief (AR) تطبيق النظام الرئيسي للأخطاء الذكية - Sad::Errors
 * (AR) المهمة: T163 - Phase 15 (US13)
 * 
 * SmartErrorSystem - unified entry point: enrichDiagnostic, printStats
 * Most accessors are inline in the header; only enrichDiagnostic & printStats here.
 */

#include "smart_errors.h"
#include <sstream>
#include <iostream>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                  enrichDiagnostic
// ═══════════════════════════════════════════════════════════════════════

Diagnostic SmartErrorSystem::enrichDiagnostic(
    const Diagnostic& diag,
    const std::string& sourceCode,
    const std::string& context
) {
    Diagnostic enriched = diag; // copy
    if (!enabled_) return enriched;
    
    ErrorCode code = diag.getCode();
    std::string codeStr = getErrorCodeString(code);
    std::string messageAr = diag.getMessage(Language::ARABIC);
    std::string messageEn = diag.getMessage(Language::ENGLISH);
    const SourceLocation& loc = diag.getLocation();
    
    // Determine primary error type
    PrimaryErrorType primaryType = PrimaryErrorType::SYNTAX_ERROR;
    if (codeStr.find("SEM_UNDEF") != std::string::npos)
        primaryType = PrimaryErrorType::UNDEFINED_VARIABLE;
    else if (codeStr.find("SEM_TYPE") != std::string::npos)
        primaryType = PrimaryErrorType::TYPE_MISMATCH;
    else if (codeStr.find("SYN") != std::string::npos || codeStr.find("LEX") != std::string::npos)
        primaryType = PrimaryErrorType::SYNTAX_ERROR;
    else if (codeStr.find("IMPORT") != std::string::npos)
        primaryType = PrimaryErrorType::IMPORT_ERROR;
    else if (codeStr.find("ARG") != std::string::npos)
        primaryType = PrimaryErrorType::ARGUMENT_ERROR;
    
    // Extract symbol name from message (heuristic: first quoted string)
    std::string symbol;
    auto qPos = messageAr.find('\'');
    if (qPos == std::string::npos) qPos = messageEn.find('\'');
    if (qPos != std::string::npos) {
        const std::string& msg = (messageAr.find('\'') != std::string::npos) ? messageAr : messageEn;
        auto qEnd = msg.find('\'', qPos + 1);
        if (qEnd != std::string::npos)
            symbol = msg.substr(qPos + 1, qEnd - qPos - 1);
    }
    
    // 1) Cascade check
    CascadeInfo cascadeInfo = cascadePrevention_.checkCascade(
        primaryType, symbol, loc
    );
    if (cascadeInfo.isCascade) {
        // Add cascade note and return without further enrichment
        Diagnostic cascadeNote(
            code,
            DiagnosticSeverity::NOTE,
            loc,
            cascadeInfo.explanation,
            cascadeInfo.explanationEn
        );
        enriched.addNote(cascadeNote);
        return enriched;
    }
    
    // Register as primary for cascade detection
    cascadePrevention_.registerPrimaryError(primaryType, symbol, loc);
    
    // 2) Suggestions from ErrorCode
    std::unordered_map<std::string, std::string> details;
    if (!symbol.empty()) details["symbol"] = symbol;
    if (!sourceCode.empty()) details["source"] = sourceCode;
    
    auto suggestions = suggestionEngine_.generateFromErrorCode(code, context, details);
    for (const auto& sg : suggestions) {
        std::string noteMsg = "\xF0\x9F\x92\xA1 " + sg.title;
        if (sg.confidence == ConfidenceLevel::HIGH)
            noteMsg += " \xE2\x9C\x93";
        Diagnostic suggNote(
            code,
            DiagnosticSeverity::HINT,
            loc,
            noteMsg,
            "\xF0\x9F\x92\xA1 " + sg.titleEn
        );
        enriched.addNote(suggNote);
    }
    
    // 3) Fix suggestions → FixItHints
    auto fixes = fixSuggestions_.getFixSuggestions(code, context, details);
    auto hints = fixSuggestions_.toFixItHints(fixes);
    for (const auto& h : hints) {
        enriched.addFixIt(h);
    }
    
    // 4) Teacher mode explanation
    if (teacherMode_.getLevel() != ExplanationLevel::BRIEF) {
        std::string explanation = teacherMode_.explainError(
            codeStr, messageAr, context, details
        );
        if (!explanation.empty()) {
            Diagnostic teachNote(
                code,
                DiagnosticSeverity::NOTE,
                loc,
                explanation,
                explanation
            );
            enriched.addNote(teachNote);
        }
    }
    
    // 5) Collect enriched diagnostic in multi-error
    multiError_.addFromDiagnostic(enriched);
    
    return enriched;
}

// ═══════════════════════════════════════════════════════════════════════
//                       printStats
// ═══════════════════════════════════════════════════════════════════════

void SmartErrorSystem::printStats() const {
    std::ostringstream oss;
    oss << "\n\xE2\x95\x90\xE2\x95\x90\xE2\x95\x90 \xD8\xA5\xD8\xAD\xD8\xB5\xD8\xA7\xD8\xA6\xD9\x8A\xD8\xA7\xD8\xAA \xD9\x86\xD8\xB8\xD8\xA7\xD9\x85 \xD8\xA7\xD9\x84\xD8\xA3\xD8\xAE\xD8\xB7\xD8\xA7\xD8\xA1 \xD8\xA7\xD9\x84\xD8\xB0\xD9\x83\xD9\x8A\xD8\xA9 \xE2\x95\x90\xE2\x95\x90\xE2\x95\x90\n";
    oss << multiError_.getSummary(true);
    oss << cascadePrevention_.formatStats(true);
    std::cerr << oss.str();
}

} // namespace Errors
} // namespace Sad
