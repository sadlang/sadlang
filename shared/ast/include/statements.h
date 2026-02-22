/**
 * @file statements.h
 * @brief Statement AST node definitions / تعريفات عُقد AST للجمل
 * @author Sad Language Team
 * @date 11 November 2025
 */

#ifndef SAD_AST_STATEMENTS_H
#define SAD_AST_STATEMENTS_H

#include "ast_node.h"
#include "ast_visitor.h"
#include "expressions.h"
#include <vector>
#include <string>

namespace Sad {
namespace AST {

// Forward declarations
class VarDeclStmt;

// =========================================================================
// Expression Statement / جملة التعبير
// =========================================================================

/**
 * @brief Expression statement node / عقدة جملة التعبير
 * 
 * Represents an expression used as a statement.
 * يمثل تعبير مستخدم كجملة.
 * 
 * @example Examples / أمثلة:
 * - print("hello")
 * - x + 5
 * - obj.method()
 */
class ExprStmt : public Statement {
public:
    ExprPtr expression;     ///< The expression / التعبير
    
    /**
     * @brief Constructor / البناء
     * @param expr Expression / التعبير
     */
    explicit ExprStmt(ExprPtr expr)
        : Statement(expr->position), expression(std::move(expr)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitExprStmt(*this);
    }
    
    std::string toString() const override {
        return expression->toString() + ";";
    }
};

// =========================================================================
// Variable Declaration Statement / جملة تصريح المتغير
// =========================================================================

/**
 * @brief Variable declaration statement node / عقدة جملة تصريح المتغير
 * 
 * Represents a variable declaration with optional initialization.
 * يمثل تصريح متغير مع إعداد اختياري.
 * 
 * @example Examples / أمثلة:
 * - var x = 10
 * - let name: string = "أحمد"
 * - const PI = 3.14
 */
class VarDeclStmt : public Statement {
public:
    std::string name;           ///< Variable name / اسم المتغير
    Data::DataType type;        ///< Variable type / نوع المتغير
    ExprPtr initializer;        ///< Initial value (optional) / القيمة الأولية
    bool isConst;               ///< Is constant? / ثابت؟
    
    /**
     * @brief Constructor / البناء
     * @param n Variable name / اسم المتغير
     * @param t Variable type / نوع المتغير
     * @param init Initializer expression / تعبير الإعداد
     * @param isConst Is constant / ثابت
     * @param pos Source position / الموقع في الكود
     */
    VarDeclStmt(const std::string& n, Data::DataType t, ExprPtr init,
                bool isConst = false, const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), name(n), type(t), 
          initializer(std::move(init)), isConst(isConst) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitVarDeclStmt(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// If Statement / جملة إذا
// =========================================================================

/**
 * @brief If statement node / عقدة جملة إذا
 * 
 * Represents an if-else conditional statement.
 * يمثل جملة شرطية إذا-وإلا.
 * 
 * @example Examples / أمثلة:
 * - if (x > 0) { ... }
 * - if (condition) { ... } else { ... }
 * - إذا (الشرط) { ... } وإلا { ... }
 */
class IfStmt : public Statement {
public:
    ExprPtr condition;          ///< Condition expression / تعبير الشرط
    StmtPtr thenBranch;         ///< Then branch / فرع إذا صح
    StmtPtr elseBranch;         ///< Else branch (optional) / فرع وإلا
    
    /**
     * @brief Constructor / البناء
     * @param cond Condition / الشرط
     * @param thenBr Then branch / فرع إذا صح
     * @param elseBr Else branch / فرع وإلا
     * @param pos Source position / الموقع في الكود
     */
    IfStmt(ExprPtr cond, StmtPtr thenBr, StmtPtr elseBr = nullptr,
           const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), condition(std::move(cond)), 
          thenBranch(std::move(thenBr)), elseBranch(std::move(elseBr)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitIfStmt(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// While Statement / جملة بينما
// =========================================================================

/**
 * @brief While loop statement node / عقدة جملة حلقة بينما
 * 
 * Represents a while loop.
 * يمثل حلقة بينما.
 * 
 * @example Examples / أمثلة:
 * - while (x < 10) { ... }
 * - بينما (العداد < 100) { ... }
 */
class WhileStmt : public Statement {
public:
    ExprPtr condition;          ///< Loop condition / شرط الحلقة
    StmtPtr body;               ///< Loop body / جسم الحلقة
    
    /**
     * @brief Constructor / البناء
     * @param cond Condition / الشرط
     * @param body Body statement / جملة الجسم
     * @param pos Source position / الموقع في الكود
     */
    WhileStmt(ExprPtr cond, StmtPtr body,
              const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), condition(std::move(cond)), body(std::move(body)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitWhileStmt(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// For Statement / جملة لكل
// =========================================================================

/**
 * @brief For loop statement node (C-style) / عقدة جملة حلقة لكل
 * 
 * Represents a C-style for loop.
 * يمثل حلقة لكل بأسلوب C.
 * 
 * @example Examples / أمثلة:
 * - for (var i = 0; i < 10; i++) { ... }
 * - for (initialization; condition; increment) { ... }
 */
class ForStmt : public Statement {
public:
    StmtPtr initializer;        ///< Initializer / التهيئة
    ExprPtr condition;          ///< Condition / الشرط
    ExprPtr increment;          ///< Increment / الزيادة
    StmtPtr body;               ///< Loop body / جسم الحلقة
    
    /**
     * @brief Constructor / البناء
     */
    ForStmt(StmtPtr init, ExprPtr cond, ExprPtr incr, StmtPtr body,
            const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), initializer(std::move(init)), 
          condition(std::move(cond)), increment(std::move(incr)), 
          body(std::move(body)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitForStmt(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// For-Range Statement / جملة لكل في
// =========================================================================

/**
 * @brief For-range loop statement node (Python-style) / عقدة جملة حلقة لكل في
 * 
 * Represents a for-each/range-based loop.
 * يمثل حلقة لكل عنصر في مجموعة.
 * 
 * @example Examples / أمثلة:
 * - for (x in array) { ... }
 * - لكل (عنصر في القائمة) { ... }
 * - for (key, value in dict) { ... }
 */
class ForRangeStmt : public Statement {
public:
    std::string variable;       ///< Loop variable / متغير الحلقة
    std::string valueVar;       ///< Value variable (for dict, optional) / متغير القيمة
    ExprPtr iterable;           ///< Iterable expression / التعبير القابل للتكرار
    StmtPtr body;               ///< Loop body / جسم الحلقة
    
    /**
     * @brief Constructor / البناء
     */
    ForRangeStmt(const std::string& var, ExprPtr iter, StmtPtr body,
                 const std::string& valVar = "",
                 const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), variable(var), valueVar(valVar),
          iterable(std::move(iter)), body(std::move(body)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitForRangeStmt(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Return Statement / جملة إرجاع
// =========================================================================

/**
 * @brief Return statement node / عقدة جملة إرجاع
 * 
 * Represents a return statement in a function.
 * يمثل جملة إرجاع في دالة.
 * 
 * @example Examples / أمثلة:
 * - return x
 * - return 42
 * - أرجع القيمة
 */
class ReturnStmt : public Statement {
public:
    ExprPtr value;              ///< Return value (optional) / قيمة الإرجاع
    
    /**
     * @brief Constructor / البناء
     * @param val Return value / قيمة الإرجاع
     * @param pos Source position / الموقع في الكود
     */
    ReturnStmt(ExprPtr val = nullptr, const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), value(std::move(val)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitReturnStmt(*this);
    }
    
    std::string toString() const override {
        return value ? "return " + value->toString() + ";" : "return;";
    }
};

// =========================================================================
// Yield Statement / جملة الإعطاء (المولّدات)
// =========================================================================

/**
 * @brief Yield statement node / عقدة جملة yield
 * 
 * Represents a yield statement in generator functions.
 * يمثل جملة yield في الدوال المولّدة.
 * 
 * Supports two forms:
 * - yield expr         : yields a single value
 * - yield from iterable: delegates to another generator
 * 
 * يدعم صيغتين:
 * - yield expr         : تُعطي قيمة واحدة
 * - yield from iterable: تفوّض إلى مولّد آخر
 * 
 * @example Examples / أمثلة:
 * - yield 42
 * - yield x * 2
 * - yield from range(10)
 * - اعطِ 100
 * - اعطِ from قائمة
 */
class YieldStmt : public Statement {
public:
    ExprPtr value;              ///< Yielded value / القيمة المُعطاة
    bool isYieldFrom;           ///< Is 'yield from'? / هل 'yield from'؟
    
    /**
     * @brief Constructor for simple yield / البناء لـ yield بسيط
     * @param val Yielded value / القيمة المُعطاة
     * @param pos Source position / الموقع في الكود
     */
    YieldStmt(ExprPtr val = nullptr, const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), value(std::move(val)), isYieldFrom(false) {}
    
    /**
     * @brief Constructor for yield from / البناء لـ yield from
     * @param val Iterable expression / تعبير قابل للتكرار
     * @param yieldFrom Flag indicating 'yield from' / علامة تدل على 'yield from'
     * @param pos Source position / الموقع في الكود
     */
    YieldStmt(ExprPtr val, bool yieldFrom, const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), value(std::move(val)), isYieldFrom(yieldFrom) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitYieldStmt(*this);
    }
    
    std::string toString() const override {
        if (isYieldFrom) {
            return value ? "yield from " + value->toString() + ";" : "yield from;";
        }
        return value ? "yield " + value->toString() + ";" : "yield;";
    }
};

// =========================================================================
// With Statement (Context Manager) / جملة باستخدام (مدير السياق)
// =========================================================================

/**
 * @brief With statement node for context managers / عقدة جملة باستخدام لمديري السياق
 * 
 * Represents a context manager statement that ensures proper resource management.
 * يمثل جملة مدير السياق التي تضمن إدارة الموارد بشكل صحيح.
 * 
 * The context manager calls __دخول__() when entering the block and __خروج__() when leaving.
 * يستدعي مدير السياق __دخول__() عند الدخول و__خروج__() عند الخروج.
 * 
 * Syntax / الصيغة:
 * - باستخدام expr كـ var: body نهاية_استخدام
 * - باستخدام resource = factory() كـ r: ... نهاية_استخدام
 * 
 * @example Examples / أمثلة:
 * @code{.s}
 * باستخدام ملف = افتح("data.txt") كـ م
 *     محتوى = م.اقرأ()
 * نهاية_استخدام
 * 
 * // With multiple resources / مع موارد متعددة
 * باستخدام افتح("input.txt") كـ دخل
 *     باستخدام افتح("output.txt", "w") كـ خرج
 *         خرج.اكتب(دخل.اقرأ())
 *     نهاية_استخدام
 * نهاية_استخدام
 * @endcode
 */
class WithStmt : public Statement {
public:
    ExprPtr resource;           ///< Resource expression / تعبير المورد
    std::string alias;          ///< Alias name (after كـ) / الاسم المستعار (بعد كـ)
    StmtPtr body;               ///< Body of the with block / جسم كتلة الاستخدام
    
    /**
     * @brief Constructor / البناء
     * @param res Resource expression / تعبير المورد
     * @param aliasName Alias for the resource / اسم مستعار للمورد
     * @param bodyStmt Body statements / جمل الجسم
     * @param pos Source position / الموقع في الكود
     */
    WithStmt(ExprPtr res, const std::string& aliasName, StmtPtr bodyStmt,
             const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), resource(std::move(res)), alias(aliasName),
          body(std::move(bodyStmt)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitWithStmt(*this);
    }
    
    std::string toString() const override {
        std::string result = "باستخدام " + resource->toString();
        if (!alias.empty()) {
            result += " كـ " + alias;
        }
        result += "\n" + body->toString() + "\nنهاية_استخدام";
        return result;
    }
};

// =========================================================================
// Break Statement / جملة خروج
// =========================================================================

/**
 * @brief Break statement node / عقدة جملة خروج
 * 
 * Represents a break statement in a loop.
 * يمثل جملة خروج من حلقة.
 * 
 * @example Examples / أمثلة:
 * - break
 * - اخرج
 */
class BreakStmt : public Statement {
public:
    /**
     * @brief Constructor / البناء
     * @param pos Source position / الموقع في الكود
     */
    explicit BreakStmt(const Lexer::Position& pos = Lexer::Position())
        : Statement(pos) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitBreakStmt(*this);
    }
    
    std::string toString() const override {
        return "break;";
    }
};

// =========================================================================
// Continue Statement / جملة تابع
// =========================================================================

/**
 * @brief Continue statement node / عقدة جملة تابع
 * 
 * Represents a continue statement in a loop.
 * يمثل جملة متابعة للتكرار التالي في حلقة.
 * 
 * @example Examples / أمثلة:
 * - continue
 * - تابع
 */
class ContinueStmt : public Statement {
public:
    /**
     * @brief Constructor / البناء
     * @param pos Source position / الموقع في الكود
     */
    explicit ContinueStmt(const Lexer::Position& pos = Lexer::Position())
        : Statement(pos) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitContinueStmt(*this);
    }
    
    std::string toString() const override {
        return "continue;";
    }
};

// =========================================================================
// Block Statement / جملة الكتلة
// =========================================================================

/**
 * @brief Block statement node / عقدة جملة الكتلة
 * 
 * Represents a block of statements enclosed in braces.
 * يمثل كتلة من الجمل محاطة بأقواس معقوفة.
 * 
 * @example Examples / أمثلة:
 * - { stmt1; stmt2; stmt3; }
 */
class BlockStmt : public Statement {
public:
    StmtList statements;        ///< Statements in the block / الجمل في الكتلة
    
    /**
     * @brief Constructor / البناء
     * @param stmts Statement list / قائمة الجمل
     * @param pos Source position / الموقع في الكود
     */
    BlockStmt(StmtList stmts, const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), statements(std::move(stmts)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitBlockStmt(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Try-Catch Statement / جملة محاولة-التقاط
// =========================================================================

/**
 * @brief Catch clause / بند الالتقاط
 */
struct CatchClause {
    std::string exceptionVar;       ///< Exception variable name / اسم متغير الاستثناء
    Data::DataType exceptionType;   ///< Exception type / نوع الاستثناء
    std::string exceptionTypeName;  ///< Custom exception class name / اسم صنف الاستثناء المخصص
    StmtPtr body;                   ///< Catch body / جسم الالتقاط
    
    CatchClause(const std::string& var, Data::DataType type, StmtPtr body,
                const std::string& typeName = "")
        : exceptionVar(var), exceptionType(type), exceptionTypeName(typeName),
          body(std::move(body)) {}
    
    // Copy constructor deleted (contains unique_ptr)
    CatchClause(const CatchClause&) = delete;
    CatchClause& operator=(const CatchClause&) = delete;
    
    // Move constructor and assignment
    CatchClause(CatchClause&&) = default;
    CatchClause& operator=(CatchClause&&) = default;
};

/**
 * @brief Try-catch statement node / عقدة جملة محاولة-التقاط
 * 
 * Represents a try-catch exception handling block.
 * يمثل كتلة معالجة الاستثناءات محاولة-التقاط.
 * 
 * @example Examples / أمثلة:
 * - try { ... } catch (e) { ... }
 * - حاول { ... } التقط (خطأ) { ... }
 * - try { ... } catch (e) { ... } finally { ... }
 */
class TryStmt : public Statement {
public:
    StmtPtr tryBlock;               ///< Try block / كتلة المحاولة
    std::vector<CatchClause> catchClauses; ///< Catch clauses / بنود الالتقاط
    StmtPtr finallyBlock;           ///< Finally block (optional) / كتلة أخيراً
    
    /**
     * @brief Constructor / البناء
     */
    TryStmt(StmtPtr tryBlk, std::vector<CatchClause> catches,
            StmtPtr finallyBlk = nullptr,
            const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), tryBlock(std::move(tryBlk)), 
          catchClauses(std::move(catches)), finallyBlock(std::move(finallyBlk)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitTryStmt(*this);
    }
    
    std::string toString() const override;
};

// =========================================================================
// Raise/Throw Statement / جملة رفع الاستثناء
// =========================================================================

/**
 * @brief Raise/throw statement node / عقدة جملة رفع الاستثناء
 * 
 * Represents a raise/throw exception statement.
 * يمثل جملة رفع استثناء.
 * 
 * @example Examples / أمثلة:
 * - raise Exception("error")
 * - throw new Error("خطأ")
 * - ارفع خطأ("رسالة")
 */
class RaiseStmt : public Statement {
public:
    ExprPtr exception;          ///< Exception expression / تعبير الاستثناء
    
    /**
     * @brief Constructor / البناء
     * @param exc Exception expression / تعبير الاستثناء
     * @param pos Source position / الموقع في الكود
     */
    RaiseStmt(ExprPtr exc, const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), exception(std::move(exc)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitRaiseStmt(*this);
    }
    
    std::string toString() const override {
        return "raise " + exception->toString() + ";";
    }
};

// =========================================================================
// Switch Statement / جملة حالة (Switch/Case)
// =========================================================================

/**
 * @brief Case branch for switch statement / فرع حالة لجملة حالة
 * 
 * Represents a single case branch في switch statement.
 * يمثل فرع حالة واحد في جملة حالة.
 * 
 * Spec: docs/language_spec/rules/04_syntax.md - switch_stmt
 */
struct CaseBranch {
    ExprPtr value;              ///< Case value / قيمة الحالة
    StmtPtr body;               ///< Case body / جسم الحالة
    
    CaseBranch(ExprPtr val, StmtPtr bod)
        : value(std::move(val)), body(std::move(bod)) {}
};

/**
 * @brief Switch statement node / عقدة جملة حالة
 * 
 * Represents a switch-case statement with parenthesized condition.
 * يمثل جملة حالة-عندما مع شرط بين أقواس.
 * 
 * Spec: docs/language_spec/rules/04_syntax.md
 * Syntax: حالة (expr) عندما val1: body1 عندما val2: body2 افتراضي: default_body نهاية
 * 
 * Features / الميزات:
 * - No fall-through: executes only first matching case / لا يوجد fall-through
 * - Default case is optional / الحالة الافتراضية اختيارية
 * - Values can be any expression / القيم يمكن أن تكون أي تعبير
 * 
 * @example Examples / أمثلة:
 * @code{.s}
 * حالة (يوم)
 *     عندما 1:
 *         اطبع("الإثنين")
 *     عندما 2:
 *         اطبع("الثلاثاء")
 *     افتراضي:
 *         اطبع("يوم آخر")
 * نهاية
 * 
 * حالة (اللون)
 *     عندما "أحمر": اطبع("Red")
 *     عندما "أزرق": اطبع("Blue")
 * نهاية
 * @endcode
 */
class SwitchStmt : public Statement {
public:
    ExprPtr expression;                 ///< Switch expression / تعبير الحالة
    std::vector<CaseBranch> cases;      ///< Case branches / فروع الحالات
    StmtPtr defaultCase;                ///< Default case (optional) / الحالة الافتراضية
    
    /**
     * @brief Constructor / البناء
     * @param expr Switch expression / تعبير الحالة
     * @param caseList Case branches / فروع الحالات
     * @param defCase Default case / الحالة الافتراضية
     * @param pos Source position / الموقع في الكود
     */
    SwitchStmt(ExprPtr expr, std::vector<CaseBranch> caseList,
               StmtPtr defCase = nullptr,
               const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), expression(std::move(expr)),
          cases(std::move(caseList)), defaultCase(std::move(defCase)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitSwitchStmt(*this);
    }
    
    std::string toString() const override;
};

} // namespace AST
} // namespace Sad

#endif // SAD_AST_STATEMENTS_H
