/**
 * =============================================================================
 * ملف: borrow_lifetimes.cpp
 * الوصف: تكامل الأعمار مع مدقق الاستعارة
 * المهمة: T206 - Integrate lifetimes with borrow checker
 * المرحلة: Phase 20 - User Story 17 (Lifetime Annotations)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * كيف تعمل الأعمار مع مدقق الاستعارة؟
 * ────────────────────────────────────
 * 
 * مدقق الاستعارة يستخدم الأعمار لـ:
 * 1. التحقق من صلاحية المراجع
 * 2. منع المراجع المتدلية (dangling references)
 * 3. ضمان أمان الذاكرة
 * 
 * مثال على التكامل:
 * ─────────────────
 * 
 * ```
 * دالة خاطئة() -> &صحيح {
 *     متغير x = 5;
 *     أرجع &x;  // ❌ خطأ! x ستُدمر قبل استخدام المرجع
 * }
 * 
 * دالة صحيحة<'أ>(x: &'أ صحيح) -> &'أ صحيح {
 *     أرجع x;   // ✅ صحيح! المرجع المُرجع له نفس عمر المدخل
 * }
 * ```
 * 
 * القواعد الأساسية:
 * ─────────────────
 * 1. لا يمكن إرجاع مرجع لمتغير محلي
 * 2. المرجع المُرجع يجب أن يأتي من مدخل أو 'ثابت
 * 3. جميع المراجع في بنية يجب أن تعيش أطول من البنية
 * 
 * =============================================================================
 */

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <optional>
#include <sstream>
#include <iostream>
#include <functional>
#include <stack>

namespace sad::borrow::integration {

// =============================================================================
// هياكل البيانات الأساسية
// =============================================================================

/**
 * معرّف موقع في الكود
 */
using LocationId = size_t;

/**
 * معرّف متغير
 */
using VarId = size_t;

/**
 * معرّف عمر
 */
using LifetimeId = size_t;

/**
 * موقع في الكود
 */
struct CodeLocation {
    std::string file;
    size_t line;
    size_t column;
    
    std::string toString() const {
        std::ostringstream ss;
        if (!file.empty()) ss << file << ":";
        ss << line << ":" << column;
        return ss.str();
    }
};

/**
 * معلومات عمر
 */
struct Lifetime {
    LifetimeId id;
    std::string name;
    bool isStatic;
    bool isInferred;
    
    std::string toString() const {
        if (isStatic) return "'ثابت";
        if (name.empty()) return "'_" + std::to_string(id);
        return "'" + name;
    }
};

/**
 * نوع المرجع
 */
enum class RefKind {
    Shared,     // & - مشترك (للقراءة فقط)
    Mutable,    // &متغير - قابل للتغيير
    Raw,        // مؤشر خام (غير آمن)
};

/**
 * معلومات مرجع
 */
struct RefInfo {
    VarId sourceVar;
    RefKind kind;
    Lifetime lifetime;
    CodeLocation createdAt;
    bool isValid;
    
    std::string toString() const {
        std::ostringstream ss;
        ss << "&";
        ss << lifetime.toString() << " ";
        if (kind == RefKind::Mutable) ss << "متغير ";
        ss << "(من متغير " << sourceVar << ")";
        return ss.str();
    }
};

/**
 * معلومات متغير
 */
struct VarInfo {
    VarId id;
    std::string name;
    std::string type;
    Lifetime lifetime;
    CodeLocation declaredAt;
    bool isOwned;         // هل يملك البيانات؟
    bool isBorrowed;      // هل هناك استعارة نشطة؟
    bool isMutBorrowed;   // هل هناك استعارة قابلة للتغيير؟
    std::vector<RefInfo> activeRefs;  // المراجع النشطة
};

// =============================================================================
// سياق الاستعارة
// =============================================================================

/**
 * سياق الاستعارة لنطاق معين
 */
class BorrowContext {
public:
    /**
     * إضافة متغير جديد
     */
    VarId addVariable(const std::string& name, const std::string& type,
                     const Lifetime& lifetime, const CodeLocation& loc) {
        VarId id = nextVarId_++;
        variables_[id] = {
            id, name, type, lifetime, loc,
            true, false, false, {}
        };
        nameToId_[name] = id;
        return id;
    }
    
    /**
     * استعارة متغير (مشتركة)
     */
    std::optional<RefInfo> borrowShared(VarId varId, const Lifetime& lifetime,
                                        const CodeLocation& loc) {
        auto it = variables_.find(varId);
        if (it == variables_.end()) return std::nullopt;
        
        auto& var = it->second;
        
        // لا يمكن استعارة مشتركة إذا كانت هناك استعارة قابلة للتغيير
        if (var.isMutBorrowed) {
            lastError_ = "لا يمكن استعارة " + var.name + 
                        " لأنها مستعارة بشكل قابل للتغيير";
            return std::nullopt;
        }
        
        RefInfo ref = {
            varId, RefKind::Shared, lifetime, loc, true
        };
        
        var.isBorrowed = true;
        var.activeRefs.push_back(ref);
        
        return ref;
    }
    
    /**
     * استعارة متغير (قابلة للتغيير)
     */
    std::optional<RefInfo> borrowMutable(VarId varId, const Lifetime& lifetime,
                                         const CodeLocation& loc) {
        auto it = variables_.find(varId);
        if (it == variables_.end()) return std::nullopt;
        
        auto& var = it->second;
        
        // لا يمكن استعارة قابلة للتغيير إذا كانت هناك أي استعارة
        if (var.isBorrowed || var.isMutBorrowed) {
            lastError_ = "لا يمكن استعارة " + var.name + 
                        " بشكل قابل للتغيير لأنها مستعارة بالفعل";
            return std::nullopt;
        }
        
        RefInfo ref = {
            varId, RefKind::Mutable, lifetime, loc, true
        };
        
        var.isBorrowed = true;
        var.isMutBorrowed = true;
        var.activeRefs.push_back(ref);
        
        return ref;
    }
    
    /**
     * إنهاء جميع الاستعارات لمتغير
     */
    void endBorrows(VarId varId) {
        auto it = variables_.find(varId);
        if (it != variables_.end()) {
            it->second.isBorrowed = false;
            it->second.isMutBorrowed = false;
            it->second.activeRefs.clear();
        }
    }
    
    /**
     * إنهاء استعارات منتهية العمر
     */
    void endExpiredBorrows(const Lifetime& currentLifetime) {
        for (auto& [id, var] : variables_) {
            var.activeRefs.erase(
                std::remove_if(var.activeRefs.begin(), var.activeRefs.end(),
                    [&](const RefInfo& ref) {
                        return !ref.lifetime.isStatic && 
                               ref.lifetime.id < currentLifetime.id;
                    }),
                var.activeRefs.end()
            );
            
            if (var.activeRefs.empty()) {
                var.isBorrowed = false;
                var.isMutBorrowed = false;
            }
        }
    }
    
    /**
     * البحث عن متغير بالاسم
     */
    std::optional<VarId> findByName(const std::string& name) const {
        auto it = nameToId_.find(name);
        if (it == nameToId_.end()) return std::nullopt;
        return it->second;
    }
    
    /**
     * الحصول على معلومات متغير
     */
    std::optional<VarInfo> getVar(VarId id) const {
        auto it = variables_.find(id);
        if (it == variables_.end()) return std::nullopt;
        return it->second;
    }
    
    /**
     * الحصول على آخر خطأ
     */
    std::string lastError() const { return lastError_; }
    
private:
    std::map<VarId, VarInfo> variables_;
    std::map<std::string, VarId> nameToId_;
    VarId nextVarId_ = 1;
    std::string lastError_;
};

// =============================================================================
// مدقق الاستعارة المتكامل مع الأعمار
// =============================================================================

/**
 * خطأ استعارة
 */
struct BorrowError {
    enum Kind {
        DanglingReference,      // مرجع متدلي
        UseAfterFree,          // استخدام بعد التحرير
        InvalidBorrow,         // استعارة غير صالحة
        LifetimeViolation,     // انتهاك عمر
        MovedValue,            // قيمة منقولة
    };
    
    Kind kind;
    std::string message;
    CodeLocation location;
    std::string hint;
    
    std::string toString() const {
        std::ostringstream ss;
        ss << "خطأ استعارة [" << location.toString() << "]: " << message;
        if (!hint.empty()) {
            ss << "\n  تلميح: " << hint;
        }
        return ss.str();
    }
};

/**
 * مدقق الاستعارة المتكامل مع الأعمار
 */
class LifetimeAwareBorrowChecker {
public:
    /**
     * دخول نطاق جديد
     */
    void enterScope(const std::string& name = "") {
        Lifetime scopeLifetime = {nextLifetimeId_++, name, false, true};
        scopeStack_.push(scopeLifetime);
        contexts_.push_back(std::make_unique<BorrowContext>());
    }
    
    /**
     * خروج من النطاق
     */
    void exitScope() {
        if (scopeStack_.empty()) return;
        
        // التحقق من المراجع التي تهرب من النطاق
        checkEscapingReferences();
        
        scopeStack_.pop();
        if (!contexts_.empty()) {
            contexts_.pop_back();
        }
    }
    
    /**
     * تعريف متغير
     */
    VarId declareVariable(const std::string& name, const std::string& type,
                         const CodeLocation& loc) {
        if (contexts_.empty()) {
            enterScope("implicit");
        }
        
        Lifetime lifetime = scopeStack_.empty() ? 
            Lifetime{0, "global", false, false} : scopeStack_.top();
        
        return contexts_.back()->addVariable(name, type, lifetime, loc);
    }
    
    /**
     * استعارة مرجع مشترك
     */
    bool borrowShared(const std::string& varName, const CodeLocation& loc) {
        auto varId = findVariable(varName);
        if (!varId.has_value()) {
            errors_.push_back({
                BorrowError::InvalidBorrow,
                "متغير غير موجود: " + varName,
                loc,
                "تأكد من تعريف المتغير قبل استخدامه"
            });
            return false;
        }
        
        Lifetime borrowLifetime = scopeStack_.empty() ?
            Lifetime{0, "", true, false} : scopeStack_.top();
        
        auto ref = contexts_.back()->borrowShared(*varId, borrowLifetime, loc);
        if (!ref.has_value()) {
            errors_.push_back({
                BorrowError::InvalidBorrow,
                contexts_.back()->lastError(),
                loc,
                "لا يمكن وجود استعارة مشتركة واستعارة قابلة للتغيير في نفس الوقت"
            });
            return false;
        }
        
        return true;
    }
    
    /**
     * استعارة مرجع قابل للتغيير
     */
    bool borrowMutable(const std::string& varName, const CodeLocation& loc) {
        auto varId = findVariable(varName);
        if (!varId.has_value()) {
            errors_.push_back({
                BorrowError::InvalidBorrow,
                "متغير غير موجود: " + varName,
                loc,
                "تأكد من تعريف المتغير قبل استخدامه"
            });
            return false;
        }
        
        Lifetime borrowLifetime = scopeStack_.empty() ?
            Lifetime{0, "", true, false} : scopeStack_.top();
        
        auto ref = contexts_.back()->borrowMutable(*varId, borrowLifetime, loc);
        if (!ref.has_value()) {
            errors_.push_back({
                BorrowError::InvalidBorrow,
                contexts_.back()->lastError(),
                loc,
                "يمكن وجود استعارة قابلة للتغيير واحدة فقط في كل مرة"
            });
            return false;
        }
        
        return true;
    }
    
    /**
     * التحقق من إرجاع مرجع
     * 
     * يتحقق من أن المرجع المُرجع صالح (لا يشير لمتغير محلي)
     */
    bool checkReturnRef(const std::string& refExpr, 
                       const std::optional<Lifetime>& expectedLifetime,
                       const CodeLocation& loc) {
        // تحليل تعبير المرجع
        // في هذا التنفيذ البسيط، نفترض أن التعبير هو اسم متغير
        
        auto varId = findVariable(refExpr);
        if (!varId.has_value()) {
            // قد يكون تعبيراً معقداً - نحتاج تحليلاً أعمق
            return true;
        }
        
        // البحث عن المتغير
        for (auto it = contexts_.rbegin(); it != contexts_.rend(); ++it) {
            auto varInfo = (*it)->getVar(*varId);
            if (varInfo.has_value()) {
                // التحقق من أن عمر المتغير يتوافق مع المتوقع
                if (expectedLifetime.has_value()) {
                    if (!expectedLifetime->isStatic && 
                        varInfo->lifetime.id >= expectedLifetime->id) {
                        errors_.push_back({
                            BorrowError::DanglingReference,
                            "محاولة إرجاع مرجع لمتغير محلي: " + varInfo->name,
                            loc,
                            "المتغيرات المحلية تُدمر عند نهاية النطاق"
                        });
                        return false;
                    }
                }
                break;
            }
        }
        
        return true;
    }
    
    /**
     * الحصول على الأخطاء
     */
    const std::vector<BorrowError>& errors() const { return errors_; }
    
    /**
     * هل هناك أخطاء؟
     */
    bool hasErrors() const { return !errors_.empty(); }
    
    /**
     * مسح الأخطاء
     */
    void clearErrors() { errors_.clear(); }
    
    /**
     * طباعة الأخطاء
     */
    void printErrors() const {
        for (const auto& err : errors_) {
            std::cout << err.toString() << "\n\n";
        }
    }
    
private:
    std::vector<std::unique_ptr<BorrowContext>> contexts_;
    std::stack<Lifetime> scopeStack_;
    std::vector<BorrowError> errors_;
    LifetimeId nextLifetimeId_ = 1;
    
    std::optional<VarId> findVariable(const std::string& name) {
        for (auto it = contexts_.rbegin(); it != contexts_.rend(); ++it) {
            auto id = (*it)->findByName(name);
            if (id.has_value()) return id;
        }
        return std::nullopt;
    }
    
    void checkEscapingReferences() {
        // التحقق من أنه لا توجد مراجع تشير لمتغيرات في هذا النطاق
        // من نطاقات خارجية
        
        if (contexts_.empty()) return;
        
        // هذا تنفيذ مبسط - في الواقع نحتاج تحليلاً أكثر تعقيداً
    }
};

// =============================================================================
// محلل التدفق مع الأعمار
// =============================================================================

/**
 * يحلل تدفق البيانات مع معلومات الأعمار
 */
class LifetimeFlowAnalyzer {
public:
    /**
     * تحليل دالة
     */
    void analyzeFunction(const std::string& name,
                        const std::vector<std::pair<std::string, Lifetime>>& params,
                        const std::optional<Lifetime>& returnLifetime) {
        functionName_ = name;
        paramLifetimes_ = params;
        returnLifetime_ = returnLifetime;
    }
    
    /**
     * التحقق من تعبير إرجاع
     */
    bool checkReturn(const std::string& expr, const Lifetime& exprLifetime) {
        if (!returnLifetime_.has_value()) {
            return true;  // لا يوجد نوع إرجاع مرجعي
        }
        
        // التحقق من أن عمر التعبير يتوافق مع عمر الإرجاع
        if (returnLifetime_->isStatic && !exprLifetime.isStatic) {
            errors_.push_back("يُتوقع عمر 'ثابت لكن العمر أقصر");
            return false;
        }
        
        // البحث عن العمر في المعاملات
        bool found = false;
        for (const auto& [name, lt] : paramLifetimes_) {
            if (lt.name == exprLifetime.name || lt.id == exprLifetime.id) {
                found = true;
                break;
            }
        }
        
        if (!found && !exprLifetime.isStatic) {
            errors_.push_back("عمر التعبير لا يتطابق مع أي معامل");
            return false;
        }
        
        return true;
    }
    
    const std::vector<std::string>& errors() const { return errors_; }
    
private:
    std::string functionName_;
    std::vector<std::pair<std::string, Lifetime>> paramLifetimes_;
    std::optional<Lifetime> returnLifetime_;
    std::vector<std::string> errors_;
};

} // namespace sad::borrow::integration

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

using namespace sad::borrow::integration;

/**
 * إنشاء مدقق استعارة جديد
 */
void* sad_borrow_checker_new() {
    return new LifetimeAwareBorrowChecker();
}

/**
 * تحرير المدقق
 */
void sad_borrow_checker_free(void* checker) {
    delete static_cast<LifetimeAwareBorrowChecker*>(checker);
}

/**
 * دخول نطاق
 */
void sad_borrow_enter_scope(void* checker, const char* name) {
    if (!checker) return;
    auto* c = static_cast<LifetimeAwareBorrowChecker*>(checker);
    c->enterScope(name ? name : "");
}

/**
 * خروج من نطاق
 */
void sad_borrow_exit_scope(void* checker) {
    if (!checker) return;
    auto* c = static_cast<LifetimeAwareBorrowChecker*>(checker);
    c->exitScope();
}

/**
 * تعريف متغير
 */
size_t sad_borrow_declare_var(void* checker, const char* name, 
                              const char* type, size_t line, size_t col) {
    if (!checker || !name || !type) return 0;
    auto* c = static_cast<LifetimeAwareBorrowChecker*>(checker);
    return c->declareVariable(name, type, {"", line, col});
}

/**
 * استعارة مشتركة
 */
int sad_borrow_shared(void* checker, const char* name, size_t line, size_t col) {
    if (!checker || !name) return -1;
    auto* c = static_cast<LifetimeAwareBorrowChecker*>(checker);
    return c->borrowShared(name, {"", line, col}) ? 0 : -1;
}

/**
 * استعارة قابلة للتغيير
 */
int sad_borrow_mutable(void* checker, const char* name, size_t line, size_t col) {
    if (!checker || !name) return -1;
    auto* c = static_cast<LifetimeAwareBorrowChecker*>(checker);
    return c->borrowMutable(name, {"", line, col}) ? 0 : -1;
}

/**
 * عدد الأخطاء
 */
int sad_borrow_error_count(void* checker) {
    if (!checker) return 0;
    auto* c = static_cast<LifetimeAwareBorrowChecker*>(checker);
    return static_cast<int>(c->errors().size());
}

/**
 * طباعة الأخطاء
 */
void sad_borrow_print_errors(void* checker) {
    if (!checker) return;
    auto* c = static_cast<LifetimeAwareBorrowChecker*>(checker);
    c->printErrors();
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef BORROW_LIFETIMES_TEST

#include <cassert>

void testBasicBorrow() {
    sad::borrow::integration::LifetimeAwareBorrowChecker checker;
    
    checker.enterScope("main");
    checker.declareVariable("x", "صحيح", {"", 1, 1});
    
    // استعارة مشتركة - يجب أن تنجح
    assert(checker.borrowShared("x", {"", 2, 1}));
    assert(!checker.hasErrors());
    
    checker.exitScope();
    
    std::cout << "✅ اختبار الاستعارة الأساسية نجح!\n";
}

void testMultipleSharedBorrows() {
    sad::borrow::integration::LifetimeAwareBorrowChecker checker;
    
    checker.enterScope("main");
    checker.declareVariable("x", "نص", {"", 1, 1});
    
    // استعارات مشتركة متعددة - يجب أن تنجح
    assert(checker.borrowShared("x", {"", 2, 1}));
    assert(checker.borrowShared("x", {"", 3, 1}));
    assert(checker.borrowShared("x", {"", 4, 1}));
    assert(!checker.hasErrors());
    
    checker.exitScope();
    
    std::cout << "✅ اختبار الاستعارات المشتركة المتعددة نجح!\n";
}

void testMutableBorrowConflict() {
    sad::borrow::integration::LifetimeAwareBorrowChecker checker;
    
    checker.enterScope("main");
    checker.declareVariable("x", "صحيح", {"", 1, 1});
    
    // استعارة قابلة للتغيير
    assert(checker.borrowMutable("x", {"", 2, 1}));
    
    // استعارة ثانية - يجب أن تفشل
    assert(!checker.borrowMutable("x", {"", 3, 1}));
    assert(checker.hasErrors());
    
    checker.exitScope();
    
    std::cout << "✅ اختبار تعارض الاستعارة القابلة للتغيير نجح!\n";
}

void testSharedAfterMutable() {
    sad::borrow::integration::LifetimeAwareBorrowChecker checker;
    
    checker.enterScope("main");
    checker.declareVariable("x", "صحيح", {"", 1, 1});
    
    // استعارة قابلة للتغيير
    assert(checker.borrowMutable("x", {"", 2, 1}));
    
    // استعارة مشتركة بعد قابلة للتغيير - يجب أن تفشل
    assert(!checker.borrowShared("x", {"", 3, 1}));
    
    checker.exitScope();
    
    std::cout << "✅ اختبار الاستعارة المشتركة بعد القابلة للتغيير نجح!\n";
}

void testNestedScopes() {
    sad::borrow::integration::LifetimeAwareBorrowChecker checker;
    
    checker.enterScope("outer");
    checker.declareVariable("x", "صحيح", {"", 1, 1});
    
    checker.enterScope("inner");
    // يجب أن نتمكن من الوصول لـ x من النطاق الخارجي
    assert(checker.borrowShared("x", {"", 2, 1}));
    checker.exitScope();
    
    checker.exitScope();
    
    std::cout << "✅ اختبار النطاقات المتداخلة نجح!\n";
}

void testUndefinedVariable() {
    sad::borrow::integration::LifetimeAwareBorrowChecker checker;
    
    checker.enterScope("main");
    
    // استعارة متغير غير موجود
    assert(!checker.borrowShared("undefined", {"", 1, 1}));
    assert(checker.hasErrors());
    
    checker.exitScope();
    
    std::cout << "✅ اختبار المتغير غير المعرّف نجح!\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات تكامل الأعمار مع مدقق الاستعارة\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testBasicBorrow();
    testMultipleSharedBorrows();
    testMutableBorrowConflict();
    testSharedAfterMutable();
    testNestedScopes();
    testUndefinedVariable();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // BORROW_LIFETIMES_TEST
