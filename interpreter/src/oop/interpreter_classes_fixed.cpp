// ======================================================================
// interpreter_classes_fixed.cpp - تنفيذ دوال البرمجة الكائنية
// OOP Functions Implementation - Fixed API Version
// ======================================================================
// الوصف: تنفيذ جميع دوال تنفيذ الأصناف والكائنات والطرق
// Description: Implementation of all class, object, and method execution functions
// ======================================================================
// تاريخ الإصلاح: يناير 2026
// Fix Date: January 2026
// ======================================================================

#include "interpreter_classes.h"
#include "core/interpreter_core.h"
#include "managers/class_manager.h"
#include "managers/object_manager.h"
#include "class_type.h"
#include "object_instance.h"
#include "value.h"

#include <iostream>
#include <stdexcept>

namespace Sad {
namespace Interpreter {

// ======================================================================
// متغير عام للكائن الحالي (thread-local للأمان)
// Global variable for current object (thread-local for safety)
// ======================================================================
static thread_local Data::ObjectInstance* g_currentObject = nullptr;

// ======================================================================
// دوال مساعدة داخلية / Internal Helper Functions
// ======================================================================

/**
 * @brief تحويل AccessModifier إلى AST::Visibility
 */
static AST::Visibility convertAccessToVisibility(AST::AccessModifier access) {
    switch (access) {
        case AST::AccessModifier::PUBLIC: return AST::Visibility::PUBLIC;
        case AST::AccessModifier::PRIVATE: return AST::Visibility::PRIVATE;
        case AST::AccessModifier::PROTECTED: return AST::Visibility::PROTECTED;
        default: return AST::Visibility::PRIVATE;
    }
}

// ======================================================================
// تنفيذ تصريح صنف / Execute Class Declaration
// ======================================================================

void executeClassDeclaration(Interpreter* interpreter, AST::ClassDeclStmt* node) {
    if (!node) {
        throw std::runtime_error("خطأ: عقدة تصريح الصنف فارغة / Error: Null class declaration node");
    }
    
    // الحصول على اسم الصنف
    std::string className = node->name;
    
    // الحصول على مدير الأصناف
    auto* classMgr = Data::ClassManager::getInstance();
    
    // التحقق من عدم وجود الصنف مسبقاً
    if (classMgr->hasClass(className)) {
        // (AR) تخطي إعادة التعريف — يحدث مع الاستيراد المتعدد
        // (EN) Skip redefinition — happens with multiple imports
        return;
    }
    
    // إنشاء نوع الصنف الجديد
    auto classType = std::make_unique<Data::ClassType>(className);
    
    // معالجة الوراثة إذا وجدت (baseClasses هو vector)
    if (!node->baseClasses.empty()) {
        const std::string& baseName = node->baseClasses[0];  // أول صنف أساسي
        if (!classMgr->hasClass(baseName)) {
            throw std::runtime_error("خطأ: الصنف الأساسي '" + baseName + 
                "' غير موجود / Error: Base class '" + baseName + "' not found");
        }
        Data::ClassType* baseClassPtr = classMgr->getClass(baseName);
        classType->setBaseClass(baseClassPtr);
    }
    
    // معالجة الحقول باستخدام API الصحيح
    // FieldDecl يستخدم: name, type, access, isStatic
    for (const auto& field : node->fields) {
        if (field) {
            classType->addField(
                field->name,
                nullptr,  // Type* - سيتم تحديده لاحقاً
                convertAccessToVisibility(field->access),
                field->isStatic,
                Data::Value()  // قيمة افتراضية فارغة
            );
        }
    }
    
    // معالجة الطرق باستخدام API الصحيح
    // MethodDecl يستخدم: name, parameters, access, isStatic, isVirtual, body
    for (const auto& method : node->methods) {
        if (method) {
            // نسخ المعاملات
            std::vector<AST::Parameter> params = method->parameters;
            
            classType->addMethod(
                method->name,
                convertAccessToVisibility(method->access),
                nullptr,  // Type* - نوع الإرجاع
                params,
                nullptr,  // body - سنخزنه بطريقة أخرى
                method->isStatic,
                method->isVirtual
            );
        }
    }
    
    // معالجة الباني إذا وجد
    // ConstructorDecl يستخدم: parameters, body, superArgs
    if (node->constructor) {
        // نحتاج نسخ الباني
    }
    
    // معالجة الهدام إذا وجد
    // DestructorDecl يستخدم: body
    if (node->destructor) {
        // مشابه للباني
    }
    
    // تسجيل الصنف
    classMgr->registerClass(std::move(classType));
}

// ======================================================================
// تنفيذ إنشاء كائن جديد / Execute New Expression
// ======================================================================

Data::Value executeNewExpression(Interpreter* interpreter, AST::NewExpr* node) {
    if (!node || !interpreter) {
        throw std::runtime_error("خطأ: معاملات فارغة / Error: Null parameters");
    }
    
    std::string className = node->className;
    
    // الحصول على مدير الأصناف
    auto* classMgr = Data::ClassManager::getInstance();
    
    // التحقق من وجود الصنف
    if (!classMgr->hasClass(className)) {
        throw std::runtime_error("خطأ: الصنف '" + className + 
            "' غير موجود / Error: Class '" + className + "' not found");
    }
    
    Data::ClassType* classType = classMgr->getClass(className);
    
    // إنشاء كائن جديد عبر ObjectManager
    auto* objMgr = Data::ObjectManager::getInstance();
    Data::ObjectInstance* object = objMgr->createObject(classType);
    
    if (!object) {
        throw std::runtime_error("خطأ: فشل إنشاء الكائن / Error: Failed to create object");
    }
    
    // تقييم معاملات الباني
    std::vector<Data::Value> arguments;
    for (const auto& arg : node->arguments) {
        arguments.push_back(interpreter->evaluateExpression(*arg));
    }
    
    // استدعاء الباني إذا وجد
    if (classType->hasConstructor()) {
        callConstructor(interpreter, object, nullptr, arguments);
    }
    
    // إرجاع قيمة الكائن باستخدام Value(ObjectPtr)
    // ObjectManager يُرجع مؤشراً خاماً، لكن نحتاج shared_ptr
    // نُنشئ shared_ptr بدون deleter لأن ObjectManager يملك الكائن
    auto objPtr = std::shared_ptr<Data::ObjectInstance>(object, [](Data::ObjectInstance*){});
    return Data::Value(objPtr);
}

// ======================================================================
// استدعاء الباني / Call Constructor
// ======================================================================

void callConstructor(Interpreter* interpreter, Data::ObjectInstance* object,
                     AST::ConstructorDecl* constructor, const std::vector<Data::Value>& arguments) {
    if (!interpreter || !object) {
        return;
    }
    
    // حفظ الكائن الحالي
    Data::ObjectInstance* previousObject = getCurrentObject(interpreter);
    setCurrentObject(interpreter, object);
    
    // إنشاء نطاق جديد للباني
    // ScopeManager::pushScope يحتاج نوع النطاق واسم اختياري
    interpreter->getScopeManager().pushScope(Data::ScopeType::FUNCTION, "constructor");
    
    // (AR) استخدام try/catch لضمان تنظيف النطاق حتى عند حدوث استثناء
    // (EN) Use try/catch to ensure scope cleanup even on exception
    try {
        // إذا كان الباني محدد، ربط المعاملات
        if (constructor) {
            const auto& params = constructor->parameters;
            for (size_t i = 0; i < params.size() && i < arguments.size(); ++i) {
                // استخدام define وليس setVariable
                interpreter->getVariableManager().define(params[i].name, arguments[i]);
            }
            
            // تنفيذ جسم الباني - body هو StmtPtr وليس vector
            if (constructor->body) {
                interpreter->executeStatement(*constructor->body);
            }
        }
    } catch (...) {
        // (AR) ضمان استعادة النطاق والكائن عند الخطأ
        // (EN) Ensure scope and object are restored on error
        interpreter->getScopeManager().popScope();
        setCurrentObject(interpreter, previousObject);
        throw;
    }
    
    // استعادة النطاق والكائن
    interpreter->getScopeManager().popScope();
    setCurrentObject(interpreter, previousObject);
    
    // تعيين علامة البناء
    object->markConstructed();
}

// ======================================================================
// استدعاء الهدام / Call Destructor
// ======================================================================

void callDestructor(Interpreter* interpreter, Data::ObjectInstance* object) {
    if (!interpreter || !object) {
        return;
    }
    
    Data::ClassType* classType = object->getClass();
    if (!classType || !classType->hasDestructor()) {
        return;
    }
    
    // (AR) حفظ الكائن الحالي / (EN) Save current object
    Data::ObjectInstance* previousObject = getCurrentObject(interpreter);
    setCurrentObject(interpreter, object);
    
    // (AR) إنشاء نطاق للهدام / (EN) Create scope for destructor
    interpreter->getScopeManager().pushScope(Data::ScopeType::FUNCTION, "destructor");
    
    // (AR) ربط حقول الكائن بالنطاق الحالي ليتاح الوصول لها عبر "هذا"
    // (EN) Bind object fields to current scope so they are accessible via "this"
    for (const auto& [fieldName, fieldValue] : object->fields) {
        interpreter->getVariableManager().define(fieldName, fieldValue);
    }
    
    // (AR) تنفيذ جسم الهدام من ClassType
    // (EN) Execute destructor body from ClassType
    try {
        if (classType->destructor && classType->destructor->body) {
            interpreter->executeStatement(*classType->destructor->body);
        }
    } catch (const std::exception& e) {
        // (AR) لا نترك استثناء الهدام يتسرب — نسجله فقط
        // (EN) Don't let destructor exceptions propagate — just log
        std::cerr << "(AR) خطأ في الهدام: " << e.what() 
                  << " / (EN) Error in destructor: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "(AR) خطأ غير معروف في الهدام / (EN) Unknown error in destructor" << std::endl;
    }
    
    // (AR) استعادة النطاق والكائن / (EN) Restore scope and object
    interpreter->getScopeManager().popScope();
    setCurrentObject(interpreter, previousObject);
    
    // (AR) استدعاء هدام الصنف الأساسي (إن وُجد)
    // (EN) Call base class destructor (if exists)
    if (classType->baseClass && classType->baseClass->hasDestructor()) {
        // (AR) نعيد استخدام نفس الكائن لأن حقول الأساس موجودة فيه
        // (EN) Reuse same object since base fields are in it
        Data::ClassType* baseClass = classType->baseClass;
        Data::ObjectInstance* previousObj2 = getCurrentObject(interpreter);
        setCurrentObject(interpreter, object);
        
        interpreter->getScopeManager().pushScope(Data::ScopeType::FUNCTION, "base_destructor");
        for (const auto& [fieldName, fieldValue] : object->fields) {
            interpreter->getVariableManager().define(fieldName, fieldValue);
        }
        
        try {
            if (baseClass->destructor && baseClass->destructor->body) {
                interpreter->executeStatement(*baseClass->destructor->body);
            }
        } catch (...) {
            // (AR) لا نسمح لأخطاء الهدام بالتسرب
        }
        
        interpreter->getScopeManager().popScope();
        setCurrentObject(interpreter, previousObj2);
    }
}

// ======================================================================
// تنفيذ الوصول للحقل / Execute Field Access
// ======================================================================

Data::Value executeFieldAccess(Interpreter* interpreter, AST::MemberAccessExpr* node) {
    if (!interpreter || !node) {
        throw std::runtime_error("خطأ: معاملات فارغة / Error: Null parameters");
    }
    
    // تقييم الكائن
    Data::Value objectValue = interpreter->evaluateExpression(*node->object);
    
    // التحقق من أن القيمة هي كائن
    if (!objectValue.isObject()) {
        throw std::runtime_error("خطأ: الوصول للحقل على قيمة غير كائنية / Error: Field access on non-object value");
    }
    
    auto objPtr = objectValue.toObject();
    std::string fieldName = node->memberName;
    
    // الوصول للحقل من الكائن
    auto it = objPtr->fields.find(fieldName);
    if (it != objPtr->fields.end()) {
        return it->second;
    }
    
    throw std::runtime_error("خطأ: الحقل '" + fieldName + "' غير موجود / Error: Field '" + fieldName + "' not found");
}

// ======================================================================
// تنفيذ تعيين الحقل / Execute Field Assignment
// ======================================================================

void executeFieldAssignment(Interpreter* interpreter, Data::ObjectInstance* object,
                            const std::string& fieldName, const Data::Value& value) {
    if (!interpreter || !object) {
        throw std::runtime_error("خطأ: معاملات فارغة / Error: Null parameters");
    }
    
    // تعيين القيمة
    object->setField(fieldName, value);
}

// ======================================================================
// تنفيذ استدعاء الطريقة / Execute Method Call
// ======================================================================

Data::Value executeMethodCall(Interpreter* interpreter, AST::MethodCallExpr* node) {
    if (!interpreter || !node) {
        throw std::runtime_error("خطأ: معاملات فارغة / Error: Null parameters");
    }
    
    // تقييم الكائن
    Data::Value objectValue = interpreter->evaluateExpression(*node->object);
    
    // التحقق من أن القيمة هي كائن
    if (!objectValue.isObject()) {
        throw std::runtime_error("خطأ: استدعاء طريقة على قيمة غير كائنية / Error: Method call on non-object value");
    }
    
    auto objPtr = objectValue.toObject();
    std::string methodName = node->methodName;
    
    // الحصول على نوع الصنف
    Data::ClassType* classType = objPtr->getClass();
    if (!classType) {
        throw std::runtime_error("خطأ: الكائن بدون صنف / Error: Object has no class");
    }
    
    // البحث عن الطريقة
    Data::ClassMethod* method = classType->findMethod(methodName);
    if (!method) {
        throw std::runtime_error("خطأ: الطريقة '" + methodName + "' غير موجودة / Error: Method '" + methodName + "' not found");
    }
    
    // تقييم المعاملات
    std::vector<Data::Value> arguments;
    for (const auto& arg : node->arguments) {
        arguments.push_back(interpreter->evaluateExpression(*arg));
    }
    
    // تنفيذ الطريقة
    return executeMethod(interpreter, objPtr.get(), method, arguments);
}

// ======================================================================
// تنفيذ طريقة / Execute Method
// ======================================================================

Data::Value executeMethod(Interpreter* interpreter, Data::ObjectInstance* object,
                          Data::ClassMethod* method, const std::vector<Data::Value>& arguments) {
    if (!interpreter || !object || !method) {
        throw std::runtime_error("خطأ: معاملات فارغة / Error: Null parameters");
    }
    
    // حفظ الكائن الحالي
    Data::ObjectInstance* previousObject = getCurrentObject(interpreter);
    setCurrentObject(interpreter, object);
    
    // إنشاء نطاق جديد
    interpreter->getScopeManager().pushScope(Data::ScopeType::FUNCTION, "method");
    
    Data::Value result;
    
    // ربط المعاملات من ClassMethod
    const auto& params = method->parameters;
    for (size_t i = 0; i < params.size() && i < arguments.size(); ++i) {
        interpreter->getVariableManager().define(params[i].name, arguments[i]);
    }
    
    // تنفيذ جسم الطريقة إذا كان موجوداً
    if (auto* theBody = method->getBody()) {
        for (const auto& stmt : theBody->statements) {
            auto execResult = interpreter->executeStatement(*stmt);
            if (!execResult.success) {
                break;
            }
        }
    }
    
    // استعادة النطاق والكائن
    interpreter->getScopeManager().popScope();
    setCurrentObject(interpreter, previousObject);
    
    return result;
}

// ======================================================================
// تنفيذ طريقة ثابتة / Execute Static Method
// ======================================================================

Data::Value executeStaticMethod(Interpreter* interpreter, Data::ClassType* classType,
                                const std::string& methodName, const std::vector<Data::Value>& arguments) {
    if (!interpreter || !classType) {
        throw std::runtime_error("خطأ: معاملات فارغة / Error: Null parameters");
    }
    
    Data::ClassMethod* method = classType->findMethod(methodName);
    if (!method || !method->isStatic) {
        throw std::runtime_error("خطأ: الطريقة الثابتة '" + methodName + 
            "' غير موجودة / Error: Static method '" + methodName + "' not found");
    }
    
    // إنشاء نطاق جديد
    interpreter->getScopeManager().pushScope(Data::ScopeType::FUNCTION, "static_method");
    
    Data::Value result;
    
    // ربط المعاملات
    const auto& params = method->parameters;
    for (size_t i = 0; i < params.size() && i < arguments.size(); ++i) {
        interpreter->getVariableManager().define(params[i].name, arguments[i]);
    }
    
    // تنفيذ جسم الطريقة
    if (auto* theBody = method->getBody()) {
        for (const auto& stmt : theBody->statements) {
            interpreter->executeStatement(*stmt);
        }
    }
    
    interpreter->getScopeManager().popScope();
    return result;
}

// ======================================================================
// تنفيذ تعبير 'هذا' / Execute This Expression
// ======================================================================

Data::Value executeThisExpression(Interpreter* interpreter, AST::ThisExpr* node) {
    if (!interpreter) {
        throw std::runtime_error("خطأ: المفسر فارغ / Error: Null interpreter");
    }
    
    Data::ObjectInstance* currentObj = getCurrentObject(interpreter);
    if (!currentObj) {
        throw std::runtime_error("خطأ: استخدام 'هذا' خارج طريقة / Error: Using 'this' outside method");
    }
    
    // إرجاع قيمة الكائن الحالي كـ Value
    auto objPtr = std::shared_ptr<Data::ObjectInstance>(currentObj, [](Data::ObjectInstance*){});
    return Data::Value(objPtr);
}

// ======================================================================
// تنفيذ تعبير 'الأساس' / Execute Super Expression
// ======================================================================

Data::Value executeSuperExpression(Interpreter* interpreter, AST::SuperExpr* node) {
    if (!interpreter) {
        throw std::runtime_error("خطأ: المفسر فارغ / Error: Null interpreter");
    }
    
    Data::ObjectInstance* currentObj = getCurrentObject(interpreter);
    if (!currentObj) {
        throw std::runtime_error("خطأ: استخدام 'الأساس' خارج طريقة / Error: Using 'super' outside method");
    }
    
    Data::ClassType* classType = currentObj->getClass();
    if (!classType || !classType->getBaseClass()) {
        throw std::runtime_error("خطأ: لا يوجد صنف أساسي / Error: No base class exists");
    }
    
    // إرجاع كائن الصنف الأساسي
    Data::ObjectInstance* baseInstance = currentObj->getBaseInstance();
    if (baseInstance) {
        auto basePtr = std::shared_ptr<Data::ObjectInstance>(baseInstance, [](Data::ObjectInstance*){});
        return Data::Value(basePtr);
    }
    
    throw std::runtime_error("خطأ: لا يوجد مثيل للصنف الأساسي / Error: No base class instance");
}

// ======================================================================
// تنفيذ الوصول للحقل الثابت / Execute Static Field Access
// ======================================================================

Data::Value executeStaticFieldAccess(Interpreter* interpreter,
                                     const std::string& className, const std::string& fieldName) {
    auto* classMgr = Data::ClassManager::getInstance();
    if (!classMgr->hasClass(className)) {
        throw std::runtime_error("خطأ: الصنف '" + className + 
            "' غير موجود / Error: Class '" + className + "' not found");
    }
    
    Data::ClassType* classType = classMgr->getClass(className);
    Data::ClassField* field = classType->findField(fieldName);
    
    if (!field || !field->isStatic) {
        throw std::runtime_error("خطأ: الحقل الثابت '" + fieldName + 
            "' غير موجود / Error: Static field '" + fieldName + "' not found");
    }
    
    Data::Value* value = classType->getStaticField(fieldName);
    if (value) {
        return *value;
    }
    
    return Data::Value();
}

// ======================================================================
// تنفيذ تعيين الحقل الثابت / Execute Static Field Assignment
// ======================================================================

void executeStaticFieldAssignment(Interpreter* interpreter,
                                  const std::string& className, const std::string& fieldName,
                                  const Data::Value& value) {
    auto* classMgr = Data::ClassManager::getInstance();
    if (!classMgr->hasClass(className)) {
        throw std::runtime_error("خطأ: الصنف '" + className + 
            "' غير موجود / Error: Class '" + className + "' not found");
    }
    
    Data::ClassType* classType = classMgr->getClass(className);
    Data::ClassField* field = classType->findField(fieldName);
    
    if (!field || !field->isStatic) {
        throw std::runtime_error("خطأ: الحقل الثابت '" + fieldName + 
            "' غير موجود / Error: Static field '" + fieldName + "' not found");
    }
    
    classType->setStaticField(fieldName, value);
}

// ======================================================================
// الدوال المساعدة / Helper Functions
// ======================================================================

bool checkMemberVisibility(Interpreter* interpreter, Data::ClassType* classType,
                           const std::string& memberName, bool isField) {
    if (!classType) {
        return false;
    }
    
    // الحصول على الكائن الحالي
    Data::ObjectInstance* currentObj = getCurrentObject(interpreter);
    
    // إذا كنا داخل نفس الصنف، الوصول مسموح
    if (currentObj) {
        Data::ClassType* currentClass = currentObj->getClass();
        if (currentClass && currentClass->getName() == classType->getName()) {
            return true;
        }
    }
    
    // فحص الرؤية باستخدام API الصحيح
    if (isField) {
        return classType->isFieldAccessible(memberName, nullptr);
    } else {
        return classType->isMethodAccessible(memberName, nullptr);
    }
}

Data::ObjectInstance* getCurrentObject(Interpreter* interpreter) {
    return g_currentObject;
}

void setCurrentObject(Interpreter* interpreter, Data::ObjectInstance* object) {
    g_currentObject = object;
}

bool checkParameterCompatibility(Interpreter* interpreter,
                                 const std::vector<AST::Parameter>& parameters,
                                 const std::vector<Data::Value>& arguments) {
    // التحقق من عدد المعاملات
    size_t requiredCount = 0;
    for (const auto& param : parameters) {
        if (!param.defaultValue) {  // استخدام defaultValue وليس hasDefault
            requiredCount++;
        }
    }
    
    if (arguments.size() < requiredCount || arguments.size() > parameters.size()) {
        return false;
    }
    
    return true;
}

void executeInitializerList(Interpreter* interpreter, Data::ObjectInstance* object,
                            const std::vector<AST::FieldInitializer>& initializers) {
    for (const auto& init : initializers) {
        Data::Value value = interpreter->evaluateExpression(*init.value);
        object->setField(init.fieldName, value);
    }
}

Data::ClassMethod* findMethodInChain(Interpreter* interpreter, Data::ClassType* classType,
                                     const std::string& methodName) {
    if (!classType) {
        return nullptr;
    }
    
    // البحث في الصنف الحالي باستخدام findMethod (يبحث أيضاً في الأصناف الأساسية)
    return classType->findMethod(methodName);
}

} // namespace Interpreter
} // namespace Sad
