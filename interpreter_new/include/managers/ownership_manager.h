// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4819)
#endif

/**
 * @file ownership_manager.h
 * @brief (AR) مدير الملكية - تكامل نظام الملكية مع المفسر
 * @brief (EN) Ownership Manager - Ownership system integration with interpreter
 * 
 * @details
 * (AR) يُغلّف نظام تتبع الملكية (OwnershipTracker) ليعمل مع المفسر:
 *      - يتتبع ملكية المتغيرات أثناء التنفيذ
 *      - يكشف أخطاء الملكية في وقت التشغيل
 *      - يدعم الاستعارة المشتركة والمتغيرة
 *      - يدعم نقل الملكية (Move Semantics)
 *      - رسائل خطأ ثنائية اللغة (عربي/إنجليزي)
 * 
 * (EN) Wraps OwnershipTracker to work with the interpreter:
 *      - Tracks variable ownership during execution
 *      - Detects ownership errors at runtime
 *      - Supports shared and mutable borrowing
 *      - Supports move semantics
 *      - Bilingual error messages (Arabic/English)
 * 
 * @author Sad Language Team
 * @date February 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety Integration
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <optional>
#include <functional>
#include <sstream>
#include <iostream>
#include <iomanip>

namespace Sad {
namespace Data {

// ============================================================================
// تعدادات نظام الملكية / Ownership System Enumerations
// ============================================================================

/**
 * @enum OwnershipState
 * @brief (AR) حالة ملكية المتغير / (EN) Variable ownership state
 */
enum class OwnershipState {
    Owned,          ///< (AR) مملوك / (EN) Variable owns the value
    Moved,          ///< (AR) منقول / (EN) Ownership transferred
    Borrowed,       ///< (AR) مستعار ثابت / (EN) Immutably borrowed
    BorrowedMut,    ///< (AR) مستعار متغير / (EN) Mutably borrowed
    Dropped,        ///< (AR) محذوف / (EN) Dropped
    Uninitialized   ///< (AR) غير مُهيّأ / (EN) Uninitialized
};

/**
 * @enum BorrowKind
 * @brief (AR) نوع الاستعارة / (EN) Borrow kind
 */
enum class BorrowKind {
    Shared,     ///< (AR) استعارة مشتركة (&) / (EN) Shared borrow (&)
    Mutable     ///< (AR) استعارة متغيرة (&متغير) / (EN) Mutable borrow (&mut)
};

/**
 * @enum OwnershipErrorKind
 * @brief (AR) أنواع أخطاء الملكية / (EN) Ownership error types
 */
enum class OwnershipErrorKind {
    UseAfterMove,           ///< (AR) استخدام بعد النقل
    DoubleMove,             ///< (AR) نقل مزدوج
    BorrowOfMoved,          ///< (AR) استعارة من منقول
    MutBorrowConflict,      ///< (AR) تعارض استعارة متغيرة
    BorrowWhileMutBorrow,   ///< (AR) استعارة أثناء استعارة متغيرة
    MutBorrowWhileBorrow,   ///< (AR) استعارة متغيرة أثناء استعارة
    MutateWhileBorrowed,    ///< (AR) تعديل أثناء الاستعارة
    UseOfUninitialized,     ///< (AR) استخدام غير مهيأ
    DropWhileBorrowed,      ///< (AR) حذف أثناء الاستعارة
    InvalidLifetime         ///< (AR) عمر غير صالح
};

// ============================================================================
// هياكل البيانات / Data Structures
// ============================================================================

/**
 * @struct BorrowInfo
 * @brief (AR) معلومات الاستعارة / (EN) Borrow information
 */
struct BorrowInfo {
    std::string borrowerName;
    BorrowKind kind;
    size_t scopeId;
    
    BorrowInfo() : kind(BorrowKind::Shared), scopeId(0) {}
    BorrowInfo(const std::string& name, BorrowKind k, size_t scope)
        : borrowerName(name), kind(k), scopeId(scope) {}
};

/**
 * @struct VariableOwnership
 * @brief (AR) معلومات الملكية الكاملة للمتغير / (EN) Complete ownership info
 */
struct VariableOwnership {
    std::string variableName;
    OwnershipState state;
    std::vector<BorrowInfo> borrows;
    size_t scopeId;
    bool isCopyType;
    std::string typeName;
    
    VariableOwnership()
        : state(OwnershipState::Uninitialized), scopeId(0), isCopyType(false) {}
    
    VariableOwnership(const std::string& name, size_t scope, bool copyType = false)
        : variableName(name), state(OwnershipState::Owned)
        , scopeId(scope), isCopyType(copyType) {}
    
    bool hasActiveBorrows() const { return !borrows.empty(); }
    bool hasActiveMutableBorrow() const {
        for (const auto& b : borrows)
            if (b.kind == BorrowKind::Mutable) return true;
        return false;
    }
};

/**
 * @struct OwnershipError
 * @brief (AR) خطأ ملكية / (EN) Ownership error
 */
struct OwnershipError {
    OwnershipErrorKind kind;
    std::string variableName;
    std::string message;
    std::string arabicMessage;
    std::string suggestion;
    
    std::string toArabicString() const {
        std::string code = std::to_string(static_cast<int>(kind));
        while (code.size() < 4) code = "0" + code;
        std::string result = "\xD8\xAE\xD8\xB7\xD8\xA3[\xD8\xB5" + code + "]: " + arabicMessage;
        if (!suggestion.empty())
            result += "\n   = \xD8\xA7\xD9\x82\xD8\xAA\xD8\xB1\xD8\xA7\xD8\xAD: " + suggestion;
        return result;
    }
    
    std::string toEnglishString() const {
        std::string code = std::to_string(static_cast<int>(kind));
        while (code.size() < 4) code = "0" + code;
        std::string result = "error[S" + code + "]: " + message;
        if (!suggestion.empty())
            result += "\n   = suggestion: " + suggestion;
        return result;
    }
};

// ============================================================================
// مدير الملكية / Ownership Manager
// ============================================================================

/**
 * @class OwnershipManager
 * @brief (AR) مدير الملكية للمفسر / (EN) Ownership manager for interpreter
 * 
 * @details
 * (AR) يُدير نظام الملكية أثناء تنفيذ البرنامج:
 *      - يتتبع ملكية كل متغير
 *      - يتحقق من قواعد الاستعارة
 *      - يكشف استخدام المتغيرات المنقولة
 *      - يمكن تفعيله/تعطيله
 * 
 * (EN) Manages ownership system during program execution:
 *      - Tracks ownership of every variable
 *      - Validates borrowing rules
 *      - Detects use of moved variables
 *      - Can be enabled/disabled
 */
class OwnershipManager {
public:
    OwnershipManager();
    ~OwnershipManager();
    
    // ==================================================================
    // تفعيل/تعطيل / Enable/Disable
    // ==================================================================
    
    /** @brief (AR) تفعيل نظام الملكية / (EN) Enable ownership system */
    void enable() { enabled_ = true; }
    
    /** @brief (AR) تعطيل نظام الملكية / (EN) Disable ownership system */
    void disable() { enabled_ = false; }
    
    /** @brief (AR) هل النظام مفعّل؟ / (EN) Is system enabled? */
    bool isEnabled() const { return enabled_; }
    
    /** @brief (AR) تعيين الرسائل العربية / (EN) Set Arabic messages */
    void setArabicMessages(bool arabic) { useArabicMessages_ = arabic; }
    
    /** @brief (AR) تعيين وضع التنقيح / (EN) Set debug mode */
    void setDebugMode(bool debug) { debugMode_ = debug; }
    
    // ==================================================================
    // إدارة النطاقات / Scope Management
    // ==================================================================
    
    /**
     * @brief (AR) دخول نطاق جديد / (EN) Enter new scope
     * @return (AR) معرف النطاق / (EN) Scope ID
     */
    size_t enterScope();
    
    /**
     * @brief (AR) خروج من النطاق / (EN) Exit scope
     * @details (AR) يحذف المتغيرات المحلية ويتحقق من الاستعارات المعلقة
     */
    void exitScope();
    
    // ==================================================================
    // تصريح واستخدام المتغيرات / Variable Declaration & Use
    // ==================================================================
    
    /**
     * @brief (AR) تصريح متغير جديد / (EN) Declare new variable
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @param typeName (AR) اسم النوع / (EN) Type name
     * @return (AR) نجاح أم لا / (EN) Success or not
     */
    bool declareVariable(const std::string& name, const std::string& typeName = "");
    
    /**
     * @brief (AR) استخدام متغير (قراءة) / (EN) Use variable (read)
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) خطأ إن وُجد / (EN) Error if any
     */
    std::optional<OwnershipError> useVariable(const std::string& name);
    
    /**
     * @brief (AR) نقل ملكية متغير / (EN) Move variable ownership
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) خطأ إن وُجد / (EN) Error if any
     */
    std::optional<OwnershipError> moveVariable(const std::string& name);
    
    /**
     * @brief (AR) تعديل متغير / (EN) Mutate variable
     * @param name (AR) اسم المتغير / (EN) Variable name
     * @return (AR) خطأ إن وُجد / (EN) Error if any
     */
    std::optional<OwnershipError> mutateVariable(const std::string& name);
    
    // ==================================================================
    // الاستعارة / Borrowing
    // ==================================================================
    
    /**
     * @brief (AR) إنشاء استعارة / (EN) Create borrow
     */
    std::optional<OwnershipError> createBorrow(
        const std::string& ownerName,
        const std::string& borrowerName,
        BorrowKind kind);
    
    /**
     * @brief (AR) إنهاء استعارة / (EN) End borrow
     */
    void endBorrow(const std::string& ownerName, const std::string& borrowerName);
    
    // ==================================================================
    // استعلامات / Queries
    // ==================================================================
    
    /** @brief (AR) هل المتغير موجود؟ / (EN) Does variable exist? */
    bool variableExists(const std::string& name) const;
    
    /** @brief (AR) هل يمكن نقل المتغير؟ / (EN) Can variable be moved? */
    bool canMove(const std::string& name) const;
    
    /** @brief (AR) الحصول على حالة الملكية / (EN) Get ownership state */
    std::optional<OwnershipState> getState(const std::string& name) const;
    
    /** @brief (AR) الحصول على الأخطاء / (EN) Get errors */
    const std::vector<OwnershipError>& getErrors() const { return errors_; }
    
    /** @brief (AR) مسح الأخطاء / (EN) Clear errors */
    void clearErrors() { errors_.clear(); }
    
    /** @brief (AR) هل هناك أخطاء؟ / (EN) Are there errors? */
    bool hasErrors() const { return !errors_.empty(); }
    
    /** @brief (AR) إعادة تعيين / (EN) Reset */
    void reset();
    
    /** @brief (AR) طباعة حالة التنقيح / (EN) Print debug state */
    void dump() const;
    
    /** @brief (AR) الحصول على ملخص / (EN) Get summary */
    std::string getSummary() const;
    
private:
    bool enabled_;
    bool useArabicMessages_;
    bool debugMode_;
    
    // (AR) خريطة المتغيرات / (EN) Variables map
    std::unordered_map<std::string, VariableOwnership> variables_;
    
    // (AR) مكدس النطاقات / (EN) Scope stack
    std::vector<size_t> scopeStack_;
    size_t nextScopeId_;
    size_t currentScope_;
    
    // (AR) قائمة الأخطاء / (EN) Error list
    std::vector<OwnershipError> errors_;
    
    // (AR) الأنواع القابلة للنسخ / (EN) Copy types
    std::unordered_set<std::string> copyTypes_;
    
    // (AR) إحصائيات / (EN) Statistics
    size_t totalVariables_;
    size_t totalBorrows_;
    size_t totalMoves_;
    
    // ==================================================================
    // دوال مساعدة / Helper functions
    // ==================================================================
    
    bool isCopyType(const std::string& typeName) const;
    void initializeCopyTypes();
    void dropVariablesInScope(size_t scopeId);
    void endBorrowsInScope(size_t scopeId);
    
    OwnershipError makeError(OwnershipErrorKind kind, const std::string& varName);
    void reportError(const OwnershipError& error);
    
    std::string getArabicMessage(OwnershipErrorKind kind, const std::string& varName) const;
    std::string getEnglishMessage(OwnershipErrorKind kind, const std::string& varName) const;
    std::string getSuggestion(OwnershipErrorKind kind) const;
};

} // namespace Data
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif
