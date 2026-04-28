/**
 * ============================================================================
 * llvm_codegen_directives.cpp - توليد كود التوجيهات / Directives Codegen
 * ============================================================================
 * 
 * (AR) هذا الملف يحتوي على توليد LLVM IR للتوجيهات الست:
 *   @حجم — حجم النوع بالبايتات
 *   @ذري — العمليات الذرية
 *   @غير_آمن — كتلة غير آمنة (metadata فقط)
 *   @وقت_الترجمة — تنفيذ وقت الترجمة (metadata فقط)
 *   @متطاير — متغير متطاير
 *   @تجميع — تجميع مضمن (inline asm)
 * 
 * (EN) This file contains LLVM IR generation for the 6 @ directives:
 *   @حجم — sizeof type in bytes
 *   @ذري — atomic operations
 *   @غير_آمن — unsafe block (metadata only)
 *   @وقت_الترجمة — compile-time execution (metadata only)
 *   @متطاير — volatile variable
 *   @تجميع — inline assembly
 * 
 * المؤلف / Author: Sad Compiler Team
 * التاريخ / Date: 2025
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/directives/directives_codegen.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR;

namespace Sad {
namespace LLVM {

/**
 * @brief (AR) توليد @حجم(نوع) — يُرجع حجم النوع بالبايتات
 * @brief (EN) Emit @حجم(type) — returns the size of a type in bytes
 * 
 * SIR Instruction:
 *   opcode: Sizeof
 *   operands[0]: ConstantI64 (size in bytes)
 *   operands[1]: ConstantString (type name for debugging)
 *   result: Register (i64)
 * 
 * @param inst SIR instruction
 * @return LLVM value (i64 constant)
 */
llvm::Value* DirectivesCodeGen::emitSizeof(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        cg_.reportError("emitSizeof: invalid instruction");
        return nullptr;
    }
    
    // (AR) الحصول على الحجم من المعامل الأول
    // (EN) Get size from first operand
    int64_t sizeValue = 8; // Default
    if (inst->operands[0].type == SIROperandType::CONSTANT && 
        inst->operands[0].dataType == SadTypeKind::Integer) {
        sizeValue = inst->operands[0].intValue;
    }
    
    // (AR) إنشاء ثابت i64
    // (EN) Create i64 constant
    llvm::Value* result = llvm::ConstantInt::get(
        llvm::Type::getInt64Ty(*cg_.context_), 
        sizeValue
    );
    
    // (AR) تسجيل النتيجة إذا كان هناك result register
    // (EN) Register result if there's a result register
    if (inst->result.has_value()) {
        cg_.context_info_.namedValues[inst->result->name] = result;
    }
    
    #ifndef NDEBUG
    std::cout << "[LLVM] emitSizeof: " << sizeValue << " bytes" << std::endl;
    #endif
    
    return result;
}

/**
 * @brief (AR) توليد @ذري(تحميل, ptr) — قراءة ذرية
 * @brief (EN) Emit @ذري(load, ptr) — atomic load
 * 
 * SIR Instruction:
 *   opcode: AtomicLoad
 *   operands[0]: Register (pointer)
 *   result: Register (loaded value)
 * 
 * @param inst SIR instruction
 * @return LLVM value (loaded value)
 */
llvm::Value* DirectivesCodeGen::emitAtomicLoad(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        cg_.reportError("emitAtomicLoad: invalid instruction");
        return nullptr;
    }
    
    // (AR) الحصول على المؤشر
    // (EN) Get pointer operand
    llvm::Value* ptr = cg_.resolveOperand(inst->operands[0]);
    if (!ptr) {
        cg_.reportError("emitAtomicLoad: could not resolve pointer operand");
        return nullptr;
    }
    
    // (AR) التأكد من أن المؤشر من نوع pointer
    // (EN) Ensure pointer is a pointer type
    if (!ptr->getType()->isPointerTy()) {
        // (AR) إذا لم يكن pointer، نحاول تحميله كقيمة مباشرة
        // (EN) If not pointer, try to treat as direct value
        if (inst->result.has_value()) {
            cg_.context_info_.namedValues[inst->result->name] = ptr;
        }
        return ptr;
    }
    
    // (AR) توليد تحميل ذري مع ترتيب seq_cst
    // (EN) Generate atomic load with seq_cst ordering
    llvm::LoadInst* loadInst = cg_.builder_->CreateLoad(
        llvm::Type::getInt64Ty(*cg_.context_),
        ptr,
        "atomic_load"
    );
    loadInst->setAtomic(llvm::AtomicOrdering::SequentiallyConsistent);
    loadInst->setAlignment(llvm::Align(8));
    
    // (AR) تسجيل النتيجة
    // (EN) Register result
    if (inst->result.has_value()) {
        cg_.context_info_.namedValues[inst->result->name] = loadInst;
    }
    
    #ifndef NDEBUG
    std::cout << "[LLVM] emitAtomicLoad generated" << std::endl;
    #endif
    
    return loadInst;
}

/**
 * @brief (AR) توليد @ذري(تخزين, ptr, value) — كتابة ذرية
 * @brief (EN) Emit @ذري(store, ptr, value) — atomic store
 * 
 * SIR Instruction:
 *   opcode: AtomicStore
 *   operands[0]: Register (pointer)
 *   operands[1]: Register (value to store)
 * 
 * @param inst SIR instruction
 * @return LLVM value (nullptr, store has no result)
 */
llvm::Value* DirectivesCodeGen::emitAtomicStore(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        cg_.reportError("emitAtomicStore: invalid instruction (need 2 operands)");
        return nullptr;
    }
    
    // (AR) الحصول على المؤشر والقيمة
    // (EN) Get pointer and value operands
    llvm::Value* ptr = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* value = cg_.resolveOperand(inst->operands[1]);
    
    if (!ptr || !value) {
        cg_.reportError("emitAtomicStore: could not resolve operands");
        return nullptr;
    }
    
    // (AR) التأكد من أن المؤشر من نوع pointer
    // (EN) Ensure pointer is a pointer type
    if (!ptr->getType()->isPointerTy()) {
        cg_.reportError("emitAtomicStore: first operand must be a pointer");
        return nullptr;
    }
    
    // (AR) تحويل القيمة إلى i64 إذا لزم الأمر
    // (EN) Convert value to i64 if necessary
    if (!value->getType()->isIntegerTy(64)) {
        if (value->getType()->isIntegerTy()) {
            value = cg_.builder_->CreateZExt(value, llvm::Type::getInt64Ty(*cg_.context_), "zext_val");
        } else if (value->getType()->isDoubleTy()) {
            value = cg_.builder_->CreateBitCast(value, llvm::Type::getInt64Ty(*cg_.context_), "bitcast_val");
        }
    }
    
    // (AR) توليد تخزين ذري مع ترتيب seq_cst
    // (EN) Generate atomic store with seq_cst ordering
    llvm::StoreInst* storeInst = cg_.builder_->CreateStore(value, ptr);
    storeInst->setAtomic(llvm::AtomicOrdering::SequentiallyConsistent);
    storeInst->setAlignment(llvm::Align(8));
    
    #ifndef NDEBUG
    std::cout << "[LLVM] emitAtomicStore generated" << std::endl;
    #endif
    
    return storeInst;
}

/**
 * @brief (AR) توليد @ذري(إضافة, ptr, value) — إضافة ذرية
 * @brief (EN) Emit @ذري(add, ptr, value) — atomic add
 * 
 * SIR Instruction:
 *   opcode: AtomicAdd
 *   operands[0]: Register (pointer)
 *   operands[1]: Register (value to add)
 *   result: Register (old value before add)
 * 
 * @param inst SIR instruction
 * @return LLVM value (old value)
 */
llvm::Value* DirectivesCodeGen::emitAtomicAdd(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        cg_.reportError("emitAtomicAdd: invalid instruction (need 2 operands)");
        return nullptr;
    }
    
    // (AR) الحصول على المؤشر والقيمة
    // (EN) Get pointer and value operands
    llvm::Value* ptr = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* value = cg_.resolveOperand(inst->operands[1]);
    
    if (!ptr || !value) {
        cg_.reportError("emitAtomicAdd: could not resolve operands");
        return nullptr;
    }
    
    // (AR) التأكد من أن المؤشر من نوع pointer
    // (EN) Ensure pointer is a pointer type
    if (!ptr->getType()->isPointerTy()) {
        cg_.reportError("emitAtomicAdd: first operand must be a pointer");
        return nullptr;
    }
    
    // (AR) تحويل القيمة إلى i64 إذا لزم الأمر
    // (EN) Convert value to i64 if necessary
    if (!value->getType()->isIntegerTy(64)) {
        if (value->getType()->isIntegerTy()) {
            value = cg_.builder_->CreateZExt(value, llvm::Type::getInt64Ty(*cg_.context_), "zext_val");
        }
    }
    
    // (AR) توليد atomicrmw add
    // (EN) Generate atomicrmw add
    llvm::Value* result = cg_.builder_->CreateAtomicRMW(
        llvm::AtomicRMWInst::Add,
        ptr,
        value,
        llvm::MaybeAlign(8),
        llvm::AtomicOrdering::SequentiallyConsistent
    );
    
    // (AR) تسجيل النتيجة (القيمة القديمة قبل الإضافة)
    // (EN) Register result (old value before add)
    if (inst->result.has_value()) {
        cg_.context_info_.namedValues[inst->result->name] = result;
    }
    
    #ifndef NDEBUG
    std::cout << "[LLVM] emitAtomicAdd generated" << std::endl;
    #endif
    
    return result;
}

/**
 * @brief (AR) توليد @ذري(طرح, ptr, value) — طرح ذري
 * @brief (EN) Emit @ذري(sub, ptr, value) — atomic sub
 * 
 * SIR Instruction:
 *   opcode: AtomicSub
 *   operands[0]: Register (pointer)
 *   operands[1]: Register (value to subtract)
 *   result: Register (old value before sub)
 * 
 * @param inst SIR instruction
 * @return LLVM value (old value)
 */
llvm::Value* DirectivesCodeGen::emitAtomicSub(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        cg_.reportError("emitAtomicSub: invalid instruction (need 2 operands)");
        return nullptr;
    }
    
    llvm::Value* ptr = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* value = cg_.resolveOperand(inst->operands[1]);
    
    if (!ptr || !value) {
        cg_.reportError("emitAtomicSub: could not resolve operands");
        return nullptr;
    }
    
    if (!ptr->getType()->isPointerTy()) {
        cg_.reportError("emitAtomicSub: first operand must be a pointer");
        return nullptr;
    }
    
    if (!value->getType()->isIntegerTy(64)) {
        if (value->getType()->isIntegerTy()) {
            value = cg_.builder_->CreateZExt(value, llvm::Type::getInt64Ty(*cg_.context_), "zext_val");
        }
    }
    
    llvm::Value* result = cg_.builder_->CreateAtomicRMW(
        llvm::AtomicRMWInst::Sub,
        ptr,
        value,
        llvm::MaybeAlign(8),
        llvm::AtomicOrdering::SequentiallyConsistent
    );
    
    if (inst->result.has_value()) {
        cg_.context_info_.namedValues[inst->result->name] = result;
    }
    
    #ifndef NDEBUG
    std::cout << "[LLVM] emitAtomicSub generated" << std::endl;
    #endif
    
    return result;
}

/**
 * @brief (AR) توليد @ذري(تبادل, ptr, value) — تبادل ذري
 * @brief (EN) Emit @ذري(exchange, ptr, value) — atomic exchange
 * 
 * SIR Instruction:
 *   opcode: AtomicExchange
 *   operands[0]: Register (pointer)
 *   operands[1]: Register (new value)
 *   result: Register (old value)
 * 
 * @param inst SIR instruction
 * @return LLVM value (old value)
 */
llvm::Value* DirectivesCodeGen::emitAtomicExchange(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        cg_.reportError("emitAtomicExchange: invalid instruction (need 2 operands)");
        return nullptr;
    }
    
    llvm::Value* ptr = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* value = cg_.resolveOperand(inst->operands[1]);
    
    if (!ptr || !value) {
        cg_.reportError("emitAtomicExchange: could not resolve operands");
        return nullptr;
    }
    
    if (!ptr->getType()->isPointerTy()) {
        cg_.reportError("emitAtomicExchange: first operand must be a pointer");
        return nullptr;
    }
    
    if (!value->getType()->isIntegerTy(64)) {
        if (value->getType()->isIntegerTy()) {
            value = cg_.builder_->CreateZExt(value, llvm::Type::getInt64Ty(*cg_.context_), "zext_val");
        }
    }
    
    llvm::Value* result = cg_.builder_->CreateAtomicRMW(
        llvm::AtomicRMWInst::Xchg,
        ptr,
        value,
        llvm::MaybeAlign(8),
        llvm::AtomicOrdering::SequentiallyConsistent
    );
    
    if (inst->result.has_value()) {
        cg_.context_info_.namedValues[inst->result->name] = result;
    }
    
    #ifndef NDEBUG
    std::cout << "[LLVM] emitAtomicExchange generated" << std::endl;
    #endif
    
    return result;
}

/**
 * @brief (AR) توليد @ذري(مقارنة_وتبديل, ptr, expected, desired) — مقارنة وتبديل ذري
 * @brief (EN) Emit @ذري(cas, ptr, expected, desired) — atomic compare-and-swap
 * 
 * SIR Instruction:
 *   opcode: AtomicCmpXchg
 *   operands[0]: Register (pointer)
 *   operands[1]: Register (expected value)
 *   operands[2]: Register (desired value)
 *   result: Register (old value / success status)
 * 
 * @param inst SIR instruction
 * @return LLVM value (old value)
 */
llvm::Value* DirectivesCodeGen::emitAtomicCmpXchg(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 3) {
        cg_.reportError("emitAtomicCmpXchg: invalid instruction (need 3 operands)");
        return nullptr;
    }
    
    llvm::Value* ptr = cg_.resolveOperand(inst->operands[0]);
    llvm::Value* expected = cg_.resolveOperand(inst->operands[1]);
    llvm::Value* desired = cg_.resolveOperand(inst->operands[2]);
    
    if (!ptr || !expected || !desired) {
        cg_.reportError("emitAtomicCmpXchg: could not resolve operands");
        return nullptr;
    }
    
    if (!ptr->getType()->isPointerTy()) {
        cg_.reportError("emitAtomicCmpXchg: first operand must be a pointer");
        return nullptr;
    }
    
    // (AR) تحويل القيم إلى i64 إذا لزم الأمر
    // (EN) Convert values to i64 if necessary
    llvm::Type* i64Type = llvm::Type::getInt64Ty(*cg_.context_);
    if (!expected->getType()->isIntegerTy(64)) {
        if (expected->getType()->isIntegerTy()) {
            expected = cg_.builder_->CreateZExt(expected, i64Type, "zext_expected");
        }
    }
    if (!desired->getType()->isIntegerTy(64)) {
        if (desired->getType()->isIntegerTy()) {
            desired = cg_.builder_->CreateZExt(desired, i64Type, "zext_desired");
        }
    }
    
    // (AR) توليد cmpxchg
    // (EN) Generate cmpxchg
    llvm::Value* casResult = cg_.builder_->CreateAtomicCmpXchg(
        ptr,
        expected,
        desired,
        llvm::MaybeAlign(8),
        llvm::AtomicOrdering::SequentiallyConsistent,
        llvm::AtomicOrdering::SequentiallyConsistent
    );
    
    // (AR) استخراج القيمة القديمة من نتيجة cmpxchg (عنصر 0)
    // (EN) Extract old value from cmpxchg result (element 0)
    llvm::Value* oldValue = cg_.builder_->CreateExtractValue(casResult, {0}, "cas_old");
    
    if (inst->result.has_value()) {
        cg_.context_info_.namedValues[inst->result->name] = oldValue;
    }
    
    #ifndef NDEBUG
    std::cout << "[LLVM] emitAtomicCmpXchg generated" << std::endl;
    #endif
    
    return oldValue;
}

} // namespace LLVM
} // namespace Sad
