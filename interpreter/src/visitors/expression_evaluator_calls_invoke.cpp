/**
 * @file expression_evaluator_calls_invoke.cpp
 * @brief (AR) تنفيذ استدعاءات الدوال والماكروز في ExpressionEvaluator
 * @brief (EN) Function and macro invocation handling in ExpressionEvaluator
 */

#include <string>
#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ast_node.h" // (AR) لضمان تعريف ASTNode الكامل عند استدعاء accept / (EN) Ensure full ASTNode definition for accept() calls
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"
#include "advanced_expr_nodes.h" // For AwaitExpr
#include "class_manager.h"
#include "utils/class_module_captures.h" // (AR) ع-1: حقن ثوابت وحدة التعريف
#include "object_instance.h"
#include "error_manager.h"
#include "ownership_manager.h"
#include "runtime_throw.h"
#include "user_thrown.h"
#include "runtime_throw.h"
#include "async_runtime.h"  // (AR) نظام التنفيذ غير المتزامن / (EN) Async runtime system
#include "tagged_enum_keys.h" // (AR) مفاتيح القيمة الموسومة (SoT) / (EN) tagged-value keys (SoT)
#include "suggestions.h"    // (AR) نظام الاقتراحات الذكية / (EN) Smart suggestion engine
#include "profiler_hooks.h" // (AR) خطافات مصحح الأداء / (EN) Profiler hooks
#include <atomic>
#include <cmath>
#include <climits>
#include <iostream>
#include "sad_type_system.h"
#include <map>

// (AR) دالة مساعدة للحصول على اقتراح وحدة الاستيراد — مُعرَّفة في builtin_registry.cpp
// (EN) Helper to get import module suggestion — defined in builtin_registry.cpp
namespace Sad
{
    namespace Interpreter
    {
        std::string suggestModuleForUnimportedFunction(const std::string &funcName);
    }
}
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>

#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;
        using namespace Lexer;

        // (AR) دالة مساعدة للحصول على اسم الملف من مدير الأخطاء
        // (EN) Helper function to get filename from error manager
        static inline std::string getSourceFilename()
        {
            const auto &fn = Sad::Errors::ErrorManager::getInstance().getSourceFilename();
            return fn.empty() ? "<input>" : fn;
        }

        // =========================================================================
        // (AR) تقييم التعابير الأحادية / (EN) Unary Expression Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitCallExpr(CallExpr &node)
        {
            // (AR) فحص عمق الاستدعاء لمنع الاستدعاء التكراري اللانهائي
            // (EN) Check call depth to prevent infinite recursion
            if (++currentCallDepth_ > maxCallDepth_)
            {
                --currentCallDepth_;
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_PANIC,
                    node.position,
                    {{"message", "call depth exceeded " + std::to_string(maxCallDepth_) + " (possible infinite recursion)"}});
            }

            // (AR) حارس RAII لتقليل العدّاد عند الخروج
            // (EN) RAII guard to decrement counter on exit
            struct CallDepthGuard
            {
                size_t &depth;
                CallDepthGuard(size_t &d) : depth(d) {}
                ~CallDepthGuard() { --depth; }
            } guard(currentCallDepth_);

            // ═══════════════════════════════════════════════════════════════
            // (AR) معالجة استدعاء الماكرو: اسم!(وسائط)
            //      الماكرو يُنفّذ في نطاق فرعي (hygiene) مع نسخ للخلف للمتغيرات
            //      يدعم: معاملات متغيرة، قيمة إرجاع، ماكروز متداخلة
            // (EN) Macro call handling: name!(args)
            //      Macro executes in child scope (hygiene) with copy-back for variables
            //      Supports: variadic params, return values, nested macros
            // ═══════════════════════════════════════════════════════════════
            if (node.isMacroCall)
            {
                handleMacroCallImpl(node);
                return;
            }

            // (AR) الحصول على اسم الدالة / (EN) Get function name
            std::string funcName;
            bool isTemplateInstantiation = false;

            // (AR) التحقق - هل callee هو TemplateInstantiation؟
            // (EN) Check - is callee a TemplateInstantiation?
            auto templateInst = dynamic_cast<TemplateInstantiation *>(node.callee.get());
            if (templateInst)
            {
                // (AR) هذا استدعاء دالة قالب!
                // (EN) This is a template function call!
                isTemplateInstantiation = true;
                funcName = "__template_" + templateInst->templateName;

                // ═════════════════════════════════════════════════════════════════
                // (AR) ربْط ثوابت القالب: نقرن typeParameters ذات isConst==true
                //      مع constArguments ثم نخزّنها في pendingTemplateConstBindings_
                //      ليحقنها executeUserFunctionBody كمتغيرات في نطاق الدالة.
                // (EN) Bind template const-params: pair isConst type-parameters with
                //      constArguments and stash into pendingTemplateConstBindings_
                //      for executeUserFunctionBody to inject into the function scope.
                // ═════════════════════════════════════════════════════════════════
                pendingTemplateConstBindings_.clear();
                auto *templateDecl = statementExecutor_.getTemplateFunction(templateInst->templateName);
                if (templateDecl && !templateInst->constArguments.empty())
                {
                    // (AR) اجمع أسماء المعاملات الثابتة بالترتيب
                    std::vector<std::string> constParamNames;
                    for (const auto &tp : templateDecl->typeParameters)
                    {
                        if (tp.isConst)
                            constParamNames.push_back(tp.name);
                    }

                    // (AR) قيّم كل وسيط ثابت (LiteralExpr عادةً) وربطه باسم المعامل
                    Value savedLastResult = lastResult_;
                    size_t numBindings = std::min(constParamNames.size(),
                                                  templateInst->constArguments.size());
                    for (size_t i = 0; i < numBindings; ++i)
                    {
                        if (templateInst->constArguments[i])
                        {
                            templateInst->constArguments[i]->accept(*this);
                            pendingTemplateConstBindings_.emplace_back(
                                constParamNames[i], lastResult_);
                        }
                    }
                    lastResult_ = savedLastResult;
                }

#ifdef DEBUG
                std::cout << "[Template] استدعاء دالة قالب: " << templateInst->templateName << "<";
                for (size_t i = 0; i < templateInst->typeArguments.size(); i++)
                {
                    if (i > 0)
                        std::cout << ", ";
                    std::cout << static_cast<int>(templateInst->typeArguments[i]);
                }
                std::cout << ">" << std::endl;
#endif
            }
            else
            {
                // (AR) التحقق - هل callee هو VariableExpr (اسم دالة) أم شيء آخر؟
                // (EN) Check - is callee a VariableExpr (function name) or something else?
                auto calleeVar = dynamic_cast<VariableExpr *>(node.callee.get());
                if (calleeVar)
                {
                    // (AR) نتحقق أولاً - هل المتغير موجود؟ / (EN) Check first - does variable exist?
                    if (variableManager_.exists(calleeVar->name))
                    {
                        // (AR) قد يكون lambda أو اسم دالة مخزّن في متغير
                        // (EN) May be lambda or function name stored in variable
                        Value varValue = variableManager_.get(calleeVar->name);

                        // (AR) أولاً: التحقق من نوع FUNCTION الجديد
                        // (EN) First: check for new FUNCTION type
                        if (varValue.isFunction())
                        {
                            funcName = varValue.getFunctionName();
                        }
                        else if (varValue.isString())
                        {
                            // (AR) توافق خلفي: STRING قد يحمل اسم lambda أو دالة
                            // (EN) Backward compat: STRING may hold lambda or function name
                            std::string strVal = varValue.toString();
                            if (strVal.find("__lambda_") == 0)
                            {
                                // (AR) هذا lambda! / (EN) This is lambda!
                                funcName = strVal;
                            }
                            else if (functionManager_.hasFunction(strVal))
                            {
                                // (AR) اسم دالة مخزّن في متغير (دالة من الدرجة الأولى)
                                // (EN) Function name stored in variable (first-class function)
                                funcName = strVal;
                            }
                            else
                            {
                                // (AR) دالة عادية / (EN) Regular function
                                funcName = calleeVar->name;
                            }
                        }
                        else if (varValue.isObject())
                        {
                            // ═══════════════════════════════════════════
                            // (AR) كائن — نبحث عن عامل () أو __call__
                            // (EN) Object — look for operator() or __call__
                            // ═══════════════════════════════════════════
                            auto objPtr = varValue.toObject();
                            if (objPtr)
                            {
                                std::string className = objPtr->getClassName();
                                auto *classManager = Data::ClassManager::getInstance();
                                auto *classType = classManager->getClass(className);
                                if (classType)
                                {
                                    // (AR) أولاً: فحص __call__
                                    auto *callMethod = classType->findMethod("__call__");
                                    if (callMethod && callMethod->getBody())
                                    {
                                        std::vector<Data::Value> callArgs;
                                        for (const auto &arg : node.arguments)
                                        {
                                            arg->accept(*this);
                                            callArgs.push_back(lastResult_);
                                        }
                                        variableManager_.enterScope(Data::ScopeType::FUNCTION, "__call__");
                                        // (AR) ع-1: ثوابت وحدة التعريف قبل «هذا» والحقول
                                        // (EN) ع-1: module constants before «هذا» and fields
                                        Utils::injectClassModuleCaptures(classType, variableManager_);
                                        variableManager_.define("هذا", varValue);
                                        for (const auto &[fn, fv] : objPtr->fields)
                                        {
                                            variableManager_.define(fn, fv);
                                        }
                                        for (size_t i = 0; i < callMethod->parameters.size() && i < callArgs.size(); ++i)
                                        {
                                            variableManager_.define(callMethod->parameters[i].name, callArgs[i]);
                                        }
                                        try
                                        {
                                            callMethod->getBody()->accept(statementExecutor_);
                                            if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                                            {
                                                lastResult_ = statementExecutor_.getReturnValue();
                                                statementExecutor_.resetFlowControl();
                                            }
                                            else
                                            {
                                                lastResult_ = Value();
                                            }
                                        }
                                        catch (...)
                                        {
                                            variableManager_.exitScope();
                                            throw;
                                        }
                                        variableManager_.exitScope();
                                        return;
                                    }
                                    // (AR) ثم: فحص عامل ()
                                    Data::OperatorOverload *callOp = classType->findOperator("()");
                                    if (callOp && callOp->body)
                                    {
                                        std::vector<Data::Value> callArgs;
                                        for (const auto &arg : node.arguments)
                                        {
                                            arg->accept(*this);
                                            callArgs.push_back(lastResult_);
                                        }
                                        variableManager_.enterScope(Data::ScopeType::FUNCTION, "operator()");
                                        // (AR) ع-1: ثوابت وحدة التعريف قبل «هذا» والحقول
                                        // (EN) ع-1: module constants before «هذا» and fields
                                        Utils::injectClassModuleCaptures(classType, variableManager_);
                                        variableManager_.define("هذا", varValue);
                                        for (const auto &[fn, fv] : objPtr->fields)
                                        {
                                            variableManager_.define(fn, fv);
                                        }
                                        for (size_t i = 0; i < callOp->parameters.size() && i < callArgs.size(); ++i)
                                        {
                                            variableManager_.define(callOp->parameters[i].name, callArgs[i]);
                                        }
                                        Value callResult;
                                        try
                                        {
                                            callOp->body->accept(statementExecutor_);
                                            if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                                            {
                                                callResult = statementExecutor_.getReturnValue();
                                                statementExecutor_.resetFlowControl();
                                            }
                                            for (auto &[fn, fv] : objPtr->fields)
                                            {
                                                if (variableManager_.exists(fn))
                                                {
                                                    objPtr->setField(fn, variableManager_.get(fn));
                                                }
                                            }
                                        }
                                        catch (...)
                                        {
                                            variableManager_.exitScope();
                                            throw;
                                        }
                                        variableManager_.exitScope();
                                        lastResult_ = callResult;
                                        return;
                                    }
                                }
                            }
                            // (AR) لا يوجد __call__ أو عامل () — نحاول كاسم دالة
                            funcName = calleeVar->name;
                        }
                        else if (varValue.isMap())
                        {
                            // ═══════════════════════════════════════════════════════
                            // (AR) متغير يحمل خريطة — قد يكون باني نموذج تعداد جبري
                            //      مثال: متغير باني = شكل.دائرة ثم باني(5)
                            // (EN) Variable holds a map — may be ADT variant constructor
                            //      Example: var ctor = Shape.Circle then ctor(5)
                            // ═══════════════════════════════════════════════════════
                            auto mapVal = varValue.toMap();
                            auto ctorIt = mapVal.find("__باني_نموذج__");
                            // (AR) دالّة *المستخدم* المُصرَّحة تتقدّم على باني المُعامِل العاري مهما كان
                            //      ترتيب التصريح (تفادي اختطاف اسم الدالّة — نظير حسم أ-م٢ الدلاليّ).
                            //      أمّا المُدمَج المتصادم فيُظلِّله الباني العاري (قرار المالك). لذا نفحص
                            //      hasUserFunction (تستثني المُدمَجات) لا hasFunction. إن وُجدت دالّة
                            //      مستخدم بنفس الاسم يسقط الحسم إلى مسار الدالّة (funcName أدناه).
                            // (EN) A declared *user* function outranks the bare variant builder regardless
                            //      of declaration order (avoid hijacking — mirrors the A-M2 semantic
                            //      resolution). A colliding builtin, however, is shadowed by the bare ctor
                            //      (owner decision). Hence hasUserFunction (excludes builtins), not
                            //      hasFunction. If a same-named user function exists, fall through to the
                            //      function path (funcName below).
                            if (ctorIt != mapVal.end() && ctorIt->second.isBoolean() && ctorIt->second.toBool() &&
                                !functionManager_.hasUserFunction(calleeVar->name))
                            {
                                // (AR) باني نموذج — نقيّم الوسائط وننشئ variant
                                // (EN) Variant constructor — evaluate args and create variant
                                std::string enumName = mapVal[AST::TaggedEnumKeys::ENUM].toString();
                                std::string memberName = mapVal[AST::TaggedEnumKeys::VARIANT].toString();
                                auto fieldNamesArr = mapVal["__حقول_أسماء__"].toArray();
                                int expectedCount = mapVal["__عدد_حقول__"].toInt();

                                std::vector<Data::Value> args;
                                for (const auto &arg : node.arguments)
                                {
                                    arg->accept(*this);
                                    args.push_back(lastResult_);
                                }

                                if (::Sad::Security::SafeArithmetic::assertSafeCast<int>(args.size(), "expression_evaluator_calls_invoke_size") != expectedCount)
                                {
                                    ::Sad::Errors::throwRuntime(
                                        ::Sad::Errors::ErrorCode::RUN_TOO_MANY_ARGS,
                                        node.position,
                                        {{"function", enumName + "." + memberName},
                                         {"expected", std::to_string(expectedCount)},
                                         {"actual", std::to_string(args.size())}});
                                }

                                Data::Value::MapType variantMap;
                                variantMap[AST::TaggedEnumKeys::ENUM] = Data::Value(enumName);
                                variantMap[AST::TaggedEnumKeys::VARIANT] = Data::Value(memberName);
                                variantMap["__جبري__"] = Data::Value(true);

                                Data::Value::ArrayType fieldsArray;
                                for (size_t i = 0; i < args.size() && i < fieldNamesArr.size(); ++i)
                                {
                                    fieldsArray.push_back(args[i]);
                                    variantMap[fieldNamesArr[i].toString()] = args[i];
                                }
                                variantMap[AST::TaggedEnumKeys::FIELDS] = Data::Value(fieldsArray);

                                lastResult_ = Data::Value(variantMap);
                                return;
                            }
                            // (AR) ليس باني نموذج — نبحث عن __callable__ أو __lambda__
                            auto callableIt = mapVal.find("__callable__");
                            if (callableIt != mapVal.end())
                            {
                                if (callableIt->second.isFunction())
                                {
                                    funcName = callableIt->second.getFunctionName();
                                }
                                else if (callableIt->second.isString())
                                {
                                    funcName = callableIt->second.toString();
                                }
                            }
                            else
                            {
                                auto lambdaIt = mapVal.find("__lambda__");
                                if (lambdaIt != mapVal.end())
                                {
                                    if (lambdaIt->second.isFunction())
                                    {
                                        funcName = lambdaIt->second.getFunctionName();
                                    }
                                    else if (lambdaIt->second.isString())
                                    {
                                        funcName = lambdaIt->second.toString();
                                    }
                                }
                                else
                                {
                                    funcName = calleeVar->name;
                                }
                            }
                        }
                        else
                        {
                            // (AR) دالة عادية / (EN) Regular function
                            funcName = calleeVar->name;
                        }
                    }
                    else
                    {
                        // (AR) دالة عادية غير مخزنة في متغير / (EN) Regular function not stored in variable
                        funcName = calleeVar->name;
                    }
                }
                else
                {
                    // ═══════════════════════════════════════════════════════════════
                    // (AR) استدعاء دالة معقد: callee ليس متغيراً بسيطاً
                    //      يدعم: استدعاء متسلسل getFunc()("arg")
                    //             فهرسة callbacks[0]("arg")
                    //             عضو فرعي obj.field الذي يحمل lambda
                    //             كائن قابل للاستدعاء مع __call__
                    //
                    // (EN) Complex function call: callee is not a simple variable
                    //      Supports: chained calls getFunc()("arg")
                    //                index access callbacks[0]("arg")
                    //                member access obj.field holding lambda
                    //                callable object with __call__
                    // ═══════════════════════════════════════════════════════════════
                    node.callee->accept(*this);
                    Value calleeValue = lastResult_;

                    if (calleeValue.isFunction())
                    {
                        // (AR) مرجع دالة من نوع FUNCTION — الطريقة الجديدة
                        // (EN) FUNCTION type reference — the new way
                        funcName = calleeValue.getFunctionName();
                    }
                    else if (calleeValue.isString())
                    {
                        // (AR) القيمة نص — قد يكون اسم lambda أو دالة (توافق خلفي)
                        // (EN) String value — may be lambda name or function name (backward compat)
                        funcName = calleeValue.toString();
                    }
                    else if (calleeValue.isMap())
                    {
                        // ═══════════════════════════════════════════════════════════
                        // (AR) خريطة — أولاً نتحقق من باني نموذج تعداد جبري (ADT)
                        //      ثم نبحث عن __callable__ أو __lambda__
                        // (EN) Map — first check for ADT variant constructor
                        //      then look for __callable__ or __lambda__ key
                        // ═══════════════════════════════════════════════════════════
                        auto mapVal = calleeValue.toMap();

                        // (AR) تحقق من باني نموذج تعداد جبري (ADT variant constructor)
                        // (EN) Check for ADT variant constructor
                        auto constructorIt = mapVal.find("__باني_نموذج__");
                        if (constructorIt != mapVal.end() && constructorIt->second.isBoolean() && constructorIt->second.toBool())
                        {
                            // ═══════════════════════════════════════════════════════
                            // (AR) هذا باني نموذج تعداد جبري!
                            //      مثال: شكل.دائرة(5) — يُنشئ خريطة variant
                            //      { __تعداد__: "شكل", __عضو__: "دائرة", نصف_القطر: 5 }
                            // (EN) This is an ADT variant constructor!
                            //      Example: Shape.Circle(5) — creates variant map
                            // ═══════════════════════════════════════════════════════
                            std::string enumName = mapVal[AST::TaggedEnumKeys::ENUM].toString();
                            std::string memberName = mapVal[AST::TaggedEnumKeys::VARIANT].toString();
                            auto fieldNamesArr = mapVal["__حقول_أسماء__"].toArray();
                            int expectedCount = mapVal["__عدد_حقول__"].toInt();

                            // (AR) تقييم الوسائط / (EN) Evaluate arguments
                            std::vector<Data::Value> args;
                            for (const auto &arg : node.arguments)
                            {
                                arg->accept(*this);
                                args.push_back(lastResult_);
                            }

                            // (AR) التحقق من عدد الوسائط
                            // (EN) Check argument count
                            if (::Sad::Security::SafeArithmetic::assertSafeCast<int>(args.size(), "expression_evaluator_calls_invoke_size") != expectedCount)
                            {
                                ::Sad::Errors::throwRuntime(
                                    ::Sad::Errors::ErrorCode::RUN_TOO_MANY_ARGS,
                                    node.position,
                                    {{"function", enumName + "." + memberName},
                                     {"expected", std::to_string(expectedCount)},
                                     {"actual", std::to_string(args.size())}});
                            }

                            // (AR) إنشاء خريطة الـ variant
                            // (EN) Create variant map
                            Data::Value::MapType variantMap;
                            variantMap[AST::TaggedEnumKeys::ENUM] = Data::Value(enumName);
                            variantMap[AST::TaggedEnumKeys::VARIANT] = Data::Value(memberName);
                            variantMap["__جبري__"] = Data::Value(true);

                            Data::Value::ArrayType fieldsArray;
                            for (size_t i = 0; i < args.size() && i < fieldNamesArr.size(); ++i)
                            {
                                fieldsArray.push_back(args[i]);
                                variantMap[fieldNamesArr[i].toString()] = args[i];
                            }
                            variantMap[AST::TaggedEnumKeys::FIELDS] = Data::Value(fieldsArray);

                            lastResult_ = Data::Value(variantMap);
                            return;
                        }

                        auto callableIt = mapVal.find("__callable__");
                        if (callableIt != mapVal.end())
                        {
                            // (AR) دعم FUNCTION أو STRING
                            // (EN) Support both FUNCTION and STRING
                            if (callableIt->second.isFunction())
                            {
                                funcName = callableIt->second.getFunctionName();
                            }
                            else if (callableIt->second.isString())
                            {
                                funcName = callableIt->second.toString();
                            }
                        }
                        else
                        {
                            auto lambdaIt = mapVal.find("__lambda__");
                            if (lambdaIt != mapVal.end())
                            {
                                if (lambdaIt->second.isFunction())
                                {
                                    funcName = lambdaIt->second.getFunctionName();
                                }
                                else if (lambdaIt->second.isString())
                                {
                                    funcName = lambdaIt->second.toString();
                                }
                            }
                            else
                            {
                                ::Sad::Errors::throwRuntime(
                                    ::Sad::Errors::ErrorCode::RUN_NOT_CALLABLE,
                                    node.position,
                                    {{"type", "map without __callable__"}});
                            }
                        }
                    }
                    else if (calleeValue.isObject())
                    {
                        // (AR) كائن — نبحث عن طريقة __call__
                        // (EN) Object — look for __call__ method
                        auto objPtr = calleeValue.toObject();
                        if (objPtr)
                        {
                            auto callIt = objPtr->fields.find("__call__");
                            if (callIt != objPtr->fields.end())
                            {
                                // (AR) دعم FUNCTION أو STRING
                                // (EN) Support both FUNCTION and STRING
                                if (callIt->second.isFunction())
                                {
                                    funcName = callIt->second.getFunctionName();
                                }
                                else if (callIt->second.isString())
                                {
                                    funcName = callIt->second.toString();
                                }
                            }
                            else
                            {
                                std::string className = objPtr->getClassName();
                                auto *classManager = Data::ClassManager::getInstance();
                                auto *classType = classManager->getClass(className);
                                if (classType)
                                {
                                    auto *callMethod = classType->findMethod("__call__");
                                    if (callMethod && callMethod->getBody())
                                    {
                                        // (AR) تقييم المعاملات / (EN) Evaluate arguments
                                        std::vector<Data::Value> callArgs;
                                        for (const auto &arg : node.arguments)
                                        {
                                            arg->accept(*this);
                                            callArgs.push_back(lastResult_);
                                        }
                                        // (AR) تنفيذ __call__ كطريقة الكائن
                                        // (EN) Execute __call__ as the object's method
                                        variableManager_.enterScope(Data::ScopeType::FUNCTION, "__call__");
                                        // (AR) ع-1: ثوابت وحدة التعريف قبل «هذا» والحقول
                                        // (EN) ع-1: module constants before «هذا» and fields
                                        Utils::injectClassModuleCaptures(classType, variableManager_);
                                        variableManager_.define("هذا", calleeValue);
                                        variableManager_.define("this", calleeValue);
                                        for (const auto &[fname, fval] : objPtr->fields)
                                        {
                                            variableManager_.define(fname, fval);
                                        }
                                        for (size_t i = 0; i < callMethod->parameters.size() && i < callArgs.size(); ++i)
                                        {
                                            variableManager_.define(callMethod->parameters[i].name, callArgs[i]);
                                        }
                                        try
                                        {
                                            // إصلاح: التحقق من وجود body قبل الاستدعاء
                                            if (!callMethod->getBody())
                                            {
                                                variableManager_.exitScope();
                                                ::Sad::Errors::throwRuntime(
                                                    ::Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND,
                                                    node.position,
                                                    {{"method", "__call__ (no body)"}, {"class", className}});
                                            }
                                            callMethod->getBody()->accept(statementExecutor_);
                                            if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                                            {
                                                lastResult_ = statementExecutor_.getReturnValue();
                                                statementExecutor_.resetFlowControl();
                                            }
                                            else
                                            {
                                                lastResult_ = Value();
                                            }
                                        }
                                        catch (...)
                                        {
                                            variableManager_.exitScope();
                                            throw;
                                        }
                                        variableManager_.exitScope();
                                        return; // (AR) تم التنفيذ بالكامل / (EN) Fully executed
                                    }

                                    // ═══════════════════════════════════════════
                                    // (AR) فحص عامل () المحمل زائداً
                                    // (EN) Check operator() overload
                                    // ═══════════════════════════════════════════
                                    Data::OperatorOverload *callOpOverload = classType->findOperator("()");
                                    if (callOpOverload && callOpOverload->body)
                                    {
                                        std::vector<Data::Value> callArgs;
                                        for (const auto &arg : node.arguments)
                                        {
                                            arg->accept(*this);
                                            callArgs.push_back(lastResult_);
                                        }

                                        variableManager_.enterScope(Data::ScopeType::FUNCTION, "operator()");
                                        // (AR) ع-1: ثوابت وحدة التعريف قبل «هذا» والحقول
                                        // (EN) ع-1: module constants before «هذا» and fields
                                        Utils::injectClassModuleCaptures(classType, variableManager_);
                                        variableManager_.define("هذا", calleeValue);
                                        for (const auto &[fname, fval] : objPtr->fields)
                                        {
                                            variableManager_.define(fname, fval);
                                        }
                                        // (AR) ربط المعاملات المعرّفة
                                        for (size_t i = 0; i < callOpOverload->parameters.size() && i < callArgs.size(); ++i)
                                        {
                                            variableManager_.define(callOpOverload->parameters[i].name, callArgs[i]);
                                        }

                                        Value callResult;
                                        try
                                        {
                                            callOpOverload->body->accept(statementExecutor_);
                                            if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                                            {
                                                callResult = statementExecutor_.getReturnValue();
                                                statementExecutor_.resetFlowControl();
                                            }
                                            // (AR) كتابة الحقول المحدثة
                                            for (auto &[fname, fval] : objPtr->fields)
                                            {
                                                if (variableManager_.exists(fname))
                                                {
                                                    objPtr->setField(fname, variableManager_.get(fname));
                                                }
                                            }
                                        }
                                        catch (...)
                                        {
                                            variableManager_.exitScope();
                                            throw;
                                        }
                                        variableManager_.exitScope();
                                        lastResult_ = callResult;
                                        return;
                                    }
                                }
                                ::Sad::Errors::throwRuntime(
                                    ::Sad::Errors::ErrorCode::RUN_NOT_CALLABLE,
                                    node.position,
                                    {{"type", "object of class '" + className + "' (no __call__)"}});
                            }
                        }
                        else
                        {
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_NULL_REFERENCE,
                                node.position,
                                {{"class", ""}, {"member", "__call__"}, {"reason", "كائن غير قابل للاستدعاء / not callable"}});
                        }
                    }
                    else
                    {
                        // (AR) نوع غير قابل للاستدعاء
                        // (EN) Non-callable type
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_NOT_CALLABLE,
                            node.position,
                            {{"type", calleeValue.getTypeName()}});
                    }
                }
            }

            // (AR) تقييم المعاملات / (EN) Evaluate arguments
            // (AR) يدعم الوسائط المسمّاة بصيغة Flutter: عمود(تباعد: 10، خلفية: "ابيض")
            // (EN) Supports Flutter-like named arguments: column(spacing: 10, bg: "white")
            std::vector<Data::Value> arguments;
            std::vector<std::pair<std::string, Data::Value>> namedArgs;
            for (const auto &arg : node.arguments)
            {
                // (AR) كشف الوسيط المسمّى / (EN) Detect named argument
                if (auto *named = dynamic_cast<AST::NamedArgExpr *>(arg.get()))
                {
                    named->value->accept(*this);
                    namedArgs.emplace_back(named->name, lastResult_);
                }
                else
                {
                    arg->accept(*this);
                    arguments.push_back(lastResult_);
                }
            }
            // (AR) إذا وُجدت وسائط مسماة: نحسب العدد الفعلي للوسائط
            //      (مكانية + مسمّاة) — ونحاول مطابقة الدالة بهذا العدد أولاً.
            //      إذا نجحت المطابقة: نُعيد ترتيب الوسائط حسب أسماء المعاملات.
            //      إذا فشلت: نُسطّح كأزواج اسم/قيمة (متوافق مع _autoChildren).
            // (EN) If named args exist: compute effective arg count
            //      (positional + named) — try matching function with this count first.
            //      If matched: reorder args by parameter names.
            //      If not: flatten as name/value pairs (compatible with _autoChildren).
            size_t effectiveArgCount = arguments.size() + namedArgs.size();

            // (AR) إلحاق الوسائط المسمّاة كأزواج اسم/قيمة (متوافق مع _autoChildren)
            // (EN) Append named args as name/value pairs (compatible with _autoChildren)
            for (size_t ni = 0; ni < namedArgs.size(); ++ni)
            {
                arguments.push_back(Data::Value(namedArgs[ni].first));
                arguments.push_back(namedArgs[ni].second);
            }

            // (AR) أولوية: إذا كنا داخل طريقة صنف، نبحث أولاً عن الطريقة في الصنف الحالي
            // (EN) Priority: if inside a class method, search class methods first
            if (handleClassMethodCall(node, funcName, arguments))
                return;

            // ═══════════════════════════════════════════════════════════════
            // (AR) التعامل مع استدعاء باني الأب: أساس(...) أو الأساس(...) أو super(...)
            // (EN) Handle super constructor call: أساس(...) or الأساس(...) or super(...)
            if (handleSuperConstructorCall(node, funcName, arguments))
                return;

            // (AR) البحث عن الدالة وتنفيذها — مُستخرج إلى ملف منفصل
            // (EN) Function lookup and dispatch — extracted to separate file
            lookupAndDispatchFunction(node, funcName, arguments, namedArgs, effectiveArgCount);
        }

    } // namespace Interpreter
} // namespace Sad
