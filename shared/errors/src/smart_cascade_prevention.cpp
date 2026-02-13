/**
 * @file smart_cascade_prevention.cpp
 * @brief (AR) تطبيق منع الأخطاء المتتالية - Sad::Errors
 * (AR) المهمة: T163 - Phase 15 (US13)
 */

#include "cascade_prevention.h"
#include <sstream>
#include <algorithm>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                       CascadePrevention
// ═══════════════════════════════════════════════════════════════════════

CascadePrevention::CascadePrevention()
    : cascadeCount_(0), suppressedCount_(0), maxCascadeDepth_(5) {}

void CascadePrevention::registerPrimaryError(
    PrimaryErrorType type,
    const std::string& symbol,
    const SourceLocation& location,
    size_t scopeDepth
) {
    PrimaryError err;
    err.type = type;
    err.symbol = symbol;
    err.location = location;
    err.scopeDepth = scopeDepth;
    primaryErrors_.push_back(err);
    
    if (isUndefinedSymbolError(type) && !symbol.empty()) {
        undefinedSymbols_.insert(symbol);
    }
    
    // Track error scope
    std::string scopeKey = location.filename + ":" + std::to_string(scopeDepth);
    errorScopes_.insert(scopeKey);
}

CascadeInfo CascadePrevention::checkCascade(
    PrimaryErrorType type,
    const std::string& symbol,
    const SourceLocation& location,
    size_t scopeDepth
) {
    CascadeInfo info;
    
    if (primaryErrors_.empty()) return info;
    
    // Check 1: Is this symbol already known as undefined?
    if (!symbol.empty() && undefinedSymbols_.count(symbol)) {
        info.isCascade = true;
        info.reason = CascadeReason::USES_UNDEFINED_SYMBOL;
        // Find matching primary
        for (size_t i = 0; i < primaryErrors_.size(); i++) {
            if (primaryErrors_[i].symbol == symbol) {
                info.primaryErrorIndex = i;
                break;
            }
        }
        info.explanation = "\xD9\x87\xD8\xB0\xD8\xA7 \xD8\xA7\xD9\x84\xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x86\xD8\xA7\xD8\xAA\xD8\xAC \xD8\xB9\xD9\x86 \xD8\xB1\xD9\x85\xD8\xB2 \xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x85\xD8\xB9\xD8\xB1\xD9\x91\xD9\x81: " + symbol;
        info.explanationEn = "This error is caused by undefined symbol: " + symbol;
        cascadeCount_++;
        return info;
    }
    
    // Check 2: Same scope as a previous error (follows syntax error)
    for (size_t i = 0; i < primaryErrors_.size(); i++) {
        const auto& primary = primaryErrors_[i];
        
        // Same file, close lines
        if (primary.location.filename == location.filename) {
            int lineDiff = static_cast<int>(location.line) - static_cast<int>(primary.location.line);
            
            if (lineDiff >= 0 && lineDiff <= 3 && primary.type == PrimaryErrorType::SYNTAX_ERROR) {
                info.isCascade = true;
                info.reason = CascadeReason::FOLLOWS_SYNTAX_ERROR;
                info.primaryErrorIndex = i;
                info.explanation = "\xD9\x87\xD8\xB0\xD8\xA7 \xD8\xA7\xD9\x84\xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x8A\xD8\xAA\xD8\xA8\xD8\xB9 \xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x86\xD8\xAD\xD9\x88\xD9\x8A\xD8\xA7\xD9\x8B \xD8\xB3\xD8\xA7\xD8\xA8\xD9\x82\xD8\xA7\xD9\x8B";
                info.explanationEn = "This error follows a previous syntax error";
                cascadeCount_++;
                return info;
            }
            
            // Same line, different error on same expression
            if (lineDiff == 0 && location.column > primary.location.column) {
                info.isCascade = true;
                info.reason = CascadeReason::SAME_EXPRESSION;
                info.primaryErrorIndex = i;
                info.explanation = "\xD9\x87\xD8\xB0\xD8\xA7 \xD8\xA7\xD9\x84\xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x81\xD9\x8A \xD9\x86\xD9\x81\xD8\xB3 \xD8\xA7\xD9\x84\xD8\xAA\xD8\xB9\xD8\xA8\xD9\x8A\xD8\xB1";
                info.explanationEn = "This error is in the same expression";
                cascadeCount_++;
                return info;
            }
        }
        
        // Type mismatch cascade
        if (primary.type == PrimaryErrorType::TYPE_MISMATCH &&
            type == PrimaryErrorType::TYPE_MISMATCH &&
            isInErrorScope(location, scopeDepth)) {
            info.isCascade = true;
            info.reason = CascadeReason::DEPENDS_ON_ERROR_TYPE;
            info.primaryErrorIndex = i;
            info.explanation = "\xD9\x87\xD8\xB0\xD8\xA7 \xD8\xA7\xD9\x84\xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x8A\xD8\xB9\xD8\xAA\xD9\x85\xD8\xAF \xD8\xB9\xD9\x84\xD9\x89 \xD9\x86\xD9\x88\xD8\xB9 \xD8\xAE\xD8\xA7\xD8\xB7\xD8\xA6";
            info.explanationEn = "This error depends on a wrong type";
            cascadeCount_++;
            return info;
        }
    }
    
    return info;
}

bool CascadePrevention::shouldReport(
    PrimaryErrorType type,
    const std::string& symbol,
    const SourceLocation& location,
    size_t scopeDepth
) {
    CascadeInfo info = checkCascade(type, symbol, location, scopeDepth);
    if (info.isCascade) {
        suppressedCount_++;
        return false;
    }
    return true;
}

size_t CascadePrevention::getPrimaryErrorCount() const { return primaryErrors_.size(); }
size_t CascadePrevention::getCascadeErrorCount() const { return cascadeCount_; }
size_t CascadePrevention::getSuppressedCount() const { return suppressedCount_; }

void CascadePrevention::setMaxCascadeDepth(size_t depth) { maxCascadeDepth_ = depth; }

void CascadePrevention::reset() {
    primaryErrors_.clear();
    undefinedSymbols_.clear();
    errorScopes_.clear();
    cascadeCount_ = 0;
    suppressedCount_ = 0;
}

std::string CascadePrevention::formatStats(bool useArabic) const {
    std::ostringstream oss;
    if (useArabic) {
        oss << "\n=== \xD8\xA5\xD8\xAD\xD8\xB5\xD8\xA7\xD8\xA6\xD9\x8A\xD8\xA7\xD8\xAA \xD9\x85\xD9\x86\xD8\xB9 \xD8\xA7\xD9\x84\xD8\xAA\xD8\xAA\xD8\xA7\xD9\x84\xD9\x8A ===\n";
        oss << "   \xD8\xA3\xD8\xAE\xD8\xB7\xD8\xA7\xD8\xA1 \xD8\xA3\xD8\xB3\xD8\xA7\xD8\xB3\xD9\x8A\xD8\xA9: " << primaryErrors_.size() << "\n";
        oss << "   \xD8\xAA\xD8\xB3\xD9\x84\xD8\xB3\xD9\x84\xD9\x8A\xD8\xA9: " << cascadeCount_ << "\n";
        oss << "   \xD8\xAA\xD9\x85 \xD8\xA7\xD9\x84\xD9\x83\xD8\xA8\xD8\xAA: " << suppressedCount_ << "\n";
    } else {
        oss << "\n=== Cascade Prevention Stats ===\n";
        oss << "   Primary errors: " << primaryErrors_.size() << "\n";
        oss << "   Cascade errors: " << cascadeCount_ << "\n";
        oss << "   Suppressed: " << suppressedCount_ << "\n";
    }
    return oss.str();
}

// ─── Private helpers ──────────────────────────────────────────────────

bool CascadePrevention::isUndefinedSymbolError(PrimaryErrorType type) const {
    return type == PrimaryErrorType::UNDEFINED_VARIABLE ||
           type == PrimaryErrorType::UNDEFINED_FUNCTION ||
           type == PrimaryErrorType::UNDEFINED_TYPE ||
           type == PrimaryErrorType::UNDEFINED_CLASS ||
           type == PrimaryErrorType::UNDEFINED_PROPERTY ||
           type == PrimaryErrorType::UNDEFINED_METHOD;
}

bool CascadePrevention::isInErrorScope(const SourceLocation& loc, size_t depth) const {
    std::string scopeKey = loc.filename + ":" + std::to_string(depth);
    return errorScopes_.count(scopeKey) > 0;
}

} // namespace Errors
} // namespace Sad
