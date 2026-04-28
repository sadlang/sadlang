/*
 * ============================================================================
 * FunctionsCodeGen - LLVM IR for function emission, optimization, verification
 * Phase 8 Step 4 - 12 methods (functions + functions2 + opt/verify)
 * ============================================================================
 */
#ifndef SAD_LLVM_FUNCTIONS_CODEGEN_H
#define SAD_LLVM_FUNCTIONS_CODEGEN_H

#include <memory>
#include <llvm/IR/Function.h>
#include "sir_module.h"
#include "llvm_optimizer.h" // sad::OptimizationLevel + sad::OptimizationStats

namespace Sad
{
    namespace LLVM
    {

        class LLVMCodeGen;
        using SIRModule = Compiler::SIR::SIRModule;
        using SIRFunction = Compiler::SIR::SIRFunction;

        class FunctionsCodeGen
        {
            LLVMCodeGen &cg_;

        public:
            explicit FunctionsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
            FunctionsCodeGen(const FunctionsCodeGen &) = delete;
            FunctionsCodeGen &operator=(const FunctionsCodeGen &) = delete;

            void emitConstants(std::shared_ptr<SIRModule> sirModule);
            void emitGlobalFunctions(std::shared_ptr<SIRModule> sirModule);
            void emitMainWrapper(std::shared_ptr<SIRModule> sirModule);
            const sad::OptimizationStats &getOptimizationStats() const;
            bool optimize();
            void printOptimizationStats() const;
            void setLTOMode(bool enable_full, bool enable_thin);
            void setOptimizationLevel(sad::OptimizationLevel level);
            bool verify() const;
            llvm::Function *emitFunction(std::shared_ptr<SIRFunction> sirFunc);
            void emitFunctionBody(std::shared_ptr<SIRFunction> sirFunc, llvm::Function *llvmFunc);
            llvm::Function *emitFunctionPrototype(std::shared_ptr<SIRFunction> sirFunc);
        };

    }
} // namespace Sad::LLVM
#endif
