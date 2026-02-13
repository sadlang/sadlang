// ════════════════════════════════════════════════════════════════════════════════
// ملف: pattern_guard.cpp
// File: pattern_guard.cpp
//
// الوصف (AR): شروط الحراسة (Guard Clauses) في أنماط match
//             يدعم شروط إذا (if) في case patterns
//             يتحقق من صحة الشروط وأنواعها
//
// Description (EN): Guard Clauses in match patterns
//                   Supports if conditions in case patterns
//                   Validates conditions and their types
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
// المهمة: T142 [US10] Implement guard clauses (إذا) in patterns
// Task: T142 [US10] Implement guard clauses (إذا) in patterns
// ════════════════════════════════════════════════════════════════════════════════

// تعطيل تحذير Unicode للتعليقات العربية
// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include "pattern_nodes.h"
#include "expressions.h"
#include "type_registry.h"
#include <iostream>
#include <sstream>
#include <unordered_set>
#include <vector>
#include <memory>

namespace Sad {
namespace Parser {

// ════════════════════════════════════════════════════════════════════════════════
// نتيجة فحص Guard / Guard Check Result
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) نتيجة فحص شرط guard
 *        (EN) Guard condition check result
 */
struct GuardCheckResult {
    bool success;                        ///< (AR) هل نجح الفحص؟ / (EN) Did check succeed?
    std::string errorMessage;            ///< (AR) رسالة الخطأ / (EN) Error message
    bool isConstantTrue;                 ///< (AR) هل الشرط دائماً صحيح؟ / (EN) Is condition always true?
    bool isConstantFalse;                ///< (AR) هل الشرط دائماً خاطئ؟ / (EN) Is condition always false?
    std::vector<std::string> usedVariables;  ///< (AR) المتغيرات المستخدمة / (EN) Used variables
    
    static GuardCheckResult success_result() {
        return GuardCheckResult{true, "", false, false, {}};
    }
    
    static GuardCheckResult failure(const std::string& msg) {
        return GuardCheckResult{false, msg, false, false, {}};
    }
    
    static GuardCheckResult constantTrue() {
        auto result = success_result();
        result.isConstantTrue = true;
        return result;
    }
    
    static GuardCheckResult constantFalse() {
        auto result = success_result();
        result.isConstantFalse = true;
        return result;
    }
};

// ════════════════════════════════════════════════════════════════════════════════
// فاحص شروط Guard / Guard Condition Checker
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) فاحص شروط guard في أنماط match
 *        (EN) Guard condition checker for match patterns
 * 
 * (AR) يتحقق من:
 *      1. أن الشرط يُرجع قيمة منطقية
 *      2. أن جميع المتغيرات المستخدمة مُربوطة في النمط
 *      3. اكتشاف الشروط الثابتة (دائماً صحيح/خاطئ)
 *      4. تحذيرات للشروط غير المفيدة
 * 
 * (EN) Verifies:
 *      1. Condition returns boolean value
 *      2. All used variables are bound in pattern
 *      3. Detects constant conditions (always true/false)
 *      4. Warnings for useless conditions
 */
class GuardChecker {
public:
    // ========================================================================
    // البناء والهدم / Construction & Destruction
    // ========================================================================
    
    GuardChecker()
        : useArabicMessages_(true)
        , debugMode_(false) {
    }
    
    ~GuardChecker() = default;
    
    // ========================================================================
    // الواجهة الرئيسية / Main Interface
    // ========================================================================
    
    /**
     * @brief (AR) فحص شرط guard
     *        (EN) Check guard condition
     * 
     * @param guard (AR) تعبير الشرط / (EN) Condition expression
     * @param boundVariables (AR) المتغيرات المربوطة في النمط / (EN) Variables bound in pattern
     * @return (AR) نتيجة الفحص / (EN) Check result
     */
    GuardCheckResult checkGuard(
        const AST::Expression& guard,
        const std::unordered_set<std::string>& boundVariables
    ) {
        GuardCheckResult result = GuardCheckResult::success_result();
        
        if (debugMode_) {
            std::cout << "[GuardChecker] فحص شرط guard\n";
            std::cout << "[GuardChecker] Checking guard condition\n";
            std::cout << "[GuardChecker] Bound variables: ";
            for (const auto& v : boundVariables) {
                std::cout << v << " ";
            }
            std::cout << "\n";
        }
        
        // (AR) جمع المتغيرات المستخدمة في الشرط
        // (EN) Collect variables used in condition
        std::unordered_set<std::string> usedVars;
        collectUsedVariables(guard, usedVars);
        
        result.usedVariables = std::vector<std::string>(usedVars.begin(), usedVars.end());
        
        // (AR) التحقق من أن جميع المتغيرات مُربوطة
        // (EN) Verify all variables are bound
        for (const auto& var : usedVars) {
            // (AR) تجاهل الدوال والثوابت المعروفة
            // (EN) Ignore known functions and constants
            if (isBuiltinIdentifier(var)) continue;
            
            if (boundVariables.find(var) == boundVariables.end()) {
                return GuardCheckResult::failure(formatError(
                    "المتغير '" + var + "' غير مُربوط في النمط",
                    "Variable '" + var + "' is not bound in pattern"
                ));
            }
        }
        
        // (AR) محاولة اكتشاف الشروط الثابتة
        // (EN) Try to detect constant conditions
        if (auto* literal = dynamic_cast<const AST::LiteralExpr*>(&guard)) {
            if (literal->getValue().getType() == Data::ValueType::BOOLEAN) {
                if (literal->getValue().toBool()) {
                    return GuardCheckResult::constantTrue();
                } else {
                    return GuardCheckResult::constantFalse();
                }
            }
        }
        
        return result;
    }
    
    /**
     * @brief (AR) فحص جميع شروط guards في جملة match
     *        (EN) Check all guard conditions in match statement
     * 
     * @param matchStmt (AR) جملة match / (EN) Match statement
     * @return (AR) قائمة الأخطاء والتحذيرات / (EN) List of errors and warnings
     */
    std::pair<std::vector<std::string>, std::vector<std::string>> checkAllGuards(
        const AST::MatchStmt& matchStmt
    ) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        for (size_t i = 0; i < matchStmt.cases.size(); ++i) {
            const auto& caseClause = matchStmt.cases[i];
            
            if (!caseClause.guard) continue;
            
            // (AR) جمع المتغيرات المربوطة في النمط
            // (EN) Collect variables bound in pattern
            std::unordered_set<std::string> boundVars;
            if (caseClause.pattern) {
                collectBoundVariables(*caseClause.pattern, boundVars);
            }
            
            // (AR) فحص الشرط
            // (EN) Check condition
            auto result = checkGuard(*caseClause.guard, boundVars);
            
            if (!result.success) {
                errors.push_back(formatError(
                    "الفرع " + std::to_string(i + 1) + ": " + result.errorMessage,
                    "Case " + std::to_string(i + 1) + ": " + result.errorMessage
                ));
            }
            
            if (result.isConstantTrue) {
                warnings.push_back(formatError(
                    "الفرع " + std::to_string(i + 1) + ": الشرط دائماً صحيح",
                    "Case " + std::to_string(i + 1) + ": Guard is always true"
                ));
            }
            
            if (result.isConstantFalse) {
                warnings.push_back(formatError(
                    "الفرع " + std::to_string(i + 1) + ": الشرط دائماً خاطئ (الفرع لن يُنفذ أبداً)",
                    "Case " + std::to_string(i + 1) + ": Guard is always false (case will never execute)"
                ));
            }
        }
        
        return {errors, warnings};
    }
    
    // ========================================================================
    // الإعدادات / Settings
    // ========================================================================
    
    void setArabicMessages(bool enable) { useArabicMessages_ = enable; }
    void setDebugMode(bool enable) { debugMode_ = enable; }

private:
    // ========================================================================
    // جمع المتغيرات / Variable Collection
    // ========================================================================
    
    /**
     * @brief (AR) جمع المتغيرات المستخدمة في تعبير
     *        (EN) Collect variables used in expression
     */
    void collectUsedVariables(
        const AST::Expression& expr,
        std::unordered_set<std::string>& vars
    ) {
        // (AR) متغير (معرّف)
        // (EN) Variable (identifier)
        if (auto* ident = dynamic_cast<const AST::IdentifierExpr*>(&expr)) {
            vars.insert(ident->getName());
            return;
        }
        
        // (AR) تعبير ثنائي
        // (EN) Binary expression
        if (auto* binary = dynamic_cast<const AST::BinaryExpr*>(&expr)) {
            if (binary->getLeft()) {
                collectUsedVariables(*binary->getLeft(), vars);
            }
            if (binary->getRight()) {
                collectUsedVariables(*binary->getRight(), vars);
            }
            return;
        }
        
        // (AR) تعبير أحادي
        // (EN) Unary expression
        if (auto* unary = dynamic_cast<const AST::UnaryExpr*>(&expr)) {
            if (unary->getOperand()) {
                collectUsedVariables(*unary->getOperand(), vars);
            }
            return;
        }
        
        // (AR) استدعاء دالة
        // (EN) Function call
        if (auto* call = dynamic_cast<const AST::CallExpr*>(&expr)) {
            // (AR) اسم الدالة ليس متغيراً عادياً
            // (EN) Function name is not a regular variable
            for (const auto& arg : call->getArgs()) {
                if (arg) {
                    collectUsedVariables(*arg, vars);
                }
            }
            return;
        }
        
        // (AR) وصول لعضو
        // (EN) Member access
        if (auto* member = dynamic_cast<const AST::MemberExpr*>(&expr)) {
            if (member->getObject()) {
                collectUsedVariables(*member->getObject(), vars);
            }
            return;
        }
        
        // (AR) وصول لفهرس
        // (EN) Index access
        if (auto* index = dynamic_cast<const AST::IndexExpr*>(&expr)) {
            if (index->getObject()) {
                collectUsedVariables(*index->getObject(), vars);
            }
            if (index->getIndex()) {
                collectUsedVariables(*index->getIndex(), vars);
            }
            return;
        }
        
        // (AR) تعبيرات أخرى لا تحتوي متغيرات (literals, etc.)
        // (EN) Other expressions don't contain variables (literals, etc.)
    }
    
    /**
     * @brief (AR) جمع المتغيرات المربوطة في نمط
     *        (EN) Collect variables bound in pattern
     */
    void collectBoundVariables(
        const AST::Pattern& pattern,
        std::unordered_set<std::string>& vars
    ) {
        // (AR) نمط متغير
        // (EN) Variable pattern
        if (auto* varPat = dynamic_cast<const AST::VariablePattern*>(&pattern)) {
            vars.insert(varPat->name);
            return;
        }
        
        // (AR) نمط قائمة
        // (EN) List pattern
        if (auto* listPat = dynamic_cast<const AST::ListPattern*>(&pattern)) {
            for (const auto& elem : listPat->elements) {
                if (elem) {
                    collectBoundVariables(*elem, vars);
                }
            }
            if (listPat->has_rest && !listPat->rest_name.empty()) {
                vars.insert(listPat->rest_name);
            }
            return;
        }
        
        // (AR) نمط OR
        // (EN) OR pattern
        if (auto* orPat = dynamic_cast<const AST::OrPattern*>(&pattern)) {
            // (AR) جميع البدائل تربط نفس المتغيرات
            // (EN) All alternatives bind same variables
            if (!orPat->alternatives.empty()) {
                collectBoundVariables(*orPat->alternatives[0], vars);
            }
            return;
        }
        
        // (AR) النمط الشامل والقيمة الحرفية لا يربطان متغيرات
        // (EN) Wildcard and literal don't bind variables
    }
    
    /**
     * @brief (AR) التحقق من معرّف مدمج
     *        (EN) Check if identifier is builtin
     */
    bool isBuiltinIdentifier(const std::string& name) {
        static const std::unordered_set<std::string> builtins = {
            // (AR) الدوال المدمجة
            // (EN) Builtin functions
            "اطبع", "print", "println",
            "طول", "len", "length",
            "نوع", "type", "typeof",
            "صحيح", "true", "خطأ", "false",
            "فارغ", "null", "nil",
            "ذات", "self", "this"
        };
        return builtins.count(name) > 0;
    }
    
    // ========================================================================
    // دوال مساعدة / Helper Functions
    // ========================================================================
    
    std::string formatError(const std::string& arabic, const std::string& english) {
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
};

// ════════════════════════════════════════════════════════════════════════════════
// محلل Guard في Parser / Guard Parser Analysis
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) تحليل شرط guard من النص
 *        (EN) Parse guard condition from text
 * 
 * (AR) ملاحظة: التحليل الفعلي يتم في parsePattern() في parser_core_impl.cpp
 *      هذه الفئة تُستخدم لفحص Guard بعد التحليل
 * 
 * (EN) Note: Actual parsing is done in parsePattern() in parser_core_impl.cpp
 *      This class is used to validate Guard after parsing
 */
class GuardParserInfo {
public:
    /**
     * @brief (AR) الكلمات المفتاحية للشرط
     *        (EN) Guard keywords
     */
    static bool isGuardKeyword(const std::string& keyword) {
        return keyword == "if" || keyword == "إذا" ||
               keyword == "when" || keyword == "عندما";
    }
    
    /**
     * @brief (AR) أسماء الشروط العربية
     *        (EN) Arabic condition names
     */
    static std::string translateOperator(const std::string& op) {
        static const std::unordered_map<std::string, std::string> translations = {
            {"==", "يساوي"},
            {"!=", "لا يساوي"},
            {">", "أكبر من"},
            {"<", "أصغر من"},
            {">=", "أكبر أو يساوي"},
            {"<=", "أصغر أو يساوي"},
            {"&&", "و"},
            {"||", "أو"},
            {"!", "ليس"}
        };
        
        auto it = translations.find(op);
        return it != translations.end() ? it->second : op;
    }
};

// ════════════════════════════════════════════════════════════════════════════════
// واجهة خارجية / External Interface
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) فحص شرط guard
 *        (EN) Check guard condition
 */
GuardCheckResult checkGuardCondition(
    const AST::Expression& guard,
    const std::unordered_set<std::string>& boundVariables
) {
    GuardChecker checker;
    return checker.checkGuard(guard, boundVariables);
}

/**
 * @brief (AR) فحص جميع Guards في match
 *        (EN) Check all guards in match
 */
std::pair<std::vector<std::string>, std::vector<std::string>> checkMatchGuards(
    const AST::MatchStmt& matchStmt
) {
    GuardChecker checker;
    return checker.checkAllGuards(matchStmt);
}

/**
 * @brief (AR) هل هذه كلمة مفتاحية guard؟
 *        (EN) Is this a guard keyword?
 */
bool isGuardKeyword(const std::string& keyword) {
    return GuardParserInfo::isGuardKeyword(keyword);
}

} // namespace Parser
} // namespace Sad

// ════════════════════════════════════════════════════════════════════════════════
// نهاية الملف / End of File
// ════════════════════════════════════════════════════════════════════════════════
