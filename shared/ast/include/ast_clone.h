/**
 * ═══════════════════════════════════════════════════════════════════════════
 * ملف: ast_clone.h
 * الوصف: واجهة استنساخ عُقد AST العميق
 * ═══════════════════════════════════════════════════════════════════════════
 *
 * @file ast_clone.h
 * @brief (AR) دوال استنساخ عميق لعُقد شجرة التركيب المجرد
 * @brief (EN) Deep clone free-functions for AST nodes
 *
 * (AR) يوفر دالتين رئيسيتين: cloneExpression و cloneStatement
 *      تستخدمان dynamic_cast داخلياً لمطابقة النوع الفعلي ونسخه.
 *      متوافق مع وضع freestanding — لا يرمي استثناءات.
 *      الأنواع غير المدعومة تُرجع nullptr.
 *
 * (EN) Provides two main functions: cloneExpression and cloneStatement
 *      Uses dynamic_cast internally to match the actual type and copy it.
 *      Freestanding compatible — does not throw exceptions.
 *      Unsupported types return nullptr.
 *
 * @author Sad Language Development Team
 * @date 2025
 */

#ifndef SAD_AST_CLONE_H
#define SAD_AST_CLONE_H

#include "ast_node.h"

namespace Sad {
namespace AST {

/**
 * @brief (AR) استنساخ عميق لتعبير AST
 * @brief (EN) Deep clone an AST expression
 * @param expr (const Expression&) — (AR) التعبير المراد نسخه
 *                                   (EN) The expression to clone
 * @return (ExprPtr) — (AR) نسخة مستقلة بالكامل، أو nullptr إذا كان النوع غير مدعوم
 *                     (EN) Fully independent copy, or nullptr for unsupported types
 */
ExprPtr cloneExpression(const Expression& expr);

/**
 * @brief (AR) استنساخ عميق لعبارة AST
 * @brief (EN) Deep clone an AST statement
 * @param stmt (const Statement&) — (AR) العبارة المراد نسخها
 *                                   (EN) The statement to clone
 * @return (StmtPtr) — (AR) نسخة مستقلة بالكامل، أو nullptr إذا كان النوع غير مدعوم
 *                     (EN) Fully independent copy, or nullptr for unsupported types
 */
StmtPtr cloneStatement(const Statement& stmt);

/**
 * @brief (AR) مساعد: استنساخ ExprPtr (يتعامل مع nullptr)
 * @brief (EN) Helper: clone an ExprPtr (handles nullptr)
 */
inline ExprPtr cloneExprPtr(const ExprPtr& e) {
    return e ? cloneExpression(*e) : nullptr;
}

/**
 * @brief (AR) مساعد: استنساخ StmtPtr (يتعامل مع nullptr)
 * @brief (EN) Helper: clone a StmtPtr (handles nullptr)
 */
inline StmtPtr cloneStmtPtr(const StmtPtr& s) {
    return s ? cloneStatement(*s) : nullptr;
}

} // namespace AST
} // namespace Sad

#endif // SAD_AST_CLONE_H
