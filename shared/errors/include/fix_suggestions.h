/**
 * @file fix_suggestions.h
 * @brief (AR) اقتراحات الإصلاح للأخطاء الشائعة
 *        (EN) Fix suggestions for common errors
 * 
 * (AR) المهمة: T166 - Phase 15 (US13)
 */

#pragma once

#include "error_codes.h"
#include "diagnostic.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                    أنواع الإصلاح | Fix Types
// ═══════════════════════════════════════════════════════════════════════

enum class FixType {
    QUICK_FIX,      ///< إصلاح سريع
    SUGGESTION,     ///< اقتراح
    REFACTOR,       ///< إعادة هيكلة
    LEARNING,       ///< تعليمي
    WARNING_FIX     ///< إصلاح تحذير
};

enum class FixPriority {
    CRITICAL,   ///< حرج
    HIGH,       ///< عالي
    MEDIUM,     ///< متوسط
    LOW         ///< منخفض
};

struct TextEdit {
    size_t startLine;
    size_t startColumn;
    size_t endLine;
    size_t endColumn;
    std::string newText;
    
    TextEdit() : startLine(0), startColumn(0), endLine(0), endColumn(0) {}
    TextEdit(size_t sl, size_t sc, size_t el, size_t ec, const std::string& text)
        : startLine(sl), startColumn(sc), endLine(el), endColumn(ec), newText(text) {}
};

struct FixSuggestionItem {
    std::string title;
    std::string titleEn;
    std::string description;
    std::string descriptionEn;
    FixType type;
    FixPriority priority;
    std::vector<TextEdit> edits;
    std::string codeExample;
    std::string explanation;        ///< شرح للمبتدئين
    
    FixSuggestionItem()
        : type(FixType::SUGGESTION), priority(FixPriority::MEDIUM) {}
    
    std::string format(bool useArabic = true) const;
};

// ═══════════════════════════════════════════════════════════════════════
//                 مدير الإصلاحات | Fix Suggestion Manager
// ═══════════════════════════════════════════════════════════════════════

class FixSuggestionManager {
public:
    FixSuggestionManager();
    
    /**
     * @brief (AR) الحصول على إصلاحات لرمز خطأ
     */
    std::vector<FixSuggestionItem> getFixSuggestions(
        ErrorCode code,
        const std::string& context,
        const std::unordered_map<std::string, std::string>& details
    );
    
    /**
     * @brief (AR) الحصول على إصلاحات لخطأ نحوي
     */    
    std::vector<FixSuggestionItem> getSyntaxFixes(
        const std::string& expected,
        const std::string& found,
        const std::string& context
    );

    /**
     * @brief (AR) الحصول على إصلاحات لخطأ نوع
     */
    std::vector<FixSuggestionItem> getTypeFixes(
        const std::string& expectedType,
        const std::string& actualType,
        const std::string& expression
    );

    /**
     * @brief (AR) تطبيق أنماط الإصلاح الشائعة
     */
    std::vector<FixSuggestionItem> getCommonFixes(
        const std::string& errorCode,
        const std::string& context
    );

    /**
     * @brief (AR) تحويل إلى FixItHint متوافق مع Diagnostic
     */
    std::vector<FixItHint> toFixItHints(const std::vector<FixSuggestionItem>& fixes);

private:
    void initializeCommonPatterns();
    
    using FixGenerator = std::function<std::vector<FixSuggestionItem>(
        const std::string&, const std::unordered_map<std::string, std::string>&)>;
    
    std::unordered_map<std::string, FixGenerator> fixGenerators_;
    std::unordered_map<std::string, std::vector<std::string>> commonPatterns_;
};

} // namespace Errors
} // namespace Sad
