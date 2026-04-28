/*
 * ============================================================================
 * InstrPlatformCodeGen - Phase 8 Step 10 - 1 method(s)
 * ============================================================================
 */
#ifndef SAD_LLVM_INSTRPLATFORM_CODEGEN_H
#define SAD_LLVM_INSTRPLATFORM_CODEGEN_H

#include <memory>
#include <string>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class InstrPlatformCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit InstrPlatformCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    InstrPlatformCodeGen(const InstrPlatformCodeGen &) = delete;
    InstrPlatformCodeGen &operator=(const InstrPlatformCodeGen &) = delete;

    llvm::Value *emitInstructionPlatform(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
