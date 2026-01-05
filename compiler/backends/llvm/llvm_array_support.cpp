/*
 * ============================================================================
 * دعم المصفوفات المتقدم في LLVM - ملف التنفيذ
 * Advanced Array Support in LLVM - Implementation File
 * ============================================================================
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement (Day 4)
 * ============================================================================
 */

#include "llvm_array_support.h"
#include "llvm_type_mapper.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>

namespace Sad {
namespace LLVM {

// ============================================================================
// المنشئ / Constructor
// ============================================================================

/**
 * منشئ دعم المصفوفات - تهيئة المكونات
 * Array support constructor - Initialize components
 */
LLVMArraySupport::LLVMArraySupport(llvm::LLVMContext& context,
                                   llvm::IRBuilder<>& builder,
                                   LLVMTypeMapper* typeMapper)
    : context_(context)
    , builder_(builder)
    , typeMapper_(typeMapper)
    , arrayStructType_(nullptr)
{
    // إنشاء نوع هيكل المصفوفة / Create array struct type
    arrayStructType_ = getOrCreateArrayStructType();
}

// ============================================================================
// إنشاء المصفوفات / Array Creation
// ============================================================================

/**
 * إنشاء مصفوفة ديناميكية فارغة
 * Create empty dynamic array
 */
llvm::Value* LLVMArraySupport::createDynamicArray(llvm::Type* elementType, size_t initialCapacity) {
    // Source: LLVM DataLayout API - الحصول على DataLayout من Module
    // Get module from current insert block
    llvm::Module* module = builder_.GetInsertBlock()->getParent()->getParent();
    const llvm::DataLayout& DL = module->getDataLayout();
    
    // حساب حجم العنصر / Calculate element size
    llvm::Value* elementSize = llvm::ConstantInt::get(
        builder_.getInt32Ty(),
        DL.getTypeAllocSize(elementType)
    );
    
    // السعة الابتدائية / Initial capacity
    llvm::Value* capacity = llvm::ConstantInt::get(builder_.getInt64Ty(), initialCapacity);
    
    // استدعاء دالة runtime لإنشاء المصفوفة / Call runtime function to create array
    std::vector<llvm::Value*> args = {elementSize, capacity};
    return callArrayRuntime("sad_array_create", args);
}

/**
 * إنشاء مصفوفة من قائمة عناصر
 * Create array from list of elements
 */
llvm::Value* LLVMArraySupport::createArrayFromElements(
    const std::vector<llvm::Value*>& elements,
    llvm::Type* elementType)
{
    // إنشاء مصفوفة فارغة / Create empty array
    llvm::Value* array = createDynamicArray(elementType, elements.size());
    
    // إضافة العناصر واحداً تلو الآخر / Add elements one by one
    for (const auto& element : elements) {
        appendElement(array, element);
    }
    
    return array;
}

/**
 * إنشاء مصفوفة بقيمة افتراضية مكررة
 * Create array with repeated default value
 */
llvm::Value* LLVMArraySupport::createArrayWithDefault(llvm::Value* defaultValue, llvm::Value* count) {
    // إنشاء مصفوفة فارغة / Create empty array
    llvm::Value* array = createDynamicArray(defaultValue->getType());
    
    // استدعاء دالة runtime لملء المصفوفة / Call runtime to fill array
    std::vector<llvm::Value*> args = {array, defaultValue, count};
    callArrayRuntime("sad_array_fill", args);
    
    return array;
}

// ============================================================================
// الوصول والتعديل / Access and Modification
// ============================================================================

/**
 * الوصول لعنصر مع فحص الحدود
 * Access element with bounds checking
 */
llvm::Value* LLVMArraySupport::getElement(llvm::Value* array, llvm::Value* index) {
    // فحص الحدود / Check bounds
    checkBounds(array, index, false);
    
    // استدعاء دالة runtime للوصول / Call runtime for access
    std::vector<llvm::Value*> args = {array, index};
    return callArrayRuntime("sad_array_get", args);
}

/**
 * تعيين عنصر مع فحص الحدود
 * Set element with bounds checking
 */
void LLVMArraySupport::setElement(llvm::Value* array, llvm::Value* index, llvm::Value* value) {
    // فحص الحدود / Check bounds
    checkBounds(array, index, false);
    
    // استدعاء دالة runtime للتعيين / Call runtime for assignment
    std::vector<llvm::Value*> args = {array, index, value};
    callArrayRuntime("sad_array_set", args);
}

/**
 * إضافة عنصر في النهاية
 * Append element at end
 */
void LLVMArraySupport::appendElement(llvm::Value* array, llvm::Value* value) {
    // استدعاء دالة runtime للإضافة / Call runtime for append
    std::vector<llvm::Value*> args = {array, value};
    callArrayRuntime("sad_array_append", args);
}

/**
 * إدراج عنصر عند فهرس معين
 * Insert element at specific index
 */
void LLVMArraySupport::insertElement(llvm::Value* array, llvm::Value* index, llvm::Value* value) {
    // فحص الحدود (مع السماح بالإضافة في النهاية) / Check bounds (allow append)
    checkBounds(array, index, true);
    
    // استدعاء دالة runtime للإدراج / Call runtime for insert
    std::vector<llvm::Value*> args = {array, index, value};
    callArrayRuntime("sad_array_insert", args);
}

/**
 * حذف عنصر عند فهرس معين
 * Remove element at specific index
 */
void LLVMArraySupport::removeElement(llvm::Value* array, llvm::Value* index) {
    // فحص الحدود / Check bounds
    checkBounds(array, index, false);
    
    // استدعاء دالة runtime للحذف / Call runtime for remove
    std::vector<llvm::Value*> args = {array, index};
    callArrayRuntime("sad_array_remove", args);
}

// ============================================================================
// عمليات المصفوفات / Array Operations
// ============================================================================

/**
 * قص مصفوفة (slice)
 * Slice array
 */
llvm::Value* LLVMArraySupport::sliceArray(llvm::Value* array, llvm::Value* start, llvm::Value* end) {
    // فحص حدود البداية والنهاية / Check start and end bounds
    checkBounds(array, start, false);
    checkBounds(array, end, true);
    
    // استدعاء دالة runtime للقص / Call runtime for slice
    std::vector<llvm::Value*> args = {array, start, end};
    return callArrayRuntime("sad_array_slice", args);
}

/**
 * دمج مصفوفتين (concatenate)
 * Concatenate two arrays
 */
llvm::Value* LLVMArraySupport::concatenate(llvm::Value* array1, llvm::Value* array2) {
    // استدعاء دالة runtime للدمج / Call runtime for concatenation
    std::vector<llvm::Value*> args = {array1, array2};
    return callArrayRuntime("sad_array_concat", args);
}

/**
 * ترتيب المصفوفة (sort)
 * Sort array
 */
void LLVMArraySupport::sortArray(llvm::Value* array, llvm::Function* compareFn) {
    // استدعاء دالة runtime للترتيب / Call runtime for sorting
    std::vector<llvm::Value*> args = {array};
    if (compareFn) {
        args.push_back(compareFn);
    }
    callArrayRuntime("sad_array_sort", args);
}

/**
 * عكس المصفوفة (reverse)
 * Reverse array
 */
void LLVMArraySupport::reverseArray(llvm::Value* array) {
    // استدعاء دالة runtime للعكس / Call runtime for reverse
    std::vector<llvm::Value*> args = {array};
    callArrayRuntime("sad_array_reverse", args);
}

/**
 * تطبيق دالة على كل عنصر (map)
 * Apply function to each element (map)
 */
llvm::Value* LLVMArraySupport::mapArray(llvm::Value* array, llvm::Function* mapFn) {
    // استدعاء دالة runtime للتحويل / Call runtime for map
    std::vector<llvm::Value*> args = {array, mapFn};
    return callArrayRuntime("sad_array_map", args);
}

/**
 * تصفية المصفوفة (filter)
 * Filter array
 */
llvm::Value* LLVMArraySupport::filterArray(llvm::Value* array, llvm::Function* filterFn) {
    // استدعاء دالة runtime للتصفية / Call runtime for filter
    std::vector<llvm::Value*> args = {array, filterFn};
    return callArrayRuntime("sad_array_filter", args);
}

/**
 * دمج عناصر المصفوفة (reduce)
 * Reduce array elements
 */
llvm::Value* LLVMArraySupport::reduceArray(
    llvm::Value* array,
    llvm::Function* reduceFn,
    llvm::Value* initialValue)
{
    // استدعاء دالة runtime للدمج / Call runtime for reduce
    std::vector<llvm::Value*> args = {array, reduceFn, initialValue};
    return callArrayRuntime("sad_array_reduce", args);
}

// ============================================================================
// استعلامات المصفوفات / Array Queries
// ============================================================================

/**
 * الحصول على طول المصفوفة
 * Get array length
 */
llvm::Value* LLVMArraySupport::getLength(llvm::Value* array) {
    // الوصول لحقل length في الهيكل / Access length field in struct
    llvm::Value* lengthPtr = getFieldPtr(array, 1);  // الحقل الثاني / Second field
    return builder_.CreateLoad(builder_.getInt64Ty(), lengthPtr, "array_length");
}

/**
 * الحصول على السعة المخصصة
 * Get allocated capacity
 */
llvm::Value* LLVMArraySupport::getCapacity(llvm::Value* array) {
    // الوصول لحقل capacity في الهيكل / Access capacity field in struct
    llvm::Value* capacityPtr = getFieldPtr(array, 2);  // الحقل الثالث / Third field
    return builder_.CreateLoad(builder_.getInt64Ty(), capacityPtr, "array_capacity");
}

/**
 * التحقق من خلو المصفوفة
 * Check if array is empty
 */
llvm::Value* LLVMArraySupport::isEmpty(llvm::Value* array) {
    // الحصول على الطول ومقارنته بصفر / Get length and compare with zero
    llvm::Value* length = getLength(array);
    llvm::Value* zero = llvm::ConstantInt::get(builder_.getInt64Ty(), 0);
    return builder_.CreateICmpEQ(length, zero, "is_empty");
}

/**
 * البحث عن عنصر
 * Search for element
 */
llvm::Value* LLVMArraySupport::indexOf(llvm::Value* array, llvm::Value* value) {
    // استدعاء دالة runtime للبحث / Call runtime for search
    std::vector<llvm::Value*> args = {array, value};
    return callArrayRuntime("sad_array_index_of", args);
}

/**
 * التحقق من وجود عنصر
 * Check if element exists
 */
llvm::Value* LLVMArraySupport::contains(llvm::Value* array, llvm::Value* value) {
    // البحث عن العنصر والتحقق من النتيجة / Search and check result
    llvm::Value* index = indexOf(array, value);
    llvm::Value* minusOne = llvm::ConstantInt::get(builder_.getInt64Ty(), -1);
    return builder_.CreateICmpNE(index, minusOne, "contains");
}

// ============================================================================
// إدارة الذاكرة / Memory Management
// ============================================================================

/**
 * تحرير ذاكرة المصفوفة
 * Free array memory
 */
void LLVMArraySupport::freeArray(llvm::Value* array) {
    // استدعاء دالة runtime لتحرير الذاكرة / Call runtime to free memory
    std::vector<llvm::Value*> args = {array};
    callArrayRuntime("sad_array_free", args);
}

/**
 * نسخ مصفوفة (deep copy)
 * Clone array (deep copy)
 */
llvm::Value* LLVMArraySupport::cloneArray(llvm::Value* array) {
    // استدعاء دالة runtime للنسخ / Call runtime for cloning
    std::vector<llvm::Value*> args = {array};
    return callArrayRuntime("sad_array_clone", args);
}

/**
 * تغيير حجم المصفوفة
 * Resize array
 */
void LLVMArraySupport::resizeArray(llvm::Value* array, llvm::Value* newCapacity) {
    // استدعاء دالة runtime لتغيير الحجم / Call runtime to resize
    std::vector<llvm::Value*> args = {array, newCapacity};
    callArrayRuntime("sad_array_resize", args);
}

// ============================================================================
// دوال مساعدة / Helper Functions
// ============================================================================

/**
 * الحصول أو إنشاء نوع هيكل المصفوفة
 * Get or create array struct type
 */
llvm::StructType* LLVMArraySupport::getOrCreateArrayStructType() {
    // التحقق من وجود النوع مسبقاً / Check if type already exists
    if (arrayStructType_) {
        return arrayStructType_;
    }
    
    // إنشاء نوع الهيكل / Create struct type
    // Source: LLVM 18+ Opaque Pointers - استخدام PointerType::getUnqual بدلاً من getInt8PtrTy()
    std::vector<llvm::Type*> fields = {
        llvm::PointerType::getUnqual(context_),   // data pointer (void*)
        builder_.getInt64Ty(),     // length
        builder_.getInt64Ty(),     // capacity
        builder_.getInt32Ty(),     // element size
        llvm::PointerType::getUnqual(context_)    // element type info
    };
    
    return llvm::StructType::create(context_, fields, "SadArray");
}

/**
 * استدعاء دالة runtime للمصفوفات
 * Call array runtime function
 */
llvm::Value* LLVMArraySupport::callArrayRuntime(
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
 * فحص حدود الفهرس
 * Check index bounds
 */
void LLVMArraySupport::checkBounds(llvm::Value* array, llvm::Value* index, bool allowEqual) {
    // الحصول على طول المصفوفة / Get array length
    llvm:: Value* length = getLength(array);
    
    // إنشاء كتل للتحقق / Create blocks for checking
    llvm::Function* currentFunction = builder_.GetInsertBlock()->getParent();
    llvm::BasicBlock* boundsOkBlock = llvm::BasicBlock::Create(context_, "bounds_ok", currentFunction);
    llvm::BasicBlock* boundsErrorBlock = llvm::BasicBlock::Create(context_, "bounds_error", currentFunction);
    
    // التحقق من الحد الأدنى (index >= 0) / Check lower bound (index >= 0)
    llvm::Value* zero = llvm::ConstantInt::get(builder_.getInt64Ty(), 0);
    llvm::Value* indexNonNegative = builder_.CreateICmpSGE(index, zero, "index_non_negative");
    
    // التحقق من الحد الأعلى / Check upper bound
    llvm::Value* indexInBounds;
    if (allowEqual) {
        // index <= length (للإدراج في النهاية) / index <= length (for append)
        indexInBounds = builder_.CreateICmpSLE(index, length, "index_in_bounds");
    } else {
        // index < length (للوصول العادي) / index < length (for normal access)
        indexInBounds = builder_.CreateICmpSLT(index, length, "index_in_bounds");
    }
    
    // دمج الشرطين / Combine conditions
    llvm::Value* boundsOk = builder_.CreateAnd(indexNonNegative, indexInBounds, "bounds_ok");
    
    // الفرع بناءً على النتيجة / Branch based on result
    builder_.CreateCondBr(boundsOk, boundsOkBlock, boundsErrorBlock);
    
    // كتلة الخطأ: استدعاء دالة runtime للإبلاغ عن الخطأ / Error block: call runtime to report error
    builder_.SetInsertPoint(boundsErrorBlock);
    std::vector<llvm::Value*> errorArgs = {array, index, length};
    callArrayRuntime("sad_array_bounds_error", errorArgs);
    builder_.CreateUnreachable();  // لن نصل هنا / Should not reach here
    
    // الاستمرار في الكتلة الطبيعية / Continue in normal block
    builder_.SetInsertPoint(boundsOkBlock);
}

/**
 * الحصول على مؤشر حقل في الهيكل
 * Get field pointer in struct
 */
llvm::Value* LLVMArraySupport::getFieldPtr(llvm::Value* array, unsigned fieldIndex) {
    // إنشاء GEP للوصول للحقل / Create GEP to access field
    std::vector<llvm::Value*> indices = {
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0),  // فهرس الهيكل / Struct index
        llvm::ConstantInt::get(builder_.getInt32Ty(), fieldIndex)  // فهرس الحقل / Field index
    };
    
    return builder_.CreateGEP(arrayStructType_, array, indices, "field_ptr");
}

} // namespace LLVM
} // namespace Sad
