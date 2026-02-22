// ════════════════════════════════════════════════════════════════════════════════
// ملف: exhaustiveness.cpp
// File: exhaustiveness.cpp
//
// الوصف (AR): فحص شمولية الأنماط في جمل match
//             يتأكد من تغطية جميع الحالات الممكنة
//             يكتشف الأنماط التي لن تُطابق أبداً (unreachable)
//
// Description (EN): Pattern exhaustiveness checking for match statements
//                   Ensures all possible cases are covered
//                   Detects patterns that will never match (unreachable)
//
// المؤلف: فريق تطوير لغة ص
// Author: Sad Language Development Team
//
// التاريخ: يناير 2026
// Date: January 2026
//
// المرحلة: Phase 12 - User Story 10 - Pattern Matching
// Phase: Phase 12 - User Story 10 - Pattern Matching
//
// المهمة: T139 [US10] Implement exhaustiveness checking
// Task: T139 [US10] Implement exhaustiveness checking
// ════════════════════════════════════════════════════════════════════════════════

// تعطيل تحذير Unicode للتعليقات العربية
// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include "pattern_nodes.h"
#include "type_registry.h"
#include "union_type.h"
#include "enum_types.h"
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <algorithm>

namespace Sad {
namespace Semantic {

// ════════════════════════════════════════════════════════════════════════════════
// نتيجة فحص الشمولية / Exhaustiveness Check Result
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) نتيجة فحص الشمولية
 *        (EN) Exhaustiveness check result
 */
struct ExhaustivenessResult {
    bool isExhaustive;                     ///< (AR) هل الأنماط شاملة؟ / (EN) Are patterns exhaustive?
    std::vector<std::string> missingCases; ///< (AR) الحالات المفقودة / (EN) Missing cases
    std::vector<size_t> unreachablePatterns; ///< (AR) الأنماط التي لن تُطابق / (EN) Unreachable patterns
    std::vector<std::string> warnings;      ///< (AR) التحذيرات / (EN) Warnings
    
    /**
     * @brief (AR) نتيجة ناجحة (شاملة)
     *        (EN) Successful (exhaustive) result
     */
    static ExhaustivenessResult exhaustive() {
        return ExhaustivenessResult{true, {}, {}, {}};
    }
    
    /**
     * @brief (AR) نتيجة غير شاملة
     *        (EN) Non-exhaustive result
     */
    static ExhaustivenessResult nonExhaustive(const std::vector<std::string>& missing) {
        return ExhaustivenessResult{false, missing, {}, {}};
    }
};

// ════════════════════════════════════════════════════════════════════════════════
// فضاء الأنماط / Pattern Space
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) يمثل فضاء القيم الممكنة التي يمكن أن يغطيها نمط
 *        (EN) Represents the space of possible values a pattern can cover
 * 
 * (AR) هذا مهم لحساب الشمولية:
 *      - فضاء كامل = يغطي كل القيم
 *      - فضاء فارغ = لا يغطي أي قيمة
 *      - فضاء جزئي = يغطي بعض القيم
 * 
 * (EN) This is important for exhaustiveness calculation:
 *      - Full space = covers all values
 *      - Empty space = covers no values
 *      - Partial space = covers some values
 */
class PatternSpace {
public:
    enum class Kind {
        Empty,      ///< (AR) فارغ - لا يغطي شيء / (EN) Empty - covers nothing
        Full,       ///< (AR) كامل - يغطي كل شيء / (EN) Full - covers everything
        Literal,    ///< (AR) قيمة حرفية واحدة / (EN) Single literal value
        Enum,       ///< (AR) مجموعة قيم enum / (EN) Set of enum values
        Union,      ///< (AR) اتحاد فضاءات / (EN) Union of spaces
        Product     ///< (AR) حاصل ضرب (للأنماط المركبة) / (EN) Product (for compound patterns)
    };
    
    Kind kind;
    std::string literalValue;                    ///< (AR) للقيمة الحرفية / (EN) For literal
    std::unordered_set<std::string> enumValues;  ///< (AR) لـ enum / (EN) For enum
    std::vector<std::shared_ptr<PatternSpace>> subSpaces;  ///< (AR) للفضاءات المركبة / (EN) For compound spaces
    
    // ========================================================================
    // المُنشئات / Constructors
    // ========================================================================
    
    /**
     * @brief (AR) فضاء فارغ
     *        (EN) Empty space
     */
    static std::shared_ptr<PatternSpace> empty() {
        auto space = std::make_shared<PatternSpace>();
        space->kind = Kind::Empty;
        return space;
    }
    
    /**
     * @brief (AR) فضاء كامل
     *        (EN) Full space
     */
    static std::shared_ptr<PatternSpace> full() {
        auto space = std::make_shared<PatternSpace>();
        space->kind = Kind::Full;
        return space;
    }
    
    /**
     * @brief (AR) فضاء قيمة حرفية
     *        (EN) Literal value space
     */
    static std::shared_ptr<PatternSpace> literal(const std::string& value) {
        auto space = std::make_shared<PatternSpace>();
        space->kind = Kind::Literal;
        space->literalValue = value;
        return space;
    }
    
    /**
     * @brief (AR) فضاء enum
     *        (EN) Enum space
     */
    static std::shared_ptr<PatternSpace> enumSpace(const std::unordered_set<std::string>& values) {
        auto space = std::make_shared<PatternSpace>();
        space->kind = Kind::Enum;
        space->enumValues = values;
        return space;
    }
    
    // ========================================================================
    // العمليات / Operations
    // ========================================================================
    
    /**
     * @brief (AR) هل الفضاء فارغ؟
     *        (EN) Is the space empty?
     */
    bool isEmpty() const {
        switch (kind) {
            case Kind::Empty:
                return true;
            case Kind::Full:
                return false;
            case Kind::Literal:
                return false;
            case Kind::Enum:
                return enumValues.empty();
            case Kind::Union:
                return subSpaces.empty() || 
                       std::all_of(subSpaces.begin(), subSpaces.end(),
                                   [](const auto& s) { return s->isEmpty(); });
            case Kind::Product:
                return subSpaces.empty() ||
                       std::any_of(subSpaces.begin(), subSpaces.end(),
                                   [](const auto& s) { return s->isEmpty(); });
        }
        return true;
    }
    
    /**
     * @brief (AR) طرح فضاء آخر
     *        (EN) Subtract another space
     */
    std::shared_ptr<PatternSpace> subtract(const std::shared_ptr<PatternSpace>& other) const {
        if (other->kind == Kind::Full) {
            return empty();
        }
        
        if (other->kind == Kind::Empty) {
            // (AR) طرح الفارغ = نفسه
            // (EN) Subtracting empty = itself
            auto result = std::make_shared<PatternSpace>(*this);
            return result;
        }
        
        switch (kind) {
            case Kind::Empty:
                return empty();
                
            case Kind::Full:
                // (AR) طرح من الكامل يعطي المتمم
                // (EN) Subtracting from full gives complement
                // (AR) هذا تقريب - التنفيذ الكامل أكثر تعقيداً
                // (EN) This is an approximation - full implementation is more complex
                return full();
                
            case Kind::Literal:
                if (other->kind == Kind::Literal && other->literalValue == literalValue) {
                    return empty();
                }
                return std::make_shared<PatternSpace>(*this);
                
            case Kind::Enum: {
                auto result = std::make_shared<PatternSpace>(*this);
                if (other->kind == Kind::Literal) {
                    result->enumValues.erase(other->literalValue);
                } else if (other->kind == Kind::Enum) {
                    for (const auto& v : other->enumValues) {
                        result->enumValues.erase(v);
                    }
                }
                if (result->enumValues.empty()) {
                    return empty();
                }
                return result;
            }
                
            default:
                // (AR) للحالات المعقدة، نرجع نسخة
                // (EN) For complex cases, return copy
                return std::make_shared<PatternSpace>(*this);
        }
    }
    
    /**
     * @brief (AR) تحويل إلى نص
     *        (EN) Convert to string
     */
    std::string toString() const {
        switch (kind) {
            case Kind::Empty:
                return "{}";
            case Kind::Full:
                return "*";
            case Kind::Literal:
                return literalValue;
            case Kind::Enum: {
                std::string result = "{";
                bool first = true;
                for (const auto& v : enumValues) {
                    if (!first) result += ", ";
                    result += v;
                    first = false;
                }
                result += "}";
                return result;
            }
            case Kind::Union: {
                std::string result = "(";
                for (size_t i = 0; i < subSpaces.size(); ++i) {
                    if (i > 0) result += " | ";
                    result += subSpaces[i]->toString();
                }
                result += ")";
                return result;
            }
            case Kind::Product: {
                std::string result = "[";
                for (size_t i = 0; i < subSpaces.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += subSpaces[i]->toString();
                }
                result += "]";
                return result;
            }
        }
        return "?";
    }
};

// ════════════════════════════════════════════════════════════════════════════════
// فاحص الشمولية / Exhaustiveness Checker
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) فاحص شمولية الأنماط
 *        (EN) Pattern exhaustiveness checker
 * 
 * (AR) الخوارزمية:
 *      1. حساب فضاء القيم الممكنة للنوع المُختبر
 *      2. لكل نمط، طرح فضاءه من الفضاء المتبقي
 *      3. إذا كان الفضاء المتبقي فارغاً، الأنماط شاملة
 *      4. إذا لم يكن فارغاً، الحالات المفقودة هي الفضاء المتبقي
 * 
 * (EN) Algorithm:
 *      1. Compute the space of possible values for matched type
 *      2. For each pattern, subtract its space from remaining space
 *      3. If remaining space is empty, patterns are exhaustive
 *      4. If not empty, missing cases are the remaining space
 */
class ExhaustivenessChecker {
public:
    // ========================================================================
    // البناء والهدم / Construction & Destruction
    // ========================================================================
    
    ExhaustivenessChecker()
        : useArabicMessages_(true)
        , debugMode_(false)
        , strictMode_(true) {
    }
    
    ~ExhaustivenessChecker() = default;
    
    // ========================================================================
    // الواجهة الرئيسية / Main Interface
    // ========================================================================
    
    /**
     * @brief (AR) فحص شمولية جملة match
     *        (EN) Check exhaustiveness of match statement
     * 
     * @param matchStmt (AR) جملة match / (EN) Match statement
     * @param matchedType (AR) نوع القيمة المُختبرة / (EN) Type of matched value
     * @return (AR) نتيجة الفحص / (EN) Check result
     */
    ExhaustivenessResult checkExhaustiveness(
        const AST::MatchStmt& matchStmt,
        TypeSystem::TypePtr matchedType
    ) {
        ExhaustivenessResult result;
        
        if (debugMode_) {
            std::cout << "[Exhaustiveness] بدء فحص الشمولية\n";
            std::cout << "[Exhaustiveness] Starting exhaustiveness check\n";
            std::cout << "[Exhaustiveness] Matched type: " 
                      << (matchedType ? matchedType->toString() : "null") << "\n";
        }
        
        // (AR) حساب الفضاء الأولي بناءً على النوع
        // (EN) Compute initial space based on type
        auto remainingSpace = computeTypeSpace(matchedType);
        
        if (debugMode_) {
            std::cout << "[Exhaustiveness] Initial space: " 
                      << remainingSpace->toString() << "\n";
        }
        
        // (AR) معالجة كل نمط
        // (EN) Process each pattern
        for (size_t i = 0; i < matchStmt.cases.size(); ++i) {
            const auto& caseClause = matchStmt.cases[i];
            
            if (!caseClause.pattern) continue;
            
            // (AR) حساب فضاء النمط
            // (EN) Compute pattern space
            auto patternSpace = computePatternSpace(*caseClause.pattern);
            
            if (debugMode_) {
                std::cout << "[Exhaustiveness] Pattern " << (i + 1) << " space: "
                          << patternSpace->toString() << "\n";
            }
            
            // (AR) التحقق من قابلية الوصول
            // (EN) Check reachability
            if (remainingSpace->isEmpty()) {
                // (AR) الفضاء المتبقي فارغ - هذا النمط لن يُطابق أبداً
                // (EN) Remaining space is empty - this pattern will never match
                result.unreachablePatterns.push_back(i);
                result.warnings.push_back(formatWarning(
                    "النمط " + std::to_string(i + 1) + " لن يُطابق أبداً (unreachable)",
                    "Pattern " + std::to_string(i + 1) + " will never match (unreachable)"
                ));
            }
            
            // (AR) طرح فضاء النمط من المتبقي
            // (EN) Subtract pattern space from remaining
            remainingSpace = remainingSpace->subtract(patternSpace);
            
            if (debugMode_) {
                std::cout << "[Exhaustiveness] Remaining space after pattern " 
                          << (i + 1) << ": " << remainingSpace->toString() << "\n";
            }
        }
        
        // (AR) التحقق من الشمولية
        // (EN) Check exhaustiveness
        result.isExhaustive = remainingSpace->isEmpty();
        
        if (!result.isExhaustive) {
            // (AR) تحديد الحالات المفقودة
            // (EN) Determine missing cases
            result.missingCases = computeMissingCases(remainingSpace);
            
            if (debugMode_) {
                std::cout << "[Exhaustiveness] NOT exhaustive! Missing cases:\n";
                for (const auto& c : result.missingCases) {
                    std::cout << "  - " << c << "\n";
                }
            }
        } else {
            if (debugMode_) {
                std::cout << "[Exhaustiveness] EXHAUSTIVE!\n";
            }
        }
        
        return result;
    }
    
    /**
     * @brief (AR) فحص سريع: هل يوجد نمط شامل (_)؟
     *        (EN) Quick check: is there a wildcard pattern (_)?
     */
    bool hasWildcardPattern(const AST::MatchStmt& matchStmt) {
        for (const auto& caseClause : matchStmt.cases) {
            if (!caseClause.pattern) continue;
            if (isWildcard(*caseClause.pattern)) {
                return true;
            }
        }
        return false;
    }
    
    // ========================================================================
    // الإعدادات / Settings
    // ========================================================================
    
    void setArabicMessages(bool enable) { useArabicMessages_ = enable; }
    void setDebugMode(bool enable) { debugMode_ = enable; }
    void setStrictMode(bool enable) { strictMode_ = enable; }

private:
    // ========================================================================
    // حساب الفضاءات / Space Computation
    // ========================================================================
    
    /**
     * @brief (AR) حساب فضاء القيم الممكنة لنوع
     *        (EN) Compute space of possible values for a type
     */
    std::shared_ptr<PatternSpace> computeTypeSpace(TypeSystem::TypePtr type) {
        if (!type) {
            // (AR) نوع غير معروف = فضاء كامل
            // (EN) Unknown type = full space
            return PatternSpace::full();
        }
        
        auto& registry = TypeSystem::TypeRegistry::getInstance();
        
        // (AR) أنواع خاصة
        // (EN) Special types
        if (type->getKind() == TypeSystem::TypeKind::Boolean) {
            // (AR) Boolean له قيمتان فقط
            // (EN) Boolean has only two values
            return PatternSpace::enumSpace({"true", "false"});
        }
        
        // TODO: Add Enum support when TypeKind::Enum is implemented
        /*
        if (type->getKind() == TypeSystem::TypeKind::Enum) {
            // (AR) Enum: جمع كل القيم
            // (EN) Enum: collect all values
            // (AR) هنا نحتاج للوصول لتعريف الـ enum
            // (EN) Here we need access to enum definition
            // (AR) حالياً نستخدم فضاء كامل
            // (EN) Currently using full space
            return PatternSpace::full();
        }
        */
        
        if (type->getKind() == TypeSystem::TypeKind::Union) {
            // (AR) Union: اتحاد فضاءات الأعضاء
            // (EN) Union: union of member spaces
            auto* unionType = dynamic_cast<const TypeSystem::UnionType*>(type.get());
            if (unionType) {
                std::unordered_set<std::string> members;
                for (const auto& member : unionType->getAlternatives()) {
                    members.insert(member->toString());
                }
                return PatternSpace::enumSpace(members);
            }
        }
        
        // (AR) للأنواع الأخرى (number، string، etc.)، فضاء كامل
        // (EN) For other types (number, string, etc.), full space
        return PatternSpace::full();
    }
    
    /**
     * @brief (AR) حساب فضاء نمط
     *        (EN) Compute space of a pattern
     */
    std::shared_ptr<PatternSpace> computePatternSpace(const AST::Pattern& pattern) {
        // (AR) النمط الشامل يغطي كل شيء
        // (EN) Wildcard covers everything
        if (isWildcard(pattern)) {
            return PatternSpace::full();
        }
        
        // (AR) نمط متغير يغطي كل شيء (يربط أي قيمة)
        // (EN) Variable pattern covers everything (binds any value)
        if (auto* var = dynamic_cast<const AST::VariablePattern*>(&pattern)) {
            return PatternSpace::full();
        }
        
        // (AR) نمط قيمة حرفية
        // (EN) Literal pattern
        if (auto* lit = dynamic_cast<const AST::LiteralPattern*>(&pattern)) {
            return PatternSpace::literal(lit->literal.toString());
        }
        
        // (AR) نمط OR: اتحاد فضاءات البدائل
        // (EN) OR pattern: union of alternative spaces
        if (auto* orPat = dynamic_cast<const AST::OrPattern*>(&pattern)) {
            std::unordered_set<std::string> values;
            for (const auto& alt : orPat->alternatives) {
                auto space = computePatternSpace(*alt);
                if (space->kind == PatternSpace::Kind::Full) {
                    return PatternSpace::full();
                }
                if (space->kind == PatternSpace::Kind::Literal) {
                    values.insert(space->literalValue);
                }
            }
            if (values.empty()) {
                return PatternSpace::full();
            }
            return PatternSpace::enumSpace(values);
        }
        
        // (AR) نمط قائمة: حاصل ضرب
        // (EN) List pattern: product
        if (auto* listPat = dynamic_cast<const AST::ListPattern*>(&pattern)) {
            if (listPat->has_rest) {
                // (AR) نمط مع *rest يغطي كل القوائم بهذا الطول أو أكثر
                // (EN) Pattern with *rest covers all lists of this length or more
                return PatternSpace::full();
            }
            // (AR) حالياً تقريب بسيط
            // (EN) Currently simple approximation
            return PatternSpace::full();
        }
        
        return PatternSpace::full();
    }
    
    /**
     * @brief (AR) هل النمط شامل (_)؟
     *        (EN) Is pattern a wildcard (_)?
     */
    bool isWildcard(const AST::Pattern& pattern) {
        return dynamic_cast<const AST::WildcardPattern*>(&pattern) != nullptr;
    }
    
    /**
     * @brief (AR) حساب الحالات المفقودة من الفضاء المتبقي
     *        (EN) Compute missing cases from remaining space
     */
    std::vector<std::string> computeMissingCases(
        const std::shared_ptr<PatternSpace>& space
    ) {
        std::vector<std::string> missing;
        
        switch (space->kind) {
            case PatternSpace::Kind::Empty:
                // (AR) لا شيء مفقود
                // (EN) Nothing missing
                break;
                
            case PatternSpace::Kind::Full:
                // (AR) كل شيء مفقود - نحتاج نمط شامل
                // (EN) Everything missing - need wildcard
                missing.push_back(formatMissing(
                    "_ (نمط شامل)",
                    "_ (wildcard pattern)"
                ));
                break;
                
            case PatternSpace::Kind::Literal:
                missing.push_back(space->literalValue);
                break;
                
            case PatternSpace::Kind::Enum:
                for (const auto& v : space->enumValues) {
                    missing.push_back(v);
                }
                break;
                
            case PatternSpace::Kind::Union:
                for (const auto& sub : space->subSpaces) {
                    auto subMissing = computeMissingCases(sub);
                    missing.insert(missing.end(), subMissing.begin(), subMissing.end());
                }
                break;
                
            case PatternSpace::Kind::Product:
                // (AR) للمنتجات، نحتاج تحليل أعمق
                // (EN) For products, need deeper analysis
                missing.push_back(formatMissing(
                    "أنماط إضافية مطلوبة",
                    "Additional patterns required"
                ));
                break;
        }
        
        return missing;
    }
    
    // ========================================================================
    // دوال مساعدة / Helper Functions
    // ========================================================================
    
    std::string formatWarning(const std::string& arabic, const std::string& english) {
        if (useArabicMessages_) {
            return arabic + " / " + english;
        }
        return english;
    }
    
    std::string formatMissing(const std::string& arabic, const std::string& english) {
        if (useArabicMessages_) {
            return arabic + " / " + english;
        }
        return english;
    }
    
    // ========================================================================
    // الحقول / Fields
    // ========================================================================
    
    bool useArabicMessages_;
    bool debugMode_;
    bool strictMode_;  ///< (AR) الوضع الصارم - يتطلب شمولية كاملة / (EN) Strict mode - requires full exhaustiveness
};

// ════════════════════════════════════════════════════════════════════════════════
// واجهة خارجية / External Interface
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) فحص شمولية جملة match
 *        (EN) Check match statement exhaustiveness
 * 
 * @param matchStmt (AR) جملة match / (EN) Match statement
 * @param matchedType (AR) نوع القيمة المُختبرة / (EN) Type of matched value
 * @return (AR) نتيجة الفحص / (EN) Check result
 */
ExhaustivenessResult checkExhaustiveness(
    const AST::MatchStmt& matchStmt,
    TypeSystem::TypePtr matchedType
) {
    ExhaustivenessChecker checker;
    return checker.checkExhaustiveness(matchStmt, matchedType);
}

/**
 * @brief (AR) التحقق السريع من وجود نمط شامل
 *        (EN) Quick check for wildcard pattern
 */
bool hasWildcardOrCatchAll(const AST::MatchStmt& matchStmt) {
    ExhaustivenessChecker checker;
    return checker.hasWildcardPattern(matchStmt);
}

/**
 * @brief (AR) رسالة خطأ عدم الشمولية
 *        (EN) Non-exhaustiveness error message
 * 
 * @param result (AR) نتيجة الفحص / (EN) Check result
 * @return (AR) رسالة خطأ مُنسقة / (EN) Formatted error message
 */
std::string formatExhaustivenessError(const ExhaustivenessResult& result) {
    if (result.isExhaustive) {
        return "";
    }
    
    std::ostringstream oss;
    oss << "خطأ: جملة match غير شاملة! / Error: match statement is not exhaustive!\n";
    oss << "الحالات المفقودة / Missing cases:\n";
    
    for (const auto& missing : result.missingCases) {
        oss << "  - " << missing << "\n";
    }
    
    oss << "\nاقتراح: أضف نمط شامل '_' للتعامل مع الحالات المتبقية.\n";
    oss << "Suggestion: Add a wildcard pattern '_' to handle remaining cases.\n";
    
    return oss.str();
}

} // namespace Semantic
} // namespace Sad

// ════════════════════════════════════════════════════════════════════════════════
// نهاية الملف / End of File
// ════════════════════════════════════════════════════════════════════════════════
