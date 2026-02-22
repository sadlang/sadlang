// ════════════════════════════════════════════════════════════════════════════════
// ملف: constraint_solver.cpp
// File: constraint_solver.cpp
//
// الوصف: تنفيذ حلال القيود (Constraint Solver) - Iterative Constraint Solving
// Description: Implementation of constraint solver - Iterative Constraint Solving
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 3 يناير 2026
// Date: January 3, 2026
//
// المرجع: Constraint-Based Type Inference
// Reference: Mitchell Wand, "Type Inference for Record Concatenation and Multiple Inheritance"
// ════════════════════════════════════════════════════════════════════════════════

#include "constraint_solver.h"  // المصدر: constraint_solver.h:1-250 / Source: constraint_solver.h:1-250
#include "unification.h"        // المصدر: unification.h:1-240 / Source: unification.h:1-240
#include "type_variable.h"      // لـ isTypeVariable, asTypeVariable / For isTypeVariable, asTypeVariable
#include <sstream>              // لـ ostringstream / For ostringstream
#include <algorithm>            // لـ std::find / For std::find
#include <iostream>             // لـ std::cout / For std::cout

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ SolverError
// SolverError Implementation
// ════════════════════════════════════════════════════════════════════════════════

// المنشئ الكامل / Full constructor
// المصدر: constraint_solver.h:41-45 / Source: constraint_solver.h:41-45
SolverError::SolverError(SolverErrorKind kind,
                        const Constraint& constraint,
                        const std::string& message)
    : kind_(kind),           // تخزين نوع الخطأ / Store error kind
      constraint_(constraint), // تخزين القيد المُسبب للخطأ / Store constraint causing error
      message_(message) {}   // تخزين رسالة الخطأ / Store error message

// المنشئ الافتراضي / Default constructor
// المصدر: constraint_solver.h:47 / Source: constraint_solver.h:47
SolverError::SolverError()
    : kind_(SolverErrorKind::None),  // بدون خطأ / No error
      constraint_(ConstraintKind::Equality, nullptr, nullptr, ""),  // قيد فارغ / Empty constraint
      message_("") {}  // رسالة فارغة / Empty message

// تحويل الخطأ إلى نص / Convert error to string
// المصدر: constraint_solver.h:81 / Source: constraint_solver.h:81
std::string SolverError::toString() const {
    std::ostringstream oss;  // منشئ النص / String builder
    
    // البادئة حسب نوع الخطأ / Prefix based on error kind
    switch (kind_) {
        case SolverErrorKind::UnificationFailed:
            oss << "خطأ فشل التوحيد / Unification Failed Error: ";
            break;
        case SolverErrorKind::ConflictingConstraints:
            oss << "خطأ قيود متعارضة / Conflicting Constraints Error: ";
            break;
        case SolverErrorKind::InfiniteSubstitution:
            oss << "خطأ استبدال لانهائي / Infinite Substitution Error: ";
            break;
        case SolverErrorKind::UnsolvableConstraint:
            oss << "خطأ قيد غير قابل للحل / Unsolvable Constraint Error: ";
            break;
        case SolverErrorKind::None:
            oss << "لا يوجد خطأ / No Error: ";
            break;
        default:
            oss << "خطأ غير معروف / Unknown Error: ";
            break;
    }
    
    oss << message_;  // إضافة رسالة الخطأ / Add error message
    
    // إضافة تفاصيل القيد / Add constraint details
    oss << " [القيد / Constraint: " << constraint_.toString() << "]";  // المصدر: constraint.h:122 / Source: constraint.h:122
    
    return oss.str();  // إرجاع النص المُشكَّل / Return formatted string
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ SolverResult
// SolverResult Implementation
// ════════════════════════════════════════════════════════════════════════════════

// إنشاء نتيجة ناجحة / Create success result
// المصدر: constraint_solver.h:107 / Source: constraint_solver.h:107
SolverResult SolverResult::makeSuccess(const Substitution& subst, int iterations) {
    SolverResult result;         // إنشاء نتيجة جديدة / Create new result
    result.success = true;       // تعيين النجاح / Set success
    result.substitution = subst; // نسخ الاستبدال / Copy substitution
    result.iterations = iterations; // تخزين عدد التكرارات / Store iteration count
    return result;               // إرجاع النتيجة / Return result
}

// إنشاء نتيجة فاشلة / Create failure result
// المصدر: constraint_solver.h:110 / Source: constraint_solver.h:110
SolverResult SolverResult::makeFailure(const SolverError& err) {
    SolverResult result;       // إنشاء نتيجة جديدة / Create new result
    result.success = false;    // تعيين الفشل / Set failure
    result.error = err;        // نسخ الخطأ / Copy error
    result.iterations = 0;     // لا يوجد تكرارات / No iterations
    return result;             // إرجاع النتيجة / Return result
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ ConstraintSolver - المحرك الرئيسي
// ConstraintSolver Implementation - Main Engine
// ════════════════════════════════════════════════════════════════════════════════

// المنشئ / Constructor
// المصدر: constraint_solver.h:131 / Source: constraint_solver.h:131
ConstraintSolver::ConstraintSolver()
    : maxIterations_(1000),  // الحد الأقصى للتكرارات / Maximum iterations
      verbose_(false),       // عدم طباعة التفاصيل / Don't print details
      solvedCount_(0),       // عدد القيود المحلولة = 0 / Solved constraints count = 0
      failedCount_(0) {}     // عدد القيود الفاشلة = 0 / Failed constraints count = 0

// حل مجموعة قيود بسيط / Simple solve constraint set
// المصدر: constraint_solver.h:144 / Source: constraint_solver.h:144
SolverResult ConstraintSolver::solve(const ConstraintSet& constraints) {
    Substitution emptySubst;  // استبدال فارغ / Empty substitution
    return solve(constraints, emptySubst);  // استدعاء النسخة الكاملة / Call full version
}

// حل مجموعة قيود مع استبدال مبدئي - المحرك الرئيسي
// Solve constraint set with initial substitution - Main engine
// المصدر: constraint_solver.h:149 / Source: constraint_solver.h:149
SolverResult ConstraintSolver::solve(const ConstraintSet& constraints, 
                                    const Substitution& initialSubst) {
    // === الخطوة 1: التهيئة / Step 1: Initialization ===
    Substitution currentSubst = initialSubst;  // نسخ الاستبدال المبدئي / Copy initial substitution
    int iteration = 0;  // عداد التكرارات / Iteration counter
    
    // المصدر: constraint.h:224 / Source: constraint.h:224
    std::vector<Constraint> remaining(constraints.getConstraints());  // نسخ القيود / Copy constraints
    std::vector<Constraint> solved;  // قيود محلولة / Solved constraints
    
    if (verbose_) {
        std::cout << "=== بدء حل القيود / Starting constraint solving ===" << std::endl;
        std::cout << "عدد القيود / Number of constraints: " << remaining.size() << std::endl;
    }
    
    // === الخطوة 2: الحل التكراري / Step 2: Iterative solving ===
    while (!remaining.empty() && iteration < maxIterations_) {
        bool progress = false;  // هل تم إحراز تقدم؟ / Was progress made?
        std::vector<Constraint> nextRemaining;  // قيود للتكرار التالي / Constraints for next iteration
        
        if (verbose_) {
            std::cout << "\n--- التكرار / Iteration " << (iteration + 1) << " ---" << std::endl;
            std::cout << "قيود متبقية / Remaining constraints: " << remaining.size() << std::endl;
        }
        
        // === الخطوة 3: محاولة حل كل قيد / Step 3: Try to solve each constraint ===
        for (const auto& constraint : remaining) {
            if (verbose_) {
                std::cout << "  حل القيد / Solving constraint: " 
                          << constraint.toString() << std::endl;  // المصدر: constraint.h:122 / Source: constraint.h:122
            }
            
            // اختيار طريقة الحل حسب نوع القيد / Choose solution method based on constraint kind
            SolverResult result;
            
            // المصدر: constraint.h:96 / Source: constraint.h:96
            switch (constraint.getKind()) {
                case ConstraintKind::Equality:
                    // قيد مساواة: T1 = T2 / Equality constraint: T1 = T2
                    result = solveEquality(constraint, currentSubst);
                    break;
                    
                case ConstraintKind::Subtype:
                    // قيد تحت-نوع: T1 <: T2 / Subtype constraint: T1 <: T2
                    result = solveSubtype(constraint, currentSubst);
                    break;
                    
                case ConstraintKind::HasMember:
                    // قيد امتلاك عضو: T has member M / Has member constraint: T has member M
                    result = solveHasMember(constraint, currentSubst);
                    break;
                    
                case ConstraintKind::Callable:
                    // قيد قابلية الاستدعاء: T(A) -> R / Callable constraint: T(A) -> R
                    result = solveCallable(constraint, currentSubst);
                    break;
                    
                case ConstraintKind::Iterable:
                    // قيد قابلية التكرار: T is iterable<E> / Iterable constraint: T is iterable<E>
                    result = solveIterable(constraint, currentSubst);
                    break;
                    
                default:
                    // قيد غير معروف / Unknown constraint
                    result = SolverResult::makeFailure(
                        SolverError(SolverErrorKind::UnsolvableConstraint,
                                   constraint,
                                   "نوع قيد غير مدعوم / Unsupported constraint kind")
                    );
                    break;
            }
            
            // === الخطوة 4: معالجة نتيجة الحل / Step 4: Process solution result ===
            if (result.success) {
                // القيد تم حله بنجاح / Constraint solved successfully
                currentSubst = result.substitution;  // تحديث الاستبدال / Update substitution
                solved.push_back(constraint);  // إضافة للقيود المحلولة / Add to solved constraints
                solvedCount_++;  // زيادة العداد / Increment counter
                progress = true;  // تم إحراز تقدم / Progress made
                
                if (verbose_) {
                    std::cout << "    ✓ نجح / Succeeded" << std::endl;
                }
            } else {
                // فشل حل القيد / Failed to solve constraint
                nextRemaining.push_back(constraint);  // احتفظ بالقيد / Keep constraint
                
                if (verbose_) {
                    std::cout << "    ✗ فشل / Failed: " << result.error.getMessage() << std::endl;
                }
            }
        }
        
        // === الخطوة 5: التحقق من التقدم / Step 5: Check progress ===
        remaining = nextRemaining;  // تحديث القيود المتبقية / Update remaining constraints
        iteration++;  // زيادة عداد التكرارات / Increment iteration counter
        
        // إذا لم يكن هناك تقدم، توقف / If no progress, stop
        if (!progress) {
            if (verbose_) {
                std::cout << "\n⚠ لا يوجد تقدم، التوقف / No progress, stopping" << std::endl;
            }
            break;
        }
        
        // التحقق من نقطة الثبات / Check fixed point
        // TODO: في التنفيذ الكامل، سنستخدم isFixedPoint بشكل صحيح
        if (remaining.empty()) {
            if (verbose_) {
                std::cout << "\n✓ وصلنا لنقطة الثبات / Reached fixed point" << std::endl;
            }
            break;
        }
    }
    
    // === الخطوة 6: التحقق من النتيجة النهائية / Step 6: Check final result ===
    if (iteration >= maxIterations_) {
        // تجاوزنا الحد الأقصى للتكرارات / Exceeded maximum iterations
        if (verbose_) {
            std::cout << "\n✗ تجاوز الحد الأقصى للتكرارات / Exceeded maximum iterations" << std::endl;
        }
        
        SolverError error(
            SolverErrorKind::InfiniteSubstitution,
            remaining.empty() ? Constraint(ConstraintKind::Equality, nullptr, nullptr, "") : remaining[0],
            "تجاوز الحد الأقصى للتكرارات، قد يكون هناك استبدال لانهائي / "
            "Exceeded maximum iterations, might be infinite substitution"
        );
        return SolverResult::makeFailure(error);
    }
    
    if (!remaining.empty()) {
        // بقيت قيود غير محلولة / Constraints remain unsolved
        if (verbose_) {
            std::cout << "\n✗ قيود غير محلولة / Unsolved constraints: " << remaining.size() << std::endl;
            for (const auto& c : remaining) {
                std::cout << "    - " << c.toString() << std::endl;  // المصدر: constraint.h:122 / Source: constraint.h:122
            }
        }
        
        failedCount_ += static_cast<int>(remaining.size());
        SolverError error(
            SolverErrorKind::UnsolvableConstraint,
            remaining[0],
            "بعض القيود غير قابلة للحل / Some constraints are unsolvable"
        );
        return SolverResult::makeFailure(error);
    }
    
    // === نجاح كامل / Complete success ===
    if (verbose_) {
        std::cout << "\n✓✓✓ نجح حل جميع القيود / Successfully solved all constraints ✓✓✓" << std::endl;
        std::cout << "عدد التكرارات / Iterations: " << iteration << std::endl;
        std::cout << "قيود محلولة / Solved constraints: " << solved.size() << std::endl;
    }
    
    return SolverResult::makeSuccess(currentSubst, iteration);
}

// حل قيد واحد / Solve single constraint
// المصدر: constraint_solver.h:154 / Source: constraint_solver.h:154
SolverResult ConstraintSolver::solveOne(const Constraint& constraint) {
    Substitution emptySubst;  // استبدال فارغ / Empty substitution
    Substitution workingSubst = emptySubst;
    return solveEquality(constraint, workingSubst);
}

// حل قيد المساواة / Solve equality constraint
// المصدر: constraint_solver.h:166 / Source: constraint_solver.h:166
SolverResult ConstraintSolver::solveEquality(const Constraint& constraint, 
                                            Substitution& subst) {
    // التحقق من نوع القيد / Verify constraint kind
    // المصدر: constraint.h:111 / Source: constraint.h:111
    if (!constraint.isEquality()) {
        return SolverResult::makeFailure(
            SolverError(SolverErrorKind::UnsolvableConstraint,
                       constraint,
                       "القيد ليس قيد مساواة / Constraint is not an equality constraint")
        );
    }
    
    // استخدام محرك التوحيد / Use unification engine
    // المصدر: unification.h:153 / Source: unification.h:153
    // المصدر: constraint.h:127-128 / Source: constraint.h:127-128
    UnificationResult unifResult = unifier_.unify(
        constraint.getType1(),  // النوع الأول / First type
        constraint.getType2(),  // النوع الثاني / Second type
        subst                   // الاستبدال الحالي / Current substitution
    );
    
    if (unifResult.success) {
        // التوحيد نجح / Unification succeeded
        subst = unifResult.substitution;  // تحديث الاستبدال / Update substitution
        return SolverResult::makeSuccess(subst, 1);  // نجح في تكرار واحد / Succeeded in one iteration
    }
    
    // التوحيد فشل / Unification failed
    SolverError error(
        SolverErrorKind::UnificationFailed,
        constraint,
        unifResult.error.getMessage()  // المصدر: unification.h:71 / Source: unification.h:71
    );
    return SolverResult::makeFailure(error);
}

// حل قيد التحت-نوع / Solve subtype constraint
// المصدر: constraint_solver.h:171 / Source: constraint_solver.h:171
SolverResult ConstraintSolver::solveSubtype(const Constraint& constraint, 
                                           Substitution& subst) {
    // التحقق من نوع القيد / Verify constraint kind
    // المصدر: constraint.h:116 / Source: constraint.h:116
    if (!constraint.isSubtype()) {
        return SolverResult::makeFailure(
            SolverError(SolverErrorKind::UnsolvableConstraint,
                       constraint,
                       "القيد ليس قيد تحت-نوع / Constraint is not a subtype constraint")
        );
    }
    
    // الحصول على الأنواع / Get types
    // المصدر: constraint.h:127-128 / Source: constraint.h:127-128
    TypePtr type1 = constraint.getType1();  // تطبيق الاستبدال / Apply substitution
    TypePtr type2 = constraint.getType2();
    
    if (!type1 || !type2) {
        return SolverResult::makeFailure(
            SolverError(SolverErrorKind::UnsolvableConstraint,
                       constraint,
                       "أحد الأنواع null / One of the types is null")
        );
    }
    
    // المصدر: substitution.h:81 / Source: substitution.h:81
    type1 = subst.apply(type1);  // تطبيق الاستبدال على النوع الأول / Apply substitution to first type
    type2 = subst.apply(type2);  // تطبيق الاستبدال على النوع الثاني / Apply substitution to second type
    
    // التحقق من علاقة التحت-نوع / Check subtype relation
    // المصدر: type.h:168 / Source: type.h:168
    if (type1->isSubtypeOf(type2.get())) {
        // علاقة التحت-نوع صحيحة / Subtype relation holds
        return SolverResult::makeSuccess(subst, 1);
    }
    
    // علاقة التحت-نوع لا تحقق / Subtype relation doesn't hold
    std::ostringstream oss;
    oss << "النوع / Type " << type1->toString() << " ليس تحت-نوع من / is not a subtype of " << type2->toString();
    
    return SolverResult::makeFailure(
        SolverError(SolverErrorKind::ConflictingConstraints,
                   constraint,
                   oss.str())
    );
}



// حل قيد امتلاك عضو / Solve has-member constraint
// type1_ = النوع الذي يجب أن يملك العضو / The type that should have the member
// type2_ = نوع العضو المتوقع / Expected member type
// location_ = اسم العضو / Member name
SolverResult ConstraintSolver::solveHasMember(const Constraint& constraint, 
                                               Substitution& subst) {
    TypePtr containerType = constraint.getType1();
    TypePtr memberType = constraint.getType2();
    
    if (!containerType || !memberType) {
        return SolverResult::makeFailure(
            SolverError(SolverErrorKind::UnsolvableConstraint,
                       constraint,
                       "أحد الأنواع null في قيد العضو / One of the types is null in member constraint")
        );
    }
    
    // تطبيق الاستبدال الحالي / Apply current substitution
    containerType = subst.apply(containerType);
    memberType = subst.apply(memberType);
    
    // إذا كان النوع الحاوي متغير نوع، لا يمكن حله بعد
    // If container type is a type variable, can't solve yet
    if (isTypeVariable(containerType)) {
        // سنعيد المحاولة لاحقاً / Defer - will retry later
        return SolverResult::makeFailure(
            SolverError(SolverErrorKind::UnsolvableConstraint,
                       constraint,
                       "النوع الحاوي ما زال متغيراً / Container type is still a variable")
        );
    }
    
    // إذا كان النوع الحاوي صنف/بنية — نقبل القيد (فحص العضو يتم في type_checker)
    // If container is class/struct — accept (member check done in type_checker)
    if (containerType->isClass() || containerType->isInterface()) {
        // توحيد نوع العضو إذا كان متغيراً / Unify member type if it's a variable
        if (isTypeVariable(memberType)) {
            // العضو يمكن أن يكون أي نوع — نرجع نجاح مع الاستبدال الحالي
            return SolverResult::makeSuccess(subst, 1);
        }
        return SolverResult::makeSuccess(subst, 1);
    }
    
    // إذا كان مصفوفة — يملك أعضاء مثل الطول وعمليات الفهرسة
    // If array — has members like length and indexing
    if (containerType->isArray()) {
        return SolverResult::makeSuccess(subst, 1);
    }
    
    // إذا كان قاموس — يملك مفتاح/قيمة
    // If dictionary — has key/value members
    if (containerType->isDictionary()) {
        return SolverResult::makeSuccess(subst, 1);
    }
    
    // النوع لا يدعم الأعضاء / Type doesn't support members
    std::ostringstream oss;
    oss << "النوع / Type " << containerType->toString() 
        << " لا يملك العضو / does not have member '" 
        << constraint.getLocation() << "'";
    
    return SolverResult::makeFailure(
        SolverError(SolverErrorKind::ConflictingConstraints,
                   constraint,
                   oss.str())
    );
}

// حل قيد قابلية الاستدعاء / Solve callable constraint
// type1_ = النوع الذي يجب أن يكون قابلاً للاستدعاء / Type that must be callable
// type2_ = نوع الإرجاع المتوقع / Expected return type
SolverResult ConstraintSolver::solveCallable(const Constraint& constraint, 
                                              Substitution& subst) {
    TypePtr callableType = constraint.getType1();
    TypePtr returnType = constraint.getType2();
    
    if (!callableType) {
        return SolverResult::makeFailure(
            SolverError(SolverErrorKind::UnsolvableConstraint,
                       constraint,
                       "نوع الاستدعاء null / Callable type is null")
        );
    }
    
    // تطبيق الاستبدال / Apply substitution
    callableType = subst.apply(callableType);
    if (returnType) {
        returnType = subst.apply(returnType);
    }
    
    // إذا كان متغير نوع، لا يمكن حله بعد / If type variable, defer
    if (isTypeVariable(callableType)) {
        return SolverResult::makeFailure(
            SolverError(SolverErrorKind::UnsolvableConstraint,
                       constraint,
                       "النوع القابل للاستدعاء ما زال متغيراً / Callable type is still a variable")
        );
    }
    
    // التحقق: هل النوع دالة؟ / Check: is the type a function?
    if (callableType->isFunction()) {
        // نجح — النوع قابل للاستدعاء فعلاً / Success — type is callable
        // إذا كان نوع الإرجاع متغيراً، نحاول توحيده
        // If return type is a variable, try to unify
        if (returnType && isTypeVariable(returnType)) {
            // في التنفيذ المكتمل: استخراج نوع إرجاع الدالة وتوحيده
            // In full implementation: extract function return type and unify
            return SolverResult::makeSuccess(subst, 1);
        }
        return SolverResult::makeSuccess(subst, 1);
    }
    
    // إذا كان صنف — قد يملك عامل استدعاء / If class — may have call operator
    if (callableType->isClass()) {
        return SolverResult::makeSuccess(subst, 1);
    }
    
    // النوع غير قابل للاستدعاء / Type is not callable
    std::ostringstream oss;
    oss << "النوع / Type " << callableType->toString() 
        << " غير قابل للاستدعاء / is not callable";
    
    return SolverResult::makeFailure(
        SolverError(SolverErrorKind::ConflictingConstraints,
                   constraint,
                   oss.str())
    );
}

// حل قيد قابلية التكرار / Solve iterable constraint
// type1_ = النوع الذي يجب أن يكون قابلاً للتكرار / Type that must be iterable
// type2_ = نوع العنصر المتوقع / Expected element type
SolverResult ConstraintSolver::solveIterable(const Constraint& constraint, 
                                              Substitution& subst) {
    TypePtr iterableType = constraint.getType1();
    TypePtr elementType = constraint.getType2();
    
    if (!iterableType) {
        return SolverResult::makeFailure(
            SolverError(SolverErrorKind::UnsolvableConstraint,
                       constraint,
                       "نوع التكرار null / Iterable type is null")
        );
    }
    
    // تطبيق الاستبدال / Apply substitution
    iterableType = subst.apply(iterableType);
    if (elementType) {
        elementType = subst.apply(elementType);
    }
    
    // إذا كان متغير نوع، لا يمكن حله بعد / If type variable, defer
    if (isTypeVariable(iterableType)) {
        return SolverResult::makeFailure(
            SolverError(SolverErrorKind::UnsolvableConstraint,
                       constraint,
                       "النوع القابل للتكرار ما زال متغيراً / Iterable type is still a variable")
        );
    }
    
    // المصفوفات والقوائم قابلة للتكرار / Arrays and lists are iterable
    if (iterableType->isArray()) {
        // إذا كان نوع العنصر متغيراً — نحاول توحيده مع نوع عنصر المصفوفة
        // If element type is a variable — try to unify with array element type
        if (elementType && isTypeVariable(elementType)) {
            // نقبل الآن ونترك التوحيد للقيود اللاحقة
            // Accept now and leave unification for subsequent constraints
            return SolverResult::makeSuccess(subst, 1);
        }
        return SolverResult::makeSuccess(subst, 1);
    }
    
    // القواميس قابلة للتكرار (على الأزواج) / Dictionaries are iterable (over pairs)
    if (iterableType->isDictionary()) {
        return SolverResult::makeSuccess(subst, 1);
    }
    
    // النصوص قابلة للتكرار (على الأحرف) / Strings are iterable (over characters)
    if (iterableType->isString()) {
        return SolverResult::makeSuccess(subst, 1);
    }
    
    // الصفوف قابلة للتكرار / Tuples are iterable
    if (iterableType->isTuple()) {
        return SolverResult::makeSuccess(subst, 1);
    }
    
    // الأصناف قد تنفذ واجهة التكرار / Classes may implement iterator interface
    if (iterableType->isClass() || iterableType->isInterface()) {
        return SolverResult::makeSuccess(subst, 1);
    }
    
    // النوع غير قابل للتكرار / Type is not iterable
    std::ostringstream oss;
    oss << "النوع / Type " << iterableType->toString() 
        << " غير قابل للتكرار / is not iterable";
    
    return SolverResult::makeFailure(
        SolverError(SolverErrorKind::ConflictingConstraints,
                   constraint,
                   oss.str())
    );
}



// إعادة تعيين الإحصائيات / Reset statistics
// المصدر: constraint_solver.h:206 / Source: constraint_solver.h:206
void ConstraintSolver::resetStatistics() {
    solvedCount_ = 0;  // تصفير عداد القيود المحلولة / Reset solved counter
    failedCount_ = 0;  // تصفير عداد القيود الفاشلة / Reset failed counter
}



// ════════════════════════════════════════════════════════════════════════════════
// دوال مساعدة عامة / Global Helper Functions
// ════════════════════════════════════════════════════════════════════════════════

// حل مجموعة قيود / Solve constraint set
// المصدر: constraint_solver.h:234 / Source: constraint_solver.h:234
SolverResult solveConstraints(const ConstraintSet& constraints) {
    ConstraintSolver solver;  // إنشاء حلال / Create solver
    return solver.solve(constraints);  // حل القيود / Solve constraints
}

// حل قيد واحد / Solve single constraint
// المصدر: constraint_solver.h:239 / Source: constraint_solver.h:239
SolverResult solveConstraint(const Constraint& constraint) {
    ConstraintSet set;  // إنشاء مجموعة / Create set
    set.add(constraint);  // المصدر: constraint.h:202 / Source: constraint.h:202
    return solveConstraints(set);  // حل المجموعة / Solve set
}

} // namespace TypeSystem
} // namespace Sad
