/*
 * ============================================================================
 * بناء التعابير المعقدة في LLVM - ملف التنفيذ
 * LLVM Complex Expression Builder - Implementation File
 * ============================================================================
 * 
 * هذا الملف يحتوي على تنفيذ بناء التعابير المعقدة في LLVM IR
 * This file contains implementation of complex expression builder in LLVM IR
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement
 * ============================================================================
 */

#include "llvm_expression_builder.h"
#include "llvm_type_mapper.h"
#include "llvm_memory_manager.h"  // إضافة مدير الذاكرة / Add memory manager
#include "llvm_array_support.h"
#include "llvm_dict_support.h"
#include "llvm_class_support.h"
#include <llvm/IR/Constants.h>

namespace Sad {
namespace LLVM {

// ============================================================================
// المنشئ / Constructor
// ============================================================================

/**
 * منشئ بناء التعابير - تهيئة المكونات
 * Expression builder constructor - Initialize components
 */
LLVMExpressionBuilder::LLVMExpressionBuilder(llvm::LLVMContext& context,
                                             llvm::IRBuilder<>& builder,
                                             LLVMTypeMapper* typeMapper,
                                             LLVMMemoryManager* memoryManager)
    : context_(context)
    , builder_(builder)
    , typeMapper_(typeMapper)
    , memoryManager_(memoryManager)  // حفظ مؤشر مدير الذاكرة / Store memory manager pointer
{
    // تهيئة دعم المصفوفات والقواميس / Initialize array and dict support
    arraySupport_ = std::make_unique<LLVMArraySupport>(context, builder, typeMapper);
    dictSupport_ = std::make_unique<LLVMDictSupport>(context, builder, typeMapper);
    
    // تهيئة دعم الأصناف والـ closures / Initialize class and closure support
    classSupport_ = std::make_unique<LLVMClassSupport>(context, builder, typeMapper);
    closureSupport_ = std::make_unique<LLVMClosureSupport>(context, builder, typeMapper);
}

// ============================================================================
// تعابير المصفوفات / Array Expressions
// ============================================================================

/**
 * بناء إنشاء مصفوفة
 * Build array creation
 */
llvm::Value* LLVMExpressionBuilder::buildArrayCreate(
    llvm::Type* elementType,
    const std::vector<llvm::Value*>& elements)
{
    // الحصول على عدد العناصر / Get number of elements
    size_t numElements = elements.size();
    
    // إنشاء نوع مصفوفة / Create array type
    llvm::ArrayType* arrayType = llvm::ArrayType::get(elementType, numElements);
    
    // تخصيص ذاكرة للمصفوفة / Allocate memory for array
    llvm::Value* arrayAlloca = builder_.CreateAlloca(arrayType, nullptr, "array");
    
    // ملء المصفوفة بالعناصر / Fill array with elements
    for (size_t i = 0; i < numElements; ++i) {
        // حساب عنوان العنصر / Calculate element address
        std::vector<llvm::Value*> indices = {
            llvm::ConstantInt::get(builder_.getInt32Ty(), 0),  // فهرس المصفوفة / Array index
            llvm::ConstantInt::get(builder_.getInt32Ty(), i)   // فهرس العنصر / Element index
        };
        
        // الحصول على مؤشر العنصر / Get element pointer
        llvm::Value* elementPtr = builder_.CreateGEP(arrayType, arrayAlloca, indices, "element_ptr");
        
        // تخزين العنصر / Store element
        builder_.CreateStore(elements[i], elementPtr);
    }
    
    // إذا كان مدير الذاكرة متوفراً، تسجيل للتحرير التلقائي / If memory manager available, register for auto-release
    if (memoryManager_) {
        // تسجيل المصفوفة للتحرير عند نهاية scope / Register array for release at scope end
        memoryManager_->registerForAutoRelease(arrayAlloca);
    }
    
    return arrayAlloca;  // إرجاع المصفوفة / Return array
}

/**
 * بناء الوصول لعنصر مصفوفة
 * Build array element access
 */
llvm::Value* LLVMExpressionBuilder::buildArrayAccess(llvm::Value* array, llvm::Value* index) {
    // الحصول على نوع المصفوفة / Get array type
    llvm::Type* arrayType = array->getType()->getPointerElementType();
    
    // إنشاء فهارس GEP / Create GEP indices
    std::vector<llvm::Value*> indices = {
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0),  // فهرس المصفوفة / Array index
        index                                               // فهرس العنصر / Element index
    };
    
    // الحصول على مؤشر العنصر / Get element pointer
    llvm::Value* elementPtr = builder_.CreateGEP(arrayType, array, indices, "array_access");
    
    // تحميل العنصر / Load element
    return builder_.CreateLoad(elementPtr->getType()->getPointerElementType(), elementPtr, "element");
}

/**
 * بناء تعيين عنصر مصفوفة
 * Build array element assignment
 */
void LLVMExpressionBuilder::buildArraySet(llvm::Value* array, llvm::Value* index, llvm::Value* value) {
    // الحصول على نوع المصفوفة / Get array type
    llvm::Type* arrayType = array->getType()->getPointerElementType();
    
    // إنشاء فهارس GEP / Create GEP indices
    std::vector<llvm::Value*> indices = {
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0),  // فهرس المصفوفة / Array index
        index                                               // فهرس العنصر / Element index
    };
    
    // الحصول على مؤشر العنصر / Get element pointer
    llvm::Value* elementPtr = builder_.CreateGEP(arrayType, array, indices, "array_set_ptr");
    
    // تخزين القيمة الجديدة / Store new value
    builder_.CreateStore(value, elementPtr);
}

/**
 * بناء طول المصفوفة
 * Build array length
 */
llvm::Value* LLVMExpressionBuilder::buildArrayLength(llvm::Value* array) {
    // الحصول على نوع المصفوفة / Get array type
    llvm::ArrayType* arrayType = llvm::dyn_cast<llvm::ArrayType>(
        array->getType()->getPointerElementType()
    );
    
    if (arrayType) {
        // إرجاع الحجم كثابت / Return size as constant
        uint64_t size = arrayType->getNumElements();
        return llvm::ConstantInt::get(builder_.getInt64Ty(), size);
    }
    
    // إذا لم يكن نوع مصفوفة ثابت، استدعاء دالة runtime / If not static array, call runtime function
    return callRuntimeFunction("sad_array_length", {array});
}

// ============================================================================
// تعابير القواميس / Dictionary Expressions
// ============================================================================

/**
 * بناء إنشاء قاموس
 * Build dictionary creation
 */
llvm::Value* LLVMExpressionBuilder::buildDictCreate(llvm::Type* keyType, llvm::Type* valueType) {
    // استدعاء دالة runtime لإنشاء قاموس / Call runtime function to create dictionary
    std::vector<llvm::Value*> args = {
        llvm::ConstantInt::get(builder_.getInt32Ty(), 16)  // الحجم الابتدائي / Initial capacity
    };
    
    // إنشاء القاموس / Create dictionary
    llvm::Value* dict = callRuntimeFunction("sad_dict_create", args);
    
    // إذا كان مدير الذاكرة متوفراً، تسجيل للتحرير التلقائي / If memory manager available, register for auto-release
    if (memoryManager_) {
        // تسجيل القاموس للتحرير عند نهاية scope / Register dictionary for release at scope end
        memoryManager_->registerForAutoRelease(dict);
    }
    
    return dict;  // إرجاع القاموس / Return dictionary
}

/**
 * بناء الوصول لقيمة في قاموس
 * Build dictionary value access
 */
llvm::Value* LLVMExpressionBuilder::buildDictAccess(llvm::Value* dict, llvm::Value* key) {
    // استدعاء دالة runtime للبحث / Call runtime function for lookup
    return callRuntimeFunction("sad_dict_get", {dict, key});
}

/**
 * بناء تعيين قيمة في قاموس
 * Build dictionary value assignment
 */
void LLVMExpressionBuilder::buildDictSet(llvm::Value* dict, llvm::Value* key, llvm::Value* value) {
    // استدعاء دالة runtime للتعيين / Call runtime function for assignment
    callRuntimeFunction("sad_dict_set", {dict, key, value});
}

/**
 * بناء حجم القاموس
 * Build dictionary size
 */
llvm::Value* LLVMExpressionBuilder::buildDictSize(llvm::Value* dict) {
    // استدعاء دالة runtime لحساب الحجم / Call runtime function for size
    return callRuntimeFunction("sad_dict_size", {dict});
}

// ============================================================================
// تعابير الأصناف / Class Expressions
// ============================================================================

/**
 * بناء إنشاء كائن
 * Build object creation
 */
llvm::Value* LLVMExpressionBuilder::buildObjectCreate(
    llvm::Type* classType,
    const std::vector<llvm::Value*>& args)
{
    llvm::Value* objectPtr = nullptr;
    
    // إذا كان مدير الذاكرة متوفراً، استخدم التخصيص المُدار بـ ARC / If memory manager available, use ARC-managed allocation
    if (memoryManager_) {
        // تخصيص كائن مُدار مع ARC / Allocate ARC-managed object
        objectPtr = memoryManager_->allocateObject(classType, "managed_object");
        
        // تهيئة ref count تلقائياً إلى 1 / Ref count automatically initialized to 1
        // ملاحظة: allocateObject يهيئ ref count داخلياً / Note: allocateObject initializes ref count internally
    }
    else {
        // تخصيص ذاكرة عادية على الـ stack / Allocate regular stack memory
        objectPtr = builder_.CreateAlloca(classType, nullptr, "object");
    }
    
    // استدعاء المنشئ إذا وُجد / Call constructor if exists
    // TODO: تنفيذ استدعاء المنشئ مع المعاملات / TODO: Implement constructor call with args
    
    return objectPtr;  // إرجاع الكائن / Return object
}

/**
 * بناء الوصول لحقل
 * Build field access
 */
llvm::Value* LLVMExpressionBuilder::buildFieldAccess(llvm::Value* object, const std::string& fieldName) {
    // TODO: تنفيذ الوصول للحقل بناءً على البيانات الوصفية للصنف
    // TODO: Implement field access based on class metadata
    
    // حالياً: استخدام GEP بفهرس 0 كمثال / Currently: Use GEP with index 0 as example
    llvm::Type* objectType = object->getType()->getPointerElementType();
    std::vector<llvm::Value*> indices = {
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0),
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0)  // TODO: استخدام فهرس الحقل الحقيقي
    };
    
    llvm::Value* fieldPtr = builder_.CreateGEP(objectType, object, indices, fieldName + "_ptr");
    return builder_.CreateLoad(fieldPtr->getType()->getPointerElementType(), fieldPtr, fieldName);
}

/**
 * بناء تعيين حقل
 * Build field assignment
 */
void LLVMExpressionBuilder::buildFieldSet(llvm::Value* object, const std::string& fieldName, llvm::Value* value) {
    // TODO: تنفيذ تعيين الحقل بناءً على البيانات الوصفية للصنف
    // TODO: Implement field assignment based on class metadata
    
    llvm::Type* objectType = object->getType()->getPointerElementType();
    std::vector<llvm::Value*> indices = {
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0),
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0)  // TODO: استخدام فهرس الحقل الحقيقي
    };
    
    llvm::Value* fieldPtr = builder_.CreateGEP(objectType, object, indices, fieldName + "_ptr");
    builder_.CreateStore(value, fieldPtr);
}

/**
 * بناء استدعاء دالة عضو
 * Build method call
 */
llvm::Value* LLVMExpressionBuilder::buildMethodCall(
    llvm::Value* object,
    const std::string& methodName,
    const std::vector<llvm::Value*>& args)
{
    // TODO: تنفيذ استدعاء الدالة العضو مع vtable lookup
    // TODO: Implement method call with vtable lookup
    
    // حالياً: استدعاء مباشر (بدون vtable) / Currently: Direct call (without vtable)
    std::vector<llvm::Value*> fullArgs = {object};  // إضافة this / Add this
    fullArgs.insert(fullArgs.end(), args.begin(), args.end());
    
    return callRuntimeFunction("sad_method_" + methodName, fullArgs);
}

// ============================================================================
// تعابير الدوال / Function Expressions
// ============================================================================

/**
 * بناء استدعاء دالة
 * Build function call
 */
llvm::Value* LLVMExpressionBuilder::buildFunctionCall(
    llvm::Function* function,
    const std::vector<llvm::Value*>& args)
{
    // إنشاء استدعاء مباشر / Create direct call
    return builder_.CreateCall(function, args, "call");
}

/**
 * بناء استدعاء دالة غير مباشر (مؤشر دالة)
 * Build indirect function call (function pointer)
 */
llvm::Value* LLVMExpressionBuilder::buildIndirectCall(
    llvm::Value* functionPtr,
    const std::vector<llvm::Value*>& args)
{
    // الحصول على نوع الدالة / Get function type
    llvm::FunctionType* funcType = llvm::cast<llvm::FunctionType>(
        functionPtr->getType()->getPointerElementType()
    );
    
    // إنشاء استدعاء غير مباشر / Create indirect call
    return builder_.CreateCall(funcType, functionPtr, args, "indirect_call");
}

/**
 * بناء closure (دالة متداخلة)
 * Build closure (nested function)
 */
llvm::Value* LLVMExpressionBuilder::buildClosure(
    llvm::Function* function,
    const std::vector<llvm::Value*>& capturedVars)
{
    // TODO: تنفيذ كامل للـ closures مع environment capturing
    // TODO: Complete implementation of closures with environment capturing
    
    // حالياً: إرجاع مؤشر الدالة فقط / Currently: Return function pointer only
    return function;
}

// ============================================================================
// Comprehensions
// ============================================================================

/**
 * بناء list comprehension
 * Build list comprehension
 */
llvm::Value* LLVMExpressionBuilder::buildListComprehension(
    llvm::Value* elementExpr,
    const std::string& iterVar,
    llvm::Value* iterableExpr,
    llvm::Value* condition)
{
    // TODO: تنفيذ كامل لـ list comprehension
    // TODO: Complete implementation of list comprehension
    
    // حالياً: إرجاع مصفوفة فارغة / Currently: Return empty array
    return buildArrayCreate(builder_.getInt64Ty(), {});
}

/**
 * بناء dict comprehension
 * Build dict comprehension
 */
llvm::Value* LLVMExpressionBuilder::buildDictComprehension(
    llvm::Value* keyExpr,
    llvm::Value* valueExpr,
    const std::string& iterVar,
    llvm::Value* iterableExpr,
    llvm::Value* condition)
{
    // TODO: تنفيذ كامل لـ dict comprehension
    // TODO: Complete implementation of dict comprehension
    
    // حالياً: إرجاع قاموس فارغ / Currently: Return empty dictionary
    return buildDictCreate(builder_.getInt64Ty(), builder_.getInt64Ty());
}

// ============================================================================
// تعابير متقدمة / Advanced Expressions
// ============================================================================

/**
 * بناء ternary operator (شرط ? صحيح : خطأ)
 * Build ternary operator (condition ? true : false)
 */
llvm::Value* LLVMExpressionBuilder::buildTernary(
    llvm::Value* condition,
    llvm::Value* trueValue,
    llvm::Value* falseValue)
{
    // استخدام select instruction (أسرع من if/else) / Use select instruction (faster than if/else)
    return builder_.CreateSelect(condition, trueValue, falseValue, "ternary");
}

/**
 * بناء string interpolation
 * Build string interpolation
 */
llvm::Value* LLVMExpressionBuilder::buildStringInterpolation(
    const std::string& format,
    const std::vector<llvm::Value*>& values)
{
    // TODO: تنفيذ كامل لـ string interpolation
    // TODO: Complete implementation of string interpolation
    
    // حالياً: استدعاء دالة runtime / Currently: Call runtime function
    std::vector<llvm::Value*> args = {
        builder_.CreateGlobalStringPtr(format)  // صيغة النص / Format string
    };
    args.insert(args.end(), values.begin(), values.end());
    
    return callRuntimeFunction("sad_string_format", args);
}

/**
 * بناء range expression (مدى)
 * Build range expression
 */
llvm::Value* LLVMExpressionBuilder::buildRange(
    llvm::Value* start,
    llvm::Value* end,
    llvm::Value* step)
{
    // تعيين الخطوة الافتراضية إذا لم تُعطَ / Set default step if not given
    if (!step) {
        step = llvm::ConstantInt::get(builder_.getInt64Ty(), 1);
    }
    
    // استدعاء دالة runtime لإنشاء المدى / Call runtime function to create range
    return callRuntimeFunction("sad_range_create", {start, end, step});
}

// ============================================================================
// دوال مساعدة / Helper Functions
// ============================================================================

/**
 * إنشاء runtime call لدالة مساعدة
 * Create runtime call to helper function
 */
llvm::Value* LLVMExpressionBuilder::callRuntimeFunction(
    const std::string& functionName,
    const std::vector<llvm::Value*>& args)
{
    // الحصول على الوحدة / Get module
    llvm::Module* module = builder_.GetInsertBlock()->getParent()->getParent();
    
    // البحث عن الدالة أو إنشائها / Look up function or create it
    llvm::Function* function = module->getFunction(functionName);
    
    if (!function) {
        // إنشاء توقيع الدالة / Create function signature
        std::vector<llvm::Type*> paramTypes;
        for (const auto& arg : args) {
            paramTypes.push_back(arg->getType());
        }
        
        // نوع الرجوع الافتراضي: i8* (مؤشر عام) / Default return type: i8* (generic pointer)
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

/**
 * التحقق من صحة الفهرس
 * Validate index
 */
void LLVMExpressionBuilder::validateIndex(llvm::Value* index, llvm::Value* size) {
    // TODO: إضافة فحص الحدود / TODO: Add bounds checking
    // يمكن إضافة assert أو استدعاء دالة runtime للتحقق
    // Can add assert or call runtime function for verification
}

} // namespace LLVM
} // namespace Sad
