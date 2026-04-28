/*
 * ============================================================================
 * MathBuiltinsCodeGen — توليد LLVM IR لدوال الرياضيات المضمنة
 * Phase 7 Step 8 — 21 methods (Min/Max + Sqrt..Clamp + Random)
 * ============================================================================
 */
#ifndef SAD_LLVM_MATH_BUILTINS_CODEGEN_H
#define SAD_LLVM_MATH_BUILTINS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class MathBuiltinsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit MathBuiltinsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    MathBuiltinsCodeGen(const MathBuiltinsCodeGen &) = delete;
    MathBuiltinsCodeGen &operator=(const MathBuiltinsCodeGen &) = delete;

    llvm::Value *emitBuiltinMin(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinMax(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinSqrt(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinLog(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinSin(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinCos(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinTan(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinAbs(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinFloor(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinCeil(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinRound(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinPow(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinLog10(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinLog2(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinAsin(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinAcos(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinAtan(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinTrunc(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinFmod(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinClamp(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinRandom(std::shared_ptr<SIRInstruction>);
};

}} // namespace Sad::LLVM
#endif
