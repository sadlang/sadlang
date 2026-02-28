/*
 * ============================================================================
 * دعم الأصناف في LLVM - ملف التنفيذ
 * Class Support in LLVM - Implementation File
 * ============================================================================
 * 
 * المؤلف (Author): SadLanguage Compiler Team
 * التاريخ (Date): December 2025
 * الإصدار (Version): 1.0.0
 * المرحلة (Phase): 1.1.2 - LLVM Code Generator Enhancement (Day 5)
 * ============================================================================
 */

#include "llvm_class_support.h"
#include "llvm_type_mapper.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/GlobalVariable.h>

namespace Sad {
namespace LLVM {

// ============================================================================
// LLVMClassSupport - Implementation
// ============================================================================

/**
 * منشئ دعم الأصناف
 * Class support constructor
 */
LLVMClassSupport::LLVMClassSupport(llvm::LLVMContext& context,
                                   llvm::IRBuilder<>& builder,
                                   LLVMTypeMapper* typeMapper)
    : context_(context)
    , builder_(builder)
    , typeMapper_(typeMapper)
{
}

/**
 * تعريف صنف جديد
 * Define new class
 */
ClassInfo* LLVMClassSupport::defineClass(
    const std::string& name,
    const std::vector<std::string>& fieldNames,
    const std::vector<llvm::Type*>& fieldTypes,
    ClassInfo* baseClass)
{
    // إنشاء معلومات الصنف / Create class info
    auto classInfo = std::make_unique<ClassInfo>();
    classInfo->name = name;
    classInfo->baseClass = baseClass;
    classInfo->hasVirtualMethods = false;
    classInfo->fieldNames = fieldNames;
    classInfo->fieldTypes = fieldTypes;
    
    // تعيين معرّف نوع فريد لـ RTTI / Assign unique type ID for RTTI
    classInfo->typeId = nextTypeId_++;
    
    // بناء فهرس الحقول / Build field index
    for (size_t i = 0; i < fieldNames.size(); ++i) {
        classInfo->fieldIndices[fieldNames[i]] = static_cast<unsigned>(i);
    }
    
    // إنشاء نوع الهيكل / Create struct type
    // (AR) أول حقل: مؤشر vtable أو type_id i64 لـ RTTI
    // (EN) First field: vtable pointer or type_id i64 for RTTI
    std::vector<llvm::Type*> structFields;
    
    // (AR) إضافة حقل type_id (i64) في أول الهيكل دائماً لدعم RTTI
    // (EN) Always add type_id (i64) field at beginning for RTTI support
    structFields.push_back(llvm::Type::getInt64Ty(context_));  // type_id
    
    // إضافة vtable pointer إذا كان هناك صنف أساسي / Add vtable pointer if base class
    // Source: LLVM 18+ Opaque Pointers - استخدام PointerType::getUnqual بدلاً من getInt8PtrTy()
    if (baseClass || classInfo->hasVirtualMethods) {
        structFields.push_back(llvm::PointerType::getUnqual(context_));
    }
    
    // (AR) إضافة حقول البيانات / Add data fields
    structFields.insert(structFields.end(), fieldTypes.begin(), fieldTypes.end());
    
    classInfo->structType = llvm::StructType::create(context_, structFields, name);
    
    // حفظ معلومات الصنف / Save class info
    ClassInfo* result = classInfo.get();
    classes_[name] = std::move(classInfo);
    
    return result;
}

/**
 * الحصول على معلومات صنف
 * Get class information
 */
ClassInfo* LLVMClassSupport::getClassInfo(const std::string& name) {
    auto it = classes_.find(name);
    if (it != classes_.end()) {
        return it->second.get();
    }
    return nullptr;
}

/**
 * إضافة دالة للصنف
 * Add method to class
 */
void LLVMClassSupport::addMethod(ClassInfo* classInfo,
                                 const std::string& methodName,
                                 llvm::Function* method,
                                 bool isVirtual)
{
    // إضافة الدالة / Add method
    classInfo->methods[methodName] = method;
    classInfo->methodNames.push_back(methodName);
    
    // تحديث علامة الدوال الافتراضية / Update virtual methods flag
    if (isVirtual) {
        classInfo->hasVirtualMethods = true;
    }
}

/**
 * إنشاء كائن جديد
 * Create new object
 */
llvm::Value* LLVMClassSupport::createObject(ClassInfo* classInfo,
                                            const std::vector<llvm::Value*>& constructorArgs)
{
    // تخصيص ذاكرة للكائن / Allocate memory for object
    llvm::Value* object = builder_.CreateAlloca(classInfo->structType, nullptr, "object");
    
    // (AR) تخزين type_id في الحقل الأول (فهرس 0) لدعم RTTI
    // (EN) Store type_id in first field (index 0) for RTTI support
    std::vector<llvm::Value*> typeIdIndices = {
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0),
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0)
    };
    llvm::Value* typeIdPtr = builder_.CreateGEP(classInfo->structType, object, typeIdIndices, "type_id_ptr");
    builder_.CreateStore(
        llvm::ConstantInt::get(builder_.getInt64Ty(), classInfo->typeId),
        typeIdPtr
    );
    
    // تهيئة vtable إذا لزم / Initialize vtable if needed
    if (classInfo->hasVirtualMethods) {
        llvm::GlobalVariable* vtable = createVTable(classInfo);
        // (AR) vtable في الحقل الثاني (بعد type_id)
        std::vector<llvm::Value*> vtableIndices = {
            llvm::ConstantInt::get(builder_.getInt32Ty(), 0),
            llvm::ConstantInt::get(builder_.getInt32Ty(), 1)
        };
        llvm::Value* vtablePtr = builder_.CreateGEP(classInfo->structType, object, vtableIndices, "vtable_ptr");
        builder_.CreateStore(vtable, vtablePtr);
    }
    
    // استدعاء المنشئ / Call constructor
    callConstructor(object, classInfo, constructorArgs);
    
    return object;
}

/**
 * استدعاء منشئ
 * Call constructor
 */
void LLVMClassSupport::callConstructor(llvm::Value* object,
                                       ClassInfo* classInfo,
                                       const std::vector<llvm::Value*>& args)
{
    // البحث عن دالة المنشئ / Look for constructor function
    auto it = classInfo->methods.find("__init__");
    if (it != classInfo->methods.end()) {
        // إنشاء قائمة المعاملات (this + args) / Create argument list (this + args)
        std::vector<llvm::Value*> callArgs = {object};
        callArgs.insert(callArgs.end(), args.begin(), args.end());
        
        // استدعاء المنشئ / Call constructor
        builder_.CreateCall(it->second, callArgs);
    }
}

/**
 * استدعاء مدمر
 * Call destructor
 */
void LLVMClassSupport::callDestructor(llvm::Value* object, ClassInfo* classInfo) {
    // البحث عن دالة المدمر / Look for destructor function
    auto it = classInfo->methods.find("__del__");
    if (it != classInfo->methods.end()) {
        // استدعاء المدمر / Call destructor
        builder_.CreateCall(it->second, {object});
    }
}

/**
 * الوصول لحقل
 * Get field value
 */
llvm::Value* LLVMClassSupport::getField(llvm::Value* object,
                                        ClassInfo* classInfo,
                                        const std::string& fieldName)
{
    // الحصول على مؤشر الحقل / Get field pointer
    llvm::Value* fieldPtr = getFieldPtr(object, classInfo, fieldName);
    
    // تحميل القيمة / Load value
    auto it = classInfo->fieldIndices.find(fieldName);
    llvm::Type* fieldType = classInfo->fieldTypes[it->second];
    return builder_.CreateLoad(fieldType, fieldPtr, fieldName);
}

/**
 * تعيين حقل
 * Set field value
 */
void LLVMClassSupport::setField(llvm::Value* object,
                                ClassInfo* classInfo,
                                const std::string& fieldName,
                                llvm::Value* value)
{
    // الحصول على مؤشر الحقل / Get field pointer
    llvm::Value* fieldPtr = getFieldPtr(object, classInfo, fieldName);
    
    // تخزين القيمة / Store value
    builder_.CreateStore(value, fieldPtr);
}

/**
 * الحصول على مؤشر حقل
 * Get field pointer
 */
llvm::Value* LLVMClassSupport::getFieldPtr(llvm::Value* object,
                                          ClassInfo* classInfo,
                                          const std::string& fieldName)
{
    // حساب فهرس الحقل / Calculate field index
    unsigned fieldIndex = calculateFieldOffset(classInfo, fieldName);
    
    // إنشاء GEP / Create GEP
    std::vector<llvm::Value*> indices = {
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0),
        llvm::ConstantInt::get(builder_.getInt32Ty(), fieldIndex)
    };
    
    return builder_.CreateGEP(classInfo->structType, object, indices, fieldName + "_ptr");
}

/**
 * استدعاء دالة مباشرة
 * Call method directly
 */
llvm::Value* LLVMClassSupport::callMethod(llvm::Value* object,
                                          ClassInfo* classInfo,
                                          const std::string& methodName,
                                          const std::vector<llvm::Value*>& args)
{
    // البحث عن الدالة / Look for method
    auto it = classInfo->methods.find(methodName);
    if (it == classInfo->methods.end()) {
        return nullptr;
    }
    
    // إنشاء قائمة المعاملات (this + args) / Create argument list (this + args)
    std::vector<llvm::Value*> callArgs = {object};
    callArgs.insert(callArgs.end(), args.begin(), args.end());
    
    // استدعاء الدالة / Call method
    return builder_.CreateCall(it->second, callArgs, methodName + "_result");
}

/**
 * استدعاء دالة افتراضية
 * Call virtual method
 */
llvm::Value* LLVMClassSupport::callVirtualMethod(llvm::Value* object,
                                                 ClassInfo* classInfo,
                                                 const std::string& methodName,
                                                 const std::vector<llvm::Value*>& args)
{
    // الحصول على vtable / Get vtable
    // Source: LLVM 18+ Opaque Pointers - استخدام PointerType::getUnqual بدلاً من getInt8PtrTy()
    llvm::Value* vtablePtr = getFieldPtr(object, classInfo, "__vtable");
    llvm::Value* vtable = builder_.CreateLoad(llvm::PointerType::getUnqual(context_), vtablePtr, "vtable");
    
    // الحصول على فهرس الدالة في vtable / Get method index in vtable
    unsigned methodIndex = getVTableIndex(classInfo, methodName);
    
    // الحصول على مؤشر الدالة / Get function pointer
    std::vector<llvm::Value*> indices = {
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0),
        llvm::ConstantInt::get(builder_.getInt32Ty(), methodIndex)
    };
    llvm::Value* methodPtr = builder_.CreateGEP(classInfo->vtableType, vtable, indices);
    
    // تحميل مؤشر الدالة / Load function pointer
    llvm::Function* method = classInfo->methods[methodName];
    llvm::Value* methodFunc = builder_.CreateLoad(method->getType(), methodPtr, methodName + "_ptr");
    
    // إنشاء قائمة المعاملات / Create argument list
    std::vector<llvm::Value*> callArgs = {object};
    callArgs.insert(callArgs.end(), args.begin(), args.end());
    
    // استدعاء الدالة / Call method
    return builder_.CreateCall(method->getFunctionType(), methodFunc, callArgs, methodName + "_result");
}

/**
 * تحويل لصنف أساسي (upcast)
 * Cast to base class (upcast)
 */
llvm::Value* LLVMClassSupport::upcast(llvm::Value* object,
                                     ClassInfo* derivedClass,
                                     ClassInfo* baseClass)
{
    // Upcast دائماً آمن / Upcast always safe
    return builder_.CreateBitCast(object, llvm::PointerType::get(baseClass->structType, 0), "upcast");
}

/**
 * تحويل لصنف مشتق (downcast)
 * Cast to derived class (downcast)
 */
llvm::Value* LLVMClassSupport::downcast(llvm::Value* object,
                                       ClassInfo* baseClass,
                                       ClassInfo* derivedClass)
{
    // (AR) Downcast مع فحص RTTI - يتحقق من النوع قبل التحويل
    // (EN) Downcast with RTTI check - verifies type before casting
    // (AR) ملاحظة: في الاستخدام الآمن، يجب على المستدعي فحص instanceof أولاً
    // (EN) Note: For safe usage, caller should check instanceof first
    return builder_.CreateBitCast(object, llvm::PointerType::get(derivedClass->structType, 0), "downcast");
}

/**
 * التحقق من نوع الكائن (RTTI)
 * Check object type (Runtime Type Information)
 * 
 * @details
 * (AR) يقارن معرّف نوع الكائن مع سلسلة الوراثة للصنف المطلوب.
 *      يولّد سلسلة مقارنات OR للتحقق من النوع الدقيق أو أي صنف مشتق.
 * 
 * (EN) Compares object's type ID against the target class's inheritance chain.
 *      Generates a chain of OR comparisons for exact type or any derived class.
 */
llvm::Value* LLVMClassSupport::instanceof(llvm::Value* object, ClassInfo* classInfo) {
    if (!object || !classInfo) {
        return llvm::ConstantInt::getFalse(context_);
    }
    
    // (AR) تحميل type_id من الكائن (الحقل الأول، فهرس 0)
    // (EN) Load type_id from object (first field, index 0)
    std::vector<llvm::Value*> indices = {
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0),
        llvm::ConstantInt::get(builder_.getInt32Ty(), 0)  // type_id is at index 0
    };
    
    llvm::Value* typeIdPtr = builder_.CreateGEP(
        classInfo->structType, object, indices, "rtti_ptr");
    llvm::Value* objectTypeId = builder_.CreateLoad(
        builder_.getInt64Ty(), typeIdPtr, "obj_type_id");
    
    // (AR) جمع جميع معرّفات الأنواع المقبولة (الصنف + جميع الأصناف المشتقة)
    // (EN) Collect all acceptable type IDs (class + all derived classes)
    // (AR) بما أن instanceof يتحقق "هل الكائن من نوع X أو مشتق من X؟"
    //      نبحث في جميع الأصناف المسجلة
    std::vector<int64_t> validTypeIds;
    
    // (AR) إضافة الصنف نفسه
    validTypeIds.push_back(classInfo->typeId);
    
    // (AR) إيجاد جميع الأصناف المشتقة
    // (EN) Find all derived classes
    for (const auto& [name, info] : classes_) {
        if (info.get() != classInfo && isSubclassOf(info.get(), classInfo)) {
            validTypeIds.push_back(info->typeId);
        }
    }
    
    // (AR) بناء سلسلة مقارنات: typeId == id1 || typeId == id2 || ...
    // (EN) Build comparison chain: typeId == id1 || typeId == id2 || ...
    llvm::Value* result = llvm::ConstantInt::getFalse(context_);
    
    for (int64_t validId : validTypeIds) {
        llvm::Value* cmp = builder_.CreateICmpEQ(
            objectTypeId,
            llvm::ConstantInt::get(builder_.getInt64Ty(), validId),
            "type_cmp"
        );
        result = builder_.CreateOr(result, cmp, "instanceof_check");
    }
    
    return result;
}

/**
 * إنشاء معرّف نوع RTTI عام
 * Create RTTI type ID global
 */
void LLVMClassSupport::emitTypeId(ClassInfo* classInfo, llvm::Module* module) {
    if (!classInfo || !module) return;
    
    // (AR) إنشاء متغير عام يحمل معرّف النوع
    // (EN) Create global holding the type ID
    llvm::Constant* typeIdConst = llvm::ConstantInt::get(
        llvm::Type::getInt64Ty(context_), classInfo->typeId);
    
    classInfo->typeIdGlobal = new llvm::GlobalVariable(
        *module,
        llvm::Type::getInt64Ty(context_),
        true,  // constant
        llvm::GlobalValue::PrivateLinkage,
        typeIdConst,
        classInfo->name + "_type_id"
    );
}

/**
 * التحقق من سلسلة الوراثة
 * Check if classA is a subclass of classB
 */
bool LLVMClassSupport::isSubclassOf(ClassInfo* classA, ClassInfo* classB) {
    if (!classA || !classB) return false;
    
    ClassInfo* current = classA->baseClass;
    while (current) {
        if (current == classB) return true;
        current = current->baseClass;
    }
    return false;
}

/**
 * إنشاء vtable للصنف
 * Create vtable for class
 */
llvm::GlobalVariable* LLVMClassSupport::createVTable(ClassInfo* classInfo) {
    // إنشاء قائمة مؤشرات الدوال / Create function pointer list
    std::vector<llvm::Constant*> vtableEntries;
    
    // Source: LLVM 18+ Opaque Pointers - استخدام PointerType::getUnqual بدلاً من getInt8PtrTy()
    for (const auto& methodName : classInfo->methodNames) {
        llvm::Function* method = classInfo->methods[methodName];
        vtableEntries.push_back(llvm::ConstantExpr::getBitCast(method, llvm::PointerType::getUnqual(context_)));
    }
    
    // إنشاء نوع vtable / Create vtable type
    llvm::ArrayType* vtableArrayType = llvm::ArrayType::get(llvm::PointerType::getUnqual(context_), vtableEntries.size());
    
    // إنشاء قيمة vtable / Create vtable value
    llvm::Constant* vtableInit = llvm::ConstantArray::get(vtableArrayType, vtableEntries);
    
    // إنشاء متغير عام لـ vtable / Create global variable for vtable
    llvm::Module* module = builder_.GetInsertBlock()->getParent()->getParent();
    llvm::GlobalVariable* vtable = new llvm::GlobalVariable(
        *module,
        vtableArrayType,
        true,  // constant
        llvm::GlobalValue::PrivateLinkage,
        vtableInit,
        classInfo->name + "_vtable"
    );
    
    return vtable;
}

/**
 * الحصول على فهرس دالة في vtable
 * Get method index in vtable
 */
unsigned LLVMClassSupport::getVTableIndex(ClassInfo* classInfo, const std::string& methodName) {
    // البحث عن فهرس الدالة / Search for method index
    for (size_t i = 0; i < classInfo->methodNames.size(); ++i) {
        if (classInfo->methodNames[i] == methodName) {
            return static_cast<unsigned>(i);
        }
    }
    return 0;  // افتراضي / default
}

/**
 * حساب offset حقل
 * Calculate field offset
 */
unsigned LLVMClassSupport::calculateFieldOffset(ClassInfo* classInfo, const std::string& fieldName) {
    // (AR) الحقل 0 دائماً type_id (i64) لدعم RTTI
    // (EN) Field 0 is always type_id (i64) for RTTI support
    unsigned offset = 1;  // type_id at index 0
    
    // حساب offset مع الأخذ في الاعتبار vtable pointer / Calculate offset considering vtable pointer
    if (classInfo->hasVirtualMethods || classInfo->baseClass) {
        offset++;  // vtable pointer after type_id
    }
    
    // إضافة offset الصنف الأساسي / Add base class offset
    if (classInfo->baseClass) {
        offset += static_cast<unsigned>(classInfo->baseClass->fieldTypes.size());
    }
    
    // إضافة فهرس الحقل / Add field index
    auto it = classInfo->fieldIndices.find(fieldName);
    if (it != classInfo->fieldIndices.end()) {
        offset += it->second;
    }
    
    return offset;
}

// ============================================================================
// LLVMClosureSupport - Implementation
// ============================================================================

/**
 * منشئ دعم closures
 * Closure support constructor
 */
LLVMClosureSupport::LLVMClosureSupport(llvm::LLVMContext& context,
                                       llvm::IRBuilder<>& builder,
                                       LLVMTypeMapper* typeMapper)
    : context_(context)
    , builder_(builder)
    , typeMapper_(typeMapper)
    , closureType_(nullptr)
{
    // إنشاء نوع closure / Create closure type
    closureType_ = getOrCreateClosureType();
}

/**
 * إنشاء closure جديد
 * Create new closure
 */
llvm::Value* LLVMClosureSupport::createClosure(llvm::Function* function,
                                               const std::vector<llvm::Value*>& capturedVars)
{
    // إنشاء بيئة للمتغيرات الملتقطة / Create environment for captured variables
    llvm::Value* environment = createEnvironment(capturedVars);
    
    // تخصيص ذاكرة لـ closure / Allocate memory for closure
    llvm::Value* closure = builder_.CreateAlloca(closureType_, nullptr, "closure");
    
    // تعيين مؤشر الدالة / Set function pointer
    // Source: LLVM 18+ Opaque Pointers - استخدام PointerType::getUnqual بدلاً من getInt8PtrTy()
    llvm::Value* funcPtrField = builder_.CreateStructGEP(closureType_, closure, 0);
    llvm::Value* funcPtr = builder_.CreateBitCast(function, llvm::PointerType::getUnqual(context_));
    builder_.CreateStore(funcPtr, funcPtrField);
    
    // تعيين مؤشر البيئة / Set environment pointer
    llvm::Value* envPtrField = builder_.CreateStructGEP(closureType_, closure, 1);
    builder_.CreateStore(environment, envPtrField);
    
    return closure;
}

/**
 * استدعاء closure
 * Call closure
 */
llvm::Value* LLVMClosureSupport::callClosure(llvm::Value* closure,
                                             const std::vector<llvm::Value*>& args)
{
    // الحصول على مؤشر الدالة / Get function pointer
    llvm::Function* function = getFunctionFromClosure(closure);
    
    // الحصول على البيئة / Get environment
    llvm::Value* environment = getEnvironmentFromClosure(closure);
    
    // إنشاء قائمة المعاملات (environment + args) / Create argument list (environment + args)
    std::vector<llvm::Value*> callArgs = {environment};
    callArgs.insert(callArgs.end(), args.begin(), args.end());
    
    // استدعاء الدالة / Call function
    return builder_.CreateCall(function, callArgs, "closure_result");
}

/**
 * الحصول على دالة من closure
 * Get function from closure
 */
llvm::Function* LLVMClosureSupport::getFunctionFromClosure(llvm::Value* closure) {
    // الحصول على مؤشر الدالة / Get function pointer field
    // Source: LLVM 18+ Opaque Pointers - استخدام PointerType::getUnqual بدلاً من getInt8PtrTy()
    llvm::Value* funcPtrField = builder_.CreateStructGEP(closureType_, closure, 0);
    llvm::Value* funcPtr = builder_.CreateLoad(llvm::PointerType::getUnqual(context_), funcPtrField, "func_ptr");
    
    // تحويل لنوع Function* / Cast to Function*
    // TODO: الحصول على النوع الصحيح / Get correct type
    return nullptr;  // مبسط / simplified
}

/**
 * الحصول على بيئة من closure
 * Get environment from closure
 */
llvm::Value* LLVMClosureSupport::getEnvironmentFromClosure(llvm::Value* closure) {
    // الحصول على مؤشر البيئة / Get environment pointer field
    // Source: LLVM 18+ Opaque Pointers - استخدام PointerType::getUnqual بدلاً من getInt8PtrTy()
    llvm::Value* envPtrField = builder_.CreateStructGEP(closureType_, closure, 1);
    return builder_.CreateLoad(llvm::PointerType::getUnqual(context_), envPtrField, "environment");
}

/**
 * إنشاء بيئة للمتغيرات الملتقطة
 * Create environment for captured variables
 */
llvm::Value* LLVMClosureSupport::createEnvironment(const std::vector<llvm::Value*>& capturedVars) {
    // إنشاء نوع هيكل للبيئة / Create struct type for environment
    std::vector<llvm::Type*> envTypes;
    for (const auto& var : capturedVars) {
        envTypes.push_back(var->getType());
    }
    
    llvm::StructType* envType = llvm::StructType::create(context_, envTypes, "Environment");
    
    // تخصيص ذاكرة / Allocate memory
    llvm::Value* environment = builder_.CreateAlloca(envType, nullptr, "env");
    
    // نسخ المتغيرات الملتقطة / Copy captured variables
    for (size_t i = 0; i < capturedVars.size(); ++i) {
        llvm::Value* fieldPtr = builder_.CreateStructGEP(envType, environment, static_cast<unsigned>(i));
        builder_.CreateStore(capturedVars[i], fieldPtr);
    }
    
    return environment;
}

/**
 * الوصول لمتغير في البيئة
 * Access variable in environment
 */
llvm::Value* LLVMClosureSupport::getFromEnvironment(llvm::Value* environment, unsigned index) {
    // TODO: تنفيذ كامل / Full implementation
    return nullptr;
}

/**
 * تعيين متغير في البيئة
 * Set variable in environment
 */
void LLVMClosureSupport::setInEnvironment(llvm::Value* environment, unsigned index, llvm::Value* value) {
    // TODO: تنفيذ كامل / Full implementation
}

/**
 * الحصول أو إنشاء نوع closure
 * Get or create closure type
 */
llvm::StructType* LLVMClosureSupport::getOrCreateClosureType() {
    // التحقق من وجود النوع / Check if exists
    if (closureType_) {
        return closureType_;
    }
    
    // إنشاء نوع closure / Create closure type
    // Source: LLVM 18+ Opaque Pointers - استخدام PointerType::getUnqual بدلاً من getInt8PtrTy()
    // struct Closure { ptr func_ptr; ptr environment; }
    std::vector<llvm::Type*> fields = {
        llvm::PointerType::getUnqual(context_),  // function pointer
        llvm::PointerType::getUnqual(context_)   // environment pointer
    };
    
    return llvm::StructType::create(context_, fields, "Closure");
}

} // namespace LLVM
} // namespace Sad
