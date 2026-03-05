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
#include "class_nodes.h"   // For ClassDeclStmt
#include "exception.h"     // For RuntimeError
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
            throw RuntimeError("(AR) الصنف '" + node.name + "' معرّف مسبقاً. (EN) Class '" + node.name + "' is already defined.", node.position);
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
                throw RuntimeError(
                    "(AR) الصنف الأساسي '" + baseName + "' غير موجود. " +
                    "(EN) Base class '" + baseName + "' not found.", node.position);
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
        
        // (AR) تخزين الأصناف الأساسية الإضافية للوراثة المتعددة
        // (EN) Store additional base classes for multiple inheritance
        if (baseClasses.size() > 1) {
            for (size_t i = 1; i < baseClasses.size(); ++i) {
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
            
            // ═══════════════════════════════════════════════════════════════
            // (AR) إصلاح: تقييم القيمة الافتراضية لجميع الحقول (ثابتة وغير ثابتة)
            // (EN) Fix: Evaluate default value for all fields (static and non-static)
            // ═══════════════════════════════════════════════════════════════
            Value defaultValue;
            
            // (AR) تقييم تعبير المُهيئ إن وجد
            // (EN) Evaluate initializer expression if present
            if (fieldDecl->initializer) {
                try {
                    fieldDecl->initializer->accept(*expressionEvaluator_);
                    defaultValue = expressionEvaluator_->getResult();
                } catch (const std::exception& e) {
                    // (AR) فشل التقييم — استخدم القيمة الافتراضية مع تحذير
                    // (EN) Evaluation failed — fall back to type default with warning
                    std::cerr << "[Warning] Field initializer evaluation failed: " << e.what() << std::endl;
                    defaultValue = Value();
                } catch (...) {
                    std::cerr << "[Warning] Field initializer evaluation failed (unknown error)" << std::endl;
                    defaultValue = Value();
                }
            } else {
                // (AR) لا يوجد مُهيئ — استخدم القيمة الافتراضية للنوع
                // (EN) No initializer — use type default
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
            }
            
            // (AR) تمرير القيمة الافتراضية عند إضافة الحقل
            // (EN) Pass default value when adding field
            classType->addField(fieldDecl->name, nullptr, vis, fieldDecl->isStatic, defaultValue);
            
            // Initialize static fields with their values
            if (fieldDecl->isStatic) {
                classType->setStaticField(fieldDecl->name, defaultValue);
                #ifdef DEBUG_OOP
                std::cout << "[OOP]   - حقل ثابت: " << fieldDecl->name << " = " << defaultValue.toString() << "\n";
                #endif
            } else {
                #ifdef DEBUG_OOP
                std::cout << "[OOP]   - حقل: " << fieldDecl->name << " = " << defaultValue.toString() << "\n";
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

    // ═══════════════════════════════════════════════════════════════
    // (AR) معالجة مزخرفات الصنف (Class Decorators) - من الأسفل للأعلى
    // (EN) Process class decorators - bottom to top (like Python)
    // @مزخرف1
    // @مزخرف2
    // صنف ص ... نهاية
    // → ص = مزخرف1(مزخرف2(ص))
    // ═══════════════════════════════════════════════════════════════
    if (!node.decorators.empty()) {
        std::string currentClassName = node.name;

        // (AR) المزخرفات تُطبّق بترتيب عكسي (الأخير أولاً)
        // (EN) Decorators apply in reverse order (last first)
        for (auto it = node.decorators.rbegin(); it != node.decorators.rend(); ++it) {
            auto* decoratorExpr = dynamic_cast<AST::DecoratorExpr*>(it->get());
            if (!decoratorExpr) continue;

            // (AR) البحث عن دالة المزخرف
            // (EN) Find decorator function
            size_t expectedArgs = 1;
            if (decoratorExpr->hasArguments) {
                expectedArgs += decoratorExpr->arguments.size();
            }

            auto decoratorFunc = functionManager_.getFunction(decoratorExpr->name, expectedArgs);
            if (!decoratorFunc) {
                decoratorFunc = functionManager_.getFunction(decoratorExpr->name, 1);
            }

            if (decoratorFunc) {
                // (AR) تنفيذ المزخرف: ندخل نطاق ونمرر اسم الصنف كوسيط أول
                // (EN) Execute decorator: enter scope and pass class name as first argument
                variableManager_.enterScope(Data::ScopeType::FUNCTION, "class_decorator_" + decoratorExpr->name);

                const auto& params = decoratorFunc->getParameters();

                // (AR) الوسيط الأول = اسم الصنف
                // (EN) First argument = class name
                if (params.size() >= 1) {
                    variableManager_.define(params[0].name, Data::Value(currentClassName));
                }

                // (AR) بقية الوسائط = وسائط المزخرف
                // (EN) Remaining args = decorator arguments
                if (decoratorExpr->hasArguments && expressionEvaluator_) {
                    for (size_t i = 0; i < decoratorExpr->arguments.size() && (i + 1) < params.size(); ++i) {
                        decoratorExpr->arguments[i]->accept(*expressionEvaluator_);
                        variableManager_.define(params[i + 1].name, expressionEvaluator_->getResult());
                    }
                }

                // (AR) تنفيذ جسم المزخرف
                // (EN) Execute decorator body
                auto bodyNode = decoratorFunc->getBody();
                auto bodyStmt = dynamic_cast<AST::Statement*>(bodyNode.get());

                Data::Value decoratorResult;
                try {
                    if (bodyStmt) {
                        auto blockStmt = dynamic_cast<AST::BlockStmt*>(bodyStmt);
                        if (blockStmt) {
                            for (auto& stmt : blockStmt->statements) {
                                stmt->accept(*this);
                                if (shouldStopExecution()) break;
                            }
                        } else {
                            bodyStmt->accept(*this);
                        }
                        if (flowControl_ == FlowControl::RETURN) {
                            decoratorResult = returnValue_;
                            resetFlowControl();
                        }
                    } else {
                        auto bodyExpr = dynamic_cast<AST::Expression*>(bodyNode.get());
                        if (bodyExpr && expressionEvaluator_) {
                            bodyExpr->accept(*expressionEvaluator_);
                            decoratorResult = expressionEvaluator_->getResult();
                        }
                    }
                } catch (...) {
                    variableManager_.exitScope();
                    throw;
                }

                variableManager_.exitScope();

                // (AR) إذا أرجع المزخرف نصاً — يمكن استخدامه كاسم صنف جديد
                // (EN) If decorator returned a string — can use as new class name
                if (decoratorResult.isString()) {
                    std::string newName = decoratorResult.toString();
                    if (!newName.empty() && newName != currentClassName) {
                        currentClassName = newName;
                    }
                }
            } else {
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

void StatementExecutor::visitClassDeclStmt(AST::ClassDeclStmt& node) {
    #ifdef DEBUG_OOP
    std::cout << "[OOP] تنفيذ تصريح صنف (ClassDeclStmt): " << node.name << "\n";
    #endif
    
    auto* classManager = ClassManager::getInstance();
    
    // (AR) تحقق من وجود الصنف / (EN) Check if class already exists
    if (classManager->hasClass(node.name)) {
        auto* existingClass = classManager->getClass(node.name);
        if (!existingClass->fields.empty() || !existingClass->methods.empty()) {
            throw RuntimeError("(AR) الصنف '" + node.name + "' معرّف مسبقاً. (EN) Class '" + node.name + "' is already defined.", node.position);
        }
    }
    
    auto classType = std::make_unique<ClassType>(node.name);
    
    // (AR) معالجة الوراثة / (EN) Handle inheritance
    if (!node.baseClasses.empty()) {
        std::vector<ClassType*> baseClasses;
        for (const auto& baseName : node.baseClasses) {
            ClassType* baseClass = classManager->getClass(baseName);
            if (!baseClass) {
                throw RuntimeError(
                    "(AR) الصنف الأساسي '" + baseName + "' غير موجود. " +
                    "(EN) Base class '" + baseName + "' not found.", node.position);
            }
            baseClasses.push_back(baseClass);
        }
        
        classType->baseClass = baseClasses[0];
        
        if (baseClasses.size() > 1) {
            for (size_t i = 1; i < baseClasses.size(); ++i) {
                classType->additionalBases.push_back(baseClasses[i]);
            }
        }
    }
    
    // (AR) معالجة الحقول / (EN) Process fields
    for (auto& field : node.fields) {
        if (!field) continue;
        
        AST::Visibility vis = AST::Visibility::PUBLIC;
        switch (field->access) {
            case AST::AccessModifier::PUBLIC:    vis = AST::Visibility::PUBLIC; break;
            case AST::AccessModifier::PRIVATE:   vis = AST::Visibility::PRIVATE; break;
            case AST::AccessModifier::PROTECTED: vis = AST::Visibility::PROTECTED; break;
        }
        
        // ═══════════════════════════════════════════════════════════════
        // (AR) إصلاح: تقييم القيمة الافتراضية لجميع الحقول
        // (EN) Fix: Evaluate default value for all fields
        // ═══════════════════════════════════════════════════════════════
        Value defaultValue;
        if (field->initializer) {
            try {
                field->initializer->accept(*expressionEvaluator_);
                defaultValue = expressionEvaluator_->getResult();
            } catch (...) {
                defaultValue = Value();
            }
        } else {
            switch (field->type) {
                case Data::DataType::INTEGER: defaultValue = Value(0); break;
                case Data::DataType::FLOAT:   defaultValue = Value(0.0); break;
                case Data::DataType::STRING:  defaultValue = Value(""); break;
                case Data::DataType::BOOLEAN: defaultValue = Value(false); break;
                default: defaultValue = Value();
            }
        }
        
        // (AR) تمرير القيمة الافتراضية عند إضافة الحقل
        // (EN) Pass default value when adding field
        classType->addField(field->name, nullptr, vis, field->isStatic, defaultValue);
        
        if (field->isStatic) {
            classType->setStaticField(field->name, defaultValue);
        }
    }
    
    // (AR) معالجة الباني / (EN) Process constructor
    if (node.constructor) {
        classType->constructor.reset(node.constructor.get());
        node.constructor.release();
    }
    
    // (AR) معالجة الهدام / (EN) Process destructor
    if (node.destructor) {
        classType->destructor.reset(node.destructor.get());
        node.destructor.release();
    }
    
    // (AR) معالجة الطرق / (EN) Process methods
    for (auto& method : node.methods) {
        if (!method) continue;
        
        AST::Visibility vis = AST::Visibility::PUBLIC;
        switch (method->access) {
            case AST::AccessModifier::PUBLIC:    vis = AST::Visibility::PUBLIC; break;
            case AST::AccessModifier::PRIVATE:   vis = AST::Visibility::PRIVATE; break;
            case AST::AccessModifier::PROTECTED: vis = AST::Visibility::PROTECTED; break;
        }
        
        classType->addMethod(
            method->name,
            vis,
            nullptr,
            method->parameters,
            method->body ? std::unique_ptr<AST::BlockStmt>(dynamic_cast<AST::BlockStmt*>(method->body.release())) : nullptr,
            method->isStatic,
            method->isVirtual,
            method->isAbstract
        );
    }
    
    // (AR) تسجيل الصنف / (EN) Register class
    if (classManager->hasClass(node.name)) {
        // overwrite
    }
    
    classManager->registerClass(std::move(classType));
    
    #ifdef DEBUG_OOP
    std::cout << "[OOP] ✅ تم تسجيل الصنف (ClassDeclStmt): " << node.name << "\n";
    #endif
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
        throw RuntimeError(
            "(AR) الواجهة '" + node.name + "' معرّفة مسبقاً. "
            "(EN) Trait '" + node.name + "' is already defined.", node.position);
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
        throw RuntimeError(
            "(AR) الصنف '" + node.targetType + "' غير معرّف. "
            "(EN) Class '" + node.targetType + "' is not defined.", node.position);
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
            throw RuntimeError(
                "(AR) الواجهة '" + node.traitName + "' غير معرّفة. "
                "(EN) Trait '" + node.traitName + "' is not defined.", node.position);
        }
        
        if (!classManager->validateTraitImpl(node.targetType, node.traitName)) {
            throw RuntimeError(
                "(AR) الصنف '" + node.targetType + "' لا ينفذ جميع دوال الواجهة '" + node.traitName + "'. "
                "(EN) Class '" + node.targetType + "' does not implement all methods of trait '" + node.traitName + "'.", node.position);
        }
        
        classManager->registerTraitImpl(node.targetType, node.traitName);
    }
}

// ======================================================================
// (AR) تنفيذ تصريح تعداد / (EN) Execute Enum Declaration
// ======================================================================

void StatementExecutor::visitEnumDecl(AST::EnumDecl& node) {
    // ═══════════════════════════════════════════════════════════════
    // (AR) تنفيذ تعداد شامل — يدعم قيم عددية ونصية
    //      ويوفر: اسم مؤهل (تعداد.عضو)، اسم بسيط، قيم()، يحتوي()
    // (EN) Full enum implementation — supports int and string values
    //      provides: qualified name (Enum.Member), simple name, values(), contains()
    // ═══════════════════════════════════════════════════════════════
    
    Data::Value::MapType enumMap;
    Data::Value::ArrayType allNames;    // (AR) كل أسماء العناصر
    Data::Value::ArrayType allValues;   // (AR) كل قيم العناصر
    Data::Value::MapType reverseMap;    // (AR) خريطة عكسية: قيمة → اسم
    int autoValue = 0;
    
    for (auto& member : node.members) {
        Data::Value memberVal;
        
        // (AR) إذا كان للعضو قيمة محددة، نقيّمها
        // (EN) If member has explicit value, evaluate it
        if (member.value) {
            memberVal = evaluateExpression(*member.value);
            if (memberVal.isInteger()) {
                autoValue = memberVal.toInt() + 1;
            }
        } else {
            // (AR) قيمة تلقائية عددية
            // (EN) Automatic integer value
            memberVal = Data::Value(autoValue);
            autoValue++;
        }
        
        // (AR) تسجيل باسم مؤهل (مثل: اللون.أحمر) / (EN) Register with qualified name
        std::string qualifiedName = node.name + "." + member.name;
        // (AR) تحسين أداء: بحث واحد / (EN) Performance: single lookup
        variableManager_.defineOrAssign(qualifiedName, memberVal);
        
        // (AR) تسجيل باسم بسيط أيضاً (مثل: أحمر) / (EN) Also register with simple name
        variableManager_.defineOrAssign(member.name, memberVal);
        
        // (AR) إضافة للخرائط / (EN) Add to maps
        enumMap[member.name] = memberVal;
        allNames.push_back(Data::Value(member.name));
        allValues.push_back(memberVal);
        reverseMap[memberVal.toString()] = Data::Value(member.name);
    }
    
    // (AR) إضافة حقول مساعدة للتعداد
    // (EN) Add helper fields to enum
    enumMap["__اسم__"] = Data::Value(node.name);           // (AR) اسم التعداد
    enumMap["__أسماء__"] = Data::Value(allNames);           // (AR) مصفوفة الأسماء
    enumMap["__قيم__"] = Data::Value(allValues);            // (AR) مصفوفة القيم
    enumMap["__عكسي__"] = Data::Value(reverseMap);          // (AR) خريطة عكسية
    enumMap["__عدد__"] = Data::Value(static_cast<int>(node.members.size())); // (AR) عدد العناصر
    
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
    Data::Value countVal(static_cast<int>(node.members.size()));
    variableManager_.defineOrAssign(countVar, countVal);
}

// ═══════════════════════════════════════════════════════════════════════════
// (AR) تنفيذ البنية — بنية بيانات بسيطة بدون وراثة
// (EN) Struct implementation — simple data structure without inheritance
// ═══════════════════════════════════════════════════════════════════════════
void StatementExecutor::visitStructDecl(AST::StructDecl& node) {
    // (AR) تسجيل البنية كصنف مبسط في ClassManager
    // (EN) Register struct as simplified class in ClassManager
    auto* classManager = Data::ClassManager::getInstance();
    
    if (classManager->hasClass(node.name)) {
        // (AR) إعادة تعريف — حذف القديم / (EN) Redefine — remove old
        // Just allow re-registration silently
    }
    
    auto classType = std::make_unique<Data::ClassType>(node.name);
    
    // (AR) تسجيل الحقول / (EN) Register fields
    for (auto& field : node.fields) {
        Data::Value defaultVal;
        if (field.defaultValue) {
            defaultVal = evaluateExpression(*field.defaultValue);
        }
        
        classType->addField(field.name, nullptr, AST::Visibility::PUBLIC, false, defaultVal);
    }
    
    // (AR) إنشاء باني تلقائي يقبل قيم الحقول
    // (EN) Create automatic constructor accepting field values
    std::string constructorName = node.name;
    
    // (AR) تسجيل الباني في FunctionManager
    // (EN) Register constructor in FunctionManager
    auto& funcMgr = functionManager_;
    auto fieldsCopy = node.fields; // (AR) نسخة من الحقول للاستخدام في الباني
    std::string structName = node.name;
    
    funcMgr.registerBuiltinFunction(constructorName,
        [structName, fieldsCopy]
        (const std::vector<std::shared_ptr<Data::Value>>& args) -> std::shared_ptr<Data::Value> {
            // (AR) إنشاء نسخة جديدة من البنية
            auto* classMgr = Data::ClassManager::getInstance();
            auto* classType = classMgr->getClass(structName);
            
            std::shared_ptr<Data::ObjectInstance> instance;
            if (classType) {
                auto* rawObj = classType->createInstance();
                instance = std::shared_ptr<Data::ObjectInstance>(rawObj);
            } else {
                // (AR) احتياطي: إنشاء كائن بسيط بدون صنف
                instance = std::make_shared<Data::ObjectInstance>(nullptr, 0);
            }
            
            // (AR) تعيين الحقول من الوسائط
            for (size_t i = 0; i < fieldsCopy.size(); ++i) {
                if (i < args.size() && args[i]) {
                    instance->fields[fieldsCopy[i].name] = *args[i];
                } else {
                    instance->fields[fieldsCopy[i].name] = Data::Value();
                }
            }
            
            return std::make_shared<Data::Value>(instance);
        }
    );
    
    // (AR) تسجيل دوال البنية إن وجدت
    // (EN) Register struct methods if any
    for (auto& method : node.methods) {
        if (method) {
            method->accept(*this);
        }
    }
    
    classManager->registerClass(std::move(classType));
}

// ═══════════════════════════════════════════════════════════════════════════
// (AR) تنفيذ اختبار — يشغل كتلة الاختبار ويطبع النتيجة
// (EN) Test execution — runs test block and prints result
// ═══════════════════════════════════════════════════════════════════════════
void StatementExecutor::visitTestDecl(AST::TestDecl& node) {
    // (AR) طباعة بداية الاختبار
    std::string testName = node.testName;
    
    try {
        // (AR) تنفيذ جسم الاختبار في نطاق جديد
        // (EN) Execute test body in new scope
        variableManager_.enterScope(Data::ScopeType::BLOCK, "test:" + testName);
        
        if (node.body) {
            node.body->accept(*this);
        }
        
        variableManager_.exitScope();
        
        if (node.shouldFail) {
            // (AR) كان متوقعاً أن يفشل لكنه نجح
            std::cout << "✗ " << testName << " (كان متوقعاً أن يفشل)" << std::endl;
        } else {
            std::cout << "✓ " << testName << std::endl;
        }
    } catch (const std::exception& e) {
        variableManager_.exitScope();
        
        if (node.shouldFail) {
            std::cout << "✓ " << testName << " (فشل كما هو متوقع)" << std::endl;
        } else {
            std::cout << "✗ " << testName << ": " << e.what() << std::endl;
        }
    } catch (...) {
        variableManager_.exitScope();
        
        if (node.shouldFail) {
            std::cout << "✓ " << testName << " (فشل كما هو متوقع)" << std::endl;
        } else {
            std::cout << "✗ " << testName << ": خطأ غير معروف" << std::endl;
        }
    }
}

} // namespace Interpreter
} // namespace Sad
