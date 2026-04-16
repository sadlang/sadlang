// ======================================================================
// class_type.cpp - تنفيذ نوع بيانات الصنف / Class Type Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ جميع دوال ClassType المُعرّفة في class_type.h
//   يشمل: إدارة الخصائص، الطرق، الوراثة، إنشاء الكائنات
//
// English Description:
//   Implementation of all ClassType functions defined in class_type.h
//   Includes: field management, methods, inheritance, object creation
// ======================================================================

#include "class_type.h"
#include "class_nodes.h"
#include "object_instance.h"
#include <algorithm>
#include <iostream>
#include <sstream>

namespace Sad {
namespace Data {

// ======================================================================
// المنشئات / Constructors
// ======================================================================

ClassType::ClassType(const std::string& className)
    : Type(className), name(className), baseClass(nullptr) {
    // (AR) إنشاء صنف جديد
    // (EN) Create new class
}

ClassType::ClassType(const std::string& className, ClassType* base)
    : Type(className), name(className), baseClass(base) {
    // (AR) إنشاء صنف جديد مع صنف أساسي
    // (EN) Create new class with base class
}

// ======================================================================
// واجهة Type / Type Interface
// ======================================================================

bool ClassType::isEqual(const Type* other) const {
    // (AR) فحص المساواة مع نوع آخر
    // (EN) Check equality with another type
    
    if (!other) return false;
    
    // (AR) يجب أن يكون النوع الآخر ClassType أيضاً
    // (EN) Other type must also be ClassType
    const ClassType* otherClass = dynamic_cast<const ClassType*>(other);
    if (!otherClass) return false;
    
    // (AR) المساواة بالاسم
    // (EN) Equality by name
    return name == otherClass->name;
}

bool ClassType::isConvertibleTo(const Type* other) const {
    // (AR) فحص إمكانية التحويل لنوع آخر
    // (EN) Check if convertible to another type
    
    if (!other) return false;
    
    const ClassType* otherClass = dynamic_cast<const ClassType*>(other);
    if (!otherClass) return false;
    
    // (AR) نفس الصنف أو يرث منه (upcast)
    // (EN) Same class or inherits from it (upcast)
    return inheritsFrom(otherClass);
}

// ======================================================================
// إدارة الخصائص / Field Management
// ======================================================================

bool ClassType::addField(const std::string& fieldName, Type* type,
                         AST::Visibility visibility, bool isStatic,
                         const Value& defaultValue) {
    // (AR) إضافة خاصية جديدة
    // (EN) Add new field
    
    // (AR) فحص عدم وجود اسم مكرر
    // (EN) Check for duplicate name
    if (fieldIndex.find(fieldName) != fieldIndex.end()) {
        return false;
    }
    
    // (AR) إنشاء الخاصية
    // (EN) Create field
    ClassField field(fieldName, type, visibility);
    field.isStatic = isStatic;
    field.defaultValue = defaultValue;
    
    // (AR) إضافة للقائمة والفهرس
    // (EN) Add to list and index
    size_t index = fields.size();
    fields.push_back(field);
    fieldIndex[fieldName] = index;
    
    // (AR) إذا كانت ثابتة، تهيئة قيمتها
    // (EN) If static, initialize its value
    if (isStatic) {
        staticFields[fieldName] = defaultValue;
    }
    
    return true;
}

ClassField* ClassType::findField(const std::string& fieldName) {
    // (AR) البحث عن خاصية في الصنف الحالي
    // (EN) Find field in current class
    
    auto it = fieldIndex.find(fieldName);
    if (it != fieldIndex.end()) {
        return &fields[it->second];
    }
    
    // (AR) البحث في الصنف الأساسي
    // (EN) Search in base class
    if (baseClass) {
        auto* result = baseClass->findField(fieldName);
        if (result) return result;
    }
    
    // (AR) البحث في الأصناف الأساسية الإضافية (وراثة متعددة)
    // (EN) Search in additional base classes (multiple inheritance)
    for (auto* base : additionalBases) {
        if (base) {
            auto* result = base->findField(fieldName);
            if (result) return result;
        }
    }
    
    return nullptr;
}

Value* ClassType::getStaticField(const std::string& fieldName) {
    // (AR) الحصول على قيمة خاصية ثابتة
    // (EN) Get static field value
    
    auto it = staticFields.find(fieldName);
    if (it != staticFields.end()) {
        return &it->second;
    }
    
    // (AR) البحث في الصنف الأساسي
    // (EN) Search in base class
    if (baseClass) {
        auto* result = baseClass->getStaticField(fieldName);
        if (result) return result;
    }
    
    // (AR) البحث في الأصناف الأساسية الإضافية
    // (EN) Search in additional base classes
    for (auto* base : additionalBases) {
        if (base) {
            auto* result = base->getStaticField(fieldName);
            if (result) return result;
        }
    }
    
    return nullptr;
}

bool ClassType::setStaticField(const std::string& fieldName, const Value& value) {
    // (AR) تعيين قيمة خاصية ثابتة
    // (EN) Set static field value
    
    // (AR) فحص وجود الخاصية
    // (EN) Check if field exists
    ClassField* field = findField(fieldName);
    if (!field || !field->isStatic) {
        return false;
    }
    
    // (AR) تعيين القيمة
    // (EN) Set value
    staticFields[fieldName] = value;
    return true;
}

// ======================================================================
// إدارة الطرق / Method Management
// ======================================================================

bool ClassType::addMethod(const std::string& methodName, AST::Visibility visibility,
                          Type* returnType, const std::vector<AST::Parameter>& parameters,
                          std::unique_ptr<AST::BlockStmt> body,
                          bool isStatic, bool isVirtual, bool isAbstract) {
    // (AR) إضافة طريقة جديدة
    // (EN) Add new method
    
    // (AR) فحص عدم وجود اسم مكرر (يُسمح بالـ overloading لاحقاً)
    // (EN) Check for duplicate name (overloading allowed later)
    if (methodIndex.find(methodName) != methodIndex.end()) {
        return false;
    }
    
    // (AR) إنشاء الطريقة
    // (EN) Create method
    ClassMethod method(methodName, visibility, returnType);
    method.parameters = parameters;
    method.body = std::move(body);
    method.isStatic = isStatic;
    method.isVirtual = isVirtual;
    method.isAbstract = isAbstract;
    
    // (AR) إضافة للقائمة والفهرس
    // (EN) Add to list and index
    size_t index = methods.size();
    methods.push_back(std::move(method));
    methodIndex[methodName] = index;
    
    return true;
}

bool ClassType::addDefaultMethod(const std::string& methodName, AST::Visibility visibility,
                                Type* returnType, const std::vector<AST::Parameter>& parameters,
                                std::shared_ptr<AST::BlockStmt> sharedBody,
                                bool isStatic, bool isVirtual) {
    // (AR) إضافة طريقة بجسم مشترك (من سمة افتراضية)
    // (EN) Add method with shared body (from trait default)
    
    if (methodIndex.find(methodName) != methodIndex.end()) {
        return false;
    }
    
    ClassMethod method(methodName, visibility, returnType);
    method.parameters = parameters;
    method.sharedBody = std::move(sharedBody);
    method.isStatic = isStatic;
    method.isVirtual = isVirtual;
    method.isAbstract = false;
    
    size_t index = methods.size();
    methods.push_back(std::move(method));
    methodIndex[methodName] = index;
    
    return true;
}

ClassMethod* ClassType::findMethod(const std::string& methodName) {
    // (AR) البحث عن طريقة
    // (EN) Find method
    
    auto it = methodIndex.find(methodName);
    if (it != methodIndex.end()) {
        return &methods[it->second];
    }
    
    // (AR) البحث في الصنف الأساسي
    // (EN) Search in base class
    if (baseClass) {
        auto* result = baseClass->findMethod(methodName);
        if (result) return result;
    }
    
    // (AR) البحث في الأصناف الأساسية الإضافية
    // (EN) Search in additional base classes
    for (auto* base : additionalBases) {
        if (base) {
            auto* result = base->findMethod(methodName);
            if (result) return result;
        }
    }
    
    return nullptr;
}

// ======================================================================
// إدارة الخصائص (Properties) / Property Management
// ======================================================================

void ClassType::addProperty(ClassProperty property) {
    // (AR) إضافة خاصية (Property) للصنف
    // (EN) Add property to class
    
    size_t index = properties.size();
    propertyIndex[property.name] = index;
    properties.push_back(std::move(property));
}

ClassProperty* ClassType::findProperty(const std::string& propertyName) {
    // (AR) البحث عن خاصية (Property)
    // (EN) Find property
    
    auto it = propertyIndex.find(propertyName);
    if (it != propertyIndex.end()) {
        return &properties[it->second];
    }
    
    // (AR) البحث في الصنف الأساسي
    // (EN) Search in base class
    if (baseClass) {
        auto* result = baseClass->findProperty(propertyName);
        if (result) return result;
    }
    
    // (AR) البحث في الأصناف الأساسية الإضافية
    // (EN) Search in additional base classes
    for (auto* base : additionalBases) {
        if (base) {
            auto* result = base->findProperty(propertyName);
            if (result) return result;
        }
    }
    
    return nullptr;
}

bool ClassType::hasProperty(const std::string& propertyName) const {
    // (AR) التحقق من وجود خاصية (Property)
    // (EN) Check if property exists
    
    if (propertyIndex.find(propertyName) != propertyIndex.end()) {
        return true;
    }
    
    if (baseClass) {
        if (baseClass->hasProperty(propertyName)) return true;
    }
    
    // (AR) البحث في الأصناف الأساسية الإضافية
    // (EN) Search in additional base classes
    for (auto* base : additionalBases) {
        if (base && base->hasProperty(propertyName)) return true;
    }
    
    return false;
}

// ======================================================================
// إدارة العوامل المحملة زائداً / Operator Overload Management
// ======================================================================

void ClassType::addOperatorOverload(OperatorOverload overload) {
    // (AR) إضافة تحميل عامل زائد للصنف
    // (EN) Add operator overload to class
    
    #ifdef DEBUG_OOP
    std::cout << "[ClassType] أضيف تحميل عامل زائد: " << overload.operatorSymbol << " للصنف " << name << "\n";
    #endif
    
    operatorOverloads.push_back(std::move(overload));
}

OperatorOverload* ClassType::findOperator(const std::string& operatorSymbol) {
    // (AR) البحث عن تحميل عامل زائد بالرمز
    // (EN) Find operator overload by symbol
    
    for (auto& op : operatorOverloads) {
        if (op.operatorSymbol == operatorSymbol) {
            return &op;
        }
    }
    
    // (AR) البحث في الصنف الأساسي
    // (EN) Search in base class
    if (baseClass) {
        auto* result = baseClass->findOperator(operatorSymbol);
        if (result) return result;
    }
    
    // (AR) البحث في الأصناف الأساسية الإضافية
    // (EN) Search in additional base classes
    for (auto* base : additionalBases) {
        if (base) {
            auto* result = base->findOperator(operatorSymbol);
            if (result) return result;
        }
    }
    
    return nullptr;
}

bool ClassType::hasOperator(const std::string& operatorSymbol) const {
    // (AR) التحقق من وجود تحميل عامل زائد
    // (EN) Check if operator overload exists
    
    for (const auto& op : operatorOverloads) {
        if (op.operatorSymbol == operatorSymbol) {
            return true;
        }
    }
    
    if (baseClass) {
        if (baseClass->hasOperator(operatorSymbol)) return true;
    }
    
    // (AR) البحث في الأصناف الأساسية الإضافية
    // (EN) Search in additional base classes
    for (auto* base : additionalBases) {
        if (base && base->hasOperator(operatorSymbol)) return true;
    }
    
    return false;
}

// ======================================================================
// الباني والهدام / Constructor and Destructor
// ======================================================================

void ClassType::setConstructor(std::unique_ptr<AST::ConstructorDecl> ctor) {
    // (AR) تعيين الباني
    // (EN) Set constructor
    constructor = std::move(ctor);
}

void ClassType::setDestructor(std::unique_ptr<AST::DestructorDecl> dtor) {
    // (AR) تعيين الهدام
    // (EN) Set destructor
    destructor = std::move(dtor);
}

// ======================================================================
// الوراثة / Inheritance
// ======================================================================

bool ClassType::inheritsFrom(const ClassType* otherClass) const {
    // (AR) فحص الوراثة من صنف آخر
    // (EN) Check inheritance from another class
    
    if (!otherClass) return false;
    
    // (AR) نفس الصنف
    // (EN) Same class
    if (this == otherClass) return true;
    
    // (AR) فحص الصنف الأساسي
    // (EN) Check base class
    if (baseClass) {
        if (baseClass->inheritsFrom(otherClass)) return true;
    }
    
    // (AR) فحص الأصناف الأساسية الإضافية (الوراثة المتعددة)
    // (EN) Check additional base classes (multiple inheritance)
    for (auto* additionalBase : additionalBases) {
        if (additionalBase && additionalBase->inheritsFrom(otherClass)) return true;
    }
    
    return false;
}

// ======================================================================
// إنشاء كائنات / Object Creation
// ======================================================================

ObjectInstance* ClassType::createInstance() {
    // (AR) إنشاء كائن جديد من هذا الصنف
    // (EN) Create new instance of this class
    
    // (AR) إنشاء معرف فريد
    // (EN) Generate unique ID
    // (AR) استخدام المعرف العام المركزي بدلاً من عداد محلي مكرر
    // (EN) Use centralized global ID generator instead of duplicate local counter
    size_t objectId = generateObjectId();
    
    // (AR) إنشاء الكائن
    // (EN) Create object
    ObjectInstance* instance = new ObjectInstance(this, objectId);
    
    // (AR) إنشاء كائن للصنف الأساسي إذا وُجد
    // (EN) Create base class instance if exists
    if (baseClass) {
        instance->setBaseInstance(
            std::unique_ptr<ObjectInstance>(baseClass->createInstance())
        );
    }
    
    // (AR) تهيئة جميع الخصائص
    // (EN) Initialize all fields
    instance->initializeFields();
    
    return instance;
}

// ======================================================================
// فحص الرؤية / Visibility Checking
// ======================================================================

bool ClassType::isFieldAccessible(const std::string& fieldName, 
                                  const ClassType* fromClass) const {
    // (AR) فحص إمكانية الوصول لخاصية
    // (EN) Check if field is accessible
    
    ClassField* field = const_cast<ClassType*>(this)->findField(fieldName);
    if (!field) return false;
    
    return checkVisibility(field->visibility, fromClass);
}

bool ClassType::isMethodAccessible(const std::string& methodName,
                                   const ClassType* fromClass) const {
    // (AR) فحص إمكانية الوصول لطريقة
    // (EN) Check if method is accessible
    
    ClassMethod* method = const_cast<ClassType*>(this)->findMethod(methodName);
    if (!method) return false;
    
    return checkVisibility(method->visibility, fromClass);
}

bool ClassType::checkVisibility(AST::Visibility vis, const ClassType* fromClass) const {
    // (AR) فحص رؤية عضو
    // (EN) Check member visibility
    
    switch (vis) {
        case AST::Visibility::PUBLIC:
            // (AR) عام - الجميع يصل
            // (EN) Public - everyone can access
            return true;
            
        case AST::Visibility::PRIVATE:
            // (AR) خاص - نفس الصنف فقط
            // (EN) Private - same class only
            return fromClass == this;
            
        case AST::Visibility::PROTECTED:
            // (AR) محمي - نفس الصنف أو الوارثون
            // (EN) Protected - same class or inheritors
            if (!fromClass) return false;
            return fromClass == this || fromClass->inheritsFrom(this);
            
        default:
            return false;
    }
}

// ======================================================================
// أدوات مساعدة / Helper Functions
// ======================================================================

size_t ClassType::getTotalFieldCount() const {
    // (AR) عدد الخصائص الكلي (مع الأصناف الأساسية)
    // (EN) Total field count (including base classes)
    
    size_t count = fields.size();
    if (baseClass) {
        count += baseClass->getTotalFieldCount();
    }
    return count;
}

size_t ClassType::getTotalMethodCount() const {
    // (AR) عدد الطرق الكلي (مع الأصناف الأساسية)
    // (EN) Total method count (including base classes)
    
    size_t count = methods.size();
    if (baseClass) {
        count += baseClass->getTotalMethodCount();
    }
    return count;
}

void ClassType::printDebugInfo() const {
    // (AR) طباعة معلومات الصنف للتصحيح
    // (EN) Print class information for debugging
    
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║   Class: " << name << "\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    
    if (baseClass) {
        std::cout << "║ Base Class: " << baseClass->name << "\n";
    }
    
    std::cout << "║ Fields: " << fields.size() << "\n";
    for (const auto& field : fields) {
        std::cout << "║   - " << field.name << " (" 
                  << (field.isStatic ? "static " : "")
                  << field.type->getName() << ")\n";
    }
    
    std::cout << "║ Methods: " << methods.size() << "\n";
    for (const auto& method : methods) {
        std::cout << "║   - " << method.name << "("
                  << method.parameters.size() << " params)\n";
    }
    
    std::cout << "║ Constructor: " << (constructor ? "Yes" : "No") << "\n";
    std::cout << "║ Destructor: " << (destructor ? "Yes" : "No") << "\n";
    std::cout << "╚════════════════════════════════════════╝\n";
}

} // namespace Data
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
