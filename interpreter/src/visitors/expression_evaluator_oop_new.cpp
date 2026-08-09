/**
 * @file expression_evaluator_oop_new.cpp
 * @brief (AR) تنفيذ تعبير إنشاء كائن جديد — visitNewExpr
 * @brief (EN) New Object Creation Expression — visitNewExpr
 *
 * (AR) هذا الملف يحتوي على تنفيذ visitNewExpr الذي يُنشئ كائنات جديدة
 *      من الأصناف المعرّفة. يدعم:
 *      ✓ إنشاء كائنات حقيقية (ObjectInstance) بمؤشر مشترك
 *      ✓ أصناف القوالب (Template Classes)
 *      ✓ فحص الأصناف المجردة
 *      ✓ سلسلة بانين الوراثة (Multi-level Constructor Chaining)
 *      ✓ المعاملات المسماة بنمط Flutter
 *      ✓ حقول القيم الافتراضية
 *      ✓ النظام التفاعلي لمكونات واجهة المستخدم
 *
 * (EN) This file contains visitNewExpr implementation that creates new
 *      objects from defined classes. Supports:
 *      ✓ Real ObjectInstance creation with shared_ptr
 *      ✓ Template classes
 *      ✓ Abstract class checks
 *      ✓ Multi-level constructor chaining
 *      ✓ Flutter-style named arguments
 *      ✓ Default field values
 *      ✓ UI reactive system hookup
 *
 * @author S Language Development Team
 * @date November 21, 2025
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
#include "profiler_hooks.h"
#include "../include/ui/ui_state_manager.h"
#include <functional>
#include <map>
#include <set>
#include <unordered_map>

namespace Sad
{
    namespace Interpreter
    {

        using namespace Data;
        using namespace AST;
        using namespace Lexer;

        // =========================================================================
        // (AR) تقييم إنشاء كائن جديد / (EN) New Object Creation Evaluation
        // =========================================================================

        void ExpressionEvaluator::visitNewExpr(NewExpr &node)
        {
            // ═══════════════════════════════════════════════════════════════════════
            // (AR) تنفيذ تعبير إنشاء كائن جديد — 'جديد صنف(معاملات)' أو 'صنف(معاملات)'
            //
            //      الآن يُنشئ كائناً حقيقياً من نوع ObjectInstance بمؤشر مشترك
            //      بدلاً من استخدام MAP مؤقت. هذا يسمح بـ:
            //      ✓ تمرير الكائنات كمعاملات للدوال بالمرجع
            //      ✓ هوية فريدة لكل كائن (objectId)
            //      ✓ البحث في سلسلة الوراثة (baseInstance)
            //      ✓ إدارة الذاكرة تلقائياً عبر shared_ptr
            //
            // (EN) Execute new object expression — 'new Class(args)' or 'Class(args)'
            //
            //      Now creates a real ObjectInstance with shared_ptr
            //      instead of using a temporary MAP. This enables:
            //      ✓ Passing objects as function parameters by reference
            //      ✓ Unique identity for each object (objectId)
            //      ✓ Inheritance chain lookup (baseInstance)
            //      ✓ Automatic memory management via shared_ptr
            // ═══════════════════════════════════════════════════════════════════════

#ifdef DEBUG_OOP
            std::cout << "[OOP] تنفيذ تعبير جديد: " << node.className << "\n";
#endif

            // (AR) دعم أصناف القوالب: جديد صنف<نوع>(معاملات)
            // (EN) Template class support: new Class<Type>(args)
            std::string effectiveClassName = node.className;
            if (!node.templateArguments.empty())
            {
                // (AR) هذا إنشاء صنف قالب - نحتاج إنشاء نسخة ملموسة
                // (EN) This is a template class instantiation - need to create concrete version
                auto *templateDecl = statementExecutor_.getTemplateClass(node.className);
                if (templateDecl)
                {
                    // (AR) التحقق من قيود القالب قبل الإنشاء
                    // (EN) Verify template constraints before instantiation
                    if (!node.templateArgumentNames.empty())
                    {
                        statementExecutor_.verifyTemplateConstraints(
                            templateDecl->typeParameters,
                            node.templateArgumentNames,
                            templateDecl->whereClause,
                            node.className,
                            node.position);
                    }

                    // (AR) إنشاء النسخة الملموسة تحت الاسم الأصلي (بدون تغيير)
                    // (EN) Instantiate concrete version under original name (unchanged)
                    // لأن المفسر ديناميكي النوع، النسخة واحدة تكفي لكل الأنواع
                    statementExecutor_.instantiateTemplateClass(*templateDecl, node.className);
                    effectiveClassName = node.className;
                }
                else
                {
                    // (AR) ربما الصنف مسجّل بالفعل (من تنفيذ سابق)
                    // (EN) Perhaps the class is already registered (from previous execution)
                    // نتابع بالاسم الأصلي
                }
            }

            // (AR) الحصول على ClassType من ClassManager
            // (EN) Get ClassType from ClassManager
            auto *classManager = Data::ClassManager::getInstance();
            ClassType *classType = classManager->getClass(effectiveClassName);

            if (!classType)
            {
                // (AR) آخر محاولة: هل هو صنف قالب ولم نحدد الأنواع؟
                // (EN) Last attempt: is it a template class without type args?
                auto *templateDecl = statementExecutor_.getTemplateClass(node.className);
                if (templateDecl)
                {
                    statementExecutor_.instantiateTemplateClass(*templateDecl, node.className);
                    classType = classManager->getClass(node.className);
                }

                if (!classType)
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_CLASS_NOT_FOUND,
                        node.position,
                        {{"class", effectiveClassName}});
                }
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) فحص الصنف المجرد — لا يمكن إنشاء كائن من صنف مجرد
            //      يفحص السلسلة الهرمية بالكامل للبحث عن دوال مجردة غير منفذة
            // (EN) Abstract class check — cannot instantiate abstract class
            //      Walks the full inheritance chain to find unimplemented abstract methods
            // ═══════════════════════════════════════════════════════════════════
            {
                // (AR) جمع جميع الدوال المجردة من الأصناف الأب
                // (EN) Collect all abstract methods from parent classes
                std::set<std::string> abstractMethods;
                ClassType *checkClass = classType;
                while (checkClass)
                {
                    for (const auto &method : checkClass->methods)
                    {
                        if (method.isAbstract)
                        {
                            abstractMethods.insert(method.name);
                        }
                    }
                    checkClass = checkClass->getBaseClass();
                }
                // (AR) إزالة الدوال التي تم تنفيذها (غير مجردة) في أي مستوى
                // (EN) Remove methods that have concrete implementations at any level
                checkClass = classType;
                while (checkClass)
                {
                    for (const auto &method : checkClass->methods)
                    {
                        if (!method.isAbstract)
                        {
                            abstractMethods.erase(method.name);
                        }
                    }
                    checkClass = checkClass->getBaseClass();
                }
                if (!abstractMethods.empty())
                {
                    std::string methodName = *abstractMethods.begin();
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_ABSTRACT_INSTANTIATION,
                        node.position,
                        {{"class", effectiveClassName}, {"method", methodName}});
                }
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) إنشاء كائن حقيقي من نوع ObjectInstance باستخدام shared_ptr
            //      هذا يختلف عن السلوك القديم الذي كان يستخدم MAP
            //
            // (EN) Create a real ObjectInstance using shared_ptr
            //      This differs from the old behavior which used MAP
            // ═══════════════════════════════════════════════════════════════════
            size_t objId = generateObjectId();
            // (AR) B-step5b: ObjectInstance يُخصَّص بـnew ويُدار بـGC (لا shared_ptr)
            // (EN) B-step5b: ObjectInstance allocated with new, managed by GC (no shared_ptr)
            auto *objectInstance = new ObjectInstance(classType, objId);

            // (AR) خطاف مصحح الأداء — تتبع إنشاء الكائنات
            // (EN) Profiler hook — track object creation
            if (auto *prof = Sad::Tools::getGlobalProfiler())
            {
                prof->onObjectCreated(effectiveClassName);
            }

            // (AR) تهيئة الحقول بقيم افتراضية (بما في ذلك الحقول الموروثة)
            // (EN) Initialize fields with default values (including inherited fields)
            std::vector<ClassField> allFields;

            // (AR) جمع جميع الحقول من السلسلة الهرمية
            // (EN) Collect all fields from the class hierarchy
            ClassType *currentClass = classType;
            while (currentClass)
            {
                for (const auto &field : currentClass->fields)
                {
                    allFields.push_back(field);
                }
                currentClass = currentClass->getBaseClass();
            }

            // (AR) تهيئة جميع الحقول غير الثابتة في الكائن
            // (EN) Initialize all non-static fields in the object
            // ═══════════════════════════════════════════════════════════════════
            // (AR) الآن نستخدم defaultValue من ClassField بدلاً من Value() فارغة
            //      هذا يدعم تحديد قيم افتراضية للحقول في تعريف الصنف
            //      مثال: عام متغير الاسم: نص = "محمد"
            // (EN) Now using defaultValue from ClassField instead of empty Value()
            //      This supports default field values in class definition
            //      Example: public var name: string = "محمد"
            // ═══════════════════════════════════════════════════════════════════
            for (const auto &field : allFields)
            {
                if (field.isStatic)
                    continue;

                // (AR) استخدام نسخة عميقة من القيمة الافتراضية لتجنب مشاركة المصفوفات/القواميس بين الكائنات
                // (EN) Use deep clone of default value to avoid sharing arrays/maps between instances
                objectInstance->initField(field.name, field.defaultValue.clone());
            }

#ifdef DEBUG_OOP
            std::cout << "[OOP] تم إنشاء كائن من صنف: " << node.className << " (ID: " << objId << ")\n";
            std::cout << "[OOP] عدد الحقول: " << allFields.size() << " (بما في ذلك الموروثة)\n";
#endif

            // ═══════════════════════════════════════════════════════════════════
            // (AR) استدعاء الباني إذا كان موجودًا
            // (EN) Call constructor if it exists
            // ═══════════════════════════════════════════════════════════════════
            AST::ConstructorDecl *constructor = nullptr;
            if (classType->constructor)
            {
                constructor = classType->constructor.get();
            }
            else
            {
                // (AR) البحث في مصادر نسخ القوالب
                // (EN) Look up constructor from template instance source
                auto *templateSrc = statementExecutor_.getTemplateInstanceSource(effectiveClassName);
                if (templateSrc)
                {
                    for (auto &member : templateSrc->members)
                    {
                        if (auto *ctorDecl = dynamic_cast<AST::ConstructorDecl *>(member.get()))
                        {
                            constructor = ctorDecl;
                            break;
                        }
                    }
                }
            }

            if (constructor)
            {
                // ═══════════════════════════════════════════════════════════════
                // (AR) دعم المعاملات المسماة بنمط Flutter:
                //      نص("مرحباً", حجم: 32, لون: "أزرق")
                //      ← "مرحباً" معامل مكاني، حجم ولون معاملات مسماة
                //
                // (EN) Flutter-style named argument support:
                //      text("Hello", size: 32, color: "blue")
                //      ← "Hello" is positional, size and color are named
                // ═══════════════════════════════════════════════════════════════

                // (AR) تقييم المعاملات — فصل المكانية عن المسماة
                // (EN) Evaluate arguments — separate positional from named
                std::vector<Value> positionalArgs;
                std::unordered_map<std::string, Value> namedArgs;

                for (auto &arg : node.arguments)
                {
                    if (auto *named = dynamic_cast<AST::NamedArgExpr *>(arg.get()))
                    {
                        named->value->accept(*this);
                        namedArgs[named->name] = lastResult_;
                    }
                    else
                    {
                        arg->accept(*this);
                        positionalArgs.push_back(lastResult_);
                    }
                }

                // (AR) التحقق: المعاملات المكانية لا تتجاوز عدد معاملات الباني
                // (EN) Check: positional args cannot exceed constructor params
                if (positionalArgs.size() > constructor->parameters.size())
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_TOO_MANY_ARGS,
                        node.position,
                        {{"function", "constructor"},
                         {"expected", std::to_string(constructor->parameters.size())},
                         {"actual", std::to_string(positionalArgs.size())}});
                }

                // (AR) إنشاء scope جديد للباني
                // (EN) Create new scope for constructor
                variableManager_.enterScope(Data::ScopeType::FUNCTION, "constructor");

                // (AR) ربط المعاملات بالقيم (مكانية ← مسماة ← افتراضية)
                // (EN) Bind parameters (positional → named → default)
                for (size_t i = 0; i < constructor->parameters.size(); ++i)
                {
                    const std::string &paramName = constructor->parameters[i].name;

                    // (AR) أولوية 1: المعاملات المكانية (بالترتيب)
                    // (EN) Priority 1: Positional arguments (in order)
                    if (i < positionalArgs.size())
                    {
                        variableManager_.define(paramName, positionalArgs[i]);
                    }
                    // (AR) أولوية 2: المعاملات المسماة (بالاسم)
                    // (EN) Priority 2: Named arguments (by name)
                    else if (namedArgs.count(paramName))
                    {
                        variableManager_.define(paramName, namedArgs[paramName]);
                    }
                    // (AR) أولوية 3: القيم الافتراضية
                    // (EN) Priority 3: Default values
                    else if (constructor->parameters[i].defaultValue)
                    {
                        constructor->parameters[i].defaultValue->accept(*this);
                        variableManager_.define(paramName, lastResult_);
                    }
                    // (AR) أولوية ٤: وسيطٌ لم يُمرَّر ولا افتراضيَّ له ⇒ **عدمٌ** (لاشيء)
                    //      لا فراغ. والفرقُ ليس تجميليًّا: `Value()` تُنشئ فراغًا (Void)،
                    //      وvalue.h:244 ينصُّ أنّ العدمَ نوعٌ متمايزٌ عن الفراغِ تمامًا
                    //      (S-TS-P1). فكان `منفذ == لاشيء` يعطي **خطأً** لمعاملٍ مُغفَل،
                    //      فيمرُّ حارسُ `منفذ != لاشيء` ولا يقصرُ `و` دائرتَه، فيُقيَّم
                    //      `منفذ > 0` على فراغٍ ⇒ SEM010 «لا تُدعَم إلّا == و!=».
                    //      وهو النمطُ الذي تقومُ عليه المكتبةُ القياسيّةُ كلُّها
                    //      (شبكات.ص:90 · 678 · 809)، فالوسيطُ المُغفَلُ عدمٌ بالتعريف.
                    // (EN) Priority 4: an argument neither passed nor defaulted is **null**,
                    //      not void. Not cosmetic: `Value()` builds a Void, and value.h:244
                    //      states Null is a fully distinct kind from Void (S-TS-P1). So
                    //      `port == null` returned false for an omitted parameter, the
                    //      `port != null` guard passed, `and` did not short-circuit, and
                    //      `port > 0` was evaluated on a void ⇒ SEM010. The whole stdlib
                    //      rests on that idiom; an omitted argument is null by definition.
                    else
                    {
                        variableManager_.define(paramName, Value::makeNull());
                    }
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) ربط 'هذا'/'this' بالكائن الحقيقي كقيمة OBJECT
                //      هذا يسمح للباني بتعديل حقول الكائن مباشرة
                //
                // (EN) Bind 'this'/هذا to the real object as OBJECT value
                //      This allows the constructor to modify object fields directly
                // ═══════════════════════════════════════════════════════════════
                Value objectValue(objectInstance);
                variableManager_.define("هذا", objectValue);
                variableManager_.define("this", objectValue);

                // (AR) تعيين الصنف المنفذ حالياً لدعم سلسلة أساس() متعددة المستويات
                // (EN) Set executing constructor class to support multi-level أساس() chains
                variableManager_.define("__executing_constructor_class__", Value(effectiveClassName));

                // ═══════════════════════════════════════════════════════════════
                // (AR) إضافة حقول الكائن للـ scope لسهولة الوصول المباشر
                //      ملاحظة: نتخطى الحقول التي تتعارض أسماؤها مع معاملات الباني
                //      لتجنب parameter shadowing — حيث يستخدم المستخدم `هذا.حقل`
                //      للوصول للحقل و `معامل` للوصول لقيمة المعامل
                // (EN) Add object fields to scope for direct access convenience
                //      Note: Skip fields whose names conflict with constructor params
                //      to avoid parameter shadowing — user accesses field via `this.field`
                //      and parameter value directly via `param`
                // ═══════════════════════════════════════════════════════════════
                std::set<std::string> constructorParamNames;
                for (const auto &param : constructor->parameters)
                {
                    constructorParamNames.insert(param.name);
                }
                for (const auto &[name, value] : objectInstance->fields)
                {
                    // (AR) تخطي الحقول المتعارضة مع معاملات الباني
                    // (EN) Skip fields that conflict with constructor params
                    if (constructorParamNames.count(name) == 0)
                    {
                        variableManager_.define(name, value);
                    }
                }

                // (AR) الحقول الثابتة لا تُضاف للنطاق المحلي — يُوصل إليها عبر اسم_الصنف.حقل فقط
                // (EN) Static fields are NOT added to local scope — accessed via ClassName.field only

                // ═══════════════════════════════════════════════════════════════
                // (AR) استدعاء باني الأب إذا كانت هناك معاملات
                // (EN) Call base constructor if superArgs exist
                // ═══════════════════════════════════════════════════════════════
                // (AR) إصلاح: دعم الوراثة متعددة المستويات بالتسلسل المتكرر
                //      مثال: قط_فارسي.superArgs → يستدعي باني قط
                //      → قط.superArgs → يستدعي باني حيوان (تسلسل تلقائي)
                // (EN) Fix: Support multi-level inheritance with recursive chaining
                // ═══════════════════════════════════════════════════════════════
                if ((!constructor->superArgs.empty() || constructor->hasBaseCall) &&
                    classType->getBaseClass())
                {
                    // (AR) دالة مساعدة لتنفيذ سلسلة البانين المتكررة
                    // (EN) Helper to execute recursive constructor chain
                    // (AR) تقييمُ وسائطِ `الأساس(...)` مع فرزِ المسمّى عن المكانيّ — نظيرَ ما
                    //      يفعلُه المسارُ المباشرُ (السطر ~٢٩٩). وبدونِه يُقيَّمُ الوسيطُ المسمّى
                    //      كأنّه مكانيٌّ فيُربَطُ **بالمعاملِ الخطأ صامتًا**: `الأساس(وسم: ق)`
                    //      لبانٍ `(مِعرّف، وسم)` كان يضعُ قيمةَ `ق` في `مِعرّف`.
                    // (EN) Evaluate `super(...)` arguments, separating named from positional — as
                    //      the direct path does (line ~299). Without it a named argument is
                    //      evaluated as if positional and binds to the **wrong parameter,
                    //      silently**: `super(tag: q)` against `(id, tag)` put q's value in `id`.
                    auto evaluateSuperArgs =
                        [&](const AST::ExprList &superArgExprs,
                            std::vector<Value> &positionalOut,
                            std::unordered_map<std::string, Value> &namedOut)
                    {
                        for (const auto &superArg : superArgExprs)
                        {
                            if (auto *namedArg = dynamic_cast<AST::NamedArgExpr *>(superArg.get()))
                            {
                                namedArg->value->accept(*this);
                                namedOut[namedArg->name] = lastResult_;
                            }
                            else
                            {
                                superArg->accept(*this);
                                positionalOut.push_back(lastResult_);
                            }
                        }
                    };

                    // (AR) مِكدَسُ نطاقاتِ المعامِلاتِ عبرَ السلسلة: طبقةٌ لكلِّ مستوًى، أدناها
                    //      البانِي المباشرُ وأعلاها الأبُ الجاري تنفيذُه. والحجبُ في المادّةِ (١)
                    //      حكمُ **مستوًى** لا حكمُ السلسلة: معامِلُ المشتقِّ يحجبُ الحقلَ في جسمِ
                    //      المشتقِّ وحدَه؛ أمّا جسمُ الأبِ فالاسمُ العاري فيه يدلُّ على الحقل.
                    //      ولذلكَ لا يكفي عدَّادٌ يقولُ «حيٌّ في مكانٍ ما» — يلزمُ معرفةُ **أيِّ**
                    //      مستوًى يملكُ الاسمَ معامِلًا، وإلّا استعادَ الخروجُ قيمةً بائتةً محلَّ
                    //      ما كتبَه الأبُ لتوِّه، فيقرأُ المستوى الوسيطُ غيرَ ما في الكائن.
                    // (EN) Stack of per-level parameter scopes: one layer per level, bottom being
                    //      the direct constructor and top the parent currently running. Article (1)
                    //      shadowing is a PER-LEVEL rule, not a chain-wide one: a derived parameter
                    //      shadows the field inside the derived body only; in the parent's body a
                    //      bare name denotes the field. A counter saying "live somewhere" is
                    //      therefore not enough — we must know WHICH level owns the name as a
                    //      parameter, otherwise the exit restore puts a stale value back over what
                    //      the parent has just written, and the intermediate level reads something
                    //      other than what the object holds.
                    std::vector<std::set<std::string>> chainParamScopes;
                    chainParamScopes.push_back(constructorParamNames);

                    std::function<void(AST::ConstructorDecl *, ClassType *, const std::vector<Value> &,
                                       const std::unordered_map<std::string, Value> &)>
                        executeCtorChain;
                    executeCtorChain = [&](AST::ConstructorDecl *ctor, ClassType *cls,
                                           const std::vector<Value> &args,
                                           const std::unordered_map<std::string, Value> &namedArgsForCtor)
                    {
                        // (AR) معامِلاتُ **هذا** المستوى تحجبُ الحقولَ المُسمّاةَ بأسمائِها،
                        //      كما يفعلُ المسارُ المباشرُ تمامًا. والنطاقُ مسطَّحٌ يجمعُ
                        //      المعامِلاتِ والحقول، فيلزمُ أمران: ألّا تُكتَبَ هذه الأسماءُ
                        //      راجعةً إلى الحقول (وإلّا دهسَ المعامِلُ حقلَه)، وأن تُردَّ إلى
                        //      حالتِها السابقةِ عندَ انتهاءِ المستوى (وإلّا تسرّبَت إلى ما بعدَه).
                        //      وبهذه القاعدةِ الواحدةِ يسقطُ كلُّ ما كان: تبطينٌ يُوسَمُ، واستثناءٌ
                        //      يُشترَط، واستردادٌ يستشيرُ الحقول — فالعدمُ لا يبلغُ الحقلَ أصلًا.
                        // (EN) THIS level's parameters shadow same-named fields, exactly as the
                        //      direct path does. The scope is flat, so two things follow: these
                        //      names must not be written back into fields (else a parameter
                        //      clobbers its field), and they must be restored when the level ends
                        //      (else they leak past it). This one rule replaces all of the former
                        //      machinery — tagged padding, a conditional exclusion, a restore that
                        //      consulted the fields — because null never reaches a field at all.
                        std::set<std::string> ctorParamNames;
                        std::unordered_map<std::string, Value> outerBeforeParams;
                        std::set<std::string> introducedByParams;

                        for (const auto &param : ctor->parameters)
                        {
                            ctorParamNames.insert(param.name);

                            if (variableManager_.exists(param.name))
                            {
                                outerBeforeParams[param.name] = variableManager_.get(param.name);
                            }
                            else
                            {
                                // (AR) اسمٌ لم يكنْ معرَّفًا: يُرفَعُ عندَ الانتهاءِ لا يُستردّ،
                                //      وإلّا انقلبَ «غيرُ معرَّف» إلى قيمةٍ فأخفى خطأً.
                                // (EN) A name that did not exist is lifted at the end, not
                                //      restored; otherwise "undefined" becomes a value and hides
                                //      an error.
                                introducedByParams.insert(param.name);
                            }
                        }

                        // (AR) النطاقُ مسطَّحٌ، فمعامِلاتُ المستوياتِ الأدنى حيّةٌ فيه وقتَ تشغيلِ
                        //      هذا المستوى. وهي **ليست** معامِلاتٍ لهذا البانِي، فالاسمُ العاري
                        //      في جسمِه يدلُّ على الحقلِ لا عليها (المادّتانِ ١ و٢). فيُعادُ ربطُ
                        //      كلِّ اسمٍ منها إلى قيمةِ الحقلِ الحاليّةِ قبلَ تشغيلِ الجسم، وتُردُّ
                        //      قيمتُه المعامِليّةُ عندَ الخروج.
                        //
                        //      وبهذا يُحلُّ الطرفانِ معًا بلا تحكّم: جسمُ الأبِ يقرأُ ويكتبُ حقلَه
                        //      كأنّ المشتقَّ لم يُسمِّ معامِلَه باسمِه (فلا تُبتلَعُ كتابةٌ مشروعة)،
                        //      والمشتقُّ بعدَ «الأساس(...)» يقرأُ معامِلَه هو لا ما كتبَه الأب.
                        //      وحدَه هذا يُغني عن تخمينِ «هل كتبَ الأبُ؟» بمقارنةِ القيم — وذاك
                        //      استدلالٌ يُخفِقُ حينَ يكتبُ الأبُ القيمةَ نفسَها.
                        // (EN) The scope is flat, so lower levels' parameters are live in it while
                        //      this level runs. They are **not** this constructor's parameters, so
                        //      a bare name in its body denotes the field, not them (Articles 1 and
                        //      2). Each such name is therefore rebound to the field's current value
                        //      before the body runs, and its parameter value restored on exit.
                        //
                        //      This settles both ends with no heuristic: the parent's body reads
                        //      and writes its own field as though the derived class had never named
                        //      a parameter after it (so no legitimate write is swallowed), and the
                        //      derived constructor after `الأساس(...)` reads its own parameter, not
                        //      what the parent wrote. It is what makes guessing "did the parent
                        //      write?" by comparing values unnecessary — and that guess fails
                        //      precisely when the parent writes the same value.
                        std::unordered_map<std::string, Value> lowerActiveParamValues;
                        for (const auto &lowerScope : chainParamScopes)
                        {
                            for (const auto &activeName : lowerScope)
                            {
                                if (ctorParamNames.count(activeName) != 0 ||
                                    lowerActiveParamValues.count(activeName) != 0 ||
                                    !variableManager_.exists(activeName))
                                    continue;

                                // (AR) ولا يُمَسُّ اسمٌ ليسَ حقلًا في هذا الكائنِ أصلًا: تسميةُ
                                //      معامِلٍ في مشتقٍّ لا تُخوِّلُنا حجبَ متغيّرٍ عامٍّ يكتبُه
                                //      جسمُ الأبِ — فحفظُه ثمّ استعادتُه يُلغي كتابتَه صامتًا.
                                // (EN) A name that is not a field of this object is left alone: a
                                //      derived class naming a parameter after a global does not
                                //      license shadowing it — saving and restoring it would undo
                                //      the parent body's write to that global, silently.
                                auto fieldIt = objectInstance->fields.find(activeName);
                                if (fieldIt == objectInstance->fields.end())
                                    continue;

                                lowerActiveParamValues[activeName] = variableManager_.get(activeName);
                                variableManager_.define(activeName, fieldIt->second);
                            }
                        }

                        chainParamScopes.push_back(ctorParamNames);

                        // (AR) ربط معاملات هذا الباني — موضعيًّا ثمّ **عدمٌ** لما لم يُمرَّر.
                        //      كان الشرطُ `args.size() == parameters.size()` أي «الكلُّ أو
                        //      لا شيء»: فباستدعاءٍ ناقصٍ لا يُربَطُ معاملٌ **واحد**، فيقرأُ
                        //      جسمُ الأبِ اسمًا غيرَ معرَّفٍ أو حقلًا يُظلّلُه. وهو نظيرُ ما
                        //      أُصلح في مسارِ الباني المباشر (أولويّة ٤ أدناه)، فيتّحدُ
                        //      المساران: `الأساس(مِعرّف)` لبانٍ يُصرّحُ معاملَين ⇒ الثاني عدمٌ.
                        //
                        //      و`define` لا `defineOrAssign`: الثانيةُ تمشي سلسلةَ النطاقاتِ
                        //      **صعودًا** وتُسنِدُ حيثُ وجدَت الاسم، فمعاملٌ يوافقُ اسمَ متغيّرٍ
                        //      عامٍّ كان يمحوه في نطاقِه هو. مقيسًا: `متغير وسم = 7` عامًّا ثمّ
                        //      بانٍ يُصرّحُ معاملًا اسمُه `وسم` ⇒ العامُّ يصيرُ عدمًا.
                        //
                        //      والسلّمُ هو سلّمُ المسارِ المباشرِ نفسُه — مكانيٌّ ← مسمًّى ←
                        //      افتراضيٌّ ← عدمٌ — لا مكانيٌّ ← عدمٌ، وإلّا لَتباعدَ المساران في
                        //      بانٍ يُصرّحُ `(مِعرّف، وسم = 42)`: `42` مباشرةً و`عدمٌ` عبرَ السلسلة.
                        // (EN) Bind this constructor's parameters, then **null** for whatever was
                        //      not passed. The condition was `args.size() == parameters.size()` —
                        //      all-or-nothing: on a short call not a single parameter was bound, so
                        //      the parent's body read an undefined name or a field it should have
                        //      shadowed.
                        //      `define`, not `defineOrAssign`: the latter walks the scope chain
                        //      **upward** and assigns wherever it finds the name, so a parameter
                        //      matching a global variable erased that global in its own scope.
                        //      The ladder is the direct path's own — positional → named → default →
                        //      null — not positional → null, which would diverge for a constructor
                        //      declaring `(id, tag = 42)`: 42 directly, null through the chain.
                        for (size_t i = 0; i < ctor->parameters.size(); ++i)
                        {
                            const auto &pname = ctor->parameters[i].name;
                            const auto namedIt = namedArgsForCtor.find(pname);

                            if (i < args.size())
                            {
                                variableManager_.define(pname, args[i]);
                            }
                            else if (namedIt != namedArgsForCtor.end())
                            {
                                variableManager_.define(pname, namedIt->second);
                            }
                            else if (ctor->parameters[i].defaultValue)
                            {
                                ctor->parameters[i].defaultValue->accept(*this);
                                variableManager_.define(pname, lastResult_);
                            }
                            else
                            {
                                variableManager_.define(pname, Value::makeNull());
                            }
                        }

                        // (AR) أولاً: إذا كان لهذا الباني superArgs خاصة به، نستدعي الجد أولاً
                        // (EN) First: if this constructor has its own superArgs, call grandparent first
                        if ((!ctor->superArgs.empty() || ctor->hasBaseCall) && cls->getBaseClass())
                        {
                            ClassType *parentClass = cls->getBaseClass();
                            if (parentClass->constructor)
                            {
                                AST::ConstructorDecl *parentCtor = parentClass->constructor.get();
                                // (AR) تقييم superArgs لهذا الباني
                                // (EN) Evaluate this constructor's superArgs
                                std::vector<Value> parentArgs;
                                std::unordered_map<std::string, Value> parentNamedArgs;
                                evaluateSuperArgs(ctor->superArgs, parentArgs, parentNamedArgs);
                                // (AR) استدعاء متكرر لباني الأب
                                // (EN) Recursive call to parent constructor
                                executeCtorChain(parentCtor, parentClass, parentArgs, parentNamedArgs);
                            }
                        }

                        // (AR) ثانياً: تنفيذ جسم هذا الباني
                        // (EN) Second: execute this constructor's body
                        try
                        {
                            ctor->body->accept(statementExecutor_);
                        }
                        catch (const std::exception &)
                        {
                            // (AR) خطأ في الباني — إعادة رفعه
                            // (EN) Constructor error — re-throw
                            throw;
                        }
                        catch (...)
                        {
                            throw Interpreter::UserThrownException(
                                "(AR) خطأ غير معروف في تنفيذ الباني. "
                                "(EN) Unknown error executing constructor.",
                                "RuntimeError", node.position);
                        }

                        // (AR) ثالثًا: كتابةُ النطاقِ راجعةً إلى الحقول — عدا أسماءِ
                        //      معامِلاتِ هذا المستوى، فالنطاقُ يحملُ عندَها قيمةَ **معامِلٍ**
                        //      لا قيمةَ حقل. والحقلُ المحجوبُ يُكتَبُ بـ`هذا.حقل`، وتلك تكتبُ
                        //      في الكائنِ مباشرةً فلا تمرُّ بهذه الحلقة. وهو حرفيًّا شرطُ
                        //      المسارِ المباشرِ نفسُه، فلا يتباعدُ المساران.
                        // (EN) Third: write the scope back into the fields — except this level's
                        //      parameter names, where the scope holds a PARAMETER, not a field. A
                        //      shadowed field is written via `this.field`, which writes the object
                        //      directly and never passes through this loop. This is verbatim the
                        //      direct path's condition, so the two paths cannot diverge.
                        for (const auto &field : cls->fields)
                        {
                            if (!field.isStatic && ctorParamNames.count(field.name) == 0)
                            {
                                if (variableManager_.exists(field.name))
                                {
                                    objectInstance->setField(field.name, variableManager_.get(field.name));
                                }
                                // (AR) الحقل لم يُعيَّن في الباني — متوقع
                                // (EN) Field not set in constructor — expected
                            }
                        }

                        // (AR) رابعًا: انتهى مستوى هذا الباني، فتُردُّ أسماءُ معامِلاتِه إلى
                        //      حالتِها السابقةِ في النطاقِ المسطَّح — ما كان معرَّفًا يُستردُّ
                        //      بقيمتِه، وما استحدثَه الربطُ يُرفَع. فلا يقرأُ المستوى الأدنى
                        //      معامِلَ الأعلى، ولا يبقى اسمٌ معرَّفًا بعدَ أن لم يكن.
                        // (EN) Fourth: this constructor's level is over, so its parameter names are
                        //      returned to their prior state in the flat scope — those that existed
                        //      are restored to their value, those the binding introduced are
                        //      lifted. No lower level reads an upper level's parameter, and no name
                        //      stays defined that was not defined before.
                        for (const auto &[outerName, outerValue] : outerBeforeParams)
                        {
                            variableManager_.define(outerName, outerValue);
                        }

                        for (const auto &introducedName : introducedByParams)
                        {
                            variableManager_.remove(introducedName);
                        }

                        chainParamScopes.pop_back();
                        const std::set<std::string> &enclosingParams = chainParamScopes.back();

                        // (AR) والاستعادةُ تسألُ المستوى الذي نعودُ إليه وحدَه: إن كانَ الاسمُ
                        //      معامِلَه فقيمتُه المحفوظةُ هي الصواب (المادّةُ ١)؛ وإلّا فالاسمُ
                        //      عندَه حقلٌ، فيُستعادُ من **الكائنِ** لا من النسخةِ المأخوذةِ عندَ
                        //      الدخول — وإلّا ضاعَ ما كتبَه جسمُ هذا المستوى لتوِّه.
                        // (EN) The restore asks only the level we are returning to: if the name is
                        //      its parameter, the saved value is right (Article 1); otherwise the
                        //      name is a field there, so it is restored from the OBJECT, not from
                        //      the copy taken on entry — else this level's own write is lost.
                        for (const auto &[lowerName, lowerValue] : lowerActiveParamValues)
                        {
                            if (enclosingParams.count(lowerName) != 0)
                            {
                                variableManager_.define(lowerName, lowerValue);
                                continue;
                            }

                            auto fieldIt = objectInstance->fields.find(lowerName);
                            variableManager_.define(
                                lowerName,
                                fieldIt != objectInstance->fields.end() ? fieldIt->second : lowerValue);
                        }
                    };

                    ClassType *baseClass = classType->getBaseClass();
                    if (baseClass->constructor)
                    {
                        AST::ConstructorDecl *baseCtor = baseClass->constructor.get();
                        // (AR) تقييم superArgs الأصلية
                        // (EN) Evaluate original superArgs
                        std::vector<Value> superArgValues;
                        std::unordered_map<std::string, Value> superNamedArgValues;
                        evaluateSuperArgs(constructor->superArgs, superArgValues, superNamedArgValues);
                        // (AR) بدء سلسلة التنفيذ المتكرر
                        // (EN) Start recursive execution chain
                        executeCtorChain(baseCtor, baseClass, superArgValues, superNamedArgValues);

                        // (AR) إنعاشُ نسخِ الحقولِ في هذا النطاقِ بعدَ انتهاءِ السلسلة: النسخُ
                        //      أُخِذَت قبلَ تشغيلِ البُناةِ الأعلى، فما كتبَه أبٌ بـ`هذا.حقل`
                        //      يبقى في الكائنِ ولا يظهرُ فيها. ثمّ تدهسُه الكتابةُ الراجعةُ عندَ
                        //      هذا المستوى بالنسخةِ البائتة — صامتًا. وهي العلّةُ نفسُها في
                        //      وجهِها الثاني: نسخةٌ في النطاقِ تُعامَلُ مصدرًا للحقيقةِ وليست به.
                        // (EN) Refresh this scope's field copies after the chain: the copies were
                        //      taken before the parent constructors ran, so whatever a parent wrote
                        //      via `this.field` lives in the object and not in them — and this
                        //      level's write-back then clobbers it with the stale copy, silently.
                        //      Same disease, second face: a scope copy treated as the truth.
                        for (const auto &[fieldName, fieldValue] : objectInstance->fields)
                        {
                            if (constructorParamNames.count(fieldName) == 0)
                            {
                                variableManager_.define(fieldName, fieldValue);
                            }
                        }
                    }
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) إصلاح: حفظ لقطة من قيم الحقول قبل تنفيذ الباني
                //      لتجنب استبدال التعديلات التي أجرتها دوال مستدعاة من الباني
                //      مثال: إذا استدعى الباني هذا._تهيئة() التي تعدل هذا.حقل = قيمة،
                //      يجب الحفاظ على هذا التعديل وعدم استبداله بالقيمة القديمة في النطاق
                //
                // (EN) Fix: Save field value snapshot before constructor execution
                //      to avoid overwriting modifications made by methods called from constructor
                //      Example: if constructor calls this._init() which sets this.field = value,
                //      we must preserve that modification and not overwrite with stale scope value
                // ═══════════════════════════════════════════════════════════════
                std::unordered_map<std::string, Value> preCtorFieldSnapshot;
                for (const auto &field : allFields)
                {
                    if (!field.isStatic)
                    {
                        preCtorFieldSnapshot[field.name] = objectInstance->fields[field.name];
                    }
                }

                // (AR) تنفيذ جسم الباني
                // (EN) Execute constructor body
                try
                {
                    constructor->body->accept(statementExecutor_);

                    // (AR) جمع القيم المحدثة من الـ scope إلى حقول الكائن
                    // (EN) Collect updated values from scope into object fields
                    for (const auto &field : allFields)
                    {
                        // (AR) تخطي الحقول الثابتة — تُدار مباشرة عبر ClassType::setStaticField
                        // (EN) Skip static fields — managed directly via ClassType::setStaticField
                        if (field.isStatic)
                            continue;

                        // (AR) حقلٌ يحملُ اسمَ معامِلٍ لا يُكتَبُ راجعًا من النطاق: النطاقُ
                        //      لا يحملُ عندَ ذلك الاسمِ قيمةَ **حقلٍ** بل قيمةَ **معامِل** —
                        //      إذ استُثنيَ الحقلُ من ملءِ النطاقِ أعلاه إقامةً للحجب. وقراءتُه
                        //      هنا كانت تنقضُ ذاك الحجبَ في الحلقةِ نفسِها، فيدهسُ المعامِلُ
                        //      الحقلَ بلا إسنادٍ أصلًا (`س(5)` ⇒ الحقلُ ٥ لا ١)، ووسيطٌ مُغفَلٌ
                        //      يُبيدُ التهيئةَ بعدمٍ — صامتًا في الحالَين، ومتباعِدًا عن المصرِّفِ
                        //      الذي يُبقي الحجبَ قائمًا. والحقلُ المحجوبُ يُكتَبُ بـ`هذا.حقل`،
                        //      وتلك تكتبُ في الكائنِ مباشرةً فلا تمرُّ بهذه الحلقة.
                        // (EN) A field sharing a parameter's name is not written back from the
                        //      scope: at that name the scope holds a PARAMETER, not a field —
                        //      the field was deliberately excluded from scope population above to
                        //      establish shadowing. Reading it here undid that shadowing in the
                        //      same loop, so a parameter clobbered its field with no assignment at
                        //      all, and an omitted argument wiped the initialiser with null —
                        //      silently, and divergent from the compiler. A shadowed field is
                        //      written via `this.field`, which writes the object directly.
                        if (constructorParamNames.count(field.name) != 0)
                            continue;

                        try
                        {
                            Value scopeValue = variableManager_.get(field.name);

                            // ═══════════════════════════════════════════════════════
                            // (AR) إصلاح: تحقق مما إذا كانت قيمة النطاق لم تتغير عن القيمة الأصلية
                            //      ولكن الحقل تم تعديله مباشرة عبر دالة مستدعاة من الباني
                            //      (عبر هذا.حقل = قيمة الذي يكتب مباشرة في objPtr->fields)
                            //      في هذه الحالة، نحتفظ بتعديل الدالة ولا نستبدله
                            //
                            // (EN) Fix: Check if scope value is unchanged from initial but the field
                            //      was modified directly by a method called from constructor
                            //      (via this.field = value which writes directly to objPtr->fields)
                            //      In that case, keep the method's modification
                            // ═══════════════════════════════════════════════════════
                            auto snapIt = preCtorFieldSnapshot.find(field.name);
                            if (snapIt != preCtorFieldSnapshot.end())
                            {
                                bool scopeMatchesInitial = (scopeValue == snapIt->second).toBool();
                                if (scopeMatchesInitial)
                                {
                                    // (AR) قيمة النطاق لم تتغير — تحقق إذا تم تعديل الحقل خارجياً
                                    // (EN) Scope value unchanged — check if field was modified externally
                                    const Value &currentFieldValue = objectInstance->fields[field.name];
                                    bool fieldModifiedExternally = !(currentFieldValue == snapIt->second).toBool();
                                    if (fieldModifiedExternally)
                                    {
                                        // (AR) الحقل عُدّل بواسطة دالة مستدعاة — نحتفظ بالتعديل
                                        // (EN) Field was modified by a called method — keep the modification
                                        continue;
                                    }
                                }
                            }

                            // (AR) تحديث حقل الكائن الحقيقي
                            // (EN) Update real object field
                            objectInstance->setField(field.name, scopeValue);
                        }
                        catch (const std::exception &)
                        {
                            // (AR) الحقل لم يتم تعيينه في الباني — متوقع
                            // (EN) Field was not set in constructor — expected
                        }
                    }

                    // (AR) الكتابة الراجعة للحقول الديناميكية التي أُنشئت في الباني
                    //      (حقول أُضيفت عبر هذا.حقل = قيمة بدون إعلان مسبق)
                    // (EN) Writeback dynamic fields created in constructor
                    //      (fields added via this.field = value without prior declaration)
                    // Note: objectInstance->fields may contain dynamically-set fields
                    // that are not in allFields, so we don't need an additional loop here
                    // since visitMemberAssignExpr already wrote them directly to objPtr->fields
                }
                catch (const std::exception &)
                {
                    variableManager_.exitScope();
                    throw;
                }

                variableManager_.exitScope();
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) وضع العلامة أن الباني اُستدعي → إرجاع الكائن كقيمة OBJECT
            // (EN) Mark constructed → Return object as OBJECT value
            // ═══════════════════════════════════════════════════════════════════
            objectInstance->markConstructed();

            // ═══════════════════════════════════════════════════════════════════
            // (AR) ربط النظام التفاعلي — إذا كان المكون واجهة مستخدم
            //      يُسجَّل في UIStateManager لمراقبة تغييرات حقول @حالة
            //      وإطلاق إعادة بناء تلقائية للشجرة
            //
            // (EN) Reactive system hookup — if this is a UI component,
            //      register with UIStateManager to watch @state field changes
            //      and trigger automatic tree rebuild
            // ═══════════════════════════════════════════════════════════════════
            if (classType && classType->isUIComponent && !classType->uiStateFields.empty())
            {
                auto &stateMgr = UIStateManager::instance();
                stateMgr.registerComponent(
                    objectInstance,
                    effectiveClassName,
                    classType->uiStateFields);
            }

            lastResult_ = Value(objectInstance);
        }

    } // namespace Interpreter
} // namespace Sad
