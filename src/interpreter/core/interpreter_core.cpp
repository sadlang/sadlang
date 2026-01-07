/**
 * @file interpreter_core.cpp
 * @brief (AR) تنفيذ المفسر الأساسي
 * @brief (EN) Core Interpreter Implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "../../../include/interpreter/core/interpreter_core.h"
#include "../../../include/parser/ast/declarations.h"
#include "../../../include/parser/ast/statements.h"
#include "../../../include/errors/error_manager.h"
#include "../../../include/errors/source_location.h"
#include "../../../include/errors/error_codes.h"
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
    variableManager_ = std::make_shared<Data::VariableManager>(*scopeManager_);
    functionManager_ = std::make_shared<Data::FunctionManager>();
    
    // (AR) إنشاء منفذ العبارات أولاً / (EN) Create statement executor first
    statementExecutor_ = std::make_unique<StatementExecutor>(
        *variableManager_,
        *functionManager_,
        *scopeManager_
    );
    
    // (AR) ثم إنشاء مقيّم التعابير مع مرجع لمنفذ العبارات / (EN) Then create expression evaluator with statement executor reference
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
        
        // (AR) البحث عن الدالة الرئيسية / (EN) Search for main function
        AST::FunctionDecl* mainFunction = nullptr;
        bool hasMainFunction = false;
        
        // (AR) المرحلة الأولى: تسجيل جميع التصريحات والبحث عن main
        // (EN) Phase 1: Register all declarations and search for main
        for (const auto& stmt : program) {
            // (AR) التحقق إذا كانت هذه دالة رئيسية
            // (EN) Check if this is a main function
            if (auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(stmt.get())) {
                if (funcDecl->isMainFunction) {
                    hasMainFunction = true;
                    mainFunction = funcDecl;
                    
                    if (options_.enableDebugMode) {
                        std::cout << "(AR) تم العثور على الدالة الرئيسية!"
                                 << " / (EN) Found main function!" << std::endl;
                    }
                }
            }
        }
        
        // (AR) التحقق من صحة البرنامج عند وجود دالة رئيسية
        // (EN) Validate program structure when main function exists
        if (hasMainFunction) {
            for (const auto& stmt : program) {
                // (AR) التحقق من أن الجمل خارج الدوال هي تصريحات فقط وليست جمل تنفيذية
                // (EN) Check that top-level statements are declarations only, not executable statements
                bool isDeclaration = 
                    dynamic_cast<AST::FunctionDecl*>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::ClassDecl*>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::EnumDecl*>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::TemplateFunctionDecl*>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::TemplateClassDecl*>(stmt.get()) != nullptr ||
                    dynamic_cast<AST::NamespaceDecl*>(stmt.get()) != nullptr;
                
                // (AR) السماح بـ VarDeclStmt للمتغيرات العامة
                // (EN) Allow VarDeclStmt for global variables
                bool isGlobalVar = dynamic_cast<AST::VarDeclStmt*>(stmt.get()) != nullptr;
                
                if (!isDeclaration && !isGlobalVar) {
                    // (AR) جملة تنفيذية خارج الدوال - غير مسموح عند وجود main
                    // (EN) Executable statement outside functions - not allowed when main exists
                    
                    // (AR) إنشاء موقع من الجملة (افتراضي إذا لم يكن متاحاً)
                    // (EN) Create location from statement (default if not available)
                    Sad::Errors::SourceLocation location("<input>", 1, 1);
                    
                    Sad::Errors::ErrorManager::getInstance().reportError(
                        Sad::Errors::ErrorCode::SEM_TYPE_MISMATCH,  // (AR) استخدام خطأ دلالي عام / (EN) Use general semantic error
                        location,
                        "(AR) خطأ: عند وجود الدالة الرئيسية 'رئيسية'، يجب أن تكون جميع الجمل التنفيذية داخل دوال.\n"
                        "       لا يُسمح بكتابة كود تنفيذي خارج الدوال عندما يحتوي البرنامج على دالة رئيسية.\n"
                        "       الحل: ضع الكود داخل الدالة الرئيسية أو دالة أخرى.",
                        "(EN) Error: When main function 'رئيسية' exists, all executable statements must be inside functions.\n"
                        "       Writing executable code outside functions is not allowed when the program has a main function.\n"
                        "       Solution: Place the code inside the main function or another function."
                    );
                    
                    return ExecutionResult(false, Data::Value(), 
                        "(AR) خطأ: كود تنفيذي خارج الدوال عند وجود main / "
                        "(EN) Error: Executable code outside functions when main exists");
                }
            }
        }
        
        // (AR) تسجيل التصريحات (الدوال، الأصناف، المتغيرات العامة)
        // (EN) Register declarations (functions, classes, global variables)
        for (const auto& stmt : program) {
            // (AR) تنفيذ التصريحات فقط (تسجيل الدوال، الأصناف، المتغيرات العامة)
            // (EN) Execute declarations only (register functions, classes, global variables)
            auto result = executeStatement(*stmt);
            
            if (!result.success) {
                return result;  // (AR) فشل التنفيذ / (EN) Execution failed
            }
            
            // (AR) التحقق من وجود أخطاء بعد تنفيذ كل جملة / (EN) Check for errors after each statement
            if (Sad::Errors::ErrorManager::getInstance().hasErrors()) {
                return ExecutionResult(false, Data::Value(), "Runtime error occurred");
            }
        }
        
        // (AR) المرحلة الثانية: تنفيذ الدالة الرئيسية إذا وُجدت
        // (EN) Phase 2: Execute main function if found
        if (hasMainFunction && mainFunction) {
            if (options_.enableDebugMode) {
                std::cout << "(AR) تنفيذ الدالة الرئيسية..."
                         << " / (EN) Executing main function..." << std::endl;
            }
            
            // (AR) تنفيذ جسم الدالة الرئيسية باستخدام executeFunctionBodyWithReturnType
            // (EN) Execute main function body using executeFunctionBodyWithReturnType
            try {
                // (AR) إنشاء نطاق جديد للدالة الرئيسية
                // (EN) Create new scope for main function
                scopeManager_->pushScope(Data::ScopeType::FUNCTION, "main");
                
                // (AR) تنفيذ جسم الدالة مع نوع الإرجاع
                // (EN) Execute function body with return type
                Data::Value returnValue;
                if (mainFunction->body) {
                    // (AR) استخدام executeFunctionBodyWithReturnType لتعيين سياق الدالة بشكل صحيح
                    // (EN) Use executeFunctionBodyWithReturnType to properly set function context
                    returnValue = statementExecutor_->executeFunctionBodyWithReturnType(
                        *mainFunction->body,
                        mainFunction->returnType,
                        "رئيسية"  // (AR) اسم الدالة بالعربية / (EN) Function name in Arabic
                    );
                    
                    // (AR) إعادة تعيين التحكم في التدفق
                    // (EN) Reset flow control
                    statementExecutor_->resetFlowControl();
                } else {
                    // (AR) إذا لم يكن هناك جسم، إرجاع 0 كقيمة افتراضية
                    // (EN) If no body, return 0 as default
                    returnValue = Data::Value(0);
                }
                
                // (AR) إزالة النطاق
                // (EN) Pop scope
                scopeManager_->popScope();
                
                if (options_.enableDebugMode) {
                    std::cout << "(AR) اكتملت الدالة الرئيسية بقيمة إرجاع: "
                             << returnValue.toString()
                             << " / (EN) Main function completed with return value: "
                             << returnValue.toString() << std::endl;
                }
                
                return ExecutionResult(true, returnValue);
            }
            catch (const std::exception& e) {
                // (AR) التأكد من إزالة النطاق حتى في حالة الخطأ
                // (EN) Ensure scope is popped even on error
                try {
                    scopeManager_->popScope();
                } catch (...) {
                    // (AR) تجاهل أخطاء popScope في حالة الاستثناء
                    // (EN) Ignore popScope errors during exception
                }
                
                return ExecutionResult(false, Data::Value(),
                    std::string("(AR) خطأ في تنفيذ الدالة الرئيسية: ") + e.what() +
                    " / (EN) Error executing main function: " + e.what());
            }
        }
        
        // (AR) إذا لم توجد دالة رئيسية، التنفيذ العادي (الوضع القديم)
        // (EN) If no main function, normal execution (legacy mode)
        if (options_.enableDebugMode) {
            std::cout << "(AR) لا توجد دالة رئيسية - الوضع القديم"
                     << " / (EN) No main function - legacy mode" << std::endl;
        }
        
        return ExecutionResult(true, Data::Value());
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
