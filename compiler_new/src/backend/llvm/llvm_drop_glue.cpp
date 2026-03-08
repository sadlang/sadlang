// ״×״¹״·„ ״×״­״°״± Unicode „„״×״¹„‚״§״× ״§„״¹״±״¨״©
#ifdef _MSC_VER
#pragma warning(disable: 4819)
#endif

/**
 * @file llvm_drop_glue.cpp
 * @brief ״×†״° †״¸״§… ״×ˆ„״¯ ƒˆ״¯ ״§„״×†״¸ (Drop Glue) / Drop Glue Code Generator
 * @author ״±‚ „״÷״© ״µ
 * @date †״§״± 2026
 * @version 1.0
 * @phase …-‡€03: ״×ˆ„״¯ ƒˆ״¯ Drop/Destructor ״¨״¯ˆ† GC
 * 
 *     ‡״°״§ ״§„…„ ˆ„‘״¯ ƒˆ״¯ LLVM IR „״×†״¸ ״§„…ˆ״§״±״¯ ״×„‚״§״¦״§‹ ״¹†״¯ †‡״§״© ״§„†״·״§‚״§״×.
 *      ˆ״¶״¹ ״§„…„ƒ״© ״§„״µ״§״±…״© (״¨„״§ GC)״ ƒ„ …ˆ״±״¯ ״¬״¨ ״£† ״­״±‘״± ״×„‚״§״¦״§‹.
 *     
 *     ״§„†…״· ״§„…״×״¨״¹ …״´״§״¨‡ „€ Rust drop glue:
 *     - „ƒ„ †ˆ״¹ ״­״×״§״¬ ״×†״¸״ ״×ˆ„‘״¯ ״¯״§„״© __sad_drop_<Type>
 *     - ״¹†״¯ †‡״§״© ״§„†״·״§‚״ ״×״³״×״¯״¹‰ drop „„…״×״÷״±״§״× ״¨״×״±״×״¨ LIFO
 *     - ״§„‚… ״§„…†‚ˆ„״© „״§ ״×†״¸‘ (״×… †‚„ ״§„…„ƒ״©)
 *     - ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© „״§ ״×״­״×״§״¬ ״×†״¸
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

namespace Sad {
namespace LLVM {

// =============================================================================
//                    ״§„״¨†״§״¡ ˆ״§„‡״¯… / Construction & Destruction
// =============================================================================

LLVMDropGlue::LLVMDropGlue(llvm::LLVMContext& context, llvm::Module& module)
    : context_(context)
    , module_(module)
    , nextScopeId_(0) {
    
    // (AR) ״×״³״¬„ ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© ״§„״× „״§ ״×״­״×״§״¬ ״×†״¸
    // (EN) Register primitive types that don't need cleanup
    primitiveTypes_ = {
        // ״£†ˆ״§״¹ ״µ״­״­״© / Integer types
        u8"״¹8", u8"״¹16", u8"״¹32", u8"״¹64", u8"״¹128",
        "i8", "i16", "i32", "i64", "i128",
        u8"״·8", u8"״·16", u8"״·32", u8"״·64",
        "u8", "u16", "u32", "u64",
        
        // ״£†ˆ״§״¹ ״¹״´״±״© / Float types
        u8"״¹32", u8"״¹64",
        "f32", "f64", "float", "double",
        
        // ״£†ˆ״§״¹ ״£״®״±‰ / Other types
        u8"…†״·‚", "bool",
        u8"״­״±", "char",
        u8"״±״§״÷", "void",
        u8"…״₪״´״±_״®״§…", "raw_ptr",
        
        // ״£†ˆ״§״¹ ״­״¬… / Size types
        u8"״­״¬…", "usize", "isize",
    };
}

LLVMDropGlue::~LLVMDropGlue() = default;

// =============================================================================
//                    ״×״³״¬„ ״§„״£†ˆ״§״¹ / Type Registration
// =============================================================================

void LLVMDropGlue::registerDroppableType(const DropTypeInfo& info) {
    if (info.isPrimitive()) {
        return;  // (AR) ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© „״§ ״×״³״¬‘„
    }
    
    registeredTypes_[info.typeName] = info;
}

bool LLVMDropGlue::needsDrop(const std::string& typeName) const {
    // (AR) ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© „״§ ״×״­״×״§״¬ drop
    if (isPrimitiveType(typeName)) {
        return false;
    }
    
    // (AR) ״§„״×״­‚‚ …† ״§„״£†ˆ״§״¹ ״§„…״³״¬„״©
    auto it = registeredTypes_.find(typeName);
    if (it != registeredTypes_.end()) {
        return it->second.needsDrop();
    }
    
    // (AR) ״§״×״±״§״¶: ״§„״£†ˆ״§״¹ ״÷״± ״§„…״¹״±ˆ״© ‚״¯ ״×״­״×״§״¬ drop
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
//                    ״¥״¯״§״±״© ״§„†״·״§‚״§״× / Scope Management
// =============================================================================

size_t LLVMDropGlue::enterScope() {
    size_t id = nextScopeId_++;
    scopeStack_.push_back(DropScope{id, {}});
    return id;
}

void LLVMDropGlue::registerVariable(const ScopedVariable& var) {
    if (scopeStack_.empty()) {
        return;  // (AR) „״§ †״·״§‚ †״´״·
    }
    scopeStack_.back().variables.push_back(var);
}

void LLVMDropGlue::markMoved(const std::string& varName) {
    // (AR) ״§„״¨״­״« …† ״§„״£״¹„‰ (״§„†״·״§‚ ״§„״¯״§״®„ ״£ˆ„״§‹)
    for (auto it = scopeStack_.rbegin(); it != scopeStack_.rend(); ++it) {
        for (auto& var : it->variables) {
            if (var.name == varName) {
                var.isMoved = true;
                
                // (AR) ״¥״°״§ ƒ״§† ‡†״§ƒ ״¹„… drop״ ״­״¯‘״«‡  IR
                // (EN) If there's a drop flag, update it in IR
                // ״³״­״¯‘״«  emitScopeExit
                return;
            }
        }
    }
}

// =============================================================================
//                    ״×ˆ„״¯ ƒˆ״¯ ״§„״®״±ˆ״¬ …† ״§„†״·״§‚
//                    Scope Exit Code Generation
// =============================================================================

void LLVMDropGlue::emitScopeExit(llvm::IRBuilder<>& builder) {
    if (scopeStack_.empty()) {
        return;
    }
    
    DropScope& scope = scopeStack_.back();
    
    // (AR) ‡״¯… ״§„…״×״÷״±״§״× ״¨״×״±״×״¨ ״¹ƒ״³ (LIFO)
    // (EN) Drop variables in reverse order (LIFO)
    // ״¢״®״± …״×״÷״± ״£†״´״¦ ג€” ״£ˆ„ …״×״÷״± ‡״¯…
    for (auto it = scope.variables.rbegin(); it != scope.variables.rend(); ++it) {
        ScopedVariable& var = *it;
        
        // (AR) ״×״®״· ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״©
        if (var.dropInfo.isPrimitive()) {
            continue;
        }
        
        // (AR) ״×״®״· ״§„‚… ״§„…†‚ˆ„״© (״¨„״§ ״´״±״·)
        if (var.isMoved && var.dropFlag == nullptr) {
            continue;
        }
        
        // (AR) ״¥״°״§ ƒ״§† ‡†״§ƒ ״¹„… drop״ ˆ„‘״¯ ״­״µ״§‹ ״´״±״·״§‹
        // (EN) If drop flag exists, generate conditional check
        if (var.dropFlag != nullptr) {
            // ״×״­…„ ״¹„… ״§„״×†״¸
            llvm::Value* flagVal = builder.CreateLoad(
                llvm::Type::getInt1Ty(context_), 
                var.dropFlag,
                var.name + ".dropflag");
            
            // ״¥†״´״§״¡ ״±ˆ״¹: drop vs skip
            llvm::Function* currentFn = builder.GetInsertBlock()->getParent();
            llvm::BasicBlock* dropBB = llvm::BasicBlock::Create(
                context_, "drop." + var.name, currentFn);
            llvm::BasicBlock* skipBB = llvm::BasicBlock::Create(
                context_, "skip." + var.name, currentFn);
            
            builder.CreateCondBr(flagVal, dropBB, skipBB);
            
            // ƒ״×„״© ״§„״×†״¸
            builder.SetInsertPoint(dropBB);
            emitDropCall(builder, var);
            builder.CreateBr(skipBB);
            
            // …״×״§״¨״¹״© ״¨״¹״¯ ״§„״×†״¸
            builder.SetInsertPoint(skipBB);
        } else {
            // (AR) drop ״÷״± …״´״±ˆ״· (״§„…״×״÷״± „… †‚„)
            emitDropCall(builder, var);
        }
    }
    
    // (AR) ״¥״²״§„״© ״§„†״·״§‚ …† ״§„…ƒ״¯״³
    scopeStack_.pop_back();
}

// =============================================================================
//                    ״×ˆ„״¯ ״§״³״×״¯״¹״§״¡״§״× Drop
//                    Drop Call Generation
// =============================================================================

void LLVMDropGlue::emitDropCall(llvm::IRBuilder<>& builder,
                                 const ScopedVariable& var) {
    if (var.dropInfo.isPrimitive() || var.alloca == nullptr) {
        return;
    }
    
    switch (var.dropInfo.dropKind) {
        case DropKind::None:
            // (AR) „״§ ״´״¡ „„״¹„
            break;
            
        case DropKind::CustomDestructor: {
            // (AR) ״§״³״×״¯״¹״§״¡ ״¯״§„״© ‡״¯…() ״§„…״®״µ״µ״©
            // (EN) Call custom destructor function
            std::string dtorName = var.dropInfo.destructorName;
            if (dtorName.empty()) {
                dtorName = getDropFunctionName(var.dropInfo.typeName);
            }
            
            llvm::Function* dropFn = module_.getFunction(dtorName);
            if (!dropFn) {
                // (AR) ״¥†״´״§״¡ ״¥״¹„״§† ״§„״¯״§„״© ״¥״°״§ „… ״×ƒ† …ˆ״¬ˆ״¯״©
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
            // (AR) ״×״­״±״± ״§„״°״§ƒ״±״© ״§„…״®״µ״µ״©
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
            
            // (AR) ״×״­…„ ״§„…״₪״´״± ˆ״×״­״±״±‡
            llvm::Value* ptr = builder.CreateLoad(
                llvm::PointerType::getUnqual(context_), var.alloca);
            builder.CreateCall(deallocFn, {ptr});
            break;
        }
        
        case DropKind::StructFields: {
            // (AR) ‡״¯… ״­‚ˆ„ ״§„״¨†״© ״¨״×״±״×״¨ ״¹ƒ״³
            // (EN) Drop struct fields in reverse order
            emitStructDrop(builder, var.alloca, var.dropInfo);
            break;
        }
        
        case DropKind::ArrayElements: {
            // (AR) ‡״¯… ״¹†״§״µ״± ״§„…״µˆ״©
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
    
    // (AR) ״¥†״´״§״¡ ״¹„… drop: i1 …‡״£ ״¨€ true (״­״×״§״¬ drop)
    // (EN) Create drop flag: i1 initialized to true (needs drop)
    llvm::Function* currentFn = builder.GetInsertBlock()->getParent();
    llvm::IRBuilder<> entryBuilder(
        &currentFn->getEntryBlock(),
        currentFn->getEntryBlock().begin());
    
    llvm::AllocaInst* flag = entryBuilder.CreateAlloca(
        llvm::Type::getInt1Ty(context_), nullptr,
        varName + ".dropflag.addr");
    
    // (AR) ״×‡״¦״© ״¨€ true (״­״×״§״¬ ״×†״¸ ״­״×‰ †‚„)
    builder.CreateStore(
        llvm::ConstantInt::getTrue(context_), flag);
    
    // (AR) ״×״³״¬„ ״§„״¹„…  ״§„…״×״÷״± ״§„…†״§״³״¨
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
//                    ״×ˆ„״¯ ״¯ˆ״§„ Drop
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
    
    // (AR) ״§„״×״­‚‚ ״¥״°״§ ƒ״§†״× ״§„״¯״§„״© …ˆ״¬ˆ״¯״© ״¨״§„״¹„
    llvm::Function* existing = module_.getFunction(fnName);
    if (existing && !existing->empty()) {
        return existing;  // (AR) ״×… ״§„״×ˆ„״¯ …״³״¨‚״§‹
    }
    
    // (AR) ״¥†״´״§״¡ †ˆ״¹ ״§„״¯״§„״©: void(ptr)
    llvm::FunctionType* fnType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(context_),
        {llvm::PointerType::getUnqual(context_)},
        false);
    
    llvm::Function* dropFn = llvm::Function::Create(
        fnType, llvm::Function::InternalLinkage, fnName, module_);
    
    // (AR) ״×״³…״© ״§„…״¹״§…„
    llvm::Argument* selfArg = dropFn->arg_begin();
    selfArg->setName("self");
    
    // (AR) ״¥†״´״§״¡ ƒ״×„״© ״§„״¯״®ˆ„
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(
        context_, "entry", dropFn);
    
    llvm::IRBuilder<> fnBuilder(entry);
    
    switch (info.dropKind) {
        case DropKind::CustomDestructor: {
            // (AR) ״§״³״×״¯״¹״§״¡ ״§„…״¯…‘״± ״§„…״®״µ״µ
            if (!info.destructorName.empty()) {
                llvm::Function* dtor = module_.getFunction(info.destructorName);
                if (!dtor) {
                    // (AR) ״¥״¹„״§† ״®״§״±״¬
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
            // (AR) ‡״¯… ״­‚ˆ„ ״§„״¨†״© ״¨״×״±״×״¨ ״¹ƒ״³
            emitStructDrop(fnBuilder, selfArg, info);
            break;
        }
        
        case DropKind::ArrayElements: {
            // (AR) ‡״¯… ״¹†״§״µ״± ״§„…״µˆ״©
            emitArrayDrop(fnBuilder, selfArg, info);
            break;
        }
        
        case DropKind::Deallocate: {
            // (AR) ״×״­״±״± ״§„״°״§ƒ״±״©
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
//                    ״×ˆ„״¯ ƒˆ״¯ Drop „„״£†ˆ״§״¹ ״§„…״±ƒ״¨״©
//                    Composite Type Drop Code Generation
// =============================================================================

void LLVMDropGlue::emitStructDrop(llvm::IRBuilder<>& builder,
                                   llvm::Value* structPtr,
                                   const DropTypeInfo& info) {
    // (AR) ‡״¯… ״§„״­‚ˆ„ ״¨״×״±״×״¨ ״¹ƒ״³
    // (EN) Drop fields in reverse order
    // ״¢״®״± ״­‚„ ״£״¹„† ג€” ״£ˆ„ ״­‚„ ‡״¯…
    
    const auto& fields = info.fieldTypes;
    
    for (int i = static_cast<int>(fields.size()) - 1; i >= 0; --i) {
        const DropTypeInfo& field = fields[i];
        
        if (field.isPrimitive()) {
            continue;  // (AR) ״§„״£†ˆ״§״¹ ״§„״¨״¯״§״¦״© „״§ ״×״­״×״§״¬ drop
        }
        
        // (AR) ״§„״­״µˆ„ ״¹„‰ ״§„†ˆ״¹ ״§„‡ƒ„ …† LLVM
        llvm::Type* structType = nullptr;
        if (auto* ptrType = llvm::dyn_cast<llvm::PointerType>(structPtr->getType())) {
            // (AR) „„…״₪״´״±״§״× ״÷״± ״§„״´״§״© (opaque pointers)
            // †״­״×״§״¬ ״§„†ˆ״¹ ״§„‡ƒ„ …† ״§„…״¹„ˆ…״§״× ״§„…״³״¬„״©
            structType = llvm::StructType::getTypeByName(context_, info.typeName);
        }
        
        // (AR) ״­״³״§״¨ …״₪״´״± ״§„״­‚„
        llvm::Value* fieldPtr = nullptr;
        if (structType && structType->isStructTy()) {
            fieldPtr = builder.CreateStructGEP(structType, structPtr, i,
                                               "field." + std::to_string(i));
        } else {
            // (AR) fallback: ״§״³״×״®״¯״§… GEP ״¹״§…
            fieldPtr = builder.CreateConstGEP2_32(
                llvm::Type::getInt8Ty(context_), structPtr, 0, i,
                "field." + std::to_string(i));
        }
        
        // (AR) ״§״³״×״¯״¹״§״¡ drop „„״­‚„
        std::string fieldDropName = getDropFunctionName(field.typeName);
        llvm::Function* fieldDropFn = module_.getFunction(fieldDropName);
        if (fieldDropFn) {
            builder.CreateCall(fieldDropFn, {fieldPtr});
        } else if (field.dropKind == DropKind::Deallocate) {
            // (AR) ״×״­״±״± …״¨״§״´״±
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
        return;  // (AR) ״¹†״§״µ״± ״¨״¯״§״¦״© ג€” „״§ ״×״­״×״§״¬ drop
    }
    
    if (info.arraySize == 0) {
        return;  // (AR) …״µˆ״© ״§״±״÷״©
    }
    
    // (AR) ‡״¯… ״§„״¹†״§״µ״± …† ״§„״£״®״± ״¥„‰ ״§„״£ˆ„
    // (EN) Drop elements from last to first
    llvm::Type* elemType = llvm::Type::getInt8Ty(context_);  // placeholder
    
    for (int i = static_cast<int>(info.arraySize) - 1; i >= 0; --i) {
        // (AR) ״­״³״§״¨ …״₪״´״± ״§„״¹†״µ״±
        llvm::Value* elemPtr = builder.CreateConstGEP2_32(
            llvm::ArrayType::get(elemType, info.arraySize),
            arrayPtr, 0, i,
            "elem." + std::to_string(i));
        
        // (AR) ״§״³״×״¯״¹״§״¡ drop „„״¹†״µ״±
        std::string elemDropName = getDropFunctionName(info.elementType->typeName);
        llvm::Function* elemDropFn = module_.getFunction(elemDropName);
        if (elemDropFn) {
            builder.CreateCall(elemDropFn, {elemPtr});
        }
    }
}

// =============================================================================
//                    ״¯ˆ״§„ …״³״§״¹״¯״© / Helper Functions
// =============================================================================

bool LLVMDropGlue::isPrimitiveType(const std::string& typeName) const {
    return primitiveTypes_.count(typeName) > 0;
}

std::string LLVMDropGlue::getDropFunctionName(const std::string& typeName) const {
    // (AR) ״×ˆ„״¯ ״§״³… ״¯״§„״© drop ״±״¯
    // (EN) Generate unique drop function name
    // __sad_drop_<TypeName>
    return "__sad_drop_" + typeName;
}

} // namespace LLVM
} // namespace Sad
