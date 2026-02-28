/**
 * @file statement_executor.cpp
 * @brief (AR) تنفيذ منفّذ الجُمل
 * @brief (EN) Statement Executor Implementation
 * 
 * @author S Language Development Team
 * @date November 21, 2025
 */

#include "statement_executor.h"
#include "declarations.h"
#include "pattern_nodes.h"
#include "error_manager.h"
#include "exception.h"
#include "object_instance.h"
#include <iostream>
#include <sstream>
#include <map>

namespace Sad {
namespace Interpreter {

// =========================================================================
// (AR) البناء والإعداد / (EN) Construction and Setup
// =========================================================================

StatementExecutor::StatementExecutor(Data::VariableManager& varMgr, 
                                   Data::FunctionManager& funcMgr,
                                   Data::ScopeManager& scopeMgr,
                                   Data::OwnershipManager& ownershipMgr)
    : variableManager_(varMgr)
    , functionManager_(funcMgr)
    , scopeManager_(scopeMgr)
    , ownershipManager_(ownershipMgr)
    , flowControl_(FlowControl::NONE)
    , returnValue_()
    , yieldValue_()
    , inGenerator_(false)
    , loopDepth_(0)
    , currentFunctionReturnType_(Data::DataType::UNKNOWN)
    , currentFunctionName_("")
{
    // (AR) إنشاء مُقيِّم التعابير / (EN) Create expression evaluator
    // Note: Pass *this to allow ExpressionEvaluator to call back for function execution
    expressionEvaluator_ = std::make_unique<ExpressionEvaluator>(varMgr, funcMgr, scopeMgr, *this, ownershipMgr);
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
    try {
        if (node.isConst) {
            variableManager_.defineConst(node.name, value);
        } else {
            variableManager_.define(node.name, value);
        }
        
        // (AR) تسجيل المتغير في نظام الملكية / (EN) Register variable in ownership system
        if (ownershipManager_.isEnabled()) {
            std::string typeName;
            switch (node.type) {
                case Data::DataType::INTEGER: typeName = "INTEGER"; break;
                case Data::DataType::FLOAT: typeName = "FLOAT"; break;
                case Data::DataType::STRING: typeName = "نص"; break;
                case Data::DataType::BOOLEAN: typeName = "BOOLEAN"; break;
                case Data::DataType::ARRAY: typeName = "مصفوفة"; break;
                case Data::DataType::MAP: typeName = "قاموس"; break;
                case Data::DataType::OBJECT: typeName = "كائن"; break;
                default: {
                    // (AR) استدلال النوع من القيمة عندما يكون النوع غير محدد (متغير)
                    // (EN) Infer type from value when type is unspecified (متغير keyword)
                    auto vt = value.getType();
                    if (vt == Data::ValueType::INTEGER) typeName = "INTEGER";
                    else if (vt == Data::ValueType::DOUBLE) typeName = "FLOAT";
                    else if (vt == Data::ValueType::STRING) typeName = "نص";
                    else if (vt == Data::ValueType::BOOLEAN) typeName = "BOOLEAN";
                    else if (vt == Data::ValueType::ARRAY) typeName = "مصفوفة";
                    else if (vt == Data::ValueType::MAP) typeName = "قاموس";
                    else typeName = "";
                    break;
                }
            }
            ownershipManager_.declareVariable(node.name, typeName);
            
            // (AR) نقل الملكية: إذا كان المُهيّئ متغيراً، انقل ملكيته
            // (EN) Move semantics: if initializer is a variable, move ownership from it
            if (node.initializer) {
                auto* varExpr = dynamic_cast<AST::VariableExpr*>(node.initializer.get());
                if (varExpr && !varExpr->name.empty() && varExpr->name != node.name) {
                    auto moveError = ownershipManager_.moveVariable(varExpr->name);
                    if (moveError.has_value()) {
                        throw SadException(
                            moveError->arabicMessage + " / " + moveError->message,
                            "OwnershipError",
                            node.position
                        );
                    }
                }
            }
        }
    }
    catch (const std::runtime_error& e) {
        // (AR) إضافة معلومات الموقع للخطأ / (EN) Add position info to error
        std::ostringstream oss;
        oss << e.what() << "\n"
            << "📍 (AR) الموقع / (EN) Location: السطر / Line " << node.position.line 
            << "، العمود / Column " << node.position.column;
        throw SadException(oss.str(), "RuntimeError", node.position);
    }
}

void StatementExecutor::visitBlockStmt(AST::BlockStmt& node) {
    // (AR) دخول نطاق جديد / (EN) Enter new scope
    variableManager_.enterScope(Data::ScopeType::BLOCK);
    ownershipManager_.enterScope();
    
    // (AR) تنفيذ جميع الجمل في الكتلة / (EN) Execute all statements in block
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
        
        // (AR) إيقاف التنفيذ إذا كان هناك تحكم بالتدفق / (EN) Stop if flow control active
        if (shouldStopExecution()) {
            break;
        }
    }
    
    // (AR) الخروج من النطاق / (EN) Exit scope
    ownershipManager_.exitScope();
    variableManager_.exitScope();
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
    variableManager_.enterScope(Data::ScopeType::LOOP);
    
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
    variableManager_.exitScope();
}

void StatementExecutor::visitForRangeStmt(AST::ForRangeStmt& node) {
    // (AR) تقييم المجموعة القابلة للتكرار / (EN) Evaluate iterable
    Data::Value iterable = evaluateExpression(*node.iterable);
    
    // (AR) دخول نطاق جديد / (EN) Enter new scope
    variableManager_.enterScope(Data::ScopeType::LOOP);
    
    // (AR) زيادة عمق الحلقة / (EN) Increase loop depth
    loopDepth_++;
    
    // (AR) التكرار حسب نوع المجموعة / (EN) Iterate based on collection type
    if (iterable.isArray()) {
        const auto& arr = iterable.toArrayRef();
        
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
    else if (iterable.isString()) {
        // (AR) التكرار على أحرف النص مع دعم UTF-8
        // (EN) Iterate over string characters with UTF-8 support
        std::string text = iterable.toString();
        const unsigned char* bytes = reinterpret_cast<const unsigned char*>(text.data());
        size_t len = text.size();
        size_t i = 0;
        
        while (i < len) {
            // (AR) تحديد طول الحرف UTF-8 / (EN) Determine UTF-8 character byte length
            size_t charLen = 1;
            unsigned char b = bytes[i];
            if (b >= 0xF0) charLen = 4;
            else if (b >= 0xE0) charLen = 3;
            else if (b >= 0xC0) charLen = 2;
            
            if (i + charLen > len) charLen = len - i; // (AR) حماية الحدود
            std::string ch = text.substr(i, charLen);
            i += charLen;
            
            // (AR) تحديث أو تعريف متغير الحلقة / (EN) Update or define loop variable
            if (variableManager_.exists(node.variable)) {
                variableManager_.assign(node.variable, Data::Value(ch));
            } else {
                variableManager_.define(node.variable, Data::Value(ch));
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
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "نوع غير قابل للتكرار",
            "Non-iterable type"
        );
    }
    
    // (AR) تقليل عمق الحلقة / (EN) Decrease loop depth
    loopDepth_--;
    
    // (AR) الخروج من النطاق / (EN) Exit scope
    variableManager_.exitScope();
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
            
            // (AR) تنفيذ جسم الحالة في نطاق خاص / (EN) Execute case body in its own scope
            variableManager_.enterScope(Data::ScopeType::BLOCK);
            caseItem.body->accept(*this);
            variableManager_.exitScope();
            
            // (AR) لا يوجد fall-through، اخرج بعد التنفيذ / (EN) No fall-through, exit after execution
            break;
        }
    }
    
    // (AR) إذا لم نجد تطابق، نفذ الحالة الافتراضية إن وُجدت / (EN) If no match, execute default case if present
    if (!foundMatch && node.defaultCase) {
        variableManager_.enterScope(Data::ScopeType::BLOCK);
        node.defaultCase->accept(*this);
        variableManager_.exitScope();
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
        
        // (AR) التحقق من توافق نوع الإرجاع / (EN) Check return type compatibility
        // (AR) إذا كان نوع الإرجاع UNKNOWN، فهذا يعني أن الدالة ديناميكية - لا خطأ
        // (EN) If return type is UNKNOWN, function is dynamically typed - no error
        // (AR) لا نبلغ عن خطأ لأن الدوال بدون نوع إرجاع صريح مسموحة في لغة ص
        // (EN) We don't report error because functions without explicit return type are allowed in Sad
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
            Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "'اخرج' خارج حلقة",
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
            Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "'تابع' خارج حلقة",
            "'continue' outside loop"
        );
        return;
    }
    
    // (AR) تعيين حالة التحكم / (EN) Set flow control state
    flowControl_ = FlowControl::CONTINUE;
}

// =========================================================================
// (AR) تنفيذ جملة yield (للمولّدات) / (EN) Yield Statement (for generators)
// =========================================================================

void StatementExecutor::visitYieldStmt(AST::YieldStmt& node) {
    // (AR) التحقق من أننا داخل مولّد / (EN) Check that we're inside a generator
    if (!inGenerator_) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
            Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
            "(AR) 'أنتج' (yield) يستخدم فقط داخل الدوال المولّدة.\n"
            "الحل: استخدم 'دالة مولّد' بدلاً من 'دالة' لتعريف المولّد.",
            "(EN) 'yield' can only be used inside generator functions.\n"
            "Solution: Use 'generator function' instead of 'function' to define a generator."
        );
        return;
    }
    
    // (AR) التحقق من نوع yield / (EN) Check yield type
    if (node.isYieldFrom) {
        // (AR) yield from - تفويض إلى مولّد آخر
        // (EN) yield from - delegate to another generator
        if (node.value) {
            Data::Value iterable = evaluateExpression(*node.value);
            
            if (!iterable.isArray()) {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::RUN_INVALID_CAST,
                    Sad::Errors::SourceLocation("<input>", static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "(AR) 'yield from' يتطلب قيمة قابلة للتكرار (مصفوفة).",
                    "(EN) 'yield from' requires an iterable value (array)."
                );
                return;
            }
            
            // (AR) yield from يُنتج كل عنصر من iterable
            // (EN) yield from produces each element from iterable
            // Note: في المستقبل، يمكن دعم generators متداخلة
            // Note: In future, can support nested generators
            yieldValue_ = iterable;
        } else {
            yieldValue_ = Data::Value(); // VOID
        }
    } else {
        // (AR) yield عادي - إنتاج قيمة واحدة
        // (EN) Regular yield - produce single value
        if (node.value) {
            yieldValue_ = evaluateExpression(*node.value);
        } else {
            yieldValue_ = Data::Value(); // VOID
        }
    }
    
    // (AR) في وضع المولّد: جمع القيم بدون إيقاف التنفيذ
    // (EN) In generator mode: collect values without stopping execution
    if (inGenerator_) {
        generatorYieldValues_.push_back(yieldValue_);
        // (AR) لا نُعيّن flowControl_ حتى يستمر التنفيذ
        // (EN) Don't set flowControl_ so execution continues
        return;
    }
    
    // (AR) تعيين حالة التحكم / (EN) Set flow control state
    flowControl_ = FlowControl::YIELD;
}

// =========================================================================
// (AR) تنفيذ مدير السياق (With Statement) / (EN) Context Manager (With Statement) Visitors
// =========================================================================

void StatementExecutor::visitWithStmt(AST::WithStmt& node) {
    // (AR) تقييم تعبير المورد / (EN) Evaluate resource expression
    Data::Value resource = evaluateExpression(*node.resource);
    
    // (AR) القيمة التي ستُعيّن للمتغير المستعار / (EN) Value to assign to alias variable
    Data::Value contextValue = resource;
    
    // (AR) إذا كان المورد كائناً، نحاول استدعاء __دخول__() أو __enter__()
    // (EN) If resource is object, try calling __دخول__() or __enter__()
    bool hasEnterExit = false;
    if (resource.isObject()) {
        auto obj = resource.toObject();
        if (obj && (obj->hasMethod("__دخول__") || obj->hasMethod("__enter__"))) {
            hasEnterExit = true;
            // (AR) استدعاء __دخول__() — نبحث عن الطريقة ونستدعيها
            // (EN) Call __enter__() — find the method and invoke it
            std::string enterName = obj->hasMethod("__دخول__") ? "__دخول__" : "__enter__";
            auto* enterMethod = obj->getMethod(enterName);
            if (enterMethod && enterMethod->body) {
                variableManager_.enterScope(Data::ScopeType::FUNCTION, enterName);
                variableManager_.define("هذا", resource);
                enterMethod->body->accept(*this);
                contextValue = returnValue_;
                variableManager_.exitScope();
            }
        }
    }
    
    // (AR) دخول نطاق جديد لـ with / (EN) Enter new scope for with
    variableManager_.enterScope(Data::ScopeType::BLOCK);
    
    // (AR) تعريف المتغير المستعار إذا وُجد / (EN) Define alias variable if present
    if (!node.alias.empty()) {
        variableManager_.define(node.alias, contextValue);
    }
    
    // (AR) تنفيذ جسم كتلة with / (EN) Execute with block body
    bool exceptionOccurred = false;
    std::string exceptionMessage;
    
    try {
        node.body->accept(*this);
    } catch (const Interpreter::SadException& e) {
        exceptionOccurred = true;
        exceptionMessage = e.getMessage();
    } catch (...) {
        exceptionOccurred = true;
        exceptionMessage = "Unknown exception";
    }
    
    // (AR) استدعاء __خروج__() أو __exit__() إذا كان الكائن يدعمها
    // (EN) Call __خروج__() or __exit__() if the object supports it
    if (hasEnterExit && resource.isObject()) {
        auto obj = resource.toObject();
        if (obj) {
            std::string exitName = obj->hasMethod("__خروج__") ? "__خروج__" : "__exit__";
            auto* exitMethod = obj->getMethod(exitName);
            if (exitMethod && exitMethod->body) {
                try {
                    variableManager_.enterScope(Data::ScopeType::FUNCTION, exitName);
                    variableManager_.define("هذا", resource);
                    // (AR) تمرير معلومات الاستثناء إن وجد
                    // (EN) Pass exception info if any
                    variableManager_.define("__استثناء__", 
                        exceptionOccurred ? Data::Value(exceptionMessage) : Data::Value());
                    exitMethod->body->accept(*this);
                    // (AR) إذا أرجعت __خروج__ صحيح، نبتلع الاستثناء
                    // (EN) If __exit__ returns true, suppress the exception
                    if (returnValue_.isBoolean() && returnValue_.toBool()) {
                        exceptionOccurred = false;
                        exceptionMessage.clear();
                    }
                    variableManager_.exitScope();
                } catch (const std::exception& exitErr) {
                    // (AR) خطأ في __خروج__ نفسها — تسجيل تحذير ومتابعة
                    // (EN) Error in __exit__ itself — log warning and continue
                    std::cerr << "(AR) تحذير: خطأ في دالة __خروج__: " << exitErr.what()
                              << " / (EN) Warning: error in __exit__: " << exitErr.what() << std::endl;
                    variableManager_.exitScope();
                } catch (...) {
                    // (AR) خطأ غير معروف في __خروج__ — تسجيل تحذير
                    // (EN) Unknown error in __exit__ — log warning
                    std::cerr << "(AR) تحذير: خطأ غير معروف في دالة __خروج__ / "
                              << "(EN) Warning: unknown error in __exit__" << std::endl;
                    variableManager_.exitScope();
                }
            }
        }
    }
    
    // (AR) خروج من نطاق with / (EN) Exit with scope
    variableManager_.exitScope();
    
    // (AR) إعادة رمي الاستثناء إذا لم يُعالج / (EN) Re-throw exception if not handled
    if (exceptionOccurred && !exceptionMessage.empty()) {
        throw Interpreter::SadException(exceptionMessage, "WithError");
    }
}

// =========================================================================
// (AR) تنفيذ معالجة الاستثناءات / (EN) Exception Handling Visitors
// =========================================================================

void StatementExecutor::visitTryStmt(AST::TryStmt& node) {
    // (AR) حارس RAII لضمان تنفيذ كتلة finally دائماً — حتى عند إعادة رفع الاستثناء
    // (EN) RAII guard to guarantee finally block always executes — even on re-throw
    std::exception_ptr pendingException = nullptr;
    
    try {
        // (AR) تنفيذ كتلة المحاولة / (EN) Execute try block
        node.tryBlock->accept(*this);
    }
    catch (const Interpreter::SadException& e) {
        // (AR) البحث عن بند التقاط مناسب مع مطابقة نوع الاستثناء
        // (EN) Find matching catch clause with exception type matching
        bool caught = false;
        const std::string exType = e.getType();
        
        for (auto& catchClause : node.catchClauses) {
            // (AR) مطابقة نوع الاستثناء — UNKNOWN يطابق الكل (catch-all)
            // (EN) Match exception type — UNKNOWN matches everything (catch-all)
            bool typeMatches = false;
            
            if (catchClause.exceptionType == Data::DataType::UNKNOWN) {
                // (AR) catch-all: يلتقط أي استثناء
                typeMatches = true;
            } else if (catchClause.exceptionType == Data::DataType::ERROR) {
                // (AR) مطابقة نوع ERROR فقط — نقارن مع نوع الاستثناء الفعلي
                // (EN) Match ERROR type — compare against actual exception type
                typeMatches = (exType == "RuntimeError" || exType == "Error" ||
                               exType == "خطأ" || exType == "خطأ_تشغيل" || exType.empty());
            } else if (catchClause.exceptionType == Data::DataType::OBJECT) {
                // (AR) مطابقة نوع كائن مخصص — نقارن اسم النوع المخصص مع نوع الاستثناء
                // (EN) Match custom object type — compare custom type name with exception type
                if (!catchClause.exceptionTypeName.empty()) {
                    typeMatches = (catchClause.exceptionTypeName == exType);
                } else {
                    typeMatches = (!catchClause.exceptionVar.empty() &&
                                   (catchClause.exceptionVar == exType ||
                                    exType.empty()));
                }
            }
            
            if (!typeMatches) {
                continue;
            }
            
            variableManager_.enterScope(Data::ScopeType::BLOCK);
            
            if (!catchClause.exceptionVar.empty()) {
                variableManager_.define(catchClause.exceptionVar, 
                    Data::Value(e.getMessage()));
            }
            
            catchClause.body->accept(*this);
            variableManager_.exitScope();
            
            caught = true;
            break;
        }
        
        if (!caught) {
            pendingException = std::current_exception();
        }
    }
    catch (const ExecutionError& e) {
        bool caught = false;
        
        for (auto& catchClause : node.catchClauses) {
            // (AR) مطابقة نوع الاستثناء — UNKNOWN يلتقط الكل
            // (EN) Type matching — UNKNOWN catches all
            if (catchClause.exceptionType != Data::DataType::UNKNOWN &&
                catchClause.exceptionType != Data::DataType::ERROR) {
                continue;
            }
            
            variableManager_.enterScope(Data::ScopeType::BLOCK);
            
            if (!catchClause.exceptionVar.empty()) {
                variableManager_.define(catchClause.exceptionVar, 
                    Data::Value(std::string(e.what())));
            }
            
            catchClause.body->accept(*this);
            variableManager_.exitScope();
            
            caught = true;
            break;
        }
        
        if (!caught) {
            pendingException = std::current_exception();
        }
    }
    catch (const std::exception& e) {
        bool caught = false;
        
        for (auto& catchClause : node.catchClauses) {
            // (AR) مطابقة نوع الاستثناء — UNKNOWN يلتقط الكل
            // (EN) Type matching — UNKNOWN catches all
            if (catchClause.exceptionType != Data::DataType::UNKNOWN &&
                catchClause.exceptionType != Data::DataType::ERROR) {
                continue;
            }
            
            variableManager_.enterScope(Data::ScopeType::BLOCK);
            
            if (!catchClause.exceptionVar.empty()) {
                variableManager_.define(catchClause.exceptionVar, 
                    Data::Value(std::string(e.what())));
            }
            
            catchClause.body->accept(*this);
            variableManager_.exitScope();
            
            caught = true;
            break;
        }
        
        if (!caught) {
            pendingException = std::current_exception();
        }
    }
    catch (...) {
        bool caught = false;
        
        for (auto& catchClause : node.catchClauses) {
            // (AR) للاستثناءات غير المعروفة، فقط catch-all (UNKNOWN) يلتقطها
            // (EN) For unknown exceptions, only catch-all (UNKNOWN) matches
            if (catchClause.exceptionType != Data::DataType::UNKNOWN) {
                continue;
            }
            
            variableManager_.enterScope(Data::ScopeType::BLOCK);
            
            if (!catchClause.exceptionVar.empty()) {
                variableManager_.define(catchClause.exceptionVar, 
                    Data::Value("Unknown exception"));
            }
            
            catchClause.body->accept(*this);
            variableManager_.exitScope();
            
            caught = true;
            break;
        }
        
        if (!caught) {
            pendingException = std::current_exception();
        }
    }
    
    // (AR) تنفيذ كتلة finally دائماً — حتى لو لم يُلتقط الاستثناء
    // (EN) Always execute finally block — even if exception was not caught
    if (node.finallyBlock) {
        // (AR) حفظ حالة التحكم بالتدفق قبل finally
        // (EN) Save flow control state before finally
        auto savedFlowBeforeFinally = flowControl_;
        
        node.finallyBlock->accept(*this);
        
        // (AR) إذا غيّرت finally الحالة (مثلاً ارجع)، لها الأولوية على الاستثناء
        // (EN) If finally changed flow (e.g. return), it takes precedence over exception
        if (flowControl_ != savedFlowBeforeFinally && flowControl_ == FlowControl::RETURN) {
            pendingException = nullptr;  // (AR) إلغاء الاستثناء لصالح الإرجاع
        }
    }
    
    // (AR) إعادة رفع الاستثناء غير المُلتقط بعد تنفيذ finally
    // (EN) Re-throw uncaught exception after finally executed
    if (pendingException) {
        std::rethrow_exception(pendingException);
    }
}

void StatementExecutor::visitRaiseStmt(AST::RaiseStmt& node) {
    // (AR) تقييم تعبير الاستثناء / (EN) Evaluate exception expression
    Data::Value exceptionValue = evaluateExpression(*node.exception);
    
    // (AR) إذا كان الاستثناء كائناً (صنف مخصص) — نرمي مع اسم الصنف كنوع
    // (EN) If exception is an object (custom class) — throw with class name as type
    if (exceptionValue.isObject()) {
        auto obj = exceptionValue.toObject();
        std::string typeName = obj ? obj->getClassName() : "UnknownObject";
        // (AR) محاولة استخراج رسالة من حقل 'رسالة' أو 'message'
        // (EN) Try to extract message from 'رسالة' or 'message' field
        std::string message = typeName;
        if (obj) {
            if (obj->hasField("رسالة")) {
                auto* field = obj->getField("رسالة");
                if (field) message = field->toString();
            } else if (obj->hasField("message")) {
                auto* field = obj->getField("message");
                if (field) message = field->toString();
            } else if (obj->hasField("الرسالة")) {
                auto* field = obj->getField("الرسالة");
                if (field) message = field->toString();
            }
        }
        throw Interpreter::SadException(message, typeName, node.position);
    }
    
    // (AR) رفع الاستثناء كخطأ تشغيل عادي / (EN) Raise as regular runtime error
    throw Interpreter::RuntimeError(
        exceptionValue.toString(),
        node.position
    );
}

// =========================================================================
// (AR) Pattern Matching Implementation / (EN) تنفيذ مطابقة الأنماط
// =========================================================================

void StatementExecutor::visitMatchStmt(AST::MatchStmt& node) {
    // (AR) تقييم القيمة المراد مطابقتها / (EN) Evaluate value to match
    Data::Value testValue = evaluateExpression(*node.value);
    
    // (AR) المرور على كل حالة بالترتيب / (EN) Try each case in order
    for (auto& caseClause : node.cases) {
        // (AR) خريطة لربط المتغيرات من النمط / (EN) Map to bind variables from pattern
        std::map<std::string, Data::Value> bindings;
        
        // (AR) محاولة مطابقة النمط / (EN) Try to match pattern
        if (caseClause.pattern->matches(testValue, bindings)) {
            // (AR) النمط تطابق - الآن نتحقق من الحارس (guard) إن وجد
            // (EN) Pattern matched - now check guard if present
            if (caseClause.guard) {
                Data::Value guardResult = evaluateExpression(*caseClause.guard);
                if (!guardResult.toBool()) {
                    // (AR) الحارس فشل، جرّب الحالة التالية / (EN) Guard failed, try next case
                    continue;
                }
            }
            
            // (AR) النمط والحارس نجحا - ننفذ الجسم / (EN) Pattern and guard succeeded - execute body
            // (AR) ندفع نطاق جديد لربط متغيرات النمط / (EN) Push new scope to bind pattern variables
            variableManager_.enterScope(Data::ScopeType::BLOCK);
            
            // (AR) ربط جميع المتغيرات من النمط / (EN) Bind all variables from pattern
            for (const auto& [name, value] : bindings) {
                variableManager_.define(name, value);
            }
            
            // (AR) تنفيذ جسم الحالة / (EN) Execute case body
            for (auto& stmt : caseClause.body) {
                stmt->accept(*this);
                
                // (AR) إذا حدث تحكم في التدفق (return, break, continue)، نتوقف
                // (EN) If flow control occurred (return, break, continue), stop
                if (flowControl_ != FlowControl::NONE) {
                    variableManager_.exitScope();
                    return;
                }
            }
            
            // (AR) إزالة النطاق / (EN) Pop scope
            variableManager_.exitScope();
            
            // (AR) وجدنا تطابق، ننهي / (EN) Found match, exit
            return;
        }
    }
    
    // (AR) لم يتطابق أي نمط - بدون حالة افتراضية نتجاهل فقط
    // (EN) No pattern matched - without default case, silently do nothing
    // NOTE: If a default arm ("_" or افتراضي) is wanted, it should be defined as a
    // WildcardPattern in the match cases. No error thrown for exhaustiveness.
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
        bool hasDefault = (param.defaultValue != nullptr);
        std::string defaultValStr = "";
        
        // (AR) إذا كان هناك قيمة افتراضية، نحولها إلى string للتخزين
        // (EN) If there's a default value, convert it to string for storage
        if (hasDefault) {
            // (AR) القيمة الافتراضية موجودة في AST، نضع علامة فقط
            // (EN) Default value exists in AST, just mark it
            // The actual evaluation will happen in ExpressionEvaluator using the AST
            defaultValStr = "<from_ast>";  // Placeholder
        }
        
        params.push_back(Data::FunctionParameter(
            param.name,
            dataTypeToString(param.type),
            hasDefault,
            defaultValStr,
            param.typeName  // (AR) اسم الصنف إذا كان المعامل من نوع OBJECT / (EN) Class name if param is OBJECT type
        ));
    }
    
    // (AR) تسجيل الدالة في FunctionManager / (EN) Register function in FunctionManager
    // (AR) static_cast آمن: Statement يرث من ASTNode في نفس الـ namespace
    // (EN) Safe static_cast: Statement inherits from ASTNode in same namespace
    std::shared_ptr<AST::ASTNode> bodyNode(
        static_cast<AST::ASTNode*>(node.body.get()),
        [](AST::ASTNode*) {} // Empty deleter - AST owns the memory
    );
    
    // (AR) إنشاء shared_ptr للـ FunctionDecl لتمريره لـ FunctionManager
    // (EN) Create shared_ptr for FunctionDecl to pass to FunctionManager
    std::shared_ptr<AST::ASTNode> declNode(
        static_cast<AST::ASTNode*>(&node),
        [](AST::ASTNode*) {} // Empty deleter - AST owns the memory
    );
    
    // (AR) استخدام النسخة الموسعة من defineFunction التي تحفظ FunctionDecl
    // (EN) Use the extended version of defineFunction that saves FunctionDecl
    functionManager_.defineFunction(node.name, params, bodyNode, declNode);
    
    // (AR) إذا كانت دالة مولد، نحفظ علامة المولد
    // (EN) If generator function, save generator flag
    if (node.isGenerator) {
        auto func = functionManager_.getFunction(node.name, params.size());
        if (func) {
            func->setIsGenerator(true);
        }
    }
    
    // (AR) إذا كانت دالة غير متزامنة، نحفظ علامة async
    // (EN) If async function, save async flag
    if (node.is_async) {
        auto func = functionManager_.getFunction(node.name, params.size());
        if (func) {
            func->setIsAsync(true);
        }
    }
    
    // ═══════════════════════════════════════════════════════════════
    // (AR) معالجة المزخرفات (Decorators) - من الأسفل للأعلى
    // (EN) Process decorators - bottom to top (like Python)
    // (AR) المزخرف يستقبل اسم الدالة كنص ويُرجع اسم الدالة الجديدة (أو نفسها)
    // (EN) Decorator receives function name as string, returns new function name (or same)
    // @مزخرف1
    // @مزخرف2
    // دالة ف() ... نهاية
    // → ف = مزخرف1(مزخرف2(ف))
    // ═══════════════════════════════════════════════════════════════
    if (!node.decorators.empty()) {
        // (AR) القيمة الحالية هي اسم الدالة الأصلية
        // (EN) Current value is the original function name
        std::string currentFuncName = node.name;
        
        // (AR) المزخرفات تُطبّق بترتيب عكسي (الأخير أولاً)
        // (EN) Decorators apply in reverse order (last first)
        for (auto it = node.decorators.rbegin(); it != node.decorators.rend(); ++it) {
            auto* decoratorExpr = dynamic_cast<AST::DecoratorExpr*>(it->get());
            if (!decoratorExpr) continue;
            
            // (AR) البحث عن دالة المزخرف (تقبل وسيطاً واحداً أو أكثر حسب وسائط المزخرف)
            // (EN) Find decorator function (accepts 1+ args depending on decorator arguments)
            size_t expectedArgs = 1; // (AR) الوسيط الأساسي = اسم الدالة
            if (decoratorExpr->hasArguments) {
                expectedArgs += decoratorExpr->arguments.size();
            }
            
            auto decoratorFunc = functionManager_.getFunction(decoratorExpr->name, expectedArgs);
            // (AR) محاولة مع وسيط واحد إذا لم يوجد مع العدد الكامل
            if (!decoratorFunc) {
                decoratorFunc = functionManager_.getFunction(decoratorExpr->name, 1);
            }
            
            if (decoratorFunc) {
                // (AR) بناء قائمة الوسائط: اسم الدالة الحالية + وسائط المزخرف
                // (EN) Build argument list: current function name + decorator arguments
                
                // (AR) نستخدم آلية مشابهة لاستدعاء الدوال — ندخل نطاق ونعرّف المعاملات
                // (EN) Use mechanism similar to function calls — enter scope and define params
                variableManager_.enterScope(Data::ScopeType::FUNCTION, "decorator_" + decoratorExpr->name);
                
                const auto& params = decoratorFunc->getParameters();
                
                // (AR) الوسيط الأول = اسم الدالة المُزخرَفة
                // (EN) First argument = decorated function name
                if (params.size() >= 1) {
                    variableManager_.define(params[0].name, Data::Value(currentFuncName));
                }
                
                // (AR) بقية الوسائط = وسائط المزخرف (@مزخرف(وسيط1, وسيط2))
                // (EN) Remaining args = decorator arguments (@decorator(arg1, arg2))
                if (decoratorExpr->hasArguments && expressionEvaluator_) {
                    for (size_t i = 0; i < decoratorExpr->arguments.size() && (i + 1) < params.size(); ++i) {
                        decoratorExpr->arguments[i]->accept(*expressionEvaluator_);
                        variableManager_.define(params[i + 1].name, expressionEvaluator_->getResult());
                    }
                }
                
                // (AR) تنفيذ جسم المزخرف — ثم استخلاص النتيجة
                // (EN) Execute decorator body — then extract result
                auto bodyNode = decoratorFunc->getBody();
                auto bodyStmt = dynamic_cast<AST::Statement*>(bodyNode.get());
                
                Data::Value decoratorResult;
                try {
                    if (bodyStmt) {
                        // (AR) إذا كان الجسم BlockStmt — ننفّذ عباراته في نطاقنا مباشرة
                        // (EN) If body is BlockStmt — execute its statements in our scope directly
                        auto blockStmt = dynamic_cast<AST::BlockStmt*>(bodyStmt);
                        if (blockStmt) {
                            for (auto& stmt : blockStmt->statements) {
                                stmt->accept(*this);
                                if (shouldStopExecution()) break;
                            }
                        } else {
                            bodyStmt->accept(*this);
                        }
                        if (flowControl_ == FlowControl::RETURN) {
                            decoratorResult = returnValue_;
                            resetFlowControl();
                        }
                    } else {
                        auto bodyExpr = dynamic_cast<AST::Expression*>(bodyNode.get());
                        if (bodyExpr && expressionEvaluator_) {
                            bodyExpr->accept(*expressionEvaluator_);
                            decoratorResult = expressionEvaluator_->getResult();
                        }
                    }
                } catch (...) {
                    variableManager_.exitScope();
                    throw;
                }
                
                variableManager_.exitScope();
                
                // (AR) إذا أرجع المزخرف اسم دالة (string) — نستخدمه كاسم الدالة الجديد
                // (EN) If decorator returned a function name (string) — use it as new function name
                if (decoratorResult.isString()) {
                    std::string newFuncName = decoratorResult.toString();
                    // (AR) إذا أرجع اسماً مختلفاً عن الأصلي، نعيد ربط الاسم الأصلي
                    // (EN) If returned different name, rebind the original name
                    if (newFuncName != currentFuncName && !newFuncName.empty()) {
                        auto wrappedFunc = functionManager_.getFunction(newFuncName, 0);
                        if (!wrappedFunc) {
                            // (AR) محاولة البحث بأي عدد من المعاملات
                            auto overloads = functionManager_.getFunctionOverloads(newFuncName);
                            if (!overloads.empty()) {
                                wrappedFunc = overloads[0];
                            }
                        }
                        if (wrappedFunc) {
                            // (AR) إعادة تسجيل الدالة باسمها الأصلي ← تشير الآن إلى الملفوفة
                            // (EN) Re-register function with original name → now points to wrapped one
                            functionManager_.redefineFunction(
                                node.name,
                                wrappedFunc->getParameters(),
                                wrappedFunc->getBody()
                            );
                        }
                        currentFuncName = newFuncName;
                    }
                }
                // (AR) إذا لم يُرجع شيئاً أو أرجع نفس الاسم — لا تغيير
                // (EN) If returned nothing or same name — no change
            } else {
                // (AR) تحذير: المزخرف غير معرّف
                // (EN) Warning: decorator not defined
                std::cerr << "(AR) تحذير: المزخرف '" << decoratorExpr->name 
                          << "' غير معرّف / (EN) Warning: Decorator '" 
                          << decoratorExpr->name << "' is not defined." << std::endl;
            }
        }
    }
    
    // (AR) حفظ نوع الإرجاع مع اسم الدالة في map داخلي
    // (EN) Save return type with function name in internal map
    // Store the node pointer with the function definition for later access to returnType
    // We'll use this in executeFunctionBody to check return types
    functionReturnTypes_[node.name] = node.returnType;
}

Data::Value StatementExecutor::executeFunctionBody(AST::Statement& body) {
    // (AR) توجيه إلى الدالة الموسعة مع نوع إرجاع UNKNOWN واسم فارغ
    // (EN) Delegate to extended function with UNKNOWN return type and empty name
    return executeFunctionBodyWithReturnType(body, Data::DataType::UNKNOWN, "");
}

Data::Value StatementExecutor::executeFunctionBodyWithFuncName(AST::Statement& body, const std::string& functionName) {
    // (AR) البحث عن نوع الإرجاع في الـ map
    // (EN) Look up return type in map
    Data::DataType returnType = Data::DataType::UNKNOWN;
    auto it = functionReturnTypes_.find(functionName);
    if (it != functionReturnTypes_.end()) {
        returnType = it->second;
    }
    
    // (AR) استدعاء الدالة المحسّنة مع نوع الإرجاع
    // (EN) Call enhanced function with return type
    return executeFunctionBodyWithReturnType(body, returnType, functionName);
}

// (AR) دالة مساعدة جديدة لتنفيذ جسم دالة مع تتبع نوع الإرجاع
// (EN) New helper function to execute function body with return type tracking
Data::Value StatementExecutor::executeFunctionBodyWithReturnType(
    AST::Statement& body, 
    Data::DataType returnType, 
    const std::string& functionName) 
{
    // (AR) حفظ الحالة الحالية / (EN) Save current state
    FlowControl previousFlowControl = flowControl_;
    Data::Value previousReturnValue = returnValue_;
    Data::DataType previousReturnType = currentFunctionReturnType_;
    std::string previousFunctionName = currentFunctionName_;
    
    // (AR) تعيين سياق الدالة الحالية / (EN) Set current function context
    currentFunctionReturnType_ = returnType;
    currentFunctionName_ = functionName;
    
    // (AR) إعادة تعيين الحالة / (EN) Reset state
    flowControl_ = FlowControl::NONE;
    returnValue_ = Data::Value();
    
    // (AR) تنفيذ جسم الدالة / (EN) Execute function body
    // IMPORTANT: إذا كان الـ body هو BlockStmt، نُنفّذ statements مباشرةً
    // بدون pushScope إضافي لأن ScopeGuard في expression_evaluator
    // بالفعل أنشأ FUNCTION scope
    // (EN) If body is BlockStmt, execute statements directly without extra pushScope
    // because ScopeGuard in expression_evaluator already created FUNCTION scope
    auto blockStmt = dynamic_cast<AST::BlockStmt*>(&body);
    if (blockStmt) {
        // (AR) تنفيذ جُمل البلوك مباشرةً بدون scope إضافي
        // (EN) Execute block statements directly without extra scope
        for (auto& stmt : blockStmt->statements) {
            stmt->accept(*this);
            if (shouldStopExecution()) {
                break;
            }
        }
    } else {
        // (AR) ليس BlockStmt - نُنفّذه مباشرةً
        // (EN) Not BlockStmt - execute directly
        body.accept(*this);
    }
    
    // (AR) الحصول على قيمة الإرجاع / (EN) Get return value
    // Clone arrays/maps to avoid aliasing between function calls
    Data::Value result = returnValue_.isArray() || returnValue_.isMap() 
        ? returnValue_.clone() 
        : returnValue_;
    
    // (AR) استعادة الحالة السابقة / (EN) Restore previous state
    flowControl_ = previousFlowControl;
    returnValue_ = previousReturnValue;
    currentFunctionReturnType_ = previousReturnType;
    currentFunctionName_ = previousFunctionName;
    
    return result;
}

void StatementExecutor::visitNamespaceDecl(AST::NamespaceDecl& node) {
    // (AR) حفظ فضاء الأسماء السابق
    // (EN) Save previous namespace
    std::string previousNamespace = currentNamespace_;
    
    // (AR) تعيين فضاء الأسماء الحالي
    // (EN) Set current namespace
    if (currentNamespace_.empty()) {
        currentNamespace_ = node.name;
    } else {
        currentNamespace_ = currentNamespace_ + "::" + node.name;
    }
    
    // (AR) تنفيذ جميع التصريحات داخل فضاء الأسماء مع تسجيل بأسماء مؤهلة
    // (EN) Execute all declarations inside namespace, registering with qualified names
    // (AR) نحفظ أسماء المتغيرات والدوال المسجلة في هذا النطاق لتسجيلها بأسماء مؤهلة
    // (EN) We track variables/functions declared in this scope and also register with qualified names
    
    // (AR) ننفذ التصريحات في النطاق الحالي (بدون نطاق فرعي)
    // (EN) Execute declarations in current scope (no sub-scope) so they persist
    for (auto& stmt : node.members) {
        if (stmt) {
            stmt->accept(*this);
        }
        if (shouldStopExecution()) {
            break;
        }
    }
    
    // (AR) استعادة فضاء الأسماء السابق
    // (EN) Restore previous namespace
    currentNamespace_ = previousNamespace;
}

void StatementExecutor::visitOperatorDecl(AST::OperatorDecl& node) {
    // (AR) تسجيل حمل العامل الزائد — يُرمى خطأ إذا ظهر خارج صنف
    // (EN) Register operator overload — throws error if appears outside class context
    // (AR) ملاحظة: التسجيل الفعلي يتم في visitClassDecl عند تحليل أعضاء الصنف.
    // (EN) Note: Actual registration happens in visitClassDecl when parsing class members.
    throw Interpreter::RuntimeError(
        "(AR) لا يمكن تعريف عامل محمّل زائداً خارج صنف: '" + node.operatorSymbol + "' / "
        "(EN) Cannot define operator overload outside a class: '" + node.operatorSymbol + "'",
        node.position
    );
}

// =========================================================================
// (AR) تنفيذ القوالب (Templates) / (EN) Templates Implementation
// =========================================================================

/**
 * @brief (AR) يُنفذ تصريح دالة قالب
 *        (EN) Executes template function declaration
 * 
 * @details
 * يحفظ الدالة القالب في مدير القوالب للاستخدام لاحقاً عند الاستدعاء.
 * Stores the template function in template manager for later instantiation.
 */
void StatementExecutor::visitTemplateFunctionDecl(AST::TemplateFunctionDecl& node) {
    // (AR) حفظ دالة القالب في FunctionManager للاستدعاء لاحقاً
    // (EN) Store template function for later instantiation
    
    #ifdef DEBUG
    std::cout << "[Template] تسجيل دالة قالب: " << node.functionName << std::endl;
    std::cout << "[Template] معاملات الأنواع: ";
    for (const auto& typeParam : node.typeParameters) {
        std::cout << typeParam.name << " ";
    }
    std::cout << std::endl;
    #endif
    
    // (AR) Helper function لتحويل DataType إلى string
    // (EN) Helper to convert DataType to string
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
    
    // (AR) تحويل المعاملات إلى تنسيق FunctionManager
    // (EN) Convert parameters to FunctionManager format
    std::vector<Data::FunctionParameter> params;
    for (const auto& param : node.parameters) {
        bool hasDefault = (param.defaultValue != nullptr);
        std::string defaultValStr = "";
        
        if (hasDefault) {
            defaultValStr = "<from_ast>";
        }
        
        params.push_back(Data::FunctionParameter(
            param.name,
            dataTypeToString(param.type),
            hasDefault,
            defaultValStr,
            param.typeName  // (AR) اسم الصنف إذا كان المعامل من نوع OBJECT / (EN) Class name if OBJECT
        ));
    }
    
    // (AR) حفظ الدالة القالب - سنستخدم اسم خاص يحتوي على علامة قالب
    // (EN) Save template function - use special name with template marker
    // Format: __template_<name>_<type_params>
    std::string templateKey = "__template_" + node.name;
    
    std::shared_ptr<AST::ASTNode> bodyNode(
        static_cast<AST::ASTNode*>(node.body.get()),
        [](AST::ASTNode*) {}
    );
    
    std::shared_ptr<AST::ASTNode> declNode(
        static_cast<AST::ASTNode*>(&node),
        [](AST::ASTNode*) {}
    );
    
    functionManager_.defineFunction(templateKey, params, bodyNode, declNode);
    
    // (AR) حفظ نوع الإرجاع
    // (EN) Save return type
    functionReturnTypes_[templateKey] = node.returnType;
}

/**
 * @brief (AR) يُنفذ تصريح صنف قالب
 *        (EN) Executes template class declaration
 * 
 * @details
 * يحفظ الصنف القالب في مدير الأصناف للاستخدام لاحقاً عند الإنشاء.
 * Stores the template class in class manager for later instantiation.
 */
void StatementExecutor::visitTemplateClassDecl(AST::TemplateClassDecl& node) {
    // (AR) حفظ صنف القالب للإنشاء لاحقاً
    // (EN) Store template class for later instantiation
    
    #ifdef DEBUG
    std::cout << "[Template] تسجيل صنف قالب: " << node.name << std::endl;
    std::cout << "[Template] معاملات الأنواع: ";
    for (const auto& typeParam : node.typeParameters) {
        std::cout << typeParam.name << " ";
    }
    std::cout << std::endl;
    #endif
    
    // (AR) حفظ مؤشر للعقدة الأصلية في خريطة القوالب
    // (EN) Store pointer to original AST node in template map
    templateClasses_[node.name] = &node;
}

} // namespace Interpreter
} // namespace Sad
