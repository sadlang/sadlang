/**
 * @file smart_teacher_mode.cpp
 * @brief (AR) تطبيق وضع المعلم - Sad::Errors
 * (AR) المهمة: T167 - Phase 15 (US13)
 */

#include "teacher_mode.h"
#include <sstream>

namespace Sad {
namespace Errors {

// ═══════════════════════════════════════════════════════════════════════
//                    ConceptExplanation::format
// ═══════════════════════════════════════════════════════════════════════

std::string ConceptExplanation::format(bool useArabic) const {
    std::ostringstream oss;
    
    oss << "\n\xF0\x9F\x93\x9A " << (useArabic ? conceptAr : conceptEn) << "\n";
    oss << "   ====================================================\n";
    
    if (!simpleExplanation.empty()) {
        oss << "\n   \xF0\x9F\x93\x96 " << (useArabic ? "\xD8\xA7\xD9\x84\xD8\xB4\xD8\xB1\xD8\xAD:" : "Explanation:") << "\n";
        oss << "   ----\n";
        oss << "   " << simpleExplanation << "\n";
    }
    
    if (!analogy.empty()) {
        oss << "\n   \xF0\x9F\x8E\xAF " << (useArabic ? "\xD8\xAA\xD8\xB4\xD8\xA8\xD9\x8A\xD9\x87:" : "Analogy:") << "\n";
        oss << "   ----\n";
        oss << "   " << analogy << "\n";
    }
    
    if (!codeExample.empty()) {
        oss << "\n   \xF0\x9F\x92\xBB " << (useArabic ? "\xD9\x85\xD8\xAB\xD8\xA7\xD9\x84:" : "Example:") << "\n";
        oss << "   ----\n";
        std::istringstream stream(codeExample);
        std::string line;
        while (std::getline(stream, line)) {
            oss << "   " << line << "\n";
        }
    }
    
    if (!keyPoints.empty()) {
        oss << "\n   \xE2\xAD\x90 " << (useArabic ? "\xD9\x86\xD9\x82\xD8\xA7\xD8\xB7 \xD9\x85\xD9\x87\xD9\x85\xD8\xA9:" : "Key Points:") << "\n";
        for (const auto& p : keyPoints) {
            oss << "   \xE2\x80\xA2 " << p << "\n";
        }
    }
    
    if (!commonMistakes.empty()) {
        oss << "\n   \xE2\x9A\xA0\xEF\xB8\x8F " << (useArabic ? "\xD8\xA3\xD8\xAE\xD8\xB7\xD8\xA7\xD8\xA1 \xD8\xB4\xD8\xA7\xD8\xA6\xD8\xB9\xD8\xA9:" : "Common Mistakes:") << "\n";
        for (const auto& m : commonMistakes) {
            oss << "   \xE2\x9C\x97 " << m << "\n";
        }
    }
    
    return oss.str();
}

// ═══════════════════════════════════════════════════════════════════════
//                    ConceptsDatabase
// ═══════════════════════════════════════════════════════════════════════

ConceptsDatabase::ConceptsDatabase() { initializeConcepts(); }

const ConceptExplanation* ConceptsDatabase::getConcept(const std::string& key) const {
    auto it = concepts_.find(key);
    return it != concepts_.end() ? &it->second : nullptr;
}

void ConceptsDatabase::initializeConcepts() {
    // Types concept
    {
        ConceptExplanation c;
        c.conceptAr = "\xD8\xA7\xD9\x84\xD8\xA3\xD9\x86\xD9\x88\xD8\xA7\xD8\xB9 (Types)";
        c.conceptEn = "Types";
        c.simpleExplanation = "\xD9\x81\xD9\x8A \xD8\xA7\xD9\x84\xD8\xA8\xD8\xB1\xD9\x85\xD8\xAC\xD8\xA9\xD8\x8C \xD9\x83\xD9\x84 \xD9\x82\xD9\x8A\xD9\x85\xD8\xA9 \xD9\x84\xD9\x87\xD8\xA7 \xD9\x86\xD9\x88\xD8\xB9 \xD9\x8A\xD8\xAD\xD8\xAF\xD8\xAF \xD8\xB7\xD8\xA8\xD9\x8A\xD8\xB9\xD8\xAA\xD9\x87\xD8\xA7.";
        c.analogy = "\xD8\xAA\xD8\xAE\xD9\x8A\xD9\x84 \xD8\xA3\xD9\x86\xD9\x88\xD8\xA7\xD8\xB9 \xD8\xA7\xD9\x84\xD8\xA8\xD9\x8A\xD8\xA7\xD9\x86\xD8\xA7\xD8\xAA \xD9\x83\xD8\xA3\xD9\x86\xD9\x88\xD8\xA7\xD8\xB9 \xD8\xA7\xD9\x84\xD8\xAD\xD8\xA7\xD9\x88\xD9\x8A\xD8\xA7\xD8\xAA";
        c.keyPoints = {
            "\xD9\x83\xD9\x84 \xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x84\xD9\x87 \xD9\x86\xD9\x88\xD8\xB9 \xD9\x85\xD8\xAD\xD8\xAF\xD8\xAF",
            "\xD8\xA7\xD9\x84\xD8\xA3\xD9\x86\xD9\x88\xD8\xA7\xD8\xB9 \xD8\xAA\xD8\xB3\xD8\xA7\xD8\xB9\xD8\xAF \xD8\xB9\xD9\x84\xD9\x89 \xD8\xA7\xD9\x83\xD8\xAA\xD8\xB4\xD8\xA7\xD9\x81 \xD8\xA7\xD9\x84\xD8\xA3\xD8\xAE\xD8\xB7\xD8\xA7\xD8\xA1 \xD9\x85\xD8\xA8\xD9\x83\xD8\xB1\xD8\xA7\xD9\x8B"
        };
        c.commonMistakes = {
            "\xD8\xAC\xD9\x85\xD8\xB9 \xD9\x86\xD8\xB5 \xD9\x85\xD8\xB9 \xD8\xB9\xD8\xAF\xD8\xAF \xD9\x85\xD8\xA8\xD8\xA7\xD8\xB4\xD8\xB1\xD8\xA9"
        };
        concepts_["types"] = c;
    }
    
    // Variables concept
    {
        ConceptExplanation c;
        c.conceptAr = "\xD8\xA7\xD9\x84\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1\xD8\xA7\xD8\xAA (Variables)";
        c.conceptEn = "Variables";
        c.simpleExplanation = "\xD8\xA7\xD9\x84\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1 \xD9\x87\xD9\x88 \xD8\xA7\xD8\xB3\xD9\x85 \xD9\x86\xD8\xB9\xD8\xB7\xD9\x8A\xD9\x87 \xD9\x84\xD9\x85\xD9\x83\xD8\xA7\xD9\x86 \xD9\x81\xD9\x8A \xD8\xA7\xD9\x84\xD8\xB0\xD8\xA7\xD9\x83\xD8\xB1\xD8\xA9 \xD9\x84\xD8\xAA\xD8\xAE\xD8\xB2\xD9\x8A\xD9\x86 \xD9\x82\xD9\x8A\xD9\x85\xD8\xA9.";
        c.keyPoints = {
            "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD9\x85 '\xD9\x85\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1' \xD9\x84\xD9\x84\xD9\x82\xD9\x8A\xD9\x85 \xD8\xA7\xD9\x84\xD8\xAA\xD9\x8A \xD9\x82\xD8\xAF \xD8\xAA\xD8\xAA\xD8\xBA\xD9\x8A\xD8\xB1",
            "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD9\x85 '\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA' \xD9\x84\xD9\x84\xD9\x82\xD9\x8A\xD9\x85 \xD8\xA7\xD9\x84\xD8\xAB\xD8\xA7\xD8\xA8\xD8\xAA\xD8\xA9"
        };
        concepts_["variables"] = c;
    }
    
    // Functions concept
    {
        ConceptExplanation c;
        c.conceptAr = "\xD8\xA7\xD9\x84\xD8\xAF\xD9\x88\xD8\xA7\xD9\x84 (Functions)";
        c.conceptEn = "Functions";
        c.simpleExplanation = "\xD8\xA7\xD9\x84\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD9\x87\xD9\x8A \xD9\x85\xD8\xAC\xD9\x85\xD9\x88\xD8\xB9\xD8\xA9 \xD8\xA3\xD9\x88\xD8\xA7\xD9\x85\xD8\xB1 \xD9\x85\xD8\xB3\xD9\x85\xD9\x91\xD8\xA7\xD8\xA9 \xD9\x8A\xD9\x85\xD9\x83\xD9\x86 \xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAF\xD8\xB9\xD8\xA7\xD8\xA4\xD9\x87\xD8\xA7.";
        c.keyPoints = {
            "\xD8\xA7\xD9\x84\xD8\xAF\xD8\xA7\xD9\x84\xD8\xA9 \xD8\xAA\xD9\x86\xD8\xB8\xD9\x85 \xD8\xA7\xD9\x84\xD9\x83\xD9\x88\xD8\xAF \xD9\x88\xD8\xAA\xD9\x85\xD9\x86\xD8\xB9 \xD8\xA7\xD9\x84\xD8\xAA\xD9\x83\xD8\xB1\xD8\xA7\xD8\xB1",
            "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD9\x85 '\xD8\xA3\xD8\xB1\xD8\xAC\xD8\xB9' \xD9\x84\xD8\xA5\xD8\xB1\xD8\xAC\xD8\xA7\xD8\xB9 \xD9\x82\xD9\x8A\xD9\x85\xD8\xA9"
        };
        concepts_["functions"] = c;
    }
    
    // Conditions concept
    {
        ConceptExplanation c;
        c.conceptAr = "\xD8\xA7\xD9\x84\xD8\xB4\xD8\xB1\xD9\x88\xD8\xB7 (Conditions)";
        c.conceptEn = "Conditions";
        c.simpleExplanation = "\xD8\xA7\xD9\x84\xD8\xB4\xD8\xB1\xD9\x88\xD8\xB7 \xD8\xAA\xD8\xB3\xD9\x85\xD8\xAD \xD9\x84\xD9\x84\xD8\xA8\xD8\xB1\xD9\x86\xD8\xA7\xD9\x85\xD8\xAC \xD8\xA8\xD8\xA7\xD8\xAA\xD8\xAE\xD8\xA7\xD8\xB0 \xD9\x82\xD8\xB1\xD8\xA7\xD8\xB1\xD8\xA7\xD8\xAA.";
        c.commonMistakes = {
            "\xD8\xA7\xD8\xB3\xD8\xAA\xD8\xAE\xD8\xAF\xD8\xA7\xD9\x85 = \xD8\xA8\xD8\xAF\xD9\x84\xD8\xA7\xD9\x8B \xD9\x85\xD9\x86 == \xD9\x84\xD9\x84\xD9\x85\xD9\x82\xD8\xA7\xD8\xB1\xD9\x86\xD8\xA9"
        };
        concepts_["conditions"] = c;
    }
    
    // Loops concept
    {
        ConceptExplanation c;
        c.conceptAr = "\xD8\xA7\xD9\x84\xD8\xAD\xD9\x84\xD9\x82\xD8\xA7\xD8\xAA (Loops)";
        c.conceptEn = "Loops";        
        c.simpleExplanation = "\xD8\xA7\xD9\x84\xD8\xAD\xD9\x84\xD9\x82\xD8\xA9 \xD8\xAA\xD9\x83\xD8\xB1\xD8\xB1 \xD9\x83\xD9\x88\xD8\xAF\xD8\xA7\xD9\x8B \xD9\x85\xD8\xB9\xD9\x8A\xD9\x86\xD8\xA7\xD9\x8B \xD8\xB9\xD8\xAF\xD8\xA9 \xD9\x85\xD8\xB1\xD8\xA7\xD8\xAA.";
        c.commonMistakes = {
            "\xD9\x86\xD8\xB3\xD9\x8A\xD8\xA7\xD9\x86 \xD8\xB2\xD9\x8A\xD8\xA7\xD8\xAF\xD8\xA9 \xD8\xA7\xD9\x84\xD8\xB9\xD8\xAF\xD8\xA7\xD8\xAF (\xD8\xAD\xD9\x84\xD9\x82\xD8\xA9 \xD9\x84\xD8\xA7\xD9\x86\xD9\x87\xD8\xA7\xD8\xA6\xD9\x8A\xD8\xA9)"
        };
        concepts_["loops"] = c;
    }
}

// ═══════════════════════════════════════════════════════════════════════
//                    TeacherMode
// ═══════════════════════════════════════════════════════════════════════

TeacherMode::TeacherMode() : level_(ExplanationLevel::NORMAL) {}

void TeacherMode::setLevel(ExplanationLevel level) { level_ = level; }
ExplanationLevel TeacherMode::getLevel() const { return level_; }

std::string TeacherMode::getLevelDescription(ExplanationLevel level, bool arabic) {
    if (arabic) {
        switch (level) {
            case ExplanationLevel::BRIEF: return "\xD9\x85\xD9\x88\xD8\xAC\xD8\xB2";
            case ExplanationLevel::NORMAL: return "\xD8\xB9\xD8\xA7\xD8\xAF\xD9\x8A";
            case ExplanationLevel::DETAILED: return "\xD9\x85\xD9\x81\xD8\xB5\xD9\x84";
            case ExplanationLevel::TEACHER: return "\xD9\x85\xD8\xB9\xD9\x84\xD9\x85";
        }
    } else {
        switch (level) {
            case ExplanationLevel::BRIEF: return "Brief";
            case ExplanationLevel::NORMAL: return "Normal";
            case ExplanationLevel::DETAILED: return "Detailed";
            case ExplanationLevel::TEACHER: return "Teacher";
        }
    }
    return "";
}

std::string TeacherMode::explainError(
    const std::string& errorCode,
    const std::string& errorMessage,
    const std::string& context,
    const std::unordered_map<std::string, std::string>& details
) {
    std::ostringstream explanation;
    
    explanation << "\xE2\x9D\x8C " << errorMessage << "\n";
    
    if (level_ == ExplanationLevel::BRIEF) return explanation.str();
    
    explanation << "\n" << generateBasicExplanation(errorCode, details);
    if (level_ == ExplanationLevel::NORMAL) return explanation.str();
    
    explanation << "\n" << generateDetailedExplanation(errorCode, context, details);
    if (level_ == ExplanationLevel::DETAILED) return explanation.str();
    
    explanation << "\n" << generateTeacherExplanation(errorCode, context, details);
    return explanation.str();
}

std::string TeacherMode::getConceptExplanation(const std::string& conceptKey) {
    const ConceptExplanation* c = db_.getConcept(conceptKey);
    return c ? c->format(true) : "";
}

std::string TeacherMode::generateBasicExplanation(
    const std::string& errorCode,
    const std::unordered_map<std::string, std::string>& details
) {
    std::ostringstream oss;
    // (AR) صنّف حسب الرمز: افحص الرموز الثلاثيّة الصريحة (TYPE/SYN/SEM/LEX/RUN) أوّلًا،
    //      فكلٌّ من SYN (نحويّ) وSEM (دلاليّ) يبدأ بحرف S — لو فُحص الحرف الأوّل قبل
    //      الرمز الكامل لَوُسِم كلّ خطأ نحويّ «دلاليًّا» خطأً (يصير فرع SYN ميّتًا).
    //      الحرف المفرد (T/P/S) يبقى احتياطًا لرموز قديمة أحاديّة الحرف فقط.
    // (EN) Classify by code: test the explicit 3-letter tags (TYPE/SYN/SEM/LEX/RUN)
    //      FIRST, because both SYN (syntax) and SEM (semantic) start with 'S' — testing
    //      the first letter before the full tag mislabels every syntax error as semantic
    //      (the SYN branch becomes dead). Single-letter (T/P/S) is a legacy fallback only.
    if (errorCode.find("TYPE") != std::string::npos) {
        oss << "\xF0\x9F\x94\x8D \xD9\x87\xD8\xB0\xD8\xA7 \xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x81\xD9\x8A \xD8\xA7\xD9\x84\xD8\xA3\xD9\x86\xD9\x88\xD8\xA7\xD8\xB9.\n";
    } else if (errorCode.find("SYN") != std::string::npos) {
        oss << "\xF0\x9F\x94\x8D \xD9\x87\xD8\xB0\xD8\xA7 \xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x86\xD8\xAD\xD9\x88\xD9\x8A.\n";
    } else if (errorCode.find("SEM") != std::string::npos) {
        oss << "\xF0\x9F\x94\x8D \xD9\x87\xD8\xB0\xD8\xA7 \xD8\xAE\xD8\xB7\xD8\xA3 \xD8\xAF\xD9\x84\xD8\xA7\xD9\x84\xD9\x8A.\n";
    } else if (errorCode.find("LEX") != std::string::npos) {
        oss << "\xF0\x9F\x94\x8D \xD9\x87\xD8\xB0\xD8\xA7 \xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x85\xD8\xB9\xD8\xAC\xD9\x85\xD9\x8A.\n";
    } else if (errorCode.find("RUN") != std::string::npos) {
        oss << "\xF0\x9F\x94\x8D \xD9\x87\xD8\xB0\xD8\xA7 \xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x88\xD9\x82\xD8\xAA \xD8\xA7\xD9\x84\xD8\xAA\xD8\xB4\xD8\xBA\xD9\x8A\xD9\x84.\n";
    } else if (errorCode.substr(0, 1) == "T") {
        oss << "\xF0\x9F\x94\x8D \xD9\x87\xD8\xB0\xD8\xA7 \xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x81\xD9\x8A \xD8\xA7\xD9\x84\xD8\xA3\xD9\x86\xD9\x88\xD8\xA7\xD8\xB9.\n";
    } else if (errorCode.substr(0, 1) == "P") {
        oss << "\xF0\x9F\x94\x8D \xD9\x87\xD8\xB0\xD8\xA7 \xD8\xAE\xD8\xB7\xD8\xA3 \xD9\x86\xD8\xAD\xD9\x88\xD9\x8A.\n";
    } else if (errorCode.substr(0, 1) == "S") {
        oss << "\xF0\x9F\x94\x8D \xD9\x87\xD8\xB0\xD8\xA7 \xD8\xAE\xD8\xB7\xD8\xA3 \xD8\xAF\xD9\x84\xD8\xA7\xD9\x84\xD9\x8A.\n";
    }
    return oss.str();
}

std::string TeacherMode::generateDetailedExplanation(
    const std::string& errorCode,
    const std::string& context,
    const std::unordered_map<std::string, std::string>& details
) {
    std::ostringstream oss;
    oss << "\xF0\x9F\x93\x96 \xD8\xB4\xD8\xB1\xD8\xAD \xD9\x85\xD9\x81\xD8\xB5\xD9\x84:\n";
    oss << "   ----\n";
    if (details.count("expected") && details.count("actual")) {
        oss << "   \xD9\x83\xD8\xA7\xD9\x86 \xD9\x85\xD8\xAA\xD9\x88\xD9\x82\xD8\xB9\xD8\xA7\xD9\x8B: " << details.at("expected") << "\n";
        oss << "   \xD9\x84\xD9\x83\xD9\x86 \xD9\x88\xD8\xAC\xD8\xAF\xD8\xAA: " << details.at("actual") << "\n";
    }
    if (!context.empty()) {
        oss << "\n   \xD8\xA7\xD9\x84\xD8\xB3\xD9\x8A\xD8\xA7\xD9\x82:\n   " << context << "\n";
    }
    return oss.str();
}

std::string TeacherMode::generateTeacherExplanation(
    const std::string& errorCode,
    const std::string& context,
    const std::unordered_map<std::string, std::string>& details
) {
    std::ostringstream oss;
    oss << "\xF0\x9F\x8E\x93 \xD9\x88\xD8\xB6\xD8\xB9 \xD8\xA7\xD9\x84\xD9\x85\xD8\xB9\xD9\x84\xD9\x85:\n";
    oss << "   ====================================================\n";
    
    // Find relevant concept
    std::string conceptKey;
    if (errorCode.find("TYPE") != std::string::npos || errorCode.substr(0,1) == "T")
        conceptKey = "types";
    else if (errorCode.find("UNDEF") != std::string::npos)
        conceptKey = "variables";
    else if (errorCode.find("ARG") != std::string::npos)
        conceptKey = "functions";
    else if (errorCode.find("SYN") != std::string::npos)
        conceptKey = "conditions";
    
    if (!conceptKey.empty()) {
        const ConceptExplanation* concept = db_.getConcept(conceptKey);
        if (concept) {
            oss << concept->format(true);
        }
    }
    
    return oss.str();
}

} // namespace Errors
} // namespace Sad
