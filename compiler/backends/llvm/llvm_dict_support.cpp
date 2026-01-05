/*
 * ============================================================================
 * دعم القواميس المتقدم في LLVM - ملف التنفيذ
 * Advanced Dictionary Support in LLVM - Implementation File
 * ============================================================================
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement (Day 4)
 * ============================================================================
 */

#include "llvm_dict_support.h"
#include "llvm_type_mapper.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

namespace Sad {
namespace LLVM {

// ============================================================================
// المنشئ / Constructor
// ============================================================================

/**
 * منشئ دعم القواميس - تهيئة المكونات
 * Dictionary support constructor - Initialize components
 */
LLVMDictSupport::LLVMDictSupport(llvm::LLVMContext& context,
                                 llvm::IRBuilder<>& builder,
                                 LLVMTypeMapper* typeMapper)
    : context_(context)
    , builder_(builder)
    , typeMapper_(typeMapper)
    , dictStructType_(nullptr)
{
    // إنشاء نوع هيكل القاموس / Create dictionary struct type
    dictStructType_ = getOrCreateDictStructType();
}

// ============================================================================
// إنشاء القواميس / Dictionary Creation
// ============================================================================

/**
 * إنشاء قاموس فارغ
 * Create empty dictionary
 */
llvm::Value* LLVMDictSupport::createDict(llvm::Type* keyType, llvm::Type* valueType, size_t initialCapacity) {
    // السعة الابتدائية / Initial capacity
    llvm::Value* capacity = llvm::ConstantInt::get(builder_.getInt64Ty(), initialCapacity);
    
    // استدعاء دالة runtime لإنشاء القاموس / Call runtime to create dict
    std::vector<llvm::Value*> args = {capacity};
    return callDictRuntime("sad_dict_create", args);
}

/**
 * إنشاء قاموس من أزواج مفتاح-قيمة
 * Create dictionary from key-value pairs
 */
llvm::Value* LLVMDictSupport::createDictFromPairs(
    const std::vector<llvm::Value*>& keys,
    const std::vector<llvm::Value*>& values)
{
    // التحقق من تطابق الأحجام / Verify sizes match
    if (keys.size() != values.size()) {
        // خطأ: عدد المفاتيح لا يساوي عدد القيم / Error: key/value count mismatch
        return nullptr;
    }
    
    // إنشاء قاموس فارغ / Create empty dict
    llvm::Value* dict = createDict(keys[0]->getType(), values[0]->getType(), keys.size());
    
    // إضافة الأزواج / Add pairs
    for (size_t i = 0; i < keys.size(); ++i) {
        setValue(dict, keys[i], values[i]);
    }
    
    return dict;
}

// ============================================================================
// الوصول والتعديل / Access and Modification
// ============================================================================

/**
 * الوصول لقيمة بمفتاح
 * Get value by key
 */
llvm::Value* LLVMDictSupport::getValue(llvm::Value* dict, llvm::Value* key) {
    // استدعاء دالة runtime للوصول / Call runtime for access
    std::vector<llvm::Value*> args = {dict, key};
    return callDictRuntime("sad_dict_get", args);
}

/**
 * الوصول لقيمة مع قيمة افتراضية
 * Get value with default
 */
llvm::Value* LLVMDictSupport::getValueOrDefault(llvm::Value* dict, llvm::Value* key, llvm::Value* defaultValue) {
    // استدعاء دالة runtime / Call runtime
    std::vector<llvm::Value*> args = {dict, key, defaultValue};
    return callDictRuntime("sad_dict_get_or_default", args);
}

/**
 * تعيين قيمة بمفتاح
 * Set value by key
 */
void LLVMDictSupport::setValue(llvm::Value* dict, llvm::Value* key, llvm::Value* value) {
    // استدعاء دالة runtime للتعيين / Call runtime for assignment
    std::vector<llvm::Value*> args = {dict, key, value};
    callDictRuntime("sad_dict_set", args);
}

/**
 * حذف زوج مفتاح-قيمة
 * Delete key-value pair
 */
llvm::Value* LLVMDictSupport::deleteKey(llvm::Value* dict, llvm::Value* key) {
    // استدعاء دالة runtime للحذف / Call runtime for deletion
    std::vector<llvm::Value*> args = {dict, key};
    return callDictRuntime("sad_dict_delete", args);
}

/**
 * مسح جميع العناصر
 * Clear all elements
 */
void LLVMDictSupport::clear(llvm::Value* dict) {
    // استدعاء دالة runtime للمسح / Call runtime for clearing
    std::vector<llvm::Value*> args = {dict};
    callDictRuntime("sad_dict_clear", args);
}

// ============================================================================
// عمليات القواميس / Dictionary Operations
// ============================================================================

/**
 * دمج قاموسين
 * Merge two dictionaries
 */
llvm::Value* LLVMDictSupport::merge(llvm::Value* dict1, llvm::Value* dict2) {
    // استدعاء دالة runtime للدمج / Call runtime for merging
    std::vector<llvm::Value*> args = {dict1, dict2};
    return callDictRuntime("sad_dict_merge", args);
}

/**
 * الحصول على جميع المفاتيح
 * Get all keys
 */
llvm::Value* LLVMDictSupport::getKeys(llvm::Value* dict) {
    // استدعاء دالة runtime للحصول على المفاتيح / Call runtime for keys
    std::vector<llvm::Value*> args = {dict};
    return callDictRuntime("sad_dict_keys", args);
}

/**
 * الحصول على جميع القيم
 * Get all values
 */
llvm::Value* LLVMDictSupport::getValues(llvm::Value* dict) {
    // استدعاء دالة runtime للحصول على القيم / Call runtime for values
    std::vector<llvm::Value*> args = {dict};
    return callDictRuntime("sad_dict_values", args);
}

/**
 * الحصول على جميع الأزواج
 * Get all items
 */
llvm::Value* LLVMDictSupport::getItems(llvm::Value* dict) {
    // استدعاء دالة runtime للحصول على الأزواج / Call runtime for items
    std::vector<llvm::Value*> args = {dict};
    return callDictRuntime("sad_dict_items", args);
}

/**
 * تطبيق دالة على كل زوج (map)
 * Apply function to each pair (map)
 */
llvm::Value* LLVMDictSupport::mapDict(llvm::Value* dict, llvm::Function* mapFn) {
    // استدعاء دالة runtime للتحويل / Call runtime for map
    std::vector<llvm::Value*> args = {dict, mapFn};
    return callDictRuntime("sad_dict_map", args);
}

/**
 * تصفية القاموس (filter)
 * Filter dictionary
 */
llvm::Value* LLVMDictSupport::filterDict(llvm::Value* dict, llvm::Function* filterFn) {
    // استدعاء دالة runtime للتصفية / Call runtime for filter
    std::vector<llvm::Value*> args = {dict, filterFn};
    return callDictRuntime("sad_dict_filter", args);
}

// ============================================================================
// استعلامات القواميس / Dictionary Queries
// ============================================================================

/**
 * الحصول على عدد العناصر
 * Get number of elements
 */
llvm::Value* LLVMDictSupport::getSize(llvm::Value* dict) {
    // استدعاء دالة runtime للحصول على الحجم / Call runtime for size
    std::vector<llvm::Value*> args = {dict};
    return callDictRuntime("sad_dict_size", args);
}

/**
 * التحقق من خلو القاموس
 * Check if dictionary is empty
 */
llvm::Value* LLVMDictSupport::isEmpty(llvm::Value* dict) {
    // الحصول على الحجم ومقارنته بصفر / Get size and compare with zero
    llvm::Value* size = getSize(dict);
    llvm::Value* zero = llvm::ConstantInt::get(builder_.getInt64Ty(), 0);
    return builder_.CreateICmpEQ(size, zero, "is_empty");
}

/**
 * التحقق من وجود مفتاح
 * Check if key exists
 */
llvm::Value* LLVMDictSupport::hasKey(llvm::Value* dict, llvm::Value* key) {
    // استدعاء دالة runtime للتحقق / Call runtime for check
    std::vector<llvm::Value*> args = {dict, key};
    return callDictRuntime("sad_dict_has_key", args);
}

/**
 * التحقق من وجود قيمة
 * Check if value exists
 */
llvm::Value* LLVMDictSupport::hasValue(llvm::Value* dict, llvm::Value* value) {
    // استدعاء دالة runtime للتحقق / Call runtime for check
    std::vector<llvm::Value*> args = {dict, value};
    return callDictRuntime("sad_dict_has_value", args);
}

// ============================================================================
// إدارة الذاكرة / Memory Management
// ============================================================================

/**
 * تحرير ذاكرة القاموس
 * Free dictionary memory
 */
void LLVMDictSupport::freeDict(llvm::Value* dict) {
    // استدعاء دالة runtime لتحرير الذاكرة / Call runtime to free
    std::vector<llvm::Value*> args = {dict};
    callDictRuntime("sad_dict_free", args);
}

/**
 * نسخ قاموس (deep copy)
 * Clone dictionary (deep copy)
 */
llvm::Value* LLVMDictSupport::cloneDict(llvm::Value* dict) {
    // استدعاء دالة runtime للنسخ / Call runtime for cloning
    std::vector<llvm::Value*> args = {dict};
    return callDictRuntime("sad_dict_clone", args);
}

// ============================================================================
// دوال مساعدة / Helper Functions
// ============================================================================

/**
 * الحصول أو إنشاء نوع هيكل القاموس
 * Get or create dictionary struct type
 */
llvm::StructType* LLVMDictSupport::getOrCreateDictStructType() {
    // التحقق من وجود النوع مسبقاً / Check if exists
    if (dictStructType_) {
        return dictStructType_;
    }
    
    // إنشاء نوع الهيكل / Create struct type
    // هيكل القاموس: جدول تجزئة مع معالجة التصادمات
    // Dictionary structure: hash table with collision handling
    // Source: LLVM 18+ Opaque Pointers - استخدام PointerType::getUnqual بدلاً من getInt8PtrTy()
    std::vector<llvm::Type*> fields = {
        llvm::PointerType::getUnqual(context_),   // buckets pointer
        builder_.getInt64Ty(),     // size (number of elements)
        builder_.getInt64Ty(),     // capacity (number of buckets)
        llvm::PointerType::getUnqual(context_),   // key type info
        llvm::PointerType::getUnqual(context_)    // value type info
    };
    
    return llvm::StructType::create(context_, fields, "SadDict");
}

/**
 * استدعاء دالة runtime للقواميس
 * Call dictionary runtime function
 */
llvm::Value* LLVMDictSupport::callDictRuntime(
    const std::string& functionName,
    const std::vector<llvm::Value*>& args)
{
    // الحصول على الوحدة / Get module
    llvm::Module* module = builder_.GetInsertBlock()->getParent()->getParent();
    
    // البحث عن الدالة أو إنشائها / Look up or create function
    llvm::Function* function = module->getFunction(functionName);
    
    if (!function) {
        // إنشاء توقيع الدالة / Create function signature
        std::vector<llvm::Type*> paramTypes;
        for (const auto& arg : args) {
            paramTypes.push_back(arg->getType());
        }
        
        // نوع الرجوع الافتراضي / Default return type
        // Source: LLVM 18+ Opaque Pointers - استخدام PointerType::getUnqual بدلاً من getInt8PtrTy()
        llvm::Type* returnType = llvm::PointerType::getUnqual(context_);
        
        llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, paramTypes, false);
        function = llvm::Function::Create(
            funcType,
            llvm::Function::ExternalLinkage,
            functionName,
            module
        );
    }
    
    // إنشاء الاستدعاء / Create call
    return builder_.CreateCall(function, args, functionName + "_result");
}

/**
 * حساب قيمة التجزئة
 * Compute hash value
 */
llvm::Value* LLVMDictSupport::computeHash(llvm::Value* key) {
    // استدعاء دالة runtime لحساب التجزئة / Call runtime for hash computation
    std::vector<llvm::Value*> args = {key};
    return callDictRuntime("sad_hash_compute", args);
}

} // namespace LLVM
} // namespace Sad
