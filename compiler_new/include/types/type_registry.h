// ════════════════════════════════════════════════════════════════════════════════
// ملف: type_registry.h
// File: type_registry.h
//
// الوصف: سجل مركزي لجميع الأنواع (Type Interning)
// Description: Central registry for all types (Type Interning)
//
// المؤلف: Sad Language Type System
// Author: Sad Language Type System
//
// التاريخ: 2 يناير 2026
// Date: January 2, 2026
// ════════════════════════════════════════════════════════════════════════════════

#ifndef SAD_COMPILER_TYPE_SYSTEM_TYPE_REGISTRY_H
#define SAD_COMPILER_TYPE_SYSTEM_TYPE_REGISTRY_H

#include "type.h"              // استيراد النوع الأساسي / Import base Type
#include "primitive_type.h"     // استيراد الأنواع البدائية / Import primitive types
#include <unordered_map>       // لـ unordered_map / For unordered_map
#include <mutex>               // لـ thread safety / For thread safety

namespace Sad {
namespace TypeSystem {

// ════════════════════════════════════════════════════════════════════════════════
// الصنف: TypeRegistry
// Class: TypeRegistry
//
// الوصف: سجل مركزي يدير جميع الأنواع ويضمن عدم التكرار
// Description: Central registry managing all types and ensuring no duplication
//
// النمط: Singleton + Type Interning
// Pattern: Singleton + Type Interning
// ════════════════════════════════════════════════════════════════════════════════

class TypeRegistry {
public:
    // ═══════════════════════════════════════════════════════════════════════════
    // الحصول على المثيل الوحيد / Get Singleton Instance
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على مثيل TypeRegistry / Get TypeRegistry instance
    static TypeRegistry& getInstance();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // حذف النسخ والتعيين / Delete Copy and Assignment
    // ═══════════════════════════════════════════════════════════════════════════
    
    // حذف copy constructor / Delete copy constructor
    TypeRegistry(const TypeRegistry&) = delete;
    
    // حذف copy assignment / Delete copy assignment
    TypeRegistry& operator=(const TypeRegistry&) = delete;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الأنواع البدائية المُعدة مسبقاً / Pre-cached Primitive Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على نوع فراغ / Get Void type
    TypePtr getVoidType();
    
    // الحصول على نوع رقم / Get Integer type
    TypePtr getIntegerType();
    
    // الحصول على نوع عشري / Get Float type
    TypePtr getFloatType();
    
    // الحصول على نوع منطقي / Get Boolean type
    TypePtr getBooleanType();
    
    // الحصول على نوع نص / Get String type
    TypePtr getStringType();
    
    // الحصول على نوع Any / Get Any type
    TypePtr getAnyType();
    
    // الحصول على نوع Never / Get Never type
    TypePtr getNeverType();
    
    // الحصول على نوع Unknown / Get Unknown type
    TypePtr getUnknownType();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // Type Interning (تجنب التكرار)
    // Type Interning (Avoid Duplication)
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على أو إنشاء نوع بدائي / Get or create primitive type
    TypePtr internPrimitiveType(TypeKind kind);
    
    // الحصول على أو إنشاء نوع عام / Get or create type with interning
    // يستخدم hash للتحقق من التكرار / Uses hash to check for duplication
    TypePtr internType(TypePtr type);
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إحصائيات / Statistics
    // ═══════════════════════════════════════════════════════════════════════════
    
    // الحصول على عدد الأنواع المسجلة / Get number of registered types
    size_t getTypeCount() const;
    
    // الحصول على عدد الأنواع البدائية / Get primitive type count
    size_t getPrimitiveTypeCount() const;
    
    // مسح السجل (للاختبارات فقط) / Clear registry (for tests only)
    void clear();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // إعادة تعيين إلى الحالة الافتراضية / Reset to Default State
    // ═══════════════════════════════════════════════════════════════════════════
    
    // إعادة تهيئة السجل / Reinitialize registry
    void reset();
    
private:
    // ═══════════════════════════════════════════════════════════════════════════
    // المُنشئ الخاص / Private Constructor
    // ═══════════════════════════════════════════════════════════════════════════
    
    // منشئ خاص (Singleton) / Private constructor (Singleton)
    TypeRegistry();
    
    // مُدمر خاص / Private destructor
    ~TypeRegistry() = default;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // تهيئة الأنواع البدائية / Initialize Primitive Types
    // ═══════════════════════════════════════════════════════════════════════════
    
    // تهيئة cache الأنواع البدائية / Initialize primitive type cache
    void initializePrimitiveTypes();
    
    // ═══════════════════════════════════════════════════════════════════════════
    // Hash للأنواع / Type Hashing
    // ═══════════════════════════════════════════════════════════════════════════
    
    // حساب hash لنوع / Calculate hash for a type
    size_t hashType(const TypePtr& type) const;
    
    // ═══════════════════════════════════════════════════════════════════════════
    // الأعضاء الخاصة / Private Members
    // ═══════════════════════════════════════════════════════════════════════════
    
    // خريطة الأنواع (للـ interning) / Type map (for interning)
    // Key: type hash, Value: TypePtr
    std::unordered_map<size_t, TypePtr> type_map_;
    
    // cache الأنواع البدائية / Primitive type cache
    TypePtr void_type_;      // نوع فراغ / Void type
    TypePtr integer_type_;   // نوع رقم / Integer type
    TypePtr float_type_;     // نوع عشري / Float type
    TypePtr boolean_type_;   // نوع منطقي / Boolean type
    TypePtr string_type_;    // نوع نص / String type
    TypePtr any_type_;       // نوع أي / Any type
    TypePtr never_type_;     // نوع أبداً / Never type
    TypePtr unknown_type_;   // نوع مجهول / Unknown type
    
    // mutex للـ thread safety / Mutex for thread safety
    mutable std::mutex mutex_;
};

} // namespace TypeSystem
} // namespace Sad

#endif // SAD_COMPILER_TYPE_SYSTEM_TYPE_REGISTRY_H
