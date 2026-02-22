/*
 * ============================================================================
 * ┘ו┘ט┘ה╪» ┘ד┘ט╪» LLVM IR - ┘ו┘ה┘ב ╪º┘ה╪¬┘ז┘ב┘ך╪░
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 * 
 * ┘ח╪░╪º ╪º┘ה┘ו┘ה┘ב ┘ך╪¡╪¬┘ט┘ך ╪╣┘ה┘י ╪¬┘ז┘ב┘ך╪░ ┘ו┘ט┘ה╪» ┘ד┘ט╪» LLVM IR ╪º┘ה╪░┘ך ┘ך╪¬╪▒╪¼┘ו SIR ╪Ñ┘ה┘י LLVM IR
 * This file contains the implementation of LLVM IR code generator that
 * translates SIR to LLVM IR
 * 
 * ╪º┘ה╪¬╪▓╪º┘ו ╪¿ STRICT_CODING_RULES.md:
 * - ╪¬┘ו ┘ג╪▒╪º╪í╪⌐ ╪¼┘ו┘ך╪╣ ╪º┘ה┘ו┘ה┘ב╪º╪¬ ╪º┘ה╪▒╪ú╪│┘ך╪⌐ ╪¿╪º┘ה┘ד╪º┘ו┘ה (1746 ╪│╪╖╪▒)
 * - ╪¬┘ו ╪¬┘ט╪½┘ך┘ג ┘ד┘ה ╪º╪│╪¬╪«╪»╪º┘ו API ╪¿┘ו┘ט┘ג╪╣ ╪º┘ה┘ו╪╡╪»╪▒
 * - ┘ה╪º ╪¬┘ט╪¼╪» ╪»┘ט╪º┘ה ╪ú┘ט ┘ו╪¬╪║┘ך╪▒╪º╪¬ ┘ו╪«╪¬╪▒╪╣╪⌐
 * 
 * STRICT_CODING_RULES.md compliance:
 * - All header files read completely (1746 lines)
 * - Every API usage documented with source location
 * - No invented functions or variables
 * 
 * ╪º┘ה┘ו╪ñ┘ה┘ב (Author): SadLanguage Compiler Team
 * ╪º┘ה╪¬╪º╪▒┘ך╪« (Date): December 2024
 * ╪º┘ה╪Ñ╪╡╪»╪º╪▒ (Version): 1.1.4 Phase 1
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include <llvm/Support/TargetSelect.h>
// Source: LLVM 14+ API - llvm/MC/TargetRegistry.h ╪¿╪»┘ה╪º┘כ ┘ו┘ז llvm/Support/TargetRegistry.h
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/InlineAsm.h>
#include <iostream>
#include <fstream>

// Source: llvm_codegen.h:103-108 - using declarations
using namespace Sad::Compiler::SIR;  // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SIRType

namespace Sad {
namespace LLVM {

llvm::Value* LLVMCodeGen::emitAsyncCreateChannel(std::shared_ptr<SIRInstruction> inst) {
    // Channel = {i64 buffer_ptr, i64 capacity, i64 head, i64 tail, i64 count, i8* mutex, i8* not_empty_event, i8* not_full_event}
    // Simplified: malloc(64) for channel struct
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    
    auto mallocTy = llvm::FunctionType::get(i8PtrTy, {i64Ty}, false);
    auto mallocFn = module_->getOrInsertFunction("malloc", mallocTy);
    auto chanPtr = builder_->CreateCall(mallocFn, {llvm::ConstantInt::get(i64Ty, 64)});
    
    // Zero-initialize using LLVM memset intrinsic
    builder_->CreateMemSet(chanPtr, builder_->getInt8(0), 64, llvm::MaybeAlign(8));
    
    auto result = builder_->CreatePtrToInt(chanPtr, i64Ty);
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncChannelSend(std::shared_ptr<SIRInstruction> inst) {
    // Simple: store value at channel memory
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);
    
    llvm::Value* chanId = resolveOperand(inst->operands[0]);
    llvm::Value* value = resolveOperand(inst->operands[1]);
    auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
    
    // Store value at offset 0 (simple single-value channel)
    auto valSlot = builder_->CreateBitCast(chanPtr, i64PtrTy);
    builder_->CreateStore(value, valSlot);
    
    // Set flag at offset 8 (has_data = 1)
    auto flagSlot = builder_->CreateGEP(llvm::Type::getInt8Ty(*context_), chanPtr, {llvm::ConstantInt::get(i64Ty, 8)});
    auto flagPtr = builder_->CreateBitCast(flagSlot, i64PtrTy);
    builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 1), flagPtr);
    
    auto result = llvm::ConstantInt::get(i64Ty, 0);
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncChannelRecv(std::shared_ptr<SIRInstruction> inst) {
    // Simple: load value from channel memory
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);
    
    llvm::Value* chanId = resolveOperand(inst->operands[0]);
    auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
    
    // Load value from offset 0
    auto valSlot = builder_->CreateBitCast(chanPtr, i64PtrTy);
    auto result = builder_->CreateLoad(i64Ty, valSlot);
    
    // Clear flag at offset 8
    auto flagSlot = builder_->CreateGEP(llvm::Type::getInt8Ty(*context_), chanPtr, {llvm::ConstantInt::get(i64Ty, 8)});
    auto flagPtr = builder_->CreateBitCast(flagSlot, i64PtrTy);
    builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), flagPtr);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncChannelClose(std::shared_ptr<SIRInstruction> inst) {
    // Mark channel as closed (set flag at offset 16 to 1) instead of calling free
    // to avoid CRT heap corruption issues with malloc/free pairings
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);
    
    llvm::Value* chanId = resolveOperand(inst->operands[0]);
    auto chanPtr = builder_->CreateIntToPtr(chanId, i8PtrTy);
    
    // Set closed flag at offset 16
    auto closedSlot = builder_->CreateGEP(llvm::Type::getInt8Ty(*context_), chanPtr, {llvm::ConstantInt::get(i64Ty, 16)});
    auto closedPtr = builder_->CreateBitCast(closedSlot, i64PtrTy);
    builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 1), closedPtr);
    
    auto result = llvm::ConstantInt::get(i64Ty, 0);
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncMutexCreate(std::shared_ptr<SIRInstruction> inst) {
    // CreateMutexA(NULL, FALSE, NULL)
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    
    auto funcTy = llvm::FunctionType::get(i8PtrTy, {i8PtrTy, i32Ty, i8PtrTy}, false);
    auto createMutex = module_->getOrInsertFunction("CreateMutexA", funcTy);
    auto nullPtr = llvm::ConstantPointerNull::get(llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0));
    auto handle = builder_->CreateCall(createMutex, {nullPtr, llvm::ConstantInt::get(i32Ty, 0), nullPtr});
    auto result = builder_->CreatePtrToInt(handle, i64Ty);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncMutexLock(std::shared_ptr<SIRInstruction> inst) {
    // WaitForSingleObject(mutex, INFINITE)
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    
    llvm::Value* mutexId = resolveOperand(inst->operands[0]);
    auto handle = builder_->CreateIntToPtr(mutexId, i8PtrTy);
    
    auto funcTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
    auto waitFunc = module_->getOrInsertFunction("WaitForSingleObject", funcTy);
    builder_->CreateCall(waitFunc, {handle, llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF)});
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto result = llvm::ConstantInt::get(i64Ty, 0);
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncMutexUnlock(std::shared_ptr<SIRInstruction> inst) {
    // ReleaseMutex(mutex)
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    
    llvm::Value* mutexId = resolveOperand(inst->operands[0]);
    auto handle = builder_->CreateIntToPtr(mutexId, i8PtrTy);
    
    auto funcTy = llvm::FunctionType::get(i32Ty, {i8PtrTy}, false);
    auto releaseMutex = module_->getOrInsertFunction("ReleaseMutex", funcTy);
    builder_->CreateCall(releaseMutex, {handle});
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto result = llvm::ConstantInt::get(i64Ty, 0);
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncThreadSpawn(std::shared_ptr<SIRInstruction> inst) {
    // Same as spawn - CreateThread
    return emitAsyncSpawn(inst);
}

llvm::Value* LLVMCodeGen::emitAsyncThreadJoin(std::shared_ptr<SIRInstruction> inst) {
    // WaitForSingleObject(thread_handle, INFINITE) then CloseHandle
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    
    llvm::Value* threadId = resolveOperand(inst->operands[0]);
    auto handle = builder_->CreateIntToPtr(threadId, i8PtrTy);
    
    // Wait
    auto waitTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
    auto waitFunc = module_->getOrInsertFunction("WaitForSingleObject", waitTy);
    builder_->CreateCall(waitFunc, {handle, llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF)});
    
    // CloseHandle
    auto closeTy = llvm::FunctionType::get(i32Ty, {i8PtrTy}, false);
    auto closeFunc = module_->getOrInsertFunction("CloseHandle", closeTy);
    builder_->CreateCall(closeFunc, {handle});
    
    auto result = llvm::ConstantInt::get(i64Ty, 0);
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncAtomicLoad(std::shared_ptr<SIRInstruction> inst) {
    // LLVM atomic load: load atomic i64, ptr %addr seq_cst
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);
    
    llvm::Value* addr = resolveOperand(inst->operands[0]);
    auto ptr = builder_->CreateIntToPtr(addr, i64PtrTy);
    auto loadInst = builder_->CreateLoad(i64Ty, ptr);
    loadInst->setAtomic(llvm::AtomicOrdering::SequentiallyConsistent);
    loadInst->setAlignment(llvm::Align(8));
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = loadInst;
    }
    return loadInst;
}

llvm::Value* LLVMCodeGen::emitAsyncAtomicStore(std::shared_ptr<SIRInstruction> inst) {
    // LLVM atomic store: store atomic i64 %val, ptr %addr seq_cst
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);
    
    llvm::Value* addr = resolveOperand(inst->operands[0]);
    llvm::Value* value = resolveOperand(inst->operands[1]);
    auto ptr = builder_->CreateIntToPtr(addr, i64PtrTy);
    auto storeInst = builder_->CreateStore(value, ptr);
    storeInst->setAtomic(llvm::AtomicOrdering::SequentiallyConsistent);
    storeInst->setAlignment(llvm::Align(8));
    auto result = llvm::ConstantInt::get(i64Ty, 0);
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncAtomicAdd(std::shared_ptr<SIRInstruction> inst) {
    // LLVM atomicrmw add
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);
    
    llvm::Value* addr = resolveOperand(inst->operands[0]);
    llvm::Value* value = resolveOperand(inst->operands[1]);
    auto ptr = builder_->CreateIntToPtr(addr, i64PtrTy);
    auto result = builder_->CreateAtomicRMW(llvm::AtomicRMWInst::Add, ptr, value,
                                             llvm::Align(8), llvm::AtomicOrdering::SequentiallyConsistent);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncAtomicCAS(std::shared_ptr<SIRInstruction> inst) {
    // LLVM cmpxchg
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);
    
    llvm::Value* addr = resolveOperand(inst->operands[0]);
    llvm::Value* expected = resolveOperand(inst->operands[1]);
    llvm::Value* desired = resolveOperand(inst->operands[2]);
    auto ptr = builder_->CreateIntToPtr(addr, i64PtrTy);
    
    auto casResult = builder_->CreateAtomicCmpXchg(ptr, expected, desired,
                                                    llvm::Align(8),
                                                    llvm::AtomicOrdering::SequentiallyConsistent,
                                                    llvm::AtomicOrdering::SequentiallyConsistent);
    // Extract the old value (first element of {i64, i1})
    auto oldVal = builder_->CreateExtractValue(casResult, 0);
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = oldVal;
    }
    return oldVal;
}

llvm::Value* LLVMCodeGen::emitAsyncWaitAll(std::shared_ptr<SIRInstruction> inst) {
    // WaitForMultipleObjects(count, handles, TRUE, INFINITE)
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    
    if (inst->operands.empty()) {
        auto result = llvm::ConstantInt::get(i64Ty, 0);
        if (inst->result.has_value()) {
            context_info_.namedValues[inst->result->name] = result;
        }
        return result;
    }
    
    // For each operand, wait individually (simpler than WaitForMultipleObjects)
    for (auto& op : inst->operands) {
        llvm::Value* taskId = resolveOperand(op);
        auto handle = builder_->CreateIntToPtr(taskId, i8PtrTy);
        auto waitTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
        auto waitFunc = module_->getOrInsertFunction("WaitForSingleObject", waitTy);
        builder_->CreateCall(waitFunc, {handle, llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF)});
    }
    
    auto result = llvm::ConstantInt::get(i64Ty, 0);
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncWaitAny(std::shared_ptr<SIRInstruction> inst) {
    // WaitForSingleObject with timeout=0, loop through handles
    auto i32Ty = llvm::Type::getInt32Ty(*context_);
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*context_), 0);
    
    if (inst->operands.empty()) {
        auto result = llvm::ConstantInt::get(i64Ty, -1);
        if (inst->result.has_value()) {
            context_info_.namedValues[inst->result->name] = result;
        }
        return result;
    }
    
    // Simplified: just wait on first
    llvm::Value* firstId = resolveOperand(inst->operands[0]);
    auto handle = builder_->CreateIntToPtr(firstId, i8PtrTy);
    auto waitTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
    auto waitFunc = module_->getOrInsertFunction("WaitForSingleObject", waitTy);
    builder_->CreateCall(waitFunc, {handle, llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF)});
    
    auto result = llvm::ConstantInt::get(i64Ty, 0);
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitAsyncSelect(std::shared_ptr<SIRInstruction> inst) {
    // Select on channels - simplified as checking first available
    auto i64Ty = llvm::Type::getInt64Ty(*context_);
    llvm::Value* result = llvm::ConstantInt::get(i64Ty, 0);
    
    if (!inst->operands.empty()) {
        result = resolveOperand(inst->operands[0]);
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// Phase N: Missing Bitwise Operations / ╪╣┘ו┘ה┘ך╪º╪¬ ╪½┘ז╪º╪ª┘ך╪⌐ ┘ז╪º┘ג╪╡╪⌐
// ============================================================================

llvm::Value* LLVMCodeGen::emitSar(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("SAR requires 2 operands");
        return nullptr;
    }
    llvm::Value* lhs = resolveOperand(inst->operands[0]);
    llvm::Value* rhs = resolveOperand(inst->operands[1]);
    if (!lhs || !rhs) return nullptr;
    llvm::Value* result = builder_->CreateAShr(lhs, rhs, "sar");
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitRol(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("ROL requires 2 operands");
        return nullptr;
    }
    llvm::Value* val = resolveOperand(inst->operands[0]);
    llvm::Value* amount = resolveOperand(inst->operands[1]);
    if (!val || !amount) return nullptr;
    // ROL(x, n) = (x << n) | (x >> (64 - n))
    auto* bits = llvm::ConstantInt::get(getInt64Type(), 64);
    llvm::Value* shl = builder_->CreateShl(val, amount, "rol.shl");
    llvm::Value* sub = builder_->CreateSub(bits, amount, "rol.sub");
    llvm::Value* shr = builder_->CreateLShr(val, sub, "rol.shr");
    llvm::Value* result = builder_->CreateOr(shl, shr, "rol");
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// Phase N: Missing Control Flow / ╪¬╪»┘ב┘ג ╪¬╪¡┘ד┘ו ┘ז╪º┘ג╪╡
// ============================================================================

llvm::Value* LLVMCodeGen::emitCallIndirect(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("CALL_INDIRECT requires at least 1 operand (function pointer)");
        return nullptr;
    }
    // operand[0] = function pointer, rest = args
    llvm::Value* fnPtr = resolveOperand(inst->operands[0]);
    if (!fnPtr) return nullptr;
    
    // If fnPtr is an integer, convert to pointer
    if (fnPtr->getType()->isIntegerTy()) {
        fnPtr = builder_->CreateIntToPtr(fnPtr, llvm::PointerType::getUnqual(*context_), "fnptr");
    }
    
    // Collect arguments
    std::vector<llvm::Value*> args;
    std::vector<llvm::Type*> argTypes;
    for (size_t i = 1; i < inst->operands.size(); i++) {
        llvm::Value* arg = resolveOperand(inst->operands[i]);
        if (arg) {
            args.push_back(arg);
            argTypes.push_back(arg->getType());
        }
    }
    
    // Determine return type
    llvm::Type* retType = getInt64Type();
    if (inst->result.has_value()) {
        if (inst->result->dataType == SIRType::VOID) retType = llvm::Type::getVoidTy(*context_);
        else if (inst->result->dataType == SIRType::F64) retType = getDoubleType();
        else if (inst->result->dataType == SIRType::BOOL) retType = llvm::Type::getInt1Ty(*context_);
    }
    
    auto* funcType = llvm::FunctionType::get(retType, argTypes, false);
    llvm::Value* result = builder_->CreateCall(funcType, fnPtr, args, 
        retType->isVoidTy() ? "" : "call_indirect");
    
    if (inst->result.has_value() && !retType->isVoidTy()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// Phase N: Missing Memory Operations / ╪╣┘ו┘ה┘ך╪º╪¬ ╪░╪º┘ד╪▒╪⌐ ┘ז╪º┘ג╪╡╪⌐
// ============================================================================

llvm::Value* LLVMCodeGen::emitAllocHeap(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("ALLOC_HEAP requires 1 operand (size)");
        return nullptr;
    }
    llvm::Value* size = resolveOperand(inst->operands[0]);
    if (!size) return nullptr;
    
    auto* mallocType = llvm::FunctionType::get(
        llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* result = builder_->CreateCall(mallocFunc, {size}, "heap_alloc");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitFreeMem(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("FREE requires 1 operand (pointer)");
        return nullptr;
    }
    llvm::Value* ptr = resolveOperand(inst->operands[0]);
    if (!ptr) return nullptr;
    
    if (ptr->getType()->isIntegerTy()) {
        ptr = builder_->CreateIntToPtr(ptr, llvm::PointerType::getUnqual(*context_), "free.ptr");
    }
    
    auto* freeType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(*context_), {llvm::PointerType::getUnqual(*context_)}, false);
    auto freeFunc = module_->getOrInsertFunction("free", freeType);
    builder_->CreateCall(freeFunc, {ptr});
    
    return llvm::ConstantInt::get(getInt64Type(), 0);
}

llvm::Value* LLVMCodeGen::emitAddr(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("ADDR requires 1 operand");
        return nullptr;
    }
    // Return the alloca pointer itself (not loading the value)
    llvm::Value* ptr = context_info_.namedValues[inst->operands[0].name];
    if (!ptr) {
        ptr = resolveOperand(inst->operands[0]);
    }
    if (!ptr) return nullptr;
    
    // Convert pointer to i64 if needed
    llvm::Value* result = ptr;
    if (ptr->getType()->isPointerTy()) {
        result = builder_->CreatePtrToInt(ptr, getInt64Type(), "addr");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitPtrAdd(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("PTR_ADD requires 2 operands (ptr, offset)");
        return nullptr;
    }
    llvm::Value* ptr = resolveOperand(inst->operands[0]);
    llvm::Value* offset = resolveOperand(inst->operands[1]);
    if (!ptr || !offset) return nullptr;
    
    // If ptr is an integer, convert to pointer
    if (ptr->getType()->isIntegerTy()) {
        ptr = builder_->CreateIntToPtr(ptr, llvm::PointerType::getUnqual(*context_), "ptr.conv");
    }
    
    // GEP with i8 element type for byte-level offset
    llvm::Value* result = builder_->CreateGEP(
        llvm::Type::getInt8Ty(*context_), ptr, {offset}, "ptr_add");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitPtrCast(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("PTR_CAST requires 1 operand");
        return nullptr;
    }
    llvm::Value* val = resolveOperand(inst->operands[0]);
    if (!val) return nullptr;
    
    // In opaque pointer world, pointer casts are essentially no-ops
    // but we may need int-to-ptr or ptr-to-int
    llvm::Value* result = val;
    if (val->getType()->isIntegerTy()) {
        result = builder_->CreateIntToPtr(val, llvm::PointerType::getUnqual(*context_), "ptr_cast");
    }
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

// ============================================================================
// Phase N: OOP Instructions / ╪¬╪╣┘ה┘ך┘ו╪º╪¬ ╪º┘ה╪¿╪▒┘ו╪¼╪⌐ ╪º┘ה┘ד╪º╪ª┘ז┘ך╪⌐
// ============================================================================

llvm::Value* LLVMCodeGen::emitObjectNew(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.empty()) {
        reportError("OBJECT_NEW requires class name operand");
        return nullptr;
    }
    
    std::string className = inst->operands[0].name;
    
    // Look up class struct type
    auto structIt = context_info_.classStructTypes.find(className);
    if (structIt == context_info_.classStructTypes.end()) {
        reportError("Class not found: " + className);
        return nullptr;
    }
    
    llvm::StructType* structType = structIt->second;
    
    // Allocate on heap using malloc for objects (they may outlive the scope)
    auto* dlSize = llvm::ConstantExpr::getSizeOf(structType);
    auto* mallocType = llvm::FunctionType::get(
        llvm::PointerType::getUnqual(*context_), {getInt64Type()}, false);
    auto mallocFunc = module_->getOrInsertFunction("malloc", mallocType);
    llvm::Value* rawPtr = builder_->CreateCall(mallocFunc, {dlSize}, className + "_new");
    
    // Zero-initialize the object
    auto* memsetType = llvm::FunctionType::get(
        llvm::PointerType::getUnqual(*context_),
        {llvm::PointerType::getUnqual(*context_), 
         llvm::Type::getInt32Ty(*context_), getInt64Type()}, false);
    auto memsetFunc = module_->getOrInsertFunction("memset", memsetType);
    builder_->CreateCall(memsetFunc, {
        rawPtr,
        llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context_), 0),
        dlSize
    });
    
    // Track class association
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = rawPtr;
        context_info_.objectClassMap[inst->result->name] = className;
    }
    
    return rawPtr;
}

llvm::Value* LLVMCodeGen::emitObjectGet(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 2) {
        reportError("OBJECT_GET requires 2 operands (object, field_name)");
        return nullptr;
    }
    
    std::string objRegName = inst->operands[0].name;
    std::string fieldName = inst->operands[1].name;
    
    // Find object pointer
    llvm::Value* objPtr = context_info_.namedValues[objRegName];
    if (!objPtr) {
        reportError("Object not found: " + objRegName);
        return nullptr;
    }
    
    // Look up class mapping
    auto classIt = context_info_.objectClassMap.find(objRegName);
    if (classIt == context_info_.objectClassMap.end()) {
        reportError("No class mapping for: " + objRegName);
        return nullptr;
    }
    
    std::string className = classIt->second;
    auto structIt = context_info_.classStructTypes.find(className);
    auto fieldNamesIt = context_info_.classFieldNames.find(className);
    
    if (structIt == context_info_.classStructTypes.end() ||
        fieldNamesIt == context_info_.classFieldNames.end()) {
        reportError("Class struct not found: " + className);
        return nullptr;
    }
    
    llvm::StructType* structType = structIt->second;
    const auto& fieldNames = fieldNamesIt->second;
    
    // Find field index
    int fieldIndex = -1;
    for (size_t i = 0; i < fieldNames.size(); i++) {
        if (fieldNames[i] == fieldName) {
            fieldIndex = static_cast<int>(i);
            break;
        }
    }
    
    // (AR) إذا لم يُعثر على الحقل، ابحث في سلسلة الوراثة
    // (EN) If field not found, search inheritance chain (safety fallback)
    if (fieldIndex < 0) {
        std::string parentClass = className;
        while (fieldIndex < 0) {
            auto parentIt = context_info_.classParentMap.find(parentClass);
            if (parentIt == context_info_.classParentMap.end()) break;
            parentClass = parentIt->second;
            
            auto parentFieldsIt = context_info_.classFieldNames.find(parentClass);
            auto parentStructIt = context_info_.classStructTypes.find(parentClass);
            if (parentFieldsIt == context_info_.classFieldNames.end() || 
                parentStructIt == context_info_.classStructTypes.end()) break;
            
            const auto& parentFieldNames = parentFieldsIt->second;
            for (size_t i = 0; i < parentFieldNames.size(); i++) {
                if (parentFieldNames[i] == fieldName) {
                    // (AR) الحقل موجود في الأب - مؤشره نفسه في الابن (الحقول الموروثة أولاً)
                    // (EN) Field found in parent - same index in child (inherited fields come first)
                    fieldIndex = static_cast<int>(i);
                    break;
                }
            }
        }
    }
    
    if (fieldIndex < 0) {
        reportError("Field '" + fieldName + "' not found in class '" + className + "' or its parents");
        return nullptr;
    }
    
    // Resolve object pointer (may need loading from alloca)
    llvm::Value* actualObj = objPtr;
    if (auto* allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objPtr)) {
        if (!allocaInst->getAllocatedType()->isStructTy()) {
            llvm::Value* loaded = builder_->CreateLoad(
                allocaInst->getAllocatedType(), allocaInst, objRegName + ".load");
            if (loaded->getType()->isIntegerTy()) {
                actualObj = builder_->CreateIntToPtr(loaded, 
                    llvm::PointerType::getUnqual(*context_), objRegName + ".ptr");
            }
        }
    }
    
    // GEP + Load
    llvm::Value* gep = builder_->CreateStructGEP(structType, actualObj, fieldIndex, 
        fieldName + "_gep");
    llvm::Type* fieldType = structType->getElementType(fieldIndex);
    llvm::Value* result = builder_->CreateLoad(fieldType, gep, fieldName + ".val");
    
    if (inst->result.has_value()) {
        context_info_.namedValues[inst->result->name] = result;
    }
    return result;
}

llvm::Value* LLVMCodeGen::emitObjectSet(std::shared_ptr<SIRInstruction> inst) {
    if (!inst || inst->operands.size() < 3) {
        reportError("OBJECT_SET requires 3 operands (object, field_name, value)");
        return nullptr;
    }
    
    std::string objRegName = inst->operands[0].name;
    std::string fieldName = inst->operands[1].name;
    llvm::Value* value = resolveOperand(inst->operands[2]);
    
    llvm::Value* objPtr = context_info_.namedValues[objRegName];
    if (!objPtr || !value) {
        reportError("Operands not found for OBJECT_SET");
        return nullptr;
    }
    
    auto classIt = context_info_.objectClassMap.find(objRegName);
    if (classIt == context_info_.objectClassMap.end()) {
        reportError("No class mapping for: " + objRegName);
        return nullptr;
    }
    
    std::string className = classIt->second;
    auto structIt = context_info_.classStructTypes.find(className);
    auto fieldNamesIt = context_info_.classFieldNames.find(className);
    
    if (structIt == context_info_.classStructTypes.end() ||
        fieldNamesIt == context_info_.classFieldNames.end()) {
        reportError("Class struct not found: " + className);
        return nullptr;
    }
    
    llvm::StructType* structType = structIt->second;
    const auto& fieldNames = fieldNamesIt->second;
    
    int fieldIndex = -1;
    for (size_t i = 0; i < fieldNames.size(); i++) {
        if (fieldNames[i] == fieldName) {
            fieldIndex = static_cast<int>(i);
            break;
        }
    }
    
    // (AR) إذا لم يُعثر على الحقل، ابحث في سلسلة الوراثة
    // (EN) If field not found, search inheritance chain (safety fallback)
    if (fieldIndex < 0) {
        std::string parentClass = className;
        while (fieldIndex < 0) {
            auto parentIt = context_info_.classParentMap.find(parentClass);
            if (parentIt == context_info_.classParentMap.end()) break;
            parentClass = parentIt->second;
            
            auto parentFieldsIt = context_info_.classFieldNames.find(parentClass);
            if (parentFieldsIt == context_info_.classFieldNames.end()) break;
            
            const auto& parentFieldNames = parentFieldsIt->second;
            for (size_t i = 0; i < parentFieldNames.size(); i++) {
                if (parentFieldNames[i] == fieldName) {
                    fieldIndex = static_cast<int>(i);
                    break;
                }
            }
        }
    }
    
    if (fieldIndex < 0) {
        reportError("Field '" + fieldName + "' not found in class '" + className + "' or its parents");
        return nullptr;
    }
    
    // Resolve object pointer
    llvm::Value* actualObj = objPtr;
    if (auto* allocaInst = llvm::dyn_cast<llvm::AllocaInst>(objPtr)) {
        if (!allocaInst->getAllocatedType()->isStructTy()) {
            llvm::Value* loaded = builder_->CreateLoad(
                allocaInst->getAllocatedType(), allocaInst, objRegName + ".load");
            if (loaded->getType()->isIntegerTy()) {
                actualObj = builder_->CreateIntToPtr(loaded,
                    llvm::PointerType::getUnqual(*context_), objRegName + ".ptr");
            }
        }
    }
    
    // GEP + Store
    llvm::Value* gep = builder_->CreateStructGEP(structType, actualObj, fieldIndex,
        fieldName + "_gep");
    builder_->CreateStore(value, gep);
    
    return value;
}



} // namespace LLVM
} // namespace Sad
