// ======================================================================
// property_nodes.cpp - تنفيذ عقد الخصائص / Property Nodes Implementation
// ======================================================================

#include "../../../include/parser/ast/property_nodes.h"
#include "../../../include/parser/ast/ast_visitor.h"

namespace Sad {
namespace AST {

void PropertyDecl::accept(ASTVisitor& visitor) {
    visitor.visitPropertyDecl(*this);
}

} // namespace AST
} // namespace Sad
