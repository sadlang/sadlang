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

#include "../../parser/ast/ast_visitor.h"
#include "../../parser/ast/expressions.h"
#include "../../data/types/value.h"
#include "../../data/managers/variable_manager.h"
#include "../../data/managers/function_manager.h"
#include "../../lexer/token.h"
#include <stdexcept>
#include <string>

// Forward declaration to avoid circular dependency
namespace Sad {
namespace Interpreter {
    class StatementExecutor;
}
}

namespace Sad {
namespace Interpreter {

/**
 * @brief (AR) استثناء وقت التشغيل
 * @brief (EN) Runtime exception
 */
class RuntimeError : public std::runtime_error {
public:
    RuntimeError(const std::string& message) : std::runtime_error(message) {}
};

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
                       StatementExecutor& executor)
        : variableManager_(varMgr), 
          functionManager_(funcMgr), 
          scopeManager_(scopeMgr),
          statementExecutor_(executor),
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
    
private:
    Data::VariableManager& variableManager_;    ///< (AR) مدير المتغيرات / (EN) Variable manager
    Data::FunctionManager& functionManager_;    ///< (AR) مدير الدوال / (EN) Function manager
    Data::ScopeManager& scopeManager_;          ///< (AR) مدير النطاقات / (EN) Scope manager
    StatementExecutor& statementExecutor_;      ///< (AR) منفذ العبارات / (EN) Statement executor
    Data::Value lastResult_;                    ///< (AR) آخر نتيجة / (EN) Last result
    
    /**
     * @brief (AR) تنفيذ عملية ثنائية حسابية
     * @brief (EN) Execute arithmetic binary operation
     */
    Data::Value evaluateArithmeticOp(const Data::Value& left, Lexer::TokenType op, const Data::Value& right);
    
    /**
     * @brief (AR) تنفيذ عملية مقارنة
     * @brief (EN) Execute comparison operation
     */
    Data::Value evaluateComparisonOp(const Data::Value& left, Lexer::TokenType op, const Data::Value& right);
    
    /**
     * @brief (AR) تنفيذ عملية منطقية
     * @brief (EN) Execute logical operation
     */
    Data::Value evaluateLogicalOp(const Data::Value& left, Lexer::TokenType op, const Data::Value& right);
    
    /**
     * @brief (AR) تحويل Token إلى Value
     * @brief (EN) Convert Token to Value
     */
    Data::Value tokenToValue(const Lexer::Token& token);
};

} // namespace Interpreter
} // namespace Sad
