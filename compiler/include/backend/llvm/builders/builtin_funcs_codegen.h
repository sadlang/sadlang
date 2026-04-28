/*
 * ============================================================================
 * BuiltinFuncsCodeGen - LLVM IR for builtin functions + math/async builtins
 * Phase 8 Step 5 - 19 methods
 * ============================================================================
 */
#ifndef SAD_LLVM_BUILTIN_FUNCS_CODEGEN_H
#define SAD_LLVM_BUILTIN_FUNCS_CODEGEN_H

#include <memory>
#include <string>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class BuiltinFuncsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit BuiltinFuncsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    BuiltinFuncsCodeGen(const BuiltinFuncsCodeGen &) = delete;
    BuiltinFuncsCodeGen &operator=(const BuiltinFuncsCodeGen &) = delete;

    llvm::Value *emitBuiltinAssert(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinDebug(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitPhi(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinIsType(std::shared_ptr<SIRInstruction> inst, const std::string &typeName);
    llvm::Value *emitBuiltinToBool(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinReadLine(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinClearScreen(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSum(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinRead(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinSleep(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinExit(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncSpawn(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncAwait(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncYield(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncSleep(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncCreateFuture(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncResolveFuture(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncGetFuture(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAsyncFutureIsReady(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
