/*
 * ============================================================================
 * FileCastsCodeGen - Phase 8 Step 10 - 8 method(s)
 * ============================================================================
 */
#ifndef SAD_LLVM_FILECASTS_CODEGEN_H
#define SAD_LLVM_FILECASTS_CODEGEN_H

#include <memory>
#include <string>
#include <llvm/IR/Value.h>
#include "sir_instruction.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRInstruction = Compiler::SIR::SIRInstruction;

class FileCastsCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit FileCastsCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    FileCastsCodeGen(const FileCastsCodeGen &) = delete;
    FileCastsCodeGen &operator=(const FileCastsCodeGen &) = delete;

    llvm::Value *emitBuiltinFileRead(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileWrite(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileWriteBytes(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileReadBytes(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileAppend(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileDelete(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileCopy(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileMove(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileCreateDir(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileListDir(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileIsDir(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileIsSymlink(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileRealPath(std::shared_ptr<SIRInstruction> inst);
    llvm::Value *emitBuiltinFileAbsPath(std::shared_ptr<SIRInstruction> inst);
};

}} // namespace Sad::LLVM
#endif
