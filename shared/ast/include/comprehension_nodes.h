/**
 * @file comprehension_nodes.h
 * @brief (AR) عقد AST للـ Comprehensions / (EN) AST Nodes for Comprehensions
 * 
 * (AR) هذا الملف يحتوي على تعريفات عقد AST لـ List/Dict/Set Comprehensions
 * (EN) This file contains AST node definitions for List/Dict/Set Comprehensions
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
#include <vector>

namespace Sad {
namespace AST {

// ============================================================================
// (AR) هيكل بند Comprehension / (EN) Comprehension Clause Structure
// ============================================================================

/**
 * @brief (AR) بند في comprehension متداخلة / (EN) Clause in nested comprehension
 * 
 * (AR) يمثل بند for..in إضافي في comprehension متداخلة
 * (EN) Represents an additional for..in clause in nested comprehension
 * 
 * مثال / Example:
 * @code
 * [x*y for x in [1,2] for y in [3,4]]
 *       ^^^^^^^^^^^  ^^^^^^^^^^^^
 *       main clause  nested clause
 * @endcode
 */
struct ComprehensionClause {
    std::string variable;                   // (AR) متغير الحلقة / (EN) Loop variable
    std::unique_ptr<Expr> iterable;        // (AR) المصدر المُكرَّر / (EN) Source iterable
    std::unique_ptr<Expr> condition;       // (AR) شرط اختياري / (EN) Optional condition (filter)
    
    ComprehensionClause(std::string var,
                       std::unique_ptr<Expr> iter,
                       std::unique_ptr<Expr> cond = nullptr)
        : variable(std::move(var)),
          iterable(std::move(iter)),
          condition(std::move(cond)) {}
};

// ============================================================================
// (AR) List Comprehension / (EN) List Comprehension
// ============================================================================

/**
 * @brief (AR) تعبير List Comprehension / (EN) List Comprehension Expression
 * 
 * (AR) يمثل بناء قائمة باستخدام comprehension syntax
 * (EN) Represents list building using comprehension syntax
 * 
 * أمثلة / Examples:
 * @code
 * [x * 2 for x in numbers]
 * [x for x in range(10) if x > 5]
 * [[x, y] for x in [1,2] for y in [3,4]]
 * @endcode
 * 
 * القواعد / Grammar:
 * @code
 * list_comp := '[' expr 'for' var 'in' iterable ['if' condition] ['for' ...]* ']'
 * @endcode
 */
class ListComprehensionExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;       // (AR) التعبير المُنتَج / (EN) Output expression (x * 2)
    std::string variable;                    // (AR) متغير الحلقة الرئيسية / (EN) Main loop variable (x)
    std::unique_ptr<Expr> iterable;         // (AR) المصدر الرئيسي / (EN) Main source (numbers)
    std::unique_ptr<Expr> condition;        // (AR) شرط التصفية (اختياري) / (EN) Filter condition (optional)
    std::vector<ComprehensionClause> nested_clauses;  // (AR) بنود متداخلة / (EN) Nested clauses
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     * 
     * @param expr (AR) التعبير المُنتَج / (EN) Output expression
     * @param var (AR) متغير الحلقة / (EN) Loop variable
     * @param iter (AR) المصدر / (EN) Iterable source
     * @param cond (AR) شرط اختياري / (EN) Optional condition
     */
    ListComprehensionExpr(std::unique_ptr<Expr> expr,
                         std::string var,
                         std::unique_ptr<Expr> iter,
                         std::unique_ptr<Expr> cond = nullptr)
        : expression(std::move(expr)),
          variable(std::move(var)),
          iterable(std::move(iter)),
          condition(std::move(cond)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    /**
     * @brief (AR) إضافة بند متداخل / (EN) Add nested clause
     */
    void addNestedClause(ComprehensionClause clause) {
        nested_clauses.push_back(std::move(clause));
    }
};

// ============================================================================
// (AR) Dict Comprehension / (EN) Dict Comprehension
// ============================================================================

/**
 * @brief (AR) تعبير Dict Comprehension / (EN) Dict Comprehension Expression
 * 
 * (AR) يمثل بناء قاموس باستخدام comprehension syntax
 * (EN) Represents dictionary building using comprehension syntax
 * 
 * أمثلة / Examples:
 * @code
 * {k: v*2 for k, v in items}
 * {x: x**2 for x in range(10) if x % 2 == 0}
 * @endcode
 * 
 * القواعد / Grammar:
 * @code
 * dict_comp := '{' key_expr ':' val_expr 'for' key_var ',' val_var 'in' iterable ['if' condition] '}'
 * @endcode
 */
class DictComprehensionExpr : public Expr {
public:
    std::unique_ptr<Expr> key_expression;   // (AR) تعبير المفتاح / (EN) Key expression
    std::unique_ptr<Expr> value_expression; // (AR) تعبير القيمة / (EN) Value expression
    std::string key_variable;                // (AR) متغير المفتاح / (EN) Key variable
    std::string value_variable;              // (AR) متغير القيمة / (EN) Value variable
    std::unique_ptr<Expr> iterable;         // (AR) المصدر / (EN) Iterable source
    std::unique_ptr<Expr> condition;        // (AR) شرط التصفية (اختياري) / (EN) Filter condition (optional)
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    DictComprehensionExpr(std::unique_ptr<Expr> key_expr,
                         std::unique_ptr<Expr> val_expr,
                         std::string key_var,
                         std::string val_var,
                         std::unique_ptr<Expr> iter,
                         std::unique_ptr<Expr> cond = nullptr)
        : key_expression(std::move(key_expr)),
          value_expression(std::move(val_expr)),
          key_variable(std::move(key_var)),
          value_variable(std::move(val_var)),
          iterable(std::move(iter)),
          condition(std::move(cond)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// ============================================================================
// (AR) Set Comprehension / (EN) Set Comprehension
// ============================================================================

/**
 * @brief (AR) تعبير Set Comprehension / (EN) Set Comprehension Expression
 * 
 * (AR) يمثل بناء مجموعة (set) باستخدام comprehension syntax
 * (EN) Represents set building using comprehension syntax
 * 
 * أمثلة / Examples:
 * @code
 * {x for x in numbers if x > 0}
 * {x*2 for x in range(10)}
 * @endcode
 * 
 * القواعد / Grammar:
 * @code
 * set_comp := '{' expr 'for' var 'in' iterable ['if' condition] '}'
 * @endcode
 */
class SetComprehensionExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;       // (AR) التعبير المُنتَج / (EN) Output expression
    std::string variable;                    // (AR) متغير الحلقة / (EN) Loop variable
    std::unique_ptr<Expr> iterable;         // (AR) المصدر / (EN) Iterable source
    std::unique_ptr<Expr> condition;        // (AR) شرط التصفية (اختياري) / (EN) Filter condition (optional)
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    SetComprehensionExpr(std::unique_ptr<Expr> expr,
                        std::string var,
                        std::unique_ptr<Expr> iter,
                        std::unique_ptr<Expr> cond = nullptr)
        : expression(std::move(expr)),
          variable(std::move(var)),
          iterable(std::move(iter)),
          condition(std::move(cond)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// ============================================================================
// (AR) Generator Expression / (EN) Generator Expression
// ============================================================================

/**
 * @brief (AR) تعبير Generator / (EN) Generator Expression
 * 
 * (AR) يمثل generator expression (يُقيَّم كسلياً)
 * (EN) Represents generator expression (lazy evaluation)
 * 
 * أمثلة / Examples:
 * @code
 * (x * 2 for x in numbers)
 * (x for x in range(1000000) if x % 2 == 0)
 * @endcode
 * 
 * ملاحظة / Note:
 * (AR) Generator expressions تُقيَّم كسلياً - تُنتج العناصر عند الطلب فقط
 * (EN) Generator expressions are lazily evaluated - produce items on-demand only
 * 
 * القواعد / Grammar:
 * @code
 * gen_expr := '(' expr 'for' var 'in' iterable ['if' condition] ')'
 * @endcode
 */
class GeneratorExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;       // (AR) التعبير المُنتَج / (EN) Output expression
    std::string variable;                    // (AR) متغير الحلقة / (EN) Loop variable
    std::unique_ptr<Expr> iterable;         // (AR) المصدر / (EN) Iterable source
    std::unique_ptr<Expr> condition;        // (AR) شرط التصفية (اختياري) / (EN) Filter condition (optional)
    std::vector<ComprehensionClause> nested_clauses;  // (AR) بنود متداخلة / (EN) Nested clauses
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    GeneratorExpr(std::unique_ptr<Expr> expr,
                 std::string var,
                 std::unique_ptr<Expr> iter,
                 std::unique_ptr<Expr> cond = nullptr)
        : expression(std::move(expr)),
          variable(std::move(var)),
          iterable(std::move(iter)),
          condition(std::move(cond)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
    
    void addNestedClause(ComprehensionClause clause) {
        nested_clauses.push_back(std::move(clause));
    }
};

} // namespace AST
} // namespace Sad
