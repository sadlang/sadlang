// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4819)
#endif

/**
 * @file lifetime_analyzer.h
 * @brief محلل الأعمار / Lifetime Analyzer
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety
 * 
 * @details
 * (AR) يحلل أعمار المراجع ويتحقق من صحتها:
 *      - NLL (Non-Lexical Lifetimes) للتحليل الدقيق
 *      - Lifetime elision للحالات الشائعة
 *      - التحقق من عدم وجود مراجع معلقة
 * 
 * (EN) Analyzes reference lifetimes and validates them:
 *      - NLL (Non-Lexical Lifetimes) for precise analysis
 *      - Lifetime elision for common cases
 *      - Validates no dangling references
 */

#ifndef SAD_LIFETIME_ANALYZER_H
#define SAD_LIFETIME_ANALYZER_H

#include "ownership/ownership_tracker.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include <set>
#include <sstream>

namespace Sad {
namespace Semantic {

// ============================================================================
// تمثيل العمر / Lifetime Representation
// ============================================================================

/**
 * @struct LifetimeId
 * @brief (AR) معرف العمر الفريد
 * @brief (EN) Unique lifetime identifier
 */
struct LifetimeId {
    size_t id;
    std::string name;   ///< (AR) اسم العمر (مثل 'أ, 'ب) / (EN) Lifetime name (like 'a, 'b)
    
    LifetimeId() : id(0), name("") {}
    LifetimeId(size_t i, const std::string& n) : id(i), name(n) {}
    
    bool operator==(const LifetimeId& other) const { return id == other.id; }
    bool operator<(const LifetimeId& other) const { return id < other.id; }
    
    std::string toString() const {
        return name.empty() ? ("'" + std::to_string(id)) : ("'" + name);
    }
};

/**
 * @enum LifetimeRelation
 * @brief (AR) العلاقة بين عمرين
 * @brief (EN) Relationship between two lifetimes
 */
enum class LifetimeRelation {
    Equal,          ///< (AR) متساويان / (EN) Equal
    Outlives,       ///< (AR) أ يعيش أطول من ب / (EN) A outlives B
    Shorter,        ///< (AR) أ أقصر من ب / (EN) A is shorter than B
    Unrelated       ///< (AR) لا علاقة / (EN) No relationship
};

/**
 * @struct LifetimeBound
 * @brief (AR) قيد على العمر
 * @brief (EN) Lifetime bound/constraint
 */
struct LifetimeBound {
    LifetimeId lifetime;
    LifetimeId bound;           ///< (AR) العمر المُقيِّد / (EN) Bounding lifetime
    bool mustOutlive;           ///< (AR) يجب أن يعيش أطول / (EN) Must outlive
    SourceLocation location;    ///< (AR) موقع القيد / (EN) Constraint location
    
    LifetimeBound() : mustOutlive(true) {}
    LifetimeBound(const LifetimeId& lt, const LifetimeId& b, bool outlive, const SourceLocation& loc)
        : lifetime(lt), bound(b), mustOutlive(outlive), location(loc) {}
};

/**
 * @struct LifetimeScope
 * @brief (AR) نطاق العمر
 * @brief (EN) Lifetime scope
 */
struct LifetimeScope {
    size_t scopeId;
    size_t startLine;
    size_t endLine;
    std::vector<std::string> variables;  ///< (AR) المتغيرات في هذا النطاق / (EN) Variables in scope
    
    LifetimeScope() : scopeId(0), startLine(0), endLine(0) {}
    LifetimeScope(size_t id, size_t start) : scopeId(id), startLine(start), endLine(0) {}
};

// ============================================================================
// أخطاء الأعمار / Lifetime Errors
// ============================================================================

/**
 * @enum LifetimeErrorKind
 * @brief (AR) أنواع أخطاء الأعمار
 * @brief (EN) Lifetime error types
 */
enum class LifetimeErrorKind {
    DanglingReference,      ///< (AR) مرجع معلق / (EN) Dangling reference
    LifetimeTooShort,       ///< (AR) العمر قصير جداً / (EN) Lifetime too short
    ReturnLocalReference,   ///< (AR) إرجاع مرجع لمتغير محلي / (EN) Return reference to local
    StoreLongLived,         ///< (AR) تخزين مرجع يعيش أطول / (EN) Store longer-lived reference
    BoundViolation,         ///< (AR) انتهاك قيد العمر / (EN) Bound violation
    AmbiguousLifetime       ///< (AR) عمر غامض / (EN) Ambiguous lifetime
};

/**
 * @struct LifetimeError
 * @brief (AR) خطأ عمر مع تفاصيل
 * @brief (EN) Lifetime error with details
 */
struct LifetimeError {
    LifetimeErrorKind kind;
    std::string message;
    std::string arabicMessage;
    SourceLocation location;
    SourceLocation relatedLocation;
    std::string suggestion;
    
    std::string toArabicString() const;
    std::string toEnglishString() const;
};

// ============================================================================
// محلل الأعمار / Lifetime Analyzer
// ============================================================================

/**
 * @class LifetimeAnalyzer
 * @brief (AR) محلل الأعمار مع دعم NLL
 * @brief (EN) Lifetime analyzer with NLL support
 * 
 * @details
 * (AR) يقوم بـ:
 *      - تتبع أعمار جميع المراجع
 *      - التحقق من قيود الأعمار
 *      - كشف المراجع المعلقة
 *      - دعم lifetime elision
 * 
 * (EN) Does:
 *      - Track lifetimes of all references
 *      - Verify lifetime constraints
 *      - Detect dangling references
 *      - Support lifetime elision
 */
class LifetimeAnalyzer {
public:
    // ==================================================================
    // بناء / Construction
    // ==================================================================
    
    LifetimeAnalyzer();
    ~LifetimeAnalyzer();
    
    // ==================================================================
    // إدارة الأعمار / Lifetime Management
    // ==================================================================
    
    /**
     * @brief (AR) إنشاء عمر جديد
     * @brief (EN) Create new lifetime
     */
    LifetimeId createLifetime(const std::string& name = "");
    
    /**
     * @brief (AR) إنشاء عمر ساكن ('static)
     * @brief (EN) Create static lifetime ('static)
     */
    LifetimeId staticLifetime() const { return staticLifetime_; }
    
    /**
     * @brief (AR) ربط عمر بمتغير
     * @brief (EN) Bind lifetime to variable
     */
    void bindLifetimeToVariable(const std::string& varName, const LifetimeId& lifetime);
    
    /**
     * @brief (AR) الحصول على عمر متغير
     * @brief (EN) Get variable's lifetime
     */
    std::optional<LifetimeId> getVariableLifetime(const std::string& varName) const;
    
    // ==================================================================
    // إدارة النطاقات / Scope Management
    // ==================================================================
    
    /**
     * @brief (AR) الدخول إلى نطاق جديد
     * @brief (EN) Enter new scope
     */
    size_t enterScope(size_t startLine);
    
    /**
     * @brief (AR) الخروج من النطاق الحالي
     * @brief (EN) Exit current scope
     */
    void exitScope(size_t endLine);
    
    /**
     * @brief (AR) الحصول على النطاق الحالي
     * @brief (EN) Get current scope
     */
    const LifetimeScope& currentScope() const;
    
    // ==================================================================
    // قيود الأعمار / Lifetime Constraints
    // ==================================================================
    
    /**
     * @brief (AR) إضافة قيد: أ يعيش أطول من ب
     * @brief (EN) Add constraint: a outlives b
     */
    void addOutlivesConstraint(const LifetimeId& longer, 
                               const LifetimeId& shorter,
                               const SourceLocation& loc);
    
    /**
     * @brief (AR) التحقق من صحة جميع القيود
     * @brief (EN) Verify all constraints are satisfied
     */
    std::vector<LifetimeError> verifyConstraints();
    
    // ==================================================================
    // التحليل / Analysis
    // ==================================================================
    
    /**
     * @brief (AR) التحقق من أن العمر يعيش أطول من نطاق
     * @brief (EN) Check if lifetime outlives a scope
     */
    bool outlivesScope(const LifetimeId& lifetime, size_t scopeId) const;
    
    /**
     * @brief (AR) مقارنة عمرين
     * @brief (EN) Compare two lifetimes
     */
    LifetimeRelation compare(const LifetimeId& a, const LifetimeId& b) const;
    
    /**
     * @brief (AR) هل العمر ساكن؟
     * @brief (EN) Is lifetime static?
     */
    bool isStatic(const LifetimeId& lifetime) const;
    
    // ==================================================================
    // Lifetime Elision / حذف العمر
    // ==================================================================
    
    /**
     * @brief (AR) استنتاج أعمار معاملات الدالة
     * @brief (EN) Infer function parameter lifetimes
     * 
     * @details
     * (AR) قواعد الحذف:
     *      1. كل معامل مرجع يحصل على عمر مستقل
     *      2. إذا كان هناك معامل واحد، يُعطى للمخرجات
     *      3. إذا كان هناك &self، يُعطى عمره للمخرجات
     * 
     * (EN) Elision rules:
     *      1. Each reference parameter gets distinct lifetime
     *      2. If one input, outputs get its lifetime
     *      3. If &self exists, outputs get its lifetime
     */
    std::vector<LifetimeId> inferFunctionLifetimes(
        const std::vector<std::string>& paramTypes,
        const std::string& returnType,
        bool hasSelf);
    
    // ==================================================================
    // NLL (Non-Lexical Lifetimes) / الأعمار غير المعجمية
    // ==================================================================
    
    /**
     * @brief (AR) تحليل نقاط الاستخدام الأخير
     * @brief (EN) Analyze last use points
     */
    void analyzeLastUsePoints();
    
    /**
     * @brief (AR) تمكين/تعطيل NLL
     * @brief (EN) Enable/disable NLL
     */
    void setNLLEnabled(bool enabled) { nllEnabled_ = enabled; }
    
    /**
     * @brief (AR) هل NLL مُفعَّل؟
     * @brief (EN) Is NLL enabled?
     */
    bool isNLLEnabled() const { return nllEnabled_; }
    
    // ==================================================================
    // تنقيح / Debugging
    // ==================================================================
    
    /**
     * @brief (AR) طباعة معلومات الأعمار
     * @brief (EN) Print lifetime information
     */
    void dump() const;
    
    /**
     * @brief (AR) الحصول على جميع الأخطاء
     * @brief (EN) Get all errors
     */
    const std::vector<LifetimeError>& getErrors() const { return errors_; }
    
    /**
     * @brief (AR) مسح الأخطاء
     * @brief (EN) Clear errors
     */
    void clearErrors() { errors_.clear(); }
    
private:
    // ==================================================================
    // بيانات خاصة / Private Data
    // ==================================================================
    
    /// (AR) عداد معرفات الأعمار / (EN) Lifetime ID counter
    size_t nextLifetimeId_;
    
    /// (AR) العمر الساكن / (EN) Static lifetime
    LifetimeId staticLifetime_;
    
    /// (AR) خريطة المتغيرات إلى الأعمار / (EN) Variable to lifetime map
    std::unordered_map<std::string, LifetimeId> variableLifetimes_;
    
    /// (AR) مكدس النطاقات / (EN) Scope stack
    std::vector<LifetimeScope> scopeStack_;
    
    /// (AR) قيود الأعمار / (EN) Lifetime constraints
    std::vector<LifetimeBound> constraints_;
    
    /// (AR) قائمة الأخطاء / (EN) Errors list
    std::vector<LifetimeError> errors_;
    
    /// (AR) تفعيل NLL / (EN) NLL enabled
    bool nllEnabled_;
    
    /// (AR) عداد النطاقات / (EN) Scope counter
    size_t nextScopeId_;
    
    // ==================================================================
    // دوال مساعدة / Helper Functions
    // ==================================================================
    
    LifetimeError makeError(LifetimeErrorKind kind,
                           const SourceLocation& loc,
                           const SourceLocation& relatedLoc = SourceLocation());
    
    bool isReferenceType(const std::string& type) const;
    
    std::string getArabicMessage(LifetimeErrorKind kind) const;
    std::string getEnglishMessage(LifetimeErrorKind kind) const;
    std::string getSuggestion(LifetimeErrorKind kind) const;
};

// ============================================================================
// تنفيذ LifetimeError strings
// ============================================================================

inline std::string LifetimeError::toArabicString() const {
    std::ostringstream oss;
    oss << "خطأ[عمر]: " << arabicMessage << "\n";
    oss << "   --> " << location.toString() << "\n";
    if (!relatedLocation.file.empty()) {
        oss << "   |   موقع ذو صلة: " << relatedLocation.toString() << "\n";
    }
    if (!suggestion.empty()) {
        oss << "   = اقتراح: " << suggestion << "\n";
    }
    return oss.str();
}

inline std::string LifetimeError::toEnglishString() const {
    std::ostringstream oss;
    oss << "error[lifetime]: " << message << "\n";
    oss << "   --> " << location.toString() << "\n";
    if (!relatedLocation.file.empty()) {
        oss << "   |   related: " << relatedLocation.toString() << "\n";
    }
    if (!suggestion.empty()) {
        oss << "   = suggestion: " << suggestion << "\n";
    }
    return oss.str();
}

} // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SAD_LIFETIME_ANALYZER_H
