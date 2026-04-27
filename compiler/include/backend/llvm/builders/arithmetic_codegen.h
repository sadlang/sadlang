/*
 * ============================================================================
 * ArithmeticCodeGen — توليد LLVM IR للحساب/الثنائيات/المقارنات/التحويلات
 * Phase 7 Step 1 — استخراج 25 method + helper من god-class LLVMCodeGen
 * ============================================================================
 */
#ifndef SAD_LLVM_ARITHMETIC_CODEGEN_H
#define SAD_LLVM_ARITHMETIC_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad
{
    namespace LLVM
    {

        class LLVMCodeGen;
        using SIRInstruction = Compiler::SIR::SIRInstruction;
        using SIROperand = Compiler::SIR::SIROperand;

        class ArithmeticCodeGen
        {
            LLVMCodeGen &cg_;

        public:
            explicit ArithmeticCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
            ArithmeticCodeGen(const ArithmeticCodeGen &) = delete;
            ArithmeticCodeGen &operator=(const ArithmeticCodeGen &) = delete;

            // العمليات الحسابية
            llvm::Value *emitAdd(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitSub(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitMul(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitDiv(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitMod(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitNeg(std::shared_ptr<SIRInstruction>);
            // الثنائية + الإزاحة
            llvm::Value *emitAnd(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitOr(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitXor(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitNot(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitShl(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitShr(std::shared_ptr<SIRInstruction>);
            // المقارنات (Eq/Ne/Lt/Le فقط — Gt/Ge في llvm_codegen_memory_control.cpp)
            llvm::Value *emitCmpEq(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitCmpNe(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitCmpLt(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitCmpLe(std::shared_ptr<SIRInstruction>);
            // (AR) Phase 7 Step 1.5: Gt/Ge منقولتان الآن
            llvm::Value *emitCmpGt(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitCmpGe(std::shared_ptr<SIRInstruction>);
            // التحويلات
            llvm::Value *emitCast(std::shared_ptr<SIRInstruction>);
            // (AR) Phase 7 Step 1.5: 6 LLVM casts منقولة من file_casts.cpp
            llvm::Value *emitBitCast(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitIntToPtr(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitPtrToInt(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitTrunc(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitZExt(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitSExt(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitI64ToF64(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitF64ToI64(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitI64ToBool(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitBoolToI64(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitI64ToString(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitF64ToString(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitBoolToString(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitArrayToString(std::shared_ptr<SIRInstruction>);
            // المساعد
            llvm::Value *resolveOperand(const SIROperand &operand);
        };

    }
} // namespace Sad::LLVM
#endif
