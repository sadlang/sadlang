/*
 * ============================================================================
 * CoroutinesCodeGen - LLVM IR for coroutines (suspend/resume/yield)
 * Phase 8 Step 7 - 6 methods
 * ============================================================================
 */
#ifndef SAD_LLVM_COROUTINES_CODEGEN_H
#define SAD_LLVM_COROUTINES_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include "sir_instruction.h"
#include "sir_module.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;
using SIRFunction = Compiler::SIR::SIRFunction;

class CoroutinesCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit CoroutinesCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    CoroutinesCodeGen(const CoroutinesCodeGen &) = delete;
    CoroutinesCodeGen &operator=(const CoroutinesCodeGen &) = delete;

    void emitCoroutinePreamble(std::shared_ptr<SIRFunction> sirFunc, llvm::Function *llvmFunc);
    void emitCoroutineEpilogue();
    llvm::Value *emitCoroSuspend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitCoroReturn(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitGeneratorYield(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitGeneratorConsume(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
