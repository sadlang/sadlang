/*
 * ============================================================================
 * InstrCoreCodeGen - Core LLVM IR instruction emission (29KB switch)
 * Phase 8 Step 9 - 1 method
 * ============================================================================
 */
#ifndef SAD_LLVM_INSTR_CORE_CODEGEN_H
#define SAD_LLVM_INSTR_CORE_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class InstrCoreCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit InstrCoreCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    InstrCoreCodeGen(const InstrCoreCodeGen &) = delete;
    InstrCoreCodeGen &operator=(const InstrCoreCodeGen &) = delete;

    llvm::Value *emitInstructionCore(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
