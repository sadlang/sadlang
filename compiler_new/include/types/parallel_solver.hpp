// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file parallel_solver.hpp
 * @brief حلال القيود المتوازي - تحسين الأداء بالتوازي
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *              حلال القيود المتوازي في لغة ص (Parallel Constraint Solver)
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * هذا الملف يحتوي على تنفيذ محسّن لحلال القيود يستخدم التوازي
 * لتحسين الأداء في المشاريع الكبيرة.
 * 
 * ═══════════════════════════════════════════════════════════════════════════
 *                    استراتيجية التوازي
 * ═══════════════════════════════════════════════════════════════════════════
 * 
 * 1. تقسيم القيود:
 *    - القيود المستقلة تُحل بالتوازي
 *    - القيود المترابطة تُحل تسلسلياً
 * 
 * 2. تحليل التبعيات:
 *    - بناء رسم بياني للتبعيات بين القيود
 *    - تحديد المجموعات المستقلة
 * 
 * 3. دمج النتائج:
 *    - دمج الاستبدالات الجزئية
 *    - التحقق من عدم التعارض
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#ifndef SAD_TYPES_PARALLEL_SOLVER_HPP
#define SAD_TYPES_PARALLEL_SOLVER_HPP

#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <future>
#include <atomic>
#include <unordered_set>
#include <functional>

namespace Sad {
namespace TypeSystem {

// التصريحات المسبقة
class Constraint;
class Substitution;
struct SolverResult;

// ═══════════════════════════════════════════════════════════════════════════
//                    إعدادات التوازي / Parallelism Settings
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct ParallelSolverConfig
 * @brief إعدادات حلال القيود المتوازي
 */
struct ParallelSolverConfig {
    size_t maxThreads = 0;              // 0 = عدد الأنوية المتاحة
    size_t minConstraintsPerThread = 10; // الحد الأدنى للقيود لكل خيط
    size_t maxIterationsPerChunk = 100; // الحد الأقصى للتكرارات لكل جزء
    bool enableWorkStealing = true;     // تفعيل سرقة العمل
    bool verbose = false;               // طباعة معلومات التنقيح
    
    ParallelSolverConfig() {
        maxThreads = std::thread::hardware_concurrency();
        if (maxThreads == 0) maxThreads = 4;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    مجموعة قيود مستقلة / Independent Constraint Group
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct ConstraintGroup
 * @brief مجموعة من القيود المستقلة يمكن حلها بالتوازي
 */
struct ConstraintGroup {
    std::vector<size_t> constraintIndices;  // فهارس القيود في المجموعة
    std::unordered_set<size_t> typeVariables; // متغيرات النوع المتأثرة
    size_t priority = 0;                     // أولوية المجموعة
    
    bool isIndependentFrom(const ConstraintGroup& other) const {
        // مجموعتان مستقلتان إذا لم تتشاركا في متغيرات النوع
        for (size_t var : typeVariables) {
            if (other.typeVariables.count(var) > 0) {
                return false;
            }
        }
        return true;
    }
    
    void merge(const ConstraintGroup& other) {
        for (size_t idx : other.constraintIndices) {
            constraintIndices.push_back(idx);
        }
        for (size_t var : other.typeVariables) {
            typeVariables.insert(var);
        }
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    نتيجة الحل المتوازي / Parallel Solve Result
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @struct ParallelSolveResult
 * @brief نتيجة الحل المتوازي
 */
struct ParallelSolveResult {
    bool success = false;
    std::shared_ptr<Substitution> substitution;
    std::vector<std::string> errors;
    
    // إحصائيات الأداء
    size_t totalConstraints = 0;
    size_t constraintGroups = 0;
    size_t threadsUsed = 0;
    double solveTimeMs = 0.0;
    
    static ParallelSolveResult makeSuccess(std::shared_ptr<Substitution> subst) {
        ParallelSolveResult result;
        result.success = true;
        result.substitution = subst;
        return result;
    }
    
    static ParallelSolveResult makeFailure(const std::string& error) {
        ParallelSolveResult result;
        result.success = false;
        result.errors.push_back(error);
        return result;
    }
};

// ═══════════════════════════════════════════════════════════════════════════
//                    حلال القيود المتوازي / Parallel Constraint Solver
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @class ParallelConstraintSolver
 * @brief حلال القيود المتوازي
 */
class ParallelConstraintSolver {
public:
    explicit ParallelConstraintSolver(const ParallelSolverConfig& config = ParallelSolverConfig());
    ~ParallelConstraintSolver();
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الحل المتوازي / Parallel Solving
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief حل مجموعة قيود بالتوازي
     * @param constraints القيود للحل
     * @return نتيجة الحل
     */
    ParallelSolveResult solve(const std::vector<Constraint>& constraints);
    
    /**
     * @brief إلغاء الحل الجاري
     */
    void cancel();
    
    /**
     * @brief هل الحل جارٍ؟
     */
    bool isSolving() const { return solving_.load(); }
    
    // ══════════════════════════════════════════════════════════════════════
    //                    تحليل التبعيات / Dependency Analysis
    // ══════════════════════════════════════════════════════════════════════
    
    /**
     * @brief تقسيم القيود إلى مجموعات مستقلة
     */
    std::vector<ConstraintGroup> partitionConstraints(
        const std::vector<Constraint>& constraints);
    
    /**
     * @brief الحصول على متغيرات النوع في قيد
     */
    std::unordered_set<size_t> getTypeVariables(const Constraint& constraint);
    
    // ══════════════════════════════════════════════════════════════════════
    //                    الإعدادات / Configuration
    // ══════════════════════════════════════════════════════════════════════
    
    void setConfig(const ParallelSolverConfig& config) { config_ = config; }
    const ParallelSolverConfig& getConfig() const { return config_; }

private:
    ParallelSolverConfig config_;
    std::atomic<bool> solving_{false};
    std::atomic<bool> cancelled_{false};
    mutable std::mutex mutex_;
    
    /**
     * @brief حل مجموعة قيود في خيط منفصل
     */
    SolverResult solveGroup(const std::vector<Constraint>& constraints,
                           const std::vector<size_t>& indices,
                           const Substitution& initialSubst);
    
    /**
     * @brief دمج استبدالين
     */
    std::shared_ptr<Substitution> mergeSubstitutions(
        const std::vector<std::shared_ptr<Substitution>>& substitutions);
    
    /**
     * @brief التحقق من عدم التعارض
     */
    bool checkConflicts(const std::vector<std::shared_ptr<Substitution>>& substitutions);
};

// ═══════════════════════════════════════════════════════════════════════════
//                    دوال مساعدة / Helper Functions
// ═══════════════════════════════════════════════════════════════════════════

/**
 * @brief حل القيود بالتوازي (دالة مختصرة)
 */
inline ParallelSolveResult solveConstraintsParallel(
    const std::vector<Constraint>& constraints,
    const ParallelSolverConfig& config = ParallelSolverConfig()) {
    ParallelConstraintSolver solver(config);
    return solver.solve(constraints);
}

/**
 * @brief تقدير ما إذا كان الحل المتوازي مفيداً
 */
inline bool shouldUseParallelSolver(size_t constraintCount, size_t typeVarCount) {
    // استخدم الحل المتوازي للمشاريع الكبيرة فقط
    return constraintCount > 50 && typeVarCount > 20;
}

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_TYPES_PARALLEL_SOLVER_HPP
