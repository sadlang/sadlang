/**
 * @file pattern_nodes.h
 * @brief (AR) عقد AST لـ Pattern Matching / (EN) AST Nodes for Pattern Matching
 * 
 * (AR) يحتوي على تعريفات عقد AST للأنماط ومطابقة الأنماط
 * (EN) Contains AST node definitions for patterns and pattern matching
 * 
 * @author Sad Language Team
 * @date December 19, 2025
 * @version 2.0.0
 */

#pragma once

#include "ast_node.h"
#include "ast_visitor.h"
#include "expressions.h"
#include "statements.h"
#include "value.h"
#include <memory>
#include <string>
#include <vector>
#include <map>

namespace Sad {
namespace AST {

// Forward declarations
class Pattern;
struct CaseClause;

// ============================================================================
// (AR) النمط الأساسي / (EN) Base Pattern
// ============================================================================

/**
 * @brief (AR) النمط الأساسي - فئة مجردة / (EN) Base Pattern - Abstract Class
 * 
 * (AR) تمثل نمط في match statement يمكن مطابقته مع قيمة
 * (EN) Represents a pattern in match statement that can be matched against a value
 */
class Pattern {
public:
    virtual ~Pattern() = default;
    
    /**
     * @brief (AR) محاولة مطابقة النمط مع قيمة / (EN) Try to match pattern with value
     * 
     * @param value (AR) القيمة المُختبرة / (EN) Value to test
     * @param bindings (AR) map للمتغيرات المربوطة / (EN) map for bound variables
     * @return (AR) true إذا تطابق / (EN) true if matched
     */
    virtual bool matches(const Data::Value& value, 
                        std::map<std::string, Data::Value>& bindings) const = 0;
    
    /**
     * @brief (AR) تحويل النمط إلى نص / (EN) Convert pattern to string
     */
    virtual std::string toString() const = 0;
};

// ============================================================================
// (AR) نمط القيمة الحرفية / (EN) Literal Pattern
// ============================================================================

/**
 * @brief (AR) نمط قيمة حرفية / (EN) Literal Pattern
 * 
 * (AR) يطابق قيمة حرفية محددة: 0، "hello"، true، إلخ
 * (EN) Matches a specific literal value: 0, "hello", true, etc.
 * 
 * @example
 * @code
 * match x:
 *     case 0:         // LiteralPattern(0)
 *     case "hello":   // LiteralPattern("hello")
 *     case true:      // LiteralPattern(true)
 * @endcode
 */
class LiteralPattern : public Pattern {
public:
    Data::Value literal;  ///< (AR) القيمة الحرفية / (EN) Literal value
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    explicit LiteralPattern(Data::Value val) 
        : literal(std::move(val)) {}
    
    bool matches(const Data::Value& value, 
                std::map<std::string, Data::Value>& bindings) const override {
        return value.toString() == literal.toString();
    }
    
    std::string toString() const override {
        return literal.toString();
    }
};

// ============================================================================
// (AR) النمط الشامل / (EN) Wildcard Pattern
// ============================================================================

/**
 * @brief (AR) النمط الشامل _ / (EN) Wildcard Pattern _
 * 
 * (AR) يطابق أي قيمة (default case)
 * (EN) Matches any value (default case)
 * 
 * @example
 * @code
 * match x:
 *     case 1:
 *         print("one")
 *     case _:         // WildcardPattern - يطابق كل شيء آخر
 *         print("other")
 * @endcode
 */
class WildcardPattern : public Pattern {
public:
    bool matches(const Data::Value& value, 
                std::map<std::string, Data::Value>& bindings) const override {
        return true;  // (AR) يطابق أي شيء / (EN) Matches anything
    }
    
    std::string toString() const override {
        return "_";
    }
};

// ============================================================================
// (AR) نمط المتغير / (EN) Variable Pattern
// ============================================================================

/**
 * @brief (AR) نمط متغير / (EN) Variable Pattern
 * 
 * (AR) يطابق أي قيمة ويربطها بمتغير
 * (EN) Matches any value and binds it to a variable
 * 
 * @example
 * @code
 * match x:
 *     case n:              // VariablePattern("n") - يربط x بـ n
 *         print("value:", n)
 *     case [first, second]: // يربط أول عنصرين
 *         print(first, second)
 * @endcode
 */
class VariablePattern : public Pattern {
public:
    std::string name;  ///< (AR) اسم المتغير / (EN) Variable name
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    explicit VariablePattern(std::string n) 
        : name(std::move(n)) {}
    
    bool matches(const Data::Value& value, 
                std::map<std::string, Data::Value>& bindings) const override {
        bindings[name] = value;  // (AR) ربط القيمة بالمتغير / (EN) Bind value to variable
        return true;
    }
    
    std::string toString() const override {
        return name;
    }
};

// ============================================================================
// (AR) نمط القائمة / (EN) List Pattern
// ============================================================================

/**
 * @brief (AR) نمط قائمة / (EN) List Pattern
 * 
 * (AR) يطابق قوائم بأنماط محددة للعناصر
 * (EN) Matches lists with specific patterns for elements
 * 
 * @example
 * @code
 * match list:
 *     case []:              // قائمة فارغة
 *     case [x]:             // عنصر واحد
 *     case [x, y]:          // عنصران
 *     case [x, y, z]:       // ثلاثة عناصر
 *     case [first, *rest]:  // أول عنصر + الباقي
 * @endcode
 */
class ListPattern : public Pattern {
public:
    std::vector<std::unique_ptr<Pattern>> elements;  ///< (AR) أنماط العناصر / (EN) Element patterns
    bool has_rest;                                   ///< (AR) هل يوجد *rest؟ / (EN) Has *rest?
    std::string rest_name;                           ///< (AR) اسم متغير rest / (EN) Rest variable name
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    ListPattern(std::vector<std::unique_ptr<Pattern>> elems,
                bool rest = false,
                std::string rest_n = "")
        : elements(std::move(elems)),
          has_rest(rest),
          rest_name(std::move(rest_n)) {}
    
    bool matches(const Data::Value& value, 
                std::map<std::string, Data::Value>& bindings) const override {
        // (AR) يجب أن تكون القيمة array / (EN) Value must be an array
        if (value.getType() != Data::ValueType::ARRAY) {
            return false;
        }
        
        size_t arr_size = value.size();
        
        // (AR) إذا لم يكن لدينا rest، يجب أن يكون الحجم مطابق / (EN) If no rest, size must match
        if (!has_rest && arr_size != elements.size()) {
            return false;
        }
        
        // (AR) إذا لدينا rest، يجب أن يكون الحجم >= عدد العناصر / (EN) If rest, size must be >= elements count
        if (has_rest && arr_size < elements.size()) {
            return false;
        }
        
        // (AR) طابق كل عنصر / (EN) Match each element
        for (size_t i = 0; i < elements.size(); ++i) {
            if (!elements[i]->matches(value[i], bindings)) {
                return false;
            }
        }
        
        // (AR) إذا كان لدينا rest، اربط العناصر المتبقية / (EN) If rest, bind remaining elements
        if (has_rest) {
            std::vector<Data::Value> rest_values;
            for (size_t i = elements.size(); i < arr_size; ++i) {
                rest_values.push_back(value[i]);
            }
            bindings[rest_name] = Data::Value(rest_values);
        }
        
        return true;
    }
    
    std::string toString() const override {
        std::string result = "[";
        for (size_t i = 0; i < elements.size(); ++i) {
            if (i > 0) result += ", ";
            result += elements[i]->toString();
        }
        if (has_rest) {
            if (!elements.empty()) result += ", ";
            result += "*" + rest_name;
        }
        result += "]";
        return result;
    }
};

// ============================================================================
// (AR) نمط النطاق / (EN) Range Pattern
// ============================================================================

/**
 * @brief (AR) نمط نطاق: 1..10 / (EN) Range Pattern: 1..10
 * 
 * (AR) يطابق قيمة ضمن نطاق محدد
 * (EN) Matches a value within a specified range
 * 
 * @example
 * @code
 * match x:
 *     case 1..10:           // RangePattern(1, 10, inclusive)
 *         print("بين 1 و 10")
 *     case 11..=20:         // RangePattern(11, 20, inclusive)
 *         print("بين 11 و 20 شاملاً")
 * @endcode
 */
class RangePattern : public Pattern {
public:
    Data::Value start;    ///< (AR) بداية النطاق / (EN) Range start
    Data::Value end;      ///< (AR) نهاية النطاق / (EN) Range end
    bool inclusive;       ///< (AR) شامل للنهاية؟ / (EN) Inclusive end?
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    RangePattern(Data::Value s, Data::Value e, bool incl = false)
        : start(std::move(s)), end(std::move(e)), inclusive(incl) {}
    
    bool matches(const Data::Value& value, 
                std::map<std::string, Data::Value>& bindings) const override {
        // (AR) التحقق من أن القيمة رقم / (EN) Check value is a number
        if (value.getType() != Data::ValueType::INTEGER && 
            value.getType() != Data::ValueType::DOUBLE) {
            return false;
        }
        
        double val = value.toDouble();
        double startVal = start.toDouble();
        double endVal = end.toDouble();
        
        if (inclusive) {
            return val >= startVal && val <= endVal;
        } else {
            return val >= startVal && val < endVal;
        }
    }
    
    std::string toString() const override {
        return start.toString() + (inclusive ? "..=" : "..") + end.toString();
    }
};

// ============================================================================
// (AR) نمط البنية/الصنف / (EN) Struct/Class Pattern
// ============================================================================

/**
 * @brief (AR) نمط بنية/صنف {حقل: نمط، ...} / (EN) Struct/Class Pattern {field: pattern, ...}
 * 
 * (AR) يطابق كائنات مع أنماط لحقولها
 * (EN) Matches objects with patterns for their fields
 * 
 * @example
 * @code
 * match person:
 *     case {الاسم: "أحمد", العمر: ع}:
 *         print("أحمد عمره " + ع)
 *     case {الاسم: ن, العمر: ع} if ع > 18:
 *         print(ن + " بالغ")
 * @endcode
 */
class StructPattern : public Pattern {
public:
    std::string typeName;  ///< (AR) اسم النوع (اختياري) / (EN) Type name (optional)
    std::vector<std::pair<std::string, std::unique_ptr<Pattern>>> fields;
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    StructPattern(std::string type, 
                  std::vector<std::pair<std::string, std::unique_ptr<Pattern>>> flds)
        : typeName(std::move(type)), fields(std::move(flds)) {}
    
    bool matches(const Data::Value& value, 
                std::map<std::string, Data::Value>& bindings) const override {
        // (AR) يجب أن تكون القيمة كائن / (EN) Value must be an object
        if (value.getType() != Data::ValueType::OBJECT) {
            return false;
        }
        
        // (AR) التحقق من النوع إذا تم تحديده / (EN) Check type if specified
        if (!typeName.empty()) {
            // TODO: التحقق من اسم صنف الكائن
        }
        
        // (AR) مطابقة كل حقل / (EN) Match each field
        for (const auto& fieldPair : fields) {
            const std::string& fieldName = fieldPair.first;
            const auto& fieldPattern = fieldPair.second;
            
            // (AR) الكائنات لا تدعم الوصول المباشر للحقول في هذا السياق
            // (EN) Objects don't support direct field access in this context
            // TODO: تنفيذ الوصول للحقول عبر ObjectInstance
            // للآن، نفترض أن Value يدعم operator[] للخرائط
            try {
                const Data::Value& fieldValue = value[fieldName];
                if (!fieldPattern->matches(fieldValue, bindings)) {
                    return false;
                }
            } catch (...) {
                return false;  // (AR) الحقل غير موجود / (EN) Field not found
            }
        }
        
        return true;
    }
    
    std::string toString() const override {
        std::string result = typeName.empty() ? "{" : typeName + " {";
        for (size_t i = 0; i < fields.size(); ++i) {
            if (i > 0) result += ", ";
            result += fields[i].first + ": " + fields[i].second->toString();
        }
        result += "}";
        return result;
    }
};

// ============================================================================
// (AR) نمط الربط / (EN) Binding Pattern
// ============================================================================

/**
 * @brief (AR) نمط ربط: اسم @ نمط / (EN) Binding Pattern: name @ pattern
 * 
 * (AR) يربط القيمة بمتغير مع التحقق من نمط آخر
 * (EN) Binds value to variable while checking another pattern
 * 
 * @example
 * @code
 * match point:
 *     case نقطة @ {س: 0, ص: ص}:
 *         print("نقطة على محور Y: " + نقطة)
 *     case قيمة @ 1..100:
 *         print("قيمة في النطاق: " + قيمة)
 * @endcode
 */
class BindingPattern : public Pattern {
public:
    std::string name;                   ///< (AR) اسم المتغير / (EN) Variable name
    std::unique_ptr<Pattern> pattern;   ///< (AR) النمط الداخلي / (EN) Inner pattern
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    BindingPattern(std::string n, std::unique_ptr<Pattern> p)
        : name(std::move(n)), pattern(std::move(p)) {}
    
    bool matches(const Data::Value& value, 
                std::map<std::string, Data::Value>& bindings) const override {
        // (AR) أولاً تحقق من النمط الداخلي / (EN) First check inner pattern
        if (!pattern->matches(value, bindings)) {
            return false;
        }
        
        // (AR) اربط القيمة الكاملة بالمتغير / (EN) Bind full value to variable
        bindings[name] = value;
        return true;
    }
    
    std::string toString() const override {
        return name + " @ " + pattern->toString();
    }
};

// ============================================================================
// (AR) نمط OR / (EN) OR Pattern
// ============================================================================

/**
 * @brief (AR) نمط OR (a | b | c) / (EN) OR Pattern (a | b | c)
 * 
 * (AR) يطابق إذا تطابق أي من البدائل
 * (EN) Matches if any of the alternatives match
 * 
 * @example
 * @code
 * match status:
 *     case "ok" | "success" | "done":  // OrPattern
 *         print("نجح")
 *     case "error" | "failed":
 *         print("فشل")
 * @endcode
 */
class OrPattern : public Pattern {
public:
    std::vector<std::unique_ptr<Pattern>> alternatives;  ///< (AR) البدائل / (EN) Alternatives
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    explicit OrPattern(std::vector<std::unique_ptr<Pattern>> alts)
        : alternatives(std::move(alts)) {}
    
    bool matches(const Data::Value& value, 
                std::map<std::string, Data::Value>& bindings) const override {
        // (AR) جرّب كل بديل / (EN) Try each alternative
        for (const auto& alt : alternatives) {
            std::map<std::string, Data::Value> temp_bindings;
            if (alt->matches(value, temp_bindings)) {
                // (AR) نجح أحد البدائل / (EN) One alternative succeeded
                bindings.insert(temp_bindings.begin(), temp_bindings.end());
                return true;
            }
        }
        return false;
    }
    
    std::string toString() const override {
        std::string result;
        for (size_t i = 0; i < alternatives.size(); ++i) {
            if (i > 0) result += " || ";
            result += alternatives[i]->toString();
        }
        return result;
    }
};

// ============================================================================
// (AR) فرع Case / (EN) Case Clause
// ============================================================================

/**
 * @brief (AR) فرع case في match statement / (EN) Case clause in match statement
 * 
 * (AR) يمثل فرع case واحد مع نمط، guard اختياري، وجسم
 * (EN) Represents one case clause with pattern, optional guard, and body
 * 
 * @example
 * @code
 * match x:
 *     case n if n > 0:     // pattern=n, guard=(n>0), body=[print...]
 *         print("positive")
 * @endcode
 */
struct CaseClause {
    std::unique_ptr<Pattern> pattern;  ///< (AR) النمط / (EN) Pattern
    ExprPtr guard;                     ///< (AR) شرط guard اختياري / (EN) Optional guard condition
    std::vector<StmtPtr> body;         ///< (AR) الكود المُنفذ / (EN) Code to execute
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    CaseClause(std::unique_ptr<Pattern> p,
               ExprPtr g,
               std::vector<StmtPtr> b)
        : pattern(std::move(p)),
          guard(std::move(g)),
          body(std::move(b)) {}
    
    // Move constructor
    CaseClause(CaseClause&& other) noexcept = default;
    CaseClause& operator=(CaseClause&& other) noexcept = default;
    
    // Delete copy operations
    CaseClause(const CaseClause&) = delete;
    CaseClause& operator=(const CaseClause&) = delete;
};

// ============================================================================
// (AR) جملة Match / (EN) Match Statement
// ============================================================================

/**
 * @brief (AR) جملة match لمطابقة الأنماط / (EN) Match statement for pattern matching
 * 
 * (AR) تمثل جملة match كاملة مع قيمة مُختبرة وقائمة حالات
 * (EN) Represents a complete match statement with test value and case list
 * 
 * @example
 * @code
 * match x:
 *     case 0:
 *         print("zero")
 *     case n if n < 0:
 *         print("negative")
 *     case n:
 *         print("positive:", n)
 * @endcode
 */
class MatchStmt : public Statement {
public:
    ExprPtr value;                     ///< (AR) القيمة المُختبرة / (EN) Value to test
    std::vector<CaseClause> cases;     ///< (AR) قائمة الحالات / (EN) List of cases
    
    /**
     * @brief (AR) المُنشئ / (EN) Constructor
     */
    MatchStmt(ExprPtr val, 
              std::vector<CaseClause> cs,
              const Lexer::Position& pos = Lexer::Position())
        : Statement(pos), value(std::move(val)), cases(std::move(cs)) {}
    
    void accept(ASTVisitor& visitor) override {
        visitor.visitMatchStmt(*this);
    }
    
    std::string toString() const override {
        return "MatchStmt";
    }
};

} // namespace AST
} // namespace Sad