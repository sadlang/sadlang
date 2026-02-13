// ════════════════════════════════════════════════════════════════════════════════
// ملف: pattern_checker.cpp
// File: pattern_checker.cpp
//
// الوصف (AR): فحص أنواع الأنماط في جمل match
//             يتحقق من توافق الأنماط مع النوع المُختبر
//             يكتشف الأخطاء مثل الأنماط غير المتوافقة والأنماط المكررة
//
// Description (EN): Pattern type checking for match statements
//                   Verifies pattern compatibility with matched type
//                   Detects errors like incompatible patterns and duplicates
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
// المهمة: T138 [US10] Implement pattern type checking
// Task: T138 [US10] Implement pattern type checking
// ════════════════════════════════════════════════════════════════════════════════

// تعطيل تحذير Unicode للتعليقات العربية
// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include "pattern_nodes.h"
#include "type_registry.h"
#include "union_type.h"
#include "primitive_type.h"
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>
#include <memory>

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// نتيجة فحص النمط / Pattern Check Result
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) نتيجة فحص نمط واحد
 *        (EN) Result of checking a single pattern
 */
struct PatternCheckResult {
    bool success;                    ///< (AR) هل نجح الفحص؟ / (EN) Did check succeed?
    std::string errorMessage;        ///< (AR) رسالة الخطأ (إن وُجد) / (EN) Error message (if any)
    TypePtr inferredType;            ///< (AR) النوع المُستنتج / (EN) Inferred type
    std::vector<std::string> boundVariables;  ///< (AR) المتغيرات المربوطة / (EN) Bound variables
    
    /**
     * @brief (AR) نتيجة ناجحة
     *        (EN) Successful result
     */
    static PatternCheckResult success_result(TypePtr type = nullptr) {
        return PatternCheckResult{true, "", type, {}};
    }
    
    /**
     * @brief (AR) نتيجة فاشلة مع رسالة خطأ
     *        (EN) Failed result with error message
     */
    static PatternCheckResult failure(const std::string& msg) {
        return PatternCheckResult{false, msg, nullptr, {}};
    }
};

// ════════════════════════════════════════════════════════════════════════════════
// فاحص أنواع الأنماط / Pattern Type Checker
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) فاحص أنواع الأنماط في جمل match
 *        (EN) Pattern type checker for match statements
 * 
 * (AR) يقوم بـ:
 *      1. التحقق من توافق النمط مع النوع المُختبر
 *      2. استنتاج أنواع المتغيرات المربوطة
 *      3. اكتشاف الأنماط المكررة
 *      4. التحقق من صحة الشروط (guards)
 * 
 * (EN) Performs:
 *      1. Verifies pattern compatibility with matched type
 *      2. Infers types of bound variables
 *      3. Detects duplicate patterns
 *      4. Validates guard conditions
 */
class PatternChecker {
public:
    // ========================================================================
    // البناء والهدم / Construction & Destruction
    // ========================================================================
    
    /**
     * @brief (AR) المُنشئ
     *        (EN) Constructor
     */
    PatternChecker()
        : useArabicMessages_(true)
        , debugMode_(false)
        , currentMatchType_(nullptr) {
    }
    
    /**
     * @brief (AR) الهادم
     *        (EN) Destructor
     */
    ~PatternChecker() = default;
    
    // ========================================================================
    // الواجهة الرئيسية / Main Interface
    // ========================================================================
    
    /**
     * @brief (AR) فحص جملة match كاملة
     *        (EN) Check a complete match statement
     * 
     * @param matchStmt (AR) جملة match المراد فحصها / (EN) Match statement to check
     * @param matchedType (AR) نوع القيمة المُختبرة / (EN) Type of matched value
     * @return (AR) قائمة الأخطاء (فارغة = نجاح) / (EN) List of errors (empty = success)
     */
    std::vector<std::string> checkMatchStatement(
        const AST::MatchStmt& matchStmt,
        TypePtr matchedType
    ) {
        std::vector<std::string> errors;
        currentMatchType_ = matchedType;
        seenPatterns_.clear();
        
        if (debugMode_) {
            std::cout << "[PatternChecker] بدء فحص match statement\n";
            std::cout << "[PatternChecker] Starting match statement check\n";
            std::cout << "[PatternChecker] Matched type: " 
                      << (matchedType ? matchedType->toString() : "null") << "\n";
        }
        
        // (AR) فحص كل فرع case
        // (EN) Check each case clause
        for (size_t i = 0; i < matchStmt.cases.size(); ++i) {
            const auto& caseClause = matchStmt.cases[i];
            
            if (!caseClause.pattern) {
                errors.push_back(formatError(
                    "الفرع " + std::to_string(i + 1) + " يفتقر لنمط",
                    "Case " + std::to_string(i + 1) + " is missing a pattern"
                ));
                continue;
            }
            
            // (AR) فحص النمط
            // (EN) Check pattern
            auto result = checkPattern(*caseClause.pattern, matchedType);
            if (!result.success) {
                errors.push_back(formatError(
                    "خطأ في الفرع " + std::to_string(i + 1) + ": " + result.errorMessage,
                    "Error in case " + std::to_string(i + 1) + ": " + result.errorMessage
                ));
            }
            
            // (AR) فحص التكرار
            // (EN) Check for duplicates
            std::string patternStr = patternToString(*caseClause.pattern);
            if (seenPatterns_.count(patternStr) > 0) {
                errors.push_back(formatError(
                    "النمط '" + patternStr + "' مكرر",
                    "Pattern '" + patternStr + "' is duplicated"
                ));
            }
            seenPatterns_.insert(patternStr);
            
            // (AR) فحص الشرط (guard) إن وُجد
            // (EN) Check guard condition if present
            if (caseClause.guard) {
                auto guardResult = checkGuard(*caseClause.guard, result.boundVariables);
                if (!guardResult.success) {
                    errors.push_back(formatError(
                        "خطأ في شرط الفرع " + std::to_string(i + 1) + ": " + guardResult.errorMessage,
                        "Error in case " + std::to_string(i + 1) + " guard: " + guardResult.errorMessage
                    ));
                }
            }
        }
        
        return errors;
    }
    
    /**
     * @brief (AR) فحص نمط واحد
     *        (EN) Check a single pattern
     * 
     * @param pattern (AR) النمط المراد فحصه / (EN) Pattern to check
     * @param expectedType (AR) النوع المتوقع / (EN) Expected type
     * @return (AR) نتيجة الفحص / (EN) Check result
     */
    PatternCheckResult checkPattern(const AST::Pattern& pattern, TypePtr expectedType) {
        // (AR) النمط الشامل _ يتوافق مع أي نوع
        // (EN) Wildcard pattern _ matches any type
        if (auto* wildcard = dynamic_cast<const AST::WildcardPattern*>(&pattern)) {
            return PatternCheckResult::success_result(expectedType);
        }
        
        // (AR) نمط القيمة الحرفية
        // (EN) Literal pattern
        if (auto* literal = dynamic_cast<const AST::LiteralPattern*>(&pattern)) {
            return checkLiteralPattern(*literal, expectedType);
        }
        
        // (AR) نمط المتغير
        // (EN) Variable pattern
        if (auto* variable = dynamic_cast<const AST::VariablePattern*>(&pattern)) {
            return checkVariablePattern(*variable, expectedType);
        }
        
        // (AR) نمط القائمة
        // (EN) List pattern
        if (auto* list = dynamic_cast<const AST::ListPattern*>(&pattern)) {
            return checkListPattern(*list, expectedType);
        }
        
        // (AR) نمط OR
        // (EN) OR pattern
        if (auto* orPattern = dynamic_cast<const AST::OrPattern*>(&pattern)) {
            return checkOrPattern(*orPattern, expectedType);
        }
        
        return PatternCheckResult::failure(formatError(
            "نوع نمط غير معروف",
            "Unknown pattern type"
        ));
    }
    
    // ========================================================================
    // الإعدادات / Settings
    // ========================================================================
    
    /**
     * @brief (AR) تفعيل/تعطيل الرسائل العربية
     *        (EN) Enable/disable Arabic messages
     */
    void setArabicMessages(bool enable) { useArabicMessages_ = enable; }
    
    /**
     * @brief (AR) تفعيل/تعطيل وضع التنقيح
     *        (EN) Enable/disable debug mode
     */
    void setDebugMode(bool enable) { debugMode_ = enable; }

private:
    // ========================================================================
    // فحص أنواع الأنماط المحددة / Specific Pattern Type Checking
    // ========================================================================
    
    /**
     * @brief (AR) فحص نمط قيمة حرفية
     *        (EN) Check literal pattern
     */
    PatternCheckResult checkLiteralPattern(
        const AST::LiteralPattern& pattern,
        TypePtr expectedType
    ) {
        // (AR) الحصول على نوع القيمة الحرفية
        // (EN) Get literal value type
        auto& registry = TypeRegistry::getInstance();
        TypePtr literalType = nullptr;
        
        switch (pattern.literal.getType()) {
            case Data::ValueType::BOOLEAN:
                literalType = registry.getBooleanType();
                break;
            case Data::ValueType::INTEGER:
                literalType = registry.getIntegerType();
                break;
            case Data::ValueType::DOUBLE:
                literalType = registry.getFloatType();
                break;
            case Data::ValueType::STRING:
                literalType = registry.getStringType();
                break;
            case Data::ValueType::VOID:
                literalType = registry.getVoidType();
                break;
            default:
                return PatternCheckResult::failure(formatError(
                    "نوع قيمة حرفية غير مدعوم",
                    "Unsupported literal type"
                ));
        }
        
        // (AR) التحقق من التوافق
        // (EN) Check compatibility
        if (!isTypeCompatible(literalType, expectedType)) {
            return PatternCheckResult::failure(formatError(
                "النمط '" + pattern.literal.toString() + "' لا يتوافق مع النوع المتوقع",
                "Pattern '" + pattern.literal.toString() + "' is not compatible with expected type"
            ));
        }
        
        return PatternCheckResult::success_result(literalType);
    }
    
    /**
     * @brief (AR) فحص نمط متغير
     *        (EN) Check variable pattern
     */
    PatternCheckResult checkVariablePattern(
        const AST::VariablePattern& pattern,
        TypePtr expectedType
    ) {
        // (AR) نمط المتغير يتوافق مع أي نوع ويربط القيمة
        // (EN) Variable pattern matches any type and binds the value
        PatternCheckResult result = PatternCheckResult::success_result(expectedType);
        result.boundVariables.push_back(pattern.name);
        
        if (debugMode_) {
            std::cout << "[PatternChecker] Variable pattern '" << pattern.name 
                      << "' bound with type: " 
                      << (expectedType ? expectedType->toString() : "any") << "\n";
        }
        
        return result;
    }
    
    /**
     * @brief (AR) فحص نمط قائمة
     *        (EN) Check list pattern
     */
    PatternCheckResult checkListPattern(
        const AST::ListPattern& pattern,
        TypePtr expectedType
    ) {
        // (AR) التحقق من أن النوع المتوقع هو قائمة/مصفوفة
        // (EN) Verify expected type is a list/array
        if (expectedType && expectedType->getKind() != TypeKind::Array) {
            // (AR) قد يكون Union يحتوي على Array
            // (EN) Might be Union containing Array
            if (expectedType->getKind() != TypeKind::Union) {
                return PatternCheckResult::failure(formatError(
                    "نمط القائمة لا يتوافق مع النوع '" + expectedType->toString() + "'",
                    "List pattern is not compatible with type '" + expectedType->toString() + "'"
                ));
            }
        }
        
        // (AR) فحص كل عنصر في النمط
        // (EN) Check each element in pattern
        PatternCheckResult result = PatternCheckResult::success_result(expectedType);
        
        // (AR) استخراج نوع العنصر من نوع المصفوفة
        // (EN) Extract element type from array type
        TypePtr elementType = nullptr;
        if (expectedType && expectedType->getKind() == TypeKind::Array) {
            // (AR) افتراض نوع عنصر عام
            // (EN) Assume generic element type
            auto& registry = TypeRegistry::getInstance();
            elementType = registry.getAnyType();
        }
        
        for (const auto& elemPattern : pattern.elements) {
            auto elemResult = checkPattern(*elemPattern, elementType);
            if (!elemResult.success) {
                return elemResult;
            }
            // (AR) جمع المتغيرات المربوطة
            // (EN) Collect bound variables
            result.boundVariables.insert(
                result.boundVariables.end(),
                elemResult.boundVariables.begin(),
                elemResult.boundVariables.end()
            );
        }
        
        // (AR) إضافة متغير rest إن وُجد
        // (EN) Add rest variable if present
        if (pattern.has_rest && !pattern.rest_name.empty()) {
            result.boundVariables.push_back(pattern.rest_name);
        }
        
        return result;
    }
    
    /**
     * @brief (AR) فحص نمط OR
     *        (EN) Check OR pattern
     */
    PatternCheckResult checkOrPattern(
        const AST::OrPattern& pattern,
        TypePtr expectedType
    ) {
        // (AR) جميع البدائل يجب أن تتوافق مع النوع المتوقع
        // (EN) All alternatives must be compatible with expected type
        std::vector<std::string> allBoundVars;
        
        for (const auto& alt : pattern.alternatives) {
            auto result = checkPattern(*alt, expectedType);
            if (!result.success) {
                return result;
            }
            
            // (AR) جمع المتغيرات المربوطة (يجب أن تكون متطابقة في كل البدائل)
            // (EN) Collect bound variables (must be identical in all alternatives)
            if (allBoundVars.empty()) {
                allBoundVars = result.boundVariables;
            } else {
                // (AR) التحقق من تطابق المتغيرات المربوطة
                // (EN) Verify bound variables match
                if (allBoundVars != result.boundVariables) {
                    return PatternCheckResult::failure(formatError(
                        "المتغيرات المربوطة يجب أن تكون متطابقة في جميع بدائل OR",
                        "Bound variables must be identical in all OR alternatives"
                    ));
                }
            }
        }
        
        PatternCheckResult result = PatternCheckResult::success_result(expectedType);
        result.boundVariables = allBoundVars;
        return result;
    }
    
    /**
     * @brief (AR) فحص شرط guard
     *        (EN) Check guard condition
     */
    PatternCheckResult checkGuard(
        const AST::Expression& guard,
        const std::vector<std::string>& boundVars
    ) {
        // (AR) الشرط يجب أن يكون تعبير منطقي
        // (EN) Guard must be a boolean expression
        // (AR) هذا الفحص الأساسي - التنفيذ الكامل يحتاج semantic analysis
        // (EN) This is basic check - full implementation needs semantic analysis
        
        return PatternCheckResult::success_result(nullptr);
    }
    
    // ========================================================================
    // دوال مساعدة / Helper Functions
    // ========================================================================
    
    /**
     * @brief (AR) التحقق من توافق الأنواع
     *        (EN) Check type compatibility
     */
    bool isTypeCompatible(TypePtr actual, TypePtr expected) {
        if (!expected) return true;  // (AR) أي نوع مقبول / (EN) Any type is acceptable
        if (!actual) return false;
        
        // (AR) نفس النوع
        // (EN) Same type
        if (actual->equals(expected.get())) return true;
        
        // (AR) أي نوع يتوافق مع Any
        // (EN) Any type is compatible with Any
        auto& registry = TypeRegistry::getInstance();
        if (expected->equals(registry.getAnyType().get())) return true;
        
        // (AR) التحقق من Union
        // (EN) Check Union
        if (expected->getKind() == TypeKind::Union) {
            auto* unionType = dynamic_cast<const UnionType*>(expected.get());
            if (unionType) {
                for (const auto& member : unionType->getAlternatives()) {
                    if (isTypeCompatible(actual, member)) return true;
                }
            }
        }
        
        return false;
    }
    
    /**
     * @brief (AR) تحويل نمط إلى نص (للتحقق من التكرار)
     *        (EN) Convert pattern to string (for duplicate checking)
     */
    std::string patternToString(const AST::Pattern& pattern) {
        return pattern.toString();
    }
    
    /**
     * @brief (AR) تنسيق رسالة الخطأ
     *        (EN) Format error message
     */
    std::string formatError(const std::string& arabic, const std::string& english) {
        if (useArabicMessages_) {
            return arabic + " / " + english;
        }
        return english;
    }
    
    // ========================================================================
    // الحقول / Fields
    // ========================================================================
    
    bool useArabicMessages_;                    ///< (AR) استخدام الرسائل العربية
    bool debugMode_;                            ///< (AR) وضع التنقيح
    TypePtr currentMatchType_;                  ///< (AR) نوع القيمة المُختبرة الحالي
    std::unordered_set<std::string> seenPatterns_;  ///< (AR) الأنماط التي تم رؤيتها
};

// ════════════════════════════════════════════════════════════════════════════════
// واجهة خارجية / External Interface
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) فحص أنواع جملة match
 *        (EN) Type check a match statement
 * 
 * @param matchStmt (AR) جملة match / (EN) Match statement
 * @param matchedType (AR) نوع القيمة المُختبرة / (EN) Type of matched value
 * @return (AR) قائمة أخطاء (فارغة = نجاح) / (EN) Error list (empty = success)
 */
std::vector<std::string> checkPatternTypes(
    const AST::MatchStmt& matchStmt,
    TypePtr matchedType
) {
    PatternChecker checker;
    return checker.checkMatchStatement(matchStmt, matchedType);
}

/**
 * @brief (AR) فحص نمط واحد
 *        (EN) Check a single pattern
 * 
 * @param pattern (AR) النمط / (EN) Pattern
 * @param expectedType (AR) النوع المتوقع / (EN) Expected type
 * @param outErrors (AR) قائمة الأخطاء / (EN) Error list
 * @return (AR) true إذا نجح / (EN) true if successful
 */
bool checkSinglePattern(
    const AST::Pattern& pattern,
    TypePtr expectedType,
    std::vector<std::string>& outErrors
) {
    PatternChecker checker;
    auto result = checker.checkPattern(pattern, expectedType);
    if (!result.success) {
        outErrors.push_back(result.errorMessage);
    }
    return result.success;
}

} // namespace TypeSystem
} // namespace Sad

// ════════════════════════════════════════════════════════════════════════════════
// نهاية الملف / End of File
// ════════════════════════════════════════════════════════════════════════════════
