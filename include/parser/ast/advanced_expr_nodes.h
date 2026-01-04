/**
 * @file advanced_expr_nodes.h
 * @brief (AR) عقد التعبيرات المتقدمة / (EN) Advanced Expression Nodes
 * 
 * (AR) هذا الملف يحتوي على عقد AST للتعبيرات المتقدمة (Walrus، Async، إلخ)
 * (EN) This file contains AST nodes for advanced expressions (Walrus, Async, etc.)
 * 
 * @author Sad Language Team
 * @date December 19, 2025
 * @version 2.0.0
 */

#pragma once

#include "ast_node.h"
#include "ast_visitor.h"
#include <memory>
#include <string>

namespace Sad {
namespace AST {

// ============================================================================
// (AR) Await Expression / (EN) Await Expression
// ============================================================================

/**
 * @brief (AR) تعبير Await (انتظار async function) / (EN) Await Expression (wait for async function)
 * 
 * (AR) يُستخدم لانتظار نتيجة دالة async
 * (EN) Used to wait for result of an async function
 * 
 * أمثلة / Examples:
 * @code
 * var result = await fetchData()
 * var data = await readFile("data.txt")
 * 
 * # Arabic
 * رقم النتيجة = انتظر جلب_البيانات()
 * @endcode
 * 
 * ملاحظة / Note:
 * (AR) await يمكن استخدامه فقط داخل دالة async
 * (EN) await can only be used inside an async function
 * 
 * القواعد / Grammar:
 * @code
 * await_expr := 'await' expression
 * @endcode
 */
class AwaitExpr : public Expression {
public:
    ExprPtr expression;      // (AR) التعبير المُنتظَر / (EN) Expression to await
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     * 
     * @param expr (AR) التعبير / (EN) Expression
     * @param pos (AR) الموقع في الكود / (EN) Position in code
     */
    explicit AwaitExpr(ExprPtr expr, const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), expression(std::move(expr)) {}
    
    /**
     * @brief (AR) قبول الزائر / (EN) Accept visitor
     * @param visitor (AR) الزائر / (EN) Visitor
     */
    void accept(ASTVisitor& visitor) override {
        visitor.visitAwaitExpr(*this);  // (AR) استدعاء visitAwaitExpr المعرّف في ast_visitor.h / (EN) Call visitAwaitExpr defined in ast_visitor.h
    }
    
    std::string toString() const override {
        return "(await " + expression->toString() + ")";
    }
    
    Data::DataType getType() const override {
        return expression->getType();  // (AR) نوع await هو نفس نوع التعبير المُنتظَر / (EN) await type is same as awaited expression
    }
};

} // namespace AST
} // namespace Sad
