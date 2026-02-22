// Disable Unicode warning for Arabic comments
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file ownership_manager.cpp
 * @brief (AR) تنفيذ مدير الملكية / (EN) Ownership Manager Implementation
 * @author Sad Language Team
 * @date February 2026
 * @version 1.0
 * @phase Phase 4: US2 - Memory Safety Integration
 */

#include "ownership_manager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace Sad {
namespace Data {

// ============================================================================
// بناء وهدم / Construction & Destruction
// ============================================================================

OwnershipManager::OwnershipManager()
    : enabled_(false)
    , useArabicMessages_(true)
    , debugMode_(false)
    , nextScopeId_(1)
    , currentScope_(0)
    , totalVariables_(0)
    , totalBorrows_(0)
    , totalMoves_(0) {
    scopeStack_.push_back(0);  // (AR) النطاق العالمي / (EN) Global scope
    initializeCopyTypes();
}

OwnershipManager::~OwnershipManager() = default;

// ============================================================================
// تهيئة الأنواع القابلة للنسخ / Initialize Copy Types
// ============================================================================

void OwnershipManager::initializeCopyTypes() {
    // (AR) الأنواع العربية الأساسية / (EN) Basic Arabic types
    copyTypes_ = {
        // أعداد صحيحة / Integers
        "\xD8\xAD""8",   // ح8
        "\xD8\xAD""16",  // ح16
        "\xD8\xAD""32",  // ح32
        "\xD8\xAD""64",  // ح64
        "\xD8\xB9""8",   // ع8
        "\xD8\xB9""16",  // ع16
        "\xD8\xB9""32",  // ع32
        "\xD8\xB9""64",  // ع64
        "\xD8\xB1\xD9\x82\xD9\x85",       // رقم
        "\xD8\xB9\xD8\xAF\xD8\xAF",       // عدد
        "\xD8\xB9\xD8\xAF\xD8\xAF_\xD8\xB5\xD8\xAD\xD9\x8A\xD8\xAD",  // عدد_صحيح
        
        // أعداد عشرية / Floats
        "\xD8\xB9\xD8\xB4""32",   // عش32
        "\xD8\xB9\xD8\xB4""64",   // عش64
        "\xD8\xB9\xD8\xB4\xD8\xB1\xD9\x8A",  // عشري
        
        // منطقي / Boolean
        "\xD9\x85\xD9\x86\xD8\xB7\xD9\x82\xD9\x8A",  // منطقي
        
        // حرف / Char
        "\xD8\xAD\xD8\xB1\xD9\x81",  // حرف
        
        // English equivalents
        "int", "i8", "i16", "i32", "i64",
        "u8", "u16", "u32", "u64",
        "f32", "f64", "float", "double",
        "bool", "boolean", "char",
        "number", "integer",
        
        // (AR) أنواع المفسر الداخلية / (EN) Interpreter internal types
        "INTEGER", "DOUBLE", "FLOAT", "BOOLEAN", "STRING"
    };
}

bool OwnershipManager::isCopyType(const std::string& typeName) const {
    return copyTypes_.count(typeName) > 0;
}

// ============================================================================
// إدارة النطاقات / Scope Management
// ============================================================================

size_t OwnershipManager::enterScope() {
    if (!enabled_) return 0;
    
    size_t scopeId = nextScopeId_++;
    scopeStack_.push_back(scopeId);
    currentScope_ = scopeId;
    
    if (debugMode_) {
        std::cout << "[ملكية] دخول نطاق " << scopeId << std::endl;
    }
    
    return scopeId;
}

void OwnershipManager::exitScope() {
    if (!enabled_) return;
    if (scopeStack_.size() <= 1) return;  // (AR) لا نخرج من النطاق العالمي
    
    size_t exitingScope = scopeStack_.back();
    
    if (debugMode_) {
        std::cout << "[ملكية] خروج من نطاق " << exitingScope << std::endl;
    }
    
    // (AR) إنهاء الاستعارات في هذا النطاق / (EN) End borrows in this scope
    endBorrowsInScope(exitingScope);
    
    // (AR) حذف المتغيرات المحلية / (EN) Drop local variables
    dropVariablesInScope(exitingScope);
    
    scopeStack_.pop_back();
    currentScope_ = scopeStack_.back();
}

void OwnershipManager::endBorrowsInScope(size_t scopeId) {
    for (auto& [name, info] : variables_) {
        auto it = std::remove_if(info.borrows.begin(), info.borrows.end(),
            [scopeId](const BorrowInfo& b) { return b.scopeId == scopeId; });
        
        if (it != info.borrows.end()) {
            info.borrows.erase(it, info.borrows.end());
            
            // (AR) إذا لم تعد هناك استعارات نشطة، أعد الحالة إلى "مملوك"
            if (info.borrows.empty() && 
                (info.state == OwnershipState::Borrowed || info.state == OwnershipState::BorrowedMut)) {
                info.state = OwnershipState::Owned;
            }
        }
    }
}

void OwnershipManager::dropVariablesInScope(size_t scopeId) {
    std::vector<std::string> toRemove;
    
    for (auto& [name, info] : variables_) {
        if (info.scopeId == scopeId) {
            // (AR) التحقق من عدم وجود استعارات معلقة
            if (info.hasActiveBorrows()) {
                auto error = makeError(OwnershipErrorKind::DropWhileBorrowed, name);
                reportError(error);
            }
            info.state = OwnershipState::Dropped;
            toRemove.push_back(name);
        }
    }
    
    for (const auto& name : toRemove) {
        variables_.erase(name);
    }
}

// ============================================================================
// تصريح واستخدام المتغيرات / Variable Declaration & Use
// ============================================================================

bool OwnershipManager::declareVariable(const std::string& name, const std::string& typeName) {
    if (!enabled_) return true;
    
    bool copyType = isCopyType(typeName);
    
    // (AR) أنواع المفسر البسيطة تُعتبر قابلة للنسخ دائماً
    // (EN) Simple interpreter types are always Copy
    if (typeName.empty()) {
        copyType = true;  // (AR) الأنواع غير المحددة تُعامل كأنواع نسخ للتوافق
    }
    
    variables_[name] = VariableOwnership(name, currentScope_, copyType);
    variables_[name].typeName = typeName;
    totalVariables_++;
    
    if (debugMode_) {
        std::cout << "[ملكية] تصريح: " << name 
                  << " (نوع: " << (typeName.empty() ? "تلقائي" : typeName) 
                  << ", نسخ: " << (copyType ? "نعم" : "لا") << ")" << std::endl;
    }
    
    return true;
}

std::optional<OwnershipError> OwnershipManager::useVariable(const std::string& name) {
    if (!enabled_) return std::nullopt;
    
    auto it = variables_.find(name);
    if (it == variables_.end()) return std::nullopt;  // (AR) متغير غير مُتتبع
    
    auto& info = it->second;
    
    switch (info.state) {
        case OwnershipState::Moved: {
            auto error = makeError(OwnershipErrorKind::UseAfterMove, name);
            reportError(error);
            return error;
        }
        case OwnershipState::Dropped: {
            auto error = makeError(OwnershipErrorKind::UseAfterMove, name);
            error.kind = OwnershipErrorKind::UseAfterMove;
            reportError(error);
            return error;
        }
        case OwnershipState::Uninitialized: {
            auto error = makeError(OwnershipErrorKind::UseOfUninitialized, name);
            reportError(error);
            return error;
        }
        default:
            break;
    }
    
    if (debugMode_) {
        std::cout << "[ملكية] استخدام: " << name << std::endl;
    }
    
    return std::nullopt;
}

std::optional<OwnershipError> OwnershipManager::moveVariable(const std::string& name) {
    if (!enabled_) return std::nullopt;
    
    auto it = variables_.find(name);
    if (it == variables_.end()) return std::nullopt;
    
    auto& info = it->second;
    
    // (AR) الأنواع القابلة للنسخ لا تُنقل / (EN) Copy types don't move
    if (info.isCopyType) {
        if (debugMode_) {
            std::cout << "[ملكية] نسخ (ليس نقل): " << name << std::endl;
        }
        return std::nullopt;
    }
    
    // (AR) التحقق من الحالة / (EN) Check state
    if (info.state == OwnershipState::Moved) {
        auto error = makeError(OwnershipErrorKind::DoubleMove, name);
        reportError(error);
        return error;
    }
    
    if (info.hasActiveBorrows()) {
        auto error = makeError(OwnershipErrorKind::BorrowOfMoved, name);
        reportError(error);
        return error;
    }
    
    // (AR) تنفيذ النقل / (EN) Execute move
    info.state = OwnershipState::Moved;
    totalMoves_++;
    
    if (debugMode_) {
        std::cout << "[ملكية] نقل: " << name << std::endl;
    }
    
    return std::nullopt;
}

std::optional<OwnershipError> OwnershipManager::mutateVariable(const std::string& name) {
    if (!enabled_) return std::nullopt;
    
    auto it = variables_.find(name);
    if (it == variables_.end()) return std::nullopt;
    
    auto& info = it->second;
    
    // (AR) لا يمكن التعديل أثناء الاستعارة / (EN) Cannot mutate while borrowed
    if (info.hasActiveBorrows()) {
        auto error = makeError(OwnershipErrorKind::MutateWhileBorrowed, name);
        reportError(error);
        return error;
    }
    
    // (AR) التحقق من أن المتغير ليس منقولاً / (EN) Check not moved
    if (info.state == OwnershipState::Moved) {
        auto error = makeError(OwnershipErrorKind::UseAfterMove, name);
        reportError(error);
        return error;
    }
    
    if (debugMode_) {
        std::cout << "[ملكية] تعديل: " << name << std::endl;
    }
    
    return std::nullopt;
}

// ============================================================================
// الاستعارة / Borrowing
// ============================================================================

std::optional<OwnershipError> OwnershipManager::createBorrow(
    const std::string& ownerName,
    const std::string& borrowerName,
    BorrowKind kind) {
    
    if (!enabled_) return std::nullopt;
    
    auto it = variables_.find(ownerName);
    if (it == variables_.end()) return std::nullopt;
    
    auto& info = it->second;
    
    // (AR) لا يمكن الاستعارة من منقول / (EN) Cannot borrow from moved
    if (info.state == OwnershipState::Moved) {
        auto error = makeError(OwnershipErrorKind::BorrowOfMoved, ownerName);
        reportError(error);
        return error;
    }
    
    if (kind == BorrowKind::Mutable) {
        // (AR) استعارة متغيرة: لا يمكن إذا كانت هناك أي استعارة أخرى
        if (info.hasActiveBorrows()) {
            auto error = makeError(OwnershipErrorKind::MutBorrowConflict, ownerName);
            reportError(error);
            return error;
        }
        info.state = OwnershipState::BorrowedMut;
    } else {
        // (AR) استعارة مشتركة: لا يمكن إذا كانت هناك استعارة متغيرة
        if (info.hasActiveMutableBorrow()) {
            auto error = makeError(OwnershipErrorKind::BorrowWhileMutBorrow, ownerName);
            reportError(error);
            return error;
        }
        info.state = OwnershipState::Borrowed;
    }
    
    info.borrows.emplace_back(borrowerName, kind, currentScope_);
    totalBorrows_++;
    
    if (debugMode_) {
        std::cout << "[ملكية] استعارة " 
                  << (kind == BorrowKind::Mutable ? "متغيرة" : "مشتركة")
                  << ": " << borrowerName << " من " << ownerName << std::endl;
    }
    
    return std::nullopt;
}

void OwnershipManager::endBorrow(const std::string& ownerName, const std::string& borrowerName) {
    if (!enabled_) return;
    
    auto it = variables_.find(ownerName);
    if (it == variables_.end()) return;
    
    auto& info = it->second;
    auto bit = std::remove_if(info.borrows.begin(), info.borrows.end(),
        [&borrowerName](const BorrowInfo& b) { return b.borrowerName == borrowerName; });
    
    if (bit != info.borrows.end()) {
        info.borrows.erase(bit, info.borrows.end());
        
        if (info.borrows.empty() && 
            (info.state == OwnershipState::Borrowed || info.state == OwnershipState::BorrowedMut)) {
            info.state = OwnershipState::Owned;
        }
    }
}

// ============================================================================
// استعلامات / Queries
// ============================================================================

bool OwnershipManager::variableExists(const std::string& name) const {
    return variables_.count(name) > 0;
}

bool OwnershipManager::canMove(const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) return false;
    
    const auto& info = it->second;
    return info.state == OwnershipState::Owned && !info.hasActiveBorrows();
}

std::optional<OwnershipState> OwnershipManager::getState(const std::string& name) const {
    auto it = variables_.find(name);
    if (it == variables_.end()) return std::nullopt;
    return it->second.state;
}

void OwnershipManager::reset() {
    variables_.clear();
    scopeStack_.clear();
    scopeStack_.push_back(0);
    errors_.clear();
    nextScopeId_ = 1;
    currentScope_ = 0;
    totalVariables_ = 0;
    totalBorrows_ = 0;
    totalMoves_ = 0;
}

// ============================================================================
// إنشاء الأخطاء / Error Creation
// ============================================================================

OwnershipError OwnershipManager::makeError(OwnershipErrorKind kind, const std::string& varName) {
    OwnershipError error;
    error.kind = kind;
    error.variableName = varName;
    error.message = getEnglishMessage(kind, varName);
    error.arabicMessage = getArabicMessage(kind, varName);
    error.suggestion = getSuggestion(kind);
    return error;
}

void OwnershipManager::reportError(const OwnershipError& error) {
    errors_.push_back(error);
    
    // (AR) طباعة الخطأ مباشرة / (EN) Print error immediately
    if (useArabicMessages_) {
        std::cerr << error.toArabicString() << std::endl;
    } else {
        std::cerr << error.toEnglishString() << std::endl;
    }
}

std::string OwnershipManager::getArabicMessage(OwnershipErrorKind kind, const std::string& varName) const {
    switch (kind) {
        case OwnershipErrorKind::UseAfterMove:
            return "المتغير '" + varName + "' مُنقول — لا يمكن استخدامه بعد النقل";
        case OwnershipErrorKind::DoubleMove:
            return "المتغير '" + varName + "' مُنقول مسبقاً — لا يمكن نقله مرة أخرى";
        case OwnershipErrorKind::BorrowOfMoved:
            return "لا يمكن استعارة المتغير '" + varName + "' لأنه مُنقول";
        case OwnershipErrorKind::MutBorrowConflict:
            return "لا يمكن إنشاء استعارة متغيرة للمتغير '" + varName + "' — يوجد استعارة نشطة";
        case OwnershipErrorKind::BorrowWhileMutBorrow:
            return "لا يمكن استعارة '" + varName + "' — يوجد استعارة متغيرة نشطة";
        case OwnershipErrorKind::MutBorrowWhileBorrow:
            return "لا يمكن إنشاء استعارة متغيرة لـ '" + varName + "' — يوجد استعارات مشتركة";
        case OwnershipErrorKind::MutateWhileBorrowed:
            return "لا يمكن تعديل '" + varName + "' أثناء وجود استعارة نشطة";
        case OwnershipErrorKind::UseOfUninitialized:
            return "المتغير '" + varName + "' غير مُهيّأ — يجب تعيين قيمة له أولاً";
        case OwnershipErrorKind::DropWhileBorrowed:
            return "لا يمكن حذف '" + varName + "' — لا يزال مُستعاراً";
        case OwnershipErrorKind::InvalidLifetime:
            return "عمر المرجع إلى '" + varName + "' غير صالح";
        default:
            return "خطأ ملكية غير معروف للمتغير '" + varName + "'";
    }
}

std::string OwnershipManager::getEnglishMessage(OwnershipErrorKind kind, const std::string& varName) const {
    switch (kind) {
        case OwnershipErrorKind::UseAfterMove:
            return "variable '" + varName + "' has been moved — cannot use after move";
        case OwnershipErrorKind::DoubleMove:
            return "variable '" + varName + "' already moved — cannot move again";
        case OwnershipErrorKind::BorrowOfMoved:
            return "cannot borrow '" + varName + "' because it has been moved";
        case OwnershipErrorKind::MutBorrowConflict:
            return "cannot create mutable borrow of '" + varName + "' — active borrow exists";
        case OwnershipErrorKind::BorrowWhileMutBorrow:
            return "cannot borrow '" + varName + "' — active mutable borrow exists";
        case OwnershipErrorKind::MutBorrowWhileBorrow:
            return "cannot mutably borrow '" + varName + "' — shared borrows exist";
        case OwnershipErrorKind::MutateWhileBorrowed:
            return "cannot mutate '" + varName + "' while it is borrowed";
        case OwnershipErrorKind::UseOfUninitialized:
            return "variable '" + varName + "' is uninitialized — assign a value first";
        case OwnershipErrorKind::DropWhileBorrowed:
            return "cannot drop '" + varName + "' — it is still borrowed";
        case OwnershipErrorKind::InvalidLifetime:
            return "invalid lifetime for reference to '" + varName + "'";
        default:
            return "unknown ownership error for variable '" + varName + "'";
    }
}

std::string OwnershipManager::getSuggestion(OwnershipErrorKind kind) const {
    switch (kind) {
        case OwnershipErrorKind::UseAfterMove:
            return useArabicMessages_ 
                ? "استخدم .انسخ() لإنشاء نسخة قبل النقل، أو أعد هيكلة الكود"
                : "Use .clone() to create a copy before moving, or restructure code";
        case OwnershipErrorKind::DoubleMove:
            return useArabicMessages_
                ? "تأكد من عدم نقل المتغير أكثر من مرة"
                : "Make sure variable is not moved more than once";
        case OwnershipErrorKind::MutBorrowConflict:
            return useArabicMessages_
                ? "أنهِ الاستعارة الحالية قبل إنشاء استعارة متغيرة"
                : "End current borrow before creating mutable borrow";
        case OwnershipErrorKind::MutateWhileBorrowed:
            return useArabicMessages_
                ? "أنهِ جميع الاستعارات قبل تعديل المتغير"
                : "End all borrows before mutating variable";
        case OwnershipErrorKind::UseOfUninitialized:
            return useArabicMessages_
                ? "عيّن قيمة للمتغير قبل استخدامه"
                : "Assign a value to the variable before using it";
        default:
            return "";
    }
}

// ============================================================================
// التنقيح / Debugging
// ============================================================================

void OwnershipManager::dump() const {
    std::cout << "\n╔══════════════════════════════════════════╗\n";
    std::cout << "║       حالة نظام الملكية                  ║\n";
    std::cout << "╠══════════════════════════════════════════╣\n";
    std::cout << "║ مفعّل: " << (enabled_ ? "نعم ✓" : "لا ✗") << std::endl;
    std::cout << "║ نطاق حالي: " << currentScope_ << std::endl;
    std::cout << "║ متغيرات: " << variables_.size() << std::endl;
    std::cout << "║ أخطاء: " << errors_.size() << std::endl;
    
    for (const auto& [name, info] : variables_) {
        std::cout << "║   " << name << ": ";
        switch (info.state) {
            case OwnershipState::Owned: std::cout << "مملوك ✓"; break;
            case OwnershipState::Moved: std::cout << "منقول ✗"; break;
            case OwnershipState::Borrowed: std::cout << "مستعار &"; break;
            case OwnershipState::BorrowedMut: std::cout << "مستعار &متغير"; break;
            case OwnershipState::Dropped: std::cout << "محذوف ☠"; break;
            case OwnershipState::Uninitialized: std::cout << "غير مهيأ ?"; break;
        }
        if (!info.borrows.empty()) {
            std::cout << " (استعارات: " << info.borrows.size() << ")";
        }
        std::cout << std::endl;
    }
    
    std::cout << "╚══════════════════════════════════════════╝\n\n";
}

std::string OwnershipManager::getSummary() const {
    std::ostringstream oss;
    if (useArabicMessages_) {
        oss << "ملخص نظام الملكية:\n";
        oss << "  الحالة: " << (errors_.empty() ? "نجح ✓" : "أخطاء ✗") << "\n";
        oss << "  المتغيرات: " << totalVariables_ << "\n";
        oss << "  الاستعارات: " << totalBorrows_ << "\n";
        oss << "  عمليات النقل: " << totalMoves_ << "\n";
        oss << "  الأخطاء: " << errors_.size() << "\n";
    } else {
        oss << "Ownership Summary:\n";
        oss << "  Status: " << (errors_.empty() ? "PASSED" : "ERRORS") << "\n";
        oss << "  Variables: " << totalVariables_ << "\n";
        oss << "  Borrows: " << totalBorrows_ << "\n";
        oss << "  Moves: " << totalMoves_ << "\n";
        oss << "  Errors: " << errors_.size() << "\n";
    }
    return oss.str();
}

} // namespace Data
} // namespace Sad
