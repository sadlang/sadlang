/**
 * @file expressions.h
 * @brief Expression AST node definitions / تعريفات عُقد AST للتعابير
 * @author Sad Language Team
 * @date 11 November 2025
 */

#ifndef SAD_AST_EXPRESSIONS_H
#define SAD_AST_EXPRESSIONS_H

#include "ast_node.h"
#include "ast_visitor.h"
#include "../../lexer/token.h"
#include <vector>
#include <string>

namespace Sad {
namespace AST {

// =========================================================================
// Binary Expression / التعبير الثنائي
// =========================================================================

/**
 * @brief Binary expression node (e.g., a + b, x * y) / عقدة التعبير الثنائي
 * 
 * Represents operations with two operands and an operator.
 * يمثل العمليات مع معاملين وعامل واحد.
 * 
 * @example Examples / أمثلة:
 * - Arithmetic: 2 + 3, x * y, a / b
 * - Comparison: x > 5, name == "أحمد"
 * - Logical: flag && true, x || y
 * - Bitwise: bits & 0xFF, value << 2
 */
class BinaryExpr : public Expression {
public:
    ExprPtr left;           ///< Left operand / المعامل الأيسر
    Lexer::TokenType op;    ///< Operator / العامل
    ExprPtr right;          ///< Right operand / المعامل الأيمن
    
    /**
     * @brief Constructor / البناء
     * @param l Left operand / المعامل الأيسر
     * @param o Operator type / نوع العامل
     * @param r Right operand / المعامل الأيمن
     * @param pos Source position / الموقع في الكود
     */
    BinaryExpr(ExprPtr l, Lexer::TokenType o, ExprPtr r, 
               const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), left(std::move(l)), op(o), right(std::move(r)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitBinaryExpr(*this);
    }
    
    std::string toString() const override {
        return "(" + left->toString() + " " + 
               Lexer::Token::typeToString(op) + " " + 
               right->toString() + ")";
    }
    
    Data::DataType getType() const override;
};

// =========================================================================
// Unary Expression / التعبير الأحادي
// =========================================================================

/**
 * @brief Unary expression node (e.g., -x, !flag) / عقدة التعبير الأحادي
 * 
 * Represents operations with one operand and an operator.
 * يمثل العمليات مع معامل واحد وعامل واحد.
 * 
 * @example Examples / أمثلة:
 * - Arithmetic: -x, +value
 * - Logical: !flag, !condition
 * - Bitwise: ~bits
 */
class UnaryExpr : public Expression {
public:
    Lexer::TokenType op;    ///< Operator / العامل
    ExprPtr operand;        ///< Operand / المعامل
    
    /**
     * @brief Constructor / البناء
     * @param o Operator type / نوع العامل
     * @param operand The operand / المعامل
     * @param pos Source position / الموقع في الكود
     */
    UnaryExpr(Lexer::TokenType o, ExprPtr operand, 
              const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), op(o), operand(std::move(operand)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitUnaryExpr(*this);
    }
    
    std::string toString() const override {
        return "(" + Lexer::Token::typeToString(op) + operand->toString() + ")";
    }
    
    Data::DataType getType() const override;
};

// =========================================================================
// Ternary Expression / التعبير الثلاثي
// =========================================================================

/**
 * @brief Ternary conditional expression node (condition ? true_val : false_val)
 *        عقدة التعبير الشرطي الثلاثي
 * 
 * Represents ternary conditional operator: condition ? trueExpr : falseExpr
 * يمثل العامل الشرطي الثلاثي: شرط ? قيمة_صحيح : قيمة_خطأ
 * 
 * Spec: docs/language_spec/rules/04_syntax.md - ternary operator
 * 
 * @example Examples / أمثلة:
 * @code{.s}
 * متغير نتيجة = (عدد > 10) ? "كبير" : "صغير"
 * متغير أكبر = (أ > ب) ? أ : ب
 * @endcode
 */
class TernaryExpr : public Expression {
public:
    ExprPtr condition;      ///< Condition expression / تعبير الشرط
    ExprPtr trueExpr;       ///< True value expression / تعبير القيمة الصحيحة
    ExprPtr falseExpr;      ///< False value expression / تعبير القيمة الخاطئة
    
    /**
     * @brief Constructor / البناء
     * @param cond Condition / الشرط
     * @param trueE True expression / تعبير الحالة الصحيحة
     * @param falseE False expression / تعبير الحالة الخاطئة
     * @param pos Source position / الموقع في الكود
     */
    TernaryExpr(ExprPtr cond, ExprPtr trueE, ExprPtr falseE,
                const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), condition(std::move(cond)),
          trueExpr(std::move(trueE)), falseExpr(std::move(falseE)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitTernaryExpr(*this);
    }
    
    std::string toString() const override {
        return "(" + condition->toString() + " ? " +
               trueExpr->toString() + " : " +
               falseExpr->toString() + ")";
    }
    
    Data::DataType getType() const override;
};

// =========================================================================
// Literal Expression / التعبير الحرفي
// =========================================================================

/**
 * @brief Literal expression node (e.g., 42, "text", true) / عقدة التعبير الحرفي
 * 
 * Represents constant literal values in the source code.
 * يمثل القيم الحرفية الثابتة في الكود المصدري.
 *
 * @example Examples / أمثلة:
 * - Integer: 42, -100, 0xFF
 * - Float: 3.14, -0.5, 1.5e10
 * - String: "مرحباً", "hello"
 * - Boolean: true, false, صحيح, خطأ
 * - None: none, null, لاشيء
 */
class LiteralExpr : public Expression {
public:
    Lexer::Token token;     ///< The literal token / الرمز الحرفي
    
    /**
     * @brief Constructor / البناء
     * @param tok Literal token / الرمز الحرفي
     */
    explicit LiteralExpr(const Lexer::Token& tok)
        : Expression(tok.getPosition()), token(tok) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitLiteralExpr(*this);
    }
    
    std::string toString() const override {
        return token.getValue();
    }
    
    Data::DataType getType() const override;
};

// =========================================================================
// Variable Expression / تعبير المتغير
// =========================================================================

/**
 * @brief Variable expression node (identifier) / عقدة تعبير المتغير
 * 
 * Represents a variable reference by its identifier.
 * يمثل مرجع متغير باسمه المعرِّف.
 * 
 * @example Examples / أمثلة:
 * - x, counter, index
 * - اسم, العمر, العداد
 */
class VariableExpr : public Expression {
public:
    std::string name;       ///< Variable name / اسم المتغير
    
    /**
     * @brief Constructor / البناء
     * @param n Variable name / اسم المتغير
     * @param pos Source position / الموقع في الكود
     */
    VariableExpr(const std::string& n, const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), name(n) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitVariableExpr(*this);
    }
    
    std::string toString() const override {
        return name;
    }
    
    Data::DataType getType() const override;
};

// =========================================================================
// Assignment Expression / تعبير الإسناد
// =========================================================================

/**
 * @brief Assignment expression node (e.g., x = 10) / عقدة تعبير الإسناد
 * 
 * Represents assignment of a value to a variable.
 * يمثل إسناد قيمة إلى متغير.
 * 
 * @example Examples / أمثلة:
 * - Simple: x = 10, name = "أحمد"
 * - Compound: counter += 1, value *= 2
 */
class AssignExpr : public Expression {
public:
    std::string name;       ///< Variable name / اسم المتغير
    ExprPtr value;          ///< Value to assign / القيمة المراد إسنادها
    
    /**
     * @brief Constructor / البناء
     * @param n Variable name / اسم المتغير
     * @param val Value expression / تعبير القيمة
     * @param pos Source position / الموقع في الكود
     */
    AssignExpr(const std::string& n, ExprPtr val, 
               const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), name(n), value(std::move(val)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitAssignExpr(*this);
    }
    
    std::string toString() const override {
        return name + " = " + value->toString();
    }
    
    Data::DataType getType() const override;
};

// =========================================================================
// Call Expression / تعبير الاستدعاء
// =========================================================================

/**
 * @brief Function call expression node / عقدة تعبير استدعاء الدالة
 * 
 * Represents a function call with arguments.
 * يمثل استدعاء دالة مع معاملات.
 * 
 * @example Examples / أمثلة:
 * - print("hello")
 * - sum(1, 2, 3)
 * - obj.method(arg)
 * - اطبع("مرحباً")
 */
class CallExpr : public Expression {
public:
    ExprPtr callee;         ///< Function being called / الدالة المستدعاة
    ExprList arguments;     ///< Function arguments / معاملات الدالة
    
    /**
     * @brief Constructor / البناء
     * @param callee Function expression / تعبير الدالة
     * @param args Argument list / قائمة المعاملات
     * @param pos Source position / الموقع في الكود
     */
    CallExpr(ExprPtr callee, ExprList args, 
             const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), callee(std::move(callee)), arguments(std::move(args)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitCallExpr(*this);
    }
    
    std::string toString() const override;
    Data::DataType getType() const override;
};

// =========================================================================
// Index Expression / تعبير الفهرسة
// =========================================================================

/**
 * @brief Index expression node (e.g., arr[0]) / عقدة تعبير الفهرسة
 * 
 * Represents indexing into an array, map, or string.
 * يمثل الفهرسة في مصفوفة أو قاموس أو نص.
 * 
 * @example Examples / أمثلة:
 * - arr[0], matrix[i][j]
 * - dict["key"], قاموس["مفتاح"]
 * - text[5]
 */
class IndexExpr : public Expression {
public:
    ExprPtr object;         ///< Object being indexed / الكائن المفهرس
    ExprPtr index;          ///< Index expression / تعبير الفهرس
    
    /**
     * @brief Constructor / البناء
     * @param obj Object expression / تعبير الكائن
     * @param idx Index expression / تعبير الفهرس
     * @param pos Source position / الموقع في الكود
     */
    IndexExpr(ExprPtr obj, ExprPtr idx, 
              const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), object(std::move(obj)), index(std::move(idx)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitIndexExpr(*this);
    }
    
    std::string toString() const override {
        return object->toString() + "[" + index->toString() + "]";
    }
    
    Data::DataType getType() const override;
};

// =========================================================================
// Member Access Expression / تعبير الوصول للعضو
// =========================================================================

/**
 * @brief Member access expression node (e.g., obj.field) / عقدة تعبير الوصول للعضو
 * 
 * Represents accessing a member (field or method) of an object.
 * يمثل الوصول إلى عضو (حقل أو طريقة) في كائن.
 * 
 * @example Examples / أمثلة:
 * - obj.field, person.name
 * - obj.method(), كائن.طريقة()
 */
class MemberExpr : public Expression {
public:
    ExprPtr object;         ///< Object expression / تعبير الكائن
    std::string member;     ///< Member name / اسم العضو
    
    /**
     * @brief Constructor / البناء
     * @param obj Object expression / تعبير الكائن
     * @param mem Member name / اسم العضو
     * @param pos Source position / الموقع في الكود
     */
    MemberExpr(ExprPtr obj, const std::string& mem, 
               const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), object(std::move(obj)), member(mem) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitMemberExpr(*this);
    }
    
    std::string toString() const override {
        return object->toString() + "." + member;
    }
    
    Data::DataType getType() const override;
};

// =========================================================================
// Member Assignment Expression / تعبير تعيين قيمة لعضو
// =========================================================================

/**
 * @brief Member assignment expression node (e.g., obj.field = value)
 * @brief عقدة تعبير تعيين قيمة لعضو في كائن
 * 
 * Represents assignment to an object member/field.
 * يمثل تعيين قيمة إلى حقل في كائن.
 * 
 * @example Examples / أمثلة:
 * - obj.field = 10
 * - person.name = "أحمد"
 * - شخص.اسم = "محمد"
 */
class MemberAssignExpr : public Expression {
public:
    ExprPtr object;         ///< Object expression / تعبير الكائن
    std::string member;     ///< Member name / اسم العضو
    ExprPtr value;          ///< Value to assign / القيمة المراد إسنادها
    
    /**
     * @brief Constructor / البناء
     * @param obj Object expression / تعبير الكائن
     * @param mem Member name / اسم العضو
     * @param val Value expression / تعبير القيمة
     * @param pos Source position / الموقع في الكود
     */
    MemberAssignExpr(ExprPtr obj, const std::string& mem, ExprPtr val,
                     const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), object(std::move(obj)), member(mem), value(std::move(val)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitMemberAssignExpr(*this);
    }
    
    std::string toString() const override {
        return object->toString() + "." + member + " = " + value->toString();
    }
    
    Data::DataType getType() const override {
        return value->getType();
    }
};

// =========================================================================
// Array Literal Expression / تعبير المصفوفة الحرفية
// =========================================================================

/**
 * @brief Array literal expression node (e.g., [1, 2, 3]) / عقدة تعبير المصفوفة الحرفية
 * 
 * Represents an array literal with elements.
 * يمثل مصفوفة حرفية مع عناصر.
 * 
 * @example Examples / أمثلة:
 * - [1, 2, 3]
 * - ["a", "b", "c"]
 * - [س، ص، ع]
 * - [] (empty array)
 */
class ArrayExpr : public Expression {
public:
    ExprList elements;      ///< Array elements / عناصر المصفوفة
    
    /**
     * @brief Constructor / البناء
     * @param elems Element list / قائمة العناصر
     * @param pos Source position / الموقع في الكود
     */
    ArrayExpr(ExprList elems, const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), elements(std::move(elems)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitArrayExpr(*this);
    }
    
    std::string toString() const override;
    
    Data::DataType getType() const override {
        return Data::DataType::ARRAY;
    }
};

// =========================================================================
// Map Literal Expression / تعبير القاموس الحرفي
// =========================================================================

/**
 * @brief Map key-value pair / زوج مفتاح-قيمة في القاموس
 */
struct MapPair {
    ExprPtr key;            ///< Key expression / تعبير المفتاح
    ExprPtr value;          ///< Value expression / تعبير القيمة
    
    MapPair(ExprPtr k, ExprPtr v)
        : key(std::move(k)), value(std::move(v)) {}
    
    // Copy constructor deleted (contains unique_ptr)
    MapPair(const MapPair&) = delete;
    MapPair& operator=(const MapPair&) = delete;
    
    // Move constructor and assignment
    MapPair(MapPair&&) = default;
    MapPair& operator=(MapPair&&) = default;
};

/**
 * @brief Map/dictionary literal expression node / عقدة تعبير القاموس الحرفي
 * 
 * Represents a map/dictionary literal with key-value pairs.
 * يمثل قاموس حرفي مع أزواج مفتاح-قيمة.
 * 
 * @example Examples / أمثلة:
 * - {"x": 10, "y": 20}
 * - {"الاسم": "أحمد", "العمر": 25}
 * - {} (empty map)
 */
class MapExpr : public Expression {
public:
    std::vector<MapPair> pairs; ///< Key-value pairs / أزواج مفتاح-قيمة
    
    /**
     * @brief Constructor / البناء
     * @param p Pairs list / قائمة الأزواج
     * @param pos Source position / الموقع في الكود
     */
    MapExpr(std::vector<MapPair> p, const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), pairs(std::move(p)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitMapExpr(*this);
    }
    
    std::string toString() const override;
    
    Data::DataType getType() const override {
        return Data::DataType::MAP;
    }
};

// =========================================================================
// Lambda Expression / تعبير Lambda
// =========================================================================

/**
 * @brief Function parameter / معامل الدالة
 */
struct Parameter {
    std::string name;           ///< Parameter name / اسم المعامل
    Data::DataType type;        ///< Parameter type / نوع المعامل
    ExprPtr defaultValue;       ///< Default value (optional) / القيمة الافتراضية
    
    Parameter(const std::string& n, Data::DataType t = Data::DataType::UNKNOWN,
              ExprPtr def = nullptr)
        : name(n), type(t), defaultValue(std::move(def)) {}
    
    // Copy constructor - deep copy the defaultValue
    Parameter(const Parameter& other)
        : name(other.name), type(other.type) {
        // Deep copy defaultValue if it exists
        if (other.defaultValue) {
            // Create a new copy by cloning (if clone method exists)
            // For now, set to nullptr as we can't clone expressions easily
            defaultValue = nullptr;
        } else {
            defaultValue = nullptr;
        }
    }
    
    // Copy assignment operator
    Parameter& operator=(const Parameter& other) {
        if (this != &other) {
            name = other.name;
            type = other.type;
            // Deep copy defaultValue if it exists
            if (other.defaultValue) {
                // For now, set to nullptr as we can't clone expressions easily
                defaultValue = nullptr;
            } else {
                defaultValue = nullptr;
            }
        }
        return *this;
    }
    
    // Move constructor and assignment
    Parameter(Parameter&&) = default;
    Parameter& operator=(Parameter&&) = default;
};

/**
 * @brief Lambda expression node (e.g., (x) => x * 2) / عقدة تعبير Lambda
 * 
 * Represents an anonymous function (lambda).
 * يمثل دالة مجهولة (lambda).
 * 
 * @example Examples / أمثلة:
 * - (x) => x * 2
 * - (a, b) => a + b
 * - lambda x: x ** 2
 */
class LambdaExpr : public Expression {
public:
    std::vector<Parameter> parameters; ///< Parameters / المعاملات
    ExprPtr body;                      ///< Lambda body / جسم Lambda
    
    /**
     * @brief Constructor / البناء
     * @param params Parameter list / قائمة المعاملات
     * @param body Body expression / تعبير الجسم
     * @param pos Source position / الموقع في الكود
     */
    LambdaExpr(std::vector<Parameter> params, ExprPtr body, 
               const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), parameters(std::move(params)), body(std::move(body)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitLambdaExpr(*this);
    }
    
    std::string toString() const override;
    
    Data::DataType getType() const override {
        return Data::DataType::FUNCTION;
    }
};

// =========================================================================
// List Comprehension Expression / تعبير الاستيعاب القائمي
// =========================================================================

/**
 * @brief List comprehension expression node / عقدة تعبير الاستيعاب القائمي
 * 
 * Represents a list comprehension (Python-style).
 * يمثل استيعاب قائمي (بأسلوب Python).
 * 
 * @example Examples / أمثلة:
 * - [x * 2 for x in range(10)]
 * - [س لكل س في قائمة إذا س > 5]
 * - [x**2 for x in numbers if x % 2 == 0]
 */
class ListComprehensionExpr : public Expression {
public:
    ExprPtr element;        ///< Element expression / تعبير العنصر
    std::string variable;   ///< Loop variable / متغير الحلقة
    ExprPtr iterable;       ///< Iterable expression / التعبير القابل للتكرار
    ExprPtr condition;      ///< Filter condition (optional) / شرط التصفية
    
    /**
     * @brief Constructor / البناء
     * @param elem Element expression / تعبير العنصر
     * @param var Loop variable name / اسم متغير الحلقة
     * @param iter Iterable expression / التعبير القابل للتكرار
     * @param cond Filter condition / شرط التصفية
     * @param pos Source position / الموقع في الكود
     */
    ListComprehensionExpr(ExprPtr elem, const std::string& var, 
                         ExprPtr iter, ExprPtr cond = nullptr,
                         const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), element(std::move(elem)), variable(var), 
          iterable(std::move(iter)), condition(std::move(cond)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitListComprehensionExpr(*this);
    }
    
    std::string toString() const override;
    
    Data::DataType getType() const override {
        return Data::DataType::ARRAY;
    }
};

// =========================================================================
// Dictionary Comprehension Expression / تعبير الاستيعاب القاموسي
// =========================================================================

/**
 * @brief Dictionary comprehension expression node / عقدة تعبير الاستيعاب القاموسي
 * 
 * Represents a dictionary comprehension (Python-style).
 * يمثل استيعاب قاموسي (بأسلوب Python).
 * 
 * @example Examples / أمثلة:
 * - {x: x**2 for x in range(10)}
 * - {س: س**2 لكل س في مدى(10) إذا س % 2 == 0}
 */
class DictComprehensionExpr : public Expression {
public:
    ExprPtr key;            ///< Key expression / تعبير المفتاح
    ExprPtr value;          ///< Value expression / تعبير القيمة
    std::string variable;   ///< Loop variable / متغير الحلقة
    ExprPtr iterable;       ///< Iterable expression / التعبير القابل للتكرار
    ExprPtr condition;      ///< Filter condition (optional) / شرط التصفية
    
    /**
     * @brief Constructor / البناء
     */
    DictComprehensionExpr(ExprPtr k, ExprPtr v, const std::string& var,
                         ExprPtr iter, ExprPtr cond = nullptr,
                         const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), key(std::move(k)), value(std::move(v)), 
          variable(var), iterable(std::move(iter)), condition(std::move(cond)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitDictComprehensionExpr(*this);
    }
    
    std::string toString() const override;
    
    Data::DataType getType() const override {
        return Data::DataType::MAP;
    }
};

// =========================================================================
// Generator Expression / تعبير المولد
// =========================================================================

/**
 * @brief Generator expression node / عقدة تعبير المولد
 * 
 * Represents a generator expression (lazy evaluation).
 * يمثل تعبير مولد (تقييم كسول).
 * 
 * @example Examples / أمثلة:
 * - (x * 2 for x in range(10))
 * - (س**2 لكل س في قائمة إذا س > 0)
 */
class GeneratorExpr : public Expression {
public:
    ExprPtr element;        ///< Element expression / تعبير العنصر
    std::string variable;   ///< Loop variable / متغير الحلقة
    ExprPtr iterable;       ///< Iterable expression / التعبير القابل للتكرار
    ExprPtr condition;      ///< Filter condition (optional) / شرط التصفية
    
    /**
     * @brief Constructor / البناء
     */
    GeneratorExpr(ExprPtr elem, const std::string& var, 
                 ExprPtr iter, ExprPtr cond = nullptr,
                 const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), element(std::move(elem)), variable(var),
          iterable(std::move(iter)), condition(std::move(cond)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitGeneratorExpr(*this);
    }
    
    std::string toString() const override;
    
    Data::DataType getType() const override {
        return Data::DataType::FUNCTION; // Generator is a special function
    }
};

// =========================================================================
// Decorator Expression / تعبير المُزخرِف
// =========================================================================

/**
 * @brief Decorator expression node / عقدة تعبير المُزخرِف
 * 
 * Represents a decorator applied to a function or class.
 * يمثل مُزخرِف (decorator) يُطبّق على دالة أو صنف.
 * 
 * Decorators modify or enhance the behavior of functions/classes.
 * المُزخرِفات تُعدّل أو تُحسّن سلوك الدوال/الأصناف.
 * 
 * @example Examples / أمثلة:
 * - Simple: @staticmethod
 * - With args: @cache(maxsize=100)
 * - Multiple: @decorator1 @decorator2 def func(): pass
 * - Arabic: @مُزخرِف دالة اسم(): pass
 * 
 * @note (AR) في لغة ص، المُزخرِفات تُطبّق من الأسفل للأعلى (مثل Python)
 * @note (EN) In Sad language, decorators apply bottom-to-top (like Python)
 */
class DecoratorExpr : public Expression {
public:
    std::string name;          ///< Decorator name / اسم المُزخرِف
    ExprList arguments;        ///< Decorator arguments (optional) / وسائط المُزخرِف
    bool hasArguments;         ///< Whether decorator has arguments / هل يحتوي على وسائط
    
    /**
     * @brief Constructor for decorator without arguments / بناء مُزخرِف بدون وسائط
     * @param decoratorName Name of the decorator / اسم المُزخرِف
     * @param pos Source position / الموقع في الكود
     * 
     * @example Examples / أمثلة:
     * @code
     * // @staticmethod
     * DecoratorExpr("staticmethod", {})
     * 
     * // @مُزخرِف
     * DecoratorExpr("مُزخرِف", {})
     * @endcode
     */
    DecoratorExpr(const std::string& decoratorName,
                  const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), name(decoratorName), 
          arguments(), hasArguments(false) {}
    
    /**
     * @brief Constructor for decorator with arguments / بناء مُزخرِف مع وسائط
     * @param decoratorName Name of the decorator / اسم المُزخرِف
     * @param args List of argument expressions / قائمة تعبيرات الوسائط
     * @param pos Source position / الموقع في الكود
     * 
     * @example Examples / أمثلة:
     * @code
     * // @cache(maxsize=100)
     * DecoratorExpr("cache", {assignExpr("maxsize", literal(100))})
     * 
     * // @retry(times=3, delay=1.5)
     * DecoratorExpr("retry", {assign("times", 3), assign("delay", 1.5)})
     * @endcode
     */
    DecoratorExpr(const std::string& decoratorName, ExprList args,
                  const Lexer::Position& pos = Lexer::Position())
        : Expression(pos), name(decoratorName), 
          arguments(std::move(args)), hasArguments(true) {}
    
    /**
     * @brief Accept visitor pattern / قبول نمط الزائر
     */
    void accept(ASTVisitor& visitor) override {
        visitor.visitDecoratorExpr(*this);
    }
    
    /**
     * @brief Convert to string representation / تحويل لنص
     * @return String representation / التمثيل النصي
     * 
     * @example Output examples / أمثلة المخرجات:
     * - "@decorator" → "Decorator(decorator)"
     * - "@cache(100)" → "Decorator(cache, args=[100])"
     */
    std::string toString() const override;
    
    /**
     * @brief Get expression type / الحصول على نوع التعبير
     * @return DataType::FUNCTION (decorators are function-like) / دالة (المُزخرِفات شبيهة بالدوال)
     */
    Data::DataType getType() const override {
        return Data::DataType::FUNCTION; // Decorators are function-like
    }
};

} // namespace AST
} // namespace Sad

#endif // SAD_AST_EXPRESSIONS_H
