/**
 * @file expression_evaluator.cpp
 * @brief (AR) تنفيذ مُقيِّم التعابير
 * @brief (EN) Expression Evaluator Implementation
 *
 * @author S Language Development Team
 * @date November 21, 2025
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
#include "object_instance.h"
#include "error_manager.h"
#include "ownership_manager.h"
#include "runtime_throw.h"
#include "user_thrown.h"
#include "runtime_throw.h"
#include "async_runtime.h"                  // (AR) نظام التنفيذ غير المتزامن / (EN) Async runtime system
#include "channel.h"                        // (AR) قنوات الاتصال بين المهام / (EN) Channel communication
#include "profiler_hooks.h"                 // (AR) خطافات مصحح الأداء / (EN) Profiler hooks
// (AR) م2-أ (sadlang-rfcs#10): بذرة عكس الاعتماد — لا ضمّ لـ sad_ui في القلب.
//      منطق WidgetBuilder نُقِل إلى sad_graphics_bridge خلف IUIEvalBridge::tryWidgetMethodCall.
// (EN) Phase 2-A: inversion seam — no sad_ui include in the core; WidgetBuilder logic
//      moved to sad_graphics_bridge behind IUIEvalBridge::tryWidgetMethodCall.
#include "ui/ui_eval_bridge.h" // (AR) بادئة ui/ كي تَحلّ في sad_core وsad_interpreter معًا
#include <atomic>
#include <cmath>
#include <climits>
#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <set>

namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;
        using namespace Lexer;
        // =========================================================================
        // (AR) استدعاء طريقة / (EN) Method Call
        // =========================================================================

        void ExpressionEvaluator::visitMethodCallExpr(MethodCallExpr &node)
        {
#ifdef DEBUG_OOP
            std::cout << "[OOP] استدعاء طريقة: " << node.methodName << "\n";
#endif

            // تقييم الكائن
            node.object->accept(*this);
            Value objectValue = lastResult_;

            // (AR) استدعاء طريقة الأساس «الأساس.طريقة()» (ISSUE-019): تقييم «الأساس» يُعيد اسم الصنف
            //      الأب نصًّا فيُحسَب استدعاءً ثابتًا ويُرفض (RUN050). نكتشف أنّ معامل الاستدعاء هو
            //      «الأساس» فنستبدل القيمة بالكائن الحاليّ «هذا» (استدعاء نسخة لا ثابت)، ثم نُعيد توجيه
            //      البحث لاحقًا إلى الصنف الأب لتفادي تكرار الطريقة المتجاوَزة.
            // (EN) Super method call «الأساس.method()» (ISSUE-019): evaluating «الأساس» yields the parent
            //      class name as a string, which is taken as a static call and rejected (RUN050). We
            //      detect that the call's receiver is «الأساس», replace the value with the current «هذا»
            //      (instance call, not static), then redirect method lookup to the base class below to
            //      avoid recursing into the overriding method.
            bool isSuperCall = (dynamic_cast<SuperExpr *>(node.object.get()) != nullptr);
            if (isSuperCall && variableManager_.exists("هذا"))
            {
                objectValue = variableManager_.get("هذا");
            }

            auto *classManager = Data::ClassManager::getInstance();
            std::string className;
            ClassType *classType = nullptr;
            Value::MapType fields;
            bool isStaticCall = false;

            // التحقق من الاستدعاء الثابت: ClassName.staticMethod()
            // Check for static call: ClassName.staticMethod()
            if (objectValue.isString())
            {
                std::string possibleClassName = objectValue.toString();
                classType = classManager->getClass(possibleClassName);

                if (classType)
                {
// هذا استدعاء ثابت: ClassName.staticMethod()
// This is static call: ClassName.staticMethod()
#ifdef DEBUG_OOP
                    std::cout << "[OOP] استدعاء طريقة ثابتة: " << possibleClassName << "." << node.methodName << "\n";
#endif
                    className = possibleClassName;
                    isStaticCall = true;
                }
            }

            // إذا لم يكن استدعاء ثابت، فهو استدعاء عادي على كائن
            // If not static call, it's regular call on object
            if (!isStaticCall)
            {
                // ═══════════════════════════════════════════════════════════════════
                // (AR) نظام الطرق المدمجة على المصفوفات — الطرق المدعومة:
                // (AR) طرق المصفوفات المدمجة — مُستخرجة إلى ملف expression_evaluator_oop_array_methods.cpp
                // (EN) Built-in array methods — extracted to expression_evaluator_oop_array_methods.cpp
                if (objectValue.isArray())
                {
                    handleArrayMethodCall(node, objectValue);
                    return;
                }

                // (AR) طرق النصوص المدمجة — مُستخرجة إلى ملف expression_evaluator_oop_string_map_methods.cpp
                // (EN) Built-in string methods — extracted to expression_evaluator_oop_string_map_methods.cpp
                if (objectValue.isString())
                {
                    handleStringMethodCall(node, objectValue);
                    return;
                }

                // (AR) طرق الخرائط المدمجة — مُستخرجة إلى ملف expression_evaluator_oop_string_map_methods.cpp
                //      فقط الخرائط العادية (بدون __class__) — الكائنات تُعالج في قسم OOP أدناه
                // (EN) Built-in map methods — extracted to expression_evaluator_oop_string_map_methods.cpp
                //      Only plain maps (no __class__) — objects handled in OOP section below
                if (objectValue.isMap())
                {
                    auto mapCheck = objectValue.toMap();
                    if (mapCheck.find("__class__") == mapCheck.end())
                    {
                        handleMapMethodCall(node, objectValue);
                        return;
                    }
                    // (AR) خريطة بها __class__ — كائن OOP، نتركه يسقط للقسم التالي
                }

                // ═══════════════════════════════════════════════════════════════════
                // (AR) دعم سلسلة المعدّلات على WidgetBuilder
                //      .حجم(24).لون("أحمر").هامش(10) — كل طريقة تُعيّن خاصية
                //      على IRNode وتُرجع نفس WidgetBuilder لدعم التسلسل
                // (EN) WidgetBuilder modifier chain support
                //      .حجم(24).لون("red").هامش(10) — each method sets a property
                //      on IRNode and returns same WidgetBuilder for chaining
                // ═══════════════════════════════════════════════════════════════════
                // ═══════════════════════════════════════════════════════════════════
                // (AR) م2-أ: سلسلة معدّلات WidgetBuilder عبر بذرة الجسر (إن حُمِّلت الرسومات).
                //      المنطق في sad_graphics_bridge؛ القلب لا يعرف WidgetBuilder.
                // (EN) Phase 2-A: WidgetBuilder modifier chain via the bridge seam.
                // ═══════════════════════════════════════════════════════════════════
                if (objectValue.isObject())
                {
                    if (IUIEvalBridge *uiBridge = uiEvalBridge())
                    {
                        if (uiBridge->tryWidgetMethodCall(*this, objectValue, node))
                            return;
                    }
                }

                // (AR) طرق التزامن — مُستخرجة إلى ملف expression_evaluator_oop_concurrency.cpp
                // (EN) Concurrency methods — extracted to expression_evaluator_oop_concurrency.cpp
                if (handleConcurrencyMethodCall(node, objectValue))
                    return;

                // ═══════════════════════════════════════════════════════════════════
                // (AR) التحقق من أن القيمة كائن — تدعم كلاً من:
                //      1. نوع OBJECT الحقيقي (الجديد) مع ObjectInstance
                //      2. نوع MAP القديم الذي يحتوي على __class__ (للتوافق)
                //
                // (EN) Check if value is an object — supports both:
                //      1. Real OBJECT type (new) with ObjectInstance
                //      2. Legacy MAP type containing __class__ (backward compat)
                // ═══════════════════════════════════════════════════════════════════
                if (objectValue.isObject())
                {
                    // (AR) نوع OBJECT الحقيقي — الحصول على اسم الصنف من ObjectInstance
                    // (EN) Real OBJECT type — get class name from ObjectInstance
                    auto objPtr = objectValue.toObject();
                    if (objPtr)
                    {
                        className = objPtr->getClassName();
                        classType = classManager->getClass(className);
                        // (AR) تحويل الحقول إلى MAP للتوافق مع باقي الكود
                        // (EN) Convert fields to MAP for compatibility with rest of code
                        fields = objPtr->fields;
                        fields["__class__"] = Value(className);
                    }
                }
                else if (objectValue.isMap())
                {
                    // (AR) نوع MAP القديم — التحقق من وجود __class__
                    // (EN) Legacy MAP type — check for __class__
                    fields = objectValue.toMap();
                    auto classNameIt = fields.find("__class__");
                    if (classNameIt == fields.end())
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_OBJECT_WITHOUT_CLASS,
                            node.position,
                            {{"method", node.methodName}});
                    }
                    className = classNameIt->second.toString();
                    classType = classManager->getClass(className);
                }
                else
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_OPERAND_TYPE_INVALID,
                        node.position,
                        {{"type", objectValue.getTypeName()}, {"operation", "." + node.methodName + "()"}});
                }
            }

            if (!classType)
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_CLASS_NOT_FOUND,
                    node.position,
                    {{"class", className}});
            }

            // (AR) استدعاء أساس: أعد توجيه البحث للصنف الأب لتُستدعى طريقته (لا المتجاوَزة) — ISSUE-019.
            //      «هذا» يبقى الكائن الحاليّ فتُربَط الحقول صحيحةً، لكن الطريقة تُؤخَذ من الأب.
            // (EN) Super call: redirect lookup to the base class so its (non-overridden) method runs —
            //      ISSUE-019. «هذا» stays the current instance (fields bind correctly), but the method
            //      is taken from the parent.
            if (isSuperCall && classType && classType->getBaseClass())
            {
                classType = classType->getBaseClass();
                className = classType->name;
            }

            // البحث عن الطريقة (في السلسلة الهرمية)
            ClassMethod *method = classType->findMethod(node.methodName);
            if (!method)
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND,
                    node.position,
                    {{"method", node.methodName}, {"class", className}});
            }

            // التحقق من التطابق بين نوع الاستدعاء ونوع الطريقة
            // Verify call type matches method type
            if (isStaticCall && !method->isStatic)
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_PERMISSION_DENIED,
                    node.position,
                    {{"resource", "non-static method '" + node.methodName + "' via class name"}});
            }
            if (!isStaticCall && method->isStatic)
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_PERMISSION_DENIED,
                    node.position,
                    {{"resource", "static method '" + node.methodName + "' via instance"}});
            }

            // فحص الوصول (Phase 6.1: Access Modifiers)
            checkMemberAccess(method->visibility, node.methodName, classType);

            // حساب عدد المعاملات المطلوبة (التي ليس لها قيم افتراضية)
            // (AR) Count required parameters (without default values)
            size_t requiredParams = 0;
            for (const auto &param : method->parameters)
            {
                if (!param.defaultValue)
                    requiredParams++;
            }

            // التحقق من عدد المعاملات - يجب أن يكون على الأقل المعاملات المطلوبة
            // وليس أكثر من إجمالي المعاملات
            if (node.arguments.size() < requiredParams ||
                node.arguments.size() > method->parameters.size())
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_TOO_MANY_ARGS,
                    node.position,
                    {{"function", className + "." + node.methodName},
                     {"expected", std::to_string(method->parameters.size())},
                     {"actual", std::to_string(node.arguments.size())}});
            }

            // تقييم المعاملات
            std::vector<Value> argValues;
            for (auto &arg : node.arguments)
            {
                arg->accept(*this);
                argValues.push_back(lastResult_);
            }

            // إنشاء scope جديد للطريقة
            variableManager_.enterScope(Data::ScopeType::FUNCTION, node.methodName);

            // ═══════════════════════════════════════════════════════════════════════
            // (AR) إضافة حقول الكائن للـ scope أولاً (محاكاة 'this') - فقط للطرق غير الثابتة
            //      ثم ربط المعاملات — حتى تتغلب المعاملات على الحقول عند تطابق الأسماء
            //      هذا يمنع خطأ "parameter shadowing" حيث الحقل يُظلل المعامل
            //
            // (EN) Add object fields to scope FIRST (simulate 'this') - only for non-static
            //      THEN bind parameters — so parameters override fields when names match
            //      This prevents "parameter shadowing" bug where field overwrites param
            // ═══════════════════════════════════════════════════════════════════════
            if (!isStaticCall)
            {
                // إضافة 'this' reference للكائن الحالي
                // Add 'this' reference to current object
                variableManager_.define("هذا", objectValue);
                variableManager_.define("this", objectValue);

                for (const auto &[name, value] : fields)
                {
                    if (name != "__class__")
                    {
                        variableManager_.define(name, value);
                    }
                }
            }

            // (AR) ربط المعاملات بالقيم أو القيم الافتراضية — بعد الحقول لتتغلب عليها
            // (EN) Bind parameters to values or default values — AFTER fields to override them
            for (size_t i = 0; i < method->parameters.size(); ++i)
            {
                if (i < argValues.size())
                {
                    // (AR) استخدام القيمة المُمررة
                    variableManager_.define(method->parameters[i].name, argValues[i]);
                }
                else if (method->parameters[i].defaultValue)
                {
                    // (AR) استخدام القيمة الافتراضية - نقيّمها الآن
                    method->parameters[i].defaultValue->accept(*this);
                    variableManager_.define(method->parameters[i].name, lastResult_);
                }
            }

            // (AR) الحقول الثابتة لا تُضاف للنطاق المحلي — يُوصل إليها عبر اسم_الصنف.حقل فقط
            // (EN) Static fields are NOT added to local scope — accessed via ClassName.field only

            // ═══════════════════════════════════════════════════════════════
            // (AR) فحص العقود البرمجية: الشروط المسبقة (يتطلب) على الطرق
            // (EN) Design by Contract: check preconditions (requires) on methods
            // ═══════════════════════════════════════════════════════════════
            if (!method->preconditions.empty())
            {
                for (size_t pc = 0; pc < method->preconditions.size(); ++pc)
                {
                    auto &precond = method->preconditions[pc];
                    if (precond)
                    {
                        precond->accept(*this);
                        Data::Value condResult = lastResult_;
                        if (condResult.isBoolean() && !condResult.toBool())
                        {
                            variableManager_.exitScope();
                            std::string condStr = precond->toString();
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_CONTRACT_PRECOND_FAILED,
                                node.position,
                                {{"index", std::to_string(pc + 1)},
                                 {"function", className + "." + node.methodName},
                                 {"expr", condStr}});
                        }
                    }
                }
            }

            // تنفيذ جسم الطريقة
            // (AR) نبحث عن الجسم في ClassType أولاً، ثم في مصدر القالب
            // (EN) Look for body in ClassType first, then in template instance source
            AST::Statement *methodBody = nullptr;
            if (method->getBody())
            {
                methodBody = method->getBody();
            }
            else
            {
                // (AR) البحث في مصدر القالب
                // (EN) Look up method body from template instance source
                auto *templateSrc = statementExecutor_.getTemplateInstanceSource(className);
                if (templateSrc)
                {
                    for (auto &member : templateSrc->members)
                    {
                        if (auto *methodDecl = dynamic_cast<AST::MethodDecl *>(member.get()))
                        {
                            if (methodDecl->name == node.methodName && methodDecl->body)
                            {
                                methodBody = methodDecl->body.get();
                                break;
                            }
                        }
                    }
                }
            }

            Value returnValue;
            bool hadExplicitReturn = false;
            try
            {
                if (methodBody)
                {
                    methodBody->accept(statementExecutor_);

                    // التحقق من وجود return
                    if (statementExecutor_.getFlowControl() == FlowControl::RETURN)
                    {
                        returnValue = statementExecutor_.getReturnValue();
                        hadExplicitReturn = true;
                        statementExecutor_.resetFlowControl();
                    }

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) فحص العقود البرمجية: الشروط اللاحقة (يضمن) على الطرق
                    // (EN) Design by Contract: check postconditions (ensures) on methods
                    // ═══════════════════════════════════════════════════════════════
                    if (!method->postconditions.empty())
                    {
                        // (AR) تعريف 'النتيجة' كمتغير مؤقت لاستخدامه في الشروط اللاحقة
                        // (EN) Define 'النتيجة' as temp variable for postconditions
                        variableManager_.define("النتيجة", returnValue);

                        for (size_t pc = 0; pc < method->postconditions.size(); ++pc)
                        {
                            auto &postcond = method->postconditions[pc];
                            if (postcond)
                            {
                                postcond->accept(*this);
                                Data::Value condResult = lastResult_;
                                if (condResult.isBoolean() && !condResult.toBool())
                                {
                                    variableManager_.exitScope();
                                    std::string condStr = postcond->toString();
                                    ::Sad::Errors::throwRuntime(
                                        ::Sad::Errors::ErrorCode::RUN_CONTRACT_POSTCOND_FAILED,
                                        node.position,
                                        {{"index", std::to_string(pc + 1)},
                                         {"function", className + "." + node.methodName},
                                         {"expr", condStr}});
                                }
                            }
                        }
                    }

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) جمع القيم المحدثة للحقول — تحديث الكائن الحقيقي
                    //      إذا كان الكائن من نوع OBJECT، نحدّث حقول ObjectInstance
                    //      مباشرة (بالمرجع). إذا كان MAP قديم، نعيد بناء MAP.
                    //
                    // (EN) Collect updated field values — update the real object
                    //      If object is OBJECT type, update ObjectInstance fields
                    //      directly (by reference). If legacy MAP, rebuild MAP.
                    // ═══════════════════════════════════════════════════════════════
                    ClassType *currentClass = classType;
                    while (currentClass)
                    {
                        for (const auto &field : currentClass->fields)
                        {
                            // (AR) تخطي الحقول الثابتة — تُدار مباشرة عبر ClassType::setStaticField
                            // (EN) Skip static fields — managed directly via ClassType::setStaticField
                            if (field.isStatic)
                                continue;

                            if (!isStaticCall)
                            {
                                try
                                {
                                    Value updatedValue = variableManager_.get(field.name);
                                    // (AR) تحديث حقل الكائن (MAP مؤقت)
                                    // (EN) Update object field (temporary MAP)
                                    fields[field.name] = updatedValue;
                                }
                                catch (const std::exception &)
                                {
                                    // (AR) الحقل لم يتم تعديله — متوقع إذا لم يكن في النطاق
                                    // (EN) Field was not modified — expected if not in scope
                                }
                            }
                        }
                        currentClass = currentClass->getBaseClass();
                    }

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) تحديث الكائن الأصلي — فقط للطرق غير الثابتة
                    //      إذا كان MAP قديم: نبني MAP جديد ونعيّنه.
                    //      إذا كان OBJECT حقيقي: لا نحتاج فعل شيء لأن التحديثات
                    //      تحدث مباشرة في objPtr->fields عبر visitMemberAssignExpr.
                    //
                    // (EN) Update original object — only for non-static methods
                    //      If legacy MAP: build new MAP and assign it.
                    //      If real OBJECT: no need to do anything because updates
                    //      happen directly in objPtr->fields via visitMemberAssignExpr.
                    //
                    // ═══════════════════════════════════════════════════════════════
                    // (AR) إصلاح المشكلة 18: كان الكود القديم يكتب `fields` (نسخة قديمة)
                    //      فوق objPtr->fields بعد تنفيذ الطريقة، مما يمحي التغييرات
                    //      التي حدثت أثناء تنفيذ الطريقة (مثل هذا.زد() في الباني).
                    //      الحل: لا نُعيد الكتابة على OBJECT حقيقي.
                    //
                    // (EN) Fix issue 18: old code was writing `fields` (stale copy)
                    //      over objPtr->fields after method execution, erasing changes
                    //      made during method execution (like this.increment() in ctor).
                    //      Solution: don't overwrite real OBJECT.
                    // ═══════════════════════════════════════════════════════════════
                    if (!isStaticCall)
                    {
                        if (objectValue.isObject())
                        {
                            // (AR) الكائنات الحقيقية (OBJECT type): لا نفعل شيئاً!
                            //      التعيين المباشر في visitMemberAssignExpr يُحدّث
                            //      objPtr->fields مباشرة بالمرجع، فلا نحتاج كتابة راجعة.
                            // (EN) Real objects (OBJECT type): do nothing!
                            //      Direct assignment in visitMemberAssignExpr updates
                            //      objPtr->fields directly by reference, no writeback needed.
                        }
                        else if (auto *varExpr = dynamic_cast<VariableExpr *>(node.object.get()))
                        {
                            // (AR) MAP القديم: نبني MAP جديد من الحقول المحدثة
                            // (EN) Legacy MAP: build new MAP from updated fields
                            Value modifiedObject(fields);
                            variableManager_.assign(varExpr->name, modifiedObject);
                        }
                    }
                }
            }
            catch (const std::exception &)
            {
                variableManager_.exitScope();
                throw;
            }

            variableManager_.exitScope();

#ifdef DEBUG_OOP

            std::cout << "[OOP] ✅ تم تنفيذ الطريقة: " << node.methodName << "\n";
#endif
            // ═══════════════════════════════════════════════════════════════════
            // (AR) إرجاع ضمني لـ 'هذا': إذا لم تُرجع الدالة قيمة صريحة (بدون ارجع)
            //      ولم تكن الدالة ثابتة، نُرجع الكائن نفسه تلقائياً
            //      لدعم تسلسل الاستدعاءات: كائن.دالة1().دالة2().دالة3()
            //      ملاحظة: إذا كان هناك 'ارجع لاشيء' صريح، فنُرجع لاشيء.
            //
            // (EN) Implicit 'this' return: if method has no explicit return statement
            //      (no ارجع at all), and is not static, automatically return the object
            //      to support method chaining: obj.method1().method2().method3()
            //      Note: if there's an explicit 'ارجع لاشيء', we return لاشيء.
            // ═══════════════════════════════════════════════════════════════════
            if (!hadExplicitReturn && !isStaticCall)
            {
                lastResult_ = objectValue;
            }
            else
            {
                lastResult_ = returnValue;
            }
        }

    } // namespace Interpreter
} // namespace Sad
