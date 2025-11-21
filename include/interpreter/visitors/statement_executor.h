/**
 * @file statement_executor.h
 * @brief (AR) منفّذ الجُمل - تنفيذ جميع أنواع الجُمل في لغة ص
 * @brief (EN) Statement Executor - executes all statement types in S Language
 * 
 * (AR) هذا الملف يحتوي على تعريف فئة StatementExecutor التي تستخدم نمط الزائر
 *      لتنفيذ جميع أنواع الجُمل في لغة ص. يتكامل مع ExpressionEvaluator لتقييم
 *      التعابير ضمن الجُمل.
 * 
 * (EN) This file contains the StatementExecutor class definition that uses the
 *      Visitor pattern to execute all statement types in S Language. It integrates
 *      with ExpressionEvaluator to evaluate expressions within statements.
 * 
 * Statement Executor is responsible for:
 * - Executing variable declarations (VarDeclStmt)
 * - Executing expression statements (ExprStmt)
 * - Managing code blocks and scopes (BlockStmt)
 * - Executing control flow (IfStmt, WhileStmt, ForStmt)
 * - Handling function declarations and calls (FuncDeclStmt, CallExpr)
 * - Managing return, break, continue statements
 * - Exception handling (TryStmt, RaiseStmt)
 * 
 * Features:
 * - Inherits from BaseASTVisitor to implement Visitor pattern
 * - Integrates with ExpressionEvaluator for expression evaluation
 * - Manages variable scopes using ScopeManager
 * - Handles control flow with special flow control states
 * - Full support for all statement types
 * - Clear bilingual error messages
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#pragma once

#include "../../parser/ast/ast_visitor.h"
#include "../../parser/ast/statements.h"
#include "../../parser/ast/expressions.h"
#include "../../data/types/value.h"
#include "../../data/managers/variable_manager.h"
#include "../../data/managers/function_manager.h"
#include "../../data/scope/scope_manager.h"
#include "expression_evaluator.h"
#include <stdexcept>
#include <string>
#include <memory>

namespace Sad {
namespace Interpreter {

/**
 * @brief (AR) حالات التحكم بالتدفق
 * @brief (EN) Flow control states
 */
enum class FlowControl {
    NONE,       ///< (AR) تدفق عادي / (EN) Normal flow
    RETURN,     ///< (AR) إرجاع من دالة / (EN) Return from function
    BREAK,      ///< (AR) كسر حلقة / (EN) Break from loop
    CONTINUE    ///< (AR) متابعة للتكرار التالي / (EN) Continue to next iteration
};

/**
 * @brief (AR) استثناء وقت التشغيل
 * @brief (EN) Runtime exception
 */
class ExecutionError : public std::runtime_error {
public:
    ExecutionError(const std::string& message) : std::runtime_error(message) {}
};

/**
 * @brief (AR) منفّذ الجُمل - ينفذ نمط الزائر لتنفيذ AST
 * @brief (EN) Statement Executor - implements Visitor pattern to execute AST
 * 
 * هذا الصنف يجتاز شجرة AST وينفذ كل جملة، مع إدارة النطاقات والتدفق
 * This class traverses the AST and executes each statement, managing scopes and flow
 * 
 * @example مثال الاستخدام / Usage Example:
 * @code{.cpp}
 * Data::VariableManager varMgr;
 * Data::FunctionManager funcMgr;
 * Data::ScopeManager scopeMgr;
 * StatementExecutor executor(varMgr, funcMgr, scopeMgr);
 * 
 * // تنفيذ كتلة كود
 * auto block = std::make_unique<BlockStmt>(...);
 * block->accept(executor);
 * @endcode
 */
class StatementExecutor : public AST::BaseASTVisitor {
public:
    /**
     * @brief (AR) البناء
     * @brief (EN) Constructor
     * 
     * @param varMgr (AR) مرجع لمدير المتغيرات / (EN) Reference to variable manager
     * @param funcMgr (AR) مرجع لمدير الدوال / (EN) Reference to function manager
     * @param scopeMgr (AR) مرجع لمدير النطاقات / (EN) Reference to scope manager
     */
    StatementExecutor(Data::VariableManager& varMgr, 
                     Data::FunctionManager& funcMgr,
                     Data::ScopeManager& scopeMgr);
    
    /**
     * @brief (AR) الحصول على حالة التحكم بالتدفق الحالية
     * @brief (EN) Get current flow control state
     */
    FlowControl getFlowControl() const { return flowControl_; }
    
    /**
     * @brief (AR) إعادة تعيين حالة التحكم بالتدفق
     * @brief (EN) Reset flow control state
     */
    void resetFlowControl() { flowControl_ = FlowControl::NONE; }
    
    /**
     * @brief (AR) الحصول على قيمة الإرجاع (للدوال)
     * @brief (EN) Get return value (for functions)
     */
    Data::Value getReturnValue() const { return returnValue_; }
    
    // =========================================================================
    // (AR) زيارة الجُمل / (EN) Statement Visitors
    // =========================================================================
    
    /**
     * @brief (AR) زيارة جملة تعبير / (EN) Visit expression statement
     * @details تنفذ تعبير مستقل كجملة / Executes standalone expression as statement
     */
    void visitExprStmt(AST::ExprStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة تعريف متغير / (EN) Visit variable declaration statement
     * @details تعرّف متغير جديد مع قيمة اختيارية / Declares new variable with optional value
     */
    void visitVarDeclStmt(AST::VarDeclStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة إذا / (EN) Visit if statement
     * @details تنفذ فرع الشرط حسب التقييم / Executes conditional branch based on evaluation
     */
    void visitIfStmt(AST::IfStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة بينما / (EN) Visit while statement
     * @details تنفذ حلقة while مع دعم break/continue / Executes while loop with break/continue support
     */
    void visitWhileStmt(AST::WhileStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة لكل / (EN) Visit for statement
     * @details تنفذ حلقة for بأسلوب C / Executes C-style for loop
     */
    void visitForStmt(AST::ForStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة لكل في / (EN) Visit for-range statement
     * @details تنفذ حلقة for-each على مجموعة / Executes for-each loop over collection
     */
    void visitForRangeStmt(AST::ForRangeStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة إرجاع / (EN) Visit return statement
     * @details تُرجع من دالة مع قيمة اختيارية / Returns from function with optional value
     */
    void visitReturnStmt(AST::ReturnStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة كسر / (EN) Visit break statement
     * @details تكسر من حلقة / Breaks from loop
     */
    void visitBreakStmt(AST::BreakStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة متابعة / (EN) Visit continue statement
     * @details تنتقل للتكرار التالي / Continues to next iteration
     */
    void visitContinueStmt(AST::ContinueStmt& node) override;
    
    /**
     * @brief (AR) زيارة كتلة كود / (EN) Visit block statement
     * @details تنفذ كتلة جمل مع نطاق جديد / Executes block of statements with new scope
     */
    void visitBlockStmt(AST::BlockStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة محاولة-التقاط / (EN) Visit try statement
     * @details تعالج الاستثناءات / Handles exceptions
     */
    void visitTryStmt(AST::TryStmt& node) override;
    
    /**
     * @brief (AR) زيارة جملة رفع استثناء / (EN) Visit raise statement
     * @details ترفع استثناء / Raises exception
     */
    void visitRaiseStmt(AST::RaiseStmt& node) override;
    
    /**
     * @brief (AR) زيارة تصريح دالة / (EN) Visit function declaration
     * @details تسجل الدالة في FunctionManager / Registers function in FunctionManager
     */
    void visitFunctionDecl(AST::FunctionDecl& node) override;
    
private:
    // (AR) المراجع للمديرين / (EN) Manager references
    Data::VariableManager& variableManager_;
    Data::FunctionManager& functionManager_;
    Data::ScopeManager& scopeManager_;
    
    // (AR) مُقيِّم التعابير / (EN) Expression evaluator
    std::unique_ptr<ExpressionEvaluator> expressionEvaluator_;
    
    // (AR) حالة التحكم بالتدفق / (EN) Flow control state
    FlowControl flowControl_;
    
    // (AR) قيمة الإرجاع / (EN) Return value
    Data::Value returnValue_;
    
    // (AR) عداد مستوى الحلقات (للتحقق من break/continue) / (EN) Loop depth counter
    int loopDepth_;
    
    /**
     * @brief (AR) تقييم تعبير وإرجاع قيمته
     * @brief (EN) Evaluate expression and return its value
     */
    Data::Value evaluateExpression(AST::Expression& expr);
    
    /**
     * @brief (AR) التحقق إذا كنا داخل حلقة
     * @brief (EN) Check if we're inside a loop
     */
    bool isInLoop() const { return loopDepth_ > 0; }

public:
    /**
     * @brief (AR) تنفيذ جسم دالة
     * @brief (EN) Execute function body
     * @param body (AR) جسم الدالة / (EN) Function body
     * @return (AR) قيمة الإرجاع / (EN) Return value
     */
    Data::Value executeFunctionBody(AST::Statement& body);
    
    /**
     * @brief (AR) التحقق إذا يجب التوقف عن التنفيذ
     * @brief (EN) Check if we should stop execution
     */
    bool shouldStopExecution() const {
        return flowControl_ != FlowControl::NONE;
    }
};

} // namespace Interpreter
} // namespace Sad
