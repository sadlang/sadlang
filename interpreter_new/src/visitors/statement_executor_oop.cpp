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
#include "property_nodes.h"
#include "declarations.h"  // For OperatorDecl
#include <iostream>

namespace Sad {
namespace Interpreter {

using namespace AST;
using namespace Data;

// ======================================================================
// (AR) تنفيذ تصريح صنف / (EN) Execute Class Declaration
// ======================================================================

void StatementExecutor::visitClassDecl(AST::ClassDecl& node) {
    #ifdef DEBUG_OOP

    std::cout << "[OOP] تنفيذ تصريح صنف: " << node.name << "\n";
#endif
    
    // Get ClassManager instance
    auto* classManager = ClassManager::getInstance();
    
    // Check if class already exists (from execution, not parsing)
    // (AR) تحقق من وجود الصنف من التنفيذ السابق، وليس من التحليل
    // (EN) Check if class already exists from previous execution, not from parsing
    if (classManager->hasClass(node.name)) {
        auto* existingClass = classManager->getClass(node.name);
        // If it's a temporary registration (empty class), we'll replace it
        // (AR) إذا كان تسجيلاً مؤقتاً (صنف فارغ)، سنستبدله
        if (!existingClass->fields.empty() || !existingClass->methods.empty()) {
            throw std::runtime_error("(AR) الصنف '" + node.name + "' معرّف مسبقاً. (EN) Class '" + node.name + "' is already defined.");
        }
        #ifdef DEBUG_OOP

        std::cout << "[OOP] استبدال التسجيل المؤقت للصنف: " << node.name << "\n";
#endif
    }
    
    // Create new ClassType (simplified registration)
    // Full field/method registration will be done later when Type system is unified
    auto classType = std::make_unique<ClassType>(node.name);
    
    // (AR) معالجة الوراثة المتعددة / (EN) Handle multiple inheritance
    if (!node.superclasses.empty()) {
        #ifdef DEBUG_OOP
        std::cout << "[OOP] الصنف '" << node.name << "' يرث من: ";
        #endif
        
        // (AR) التحقق من وجود جميع الأصناف الأساسية / (EN) Verify all base classes exist
        std::vector<ClassType*> baseClasses;
        for (const auto& baseName : node.superclasses) {
            ClassType* baseClass = classManager->getClass(baseName);
            if (!baseClass) {
                throw std::runtime_error(
                    "(AR) الصنف الأساسي '" + baseName + "' غير موجود. " +
                    "(EN) Base class '" + baseName + "' not found.");
            }
            baseClasses.push_back(baseClass);
            #ifdef DEBUG_OOP
            std::cout << "'" << baseName << "' ";
            #endif
        }
        #ifdef DEBUG_OOP
        std::cout << "\n";
        #endif
        
        // (AR) تعيين الصنف الأساسي الأول (للتوافق مع النظام الحالي) / (EN) Set first base class (for current system compatibility)
        classType->baseClass = baseClasses[0];
        
        // (AR) ملاحظة: دعم كامل للوراثة المتعددة يتطلب تحديث ClassType لتخزين vector من base classes
        // (EN) Note: Full multiple inheritance support requires updating ClassType to store vector of base classes
        if (baseClasses.size() > 1) {
            #ifdef DEBUG_OOP
            std::cout << "[OOP] تحذير: الوراثة المتعددة مدعومة جزئياً. الصنف الأول فقط '" 
                      << node.superclasses[0] << "' سيُستخدم حالياً.\n";
            std::cout << "[OOP] Warning: Multiple inheritance partially supported. Only first class '" 
                      << node.superclasses[0] << "' will be used currently.\n";
            #endif
        }
    }
    
    // Count fields and methods for reporting
    int fieldCount = 0;
    int methodCount = 0;
    int propertyCount = 0;
    bool hasConstructor = false;
    bool hasDestructor = false;
    
    for (auto& member : node.members) {
        if (dynamic_cast<FieldDecl*>(member.get())) {
            fieldCount++;
        } else if (dynamic_cast<MethodDecl*>(member.get())) {
            methodCount++;
        } else if (dynamic_cast<AST::PropertyDecl*>(member.get())) {
            propertyCount++;
        } else if (dynamic_cast<ConstructorDecl*>(member.get())) {
            hasConstructor = true;
        } else if (dynamic_cast<DestructorDecl*>(member.get())) {
            hasDestructor = true;
        }
    }
    
    #ifdef DEBUG_OOP
    std::cout << "[OOP] الصنف يحتوي على: " << fieldCount << " حقل، " 
              << methodCount << " طريقة، " << propertyCount << " خاصية";
    if (hasConstructor) std::cout << "، باني";
    if (hasDestructor) std::cout << "، هادم";
    std::cout << "\n";
    #endif
    
    // Register fields (simplified - without full type resolution for now)
    for (auto& member : node.members) {
        if (auto* fieldDecl = dynamic_cast<FieldDecl*>(member.get())) {
            // Convert AccessModifier to Visibility
            AST::Visibility vis = AST::Visibility::PUBLIC;
            switch (fieldDecl->access) {
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
            
            // For now, use nullptr for type (will be resolved later when Type system is unified)
            classType->addField(fieldDecl->name, nullptr, vis, fieldDecl->isStatic);
            
            // Initialize static fields with default values
            if (fieldDecl->isStatic) {
                Value defaultValue;
                // Use type default (initializer evaluation requires expression evaluator)
                switch (fieldDecl->type) {
                    case Data::DataType::INTEGER:
                        defaultValue = Value(0);
                        break;
                    case Data::DataType::FLOAT:
                        defaultValue = Value(0.0);
                        break;
                    case Data::DataType::STRING:
                        defaultValue = Value("");
                        break;
                    case Data::DataType::BOOLEAN:
                        defaultValue = Value(false);
                        break;
                    default:
                        defaultValue = Value();
                }
                classType->setStaticField(fieldDecl->name, defaultValue);
                #ifdef DEBUG_OOP

                std::cout << "[OOP]   - حقل ثابت: " << fieldDecl->name << " = " << defaultValue.toString() << "\n";
#endif
            } else {
                #ifdef DEBUG_OOP

                std::cout << "[OOP]   - حقل: " << fieldDecl->name << "\n";
#endif
            }
        }
        // Store constructor
        else if (auto* ctorDecl = dynamic_cast<ConstructorDecl*>(member.get())) {
            // Store pointer to constructor (it's owned by ClassDecl)
            // We'll access it through this pointer when creating instances
            classType->constructor.reset(ctorDecl);
            // Prevent ClassDecl from deleting it
            member.release();
        }
        // Store methods
        else if (auto* methodDecl = dynamic_cast<MethodDecl*>(member.get())) {
            // Convert AccessModifier to Visibility
            AST::Visibility vis = AST::Visibility::PUBLIC;
            switch (methodDecl->access) {
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
                nullptr,  // returnType
                methodDecl->parameters,
                methodDecl->body ? std::unique_ptr<AST::BlockStmt>(dynamic_cast<AST::BlockStmt*>(methodDecl->body.release())) : nullptr,
                methodDecl->isStatic,
                methodDecl->isVirtual,
                methodDecl->isAbstract
            );
            
            #ifdef DEBUG_OOP

            
            std::cout << "[OOP]   - طريقة: " << methodDecl->name 
                      << " بـ " << methodDecl->parameters.size() << " معاملات\n";
#endif
        }
        // Store properties
        else if (auto* propertyDecl = dynamic_cast<AST::PropertyDecl*>(member.get())) {
            // Convert AccessModifier to Visibility
            AST::Visibility vis = AST::Visibility::PUBLIC;
            if (propertyDecl->visibility == AST::AccessModifier::PUBLIC) {
                vis = AST::Visibility::PUBLIC;
            } else if (propertyDecl->visibility == AST::AccessModifier::PRIVATE) {
                vis = AST::Visibility::PRIVATE;
            } else if (propertyDecl->visibility == AST::AccessModifier::PROTECTED) {
                vis = AST::Visibility::PROTECTED;
            }
            
            // Create ClassProperty
            ClassProperty property(propertyDecl->name, nullptr, vis);
            property.isStatic = propertyDecl->isStatic;
            
            // Store getter body
            if (propertyDecl->getter && propertyDecl->getter->body) {
                property.getterBody = std::unique_ptr<AST::BlockStmt>(
                    dynamic_cast<AST::BlockStmt*>(propertyDecl->getter->body.release())
                );
            }
            
            // Store setter body if exists
            if (propertyDecl->setter && propertyDecl->setter->body) {
                property.setterBody = std::unique_ptr<AST::BlockStmt>(
                    dynamic_cast<AST::BlockStmt*>(propertyDecl->setter->body.release())
                );
                property.setterParamName = propertyDecl->setter->parameterName;
            }
            
            classType->addProperty(std::move(property));
            
            std::string accessType = property.isReadOnly() ? " (للقراءة فقط)" : 
                                   property.isWriteOnly() ? " (للكتابة فقط)" : "";
            #ifdef DEBUG_OOP

            std::cout << "[OOP]   - خاصية: " << propertyDecl->name << accessType << "\n";
#endif
        }
        // Store operator overloads
        // (AR) تسجيل العوامل المحملة زائداً / (EN) Register operator overloads
        else if (auto* operatorDecl = dynamic_cast<AST::OperatorDecl*>(member.get())) {
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
    if (classManager->hasClass(node.name)) {
        #ifdef DEBUG_OOP

        std::cout << "[OOP] الصنف مسجل مسبقاً (من مرحلة التحليل)، سيتم تحديثه...\n";
#endif
        // Note: ClassManager automatically overwrites existing class definition
        // No need for explicit removeClass method - registerClass handles this
    }
    
    bool registered = classManager->registerClass(std::move(classType));
    
    if (registered) {
        #ifdef DEBUG_OOP

        std::cout << "[OOP] ✅ تم تسجيل الصنف: " << node.name << "\n";
#endif
    } else {
        // Already registered - update it instead
        #ifdef DEBUG_OOP

        std::cout << "[OOP] ⚠️ الصنف موجود مسبقاً - تم التخطي\n";
#endif
    }
}

void StatementExecutor::visitFieldDecl(AST::FieldDecl& node) {
    #ifdef DEBUG_OOP

    std::cout << "[OOP] تنفيذ تصريح حقل: " << node.name << "\n";
#endif
    
    // (AR) تصريحات الحقول جزء من بنية الصنف، سيتم معالجتها عند إنشاء كائنات الصنف
    // (EN) Field declarations are part of class structure, processed when class is instantiated
    // Note: Fields are already registered in ClassManager during class definition (visitClassDecl)
}

void StatementExecutor::visitMethodDecl(AST::MethodDecl& node) {
    #ifdef DEBUG_OOP

    std::cout << "[OOP] تنفيذ تصريح طريقة: " << node.name << "\n";
#endif
    
    // (AR) تصريحات الطرق جزء من بنية الصنف، سيتم تسجيلها عند تعريف الصنف
    // (EN) Method declarations are part of class structure, registered when class is defined
    // Note: Methods are already registered in ClassManager during class definition (visitClassDecl)
}

void StatementExecutor::visitPropertyDecl(AST::PropertyDecl& node) {
    #ifdef DEBUG_OOP

    std::cout << "[OOP] تنفيذ تصريح خاصية (Property): " << node.name << "\n";
#endif
    
    // (AR) تصريحات الخصائص جزء من بنية الصنف، سيتم تسجيلها عند تعريف الصنف
    // (EN) Property declarations are part of class structure, registered when class is defined
    // Note: Properties with getters/setters are already registered in ClassManager (visitClassDecl)
}

void StatementExecutor::visitConstructorDecl(AST::ConstructorDecl& node) {
    #ifdef DEBUG_OOP

    std::cout << "[OOP] تنفيذ تصريح باني\n";
#endif
    
    // (AR) الباني جزء من بنية الصنف، سيتم استدعاؤه عند إنشاء كائنات جديدة
    // (EN) Constructor is part of class structure, invoked when creating new instances
    // Note: Constructor is executed automatically in NewExpr evaluation
}

void StatementExecutor::visitDestructorDecl(AST::DestructorDecl& node) {
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

void StatementExecutor::instantiateTemplateClass(AST::TemplateClassDecl& templateNode, const std::string& className) {
    auto* classManager = ClassManager::getInstance();
    
    // (AR) لا نحتاج إنشاءه مجدداً إن كان موجوداً
    // (EN) No need to re-instantiate if already registered
    if (classManager->hasClass(className)) {
        return;
    }
    
    // (AR) إنشاء ClassType جديد بالاسم الملموس
    // (EN) Create new ClassType with concrete name
    auto classType = std::make_unique<ClassType>(className);
    
    // (AR) معالجة الوراثة
    // (EN) Handle inheritance
    if (!templateNode.superclasses.empty()) {
        ClassType* baseClass = classManager->getClass(templateNode.superclasses[0]);
        if (baseClass) {
            classType->baseClass = baseClass;
        }
    }
    
    // (AR) معالجة الأعضاء: حقول فقط + توقيعات الطرق (بدون أجسام)
    // (EN) Process members: fields only + method signatures (no bodies)
    // ملاحظة: لا نخزّن الباني أو أجسام الطرق في ClassType لتجنب مشاكل الملكية
    // Note: We DON'T store constructor or method bodies in ClassType to avoid unique_ptr ownership conflicts
    // Instead, we look them up at runtime from the template AST via templateInstanceSources_
    for (auto& member : templateNode.members) {
        if (auto* fieldDecl = dynamic_cast<FieldDecl*>(member.get())) {
            AST::Visibility vis = AST::Visibility::PUBLIC;
            switch (fieldDecl->access) {
                case AST::AccessModifier::PUBLIC: vis = AST::Visibility::PUBLIC; break;
                case AST::AccessModifier::PRIVATE: vis = AST::Visibility::PRIVATE; break;
                case AST::AccessModifier::PROTECTED: vis = AST::Visibility::PROTECTED; break;
            }
            classType->addField(fieldDecl->name, nullptr, vis, fieldDecl->isStatic);
        }
        else if (auto* methodDecl = dynamic_cast<MethodDecl*>(member.get())) {
            // (AR) نضيف توقيع الطريقة فقط (بدون body) — الجسم يُحلّ من القالب عند التنفيذ
            // (EN) Add method signature only (no body) — body resolved from template at runtime
            AST::Visibility vis = AST::Visibility::PUBLIC;
            switch (methodDecl->access) {
                case AST::AccessModifier::PUBLIC: vis = AST::Visibility::PUBLIC; break;
                case AST::AccessModifier::PRIVATE: vis = AST::Visibility::PRIVATE; break;
                case AST::AccessModifier::PROTECTED: vis = AST::Visibility::PROTECTED; break;
            }
            classType->addMethod(
                methodDecl->name,
                vis,
                nullptr,  // returnType
                methodDecl->parameters,
                nullptr,  // body — resolved at runtime from template
                methodDecl->isStatic,
                methodDecl->isVirtual
            );
        }
        else if (auto* propertyDecl = dynamic_cast<AST::PropertyDecl*>(member.get())) {
            AST::Visibility vis = AST::Visibility::PUBLIC;
            if (propertyDecl->visibility == AST::AccessModifier::PUBLIC) vis = AST::Visibility::PUBLIC;
            else if (propertyDecl->visibility == AST::AccessModifier::PRIVATE) vis = AST::Visibility::PRIVATE;
            else if (propertyDecl->visibility == AST::AccessModifier::PROTECTED) vis = AST::Visibility::PROTECTED;
            
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

void StatementExecutor::visitTraitDecl(AST::TraitDecl& node) {
    // (AR) إنشاء تعريف الواجهة
    // (EN) Create trait definition
    auto* classManager = ClassManager::getInstance();
    
    TraitDefinition traitDef(node.name);
    traitDef.superTraits = node.superTraits;
    
    // (AR) تحويل دوال الواجهة
    // (EN) Convert trait methods
    for (auto& method : node.methods) {
        TraitMethodInfo info;
        info.name = method.name;
        info.returnType = method.returnType;
        info.hasDefaultImpl = (method.defaultImpl != nullptr);
        
        for (const auto& param : method.params) {
            info.paramTypes.push_back(param.type);
        }
        
        traitDef.requiredMethods.push_back(std::move(info));
    }
    
    // (AR) تسجيل الواجهة في مدير الأصناف
    // (EN) Register trait in class manager
    if (!classManager->registerTrait(std::move(traitDef))) {
        throw std::runtime_error(
            "(AR) الواجهة '" + node.name + "' معرّفة مسبقاً. "
            "(EN) Trait '" + node.name + "' is already defined.");
    }
}

// ======================================================================
// (AR) تنفيذ كتلة التنفيذ (impl) / (EN) Execute Impl Block
// ======================================================================

void StatementExecutor::visitImplDecl(AST::ImplDecl& node) {
    auto* classManager = ClassManager::getInstance();
    
    // (AR) التحقق من وجود الصنف المستهدف
    // (EN) Verify target class exists
    ClassType* targetClass = classManager->getClass(node.targetType);
    if (!targetClass) {
        throw std::runtime_error(
            "(AR) الصنف '" + node.targetType + "' غير معرّف. "
            "(EN) Class '" + node.targetType + "' is not defined.");
    }
    
    // (AR) إضافة الدوال من كتلة التنفيذ إلى الصنف
    // (EN) Add methods from impl block to the class
    for (auto& method : node.methods) {
        // (AR) معالجة كل دالة
        auto* funcDecl = dynamic_cast<AST::FunctionDecl*>(method.get());
        if (funcDecl) {
            // (AR) تحويل الجسم إلى BlockStmt
            std::unique_ptr<AST::BlockStmt> bodyBlock = nullptr;
            if (funcDecl->body) {
                auto* block = dynamic_cast<AST::BlockStmt*>(funcDecl->body.get());
                if (block) {
                    bodyBlock = std::unique_ptr<AST::BlockStmt>(
                        dynamic_cast<AST::BlockStmt*>(funcDecl->body.release()));
                }
            }
            
            targetClass->addMethod(
                funcDecl->name,
                AST::Visibility::PUBLIC,
                nullptr,  // returnType as Type*
                funcDecl->parameters,
                std::move(bodyBlock),
                false,  // isStatic
                false   // isVirtual
            );
        }
    }
    
    // (AR) إذا كانت هناك واجهة محددة، التحقق منها وتسجيلها
    // (EN) If there's a specific trait, validate and register it
    if (!node.traitName.empty()) {
        if (!classManager->hasTrait(node.traitName)) {
            throw std::runtime_error(
                "(AR) الواجهة '" + node.traitName + "' غير معرّفة. "
                "(EN) Trait '" + node.traitName + "' is not defined.");
        }
        
        if (!classManager->validateTraitImpl(node.targetType, node.traitName)) {
            throw std::runtime_error(
                "(AR) الصنف '" + node.targetType + "' لا ينفذ جميع دوال الواجهة '" + node.traitName + "'. "
                "(EN) Class '" + node.targetType + "' does not implement all methods of trait '" + node.traitName + "'.");
        }
        
        classManager->registerTraitImpl(node.targetType, node.traitName);
    }
}

} // namespace Interpreter
} // namespace Sad
