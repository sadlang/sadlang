/**
 * @file smart_error_recovery.cpp
 * @brief (AR) تطبيق استرداد الأخطاء - Sad::Errors
 * (AR) المهمة: T163 - Phase 15 (US13)
 */

#include "error_recovery.h"
#include <sstream>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                    ErrorRecoverySystem
// ═══════════════════════════════════════════════════════════════════════

ErrorRecoverySystem::ErrorRecoverySystem()
    : recoveryCount_(0), failedCount_(0) {
    initializeSyncPoints();
}

void ErrorRecoverySystem::initializeSyncPoints() {
    // Arabic keywords as sync points
    syncPoints_.insert("\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9");      // دالة
    syncPoints_.insert("\xD8\xB5\xD9\x86\xD9\x81");              // صنف
    syncPoints_.insert("\xD8\xA5\xD8\xB0\xD8\xA7");              // إذا
    syncPoints_.insert("\xD9\x88\xD8\xA5\xD9\x84\xD8\xA7");      // وإلا
    syncPoints_.insert("\xD8\xB7\xD8\xA7\xD9\x84\xD9\x85\xD8\xA7");  // طالما
    syncPoints_.insert("\xD9\x84\xD9\x83\xD9\x84");              // لكل
    syncPoints_.insert("\xD8\xA3\xD8\xB1\xD8\xAC\xD8\xB9");      // أرجع
    syncPoints_.insert("\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1");  // متغير
    syncPoints_.insert("\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA");      // ثابت
    syncPoints_.insert("\xD8\xA7\xD8\xB3\xD8\xAA\xD9\x88\xD8\xB1\xD8\xAF");  // استورد
    syncPoints_.insert("\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9");  // نهاية
    syncPoints_.insert("\xD8\xB7\xD8\xA7\xD8\xA8\xD9\x82");      // طابق
    // Delimiters
    syncPoints_.insert("{");
    syncPoints_.insert("}");
    syncPoints_.insert(";");
    syncPoints_.insert("\n");
}

RecoveryResult ErrorRecoverySystem::tryRecover(
    const std::string& expected,
    const std::string& found,
    const std::string& context,
    size_t line, size_t column
) {
    RecoveryResult result;
    result.line = line;
    result.column = column;
    
    // Strategy 1: Try inserting expected token
    result = tryInsertMissing(expected, line, column);
    if (result.recovered) {
        recoveryCount_++;
        recoveryLog_.push_back(result);
        return result;
    }
    
    // Strategy 2: Try syncing to delimiter
    result = trySyncToDelimiter(context, line, column);
    if (result.recovered) {
        recoveryCount_++;
        recoveryLog_.push_back(result);
        return result;
    }
    
    // Strategy 3: Try syncing to keyword
    result = trySyncToKeyword(context, line, column);
    if (result.recovered) {
        recoveryCount_++;
        recoveryLog_.push_back(result);
        return result;
    }
    
    // Failed to recover
    failedCount_++;
    result.recovered = false;
    result.errorMessage = "\xD9\x84\xD9\x85 \xD9\x8A\xD8\xAA\xD9\x85\xD9\x83\xD9\x86 \xD9\x85\xD9\x86 \xD8\xA7\xD9\x84\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xB1\xD8\xAF\xD8\xA7\xD8\xAF";
    result.errorMessageEn = "Could not recover";
    recoveryLog_.push_back(result);
    return result;
}

const std::unordered_set<std::string>& ErrorRecoverySystem::getSyncPoints() const {
    return syncPoints_;
}

void ErrorRecoverySystem::addSyncPoint(const std::string& token) {
    syncPoints_.insert(token);
}

const std::vector<RecoveryResult>& ErrorRecoverySystem::getRecoveryLog() const {
    return recoveryLog_;
}

void ErrorRecoverySystem::clearLog() {
    recoveryLog_.clear();
    recoveryCount_ = 0;
    failedCount_ = 0;
}

size_t ErrorRecoverySystem::getRecoveryCount() const { return recoveryCount_; }
size_t ErrorRecoverySystem::getFailedRecoveryCount() const { return failedCount_; }

std::string ErrorRecoverySystem::formatStats(bool useArabic) const {
    std::ostringstream oss;
    if (useArabic) {
        oss << "\n=== \xD8\xA5\xD8\xAD\xD8\xB5\xD8\xA7\xD8\xA6\xD9\x8A\xD8\xA7\xD8\xAA \xD8\xA7\xD9\x84\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xB1\xD8\xAF\xD8\xA7\xD8\xAF ===\n";
        oss << "   \xD9\x86\xD8\xA7\xD8\xAC\xD8\xAD: " << recoveryCount_ << "\n";
        oss << "   \xD9\x81\xD8\xA7\xD8\xB4\xD9\x84: " << failedCount_ << "\n";
    } else {
        oss << "\n=== Recovery Stats ===\n";
        oss << "   Successful: " << recoveryCount_ << "\n";
        oss << "   Failed: " << failedCount_ << "\n";
    }
    return oss.str();
}

RecoveryResult ErrorRecoverySystem::tryInsertMissing(
    const std::string& expected, size_t line, size_t col
) {
    RecoveryResult result;
    result.line = line;
    result.column = col;
    
    // (AR) الرموز القابلة للإدراج التلقائي
    // (EN) Tokens that can be auto-inserted
    static const std::unordered_set<std::string> insertable = {
        ")", "(", "}", "{", "]", "[", ":", ";",
        "=", ",", "->",
        "\xD9\x86\xD9\x87\xD8\xA7\xD9\x8A\xD8\xA9"  // نهاية
    };
    
    if (insertable.count(expected) > 0) {
        result.recovered = true;
        result.action.strategy = RecoveryStrategyType::INSERT_TOKEN;
        result.action.insertedText = expected;
        result.action.description = "\xD8\xA5\xD8\xAF\xD8\xB1\xD8\xA7\xD8\xAC '" + expected + "' \xD8\xA7\xD9\x84\xD9\x85\xD9\x81\xD9\x82\xD9\x88\xD8\xAF\xD8\xA9";
        result.action.descriptionEn = "Inserted missing '" + expected + "'";
        result.errorMessage = "\xD8\xAA\xD9\x85 \xD8\xA5\xD8\xAF\xD8\xB1\xD8\xA7\xD8\xAC '" + expected + "'";
        result.errorMessageEn = "Inserted '" + expected + "'";
    }
    
    return result;
}

RecoveryResult ErrorRecoverySystem::trySyncToDelimiter(
    const std::string& context, size_t line, size_t col
) {
    RecoveryResult result;
    result.line = line;
    result.column = col;
    
    // Check if we can find a delimiter in context
    for (const auto& delim : {"\n", ";", "}", ")"}) {
        if (context.find(delim) != std::string::npos) {
            result.recovered = true;
            result.action.strategy = RecoveryStrategyType::SYNC_TO_DELIMITER;
            result.action.description = "\xD9\x85\xD8\xB2\xD8\xA7\xD9\x85\xD9\x86\xD8\xA9 \xD9\x85\xD8\xB9 \xD8\xA7\xD9\x84\xD9\x85\xD8\xAD\xD8\xAF\xD8\xAF";
            result.action.descriptionEn = "Synced to delimiter";
            result.errorMessage = "\xD8\xAA\xD9\x85 \xD8\xA7\xD9\x84\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xB1\xD8\xAF\xD8\xA7\xD8\xAF \xD8\xB9\xD9\x86\xD8\xAF \xD8\xA7\xD9\x84\xD9\x85\xD8\xAD\xD8\xAF\xD8\xAF";
            result.errorMessageEn = "Recovered at delimiter";
            return result;
        }
    }
    
    return result;
}

RecoveryResult ErrorRecoverySystem::trySyncToKeyword(
    const std::string& context, size_t line, size_t col
) {
    RecoveryResult result;
    result.line = line;
    result.column = col;
    
    for (const auto& keyword : syncPoints_) {
        if (context.find(keyword) != std::string::npos) {
            result.recovered = true;
            result.action.strategy = RecoveryStrategyType::SYNC_TO_KEYWORD;
            result.action.description = "\xD9\x85\xD8\xB2\xD8\xA7\xD9\x85\xD9\x86\xD8\xA9 \xD9\x85\xD8\xB9 \xD9\x83\xD9\x84\xD9\x85\xD8\xA9 \xD9\x85\xD8\xAD\xD8\xAC\xD9\x88\xD8\xB2\xD8\xA9";
            result.action.descriptionEn = "Synced to keyword";
            result.errorMessage = "\xD8\xAA\xD9\x85 \xD8\xA7\xD9\x84\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xB1\xD8\xAF\xD8\xA7\xD8\xAF";
            result.errorMessageEn = "Recovered at keyword";
            return result;
        }
    }
    
    return result;
}

} // namespace Errors
} // namespace Sad
