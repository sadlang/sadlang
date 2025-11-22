/**
 * @file interpreter_core.h
 * @brief (AR) المفسر الأساسي - يدير تنفيذ برامج لغة ص
 * @brief (EN) Core Interpreter - manages execution of S Language programs
 * 
 * الوصف بالعربية:
 * =================
 * المفسر الأساسي هو نقطة الدخول الرئيسية لتنفيذ برامج لغة ص.
 * يُنسق بين جميع المكونات:
 * - إدارة المتغيرات (VariableManager)
 * - إدارة الدوال (FunctionManager)
 * - إدارة النطاقات (ScopeManager)
 * - تقييم التعابير (ExpressionEvaluator)
 * - تنفيذ الجُمل (StatementExecutor)
 * 
 * المميزات:
 * - واجهة بسيطة: execute(AST)
 * - إدارة تلقائية للموارد
 * - رسائل خطأ واضحة ثنائية اللغة
 * - دعم كامل لجميع ميزات اللغة
 * 
 * English Description:
 * ====================
 * The Core Interpreter is the main entry point for executing S Language programs.
 * It coordinates all components:
 * - Variable Management (VariableManager)
 * - Function Management (FunctionManager)
 * - Scope Management (ScopeManager)
 * - Expression Evaluation (ExpressionEvaluator)
 * - Statement Execution (StatementExecutor)
 * 
 * Features:
 * - Simple interface: execute(AST)
 * - Automatic resource management
 * - Clear bilingual error messages
 * - Full support for all language features
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 * @version 1.0
 */

#pragma once

#include "../../parser/ast/ast_node.h"
#include "../../parser/ast/statements.h"
#include "../../data/types/value.h"
#include "../../data/managers/variable_manager.h"
#include "../../data/managers/function_manager.h"
#include "../../data/scope/scope_manager.h"
#include "../visitors/expression_evaluator.h"
#include "../visitors/statement_executor.h"
#include <memory>
#include <string>
#include <vector>

namespace Sad {
namespace Interpreter {

/**
 * @brief (AR) خيارات المفسر
 * @brief (EN) Interpreter options
 */
struct InterpreterOptions {
    bool enableDebugMode = false;       ///< (AR) تفعيل وضع التصحيح / (EN) Enable debug mode
    bool enableStrictMode = false;      ///< (AR) تفعيل الوضع الصارم / (EN) Enable strict mode
    bool printResults = false;          ///< (AR) طباعة النتائج / (EN) Print results
    size_t maxCallDepth = 1000;        ///< (AR) أقصى عمق للاستدعاءات / (EN) Maximum call depth
};

/**
 * @brief (AR) نتيجة التنفيذ
 * @brief (EN) Execution result
 */
struct ExecutionResult {
    bool success = false;               ///< (AR) نجح التنفيذ / (EN) Execution succeeded
    Data::Value result;                 ///< (AR) النتيجة / (EN) Result value
    std::string errorMessage;           ///< (AR) رسالة الخطأ / (EN) Error message
    
    ExecutionResult() = default;
    
    explicit ExecutionResult(bool s, const Data::Value& r = Data::Value(), const std::string& err = "")
        : success(s), result(r), errorMessage(err) {}
};

/**
 * @brief (AR) المفسر الأساسي - ينفذ برامج لغة ص
 * @brief (EN) Core Interpreter - executes S Language programs
 * 
 * هذا الصنف يُنسق بين جميع مكونات المفسر لتنفيذ شجرة AST
 * This class coordinates all interpreter components to execute an AST
 * 
 * @example مثال الاستخدام / Usage Example:
 * @code{.cpp}
 * using namespace Sad::Interpreter;
 * 
 * // إنشاء مفسر
 * Interpreter interpreter;
 * 
 * // تنفيذ AST
 * auto result = interpreter.execute(programAST);
 * 
 * if (result.success) {
 *     std::cout << "النتيجة: " << result.result.toString() << std::endl;
 * } else {
 *     std::cerr << "خطأ: " << result.errorMessage << std::endl;
 * }
 * @endcode
 */
class Interpreter {
public:
    /**
     * @brief (AR) البناء - ينشئ مفسر جديد
     * @brief (EN) Constructor - creates new interpreter
     * 
     * @param options (AR) خيارات المفسر / (EN) Interpreter options
     */
    explicit Interpreter(const InterpreterOptions& options = InterpreterOptions());
    
    /**
     * @brief (AR) تنفيذ برنامج من AST
     * @brief (EN) Execute program from AST
     * 
     * @param program (AR) برنامج (قائمة جمل) / (EN) Program (list of statements)
     * @return (AR) نتيجة التنفيذ / (EN) Execution result
     */
    ExecutionResult execute(const std::vector<std::unique_ptr<AST::Statement>>& program);
    
    /**
     * @brief (AR) تنفيذ جملة واحدة
     * @brief (EN) Execute single statement
     * 
     * @param statement (AR) الجملة / (EN) Statement to execute
     * @return (AR) نتيجة التنفيذ / (EN) Execution result
     */
    ExecutionResult executeStatement(AST::Statement& statement);
    
    /**
     * @brief (AR) تقييم تعبير
     * @brief (EN) Evaluate expression
     * 
     * @param expression (AR) التعبير / (EN) Expression to evaluate
     * @return (AR) القيمة الناتجة / (EN) Resulting value
     */
    Data::Value evaluateExpression(AST::Expression& expression);
    
    /**
     * @brief (AR) إعادة تعيين المفسر (مسح جميع المتغيرات والدوال)
     * @brief (EN) Reset interpreter (clear all variables and functions)
     */
    void reset();
    
    /**
     * @brief (AR) الحصول على مدير المتغيرات
     * @brief (EN) Get variable manager
     */
    Data::VariableManager& getVariableManager() { return *variableManager_; }
    
    /**
     * @brief (AR) الحصول على مدير الدوال
     * @brief (EN) Get function manager
     */
    Data::FunctionManager& getFunctionManager() { return *functionManager_; }
    
    /**
     * @brief (AR) الحصول على مدير النطاقات
     * @brief (EN) Get scope manager
     */
    Data::ScopeManager& getScopeManager() { return *scopeManager_; }
    
    /**
     * @brief (AR) الحصول على الخيارات
     * @brief (EN) Get options
     */
    const InterpreterOptions& getOptions() const { return options_; }
    
    /**
     * @brief (AR) تعيين خيارات جديدة
     * @brief (EN) Set new options
     */
    void setOptions(const InterpreterOptions& options) { options_ = options; }
    
private:
    // (AR) خيارات المفسر / (EN) Interpreter options
    InterpreterOptions options_;
    
    // (AR) المديرون / (EN) Managers
    std::shared_ptr<Data::ScopeManager> scopeManager_;
    std::shared_ptr<Data::VariableManager> variableManager_;
    std::shared_ptr<Data::FunctionManager> functionManager_;
    
    // (AR) المنفذون / (EN) Executors
    std::unique_ptr<ExpressionEvaluator> expressionEvaluator_;
    std::unique_ptr<StatementExecutor> statementExecutor_;
    
    /**
     * @brief (AR) تهيئة المكونات
     * @brief (EN) Initialize components
     */
    void initializeComponents();
};

// Forward declaration for built-in function registration
// (AR) تصريح تقديمي لتسجيل الدوال المضمنة
// (EN) Forward declaration for built-in function registration
void registerBuiltinFunctions(Interpreter& interpreter);

} // namespace Interpreter
} // namespace Sad
