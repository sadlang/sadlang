/*
 * ============================================================================
 * MapOpsCodeGen — توليد LLVM IR لعمليات الخرائط (HashMap)
 * Phase 7 Step 9 — 3 methods (emitCallMap entry + 2 helpers)
 * ============================================================================
 */
#ifndef SAD_LLVM_MAP_OPS_CODEGEN_H
#define SAD_LLVM_MAP_OPS_CODEGEN_H

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <llvm/IR/Value.h>
#include <llvm/IR/Function.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class MapOpsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit MapOpsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    MapOpsCodeGen(const MapOpsCodeGen &) = delete;
    MapOpsCodeGen &operator=(const MapOpsCodeGen &) = delete;

    std::optional<llvm::Value *> emitCallMap(const std::string &funcName,
                                             std::vector<llvm::Value *> &args,
                                             std::shared_ptr<SIRInstruction> inst);
    llvm::Function *getOrCreateMapFindSlot();
    llvm::Function *getOrCreateMapCollect();
};

}} // namespace Sad::LLVM
#endif
