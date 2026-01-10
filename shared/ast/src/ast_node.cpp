/**
 * @file ast_node.cpp
 * @brief Implementation of base AST node classes / تنفيذ فئات عُقد AST الأساسية
 * @author Sad Language Team
 * @date 11 November 2025
 */

#include "ast_node.h"
// #include "debug/debug.h"  // TODO: Create debug.h if needed

namespace Sad {
namespace AST {

// Implementation note: Base classes are mostly abstract,
// so there's minimal implementation here. Most functionality
// is in the derived classes (expressions.cpp, statements.cpp, etc.)
//
// ملاحظة: الفئات الأساسية مجردة في معظمها،
// لذا لا يوجد الكثير من التنفيذ هنا. معظم الوظائف
// في الفئات المشتقة (expressions.cpp, statements.cpp, إلخ.)

// If needed, we can add utility functions here:
//
// std::string ASTNode::getPositionString() const {
//     return position.toString();
// }

} // namespace AST
} // namespace Sad
