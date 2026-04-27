/*
 * ============================================================================
 * ControlFlowCodeGen — توليد LLVM IR لتدفق التحكم
 * Phase 7 Step 3 — Branch/CondBranch/Call/Return/Switch
 * ============================================================================
 */
#ifndef SAD_LLVM_CONTROLFLOW_CODEGEN_H
#define SAD_LLVM_CONTROLFLOW_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class ControlFlowCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit ControlFlowCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    ControlFlowCodeGen(const ControlFlowCodeGen &) = delete;
    ControlFlowCodeGen &operator=(const ControlFlowCodeGen &) = delete;

    llvm::Value *emitBranch(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitCondBranch(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitCall(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitReturn(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitSwitch(std::shared_ptr<SIRInstruction>);
};

}} // namespace Sad::LLVM
#endif
