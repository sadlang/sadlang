/**
 * @file teacher_mode.h
 * @brief (AR) وضع المعلم - شروحات تعليمية مفصلة
 *        (EN) Teacher Mode - Detailed educational explanations
 * 
 * (AR) المهمة: T167 - Phase 15 (US13)
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                    مستويات الشرح | Explanation Levels
// ═══════════════════════════════════════════════════════════════════════

enum class ExplanationLevel {
    BRIEF,      ///< موجز - للخبراء
    NORMAL,     ///< عادي - رسالة + اقتراحات
    DETAILED,   ///< مفصل - رسالة + شرح + أمثلة
    TEACHER     ///< معلم - كل شيء + تشبيهات
};

// ═══════════════════════════════════════════════════════════════════════
//                    محتوى تعليمي | Educational Content
// ═══════════════════════════════════════════════════════════════════════

struct ConceptExplanation {
    std::string concept;
    std::string conceptEn;
    std::string simpleExplanation;
    std::string analogy;
    std::string codeExample;
    std::vector<std::string> keyPoints;
    std::vector<std::string> commonMistakes;
    std::string furtherReading;
    
    std::string format(bool useArabic = true) const;
};

// ═══════════════════════════════════════════════════════════════════════
//                  قاعدة المفاهيم | Concepts Database
// ═══════════════════════════════════════════════════════════════════════

class ConceptsDatabase {
public:
    ConceptsDatabase();
    const ConceptExplanation* getConcept(const std::string& key) const;
    
private:
    std::unordered_map<std::string, ConceptExplanation> concepts_;
    void initializeConcepts();
};

// ═══════════════════════════════════════════════════════════════════════
//                     وضع المعلم | Teacher Mode
// ═══════════════════════════════════════════════════════════════════════

class TeacherMode {
public:
    TeacherMode();
    
    /// (AR) تعيين مستوى الشرح
    void setLevel(ExplanationLevel level);
    ExplanationLevel getLevel() const;
    
    /**
     * @brief (AR) توليد شرح لخطأ
     */
    std::string explainError(
        const std::string& errorCode,
        const std::string& errorMessage,
        const std::string& context = "",
        const std::unordered_map<std::string, std::string>& details = {}
    );
    
    /**
     * @brief (AR) الحصول على شرح مفهوم
     */
    std::string getConceptExplanation(const std::string& conceptKey);
    
    /**
     * @brief (AR) الحصول على مستويات الشرح المتاحة
     */
    static std::string getLevelDescription(ExplanationLevel level, bool arabic = true);
    
private:
    ExplanationLevel level_;
    ConceptsDatabase db_;
    
    std::string generateBasicExplanation(
        const std::string& errorCode,
        const std::unordered_map<std::string, std::string>& details);
    std::string generateDetailedExplanation(
        const std::string& errorCode,
        const std::string& context,
        const std::unordered_map<std::string, std::string>& details);
    std::string generateTeacherExplanation(
        const std::string& errorCode,
        const std::string& context,
        const std::unordered_map<std::string, std::string>& details);
};

} // namespace Errors
} // namespace Sad
