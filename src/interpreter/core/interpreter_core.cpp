/**
 * @file interpreter_core.cpp
 * @brief (AR) تنفيذ المفسر الأساسي
 * @brief (EN) Core Interpreter Implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "../../../include/interpreter/core/interpreter_core.h"
#include "../../../include/errors/error_manager.h"
#include <iostream>
#include <stdexcept>

namespace Sad {
namespace Interpreter {

// =========================================================================
// (AR) البناء والإعداد / (EN) Construction and Setup
// =========================================================================

Interpreter::Interpreter(const InterpreterOptions& options)
    : options_(options)
{
    initializeComponents();
}

void Interpreter::initializeComponents() {
    // (AR) إنشاء المديرين بالترتيب الصحيح / (EN) Create managers in correct order
    scopeManager_ = std::make_shared<Data::ScopeManager>();
    variableManager_ = std::make_shared<Data::VariableManager>();
    functionManager_ = std::make_shared<Data::FunctionManager>();
    
    // (AR) إنشاء المنفذين / (EN) Create executors
    statementExecutor_ = std::make_unique<StatementExecutor>(
        *variableManager_,
        *functionManager_,
        *scopeManager_
    );
    
    expressionEvaluator_ = std::make_unique<ExpressionEvaluator>(
        *variableManager_,
        *functionManager_,
        *scopeManager_,
        *statementExecutor_
    );
    
    // (AR) تسجيل جميع الدوال المضمنة / (EN) Register all built-in functions
    registerBuiltinFunctions(*this);
    
    if (options_.enableDebugMode) {
        std::cout << "(AR) تم تهيئة المفسر / (EN) Interpreter initialized" << std::endl;
    }
}

// =========================================================================
// (AR) تنفيذ البرنامج / (EN) Program Execution
// =========================================================================

ExecutionResult Interpreter::execute(const std::vector<std::unique_ptr<AST::Statement>>& program) {
    try {
        if (options_.enableDebugMode) {
            std::cout << "(AR) بدء تنفيذ البرنامج (" << program.size() << " جملة)"
                     << " / (EN) Starting program execution (" << program.size() << " statements)"
                     << std::endl;
        }
        
        // (AR) تنفيذ كل جملة / (EN) Execute each statement
        Data::Value lastValue;
        for (const auto& stmt : program) {
            auto result = executeStatement(*stmt);
            
            if (!result.success) {
                return result;  // (AR) فشل التنفيذ / (EN) Execution failed
            }
            
            // (AR) التحقق من وجود أخطاء بعد تنفيذ كل جملة / (EN) Check for errors after each statement
            if (Sad::Errors::ErrorManager::getInstance().hasErrors()) {
                return ExecutionResult(false, Data::Value(), "Runtime error occurred");
            }
            
            lastValue = result.result;
            
            if (options_.printResults && !lastValue.isVoid()) {
                std::cout << lastValue.toString() << std::endl;
            }
        }
        
        if (options_.enableDebugMode) {
            std::cout << "(AR) اكتمل التنفيذ بنجاح / (EN) Execution completed successfully" << std::endl;
        }
        
        return ExecutionResult(true, lastValue);
    }
    catch (const ExecutionError& e) {
        std::string errorMsg = "(AR) خطأ في التنفيذ: " + std::string(e.what()) +
                              " / (EN) Execution error: " + std::string(e.what());
        
        if (options_.enableDebugMode) {
            std::cerr << errorMsg << std::endl;
        }
        
        return ExecutionResult(false, Data::Value(), errorMsg);
    }
    catch (const RuntimeError& e) {
        std::string errorMsg = "(AR) خطأ في وقت التشغيل: " + std::string(e.what()) +
                              " / (EN) Runtime error: " + std::string(e.what());
        
        if (options_.enableDebugMode) {
            std::cerr << errorMsg << std::endl;
        }
        
        return ExecutionResult(false, Data::Value(), errorMsg);
    }
    catch (const std::exception& e) {
        std::string errorMsg = "(AR) خطأ غير متوقع: " + std::string(e.what()) +
                              " / (EN) Unexpected error: " + std::string(e.what());
        
        if (options_.enableDebugMode) {
            std::cerr << errorMsg << std::endl;
        }
        
        return ExecutionResult(false, Data::Value(), errorMsg);
    }
}

ExecutionResult Interpreter::executeStatement(AST::Statement& statement) {
    try {
        // (AR) تنفيذ الجملة / (EN) Execute statement
        statement.accept(*statementExecutor_);
        
        // (AR) التحقق من وجود return / (EN) Check for return
        if (statementExecutor_->getFlowControl() == FlowControl::RETURN) {
            Data::Value returnValue = statementExecutor_->getReturnValue();
            statementExecutor_->resetFlowControl();
            return ExecutionResult(true, returnValue);
        }
        
        // (AR) نجح التنفيذ بدون إرجاع / (EN) Execution succeeded without return
        return ExecutionResult(true);
    }
    catch (const ExecutionError& e) {
        return ExecutionResult(false, Data::Value(), std::string(e.what()));
    }
    catch (const RuntimeError& e) {
        return ExecutionResult(false, Data::Value(), std::string(e.what()));
    }
    catch (const std::exception& e) {
        return ExecutionResult(false, Data::Value(), std::string(e.what()));
    }
}

Data::Value Interpreter::evaluateExpression(AST::Expression& expression) {
    expression.accept(*expressionEvaluator_);
    return expressionEvaluator_->getResult();
}

// =========================================================================
// (AR) إدارة الحالة / (EN) State Management
// =========================================================================

void Interpreter::reset() {
    if (options_.enableDebugMode) {
        std::cout << "(AR) إعادة تعيين المفسر / (EN) Resetting interpreter" << std::endl;
    }
    
    // (AR) إعادة إنشاء جميع المكونات / (EN) Recreate all components
    initializeComponents();
}

} // namespace Interpreter
} // namespace Sad
