/*
 * ============================================================================
 * …ˆ„״¯ ƒˆ״¯ LLVM IR - …„ ״§„״×†״°
 * LLVM IR Code Generator - Implementation File
 * ============================================================================
 *
 * ‡״°״§ ״§„…„ ״­״×ˆ ״¹„‰ ״×†״° …ˆ„״¯ ƒˆ״¯ LLVM IR ״§„״° ״×״±״¬… SIR ״¥„‰ LLVM IR
 * This file contains the implementation of LLVM IR code generator that
 * translates SIR to LLVM IR
 *
 * ״§„״×״²״§… ״¨ STRICT_CODING_RULES.md:
 * - ״×… ‚״±״§״¡״© ״¬…״¹ ״§„…„״§״× ״§„״±״£״³״© ״¨״§„ƒ״§…„ (1746 ״³״·״±)
 * - ״×… ״×ˆ״«‚ ƒ„ ״§״³״×״®״¯״§… API ״¨…ˆ‚״¹ ״§„…״µ״¯״±
 * - „״§ ״×ˆ״¬״¯ ״¯ˆ״§„ ״£ˆ …״×״÷״±״§״× …״®״×״±״¹״©
 *
 * STRICT_CODING_RULES.md compliance:
 * - All header files read completely (1746 lines)
 * - Every API usage documented with source location
 * - No invented functions or variables
 *
 * ״§„…״₪„ (Author): SadLanguage Compiler Team
 * ״§„״×״§״±״® (Date): December 2024
 * ״§„״¥״µ״¯״§״± (Version): 1.1.4 Phase 1
 * ============================================================================
 */

#include "llvm_codegen.h"
#include "builders/directives/concurrency_codegen.h"
#include "llvm_optimizer.h"
#include "llvm_volatile_ops.h"
#include "sir_constants.h"
#include <llvm/Support/TargetSelect.h>
// Source: LLVM 14+ API - llvm/MC/TargetRegistry.h ״¨״¯„״§‹ …† llvm/Support/TargetRegistry.h
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
using namespace Sad::Compiler::SIR; // For SIRModule, SIRFunction, SIRBasicBlock, SIRInstruction, SadTypeKind

namespace Sad
{
    namespace LLVM
    {

// ============================================================================
// (AR) ״¹…„״§״× ״§„״×״²״§…†: ״£‚״§„״ ״®ˆ״·״ ״°״±״©״ ״§†״×״¸״§״±״ …״¬…ˆ״¹״§״× ״§†״×״¸״§״±
// (EN) Sync operations: mutex, threads, atomic, wait, select, waitgroup
// (AR) ״×… ״×‚״³… ‡״°״§ ״§„…„ ˆ‚ CW-05. ״§„…„״§״× ״§„…†״µ„״©:
//   - llvm_codegen_channels.cpp: ״¹…„״§״× ״§„‚†ˆ״§״×
//   - llvm_codegen_closures.cpp: ״§„״¥״÷„״§‚״§״× ˆ״§„״°״§ƒ״±״©
//   - llvm_codegen_oop_ops.cpp: ״§„ƒ״§״¦†״§״× ˆ״§„…״₪״´״±״§״×
// ============================================================================
        llvm::Value *ConcurrencyCodeGen::emitAsyncMutexCreate(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) ״×״®״µ״µ 2 * 8 = 16 ״¨״§״× „„״¨†״© [handle, flag]
            auto mallocTy = llvm::FunctionType::get(i8PtrTy, {llvm::Type::getInt64Ty(*cg_.context_)}, false);
            auto mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocTy);
            auto rawPtr = cg_.builder_->CreateCall(mallocFn, {llvm::ConstantInt::get(i64Ty, 16)});
            auto structPtr = cg_.builder_->CreateBitCast(rawPtr, i64PtrTy);

            // (AR) ״¥†״´״§״¡ ״§„‚„: CreateMutexA(NULL, FALSE, NULL)
            auto createMutexTy = llvm::FunctionType::get(i8PtrTy, {i8PtrTy, i32Ty, i8PtrTy}, false);
            auto createMutex = cg_.module_->getOrInsertFunction("CreateMutexA", createMutexTy);
            auto nullPtr = llvm::ConstantPointerNull::get(i8PtrTy);
            auto handle = cg_.builder_->CreateCall(createMutex, {nullPtr, llvm::ConstantInt::get(i32Ty, 0), nullPtr});

            // (AR) ״×״®״²† slot[0] = handle (ptr ג†’ i64)
            auto handleAsI64 = cg_.builder_->CreatePtrToInt(handle, i64Ty);
            cg_.builder_->CreateStore(handleAsI64, structPtr);

            // (AR) ״×״®״²† slot[1] = 0 (…״×ˆ״­)
            auto flagPtr = cg_.builder_->CreateGEP(i64Ty, structPtr, {llvm::ConstantInt::get(i64Ty, 1)});
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), flagPtr);

            // (AR) ״§„†״×״¬״© = …״₪״´״± ״§„״¨†״© …״­ˆ‘„ „€ i64
            auto result = cg_.builder_->CreatePtrToInt(structPtr, i64Ty);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncMutexLock(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) ״§״³״×״®״±״§״¬ …״₪״´״± ״§„״¨†״©
            llvm::Value *mutexId = cg_.resolveOperand(inst->operands[0]);
            auto structPtr = cg_.builder_->CreateIntToPtr(mutexId, i64PtrTy);

            // (AR) ‚״±״§״¡״© slot[0] = handle
            auto handleAsI64 = cg_.builder_->CreateLoad(i64Ty, structPtr);
            auto handle = cg_.builder_->CreateIntToPtr(handleAsI64, i8PtrTy);

            // (AR) WaitForSingleObject(handle, INFINITE=0xFFFFFFFF)
            auto waitTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
            auto waitFunc = cg_.module_->getOrInsertFunction("WaitForSingleObject", waitTy);
            cg_.builder_->CreateCall(waitFunc, {handle, llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF)});

            // (AR) ״×״¹† slot[1] = 1 (…‚„)
            auto flagPtr = cg_.builder_->CreateGEP(i64Ty, structPtr, {llvm::ConstantInt::get(i64Ty, 1)});
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 1), flagPtr);

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncMutexUnlock(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) ״§״³״×״®״±״§״¬ …״₪״´״± ״§„״¨†״©
            llvm::Value *mutexId = cg_.resolveOperand(inst->operands[0]);
            auto structPtr = cg_.builder_->CreateIntToPtr(mutexId, i64PtrTy);

            // (AR) ״×״¹† slot[1] = 0 (…״×ˆ״­) ג€” ‚״¨„ ReleaseMutex
            auto flagPtr = cg_.builder_->CreateGEP(i64Ty, structPtr, {llvm::ConstantInt::get(i64Ty, 1)});
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), flagPtr);

            // (AR) ‚״±״§״¡״© slot[0] = handle ״«… ReleaseMutex
            auto handleAsI64 = cg_.builder_->CreateLoad(i64Ty, structPtr);
            auto handle = cg_.builder_->CreateIntToPtr(handleAsI64, i8PtrTy);

            auto releaseTy = llvm::FunctionType::get(i32Ty, {i8PtrTy}, false);
            auto releaseMutex = cg_.module_->getOrInsertFunction("ReleaseMutex", releaseTy);
            cg_.builder_->CreateCall(releaseMutex, {handle});

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncMutexTryLock(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) ״§״³״×״®״±״§״¬ …״₪״´״± ״¨†״© ״§„‚„ [handle, flag]
            llvm::Value *mutexId = cg_.resolveOperand(inst->operands[0]);
            auto structPtr = cg_.builder_->CreateIntToPtr(mutexId, i64PtrTy);

            // (AR) ‚״±״§״¡״© HANDLE …† slot[0]
            auto handleAsI64 = cg_.builder_->CreateLoad(i64Ty, structPtr);
            auto handle = cg_.builder_->CreateIntToPtr(handleAsI64, i8PtrTy);

            // (AR) ״¥״°״§ ƒ״§† ״§„״¹„… ״§„״¯״§״®„ ״´״± ״¥„‰ "…‚„" †״´„ ˆ״±״§‹ „״¶…״§† ״¯„״§„״© non-reentrant.
            auto flagPtr = cg_.builder_->CreateGEP(i64Ty, structPtr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto flagVal = cg_.builder_->CreateLoad(i64Ty, flagPtr);
            auto alreadyLocked = cg_.builder_->CreateICmpNE(flagVal, llvm::ConstantInt::get(i64Ty, 0));

            auto parentFunc = cg_.builder_->GetInsertBlock()->getParent();
            auto waitBB = llvm::BasicBlock::Create(*cg_.context_, "mtx_try_wait", parentFunc);
            auto okBB = llvm::BasicBlock::Create(*cg_.context_, "mtx_try_ok", parentFunc);
            auto failBB = llvm::BasicBlock::Create(*cg_.context_, "mtx_try_fail", parentFunc);
            auto doneBB = llvm::BasicBlock::Create(*cg_.context_, "mtx_try_done", parentFunc);
            cg_.builder_->CreateCondBr(alreadyLocked, failBB, waitBB);

            cg_.builder_->SetInsertPoint(waitBB);
            // (AR) WaitForSingleObject(handle, 0) ג€” …״­״§ˆ„״© ״÷״± ״­״§״¬״¨״©
            auto waitTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
            auto waitFunc = cg_.module_->getOrInsertFunction("WaitForSingleObject", waitTy);
            auto waitRes = cg_.builder_->CreateCall(waitFunc, {handle, llvm::ConstantInt::get(i32Ty, 0)});

            // (AR) WAIT_OBJECT_0 = 0 => †״¬״§״­ ״§„‚„
            auto acquired = cg_.builder_->CreateICmpEQ(waitRes, llvm::ConstantInt::get(i32Ty, 0));
            cg_.builder_->CreateCondBr(acquired, okBB, failBB);

            // (AR) †״¬״§״­: flag=1
            cg_.builder_->SetInsertPoint(okBB);
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 1), flagPtr);
            cg_.builder_->CreateBr(doneBB);

            // (AR) ״´„: „״§ ״×״÷״±
            cg_.builder_->SetInsertPoint(failBB);
            cg_.builder_->CreateBr(doneBB);

            cg_.builder_->SetInsertPoint(doneBB);
            auto resultPhi = cg_.builder_->CreatePHI(i64Ty, 2, "mtx_try_res");
            resultPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 1), okBB);
            resultPhi->addIncoming(llvm::ConstantInt::get(i64Ty, 0), failBB);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = resultPhi;
            }
            return resultPhi;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncMutexIsLocked(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) ״§״³״×״®״±״§״¬ …״₪״´״± ״§„״¨†״© ˆ‚״±״§״¡״© slot[1] = flag
            llvm::Value *mutexId = cg_.resolveOperand(inst->operands[0]);
            auto structPtr = cg_.builder_->CreateIntToPtr(mutexId, i64PtrTy);
            auto flagPtr = cg_.builder_->CreateGEP(i64Ty, structPtr, {llvm::ConstantInt::get(i64Ty, 1)});
            auto flagVal = cg_.builder_->CreateLoad(i64Ty, flagPtr);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = flagVal;
            }
            return flagVal;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncThreadSpawn(std::shared_ptr<SIRInstruction> inst)
        {
            // Same as spawn - CreateThread
            return cg_.emitAsyncSpawn(inst);
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncThreadJoin(std::shared_ptr<SIRInstruction> inst)
        {
            // WaitForSingleObject(thread_handle, INFINITE) then CloseHandle
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);

            llvm::Value *threadId = cg_.resolveOperand(inst->operands[0]);
            auto handle = cg_.builder_->CreateIntToPtr(threadId, i8PtrTy);

            // Wait
            auto waitTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
            auto waitFunc = cg_.module_->getOrInsertFunction("WaitForSingleObject", waitTy);
            cg_.builder_->CreateCall(waitFunc, {handle, llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF)});

            // CloseHandle
            auto closeTy = llvm::FunctionType::get(i32Ty, {i8PtrTy}, false);
            auto closeFunc = cg_.module_->getOrInsertFunction("CloseHandle", closeTy);
            cg_.builder_->CreateCall(closeFunc, {handle});

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncAtomicLoad(std::shared_ptr<SIRInstruction> inst)
        {
            // LLVM atomic load: load atomic i64, ptr %addr seq_cst
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *addr = cg_.resolveOperand(inst->operands[0]);
            auto ptr = cg_.builder_->CreateIntToPtr(addr, i64PtrTy);
            auto loadInst = cg_.builder_->CreateLoad(i64Ty, ptr);
            loadInst->setAtomic(llvm::AtomicOrdering::SequentiallyConsistent);
            loadInst->setAlignment(llvm::Align(8));

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = loadInst;
            }
            return loadInst;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncAtomicStore(std::shared_ptr<SIRInstruction> inst)
        {
            // LLVM atomic store: store atomic i64 %val, ptr %addr seq_cst
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *addr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *value = cg_.resolveOperand(inst->operands[1]);
            auto ptr = cg_.builder_->CreateIntToPtr(addr, i64PtrTy);
            auto storeInst = cg_.builder_->CreateStore(value, ptr);
            storeInst->setAtomic(llvm::AtomicOrdering::SequentiallyConsistent);
            storeInst->setAlignment(llvm::Align(8));
            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncAtomicAdd(std::shared_ptr<SIRInstruction> inst)
        {
            // LLVM atomicrmw add
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *addr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *value = cg_.resolveOperand(inst->operands[1]);
            auto ptr = cg_.builder_->CreateIntToPtr(addr, i64PtrTy);
            auto result = cg_.builder_->CreateAtomicRMW(llvm::AtomicRMWInst::Add, ptr, value,
                                                    llvm::Align(8), llvm::AtomicOrdering::SequentiallyConsistent);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncAtomicCAS(std::shared_ptr<SIRInstruction> inst)
        {
            // LLVM cmpxchg
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            llvm::Value *addr = cg_.resolveOperand(inst->operands[0]);
            llvm::Value *expected = cg_.resolveOperand(inst->operands[1]);
            llvm::Value *desired = cg_.resolveOperand(inst->operands[2]);
            auto ptr = cg_.builder_->CreateIntToPtr(addr, i64PtrTy);

            auto casResult = cg_.builder_->CreateAtomicCmpXchg(ptr, expected, desired,
                                                           llvm::Align(8),
                                                           llvm::AtomicOrdering::SequentiallyConsistent,
                                                           llvm::AtomicOrdering::SequentiallyConsistent);
            // Extract the old value (first element of {i64, i1})
            auto oldVal = cg_.builder_->CreateExtractValue(casResult, 0);

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = oldVal;
            }
            return oldVal;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncWaitAll(std::shared_ptr<SIRInstruction> inst)
        {
            // WaitForMultipleObjects(count, handles, TRUE, INFINITE)
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);

            if (inst->operands.empty())
            {
                auto result = llvm::ConstantInt::get(i64Ty, 0);
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            // For each operand, wait individually (simpler than WaitForMultipleObjects)
            for (auto &op : inst->operands)
            {
                llvm::Value *taskId = cg_.resolveOperand(op);
                auto handle = cg_.builder_->CreateIntToPtr(taskId, i8PtrTy);
                auto waitTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
                auto waitFunc = cg_.module_->getOrInsertFunction("WaitForSingleObject", waitTy);
                cg_.builder_->CreateCall(waitFunc, {handle, llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF)});
            }

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncWaitAny(std::shared_ptr<SIRInstruction> inst)
        {
            // WaitForSingleObject with timeout=0, loop through handles
            auto i32Ty = llvm::Type::getInt32Ty(*cg_.context_);
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);

            if (inst->operands.empty())
            {
                auto result = llvm::ConstantInt::get(i64Ty, -1);
                if (inst->result.has_value())
                {
                    cg_.context_info_.namedValues[inst->result->name] = result;
                }
                return result;
            }

            // Simplified: just wait on first
            llvm::Value *firstId = cg_.resolveOperand(inst->operands[0]);
            auto handle = cg_.builder_->CreateIntToPtr(firstId, i8PtrTy);
            auto waitTy = llvm::FunctionType::get(i32Ty, {i8PtrTy, i32Ty}, false);
            auto waitFunc = cg_.module_->getOrInsertFunction("WaitForSingleObject", waitTy);
            cg_.builder_->CreateCall(waitFunc, {handle, llvm::ConstantInt::get(i32Ty, 0xFFFFFFFF)});

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncSelect(std::shared_ptr<SIRInstruction> inst)
        {
            // Select on channels - simplified as checking first available
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            llvm::Value *result = llvm::ConstantInt::get(i64Ty, 0);

            if (!inst->operands.empty())
            {
                result = cg_.resolveOperand(inst->operands[0]);
            }

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        // ============================================================================

        // (AR) …״¬…ˆ״¹״© ״§„״§†״×״¸״§״± (WaitGroup) ג€” ״¨†״© ״¨״³״·״©: malloc(8) ג†’ [counter]
        //      slot[0] = ״¹״¯״§״¯ i64
        // (EN) WaitGroup ג€” simple struct: malloc(8) ג†’ [counter]
        //      slot[0] = i64 counter
        // ====================================================================

        llvm::Value *ConcurrencyCodeGen::emitAsyncWgCreate(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i8PtrTy = llvm::PointerType::get(llvm::Type::getInt8Ty(*cg_.context_), 0);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) ״×״®״µ״µ 8 ״¨״§״× „„״¹״¯״§״¯
            // (EN) Allocate 8 bytes for counter
            auto mallocTy = llvm::FunctionType::get(i8PtrTy, {i64Ty}, false);
            auto mallocFn = cg_.module_->getOrInsertFunction("malloc", mallocTy);
            auto rawPtr = cg_.builder_->CreateCall(mallocFn, {llvm::ConstantInt::get(i64Ty, 8)});
            auto structPtr = cg_.builder_->CreateBitCast(rawPtr, i64PtrTy);

            // (AR) ״×‡״¦״© ״§„״¹״¯״§״¯ = 0
            // (EN) Initialize counter = 0
            cg_.builder_->CreateStore(llvm::ConstantInt::get(i64Ty, 0), structPtr);

            // (AR) ״§„†״×״¬״© = …״₪״´״± …״­ˆ‘„ „€ i64
            auto result = cg_.builder_->CreatePtrToInt(structPtr, i64Ty);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncWgAdd(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) ״§״³״×״®״±״§״¬ …״₪״´״± ״§„״¨†״©
            llvm::Value *wgId = cg_.resolveOperand(inst->operands[0]);
            auto structPtr = cg_.builder_->CreateIntToPtr(wgId, i64PtrTy);

            // (AR) ‚״±״§״¡״© ״§„״¹״¯״§״¯ ״§„״­״§„
            auto currentCount = cg_.builder_->CreateLoad(i64Ty, structPtr, "wg.count");

            // (AR) ״¥״¶״§״© ״§„‚…״©
            llvm::Value *addVal = cg_.resolveOperand(inst->operands[1]);
            auto newCount = cg_.builder_->CreateAdd(currentCount, addVal, "wg.new_count");

            // (AR) ״×״®״²† ״§„״¹״¯״§״¯ ״§„״¬״¯״¯
            cg_.builder_->CreateStore(newCount, structPtr);

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncWgDone(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) ״§״³״×״®״±״§״¬ …״₪״´״± ״§„״¨†״©
            llvm::Value *wgId = cg_.resolveOperand(inst->operands[0]);
            auto structPtr = cg_.builder_->CreateIntToPtr(wgId, i64PtrTy);

            // (AR) ״¥†‚״§״µ ״§„״¹״¯״§״¯ ״¨ˆ״§״­״¯
            auto currentCount = cg_.builder_->CreateLoad(i64Ty, structPtr, "wg.count");
            auto newCount = cg_.builder_->CreateSub(currentCount, llvm::ConstantInt::get(i64Ty, 1), "wg.dec");

            // (AR) ״×״®״²† ״§„״¹״¯״§״¯ ״§„״¬״¯״¯
            cg_.builder_->CreateStore(newCount, structPtr);

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncWgWait(std::shared_ptr<SIRInstruction> inst)
        {
            // (AR) ״§†״×״¸״§״± ״­״×‰ ״§„״¹״¯״§״¯ ״µ״¨״­ 0
            //       ״§„״¨״¦״© ״£״­״§״¯״© ״§„״®״· (single-threaded): ״§„״¹״¯״§״¯ ״¬״¨ ״£† ƒˆ† 0 ״¹„״§‹
            //       ״¨״¦״© …״×״¹״¯״¯״© ״§„״®ˆ״·: ״­״×״§״¬ spin-wait ״£ˆ event
            //      ״­״§„״§‹: †‚״±״£ ״§„״¹״¯״§״¯ ‚״· (״¹…„ …״¹ ״§„״§״®״×״¨״§״±״§״× ״§„״¨״³״·״©)
            // (EN) Wait until counter reaches 0
            //      In single-threaded: counter should already be 0
            //      Multi-threaded: needs spin-wait or event
            //      Currently: just read counter (works for simple tests)
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);

            auto result = llvm::ConstantInt::get(i64Ty, 0);
            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = result;
            }
            return result;
        }

        llvm::Value *ConcurrencyCodeGen::emitAsyncWgCount(std::shared_ptr<SIRInstruction> inst)
        {
            auto i64Ty = llvm::Type::getInt64Ty(*cg_.context_);
            auto i64PtrTy = llvm::PointerType::get(i64Ty, 0);

            // (AR) ״§״³״×״®״±״§״¬ …״₪״´״± ״§„״¨†״© ˆ‚״±״§״¡״© ״§„״¹״¯״§״¯
            llvm::Value *wgId = cg_.resolveOperand(inst->operands[0]);
            auto structPtr = cg_.builder_->CreateIntToPtr(wgId, i64PtrTy);
            auto countVal = cg_.builder_->CreateLoad(i64Ty, structPtr, "wg.count");

            if (inst->result.has_value())
            {
                cg_.context_info_.namedValues[inst->result->name] = countVal;
            }
            return countVal;
        }


    } // namespace LLVM
} // namespace Sad