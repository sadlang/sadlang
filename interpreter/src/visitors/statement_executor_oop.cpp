/**
 * @file statement_executor_oop.cpp
 * @brief (AR) تنفيذ دوال OOP في منفذ الجمل
 *        (EN) OOP functions implementation in statement executor
 *
 * (AR) يحتوي على تنفيذ دوال تنفيذ الأصناف والبرمجة الكائنية
 * (EN) Contains implementation of class and OOP execution functions
 *
 * @author Sad Language Development Team
 * @date 2025-11-23
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
#include <unordered_set> // (AR) مزورات سلسلة baseClass — كشف دورة الوراثة

namespace Sad
{
    namespace Interpreter
    {

        using namespace AST;
        using namespace Data;

        // ======================================================================
        // (AR) تنفيذ تصريح صنف / (EN) Execute Class Declaration
        // ======================================================================

        void StatementExecutor::visitClassDecl(AST::ClassDecl &node)
        {
#ifdef DEBUG_OOP

            std::cout << "[OOP] تنفيذ تصريح صنف: " << node.name << "\n";
#endif

            // Get ClassManager instance
            auto *classManager = ClassManager::getInstance();

            // Check if class already exists (from execution, not parsing)
            // (AR) تحقق من وجود الصنف من التنفيذ السابق، وليس من التحليل
            // (EN) Check if class already exists from previous execution, not from parsing
            if (classManager->hasClass(node.name))
            {
                auto *existingClass = classManager->getClass(node.name);
                // If it's a temporary registration (empty class), we'll replace it
                // (AR) إذا كان تسجيلاً مؤقتاً (صنف فارغ)، سنستبدله
                if (!existingClass->fields.empty() || !existingClass->methods.empty() || existingClass->constructor)
                {
                    // (AR) الصنف موجود بالكامل — تخطيه بدلاً من رمي خطأ (يحدث مع استيراد * المتعدد)
                    // (EN) Class fully exists — skip silently instead of throwing (happens with multiple * imports)
#ifdef DEBUG_OOP
                    std::cout << "[OOP] تخطي إعادة تعريف الصنف: " << node.name << "\n";
#endif
                    return;
                }
#ifdef DEBUG_OOP

                std::cout << "[OOP] استبدال التسجيل المؤقت للصنف: " << node.name << "\n";
#endif
            }

            // Create new ClassType (simplified registration)
            // Full field/method registration will be done later when Type system is unified
            auto classType = std::make_unique<ClassType>(node.name);

            // (AR) تعيين علامة العقد الذكي / (EN) Set smart contract flag
            classType->isContract = node.isContract;

            // (AR) تعيين علامة الصنف المحكم ومسار الملف المصدري / (EN) Set sealed flag and source file
            // (AR) نستخدم currentFilePath_ من المفسر بدلاً من node.sourceFile لأن المحلل النحوي
            //      لا يعرف مسار الملف الفعلي — فقط المفسر يتتبع المسار الصحيح
            // (EN) Use currentFilePath_ from interpreter instead of node.sourceFile because
            //      the parser doesn't know the actual file path — only the interpreter tracks it
            classType->isSealed = node.isSealed;
            classType->sourceFile = currentFilePath_;

            // (AR) معالجة الوراثة المتعددة / (EN) Handle multiple inheritance
            if (!node.superclasses.empty())
            {
#ifdef DEBUG_OOP
                std::cout << "[OOP] الصنف '" << node.name << "' يرث من: ";
#endif

                // (AR) التحقق من وجود جميع الأصناف الأساسية / (EN) Verify all base classes exist
                std::vector<ClassType *> baseClasses;
                for (const auto &baseName : node.superclasses)
                {
                    ClassType *baseClass = classManager->getClass(baseName);
                    if (!baseClass)
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BASE_CLASS_NOT_FOUND,
                            node.position,
                            {{"base", baseName}, {"class", node.name}});
                    }
                    baseClasses.push_back(baseClass);
#ifdef DEBUG_OOP
                    std::cout << "'" << baseName << "' ";
#endif
                }
#ifdef DEBUG_OOP
                std::cout << "\n";
#endif

                // (AR) تعيين الصنف الأساسي الأول / (EN) Set first base class
                classType->baseClass = baseClasses[0];

                // ═══════════════════════════════════════════════════════════════
                // (AR) قواعد العقود الذكية: عقد لا يرث إلا من عقد آخر
                // (EN) Smart contract rules: contracts can only inherit from contracts
                // ═══════════════════════════════════════════════════════════════
                if (node.isContract)
                {
                    for (const auto *base : baseClasses)
                    {
                        if (!base->isContract)
                        {
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_INHERITANCE_CYCLE,
                                node.position,
                                {{"class", node.name}, {"base", base->name}});
                        }
                    }
                }

                // ═══════════════════════════════════════════════════════════════
                // (AR) قواعد الأصناف المحكمة (Sealed Classes):
                //      الصنف المحكم لا يمكن وراثته إلا من نفس الملف المصدري
                // (EN) Sealed class rules:
                //      Sealed classes can only be inherited from the same source file
                // ═══════════════════════════════════════════════════════════════
                for (const auto *base : baseClasses)
                {
                    if (base->isSealed && !base->sourceFile.empty())
                    {
                        // (AR) مقارنة مسار الملف الحالي (من المفسر) مع مسار الصنف المحكم
                        // (EN) Compare current file path (from interpreter) with sealed class source file
                        if (currentFilePath_ != base->sourceFile)
                        {
                            ::Sad::Errors::throwRuntime(
                                ::Sad::Errors::ErrorCode::RUN_PERMISSION_DENIED,
                                node.position,
                                {{"resource", "sealed class " + base->name}});
                        }
                    }
                }

                // (AR) تخزين الأصناف الأساسية الإضافية للوراثة المتعددة
                // (EN) Store additional base classes for multiple inheritance
                if (baseClasses.size() > 1)
                {
                    for (size_t i = 1; i < baseClasses.size(); ++i)
                    {
                        classType->additionalBases.push_back(baseClasses[i]);
                    }
#ifdef DEBUG_OOP
                    std::cout << "[OOP] الوراثة المتعددة: " << baseClasses.size()
                              << " أصناف أساسية مسجلة للصنف '" << node.name << "'\\n";
#endif
                }
            }

            // Count fields and methods for reporting
            int fieldCount = 0;
            int methodCount = 0;
            int propertyCount = 0;
            bool hasConstructor = false;
            bool hasDestructor = false;

            for (auto &member : node.members)
            {
                if (dynamic_cast<FieldDecl *>(member.get()))
                {
                    fieldCount++;
                }
                else if (dynamic_cast<MethodDecl *>(member.get()))
                {
                    methodCount++;
                }
                else if (dynamic_cast<AST::PropertyDecl *>(member.get()))
                {
                    propertyCount++;
                }
                else if (dynamic_cast<ConstructorDecl *>(member.get()))
                {
                    hasConstructor = true;
                }
                else if (dynamic_cast<DestructorDecl *>(member.get()))
                {
                    hasDestructor = true;
                }
            }

#ifdef DEBUG_OOP
            std::cout << "[OOP] الصنف يحتوي على: " << fieldCount << " حقل، "
                      << methodCount << " طريقة، " << propertyCount << " خاصية";
            if (hasConstructor)
                std::cout << "، باني";
            if (hasDestructor)
                std::cout << "، هادم";
            std::cout << "\n";
#endif

            // Register fields (simplified - without full type resolution for now)
            for (auto &member : node.members)
            {
                if (auto *fieldDecl = dynamic_cast<FieldDecl *>(member.get()))
                {
                    // Convert AccessModifier to Visibility
                    AST::Visibility vis = AST::Visibility::PUBLIC;
                    switch (fieldDecl->access)
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

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) إصلاح: تقييم القيمة الافتراضية لجميع الحقول (ثابتة وغير ثابتة)
                    // (EN) Fix: Evaluate default value for all fields (static and non-static)
                    // ═══════════════════════════════════════════════════════════════
                    Value defaultValue;

                    // (AR) تقييم تعبير المُهيئ إن وجد
                    // (EN) Evaluate initializer expression if present
                    // ═══════════════════════════════════════════════════════════════
                    // (AR) 🔑 مُهيّئٌ يُنشِئ كائنًا لا يُقيَّم هنا. التقييمُ في هذا
                    //      الموضعِ يقع مرّةً واحدةً **وقتَ تصريحِ الصنف**، فينتج عنه
                    //      عطبان قِيسا معًا: (أ) بانيَ الحقلِ يُنفَّذ ولو لم يُنشَأ من
                    //      الصنفِ كائنٌ قطّ (طُبِع «انشئ» قبل أوّلِ إنشاء)، (ب) القيمةُ
                    //      المُقيَّمةُ كائنٌ واحدٌ يُستنسَخ بـ`clone()` وهي نسخةُ سطحٍ
                    //      واحد، فكائنان من صنفٍ مركَّبٍ بمستويَين يتقاسمان الأعمق.
                    //      فيُؤجَّل تقييمُه إلى لحظةِ الإنشاءِ عبر `initializerExpr`.
                    //      ⚠️ ولا يُوسَّع الشرط: المُهيّئاتُ الأخرى (حرفيّةٌ وغيرُها)
                    //      تبقى على تقييمِها السابقِ حرفًا بحرف.
                    // (EN) An object-creating initializer must not be evaluated here:
                    //      that happens once at class-declaration time, which both ran
                    //      the field's constructor before any instantiation and shared
                    //      one object across instances (clone() is one level deep).
                    // ═══════════════════════════════════════════════════════════════
                    // (AR) ⚠️ الساكنُ يُستثنى: حلقةُ الإنشاءِ تتخطّى الحقولَ الساكنةَ،
                    //      فتأجيلُ مُهيّئِها يعني ألّا يُقيَّم أبدًا. قِيس (2026-08-15):
                    //      «متغير ساكن نسخة = شخص() جديد» يطبع «احمد» على الأصل
                    //      ويرفع RUN033 بالتأجيل — انحدارٌ صامتٌ أحدثه شرطٌ غيرُ
                    //      مشروطٍ بـisStatic. والساكنُ واحدٌ للصنفِ كلِّه فلا مشاركةَ
                    //      بين نُسَخٍ تُخشى أصلًا.
                    // (EN) Static fields are excluded: the instantiation loop skips
                    //      them, so deferring their initializer means never evaluating
                    //      it. A static field is shared by design, so the aliasing
                    //      concern that motivated the deferral does not apply.
                    std::string deferredConstructClass;
                    size_t deferredConstructLine = 0;
                    size_t deferredConstructColumn = 0;
                    AST::NewExpr *newObjectInitializer =
                        fieldDecl->isStatic
                            ? nullptr
                            : dynamic_cast<AST::NewExpr *>(
                                  fieldDecl->initializer ? fieldDecl->initializer.get() : nullptr);
                    if (newObjectInitializer)
                    {
                        deferredConstructClass = newObjectInitializer->className;
                        deferredConstructLine = newObjectInitializer->position.line;
                        deferredConstructColumn = newObjectInitializer->position.column;
                    }
                    else if (fieldDecl->initializer)
                    {
                        try
                        {
                            fieldDecl->initializer->accept(*expressionEvaluator_);
                            defaultValue = expressionEvaluator_->getResult();
                        }
                        catch (const std::exception &e)
                        {
                            // (AR) فشل التقييم — استخدم القيمة الافتراضية مع تحذير
                            // (EN) Evaluation failed — fall back to type default with warning
                            std::cerr << "[Warning] Field initializer evaluation failed: " << e.what() << std::endl;
                            defaultValue = Value();
                        }
                        catch (...)
                        {
                            std::cerr << "[Warning] Field initializer evaluation failed (unknown error)" << std::endl;
                            defaultValue = Value();
                        }
                    }
                    else
                    {
                        // (AR) لا يوجد مُهيئ — استخدم القيمة الافتراضية للنوع
                        // (EN) No initializer — use type default
                        // (AR) 🔑 من مصدرِ الحقيقةِ الواحدِ لا بنسخةٍ يدويّة. وقد كتبتُ
                        //      في `class_type.h` حرفًا: «فمن أراد قيمةً افتراضيّةً
                        //      فليطلبها من هنا، ولا ينسخِ الجدولَ مرّةً أخرى» — ثمّ
                        //      نسختُه في هذا الملفِّ مرّتَين في الرقعةِ نفسِها، وعدّلتُ
                        //      النسختَين يدويًّا بإضافةِ «بايت» و«طبيعي».
                        //      ونظيرُه في المترجّمِ نسي «طبيعي» فقرأ مكدّسًا غيرَ
                        //      مهيّأ — وهو بالضبطِ ما يحدث حين يُصان جدولٌ في أربعةِ
                        //      مواضعَ باليد. **عقدٌ نقضه كاتبُه لا يُصلِحه تشديدُ لفظِه.**
                        // (EN) From the single source of truth, not a hand copy. The
                        //      contract written in class_type.h ("ask here, never copy
                        //      the table again") was broken twice in this same patch.
                        defaultValue = Data::defaultValueForTypeKind(fieldDecl->type);
                    }

                    // (AR) تمرير القيمة الافتراضية عند إضافة الحقل
                    // (EN) Pass default value when adding field
                    // (AR) SEM045 (حقول الأصناف): التصنيفُ المُصرَّح يُدوَّن ليقرأه حارسُ
                    //      «الفراغُ لا يعبر إلى خانةٍ مصنَّفة» في إسناد الأعضاء —
                    //      `type` يُمرَّر nullptr أعلاه فلا طريقَ آخرَ للنوع.
                    //      ⚠️ الملءُ مشروطٌ بنجاح addField: عند فشلِه (اسمٌ مكرَّر)
                    //      يمشي findField سلسلةَ الوراثة فيطمسُ تصنيفَ حقلِ **الأب**
                    //      (رصدُ مراجعة الجودة).
                    // (EN) SEM045 (class fields): record the declared kind for the
                    //      member-assign Void guard — `type` is nullptr above, so no
                    //      other route carries it. Gated on addField succeeding: on a
                    //      duplicate name findField walks the inheritance chain and
                    //      would clobber the PARENT's field kind (quality review).
                    if (classType->addField(fieldDecl->name, nullptr, vis, fieldDecl->isStatic, defaultValue,
                                            deferredConstructClass, deferredConstructLine,
                                            deferredConstructColumn))
                    {
                        if (Data::ClassField *addedField = classType->findField(fieldDecl->name))
                        {
                            addedField->declaredKind = fieldDecl->type;
                        }
                    }

                    // Initialize static fields with their values
                    if (fieldDecl->isStatic)
                    {
                        classType->setStaticField(fieldDecl->name, defaultValue);
#ifdef DEBUG_OOP
                        std::cout << "[OOP]   - حقل ثابت: " << fieldDecl->name << " = " << defaultValue.toString() << "\n";
#endif
                    }
                    else
                    {
#ifdef DEBUG_OOP
                        std::cout << "[OOP]   - حقل: " << fieldDecl->name << " = " << defaultValue.toString() << "\n";
#endif
                    }
                }
                // Store constructor
                else if (auto *ctorDecl = dynamic_cast<ConstructorDecl *>(member.get()))
                {
                    // Store pointer to constructor (it's owned by ClassDecl)
                    // We'll access it through this pointer when creating instances
                    classType->constructor.reset(ctorDecl);
                    // Prevent ClassDecl from deleting it
                    member.release();
                }
                // Store destructor
                // (AR) تسجيل الهدام / (EN) Register destructor
                else if (auto *dtorDecl = dynamic_cast<DestructorDecl *>(member.get()))
                {
                    classType->destructor.reset(dtorDecl);
                    member.release();
                }
                // Store methods
                else if (auto *methodDecl = dynamic_cast<MethodDecl *>(member.get()))
                {
                    // Convert AccessModifier to Visibility
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

                    // Add method to ClassType
                    classType->addMethod(
                        methodDecl->name,
                        vis,
                        nullptr, // returnType
                        methodDecl->parameters,
                        methodDecl->body ? std::unique_ptr<AST::BlockStmt>(dynamic_cast<AST::BlockStmt *>(methodDecl->body.release())) : nullptr,
                        methodDecl->isStatic,
                        methodDecl->isVirtual,
                        methodDecl->isAbstract);

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) 🔑 نوعُ إرجاعِ الطريقةِ المُصرَّحُ يُسجَّل باسمٍ مؤهَّلٍ
                    //      `صنف.طريقة`. و`addMethod` أعلاه يمرّرُ `nullptr` لنوعِ
                    //      الإرجاع (حقلُ `ClassMethod::returnType` من نوعِ `Type*`
                    //      القديمِ ولا يُملأ قطُّ)، فكان جسمُ الطريقةِ يُنفَّذ بنوعِ
                    //      إرجاعٍ `Unknown` — فلا تُطبَّق عليه موافقةُ النوعِ المُصرَّح
                    //      (`عشري`/`رقم`/`بايت`) التي تُطبَّق على الدوالِّ الحرّة.
                    //      فكانت `دالة بايت هات() ارجع 300` تُخرِج 300 في المفسّرِ و44
                    //      في المترجّم. والسجلُّ هنا لا يُغيّرُ بنيةَ `ClassMethod`،
                    //      ويقرؤه `executeFunctionBodyWithFuncName` بالمفتاحِ نفسِه.
                    // (AR) ⚠️ وطريقةٌ مورَّثةٌ تُنادى على كائنِ الصنفِ المشتقِّ لا
                    //      يجدُها المفتاحُ (اسمُ الصنفِ يخالفُ صنفَ التعريف)، فتسقطُ
                    //      إلى `Unknown` كما كانت — نقصٌ معلومٌ لا انحدار.
                    // (EN) The method's declared return kind, recorded under a qualified
                    //      `Class.method` key. addMethod above passes nullptr for the
                    //      return type (ClassMethod::returnType is the legacy Type* and is
                    //      never filled), so method bodies ran with an Unknown return type
                    //      and never got the declared-type coercion free functions get —
                    //      a `بايت` method returning 300 printed 300 in the interpreter and
                    //      44 in the compiler. An inherited method invoked on a derived
                    //      object still misses the key and falls back to Unknown as before.
                    // ═══════════════════════════════════════════════════════════════
                    functionReturnTypes_[node.name + "." + methodDecl->name] =
                        methodDecl->returnType;

                    // ═══════════════════════════════════════════════════════════════
                    // (AR) نقل العقود البرمجية (يتطلب/يضمن) إلى ClassMethod
                    // (EN) Transfer Design by Contract (requires/ensures) to ClassMethod
                    // ═══════════════════════════════════════════════════════════════
                    if (!methodDecl->preconditions.empty() || !methodDecl->postconditions.empty())
                    {
                        auto *registeredMethod = classType->findMethod(methodDecl->name);
                        if (registeredMethod)
                        {
                            for (auto &pre : methodDecl->preconditions)
                            {
                                registeredMethod->preconditions.push_back(std::shared_ptr<AST::Expression>(pre.release()));
                            }
                            for (auto &post : methodDecl->postconditions)
                            {
                                registeredMethod->postconditions.push_back(std::shared_ptr<AST::Expression>(post.release()));
                            }
                        }
                    }

#ifdef DEBUG_OOP

                    std::cout << "[OOP]   - طريقة: " << methodDecl->name
                              << " بـ " << methodDecl->parameters.size() << " معاملات\n";
#endif
                }
                // Store properties
                else if (auto *propertyDecl = dynamic_cast<AST::PropertyDecl *>(member.get()))
                {
                    // Convert AccessModifier to Visibility
                    AST::Visibility vis = AST::Visibility::PUBLIC;
                    if (propertyDecl->visibility == AST::AccessModifier::PUBLIC)
                    {
                        vis = AST::Visibility::PUBLIC;
                    }
                    else if (propertyDecl->visibility == AST::AccessModifier::PRIVATE)
                    {
                        vis = AST::Visibility::PRIVATE;
                    }
                    else if (propertyDecl->visibility == AST::AccessModifier::PROTECTED)
                    {
                        vis = AST::Visibility::PROTECTED;
                    }

                    // Create ClassProperty
                    ClassProperty property(propertyDecl->name, nullptr, vis);
                    property.isStatic = propertyDecl->isStatic;

                    // Store getter body
                    if (propertyDecl->getter && propertyDecl->getter->body)
                    {
                        property.getterBody = std::unique_ptr<AST::BlockStmt>(
                            dynamic_cast<AST::BlockStmt *>(propertyDecl->getter->body.release()));
                    }

                    // Store setter body if exists
                    if (propertyDecl->setter && propertyDecl->setter->body)
                    {
                        property.setterBody = std::unique_ptr<AST::BlockStmt>(
                            dynamic_cast<AST::BlockStmt *>(propertyDecl->setter->body.release()));
                        property.setterParamName = propertyDecl->setter->parameterName;
                    }

                    classType->addProperty(std::move(property));

                    std::string accessType = property.isReadOnly() ? " (للقراءة فقط)" : property.isWriteOnly() ? " (للكتابة فقط)"
                                                                                                               : "";
#ifdef DEBUG_OOP

                    std::cout << "[OOP]   - خاصية: " << propertyDecl->name << accessType << "\n";
#endif
                }
                // Store operator overloads
                // (AR) تسجيل العوامل المحملة زائداً / (EN) Register operator overloads
                else if (auto *operatorDecl = dynamic_cast<AST::OperatorDecl *>(member.get()))
                {
                    OperatorOverload overload(operatorDecl->operatorSymbol, operatorDecl->access);
                    overload.parameters = operatorDecl->parameters;
                    overload.returnType = operatorDecl->returnType;
                    overload.body = std::move(operatorDecl->body);

                    classType->addOperatorOverload(std::move(overload));

#ifdef DEBUG_OOP
                    std::cout << "[OOP]   - عامل محمل زائداً: " << operatorDecl->operatorSymbol
                              << " بـ " << operatorDecl->parameters.size() << " معاملات\n";
#endif
                }
            }

            // Register class with ClassManager
            // (AR) إذا كان الصنف مسجلاً بالفعل (من التحليل)، قم بإزالته أولاً
            // (EN) If class is already registered (from parsing), remove it first
            if (classManager->hasClass(node.name))
            {
#ifdef DEBUG_OOP

                std::cout << "[OOP] الصنف مسجل مسبقاً (من مرحلة التحليل)، سيتم تحديثه...\n";
#endif
                // Note: ClassManager automatically overwrites existing class definition
                // No need for explicit removeClass method - registerClass handles this
            }

            bool registered = classManager->registerClass(std::move(classType));

            // (AR) كشف دورة الوراثة الفاسدة بعد اكتمال التسجيل: الدورةُ لا تكتمل
            //      إلا حين يُحدَّث تسجيلٌ مؤقتٌ في مكانه («أ يرث ب» ثم «ب يرث أ»)
            //      — وقبل هذا الحارس كان أولُ نداءِ طريقةٍ يُعلّق البحثَ في سلسلةِ
            //      baseClass إلى الأبد بلا تشخيص (رصد المراجعة العدائية — a_cycle).
            // (EN) Detect an invalid inheritance cycle once registration completes:
            //      the cycle only closes when a temporary registration is updated
            //      in place («أ يرث ب» then «ب يرث أ») — and before this guard the
            //      first method call hung the baseClass-chain lookup forever with
            //      no diagnostic (adversarial finding — a_cycle).
            {
                ClassType *cycleWalker = classManager->getClass(node.name);
                std::unordered_set<const ClassType *> chainSeen;
                while (cycleWalker && chainSeen.insert(cycleWalker).second)
                {
                    cycleWalker = cycleWalker->baseClass;
                }
                if (cycleWalker)
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_INHERITANCE_CYCLE,
                        node.position,
                        {{"class", node.name}, {"base", cycleWalker->name}});
                }
            }

            if (registered)
            {
#ifdef DEBUG_OOP

                std::cout << "[OOP] ✅ تم تسجيل الصنف: " << node.name << "\n";
#endif
            }
            else
            {
// Already registered - update it instead
#ifdef DEBUG_OOP

                std::cout << "[OOP] ⚠️ الصنف موجود مسبقاً - تم التخطي\n";
#endif
            }

            // ═══════════════════════════════════════════════════════════════
            // (AR) معالجة مزخرفات الصنف (Class Decorators) - من الأسفل للأعلى
            // (EN) Process class decorators - bottom to top (like Python)
            // @مزخرف1
            // @مزخرف2
            // صنف ص ... نهاية
            // → ص = مزخرف1(مزخرف2(ص))
            // ═══════════════════════════════════════════════════════════════
            if (!node.decorators.empty())
            {
                std::string currentClassName = node.name;

                // (AR) المزخرفات تُطبّق بترتيب عكسي (الأخير أولاً)
                // (EN) Decorators apply in reverse order (last first)
                for (auto it = node.decorators.rbegin(); it != node.decorators.rend(); ++it)
                {
                    auto *decoratorExpr = dynamic_cast<AST::DecoratorExpr *>(it->get());
                    if (!decoratorExpr)
                        continue;

                    // (AR) البحث عن دالة المزخرف
                    // (EN) Find decorator function
                    size_t expectedArgs = 1;
                    if (decoratorExpr->hasArguments)
                    {
                        expectedArgs += decoratorExpr->arguments.size();
                    }

                    auto decoratorFunc = functionManager_.getFunction(decoratorExpr->name, expectedArgs);
                    if (!decoratorFunc)
                    {
                        decoratorFunc = functionManager_.getFunction(decoratorExpr->name, 1);
                    }

                    if (decoratorFunc)
                    {
                        // (AR) تنفيذ المزخرف: ندخل نطاق ونمرر اسم الصنف كوسيط أول
                        // (EN) Execute decorator: enter scope and pass class name as first argument
                        variableManager_.enterScope(Data::ScopeType::FUNCTION, "class_decorator_" + decoratorExpr->name);

                        const auto &params = decoratorFunc->getParameters();

                        // (AR) الوسيط الأول = اسم الصنف
                        // (EN) First argument = class name
                        if (params.size() >= 1)
                        {
                            variableManager_.define(params[0].name, Data::Value(currentClassName));
                        }

                        // (AR) بقية الوسائط = وسائط المزخرف
                        // (EN) Remaining args = decorator arguments
                        if (decoratorExpr->hasArguments && expressionEvaluator_)
                        {
                            for (size_t i = 0; i < decoratorExpr->arguments.size() && (i + 1) < params.size(); ++i)
                            {
                                decoratorExpr->arguments[i]->accept(*expressionEvaluator_);
                                variableManager_.define(params[i + 1].name, expressionEvaluator_->getResult());
                            }
                        }

                        // (AR) تنفيذ جسم المزخرف
                        // (EN) Execute decorator body
                        auto bodyNode = decoratorFunc->getBody();
                        auto bodyStmt = dynamic_cast<AST::Statement *>(bodyNode.get());

                        Data::Value decoratorResult;
                        try
                        {
                            if (bodyStmt)
                            {
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

                        // (AR) إذا أرجع المزخرف نصاً — يمكن استخدامه كاسم صنف جديد
                        // (EN) If decorator returned a string — can use as new class name
                        if (decoratorResult.isString())
                        {
                            std::string newName = decoratorResult.toString();
                            if (!newName.empty() && newName != currentClassName)
                            {
                                currentClassName = newName;
                            }
                        }
                    }
                    else
                    {
                        std::cerr << "(AR) تحذير: مزخرف الصنف '" << decoratorExpr->name
                                  << "' غير معرّف / (EN) Warning: Class decorator '"
                                  << decoratorExpr->name << "' is not defined." << std::endl;
                    }
                }
            }
        }

        // ======================================================================
        // (AR) تسجيل صنف من عقدة ClassDeclStmt (class_nodes.h)
        // (EN) Register class from ClassDeclStmt node (class_nodes.h)
        // ======================================================================

        void StatementExecutor::visitClassDeclStmt(AST::ClassDeclStmt &node)
        {
#ifdef DEBUG_OOP
            std::cout << "[OOP] تنفيذ تصريح صنف (ClassDeclStmt): " << node.name << "\n";
#endif

            auto *classManager = ClassManager::getInstance();

            // (AR) تحقق من وجود الصنف / (EN) Check if class already exists
            if (classManager->hasClass(node.name))
            {
                auto *existingClass = classManager->getClass(node.name);
                if (!existingClass->fields.empty() || !existingClass->methods.empty() || existingClass->constructor)
                {
                    // (AR) الصنف موجود بالكامل — تخطيه بدلاً من رمي خطأ (يحدث مع استيراد * المتعدد)
                    // (EN) Class fully exists — skip silently instead of throwing (happens with multiple * imports)
#ifdef DEBUG_OOP
                    std::cout << "[OOP] تخطي إعادة تعريف الصنف (ClassDeclStmt): " << node.name << "\n";
#endif
                    return;
                }
            }

            auto classType = std::make_unique<ClassType>(node.name);

            // (AR) مسار التسجيل الثاني يجب أن يعبئ sourceFile أيضا — معيار
            //      ربط moduleCaptures في المرحلة 3 (ع-1) يقارن به
            // (EN) Second registration path must also fill sourceFile — the
            //      moduleCaptures attachment criterion (ع-1) compares against it
            classType->sourceFile = currentFilePath_;

            // (AR) معالجة الوراثة / (EN) Handle inheritance
            if (!node.baseClasses.empty())
            {
                std::vector<ClassType *> baseClasses;
                for (const auto &baseName : node.baseClasses)
                {
                    ClassType *baseClass = classManager->getClass(baseName);
                    if (!baseClass)
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_BASE_CLASS_NOT_FOUND,
                            node.position,
                            {{"base", baseName}, {"class", node.name}});
                    }
                    baseClasses.push_back(baseClass);
                }

                classType->baseClass = baseClasses[0];

                if (baseClasses.size() > 1)
                {
                    for (size_t i = 1; i < baseClasses.size(); ++i)
                    {
                        classType->additionalBases.push_back(baseClasses[i]);
                    }
                }
            }

            // (AR) معالجة الحقول / (EN) Process fields
            for (auto &field : node.fields)
            {
                if (!field)
                    continue;

                AST::Visibility vis = AST::Visibility::PUBLIC;
                switch (field->access)
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

                // ═══════════════════════════════════════════════════════════════
                // (AR) إصلاح: تقييم القيمة الافتراضية لجميع الحقول
                // (EN) Fix: Evaluate default value for all fields
                // ═══════════════════════════════════════════════════════════════
                Value defaultValue;
                // (AR) نسخةٌ ثانيةٌ من تأجيلِ المُهيّئِ المُنشِئ — يجب أن تبقى مطابقةً
                //      للأولى في visitClassDecl، وإلّا انقسم الصنفُ الواحدُ سلوكًا
                //      بحسبِ المسارِ الذي دخل منه.
                // (EN) Second copy of the deferral; must stay identical to the first.
                // (AR) نسخةٌ ثانيةٌ من استثناءِ الساكن — يجب أن تبقى مطابقةً للأولى.
                // (EN) Second copy of the static exclusion; must match the first.
                std::string deferredConstructClass;
                size_t deferredConstructLine = 0;
                size_t deferredConstructColumn = 0;
                AST::NewExpr *newObjectInitializer =
                    field->isStatic
                        ? nullptr
                        : dynamic_cast<AST::NewExpr *>(
                              field->initializer ? field->initializer.get() : nullptr);
                if (newObjectInitializer)
                {
                    deferredConstructClass = newObjectInitializer->className;
                    deferredConstructLine = newObjectInitializer->position.line;
                    deferredConstructColumn = newObjectInitializer->position.column;
                }
                else if (field->initializer)
                {
                    try
                    {
                        field->initializer->accept(*expressionEvaluator_);
                        defaultValue = expressionEvaluator_->getResult();
                    }
                    catch (const std::exception &e)
                    {
                        // (AR) فشل تقييم المُهيئ — استخدم القيمة الافتراضية مع تحذير
                        // (EN) Initializer evaluation failed — use default with warning
                        std::cerr << "[Warning] Field '" << field->name
                                  << "' initializer failed: " << e.what() << std::endl;
                        defaultValue = Value();
                    }
                    catch (...)
                    {
                        std::cerr << "[Warning] Field '" << field->name
                                  << "' initializer failed (unknown error)" << std::endl;
                        defaultValue = Value();
                    }
                }
                else
                {
                    // (AR) من مصدرِ الحقيقةِ الواحد — كنظيرتِها في `visitClassDecl`.
                    //      ولم تعد الحاجةُ إلى تحذيرِ «أبقِ النسختَين متطابقتَين»:
                    //      نسختان تُصانان باليدِ تتطابقان اليومَ وتفترقان غدًا،
                    //      ونداءٌ واحدٌ لا يفترق أصلًا.
                    // (EN) From the single source of truth, like its twin. Two
                    //      hand-maintained copies match today and drift tomorrow;
                    //      one call cannot drift at all.
                    defaultValue = Data::defaultValueForTypeKind(field->type);
                }

                // (AR) تمرير القيمة الافتراضية عند إضافة الحقل
                // (EN) Pass default value when adding field
                // (AR) SEM045: نظيرُ موضعِ التسجيل الأوّل — التصنيفُ المُصرَّح يُدوَّن هنا
                //      أيضًا وإلّا بقيت حقولُ هذا المسارِ عمياءَ عن الحارس (درسُ
                //      «الرقعة تسدّ في ملفٍّ وتترك الأشقاء»). الملءُ مشروطٌ بنجاح
                //      addField (انظر الموضعَ الأوّل).
                // (EN) SEM045: twin of the first registration site — record here too or
                //      this path's fields stay invisible to the guard. Gated on
                //      addField succeeding (see the first site).
                if (classType->addField(field->name, nullptr, vis, field->isStatic, defaultValue,
                                        deferredConstructClass, deferredConstructLine,
                                        deferredConstructColumn))
                {
                    if (Data::ClassField *addedField = classType->findField(field->name))
                    {
                        addedField->declaredKind = field->type;
                    }
                }

                if (field->isStatic)
                {
                    classType->setStaticField(field->name, defaultValue);
                }
            }

            // (AR) معالجة الباني / (EN) Process constructor
            if (node.constructor)
            {
                classType->constructor.reset(node.constructor.get());
                node.constructor.release();
            }

            // (AR) معالجة الهدام / (EN) Process destructor
            if (node.destructor)
            {
                classType->destructor.reset(node.destructor.get());
                node.destructor.release();
            }

            // (AR) معالجة الطرق / (EN) Process methods
            for (auto &method : node.methods)
            {
                if (!method)
                    continue;

                AST::Visibility vis = AST::Visibility::PUBLIC;
                switch (method->access)
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
                    method->name,
                    vis,
                    nullptr,
                    method->parameters,
                    method->body ? std::unique_ptr<AST::BlockStmt>(dynamic_cast<AST::BlockStmt *>(method->body.release())) : nullptr,
                    method->isStatic,
                    method->isVirtual,
                    method->isAbstract);
            }

            // (AR) تسجيل الصنف / (EN) Register class
            if (classManager->hasClass(node.name))
            {
                // overwrite
            }

            classManager->registerClass(std::move(classType));

#ifdef DEBUG_OOP
            std::cout << "[OOP] ✅ تم تسجيل الصنف (ClassDeclStmt): " << node.name << "\n";
#endif
        }

        void StatementExecutor::visitFieldDecl(AST::FieldDecl &node)
        {
#ifdef DEBUG_OOP

            std::cout << "[OOP] تنفيذ تصريح حقل: " << node.name << "\n";
#endif

            // (AR) تصريحات الحقول جزء من بنية الصنف، سيتم معالجتها عند إنشاء كائنات الصنف
            // (EN) Field declarations are part of class structure, processed when class is instantiated
            // Note: Fields are already registered in ClassManager during class definition (visitClassDecl)
        }

        void StatementExecutor::visitMethodDecl(AST::MethodDecl &node)
        {
#ifdef DEBUG_OOP

            std::cout << "[OOP] تنفيذ تصريح طريقة: " << node.name << "\n";
#endif

            // (AR) تصريحات الطرق جزء من بنية الصنف، سيتم تسجيلها عند تعريف الصنف
            // (EN) Method declarations are part of class structure, registered when class is defined
            // Note: Methods are already registered in ClassManager during class definition (visitClassDecl)
        }

        void StatementExecutor::visitPropertyDecl(AST::PropertyDecl &node)
        {
#ifdef DEBUG_OOP

            std::cout << "[OOP] تنفيذ تصريح خاصية (Property): " << node.name << "\n";
#endif

            // (AR) تصريحات الخصائص جزء من بنية الصنف، سيتم تسجيلها عند تعريف الصنف
            // (EN) Property declarations are part of class structure, registered when class is defined
            // Note: Properties with getters/setters are already registered in ClassManager (visitClassDecl)
        }

        void StatementExecutor::visitConstructorDecl(AST::ConstructorDecl &node)
        {
#ifdef DEBUG_OOP

            std::cout << "[OOP] تنفيذ تصريح باني\n";
#endif

            // (AR) الباني جزء من بنية الصنف، سيتم استدعاؤه عند إنشاء كائنات جديدة
            // (EN) Constructor is part of class structure, invoked when creating new instances
            // Note: Constructor is executed automatically in NewExpr evaluation
        }

        void StatementExecutor::visitDestructorDecl(AST::DestructorDecl &node)
        {
#ifdef DEBUG_OOP

            std::cout << "[OOP] تنفيذ تصريح هادم\n";
#endif

            // (AR) الهادم جزء من بنية الصنف، سيتم استدعاؤه عند حذف الكائنات
            // (EN) Destructor is part of class structure, invoked when deleting instances
            // Note: Destructor is called automatically during object cleanup
        }

        // ======================================================================
        // (AR) إنشاء نسخة ملموسة من صنف قالب
        // (EN) Instantiate a concrete version of a template class
        // ======================================================================

    } // namespace Interpreter
} // namespace Sad
