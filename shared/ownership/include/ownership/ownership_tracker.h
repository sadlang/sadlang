// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4819)
#endif

/**
 * @file ownership_tracker.h
 * @brief نظام تتبع الملكية / Ownership Tracking System
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety
 * 
 * @details
 * (AR) يُنفِّذ نظام تتبع الملكية على طريقة Rust:
 *      - كل قيمة لها مالك واحد فقط
 *      - نقل الملكية (Move) يُبطل المتغير الأصلي
 *      - الاستعارة المتغيرة (&متغير) حصرية
 *      - الاستعارة الثابتة (&) يمكن أن تكون متعددة
 * 
 * (EN) Implements Rust-style ownership tracking:
 *      - Each value has exactly one owner
 *      - Move semantics invalidate original variable
 *      - Mutable borrows (&متغير) are exclusive
 *      - Immutable borrows (&) can be multiple
 * 
 * @see borrow_checker.h
 * @see lifetime_analyzer.h
 */

#ifndef SAD_OWNERSHIP_TRACKER_H
#define SAD_OWNERSHIP_TRACKER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include <variant>

namespace Sad {
namespace Semantic {

// ============================================================================
// تعدادات حالة الملكية / Ownership State Enumerations
// ============================================================================

/**
 * @enum OwnershipState
 * @brief (AR) حالة ملكية المتغير
 * @brief (EN) Variable ownership state
 */
enum class OwnershipState {
    Owned,          ///< (AR) مملوك - المتغير يملك القيمة / (EN) Owned - variable owns the value
    Moved,          ///< (AR) منقول - الملكية انتقلت / (EN) Moved - ownership transferred
    Borrowed,       ///< (AR) مستعار (ثابت) / (EN) Borrowed (immutable)
    BorrowedMut,    ///< (AR) مستعار (متغير) / (EN) Borrowed (mutable)
    Dropped,        ///< (AR) محذوف - تم تحرير الذاكرة / (EN) Dropped - memory freed
    Uninitialized   ///< (AR) غير مهيأ / (EN) Uninitialized
};

/**
 * @enum BorrowKind
 * @brief (AR) نوع الاستعارة
 * @brief (EN) Borrow kind
 */
enum class BorrowKind {
    Shared,     ///< (AR) استعارة مشتركة (&) / (EN) Shared borrow (&)
    Mutable     ///< (AR) استعارة متغيرة (&متغير) / (EN) Mutable borrow (&mut)
};

// ============================================================================
// هياكل البيانات / Data Structures
// ============================================================================

/**
 * @struct SourceLocation
 * @brief (AR) موقع في الكود المصدري
 * @brief (EN) Source code location
 */
struct SourceLocation {
    std::string file;       ///< (AR) اسم الملف / (EN) File name
    size_t line;            ///< (AR) رقم السطر / (EN) Line number
    size_t column;          ///< (AR) رقم العمود / (EN) Column number
    
    SourceLocation() : file(""), line(0), column(0) {}
    SourceLocation(const std::string& f, size_t l, size_t c) 
        : file(f), line(l), column(c) {}
    
    std::string toString() const {
        return file + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

/**
 * @struct BorrowInfo
 * @brief (AR) معلومات الاستعارة
 * @brief (EN) Borrow information
 */
struct BorrowInfo {
    std::string borrowerName;   ///< (AR) اسم المستعير / (EN) Borrower name
    BorrowKind kind;            ///< (AR) نوع الاستعارة / (EN) Borrow kind
    SourceLocation location;    ///< (AR) موقع الاستعارة / (EN) Borrow location
    size_t scopeId;             ///< (AR) معرف النطاق / (EN) Scope ID
    
    BorrowInfo() : borrowerName(""), kind(BorrowKind::Shared), scopeId(0) {}
    BorrowInfo(const std::string& name, BorrowKind k, const SourceLocation& loc, size_t scope)
        : borrowerName(name), kind(k), location(loc), scopeId(scope) {}
};

/**
 * @struct OwnershipInfo
 * @brief (AR) معلومات الملكية الكاملة للمتغير
 * @brief (EN) Complete ownership information for a variable
 */
struct OwnershipInfo {
    std::string variableName;           ///< (AR) اسم المتغير / (EN) Variable name
    OwnershipState state;               ///< (AR) حالة الملكية / (EN) Ownership state
    SourceLocation definedAt;           ///< (AR) موقع التعريف / (EN) Definition location
    SourceLocation movedAt;             ///< (AR) موقع النقل (إن حدث) / (EN) Move location (if moved)
    std::vector<BorrowInfo> borrows;    ///< (AR) قائمة الاستعارات النشطة / (EN) Active borrows
    size_t scopeId;                     ///< (AR) معرف النطاق المُعرَّف فيه / (EN) Defining scope ID
    bool isCopyType;                    ///< (AR) هل النوع قابل للنسخ؟ / (EN) Is type Copy?
    std::string typeName;               ///< (AR) اسم النوع / (EN) Type name
    
    OwnershipInfo() 
        : variableName("")
        , state(OwnershipState::Uninitialized)
        , scopeId(0)
        , isCopyType(false)
        , typeName("") {}
    
    OwnershipInfo(const std::string& name, const SourceLocation& loc, size_t scope)
        : variableName(name)
        , state(OwnershipState::Owned)
        , definedAt(loc)
        , scopeId(scope)
        , isCopyType(false)
        , typeName("") {}
    
    bool hasActiveBorrows() const { return !borrows.empty(); }
    bool hasActiveMutableBorrow() const {
        for (const auto& b : borrows) {
            if (b.kind == BorrowKind::Mutable) return true;
        }
        return false;
    }
    size_t sharedBorrowCount() const {
        size_t count = 0;
        for (const auto& b : borrows) {
            if (b.kind == BorrowKind::Shared) count++;
        }
        return count;
    }
};

// ============================================================================
// أخطاء الملكية / Ownership Errors
// ============================================================================

/**
 * @enum OwnershipErrorKind
 * @brief (AR) أنواع أخطاء الملكية
 * @brief (EN) Ownership error types
 */
enum class OwnershipErrorKind {
    UseAfterMove,           ///< (AR) استخدام بعد النقل / (EN) Use after move
    DoubleMove,             ///< (AR) نقل مزدوج / (EN) Double move
    BorrowOfMoved,          ///< (AR) استعارة من منقول / (EN) Borrow of moved value
    MutBorrowConflict,      ///< (AR) تعارض استعارة متغيرة / (EN) Mutable borrow conflict
    BorrowWhileMutBorrow,   ///< (AR) استعارة أثناء استعارة متغيرة / (EN) Borrow while mut borrowed
    MutBorrowWhileBorrow,   ///< (AR) استعارة متغيرة أثناء استعارة / (EN) Mut borrow while borrowed
    MutateWhileBorrowed,    ///< (AR) تعديل أثناء الاستعارة / (EN) Mutate while borrowed
    UseOfUninitialized,     ///< (AR) استخدام غير مهيأ / (EN) Use of uninitialized
    DropWhileBorrowed,      ///< (AR) حذف أثناء الاستعارة / (EN) Drop while borrowed
    InvalidLifetime         ///< (AR) عمر غير صالح / (EN) Invalid lifetime
};

/**
 * @struct OwnershipError
 * @brief (AR) خطأ ملكية مع تفاصيل كاملة
 * @brief (EN) Ownership error with full details
 */
struct OwnershipError {
    OwnershipErrorKind kind;            ///< (AR) نوع الخطأ / (EN) Error kind
    std::string variableName;           ///< (AR) اسم المتغير / (EN) Variable name
    SourceLocation errorLocation;       ///< (AR) موقع الخطأ / (EN) Error location
    SourceLocation relatedLocation;     ///< (AR) موقع ذو صلة / (EN) Related location
    std::string message;                ///< (AR) رسالة الخطأ / (EN) Error message
    std::string arabicMessage;          ///< (AR) رسالة الخطأ بالعربية / (EN) Arabic error message
    std::string suggestion;             ///< (AR) اقتراح الإصلاح / (EN) Fix suggestion
    
    std::string toArabicString() const;
    std::string toEnglishString() const;
};

// ============================================================================
// متتبع الملكية / Ownership Tracker
// ============================================================================

/**
 * @class OwnershipTracker
 * @brief (AR) متتبع الملكية الرئيسي
 * @brief (EN) Main ownership tracker
 * 
 * @details
 * (AR) يتتبع حالة ملكية كل متغير في البرنامج:
 *      - يسجل تعريف المتغيرات
 *      - يتتبع نقل الملكية
 *      - يتتبع الاستعارات النشطة
 *      - يكشف الأخطاء في وقت الترجمة
 * 
 * (EN) Tracks ownership state of every variable in the program:
 *      - Records variable definitions
 *      - Tracks ownership transfers
 *      - Tracks active borrows
 *      - Detects errors at compile time
 * 
 * @example
 * @code{.cpp}
 * OwnershipTracker tracker;
 * tracker.enterScope();
 * tracker.declareVariable("س", "نص", location);
 * auto result = tracker.useVariable("س", location);
 * if (!result.success) {
 *     // خطأ ملكية / Ownership error
 * }
 * @endcode
 */
class OwnershipTracker {
public:
    // ==================================================================
    // بناء وهدم / Construction & Destruction
    // ==================================================================
    
    OwnershipTracker();
    ~OwnershipTracker();
    
    // ==================================================================
    // إدارة النطاقات / Scope Management
    // ==================================================================
    
    /**
     * @brief (AR) الدخول إلى نطاق جديد
     * @brief (EN) Enter a new scope
     * @return (size_t) — (AR) معرف النطاق الجديد / (EN) New scope ID
     */
    size_t enterScope();
    
    /**
     * @brief (AR) الخروج من النطاق الحالي
     * @brief (EN) Exit current scope
     * @details
     * (AR) يُحرر جميع المتغيرات المحلية ويتحقق من الاستعارات المعلقة
     * (EN) Drops all local variables and checks for dangling borrows
     */
    void exitScope();
    
    /**
     * @brief (AR) الحصول على معرف النطاق الحالي
     * @brief (EN) Get current scope ID
     */
    size_t currentScopeId() const { return currentScope_; }
    
    // ==================================================================
    // تصريح واستخدام المتغيرات / Variable Declaration & Use
    // ==================================================================
    
    /**
     * @brief (AR) تصريح متغير جديد
     * @brief (EN) Declare a new variable
     * 
     * @param name (std::string) — (AR) اسم المتغير / (EN) Variable name
     * @param typeName (std::string) — (AR) اسم النوع / (EN) Type name
     * @param location (SourceLocation) — (AR) موقع التعريف / (EN) Definition location
     * @param isCopyType (bool) — (AR) هل النوع قابل للنسخ؟ / (EN) Is type Copy?
     * @return (bool) — (AR) نجاح العملية / (EN) Operation success
     */
    bool declareVariable(const std::string& name, 
                         const std::string& typeName,
                         const SourceLocation& location,
                         bool isCopyType = false);
    
    /**
     * @brief (AR) استخدام متغير (قراءة)
     * @brief (EN) Use a variable (read)
     * 
     * @param name (std::string) — (AR) اسم المتغير / (EN) Variable name
     * @param location (SourceLocation) — (AR) موقع الاستخدام / (EN) Use location
     * @return (std::optional<OwnershipError>) — (AR) خطأ إن وجد / (EN) Error if any
     */
    std::optional<OwnershipError> useVariable(const std::string& name, 
                                               const SourceLocation& location);
    
    /**
     * @brief (AR) نقل ملكية متغير
     * @brief (EN) Move ownership of a variable
     * 
     * @param name (std::string) — (AR) اسم المتغير / (EN) Variable name
     * @param location (SourceLocation) — (AR) موقع النقل / (EN) Move location
     * @return (std::optional<OwnershipError>) — (AR) خطأ إن وجد / (EN) Error if any
     * 
     * @details
     * (AR) إذا كان النوع قابلاً للنسخ (Copy)، لا يحدث نقل فعلي
     * (EN) If type is Copy, no actual move occurs
     */
    std::optional<OwnershipError> moveVariable(const std::string& name,
                                                const SourceLocation& location);
    
    /**
     * @brief (AR) تعديل متغير
     * @brief (EN) Mutate a variable
     * 
     * @param name (std::string) — (AR) اسم المتغير / (EN) Variable name
     * @param location (SourceLocation) — (AR) موقع التعديل / (EN) Mutation location
     * @return (std::optional<OwnershipError>) — (AR) خطأ إن وجد / (EN) Error if any
     * 
     * @details
     * (AR) يفشل إذا كان المتغير مستعاراً (حتى لو كانت استعارة ثابتة)
     * (EN) Fails if variable is borrowed (even immutably)
     */
    std::optional<OwnershipError> mutateVariable(const std::string& name,
                                                  const SourceLocation& location);
    
    // ==================================================================
    // الاستعارة / Borrowing
    // ==================================================================
    
    /**
     * @brief (AR) إنشاء استعارة
     * @brief (EN) Create a borrow
     * 
     * @param ownerName (std::string) — (AR) اسم المالك / (EN) Owner name
     * @param borrowerName (std::string) — (AR) اسم المستعير / (EN) Borrower name
     * @param kind (BorrowKind) — (AR) نوع الاستعارة / (EN) Borrow kind
     * @param location (SourceLocation) — (AR) موقع الاستعارة / (EN) Borrow location
     * @return (std::optional<OwnershipError>) — (AR) خطأ إن وجد / (EN) Error if any
     * 
     * @details
     * (AR) قواعد الاستعارة:
     *      - استعارة متغيرة واحدة فقط في كل وقت
     *      - أو عدة استعارات ثابتة
     *      - لا يمكن الجمع بين الاثنين
     * 
     * (EN) Borrowing rules:
     *      - Only one mutable borrow at a time
     *      - Or multiple immutable borrows
     *      - Cannot mix both
     */
    std::optional<OwnershipError> createBorrow(const std::string& ownerName,
                                                const std::string& borrowerName,
                                                BorrowKind kind,
                                                const SourceLocation& location);
    
    /**
     * @brief (AR) إنهاء استعارة
     * @brief (EN) End a borrow
     * 
     * @param ownerName (std::string) — (AR) اسم المالك / (EN) Owner name
     * @param borrowerName (std::string) — (AR) اسم المستعير / (EN) Borrower name
     */
    void endBorrow(const std::string& ownerName, const std::string& borrowerName);
    
    /**
     * @brief (AR) إنهاء جميع استعارات نطاق معين
     * @brief (EN) End all borrows for a scope
     */
    void endBorrowsInScope(size_t scopeId);
    
    // ==================================================================
    // استعلامات / Queries
    // ==================================================================
    
    /**
     * @brief (AR) الحصول على معلومات ملكية متغير
     * @brief (EN) Get ownership info for a variable
     */
    std::optional<OwnershipInfo> getOwnershipInfo(const std::string& name) const;
    
    /**
     * @brief (AR) التحقق من إمكانية نقل متغير
     * @brief (EN) Check if variable can be moved
     */
    bool canMove(const std::string& name) const;
    
    /**
     * @brief (AR) التحقق من إمكانية استعارة متغير
     * @brief (EN) Check if variable can be borrowed
     */
    bool canBorrow(const std::string& name, BorrowKind kind) const;
    
    /**
     * @brief (AR) التحقق من وجود متغير
     * @brief (EN) Check if variable exists
     */
    bool variableExists(const std::string& name) const;
    
    /**
     * @brief (AR) الحصول على جميع الأخطاء
     * @brief (EN) Get all errors
     */
    const std::vector<OwnershipError>& getErrors() const { return errors_; }
    
    /**
     * @brief (AR) مسح جميع الأخطاء
     * @brief (EN) Clear all errors
     */
    void clearErrors() { errors_.clear(); }
    
    // ==================================================================
    // تنقيح / Debugging
    // ==================================================================
    
    /**
     * @brief (AR) طباعة حالة الملكية للتنقيح
     * @brief (EN) Print ownership state for debugging
     */
    void dump() const;
    
private:
    // ==================================================================
    // بيانات خاصة / Private Data
    // ==================================================================
    
    /// (AR) خريطة المتغيرات / (EN) Variables map
    std::unordered_map<std::string, OwnershipInfo> variables_;
    
    /// (AR) مكدس النطاقات / (EN) Scope stack
    std::vector<size_t> scopeStack_;
    
    /// (AR) عداد النطاقات / (EN) Scope counter
    size_t nextScopeId_;
    
    /// (AR) النطاق الحالي / (EN) Current scope
    size_t currentScope_;
    
    /// (AR) قائمة الأخطاء / (EN) Errors list
    std::vector<OwnershipError> errors_;
    
    // ==================================================================
    // دوال مساعدة خاصة / Private Helper Functions
    // ==================================================================
    
    OwnershipError makeError(OwnershipErrorKind kind,
                             const std::string& varName,
                             const SourceLocation& errorLoc,
                             const SourceLocation& relatedLoc = SourceLocation());
    
    void dropVariablesInScope(size_t scopeId);
    
    std::string getArabicErrorMessage(OwnershipErrorKind kind, 
                                      const std::string& varName) const;
    
    std::string getEnglishErrorMessage(OwnershipErrorKind kind,
                                       const std::string& varName) const;
    
    std::string getSuggestion(OwnershipErrorKind kind) const;
};

} // namespace Semantic
} // namespace Sad

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif // SAD_OWNERSHIP_TRACKER_H
