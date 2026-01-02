// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_registry.cpp
// File: type_registry.cpp
//
// الوصف: تنفيذ سجل مركزي لجميع الأنواع
// Description: Implementation of central registry for all types
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 2 يناير 2026
// Date: January 2, 2026
// ════════════════════════════════════════════════════════════════════════════════

#include "../include/type_registry.h"  // استيراد التعريف / Import definition
#include <functional>                  // لـ std::hash / For std::hash

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ Singleton Pattern
// Singleton Pattern Implementation
// ════════════════════════════════════════════════════════════════════════════════

// الحصول على المثيل الوحيد / Get singleton instance
TypeRegistry& TypeRegistry::getInstance() {
    // static محلي thread-safe في C++11+ / Local static is thread-safe in C++11+
    static TypeRegistry instance;
    
    // إرجاع المثيل / Return instance
    return instance;
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ المُنشئ / Constructor Implementation
// ════════════════════════════════════════════════════════════════════════════════

// المنشئ الخاص / Private constructor
TypeRegistry::TypeRegistry() {
    // تهيئة الأنواع البدائية / Initialize primitive types
    initializePrimitiveTypes();
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ التهيئة / Initialization Implementation
// ════════════════════════════════════════════════════════════════════════════════

// تهيئة cache الأنواع البدائية / Initialize primitive type cache
void TypeRegistry::initializePrimitiveTypes() {
    // إنشاء الأنواع البدائية / Create primitive types
    void_type_ = std::make_shared<PrimitiveType>(TypeKind::Void);      // فراغ
    integer_type_ = std::make_shared<PrimitiveType>(TypeKind::Integer); // رقم
    float_type_ = std::make_shared<PrimitiveType>(TypeKind::Float);    // عشري
    boolean_type_ = std::make_shared<PrimitiveType>(TypeKind::Boolean); // منطقي
    string_type_ = std::make_shared<PrimitiveType>(TypeKind::String);  // نص
    
    // إنشاء الأنواع الخاصة / Create special types
    any_type_ = std::make_shared<PrimitiveType>(TypeKind::Any);        // أي
    never_type_ = std::make_shared<PrimitiveType>(TypeKind::Never);    // أبداً
    unknown_type_ = std::make_shared<PrimitiveType>(TypeKind::Unknown); // مجهول
    
    // إضافة الأنواع إلى الخريطة / Add types to map
    type_map_[hashType(void_type_)] = void_type_;
    type_map_[hashType(integer_type_)] = integer_type_;
    type_map_[hashType(float_type_)] = float_type_;
    type_map_[hashType(boolean_type_)] = boolean_type_;
    type_map_[hashType(string_type_)] = string_type_;
    type_map_[hashType(any_type_)] = any_type_;
    type_map_[hashType(never_type_)] = never_type_;
    type_map_[hashType(unknown_type_)] = unknown_type_;
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ الحصول على الأنواع البدائية / Primitive Type Getters Implementation
// ════════════════════════════════════════════════════════════════════════════════

// الحصول على نوع فراغ / Get Void type
TypePtr TypeRegistry::getVoidType() {
    // إرجاع من cache / Return from cache
    return void_type_;
}

// الحصول على نوع رقم / Get Integer type
TypePtr TypeRegistry::getIntegerType() {
    // إرجاع من cache / Return from cache
    return integer_type_;
}

// الحصول على نوع عشري / Get Float type
TypePtr TypeRegistry::getFloatType() {
    // إرجاع من cache / Return from cache
    return float_type_;
}

// الحصول على نوع منطقي / Get Boolean type
TypePtr TypeRegistry::getBooleanType() {
    // إرجاع من cache / Return from cache
    return boolean_type_;
}

// الحصول على نوع نص / Get String type
TypePtr TypeRegistry::getStringType() {
    // إرجاع من cache / Return from cache
    return string_type_;
}

// الحصول على نوع Any / Get Any type
TypePtr TypeRegistry::getAnyType() {
    // إرجاع من cache / Return from cache
    return any_type_;
}

// الحصول على نوع Never / Get Never type
TypePtr TypeRegistry::getNeverType() {
    // إرجاع من cache / Return from cache
    return never_type_;
}

// الحصول على نوع Unknown / Get Unknown type
TypePtr TypeRegistry::getUnknownType() {
    // إرجاع من cache / Return from cache
    return unknown_type_;
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ Type Interning
// Type Interning Implementation
// ════════════════════════════════════════════════════════════════════════════════

// الحصول على أو إنشاء نوع بدائي / Get or create primitive type
TypePtr TypeRegistry::internPrimitiveType(TypeKind kind) {
    // استخدام switch للأنواع المختلفة / Use switch for different kinds
    switch (kind) {
        case TypeKind::Void:
            return getVoidType();      // فراغ
        case TypeKind::Integer:
            return getIntegerType();   // رقم
        case TypeKind::Float:
            return getFloatType();     // عشري
        case TypeKind::Boolean:
            return getBooleanType();   // منطقي
        case TypeKind::String:
            return getStringType();    // نص
        case TypeKind::Any:
            return getAnyType();       // أي
        case TypeKind::Never:
            return getNeverType();     // أبداً
        case TypeKind::Unknown:
            return getUnknownType();   // مجهول
        default:
            // للأنواع الأخرى، استخدم internType / For other types, use internType
            return internType(std::make_shared<PrimitiveType>(kind));
    }
}

// الحصول على أو إنشاء نوع عام / Get or create type with interning
TypePtr TypeRegistry::internType(TypePtr type) {
    // التحقق من null / Check for null
    if (!type) {
        return nullptr; // إرجاع null إذا كان المدخل null / Return null if input is null
    }
    
    // قفل mutex للـ thread safety / Lock mutex for thread safety
    std::lock_guard<std::mutex> lock(mutex_);
    
    // حساب hash / Calculate hash
    size_t hash = hashType(type);
    
    // البحث في الخريطة / Search in map
    auto it = type_map_.find(hash);
    
    // إذا وُجد، إرجاع النوع الموجود / If found, return existing type
    if (it != type_map_.end()) {
        return it->second; // إرجاع من cache / Return from cache
    }
    
    // إضافة نوع جديد / Add new type
    type_map_[hash] = type;
    
    // إرجاع النوع / Return type
    return type;
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ Hash / Hash Implementation
// ════════════════════════════════════════════════════════════════════════════════

// حساب hash لنوع / Calculate hash for a type
size_t TypeRegistry::hashType(const TypePtr& type) const {
    // التحقق من null / Check for null
    if (!type) {
        return 0; // hash = 0 للـ null / hash = 0 for null
    }
    
    // بدء بـ hash نوع النوع / Start with kind hash
    size_t hash = std::hash<int>{}(static_cast<int>(type->getKind()));
    
    // للأنواع البدائية، النوع كافٍ / For primitive types, kind is sufficient
    if (type->isPrimitive() || type->isVoid() || 
        type->isAny() || type->isNever() || type->isUnknown()) {
        return hash; // إرجاع hash النوع / Return kind hash
    }
    
    // للأنواع المركبة، نحتاج معلومات إضافية / For composite types, need more info
    // TODO: إضافة hash للمعلومات الإضافية في الأنواع المركبة
    // TODO: Add hash for additional info in composite types
    
    // إرجاع hash / Return hash
    return hash;
}

// ════════════════════════════════════════════════════════════════════════════════
// تنفيذ الإحصائيات / Statistics Implementation
// ════════════════════════════════════════════════════════════════════════════════

// الحصول على عدد الأنواع المسجلة / Get number of registered types
size_t TypeRegistry::getTypeCount() const {
    // قفل mutex / Lock mutex
    std::lock_guard<std::mutex> lock(mutex_);
    
    // إرجاع حجم الخريطة / Return map size
    return type_map_.size();
}

// الحصول على عدد الأنواع البدائية / Get primitive type count
size_t TypeRegistry::getPrimitiveTypeCount() const {
    // الأنواع البدائية المُعدة مسبقاً: 8 أنواع / Pre-cached primitive types: 8 types
    // (Void, Integer, Float, Boolean, String, Any, Never, Unknown)
    return 8;
}

// مسح السجل (للاختبارات فقط) / Clear registry (for tests only)
void TypeRegistry::clear() {
    // قفل mutex / Lock mutex
    std::lock_guard<std::mutex> lock(mutex_);
    
    // مسح الخريطة / Clear map
    type_map_.clear();
    
    // إعادة تهيئة / Reinitialize
    initializePrimitiveTypes();
}

// إعادة تهيئة السجل / Reinitialize registry
void TypeRegistry::reset() {
    // استدعاء clear / Call clear
    clear();
}

} // namespace TypeSystem
} // namespace Sad
