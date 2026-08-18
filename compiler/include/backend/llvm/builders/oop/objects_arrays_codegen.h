/*
 * ============================================================================
 * ObjectsArraysCodeGen - LLVM IR for object/class operations
 * Phase 7 Step 17 - 7 methods
 * ============================================================================
 */
#ifndef SAD_LLVM_OBJECTS_ARRAYS_CODEGEN_H
#define SAD_LLVM_OBJECTS_ARRAYS_CODEGEN_H

#include <memory>
#include <string>
#include <vector>
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

private:
    /// (AR) سلسلةُ الأصنافِ المُنشَأةِ تعاوديًّا الآن — حارسُ دورةٍ يُعلِن اعتمادَنا
    ///      على رفضِ المحلّلِ للإحالةِ الأماميّة، ولا يستغني عنه.
    /// (EN) Classes currently being constructed recursively — a cycle guard that
    ///      declares (not replaces) our reliance on the parser rejecting forward refs.
    std::vector<std::string> nestedCtorStack_;
};

}} // namespace Sad::LLVM
#endif
