/**
 * @file expression_evaluator_calls_dispatch.cpp
 * @brief (AR) توزيع استدعاءات الدوال: بحث طريقة الصنف، باني الأب، بحث الدوال والتنفيذ
 * @brief (EN) Function call dispatch: class method lookup, super constructor, function resolution and execution
 *
 * (AR) هذا الملف يحتوي على 3 دوال مساعدة مُستخرجة من visitCallExpr:
 *      1. handleClassMethodCall — بحث أولوية طريقة الصنف الحالي
 *      2. handleSuperConstructorCall — استدعاء باني الأب (أساس/الأساس/super)
 *      3. lookupAndDispatchFunction — بحث الدالة (overloads) وتنفيذها (native/extern/user)
 *
 * (EN) This file contains 3 helper functions extracted from visitCallExpr:
 *      1. handleClassMethodCall — current class method priority lookup
 *      2. handleSuperConstructorCall — super constructor call (أساس/الأساس/super)
 *      3. lookupAndDispatchFunction — function lookup (overloads) and dispatch (native/extern/user)
 */

#include <string>
#include "expression_evaluator.h"
#include "statement_executor.h"
#include "ast_node.h"
#include "statements.h"
#include "declarations.h"
#include "class_nodes.h"
#include "expressions.h"
#include "class_manager.h"
#include "object_instance.h"
#include "error_manager.h"
#include "exception.h"
#include "suggestions.h"
#include "profiler_hooks.h"
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <functional>

// (AR) دالة مساعدة للحصول على اقتراح وحدة الاستيراد — مُعرَّفة في builtin_registry.cpp
// (EN) Helper to get import module suggestion — defined in builtin_registry.cpp
namespace Sad
{
    namespace Interpreter
    {
        std::string suggestModuleForUnimportedFunction(const std::string &funcName);
    }
}

namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;

        // (AR) دالة مساعدة محلية للحصول على اسم الملف
        // (EN) Local helper to get source filename
        static inline std::string getDispatchSourceFilename()
        {
            const auto &fn = Sad::Errors::ErrorManager::getInstance().getSourceFilename();
            return fn.empty() ? "<input>" : fn;
        }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) 1. بحث طريقة الصنف الحالي وتنفيذها بأولوية
        //      إذا كنا داخل طريقة صنف، نبحث أولاً عن الطريقة في الصنف الحالي
        //      قبل البحث في الدوال العامة (لتجنب تعارض الأسماء مع الدوال المضمنة)
        //
        // (EN) 1. Search current class method and execute with priority
        //      If inside a class method, search class methods first
        //      before global functions (to avoid name conflicts with built-ins)
        // ═══════════════════════════════════════════════════════════════════════
        bool ExpressionEvaluator::handleClassMethodCall(
            CallExpr &node,
            const std::string &funcName,
            std::vector<Value> &arguments)
        {
            auto *classManager = Data::ClassManager::getInstance();
            if (!variableManager_.exists("هذا"))
                return false;

            Value thisValue = variableManager_.get("هذا");
            // (AR) الحصول على اسم الصنف — يدعم OBJECT الحقيقي و MAP القديم
            // (EN) Get class name — supports real OBJECT and legacy MAP
            std::string thisClassName;
            Data::ClassType *thisClassType = nullptr;

            if (thisValue.isObject())
            {
                // (AR) نوع OBJECT الحقيقي
                // (EN) Real OBJECT type
                auto objPtr = thisValue.toObject();
                if (objPtr)
                {
                    thisClassName = objPtr->getClassName();
                    thisClassType = classManager->getClass(thisClassName);
                }
            }
            else if (thisValue.isMap())
            {
                // (AR) MAP القديم مع __class__
                // (EN) Legacy MAP with __class__
                auto thisFields = thisValue.toMap();
                auto classNameIt = thisFields.find("__class__");
                if (classNameIt != thisFields.end())
                {
                    thisClassName = classNameIt->second.toString();
                    thisClassType = classManager->getClass(thisClassName);
                }
            }

            if (!thisClassType)
                return false;

            Data::ClassMethod *method = thisClassType->findMethod(funcName);
            // (AR) البحث عن الجسم: أولاً في الطريقة نفسها، ثم في مصدر القالب
            // (EN) Find body: first in method itself, then in template instance source
            AST::Statement *methodBody = nullptr;
            if (method && method->getBody())
            {
                methodBody = method->getBody();
            }
            else if (method)
            {
                auto *tplSrc = statementExecutor_.getTemplateInstanceSource(thisClassName);
                if (tplSrc)
                {
                    for (auto &m : tplSrc->members)
                    {
                        if (auto *md = dynamic_cast<AST::MethodDecl *>(m.get()))
                        {
                            if (md->name == funcName && md->body)
                            {
                                methodBody = md->body.get();
                                break;
                            }
                        }
                    }
                }
            }
            if (!method || !methodBody)
                return false;

            // (AR) وجدنا طريقة في الصنف الحالي - ننفذها بأولوية
            // (EN) Found method in current class - execute with priority
            if (arguments.size() != method->parameters.size())
            {
                std::string errMsg = "(AR) عدد معاملات الطريقة '" + funcName + "' غير متطابق. ";
                errMsg += "توقع " + std::to_string(method->parameters.size()) + " لكن حصل على " + std::to_string(arguments.size()) + ". ";
                errMsg += "(EN) Argument count mismatch for method '" + funcName + "'.";
                throw RuntimeError(errMsg, node.position);
            }

            // (AR) ندفع نطاق فقط للمعاملات - الحقول موروثة من النطاق الأب
            // (EN) Push scope only for parameters - fields inherited from parent scope
            variableManager_.enterScope(Data::ScopeType::FUNCTION, funcName);

            for (size_t i = 0; i < method->parameters.size(); ++i)
            {
                variableManager_.define(method->parameters[i].name, arguments[i]);
            }

            Value returnValue;
            try
            {
                methodBody->accept(statementExecutor_);
                if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                {
                    returnValue = statementExecutor_.getReturnValue();
                    statementExecutor_.resetFlowControl();
                }
            }
            catch (...)
            {
                variableManager_.exitScope();
                throw;
            }

            variableManager_.exitScope();
            lastResult_ = returnValue;
            return true;
        }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) 2. معالجة استدعاء باني الأب: أساس(...) أو الأساس(...) أو super(...)
        //      يبحث عن الصنف المنفذ حالياً ويتتبع سلسلة الوراثة
        //      يدعم الوراثة متعددة المستويات (A→B→C)
        //
        // (EN) 2. Handle super constructor call: أساس(...) or الأساس(...) or super(...)
        //      Finds currently executing class and traces inheritance chain
        //      Supports multi-level inheritance (A→B→C)
        // ═══════════════════════════════════════════════════════════════════════
        bool ExpressionEvaluator::handleSuperConstructorCall(
            CallExpr &node,
            const std::string &funcName,
            std::vector<Value> &arguments)
        {
            // (AR) التحقق من أن الاسم هو أحد أشكال super
            // (EN) Check if name is one of the super forms
            if (funcName != "أساس" && funcName != "الأساس" && funcName != "الاساس" &&
                funcName != "اساس" && funcName != "super")
            {
                return false;
            }

            // (AR) نبحث عن هذا في النطاق لتحديد الصنف الحالي
            // (EN) Look for this in scope to determine current class
            if (!variableManager_.exists("هذا"))
            {
                lastResult_ = Value();
                return true;
            }

            Value thisValue = variableManager_.get("هذا");
            auto *classManager = Data::ClassManager::getInstance();

            std::string currentClassName;
            Data::ObjectInstance *objPtr = nullptr;

            if (thisValue.isObject())
            {
                auto objShared = thisValue.toObject();
                objPtr = objShared.get();
                if (objPtr)
                    currentClassName = objPtr->getClassName();
            }
            else if (thisValue.isMap())
            {
                auto fields = thisValue.toMap();
                auto it = fields.find("__class__");
                if (it != fields.end())
                    currentClassName = it->second.toString();
            }

            // ═══════════════════════════════════════════════════════════
            // (AR) إصلاح الوراثة متعددة المستويات: نتتبع أي صنف ننفذ بانيه حالياً
            // (EN) Fix multi-level inheritance: track which class constructor is executing
            // (AR) مثال: قط_فارسي.باني يستدعي أساس() → يجب أن ينادي باني قط (وليس قط_فارسي مرة أخرى)
            //       قط.باني يستدعي أساس() → يجب أن ينادي باني حيوان (وليس قط مرة أخرى)
            // ═══════════════════════════════════════════════════════════
            std::string executingClassName = currentClassName;
            if (variableManager_.exists("__executing_constructor_class__"))
            {
                executingClassName = variableManager_.get("__executing_constructor_class__").toString();
            }

            if (!executingClassName.empty())
            {
                Data::ClassType *executingClass = classManager->getClass(executingClassName);
                if (executingClass)
                {
                    Data::ClassType *baseClass = executingClass->getBaseClass();
                    if (baseClass && baseClass->constructor)
                    {
                        AST::ConstructorDecl *baseCtor = baseClass->constructor.get();
                        // (AR) ربط معاملات باني الأب
                        // (EN) Bind base constructor parameters
                        if (arguments.size() == baseCtor->parameters.size())
                        {
                            for (size_t i = 0; i < baseCtor->parameters.size(); ++i)
                            {
                                const auto &pname = baseCtor->parameters[i].name;
                                // (AR) تحسين أداء: بحث واحد / (EN) Performance: single lookup
                                variableManager_.defineOrAssign(pname, arguments[i]);
                            }
                            // (AR) تعيين الصنف المنفذ حالياً لدعم السلسلة (A→B→C)
                            // (EN) Set executing class for chained super calls (A→B→C)
                            std::string baseClassName = baseClass->name;
                            bool hadPrevious = variableManager_.exists("__executing_constructor_class__");
                            Value previousVal;
                            if (hadPrevious)
                                previousVal = variableManager_.get("__executing_constructor_class__");

                            variableManager_.defineOrAssign("__executing_constructor_class__", Value(baseClassName));

                            try
                            {
                                baseCtor->body->accept(statementExecutor_);
                            }
                            catch (const std::exception &e)
                            {
                                // (AR) خطأ في تنفيذ باني الصنف الأساسي — إعادة رفعه
                                // (EN) Error executing base class constructor — re-throw
                                throw;
                            }
                            catch (...)
                            {
                                throw Interpreter::SadException(
                                    "(AR) خطأ غير معروف في باني الصنف الأساسي '" + baseClassName + "'. "
                                                                                                   "(EN) Unknown error in base class constructor '" +
                                        baseClassName + "'.",
                                    "RuntimeError", node.position);
                            }

                            // (AR) استعادة القيمة السابقة
                            // (EN) Restore previous value
                            if (hadPrevious)
                            {
                                variableManager_.assign("__executing_constructor_class__", previousVal);
                            }

                            // (AR) قراءة القيم المحدثة من باني الأب وتحديث الكائن
                            // (EN) Read updated values from base constructor and update object
                            if (objPtr)
                            {
                                // (AR) نجمع كل الحقول من سلسلة الوراثة
                                // (EN) Collect all fields from inheritance chain
                                std::function<void(Data::ClassType *)> collectFields;
                                collectFields = [&](Data::ClassType *cls)
                                {
                                    if (!cls)
                                        return;
                                    if (cls->getBaseClass())
                                        collectFields(cls->getBaseClass());
                                    for (const auto &field : cls->fields)
                                    {
                                        if (!field.isStatic)
                                        {
                                            if (variableManager_.exists(field.name))
                                            {
                                                Value val = variableManager_.get(field.name);
                                                objPtr->setField(field.name, val);
                                            }
                                        }
                                    }
                                };
                                collectFields(baseClass);
                            }
                        }
                    }
                }
            }

            // (AR) لم نجد باني أب أو تم تنفيذه — نرجع بقيمة فارغة
            // (EN) No base constructor found or already executed — return void
            lastResult_ = Value();
            return true;
        }

        // ═══════════════════════════════════════════════════════════════════════
        // (AR) 3. البحث عن الدالة وتنفيذها
        //      يبحث عن الدالة بالترتيب: overloads → أصناف → مضمنة → exact match
        //      ثم ينفذ: native → extern → user function body
        //
        // (EN) 3. Function lookup and dispatch
        //      Searches: overloads → classes → builtins → exact match
        //      Then executes: native → extern → user function body
        // ═══════════════════════════════════════════════════════════════════════
        void ExpressionEvaluator::lookupAndDispatchFunction(
            CallExpr &node,
            const std::string &funcName,
            std::vector<Value> &arguments,
            std::vector<std::pair<std::string, Value>> &namedArgs,
            size_t effectiveArgCount)
        {
            // (AR) البحث عن الدالة - flexible matching مع default parameters
            // (EN) Find function - flexible matching with default parameters
            auto allOverloads = functionManager_.getFunctionOverloads(funcName);
            std::shared_ptr<Data::FunctionDefinition> func = nullptr;

            // (AR) الأولوية 0: إذا وُجدت وسائط مسماة — نبحث بالعدد الفعلي أولاً
            // (EN) Priority 0: If named args exist — search by effective count first
            if (!namedArgs.empty())
            {
                for (const auto &candidate : allOverloads)
                {
                    if (!candidate->hasNativeImplementation() && candidate->acceptsArgumentCount(effectiveArgCount))
                    {
                        // (AR) نجحت المطابقة! نُعيد بناء الوسائط حسب أسماء المعاملات
                        // (EN) Match found! Rebuild arguments by parameter names
                        std::vector<Data::Value> reorderedArgs;
                        size_t positionalCount = effectiveArgCount - namedArgs.size();

                        // (AR) بناء خريطة الوسائط المسماة
                        std::unordered_map<std::string, Data::Value> namedMap;
                        for (auto &[name, val] : namedArgs)
                        {
                            namedMap[name] = val;
                        }

                        // (AR) ترتيب: مكانية أولاً، ثم مسماة حسب ترتيب المعاملات
                        for (size_t pi = 0; pi < candidate->getParameters().size(); ++pi)
                        {
                            const std::string &paramName = candidate->getParameters()[pi].name;
                            if (pi < positionalCount)
                            {
                                // (AR) معامل مكاني
                                reorderedArgs.push_back(arguments[pi]);
                            }
                            else if (namedMap.count(paramName))
                            {
                                // (AR) معامل مسمى — نطابق بالاسم
                                reorderedArgs.push_back(namedMap[paramName]);
                            }
                            else
                            {
                                // (AR) قيمة افتراضية أو فارغة — نترك الدالة تتعامل معها
                                // (EN) Default or empty — let function executor handle it
                                break;
                            }
                        }

                        arguments = std::move(reorderedArgs);
                        func = candidate;
                        break;
                    }
                }
            }

            // (AR) الأولوية 1: الدوال المعرفة من المستخدم التي تطابق عدد المعاملات
            // (EN) Priority 1: User-defined functions that match argument count
            if (!func)
            {
                for (const auto &candidate : allOverloads)
                {
                    if (!candidate->hasNativeImplementation() && candidate->acceptsArgumentCount(arguments.size()))
                    {
                        func = candidate;
                        break;
                    }
                }
            }
            // (AR) الأولوية 2: التحقق إذا كان الاسم صنف مسجّل → إنشاء كائن بدون 'جديد'
            // (EN) Priority 2: Check if name is a registered class → create object without 'new'
            if (!func)
            {
                auto *classManager2 = Data::ClassManager::getInstance();
                Data::ClassType *classType = classManager2->getClass(funcName);
                if (classType)
                {
                    NewExpr tempNewExpr(funcName);
                    tempNewExpr.position = node.position;
                    for (auto &arg : node.arguments)
                    {
                        tempNewExpr.arguments.push_back(std::move(arg));
                    }
                    visitNewExpr(tempNewExpr);
                    node.arguments.clear();
                    for (auto &arg : tempNewExpr.arguments)
                    {
                        node.arguments.push_back(std::move(arg));
                    }
                    return;
                }
            }
            // (AR) الأولوية 3: الدوال المضمنة (كنسخة احتياطية)
            // (EN) Priority 3: Built-in functions (as fallback)
            if (!func)
            {
                for (const auto &candidate : allOverloads)
                {
                    if (candidate->hasNativeImplementation())
                    {
                        func = candidate;
                        break;
                    }
                }
            }

            // (AR) إذا لم نجد دالة مناسبة
            // (EN) If no suitable function found
            if (!func)
            {
                // (AR) محاولة أخيرة - exact match (للدوال بدون defaults)
                // (EN) Last attempt - exact match (for functions without defaults)
                func = functionManager_.getFunction(funcName, arguments.size());

                if (!func)
                {
                    // (AR) دالة غير معرّفة — مع اقتراح "هل قصدت؟"
                    // (EN) Undefined function — with "Did you mean?" suggestion
                    std::string msgAr = "الدالة '" + funcName + "' غير معرفة بعدد معاملات " + std::to_string(arguments.size());
                    std::string msgEn = "Function '" + funcName + "' not defined with " + std::to_string(arguments.size()) + " parameters";

                    // (AR) بحث عن أسماء دوال مشابهة / (EN) Search for similar function names
                    auto availableFuncs = functionManager_.getFunctionNames();
                    Sad::Errors::SuggestionEngine sugEngine;
                    auto similar = sugEngine.findSimilarSymbols(funcName, availableFuncs);
                    if (!similar.empty())
                    {
                        msgAr += " — هل قصدت: '" + similar[0] + "'؟";
                        msgEn += " — Did you mean: '" + similar[0] + "'?";
                    }

                    // (AR) اقتراح الوحدة المناسبة للاستيراد / (EN) Suggest appropriate module to import
                    auto suggestedModule = suggestModuleForUnimportedFunction(funcName);
                    if (!suggestedModule.empty())
                    {
                        msgAr += "\n    💡 جرّب: استورد " + suggestedModule;
                        msgEn += "\n    💡 Try: import " + suggestedModule;
                    }

                    Sad::Errors::ErrorManager::getInstance().reportError(
                        Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
                        Sad::Errors::SourceLocation(getDispatchSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                        msgAr,
                        msgEn);
                    lastResult_ = Value();
                    return;
                }
            }

            // (AR) التحقق من وجود تنفيذ أصلي (دالة مضمنة) / (EN) Check for native implementation (built-in function)
            if (func->hasNativeImplementation())
            {
                // ═══════════════════════════════════════════════════════
                // (AR) خطاف مصحح الأداء — دالة مدمجة
                // (EN) Profiler hook — built-in function
                // ═══════════════════════════════════════════════════════
                Sad::Tools::ProfileGuard _profGuard(
                    funcName, Sad::Tools::getGlobalProfiler(), true, "",
                    node.position.line);

                // ═══════════════════════════════════════════════════════════
                // (AR) تحسين أداء: حجز الذاكرة مسبقاً للمؤشرات
                // (EN) Performance: pre-reserve memory for argument pointers
                // ═══════════════════════════════════════════════════════════
                std::vector<std::shared_ptr<Data::Value>> valuePtrs;
                valuePtrs.reserve(arguments.size());
                for (const auto &arg : arguments)
                {
                    valuePtrs.push_back(std::make_shared<Data::Value>(arg));
                }

                // (AR) استدعاء التنفيذ الأصلي / (EN) Call native implementation
                auto resultPtr = func->callNative(valuePtrs);

                if (resultPtr)
                {
                    lastResult_ = *resultPtr;
                }
                else
                {
                    lastResult_ = Data::Value(); // void return
                }

                return;
            }

            // (AR) إذا كانت دالة خارجية (FFI) — نُرجع قيمة فارغة
            // (EN) If extern function (FFI) — return empty value
            if (func->isExtern())
            {
                // (AR) الدوال الخارجية مُعدّة للمترجم وليس المفسر — نُرجع 0
                // (EN) Extern functions are for compiler, not interpreter — return 0
                lastResult_ = Value(static_cast<int64_t>(0));
                return;
            }

            // (AR) التحقق من وجود جسم للدالة / (EN) Check if function has body
            if (!func->hasBody())
            {
                Sad::Errors::ErrorManager::getInstance().reportError(
                    Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
                    Sad::Errors::SourceLocation(getDispatchSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)),
                    "الدالة '" + funcName + "' ليس لها جسم",
                    "Function '" + funcName + "' has no body");
                lastResult_ = Value();
                return;
            }

            // (AR) تنفيذ جسم دالة المستخدم — مُستخرج إلى ملف منفصل
            // (EN) Execute user function body — extracted to separate file
            executeUserFunctionBody(node, funcName, func, arguments);
        }

    } // namespace Interpreter
} // namespace Sad
