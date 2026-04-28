/*
 * ============================================================================
 * InstrLowlevelCodeGen - Phase 8 Step 10 - 1 method(s)
 * ============================================================================
 */
#ifndef SAD_LLVM_INSTRLOWLEVEL_CODEGEN_H
#define SAD_LLVM_INSTRLOWLEVEL_CODEGEN_H

#include <memory>
#include <string>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class InstrLowlevelCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit InstrLowlevelCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    InstrLowlevelCodeGen(const InstrLowlevelCodeGen &) = delete;
    InstrLowlevelCodeGen &operator=(const InstrLowlevelCodeGen &) = delete;

    llvm::Value *emitInstructionLowlevel(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
