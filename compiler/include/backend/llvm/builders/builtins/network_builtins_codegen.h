/*
 * ============================================================================
 * NetworkBuiltinsCodeGen - LLVM IR for network builtin functions
 * Phase 8 Step 6 - 2 methods
 * ============================================================================
 */
#ifndef SAD_LLVM_NETWORK_BUILTINS_CODEGEN_H
#define SAD_LLVM_NETWORK_BUILTINS_CODEGEN_H

#include <memory>
#include <vector>
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class NetworkBuiltinsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit NetworkBuiltinsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    NetworkBuiltinsCodeGen(const NetworkBuiltinsCodeGen &) = delete;
    NetworkBuiltinsCodeGen &operator=(const NetworkBuiltinsCodeGen &) = delete;

    llvm::Value *emitNetworkBuiltin(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitNetworkCall(std::shared_ptr<SIRInstruction> inst, const char *cFuncName, llvm::Type *returnType, const std::vector<llvm::Type *> &paramTypes);
};

}} // namespace Sad::LLVM
#endif
