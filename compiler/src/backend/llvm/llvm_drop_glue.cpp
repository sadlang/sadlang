// تعطيل تحذير Unicode للتعليقات العربية
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file llvm_drop_glue.cpp
 * @brief تنفيذ نظام توليد كود التنظيف (Drop Glue) / Drop Glue Code Generator
 * @author فريق لغة ص
 * @date يناير 2026
 * @version 1.0
 * @phase م-هـ03: توليد كود Drop/Destructor بدون GC
 * 
 *     هذا الملف يُولّد كود LLVM IR لتنظيف الموارد تلقائياً عند نهاية النطاقات.
 *     في وضع الملكية الصارمة (بلا GC)، كل مورد يجب أن يُحرَّر تلقائياً.
 *     
 *     النمط المُتبع مشابه لـ Rust drop glue:
 *     - لكل نوع يحتاج تنظيف، تُولّد دالة __sad_drop_<Type>
 *     - عند نهاية النطاق، تُستدعى drop للمتغيرات بترتيب LIFO
 *     - القيم المنقولة لا تُنظَّف (تم نقل الملكية)
 *     - الأنواع البدائية لا تحتاج تنظيف
 */

#include "backend/llvm/llvm_drop_glue.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>

#include <iostream>
#include <algorithm>
#include "safe_arithmetic.h" // (AR) تحويل آمن مع كشف الفيض / (EN) bounds-checked size_t->int

namespace Sad {
namespace LLVM {

// =============================================================================
//                    البناء والهدم / Construction & Destruction
// =============================================================================

LLVMDropGlue::LLVMDropGlue(llvm::LLVMContext& context, llvm::Module& module)
    : context_(context)
    , module_(module)
    , nextScopeId_(0) {
    
    // (AR) تسجيل الأنواع البدائية التي لا تحتاج تنظيف
    // (EN) Register primitive types that don't need cleanup
    primitiveTypes_ = {
        // أنواع صحيحة / Integer types
        "ع8", "ع16", "ع32", "ع64", "ع128",
        "i8", "i16", "i32", "i64", "i128",
        "ط8", "ط16", "ط32", "ط64",
        "", "u16", "u32", "u64",
        
        // أنواع عشرية / Float types
        "ع32ف", "ع64ف",
        "f32", "f64", "float", "double",
        
        // أنواع أخرى / Other types
        "منطق", "bool",
        "حرف", "char",
        "فراغ", "void",
        "مؤشر_خام", "raw_ptr",
        
        // أنواع حجم / Size types
        "حجم", "usize", "isize",
    };
}

LLVMDropGlue::~LLVMDropGlue() = default;

// =============================================================================
//                    تسجيل الأنواع / Type Registration
// =============================================================================

void LLVMDropGlue::registerDroppableType(const DropTypeInfo& info) {
    if (info.isPrimitive()) {
        return;  // (AR) الأنواع البدائية لا تُسجَّل
    }
    
    registeredTypes_[info.typeName] = info;
}

bool LLVMDropGlue::needsDrop(const std::string& typeName) const {
    // (AR) الأنواع البدائية لا تحتاج drop
    if (isPrimitiveType(typeName)) {
        return false;
    }
    
    // (AR) التحقق من الأنواع المسجلة
    auto it = registeredTypes_.find(typeName);
    if (it != registeredTypes_.end()) {
        return it->second.needsDrop();
    }
    
    // (AR) افتراضي: الأنواع غير المعروفة قد تحتاج drop
    return true;
}

const DropTypeInfo* LLVMDropGlue::getDropInfo(const std::string& typeName) const {
    auto it = registeredTypes_.find(typeName);
    if (it != registeredTypes_.end()) {
        return &it->second;
    }
    return nullptr;
}

// =============================================================================
//                    إدارة النطاقات / Scope Management
// =============================================================================

size_t LLVMDropGlue::enterScope() {
    size_t id = nextScopeId_++;
    scopeStack_.push_back(DropScope{id, {}});
    return id;
}

void LLVMDropGlue::registerVariable(const ScopedVariable& var) {
    if (scopeStack_.empty()) {
        return;  // (AR) لا نطاق نشط
    }
    scopeStack_.back().variables.push_back(var);
}

void LLVMDropGlue::markMoved(const std::string& varName) {
    // (AR) البحث من الأعلى (النطاق الداخلي أولاً)
    for (auto it = scopeStack_.rbegin(); it != scopeStack_.rend(); ++it) {
        for (auto& var : it->variables) {
            if (var.name == varName) {
                var.isMoved = true;
                
                // (AR) إذا كان هناك علم drop، حدّثه في IR
                // (EN) If there's a drop flag, update it in IR
                // سيُحدَّث في emitScopeExit
                return;
            }
        }
    }
}

// =============================================================================
//                    توليد كود الخروج من النطاق
//                    Scope Exit Code Generation
// =============================================================================

void LLVMDropGlue::emitScopeExit(llvm::IRBuilder<>& builder) {
    if (scopeStack_.empty()) {
        return;
    }
    
    DropScope& scope = scopeStack_.back();
    
    // (AR) هدم المتغيرات بترتيب عكسي (LIFO)
    // (EN) Drop variables in reverse order (LIFO)
    for (auto it = scope.variables.rbegin(); it != scope.variables.rend(); ++it) {
        ScopedVariable& var = *it;
        
        // (AR) تخطي الأنواع البدائية
        if (var.dropInfo.isPrimitive()) {
            continue;
        }
        
        // (AR) تخطي القيم المنقولة (بلا شرط)
        if (var.isMoved && var.dropFlag == nullptr) {
            continue;
        }
        
        // (AR) إذا كان هناك علم drop، ولّد فحصاً شرطياً
        // (EN) If drop flag exists, generate conditional check
        if (var.dropFlag != nullptr) {
            // تحميل علم التنظيف
            llvm::Value* flagVal = builder.CreateLoad(
                llvm::Type::getInt1Ty(context_), 
                var.dropFlag,
                var.name + ".dropflag");
            
            // إنشاء فروع: drop vs skip
            llvm::Function* currentFn = builder.GetInsertBlock()->getParent();
            llvm::BasicBlock* dropBB = llvm::BasicBlock::Create(
                context_, "drop." + var.name, currentFn);
            llvm::BasicBlock* skipBB = llvm::BasicBlock::Create(
                context_, "skip." + var.name, currentFn);
            
            builder.CreateCondBr(flagVal, dropBB, skipBB);
            
            // كتلة التنظيف
            builder.SetInsertPoint(dropBB);
            emitDropCall(builder, var);
            builder.CreateBr(skipBB);
            
            // متابعة بعد التنظيف
            builder.SetInsertPoint(skipBB);
        } else {
            // (AR) drop غير مشروط (المتغير لم يُنقل)
            emitDropCall(builder, var);
        }
    }
    
    // (AR) إزالة النطاق من المكدس
    scopeStack_.pop_back();
}

// =============================================================================
//                    توليد استدعاءات Drop
//                    Drop Call Generation
// =============================================================================

void LLVMDropGlue::emitDropCall(llvm::IRBuilder<>& builder,
                                 const ScopedVariable& var) {
    if (var.dropInfo.isPrimitive() || var.alloca == nullptr) {
        return;
    }
    
    switch (var.dropInfo.dropKind) {
        case DropKind::None:
            // (AR) لا شيء للفعل
            break;
            
        case DropKind::CustomDestructor: {
            // (AR) استدعاء دالة هدم() المخصصة
            // (EN) Call custom destructor function
            std::string dtorName = var.dropInfo.destructorName;
            if (dtorName.empty()) {
                dtorName = getDropFunctionName(var.dropInfo.typeName);
            }
            
            llvm::Function* dropFn = module_.getFunction(dtorName);
            if (!dropFn) {
                // (AR) إنشاء إعلان الدالة إذا لم تكن موجودة
                llvm::FunctionType* fnType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(context_),
                    {llvm::PointerType::getUnqual(context_)},
                    false);
                dropFn = llvm::Function::Create(
                    fnType, llvm::Function::ExternalLinkage, dtorName, module_);
            }
            
            builder.CreateCall(dropFn, {var.alloca});
            break;
        }
        
        case DropKind::Deallocate: {
            // (AR) تحرير الذاكرة المُخصصة
            // (EN) Deallocate heap memory
            std::string deallocName = "__sad_dealloc";
            llvm::Function* deallocFn = module_.getFunction(deallocName);
            if (!deallocFn) {
                llvm::FunctionType* fnType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(context_),
                    {llvm::PointerType::getUnqual(context_)},
                    false);
                deallocFn = llvm::Function::Create(
                    fnType, llvm::Function::ExternalLinkage, deallocName, module_);
            }
            
            // (AR) تحميل المؤشر وتحريره
            llvm::Value* ptr = builder.CreateLoad(
                llvm::PointerType::getUnqual(context_), var.alloca);
            builder.CreateCall(deallocFn, {ptr});
            break;
        }
        
        case DropKind::StructFields: {
            // (AR) هدم حقول البنية بترتيب عكسي
            // (EN) Drop struct fields in reverse order
            emitStructDrop(builder, var.alloca, var.dropInfo);
            break;
        }
        
        case DropKind::ArrayElements: {
            // (AR) هدم عناصر المصفوفة
            // (EN) Drop array elements
            emitArrayDrop(builder, var.alloca, var.dropInfo);
            break;
        }
        
        case DropKind::EnumVariant: {
            // (AR) فحص المميّز ثم هدم الحالة المناسبة
            // (EN) Check discriminant then drop appropriate variant
            
            // (AR) تحميل المميّز (الحقل الأول دائماً في تمثيل tagged union)
            // (EN) Load discriminant (always first field in tagged union representation)
            llvm::Type* enumType = llvm::StructType::getTypeByName(context_, var.dropInfo.typeName);
            
            if (enumType && enumType->isStructTy()) {
                // (AR) GEP للحقل 0 = المميّز (i32)
                // (EN) GEP to field 0 = discriminant (i32)
                llvm::Value* discPtr = builder.CreateStructGEP(
                    enumType, var.alloca, 0, "disc.ptr");
                llvm::Value* disc = builder.CreateLoad(
                    llvm::Type::getInt32Ty(context_), discPtr, "disc.val");
                
                // (AR) إنشاء switch على المميّز — لكل حالة استدعاء drop المناسب
                // (EN) Create switch on discriminant — call appropriate drop for each variant
                llvm::Function* currentFn = builder.GetInsertBlock()->getParent();
                llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(
                    context_, "enum.drop.merge", currentFn);
                
                // (AR) الافتراضي = القفز للنهاية (حالات بدائية لا تحتاج هدم)
                // (EN) Default = jump to merge (primitive variants need no drop)
                llvm::SwitchInst* sw = builder.CreateSwitch(disc, mergeBB,
                    static_cast<unsigned>(var.dropInfo.fieldTypes.size()));
                
                // (AR) لكل حالة تعداد — فحص إذا تحتاج drop
                // (EN) For each enum variant — check if it needs drop
                for (size_t vi = 0; vi < var.dropInfo.fieldTypes.size(); ++vi) {
                    const DropTypeInfo& vInfo = var.dropInfo.fieldTypes[vi];
                    if (vInfo.isPrimitive()) continue;  // لا تحتاج هدم
                    
                    llvm::BasicBlock* caseBB = llvm::BasicBlock::Create(
                        context_, "enum.drop.case." + std::to_string(vi), currentFn);
                    sw->addCase(
                        llvm::ConstantInt::get(llvm::Type::getInt32Ty(context_), vi),
                        caseBB);
                    
                    builder.SetInsertPoint(caseBB);
                    std::string variantDropName = getDropFunctionName(vInfo.typeName);
                    llvm::Function* variantDropFn = module_.getFunction(variantDropName);
                    if (variantDropFn) {
                        builder.CreateCall(variantDropFn, {var.alloca});
                    }
                    builder.CreateBr(mergeBB);
                }
                
                builder.SetInsertPoint(mergeBB);
            } else {
                // (AR) fallback: استدعاء drop عام بدون تمييز
                // (EN) Fallback: call generic drop without discriminant check
                std::string dropFnName = getDropFunctionName(var.dropInfo.typeName);
                llvm::Function* dropFn = module_.getFunction(dropFnName);
                if (dropFn) {
                    builder.CreateCall(dropFn, {var.alloca});
                }
            }
            break;
        }
    }
}

llvm::AllocaInst* LLVMDropGlue::emitDropFlag(
    llvm::IRBuilder<>& builder,
    const std::string& varName) {
    
    // (AR) إنشاء علم drop: i1 مُهيأ بـ true (يحتاج drop)
    // (EN) Create drop flag: i1 initialized to true (needs drop)
    llvm::Function* currentFn = builder.GetInsertBlock()->getParent();
    llvm::IRBuilder<> entryBuilder(
        &currentFn->getEntryBlock(),
        currentFn->getEntryBlock().begin());
    
    llvm::AllocaInst* flag = entryBuilder.CreateAlloca(
        llvm::Type::getInt1Ty(context_), nullptr,
        varName + ".dropflag.addr");
    
    // (AR) تهيئة بـ true (يحتاج تنظيف حتى يُنقل)
    builder.CreateStore(
        llvm::ConstantInt::getTrue(context_), flag);
    
    // (AR) تسجيل العلم في المتغير المناسب
    for (auto it = scopeStack_.rbegin(); it != scopeStack_.rend(); ++it) {
        for (auto& var : it->variables) {
            if (var.name == varName) {
                var.dropFlag = flag;
                return flag;
            }
        }
    }
    
    return flag;
}

// =============================================================================
//                    توليد دوال Drop
//                    Drop Function Generation
// =============================================================================

void LLVMDropGlue::generateAllDropFunctions(
    llvm::IRBuilder<>& builder) {
    
    for (const auto& [typeName, info] : registeredTypes_) {
        if (info.needsDrop() && dropFunctions_.find(typeName) == dropFunctions_.end()) {
            llvm::Function* fn = generateDropFunction(info);
            if (fn) {
                dropFunctions_[typeName] = fn;
            }
        }
    }
}

llvm::Function* LLVMDropGlue::generateDropFunction(const DropTypeInfo& info) {
    std::string fnName = getDropFunctionName(info.typeName);
    
    // (AR) التحقق إذا كانت الدالة موجودة بالفعل
    llvm::Function* existing = module_.getFunction(fnName);
    if (existing && !existing->empty()) {
        return existing;  // (AR) تم التوليد مسبقاً
    }
    
    // (AR) إنشاء نوع الدالة: void(ptr)
    llvm::FunctionType* fnType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(context_),
        {llvm::PointerType::getUnqual(context_)},
        false);
    
    llvm::Function* dropFn = llvm::Function::Create(
        fnType, llvm::Function::InternalLinkage, fnName, module_);
    
    // (AR) تسمية المعامل
    llvm::Argument* selfArg = dropFn->arg_begin();
    selfArg->setName("self");
    
    // (AR) إنشاء كتلة الدخول
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(
        context_, "entry", dropFn);
    
    llvm::IRBuilder<> fnBuilder(entry);
    
    switch (info.dropKind) {
        case DropKind::CustomDestructor: {
            // (AR) استدعاء المُدمِّر المخصص
            if (!info.destructorName.empty()) {
                llvm::Function* dtor = module_.getFunction(info.destructorName);
                if (!dtor) {
                    // (AR) إعلان خارجي
                    llvm::FunctionType* dtorType = llvm::FunctionType::get(
                        llvm::Type::getVoidTy(context_),
                        {llvm::PointerType::getUnqual(context_)},
                        false);
                    dtor = llvm::Function::Create(
                        dtorType, llvm::Function::ExternalLinkage,
                        info.destructorName, module_);
                }
                fnBuilder.CreateCall(dtor, {selfArg});
            }
            break;
        }
        
        case DropKind::StructFields: {
            // (AR) هدم حقول البنية بترتيب عكسي
            emitStructDrop(fnBuilder, selfArg, info);
            break;
        }
        
        case DropKind::ArrayElements: {
            // (AR) هدم عناصر المصفوفة
            emitArrayDrop(fnBuilder, selfArg, info);
            break;
        }
        
        case DropKind::Deallocate: {
            // (AR) تحرير الذاكرة
            std::string deallocName = "__sad_dealloc";
            llvm::Function* deallocFn = module_.getFunction(deallocName);
            if (!deallocFn) {
                llvm::FunctionType* deallocType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(context_),
                    {llvm::PointerType::getUnqual(context_)},
                    false);
                deallocFn = llvm::Function::Create(
                    deallocType, llvm::Function::ExternalLinkage,
                    deallocName, module_);
            }
            llvm::Value* ptr = fnBuilder.CreateLoad(
                llvm::PointerType::getUnqual(context_), selfArg);
            fnBuilder.CreateCall(deallocFn, {ptr});
            break;
        }
        
        default:
            break;
    }
    
    fnBuilder.CreateRetVoid();
    return dropFn;
}

// =============================================================================
//                    توليد كود Drop للأنواع المركبة
//                    Composite Type Drop Code Generation
// =============================================================================

void LLVMDropGlue::emitStructDrop(llvm::IRBuilder<>& builder,
                                   llvm::Value* structPtr,
                                   const DropTypeInfo& info) {
    // (AR) هدم الحقول بترتيب عكسي
    // (EN) Drop fields in reverse order
    
    const auto& fields = info.fieldTypes;
    
    for (int i = Sad::Security::SafeArithmetic::assertSafeCast<int>(fields.size(), "llvm_drop_glue_size") - 1; i >= 0; --i) {
        const DropTypeInfo& field = fields[i];
        
        if (field.isPrimitive()) {
            continue;  // (AR) الأنواع البدائية لا تحتاج drop
        }
        
        // (AR) الحصول على النوع الهيكلي من LLVM
        llvm::Type* structType = nullptr;
        if (auto* ptrType = llvm::dyn_cast<llvm::PointerType>(structPtr->getType())) {
            // (AR) للمؤشرات غير الشفافة (opaque pointers)
            // نحتاج النوع الهيكلي من المعلومات المسجلة
            structType = llvm::StructType::getTypeByName(context_, info.typeName);
        }
        
        // (AR) حساب مؤشر الحقل
        llvm::Value* fieldPtr = nullptr;
        if (structType && structType->isStructTy()) {
            fieldPtr = builder.CreateStructGEP(structType, structPtr, i,
                                               "field." + std::to_string(i));
        } else {
            // (AR) fallback: استخدام GEP عام
            fieldPtr = builder.CreateConstGEP2_32(
                llvm::Type::getInt8Ty(context_), structPtr, 0, i,
                "field." + std::to_string(i));
        }
        
        // (AR) استدعاء drop للحقل
        std::string fieldDropName = getDropFunctionName(field.typeName);
        llvm::Function* fieldDropFn = module_.getFunction(fieldDropName);
        if (fieldDropFn) {
            builder.CreateCall(fieldDropFn, {fieldPtr});
        } else if (field.dropKind == DropKind::Deallocate) {
            // (AR) تحرير مباشر
            std::string deallocName = "__sad_dealloc";
            llvm::Function* deallocFn = module_.getFunction(deallocName);
            if (deallocFn) {
                llvm::Value* ptr = builder.CreateLoad(
                    llvm::PointerType::getUnqual(context_), fieldPtr);
                builder.CreateCall(deallocFn, {ptr});
            }
        }
    }
}

void LLVMDropGlue::emitArrayDrop(llvm::IRBuilder<>& builder,
                                  llvm::Value* arrayPtr,
                                  const DropTypeInfo& info) {
    if (!info.elementType || info.elementType->isPrimitive()) {
        return;  // (AR) الأنواع البدائيّة لا تحتاج drop / (EN) primitives need no drop glue
    }
    
    if (info.arraySize == 0) {
        return;  // (AR) مصفوفة فارغة
    }
    
    // (AR) هدم العناصر من الأخير إلى الأول
    // (EN) Drop elements from last to first
    llvm::Type* elemType = llvm::Type::getInt8Ty(context_);  // placeholder
    
    for (int i = static_cast<int>(info.arraySize) - 1; i >= 0; --i) {
        // (AR) حساب مؤشر العنصر
        llvm::Value* elemPtr = builder.CreateConstGEP2_32(
            llvm::ArrayType::get(elemType, info.arraySize),
            arrayPtr, 0, i,
            "elem." + std::to_string(i));
        
        // (AR) استدعاء drop للعنصر
        std::string elemDropName = getDropFunctionName(info.elementType->typeName);
        llvm::Function* elemDropFn = module_.getFunction(elemDropName);
        if (elemDropFn) {
            builder.CreateCall(elemDropFn, {elemPtr});
        }
    }
}

// =============================================================================
//                    دوال مساعدة / Helper Functions
// =============================================================================

bool LLVMDropGlue::isPrimitiveType(const std::string& typeName) const {
    return primitiveTypes_.count(typeName) > 0;
}

std::string LLVMDropGlue::getDropFunctionName(const std::string& typeName) const {
    // (AR) توليد اسم دالة drop فريد
    // (EN) Generate unique drop function name
    // __sad_drop_<TypeName>
    return "__sad_drop_" + typeName;
}

} // namespace LLVM
} // namespace Sad
