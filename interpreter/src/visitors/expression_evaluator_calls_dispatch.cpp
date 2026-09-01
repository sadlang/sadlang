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
#include "runtime_throw.h"
#include "user_thrown.h"
#include "suggestions.h"
#include "profiler_hooks.h"
// (AR) بوّابةُ الاستيراد المولَّدة (IMPORT_GATE) + سجلُّ الوحدات المحمَّلة
// (EN) The generated import gate + the loaded-module registry
#include "builtin_registry.h"
#include "builtin_module_registry.h"
// (AR) جدولُ رتبِ المدمَجاتِ المولَّدُ من مصدرِ الحقيقة — الجدولُ نفسُه الذي
//      يقرؤه المترجّم، مسؤولًا عنه بالاسمِ العربيِّ لا بالمعرّف.
// (EN) The SoT-generated builtin arity table — the same table the compiler
//      reads, queried by Arabic name rather than by C++ identifier.
#include "builtin_arity.h"
#include "utils/class_module_captures.h"
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
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_TOO_MANY_ARGS,
                    node.position,
                    {{"function", funcName},
                     {"expected", std::to_string(method->parameters.size())},
                     {"actual", std::to_string(arguments.size())}});
            }

            // (AR) ندفع نطاق فقط للمعاملات - الحقول موروثة من النطاق الأب
            // (EN) Push scope only for parameters - fields inherited from parent scope
            variableManager_.enterScope(Data::ScopeType::FUNCTION, funcName);

            // (AR) ع-1: حقن التقاطات وحدة التعريف هنا أيضا — كان هذا الموضع
            //      يعتمد على رؤية نطاق الأب (طريقة محقونة) عبر سلسلة الآباء،
            //      وهو اعتماد هش رصدته المراجعة؛ الحقن الصريح قبل ربط
            //      المعاملات يوحده مع بقية مواضع تنفيذ أجسام الطرق.
            // (EN) ع-1: inject the defining module's captures here too — this
            //      site used to lean on parent-scope visibility (an injected
            //      caller method), a fragile chain; explicit injection before
            //      parameter binding unifies it with every other method-body
            //      execution site.
            Utils::injectClassModuleCaptures(thisClassType, variableManager_);

            for (size_t i = 0; i < method->parameters.size(); ++i)
            {
                variableManager_.define(method->parameters[i].name, arguments[i]);
            }

            Value returnValue;
            try
            {
                // (AR) حدُّ الاستدعاء: حلقاتُ المستدعِي لا تُرى داخلَ جسمِ الطريقة
                // (EN) Call boundary: the caller's loops are invisible inside the method body
                StatementExecutor::CallBoundaryScope callBoundary(statementExecutor_);

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

            // (AR) نبحث عن هذا في النطاق لتحديد الصنف الحالي.
            //      خارجَ الطرائق (لا «هذا») الاسمُ ليس نداءَ باني الأبِ بل معرِّفٌ
            //      عاديٌّ — صنفُ مستخدمٍ باسم «أساس» مثلًا — فنُفسِح للتوزيعِ العاديّ.
            //      الابتلاعُ القديمُ (فراغٌ + true) جعل «أساس()» أعلى الملفِّ يُنشئ
            //      فراغًا صامتًا بينما المصرِّفُ يحصرُ الادّعاءَ بداخلِ صنفٍ — مقيس.
            // (EN) Look for «هذا» in scope to determine current class. Outside
            //      methods (no «هذا») the name is NOT a super call but an ordinary
            //      identifier — e.g. a user class literally named «أساس» — so fall
            //      through to normal dispatch. The old swallow (void + true) made a
            //      top-level «أساس()» silently yield void while the compiler claims
            //      the call only inside a class — measured divergence.
            if (!variableManager_.exists("هذا"))
            {
                return false;
            }

            Value thisValue = variableManager_.get("هذا");
            auto *classManager = Data::ClassManager::getInstance();

            std::string currentClassName;
            Data::ObjectInstance *objPtr = nullptr;

            if (thisValue.isObject())
            {
                objPtr = thisValue.toObject();
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
                                // (AR) مؤهَّلٌ كاملًا: `builtin_registry.h` يصرّح `class Interpreter`
                                //      داخل `Sad::Interpreter`، فيحجب الاسمُ المصرَّحُ اسمَ الفضاء.
                                // (EN) Fully qualified: builtin_registry.h declares class Interpreter
                                //      inside Sad::Interpreter, shadowing the namespace name.
                                throw Sad::Interpreter::UserThrownException(
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

            // ═══════════════════════════════════════════════════════════════
            // (AR) بوّابةُ الاستيراد — نقطةُ الاختناقِ الوحيدة في المفسّر.
            //
            //      المفسّرُ يسجّل جزءًا من المكتبةِ القياسيّةِ **عند الإقلاع**
            //      (StandardLibraryManager) وجزءًا **عند الاستيراد**، فما سُجّل
            //      إقلاعًا كان يعمل بلا `استورد` مهما قال مصدرُ الحقيقة. والمصرِّفُ
            //      يحجب بجدولٍ آخر. فبرنامجٌ واحدٌ يسلك مسلكين.
            //
            //      الحجبُ هنا — لا عند التسجيل — عمدًا: التسجيلُ موزَّعٌ على عشراتِ
            //      الملفّات، والنداءُ نقطةٌ واحدة. فيبقى التسجيلُ كما هو ويُرفَض
            //      النداءُ إن لم تُستورَد الوحدةُ التي يقول SoT إنّها تملكه.
            //
            //      ودالّةُ المستخدمِ لا تمرّ من هنا: الحجبُ مشروطٌ بـ
            //      hasNativeImplementation، فمن عرّف `مربع` بنفسه لم يُحجَب.
            // (EN) The import gate — the interpreter's single choke point.
            //      Part of the stdlib registers at STARTUP and part on import, so
            //      whatever registered at startup worked without «استورد» no matter
            //      what the SoT said, while the compiler gated by a different table.
            //      Gating at the CALL (not at registration) is deliberate:
            //      registration is spread over dozens of files; the call is one spot.
            //      A user-defined function never reaches this: the gate requires
            //      hasNativeImplementation, so redefining «مربع» yourself is unaffected.
            // ═══════════════════════════════════════════════════════════════
            //      ونداءُ الصياغةِ المخفَّضةِ (`|س|` ⇒ `مطلق(س)`) يتخطّى البوّابة:
            //      صياغةُ اللغةِ الأساسيّةُ لا تُحجَب خلف وحدة.
            // (EN) A syntax-desugared call (`|x|` ⇒ `abs(x)`) skips the gate: core
            //      language syntax is never hidden behind a module.
            if (func && func->hasNativeImplementation() && !node.isSyntaxDesugared)
            {
                // (AR) الاسمُ قد يُحجَب في أكثرَ من وحدة (`أرسل` في «شبكة» و«تزامن_متقدم»)
                //      فيكفي استيرادُ إحداها؛ واختيارُ واحدةٍ اعتباطًا كان يمنع الأخرى.
                // (EN) A name may be gated by several modules (`أرسل` in both network and
                //      async); importing any one suffices. Picking one arbitrarily blocked
                //      the other legitimate use.
                const std::string requiredModule =
                    std::string(Sad::Builtins::importGateUnsatisfiedModuleName(
                        std::string_view(funcName),
                        [](std::string_view moduleName) {
                            return BuiltinModuleRegistry::getInstance().isModuleLoaded(
                                std::string(moduleName));
                        }));
                if (!requiredModule.empty())
                {
                    const std::string msgAr =
                        "الدالة '" + funcName + "' تنتمي إلى وحدة '" + requiredModule +
                        "' ولم تُستورَد\n    💡 جرّب: استورد " + requiredModule;
                    const std::string msgEn =
                        "Function '" + funcName + "' belongs to module '" + requiredModule +
                        "' which was not imported\n    💡 Try: import " + requiredModule;
                    Sad::Errors::ErrorManager::getInstance().reportError(
                        Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
                        Sad::Errors::SourceLocation(getDispatchSourceFilename(),
                                                    static_cast<int>(node.position.line),
                                                    static_cast<int>(node.position.column)),
                        msgAr, msgEn);
                    lastResult_ = Value();
                    return;
                }
            }

            // ═══════════════════════════════════════════════════════════════
            // (AR) بوّابةُ الرتبة — الموضعُ نفسُه وللسببِ نفسِه.
            //
            //      🔑 حقلُ `arity` في مصدرِ الحقيقةِ كان يبلغُ **محرّكًا واحدًا**:
            //      يستهلكُ المترجّمُ الجدولَ المولَّدَ في واحدٍ وعشرين ملفًّا
            //      (`checkBuiltinArity`)، والمفسّرُ في **صفر** — لأنّ
            //      `registerBuiltinFunction(name, func)` لا يأخذُ رتبةً أصلًا،
            //      فيُسجَّلُ المدمَجُ بقائمةِ معاملاتٍ **فارغة**. والأولويّةُ ٣
            //      أعلاه تُطابقُ بـ`hasNativeImplementation()` وحدَها، فينفَّذُ
            //      المدمَجُ بأيِّ عددِ وسائط. فكان العقدُ الواحدُ يُفرَضُ في
            //      محرّكٍ ويُهمَلُ في الآخر: برنامجٌ واحدٌ يسلك مسلكَين.
            //
            //      والحكمُ **عند النداءِ لا عند التسجيل** كبوّابةِ الاستيراد:
            //      التسجيلُ موزَّعٌ على تسعِ مئةٍ وتسعةٍ وخمسين موضعًا، والنداءُ
            //      نقطةٌ واحدة. ولا تمرُّ دالّةُ المستخدمِ من هنا للشرطِ نفسِه.
            //
            //      والنصُّ من كتالوجِ الأخطاءِ (SEM005) لا مكتوبًا بيد، فيكونُ
            //      التشخيصُ حرفًا بحرفٍ كتشخيصِ المترجّم — وهذا هو التكافؤ.
            // (EN) The arity gate — same choke point, same rationale as the
            //      import gate. SoT `arity` reached only the compiler (21 files
            //      vs 0) because registerBuiltinFunction takes no arity, so a
            //      builtin ran with any argument count. Judged at the call, not
            //      at registration; message from the catalog, so both engines
            //      emit the identical SEM005 diagnostic.
            // ═══════════════════════════════════════════════════════════════
            if (func && func->hasNativeImplementation() && !node.isSyntaxDesugared)
            {
                // (AR) nullptr لِما لا عقدَ له: مدمَجٌ بلا حقلِ `arity` في مصدرِ
                //      الحقيقة، أو اسمٌ أُعلِن برتبتَين متضاربتَين. لا تُخترَعُ له
                //      رتبةٌ — السكوتُ عن غيرِ المقيسِ أصدقُ من فرضِ ما لم يُعلَن.
                const Sad::Builtins::Arity::Range *range =
                    Sad::Builtins::Arity::ByName::lookup(funcName);
                // (AR) 🔑 العددُ المقيسُ هو `effectiveArgCount` لا `arguments.size()`:
                //      الوسيطُ المسمّى يُسطَّحُ إلى **قيمتَين** (اسمٌ ثمّ قيمة) في
                //      `arguments`، فقياسُ الطولِ الخامِّ يُضاعِفُ العدّ. وقِيس:
                //      `جذر(العدد: 9)` كان يُرفَضُ بـ«مُرِّر 2» **والمترجّمُ
                //      يقبلُه** — تباعدٌ جديدٌ أحدثَه حارسٌ أُدخِل للتكافؤ.
                //      والطولُ الخامُّ يصلحُ للنداءِ المكانيِّ وحدَه، وهو ما جعل
                //      البوّابةَ خضراءَ في ٩٥٦ اختبارًا: الصيغةُ خارجَ المجموعة.
                const size_t argCount = effectiveArgCount;
                if (range && (argCount < range->min || argCount > range->max))
                {
                    // (AR) المتوقَّعُ المعروضُ هو الطرفُ الذي خولِف — الأدنى عند
                    //      النقصِ والأقصى عند الزيادة — نظيرَ `checkBuiltinArity`
                    //      في المترجّم حرفًا بحرف. فالرسالةُ تدلُّ على الإصلاح.
                    Sad::Errors::RenderContext ctx;
                    ctx.placeholders = {
                        {"name", funcName},
                        {"expected", std::to_string(argCount < range->min
                                                        ? range->min
                                                        : range->max)},
                        {"found", std::to_string(argCount)}};
                    Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                        Sad::Errors::ErrorCode::SEM_WRONG_ARG_COUNT,
                        Sad::Errors::SourceLocation(getDispatchSourceFilename(),
                                                    static_cast<int>(node.position.line),
                                                    static_cast<int>(node.position.column)),
                        ctx);
                    lastResult_ = Value();
                    return;
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
                    //
                    // (AR) الاسمُ المعروضُ اسمُ المستخدمِ لا اسمُ الإغلاقِ المشتقّ:
                    //      تُسجَّل `دالة س(...)` داخليًّا `س__closure_N`، وكان هذا
                    //      اللاحِقُ يظهر في وجه المستخدم فيحيله على رمزٍ لم يكتبه.
                    // (EN) Show the user's name, not the derived closure name:
                    //      `دالة س(...)` registers as `س__closure_N` internally, and
                    //      that suffix leaked into the message, naming a symbol the
                    //      user never wrote.
                    const size_t closureMarker = funcName.find("__closure_");
                    const std::string displayName =
                        closureMarker == std::string::npos ? funcName : funcName.substr(0, closureMarker);
                    std::string msgAr = "الدالة '" + displayName + "' غير معرفة بعدد معاملات " + std::to_string(arguments.size());
                    std::string msgEn = "Function '" + displayName + "' not defined with " + std::to_string(arguments.size()) + " parameters";

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

                // (AR) [طبقة طبيعي — الخطوة ٤] أنواع الوسائط الساكنة لانتقاء التنسيق
                //      اللا-موقَّع في مدمجات الطباعة/التحويل. نحسبها من شجرات node.arguments
                //      فقط عند محاذاة العدد للقيم (نداء موضعيّ بلا إعادة ترتيب/وسائط مسمّاة)
                //      — وإلا نمرّر null (افتراض موقَّع، لا مخاطرة اختلال محاذاة).
                // (EN) [طبيعي layer — Step 4] Static arg types for unsigned-format selection in
                //      print/convert built-ins. Computed from node.arguments only when the count
                //      aligns with the values (positional call, no reorder/named args) — else pass
                //      null (signed default, no misalignment risk).
                std::vector<Types::SadTypeKind> argTypes;
                const std::vector<Types::SadTypeKind> *argTypesPtr = nullptr;
                if (node.arguments.size() == valuePtrs.size())
                {
                    argTypes.reserve(node.arguments.size());
                    for (const auto &argExpr : node.arguments)
                        argTypes.push_back(resolveStaticType(argExpr.get()));
                    argTypesPtr = &argTypes;
                }

                // (AR) استدعاء التنفيذ الأصلي / (EN) Call native implementation
                // (AR) EM-CPP: تمرير موقع الاستدعاء ليرندر ctx.error من الكتالوج بموقع صحيح.
                auto resultPtr = func->callNative(valuePtrs, node.position, argTypesPtr);

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

            // (AR) دالة خارجية (FFI) — ترمي، ولا تُرجع صفراً صامتاً
            // (EN) Extern function (FFI) — throws; never a silent zero
            // (AR) نظير الحاجز في interpreter_core.cpp — الموضعان يلزمهما التطابق.
            // (EN) Mirrors the gate in interpreter_core.cpp — both sites must agree.
            if (func->isExtern())
            {
                ::Sad::Errors::throwRuntime(::Sad::Errors::ErrorCode::RUN_EXTERN_NOT_SUPPORTED,
                                            node.position, {{"function", func->getName()}});
            }

            // (AR) التحقق من وجود جسم للدالة / (EN) Check if function has body
            if (!func->hasBody())
            {
                Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
                    ::Sad::Errors::ErrorCode::SEM_EMPTY_FUNCTION_BODY,
                    Sad::Errors::SourceLocation(getDispatchSourceFilename(), static_cast<int>(node.position.line), static_cast<int>(node.position.column)));
                lastResult_ = Value();
                return;
            }

            // (AR) تنفيذ جسم دالة المستخدم — مُستخرج إلى ملف منفصل
            // (EN) Execute user function body — extracted to separate file
            executeUserFunctionBody(node, funcName, func, arguments);
        }

    } // namespace Interpreter
} // namespace Sad
