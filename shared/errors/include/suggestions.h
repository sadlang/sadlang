/**
 * @file suggestions.h
 * @brief (AR) نظام اقتراحات الإصلاح الذكي
 *        (EN) Smart Fix Suggestion System
 * 
 * (AR) المهمة: T161 - Phase 15 (US13)
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
//                    أنواع الاقتراحات | Suggestion Types
// ═══════════════════════════════════════════════════════════════════════

enum class SuggestionType {
    REPLACEMENT,  ///< استبدال
    INSERTION,    ///< إضافة
    DELETION,     ///< حذف
    REFACTOR,     ///< إعادة هيكلة
    HINT          ///< تلميح فقط
};

enum class ConfidenceLevel {
    HIGH,    ///< ثقة عالية (90%+)
    MEDIUM,  ///< ثقة متوسطة (70%+)
    LOW      ///< ثقة منخفضة
};

enum class SmartErrorCategory {
    TYPE_MISMATCH,      ///< عدم تطابق الأنواع
    UNDEFINED_SYMBOL,   ///< رمز غير معرّف
    SYNTAX_ERROR,       ///< خطأ نحوي
    IMPORT_ERROR,       ///< خطأ استيراد
    SCOPE_ERROR,        ///< خطأ نطاق
    ARGUMENT_ERROR,     ///< خطأ معاملات
    ARABIC_ENCODING,    ///< خطأ ترميز عربي
    NAMING_CONVENTION,  ///< خطأ تسمية
    LOGIC_ERROR,        ///< خطأ منطقي
    PERFORMANCE_HINT    ///< تلميح أداء
};

// ═══════════════════════════════════════════════════════════════════════
//                       هياكل البيانات | Data Structures
// ═══════════════════════════════════════════════════════════════════════

struct SuggestedEdit {
    SourceLocation location;
    std::string oldText;
    std::string newText;
    
    SuggestedEdit() = default;
    SuggestedEdit(const SourceLocation& loc, const std::string& oldT, const std::string& newT)
        : location(loc), oldText(oldT), newText(newT) {}
};

struct SmartSuggestion {
    std::string title;              ///< عنوان بالعربية
    std::string titleEn;            ///< عنوان بالإنجليزية
    std::string description;        ///< شرح بالعربية
    std::string descriptionEn;      ///< شرح بالإنجليزية
    SuggestionType type;
    ConfidenceLevel confidence;
    std::vector<SuggestedEdit> edits;
    std::string codeExample;        ///< مثال كود صحيح
    
    SmartSuggestion()
        : type(SuggestionType::HINT), confidence(ConfidenceLevel::MEDIUM) {}
    
    std::string format(bool useArabic = true) const;
};

// ═══════════════════════════════════════════════════════════════════════
//                     مولد الاقتراحات | Suggestion Engine
// ═══════════════════════════════════════════════════════════════════════

class SuggestionEngine {
public:
    SuggestionEngine();
    
    /**
     * @brief (AR) توليد اقتراحات لخطأ
     */
    std::vector<SmartSuggestion> generateSuggestions(
        SmartErrorCategory category,
        const std::string& errorCode,
        const std::string& context,
        const std::unordered_map<std::string, std::string>& details
    );

    /**
     * @brief (AR) توليد اقتراحات من ErrorCode
     */
    std::vector<SmartSuggestion> generateFromErrorCode(
        ErrorCode code,
        const std::string& context,
        const std::unordered_map<std::string, std::string>& details
    );

    /**
     * @brief (AR) بحث عن رموز مشابهة (تصحيح إملائي)
     */
    std::vector<std::string> findSimilarSymbols(
        const std::string& symbol,
        const std::vector<std::string>& available
    );

private:
    void initializePatterns();
    
    std::vector<SmartSuggestion> generateTypeMismatchSuggestions(
        const std::unordered_map<std::string, std::string>& details);
    std::vector<SmartSuggestion> generateUndefinedSymbolSuggestions(
        const std::unordered_map<std::string, std::string>& details,
        const std::string& context);
    std::vector<SmartSuggestion> generateSyntaxSuggestions(
        const std::unordered_map<std::string, std::string>& details,
        const std::string& context);
    std::vector<SmartSuggestion> generateImportSuggestions(
        const std::unordered_map<std::string, std::string>& details);
    std::vector<SmartSuggestion> generateArgumentSuggestions(
        const std::unordered_map<std::string, std::string>& details);
    std::vector<SmartSuggestion> generateArabicEncodingSuggestions(
        const std::unordered_map<std::string, std::string>& details);
    std::vector<SmartSuggestion> generateNamingSuggestions(
        const std::unordered_map<std::string, std::string>& details);
    
    size_t levenshteinDistance(const std::string& a, const std::string& b);
    
    std::unordered_map<std::string, std::vector<std::string>> commonPatterns_;
};

} // namespace Errors
} // namespace Sad
