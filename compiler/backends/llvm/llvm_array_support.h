/*
 * ============================================================================
 * دعم المصفوفات المتقدم في LLVM - ملف الرأس
 * Advanced Array Support in LLVM - Header File
 * ============================================================================
 * 
 * هذا الملف يوفر دعماً متقدماً للمصفوفات في LLVM IR
 * This file provides advanced array support in LLVM IR
 * 
 * الميزات / Features:
 * - مصفوفات ديناميكية (dynamic arrays)
 * - عمليات المصفوفات (operations: slice, concat, sort, etc.)
 * - تحسينات الأداء (performance optimizations)
 * - فحص الحدود (bounds checking)
 * - إدارة الذاكرة التلقائية (automatic memory management)
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
#include <llvm/IR/DerivedTypes.h>
#include <memory>
#include <vector>
#include <string>

namespace Sad {
namespace LLVM {

// Forward declarations / التصريحات المسبقة
class LLVMTypeMapper;

// ============================================================================
// هيكل المصفوفة الديناميكية / Dynamic Array Structure
// ============================================================================

/**
 * تمثيل المصفوفة الديناميكية في LLVM IR
 * Representation of dynamic array in LLVM IR
 * 
 * الهيكل / Structure:
 * struct SadArray {
 *     void* data;        // مؤشر البيانات / Data pointer
 *     i64 length;        // الطول الحالي / Current length
 *     i64 capacity;      // السعة المخصصة / Allocated capacity
 *     i32 elementSize;   // حجم العنصر / Element size
 *     i8* elementType;   // نوع العنصر / Element type info
 * }
 */
struct DynamicArrayType {
    llvm::PointerType* dataPtr;      ///< مؤشر البيانات / Data pointer
    llvm::Type* lengthType;          ///< نوع الطول / Length type
    llvm::Type* capacityType;        ///< نوع السعة / Capacity type
    llvm::Type* elementSizeType;     ///< نوع حجم العنصر / Element size type
    llvm::PointerType* typeInfoPtr;  ///< مؤشر معلومات النوع / Type info pointer
};

// ============================================================================
// فئة دعم المصفوفات المتقدم / Advanced Array Support Class
// ============================================================================

/**
 * LLVMArraySupport - دعم متقدم للمصفوفات في LLVM
 * LLVMArraySupport - Advanced array support in LLVM
 * 
 * المسؤوليات / Responsibilities:
 * - إنشاء مصفوفات ديناميكية (dynamic array creation)
 * - عمليات المصفوفات (array operations)
 * - تحسينات الأداء (performance optimizations)
 * - إدارة الذاكرة (memory management)
 */
class LLVMArraySupport {
public:
    // ========================================================================
    // المنشئ / Constructor
    // ========================================================================
    
    /**
     * منشئ دعم المصفوفات
     * Array support constructor
     * 
     * @param context سياق LLVM / LLVM context
     * @param builder بناء IR / IR builder
     * @param typeMapper محول الأنواع / Type mapper
     */
    LLVMArraySupport(llvm::LLVMContext& context,
                     llvm::IRBuilder<>& builder,
                     LLVMTypeMapper* typeMapper);
    
    /**
     * مدمر دعم المصفوفات
     * Array support destructor
     */
    ~LLVMArraySupport() = default;
    
    // ========================================================================
    // إنشاء المصفوفات / Array Creation
    // ========================================================================
    
    /**
     * إنشاء مصفوفة ديناميكية فارغة
     * Create empty dynamic array
     * 
     * @param elementType نوع العناصر / Element type
     * @param initialCapacity السعة الابتدائية / Initial capacity
     * @return مصفوفة ديناميكية / Dynamic array
     */
    llvm::Value* createDynamicArray(llvm::Type* elementType, size_t initialCapacity = 16);
    
    /**
     * إنشاء مصفوفة من قائمة عناصر
     * Create array from list of elements
     * 
     * @param elements العناصر / Elements
     * @param elementType نوع العناصر / Element type
     * @return مصفوفة ديناميكية / Dynamic array
     */
    llvm::Value* createArrayFromElements(const std::vector<llvm::Value*>& elements,
                                         llvm::Type* elementType);
    
    /**
     * إنشاء مصفوفة بقيمة افتراضية مكررة
     * Create array with repeated default value
     * 
     * @param defaultValue القيمة الافتراضية / Default value
     * @param count العدد / Count
     * @return مصفوفة ديناميكية / Dynamic array
     */
    llvm::Value* createArrayWithDefault(llvm::Value* defaultValue, llvm::Value* count);
    
    // ========================================================================
    // الوصول والتعديل / Access and Modification
    // ========================================================================
    
    /**
     * الوصول لعنصر مع فحص الحدود
     * Access element with bounds checking
     * 
     * @param array المصفوفة / Array
     * @param index الفهرس / Index
     * @return قيمة العنصر / Element value
     */
    llvm::Value* getElement(llvm::Value* array, llvm::Value* index);
    
    /**
     * تعيين عنصر مع فحص الحدود
     * Set element with bounds checking
     * 
     * @param array المصفوفة / Array
     * @param index الفهرس / Index
     * @param value القيمة الجديدة / New value
     */
    void setElement(llvm::Value* array, llvm::Value* index, llvm::Value* value);
    
    /**
     * إضافة عنصر في النهاية
     * Append element at end
     * 
     * @param array المصفوفة / Array
     * @param value العنصر الجديد / New element
     */
    void appendElement(llvm::Value* array, llvm::Value* value);
    
    /**
     * إدراج عنصر عند فهرس معين
     * Insert element at specific index
     * 
     * @param array المصفوفة / Array
     * @param index الفهرس / Index
     * @param value العنصر الجديد / New element
     */
    void insertElement(llvm::Value* array, llvm::Value* index, llvm::Value* value);
    
    /**
     * حذف عنصر عند فهرس معين
     * Remove element at specific index
     * 
     * @param array المصفوفة / Array
     * @param index الفهرس / Index
     */
    void removeElement(llvm::Value* array, llvm::Value* index);
    
    // ========================================================================
    // عمليات المصفوفات / Array Operations
    // ========================================================================
    
    /**
     * قص مصفوفة (slice)
     * Slice array
     * 
     * @param array المصفوفة / Array
     * @param start فهرس البداية / Start index
     * @param end فهرس النهاية / End index
     * @return مصفوفة جديدة / New array
     */
    llvm::Value* sliceArray(llvm::Value* array, llvm::Value* start, llvm::Value* end);
    
    /**
     * دمج مصفوفتين (concatenate)
     * Concatenate two arrays
     * 
     * @param array1 المصفوفة الأولى / First array
     * @param array2 المصفوفة الثانية / Second array
     * @return مصفوفة جديدة / New array
     */
    llvm::Value* concatenate(llvm::Value* array1, llvm::Value* array2);
    
    /**
     * ترتيب المصفوفة (sort)
     * Sort array
     * 
     * @param array المصفوفة / Array
     * @param compareFn دالة المقارنة / Compare function
     */
    void sortArray(llvm::Value* array, llvm::Function* compareFn = nullptr);
    
    /**
     * عكس المصفوفة (reverse)
     * Reverse array
     * 
     * @param array المصفوفة / Array
     */
    void reverseArray(llvm::Value* array);
    
    /**
     * تطبيق دالة على كل عنصر (map)
     * Apply function to each element (map)
     * 
     * @param array المصفوفة / Array
     * @param mapFn دالة التحويل / Map function
     * @return مصفوفة جديدة / New array
     */
    llvm::Value* mapArray(llvm::Value* array, llvm::Function* mapFn);
    
    /**
     * تصفية المصفوفة (filter)
     * Filter array
     * 
     * @param array المصفوفة / Array
     * @param filterFn دالة التصفية / Filter function
     * @return مصفوفة جديدة / New array
     */
    llvm::Value* filterArray(llvm::Value* array, llvm::Function* filterFn);
    
    /**
     * دمج عناصر المصفوفة (reduce)
     * Reduce array elements
     * 
     * @param array المصفوفة / Array
     * @param reduceFn دالة الدمج / Reduce function
     * @param initialValue القيمة الابتدائية / Initial value
     * @return القيمة النهائية / Final value
     */
    llvm::Value* reduceArray(llvm::Value* array, llvm::Function* reduceFn, llvm::Value* initialValue);
    
    // ========================================================================
    // استعلامات المصفوفات / Array Queries
    // ========================================================================
    
    /**
     * الحصول على طول المصفوفة
     * Get array length
     * 
     * @param array المصفوفة / Array
     * @return الطول / Length
     */
    llvm::Value* getLength(llvm::Value* array);
    
    /**
     * الحصول على السعة المخصصة
     * Get allocated capacity
     * 
     * @param array المصفوفة / Array
     * @return السعة / Capacity
     */
    llvm::Value* getCapacity(llvm::Value* array);
    
    /**
     * التحقق من خلو المصفوفة
     * Check if array is empty
     * 
     * @param array المصفوفة / Array
     * @return true إذا كانت فارغة / true if empty
     */
    llvm::Value* isEmpty(llvm::Value* array);
    
    /**
     * البحث عن عنصر
     * Search for element
     * 
     * @param array المصفوفة / Array
     * @param value القيمة المطلوبة / Search value
     * @return فهرس العنصر أو -1 / Element index or -1
     */
    llvm::Value* indexOf(llvm::Value* array, llvm::Value* value);
    
    /**
     * التحقق من وجود عنصر
     * Check if element exists
     * 
     * @param array المصفوفة / Array
     * @param value القيمة / Value
     * @return true إذا كان موجوداً / true if exists
     */
    llvm::Value* contains(llvm::Value* array, llvm::Value* value);
    
    // ========================================================================
    // إدارة الذاكرة / Memory Management
    // ========================================================================
    
    /**
     * تحرير ذاكرة المصفوفة
     * Free array memory
     * 
     * @param array المصفوفة / Array
     */
    void freeArray(llvm::Value* array);
    
    /**
     * نسخ مصفوفة (deep copy)
     * Clone array (deep copy)
     * 
     * @param array المصفوفة / Array
     * @return نسخة جديدة / New copy
     */
    llvm::Value* cloneArray(llvm::Value* array);
    
    /**
     * تغيير حجم المصفوفة
     * Resize array
     * 
     * @param array المصفوفة / Array
     * @param newCapacity السعة الجديدة / New capacity
     */
    void resizeArray(llvm::Value* array, llvm::Value* newCapacity);
    
private:
    // ========================================================================
    // البيانات الخاصة / Private Data
    // ========================================================================
    
    llvm::LLVMContext& context_;      ///< سياق LLVM / LLVM context
    llvm::IRBuilder<>& builder_;      ///< بناء IR / IR builder
    LLVMTypeMapper* typeMapper_;      ///< محول الأنواع / Type mapper
    
    llvm::StructType* arrayStructType_;  ///< نوع هيكل المصفوفة / Array struct type
    
    // ========================================================================
    // دوال مساعدة / Helper Functions
    // ========================================================================
    
    /**
     * الحصول أو إنشاء نوع هيكل المصفوفة
     * Get or create array struct type
     * 
     * @return نوع الهيكل / Struct type
     */
    llvm::StructType* getOrCreateArrayStructType();
    
    /**
     * استدعاء دالة runtime للمصفوفات
     * Call array runtime function
     * 
     * @param functionName اسم الدالة / Function name
     * @param args المعاملات / Arguments
     * @return قيمة الرجوع / Return value
     */
    llvm::Value* callArrayRuntime(const std::string& functionName,
                                   const std::vector<llvm::Value*>& args);
    
    /**
     * فحص حدود الفهرس
     * Check index bounds
     * 
     * @param array المصفوفة / Array
     * @param index الفهرس / Index
     * @param allowEqual السماح بالمساواة / Allow equal
     */
    void checkBounds(llvm::Value* array, llvm::Value* index, bool allowEqual = false);
    
    /**
     * الحصول على مؤشر حقل في الهيكل
     * Get field pointer in struct
     * 
     * @param array المصفوفة / Array
     * @param fieldIndex فهرس الحقل / Field index
     * @return مؤشر الحقل / Field pointer
     */
    llvm::Value* getFieldPtr(llvm::Value* array, unsigned fieldIndex);
};

} // namespace LLVM
} // namespace Sad
