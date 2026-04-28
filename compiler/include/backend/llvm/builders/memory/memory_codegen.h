/*
 * ============================================================================
 * MemoryCodeGen — توليد LLVM IR لعمليات الذاكرة
 * Phase 7 Step 2 — Load/Store/Alloca/GEP/Move
 * ============================================================================
 */
#ifndef SAD_LLVM_MEMORY_CODEGEN_H
#define SAD_LLVM_MEMORY_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class MemoryCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit MemoryCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    MemoryCodeGen(const MemoryCodeGen &) = delete;
    MemoryCodeGen &operator=(const MemoryCodeGen &) = delete;

    llvm::Value *emitLoad(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitStore(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitAlloca(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitGEP(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitMove(std::shared_ptr<SIRInstruction>);
};

}} // namespace Sad::LLVM
#endif
