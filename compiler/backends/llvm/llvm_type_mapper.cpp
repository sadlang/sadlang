/*
 * ============================================================================
 * محول أنواع Sad/SIR إلى LLVM - ملف التنفيذ
 * Sad/SIR to LLVM Type Mapper - Implementation File
 * ============================================================================
 * 
 * هذا الملف يحتوي على تنفيذ محول أنواع Sad و SIR إلى LLVM IR
 * This file contains implementation of Sad and SIR to LLVM type mapper
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement
 * ============================================================================
 */

#include "llvm_type_mapper.h"
#include <stdexcept>

namespace Sad {
namespace LLVM {

// ============================================================================
// المنشئ والمدمر / Constructor & Destructor
// ============================================================================

/**
 * منشئ محول الأنواع - يتم تهيئة السياق
 * Type mapper constructor - Initialize context
 */
LLVMTypeMapper::LLVMTypeMapper(llvm::LLVMContext& context)
    : context_(context)
{
    // لا حاجة لتهيئة أخرى هنا / No further initialization needed here
    // التخزين المؤقت سيُملأ تدريجياً / Cache will be filled gradually
}

// ============================================================================
// تحويل أنواع Sad / Sad Type Conversion
// ============================================================================

/**
 * تحويل نوع Sad إلى نوع LLVM
 * Convert Sad type to LLVM type
 */
llvm::Type* LLVMTypeMapper::mapSadType(std::shared_ptr<Type> sadType) {
    // التحقق من أن النوع ليس null / Check that type is not null
    if (!sadType) {
        return getVoidType(); // افتراضي: نوع فارغ / Default: void type
    }
    
    // التحقق من التخزين المؤقت أولاً / Check cache first
    std::string typeName = sadType->toString(); // الحصول على اسم النوع / Get type name
    if (auto cached = getCachedType(typeName)) {
        return cached; // إرجاع من التخزين المؤقت / Return from cache
    }
    
    llvm::Type* result = nullptr; // النتيجة / Result
    
    // تحديد نوع Sad وتحويله / Determine Sad type and convert
    if (sadType->isVoid()) {
        // نوع فارغ / Void type
        result = getVoidType();
    }
    else if (sadType->isBoolean()) {
        // نوع منطقي (i1) / Boolean type (i1)
        result = getBoolType();
    }
    else if (sadType->isInteger()) {
        // نوع عدد صحيح / Integer type
        int bitWidth = sadType->getBitWidth(); // الحصول على حجم البتات / Get bit width
        result = getIntType(bitWidth); // تحويل إلى نوع LLVM / Convert to LLVM type
    }
    else if (sadType->isFloat()) {
        // نوع عدد عشري / Float type
        if (sadType->isFloat32()) {
            result = getFloatType(); // float (32-bit)
        } else {
            result = getDoubleType(); // double (64-bit) - افتراضي / default
        }
    }
    else if (sadType->isString()) {
        // نوع نص (i8*) / String type (i8*)
        result = getStringPtrType();
    }
    else if (sadType->isPointer()) {
        // نوع مؤشر / Pointer type
        result = mapPointerType(sadType);
    }
    else if (sadType->isArray()) {
        // نوع مصفوفة / Array type
        result = mapArrayType(sadType);
    }
    else if (sadType->isFunction()) {
        // نوع دالة - يحتاج معالجة خاصة / Function type - needs special handling
        // لا يمكن تحويله مباشرة، استخدم mapFunctionType / Cannot convert directly, use mapFunctionType
        result = getInt8PtrType(); // مؤقت: مؤشر عام / Temporary: generic pointer
    }
    else if (sadType->isClass()) {
        // نوع صنف / Class type
        result = mapClassType(sadType);
    }
    else {
        // نوع غير معروف - افتراضي i64 / Unknown type - default i64
        result = getInt64Type();
    }
    
    // حفظ في التخزين المؤقت / Save to cache
    cacheType(typeName, result);
    
    return result; // إرجاع النتيجة / Return result
}

/**
 * تحويل نوع دالة Sad إلى نوع دالة LLVM
 * Convert Sad function type to LLVM function type
 */
llvm::FunctionType* LLVMTypeMapper::mapFunctionType(
    std::shared_ptr<Type> returnType,
    const std::vector<std::shared_ptr<Type>>& paramTypes,
    bool isVarArg)
{
    // تحويل نوع الرجوع / Convert return type
    llvm::Type* llvmReturnType = mapSadType(returnType);
    
    // تحويل أنواع المعاملات / Convert parameter types
    std::vector<llvm::Type*> llvmParamTypes;
    llvmParamTypes.reserve(paramTypes.size()); // حجز مساحة مسبقاً / Reserve space
    
    for (const auto& paramType : paramTypes) {
        llvm::Type* llvmParamType = mapSadType(paramType); // تحويل كل معامل / Convert each parameter
        llvmParamTypes.push_back(llvmParamType);
    }
    
    // إنشاء نوع الدالة / Create function type
    return createFunctionType(llvmReturnType, llvmParamTypes, isVarArg);
}

// ============================================================================
// تحويل أنواع SIR / SIR Type Conversion
// ============================================================================

/**
 * تحويل نوع SIR إلى نوع LLVM
 * Convert SIR type to LLVM type
 */
llvm::Type* LLVMTypeMapper::mapSIRType(Compiler::SIR::SIRType sirType) {
    // تحويل مباشر بناءً على نوع SIR / Direct conversion based on SIR type
    switch (sirType) {
        case Compiler::SIR::SIRType::VOID:
            // نوع فارغ / Void type
            return getVoidType();
            
        case Compiler::SIR::SIRType::I64:
            // عدد صحيح 64-bit / 64-bit integer
            return getInt64Type();
            
        case Compiler::SIR::SIRType::F64:
            // عدد عشري 64-bit / 64-bit float
            return getDoubleType();
            
        case Compiler::SIR::SIRType::BOOL:
            // منطقي (i1) / Boolean (i1)
            return getBoolType();
            
        case Compiler::SIR::SIRType::PTR:
            // مؤشر عام (i8*) / Generic pointer (i8*)
            return getStringPtrType();
            
        case Compiler::SIR::SIRType::STRING:
            // نص (i8*) / String (i8*)
            return getStringPtrType();
            
        case Compiler::SIR::SIRType::ARRAY:
            // مصفوفة - يحتاج معلومات إضافية / Array - needs additional info
            // افتراضي: مؤشر / Default: pointer
            return getStringPtrType();
            
        case Compiler::SIR::SIRType::STRUCT:
            // بنية - يحتاج تعريف / Struct - needs definition
            // افتراضي: مؤشر / Default: pointer
            return getStringPtrType();
            
        case Compiler::SIR::SIRType::FUNCTION:
            // دالة - يحتاج توقيع / Function - needs signature
            // افتراضي: مؤشر دالة / Default: function pointer
            return getStringPtrType();
            
        default:
            // نوع غير معروف - افتراضي i64 / Unknown type - default i64
            return getInt64Type();
    }
}

/**
 * تحويل نوع SIR إلى نوع مؤشر LLVM
 * Convert SIR type to LLVM pointer type
 */
llvm::PointerType* LLVMTypeMapper::mapSIRTypeToPointer(Compiler::SIR::SIRType sirType) {
    // الحصول على النوع الأساسي أولاً / Get base type first
    llvm::Type* baseType = mapSIRType(sirType);
    
    // إنشاء نوع مؤشر / Create pointer type
    return createPointerType(baseType);
}

// ============================================================================
// أنواع أساسية / Primitive Types
// ============================================================================

/**
 * الحصول على نوع فارغ LLVM / Get LLVM void type
 */
llvm::Type* LLVMTypeMapper::getVoidType() const {
    return llvm::Type::getVoidTy(context_); // نوع فارغ / Void type
}

/**
 * الحصول على نوع منطقي (i1) / Get boolean type (i1)
 */
llvm::Type* LLVMTypeMapper::getBoolType() const {
    return llvm::Type::getInt1Ty(context_); // i1 للمنطقيات / i1 for booleans
}

/**
 * الحصول على نوع عدد صحيح 8-bit / Get 8-bit integer type
 */
llvm::Type* LLVMTypeMapper::getInt8Type() const {
    return llvm::Type::getInt8Ty(context_); // i8
}

/**
 * الحصول على نوع عدد صحيح 16-bit / Get 16-bit integer type
 */
llvm::Type* LLVMTypeMapper::getInt16Type() const {
    return llvm::Type::getInt16Ty(context_); // i16
}

/**
 * الحصول على نوع عدد صحيح 32-bit / Get 32-bit integer type
 */
llvm::Type* LLVMTypeMapper::getInt32Type() const {
    return llvm::Type::getInt32Ty(context_); // i32
}

/**
 * الحصول على نوع عدد صحيح 64-bit / Get 64-bit integer type
 */
llvm::Type* LLVMTypeMapper::getInt64Type() const {
    return llvm::Type::getInt64Ty(context_); // i64
}

/**
 * الحصول على نوع عدد عشري 32-bit (float) / Get 32-bit float type
 */
llvm::Type* LLVMTypeMapper::getFloatType() const {
    return llvm::Type::getFloatTy(context_); // float (32-bit)
}

/**
 * الحصول على نوع عدد عشري 64-bit (double) / Get 64-bit double type
 */
llvm::Type* LLVMTypeMapper::getDoubleType() const {
    return llvm::Type::getDoubleTy(context_); // double (64-bit)
}

/**
 * الحصول على نوع مؤشر i8* (للنصوص) / Get i8* pointer type (for strings)
 */
llvm::PointerType* LLVMTypeMapper::getStringPtrType() const {
    return llvm::Type::getInt8PtrTy(context_); // i8* للنصوص / i8* for strings
}

/**
 * الحصول على نوع عدد صحيح بحجم محدد / Get integer type with specific bit width
 */
llvm::Type* LLVMTypeMapper::getIntType(unsigned bitWidth) const {
    // التحقق من الأحجام المدعومة / Check supported sizes
    switch (bitWidth) {
        case 1:  return getBoolType();   // i1 للمنطقيات / i1 for booleans
        case 8:  return getInt8Type();   // i8
        case 16: return getInt16Type();  // i16
        case 32: return getInt32Type();  // i32
        case 64: return getInt64Type();  // i64
        default:
            // حجم غير مدعوم - افتراضي i64 / Unsupported size - default i64
            return getInt64Type();
    }
}

// ============================================================================
// أنواع مركبة / Composite Types
// ============================================================================

/**
 * إنشاء نوع مصفوفة LLVM / Create LLVM array type
 */
llvm::ArrayType* LLVMTypeMapper::createArrayType(llvm::Type* elementType, size_t size) {
    // إنشاء نوع مصفوفة / Create array type
    return llvm::ArrayType::get(elementType, size);
}

/**
 * إنشاء نوع بنية (struct) / Create struct type
 */
llvm::StructType* LLVMTypeMapper::createStructType(
    const std::string& name,
    const std::vector<llvm::Type*>& fieldTypes,
    bool isPacked)
{
    // التحقق من التخزين المؤقت / Check cache
    auto it = structCache_.find(name);
    if (it != structCache_.end()) {
        return it->second; // إرجاع من التخزين المؤقت / Return from cache
    }
    
    // إنشاء بنية جديدة / Create new struct
    llvm::StructType* structType = llvm::StructType::create(
        context_,      // السياق / Context
        fieldTypes,    // أنواع الحقول / Field types
        name,          // الاسم / Name
        isPacked       // هل مُرصّة؟ / Is packed?
    );
    
    // حفظ في التخزين المؤقت / Save to cache
    structCache_[name] = structType;
    
    return structType; // إرجاع البنية / Return struct
}

/**
 * إنشاء نوع مؤشر / Create pointer type
 */
llvm::PointerType* LLVMTypeMapper::createPointerType(llvm::Type* pointeeType) {
    // إنشاء نوع مؤشر / Create pointer type
    return pointeeType->getPointerTo();
}

/**
 * إنشاء نوع دالة / Create function type
 */
llvm::FunctionType* LLVMTypeMapper::createFunctionType(
    llvm::Type* returnType,
    const std::vector<llvm::Type*>& paramTypes,
    bool isVarArg)
{
    // إنشاء نوع دالة / Create function type
    return llvm::FunctionType::get(
        returnType,  // نوع الرجوع / Return type
        paramTypes,  // أنواع المعاملات / Parameter types
        isVarArg     // هل تقبل عدد متغير؟ / Is variadic?
    );
}

// ============================================================================
// إدارة التخزين المؤقت / Cache Management
// ============================================================================

/**
 * مسح التخزين المؤقت للأنواع / Clear type cache
 */
void LLVMTypeMapper::clearCache() {
    typeCache_.clear();    // مسح تخزين الأنواع / Clear type cache
    structCache_.clear();  // مسح تخزين البنى / Clear struct cache
}

/**
 * الحصول على عدد الأنواع المخزنة مؤقتاً / Get number of cached types
 */
size_t LLVMTypeMapper::getCacheSize() const {
    return typeCache_.size() + structCache_.size(); // مجموع التخزين / Total cache
}

// ============================================================================
// دوال مساعدة خاصة / Private Helper Functions
// ============================================================================

/**
 * تحويل نوع مصفوفة Sad / Convert Sad array type
 */
llvm::Type* LLVMTypeMapper::mapArrayType(std::shared_ptr<Type> sadType) {
    // الحصول على نوع العنصر / Get element type
    auto elementType = sadType->getElementType();
    
    // تحويل نوع العنصر إلى LLVM / Convert element type to LLVM
    llvm::Type* llvmElementType = mapSadType(elementType);
    
    // الحصول على حجم المصفوفة / Get array size
    size_t arraySize = sadType->getArraySize();
    
    // إنشاء نوع مصفوفة / Create array type
    return createArrayType(llvmElementType, arraySize);
}

/**
 * تحويل نوع مؤشر Sad / Convert Sad pointer type
 */
llvm::Type* LLVMTypeMapper::mapPointerType(std::shared_ptr<Type> sadType) {
    // الحصول على نوع البيانات المشار إليه / Get pointee type
    auto pointeeType = sadType->getPointeeType();
    
    // تحويل نوع البيانات المشار إليه إلى LLVM / Convert pointee type to LLVM
    llvm::Type* llvmPointeeType = mapSadType(pointeeType);
    
    // إنشاء نوع مؤشر / Create pointer type
    return createPointerType(llvmPointeeType);
}

/**
 * تحويل نوع صنف Sad / Convert Sad class type
 */
llvm::Type* LLVMTypeMapper::mapClassType(std::shared_ptr<Type> sadType) {
    // الحصول على اسم الصنف / Get class name
    std::string className = sadType->toString();
    
    // التحقق من التخزين المؤقت / Check cache
    auto it = structCache_.find(className);
    if (it != structCache_.end()) {
        return it->second->getPointerTo(); // إرجاع مؤشر للبنية / Return pointer to struct
    }
    
    // إنشاء بنية فارغة مؤقتاً / Create empty struct temporarily
    // سيتم ملؤها لاحقاً عند تعريف الصنف / Will be filled later when class is defined
    llvm::StructType* classStruct = llvm::StructType::create(context_, className);
    
    // حفظ في التخزين المؤقت / Save to cache
    structCache_[className] = classStruct;
    
    // إرجاع مؤشر للبنية / Return pointer to struct
    return classStruct->getPointerTo();
}

/**
 * الحصول على نوع من التخزين المؤقت / Get type from cache
 */
llvm::Type* LLVMTypeMapper::getCachedType(const std::string& typeName) const {
    // البحث في التخزين المؤقت / Search in cache
    auto it = typeCache_.find(typeName);
    
    if (it != typeCache_.end()) {
        return it->second; // تم العثور عليه / Found
    }
    
    return nullptr; // لم يتم العثور عليه / Not found
}

/**
 * حفظ نوع في التخزين المؤقت / Save type to cache
 */
void LLVMTypeMapper::cacheType(const std::string& typeName, llvm::Type* type) {
    // حفظ في التخزين المؤقت / Save to cache
    typeCache_[typeName] = type;
}

} // namespace LLVM
} // namespace Sad
