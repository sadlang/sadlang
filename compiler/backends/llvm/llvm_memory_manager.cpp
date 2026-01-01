/*
 * ============================================================================
 * إدارة الذاكرة التلقائية و ARC - ملف التنفيذ
 * Automatic Memory Management & ARC - Implementation File
 * ============================================================================
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement (Days 6-7)
 * ============================================================================
 */

#include "llvm_memory_manager.h"
#include "llvm_type_mapper.h"
#include <llvm/IR/Constants.h>

namespace Sad {
namespace LLVM {

// ============================================================================
// المنشئ / Constructor
// ============================================================================

/**
 * منشئ مدير الذاكرة
 * Memory manager constructor
 */
LLVMMemoryManager::LLVMMemoryManager(llvm::LLVMContext& context,
                                     llvm::IRBuilder<>& builder,
                                     LLVMTypeMapper* typeMapper)
    : context_(context)
    , builder_(builder)
    , typeMapper_(typeMapper)
    , gcEnabled_(true)
    , refCountedType_(nullptr)
{
    // إنشاء نوع ref-counted / Create ref-counted type
    refCountedType_ = getOrCreateRefCountedType();
}

// ============================================================================
// تخصيص الذاكرة / Memory Allocation
// ============================================================================

/**
 * تخصيص كائن مُدار
 * Allocate managed object
 */
llvm::Value* LLVMMemoryManager::allocateObject(llvm::Type* type, const std::string& name) {
    // استدعاء دالة runtime للتخصيص / Call runtime for allocation
    // الحصول على Module من builder / Get Module from builder
    llvm::Module* module = builder_.GetInsertBlock()->getParent()->getParent();
    llvm::Value* size = llvm::ConstantInt::get(
        builder_.getInt64Ty(),
        module->getDataLayout().getTypeAllocSize(type)
    );
    
    std::vector<llvm::Value*> args = {size};
    llvm::Value* object = callMemoryRuntime("sad_alloc", args);
    
    // تهيئة ref count إلى 1 / Initialize ref count to 1
    setRefCount(object, llvm::ConstantInt::get(builder_.getInt64Ty(), 1));
    
    // تسجيل الكائن / Register object
    managedObjects_.insert(object);
    
    return object;
}

/**
 * تخصيص مصفوفة مُدارة
 * Allocate managed array
 */
llvm::Value* LLVMMemoryManager::allocateArray(llvm::Type* elementType, llvm::Value* count) {
    // حساب الحجم الكلي / Calculate total size
    // الحصول على Module من builder / Get Module from builder
    llvm::Module* module = builder_.GetInsertBlock()->getParent()->getParent();
    llvm::Value* elementSize = llvm::ConstantInt::get(
        builder_.getInt64Ty(),
        module->getDataLayout().getTypeAllocSize(elementType)
    );
    llvm::Value* totalSize = builder_.CreateMul(elementSize, count, "array_size");
    
    // تخصيص / Allocate
    std::vector<llvm::Value*> args = {totalSize, count};
    llvm::Value* array = callMemoryRuntime("sad_alloc_array", args);
    
    // تهيئة ref count / Initialize ref count
    setRefCount(array, llvm::ConstantInt::get(builder_.getInt64Ty(), 1));
    
    // تسجيل / Register
    managedObjects_.insert(array);
    
    return array;
}

/**
 * تخصيص ذاكرة خام
 * Allocate raw memory
 */
llvm::Value* LLVMMemoryManager::allocateRaw(llvm::Value* size) {
    // استدعاء malloc مباشرة / Call malloc directly
    std::vector<llvm::Value*> args = {size};
    return callMemoryRuntime("malloc", args);
}

// ============================================================================
// إدارة المراجع / Reference Management
// ============================================================================

/**
 * زيادة عداد المراجع (retain)
 * Increment reference count (retain)
 */
void LLVMMemoryManager::retain(llvm::Value* object) {
    // الحصول على مؤشر ref count / Get ref count pointer
    llvm::Value* refCountPtr = getRefCountPtr(object);
    
    // تحميل القيمة الحالية / Load current value
    llvm::Value* currentCount = builder_.CreateLoad(builder_.getInt64Ty(), refCountPtr, "current_ref");
    
    // زيادة / Increment
    llvm::Value* newCount = builder_.CreateAdd(
        currentCount,
        llvm::ConstantInt::get(builder_.getInt64Ty(), 1),
        "new_ref"
    );
    
    // تخزين القيمة الجديدة / Store new value
    builder_.CreateStore(newCount, refCountPtr);
}

/**
 * تقليل عداد المراجع (release)
 * Decrement reference count (release)
 */
void LLVMMemoryManager::release(llvm::Value* object) {
    // الحصول على مؤشر ref count / Get ref count pointer
    llvm::Value* refCountPtr = getRefCountPtr(object);
    
    // تحميل القيمة الحالية / Load current value
    llvm::Value* currentCount = builder_.CreateLoad(builder_.getInt64Ty(), refCountPtr, "current_ref");
    
    // تقليل / Decrement
    llvm::Value* newCount = builder_.CreateSub(
        currentCount,
        llvm::ConstantInt::get(builder_.getInt64Ty(), 1),
        "new_ref"
    );
    
    // تخزين القيمة الجديدة / Store new value
    builder_.CreateStore(newCount, refCountPtr);
    
    // التحقق من الوصول للصفر / Check if reached zero
    llvm::Value* isZero = builder_.CreateICmpEQ(
        newCount,
        llvm::ConstantInt::get(builder_.getInt64Ty(), 0),
        "is_zero"
    );
    
    // إنشاء كتل / Create blocks
    llvm::Function* currentFunction = builder_.GetInsertBlock()->getParent();
    llvm::BasicBlock* deallocBlock = llvm::BasicBlock::Create(context_, "dealloc", currentFunction);
    llvm::BasicBlock* continueBlock = llvm::BasicBlock::Create(context_, "continue", currentFunction);
    
    // فرع / Branch
    builder_.CreateCondBr(isZero, deallocBlock, continueBlock);
    
    // كتلة التحرير / Dealloc block
    builder_.SetInsertPoint(deallocBlock);
    deallocate(object);
    builder_.CreateBr(continueBlock);
    
    // كتلة الاستمرار / Continue block
    builder_.SetInsertPoint(continueBlock);
}

/**
 * نقل ملكية (move)
 * Transfer ownership (move)
 */
llvm::Value* LLVMMemoryManager::move(llvm::Value* object) {
    // Move لا يغير ref count / Move doesn't change ref count
    // فقط نقل الملكية / Just transfer ownership
    return object;
}

/**
 * إنشاء مرجع ضعيف
 * Create weak reference
 */
llvm::Value* LLVMMemoryManager::createWeakRef(llvm::Value* object) {
    // استدعاء دالة runtime لإنشاء weak ref / Call runtime to create weak ref
    std::vector<llvm::Value*> args = {object};
    return callMemoryRuntime("sad_create_weak_ref", args);
}

/**
 * ترقية مرجع ضعيف
 * Upgrade weak reference
 */
llvm::Value* LLVMMemoryManager::upgradeWeakRef(llvm::Value* weakRef) {
    // استدعاء دالة runtime للترقية / Call runtime to upgrade
    std::vector<llvm::Value*> args = {weakRef};
    llvm::Value* strongRef = callMemoryRuntime("sad_upgrade_weak_ref", args);
    
    // زيادة ref count إذا نجح / Increment ref count if successful
    llvm::Value* isNull = builder_.CreateICmpEQ(
        strongRef,
        llvm::ConstantPointerNull::get(llvm::cast<llvm::PointerType>(strongRef->getType())),
        "is_null"
    );
    
    llvm::Function* currentFunction = builder_.GetInsertBlock()->getParent();
    llvm::BasicBlock* retainBlock = llvm::BasicBlock::Create(context_, "retain", currentFunction);
    llvm::BasicBlock* continueBlock = llvm::BasicBlock::Create(context_, "continue", currentFunction);
    
    builder_.CreateCondBr(isNull, continueBlock, retainBlock);
    
    builder_.SetInsertPoint(retainBlock);
    retain(strongRef);
    builder_.CreateBr(continueBlock);
    
    builder_.SetInsertPoint(continueBlock);
    
    return strongRef;
}

// ============================================================================
// عمليات ARC التلقائية / Automatic ARC Operations
// ============================================================================

/**
 * إدراج retain تلقائية
 * Auto-insert retain
 */
void LLVMMemoryManager::autoRetain(llvm::Value* object) {
    // إدراج retain تلقائياً / Auto-insert retain
    retain(object);
}

/**
 * إدراج release تلقائية
 * Auto-insert release
 */
void LLVMMemoryManager::autoRelease(llvm::Value* object) {
    // إضافة للمجموعة التلقائية / Add to autorelease pool
    registerForAutoRelease(object);
}

/**
 * تسجيل للتحرير التلقائي
 * Register for automatic release
 */
void LLVMMemoryManager::registerForAutoRelease(llvm::Value* object) {
    // إضافة لمجموعة التحرير / Add to release pool
    autoReleasePool_.push_back(object);
}

/**
 * تحرير جميع الكائنات المُسجلة
 * Release all registered objects
 */
void LLVMMemoryManager::releaseAll() {
    // تحرير جميع الكائنات في المجموعة / Release all in pool
    for (auto object : autoReleasePool_) {
        release(object);
    }
    
    // مسح المجموعة / Clear pool
    autoReleasePool_.clear();
}

// ============================================================================
// تحرير الذاكرة / Memory Deallocation
// ============================================================================

/**
 * تحرير كائن
 * Deallocate object
 */
void LLVMMemoryManager::deallocate(llvm::Value* object) {
    // استدعاء دالة runtime للتحرير / Call runtime to deallocate
    std::vector<llvm::Value*> args = {object};
    callMemoryRuntime("sad_dealloc", args);
    
    // إزالة من المجموعة المُدارة / Remove from managed set
    managedObjects_.erase(object);
}

/**
 * تحرير مصفوفة
 * Deallocate array
 */
void LLVMMemoryManager::deallocateArray(llvm::Value* array) {
    // استدعاء دالة runtime للتحرير / Call runtime to deallocate
    std::vector<llvm::Value*> args = {array};
    callMemoryRuntime("sad_dealloc_array", args);
    
    // إزالة من المجموعة / Remove from set
    managedObjects_.erase(array);
}

/**
 * تحرير ذاكرة خام
 * Deallocate raw memory
 */
void LLVMMemoryManager::deallocateRaw(llvm::Value* ptr) {
    // استدعاء free مباشرة / Call free directly
    std::vector<llvm::Value*> args = {ptr};
    callMemoryRuntime("free", args);
}

// ============================================================================
// كشف الدورات / Cycle Detection
// ============================================================================

/**
 * كشف دورات المراجع
 * Detect reference cycles
 */
bool LLVMMemoryManager::detectCycles(llvm::Value* object) {
    // TODO: تنفيذ خوارزمية كشف الدورات / Implement cycle detection algorithm
    // يمكن استخدام mark-and-sweep أو colored pointers
    return false;  // مبسط / simplified
}

/**
 * كسر دورات المراجع
 * Break reference cycles
 */
void LLVMMemoryManager::breakCycles(llvm::Value* object) {
    // TODO: تنفيذ كسر الدورات / Implement cycle breaking
    // تحويل بعض المراجع إلى weak
}

/**
 * وضع علامة على القابل للوصول
 * Mark reachable objects
 */
void LLVMMemoryManager::markReachable(llvm::Value* root) {
    // TODO: تنفيذ marking phase من mark-and-sweep
}

/**
 * مسح غير القابل للوصول
 * Sweep unreachable objects
 */
void LLVMMemoryManager::sweepUnreachable() {
    // TODO: تنفيذ sweep phase
    // تحرير الكائنات غير الموضوع عليها علامة
}

// ============================================================================
// دوال مساعدة ARC / ARC Helper Functions
// ============================================================================

/**
 * الحصول على عدد المراجع
 * Get reference count
 */
llvm::Value* LLVMMemoryManager::getRefCount(llvm::Value* object) {
    // الحصول على مؤشر ref count / Get ref count pointer
    llvm::Value* refCountPtr = getRefCountPtr(object);
    
    // تحميل القيمة / Load value
    return builder_.CreateLoad(builder_.getInt64Ty(), refCountPtr, "ref_count");
}

/**
 * تعيين عدد المراجع
 * Set reference count
 */
void LLVMMemoryManager::setRefCount(llvm::Value* object, llvm::Value* count) {
    // الحصول على مؤشر ref count / Get ref count pointer
    llvm::Value* refCountPtr = getRefCountPtr(object);
    
    // تخزين القيمة / Store value
    builder_.CreateStore(count, refCountPtr);
}

/**
 * التحقق من كون الكائن حياً
 * Check if object is alive
 */
llvm::Value* LLVMMemoryManager::isAlive(llvm::Value* object) {
    // الحصول على ref count / Get ref count
    llvm::Value* refCount = getRefCount(object);
    
    // المقارنة بصفر / Compare with zero
    return builder_.CreateICmpSGT(
        refCount,
        llvm::ConstantInt::get(builder_.getInt64Ty(), 0),
        "is_alive"
    );
}

// ============================================================================
// GC Hooks
// ============================================================================

/**
 * تسجيل GC hook
 * Register GC hook
 */
void LLVMMemoryManager::registerGCHook(llvm::Function* hook) {
    gcHooks_.push_back(hook);
}

/**
 * استدعاء GC hooks
 * Invoke GC hooks
 */
void LLVMMemoryManager::invokeGCHooks() {
    // استدعاء جميع الـ hooks المسجلة / Call all registered hooks
    for (auto hook : gcHooks_) {
        builder_.CreateCall(hook, {});
    }
}

/**
 * تمكين/تعطيل GC
 * Enable/disable GC
 */
void LLVMMemoryManager::setGCEnabled(bool enabled) {
    gcEnabled_ = enabled;
}

// ============================================================================
// تحسينات الأداء / Performance Optimizations
// ============================================================================

/**
 * دمج retain/release المتتالية
 * Merge consecutive retain/release
 */
void LLVMMemoryManager::optimizeRetainRelease() {
    // TODO: تنفيذ تحسين retain/release
    // مسح الأزواج المتتالية retain->release
}

/**
 * إزالة retain/release غير الضرورية
 * Remove unnecessary retain/release
 */
void LLVMMemoryManager::eliminateRedundantOperations() {
    // TODO: تحليل data flow وإزالة العمليات الزائدة
}

/**
 * تجميع التحريرات
 * Batch deallocations
 */
void LLVMMemoryManager::batchDeallocations() {
    // TODO: تجميع عمليات التحرير لتحسين الأداء
}

// ============================================================================
// دوال مساعدة خاصة / Private Helper Functions
// ============================================================================

/**
 * إنشاء رأس الكائن المُدار
 * Create managed object header
 */
llvm::Value* LLVMMemoryManager::createObjectHeader(llvm::Type* type) {
    // TODO: إنشاء header يحتوي على ref count ومعلومات النوع
    return nullptr;
}

/**
 * الحصول على مؤشر ref count
 * Get ref count pointer
 */
llvm::Value* LLVMMemoryManager::getRefCountPtr(llvm::Value* object) {
    // ref count في بداية الكائن (offset 0) / ref count at start of object (offset 0)
    return builder_.CreateStructGEP(refCountedType_, object, 0, "ref_count_ptr");
}

/**
 * الحصول أو إنشاء نوع ref-counted
 * Get or create ref-counted type
 */
llvm::StructType* LLVMMemoryManager::getOrCreateRefCountedType() {
    // التحقق من وجوده / Check if exists
    if (refCountedType_) {
        return refCountedType_;
    }
    
    // إنشاء نوع / Create type
    // struct RefCounted { i64 refCount; i8* data; }
    std::vector<llvm::Type*> fields = {
        builder_.getInt64Ty(),   // ref count
        builder_.getInt8PtrTy()  // data pointer
    };
    
    return llvm::StructType::create(context_, fields, "RefCounted");
}

/**
 * استدعاء دالة runtime للذاكرة
 * Call memory runtime function
 */
llvm::Value* LLVMMemoryManager::callMemoryRuntime(
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
        llvm::Type* returnType = builder_.getInt8PtrTy();
        
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

} // namespace LLVM
} // namespace Sad
