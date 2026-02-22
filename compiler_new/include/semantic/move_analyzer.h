// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4819)
#endif

/**
 * @file move_analyzer.h
 * @brief محلل دلالات النقل / Move Semantics Analyzer
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety
 * 
 * =============================================================================
 *                         نظرة عامة / Overview
 * =============================================================================
 * 
 * (AR) هذا الملف يُنفِّذ محلل دلالات النقل (Move Semantics) للغة ص.
 *      دلالات النقل هي مفهوم أساسي في إدارة الذاكرة الآمنة، حيث:
 * 
 *      1. **النقل (Move)**: عند تمرير قيمة غير قابلة للنسخ، تُنقل الملكية
 *         بدلاً من نسخها. المتغير الأصلي يصبح "منقولاً" ولا يمكن استخدامه.
 * 
 *      2. **النسخ (Copy)**: الأنواع البسيطة (الأرقام، المنطقي، الحرف) تُنسخ
 *         تلقائياً لأنها رخيصة التكلفة.
 * 
 *      3. **الاستنساخ (Clone)**: الأنواع المعقدة يمكن استنساخها صراحةً
 *         باستخدام الدالة `.استنسخ()` أو `.clone()`.
 * 
 * (EN) This file implements the Move Semantics Analyzer for Sad language.
 *      Move semantics are fundamental for safe memory management:
 * 
 *      1. **Move**: When passing a non-Copy value, ownership transfers
 *         instead of copying. The original variable becomes "moved".
 * 
 *      2. **Copy**: Simple types (numbers, bool, char) copy automatically
 *         because they're cheap to copy.
 * 
 *      3. **Clone**: Complex types can be explicitly cloned using
 *         `.clone()` or `.استنسخ()` method.
 * 
 * =============================================================================
 *                         أمثلة / Examples
 * =============================================================================
 * 
 * @code{.ص}
 * // مثال 1: النقل التلقائي
 * // Example 1: Automatic Move
 * متغير نص1 = "مرحباً"
 * متغير نص2 = نص1        // نص1 نُقل إلى نص2
 * // اطبع(نص1)           // خطأ! نص1 منقول
 * 
 * // مثال 2: الاستنساخ الصريح
 * // Example 2: Explicit Clone
 * متغير نص3 = "أهلاً"
 * متغير نص4 = نص3.استنسخ()  // نص3 لا يزال صالحاً
 * اطبع(نص3)               // يعمل!
 * 
 * // مثال 3: النسخ التلقائي للأنواع البسيطة
 * // Example 3: Automatic Copy for Simple Types
 * متغير عدد1 = 42
 * متغير عدد2 = عدد1       // نُسخ تلقائياً
 * اطبع(عدد1)              // يعمل! الأرقام قابلة للنسخ
 * 
 * // مثال 4: النقل في استدعاء الدوال
 * // Example 4: Move in Function Calls
 * دالة استهلك(نص: نصي) {
 *     اطبع(نص)
 * }
 * متغير رسالة = "تحية"
 * استهلك(رسالة)           // رسالة نُقلت للدالة
 * // استهلك(رسالة)        // خطأ! رسالة منقولة
 * @endcode
 * 
 * =============================================================================
 *                   قواعد النقل / Move Rules
 * =============================================================================
 * 
 * (AR) قواعد النقل في لغة ص:
 * 
 *      1. الأنواع غير القابلة للنسخ تُنقل دائماً:
 *         - النصوص (نصي/String)
 *         - المتجهات (متجه/Vec)
 *         - الخرائط (قاموس/HashMap)
 *         - الهياكل المخصصة (ما لم تُنفذ سمة قابل_للنسخ)
 * 
 *      2. النقل يحدث في:
 *         - الإسناد: `س = ص` ينقل ص إلى س
 *         - تمرير للدوال: `دالة(س)` ينقل س
 *         - الإرجاع: `إرجاع س` ينقل س للمستدعي
 * 
 *      3. منع استخدام المتغير بعد النقل:
 *         - أي استخدام للمتغير بعد نقله يُنتج خطأ ترجمة
 *         - يمكن إعادة تعيين المتغير بقيمة جديدة
 * 
 * (EN) Move rules in Sad language:
 * 
 *      1. Non-Copy types always move:
 *         - Strings
 *         - Vectors
 *         - HashMaps
 *         - Custom structs (unless they implement Copy)
 * 
 *      2. Move occurs in:
 *         - Assignment: `x = y` moves y to x
 *         - Function parameters: `func(x)` moves x
 *         - Return: `return x` moves x to caller
 * 
 *      3. Prevent use after move:
 *         - Any use after move produces compile error
 *         - Variable can be reassigned with new value
 * 
 * @see ownership_tracker.h
 * @see borrow_checker.h
 */

#ifndef SAD_MOVE_ANALYZER_H
#define SAD_MOVE_ANALYZER_H

#include "semantic/ownership_tracker.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <optional>

namespace Sad {
namespace Semantic {

// ============================================================================
//                    أنواع النقل / Move Types
// ============================================================================

/**
 * @enum MoveKind
 * @brief (AR) نوع عملية النقل
 * @brief (EN) Type of move operation
 * 
 * (AR) يحدد كيف حدث النقل - هل كان إسناداً، تمريراً لدالة، إلخ.
 * (EN) Specifies how the move occurred - assignment, function parameter, etc.
 */
enum class MoveKind {
    Assignment,         ///< (AR) نقل عبر الإسناد / (EN) Move via assignment
    FunctionParameter,  ///< (AR) نقل كمعامل دالة / (EN) Move as function parameter
    Return,             ///< (AR) نقل عبر الإرجاع / (EN) Move via return
    FieldAccess,        ///< (AR) نقل حقل من هيكل / (EN) Move field from struct
    PatternBinding,     ///< (AR) نقل عبر ربط النمط / (EN) Move via pattern binding
    ClosureCapture      ///< (AR) التقاط في إغلاق / (EN) Closure capture
};

/**
 * @enum MoveCheckResult
 * @brief (AR) نتيجة فحص إمكانية النقل
 * @brief (EN) Result of move possibility check
 */
enum class MoveCheckResult {
    CanMove,            ///< (AR) يمكن النقل / (EN) Can move
    AlreadyMoved,       ///< (AR) تم نقله مسبقاً / (EN) Already moved
    Borrowed,           ///< (AR) مستعار حالياً / (EN) Currently borrowed
    PartiallyMoved,     ///< (AR) منقول جزئياً / (EN) Partially moved
    IsCopyType          ///< (AR) نوع قابل للنسخ / (EN) Is Copy type
};

// ============================================================================
//                    معلومات النقل / Move Information
// ============================================================================

/**
 * @struct MoveInfo
 * @brief (AR) معلومات عملية نقل واحدة
 * @brief (EN) Information about a single move operation
 * 
 * (AR) يحتفظ بكل تفاصيل عملية النقل لاستخدامها في:
 *      - رسائل الخطأ
 *      - التنقيح
 *      - تحليل تدفق البيانات
 * 
 * (EN) Stores all details about a move operation for use in:
 *      - Error messages
 *      - Debugging
 *      - Data flow analysis
 */
struct MoveInfo {
    /// (AR) اسم المتغير المنقول / (EN) Name of moved variable
    std::string variableName;
    
    /// (AR) نوع المتغير / (EN) Variable type
    std::string typeName;
    
    /// (AR) نوع النقل / (EN) Move kind
    MoveKind kind;
    
    /// (AR) موقع حدوث النقل / (EN) Location where move occurred
    SourceLocation moveLocation;
    
    /// (AR) موقع تعريف المتغير الأصلي / (EN) Original variable definition location
    SourceLocation definitionLocation;
    
    /// (AR) الوجهة (اسم المتغير أو الدالة) / (EN) Destination (variable or function name)
    std::string destination;
    
    /// (AR) هل النقل جزئي؟ / (EN) Is this a partial move?
    bool isPartialMove = false;
    
    /// (AR) مسار الحقل (للنقل الجزئي) / (EN) Field path (for partial moves)
    std::string fieldPath;
    
    /**
     * @brief (AR) الحصول على وصف النقل بالعربية
     * @brief (EN) Get Arabic description of the move
     */
    std::string getArabicDescription() const;
    
    /**
     * @brief (AR) الحصول على وصف النقل بالإنجليزية
     * @brief (EN) Get English description of the move
     */
    std::string getEnglishDescription() const;
};

/**
 * @struct MoveError
 * @brief (AR) خطأ نقل مع تفاصيل كاملة
 * @brief (EN) Move error with full details
 */
struct MoveError {
    /// (AR) نوع الخطأ / (EN) Error type
    enum class Kind {
        UseAfterMove,           ///< (AR) استخدام بعد النقل / (EN) Use after move
        MoveOfBorrowed,         ///< (AR) نقل متغير مستعار / (EN) Move of borrowed variable
        PartialMoveConflict,    ///< (AR) تعارض نقل جزئي / (EN) Partial move conflict
        DoubleMove,             ///< (AR) نقل مزدوج / (EN) Double move
        MoveInLoop,             ///< (AR) نقل داخل حلقة / (EN) Move inside loop
        CapturedMoved           ///< (AR) ملتقط ومنقول / (EN) Captured and moved
    };
    
    Kind kind;
    std::string message;
    std::string arabicMessage;
    SourceLocation useLocation;      ///< (AR) موقع الاستخدام / (EN) Use location
    SourceLocation moveLocation;     ///< (AR) موقع النقل / (EN) Move location
    std::string variableName;
    std::string suggestion;
    
    std::string toArabicString() const;
    std::string toEnglishString() const;
};

// ============================================================================
//                    محلل النقل / Move Analyzer
// ============================================================================

/**
 * @class MoveAnalyzer
 * @brief (AR) محلل دلالات النقل الرئيسي
 * @brief (EN) Main move semantics analyzer
 * 
 * ==========================================================================
 *                    الوظائف الرئيسية / Main Functions
 * ==========================================================================
 * 
 * (AR) يقوم المحلل بـ:
 * 
 *      1. **تتبع عمليات النقل**: يسجل كل عملية نقل تحدث
 *      2. **كشف الاستخدام بعد النقل**: يكشف محاولات استخدام متغير منقول
 *      3. **تحليل النقل الجزئي**: يتتبع نقل أجزاء من الهياكل
 *      4. **تحليل الحلقات**: يكشف النقل غير الآمن داخل الحلقات
 *      5. **تحليل الإغلاقات**: يتتبع الالتقاط والنقل في الإغلاقات
 * 
 * (EN) The analyzer does:
 * 
 *      1. **Track move operations**: Records every move that occurs
 *      2. **Detect use after move**: Detects attempts to use moved variables
 *      3. **Partial move analysis**: Tracks moves of struct fields
 *      4. **Loop analysis**: Detects unsafe moves inside loops
 *      5. **Closure analysis**: Tracks captures and moves in closures
 * 
 * ==========================================================================
 *                    مثال الاستخدام / Usage Example
 * ==========================================================================
 * 
 * @code{.cpp}
 * MoveAnalyzer analyzer;
 * 
 * // تسجيل تعريف متغير جديد
 * analyzer.declareVariable("رسالة", "نصي", location);
 * 
 * // تسجيل عملية نقل
 * auto result = analyzer.recordMove("رسالة", MoveKind::Assignment, 
 *                                   location, "رسالة2");
 * 
 * // التحقق من إمكانية الاستخدام
 * if (analyzer.isVariableMoved("رسالة")) {
 *     // خطأ: المتغير منقول
 * }
 * @endcode
 */
class MoveAnalyzer {
public:
    // ======================================================================
    //                    البناء / Construction
    // ======================================================================
    
    MoveAnalyzer();
    ~MoveAnalyzer();
    
    // ======================================================================
    //                    إدارة المتغيرات / Variable Management
    // ======================================================================
    
    /**
     * @brief (AR) تسجيل تعريف متغير جديد
     * @brief (EN) Register new variable definition
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @param typeName (AR) نوع المتغير / (EN) Type name
     * @param location (AR) موقع التعريف / (EN) Definition location
     * @param isCopyType (AR) هل النوع قابل للنسخ / (EN) Is type Copy
     */
    void declareVariable(const std::string& name,
                        const std::string& typeName,
                        const SourceLocation& location,
                        bool isCopyType = false);
    
    /**
     * @brief (AR) إعادة تعيين متغير (يلغي حالة النقل)
     * @brief (EN) Reassign variable (cancels moved state)
     * 
     * (AR) عندما يُعاد تعيين متغير منقول، يعود صالحاً للاستخدام:
     *      `رسالة = "قيمة جديدة"` يُحيي المتغير
     * 
     * (EN) When a moved variable is reassigned, it becomes valid again:
     *      `message = "new value"` revives the variable
     */
    void reassignVariable(const std::string& name,
                         const SourceLocation& location);
    
    // ======================================================================
    //                    تسجيل النقل / Move Recording
    // ======================================================================
    
    /**
     * @brief (AR) تسجيل عملية نقل
     * @brief (EN) Record a move operation
     * 
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @param kind (AR) نوع النقل / (EN) Move kind
     * @param location (AR) موقع النقل / (EN) Move location
     * @param destination (AR) الوجهة / (EN) Destination
     * @return (AR) نتيجة فحص النقل / (EN) Move check result
     */
    MoveCheckResult recordMove(const std::string& name,
                               MoveKind kind,
                               const SourceLocation& location,
                               const std::string& destination = "");
    
    /**
     * @brief (AR) تسجيل نقل جزئي (نقل حقل من هيكل)
     * @brief (EN) Record partial move (move field from struct)
     * 
     * @code{.ص}
     * هيكل شخص {
     *     الاسم: نصي,
     *     العمر: عدد
     * }
     * متغير ش = شخص { الاسم: "أحمد", العمر: 25 }
     * متغير اسم = ش.الاسم    // نقل جزئي - فقط الاسم نُقل
     * اطبع(ش.العمر)          // يعمل! العمر لم يُنقل
     * // اطبع(ش.الاسم)       // خطأ! الاسم نُقل
     * @endcode
     */
    MoveCheckResult recordPartialMove(const std::string& structName,
                                      const std::string& fieldPath,
                                      const SourceLocation& location,
                                      const std::string& destination);
    
    // ======================================================================
    //                    الاستعلام / Querying
    // ======================================================================
    
    /**
     * @brief (AR) هل المتغير منقول؟
     * @brief (EN) Is variable moved?
     */
    bool isVariableMoved(const std::string& name) const;
    
    /**
     * @brief (AR) هل المتغير منقول جزئياً؟
     * @brief (EN) Is variable partially moved?
     */
    bool isPartiallyMoved(const std::string& name) const;
    
    /**
     * @brief (AR) الحصول على معلومات النقل
     * @brief (EN) Get move information
     */
    std::optional<MoveInfo> getMoveInfo(const std::string& name) const;
    
    /**
     * @brief (AR) الحصول على الحقول المنقولة (للنقل الجزئي)
     * @brief (EN) Get moved fields (for partial moves)
     */
    std::vector<std::string> getMovedFields(const std::string& structName) const;
    
    /**
     * @brief (AR) التحقق من إمكانية الاستخدام
     * @brief (EN) Check if variable can be used
     */
    std::optional<MoveError> checkUse(const std::string& name,
                                      const SourceLocation& location);
    
    // ======================================================================
    //                    إدارة النطاقات / Scope Management
    // ======================================================================
    
    /**
     * @brief (AR) الدخول في نطاق جديد
     * @brief (EN) Enter new scope
     */
    void enterScope();
    
    /**
     * @brief (AR) الخروج من النطاق الحالي
     * @brief (EN) Exit current scope
     */
    void exitScope();
    
    /**
     * @brief (AR) الدخول في نطاق حلقة
     * @brief (EN) Enter loop scope
     * 
     * (AR) النقل داخل الحلقات يحتاج معالجة خاصة:
     *      - لا يمكن نقل متغير من خارج الحلقة داخلها
     *      - إلا إذا كانت الحلقة تنفذ مرة واحدة مضمونة
     * 
     * (EN) Moves inside loops need special handling:
     *      - Can't move variable from outside loop inside it
     *      - Unless loop is guaranteed to execute once
     */
    void enterLoopScope();
    
    /**
     * @brief (AR) الخروج من نطاق الحلقة
     * @brief (EN) Exit loop scope
     */
    void exitLoopScope();
    
    // ======================================================================
    //                    تحليل الإغلاقات / Closure Analysis
    // ======================================================================
    
    /**
     * @brief (AR) بدء تحليل إغلاق
     * @brief (EN) Begin closure analysis
     * 
     * (AR) عند تعريف إغلاق، نحتاج تتبع:
     *      - المتغيرات الملتقطة
     *      - نوع الالتقاط (بالمرجع أو بالنقل)
     *      - تعارضات مع النقل الخارجي
     * 
     * @param capturedVariables (AR) المتغيرات الملتقطة / (EN) Captured variables
     * @param moveCapture (AR) هل الالتقاط بالنقل / (EN) Is capture by move
     */
    void beginClosureAnalysis(const std::vector<std::string>& capturedVariables,
                              bool moveCapture = false);
    
    /**
     * @brief (AR) إنهاء تحليل الإغلاق
     * @brief (EN) End closure analysis
     */
    void endClosureAnalysis();
    
    // ======================================================================
    //                    الأخطاء / Errors
    // ======================================================================
    
    /**
     * @brief (AR) الحصول على جميع الأخطاء
     * @brief (EN) Get all errors
     */
    const std::vector<MoveError>& getErrors() const { return errors_; }
    
    /**
     * @brief (AR) مسح الأخطاء
     * @brief (EN) Clear errors
     */
    void clearErrors() { errors_.clear(); }
    
    /**
     * @brief (AR) هل يوجد أخطاء؟
     * @brief (EN) Has errors?
     */
    bool hasErrors() const { return !errors_.empty(); }
    
    // ======================================================================
    //                    إعدادات / Settings
    // ======================================================================
    
    /**
     * @brief (AR) تعيين وضع الرسائل العربية
     * @brief (EN) Set Arabic messages mode
     */
    void setArabicMessages(bool arabic) { useArabicMessages_ = arabic; }
    
    /**
     * @brief (AR) تعيين وضع التنقيح
     * @brief (EN) Set debug mode
     */
    void setDebugMode(bool debug) { debugMode_ = debug; }
    
    // ======================================================================
    //                    تنقيح / Debugging
    // ======================================================================
    
    /**
     * @brief (AR) طباعة حالة المحلل
     * @brief (EN) Dump analyzer state
     */
    void dump() const;
    
private:
    // ======================================================================
    //                    بيانات خاصة / Private Data
    // ======================================================================
    
    /// (AR) معلومات المتغيرات / (EN) Variable information
    struct VariableInfo {
        std::string name;
        std::string typeName;
        SourceLocation definitionLocation;
        bool isCopyType;
        bool isMoved;
        bool isPartiallyMoved;
        std::optional<MoveInfo> moveInfo;
        std::unordered_set<std::string> movedFields;
    };
    
    /// (AR) خريطة المتغيرات / (EN) Variable map
    std::unordered_map<std::string, VariableInfo> variables_;
    
    /// (AR) مكدس النطاقات / (EN) Scope stack
    std::vector<std::vector<std::string>> scopeStack_;
    
    /// (AR) عمق الحلقات / (EN) Loop depth
    int loopDepth_ = 0;
    
    /// (AR) هل نحن في إغلاق؟ / (EN) Are we in a closure?
    bool inClosure_ = false;
    
    /// (AR) المتغيرات الملتقطة في الإغلاق / (EN) Captured variables in closure
    std::unordered_set<std::string> capturedVariables_;
    
    /// (AR) قائمة الأخطاء / (EN) Error list
    std::vector<MoveError> errors_;
    
    /// (AR) استخدام الرسائل العربية / (EN) Use Arabic messages
    bool useArabicMessages_ = true;
    
    /// (AR) وضع التنقيح / (EN) Debug mode
    bool debugMode_ = false;
    
    // ======================================================================
    //                    دوال مساعدة / Helper Functions
    // ======================================================================
    
    void recordError(MoveError::Kind kind,
                    const std::string& varName,
                    const SourceLocation& useLocation,
                    const SourceLocation& moveLocation);
    
    std::string getArabicErrorMessage(MoveError::Kind kind,
                                      const std::string& varName) const;
    
    std::string getEnglishErrorMessage(MoveError::Kind kind,
                                       const std::string& varName) const;
    
    std::string getSuggestion(MoveError::Kind kind,
                             const std::string& varName) const;
};

// ============================================================================
//                    تنفيذ الدوال المضمنة / Inline Implementations
// ============================================================================

inline std::string MoveInfo::getArabicDescription() const {
    std::string desc = "نُقل '" + variableName + "'";
    switch (kind) {
        case MoveKind::Assignment:
            desc += " عبر الإسناد إلى '" + destination + "'";
            break;
        case MoveKind::FunctionParameter:
            desc += " كمعامل للدالة '" + destination + "'";
            break;
        case MoveKind::Return:
            desc += " عبر جملة الإرجاع";
            break;
        case MoveKind::FieldAccess:
            desc += " عبر الوصول للحقل '" + fieldPath + "'";
            break;
        case MoveKind::PatternBinding:
            desc += " عبر ربط النمط";
            break;
        case MoveKind::ClosureCapture:
            desc += " عبر الالتقاط في إغلاق";
            break;
    }
    return desc;
}

inline std::string MoveInfo::getEnglishDescription() const {
    std::string desc = "'" + variableName + "' was moved";
    switch (kind) {
        case MoveKind::Assignment:
            desc += " via assignment to '" + destination + "'";
            break;
        case MoveKind::FunctionParameter:
            desc += " as parameter to function '" + destination + "'";
            break;
        case MoveKind::Return:
            desc += " via return statement";
            break;
        case MoveKind::FieldAccess:
            desc += " via field access '" + fieldPath + "'";
            break;
        case MoveKind::PatternBinding:
            desc += " via pattern binding";
            break;
        case MoveKind::ClosureCapture:
            desc += " via closure capture";
            break;
    }
    return desc;
}

inline std::string MoveError::toArabicString() const {
    std::string result = "خطأ[نقل]: " + arabicMessage + "\n";
    result += "   --> " + useLocation.toString() + "\n";
    result += "   |   نُقل هنا: " + moveLocation.toString() + "\n";
    if (!suggestion.empty()) {
        result += "   = اقتراح: " + suggestion + "\n";
    }
    return result;
}

inline std::string MoveError::toEnglishString() const {
    std::string result = "error[move]: " + message + "\n";
    result += "   --> " + useLocation.toString() + "\n";
    result += "   |   moved here: " + moveLocation.toString() + "\n";
    if (!suggestion.empty()) {
        result += "   = suggestion: " + suggestion + "\n";
    }
    return result;
}

} // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SAD_MOVE_ANALYZER_H
