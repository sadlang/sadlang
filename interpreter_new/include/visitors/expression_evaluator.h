/**
 * @file expression_evaluator.h
 * @brief (AR) مُقيِّم التعابير - تنفيذ وتقييم التعابير في وقت التشغيل
 * @brief (EN) Expression Evaluator - Evaluate and execute expressions at runtime
 * 
 * الوصف بالعربية:
 * =================
 * مُقيِّم التعابير مسؤول عن:
 * - تقييم جميع أنواع التعابير (حسابية، منطقية، مقارنة)
 * - التعامل مع القيم الحرفية (أرقام، نصوص، boolean)
 * - قراءة وكتابة المتغيرات
 * - تنفيذ العمليات الثنائية والأحادية
 * - التعامل مع المصفوفات والقواميس
 * - استدعاء الدوال وتقييم الفهرسة
 * 
 * المميزات:
 * - يرث من BaseASTVisitor لتطبيق نمط الزائر
 * - يُرجع Value من كل عملية تقييم
 * - يتكامل مع VariableManager و FunctionManager
 * - دعم كامل لجميع أنواع العمليات
 * - رسائل خطأ واضحة ثنائية اللغة
 * 
 * English Description:
 * ====================
 * Expression Evaluator is responsible for:
 * - Evaluating all expression types (arithmetic, logical, comparison)
 * - Handling literal values (numbers, strings, booleans)
 * - Reading and writing variables
 * - Executing binary and unary operations
 * - Handling arrays and maps
 * - Function calls and indexing evaluation
 * 
 * Features:
 * - Inherits from BaseASTVisitor to implement Visitor pattern
 * - Returns Value from each evaluation
 * - Integrates with VariableManager and FunctionManager
 * - Full support for all operation types
 * - Clear bilingual error messages
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#pragma once

#include "ast_visitor.h"
#include "expressions.h"
#include "class_nodes.h"
#include "value.h"
#include "variable_manager.h"
#include "function_manager.h"
#include "ownership_manager.h"
#include "token.h"
#include "exception.h"  // For RuntimeError, TypeError, etc.
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>

// Forward declaration to avoid circular dependency
namespace Sad {
namespace Data {
    class ClassType;
    struct OperatorOverload;
}
namespace Interpreter {
    class StatementExecutor;
}
}

namespace Sad {
namespace Interpreter {

// Note: RuntimeError is now defined in interpreter/exception.h
// ملاحظة: RuntimeError معرف الآن في interpreter/exception.h

/**
 * @brief (AR) مُقيِّم التعابير - ينفذ نمط الزائر لتقييم AST
 * @brief (EN) Expression Evaluator - implements Visitor pattern to evaluate AST
 * 
 * هذا الصنف يجتاز شجرة AST ويقيم كل تعبير، ويُرجع قيمة Value
 * This class traverses the AST and evaluates each expression, returning a Value
 * 
 * @example مثال الاستخدام / Usage Example:
 * @code{.cpp}
 * Data::VariableManager varMgr;
 * Data::FunctionManager funcMgr;
 * ExpressionEvaluator evaluator(varMgr, funcMgr);
 * 
 * // تقييم تعبير: 10 + 20
 * auto expr = std::make_unique<BinaryExpr>(...);
 * expr->accept(evaluator);
 * Value result = evaluator.getResult();  // 30
 * @endcode
 */
class ExpressionEvaluator : public AST::BaseASTVisitor {
public:
    /**
     * @brief (AR) البناء
     * @brief (EN) Constructor
     * 
     * @param varMgr (AR) مرجع لمدير المتغيرات / (EN) Reference to variable manager
     * @param funcMgr (AR) مرجع لمدير الدوال / (EN) Reference to function manager
     * @param scopeMgr (AR) مرجع لمدير النطاقات / (EN) Reference to scope manager
     * @param executor (AR) مرجع لمنفذ العبارات / (EN) Reference to statement executor
     */
    ExpressionEvaluator(Data::VariableManager& varMgr, 
                       Data::FunctionManager& funcMgr, 
                       Data::ScopeManager& scopeMgr,
                       StatementExecutor& executor,
                       Data::OwnershipManager& ownershipMgr)
        : variableManager_(varMgr), 
          functionManager_(funcMgr), 
          scopeManager_(scopeMgr),
          statementExecutor_(executor),
          ownershipManager_(ownershipMgr),
          lastResult_() {}
    
    /**
     * @brief (AR) الحصول على نتيجة آخر تقييم
     * @brief (EN) Get result of last evaluation
     * 
     * @return (AR) القيمة الناتجة / (EN) Resulting value
     */
    Data::Value getResult() const { return lastResult_; }
    
    /**
     * @brief (AR) مسح النتيجة الحالية
     * @brief (EN) Clear current result
     */
    void clearResult() { lastResult_ = Data::Value(); }
    
    // =====================================================================
    // (AR) زيارة التعابير / (EN) Expression Visitors
    // =====================================================================
    
    /**
     * @brief (AR) تقييم تعبير ثنائي (10 + 20، x * y)
     * @brief (EN) Evaluate binary expression (10 + 20, x * y)
     */
    void visitBinaryExpr(AST::BinaryExpr& node) override;
    
    /**
     * @brief (AR) تقييم تعبير أحادي (-42، !flag)
     * @brief (EN) Evaluate unary expression (-42, !flag)
     */
    void visitUnaryExpr(AST::UnaryExpr& node) override;
    
    /**
     * @brief (AR) تقييم تعبير ثلاثي شرطي (cond ? true : false)
     * @brief (EN) Evaluate ternary conditional expression (cond ? true : false)
     */
    void visitTernaryExpr(AST::TernaryExpr& node) override;
    
    /**
     * @brief (AR) تقييم قيمة حرفية (42، "hello"، true)
     * @brief (EN) Evaluate literal value (42, "hello", true)
     */
    void visitLiteralExpr(AST::LiteralExpr& node) override;
    
    /**
     * @brief (AR) قراءة قيمة متغير (x، counter)
     * @brief (EN) Read variable value (x, counter)
     */
    void visitVariableExpr(AST::VariableExpr& node) override;
    
    /**
     * @brief (AR) تنفيذ تعبير الاستعارة (&x، &متغير x)
     * @brief (EN) Execute borrow expression (&x, &mut x)
     */
    void visitBorrowExpr(AST::BorrowExpr& node) override;
    
    /**
     * @brief (AR) تنفيذ تعبير 'هذا' (this)
     * @brief (EN) Execute 'this' expression
     */
    void visitThisExpr(AST::ThisExpr& node) override;
    void visitSuperExpr(AST::SuperExpr& node) override;
    
    /**
     * @brief (AR) تنفيذ عملية إسناد (x = 10)
     * @brief (EN) Execute assignment (x = 10)
     */
    void visitAssignExpr(AST::AssignExpr& node) override;
    
    /**
     * @brief (AR) استدعاء دالة (func(1, 2, 3))
     * @brief (EN) Call function (func(1, 2, 3))
     */
    void visitCallExpr(AST::CallExpr& node) override;
    
    /**
     * @brief (AR) فهرسة مصفوفة/قاموس (arr[0]، dict["key"])
     * @brief (EN) Index array/map (arr[0], dict["key"])
     */
    void visitIndexExpr(AST::IndexExpr& node) override;
    
    /**
     * @brief (AR) الوصول لعضو (obj.field)
     * @brief (EN) Member access (obj.field)
     */
    void visitMemberExpr(AST::MemberExpr& node) override;
    
    /**
     * @brief (AR) إنشاء مصفوفة ([1, 2, 3])
     * @brief (EN) Create array ([1, 2, 3])
     */
    void visitArrayExpr(AST::ArrayExpr& node) override;
    
    /**
     * @brief (AR) إنشاء قاموس ({x: 10، y: 20})
     * @brief (EN) Create map ({x: 10, y: 20})
     */
    void visitMapExpr(AST::MapExpr& node) override;
    
    /**
     * @brief (AR) تقييم تعبير Walrus (:= variable value)
     * @brief (EN) Evaluate Walrus expression (:= variable value)
     * @details (AR) يُعيّن القيمة للمتغير ويُرجعها
     * @details (EN) Assigns value to variable and returns it
     */
    void visitWalrusExpr(AST::WalrusExpr& node) override;
    
    /**
     * @brief (AR) تقييم تعبير Await (await expr)
     * @brief (EN) Evaluate Await expression (await expr)
     * @details (AR) ينتظر نتيجة تعبير غير متزامن
     * @details (EN) Waits for result of async expression
     */
    void visitAwaitExpr(AST::AwaitExpr& node) override;
    
    /**
     * @brief (AR) تقييم دالة Lambda (lambda(x) { return x * 2; })
     * @brief (EN) Evaluate Lambda function (lambda(x) { return x * 2; })
     */
    void visitLambdaExpr(AST::LambdaExpr& node) override;
    
    /**
     * @brief (AR) تقييم استيعاب قائمي ([x*2 for x in arr])
     * @brief (EN) Evaluate list comprehension ([x*2 for x in arr])
     */
    void visitListComprehensionExpr(AST::ListComprehensionExpr& node) override;
    
    /**
     * @brief (AR) تقييم استيعاب قاموسي ({k: v for item in arr})
     * @brief (EN) Evaluate dict comprehension ({k: v for item in arr})
     */
    void visitDictComprehensionExpr(AST::DictComprehensionExpr& node) override;
    
    /**
     * @brief (AR) تقييم استيعاب مجموعة ({x for x in arr})
     * @brief (EN) Evaluate set comprehension ({x for x in arr})
     */
    void visitSetComprehensionExpr(AST::SetComprehensionExpr& node) override;
    
    /**
     * @brief (AR) تقييم إنشاء كائن جديد (جديد صنف())
     * @brief (EN) Evaluate new object creation (new Class())
     */
    void visitNewExpr(AST::NewExpr& node) override;
    
    /**
     * @brief (AR) تقييم تعيين قيمة لعضو في كائن (obj.field = value)
     * @brief (EN) Evaluate member assignment (obj.field = value)
     */
    void visitMemberAssignExpr(AST::MemberAssignExpr& node) override;
    
    /**
     * @brief (AR) تقييم الإسناد بالفهرس (م[0] = قيمة، قاموس["مفتاح"] = قيمة)
     * @brief (EN) Evaluate index assignment (arr[0] = value, map["key"] = value)
     */
    void visitIndexAssignExpr(AST::IndexAssignExpr& node) override;
    
    /**
     * @brief (AR) تقييم استدعاء طريقة على كائن (obj.method(args))
     * @brief (EN) Evaluate method call on object (obj.method(args))
     */
    void visitMethodCallExpr(AST::MethodCallExpr& node) override;
    
    /**
     * @brief (AR) تقييم تنفيذ قالب (max<int>(a, b))
     * @brief (EN) Evaluate template instantiation (max<int>(a, b))
     */
    void visitTemplateInstantiation(AST::TemplateInstantiation& node) override;
    
private:
    Data::VariableManager& variableManager_;    ///< (AR) مدير المتغيرات / (EN) Variable manager
    Data::FunctionManager& functionManager_;    ///< (AR) مدير الدوال / (EN) Function manager
    Data::ScopeManager& scopeManager_;          ///< (AR) مدير النطاقات / (EN) Scope manager
    StatementExecutor& statementExecutor_;      ///< (AR) منفذ العبارات / (EN) Statement executor
    Data::OwnershipManager& ownershipManager_;  ///< (AR) مدير الملكية / (EN) Ownership manager
    Data::Value lastResult_;                    ///< (AR) آخر نتيجة / (EN) Last result
    
    // =====================================================================
    // (AR) تحسين النصوص العربية / (EN) Arabic String Optimization
    // =====================================================================
    
    /**
     * @brief (AR) مُجمّع النصوص - يُخزّن النصوص المتكررة مرة واحدة فقط
     * @brief (EN) String Pool - stores repeated strings only once
     * 
     * عندما يصادف المفسّر نصاً حرفياً، يفحص هل هو موجود في المُجمّع.
     * إذا كان موجوداً، يُعيد مرجعاً للنسخة المُخزّنة بدلاً من إنشاء نسخة جديدة.
     * هذا يوفر الذاكرة خاصةً مع النصوص العربية المتكررة.
     */
    std::unordered_set<std::string> stringPool_;
    
    /**
     * @brief (AR) إحصائيات التحسين العربي للمفسّر
     * @brief (EN) Arabic optimization statistics for interpreter
     */
    struct ArabicOptStats {
        size_t totalStrings = 0;      ///< (AR) مجموع النصوص / (EN) Total strings
        size_t pooledStrings = 0;     ///< (AR) النصوص المُدمجة / (EN) Pooled (deduplicated) strings
        size_t poolHits = 0;          ///< (AR) عدد الإصابات / (EN) Pool hits
        size_t arabicStrings = 0;     ///< (AR) النصوص العربية / (EN) Arabic strings
        size_t savedBytes = 0;        ///< (AR) البايتات الموفرة / (EN) Saved bytes
    } arabicOptStats_;
    
    /**
     * @brief (AR) فحص إذا كان النص يحتوي أحرف عربية
     * @brief (EN) Check if string contains Arabic characters
     */
    static bool containsArabic(const std::string& str);
    
    /**
     * @brief (AR) تجميع نص في المُجمّع (intern)
     * @brief (EN) Intern a string into the pool
     * @return (AR) مرجع للنص المُجمّع / (EN) Reference to the pooled string
     */
    const std::string& internString(const std::string& str);
    
public:
    /**
     * @brief (AR) طباعة إحصائيات التحسين العربي
     * @brief (EN) Print Arabic optimization statistics
     */
    void printArabicOptStats() const;
    
    /**
     * @brief (AR) تنفيذ عملية ثنائية حسابية
     * @brief (EN) Execute arithmetic binary operation
     */
    Data::Value evaluateArithmeticOp(const Data::Value& left, Lexer::TokenType op, const Data::Value& right, const Lexer::Position& pos);
    
    /**
     * @brief (AR) تنفيذ عملية مقارنة
     * @brief (EN) Execute comparison operation
     */
    Data::Value evaluateComparisonOp(const Data::Value& left, Lexer::TokenType op, const Data::Value& right, const Lexer::Position& pos);
    
    /**
     * @brief (AR) تنفيذ عملية منطقية
     * @brief (EN) Execute logical operation
     */
    Data::Value evaluateLogicalOp(const Data::Value& left, Lexer::TokenType op, const Data::Value& right, const Lexer::Position& pos);

    /**
     * @brief (AR) تنفيذ عمليات البت: ^ | & << >>
     * @brief (EN) Execute bitwise operations: ^ | & << >>
     */
    Data::Value evaluateBitwiseOp(const Data::Value& left, Lexer::TokenType op, const Data::Value& right, const Lexer::Position& pos);
    
    /**
     * @brief (AR) تنفيذ عامل محمل زائداً على كائن
     * @brief (EN) Execute operator overload on an object
     * 
     * @param left (const Data::Value&) — (AR) الكائن (الطرف الأيسر) / (EN) object (left operand)
     * @param overload (Data::OperatorOverload&) — (AR) تعريف العامل / (EN) operator definition
     * @param right (const Data::Value&) — (AR) الطرف الأيمن / (EN) right operand
     * @param pos (const Lexer::Position&) — (AR) موقع في الكود / (EN) source position
     * @return (Data::Value) — (AR) نتيجة العملية / (EN) operation result
     */
    Data::Value executeOperatorOverload(const Data::Value& left, Data::OperatorOverload& overload, const Data::Value& right, const Lexer::Position& pos);
    
    /**
     * @brief (AR) تحويل Token إلى Value
     * @brief (EN) Convert Token to Value
     */
    Data::Value tokenToValue(const Lexer::Token& token);
    
    /**
     * @brief (AR) فحص إمكانية الوصول لعضو في صنف
     * @brief (EN) Check access permission for class member
     * 
     * @param visibility (AR) مستوى الوصول / (EN) Visibility level
     * @param memberName (AR) اسم العضو / (EN) Member name
     * @param targetClass (AR) الصنف الهدف / (EN) Target class
     * @throws RuntimeError (AR) إذا كان الوصول محظوراً / (EN) If access is denied
     */
    void checkMemberAccess(AST::Visibility visibility, 
                          const std::string& memberName,
                          Data::ClassType* targetClass);
};

} // namespace Interpreter
} // namespace Sad
