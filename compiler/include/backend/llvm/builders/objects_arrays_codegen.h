/*
 * ============================================================================
 * ObjectsArraysCodeGen - LLVM IR for object/class operations
 * Phase 7 Step 17 - 7 methods
 * ============================================================================
 */
#ifndef SAD_LLVM_OBJECTS_ARRAYS_CODEGEN_H
#define SAD_LLVM_OBJECTS_ARRAYS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class ObjectsArraysCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit ObjectsArraysCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    ObjectsArraysCodeGen(const ObjectsArraysCodeGen &) = delete;
    ObjectsArraysCodeGen &operator=(const ObjectsArraysCodeGen &) = delete;

    llvm::Value *emitObjectCall(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitInstanceOf(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitObjectCast(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitClassDef(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitMethodDef(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitFieldDef(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitConstructorCall(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
