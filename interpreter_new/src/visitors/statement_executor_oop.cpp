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
        std::cout << "[OOP] استبدال التسجيل المؤقت للصنف: " << node.name << "\n";
    }
    
    // Create new ClassType (simplified registration)
    // Full field/method registration will be done later when Type system is unified
    auto classType = std::make_unique<ClassType>(node.name);
    
    // (AR) معالجة الوراثة المتعددة / (EN) Handle multiple inheritance
    if (!node.superclasses.empty()) {
        std::cout << "[OOP] الصنف '" << node.name << "' يرث من: ";
        
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
            std::cout << "'" << baseName << "' ";
        }
        std::cout << "\n";
        
        // (AR) تعيين الصنف الأساسي الأول (للتوافق مع النظام الحالي) / (EN) Set first base class (for current system compatibility)
        classType->baseClass = baseClasses[0];
        
        // (AR) ملاحظة: دعم كامل للوراثة المتعددة يتطلب تحديث ClassType لتخزين vector من base classes
        // (EN) Note: Full multiple inheritance support requires updating ClassType to store vector of base classes
        if (baseClasses.size() > 1) {
            std::cout << "[OOP] تحذير: الوراثة المتعددة مدعومة جزئياً. الصنف الأول فقط '" 
                      << node.superclasses[0] << "' سيُستخدم حالياً.\n";
            std::cout << "[OOP] Warning: Multiple inheritance partially supported. Only first class '" 
                      << node.superclasses[0] << "' will be used currently.\n";
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
    // (AR) إذا كان الصنف مسجلاً بالفعل (من التحليل)، قم بإزالته أولاً
    // (EN) If class is already registered (from parsing), remove it first
    if (classManager->hasClass(node.name)) {
        std::cout << "[OOP] الصنف مسجل مسبقاً (من مرحلة التحليل)، سيتم تحديثه...\n";
        // Note: ClassManager automatically overwrites existing class definition
        // No need for explicit removeClass method - registerClass handles this
    }
    
    bool registered = classManager->registerClass(std::move(classType));
    
    if (registered) {
        std::cout << "[OOP] ✅ تم تسجيل الصنف: " << node.name << "\n";
    } else {
        // Already registered - update it instead
        std::cout << "[OOP] ⚠️ الصنف موجود مسبقاً - تم التخطي\n";
    }
}

void StatementExecutor::visitFieldDecl(AST::FieldDecl& node) {
    std::cout << "[OOP] تنفيذ تصريح حقل: " << node.name << "\n";
    
    // (AR) تصريحات الحقول جزء من بنية الصنف، سيتم معالجتها عند إنشاء كائنات الصنف
    // (EN) Field declarations are part of class structure, processed when class is instantiated
    // Note: Fields are already registered in ClassManager during class definition (visitClassDecl)
}

void StatementExecutor::visitMethodDecl(AST::MethodDecl& node) {
    std::cout << "[OOP] تنفيذ تصريح طريقة: " << node.name << "\n";
    
    // (AR) تصريحات الطرق جزء من بنية الصنف، سيتم تسجيلها عند تعريف الصنف
    // (EN) Method declarations are part of class structure, registered when class is defined
    // Note: Methods are already registered in ClassManager during class definition (visitClassDecl)
}

void StatementExecutor::visitPropertyDecl(AST::PropertyDecl& node) {
    std::cout << "[OOP] تنفيذ تصريح خاصية (Property): " << node.name << "\n";
    
    // (AR) تصريحات الخصائص جزء من بنية الصنف، سيتم تسجيلها عند تعريف الصنف
    // (EN) Property declarations are part of class structure, registered when class is defined
    // Note: Properties with getters/setters are already registered in ClassManager (visitClassDecl)
}

void StatementExecutor::visitConstructorDecl(AST::ConstructorDecl& node) {
    std::cout << "[OOP] تنفيذ تصريح باني\n";
    
    // (AR) الباني جزء من بنية الصنف، سيتم استدعاؤه عند إنشاء كائنات جديدة
    // (EN) Constructor is part of class structure, invoked when creating new instances
    // Note: Constructor is executed automatically in NewExpr evaluation
}

void StatementExecutor::visitDestructorDecl(AST::DestructorDecl& node) {
    std::cout << "[OOP] تنفيذ تصريح هادم\n";
    
    // (AR) الهادم جزء من بنية الصنف، سيتم استدعاؤه عند حذف الكائنات
    // (EN) Destructor is part of class structure, invoked when deleting instances
    // Note: Destructor is called automatically during object cleanup
}

} // namespace Interpreter
} // namespace Sad
