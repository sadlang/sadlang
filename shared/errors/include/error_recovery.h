/**
 * @file error_recovery.h
 * @brief (AR) استرداد أخطاء المحلل النحوي
 *        (EN) Parser error recovery system
 * 
 * (AR) المهمة: T163 - Phase 15 (US13)
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <functional>

namespace Sad {
namespace Errors {

// (AR) متغيّر بيئة يُظهر آثار الاسترداد الداخليّة (إحصائيّات + أفعال «🔧») للمطوّر.
//      الاسترداد آليّةُ مرونةٍ داخليّة للمحلّل لا شأن للمستخدم النهائيّ بها: التقرير
//      التشخيصيّ النهائيّ يكفيه. اضبطه لأيّ قيمة لتفعيل الآثار عند تشخيص المحلّل.
// (EN) Env var that surfaces internal recovery traces (stats + "🔧" actions) for devs.
//      Recovery is an internal parser-resilience mechanism, not user-facing: the final
//      diagnostic report suffices. Set it (any value) to enable traces when debugging.
inline constexpr const char* kDiagStatsEnvVar = "SAD_DIAG_STATS";

// ═══════════════════════════════════════════════════════════════════════
//                    استراتيجيات الاسترداد | Recovery Strategies
// ═══════════════════════════════════════════════════════════════════════

enum class RecoveryStrategyType {
    SKIP_TOKEN,         ///< تخطي الرمز الحالي
    INSERT_TOKEN,       ///< إدراج رمز مفقود
    SYNC_TO_DELIMITER,  ///< المزامنة مع محدد
    SYNC_TO_KEYWORD,    ///< المزامنة مع كلمة محجوزة
    SKIP_TO_END,        ///< تخطي حتى النهاية
    REPLACE_TOKEN       ///< استبدال الرمز الحالي
};

struct RecoveryAction {
    RecoveryStrategyType strategy;
    std::string description;
    std::string descriptionEn;
    std::string insertedText;   ///< النص المُدرج (إن وُجد)
    size_t tokensSkipped;       ///< عدد الرموز المتخطاة
    
    RecoveryAction()
        : strategy(RecoveryStrategyType::SKIP_TOKEN), tokensSkipped(0) {}
};

struct RecoveryResult {
    bool recovered;
    RecoveryAction action;
    std::string errorMessage;
    std::string errorMessageEn;
    size_t line;
    size_t column;
    
    RecoveryResult() : recovered(false), line(0), column(0) {}
};

// ═══════════════════════════════════════════════════════════════════════
//                  نظام الاسترداد | Recovery System
// ═══════════════════════════════════════════════════════════════════════

class ErrorRecoverySystem {
public:
    ErrorRecoverySystem();
    
    /**
     * @brief (AR) محاولة الاسترداد من خطأ
     */
    RecoveryResult tryRecover(
        const std::string& expected,
        const std::string& found,
        const std::string& context,
        size_t line, size_t column
    );

    /**
     * @brief (AR) الحصول على نقاط المزامنة 
     */
    const std::unordered_set<std::string>& getSyncPoints() const;
    
    /**
     * @brief (AR) إضافة نقطة مزامنة
     */
    void addSyncPoint(const std::string& token);

    /**
     * @brief (AR) الحصول على سجل الاسترداد
     */
    const std::vector<RecoveryResult>& getRecoveryLog() const;
    
    /**
     * @brief (AR) مسح السجل
     */
    void clearLog();
    
    /**
     * @brief (AR) إحصائيات الاسترداد
     */
    size_t getRecoveryCount() const;
    size_t getFailedRecoveryCount() const;
    std::string formatStats(bool useArabic = true) const;

private:
    std::unordered_set<std::string> syncPoints_;
    std::vector<RecoveryResult> recoveryLog_;
    size_t recoveryCount_;
    size_t failedCount_;
    
    void initializeSyncPoints();
    RecoveryResult tryInsertMissing(const std::string& expected, size_t line, size_t col);
    RecoveryResult trySyncToDelimiter(const std::string& context, size_t line, size_t col);
    RecoveryResult trySyncToKeyword(const std::string& context, size_t line, size_t col);
};

} // namespace Errors
} // namespace Sad
