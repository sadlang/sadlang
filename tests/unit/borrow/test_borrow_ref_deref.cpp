// ════════════════════════════════════════════════════════════════════════════════
// ملف: test_borrow_ref_deref.cpp
// File: test_borrow_ref_deref.cpp
//
// الوصف: اختبارات وحدة لـ borrow checker مع المراجع — 30+ اختبار
// Description: Borrow checker unit tests with references — 30+ tests
//
// المؤلف: Sad Compiler — Phase ح — م-ح03
// ════════════════════════════════════════════════════════════════════════════════

#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

#include <gtest/gtest.h>
#include <optional>
#include <string>
#include <vector>

// ═══════════════════════════════════════════════════════
// Minimal borrow checker types for self-contained testing
// أنواع مُصغّرة لـ borrow checker لاختبار مستقل
// ═══════════════════════════════════════════════════════

namespace Sad {
namespace Semantic {

// ─── Enums ───

enum class OwnershipState {
    Owned, Moved, Borrowed, BorrowedMut, Dropped, Uninitialized
};

enum class BorrowKind {
    Shared,    // & (immutable)
    Mutable    // &mut / &متغير 
};

enum class OwnershipErrorKind {
    UseAfterMove,
    DoubleMove,
    BorrowOfMoved,
    MutBorrowConflict,
    BorrowWhileMutBorrow,
    MutBorrowWhileBorrow,
    MutateWhileBorrowed,
    UseOfUninitialized,
    DropWhileBorrowed,
    InvalidLifetime
};

// ─── Structs ───

struct SourceLocation {
    std::string file;
    size_t line = 0;
    size_t column = 0;
    
    SourceLocation() = default;
    SourceLocation(const std::string& f, size_t l, size_t c)
        : file(f), line(l), column(c) {}
};

struct BorrowInfo {
    std::string borrowerName;
    BorrowKind kind;
    SourceLocation location;
    size_t scopeId;
    
    BorrowInfo() : kind(BorrowKind::Shared), scopeId(0) {}
    BorrowInfo(const std::string& name, BorrowKind k, const SourceLocation& loc, size_t scope)
        : borrowerName(name), kind(k), location(loc), scopeId(scope) {}
};

struct OwnershipError {
    OwnershipErrorKind kind;
    std::string variableName;
    SourceLocation errorLocation;
    SourceLocation relatedLocation;
    std::string message;
};

struct OwnershipInfo {
    std::string variableName;
    OwnershipState state;
    SourceLocation definedAt;
    SourceLocation movedAt;
    std::vector<BorrowInfo> borrows;
    size_t scopeId;
    bool isCopyType;
    std::string typeName;
    
    bool hasActiveBorrows() const { return !borrows.empty(); }
    bool hasActiveMutableBorrow() const {
        for (const auto& b : borrows)
            if (b.kind == BorrowKind::Mutable) return true;
        return false;
    }
    size_t sharedBorrowCount() const {
        size_t count = 0;
        for (const auto& b : borrows)
            if (b.kind == BorrowKind::Shared) count++;
        return count;
    }
};

// ═══════════════════════════════════════════════════════
// OwnershipTracker — التتبع المُبسّط / Simplified Tracker
// (مُحاكاة لنفس واجهة OwnershipTracker الحقيقي)
// ═══════════════════════════════════════════════════════

class OwnershipTracker {
public:
    OwnershipTracker() : currentScope_(0) {}
    
    // ─── إدارة النطاق / Scope management ───
    size_t enterScope() { return ++currentScope_; }
    
    void exitScope() {
        // إنهاء الاستعارات في النطاق الحالي / End borrows in current scope
        for (auto& [name, info] : variables_) {
            info.borrows.erase(
                std::remove_if(info.borrows.begin(), info.borrows.end(),
                    [this](const BorrowInfo& b) { return b.scopeId == currentScope_; }),
                info.borrows.end());
            
            // إعادة الحالة إذا لم يعد هناك استعارة / Reset state if no borrows
            if (info.borrows.empty() && 
                (info.state == OwnershipState::Borrowed || info.state == OwnershipState::BorrowedMut)) {
                info.state = OwnershipState::Owned;
            }
        }
        if (currentScope_ > 0) currentScope_--;
    }
    
    size_t currentScopeId() const { return currentScope_; }
    
    // ─── دورة حياة المتغيرات / Variable lifecycle ───
    bool declareVariable(const std::string& name, const std::string& typeName, 
                          const SourceLocation& loc, bool isCopyType = false) {
        if (variables_.count(name)) return false;
        
        OwnershipInfo info;
        info.variableName = name;
        info.state = OwnershipState::Owned;
        info.definedAt = loc;
        info.scopeId = currentScope_;
        info.isCopyType = isCopyType;
        info.typeName = typeName;
        variables_[name] = info;
        return true;
    }
    
    std::optional<OwnershipError> useVariable(const std::string& name, const SourceLocation& loc) {
        auto it = variables_.find(name);
        if (it == variables_.end()) return std::nullopt;
        
        auto& info = it->second;
        
        if (info.state == OwnershipState::Moved) {
            return makeError(OwnershipErrorKind::UseAfterMove, name, loc, info.movedAt);
        }
        if (info.state == OwnershipState::Uninitialized) {
            return makeError(OwnershipErrorKind::UseOfUninitialized, name, loc, info.definedAt);
        }
        return std::nullopt;
    }
    
    std::optional<OwnershipError> moveVariable(const std::string& name, const SourceLocation& loc) {
        auto it = variables_.find(name);
        if (it == variables_.end()) return std::nullopt;
        
        auto& info = it->second;
        
        if (info.state == OwnershipState::Moved) {
            return makeError(OwnershipErrorKind::DoubleMove, name, loc, info.movedAt);
        }
        if (info.hasActiveBorrows()) {
            return makeError(OwnershipErrorKind::DropWhileBorrowed, name, loc, 
                              info.borrows.front().location);
        }
        if (info.isCopyType) {
            return std::nullopt;  // النسخ بدل النقل / Copy instead of move
        }
        
        info.state = OwnershipState::Moved;
        info.movedAt = loc;
        return std::nullopt;
    }
    
    std::optional<OwnershipError> mutateVariable(const std::string& name, const SourceLocation& loc) {
        auto it = variables_.find(name);
        if (it == variables_.end()) return std::nullopt;
        
        auto& info = it->second;
        
        if (info.state == OwnershipState::Moved) {
            return makeError(OwnershipErrorKind::UseAfterMove, name, loc, info.movedAt);
        }
        
        // لا يمكن التعديل أثناء الاستعارة المشتركة / Can't mutate while borrowed
        if (info.sharedBorrowCount() > 0) {
            return makeError(OwnershipErrorKind::MutateWhileBorrowed, name, loc,
                              info.borrows.front().location);
        }
        
        return std::nullopt;
    }
    
    // ─── الاستعارة / Borrowing ───
    std::optional<OwnershipError> createBorrow(const std::string& ownerName,
                                                 const std::string& borrowerName,
                                                 BorrowKind kind,
                                                 const SourceLocation& loc) {
        auto it = variables_.find(ownerName);
        if (it == variables_.end()) return std::nullopt;
        
        auto& info = it->second;
        
        // لا يمكن استعارة متغير منقول / Can't borrow moved variable
        if (info.state == OwnershipState::Moved) {
            return makeError(OwnershipErrorKind::BorrowOfMoved, ownerName, loc, info.movedAt);
        }
        
        if (kind == BorrowKind::Mutable) {
            // لا يمكن استعارة متغيرة إذا توجد استعارة مشتركة نشطة
            // Can't create mutable borrow if shared borrows exist
            if (info.sharedBorrowCount() > 0) {
                return makeError(OwnershipErrorKind::MutBorrowWhileBorrow, ownerName, loc,
                                  info.borrows.front().location);
            }
            // لا يمكن استعارة متغيرة ثانية
            // Can't create second mutable borrow
            if (info.hasActiveMutableBorrow()) {
                return makeError(OwnershipErrorKind::MutBorrowConflict, ownerName, loc,
                                  info.borrows.front().location);
            }
            info.state = OwnershipState::BorrowedMut;
        } else {
            // لا يمكن استعارة مشتركة إذا توجد استعارة متغيرة
            // Can't create shared borrow if mutable borrow exists
            if (info.hasActiveMutableBorrow()) {
                return makeError(OwnershipErrorKind::BorrowWhileMutBorrow, ownerName, loc,
                                  info.borrows.front().location);
            }
            info.state = OwnershipState::Borrowed;
        }
        
        info.borrows.emplace_back(borrowerName, kind, loc, currentScope_);
        return std::nullopt;
    }
    
    void endBorrow(const std::string& ownerName, const std::string& borrowerName) {
        auto it = variables_.find(ownerName);
        if (it == variables_.end()) return;
        
        auto& borrows = it->second.borrows;
        borrows.erase(
            std::remove_if(borrows.begin(), borrows.end(),
                [&](const BorrowInfo& b) { return b.borrowerName == borrowerName; }),
            borrows.end());
        
        if (borrows.empty()) {
            it->second.state = OwnershipState::Owned;
        }
    }
    
    // ─── استعلامات / Queries ───
    std::optional<OwnershipInfo> getOwnershipInfo(const std::string& name) const {
        auto it = variables_.find(name);
        if (it == variables_.end()) return std::nullopt;
        return it->second;
    }
    
    bool canMove(const std::string& name) const {
        auto it = variables_.find(name);
        if (it == variables_.end()) return false;
        return it->second.state == OwnershipState::Owned && !it->second.hasActiveBorrows();
    }
    
    bool canBorrow(const std::string& name, BorrowKind kind) const {
        auto it = variables_.find(name);
        if (it == variables_.end()) return false;
        
        const auto& info = it->second;
        if (info.state == OwnershipState::Moved) return false;
        
        if (kind == BorrowKind::Mutable) {
            return !info.hasActiveBorrows();
        } else {
            return !info.hasActiveMutableBorrow();
        }
    }
    
    bool variableExists(const std::string& name) const {
        return variables_.count(name) > 0;
    }

private:
    std::unordered_map<std::string, OwnershipInfo> variables_;
    size_t currentScope_;
    
    OwnershipError makeError(OwnershipErrorKind kind, const std::string& name,
                               const SourceLocation& errLoc, const SourceLocation& relLoc) {
        OwnershipError err;
        err.kind = kind;
        err.variableName = name;
        err.errorLocation = errLoc;
        err.relatedLocation = relLoc;
        return err;
    }
};

} // namespace Semantic 
} // namespace Sad

// ═════════════════════════════════════════════════════════════
// إطار الاختبار / Test Fixture
// ═════════════════════════════════════════════════════════════

using namespace Sad::Semantic;

class BorrowCheckerTest : public ::testing::Test {
protected:
    OwnershipTracker tracker;
    
    void SetUp() override {
        tracker.enterScope();
    }
    
    void TearDown() override {
        tracker.exitScope();
    }
    
    // مساعدات / Helpers
    SourceLocation loc(size_t line) { return SourceLocation("test.ص", line, 1); }
    
    void declareVar(const std::string& name, size_t line, bool isCopy = false) {
        tracker.declareVariable(name, "T", loc(line), isCopy);
    }
    
    bool borrowShared(const std::string& owner, const std::string& borrower, size_t line) {
        auto err = tracker.createBorrow(owner, borrower, BorrowKind::Shared, loc(line));
        return !err.has_value();
    }
    
    bool borrowMut(const std::string& owner, const std::string& borrower, size_t line) {
        auto err = tracker.createBorrow(owner, borrower, BorrowKind::Mutable, loc(line));
        return !err.has_value();
    }
    
    OwnershipErrorKind getErrorKind(const std::string& owner, const std::string& borrower,
                                      BorrowKind kind, size_t line) {
        auto err = tracker.createBorrow(owner, borrower, kind, loc(line));
        EXPECT_TRUE(err.has_value());
        return err->kind;
    }
};

// ═══════════════════════════════════════════════════════
// الاختبارات الأساسية / Basic Tests
// ═══════════════════════════════════════════════════════

TEST_F(BorrowCheckerTest, DeclareVariable_Success) {
    declareVar("x", 1);
    EXPECT_TRUE(tracker.variableExists("x"));
}

TEST_F(BorrowCheckerTest, DuplicateDeclaration_Fails) {
    declareVar("x", 1);
    EXPECT_FALSE(tracker.declareVariable("x", "T", loc(2)));
}

TEST_F(BorrowCheckerTest, UseOwnedVariable_Success) {
    declareVar("x", 1);
    auto err = tracker.useVariable("x", loc(2));
    EXPECT_FALSE(err.has_value());
}

TEST_F(BorrowCheckerTest, OwnershipState_InitiallyOwned) {
    declareVar("x", 1);
    auto info = tracker.getOwnershipInfo("x");
    ASSERT_TRUE(info.has_value());
    EXPECT_EQ(info->state, OwnershipState::Owned);
}

// ═══════════════════════════════════════════════════════
// اختبارات الاستعارة المشتركة / Shared Borrow Tests (&x)
// ═══════════════════════════════════════════════════════

TEST_F(BorrowCheckerTest, SharedBorrow_Success) {
    declareVar("x", 1);
    EXPECT_TRUE(borrowShared("x", "&x", 2));
}

TEST_F(BorrowCheckerTest, MultipleSharedBorrows_Success) {
    declareVar("x", 1);
    EXPECT_TRUE(borrowShared("x", "&x1", 2));
    EXPECT_TRUE(borrowShared("x", "&x2", 3));
    EXPECT_TRUE(borrowShared("x", "&x3", 4));
}

TEST_F(BorrowCheckerTest, SharedBorrow_StateIsBorrowed) {
    declareVar("x", 1);
    borrowShared("x", "&x", 2);
    auto info = tracker.getOwnershipInfo("x");
    EXPECT_EQ(info->state, OwnershipState::Borrowed);
}

TEST_F(BorrowCheckerTest, SharedBorrow_CountsCorrect) {
    declareVar("x", 1);
    borrowShared("x", "&x1", 2);
    borrowShared("x", "&x2", 3);
    auto info = tracker.getOwnershipInfo("x");
    EXPECT_EQ(info->sharedBorrowCount(), 2u);
}

// ═══════════════════════════════════════════════════════
// اختبارات الاستعارة المتغيرة / Mutable Borrow Tests (&mut x / &متغير x)
// ═══════════════════════════════════════════════════════

TEST_F(BorrowCheckerTest, MutableBorrow_Success) {
    declareVar("x", 1);
    EXPECT_TRUE(borrowMut("x", "&mut_x", 2));
}

TEST_F(BorrowCheckerTest, MutableBorrow_StateIsBorrowedMut) {
    declareVar("x", 1);
    borrowMut("x", "&mut_x", 2);
    auto info = tracker.getOwnershipInfo("x");
    EXPECT_EQ(info->state, OwnershipState::BorrowedMut);
}

// ═══════════════════════════════════════════════════════
// اختبارات التعارض / Conflict Tests
// ═══════════════════════════════════════════════════════

TEST_F(BorrowCheckerTest, SharedAndMutBorrow_SameScope_Error) {
    // &x و &mut x في نفس النطاق ← خطأ
    declareVar("x", 1);
    borrowShared("x", "&x", 2);
    auto err = tracker.createBorrow("x", "&mut_x", BorrowKind::Mutable, loc(3));
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(err->kind, OwnershipErrorKind::MutBorrowWhileBorrow);
}

TEST_F(BorrowCheckerTest, MutBorrowAndShared_SameScope_Error) {
    declareVar("x", 1);
    borrowMut("x", "&mut_x", 2);
    auto err = tracker.createBorrow("x", "&x", BorrowKind::Shared, loc(3));
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(err->kind, OwnershipErrorKind::BorrowWhileMutBorrow);
}

TEST_F(BorrowCheckerTest, TwoMutableBorrows_SameScope_Error) {
    declareVar("x", 1);
    borrowMut("x", "&mut_x1", 2);
    auto err = tracker.createBorrow("x", "&mut_x2", BorrowKind::Mutable, loc(3));
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(err->kind, OwnershipErrorKind::MutBorrowConflict);
}

// ═══════════════════════════════════════════════════════
// اختبارات النقل / Move Tests
// ═══════════════════════════════════════════════════════

TEST_F(BorrowCheckerTest, Move_Success) {
    declareVar("x", 1);
    auto err = tracker.moveVariable("x", loc(2));
    EXPECT_FALSE(err.has_value());
}

TEST_F(BorrowCheckerTest, UseAfterMove_Error) {
    declareVar("x", 1);
    tracker.moveVariable("x", loc(2));
    auto err = tracker.useVariable("x", loc(3));
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(err->kind, OwnershipErrorKind::UseAfterMove);
}

TEST_F(BorrowCheckerTest, DoubleMove_Error) {
    declareVar("x", 1);
    tracker.moveVariable("x", loc(2));
    auto err = tracker.moveVariable("x", loc(3));
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(err->kind, OwnershipErrorKind::DoubleMove);
}

TEST_F(BorrowCheckerTest, BorrowAfterMove_Error) {
    declareVar("x", 1);
    tracker.moveVariable("x", loc(2));
    auto err = tracker.createBorrow("x", "&x", BorrowKind::Shared, loc(3));
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(err->kind, OwnershipErrorKind::BorrowOfMoved);
}

TEST_F(BorrowCheckerTest, MoveWhileBorrowed_Error) {
    declareVar("x", 1);
    borrowShared("x", "&x", 2);
    auto err = tracker.moveVariable("x", loc(3));
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(err->kind, OwnershipErrorKind::DropWhileBorrowed);
}

// ═══════════════════════════════════════════════════════
// اختبارات النسخ / Copy Type Tests
// ═══════════════════════════════════════════════════════

TEST_F(BorrowCheckerTest, CopyType_MoveDoesNotInvalidate) {
    declareVar("x", 1, /*isCopy=*/true);
    tracker.moveVariable("x", loc(2));
    auto err = tracker.useVariable("x", loc(3));
    // Copy types: move is actually a copy, so use after "move" is fine
    EXPECT_FALSE(err.has_value());
}

TEST_F(BorrowCheckerTest, CopyType_CanMoveTwice) {
    declareVar("x", 1, /*isCopy=*/true);
    auto err1 = tracker.moveVariable("x", loc(2));
    auto err2 = tracker.moveVariable("x", loc(3));
    EXPECT_FALSE(err1.has_value());
    EXPECT_FALSE(err2.has_value());
}

// ═══════════════════════════════════════════════════════
// اختبارات التعديل / Mutation Tests
// ═══════════════════════════════════════════════════════

TEST_F(BorrowCheckerTest, MutateOwned_Success) {
    declareVar("x", 1);
    auto err = tracker.mutateVariable("x", loc(2));
    EXPECT_FALSE(err.has_value());
}

TEST_F(BorrowCheckerTest, MutateWhileSharedBorrow_Error) {
    declareVar("x", 1);
    borrowShared("x", "&x", 2);
    auto err = tracker.mutateVariable("x", loc(3));
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(err->kind, OwnershipErrorKind::MutateWhileBorrowed);
}

TEST_F(BorrowCheckerTest, MutateAfterMove_Error) {
    declareVar("x", 1);
    tracker.moveVariable("x", loc(2));
    auto err = tracker.mutateVariable("x", loc(3));
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(err->kind, OwnershipErrorKind::UseAfterMove);
}

// ═══════════════════════════════════════════════════════
// اختبارات النطاقات / Scope Tests
// ═══════════════════════════════════════════════════════

TEST_F(BorrowCheckerTest, BorrowEndsWithScope) {
    declareVar("x", 1);
    
    // نطاق داخلي / Inner scope
    tracker.enterScope();
    borrowShared("x", "&x", 2);
    
    auto info1 = tracker.getOwnershipInfo("x");
    EXPECT_TRUE(info1->hasActiveBorrows());
    
    tracker.exitScope();
    
    auto info2 = tracker.getOwnershipInfo("x");
    EXPECT_FALSE(info2->hasActiveBorrows());
    EXPECT_EQ(info2->state, OwnershipState::Owned);
}

TEST_F(BorrowCheckerTest, MutBorrow_AfterScopeEnds_Success) {
    declareVar("x", 1);
    
    // استعارة مشتركة في نطاق داخلي / Shared borrow in inner scope
    tracker.enterScope();
    borrowShared("x", "&x", 2);
    tracker.exitScope();
    
    // الآن يمكن استعارة متغيرة / Now we can create mutable borrow
    EXPECT_TRUE(borrowMut("x", "&mut_x", 4));
}

TEST_F(BorrowCheckerTest, NestedScopes_BorrowIsolation) {
    declareVar("x", 1);
    
    tracker.enterScope();
    borrowShared("x", "&x_inner", 2);
    tracker.exitScope();
    
    tracker.enterScope();
    // يجب أن ينجح — الاستعارة السابقة انتهت
    EXPECT_TRUE(borrowMut("x", "&mut_x", 4));
    tracker.exitScope();
}

// ═══════════════════════════════════════════════════════
// اختبارات canMove و canBorrow / canMove & canBorrow Tests
// ═══════════════════════════════════════════════════════

TEST_F(BorrowCheckerTest, CanMove_OwnedVariable) {
    declareVar("x", 1);
    EXPECT_TRUE(tracker.canMove("x"));
}

TEST_F(BorrowCheckerTest, CannotMove_BorrowedVariable) {
    declareVar("x", 1);
    borrowShared("x", "&x", 2);
    EXPECT_FALSE(tracker.canMove("x"));
}

TEST_F(BorrowCheckerTest, CanBorrowShared_OwnedVariable) {
    declareVar("x", 1);
    EXPECT_TRUE(tracker.canBorrow("x", BorrowKind::Shared));
}

TEST_F(BorrowCheckerTest, CanBorrowMut_OwnedVariable) {
    declareVar("x", 1);
    EXPECT_TRUE(tracker.canBorrow("x", BorrowKind::Mutable));
}

TEST_F(BorrowCheckerTest, CannotBorrowMut_SharedBorrowed) {
    declareVar("x", 1);
    borrowShared("x", "&x", 2);
    EXPECT_FALSE(tracker.canBorrow("x", BorrowKind::Mutable));
}

TEST_F(BorrowCheckerTest, CannotBorrowShared_MutBorrowed) {
    declareVar("x", 1);
    borrowMut("x", "&mut_x", 2);
    EXPECT_FALSE(tracker.canBorrow("x", BorrowKind::Shared));
}

TEST_F(BorrowCheckerTest, CannotBorrow_MovedVariable) {
    declareVar("x", 1);
    tracker.moveVariable("x", loc(2));
    EXPECT_FALSE(tracker.canBorrow("x", BorrowKind::Shared));
    EXPECT_FALSE(tracker.canBorrow("x", BorrowKind::Mutable));
}

// ═══════════════════════════════════════════════════════
// اختبارات endBorrow / endBorrow Tests
// ═══════════════════════════════════════════════════════

TEST_F(BorrowCheckerTest, EndBorrow_RestoresOwned) {
    declareVar("x", 1);
    borrowShared("x", "&x", 2);
    tracker.endBorrow("x", "&x");
    
    auto info = tracker.getOwnershipInfo("x");
    EXPECT_EQ(info->state, OwnershipState::Owned);
    EXPECT_FALSE(info->hasActiveBorrows());
}

TEST_F(BorrowCheckerTest, EndOneBorrow_OtherRemains) {
    declareVar("x", 1);
    borrowShared("x", "&x1", 2);
    borrowShared("x", "&x2", 3);
    tracker.endBorrow("x", "&x1");
    
    auto info = tracker.getOwnershipInfo("x");
    EXPECT_TRUE(info->hasActiveBorrows());
    EXPECT_EQ(info->sharedBorrowCount(), 1u);
}

TEST_F(BorrowCheckerTest, EndMutBorrow_CanBorrowSharedAgain) {
    declareVar("x", 1);
    borrowMut("x", "&mut_x", 2);
    tracker.endBorrow("x", "&mut_x");
    
    EXPECT_TRUE(borrowShared("x", "&x", 4));
}

// ═══════════════════════════════════════════════════════
// اختبارات الأسماء العربية / Arabic Name Tests
// ═══════════════════════════════════════════════════════

TEST_F(BorrowCheckerTest, ArabicVariableNames_Work) {
    tracker.declareVariable(u8"متغير_عربي", u8"نوع", loc(1));
    EXPECT_TRUE(tracker.variableExists(u8"متغير_عربي"));
    
    EXPECT_TRUE(borrowShared(u8"متغير_عربي", u8"&مرجع", 2));
}

TEST_F(BorrowCheckerTest, ArabicNames_BorrowRulesApply) {
    tracker.declareVariable(u8"قائمة", u8"مصفوفة", loc(1));
    borrowShared(u8"قائمة", u8"&قائمة1", 2);
    
    auto err = tracker.createBorrow(u8"قائمة", u8"&متغير_قائمة", BorrowKind::Mutable, loc(3));
    EXPECT_TRUE(err.has_value());
    EXPECT_EQ(err->kind, OwnershipErrorKind::MutBorrowWhileBorrow);
}

// ═══════════════════════════════════════
// نقطة الدخول / Entry point
// ═══════════════════════════════════════

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
