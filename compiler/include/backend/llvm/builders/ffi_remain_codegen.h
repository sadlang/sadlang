/*
 * ============================================================================
 * FFIRemainCodeGen - LLVM IR for misc FFI builtins (system/getenv/atoi/atof/snprintf)
 * Phase 7 Step 15
 * ============================================================================
 */
#ifndef SAD_LLVM_FFI_REMAIN_CODEGEN_H
#define SAD_LLVM_FFI_REMAIN_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class FFIRemainCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit FFIRemainCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    FFIRemainCodeGen(const FFIRemainCodeGen &) = delete;
    FFIRemainCodeGen &operator=(const FFIRemainCodeGen &) = delete;

    llvm::Value *emitFFISystem(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIGetenv(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIAtoi(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFIAtof(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFFISnprintf(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
