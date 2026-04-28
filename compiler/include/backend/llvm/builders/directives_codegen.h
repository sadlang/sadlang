/*
 * ============================================================================
 * DirectivesCodeGen - Phase 8 Step 10 - 7 method(s)
 * ============================================================================
 */
#ifndef SAD_LLVM_DIRECTIVES_CODEGEN_H
#define SAD_LLVM_DIRECTIVES_CODEGEN_H

#include <memory>
#include <string>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class DirectivesCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit DirectivesCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    DirectivesCodeGen(const DirectivesCodeGen &) = delete;
    DirectivesCodeGen &operator=(const DirectivesCodeGen &) = delete;

    llvm::Value *emitSizeof(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAtomicLoad(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAtomicStore(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAtomicAdd(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAtomicSub(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAtomicExchange(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitAtomicCmpXchg(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
