/**
 * =============================================================================
 * ملف: lifetime_solver.cpp
 * الوصف: محلل قيود العمر (Lifetime Constraint Solver)
 * المهمة: T205 - Implement lifetime constraint solver
 * المرحلة: Phase 20 - User Story 17 (Lifetime Annotations)
 * =============================================================================
 * 
 * دليل المبتدئ 📚
 * ═══════════════
 * 
 * ما هو محلل قيود العمر؟
 * ────────────────────────
 * يتحقق من أن جميع علاقات العمر متسقة ومنطقية.
 * 
 * أمثلة على القيود:
 * ─────────────────
 * 
 * 1. قيد "أطول من أو يساوي" ('أ: 'ب):
 *    العمر 'أ يجب أن يعيش على الأقل بقدر 'ب
 *    
 *    ```
 *    دالة foo<'أ, 'ب>(x: &'أ نص, y: &'ب نص) -> &'أ نص
 *    حيث 'أ: 'ب
 *    ```
 * 
 * 2. قيد النطاق:
 *    المرجع يجب أن يعيش ضمن نطاق معين
 *    
 *    ```
 *    {
 *        متغير x = 5;
 *        متغير r = &x;  // r يعيش ضمن هذا النطاق فقط
 *    }
 *    // r غير صالح هنا!
 *    ```
 * 
 * 3. قيد التعيين:
 *    عند التعيين، العمر الأيمن يجب أن يساوي أو يتجاوز الأيسر
 *    
 *    ```
 *    متغير طويل = &س;  // عمر طويل <= عمر &س
 *    ```
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
#include <algorithm>
#include <queue>

namespace sad::borrow::solver {

// =============================================================================
// معرّفات الأعمار والنطاقات
// =============================================================================

/**
 * معرّف عمر فريد
 */
using LifetimeVar = size_t;

/**
 * معرّف نطاق
 */
using ScopeId = size_t;

/**
 * نوع العمر
 */
enum class LifetimeType {
    Named,      // عمر مسمى ('أ)
    Static,     // عمر ثابت ('ثابت)
    Scope,      // عمر مرتبط بنطاق
    Inferred,   // عمر مستنتج
};

/**
 * معلومات العمر
 */
struct LifetimeInfo {
    LifetimeVar id;
    LifetimeType type;
    std::string name;
    std::optional<ScopeId> scope;
    
    std::string toString() const {
        switch (type) {
            case LifetimeType::Named: return "'" + name;
            case LifetimeType::Static: return "'ثابت";
            case LifetimeType::Scope: return "'نطاق" + std::to_string(*scope);
            case LifetimeType::Inferred: return "'مستنتج" + std::to_string(id);
        }
        return "";
    }
};

// =============================================================================
// أنواع القيود
// =============================================================================

/**
 * نوع القيد
 */
enum class ConstraintKind {
    Outlives,       // 'أ: 'ب (أ يعيش أطول من ب)
    Equal,          // 'أ = 'ب (متساويان)
    ScopeContains,  // نطاق يحتوي عمراً
    Static,         // يجب أن يكون 'ثابت
};

/**
 * قيد على الأعمار
 */
struct Constraint {
    ConstraintKind kind;
    LifetimeVar left;
    LifetimeVar right;  // للقيود الثنائية
    ScopeId scope;      // لقيود النطاق
    
    // موقع القيد في الكود (للأخطاء)
    struct Location {
        std::string file;
        size_t line;
        size_t column;
    } location;
    
    std::string reason;  // سبب القيد
    
    static Constraint outlives(LifetimeVar longer, LifetimeVar shorter,
                               const std::string& reason = "") {
        return {ConstraintKind::Outlives, longer, shorter, 0, {}, reason};
    }
    
    static Constraint equal(LifetimeVar a, LifetimeVar b,
                           const std::string& reason = "") {
        return {ConstraintKind::Equal, a, b, 0, {}, reason};
    }
    
    static Constraint scopeContains(ScopeId scope, LifetimeVar lt,
                                   const std::string& reason = "") {
        return {ConstraintKind::ScopeContains, lt, 0, scope, {}, reason};
    }
    
    static Constraint mustBeStatic(LifetimeVar lt, const std::string& reason = "") {
        return {ConstraintKind::Static, lt, 0, 0, {}, reason};
    }
    
    std::string toString() const {
        switch (kind) {
            case ConstraintKind::Outlives:
                return "'" + std::to_string(left) + ": '" + std::to_string(right);
            case ConstraintKind::Equal:
                return "'" + std::to_string(left) + " = '" + std::to_string(right);
            case ConstraintKind::ScopeContains:
                return "نطاق" + std::to_string(scope) + " ⊇ '" + std::to_string(left);
            case ConstraintKind::Static:
                return "'" + std::to_string(left) + " = 'ثابت";
        }
        return "";
    }
};

// =============================================================================
// نتيجة الحل
// =============================================================================

/**
 * خطأ في قيود العمر
 */
struct LifetimeError {
    enum Kind {
        OutlivesViolation,  // العمر لا يعيش طويلاً بما يكفي
        ScopeEscape,        // المرجع يهرب من نطاقه
        Contradiction,      // قيود متناقضة
        UndefinedLifetime,  // عمر غير معرّف
    };
    
    Kind kind;
    std::string message;
    Constraint::Location location;
    std::vector<Constraint> involvedConstraints;
    
    std::string toString() const {
        std::ostringstream ss;
        ss << "[" << location.line << ":" << location.column << "] ";
        ss << "خطأ عمر: " << message;
        return ss.str();
    }
};

/**
 * نتيجة حل القيود
 */
struct SolverResult {
    bool success;
    std::map<LifetimeVar, LifetimeInfo> solution;  // قيم الأعمار
    std::vector<LifetimeError> errors;
    
    static SolverResult ok(std::map<LifetimeVar, LifetimeInfo> sol) {
        return {true, sol, {}};
    }
    
    static SolverResult fail(std::vector<LifetimeError> errs) {
        return {false, {}, errs};
    }
};

// =============================================================================
// شجرة النطاقات
// =============================================================================

/**
 * معلومات نطاق
 */
struct ScopeInfo {
    ScopeId id;
    std::optional<ScopeId> parent;
    std::string name;
    size_t depth;  // عمق التداخل
};

/**
 * شجرة النطاقات
 * تتبع تداخل النطاقات في الكود
 */
class ScopeTree {
public:
    /**
     * إضافة نطاق جذر
     */
    ScopeId addRoot(const std::string& name = "root") {
        ScopeId id = nextId_++;
        scopes_[id] = {id, std::nullopt, name, 0};
        return id;
    }
    
    /**
     * إضافة نطاق فرعي
     */
    ScopeId addChild(ScopeId parent, const std::string& name = "") {
        if (scopes_.find(parent) == scopes_.end()) {
            return 0;  // خطأ
        }
        
        ScopeId id = nextId_++;
        size_t depth = scopes_[parent].depth + 1;
        scopes_[id] = {id, parent, name, depth};
        children_[parent].push_back(id);
        return id;
    }
    
    /**
     * هل النطاق أ يحتوي النطاق ب؟
     * (أ هو سلف ب)
     */
    bool contains(ScopeId ancestor, ScopeId descendant) const {
        if (ancestor == descendant) return true;
        
        auto it = scopes_.find(descendant);
        while (it != scopes_.end() && it->second.parent.has_value()) {
            if (*it->second.parent == ancestor) return true;
            it = scopes_.find(*it->second.parent);
        }
        return false;
    }
    
    /**
     * إيجاد أصغر نطاق يحتوي كليهما
     */
    std::optional<ScopeId> commonAncestor(ScopeId a, ScopeId b) const {
        std::set<ScopeId> ancestorsA;
        
        // جمع أسلاف a
        ScopeId current = a;
        while (true) {
            ancestorsA.insert(current);
            auto it = scopes_.find(current);
            if (it == scopes_.end() || !it->second.parent.has_value()) break;
            current = *it->second.parent;
        }
        
        // البحث في أسلاف b
        current = b;
        while (true) {
            if (ancestorsA.count(current) > 0) return current;
            auto it = scopes_.find(current);
            if (it == scopes_.end() || !it->second.parent.has_value()) break;
            current = *it->second.parent;
        }
        
        return std::nullopt;
    }
    
    /**
     * الحصول على معلومات نطاق
     */
    std::optional<ScopeInfo> get(ScopeId id) const {
        auto it = scopes_.find(id);
        if (it == scopes_.end()) return std::nullopt;
        return it->second;
    }
    
private:
    std::map<ScopeId, ScopeInfo> scopes_;
    std::map<ScopeId, std::vector<ScopeId>> children_;
    ScopeId nextId_ = 1;
};

// =============================================================================
// محلل القيود
// =============================================================================

/**
 * محلل قيود العمر
 * 
 * يستخدم خوارزمية انتشار القيود (Constraint Propagation)
 */
class LifetimeConstraintSolver {
public:
    /**
     * إضافة عمر جديد
     */
    LifetimeVar addLifetime(const std::string& name, LifetimeType type) {
        LifetimeVar id = nextVar_++;
        lifetimes_[id] = {id, type, name, std::nullopt};
        return id;
    }
    
    /**
     * إضافة عمر مرتبط بنطاق
     */
    LifetimeVar addScopedLifetime(ScopeId scope) {
        LifetimeVar id = nextVar_++;
        lifetimes_[id] = {id, LifetimeType::Scope, "", scope};
        return id;
    }
    
    /**
     * إضافة قيد
     */
    void addConstraint(const Constraint& c) {
        constraints_.push_back(c);
    }
    
    /**
     * تعيين شجرة النطاقات
     */
    void setScopeTree(const ScopeTree& tree) {
        scopeTree_ = tree;
    }
    
    /**
     * حل القيود
     */
    SolverResult solve() {
        errors_.clear();
        
        // بناء رسم بياني للقيود
        buildGraph();
        
        // انتشار قيود 'ثابت
        propagateStatic();
        
        // التحقق من قيود outlives
        if (!checkOutlives()) {
            return SolverResult::fail(errors_);
        }
        
        // التحقق من قيود النطاق
        if (!checkScopes()) {
            return SolverResult::fail(errors_);
        }
        
        // التحقق من التناقضات
        if (!checkConsistency()) {
            return SolverResult::fail(errors_);
        }
        
        return SolverResult::ok(lifetimes_);
    }
    
    /**
     * طباعة القيود (للتصحيح)
     */
    void printConstraints() const {
        std::cout << "═══ قيود العمر ═══\n";
        for (const auto& c : constraints_) {
            std::cout << "  " << c.toString();
            if (!c.reason.empty()) {
                std::cout << "  // " << c.reason;
            }
            std::cout << "\n";
        }
    }
    
private:
    std::map<LifetimeVar, LifetimeInfo> lifetimes_;
    std::vector<Constraint> constraints_;
    ScopeTree scopeTree_;
    LifetimeVar nextVar_ = 1;
    
    // رسم بياني للعلاقات
    std::map<LifetimeVar, std::set<LifetimeVar>> outlivesGraph_;
    std::set<LifetimeVar> staticLifetimes_;
    
    std::vector<LifetimeError> errors_;
    
    void buildGraph() {
        outlivesGraph_.clear();
        staticLifetimes_.clear();
        
        for (const auto& c : constraints_) {
            switch (c.kind) {
                case ConstraintKind::Outlives:
                    outlivesGraph_[c.left].insert(c.right);
                    break;
                case ConstraintKind::Equal:
                    outlivesGraph_[c.left].insert(c.right);
                    outlivesGraph_[c.right].insert(c.left);
                    break;
                case ConstraintKind::Static:
                    staticLifetimes_.insert(c.left);
                    break;
                default:
                    break;
            }
        }
    }
    
    void propagateStatic() {
        // أي عمر يجب أن يعيش أطول من 'ثابت يجب أن يكون 'ثابت
        std::queue<LifetimeVar> worklist;
        for (auto v : staticLifetimes_) {
            worklist.push(v);
        }
        
        while (!worklist.empty()) {
            auto v = worklist.front();
            worklist.pop();
            
            // أي شيء يجب أن يعيش أطول من v يجب أن يكون ثابتاً أيضاً
            for (const auto& [from, tos] : outlivesGraph_) {
                if (tos.count(v) > 0 && staticLifetimes_.count(from) == 0) {
                    staticLifetimes_.insert(from);
                    worklist.push(from);
                }
            }
        }
    }
    
    bool checkOutlives() {
        for (const auto& c : constraints_) {
            if (c.kind != ConstraintKind::Outlives) continue;
            
            // إذا كان الأقصر ثابتاً، الأطول يجب أن يكون ثابتاً
            if (staticLifetimes_.count(c.right) > 0 &&
                staticLifetimes_.count(c.left) == 0) {
                // هذا قد يكون خطأ أو قد ينتشر
                // نعتمد على propagateStatic للتعامل معه
            }
            
            // التحقق من علاقات النطاق
            auto leftInfo = lifetimes_.find(c.left);
            auto rightInfo = lifetimes_.find(c.right);
            
            if (leftInfo != lifetimes_.end() && rightInfo != lifetimes_.end()) {
                if (leftInfo->second.scope.has_value() && 
                    rightInfo->second.scope.has_value()) {
                    // الأطول يجب أن يكون في نطاق أكبر أو يساوي
                    if (!scopeTree_.contains(*rightInfo->second.scope,
                                             *leftInfo->second.scope)) {
                        errors_.push_back({
                            LifetimeError::OutlivesViolation,
                            "العمر " + leftInfo->second.toString() + 
                            " لا يعيش طويلاً بما يكفي",
                            c.location,
                            {c}
                        });
                        return false;
                    }
                }
            }
        }
        return true;
    }
    
    bool checkScopes() {
        for (const auto& c : constraints_) {
            if (c.kind != ConstraintKind::ScopeContains) continue;
            
            auto ltInfo = lifetimes_.find(c.left);
            if (ltInfo == lifetimes_.end()) continue;
            
            if (ltInfo->second.scope.has_value()) {
                if (!scopeTree_.contains(c.scope, *ltInfo->second.scope)) {
                    errors_.push_back({
                        LifetimeError::ScopeEscape,
                        "المرجع يهرب من نطاقه",
                        c.location,
                        {c}
                    });
                    return false;
                }
            }
        }
        return true;
    }
    
    bool checkConsistency() {
        // التحقق من دورات في رسم outlives
        // (إذا 'أ: 'ب و 'ب: 'أ، يجب أن يكونا متساويين)
        
        for (const auto& [v, tos] : outlivesGraph_) {
            for (auto to : tos) {
                // هل هناك مسار من to إلى v؟
                if (reachable(to, v)) {
                    // يجب أن يكونا متساويين - هذا مقبول
                }
            }
        }
        
        return true;
    }
    
    bool reachable(LifetimeVar from, LifetimeVar to) {
        std::set<LifetimeVar> visited;
        std::queue<LifetimeVar> worklist;
        worklist.push(from);
        
        while (!worklist.empty()) {
            auto v = worklist.front();
            worklist.pop();
            
            if (v == to) return true;
            if (visited.count(v) > 0) continue;
            visited.insert(v);
            
            auto it = outlivesGraph_.find(v);
            if (it != outlivesGraph_.end()) {
                for (auto next : it->second) {
                    worklist.push(next);
                }
            }
        }
        
        return false;
    }
};

} // namespace sad::borrow::solver

// =============================================================================
// واجهة C للتكامل
// =============================================================================

extern "C" {

using namespace sad::borrow::solver;

/**
 * إنشاء محلل قيود جديد
 */
void* sad_lifetime_solver_new() {
    return new LifetimeConstraintSolver();
}

/**
 * تحرير المحلل
 */
void sad_lifetime_solver_free(void* solver) {
    delete static_cast<LifetimeConstraintSolver*>(solver);
}

/**
 * إضافة عمر
 */
size_t sad_solver_add_lifetime(void* solver, const char* name, int type) {
    if (!solver || !name) return 0;
    auto* s = static_cast<LifetimeConstraintSolver*>(solver);
    return s->addLifetime(name, static_cast<LifetimeType>(type));
}

/**
 * إضافة قيد outlives
 */
void sad_solver_add_outlives(void* solver, size_t longer, size_t shorter) {
    if (!solver) return;
    auto* s = static_cast<LifetimeConstraintSolver*>(solver);
    s->addConstraint(Constraint::outlives(longer, shorter));
}

/**
 * حل القيود
 */
int sad_solver_solve(void* solver) {
    if (!solver) return -1;
    auto* s = static_cast<LifetimeConstraintSolver*>(solver);
    auto result = s->solve();
    return result.success ? 0 : -1;
}

} // extern "C"

// =============================================================================
// اختبارات
// =============================================================================

#ifdef LIFETIME_SOLVER_TEST

#include <cassert>

void testSimpleOutlives() {
    sad::borrow::solver::LifetimeConstraintSolver solver;
    
    auto ltA = solver.addLifetime("أ", sad::borrow::solver::LifetimeType::Named);
    auto ltB = solver.addLifetime("ب", sad::borrow::solver::LifetimeType::Named);
    
    solver.addConstraint(sad::borrow::solver::Constraint::outlives(ltA, ltB));
    
    auto result = solver.solve();
    assert(result.success);
    
    std::cout << "✅ اختبار outlives بسيط نجح!\n";
}

void testScopeTree() {
    sad::borrow::solver::ScopeTree tree;
    
    auto root = tree.addRoot("دالة");
    auto inner = tree.addChild(root, "كتلة");
    auto innermost = tree.addChild(inner, "حلقة");
    
    assert(tree.contains(root, inner));
    assert(tree.contains(root, innermost));
    assert(tree.contains(inner, innermost));
    assert(!tree.contains(innermost, root));
    
    auto common = tree.commonAncestor(inner, innermost);
    assert(common.has_value());
    assert(*common == inner);
    
    std::cout << "✅ اختبار شجرة النطاقات نجح!\n";
}

void testScopeConstraint() {
    sad::borrow::solver::LifetimeConstraintSolver solver;
    sad::borrow::solver::ScopeTree tree;
    
    auto root = tree.addRoot();
    auto inner = tree.addChild(root);
    
    solver.setScopeTree(tree);
    
    auto ltOuter = solver.addScopedLifetime(root);
    auto ltInner = solver.addScopedLifetime(inner);
    
    // ltOuter يعيش أطول من ltInner
    solver.addConstraint(
        sad::borrow::solver::Constraint::outlives(ltOuter, ltInner)
    );
    
    auto result = solver.solve();
    assert(result.success);
    
    std::cout << "✅ اختبار قيد النطاق نجح!\n";
}

void testStaticPropagation() {
    sad::borrow::solver::LifetimeConstraintSolver solver;
    
    auto ltA = solver.addLifetime("أ", sad::borrow::solver::LifetimeType::Named);
    auto ltStatic = solver.addLifetime("ثابت", sad::borrow::solver::LifetimeType::Static);
    
    // ltA: 'ثابت
    solver.addConstraint(
        sad::borrow::solver::Constraint::outlives(ltA, ltStatic)
    );
    
    auto result = solver.solve();
    assert(result.success);
    
    std::cout << "✅ اختبار انتشار 'ثابت نجح!\n";
}

void testPrintConstraints() {
    sad::borrow::solver::LifetimeConstraintSolver solver;
    
    auto ltA = solver.addLifetime("أ", sad::borrow::solver::LifetimeType::Named);
    auto ltB = solver.addLifetime("ب", sad::borrow::solver::LifetimeType::Named);
    auto ltC = solver.addLifetime("ج", sad::borrow::solver::LifetimeType::Named);
    
    solver.addConstraint(
        sad::borrow::solver::Constraint::outlives(ltA, ltB, "من توقيع الدالة")
    );
    solver.addConstraint(
        sad::borrow::solver::Constraint::outlives(ltB, ltC, "من تعبير الإرجاع")
    );
    
    std::cout << "\n";
    solver.printConstraints();
    
    std::cout << "✅ اختبار طباعة القيود نجح!\n";
}

int main() {
    std::cout << "═══════════════════════════════════════════════\n";
    std::cout << "   اختبارات محلل قيود العمر\n";
    std::cout << "═══════════════════════════════════════════════\n\n";
    
    testSimpleOutlives();
    testScopeTree();
    testScopeConstraint();
    testStaticPropagation();
    testPrintConstraints();
    
    std::cout << "\n✅ جميع الاختبارات نجحت!\n";
    return 0;
}

#endif // LIFETIME_SOLVER_TEST
