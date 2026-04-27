/*
 * ============================================================================
 * ExceptionCodeGen — توليد LLVM IR لاستثناءات try/catch/throw + setjmp/longjmp
 * Phase 7 Step 10 — 2 methods (emitCallException + markSetjmpGlobalsVolatile)
 * ============================================================================
 */
#ifndef SAD_LLVM_EXCEPTION_CODEGEN_H
#define SAD_LLVM_EXCEPTION_CODEGEN_H

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class ExceptionCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit ExceptionCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    ExceptionCodeGen(const ExceptionCodeGen &) = delete;
    ExceptionCodeGen &operator=(const ExceptionCodeGen &) = delete;

    std::optional<llvm::Value *> emitCallException(const std::string &funcName,
                                                   std::vector<llvm::Value *> &args,
                                                   std::shared_ptr<SIRInstruction> inst);
    void markSetjmpGlobalsVolatile();
};

}} // namespace Sad::LLVM
#endif
