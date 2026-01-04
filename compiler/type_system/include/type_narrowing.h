/**
 * @file type_narrowing.h
 * @brief Type Narrowing System / نظام تضييق الأنواع
 * @details Type state tracking and control flow analysis for type guards
 *          تتبع حالة الأنواع وتحليل التدفق للحراس
 * 
 * Phase 1.3.5.2: Type Narrowing Integration
 * 
 * Features:
 * - Type state tracking in control flow / تتبع حالة الأنواع في التدفق
 * - Scope-based narrowing contexts / سياقات التضييق حسب النطاق
 * - If/else branch narrowing / تضييق في فروع if/else
 * - Match expression narrowing / تضييق في تعبيرات match
 * - Integration with Type Guards / تكامل مع حراس الأنواع
 * 
 * @author Sad Language Team
 * @date 2026-01-03
 */

#pragma once

#include "type.h"
#include "type_guard.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Sad {
namespace TypeSystem {

// Forward declarations
class TypeNarrowingContext;
class TypeNarrowingScope;

// ════════════════════════════════════════════════════════════════════════════════
// Type Narrowing Context
// سياق تضييق الأنواع
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief سياق تضييق الأنواع / Type narrowing context
 * @details يتتبع حالة نوع المتغيرات في نطاق معين
 *          Tracks variable type state within a scope
 * 
 * Example:
 * @code
 * // قيمة: رقم | نص
 * إذا (قيمة نوع رقم) {
 *     // هنا: قيمة هو رقم (narrowed)
 *     اطبع(قيمة + 10)  // OK!
 * } وإلا {
 *     // هنا: قيمة هو نص (narrowed else)
 *     اطبع(طول_نص(قيمة))  // OK!
 * }
 * @endcode
 */
class TypeNarrowingContext {
public:
    TypeNarrowingContext() = default;
    ~TypeNarrowingContext() = default;

    // ════════════════════════════════════════════════════════════════════════════
    // Scope Management / إدارة النطاقات
    // ════════════════════════════════════════════════════════════════════════════

    /**
     * @brief دخول نطاق جديد / Enter new scope
     * @details ينشئ نطاق تضييق جديد يرث من النطاق الحالي
     *          Creates new narrowing scope inheriting from current
     */
    void pushScope();

    /**
     * @brief الخروج من النطاق الحالي / Exit current scope
     * @details يعيد إلى النطاق السابق
     *          Returns to previous scope
     */
    void popScope();

    /**
     * @brief الحصول على عمق النطاق / Get scope depth
     * @return عدد النطاقات المتداخلة / Number of nested scopes
     */
    size_t getScopeDepth() const { return scopes_.size(); }

    // ════════════════════════════════════════════════════════════════════════════
    // Variable Type Tracking / تتبع أنواع المتغيرات
    // ════════════════════════════════════════════════════════════════════════════

    /**
     * @brief تسجيل نوع متغير / Register variable type
     * @param varName اسم المتغير / Variable name
     * @param type النوع الأصلي / Original type
     * @details يحفظ النوع الأصلي للمتغير قبل أي تضييق
     *          Saves original variable type before any narrowing
     */
    void registerVariable(const std::string& varName, TypePtr type);

    /**
     * @brief تضييق نوع متغير / Narrow variable type
     * @param varName اسم المتغير / Variable name
     * @param narrowedType النوع المضيّق / Narrowed type
     * @details يطبق تضييق على نوع المتغير في النطاق الحالي
     *          Applies narrowing to variable type in current scope
     */
    void narrowVariable(const std::string& varName, TypePtr narrowedType);

    /**
     * @brief الحصول على نوع متغير / Get variable type
     * @param varName اسم المتغير / Variable name
     * @return النوع المضيّق أو الأصلي / Narrowed or original type
     * @details يُرجع النوع المضيّق إن وُجد، وإلا النوع الأصلي
     *          Returns narrowed type if exists, otherwise original
     */
    TypePtr getVariableType(const std::string& varName) const;

    /**
     * @brief التحقق من وجود متغير / Check if variable exists
     * @param varName اسم المتغير / Variable name
     * @return true إذا كان المتغير مسجلاً / if variable is registered
     */
    bool hasVariable(const std::string& varName) const;

    /**
     * @brief الحصول على النوع الأصلي للمتغير / Get original variable type
     * @param varName اسم المتغير / Variable name
     * @return النوع الأصلي قبل التضييق / Original type before narrowing
     */
    TypePtr getOriginalType(const std::string& varName) const;

    // ════════════════════════════════════════════════════════════════════════════
    // Guard Application / تطبيق الحراس
    // ════════════════════════════════════════════════════════════════════════════

    /**
     * @brief تطبيق حارس نوع على متغير / Apply type guard to variable
     * @param varName اسم المتغير / Variable name
     * @param guard حارس النوع / Type guard
     * @return true إذا نجح التضييق / if narrowing succeeded
     * @details يطبق حارس النوع ويُضيّق النوع إذا نجح
     *          Applies type guard and narrows type if successful
     */
    bool applyGuard(const std::string& varName, TypeGuardPtr guard);

    /**
     * @brief تطبيق حارس معكوس (else branch) / Apply inverted guard (else branch)
     * @param varName اسم المتغير / Variable name
     * @param guard حارس النوع / Type guard
     * @return true إذا نجح التضييق / if narrowing succeeded
     * @details يطبق narrowElse() للحصول على النوع في else branch
     *          Applies narrowElse() to get type in else branch
     */
    bool applyElseGuard(const std::string& varName, TypeGuardPtr guard);

    // ════════════════════════════════════════════════════════════════════════════
    // Branch Merging / دمج الفروع
    // ════════════════════════════════════════════════════════════════════════════

    /**
     * @brief دمج نطاقين (بعد if-else) / Merge two scopes (after if-else)
     * @param thenContext سياق then branch / Then branch context
     * @param elseContext سياق else branch / Else branch context
     * @details يدمج معلومات التضييق من فرعي if-else
     *          Merges narrowing info from both if-else branches
     * 
     * Rules:
     * - إذا ضُيّق في كلا الفرعين → Union / If narrowed in both → Union
     * - إذا ضُيّق في فرع واحد → النوع الأصلي / If narrowed in one → Original type
     */
    void mergeBranches(const TypeNarrowingContext& thenContext,
                      const TypeNarrowingContext& elseContext);

    /**
     * @brief دمج نطاق واحد (بعد if بدون else) / Merge single scope (after if without else)
     * @param branchContext سياق الفرع / Branch context
     * @details يدمج معلومات من فرع if عندما لا يوجد else
     *          Merges info from if branch when no else exists
     */
    void mergeSingleBranch(const TypeNarrowingContext& branchContext);

    // ════════════════════════════════════════════════════════════════════════════
    // Debugging / التنقيح
    // ════════════════════════════════════════════════════════════════════════════

    /**
     * @brief طباعة حالة السياق / Print context state
     * @details للتنقيح - يطبع جميع المتغيرات وأنواعها
     *          For debugging - prints all variables and their types
     */
    void dump() const;

private:
    /**
     * @brief نطاق تضييق واحد / Single narrowing scope
     * @details يحتوي على خريطة المتغيرات → الأنواع المضيّقة
     *          Contains map of variables → narrowed types
     */
    struct Scope {
        // خريطة: اسم المتغير → النوع المضيّق
        // Map: variable name → narrowed type
        std::unordered_map<std::string, TypePtr> narrowedTypes;
    };

    // ════════════════════════════════════════════════════════════════════════════
    // Private Members / الأعضاء الخاصة
    // ════════════════════════════════════════════════════════════════════════════

    // مجموعة النطاقات (stack) / Stack of scopes
    std::vector<Scope> scopes_;

    // الأنواع الأصلية للمتغيرات / Original variable types
    std::unordered_map<std::string, TypePtr> originalTypes_;
};

// ════════════════════════════════════════════════════════════════════════════════
// Type Narrowing Analyzer
// محلل تضييق الأنواع
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief محلل تضييق الأنواع / Type narrowing analyzer
 * @details يحلل control flow ويطبق تضييق الأنواع
 *          Analyzes control flow and applies type narrowing
 * 
 * Used by:
 * - Semantic Analyzer (compile-time)
 * - Type Checker (compile-time)
 * - IDE (for intellisense)
 */
class TypeNarrowingAnalyzer {
public:
    TypeNarrowingAnalyzer() = default;
    ~TypeNarrowingAnalyzer() = default;

    // ════════════════════════════════════════════════════════════════════════════
    // Context Management / إدارة السياق
    // ════════════════════════════════════════════════════════════════════════════

    /**
     * @brief الحصول على السياق الحالي / Get current context
     * @return مؤشر للسياق / Pointer to context
     */
    TypeNarrowingContext* getCurrentContext() { return &currentContext_; }

    /**
     * @brief إعادة تعيين السياق / Reset context
     * @details يبدأ من سياق جديد نظيف
     *          Starts with fresh clean context
     */
    void resetContext();

    // ════════════════════════════════════════════════════════════════════════════
    // Control Flow Analysis / تحليل التدفق
    // ════════════════════════════════════════════════════════════════════════════

    /**
     * @brief تحليل جملة if / Analyze if statement
     * @param condition تعبير الشرط / Condition expression
     * @param thenBlock كتلة then / Then block
     * @param elseBlock كتلة else (اختياري) / Else block (optional)
     * @details يطبق تضييق في كلا الفرعين حسب الشرط
     *          Applies narrowing in both branches based on condition
     * 
     * Example:
     * @code
     * إذا (قيمة نوع رقم) {
     *     // then: narrow to رقم
     * } وإلا {
     *     // else: narrow else (e.g., نص)
     * }
     * @endcode
     */
    void analyzeIfStatement(void* condition, void* thenBlock, void* elseBlock);

    /**
     * @brief استخراج حارس من تعبير شرط / Extract guard from condition expression
     * @param condition تعبير الشرط / Condition expression
     * @param varName [out] اسم المتغير / Variable name
     * @return حارس النوع إن وُجد / Type guard if found, nullptr otherwise
     * @details يحاول استخراج Type Guard من تعبيرات مثل:
     *          Tries to extract Type Guard from expressions like:
     *          - x is Integer → IsType guard
     *          - typeof(x) == "integer" → TypeOf guard
     *          - x != null → NotNull guard
     */
    TypeGuardPtr extractGuardFromCondition(void* condition, std::string& varName);

    // ════════════════════════════════════════════════════════════════════════════
    // Utility Functions / دوال مساعدة
    // ════════════════════════════════════════════════════════════════════════════

    /**
     * @brief إنشاء Union من نوعين / Create Union of two types
     * @param type1 النوع الأول / First type
     * @param type2 النوع الثاني / Second type
     * @return Union type أو null / Union type or null
     * @details إذا كان النوعان نفسهما، يُرجع أحدهما
     *          If types are same, returns one of them
     */
    static TypePtr createUnionType(TypePtr type1, TypePtr type2);

private:
    // السياق الحالي / Current context
    TypeNarrowingContext currentContext_;
};

// ════════════════════════════════════════════════════════════════════════════════
// Global Helper Functions
// دوال مساعدة عامة
// ════════════════════════════════════════════════════════════════════════════════

/**
 * @brief التحقق من إمكانية تضييق متغير / Check if variable can be narrowed
 * @param varName اسم المتغير / Variable name
 * @param type نوع المتغير / Variable type
 * @param guard حارس النوع / Type guard
 * @return true إذا كان التضييق ممكناً / if narrowing is possible
 */
bool canNarrowVariable(const std::string& varName, TypePtr type, TypeGuardPtr guard);

/**
 * @brief تطبيق تضييق على متغير / Apply narrowing to variable
 * @param context السياق / Context
 * @param varName اسم المتغير / Variable name
 * @param guard حارس النوع / Type guard
 * @return النوع المضيّق / Narrowed type
 */
TypePtr applyNarrowingToVariable(TypeNarrowingContext& context,
                                 const std::string& varName,
                                 TypeGuardPtr guard);

} // namespace TypeSystem
} // namespace Sad
