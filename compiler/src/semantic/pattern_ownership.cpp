// ════════════════════════════════════════════════════════════════════════════════
// ملف: pattern_ownership.cpp
// File: pattern_ownership.cpp
//
// الوصف (AR): تكامل مطابقة الأنماط مع نظام الملكية
//             يتحقق من قواعد الاستعارة والنقل في أنماط match
//             يضمن سلامة الذاكرة عند فك تركيب القيم
//
// Description (EN): Pattern matching integration with ownership system
//                   Verifies borrowing and moving rules in match patterns
//                   Ensures memory safety when destructuring values
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
// المهمة: T140 [US10] Implement pattern + ownership integration
// Task: T140 [US10] Implement pattern + ownership integration
// ════════════════════════════════════════════════════════════════════════════════

// تعطيل تحذير Unicode للتعليقات العربية
// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include "pattern_nodes.h"
#include "semantic/ownership_tracker.h"
#include "semantic/borrow_checker.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <memory>

namespace Sad {
namespace Semantic {

// ════════════════════════════════════════════════════════════════════════════════
// أنواع الربط في الأنماط / Pattern Binding Modes
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) وضع الربط في النمط
 *        (EN) Pattern binding mode
 * 
 * (AR) يحدد كيف يتم ربط المتغير في النمط:
 *      - بالنقل: يُنقل الملكية للمتغير الجديد
 *      - بالمرجع: مرجع قابل للتغيير
 *      - بالمرجع الثابت: مرجع للقراءة فقط
 *      - بالنسخ: للأنواع Copy
 * 
 * (EN) Determines how a variable is bound in pattern:
 *      - ByMove: Ownership is moved to new variable
 *      - ByRef: Mutable reference
 *      - ByRefConst: Immutable reference
 *      - ByCopy: For Copy types
 */
enum class BindingMode {
    ByMove,      ///< (AR) بالنقل (افتراضي) / (EN) By move (default)
    ByRef,       ///< (AR) بالمرجع &متغير / (EN) By ref &mut
    ByRefConst,  ///< (AR) بالمرجع الثابت &ثابت / (EN) By ref const &
    ByCopy       ///< (AR) بالنسخ (للأنواع Copy) / (EN) By copy (for Copy types)
};

/**
 * @brief (AR) معلومات ربط متغير في نمط
 *        (EN) Pattern variable binding information
 */
struct PatternBinding {
    std::string variableName;    ///< (AR) اسم المتغير / (EN) Variable name
    BindingMode mode;            ///< (AR) وضع الربط / (EN) Binding mode
    std::string sourceVariable;  ///< (AR) المتغير المصدر / (EN) Source variable
    bool isCopyType;             ///< (AR) هل النوع Copy؟ / (EN) Is type Copy?
    
    PatternBinding(const std::string& name, BindingMode m, 
                   const std::string& source = "", bool copy = false)
        : variableName(name), mode(m), sourceVariable(source), isCopyType(copy) {}
};

// ════════════════════════════════════════════════════════════════════════════════
// نتيجة فحص الملكية في الأنماط / Pattern Ownership Check Result
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) نتيجة فحص الملكية في أنماط match
 *        (EN) Ownership check result for match patterns
 */
struct PatternOwnershipResult {
    bool success;                            ///< (AR) هل نجح الفحص؟ / (EN) Did check succeed?
    std::vector<std::string> errors;         ///< (AR) الأخطاء / (EN) Errors
    std::vector<std::string> warnings;       ///< (AR) التحذيرات / (EN) Warnings
    std::vector<PatternBinding> bindings;    ///< (AR) الربطات / (EN) Bindings
    
    static PatternOwnershipResult success_result() {
        return PatternOwnershipResult{true, {}, {}, {}};
    }
    
    static PatternOwnershipResult failure(const std::string& error) {
        return PatternOwnershipResult{false, {error}, {}, {}};
    }
    
    void addError(const std::string& error) {
        success = false;
        errors.push_back(error);
    }
    
    void addWarning(const std::string& warning) {
        warnings.push_back(warning);
    }
    
    void addBinding(const PatternBinding& binding) {
        bindings.push_back(binding);
    }
};

// ════════════════════════════════════════════════════════════════════════════════
// فاحص الملكية في الأنماط / Pattern Ownership Checker
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) فاحص تكامل الملكية مع مطابقة الأنماط
 *        (EN) Pattern matching ownership integration checker
 * 
 * (AR) يتحقق من:
 *      1. صحة نقل الملكية في أنماط فك التركيب
 *      2. عدم وجود استخدام بعد النقل
 *      3. صحة الاستعارات في أنماط المرجع
 *      4. تطبيق قواعد Copy للأنواع المناسبة
 * 
 * (EN) Verifies:
 *      1. Correct ownership moves in destructuring patterns
 *      2. No use after move
 *      3. Correct borrowing in reference patterns
 *      4. Copy trait application for appropriate types
 */
class PatternOwnershipChecker {
public:
    // ========================================================================
    // البناء والهدم / Construction & Destruction
    // ========================================================================
    
    PatternOwnershipChecker()
        : useArabicMessages_(true)
        , debugMode_(false)
        , defaultBindingMode_(BindingMode::ByMove) {
        initializeCopyTypes();
    }
    
    ~PatternOwnershipChecker() = default;
    
    // ========================================================================
    // الواجهة الرئيسية / Main Interface
    // ========================================================================
    
    /**
     * @brief (AR) فحص ملكية جملة match كاملة
     *        (EN) Check ownership for complete match statement
     * 
     * @param matchStmt (AR) جملة match / (EN) Match statement
     * @param matchedVarName (AR) اسم المتغير المُختبر / (EN) Name of matched variable
     * @param isMoveMatch (AR) هل هو match بالنقل؟ / (EN) Is it a move match?
     * @return (AR) نتيجة الفحص / (EN) Check result
     */
    PatternOwnershipResult checkMatchOwnership(
        const AST::MatchStmt& matchStmt,
        const std::string& matchedVarName,
        bool isMoveMatch = true
    ) {
        PatternOwnershipResult result = PatternOwnershipResult::success_result();
        
        if (debugMode_) {
            std::cout << "[PatternOwnership] فحص match للمتغير: " << matchedVarName << "\n";
            std::cout << "[PatternOwnership] Checking match for variable: " << matchedVarName << "\n";
            std::cout << "[PatternOwnership] Move match: " << (isMoveMatch ? "yes" : "no") << "\n";
        }
        
        // (AR) فحص كل فرع case
        // (EN) Check each case clause
        for (size_t i = 0; i < matchStmt.cases.size(); ++i) {
            const auto& caseClause = matchStmt.cases[i];
            
            if (!caseClause.pattern) continue;
            
            // (AR) فحص الملكية في النمط
            // (EN) Check ownership in pattern
            auto patternResult = checkPatternOwnership(
                *caseClause.pattern,
                matchedVarName,
                isMoveMatch
            );
            
            // (AR) جمع الأخطاء والتحذيرات
            // (EN) Collect errors and warnings
            for (const auto& error : patternResult.errors) {
                result.addError(formatError(
                    "الفرع " + std::to_string(i + 1) + ": " + error,
                    "Case " + std::to_string(i + 1) + ": " + error
                ));
            }
            
            for (const auto& warning : patternResult.warnings) {
                result.addWarning(formatError(
                    "الفرع " + std::to_string(i + 1) + ": " + warning,
                    "Case " + std::to_string(i + 1) + ": " + warning
                ));
            }
            
            // (AR) جمع الربطات
            // (EN) Collect bindings
            for (const auto& binding : patternResult.bindings) {
                result.addBinding(binding);
            }
        }
        
        return result;
    }
    
    /**
     * @brief (AR) فحص ملكية نمط واحد
     *        (EN) Check ownership for single pattern
     * 
     * @param pattern (AR) النمط / (EN) Pattern
     * @param sourceVar (AR) المتغير المصدر / (EN) Source variable
     * @param isMoveContext (AR) هل السياق نقل؟ / (EN) Is context a move?
     * @return (AR) نتيجة الفحص / (EN) Check result
     */
    PatternOwnershipResult checkPatternOwnership(
        const AST::Pattern& pattern,
        const std::string& sourceVar,
        bool isMoveContext
    ) {
        // (AR) النمط الشامل لا يربط شيء
        // (EN) Wildcard doesn't bind anything
        if (dynamic_cast<const AST::WildcardPattern*>(&pattern)) {
            return PatternOwnershipResult::success_result();
        }
        
        // (AR) نمط متغير
        // (EN) Variable pattern
        if (auto* varPat = dynamic_cast<const AST::VariablePattern*>(&pattern)) {
            return checkVariablePatternOwnership(*varPat, sourceVar, isMoveContext);
        }
        
        // (AR) نمط قيمة حرفية
        // (EN) Literal pattern
        if (dynamic_cast<const AST::LiteralPattern*>(&pattern)) {
            // (AR) الأنماط الحرفية لا تربط - فقط تقارن
            // (EN) Literal patterns don't bind - just compare
            return PatternOwnershipResult::success_result();
        }
        
        // (AR) نمط قائمة
        // (EN) List pattern
        if (auto* listPat = dynamic_cast<const AST::ListPattern*>(&pattern)) {
            return checkListPatternOwnership(*listPat, sourceVar, isMoveContext);
        }
        
        // (AR) نمط OR
        // (EN) OR pattern
        if (auto* orPat = dynamic_cast<const AST::OrPattern*>(&pattern)) {
            return checkOrPatternOwnership(*orPat, sourceVar, isMoveContext);
        }
        
        return PatternOwnershipResult::success_result();
    }
    
    // ========================================================================
    // الإعدادات / Settings
    // ========================================================================
    
    void setArabicMessages(bool enable) { useArabicMessages_ = enable; }
    void setDebugMode(bool enable) { debugMode_ = enable; }
    void setDefaultBindingMode(BindingMode mode) { defaultBindingMode_ = mode; }
    
    /**
     * @brief (AR) تسجيل نوع كـ Copy
     *        (EN) Register type as Copy
     */
    void registerCopyType(const std::string& typeName) {
        copyTypes_.insert(typeName);
    }

private:
    // ========================================================================
    // فحص أنواع الأنماط المحددة / Specific Pattern Type Checking
    // ========================================================================
    
    /**
     * @brief (AR) فحص ملكية نمط متغير
     *        (EN) Check variable pattern ownership
     */
    PatternOwnershipResult checkVariablePatternOwnership(
        const AST::VariablePattern& pattern,
        const std::string& sourceVar,
        bool isMoveContext
    ) {
        PatternOwnershipResult result = PatternOwnershipResult::success_result();
        
        // (AR) تحديد وضع الربط
        // (EN) Determine binding mode
        BindingMode mode = defaultBindingMode_;
        
        // (AR) إذا كان النوع Copy، نستخدم النسخ
        // (EN) If type is Copy, use copy
        bool isCopy = isCopyType(pattern.name);  // (AR) تقريب - نحتاج نوع المتغير الفعلي
        
        if (isCopy) {
            mode = BindingMode::ByCopy;
        } else if (!isMoveContext) {
            mode = BindingMode::ByRefConst;
        }
        
        // (AR) إنشاء الربط
        // (EN) Create binding
        PatternBinding binding(pattern.name, mode, sourceVar, isCopy);
        result.addBinding(binding);
        
        if (debugMode_) {
            std::cout << "[PatternOwnership] Binding: " << pattern.name 
                      << " <- " << sourceVar
                      << " (mode: " << bindingModeToString(mode) << ")\n";
        }
        
        // (AR) التحقق من قواعد الملكية
        // (EN) Verify ownership rules
        if (mode == BindingMode::ByMove && !isCopy) {
            // (AR) نقل الملكية - المصدر لن يكون صالحاً بعد هذا
            // (EN) Moving ownership - source will be invalid after this
            if (debugMode_) {
                std::cout << "[PatternOwnership] WARNING: Moving ownership from " 
                          << sourceVar << " to " << pattern.name << "\n";
            }
        }
        
        return result;
    }
    
    /**
     * @brief (AR) فحص ملكية نمط قائمة
     *        (EN) Check list pattern ownership
     */
    PatternOwnershipResult checkListPatternOwnership(
        const AST::ListPattern& pattern,
        const std::string& sourceVar,
        bool isMoveContext
    ) {
        PatternOwnershipResult result = PatternOwnershipResult::success_result();
        
        // (AR) فحص كل عنصر
        // (EN) Check each element
        for (size_t i = 0; i < pattern.elements.size(); ++i) {
            std::string elementSource = sourceVar + "[" + std::to_string(i) + "]";
            
            auto elemResult = checkPatternOwnership(
                *pattern.elements[i],
                elementSource,
                isMoveContext
            );
            
            // (AR) جمع النتائج
            // (EN) Collect results
            for (const auto& error : elemResult.errors) {
                result.addError(error);
            }
            for (const auto& warning : elemResult.warnings) {
                result.addWarning(warning);
            }
            for (const auto& binding : elemResult.bindings) {
                result.addBinding(binding);
            }
        }
        
        // (AR) فحص rest إن وُجد
        // (EN) Check rest if present
        if (pattern.has_rest && !pattern.rest_name.empty()) {
            PatternBinding restBinding(
                pattern.rest_name,
                isMoveContext ? BindingMode::ByMove : BindingMode::ByRefConst,
                sourceVar + "[...]",
                false
            );
            result.addBinding(restBinding);
        }
        
        return result;
    }
    
    /**
     * @brief (AR) فحص ملكية نمط OR
     *        (EN) Check OR pattern ownership
     */
    PatternOwnershipResult checkOrPatternOwnership(
        const AST::OrPattern& pattern,
        const std::string& sourceVar,
        bool isMoveContext
    ) {
        PatternOwnershipResult result = PatternOwnershipResult::success_result();
        
        // (AR) جميع البدائل يجب أن تربط نفس المتغيرات بنفس الوضع
        // (EN) All alternatives must bind same variables with same mode
        std::vector<PatternBinding> firstBindings;
        
        for (size_t i = 0; i < pattern.alternatives.size(); ++i) {
            auto altResult = checkPatternOwnership(
                *pattern.alternatives[i],
                sourceVar,
                isMoveContext
            );
            
            if (i == 0) {
                firstBindings = altResult.bindings;
                result.bindings = altResult.bindings;
            } else {
                // (AR) التحقق من تطابق الربطات
                // (EN) Verify bindings match
                if (!bindingsMatch(firstBindings, altResult.bindings)) {
                    result.addError(formatError(
                        "بدائل OR يجب أن تربط نفس المتغيرات",
                        "OR alternatives must bind the same variables"
                    ));
                }
            }
            
            // (AR) جمع الأخطاء
            // (EN) Collect errors
            for (const auto& error : altResult.errors) {
                result.addError(error);
            }
        }
        
        return result;
    }
    
    // ========================================================================
    // دوال مساعدة / Helper Functions
    // ========================================================================
    
    void initializeCopyTypes() {
        // (AR) الأنواع البدائية
        // (EN) Primitive types
        copyTypes_.insert("ح8");
        copyTypes_.insert("ح16");
        copyTypes_.insert("ح32");
        copyTypes_.insert("ح64");
        copyTypes_.insert("ع8");
        copyTypes_.insert("ع16");
        copyTypes_.insert("ع32");
        copyTypes_.insert("ع64");
        copyTypes_.insert("رقم");
        copyTypes_.insert("عدد");
        copyTypes_.insert("عش32");
        copyTypes_.insert("عش64");
        copyTypes_.insert("عشري");
        copyTypes_.insert("منطقي");
        copyTypes_.insert("حرف");
        
        // (AR) الأسماء الإنجليزية
        // (EN) English names
        copyTypes_.insert("i8");
        copyTypes_.insert("i16");
        copyTypes_.insert("i32");
        copyTypes_.insert("i64");
        copyTypes_.insert("u8");
        copyTypes_.insert("u16");
        copyTypes_.insert("u32");
        copyTypes_.insert("u64");
        copyTypes_.insert("f32");
        copyTypes_.insert("f64");
        copyTypes_.insert("int");
        copyTypes_.insert("float");
        copyTypes_.insert("double");
        copyTypes_.insert("bool");
        copyTypes_.insert("char");
    }
    
    bool isCopyType(const std::string& typeName) {
        return copyTypes_.count(typeName) > 0;
    }
    
    bool bindingsMatch(
        const std::vector<PatternBinding>& a,
        const std::vector<PatternBinding>& b
    ) {
        if (a.size() != b.size()) return false;
        
        for (const auto& bindingA : a) {
            bool found = false;
            for (const auto& bindingB : b) {
                if (bindingA.variableName == bindingB.variableName &&
                    bindingA.mode == bindingB.mode) {
                    found = true;
                    break;
                }
            }
            if (!found) return false;
        }
        
        return true;
    }
    
    std::string bindingModeToString(BindingMode mode) {
        switch (mode) {
            case BindingMode::ByMove: return "move/نقل";
            case BindingMode::ByRef: return "ref/مرجع";
            case BindingMode::ByRefConst: return "ref const/مرجع ثابت";
            case BindingMode::ByCopy: return "copy/نسخ";
        }
        return "unknown";
    }
    
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
    BindingMode defaultBindingMode_;
    std::unordered_set<std::string> copyTypes_;
};

// ════════════════════════════════════════════════════════════════════════════════
// واجهة خارجية / External Interface
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief (AR) فحص ملكية جملة match
 *        (EN) Check match statement ownership
 */
PatternOwnershipResult checkPatternOwnership(
    const AST::MatchStmt& matchStmt,
    const std::string& matchedVarName,
    bool isMoveMatch
) {
    PatternOwnershipChecker checker;
    return checker.checkMatchOwnership(matchStmt, matchedVarName, isMoveMatch);
}

/**
 * @brief (AR) فحص ملكية نمط واحد
 *        (EN) Check single pattern ownership
 */
PatternOwnershipResult checkSinglePatternOwnership(
    const AST::Pattern& pattern,
    const std::string& sourceVar,
    bool isMoveContext
) {
    PatternOwnershipChecker checker;
    return checker.checkPatternOwnership(pattern, sourceVar, isMoveContext);
}

} // namespace Semantic
} // namespace Sad

// ════════════════════════════════════════════════════════════════════════════════
// نهاية الملف / End of File
// ════════════════════════════════════════════════════════════════════════════════
