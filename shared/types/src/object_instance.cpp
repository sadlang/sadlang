// ======================================================================
// object_instance.cpp - تنفيذ كائن نسخة من صنف / Object Instance Implementation
// ======================================================================
// الوصف بالعربية:
//   تنفيذ جميع دوال ObjectInstance المُعرّفة في object_instance.h
//   يشمل: الوصول للخصائص، استدعاء الطرق، الوراثة
//
// English Description:
//   Implementation of all ObjectInstance functions defined in object_instance.h
//   Includes: field access, method calls, inheritance
// ======================================================================

#include "object_instance.h"
#include "class_type.h"
#include <sstream>
#include <iostream>
#include <algorithm>

namespace Sad {
namespace Data {

// عداد عام لتوليد معرفات فريدة
// Global counter for generating unique IDs
static size_t globalObjectIdCounter = 1;

// ======================================================================
// المنشئات والهدامات / Constructors and Destructors
// ======================================================================

ObjectInstance::ObjectInstance(ClassType* cls, size_t id)
    : classType(cls), objectId(id), isConstructed(false) {
    // (AR) إنشاء كائن جديد
    // (EN) Create new object
}

ObjectInstance::~ObjectInstance() {
    // (AR) الهدام - يُستدعى تلقائياً عند حذف الكائن
    // (EN) Destructor - called automatically when object is deleted
    
    // (AR) ملاحظة: استدعاء الهدام المخصص يتم من ObjectManager
    // (EN) Note: custom destructor call is handled by ObjectManager
}

// ======================================================================
// الوصول للخصائص / Field Access
// ======================================================================

Value* ObjectInstance::getField(const std::string& fieldName) {
    // (AR) الحصول على قيمة خاصية
    // (EN) Get field value
    
    // (AR) البحث في الكائن الحالي
    // (EN) Search in current object
    auto it = fields.find(fieldName);
    if (it != fields.end()) {
        return &it->second;
    }
    
    // (AR) البحث في الكائن الأساسي
    // (EN) Search in base object
    if (baseInstance) {
        return baseInstance->getField(fieldName);
    }
    
    return nullptr;
}

bool ObjectInstance::setField(const std::string& fieldName, const Value& value) {
    // (AR) تعيين قيمة خاصية
    // (EN) Set field value
    
    // (AR) فحص وجود الخاصية في تعريف الصنف
    // (EN) Check if field exists in class definition
    ClassField* fieldDef = classType->findField(fieldName);
    if (!fieldDef) {
        return false;
    }
    
    // (AR) فحص أن الخاصية ليست ثابتة
    // (EN) Check that field is not static
    if (fieldDef->isStatic) {
        // (AR) الخصائص الثابتة تُخزّن في الصنف، ليس الكائن
        // (EN) Static fields are stored in class, not object
        return false;
    }
    
    // (AR) تعيين القيمة
    // (EN) Set value
    fields[fieldName] = value;
    return true;
}

bool ObjectInstance::hasField(const std::string& fieldName) const {
    // (AR) فحص وجود خاصية
    // (EN) Check if field exists
    
    if (fields.find(fieldName) != fields.end()) {
        return true;
    }
    
    if (baseInstance) {
        return baseInstance->hasField(fieldName);
    }
    
    return false;
}

std::vector<std::string> ObjectInstance::getFieldNames() const {
    // (AR) الحصول على أسماء جميع الخصائص
    // (EN) Get all field names
    
    std::vector<std::string> names;
    
    // (AR) إضافة خصائص الكائن الحالي
    // (EN) Add current object fields
    for (const auto& pair : fields) {
        names.push_back(pair.first);
    }
    
    // (AR) إضافة خصائص الكائن الأساسي
    // (EN) Add base object fields
    if (baseInstance) {
        auto baseNames = baseInstance->getFieldNames();
        names.insert(names.end(), baseNames.begin(), baseNames.end());
    }
    
    return names;
}

// ======================================================================
// استدعاء الطرق / Method Invocation
// ======================================================================

bool ObjectInstance::hasMethod(const std::string& methodName) const {
    // (AR) فحص وجود طريقة
    // (EN) Check if method exists
    
    return classType->findMethod(methodName) != nullptr;
}

ClassMethod* ObjectInstance::getMethod(const std::string& methodName) {
    // (AR) الحصول على طريقة
    // (EN) Get method
    
    return classType->findMethod(methodName);
}

// ======================================================================
// معلومات الصنف / Class Information
// ======================================================================

bool ObjectInstance::isInstanceOf(const std::string& className) const {
    // (AR) فحص نوع الكائن
    // (EN) Check object type
    
    return classType->name == className || 
           (classType->baseClass && classType->inheritsFrom(classType->baseClass));
}

bool ObjectInstance::isInstanceOf(const ClassType* cls) const {
    // (AR) فحص نوع الكائن
    // (EN) Check object type
    
    if (!cls) return false;
    return classType->inheritsFrom(cls);
}

// ======================================================================
// الوراثة / Inheritance
// ======================================================================

void ObjectInstance::setBaseInstance(std::unique_ptr<ObjectInstance> base) {
    // (AR) تعيين كائن الصنف الأساسي
    // (EN) Set base class instance
    
    baseInstance = std::move(base);
}

// ======================================================================
// تهيئة الخصائص / Field Initialization
// ======================================================================

void ObjectInstance::initializeFields() {
    // (AR) تهيئة جميع الخصائص بقيمها الافتراضية
    // (EN) Initialize all fields with default values
    
    // (AR) تهيئة خصائص الصنف الأساسي أولاً
    // (EN) Initialize base class fields first
    if (baseInstance) {
        baseInstance->initializeFields();
    }
    
    // (AR) تهيئة خصائص الصنف الحالي
    // (EN) Initialize current class fields
    for (const auto& field : classType->fields) {
        if (!field.isStatic) {
            // (AR) الخصائص غير الثابتة تُخزّن في الكائن
            // (EN) Non-static fields are stored in object
            fields[field.name] = field.defaultValue;
        }
    }
}

// ======================================================================
// التمثيل النصي / String Representation
// ======================================================================

std::string ObjectInstance::toString() const {
    // (AR) تحويل الكائن إلى نص
    // (EN) Convert object to string
    
    std::ostringstream oss;
    oss << "<" << classType->name << "#" << objectId << ": ";
    
    bool first = true;
    for (const auto& pair : fields) {
        if (!first) oss << ", ";
        oss << pair.first << "=" << pair.second.toString();
        first = false;
    }
    
    oss << ">";
    return oss.str();
}

void ObjectInstance::printDebugInfo() const {
    // (AR) طباعة معلومات الكائن للتصحيح
    // (EN) Print object information for debugging
    
    std::cout << "╔════════════════════════════════════════╗\n";
    std::cout << "║   Object #" << objectId << " of " << classType->name << "\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║ Constructed: " << (isConstructed ? "Yes" : "No") << "\n";
    std::cout << "║ Fields: " << fields.size() << "\n";
    
    for (const auto& pair : fields) {
        std::cout << "║   - " << pair.first << " = " 
                  << pair.second.toString() << "\n";
    }
    
    if (baseInstance) {
        std::cout << "║ Base Instance: #" << baseInstance->objectId << "\n";
    }
    
    std::cout << "╚════════════════════════════════════════╝\n";
}

// ======================================================================
// معلومات الذاكرة / Memory Information
// ======================================================================

size_t ObjectInstance::getMemoryUsage() const {
    // (AR) حساب حجم الذاكرة المستخدمة تقريبياً
    // (EN) Calculate approximate memory usage
    
    size_t size = sizeof(ObjectInstance);
    
    // (AR) حجم خريطة الخصائص
    // (EN) Size of fields map
    size += fields.size() * (sizeof(std::string) + sizeof(Value));
    
    // (AR) حجم الكائن الأساسي
    // (EN) Size of base instance
    if (baseInstance) {
        size += baseInstance->getMemoryUsage();
    }
    
    return size;
}

// ======================================================================
// دوال مساعدة عامة / Global Helper Functions
// ======================================================================

size_t generateObjectId() {
    // (AR) توليد معرف فريد للكائن
    // (EN) Generate unique object ID
    
    return globalObjectIdCounter++;
}

bool compareObjects(const ObjectInstance* obj1, const ObjectInstance* obj2) {
    // (AR) مقارنة كائنين
    // (EN) Compare two objects
    
    if (!obj1 || !obj2) return false;
    
    // (AR) يجب أن يكونا من نفس الصنف
    // (EN) Must be of same class
    if (obj1->classType != obj2->classType) return false;
    
    // (AR) مقارنة جميع الخصائص
    // (EN) Compare all fields
    if (obj1->fields.size() != obj2->fields.size()) return false;
    
    for (const auto& pair : obj1->fields) {
        auto it = obj2->fields.find(pair.first);
        if (it == obj2->fields.end()) return false;
        
        // (AR) مقارنة القيم (يحتاج تنفيذ Value::operator==)
        // (EN) Compare values (needs Value::operator== implementation)
        // if (pair.second != it->second) return false;
    }
    
    // (AR) مقارنة الكائنات الأساسية
    // (EN) Compare base instances
    if (obj1->baseInstance && obj2->baseInstance) {
        return compareObjects(obj1->baseInstance.get(), obj2->baseInstance.get());
    }
    
    return !obj1->baseInstance && !obj2->baseInstance;
}

} // namespace Data
} // namespace Sad

// ======================================================================
// نهاية الملف / End of File
// ======================================================================
