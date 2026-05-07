/**
 * @file statement_executor_oop_types.cpp
 * @brief (AR) تنفيذ الميزات النوعية OOP (template/trait/impl/extension/enum) في StatementExecutor
 * @brief (EN) OOP type-level features execution in StatementExecutor
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

#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int
namespace Sad
{
    namespace Interpreter
    {

        using namespace AST;
        using namespace Data;

        // ======================================================================
        // (AR) تنفيذ تصريح صنف / (EN) Execute Class Declaration
        // ======================================================================

        void StatementExecutor::instantiateTemplateClass(AST::TemplateClassDecl &templateNode, const std::string &className)
        {
            auto *classManager = ClassManager::getInstance();

            // (AR) لا نحتاج إنشاءه مجدداً إن كان موجوداً
            // (EN) No need to re-instantiate if already registered
            if (classManager->hasClass(className))
            {
                return;
            }

            // (AR) إنشاء ClassType جديد بالاسم الملموس
            // (EN) Create new ClassType with concrete name
            auto classType = std::make_unique<ClassType>(className);

            // (AR) معالجة الوراثة
            // (EN) Handle inheritance
            if (!templateNode.superclasses.empty())
            {
                ClassType *baseClass = classManager->getClass(templateNode.superclasses[0]);
                if (baseClass)
                {
                    classType->baseClass = baseClass;
                }
            }

            // (AR) معالجة الأعضاء: حقول فقط + توقيعات الطرق (بدون أجسام)
            // (EN) Process members: fields only + method signatures (no bodies)
            // ملاحظة: لا نخزّن الباني أو أجسام الطرق في ClassType لتجنب مشاكل الملكية
            // Note: We DON'T store constructor or method bodies in ClassType to avoid unique_ptr ownership conflicts
            // Instead, we look them up at runtime from the template AST via templateInstanceSources_
            for (auto &member : templateNode.members)
            {
                if (auto *fieldDecl = dynamic_cast<FieldDecl *>(member.get()))
                {
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
                    classType->addField(fieldDecl->name, nullptr, vis, fieldDecl->isStatic);
                }
                else if (auto *methodDecl = dynamic_cast<MethodDecl *>(member.get()))
                {
                    // (AR) نضيف توقيع الطريقة فقط (بدون body) — الجسم يُحلّ من القالب عند التنفيذ
                    // (EN) Add method signature only (no body) — body resolved from template at runtime
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
                        nullptr, // returnType
                        methodDecl->parameters,
                        nullptr, // body — resolved at runtime from template
                        methodDecl->isStatic,
                        methodDecl->isVirtual);
                }
                else if (auto *propertyDecl = dynamic_cast<AST::PropertyDecl *>(member.get()))
                {
                    AST::Visibility vis = AST::Visibility::PUBLIC;
                    if (propertyDecl->visibility == AST::AccessModifier::PUBLIC)
                        vis = AST::Visibility::PUBLIC;
                    else if (propertyDecl->visibility == AST::AccessModifier::PRIVATE)
                        vis = AST::Visibility::PRIVATE;
                    else if (propertyDecl->visibility == AST::AccessModifier::PROTECTED)
                        vis = AST::Visibility::PROTECTED;

                    ClassProperty property(propertyDecl->name, nullptr, vis);
                    property.isStatic = propertyDecl->isStatic;
                    classType->addProperty(std::move(property));
                }
                // (AR) الباني يُتخطّى هنا — يُحلّ عبر templateInstanceSources_ عند إنشاء الكائن
                // (EN) Constructor is skipped here — resolved via templateInstanceSources_ at object creation
            }

            // (AR) حفظ مرجع مصدر القالب للبحث عن الباني والطرق عند التنفيذ
            // (EN) Store template source reference for constructor/method lookup at runtime
            templateInstanceSources_[className] = &templateNode;

            // (AR) تسجيل الصنف الملموس
            // (EN) Register the concrete class
            classManager->registerClass(std::move(classType));
        }

        // ======================================================================
        // (AR) تنفيذ تصريح واجهة/سمة / (EN) Execute Trait Declaration
        // ======================================================================

        void StatementExecutor::visitTraitDecl(AST::TraitDecl &node)
        {
            // (AR) إنشاء تعريف الواجهة
            // (EN) Create trait definition
            auto *classManager = ClassManager::getInstance();

            TraitDefinition traitDef(node.name);
            traitDef.superTraits = node.superTraits;

            // (AR) تحويل دوال الواجهة
            // (EN) Convert trait methods
            for (auto &method : node.methods)
            {
                TraitMethodInfo info;
                info.name = method.name;
                info.returnType = method.returnType;
                info.hasDefaultImpl = (method.defaultImpl != nullptr);

                // (AR) تحويل defaultImpl من shared_ptr<Statement> إلى shared_ptr<BlockStmt>
                // (EN) Convert defaultImpl from shared_ptr<Statement> to shared_ptr<BlockStmt>
                if (method.defaultImpl)
                {
                    info.defaultBody = std::dynamic_pointer_cast<AST::BlockStmt>(method.defaultImpl);
                }

                for (const auto &param : method.params)
                {
                    info.paramTypes.push_back(param.type);
                    info.paramNames.push_back(param.name);
                }

                traitDef.requiredMethods.push_back(std::move(info));
            }

            // (AR) تسجيل الواجهة في مدير الأصناف
            // (EN) Register trait in class manager
            if (!classManager->registerTrait(std::move(traitDef)))
            {
                // (AR) تخطي السمة المكررة — يحدث مع الاستيراد المتعدد
                // (EN) Skip duplicate trait — happens with multiple imports
#ifdef DEBUG_OOP
                std::cout << "[OOP] تخطي إعادة تعريف السمة: " << node.name << "\n";
#endif
            }
        }

        // ======================================================================
        // (AR) تنفيذ كتلة التنفيذ (impl) / (EN) Execute Impl Block
        // ======================================================================

        void StatementExecutor::visitImplDecl(AST::ImplDecl &node)
        {
            auto *classManager = ClassManager::getInstance();

            // (AR) التحقق من وجود الصنف المستهدف
            // (EN) Verify target class exists
            ClassType *targetClass = classManager->getClass(node.targetType);
            if (!targetClass)
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_CLASS_NOT_FOUND,
                    node.position,
                    {{"class", node.targetType}});
            }

            // (AR) إضافة الدوال من كتلة التنفيذ إلى الصنف
            // (EN) Add methods from impl block to the class
            for (auto &method : node.methods)
            {
                // (AR) معالجة كل دالة
                auto *funcDecl = dynamic_cast<AST::FunctionDecl *>(method.get());
                if (funcDecl)
                {
                    // (AR) تحويل الجسم إلى BlockStmt
                    std::unique_ptr<AST::BlockStmt> bodyBlock = nullptr;
                    if (funcDecl->body)
                    {
                        auto *block = dynamic_cast<AST::BlockStmt *>(funcDecl->body.get());
                        if (block)
                        {
                            bodyBlock = std::unique_ptr<AST::BlockStmt>(
                                dynamic_cast<AST::BlockStmt *>(funcDecl->body.release()));
                        }
                    }

                    targetClass->addMethod(
                        funcDecl->name,
                        AST::Visibility::PUBLIC,
                        nullptr, // returnType as Type*
                        funcDecl->parameters,
                        std::move(bodyBlock),
                        false, // isStatic
                        false  // isVirtual
                    );
                }
            }

            // (AR) إذا كانت هناك واجهة محددة، التحقق منها وتسجيلها
            // (EN) If there's a specific trait, validate and register it
            if (!node.traitName.empty())
            {
                if (!classManager->hasTrait(node.traitName))
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_CLASS_NOT_FOUND,
                        node.position,
                        {{"class", "trait " + node.traitName}});
                }

                // (AR) إضافة الدوال الافتراضية المفقودة من السمة
                // (EN) Add missing default methods from trait
                const TraitDefinition *traitDef = classManager->getTrait(node.traitName);
                if (traitDef)
                {
                    for (const auto &reqMethod : traitDef->requiredMethods)
                    {
                        if (reqMethod.hasDefaultImpl && reqMethod.defaultBody)
                        {
                            // (AR) تحقق هل الصنف يملك هذه الدالة فعلاً
                            // (EN) Check if class already has this method
                            if (!targetClass->findMethod(reqMethod.name))
                            {
                                // (AR) بناء قائمة المعاملات
                                // (EN) Build parameter list
                                std::vector<AST::Parameter> params;
                                for (size_t i = 0; i < reqMethod.paramNames.size(); ++i)
                                {
                                    params.push_back(AST::Parameter(
                                        reqMethod.paramNames[i],
                                        i < reqMethod.paramTypes.size() ? reqMethod.paramTypes[i] : DataType::UNKNOWN));
                                }

                                // (AR) استخدام الجسم المشترك مباشرة — shared_ptr يضمن الحياة
                                // (EN) Use shared body directly — shared_ptr guarantees lifetime
                                targetClass->addDefaultMethod(
                                    reqMethod.name,
                                    AST::Visibility::PUBLIC,
                                    nullptr,
                                    params,
                                    reqMethod.defaultBody,
                                    false,
                                    false);
                            }
                        }
                    }
                }

                if (!classManager->validateTraitImpl(node.targetType, node.traitName))
                {
                    ::Sad::Errors::throwRuntime(
                        ::Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND,
                        node.position,
                        {{"class", node.targetType}, {"method", "trait " + node.traitName + " methods"}});
                }

                classManager->registerTraitImpl(node.targetType, node.traitName);
            }
        }

        // ======================================================================
        // (AR) تنفيذ كتلة الامتداد / (EN) Execute Extension Block
        // ======================================================================
        //
        // (AR) كتلة الامتداد تضيف دوال جديدة لصنف/نوع موجود بدون تعديله مباشرة.
        //      تعيد استخدام نفس آلية ImplDecl — تبحث عن الصنف في ClassManager
        //      ثم تضيف الدوال الجديدة إليه.
        //
        // (EN) Extension block adds new methods to an existing class/type without
        //      modifying it directly. Reuses the same mechanism as ImplDecl — looks up
        //      the class in ClassManager and adds the new methods to it.
        // ======================================================================

        void StatementExecutor::visitExtensionDecl(AST::ExtensionDecl &node)
        {
            auto *classManager = ClassManager::getInstance();

            // (AR) التحقق من وجود الصنف المستهدف
            // (EN) Verify target class exists
            ClassType *targetClass = classManager->getClass(node.targetType);
            if (!targetClass)
            {
                ::Sad::Errors::throwRuntime(
                    ::Sad::Errors::ErrorCode::RUN_CLASS_NOT_FOUND,
                    node.position,
                    {{"class", node.targetType}});
            }

            // (AR) إضافة الدوال من كتلة الامتداد إلى الصنف
            // (EN) Add methods from extension block to the class
            for (auto &method : node.methods)
            {
                auto *funcDecl = dynamic_cast<AST::FunctionDecl *>(method.get());
                if (funcDecl)
                {
                    // (AR) التحقق من عدم وجود دالة بنفس الاسم — الامتداد لا يتجاوز الدوال الموجودة
                    // (EN) Verify no method with same name exists — extensions cannot override
                    if (targetClass->findMethod(funcDecl->name))
                    {
                        ::Sad::Errors::throwRuntime(
                            ::Sad::Errors::ErrorCode::RUN_METHOD_NOT_FOUND,
                            node.position,
                            {{"class", node.targetType}, {"method", funcDecl->name + " (already exists, cannot override via extension)"}});
                    }

                    // (AR) تحويل الجسم إلى BlockStmt
                    // (EN) Convert body to BlockStmt
                    std::unique_ptr<AST::BlockStmt> bodyBlock = nullptr;
                    if (funcDecl->body)
                    {
                        auto *block = dynamic_cast<AST::BlockStmt *>(funcDecl->body.get());
                        if (block)
                        {
                            bodyBlock = std::unique_ptr<AST::BlockStmt>(
                                dynamic_cast<AST::BlockStmt *>(funcDecl->body.release()));
                        }
                    }

                    targetClass->addMethod(
                        funcDecl->name,
                        AST::Visibility::PUBLIC,
                        nullptr,
                        funcDecl->parameters,
                        std::move(bodyBlock),
                        false,
                        false);
                }
            }
        }

        // ======================================================================
        // (AR) تنفيذ تصريح ماكرو / (EN) Execute Macro Declaration
        // ======================================================================
        //
        // (AR) تخزين الماكرو في الخريطة للتوسيع لاحقاً عند الاستدعاء بأسلوب اسم!(...)
        // (EN) Store macro in map for later expansion when invoked with name!(...)
        // ======================================================================

        void StatementExecutor::visitMacroDecl(AST::MacroDecl &node)
        {
            // (AR) تخزين مؤشر الماكرو (AST يملك الذاكرة)
            // (EN) Store macro pointer (AST owns the memory)
            macros_[node.name] = &node;
        }

        // ======================================================================
        // (AR) تنفيذ تصريح تعداد / (EN) Execute Enum Declaration
        // ======================================================================

        void StatementExecutor::visitEnumDecl(AST::EnumDecl &node)
        {
            // ═══════════════════════════════════════════════════════════════
            // (AR) تنفيذ تعداد شامل — يدعم التعدادات البسيطة والجبرية (ADT)
            //      البسيطة: أعضاء بقيم عددية (مثل: تعداد اللون { أحمر، أخضر })
            //      الجبرية (ADT): أعضاء مع بيانات مرتبطة (مثل: دائرة(نصف_القطر))
            // (EN) Full enum implementation — supports simple enums and ADT
            //      Simple: members with integer values
            //      ADT: members carrying associated data (e.g., Circle(radius))
            // ═══════════════════════════════════════════════════════════════

            // (AR) هل التعداد جبري (يحتوي على أعضاء مع بيانات)؟
            // (EN) Is the enum an ADT (contains members with data)?
            bool isADT = node.isADT();

            Data::Value::MapType enumMap;
            Data::Value::ArrayType allNames;  // (AR) كل أسماء العناصر
            Data::Value::ArrayType allValues; // (AR) كل قيم العناصر
            Data::Value::MapType reverseMap;  // (AR) خريطة عكسية: قيمة → اسم
            int autoValue = 0;

            for (auto &member : node.members)
            {
                if (member.hasData())
                {
                    // ═══════════════════════════════════════════════════════════
                    // (AR) عضو تعداد جبري (ADT) — مع بيانات مرتبطة
                    //      مثال: دائرة(نصف_القطر) — يتم تسجيله كدالة بانية
                    //      عند استدعاء شكل.دائرة(5) يُنشئ خريطة:
                    //        { __تعداد__: "شكل", __عضو__: "دائرة", __حقول__: [5],
                    //          __جبري__: true, نصف_القطر: 5 }
                    // (EN) ADT enum member — with associated data
                    //      Example: Circle(radius) — registered as constructor function
                    //      Calling Shape.Circle(5) creates a map:
                    //        { __تعداد__: "Shape", __عضو__: "Circle", __حقول__: [5],
                    //          __جبري__: true, radius: 5 }
                    // ═══════════════════════════════════════════════════════════

                    // (AR) إنشاء خريطة بيانات وصفية للباني (بدلاً من lambda)
                    //      عند استدعاء شكل.دائرة(5)، يكتشف visitCallExpr هذه الخريطة
                    //      ويُنشئ كائن variant تلقائياً من الوسائط
                    // (EN) Create constructor metadata map (instead of lambda)
                    //      When calling Shape.Circle(5), visitCallExpr detects this map
                    //      and creates a variant object automatically from the arguments
                    std::string qualifiedName = node.name + "." + member.name;

                    Data::Value::MapType constructorMap;
                    constructorMap["__باني_نموذج__"] = Data::Value(true);
                    constructorMap["__تعداد__"] = Data::Value(node.name);
                    constructorMap["__عضو__"] = Data::Value(member.name);

                    // (AR) أسماء الحقول كمصفوفة نصية
                    // (EN) Field names as string array
                    Data::Value::ArrayType fieldNamesArr;
                    for (const auto &fn : member.fields)
                    {
                        fieldNamesArr.push_back(Data::Value(fn));
                    }
                    constructorMap["__حقول_أسماء__"] = Data::Value(fieldNamesArr);
                    constructorMap["__عدد_حقول__"] = Data::Value(::Sad::Security::SafeArithmetic::assertSafeCast<int>(member.fields.size(), "statement_executor_oop_types_size"));

                    Data::Value constructorVal(constructorMap);
                    variableManager_.defineOrAssign(qualifiedName, constructorVal);

                    // (AR) تسجيل في خريطة التعداد
                    // (EN) Register in enum map
                    enumMap[member.name] = constructorVal;
                    allNames.push_back(Data::Value(member.name));
                    allValues.push_back(Data::Value(qualifiedName));
                }
                else
                {
                    // ═══════════════════════════════════════════════════════════
                    // (AR) عضو تعداد بسيط أو Unit variant في ADT
                    // (EN) Simple enum member or Unit variant in ADT
                    // ═══════════════════════════════════════════════════════════

                    Data::Value memberVal;

                    if (isADT)
                    {
                        // (AR) عضو Unit في تعداد ADT — يُخزن كخريطة variant بدون حقول
                        // (EN) Unit member in ADT enum — stored as variant map without fields
                        Data::Value::MapType variantMap;
                        variantMap["__تعداد__"] = Data::Value(node.name);
                        variantMap["__عضو__"] = Data::Value(member.name);
                        variantMap["__جبري__"] = Data::Value(true);
                        variantMap["__حقول__"] = Data::Value(Data::Value::ArrayType{});
                        memberVal = Data::Value(variantMap);
                    }
                    else if (member.value)
                    {
                        // (AR) قيمة صريحة في تعداد بسيط
                        // (EN) Explicit value in simple enum
                        memberVal = evaluateExpression(*member.value);
                        if (memberVal.isInteger())
                        {
                            autoValue = memberVal.toInt() + 1;
                        }
                    }
                    else
                    {
                        // (AR) قيمة تلقائية عددية في تعداد بسيط
                        // (EN) Automatic integer value in simple enum
                        memberVal = Data::Value(autoValue);
                        autoValue++;
                    }

                    // (AR) تسجيل باسم مؤهل (تعداد.عضو)
                    // (EN) Register with qualified name (Enum.Member)
                    std::string qualifiedName = node.name + "." + member.name;
                    variableManager_.defineOrAssign(qualifiedName, memberVal);

                    // (AR) تسجيل باسم بسيط أيضاً
                    // (EN) Also register with simple name
                    if (!isADT)
                    {
                        variableManager_.defineOrAssign(member.name, memberVal);
                    }

                    // (AR) إضافة للخرائط
                    // (EN) Add to maps
                    enumMap[member.name] = memberVal;
                    allNames.push_back(Data::Value(member.name));
                    allValues.push_back(memberVal);
                    if (!isADT)
                    {
                        reverseMap[memberVal.toString()] = Data::Value(member.name);
                    }
                }
            }

            // (AR) إضافة حقول مساعدة للتعداد
            // (EN) Add helper fields to enum
            enumMap["__اسم__"] = Data::Value(node.name);
            enumMap["__أسماء__"] = Data::Value(allNames);
            enumMap["__قيم__"] = Data::Value(allValues);
            enumMap["__عكسي__"] = Data::Value(reverseMap);
            enumMap["__عدد__"] = Data::Value(::Sad::Security::SafeArithmetic::assertSafeCast<int>(node.members.size(), "statement_executor_oop_types_size"));
            enumMap["__جبري__"] = Data::Value(isADT);

            // (AR) تسجيل التعداد نفسه كخريطة / (EN) Register the enum itself as a map
            Data::Value enumValue(enumMap);
            variableManager_.defineOrAssign(node.name, enumValue);

            // ═══════════════════════════════════════════════════════════════
            // (AR) تسجيل دالة مساعدة: اسم_التعداد.قيم() — ترجع مصفوفة بأسماء العناصر
            // (EN) Register helper: EnumName.قيم() — returns array of member names
            // ═══════════════════════════════════════════════════════════════
            std::string valuesFunc = node.name + ".أسماء";
            Data::Value namesList(allNames);
            variableManager_.defineOrAssign(valuesFunc, namesList);

            std::string countVar = node.name + ".عدد";
            Data::Value countVal(::Sad::Security::SafeArithmetic::assertSafeCast<int>(node.members.size(), "statement_executor_oop_types_size"));
            variableManager_.defineOrAssign(countVar, countVal);
        }

        // ═══════════════════════════════════════════════════════════════════════════
        // (AR) تنفيذ البنية — بنية بيانات بسيطة بدون وراثة
        // (EN) Struct implementation — simple data structure without inheritance
        // ═══════════════════════════════════════════════════════════════════════════

    } // namespace Interpreter
} // namespace Sad
