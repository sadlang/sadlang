/*
 * ============================================================================
 * AggregateOpsCodeGen — توليد LLVM IR لعمليات التجميع
 * Phase 7 Step 4 — ExtractValue/InsertValue/ExtractElement/InsertElement/Select
 * ============================================================================
 */
#ifndef SAD_LLVM_AGGREGATE_OPS_CODEGEN_H
#define SAD_LLVM_AGGREGATE_OPS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class AggregateOpsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit AggregateOpsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    AggregateOpsCodeGen(const AggregateOpsCodeGen &) = delete;
    AggregateOpsCodeGen &operator=(const AggregateOpsCodeGen &) = delete;

    llvm::Value *emitExtractValue(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitInsertValue(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitExtractElement(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitInsertElement(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitSelect(std::shared_ptr<SIRInstruction>);
};

}} // namespace Sad::LLVM
#endif
