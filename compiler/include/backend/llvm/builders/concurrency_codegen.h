/*
 * ============================================================================
 * ConcurrencyCodeGen - LLVM IR for async/concurrency operations
 * Phase 8 Step 1 - 31 methods
 *   - Atomics (4) + Mutex (5) + WaitGroup (5) + Threads (2) + Wait (2) + Select (1)
 *   - Channels (12)
 * ============================================================================
 */
#ifndef SAD_LLVM_CONCURRENCY_CODEGEN_H
#define SAD_LLVM_CONCURRENCY_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class ConcurrencyCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit ConcurrencyCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    ConcurrencyCodeGen(const ConcurrencyCodeGen &) = delete;
    ConcurrencyCodeGen &operator=(const ConcurrencyCodeGen &) = delete;

    llvm::Value *emitAsyncAtomicAdd(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncAtomicCAS(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncAtomicLoad(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncAtomicStore(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncMutexCreate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncMutexIsLocked(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncMutexLock(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncMutexTryLock(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncMutexUnlock(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncSelect(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncThreadJoin(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncThreadSpawn(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncWaitAll(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncWaitAny(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncWgAdd(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncWgCount(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncWgCreate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncWgDone(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncWgWait(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncChannelCapacity(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncChannelClose(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncChannelHasData(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncChannelIsClosed(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncChannelRecv(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncChannelRecvTimeout(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncChannelSend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncChannelSendTimeout(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncChannelSize(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncChannelTryRecv(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncChannelTrySend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncCreateChannel(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
