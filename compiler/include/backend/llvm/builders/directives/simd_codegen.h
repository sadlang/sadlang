/*
 * ============================================================================
 * SimdCodeGen - Phase 8 Step 10 - 1 method(s)
 * ============================================================================
 */
#ifndef SAD_LLVM_SIMD_CODEGEN_H
#define SAD_LLVM_SIMD_CODEGEN_H

#include <memory>
#include <string>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class SimdCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit SimdCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    SimdCodeGen(const SimdCodeGen &) = delete;
    SimdCodeGen &operator=(const SimdCodeGen &) = delete;

    llvm::Value *emitInstructionSimd(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
