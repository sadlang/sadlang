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
// (AR) Walrus Operator (:=) / (EN) Walrus Operator (:=)
// ============================================================================

/**
 * @brief (AR) تعبير Walrus (إسناد داخل تعبير) / (EN) Walrus Expression (Assignment Expression)
 * 
 * (AR) يسمح بإسناد قيمة لمتغير داخل تعبير
 * (EN) Allows assigning a value to a variable within an expression
 * 
 * أمثلة / Examples:
 * @code
 * if (x := getValue()) > 0 {
 *     print(x)  # x is available here
 * }
 * 
 * while (line := readLine()) != "" {
 *     process(line)
 * }
 * 
 * # Arabic
 * إذا (س := الحصول_على_القيمة()) > 0 {
 *     اطبع(س)
 * }
 * @endcode
 * 
 * ملاحظة / Note:
 * (AR) Walrus operator يختلف عن = العادي - يُعيد القيمة ويُسند في نفس الوقت
 * (EN) Walrus operator differs from regular = - returns value and assigns simultaneously
 * 
 * القواعد / Grammar:
 * @code
 * walrus_expr := identifier ':=' expression
 * @endcode
 */
class WalrusExpr : public Expr {
public:
    std::string variable;                   // (AR) اسم المتغير / (EN) Variable name
    std::unique_ptr<Expr> value;           // (AR) القيمة المُسنَدة / (EN) Assigned value
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     * 
     * @param var (AR) اسم المتغير / (EN) Variable name
     * @param val (AR) القيمة / (EN) Value expression
     * @param pos (AR) الموقع في الكود / (EN) Position in code
     */
    WalrusExpr(std::string var, std::unique_ptr<Expr> val, Position pos = Position())
        : Expr(pos),
          variable(std::move(var)),
          value(std::move(val)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};

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
class AwaitExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;      // (AR) التعبير المُنتظَر / (EN) Expression to await
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     * 
     * @param expr (AR) التعبير / (EN) Expression
     */
    explicit AwaitExpr(std::unique_ptr<Expr> expr)
        : expression(std::move(expr)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// ============================================================================
// (AR) Yield Expression / (EN) Yield Expression
// ============================================================================

/**
 * @brief (AR) تعبير Yield (في Generator) / (EN) Yield Expression (in Generator)
 * 
 * (AR) يُستخدم في دالة generator لإنتاج قيمة
 * (EN) Used in generator function to produce a value
 * 
 * أمثلة / Examples:
 * @code
 * function* counter() {
 *     var i = 0
 *     while (true) {
 *         yield i
 *         i = i + 1
 *     }
 * }
 * 
 * function* fibonacci() {
 *     var a = 0, b = 1
 *     yield a
 *     yield b
 *     while (true) {
 *         var c = a + b
 *         yield c
 *         a = b
 *         b = c
 *     }
 * }
 * @endcode
 * 
 * القواعد / Grammar:
 * @code
 * yield_expr := 'yield' [expression]
 * yield_from_expr := 'yield' 'from' expression
 * @endcode
 */
class YieldExpr : public Expr {
public:
    std::unique_ptr<Expr> value;           // (AR) القيمة المُنتَجة (اختياري) / (EN) Yielded value (optional)
    bool is_yield_from;                     // (AR) هل هو yield from؟ / (EN) Is it yield from?
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     * 
     * @param val (AR) القيمة / (EN) Value to yield
     * @param yield_from (AR) yield from / (EN) yield from flag
     */
    YieldExpr(std::unique_ptr<Expr> val = nullptr, bool yield_from = false)
        : value(std::move(val)),
          is_yield_from(yield_from) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// ============================================================================
// (AR) Lambda Expression / (EN) Lambda Expression
// ============================================================================

/**
 * @brief (AR) تعبير Lambda (دالة مجهولة) / (EN) Lambda Expression (Anonymous Function)
 * 
 * (AR) دالة مختصرة بدون اسم
 * (EN) Short anonymous function
 * 
 * أمثلة / Examples:
 * @code
 * var add = lambda(x, y) x + y
 * var square = lambda(x) x * x
 * 
 * var numbers = [1, 2, 3, 4, 5]
 * var doubled = map(lambda(x) x * 2, numbers)
 * 
 * # Arabic
 * دالة_مجهولة(س) س * 2
 * @endcode
 * 
 * القواعد / Grammar:
 * @code
 * lambda_expr := 'lambda' '(' [param_list] ')' expression
 * @endcode
 */
class LambdaExpr : public Expr {
public:
    std::vector<std::string> parameters;    // (AR) المعاملات / (EN) Parameters
    std::unique_ptr<Expr> body;            // (AR) جسم Lambda (تعبير واحد) / (EN) Lambda body (single expression)
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     * 
     * @param params (AR) المعاملات / (EN) Parameters
     * @param body_expr (AR) جسم Lambda / (EN) Lambda body
     */
    LambdaExpr(std::vector<std::string> params, std::unique_ptr<Expr> body_expr)
        : parameters(std::move(params)),
          body(std::move(body_expr)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};

// ============================================================================
// (AR) Spread/Unpacking Expression / (EN) Spread/Unpacking Expression
// ============================================================================

/**
 * @brief (AR) تعبير Spread/Unpacking (*args، **kwargs) / (EN) Spread/Unpacking Expression
 * 
 * (AR) يُستخدم لتفكيك مصفوفة أو قاموس
 * (EN) Used to unpack array or dictionary
 * 
 * أمثلة / Examples:
 * @code
 * var list1 = [1, 2, 3]
 * var list2 = [*list1, 4, 5, 6]  # [1, 2, 3, 4, 5, 6]
 * 
 * function add(a, b, c) { return a + b + c }
 * var args = [1, 2, 3]
 * var result = add(*args)  # add(1, 2, 3)
 * 
 * var dict1 = {a: 1, b: 2}
 * var dict2 = {**dict1, c: 3}  # {a: 1, b: 2, c: 3}
 * @endcode
 * 
 * القواعد / Grammar:
 * @code
 * spread_expr := '*' expression    # Array spread
 * spread_expr := '**' expression   # Dict spread
 * @endcode
 */
class SpreadExpr : public Expr {
public:
    std::unique_ptr<Expr> expression;      // (AR) التعبير المُفكَّك / (EN) Expression to spread
    bool is_dict_spread;                    // (AR) هل هو ** (قاموس)؟ / (EN) Is it ** (dict)?
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     * 
     * @param expr (AR) التعبير / (EN) Expression
     * @param dict_spread (AR) تفكيك قاموس؟ / (EN) Dict spread?
     */
    SpreadExpr(std::unique_ptr<Expr> expr, bool dict_spread = false)
        : expression(std::move(expr)),
          is_dict_spread(dict_spread) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visit(*this);
    }
};

} // namespace AST
} // namespace Sad
