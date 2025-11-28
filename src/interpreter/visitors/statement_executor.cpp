/**
 * @file statement_executor.cpp
 * @brief (AR) تنفيذ منفّذ الجُمل
 * @brief (EN) Statement Executor Implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "../../../include/interpreter/visitors/statement_executor.h"
#include "../../../include/parser/ast/declarations.h"
#include "../../../include/errors/error_manager.h"
#include "../../../include/interpreter/exception.h"
#include <iostream>

namespace Sad {
namespace Interpreter {

// =========================================================================
// (AR) البناء والإعداد / (EN) Construction and Setup
// =========================================================================

StatementExecutor::StatementExecutor(Data::VariableManager& varMgr, 
                                   Data::FunctionManager& funcMgr,
                                   Data::ScopeManager& scopeMgr)
    : variableManager_(varMgr)
    , functionManager_(funcMgr)
    , scopeManager_(scopeMgr)
    , flowControl_(FlowControl::NONE)
    , returnValue_()
    , loopDepth_(0)
{
    // (AR) إنشاء مُقيِّم التعابير / (EN) Create expression evaluator
    // Note: Pass *this to allow ExpressionEvaluator to call back for function execution
    expressionEvaluator_ = std::make_unique<ExpressionEvaluator>(varMgr, funcMgr, scopeMgr, *this);
}

// =========================================================================
// (AR) دوال مساعدة خاصة / (EN) Private Helper Functions
// =========================================================================

Data::Value StatementExecutor::evaluateExpression(AST::Expression& expr) {
    expr.accept(*expressionEvaluator_);
    return expressionEvaluator_->getResult();
}

// =========================================================================
// (AR) تنفيذ زيارة الجُمل الأساسية / (EN) Basic Statement Visitors
// =========================================================================

void StatementExecutor::visitExprStmt(AST::ExprStmt& node) {
    // (AR) تنفيذ التعبير وتجاهل النتيجة / (EN) Execute expression and ignore result
    evaluateExpression(*node.expression);
}

void StatementExecutor::visitVarDeclStmt(AST::VarDeclStmt& node) {
    // (AR) تقييم القيمة الابتدائية إن وُجدت / (EN) Evaluate initializer if present
    Data::Value value;
    
    if (node.initializer) {
        value = evaluateExpression(*node.initializer);
        
        // (AR) تحويل النوع إذا لزم الأمر / (EN) Type conversion if needed
        if (node.type == Data::DataType::INTEGER && value.getType() == Data::ValueType::DOUBLE) {
            // (AR) تحويل عشري → رقم صحيح / (EN) Convert double → integer
            value = Data::Value(static_cast<int>(value.toDouble()));
        } else if (node.type == Data::DataType::FLOAT && value.getType() == Data::ValueType::INTEGER) {
            // (AR) تحويل رقم صحيح → عشري / (EN) Convert integer → double
            value = Data::Value(static_cast<double>(value.toInt()));
        }
    } else {
        // (AR) قيمة افتراضية حسب النوع / (EN) Default value based on type
        switch (node.type) {
            case Data::DataType::INTEGER:
                value = Data::Value(0);
                break;
            case Data::DataType::FLOAT:
                value = Data::Value(0.0);
                break;
            case Data::DataType::STRING:
                value = Data::Value("");
                break;
            case Data::DataType::BOOLEAN:
                value = Data::Value(false);
                break;
            default:
                value = Data::Value(); // VOID
                break;
        }
    }
    
    // (AR) تعريف المتغير / (EN) Define variable
    variableManager_.define(node.name, value);
}

void StatementExecutor::visitBlockStmt(AST::BlockStmt& node) {
    // (AR) دخول نطاق جديد / (EN) Enter new scope
    scopeManager_.pushScope(Data::ScopeType::BLOCK);
    
    // (AR) تنفيذ جميع الجمل في الكتلة / (EN) Execute all statements in block
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
        
        // (AR) إيقاف التنفيذ إذا كان هناك تحكم بالتدفق / (EN) Stop if flow control active
        if (shouldStopExecution()) {
            break;
        }
    }
    
    // (AR) الخروج من النطاق / (EN) Exit scope
    scopeManager_.popScope();
}

// =========================================================================
// (AR) تنفيذ جُمل التحكم بالتدفق / (EN) Control Flow Statement Visitors
// =========================================================================

void StatementExecutor::visitIfStmt(AST::IfStmt& node) {
    // (AR) تقييم الشرط / (EN) Evaluate condition
    Data::Value condition = evaluateExpression(*node.condition);
    
    // (AR) تنفيذ الفرع المناسب / (EN) Execute appropriate branch
    if (condition.toBool()) {
        node.thenBranch->accept(*this);
    } else if (node.elseBranch) {
        node.elseBranch->accept(*this);
    }
}

void StatementExecutor::visitWhileStmt(AST::WhileStmt& node) {
    // (AR) زيادة عمق الحلقة / (EN) Increase loop depth
    loopDepth_++;
    
    // (AR) تنفيذ الحلقة / (EN) Execute loop
    while (true) {
        // (AR) تقييم الشرط / (EN) Evaluate condition
        Data::Value condition = evaluateExpression(*node.condition);
        
        if (!condition.toBool()) {
            break;
        }
        
        // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
        node.body->accept(*this);
        
        // (AR) معالجة break / (EN) Handle break
        if (flowControl_ == FlowControl::BREAK) {
            flowControl_ = FlowControl::NONE;
            break;
        }
        
        // (AR) معالجة continue / (EN) Handle continue
        if (flowControl_ == FlowControl::CONTINUE) {
            flowControl_ = FlowControl::NONE;
            continue;
        }
        
        // (AR) معالجة return (يُمرر للأعلى) / (EN) Handle return (pass to caller)
        if (flowControl_ == FlowControl::RETURN) {
            break;
        }
    }
    
    // (AR) تقليل عمق الحلقة / (EN) Decrease loop depth
    loopDepth_--;
}

void StatementExecutor::visitForStmt(AST::ForStmt& node) {
    // (AR) دخول نطاق جديد للحلقة / (EN) Enter new scope for loop
    scopeManager_.pushScope(Data::ScopeType::LOOP);
    
    // (AR) تنفيذ التهيئة / (EN) Execute initializer
    if (node.initializer) {
        node.initializer->accept(*this);
    }
    
    // (AR) زيادة عمق الحلقة / (EN) Increase loop depth
    loopDepth_++;
    
    // (AR) تنفيذ الحلقة / (EN) Execute loop
    while (true) {
        // (AR) تقييم الشرط / (EN) Evaluate condition
        if (node.condition) {
            Data::Value condition = evaluateExpression(*node.condition);
            if (!condition.toBool()) {
                break;
            }
        }
        
        // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
        node.body->accept(*this);
        
        // (AR) معالجة break / (EN) Handle break
        if (flowControl_ == FlowControl::BREAK) {
            flowControl_ = FlowControl::NONE;
            break;
        }
        
        // (AR) معالجة continue / (EN) Handle continue
        if (flowControl_ == FlowControl::CONTINUE) {
            flowControl_ = FlowControl::NONE;
        }
        
        // (AR) معالجة return / (EN) Handle return
        if (flowControl_ == FlowControl::RETURN) {
            break;
        }
        
        // (AR) تنفيذ الزيادة / (EN) Execute increment
        if (node.increment) {
            evaluateExpression(*node.increment);
        }
    }
    
    // (AR) تقليل عمق الحلقة / (EN) Decrease loop depth
    loopDepth_--;
    
    // (AR) الخروج من نطاق الحلقة / (EN) Exit loop scope
    scopeManager_.popScope();
}

void StatementExecutor::visitForRangeStmt(AST::ForRangeStmt& node) {
    // (AR) تقييم المجموعة القابلة للتكرار / (EN) Evaluate iterable
    Data::Value iterable = evaluateExpression(*node.iterable);
    
    // (AR) دخول نطاق جديد / (EN) Enter new scope
    scopeManager_.pushScope(Data::ScopeType::LOOP);
    
    // (AR) زيادة عمق الحلقة / (EN) Increase loop depth
    loopDepth_++;
    
    // (AR) التكرار حسب نوع المجموعة / (EN) Iterate based on collection type
    if (iterable.isArray()) {
        const auto& arr = iterable.toArray();
        
        for (size_t i = 0; i < arr.size(); i++) {
            // (AR) تحديث أو تعريف متغير الحلقة / (EN) Update or define loop variable
            if (variableManager_.exists(node.variable)) {
                variableManager_.assign(node.variable, arr[i]);
            } else {
                variableManager_.define(node.variable, arr[i]);
            }
            
            // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
            node.body->accept(*this);
            
            // (AR) معالجة التحكم بالتدفق / (EN) Handle flow control
            if (flowControl_ == FlowControl::BREAK) {
                flowControl_ = FlowControl::NONE;
                break;
            }
            if (flowControl_ == FlowControl::CONTINUE) {
                flowControl_ = FlowControl::NONE;
                continue;
            }
            if (flowControl_ == FlowControl::RETURN) {
                break;
            }
        }
    }
    else if (iterable.isMap()) {
        const auto& map = iterable.toMap();
        
        for (const auto& [key, value] : map) {
            // (AR) تحديث أو تعريف متغير المفتاح / (EN) Update or define key variable
            if (variableManager_.exists(node.variable)) {
                variableManager_.assign(node.variable, Data::Value(key));
            } else {
                variableManager_.define(node.variable, Data::Value(key));
            }
            
            // (AR) تحديث أو تعريف متغير القيمة إن وُجد / (EN) Update or define value variable if present
            if (!node.valueVar.empty()) {
                if (variableManager_.exists(node.valueVar)) {
                    variableManager_.assign(node.valueVar, value);
                } else {
                    variableManager_.define(node.valueVar, value);
                }
            }
            
            // (AR) تنفيذ جسم الحلقة / (EN) Execute loop body
            node.body->accept(*this);
            
            // (AR) معالجة التحكم بالتدفق / (EN) Handle flow control
            if (flowControl_ == FlowControl::BREAK) {
                flowControl_ = FlowControl::NONE;
                break;
            }
            if (flowControl_ == FlowControl::CONTINUE) {
                flowControl_ = FlowControl::NONE;
                continue;
            }
            if (flowControl_ == FlowControl::RETURN) {
                break;
            }
        }
    }
    else {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_INVALID_CAST,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "نوع غير قابل للتكرار",
            "Non-iterable type"
        );
    }
    
    // (AR) تقليل عمق الحلقة / (EN) Decrease loop depth
    loopDepth_--;
    
    // (AR) الخروج من النطاق / (EN) Exit scope
    scopeManager_.popScope();
}

// =========================================================================
// (AR) تنفيذ جملة Switch-Case / (EN) Switch-Case Statement Execution
// =========================================================================

/**
 * @brief (AR) يُنفذ جملة switch-case: حالة تعبير ... نهاية
 *        (EN) Executes switch-case statement: case expression ... end
 * 
 * Evaluates the switch expression, then compares it against each case value.
 * Executes the body of the first matching case, or the default case if no match.
 * 
 * يُقيّم تعبير switch، ثم يقارنه مع كل قيمة حالة.
 * يُنفذ جسم أول حالة مطابقة، أو الحالة الافتراضية إذا لم يوجد تطابق.
 * 
 * Syntax / النحو:
 *   حالة <expression>
 *       عندما <value>: <statement>
 *       [افتراضي: <statement>]
 *   نهاية
 * 
 * @note Unlike C, there's no fall-through - only one case executes
 * @note على عكس C، لا يوجد fall-through - تُنفذ حالة واحدة فقط
 */
void StatementExecutor::visitSwitchStmt(AST::SwitchStmt& node) {
    // (AR) تقييم تعبير Switch / (EN) Evaluate switch expression
    Data::Value switchValue = evaluateExpression(*node.expression);
    
    // (AR) علامة لمعرفة إذا وجدنا تطابق / (EN) Flag to track if we found a match
    bool foundMatch = false;
    
    // (AR) التكرار عبر حالات Case / (EN) Iterate through case branches
    for (const auto& caseItem : node.cases) {
        // (AR) تقييم قيمة الحالة / (EN) Evaluate case value
        Data::Value caseValue = evaluateExpression(*caseItem.value);
        
        // (AR) مقارنة القيم / (EN) Compare values
        // Use operator== which returns Value, then convert to bool
        if ((switchValue == caseValue).toBool()) {
            // (AR) وجدنا تطابق، نفذ جسم الحالة / (EN) Found match, execute case body
            foundMatch = true;
            
            // (AR) تنفيذ جسم الحالة / (EN) Execute case body
            caseItem.body->accept(*this);
            
            // (AR) لا يوجد fall-through، اخرج بعد التنفيذ / (EN) No fall-through, exit after execution
            break;
        }
    }
    
    // (AR) إذا لم نجد تطابق، نفذ الحالة الافتراضية إن وُجدت / (EN) If no match, execute default case if present
    if (!foundMatch && node.defaultCase) {
        node.defaultCase->accept(*this);
    }
    
    // (AR) ملاحظة: لا حاجة لإدارة break/continue لأن switch ليس حلقة
    // (EN) Note: No need to handle break/continue as switch is not a loop
}

// =========================================================================
// (AR) تنفيذ جُمل إدارة التدفق / (EN) Flow Management Statement Visitors
// =========================================================================

void StatementExecutor::visitReturnStmt(AST::ReturnStmt& node) {
    // (AR) تقييم قيمة الإرجاع إن وُجدت / (EN) Evaluate return value if present
    if (node.value) {
        returnValue_ = evaluateExpression(*node.value);
    } else {
        returnValue_ = Data::Value(); // VOID
    }
    
    // (AR) تعيين حالة التحكم / (EN) Set flow control state
    flowControl_ = FlowControl::RETURN;
}

void StatementExecutor::visitBreakStmt(AST::BreakStmt& node) {
    // (AR) التحقق من أننا داخل حلقة / (EN) Check that we're inside a loop
    if (!isInLoop()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_STACK_OVERFLOW,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "'break' خارج حلقة",
            "'break' outside loop"
        );
        return;
    }
    
    // (AR) تعيين حالة التحكم / (EN) Set flow control state
    flowControl_ = FlowControl::BREAK;
}

void StatementExecutor::visitContinueStmt(AST::ContinueStmt& node) {
    // (AR) التحقق من أننا داخل حلقة / (EN) Check that we're inside a loop
    if (!isInLoop()) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_STACK_OVERFLOW,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "'continue' خارج حلقة",
            "'continue' outside loop"
        );
        return;
    }
    
    // (AR) تعيين حالة التحكم / (EN) Set flow control state
    flowControl_ = FlowControl::CONTINUE;
}

// =========================================================================
// (AR) تنفيذ معالجة الاستثناءات / (EN) Exception Handling Visitors
// =========================================================================

void StatementExecutor::visitTryStmt(AST::TryStmt& node) {
    try {
        // (AR) تنفيذ كتلة المحاولة / (EN) Execute try block
        node.tryBlock->accept(*this);
    }
    catch (const Interpreter::SadException& e) {
        // (AR) البحث عن بند التقاط مناسب / (EN) Find matching catch clause
        bool caught = false;
        
        for (auto& catchClause : node.catchClauses) {
            // (AR) حاليًا نلتقط جميع الأخطاء / (EN) Currently catch all errors
            // TODO: تطبيق مطابقة الأنواع / Implement type matching
            
            // (AR) دخول نطاق جديد / (EN) Enter new scope
            scopeManager_.pushScope(Data::ScopeType::BLOCK);
            
            // (AR) تعريف متغير الاستثناء / (EN) Define exception variable
            if (!catchClause.exceptionVar.empty()) {
                variableManager_.define(catchClause.exceptionVar, 
                    Data::Value(e.getMessage()));
            }
            
            // (AR) تنفيذ كتلة الالتقاط / (EN) Execute catch block
            catchClause.body->accept(*this);
            
            // (AR) الخروج من النطاق / (EN) Exit scope
            scopeManager_.popScope();
            
            caught = true;
            break;
        }
        
        // (AR) إعادة رفع الاستثناء إذا لم يُلتقط / (EN) Re-raise if not caught
        if (!caught) {
            throw;
        }
    }
    catch (const ExecutionError& e) {
        // (AR) التقاط ExecutionError القديم للتوافق / (EN) Catch old ExecutionError for compatibility
        bool caught = false;
        
        for (auto& catchClause : node.catchClauses) {
            scopeManager_.pushScope(Data::ScopeType::BLOCK);
            
            if (!catchClause.exceptionVar.empty()) {
                variableManager_.define(catchClause.exceptionVar, 
                    Data::Value(std::string(e.what())));
            }
            
            catchClause.body->accept(*this);
            scopeManager_.popScope();
            
            caught = true;
            break;
        }
        
        if (!caught) {
            throw;
        }
    }
    catch (...) {
        // (AR) التقاط استثناءات أخرى / (EN) Catch other exceptions
        bool caught = false;
        
        for (auto& catchClause : node.catchClauses) {
            scopeManager_.pushScope(Data::ScopeType::BLOCK);
            
            if (!catchClause.exceptionVar.empty()) {
                variableManager_.define(catchClause.exceptionVar, 
                    Data::Value("Unknown exception"));
            }
            
            catchClause.body->accept(*this);
            scopeManager_.popScope();
            
            caught = true;
            break;
        }
        
        if (!caught) {
            throw;
        }
    }
    
    // (AR) تنفيذ كتلة finally إن وُجدت / (EN) Execute finally block if present
    if (node.finallyBlock) {
        node.finallyBlock->accept(*this);
    }
}

void StatementExecutor::visitRaiseStmt(AST::RaiseStmt& node) {
    // (AR) تقييم تعبير الاستثناء / (EN) Evaluate exception expression
    Data::Value exceptionValue = evaluateExpression(*node.exception);
    
    // (AR) رفع الاستثناء / (EN) Raise exception
    throw Interpreter::RuntimeError(
        exceptionValue.toString(),
        node.position
    );
}

void StatementExecutor::visitFunctionDecl(AST::FunctionDecl& node) {
    // (AR) Helper function لتحويل DataType إلى string / (EN) Helper to convert DataType to string
    auto dataTypeToString = [](Data::DataType type) -> std::string {
        switch (type) {
            case Data::DataType::INTEGER: return "integer";
            case Data::DataType::FLOAT: return "float";
            case Data::DataType::STRING: return "string";
            case Data::DataType::BOOLEAN: return "boolean";
            case Data::DataType::NONE: return "none";
            case Data::DataType::ARRAY: return "array";
            case Data::DataType::MAP: return "map";
            case Data::DataType::TUPLE: return "tuple";
            case Data::DataType::FUNCTION: return "function";
            case Data::DataType::OBJECT: return "object";
            case Data::DataType::ENUM: return "enum";
            case Data::DataType::BYTE: return "byte";
            default: return "unknown";
        }
    };
    
    // (AR) تحويل المعاملات إلى تنسيق FunctionManager / (EN) Convert parameters to FunctionManager format
    std::vector<Data::FunctionParameter> params;
    for (const auto& param : node.parameters) {
        params.push_back(Data::FunctionParameter(
            param.name,
            dataTypeToString(param.type)
        ));
    }
    
    // (AR) تسجيل الدالة في FunctionManager / (EN) Register function in FunctionManager
    // Note: FunctionManager uses Parser::ASTNode (forward declaration)
    // We cast Statement* to this type with custom no-op deleter
    // The AST owns the actual memory, so the deleter does nothing
    std::shared_ptr<Parser::ASTNode> bodyNode(
        reinterpret_cast<Parser::ASTNode*>(node.body.get()),
        [](Parser::ASTNode*) {} // Empty deleter - AST owns the memory
    );
    
    functionManager_.defineFunction(node.name, params, bodyNode);
}

Data::Value StatementExecutor::executeFunctionBody(AST::Statement& body) {
    // (AR) حفظ الحالة الحالية / (EN) Save current state
    FlowControl previousFlowControl = flowControl_;
    Data::Value previousReturnValue = returnValue_;
    
    // (AR) إعادة تعيين الحالة / (EN) Reset state
    flowControl_ = FlowControl::NONE;
    returnValue_ = Data::Value();
    
    // (AR) تنفيذ جسم الدالة / (EN) Execute function body
    body.accept(*this);
    
    // (AR) الحصول على قيمة الإرجاع / (EN) Get return value
    Data::Value result = returnValue_;
    
    // (AR) استعادة الحالة السابقة / (EN) Restore previous state
    flowControl_ = previousFlowControl;
    returnValue_ = previousReturnValue;
    
    return result;
}

} // namespace Interpreter
} // namespace Sad
