/**
 * @file statement_executor_oop_struct_test.cpp
 * @brief (AR) تنفيذ struct/test/type-alias/tuple-destructure/ui declarations في StatementExecutor
 * @brief (EN) struct/test/type-alias/tuple-destructure/ui declaration execution in StatementExecutor
 */

#include "statement_executor.h"
#include "class_manager.h"
#include "object_manager.h"
#include <ctime>
#include "property_nodes.h"
#include "declarations.h" // For OperatorDecl
#include "class_nodes.h"  // For ClassDeclStmt
#include "runtime_throw.h"
#include "user_thrown.h"
#include "ui_nodes.h"     // For UIDeclarationNode
#include <iostream>

namespace Sad
{
    namespace Interpreter
    {

        using namespace AST;
        using namespace Data;

        // ======================================================================
        // (AR) تنفيذ تصريح صنف / (EN) Execute Class Declaration
        // ======================================================================

        void StatementExecutor::visitStructDecl(AST::StructDecl &node)
        {
            // (AR) تسجيل البنية كصنف مبسط في ClassManager
            // (EN) Register struct as simplified class in ClassManager
            auto *classManager = Data::ClassManager::getInstance();

            if (classManager->hasClass(node.name))
            {
                // (AR) إعادة تعريف — حذف القديم / (EN) Redefine — remove old
                // Just allow re-registration silently
            }

            auto classType = std::make_unique<Data::ClassType>(node.name);

            // (AR) تسجيل الحقول / (EN) Register fields
            for (auto &field : node.fields)
            {
                Data::Value defaultVal;
                if (field.defaultValue)
                {
                    defaultVal = evaluateExpression(*field.defaultValue);
                }

                classType->addField(field.name, nullptr, AST::Visibility::PUBLIC, false, defaultVal);
            }

            // ═══════════════════════════════════════════════════════════════════
            // (AR) البحث عن باني صريح في دوال البنية
            //      إذا وُجد باني صريح، يُسجل في ClassType (مثل الأصناف)
            //      وسيستخدمه visitNewExpr عند استدعاء 'جديد بنية(...)'.
            //      إذا لم يوجد باني، نُنشئ باني تلقائي (التوافق الخلفي).
            // (EN) Look for explicit constructor in struct methods
            //      If found, register in ClassType (like classes do)
            //      and visitNewExpr will use it for 'new struct(...)'.
            //      If not found, create auto-constructor (backward compat).
            // ═══════════════════════════════════════════════════════════════════
            bool hasExplicitConstructor = false;
            for (auto &method : node.methods)
            {
                if (dynamic_cast<AST::ConstructorDecl *>(method.get()))
                {
                    hasExplicitConstructor = true;
                    break;
                }
            }

            std::string structName = node.name;

            if (hasExplicitConstructor)
            {
                // (AR) معالجة الأعضاء: باني + دوال (مثل الأصناف)
                // (EN) Process members: constructor + methods (like classes)
                for (auto &method : node.methods)
                {
                    if (auto *ctorDecl = dynamic_cast<AST::ConstructorDecl *>(method.get()))
                    {
                        // (AR) نقل الباني إلى ClassType (مثل visitClassDecl)
                        // (EN) Transfer constructor to ClassType (like visitClassDecl)
                        classType->constructor.reset(ctorDecl);
                        method.release(); // (AR) منع الحذف المزدوج / (EN) Prevent double-free
                    }
                    else if (auto *methodDecl = dynamic_cast<AST::MethodDecl *>(method.get()))
                    {
                        // (AR) تسجيل الدالة في ClassType
                        // (EN) Register method in ClassType
                        AST::Visibility vis = AST::Visibility::PUBLIC;
                        switch (methodDecl->access)
                        {
                        case AST::AccessModifier::PUBLIC:
                            vis = AST::Visibility::PUBLIC;
                            break;
                        case AST::AccessModifier::PRIVATE:
                            vis = AST::Visibility::PRIVATE;
                            break;
                        case AST::AccessModifier::PROTECTED:
                            vis = AST::Visibility::PROTECTED;
                            break;
                        }
                        classType->addMethod(
                            methodDecl->name,
                            vis,
                            nullptr,
                            methodDecl->parameters,
                            methodDecl->body ? std::unique_ptr<AST::BlockStmt>(
                                                   dynamic_cast<AST::BlockStmt *>(methodDecl->body.release()))
                                             : nullptr,
                            methodDecl->isStatic,
                            false, false);
                    }
                }

                // (AR) تسجيل دالة مصنّعة باسم البنية لدعم 'بنية(...)' بدون 'جديد'
                //      هذه الدالة تُنشئ كائناً وتستدعي الباني يدوياً
                // (EN) Register factory function with struct name to support 'struct(...)' without 'new'
                //      This function creates an object and manually invokes the constructor
                auto &funcMgr = functionManager_;
                auto fieldsCopy = node.fields;
                auto *rawClassType = classType.get(); // (AR) مؤشر خام لاستخدامه في اللامدا

                funcMgr.registerBuiltinFunction(structName,
                                                [structName, fieldsCopy, rawClassType](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                                                {
                                                    // (AR) إنشاء نسخة جديدة وتهيئة الحقول بقيمها الافتراضية
                                                    // (EN) Create new instance and initialize fields with defaults
                                                    auto *classMgr = Data::ClassManager::getInstance();
                                                    auto *clsType = classMgr->getClass(structName);

                                                    Data::ObjectInstance *instance;
                                                    if (clsType)
                                                    {
                                                        auto *rawObj = clsType->createInstance();
                                                        instance = rawObj;
                                                    }
                                                    else
                                                    {
                                                        instance = new Data::ObjectInstance(nullptr, 0);
                                                    }

                                                    // (AR) تهيئة الحقول بقيمها الافتراضية
                                                    // (EN) Initialize fields with default values
                                                    if (clsType)
                                                    {
                                                        for (const auto &field : clsType->fields)
                                                        {
                                                            instance->fields[field.name] = field.defaultValue.clone();
                                                        }
                                                    }

                                                    // (AR) لا يمكن تنفيذ جسم الباني من لامدا مدمجة —
                                                    //      نعيّن الحقول من الوسائط بالترتيب (مثل الباني التلقائي)
                                                    //      المستخدم يجب أن يستخدم 'جديد' لتنفيذ الباني الكامل
                                                    // (EN) Cannot execute constructor body from builtin lambda —
                                                    //      assign fields from args by position (auto-constructor fallback)
                                                    //      User should use 'new' for full constructor execution
                                                    for (size_t i = 0; i < fieldsCopy.size(); ++i)
                                                    {
                                                        if (i < args.size() && args[i])
                                                        {
                                                            instance->fields[fieldsCopy[i].name] = *args[i];
                                                        }
                                                    }

                                                    return std::make_shared<Data::Value>(instance);
                                                });
            }
            else
            {
                // (AR) لا يوجد باني صريح — إنشاء باني تلقائي يقبل قيم الحقول
                // (EN) No explicit constructor — create automatic constructor accepting field values
                auto &funcMgr = functionManager_;
                auto fieldsCopy = node.fields;

                funcMgr.registerBuiltinFunction(structName,
                                                [structName, fieldsCopy](const std::vector<std::shared_ptr<Data::Value>> &args) -> std::shared_ptr<Data::Value>
                                                {
                                                    auto *classMgr = Data::ClassManager::getInstance();
                                                    auto *classType = classMgr->getClass(structName);

                                                    Data::ObjectInstance *instance;
                                                    if (classType)
                                                    {
                                                        auto *rawObj = classType->createInstance();
                                                        instance = rawObj;
                                                    }
                                                    else
                                                    {
                                                        instance = new Data::ObjectInstance(nullptr, 0);
                                                    }

                                                    for (size_t i = 0; i < fieldsCopy.size(); ++i)
                                                    {
                                                        if (i < args.size() && args[i])
                                                        {
                                                            instance->fields[fieldsCopy[i].name] = *args[i];
                                                        }
                                                        else
                                                        {
                                                            instance->fields[fieldsCopy[i].name] = Data::Value();
                                                        }
                                                    }

                                                    return std::make_shared<Data::Value>(instance);
                                                });

                // (AR) تسجيل دوال البنية إن وجدت (بدون باني)
                // (EN) Register struct methods if any (no constructor)
                for (auto &method : node.methods)
                {
                    if (auto *methodDecl = dynamic_cast<AST::MethodDecl *>(method.get()))
                    {
                        AST::Visibility vis = AST::Visibility::PUBLIC;
                        switch (methodDecl->access)
                        {
                        case AST::AccessModifier::PUBLIC:
                            vis = AST::Visibility::PUBLIC;
                            break;
                        case AST::AccessModifier::PRIVATE:
                            vis = AST::Visibility::PRIVATE;
                            break;
                        case AST::AccessModifier::PROTECTED:
                            vis = AST::Visibility::PROTECTED;
                            break;
                        }
                        classType->addMethod(
                            methodDecl->name,
                            vis,
                            nullptr,
                            methodDecl->parameters,
                            methodDecl->body ? std::unique_ptr<AST::BlockStmt>(
                                                   dynamic_cast<AST::BlockStmt *>(methodDecl->body.release()))
                                             : nullptr,
                            methodDecl->isStatic,
                            false, false);
                    }
                    else if (method)
                    {
                        method->accept(*this);
                    }
                }
            }

            classManager->registerClass(std::move(classType));
        }

        // ═══════════════════════════════════════════════════════════════════════════
        // (AR) تنفيذ اختبار — يشغل كتلة الاختبار ويطبع النتيجة
        // (EN) Test execution — runs test block and prints result
        // ═══════════════════════════════════════════════════════════════════════════
        void StatementExecutor::visitTestDecl(AST::TestDecl &node)
        {
            // (AR) طباعة بداية الاختبار
            std::string testName = node.testName;

            // (AR) اختبار الخصائص (Property Testing) — تكرار الجسم N مرة
            // (EN) Property Testing — repeat body N times
            if (node.isPropertyTest && node.iterations > 0)
            {
                int passed = 0;
                int failed = 0;

                for (int i = 0; i < node.iterations; i++)
                {
                    try
                    {
                        variableManager_.enterScope(Data::ScopeType::BLOCK, "property_test:" + testName);

                        // (AR) تعريف متغيرات خاصة بالتكرار
                        // (EN) Define iteration-specific variables
                        variableManager_.define("_تكرار", Data::Value(static_cast<double>(i)));
                        variableManager_.define("_التكرار_الحالي", Data::Value(static_cast<double>(i + 1)));
                        variableManager_.define("_إجمالي_التكرارات", Data::Value(static_cast<double>(node.iterations)));

                        // (AR) بذرة عشوائية: seed + i لقيم مختلفة كل تكرار
                        int currentSeed = (node.seed >= 0) ? (node.seed + i) : (static_cast<int>(std::time(nullptr)) + i);
                        variableManager_.define("_بذرة", Data::Value(static_cast<double>(currentSeed)));

                        // (AR) متغيرات عشوائية مشتقة من البذرة — LCG بسيط
                        // (EN) Random variables derived from seed — simple LCG
                        unsigned int rng = static_cast<unsigned int>(currentSeed);
                        rng = rng * 1664525u + 1013904223u;                              // LCG
                        double randVal = static_cast<double>(rng % 1000000) / 1000000.0; // 0.0 .. ~1.0
                        variableManager_.define("_عشوائي", Data::Value(randVal));
                        variableManager_.define("_عشوائي_صحيح", Data::Value(static_cast<double>(rng % 1000)));
                        variableManager_.define("_عشوائي_سالب", Data::Value(static_cast<double>(static_cast<int>(rng % 2001) - 1000)));

                        if (node.body)
                        {
                            node.body->accept(*this);
                        }

                        variableManager_.exitScope();
                        passed++;
                    }
                    catch (const std::exception &e)
                    {
                        variableManager_.exitScope();
                        failed++;

                        // (AR) طباعة أول فشل فقط
                        if (failed == 1)
                        {
                            std::cout << "  ⚠ فشل التكرار " << (i + 1) << ": " << e.what() << std::endl;
                        }
                    }
                    catch (...)
                    {
                        variableManager_.exitScope();
                        failed++;
                    }
                }

                // (AR) طباعة نتيجة اختبار الخصائص
                if (failed == 0)
                {
                    std::cout << "✓ " << testName << " (" << passed << "/" << node.iterations << " تكرار نجح)" << std::endl;
                }
                else
                {
                    std::cout << "✗ " << testName << " (" << failed << "/" << node.iterations << " تكرار فشل)" << std::endl;
                }
                return;
            }

            // (AR) اختبار عادي
            // (EN) Normal test
            try
            {
                // (AR) تنفيذ جسم الاختبار في نطاق جديد
                // (EN) Execute test body in new scope
                variableManager_.enterScope(Data::ScopeType::BLOCK, "test:" + testName);

                if (node.body)
                {
                    node.body->accept(*this);
                }

                variableManager_.exitScope();

                if (node.shouldFail)
                {
                    // (AR) كان متوقعاً أن يفشل لكنه نجح
                    std::cout << "✗ " << testName << " (كان متوقعاً أن يفشل)" << std::endl;
                }
                else
                {
                    std::cout << "✓ " << testName << std::endl;
                }
            }
            catch (const std::exception &e)
            {
                variableManager_.exitScope();

                if (node.shouldFail)
                {
                    std::cout << "✓ " << testName << " (فشل كما هو متوقع)" << std::endl;
                }
                else
                {
                    std::cout << "✗ " << testName << ": " << e.what() << std::endl;
                }
            }
            catch (...)
            {
                variableManager_.exitScope();

                if (node.shouldFail)
                {
                    std::cout << "✓ " << testName << " (فشل كما هو متوقع)" << std::endl;
                }
                else
                {
                    std::cout << "✗ " << testName << ": خطأ غير معروف" << std::endl;
                }
            }
        }

        // ======================================================================
        // (AR) تنفيذ تصريح اسم مستعار للنوع: نوع اسم = تعبير_هدف
        //      يُقيّم التعبير الهدف ويسجل الاسم المستعار كمتغير يشير لنفس القيمة.
        //      هذا يسمح بإنشاء أسماء بديلة للأصناف والتعدادات والأنواع الأخرى.
        //      مثال: نوع عدد = رقم  →  عدد يصبح اسماً بديلاً لـ رقم
        //      مثال: نوع مستخدم = شخص  →  مستخدم يشير لنفس الصنف شخص
        // (EN) Execute type alias declaration: type name = target_expression
        //      Evaluates target and registers alias name as a variable pointing to same value.
        // ======================================================================

        void StatementExecutor::visitTypeAliasDecl(AST::TypeAliasDecl &node)
        {
            // (AR) تقييم التعبير الهدف للحصول على القيمة (صنف، تعداد، دالة، نص نوع...)
            // (EN) Evaluate target expression to get value (class, enum, function, type string...)
            node.target->accept(*expressionEvaluator_);
            Data::Value targetValue = expressionEvaluator_->getResult();

            // (AR) تسجيل الاسم المستعار في مدير المتغيرات
            // (EN) Register alias name in variable manager
            variableManager_.defineOrAssign(node.name, targetValue);

            // (AR) إذا كان الهدف اسم صنف مسجل في ClassManager، نسجل اسماً مستعاراً أيضاً
            //      حتى يعمل 'جديد اسم_مستعار(...)' بشكل صحيح
            //      ندعم أيضاً الأسماء المستعارة المتسلسلة: نوع أ = صنف، نوع ب = أ
            // (EN) If target is a registered class name, also register alias in ClassManager
            //      so that 'new aliasName(...)' works correctly
            //      Also supports chained aliases: type A = Class, type B = A
            if (auto *varExpr = dynamic_cast<AST::VariableExpr *>(node.target.get()))
            {
                auto *classManager = Data::ClassManager::getInstance();
                if (classManager)
                {
                    // (AR) بحث مباشر عن الصنف أو عبر اسم مستعار موجود
                    // (EN) Direct class lookup or via existing alias
                    if (classManager->hasClass(varExpr->name))
                    {
                        // (AR) getClass يحل الأسماء المستعارة تلقائياً
                        //      لكن نحتاج الاسم الأصلي للصنف الفعلي
                        // (EN) getClass resolves aliases automatically
                        //      but we need the original class name
                        ClassType *resolved = classManager->getClass(varExpr->name);
                        if (resolved)
                        {
                            classManager->registerClassAlias(node.name, resolved->name);
                        }
                    }
                }
            }
        }

        // =====================================================================
        // (AR) تفكيك الصف — متغير (أ، ب، ج) = (1، 2، 3)
        // (EN) Tuple destructuring — var (a, b, c) = (1, 2, 3)
        // =====================================================================
        void StatementExecutor::visitTupleDestructureStmt(AST::TupleDestructureStmt &node)
        {
            // (AR) تقييم تعبير الصف / (EN) Evaluate the tuple expression
            node.initializer->accept(*expressionEvaluator_);
            Data::Value tupleValue = expressionEvaluator_->getResult();

            // (AR) التحقق من أن القيمة صف فعلاً / (EN) Verify the value is a tuple
            if (!tupleValue.isTuple())
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_TYPE_CHECK_FAILED,
                    node.position,
                    {{"actual", tupleValue.getTypeName()}, {"expected", "tuple"}});
            }

            const auto &elements = tupleValue.toTupleRef();

            // (AR) التحقق من تطابق عدد المتغيرات مع عدد عناصر الصف
            // (EN) Verify variable count matches tuple element count
            if (node.names.size() != elements.size())
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_TOO_MANY_ARGS,
                    node.position,
                    {{"function", "tuple destructure"},
                     {"expected", std::to_string(elements.size())},
                     {"actual", std::to_string(node.names.size())}});
            }

            // (AR) تعيين كل عنصر في الصف إلى المتغير المقابل
            // (EN) Assign each tuple element to the corresponding variable
            for (size_t i = 0; i < node.names.size(); ++i)
            {
                if (node.names[i] != "_")
                {
                    // (AR) تخطي المتغيرات بالاسم _ (عنصر نائب)
                    // (EN) Skip variables named _ (placeholder)
                    variableManager_.defineOrAssign(node.names[i], elements[i]);
                }
            }
        }

        // ======================================================================
        // (AR) تنفيذ تصريح واجهة / (EN) Execute UI Declaration
        // ======================================================================
        //
        // (AR) يسجل مكون الواجهة كصنف خاص في ClassManager مع علامة UI.
        //      يُعامل الواجهة كصنف يحتوي:
        //      - حقول @حالة كمتغيرات عضوية
        //      - دوال المكون (بما فيها بناء()) كطرق عادية
        //      - الوراثة تعمل كوراثة أصناف عادية
        //
        // (EN) Registers the UI component as a special class in ClassManager.
        //      Treats the واجهة as a class containing:
        //      - @state declarations as member fields
        //      - Component methods (including بناء()) as regular methods
        //      - Inheritance works like regular class inheritance
        // ======================================================================

        void StatementExecutor::visitUIDeclaration(AST::UIDeclarationNode &node)
        {
            auto *classManager = ClassManager::getInstance();

            // (AR) التحقق من عدم وجود مكون بنفس الاسم
            // (EN) Check for duplicates
            if (classManager->hasClass(node.name))
            {
                auto *existing = classManager->getClass(node.name);
                if (!existing->fields.empty() || !existing->methods.empty() || existing->constructor)
                {
                    return; // (AR) تخطي إعادة التعريف بصمت
                }
            }

            auto classType = std::make_unique<ClassType>(node.name);
            classType->isUIComponent = true;

            // (AR) معالجة الوراثة / (EN) Handle inheritance
            if (node.hasParent())
            {
                ClassType *parentClass = classManager->getClass(node.parentName);
                if (parentClass)
                {
                    classType->baseClass = parentClass;
                }
            }

            // (AR) تسجيل حقول @حالة كحقول عضوية / (EN) Register @state as member fields
            for (auto &stateDecl : node.stateDecls)
            {
                Value defaultValue;
                if (stateDecl->initializer)
                {
                    try
                    {
                        defaultValue = evaluateExpression(*stateDecl->initializer);
                    }
                    catch (...)
                    {
                        defaultValue = Value();
                    }
                }

                classType->addField(stateDecl->name, nullptr,
                                    AST::Visibility::PUBLIC, false, defaultValue);

                // (AR) تسجيل اسم الحقل كحالة تفاعلية
                // (EN) Register field name as reactive state
                classType->uiStateFields.insert(stateDecl->name);
            }

            // (AR) تسجيل الدوال / (EN) Register methods
            for (auto &method : node.methods)
            {
                if (auto *funcDecl = dynamic_cast<AST::FunctionDecl *>(method.get()))
                {
                    // (AR) تسجيل كطريقة عادية (بما فيها "بناء")
                    // (EN) Register as regular method (including "بناء")
                    auto bodyPtr = funcDecl->body
                                       ? std::unique_ptr<AST::BlockStmt>(
                                             dynamic_cast<AST::BlockStmt *>(funcDecl->body.release()))
                                       : nullptr;

                    classType->addMethod(
                        funcDecl->name,
                        AST::Visibility::PUBLIC,
                        nullptr,
                        funcDecl->parameters,
                        std::move(bodyPtr),
                        false,  // isStatic
                        false,  // isVirtual
                        false); // isAbstract
                }
            }

            // (AR) تسجيل المكون / (EN) Register component
            classManager->registerClass(std::move(classType));
        }

    } // namespace Interpreter
} // namespace Sad
