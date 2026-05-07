/**
 * @file statement_executor_functions.cpp
 * @brief (AR) تنفيذ تصريحات الدوال والقوالب وفضاءات الأسماء في StatementExecutor
 * @brief (EN) Function/template/namespace statement execution for StatementExecutor
 *
 * (AR) ملف مُستخرج من statement_executor.cpp لتقليل حجم الملف وفق CW-05
 */


#include "statement_executor.h"
#include "declarations.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "error_manager.h"
#include "runtime_throw.h"
#include "user_thrown.h"
#include "object_instance.h"
#include "debug_server.h"
#include "class_manager.h"
#include "channel.h"
#include "sad_type_system.h"
#include "profiler_hooks.h" // (AR) خطافات مصحح الأداء / (EN) Profiler hooks
#include <iostream>
#include <sstream>
#include <map>

#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
namespace Sad
{
    namespace Interpreter
    {

        // (AR) دالة مساعدة للحصول على اسم الملف من مدير الأخطاء
        // (EN) Helper function to get filename from error manager
        static inline std::string getSourceFilename()
        {
            const auto &fn = Sad::Errors::ErrorManager::getInstance().getSourceFilename();
            return fn.empty() ? "<input>" : fn;
        }

        // ═══════════════════════════════════════════════════════════════
        // (AR) خطاف التصحيح — يُستدعى قبل تنفيذ كل جملة
        // (EN) Debug hook — called before each statement execution
        // ═══════════════════════════════════════════════════════════════
        static inline void debugHook(const Sad::Lexer::Position &pos)
        {
            auto *dbg = Sad::Debug::DebugServer::getInstance();
            if (dbg && dbg->isConnected())
            {
                dbg->onBeforeStatement(
                    getSourceFilename(),
                    static_cast<int>(pos.line),
                    static_cast<int>(pos.column));
                if (dbg->shouldStop())
                {
                    throw std::runtime_error("تم فصل المصحح / Debugger disconnected");
                }
            }
        }
        void StatementExecutor::visitFunctionDecl(AST::FunctionDecl &node)
        {
            // (AR) Helper function لتحويل DataType إلى string / (EN) Helper to convert DataType to string
            auto dataTypeToString = [](Data::DataType type) -> std::string
            {
                switch (type)
                {
                case Data::DataType::INTEGER:
                    return "integer";
                case Data::DataType::FLOAT:
                    return "float";
                case Data::DataType::STRING:
                    return "string";
                case Data::DataType::BOOLEAN:
                    return "boolean";
                case Data::DataType::NONE:
                    return "none";
                case Data::DataType::ARRAY:
                    return "array";
                case Data::DataType::MAP:
                    return "map";
                case Data::DataType::TUPLE:
                    return "tuple";
                case Data::DataType::FUNCTION:
                    return "function";
                case Data::DataType::OBJECT:
                    return "object";
                case Data::DataType::ENUM:
                    return "enum";
                case Data::DataType::BYTE:
                    return "byte";
                default:
                    return "unknown";
                }
            };

            // (AR) تحويل المعاملات إلى تنسيق FunctionManager / (EN) Convert parameters to FunctionManager format
            std::vector<Data::FunctionParameter> params;
            for (const auto &param : node.parameters)
            {
                bool hasDefault = (param.defaultValue != nullptr);
                std::string defaultValStr = "";

                // (AR) إذا كان هناك قيمة افتراضية، نحولها إلى string للتخزين
                // (EN) If there's a default value, convert it to string for storage
                if (hasDefault)
                {
                    // (AR) القيمة الافتراضية موجودة في AST، نضع علامة فقط
                    // (EN) Default value exists in AST, just mark it
                    // The actual evaluation will happen in ExpressionEvaluator using the AST
                    defaultValStr = "<from_ast>"; // Placeholder
                }

                params.push_back(Data::FunctionParameter(
                    param.name,
                    dataTypeToString(param.type),
                    hasDefault,
                    defaultValStr,
                    param.typeName // (AR) اسم الصنف إذا كان المعامل من نوع OBJECT / (EN) Class name if param is OBJECT type
                    ));
            }

            // (AR) تسجيل الدالة في FunctionManager / (EN) Register function in FunctionManager
            // (AR) static_cast آمن: Statement يرث من ASTNode في نفس الـ namespace
            // (EN) Safe static_cast: Statement inherits from ASTNode in same namespace

            // (AR) إذا كانت دالة خارجية (FFI) — نسجلها بدون جسم ونتجاوز
            // (EN) If extern function (FFI) — register without body and skip
            if (node.isExtern)
            {
                std::shared_ptr<AST::ASTNode> declNode(
                    static_cast<AST::ASTNode *>(&node),
                    [](AST::ASTNode *) {});
                functionManager_.defineFunction(node.name, params, nullptr, declNode);
                // (AR) تعيين علامة extern
                // (EN) Set extern flag
                auto func = functionManager_.getFunction(node.name, params.size());
                if (func)
                {
                    func->setIsExtern(true);
                }
                return;
            }

            std::shared_ptr<AST::ASTNode> bodyNode(
                static_cast<AST::ASTNode *>(node.body.get()),
                [](AST::ASTNode *) {} // Empty deleter - AST owns the memory
            );

            // (AR) إنشاء shared_ptr للـ FunctionDecl لتمريره لـ FunctionManager
            // (EN) Create shared_ptr for FunctionDecl to pass to FunctionManager
            std::shared_ptr<AST::ASTNode> declNode(
                static_cast<AST::ASTNode *>(&node),
                [](AST::ASTNode *) {} // Empty deleter - AST owns the memory
            );

            // (AR) استخدام النسخة الموسعة من defineFunction التي تحفظ FunctionDecl
            // (EN) Use the extended version of defineFunction that saves FunctionDecl

            // ═══════════════════════════════════════════════════════════════
            // (AR) إذا كنا داخل نطاق غير عام، ننشئ نسخة فريدة (إغلاق / Closure)
            // (EN) If inside a non-global scope, create a unique closure instance
            // (AR) كل إغلاق يحصل على اسم فريد حتى لا تتشارك الإغلاقات متغيراتها
            // (EN) Each closure gets a unique name so closures don't share variables
            // ═══════════════════════════════════════════════════════════════
            auto *currentScope = scopeManager_.getCurrentScope();
            bool isNestedFunction = (currentScope && !currentScope->isGlobal());
            std::string effectiveName = node.name;

            if (isNestedFunction)
            {
                // (AR) توليد اسم فريد لكل نسخة من الإغلاق
                // (EN) Generate unique name for each closure instance
                static int closureCounter = 0;
                effectiveName = node.name + "__closure_" + std::to_string(++closureCounter);
            }

            functionManager_.defineFunction(effectiveName, params, bodyNode, declNode);

            // (AR) إذا كانت دالة مولد، نحفظ علامة المولد
            // (EN) If generator function, save generator flag
            if (node.isGenerator)
            {
                auto func = functionManager_.getFunction(effectiveName, params.size());
                if (func)
                {
                    func->setIsGenerator(true);
                }
            }

            // (AR) إذا كانت دالة غير متزامنة، نحفظ علامة async
            // (EN) If async function, save async flag
            if (node.is_async)
            {
                auto func = functionManager_.getFunction(effectiveName, params.size());
                if (func)
                {
                    func->setIsAsync(true);
                }
            }

            // ═══════════════════════════════════════════════════════════════
            // (AR) التقاط المتغيرات للدوال المتداخلة (الإغلاقات / Closures)
            // (EN) Capture variables for nested functions (closures)
            // (AR) إذا كنا داخل نطاق غير عام، نلتقط المتغيرات من النطاق الخارجي
            // (EN) If inside a non-global scope, capture variables from enclosing scope
            // ═══════════════════════════════════════════════════════════════
            if (isNestedFunction)
            {
                auto func = functionManager_.getFunction(effectiveName, params.size());
                if (func)
                {
                    std::unordered_map<std::string, Data::Value> captures;
                    auto varNames = variableManager_.getVariableNames();
                    for (const auto &vname : varNames)
                    {
                        // (AR) لا نلتقط اسم الدالة نفسها ولا المعاملات
                        // (EN) Don't capture the function name itself or parameter names
                        bool isParam = false;
                        for (const auto &p : params)
                        {
                            if (p.name == vname)
                            {
                                isParam = true;
                                break;
                            }
                        }
                        if (!isParam && vname != node.name && vname != effectiveName)
                        {
                            try
                            {
                                captures[vname] = variableManager_.get(vname);
                            }
                            catch (const std::exception &)
                            {
                                // (AR) بعض المتغيرات قد لا تكون متاحة
                                // (EN) Some variables may not be accessible
                            }
                        }
                    }
                    if (!captures.empty())
                    {
                        func->setCaptures(captures);
                    }

                    // (AR) تعريف متغير محلي بالاسم الأصلي يحمل مرجع الإغلاق الفريد
                    // (EN) Define local variable with original name holding unique closure reference
                    auto funcRef = std::make_shared<Data::FunctionRef>(
                        node.name,                           // displayName (اسم العرض)
                        effectiveName,                       // registeredName (اسم فريد)
                        Data::FunctionRefKind::USER_DEFINED, // kind
                        ::Sad::Security::SafeArithmetic::assertSafeCast<int>(params.size(), "statement_executor_functions_size")      // arity
                    );
                    for (const auto &p : params)
                    {
                        funcRef->parameterNames.push_back(p.name);
                    }
                    variableManager_.define(node.name, Data::Value(std::move(funcRef)));
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
            if (!node.decorators.empty())
            {
                // (AR) القيمة الحالية هي اسم الدالة الأصلية
                // (EN) Current value is the original function name
                std::string currentFuncName = node.name;

                // (AR) المزخرفات تُطبّق بترتيب عكسي (الأخير أولاً)
                // (EN) Decorators apply in reverse order (last first)
                for (auto it = node.decorators.rbegin(); it != node.decorators.rend(); ++it)
                {
                    auto *decoratorExpr = dynamic_cast<AST::DecoratorExpr *>(it->get());
                    if (!decoratorExpr)
                        continue;

                    // (AR) البحث عن دالة المزخرف (تقبل وسيطاً واحداً أو أكثر حسب وسائط المزخرف)
                    // (EN) Find decorator function (accepts 1+ args depending on decorator arguments)
                    size_t expectedArgs = 1; // (AR) الوسيط الأساسي = اسم الدالة
                    if (decoratorExpr->hasArguments)
                    {
                        expectedArgs += decoratorExpr->arguments.size();
                    }

                    auto decoratorFunc = functionManager_.getFunction(decoratorExpr->name, expectedArgs);
                    // (AR) محاولة مع وسيط واحد إذا لم يوجد مع العدد الكامل
                    if (!decoratorFunc)
                    {
                        decoratorFunc = functionManager_.getFunction(decoratorExpr->name, 1);
                    }

                    if (decoratorFunc)
                    {
                        // (AR) بناء قائمة الوسائط: اسم الدالة الحالية + وسائط المزخرف
                        // (EN) Build argument list: current function name + decorator arguments

                        // (AR) نستخدم آلية مشابهة لاستدعاء الدوال — ندخل نطاق ونعرّف المعاملات
                        // (EN) Use mechanism similar to function calls — enter scope and define params
                        variableManager_.enterScope(Data::ScopeType::FUNCTION, "decorator_" + decoratorExpr->name);

                        const auto &params = decoratorFunc->getParameters();

                        // (AR) الوسيط الأول = اسم الدالة المُزخرَفة
                        // (EN) First argument = decorated function name
                        if (params.size() >= 1)
                        {
                            variableManager_.define(params[0].name, Data::Value(currentFuncName));
                        }

                        // (AR) بقية الوسائط = وسائط المزخرف (@مزخرف(وسيط1, وسيط2))
                        // (EN) Remaining args = decorator arguments (@decorator(arg1, arg2))
                        if (decoratorExpr->hasArguments && expressionEvaluator_)
                        {
                            for (size_t i = 0; i < decoratorExpr->arguments.size() && (i + 1) < params.size(); ++i)
                            {
                                decoratorExpr->arguments[i]->accept(*expressionEvaluator_);
                                variableManager_.define(params[i + 1].name, expressionEvaluator_->getResult());
                            }
                        }

                        // (AR) تنفيذ جسم المزخرف — ثم استخلاص النتيجة
                        // (EN) Execute decorator body — then extract result
                        auto bodyNode = decoratorFunc->getBody();
                        auto bodyStmt = dynamic_cast<AST::Statement *>(bodyNode.get());

                        Data::Value decoratorResult;
                        try
                        {
                            if (bodyStmt)
                            {
                                // (AR) إذا كان الجسم BlockStmt — ننفّذ عباراته في نطاقنا مباشرة
                                // (EN) If body is BlockStmt — execute its statements in our scope directly
                                auto blockStmt = dynamic_cast<AST::BlockStmt *>(bodyStmt);
                                if (blockStmt)
                                {
                                    for (auto &stmt : blockStmt->statements)
                                    {
                                        stmt->accept(*this);
                                        if (shouldStopExecution())
                                            break;
                                    }
                                }
                                else
                                {
                                    bodyStmt->accept(*this);
                                }
                                if (flowControl_ == FlowControl::RETURN)
                                {
                                    decoratorResult = returnValue_;
                                    resetFlowControl();
                                }
                            }
                            else
                            {
                                auto bodyExpr = dynamic_cast<AST::Expression *>(bodyNode.get());
                                if (bodyExpr && expressionEvaluator_)
                                {
                                    bodyExpr->accept(*expressionEvaluator_);
                                    decoratorResult = expressionEvaluator_->getResult();
                                }
                            }
                        }
                        catch (...)
                        {
                            variableManager_.exitScope();
                            throw;
                        }

                        variableManager_.exitScope();

                        // (AR) إذا أرجع المزخرف اسم دالة (string) — نستخدمه كاسم الدالة الجديد
                        // (EN) If decorator returned a function name (string) — use it as new function name
                        if (decoratorResult.isString())
                        {
                            std::string newFuncName = decoratorResult.toString();
                            // (AR) إذا أرجع اسماً مختلفاً عن الأصلي، نعيد ربط الاسم الأصلي
                            // (EN) If returned different name, rebind the original name
                            if (newFuncName != currentFuncName && !newFuncName.empty())
                            {
                                auto wrappedFunc = functionManager_.getFunction(newFuncName, 0);
                                if (!wrappedFunc)
                                {
                                    // (AR) محاولة البحث بأي عدد من المعاملات
                                    auto overloads = functionManager_.getFunctionOverloads(newFuncName);
                                    if (!overloads.empty())
                                    {
                                        wrappedFunc = overloads[0];
                                    }
                                }
                                if (wrappedFunc)
                                {
                                    // (AR) إعادة تسجيل الدالة باسمها الأصلي ← تشير الآن إلى الملفوفة
                                    // (EN) Re-register function with original name → now points to wrapped one
                                    functionManager_.redefineFunction(
                                        node.name,
                                        wrappedFunc->getParameters(),
                                        wrappedFunc->getBody());
                                }
                                currentFuncName = newFuncName;
                            }
                        }
                        // (AR) إذا لم يُرجع شيئاً أو أرجع نفس الاسم — لا تغيير
                        // (EN) If returned nothing or same name — no change
                    }
                    else
                    {
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

        Data::Value StatementExecutor::executeFunctionBody(AST::Statement &body)
        {
            // (AR) توجيه إلى الدالة الموسعة مع نوع إرجاع UNKNOWN واسم فارغ
            // (EN) Delegate to extended function with UNKNOWN return type and empty name
            return executeFunctionBodyWithReturnType(body, Data::DataType::UNKNOWN, "");
        }

        Data::Value StatementExecutor::executeFunctionBodyWithFuncName(AST::Statement &body, const std::string &functionName)
        {
            // (AR) البحث عن نوع الإرجاع في الـ map
            // (EN) Look up return type in map
            Data::DataType returnType = Data::DataType::UNKNOWN;
            auto it = functionReturnTypes_.find(functionName);
            if (it != functionReturnTypes_.end())
            {
                returnType = it->second;
            }

            // (AR) استدعاء الدالة المحسّنة مع نوع الإرجاع
            // (EN) Call enhanced function with return type
            return executeFunctionBodyWithReturnType(body, returnType, functionName);
        }

        // (AR) دالة مساعدة جديدة لتنفيذ جسم دالة مع تتبع نوع الإرجاع
        // (EN) New helper function to execute function body with return type tracking
        Data::Value StatementExecutor::executeFunctionBodyWithReturnType(
            AST::Statement &body,
            Data::DataType returnType,
            const std::string &functionName)
        {
            // (AR) حفظ الحالة الحالية / (EN) Save current state
            FlowControl previousFlowControl = flowControl_;
            Data::Value previousReturnValue = returnValue_;
            Data::DataType previousReturnType = currentFunctionReturnType_;
            std::string previousFunctionName = currentFunctionName_;
            Types::SadTypePtr previousSadReturnType = currentFunctionSadReturnType_;

            // (AR) تعيين سياق الدالة الحالية / (EN) Set current function context
            currentFunctionReturnType_ = returnType;
            currentFunctionName_ = functionName;

            // (AR) إنشاء مكدس تأجيل جديد لهذه الدالة
            // (EN) Create a new defer stack for this function
            deferStacks_.push_back({});

            // (AR) إخطار خادم التصحيح بدخول دالة / (EN) Notify debug server of function entry
            {
                auto *dbg = Sad::Debug::DebugServer::getInstance();
                if (dbg && dbg->isConnected())
                {
                    dbg->onFunctionEnter(functionName, getSourceFilename(), static_cast<int>(body.position.line));
                }
            }

            // (AR) إعادة تعيين الحالة / (EN) Reset state
            flowControl_ = FlowControl::NONE;
            returnValue_ = Data::Value();

            // (AR) تنفيذ جسم الدالة / (EN) Execute function body
            // IMPORTANT: إذا كان الـ body هو BlockStmt، نُنفّذ statements مباشرةً
            // بدون pushScope إضافي لأن ScopeGuard في expression_evaluator
            // بالفعل أنشأ FUNCTION scope
            // (EN) If body is BlockStmt, execute statements directly without extra pushScope
            // because ScopeGuard in expression_evaluator already created FUNCTION scope

            bool exceptionDuringBody = false;
            std::exception_ptr bodyException = nullptr;

            try
            {
                auto blockStmt = dynamic_cast<AST::BlockStmt *>(&body);
                if (blockStmt)
                {
                    // (AR) تنفيذ جُمل البلوك مباشرةً بدون scope إضافي
                    // (EN) Execute block statements directly without extra scope
                    for (auto &stmt : blockStmt->statements)
                    {
                        stmt->accept(*this);
                        if (shouldStopExecution())
                        {
                            break;
                        }
                    }
                }
                else
                {
                    // (AR) ليس BlockStmt - نُنفّذه مباشرةً
                    // (EN) Not BlockStmt - execute directly
                    body.accept(*this);
                }
            }
            catch (...)
            {
                // (AR) حدث استثناء أثناء تنفيذ الجسم — نُسجّله لكن نُنفّذ deferred أولاً
                // (EN) Exception during body execution — save it, but execute deferred stmts first
                exceptionDuringBody = true;
                bodyException = std::current_exception();
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) تنفيذ الجمل المؤجلة بترتيب LIFO (الأخير أولاً)
            //      تُنفَّذ دائماً: سواء خرجت الدالة طبيعياً أو بـ return أو باستثناء
            // (EN) Execute deferred statements in LIFO order (last-in, first-out)
            //      Always executes: whether function exits normally, via return, or exception
            // ═══════════════════════════════════════════════════════════════════
            if (!deferStacks_.empty())
            {
                auto &currentDeferStack = deferStacks_.back();

                // (AR) حفظ حالة التحكم بالتدفق أثناء تنفيذ الجمل المؤجلة
                //      لا نريد أن يؤثر return داخل defer على الدالة الأصلية
                // (EN) Save flow control state during deferred execution
                //      We don't want return inside defer to affect the original function
                FlowControl savedFlowControl = flowControl_;
                Data::Value savedReturnValue = returnValue_;

                // (AR) تنفيذ بترتيب عكسي (LIFO)
                // (EN) Execute in reverse order (LIFO)
                for (auto it = currentDeferStack.rbegin(); it != currentDeferStack.rend(); ++it)
                {
                    // (AR) إعادة تعيين التحكم بالتدفق لكل جملة مؤجلة
                    // (EN) Reset flow control for each deferred statement
                    flowControl_ = FlowControl::NONE;

                    try
                    {
                        (*it)->accept(*this);
                    }
                    catch (const std::exception &e)
                    {
                        // (AR) خطأ في جملة مؤجلة — نتجاهله ونستمر مع البقية
                        //      مثل Go: panic في defer لا يمنع تنفيذ باقي defers
                        // (EN) Error in deferred statement — ignore and continue with rest
                        //      Like Go: panic in defer doesn't prevent other defers
                        std::cerr << "[defer] خطأ في جملة مؤجلة / Error in deferred statement: "
                                  << e.what() << std::endl;
                    }
                    catch (...)
                    {
                        std::cerr << "[defer] خطأ غير معروف في جملة مؤجلة / Unknown error in deferred statement"
                                  << std::endl;
                    }
                }

                // (AR) استعادة حالة التحكم بالتدفق وقيمة الإرجاع
                // (EN) Restore flow control state and return value
                flowControl_ = savedFlowControl;
                returnValue_ = savedReturnValue;

                // (AR) حذف مكدس التأجيل لهذه الدالة
                // (EN) Remove the defer stack for this function
                deferStacks_.pop_back();
            }

            // (AR) إذا حدث استثناء أثناء الجسم، أعد رميه بعد تنفيذ deferred
            // (EN) If an exception occurred during body, re-throw after executing deferred
            if (exceptionDuringBody && bodyException)
            {
                // (AR) استعادة الحالة السابقة قبل إعادة الرمي
                // (EN) Restore previous state before re-throwing
                flowControl_ = previousFlowControl;
                returnValue_ = previousReturnValue;
                currentFunctionReturnType_ = previousReturnType;
                currentFunctionName_ = previousFunctionName;
                currentFunctionSadReturnType_ = previousSadReturnType;
                std::rethrow_exception(bodyException);
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
            currentFunctionSadReturnType_ = previousSadReturnType;

            // (AR) إخطار خادم التصحيح بخروج دالة / (EN) Notify debug server of function exit
            {
                auto *dbg = Sad::Debug::DebugServer::getInstance();
                if (dbg && dbg->isConnected())
                {
                    dbg->onFunctionExit();
                }
            }

            return result;
        }


    } // namespace Interpreter
} // namespace Sad

