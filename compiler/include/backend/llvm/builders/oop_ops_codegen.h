/*
 * ============================================================================
 * OOPOpsCodeGen - LLVM IR for OOP/pointer operations
 * Phase 7 Step 18 - 6 methods (Addr, PtrAdd, PtrCast, ObjectNew, ObjectGet, ObjectSet)
 * ============================================================================
 */
#ifndef SAD_LLVM_OOP_OPS_CODEGEN_H
#define SAD_LLVM_OOP_OPS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class OOPOpsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit OOPOpsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    OOPOpsCodeGen(const OOPOpsCodeGen &) = delete;
    OOPOpsCodeGen &operator=(const OOPOpsCodeGen &) = delete;

    llvm::Value *emitAddr(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitPtrAdd(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitPtrCast(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitObjectNew(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitObjectGet(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitObjectSet(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
