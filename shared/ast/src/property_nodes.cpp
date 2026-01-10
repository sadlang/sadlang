// ======================================================================
// property_nodes.cpp - تنفيذ عقد الخصائص / Property Nodes Implementation
// ======================================================================

#include "property_nodes.h"
#include "ast_visitor.h"

namespace Sad {
namespace AST {

void PropertyDecl::accept(ASTVisitor& visitor) {
    visitor.visitPropertyDecl(*this);
}

} // namespace AST
} // namespace Sad
