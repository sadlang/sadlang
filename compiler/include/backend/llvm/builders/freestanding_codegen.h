/*
 * ============================================================================
 * FreestandingCodeGen - LLVM IR for freestanding builtins (no-OS environment)
 * Phase 7 Step 16 - 19 methods
 * ============================================================================
 */
#ifndef SAD_LLVM_FREESTANDING_CODEGEN_H
#define SAD_LLVM_FREESTANDING_CODEGEN_H

#include <llvm/IR/Type.h>

namespace Sad { namespace LLVM {

class LLVMCodeGen;

class FreestandingCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit FreestandingCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    FreestandingCodeGen(const FreestandingCodeGen &) = delete;
    FreestandingCodeGen &operator=(const FreestandingCodeGen &) = delete;

    void emitFreestandingRuntime();
    void emitFreestandingMalloc(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingFree(llvm::Type *ptrTy, llvm::Type *voidTy);
    void emitFreestandingMemcpy(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingMemset(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingStrlen(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingStrcmp(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingStrcpy(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingStrcat(llvm::Type *ptrTy);
    void emitFreestandingRealloc(llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingCalloc(llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingPrintf(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingSprintf(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingPow(llvm::Type *dblTy);
    void emitFreestandingSerialPuts(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingSerialPutInt(llvm::Type *i8Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingItoa(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingFtoa(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
    void emitFreestandingXtoa(llvm::Type *i8Ty, llvm::Type *i32Ty, llvm::Type *i64Ty, llvm::Type *ptrTy);
};

}} // namespace Sad::LLVM
#endif
