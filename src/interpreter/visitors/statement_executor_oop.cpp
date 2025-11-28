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

#include "interpreter/visitors/statement_executor.h"
#include "data/managers/class_manager.h"
#include "data/managers/object_manager.h"
#include "parser/ast/property_nodes.h"
#include <iostream>

namespace Sad {
namespace Interpreter {

using namespace AST;
using namespace Data;

// ======================================================================
// (AR) تنفيذ تصريح صنف / (EN) Execute Class Declaration
// ======================================================================

void StatementExecutor::visitClassDecl(AST::ClassDecl& node) {
    std::cout << "[OOP] تنفيذ تصريح صنف: " << node.name << "\n";
    
    // Get ClassManager instance
    auto* classManager = ClassManager::getInstance();
    
    // Check if class already exists
    if (classManager->hasClass(node.name)) {
        throw std::runtime_error("(AR) الصنف '" + node.name + "' معرّف مسبقاً. (EN) Class '" + node.name + "' is already defined.");
    }
    
    // Create new ClassType (simplified registration)
    // Full field/method registration will be done later when Type system is unified
    auto classType = std::make_unique<ClassType>(node.name);
    
    // Handle inheritance
    if (!node.superclass.empty()) {
        ClassType* baseClass = classManager->getClass(node.superclass);
        if (!baseClass) {
            throw std::runtime_error("(AR) الصنف الأساسي '" + node.superclass + "' غير موجود. (EN) Base class '" + node.superclass + "' not found.");
        }
        classType->baseClass = baseClass;
        std::cout << "[OOP] الصنف '" << node.name << "' يرث من '" << node.superclass << "'\n";
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
    
    std::cout << "[OOP] الصنف يحتوي على: " << fieldCount << " حقل، " 
              << methodCount << " طريقة، " << propertyCount << " خاصية";
    if (hasConstructor) std::cout << "، باني";
    if (hasDestructor) std::cout << "، هادم";
    std::cout << "\n";
    
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
                std::cout << "[OOP]   - حقل ثابت: " << fieldDecl->name << " = " << defaultValue.toString() << "\n";
            } else {
                std::cout << "[OOP]   - حقل: " << fieldDecl->name << "\n";
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
                methodDecl->isVirtual
            );
            
            std::cout << "[OOP]   - طريقة: " << methodDecl->name 
                      << " بـ " << methodDecl->parameters.size() << " معاملات\n";
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
            std::cout << "[OOP]   - خاصية: " << propertyDecl->name << accessType << "\n";
        }
    }
    
    // Register class with ClassManager
    bool registered = classManager->registerClass(std::move(classType));
    
    if (registered) {
        std::cout << "[OOP] ✅ تم تسجيل الصنف: " << node.name << "\n";
    } else {
        throw std::runtime_error("(AR) فشل تسجيل الصنف. (EN) Failed to register class.");
    }
}

void StatementExecutor::visitFieldDecl(AST::FieldDecl& node) {
    std::cout << "[OOP] تنفيذ تصريح حقل: " << node.name << "\n";
    
    // TODO: Field declarations are part of class structure
    // They will be processed when the class is instantiated
}

void StatementExecutor::visitMethodDecl(AST::MethodDecl& node) {
    std::cout << "[OOP] تنفيذ تصريح طريقة: " << node.name << "\n";
    
    // TODO: Method declarations are part of class structure
    // They will be registered when the class is defined
}

void StatementExecutor::visitPropertyDecl(AST::PropertyDecl& node) {
    std::cout << "[OOP] تنفيذ تصريح خاصية (Property): " << node.name << "\n";
    
    // TODO: Property declarations are part of class structure
    // They will be registered when the class is defined
}

void StatementExecutor::visitConstructorDecl(AST::ConstructorDecl& node) {
    std::cout << "[OOP] تنفيذ تصريح باني\n";
    
    // TODO: Constructor is part of class structure
    // It will be called when creating instances
}

void StatementExecutor::visitDestructorDecl(AST::DestructorDecl& node) {
    std::cout << "[OOP] تنفيذ تصريح هادم\n";
    
    // TODO: Destructor is part of class structure
    // It will be called when destroying instances
}

} // namespace Interpreter
} // namespace Sad
