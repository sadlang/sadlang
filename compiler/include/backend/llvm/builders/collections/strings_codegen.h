/*
 * ============================================================================
 * StringsCodeGen - LLVM IR for string ops + helpers
 * Phase 8 Step 8 - 5 methods (from 3 source files)
 * ============================================================================
 */
#ifndef SAD_LLVM_STRINGS_CODEGEN_H
#define SAD_LLVM_STRINGS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class StringsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit StringsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    StringsCodeGen(const StringsCodeGen &) = delete;
    StringsCodeGen &operator=(const StringsCodeGen &) = delete;

    llvm::Value *emitInlineAsm(std::shared_ptr<SIRInstruction> inst);
    void ensureArrayToStringHelper();
    llvm::Value *emitBuiltinTypeOf(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinArrayAppend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinArrayRemove(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
