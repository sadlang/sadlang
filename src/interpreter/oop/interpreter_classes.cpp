// ======================================================================
// interpreter_classes.cpp - تنفيذ مفسّر الأصناف / Class Interpreter Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ جميع دوال تنفيذ الأصناف والبرمجة الكائنية
//   يشمل تنفيذ الأصناف، الكائنات، الدوال، البانيات، والحقول
//
// English Description:
//   Implementation of all OOP execution functions
//   Includes class, object, method, constructor, and field execution
// ======================================================================

#include "interpreter/interpreter_classes.h"
#include "data/managers/class_manager.h"
#include "data/managers/object_manager.h"
#include "errors/error_manager.h"
#include <iostream>

namespace Sad {
namespace Interpreter {

using namespace AST;
using namespace Data;

// ======================================================================
// تنفيذ تصريح صنف / Execute Class Declaration
// ======================================================================

Value InterpreterClasses::executeClassDeclaration(ClassDeclaration* node) {
    // (AR) تنفيذ تصريح صنف وتسجيله في المدير
    // (EN) Execute class declaration and register in manager
    
    ClassManager* classMgr = ClassManager::getInstance();
    
    // (AR) التحقق من عدم تكرار الاسم
    // (EN) Check for duplicate name
    if (classMgr->hasClass(node->className)) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_REDEFINITION,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الصنف '" + node->className + "' معرّف مسبقاً",
            "Class '" + node->className + "' already defined"
        );
        return;
    }
    
    // (AR) إنشاء نوع الصنف
    // (EN) Create class type
    auto classType = std::make_unique<ClassType>(node->className);
    
    // (AR) تعيين الصنف الأساسي إذا وجد
    // (EN) Set base class if exists
    if (!node->baseClassName.empty()) {
        ClassType* baseClass = classMgr->getClass(node->baseClassName);
        if (!baseClass) {
            Sad::Errors::ErrorManager::getInstance().reportError(
                Sad::Errors::ErrorCode::SEM_UNDEFINED_CLASS,
                Sad::Errors::SourceLocation("<runtime>", 0, 0),
                "الصنف الأساسي '" + node->baseClassName + "' غير موجود",
                "Base class '" + node->baseClassName + "' not found"
            );
            return;
        }
        classType->baseClass = baseClass;
    }
    
    // (AR) إضافة الحقول
    // (EN) Add fields
    for (const auto& fieldNode : node->fields) {
        // (AR) حساب القيمة الافتراضية
        // (EN) Calculate default value
        Value defaultValue;
        if (fieldNode->defaultValue) {
            defaultValue = executeExpression(fieldNode->defaultValue.get());
        }
        
        // (AR) الحصول على النوع (مبسط - يمكن تحسينه)
        // (EN) Get type (simplified - can be improved)
        Type* fieldType = getType(fieldNode->typeName);
        
        // (AR) إضافة الحقل
        // (EN) Add field
        classType->addField(
            fieldNode->fieldName,
            fieldType,
            fieldNode->visibility,
            fieldNode->isStatic,
            defaultValue
        );
    }
    
    // (AR) إضافة الدوال
    // (EN) Add methods
    for (const auto& methodNode : node->methods) {
        ClassMethod method;
        method.name = methodNode->methodName;
        method.visibility = methodNode->visibility;
        method.isStatic = methodNode->isStatic;
        method.isVirtual = methodNode->isVirtual;
        method.isAbstract = methodNode->isAbstract;
        
        // (AR) إضافة المعاملات
        // (EN) Add parameters
        for (const auto& param : methodNode->parameters) {
            method.parameters.push_back({param.name, getType(param.typeName)});
        }
        
        // (AR) تعيين نوع الإرجاع
        // (EN) Set return type
        if (!methodNode->returnTypeName.empty()) {
            method.returnType = getType(methodNode->returnTypeName);
        }
        
        // (AR) حفظ AST للتنفيذ لاحقاً
        // (EN) Save AST for later execution
        method.astNode = methodNode.get();
        
        classType->addMethod(method);
    }
    
    // (AR) تعيين البانيات
    // (EN) Set constructors
    for (const auto& ctorNode : node->constructors) {
        // (AR) حفظ أول باني (يمكن تحسينه لدعم التحميل الزائد)
        // (EN) Save first constructor (can improve for overloading)
        if (!classType->hasConstructor()) {
            classType->setConstructor(ctorNode.get());
        }
    }
    
    // (AR) تعيين الهادم
    // (EN) Set destructor
    if (node->destructor) {
        classType->setDestructor(node->destructor.get());
    }
    
    // (AR) تسجيل الصنف
    // (EN) Register class
    classMgr->registerClass(std::move(classType));
    
    return Value(); // void
}

// ======================================================================
// تنفيذ تعبير إنشاء كائن / Execute New Expression
// ======================================================================

Value InterpreterClasses::executeNewExpression(NewExpression* node) {
    // (AR) إنشاء كائن جديد من صنف
    // (EN) Create new object from class
    
    ObjectManager* objMgr = ObjectManager::getInstance();
    ClassManager* classMgr = ClassManager::getInstance();
    
    // (AR) التحقق من وجود الصنف
    // (EN) Check class exists
    if (!classMgr->hasClass(node->className)) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_CLASS,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الصنف '" + node->className + "' غير موجود",
            "Class '" + node->className + "' not found"
        );
        return Value();
    }
    
    // (AR) إنشاء الكائن
    // (EN) Create object
    ObjectInstance* obj = objMgr->createObject(node->className);
    
    if (!obj) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_NULL_REFERENCE,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "فشل إنشاء كائن من الصنف '" + node->className + "'",
            "Failed to create object from class '" + node->className + "'"
        );
        return Value();
    }
    
    // (AR) حساب معاملات الباني
    // (EN) Evaluate constructor arguments
    std::vector<Value> args;
    for (const auto& argExpr : node->constructorArgs) {
        args.push_back(executeExpression(argExpr.get()));
    }
    
    // (AR) استدعاء الباني
    // (EN) Call constructor
    callConstructor(obj, args);
    
    // (AR) إرجاع مرجع للكائن
    // (EN) Return object reference
    return Value(obj);
}

// ======================================================================
// استدعاء باني / Call Constructor
// ======================================================================

void InterpreterClasses::callConstructor(ObjectInstance* obj, const std::vector<Value>& args) {
    // (AR) استدعاء باني الكائن
    // (EN) Call object constructor
    
    ClassType* classType = obj->classType;
    
    // (AR) استدعاء باني الصنف الأساسي أولاً
    // (EN) Call base class constructor first
    if (classType->baseClass && obj->getBaseInstance()) {
        callConstructor(obj->getBaseInstance(), {});
    }
    
    // (AR) استدعاء باني الصنف الحالي
    // (EN) Call current class constructor
    if (classType->hasConstructor()) {
        ConstructorDeclaration* ctorNode = 
            static_cast<ConstructorDeclaration*>(classType->constructor);
        
        // (AR) حفظ السياق الحالي
        // (EN) Save current context
        pushScope();
        currentObject_ = obj;
        
        // (AR) ربط المعاملات
        // (EN) Bind parameters
        for (size_t i = 0; i < ctorNode->parameters.size() && i < args.size(); i++) {
            setVariable(ctorNode->parameters[i].name, args[i]);
        }
        
        // (AR) تنفيذ جسم الباني
        // (EN) Execute constructor body
        if (ctorNode->body) {
            executeStatement(ctorNode->body.get());
        }
        
        // (AR) استعادة السياق
        // (EN) Restore context
        currentObject_ = nullptr;
        popScope();
    }
    
    // (AR) تمييز الكائن كمُنشأ
    // (EN) Mark object as constructed
    obj->markConstructed();
}

// ======================================================================
// استدعاء هادم / Call Destructor
// ======================================================================

void InterpreterClasses::callDestructor(ObjectInstance* obj) {
    // (AR) استدعاء هادم الكائن
    // (EN) Call object destructor
    
    ClassType* classType = obj->classType;
    
    // (AR) استدعاء هادم الصنف الحالي
    // (EN) Call current class destructor
    if (classType->hasDestructor()) {
        DestructorDeclaration* dtorNode = 
            static_cast<DestructorDeclaration*>(classType->destructor);
        
        // (AR) حفظ السياق الحالي
        // (EN) Save current context
        pushScope();
        currentObject_ = obj;
        
        // (AR) تنفيذ جسم الهادم
        // (EN) Execute destructor body
        if (dtorNode->body) {
            executeStatement(dtorNode->body.get());
        }
        
        // (AR) استعادة السياق
        // (EN) Restore context
        currentObject_ = nullptr;
        popScope();
    }
    
    // (AR) استدعاء هادم الصنف الأساسي
    // (EN) Call base class destructor
    if (classType->baseClass && obj->getBaseInstance()) {
        callDestructor(obj->getBaseInstance());
    }
}

// ======================================================================
// تنفيذ الوصول للحقل / Execute Field Access
// ======================================================================

Value InterpreterClasses::executeFieldAccess(ObjectInstance* obj, const std::string& fieldName) {
    // (AR) الوصول لحقل في كائن
    // (EN) Access field in object
    
    // (AR) التحقق من وجود الحقل
    // (EN) Check field exists
    if (!obj->hasField(fieldName)) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الحقل '" + fieldName + "' غير موجود في الكائن",
            "Field '" + fieldName + "' not found in object"
        );
        return Value();
    }
    
    // (AR) التحقق من الرؤية
    // (EN) Check visibility
    if (!checkFieldVisibility(obj, fieldName)) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_PRIVATE_ACCESS,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الحقل '" + fieldName + "' غير مرئي في السياق الحالي",
            "Field '" + fieldName + "' not visible in current context"
        );
        return Value();
    }
    
    // (AR) الحصول على القيمة
    // (EN) Get value
    Value* value = obj->getField(fieldName);
    if (!value) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::RUN_NULL_REFERENCE,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "فشل الوصول للحقل '" + fieldName + "'",
            "Failed to access field '" + fieldName + "'"
        );
        return Value();
    }
    
    return *value;
}

// ======================================================================
// تعيين قيمة حقل / Set Field Value
// ======================================================================

void InterpreterClasses::setFieldValue(ObjectInstance* obj, const std::string& fieldName,
                                       const Value& value) {
    // (AR) تعيين قيمة لحقل في كائن
    // (EN) Set value for field in object
    
    // (AR) التحقق من وجود الحقل
    // (EN) Check field exists
    if (!obj->hasField(fieldName)) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الحقل '" + fieldName + "' غير موجود في الكائن",
            "Field '" + fieldName + "' not found in object"
        );
        return;
    }
    
    // (AR) التحقق من الرؤية
    // (EN) Check visibility
    if (!checkFieldVisibility(obj, fieldName)) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_PRIVATE_ACCESS,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الحقل '" + fieldName + "' غير مرئي في السياق الحالي",
            "Field '" + fieldName + "' not visible in current context"
        );
        return;
    }
    
    // (AR) تعيين القيمة
    // (EN) Set value
    obj->setField(fieldName, value);
}

// ======================================================================
// تنفيذ استدعاء دالة / Execute Method Call
// ======================================================================

Value InterpreterClasses::executeMethodCall(ObjectInstance* obj, const std::string& methodName,
                                            const std::vector<Value>& args) {
    // (AR) استدعاء دالة في كائن
    // (EN) Call method in object
    
    ClassType* classType = obj->classType;
    
    // (AR) البحث عن الدالة
    // (EN) Find method
    const ClassMethod* method = classType->findMethod(methodName);
    
    if (!method) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الدالة '" + methodName + "' غير موجودة في الصنف",
            "Method '" + methodName + "' not found in class"
        );
        return Value();
    }
    
    // (AR) التحقق من الرؤية
    // (EN) Check visibility
    if (!checkMethodVisibility(obj, methodName)) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_PRIVATE_ACCESS,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الدالة '" + methodName + "' غير مرئية في السياق الحالي",
            "Method '" + methodName + "' not visible in current context"
        );
        return Value();
    }
    
    // (AR) حفظ السياق الحالي
    // (EN) Save current context
    pushScope();
    ObjectInstance* savedObject = currentObject_;
    currentObject_ = obj;
    
    // (AR) ربط المعاملات
    // (EN) Bind parameters
    MethodDeclaration* methodNode = static_cast<MethodDeclaration*>(method->astNode);
    
    for (size_t i = 0; i < methodNode->parameters.size() && i < args.size(); i++) {
        setVariable(methodNode->parameters[i].name, args[i]);
    }
    
    // (AR) تنفيذ جسم الدالة
    // (EN) Execute method body
    Value result;
    if (methodNode->body) {
        try {
            executeStatement(methodNode->body.get());
        }
        catch (const ReturnException& ret) {
            result = ret.value;
        }
    }
    
    // (AR) استعادة السياق
    // (EN) Restore context
    currentObject_ = savedObject;
    popScope();
    
    return result;
}

// ======================================================================
// تنفيذ تعبير هذا / Execute This Expression
// ======================================================================

Value InterpreterClasses::executeThisExpression() {
    // (AR) الحصول على الكائن الحالي
    // (EN) Get current object
    
    if (!currentObject_) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "استخدام 'هذا' خارج سياق صنف",
            "'this' used outside class context"
        );
        return Value();
    }
    
    return Value(currentObject_);
}

// ======================================================================
// تنفيذ تعبير الأساس / Execute Super Expression
// ======================================================================

Value InterpreterClasses::executeSuperExpression() {
    // (AR) الحصول على الكائن الأساسي
    // (EN) Get base object
    
    if (!currentObject_) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "استخدام 'الأساس' خارج سياق صنف",
            "'super' used outside class context"
        );
        return Value();
    }
    
    ObjectInstance* baseObj = currentObject_->getBaseInstance();
    
    if (!baseObj) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_CLASS,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الصنف الحالي لا يرث من صنف آخر",
            "Current class does not inherit from another class"
        );
        return Value();
    }
    
    return Value(baseObj);
}

// ======================================================================
// الوصول للحقل الثابت / Access Static Field
// ======================================================================

Value InterpreterClasses::getStaticField(const std::string& className, 
                                         const std::string& fieldName) {
    // (AR) الوصول لحقل ثابت
    // (EN) Access static field
    
    ClassManager* classMgr = ClassManager::getInstance();
    ClassType* classType = classMgr->getClass(className);
    
    if (!classType) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_CLASS,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الصنف '" + className + "' غير موجود",
            "Class '" + className + "' not found"
        );
        return Value();
    }
    
    Value* value = classType->getStaticField(fieldName);
    if (!value) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الحقل الثابت '" + fieldName + "' غير موجود",
            "Static field '" + fieldName + "' not found"
        );
        return Value();
    }
    
    return *value;
}

// ======================================================================
// تعيين الحقل الثابت / Set Static Field
// ======================================================================

void InterpreterClasses::setStaticField(const std::string& className,
                                        const std::string& fieldName,
                                        const Value& value) {
    // (AR) تعيين قيمة لحقل ثابت
    // (EN) Set value for static field
    
    ClassManager* classMgr = ClassManager::getInstance();
    ClassType* classType = classMgr->getClass(className);
    
    if (!classType) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_CLASS,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الصنف '" + className + "' غير موجود",
            "Class '" + className + "' not found"
        );
        return;
    }
    
    classType->setStaticField(fieldName, value);
}

// ======================================================================
// استدعاء دالة ثابتة / Call Static Method
// ======================================================================

Value InterpreterClasses::callStaticMethod(const std::string& className,
                                           const std::string& methodName,
                                           const std::vector<Value>& args) {
    // (AR) استدعاء دالة ثابتة
    // (EN) Call static method
    
    ClassManager* classMgr = ClassManager::getInstance();
    ClassType* classType = classMgr->getClass(className);
    
    if (!classType) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_CLASS,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الصنف '" + className + "' غير موجود",
            "Class '" + className + "' not found"
        );
        return Value();
    }
    
    // (AR) البحث عن الدالة
    // (EN) Find method
    const ClassMethod* method = classType->findMethod(methodName);
    
    if (!method || !method->isStatic) {
        Sad::Errors::ErrorManager::getInstance().reportError(
            Sad::Errors::ErrorCode::SEM_UNDEFINED_FUNCTION,
            Sad::Errors::SourceLocation("<runtime>", 0, 0),
            "الدالة الثابتة '" + methodName + "' غير موجودة",
            "Static method '" + methodName + "' not found"
        );
        return Value();
    }
    
    // (AR) حفظ السياق الحالي
    // (EN) Save current context
    pushScope();
    
    // (AR) ربط المعاملات
    // (EN) Bind parameters
    MethodDeclaration* methodNode = static_cast<MethodDeclaration*>(method->astNode);
    
    for (size_t i = 0; i < methodNode->parameters.size() && i < args.size(); i++) {
        setVariable(methodNode->parameters[i].name, args[i]);
    }
    
    // (AR) تنفيذ جسم الدالة
    // (EN) Execute method body
    Value result;
    if (methodNode->body) {
        try {
            executeStatement(methodNode->body.get());
        }
        catch (const ReturnException& ret) {
            result = ret.value;
        }
    }
    
    // (AR) استعادة السياق
    // (EN) Restore context
    popScope();
    
    return result;
}

// ======================================================================
// فحص الرؤية / Visibility Checking
// ======================================================================

bool InterpreterClasses::checkFieldVisibility(ObjectInstance* obj, 
                                               const std::string& fieldName) const {
    // (AR) فحص ما إذا كان الحقل مرئياً في السياق الحالي
    // (EN) Check if field is visible in current context
    
    return obj->classType->isFieldAccessible(fieldName, currentObject_ == obj);
}

bool InterpreterClasses::checkMethodVisibility(ObjectInstance* obj,
                                                const std::string& methodName) const {
    // (AR) فحص ما إذا كانت الدالة مرئية في السياق الحالي
    // (EN) Check if method is visible in current context
    
    return obj->classType->isMethodAccessible(methodName, currentObject_ == obj);
}

// ======================================================================
// دوال مساعدة / Helper Functions
// ======================================================================

Type* InterpreterClasses::getType(const std::string& typeName) {
    // (AR) الحصول على نوع بالاسم
    // (EN) Get type by name
    
    // (AR) ملاحظة: دالة مبسطة، يجب الربط بنظام الأنواع الكامل
    // (EN) Note: Simplified, should connect to full type system
    
    // (AR) فحص الأنواع الأساسية
    // (EN) Check basic types
    static Type intType("int");
    static Type floatType("float");
    static Type stringType("string");
    static Type boolType("bool");
    
    if (typeName == "int") return &intType;
    if (typeName == "float") return &floatType;
    if (typeName == "string") return &stringType;
    if (typeName == "bool") return &boolType;
    
    // (AR) فحص أصناف مُعرّفة
    // (EN) Check user-defined classes
    ClassManager* classMgr = ClassManager::getInstance();
    ClassType* classType = classMgr->getClass(typeName);
    
    if (classType) {
        return classType;
    }
    
    Sad::Errors::ErrorManager::getInstance().reportError(
        Sad::Errors::ErrorCode::SEM_UNDEFINED_CLASS,
        Sad::Errors::SourceLocation("<runtime>", 0, 0),
        "النوع '" + typeName + "' غير معروف",
        "Type '" + typeName + "' is unknown"
    );
    return nullptr;
}

void InterpreterClasses::pushScope() {
    // (AR) إنشاء نطاق جديد
    // (EN) Create new scope
    
    scopes_.push_back({});
}

void InterpreterClasses::popScope() {
    // (AR) حذف النطاق الحالي
    // (EN) Remove current scope
    
    if (!scopes_.empty()) {
        scopes_.pop_back();
    }
}

void InterpreterClasses::setVariable(const std::string& name, const Value& value) {
    // (AR) تعيين قيمة متغير في النطاق الحالي
    // (EN) Set variable value in current scope
    
    if (scopes_.empty()) {
        pushScope();
    }
    
    scopes_.back()[name] = value;
}

Value InterpreterClasses::getVariable(const std::string& name) const {
    // (AR) الحصول على قيمة متغير
    // (EN) Get variable value
    
    // (AR) البحث من النطاق الأحدث للأقدم
    // (EN) Search from newest to oldest scope
    for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second;
        }
    }
    
    Sad::Errors::ErrorManager::getInstance().reportError(
        Sad::Errors::ErrorCode::SEM_UNDEFINED_VARIABLE,
        Sad::Errors::SourceLocation("<runtime>", 0, 0),
        "المتغير '" + name + "' غير معرّف",
        "Variable '" + name + "' is undefined"
    );
    return Value(); // Return null value
}

Value InterpreterClasses::executeExpression(Expression* expr) {
    // (AR) تنفيذ تعبير عام
    // (EN) Execute general expression
    
    // (AR) ملاحظة: يجب الربط بالمفسر الرئيسي
    // (EN) Note: Should connect to main interpreter
    
    // (AR) فحص التعبيرات الخاصة بالأصناف
    // (EN) Check class-specific expressions
    
    if (auto* newExpr = dynamic_cast<NewExpression*>(expr)) {
        return executeNewExpression(newExpr);
    }
    else if (auto* thisExpr = dynamic_cast<ThisExpression*>(expr)) {
        return executeThisExpression();
    }
    else if (auto* superExpr = dynamic_cast<SuperExpression*>(expr)) {
        return executeSuperExpression();
    }
    else if (auto* memberAccess = dynamic_cast<MemberAccessExpression*>(expr)) {
        Value objValue = executeExpression(memberAccess->object.get());
        ObjectInstance* obj = objValue.asObject();
        return executeFieldAccess(obj, memberAccess->memberName);
    }
    else if (auto* methodCall = dynamic_cast<MethodCallExpression*>(expr)) {
        Value objValue = executeExpression(methodCall->object.get());
        ObjectInstance* obj = objValue.asObject();
        
        std::vector<Value> args;
        for (const auto& argExpr : methodCall->arguments) {
            args.push_back(executeExpression(argExpr.get()));
        }
        
        return executeMethodCall(obj, methodCall->methodName, args);
    }
    
    Sad::Errors::ErrorManager::getInstance().reportError(
        Sad::Errors::ErrorCode::SEM_INVALID_OPERATION,
        Sad::Errors::SourceLocation("<runtime>", 0, 0),
        "تعبير غير مدعوم",
        "Unsupported expression"
    );
    return Value();
}

void InterpreterClasses::executeStatement(Statement* stmt) {
    // (AR) تنفيذ جملة
    // (EN) Execute statement
    
    // (AR) ملاحظة: يجب الربط بالمفسر الرئيسي
    // (EN) Note: Should connect to main interpreter
    
    // (AR) تنفيذ بسيط - يجب توسيعه
    // (EN) Simple execution - should be expanded
}

} // namespace Interpreter
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
