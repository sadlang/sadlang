/*
 * ============================================================================
 * EnumOpsCodeGen - LLVM IR generation for ADT enums (construct/tag/payload/...)
 * Phase 7 Step 12 - 5 methods
 * ============================================================================
 */
#ifndef SAD_LLVM_ENUM_OPS_CODEGEN_H
#define SAD_LLVM_ENUM_OPS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class EnumOpsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit EnumOpsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    EnumOpsCodeGen(const EnumOpsCodeGen &) = delete;
    EnumOpsCodeGen &operator=(const EnumOpsCodeGen &) = delete;

    llvm::Value *emitEnumConstruct(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitEnumGetTag(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitEnumGetPayload(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitEnumIsVariant(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitEnumFree(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
