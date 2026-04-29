// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file ownership_tracker.cpp
 * @brief تنفيذ نظام تتبع الملكية / Ownership Tracking Implementation
 * @author Sad Language Team
 * @date January 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety
 */

#include "ownership/ownership_tracker.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Sad {
namespace Semantic {

// ============================================================================
// تنفيذ OwnershipError / OwnershipError Implementation
// ============================================================================

std::string OwnershipError::toArabicString() const {
    std::ostringstream oss;
    oss << "خطأ[ص" << std::setfill('0') << std::setw(4) 
        << static_cast<int>(kind) << "]: " << arabicMessage << "\n";
    oss << "   --> " << errorLocation.toString() << "\n";
    if (!relatedLocation.file.empty()) {
        oss << "   |   موقع ذو صلة: " << relatedLocation.toString() << "\n";
    }
    if (!suggestion.empty()) {
        oss << "   = اقتراح: " << suggestion << "\n";
    }
    return oss.str();
}

std::string OwnershipError::toEnglishString() const {
    std::ostringstream oss;
    oss << "error[S" << std::setfill('0') << std::setw(4) 
        << static_cast<int>(kind) << "]: " << message << "\n";
    oss << "   --> " << errorLocation.toString() << "\n";
    if (!relatedLocation.file.empty()) {
        oss << "   |   related location: " << relatedLocation.toString() << "\n";
    }
    if (!suggestion.empty()) {
        oss << "   = suggestion: " << suggestion << "\n";
    }
    return oss.str();
}

// ============================================================================
// بناء وهدم / Construction & Destruction
// ============================================================================

OwnershipTracker::OwnershipTracker() 
    : nextScopeId_(1)
    , currentScope_(0) {
    // (AR) النطاق العالمي يبدأ من 0
    // (EN) Global scope starts at 0
    scopeStack_.push_back(0);
}

OwnershipTracker::~OwnershipTracker() {
    // (AR) تنظيف تلقائي
    // (EN) Automatic cleanup
}

// ============================================================================
// إدارة النطاقات / Scope Management
// ============================================================================

size_t OwnershipTracker::enterScope() {
    size_t newScope = nextScopeId_++;
    scopeStack_.push_back(newScope);
    currentScope_ = newScope;
    return newScope;
}

void OwnershipTracker::exitScope() {
    if (scopeStack_.size() <= 1) {
        // (AR) لا يمكن الخروج من النطاق العالمي
        // (EN) Cannot exit global scope
        return;
    }
    
    size_t exitingScope = scopeStack_.back();
    
    // (AR) إنهاء جميع الاستعارات في هذا النطاق
    // (EN) End all borrows in this scope
    endBorrowsInScope(exitingScope);
    
    // (AR) حذف المتغيرات المحلية
    // (EN) Drop local variables
    dropVariablesInScope(exitingScope);
    
    scopeStack_.pop_back();
    currentScope_ = scopeStack_.back();
}

// ============================================================================
// تصريح واستخدام المتغيرات / Variable Declaration & Use
// ============================================================================

bool OwnershipTracker::declareVariable(const std::string& name,
                                       const std::string& typeName,
                                       const SourceLocation& location,
                                       bool isCopyType) {
    // (AR) التحقق من عدم وجود المتغير في النطاق الحالي
    // (EN) Check variable doesn't exist in current scope
    auto it = variables_.find(name);
    if (it != variables_.end() && it->second.scopeId == currentScope_) {
        // (AR) إعادة تعريف في نفس النطاق
        // (EN) Redefinition in same scope
        return false;
    }
    
    OwnershipInfo info(name, location, currentScope_);
    info.typeName = typeName;
    info.isCopyType = isCopyType;
    info.state = OwnershipState::Owned;
    
    variables_[name] = info;
    return true;
}

std::optional<OwnershipError> OwnershipTracker::useVariable(
    const std::string& name, 
    const SourceLocation& location) {
    
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        // (AR) المتغير غير موجود - سيتم التعامل معه في مرحلة أخرى
        // (EN) Variable doesn't exist - handled elsewhere
        return std::nullopt;
    }
    
    OwnershipInfo& info = it->second;
    
    // (AR) الأنواع القابلة للنسخ لا تُنقل أبداً حتى لو أُشير لحالتها كمنقولة
    // (EN) Copy types are never truly moved, even if state was set to Moved
    if (info.isCopyType) {
        return std::nullopt;
    }
    
    // (AR) المتغيرات العامة (النطاق 0) يمكن استخدامها دائماً
    // (EN) Global variables (scope 0) can always be used
    if (info.scopeId == 0) {
        return std::nullopt;
    }
    
    // (AR) التحقق من الحالة
    // (EN) Check state
    switch (info.state) {
        case OwnershipState::Moved:
            return makeError(OwnershipErrorKind::UseAfterMove, name, 
                           location, info.movedAt);
        
        case OwnershipState::Dropped:
            return makeError(OwnershipErrorKind::UseAfterMove, name, 
                           location, info.movedAt);
        
        case OwnershipState::Uninitialized:
            return makeError(OwnershipErrorKind::UseOfUninitialized, name, 
                           location, info.definedAt);
        
        case OwnershipState::Owned:
        case OwnershipState::Borrowed:
        case OwnershipState::BorrowedMut:
            // (AR) الاستخدام مسموح
            // (EN) Use is allowed
            return std::nullopt;
    }
    
    return std::nullopt;
}

std::optional<OwnershipError> OwnershipTracker::moveVariable(
    const std::string& name,
    const SourceLocation& location) {
    
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        return std::nullopt;
    }
    
    OwnershipInfo& info = it->second;
    
    // (AR) الأنواع القابلة للنسخ لا تُنقل، تُنسخ
    // (EN) Copy types don't move, they copy
    if (info.isCopyType) {
        return std::nullopt;
    }
    
    // (AR) المتغيرات العامة (النطاق 0) تُنسخ دائماً - لا يمكن نقلها لأنها تستمر طوال البرنامج
    // (EN) Global variables (scope 0) are always copied - cannot be moved as they persist for program lifetime
    if (info.scopeId == 0) {
        return std::nullopt;
    }
    
    // (AR) التحقق من الحالة
    // (EN) Check state
    switch (info.state) {
        case OwnershipState::Moved:
            return makeError(OwnershipErrorKind::DoubleMove, name, 
                           location, info.movedAt);
        
        case OwnershipState::Borrowed:
        case OwnershipState::BorrowedMut:
            // (AR) لا يمكن نقل قيمة مستعارة
            // (EN) Cannot move borrowed value
            if (!info.borrows.empty()) {
                return makeError(OwnershipErrorKind::BorrowOfMoved, name,
                               location, info.borrows[0].location);
            }
            break;
        
        case OwnershipState::Uninitialized:
            return makeError(OwnershipErrorKind::UseOfUninitialized, name,
                           location, info.definedAt);
        
        case OwnershipState::Owned:
            // (AR) النقل مسموح
            // (EN) Move is allowed
            info.state = OwnershipState::Moved;
            info.movedAt = location;
            return std::nullopt;
        
        case OwnershipState::Dropped:
            return makeError(OwnershipErrorKind::UseAfterMove, name,
                           location, info.movedAt);
    }
    
    return std::nullopt;
}

std::optional<OwnershipError> OwnershipTracker::mutateVariable(
    const std::string& name,
    const SourceLocation& location) {
    
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        return std::nullopt;
    }
    
    OwnershipInfo& info = it->second;
    
    // (AR) التحقق من وجود استعارات نشطة
    // (EN) Check for active borrows
    if (info.hasActiveBorrows()) {
        return makeError(OwnershipErrorKind::MutateWhileBorrowed, name,
                       location, info.borrows[0].location);
    }
    
    // (AR) التحقق من الحالة
    // (EN) Check state
    switch (info.state) {
        case OwnershipState::Moved:
            return makeError(OwnershipErrorKind::UseAfterMove, name,
                           location, info.movedAt);
        
        case OwnershipState::Uninitialized:
            return makeError(OwnershipErrorKind::UseOfUninitialized, name,
                           location, info.definedAt);
        
        case OwnershipState::Owned:
            // (AR) التعديل مسموح
            // (EN) Mutation is allowed
            return std::nullopt;
        
        case OwnershipState::Dropped:
            return makeError(OwnershipErrorKind::UseAfterMove, name,
                           location, info.movedAt);
        
        default:
            return std::nullopt;
    }
}

// ============================================================================
// الاستعارة / Borrowing
// ============================================================================

std::optional<OwnershipError> OwnershipTracker::createBorrow(
    const std::string& ownerName,
    const std::string& borrowerName,
    BorrowKind kind,
    const SourceLocation& location) {
    
    auto it = variables_.find(ownerName);
    if (it == variables_.end()) {
        return std::nullopt;
    }
    
    OwnershipInfo& info = it->second;
    
    // (AR) التحقق من حالة المالك
    // (EN) Check owner state
    if (info.state == OwnershipState::Moved) {
        return makeError(OwnershipErrorKind::BorrowOfMoved, ownerName,
                       location, info.movedAt);
    }
    
    if (info.state == OwnershipState::Uninitialized) {
        return makeError(OwnershipErrorKind::UseOfUninitialized, ownerName,
                       location, info.definedAt);
    }
    
    // (AR) قواعد الاستعارة
    // (EN) Borrowing rules
    if (kind == BorrowKind::Mutable) {
        // (AR) استعارة متغيرة: لا يمكن وجود أي استعارة أخرى
        // (EN) Mutable borrow: no other borrows allowed
        if (info.hasActiveBorrows()) {
            if (info.hasActiveMutableBorrow()) {
                return makeError(OwnershipErrorKind::MutBorrowConflict, ownerName,
                               location, info.borrows[0].location);
            } else {
                return makeError(OwnershipErrorKind::MutBorrowWhileBorrow, ownerName,
                               location, info.borrows[0].location);
            }
        }
        info.state = OwnershipState::BorrowedMut;
    } else {
        // (AR) استعارة ثابتة: لا يمكن وجود استعارة متغيرة
        // (EN) Immutable borrow: no mutable borrow allowed
        if (info.hasActiveMutableBorrow()) {
            return makeError(OwnershipErrorKind::BorrowWhileMutBorrow, ownerName,
                           location, info.borrows[0].location);
        }
        info.state = OwnershipState::Borrowed;
    }
    
    // (AR) إضافة الاستعارة
    // (EN) Add the borrow
    info.borrows.emplace_back(borrowerName, kind, location, currentScope_);
    
    return std::nullopt;
}

void OwnershipTracker::endBorrow(const std::string& ownerName, 
                                  const std::string& borrowerName) {
    auto it = variables_.find(ownerName);
    if (it == variables_.end()) {
        return;
    }
    
    OwnershipInfo& info = it->second;
    
    // (AR) إزالة الاستعارة
    // (EN) Remove the borrow
    info.borrows.erase(
        std::remove_if(info.borrows.begin(), info.borrows.end(),
            [&borrowerName](const BorrowInfo& b) {
                return b.borrowerName == borrowerName;
            }),
        info.borrows.end()
    );
    
    // (AR) تحديث الحالة
    // (EN) Update state
    if (info.borrows.empty()) {
        info.state = OwnershipState::Owned;
    } else if (!info.hasActiveMutableBorrow()) {
        info.state = OwnershipState::Borrowed;
    }
}

void OwnershipTracker::endBorrowsInScope(size_t scopeId) {
    for (auto& [name, info] : variables_) {
        // (AR) إزالة الاستعارات من هذا النطاق
        // (EN) Remove borrows from this scope
        info.borrows.erase(
            std::remove_if(info.borrows.begin(), info.borrows.end(),
                [scopeId](const BorrowInfo& b) {
                    return b.scopeId == scopeId;
                }),
            info.borrows.end()
        );
        
        // (AR) تحديث الحالة
        // (EN) Update state
        if (info.borrows.empty() && 
            (info.state == OwnershipState::Borrowed || 
             info.state == OwnershipState::BorrowedMut)) {
            info.state = OwnershipState::Owned;
        }
    }
}

// ============================================================================
// استعلامات / Queries
// ============================================================================

std::optional<OwnershipInfo> OwnershipTracker::getOwnershipInfo(
    const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        return std::nullopt;
    }
    return it->second;
}

bool OwnershipTracker::canMove(const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        return false;
    }
    const OwnershipInfo& info = it->second;
    return info.state == OwnershipState::Owned && !info.hasActiveBorrows();
}

bool OwnershipTracker::canBorrow(const std::string& name, BorrowKind kind) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) {
        return false;
    }
    const OwnershipInfo& info = it->second;
    
    if (info.state == OwnershipState::Moved || 
        info.state == OwnershipState::Uninitialized ||
        info.state == OwnershipState::Dropped) {
        return false;
    }
    
    if (kind == BorrowKind::Mutable) {
        return !info.hasActiveBorrows();
    } else {
        return !info.hasActiveMutableBorrow();
    }
}

bool OwnershipTracker::variableExists(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

// ============================================================================
// تنقيح / Debugging
// ============================================================================

void OwnershipTracker::dump() const {
    std::cout << "=== حالة الملكية / Ownership State ===\n";
    std::cout << "النطاق الحالي / Current Scope: " << currentScope_ << "\n";
    std::cout << "عدد المتغيرات / Variables: " << variables_.size() << "\n\n";
    
    for (const auto& [name, info] : variables_) {
        std::cout << "  " << name << ":\n";
        std::cout << "    النوع / Type: " << info.typeName << "\n";
        std::cout << "    الحالة / State: ";
        
        switch (info.state) {
            case OwnershipState::Owned: std::cout << "مملوك/Owned"; break;
            case OwnershipState::Moved: std::cout << "منقول/Moved"; break;
            case OwnershipState::Borrowed: std::cout << "مستعار/Borrowed"; break;
            case OwnershipState::BorrowedMut: std::cout << "مستعار_متغير/BorrowedMut"; break;
            case OwnershipState::Dropped: std::cout << "محذوف/Dropped"; break;
            case OwnershipState::Uninitialized: std::cout << "غير_مهيأ/Uninitialized"; break;
        }
        std::cout << "\n";
        
        std::cout << "    النطاق / Scope: " << info.scopeId << "\n";
        std::cout << "    قابل للنسخ / Copy: " << (info.isCopyType ? "نعم/yes" : "لا/no") << "\n";
        
        if (!info.borrows.empty()) {
            std::cout << "    الاستعارات / Borrows:\n";
            for (const auto& b : info.borrows) {
                std::cout << "      - " << b.borrowerName << " ("
                          << (b.kind == BorrowKind::Mutable ? "متغير/mut" : "ثابت/shared")
                          << ") at " << b.location.toString() << "\n";
            }
        }
        std::cout << "\n";
    }
}

// ============================================================================
// دوال مساعدة خاصة / Private Helper Functions
// ============================================================================

OwnershipError OwnershipTracker::makeError(
    OwnershipErrorKind kind,
    const std::string& varName,
    const SourceLocation& errorLoc,
    const SourceLocation& relatedLoc) {
    
    OwnershipError error;
    error.kind = kind;
    error.variableName = varName;
    error.errorLocation = errorLoc;
    error.relatedLocation = relatedLoc;
    error.message = getEnglishErrorMessage(kind, varName);
    error.arabicMessage = getArabicErrorMessage(kind, varName);
    error.suggestion = getSuggestion(kind);
    
    errors_.push_back(error);
    return error;
}

void OwnershipTracker::dropVariablesInScope(size_t scopeId) {
    std::vector<std::string> toRemove;
    
    for (auto& [name, info] : variables_) {
        if (info.scopeId == scopeId) {
            // (AR) التحقق من عدم وجود استعارات معلقة
            // (EN) Check for dangling borrows
            if (info.hasActiveBorrows()) {
                makeError(OwnershipErrorKind::DropWhileBorrowed, name,
                         SourceLocation(), info.borrows[0].location);
            }
            
            info.state = OwnershipState::Dropped;
            toRemove.push_back(name);
        }
    }
    
    // (AR) إزالة المتغيرات المحلية
    // (EN) Remove local variables
    for (const auto& name : toRemove) {
        variables_.erase(name);
    }
}

std::string OwnershipTracker::getArabicErrorMessage(
    OwnershipErrorKind kind,
    const std::string& varName) const {
    
    switch (kind) {
        case OwnershipErrorKind::UseAfterMove:
            return "استخدام المتغير '" + varName + "' بعد نقل ملكيته";
        
        case OwnershipErrorKind::DoubleMove:
            return "محاولة نقل المتغير '" + varName + "' مرتين";
        
        case OwnershipErrorKind::BorrowOfMoved:
            return "استعارة المتغير '" + varName + "' بعد نقله";
        
        case OwnershipErrorKind::MutBorrowConflict:
            return "لا يمكن إنشاء استعارتين متغيرتين للمتغير '" + varName + "'";
        
        case OwnershipErrorKind::BorrowWhileMutBorrow:
            return "لا يمكن استعارة '" + varName + "' أثناء وجود استعارة متغيرة";
        
        case OwnershipErrorKind::MutBorrowWhileBorrow:
            return "لا يمكن إنشاء استعارة متغيرة لـ'" + varName + "' أثناء وجود استعارات ثابتة";
        
        case OwnershipErrorKind::MutateWhileBorrowed:
            return "لا يمكن تعديل '" + varName + "' أثناء استعارته";
        
        case OwnershipErrorKind::UseOfUninitialized:
            return "استخدام المتغير '" + varName + "' قبل تهيئته";
        
        case OwnershipErrorKind::DropWhileBorrowed:
            return "حذف '" + varName + "' أثناء وجود استعارات نشطة";
        
        case OwnershipErrorKind::InvalidLifetime:
            return "عمر غير صالح للمرجع إلى '" + varName + "'";
        
        default:
            return "خطأ ملكية غير معروف للمتغير '" + varName + "'";
    }
}

std::string OwnershipTracker::getEnglishErrorMessage(
    OwnershipErrorKind kind,
    const std::string& varName) const {
    
    switch (kind) {
        case OwnershipErrorKind::UseAfterMove:
            return "use of moved value: `" + varName + "`";
        
        case OwnershipErrorKind::DoubleMove:
            return "value moved here: `" + varName + "` (moved previously)";
        
        case OwnershipErrorKind::BorrowOfMoved:
            return "borrow of moved value: `" + varName + "`";
        
        case OwnershipErrorKind::MutBorrowConflict:
            return "cannot borrow `" + varName + "` as mutable more than once at a time";
        
        case OwnershipErrorKind::BorrowWhileMutBorrow:
            return "cannot borrow `" + varName + "` as immutable because it is also borrowed as mutable";
        
        case OwnershipErrorKind::MutBorrowWhileBorrow:
            return "cannot borrow `" + varName + "` as mutable because it is also borrowed as immutable";
        
        case OwnershipErrorKind::MutateWhileBorrowed:
            return "cannot assign to `" + varName + "` because it is borrowed";
        
        case OwnershipErrorKind::UseOfUninitialized:
            return "use of uninitialized variable: `" + varName + "`";
        
        case OwnershipErrorKind::DropWhileBorrowed:
            return "cannot drop `" + varName + "` while still borrowed";
        
        case OwnershipErrorKind::InvalidLifetime:
            return "lifetime may not live long enough for `" + varName + "`";
        
        default:
            return "unknown ownership error for `" + varName + "`";
    }
}

std::string OwnershipTracker::getSuggestion(OwnershipErrorKind kind) const {
    switch (kind) {
        case OwnershipErrorKind::UseAfterMove:
            return "استخدم .استنسخ() إذا كنت تحتاج نسخة من القيمة / use .clone() if you need a copy";
        
        case OwnershipErrorKind::DoubleMove:
            return "احفظ القيمة في متغير مؤقت قبل النقل / store value in temporary before move";
        
        case OwnershipErrorKind::BorrowOfMoved:
            return "أنشئ الاستعارة قبل نقل القيمة / create borrow before moving value";
        
        case OwnershipErrorKind::MutBorrowConflict:
            return "استخدم استعارة واحدة متغيرة في كل مرة / use one mutable borrow at a time";
        
        case OwnershipErrorKind::BorrowWhileMutBorrow:
        case OwnershipErrorKind::MutBorrowWhileBorrow:
            return "تأكد من انتهاء الاستعارة الأولى قبل إنشاء الثانية / ensure first borrow ends before creating second";
        
        case OwnershipErrorKind::MutateWhileBorrowed:
            return "أنهِ الاستعارة قبل التعديل / end borrow before mutation";
        
        case OwnershipErrorKind::UseOfUninitialized:
            return "هيّئ المتغير قبل استخدامه / initialize variable before use";
        
        case OwnershipErrorKind::DropWhileBorrowed:
            return "تأكد من انتهاء جميع الاستعارات قبل نهاية النطاق / ensure all borrows end before scope exit";
        
        case OwnershipErrorKind::InvalidLifetime:
            return "أضف قيود العمر الصريحة أو أعد هيكلة الكود / add explicit lifetime bounds or restructure code";
        
        default:
            return "";
    }
}

} // namespace Semantic
} // namespace Sad
