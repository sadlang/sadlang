/*
 * ============================================================================
 * StringOpsCodeGen — توليد LLVM IR لعمليات النصوص
 * Phase 7 Step 6 — 17 methods من 3 ملفات
 * ============================================================================
 */
#ifndef SAD_LLVM_STRING_OPS_CODEGEN_H
#define SAD_LLVM_STRING_OPS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class StringOpsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit StringOpsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    StringOpsCodeGen(const StringOpsCodeGen &) = delete;
    StringOpsCodeGen &operator=(const StringOpsCodeGen &) = delete;

    // (AR) العمليات الأساسية
    llvm::Value *emitStringConcat(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitStringCharAt(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitStringCmp(std::shared_ptr<SIRInstruction>);

    // (AR) التحويلات
    llvm::Value *emitStringToI64(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitStringToF64(std::shared_ptr<SIRInstruction>);

    // (AR) builtins (ops.cpp)
    llvm::Value *emitBuiltinStringLength(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringToUpper(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringToLower(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringFind(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringReplace(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringSubstring(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringTrim(std::shared_ptr<SIRInstruction>);

    // (AR) builtins (ops2.cpp)
    llvm::Value *emitBuiltinStringSplit(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringJoin(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringStartsWith(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringEndsWith(std::shared_ptr<SIRInstruction>);
    llvm::Value *emitBuiltinStringContains(std::shared_ptr<SIRInstruction>);
};

}} // namespace Sad::LLVM
#endif
