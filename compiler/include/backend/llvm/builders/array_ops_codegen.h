/*
 * ============================================================================
 * ArrayOpsCodeGen — توليد LLVM IR لعمليات المصفوفات والنصوص
 * Phase 7 Step 5
 * - normalizeArrayPtr / emitBoundsCheck (helpers داخلية)
 * - emitArrayNew / Get / Set / Len / Concat
 * - emitStringNew
 * ============================================================================
 */
#ifndef SAD_LLVM_ARRAY_OPS_CODEGEN_H
#define SAD_LLVM_ARRAY_OPS_CODEGEN_H

#include <memory>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad
{
    namespace LLVM
    {

        class LLVMCodeGen;
        using SIRInstruction = Compiler::SIR::SIRInstruction;

        class ArrayOpsCodeGen
        {
            LLVMCodeGen &cg_;

        public:
            explicit ArrayOpsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
            ArrayOpsCodeGen(const ArrayOpsCodeGen &) = delete;
            ArrayOpsCodeGen &operator=(const ArrayOpsCodeGen &) = delete;

            // (AR) دوال مساعدة داخلية
            llvm::Value *normalizeArrayPtr(llvm::Value *arrPtr, const char *label = "arr");
            llvm::Value *normalizeArrayIndex(llvm::Value *index, llvm::Value *arrPtr, const char *label = "idx");
            void emitBoundsCheck(llvm::Value *index, llvm::Value *arrPtr, const char *label = "bc");

            // (AR) عمليات المصفوفات
            llvm::Value *emitArrayNew(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitArrayGet(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitArraySet(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitArrayLen(std::shared_ptr<SIRInstruction>);
            llvm::Value *emitArrayConcat(std::shared_ptr<SIRInstruction>);

            // (AR) إنشاء النصوص
            llvm::Value *emitStringNew(std::shared_ptr<SIRInstruction>);
        };

    }
} // namespace Sad::LLVM
#endif
