/*
 * ============================================================================
 * ClosureCodeGen - LLVM IR generation for closures, env capture, indirect calls
 * Phase 7 Step 14 - 9 methods
 * ============================================================================
 */
#ifndef SAD_LLVM_CLOSURE_CODEGEN_H
#define SAD_LLVM_CLOSURE_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class ClosureCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit ClosureCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    ClosureCodeGen(const ClosureCodeGen &) = delete;
    ClosureCodeGen &operator=(const ClosureCodeGen &) = delete;

    llvm::Value *emitAllocHeap(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFreeMem(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitCallIndirect(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitClosureCreate(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitClosureCall(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitEnvLoad(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitEnvStore(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitRol(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitSar(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
