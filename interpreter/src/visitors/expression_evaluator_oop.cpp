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
#include "../include/ui/ui_state_manager.h" // (AR) مدير الحالة التفاعلية / (EN) Reactive state manager
#if __has_include("sad_ui/ir.h")
#include "../ui/widget_builder.h" // (AR) دعم سلسلة المعدّلات / (EN) Modifier chain support
#define HAS_WIDGET_BUILDER 1
#endif
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
#ifdef HAS_WIDGET_BUILDER
                if (objectValue.isObject())
                {
                    auto *objPtr = objectValue.toObject();
                    if (objPtr && isWidgetBuilder(objPtr))
                    {
                        auto *wb = static_cast<Sad::Interpreter::WidgetBuilder *>(objPtr);
                        const std::string &m = node.methodName;

                        // (AR) تقييم المعاملات
                        std::vector<Value> args;
                        for (auto &arg : node.arguments)
                        {
                            arg->accept(*this);
                            args.push_back(lastResult_);
                        }

                        // (AR) طريقة ابن/أبناء — لإضافة عناصر فرعية
                        if (m == "\xd8\xa7\xd8\xa8\xd9\x86" || m == "child")
                        {
                            // ابن
                            if (!args.empty() && args[0].isObject())
                            {
                                auto childObj = args[0].toObject();
                                if (childObj && isWidgetBuilder(childObj))
                                {
                                    auto *childWB = static_cast<WidgetBuilder *>(childObj);
                                    wb->addChild(childWB->getIRNode());
                                }
                            }
                            lastResult_ = objectValue;
                            return;
                        }
                        if (m == "\xd8\xa3\xd8\xa8\xd9\x86\xd8\xa7\xd8\xa1" || m == "children")
                        {
                            // أبناء
                            for (auto &a : args)
                            {
                                if (a.isObject())
                                {
                                    auto childObj = a.toObject();
                                    if (childObj && isWidgetBuilder(childObj))
                                    {
                                        auto *childWB = static_cast<WidgetBuilder *>(childObj);
                                        wb->addChild(childWB->getIRNode());
                                    }
                                }
                            }
                            lastResult_ = objectValue;
                            return;
                        }

                        // (AR) طريقة عند_* — تسجيل حدث
                        if (m.find("\xd8\xb9\xd9\x86\xd8\xaf_") == 0 || m.find("on_") == 0)
                        {
                            // عند_النقر، عند_التغيير...
                            if (!args.empty())
                            {
                                // (AR) نحفظ المعالج كحقل على WidgetBuilder
                                //      ونضيف حدث بمعرّف مؤقت — UIBridge يسجّله لاحقاً
                                std::string eventKey = "__event_" + m;
                                wb->fields[eventKey] = args[0];
                                wb->addIREvent(m, eventKey);
                            }
                            lastResult_ = objectValue;
                            return;
                        }

                        // ═══════════════════════════════════════════════════════
                        // (AR) معدّلات التحريك — .حرّك("نوع").مدة(ثوان).منحنى("اسم")
                        //      تُنشئ IRAnimation على IRNode عبر WidgetBuilder
                        // (EN) Animation modifiers — .حرّك("type").مدة(sec).منحنى("name")
                        //      Supports compound: .حرّك("ظهور,دوران") or .حرّك("ظهور", "دوران")
                        // ═══════════════════════════════════════════════════════
                        // حرّك / animate — بدء تسلسل تحريك جديد (أو مركّب بفاصلة)
                        if (m == "\xd8\xad\xd8\xb1\xd9\x91\xd9\x83" || m == "animate" ||
                            m == "\xd8\xaa\xd8\xad\xd8\xb1\xd9\x8a\xd9\x83" || m == "\xd8\xad\xd8\xb1\xd9\x83")
                        {
                            // (AR) جمع كل أسماء الأنواع من الوسائط
                            std::vector<std::string> types;

                            // (AR) معالجة كل وسيط — قد يحتوي فاصلة داخلية
                            for (size_t ai = 0; ai < args.size(); ++ai)
                            {
                                std::string raw = args[ai].toString();

                                // (AR) تقسيم بالفاصلة (ASCII ',' فقط — الفاصلة العربية ،
                                //      تُحوَّل تلقائياً بواسطة المحلل المعجمي)
                                size_t start = 0;
                                for (size_t i = 0; i <= raw.size(); ++i)
                                {
                                    if (i == raw.size() || raw[i] == ',')
                                    {
                                        // (AR) استخراج الجزء وتنظيفه من المسافات
                                        std::string part;
                                        for (size_t j = start; j < i; ++j)
                                            if (raw[j] != ' ' && raw[j] != '\t')
                                                part += raw[j];
                                        if (!part.empty())
                                            types.push_back(part);
                                        start = i + 1;
                                    }
                                }
                            }

                            // (AR) احتياطي — إذا لم تنتج أي أنواع
                            if (types.empty())
                                types.push_back("fadeIn");

                            // (AR) وضع المجموعة المركبة إذا كان هناك أكثر من نوع
                            bool isCompound = types.size() > 1;
                            if (isCompound)
                                wb->beginCompoundBatch();

                            for (const auto &t : types)
                                wb->startAnimationChain(t);

                            if (isCompound)
                                wb->endCompoundBatch();

                            lastResult_ = objectValue;
                            return;
                        }

                        // مدة / duration — تعيين مدة التحريك (تُطبّق على كل المجموعة المركبة)
                        if (m == "\xd9\x85\xd8\xaf\xd8\xa9" || m == "duration")
                        {
                            if (wb->isInAnimationChain() && !args.empty())
                            {
                                float dur = static_cast<float>(args[0].toDouble());
                                wb->applyToActiveAnimations([dur](sad::ui::IRAnimation &a)
                                                            { a.duration = dur; });
                            }
                            lastResult_ = objectValue;
                            return;
                        }

                        // منحنى / easing — تعيين منحنى التحريك (تُطبّق على كل المجموعة المركبة)
                        if (m == "\xd9\x85\xd9\x86\xd8\xad\xd9\x86\xd9\x89" || m == "easing" || m == "\xd9\x85\xd9\x86\xd8\xad\xd9\x86\xd8\xa7")
                        {
                            if (wb->isInAnimationChain() && !args.empty())
                            {
                                auto curve = sad::ui::stringToEasingCurve(args[0].toString());
                                wb->applyToActiveAnimations([curve](sad::ui::IRAnimation &a)
                                                            { a.easing = curve; });
                            }
                            lastResult_ = objectValue;
                            return;
                        }

                        // تأخير / delay — تعيين التأخير قبل التحريك (تُطبّق على كل المجموعة المركبة)
                        if (m == "\xd8\xaa\xd8\xa3\xd8\xae\xd9\x8a\xd8\xb1" || m == "delay")
                        {
                            if (wb->isInAnimationChain() && !args.empty())
                            {
                                float d = static_cast<float>(args[0].toDouble());
                                wb->applyToActiveAnimations([d](sad::ui::IRAnimation &a)
                                                            { a.delay = d; });
                            }
                            lastResult_ = objectValue;
                            return;
                        }

                        // تكرار / repeat — عدد تكرارات التحريك (تُطبّق على كل المجموعة المركبة)
                        if (m == "\xd8\xaa\xd9\x83\xd8\xb1\xd8\xa7\xd8\xb1" || m == "repeat")
                        {
                            if (wb->isInAnimationChain() && !args.empty())
                            {
                                int r = static_cast<int>(args[0].toInt());
                                wb->applyToActiveAnimations([r](sad::ui::IRAnimation &a)
                                                            { a.repeatCount = r; });
                            }
                            lastResult_ = objectValue;
                            return;
                        }

                        // عكس_تلقائي / autoReverse — عكس الحركة تلقائياً (تُطبّق على كل المجموعة المركبة)
                        if (m == "\xd8\xb9\xd9\x83\xd8\xb3_\xd8\xaa\xd9\x84\xd9\x82\xd8\xa7\xd8\xa6\xd9\x8a" || m == "autoReverse" || m == "auto_reverse")
                        {
                            if (wb->isInAnimationChain())
                            {
                                bool rev = args.empty() ? true : args[0].toBool();
                                wb->applyToActiveAnimations([rev](sad::ui::IRAnimation &a)
                                                            { a.autoReverse = rev; });
                            }
                            lastResult_ = objectValue;
                            return;
                        }

                        // (AR) إذا كان في تسلسل تحريك ووصلنا لمعدّل غير تحريكي — نُنهي التحريك أولاً
                        if (wb->isInAnimationChain())
                        {
                            wb->commitAnimation();
                        }

                        // (AR) أي طريقة أخرى → معدّل خاصية بصرية
                        //      الوسيط الوحيد → خاصية بقيمة
                        //      بدون وسائط → خاصية منطقية true
                        //      عدة وسائط → قيم مفصولة بفواصل
                        if (args.empty())
                        {
                            wb->setIRProperty(m, true);
                        }
                        else if (args.size() == 1)
                        {
                            wb->setIRPropertyFromValue(m, args[0]);
                        }
                        else
                        {
                            std::string combined;
                            for (size_t i = 0; i < args.size(); i++)
                            {
                                if (i > 0)
                                    combined += ",";
                                combined += args[i].toString();
                            }
                            wb->setIRProperty(m, combined);
                        }

                        lastResult_ = objectValue;
                        return;
                    }
                }
#endif // HAS_WIDGET_BUILDER

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
