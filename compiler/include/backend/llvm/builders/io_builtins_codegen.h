/*
 * ============================================================================
 * IOBuiltinsCodeGen - LLVM IR for IO built-ins (Print)
 * Phase 7 Step 15
 * ============================================================================
 */
#ifndef SAD_LLVM_IO_BUILTINS_CODEGEN_H
#define SAD_LLVM_IO_BUILTINS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class IOBuiltinsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit IOBuiltinsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    IOBuiltinsCodeGen(const IOBuiltinsCodeGen &) = delete;
    IOBuiltinsCodeGen &operator=(const IOBuiltinsCodeGen &) = delete;

    llvm::Value *emitBuiltinPrint(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
