/**
 * @file cascade_prevention.h
 * @brief (AR) منع تسلسل الأخطاء
 *        (EN) Error cascade prevention system
 * 
 * (AR) المهمة: T165 - Phase 15 (US13)
 */

#pragma once

#include "source_location.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                    أنواع التسلسل | Cascade Types
// ═══════════════════════════════════════════════════════════════════════

enum class PrimaryErrorType {
    UNDEFINED_VARIABLE,
    UNDEFINED_FUNCTION,
    UNDEFINED_TYPE,
    UNDEFINED_CLASS,
    UNDEFINED_PROPERTY,
    UNDEFINED_METHOD,
    TYPE_MISMATCH,
    SYNTAX_ERROR,
    IMPORT_ERROR,
    ARGUMENT_ERROR
};

enum class CascadeReason {
    USES_UNDEFINED_SYMBOL,  ///< يستخدم رمزاً غير معرّف
    DEPENDS_ON_ERROR_TYPE,  ///< يعتمد على نوع خاطئ
    IN_ERROR_CONTEXT,       ///< داخل سياق خاطئ
    FOLLOWS_SYNTAX_ERROR,   ///< يتبع خطأ نحوي
    SAME_EXPRESSION         ///< في نفس التعبير
};

struct PrimaryError {
    PrimaryErrorType type;
    std::string symbol;
    SourceLocation location;
    size_t scopeDepth;
    
    PrimaryError() : type(PrimaryErrorType::SYNTAX_ERROR), scopeDepth(0) {}
};

struct CascadeInfo {
    bool isCascade;
    CascadeReason reason;
    size_t primaryErrorIndex;   ///< فهرس الخطأ الأساسي
    std::string explanation;
    std::string explanationEn;
    
    CascadeInfo()
        : isCascade(false)
        , reason(CascadeReason::USES_UNDEFINED_SYMBOL)
        , primaryErrorIndex(0) {}
};

// ═══════════════════════════════════════════════════════════════════════
//                  نظام منع التسلسل | Cascade Prevention
// ═══════════════════════════════════════════════════════════════════════

class CascadePrevention {
public:
    CascadePrevention();
    
    /**
     * @brief (AR) تسجيل خطأ أساسي
     */
    void registerPrimaryError(
        PrimaryErrorType type,
        const std::string& symbol,
        const SourceLocation& location,
        size_t scopeDepth = 0
    );
    
    /**
     * @brief (AR) فحص إذا كان الخطأ تسلسلياً
     */
    CascadeInfo checkCascade(
        PrimaryErrorType type,
        const std::string& symbol,
        const SourceLocation& location,
        size_t scopeDepth = 0
    );
    
    /**
     * @brief (AR) هل يجب عرض هذا الخطأ؟
     */
    bool shouldReport(
        PrimaryErrorType type,
        const std::string& symbol,
        const SourceLocation& location,
        size_t scopeDepth = 0
    );
    
    /// الإحصائيات
    size_t getPrimaryErrorCount() const;
    size_t getCascadeErrorCount() const;
    size_t getSuppressedCount() const;
    std::string formatStats(bool useArabic = true) const;
    
    /// الإدارة
    void reset();
    void setMaxCascadeDepth(size_t depth);    

private:
    std::vector<PrimaryError> primaryErrors_;
    std::unordered_set<std::string> undefinedSymbols_;
    std::unordered_set<std::string> errorScopes_;
    size_t cascadeCount_;
    size_t suppressedCount_;
    size_t maxCascadeDepth_;
    
    bool isUndefinedSymbolError(PrimaryErrorType type) const;
    bool isInErrorScope(const SourceLocation& loc, size_t depth) const;
};

} // namespace Errors
} // namespace Sad
