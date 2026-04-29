/*
 * ============================================================================
 * OutputCodeGen - Phase 8 Step 10 - 5 method(s)
 * ============================================================================
 */
#ifndef SAD_LLVM_OUTPUT_CODEGEN_H
#define SAD_LLVM_OUTPUT_CODEGEN_H

#include <memory>
#include <string>
#include <llvm/IR/Value.h>
#include <llvm/IR/Module.h>
#include "sir_instruction.h"

namespace Sad
{
    namespace LLVM
    {

        class LLVMCodeGen;
        using SIRInstruction = Compiler::SIR::SIRInstruction;

        class OutputCodeGen
        {
            LLVMCodeGen &cg_;

        public:
            explicit OutputCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
            OutputCodeGen(const OutputCodeGen &) = delete;
            OutputCodeGen &operator=(const OutputCodeGen &) = delete;

            std::string emitToString() const;
            bool emitToFile(const std::string &filename) const;
            bool emitAssembly(const std::string &filename);
            bool emitAssembly(const std::string &filename, llvm::Module *module);
            bool emitObjectFile(const std::string &filename);
            bool emitObjectFile(const std::string &filename, llvm::Module *module);
            void dump() const;
        };

    }
} // namespace Sad::LLVM
#endif
