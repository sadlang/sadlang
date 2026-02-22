// ════════════════════════════════════════════════════════════════════════════════
// ملف: constraint_solver.h
// File: constraint_solver.h
//
// الوصف: حلال القيود (Constraint Solver) لنظام استنتاج الأنواع
// Description: Constraint solver for type inference system
//
// المؤلف: Sad Language Type System - Type Inference Engine
// Author: Sad Language Type System - Type Inference Engine
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// الخوارزمية: Iterative constraint solving with unification
// Algorithm: Iterative constraint solving with unification
//
// الغرض: حل مجموعة من القيود للحصول على استبدال موحد
// Purpose: Solve a set of constraints to obtain unified substitution
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_CONSTRAINT_SOLVER_H
#define SAD_COMPILER_TYPE_SYSTEM_CONSTRAINT_SOLVER_H

#include "type.h"              // استيراد النوع الأساسي / Import base Type (type.h:1-218)
#include "constraint.h"         // استيراد القيود / Import Constraint
#include "unification.h"        // استيراد التوحيد / Import Unification
#include "substitution.h"       // استيراد الاستبدال / Import Substitution
#include <vector>              // لـ القوائم / For lists
#include <string>              // لـ الرسائل / For messages

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تعداد أخطاء الحل / Solver Error Enumeration
// ════════════════════════════════════════════════════════════════════════════════

enum class SolverErrorKind {
    // لا يوجد خطأ / No error
    None,
    
    // فشل التوحيد / Unification failed
    UnificationFailed,
    
    // قيود متناقضة / Conflicting constraints
    ConflictingConstraints,
    
    // حلقة لا نهائية في الاستبدال / Infinite loop in substitution
    InfiniteSubstitution,
    
    // قيد غير قابل للحل / Unsolvable constraint
    UnsolvableConstraint,
    
    // timeout (عدد كبير جداً من التكرارات)
    Timeout
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: SolverError
// Class: SolverError
//
// الوصف: خطأ حدث أثناء حل القيود / Error that occurred during constraint solving
// يحتوي على معلومات تفصيلية للتشخيص / Contains detailed information for diagnosis
// ════════════════════════════════════════════════════════════════════════════════

class SolverError {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ كامل / Full constructor
    // المعامل / Parameter: kind - نوع الخطأ / Error kind
    // المعامل / Parameter: constraint - القيد الذي فشل / Failed constraint
    // المعامل / Parameter: message - رسالة الخطأ / Error message
    SolverError(SolverErrorKind kind,
               const Constraint& constraint,
               const std::string& message);
    
    // منشئ افتراضي - لا يوجد خطأ / Default constructor - no error
    SolverError();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الدوال العامة / Public Methods
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على نوع الخطأ / Get error kind
    SolverErrorKind getKind() const { return kind_; }
    
    // الحصول على القيد الذي فشل / Get failed constraint
    const Constraint& getConstraint() const { return constraint_; }
    
    // الحصول على رسالة الخطأ / Get error message
    const std::string& getMessage() const { return message_; }
    
    // هل يوجد خطأ؟ / Is there an error?
    bool hasError() const { return kind_ != SolverErrorKind::None; }
    
    // تحويل إلى نص / Convert to string
    std::string toString() const;
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    SolverErrorKind kind_;       // نوع الخطأ / Error kind
    Constraint constraint_;      // القيد الذي فشل / Failed constraint
    std::string message_;        // رسالة الخطأ / Error message
};

// ════════════════════════════════════════════════════════════════════════════════
// نوع النتيجة / Result Type
// ════════════════════════════════════════════════════════════════════════════════

// نتيجة الحل: إما Substitution أو SolverError
// Solver result: either Substitution or SolverError
struct SolverResult {
    // هل نجح الحل؟ / Did solving succeed?
    bool success;
    
    // الاستبدال الناتج (إذا نجح) / Resulting substitution (if succeeded)
    Substitution substitution;
    
    // الخطأ (إذا فشل) / Error (if failed)
    SolverError error;
    
    // عدد التكرارات المُستخدمة / Number of iterations used
    int iterations;
    
    // منشئ لنتيجة ناجحة / Constructor for successful result
    static SolverResult makeSuccess(const Substitution& subst, int iterations);
    
    // منشئ لنتيجة فاشلة / Constructor for failed result
    static SolverResult makeFailure(const SolverError& err);
};

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: ConstraintSolver
// Class: ConstraintSolver
//
// الوصف: حلال القيود الرئيسي / Main constraint solver
// يحل مجموعة من القيود بشكل متكرر حتى الوصول إلى نقطة ثابتة
// Solves a set of constraints iteratively until reaching a fixed point
//
// الاستخدام / Usage:
//   ConstraintSolver solver;
//   ConstraintSet constraints = ...; // جمع القيود / Collect constraints
//   auto result = solver.solve(constraints);
//   if (result.success) {
//       // استخدم result.substitution / Use result.substitution
//   }
// ════════════════════════════════════════════════════════════════════════════════

class ConstraintSolver {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئات / Constructors
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ افتراضي / Default constructor
    ConstraintSolver();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الحل الرئيسي / Main Solving
    // ═══════════════════════════════════════════════════════════════════════════
    
    // حل مجموعة من القيود / Solve a set of constraints
    // المعامل / Parameter: constraints - مجموعة القيود / Constraint set
    // الإرجاع / Return: نتيجة الحل / Solver result
    SolverResult solve(const ConstraintSet& constraints);
    
    // حل قيد واحد / Solve a single constraint
    // المعامل / Parameter: constraint - القيد / Constraint
    // الإرجاع / Return: نتيجة الحل / Solver result
    SolverResult solveOne(const Constraint& constraint);
    
    // حل مع استبدال موجود / Solve with existing substitution
    // المعامل / Parameter: constraints - مجموعة القيود / Constraint set
    // المعامل / Parameter: initialSubst - الاستبدال الابتدائي / Initial substitution
    // الإرجاع / Return: نتيجة الحل / Solver result
    SolverResult solve(const ConstraintSet& constraints, 
                      const Substitution& initialSubst);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الإعدادات / Settings
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تعيين الحد الأقصى للتكرارات / Set maximum iterations
    // المعامل / Parameter: maxIter - الحد الأقصى / Maximum iterations
    void setMaxIterations(int maxIter) { maxIterations_ = maxIter; }
    
    // الحصول على الحد الأقصى للتكرارات / Get maximum iterations
    int getMaxIterations() const { return maxIterations_; }
    
    // تفعيل/تعطيل Verbose mode (طباعة تفصيلية)
    // Enable/disable verbose mode (detailed logging)
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    // هل Verbose mode مُفعّل؟ / Is verbose mode enabled?
    bool isVerbose() const { return verbose_; }
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الإحصائيات / Statistics
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على عدد القيود المحلولة / Get solved constraints count
    int getSolvedConstraintsCount() const { return solvedCount_; }
    
    // الحصول على عدد القيود الفاشلة / Get failed constraints count
    int getFailedConstraintsCount() const { return failedCount_; }
    
    // إعادة تعيين الإحصائيات / Reset statistics
    void resetStatistics();
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال الحل الداخلية / Internal Solving Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // حل قيد مساواة / Solve equality constraint
    // المعامل / Parameter: constraint - القيد / Constraint
    // المعامل / Parameter: subst - الاستبدال (يُعدّل) / Substitution (modified)
    // الإرجاع / Return: نتيجة الحل / Solver result
    SolverResult solveEquality(const Constraint& constraint, Substitution& subst);
    
    // حل قيد نوع فرعي / Solve subtype constraint
    // المعامل / Parameter: constraint - القيد / Constraint
    // المعامل / Parameter: subst - الاستبدال (يُعدّل) / Substitution (modified)
    // الإرجاع / Return: نتيجة الحل / Solver result
    SolverResult solveSubtype(const Constraint& constraint, Substitution& subst);
    
    // حل قيد امتلاك عضو / Solve has-member constraint
    // type1 يملك عضواً باسم location_ وبنوع type2
    // type1 has a member named location_ of type type2
    SolverResult solveHasMember(const Constraint& constraint, Substitution& subst);
    
    // حل قيد قابلية الاستدعاء / Solve callable constraint
    // type1 قابل للاستدعاء ويُرجع type2
    // type1 is callable and returns type2
    SolverResult solveCallable(const Constraint& constraint, Substitution& subst);
    
    // حل قيد قابلية التكرار / Solve iterable constraint
    // type1 قابل للتكرار وعنصره type2
    // type1 is iterable with element type type2
    SolverResult solveIterable(const Constraint& constraint, Substitution& subst);
    
    // تطبيق الاستبدال على القيود المتبقية / Apply substitution to remaining constraints
    // المعامل / Parameter: constraints - القيود / Constraints
    // المعامل / Parameter: subst - الاستبدال / Substitution
    void applySubstitutionToConstraints(std::vector<Constraint>& constraints, 
                                       const Substitution& subst);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال التحقق / Validation Functions
    // ═══════════════════════════════════════════════════════════════════════════
    
    // التحقق من عدم وجود تناقضات / Check for contradictions
    // المعامل / Parameter: subst - الاستبدال / Substitution
    // الإرجاع / Return: true إذا كان متسقاً / true if consistent
    bool isConsistent(const Substitution& subst) const;
    
    // التحقق من الوصول إلى نقطة ثابتة / Check if fixed point reached
    // المعامل / Parameter: oldSubst - الاستبدال القديم / Old substitution
    // المعامل / Parameter: newSubst - الاستبدال الجديد / New substitution
    // الإرجاع / Return: true إذا وصلنا إلى نقطة ثابتة / true if fixed point
    bool isFixedPoint(const Substitution& oldSubst, 
                     const Substitution& newSubst) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // دوال الطباعة (للتشخيص) / Logging Functions (for diagnostics)
    // ═══════════════════════════════════════════════════════════════════════════
    
    // طباعة حالة الحل / Log solving state
    // المعامل / Parameter: iteration - رقم التكرار / Iteration number
    // المعامل / Parameter: remaining - عدد القيود المتبقية / Remaining constraints count
    void logState(int iteration, int remaining) const;
    
    // طباعة قيد / Log constraint
    // المعامل / Parameter: constraint - القيد / Constraint
    // المعامل / Parameter: solved - هل تم حله؟ / Was it solved?
    void logConstraint(const Constraint& constraint, bool solved) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // محرك التوحيد / Unification engine
    Unifier unifier_;
    
    // الحد الأقصى للتكرارات / Maximum iterations
    int maxIterations_;
    
    // وضع Verbose (طباعة تفصيلية) / Verbose mode (detailed logging)
    bool verbose_;
    
    // إحصائيات / Statistics
    int solvedCount_;        // عدد القيود المحلولة / Solved constraints count
    int failedCount_;        // عدد القيود الفاشلة / Failed constraints count
};

// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة عامة / Global Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

// حل سريع لمجموعة قيود / Quick solve constraint set
// المعامل / Parameter: constraints - مجموعة القيود / Constraint set
// الإرجاع / Return: نتيجة الحل / Solver result
SolverResult solveConstraints(const ConstraintSet& constraints);

// حل قيد واحد بسرعة / Quick solve single constraint
// المعامل / Parameter: constraint - القيد / Constraint
// الإرجاع / Return: نتيجة الحل / Solver result
SolverResult solveConstraint(const Constraint& constraint);

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_CONSTRAINT_SOLVER_H
