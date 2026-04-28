/*
 * ============================================================================
 * ClassesVtablesCodeGen - LLVM IR for class lifecycle, vtables, virtual dispatch
 * Phase 8 Step 3 - 9 methods (module/preprocess/vtables/destructor)
 * ============================================================================
 */
#ifndef SAD_LLVM_CLASSES_VTABLES_CODEGEN_H
#define SAD_LLVM_CLASSES_VTABLES_CODEGEN_H

#include <memory>
#include <string>
#include <vector>
#include <llvm/IR/Value.h>
#include "sir_module.h"

namespace Sad { namespace LLVM {

class LLVMCodeGen;
using SIRModule = Compiler::SIR::SIRModule;

class ClassesVtablesCodeGen
{
    LLVMCodeGen &cg_;
public:
    explicit ClassesVtablesCodeGen(LLVMCodeGen &cg) : cg_(cg) {}
    ClassesVtablesCodeGen(const ClassesVtablesCodeGen &) = delete;
    ClassesVtablesCodeGen &operator=(const ClassesVtablesCodeGen &) = delete;

    void emitModule(std::shared_ptr<SIRModule> sirModule);
    void preprocessClasses(std::shared_ptr<SIRModule> sirModule);
    void emitGlobalVariables(std::shared_ptr<SIRModule> sirModule);
    void buildClassVtables(std::shared_ptr<SIRModule> sirModule);
    void patchClassVtables();
    llvm::Value *emitVirtualCall(llvm::Value *objPtr, const std::string &className, const std::string &methodName, const std::vector<llvm::Value *> &extraArgs);
    void storeVtablePtr(llvm::Value *objPtr, const std::string &className);
    void emitDestructorCall(llvm::Value *objPtr, const std::string &className);
    int getFieldStructIndex(const std::string &className, int userFieldIndex) const;
};

}} // namespace Sad::LLVM
#endif
