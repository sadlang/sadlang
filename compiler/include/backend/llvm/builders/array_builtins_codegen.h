/*
 * ============================================================================
 * ArrayBuiltinsCodeGen — توليد LLVM IR لدوال المصفوفات المضمنة
 * Phase 7 Step 7 — 8 methods (Size/IndexOf/Contains/Reverse/Sort/First/Last/Slice)
 * ============================================================================
 */
#ifndef SAD_LLVM_ARRAY_BUILTINS_CODEGEN_H
#define SAD_LLVM_ARRAY_BUILTINS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class ArrayBuiltinsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit ArrayBuiltinsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    ArrayBuiltinsCodeGen(const ArrayBuiltinsCodeGen &) = delete;
    ArrayBuiltinsCodeGen &operator=(const ArrayBuiltinsCodeGen &) = delete;

    llvm::Value *emitBuiltinArraySize(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinArrayIndexOf(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinArrayContains(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinArrayReverse(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinArraySort(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinArrayFirst(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinArrayLast(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinArraySlice(std::shared_ptr<SIRInstruction>);
};

}} // namespace Sad::LLVM
#endif
