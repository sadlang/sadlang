/*
 * ============================================================================
 * دعم القواميس المتقدم في LLVM - ملف الرأس
 * Advanced Dictionary Support in LLVM - Header File
 * ============================================================================
 * 
 * هذا الملف يوفر دعماً متقدماً للقواميس (hash maps) في LLVM IR
 * This file provides advanced dictionary (hash map) support in LLVM IR
 * 
 * الميزات / Features:
 * - جداول تجزئة (hash tables)
 * - معالجة التصادمات (collision handling)
 * - إعادة التجزئة التلقائية (automatic rehashing)
 * - عمليات القواميس (dict operations: keys, values, items, etc.)
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement (Day 4)
 * ============================================================================
 */

#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <memory>
#include <vector>
#include <string>

namespace Sad {
namespace LLVM {

// Forward declarations / التصريحات المسبقة
class LLVMTypeMapper;

// ============================================================================
// فئة دعم القواميس المتقدم / Advanced Dictionary Support Class
// ============================================================================

/**
 * LLVMDictSupport - دعم متقدم للقواميس في LLVM
 * LLVMDictSupport - Advanced dictionary support in LLVM
 * 
 * المسؤوليات / Responsibilities:
 * - إنشاء قواميس (dictionary creation)
 * - عمليات القواميس (dictionary operations)
 * - إدارة جداول التجزئة (hash table management)
 * - معالجة التصادمات (collision handling)
 */
class LLVMDictSupport {
public:
    // ========================================================================
    // المنشئ / Constructor
    // ========================================================================
    
    /**
     * منشئ دعم القواميس
     * Dictionary support constructor
     * 
     * @param context سياق LLVM / LLVM context
     * @param builder بناء IR / IR builder
     * @param typeMapper محول الأنواع / Type mapper
     */
    LLVMDictSupport(llvm::LLVMContext& context,
                    llvm::IRBuilder<>& builder,
                    LLVMTypeMapper* typeMapper);
    
    /**
     * مدمر دعم القواميس
     * Dictionary support destructor
     */
    ~LLVMDictSupport() = default;
    
    // ========================================================================
    // إنشاء القواميس / Dictionary Creation
    // ========================================================================
    
    /**
     * إنشاء قاموس فارغ
     * Create empty dictionary
     * 
     * @param keyType نوع المفاتيح / Key type
     * @param valueType نوع القيم / Value type
     * @param initialCapacity السعة الابتدائية / Initial capacity
     * @return قاموس جديد / New dictionary
     */
    llvm::Value* createDict(llvm::Type* keyType, llvm::Type* valueType, size_t initialCapacity = 16);
    
    /**
     * إنشاء قاموس من أزواج مفتاح-قيمة
     * Create dictionary from key-value pairs
     * 
     * @param keys المفاتيح / Keys
     * @param values القيم / Values
     * @return قاموس جديد / New dictionary
     */
    llvm::Value* createDictFromPairs(const std::vector<llvm::Value*>& keys,
                                     const std::vector<llvm::Value*>& values);
    
    // ========================================================================
    // الوصول والتعديل / Access and Modification
    // ========================================================================
    
    /**
     * الوصول لقيمة بمفتاح
     * Get value by key
     * 
     * @param dict القاموس / Dictionary
     * @param key المفتاح / Key
     * @return القيمة أو null / Value or null
     */
    llvm::Value* getValue(llvm::Value* dict, llvm::Value* key);
    
    /**
     * الوصول لقيمة مع قيمة افتراضية
     * Get value with default
     * 
     * @param dict القاموس / Dictionary
     * @param key المفتاح / Key
     * @param defaultValue القيمة الافتراضية / Default value
     * @return القيمة أو الافتراضية / Value or default
     */
    llvm::Value* getValueOrDefault(llvm::Value* dict, llvm::Value* key, llvm::Value* defaultValue);
    
    /**
     * تعيين قيمة بمفتاح
     * Set value by key
     * 
     * @param dict القاموس / Dictionary
     * @param key المفتاح / Key
     * @param value القيمة / Value
     */
    void setValue(llvm::Value* dict, llvm::Value* key, llvm::Value* value);
    
    /**
     * حذف زوج مفتاح-قيمة
     * Delete key-value pair
     * 
     * @param dict القاموس / Dictionary
     * @param key المفتاح / Key
     * @return true إذا تم الحذف / true if deleted
     */
    llvm::Value* deleteKey(llvm::Value* dict, llvm::Value* key);
    
    /**
     * مسح جميع العناصر
     * Clear all elements
     * 
     * @param dict القاموس / Dictionary
     */
    void clear(llvm::Value* dict);
    
    // ========================================================================
    // عمليات القواميس / Dictionary Operations
    // ========================================================================
    
    /**
     * دمج قاموسين
     * Merge two dictionaries
     * 
     * @param dict1 القاموس الأول / First dictionary
     * @param dict2 القاموس الثاني / Second dictionary
     * @return قاموس جديد / New dictionary
     */
    llvm::Value* merge(llvm::Value* dict1, llvm::Value* dict2);
    
    /**
     * الحصول على جميع المفاتيح
     * Get all keys
     * 
     * @param dict القاموس / Dictionary
     * @return مصفوفة المفاتيح / Array of keys
     */
    llvm::Value* getKeys(llvm::Value* dict);
    
    /**
     * الحصول على جميع القيم
     * Get all values
     * 
     * @param dict القاموس / Dictionary
     * @return مصفوفة القيم / Array of values
     */
    llvm::Value* getValues(llvm::Value* dict);
    
    /**
     * الحصول على جميع الأزواج (مفتاح، قيمة)
     * Get all items (key, value pairs)
     * 
     * @param dict القاموس / Dictionary
     * @return مصفوفة الأزواج / Array of pairs
     */
    llvm::Value* getItems(llvm::Value* dict);
    
    /**
     * تطبيق دالة على كل زوج (map)
     * Apply function to each pair (map)
     * 
     * @param dict القاموس / Dictionary
     * @param mapFn دالة التحويل / Map function
     * @return قاموس جديد / New dictionary
     */
    llvm::Value* mapDict(llvm::Value* dict, llvm::Function* mapFn);
    
    /**
     * تصفية القاموس (filter)
     * Filter dictionary
     * 
     * @param dict القاموس / Dictionary
     * @param filterFn دالة التصفية / Filter function
     * @return قاموس جديد / New dictionary
     */
    llvm::Value* filterDict(llvm::Value* dict, llvm::Function* filterFn);
    
    // ========================================================================
    // استعلامات القواميس / Dictionary Queries
    // ========================================================================
    
    /**
     * الحصول على عدد العناصر
     * Get number of elements
     * 
     * @param dict القاموس / Dictionary
     * @return العدد / Count
     */
    llvm::Value* getSize(llvm::Value* dict);
    
    /**
     * التحقق من خلو القاموس
     * Check if dictionary is empty
     * 
     * @param dict القاموس / Dictionary
     * @return true إذا كان فارغاً / true if empty
     */
    llvm::Value* isEmpty(llvm::Value* dict);
    
    /**
     * التحقق من وجود مفتاح
     * Check if key exists
     * 
     * @param dict القاموس / Dictionary
     * @param key المفتاح / Key
     * @return true إذا كان موجوداً / true if exists
     */
    llvm::Value* hasKey(llvm::Value* dict, llvm::Value* key);
    
    /**
     * التحقق من وجود قيمة
     * Check if value exists
     * 
     * @param dict القاموس / Dictionary
     * @param value القيمة / Value
     * @return true إذا كانت موجودة / true if exists
     */
    llvm::Value* hasValue(llvm::Value* dict, llvm::Value* value);
    
    // ========================================================================
    // إدارة الذاكرة / Memory Management
    // ========================================================================
    
    /**
     * تحرير ذاكرة القاموس
     * Free dictionary memory
     * 
     * @param dict القاموس / Dictionary
     */
    void freeDict(llvm::Value* dict);
    
    /**
     * نسخ قاموس (deep copy)
     * Clone dictionary (deep copy)
     * 
     * @param dict القاموس / Dictionary
     * @return نسخة جديدة / New copy
     */
    llvm::Value* cloneDict(llvm::Value* dict);
    
private:
    // ========================================================================
    // البيانات الخاصة / Private Data
    // ========================================================================
    
    llvm::LLVMContext& context_;      ///< سياق LLVM / LLVM context
    llvm::IRBuilder<>& builder_;      ///< بناء IR / IR builder
    LLVMTypeMapper* typeMapper_;      ///< محول الأنواع / Type mapper
    
    llvm::StructType* dictStructType_;  ///< نوع هيكل القاموس / Dict struct type
    
    // ========================================================================
    // دوال مساعدة / Helper Functions
    // ========================================================================
    
    /**
     * الحصول أو إنشاء نوع هيكل القاموس
     * Get or create dictionary struct type
     * 
     * @return نوع الهيكل / Struct type
     */
    llvm::StructType* getOrCreateDictStructType();
    
    /**
     * استدعاء دالة runtime للقواميس
     * Call dictionary runtime function
     * 
     * @param functionName اسم الدالة / Function name
     * @param args المعاملات / Arguments
     * @return قيمة الرجوع / Return value
     */
    llvm::Value* callDictRuntime(const std::string& functionName,
                                  const std::vector<llvm::Value*>& args);
    
    /**
     * حساب قيمة التجزئة
     * Compute hash value
     * 
     * @param key المفتاح / Key
     * @return قيمة التجزئة / Hash value
     */
    llvm::Value* computeHash(llvm::Value* key);
};

} // namespace LLVM
} // namespace Sad
