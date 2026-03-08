// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file parallel_solver.cpp
 * @brief تنفيذ حلال القيود المتوازي
 * 
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 */

#include "parallel_solver.hpp"
#include "constraint_solver.h"
#include "constraint.h"
#include "substitution.h"
#include <algorithm>
#include <chrono>
#include <iostream>

namespace Sad {
namespace TypeSystem {

// ═══════════════════════════════════════════════════════════════════════════
//                    المنشئ والمدمر / Constructor & Destructor
// ═══════════════════════════════════════════════════════════════════════════

ParallelConstraintSolver::ParallelConstraintSolver(const ParallelSolverConfig& config)
    : config_(config), solving_(false), cancelled_(false) {}

ParallelConstraintSolver::~ParallelConstraintSolver() {
    cancel();
}

// ═══════════════════════════════════════════════════════════════════════════
//                    الحل المتوازي / Parallel Solving
// ═══════════════════════════════════════════════════════════════════════════

ParallelSolveResult ParallelConstraintSolver::solve(const std::vector<Constraint>& constraints) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // التحقق من القيود الفارغة
    if (constraints.empty()) {
        auto result = ParallelSolveResult::makeSuccess(std::make_shared<Substitution>());
        result.totalConstraints = 0;
        result.constraintGroups = 0;
        result.threadsUsed = 0;
        return result;
    }
    
    // ضبط حالة الحل
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (solving_.load()) {
            return ParallelSolveResult::makeFailure("الحلال مشغول بالفعل / Solver is already busy");
        }
        solving_.store(true);
        cancelled_.store(false);
    }
    
    ParallelSolveResult result;
    result.totalConstraints = constraints.size();
    
    try {
        // تقسيم القيود إلى مجموعات مستقلة
        auto groups = partitionConstraints(constraints);
        result.constraintGroups = groups.size();
        
        if (config_.verbose) {
            std::cout << "=== حل متوازي للقيود / Parallel constraint solving ===" << std::endl;
            std::cout << "إجمالي القيود / Total constraints: " << constraints.size() << std::endl;
            std::cout << "مجموعات مستقلة / Independent groups: " << groups.size() << std::endl;
        }
        
        // تحديد عدد الخيوط
        size_t numThreads = std::min(config_.maxThreads, groups.size());
        result.threadsUsed = numThreads;
        
        // إذا كان لدينا مجموعة واحدة أو خيط واحد، استخدم الحل التسلسلي
        if (groups.size() <= 1 || numThreads <= 1) {
            ConstraintSolver solver;
            ConstraintSet constraintSet;
            for (const auto& c : constraints) {
                constraintSet.add(c);
            }
            auto solverResult = solver.solve(constraintSet);
            
            if (solverResult.success) {
                result = ParallelSolveResult::makeSuccess(
                    std::make_shared<Substitution>(solverResult.substitution));
            } else {
                result = ParallelSolveResult::makeFailure(solverResult.error.getMessage());
            }
            result.threadsUsed = 1;
        } else {
            // حل كل مجموعة في خيط منفصل
            std::vector<std::future<SolverResult>> futures;
            std::vector<std::shared_ptr<Substitution>> substitutions;
            
            Substitution emptySubst;
            
            for (size_t i = 0; i < groups.size() && !cancelled_.load(); ++i) {
                auto& group = groups[i];
                
                // إنشاء مهمة للمجموعة
                futures.push_back(std::async(std::launch::async,
                    [this, &constraints, &group, &emptySubst]() {
                        return solveGroup(constraints, group.constraintIndices, emptySubst);
                    }));
            }
            
            // جمع النتائج
            bool allSuccess = true;
            for (auto& future : futures) {
                if (cancelled_.load()) {
                    allSuccess = false;
                    result.errors.push_back("تم إلغاء الحل / Solving was cancelled");
                    break;
                }
                
                auto groupResult = future.get();
                if (groupResult.success) {
                    substitutions.push_back(
                        std::make_shared<Substitution>(groupResult.substitution));
                } else {
                    allSuccess = false;
                    result.errors.push_back(groupResult.error.getMessage());
                }
            }
            
            if (allSuccess && !substitutions.empty()) {
                // دمج الاستبدالات
                auto merged = mergeSubstitutions(substitutions);
                if (merged) {
                    result = ParallelSolveResult::makeSuccess(merged);
                } else {
                    result = ParallelSolveResult::makeFailure(
                        "فشل دمج الاستبدالات / Failed to merge substitutions");
                }
            } else {
                result.success = false;
            }
        }
    } catch (const std::exception& e) {
        result = ParallelSolveResult::makeFailure(
            std::string("استثناء أثناء الحل / Exception during solving: ") + e.what());
    }
    
    // حساب الوقت
    auto endTime = std::chrono::high_resolution_clock::now();
    result.solveTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    result.totalConstraints = constraints.size();
    
    if (config_.verbose) {
        std::cout << "الوقت / Time: " << result.solveTimeMs << " ms" << std::endl;
        std::cout << "النتيجة / Result: " << (result.success ? "نجاح" : "فشل") << std::endl;
    }
    
    solving_.store(false);
    return result;
}

void ParallelConstraintSolver::cancel() {
    cancelled_.store(true);
}

// ═══════════════════════════════════════════════════════════════════════════
//                    تحليل التبعيات / Dependency Analysis
// ═══════════════════════════════════════════════════════════════════════════

std::vector<ConstraintGroup> ParallelConstraintSolver::partitionConstraints(
    const std::vector<Constraint>& constraints) {
    
    std::vector<ConstraintGroup> groups;
    
    // إنشاء مجموعة لكل قيد
    for (size_t i = 0; i < constraints.size(); ++i) {
        ConstraintGroup group;
        group.constraintIndices.push_back(i);
        group.typeVariables = getTypeVariables(constraints[i]);
        groups.push_back(group);
    }
    
    // دمج المجموعات المترابطة
    bool merged = true;
    while (merged) {
        merged = false;
        
        for (size_t i = 0; i < groups.size() && !merged; ++i) {
            for (size_t j = i + 1; j < groups.size() && !merged; ++j) {
                if (!groups[i].isIndependentFrom(groups[j])) {
                    // دمج المجموعتين
                    groups[i].merge(groups[j]);
                    groups.erase(groups.begin() + j);
                    merged = true;
                }
            }
        }
    }
    
    // ترتيب المجموعات حسب الحجم (الأكبر أولاً للتوزيع الأفضل)
    std::sort(groups.begin(), groups.end(),
        [](const ConstraintGroup& a, const ConstraintGroup& b) {
            return a.constraintIndices.size() > b.constraintIndices.size();
        });
    
    return groups;
}

std::unordered_set<size_t> ParallelConstraintSolver::getTypeVariables(const Constraint& constraint) {
    std::unordered_set<size_t> vars;
    
    // استخراج متغيرات النوع من الطرفين
    // هذه نسخة مبسطة - في التنفيذ الكامل، نحتاج للتعمق في بنية النوع
    
    auto type1 = constraint.getType1();
    auto type2 = constraint.getType2();
    
    if (type1 && type1->getKind() == TypeKind::TypeParameter) {
        // افترض أن TypeVariable له getId()
        // vars.insert(type1->asTypeVariable()->getId());
    }
    
    if (type2 && type2->getKind() == TypeKind::TypeParameter) {
        // vars.insert(type2->asTypeVariable()->getId());
    }
    
    return vars;
}

// ═══════════════════════════════════════════════════════════════════════════
//                    الدوال الخاصة / Private Functions
// ═══════════════════════════════════════════════════════════════════════════

SolverResult ParallelConstraintSolver::solveGroup(
    const std::vector<Constraint>& constraints,
    const std::vector<size_t>& indices,
    const Substitution& initialSubst) {
    
    // إنشاء مجموعة قيود للمجموعة الفرعية
    ConstraintSet subset;
    for (size_t idx : indices) {
        if (idx < constraints.size()) {
            subset.add(constraints[idx]);
        }
    }
    
    // حل المجموعة الفرعية
    ConstraintSolver solver;
    solver.setVerbose(config_.verbose);
    return solver.solve(subset, initialSubst);
}

std::shared_ptr<Substitution> ParallelConstraintSolver::mergeSubstitutions(
    const std::vector<std::shared_ptr<Substitution>>& substitutions) {
    
    if (substitutions.empty()) {
        return std::make_shared<Substitution>();
    }
    
    if (substitutions.size() == 1) {
        return substitutions[0];
    }
    
    // التحقق من عدم التعارض
    if (!checkConflicts(substitutions)) {
        return nullptr;
    }
    
    // دمج كل الاستبدالات
    auto merged = std::make_shared<Substitution>(*substitutions[0]);
    
    for (size_t i = 1; i < substitutions.size(); ++i) {
        auto composed = std::make_shared<Substitution>(merged->compose(*substitutions[i]));
        if (composed) {
            merged = composed;
        } else {
            return nullptr; // فشل الدمج
        }
    }
    
    return merged;
}

bool ParallelConstraintSolver::checkConflicts(
    const std::vector<std::shared_ptr<Substitution>>& substitutions) {
    
    // (AR) التحقق من عدم وجود تعيينات متعارضة لنفس المتغير
    // (EN) Check that no variable has conflicting assignments
    std::unordered_map<std::string, TypePtr> allMappings;
    
    for (const auto& subst : substitutions) {
        if (!subst) continue;
        for (const auto& varName : subst->getVariableNames()) {
            auto type = subst->lookup(varName);
            if (!type) continue;
            
            auto it = allMappings.find(varName);
            if (it != allMappings.end()) {
                // (AR) تحقق من التطابق — نفس المتغير يجب أن يحصل على نفس النوع
                // (EN) Check match — same variable must get same type
                if (it->second && !it->second->equals(type.get())) {
                    // (AR) تعارض: نفس المتغير له قيمتان مختلفتان
                    // (EN) Conflict: same variable has two different types
                    return false;
                }
            } else {
                allMappings[varName] = type;
            }
        }
    }
    
    return true;
}

} // namespace TypeSystem
} // namespace Sad
