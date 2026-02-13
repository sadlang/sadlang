/**
 * ═══════════════════════════════════════════════════════════════════════════════
 * ╔═══════════════════════════════════════════════════════════════════════════╗
 * ║                       محلل القيود - لغة ص                                  ║
 * ║                  Constraint Solver - Sad Language                         ║
 * ╚═══════════════════════════════════════════════════════════════════════════╝
 * ═══════════════════════════════════════════════════════════════════════════════
 */

#ifndef SAD_COMPILER_TYPES_CONSTRAINT_SOLVER_HPP
#define SAD_COMPILER_TYPES_CONSTRAINT_SOLVER_HPP

#include "constraint.h"
#include "substitution.h"
#include <vector>
#include <memory>

namespace Sad {
namespace TypeSystem {

/**
 * محلل القيود النوعية
 * Type constraint solver
 */
class ConstraintSolver {
public:
    ConstraintSolver() = default;
    
    /**
     * حل مجموعة من القيود
     * Solve a set of constraints
     */
    bool solve(const std::vector<Constraint>& constraints, Substitution& result);
    
    /**
     * التحقق من تناسق القيود
     * Check constraint consistency
     */
    bool areConsistent(const std::vector<Constraint>& constraints);
    
private:
    bool unify(TypePtr t1, TypePtr t2, Substitution& subst);
};

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPES_CONSTRAINT_SOLVER_HPP
